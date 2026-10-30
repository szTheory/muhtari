/* Pixel equality plus independent battery glyph observations; exits 0/1/2. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int bmp_read(const char *path, uint32_t *w, uint32_t *h, uint8_t **rgb);
int diag_battery_pixels(const uint8_t *rgb, uint32_t w, uint32_t h, unsigned count, unsigned marker,
                        int controls);
unsigned input_response(const uint8_t *rgb, uint32_t w, uint32_t h, int held);

static unsigned compare_hex(const uint8_t *p)
{
    return (unsigned)p[0] << 16 | (unsigned)p[1] << 8 | (unsigned)p[2];
}

int main(int argc, char **argv)
{
    int input_controls = argc == 5 && strcmp(argv[1], "--input-controls") == 0;
    int held = input_controls && strcmp(argv[2], "held") == 0;
    int released = input_controls && strcmp(argv[2], "released") == 0;
    if (input_controls && !held && !released)
        return 2;
    if (input_controls) {
        argc -= 2;
        argv += 2;
    }
    int controls = argc == 6 && strcmp(argv[1], "--battery-controls") == 0;
    int check = argc == 6 && (controls || strcmp(argv[1], "--battery") == 0);
    unsigned count = 0, marker = 0;
    if (check) {
        char *end;
        unsigned long n = strtoul(argv[2], &end, 10);
        if (*end != '\0' || n > 65535u)
            return 2;
        count = (unsigned)n;
        n = strtoul(argv[3], &end, 10);
        if (*end != '\0' || n > 255u)
            return 2;
        marker = (unsigned)n;
        argc -= 3;
        argv += 3;
    }
    if (argc != 3) {
        fprintf(stderr,
                "usage: compare [--battery-controls count marker | --input-controls held|released] "
                "EXPECTED.bmp ACTUAL.bmp\n");
        return 2;
    }
    uint32_t w[2], h[2];
    uint8_t *rgb[2] = {NULL, NULL};
    int rc = 0;
    for (int i = 0; i < 2 && rc == 0; i++) {
        if (bmp_read(argv[i + 1], &w[i], &h[i], &rgb[i]) != 0) {
            fprintf(stderr, "compare: cannot read %s as a BMP\n", argv[i + 1]);
            rc = 2;
        }
    }
    if (rc == 0 && (w[0] != w[1] || h[0] != h[1])) {
        printf("size differs: expected %ux%u, actual %ux%u\n", (unsigned)w[0], (unsigned)h[0],
               (unsigned)w[1], (unsigned)h[1]);
        rc = 1;
    }
    if (rc == 0 && check)
        rc = diag_battery_pixels(rgb[0], w[0], h[0], count, marker, controls) ||
             diag_battery_pixels(rgb[1], w[1], h[1], count, marker, 0);
    if (rc == 0 && input_controls) {
        for (unsigned i = 0; i < 2u && rc == 0; i++) {
            unsigned failed = input_response(rgb[i], w[i], h[i], held);
            if (failed != 0u) {
                static const char *const faults[] = {"invalid", "image dimensions",
                                                     "SWCHA",   "SWCHB",
                                                     "INPT0",   "INPT1",
                                                     "INPT2",   "INPT3",
                                                     "INPT4",   "INPT5"};
                printf("input-control: %s mismatch\n", faults[failed]);
                rc = 1;
            }
        }
    }
    for (uint32_t y = 0; rc == 0 && y < h[0]; y++) {
        for (uint32_t x = 0; rc == 0 && x < w[0]; x++) {
            const uint8_t *a = rgb[0] + ((size_t)y * w[0] + x) * 3u;
            const uint8_t *b = rgb[1] + ((size_t)y * w[0] + x) * 3u;
            if (a[0] != b[0] || a[1] != b[1] || a[2] != b[2]) {
                printf("first difference at %u,%u: expected %06X, actual %06X\n", (unsigned)x,
                       (unsigned)y, compare_hex(a), compare_hex(b));
                rc = 1;
            }
        }
    }
    if (rc == 0)
        printf("%ux%u, every pixel equal\n", (unsigned)w[0], (unsigned)h[0]);
    free(rgb[0]);
    free(rgb[1]);
    return rc;
}
