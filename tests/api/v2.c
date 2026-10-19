/* API version 2: what it adds to version 1, muhtari_probe and its types.
   FROZEN once released, as v1.c is: it calls the new function, assigns every
   field of the new struct and checks every new constant, so a rename, a
   removal or a changed value fails to build or fails here. */
#include <string.h>

#include "muhtari.h"
#include "../check.h"

/* A clean 4 KiB linear image: an A78 version 3 header over a zero body. */
static uint8_t image[128 + 4096];

int main(void)
{
    CHECK_EQ_U64(MUHTARI_API_VERSION, 2);
    CHECK_EQ_U64(MUHTARI_IMAGE_SIZE_MAX, 1048704);

    CHECK_EQ_U64(MUHTARI_STATUS_ERR_INVALID_IMAGE, 5);
    CHECK_EQ_U64(MUHTARI_STATUS_ERR_TRUNCATED_IMAGE, 6);
    CHECK_EQ_U64(MUHTARI_STATUS_ERR_UNSUPPORTED_SIZE, 7);
    CHECK_EQ_U64(MUHTARI_STATUS_ERR_UNSUPPORTED_2600, 8);
    CHECK_EQ_U64(MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD, 9);

    CHECK_EQ_U64(MUHTARI_KIND_7800, 1);
    CHECK_EQ_U64(MUHTARI_KIND_2600, 2);
    CHECK_EQ_U64(MUHTARI_SOURCE_HEADER, 1);
    CHECK_EQ_U64(MUHTARI_SOURCE_HASH, 2);
    CHECK_EQ_U64(MUHTARI_SOURCE_FOOTER, 3);
    CHECK_EQ_U64(MUHTARI_SOURCE_SIZE, 4);
    CHECK_EQ_U64(MUHTARI_BOARD_LINEAR, 1);
    CHECK_EQ_U64(MUHTARI_BOARD_SUPERGAME, 2);
    CHECK_EQ_U64(MUHTARI_BOARD_ACTIVISION, 3);
    CHECK_EQ_U64(MUHTARI_BOARD_ABSOLUTE, 4);
    CHECK_EQ_U64(MUHTARI_BOARD_FRACTALUS, 5);
    CHECK_EQ_U64(MUHTARI_BOARD_BANKSET, 6);
    CHECK_EQ_U64(MUHTARI_BOARD_SOUPER, 7);
    CHECK_EQ_U64(MUHTARI_BOARD_ROM_AT_4000, 1);
    CHECK_EQ_U64(MUHTARI_BOARD_BANK6_AT_4000, 2);
    CHECK_EQ_U64(MUHTARI_BOARD_BANKED_RAM, 4);
    CHECK_EQ_U64(MUHTARI_BOARD_HALT_BANKED_RAM, 8);
    CHECK_EQ_U64(MUHTARI_SOUND_POKEY_4000, 1);
    CHECK_EQ_U64(MUHTARI_SOUND_POKEY_450, 2);
    CHECK_EQ_U64(MUHTARI_SOUND_POKEY_440, 4);
    CHECK_EQ_U64(MUHTARI_SOUND_POKEY_800, 8);
    CHECK_EQ_U64(MUHTARI_SOUND_YM2151, 16);
    CHECK_EQ_U64(MUHTARI_SOUND_COVOX, 32);
    CHECK_EQ_U64(MUHTARI_DEVICE_HSC, 1);
    CHECK_EQ_U64(MUHTARI_DEVICE_SAVEKEY, 2);
    CHECK_EQ_U64(MUHTARI_DEVICE_XM, 4);
    CHECK_EQ_U64(MUHTARI_PORT_NONE, 0);
    CHECK_EQ_U64(MUHTARI_PORT_PROLINE, 1);
    CHECK_EQ_U64(MUHTARI_PORT_LIGHTGUN, 2);
    CHECK_EQ_U64(MUHTARI_PORT_PADDLE, 3);
    CHECK_EQ_U64(MUHTARI_PORT_TRAKBALL, 4);
    CHECK_EQ_U64(MUHTARI_PORT_JOYSTICK, 5);
    CHECK_EQ_U64(MUHTARI_PORT_DRIVING, 6);
    CHECK_EQ_U64(MUHTARI_PORT_KEYPAD, 7);
    CHECK_EQ_U64(MUHTARI_PORT_ST_MOUSE, 8);
    CHECK_EQ_U64(MUHTARI_PORT_AMIGA_MOUSE, 9);
    CHECK_EQ_U64(MUHTARI_PORT_ATARIVOX, 10);
    CHECK_EQ_U64(MUHTARI_PORT_SNES2ATARI, 11);
    CHECK_EQ_U64(MUHTARI_PORT_MEGA7800, 12);
    CHECK_EQ_U64(MUHTARI_REGION_NTSC, 1);
    CHECK_EQ_U64(MUHTARI_REGION_PAL, 2);
    CHECK_EQ_U64(MUHTARI_CART_HEADER, 1);
    CHECK_EQ_U64(MUHTARI_CART_HASH_MATCH, 2);
    CHECK_EQ_U64(MUHTARI_CART_WARN_HEADER_V4_IGNORED, 1);
    CHECK_EQ_U64(MUHTARI_CART_WARN_HEADER_CORRECTED, 2);
    CHECK_EQ_U64(MUHTARI_CART_WARN_VERSION_ABOVE_4, 4);
    CHECK_EQ_U64(MUHTARI_CART_WARN_SIZE_FIELD_MISMATCH, 8);
    CHECK_EQ_U64(MUHTARI_CART_WARN_TITLE_NONPRINTABLE, 16);
    CHECK_EQ_U64(MUHTARI_CART_WARN_UNKNOWN_PORT, 32);
    CHECK_EQ_U64(MUHTARI_CART_WARN_LEGACY_SAVE_BIT, 64);
    CHECK_EQ_U64(MUHTARI_CART_WARN_XM_VALUE, 128);
    CHECK_EQ_U64(MUHTARI_CART_WARN_RESERVED_NONZERO, 256);
    CHECK_EQ_U64(MUHTARI_CART_WARN_CART_TYPE_CONFLICT, 512);
    CHECK_EQ_U64(sizeof(muhtari_cart_info), 140);

    image[0] = 3u;
    memcpy(image + 1, "ATARI7800", 9);
    memcpy(image + 17, "API", 3);
    image[51] = 0x10u; /* size field 4096 */
    image[55] = 1u;
    image[56] = 1u;

    muhtari_cart_info info;
    memset(&info, 0, sizeof info);
    info.size = (uint32_t)sizeof info;
    info.kind = 0u;
    info.source = 0u;
    info.flags = 0u;
    info.warnings = 0u;
    info.rom_size = 0u;
    info.board = 0u;
    info.board_options = 0u;
    info.ram_size = 0u;
    info.sound = 0u;
    info.devices = 0u;
    info.port1 = 0u;
    info.port2 = 0u;
    info.region = 0u;
    info.a78_version = 0u;
    info.a78_rom_size = 0u;
    info.a78_cart_type = 0u;
    info.a78_port1 = 0u;
    info.a78_port2 = 0u;
    info.a78_tv = 0u;
    info.a78_save = 0u;
    info.a78_xm = 0u;
    info.a78_mapper = 0u;
    info.a78_mapper_options = 0u;
    info.a78_audio = 0u;
    info.a78_irq = 0u;
    info.title[0] = 0u;
    CHECK_EQ_U64(muhtari_probe(image, sizeof image, &info), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(info.size, sizeof info);
    CHECK_EQ_U64(info.kind, MUHTARI_KIND_7800);
    CHECK_EQ_U64(info.source, MUHTARI_SOURCE_HEADER);
    CHECK_EQ_U64(info.flags, MUHTARI_CART_HEADER);
    CHECK_EQ_U64(info.warnings, 0);
    CHECK_EQ_U64(info.rom_size, 4096);
    CHECK_EQ_U64(info.board, MUHTARI_BOARD_LINEAR);
    CHECK_EQ_U64(info.port1, MUHTARI_PORT_PROLINE);
    CHECK_EQ_U64(info.region, MUHTARI_REGION_NTSC);
    CHECK_EQ_U64(info.a78_version, 3);
    CHECK_EQ_U64(info.a78_rom_size, 4096);
    CHECK(memcmp(info.title, "API", 4) == 0);
    CHECK_EQ_U64(muhtari_probe(image, sizeof image, NULL), MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK_DONE();
}
