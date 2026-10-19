/* SALLY's bus: the decode, the length of each cycle, the open-bus byte and
   the events that fall inside a cycle. The only place time advances
   (docs/hardware/bus.md). */
#include "internal.h"

/* What answers an address (docs/hardware/bus.md, Memory map). DEV_OPEN is
   every address nothing on the board drives yet: the cartridge and BIOS
   space with nothing loaded, the unused windows, and the RIOT until it lands.
   DEV_TIA and DEV_RIOT, and every address while MEN = 0, take slow cycles. */
enum bus_device {
    DEV_OPEN,
    DEV_RAM,
    DEV_MARIA,
    DEV_TIA,
    DEV_RIOT
};

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
        return low < 0x80u ? DEV_OPEN : DEV_RIOT;
    }
    if (addr < 0x0600u) {
        return (addr & 0x80u) != 0u ? DEV_RIOT : DEV_OPEN;
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

/* Recomputes next_deadline from the pending events. */
static void rearm(muhtari *m)
{
    m->bus.next_deadline = UINT64_MAX;
}

/* The events at or before a cycle's data tick. */
static void events(muhtari *m)
{
    rearm(m);
}

/* Runs everything before a cycle's access and returns its length: one
   compare against next_deadline on the fast path. */
static uint32_t begin(muhtari *m, enum bus_device dev)
{
    uint32_t data = slow(m, dev) ? BUS_SLOW_DATA : BUS_FAST_DATA;
    if (m->bus.ticks + data >= m->bus.next_deadline) {
        events(m);
    }
    return slow(m, dev) ? BUS_SLOW_TICKS : BUS_FAST_TICKS;
}

static void end(muhtari *m, uint32_t len)
{
    m->bus.ticks += len;
    m->bus.phi2++;
}

uint8_t muhtari__bus_read(muhtari *m, uint16_t addr)
{
    enum bus_device dev = decode(m, addr);
    uint32_t len = begin(m, dev);
    uint8_t v = m->bus.open_bus;
    if (dev == DEV_RAM) {
        v = m->ram[ram_index(addr)];
    }
    m->bus.open_bus = v;
    end(m, len);
    return v;
}

void muhtari__bus_write(muhtari *m, uint16_t addr, uint8_t v)
{
    enum bus_device dev = decode(m, addr);
    uint32_t len = begin(m, dev);
    if (dev == DEV_RAM) {
        m->ram[ram_index(addr)] = v;
    }
    m->bus.open_bus = v;
    end(m, len);
}
