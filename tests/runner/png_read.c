/* The PNG reader the frame tests share (png_read.h). Every check follows the
   PNG specification and RFC 1950/1951, never the runner's writer. */
#include "png_read.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* The runner's file: 8 + 25 + 12 + 2 + 4 * 5 + PNG_READ_RAW_SIZE + 4 + 12
   bytes. A larger file is refused before it is parsed. */
#define FILE_LIMIT 262144u

uint32_t png_read_crc(const uint8_t *p, size_t n)
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

static int fault(const char *why)
{
    fprintf(stderr, "png: %s\n", why);
    return -1;
}

int png_read_height(const uint8_t *d, size_t n, uint8_t *raw, uint32_t height)
{
    if (height != 242u && height != 292u)
        return fault("unsupported native height");
    uint32_t raw_size = height * PNG_READ_ROW;
    static const uint8_t signature[8] = {0x89, 'P', 'N', 'G', '\r', '\n', 0x1a, '\n'};
    if (n < 8u || memcmp(d, signature, 8) != 0) {
        return fault("no PNG signature");
    }

    /* Exactly IHDR, IDAT, IEND, each with a valid CRC. */
    static const char *const want[3] = {"IHDR", "IDAT", "IEND"};
    const uint8_t *chunk_data[3] = {NULL, NULL, NULL};
    uint32_t chunk_len[3] = {0, 0, 0};
    size_t at = 8;
    for (int c = 0; c < 3; c++) {
        if (n - at < 12u) {
            return fault("truncated chunk");
        }
        uint32_t len = be32(d + at);
        if (len > n - at - 12u) {
            return fault("chunk longer than the file");
        }
        if (memcmp(d + at + 4, want[c], 4) != 0) {
            return fault("chunks are not IHDR, IDAT, IEND");
        }
        if (be32(d + at + 8 + len) != png_read_crc(d + at + 4, 4u + (size_t)len)) {
            return fault("chunk CRC mismatch");
        }
        chunk_data[c] = d + at + 8;
        chunk_len[c] = len;
        at += 12u + len;
    }
    if (at != n) {
        return fault("bytes after IEND");
    }

    /* IHDR: 320 x selected height, depth 8, colour type 2, compression, filter and
       interlace 0. */
    const uint8_t ihdr[13] = {0, 0, 1, 0x40, 0, 0, (uint8_t)(height >> 8), (uint8_t)height,
                              8, 2, 0, 0,    0};
    if (chunk_len[0] != 13u || memcmp(chunk_data[0], ihdr, 13) != 0 || chunk_len[2] != 0u) {
        return fault("not a supported 320-pixel RGB image at the requested height");
    }

    /* IDAT: zlib header 78 01, stored blocks, Adler-32. */
    const uint8_t *z = chunk_data[1];
    uint32_t zn = chunk_len[1];
    if (zn < 6u || z[0] != 0x78 || z[1] != 0x01) {
        return fault("not a zlib stream with header 78 01");
    }
    uint32_t pos = 2, got = 0;
    int blocks = 0, final = 0;
    while (!final && pos + 5u <= zn) {
        if ((z[pos] & 0xfeu) != 0u) {
            return fault("a block that is not stored");
        }
        final = z[pos] & 1;
        uint32_t len = z[pos + 1] | ((uint32_t)z[pos + 2] << 8);
        uint32_t nlen = z[pos + 3] | ((uint32_t)z[pos + 4] << 8);
        pos += 5u;
        if (nlen != (~len & 0xffffu) || len > zn - pos || len > raw_size - got) {
            return fault("a stored block's length is wrong");
        }
        memcpy(raw + got, z + pos, len);
        pos += len;
        got += len;
        blocks++;
    }
    if (!final || got != raw_size || pos + 4u != zn) {
        return fault("the image data is not one whole image");
    }
    uint32_t s1 = 1, s2 = 0;
    for (uint32_t i = 0; i < got; i++) {
        s1 = (s1 + raw[i]) % 65521u;
        s2 = (s2 + s1) % 65521u;
    }
    if (be32(z + zn - 4u) != ((s2 << 16) | s1)) {
        return fault("Adler-32 mismatch");
    }
    return blocks;
}

int png_read(const uint8_t *d, size_t n, uint8_t *raw)
{
    return png_read_height(d, n, raw, PNG_READ_HEIGHT);
}

int png_read_file(const char *path, uint8_t *raw)
{
    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        fprintf(stderr, "png: cannot open %s\n", path);
        return -1;
    }
    uint8_t *buf = (uint8_t *)malloc(FILE_LIMIT + 1u);
    size_t n = buf == NULL ? 0u : fread(buf, 1, FILE_LIMIT + 1u, f);
    fclose(f);
    int rc = -1;
    if (buf == NULL || n > FILE_LIMIT) {
        fprintf(stderr, "png: %s is too large or memory ran out\n", path);
    } else if (png_read(buf, n, raw) >= 0) {
        rc = 0;
        for (uint32_t y = 0; y < PNG_READ_HEIGHT && rc == 0; y++) {
            if (raw[y * PNG_READ_ROW] != 0u) {
                rc = fault("a row filter other than 0");
            }
        }
    } else {
        fprintf(stderr, "png: in %s\n", path);
    }
    free(buf);
    return rc;
}
