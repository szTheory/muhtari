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
#include "audio_tables.h"
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
static void test_audio_validate(void);
static void load_audio_program(muhtari *m);

static void test_validate_first(void)
{
    test_audio_validate();
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
/* Cumulative production step response; the 88 sample alignments exhaust
   integer tick positions since gcd(88,13125)=1. Summation by parts turns
   arbitrary bounded levels into finite differences of these steps. */
static int64_t synth_step(uint32_t n, uint32_t tick, const int32_t cumulative[13125][16])
{
    uint32_t q = tick * 88u / 13125u;
    uint32_t phase = tick * 88u % 13125u;
    if (n < q) {
        return 0;
    }
    return n - q >= 16u ? MUHTARI__SYNTH_Q23 : cumulative[phase][n - q];
}

static void test_synth_bound(void)
{
    static int32_t cumulative[13125][16];
    int64_t cmax = 0;
    for (uint32_t phase = 0; phase < 13125u; phase++) {
        int32_t c[16];
        int32_t sum = 0;
        muhtari__synth_coefficients(phase, c);
        for (unsigned j = 0; j < 16u; j++) {
            sum += c[j];
            cumulative[phase][j] = sum;
            int64_t magnitude = c[j] < 0 ? -(int64_t)c[j] : c[j];
            cmax = magnitude > cmax ? magnitude : cmax;
        }
        CHECK_EQ_U64(sum, 8388608);
    }
    int64_t positive = 0, negative = 0;
    muhtari *adverse = make();
    for (uint32_t n = 100u; n < 188u; n++) {
        int64_t p = 0, minus = 0;
        uint32_t first_tick = (n - 17u) * 13125u / 88u;
        uint32_t last_tick = ((n + 1u) * 13125u + 87u) / 88u - 1u;
        for (uint32_t tick = first_tick; tick <= last_tick; tick++) {
            int64_t w = synth_step(n, tick, cumulative) - synth_step(n, tick + 1u, cumulative);
            if (w >= 0) {
                p += w;
            } else {
                minus -= w;
            }
        }
        CHECK_EQ_U64(p - minus, MUHTARI__SYNTH_Q23);
        positive = p > positive ? p : positive;
        negative = minus > negative ? minus : negative;
        /* Construct each extremizing 0/M level sequence and pass it
           through production event ordering, ring and blocker as well. */
        for (unsigned sign = 0; sign < 2u; sign++) {
            muhtari__synth_reset(adverse, 0u, 0u);
            for (uint32_t tick = first_tick; tick <= last_tick; tick++) {
                int64_t w = synth_step(n, tick, cumulative) - synth_step(n, tick + 1u, cumulative);
                uint16_t raw = (uint16_t)((sign == 0u ? w > 0 : w < 0) ? 15u : 0u);
                muhtari__synth_level(adverse, tick, 0u, raw);
                muhtari__synth_level(adverse, tick, 1u, raw);
            }
            CHECK_EQ_U64(muhtari__synth_render(adverse, last_tick + 1u, NULL), n + 1u);
            CHECK_EQ_U64(adverse->synth.integrator_q23,
                         (sign == 0u ? p : -minus) * MUHTARI__SYNTH_MAX_LEVEL);
            CHECK_EQ_U64(adverse->synth.clipped_samples, 0);
        }
    }
    muhtari_destroy(adverse);
    CHECK_EQ_U64(cmax, 7283992);
    CHECK_EQ_U64(positive, 10865505);
    CHECK_EQ_U64(negative, 2476897);
    CHECK_EQ_U64(positive + negative, 13342402);
    /* 29203.6 is below 32767*10^(-1/20). One whole PCM LSB covers both
       the blocker feedback's <1/(1-R) Q30 units and final half-LSB. */
    CHECK(((positive + negative) * MUHTARI__SYNTH_MAX_LEVEL + MUHTARI__SYNTH_Q23) * 5 <=
          INT64_C(146018) * MUHTARI__SYNTH_Q23);
    int64_t pending = INT64_C(16) * 150 * MUHTARI__SYNTH_MAX_LEVEL * cmax;
    CHECK_EQ_U64(pending, INT64_C(320961823488000));
    CHECK(pending < (INT64_C(1) << 49));
    CHECK(pending + positive * MUHTARI__SYNTH_MAX_LEVEL < (INT64_C(1) << 50));
    CHECK(positive * MUHTARI__SYNTH_MAX_LEVEL < (INT64_C(1) << 38));
    CHECK(3 * positive * MUHTARI__SYNTH_MAX_LEVEL * 128 < (INT64_C(1) << 47));
    CHECK_EQ_U64(13124u * MUHTARI__SYNTH_PHASES, 839936u);
    CHECK(13124u * MUHTARI__SYNTH_PHASES < (1u << 20));
    CHECK_EQ_U64(MUHTARI__SYNTH_OVERSHOOT_TICKS, 20730u);
    CHECK_EQ_U64(MUHTARI__SYNTH_CARRY_SAMPLES, 139u);
    CHECK_EQ_U64(MUHTARI__SYNTH_PCM_CAPACITY, 940u);
    CHECK(positive * MUHTARI__SYNTH_MAX_LEVEL * 128 < (INT64_C(1) << 45));
    CHECK((MUHTARI__SYNTH_Q30 - 1) * INT64_C(1070934451) < (INT64_C(1) << 60));
}

/* Independent test-only exponential series, avoiding a new analysis library.
   Forty terms on [-2.451,0] make error far below one Q24 table LSB. */
static double synth_test_exp(double x)
{
    double total = 1, term = 1;
    for (unsigned k = 1; k <= 40u; k++) {
        term *= x / k;
        total += term;
    }
    return total;
}

static void test_synth_mix(void)
{
    CHECK_EQ_U64(sizeof muhtari__synth_kernel, 4096);
    uint64_t max_current = (uint64_t)MUHTARI__MIX_Q24 * MUHTARI__TIA_CURRENT +
                           2u * (uint64_t)muhtari__pokey_mix[824] * MUHTARI__POKEY_CURRENT;
    CHECK(max_current < (UINT64_C(1) << 34));
    CHECK(max_current * MUHTARI__SYNTH_MAX_LEVEL < (UINT64_C(1) << 49));
    CHECK(2u * (uint64_t)muhtari__pokey_mix[824] * MUHTARI__POKEY_CURRENT * MUHTARI__CART_GAIN_Q12 <
          (UINT64_C(1) << 49));
    for (unsigned row = 0; row < MUHTARI__SYNTH_PHASES; row++) {
        int32_t sum = 0;
        for (unsigned tap = 0; tap < 16u; tap++) {
            sum += muhtari__synth_kernel[row][tap];
        }
        CHECK_EQ_U64(sum, 8388608);
    }
    CHECK_EQ_U64(muhtari__synth_kernel[0][15], 0);
    CHECK_EQ_U64(sizeof muhtari__tia_mix / sizeof muhtari__tia_mix[0], 31);
    CHECK_EQ_U64(sizeof muhtari__pokey_mix / sizeof muhtari__pokey_mix[0], 825);
    for (unsigned n = 0; n <= 30u; n++) {
        uint32_t expected =
            (uint32_t)(((uint64_t)2u * n * MUHTARI__MIX_Q24 + (n + 30u) / 2u) / (n + 30u));
        CHECK_EQ_U64(muhtari__tia_mix[n], expected);
        if (n > 0u) {
            CHECK(muhtari__tia_mix[n] > muhtari__tia_mix[n - 1u]);
        }
    }
    CHECK_EQ_U64(muhtari__tia_mix[30], MUHTARI__MIX_Q24);
    CHECK_EQ_U64(muhtari__pokey_mix[0], 0);
    for (unsigned n = 0; n <= 824u; n++) {
        double x = (double)n / 824;
        double curve =
            2.171 * (x <= 0.14 ? x : 0.14 + (1 - synth_test_exp(-2.85 * (x - 0.14))) / 2.85);
        uint32_t expected = (uint32_t)(curve * MUHTARI__MIX_Q24 + 0.5);
        CHECK_EQ_U64(muhtari__pokey_mix[n], expected);
        if (n > 0u) {
            CHECK(muhtari__pokey_mix[n] > muhtari__pokey_mix[n - 1u]);
        }
    }
    CHECK_EQ_U64(muhtari__pokey_mix[206], 8538596);
    CHECK_EQ_U64(muhtari__pokey_mix[824], 16777645);
    /* At the adopted node, one maximum channel is 2.1 times TIA full
       current (unmeasured cartridge loading remains model uncertainty). */
    uint64_t ratio = (uint64_t)muhtari__pokey_mix[206] * MUHTARI__POKEY_CURRENT * 1000u /
                     ((uint64_t)muhtari__tia_mix[30] * MUHTARI__TIA_CURRENT);
    CHECK(ratio >= 2047u && ratio <= 2049u);

    muhtari *m = make();
    for (uint8_t chips = 0; chips <= 2u; chips++) {
        muhtari__synth_reset(m, 0u, chips);
        for (unsigned source = 2u; source < 2u + 4u * chips; source++) {
            muhtari__synth_level(m, 0u, source, 206u);
        }
        CHECK_EQ_U64(muhtari__synth_render(m, 1u, NULL), 0);
        CHECK_EQ_U64(m->synth.mixed_level, chips == 0u ? 0u : chips == 1u ? 14705u : 16331u);
        muhtari__synth_level(m, 1u, 0u, 15u);
        muhtari__synth_level(m, 1u, 1u, 15u);
        CHECK_EQ_U64(muhtari__synth_render(m, 2u, NULL), 0);
        CHECK_EQ_U64(m->synth.mixed_level, MUHTARI__SYNTH_MAX_LEVEL);

        /* Adverse full-range one-tick transitions, then slower ringing
           steps, including long DC holds and the ring-wrap boundaries. */
        for (uint32_t tick = 2u; tick < 100000u; tick++) {
            uint32_t period = tick < 12000u ? 1u : tick < 40000u ? 149u : 4093u;
            if (tick % period == 0u) {
                uint16_t high = (uint16_t)((tick / period) & 1u);
                muhtari__synth_level(m, tick, 0u, (uint16_t)(high * 15u));
                muhtari__synth_level(m, tick, 1u, (uint16_t)(high * 15u));
                for (unsigned source = 2u; source < 2u + 4u * chips; source++) {
                    muhtari__synth_level(m, tick, source, (uint16_t)(high * 206u));
                }
            }
            if (tick % 13125u == 0u) {
                CHECK_EQ_U64(muhtari__synth_render(m, tick, NULL), 88u);
            }
        }
        (void)muhtari__synth_render(m, 100000u, NULL);
        CHECK_EQ_U64(m->synth.clipped_samples, 0);
        CHECK(m->synth.pre_clip_peak > 10000u && m->synth.pre_clip_peak <= 29203u);
        CHECK(m->synth.kernel_adds_per_sample <= 150u);
    }
    muhtari_destroy(m);
}

static void test_cart_audio(void);

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

    /* Exercise the shipped reconstruction separately from the silent card. */
    muhtari__synth_reset(m, 0u, 0u);
    muhtari__synth_level(m, 0u, 0u, 15u);
    uint32_t reconstructed = muhtari__synth_render(m, 13125u, audio);
    CHECK_EQ_U64(reconstructed, 88u);
    int nonzero = 0;
    for (uint32_t i = 0; i < reconstructed; i++) {
        nonzero |= audio[i] != 0;
    }
    CHECK(nonzero);
    /* Half the TIA DAC sum produces 2n/(n+30)=2/3 of full current,
       rather than half the PCM level (A7800-AUDIO section 3). */
    CHECK_EQ_U64(m->synth.mixed_level, 12240);
    CHECK_EQ_U64(muhtari__synth_round(-5, 2), -3);
    CHECK_EQ_U64(muhtari__synth_round(-4, 3), -1);
    CHECK_EQ_U64(muhtari__synth_round(5, 2), 3);
    int16_t continuous[176], partitioned[176];
    muhtari *other = make();
    muhtari__synth_reset(m, 0u, 0u);
    muhtari__synth_reset(other, 0u, 0u);
    static const uint32_t events[] = {0u, 149u, 150u, 2387u, 13124u, 13125u, 19000u};
    uint32_t written = 0;
    for (unsigned i = 0; i < sizeof events / sizeof events[0]; i++) {
        uint16_t level = (uint16_t)((i & 1u) == 0u ? 15u : 0u);
        muhtari__synth_level(m, events[i], 0u, level);
        muhtari__synth_level(other, events[i], 0u, level);
        written += muhtari__synth_render(other, events[i], partitioned + written);
    }
    CHECK_EQ_U64(muhtari__synth_render(m, 26250u, continuous), 176u);
    written += muhtari__synth_render(other, 26250u, partitioned + written);
    CHECK_EQ_U64(written, 176u);
    CHECK(memcmp(continuous, partitioned, sizeof continuous) == 0);
    CHECK(memcmp(m->synth.impulse, other->synth.impulse, sizeof m->synth.impulse) == 0);
    CHECK_EQ_U64(m->synth.integrator_q23, other->synth.integrator_q23);
    CHECK_EQ_U64(m->synth.blocker_q30, other->synth.blocker_q30);
    muhtari_destroy(other);
    /* CPU catch-up may queue future PCM. Drain only the logical prefix;
       discarded output must leave byte-identical future samples. */
    other = make();
    muhtari__synth_reset(m, 0u, 0u);
    muhtari__synth_reset(other, 0u, 0u);
    uint32_t future = 13125u + MUHTARI__SYNTH_OVERSHOOT_TICKS;
    for (uint32_t tick = 0; tick <= future; tick += 17u) {
        uint16_t raw = (uint16_t)((tick / 17u & 1u) * 15u);
        muhtari__synth_level(m, tick, 0u, raw);
        muhtari__synth_level(other, tick, 0u, raw);
    }
    CHECK_EQ_U64(muhtari__synth_render(m, 13125u, continuous), 88);
    CHECK_EQ_U64(muhtari__synth_render(other, 13125u, NULL), 88);
    CHECK(m->synth.carry_high_water > 0u);
    CHECK(m->synth.carry_high_water <= MUHTARI__SYNTH_CARRY_SAMPLES);
    uint32_t tail = muhtari__synth_render(m, future, continuous);
    CHECK_EQ_U64(muhtari__synth_render(other, future, partitioned), tail);
    CHECK(memcmp(continuous, partitioned, (size_t)tail * sizeof continuous[0]) == 0);
    muhtari_destroy(other);
    /* A boundary event is still pending after the completed half-open
       prefix: all 88 samples are silent, the next interval starts its step. */
    muhtari__synth_reset(m, 0u, 0u);
    muhtari__synth_level(m, 13125u, 0u, 15u);
    CHECK_EQ_U64(muhtari__synth_render(m, 13125u, continuous), 88u);
    CHECK_EQ_U64(m->synth.pending, 1u);
    CHECK_EQ_U64(m->synth.mixed_level, 0u);
    for (unsigned i = 0u; i < 88u; i++) {
        CHECK_EQ_U64(continuous[i], 0u);
    }
    CHECK_EQ_U64(muhtari__synth_render(m, 13275u, partitioned), 1u);
    CHECK_EQ_U64(m->synth.pending, 0u);
    CHECK(partitioned[0] != 0);
    test_synth_bound();
    test_synth_mix();
    test_cart_audio();
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
    CHECK(v.behaviour_revision > 0u);
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
    load_audio_program(m);
    CHECK_EQ_U64(c.allocs, 2);
    for (uint32_t f = 0; f < LONG_RUN; f++) {
        run(m, NULL);
    }
    CHECK(m->synth.pre_clip_peak > 0u);
    CHECK_EQ_U64(c.allocs, 2);
    CHECK_EQ_U64(c.frees, 0);
    muhtari_destroy(m);
    CHECK_EQ_U64(c.allocs, 2);
    CHECK_EQ_U64(c.frees, 2);
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

/* A declared cartridge, executed by SALLY, must reach the public PCM.
   POKEY registers/windows: cartridge notes section 2.1 and pokey.md. */
static void test_cart_audio(void)
{
    static const uint16_t types[] = {0x0001u, 0x8000u, 0x0400u, 0x0040u};
    static const uint16_t windows[] = {0x4000u, 0x0800u, 0x0440u, 0x0450u};
    for (unsigned k = 0u; k < 4u; k++) {
        uint16_t at = windows[k];
        uint8_t program[] = {
            0xA9, 7, 0x85, 1, /* lock 7800 */
            0xA9, 3, 0x8D, (uint8_t)(at + 15u), (uint8_t)(at >> 8),
            0xA9, 20, 0x8D, (uint8_t)at, (uint8_t)(at >> 8),
            0xA9, 0xAF, 0x8D, (uint8_t)(at + 1u), (uint8_t)(at >> 8),
            0xAD, (uint8_t)(at + 14u), (uint8_t)(at >> 8),
            0x8D, 0, 0x18, 0x4C, 0x13, 0xF0
        };
        muhtari *m = make();
        make_cart(cart, program, sizeof program, 0u);
        cart[53] = (uint8_t)(types[k] >> 8);
        cart[54] = (uint8_t)types[k];
        muhtari_media media = media_for(cart, sizeof cart);
        CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
        int nonzero = 0;
        for (unsigned f = 0u; f < 3u; f++) {
            muhtari_frame io;
            frame_io(&io, NULL, 0u, audio, AUDIO_CAP);
            CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_OK);
            for (unsigned i = 0u; i < io.audio_count; i++) nonzero |= audio[i] != 0;
        }
        CHECK(nonzero);
        CHECK_EQ_HEX(m->ram[0], 255u);
        uint16_t mirror = k == 0u ? 0x7FFEu : k == 1u ? 0x0FFEu : (uint16_t)(at + 14u);
        CHECK_EQ_HEX(muhtari__bus_read(m, mirror), 255u);
        muhtari__bus_write(m, (uint16_t)(mirror - 13u), 0x1Fu);
        CHECK_EQ_U64(m->synth.raw[k == 2u ? 6u : 2u], 206u);
        CHECK_EQ_U64(m->synth.raw[k == 2u ? 2u : 6u], 0u);
        struct muhtari__pokey before[2];
        memcpy(before, m->pokey, sizeof before);
        (void)muhtari__bus_peek(m, (uint16_t)(at + 10u));
        (void)muhtari__cart_read(m, (uint16_t)(at + 10u), 1);
        CHECK(memcmp(before, m->pokey, sizeof before) == 0);
        CHECK_EQ_U64(muhtari_power_cycle(m), MUHTARI_STATUS_OK);
        CHECK_EQ_U64(m->synth.pokeys, 1u);
        muhtari_destroy(m);
    }
    /* Readable ROM overlaps a write-only POKEY; DMA/peek remain pure. */
    static uint8_t overlap[IMAGES_HEADER + 49152u];
    images_header(overlap, 3u, "Overlap", 49152u, 1u);
    memset(overlap + IMAGES_HEADER, 0x5Au, 49152u);
    overlap[IMAGES_HEADER + 49148u] = 0u;
    overlap[IMAGES_HEADER + 49149u] = 0xF0u;
    muhtari *m = make();
    muhtari_media media = media_for(overlap, sizeof overlap);
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    muhtari__bus_write(m, 0x4001u, 0x1Fu);
    CHECK_EQ_U64(m->synth.raw[2], 206u);
    CHECK_EQ_HEX(muhtari__bus_read(m, 0x400Eu), 0x5Au);
    CHECK_EQ_HEX(muhtari__bus_peek(m, 0x400Eu), 0x5Au);
    CHECK_EQ_HEX(muhtari__cart_read(m, 0x400Eu, 1), 0x5Au);
    /* v4 permits explicit RAM beside POKEY: writes select the chip,
       reads select RAM. v3's conflicting RAM/POKEY pair drops POKEY. */
    make_cart(cart, prog_idle, sizeof prog_idle, 0u);
    cart[0] = 4u;
    cart[65] = 1u;
    cart[67] = 5u;
    cart[69] = 3u;
    media = media_for(cart, sizeof cart);
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(m->cart.irq_mask, 3u);
    m->cart.ram[1] = 0x63u;
    muhtari__bus_write(m, 0x4001u, 0x1Fu);
    CHECK_EQ_U64(m->synth.raw[2], 206u);
    CHECK_EQ_HEX(muhtari__bus_read(m, 0x4001u), 0x63u);
    cart[64] = 99u; /* ignored v4 retains raw fields, never wiring */
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(m->cart.info.a78_irq, 3u);
    CHECK_EQ_U64(m->cart.irq_mask, 0u);
    CHECK_EQ_U64(m->synth.pokeys, 0u);
    cart[64] = 0u;
    cart[65] = 0u;
    cart[67] = 1u; /* lone440 retains secondary wiring and source */
    cart[69] = 2u;
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(m->cart.irq_mask, 2u);
    CHECK_EQ_U64(m->synth.pokeys, 1u);
    muhtari__bus_write(m, 0x0441u, 0x1Fu);
    CHECK_EQ_U64(m->synth.raw[6], 206u);
    cart[67] = 3u;
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(m->synth.pokeys, 2u);
    muhtari__bus_write(m, 0x0451u, 0x1Fu);
    muhtari__bus_write(m, 0x0443u, 0x18u);
    CHECK_EQ_U64(m->synth.raw[2], 206u);
    CHECK_EQ_U64(m->synth.raw[7], 112u);
    CHECK_EQ_U64(m->synth.raw[6], 0u);
    CHECK_EQ_HEX(muhtari__bus_read(m, 0x045Eu), 255u);
    CHECK_EQ_HEX(muhtari__bus_read(m, 0x044Eu), 255u);
    make_cart(cart, prog_idle, sizeof prog_idle, 0u);
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(m->cart.irq_mask, 0u);
    CHECK_EQ_U64(m->synth.pokeys, 0u);
    muhtari_destroy(m);
}

static const uint8_t tone[] = {
    0xA9, 0x07, 0x85, 0x01, /* lock the 7800 decode */
    0xA9, 0x04, 0x85, 0x15, /* AUDC0 div2 */
    0xA9, 0x05, 0x85, 0x17, /* AUDF0 */
    0xA9, 0x0F, 0x85, 0x19, /* AUDV0 */
    0x4C, 0x10, 0xF0        /* JMP $F010 */
};

static void load_audio_program(muhtari *m)
{
    load_program(m, tone, sizeof tone);
}

/* Validate with audible chip/filter history, pending input and future carry.
   A byte comparison includes every unobserved audio field as well. */
static void test_audio_validate(void)
{
    muhtari *m = make();
    load_audio_program(m);
    run(m, NULL);
    CHECK(m->synth.pre_clip_peak > 0u);
    muhtari_input in;
    memset(&in, 0, sizeof in);
    in.size = (uint32_t)sizeof in;
    in.switches = MUHTARI_SWITCH_RESET;
    CHECK_EQ_U64(muhtari_set_input(m, &in), MUHTARI_STATUS_OK);
    struct muhtari before = *m;
    const uint32_t count = (m->audio_rem + TICKS_PER_FRAME * 88u) / 13125u;
    for (unsigned bad = 0u; bad < 8u; bad++) {
        muhtari_frame io;
        frame_io(&io, video, WIDTH, audio, count);
        io.audio_count = COUNT_SENTINEL;
        memset(video, PAD_BYTE, sizeof video);
        for (unsigned i = 0u; i <= AUDIO_CAP; i++) {
            audio[i] = AUDIO_SENTINEL;
        }
        muhtari_status want = MUHTARI_STATUS_ERR_ARGUMENT;
        if (bad == 0u) {
            CHECK_EQ_U64(muhtari_run_frame(m, NULL), want);
        } else {
            if (bad == 1u) {
                io.size = 4u;
                want = MUHTARI_STATUS_ERR_STRUCT_SIZE;
            } else if (bad == 2u) {
                io.size = UINT32_MAX;
                want = MUHTARI_STATUS_ERR_STRUCT_SIZE;
            } else if (bad == 3u || bad == 4u) {
                io.video_pitch = bad == 3u ? WIDTH - 1u : 65537u;
            } else if (bad == 5u || bad == 6u) {
                io.audio_capacity = bad == 5u ? 0u : count - 1u;
                want = MUHTARI_STATUS_ERR_BUFFER_TOO_SMALL;
            }
            CHECK_EQ_U64(muhtari_run_frame(bad == 7u ? NULL : m, &io), want);
            CHECK_EQ_U64(io.audio_count, bad == 1u || bad == 2u || bad == 7u ? COUNT_SENTINEL : 0u);
        }
        CHECK(memcmp(m, &before, sizeof before) == 0);
        for (unsigned i = 0u; i <= AUDIO_CAP; i++) {
            CHECK_EQ_U64(audio[i], AUDIO_SENTINEL);
        }
        for (unsigned i = 0u; i < sizeof video; i++) {
            CHECK_EQ_U64(video[i], PAD_BYTE);
        }
    }
    muhtari_frame io;
    frame_io(&io, NULL, 0u, audio, count);
    CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(io.audio_count, count);
    CHECK_EQ_U64(audio[count], AUDIO_SENTINEL);
    CHECK(m->bus.ticks > before.bus.ticks);
    muhtari_destroy(m);
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

/* A SuperGame image built in memory: an A78 version 3 header with the cart
   type given over 8 banks of 16 KiB. Each bank starts with its own number;
   the last bank, fixed at $C000, runs program at $C000. */
#define SG_BANKS 8u
#define SG_BODY (SG_BANKS * 16384u)
static uint8_t sg_image[IMAGES_HEADER + SG_BODY];

static void make_sg(uint16_t cart_type, const uint8_t *program, size_t n)
{
    images_header(sg_image, 3u, "SuperGame", SG_BODY, cart_type);
    uint8_t *body = sg_image + IMAGES_HEADER;
    memset(body, 0xEA, SG_BODY); /* NOP */
    for (uint32_t b = 0; b < SG_BANKS; b++) {
        body[b * 16384u] = (uint8_t)b;
    }
    memcpy(body + SG_BODY - 16384u + 1u, program, n);
    body[SG_BODY - 4u] = 0x01u; /* reset vector $C001 */
    body[SG_BODY - 3u] = 0xC0u;
}

/* Increments $4000 forever: the cartridge RAM's first byte. */
static const uint8_t prog_cart_ram[] = {0xEE, 0x00, 0x40, 0x4C, 0x01, 0xC0}; /* INC $4000 */

/* A SuperGame cartridge is one allocation of its ROM and RAM, freed with
   the same size; a refused SuperGame image allocates nothing. The banks
   follow the board: bank 0 at power-on, the value modulo the bank count,
   the last bank at $C000, the second-to-last at $4000 without RAM; a
   banked-RAM board takes its RAM half from D5, and a linear board with
   RAM maps it at $4000. */
static void test_load_supergame_one_allocation(void)
{
    alloc_counts c;
    muhtari_config cfg = alloc_config(&c);
    muhtari *m = NULL;
    CHECK_EQ_U64(muhtari_create(&cfg, &m), MUHTARI_STATUS_OK);

    make_sg(0x4002u, prog_idle, 0u); /* SuperGame with halt-banked RAM: refused */
    muhtari_media media = media_for(sg_image, sizeof sg_image);
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD);
    CHECK_EQ_U64(c.allocs, 1);
    CHECK(m->cart.rom == NULL);

    make_sg(0x0006u, prog_idle, 0u); /* SuperGame with 16 KiB RAM */
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(c.allocs, 2);
    CHECK_EQ_U64(c.alloc_size, SG_BODY + 16384u);
    CHECK(m->cart.ram == m->cart.rom + SG_BODY);
    CHECK_EQ_HEX(muhtari__bus_peek(m, 0x4000u), 0x00);
    CHECK_EQ_HEX(muhtari__bus_peek(m, 0x8000u), 0x00);
    CHECK_EQ_HEX(muhtari__bus_peek(m, 0xC000u), SG_BANKS - 1u);
    muhtari__bus_write(m, 0x8000u, 3u);
    CHECK_EQ_HEX(muhtari__bus_peek(m, 0x8000u), 0x03);
    muhtari__bus_write(m, 0xBFFFu, 0xFFu); /* 255 mod 8 */
    CHECK_EQ_HEX(muhtari__bus_peek(m, 0x8000u), 0x07);
    muhtari__bus_write(m, 0x7FFFu, 0x5Au);
    CHECK_EQ_HEX(m->cart.ram[0x3FFF], 0x5A);
    CHECK_EQ_U64(muhtari_power_cycle(m), MUHTARI_STATUS_OK);
    CHECK_EQ_HEX(muhtari__bus_peek(m, 0x8000u), 0x00);
    CHECK_EQ_HEX(m->cart.ram[0x3FFF], 0x00);

    make_sg(0x0002u, prog_idle, 0u); /* plain SuperGame: no RAM */
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(c.allocs, 3);
    CHECK_EQ_U64(c.frees, 1);
    CHECK_EQ_U64(c.free_size, SG_BODY + 16384u);
    CHECK_EQ_U64(c.alloc_size, SG_BODY);
    CHECK(m->cart.ram == NULL);
    CHECK_EQ_HEX(muhtari__bus_peek(m, 0x4000u), SG_BANKS - 2u);

    make_sg(0x0022u, prog_idle, 0u); /* SuperGame with banked RAM */
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(c.alloc_size, SG_BODY + 32768u);
    muhtari__bus_write(m, 0x4000u, 0x11u);
    muhtari__bus_write(m, 0x8000u, 0x21u); /* bank 1, RAM half 1 */
    CHECK_EQ_HEX(muhtari__bus_peek(m, 0x8000u), 0x01);
    CHECK_EQ_HEX(muhtari__bus_peek(m, 0x4000u), 0x00);
    muhtari__bus_write(m, 0x4000u, 0x22u);
    muhtari__bus_write(m, 0x8000u, 0x01u);
    CHECK_EQ_HEX(muhtari__bus_peek(m, 0x4000u), 0x11);
    CHECK_EQ_HEX(m->cart.ram[0x4000], 0x22);

    /* RAM without SuperGame is a linear board: here 32 KiB, banks 6 and 7. */
    make_sg(0x0002u, prog_idle, 0u);
    images_header(sg_image, 3u, "Linear RAM", 32768u, 0x0004u);
    memcpy(sg_image + IMAGES_HEADER, sg_image + IMAGES_HEADER + SG_BODY - 32768u, 32768u);
    media = media_for(sg_image, IMAGES_HEADER + 32768u);
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(c.alloc_size, 32768u + 16384u);
    CHECK_EQ_HEX(muhtari__bus_peek(m, 0x8000u), SG_BANKS - 2u);
    muhtari__bus_write(m, 0x4001u, 0x77u);
    CHECK_EQ_HEX(muhtari__bus_peek(m, 0x4001u), 0x77);

    uint32_t allocs = c.allocs;
    muhtari_destroy(m);
    CHECK_EQ_U64(c.allocs, allocs);
    CHECK_EQ_U64(c.frees, allocs);
    CHECK_EQ_U64(c.free_size, sizeof *m);
}

/* CART_RAM is the board's 16 KiB at $4000: a live, stable span equal to
   what the CPU reads there. */
static void test_get_memory_cart_ram(void)
{
    muhtari *m = make();
    make_sg(0x0006u, prog_cart_ram, sizeof prog_cart_ram);
    muhtari_media media = media_for(sg_image, sizeof sg_image);
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    muhtari_span ram = span_of(m, MUHTARI_MEMORY_CART_RAM, MUHTARI_STATUS_OK);
    CHECK(ram.data != NULL);
    CHECK_EQ_U64(ram.bytes, 16384);
    run(m, NULL);
    run(m, NULL);
    muhtari_span again = span_of(m, MUHTARI_MEMORY_CART_RAM, MUHTARI_STATUS_OK);
    CHECK(again.data == ram.data);
    CHECK(ram.data[0] != 0u);
    CHECK_EQ_HEX(ram.data[0], muhtari__bus_peek(m, 0x4000u));
    ram.data[0x3FFF] = 0xA5u;
    CHECK_EQ_HEX(muhtari__bus_peek(m, 0x7FFFu), 0xA5);
    CHECK_EQ_U64(muhtari_power_cycle(m), MUHTARI_STATUS_OK);
    muhtari_span cycled = span_of(m, MUHTARI_MEMORY_CART_RAM, MUHTARI_STATUS_OK);
    CHECK(cycled.data == ram.data);
    CHECK_EQ_U64(cycled.bytes, 16384);
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
    {"get_memory.cart_ram", test_get_memory_cart_ram},
    {"get_memory.spans", test_get_memory_spans},
    {"load.empty_refused", test_load_empty_refused},
    {"load.invalid_bios", test_load_invalid_bios},
    {"load.keeps_previous", test_load_keeps_previous},
    {"load.supergame_one_allocation", test_load_supergame_one_allocation},
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
