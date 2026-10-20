/* The run loop through the shipped archive, one case per run:

     muhtari-test-frame <case>

   Each case builds a 48 KiB linear cartridge in memory from a few bytes of
   its own 6502 code, so no image is tracked, loads it through the internal
   load and the fast-boot hand-off, and runs frames. Expected ticks come from
   the beam in docs/hardware/bus.md, The run loop: a line is 454 ticks, the
   hand-off is line 240, t 24, and each frame ends at VBLANK start, line
   258, t 42. Exit status: 0 when the case passes, 1 when it fails, 2 on a
   usage error or an unknown case. */
#include <stdio.h>
#include <string.h>

#include "../alloc.h"
#include "../cart/images.h"
#include "../check.h"
#include "internal.h"

#define LINE 454u
#define VBLANK_START (258u * LINE + 42u)
#define FRAME 119402u

/* A 48 KiB linear body maps $4000-$FFFF; every program starts at $4000. */
#define BODY (48u * IMAGES_KIB)
#define ROM_BASE 0x4000u

static alloc_counts counts;
static uint8_t image[IMAGES_HEADER + BODY];
static uint8_t video[MUHTARI__WIDTH * MUHTARI__HEIGHT_NTSC];
static int16_t audio[2u * MUHTARI__MAX_AUDIO_SAMPLES];

/* An instance running code from $4000 after the fast-boot hand-off: the
   body is zero but for the code and the reset vector. NULL on failure. */
static muhtari *boot(const uint8_t *code, size_t n)
{
    muhtari_config cfg = alloc_config(&counts);
    muhtari *m = NULL;
    memset(image, 0, sizeof image);
    images_header(image, 3u, "Frame", BODY, 0u);
    memcpy(image + IMAGES_HEADER, code, n);
    image[IMAGES_HEADER + 0xFFFCu - ROM_BASE] = (uint8_t)(ROM_BASE & 0xFFu);
    image[IMAGES_HEADER + 0xFFFDu - ROM_BASE] = (uint8_t)(ROM_BASE >> 8);
    if (muhtari_create(&cfg, &m) != MUHTARI_STATUS_OK) {
        fprintf(stderr, "muhtari_create failed\n");
        return NULL;
    }
    if (muhtari__cart_load(m, image, sizeof image) != MUHTARI_STATUS_OK) {
        fprintf(stderr, "muhtari__cart_load failed\n");
        muhtari_destroy(m);
        return NULL;
    }
    muhtari__boot_fast(m);
    return m;
}

/* Runs one frame into the case's buffers; returns its status. */
static muhtari_status frame(muhtari *m, uint32_t *samples)
{
    muhtari_frame io;
    memset(&io, 0, sizeof io);
    io.size = (uint32_t)sizeof io;
    io.video = video;
    io.video_pitch = MUHTARI__WIDTH;
    io.audio = audio;
    io.audio_capacity = (uint32_t)(sizeof audio / sizeof audio[0]);
    muhtari_status st = muhtari_run_frame(m, &io);
    *samples = io.audio_count;
    return st;
}

/* A JAM at the reset vector: the CPU jams on its first instruction, then
   each step reads $FFFF, a fast 4-tick cycle, and the frame still ends.
   Frame k ends at the first step boundary at or after VBLANK start of frame
   k, so 0 to 3 ticks past it; frame 1 is the hand-off's 8,190 ticks. */
static int jam_ends(void)
{
    static const uint8_t code[] = {0x02u}; /* $4000 JAM */
    muhtari *m = boot(code, sizeof code);
    if (m == NULL) {
        return 0;
    }
    for (uint32_t k = 0u; k < 3u; k++) {
        uint32_t samples = 0u;
        uint64_t deadline = VBLANK_START + (uint64_t)k * FRAME;
        CHECK_EQ_U64(frame(m, &samples), MUHTARI_STATUS_OK);
        CHECK(m->cpu.jammed != 0u);
        CHECK(m->bus.ticks >= deadline);
        CHECK(m->bus.ticks < deadline + 4u);
        if (k == 0u) {
            CHECK_EQ_U64(samples, 54u); /* 8,190 x 88 / 13,125 */
        } else {
            CHECK(samples == 800u || samples == 801u);
        }
    }
    muhtari_destroy(m);
    return check_failures == 0;
}

/* MARIA's registers, as bus addresses, and CTRL's DMA modes: bits 6-5 are
   10 for DMA on; $60 and $7F have them 11, DMA off (A7800-MARIA section
   2.4 and 3). */
#define WSYNC_TIA 0x0002u
#define WSYNC_MARIA 0x0024u
#define MSTAT 0x0028u
#define CTRL 0x003Cu
#define CTRL_DMA_ON 0x40u
#define CTRL_DMA_OFF 0x7Fu

/* A RAM byte that stays 0, so reading it leaves $00 on the bus. */
#define ZERO_BYTE 0x2000u
/* A TIA address that drives nothing: a 6-tick read of the open-bus byte. */
#define TIA_UNDRIVEN 0x000Eu

/* Where the DMA cases put their display: the DLL at DPP's hand-off value
   $1F84, 16 zones of 16 lines (OFFSET 15), each pointing at the DL at
   $2200, which RAM's zero fill makes an empty list (A7800-MARIA section
   2.1). */
#define DLL_AT 0x1F84u
#define DL_AT 0x2200u
#define ZONES 16u

/* Frame 2 starts at line 0 one frame after the hand-off frame's origin. */
#define FRAME2 FRAME

/* Locks INPTCTRL at $07, as a game does, so that later writes in the TIA's
   range, WSYNC's among them, leave MEN set (docs/hardware/bus.md,
   INPTCTRL). */
static void lock(muhtari *m)
{
    muhtari__bus_write(m, 0x0001u, 0x07u);
}

static void poke(muhtari *m, uint16_t addr, uint8_t v)
{
    m->ram[addr - MUHTARI__RAM_BASE] = v;
}

/* Locks INPTCTRL, writes the 16-zone DLL and turns DMA on with a CPU write
   to CTRL. */
static void display_on(muhtari *m)
{
    lock(m);
    for (uint16_t z = 0u; z < ZONES; z++) {
        poke(m, (uint16_t)(DLL_AT + 3u * z), 0x0Fu);
        poke(m, (uint16_t)(DLL_AT + 3u * z + 1u), (uint8_t)(DL_AT >> 8));
        poke(m, (uint16_t)(DLL_AT + 3u * z + 2u), (uint8_t)(DL_AT & 0xFFu));
    }
    muhtari__bus_write(m, CTRL, CTRL_DMA_ON);
}

/* Acting as the CPU: 4-tick reads of a zero RAM byte until the tick is at
   or past at. With DMA on a burst's hold can carry it a little further. */
static void read_until(muhtari *m, uint64_t at)
{
    while (m->bus.ticks < at) {
        (void)muhtari__bus_read(m, ZERO_BYTE);
    }
}

/* Acting as the CPU with DMA off: brings the tick to exactly at, which must
   be even and at least 6 ahead, with one 6-tick read of an undriven TIA
   address when the distance is 2 mod 4, then 4-tick reads of a zero byte,
   so the bus ends holding $00. */
static void seek(muhtari *m, uint64_t at)
{
    if ((at - m->bus.ticks) % 4u == 2u) {
        (void)muhtari__bus_read(m, TIA_UNDRIVEN);
    }
    read_until(m, at);
    CHECK_EQ_U64(m->bus.ticks, at);
}

/* The TIA line offset that puts a WSYNC's release at tick at: the TIA's
   line wraps where at + tia_phase is a multiple of 456 (docs/hardware/bus.md,
   RDY). */
static uint16_t tia_phase_for(uint64_t at)
{
    return (uint16_t)((456u - at % 456u) % 456u);
}

/* The first 4-tick cycle start at or after release, from tick from, plus
   the 4-tick read that runs there (docs/hardware/bus.md, RDY). */
static uint64_t held_read_end(uint64_t from, uint64_t release)
{
    return from + (release - from + 3u) / 4u * 4u + 4u;
}

/* CTRL $7F has DMA off. Every line's t 28 still comes from the tick count,
   so each frame ends at its deadline, within the 12 ticks of the JMP that
   crosses it, and every DMA line, 17 to 258, says DMA off. */
static int dma_off_ends(void)
{
    /* clang-format off */
    static const uint8_t code[] = {
        0xA9u, CTRL_DMA_OFF, /* $4000 LDA #$7F */
        0x85u, 0x3Cu,        /* $4002 STA CTRL */
        0x4Cu, 0x04u, 0x40u, /* $4004 JMP $4004 */
    };
    /* clang-format on */
    muhtari *m = boot(code, sizeof code);
    if (m == NULL) {
        return 0;
    }
    for (uint32_t k = 0u; k < 3u; k++) {
        uint32_t samples = 0u;
        uint64_t deadline = VBLANK_START + (uint64_t)k * FRAME;
        memset(m->maria.stats, 0, sizeof m->maria.stats);
        CHECK_EQ_U64(frame(m, &samples), MUHTARI_STATUS_OK);
        CHECK(m->bus.ticks >= deadline);
        CHECK(m->bus.ticks < deadline + 12u);
        if (k > 0u) {
            for (uint32_t line = 17u; line <= 258u; line++) {
                CHECK_EQ_HEX(m->maria.stats[line].flags, MUHTARI__LINE_DMA_OFF);
            }
        }
    }
    muhtari_destroy(m);
    return check_failures == 0;
}

/* STA WSYNC near t 0 of line 100 of frame 2, with tia_phase putting the
   TIA's release at t 100: the held read spans t 28, and the line's burst
   still runs, an empty DL on an ordinary line, so cost T0 + 9 - T0 = 9
   (docs/hardware/maria.md, DMA cost). The same at line 258, with the
   release at t 100, spans t 42, and MSTAT reads VBLANK set afterwards. */
static int hold_split_wsync_across_dma(void)
{
    static const uint8_t code[] = {0x02u}; /* $4000 JAM, never run */
    muhtari *m = boot(code, sizeof code);
    if (m == NULL) {
        return 0;
    }
    display_on(m);
    static const uint32_t lines[2] = {100u, 258u};
    for (uint32_t i = 0u; i < 2u; i++) {
        uint64_t line_start = FRAME2 + (uint64_t)lines[i] * LINE;
        uint64_t release = line_start + 100u;
        read_until(m, line_start);
        CHECK(m->bus.ticks < line_start + 20u);
        memset(&m->maria.stats[lines[i]], 0, sizeof m->maria.stats[0]);
        m->profile.tia_phase = tia_phase_for(release);
        muhtari__bus_write(m, WSYNC_TIA, 0x00u);
        (void)muhtari__bus_read(m, ZERO_BYTE);
        CHECK(m->bus.ticks >= release + 4u);
        CHECK_EQ_U64(m->maria.stats[lines[i]].cost, 9u);
        CHECK_EQ_HEX(m->maria.stats[lines[i]].flags &
                         (MUHTARI__LINE_DMA_OFF | MUHTARI__LINE_DMA_BLOCKED),
                     0u);
    }
    CHECK_EQ_HEX(muhtari__bus_read(m, MSTAT), 0x80u);
    muhtari_destroy(m);
    return check_failures == 0;
}

/* Reads up to the cycle that fires line's t 28 request (its data tick at or
   after t 28), runs it and c0, then makes the cycle-c1 access, a write of
   v to addr, when at_c2 is 0; with at_c2 1 it makes one more read first,
   so the write is c2's. Then one read for the cycle after (docs/hardware/
   bus.md, HALT). */
static void write_in_window(muhtari *m, uint32_t line, uint16_t addr, uint8_t v, int at_c2)
{
    uint64_t line_start = FRAME2 + (uint64_t)line * LINE;
    read_until(m, line_start);
    while (m->bus.ticks + 4u < line_start + 28u) {
        (void)muhtari__bus_read(m, ZERO_BYTE);
    }
    (void)muhtari__bus_read(m, ZERO_BYTE); /* fires t 28: the request */
    (void)muhtari__bus_read(m, ZERO_BYTE); /* c0, from the /HALT fall */
    if (at_c2) {
        (void)muhtari__bus_read(m, ZERO_BYTE); /* c1 */
    }
    muhtari__bus_write(m, addr, v);
    (void)muhtari__bus_read(m, ZERO_BYTE);
}

/* The burst runs at c2 with T0 = fall + 6, so a write into the DL's second
   header byte in c1 is fetched by the line's DMA: width field 31 makes a
   4-byte header of one graphics byte, one header read whole. The same
   write in c2 lands after the burst, so that line still sees the empty DL
   (docs/hardware/maria.md, The HALT handshake). */
static int halt_c1_write_visible(void)
{
    static const uint8_t code[] = {0x02u}; /* $4000 JAM, never run */
    muhtari *m = boot(code, sizeof code);
    if (m == NULL) {
        return 0;
    }
    display_on(m);
    write_in_window(m, 101u, DL_AT + 1u, 0x1Fu, 0);
    CHECK_EQ_U64(m->maria.stats[101].headers, 1u);
    CHECK_EQ_U64(m->maria.stats[101].cost, 20u); /* 9 + header 8 + graphics 3 */
    poke(m, DL_AT + 1u, 0x00u);
    write_in_window(m, 103u, DL_AT + 1u, 0x1Fu, 1);
    CHECK_EQ_U64(m->maria.stats[103].headers, 0u);
    CHECK_EQ_U64(m->maria.stats[103].cost, 9u);
    muhtari_destroy(m);
    return check_failures == 0;
}

/* A write to $24 holds the next read until t 0 of the next line: it runs
   in the first 4-tick cycle that starts at or after it. Wire-ANDed with
   the TIA's WSYNC, the later release wins: written at t 400, the TIA's
   release at t 100 of the next line is not cut short by MARIA's at t 0.
   The TIA releases within one 456-tick line of the write, so the target is
   less than that ahead. DMA is off, so no burst adds to the hold. */
static int maria_wsync_releases_at_lrc(void)
{
    static const uint8_t code[] = {0x02u}; /* $4000 JAM, never run */
    muhtari *m = boot(code, sizeof code);
    if (m == NULL) {
        return 0;
    }
    lock(m);
    seek(m, FRAME2 + 101u * LINE + 10u);
    muhtari__bus_write(m, WSYNC_MARIA, 0x00u);
    uint64_t from = m->bus.ticks;
    (void)muhtari__bus_read(m, ZERO_BYTE);
    CHECK_EQ_U64(m->bus.ticks, held_read_end(from, FRAME2 + 102u * LINE));

    uint64_t tia_release = FRAME2 + 103u * LINE + 100u;
    seek(m, FRAME2 + 102u * LINE + 400u);
    m->profile.tia_phase = tia_phase_for(tia_release);
    muhtari__bus_write(m, WSYNC_TIA, 0x00u);
    muhtari__bus_write(m, WSYNC_MARIA, 0x00u);
    from = m->bus.ticks;
    (void)muhtari__bus_read(m, ZERO_BYTE);
    CHECK_EQ_U64(m->bus.ticks, held_read_end(from, tia_release));
    muhtari_destroy(m);
    return check_failures == 0;
}

/* MSTAT drives D7 only, VBLANK, and the rest is the open-bus byte, here
   $00. It sets at line 258, t 42 and clears at line 16, t 42: a read whose
   data tick is t 40 sees the old value, one whose data tick is t 42 or t 44
   the new one (A7800-MARIA section 4.1). */
static int mstat_edges(void)
{
    static const uint8_t code[] = {0x02u}; /* $4000 JAM, never run */
    static const struct {
        uint32_t line, data_t;
        uint8_t mstat;
    } reads[] = {
        {100u, 40u, 0x00u}, {258u, 40u, 0x00u}, {258u, 44u, 0x80u},
        {16u, 40u, 0x80u},  {16u, 44u, 0x00u},
    };
    muhtari *m = boot(code, sizeof code);
    if (m == NULL) {
        return 0;
    }
    for (size_t i = 0u; i < sizeof reads / sizeof reads[0]; i++) {
        uint64_t frame_at = reads[i].line == 16u ? 2u * FRAME : FRAME;
        seek(m, frame_at + (uint64_t)reads[i].line * LINE + reads[i].data_t - 4u);
        CHECK_EQ_HEX(muhtari__bus_read(m, MSTAT), reads[i].mstat);
    }
    muhtari_destroy(m);
    m = boot(code, sizeof code);
    if (m == NULL) {
        return 0;
    }
    seek(m, FRAME + 258u * LINE + 42u - 4u);
    CHECK_EQ_HEX(muhtari__bus_read(m, MSTAT), 0x80u);
    muhtari_destroy(m);
    return check_failures == 0;
}

/* The fast-boot hand-off (A7800-SYSTEM section 7, C10): PC from $FFFC,
   S = X = $16, A = Y = 0, I and D set; INPTCTRL $16 stored as $06 with the
   gate's two writes counted; CTRL $60, DPP $1F84; the DL at $2200, the next
   DLL entry at $1FF3, OFFSET 2; and the beam at line 240, t 24, on a tick
   that is a multiple of 4. */
static int handoff_state(void)
{
    static const uint8_t code[] = {0x02u};
    muhtari *m = boot(code, sizeof code);
    if (m == NULL) {
        return 0;
    }
    uint64_t beam = m->bus.ticks - m->maria.origin;
    CHECK_EQ_HEX(m->cpu.pc, ROM_BASE);
    CHECK_EQ_HEX(m->cpu.s, 0x16u);
    CHECK_EQ_HEX(m->cpu.x, 0x16u);
    CHECK_EQ_HEX(m->cpu.a, 0x00u);
    CHECK_EQ_HEX(m->cpu.y, 0x00u);
    CHECK_EQ_HEX(m->cpu.p & 0xDFu, 0x0Cu);
    CHECK_EQ_HEX(m->bus.inptctrl, 0x06u);
    CHECK_EQ_U64(m->bus.halt_gate_writes, 2u);
    CHECK_EQ_HEX(m->maria.reg[0x1Cu], 0x60u);
    CHECK_EQ_HEX(m->maria.reg[0x0Cu], 0x1Fu);
    CHECK_EQ_HEX(m->maria.reg[0x10u], 0x84u);
    CHECK_EQ_HEX(m->maria.dl, 0x2200u);
    CHECK_EQ_HEX(m->maria.dll, 0x1FF3u);
    CHECK_EQ_U64(m->maria.offset, 2u);
    CHECK_EQ_U64(beam / LINE, 240u);
    CHECK_EQ_U64(beam % LINE, 24u);
    CHECK_EQ_U64(m->bus.ticks % 4u, 0u);
    CHECK_EQ_U64(VBLANK_START - beam, 8190u);
    muhtari_destroy(m);
    return check_failures == 0;
}

/* Frame 1 after the hand-off is the 8,190 ticks to VBLANK start: 54
   samples (8,190 x 88 / 13,125 rounded down), and rows 0-223, which the
   beam has not scanned since the load, are $00. Frame 2 is a full frame. */
static int first_frame_partial(void)
{
    static const uint8_t code[] = {0x4Cu, 0x00u, 0x40u}; /* $4000 JMP $4000 */
    muhtari *m = boot(code, sizeof code);
    uint32_t samples = 0u;
    if (m == NULL) {
        return 0;
    }
    memset(video, 0xEE, sizeof video);
    CHECK_EQ_U64(frame(m, &samples), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(samples, 54u);
    for (size_t i = 0u; i < 224u * MUHTARI__WIDTH; i++) {
        if (video[i] != 0u) {
            CHECK_EQ_HEX(video[i], 0u);
            break;
        }
    }
    CHECK_EQ_U64(frame(m, &samples), MUHTARI_STATUS_OK);
    CHECK(samples == 800u || samples == 801u);
    muhtari_destroy(m);
    return check_failures == 0;
}

/* With the HALT gate shut, MARIA's request on each DMA line is refused: the
   line runs no burst, costs nothing and says DMA blocked, and the sticky
   feature mask records it. */
static int dma_blocked_flag(void)
{
    /* clang-format off */
    static const uint8_t code[] = {
        0xA9u, CTRL_DMA_ON,  /* $4000 LDA #$40 */
        0x85u, 0x3Cu,        /* $4002 STA CTRL */
        0x4Cu, 0x04u, 0x40u, /* $4004 JMP $4004 */
    };
    /* clang-format on */
    muhtari *m = boot(code, sizeof code);
    uint32_t samples = 0u;
    if (m == NULL) {
        return 0;
    }
    m->bus.halt_gate_writes = 0u;
    CHECK_EQ_U64(frame(m, &samples), MUHTARI_STATUS_OK);
    memset(m->maria.stats, 0, sizeof m->maria.stats);
    CHECK_EQ_U64(frame(m, &samples), MUHTARI_STATUS_OK);
    for (uint32_t line = 17u; line <= 258u; line++) {
        CHECK_EQ_HEX(m->maria.stats[line].flags, MUHTARI__LINE_DMA_BLOCKED);
        CHECK_EQ_U64(m->maria.stats[line].cost, 0u);
    }
    CHECK((m->maria.features & MUHTARI__FEAT_DMA_BLOCKED) != 0u);
    muhtari_destroy(m);
    return check_failures == 0;
}

static const struct {
    const char *name;
    int (*run)(void);
} cases[] = {
    {"jam_ends", jam_ends},
    {"dma_off_ends", dma_off_ends},
    {"hold_split_wsync_across_dma", hold_split_wsync_across_dma},
    {"halt_c1_write_visible", halt_c1_write_visible},
    {"maria_wsync_releases_at_lrc", maria_wsync_releases_at_lrc},
    {"mstat_edges", mstat_edges},
    {"handoff_state", handoff_state},
    {"first_frame_partial", first_frame_partial},
    {"dma_blocked_flag", dma_blocked_flag},
};

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: muhtari-test-frame <case>\n");
        return 2;
    }
    for (size_t i = 0u; i < sizeof cases / sizeof cases[0]; i++) {
        if (strcmp(argv[1], cases[i].name) == 0) {
            return cases[i].run() ? 0 : 1;
        }
    }
    fprintf(stderr, "muhtari-test-frame: unknown case %s\n", argv[1]);
    return 2;
}
