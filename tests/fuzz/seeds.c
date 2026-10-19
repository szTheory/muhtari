/* The loader's seed inputs (seeds.h), each built by images.c. The names are
   stable: they become file names in the build directory. */
#include "seeds.h"

#include "../cart/images.h"

#define KIB IMAGES_KIB

static size_t linear_v3(uint8_t *b)
{
    return images_linear(b, 3u, 16u * KIB);
}

static size_t linear_v4(uint8_t *b)
{
    return images_linear(b, 4u, 48u * KIB);
}

static size_t supergame(uint8_t *b)
{
    return images_a78(b, 3u, 128u * KIB, 0x0002u, 128u * KIB);
}

static size_t sg9(uint8_t *b)
{
    return images_a78(b, 3u, 144u * KIB, 0x000Au, 144u * KIB);
}

static size_t cap(uint8_t *b)
{
    return images_a78(b, 3u, 1024u * KIB, 0x0002u, 1024u * KIB);
}

static size_t headerless(uint8_t *b)
{
    return images_headerless(b, 32u * KIB);
}

static size_t headerless_sg(uint8_t *b)
{
    return images_headerless(b, 128u * KIB);
}

static size_t a2600_4k(uint8_t *b)
{
    return images_2600(b, 4u * KIB);
}

static size_t a2600_8k(uint8_t *b)
{
    return images_2600(b, 8u * KIB);
}

/* Souper as the version 4 mapper, with cart-type bit 12. */
static size_t souper(uint8_t *b)
{
    size_t n = images_a78(b, 4u, 512u * KIB, 0x1000u, 512u * KIB);
    b[64] = 4u;
    return n;
}

static size_t activision(uint8_t *b)
{
    return images_a78(b, 3u, 128u * KIB, 0x0100u, 128u * KIB);
}

static size_t size_zero(uint8_t *b)
{
    return images_a78(b, 3u, 0u, 0u, 32u * KIB);
}

static size_t bad_magic(uint8_t *b)
{
    images_fill(b, 1000u, 1000u);
    return 1000u;
}

static size_t header_only(uint8_t *b)
{
    images_header(b, 3u, "Header only", 32u * KIB, 0u);
    return IMAGES_HEADER;
}

static size_t sg_short(uint8_t *b)
{
    return images_a78(b, 3u, 128u * KIB, 0x0002u, 40u * KIB);
}

static size_t oversized(uint8_t *b)
{
    size_t n = cap(b);
    b[n] = 0u;
    return n + 1u;
}

/* An invalid v4 block: mapper 5 does not exist. */
static size_t v4_ignored(uint8_t *b)
{
    size_t n = images_linear(b, 4u, 16u * KIB);
    b[64] = 5u;
    return n;
}

static size_t size_mismatch(uint8_t *b)
{
    size_t n = images_linear(b, 3u, 16u * KIB);
    b[52] = 0x80u; /* the size field reads 16 KiB + 128 */
    return n;
}

static size_t two_chip(uint8_t *b)
{
    return images_seeded(b, 48u * KIB, IMAGES_SEED_TWO_CHIP);
}

static size_t mirrored(uint8_t *b)
{
    return images_mirrored(b);
}

static size_t activision_swapped(uint8_t *b)
{
    size_t n = images_seeded(b, 128u * KIB, IMAGES_SEED_ACTIVISION);
    images_swap_halves(b, n);
    return n;
}

static const struct {
    const char *name;
    size_t (*build)(uint8_t *b);
} seeds[] = {
    {"linear-v3-16k", linear_v3},
    {"linear-v4-48k", linear_v4},
    {"supergame-128k", supergame},
    {"sg9-144k", sg9},
    {"supergame-cap", cap},
    {"headerless-32k", headerless},
    {"headerless-128k", headerless_sg},
    {"2600-4k", a2600_4k},
    {"2600-8k", a2600_8k},
    {"souper-v4", souper},
    {"activision", activision},
    {"size-zero", size_zero},
    {"bad-magic-1000", bad_magic},
    {"header-only", header_only},
    {"supergame-short", sg_short},
    {"oversized", oversized},
    {"v4-ignored", v4_ignored},
    {"size-mismatch", size_mismatch},
    {"two-chip-48k", two_chip},
    {"mirrored-32k", mirrored},
    {"activision-swapped", activision_swapped},
};

uint32_t seeds_count(void)
{
    return (uint32_t)(sizeof seeds / sizeof seeds[0]);
}

const char *seeds_name(uint32_t i)
{
    return seeds[i].name;
}

size_t seeds_build(uint32_t i, uint8_t *buf)
{
    return seeds[i].build(buf);
}
