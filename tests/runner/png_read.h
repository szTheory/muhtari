/* A reader for the PNG the runner writes, shared by the tests that look at
   dumped frames. It knows the PNG and zlib formats, not the writer's code. */
#ifndef MUHTARI_TEST_PNG_READ_H
#define MUHTARI_TEST_PNG_READ_H

#include <stddef.h>
#include <stdint.h>

#define PNG_READ_WIDTH 320u
#define PNG_READ_HEIGHT 242u
#define PNG_READ_ROW (1u + 3u * PNG_READ_WIDTH)
#define PNG_READ_RAW_SIZE (PNG_READ_HEIGHT * PNG_READ_ROW)

/* CRC-32 one bit at a time (PNG specification, Annex D). */
uint32_t png_read_crc(const uint8_t *p, size_t n);

/* Parses the n bytes at d: the signature, then exactly IHDR (320 x 242,
   depth 8, colour type 2, no interlace), IDAT and IEND, each with a valid
   CRC; IDAT a zlib stream of stored blocks (header 78 01) with a valid
   Adler-32. Copies the PNG_READ_RAW_SIZE filtered bytes into raw: per row a
   filter byte, then R, G, B per pixel. Returns the number of stored blocks,
   or -1 after naming the fault on standard error. */
int png_read(const uint8_t *d, size_t n, uint8_t *raw);

/* Reads the file at path and parses it as png_read does. Returns 0, or -1
   after naming the fault on standard error. A row whose filter byte is not
   0 is a fault. */
int png_read_file(const char *path, uint8_t *raw);

#endif /* MUHTARI_TEST_PNG_READ_H */
