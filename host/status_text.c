/* Status messages and value names (status_text.h). Each table is indexed by
   the value or bit number the public header gives it. */
#include "status_text.h"

#include <stddef.h>

#define COUNT(a) ((uint32_t)(sizeof(a) / sizeof((a)[0])))

const char *muhtari__status_text(muhtari_status status)
{
    switch (status) {
    case MUHTARI_STATUS_OK:
        return "ok";
    case MUHTARI_STATUS_ERR_ARGUMENT:
        return "invalid argument";
    case MUHTARI_STATUS_ERR_STRUCT_SIZE:
        return "structure size not supported";
    case MUHTARI_STATUS_ERR_NO_MEMORY:
        return "out of memory";
    case MUHTARI_STATUS_ERR_BUFFER_TOO_SMALL:
        return "buffer too small";
    case MUHTARI_STATUS_ERR_INVALID_IMAGE:
        return "not a 7800 or 2600 cartridge image";
    case MUHTARI_STATUS_ERR_TRUNCATED_IMAGE:
        return "the image ends before the size its header gives";
    case MUHTARI_STATUS_ERR_UNSUPPORTED_SIZE:
        return "larger than 1 MiB plus a 128-byte header";
    case MUHTARI_STATUS_ERR_UNSUPPORTED_2600:
        return "unsupported: 2600 (planned)";
    case MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD:
        return "unsupported board";
    case MUHTARI_STATUS_ERR_BIOS_REJECTED_CART:
        return "the BIOS rejected the cartridge";
    case MUHTARI_STATUS_ERR_INVALID_BIOS:
        return "not a 4 KiB or 16 KiB BIOS image";
    case MUHTARI_STATUS_ERR_UNSUPPORTED_REGION:
        return "unsupported region: PAL (planned)";
    case MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE:
        return "unsupported controller";
    }
    return "unknown status";
}

/* Looks value up in a table whose first entry is value 0. */
static const char *lookup(const char *const *names, uint32_t count, uint32_t value)
{
    return value < count && names[value] != NULL ? names[value] : "unknown";
}

const char *muhtari__board_name(uint32_t board)
{
    static const char *const names[] = {
        NULL, "linear", "supergame", "activision", "absolute", "fractalus", "bankset", "souper",
    };
    return lookup(names, COUNT(names), board);
}

const char *muhtari__kind_name(uint32_t kind)
{
    static const char *const names[] = {NULL, "7800", "2600"};
    return lookup(names, COUNT(names), kind);
}

const char *muhtari__source_name(uint32_t source)
{
    static const char *const names[] = {NULL, "header", "hash", "footer", "size"};
    return lookup(names, COUNT(names), source);
}

/* The A78 header's controller codes, bytes 55 and 56. */
const char *muhtari__port_name(uint32_t port)
{
    static const char *const names[] = {
        "none",   "proline",  "lightgun",    "paddle",   "trakball",   "joystick", "driving",
        "keypad", "st_mouse", "amiga_mouse", "atarivox", "snes2atari", "mega7800",
    };
    return lookup(names, COUNT(names), port);
}

const char *muhtari__region_name(uint32_t region)
{
    static const char *const names[] = {NULL, "ntsc", "pal"};
    return lookup(names, COUNT(names), region);
}

const char *muhtari__bit_name(muhtari__bit_set set, uint32_t bit)
{
    static const char *const board_options[] = {
        "rom_at_4000",
        "bank6_at_4000",
        "banked_ram",
        "halt_banked_ram",
    };
    static const char *const sound[] = {
        "pokey_4000", "pokey_450", "pokey_440", "pokey_800", "ym2151", "covox",
    };
    static const char *const devices[] = {"hsc", "savekey", "xm"};
    static const char *const flags[] = {"header", "hash_match", "activision_half_swapped"};
    static const char *const warnings[] = {
        "header_v4_ignored",  "header_corrected",   "version_above_4", "size_field_mismatch",
        "title_nonprintable", "unknown_port",       "legacy_save_bit", "xm_value",
        "reserved_nonzero",   "cart_type_conflict", "activision_layout_assumed",
    };
    switch (set) {
    case MUHTARI__BITS_BOARD_OPTIONS:
        return lookup(board_options, COUNT(board_options), bit);
    case MUHTARI__BITS_SOUND:
        return lookup(sound, COUNT(sound), bit);
    case MUHTARI__BITS_DEVICES:
        return lookup(devices, COUNT(devices), bit);
    case MUHTARI__BITS_FLAGS:
        return lookup(flags, COUNT(flags), bit);
    case MUHTARI__BITS_WARNINGS:
        return lookup(warnings, COUNT(warnings), bit);
    }
    return "unknown";
}
