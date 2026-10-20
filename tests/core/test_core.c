/* The core library through its public header: muhtari-test-core <case>.
   The input, lag and memory cases also look inside the instance through
   src/internal.h, at what the public API does not show.
   Expected test-card values follow the card's rules as docs/ describes them,
   and the pinned hash was computed by an independent model of those rules,
   not by this code. */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"
#include "muhtari.h"
#include "sha256.h"
#include "../alloc.h"
#include "../cart/images.h"
#include "../check.h"

#define WIDTH 320u
#define HEIGHT 242u
#define FRAME_BYTES (WIDTH * HEIGHT)
#define AUDIO_CAP 801u
#define WIDE 336u                  /* a pitch 16 bytes wider than a row */
#define PAD_BYTE 0xA5u             /* fill the core must leave alone */
#define AUDIO_SENTINEL 0x5A5A      /* sample the core must overwrite or leave alone */
#define COUNT_SENTINEL 0xDEADBEEFu /* audio_count before a call that must write it */

/* SHA-256 of the NTSC test card's 77,440 native colour bytes. */
#define PINNED_HASH "9eb011e22feb3f48da2c0abf3dd8d5236d4d0f5c0e67a732eb2172259e0a05c4"

/* One NTSC frame is 119,402 ticks; 48 kHz audio is 88 samples per 13,125
   ticks (A7800-AUDIO, timing table). */
#define TICKS_PER_FRAME 119402u
#define SAMPLES_PER_PERIOD 88u
#define TICKS_PER_PERIOD 13125u

static uint8_t video[FRAME_BYTES];
static uint8_t video2[FRAME_BYTES];
static uint8_t wide[WIDE * HEIGHT];
static int16_t audio[AUDIO_CAP + 1u];

/* The plain malloc allocator, as every host defines it. */
static void *test_alloc(void *user, size_t size)
{
    (void)user;
    return malloc(size);
}

static void test_free(void *user, void *ptr, size_t size)
{
    (void)user;
    (void)size;
    free(ptr);
}

static muhtari *make(void)
{
    muhtari_config cfg;
    memset(&cfg, 0, sizeof cfg);
    cfg.size = (uint32_t)sizeof cfg;
    cfg.allocator.alloc = test_alloc;
    cfg.allocator.free = test_free;
    muhtari *m = NULL;
    CHECK_EQ_U64(muhtari_create(&cfg, &m), MUHTARI_STATUS_OK);
    return m;
}

/* Fills *io in place after a memset, so no padding is left unspecified. */
static void frame_io(muhtari_frame *io, uint8_t *v, uint32_t pitch, int16_t *a, uint32_t cap)
{
    memset(io, 0, sizeof *io);
    io->size = (uint32_t)sizeof *io;
    io->video = v;
    io->video_pitch = pitch;
    io->audio = a;
    io->audio_capacity = cap;
}

/* Runs one frame into the 320-pitch buffer v with full audio. */
static void run(muhtari *m, uint8_t *v)
{
    muhtari_frame io;
    frame_io(&io, v, WIDTH, audio, AUDIO_CAP);
    CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_OK);
}

static void hash_hex(const uint8_t *bytes, size_t len, char hex[65])
{
    muhtari__sha256 s;
    uint8_t digest[32];
    muhtari__sha256_init(&s);
    muhtari__sha256_update(&s, bytes, len);
    muhtari__sha256_final(&s, digest);
    muhtari__sha256_hex(digest, hex);
}

static void check_str(const char *what, const char *got, const char *want)
{
    if (strcmp(got, want) != 0) {
        fprintf(stderr, "%s: expected %s, got %s\n", what, want, got);
        check_failures++;
    }
}

/* Frame 1 of a new instance hashes to the pinned value. */
static void test_pinned_hash(void)
{
    muhtari *m = make();
    run(m, video);
    char hex[65];
    hash_hex(video, FRAME_BYTES, hex);
    check_str("frame 1 sha256", hex, PINNED_HASH);
    muhtari_destroy(m);
}

/* The colour chart holds every value $00-$FF at least once. */
static void test_all_colours(void)
{
    uint32_t seen[256] = {0};
    muhtari *m = make();
    run(m, video);
    for (uint32_t i = 0; i < FRAME_BYTES; i++) {
        seen[video[i]]++;
    }
    for (uint32_t v = 0; v < 256u; v++) {
        if (seen[v] == 0u) {
            fprintf(stderr, "colour 0x%02x: expected at least 1 pixel, got 0\n", (unsigned)v);
            check_failures++;
        }
    }
    muhtari_destroy(m);
}

/* With no content the card is static: frame LONG_RUN (600, or 60 on short
   runs) equals frame 1. */
static void test_static(void)
{
    muhtari *m = make();
    char first[65], last[65];
    run(m, video);
    hash_hex(video, FRAME_BYTES, first);
    for (uint32_t f = 2; f <= LONG_RUN; f++) {
        run(m, video2);
    }
    hash_hex(video2, FRAME_BYTES, last);
    check_str("last frame sha256", last, first);
    muhtari_destroy(m);
}

/* Pixels on each side of every rule's edge. The rules apply in order, first
   match wins, with the chart's first row cy = (242 - 192) / 2 = 25. */
static void test_boundaries(void)
{
    static const struct {
        uint32_t x, y;
        uint8_t want;
    } px[] = {
        {0, 0, 0x0F},     /* corner block */
        {7, 7, 0x0F},     /* corner block, last pixel */
        {8, 8, 0x08},     /* overscan line, just past the corner block */
        {319, 100, 0x0F}, /* right border */
        {100, 241, 0x0F}, /* bottom border */
        {100, 8, 0x08},   /* first overscan line */
        {100, 231, 0x08}, /* last overscan line, h - 11 */
        {100, 24, 0x46},  /* red band, last row above the chart */
        {100, 25, 0x03},  /* chart, first row: hue 0, luma (100 - 48) / 14 */
        {48, 25, 0x00},   /* chart, first cell */
        {61, 25, 0x00},   /* chart, first cell's last column */
        {62, 25, 0x01},   /* chart, second cell */
        {271, 216, 0xFF}, /* chart, last cell */
        {47, 25, 0x00},   /* 1x1 checker: (47 ^ 25) & 1 = 0 */
        {47, 26, 0x0F},   /* 1x1 checker: (47 ^ 26) & 1 = 1 */
        {272, 25, 0x0F},  /* 2x1 checker: (136 ^ 25) & 1 = 1 */
        {273, 25, 0x0F},  /* 2x1 checker: the pair's second pixel */
        {274, 25, 0x00},  /* 2x1 checker: (137 ^ 25) & 1 = 0 */
        {100, 217, 0xC6}, /* green band, first row below the chart */
    };
    muhtari *m = make();
    run(m, video);
    for (size_t i = 0; i < sizeof px / sizeof px[0]; i++) {
        uint8_t got = video[px[i].y * WIDTH + px[i].x];
        if (got != px[i].want) {
            fprintf(stderr, "pixel (%u,%u): expected 0x%02x, got 0x%02x\n", (unsigned)px[i].x,
                    (unsigned)px[i].y, (unsigned)px[i].want, (unsigned)got);
            check_failures++;
        }
    }
    muhtari_destroy(m);
}

/* At pitch 336 each row's 16 padding bytes stay untouched and its 320 pixels
   equal the pitch-320 render. */
static void test_pitch_336(void)
{
    muhtari *m = make();
    run(m, video);
    memset(wide, (int)PAD_BYTE, sizeof wide);
    muhtari_frame io;
    frame_io(&io, wide, WIDE, audio, AUDIO_CAP);
    CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_OK);
    for (uint32_t y = 0; y < HEIGHT; y++) {
        const uint8_t *row = wide + (size_t)y * WIDE;
        if (memcmp(row, video + (size_t)y * WIDTH, WIDTH) != 0) {
            fprintf(stderr, "row %u: expected the pitch-320 row, got different pixels\n",
                    (unsigned)y);
            check_failures++;
        }
        for (uint32_t x = WIDTH; x < WIDE; x++) {
            if (row[x] != PAD_BYTE) {
                fprintf(stderr, "row %u byte %u: expected padding 0x%02x, got 0x%02x\n",
                        (unsigned)y, (unsigned)x, PAD_BYTE, (unsigned)row[x]);
                check_failures++;
            }
        }
    }
    muhtari_destroy(m);
}

/* Each refused call returns its status and changes no state, and one refused
   after its size tag is accepted writes audio_count as zero: the following
   valid frames give the counts an untouched instance gives, 800 then 801. */
static void test_validate_first(void)
{
    muhtari *m = make();
    muhtari_frame io;
    frame_io(&io, video, WIDTH, audio, AUDIO_CAP);

    CHECK_EQ_U64(muhtari_run_frame(NULL, &io), MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK_EQ_U64(muhtari_run_frame(m, NULL), MUHTARI_STATUS_ERR_ARGUMENT);

    frame_io(&io, video, 319u, audio, AUDIO_CAP);
    io.audio_count = COUNT_SENTINEL;
    CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK_EQ_U64(io.audio_count, 0);

    /* Large enough for 242 rows at pitch 65537, so a core that wrongly
       accepts that pitch fails this check instead of overrunning memory. */
    uint8_t *big = (uint8_t *)malloc((size_t)65537u * HEIGHT);
    CHECK(big != NULL);
    if (big != NULL) {
        frame_io(&io, big, 65537u, audio, AUDIO_CAP);
        io.audio_count = COUNT_SENTINEL;
        CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_ERR_ARGUMENT);
        CHECK_EQ_U64(io.audio_count, 0);
    }

    /* Frame 1 needs 800 samples. */
    frame_io(&io, video, WIDTH, audio, 799u);
    io.audio_count = COUNT_SENTINEL;
    CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_ERR_BUFFER_TOO_SMALL);
    CHECK_EQ_U64(io.audio_count, 0);

    frame_io(&io, video, WIDTH, audio, 800u);
    CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(io.audio_count, 800);

    /* Frame 2 needs 801. */
    frame_io(&io, video, WIDTH, audio, 800u);
    io.audio_count = COUNT_SENTINEL;
    CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_ERR_BUFFER_TOO_SMALL);
    CHECK_EQ_U64(io.audio_count, 0);

    /* No video and no audio is a valid frame; it still advances the audio. */
    frame_io(&io, NULL, 0u, NULL, 0u);
    io.audio_count = COUNT_SENTINEL;
    CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(io.audio_count, 801);

    /* The largest pitch is accepted. */
    if (big != NULL) {
        frame_io(&io, big, 65536u, NULL, 0u);
        CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_OK);
        CHECK_EQ_U64(io.audio_count, 800);
        CHECK_EQ_HEX(big[(size_t)241u * 65536u + 319u], 0x0F);
        free(big);
    }
    muhtari_destroy(m);
}

/* Every frame writes 800 or 801 zero samples, alternating from 800, 801, and
   the total never drifts: N frames give floor(N * 119402 * 88 / 13125). */
static void test_silence(void)
{
    static const uint32_t first[4] = {800, 801, 800, 801};
    muhtari *m = make();
    muhtari_frame io;
    uint64_t total = 0;
    for (uint32_t f = 1; f <= 13125u; f++) {
        for (uint32_t i = 0; i <= AUDIO_CAP; i++) {
            audio[i] = AUDIO_SENTINEL;
        }
        frame_io(&io, NULL, 0u, audio, AUDIO_CAP);
        CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_OK);
        if (f <= 4u) {
            CHECK_EQ_U64(io.audio_count, first[f - 1u]);
        }
        CHECK(io.audio_count == 800u || io.audio_count == 801u);
        for (uint32_t i = 0; i < io.audio_count && i < AUDIO_CAP; i++) {
            if (audio[i] != 0) {
                fprintf(stderr, "frame %u sample %u: expected 0, got %d\n", (unsigned)f,
                        (unsigned)i, (int)audio[i]);
                check_failures++;
                break;
            }
        }
        CHECK_EQ_HEX((uint16_t)audio[AUDIO_CAP], AUDIO_SENTINEL);
        if (io.audio_count == 800u) {
            CHECK_EQ_HEX((uint16_t)audio[800], AUDIO_SENTINEL);
        }
        total += io.audio_count;
    }
    CHECK_EQ_U64(total, (uint64_t)13125u * TICKS_PER_FRAME * SAMPLES_PER_PERIOD / TICKS_PER_PERIOD);
    muhtari_destroy(m);
}

/* A buffer for one size-tagged struct and 16 bytes past it, aligned for any
   of them. The cart info is the largest, and its +8 cases need 8 more. */
#define TAIL 16u
typedef union tagged {
    muhtari_config config;
    muhtari_versions versions;
    muhtari_info info;
    muhtari_frame frame;
    muhtari_cart_info cart;
    uint8_t bytes[sizeof(muhtari_cart_info) + TAIL + 8u];
} tagged;

/* Each size-tag case for one struct, through call(), which runs the call
   that takes it. ours is the struct's sizeof, which is also its first-release
   size. An output must write no byte past min(size, ours). */
static void size_cases(const char *name, uint32_t ours, int is_output,
                       muhtari_status (*call)(tagged *t, muhtari *m), muhtari *m)
{
    static const struct {
        int32_t delta;    /* size = ours + delta */
        uint32_t nonzero; /* offset past ours of a nonzero tail byte, or 0 for none */
        muhtari_status want;
    } cases[] = {
        {-4, 0, MUHTARI_STATUS_ERR_STRUCT_SIZE},
        {-1, 0, MUHTARI_STATUS_ERR_STRUCT_SIZE},
        {0, 0, MUHTARI_STATUS_OK},
        {8, 0, MUHTARI_STATUS_OK},
        {8, 1, MUHTARI_STATUS_ERR_STRUCT_SIZE},
        {8, 8, MUHTARI_STATUS_ERR_STRUCT_SIZE},
    };
    for (size_t i = 0; i < sizeof cases / sizeof cases[0]; i++) {
        tagged t;
        memset(&t, 0, sizeof t);
        uint32_t size = (uint32_t)((int32_t)ours + cases[i].delta);
        /* Bytes past the declared size are a sentinel no call may touch. */
        memset(t.bytes + size, 0xA5, sizeof t.bytes - size);
        memcpy(t.bytes, &size, sizeof size);
        if (cases[i].nonzero != 0u) {
            t.bytes[ours + cases[i].nonzero - 1u] = 0x01u;
        }
        tagged before = t;
        muhtari_status got = call(&t, m);
        if (got != cases[i].want) {
            fprintf(stderr, "%s size %u (tail byte %u): expected status %u, got %u\n", name,
                    (unsigned)size, (unsigned)cases[i].nonzero, (unsigned)cases[i].want,
                    (unsigned)got);
            check_failures++;
        }
        uint32_t keep = got != MUHTARI_STATUS_OK || !is_output ? 0u : (size < ours ? size : ours);
        for (uint32_t b = keep; b < sizeof t.bytes; b++) {
            /* An input struct's fields are the caller's; only a frame's
               audio_count is written back, and only on success. */
            if (b < ours && !is_output && got == MUHTARI_STATUS_OK) {
                continue;
            }
            if (t.bytes[b] != before.bytes[b]) {
                fprintf(stderr, "%s size %u: byte %u changed from 0x%02x to 0x%02x\n", name,
                        (unsigned)size, (unsigned)b, (unsigned)before.bytes[b],
                        (unsigned)t.bytes[b]);
                check_failures++;
                break;
            }
        }
    }
    /* A garbage tag far past the buffer is refused before any byte past the
       tag is read, and nothing is written. */
    tagged t;
    memset(&t, 0, sizeof t);
    uint32_t garbage = 0xFFFFFFFFu;
    memcpy(t.bytes, &garbage, sizeof garbage);
    tagged before = t;
    muhtari_status got = call(&t, m);
    if (got != MUHTARI_STATUS_ERR_STRUCT_SIZE) {
        fprintf(stderr, "%s size 0xFFFFFFFF: expected status %u, got %u\n", name,
                (unsigned)MUHTARI_STATUS_ERR_STRUCT_SIZE, (unsigned)got);
        check_failures++;
    }
    CHECK(memcmp(t.bytes, before.bytes, sizeof t.bytes) == 0);
}

static muhtari_status call_versions(tagged *t, muhtari *m)
{
    (void)m;
    return muhtari_get_versions(&t->versions);
}

static muhtari_status call_info(tagged *t, muhtari *m)
{
    return muhtari_get_info(m, &t->info);
}

static muhtari_status call_frame(tagged *t, muhtari *m)
{
    return muhtari_run_frame(m, &t->frame);
}

/* A clean A78 header over a 4 KiB linear body, for the probe's size tags. */
static uint8_t probe_image[128 + 4096];

static muhtari_status call_probe(tagged *t, muhtari *m)
{
    (void)m;
    return muhtari_probe(probe_image, sizeof probe_image, &t->cart);
}

static muhtari_status call_config(tagged *t, muhtari *m)
{
    (void)m;
    /* The hooks are the test's own input; put the bytes back afterwards so
       only a write by the core would show. */
    muhtari_allocator saved = t->config.allocator;
    t->config.allocator.alloc = test_alloc;
    t->config.allocator.free = test_free;
    muhtari *made = NULL;
    muhtari_status st = muhtari_create(&t->config, &made);
    muhtari_destroy(made);
    t->config.allocator = saved;
    return st;
}

static void test_size_tags(void)
{
    muhtari *m = make();
    size_cases("config", (uint32_t)sizeof(muhtari_config), 0, call_config, m);
    size_cases("versions", (uint32_t)sizeof(muhtari_versions), 1, call_versions, m);
    size_cases("info", (uint32_t)sizeof(muhtari_info), 1, call_info, m);
    size_cases("frame", (uint32_t)sizeof(muhtari_frame), 0, call_frame, m);
    probe_image[0] = 3u;
    memcpy(probe_image + 1, "ATARI7800", 9);
    probe_image[51] = 0x10u; /* size field 4096 */
    size_cases("cart_info", (uint32_t)sizeof(muhtari_cart_info), 1, call_probe, m);

    /* An output's own fields are written: the size stays the caller's. */
    muhtari_info info;
    memset(&info, 0, sizeof info);
    info.size = (uint32_t)sizeof info;
    CHECK_EQ_U64(muhtari_get_info(m, &info), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(info.size, sizeof info);
    CHECK_EQ_U64(info.width, WIDTH);
    muhtari_destroy(m);
}

/* create requires both hooks, reports a failed allocation without touching the
   out pointer, and destroy hands free the size alloc was given. */
static void test_allocator(void)
{
    alloc_counts c;
    memset(&c, 0, sizeof c);
    muhtari_config cfg;
    memset(&cfg, 0, sizeof cfg);
    cfg.size = (uint32_t)sizeof cfg;
    cfg.allocator.user = &c;
    muhtari *const marker = (muhtari *)&c;
    muhtari *m = marker;

    CHECK_EQ_U64(muhtari_create(NULL, &m), MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK_EQ_U64(muhtari_create(&cfg, NULL), MUHTARI_STATUS_ERR_ARGUMENT);

    cfg.allocator.free = count_free;
    CHECK_EQ_U64(muhtari_create(&cfg, &m), MUHTARI_STATUS_ERR_ARGUMENT);
    cfg.allocator.alloc = count_alloc;
    cfg.allocator.free = NULL;
    CHECK_EQ_U64(muhtari_create(&cfg, &m), MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK_EQ_U64(c.allocs, 0);
    CHECK(m == marker);

    cfg.allocator.free = count_free;
    c.fail_from = 1u;
    CHECK_EQ_U64(muhtari_create(&cfg, &m), MUHTARI_STATUS_ERR_NO_MEMORY);
    CHECK_EQ_U64(c.allocs, 1);
    CHECK(m == marker);

    c.fail_from = 0u;
    CHECK_EQ_U64(muhtari_create(&cfg, &m), MUHTARI_STATUS_OK);
    CHECK(m != marker && m != NULL);
    muhtari_destroy(m);
    CHECK_EQ_U64(c.frees, 1);
    CHECK_EQ_U64(c.free_size, c.alloc_size);
    CHECK(c.alloc_size > 0u);

    muhtari_destroy(NULL);
}

/* The frame, rate and audio constants hosts size their output from, and the
   interface numbers. Zero means absent for api, abi and behaviour_revision,
   which start at 1; library_major is legitimately 0 before 1.0, so the
   library version is not checked here. */
static void test_info(void)
{
    muhtari *m = make();
    muhtari_info info;
    memset(&info, 0, sizeof info);
    info.size = (uint32_t)sizeof info;
    CHECK_EQ_U64(muhtari_get_info(NULL, &info), MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK_EQ_U64(muhtari_get_info(m, NULL), MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK_EQ_U64(muhtari_get_info(m, &info), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(info.width, 320);
    CHECK_EQ_U64(info.height, 242);
    CHECK_EQ_U64(info.fps_num, 39375000);
    CHECK_EQ_U64(info.fps_den, 656711);
    CHECK_EQ_U64(info.par_num, 6);
    CHECK_EQ_U64(info.par_den, 7);
    CHECK_EQ_U64(info.sample_rate_num, 48000);
    CHECK_EQ_U64(info.sample_rate_den, 1);
    CHECK_EQ_U64(info.max_audio_samples, 801);

    muhtari_versions v;
    memset(&v, 0, sizeof v);
    v.size = (uint32_t)sizeof v;
    CHECK_EQ_U64(muhtari_get_versions(NULL), MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK_EQ_U64(muhtari_get_versions(&v), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(v.api, MUHTARI_API_VERSION);
    CHECK_EQ_U64(v.api, 3);
    CHECK_EQ_U64(v.abi, 1);
    CHECK_EQ_U64(v.behaviour_revision, 1);
    muhtari_destroy(m);
}

/* After create the core never allocates: one alloc at create, none in
   LONG_RUN frames, one free at destroy. */
static void test_no_alloc_after_load(void)
{
    alloc_counts c;
    memset(&c, 0, sizeof c);
    muhtari_config cfg;
    memset(&cfg, 0, sizeof cfg);
    cfg.size = (uint32_t)sizeof cfg;
    cfg.allocator.alloc = count_alloc;
    cfg.allocator.free = count_free;
    cfg.allocator.user = &c;
    muhtari *m = NULL;
    CHECK_EQ_U64(muhtari_create(&cfg, &m), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(c.allocs, 1);
    for (uint32_t f = 0; f < LONG_RUN; f++) {
        run(m, video);
    }
    CHECK_EQ_U64(c.allocs, 1);
    CHECK_EQ_U64(c.frees, 0);
    muhtari_destroy(m);
    CHECK_EQ_U64(c.allocs, 1);
    CHECK_EQ_U64(c.frees, 1);
}

/* A 4 KiB linear cartridge built in memory: an A78 version 3 header with
   ProLine on both ports and the TV byte tv, over a body that runs program,
   n bytes at $F000, which the reset vector points at. */
#define CART_BODY 4096u
#define CART_SIZE (IMAGES_HEADER + CART_BODY)

static void make_cart(uint8_t *buf, const uint8_t *program, size_t n, uint8_t tv)
{
    images_header(buf, 3u, "Core", CART_BODY, 0u);
    buf[57] = tv;
    uint8_t *body = buf + IMAGES_HEADER;
    memset(body, 0xEA, CART_BODY); /* NOP */
    memcpy(body, program, n);
    body[CART_BODY - 4u] = 0x00u; /* reset vector $F000 */
    body[CART_BODY - 3u] = 0xF0u;
}

/* The programs: each loops at $F000 forever. */
static const uint8_t prog_idle[] = {0x4C, 0x00, 0xF0};                    /* JMP $F000 */
static const uint8_t prog_count[] = {0xEE, 0x00, 0x18, 0x4C, 0x00, 0xF0}; /* INC $1800 */
static const uint8_t prog_swcha[] = {0xAD, 0x80, 0x02, 0x4C, 0x00, 0xF0}; /* LDA $0280 */
static const uint8_t prog_inpt4[] = {0xA5, 0x0C, 0x4C, 0x00, 0xF0};       /* LDA $0C */

static uint8_t cart[CART_SIZE];
static uint8_t other[CART_SIZE];
static uint8_t bios[16384];

static muhtari_media media_for(const void *image, size_t n)
{
    muhtari_media media;
    memset(&media, 0, sizeof media);
    media.size = (uint32_t)sizeof media;
    media.image = image;
    media.image_size = n;
    return media;
}

/* Loads the cartridge running program into m. */
static void load_program(muhtari *m, const uint8_t *program, size_t n)
{
    make_cart(cart, program, n, 0u);
    muhtari_media media = media_for(cart, sizeof cart);
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
}

/* A refused load, for each reason, leaves the previous cartridge running:
   the instance that saw the refusals gives the frames and RAM of one that
   did not. */
static void test_load_keeps_previous(void)
{
    muhtari *a = make();
    muhtari *b = make();
    load_program(a, prog_count, sizeof prog_count);
    load_program(b, prog_count, sizeof prog_count);
    run(a, video);
    run(b, video2);

    muhtari_media media = media_for(other, images_2600(other, 4096u));
    CHECK_EQ_U64(muhtari_load(a, &media), MUHTARI_STATUS_ERR_UNSUPPORTED_2600);
    make_cart(other, prog_idle, sizeof prog_idle, 1u);
    media = media_for(other, sizeof other);
    CHECK_EQ_U64(muhtari_load(a, &media), MUHTARI_STATUS_ERR_UNSUPPORTED_REGION);
    make_cart(other, prog_idle, sizeof prog_idle, 0u);
    media.region = MUHTARI_REGION_PAL;
    CHECK_EQ_U64(muhtari_load(a, &media), MUHTARI_STATUS_ERR_UNSUPPORTED_REGION);
    media.region = 0u;
    media.bios = bios;
    media.bios_size = 8192u;
    CHECK_EQ_U64(muhtari_load(a, &media), MUHTARI_STATUS_ERR_INVALID_BIOS);

    run(a, video);
    run(b, video2);
    char ha[65], hb[65];
    hash_hex(video, FRAME_BYTES, ha);
    hash_hex(video2, FRAME_BYTES, hb);
    check_str("frame 2 after refused loads", ha, hb);
    CHECK(memcmp(a->ram, b->ram, sizeof a->ram) == 0);
    CHECK(a->ram[0] != 0u);
    CHECK_EQ_U64(a->bios_size, 0);
    muhtari_destroy(a);
    muhtari_destroy(b);
}

/* An empty image, and a NULL one with a size, are refused before anything
   is allocated. */
static void test_load_empty_refused(void)
{
    alloc_counts c;
    muhtari_config cfg = alloc_config(&c);
    muhtari *m = NULL;
    CHECK_EQ_U64(muhtari_create(&cfg, &m), MUHTARI_STATUS_OK);
    muhtari_media media = media_for(cart, 0u);
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_ERR_INVALID_IMAGE);
    media = media_for(NULL, 0u);
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_ERR_INVALID_IMAGE);
    media = media_for(NULL, CART_SIZE);
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_ERR_ARGUMENT);
    make_cart(cart, prog_idle, sizeof prog_idle, 0u);
    media = media_for(cart, sizeof cart);
    media.bios_size = 4096u; /* a size with no BIOS */
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK_EQ_U64(muhtari_load(NULL, &media), MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK_EQ_U64(muhtari_load(m, NULL), MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK_EQ_U64(c.allocs, 1);
    CHECK(m->cart.rom == NULL);
    muhtari_destroy(m);
}

/* A BIOS must be 4 KiB or 16 KiB with its reset vector inside it; a valid
   one is copied into the instance and allocates nothing. */
static void test_load_invalid_bios(void)
{
    alloc_counts c;
    muhtari_config cfg = alloc_config(&c);
    muhtari *m = NULL;
    CHECK_EQ_U64(muhtari_create(&cfg, &m), MUHTARI_STATUS_OK);
    make_cart(cart, prog_idle, sizeof prog_idle, 0u);
    muhtari_media media = media_for(cart, sizeof cart);
    media.bios = bios;
    memset(bios, 0xEA, sizeof bios);
    static const size_t bad_sizes[] = {0u, 4095u, 8192u};
    for (size_t i = 0; i < sizeof bad_sizes / sizeof bad_sizes[0]; i++) {
        media.bios_size = bad_sizes[i];
        bios[bad_sizes[i] > 3u ? bad_sizes[i] - 3u : 0u] = 0xF0u;
        CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_ERR_INVALID_BIOS);
    }
    media.bios_size = 16384u;
    bios[16380] = 0x00u; /* reset vector $0000 */
    bios[16381] = 0x00u;
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_ERR_INVALID_BIOS);
    bios[16381] = 0xBFu; /* $BF00, below $C000 */
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_ERR_INVALID_BIOS);
    CHECK_EQ_U64(c.allocs, 1);

    bios[16381] = 0xC0u; /* $C000 */
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(m->bios_size, 16384);
    CHECK(memcmp(m->bios, bios, sizeof bios) == 0);
    media.bios_size = 4096u;
    bios[4092] = 0x00u; /* $F000 */
    bios[4093] = 0xF0u;
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(m->bios_size, 4096);
    CHECK_EQ_U64(c.allocs, 3); /* the instance and two ROM copies */
    muhtari_destroy(m);
}

static muhtari_input input_of(uint32_t buttons0, uint32_t switches)
{
    muhtari_input in;
    memset(&in, 0, sizeof in);
    in.size = (uint32_t)sizeof in;
    in.buttons[0] = buttons0;
    in.switches = switches;
    return in;
}

/* Unknown bits and devices are refused and change nothing. */
static void test_set_input_validates(void)
{
    muhtari *m = make();
    load_program(m, prog_idle, sizeof prog_idle);
    muhtari_input ok = input_of(MUHTARI_BUTTON_UP | MUHTARI_BUTTON_2, MUHTARI_SWITCH_PAUSE);
    ok.device[1] = MUHTARI_PORT_JOYSTICK;
    CHECK_EQ_U64(muhtari_set_input(m, &ok), MUHTARI_STATUS_OK);
    struct muhtari__ports before = m->ports;

    muhtari_input in = input_of(0x40u, 0u);
    CHECK_EQ_U64(muhtari_set_input(m, &in), MUHTARI_STATUS_ERR_ARGUMENT);
    in = input_of(0u, 0u);
    in.buttons[1] = 0x80000000u;
    CHECK_EQ_U64(muhtari_set_input(m, &in), MUHTARI_STATUS_ERR_ARGUMENT);
    in = input_of(0u, 0x20u);
    CHECK_EQ_U64(muhtari_set_input(m, &in), MUHTARI_STATUS_ERR_ARGUMENT);
    in = input_of(0u, 0u);
    in.device[1] = MUHTARI_PORT_PADDLE;
    CHECK_EQ_U64(muhtari_set_input(m, &in), MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE);
    in.device[1] = 13u; /* no device has this code */
    CHECK_EQ_U64(muhtari_set_input(m, &in), MUHTARI_STATUS_ERR_ARGUMENT);
    in.device[1] = 0u;
    CHECK_EQ_U64(muhtari_set_input(NULL, &in), MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK_EQ_U64(muhtari_set_input(m, NULL), MUHTARI_STATUS_ERR_ARGUMENT);
    in.size = 4u;
    CHECK_EQ_U64(muhtari_set_input(m, &in), MUHTARI_STATUS_ERR_STRUCT_SIZE);
    CHECK(memcmp(&m->ports, &before, sizeof before) == 0);

    /* The devices resolve: the header's ProLine for 0, the host's choice
       otherwise, and nothing before the input applies. */
    CHECK_EQ_U64(muhtari__ports_device(m, 1u), MUHTARI_PORT_PROLINE);
    run(m, NULL);
    run(m, NULL);
    CHECK_EQ_U64(muhtari__ports_device(m, 0u), MUHTARI_PORT_PROLINE);
    CHECK_EQ_U64(muhtari__ports_device(m, 1u), MUHTARI_PORT_JOYSTICK);
    in = input_of(0u, 0u);
    in.device[0] = MUHTARI_PORT_UNPLUGGED;
    CHECK_EQ_U64(muhtari_set_input(m, &in), MUHTARI_STATUS_OK);
    run(m, NULL);
    CHECK_EQ_U64(muhtari__ports_device(m, 0u), MUHTARI_PORT_UNPLUGGED);
    muhtari_destroy(m);
}

/* Input set after the load, which leaves the beam mid-frame, reaches the
   machine at the next VBLANK start, the start of frame 2; of two inputs
   set before it, the second applies. */
static void test_set_input_latched_at_vblank(void)
{
    muhtari *m = make();
    load_program(m, prog_idle, sizeof prog_idle);
    muhtari_input a = input_of(MUHTARI_BUTTON_LEFT, MUHTARI_SWITCH_RESET);
    muhtari_input b = input_of(MUHTARI_BUTTON_1, MUHTARI_SWITCH_LEFT_DIFFICULTY_A);
    CHECK_EQ_U64(muhtari_set_input(m, &a), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari_set_input(m, &b), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(m->ports.buttons[0], 0);
    run(m, NULL); /* frame 1: hand-off to VBLANK start */
    CHECK_EQ_U64(m->ports.buttons[0], 0);
    CHECK_EQ_U64(m->ports.switches, 0);
    run(m, NULL); /* frame 2 starts at VBLANK start */
    CHECK_EQ_U64(m->ports.buttons[0], MUHTARI_BUTTON_1);
    CHECK_EQ_U64(m->ports.switches, MUHTARI_SWITCH_LEFT_DIFFICULTY_A);
    CHECK_EQ_U64(m->ports.pending, 0);
    /* From then on every frame starts at a VBLANK start. */
    CHECK_EQ_U64(muhtari_set_input(m, &a), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(m->ports.buttons[0], MUHTARI_BUTTON_1);
    run(m, NULL);
    CHECK_EQ_U64(m->ports.buttons[0], MUHTARI_BUTTON_LEFT);
    CHECK_EQ_U64(m->ports.switches, MUHTARI_SWITCH_RESET);
    muhtari_destroy(m);
}

static muhtari_span span_of(muhtari *m, uint32_t kind, muhtari_status want)
{
    muhtari_span span;
    memset(&span, 0, sizeof span);
    span.size = (uint32_t)sizeof span;
    CHECK_EQ_U64(muhtari_get_memory(m, kind, &span), want);
    return span;
}

/* The spans are live, stable pointers into the machine's RAM. */
static void test_get_memory_spans(void)
{
    muhtari *m = make();
    muhtari_span ram = span_of(m, MUHTARI_MEMORY_SYSTEM_RAM, MUHTARI_STATUS_OK);
    CHECK(ram.data != NULL);
    CHECK_EQ_U64(ram.bytes, 4096);
    load_program(m, prog_count, sizeof prog_count);
    run(m, NULL);
    muhtari_span again = span_of(m, MUHTARI_MEMORY_SYSTEM_RAM, MUHTARI_STATUS_OK);
    CHECK(again.data == ram.data);
    CHECK(ram.data[0] != 0u);
    CHECK_EQ_HEX(ram.data[0], muhtari__bus_peek(m, 0x1800u));
    ram.data[0x123] = 0x5Au;
    CHECK_EQ_HEX(muhtari__bus_peek(m, 0x1923u), 0x5A);

    muhtari_span riot = span_of(m, MUHTARI_MEMORY_RIOT_RAM, MUHTARI_STATUS_OK);
    CHECK(riot.data != NULL);
    CHECK_EQ_U64(riot.bytes, 128);
    riot.data[5] = 0xA7u;
    CHECK_EQ_HEX(muhtari__bus_peek(m, 0x0485u), 0xA7);

    muhtari_span cart_ram = span_of(m, MUHTARI_MEMORY_CART_RAM, MUHTARI_STATUS_OK);
    CHECK(cart_ram.data == NULL);
    CHECK_EQ_U64(cart_ram.bytes, 0);
    (void)span_of(m, 0u, MUHTARI_STATUS_ERR_ARGUMENT);
    (void)span_of(m, 4u, MUHTARI_STATUS_ERR_ARGUMENT);
    muhtari_span span;
    memset(&span, 0, sizeof span);
    span.size = (uint32_t)sizeof span;
    CHECK_EQ_U64(muhtari_get_memory(NULL, MUHTARI_MEMORY_SYSTEM_RAM, &span),
                 MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK_EQ_U64(muhtari_get_memory(m, MUHTARI_MEMORY_SYSTEM_RAM, NULL),
                 MUHTARI_STATUS_ERR_ARGUMENT);
    span.size = 4u;
    CHECK_EQ_U64(muhtari_get_memory(m, MUHTARI_MEMORY_SYSTEM_RAM, &span),
                 MUHTARI_STATUS_ERR_STRUCT_SIZE);
    CHECK(span.data == NULL);

    for (uint32_t f = 0; f < 3u; f++) {
        run(m, NULL);
    }
    muhtari_span later = span_of(m, MUHTARI_MEMORY_RIOT_RAM, MUHTARI_STATUS_OK);
    CHECK(later.data == riot.data);
    muhtari_destroy(m);
}

static muhtari_counters counters_of(const muhtari *m)
{
    muhtari_counters c;
    memset(&c, 0, sizeof c);
    c.size = (uint32_t)sizeof c;
    CHECK_EQ_U64(muhtari_get_counters(m, &c), MUHTARI_STATUS_OK);
    return c;
}

/* Frames count from the load or power cycle; a frame is a lag frame unless
   the CPU read SWCHA, SWCHB or INPT0-5 in it. A peek and a MARIA fetch of
   $0280 are not CPU reads. */
static void test_counters_frames_and_lag(void)
{
    muhtari *m = make();
    run(m, NULL);
    muhtari_counters c = counters_of(m);
    CHECK_EQ_U64(c.frames, 1);
    CHECK_EQ_U64(c.lag_frames, 1);

    load_program(m, prog_idle, sizeof prog_idle);
    c = counters_of(m);
    CHECK_EQ_U64(c.frames, 0);
    CHECK_EQ_U64(c.lag_frames, 0);
    for (uint32_t f = 0; f < 5u; f++) {
        run(m, NULL);
        (void)muhtari__bus_peek(m, 0x0280u);
        (void)muhtari__bus_peek(m, 0x000Cu);
    }
    c = counters_of(m);
    CHECK_EQ_U64(c.frames, 5);
    CHECK_EQ_U64(c.lag_frames, 5);

    /* MARIA's DMA fetches a graphics byte from $0280: a one-byte object
       whose graphics address is page 2 plus OFFSET 0, low byte $80. */
    static const uint8_t dl[] = {0x80u, 0x1Fu, 0x02u, 0x00u, 0u, 0u, 0u, 0u};
    memcpy(&m->ram[0x1900u - MUHTARI__RAM_BASE], dl, sizeof dl);
    m->maria.dl = 0x1900u;
    m->maria.offset = 0u;
    struct muhtari__line_stats st;
    memset(&st, 0, sizeof st);
    (void)muhtari__maria_dma_line(m, 1000u, 1454u, 300u, &st);
    CHECK_EQ_U64(st.bytes, 1);
    CHECK_EQ_U64(m->ports.input_read, 0);

    CHECK_EQ_U64(muhtari_power_cycle(m), MUHTARI_STATUS_OK);
    c = counters_of(m);
    CHECK_EQ_U64(c.frames, 0);
    CHECK_EQ_U64(c.lag_frames, 0);

    load_program(m, prog_swcha, sizeof prog_swcha);
    for (uint32_t f = 0; f < 4u; f++) {
        run(m, NULL);
    }
    c = counters_of(m);
    CHECK_EQ_U64(c.frames, 4);
    CHECK_EQ_U64(c.lag_frames, 0);

    load_program(m, prog_inpt4, sizeof prog_inpt4);
    run(m, NULL);
    run(m, NULL);
    c = counters_of(m);
    CHECK_EQ_U64(c.frames, 2);
    CHECK_EQ_U64(c.lag_frames, 0);

    CHECK_EQ_U64(muhtari_get_counters(NULL, &c), MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK_EQ_U64(muhtari_get_counters(m, NULL), MUHTARI_STATUS_ERR_ARGUMENT);
    muhtari_destroy(m);
}

static const struct {
    const char *name;
    void (*run)(void);
} cases[] = {
    {"api.size_tags", test_size_tags},
    {"audio.silence", test_silence},
    {"create.allocator", test_allocator},
    {"frame.pitch_336", test_pitch_336},
    {"frame.validate_first", test_validate_first},
    {"info", test_info},
    {"counters.frames_and_lag", test_counters_frames_and_lag},
    {"get_memory.spans", test_get_memory_spans},
    {"load.empty_refused", test_load_empty_refused},
    {"load.invalid_bios", test_load_invalid_bios},
    {"load.keeps_previous", test_load_keeps_previous},
    {"no_alloc_after_load", test_no_alloc_after_load},
    {"set_input.latched_at_vblank", test_set_input_latched_at_vblank},
    {"set_input.validates", test_set_input_validates},
    {"testcard.all_colours", test_all_colours},
    {"testcard.boundaries", test_boundaries},
    {"testcard.pinned_hash", test_pinned_hash},
    {"testcard.static", test_static},
};

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: muhtari-test-core <case>\n");
        return 2;
    }
    for (size_t i = 0; i < sizeof cases / sizeof cases[0]; i++) {
        if (strcmp(argv[1], cases[i].name) == 0) {
            cases[i].run();
            CHECK_DONE();
        }
    }
    fprintf(stderr, "muhtari-test-core: unknown case %s\n", argv[1]);
    return 2;
}
