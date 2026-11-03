#ifndef MUHTARI_TEST_INPUT_PIXELS_H
#define MUHTARI_TEST_INPUT_PIXELS_H

#include <stdint.h>

unsigned input_response(const uint8_t *rgb, uint32_t w, uint32_t h, int held);
unsigned input_history_response(const uint8_t *rgb, uint32_t w, uint32_t h);

enum {
    HISTORY_PIXEL_COMPLETE = 0u,
    HISTORY_PIXEL_DIMENSIONS = 1u,
    HISTORY_PIXEL_WAITING = 2u,
    HISTORY_PIXEL_HELD = 3u,
    HISTORY_PIXEL_SIGNATURE = 4u
};

#endif
