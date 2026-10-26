/* API 7's load selector, effective attachment and physical memory contract.
   The old media/info layouts below are frozen independently of this header. */
#include <stdlib.h>
#include <string.h>
#include "muhtari.h"
#include "../check.h"
#include "../alloc.h"

struct media_v6 {
    uint32_t size, region;
    const void *image;
    size_t image_size;
    const void *bios;
    size_t bios_size;
    uint32_t hsc_mode;
    const void *hsc_rom;
    size_t hsc_rom_size;
    uint32_t port[2];
};
struct info_v6 {
    uint32_t size, width, height, fps_num, fps_den, par_num, par_den;
    uint32_t sample_rate_num, sample_rate_den, max_audio_samples;
};

static uint8_t image[128u + 1048576u];
static uint32_t allocations;
static alloc_counts counts;
static void *api_alloc(void *user, size_t n)
{
    (void)user;
    allocations++;
    return count_alloc(&counts, n);
}
static void api_free(void *user, void *p, size_t n)
{
    (void)user;
    count_free(&counts, p, n);
}
static muhtari_media media_for(uint32_t body, uint16_t type, uint8_t xm)
{
    memset(image, 0, 128u + body);
    image[0] = 3u;
    memcpy(image + 1u, "ATARI7800", 9u);
    for (unsigned i = 0u; i < 4u; i++)
        image[49u + i] = (uint8_t)(body >> (24u - i * 8u));
    image[53] = (uint8_t)(type >> 8);
    image[54] = (uint8_t)type;
    image[55] = image[56] = 1u;
    image[63] = xm;
    /* A bounded JMP loop at $F000 in the CPU half. */
    uint32_t half = (type & 0x2000u) != 0u ? body / 2u : body;
    image[128u + half - 4096u] = 0x4Cu;
    image[128u + half - 4094u] = 0xF0u;
    image[128u + half - 3u] = 0xF0u;
    muhtari_media media;
    memset(&media, 0, sizeof media);
    media.size = (uint32_t)sizeof media;
    media.image = image;
    media.image_size = 128u + body;
    return media;
}
static uint32_t expansion_of(muhtari *m)
{
    muhtari_info info;
    memset(&info, 0, sizeof info);
    info.size = (uint32_t)sizeof info;
    CHECK_EQ_U64(muhtari_get_info(m, &info), MUHTARI_STATUS_OK);
    return info.expansion;
}
static muhtari_span span_of(muhtari *m, uint32_t kind)
{
    muhtari_span span;
    memset(&span, 0, sizeof span);
    span.size = (uint32_t)sizeof span;
    CHECK_EQ_U64(muhtari_get_memory(m, kind, &span), MUHTARI_STATUS_OK);
    return span;
}
static void sizes(muhtari *m)
{
    CHECK_EQ_U64(offsetof(muhtari_media, expansion), sizeof(struct media_v6));
    CHECK_EQ_U64(offsetof(muhtari_info, expansion), sizeof(struct info_v6));
    CHECK_EQ_U64(sizeof(struct info_v6), 40u);
    CHECK_EQ_U64(offsetof(muhtari_media, port), offsetof(struct media_v6, port));
    CHECK_EQ_U64(offsetof(muhtari_media, hsc_mode), offsetof(struct media_v6, hsc_mode));
    CHECK_EQ_U64(offsetof(muhtari_media, hsc_rom), offsetof(struct media_v6, hsc_rom));
    CHECK_EQ_U64(offsetof(muhtari_media, hsc_rom_size), offsetof(struct media_v6, hsc_rom_size));
    uint32_t first = (uint32_t)(offsetof(muhtari_media, bios_size) + sizeof(size_t));
    CHECK_EQ_U64(first, offsetof(struct media_v6, bios_size) + sizeof(size_t));
    CHECK_EQ_U64(first, sizeof(size_t) == 8u ? 40u : 24u);
    uint32_t end = (uint32_t)(offsetof(muhtari_media, expansion) + sizeof(uint32_t));
    for (unsigned xm = 0u; xm < 2u; xm++) {
        muhtari_media media = media_for(32768u, 0u, (uint8_t)xm);
        CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
        uint32_t automatic = expansion_of(m);
        CHECK_EQ_U64(automatic, xm != 0u ? MUHTARI_EXPANSION_XM : MUHTARI_EXPANSION_OFF);
        for (uint32_t size = first; size < end; size++) {
            media.expansion = UINT32_MAX;
            media.size = size;
            muhtari_media before = media;
            CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
            CHECK_EQ_U64(expansion_of(m), automatic);
            CHECK(memcmp(&media, &before, sizeof media) == 0);
        }
        media.size = end;
        CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_ERR_ARGUMENT);
        for (uint32_t choice = MUHTARI_EXPANSION_AUTO; choice <= MUHTARI_EXPANSION_XM; choice++) {
            media.expansion = choice;
            for (uint32_t size = (uint32_t)offsetof(muhtari_media, expansion); size <= end;
                 size++) {
                media.size = size;
                CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
                CHECK_EQ_U64(expansion_of(m),
                             size < end || choice == MUHTARI_EXPANSION_AUTO ? automatic : choice);
            }
        }
        media.expansion = MUHTARI_EXPANSION_XBOARD;
        CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
        CHECK_EQ_U64(expansion_of(m), MUHTARI_EXPANSION_XBOARD);
        media.expansion = MUHTARI_EXPANSION_OFF;
        CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
        CHECK_EQ_U64(expansion_of(m), MUHTARI_EXPANSION_OFF);
        struct media_v6 old;
        memcpy(&old, &media, sizeof old);
        old.size = (uint32_t)sizeof old;
        CHECK_EQ_U64(muhtari_load(m, (const muhtari_media *)(const void *)&old), MUHTARI_STATUS_OK);
        CHECK_EQ_U64(expansion_of(m), automatic);
    }
    muhtari_media media = media_for(32768u, 0u, 0u);
    media.expansion = MUHTARI_EXPANSION_XBOARD;
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    union {
        max_align_t alignment;
        uint8_t bytes[sizeof(muhtari_info) + 16u];
    } output;
    for (uint32_t size = 39u; size <= sizeof output.bytes; size++) {
        memset(&output, 0xA5, sizeof output);
        memset(output.bytes, 0, size);
        memcpy(output.bytes, &size, sizeof size);
        CHECK_EQ_U64(muhtari_get_info(m, (muhtari_info *)(void *)output.bytes),
                     size < 40u ? MUHTARI_STATUS_ERR_STRUCT_SIZE : MUHTARI_STATUS_OK);
        for (size_t i = size; i < sizeof output.bytes; i++)
            CHECK_EQ_U64(output.bytes[i], 0xA5u);
        if (size >= 40u) {
            muhtari_info full;
            memset(&full, 0, sizeof full);
            full.size = (uint32_t)sizeof full;
            CHECK_EQ_U64(muhtari_get_info(m, &full), MUHTARI_STATUS_OK);
            full.size = size;
            CHECK(memcmp(output.bytes, &full, size < sizeof full ? size : sizeof full) == 0);
        }
    }
    struct {
        muhtari_media media;
        uint8_t tail[8];
    } newer;
    memset(&newer, 0, sizeof newer);
    newer.media = media;
    newer.media.size = (uint32_t)sizeof newer;
    CHECK_EQ_U64(muhtari_load(m, &newer.media), MUHTARI_STATUS_OK);
    newer.tail[7] = 1u;
    CHECK_EQ_U64(muhtari_load(m, &newer.media), MUHTARI_STATUS_ERR_STRUCT_SIZE);
    memset(&output, 0, sizeof output);
    uint32_t size = (uint32_t)sizeof output.bytes;
    memcpy(output.bytes, &size, sizeof size);
    output.bytes[sizeof(muhtari_info)] = 1u;
    CHECK_EQ_U64(muhtari_get_info(m, (muhtari_info *)(void *)output.bytes),
                 MUHTARI_STATUS_ERR_STRUCT_SIZE);
    struct info_v6 old_info;
    memset(&old_info, 0, sizeof old_info);
    old_info.size = (uint32_t)sizeof old_info;
    CHECK_EQ_U64(muhtari_get_info(m, (muhtari_info *)(void *)&old_info), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(old_info.width, 320u);
    CHECK_EQ_U64(old_info.max_audio_samples, 801u);
}

static void topology(muhtari *m)
{
    muhtari_media media = media_for(65536u, 0x6000u, 0u);
    media.expansion = MUHTARI_EXPANSION_XBOARD;
    media.hsc_mode = MUHTARI_HSC_ON;
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    muhtari_span expansion = span_of(m, MUHTARI_MEMORY_EXPANSION_RAM);
    muhtari_span cart = span_of(m, MUHTARI_MEMORY_CART_RAM);
    CHECK_EQ_U64(expansion.bytes, 131072u);
    CHECK_EQ_U64(cart.bytes, 32768u);
    CHECK(expansion.data != NULL && cart.data != NULL && expansion.data != cart.data);
    muhtari_battery_info battery;
    memset(&battery, 0, sizeof battery);
    battery.size = (uint32_t)sizeof battery;
    CHECK_EQ_U64(muhtari_get_battery(m, &battery), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(battery.layout_version, 1u);
    CHECK_EQ_U64(battery.bytes, 2048u);
    CHECK(battery.data != expansion.data && battery.data != cart.data);
    expansion.data[131071] = 0x5Au;
    uint32_t calls = allocations;
    media.expansion = 4u;
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK_EQ_U64(allocations, calls);
    CHECK(span_of(m, MUHTARI_MEMORY_EXPANSION_RAM).data == expansion.data);
    CHECK_EQ_U64(expansion.data[131071], 0x5Au);
    CHECK_EQ_U64(muhtari_power_cycle(m), MUHTARI_STATUS_OK);
    CHECK(span_of(m, MUHTARI_MEMORY_EXPANSION_RAM).data == expansion.data);
    CHECK_EQ_U64(expansion.data[131071], 0u);
    media.expansion = MUHTARI_EXPANSION_OFF;
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    expansion = span_of(m, MUHTARI_MEMORY_EXPANSION_RAM);
    CHECK(expansion.data == NULL);
    CHECK_EQ_U64(expansion.bytes, 0u);

    media = media_for(106496u, 0x2000u, 1u);
    calls = allocations;
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD);
    CHECK_EQ_U64(allocations, calls);
    media.hsc_mode = MUHTARI_HSC_OFF;
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(expansion_of(m), MUHTARI_EXPANSION_XM);
    media.hsc_mode = MUHTARI_HSC_AUTO;
    media.expansion = MUHTARI_EXPANSION_OFF;
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(expansion_of(m), MUHTARI_EXPANSION_OFF);

    static const uint16_t primary[] = {1u, 0x8000u, 0x41u};
    for (unsigned i = 0u; i < 3u; i++) {
        media = media_for(32768u, primary[i], 0u);
        media.expansion = MUHTARI_EXPANSION_XBOARD;
        calls = allocations;
        CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE);
        CHECK_EQ_U64(allocations, calls);
    }
    media = media_for(32768u, 0x440u, 0u);
    media.expansion = MUHTARI_EXPANSION_XBOARD;
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    media = media_for(32768u, 0u, 2u);
    muhtari_cart_info probe;
    memset(&probe, 0, sizeof probe);
    probe.size = (uint32_t)sizeof probe;
    CHECK_EQ_U64(muhtari_probe(image, media.image_size, &probe), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(probe.a78_xm, 2u);
    CHECK((probe.warnings & MUHTARI_CART_WARN_XM_VALUE) != 0u);
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(expansion_of(m), MUHTARI_EXPANSION_XM);
    image[0] = 4u;
    image[67] = 0x20u;
    CHECK_EQ_U64(muhtari_probe(image, media.image_size, &probe), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(probe.sound, MUHTARI_SOUND_ADPCM);
    media.expansion = MUHTARI_EXPANSION_OFF;
    calls = allocations;
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE);
    CHECK_EQ_U64(allocations, calls);
}

/* The real physical maximum, through a client that includes only the public
   header. The allocator independently remembers every requested free size. */
static void capacity(muhtari *m)
{
    muhtari_media media = media_for(1048576u, 0x6002u, 0u);
    media.expansion = MUHTARI_EXPANSION_XM;
    media.hsc_mode = MUHTARI_HSC_ON;
    media.port[1] = MUHTARI_PORT_SAVEKEY;
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(counts.alloc_size, 1212416u);
    muhtari_span cart = span_of(m, MUHTARI_MEMORY_CART_RAM);
    muhtari_span expansion = span_of(m, MUHTARI_MEMORY_EXPANSION_RAM);
    CHECK_EQ_U64(cart.bytes, 32768u);
    CHECK_EQ_U64(expansion.bytes, 131072u);
    CHECK(cart.data != NULL && expansion.data == cart.data + 32768u);
    cart.data[32767] = 0x56u;
    expansion.data[131071] = 0x87u;
    muhtari_battery_info battery = {0};
    battery.size = (uint32_t)sizeof battery;
    CHECK_EQ_U64(muhtari_get_battery(m, &battery), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(battery.bytes, 34816u);
    CHECK_EQ_U64(battery.layout_version, 1u);
    battery.data[34815] = 0x69u;
    uint32_t calls = allocations;
    counts.fail_from = counts.allocs + 1u;
    media.expansion = MUHTARI_EXPANSION_XBOARD;
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_ERR_NO_MEMORY);
    CHECK_EQ_U64(allocations, calls + 1u);
    CHECK_EQ_U64(counts.alloc_size, 1212416u);
    CHECK_EQ_U64(expansion_of(m), MUHTARI_EXPANSION_XM);
    CHECK(span_of(m, MUHTARI_MEMORY_CART_RAM).data == cart.data);
    CHECK(span_of(m, MUHTARI_MEMORY_EXPANSION_RAM).data == expansion.data);
    CHECK_EQ_U64(cart.data[32767], 0x56u);
    CHECK_EQ_U64(expansion.data[131071], 0x87u);
    muhtari_battery_info after = {0};
    after.size = (uint32_t)sizeof after;
    CHECK_EQ_U64(muhtari_get_battery(m, &after), MUHTARI_STATUS_OK);
    CHECK(memcmp(&battery, &after, sizeof after) == 0);
    CHECK_EQ_U64(after.data[34815], 0x69u);
    counts.fail_from = 0u;
    CHECK_EQ_U64(muhtari_power_cycle(m), MUHTARI_STATUS_OK);
    CHECK(span_of(m, MUHTARI_MEMORY_CART_RAM).data == cart.data);
    CHECK(span_of(m, MUHTARI_MEMORY_EXPANSION_RAM).data == expansion.data);
    CHECK_EQ_U64(cart.data[32767], 0u);
    CHECK_EQ_U64(expansion.data[131071], 0u);
    CHECK_EQ_U64(after.data[34815], 0x69u);
    media.expansion = MUHTARI_EXPANSION_OFF;
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(counts.free_size, 1212416u);
    CHECK_EQ_U64(counts.alloc_size, 1081344u);
    CHECK_EQ_U64(span_of(m, MUHTARI_MEMORY_EXPANSION_RAM).bytes, 0u);
    CHECK_EQ_U64(counts.size_errors, 0u);
}

int main(void)
{
    CHECK_EQ_U64(MUHTARI_API_VERSION, 7u);
    CHECK_EQ_U64(MUHTARI_EXPANSION_AUTO, 0u);
    CHECK_EQ_U64(MUHTARI_EXPANSION_OFF, 1u);
    CHECK_EQ_U64(MUHTARI_EXPANSION_XBOARD, 2u);
    CHECK_EQ_U64(MUHTARI_EXPANSION_XM, 3u);
    CHECK_EQ_U64(MUHTARI_MEMORY_EXPANSION_RAM, 4u);
    CHECK_EQ_U64(MUHTARI_SOUND_ADPCM, 64u);
    muhtari_config cfg;
    memset(&cfg, 0, sizeof cfg);
    cfg.size = (uint32_t)sizeof cfg;
    cfg.allocator.alloc = api_alloc;
    cfg.allocator.free = api_free;
    counts.check_sizes = 1u;
    muhtari *m = NULL;
    CHECK_EQ_U64(muhtari_create(&cfg, &m), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(expansion_of(m), MUHTARI_EXPANSION_OFF);
    sizes(m);
    topology(m);
    capacity(m);
    muhtari_destroy(m);
    CHECK_EQ_U64(counts.live_bytes, 0u);
    CHECK_EQ_U64(counts.size_errors, 0u);
    CHECK_DONE();
}
