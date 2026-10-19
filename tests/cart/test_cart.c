/* muhtari_probe and the internal load on images built in memory (images.h),
   one case per argv. The program links the core's objects rather than the
   archive. */
#include <string.h>

#include "../alloc.h"
#include "cartdb.h"
#include "images.h"
#include "internal.h"
#include "muhtari.h"
#include "sha1.h"
#include "../check.h"

/* Room for the largest image probed, one byte past the cap. */
static uint8_t buf[MUHTARI_IMAGE_SIZE_MAX + 1u];

/* Fills *out with byte b and sets its size tag to the struct's size. */
static void prefill(muhtari_cart_info *out, uint8_t b)
{
    memset(out, b, sizeof *out);
    out->size = (uint32_t)sizeof *out;
}

/* Probes n bytes at img (NULL allowed) and checks the status and that out
   was left exactly as the caller filled it. */
static void expect_untouched(const char *what, const void *img, size_t n, muhtari_status want)
{
    muhtari_cart_info out;
    muhtari_cart_info before;
    prefill(&out, 0x5Au);
    before = out;
    muhtari_status got = muhtari_probe(img, n, &out);
    if (got != want) {
        fprintf(stderr, "%s: expected status %u, got %u\n", what, (unsigned)want, (unsigned)got);
        check_failures++;
    }
    if (memcmp(&out, &before, sizeof out) != 0) {
        fprintf(stderr, "%s: out was written\n", what);
        check_failures++;
    }
}

/* Probes n bytes at img, expecting want and a written out. */
static muhtari_cart_info expect_written(const char *what, const void *img, size_t n,
                                        muhtari_status want)
{
    muhtari_cart_info out;
    prefill(&out, 0x5Au);
    muhtari_status got = muhtari_probe(img, n, &out);
    if (got != want) {
        fprintf(stderr, "%s: expected status %u, got %u\n", what, (unsigned)want, (unsigned)got);
        check_failures++;
    }
    CHECK_EQ_U64(out.size, sizeof out);
    return out;
}

/* A one-entry test list: the 7800 key is the SHA-1 of n bytes at img,
   pointing at entry, and the 2600 list holds that SHA-1's first 8 bytes when
   as_2600 is set. Returns identify's status. */
static muhtari_status identify_listed(const uint8_t *img, size_t n, const uint8_t *hashed,
                                      size_t hashed_n, struct muhtari__cartdb_entry entry,
                                      int as_2600, muhtari_cart_info *out)
{
    struct muhtari__sha1 ctx;
    struct muhtari__cartdb_key key;
    uint8_t prefix[1][8];
    memset(&key, 0, sizeof key);
    muhtari__sha1_init(&ctx);
    muhtari__sha1_update(&ctx, hashed, hashed_n);
    muhtari__sha1_final(&ctx, key.sha1);
    memcpy(prefix[0], key.sha1, 8);
    return muhtari__cart_identify(img, n, &key, as_2600 ? 0u : 1u, &entry, 1u, NULL, 0u,
                                  (const uint8_t (*)[8])prefix, as_2600 ? 1u : 0u, out);
}

/* A clean linear header over a valid body probes OK with every field from
   the header, and two calls on the same bytes write the same 140 bytes. */
static void test_accepted(void)
{
    static const uint8_t versions[2] = {3u, 4u};
    for (size_t v = 0; v < sizeof versions; v++) {
        size_t n = images_linear(buf, versions[v], 32u * IMAGES_KIB);
        muhtari_cart_info a;
        muhtari_cart_info b;
        prefill(&a, 0x00u);
        prefill(&b, 0xA5u);
        CHECK_EQ_U64(muhtari_probe(buf, n, &a), MUHTARI_STATUS_OK);
        CHECK_EQ_U64(muhtari_probe(buf, n, &b), MUHTARI_STATUS_OK);
        CHECK(memcmp(&a, &b, sizeof a) == 0);
        CHECK_EQ_U64(a.size, sizeof a);
        CHECK_EQ_U64(a.kind, MUHTARI_KIND_7800);
        CHECK_EQ_U64(a.source, MUHTARI_SOURCE_HEADER);
        CHECK_EQ_U64(a.flags, MUHTARI_CART_HEADER);
        CHECK_EQ_U64(a.warnings, 0);
        CHECK_EQ_U64(a.rom_size, 32u * IMAGES_KIB);
        CHECK_EQ_U64(a.board, MUHTARI_BOARD_LINEAR);
        CHECK_EQ_U64(a.board_options, 0);
        CHECK_EQ_U64(a.ram_size, 0);
        CHECK_EQ_U64(a.sound, 0);
        CHECK_EQ_U64(a.devices, 0);
        CHECK_EQ_U64(a.port1, MUHTARI_PORT_PROLINE);
        CHECK_EQ_U64(a.port2, MUHTARI_PORT_PROLINE);
        CHECK_EQ_U64(a.region, MUHTARI_REGION_NTSC);
        CHECK_EQ_U64(a.a78_version, versions[v]);
        CHECK_EQ_U64(a.a78_rom_size, 32u * IMAGES_KIB);
        CHECK_EQ_U64(a.a78_cart_type, 0);
        CHECK_EQ_U64(a.a78_port1, 1);
        CHECK_EQ_U64(a.a78_port2, 1);
        CHECK_EQ_U64(a.a78_tv, 0);
        CHECK_EQ_U64(a.a78_save, 0);
        CHECK_EQ_U64(a.a78_xm, 0);
        CHECK_EQ_U64(a.a78_mapper, 0);
        CHECK_EQ_U64(a.a78_mapper_options, 0);
        CHECK_EQ_U64(a.a78_audio, 0);
        CHECK_EQ_U64(a.a78_irq, 0);
        uint8_t title[36];
        memset(title, 0, sizeof title);
        memcpy(title, "Clean Linear", 12);
        CHECK(memcmp(a.title, title, sizeof title) == 0);
    }

    /* A SuperGame header with 16 KiB of RAM at $4000 over 128 KiB. */
    uint32_t body = 128u * IMAGES_KIB;
    images_header(buf, 3u, "SuperGame", body, 0x0006u);
    images_fill(buf + IMAGES_HEADER, body, 7u);
    muhtari_cart_info sg;
    prefill(&sg, 0u);
    CHECK_EQ_U64(muhtari_probe(buf, IMAGES_HEADER + body, &sg), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(sg.board, MUHTARI_BOARD_SUPERGAME);
    CHECK_EQ_U64(sg.ram_size, 16u * IMAGES_KIB);
    CHECK_EQ_U64(sg.sound, 0);
    CHECK_EQ_U64(sg.rom_size, body);
    CHECK_EQ_U64(sg.warnings, 0);

    /* Headerless images by their footer: linear up to 48 KiB, else SuperGame. */
    static const struct {
        uint32_t n;
        uint32_t board;
    } bare[] = {{4u * IMAGES_KIB, MUHTARI_BOARD_LINEAR},
                {48u * IMAGES_KIB, MUHTARI_BOARD_LINEAR},
                {128u * IMAGES_KIB, MUHTARI_BOARD_SUPERGAME}};
    for (size_t i = 0; i < sizeof bare / sizeof bare[0]; i++) {
        size_t n = images_headerless(buf, bare[i].n);
        muhtari_cart_info out = expect_written("footer", buf, n, MUHTARI_STATUS_OK);
        CHECK_EQ_U64(out.kind, MUHTARI_KIND_7800);
        CHECK_EQ_U64(out.source, MUHTARI_SOURCE_FOOTER);
        CHECK_EQ_U64(out.flags, 0);
        CHECK_EQ_U64(out.rom_size, bare[i].n);
        CHECK_EQ_U64(out.board, bare[i].board);
        CHECK_EQ_U64(out.port1, MUHTARI_PORT_PROLINE);
        CHECK_EQ_U64(out.port2, MUHTARI_PORT_PROLINE);
        CHECK_EQ_U64(out.region, MUHTARI_REGION_NTSC);
        CHECK_EQ_U64(out.a78_version, 0);
        CHECK_EQ_U64(out.title[0], 0);
    }

    /* A headerless image matching a PAL light-gun list entry takes the
       entry's board, RAM, sound and region, and a light gun on port 1. */
    size_t n = images_2600(buf, 16u * IMAGES_KIB);
    struct muhtari__cartdb_entry e;
    memset(&e, 0, sizeof e);
    e.board = MUHTARI_BOARD_SUPERGAME;
    e.ram_kib = 16u;
    e.sound = MUHTARI_SOUND_POKEY_450;
    e.flags = MUHTARI__CARTDB_PAL | MUHTARI__CARTDB_LIGHTGUN;
    e.parts = 1u;
    muhtari_cart_info out;
    CHECK_EQ_U64(identify_listed(buf, n, buf, n, e, 0, &out), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(out.kind, MUHTARI_KIND_7800);
    CHECK_EQ_U64(out.source, MUHTARI_SOURCE_HASH);
    CHECK_EQ_U64(out.flags, MUHTARI_CART_HASH_MATCH);
    CHECK_EQ_U64(out.board, MUHTARI_BOARD_SUPERGAME);
    CHECK_EQ_U64(out.ram_size, 16u * IMAGES_KIB);
    CHECK_EQ_U64(out.sound, MUHTARI_SOUND_POKEY_450);
    CHECK_EQ_U64(out.region, MUHTARI_REGION_PAL);
    CHECK_EQ_U64(out.port1, MUHTARI_PORT_LIGHTGUN);
    CHECK_EQ_U64(out.port2, MUHTARI_PORT_PROLINE);
    CHECK_EQ_U64(out.rom_size, n);
}

/* The argument checks come first, and none of them writes out. */
static void test_arguments(void)
{
    size_t n = images_linear(buf, 3u, 4u * IMAGES_KIB);
    CHECK_EQ_U64(muhtari_probe(buf, n, NULL), MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK_EQ_U64(muhtari_probe(NULL, 0u, NULL), MUHTARI_STATUS_ERR_ARGUMENT);
    expect_untouched("NULL image, n 1", NULL, 1u, MUHTARI_STATUS_ERR_ARGUMENT);
    expect_untouched("NULL image, n 0", NULL, 0u, MUHTARI_STATUS_ERR_INVALID_IMAGE);

    /* Size tags: one below ours, and ours + 8 with a non-zero tail byte. */
    union {
        muhtari_cart_info info;
        uint8_t bytes[sizeof(muhtari_cart_info) + 16u];
    } t;
    uint8_t before[sizeof t.bytes];
    memset(t.bytes, 0, sizeof t.bytes);
    t.info.size = (uint32_t)sizeof t.info - 1u;
    memcpy(before, t.bytes, sizeof before);
    CHECK_EQ_U64(muhtari_probe(buf, n, &t.info), MUHTARI_STATUS_ERR_STRUCT_SIZE);
    CHECK(memcmp(t.bytes, before, sizeof before) == 0);
    memset(t.bytes, 0, sizeof t.bytes);
    t.info.size = (uint32_t)sizeof t.info + 8u;
    t.bytes[sizeof t.info + 3u] = 1u;
    memcpy(before, t.bytes, sizeof before);
    CHECK_EQ_U64(muhtari_probe(buf, n, &t.info), MUHTARI_STATUS_ERR_STRUCT_SIZE);
    CHECK(memcmp(t.bytes, before, sizeof before) == 0);
    /* The size tag is checked before the image is looked at. */
    t.info.size = 4u;
    CHECK_EQ_U64(muhtari_probe(buf, MUHTARI_IMAGE_SIZE_MAX + 1u, &t.info),
                 MUHTARI_STATUS_ERR_STRUCT_SIZE);
}

/* The cap is a 1 MiB SuperGame body under a header; one byte more is refused
   before anything is hashed. */
static void test_oversized(void)
{
    uint32_t body = 1024u * IMAGES_KIB;
    size_t n = images_a78(buf, 3u, body, 0x0002u, body);
    CHECK_EQ_U64(n, MUHTARI_IMAGE_SIZE_MAX);
    muhtari_cart_info out = expect_written("cap", buf, n, MUHTARI_STATUS_OK);
    CHECK_EQ_U64(out.board, MUHTARI_BOARD_SUPERGAME);
    CHECK_EQ_U64(out.rom_size, body);
    buf[n] = 0u;
    expect_untouched("cap + 1", buf, n + 1u, MUHTARI_STATUS_ERR_UNSUPPORTED_SIZE);
}

/* A 2600 image writes only kind, source, rom_size and flags. */
static void check_2600(const char *what, const muhtari_cart_info *got, uint32_t source,
                       uint32_t flags, uint32_t n)
{
    muhtari_cart_info want;
    memset(&want, 0, sizeof want);
    want.size = (uint32_t)sizeof want;
    want.kind = MUHTARI_KIND_2600;
    want.source = source;
    want.flags = flags;
    want.rom_size = n;
    if (memcmp(got, &want, sizeof want) != 0) {
        fprintf(stderr, "%s: kind %u source %u flags %u rom_size %u, or another field set\n", what,
                (unsigned)got->kind, (unsigned)got->source, (unsigned)got->flags,
                (unsigned)got->rom_size);
        check_failures++;
    }
}

/* 2600 images by size with a failing footer, and by the 2600 list. */
static void test_2600(void)
{
    static const uint32_t sizes[] = {2048u, 4096u, 8192u, 16384u, 10495u, 8448u, 3u * 8448u};
    for (size_t i = 0; i < sizeof sizes / sizeof sizes[0]; i++) {
        size_t n = images_2600(buf, sizes[i]);
        muhtari_cart_info out =
            expect_written("2600 size", buf, n, MUHTARI_STATUS_ERR_UNSUPPORTED_2600);
        check_2600("2600 size", &out, MUHTARI_SOURCE_SIZE, 0u, sizes[i]);
    }
    /* A 2600 list hit is refused even where the footer would pass. */
    size_t n = images_headerless(buf, 8u * IMAGES_KIB);
    struct muhtari__cartdb_entry none;
    memset(&none, 0, sizeof none);
    muhtari_cart_info out;
    CHECK_EQ_U64(identify_listed(buf, n, buf, n, none, 1, &out),
                 MUHTARI_STATUS_ERR_UNSUPPORTED_2600);
    check_2600("2600 list", &out, MUHTARI_SOURCE_HASH, MUHTARI_CART_HASH_MATCH, (uint32_t)n);
}

/* Each board the loader does not run is refused with out written. */
static void test_unsupported_board(void)
{
    static const struct {
        const char *name;
        uint16_t cart_type;
        uint32_t body;
        uint32_t board;
        uint32_t options;
    } boards[] = {
        {"Activision", 0x0100u, 128u * IMAGES_KIB, MUHTARI_BOARD_ACTIVISION, 0u},
        {"Absolute", 0x0200u, 64u * IMAGES_KIB, MUHTARI_BOARD_ABSOLUTE, 0u},
        {"Fractalus", 0x0080u, 32u * IMAGES_KIB, MUHTARI_BOARD_FRACTALUS, 0u},
        {"bankset", 0x2002u, 64u * IMAGES_KIB, MUHTARI_BOARD_BANKSET, 0u},
        {"halt-banked RAM", 0x4002u, 128u * IMAGES_KIB, MUHTARI_BOARD_SUPERGAME,
         MUHTARI_BOARD_HALT_BANKED_RAM},
        {"Souper, cart type", 0x1000u, 512u * IMAGES_KIB, MUHTARI_BOARD_SOUPER, 0u},
    };
    for (size_t i = 0; i < sizeof boards / sizeof boards[0]; i++) {
        size_t n = images_a78(buf, 3u, boards[i].body, boards[i].cart_type, boards[i].body);
        muhtari_cart_info out =
            expect_written(boards[i].name, buf, n, MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD);
        CHECK_EQ_U64(out.kind, MUHTARI_KIND_7800);
        CHECK_EQ_U64(out.flags, MUHTARI_CART_HEADER);
        CHECK_EQ_U64(out.board, boards[i].board);
        CHECK_EQ_U64(out.board_options, boards[i].options);
    }
    /* Souper as the version 4 mapper with cart-type bit 12. */
    size_t n = images_a78(buf, 4u, 512u * IMAGES_KIB, 0x1000u, 512u * IMAGES_KIB);
    buf[64] = 4u;
    muhtari_cart_info out = expect_written("Souper", buf, n, MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD);
    CHECK_EQ_U64(out.board, MUHTARI_BOARD_SOUPER);
    CHECK_EQ_U64(out.a78_mapper, 4);
    /* A list entry on a refused board refuses a headerless image. */
    n = images_headerless(buf, 32u * IMAGES_KIB);
    struct muhtari__cartdb_entry act;
    memset(&act, 0, sizeof act);
    act.board = MUHTARI_BOARD_ACTIVISION;
    act.flags = MUHTARI__CARTDB_NTSC;
    act.parts = 1u;
    CHECK_EQ_U64(identify_listed(buf, n, buf, n, act, 0, &out),
                 MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD);
    CHECK_EQ_U64(out.board, MUHTARI_BOARD_ACTIVISION);
    CHECK_EQ_U64(out.source, MUHTARI_SOURCE_HASH);
}

/* Not a cartridge image, or a size no board has. */
static void test_malformed(void)
{
    size_t n = images_a78(buf, 3u, 0u, 0u, 32u * IMAGES_KIB);
    expect_untouched("size field 0", buf, n, MUHTARI_STATUS_ERR_INVALID_IMAGE);
    images_fill(buf, 1000u, 1000u);
    buf[1] = (uint8_t)'X';
    buf[1000u - 3u] = 0u; /* reset $00xx: the footer fails */
    expect_untouched("bad magic, 1,000 bytes", buf, 1000u, MUHTARI_STATUS_ERR_INVALID_IMAGE);
    n = images_a78(buf, 3u, 40u * IMAGES_KIB, 0x0002u, 40u * IMAGES_KIB);
    expect_untouched("SuperGame over 40 KiB", buf, n, MUHTARI_STATUS_ERR_INVALID_IMAGE);
    n = images_a78(buf, 3u, 50u * IMAGES_KIB, 0u, 50u * IMAGES_KIB);
    expect_untouched("linear over 50 KiB", buf, n, MUHTARI_STATUS_ERR_INVALID_IMAGE);
    /* The footer passes, but 10 KiB fits no board. */
    n = images_headerless(buf, 10u * IMAGES_KIB);
    expect_untouched("footer over 10 KiB", buf, n, MUHTARI_STATUS_ERR_INVALID_IMAGE);
    expect_untouched("7 bytes", buf, 7u, MUHTARI_STATUS_ERR_INVALID_IMAGE);
    /* A list entry that is not a cartridge, headered and headerless. */
    struct muhtari__cartdb_entry hsc;
    memset(&hsc, 0, sizeof hsc);
    hsc.flags = MUHTARI__CARTDB_NOT_CART;
    hsc.parts = 1u;
    muhtari_cart_info out;
    n = images_headerless(buf, 4u * IMAGES_KIB);
    CHECK_EQ_U64(identify_listed(buf, n, buf, n, hsc, 0, &out), MUHTARI_STATUS_ERR_INVALID_IMAGE);
    n = images_linear(buf, 3u, 4u * IMAGES_KIB);
    CHECK_EQ_U64(identify_listed(buf, n, buf + IMAGES_HEADER, n - IMAGES_HEADER, hsc, 0, &out),
                 MUHTARI_STATUS_ERR_INVALID_IMAGE);
}

/* An A78 file that ends early. */
static void test_truncated(void)
{
    images_header(buf, 3u, "Short", 32u * IMAGES_KIB, 0u);
    expect_untouched("header only", buf, IMAGES_HEADER, MUHTARI_STATUS_ERR_TRUNCATED_IMAGE);
    expect_untouched("inside the header", buf, 100u, MUHTARI_STATUS_ERR_TRUNCATED_IMAGE);
    size_t n = images_a78(buf, 3u, 128u * IMAGES_KIB, 0x0002u, 40u * IMAGES_KIB);
    expect_untouched("SuperGame 128 KiB over 40 KiB", buf, n, MUHTARI_STATUS_ERR_TRUNCATED_IMAGE);
    n = images_a78(buf, 3u, 48u * IMAGES_KIB, 0u, 10u * IMAGES_KIB);
    expect_untouched("linear 48 KiB over 10 KiB", buf, n, MUHTARI_STATUS_ERR_TRUNCATED_IMAGE);
}

/* Probes the image, expecting OK and exactly the warning bits want. */
static muhtari_cart_info expect_warnings(const char *what, size_t n, uint32_t want)
{
    muhtari_cart_info out = expect_written(what, buf, n, MUHTARI_STATUS_OK);
    if (out.warnings != want) {
        fprintf(stderr, "%s: expected warnings 0x%x, got 0x%x\n", what, (unsigned)want,
                (unsigned)out.warnings);
        check_failures++;
    }
    return out;
}

/* Each header defect the probe accepts sets its own warning bit; a clean
   header sets none. */
static void test_warnings(void)
{
    uint32_t body = 32u * IMAGES_KIB;
    size_t n = images_linear(buf, 4u, body);
    expect_warnings("clean header", n, 0u);

    /* Magic padded with spaces, and a title padded with spaces and no NUL,
       as the 7800basic tools write them. */
    memset(buf + 10, ' ', 7);
    memset(buf + 17, ' ', 32);
    memcpy(buf + 17, "My Game", 7);
    muhtari_cart_info out = expect_warnings("space padding", n, 0u);
    CHECK(memcmp(out.title, "My Game\0", 8) == 0);
    memset(buf + 17, 'A', 32);
    out = expect_warnings("32-byte title", n, 0u);
    CHECK_EQ_U64(out.title[31], 'A');
    CHECK_EQ_U64(out.title[32], 0);

    n = images_linear(buf, 4u, body);
    buf[64] = 9u; /* mapper 9: the version 4 block is invalid */
    buf[53] = 0x00u;
    buf[54] = 0x01u; /* the cart type says POKEY at $4000 */
    out = expect_warnings("invalid v4 block", n, MUHTARI_CART_WARN_HEADER_V4_IGNORED);
    CHECK_EQ_U64(out.sound, MUHTARI_SOUND_POKEY_4000);
    CHECK_EQ_U64(out.a78_mapper, 9);

    /* A zero version 4 block says linear, which 128 KiB does not fit, so
       the cart type's SuperGame is used. */
    n = images_a78(buf, 4u, 128u * IMAGES_KIB, 0x0002u, 128u * IMAGES_KIB);
    out = expect_warnings("v4 size misfit", n, MUHTARI_CART_WARN_HEADER_V4_IGNORED);
    CHECK_EQ_U64(out.board, MUHTARI_BOARD_SUPERGAME);

    /* A valid version 4 block wins over the cart type. */
    n = images_a78(buf, 4u, 128u * IMAGES_KIB, 0x0000u, 128u * IMAGES_KIB);
    buf[64] = 1u;
    buf[65] = 6u;    /* 32K EXRAM/X2 */
    buf[67] = 0x1Au; /* POKEY at $0450, YM2151, Covox */
    out = expect_warnings("v4 block", n, 0u);
    CHECK_EQ_U64(out.board, MUHTARI_BOARD_SUPERGAME);
    CHECK_EQ_U64(out.board_options, MUHTARI_BOARD_BANKED_RAM);
    CHECK_EQ_U64(out.ram_size, 32u * IMAGES_KIB);
    CHECK_EQ_U64(out.sound, MUHTARI_SOUND_POKEY_450 | MUHTARI_SOUND_YM2151 | MUHTARI_SOUND_COVOX);

    n = images_linear(buf, 5u, body);
    out = expect_warnings("version 5", n, MUHTARI_CART_WARN_VERSION_ABOVE_4);
    CHECK_EQ_U64(out.a78_version, 5);

    /* A size field that counts the header over a valid linear body. */
    n = images_linear(buf, 4u, body);
    buf[51] = (uint8_t)((body + IMAGES_HEADER) >> 8);
    buf[52] = (uint8_t)(body + IMAGES_HEADER);
    out = expect_warnings("size field + 128", n, MUHTARI_CART_WARN_SIZE_FIELD_MISMATCH);
    CHECK_EQ_U64(out.rom_size, body);
    CHECK_EQ_U64(out.a78_rom_size, body + IMAGES_HEADER);
    /* A short body that satisfies its geometry loads as the bytes present. */
    n = images_a78(buf, 4u, 48u * IMAGES_KIB, 0u, body);
    out = expect_warnings("short body", n, MUHTARI_CART_WARN_SIZE_FIELD_MISMATCH);
    CHECK_EQ_U64(out.rom_size, body);

    n = images_linear(buf, 4u, body);
    buf[20] = 0x01u;
    out = expect_warnings("control byte in title", n, MUHTARI_CART_WARN_TITLE_NONPRINTABLE);
    CHECK_EQ_U64(out.title[3], '?');
    buf[20] = 0x80u;
    expect_warnings("high byte in title", n, MUHTARI_CART_WARN_TITLE_NONPRINTABLE);
    buf[20] = 0u;
    buf[21] = 0x01u; /* after the first NUL: not part of the title */
    expect_warnings("byte after NUL", n, 0u);

    n = images_linear(buf, 4u, body);
    buf[56] = 13u;
    out = expect_warnings("port code 13", n, MUHTARI_CART_WARN_UNKNOWN_PORT);
    CHECK_EQ_U64(out.port2, MUHTARI_PORT_NONE);
    CHECK_EQ_U64(out.a78_port2, 13);
    CHECK_EQ_U64(out.port1, MUHTARI_PORT_PROLINE);

    n = images_linear(buf, 4u, body);
    buf[58] = 0x04u;
    out = expect_warnings("save bit 2", n, MUHTARI_CART_WARN_LEGACY_SAVE_BIT);
    CHECK_EQ_U64(out.devices, MUHTARI_DEVICE_HSC);
    buf[58] = 0x03u;
    out = expect_warnings("HSC and SaveKey", n, 0u);
    CHECK_EQ_U64(out.devices, MUHTARI_DEVICE_HSC | MUHTARI_DEVICE_SAVEKEY);

    n = images_linear(buf, 4u, body);
    buf[63] = 2u;
    out = expect_warnings("XM byte 2", n, MUHTARI_CART_WARN_XM_VALUE);
    CHECK_EQ_U64(out.devices, MUHTARI_DEVICE_XM);
    buf[63] = 1u;
    expect_warnings("XM byte 1", n, 0u);

    static const uint32_t reserved[] = {59u, 62u, 70u, 99u};
    for (size_t i = 0; i < sizeof reserved / sizeof reserved[0]; i++) {
        n = images_linear(buf, 4u, body);
        buf[reserved[i]] = 1u;
        expect_warnings("reserved byte", n, MUHTARI_CART_WARN_RESERVED_NONZERO);
    }
    n = images_linear(buf, 3u, body);
    buf[64] = 1u; /* below version 4 the v4 block is reserved */
    out = expect_warnings("v4 byte in a v3 header", n, MUHTARI_CART_WARN_RESERVED_NONZERO);
    CHECK_EQ_U64(out.a78_mapper, 0);
    CHECK_EQ_U64(out.board, MUHTARI_BOARD_LINEAR);

    /* Cart-type bits that cannot all hold, resolved in MAME's order. Over
       144 KiB every SuperGame variant fits. */
    n = images_a78(buf, 3u, 144u * IMAGES_KIB, 0x0002u, 144u * IMAGES_KIB);
    buf[54] = 0x07u; /* POKEY at $4000 and RAM at $4000: the RAM stays */
    out = expect_warnings("POKEY and RAM", n, MUHTARI_CART_WARN_CART_TYPE_CONFLICT);
    CHECK_EQ_U64(out.sound, 0);
    CHECK_EQ_U64(out.ram_size, 16u * IMAGES_KIB);
    buf[54] = 0x13u; /* POKEY at $4000 and bank 6 at $4000 */
    out = expect_warnings("POKEY and bank 6", n, MUHTARI_CART_WARN_CART_TYPE_CONFLICT);
    CHECK_EQ_U64(out.sound, 0);
    CHECK_EQ_U64(out.board_options, MUHTARI_BOARD_BANK6_AT_4000);
    buf[54] = 0x0Eu; /* RAM at $4000 and ROM at $4000: the ROM stays */
    out = expect_warnings("RAM and ROM", n, MUHTARI_CART_WARN_CART_TYPE_CONFLICT);
    CHECK_EQ_U64(out.ram_size, 0);
    CHECK_EQ_U64(out.board_options, MUHTARI_BOARD_ROM_AT_4000);
    buf[54] = 0x3Au; /* ROM, bank 6 and banked RAM: banked RAM stays */
    out = expect_warnings("ROM, bank 6, banked RAM", n, MUHTARI_CART_WARN_CART_TYPE_CONFLICT);
    CHECK_EQ_U64(out.board_options, MUHTARI_BOARD_BANKED_RAM);
    buf[54] = 0x0Bu; /* POKEY at $4000 and ROM at $4000 are kept together */
    out = expect_warnings("POKEY and ROM", n, 0u);
    CHECK_EQ_U64(out.sound, MUHTARI_SOUND_POKEY_4000);
    CHECK_EQ_U64(out.board_options, MUHTARI_BOARD_ROM_AT_4000);
    buf[54] = 0x08u; /* ROM at $4000 without SuperGame: SuperGame is set */
    out = expect_warnings("ROM without SuperGame", n, MUHTARI_CART_WARN_CART_TYPE_CONFLICT);
    CHECK_EQ_U64(out.board, MUHTARI_BOARD_SUPERGAME);
    /* Two special boards at once, and Activision with low-byte bits. */
    n = images_a78(buf, 3u, 128u * IMAGES_KIB, 0x0300u, 128u * IMAGES_KIB);
    out = expect_written("Activision and Absolute", buf, n, MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD);
    CHECK_EQ_U64(out.warnings, MUHTARI_CART_WARN_CART_TYPE_CONFLICT);
    n = images_a78(buf, 3u, 128u * IMAGES_KIB, 0x0141u, 128u * IMAGES_KIB);
    out = expect_written("Activision and POKEYs", buf, n, MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD);
    CHECK_EQ_U64(out.warnings, MUHTARI_CART_WARN_CART_TYPE_CONFLICT);
    CHECK_EQ_U64(out.sound, 0);
}

/* A header over the 64 KiB body the test list keys as SuperGame with 16 KiB
   of RAM, PAL, light gun. Returns the image's size. */
static size_t override_image(uint16_t cart_type, uint8_t tv)
{
    images_header(buf, 3u, "Override", 64u * IMAGES_KIB, cart_type);
    buf[56] = MUHTARI_PORT_JOYSTICK;
    buf[57] = tv;
    images_seeded(buf + IMAGES_HEADER, 64u * IMAGES_KIB, IMAGES_SEED_OVERRIDE);
    return IMAGES_HEADER + 64u * IMAGES_KIB;
}

/* A list match beats the header for the board, RAM and sound only: the
   ports stay the header's, and so does the region unless the header names
   none (multi-region), when the entry's applies. */
static void test_hash_override(void)
{
    size_t n = override_image(0x0000u, 0u); /* the header says plain linear */
    muhtari_cart_info out = expect_written("override", buf, n, MUHTARI_STATUS_OK);
    CHECK_EQ_U64(out.kind, MUHTARI_KIND_7800);
    CHECK_EQ_U64(out.source, MUHTARI_SOURCE_HASH);
    CHECK_EQ_U64(out.flags, MUHTARI_CART_HEADER | MUHTARI_CART_HASH_MATCH);
    CHECK_EQ_HEX(out.warnings, MUHTARI_CART_WARN_HEADER_CORRECTED);
    CHECK_EQ_U64(out.board, MUHTARI_BOARD_SUPERGAME);
    CHECK_EQ_U64(out.board_options, 0);
    CHECK_EQ_U64(out.ram_size, 16u * IMAGES_KIB);
    CHECK_EQ_U64(out.sound, 0);
    CHECK_EQ_U64(out.rom_size, 64u * IMAGES_KIB);
    CHECK_EQ_U64(out.port1, MUHTARI_PORT_PROLINE); /* the entry's light gun is not used */
    CHECK_EQ_U64(out.port2, MUHTARI_PORT_JOYSTICK);
    CHECK_EQ_U64(out.region, MUHTARI_REGION_NTSC); /* the header's, not the PAL entry's */

    /* TV bit 2, multi-region: the header names no region, so the entry's. */
    n = override_image(0x0000u, 0x04u);
    out = expect_written("override, multi-region", buf, n, MUHTARI_STATUS_OK);
    CHECK_EQ_U64(out.region, MUHTARI_REGION_PAL);
    CHECK_EQ_HEX(out.warnings, MUHTARI_CART_WARN_HEADER_CORRECTED);
    CHECK_EQ_U64(out.port2, MUHTARI_PORT_JOYSTICK);

    /* A header that already says SuperGame with RAM is not corrected. */
    n = override_image(0x0006u, 0u);
    out = expect_written("override, header agrees", buf, n, MUHTARI_STATUS_OK);
    CHECK_EQ_HEX(out.warnings, 0);
    CHECK_EQ_U64(out.source, MUHTARI_SOURCE_HASH);
    CHECK_EQ_U64(out.ram_size, 16u * IMAGES_KIB);

    /* One body byte changed: no match, so the header's linear board. */
    n = override_image(0x0000u, 0u);
    buf[IMAGES_HEADER + 5u] ^= 1u;
    out = expect_written("override, no match", buf, n, MUHTARI_STATUS_OK);
    CHECK_EQ_U64(out.source, MUHTARI_SOURCE_HEADER);
    CHECK_EQ_U64(out.flags, MUHTARI_CART_HEADER);
    CHECK_EQ_U64(out.board, MUHTARI_BOARD_LINEAR);
}

/* Multi-chip entries: each slice of a layout whose sizes sum to the body is
   hashed, and the entry matches when its parts come in order. */
static void test_slices(void)
{
    /* 16 KiB + 32 KiB chips; a 32 + 16 KiB layout with no entry comes first. */
    uint32_t body = 48u * IMAGES_KIB;
    size_t n = images_seeded(buf, body, IMAGES_SEED_TWO_CHIP);
    muhtari_cart_info out = expect_written("two chips", buf, n, MUHTARI_STATUS_OK);
    CHECK_EQ_U64(out.source, MUHTARI_SOURCE_HASH);
    CHECK_EQ_U64(out.flags, MUHTARI_CART_HASH_MATCH);
    CHECK_EQ_U64(out.board, MUHTARI_BOARD_LINEAR);
    CHECK_EQ_U64(out.sound, MUHTARI_SOUND_POKEY_450);
    CHECK_EQ_U64(out.region, MUHTARI_REGION_PAL);
    CHECK_EQ_U64(out.rom_size, body);
    /* The same body under a header that says no POKEY: corrected. */
    images_header(buf, 3u, "Two chips", body, 0u);
    images_seeded(buf + IMAGES_HEADER, body, IMAGES_SEED_TWO_CHIP);
    out = expect_written("two chips, headered", buf, IMAGES_HEADER + body, MUHTARI_STATUS_OK);
    CHECK_EQ_U64(out.source, MUHTARI_SOURCE_HASH);
    CHECK_EQ_U64(out.flags, MUHTARI_CART_HEADER | MUHTARI_CART_HASH_MATCH);
    CHECK_EQ_U64(out.sound, MUHTARI_SOUND_POKEY_450);
    CHECK_EQ_HEX(out.warnings, MUHTARI_CART_WARN_HEADER_CORRECTED);
    /* One byte changed in either slice: no match, so the footer test. */
    static const uint32_t changed[2] = {100u, 20u * IMAGES_KIB};
    for (size_t i = 0; i < 2u; i++) {
        n = images_seeded(buf, body, IMAGES_SEED_TWO_CHIP);
        buf[changed[i]] ^= 1u;
        out = expect_written("two chips, one changed", buf, n, MUHTARI_STATUS_OK);
        CHECK_EQ_U64(out.source, MUHTARI_SOURCE_FOOTER);
        CHECK_EQ_U64(out.sound, 0);
    }

    /* 16 KiB + one 8 KiB chip mapped twice. */
    n = images_mirrored(buf);
    out = expect_written("mirrored chip", buf, n, MUHTARI_STATUS_OK);
    CHECK_EQ_U64(out.source, MUHTARI_SOURCE_HASH);
    CHECK_EQ_U64(out.board, MUHTARI_BOARD_SUPERGAME);
    /* The two copies differ: no match, so the footer's linear board. */
    buf[16u * IMAGES_KIB] ^= 1u;
    out = expect_written("mirrored chip, copies differ", buf, n, MUHTARI_STATUS_OK);
    CHECK_EQ_U64(out.source, MUHTARI_SOURCE_FOOTER);
    CHECK_EQ_U64(out.board, MUHTARI_BOARD_LINEAR);
}

/* A 128 KiB Activision dump in the other layout matches its entry through
   the half-swapped pass; another size gets no such pass. */
static void test_activision_swap(void)
{
    uint32_t body = 128u * IMAGES_KIB;
    size_t n = images_seeded(buf, body, IMAGES_SEED_ACTIVISION);
    muhtari_cart_info out =
        expect_written("listed layout", buf, n, MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD);
    CHECK_EQ_U64(out.source, MUHTARI_SOURCE_HASH);
    CHECK_EQ_U64(out.board, MUHTARI_BOARD_ACTIVISION);
    images_swap_halves(buf, n);
    out = expect_written("swapped layout", buf, n, MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD);
    CHECK_EQ_U64(out.kind, MUHTARI_KIND_7800);
    CHECK_EQ_U64(out.source, MUHTARI_SOURCE_HASH);
    CHECK_EQ_U64(out.flags, MUHTARI_CART_HASH_MATCH);
    CHECK_EQ_U64(out.board, MUHTARI_BOARD_ACTIVISION);
    CHECK_EQ_U64(out.rom_size, body);
    /* Under a header that says Activision, the match is still found. */
    images_header(buf, 3u, "Activision", body, 0x0100u);
    images_seeded(buf + IMAGES_HEADER, body, IMAGES_SEED_ACTIVISION);
    images_swap_halves(buf + IMAGES_HEADER, body);
    out = expect_written("swapped layout, headered", buf, IMAGES_HEADER + body,
                         MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD);
    CHECK_EQ_U64(out.flags, MUHTARI_CART_HEADER | MUHTARI_CART_HASH_MATCH);
    CHECK_EQ_U64(out.board, MUHTARI_BOARD_ACTIVISION);

    /* 64 KiB whose swapped bytes are listed: no swapped pass, so the footer. */
    n = images_seeded(buf, 64u * IMAGES_KIB, IMAGES_SEED_SWAP_64K);
    out = expect_written("64 KiB", buf, n, MUHTARI_STATUS_OK);
    CHECK_EQ_U64(out.source, MUHTARI_SOURCE_FOOTER);
    CHECK_EQ_U64(out.board, MUHTARI_BOARD_SUPERGAME);
}

/* A 2600 list hit refuses an image whose footer the 7800 test passes. */
static void test_2600_prefix(void)
{
    size_t n = images_seeded(buf, 8u * IMAGES_KIB, IMAGES_SEED_2600);
    muhtari_cart_info out =
        expect_written("2600 prefix", buf, n, MUHTARI_STATUS_ERR_UNSUPPORTED_2600);
    check_2600("2600 prefix", &out, MUHTARI_SOURCE_HASH, MUHTARI_CART_HASH_MATCH, (uint32_t)n);
    buf[0] ^= 1u; /* no longer listed: a headerless 7800 image by its footer */
    out = expect_written("2600 prefix, changed", buf, n, MUHTARI_STATUS_OK);
    CHECK_EQ_U64(out.source, MUHTARI_SOURCE_FOOTER);
}

/* A match to an entry that is not a cartridge is invalid, out untouched. */
static void test_not_a_cartridge(void)
{
    size_t n = images_seeded(buf, 4u * IMAGES_KIB, IMAGES_SEED_NOT_CART);
    expect_untouched("not a cartridge", buf, n, MUHTARI_STATUS_ERR_INVALID_IMAGE);
    images_header(buf, 3u, "Not a cartridge", 4u * IMAGES_KIB, 0u);
    images_seeded(buf + IMAGES_HEADER, 4u * IMAGES_KIB, IMAGES_SEED_NOT_CART);
    expect_untouched("not a cartridge, headered", buf, IMAGES_HEADER + 4u * IMAGES_KIB,
                     MUHTARI_STATUS_ERR_INVALID_IMAGE);
}

/* A headerless match's ports: a light gun on port 1 for a light-gun entry,
   ProLine on both otherwise. */
static void test_headerless_ports(void)
{
    size_t n = images_seeded(buf, 16u * IMAGES_KIB, IMAGES_SEED_LIGHTGUN);
    muhtari_cart_info out = expect_written("light gun", buf, n, MUHTARI_STATUS_OK);
    CHECK_EQ_U64(out.source, MUHTARI_SOURCE_HASH);
    CHECK_EQ_U64(out.port1, MUHTARI_PORT_LIGHTGUN);
    CHECK_EQ_U64(out.port2, MUHTARI_PORT_PROLINE);
    CHECK_EQ_U64(out.region, MUHTARI_REGION_NTSC);
    n = images_seeded(buf, 16u * IMAGES_KIB, IMAGES_SEED_PROLINE);
    out = expect_written("ProLine", buf, n, MUHTARI_STATUS_OK);
    CHECK_EQ_U64(out.source, MUHTARI_SOURCE_HASH);
    CHECK_EQ_U64(out.port1, MUHTARI_PORT_PROLINE);
    CHECK_EQ_U64(out.port2, MUHTARI_PORT_PROLINE);
}

/* One image of each refusal class, built into buf, and the status the class
   gives. */
static size_t reject_2600(void)
{
    return images_2600(buf, 4u * IMAGES_KIB);
}

static size_t reject_2600_listed(void)
{
    return images_seeded(buf, 8u * IMAGES_KIB, IMAGES_SEED_2600);
}

static size_t reject_board(void)
{
    size_t n = images_a78(buf, 4u, 512u * IMAGES_KIB, 0x1000u, 512u * IMAGES_KIB);
    buf[64] = 4u; /* Souper as the version 4 mapper */
    return n;
}

static size_t reject_size_zero(void)
{
    return images_a78(buf, 3u, 0u, 0u, 32u * IMAGES_KIB);
}

static size_t reject_bad_magic(void)
{
    images_fill(buf, 1000u, 1000u);
    return 1000u;
}

static size_t reject_not_cart(void)
{
    return images_seeded(buf, 4u * IMAGES_KIB, IMAGES_SEED_NOT_CART);
}

static size_t reject_header_only(void)
{
    images_header(buf, 3u, "Short", 32u * IMAGES_KIB, 0u);
    return IMAGES_HEADER;
}

static size_t reject_sg_short(void)
{
    return images_a78(buf, 3u, 128u * IMAGES_KIB, 0x0002u, 40u * IMAGES_KIB);
}

static size_t reject_oversized(void)
{
    size_t n = images_a78(buf, 3u, 1024u * IMAGES_KIB, 0x0002u, 1024u * IMAGES_KIB);
    buf[n] = 0u;
    return n + 1u;
}

static const struct {
    const char *name;
    size_t (*build)(void);
    muhtari_status want;
} rejects[] = {
    {"2600", reject_2600, MUHTARI_STATUS_ERR_UNSUPPORTED_2600},
    {"2600, listed", reject_2600_listed, MUHTARI_STATUS_ERR_UNSUPPORTED_2600},
    {"unsupported board", reject_board, MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD},
    {"size field 0", reject_size_zero, MUHTARI_STATUS_ERR_INVALID_IMAGE},
    {"bad magic", reject_bad_magic, MUHTARI_STATUS_ERR_INVALID_IMAGE},
    {"not a cartridge", reject_not_cart, MUHTARI_STATUS_ERR_INVALID_IMAGE},
    {"header only", reject_header_only, MUHTARI_STATUS_ERR_TRUNCATED_IMAGE},
    {"SuperGame short", reject_sg_short, MUHTARI_STATUS_ERR_TRUNCATED_IMAGE},
    {"oversized", reject_oversized, MUHTARI_STATUS_ERR_UNSUPPORTED_SIZE},
};

static struct muhtari before;

/* Loads n bytes of buf into a fresh instance, with every allocation after
   create failing when fail is set. Checks the status, that the instance is
   byte-identical unless the load is OK, and the allocator's counts. */
static void expect_load(const char *what, size_t n, int fail, muhtari_status want)
{
    alloc_counts c;
    muhtari_config cfg = alloc_config(&c);
    muhtari *m = NULL;
    CHECK_EQ_U64(muhtari_create(&cfg, &m), MUHTARI_STATUS_OK);
    if (m == NULL) {
        return;
    }
    alloc_counts created = c;
    if (fail) {
        c.fail_from = c.allocs + 1u;
    }
    memcpy(&before, m, sizeof before);
    muhtari_status got = muhtari__cart_load(m, buf, n);
    if (got != want) {
        fprintf(stderr, "%s%s: expected status %u, got %u\n", what, fail ? ", failing" : "",
                (unsigned)want, (unsigned)got);
        check_failures++;
    }
    if (want == MUHTARI_STATUS_OK) {
        /* The positive control: one allocation of rom_size bytes. */
        muhtari_cart_info info = expect_written(what, buf, n, MUHTARI_STATUS_OK);
        CHECK_EQ_U64(c.allocs, created.allocs + 1u);
        CHECK_EQ_U64(c.alloc_size, info.rom_size);
        CHECK(memcmp(m->cart.rom, buf + IMAGES_HEADER, info.rom_size) == 0);
    } else {
        if (want != MUHTARI_STATUS_ERR_NO_MEMORY) {
            CHECK_EQ_U64(c.allocs, created.allocs);
        }
        if (memcmp(m, &before, sizeof before) != 0) {
            fprintf(stderr, "%s%s: the instance changed\n", what, fail ? ", failing" : "");
            check_failures++;
        }
    }
    CHECK_EQ_U64(c.frees, created.frees);
    muhtari_destroy(m);
    /* destroy frees the ROM, when there is one, and the instance. */
    CHECK_EQ_U64(c.frees - created.frees, want == MUHTARI_STATUS_OK ? 2u : 1u);
}

/* A refused image allocates nothing and leaves the instance byte-identical,
   with the status muhtari_probe gives; with an allocator that fails every
   call the status is still the refusal's, so validation runs first. An
   accepted linear image makes exactly one allocation, of rom_size bytes,
   and destroy frees it. */
static void test_no_alloc_on_reject(void)
{
    for (size_t i = 0; i < sizeof rejects / sizeof rejects[0]; i++) {
        size_t n = rejects[i].build();
        muhtari_cart_info out;
        prefill(&out, 0u);
        CHECK_EQ_U64(muhtari_probe(buf, n, &out), rejects[i].want);
        expect_load(rejects[i].name, n, 0, rejects[i].want);
        expect_load(rejects[i].name, n, 1, rejects[i].want);
    }
    size_t n = images_linear(buf, 3u, 16u * IMAGES_KIB);
    expect_load("linear", n, 0, MUHTARI_STATUS_OK);
    expect_load("linear", n, 1, MUHTARI_STATUS_ERR_NO_MEMORY);
}

/* The file named after the case: the stripped 7800basic sample. */
static const char *case_file;

/* The stripped 7800basic sample, read from the build tree, resolves through
   the test list: its SHA-1 1aa9559b74eab35d4c5443b56bc4dfd7a445ac4a is in
   neither MAME list, so only test_cartdb.c's entry can name it. */
static void test_test_list(void)
{
    FILE *f = case_file ? fopen(case_file, "rb") : NULL;
    if (!f) {
        fprintf(stderr, "identify.test_list: cannot open %s\n", case_file ? case_file : "(none)");
        check_failures++;
        return;
    }
    size_t n = fread(buf, 1, sizeof buf, f);
    fclose(f);
    CHECK_EQ_U64(n, 32u * IMAGES_KIB);
    muhtari_cart_info out = expect_written("test list", buf, n, MUHTARI_STATUS_OK);
    CHECK_EQ_U64(out.kind, MUHTARI_KIND_7800);
    CHECK_EQ_U64(out.source, MUHTARI_SOURCE_HASH);
    CHECK_EQ_U64(out.flags, MUHTARI_CART_HASH_MATCH);
    CHECK_EQ_U64(out.board, MUHTARI_BOARD_LINEAR);
    CHECK_EQ_U64(out.rom_size, 32u * IMAGES_KIB);
    CHECK_EQ_U64(out.region, MUHTARI_REGION_NTSC);
}

static const struct {
    const char *name;
    void (*run)(void);
} cases[] = {
    {"identify.2600_prefix", test_2600_prefix},
    {"identify.activision_swap", test_activision_swap},
    {"identify.headerless_ports", test_headerless_ports},
    {"identify.not_a_cartridge", test_not_a_cartridge},
    {"identify.slices", test_slices},
    {"identify.test_list", test_test_list},
    {"identify.hash_override", test_hash_override},
    {"load.no_alloc_on_reject", test_no_alloc_on_reject},
    {"probe.2600", test_2600},
    {"probe.accepted", test_accepted},
    {"probe.arguments", test_arguments},
    {"probe.malformed", test_malformed},
    {"probe.oversized", test_oversized},
    {"probe.truncated", test_truncated},
    {"probe.unsupported_board", test_unsupported_board},
    {"probe.warnings", test_warnings},
};

int main(int argc, char **argv)
{
    if (argc != 2 && argc != 3) {
        fprintf(stderr, "usage: muhtari-test-cart <case> [file]\n");
        return 2;
    }
    case_file = argc == 3 ? argv[2] : NULL;
    for (size_t i = 0; i < sizeof cases / sizeof cases[0]; i++) {
        if (strcmp(argv[1], cases[i].name) == 0) {
            cases[i].run();
            CHECK_DONE();
        }
    }
    fprintf(stderr, "muhtari-test-cart: unknown case %s\n", argv[1]);
    return 2;
}
