/* Public XM/YM frame path and fail-closed state gate. */
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"
#include "../check.h"
#include "../cart/images.h"
#include "ym_vectors.h"

#define YM_BODY (16u * IMAGES_KIB)
#define YM_IMAGE_BYTES (IMAGES_HEADER + YM_BODY)
#define AUDIO_CAPACITY 962u

struct ym_allocations {
    uint32_t allocs, frees;
};

static uint8_t ym_image[YM_IMAGE_BYTES];
static uint8_t old_image[YM_IMAGE_BYTES];
static uint8_t irq_image[YM_IMAGE_BYTES];

static void *test_alloc(void *user, size_t bytes)
{
    struct ym_allocations *counts = user;
    if (counts != NULL)
        counts->allocs++;
    return malloc(bytes);
}

static void test_free(void *user, void *ptr, size_t bytes)
{
    struct ym_allocations *counts = user;
    (void)bytes;
    if (counts != NULL)
        counts->frees++;
    free(ptr);
}

static muhtari *make_machine(void)
{
    muhtari_config config;
    memset(&config, 0, sizeof config);
    config.size = (uint32_t)sizeof config;
    config.allocator.alloc = test_alloc;
    config.allocator.free = test_free;
    muhtari *machine = NULL;
    CHECK_EQ_U64(muhtari_create(&config, &machine), MUHTARI_STATUS_OK);
    return machine;
}

static muhtari *make_machine_counted(struct ym_allocations *counts)
{
    muhtari_config config;
    memset(&config, 0, sizeof config);
    config.size = (uint32_t)sizeof config;
    config.allocator.user = counts;
    config.allocator.alloc = test_alloc;
    config.allocator.free = test_free;
    muhtari *machine = NULL;
    CHECK_EQ_U64(muhtari_create(&config, &machine), MUHTARI_STATUS_OK);
    return machine;
}

static muhtari_media media_for(const uint8_t *image, size_t bytes, uint32_t expansion)
{
    muhtari_media media;
    memset(&media, 0, sizeof media);
    media.size = (uint32_t)sizeof media;
    media.image = image;
    media.image_size = bytes;
    media.expansion = expansion;
    return media;
}

static muhtari_status frame(muhtari *machine, int16_t *audio, uint32_t *count)
{
    muhtari_frame io;
    memset(&io, 0, sizeof io);
    io.size = (uint32_t)sizeof io;
    io.audio = audio;
    io.audio_capacity = AUDIO_CAPACITY;
    muhtari_status status = muhtari_run_frame(machine, &io);
    *count = io.audio_count;
    return status;
}

struct span_snapshot {
    uint32_t kind;
    muhtari_span span;
    uint8_t *copy;
};

static void ym_tracer(void)
{
    static struct span_snapshot spans[4];
    static muhtari before;
    static const uint32_t kinds[4] = {MUHTARI_MEMORY_SYSTEM_RAM, MUHTARI_MEMORY_RIOT_RAM,
                                      MUHTARI_MEMORY_CART_RAM, MUHTARI_MEMORY_EXPANSION_RAM};
    int16_t audio_a[AUDIO_CAPACITY];
    int16_t audio_b[AUDIO_CAPACITY];
    struct ym_allocations a_alloc = {0u, 0u};
    struct ym_allocations b_alloc = {0u, 0u};
    uint8_t *battery_before;
    uint8_t *state;
    uint8_t *candidate;
    size_t state_bytes;
    size_t old_state_bytes;
    uint8_t *old_state;

    size_t ym_bytes = images_ym2151(ym_image);
    muhtari_media ym_media = media_for(ym_image, ym_bytes, MUHTARI_EXPANSION_XM);
    ym_media.port[1] = MUHTARI_PORT_SAVEKEY;
    muhtari *a = make_machine_counted(&a_alloc);
    muhtari *b = make_machine_counted(&b_alloc);
    CHECK_EQ_U64(muhtari_load(a, &ym_media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari_load(b, &ym_media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(a->synth.ym_installed, 1u);
    CHECK_EQ_U64(a->cart.irq_mask, 4u);
    CHECK_EQ_U64(a->ym.output[0], 0u);
    CHECK_EQ_U64(a->ym.output[1], 0u);
    CHECK_EQ_U64(a->synth.raw[10], 0u);
    CHECK_EQ_U64(a->synth.raw[11], 0u);
    CHECK_EQ_U64(a->battery.bytes, 34816u);
    state_bytes = muhtari_state_size(a);
    CHECK(state_bytes > 64u);
    CHECK_EQ_U64(state_bytes, muhtari_state_size(b));
    state = malloc(state_bytes + 4u);
    candidate = malloc(state_bytes);
    CHECK(state != NULL && candidate != NULL);
    if (state == NULL || candidate == NULL) {
        free(candidate);
        free(state);
        muhtari_destroy(b);
        muhtari_destroy(a);
        return;
    }

    for (unsigned i = 0u; i < 4u; i++) {
        memset(&spans[i].span, 0, sizeof spans[i].span);
        spans[i].kind = kinds[i];
        spans[i].span.size = (uint32_t)sizeof spans[i].span;
        CHECK_EQ_U64(muhtari_get_memory(a, spans[i].kind, &spans[i].span), MUHTARI_STATUS_OK);
        spans[i].copy = spans[i].span.bytes != 0u ? malloc(spans[i].span.bytes) : NULL;
        CHECK(spans[i].span.bytes == 0u || spans[i].copy != NULL);
        if (spans[i].span.bytes != 0u && spans[i].copy != NULL)
            memcpy(spans[i].copy, spans[i].span.data, spans[i].span.bytes);
    }
    battery_before = malloc(a->battery.bytes);
    CHECK(battery_before != NULL);
    if (battery_before != NULL)
        memcpy(battery_before, a->battery.data, a->battery.bytes);

    memset(state, 0xA7, state_bytes + 4u);
    uint32_t a_allocs = a_alloc.allocs, a_frees = a_alloc.frees;
    CHECK_EQ_U64(muhtari_save_state(a, state, state_bytes + 4u), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(state[8], 2u);
    CHECK_EQ_U64(state[20], 7u);
    CHECK_EQ_HEX(state[state_bytes], 0xA7u);
    memcpy(candidate, state, state_bytes);
    CHECK_EQ_U64(muhtari_save_state(a, state, state_bytes - 1u),
                 MUHTARI_STATUS_ERR_BUFFER_TOO_SMALL);
    CHECK(memcmp(candidate, state, state_bytes) == 0);
    CHECK_EQ_U64(muhtari_save_state(a, state, state_bytes + 4u), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari_load_state(b, state, state_bytes), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari_save_state(b, candidate, state_bytes), MUHTARI_STATUS_OK);
    CHECK(memcmp(state, candidate, state_bytes) == 0);
    CHECK_EQ_U64(a_alloc.allocs, a_allocs);
    CHECK_EQ_U64(a_alloc.frees, a_frees);
    CHECK_EQ_HEX(state[state_bytes], 0xA7u);

    /* Reject a damaged envelope and a truncated caller span without changing
       the live machine, expansion RAM, battery bytes or stable RAM spans. */
    memcpy(&before, a, sizeof before);
    memcpy(candidate, state, state_bytes);
    candidate[24] ^= 1u;
    CHECK_EQ_U64(muhtari_load_state(a, candidate, state_bytes),
                 MUHTARI_STATUS_ERR_INVALID_STATE);
    CHECK(memcmp(&before, a, sizeof before) == 0);
    CHECK(battery_before == NULL ||
          memcmp(a->battery.data, battery_before, a->battery.bytes) == 0);
    memcpy(candidate, state, 64u);
    CHECK_EQ_U64(muhtari_load_state(a, candidate, 64u), MUHTARI_STATUS_ERR_INVALID_STATE);
    for (unsigned i = 0u; i < 4u; i++) {
        muhtari_span after;
        memset(&after, 0, sizeof after);
        after.size = (uint32_t)sizeof after;
        CHECK_EQ_U64(muhtari_get_memory(a, spans[i].kind, &after), MUHTARI_STATUS_OK);
        CHECK(after.data == spans[i].span.data);
        CHECK_EQ_U64(after.bytes, spans[i].span.bytes);
        if (after.bytes != 0u && spans[i].copy != NULL)
            CHECK(memcmp(after.data, spans[i].copy, after.bytes) == 0);
    }

    /* A format-1 state remains valid for a non-YM machine and refuses on a
       YM machine because the per-machine envelope extent differs. */
    images_header(old_image, 4u, "YM legacy control", YM_BODY, 0u);
    images_fill(old_image + IMAGES_HEADER, YM_BODY, 0x2151u);
    images_footer(old_image + IMAGES_HEADER, YM_BODY);
    images_header(irq_image, 4u, "YM legacy control", YM_BODY, 0u);
    irq_image[69] = 4u; /* valid v4 bit 2 is ignored without an installed YM */
    memcpy(irq_image + IMAGES_HEADER, old_image + IMAGES_HEADER, YM_BODY);
    muhtari_media old_media = media_for(old_image, YM_IMAGE_BYTES, MUHTARI_EXPANSION_OFF);
    muhtari_media irq_media = media_for(irq_image, YM_IMAGE_BYTES, MUHTARI_EXPANSION_OFF);
    muhtari *legacy = make_machine();
    muhtari *legacy_irq = make_machine();
    CHECK_EQ_U64(muhtari_load(legacy, &old_media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari_load(legacy_irq, &irq_media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(legacy_irq->cart.irq_mask, 0u);
    CHECK(memcmp(legacy->state_identity, legacy_irq->state_identity,
                 sizeof legacy->state_identity) == 0);
    old_state_bytes = muhtari_state_size(legacy);
    old_state = malloc(old_state_bytes);
    CHECK(old_state != NULL);
    if (old_state != NULL) {
        CHECK_EQ_U64(muhtari_save_state(legacy, old_state, old_state_bytes), MUHTARI_STATUS_OK);
        CHECK_EQ_U64(muhtari_load_state(a, old_state, old_state_bytes),
                     MUHTARI_STATUS_ERR_INVALID_STATE);
    }

    /* Resume from a nontrivial cartridge-generated YM stream and keep
       repeating save/load between public frame calls. */
    for (unsigned i = 0u; i < 3u; i++) {
        uint32_t count = 0u;
        CHECK_EQ_U64(frame(a, audio_a, &count), MUHTARI_STATUS_OK);
    }
    CHECK(a->ym.operators[0].key_state != 0u);
    CHECK_EQ_U64(muhtari_save_state(a, state, state_bytes), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari_load_state(b, state, state_bytes), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari_save_state(b, candidate, state_bytes), MUHTARI_STATUS_OK);
    CHECK(memcmp(state, candidate, state_bytes) == 0);
    int16_t minimum = INT16_MAX;
    int16_t maximum = INT16_MIN;
    uint64_t nonzero = 0u;
    for (unsigned frame_index = 0u; frame_index < 3u; frame_index++) {
        uint32_t count_a = 0u, count_b = 0u;
        CHECK_EQ_U64(frame(a, audio_a, &count_a), MUHTARI_STATUS_OK);
        CHECK_EQ_U64(frame(b, audio_b, &count_b), MUHTARI_STATUS_OK);
        CHECK_EQ_U64(count_a, count_b);
        CHECK(memcmp(audio_a, audio_b, count_a * sizeof audio_a[0]) == 0);
        CHECK(count_a == 800u || count_a == 801u);
        for (uint32_t i = 0u; i < count_a; i++) {
            int16_t sample = audio_a[i];
            if (sample < minimum)
                minimum = sample;
            if (sample > maximum)
                maximum = sample;
            nonzero += sample != 0;
            CHECK(sample >= -29204 && sample <= 29204);
        }
        CHECK_EQ_U64(a->ym.clocks, b->ym.clocks);
        CHECK(memcmp(&a->ym, &b->ym, sizeof a->ym) == 0);
        CHECK_EQ_U64(a->bus.ticks, b->bus.ticks);
        CHECK_EQ_U64(a->frames, b->frames);
        CHECK_EQ_U64(a->lag_frames, b->lag_frames);
        CHECK_EQ_U64(a->synth.mixed_level, b->synth.mixed_level);
        CHECK(memcmp(a->synth.raw, b->synth.raw, sizeof a->synth.raw) == 0);
        CHECK_EQ_U64(a->battery.generation, b->battery.generation);
        CHECK(memcmp(a->battery.data, b->battery.data, a->battery.bytes) == 0);
        CHECK_EQ_U64(muhtari_save_state(a, state, state_bytes), MUHTARI_STATUS_OK);
        CHECK_EQ_U64(muhtari_load_state(b, state, state_bytes), MUHTARI_STATUS_OK);
        CHECK_EQ_U64(muhtari_save_state(b, candidate, state_bytes), MUHTARI_STATUS_OK);
        CHECK(memcmp(state, candidate, state_bytes) == 0);
    }
    CHECK(nonzero > 100u);
    CHECK(minimum < 0);
    CHECK(maximum > 0);

    /* Format-1 non-YM state remains a normal save/restore path. */
    CHECK(old_state_bytes > 64u);
    if (old_state != NULL) {
        legacy->ram[0] = 0xA5u;
        CHECK_EQ_U64(muhtari_load_state(legacy, old_state, old_state_bytes), MUHTARI_STATUS_OK);
        CHECK_EQ_U64(legacy->ram[0], 0u);
    }

    for (unsigned i = 0u; i < 4u; i++)
        free(spans[i].copy);
    free(battery_before);
    free(old_state);
    free(candidate);
    free(state);
    muhtari_destroy(legacy_irq);
    muhtari_destroy(legacy);
    muhtari_destroy(b);
    muhtari_destroy(a);
}

static muhtari *ym_test_chip(void)
{
    muhtari *m = calloc(1u, sizeof *m);
    CHECK(m != NULL);
    if (m != NULL) {
        muhtari__synth_reset(m, 0u, 0u);
        muhtari__ym2151_reset(m);
    }
    return m;
}

static void ym_write_reg(muhtari *m, uint8_t reg, uint8_t value, uint64_t tick)
{
    muhtari__ym2151_write(m, 0x0460u, reg, tick);
    muhtari__ym2151_write(m, 0x0461u, value, tick);
}

/* Test-only chip advancement uses the normal half-open PCM consumer at each
   frame boundary, keeping numeric and synthesis loops within the real FIFO. */
static void ym_advance_and_discard(muhtari *m, uint64_t target)
{
    const struct muhtari__timing timing = muhtari__timing(m);
    CHECK(timing.frame_ticks != 0u);
    CHECK(target >= m->synth.horizon);
    if (timing.frame_ticks == 0u || target < m->synth.horizon)
        return;

    while (target - m->synth.horizon >= timing.frame_ticks) {
        const uint64_t end = m->synth.horizon + timing.frame_ticks;
        CHECK(end <= target);
        muhtari__ym2151_catch_up(m, end);
        CHECK(m->synth.pcm_count <= MUHTARI__SYNTH_PCM_CAPACITY);
        const uint32_t discarded = muhtari__synth_render(m, end, NULL);
        CHECK(discarded <= timing.max_samples);
        CHECK_EQ_U64(m->synth.horizon, end);
        CHECK(m->synth.pcm_count <= MUHTARI__SYNTH_PCM_CAPACITY);
    }

    muhtari__ym2151_catch_up(m, target);
    CHECK(m->synth.pcm_count <= MUHTARI__SYNTH_PCM_CAPACITY);
}

static void ym_numeric_timer(uint32_t timer, uint32_t value, uint64_t expected_period)
{
    muhtari *m = ym_test_chip();
    CHECK(m != NULL);
    if (m == NULL)
        return;

    uint8_t mode;
    uint8_t flag = timer == 0u ? 1u : 2u;
    if (timer == 0u) {
        ym_write_reg(m, 0x10u, (uint8_t)(value >> 2), 0u);
        ym_write_reg(m, 0x11u, (uint8_t)(value & 3u), 0u);
        mode = 0x05u;
    } else {
        ym_write_reg(m, 0x12u, (uint8_t)value, 0u);
        mode = 0x0au;
    }
    ym_write_reg(m, 0x14u, mode, 0u);
    CHECK_EQ_U64(m->ym.timer_running[timer], 1u);
    CHECK_EQ_U64(m->ym.timer_deadline[timer], expected_period);

    uint64_t just_before = (expected_period - 1u) * 2u;
    uint64_t exact = expected_period * 2u;
    uint64_t just_after = (expected_period + 1u) * 2u;
    ym_advance_and_discard(m, just_before);
    CHECK_EQ_U64(muhtari__ym2151_read(m, 0x0461u, just_before) & flag, 0u);
    CHECK_EQ_U64(muhtari__ym2151_irq(m), 0u);
    ym_advance_and_discard(m, exact);
    CHECK_EQ_U64(muhtari__ym2151_read(m, 0x0461u, exact) & flag, flag);
    CHECK_EQ_U64(muhtari__ym2151_irq(m), 1u);
    ym_advance_and_discard(m, just_after);
    CHECK_EQ_U64(muhtari__ym2151_read(m, 0x0461u, just_after) & flag, flag);
    CHECK_EQ_U64(muhtari__ym2151_irq(m), 1u);
    CHECK_EQ_U64(m->ym.timer_running[timer], 1u);
    CHECK_EQ_U64(m->ym.timer_deadline[timer], expected_period * 2u);

    /* Clearing the carry-enable bit does not erase an already latched flag
       or mask its IRQ; only the documented reset bit clears that status. */
    uint8_t disabled_mode = (uint8_t)(mode & (timer == 0u ? 0xfbu : 0xf7u));
    ym_write_reg(m, 0x14u, disabled_mode, just_after);
    CHECK_EQ_U64(muhtari__ym2151_read(m, 0x0461u, just_after) & flag, flag);
    CHECK_EQ_U64(muhtari__ym2151_irq(m), 1u);

    uint8_t reset_mode = (uint8_t)(mode | (timer == 0u ? 0x10u : 0x20u));
    ym_write_reg(m, 0x14u, reset_mode, just_after);
    CHECK_EQ_U64(muhtari__ym2151_read(m, 0x0461u, just_after) & flag, 0u);
    CHECK_EQ_U64(muhtari__ym2151_irq(m), 0u);
    free(m);
}

static void ym_numeric(void)
{
    muhtari *m = ym_test_chip();
    CHECK(m != NULL);
    if (m != NULL) {
        ym_write_reg(m, 0x20u, 0u, 0u);
        ym_advance_and_discard(m, 67u * 2u);
        CHECK_EQ_U64(muhtari__ym2151_read(m, 0x0461u, 67u * 2u) & 0x80u, 0x80u);
        ym_advance_and_discard(m, 68u * 2u);
        CHECK_EQ_U64(muhtari__ym2151_read(m, 0x0461u, 68u * 2u) & 0x80u, 0u);
        ym_advance_and_discard(m, 69u * 2u);
        CHECK_EQ_U64(muhtari__ym2151_read(m, 0x0461u, 69u * 2u) & 0x80u, 0u);
        CHECK_EQ_U64(m->ym.busy_end, 68u);
        free(m);
    }

    /* Yamaha Application Manual §2.1.7, measured in φM periods. */
    ym_numeric_timer(0u, 1023u, 64u * (1024u - 1023u));
    ym_numeric_timer(0u, 0u, 64u * (1024u - 0u));
    ym_numeric_timer(1u, 255u, 1024u * (256u - 255u));
    ym_numeric_timer(1u, 0u, 1024u * (256u - 0u));

    muhtari *same_edge = ym_test_chip();
    CHECK(same_edge != NULL);
    if (same_edge != NULL) {
        ym_write_reg(same_edge, 0x10u, 0xffu, 0u);
        ym_write_reg(same_edge, 0x11u, 3u, 0u);
        ym_write_reg(same_edge, 0x14u, 0x05u, 0u);
        ym_advance_and_discard(same_edge, 128u);
        CHECK_EQ_U64(muhtari__ym2151_read(same_edge, 0x0461u, 128u) & 1u, 1u);
        ym_write_reg(same_edge, 0x14u, 0x15u, 128u);
        CHECK_EQ_U64(muhtari__ym2151_read(same_edge, 0x0461u, 128u) & 1u, 0u);
        CHECK_EQ_U64(muhtari__ym2151_irq(same_edge), 0u);
        free(same_edge);
    }

    /* Exercise the last legal MARIA tick without wrapping catch-up arithmetic;
       Timer B remains scheduled for its complete documented maximum period. */
    muhtari *epoch = ym_test_chip();
    CHECK(epoch != NULL);
    if (epoch != NULL) {
        uint64_t base_tick = UINT64_MAX - 10u;
        epoch->ym.last_tick = base_tick;
        epoch->ym.clocks = base_tick / 2u;
        uint64_t timer_clock = epoch->ym.clocks;
        uint64_t expected_deadline = timer_clock + 1024u * 256u;
        ym_write_reg(epoch, 0x12u, 0u, base_tick);
        ym_write_reg(epoch, 0x14u, 0x0au, base_tick);
        CHECK_EQ_U64(epoch->ym.timer_deadline[1], expected_deadline);
        muhtari__ym2151_catch_up(epoch, UINT64_MAX);
        CHECK_EQ_U64(epoch->ym.clocks, timer_clock + 5u);
        CHECK_EQ_U64(epoch->ym.last_tick, UINT64_MAX);
        CHECK_EQ_U64(epoch->ym.sample_divider, 5u);
        CHECK_EQ_U64(epoch->ym.timer_deadline[1], expected_deadline);
        CHECK(expected_deadline > epoch->ym.clocks);
        free(epoch);
    }
}

static uint64_t ym_hash_sample(uint64_t hash, int16_t left, int16_t right)
{
    uint16_t values[2] = {(uint16_t)left, (uint16_t)right};
    for (unsigned channel = 0u; channel < 2u; channel++) {
        hash ^= values[channel] & 0xffu;
        hash *= UINT64_C(1099511628211);
        hash ^= values[channel] >> 8;
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}

static void ym_case_write(muhtari *machines[2], uint8_t count, uint8_t chip,
                          uint8_t reg, uint8_t value, uint64_t tick)
{
    if (chip == YM_CASE_ALL) {
        for (uint8_t i = 0u; i < count; i++)
            ym_write_reg(machines[i], reg, value, tick);
    } else if (chip < count) {
        ym_write_reg(machines[chip], reg, value, tick);
    }
}

static uint64_t ym_run_case(const struct ym_case_vector *test, uint32_t *sample_count,
                            int16_t minimum[2], int16_t maximum[2])
{
    muhtari *machines[2] = {NULL, NULL};
    uint64_t now = 0u;
    uint64_t hash = UINT64_C(14695981039346656037);
    uint32_t generated = 0u;
    minimum[0] = INT16_MAX;
    minimum[1] = INT16_MAX;
    maximum[0] = INT16_MIN;
    maximum[1] = INT16_MIN;

    CHECK(test->chip_count == 1u || test->chip_count == 2u);
    CHECK(test->sample_count <= 2048u);
    for (uint8_t chip = 0u; chip < test->chip_count; chip++)
        machines[chip] = ym_test_chip();
    for (size_t i = 0u; i < sizeof ym_common / sizeof ym_common[0]; i++)
        ym_case_write(machines, test->chip_count, ym_common[i].chip,
                      ym_common[i].reg, ym_common[i].value, 0u);

    for (uint16_t action = 0u; action < test->action_count; action++) {
        const struct ym_case_action *item = &test->actions[action];
        if (item->reg != YM_CASE_STEP)
            ym_case_write(machines, test->chip_count, item->chip, item->reg,
                          item->value, now * 2u);
        for (uint16_t step = 0u; step < item->samples; step++) {
            now += 64u;
            for (uint8_t chip = 0u; chip < test->chip_count; chip++) {
                ym_advance_and_discard(machines[chip], now * 2u);
                hash = ym_hash_sample(hash, machines[chip]->ym.output[0],
                                      machines[chip]->ym.output[1]);
                for (unsigned channel = 0u; channel < 2u; channel++) {
                    int16_t output = machines[chip]->ym.output[channel];
                    if (output < minimum[channel])
                        minimum[channel] = output;
                    if (output > maximum[channel])
                        maximum[channel] = output;
                }
            }
            generated++;
        }
    }
    CHECK_EQ_U64(generated, test->sample_count);
    for (uint8_t chip = 0u; chip < test->chip_count; chip++)
        free(machines[chip]);
    *sample_count = generated;
    return hash;
}

static void ym_synthesis(void)
{
    for (size_t i = 0u; i < YM_CASE_COUNT; i++) {
        uint32_t samples = 0u;
        int16_t minimum[2], maximum[2];
        uint64_t hash = ym_run_case(&ym_cases[i], &samples, minimum, maximum);
        (void)printf("%s %u %016llx\n", ym_cases[i].name, samples,
                     (unsigned long long)hash);
        CHECK_EQ_U64(samples, ym_cases[i].sample_count);
        CHECK_EQ_U64(hash, ym_cases[i].hash);
        if (i == 0u) {
            CHECK(minimum[0] < 0 && minimum[1] < 0);
            CHECK(maximum[0] > 0 && maximum[1] > 0);
        }
    }
}

static int ym_judge(const struct ym_case_vector *expected, const char *name,
                    uint32_t samples, uint64_t hash, const char *seed)
{
    static const char expected_seed[] = "81aec25ccbb98f4873a255f7551ac4dadac59b4a";
    return strcmp(name, expected->name) == 0 && samples == expected->sample_count &&
           hash == expected->hash && strcmp(seed, expected_seed) == 0;
}

static void ym_controls(void)
{
    const struct ym_case_vector *expected = &ym_cases[0];
    int16_t one[2] = {1234, -567};
    uint64_t correct = ym_hash_sample(UINT64_C(14695981039346656037), one[0], one[1]);
    uint64_t swapped = ym_hash_sample(UINT64_C(14695981039346656037), one[1], one[0]);
    struct ym_case_vector control = *expected;
    control.name = "control";
    control.sample_count = 1u;
    control.hash = correct;
    CHECK(ym_judge(&control, control.name, 1u, correct,
                   "81aec25ccbb98f4873a255f7551ac4dadac59b4a"));
    CHECK(correct != swapped);
    CHECK(!ym_judge(&control, control.name, 1u,
                    control.hash ^ 1u, "81aec25ccbb98f4873a255f7551ac4dadac59b4a"));
    CHECK(!ym_judge(&control, control.name, 1u, swapped,
                    "81aec25ccbb98f4873a255f7551ac4dadac59b4a"));
    CHECK(!ym_judge(&control, "missing_case", 1u, correct,
                    "81aec25ccbb98f4873a255f7551ac4dadac59b4a"));
    CHECK(!ym_judge(&control, control.name, 1u, correct,
                    "wrong-seed"));
}

static muhtari_status ym_load_variant(muhtari *m, const uint8_t *image, size_t bytes,
                                      uint32_t expansion)
{
    muhtari_media media = media_for(image, bytes, expansion);
    return muhtari_load(m, &media);
}

static void ym_profile(void)
{
    uint8_t variant[YM_IMAGE_BYTES];
    struct ym_allocations counts = {0u, 0u};
    size_t bytes = images_ym2151(ym_image);
    muhtari *m = make_machine_counted(&counts);
    CHECK(m != NULL);
    if (m == NULL)
        return;

    CHECK_EQ_U64(ym_load_variant(m, ym_image, bytes, MUHTARI_EXPANSION_XM),
                 MUHTARI_STATUS_OK);
    CHECK_EQ_U64(m->synth.ym_installed, 1u);
    CHECK_EQ_U64(m->cart.irq_mask, 4u);

    /* Every lower control bit (including HSC/POKEY/RAM controls) is
       orthogonal to D7: it gates only the exact YM address/data pair. */
    for (unsigned low = 0u; low < 128u; low++) {
        uint8_t previous = m->ym.registers[0x20u];
        muhtari__bus_write(m, 0x0470u, (uint8_t)low);
        muhtari__bus_write(m, 0x0460u, 0x20u);
        muhtari__bus_write(m, 0x0461u, (uint8_t)(low ^ 0x5au));
        CHECK_EQ_U64(m->ym.registers[0x20u], previous);

        muhtari__bus_write(m, 0x0470u, (uint8_t)(0x80u | low));
        muhtari__bus_write(m, 0x0460u, 0x20u);
        muhtari__bus_write(m, 0x0461u, (uint8_t)(low ^ 0xa5u));
        CHECK_EQ_U64(m->ym.registers[0x20u], (low ^ 0xa5u));
    }
    CHECK_EQ_U64(m->cart.expansion.control, 0xffu);

    /* Page registers and all nearby aliases leave the chip address/data
       state alone. Peeks and MARIA reads cannot clock or read YM status. */
    muhtari__bus_write(m, 0x0470u, 0x84u);
    muhtari__bus_write(m, 0x0460u, 0x20u);
    muhtari__bus_write(m, 0x0461u, 0x6du);
    uint8_t register_value = m->ym.registers[0x20u];
    uint8_t address = m->ym.address;
    uint64_t clocks = m->ym.clocks;
    uint64_t last_tick = m->ym.last_tick;
    for (unsigned page = 0u; page < 16u; page++) {
        muhtari__bus_write(m, 0x0478u, (uint8_t)(page * 17u));
        muhtari__bus_write(m, 0x047cu, (uint8_t)(page * 17u));
        CHECK_EQ_U64(m->ym.registers[0x20u], register_value);
        CHECK_EQ_U64(m->ym.address, address);
    }
    static const uint16_t aliases[] = {0x045fu, 0x0462u, 0x0463u, 0x046fu,
                                       0x0471u, 0x0477u, 0x0479u, 0x047du};
    for (size_t i = 0u; i < sizeof aliases / sizeof aliases[0]; i++) {
        muhtari__bus_write(m, aliases[i], 0x3cu);
        CHECK_EQ_U64(m->ym.registers[0x20u], register_value);
        CHECK_EQ_U64(m->ym.address, address);
    }
    clocks = m->ym.clocks;
    last_tick = m->ym.last_tick;
    (void)muhtari__bus_peek(m, 0x0460u);
    (void)muhtari__bus_peek(m, 0x0461u);
    CHECK_EQ_U64(m->ym.clocks, clocks);
    CHECK_EQ_U64(m->ym.last_tick, last_tick);
    (void)muhtari__cart_read_at(m, 0x0461u, 1, m->bus.ticks + 100u);
    CHECK_EQ_U64(m->ym.clocks, clocks);
    CHECK_EQ_U64(m->ym.last_tick, last_tick);

    /* D7 only suppresses register access. Timer and output clocks keep
       running, and re-enabling does not reset either source. */
    muhtari__bus_write(m, 0x0470u, 0x80u);
    muhtari__bus_write(m, 0x0460u, 0x10u);
    muhtari__bus_write(m, 0x0461u, 0xffu);
    muhtari__bus_write(m, 0x0460u, 0x11u);
    muhtari__bus_write(m, 0x0461u, 0x03u);
    muhtari__bus_write(m, 0x0460u, 0x14u);
    muhtari__bus_write(m, 0x0461u, 0x05u);
    uint64_t timer_deadline = m->ym.timer_deadline[0];
    uint32_t output_steps = m->ym.total_clocks;
    muhtari__bus_write(m, 0x0470u, 0u);
    for (unsigned i = 0u; i < 33u; i++)
        (void)muhtari__bus_read(m, 0xc000u);
    CHECK((m->ym.status & 1u) != 0u);
    CHECK(m->ym.total_clocks > output_steps);
    CHECK(m->ym.timer_deadline[0] > timer_deadline);
    uint64_t running_deadline = m->ym.timer_deadline[0];
    muhtari__bus_write(m, 0x0470u, 0x84u);
    CHECK((m->ym.status & 1u) != 0u);
    CHECK_EQ_U64(m->ym.timer_deadline[0], running_deadline);

    /* A status-reset data write exactly on the timer edge sees the due chip
       event first, then applies the reset in the same bus data phase. */
    muhtari *edge = make_machine();
    CHECK(edge != NULL);
    if (edge != NULL) {
        CHECK_EQ_U64(ym_load_variant(edge, ym_image, bytes, MUHTARI_EXPANSION_XM),
                     MUHTARI_STATUS_OK);
        edge->cart.expansion.control = 0x80u;
        ym_write_reg(edge, 0x10u, 0xffu, edge->bus.ticks);
        ym_write_reg(edge, 0x11u, 3u, edge->bus.ticks);
        ym_write_reg(edge, 0x14u, 0x05u, edge->bus.ticks);
        uint64_t due_tick = muhtari__ym2151_next_timer_tick(edge);
        CHECK(due_tick >= edge->ym.last_tick + 4u);
        edge->bus.ticks = due_tick - 4u;
        edge->ym.last_tick = due_tick - 4u;
        edge->ym.clocks = edge->ym.timer_deadline[0] - 2u;
        edge->ym.address = 0x14u;
        muhtari__bus_write(edge, 0x0461u, 0x15u);
        CHECK_EQ_U64(edge->ym.status & 1u, 0u);
        CHECK_EQ_U64(edge->ym.timer_running[0], 1u);
        muhtari_destroy(edge);
    }

    /* Refusal cases are validated before allocation or mutation of the live
       machine, YM instance, or battery bytes/generation. */
    uint8_t *rom_before = m->cart.rom;
    uint64_t ticks_before = m->bus.ticks;
    struct muhtari__ym2151 ym_before = m->ym;
    uint64_t generation_before = m->battery.generation;
    size_t battery_bytes = m->battery.bytes;
    uint8_t *battery_before = malloc(battery_bytes);
    CHECK(battery_before != NULL);
    if (battery_before != NULL)
        memcpy(battery_before, m->battery.data, battery_bytes);
    m->ram[0] = 0xa5u;
    m->battery.data[0] = 0x39u;
    m->battery.generation = 17u;
    generation_before = m->battery.generation;
    if (battery_before != NULL)
        memcpy(battery_before, m->battery.data, battery_bytes);

#define YM_REFUSE(expected_status, expansion_value)                                                 \
    do {                                                                                            \
        uint32_t allocs_before = counts.allocs;                                                     \
        uint32_t frees_before = counts.frees;                                                       \
        CHECK_EQ_U64(ym_load_variant(m, variant, bytes, (expansion_value)),                        \
                     (expected_status));                                                           \
        CHECK_EQ_U64(counts.allocs, allocs_before);                                                 \
        CHECK_EQ_U64(counts.frees, frees_before);                                                   \
        CHECK(m->cart.rom == rom_before);                                                            \
        CHECK_EQ_U64(m->bus.ticks, ticks_before);                                                    \
        CHECK(memcmp(&m->ym, &ym_before, sizeof ym_before) == 0);                                   \
        CHECK_EQ_U64(m->battery.generation, generation_before);                                     \
        CHECK_EQ_U64(m->battery.bytes, battery_bytes);                                              \
        CHECK(m->ram[0] == 0xa5u);                                                                  \
        CHECK(battery_before == NULL || memcmp(m->battery.data, battery_before, battery_bytes) == 0); \
    } while (0)

    memcpy(variant, ym_image, bytes);
    variant[63] = 0u; /* YM declared without XM */
    YM_REFUSE(MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE, MUHTARI_EXPANSION_AUTO);
    memcpy(variant, ym_image, bytes);
    YM_REFUSE(MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE, MUHTARI_EXPANSION_OFF);
    memcpy(variant, ym_image, bytes);
    YM_REFUSE(MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE, MUHTARI_EXPANSION_XBOARD);
    memcpy(variant, ym_image, bytes);
    variant[67] = 0x18u; /* YM plus unsupported Covox */
    YM_REFUSE(MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE, MUHTARI_EXPANSION_XM);
    memcpy(variant, ym_image, bytes);
    variant[67] = 0x28u; /* YM plus unsupported ADPCM */
    YM_REFUSE(MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE, MUHTARI_EXPANSION_XM);
    memcpy(variant, ym_image, bytes);
    variant[67] = 0x0du; /* YM plus conflicting POKEY at $4000 */
    YM_REFUSE(MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE, MUHTARI_EXPANSION_XM);
    memcpy(variant, ym_image, bytes);
    variant[64] = 4u; /* valid Souper mapper */
    YM_REFUSE(MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD, MUHTARI_EXPANSION_XM);
#undef YM_REFUSE

    /* Selected and auto-detected XM both install the declaration. Plain XM,
       v3, headerless and ignored v4 data never infer a YM or IRQ source. */
    memcpy(variant, ym_image, bytes);
    muhtari *auto_xm = make_machine();
    CHECK(auto_xm != NULL);
    if (auto_xm != NULL) {
        CHECK_EQ_U64(ym_load_variant(auto_xm, variant, bytes, MUHTARI_EXPANSION_AUTO),
                     MUHTARI_STATUS_OK);
        CHECK_EQ_U64(auto_xm->synth.ym_installed, 1u);
        muhtari_destroy(auto_xm);
    }
    memcpy(variant, ym_image, bytes);
    variant[67] = 0u;
    variant[69] = 0u;
    muhtari *plain_xm = make_machine();
    CHECK(plain_xm != NULL);
    if (plain_xm != NULL) {
        CHECK_EQ_U64(ym_load_variant(plain_xm, variant, bytes, MUHTARI_EXPANSION_XM),
                     MUHTARI_STATUS_OK);
        CHECK_EQ_U64(plain_xm->synth.ym_installed, 0u);
        muhtari_destroy(plain_xm);
    }
    memcpy(variant, ym_image, bytes);
    variant[0] = 3u;
    muhtari *v3 = make_machine();
    CHECK(v3 != NULL);
    if (v3 != NULL) {
        CHECK_EQ_U64(ym_load_variant(v3, variant, bytes, MUHTARI_EXPANSION_AUTO),
                     MUHTARI_STATUS_OK);
        CHECK_EQ_U64(v3->synth.ym_installed, 0u);
        CHECK_EQ_U64(v3->cart.irq_mask, 0u);
        muhtari_destroy(v3);
    }
    memcpy(variant, ym_image, bytes);
    variant[64] = 9u; /* malformed v4 block is ignored */
    muhtari *ignored_v4 = make_machine();
    CHECK(ignored_v4 != NULL);
    if (ignored_v4 != NULL) {
        CHECK_EQ_U64(ym_load_variant(ignored_v4, variant, bytes, MUHTARI_EXPANSION_AUTO),
                     MUHTARI_STATUS_OK);
        CHECK_EQ_U64(ignored_v4->synth.ym_installed, 0u);
        CHECK_EQ_U64(ignored_v4->cart.irq_mask, 0u);
        muhtari_destroy(ignored_v4);
    }
    uint8_t headerless[16u * IMAGES_KIB];
    size_t headerless_bytes = images_headerless(headerless, sizeof headerless);
    muhtari *legacy = make_machine();
    CHECK(legacy != NULL);
    if (legacy != NULL) {
        CHECK_EQ_U64(ym_load_variant(legacy, headerless, headerless_bytes, MUHTARI_EXPANSION_XM),
                     MUHTARI_STATUS_OK);
        CHECK_EQ_U64(legacy->synth.ym_installed, 0u);
        CHECK_EQ_U64(legacy->cart.irq_mask, 0u);
        muhtari_destroy(legacy);
    }

    free(battery_before);
    muhtari_destroy(m);
    CHECK_EQ_U64(counts.allocs, counts.frees);
}

static void ym_clock_region(uint32_t region)
{
    size_t bytes = images_ym2151(ym_image);
    muhtari_media media = media_for(ym_image, bytes, MUHTARI_EXPANSION_XM);
    media.region = region;
    muhtari *m = make_machine();
    CHECK(m != NULL);
    if (m == NULL)
        return;
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    uint64_t clock_origin = m->ym.last_tick;
    uint64_t sample_origin = m->synth.origin;

    int16_t audio[AUDIO_CAPACITY];
    for (unsigned i = 0u; i < 2u; i++) {
        uint32_t count = 0u;
        CHECK_EQ_U64(frame(m, audio, &count), MUHTARI_STATUS_OK);
        CHECK(count <= AUDIO_CAPACITY);
        CHECK_EQ_U64(m->ym.last_tick, m->bus.ticks);
        CHECK_EQ_U64(m->ym.clocks, (m->bus.ticks - clock_origin) / 2u);
        CHECK_EQ_U64(m->ym.clock_remainder, (m->bus.ticks - clock_origin) & 1u);
        CHECK_EQ_U64(m->ym.sample_divider, m->ym.clocks % 64u);
        CHECK_EQ_U64(m->ym.total_clocks, (m->ym.clocks / 64u) & 0xffu);
    }

    /* A slow TIA access, RDY hold, and /HALT-held cycles all consume the
       same absolute master tick clock as normal frame DMA. */
    uint64_t before = m->bus.ticks;
    (void)muhtari__bus_read(m, 0x0004u);
    CHECK_EQ_U64(m->ym.last_tick, m->bus.ticks);
    CHECK_EQ_U64(m->ym.clocks, (m->bus.ticks - clock_origin) / 2u);
    CHECK(m->bus.ticks - before >= 6u);

    before = m->bus.ticks;
    m->bus.rdy_release = before + 80u;
    muhtari__bus_rearm(m);
    (void)muhtari__bus_read(m, 0x0004u);
    CHECK(m->bus.ticks >= before + 80u);
    CHECK_EQ_U64(m->ym.last_tick, m->bus.ticks);
    CHECK_EQ_U64(m->ym.clocks, (m->bus.ticks - clock_origin) / 2u);

    before = m->bus.ticks;
    m->bus.halt_stage = 1u; /* HALT_REQUESTED */
    m->bus.halt_at = before + 1u;
    m->bus.halt_until = before + 64u;
    m->bus.halt_cycles = 0u;
    muhtari__bus_rearm(m);
    (void)muhtari__bus_read(m, 0x0004u);
    CHECK(m->bus.ticks > before);
    CHECK_EQ_U64(m->ym.last_tick, m->bus.ticks);
    CHECK_EQ_U64(m->ym.clocks, (m->bus.ticks - clock_origin) / 2u);

    uint64_t sample_num = region == MUHTARI_REGION_PAL ? 4000u : 88u;
    uint64_t sample_den = region == MUHTARI_REGION_PAL ? 591149u : 13125u;
    CHECK_EQ_U64(m->synth.phase,
                 ((m->synth.horizon - sample_origin) * sample_num) % sample_den);
    muhtari_destroy(m);
}

static void ym_clock(void)
{
    ym_clock_region(MUHTARI_REGION_NTSC);
    ym_clock_region(MUHTARI_REGION_PAL);
}

static void ym_irq(void)
{
    uint8_t image[YM_IMAGE_BYTES];
    size_t bytes = images_ym2151(image);
    for (unsigned wired = 0u; wired < 2u; wired++) {
        image[67] = 0x0au; /* supported POKEY $0450 alongside YM */
        image[69] = wired != 0u ? 0x05u : 0x01u;
        muhtari_media media = media_for(image, bytes, MUHTARI_EXPANSION_XM);
        muhtari *m = make_machine();
        CHECK(m != NULL);
        if (m == NULL)
            continue;
        CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
        CHECK_EQ_U64(m->cart.irq_mask, wired != 0u ? 5u : 1u);
        uint64_t tick = m->bus.ticks;
        ym_write_reg(m, 0x10u, 0xffu, tick);
        ym_write_reg(m, 0x11u, 3u, tick);
        ym_write_reg(m, 0x14u, 0x05u, tick);
        muhtari__bus_rearm(m);
        uint64_t due = muhtari__ym2151_next_timer_tick(m);
        CHECK(due != 0u);
        if (muhtari__maria_next_event(m) > due && muhtari__ports_next_event(m) > due)
            CHECK_EQ_U64(m->bus.next_deadline, due);

        for (unsigned cycle = 0u; cycle < 40u; cycle++)
            (void)muhtari__bus_read(m, 0xc000u);
        CHECK_EQ_U64(m->ym.status & 1u, 1u);
        if (wired != 0u) {
            CHECK_EQ_U64(m->bus.irq_pipe, 7u);
            CHECK_EQ_U64(m->bus.irq_level, 1u);
        } else {
            CHECK_EQ_U64(m->bus.irq_pipe, 0u);
            CHECK_EQ_U64(m->bus.irq_level, 0u);
        }
        muhtari_destroy(m);
    }
}

struct ym_mix_pair {
    int16_t left;
    int16_t right;
};

static int32_t ym_decode_word(uint16_t bits)
{
    return (bits & 0x8000u) != 0u ? (int32_t)bits - 65536 : (int32_t)bits;
}

static int32_t ym_expected_level(int32_t base, int16_t left, int16_t right)
{
    int32_t mono = ((int32_t)left + (int32_t)right) / 2;
    return base / 2 + (mono * 4590) / 32768;
}

static void ym_synth_base(muhtari *m, int maximum)
{
    if (maximum == 0)
        return;
    for (unsigned source = 0u; source < 2u; source++)
        muhtari__synth_level(m, 0u, source, 15u);
    for (unsigned source = 2u; source < 10u; source++)
        muhtari__synth_level(m, 0u, source, 206u);
}

static void ym_mix_extrema(void)
{
    static const struct ym_mix_pair pairs[] = {
        {-32768, -32768}, {32767, 32767}, {-32768, 32767},
        {-32768, -32767}, {32767, 32766}, {-1, 0},
    };
    for (unsigned region_index = 0u; region_index < 2u; region_index++) {
        uint32_t region = region_index == 0u ? MUHTARI_REGION_NTSC : MUHTARI_REGION_PAL;
        struct muhtari__timing timing = muhtari__timing_for_region(region);
        for (unsigned maximum = 0u; maximum < 2u; maximum++) {
            for (size_t pair = 0u; pair < sizeof pairs / sizeof pairs[0]; pair++) {
                muhtari *m = calloc(1u, sizeof *m);
                CHECK(m != NULL);
                if (m == NULL)
                    continue;
                m->effective_region = region;
                muhtari__synth_reset(m, 0u, 2u);
                m->synth.ym_installed = 1u;
                ym_synth_base(m, (int)maximum);
                muhtari__synth_level(m, 0u, 10u, (uint16_t)pairs[pair].left);
                muhtari__synth_level(m, 0u, 11u, (uint16_t)pairs[pair].right);

                /* This is the production event flush, mixer, kernel, Q23
                   integrator, Q30 blocker, and PCM output path. */
                int16_t pcm[AUDIO_CAPACITY];
                uint32_t count = muhtari__synth_render(m, timing.frame_ticks, pcm);
                int32_t base = maximum != 0 ? 18360 : 0;
                int32_t want = ym_expected_level(base, pairs[pair].left, pairs[pair].right);
                CHECK_EQ_U64(m->synth.mixed_level, (uint64_t)(int64_t)want);
                CHECK(want >= -4590 && want <= 13769);
                CHECK(count <= timing.max_samples);
                CHECK_EQ_U64(m->synth.clipped_samples, 0u);
                CHECK(m->synth.pre_clip_peak <= INT16_MAX);
                for (uint32_t i = 0u; i < count; i++)
                    CHECK(pcm[i] >= INT16_MIN && pcm[i] <= INT16_MAX);
                if (maximum == 0u) {
                    CHECK(want >= -4590 && want <= 4589);
                } else {
                    CHECK(want >= 4590 && want <= 13769);
                }
                free(m);
            }
        }
    }

    /* The signed YM words are ignored unless the topology was installed;
       PCM and mixer state therefore retain the old no-YM result. */
    for (unsigned region_index = 0u; region_index < 2u; region_index++) {
        uint32_t region = region_index == 0u ? MUHTARI_REGION_NTSC : MUHTARI_REGION_PAL;
        struct muhtari__timing timing = muhtari__timing_for_region(region);
        for (unsigned maximum = 0u; maximum < 2u; maximum++) {
            muhtari *plain = calloc(1u, sizeof *plain);
            muhtari *ignored = calloc(1u, sizeof *ignored);
            CHECK(plain != NULL && ignored != NULL);
            if (plain == NULL || ignored == NULL) {
                free(plain);
                free(ignored);
                continue;
            }
            plain->effective_region = ignored->effective_region = region;
            muhtari__synth_reset(plain, 0u, 2u);
            muhtari__synth_reset(ignored, 0u, 2u);
            ym_synth_base(plain, (int)maximum);
            ym_synth_base(ignored, (int)maximum);
            muhtari__synth_level(ignored, 0u, 10u, 0x8000u);
            muhtari__synth_level(ignored, 0u, 11u, 0x7fffu);
            int16_t expected[AUDIO_CAPACITY];
            int16_t actual[AUDIO_CAPACITY];
            uint32_t a = muhtari__synth_render(plain, timing.frame_ticks, expected);
            uint32_t b = muhtari__synth_render(ignored, timing.frame_ticks, actual);
            CHECK_EQ_U64(a, b);
            CHECK(memcmp(expected, actual, a * sizeof expected[0]) == 0);
            CHECK_EQ_U64(plain->synth.mixed_level, ignored->synth.mixed_level);
            free(ignored);
            free(plain);
        }
    }

    /* Actual OPM output samples enter the production mixer as signed values.
       Check every output edge against an independent widened formula. */
    muhtari *m = ym_test_chip();
    CHECK(m != NULL);
    if (m == NULL)
        return;
    m->synth.ym_installed = 1u;
    for (size_t i = 0u; i < sizeof ym_common / sizeof ym_common[0]; i++)
        ym_write_reg(m, ym_common[i].reg, ym_common[i].value, 0u);
    int32_t minimum[2] = {INT32_MAX, INT32_MAX};
    int32_t maximum[2] = {INT32_MIN, INT32_MIN};
    for (uint64_t sample = 1u; sample <= 256u; sample++) {
        uint64_t edge = sample * 128u + 1u;
        ym_advance_and_discard(m, edge);
        (void)muhtari__synth_render(m, edge + 1u, NULL);
        for (unsigned channel = 0u; channel < 2u; channel++) {
            int32_t output = m->ym.output[channel];
            if (output < minimum[channel])
                minimum[channel] = output;
            if (output > maximum[channel])
                maximum[channel] = output;
        }
        CHECK_EQ_U64(m->synth.raw[10], (uint16_t)m->ym.output[0]);
        CHECK_EQ_U64(m->synth.raw[11], (uint16_t)m->ym.output[1]);
        CHECK_EQ_U64(m->synth.mixed_level,
                     (uint64_t)(int64_t)ym_expected_level(0, m->ym.output[0], m->ym.output[1]));
        CHECK(m->synth.mixed_level >= -4590 && m->synth.mixed_level <= 4589);
    }
    for (unsigned channel = 0u; channel < 2u; channel++) {
        CHECK(minimum[channel] < 0);
        CHECK(maximum[channel] > 0);
    }
    CHECK_EQ_U64(m->synth.clipped_samples, 0u);
    CHECK(m->synth.pre_clip_peak <= INT16_MAX);
    free(m);
}

struct ym_observed_event {
    uint64_t tick;
    uint16_t raw;
    uint8_t source;
};
struct ym_observed_events {
    uint32_t count;
    uint32_t source_count[2];
    uint32_t negative[2];
    uint32_t positive[2];
    uint8_t have_previous;
    struct ym_observed_event previous;
    uint8_t first_count;
    struct ym_observed_event first[4];
};

static void ym_observe_collect(void *user, uint64_t tick, uint8_t source, uint16_t raw)
{
    struct ym_observed_events *events = user;
    if (events->have_previous != 0u) {
        CHECK(tick >= events->previous.tick);
        if (tick == events->previous.tick)
            CHECK(source >= events->previous.source);
    }
    if (events->first_count < sizeof events->first / sizeof events->first[0])
        events->first[events->first_count++] = (struct ym_observed_event){tick, raw, source};
    if (source == 10u || source == 11u) {
        unsigned channel = source - 10u;
        int32_t value = ym_decode_word(raw);
        events->source_count[channel]++;
        events->negative[channel] += value < 0;
        events->positive[channel] += value > 0;
    }
    events->count++;
    events->previous = (struct ym_observed_event){tick, raw, source};
    events->have_previous = 1u;
}

static uint32_t ym_frame_sample_count(muhtari *m)
{
    struct muhtari__timing timing = muhtari__timing(m);
    uint64_t elapsed = m->loaded != 0u ? m->frame_deadline - m->frame_start : timing.frame_ticks;
    return (uint32_t)(((uint64_t)m->audio_rem + elapsed * timing.sample_num) / timing.sample_den);
}

static void ym_observer_extrema(void)
{
    struct ym_observed_events events = {0};
    muhtari *m = make_machine();
    CHECK(m != NULL);
    if (m == NULL)
        return;
    CHECK_EQ_U64(muhtari__synth_observe(m, ym_observe_collect, &events), MUHTARI_STATUS_OK);
    muhtari__observe_begin(m, 10u);
    muhtari__synth_raw(m, 2u, 10u, 0x8000u);
    muhtari__synth_raw(m, 2u, 11u, 0x7fffu);
    muhtari__observe_end(m);
    CHECK_EQ_U64(events.count, 2u);
    CHECK_EQ_U64(events.first[0].source, 10u);
    CHECK_EQ_U64(events.first[0].raw, 0x8000u);
    CHECK_EQ_U64(ym_decode_word(events.first[0].raw), -32768);
    CHECK_EQ_U64(events.first[1].source, 11u);
    CHECK_EQ_U64(events.first[1].raw, 0x7fffu);
    CHECK_EQ_U64(ym_decode_word(events.first[1].raw), 32767);

    /* A future half-open event is retained with its signed word intact and
       delivered at the next frame start in canonical source order. */
    memset(&events, 0, sizeof events);
    muhtari__observe_begin(m, 10u);
    muhtari__synth_raw(m, 12u, 10u, 0xffffu);
    muhtari__synth_raw(m, 12u, 11u, 0x8001u);
    muhtari__observe_end(m);
    CHECK_EQ_U64(events.count, 0u);
    muhtari__observe_begin(m, 13u);
    CHECK_EQ_U64(events.count, 2u);
    CHECK_EQ_U64(events.first[0].tick, 12u);
    CHECK_EQ_U64(events.first[0].source, 10u);
    CHECK_EQ_U64(events.first[0].raw, 0xffffu);
    CHECK_EQ_U64(events.first[1].tick, 12u);
    CHECK_EQ_U64(events.first[1].source, 11u);
    CHECK_EQ_U64(events.first[1].raw, 0x8001u);

    /* Fill the complete derived carry bound using twelve changing sources
       on each overshoot tick, then drain through the same callback path. */
    memset(&events, 0, sizeof events);
    const uint64_t carry_ticks = MUHTARI__SYNTH_OVERSHOOT_TICKS + 1u;
    const uint64_t carry_records = 12u * carry_ticks;
    const uint64_t carry_start = 100u;
    CHECK_EQ_U64(MUHTARI__SYNTH_OVERSHOOT_TICKS, 20730u);
    muhtari__observe_begin(m, carry_start);
    for (uint64_t offset = 0u; offset < carry_ticks; offset++) {
        uint64_t tick = carry_start + offset;
        uint16_t level = (uint16_t)((tick & 1u) + 1u);
        for (unsigned source = 0u; source < 10u; source++)
            muhtari__synth_raw(m, tick, source, level);
        muhtari__synth_raw(m, tick, 10u, (uint16_t)(0x8000u | (tick & 1u)));
        muhtari__synth_raw(m, tick, 11u, (uint16_t)(0x7fffu - (tick & 1u)));
    }
    muhtari__observe_end(m);
    CHECK_EQ_U64(events.count, 0u);
    muhtari__observe_begin(m, carry_start + carry_ticks);
    CHECK_EQ_U64(events.count, carry_records);
    CHECK_EQ_U64(events.source_count[0], carry_ticks);
    CHECK_EQ_U64(events.source_count[1], carry_ticks);
    CHECK_EQ_U64(events.previous.tick, carry_start + carry_ticks - 1u);
    CHECK_EQ_U64(events.previous.source, 11u);
    muhtari_destroy(m);
}

static void ym_observer_public_region(uint32_t region)
{
    struct ym_allocations allocations = {0u, 0u};
    struct ym_observed_events events = {0};
    size_t bytes = images_ym2151(ym_image);
    muhtari_media media = media_for(ym_image, bytes, MUHTARI_EXPANSION_XM);
    media.region = region;
    muhtari *m = make_machine_counted(&allocations);
    CHECK(m != NULL);
    if (m == NULL)
        return;
    CHECK_EQ_U64(muhtari__synth_observe(m, ym_observe_collect, &events), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(m->synth.ym_installed, 1u);
    muhtari_info info;
    memset(&info, 0, sizeof info);
    info.size = (uint32_t)sizeof info;
    CHECK_EQ_U64(muhtari_get_info(m, &info), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(info.sample_rate_num, 48000u);
    CHECK_EQ_U64(info.sample_rate_den, 1u);
    m->cart.expansion.control = 0x80u;
    for (size_t i = 0u; i < sizeof ym_common / sizeof ym_common[0]; i++)
        ym_write_reg(m, ym_common[i].reg, ym_common[i].value, m->bus.ticks);

    uint32_t allocated_after_load = allocations.allocs;
    uint32_t freed_after_load = allocations.frees;
    muhtari_frame io;
    memset(&io, 0, sizeof io);
    io.size = (uint32_t)sizeof io;
    io.audio = NULL;
    io.audio_capacity = 0u;
    CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_OK);
    CHECK(io.audio_count <= muhtari__timing(m).max_samples);
    CHECK_EQ_U64(allocations.allocs, allocated_after_load);
    CHECK_EQ_U64(allocations.frees, freed_after_load);

    int16_t guarded[AUDIO_CAPACITY + 8u];
    uint32_t required = ym_frame_sample_count(m);
    CHECK(required > 0u && required <= muhtari__timing(m).max_samples);
    for (size_t i = 0u; i < sizeof guarded / sizeof guarded[0]; i++)
        guarded[i] = (int16_t)0x5a5a;
    io.audio = guarded;
    io.audio_capacity = required - 1u;
    uint64_t ticks_before = m->bus.ticks;
    uint32_t events_before = events.count;
    CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_ERR_BUFFER_TOO_SMALL);
    CHECK_EQ_U64(io.audio_count, 0u);
    CHECK_EQ_U64(m->bus.ticks, ticks_before);
    CHECK_EQ_U64(events.count, events_before);
    for (size_t i = 0u; i < sizeof guarded / sizeof guarded[0]; i++)
        CHECK_EQ_U64((uint16_t)guarded[i], 0x5a5au);

    uint32_t ym_count_before[2] = {events.source_count[0], events.source_count[1]};
    int16_t pcm_min = INT16_MAX, pcm_max = INT16_MIN;
    for (unsigned frame_index = 0u; frame_index < 3u; frame_index++) {
        required = ym_frame_sample_count(m);
        for (size_t i = 0u; i < sizeof guarded / sizeof guarded[0]; i++)
            guarded[i] = (int16_t)0x5a5a;
        io.audio_capacity = required;
        uint64_t audio_end = m->frame_deadline;
        CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_OK);
        CHECK_EQ_U64(io.audio_count, required);
        for (uint32_t i = io.audio_count; i < sizeof guarded / sizeof guarded[0]; i++)
            CHECK_EQ_U64((uint16_t)guarded[i], 0x5a5au);
        for (uint32_t i = 0u; i < io.audio_count; i++) {
            if (guarded[i] < pcm_min)
                pcm_min = guarded[i];
            if (guarded[i] > pcm_max)
                pcm_max = guarded[i];
        }
        CHECK(events.previous.tick < audio_end);
        CHECK_EQ_U64(allocations.allocs, allocated_after_load);
        CHECK_EQ_U64(allocations.frees, freed_after_load);
    }
    CHECK(events.source_count[0] > ym_count_before[0]);
    CHECK(events.source_count[1] > ym_count_before[1]);
    CHECK(events.negative[0] > 0u && events.positive[0] > 0u);
    CHECK(events.negative[1] > 0u && events.positive[1] > 0u);
    CHECK(pcm_min < 0 && pcm_max > 0);
    muhtari_destroy(m);
    CHECK_EQ_U64(allocations.allocs, allocations.frees);
}

static void ym_observer(void)
{
    ym_observer_extrema();
    ym_observer_public_region(MUHTARI_REGION_NTSC);
    ym_observer_public_region(MUHTARI_REGION_PAL);
}

static void ym_make_legacy_image(uint8_t *image)
{
    images_header(image, 4u, "YM lifecycle control", YM_BODY, 0u);
    images_fill(image + IMAGES_HEADER, YM_BODY, 0x2151u);
    images_footer(image + IMAGES_HEADER, YM_BODY);
}

static void ym_lifecycle(void)
{
    struct ym_allocations a_alloc = {0u, 0u};
    struct ym_observed_events events = {0};
    size_t bytes = images_ym2151(ym_image);
    muhtari_media ntsc_media = media_for(ym_image, bytes, MUHTARI_EXPANSION_XM);
    muhtari_media pal_media = ntsc_media;
    pal_media.region = MUHTARI_REGION_PAL;
    muhtari *a = make_machine_counted(&a_alloc);
    muhtari *pal = make_machine();
    muhtari *pal_reference = make_machine();
    muhtari *fresh = make_machine();
    CHECK(a != NULL && pal != NULL && pal_reference != NULL && fresh != NULL);
    if (a == NULL || pal == NULL || pal_reference == NULL || fresh == NULL) {
        muhtari_destroy(fresh);
        muhtari_destroy(pal_reference);
        muhtari_destroy(pal);
        muhtari_destroy(a);
        return;
    }
    CHECK_EQ_U64(muhtari__synth_observe(a, ym_observe_collect, &events), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari_load(a, &ntsc_media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari_load(pal, &pal_media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari_load(pal_reference, &pal_media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari_load(fresh, &ntsc_media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(a->synth.ym_installed, 1u);
    CHECK_EQ_U64(pal->synth.ym_installed, 1u);
    CHECK(muhtari_state_size(a) > 64u);

    /* Interleave different regions, then require the PAL instance to match
       its isolated control byte-for-byte across chip and scheduler state. */
    int16_t pal_audio[2][AUDIO_CAPACITY];
    uint32_t count[2] = {0u, 0u};
    uint32_t pal_first_count = ym_frame_sample_count(pal);
    CHECK_EQ_U64(frame(pal, pal_audio[0], &count[0]), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(count[0], pal_first_count);
    int16_t ntsc_audio[AUDIO_CAPACITY];
    for (unsigned i = 0u; i < 3u; i++)
        CHECK_EQ_U64(frame(a, ntsc_audio, &count[0]), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(frame(pal, pal_audio[1], &count[1]), MUHTARI_STATUS_OK);
    uint32_t reference_count[2] = {0u, 0u};
    int16_t reference_audio[2][AUDIO_CAPACITY];
    CHECK_EQ_U64(frame(pal_reference, reference_audio[0], &reference_count[0]),
                 MUHTARI_STATUS_OK);
    CHECK_EQ_U64(frame(pal_reference, reference_audio[1], &reference_count[1]),
                 MUHTARI_STATUS_OK);
    CHECK_EQ_U64(count[1], reference_count[1]);
    CHECK(memcmp(pal_audio[1], reference_audio[1],
                 count[1] * sizeof pal_audio[1][0]) == 0);
    CHECK(memcmp(&pal->ym, &pal_reference->ym, sizeof pal->ym) == 0);
    CHECK(memcmp(&pal->synth, &pal_reference->synth, sizeof pal->synth) == 0);
    CHECK(memcmp(&pal->cpu, &pal_reference->cpu, sizeof pal->cpu) == 0);
    CHECK(memcmp(&pal->bus, &pal_reference->bus, sizeof pal->bus) == 0);
    CHECK(memcmp(pal->ram, pal_reference->ram, sizeof pal->ram) == 0);
    CHECK(a->ym.clocks > 0u && a->ym.total_clocks > 0u);
    CHECK(pal->ym.clocks > a->ym.clocks);

    /* Power-cycle the active instance against a fresh equivalent machine.
       Compare the complete chip and synth state, including timers, busy,
       phase, output and operator/channel caches. */
    CHECK_EQ_U64(muhtari_power_cycle(a), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari_power_cycle(fresh), MUHTARI_STATUS_OK);
    CHECK(memcmp(&a->ym, &fresh->ym, sizeof a->ym) == 0);
    CHECK(memcmp(&a->synth, &fresh->synth, sizeof a->synth) == 0);
    CHECK_EQ_U64(a->ym.clocks, 0u);
    CHECK_EQ_U64(a->ym.sample_divider, 0u);
    CHECK_EQ_U64(a->ym.busy_end, 0u);
    CHECK_EQ_U64(a->ym.status, 0u);
    CHECK_EQ_U64(a->ym.timer_running[0], 0u);
    CHECK_EQ_U64(a->ym.timer_running[1], 0u);
    CHECK_EQ_U64(a->ym.output[0], 0u);
    CHECK_EQ_U64(a->ym.output[1], 0u);
    CHECK_EQ_U64(a->synth.raw[10], 0u);
    CHECK_EQ_U64(a->synth.raw[11], 0u);
    CHECK_EQ_U64(a->synth.mixed_level, 0u);
    CHECK_EQ_U64(a->synth.pending, 0u);
    CHECK_EQ_U64(a->synth.pcm_count, 0u);
    CHECK(muhtari_state_size(a) > 64u);
    CHECK(a->observer == ym_observe_collect);

    memset(&events, 0, sizeof events);
    int16_t after_reset[AUDIO_CAPACITY], fresh_after_reset[AUDIO_CAPACITY];
    uint32_t after_count = 0u, fresh_count = 0u;
    CHECK_EQ_U64(frame(a, after_reset, &after_count), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(frame(fresh, fresh_after_reset, &fresh_count), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(after_count, fresh_count);
    CHECK(memcmp(after_reset, fresh_after_reset, after_count * sizeof after_reset[0]) == 0);
    CHECK(memcmp(&a->ym, &fresh->ym, sizeof a->ym) == 0);
    CHECK(memcmp(&a->synth, &fresh->synth, sizeof a->synth) == 0);

    uint8_t refused[YM_IMAGE_BYTES];
    memcpy(refused, ym_image, bytes);
    refused[63] = 0u;
    muhtari_media refused_media = media_for(refused, bytes, MUHTARI_EXPANSION_AUTO);
    uint8_t *rom_before = a->cart.rom;
    struct muhtari__ym2151 ym_before = a->ym;
    uint32_t allocs_before = a_alloc.allocs;
    uint32_t frees_before = a_alloc.frees;
    CHECK_EQ_U64(muhtari_load(a, &refused_media), MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE);
    CHECK(a->cart.rom == rom_before);
    CHECK(memcmp(&a->ym, &ym_before, sizeof ym_before) == 0);
    CHECK_EQ_U64(a_alloc.allocs, allocs_before);
    CHECK_EQ_U64(a_alloc.frees, frees_before);

    /* Replacing content with an accepted non-YM image is the public unload
       transition; reloading XM reinstalls a reset YM with a valid envelope. */
    ym_make_legacy_image(old_image);
    muhtari_media legacy_media = media_for(old_image, YM_IMAGE_BYTES, MUHTARI_EXPANSION_OFF);
    CHECK_EQ_U64(muhtari_load(a, &legacy_media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(a->synth.ym_installed, 0u);
    CHECK(muhtari_state_size(a) > 0u);
    CHECK_EQ_U64(muhtari_load(a, &ntsc_media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(a->synth.ym_installed, 1u);
    CHECK(muhtari_state_size(a) > 64u);
    CHECK_EQ_U64(a->ym.clocks, 0u);
    CHECK_EQ_U64(a->ym.output[0], 0u);
    CHECK_EQ_U64(a->ym.output[1], 0u);
    CHECK(a->observer == ym_observe_collect);
    CHECK(muhtari_state_size(a) > 64u);

    muhtari_destroy(fresh);
    muhtari_destroy(pal_reference);
    muhtari_destroy(pal);
    muhtari_destroy(a);
    CHECK_EQ_U64(a_alloc.allocs, a_alloc.frees);
}

static size_t ym_emit_write(uint8_t *body, size_t at, uint8_t reg, uint8_t value)
{
    static const uint8_t address_write[] = {0xa9u, 0u, 0x8du, 0x60u, 0x04u};
    static const uint8_t data_write[] = {0xa9u, 0u, 0x8du, 0x61u, 0x04u};
    memcpy(body + at, address_write, sizeof address_write);
    at += sizeof address_write;
    body[at - 4u] = reg;
    memcpy(body + at, data_write, sizeof data_write);
    at += sizeof data_write;
    body[at - 4u] = value;
    return at;
}

static size_t ym_emit_control(uint8_t *body, size_t at, uint8_t control)
{
    static const uint8_t write_control[] = {0xa9u, 0u, 0x8du, 0x70u, 0x04u};
    memcpy(body + at, write_control, sizeof write_control);
    body[at + 1u] = control;
    return at + sizeof write_control;
}

static size_t ym_hostile_image(uint8_t *image, int jammed)
{
    size_t bytes = images_ym2151(image);
    uint8_t *body = image + IMAGES_HEADER;
    memset(body, 0xeau, YM_BODY);
    body[0] = 0xa2u; /* LDX #$FD: keep IRQ stack in system RAM */
    body[1] = 0xfdu;
    body[2] = 0x9au; /* TXS */
    size_t at = ym_emit_control(body, 3u, 0x84u);
    for (unsigned channel = 0u; channel < 8u; channel++) {
        at = ym_emit_write(body, at, (uint8_t)(0x20u + channel), 0xc7u);
        at = ym_emit_write(body, at, (uint8_t)(0x28u + channel), 0x4cu);
        at = ym_emit_write(body, at, (uint8_t)(0x30u + channel), 0x00u);
    }
    static const uint8_t bases[] = {0x60u, 0x80u, 0xa0u, 0xc0u, 0xe0u, 0x40u};
    static const uint8_t values[] = {0x00u, 0x1fu, 0x00u, 0x00u, 0x0fu, 0x01u};
    static const uint8_t slots[] = {0u, 8u, 16u, 24u};
    for (unsigned channel = 0u; channel < 8u; channel++) {
        for (unsigned group = 0u; group < sizeof bases; group++) {
            for (unsigned slot = 0u; slot < sizeof slots; slot++) {
                at = ym_emit_write(body, at, (uint8_t)(bases[group] + slots[slot] + channel),
                                   values[group]);
            }
        }
    }
    for (unsigned channel = 0u; channel < 8u; channel++)
        at = ym_emit_write(body, at, 0x08u, (uint8_t)(0x78u | channel));
    at = ym_emit_write(body, at, 0x10u, 0xffu);
    at = ym_emit_write(body, at, 0x11u, 3u);
    at = ym_emit_write(body, at, 0x14u, 0x05u); /* minimum-period Timer A IRQ */
    if (jammed != 0) {
        body[at++] = 0x58u; /* CLI immediately before the jam. */
        body[at++] = 0x02u; /* JAM after programming the complete profile */
    } else {
        at = ym_emit_control(body, at, 0x04u); /* D7 off */
        at = ym_emit_write(body, at, 0x28u, 0x7fu); /* ignored data */
        at = ym_emit_control(body, at, 0x84u); /* D7 on */
        at = ym_emit_write(body, at, 0x28u, 0x4au);
        at = ym_emit_write(body, at, 0x28u, 0x4cu);
        at = ym_emit_write(body, at, 0x28u, 0x4au);
        at = ym_emit_write(body, at, 0xa0u, 0x01u);
        at = ym_emit_write(body, at, 0x68u, 0x02u);
        at = ym_emit_write(body, at, 0x48u, 0x03u);
        at = ym_emit_write(body, at, 0x08u, 0x78u);
        at = ym_emit_control(body, at, 0x04u);
        at = ym_emit_write(body, at, 0x2fu, 0x66u); /* ignored while disabled */
        at = ym_emit_control(body, at, 0x84u);
        body[at++] = 0x58u; /* Admit any pending IRQ after all latch/data pairs. */
        size_t spin = at;
        body[at++] = 0x4cu;
        body[at++] = (uint8_t)spin;
        body[at++] = (uint8_t)((0xc000u + spin) >> 8);
    }
    CHECK(at < 0x0ff0u);
    size_t handler = ym_emit_control(body, 0x0ff0u, 0x84u);
    handler = ym_emit_write(body, handler, 0x14u, 0x15u); /* clear flag, keep timer running */
    body[handler++] = 0xeeu; /* INC $1800: count accepted timer IRQ entries */
    body[handler++] = 0x00u;
    body[handler++] = 0x18u;
    body[handler++] = 0x40u; /* RTI */
    body[0x3ffcu] = 0x00u;
    body[0x3ffdu] = 0xc0u;
    body[0x3ffeu] = 0xf0u;
    body[0x3fffu] = 0xcfu;
    images_footer(body, YM_BODY);
    return bytes;
}

static muhtari_status ym_hostile_frame(muhtari *m, int16_t *audio, uint8_t *video,
                                       uint32_t *count)
{
    struct muhtari__timing timing = muhtari__timing(m);
    muhtari_frame io;
    memset(&io, 0, sizeof io);
    io.size = (uint32_t)sizeof io;
    io.audio = audio;
    io.audio_capacity = timing.max_samples;
    io.video = video;
    io.video_pitch = MUHTARI__WIDTH;
    muhtari_status status = muhtari_run_frame(m, &io);
    *count = io.audio_count;
    return status;
}

static void ym_hostile_run(int jammed)
{
    struct ym_allocations allocations = {0u, 0u};
    struct ym_observed_events events = {0};
    uint8_t image[YM_IMAGE_BYTES];
    size_t bytes = ym_hostile_image(image, jammed);
    muhtari_media media = media_for(image, bytes, MUHTARI_EXPANSION_XM);
    muhtari *m = make_machine_counted(&allocations);
    CHECK(m != NULL);
    if (m == NULL)
        return;
    CHECK_EQ_U64(muhtari__synth_observe(m, ym_observe_collect, &events), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    uint32_t allocs_after_load = allocations.allocs;
    uint32_t frees_after_load = allocations.frees;
    uint64_t clock_origin_tick = m->ym.last_tick;
    int16_t audio[AUDIO_CAPACITY + 8u];
    uint8_t video[320u * 292u + 8u];
    unsigned active_frames = 0u;
    while ((m->ym.timer_running[0] == 0u || events.source_count[0] == 0u ||
            events.source_count[1] == 0u || (jammed != 0 && m->cpu.jammed == 0u) ||
            (jammed == 0 && m->ram[0] == 0u)) &&
           active_frames < 6u) {
        struct muhtari__timing timing = muhtari__timing(m);
        uint32_t required = ym_frame_sample_count(m);
        CHECK(required > 0u && required <= timing.max_samples);
        for (size_t i = 0u; i < sizeof audio / sizeof audio[0]; i++)
            audio[i] = (int16_t)0x5a5a;
        memset(video, 0xa7, sizeof video);
        muhtari_frame short_io;
        memset(&short_io, 0, sizeof short_io);
        short_io.size = (uint32_t)sizeof short_io;
        short_io.audio = audio;
        short_io.audio_capacity = required - 1u;
        short_io.video = video;
        short_io.video_pitch = MUHTARI__WIDTH;
        uint64_t before_tick = m->bus.ticks;
        uint64_t deadline = m->frame_deadline;
        CHECK_EQ_U64(muhtari_run_frame(m, &short_io), MUHTARI_STATUS_ERR_BUFFER_TOO_SMALL);
        CHECK_EQ_U64(short_io.audio_count, 0u);
        CHECK_EQ_U64(m->bus.ticks, before_tick);
        for (size_t i = 0u; i < sizeof video / sizeof video[0]; i++)
            CHECK_EQ_U64(video[i], 0xa7u);

        uint32_t count = 0u;
        CHECK_EQ_U64(ym_hostile_frame(m, audio, video, &count), MUHTARI_STATUS_OK);
        CHECK_EQ_U64(count, required);
        CHECK(m->bus.ticks >= deadline);
        CHECK(m->bus.ticks - deadline <= MUHTARI__SYNTH_OVERSHOOT_TICKS);
        CHECK_EQ_U64(m->ym.clocks, (m->ym.last_tick - clock_origin_tick) / 2u);
        CHECK_EQ_U64(m->ym.clock_remainder, (m->ym.last_tick - clock_origin_tick) & 1u);
        for (size_t i = count; i < sizeof audio / sizeof audio[0]; i++)
            CHECK_EQ_U64((uint16_t)audio[i], 0x5a5au);
        size_t used_video = (size_t)timing.height * MUHTARI__WIDTH;
        for (size_t i = used_video; i < sizeof video / sizeof video[0]; i++)
            CHECK_EQ_U64(video[i], 0xa7u);
        CHECK(m->ym.sample_divider < 64u);
        CHECK(muhtari_state_size(m) > 64u);
        CHECK_EQ_U64(allocations.allocs, allocs_after_load);
        CHECK_EQ_U64(allocations.frees, frees_after_load);
        active_frames++;
    }
    CHECK(active_frames < 6u);
    CHECK_EQ_U64(m->cart.irq_mask, 4u);
    CHECK_EQ_U64(m->ym.timer_running[0], 1u);
    CHECK_EQ_U64(m->ym.active_channels, 0xffu);
    if (jammed != 0) {
        CHECK_EQ_U64(m->cpu.jammed, 1u);
    } else {
        CHECK(m->ram[0] > 0u); /* IRQ handler ran through the normal CPU path. */
        CHECK(m->ym.timer_deadline[0] > m->ym.clocks);
    }
    CHECK(events.source_count[0] > 0u && events.source_count[1] > 0u);
    CHECK(events.count < 12u * (m->bus.ticks + 1u));
    CHECK(events.source_count[0] + events.source_count[1] <= 2u * (m->ym.clocks / 64u + 1u));

    /* A jammed CPU keeps issuing bounded open-bus reads while the timer and
       output clocks continue; a live interrupt loop keeps retriggering. */
    if (jammed != 0) {
        struct muhtari__timing timing = muhtari__timing(m);
        uint32_t required = ym_frame_sample_count(m);
        muhtari_frame io;
        memset(&io, 0, sizeof io);
        io.size = (uint32_t)sizeof io;
        io.audio = audio;
        io.audio_capacity = timing.max_samples;
        io.video = video;
        io.video_pitch = MUHTARI__WIDTH;
        uint64_t deadline = m->frame_deadline;
        CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_OK);
        CHECK_EQ_U64(io.audio_count, required);
        CHECK(m->bus.ticks >= deadline);
        CHECK(m->bus.ticks - deadline <= MUHTARI__SYNTH_OVERSHOOT_TICKS);
        CHECK_EQ_U64(m->cpu.jammed, 1u);
        CHECK(m->ym.clocks > 0u);
        CHECK_EQ_U64(allocations.allocs, allocs_after_load);
        CHECK_EQ_U64(allocations.frees, frees_after_load);
    }
    muhtari_destroy(m);
    CHECK_EQ_U64(allocations.allocs, allocations.frees);
}

static void ym_hostile(void)
{
    ym_hostile_run(0);
    ym_hostile_run(1);
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    if (strcmp(argv[1], "tracer") == 0)
        ym_tracer();
    else if (strcmp(argv[1], "numeric") == 0)
        ym_numeric();
    else if (strcmp(argv[1], "synthesis") == 0)
        ym_synthesis();
    else if (strcmp(argv[1], "controls") == 0)
        ym_controls();
    else if (strcmp(argv[1], "profile") == 0)
        ym_profile();
    else if (strcmp(argv[1], "clock") == 0)
        ym_clock();
    else if (strcmp(argv[1], "irq") == 0)
        ym_irq();
    else if (strcmp(argv[1], "mix") == 0)
        ym_mix_extrema();
    else if (strcmp(argv[1], "observer") == 0)
        ym_observer();
    else if (strcmp(argv[1], "lifecycle") == 0)
        ym_lifecycle();
    else if (strcmp(argv[1], "hostile") == 0)
        ym_hostile();
    else
        return 2;
    CHECK_DONE();
}
