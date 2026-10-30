/* Held-out faults use executed cartridge RGB, never the oracle's glyph builder. */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int bmp_read(const char *, uint32_t *, uint32_t *, uint8_t **);
int diag_battery_pixels(const uint8_t *, uint32_t, uint32_t, unsigned, unsigned, int);

int main(int argc, char **argv)
{
    uint32_t w, h;
    uint8_t *rgb = NULL;
    if (argc != 2 || bmp_read(argv[1], &w, &h, &rgb) != 0)
        return 2;
    int failed = diag_battery_pixels(rgb, w, h, 1, 255, 0) != 0;
    for (int delta = -1; delta <= 1; delta += 2)
        failed |= diag_battery_pixels(rgb, (uint32_t)((int)w + delta), h, 1, 255, 0) == 0 ||
                  diag_battery_pixels(rgb, w, (uint32_t)((int)h + delta), 1, 255, 0) == 0;
    failed |= diag_battery_pixels(rgb, w, h, 0, 255, 0) == 0 ||
              diag_battery_pixels(rgb, w, h, 2, 255, 0) == 0 ||
              diag_battery_pixels(rgb, w, h, 1, 254, 0) == 0;
    const unsigned row[] = {26, 34, 50};
    for (unsigned field = 0; field < 3; field++) {
        size_t offset = ((size_t)row[field] * w + 40u) * 3u;
        rgb[offset] ^= 255;
        failed |= diag_battery_pixels(rgb, w, h, 1, 255, 0) == 0;
        rgb[offset] ^= 255;
    }
    memset(rgb, 0, (size_t)w * h * 3u);
    failed |= diag_battery_pixels(rgb, w, h, 1, 255, 0) == 0;
    free(rgb);
    return failed;
}
