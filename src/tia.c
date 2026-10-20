/* TIA input drivers, horizontal clock and audio (docs/hardware/tia.md). */
#include <string.h>
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
void muhtari__tia_reset(muhtari *m)
{
    static const uint16_t phases[4] = {0u, 64u, 144u, 288u};
    memset(&m->tia, 0, sizeof m->tia);
    m->tia.initialized = 1u;
    m->tia.epoch_phase = (uint16_t)(m->profile.tia_phase % TIA_LINE_TICKS);
    unsigned phase = 0u;
    while (phase < 4u && phases[phase] < m->tia.epoch_phase) {
        phase++;
    }
    m->tia.phase = (uint8_t)(phase & 3u);
    m->tia.next_tick = (phase == 4u ? TIA_LINE_TICKS : phases[phase]) - m->tia.epoch_phase;
}

static void tia_level(muhtari *m, unsigned ch, uint64_t tick)
{
    const struct muhtari__tia_channel *c = &m->tia.channel[ch];
    uint16_t level = c->output != 0u ? c->audv : 0u;
    if (level != m->synth.raw[ch]) {
        muhtari__synth_level(m, tick, ch, level);
    }
}

/* A compares AUDF, B clocks the selected waveform. The 456-tick
   schedule is independent of MARIA (A7800-AUDIO section 2). */
void muhtari__tia_catch_up(muhtari *m, uint64_t tick)
{
    static const uint16_t spacing[4] = {64u, 80u, 144u, 168u};
    if (m->tia.initialized == 0u) {
        muhtari__tia_reset(m);
    }
    while (m->tia.next_tick <= tick) {
        for (unsigned ch = 0u; ch < 2u; ch++) {
            struct muhtari__tia_channel *c = &m->tia.channel[ch];
            if ((m->tia.phase & 1u) == 0u) {
                c->enabled = (uint8_t)(c->count == c->audf);
                c->count = c->enabled != 0u ? 0u : (uint8_t)((c->count + 1u) & 31u);
            } else if (c->enabled != 0u && (c->audc == 4u || c->audc == 5u)) {
                c->output ^= 1u;
                tia_level(m, ch, m->tia.next_tick);
            }
        }
        m->tia.next_tick += spacing[m->tia.phase];
        m->tia.phase = (uint8_t)((m->tia.phase + 1u) & 3u);
    }
}

void muhtari__tia_write(muhtari *m, uint16_t addr, uint8_t v, uint64_t data_tick)
{
    muhtari__tia_catch_up(m, data_tick);
    unsigned reg = addr & 0x3Fu;
    if (reg >= 0x15u && reg <= 0x1Au) {
        unsigned ch = (reg - 0x15u) & 1u;
        struct muhtari__tia_channel *c = &m->tia.channel[ch];
        if (reg < 0x17u) {
            c->audc = v & 15u;
        } else if (reg < 0x19u) {
            c->audf = v & 31u;
        } else {
            c->audv = v & 15u;
            tia_level(m, ch, data_tick);
        }
    }
    if (reg != TIA_WSYNC) {
        return;
    }
    uint64_t next = data_tick + 1u;
    uint64_t into = (next + m->profile.tia_phase) % TIA_LINE_TICKS;
    uint64_t release = next + (into == 0u ? 0u : TIA_LINE_TICKS - into);
    if (m->bus.rdy_release < release) {
        m->bus.rdy_release = release;
    }
}
