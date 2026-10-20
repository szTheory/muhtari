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

static const struct {
    const char *name;
    int (*run)(void);
} cases[] = {
    {"jam_ends", jam_ends},
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
