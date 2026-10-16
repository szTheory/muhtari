/* A PNG writer for 8-bit RGB images with no compression library: the image
   data goes into stored (uncompressed) deflate blocks. Owned code for the
   runner's frame dumps. */
#ifndef MUHTARI_RUNNER_PNG_H
#define MUHTARI_RUNNER_PNG_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/* Continues an Adler-32 (RFC 1950 section 8) over n bytes at p. Start with
   adler = 1. */
uint32_t muhtari__adler32(uint32_t adler, const uint8_t *p, size_t n);

/* One PNG being written. Fields are private to png.c. */
typedef struct muhtari__png {
    FILE *f;
    uint32_t width;
    uint32_t rows_left;
    uint32_t crc;        /* CRC-32 of the IDAT chunk so far */
    uint32_t adler;      /* Adler-32 of the raw image data so far */
    uint32_t block_left; /* raw bytes left in the current stored block */
    uint32_t raw_left;   /* raw bytes not yet written */
    int failed;          /* a write failed */
} muhtari__png;

/* Starts a width x height RGB image on f, which must be open for binary
   writing. Returns 0, or -1 when the size is 0 or too large for one IDAT
   chunk, or a write failed. */
int muhtari__png_begin(muhtari__png *png, FILE *f, uint32_t width, uint32_t height);

/* Writes the next row, top first: width pixels of R, G, B bytes. Returns 0,
   or -1 when a write failed or every row has been written. */
int muhtari__png_row(muhtari__png *png, const uint8_t *rgb);

/* Finishes the image after the last row. Returns 0, or -1 when a row is
   missing or any write failed. Does not close f. */
int muhtari__png_end(muhtari__png *png);

#endif /* MUHTARI_RUNNER_PNG_H */
