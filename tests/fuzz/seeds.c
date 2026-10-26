/* The loader's seed inputs (seeds.h), each built by images.c. The names are
   stable: they become file names in the build directory. */
#include "seeds.h"
#include <stdlib.h>
#include <string.h>

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

#define RETAIL_SEED(name, type, bytes)                                                             \
    static size_t name(uint8_t *b)                                                                 \
    {                                                                                              \
        return images_a78(b, 3u, bytes, type, bytes);                                              \
    }
RETAIL_SEED(absolute, 0x0200u, 65536u)
RETAIL_SEED(fractalus, 0x0080u, 32768u)
RETAIL_SEED(activision_short, 0x0100u, 131071u)
RETAIL_SEED(activision_long, 0x0100u, 131073u)
RETAIL_SEED(absolute_short, 0x0200u, 65535u)
RETAIL_SEED(absolute_long, 0x0200u, 65537u)
RETAIL_SEED(fractalus_short, 0x0080u, 32767u)
RETAIL_SEED(fractalus_long, 0x0080u, 32769u)
#undef RETAIL_SEED

static size_t sg_a8(uint8_t *b)
{
    size_t n = images_a78(b, 4u, 32768u, 0x0080u, 32768u);
    b[64] = 1u;
    b[65] = 2u;
    return n;
}
static size_t bankset_a8(uint8_t *b)
{
    size_t n = sg_a8(b);
    b[65] = 0x82u;
    return n;
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

/* The loader corpus independently exercises typed header selection. */
#define TYPED_SEED(name, device)                                                                   \
    static size_t name(uint8_t *b)                                                                 \
    {                                                                                              \
        size_t n = images_linear(b, 3u, 16u * KIB);                                                \
        b[55] = b[56] = device;                                                                    \
        return n;                                                                                  \
    }
TYPED_SEED(lightgun, MUHTARI_PORT_LIGHTGUN)
TYPED_SEED(paddle, MUHTARI_PORT_PADDLE)
TYPED_SEED(keypad, MUHTARI_PORT_KEYPAD)
TYPED_SEED(driving, MUHTARI_PORT_DRIVING)
TYPED_SEED(trakball, MUHTARI_PORT_TRAKBALL)
TYPED_SEED(st_mouse, MUHTARI_PORT_ST_MOUSE)
TYPED_SEED(amiga_mouse, MUHTARI_PORT_AMIGA_MOUSE)
TYPED_SEED(snes, MUHTARI_PORT_SNES2ATARI)
TYPED_SEED(mega, MUHTARI_PORT_MEGA7800)
#undef TYPED_SEED

/* Both valid physical state and malformed declarations enter the existing
   probe/internal/public-load consumers; unsupported sound stays descriptive. */
#define EXPANSION_SEED(name, body, mapper, option, audio, xm)                                      \
    static size_t name(uint8_t *b)                                                                 \
    {                                                                                              \
        size_t n = images_a78(b, 4u, body, 0u, body);                                              \
        b[64] = mapper;                                                                            \
        b[65] = option;                                                                            \
        b[67] = audio;                                                                             \
        b[63] = xm;                                                                                \
        return n;                                                                                  \
    }
EXPANSION_SEED(bankset_shared, 65536u, 0u, 0x81u, 3u, 0u)
EXPANSION_SEED(bankset_halt, 1048576u, 1u, 0x83u, 3u, 1u)
EXPANSION_SEED(bankset_52k, 106496u, 0u, 0x80u, 0u, 1u)
EXPANSION_SEED(bankset_odd, 65535u, 0u, 0x80u, 0u, 0u)
EXPANSION_SEED(bankset_bad_ram, 98304u, 0u, 0x81u, 0u, 0u)
EXPANSION_SEED(bankset_unsupported, 65536u, 2u, 0x80u, 0u, 0u)
EXPANSION_SEED(xm_warning, 32768u, 0u, 0u, 0u, 0xFEu)
EXPANSION_SEED(xm_pokey_conflict, 32768u, 0u, 0u, 5u, 1u)
EXPANSION_SEED(audio_ym, 32768u, 0u, 0u, 8u, 0u)
EXPANSION_SEED(audio_covox, 32768u, 0u, 0u, 16u, 0u)
EXPANSION_SEED(audio_adpcm, 32768u, 0u, 0u, 32u, 0u)
#undef EXPANSION_SEED

static size_t bankset_truncated(uint8_t *b)
{
    return bankset_shared(b) - 1u;
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
    {"absolute", absolute},
    {"fractalus", fractalus},
    {"activision-short", activision_short},
    {"activision-long", activision_long},
    {"absolute-short", absolute_short},
    {"absolute-long", absolute_long},
    {"fractalus-short", fractalus_short},
    {"fractalus-long", fractalus_long},
    {"sg-a8", sg_a8},
    {"bankset-a8", bankset_a8},
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
    {"typed-lightgun", lightgun},
    {"typed-paddle", paddle},
    {"typed-keypad", keypad},
    {"typed-driving", driving},
    {"typed-trakball", trakball},
    {"typed-st-mouse", st_mouse},
    {"typed-amiga-mouse", amiga_mouse},
    {"typed-snes", snes},
    {"typed-mega", mega},
    {"bankset-shared", bankset_shared},
    {"bankset-halt-cap", bankset_halt},
    {"bankset-52k-xm", bankset_52k},
    {"bankset-odd", bankset_odd},
    {"bankset-truncated", bankset_truncated},
    {"bankset-bad-ram", bankset_bad_ram},
    {"bankset-unsupported-v4", bankset_unsupported},
    {"xm-byte63-warning", xm_warning},
    {"xm-pokey-conflict", xm_pokey_conflict},
    {"audio-ym", audio_ym},
    {"audio-covox", audio_covox},
    {"audio-adpcm-20", audio_adpcm},
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
    size_t n = seeds[i].build(buf);
    if (i >= 40u) {
        muhtari_cart_info info = {0};
        info.size = (uint32_t)sizeof info;
        muhtari_status want = MUHTARI_STATUS_OK;
        if (strcmp(seeds[i].name, "bankset-odd") == 0 ||
            strcmp(seeds[i].name, "bankset-bad-ram") == 0)
            want = MUHTARI_STATUS_ERR_INVALID_IMAGE;
        if (strcmp(seeds[i].name, "bankset-truncated") == 0)
            want = MUHTARI_STATUS_ERR_TRUNCATED_IMAGE;
        if (strcmp(seeds[i].name, "bankset-unsupported-v4") == 0)
            want = MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD;
        if (muhtari_probe(buf, n, &info) != want)
            abort();
        if (strcmp(seeds[i].name, "audio-adpcm-20") == 0 &&
            (info.a78_audio != 0x20u || (info.sound & MUHTARI_SOUND_ADPCM) == 0u))
            abort();
        if (strcmp(seeds[i].name, "xm-byte63-warning") == 0 &&
            (info.warnings & MUHTARI_CART_WARN_XM_VALUE) == 0u)
            abort();
    }
    uint32_t board = strcmp(seeds[i].name, "activision") == 0  ? MUHTARI_BOARD_ACTIVISION
                     : strcmp(seeds[i].name, "absolute") == 0  ? MUHTARI_BOARD_ABSOLUTE
                     : strcmp(seeds[i].name, "fractalus") == 0 ? MUHTARI_BOARD_FRACTALUS
                                                               : 0u;
    if (board != 0u) {
        muhtari_cart_info info = {0};
        info.size = (uint32_t)sizeof info;
        if (muhtari_probe(buf, n, &info) != MUHTARI_STATUS_OK || info.board != board)
            abort();
    }
    if ((strstr(seeds[i].name, "-short") != NULL || strstr(seeds[i].name, "-long") != NULL) &&
        (strncmp(seeds[i].name, "activision-", 11u) == 0 ||
         strncmp(seeds[i].name, "absolute-", 9u) == 0 ||
         strncmp(seeds[i].name, "fractalus-", 10u) == 0)) {
        muhtari_cart_info info = {0};
        info.size = (uint32_t)sizeof info;
        if (muhtari_probe(buf, n, &info) != MUHTARI_STATUS_ERR_INVALID_IMAGE)
            abort();
    }
#ifdef MUHTARI_FUZZ_REPLAY
    if (strcmp(seeds[i].name, "activision-swapped") == 0) {
        muhtari_cart_info info = {0};
        info.size = (uint32_t)sizeof info;
        if (muhtari_probe(buf, n, &info) != MUHTARI_STATUS_OK ||
            (info.flags & MUHTARI_CART_ACTIVISION_HALF_SWAPPED) == 0u)
            abort();
    }
#endif
    return n;
}
