/* The BIOS boot through the shipped archive, one case per run:

     muhtari-test-boot <case> [BIOS ROM]

   BIOS is the OpenBIOS image built.openbios assembles and ROM the simple
   sample built.simple builds; neither is tracked. The cases that take them
   load them through muhtari_load, which boots through the BIOS, and run
   frames; the others build a small BIOS and cartridge in memory. K is
   the hand-off frame: the frame, counted from 1, of the first opcode fetch
   from cartridge space after INPTCTRL's EXT went from 0 to 1
   (docs/embedding.md, Booting with a BIOS). Exit status: 0 when the case
   passes, 1 when it fails, 2 on a usage error or an unknown case. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../alloc.h"
#include "../cart/images.h"
#include "../check.h"
#include "internal.h"

/* OpenBIOS shows its Fuji for DISPLAYTIME = 10 half-second ticks of 32
   frames, 320 frames, then hands off at the next VBLANK start
   (7800openbios.asm, DISPLAYTIME and LeaveLoader). The window allows its
   start-up and the hand-off's wait on either side. */
#define K_MIN 300u
#define K_MAX 340u

/* The frames a case runs at most while waiting for the hand-off. */
#define HANDOFF_LIMIT 400u

static alloc_counts counts;
static uint8_t video[320u * 292u];
static int16_t audio[2u * MUHTARI__MAX_AUDIO_SAMPLES];
static uint8_t bios[MUHTARI__BIOS_SIZE_MAX + 1u];
static size_t bios_n;
static uint8_t *rom;
static size_t rom_n;
static uint32_t console_region;

/* Whether OpenBIOS's hand-off frame K is in the window [300, 340]. */
static int k_in_window(uint64_t k)
{
    return k >= K_MIN && k <= K_MAX;
}

/* Reads at most cap bytes of path into buf; returns the count, or 0 after
   a message when the file cannot be read or is empty. */
static size_t read_file(const char *path, uint8_t *buf, size_t cap)
{
    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        fprintf(stderr, "%s: cannot open\n", path);
        return 0u;
    }
    size_t n = fread(buf, 1u, cap, f);
    int bad = ferror(f) != 0;
    (void)fclose(f);
    if (bad || n == 0u) {
        fprintf(stderr, "%s: cannot read\n", path);
        return 0u;
    }
    return n;
}

/* A new instance with img loaded through the BIOS in bios. NULL after a
   message on failure. */
static muhtari *boot_bios(const uint8_t *img, size_t n)
{
    muhtari_config cfg = alloc_config(&counts);
    muhtari *m = NULL;
    if (muhtari_create(&cfg, &m) != MUHTARI_STATUS_OK) {
        fprintf(stderr, "muhtari_create failed\n");
        return NULL;
    }
    muhtari_media media;
    memset(&media, 0, sizeof media);
    media.size = (uint32_t)sizeof media;
    media.image = img;
    media.image_size = n;
    media.bios = bios;
    media.bios_size = bios_n;
    media.region = console_region;
    muhtari_status st = muhtari_load(m, &media);
    if (st != MUHTARI_STATUS_OK) {
        fprintf(stderr, "muhtari_load failed with status %lu\n", (unsigned long)st);
        muhtari_destroy(m);
        return NULL;
    }
    return m;
}

/* Sets the console switches the next frame latches. */
static void set_switches(muhtari *m, uint32_t switches)
{
    muhtari_input in;
    memset(&in, 0, sizeof in);
    in.size = (uint32_t)sizeof in;
    in.switches = switches;
    CHECK_EQ_U64(muhtari_set_input(m, &in), MUHTARI_STATUS_OK);
}

/* Runs one frame; returns its status. */
static muhtari_status frame(muhtari *m)
{
    muhtari_frame io;
    memset(&io, 0, sizeof io);
    io.size = (uint32_t)sizeof io;
    io.video = video;
    io.video_pitch = MUHTARI__WIDTH;
    io.audio = audio;
    io.audio_capacity = (uint32_t)(sizeof audio / sizeof audio[0]);
    return muhtari_run_frame(m, &io);
}

/* Runs frames until the hand-off, at most HANDOFF_LIMIT; returns K, or 0
   when no hand-off came or a frame failed. */
static uint64_t run_to_handoff(muhtari *m)
{
    for (uint32_t f = 0u; f < HANDOFF_LIMIT && muhtari__boot_handoff_frame(m) == 0u; f++) {
        if (frame(m) != MUHTARI_STATUS_OK) {
            return 0u;
        }
    }
    return muhtari__boot_handoff_frame(m);
}

/* OpenBIOS boots simple: power-on puts the CPU through RES at the BIOS's
   reset vector with S = $FD and I set, the Fuji runs its 320 frames, and
   the first cartridge fetch comes with K in the window. */
static int handoff_simple(void)
{
    muhtari *m = boot_bios(rom, rom_n);
    if (m == NULL) {
        return 0;
    }
    uint16_t reset = (uint16_t)(bios[bios_n - 4u] | ((uint16_t)bios[bios_n - 3u] << 8));
    CHECK_EQ_HEX(m->cpu.pc, reset);
    CHECK_EQ_HEX(m->cpu.s, 0xFDu);
    CHECK((m->cpu.p & 0x04u) != 0u);
    CHECK_EQ_HEX(m->bus.inptctrl, 0x00u);
    CHECK_EQ_HEX(m->maria.reg[0x1Cu], 0x00u); /* CTRL: DMA off */
    CHECK_EQ_U64(m->frame_deadline - m->frame_start, 117174u);
    uint64_t k = run_to_handoff(m);
    printf("handoff_frame %llu\n", (unsigned long long)k);
    CHECK(k_in_window(k));
    muhtari_destroy(m);
    /* The same stripped OpenBIOS executes as PAL, independently of its
       firmware geometry and without demanding the fast-boot snapshot. */
    console_region = MUHTARI_REGION_PAL;
    m = boot_bios(rom, rom_n);
    CHECK(m != NULL);
    if (m != NULL) {
        CHECK_EQ_U64(m->frame_deadline, 139874u);
        CHECK_EQ_U64(m->bus.ticks, 42u);
        CHECK_EQ_U64(m->cpu.s, 0xFDu);
        CHECK(k_in_window(run_to_handoff(m)));
        muhtari_destroy(m);
    }
    console_region = 0u;
    return check_failures == 0;
}

/* The window's edges: 300 and 340 pass, one frame outside fails. */
static int k_window(void)
{
    CHECK(k_in_window(K_MIN));
    CHECK(k_in_window(K_MAX));
    CHECK(!k_in_window(K_MIN - 1u));
    CHECK(!k_in_window(K_MAX + 1u));
    return check_failures == 0;
}

/* A 4 KiB BIOS at $F000 and a 48 KiB linear cartridge at $4000, for the
   cases that need no OpenBIOS. The BIOS runs from RES with MEN = 0, waits
   in a loop, then writes INPTCTRL $06 (MEN and EXT), so its next fetch,
   at $F00E, is the cartridge's: there the cartridge jumps through its
   reset vector to $4000. That code counts its entries at $2000, waits,
   writes INPTCTRL $02, which maps the BIOS again, and jumps back into it
   at $F000, so EXT rises and a cartridge fetch follows again and again. */
#define CART_BODY (48u * IMAGES_KIB)
#define CART_BASE 0x4000u
#define ENTRIES 0x2000u
static uint8_t cart[IMAGES_HEADER + CART_BODY];

static void put(uint16_t addr, const uint8_t *code, size_t n)
{
    memcpy(cart + IMAGES_HEADER + (addr - CART_BASE), code, n);
}

static void build_reentry(void)
{
    static const uint8_t bios_code[] = {
        0xA0u, 0x20u, /* $F000 LDY #$20 */
        0xA2u, 0x00u, /* $F002 LDX #$00 */
        0xCAu,        /* $F004 DEX */
        0xD0u, 0xFDu, /* $F005 BNE $F004 */
        0x88u,        /* $F007 DEY */
        0xD0u, 0xF8u, /* $F008 BNE $F002 */
        0xA9u, 0x06u, /* $F00A LDA #$06 */
        0x85u, 0x01u, /* $F00C STA INPTCTRL */
    };
    static const uint8_t to_reset[] = {0x6Cu, 0xFCu, 0xFFu}; /* $F00E JMP ($FFFC) */
    static const uint8_t cart_code[] = {
        0xEEu, 0x00u, 0x20u, /* $4000 INC $2000 */
        0xA0u, 0x40u,        /* $4003 LDY #$40 */
        0xA2u, 0x00u,        /* $4005 LDX #$00 */
        0xCAu,               /* $4007 DEX */
        0xD0u, 0xFDu,        /* $4008 BNE $4007 */
        0x88u,               /* $400A DEY */
        0xD0u, 0xF8u,        /* $400B BNE $4005 */
        0xA9u, 0x02u,        /* $400D LDA #$02 */
        0x85u, 0x01u,        /* $400F STA INPTCTRL */
        0x4Cu, 0x00u, 0xF0u, /* $4011 JMP $F000 */
    };
    static const uint8_t vector[] = {(uint8_t)(CART_BASE & 0xFFu), (uint8_t)(CART_BASE >> 8)};
    memset(bios, 0, 4096u);
    memcpy(bios, bios_code, sizeof bios_code);
    bios[4092] = 0x00u; /* reset vector $F000 */
    bios[4093] = 0xF0u;
    bios_n = 4096u;
    memset(cart, 0, sizeof cart);
    images_header(cart, 3u, "Boot", CART_BODY, 0u);
    put(0xF00Eu, to_reset, sizeof to_reset);
    put(CART_BASE, cart_code, sizeof cart_code);
    put(0xFFFCu, vector, sizeof vector);
}

/* Only the first cartridge fetch after EXT rises is the hand-off: the
   later returns into the cartridge, each after EXT rises again, leave K
   where the first one put it. */
static int k_first_fetch(void)
{
    build_reentry();
    muhtari *m = boot_bios(cart, sizeof cart);
    if (m == NULL) {
        return 0;
    }
    uint64_t k = run_to_handoff(m);
    CHECK(k >= 2u); /* the BIOS's loop runs first */
    for (uint32_t f = 0u; f < 40u; f++) {
        CHECK_EQ_U64(frame(m), MUHTARI_STATUS_OK);
    }
    CHECK(m->ram[ENTRIES - MUHTARI__RAM_BASE] >= 3u);
    CHECK_EQ_U64(muhtari__boot_handoff_frame(m), k);
    muhtari_destroy(m);
    /* Owned firmware puts INPTCTRL's data tick at72 and the first real
       cartridge opcode fetch at76: seven slow RES cycles, LDA12, STA18.
       Four/16 KiB changes mapping only, in either console region. */
    for (unsigned region = 1u; region <= 2u; region++) {
        for (unsigned large = 0u; large < 2u; large++) {
            bios_n = large ? 16384u : 4096u;
            memset(bios, 0xEA, bios_n);
            bios[0] = 0xACu;
            static const uint8_t code[] = {0xA9u, 0x06u, 0x85u, 0x01u};
            memcpy(bios + bios_n - 4096u, code, sizeof code);
            bios[bios_n - 4u] = 0u;
            bios[bios_n - 3u] = 0xF0u;
            images_header(cart, 3u, "Boot", CART_BODY, 0u);
            memset(cart + 128u, 0xEA, CART_BODY);
            images_footer(cart + 128u, CART_BODY);
            console_region = region;
            m = boot_bios(cart, sizeof cart);
            CHECK(m != NULL);
            if (m == NULL)
                continue;
            CHECK_EQ_U64(m->bus.ticks, 42u);
            CHECK_EQ_U64(m->frame_start, 0u);
            CHECK_EQ_U64(m->frame_deadline, region == 2u ? 139874u : 117174u);
            CHECK_EQ_U64(m->cpu.pc, 0xF000u);
            CHECK_EQ_U64(m->boot.handoff_frame, 0u);
            muhtari__cpu_step(m);
            CHECK_EQ_U64(m->bus.ticks, 54u);
            muhtari__cpu_step(m);
            CHECK_EQ_U64(m->bus.ticks, 72u);
            CHECK_EQ_U64(m->boot.handoff_frame, 0u);
            CHECK_EQ_U64(m->cpu.pc, 0xF004u);
            /* Own the fetch itself to judge its actual data timestamp. */
            m->bus.sync = 1u;
            CHECK_EQ_U64(muhtari__bus_read(m, 0xF004u), 0xEAu);
            CHECK_EQ_U64(m->bus.ticks, 76u);
            CHECK_EQ_U64(m->boot.handoff_frame, 1u);
            CHECK_EQ_U64(muhtari_power_cycle(m), MUHTARI_STATUS_OK);
            muhtari_frame io = {0};
            io.size = (uint32_t)sizeof io;
            CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_OK);
            CHECK_EQ_U64(io.audio_count, region == 2u ? 946u : 785u);
            if (region == 2u)
                CHECK_EQ_U64(m->audio_rem, 269046u);
            /* Mapping is the whole image when MEN is set, not a region
               decision based on firmware size. */
            m->bus.inptctrl = 2u;
            CHECK_EQ_U64(muhtari__boot_bios_byte(m, 0xC000u), large ? 0xACu : 0xA9u);
            muhtari_destroy(m);

            bios[bios_n - 4096u + 1u] = 0xFDu;
            m = boot_bios(cart, sizeof cart);
            CHECK(m != NULL);
            if (m != NULL) {
                CHECK_EQ_U64(frame(m), MUHTARI_STATUS_ERR_BIOS_REJECTED_CART);
                uint64_t stopped = m->bus.ticks;
                CHECK_EQ_U64(frame(m), MUHTARI_STATUS_ERR_BIOS_REJECTED_CART);
                CHECK_EQ_U64(m->bus.ticks, stopped);
                muhtari_destroy(m);
            }
        }
    }
    console_region = 0u;
    return check_failures == 0;
}

/* Pause held skips OpenBIOS's Fuji. OpenBIOS first reads SWCHB in frame
   3, at the top of its main loop (7800openbios.asm, LeaveLoaderCheck,
   measured as frame 3 here), so Pause is held for frames 1-3 and released
   after; the hand-off follows within the same frame's VBLANK. */
#define SPLASH_HOLD 3u
static int splash_skip(void)
{
    muhtari *m = boot_bios(rom, rom_n);
    if (m == NULL) {
        return 0;
    }
    uint64_t k = 0u;
    for (uint32_t f = 1u; f <= 60u && k == 0u; f++) {
        set_switches(m, f <= SPLASH_HOLD ? (uint32_t)MUHTARI_SWITCH_PAUSE : 0u);
        CHECK_EQ_U64(frame(m), MUHTARI_STATUS_OK);
        k = muhtari__boot_handoff_frame(m);
    }
    printf("handoff_frame %llu\n", (unsigned long long)k);
    CHECK(k != 0u && k < 30u);
    muhtari_destroy(m);
    return check_failures == 0;
}

/* OpenBIOS's region check needs $FFF8's low bit set; with it clear the
   BIOS takes its 2600 path, whose loader ends by writing $FD to a TIA
   register, locking INPTCTRL with MEN = 0. That frame and every later one
   return ERR_BIOS_REJECTED_CART with no samples, until a power cycle or a
   load starts the machine again. */
static int rejects_cart(void)
{
    uint8_t *bad = (uint8_t *)malloc(rom_n);
    if (bad == NULL) {
        return 0;
    }
    memcpy(bad, rom, rom_n);
    bad[rom_n - 8u] = (uint8_t)(bad[rom_n - 8u] & 0xFEu); /* $FFF8 */
    muhtari *m = boot_bios(bad, rom_n);
    free(bad);
    if (m == NULL) {
        return 0;
    }
    muhtari_status st = MUHTARI_STATUS_OK;
    uint32_t f = 0u;
    while (f < 30u && st == MUHTARI_STATUS_OK) {
        st = frame(m);
        f++;
    }
    CHECK_EQ_U64(st, MUHTARI_STATUS_ERR_BIOS_REJECTED_CART);
    CHECK_EQ_U64(muhtari__boot_handoff_frame(m), 0u);
    muhtari_frame io;
    memset(&io, 0, sizeof io);
    io.size = (uint32_t)sizeof io;
    io.audio = audio;
    io.audio_capacity = (uint32_t)(sizeof audio / sizeof audio[0]);
    io.audio_count = 99u;
    muhtari *refused = malloc(sizeof *refused);
    CHECK(refused != NULL);
    if (refused == NULL) {
        muhtari_destroy(m);
        return 0;
    }
    memcpy(refused, m, sizeof *refused);
    for (size_t i = 0u; i < sizeof audio / sizeof audio[0]; i++) {
        audio[i] = 0x5A5A;
    }
    CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_ERR_BIOS_REJECTED_CART);
    CHECK_EQ_U64(io.audio_count, 0u);
    CHECK(memcmp(m, refused, sizeof *refused) == 0);
    free(refused);
    for (size_t i = 0u; i < sizeof audio / sizeof audio[0]; i++) {
        CHECK_EQ_U64(audio[i], 0x5A5A);
    }
    CHECK_EQ_U64(frame(m), MUHTARI_STATUS_ERR_BIOS_REJECTED_CART);

    /* A power cycle starts the BIOS again, which rejects the cartridge in
       the same frame as before. */
    CHECK_EQ_U64(muhtari_power_cycle(m), MUHTARI_STATUS_OK);
    for (uint32_t g = 1u; g < f; g++) {
        CHECK_EQ_U64(frame(m), MUHTARI_STATUS_OK);
    }
    CHECK_EQ_U64(frame(m), MUHTARI_STATUS_ERR_BIOS_REJECTED_CART);

    /* A load of the good image clears it, and the BIOS hands off. */
    muhtari_media media;
    memset(&media, 0, sizeof media);
    media.size = (uint32_t)sizeof media;
    media.image = rom;
    media.image_size = rom_n;
    media.bios = bios;
    media.bios_size = bios_n;
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    CHECK(k_in_window(run_to_handoff(m)));
    muhtari_destroy(m);
    return check_failures == 0;
}

int main(int argc, char **argv)
{
    static const struct {
        const char *name;
        int (*run)(void);
        int files; /* takes BIOS and ROM */
    } cases[] = {
        {"handoff_simple", handoff_simple, 1}, {"k_window", k_window, 0},
        {"k_first_fetch", k_first_fetch, 0},   {"splash_skip", splash_skip, 1},
        {"rejects_cart", rejects_cart, 1},
    };
    size_t i = 0;
    while (i < sizeof cases / sizeof cases[0] &&
           (argc < 2 || strcmp(argv[1], cases[i].name) != 0)) {
        i++;
    }
    if (i == sizeof cases / sizeof cases[0] || argc != (cases[i].files ? 4 : 2)) {
        fprintf(stderr, "usage: muhtari-test-boot <case> [BIOS ROM]\n");
        return 2;
    }
    if (cases[i].files) {
        bios_n = read_file(argv[2], bios, sizeof bios);
        rom = (uint8_t *)malloc((size_t)MUHTARI_IMAGE_SIZE_MAX + 1u);
        rom_n = rom == NULL ? 0u : read_file(argv[3], rom, (size_t)MUHTARI_IMAGE_SIZE_MAX + 1u);
        if (bios_n == 0u || rom_n < 8u) {
            free(rom);
            return 2;
        }
    }
    int ok = cases[i].run();
    free(rom);
    return ok ? 0 : 1;
}
