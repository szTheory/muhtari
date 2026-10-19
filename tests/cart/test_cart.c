/* muhtari_probe on images built in memory (images.h), one case per argv.
   The program links the core's objects rather than the archive. */
#include <string.h>

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

    /* A SuperGame header with POKEY at $4000 and 16 KiB of RAM over 128 KiB. */
    uint32_t body = 128u * IMAGES_KIB;
    images_header(buf, 3u, "SuperGame", body, 0x0007u);
    images_fill(buf + IMAGES_HEADER, body, 7u);
    muhtari_cart_info sg;
    prefill(&sg, 0u);
    CHECK_EQ_U64(muhtari_probe(buf, IMAGES_HEADER + body, &sg), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(sg.board, MUHTARI_BOARD_SUPERGAME);
    CHECK_EQ_U64(sg.ram_size, 16u * IMAGES_KIB);
    CHECK_EQ_U64(sg.sound, MUHTARI_SOUND_POKEY_4000);
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

static const struct {
    const char *name;
    void (*run)(void);
} cases[] = {
    {"probe.2600", test_2600},
    {"probe.accepted", test_accepted},
    {"probe.arguments", test_arguments},
    {"probe.malformed", test_malformed},
    {"probe.oversized", test_oversized},
    {"probe.truncated", test_truncated},
    {"probe.unsupported_board", test_unsupported_board},
};

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: muhtari-test-cart <case>\n");
        return 2;
    }
    for (size_t i = 0; i < sizeof cases / sizeof cases[0]; i++) {
        if (strcmp(argv[1], cases[i].name) == 0) {
            cases[i].run();
            CHECK_DONE();
        }
    }
    fprintf(stderr, "muhtari-test-cart: unknown case %s\n", argv[1]);
    return 2;
}
