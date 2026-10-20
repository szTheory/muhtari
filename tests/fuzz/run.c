/* The run fuzz target: any bytes, made a headerless 48 KiB linear cartridge
   (run_cart.h: truncated or padded with $00 to 49,152 bytes, footer made to
   pass), loaded with the fast boot into two instances and run for 2 frames
   in each, call by call. Any broken invariant calls abort(), which libFuzzer
   and the replay both report:
   1. both loads return the same status;
   2. each frame returns the same status, audio count and picture in both;
   3. each frame of a loaded cartridge returns within its bound: at the first
      instruction boundary at or after its deadline, and less than RUN_BOUND
      ticks past it, so no program can hold a call (docs/hardware/bus.md,
      The run loop).
   Sanitizer findings and libFuzzer's -timeout catch the rest. */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"
#include "muhtari.h"
#include "run_cart.h"

/* Four lines. WSYNC can hold an instruction to the end of a line and DMA
   most of the next; the worst seen over 3,000 random cartridges run for 60
   frames each was 794 ticks. */
#define RUN_BOUND (4u * MUHTARI__LINE_TICKS)
#define RUN_FRAMES 2u
#define RUN_PICTURE (MUHTARI__WIDTH * MUHTARI__HEIGHT_NTSC)

/* libFuzzer's -max_len in tests.cmake is the cartridge's size. */
_Static_assert(RUN_CART_BYTES == 49152u, "-max_len=49152");

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size);

static void require(int cond)
{
    if (!cond) {
        abort();
    }
}

static void *run_alloc(void *user, size_t size)
{
    (void)user;
    return malloc(size);
}

static void run_free(void *user, void *ptr, size_t size)
{
    (void)user;
    (void)size;
    free(ptr);
}

static muhtari *run_create(void)
{
    muhtari_config cfg;
    muhtari *m = NULL;
    memset(&cfg, 0, sizeof cfg);
    cfg.size = (uint32_t)sizeof cfg;
    cfg.allocator.alloc = run_alloc;
    cfg.allocator.free = run_free;
    require(muhtari_create(&cfg, &m) == MUHTARI_STATUS_OK);
    return m;
}

/* Runs one frame into video and audio and checks its bound. */
static muhtari_status run_one(muhtari *m, uint8_t *video, int16_t *audio, uint32_t *count)
{
    muhtari_frame io;
    memset(&io, 0, sizeof io);
    io.size = (uint32_t)sizeof io;
    io.video_pitch = MUHTARI__WIDTH;
    io.video = video;
    io.audio = audio;
    io.audio_capacity = MUHTARI__MAX_AUDIO_SAMPLES;
    uint8_t loaded = m->loaded;
    uint64_t deadline = m->frame_deadline;
    muhtari_status st = muhtari_run_frame(m, &io);
    if (loaded && st == MUHTARI_STATUS_OK) {
        require(m->bus.ticks >= deadline && m->bus.ticks - deadline < RUN_BOUND);
    }
    *count = io.audio_count;
    return st;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    uint8_t *cart = (uint8_t *)malloc(RUN_CART_BYTES);
    uint8_t *video = (uint8_t *)malloc(2u * RUN_PICTURE);
    int16_t *audio = (int16_t *)malloc(2u * MUHTARI__MAX_AUDIO_SAMPLES * sizeof *audio);
    require(cart != NULL && video != NULL && audio != NULL);
    run_cart(data, size, cart);

    muhtari_media media;
    memset(&media, 0, sizeof media);
    media.size = (uint32_t)sizeof media;
    media.image = cart;
    media.image_size = RUN_CART_BYTES;
    muhtari *m[2] = {run_create(), run_create()};
    require(muhtari_load(m[0], &media) == muhtari_load(m[1], &media));
    for (uint32_t f = 0; f < RUN_FRAMES; f++) {
        uint32_t count[2];
        muhtari_status st[2];
        for (uint32_t i = 0; i < 2u; i++) {
            st[i] = run_one(m[i], video + i * RUN_PICTURE, audio + i * MUHTARI__MAX_AUDIO_SAMPLES,
                            &count[i]);
        }
        require(st[0] == st[1] && count[0] == count[1]);
        require(memcmp(video, video + RUN_PICTURE, RUN_PICTURE) == 0);
    }
    muhtari_destroy(m[0]);
    muhtari_destroy(m[1]);
    free(cart);
    free(video);
    free(audio);
    return 0;
}

#include "replay_main.h"
