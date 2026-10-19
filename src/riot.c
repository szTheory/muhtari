/* The MOS 6532 RIOT: its interval timer, its two ports with their direction
   registers, and its 128 bytes of RAM (docs/hardware/riot.md). */
#include "internal.h"

/* The 6532's register select: RS, wired to A9, picks the ports and the
   timer over the RAM; A2 then picks the timer over the ports, A1-A0 the
   port register or the timer's interval, and A4 a timer write over an edge
   control write (docs/hardware/riot.md, Registers). */
#define RIOT_RS 0x0200u
#define RIOT_A2 0x04u
#define RIOT_A4 0x10u

/* The four intervals as shifts: 1, 8, 64 and 1024 cycles, by A1-A0. */
static const uint8_t riot_interval_shift[4] = {0u, 3u, 6u, 10u};

/* The phi2 count of the cycle in progress; bus.phi2 counts completed ones. */
static uint64_t riot_now(const muhtari *m)
{
    return m->bus.phi2 + 1u;
}

/* The cycles since the timer was written, p >= 1: the writing cycle is p = 0
   (docs/hardware/riot.md, Timer). */
static uint64_t riot_elapsed(const muhtari *m)
{
    return riot_now(m) - m->riot.timer_phi2;
}

/* The cycle, counted as p, at which the timer passes zero: N intervals. */
static uint64_t riot_full(const struct muhtari__riot *r)
{
    return (uint64_t)r->timer_n << r->timer_shift;
}

/* INTIM, computed rather than counted: N - 1 - (p - 1) / I up to p = N I,
   then $FF at p = N I + 1 and one count less every cycle after, wrapping
   (docs/hardware/riot.md, Timer). Shifts stand in for the divisions, so no
   64-bit division helper is called. */
static uint8_t riot_intim(const muhtari *m)
{
    const struct muhtari__riot *r = &m->riot;
    uint64_t p = riot_elapsed(m);
    uint64_t full = riot_full(r);
    if (p <= full) {
        return (uint8_t)(r->timer_n - 1u - ((p - 1u) >> r->timer_shift));
    }
    return (uint8_t)(0xFFu - (p - full - 1u));
}

/* A port reads its output register on output pins and the pin on input
   pins, and every input pin reads high: nothing drives them yet
   (docs/hardware/riot.md, Ports). */
static uint8_t riot_port(uint8_t out, uint8_t ddr)
{
    return (uint8_t)((out & ddr) | (uint8_t)~ddr);
}

static uint8_t riot_io_read(const muhtari *m, uint16_t addr)
{
    const struct muhtari__riot *r = &m->riot;
    switch (addr & 0x03u) {
    case 0u:
        return riot_port(r->ora, r->ddra);
    case 1u:
        return r->ddra;
    case 2u:
        return riot_port(r->orb, r->ddrb);
    default:
        return r->ddrb;
    }
}

static void riot_io_write(muhtari *m, uint16_t addr, uint8_t v)
{
    struct muhtari__riot *r = &m->riot;
    switch (addr & 0x03u) {
    case 0u:
        r->ora = v;
        break;
    case 1u:
        r->ddra = v;
        break;
    case 2u:
        r->orb = v;
        break;
    default:
        r->ddrb = v;
        break;
    }
}

uint8_t muhtari__riot_read(muhtari *m, uint16_t addr)
{
    if ((addr & RIOT_RS) == 0u) {
        return m->riot.ram[addr & (MUHTARI__RIOT_RAM_SIZE - 1u)];
    }
    if ((addr & RIOT_A2) == 0u) {
        return riot_io_read(m, addr);
    }
    if ((addr & 0x01u) == 0u) {
        return riot_intim(m);
    }
    return 0x00u;
}

void muhtari__riot_write(muhtari *m, uint16_t addr, uint8_t v)
{
    struct muhtari__riot *r = &m->riot;
    if ((addr & RIOT_RS) == 0u) {
        r->ram[addr & (MUHTARI__RIOT_RAM_SIZE - 1u)] = v;
        return;
    }
    if ((addr & RIOT_A2) == 0u) {
        riot_io_write(m, addr, v);
        return;
    }
    if ((addr & RIOT_A4) != 0u) {
        r->timer_n = v;
        r->timer_shift = riot_interval_shift[addr & 0x03u];
        r->timer_phi2 = riot_now(m);
    }
}

/* RES zeroes the four I/O registers, though the profile may say port B's
   output register powers on otherwise, and selects a falling PA7 edge. The
   timer is not reset: it powers on reading $FF on the 1024-cycle interval,
   as if 256 had been written in the cycle before the first. RAM is zero
   filled (docs/hardware/riot.md, Power-on). */
void muhtari__riot_reset(muhtari *m)
{
    struct muhtari__riot *r = &m->riot;
    for (uint32_t i = 0u; i < MUHTARI__RIOT_RAM_SIZE; i++) {
        r->ram[i] = 0u;
    }
    r->ora = 0u;
    r->ddra = 0u;
    r->orb = m->profile.riot_orb_power_on;
    r->ddrb = 0u;
    r->timer_n = 0x100u;
    r->timer_shift = 10u;
    r->timer_phi2 = 0u;
    r->timer_cleared = 0u;
    r->pa7_flag = 0u;
    r->pa7_positive = 0u;
    r->pa7_detect = 1u;
}
