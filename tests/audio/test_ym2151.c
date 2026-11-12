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

static uint8_t ym_image[YM_IMAGE_BYTES];
static uint8_t old_image[YM_IMAGE_BYTES];
static uint8_t irq_image[YM_IMAGE_BYTES];

static void *test_alloc(void *user, size_t bytes)
{
    (void)user;
    return malloc(bytes);
}

static void test_free(void *user, void *ptr, size_t bytes)
{
    (void)user;
    (void)bytes;
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
    static const uint32_t kinds[4] = {MUHTARI_MEMORY_SYSTEM_RAM, MUHTARI_MEMORY_RIOT_RAM,
                                      MUHTARI_MEMORY_CART_RAM, MUHTARI_MEMORY_EXPANSION_RAM};
    static uint8_t guarded[4096];
    static uint8_t guarded_before[sizeof guarded];
    int16_t audio_a[AUDIO_CAPACITY];
    int16_t audio_b[AUDIO_CAPACITY];
    uint8_t arbitrary[64];
    uint8_t *battery_before;
    size_t old_state_bytes;
    uint8_t *old_state;
    uint64_t initial_ticks, initial_clocks, initial_generation;

    size_t ym_bytes = images_ym2151(ym_image);
    muhtari_media ym_media = media_for(ym_image, ym_bytes, MUHTARI_EXPANSION_XM);
    ym_media.port[1] = MUHTARI_PORT_SAVEKEY;
    muhtari *a = make_machine();
    muhtari *b = make_machine();
    CHECK_EQ_U64(muhtari_load(a, &ym_media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari_load(b, &ym_media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(a->synth.ym_installed, 1u);
    CHECK_EQ_U64(a->cart.irq_mask, 4u);
    CHECK_EQ_U64(a->ym.output[0], 0u);
    CHECK_EQ_U64(a->ym.output[1], 0u);
    CHECK_EQ_U64(a->synth.raw[10], 0u);
    CHECK_EQ_U64(a->synth.raw[11], 0u);
    CHECK_EQ_U64(a->battery.bytes, 34816u);
    CHECK_EQ_U64(muhtari_state_size(a), 0u);

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
    initial_ticks = a->bus.ticks;
    initial_clocks = a->ym.clocks;
    initial_generation = a->battery.generation;

    memset(guarded, 0xA7, sizeof guarded);
    memcpy(guarded_before, guarded, sizeof guarded);
    CHECK_EQ_U64(muhtari_save_state(a, guarded, sizeof guarded),
                 MUHTARI_STATUS_ERR_STATE_UNAVAILABLE);
    memset(arbitrary, 0x39, sizeof arbitrary);
    CHECK_EQ_U64(muhtari_load_state(a, arbitrary, sizeof arbitrary),
                 MUHTARI_STATUS_ERR_STATE_UNAVAILABLE);
    CHECK_EQ_U64(a->bus.ticks, initial_ticks);
    CHECK_EQ_U64(a->ym.clocks, initial_clocks);
    CHECK_EQ_U64(a->battery.generation, initial_generation);
    CHECK(memcmp(guarded, guarded_before, sizeof guarded) == 0);
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
    CHECK(battery_before == NULL ||
          memcmp(a->battery.data, battery_before, a->battery.bytes) == 0);

    /* A valid legacy-format state from a non-YM machine is also refused
       before its input is inspected or applied. */
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
                     MUHTARI_STATUS_ERR_STATE_UNAVAILABLE);
    }

    /* Public refusal leaves the live YM trace untouched: both independently
       loaded instances now execute and produce the same bounded PCM. */
    int16_t minimum = INT16_MAX;
    int16_t maximum = INT16_MIN;
    uint64_t nonzero = 0u;
    for (unsigned frame_index = 0u; frame_index < 3u; frame_index++) {
        uint32_t count_a = 0u, count_b = 0u;
        CHECK_EQ_U64(frame(a, audio_a, &count_a), MUHTARI_STATUS_OK);
        CHECK_EQ_U64(frame(b, audio_b, &count_b), MUHTARI_STATUS_OK);
        CHECK_EQ_U64(count_a, count_b);
        CHECK(memcmp(audio_a, audio_b, count_a * sizeof audio_a[0]) == 0);
        if (frame_index == 0u)
            CHECK_EQ_U64(count_a, 54u);
        else
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
    CHECK_EQ_U64(muhtari__ym2151_read(m, 0x0461u, just_before) & flag, 0u);
    CHECK_EQ_U64(muhtari__ym2151_irq(m), 0u);
    CHECK_EQ_U64(muhtari__ym2151_read(m, 0x0461u, exact) & flag, flag);
    CHECK_EQ_U64(muhtari__ym2151_irq(m), 1u);
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
        CHECK_EQ_U64(muhtari__ym2151_read(m, 0x0461u, 67u * 2u) & 0x80u, 0x80u);
        CHECK_EQ_U64(muhtari__ym2151_read(m, 0x0461u, 68u * 2u) & 0x80u, 0u);
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
                muhtari__ym2151_catch_up(machines[chip], now * 2u);
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
    else
        return 2;
    CHECK_DONE();
}
