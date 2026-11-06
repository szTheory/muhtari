#ifndef MUHTARI_TEST_INPUT_HISTORY_PIXELS_H
#define MUHTARI_TEST_INPUT_HISTORY_PIXELS_H

#include <stdint.h>

enum {
    INPUT_HISTORY_WAITING,
    INPUT_HISTORY_HELD,
    INPUT_HISTORY_COMPLETE,
    INPUT_HISTORY_STATES
};

#define INPUT_HISTORY_WAITING_COLOR 0x00u
#define INPUT_HISTORY_HELD_COLOR 0x0Eu
#define INPUT_HISTORY_COMPLETE_COLOR 0x1Eu

unsigned input_history_response(const uint8_t *pixels, uint32_t width, uint32_t height,
                                unsigned state);

#endif
