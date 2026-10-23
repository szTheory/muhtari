/* Bounded execution cartridges: retain the original headerless 48 KiB
   linear builder, and honor explicit retail-board inputs with supported
   headers and fixed-window vectors. Every builder returns its actual
   length and requested board; output storage never exceeds 131200 bytes. */
#ifndef MUHTARI_FUZZ_RUN_CART_H
#define MUHTARI_FUZZ_RUN_CART_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "muhtari.h"

#define RUN_CART_BYTES 131200u
#define RUN_LINEAR_BYTES 49152u

struct run_cart_info {
    size_t bytes;
    uint32_t board;
};

/* Explicit A78 board bits select a retail builder. Other inputs keep the
   original headerless linear interpretation, including old regressions. */
static inline uint32_t run_cart_board(const uint8_t *data, size_t size)
{
    if (size >= 128u && memcmp(data + 1u, "ATARI7800", 9u) == 0) {
        if (data[53] == 1u)
            return MUHTARI_BOARD_ACTIVISION;
        if (data[53] == 2u)
            return MUHTARI_BOARD_ABSOLUTE;
        if (data[54] == 0x80u || (data[0] >= 4u && data[65] == 2u))
            return MUHTARI_BOARD_FRACTALUS;
    }
    return MUHTARI_BOARD_LINEAR;
}

static inline struct run_cart_info run_cart_build(const uint8_t *data, size_t size, uint8_t *cart,
                                                  uint32_t board)
{
    struct run_cart_info result = {RUN_LINEAR_BYTES, board};
    uint32_t body = board == MUHTARI_BOARD_ACTIVISION ? 131072u
                    : board == MUHTARI_BOARD_ABSOLUTE ? 65536u
                                                      : 32768u;
    if (board != MUHTARI_BOARD_LINEAR) {
        result.bytes = 128u + body;
        memset(cart, 0, result.bytes);
        cart[0] = 3u;
        memcpy(cart + 1u, "ATARI7800", 9u);
        cart[49] = (uint8_t)(body >> 24);
        cart[50] = (uint8_t)(body >> 16);
        cart[51] = (uint8_t)(body >> 8);
        cart[52] = (uint8_t)body;
        cart[53] = board == MUHTARI_BOARD_ACTIVISION ? 1u
                   : board == MUHTARI_BOARD_ABSOLUTE ? 2u
                                                     : 0u;
        cart[54] = board == MUHTARI_BOARD_FRACTALUS ? 0x80u : 0u;
        if (size > 128u) {
            size_t copied = size - 128u < body ? size - 128u : body;
            memcpy(cart + 128u, data + 128u, copied);
        }
        /* All three boards have a fixed $E000 window; random execution
           starts there, never in a switched bank. */
        uint8_t *f = cart + result.bytes - 8u;
        f[0] = 0xFFu;
        f[1] = 0xE7u;
        f[4] = 0u;
        f[5] = 0xE0u;
        return result;
    }
    size_t n = size < RUN_LINEAR_BYTES ? size : RUN_LINEAR_BYTES;
    if (n != 0u)
        memcpy(cart, data, n);
    memset(cart + n, 0, RUN_LINEAR_BYTES - n);
    /* These are CPU bytes, not an A78 header. A fuzzed magic prefix must
       not make probe reinterpret the linear payload as loader metadata. */
    if (memcmp(cart + 1u, "ATARI7800", 9u) == 0)
        cart[1] = 0u;
    uint8_t *f = cart + RUN_LINEAR_BYTES - 8u;
    f[0] = (uint8_t)(f[0] | 0xF0u);
    uint32_t start = (uint32_t)(f[1] >> 4);
    start = start < 4u ? start + 4u : start;
    f[1] = (uint8_t)((start << 4) | ((f[1] & 0x04u) != 0u ? 0x07u : 0x03u));
    uint32_t reset = (uint32_t)f[4] | ((uint32_t)f[5] << 8);
    if ((reset >> 8) < (start << 4))
        reset = (reset & 0xFFu) | (start << 12);
    if (reset == 0xFFFFu)
        reset = 0xFFFEu;
    f[4] = (uint8_t)reset;
    f[5] = (uint8_t)(reset >> 8);
    return result;
}

static inline struct run_cart_info run_cart(const uint8_t *data, size_t size, uint8_t *cart)
{
    return run_cart_build(data, size, cart, run_cart_board(data, size));
}

/* An audible, changing fixed-ROM control for failed-load continuation. */
static inline struct run_cart_info run_cart_control(uint8_t *cart, uint32_t board)
{
    struct run_cart_info info = run_cart_build(NULL, 0u, cart, board);
    static const uint8_t code[] = {0xA9, 0x07, 0x85, 0x01, 0xA9, 0x60, 0x85, 0x3C, 0xA9,
                                   0x04, 0x85, 0x15, 0xA9, 0x03, 0x85, 0x17, 0xA9, 0x0F,
                                   0x85, 0x19, 0xEE, 0x00, 0x22, 0x4C, 0x14, 0xE0};
    memcpy(cart + info.bytes - 8192u, code, sizeof code);
    return info;
}

#endif /* MUHTARI_FUZZ_RUN_CART_H */
