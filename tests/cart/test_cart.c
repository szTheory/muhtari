/* muhtari_probe on images built in memory (images.h), one case per argv.
   The program links the core's objects rather than the archive. */
#include <string.h>

#include "muhtari.h"
#include "images.h"
#include "../check.h"

/* Room for the largest image probed, one byte past the cap. */
static uint8_t buf[MUHTARI_IMAGE_SIZE_MAX + 1u];

/* Fills *out with byte b and sets its size tag to the struct's size. */
static void prefill(muhtari_cart_info *out, uint8_t b)
{
    memset(out, b, sizeof *out);
    out->size = (uint32_t)sizeof *out;
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
}

static const struct {
    const char *name;
    void (*run)(void);
} cases[] = {
    {"probe.accepted", test_accepted},
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
