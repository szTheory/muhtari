/* MARIA: the registers, the display-list walk and its DMA burst, line RAM
   and the per-line statistics (docs/hardware/maria.md). */
#include "internal.h"

/* The DMA schedule in ticks (A7800-MARIA section 4.8; docs/hardware/maria.md,
   DMA cost). The PLA owns the bus from T0 and addresses its first byte 2
   ticks later; each DL or DLL byte takes 2 ticks, each graphics byte 3 and
   each character map read 3. A terminator is a full 4-byte read. After the
   last DL byte, HALTRST follows 1 tick later on an ordinary line, or 7 on a
   zone's last line, which reads the next DLL entry first. An abort at LRC
   ends the burst with HALTRST at LRC + 3, or LRC + 9 on a last line. */
#define MARIA_PLA_START 2u
#define MARIA_DL_BYTE 2u
#define MARIA_GFX_BYTE 3u
#define MARIA_CHARMAP 3u
#define MARIA_TERMINATOR 8u
#define MARIA_TAIL_OTHER 1u
#define MARIA_TAIL_LAST 7u
#define MARIA_ABORT_TAIL_OTHER 3u
#define MARIA_ABORT_TAIL_LAST 9u
#define MARIA_LINE_TICKS 454u
#define MARIA_DMA_REQ_T 28u

/* The bytes of a DLL entry, and VBE's length: the PLA start and the entry's
   three bytes (A7800-MARIA section 4.2). */
#define MARIA_DLL_ENTRY 3u
#define MARIA_VBE_LEN (MARIA_PLA_START + MARIA_DLL_ENTRY * MARIA_DL_BYTE - 1u)

/* The registers MARIA's DMA reads, as indexes into reg (A7800-MARIA
   section 2.4). */
#define MARIA_DPPH 0x0Cu
#define MARIA_DPPL 0x10u

/* A DLL entry's byte 0: OFFSET in bits 3-0 (A7800-MARIA section 2.1). */
#define MARIA_ENTRY_OFFSET 0x0Fu
#define MARIA_ENTRY_DLI 0x80u

/* A header is a terminator when (b1 & 0x5F) == 0 (A7800-MARIA section 2.2). */
#define MARIA_TERM_MASK 0x5Fu

/* One DMA read at addr. RAM answers, $1800-$27FF and its page 0 and 1
   mirrors by the rule src/bus.c's ram_index applies. TIA, MARIA and RIOT
   selects are gated off while MARIA drives the bus, so those reads return
   the floating byte, the previous fetch's, with no side effect; so does
   cartridge space, which the board does not answer yet (A7800-MARIA
   section 4.7). Every fetch puts its byte on the bus. */
static uint8_t fetch(muhtari *m, uint16_t addr)
{
    uint8_t v = m->bus.open_bus;
    if (addr < 0x0200u && (addr & 0xFFu) >= 0x40u) {
        v = m->ram[(uint16_t)(addr + 0x2000u - MUHTARI__RAM_BASE)];
    } else if (addr >= MUHTARI__RAM_BASE && addr < MUHTARI__RAM_BASE + MUHTARI__RAM_SIZE) {
        v = m->ram[(uint16_t)(addr - MUHTARI__RAM_BASE)];
    }
    m->bus.open_bus = v;
    return v;
}

/* Reads the DLL entry at dll, its bytes addressed at t, t + 2 and t + 4,
   and makes it the zone's: byte 0 sets OFFSET, H16, H8 and DLI, bytes 1
   and 2 the DL address, high byte first (A7800-MARIA section 2.1). */
static void read_entry(muhtari *m)
{
    struct muhtari__maria *ma = &m->maria;
    uint8_t b0 = fetch(m, ma->dll);
    uint8_t hi = fetch(m, (uint16_t)(ma->dll + 1u));
    uint8_t lo = fetch(m, (uint16_t)(ma->dll + 2u));
    ma->dll = (uint16_t)(ma->dll + MARIA_DLL_ENTRY);
    ma->entry = b0;
    ma->offset = (uint8_t)(b0 & MARIA_ENTRY_OFFSET);
    ma->dl = (uint16_t)(((uint16_t)hi << 8) | lo);
}

uint64_t muhtari__maria_vbe(muhtari *m, uint64_t t0)
{
    struct muhtari__maria *ma = &m->maria;
    ma->dll = (uint16_t)(((uint16_t)ma->reg[MARIA_DPPH] << 8) | ma->reg[MARIA_DPPL]);
    read_entry(m);
    return t0 + MARIA_VBE_LEN;
}

uint64_t muhtari__maria_dma_line(muhtari *m, uint64_t t0, uint64_t lrc, uint32_t line,
                                 struct muhtari__line_stats *st)
{
    struct muhtari__maria *ma = &m->maria;
    uint16_t p = ma->dl;
    uint64_t t = t0 + MARIA_PLA_START; /* the next fetch's address tick */
    uint64_t end;
    int last = ma->offset == 0u;

    st->cost = 0u;
    st->bytes = 0u;
    st->headers = 0u;
    st->objects = 0u;
    st->flags = 0u;
    if (lrc > t0 + MARIA_LINE_TICKS) {
        lrc = t0 + MARIA_LINE_TICKS;
    }

    /* The terminator: all four bytes are read (A7800-MARIA section 2.2). */
    (void)fetch(m, p);
    (void)fetch(m, (uint16_t)(p + 1u));
    (void)fetch(m, (uint16_t)(p + 2u));
    (void)fetch(m, (uint16_t)(p + 3u));
    t += MARIA_TERMINATOR;
    end = t - MARIA_DL_BYTE;

    if (last) {
        st->flags |= MUHTARI__LINE_LAST;
        read_entry(m);
        if ((ma->entry & MARIA_ENTRY_DLI) != 0u) {
            st->flags |= MUHTARI__LINE_DLI;
            ma->features |= MUHTARI__FEAT_DLI;
        }
        end += MARIA_TAIL_LAST;
    } else {
        ma->offset--;
        end += MARIA_TAIL_OTHER;
    }
    st->cost = (uint16_t)(end - t0);
    if (line < MUHTARI__LINES_NTSC) {
        ma->stats[line] = *st;
    }
    return end;
}
