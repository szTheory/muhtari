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

/* The seeds of the bodies tests/cart/test_cartdb.c lists, each named for the
   entry that keys it. No other builder uses a seed this large. */
enum {
    IMAGES_SEED_OVERRIDE = 0x7E570001,   /* 64 KiB: SuperGame with RAM, PAL */
    IMAGES_SEED_LIGHTGUN = 0x7E570002,   /* 16 KiB: linear, light gun */
    IMAGES_SEED_PROLINE = 0x7E570003,    /* 16 KiB: linear */
    IMAGES_SEED_TWO_CHIP = 0x7E570004,   /* 48 KiB: 16 KiB + 32 KiB chips */
    IMAGES_SEED_MIRRORED = 0x7E570005,   /* 32 KiB: 16 KiB + 8 KiB chip twice */
    IMAGES_SEED_ACTIVISION = 0x7E570006, /* 128 KiB, keyed in its other layout */
    IMAGES_SEED_SWAP_64K = 0x7E570007,   /* 64 KiB, keyed as if swapped */
    IMAGES_SEED_2600 = 0x7E570008,       /* 8 KiB: a 2600 prefix, footer passes */
    IMAGES_SEED_NOT_CART = 0x7E570009    /* 4 KiB: not a cartridge */
};

/* A headerless image of n bytes filled from seed, ending with the footer a
   7800 BIOS accepts. */
size_t images_seeded(uint8_t *buf, uint32_t n, uint32_t seed);

/* The 32 KiB mirrored body: images_seeded, then the 8 KiB at 16 KiB set to
   the last 8 KiB, as a board that maps one 8 KiB chip twice holds it. */
size_t images_mirrored(uint8_t *buf);

/* Swaps the 8 KiB halves of each 16 KiB bank of n bytes in place, turning
   one Activision dump layout into the other (cartridge notes, section 1.4). */
void images_swap_halves(uint8_t *p, size_t n);

/* OM half tags $30-$3F and a fixed-ROM program reporting bank-zero $A000
   and bank-five $C000 at CPU $2200/$2201. Returns 128 KiB. */
size_t images_activision(uint8_t *buf);

/* Equal CPU/MARIA sets with distinct first/last tags in every 4 KiB linear
   block or 16 KiB switched bank. ram is the v4 device code (0, 1 or 3). */
size_t images_bankset(uint8_t *buf, uint32_t half_kib, uint8_t ram);

#endif /* MUHTARI_TEST_IMAGES_H */
