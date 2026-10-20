/* The BIOS boot through the shipped archive, one case per run:

     muhtari-test-boot <case> BIOS ROM

   BIOS is the OpenBIOS image built.openbios assembles and ROM the simple
   sample built.simple builds; neither is tracked. Each case loads them
   through muhtari_load, which boots through the BIOS, and runs frames. K is
   the hand-off frame: the frame, counted from 1, of the first opcode fetch
   from cartridge space after INPTCTRL's EXT went from 0 to 1
   (docs/embedding.md, Booting with a BIOS). Exit status: 0 when the case
   passes, 1 when it fails, 2 on a usage error or an unknown case. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../alloc.h"
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
static uint8_t video[MUHTARI__WIDTH * MUHTARI__HEIGHT_NTSC];
static int16_t audio[2u * MUHTARI__MAX_AUDIO_SAMPLES];
static uint8_t bios[MUHTARI__BIOS_SIZE_MAX + 1u];
static size_t bios_n;
static uint8_t *rom;
static size_t rom_n;

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

/* A new instance with img loaded through the BIOS read from the command
   line. NULL after a message on failure. */
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
    muhtari_status st = muhtari_load(m, &media);
    if (st != MUHTARI_STATUS_OK) {
        fprintf(stderr, "muhtari_load failed with status %lu\n", (unsigned long)st);
        muhtari_destroy(m);
        return NULL;
    }
    return m;
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
    return check_failures == 0;
}

int main(int argc, char **argv)
{
    static const struct {
        const char *name;
        int (*run)(void);
    } cases[] = {
        {"handoff_simple", handoff_simple},
    };
    if (argc != 4) {
        fprintf(stderr, "usage: muhtari-test-boot <case> BIOS ROM\n");
        return 2;
    }
    bios_n = read_file(argv[2], bios, sizeof bios);
    rom = (uint8_t *)malloc((size_t)MUHTARI_IMAGE_SIZE_MAX + 1u);
    rom_n = rom == NULL ? 0u : read_file(argv[3], rom, (size_t)MUHTARI_IMAGE_SIZE_MAX + 1u);
    if (bios_n == 0u || rom_n == 0u) {
        free(rom);
        return 2;
    }
    for (size_t i = 0; i < sizeof cases / sizeof cases[0]; i++) {
        if (strcmp(argv[1], cases[i].name) == 0) {
            int ok = cases[i].run();
            free(rom);
            return ok ? 0 : 1;
        }
    }
    fprintf(stderr, "unknown case '%s'\n", argv[1]);
    free(rom);
    return 2;
}
