/* The runner's PNG writer. The checksums are tested against published check
   values. A PNG of the test card is then written to the path in argv[1] and
   parsed back without the writer's code, by the shared reader (png_read.c:
   chunk CRCs with a bitwise CRC-32, stored blocks reassembled by hand,
   Adler-32 by its definition), and every pixel is compared with the palette
   colour of the card's native byte. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "muhtari.h"
#include "palette.h"
#include "../../runner/crc32.h"
#include "../../runner/png.h"
#include "../check.h"
#include "png_read.h"

#define WIDTH 320u
#define HEIGHT 242u
#define ROW_RAW PNG_READ_ROW
#define RAW_SIZE PNG_READ_RAW_SIZE
#define FILE_SIZE 232645u

static void *test_alloc(void *user, size_t size)
{
    (void)user;
    return malloc(size);
}

static void test_free(void *user, void *ptr, size_t size)
{
    (void)user;
    (void)size;
    free(ptr);
}

/* Renders the card into video (WIDTH * HEIGHT bytes). Returns 0 on success. */
static int render_card(uint8_t *video)
{
    int16_t audio[801];
    muhtari_config cfg;
    memset(&cfg, 0, sizeof cfg);
    cfg.size = (uint32_t)sizeof cfg;
    cfg.allocator.alloc = test_alloc;
    cfg.allocator.free = test_free;
    muhtari *m = NULL;
    if (muhtari_create(&cfg, &m) != MUHTARI_STATUS_OK) {
        return -1;
    }
    muhtari_frame io;
    memset(&io, 0, sizeof io);
    io.size = (uint32_t)sizeof io;
    io.video = video;
    io.video_pitch = WIDTH;
    io.audio = audio;
    io.audio_capacity = 801;
    muhtari_status st = muhtari_run_frame(m, &io);
    muhtari_destroy(m);
    return st == MUHTARI_STATUS_OK ? 0 : -1;
}

/* Writes the card through the writer, converting with the palette. */
static int write_card(const char *path, const uint8_t *video)
{
    uint8_t row[3u * WIDTH];
    FILE *f = fopen(path, "wb");
    if (f == NULL) {
        return -1;
    }
    muhtari__png png;
    int failed = muhtari__png_begin(&png, f, WIDTH, HEIGHT) != 0;
    for (uint32_t y = 0; y < HEIGHT && !failed; y++) {
        for (uint32_t x = 0; x < WIDTH; x++) {
            uint32_t rgb = muhtari__palette_rgb(video[y * WIDTH + x]);
            row[3u * x] = (uint8_t)(rgb >> 16);
            row[3u * x + 1u] = (uint8_t)(rgb >> 8);
            row[3u * x + 2u] = (uint8_t)rgb;
        }
        failed = muhtari__png_row(&png, row) != 0;
    }
    /* A row past the last is refused. */
    if (!failed) {
        CHECK(muhtari__png_row(&png, row) == -1);
        failed = muhtari__png_end(&png) != 0;
    }
    return fclose(f) == 0 && !failed ? 0 : -1;
}

/* Reads the whole file into a heap buffer of size *n. */
static uint8_t *read_file(const char *path, size_t *n)
{
    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        return NULL;
    }
    uint8_t *buf = (uint8_t *)malloc(FILE_SIZE + 1u);
    *n = buf == NULL ? 0u : fread(buf, 1, FILE_SIZE + 1u, f);
    fclose(f);
    return buf;
}

/* Parses the PNG in d with the shared reader and checks it against the
   card: the writer's size and four stored blocks, then every pixel. */
static void check_png(const uint8_t *d, size_t n, const uint8_t *video, uint8_t *raw)
{
    CHECK_EQ_U64(n, FILE_SIZE);
    int blocks = png_read(d, n, raw);
    CHECK_EQ_U64(blocks, 4u);
    if (blocks < 0) {
        return;
    }

    /* Each row: filter 0, then R, G, B of palette(card byte). */
    uint32_t bad = 0;
    for (uint32_t y = 0; y < HEIGHT; y++) {
        const uint8_t *r = raw + y * ROW_RAW;
        bad += (uint32_t)(r[0] != 0u);
        for (uint32_t x = 0; x < WIDTH; x++) {
            uint32_t rgb = muhtari__palette_rgb(video[y * WIDTH + x]);
            uint32_t png_rgb =
                ((uint32_t)r[1u + 3u * x] << 16) | ((uint32_t)r[2u + 3u * x] << 8) | r[3u + 3u * x];
            bad += (uint32_t)(png_rgb != rgb);
        }
    }
    CHECK_EQ_U64(bad, 0u);
}

int main(int argc, char **argv)
{
    /* Published check values: CRC-32 of "123456789" (PNG specification,
       Annex D and the CRC catalogue), Adler-32 of "Wikipedia". */
    CHECK_EQ_HEX(muhtari__crc32(0, (const uint8_t *)"123456789", 9), 0xCBF43926u);
    CHECK_EQ_HEX(png_read_crc((const uint8_t *)"123456789", 9), 0xCBF43926u);
    CHECK_EQ_HEX(muhtari__adler32(1, (const uint8_t *)"Wikipedia", 9), 0x11E60398u);

    /* An empty image is refused. */
    muhtari__png empty;
    CHECK(muhtari__png_begin(&empty, NULL, 0, HEIGHT) == -1);

    CHECK(argc == 2);
    uint8_t *video = (uint8_t *)malloc((size_t)WIDTH * HEIGHT);
    uint8_t *raw = (uint8_t *)malloc(RAW_SIZE);
    if (argc == 2 && video != NULL && raw != NULL) {
        CHECK(render_card(video) == 0);
        CHECK(write_card(argv[1], video) == 0);
        size_t n = 0;
        uint8_t *d = read_file(argv[1], &n);
        CHECK(d != NULL);
        if (d != NULL) {
            check_png(d, n, video, raw);
        }
        free(d);
    } else {
        CHECK(0);
    }
    free(video);
    free(raw);
    CHECK_DONE();
}
