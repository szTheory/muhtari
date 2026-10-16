/* Native colour byte to RGB, shared by the runner's image and the libretro
   adapter so both show the same frame. A host helper: the core never sees
   RGB, and the palette is outside the determinism hash. */
#ifndef MUHTARI_HOST_PALETTE_H
#define MUHTARI_HOST_PALETTE_H

#include <stdint.h>

/* The warm NTSC colour of a native byte $hl (hue high nibble, luma low
   nibble) as 0x00RRGGBB, the XRGB8888 pixel. */
uint32_t muhtari__palette_rgb(uint8_t index);

/* The same colour as RGB565, each channel truncated: r>>3, g>>2, b>>3. */
uint16_t muhtari__palette_rgb565(uint8_t index);

#endif /* MUHTARI_HOST_PALETTE_H */
