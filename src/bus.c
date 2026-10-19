/* SALLY's bus: the decode, the length of each cycle, the open-bus byte and
   the events that fall inside a cycle. The only place time advances
   (docs/hardware/bus.md). */
#include "internal.h"

/* What answers an address (docs/hardware/bus.md, Memory map). DEV_OPEN is
   every address nothing on the board drives yet: the cartridge and BIOS
   space with nothing loaded, and the unused windows. DEV_RIOT is the
   RIOT's ports and timer at $0280-$02FF and its RAM at $0480-$04FF, with
   their mirrors. DEV_TIA and DEV_RIOT, and every address while MEN = 0,
   take slow cycles. */
enum bus_device { DEV_OPEN, DEV_RAM, DEV_MARIA, DEV_TIA, DEV_RIOT };

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
   at $480 and $580, and system RAM at $1800-$27FF. With MEN = 0, the 2600
   decode: A12 = 0 selects the TIA when A7 = 0 and the RIOT when A7 = 1, and
   there is no system RAM (docs/hardware/bus.md, Memory map). */
static enum bus_device decode(const muhtari *m, uint16_t addr)
{
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
        return (addr & 0x80u) != 0u ? DEV_RIOT : DEV_OPEN; /* $0480, $0580: src/riot.c */
    }
    if (addr >= MUHTARI__RAM_BASE && addr < MUHTARI__RAM_BASE + MUHTARI__RAM_SIZE) {
        return DEV_RAM;
    }
    return DEV_OPEN;
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

/* Recomputes next_deadline from the pending events. While RDY is low or
   /HALT is low every cycle takes the slow path. */
static void rearm(muhtari *m)
{
    struct muhtari__bus *b = &m->bus;
    uint64_t d = UINT64_MAX;
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

/* Runs n whole 4-tick phi2 cycles in which the CPU is held. Each is counted
   in phi2, so whatever counts phi2 counts through the hold, and a pending
   NMI edge is latched by the held cycle whose phi2 falls at or after it
   (docs/hardware/bus.md, RDY and NMI). */
static void hold(muhtari *m, uint64_t n)
{
    struct muhtari__bus *b = &m->bus;
    if (n == 0u) {
        return;
    }
    if (b->nmi_pending != 0u && b->nmi_at <= b->ticks + n * BUS_FAST_TICKS) {
        uint64_t j = b->nmi_at > b->ticks ? (b->nmi_at - b->ticks - 1u) / BUS_FAST_TICKS : 0u;
        latch_nmi(m, b->phi2 + 1u + j);
    }
    b->ticks += n * BUS_FAST_TICKS;
    b->phi2 += n;
}

/* Whole 4-tick cycles from tick from until tick until, rounded up. */
static uint64_t cycles_until(uint64_t from, uint64_t until)
{
    return until > from ? (until - from + BUS_FAST_TICKS - 1u) / BUS_FAST_TICKS : 0u;
}

/* The events at or before a cycle's data tick, data ticks after its start:
   the HALT window, then RDY, which holds a read that starts before its
   release until a cycle starts at or after it and never holds a write, then
   the NMI latch (docs/hardware/bus.md, HALT, RDY and NMI). */
static void events(muhtari *m, int is_read, uint32_t data)
{
    struct muhtari__bus *b = &m->bus;
    if (b->halt_stage == HALT_REQUESTED && b->halt_at <= b->ticks) {
        b->halt_stage = HALT_LOW;
        b->halt_cycles = 0u;
        b->halt_lost = cycles_until(b->ticks, b->halt_until);
    }
    if (b->halt_stage == HALT_LOW) {
        if (b->halt_cycles == HALT_RUN_CYCLES) {
            b->halt_stage = HALT_NONE;
            hold(m, b->halt_lost);
        } else {
            b->halt_cycles++;
        }
    }
    if (is_read && b->rdy_release != 0u) {
        hold(m, cycles_until(b->ticks, b->rdy_release));
        b->rdy_release = 0u;
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
    return len;
}

static void end(muhtari *m, uint32_t len)
{
    m->bus.ticks += len;
    m->bus.phi2++;
}

uint8_t muhtari__bus_read(muhtari *m, uint16_t addr)
{
    enum bus_device dev = decode(m, addr);
    uint32_t len = begin(m, dev, 1);
    uint8_t v = m->bus.open_bus;
    if (dev == DEV_RAM) {
        v = m->ram[ram_index(addr)];
    } else if (dev == DEV_TIA) {
        v = muhtari__tia_read(m, addr);
    } else if (dev == DEV_RIOT) {
        v = muhtari__riot_read(m, addr);
    }
    m->bus.open_bus = v;
    end(m, len);
    return v;
}

/* Until LOCK is set, every write the TIA's chip select sees also writes
   INPTCTRL, whose bits 4-7 do not exist, and counts towards the HALT gate,
   which opens at 2 (docs/hardware/bus.md, INPTCTRL and HALT). */
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
}

void muhtari__bus_write(muhtari *m, uint16_t addr, uint8_t v)
{
    enum bus_device dev = decode(m, addr);
    uint32_t len = begin(m, dev, 0);
    if (dev == DEV_RAM) {
        m->ram[ram_index(addr)] = v;
    } else if (dev == DEV_TIA) {
        muhtari__tia_write(m, addr, m->bus.ticks + data_at(len));
        inptctrl_write(m, v);
        rearm(m);
    } else if (dev == DEV_RIOT) {
        muhtari__riot_write(m, addr, v);
    }
    m->bus.open_bus = v;
    end(m, len);
}

/* One window at a time: a request while one is waiting or /HALT is low is
   ignored, and the window in flight keeps its hold (docs/hardware/bus.md,
   HALT). */
void muhtari__bus_request_halt(muhtari *m, uint64_t at_tick, uint64_t until_tick)
{
    struct muhtari__bus *b = &m->bus;
    if (b->halt_gate_writes < 2u || b->halt_stage != HALT_NONE) {
        return;
    }
    b->halt_at = at_tick;
    b->halt_until = until_tick;
    b->halt_stage = HALT_REQUESTED;
    rearm(m);
}

/* One edge at a time: an edge raised while an earlier one waits to be
   latched is ignored (docs/hardware/bus.md, NMI). */
void muhtari__bus_raise_nmi(muhtari *m, uint64_t at_tick)
{
    if (!men(m) || m->bus.nmi_pending != 0u) {
        return;
    }
    m->bus.nmi_at = at_tick;
    m->bus.nmi_pending = 1u;
    rearm(m);
}
