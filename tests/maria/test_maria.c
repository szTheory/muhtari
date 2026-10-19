/* MARIA's DMA burst through the shipped archive, one case per run:

     muhtari-test-maria <case>

   Each case creates an instance through the public API, writes a DLL and
   its display lists into system RAM, sets MARIA's registers and calls the
   burst directly, with no CPU. Every expected tick is written from the
   costs in A7800-MARIA section 4.2 (docs/hardware/maria.md, DMA cost), with
   the arithmetic beside it. Exit status: 0 when the case passes, 1 when it
   fails, 2 on a usage error or an unknown case. */
#include <stdio.h>
#include <string.h>

#include "../alloc.h"
#include "../check.h"
#include "internal.h"

/* The burst's start and LRC in every case: a line is 454 ticks
   (A7800-MARIA section 4.1). */
#define T0 1000u
#define LINE 454u
#define LRC (T0 + LINE)

/* Where the cases put things in system RAM. */
#define DLL_AT 0x1800u
#define DL_AT 0x1900u
#define DL2_AT 0x1980u

/* MARIA's registers, as indexes into reg (A7800-MARIA section 2.4). */
#define REG_DPPH 0x0Cu
#define REG_DPPL 0x10u

/* A cell no burst can write: cells hold 5 bits. */
#define SENTINEL 0xEEu

static alloc_counts counts;

static muhtari *create(void)
{
    muhtari_config cfg = alloc_config(&counts);
    muhtari *m = NULL;
    if (muhtari_create(&cfg, &m) != MUHTARI_STATUS_OK) {
        fprintf(stderr, "muhtari_create failed\n");
        return NULL;
    }
    memset(m->maria.line_ram, SENTINEL, sizeof m->maria.line_ram);
    return m;
}

/* Writes n bytes into system RAM at addr. */
static void poke(muhtari *m, uint16_t addr, const uint8_t *bytes, size_t n)
{
    memcpy(&m->ram[addr - MUHTARI__RAM_BASE], bytes, n);
}

/* Points DPPH and DPPL at the DLL and runs VBE, which reads zone 0's entry. */
static void start_frame(muhtari *m)
{
    m->maria.reg[REG_DPPH] = (uint8_t)(DLL_AT >> 8);
    m->maria.reg[REG_DPPL] = (uint8_t)(DLL_AT & 0xFFu);
    CHECK_EQ_U64(muhtari__maria_vbe(m, T0 - 100u), T0 - 100u + 7u);
}

/* The number of line-RAM cells, in both buffers, a burst changed. */
static unsigned cells_written(const muhtari *m)
{
    unsigned n = 0u;
    for (size_t b = 0u; b < 2u; b++) {
        for (size_t c = 0u; c < MUHTARI__LINE_CELLS; c++) {
            n += m->maria.line_ram[b][c] != SENTINEL;
        }
    }
    return n;
}

/* A DLL entry pointing at a DL whose first header is a terminator. On an
   ordinary line the terminator's four bytes are addressed at T0 + 2, + 4,
   + 6 and + 8 and HALTRST follows 1 tick later: T0 + 9. On the zone's last
   line the next DLL entry's three bytes follow at + 10, + 12 and + 14 and
   HALTRST comes with the third: T0 + 15 (A7800-MARIA section 4.2). */
static void empty_dl(void)
{
    static const uint8_t dll[] = {0x01u, DL_AT >> 8,  DL_AT & 0xFFu,
                                  0x00u, DL2_AT >> 8, DL2_AT & 0xFFu};
    static const uint8_t term[] = {0x00u, 0x00u, 0x00u, 0x00u};
    struct muhtari__line_stats st;
    muhtari *m = create();
    if (m == NULL) {
        check_failures++;
        return;
    }
    poke(m, DLL_AT, dll, sizeof dll);
    poke(m, DL_AT, term, sizeof term);
    poke(m, DL2_AT, term, sizeof term);
    start_frame(m);
    CHECK_EQ_U64(m->maria.offset, 1u);

    /* Ordinary line: section 4.2's T0 + 9 + D with D = 0. */
    CHECK_EQ_U64(muhtari__maria_dma_line(m, T0, LRC, 17u, &st), T0 + 9u);
    CHECK_EQ_U64(st.cost, 9u);
    CHECK_EQ_U64(st.flags, 0u);
    CHECK_EQ_U64(st.headers, 0u);
    CHECK_EQ_U64(st.objects, 0u);
    CHECK_EQ_U64(st.bytes, 0u);
    CHECK_EQ_U64(m->maria.offset, 0u);
    CHECK_EQ_U64(m->maria.stats[17].cost, 9u);

    /* Last line: section 4.2's T0 + 15 + D with D = 0, and zone 1's entry. */
    CHECK_EQ_U64(muhtari__maria_dma_line(m, T0 + LINE, LRC + LINE, 18u, &st), T0 + LINE + 15u);
    CHECK_EQ_U64(st.cost, 15u);
    CHECK_EQ_HEX(st.flags, MUHTARI__LINE_LAST);
    CHECK_EQ_HEX(m->maria.dl, DL2_AT);
    CHECK_EQ_HEX(m->maria.dll, DLL_AT + 6u);
    CHECK_EQ_U64(m->maria.stats[18].cost, 15u);

    /* A zero-object line writes no line-RAM cell. */
    CHECK_EQ_U64(cells_written(m), 0u);
    muhtari_destroy(m);
}

static const struct {
    const char *name;
    void (*run)(void);
} cases[] = {
    {"empty_dl", empty_dl},
};

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: muhtari-test-maria <case>\n");
        return 2;
    }
    for (size_t i = 0u; i < sizeof cases / sizeof cases[0]; i++) {
        if (strcmp(argv[1], cases[i].name) == 0) {
            cases[i].run();
            CHECK_DONE();
        }
    }
    fprintf(stderr, "muhtari-test-maria: unknown case %s\n", argv[1]);
    return 2;
}
