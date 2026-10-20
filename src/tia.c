/* The TIA as the bus sees it: WSYNC, the TIA's source of RDY, and the read
   drivers. Video and audio come later (docs/hardware/bus.md). */
#include "internal.h"

/* The TIA counts its own line of 228 colour clocks of 2 ticks each, 2 ticks
   longer than MARIA's. */
#define TIA_LINE_TICKS 456u /* docs/hardware/bus.md, RDY */

/* The write register that strobes WSYNC, in every mirror. */
#define TIA_WSYNC 0x02u

/* The bits each read register drives, by A3-A0: D7-D6 for the collision
   latches, D7 alone for CXBLPF ($06) and INPT0-INPT5 ($08-$0D), nothing at
   $0E and $0F (docs/hardware/bus.md, Open bus). */
static const uint8_t tia_driven[16] = {
    0xC0u, 0xC0u, 0xC0u, 0xC0u, 0xC0u, 0xC0u, 0x80u, 0xC0u,
    0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x00u, 0x00u,
};

/* The driven bits: D7 of INPT0-INPT5 is the level the controllers put on
   it (src/ports.c); the collision latches read 0, as nothing is drawn by
   the TIA yet (docs/hardware/bus.md, Open bus). */
uint8_t muhtari__tia_read(const muhtari *m, uint16_t addr)
{
    unsigned reg = addr & 0x0Fu;
    uint8_t driven = tia_driven[reg];
    uint8_t level = 0u;
    if (reg >= 0x08u && reg <= 0x0Du) {
        level = (uint8_t)(muhtari__ports_inpt(m, reg - 0x08u) << 7);
    }
    return (uint8_t)((level & driven) | (m->bus.open_bus & (uint8_t)~driven));
}

/* A WSYNC write pulls RDY low until the TIA's line wraps: the first tick
   after the write's data at which tick + tia_phase is a multiple of 456
   (docs/hardware/bus.md, RDY; the edge is fitted). RDY is wire-ANDed with
   MARIA's WSYNC, so a later release already pending stands. The modulo is
   unsigned, so no signed 64-bit helper is called. */
void muhtari__tia_write(muhtari *m, uint16_t addr, uint64_t data_tick)
{
    if ((addr & 0x3Fu) != TIA_WSYNC) {
        return;
    }
    uint64_t next = data_tick + 1u;
    uint64_t into = (next + m->profile.tia_phase) % TIA_LINE_TICKS;
    uint64_t release = next + (into == 0u ? 0u : TIA_LINE_TICKS - into);
    if (m->bus.rdy_release < release) {
        m->bus.rdy_release = release;
    }
}
