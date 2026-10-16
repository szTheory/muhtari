/* The host palette. The expected colours are literals typed from the upstream
   warm NTSC rows (THIRD-PARTY-NOTICES.md names the file and commit), not values
   read back from the table, so a changed table entry fails here. */
#include <stdint.h>

#include "palette.h"
#include "../check.h"

static uint32_t red(uint32_t rgb)
{
    return (rgb >> 16) & 0xFFu;
}

static uint32_t green(uint32_t rgb)
{
    return (rgb >> 8) & 0xFFu;
}

static uint32_t blue(uint32_t rgb)
{
    return rgb & 0xFFu;
}

int main(void)
{
    /* The test card draws $46 as its red band and $C6 as its green band, so
       red must be the largest channel of $46 and green the largest of $C6. */
    uint32_t c46 = muhtari__palette_rgb(0x46);
    uint32_t cc6 = muhtari__palette_rgb(0xC6);
    CHECK(red(c46) > green(c46) && red(c46) > blue(c46));
    CHECK(green(cc6) > red(cc6) && green(cc6) > blue(cc6));

    /* Spot values: first and last grey, and the two band colours. */
    CHECK_EQ_HEX(muhtari__palette_rgb(0x00), 0x000000u);
    CHECK_EQ_HEX(muhtari__palette_rgb(0x0F), 0xEFEFEFu);
    CHECK_EQ_HEX(c46, 0xAF5163u);
    CHECK_EQ_HEX(cc6, 0x469A46u);

    /* RGB565 truncates each channel: r>>3, g>>2, b>>3.
       $0F: 0xEF>>3 = 29, 0xEF>>2 = 59, 0xEF>>3 = 29.
       $46: 0xAF>>3 = 21, 0x51>>2 = 20, 0x63>>3 = 12.
       $C6: 0x46>>3 = 8,  0x9A>>2 = 38, 0x46>>3 = 8. */
    CHECK_EQ_HEX(muhtari__palette_rgb565(0x0F), (29u << 11) | (59u << 5) | 29u);
    CHECK_EQ_HEX(muhtari__palette_rgb565(0x46), (21u << 11) | (20u << 5) | 12u);
    CHECK_EQ_HEX(muhtari__palette_rgb565(0xC6), (8u << 11) | (38u << 5) | 8u);

    CHECK_DONE();
}
