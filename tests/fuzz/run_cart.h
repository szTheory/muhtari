/* The run target's cartridge: any bytes as a headerless 48 KiB linear
   cartridge. The input is truncated, or padded with $00, to 49,152 bytes,
   and its last 8 bytes, $FFF8-$FFFF, are then changed as little as needed
   to pass the BIOS's footer test, which src/cart.c applies to a headerless
   image (A7800-CARTRIDGES-AND-PERIPHERALS section 1.5): without it every
   input would be refused as a 2600 image and no frame would run code. */
#ifndef MUHTARI_FUZZ_RUN_CART_H
#define MUHTARI_FUZZ_RUN_CART_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define RUN_CART_BYTES 49152u

static inline void run_cart(const uint8_t *data, size_t size, uint8_t *cart)
{
    size_t n = size < RUN_CART_BYTES ? size : RUN_CART_BYTES;
    if (n != 0u) {
        memcpy(cart, data, n);
    }
    memset(cart + n, 0, RUN_CART_BYTES - n);
    uint8_t *f = cart + RUN_CART_BYTES - 8u;
    /* $FFF8's high nibble is $F; $FFF9's low nibble is 3 or 7 and its high
       nibble, the start page, at least 4. */
    f[0] = (uint8_t)(f[0] | 0xF0u);
    uint32_t start = (uint32_t)(f[1] >> 4);
    start = start < 4u ? start + 4u : start;
    f[1] = (uint8_t)((start << 4) | ((f[1] & 0x04u) != 0u ? 0x07u : 0x03u));
    /* The reset vector's page is at or above the start page, and the vector
       is not $FFFF. */
    uint32_t reset = (uint32_t)f[4] | ((uint32_t)f[5] << 8);
    if ((reset >> 8) < (start << 4)) {
        reset = (reset & 0xFFu) | (start << 12);
    }
    if (reset == 0xFFFFu) {
        reset = 0xFFFEu;
    }
    f[4] = (uint8_t)(reset & 0xFFu);
    f[5] = (uint8_t)(reset >> 8);
}

#endif /* MUHTARI_FUZZ_RUN_CART_H */
