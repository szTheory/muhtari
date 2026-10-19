/* Cartridge images built in memory (images.h). */
#include "images.h"

#include <string.h>

void images_fill(uint8_t *p, size_t n, uint32_t seed)
{
    uint32_t x = seed * 2654435761u + 0x9E3779B9u;
    for (size_t i = 0; i < n; i++) {
        x = x * 1664525u + 1013904223u;
        p[i] = (uint8_t)(x >> 24);
    }
}

void images_footer(uint8_t *body, size_t n)
{
    size_t mapped = n < 48u * IMAGES_KIB ? n : 48u * IMAGES_KIB;
    uint32_t start = 0x10000u - (uint32_t)mapped;
    uint8_t *f = body + n - 8u; /* $FFF8-$FFFF */
    f[0] = 0xFFu;
    f[1] = (uint8_t)(((start >> 8) & 0xF0u) | 0x07u);
    f[4] = (uint8_t)(start & 0xFFu);
    f[5] = (uint8_t)(start >> 8);
}

void images_header(uint8_t *h, uint8_t version, const char *title, uint32_t rom_size,
                   uint16_t cart_type)
{
    memset(h, 0, IMAGES_HEADER);
    h[0] = version;
    memcpy(h + 1, "ATARI7800", 9);
    size_t len = strlen(title);
    memcpy(h + 17, title, len < 32u ? len : 32u);
    h[49] = (uint8_t)(rom_size >> 24);
    h[50] = (uint8_t)(rom_size >> 16);
    h[51] = (uint8_t)(rom_size >> 8);
    h[52] = (uint8_t)rom_size;
    h[53] = (uint8_t)(cart_type >> 8);
    h[54] = (uint8_t)cart_type;
    h[55] = 1u;
    h[56] = 1u;
    memcpy(h + 100, "ACTUAL CART DATA STARTS HERE", 28);
}

size_t images_linear(uint8_t *buf, uint8_t version, uint32_t body)
{
    images_header(buf, version, "Clean Linear", body, 0u);
    images_fill(buf + IMAGES_HEADER, body, body);
    images_footer(buf + IMAGES_HEADER, body);
    return IMAGES_HEADER + body;
}

size_t images_a78(uint8_t *buf, uint8_t version, uint32_t rom_size, uint16_t cart_type,
                  uint32_t body)
{
    images_header(buf, version, "Image", rom_size, cart_type);
    images_fill(buf + IMAGES_HEADER, body, rom_size ^ cart_type ^ body);
    return IMAGES_HEADER + body;
}

size_t images_headerless(uint8_t *buf, uint32_t n)
{
    images_fill(buf, n, n + 1u);
    images_footer(buf, n);
    return n;
}

size_t images_2600(uint8_t *buf, uint32_t n)
{
    images_fill(buf, n, n + 2u);
    uint8_t *f = buf + n - 8u; /* $FFF8-$FFFF, as a 2600 cartridge maps them */
    f[0] = 0u;
    f[1] = 0u;
    f[4] = 0x00u;
    f[5] = 0xF0u;
    f[6] = 0x00u;
    f[7] = 0xF0u;
    return n;
}
