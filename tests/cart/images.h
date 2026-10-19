/* Cartridge images built in memory, so no image bytes are tracked. Every
   builder writes into a caller's buffer and returns the image's size. */
#ifndef MUHTARI_TEST_IMAGES_H
#define MUHTARI_TEST_IMAGES_H

#include <stddef.h>
#include <stdint.h>

#define IMAGES_HEADER 128u
#define IMAGES_KIB 1024u

/* Fills n bytes with a pattern from seed that no list holds. */
void images_fill(uint8_t *p, size_t n, uint32_t seed);

/* Ends a body of n bytes, mapped to end at $FFFF, with the footer a 7800
   BIOS accepts: start page at the body's first page, reset at its start. */
void images_footer(uint8_t *body, size_t n);

/* Writes a clean A78 header: version, magic padded with zeros, title padded
   with zeros, size field, cart type, ProLine on both ports, NTSC, no save, no
   XM, and a zero v4 block. */
void images_header(uint8_t *h, uint8_t version, const char *title, uint32_t rom_size,
                   uint16_t cart_type);

/* A clean linear image: a header of the given version over a body of body
   bytes with a valid footer. buf holds IMAGES_HEADER + body bytes. */
size_t images_linear(uint8_t *buf, uint8_t version, uint32_t body);

/* A headered image over a filled body: header fields as images_header, the
   size field rom_size, body bytes after the header. */
size_t images_a78(uint8_t *buf, uint8_t version, uint32_t rom_size, uint16_t cart_type,
                  uint32_t body);

/* A headerless image of n bytes whose footer a 7800 BIOS accepts. */
size_t images_headerless(uint8_t *buf, uint32_t n);

/* A 2600 image of n bytes: reset at $F000 and a footer the 7800 test fails. */
size_t images_2600(uint8_t *buf, uint32_t n);

#endif /* MUHTARI_TEST_IMAGES_H */
