/* Owned chip vectors from hardware/pokey.md, through shipped objects. */
#include <string.h>
#include "internal.h"
#include "../check.h"

static muhtari machine;

static muhtari *fresh(void)
{
    memset(&machine, 0, sizeof machine);
    muhtari__power_on(&machine);
    machine.bus.inptctrl = 7u;
    muhtari__synth_reset(&machine, 0u, 2u);
    muhtari__pokey_reset(&machine, 0u);
    muhtari__pokey_reset(&machine, 1u);
    return &machine;
}

static void clock_vectors(void)
{
    muhtari *m = fresh();
    (void)muhtari__bus_read(m, 0x0280u);
    CHECK_EQ_U64(m->bus.ticks, 6u);
    CHECK_EQ_U64(m->pokey[0].tick, 5u);
    CHECK_EQ_U64(m->pokey[0].half, 2u);
    CHECK_EQ_U64(m->pokey[0].cycle, 1u);
    (void)muhtari__bus_read(m, 0x1800u);
    CHECK_EQ_U64(m->pokey[0].tick, 10u);
    CHECK_EQ_U64(m->pokey[0].half, 4u);
    CHECK_EQ_U64(m->pokey[0].cycle, 2u);
    for (unsigned slow = 0; slow < 2u; slow++) {
        m = fresh();
        muhtari__pokey_write(m, 0u, 8u, 64u, 0u);
        muhtari__pokey_write(m, 0u, 0u, 100u, 0u);
        muhtari__pokey_write(m, 0u, 1u, 0xAFu, 0u);
        muhtari__pokey_write(m, 0u, 9u, 0u, 0u);
        for (unsigned n = 0; n < 100u; n++) {
            (void)muhtari__bus_read(m, slow != 0u ? 0x0280u : 0x1800u);
        }
        for (unsigned n = 0; n < 8u; n++) {
            (void)muhtari__bus_read(m, 0x1800u);
        }
        CHECK_EQ_U64(m->pokey[0].channel[0].last_fire, 432u + slow * 200u);
        CHECK_EQ_U64(m->pokey[0].channel[0].fires, 1u);
    }
    m = fresh();
    m->bus.rdy_release = 100u;
    (void)muhtari__bus_read(m, 0x1800u);
    CHECK_EQ_U64(m->pokey[0].cycle, 26u);
    CHECK_EQ_U64(m->pokey[0].half, 52u);
    CHECK_EQ_U64(m->pokey[0].tick, 104u);
    CHECK_EQ_U64(m->pokey[1].cycle, 26u);
}

static void timer_vectors(void)
{
    /* The manual's 8-bit fast timer reloads at T+3, first IRQ/output
       at T+N+8, then every N+4 clocks (manual figure8, pokey.md). */
    const uint8_t divisors[] = {0u, 1u, 255u};
    for (unsigned d = 0; d < 3u; d++) {
        muhtari *p = fresh();
        muhtari__synth_reset(p, 0u, 1u);
        muhtari__pokey_reset(p, 0u);
        muhtari__pokey_reset(p, 1u);
        CHECK_EQ_U64(p->pokey[0].skctl, 3u);
        muhtari__pokey_write(p, 0u, 8u, 0x40u, 0u);
        muhtari__pokey_write(p, 0u, 0u, divisors[d], 0u);
        muhtari__pokey_write(p, 0u, 1u, 0xAFu, 0u);
        muhtari__pokey_write(p, 0u, 14u, 1u, 0u);
        muhtari__pokey_write(p, 0u, 9u, 0u, 0u);
        uint64_t first = 4u * ((uint64_t)divisors[d] + 8u);
        muhtari__pokey_catch_up(p, first - 1u);
        CHECK_EQ_U64(p->synth.raw[2], 0u);
        CHECK(!muhtari__pokey_irq(p, 0u));
        muhtari__pokey_catch_up(p, first);
        CHECK_EQ_U64(p->synth.raw[2], 206u);
        CHECK(muhtari__pokey_irq(p, 0u));
        CHECK_EQ_U64(muhtari__pokey_read(p, 0u, 14u, first), 0xFEu);
        muhtari__pokey_catch_up(p, first + 4u * (divisors[d] + 4u));
        CHECK_EQ_U64(p->synth.raw[2], 0u);
        muhtari__pokey_catch_up(p, 13124u);
        int16_t pcm[88];
        CHECK_EQ_U64(muhtari__synth_render(p, 13125u, pcm), 88u);
        unsigned nonzero = 0u;
        for (unsigned i = 0; i < 88u; i++) {
            nonzero += pcm[i] != 0;
        }
        CHECK(nonzero != 0u);
        CHECK_EQ_U64(p->pokey[1].cycle, p->pokey[0].cycle);
        CHECK_EQ_U64(p->pokey[1].channel[0].audc, 0u);
    }
    muhtari *alternate = fresh();
    alternate->profile.pokey_init = 1u;
    muhtari__pokey_reset(alternate, 0u);
    CHECK_EQ_U64(alternate->pokey[0].skctl, 0u);
}

static void logic_vectors(void)
{
    muhtari *m = fresh();
    muhtari__pokey_write(m, 0u, 8u, 0x60u, 0u);
    for (unsigned ch = 0; ch < 4u; ch++) {
        muhtari__pokey_write(m, 0u, (uint8_t)(ch * 2u), 0u, 0u);
        muhtari__pokey_write(m, 0u, (uint8_t)(ch * 2u + 1u), 0xAFu, 0u);
    }
    muhtari__pokey_write(m, 0u, 14u, 7u, 0u);
    muhtari__pokey_write(m, 0u, 9u, 0u, 0u);
    muhtari__pokey_catch_up(m, 96u);
    CHECK_EQ_U64(m->pokey[0].channel[0].fires, 5u);
    CHECK_EQ_U64(m->pokey[0].channel[2].fires, 5u);
    CHECK_EQ_U64(m->pokey[0].channel[1].last_fire, 96u);
    CHECK_EQ_U64(m->pokey[0].channel[3].last_fire, 96u);
    CHECK_EQ_U64(m->pokey[0].irqst, 248u);
    CHECK_EQ_U64(m->pokey[0].poly17 == 0u, 0u);
    CHECK_EQ_U64(m->pokey[1].channel[2].audc, 0u);
    muhtari__pokey_write(m, 0u, 14u, 0u, 96u);
    CHECK(!muhtari__pokey_irq(m, 0u));
}

static void high_pass_vectors(void)
{
    for (unsigned slow = 0; slow < 3u; slow++) {
        muhtari *m = fresh();
        /* High timer runs two cycles ahead of low, then both period6.
           The delayed cancellation leaves a half-phase pulse each lap. */
        muhtari__pokey_write(m, 0u, 8u, 0x64u, 0u);
        muhtari__pokey_write(m, 0u, 0u, 2u, 0u);
        muhtari__pokey_write(m, 0u, 4u, 0u, 0u);
        muhtari__pokey_write(m, 0u, 1u, 0xAFu, 0u);
        muhtari__pokey_write(m, 0u, 9u, 0u, 0u);
        for (unsigned cycle = 0; cycle < 14u; cycle++) {
            (void)muhtari__bus_read(m, slow != 0u ? 0x0280u : 0x1800u);
            if (cycle == 3u) {
                muhtari__pokey_write(m, 0u, 4u, 2u, m->pokey[0].tick);
            }
        }
        uint64_t fired = slow != 0u ? 83u : 56u;
        uint64_t start = m->bus.ticks;
        uint64_t fall = start + (slow == 1u ? 5u : 4u);
        uint64_t next_start = start + (slow == 1u ? 6u : 4u);
        CHECK_EQ_U64(m->pokey[0].channel[2].last_fire, fired);
        CHECK_EQ_U64(m->pokey[0].hp_delay[0], 3u);
        CHECK_EQ_U64(m->synth.raw[2], 206u);
        muhtari__pokey_half_edge(m, start + 2u, 0);
        muhtari__pokey_half_edge(m, fall, 1);
        CHECK_EQ_U64(m->synth.raw[2], 206u);
        muhtari__pokey_half_edge(m, next_start + 2u, 0);
        CHECK_EQ_U64(next_start + 2u - fired, slow == 0u ? 6u : slow == 1u ? 9u : 7u);
        CHECK_EQ_U64(m->synth.raw[2], 0u);
        muhtari__pokey_half_edge(m, next_start + (slow == 1u ? 5u : 4u), 1);
        CHECK_EQ_U64(m->synth.raw[2], 206u);
    }
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        return 2;
    }
    if (strcmp(argv[1], "clock") == 0) {
        clock_vectors();
    } else if (strcmp(argv[1], "logic") == 0) {
        timer_vectors();
        logic_vectors();
        high_pass_vectors();
    } else {
        return 2;
    }
    CHECK_DONE();
}
