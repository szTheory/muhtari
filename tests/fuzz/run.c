/* The run fuzz target: bounded linear or explicitly selected retail images
   (run_cart.h), loaded with the fast boot into two instances and run for 2 frames
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
#include "../alloc.h"

/* Four lines. WSYNC can hold an instruction to the end of a line and DMA
   most of the next; the worst seen over 3,000 random cartridges run for 60
   frames each was 794 ticks. */
#define RUN_BOUND (4u * MUHTARI__LINE_TICKS)
#define RUN_FRAMES 2u
#define RUN_PICTURE (MUHTARI__WIDTH * MUHTARI__HEIGHT_NTSC)

/* libFuzzer's -max_len in tests.cmake is the cartridge's size. */
_Static_assert(RUN_CART_BYTES == 131200u, "-max_len=131200");

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size);

static void require(int cond)
{
    if (!cond) {
        abort();
    }
}

static muhtari *run_create(alloc_counts *counts)
{
    muhtari_config cfg = alloc_config(counts);
    muhtari *m = NULL;
    require(muhtari_create(&cfg, &m) == MUHTARI_STATUS_OK);
    return m;
}

static void run_traffic(muhtari *m)
{
    static const uint16_t neighbors[] = {0x7FFFu, 0x8001u, 0xFF7Fu, 0xFF90u};
    for (uint32_t value = 0u; value < 256u; value++) {
        uint16_t at = m->cart.info.board == MUHTARI_BOARD_ACTIVISION
                          ? (uint16_t)(0xFF80u + value % 16u)
                          : 0x8000u;
        muhtari__cart_write(m, at, (uint8_t)value);
        uint8_t bank = m->cart.bank;
        for (size_t i = 0u; i < sizeof neighbors / sizeof neighbors[0]; i++) {
            muhtari__cart_write(m, neighbors[i], (uint8_t)value);
            if (m->cart.info.board == MUHTARI_BOARD_ABSOLUTE ||
                m->cart.info.board == MUHTARI_BOARD_ACTIVISION)
                require(m->cart.bank == bank);
        }
        if (m->cart.info.board == MUHTARI_BOARD_FRACTALUS) {
            for (uint32_t page = 0u; page < 64u; page++) {
                muhtari__cart_write(m, (uint16_t)(0x4000u + page * 256u + value), (uint8_t)value);
            }
        }
        (void)muhtari__cart_read(m, at, 0);
        (void)muhtari__cart_read(m, at, 1);
        (void)muhtari__cart_peek(m, at);
        require(m->cart.bank == bank);
    }
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
    struct run_cart_info built = run_cart(data, size, cart);
    muhtari_cart_info info = {0};
    info.size = (uint32_t)sizeof info;
    require(built.bytes <= RUN_CART_BYTES &&
            muhtari_probe(cart, built.bytes, &info) == MUHTARI_STATUS_OK);
    require(info.board == built.board);

    muhtari_media media;
    memset(&media, 0, sizeof media);
    media.size = (uint32_t)sizeof media;
    media.image = cart;
    media.image_size = built.bytes;
    alloc_counts counts[2];
    muhtari *m[2] = {run_create(&counts[0]), run_create(&counts[1])};
    require(muhtari_load(m[0], &media) == MUHTARI_STATUS_OK);
    require(muhtari_load(m[1], &media) == MUHTARI_STATUS_OK);
    run_traffic(m[0]);
    run_traffic(m[1]);
    for (uint32_t f = 0; f < RUN_FRAMES; f++) {
        uint32_t count[2];
        muhtari_status st[2];
        for (uint32_t i = 0; i < 2u; i++) {
            st[i] = run_one(m[i], video + i * RUN_PICTURE, audio + i * MUHTARI__MAX_AUDIO_SAMPLES,
                            &count[i]);
        }
        require(st[0] == st[1] && count[0] == count[1]);
        require(memcmp(video, video + RUN_PICTURE, RUN_PICTURE) == 0);
        require(memcmp(audio, audio + MUHTARI__MAX_AUDIO_SAMPLES, count[0] * sizeof *audio) == 0);
    }
    for (uint32_t i = 0u; i < 2u; i++) {
        require(counts[i].allocs == 2u && counts[i].frees == 0u);
        require(muhtari_power_cycle(m[i]) == MUHTARI_STATUS_OK);
        require(counts[i].allocs == 2u && counts[i].frees == 0u);
    }
    muhtari_destroy(m[0]);
    muhtari_destroy(m[1]);
    free(cart);
    free(video);
    free(audio);
    return 0;
}

#include "replay_main.h"
