/* TIA input drivers, horizontal clock and audio (docs/hardware/tia.md). */
#include <string.h>
#include "internal.h"

/* The TIA counts its own line of 228 colour clocks of 2 ticks each, 2 ticks
   longer than MARIA's. */
#define TIA_LINE_TICKS 456u /* docs/hardware/bus.md, RDY */

/* Horizontal sync strobes, in every mirror (docs/hardware/tia.md). */
#define TIA_WSYNC 0x02u
#define TIA_RSYNC 0x03u

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

/* TIA-1A revision E sheet 4: A captures the noise input and ACLK gate;
   B clocks the pulse feedback from the old noise output (tia.md). */
static void tia_capture(struct muhtari__tia_channel *c)
{
    unsigned low = c->audc & 3u;
    unsigned tap = low != 0u ? c->noise >> 2 : c->pulse >> 3;
    c->noise_input = (uint8_t)(c->audc == 0u || (((c->noise >> 4) ^ tap) & 1u) != 0u ||
                               (c->noise == 0u && (low != 0u || c->pulse == 0u)));
    c->pulse_gate = (uint8_t)(low < 2u || (low == 2u && (c->noise == 8u || c->noise == 24u)) ||
                              (low == 3u && (c->noise & 16u) != 0u));
}

static void tia_shift(struct muhtari__tia_channel *c)
{
    unsigned pulse;
    if (c->audc == 0u) {
        pulse = 1u;
    } else {
        switch (c->audc >> 2) {
        case 0u:
            pulse = (((c->pulse >> 2) ^ (c->pulse >> 3)) & 1u) | (c->pulse == 0u);
            break;
        case 1u:
            pulse = (c->pulse & 1u) ^ 1u;
            break;
        case 2u:
            pulse = c->noise >> 4;
            break;
        default:
            pulse = (c->pulse & 4u) == 0u || (c->pulse & 7u) == 5u;
            break;
        }
    }
    if (c->pulse_gate != 0u) {
        c->pulse = (uint8_t)(((unsigned)c->pulse << 1 | pulse) & 15u);
    }
    c->noise = (uint8_t)(((unsigned)c->noise << 1 | c->noise_input) & 31u);
    c->output = c->pulse >> 3;
}

/* A compares AUDF, B commits the selected waveform. The 456-tick
   schedule is independent of MARIA (docs/hardware/tia.md). */
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
                if (c->enabled != 0u) {
                    tia_capture(c);
                }
            } else if (c->enabled != 0u) {
                tia_shift(c);
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
    if (reg == TIA_RSYNC) {
        /* Complete old due phases first. Skip the new epoch's A at zero:
           resynchronization schedules future edges only (tia.md). */
        m->tia.epoch_tick = data_tick;
        m->tia.epoch_phase = 0u;
        m->tia.next_tick = data_tick + 64u;
        m->tia.phase = 1u;
    }
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
    /* RDY is wire-ANDed with MARIA's pending WSYNC release. */
    uint64_t into =
        ((next - m->tia.epoch_tick) % TIA_LINE_TICKS + m->tia.epoch_phase) % TIA_LINE_TICKS;
    uint64_t release = next + (into == 0u ? 0u : TIA_LINE_TICKS - into);
    if (m->bus.rdy_release < release) {
        m->bus.rdy_release = release;
    }
}
