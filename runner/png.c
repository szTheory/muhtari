/* PNG writer, from the PNG specification (ISO/IEC 15948, W3C PNG Third
   Edition) and RFC 1950/1951. The file is the signature, IHDR, one IDAT and
   IEND, with no colour, gamma or aspect chunks. IDAT holds a zlib stream
   whose deflate data is stored blocks of at most 65,535 bytes (RFC 1951
   section 3.2.4), so no compressor is needed. Every multi-byte field is
   written byte by byte, so the file does not depend on the host's byte
   order. */
#include "png.h"

#include "crc32.h"

/* RFC 1951 section 3.2.4: LEN is 16 bits. */
#define STORED_MAX 65535u
/* RFC 1950 section 8: the largest prime below 65536. */
#define ADLER_BASE 65521u
/* PNG section 5.3: a chunk's length is at most 2^31 - 1. */
#define CHUNK_MAX 0x7fffffffu

uint32_t muhtari__adler32(uint32_t adler, const uint8_t *p, size_t n)
{
    uint32_t s1 = adler & 0xffffu;
    uint32_t s2 = adler >> 16;
    for (size_t i = 0; i < n; i++) {
        s1 = (s1 + p[i]) % ADLER_BASE;
        s2 = (s2 + s1) % ADLER_BASE;
    }
    return (s2 << 16) | s1;
}

static void put32(uint8_t *b, uint32_t v)
{
    b[0] = (uint8_t)(v >> 24);
    b[1] = (uint8_t)(v >> 16);
    b[2] = (uint8_t)(v >> 8);
    b[3] = (uint8_t)v;
}

/* Writes n bytes to the file only. */
static void out(muhtari__png *png, const uint8_t *p, size_t n)
{
    if (n > 0u && !png->failed && fwrite(p, 1, n, png->f) != n) {
        png->failed = 1;
    }
}

/* Writes n bytes of IDAT data: to the file and into the chunk CRC. */
static void idat(muhtari__png *png, const uint8_t *p, size_t n)
{
    png->crc = muhtari__crc32(png->crc, p, n);
    out(png, p, n);
}

/* Writes a whole chunk whose data is in memory (IHDR, IEND). */
static void chunk(muhtari__png *png, const char type[4], const uint8_t *data, uint32_t n)
{
    uint8_t b[8];
    put32(b, n);
    for (int i = 0; i < 4; i++) {
        b[4 + i] = (uint8_t)type[i];
    }
    out(png, b, 8);
    out(png, data, n);
    uint32_t crc = muhtari__crc32(0, b + 4, 4);
    crc = muhtari__crc32(crc, data, n);
    put32(b, crc);
    out(png, b, 4);
}

/* Writes n bytes of raw image data, opening a stored block whenever the
   current one is full. A block header is BFINAL (1 on the last block) with
   BTYPE 00, then LEN and its complement NLEN, little-endian. */
static void raw(muhtari__png *png, const uint8_t *p, uint32_t n)
{
    png->adler = muhtari__adler32(png->adler, p, n);
    while (n > 0u) {
        if (png->block_left == 0u) {
            uint32_t len = png->raw_left < STORED_MAX ? png->raw_left : STORED_MAX;
            uint32_t nlen = ~len & 0xffffu;
            uint8_t h[5] = {(uint8_t)(len == png->raw_left ? 1u : 0u), (uint8_t)len,
                            (uint8_t)(len >> 8), (uint8_t)nlen, (uint8_t)(nlen >> 8)};
            idat(png, h, 5);
            png->block_left = len;
        }
        uint32_t k = n < png->block_left ? n : png->block_left;
        idat(png, p, k);
        p += k;
        n -= k;
        png->block_left -= k;
        png->raw_left -= k;
    }
}

int muhtari__png_begin(muhtari__png *png, FILE *f, uint32_t width, uint32_t height)
{
    png->f = f;
    png->width = width;
    png->rows_left = height;
    png->crc = 0;
    png->adler = 1;
    png->block_left = 0;
    png->raw_left = 0;
    png->failed = 0;

    /* Raw data is one filter byte plus 3 bytes per pixel per row. IDAT holds
       the 2-byte zlib header, 5 bytes per stored block, the raw data and the
       4-byte Adler-32. */
    uint64_t raw_size = (uint64_t)height * (1u + 3u * (uint64_t)width);
    uint64_t blocks = (raw_size + STORED_MAX - 1u) / STORED_MAX;
    uint64_t idat_size = 2u + 5u * blocks + raw_size + 4u;
    if (width == 0u || height == 0u || width > CHUNK_MAX || idat_size > CHUNK_MAX) {
        return -1;
    }
    png->raw_left = (uint32_t)raw_size;

    static const uint8_t signature[8] = {0x89, 'P', 'N', 'G', '\r', '\n', 0x1a, '\n'};
    out(png, signature, 8);

    /* IHDR: width, height, bit depth 8, colour type 2 (RGB), compression 0,
       filter method 0, interlace 0 (PNG section 11.2.1). */
    uint8_t ihdr[13];
    put32(ihdr, width);
    put32(ihdr + 4, height);
    ihdr[8] = 8;
    ihdr[9] = 2;
    ihdr[10] = 0;
    ihdr[11] = 0;
    ihdr[12] = 0;
    chunk(png, "IHDR", ihdr, 13);

    /* IDAT length and type, then the zlib header: CM 8 with a 256-byte
       window (0x78) and FLEVEL 0 with FCHECK making 0x7801 a multiple of 31
       (RFC 1950 section 2.2). */
    uint8_t b[8];
    put32(b, (uint32_t)idat_size);
    b[4] = 'I';
    b[5] = 'D';
    b[6] = 'A';
    b[7] = 'T';
    out(png, b, 4);
    idat(png, b + 4, 4);
    static const uint8_t zlib_header[2] = {0x78, 0x01};
    idat(png, zlib_header, 2);
    return png->failed ? -1 : 0;
}

int muhtari__png_row(muhtari__png *png, const uint8_t *rgb)
{
    if (png->rows_left == 0u) {
        return -1;
    }
    /* Filter type 0 (None): the row's bytes are stored as they are. */
    static const uint8_t filter_none = 0;
    raw(png, &filter_none, 1);
    raw(png, rgb, 3u * png->width);
    png->rows_left--;
    return png->failed ? -1 : 0;
}

int muhtari__png_end(muhtari__png *png)
{
    if (png->rows_left != 0u) {
        return -1;
    }
    uint8_t b[4];
    put32(b, png->adler);
    idat(png, b, 4);
    put32(b, png->crc);
    out(png, b, 4);
    chunk(png, "IEND", NULL, 0);
    return png->failed ? -1 : 0;
}
