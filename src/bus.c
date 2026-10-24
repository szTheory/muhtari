/* SALLY's bus: the decode, the length of each cycle, the open-bus byte and
   the events that fall inside a cycle. The only place time advances
   (docs/hardware/bus.md). */
#include "internal.h"

/* What answers an address (docs/hardware/bus.md, Memory map). DEV_OPEN is
   every address nothing drives: the unused windows in pages 2 and 3, and
   with MEN = 0 everything outside the TIA and the RIOT. DEV_CART is
   cartridge space, which the board answers through src/cart.c, or leaves
   to the open-bus byte. DEV_RIOT is the RIOT's ports and timer at
   $0280-$02FF and its RAM at $0480-$04FF, with their mirrors. DEV_TIA and
   DEV_RIOT, and every address while MEN = 0, take slow cycles. DEV_BIOS
   is the BIOS image while INPTCTRL's EXT is 0 (src/boot.c). */
enum bus_device { DEV_OPEN, DEV_RAM, DEV_MARIA, DEV_TIA, DEV_RIOT, DEV_CART, DEV_BIOS };

/* A fast cycle is 4 ticks with its data at the phi2 fall, tick 4; a slow
   cycle is 6 ticks with its data at tick 5 (docs/hardware/bus.md, Clocks). */
#define BUS_FAST_TICKS 4u
#define BUS_FAST_DATA 4u
#define BUS_SLOW_TICKS 6u
#define BUS_SLOW_DATA 5u

static int men(const muhtari *m)
{
    return (m->bus.inptctrl & MUHTARI__INPTCTRL_MEN) != 0u;
}

/* With MEN = 1: the TIA at $00-$1F and MARIA at $20-$3F of pages 0-3, RAM
   at $40-$FF of pages 0 and 1, the RIOT's ports at $280 and $380 and its RAM
   at $480 and $580, system RAM at $1800-$27FF, and the cartridge
   everywhere else from $0400 up. With MEN = 0, the 2600
   decode: A12 = 0 selects the TIA when A7 = 0 and the RIOT when A7 = 1, and
   there is no system RAM. A loaded BIOS answers A15 = 1 with MEN = 1, and
   A12 = 1 with MEN = 0, while EXT = 0 (docs/hardware/bus.md, Memory map). */
static enum bus_device decode(const muhtari *m, uint16_t addr)
{
    if (muhtari__boot_bios_maps(m, addr)) {
        return DEV_BIOS;
    }
    if (!men(m)) {
        if ((addr & 0x1000u) != 0u) {
            return DEV_OPEN;
        }
        return (addr & 0x80u) == 0u ? DEV_TIA : DEV_RIOT;
    }
    if (addr < 0x0400u) {
        uint16_t low = (uint16_t)(addr & 0xFFu);
        if (low < 0x20u) {
            return DEV_TIA;
        }
        if (low < 0x40u) {
            return DEV_MARIA;
        }
        if (addr < 0x0200u) {
            return DEV_RAM;
        }
        return low < 0x80u ? DEV_OPEN : DEV_RIOT; /* $0280, $0380: src/riot.c */
    }
    if (addr < 0x0600u) {
        return (addr & 0x80u) != 0u ? DEV_RIOT : DEV_CART; /* $0480, $0580: src/riot.c */
    }
    if (addr >= MUHTARI__RAM_BASE && addr < MUHTARI__RAM_BASE + MUHTARI__RAM_SIZE) {
        return DEV_RAM;
    }
    return DEV_CART;
}

/* $0040-$00FF and $0140-$01FF are the same RAM as $2040-$20FF and
   $2140-$21FF (docs/hardware/bus.md, Memory map). */
static uint16_t ram_index(uint16_t addr)
{
    if (addr < 0x0200u) {
        addr = (uint16_t)(addr + 0x2000u);
    }
    return (uint16_t)(addr - MUHTARI__RAM_BASE);
}

static int slow(const muhtari *m, enum bus_device dev)
{
    return dev == DEV_TIA || dev == DEV_RIOT || !men(m);
}

/* The fitted HALT rule (docs/hardware/bus.md, HALT): /HALT falls at the
   first cycle boundary at or after the request; SALLY runs that cycle (c0)
   and the next (c1), then freezes the third (c2) for every whole 4-tick
   cycle /HALT stays low, and completes it. HALT_RUN_CYCLES is the two
   cycles of latency, fitted, not measured. */
#define HALT_RUN_CYCLES 2u

/* Where a HALT request stands. */
enum halt_stage {
    HALT_NONE,
    HALT_REQUESTED, /* /HALT has not fallen yet */
    HALT_LOW        /* /HALT has fallen; halt_cycles counts c0 and c1 */
};

/* Recomputes next_deadline from the pending events: MARIA's next line
   event, a HALT request, an NMI edge. While RDY is low or /HALT is low
   every cycle takes the slow path. */
static void rearm(muhtari *m)
{
    struct muhtari__bus *b = &m->bus;
    uint64_t d = muhtari__maria_next_event(m);
    if (b->rdy_release != 0u || b->halt_stage == HALT_LOW) {
        d = 0u;
    }
    if (b->halt_stage == HALT_REQUESTED && b->halt_at < d) {
        d = b->halt_at;
    }
    if (b->nmi_pending != 0u && b->nmi_at < d) {
        d = b->nmi_at;
    }
    b->next_deadline = d;
}

/* Latches the pending NMI edge as phi2 cycle index, unless MEN is clear or
   an edge the CPU has not taken is already latched (docs/hardware/bus.md,
   NMI). */
static void latch_nmi(muhtari *m, uint64_t index)
{
    m->bus.nmi_pending = 0u;
    if (men(m) && m->bus.nmi_phi2 == 0u) {
        m->bus.nmi_phi2 = index;
    }
}

/* Whole 4-tick cycles from tick from until tick until, rounded up. */
static uint64_t cycles_until(uint64_t from, uint64_t until)
{
    return until > from ? (until - from + BUS_FAST_TICKS - 1u) / BUS_FAST_TICKS : 0u;
}

/* Fires every MARIA line event at or before tick limit, in tick order
   (docs/hardware/bus.md, Line events). */
static void maria_events(muhtari *m, uint64_t limit)
{
    while (muhtari__maria_next_event(m) <= limit) {
        muhtari__maria_event(m);
    }
}

/* The HALT window at a cycle's start, held or not: /HALT falls at the first
   boundary at or after the request; c0 and c1 run; c2 is frozen. Returns
   the whole cycles c2 is frozen for, 0 for any other cycle. A window MARIA
   asked for ends at the HALTRST of the burst it runs here, at c2, so a
   write the CPU made in c1 is in memory before the first fetch
   (docs/hardware/bus.md, HALT). */
static uint64_t halt_cycle(muhtari *m)
{
    struct muhtari__bus *b = &m->bus;
    uint64_t until = b->halt_until;
    if (b->halt_stage == HALT_REQUESTED && b->halt_at <= b->ticks) {
        b->halt_stage = HALT_LOW;
        b->halt_cycles = 0u;
        b->halt_fall = b->ticks;
    }
    if (b->halt_stage != HALT_LOW) {
        return 0u;
    }
    if (b->halt_cycles < HALT_RUN_CYCLES) {
        b->halt_cycles++;
        return 0u;
    }
    b->halt_stage = HALT_NONE;
    if (until == MUHTARI__HALT_UNTIL_MARIA) {
        until = muhtari__maria_burst(m, b->halt_fall);
    }
    return cycles_until(b->halt_fall, until);
}

/* Everything at a cycle's start whose data comes data ticks later: MARIA's
   line events up to the data tick, then the HALT window. Returns the
   cycles a frozen c2 is held for. */
static uint64_t cycle_start(muhtari *m, uint32_t data)
{
    m->bus.cycle_len = (uint8_t)(data == BUS_SLOW_DATA ? BUS_SLOW_TICKS : BUS_FAST_TICKS);
    maria_events(m, m->bus.ticks + data);
    return halt_cycle(m);
}

/* Advances k whole 4-tick held cycles. Each is counted in phi2, so whatever
   counts phi2 counts through the hold, and a pending NMI edge is latched by
   the held cycle whose phi2 falls at or after it (docs/hardware/bus.md, RDY
   and NMI). */
static void advance(muhtari *m, uint64_t k)
{
    struct muhtari__bus *b = &m->bus;
    if (m->synth.pokeys != 0u) {
        for (uint64_t cycle = 0; cycle < k; cycle++) {
            uint64_t start = b->ticks + cycle * BUS_FAST_TICKS;
            muhtari__pokey_half_edge(m, start + 2u, 0);
            muhtari__pokey_half_edge(m, start + BUS_FAST_DATA, 1);
        }
    }
    if (b->nmi_pending != 0u && b->nmi_at <= b->ticks + k * BUS_FAST_TICKS) {
        uint64_t j = b->nmi_at > b->ticks ? (b->nmi_at - b->ticks - 1u) / BUS_FAST_TICKS : 0u;
        latch_nmi(m, b->phi2 + 1u + j);
    }
    b->ticks += k * BUS_FAST_TICKS;
    b->phi2 += k;
}

/* The held cycles to advance before the next cycle start a pending event
   needs: MARIA's next line event, due by a held cycle's data tick, or the
   HALT window's next step. At least 1, at most n. */
static uint64_t hold_step(const muhtari *m, uint64_t n)
{
    const struct muhtari__bus *b = &m->bus;
    uint64_t k = n;
    uint64_t e = muhtari__maria_next_event(m);
    if (e > b->ticks + 2u * BUS_FAST_TICKS) {
        uint64_t j = cycles_until(b->ticks + BUS_FAST_DATA, e);
        k = j < k ? j : k;
    } else {
        k = 1u;
    }
    if (b->halt_stage == HALT_LOW) {
        k = 1u;
    } else if (b->halt_stage == HALT_REQUESTED) {
        uint64_t j = cycles_until(b->ticks, b->halt_at);
        k = j != 0u && j < k ? j : k;
    }
    return k != 0u ? k : 1u;
}

/* Holds the CPU for n whole 4-tick cycles from the current cycle start,
   which the caller has already run through cycle_start. The hold stops at
   every later held cycle start where a line event or the HALT window needs
   it, so a CPU parked in WSYNC or under HALT still sees each line's t 0,
   t 28, t 42 and t 43, and the line's burst runs at its own time. A c2
   frozen inside the hold extends it: the CPU runs on only when RDY and
   HALT have both released (docs/hardware/bus.md, Line events). */
static void hold(muhtari *m, uint64_t n)
{
    while (n > 0u) {
        uint64_t k = hold_step(m, n);
        advance(m, k);
        n -= k;
        if (n > 0u) {
            uint64_t frozen = cycle_start(m, BUS_FAST_DATA);
            n = frozen > n ? frozen : n;
        }
    }
}

/* The events at or before a cycle's data tick, data ticks after its start:
   MARIA's line events and the HALT window, whose c2 is frozen, then RDY,
   which holds a read that starts before its release until a cycle starts
   at or after it and never holds a write, then the NMI latch. After a hold
   the access runs at a new cycle start, which gets its own pass
   (docs/hardware/bus.md, HALT, RDY and NMI). */
static void events(muhtari *m, int is_read, uint32_t data)
{
    struct muhtari__bus *b = &m->bus;
    for (;;) {
        uint64_t n = cycle_start(m, data);
        if (n == 0u && is_read && b->rdy_release != 0u) {
            n = cycles_until(b->ticks, b->rdy_release);
            b->rdy_release = 0u;
        }
        if (n == 0u) {
            break;
        }
        hold(m, n);
    }
    if (b->nmi_pending != 0u && b->nmi_at <= b->ticks + data) {
        latch_nmi(m, b->phi2 + 1u);
    }
    rearm(m);
}

/* The tick, counted from a cycle's start, at which its data is on the bus. */
static uint32_t data_at(uint32_t len)
{
    return len == BUS_SLOW_TICKS ? BUS_SLOW_DATA : BUS_FAST_DATA;
}

/* Runs everything before a cycle's access and returns its length: one
   compare against next_deadline on the fast path. */
static uint32_t begin(muhtari *m, enum bus_device dev, int is_read)
{
    uint32_t len = slow(m, dev) ? BUS_SLOW_TICKS : BUS_FAST_TICKS;
    if (m->bus.ticks + data_at(len) >= m->bus.next_deadline) {
        events(m, is_read, data_at(len));
    }
    /* The asymmetric divider has rise s+2 in both modes, fall/data s+4
       fast or s+5 slow, completion s+4/s+6 (hardware/pokey.md). Flush
       every actual edge before data, including held-cycle segments. */
    if (m->synth.pokeys != 0u) {
        muhtari__pokey_half_edge(m, m->bus.ticks + 2u, 0);
        muhtari__pokey_half_edge(m, m->bus.ticks + data_at(len), 1);
    }
    return len;
}

static void end(muhtari *m, uint32_t len)
{
    m->bus.ticks += len;
    m->bus.phi2++;
    /* Both chips have consumed this cycle's actual falling edge before
       the CPU can poll it (hardware/pokey.md, Interrupts). */
    unsigned requested = (unsigned)(((m->cart.irq_mask & 1u) != 0u && m->cart.pokey_at[0] != 0u &&
                                     m->pokey[0].irq_signal != 0u) ||
                                    ((m->cart.irq_mask & 2u) != 0u && m->cart.pokey_at[1] != 0u &&
                                     m->pokey[1].irq_signal != 0u));
    /* The manual's common three-unhalted-cycle status-to-entry delay:
       two synchronization cycles, then the CPU's final instruction cycle.
       Held batches advance the source above, but never this CPU pipeline. */
    m->bus.irq_pipe = (uint8_t)((((unsigned)m->bus.irq_pipe << 1) | requested) & 7u);
    m->bus.irq_level = (uint8_t)((m->bus.irq_pipe & 4u) != 0u);
}

/* The input registers: the RIOT's SWCHA and SWCHB, its port registers 0
   and 2 (A9 = 1, A2 = 0, A0 = 0), and the TIA's INPT0-INPT5 at $08-$0D, in
   every mirror (docs/hardware/riot.md, Registers; docs/hardware/bus.md,
   Open bus). */
static int input_register(enum bus_device dev, uint16_t addr)
{
    if (dev == DEV_RIOT) {
        return (addr & 0x0205u) == 0x0200u;
    }
    if (dev == DEV_TIA) {
        unsigned reg = addr & 0x0Fu;
        return reg >= 0x08u && reg <= 0x0Du;
    }
    return 0;
}

/* The hand-off: the first opcode fetch from cartridge space once EXT is 1.
   INPTCTRL is 0 at power-on, so EXT = 1 means it rose since; the fast boot
   records frame 1 itself. Only the first such fetch counts, so a later
   return to the BIOS and back to the cartridge does not move it
   (docs/embedding.md, Booting with a BIOS). */
static void note_handoff(muhtari *m, enum bus_device dev)
{
    if (dev == DEV_CART && m->boot.handoff_frame == 0u && m->boot.rejected == 0u &&
        (m->bus.inptctrl & MUHTARI__INPTCTRL_EXT) != 0u) {
        m->boot.handoff_frame = m->frames + 1u;
    }
}

uint8_t muhtari__bus_read(muhtari *m, uint16_t addr)
{
    enum bus_device dev = decode(m, addr);
    uint32_t len = begin(m, dev, 1);
    uint8_t v = m->bus.open_bus;
    if (m->bus.sync != 0u) {
        m->bus.sync = 0u;
        note_handoff(m, dev);
    }
    /* Only a CPU read clears the frame's lag flag: MARIA's fetches and the
       peek below do not come through here. */
    if (input_register(dev, addr)) {
        m->ports.input_read = 1u;
    }
    if (dev == DEV_RAM) {
        v = m->ram[ram_index(addr)];
    } else if (dev == DEV_TIA) {
        v = muhtari__tia_read(m, addr);
    } else if (dev == DEV_RIOT) {
        v = muhtari__riot_read(m, addr);
    } else if (dev == DEV_MARIA) {
        v = muhtari__maria_read(m, addr);
    } else if (dev == DEV_CART) {
        v = muhtari__cart_read(m, addr, 0);
    } else if (dev == DEV_BIOS) {
        v = muhtari__boot_bios_byte(m, addr);
    }
    m->bus.open_bus = v;
    end(m, len);
    return v;
}

uint8_t muhtari__bus_peek(const muhtari *m, uint16_t addr)
{
    enum bus_device dev = decode(m, addr);
    switch (dev) {
    case DEV_RAM:
        return m->ram[ram_index(addr)];
    case DEV_TIA:
        return muhtari__tia_read(m, addr);
    case DEV_RIOT:
        return muhtari__riot_peek(m, addr);
    case DEV_MARIA:
        return muhtari__maria_read(m, addr);
    case DEV_CART:
        return muhtari__cart_peek(m, addr);
    case DEV_BIOS:
        return muhtari__boot_bios_byte(m, addr);
    default:
        return m->bus.open_bus;
    }
}

uint8_t muhtari__peek(const muhtari *m, uint16_t addr)
{
    enum bus_device dev = decode(m, addr);
    if (dev == DEV_RAM) {
        return m->ram[ram_index(addr)];
    }
    if (dev == DEV_RIOT && (addr & 0x0200u) == 0u) {
        return m->riot.ram[addr & (MUHTARI__RIOT_RAM_SIZE - 1u)];
    }
    if (dev == DEV_CART) {
        return muhtari__cart_peek(m, addr);
    }
    if (dev == DEV_BIOS) {
        return muhtari__boot_bios_byte(m, addr);
    }
    return m->bus.open_bus;
}

/* Until LOCK is set, every write the TIA's chip select sees also writes
   INPTCTRL, whose bits 4-7 do not exist, and counts towards the HALT gate,
   which opens at 2 (docs/hardware/bus.md, INPTCTRL and HALT). A BIOS that
   locks it with MEN = 0 before the hand-off has taken its 2600 path, as
   every BIOS's 2600 loader does with its last write, $FD
   (A7800-SYSTEM section 5); the core runs no 2600 cartridge, so the boot
   is rejected. */
static void inptctrl_write(muhtari *m, uint8_t v)
{
    struct muhtari__bus *b = &m->bus;
    if ((b->inptctrl & MUHTARI__INPTCTRL_LOCK) != 0u) {
        return;
    }
    b->inptctrl = (uint8_t)(v & MUHTARI__INPTCTRL_BITS);
    if (b->halt_gate_writes < 2u) {
        b->halt_gate_writes++;
    }
    if ((b->inptctrl & (MUHTARI__INPTCTRL_LOCK | MUHTARI__INPTCTRL_MEN)) ==
            MUHTARI__INPTCTRL_LOCK &&
        m->bios_size != 0u && m->boot.handoff_frame == 0u) {
        m->boot.rejected = 1u;
    }
}

void muhtari__bus_write(muhtari *m, uint16_t addr, uint8_t v)
{
    enum bus_device dev = decode(m, addr);
    uint32_t len = begin(m, dev, 0);
    if (dev == DEV_RAM) {
        m->ram[ram_index(addr)] = v;
    } else if (dev == DEV_TIA) {
        muhtari__tia_write(m, addr, v, m->bus.ticks + data_at(len));
        inptctrl_write(m, v);
        rearm(m);
    } else if (dev == DEV_RIOT) {
        muhtari__riot_write(m, addr, v, m->bus.ticks + data_at(len));
    } else if (dev == DEV_MARIA) {
        muhtari__maria_write(m, addr, v, m->bus.ticks + data_at(len));
        rearm(m);
    } else if (dev == DEV_CART) {
        muhtari__cart_write(m, addr, v);
    }
    m->bus.open_bus = v;
    end(m, len);
}

/* One window at a time: a request while one is waiting or /HALT is low is
   ignored, and the window in flight keeps its hold (docs/hardware/bus.md,
   HALT). Returns 1 when the window is taken, 0 when it is ignored. */
int muhtari__bus_request_halt(muhtari *m, uint64_t at_tick, uint64_t until_tick)
{
    struct muhtari__bus *b = &m->bus;
    if (b->halt_gate_writes < 2u || b->halt_stage != HALT_NONE) {
        return 0;
    }
    b->halt_at = at_tick;
    b->halt_until = until_tick;
    b->halt_stage = HALT_REQUESTED;
    rearm(m);
    return 1;
}

/* One edge at a time: an edge raised while an earlier one waits to be
   latched is ignored (docs/hardware/bus.md, NMI). Returns 1 when the edge
   is queued for latching, 0 when it is ignored; latch_nmi can still drop a
   queued edge. */
int muhtari__bus_raise_nmi(muhtari *m, uint64_t at_tick)
{
    if (!men(m) || m->bus.nmi_pending != 0u) {
        return 0;
    }
    m->bus.nmi_at = at_tick;
    m->bus.nmi_pending = 1u;
    rearm(m);
    return 1;
}

uint64_t muhtari__bus_phi1_after(const muhtari *m, uint64_t t, uint32_t n)
{
    uint64_t s = m->bus.ticks + m->bus.cycle_len;
    if (t >= s) {
        s += ((t - s) / BUS_FAST_TICKS + 1u) * BUS_FAST_TICKS;
    }
    return s + (uint64_t)(n - 1u) * BUS_FAST_TICKS;
}
