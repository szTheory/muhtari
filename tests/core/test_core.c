/* The core library through its public header: muhtari-test-core <case>.
   Expected test-card values follow the card's rules as docs/ describes them,
   and the pinned hash was computed by an independent model of those rules,
   not by this code. */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "muhtari.h"
#include "sha256.h"
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

/* A counting allocator: records every call and the sizes it sees. */
typedef struct counts {
    uint32_t allocs, frees;
    size_t alloc_size, free_size;
    int fail; /* nonzero: alloc returns NULL */
} counts;

static void *count_alloc(void *user, size_t size)
{
    counts *c = (counts *)user;
    c->allocs++;
    c->alloc_size = size;
    return c->fail ? NULL : malloc(size);
}

static void count_free(void *user, void *ptr, size_t size)
{
    counts *c = (counts *)user;
    c->frees++;
    c->free_size = size;
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
   of them. */
#define TAIL 16u
typedef union tagged {
    muhtari_config config;
    muhtari_versions versions;
    muhtari_info info;
    muhtari_frame frame;
    uint8_t bytes[sizeof(muhtari_frame) + sizeof(muhtari_config) + TAIL];
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
    counts c;
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
    c.fail = 1;
    CHECK_EQ_U64(muhtari_create(&cfg, &m), MUHTARI_STATUS_ERR_NO_MEMORY);
    CHECK_EQ_U64(c.allocs, 1);
    CHECK(m == marker);

    c.fail = 0;
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
    CHECK_EQ_U64(v.api, 2);
    CHECK_EQ_U64(v.abi, 1);
    CHECK_EQ_U64(v.behaviour_revision, 1);
    muhtari_destroy(m);
}

/* After create the core never allocates: one alloc at create, none in
   LONG_RUN frames, one free at destroy. */
static void test_no_alloc_after_load(void)
{
    counts c;
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
    {"no_alloc_after_load", test_no_alloc_after_load},
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
