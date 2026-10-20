/* Owned POKEY timers and DACs, from the hardware manual, never emulator
   source. Adopted clock/initialization conventions: hardware/pokey.md. */
#include <assert.h>
#include <string.h>
#include "internal.h"

static void level(muhtari *m, unsigned chip, unsigned ch, uint64_t tick)
{
    struct muhtari__pokey_channel *c = &m->pokey[chip].channel[ch];
    unsigned volume = c->audc & 15u;
    unsigned bit = c->output ^ (ch < 2u ? c->hp : 0u);
    unsigned raw = ((volume & 1u) * 12u + ((volume >> 1) & 1u) * 26u + ((volume >> 2) & 1u) * 56u +
                    ((volume >> 3) & 1u) * 112u);
    uint16_t value = (uint16_t)(((c->audc & 16u) != 0u || bit != 0u) ? raw : 0u);
    unsigned source = 2u + chip * 4u + ch;
    if (chip < m->synth.pokeys && m->synth.raw[source] != value) {
        muhtari__synth_level(m, tick, source, value);
    }
}

void muhtari__pokey_reset(muhtari *m, unsigned chip)
{
    assert(chip < 2u);
    struct muhtari__pokey *p = &m->pokey[chip];
    memset(p, 0, sizeof *p);
    p->initialized = 1u;
    p->skctl = m->profile.pokey_init != 0u ? 0u : 3u;
    p->irqst = 255u;
    p->tick = m->bus.clock_origin_tick;
    p->cycle = m->bus.clock_origin_phi2;
    p->carry[0] = p->carry[1] = UINT64_MAX;
    p->resync[0] = p->resync[1] = UINT64_MAX;
    p->prescale28 = 19u;
    p->prescale114 = 78u;
    for (unsigned ch = 0; ch < 4u; ch++) {
        p->channel[ch].hp = 1u;
        p->channel[ch].reload = p->cycle + 3u;
        p->channel[ch].fire = UINT64_MAX;
        p->channel[ch].starting = 1u;
    }
}

static void clock_cycle(muhtari *m, unsigned chip, uint64_t tick)
{
    struct muhtari__pokey *p = &m->pokey[chip];
    p->cycle++;
    unsigned base28 = 0u, base114 = 0u;
    if ((p->skctl & 3u) != 0u) {
        base28 = --p->prescale28 == 0u;
        base114 = --p->prescale114 == 0u;
        if (base28 != 0u) {
            p->prescale28 = 28u;
        }
        if (base114 != 0u) {
            p->prescale114 = 114u;
        }
        /* XNOR 1+x12+x17, shifting toward RANDOM's right end. Hidden
           state advances even with zero volume (manual chapter5). */
        uint32_t feedback = (~(p->poly17 ^ (p->poly17 >> 5))) & 1u;
        p->poly17 = (p->poly17 >> 1) | (feedback << 16);
    }
    for (unsigned ch = 0u; ch < 4u; ch++) {
        struct muhtari__pokey_channel *c = &p->channel[ch];
        unsigned direct =
            (ch == 0u && (p->audctl & 64u) != 0u) || (ch == 2u && (p->audctl & 32u) != 0u);
        unsigned joined = (p->audctl & (ch < 2u ? 16u : 8u)) != 0u;
        unsigned pulse = direct != 0u ? 1u : (p->audctl & 1u) != 0u ? base114 : base28;
        if (joined != 0u && (ch & 1u) != 0u) {
            pulse = p->carry[ch / 2u] == p->cycle;
            direct = (p->audctl & (ch == 1u ? 64u : 32u)) != 0u;
            if (pulse != 0u)
                p->carry[ch / 2u] = UINT64_MAX;
        }
        /* Process old pipeline output before a new underflow can queue
           its successor. Equal-tick channel order is always1,2,3,4. */
        if (p->cycle == c->fire) {
            c->fire = UINT64_MAX;
            if ((c->audc & 0xA0u) == 0xA0u) {
                c->output ^= 1u;
            }
            /* Serial output idles at mark. Only its two-tone timer
               selection is modeled: timer2 unconditional, timer1 mark
               unless force-break. Manual chapter5, two-tone resync. */
            if ((p->skctl & 8u) != 0u && (ch == 1u || (ch == 0u && (p->skctl & 128u) == 0u))) {
                p->resync[ch] = p->cycle + 1u;
            }
            c->last_fire = tick;
            c->fires++;
            unsigned mask = ch == 0u ? 1u : ch == 1u ? 2u : ch == 3u ? 4u : 0u;
            p->irqst &= (uint8_t)~(p->irqen & mask);
            if (ch >= 2u && (p->audctl & (ch == 2u ? 4u : 2u)) != 0u) {
                p->hp_capture[ch - 2u] = p->channel[ch - 2u].output;
                p->hp_delay[ch - 2u] = 3u;
            }
            level(m, chip, ch, tick);
        }
        /* Counter reload is three clocks after underflow. Its output
           pipeline takes one further clock. The forced STIMER reload
           instead initializes the counter without producing a pulse. */
        if (p->cycle == c->reload) {
            c->count = c->audf;
            c->reload = UINT64_MAX;
            c->starting = 0u;
        } else if (c->reload == UINT64_MAX && pulse != 0u) {
            if (c->count == 0u) {
                c->fire = p->cycle + (direct != 0u ? 4u : 5u);
                if (joined != 0u && (ch & 1u) == 0u) {
                    /* Linked low counter wraps without reloading; its
                       carry reaches the high counter three clocks later.
                       Manual chapter5, linked timer fire timing. */
                    c->count = 255u;
                    p->carry[ch / 2u] = p->cycle + 3u;
                } else {
                    c->reload = p->cycle + (direct != 0u ? 3u : 4u);
                    if (joined != 0u)
                        p->channel[ch - 1u].reload = c->reload;
                }
            } else {
                c->count--;
            }
        }
    }
    if (p->resync[0] == p->cycle || p->resync[1] == p->cycle) {
        for (unsigned i = 0u; i < 2u; i++) {
            if (p->resync[i] == p->cycle)
                p->resync[i] = UINT64_MAX;
            struct muhtari__pokey_channel *c = &p->channel[i];
            c->count = c->audf;
            c->reload = UINT64_MAX;
            c->starting = 0u;
            /* A pulse already delivered this clock survives; a later
               pulse is preempted. Adjacent delivered pulses may leave
               two distinct resyncs, one on each following clock. */
            if (c->fire > p->cycle)
                c->fire = UINT64_MAX;
        }
        p->carry[0] = UINT64_MAX;
    }
}

void muhtari__pokey_half_edge(muhtari *m, uint64_t tick, int falling)
{
    /* Merge the independent TIA stream before this CLK2 edge. It emits
       only through this shared minimum tick, so no later chip can insert
       an event behind the synthesizer's pending timestamp. */
    muhtari__tia_catch_up(m, tick);
    for (unsigned chip = 0; chip < 2u; chip++) {
        if (m->pokey[chip].initialized == 0u) {
            muhtari__pokey_reset(m, chip);
        }
        struct muhtari__pokey *p = &m->pokey[chip];
        assert(tick > p->tick);
        p->tick = tick;
        p->half++;
        for (unsigned ch = 0; ch < 2u; ch++) {
            if (p->hp_delay[ch] != 0u && --p->hp_delay[ch] == 0u) {
                p->channel[ch].hp = p->hp_capture[ch];
                level(m, chip, ch, tick);
            }
        }
        if (falling != 0) {
            clock_cycle(m, chip, tick);
        }
    }
}

void muhtari__pokey_catch_up(muhtari *m, uint64_t tick)
{
    for (unsigned chip = 0; chip < 2u; chip++) {
        if (m->pokey[chip].initialized == 0u) {
            muhtari__pokey_reset(m, chip);
        }
    }
    /* An explicitly requested fast segment, used by isolated chip
       callers. Bus accesses supply their actual asymmetric half edges. */
    while (m->pokey[0].tick + 2u <= tick) {
        int falling = (m->pokey[0].half & 1u) != 0u;
        muhtari__pokey_half_edge(m, m->pokey[0].tick + 2u, falling);
    }
    muhtari__tia_catch_up(m, tick);
}
void muhtari__pokey_write(muhtari *m, unsigned chip, uint8_t reg, uint8_t v, uint64_t tick)
{
    assert(chip < 2u);
    muhtari__pokey_catch_up(m, tick);
    struct muhtari__pokey *p = &m->pokey[chip];
    reg &= 15u;
    if (reg < 8u) {
        struct muhtari__pokey_channel *c = &p->channel[reg / 2u];
        if ((reg & 1u) == 0u) {
            c->audf = v;
        } else {
            c->audc = v;
            level(m, chip, reg / 2u, tick);
        }
    } else if (reg == 8u) {
        p->audctl = v;
        for (unsigned ch = 0; ch < 2u; ch++) {
            if ((v & (ch == 0u ? 4u : 2u)) == 0u) {
                p->hp_delay[ch] = 0u;
                p->channel[ch].hp = 1u;
                level(m, chip, ch, tick);
            }
        }
    } else if (reg == 9u) {
        p->carry[0] = p->carry[1] = UINT64_MAX;
        for (unsigned ch = 0; ch < 4u; ch++) {
            p->channel[ch].output = 1u;
            p->channel[ch].reload = p->cycle + 3u;
            /* Figure8: T+4 preempts IRQST atT+8, but T+5 cannot.
               A pulse past that boundary is already in the pipeline. */
            if (p->channel[ch].fire >= p->cycle + 4u)
                p->channel[ch].fire = UINT64_MAX;
            p->channel[ch].starting = 1u;
            level(m, chip, ch, tick);
        }
    } else if (reg == 14u) {
        p->irqen = v;
        p->irqst |= (uint8_t)~v;
    } else if (reg == 15u) {
        if ((v & 3u) == 0u) {
            p->prescale28 = 19u;
            p->prescale114 = 78u;
        }
        if ((v & 8u) == 0u)
            p->resync[0] = p->resync[1] = UINT64_MAX;
        p->skctl = v;
    }
}
uint8_t muhtari__pokey_read(muhtari *m, unsigned chip, uint8_t reg, uint64_t tick)
{
    assert(chip < 2u);
    muhtari__pokey_catch_up(m, tick);
    struct muhtari__pokey *p = &m->pokey[chip];
    if ((reg & 15u) == 14u) {
        return p->irqst;
    }
    return (reg & 15u) == 10u ? (uint8_t)~(p->poly17 >> 8) : 255u;
}
int muhtari__pokey_irq(const muhtari *m, unsigned chip)
{
    assert(chip < 2u);
    return (m->pokey[chip].irqen & (uint8_t)~m->pokey[chip].irqst) != 0u;
}
