/* English text for statuses and the names of muhtari_cart_info's values,
   shared by the runner and the libretro adapter. A host helper: the core
   and its ABI hold no text. Every function returns a static string and
   never NULL. */
#ifndef MUHTARI_HOST_STATUS_TEXT_H
#define MUHTARI_HOST_STATUS_TEXT_H

#include <stdint.h>

#include "muhtari.h"

/* The message for a status, such as "not a 7800 or 2600 cartridge image".
   For MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD it is "unsupported board", which
   a host follows with ": " and muhtari__board_name. */
const char *muhtari__status_text(muhtari_status status);

/* The lowercase names of single values: a board ("supergame"), a kind
   ("7800"), a source ("header"), a port ("proline") and a region ("ntsc").
   A value outside its enum is "unknown". */
const char *muhtari__board_name(uint32_t board);
const char *muhtari__kind_name(uint32_t kind);
const char *muhtari__source_name(uint32_t source);
const char *muhtari__port_name(uint32_t port);
const char *muhtari__region_name(uint32_t region);

/* The bit sets of muhtari_cart_info. */
typedef enum muhtari__bit_set {
    MUHTARI__BITS_BOARD_OPTIONS,
    MUHTARI__BITS_SOUND,
    MUHTARI__BITS_DEVICES,
    MUHTARI__BITS_FLAGS,
    MUHTARI__BITS_WARNINGS
} muhtari__bit_set;

/* The lowercase name of bit number bit (0 to 31) of a set, such as
   "pokey_450" for bit 1 of MUHTARI__BITS_SOUND. A bit the set does not
   define is "unknown". */
const char *muhtari__bit_name(muhtari__bit_set set, uint32_t bit);

#endif /* MUHTARI_HOST_STATUS_TEXT_H */
