/* API 5: load-time storage and stable battery/installed-ROM queries. */
#include <stdlib.h>
#include <string.h>
#include "muhtari.h"
#include "../check.h"

static void *api_alloc(void *user, size_t n)
{
    (void)user;
    return malloc(n);
}
static void api_free(void *user, void *p, size_t n)
{
    (void)user;
    (void)n;
    free(p);
}

int main(void)
{
    CHECK(MUHTARI_API_VERSION >= 5);
    CHECK_EQ_U64(MUHTARI_STATUS_ERR_INVALID_HSC, 14);
    CHECK_EQ_U64(MUHTARI_HSC_AUTO, 0);
    CHECK_EQ_U64(MUHTARI_HSC_ON, 1);
    CHECK_EQ_U64(MUHTARI_HSC_OFF, 2);
    CHECK_EQ_U64(MUHTARI_PORT_SAVEKEY, 257);
    CHECK_EQ_U64(MUHTARI_HSC_FIRMWARE_STUB, 1);
    CHECK_EQ_U64(MUHTARI_HSC_FIRMWARE_KNOWN, 2);
    CHECK_EQ_U64(MUHTARI_HSC_FIRMWARE_WARN_UNKNOWN, 4);
    CHECK_EQ_U64(MUHTARI_HSC_FIRMWARE_WARN_SIGNATURE, 8);
    muhtari_config cfg;
    memset(&cfg, 0, sizeof cfg);
    cfg.size = (uint32_t)sizeof cfg;
    cfg.allocator.alloc = api_alloc;
    cfg.allocator.free = api_free;
    muhtari *m = NULL;
    CHECK_EQ_U64(muhtari_create(&cfg, &m), MUHTARI_STATUS_OK);
    uint8_t image[128 + 4096];
    memset(image, 0, sizeof image);
    image[0] = 3u;
    memcpy(image + 1, "ATARI7800", 9);
    image[51] = 0x10u;
    image[55] = 1u;
    image[56] = 1u;
    memset(image + 128, 0xEA, 4096);
    image[128 + 4092] = 0u;
    image[128 + 4093] = 0xF0u;
    muhtari_media media;
    memset(&media, 0, sizeof media);
    media.size = (uint32_t)sizeof media;
    media.image = image;
    media.image_size = sizeof image;
    media.hsc_mode = MUHTARI_HSC_ON;
    media.hsc_rom = NULL;
    media.hsc_rom_size = 0u;
    media.port[0] = MUHTARI_PORT_SAVEKEY;
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    muhtari_battery_info battery;
    memset(&battery, 0, sizeof battery);
    battery.size = (uint32_t)sizeof battery;
    CHECK_EQ_U64(muhtari_get_battery(m, &battery), MUHTARI_STATUS_OK);
    CHECK(battery.data != NULL);
    CHECK_EQ_U64(battery.bytes, 34816);
    CHECK_EQ_U64(battery.layout_version, 1);
    CHECK_EQ_U64(battery.generation, 0);
    CHECK_EQ_U64(battery.hsc_attached, 1);
    CHECK_EQ_U64(battery.eeprom_port, 1);
    CHECK_EQ_U64(battery.eeprom_device, MUHTARI_PORT_SAVEKEY);
    CHECK_EQ_U64(battery.firmware_flags, MUHTARI_HSC_FIRMWARE_STUB);
    for (unsigned i = 0; i < sizeof battery.hsc_sha1; i++) {
        CHECK_EQ_U64(battery.hsc_sha1[i], 0);
    }
    muhtari_const_span rom;
    memset(&rom, 0, sizeof rom);
    rom.size = (uint32_t)sizeof rom;
    CHECK_EQ_U64(muhtari_get_hsc_rom(m, &rom), MUHTARI_STATUS_OK);
    CHECK(rom.data != NULL);
    CHECK_EQ_U64(rom.bytes, 4096);
    if (rom.data != NULL) {
        CHECK_EQ_U64(rom.data[0x900], 0xC6);
    }
    muhtari_destroy(m);
    CHECK_DONE();
}
