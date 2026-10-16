/* The runner's PNG writer. The checksums are tested against published check
   values. A PNG of the test card is then written to the path in argv[1] and
   parsed back here without the writer's code: chunk CRCs with a bitwise
   CRC-32, stored blocks reassembled by hand, Adler-32 by its definition, and
   every pixel compared with the palette colour of the card's native byte. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "muhtari.h"
#include "palette.h"
#include "../../runner/crc32.h"
#include "../../runner/png.h"
#include "../check.h"

#define WIDTH 320u
#define HEIGHT 242u
#define ROW_RAW (1u + 3u * WIDTH)
#define RAW_SIZE (HEIGHT * ROW_RAW)
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

/* CRC-32 one bit at a time (PNG specification, Annex D), sharing nothing
   with the writer's table. */
static uint32_t bitwise_crc(const uint8_t *p, size_t n)
{
    uint32_t c = 0xffffffffu;
    for (size_t i = 0; i < n; i++) {
        c ^= p[i];
        for (int k = 0; k < 8; k++) {
            c = (c & 1u) != 0u ? 0xedb88320u ^ (c >> 1) : c >> 1;
        }
    }
    return ~c;
}

static uint32_t be32(const uint8_t *p)
{
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | p[3];
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

/* Parses the PNG in d and checks it against the card. */
static void check_png(const uint8_t *d, size_t n, const uint8_t *video, uint8_t *raw)
{
    static const uint8_t signature[8] = {0x89, 'P', 'N', 'G', '\r', '\n', 0x1a, '\n'};
    CHECK_EQ_U64(n, FILE_SIZE);
    if (n != FILE_SIZE) {
        return;
    }
    CHECK(memcmp(d, signature, 8) == 0);

    /* Exactly IHDR, IDAT, IEND, each with a valid CRC. */
    static const char *const want[3] = {"IHDR", "IDAT", "IEND"};
    const uint8_t *chunk_data[3] = {NULL, NULL, NULL};
    uint32_t chunk_len[3] = {0, 0, 0};
    size_t at = 8;
    for (int c = 0; c < 3; c++) {
        CHECK(at + 12u <= n);
        if (at + 12u > n) {
            return;
        }
        uint32_t len = be32(d + at);
        CHECK(len <= n - at - 12u);
        if (len > n - at - 12u) {
            return;
        }
        CHECK(memcmp(d + at + 4, want[c], 4) == 0);
        CHECK_EQ_HEX(be32(d + at + 8 + len), bitwise_crc(d + at + 4, 4u + (size_t)len));
        chunk_data[c] = d + at + 8;
        chunk_len[c] = len;
        at += 12u + len;
    }
    CHECK_EQ_U64(at, n);

    /* IHDR: 320 x 242, depth 8, colour type 2, compression, filter and
       interlace 0. */
    static const uint8_t ihdr[13] = {0, 0, 1, 0x40, 0, 0, 0, 242, 8, 2, 0, 0, 0};
    CHECK_EQ_U64(chunk_len[0], 13u);
    CHECK(memcmp(chunk_data[0], ihdr, 13) == 0);
    CHECK_EQ_U64(chunk_len[2], 0u);

    /* IDAT: zlib header 78 01, stored blocks, Adler-32. */
    const uint8_t *z = chunk_data[1];
    uint32_t zn = chunk_len[1];
    CHECK(zn >= 6u && z[0] == 0x78 && z[1] == 0x01);
    uint32_t pos = 2, got = 0, blocks = 0;
    int final = 0;
    while (!final && pos + 5u <= zn) {
        CHECK((z[pos] & 0xfeu) == 0u); /* BTYPE 00, no other bits */
        final = z[pos] & 1;
        uint32_t len = z[pos + 1] | ((uint32_t)z[pos + 2] << 8);
        uint32_t nlen = z[pos + 3] | ((uint32_t)z[pos + 4] << 8);
        CHECK_EQ_HEX(nlen, ~len & 0xffffu);
        pos += 5u;
        if (pos + len > zn || got + len > RAW_SIZE) {
            CHECK(0);
            return;
        }
        memcpy(raw + got, z + pos, len);
        pos += len;
        got += len;
        blocks++;
    }
    CHECK(final);
    CHECK_EQ_U64(got, RAW_SIZE);
    CHECK_EQ_U64(blocks, 4u);
    CHECK_EQ_U64(pos + 4u, zn);
    uint32_t s1 = 1, s2 = 0;
    for (uint32_t i = 0; i < got; i++) {
        s1 = (s1 + raw[i]) % 65521u;
        s2 = (s2 + s1) % 65521u;
    }
    CHECK_EQ_HEX(be32(z + zn - 4u), (s2 << 16) | s1);

    /* Each row: filter 0, then R, G, B of palette(card byte). */
    uint32_t bad = 0;
    for (uint32_t y = 0; y < HEIGHT; y++) {
        const uint8_t *r = raw + y * ROW_RAW;
        bad += (uint32_t)(r[0] != 0u);
        for (uint32_t x = 0; x < WIDTH; x++) {
            uint32_t rgb = muhtari__palette_rgb(video[y * WIDTH + x]);
            uint32_t png_rgb = ((uint32_t)r[1u + 3u * x] << 16) |
                               ((uint32_t)r[2u + 3u * x] << 8) | r[3u + 3u * x];
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
    CHECK_EQ_HEX(bitwise_crc((const uint8_t *)"123456789", 9), 0xCBF43926u);
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
