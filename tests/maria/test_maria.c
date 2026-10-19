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

/* MARIA's registers the cases set, as indexes into reg, and CTRL's bits:
   character width in bit 4, kangaroo mode in bit 2, the read mode in bits
   1-0 (A7800-MARIA section 3). */
#define REG_CHARBASE 0x14u
#define REG_CTRL 0x1Cu
#define CTRL_CW 0x10u
#define CTRL_KANGAROO 0x04u
#define RM_160 0x00u
#define RM_320BD 0x02u
#define RM_320AC 0x03u

/* A 5-byte header's flags in its second byte: WM and IND
   (A7800-MARIA section 2.2). */
#define WM 0x80u
#define IND 0x20u

/* Where character maps and graphics go. */
#define MAP_AT 0x2200u

/* A terminator, and the two header shapes of A7800-MARIA section 2.2: a
   4-byte header for graphics at hi:lo, palette pal, width 1-31 and position
   hpos; a 5-byte header with flags WM and IND and width 1-32. */
#define TERM 0x00u, 0x00u, 0x00u, 0x00u
#define H4(lo, hi, pal, width, hpos) (lo), (uint8_t)(((pal) << 5) | (32u - (width))), (hi), (hpos)
#define H5(lo, hi, flags, pal, width, hpos)                                                        \
    (lo), (uint8_t)(0x40u | (flags)), (hi), (uint8_t)(((pal) << 5) | ((32u - (width)) & 0x1Fu)),   \
        (hpos)

/* got must equal want; what names the quantity in the message. */
static void expect(const char *what, uint64_t got, uint64_t want)
{
    if (got != want) {
        fprintf(stderr, "%s: expected %llu got %llu\n", what, (unsigned long long)want,
                (unsigned long long)got);
        check_failures++;
    }
}

/* Fills n bytes of system RAM at addr with v. */
static void fill(muhtari *m, uint16_t addr, uint8_t v, size_t n)
{
    memset(&m->ram[addr - MUHTARI__RAM_BASE], v, n);
}

/* An instance whose zone 0 has DLL byte 0 entry and the n-byte DL dl, and
   whose zone 1 has DLL byte 0 next and a terminator-only DL; VBE has run. */
static muhtari *zone(uint8_t entry, uint8_t next, const uint8_t *dl, size_t n)
{
    const uint8_t dll[] = {entry, DL_AT >> 8, DL_AT & 0xFFu, next, DL2_AT >> 8, DL2_AT & 0xFFu};
    static const uint8_t term[] = {TERM};
    muhtari *m = create();
    if (m == NULL) {
        check_failures++;
        return NULL;
    }
    poke(m, DLL_AT, dll, sizeof dll);
    poke(m, DL_AT, dl, n);
    poke(m, DL2_AT, term, sizeof term);
    start_frame(m);
    return m;
}

/* Runs the burst at T0 as line 20 and checks HALTRST and the cost. */
static struct muhtari__line_stats burst(muhtari *m, const char *what, uint64_t haltrst)
{
    struct muhtari__line_stats st;
    expect(what, muhtari__maria_dma_line(m, T0, LRC, 20u, &st), haltrst);
    expect(what, st.cost, haltrst - T0);
    return st;
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

/* One header and one graphics byte. A 4-byte header costs 8 and a 5-byte
   header 10, and a graphics byte 3: T0 + 9 + 8 + 3 and T0 + 9 + 10 + 3
   (A7800-MARIA section 4.2). */
static void headers(void)
{
    static const uint8_t dl4[] = {H4(0x00u, 0x20u, 0u, 1u, 0u), TERM};
    static const uint8_t dl5[] = {H5(0x00u, 0x20u, 0u, 0u, 1u, 0u), TERM};
    struct muhtari__line_stats st;
    muhtari *m = zone(0x01u, 0x00u, dl4, sizeof dl4);
    if (m != NULL) {
        st = burst(m, "4-byte header", T0 + 9u + 8u + 3u);
        expect("4-byte headers", st.headers, 1u);
        expect("4-byte objects", st.objects, 1u);
        expect("4-byte bytes", st.bytes, 1u);
        muhtari_destroy(m);
    }
    m = zone(0x01u, 0x00u, dl5, sizeof dl5);
    if (m != NULL) {
        st = burst(m, "5-byte header", T0 + 9u + 10u + 3u);
        expect("5-byte headers", st.headers, 1u);
        expect("5-byte objects", st.objects, 1u);
        muhtari_destroy(m);
    }
}

/* Width is 32 - W, and a 5-byte header's W = 0 means 32 (A7800-MARIA
   section 2.2); each graphics byte costs 3 (section 4.2). */
static void widths(void)
{
    static const uint8_t w1[] = {H4(0x00u, 0x20u, 0u, 1u, 0u), TERM};
    static const uint8_t w8[] = {H4(0x00u, 0x20u, 0u, 8u, 0u), TERM};
    static const uint8_t w31[] = {H4(0x00u, 0x20u, 0u, 31u, 0u), TERM};
    static const uint8_t w32[] = {H5(0x00u, 0x20u, 0u, 0u, 32u, 0u), TERM};
    static const struct {
        const uint8_t *dl;
        size_t n;
        uint32_t width;
        uint64_t haltrst;
    } rows[] = {
        {w1, sizeof w1, 1u, T0 + 9u + 8u + 1u * 3u},     /* section 4.2 */
        {w8, sizeof w8, 8u, T0 + 9u + 8u + 8u * 3u},     /* section 4.2 */
        {w31, sizeof w31, 31u, T0 + 9u + 8u + 31u * 3u}, /* section 4.2 */
        {w32, sizeof w32, 32u, T0 + 9u + 10u + 32u * 3u} /* section 4.2 */
    };
    for (size_t i = 0u; i < sizeof rows / sizeof rows[0]; i++) {
        muhtari *m = zone(0x01u, 0x00u, rows[i].dl, rows[i].n);
        char what[32];
        if (m == NULL) {
            continue;
        }
        snprintf(what, sizeof what, "width %u", (unsigned)rows[i].width);
        expect(what, burst(m, what, rows[i].haltrst).bytes, rows[i].width);
        muhtari_destroy(m);
    }
}

/* An indirect object of two characters. A character costs a map read of 3
   and 3 per graphics byte: 6 with 1-byte characters, 9 with 2-byte ones
   (A7800-MARIA section 4.2). The map is read at hi:lo with no OFFSET and
   the graphics at CHARBASE:c plus OFFSET (section 2.3). */
static void indirect(void)
{
    static const uint8_t dl[] = {H5(0x00u, MAP_AT >> 8, IND, 0u, 2u, 0u), TERM};
    static const uint8_t map[] = {0x00u, 0x01u};
    for (uint8_t cw = 0u; cw < 2u; cw++) {
        muhtari *m = zone(0x01u, 0x00u, dl, sizeof dl);
        struct muhtari__line_stats st;
        if (m == NULL) {
            continue;
        }
        poke(m, MAP_AT, map, sizeof map);
        fill(m, 0x2400u, 0xFFu, 4u); /* CHARBASE $23 plus OFFSET 1 */
        m->maria.reg[REG_CHARBASE] = 0x23u;
        m->maria.reg[REG_CTRL] = cw != 0u ? CTRL_CW : 0u;
        if (cw == 0u) {
            st = burst(m, "1-byte characters", T0 + 9u + 10u + 2u * 6u); /* section 4.2 */
            expect("1-byte bytes", st.bytes, 2u + 2u);
            expect("1-byte cells", cells_written(m), 2u * 4u);
            expect("1-byte feature", m->maria.features & MUHTARI__FEAT_IND1, MUHTARI__FEAT_IND1);
        } else {
            st = burst(m, "2-byte characters", T0 + 9u + 10u + 2u * 9u); /* section 4.2 */
            expect("2-byte bytes", st.bytes, 2u + 4u);
            expect("2-byte cells", cells_written(m), 2u * 8u);
            expect("2-byte feature", m->maria.features & MUHTARI__FEAT_IND2, MUHTARI__FEAT_IND2);
        }
        expect("indirect objects", st.objects, 1u);
        muhtari_destroy(m);
    }
}

/* Holes: a graphics address with A15 set and A12 under H16 or A11 under H8,
   tested after OFFSET is added (A7800-MARIA section 2.3). A holey slot
   costs one slot of the object's kind, 3, 6 or 9, draws nothing and ends
   the object (section 4.4). */
static void holes(void)
{
    /* $8F00 + OFFSET 1 = $9000: A15 and A12, a hole under H16. */
    static const uint8_t direct[] = {H4(0x00u, 0x8Fu, 0u, 4u, 0u), TERM};
    /* $8000 + OFFSET 8 = $8800: A15 and A11, a hole under H8 only with
       OFFSET added. */
    static const uint8_t h8[] = {H4(0x00u, 0x80u, 0u, 4u, 0u), TERM};
    /* $8EFE + OFFSET 1: $8FFE and $8FFF are drawn, $9000 is a hole. */
    static const uint8_t middle[] = {H4(0xFEu, 0x8Eu, 0u, 4u, 0u), TERM};
    static const uint8_t ind[] = {H5(0x00u, MAP_AT >> 8, IND, 0u, 3u, 0u), TERM};
    static const uint8_t map[] = {0x00u, 0x01u, 0x02u};
    struct muhtari__line_stats st;
    muhtari *m = zone(0x41u, 0x00u, direct, sizeof direct);
    if (m != NULL) {
        st = burst(m, "direct hole", T0 + 9u + 8u + 3u); /* header + 3, section 4.4 */
        expect("direct hole bytes", st.bytes, 0u);
        expect("direct hole objects", st.objects, 1u);
        expect("direct hole cells", cells_written(m), 0u);
        expect("holey 16 feature", m->maria.features & MUHTARI__FEAT_HOLEY16,
               MUHTARI__FEAT_HOLEY16);
        muhtari_destroy(m);
    }
    m = zone(0x01u, 0x00u, direct, sizeof direct);
    if (m != NULL) {
        (void)burst(m, "no H16", T0 + 9u + 8u + 4u * 3u); /* section 4.2 */
        muhtari_destroy(m);
    }
    m = zone(0x28u, 0x00u, h8, sizeof h8);
    if (m != NULL) {
        (void)burst(m, "H8 hole after OFFSET", T0 + 9u + 8u + 3u); /* section 4.4 */
        expect("holey 8 feature", m->maria.features & MUHTARI__FEAT_HOLEY8, MUHTARI__FEAT_HOLEY8);
        muhtari_destroy(m);
    }
    m = zone(0x21u, 0x00u, h8, sizeof h8);
    if (m != NULL) {
        (void)burst(m, "H8 at $8100", T0 + 9u + 8u + 4u * 3u); /* section 4.2 */
        muhtari_destroy(m);
    }
    m = zone(0x41u, 0x00u, middle, sizeof middle);
    if (m != NULL) {
        st = burst(m, "hole mid-object", T0 + 9u + 8u + 2u * 3u + 3u); /* section 4.4 */
        expect("hole mid-object bytes", st.bytes, 2u);
        muhtari_destroy(m);
    }
    for (uint8_t cw = 0u; cw < 2u; cw++) {
        m = zone(0x41u, 0x00u, ind, sizeof ind);
        if (m == NULL) {
            continue;
        }
        poke(m, MAP_AT, map, sizeof map);
        m->maria.reg[REG_CHARBASE] = 0x8Fu; /* $8F00 + OFFSET 1 = $9000 */
        m->maria.reg[REG_CTRL] = cw != 0u ? CTRL_CW : 0u;
        /* The first character's slot, 6 or 9, then the object ends
           (section 4.4). */
        st = burst(m, cw != 0u ? "2-byte indirect hole" : "1-byte indirect hole",
                   T0 + 9u + 10u + (cw != 0u ? 9u : 6u));
        expect("indirect hole cells", cells_written(m), 0u);
        expect("indirect hole objects", st.objects, 1u);
        muhtari_destroy(m);
    }
}

/* A header ends the DL when (b1 & 0x5F) == 0: bits 7 and 5 are ignored. A
   b1 with bit 6 set and bits 4-0 clear is a 5-byte header (A7800-MARIA
   section 2.2). A 5-byte header of width 1 costs 10 + 3 direct or 10 + 6
   indirect (section 4.2), and loads WM from bit 7. */
static void terminators(void)
{
    static const uint8_t ends[] = {0x00u, 0x20u, 0x80u, 0xA0u};
    static const uint8_t fives[] = {0x40u, 0x60u, 0xC0u, 0xE0u};
    for (size_t i = 0u; i < sizeof ends; i++) {
        const uint8_t dl[] = {0x00u, ends[i], 0x00u, 0x00u};
        muhtari *m = zone(0x01u, 0x00u, dl, sizeof dl);
        char what[32];
        if (m == NULL) {
            continue;
        }
        snprintf(what, sizeof what, "terminator $%02X", (unsigned)ends[i]);
        expect(what, burst(m, what, T0 + 9u).headers, 0u); /* section 4.2 */
        muhtari_destroy(m);
    }
    for (size_t i = 0u; i < sizeof fives; i++) {
        const uint8_t dl[] = {0x00u, fives[i], MAP_AT >> 8, 0x1Fu, 0x00u, TERM};
        int ind = (fives[i] & IND) != 0u;
        muhtari *m = zone(0x01u, 0x00u, dl, sizeof dl);
        char what[32];
        if (m == NULL) {
            continue;
        }
        m->maria.write_mode = (uint8_t)((fives[i] & WM) == 0u);
        snprintf(what, sizeof what, "5-byte header $%02X", (unsigned)fives[i]);
        expect(what, burst(m, what, T0 + 9u + 10u + (ind ? 6u : 3u)).headers, 1u); /* 4.2 */
        expect("write mode loaded", m->maria.write_mode, (fives[i] & WM) != 0u);
        muhtari_destroy(m);
    }
}

/* The same one-object DL costs 6 more ticks on the zone's last line, which
   reads the next DLL entry: T0 + 15 + 11 against T0 + 9 + 11 (A7800-MARIA
   section 4.2). The entry's DLI bit fires on this line (section 2.1). */
static void last_line(void)
{
    static const uint8_t dl[] = {H4(0x00u, 0x20u, 0u, 1u, 0u), TERM};
    struct muhtari__line_stats st;
    muhtari *m = zone(0x01u, 0x82u, dl, sizeof dl);
    if (m == NULL) {
        return;
    }
    st = burst(m, "ordinary line", T0 + 9u + 8u + 3u); /* section 4.2 */
    expect("ordinary flags", st.flags, 0u);
    expect("ordinary line OFFSET", m->maria.offset, 0u);
    expect("last line", muhtari__maria_dma_line(m, T0 + LINE, LRC + LINE, 21u, &st),
           T0 + LINE + 15u + 8u + 3u); /* section 4.2 */
    expect("last line cost", st.cost, 15u + 8u + 3u);
    expect("last line flags", st.flags, MUHTARI__LINE_LAST | MUHTARI__LINE_DLI);
    expect("next DL", m->maria.dl, DL2_AT);
    expect("next OFFSET", m->maria.offset, 2u);
    expect("next entry", m->maria.dll, DLL_AT + 6u);
    expect("DLI feature", m->maria.features & MUHTARI__FEAT_DLI, MUHTARI__FEAT_DLI);
    muhtari_destroy(m);
}

/* Five 4-byte objects of width 31 overrun the line. Object k's header is
   addressed at T0 + 2 + 101k to + 8 + 101k and its graphics at T0 + 10 +
   101k + 3j, so object 3's last fetch is at T0 + 403 and object 4's
   graphics byte j at T0 + 414 + 3j: bytes 0-13 come before LRC = T0 + 454
   and byte 14, at T0 + 456, is dropped. HALTRST is LRC + 3 on an ordinary
   line and LRC + 9 on a last line, against T0 + 9 + 5 * 101 = T0 + 514
   without the abort (A7800-MARIA section 4.6). */
static void abort_case(void)
{
    for (uint8_t offset = 2u; offset-- > 0u;) {
        uint8_t dl[5u * 4u + 4u];
        struct muhtari__line_stats st;
        muhtari *m;
        for (size_t k = 0u; k < 4u; k++) { /* transparent: graphics $00 */
            const uint8_t h[] = {H4(0x00u, (uint8_t)(0x20u - offset), 0u, 31u, 0u)};
            memcpy(&dl[4u * k], h, sizeof h);
        }
        {
            const uint8_t h[] = {H4(0x00u, (uint8_t)(0x21u - offset), 5u, 31u, 0u), TERM};
            memcpy(&dl[16], h, sizeof h);
        }
        m = zone(offset, 0x00u, dl, sizeof dl);
        if (m == NULL) {
            break;
        }
        fill(m, 0x2000u, 0x00u, 0x100u);
        fill(m, 0x2100u, 0xFFu, 0x100u);
        if (offset == 1u) {
            st = burst(m, "abort, ordinary line", LRC + 3u); /* section 4.6 */
            expect("abort flags", st.flags, MUHTARI__LINE_ABORTED);
            expect("abort OFFSET", m->maria.offset, 0u);
        } else {
            st = burst(m, "abort, last line", LRC + 9u); /* section 4.6 */
            expect("abort last flags", st.flags, MUHTARI__LINE_LAST | MUHTARI__LINE_ABORTED);
            expect("abort next DL", m->maria.dl, DL2_AT);
        }
        expect("abort headers", st.headers, 5u);
        expect("abort objects", st.objects, 4u);
        expect("abort bytes", st.bytes, 4u * 31u + 14u);
        /* Byte 13's four cells end at cell 55; byte 14 was never fetched. */
        expect("abort cells", cells_written(m), 14u * 4u);
        expect("cell 55", m->maria.line_ram[m->maria.write_buf][55], (5u << 2) | 3u);
        expect("cell 56", m->maria.line_ram[m->maria.write_buf][56], SENTINEL);
        muhtari_destroy(m);
    }
}

/* The cells a two-byte object of palette 5 writes from hpos 10, bytes $E4
   and $0C, in write mode wm with kangaroo mode k. In write mode 0 cell k
   holds P2 P1 P0 and data bits 7-2k and 6-2k; in write mode 1 cell 0 holds
   P2 D3 D2 and D7 D6, cell 1 P2 D1 D0 and D5 D4. A cell is written when its
   data pair is not 00, or in kangaroo mode (A7800-MARIA section 3). The
   read mode changes none of it. */
static void ram_mode(const char *mode, uint8_t wm, uint8_t rm, uint8_t k, const uint8_t want[8])
{
    const uint8_t dl[] = {H5(0x00u, 0x20u, wm != 0u ? WM : 0u, 5u, 2u, 10u), TERM};
    static const uint8_t gfx[] = {0xE4u, 0x0Cu};
    muhtari *m = zone(0x01u, 0x00u, dl, sizeof dl);
    char what[48];
    if (m == NULL) {
        return;
    }
    poke(m, 0x2100u, gfx, sizeof gfx);
    m->maria.reg[REG_CTRL] = (uint8_t)(rm | (k != 0u ? CTRL_KANGAROO : 0u));
    (void)burst(m, mode, T0 + 9u + 10u + 2u * 3u); /* section 4.2 */
    for (size_t c = 0u; c < 8u; c++) {
        snprintf(what, sizeof what, "%s%s cell %u", mode, k != 0u ? " kangaroo" : "",
                 (unsigned)(10u + c));
        expect(what, m->maria.line_ram[m->maria.write_buf][10u + c], want[c]);
    }
    muhtari_destroy(m);
}

#define S SENTINEL
static void line_ram(void)
{
    /* $E4 = 11 10 01 00 and $0C = 00 00 11 00, palette 5. */
    static const uint8_t wm0[8] = {0x17u, 0x16u, 0x15u, S, S, S, 0x17u, S};
    static const uint8_t wm0k[8] = {0x17u, 0x16u, 0x15u, 0x14u, 0x14u, 0x14u, 0x17u, 0x14u};
    /* $E4: cell 0 P2 D3 D2 = 101, D7 D6 = 11; cell 1 P2 D1 D0 = 100,
       D5 D4 = 10. $0C: both pairs 00, so nothing though D3 D2 = 11. */
    static const uint8_t wm1[8] = {0x17u, 0x12u, S, S, S, S, S, S};
    static const uint8_t wm1k[8] = {0x17u, 0x12u, 0x1Cu, 0x10u, S, S, S, S};
    static const struct {
        const char *mode;
        uint8_t wm, rm;
    } modes[] = {{"160A", 0u, RM_160},   {"160B", 1u, RM_160},   {"320A", 0u, RM_320AC},
                 {"320B", 1u, RM_320BD}, {"320C", 1u, RM_320AC}, {"320D", 0u, RM_320BD}};
    for (size_t i = 0u; i < sizeof modes / sizeof modes[0]; i++) {
        ram_mode(modes[i].mode, modes[i].wm, modes[i].rm, 0u, modes[i].wm != 0u ? wm1 : wm0);
        ram_mode(modes[i].mode, modes[i].wm, modes[i].rm, 1u, modes[i].wm != 0u ? wm1k : wm0k);
    }
    {
        /* hpos wraps at 8 bits and cells 160-255 are scratch: a byte at hpos
           254 in write mode 0 lands only its last two cells, at 0 and 1
           (A7800-MARIA section 3). */
        static const uint8_t dl[] = {H4(0x00u, 0x20u, 5u, 1u, 254u), TERM};
        muhtari *m = zone(0x01u, 0x00u, dl, sizeof dl);
        if (m != NULL) {
            fill(m, 0x2100u, 0xE4u, 1u);
            (void)burst(m, "wrap", T0 + 9u + 8u + 3u); /* section 4.2 */
            expect("wrap cell 0", m->maria.line_ram[m->maria.write_buf][0], 0x15u);
            expect("wrap cells", cells_written(m), 1u);
            muhtari_destroy(m);
        }
    }
    {
        /* WM persists and 4-byte headers inherit it (A7800-MARIA section
           2.2): the 4-byte object after a 5-byte WM = 1 header draws in
           write mode 1. Cost T0 + 9 + 13 + 11 (section 4.2). */
        static const uint8_t dl[] = {H5(0x00u, 0x20u, WM, 5u, 1u, 10u),
                                     H4(0x01u, 0x20u, 5u, 1u, 20u), TERM};
        muhtari *m = zone(0x01u, 0x00u, dl, sizeof dl);
        if (m != NULL) {
            fill(m, 0x2100u, 0xE4u, 2u);
            (void)burst(m, "inherit", T0 + 9u + 13u + 11u);
            expect("inherit cell 20", m->maria.line_ram[m->maria.write_buf][20], 0x17u);
            expect("inherit cell 21", m->maria.line_ram[m->maria.write_buf][21], 0x12u);
            expect("inherit cells", cells_written(m), 4u);
            muhtari_destroy(m);
        }
    }
}
#undef S

/* A mixed DL: a 4-byte direct object of width 2 (8 + 2 * 3), a 5-byte
   indirect object of two 1-byte characters (10 + 2 * 6) and a 5-byte direct
   object of width 3 (10 + 3 * 3). D = 14 + 22 + 19 = 55, so HALTRST =
   T0 + 9 + 55 (A7800-MARIA section 4.2); the bytes fetched are 2 + 2 map
   + 2 character + 3. */
static void stats(void)
{
    static const uint8_t dl[] = {H4(0x00u, 0x1Fu, 0u, 2u, 0u),
                                 H5(0x00u, MAP_AT >> 8, IND, 0u, 2u, 40u),
                                 H5(0x00u, 0x1Fu, WM, 0u, 3u, 80u), TERM};
    static const uint8_t map[] = {0x00u, 0x01u};
    struct muhtari__line_stats st;
    muhtari *m = zone(0x01u, 0x00u, dl, sizeof dl);
    if (m == NULL) {
        return;
    }
    poke(m, MAP_AT, map, sizeof map);
    m->maria.reg[REG_CHARBASE] = 0x23u;
    st = burst(m, "mixed DL", T0 + 9u + 55u); /* section 4.2 */
    expect("mixed headers", st.headers, 3u);
    expect("mixed objects", st.objects, 3u);
    expect("mixed bytes", st.bytes, 2u + 2u + 2u + 3u);
    expect("mixed flags", st.flags, 0u);
    expect("stored cost", m->maria.stats[20].cost, 9u + 55u);
    expect("stored bytes", m->maria.stats[20].bytes, 9u);
    expect("write mode after", m->maria.write_mode, 1u);
    muhtari_destroy(m);
}

/* Four one-line zones, each line the zone's last (A7800-MARIA section 4.2):
   each feature bit sets on the first line that uses it and stays set. */
static void feature_mask(void)
{
    static const uint8_t dll[] = {0x00u, 0x19u, 0x00u, 0x40u, 0x19u, 0x20u,
                                  0xA0u, 0x19u, 0x40u, 0x00u, 0x19u, 0x60u};
    static const uint8_t dla[] = {H4(0x00u, 0x20u, 0u, 1u, 0u), TERM};
    static const uint8_t dlb[] = {H5(0x00u, MAP_AT >> 8, WM | IND, 0u, 1u, 0u), TERM};
    static const uint8_t dlc[] = {H5(0x00u, MAP_AT >> 8, IND, 0u, 1u, 0u), TERM};
    static const uint8_t dld[] = {TERM};
    static const uint8_t ctrl[] = {RM_160, RM_320BD | CTRL_KANGAROO | CTRL_CW, RM_320AC, 0x01u};
    static const uint32_t want[] = {
        MUHTARI__FEAT_RM0,
        MUHTARI__FEAT_RM0 | MUHTARI__FEAT_RM2 | MUHTARI__FEAT_KANGAROO | MUHTARI__FEAT_WRITE_MODE |
            MUHTARI__FEAT_IND2 | MUHTARI__FEAT_HOLEY16 | MUHTARI__FEAT_DLI,
        MUHTARI__FEAT_RM0 | MUHTARI__FEAT_RM2 | MUHTARI__FEAT_KANGAROO | MUHTARI__FEAT_WRITE_MODE |
            MUHTARI__FEAT_IND2 | MUHTARI__FEAT_HOLEY16 | MUHTARI__FEAT_DLI | MUHTARI__FEAT_RM3 |
            MUHTARI__FEAT_IND1 | MUHTARI__FEAT_HOLEY8,
        MUHTARI__FEAT_RM0 | MUHTARI__FEAT_RM2 | MUHTARI__FEAT_KANGAROO | MUHTARI__FEAT_WRITE_MODE |
            MUHTARI__FEAT_IND2 | MUHTARI__FEAT_HOLEY16 | MUHTARI__FEAT_DLI | MUHTARI__FEAT_RM3 |
            MUHTARI__FEAT_IND1 | MUHTARI__FEAT_HOLEY8};
    struct muhtari__line_stats st;
    muhtari *m = create();
    if (m == NULL) {
        check_failures++;
        return;
    }
    poke(m, DLL_AT, dll, sizeof dll);
    poke(m, 0x1900u, dla, sizeof dla);
    poke(m, 0x1920u, dlb, sizeof dlb);
    poke(m, 0x1940u, dlc, sizeof dlc);
    poke(m, 0x1960u, dld, sizeof dld);
    m->maria.reg[REG_CHARBASE] = 0x23u;
    start_frame(m);
    expect("features at power-on", m->maria.features, 0u);
    for (uint32_t i = 0u; i < 4u; i++) {
        char what[24];
        m->maria.reg[REG_CTRL] = ctrl[i];
        (void)muhtari__maria_dma_line(m, T0 + i * LINE, LRC + i * LINE, 20u + i, &st);
        snprintf(what, sizeof what, "features after line %u", (unsigned)(i + 1u));
        expect(what, m->maria.features, want[i]);
    }
    muhtari_destroy(m);
}

static const struct {
    const char *name;
    void (*run)(void);
} cases[] = {
    {"empty_dl", empty_dl},
    {"headers", headers},
    {"widths", widths},
    {"indirect", indirect},
    {"holes", holes},
    {"terminators", terminators},
    {"last_line", last_line},
    {"abort", abort_case},
    {"line_ram", line_ram},
    {"stats", stats},
    {"feature_mask", feature_mask},
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
