/* The loader fuzz target: muhtari_probe and the internal load on the same
   bytes. Any broken invariant calls abort(), which libFuzzer and the replay
   both report:
   1. two probes of the same bytes give the same status and the same info;
   2. the load returns probe's status;
   3. through a counting allocator, a refusal allocates nothing and leaves
      the instance byte-identical, and an accepted image makes exactly one
      allocation of ROM plus physical RAM holding independently normalized
      mapped bytes; with an
      allocator that fails every call after create, the status is the
      refusal's own, or ERR_NO_MEMORY for an accepted image, and the
      instance is unchanged;
   4. destroy frees everything allocated, and a second accepted load
      frees the first. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../alloc.h"
#include "internal.h"
#include "muhtari.h"
#include "run_cart.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size);

static struct muhtari before;

static void require_at(int cond, int line)
{
    if (!cond) {
        fprintf(stderr, "loader invariant failed at line %d\n", line);
        abort();
    }
}
#define require(cond) require_at((cond), __LINE__)

/* Independent 8 KiB half copies, rather than the production in-place swap. */
static void check_installed(const uint8_t *data, size_t size, const muhtari *m)
{
    const muhtari_cart_info *info = &m->cart.info;
    size_t at = (info->flags & MUHTARI_CART_HEADER) != 0u ? 128u : 0u;
    require(at + info->rom_size <= size);
    if ((info->flags & MUHTARI_CART_ACTIVISION_HALF_SWAPPED) != 0u) {
        require(info->board == MUHTARI_BOARD_ACTIVISION && info->rom_size == 131072u);
        for (uint32_t bank = 0u; bank < 8u; bank++) {
            require(memcmp(m->cart.rom + bank * 16384u, data + at + bank * 16384u + 8192u, 8192u) ==
                    0);
            require(memcmp(m->cart.rom + bank * 16384u + 8192u, data + at + bank * 16384u, 8192u) ==
                    0);
        }
    } else {
        require(memcmp(m->cart.rom, data + at, info->rom_size) == 0);
    }
    for (uint32_t i = 0u; i < info->ram_size; i++)
        require(m->cart.ram[i] == 0u);
}

static void warmed_failure(const uint8_t *data, size_t size, muhtari_status want, uint32_t board)
{
    uint8_t *cart = (uint8_t *)malloc(RUN_CART_BYTES);
    uint8_t *video = (uint8_t *)malloc(2u * MUHTARI__WIDTH * MUHTARI__HEIGHT_NTSC);
    require(cart != NULL && video != NULL);
    struct run_cart_info built = run_cart_control(cart, board);
    muhtari_cart_info info = {0};
    info.size = (uint32_t)sizeof info;
    require(muhtari_probe(cart, built.bytes, &info) == MUHTARI_STATUS_OK && info.board == board);
    alloc_counts counts[2];
    muhtari *m[2] = {NULL, NULL};
    for (uint32_t i = 0u; i < 2u; i++) {
        muhtari_config cfg = alloc_config(&counts[i]);
        require(muhtari_create(&cfg, &m[i]) == MUHTARI_STATUS_OK);
        muhtari_media media = {0};
        media.size = (uint32_t)sizeof media;
        media.image = cart;
        media.image_size = built.bytes;
        require(muhtari_load(m[i], &media) == MUHTARI_STATUS_OK);
        if (board == MUHTARI_BOARD_ACTIVISION)
            muhtari__cart_write(m[i], 0xFF85u, 0u);
        if (board == MUHTARI_BOARD_ABSOLUTE)
            muhtari__cart_write(m[i], 0x8000u, 2u);
        if (board == MUHTARI_BOARD_FRACTALUS) {
            for (uint32_t p = 0u; p < 2048u; p++)
                m[i]->cart.ram[p] = (uint8_t)(p * 29u + 7u);
        }
        muhtari_frame io = {0};
        io.size = (uint32_t)sizeof io;
        require(muhtari_run_frame(m[i], &io) == MUHTARI_STATUS_OK);
        require(m[i]->ram[0xA00u] != 0u);
    }
    size_t bytes = (size_t)m[0]->cart.info.rom_size + m[0]->cart.info.ram_size;
    uint8_t *deep = (uint8_t *)malloc(bytes);
    require(deep != NULL);
    memcpy(deep, m[0]->cart.rom, bytes);
    memcpy(&before, m[0], sizeof before);
    uint32_t allocations = counts[0].allocs, frees = counts[0].frees;
    counts[0].fail_from = allocations + 1u;
    muhtari_status st = muhtari__cart_load(m[0], data, size);
    require(st == (want == MUHTARI_STATUS_OK ? MUHTARI_STATUS_ERR_NO_MEMORY : want));
    require(memcmp(m[0], &before, sizeof before) == 0);
    require(memcmp(m[0]->cart.rom, deep, bytes) == 0);
    require(counts[0].allocs == allocations + (want == MUHTARI_STATUS_OK ? 1u : 0u));
    require(counts[0].frees == frees);
    counts[0].fail_from = 0u;
    int16_t audio[2][MUHTARI__MAX_AUDIO_SAMPLES];
    uint32_t nonzero = 0u;
    for (uint32_t frame = 0u; frame < 2u; frame++) {
        uint32_t samples[2];
        for (uint32_t i = 0u; i < 2u; i++) {
            muhtari_frame io = {0};
            io.size = (uint32_t)sizeof io;
            io.video = video + i * MUHTARI__WIDTH * MUHTARI__HEIGHT_NTSC;
            io.video_pitch = MUHTARI__WIDTH;
            io.audio = audio[i];
            io.audio_capacity = MUHTARI__MAX_AUDIO_SAMPLES;
            require(muhtari_run_frame(m[i], &io) == MUHTARI_STATUS_OK);
            samples[i] = io.audio_count;
        }
        require(samples[0] == samples[1]);
        require(memcmp(audio[0], audio[1], samples[0] * sizeof audio[0][0]) == 0);
        for (uint32_t i = 0u; i < samples[0]; i++)
            nonzero += audio[0][i] != 0;
        require(memcmp(video, video + MUHTARI__WIDTH * MUHTARI__HEIGHT_NTSC,
                       MUHTARI__WIDTH * MUHTARI__HEIGHT_NTSC) == 0);
        require(memcmp(m[0]->ram, m[1]->ram, sizeof m[0]->ram) == 0);
        require(memcmp(&m[0]->cpu, &m[1]->cpu, sizeof m[0]->cpu) == 0);
        require(memcmp(m[0]->cart.rom, m[1]->cart.rom, bytes) == 0);
    }
    require(nonzero != 0u);
    require(counts[0].allocs == allocations + (want == MUHTARI_STATUS_OK ? 1u : 0u));
    require(counts[1].allocs == 2u);
    muhtari_destroy(m[0]);
    muhtari_destroy(m[1]);
    free(cart);
    free(video);
    free(deep);
}

/* Loads data into a fresh instance; fail makes every allocation after
   create fail. Returns the load's status. */
static muhtari_status load_once(const uint8_t *data, size_t size, int fail,
                                const muhtari_cart_info *probed, muhtari_status want)
{
    alloc_counts c;
    muhtari_config cfg = alloc_config(&c);
    muhtari *m = NULL;
    require(muhtari_create(&cfg, &m) == MUHTARI_STATUS_OK);
    alloc_counts created = c;
    if (fail) {
        c.fail_from = c.allocs + 1u;
    }
    memcpy(&before, m, sizeof before);
    muhtari_status st = muhtari__cart_load(m, data, size);
    if (st == MUHTARI_STATUS_OK) {
        require(!fail && want == MUHTARI_STATUS_OK);
        require(c.allocs == created.allocs + 1u && c.frees == created.frees);
        require(c.alloc_size == (size_t)probed->rom_size + probed->ram_size);
        require(memcmp(&m->cart.info, probed, sizeof *probed) == 0);
        check_installed(data, size, m);
        /* A second load replaces the first: one allocation, one free. */
        require(muhtari__cart_load(m, data, size) == MUHTARI_STATUS_OK);
        require(c.allocs == created.allocs + 2u && c.frees == created.frees + 1u);
        require(c.free_size == (size_t)probed->rom_size + probed->ram_size);
    } else {
        /* A refusal makes no call at all; a failed allocation is the one
           call an accepted image makes. */
        if (want == MUHTARI_STATUS_OK) {
            require(fail && st == MUHTARI_STATUS_ERR_NO_MEMORY);
            require(c.allocs == created.allocs + 1u);
        } else {
            require(st == want);
            require(c.allocs == created.allocs);
        }
        require(c.frees == created.frees);
        require(memcmp(m, &before, sizeof before) == 0);
    }
    muhtari_destroy(m);
    require(c.frees == c.allocs - (st == MUHTARI_STATUS_ERR_NO_MEMORY ? 1u : 0u));
    return st;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    muhtari_cart_info a;
    muhtari_cart_info b;
    memset(&a, 0, sizeof a);
    memset(&b, 0, sizeof b);
    a.size = (uint32_t)sizeof a;
    b.size = (uint32_t)sizeof b;
    muhtari_status first = muhtari_probe(data, size, &a);
    muhtari_status second = muhtari_probe(data, size, &b);
    require(first == second && memcmp(&a, &b, sizeof a) == 0);

    require(load_once(data, size, 0, &a, first) == first);
    (void)load_once(data, size, 1, &a, first);
    static const uint32_t boards[] = {MUHTARI_BOARD_ACTIVISION, MUHTARI_BOARD_ABSOLUTE,
                                      MUHTARI_BOARD_FRACTALUS};
    warmed_failure(data, size, first, boards[size % 3u]);
    return 0;
}

#include "replay_main.h"
