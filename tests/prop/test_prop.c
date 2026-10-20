/* muhtari-test-prop NAME BUILT_DIR: runs one property (tests/prop.h) over
   its cases. Each case draws its cartridge from a pool: the run fuzz
   target's seed images (tests/fuzz/run_seeds.c) and 48 KiB of random bytes
   from the case's stream, each made a cartridge as that target makes it
   (tests/fuzz/run_cart.h), and the built simple and eightysix samples under
   BUILT_DIR. Every load is a fast boot, and a case runs at most 8 frames.
   Two runs are equal when every frame's status, audio count and picture
   match and so does the state digest: SHA-256 over struct muhtari with its
   pointers cleared, then the cartridge's ROM and RAM. Exits 0 when the
   property holds, 1 on a failure and 2 on a usage error. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../fuzz/run_cart.h"
#include "../fuzz/seeds.h"
#include "../prop.h"
#include "internal.h"
#include "muhtari.h"
#include "sha256.h"

#define WIDTH 320u
#define HEIGHT 242u
#define WIDE 336u  /* a second pitch, wider than the picture */
#define SAMPLES 2u /* simple and eightysix */

/* One frame as the caller sees it. */
struct frame_out {
    muhtari_status status;
    uint32_t audio_count;
    uint8_t picture[32];
};

static uint8_t *samples[SAMPLES];
static size_t sample_sizes[SAMPLES];
static uint8_t *image;
static uint8_t *raw;
static uint8_t video[WIDE * HEIGHT];
static int16_t audio[MUHTARI__MAX_AUDIO_SAMPLES];
static struct muhtari scratch;

static void *prop_alloc(void *user, size_t size)
{
    (void)user;
    return malloc(size);
}

static void prop_free(void *user, void *ptr, size_t size)
{
    (void)user;
    (void)size;
    free(ptr);
}

static muhtari *create(void)
{
    muhtari_config cfg;
    muhtari *m = NULL;
    memset(&cfg, 0, sizeof cfg);
    cfg.size = (uint32_t)sizeof cfg;
    cfg.allocator.alloc = prop_alloc;
    cfg.allocator.free = prop_free;
    if (muhtari_create(&cfg, &m) != MUHTARI_STATUS_OK) {
        fputs("muhtari-test-prop: create failed\n", stderr);
        exit(1);
    }
    return m;
}

static uint32_t pool_count(void)
{
    return seeds_count() + 1u + SAMPLES;
}

/* Builds pool entry p into image and returns its size; the random cart
   takes its bytes from rng. */
static size_t pool_build(uint32_t p, uint64_t *rng)
{
    if (p < seeds_count()) {
        run_cart(raw, seeds_build(p, raw), image);
        return RUN_CART_BYTES;
    }
    if (p == seeds_count()) {
        for (uint32_t at = 0; at < RUN_CART_BYTES; at += 8u) {
            uint64_t v = prop_next(rng);
            for (uint32_t b = 0; b < 8u; b++) {
                raw[at + b] = (uint8_t)(v >> (8u * b));
            }
        }
        run_cart(raw, RUN_CART_BYTES, image);
        return RUN_CART_BYTES;
    }
    uint32_t s = p - seeds_count() - 1u;
    memcpy(image, samples[s], sample_sizes[s]);
    return sample_sizes[s];
}

/* Loads the n bytes in image with the fast boot; the status is part of the
   comparison, since a refused image leaves the test card running. */
static muhtari_status load(muhtari *m, size_t n)
{
    muhtari_media media;
    memset(&media, 0, sizeof media);
    media.size = (uint32_t)sizeof media;
    media.image = image;
    media.image_size = n;
    return muhtari_load(m, &media);
}

/* Runs one frame at the given pitch and records it. */
static void run(muhtari *m, uint32_t pitch, struct frame_out *out)
{
    muhtari_frame io;
    memset(&io, 0, sizeof io);
    io.size = (uint32_t)sizeof io;
    io.video_pitch = pitch;
    io.video = video;
    io.audio = audio;
    io.audio_capacity = MUHTARI__MAX_AUDIO_SAMPLES;
    memset(out, 0, sizeof *out);
    out->status = muhtari_run_frame(m, &io);
    out->audio_count = io.audio_count;
    muhtari__sha256 s;
    muhtari__sha256_init(&s);
    for (uint32_t y = 0; y < HEIGHT; y++) {
        muhtari__sha256_update(&s, video + (size_t)y * pitch, WIDTH);
    }
    muhtari__sha256_final(&s, out->picture);
}

/* The state digest. Without pending, the input waiting for the next VBLANK
   start is left out too: it is not machine state. */
static void digest(const muhtari *m, int pending, uint8_t out[32])
{
    memcpy(&scratch, m, sizeof scratch);
    memset(&scratch.allocator, 0, sizeof scratch.allocator);
    scratch.cart.rom = NULL;
    scratch.cart.ram = NULL;
    scratch.maria.rows = NULL;
    if (!pending) {
        memset(scratch.ports.pending_device, 0, sizeof scratch.ports.pending_device);
        memset(scratch.ports.pending_buttons, 0, sizeof scratch.ports.pending_buttons);
        scratch.ports.pending_switches = 0u;
        scratch.ports.pending = 0u;
    }
    muhtari__sha256 s;
    muhtari__sha256_init(&s);
    muhtari__sha256_update(&s, &scratch, sizeof scratch);
    if (m->cart.rom != NULL) {
        muhtari__sha256_update(&s, m->cart.rom,
                               (size_t)m->cart.info.rom_size + m->cart.info.ram_size);
    }
    muhtari__sha256_final(&s, out);
}

/* Reports a mismatch; returns 0 so a property can return it. */
static int differ(const char *what, uint32_t at)
{
    fprintf(stderr, "  %s differs at %u\n", what, (unsigned)at);
    return 0;
}

/* Frames 1..k, then a pause in which the host queries the instance, then
   frames k+1..N, equal one sequence of N frames. The first part draws at a
   wider pitch, so neither the split nor the caller's buffer reaches the
   machine. */
static int split_run(uint64_t *rng, uint32_t size)
{
    uint32_t p = prop_below(rng, pool_count());
    size_t n = pool_build(p, rng);
    uint32_t frames = 1u + prop_below(rng, size);
    uint32_t k = prop_below(rng, frames + 1u);
    struct frame_out one[PROP_SIZE_MAX];
    struct frame_out two[PROP_SIZE_MAX];
    uint8_t d1[32];
    uint8_t d2[32];
    muhtari *a = create();
    muhtari *b = create();
    int ok = load(a, n) == load(b, n) ? 1 : differ("load status", 0u);
    for (uint32_t i = 0; i < frames; i++) {
        run(a, WIDTH, &one[i]);
    }
    for (uint32_t i = 0; i < k; i++) {
        run(b, WIDE, &two[i]);
    }
    muhtari_info info;
    muhtari_counters counters;
    muhtari_span span;
    memset(&info, 0, sizeof info);
    memset(&counters, 0, sizeof counters);
    info.size = (uint32_t)sizeof info;
    counters.size = (uint32_t)sizeof counters;
    (void)muhtari_get_info(b, &info);
    (void)muhtari_get_counters(b, &counters);
    for (uint32_t kind = MUHTARI_MEMORY_SYSTEM_RAM; kind <= MUHTARI_MEMORY_CART_RAM; kind++) {
        memset(&span, 0, sizeof span);
        span.size = (uint32_t)sizeof span;
        (void)muhtari_get_memory(b, kind, &span);
    }
    for (uint32_t i = k; i < frames; i++) {
        run(b, WIDTH, &two[i]);
    }
    for (uint32_t i = 0; ok && i < frames; i++) {
        if (memcmp(&one[i], &two[i], sizeof one[i]) != 0) {
            ok = differ("frame", i + 1u);
        }
    }
    digest(a, 1, d1);
    digest(b, 1, d2);
    if (ok && memcmp(d1, d2, sizeof d1) != 0) {
        ok = differ("state after frame", frames);
    }
    muhtari_destroy(a);
    muhtari_destroy(b);
    return ok;
}

/* Reads BUILT_DIR/name/name.bas.a78 into samples[i]. */
static int read_sample(const char *dir, const char *name, uint32_t i)
{
    size_t len = strlen(dir) + 2u * strlen(name) + 16u;
    char *path = (char *)malloc(len);
    if (path == NULL) {
        return 0;
    }
    (void)snprintf(path, len, "%s/%s/%s.bas.a78", dir, name, name);
    FILE *f = fopen(path, "rb");
    samples[i] = (uint8_t *)malloc(MUHTARI_IMAGE_SIZE_MAX + 1u);
    size_t got = 0u;
    if (f != NULL && samples[i] != NULL) {
        got = fread(samples[i], 1u, MUHTARI_IMAGE_SIZE_MAX + 1u, f);
    }
    int ok =
        f != NULL && samples[i] != NULL && !ferror(f) && got > 0u && got <= MUHTARI_IMAGE_SIZE_MAX;
    if (f != NULL) {
        (void)fclose(f);
    }
    if (!ok) {
        fprintf(stderr, "muhtari-test-prop: cannot read %s\n", path);
    }
    sample_sizes[i] = got;
    free(path);
    return ok;
}

static const struct {
    const char *name;
    int (*fn)(uint64_t *rng, uint32_t size);
} props[] = {
    {"split_run", split_run},
};

int main(int argc, char **argv)
{
    if (argc != 3) {
        fputs("usage: muhtari-test-prop NAME BUILT_DIR\n", stderr);
        return 2;
    }
    image = (uint8_t *)malloc(SEEDS_SIZE_MAX);
    raw = (uint8_t *)malloc(SEEDS_SIZE_MAX);
    if (image == NULL || raw == NULL || !read_sample(argv[2], "simple", 0u) ||
        !read_sample(argv[2], "eightysix", 1u)) {
        return 1;
    }
    for (size_t i = 0; i < sizeof props / sizeof props[0]; i++) {
        if (strcmp(argv[1], props[i].name) == 0) {
            return prop_run(props[i].name, props[i].fn);
        }
    }
    fprintf(stderr, "muhtari-test-prop: unknown property %s\n", argv[1]);
    return 2;
}
