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
_Static_assert(MARIA_TERMINATOR == 4u * MARIA_DL_BYTE, "a terminator is a full 4-byte read");

/* The bytes of a DLL entry, and VBE's length: the PLA start and the entry's
   three bytes (A7800-MARIA section 4.2). */
#define MARIA_DLL_ENTRY 3u
#define MARIA_VBE_LEN (MARIA_PLA_START + MARIA_DLL_ENTRY * MARIA_DL_BYTE - 1u)

/* The registers MARIA's DMA reads, as indexes into reg (A7800-MARIA
   section 2.4), and CTRL's DMA-side bits: the DMA mode in bits 6-5, on only
   at 10, character width in bit 4, kangaroo mode in bit 2 and the read mode
   in bits 1-0 (section 3; modes 00 and 01, test modes, count as off,
   Choice 3.B). */
#define MARIA_DPPH 0x0Cu
#define MARIA_DPPL 0x10u
#define MARIA_CHARBASE 0x14u
#define MARIA_CTRL 0x1Cu
#define MARIA_CTRL_DM 0x60u
#define MARIA_CTRL_CW 0x10u
#define MARIA_CTRL_KANGAROO 0x04u
#define MARIA_CTRL_RM 0x03u
#define MARIA_DM_ON 0x40u

/* A DLL entry's byte 0: DLI in bit 7, H16 in bit 6, H8 in bit 5 and OFFSET
   in bits 3-0 (A7800-MARIA section 2.1). */
#define MARIA_ENTRY_DLI 0x80u
#define MARIA_ENTRY_H16 0x40u
#define MARIA_ENTRY_H8 0x20u
#define MARIA_ENTRY_OFFSET 0x0Fu

/* A header's second byte (A7800-MARIA section 2.2): a 4-byte header when
   its width field is not 0; else a 5-byte header when bit 6 is set, with
   WM in bit 7 and IND in bit 5; else the terminator, (b1 & 0x5F) == 0. */
#define MARIA_HDR_WIDTH 0x1Fu
#define MARIA_HDR_FIVE 0x40u
#define MARIA_HDR_WM 0x80u
#define MARIA_HDR_IND 0x20u
#define MARIA_TERM_MASK 0x5Fu
#define MARIA_WIDTH_MAX 32u

/* The hole test's address bits: A15, and A12 under H16 or A11 under H8
   (A7800-MARIA section 2.3). */
#define MARIA_A15 0x8000u
#define MARIA_A12 0x1000u
#define MARIA_A11 0x0800u

/* One DMA read at addr. RAM answers, $1800-$27FF and its page 0 and 1
   mirrors by the rule src/bus.c's ram_index applies. TIA, MARIA and RIOT
   selects are gated off while MARIA drives the bus, so those reads return
   the floating byte, the previous fetch's, with no side effect. Cartridge
   space, from $0400 up outside the RIOT's RAM and system RAM, goes to the
   board, which answers MARIA as it answers the CPU (A7800-MARIA,
   section 4.7). Every fetch puts its byte on the bus. */
static uint8_t fetch(muhtari *m, uint16_t addr)
{
    uint8_t v = m->bus.open_bus;
    if (addr < 0x0200u && (addr & 0xFFu) >= 0x40u) {
        v = m->ram[(uint16_t)(addr + 0x2000u - MUHTARI__RAM_BASE)];
    } else if (addr >= MUHTARI__RAM_BASE && addr < MUHTARI__RAM_BASE + MUHTARI__RAM_SIZE) {
        v = m->ram[(uint16_t)(addr - MUHTARI__RAM_BASE)];
    } else if (addr >= 0x0400u && !(addr < 0x0600u && (addr & 0x80u) != 0u)) {
        v = muhtari__cart_read(m, addr, 1);
    }
    m->bus.open_bus = v;
    return v;
}

/* Reads the DLL entry at dll and makes it the zone's: byte 0 sets OFFSET,
   H16, H8 and DLI, bytes 1 and 2 the DL address, high byte first
   (A7800-MARIA section 2.1). */
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

/* One burst in progress: t is the next fetch's address tick, and a fetch at
   or after lrc is dropped and ends the burst (A7800-MARIA section 4.6). p
   walks the DL; hpos is the object's next line-RAM cell. ctrl is CTRL as
   the PLA start samples it (section 5). */
struct walk {
    muhtari *m;
    struct muhtari__line_stats *st;
    uint64_t t;
    uint64_t lrc;
    uint16_t p;
    uint8_t ctrl;
    uint8_t hpos;
    uint8_t aborted;
};

/* Whether the fetch addressed at w->t runs: one at or after LRC is dropped
   and the burst ends (A7800-MARIA section 4.6). */
static int runs(struct walk *w)
{
    if (w->t >= w->lrc) {
        w->aborted = 1u;
    }
    return !w->aborted;
}

/* The next DL byte into *v; 0 when the abort dropped it. */
static int dl_byte(struct walk *w, uint8_t *v)
{
    if (!runs(w)) {
        return 0;
    }
    *v = fetch(w->m, w->p);
    w->p = (uint16_t)(w->p + 1u);
    w->t += MARIA_DL_BYTE;
    return 1;
}

/* Whether a graphics address falls in a hole of the zone (A7800-MARIA
   section 2.3). */
static int holey(uint8_t entry, uint16_t addr)
{
    if ((addr & MARIA_A15) == 0u) {
        return 0;
    }
    return ((entry & MARIA_ENTRY_H16) != 0u && (addr & MARIA_A12) != 0u) ||
           ((entry & MARIA_ENTRY_H8) != 0u && (addr & MARIA_A11) != 0u);
}

/* Writes one cell when its data pair is not 00 or kangaroo mode is on.
   hpos wraps at 8 bits; cells 160-255 are scratch and keep nothing
   (A7800-MARIA section 3). */
static void cell(struct walk *w, uint8_t pos, uint8_t pal, uint8_t data)
{
    struct muhtari__maria *ma = &w->m->maria;
    if ((data != 0u || (w->ctrl & MARIA_CTRL_KANGAROO) != 0u) && pos < MUHTARI__LINE_CELLS) {
        ma->line_ram[ma->write_buf & 1u][pos] = (uint8_t)((pal << 2) | data);
    }
}

/* Writes graphics byte v of palette pal at hpos (A7800-MARIA section 3).
   Write mode 0: four cells, cell k holding P2 P1 P0 and data bits 7-2k and
   6-2k. Write mode 1: two cells, cell 0 holding P2 D3 D2 and D7 D6, cell 1
   P2 D1 D0 and D5 D4. */
static void draw(struct walk *w, uint8_t pal, uint8_t v)
{
    if (w->m->maria.write_mode == 0u) {
        for (uint8_t k = 0u; k < 4u; k++) {
            cell(w, (uint8_t)(w->hpos + k), pal, (uint8_t)((v >> (6u - 2u * k)) & 3u));
        }
        w->hpos = (uint8_t)(w->hpos + 4u);
    } else {
        uint8_t p2 = (uint8_t)(pal & 4u);
        cell(w, w->hpos, (uint8_t)(p2 | ((v >> 2) & 3u)), (uint8_t)(v >> 6));
        cell(w, (uint8_t)(w->hpos + 1u), (uint8_t)(p2 | (v & 3u)), (uint8_t)((v >> 4) & 3u));
        w->hpos = (uint8_t)(w->hpos + 2u);
    }
}

/* One graphics slot of 3 ticks at addr: drawn unless the address is in a
   hole (A7800-MARIA section 4.4). Returns 0 when the abort dropped it. */
static int gfx(struct walk *w, uint16_t addr, int hole, uint8_t pal)
{
    if (!runs(w)) {
        return 0;
    }
    if (!hole) {
        draw(w, pal, fetch(w->m, addr));
        w->st->bytes++;
    }
    w->t += MARIA_GFX_BYTE;
    return 1;
}

/* A direct object: byte i at hi:lo + i plus OFFSET pages, 3 ticks each. A
   holey byte costs its slot and ends the object (A7800-MARIA, section 2.3,
   section 4.2 and section 4.4). Returns 0 when the abort cut it short. */
static int direct(struct walk *w, uint16_t base, uint32_t width, uint8_t pal)
{
    const struct muhtari__maria *ma = &w->m->maria;
    for (uint32_t i = 0u; i < width; i++) {
        uint16_t addr = (uint16_t)(base + i + ((uint32_t)ma->offset << 8));
        int hole = holey(ma->entry, addr);
        if (!gfx(w, addr, hole, pal)) {
            return 0;
        }
        if (hole) {
            break;
        }
    }
    return 1;
}

/* An indirect object: map entry i at hi:lo + i with no OFFSET, 3 ticks,
   then the character's 1 or 2 graphics bytes at CHARBASE:c plus OFFSET
   pages, 3 ticks each. A character whose graphics fall in a hole costs its
   whole slot and ends the object (A7800-MARIA, section 2.3, section 4.2
   and section 4.4). Returns 0 when the abort cut it short. */
static int indirect(struct walk *w, uint16_t base, uint32_t width, uint8_t pal)
{
    const struct muhtari__maria *ma = &w->m->maria;
    uint32_t n = (w->ctrl & MARIA_CTRL_CW) != 0u ? 2u : 1u;
    for (uint32_t i = 0u; i < width; i++) {
        uint16_t addr;
        int hole;
        if (!runs(w)) {
            return 0;
        }
        addr =
            (uint16_t)(((uint32_t)ma->reg[MARIA_CHARBASE] << 8) +
                       (uint32_t)fetch(w->m, (uint16_t)(base + i)) + ((uint32_t)ma->offset << 8));
        w->st->bytes++;
        w->t += MARIA_CHARMAP;
        hole = holey(ma->entry, addr);
        for (uint32_t j = 0u; j < n; j++) {
            if (!gfx(w, (uint16_t)(addr + j), hole, pal)) {
                return 0;
            }
        }
        if (hole) {
            break;
        }
    }
    return 1;
}

/* Records the features an object uses (docs/hardware/maria.md,
   Statistics). */
static void object_features(struct walk *w, int ind)
{
    struct muhtari__maria *ma = &w->m->maria;
    static const uint32_t rm[4] = {MUHTARI__FEAT_RM0, MUHTARI__FEAT_RM0, MUHTARI__FEAT_RM2,
                                   MUHTARI__FEAT_RM3};
    uint32_t f = rm[w->ctrl & MARIA_CTRL_RM];
    if ((w->ctrl & MARIA_CTRL_KANGAROO) != 0u) {
        f |= MUHTARI__FEAT_KANGAROO;
    }
    if (ma->write_mode != 0u) {
        f |= MUHTARI__FEAT_WRITE_MODE;
    }
    if ((ma->entry & MARIA_ENTRY_H16) != 0u) {
        f |= MUHTARI__FEAT_HOLEY16;
    }
    if ((ma->entry & MARIA_ENTRY_H8) != 0u) {
        f |= MUHTARI__FEAT_HOLEY8;
    }
    if (ind) {
        f |= (w->ctrl & MARIA_CTRL_CW) != 0u ? MUHTARI__FEAT_IND2 : MUHTARI__FEAT_IND1;
    }
    ma->features |= f;
}

/* Walks the zone's DL from its base, header by header, until the
   terminator or the abort (A7800-MARIA section 2.2). Returns the address
   tick of the terminator's last byte; meaningless after an abort. */
static uint64_t walk_dl(struct walk *w)
{
    struct muhtari__maria *ma = &w->m->maria;
    for (;;) {
        uint8_t h[5] = {0u, 0u, 0u, 0u, 0u};
        uint8_t pw, five, ind = 0u;
        uint16_t base;
        int done;
        if (!dl_byte(w, &h[0]) || !dl_byte(w, &h[1]) || !dl_byte(w, &h[2]) || !dl_byte(w, &h[3])) {
            return 0u;
        }
        if ((h[1] & MARIA_TERM_MASK) == 0u) {
            return w->t - MARIA_DL_BYTE;
        }
        five = (uint8_t)((h[1] & MARIA_HDR_WIDTH) == 0u);
        if (five && !dl_byte(w, &h[4])) {
            return 0u;
        }
        w->st->headers++;
        base = (uint16_t)(((uint16_t)h[2] << 8) | h[0]);
        if (five) {
            ma->write_mode = (uint8_t)((h[1] & MARIA_HDR_WM) != 0u);
            ind = (uint8_t)((h[1] & MARIA_HDR_IND) != 0u);
            pw = h[3];
            w->hpos = h[4];
        } else {
            pw = h[1];
            w->hpos = h[3];
        }
        object_features(w, ind);
        if (ind) {
            done = indirect(w, base, MARIA_WIDTH_MAX - (pw & MARIA_HDR_WIDTH), (uint8_t)(pw >> 5));
        } else {
            done = direct(w, base, MARIA_WIDTH_MAX - (pw & MARIA_HDR_WIDTH), (uint8_t)(pw >> 5));
        }
        if (!done) {
            return 0u;
        }
        w->st->objects++;
    }
}

uint64_t muhtari__maria_dma_line(muhtari *m, uint64_t t0, uint64_t lrc, uint32_t line,
                                 struct muhtari__line_stats *st)
{
    struct muhtari__maria *ma = &m->maria;
    struct walk w;
    uint64_t end;
    int last = ma->offset == 0u;

    st->cost = 0u;
    st->bytes = 0u;
    st->headers = 0u;
    st->objects = 0u;
    st->flags = 0u;
    w.m = m;
    w.st = st;
    w.t = t0 + MARIA_PLA_START;
    w.lrc = lrc < t0 + MARIA_LINE_TICKS ? lrc : t0 + MARIA_LINE_TICKS;
    w.p = ma->dl;
    w.ctrl = ma->reg[MARIA_CTRL];
    w.hpos = 0u;
    w.aborted = 0u;

    end = walk_dl(&w);
    if (w.aborted) {
        st->flags |= MUHTARI__LINE_ABORTED;
        end = w.lrc + (last ? MARIA_ABORT_TAIL_LAST : MARIA_ABORT_TAIL_OTHER);
    } else {
        end += last ? MARIA_TAIL_LAST : MARIA_TAIL_OTHER;
    }
    if (last) {
        st->flags |= MUHTARI__LINE_LAST;
        read_entry(m);
        if ((ma->entry & MARIA_ENTRY_DLI) != 0u) {
            st->flags |= MUHTARI__LINE_DLI;
            ma->features |= MUHTARI__FEAT_DLI;
        }
    } else {
        ma->offset--;
    }
    st->cost = (uint16_t)(end - t0);
    if (line < MUHTARI__LINES_NTSC) {
        ma->stats[line] = *st;
    }
    return end;
}

/* The registers the CPU reads and writes outside the DMA set: WSYNC, MSTAT
   with VBLANK in bit 7, and OFFSET, whose writes are ignored (A7800-MARIA
   section 5). */
#define MARIA_WSYNC 0x04u
#define MARIA_MSTAT 0x08u
#define MARIA_OFFSET 0x18u
#define MARIA_VBLANK 0x80u

/* The fitted HALT handshake: the PLA starts 6 ticks after /HALT falls
   (docs/hardware/maria.md, The HALT handshake). */
#define MARIA_HALT_T0 6u

/* The line events, by index, and the tick in the line of each
   (A7800-MARIA section 4.1): t 0, LRC, swaps line RAM and is the abort
   deadline; t 28 requests the line's DMA; t 42 moves MSTAT on lines 16 and
   258; t 43 runs VBE on line 16. */
enum maria_event { EV_LRC, EV_DMA, EV_MSTAT, EV_VBE, EV_COUNT };
static const uint8_t maria_event_t[EV_COUNT] = {0u, MARIA_DMA_REQ_T, 42u, 43u};
#define MARIA_FIRST_DMA_LINE 17u
#define MARIA_VBE_LINE 16u
#define MARIA_VBLANK_LINE 258u

/* The picture (A7800-MARIA section 3.2, section 3.3 and section 6): pixel
   x of raster n, 0-319, plays at t 135 + x of line n from the line-RAM
   buffer line n - 1's burst filled, two pixels per cell. Frame rows 0-241
   are rasters 16-257. Rasters 16 and 17 show only BACKGRND, because no
   burst runs before line 17, so their buffers stay clear. Colour kill,
   CTRL bit 7, masks every output byte with $0F. */
#define MARIA_FIRST_PIXEL_T 135u
#define MARIA_PIXELS 320u
#define MARIA_FIRST_RASTER 16u
#define MARIA_BACKGRND 0x00u
#define MARIA_CTRL_KILL 0x80u
#define MARIA_KILL_MASK 0x0Fu

/* The pixels of a line played by tick t of it: those at t 135 to t. */
static uint32_t pixels_by(uint32_t t)
{
    if (t < MARIA_FIRST_PIXEL_T) {
        return 0u;
    }
    t = t - MARIA_FIRST_PIXEL_T + 1u;
    return t < MARIA_PIXELS ? t : MARIA_PIXELS;
}

/* The register a cell's pixel shows under read mode rm, right being the
   cell's second 320-pixel pixel: 0, BACKGRND, for colour 00, else palette
   p colour c at 4p + c (A7800-MARIA section 3.2). A cell holds P2 P1 P0 in
   bits 4-2 and its pair in bits 1-0. Read modes 0 and 1: the pair, for
   both pixels. Read mode 2 (320B, 320D): palette P2 0 0, colour bit 1 then
   bit 3 on the left and bit 0 then bit 2 on the right. Read mode 3 (320A,
   320C): the whole palette, colour bit 1 then 0 on the left and bit 0 then
   0 on the right. */
static uint32_t pixel_reg(uint8_t cell, uint32_t rm, uint32_t right)
{
    uint32_t pal = (uint32_t)(cell >> 2) & 7u;
    uint32_t c = cell & 3u;
    if (rm == 2u) {
        pal &= 4u;
        c = right ? ((cell & 1u) << 1) | ((cell >> 2) & 1u)
                  : (((uint32_t)cell >> 1 & 1u) << 1) | ((cell >> 3) & 1u);
    } else if (rm == 3u) {
        c = (right ? (cell & 1u) : ((uint32_t)cell >> 1 & 1u)) << 1;
    }
    return c == 0u ? MARIA_BACKGRND : pal * 4u + c;
}

/* Plays raster line's pixels from px up to x_end into the caller's row,
   with the registers as they stand. With no caller rows, or a raster
   outside the frame, nothing is written, but px moves on all the same, so
   a frame with no video leaves MARIA's state as a frame with video does. */
static void render(muhtari *m, uint32_t line, uint32_t x_end)
{
    struct muhtari__maria *ma = &m->maria;
    if (x_end <= ma->px) {
        return;
    }
    if (ma->rows != NULL && line >= MARIA_FIRST_RASTER &&
        line < MARIA_FIRST_RASTER + MUHTARI__HEIGHT_NTSC) {
        uint8_t *row = ma->rows + (size_t)(line - MARIA_FIRST_RASTER) * ma->pitch;
        const uint8_t *cells = ma->line_ram[(ma->write_buf ^ 1u) & 1u];
        uint8_t ctrl = ma->reg[MARIA_CTRL];
        uint32_t rm = ctrl & MARIA_CTRL_RM;
        uint8_t mask = (ctrl & MARIA_CTRL_KILL) != 0u ? MARIA_KILL_MASK : 0xFFu;
        for (uint32_t x = ma->px; x < x_end; x++) {
            row[x] = (uint8_t)(ma->reg[pixel_reg(cells[x >> 1], rm, x & 1u)] & mask);
        }
    }
    ma->px = (uint16_t)x_end;
}

/* Renders up to tick at, a register write's data tick: every pixel played
   at or before it shows the old value, later ones the new (A7800-MARIA
   section 5). */
static void render_to(muhtari *m, uint64_t at)
{
    uint64_t beam = at - m->maria.origin;
    uint64_t line = beam / MARIA_LINE_TICKS;
    if (at >= m->maria.origin && line < MUHTARI__LINES_NTSC) {
        render(m, (uint32_t)line, pixels_by((uint32_t)(beam % MARIA_LINE_TICKS)));
    }
}

/* Whether the event happens on line (A7800-MARIA section 4.1). */
static int event_on(uint32_t line, uint32_t ev)
{
    switch (ev) {
    case EV_LRC:
        return 1;
    case EV_DMA:
        return line >= MARIA_FIRST_DMA_LINE && line <= MARIA_VBLANK_LINE;
    case EV_MSTAT:
        return line == MARIA_VBE_LINE || line == MARIA_VBLANK_LINE;
    default:
        return line == MARIA_VBE_LINE;
    }
}

/* Arms the first event after tick t of line, or else the next line's t 0.
   Line 263 is line 0 of the next frame, whose t 0 moves the origin. */
static void arm_after(struct muhtari__maria *ma, uint32_t line, uint32_t t)
{
    for (uint32_t ev = 0u; ev < EV_COUNT; ev++) {
        if (maria_event_t[ev] > t && event_on(line, ev)) {
            ma->event_line = (uint16_t)line;
            ma->event_index = (uint8_t)ev;
            return;
        }
    }
    ma->event_line = (uint16_t)(line + 1u);
    ma->event_index = EV_LRC;
}

void muhtari__maria_start(muhtari *m)
{
    struct muhtari__maria *ma = &m->maria;
    uint64_t beam = m->bus.ticks - ma->origin;
    uint32_t line = (uint32_t)(beam / MARIA_LINE_TICKS);
    uint32_t t = (uint32_t)(beam % MARIA_LINE_TICKS);
    int vblank = line < MARIA_VBE_LINE || line > MARIA_VBLANK_LINE ||
                 (line == MARIA_VBE_LINE && t < maria_event_t[EV_MSTAT]) ||
                 (line == MARIA_VBLANK_LINE && t >= maria_event_t[EV_MSTAT]);
    ma->mstat = vblank ? MARIA_VBLANK : 0u;
    ma->px = (uint16_t)pixels_by(t);
    arm_after(ma, line, t);
}

uint64_t muhtari__maria_next_event(const muhtari *m)
{
    const struct muhtari__maria *ma = &m->maria;
    return ma->origin + (uint64_t)ma->event_line * MARIA_LINE_TICKS +
           maria_event_t[ma->event_index];
}

/* The line's DMA request at t 28: with CTRL's DMA mode on, HALT from now
   until the burst's HALTRST; a refusal means no burst this line. With DMA
   off the line says so. Either way a line with no burst has zero cost. */
static void request_dma(muhtari *m, uint32_t line, uint64_t tick)
{
    struct muhtari__maria *ma = &m->maria;
    struct muhtari__line_stats *st = &ma->stats[line];
    if ((ma->reg[MARIA_CTRL] & MARIA_CTRL_DM) == MARIA_DM_ON &&
        muhtari__bus_request_halt(m, tick, MUHTARI__HALT_UNTIL_MARIA)) {
        ma->dma_line = (uint16_t)line;
        ma->dma_lrc = tick - MARIA_DMA_REQ_T + MARIA_LINE_TICKS;
        return;
    }
    st->cost = 0u;
    st->bytes = 0u;
    st->headers = 0u;
    st->objects = 0u;
    if ((ma->reg[MARIA_CTRL] & MARIA_CTRL_DM) == MARIA_DM_ON) {
        st->flags = MUHTARI__LINE_DMA_BLOCKED;
        ma->features |= MUHTARI__FEAT_DMA_BLOCKED;
    } else {
        st->flags = MUHTARI__LINE_DMA_OFF;
    }
}

void muhtari__maria_event(muhtari *m)
{
    struct muhtari__maria *ma = &m->maria;
    uint64_t tick = muhtari__maria_next_event(m);
    uint32_t ev = ma->event_index;
    uint32_t line = ma->event_line;
    if (ev == EV_LRC) {
        /* The last pixel of the previous raster plays at this t 0. */
        render(m, line - 1u, MARIA_PIXELS);
        ma->px = 0u;
    }
    if (line == MUHTARI__LINES_NTSC) {
        ma->origin += MUHTARI__TICKS_PER_FRAME_NTSC;
        line = 0u;
    }
    switch (ev) {
    case EV_LRC:
        ma->write_buf ^= 1u;
        for (uint32_t c = 0u; c < MUHTARI__LINE_CELLS; c++) {
            ma->line_ram[ma->write_buf][c] = 0u;
        }
        break;
    case EV_DMA:
        request_dma(m, line, tick);
        break;
    case EV_MSTAT:
        ma->mstat = line == MARIA_VBLANK_LINE ? MARIA_VBLANK : 0u;
        break;
    default:
        if ((ma->reg[MARIA_CTRL] & MARIA_CTRL_DM) == MARIA_DM_ON) {
            (void)muhtari__maria_vbe(m, tick);
        }
        break;
    }
    arm_after(ma, line, maria_event_t[ev]);
}

/* The DLI: a burst that read an entry with DLI set pulls NMI low from the
   second phi1 after /HALT rises at its HALTRST (A7800-SYSTEM section 2,
   C6, MiSTer's timing, fitted). SALLY's phi1s fall on its 4-tick grid from
   c2, where the bus runs the burst, so the edge is the second grid tick
   after HALTRST. MARIA raises one edge per zone and never retries, so it
   keeps only whether the bus queued it (docs/hardware/bus.md, NMI). */
#define MARIA_CPU_CYCLE 4u
#define MARIA_DLI_PHI1 2u

uint64_t muhtari__maria_burst(muhtari *m, uint64_t fall)
{
    struct muhtari__maria *ma = &m->maria;
    struct muhtari__line_stats st;
    uint64_t c2 = m->bus.ticks;
    uint64_t haltrst =
        muhtari__maria_dma_line(m, fall + MARIA_HALT_T0, ma->dma_lrc, ma->dma_line, &st);
    if ((st.flags & MUHTARI__LINE_DLI) != 0u) {
        uint64_t phi1 = c2 + ((haltrst - c2) / MARIA_CPU_CYCLE + MARIA_DLI_PHI1) * MARIA_CPU_CYCLE;
        if (muhtari__bus_raise_nmi(m, phi1) && ma->dma_line < MUHTARI__LINES_NTSC) {
            ma->stats[ma->dma_line].flags |= MUHTARI__LINE_DLI_QUEUED;
            ma->features |= MUHTARI__FEAT_DLI_QUEUED;
        }
    }
    return haltrst;
}

uint8_t muhtari__maria_read(const muhtari *m, uint16_t addr)
{
    if ((addr & 0x1Fu) == MARIA_MSTAT) {
        return (uint8_t)((m->bus.open_bus & (uint8_t)~MARIA_VBLANK) | m->maria.mstat);
    }
    return m->bus.open_bus;
}

/* RDY is MARIA's WSYNC wire-ANDed with the TIA's, so the later release
   wins (docs/hardware/bus.md, RDY). Any other register write first
   renders the span played up to its data tick (A7800-MARIA section 10). */
void muhtari__maria_write(muhtari *m, uint16_t addr, uint8_t v, uint64_t data_tick)
{
    struct muhtari__maria *ma = &m->maria;
    uint32_t r = addr & 0x1Fu;
    if (r == MARIA_WSYNC) {
        uint64_t lines = (data_tick - ma->origin) / MARIA_LINE_TICKS + 1u;
        uint64_t release = ma->origin + lines * MARIA_LINE_TICKS;
        if (m->bus.rdy_release < release) {
            m->bus.rdy_release = release;
        }
        return;
    }
    if (r == MARIA_MSTAT || r == MARIA_OFFSET) {
        return;
    }
    render_to(m, data_tick);
    ma->reg[r] = v;
}
