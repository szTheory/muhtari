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
    struct muhtari__pokey_channel *c = &p->channel[0];
    p->tick = tick;
    p->cycle++;
    /* Fast timer reload at T+3; AUDF is sampled only at reload. The
       resulting N+4 period includes the split-counter pipeline (manual
       chapter 5, Reload timing). */
    if ((p->audctl & 64u) != 0u) {
        if (p->cycle == c->reload) {
            if (c->starting == 0u) {
                c->fire = p->cycle + 1u;
            }
            c->starting = 0u;
            c->reload = p->cycle + c->audf + 4u;
        }
        if (p->cycle == c->fire) {
            c->output ^= 1u;
            c->last_fire = tick;
            c->fires++;
            p->irqst &= (uint8_t)~(p->irqen & 1u);
            level(m, chip, 0u, tick);
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
    while (m->pokey[0].tick + 4u <= tick) {
        uint64_t next = m->pokey[0].tick + 4u;
        muhtari__tia_catch_up(m, next);
        for (unsigned chip = 0; chip < 2u; chip++) {
            clock_cycle(m, chip, next);
        }
    }
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
    } else if (reg == 9u) {
        for (unsigned ch = 0; ch < 4u; ch++) {
            p->channel[ch].output = 1u;
            p->channel[ch].reload = p->cycle + 3u;
            p->channel[ch].fire = UINT64_MAX;
            p->channel[ch].starting = 1u;
            level(m, chip, ch, tick);
        }
    } else if (reg == 14u) {
        p->irqen = v;
        p->irqst |= (uint8_t)~v;
    } else if (reg == 15u) {
        p->skctl = v;
    }
}
uint8_t muhtari__pokey_read(muhtari *m, unsigned chip, uint8_t reg, uint64_t tick)
{
    assert(chip < 2u);
    muhtari__pokey_catch_up(m, tick);
    return (reg & 15u) == 14u ? m->pokey[chip].irqst : 255u;
}
int muhtari__pokey_irq(const muhtari *m, unsigned chip)
{
    assert(chip < 2u);
    return (m->pokey[chip].irqen & (uint8_t)~m->pokey[chip].irqst) != 0u;
}
