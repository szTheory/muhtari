/* Independent input-band oracle shared by cartridge tests and the host judge. */
#include <stddef.h>
#include <stdint.h>

#include "palette.h"

unsigned input_response(const uint8_t *rgb, uint32_t w, uint32_t h, int held);

static int input_color(const uint8_t *p, unsigned value, unsigned mask)
{
    uint32_t rgb = (uint32_t)p[0] << 16 | (uint32_t)p[1] << 8 | p[2];
    int found = 0;
    for (unsigned v = 0; v < 256u; v++) {
        if (rgb == muhtari__palette_rgb_region((uint8_t)v, MUHTARI_REGION_NTSC)) {
            if ((v & mask) != value)
                return 0;
            found = 1;
        }
    }
    return found;
}

/* docs/hardware/controllers.md: P1 Up clears SWCHA bit4; button1 raises
   INPT0 and clears one-button INPT4. P2 and button2 stay idle. The owned
   cartridge draws the eight register reads in successive 28-line bands.
   Only D7 is driven on INPT reads; RGB must unambiguously identify it. */
unsigned input_response(const uint8_t *rgb, uint32_t w, uint32_t h, int held)
{
    if (w != 320u || h != 242u)
        return 1u;
    const unsigned want[8] = {held ? 0xEFu : 0xFFu, 0x3Fu, held ? 0x80u : 0u, 0u, 0u, 0u,
                              held ? 0u : 0x80u,    0x80u};
    for (unsigned field = 0; field < 8u; field++) {
        const uint8_t *p = rgb + ((size_t)(24u + 28u * field) * w + 160u) * 3u;
        if (!input_color(p, want[field], field < 2u ? 0xFFu : 0x80u))
            return field + 2u;
    }
    return 0u;
}
