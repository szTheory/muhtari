/* Public XM/YM frame path and fail-closed state gate. */
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"
#include "../check.h"
#include "../cart/images.h"

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

int main(int argc, char **argv)
{
    if (argc != 2 || strcmp(argv[1], "tracer") != 0)
        return 2;
    ym_tracer();
    CHECK_DONE();
}
