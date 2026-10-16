/* The built-in test card shown while no content is loaded. A pure function of
   the coordinates: no table, no font, no state. It holds no version or text,
   so its hash never changes with a release. docs/ explains what each element
   reveals. */
#include "internal.h"

uint8_t muhtari__testcard_pixel(uint32_t x, uint32_t y, uint32_t h)
{
    /* The chart's first row: (h - 192) / 2 centres 192 rows, which for NTSC
       (h = 242) is frame row 25, the top of the Software Guide's safe area
       (A7800-MARIA section 6). */
    uint32_t cy = (h - 192u) / 2u;

    /* A white ($0F) block in the top-left corner only, so a mirror or a flip
       of the image shows. */
    if (x < 8u && y < 8u) {
        return 0x0Fu;
    }
    /* A white one-pixel border, so cropping shows. */
    if (x == 0u || x == MUHTARI__WIDTH - 1u || y == 0u || y == h - 1u) {
        return 0x0Fu;
    }
    /* Grey ($08) lines on the first and last rows of the 224-row overscan crop,
       frame rows 8-231 (A7800-MARIA section 6). */
    if (y == 8u || y == h - 11u) {
        return 0x08u;
    }
    if (y >= cy && y < cy + 192u) {
        /* The colour chart: each 14x12 cell (square at a 6:7 pixel aspect) is
           one colour, hue the row and luma the column, so cell (r, c) is $rc
           and every value $00-$FF appears once. */
        if (x >= 48u && x < 272u) {
            return (uint8_t)((((y - cy) / 12u) << 4) | ((x - 48u) / 14u));
        }
        /* A 1x1 checker on the left, which shows filtering and non-integer
           scaling. */
        if (x < 48u) {
            return ((x ^ y) & 1u) != 0u ? 0x0Fu : 0x00u;
        }
        /* A 2x1 checker on the right: MARIA's 160-pixel modes draw each pixel
           twice (A7800-MARIA section 6). */
        return (((x >> 1) ^ y) & 1u) != 0u ? 0x0Fu : 0x00u;
    }
    /* A red ($46) band above the chart and a green ($C6) one below, so a
       red/blue swap looks different from a vertical flip. */
    if (y < cy) {
        return 0x46u;
    }
    return 0xC6u;
}
