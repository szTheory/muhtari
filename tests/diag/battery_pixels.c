/* Independent RGB battery oracle and its faults; no host launch or fixture generation. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int diag_battery_pixels(const uint8_t *rgb, uint32_t w, uint32_t h, unsigned count, unsigned marker,
                        int controls);

static unsigned battery_hex(const uint8_t *p)
{
    return (unsigned)p[0] << 16 | (unsigned)p[1] << 8 | (unsigned)p[2];
}

/* Independent 3x5 hex glyphs: double-width, white, eight-line rows, values at x=40. */
static const uint8_t glyph[16][5] = {
    {7, 5, 5, 5, 7}, {2, 6, 2, 2, 7}, {7, 1, 7, 4, 7}, {7, 1, 7, 1, 7},
    {5, 5, 7, 1, 1}, {7, 4, 7, 1, 7}, {7, 4, 7, 5, 7}, {7, 1, 1, 1, 1},
    {7, 5, 7, 5, 7}, {7, 5, 7, 1, 7}, {7, 5, 7, 5, 5}, {6, 5, 6, 5, 7},
    {7, 4, 4, 4, 7}, {6, 5, 5, 5, 6}, {7, 4, 7, 4, 7}, {7, 4, 7, 4, 4}};

static int battery(const uint8_t *rgb, uint32_t w, uint32_t h, unsigned count, unsigned marker)
{
    if (w != 320u || h != 242u)
        return 1;
    for (unsigned field = 0; field < 3; field++) {
        unsigned value = field == 2 ? marker : count;
        unsigned row = field == 2 ? 5u : field + 2u;
        for (unsigned d = 0; d < 4; d++) {
            unsigned digit = (value >> (12u - 4u * d)) & 15u;
            for (unsigned y = 0; y < 8; y++) {
                for (unsigned x = 0; x < 8; x++) {
                    unsigned on = y >= 1u && y <= 5u && x < 6u &&
                                  (glyph[digit][y - 1u] & (4u >> (x / 2u))) != 0u;
                    unsigned want = on ? 0xEFEFEFu : 0u;
                    const uint8_t *p =
                        rgb + ((size_t)(10u + row * 8u + y) * w + 40u + d * 8u + x) * 3u;
                    if (battery_hex(p) != want) {
                        printf("battery pixel field %u digit %u at %u,%u: %06X expected %06X\n",
                               field, d, 40u + d * 8u + x, 10u + row * 8u + y, battery_hex(p),
                               want);
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

int diag_battery_pixels(const uint8_t *rgb, uint32_t w, uint32_t h, unsigned count, unsigned marker,
                        int controls)
{
    int rc = battery(rgb, w, h, count, marker);
    if (rc != 0 || !controls)
        return rc;
    size_t n = (size_t)w * h * 3u;
    uint8_t *bad = calloc(n, 1u);
    if (bad == NULL)
        return 2;
    rc = !battery(bad, w, h, count, marker) || !battery(rgb, w, h, count + 1u, marker);
    memcpy(bad, rgb, n);
    /* Keep HSC and marker intact but give EEPROM an earlier value. */
    for (unsigned d = 0; d < 4; d++)
        for (unsigned y = 0; y < 8; y++)
            for (unsigned x = 0; x < 8; x++) {
                unsigned digit = ((count - 1u) >> (12u - 4u * d)) & 15u;
                uint8_t v =
                    y >= 1u && y <= 5u && x < 6u && (glyph[digit][y - 1u] & (4u >> (x / 2u))) ? 239u
                                                                                              : 0u;
                memset(bad + ((size_t)(34u + y) * w + 40u + d * 8u + x) * 3u, v, 3u);
            }
    rc |= !battery(bad, w, h, count, marker);
    free(bad);
    return rc;
}
