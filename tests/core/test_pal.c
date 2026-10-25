/* Independent PAL numbers from docs/hardware/bus.md and audio.md. */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"
#include "audio_tables.h"
#include "cartdb.h"
#include "../alloc.h"
#include "../cart/images.h"
#include "../check.h"

static uint8_t image[128u + 4096u];
static uint8_t picture[320u * 292u + 2u];
static int16_t pcm[963];

static void snapshot(muhtari *m)
{
    CHECK_EQ_U64(m->cpu.pc, 0xF000u);
    CHECK_EQ_U64(m->cpu.s, 0x16u);
    CHECK_EQ_U64(m->cpu.x, 0x16u);
    CHECK_EQ_U64(m->cpu.a, 0x60u);
    CHECK_EQ_U64(m->cpu.y, 0u);
    CHECK_EQ_U64(m->cpu.p, 0x2Cu);
    CHECK_EQ_U64(m->bus.inptctrl, 6u);
    CHECK_EQ_U64(m->bus.halt_gate_writes, 2u);
    CHECK_EQ_U64(m->maria.reg[0x1Cu], 0x60u);
    CHECK_EQ_U64(m->maria.reg[0x0Cu], 0x27u);
    CHECK_EQ_U64(m->maria.reg[0x10u], 7u);
    CHECK_EQ_U64(m->maria.dl, 0u);
    CHECK_EQ_U64(m->maria.dll, 0u);
    CHECK_EQ_U64(m->maria.offset, 0u);
    CHECK_EQ_U64(m->maria.write_mode, 0u);
    CHECK_EQ_U64(m->bus.ticks, 140016u);
    CHECK_EQ_U64(m->frame_start, 140016u);
    CHECK_EQ_U64(m->frame_deadline, 281976u);
    CHECK_EQ_U64(m->synth.origin, 140016u);
    CHECK(m->tia.next_tick > 140016u);
    for (size_t i = 0u; i < sizeof m->ram; i++) {
        int filled = (i >= 0x800u && i < 0x840u) || (i >= 0x900u && i < 0x940u);
        CHECK_EQ_U64(m->ram[i], filled ? 255u : 0u);
    }
    for (size_t i = 0u; i < sizeof m->riot.ram; i++)
        CHECK_EQ_U64(m->riot.ram[i], 0u);
}

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
    snapshot(m);
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

static void through(muhtari *m, uint64_t tick)
{
    while (muhtari__maria_next_event(m) <= tick)
        muhtari__maria_event(m);
    m->bus.ticks = tick;
}

static void edges(muhtari *m)
{
    memset(&m->maria, 0, sizeof m->maria);
    m->bus.ticks = 0u;
    m->maria.reg[0x1Cu] = 0x40u;
    m->maria.reg[0x0Cu] = 0x18u;
    m->maria.reg[0x10u] = 0u;
    m->ram[0] = 0u;
    m->ram[1] = 0x19u;
    m->ram[2] = 0u;
    muhtari__maria_start(m);
    through(m, 7305u);
    CHECK_EQ_U64(m->maria.mstat, 0x80u);
    through(m, 7306u);
    CHECK_EQ_U64(m->maria.mstat, 0u);
    CHECK_EQ_U64(m->maria.dl, 0u);
    through(m, 7307u);
    CHECK_EQ_U64(m->maria.dll, 0x1803u);
    CHECK_EQ_U64(m->maria.dl, 0x1900u);
    through(m, 7308u);
    CHECK_EQ_U64(m->maria.mstat, 0u);
    m->maria.reg[0x1Cu] = 0x60u;
    m->maria.stats[308].flags = 0xBEEFu;
    m->maria.stats[309].flags = 0xBEEFu;
    through(m, 139859u);
    CHECK_EQ_U64(m->maria.stats[308].flags, 0xBEEFu);
    through(m, 139860u);
    CHECK_EQ_U64(m->maria.stats[308].flags, MUHTARI__LINE_DMA_OFF);
    through(m, 139861u);
    CHECK_EQ_U64(m->maria.mstat, 0u);
    through(m, 139873u);
    CHECK_EQ_U64(m->maria.mstat, 0u);
    through(m, 139874u);
    CHECK_EQ_U64(m->maria.mstat, 0x80u);
    through(m, 139875u);
    CHECK_EQ_U64(m->maria.mstat, 0x80u);
    through(m, 140314u);
    CHECK_EQ_U64(m->maria.stats[309].flags, 0xBEEFu);
    through(m, 142101u);
    CHECK_EQ_U64(m->maria.origin, 0u);
    through(m, 142102u);
    CHECK_EQ_U64(m->maria.origin, 142102u);
    CHECK_EQ_U64(m->maria.event_line, 1u);
    through(m, 142103u);
    CHECK_EQ_U64(m->maria.origin, 142102u);

    /* Pixel319 of raster307 belongs to the old line before LRC swaps. */
    memset(&m->maria, 0, sizeof m->maria);
    m->bus.ticks = 139831u;
    m->maria.rows = picture + 1u;
    m->maria.pitch = 320u;
    m->maria.reg[1] = 0xA7u;
    m->maria.line_ram[1][159] = 1u;
    muhtari__maria_start(m);
    picture[93440u] = 0xEEu;
    muhtari__maria_event(m);
    CHECK_EQ_U64(picture[93440u], 0xA7u);
    CHECK_EQ_U64(m->maria.write_buf, 1u);
    m->maria.rows = NULL;
}

static void observed(void *user, uint64_t tick, uint8_t source, uint16_t raw)
{
    (void)tick;
    (void)source;
    (void)raw;
    (*(uint32_t *)user)++;
}

static void observed_hash(void *user, uint64_t tick, uint8_t source, uint16_t raw)
{
    uint32_t *hash = user;
    *hash = (*hash ^ (uint32_t)tick) * 16777619u;
    *hash = (*hash ^ (uint32_t)(tick >> 32)) * 16777619u;
    *hash = (*hash ^ source) * 16777619u;
    *hash = (*hash ^ raw) * 16777619u;
}

static void metadata(muhtari *m)
{
    static uint8_t known[128u + 65536u];
    images_seeded(known + 128u, 65536u, IMAGES_SEED_OVERRIDE);
    muhtari_media media = {0};
    media.size = (uint32_t)sizeof media;
    media.image = known + 128u;
    media.image_size = 65536u;
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(m->effective_region, MUHTARI_REGION_PAL);
    images_header(known, 3u, "Region", 65536u, 2u);
    media.image = known;
    media.image_size = sizeof known;
    /* Explicit NTSC header wins over a PAL hash; multiregion follows it. */
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(m->effective_region, MUHTARI_REGION_NTSC);
    known[57] = 4u;
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(m->effective_region, MUHTARI_REGION_PAL);
    media.region = MUHTARI_REGION_NTSC;
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(m->effective_region, MUHTARI_REGION_NTSC);
    CHECK_EQ_U64(m->cart.info.region, MUHTARI_REGION_PAL);
    /* Dual-region preference through the existing table-substitution seam. */
    struct muhtari__cartdb_entry entries[9];
    memcpy(entries, muhtari__cartdb_7800_entries, sizeof entries);
    entries[1].flags = 3u;
    muhtari_cart_info info = {0};
    info.size = (uint32_t)sizeof info;
    CHECK_EQ_U64(muhtari__cart_identify(
                     known + 128u, 65536u, muhtari__cartdb_7800_keys,
                     muhtari__cartdb_7800_key_count, entries, 9u, muhtari__cartdb_7800_layouts,
                     muhtari__cartdb_7800_layout_count, muhtari__cartdb_2600_prefix,
                     muhtari__cartdb_2600_prefix_count, &info),
                 MUHTARI_STATUS_OK);
    CHECK_EQ_U64(info.region, MUHTARI_REGION_NTSC);
}

static void defaults_and_stalls(muhtari *m)
{
    muhtari_media media = cartridge();
    /* Omitted and partially present optional scalar tails take zero defaults. */
    static const uint32_t sizes[] = {
        MUHTARI__MEDIA_SIZE_FIRST, (uint32_t)offsetof(muhtari_media, port) + 1u,
        (uint32_t)offsetof(muhtari_media, port) + 3u, (uint32_t)sizeof(muhtari_media)};
    for (unsigned n = 0u; n < 4u; n++) {
        media.size = sizes[n];
        media.port[0] = n < 3u ? 0xDEADu : 0u;
        CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
        snapshot(m);
        CHECK_EQ_U64(m->ports.load_device[0], 0u);
    }
    /* Legacy first input waits; a complete extended packet precedes opcode1. */
    media = cartridge();
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    muhtari_input legacy = {0};
    legacy.size = (uint32_t)sizeof legacy;
    legacy.buttons[0] = MUHTARI_BUTTON_1;
    CHECK_EQ_U64(muhtari_set_input(m, &legacy), MUHTARI_STATUS_OK);
    muhtari_frame io = {0};
    io.size = (uint32_t)sizeof io;
    CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(m->ports.buttons[0], 0u);
    CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(m->ports.buttons[0], MUHTARI_BUTTON_1);
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    muhtari_input_ex extended = {0};
    extended.size = (uint32_t)sizeof extended;
    extended.port[0].buttons = MUHTARI_BUTTON_1;
    CHECK_EQ_U64(muhtari_set_input_ex(m, &extended), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(m->ports.buttons[0], MUHTARI_BUTTON_1);
    CHECK_EQ_U64(m->audio_rem, 336960u);

    /* PAL hostile loops: JAM, MARIA WSYNC, TIA WSYNC and interrupt tails.
       DMA-on uses a deliberately unterminated full-width object stream. */
    static const uint8_t programs[][9] = {{0x02u, 0u},
                                          {0x85u, 0x24u, 0x4Cu, 0x00u, 0xF0u},
                                          {0x85u, 0x02u, 0x4Cu, 0x00u, 0xF0u},
                                          {0x58u, 0xEAu, 0x4Cu, 0x01u, 0xF0u}};
    for (unsigned hostile = 0u; hostile < 4u; hostile++) {
        media = cartridge();
        memcpy(image + 128u, programs[hostile], sizeof programs[hostile]);
        CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
        if (hostile == 3u) {
            m->bus.nmi_pending = 1u;
            m->bus.nmi_at = 281970u;
        }
        for (unsigned full = 0u; full < 3u; full++) {
            if (hostile == 2u) {
                m->maria.reg[0x1Cu] = 0x40u;
                m->maria.dl = 0x1800u;
                memset(m->ram, 0xFF, 1024u);
            }
            uint64_t end = m->frame_deadline;
            CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_OK);
            CHECK(m->bus.ticks >= end);
            CHECK(m->bus.ticks - end <= 20730u);
            CHECK(io.audio_count >= 960u && io.audio_count <= 962u);
        }
    }
}

static void discard_equivalence(void)
{
    alloc_counts ca, cb;
    muhtari *a = make(&ca), *b = make(&cb);
    uint32_t raw_a = 0u, raw_b = 0u;
    CHECK_EQ_U64(muhtari__synth_observe(a, observed_hash, &raw_a), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari__synth_observe(b, observed_hash, &raw_b), MUHTARI_STATUS_OK);
    muhtari_media media = cartridge();
    CHECK_EQ_U64(muhtari_load(a, &media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari_load(b, &media), MUHTARI_STATUS_OK);
    uint32_t allocations_a = ca.allocs, allocations_b = cb.allocs;
    muhtari_input_ex input = {0};
    input.size = (uint32_t)sizeof input;
    input.port[0].device = MUHTARI_PORT_LIGHTGUN;
    input.port[0].gun_x = 319u;
    input.port[0].gun_y = 291u;
    CHECK_EQ_U64(muhtari_set_input_ex(a, &input), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari_set_input_ex(b, &input), MUHTARI_STATUS_OK);
    muhtari__bus_write(a, 0x20u, 8u);
    muhtari__bus_write(b, 0x20u, 8u);
    muhtari__bus_write(a, 1u, 0x40u);
    muhtari__bus_write(b, 1u, 0x40u);
    muhtari__bus_write(a, 0x15u, 4u);
    muhtari__bus_write(b, 0x15u, 4u);
    muhtari__bus_write(a, 0x19u, 7u);
    muhtari__bus_write(b, 0x19u, 7u);
    for (unsigned frame = 0u; frame < 3u; frame++) {
        int16_t peer_pcm[962];
        muhtari_frame output = {0}, discard = {0};
        output.size = discard.size = (uint32_t)sizeof output;
        output.video = picture + 1u;
        output.video_pitch = 320u;
        output.audio = pcm;
        output.audio_capacity = 962u;
        discard.audio = peer_pcm;
        discard.audio_capacity = 962u;
        CHECK_EQ_U64(muhtari_run_frame(a, &output), MUHTARI_STATUS_OK);
        CHECK_EQ_U64(muhtari_run_frame(b, &discard), MUHTARI_STATUS_OK);
        CHECK_EQ_U64(output.audio_count, discard.audio_count);
        CHECK(memcmp(pcm, peer_pcm, output.audio_count * sizeof pcm[0]) == 0);
        CHECK_EQ_U64(a->audio_rem, b->audio_rem);
        CHECK(memcmp(&a->bus, &b->bus, sizeof a->bus) == 0);
        CHECK(memcmp(&a->cpu, &b->cpu, sizeof a->cpu) == 0);
        CHECK(memcmp(&a->maria, &b->maria, sizeof a->maria) == 0);
        CHECK(memcmp(&a->synth, &b->synth, sizeof a->synth) == 0);
        CHECK(memcmp(&a->tia, &b->tia, sizeof a->tia) == 0);
        CHECK(memcmp(&a->ports, &b->ports, sizeof a->ports) == 0);
        CHECK_EQ_U64(raw_a, raw_b);
        CHECK_EQ_U64(muhtari__bus_peek(a, 12u), muhtari__bus_peek(b, 12u));
        CHECK(memcmp(a->pokey, b->pokey, sizeof a->pokey) == 0);
    }
    CHECK_EQ_U64(ca.allocs, allocations_a);
    CHECK_EQ_U64(cb.allocs, allocations_b);
    CHECK(raw_a != 0u);
    muhtari_destroy(a);
    muhtari_destroy(b);
}

static void lifecycle(void)
{
    alloc_counts counts;
    muhtari *m = make(&counts);
    muhtari_media media = cartridge();
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    edges(m);
    CHECK_EQ_U64(muhtari_power_cycle(m), MUHTARI_STATUS_OK);
    snapshot(m);
    uint32_t calls = 0u;
    muhtari_destroy(m);
    m = make(&counts);
    CHECK_EQ_U64(muhtari__synth_observe(m, observed, &calls), MUHTARI_STATUS_OK);
    media = cartridge();
    media.port[1] = MUHTARI_PORT_SAVEKEY;
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    m->battery.data[123u] = 0x69u;
    m->battery.generation = 42u;
    muhtari_input_ex in = {0};
    in.size = (uint32_t)sizeof in;
    in.port[0].device = MUHTARI_PORT_LIGHTGUN;
    in.port[0].gun_x = 319u;
    in.port[0].gun_y = 291u;
    CHECK_EQ_U64(muhtari_set_input_ex(m, &in), MUHTARI_STATUS_OK);
    struct muhtari__ports accepted = m->ports;
    in.port[0].gun_y = 292u;
    CHECK_EQ_U64(muhtari_set_input_ex(m, &in), MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK(memcmp(&accepted, &m->ports, sizeof accepted) == 0);
    muhtari *before = malloc(sizeof *before);
    CHECK(before != NULL);
    if (before == NULL) {
        muhtari_destroy(m);
        return;
    }
    memcpy(before, m, sizeof *before);
    muhtari_frame io = {0};
    io.size = (uint32_t)sizeof io;
    io.video = picture + 1u;
    io.video_pitch = 319u;
    io.audio = pcm;
    io.audio_capacity = 959u;
    CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK_EQ_U64(io.audio_count, 0u);
    CHECK(memcmp(m, before, sizeof *m) == 0);
    io.video_pitch = 65537u;
    CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK(memcmp(m, before, sizeof *m) == 0);
    io.video_pitch = 320u;
    CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_ERR_BUFFER_TOO_SMALL);
    CHECK_EQ_U64(io.audio_count, 0u);
    CHECK_EQ_U64(calls, 0u);
    CHECK(memcmp(m, before, sizeof *m) == 0);
    for (unsigned fault = 0u; fault < 5u; fault++) {
        muhtari_media bad = media;
        if (fault == 0u)
            bad.region = 3u;
        if (fault == 1u)
            bad.image_size = 0u;
        if (fault == 2u) {
            bad.bios = image;
            bad.bios_size = 8192u;
        }
        if (fault == 3u)
            bad.port[0] = MUHTARI_PORT_SAVEKEY;
        if (fault == 4u) {
            bad.region = MUHTARI_REGION_NTSC;
            counts.fail_from = counts.allocs + 1u;
        }
        CHECK(muhtari_load(m, &bad) != MUHTARI_STATUS_OK);
        CHECK(memcmp(m, before, sizeof *m) == 0);
    }
    counts.fail_from = 0u;
    io.audio_capacity = 960u;
    pcm[960] = 0x5A5A;
    CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(io.audio_count, 960u);
    CHECK_EQ_U64(pcm[960], 0x5A5A);
    CHECK_EQ_U64(muhtari_power_cycle(m), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(m->battery.data[123u], 0x69u);
    CHECK_EQ_U64(m->battery.generation, 42u);
    free(before);

    /* Padded PAL rows and the actual maximum-pitch accessible extent. */
    static const uint32_t pitches[] = {320u, 336u, 65536u};
    for (unsigned test = 0u; test < 3u; test++) {
        size_t extent = (size_t)291u * pitches[test] + 320u;
        uint8_t *output = malloc(extent + 2u);
        CHECK(output != NULL);
        if (output == NULL)
            continue;
        memset(output, 0xA5, extent + 2u);
        CHECK_EQ_U64(muhtari_power_cycle(m), MUHTARI_STATUS_OK);
        io.video = output + 1u;
        io.video_pitch = pitches[test];
        io.audio = NULL;
        CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_OK);
        CHECK_EQ_U64(output[0], 0xA5u);
        CHECK_EQ_U64(output[extent + 1u], 0xA5u);
        for (uint32_t y = 0u; y < 292u; y++) {
            CHECK_EQ_U64(output[1u + (size_t)y * pitches[test] + 319u], 0u);
            if (y < 291u)
                for (uint32_t x = 320u; x < pitches[test]; x++)
                    CHECK_EQ_U64(output[1u + (size_t)y * pitches[test] + x], 0xA5u);
        }
        free(output);
    }
    media.port[1] = 0u;
    media.region = MUHTARI_REGION_NTSC;
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    muhtari_info info = {0};
    info.size = (uint32_t)sizeof info;
    CHECK_EQ_U64(muhtari_get_info(m, &info), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(info.height, 242u);
    in.port[0].gun_y = 241u;
    CHECK_EQ_U64(muhtari_set_input_ex(m, &in), MUHTARI_STATUS_OK);
    accepted = m->ports;
    in.port[0].gun_y = 242u;
    CHECK_EQ_U64(muhtari_set_input_ex(m, &in), MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK(memcmp(&accepted, &m->ports, sizeof accepted) == 0);
    metadata(m);
    defaults_and_stalls(m);
    discard_equivalence();
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
