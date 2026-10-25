/* Independent PAL numbers from docs/hardware/bus.md and audio.md. */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"
#include "audio_tables.h"
#include "../alloc.h"
#include "../cart/images.h"
#include "../check.h"

static uint8_t image[128u + 4096u];
static uint8_t picture[320u * 292u + 2u];
static int16_t pcm[963];

static muhtari *make(alloc_counts *counts)
{
    muhtari_config cfg = alloc_config(counts);
    muhtari *m = NULL;
    CHECK_EQ_U64(muhtari_create(&cfg, &m), MUHTARI_STATUS_OK);
    return m;
}

static muhtari_media cartridge(void)
{
    images_header(image, 3u, "Owned PAL", 4096u, 0u);
    image[57] = 1u;
    memset(image + 128u, 0xEA, 4096u);
    image[128] = 0x4Cu;
    image[129] = 0u;
    image[130] = 0xF0u;
    images_footer(image + 128u, 4096u);
    muhtari_media media = {0};
    media.size = (uint32_t)sizeof media;
    media.image = image;
    media.image_size = sizeof image;
    return media;
}

static void tracer(void)
{
    alloc_counts counts;
    muhtari *m = make(&counts);
    muhtari_media media = cartridge();
    muhtari_status status = muhtari_load(m, &media);
    CHECK_EQ_U64(status, MUHTARI_STATUS_OK);
    if (status != MUHTARI_STATUS_OK) {
        muhtari_destroy(m);
        return;
    }
    muhtari_info info = {0};
    info.size = (uint32_t)sizeof info;
    CHECK_EQ_U64(muhtari_get_info(m, &info), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(info.width, 320u);
    CHECK_EQ_U64(info.height, 292u);
    CHECK_EQ_U64(info.fps_num, 3546894u);
    CHECK_EQ_U64(info.fps_den, 71051u);
    CHECK_EQ_U64(info.par_num, 1843750u);
    CHECK_EQ_U64(info.par_den, 1773447u);
    CHECK_EQ_U64(info.max_audio_samples, 962u);
    CHECK_EQ_U64(m->bus.ticks, 140016u);
    CHECK_EQ_U64(m->frame_deadline, 281976u);
    memset(picture, 0xA5, sizeof picture);
    muhtari_frame io = {0};
    io.size = (uint32_t)sizeof io;
    io.video = picture + 1u;
    io.video_pitch = 320u;
    io.audio = pcm;
    io.audio_capacity = 962u;
    uint32_t allocations = counts.allocs;
    CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(io.audio_count, 960u);
    CHECK_EQ_U64(m->audio_rem, 336960u);
    CHECK_EQ_U64(picture[0], 0xA5u);
    CHECK_EQ_U64(picture[93441], 0xA5u);
    CHECK_EQ_U64(picture[93440], 0u);
    CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(io.audio_count, 962u);
    CHECK_EQ_U64(m->frame_start, 424078u);
    CHECK_EQ_U64(counts.allocs, allocations);
    muhtari_destroy(m);
}

/* Reference interpolation uses only the fixed kernel as input. Expectations
   never use a production timing/profile getter. */
static void coefficients(uint32_t phase, int32_t out[16])
{
    uint32_t row = phase * 64u / 591149u;
    uint32_t fraction = phase * 64u % 591149u;
    int32_t sum = 0;
    for (unsigned j = 0u; j < 16u; j++) {
        int32_t a = muhtari__synth_kernel[row][j];
        int32_t b = row < 63u ? muhtari__synth_kernel[row + 1u][j]
                              : (j != 0u ? muhtari__synth_kernel[0][j - 1u] : 0);
        int64_t product = (int64_t)(b - a) * fraction;
        int64_t rounded =
            product < 0 ? -((-product + 295574) / 591149) : (product + 295574) / 591149;
        out[j] = a + (int32_t)rounded;
        sum += out[j];
    }
    out[phase < 295574u ? 7u : 8u] += 8388608 - sum;
}

static int64_t step(uint32_t n, uint32_t tick)
{
    uint64_t position = (uint64_t)tick * 4000u;
    uint32_t q = (uint32_t)(position / 591149u);
    if (n < q)
        return 0;
    if (n - q >= 16u)
        return 8388608;
    int32_t c[16];
    coefficients((uint32_t)(position % 591149u), c);
    int64_t sum = 0;
    for (unsigned j = 0u; j <= n - q; j++)
        sum += c[j];
    return sum;
}

static void bounds(void)
{
    int64_t maximum = 0, positive = 0, negative = 0;
    for (uint32_t phase = 0u; phase < 591149u; phase++) {
        int32_t c[16], actual[16];
        coefficients(phase, c);
        muhtari__synth_coefficients_for_period(phase, 591149u, actual);
        CHECK(memcmp(c, actual, sizeof c) == 0);
        int64_t sum = 0;
        for (unsigned j = 0u; j < 16u; j++) {
            sum += c[j];
            int64_t magnitude = c[j] < 0 ? -(int64_t)c[j] : c[j];
            maximum = magnitude > maximum ? magnitude : maximum;
        }
        CHECK_EQ_U64(sum, 8388608);
    }
    for (uint32_t n = 100u; n < 4100u; n++) {
        uint32_t first = (uint32_t)((uint64_t)(n - 17u) * 591149u / 4000u);
        uint32_t last = (uint32_t)(((uint64_t)(n + 1u) * 591149u + 3999u) / 4000u - 1u);
        int64_t p = 0, minus = 0, previous = step(n, first);
        for (uint32_t tick = first; tick <= last; tick++) {
            int64_t next = step(n, tick + 1u);
            int64_t weight = previous - next;
            previous = next;
            if (weight >= 0)
                p += weight;
            else
                minus -= weight;
        }
        CHECK_EQ_U64(p - minus, 8388608);
        positive = p > positive ? p : positive;
        negative = minus > negative ? minus : negative;
    }
    CHECK_EQ_U64(maximum, 7283993);
    CHECK_EQ_U64(positive, 10865492);
    CHECK_EQ_U64(negative, 2476884);
    CHECK_EQ_U64(positive + negative, 13342376);
    CHECK_EQ_U64(MUHTARI__SYNTH_MAX_LEVEL, 18360);
    CHECK_EQ_U64(MUHTARI__TIA_CURRENT, 85);
    CHECK_EQ_U64(MUHTARI__POKEY_CURRENT, 342);
    CHECK(((positive + negative) * 18360 + 8388608) * 5 <= INT64_C(146018) * 8388608);
    CHECK_EQ_U64(INT64_C(16) * 150 * 18360 * maximum, INT64_C(320961867552000));
    CHECK(INT64_C(16) * 150 * 18360 * maximum < (INT64_C(1) << 49));
    CHECK(positive * 18360 < (INT64_C(1) << 38));
    CHECK(positive * 18360 * 128 < (INT64_C(1) << 45));
    CHECK(3 * positive * 18360 * 128 < (INT64_C(1) << 47));
    CHECK((INT64_C(1073741824) - 1) * 1070934451 < (INT64_C(1) << 60));
    CHECK_EQ_U64(591148u + 142102u * 4000u, 568999148u);
    CHECK_EQ_U64(591148u * 64u, 37833472u);
    CHECK_EQ_U64((20730u * 4000u + 591148u) / 591149u, 141u);
    CHECK_EQ_U64(MUHTARI__SYNTH_OVERSHOOT_TICKS, 20730u);
    CHECK((591148u + 142102u * 4000u) < (1u << 30));
    CHECK(591148u * 64u < (1u << 26));
    uint64_t current =
        (uint64_t)MUHTARI__MIX_Q24 * 85u + 2u * (uint64_t)muhtari__pokey_mix[824] * 342u;
    CHECK(current < (UINT64_C(1) << 34));
    CHECK(current * 18360u < (UINT64_C(1) << 49));
    CHECK(2u * (uint64_t)muhtari__pokey_mix[824] * 342u * MUHTARI__CART_GAIN_Q12 <
          (UINT64_C(1) << 49));
    CHECK(sizeof(((muhtari *)0)->maria.stats) / sizeof(struct muhtari__line_stats) >= 313u);
    CHECK(sizeof(((muhtari *)0)->audio_work) / sizeof(int16_t) >= 962u);
    CHECK(sizeof(((muhtari *)0)->synth.pcm) / sizeof(int16_t) >= 1103u);
}

static void lifecycle(void)
{
    alloc_counts counts;
    muhtari *m = make(&counts);
    muhtari_media media = cartridge();
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    media.region = MUHTARI_REGION_NTSC;
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    muhtari_info info = {0};
    info.size = (uint32_t)sizeof info;
    CHECK_EQ_U64(muhtari_get_info(m, &info), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(info.height, 242u);
    muhtari_destroy(m);
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    if (strcmp(argv[1], "tracer") == 0)
        tracer();
    else if (strcmp(argv[1], "bounds") == 0)
        bounds();
    else if (strcmp(argv[1], "lifecycle") == 0)
        lifecycle();
    else
        return 2;
    CHECK_DONE();
}
