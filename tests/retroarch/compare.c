/* Pixel equality plus independent battery glyph observations; exits 0/1/2. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int bmp_read(const char *path, uint32_t *w, uint32_t *h, uint8_t **rgb);

static unsigned compare_hex(const uint8_t *p)
{
    return (unsigned)p[0] << 16 | (unsigned)p[1] << 8 | (unsigned)p[2];
}

/* Independent 3x5 hex glyphs: double-width, white, eight-line rows, values at x=40. */
static const uint8_t glyph[16][5] = {
    {7, 5, 5, 5, 7}, {2, 6, 2, 2, 7}, {7, 1, 7, 4, 7}, {7, 1, 7, 1, 7},
    {5, 5, 7, 1, 1}, {7, 4, 7, 1, 7}, {7, 4, 7, 5, 7}, {7, 1, 1, 1, 1},
    {7, 5, 7, 5, 7}, {7, 5, 7, 1, 7}, {7, 5, 7, 5, 5}, {6, 5, 6, 5, 6},
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
                    if (compare_hex(p) != want) {
                        printf("battery pixel field %u digit %u at %u,%u: %06X expected %06X\n",
                               field, d, 40u + d * 8u + x, 10u + row * 8u + y, compare_hex(p),
                               want);
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

enum progress_result {
    PROGRESS_OK,
    PROGRESS_BAD_IDENTITY,
    PROGRESS_BAD_COUNTER
};

static int progress_glyph(const uint8_t *rgb, uint32_t w, unsigned digit, unsigned value)
{
    for (unsigned y = 0; y < 8u; y++) {
        for (unsigned x = 0; x < 8u; x++) {
            unsigned on = y >= 1u && y <= 5u && x < 6u &&
                          (glyph[value][y - 1u] & (4u >> (x / 2u))) != 0u;
            unsigned want = on ? 0xEFEFEFu : 0u;
            const uint8_t *p = rgb + ((size_t)(26u + y) * w + 40u + digit * 8u + x) * 3u;
            if (compare_hex(p) != want)
                return -1;
        }
    }
    return (int)value;
}

/* The owned progress screen is 320x242 with four counter glyphs and a black
   border. Validate identity and decode progress in each image before comparing
   the complete pictures, so equal wrong captures cannot pass. */
static int progress_judge(const uint8_t *rgb, uint32_t w, uint32_t h, unsigned expected,
                          unsigned image)
{
    static const unsigned anchors[4][2] = {{0u, 0u}, {319u, 0u}, {0u, 241u}, {319u, 241u}};
    if (w != 320u || h != 242u) {
        printf("progress fixture identity image %u has size %ux%u, expected 320x242\n", image,
               (unsigned)w, (unsigned)h);
        return PROGRESS_BAD_IDENTITY;
    }
    for (unsigned i = 0; i < 4u; i++) {
        unsigned x = anchors[i][0], y = anchors[i][1];
        const uint8_t *p = rgb + ((size_t)y * w + x) * 3u;
        if (compare_hex(p) != 0u) {
            printf("progress fixture identity image %u differs at %u,%u: %06X, expected 000000\n",
                   image, x, y, compare_hex(p));
            return PROGRESS_BAD_IDENTITY;
        }
    }

    unsigned observed = 0u;
    for (unsigned digit = 0; digit < 4u; digit++) {
        int decoded = -1;
        for (unsigned candidate = 0; candidate < 16u; candidate++) {
            if (progress_glyph(rgb, w, digit, candidate) >= 0) {
                decoded = (int)candidate;
                break;
            }
        }
        if (decoded < 0) {
            printf("progress counter image %u digit %u is not a valid 3x5 hex glyph\n", image,
                   digit);
            return PROGRESS_BAD_COUNTER;
        }
        observed = (observed << 4) | (unsigned)decoded;
    }
    if (observed != expected) {
        printf("progress counter image %u is %04X, expected %04X\n", image, observed, expected);
        return PROGRESS_BAD_COUNTER;
    }
    return PROGRESS_OK;
}

static int pixels_equal(const uint8_t *a, const uint8_t *b, uint32_t w, uint32_t h,
                        int report_difference)
{
    for (uint32_t y = 0; y < h; y++) {
        for (uint32_t x = 0; x < w; x++) {
            const uint8_t *pa = a + ((size_t)y * w + x) * 3u;
            const uint8_t *pb = b + ((size_t)y * w + x) * 3u;
            if (pa[0] != pb[0] || pa[1] != pb[1] || pa[2] != pb[2]) {
                if (report_difference)
                    printf("first difference at %u,%u: expected %06X, actual %06X\n", x, y,
                           compare_hex(pa), compare_hex(pb));
                return 0;
            }
        }
    }
    return 1;
}

static int progress_controls(const uint8_t *rgb, uint32_t w, uint32_t h, unsigned expected)
{
    size_t n = (size_t)w * h * 3u;
    uint8_t *bad = calloc(n, 1u);
    if (bad == NULL)
        return 2;
    int failed = progress_judge(bad, w, h, expected, 2u) != PROGRESS_BAD_COUNTER;
    if (!failed)
        failed = progress_judge(rgb, w, h, (expected + 1u) & 0xFFFFu, 0u) !=
                 PROGRESS_BAD_COUNTER;
    memcpy(bad, rgb, n);
    bad[((size_t)100u * w + 100u) * 3u] ^= 0xFFu;
    if (!failed &&
        (progress_judge(bad, w, h, expected, 1u) != PROGRESS_OK ||
         pixels_equal(rgb, bad, w, h, 0)))
        failed = 1;
    free(bad);
    if (failed)
        return 1;
    printf("progress controls: equal wrong glyph, changed counter, and unrelated pixel rejected\n");
    return 0;
}

int main(int argc, char **argv)
{
    int battery_controls = argc == 6 && strcmp(argv[1], "--battery-controls") == 0;
    int battery_check = argc == 6 &&
                        (battery_controls || strcmp(argv[1], "--battery") == 0);
    int progress_controls_mode = argc == 5 && strcmp(argv[1], "--progress-controls") == 0;
    int progress_mode = argc == 5 &&
                        (progress_controls_mode || strcmp(argv[1], "--progress") == 0);
    unsigned count = 0, marker = 0, progress_count = 0;
    const char *expected_path = NULL, *actual_path = NULL;
    if (battery_check) {
        char *end;
        unsigned long n = strtoul(argv[2], &end, 10);
        if (*end != '\0' || n > 65535u)
            return 2;
        count = (unsigned)n;
        n = strtoul(argv[3], &end, 10);
        if (*end != '\0' || n > 255u)
            return 2;
        marker = (unsigned)n;
        expected_path = argv[4];
        actual_path = argv[5];
    } else if (progress_mode) {
        char *end;
        unsigned long n = strtoul(argv[2], &end, 16);
        if (argv[2][0] == '\0' || *end != '\0' || n > 65535u)
            return 2;
        progress_count = (unsigned)n;
        expected_path = argv[3];
        actual_path = argv[4];
    } else if (argc == 3) {
        expected_path = argv[1];
        actual_path = argv[2];
    }
    if (expected_path == NULL || actual_path == NULL) {
        fprintf(stderr, "usage: compare EXPECTED.bmp ACTUAL.bmp | --battery COUNT MARKER EXPECTED.bmp ACTUAL.bmp | --progress HEX EXPECTED.bmp ACTUAL.bmp\n");
        return 2;
    }
    uint32_t w[2], h[2];
    uint8_t *rgb[2] = {NULL, NULL};
    int rc = 0;
    for (int i = 0; i < 2 && rc == 0; i++) {
        const char *path = i == 0 ? expected_path : actual_path;
        if (bmp_read(path, &w[i], &h[i], &rgb[i]) != 0) {
            fprintf(stderr, "compare: cannot read %s as a BMP\n", path);
            rc = 2;
        }
    }
    if (rc == 0 && (w[0] != w[1] || h[0] != h[1])) {
        printf("size differs: expected %ux%u, actual %ux%u\n", (unsigned)w[0], (unsigned)h[0],
               (unsigned)w[1], (unsigned)h[1]);
        rc = 1;
    }
    if (rc == 0 && battery_check)
        rc = battery(rgb[0], w[0], h[0], count, marker) ||
             battery(rgb[1], w[1], h[1], count, marker);
    if (rc == 0 && progress_mode) {
        rc = progress_judge(rgb[0], w[0], h[0], progress_count, 0u) != PROGRESS_OK ||
             progress_judge(rgb[1], w[1], h[1], progress_count, 1u) != PROGRESS_OK;
    }
    if (rc == 0 && !pixels_equal(rgb[0], rgb[1], w[0], h[0], 1))
        rc = 1;
    if (rc == 0 && battery_controls) {
        size_t n = (size_t)w[0] * h[0] * 3u;
        uint8_t *bad = calloc(n, 1u);
        if (bad == NULL)
            rc = 2;
        else {
            rc = !battery(bad, w[0], h[0], count, marker) ||
                 !battery(rgb[0], w[0], h[0], count + 1u, marker);
            memcpy(bad, rgb[0], n);
            /* Keep HSC and marker intact but give EEPROM an earlier value. */
            for (unsigned d = 0; d < 4; d++)
                for (unsigned y = 0; y < 8; y++)
                    for (unsigned x = 0; x < 8; x++) {
                        unsigned digit = ((count - 1u) >> (12u - 4u * d)) & 15u;
                        uint8_t v = y >= 1u && y <= 5u && x < 6u &&
                                            (glyph[digit][y - 1u] & (4u >> (x / 2u)))
                                        ? 239u
                                        : 0u;
                        memset(bad + ((size_t)(34u + y) * w[0] + 40u + d * 8u + x) * 3u, v, 3u);
                    }
            rc |= !battery(bad, w[0], h[0], count, marker);
        }
        free(bad);
    }
    if (rc == 0 && progress_controls_mode)
        rc = progress_controls(rgb[0], w[0], h[0], progress_count);
    if (rc == 0)
        printf("%ux%u, every pixel equal\n", (unsigned)w[0], (unsigned)h[0]);
    free(rgb[0]);
    free(rgb[1]);
    return rc;
}
