#include <stddef.h>

#include "history_pixels.h"
#include "palette.h"

static const uint8_t state_color[INPUT_HISTORY_STATES] = {
    INPUT_HISTORY_WAITING_COLOR, INPUT_HISTORY_HELD_COLOR, INPUT_HISTORY_COMPLETE_COLOR};

unsigned input_history_response(const uint8_t *pixels, uint32_t width, uint32_t height,
                                unsigned state)
{
    static const uint16_t sample[][2] = {{40u, 60u},  {160u, 60u},  {280u, 60u},
                                         {40u, 121u}, {160u, 121u}, {280u, 121u},
                                         {40u, 200u}, {160u, 200u}, {280u, 200u}};

    if (pixels == NULL || width != 320u || height != 242u || state >= INPUT_HISTORY_STATES)
        return 1u;
    for (unsigned i = 0; i < sizeof sample / sizeof sample[0]; i++) {
        size_t at = ((size_t)sample[i][1] * width + sample[i][0]) * 3u;
        uint32_t want = muhtari__palette_rgb_region(state_color[state], MUHTARI_REGION_NTSC);
        if (pixels[at] != (uint8_t)(want >> 16) || pixels[at + 1u] != (uint8_t)(want >> 8) ||
            pixels[at + 2u] != (uint8_t)want)
            return 2u;
    }
    return 0u;
}
