/* Owned chip vectors from hardware/pokey.md, through shipped objects. */
#include <stdlib.h>
#include <string.h>
#include "internal.h"
#include "cartdb.h"
#include "sha1.h"
#include "../check.h"
#include "../cart/images.h"

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

/* Manual chapter5: joined $0000 fast high channel fires at11,18,...;
   high-pass channel2 is sampled by timer4, three half edges later. */
static void joined_pcm_trace(void)
{
    muhtari *m = fresh();
    muhtari__pokey_write(m, 0u, 8u, 0x72u, 0u);
    muhtari__pokey_write(m, 0u, 3u, 0xAFu, 0u);
    muhtari__pokey_write(m, 0u, 6u, 0u, 0u);
    muhtari__pokey_write(m, 0u, 9u, 0u, 0u);
    static muhtari reference;
    memset(&reference, 0, sizeof reference);
    muhtari__synth_reset(&reference, 0u, 2u);
    unsigned output = 1u, latch = 1u, captured = 0u, delay = 0u;
    for (unsigned cycle = 1u; cycle <= 3281u; cycle++) {
        for (unsigned half = 0u; half < 2u; half++) {
            uint64_t tick = cycle * 4u - 2u + half * 2u;
            if (delay != 0u && --delay == 0u)
                latch = captured;
            if (half != 0u) {
                if (cycle >= 11u && (cycle - 11u) % 7u == 0u)
                    output ^= 1u;
                if (cycle >= 24u && (cycle - 24u) % 28u == 0u) {
                    captured = output;
                    delay = 3u;
                }
            }
            muhtari__synth_level(&reference, tick, 3u, (uint16_t)((output ^ latch) * 206u));
        }
        (void)muhtari__bus_read(m, 0x1800u);
        CHECK_EQ_U64(m->synth.raw[3], (output ^ latch) * 206u);
        if (cycle == 10u)
            CHECK_EQ_U64(m->pokey[0].channel[1].fires, 0u);
        if (cycle == 11u) {
            CHECK_EQ_U64(m->pokey[0].channel[0].fires, 1u);
            CHECK_EQ_U64(m->pokey[0].channel[1].fires, 1u);
            CHECK_EQ_U64(m->synth.raw[3], 206u);
        }
        if (cycle == 18u)
            CHECK_EQ_U64(m->pokey[0].channel[1].fires, 2u);
        if (cycle == 24u)
            CHECK_EQ_U64(m->pokey[0].hp_delay[1], 3u);
    }
    CHECK_EQ_U64(m->pokey[0].channel[1].last_fire, (11u + 467u * 7u) * 4u);
    int16_t pcm[88], expected[88];
    CHECK_EQ_U64(muhtari__synth_render(m, 13125u, pcm), 88u);
    CHECK_EQ_U64(muhtari__synth_render(&reference, 13125u, expected), 88u);
    CHECK(memcmp(pcm, expected, sizeof pcm) == 0);
    unsigned nonzero = 0u;
    for (unsigned i = 0; i < 88u; i++)
        nonzero += pcm[i] != 0;
    CHECK(nonzero != 0u);
    CHECK(m->synth.kernel_adds_per_sample > 0u);
    CHECK_EQ_U64(m->synth.clipped_samples, 0u);
}

/* Drain production PCM during long timer vectors, retaining all chip state. */
static void run_to(muhtari *m, uint64_t cycle)
{
    while (m->pokey[0].cycle < cycle) {
        uint64_t next = m->pokey[0].cycle + 128u;
        if (next > cycle)
            next = cycle;
        muhtari__pokey_catch_up(m, next * 4u);
        (void)muhtari__synth_render(m, next * 4u, NULL);
    }
}

static void period_vectors(void)
{
    const unsigned values[] = {0u, 1u, 255u, 320u};
    for (unsigned pair = 0u; pair < 2u; pair++) {
        for (unsigned joined = 0u; joined < 2u; joined++) {
            for (unsigned clock = 0u; clock < 3u; clock++) {
                for (unsigned n = 0u; n < (joined != 0u ? 4u : 3u); n++) {
                    unsigned value = values[n], low = pair * 2u;
                    unsigned high = low + joined;
                    unsigned base = clock == 1u ? 28u : 114u;
                    uint64_t period =
                        clock == 0u ? value + (joined != 0u ? 7u : 4u) : (value + 1u) * base;
                    uint64_t first = clock == 0u ? value + (joined != 0u ? 11u : 8u)
                                                 : (clock == 1u ? 19u : 78u) + value * base +
                                                       (joined != 0u ? 8u : 5u);
                    muhtari *m = fresh();
                    uint8_t ctl = (uint8_t)((joined != 0u ? (pair == 0u ? 16u : 8u) : 0u) |
                                            (clock == 0u   ? (pair == 0u ? 64u : 32u)
                                             : clock == 2u ? 1u
                                                           : 0u));
                    muhtari__pokey_write(m, 0u, 8u, ctl, 0u);
                    muhtari__pokey_write(m, 0u, (uint8_t)(2u * low), (uint8_t)value, 0u);
                    if (joined != 0u)
                        muhtari__pokey_write(m, 0u, (uint8_t)(2u * high), (uint8_t)(value >> 8),
                                             0u);
                    muhtari__pokey_write(m, 0u, 9u, 0u, 0u);
                    run_to(m, first - 1u);
                    CHECK_EQ_U64(m->pokey[0].channel[high].fires, 0u);
                    run_to(m, first);
                    CHECK_EQ_U64(m->pokey[0].channel[high].last_fire, first * 4u);
                    if (joined != 0u)
                        CHECK_EQ_U64(m->pokey[0].channel[low].fires, (value >> 8) + 1u);
                    run_to(m, first + period);
                    CHECK_EQ_U64(m->pokey[0].channel[high].fires, 2u);
                    CHECK_EQ_U64(m->pokey[0].channel[high].last_fire, (first + period) * 4u);
                }
            }
        }
    }
}

static void reload_vectors(void)
{
    for (unsigned late = 0u; late < 2u; late++) {
        muhtari *m = fresh();
        muhtari__pokey_write(m, 0u, 8u, 64u, 0u);
        muhtari__pokey_write(m, 0u, 14u, 1u, 0u);
        muhtari__pokey_write(m, 0u, 9u, 0u, 0u);
        run_to(m, 4u + late);
        muhtari__pokey_write(m, 0u, 9u, 0u, (4u + late) * 4u);
        run_to(m, 8u);
        CHECK_EQ_U64(m->pokey[0].channel[0].fires, late);
        CHECK_EQ_U64(muhtari__pokey_irq(m, 0u), late);
    }
    for (unsigned late = 0u; late < 2u; late++) {
        muhtari *m = fresh();
        muhtari__pokey_write(m, 0u, 8u, 64u, 0u);
        muhtari__pokey_write(m, 0u, 9u, 0u, 0u);
        run_to(m, 2u + late);
        muhtari__pokey_write(m, 0u, 0u, 5u, (2u + late) * 4u);
        run_to(m, 13u);
        CHECK_EQ_U64(m->pokey[0].channel[0].last_fire, (late == 0u ? 13u : 8u) * 4u);
        CHECK_EQ_U64(m->pokey[0].channel[0].fires, 1u);
    }
    muhtari *m = fresh();
    muhtari__pokey_write(m, 0u, 8u, 64u, 0u);
    muhtari__pokey_write(m, 0u, 9u, 0u, 0u);
    run_to(m, 3u);
    CHECK_EQ_U64(m->pokey[0].channel[0].count, 0u);
    run_to(m, 6u);
    muhtari__pokey_write(m, 0u, 0u, 5u, 24u);
    run_to(m, 7u);
    CHECK_EQ_U64(m->pokey[0].channel[0].count, 5u);
    run_to(m, 17u);
    CHECK_EQ_U64(m->pokey[0].channel[0].last_fire, 68u);
    /* STIMER must not move either base clock. */
    CHECK_EQ_U64(m->pokey[0].prescale28, 2u);
}

static void two_tone_vectors(void)
{
    muhtari *m = fresh();
    muhtari__pokey_write(m, 0u, 8u, 64u, 0u);
    muhtari__pokey_write(m, 0u, 15u, 11u, 0u);
    muhtari__pokey_write(m, 0u, 9u, 0u, 0u);
    const unsigned fires[] = {8u, 14u, 20u};
    for (unsigned i = 0u; i < 3u; i++) {
        run_to(m, fires[i]);
        CHECK_EQ_U64(m->pokey[0].channel[0].last_fire, fires[i] * 4u);
        CHECK_EQ_U64(m->pokey[0].channel[0].fires, i + 1u);
    }
    m = fresh();
    muhtari__pokey_write(m, 0u, 8u, 64u, 0u);
    muhtari__pokey_write(m, 0u, 15u, 139u, 0u);
    muhtari__pokey_write(m, 0u, 9u, 0u, 0u);
    run_to(m, 20u);
    CHECK_EQ_U64(m->pokey[0].channel[0].fires, 4u);
    CHECK_EQ_U64(m->pokey[0].channel[0].last_fire, 80u);
    for (unsigned force = 0u; force < 2u; force++) {
        m = fresh();
        muhtari__pokey_write(m, 0u, 8u, 64u, 0u);
        muhtari__pokey_write(m, 0u, 0u, 15u, 0u);
        muhtari__pokey_write(m, 0u, 15u, (uint8_t)(force != 0u ? 139u : 11u), 0u);
        muhtari__pokey_write(m, 0u, 9u, 0u, 0u);
        run_to(m, 25u);
        CHECK_EQ_U64(m->pokey[0].channel[0].last_fire, 92u);
        CHECK_EQ_U64(m->pokey[0].channel[1].last_fire, 96u);
        run_to(m, 45u);
        /* Adjacent resyncs at24/25 extend N+4 by3. Force break
           removes timer1 resync; timer2 still resyncs at25. */
        CHECK_EQ_U64(m->pokey[0].channel[0].last_fire, 180u);
    }
}

static void polynomial_vectors(void)
{
    /* Chapter5 literal strings, not generated by the implementation. */
    static const char *const pattern[] = {"000011101100101", "0000011100100010101111011010011"};
    muhtari *m = fresh();
    unsigned history[2] = {0u, 0u};
    for (unsigned cycle = 1u; cycle <= 62u; cycle++) {
        run_to(m, cycle);
        for (unsigned poly = 0u; poly < 2u; poly++) {
            unsigned period = poly == 0u ? 15u : 31u;
            unsigned bit = (unsigned)(pattern[poly][(cycle - 1u) % period] - '0');
            history[poly] = (history[poly] << 1) | bit;
            for (unsigned ch = 0u; ch < 4u; ch++) {
                CHECK_EQ_U64((m->pokey[0].noise[poly] >> ch) & 1u, (history[poly] >> ch) & 1u);
            }
        }
    }
    /* Independent recurrence from the documented long polynomial;
       bit8 reaches the audio pipeline before the register's right end. */
    unsigned bits[96] = {0u}, history17 = 0u;
    for (unsigned i = 17u; i < 96u; i++)
        bits[i] = 1u ^ bits[i - 17u] ^ bits[i - 12u];
    m = fresh();
    for (unsigned cycle = 1u; cycle <= 80u; cycle++) {
        run_to(m, cycle);
        history17 = (history17 << 1) | bits[cycle + 7u];
        for (unsigned ch = 0u; ch < 4u; ch++)
            CHECK_EQ_U64((m->pokey[0].noise[2] >> ch) & 1u, (history17 >> ch) & 1u);
    }
    /* RANDOM's 9-bit reset/exit timing and 114-cycle sequence, chapter5.
       The unique00 state is382 clocks after the fully drained zero state. */
    m = fresh();
    muhtari__pokey_write(m, 0u, 8u, 128u, 0u);
    muhtari__pokey_write(m, 0u, 15u, 0u, 0u);
    run_to(m, 18u);
    muhtari__pokey_write(m, 0u, 15u, 3u, 72u);
    run_to(m, 22u);
    CHECK_EQ_HEX(muhtari__pokey_read(m, 0u, 10u, 88u), 0x1Fu);
    const unsigned bytes[] = {0x00u, 0xDFu, 0xEEu, 0x16u, 0xB9u};
    for (unsigned i = 0u; i < 5u; i++) {
        uint64_t cycle = 18u + 382u + i * 114u;
        run_to(m, cycle);
        CHECK_EQ_HEX(muhtari__pokey_read(m, 0u, 10u, cycle * 4u), bytes[i]);
    }
    uint64_t cycle = m->pokey[0].cycle;
    uint32_t draining = m->pokey[0].poly17;
    muhtari__pokey_write(m, 0u, 15u, 0u, cycle * 4u);
    run_to(m, cycle + 9u);
    CHECK_EQ_U64(m->pokey[0].poly17, draining >> 9);
    CHECK_EQ_HEX(muhtari__pokey_read(m, 0u, 10u, (cycle + 9u) * 4u), 0xFFu);
    run_to(m, cycle + 18u);
    CHECK_EQ_U64(m->pokey[0].poly17, 0u);
    for (unsigned reg = 0u; reg < 16u; reg++) {
        if (reg != 10u && reg != 14u)
            CHECK_EQ_HEX(muhtari__pokey_read(m, 0u, (uint8_t)reg, m->pokey[0].tick), 0xFFu);
    }
}

static void init_vectors(void)
{
    for (unsigned clock = 0u; clock < 2u; clock++) {
        muhtari *m = fresh();
        m->profile.pokey_init = 1u;
        muhtari__pokey_reset(m, 0u);
        CHECK_EQ_U64(m->pokey[0].skctl, 0u);
        muhtari__pokey_write(m, 0u, 8u, (uint8_t)(64u | clock), 0u);
        muhtari__pokey_write(m, 0u, 1u, 0xAFu, 0u);
        muhtari__pokey_write(m, 0u, 14u, 2u, 0u);
        muhtari__pokey_write(m, 0u, 9u, 0u, 0u);
        run_to(m, 200u);
        CHECK(m->pokey[0].channel[0].fires > 40u);
        CHECK_EQ_U64(m->pokey[0].channel[1].fires, 0u);
        CHECK_EQ_U64(m->pokey[0].prescale28, 19u);
        CHECK_EQ_U64(m->pokey[0].prescale114, 78u);
        CHECK_EQ_U64(m->pokey[0].poly17, 0u);
        muhtari__pokey_write(m, 0u, 15u, 3u, 800u);
        unsigned first = clock == 0u ? 19u : 78u;
        run_to(m, 200u + first - 1u);
        CHECK_EQ_U64(m->pokey[0].channel[1].fires, 0u);
        run_to(m, 200u + first);
        CHECK_EQ_U64(clock == 0u ? m->pokey[0].prescale28 : m->pokey[0].prescale114,
                     clock == 0u ? 28u : 114u);
        CHECK(!muhtari__pokey_irq(m, 0u));
        run_to(m, 200u + first + 5u);
        CHECK_EQ_U64(m->pokey[0].channel[1].last_fire, (200u + first + 5u) * 4u);
        CHECK(muhtari__pokey_irq(m, 0u));
    }
}

static void joined_init_vectors(void)
{
    muhtari *m = fresh();
    muhtari__pokey_write(m, 0u, 15u, 0u, 0u);
    muhtari__pokey_write(m, 0u, 8u, 0x58u, 0u);
    muhtari__pokey_write(m, 0u, 9u, 0u, 0u);
    run_to(m, 200u);
    CHECK_EQ_U64(m->pokey[0].channel[1].fires, 28u);
    CHECK_EQ_U64(m->pokey[0].channel[3].fires, 0u);
}

static void distortion_vectors(void)
{
    /* Every AUDC mode, including poly5 gate, all channel sample stages,
       volume-only, both long/short noise modes, and zero volume. */
    for (unsigned short_poly = 0u; short_poly < 2u; short_poly++) {
        for (unsigned mode = 0u; mode < 16u; mode++) {
            muhtari *m = fresh();
            muhtari__pokey_write(m, 0u, 8u, (uint8_t)(0x60u | (short_poly * 128u)), 0u);
            for (unsigned ch = 0u; ch < 4u; ch++)
                muhtari__pokey_write(m, 0u, (uint8_t)(2u * ch + 1u), (uint8_t)(mode * 16u + 15u),
                                     0u);
            muhtari__pokey_write(m, 0u, 9u, 0u, 0u);
            unsigned expected[4] = {1u, 1u, 1u, 1u};
            for (unsigned cycle = 1u; cycle <= 400u; cycle++) {
                uint32_t before[4];
                for (unsigned ch = 0u; ch < 4u; ch++)
                    before[ch] = m->pokey[0].channel[ch].fires;
                run_to(m, cycle);
                for (unsigned ch = 0u; ch < 4u; ch++) {
                    const struct muhtari__pokey_channel *c = &m->pokey[0].channel[ch];
                    if (c->fires != before[ch] &&
                        ((mode & 8u) != 0u || ((m->pokey[0].noise[1] >> ch) & 1u) != 0u)) {
                        if ((mode & 2u) != 0u)
                            expected[ch] ^= 1u;
                        else
                            expected[ch] =
                                (m->pokey[0].noise[(mode & 4u) != 0u ? 0u : 2u] >> ch) & 1u;
                    }
                    CHECK_EQ_U64(c->output, expected[ch]);
                    unsigned bit = expected[ch] ^ (ch < 2u ? 1u : 0u);
                    CHECK_EQ_U64(m->synth.raw[2u + ch],
                                 ((mode & 1u) != 0u || bit != 0u) ? 206u : 0u);
                }
            }
        }
    }
}

static void silent_state_vectors(void)
{
    static muhtari control;
    for (unsigned volume_only = 0u; volume_only < 2u; volume_only++) {
        muhtari *m = fresh();
        muhtari__pokey_write(m, 0u, 8u, 0xE4u, 0u);
        muhtari__pokey_write(m, 0u, 1u, 0xCFu, 0u);
        muhtari__pokey_write(m, 0u, 9u, 0u, 0u);
        memcpy(&control, m, sizeof control);
        muhtari__pokey_write(m, 0u, 1u, volume_only != 0u ? 0xDFu : 0xC0u, 0u);
        /* Different catch-up partitions and inaudible/overridden state. */
        run_to(m, 1000u);
        for (unsigned cycle = 1u; cycle <= 1000u; cycle++)
            run_to(&control, cycle);
        muhtari__synth_reset(m, 4000u, 2u);
        muhtari__synth_reset(&control, 4000u, 2u);
        muhtari__pokey_write(m, 0u, 1u, 0xCFu, 4000u);
        muhtari__pokey_write(&control, 0u, 1u, 0xCFu, 4000u);
        CHECK(memcmp(&m->pokey, &control.pokey, sizeof m->pokey) == 0);
        for (unsigned cycle = 1001u; cycle <= 4281u; cycle++) {
            muhtari__pokey_catch_up(m, cycle * 4u);
            muhtari__pokey_catch_up(&control, cycle * 4u);
            CHECK_EQ_U64(m->synth.raw[2], control.synth.raw[2]);
        }
        int16_t pcm[88], expected[88];
        CHECK_EQ_U64(muhtari__synth_render(m, 17125u, pcm), 88u);
        CHECK_EQ_U64(muhtari__synth_render(&control, 17125u, expected), 88u);
        CHECK(memcmp(pcm, expected, sizeof pcm) == 0);
    }
}

static void independence_vectors(void)
{
    static muhtari control;
    muhtari *m = fresh();
    muhtari__pokey_write(m, 1u, 8u, 0xE4u, 0u);
    muhtari__pokey_write(m, 1u, 0u, 31u, 0u);
    muhtari__pokey_write(m, 1u, 1u, 0x4Fu, 0u);
    muhtari__pokey_write(m, 1u, 9u, 0u, 0u);
    memcpy(&control, m, sizeof control);
    muhtari__pokey_write(m, 0u, 8u, 0x72u, 0u);
    muhtari__pokey_write(m, 0u, 15u, 139u, 0u);
    muhtari__pokey_write(m, 0u, 9u, 0u, 0u);
    for (unsigned cycle = 128u; cycle <= 4096u; cycle += 128u) {
        run_to(m, cycle);
        run_to(&control, cycle);
        CHECK(memcmp(&m->pokey[1], &control.pokey[1], sizeof m->pokey[1]) == 0);
        CHECK(memcmp(&m->synth.raw[6], &control.synth.raw[6], 4u * sizeof(uint16_t)) == 0);
    }
    CHECK(m->pokey[1].channel[0].fires != 0u);
}

static void volume_override_vectors(void)
{
    muhtari *m = fresh();
    muhtari__pokey_write(m, 0u, 8u, 0x64u, 0u);
    muhtari__pokey_write(m, 0u, 1u, 0xBFu, 0u);
    muhtari__pokey_write(m, 0u, 9u, 0u, 0u);
    for (unsigned cycle = 1u; cycle <= 64u; cycle++) {
        run_to(m, cycle);
        CHECK_EQ_U64(m->synth.raw[2], 206u);
    }
    /* Removing the override reveals the still-running HP/output state. */
    muhtari__pokey_write(m, 0u, 1u, 0xAFu, 256u);
    CHECK_EQ_U64(m->synth.raw[2],
                 (m->pokey[0].channel[0].output ^ m->pokey[0].channel[0].hp) * 206u);
    muhtari__pokey_write(m, 0u, 8u, 0x60u, 256u);
    CHECK_EQ_U64(m->pokey[0].channel[0].hp, 1u);
    CHECK_EQ_U64(m->pokey[0].hp_delay[0], 0u);
    CHECK_EQ_U64(m->synth.raw[2], (m->pokey[0].channel[0].output ^ 1u) * 206u);
}

/* Runtime maps: A78 codes and access priorities in hardware/cartridge.md.
   Generate every image in memory and run actual SALLY instructions. */
#define MAP_BODY 16384u
#define MAP_AUDIO 801u
static uint8_t map_image[IMAGES_HEADER + 65536u];

static void *map_alloc(void *user, size_t size)
{
    (void)user;
    return malloc(size);
}

static void map_free(void *user, void *ptr, size_t size)
{
    (void)user;
    (void)size;
    free(ptr);
}

static muhtari *map_machine(void)
{
    muhtari_config cfg;
    memset(&cfg, 0, sizeof cfg);
    cfg.size = (uint32_t)sizeof cfg;
    cfg.allocator.alloc = map_alloc;
    cfg.allocator.free = map_free;
    muhtari *m = NULL;
    CHECK_EQ_U64(muhtari_create(&cfg, &m), MUHTARI_STATUS_OK);
    return m;
}

static size_t map_header(uint8_t version, uint16_t type, uint8_t code, uint32_t body)
{
    images_header(map_image, version, "Mapped sound", body, type);
    map_image[67] = version >= 4u ? code : 0u;
    memset(map_image + IMAGES_HEADER, 0x5A, body);
    images_footer(map_image + IMAGES_HEADER, body);
    return IMAGES_HEADER + body;
}

static void map_load(muhtari *m, size_t size)
{
    muhtari_media media;
    memset(&media, 0, sizeof media);
    media.size = (uint32_t)sizeof media;
    media.image = map_image;
    media.image_size = size;
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    /* Fast boot leaves an unlocked handoff; ordinary cart code locks MEN. */
    muhtari__bus_write(m, 1u, 7u);
}

static void map_store(uint8_t *program, size_t *n, uint16_t addr, uint8_t value)
{
    program[(*n)++] = 0xA9u; /* LDA #value; STA addr */
    program[(*n)++] = value;
    program[(*n)++] = 0x8Du;
    program[(*n)++] = (uint8_t)addr;
    program[(*n)++] = (uint8_t)(addr >> 8);
}

static void map_readback(uint8_t *program, size_t *n, uint16_t addr, uint8_t mailbox)
{
    program[(*n)++] = 0xADu; /* LDA addr; STA $18xx */
    program[(*n)++] = (uint8_t)addr;
    program[(*n)++] = (uint8_t)(addr >> 8);
    program[(*n)++] = 0x8Du;
    program[(*n)++] = mailbox;
    program[(*n)++] = 0x18u;
}

static void map_program(uint16_t primary, int secondary)
{
    uint8_t *program = map_image + IMAGES_HEADER;
    size_t n = 0u;
    for (unsigned chip = 0u; chip < 2u; chip++) {
        uint16_t at = chip == 0u ? primary : 0x0440u;
        if ((chip == 0u && at == 0u) || (chip == 1u && !secondary))
            continue;
        map_store(program, &n, (uint16_t)(at + 15u), 0u); /* INIT */
        for (unsigned delay = 0u; delay < 5u; delay++)
            program[n++] = 0xEAu; /* ten clocks drain RANDOM to $FF */
        map_readback(program, &n, (uint16_t)(at + 10u), (uint8_t)(2u * chip));
        map_readback(program, &n, (uint16_t)(at + 14u), (uint8_t)(2u * chip + 1u));
        map_store(program, &n, (uint16_t)(at + 15u), 3u);
        map_store(program, &n, (uint16_t)(at + 8u), 0x40u);
        map_store(program, &n, at, chip == 0u ? 200u : 137u);
        map_store(program, &n, (uint16_t)(at + 1u), 0xAFu);
        map_store(program, &n, (uint16_t)(at + 9u), 0u);
    }
    /* Linear 16K body begins at $C000. Keep the loop clear of registers. */
    uint16_t loop = (uint16_t)(0xC000u + n);
    program[n++] = 0x4Cu;
    program[n++] = (uint8_t)loop;
    program[n] = (uint8_t)(loop >> 8);
}

static uint32_t map_frame(muhtari *m, int16_t pcm[MAP_AUDIO])
{
    muhtari_frame frame;
    memset(&frame, 0, sizeof frame);
    frame.size = (uint32_t)sizeof frame;
    frame.audio = pcm;
    frame.audio_capacity = MAP_AUDIO;
    CHECK_EQ_U64(muhtari_run_frame(m, &frame), MUHTARI_STATUS_OK);
    unsigned nonzero = 0u;
    for (uint32_t i = 0u; i < frame.audio_count; i++)
        nonzero += pcm[i] != 0;
    CHECK(nonzero != 0u);
    return frame.audio_count;
}

static void map_windows(void)
{
    static const uint16_t types[] = {1u, 0x40u, 0x8000u, 0x400u, 0x440u};
    static const uint8_t codes[] = {5u, 2u, 4u, 1u, 3u};
    static const uint16_t primary[] = {0x4000u, 0x0450u, 0x0800u, 0u, 0x0450u};
    muhtari *m = map_machine();
    for (unsigned version = 3u; version <= 4u; version++) {
        for (unsigned k = 0u; k < 5u; k++) {
            size_t size = map_header((uint8_t)version, types[k], codes[k], MAP_BODY);
            map_program(primary[k], k >= 3u);
            map_load(m, size);
            CHECK_EQ_U64(m->cart.info.warnings, 0u);
            CHECK_EQ_HEX(m->cart.pokey_at[0], primary[k]);
            CHECK_EQ_HEX(m->cart.pokey_at[1], k >= 3u ? 0x0440u : 0u);
            CHECK_EQ_U64(m->synth.pokeys, k == 4u ? 2u : 1u);
            CHECK_EQ_U64(m->synth.secondary, k == 3u);
            int16_t pcm[MAP_AUDIO];
            for (unsigned f = 0u; f < 3u; f++)
                (void)map_frame(m, pcm);
            for (unsigned chip = 0u; chip < 2u; chip++) {
                uint16_t at = chip == 0u ? primary[k] : (k >= 3u ? 0x0440u : 0u);
                if (at == 0u) {
                    for (unsigned ch = 0u; ch < 4u; ch++)
                        CHECK_EQ_U64(m->synth.raw[2u + chip * 4u + ch], 0u);
                    continue;
                }
                CHECK_EQ_HEX(m->ram[2u * chip], 0xFFu);      /* INIT RANDOM */
                CHECK_EQ_HEX(m->ram[2u * chip + 1u], 0xFFu); /* IRQST */
                CHECK_EQ_U64(m->pokey[chip].channel[0].audf, chip == 0u ? 200u : 137u);
                CHECK(m->pokey[chip].channel[0].fires != 0u);
                muhtari__bus_write(m, (uint16_t)(at + 15u), 3u);
                uint32_t end = at == 0x4000u ? 0x8000u : at == 0x0800u ? 0x1000u : at + 16u;
                /* Every mirrored block, including the last, writes the same
                   timer/DAC and reads documentary IRQST/unconnected values. */
                for (uint32_t block = at; block < end; block += 16u) {
                    muhtari__bus_write(m, (uint16_t)block, 37u);
                    CHECK_EQ_U64(m->pokey[chip].channel[0].audf, 37u);
                    muhtari__bus_write(m, (uint16_t)(block + 1u), 0x1Fu);
                    CHECK_EQ_U64(m->synth.raw[2u + chip * 4u], 206u);
                    CHECK_EQ_HEX(muhtari__bus_read(m, (uint16_t)(block + 14u)), 0xFFu);
                    CHECK_EQ_HEX(muhtari__bus_read(m, (uint16_t)(block + 15u)), 0xFFu);
                }
                /* Addresses immediately outside must not change this chip. */
                muhtari__bus_write(m, (uint16_t)(at - 1u), 0u);
                muhtari__bus_write(m, (uint16_t)end, 0u);
                CHECK_EQ_U64(m->pokey[chip].channel[0].audf, 37u);
                CHECK_EQ_U64(m->pokey[chip].skctl, 3u);
            }
            CHECK_EQ_U64(muhtari_power_cycle(m), MUHTARI_STATUS_OK);
            CHECK_EQ_U64(m->synth.pokeys, k == 4u ? 2u : 1u);
            for (unsigned source = 2u; source < 10u; source++)
                CHECK_EQ_U64(m->synth.raw[source], 0u);
        }
    }
    size_t size = map_header(3u, 0u, 0u, MAP_BODY);
    map_load(m, size);
    CHECK_EQ_U64(m->synth.pokeys, 0u);
    CHECK_EQ_U64(m->synth.secondary, 0u);
    CHECK_EQ_U64(m->cart.pokey_at[0], 0u);
    CHECK_EQ_U64(m->cart.pokey_at[1], 0u);
    muhtari__bus_write(m, 0x0451u, 0x1Fu);
    muhtari__bus_write(m, 0x0441u, 0x1Fu);
    for (unsigned source = 2u; source < 10u; source++)
        CHECK_EQ_U64(m->synth.raw[source], 0u);
    muhtari_destroy(m);
}

static void map_mono(void)
{
    int16_t pcm[3][MAP_AUDIO];
    uint32_t counts[3];
    static const uint8_t codes[] = {2u, 1u, 3u};
    muhtari *m = map_machine();
    for (unsigned k = 0u; k < 3u; k++) {
        size_t size = map_header(4u, 0u, codes[k], MAP_BODY);
        /* Identical instructions/timing even for the absent chip. */
        map_program(0x0450u, 1);
        map_load(m, size);
        (void)map_frame(m, pcm[k]);
        (void)map_frame(m, pcm[k]);
        counts[k] = map_frame(m, pcm[k]);
        CHECK_EQ_U64(m->pokey[0].channel[0].audc, k == 1u ? 0u : 0xAFu);
        CHECK_EQ_U64(m->pokey[1].channel[0].audc, k == 0u ? 0u : 0xAFu);
    }
    CHECK_EQ_U64(counts[0], counts[2]);
    CHECK_EQ_U64(counts[1], counts[2]);
    CHECK(memcmp(pcm[0], pcm[2], counts[2] * sizeof(int16_t)) != 0);
    CHECK(memcmp(pcm[1], pcm[2], counts[2] * sizeof(int16_t)) != 0);
    /* Hold the two-chip normalization constant while muting each chip.
       Either disconnected mix input must make one comparison equal. */
    for (unsigned k = 0u; k < 3u; k++) {
        size_t size = map_header(4u, 0u, 3u, MAP_BODY);
        map_program(0x0450u, 1);
        map_load(m, size);
        (void)map_frame(m, pcm[k]);
        (void)map_frame(m, pcm[k]);
        muhtari__bus_write(m, 0x0451u, k == 1u ? 0u : 0xAFu);
        muhtari__bus_write(m, 0x0441u, k == 0u ? 0u : 0xAFu);
        counts[k] = map_frame(m, pcm[k]);
    }
    CHECK_EQ_U64(counts[0], counts[2]);
    CHECK_EQ_U64(counts[1], counts[2]);
    CHECK(memcmp(pcm[0], pcm[2], counts[2] * sizeof(int16_t)) != 0);
    CHECK(memcmp(pcm[1], pcm[2], counts[2] * sizeof(int16_t)) != 0);
    muhtari_destroy(m);
}

static void map_pure_reads(muhtari *m, uint16_t addr, uint8_t value)
{
    static muhtari before;
    memcpy(&before, m, sizeof before);
    CHECK_EQ_HEX(muhtari__bus_peek(m, addr), value);
    CHECK(memcmp(&before, m, sizeof before) == 0);
    /* This is the board access used by MARIA's actual DMA fetch. The
       fetch itself additionally updates open_bus; this seam must not. */
    CHECK_EQ_HEX(muhtari__cart_read(m, addr, 1), value);
    CHECK(memcmp(&before, m, sizeof before) == 0);
}

/* Run the actual DMA walker as well as its board-read seam: a one-byte
   direct object at RANDOM must use floating/board data, not the chip. */
static void map_dma(muhtari *m, uint16_t addr, uint8_t value)
{
    uint8_t dl[] = {(uint8_t)addr, 31u, (uint8_t)(addr >> 8), 60u, 0u, 0u, 0u, 0u};
    memcpy(m->ram + 0x100u, dl, sizeof dl); /* DL at $1900 */
    m->maria.dl = 0x1900u;
    m->maria.dll = 0x1A00u;
    m->maria.offset = 0u;
    m->maria.entry = 0u;
    m->maria.write_mode = 0u;
    m->maria.reg[0x1Cu] = 0u; /* 160A, one byte -> four two-bit cells */
    memset(m->maria.line_ram, 0, sizeof m->maria.line_ram);
    struct muhtari__pokey before[2];
    static struct muhtari__synth synth;
    memcpy(before, m->pokey, sizeof before);
    memcpy(&synth, &m->synth, sizeof synth);
    uint64_t ticks = m->bus.ticks;
    uint64_t phi2 = m->bus.phi2;
    struct muhtari__line_stats stats;
    CHECK_EQ_U64(muhtari__maria_dma_line(m, ticks, ticks + 454u, 20u, &stats), ticks + 26u);
    CHECK_EQ_U64(stats.bytes, 1u);
    for (unsigned cell = 0u; cell < 4u; cell++)
        CHECK_EQ_U64(m->maria.line_ram[m->maria.write_buf][60u + cell],
                     (value >> (6u - 2u * cell)) & 3u);
    CHECK(memcmp(before, m->pokey, sizeof before) == 0);
    CHECK(memcmp(&synth, &m->synth, sizeof synth) == 0);
    CHECK_EQ_U64(m->bus.ticks, ticks);
    CHECK_EQ_U64(m->bus.phi2, phi2);
}

static void map_accesses(void)
{
    static const uint16_t types[] = {1u, 0x40u, 0x8000u, 0x400u, 0x440u};
    static const uint16_t windows[] = {0x4000u, 0x0450u, 0x0800u, 0x0440u, 0x0450u};
    muhtari *m = map_machine();
    for (unsigned k = 0u; k < 5u; k++) {
        map_load(m, map_header(3u, types[k], 0u, MAP_BODY));
        for (unsigned n = 0u; n < (k == 4u ? 2u : 1u); n++) {
            uint16_t at = n == 0u ? windows[k] : 0x0440u;
            unsigned chip = at == 0x0440u ? 1u : 0u;
            muhtari__bus_write(m, (uint16_t)(at + 15u), 0u);
            for (unsigned delay = 0u; delay < 20u; delay++)
                (void)muhtari__bus_read(m, 0x1800u);
            /* Pure observation returns board/open data even at RANDOM
               and IRQST; the CPU reads documented chip data instead. */
            m->bus.open_bus = 0x3Cu;
            map_pure_reads(m, (uint16_t)(at + 10u), 0x3Cu);
            map_pure_reads(m, (uint16_t)(at + 14u), 0x3Cu);
            map_dma(m, (uint16_t)(at + 10u), 60u);
            uint64_t cycles = m->pokey[chip].cycle;
            CHECK_EQ_HEX(muhtari__bus_read(m, (uint16_t)(at + 10u)), 0xFFu);
            CHECK_EQ_U64(m->pokey[chip].cycle, cycles + 1u);
            muhtari__bus_write(m, (uint16_t)(at + 15u), 3u);
            muhtari__bus_write(m, (uint16_t)(at + 8u), 0x40u);
            muhtari__bus_write(m, at, 0u);
            muhtari__bus_write(m, (uint16_t)(at + 1u), 0xAFu);
            muhtari__bus_write(m, (uint16_t)(at + 14u), 1u);
            muhtari__bus_write(m, (uint16_t)(at + 9u), 0u);
            CHECK_EQ_U64(m->pokey[chip].channel[0].output, 1u);
            for (unsigned delay = 0u; delay < 12u; delay++)
                (void)muhtari__bus_read(m, 0x1800u);
            CHECK(muhtari__pokey_irq(m, chip));
            m->bus.open_bus = 0x63u;
            map_pure_reads(m, (uint16_t)(at + 14u), 0x63u);
            CHECK_EQ_HEX(muhtari__bus_read(m, (uint16_t)(at + 14u)), 0xFEu);
            CHECK(muhtari__pokey_irq(m, chip)); /* reads do not acknowledge */
            muhtari__bus_write(m, (uint16_t)(at + 14u), 0u);
            CHECK(!muhtari__pokey_irq(m, chip));
            CHECK_EQ_HEX(muhtari__bus_read(m, (uint16_t)(at + 14u)), 0xFFu);
        }
    }
    muhtari_destroy(m);
}

static void map_overlaps(void)
{
    /* Linear 48/64K, SG EXROM/EXFIX, and v4 linear/SG/banked RAM.
       Legacy RAM conflicts deliberately normalize away POKEY elsewhere. */
    static const struct {
        uint8_t version, mapper, option;
        uint16_t type;
        uint32_t body;
        int ram;
    } cases[] = {{3u, 0u, 0u, 1u, 49152u, 0},   {3u, 0u, 0u, 1u, 65536u, 0},
                 {3u, 0u, 0u, 0xBu, 49152u, 0}, {4u, 1u, 5u, 0u, 65536u, 0},
                 {4u, 0u, 1u, 0u, MAP_BODY, 1}, {4u, 1u, 1u, 0u, 65536u, 1},
                 {4u, 1u, 6u, 0u, 65536u, 1}};
    muhtari *m = map_machine();
    for (unsigned k = 0u; k < sizeof cases / sizeof cases[0]; k++) {
        size_t size = map_header(cases[k].version, cases[k].type, 5u, cases[k].body);
        map_image[64] = cases[k].mapper;
        map_image[65] = cases[k].option;
        map_load(m, size);
        CHECK_EQ_HEX(m->cart.pokey_at[0], 0x4000u);
        if (cases[k].ram)
            memset(m->cart.ram, 0x63, m->cart.info.ram_size);
        /* Distinct ROM and RAM sentinels make accidentally reading POKEY,
           writing RAM, or selecting the wrong priority observable. */
        uint8_t expected = cases[k].ram ? 0x63u : 0x5Au;
        static const uint16_t blocks[] = {0x4000u, 0x5FF0u, 0x7FF0u};
        for (unsigned b = 0u; b < 3u; b++) {
            uint16_t at = blocks[b];
            muhtari__bus_write(m, (uint16_t)(at + 1u), 0x1Fu);
            CHECK_EQ_U64(m->synth.raw[2], 206u);
            CHECK_EQ_U64(m->pokey[0].channel[0].audc, 0x1Fu);
            CHECK_EQ_HEX(muhtari__bus_read(m, (uint16_t)(at + 1u)), expected);
            CHECK_EQ_HEX(muhtari__bus_read(m, (uint16_t)(at + 14u)), expected);
            map_pure_reads(m, (uint16_t)(at + 1u), expected);
            map_pure_reads(m, (uint16_t)(at + 10u), expected);
            map_dma(m, (uint16_t)(at + 10u), expected);
            CHECK_EQ_U64(m->cart.bank, 0u);
        }
        /* Banked RAM's other half keeps the same priority. */
        if (cases[k].option == 6u) {
            muhtari__bus_write(m, 0x8000u, 32u);
            CHECK_EQ_U64(m->cart.ram_bank, 1u);
            m->cart.ram[16385u] = 0x79u;
            muhtari__bus_write(m, 0x4001u, 0x18u);
            CHECK_EQ_U64(m->synth.raw[2], 112u);
            CHECK_EQ_HEX(muhtari__bus_read(m, 0x4001u), 0x79u);
            map_pure_reads(m, 0x4001u, 0x79u);
        }
    }
    muhtari_destroy(m);
}

struct gate_record {
    uint64_t tick;
    uint8_t source;
    uint16_t raw;
};
struct gate_capture {
    struct gate_record records[8192];
    unsigned count;
};

static void gate_record(void *user, uint64_t tick, uint8_t source, uint16_t raw)
{
    struct gate_capture *capture = user;
    CHECK(capture->count < 8192u);
    if (capture->count < 8192u) {
        struct gate_record *r = &capture->records[capture->count++];
        r->tick = tick;
        r->source = source;
        r->raw = raw;
    }
}

/* Compare independently loaded, continuously enabled tone controls. Gate
   stores have matched bus cycles, including slow accesses and HALT holds. */
static void expansion_continuity(void)
{
    static struct gate_capture capture[2];
    int16_t pcm[2][1000];
    for (uint32_t region = MUHTARI_REGION_NTSC; region <= MUHTARI_REGION_PAL; region++) {
        for (unsigned dual = 0u; dual < 2u; dual++) {
            for (uint32_t expansion = MUHTARI_EXPANSION_XBOARD; expansion <= MUHTARI_EXPANSION_XM;
                 expansion++) {
                memset(capture, 0, sizeof capture);
                muhtari *pair[2] = {map_machine(), map_machine()};
                size_t n = map_header(4u, 0u, dual != 0u ? 3u : 2u, MAP_BODY);
                /* A self-loop leaves the already configured chips alone. */
                map_image[128u] = 0x4Cu;
                map_image[129u] = 0u;
                map_image[130u] = 0xC0u;
                map_image[69] = dual != 0u ? 3u : 1u;
                muhtari_media media = {0};
                media.size = (uint32_t)sizeof media;
                media.image = map_image;
                media.image_size = n;
                media.region = region;
                media.expansion = expansion;
                for (unsigned i = 0u; i < 2u; i++) {
                    muhtari *m = pair[i];
                    CHECK_EQ_U64(muhtari__synth_observe(m, gate_record, &capture[i]),
                                 MUHTARI_STATUS_OK);
                    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
                    muhtari__bus_write(m, 1u, 7u);
                    muhtari__bus_write(m, 0x0470u, 16u);
                    for (unsigned chip = 0u; chip <= dual; chip++) {
                        uint16_t at = chip == 0u ? 0x0450u : 0x0440u;
                        muhtari__bus_write(m, (uint16_t)(at + 8u), 0x40u);
                        muhtari__bus_write(m, at, chip == 0u ? 37u : 53u);
                        muhtari__bus_write(m, (uint16_t)(at + 1u), 0xAFu);
                        muhtari__bus_write(m, (uint16_t)(at + 14u), 1u);
                        muhtari__bus_write(m, (uint16_t)(at + 9u), 0u);
                    }
                }
                uint64_t start = pair[0]->pokey[0].cycle;
                for (unsigned i = 0u; i < 512u; i++) {
                    for (unsigned member = 0u; member < 2u; member++) {
                        muhtari *m = pair[member];
                        if (i == 0u || i == 1u || i == 255u)
                            muhtari__bus_write(m, 0x0470u, member == 0u ? 0u : 16u);
                        if (i == 8u) {
                            /* A disabled store must not mute or enter INIT. */
                            muhtari__bus_write(m, member == 0u ? 0x0451u : 0x0461u, 0u);
                            muhtari__bus_write(m, member == 0u ? 0x045Fu : 0x046Fu, 0u);
                        }
                        if (i == 64u) {
                            m->bus.halt_gate_writes = 2u;
                            muhtari__bus_request_halt(m, m->bus.ticks, m->bus.ticks + 64u);
                        }
                        (void)muhtari__bus_read(m, i % 3u == 0u ? 0x0280u : 0x1800u);
                    }
                    CHECK(memcmp(pair[0]->pokey, pair[1]->pokey, sizeof pair[0]->pokey) == 0);
                    CHECK_EQ_U64(pair[0]->bus.irq_level, pair[1]->bus.irq_level);
                }
                CHECK(pair[0]->pokey[0].cycle > start + 512u);
                CHECK(pair[0]->pokey[0].channel[0].fires > 5u);
                CHECK_EQ_U64(pair[0]->bus.irq_level, 1u);
                CHECK_EQ_U64(pair[0]->pokey[0].channel[0].audc, 0xAFu);
                muhtari_input input = {0};
                input.size = (uint32_t)sizeof input;
                input.switches = MUHTARI_SWITCH_RESET;
                CHECK_EQ_U64(muhtari_set_input(pair[0], &input), MUHTARI_STATUS_OK);
                CHECK_EQ_U64(muhtari_set_input(pair[1], &input), MUHTARI_STATUS_OK);
                for (unsigned frame = 0u; frame < 2u; frame++) {
                    uint32_t samples[2];
                    for (unsigned i = 0u; i < 2u; i++) {
                        muhtari_frame f = {0};
                        f.size = (uint32_t)sizeof f;
                        f.audio = pcm[i];
                        f.audio_capacity = 1000u;
                        CHECK_EQ_U64(muhtari_run_frame(pair[i], &f), MUHTARI_STATUS_OK);
                        samples[i] = f.audio_count;
                    }
                    CHECK_EQ_U64(samples[0], samples[1]);
                    CHECK(memcmp(pcm[0], pcm[1], samples[0] * sizeof(int16_t)) == 0);
                    unsigned audible = 0u;
                    for (unsigned i = 0u; i < samples[0]; i++)
                        audible += pcm[0][i] != 0;
                    CHECK(audible != 0u);
                }
                for (unsigned i = 0u; i < 2u; i++) {
                    muhtari *m = pair[i];
                    muhtari__bus_write(m, 0x0470u, 16u);
                    CHECK_EQ_U64(m->synth.pokeys, 1u + dual);
                    CHECK_EQ_U64(m->synth.secondary, 0u);
                    CHECK_EQ_U64(m->cart.pokey_at[0], 0x0450u);
                    CHECK_EQ_U64(m->cart.pokey_at[1], dual != 0u ? 0x0440u : 0u);
                    CHECK_EQ_U64(m->cart.irq_mask, dual != 0u ? 3u : 1u);
                }
                CHECK_EQ_U64(muhtari__bus_read(pair[0], 0x045Au),
                             muhtari__bus_read(pair[1], 0x045Au));
                CHECK_EQ_U64(muhtari__bus_read(pair[0], 0x045Eu), 0xFEu);
                CHECK_EQ_U64(muhtari__bus_read(pair[1], 0x045Eu), 0xFEu);
                muhtari__observe_end(pair[0]);
                muhtari__observe_end(pair[1]);
                CHECK_EQ_U64(capture[0].count, capture[1].count);
                CHECK(capture[0].count > 100u);
                unsigned sources[10] = {0u};
                for (unsigned i = 0u; i < capture[0].count; i++) {
                    struct gate_record a = capture[0].records[i], b = capture[1].records[i];
                    CHECK_EQ_U64(a.tick, b.tick);
                    CHECK_EQ_U64(a.source, b.source);
                    CHECK_EQ_U64(a.raw, b.raw);
                    CHECK(a.source == 2u || (dual != 0u && a.source == 6u));
                    CHECK(a.raw == 0u || a.raw == 15u);
                    if (a.source < 10u)
                        sources[a.source]++;
                }
                CHECK(sources[2] > 100u);
                CHECK_EQ_U64(sources[6] != 0u, dual);
                CHECK_EQ_U64(muhtari_power_cycle(pair[0]), MUHTARI_STATUS_OK);
                CHECK_EQ_U64(pair[0]->cart.expansion.control, 0u);
                CHECK_EQ_U64(pair[0]->pokey[0].channel[0].audc, 0u);
                CHECK_EQ_U64(pair[0]->synth.raw[2], 0u);
                CHECK_EQ_U64(pair[0]->synth.pokeys, 1u + dual);
                muhtari_destroy(pair[0]);
                muhtari_destroy(pair[1]);
            }
        }
    }
}

static void map_normalization(void)
{
    expansion_continuity();
    muhtari *m = map_machine();
    /* Each individual primary can coexist with the secondary. Independent
       primary declarations are refused rather than silently discarded. */
    static const uint16_t types[] = {0x0440u, 0x0401u, 0x8400u};
    static const uint16_t at[] = {0x0450u, 0x4000u, 0x0800u};
    for (unsigned k = 0u; k < 3u; k++) {
        map_load(m, map_header(3u, types[k], 0u, MAP_BODY));
        CHECK_EQ_U64(m->synth.pokeys, 2u);
        CHECK_EQ_HEX(m->cart.pokey_at[0], at[k]);
        CHECK_EQ_HEX(m->cart.pokey_at[1], 0x0440u);
        static const uint16_t candidates[] = {0x0450u, 0x4000u, 0x0800u};
        for (unsigned j = 0u; j < 3u; j++) {
            muhtari__bus_write(m, at[k], 37u);
            muhtari__bus_write(m, candidates[j], 91u);
            CHECK_EQ_U64(m->pokey[0].channel[0].audf, j == k ? 91u : 37u);
        }
    }
    static const uint16_t primary_conflicts[] = {0x8441u, 0x8401u, 0x0041u};
    for (unsigned k = 0u; k < 3u; k++) {
        muhtari_media media;
        memset(&media, 0, sizeof media);
        media.size = (uint32_t)sizeof media;
        media.image = map_image;
        media.image_size = map_header(3u, primary_conflicts[k], 0u, MAP_BODY);
        muhtari_cart_info info;
        memset(&info, 0, sizeof info);
        info.size = (uint32_t)sizeof info;
        CHECK_EQ_U64(muhtari_probe(media.image, media.image_size, &info), MUHTARI_STATUS_OK);
        uint8_t *rom = m->cart.rom;
        CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE);
        CHECK(m->cart.rom == rom);
        CHECK_EQ_HEX(m->cart.pokey_at[0], 0x0800u);
        CHECK_EQ_HEX(m->cart.pokey_at[1], 0x0440u);
    }
    static const uint16_t conflicts[] = {0x0007u, 0x0013u, 0x0023u};
    for (unsigned k = 0u; k < 3u; k++) {
        map_load(m, map_header(3u, conflicts[k], 0u, 65536u));
        CHECK((m->cart.info.warnings & MUHTARI_CART_WARN_CART_TYPE_CONFLICT) != 0u);
        CHECK_EQ_U64(m->cart.info.sound, 0u);
        CHECK_EQ_U64(m->synth.pokeys, 0u);
        muhtari__bus_write(m, 0x4001u, 0x1Fu);
        CHECK_EQ_U64(m->pokey[0].channel[0].audc, 0u);
    }
    /* Supply an owned one-entry hash table to the existing identification
       seam, then install its result exactly as the bundled loader does. */
    size_t size = map_header(4u, 1u, 5u, MAP_BODY);
    map_image[69] = 2u;
    struct muhtari__cartdb_key key;
    memset(&key, 0, sizeof key);
    struct muhtari__sha1 hash;
    muhtari__sha1_init(&hash);
    muhtari__sha1_update(&hash, map_image + IMAGES_HEADER, MAP_BODY);
    muhtari__sha1_final(&hash, key.sha1);
    struct muhtari__cartdb_entry entry;
    memset(&entry, 0, sizeof entry);
    entry.board = MUHTARI_BOARD_LINEAR;
    entry.sound = MUHTARI_SOUND_POKEY_450 | MUHTARI_SOUND_POKEY_4000 | MUHTARI_SOUND_POKEY_800 |
                  MUHTARI_SOUND_POKEY_440;
    entry.flags = MUHTARI__CARTDB_NTSC;
    entry.parts = 1u;
    muhtari_cart_info info;
    CHECK_EQ_U64(
        muhtari__cart_identify(map_image, size, &key, 1u, &entry, 1u, NULL, 0u, NULL, 0u, &info),
        MUHTARI_STATUS_OK);
    CHECK_EQ_U64(info.source, MUHTARI_SOURCE_HASH);
    CHECK((info.warnings & MUHTARI_CART_WARN_HEADER_CORRECTED) != 0u);
    CHECK_EQ_U64(info.sound, entry.sound);
    CHECK_EQ_U64(muhtari__cart_install(m, map_image, &info, MUHTARI_EXPANSION_OFF),
                 MUHTARI_STATUS_OK);
    CHECK_EQ_HEX(m->cart.pokey_at[0], 0x0450u);
    CHECK_EQ_HEX(m->cart.pokey_at[1], 0x0440u);
    CHECK_EQ_U64(m->synth.pokeys, 2u);
    CHECK_EQ_U64(m->cart.irq_mask, 2u); /* only accepted header supplies it */
    muhtari__bus_write(m, 0x0451u, 0x1Fu);
    muhtari__bus_write(m, 0x0447u, 0x18u);
    CHECK_EQ_U64(m->synth.raw[2], 206u);
    CHECK_EQ_U64(m->synth.raw[9], 112u);
    map_load(m, map_header(3u, 0u, 0u, MAP_BODY));
    CHECK_EQ_U64(m->cart.irq_mask, 0u);
    muhtari_destroy(m);
}

static void map_wiring(void)
{
    muhtari *m = map_machine();
    /* Every accepted v4 code and mask, including the non-POKEY IRQ bit. */
    for (unsigned code = 0u; code <= 5u; code++) {
        for (unsigned irq = 0u; irq < 8u; irq++) {
            size_t size = map_header(4u, 0x440u, (uint8_t)code, MAP_BODY);
            map_image[69] = (uint8_t)irq;
            map_load(m, size);
            CHECK_EQ_U64(m->cart.irq_mask, irq & 3u);
            CHECK_EQ_U64(m->cart.info.a78_irq, irq);
            CHECK((m->cart.info.warnings & MUHTARI_CART_WARN_HEADER_V4_IGNORED) == 0u);
            if (code == 1u) {
                CHECK_EQ_U64(m->synth.pokeys, 1u);
                CHECK_EQ_U64(m->synth.secondary, 1u);
                CHECK_EQ_U64(m->cart.pokey_at[0], 0u);
                /* All four channels retain secondary source IDs6-9. */
                for (unsigned ch = 0u; ch < 4u; ch++) {
                    muhtari__bus_write(m, (uint16_t)(0x0441u + 2u * ch), 0x1Fu);
                    CHECK_EQ_U64(m->synth.raw[6u + ch], 206u);
                    CHECK_EQ_U64(m->synth.raw[2u + ch], 0u);
                }
            }
            CHECK_EQ_U64(muhtari_power_cycle(m), MUHTARI_STATUS_OK);
            CHECK_EQ_U64(m->cart.irq_mask, irq & 3u);
        }
    }
    /* Each invalid field and a geometrically invalid v4 block falls back
       to the legacy dual declaration, never to its raw IRQ metadata. */
    static const struct {
        unsigned offset;
        uint8_t value;
    } invalid[] = {{64u, 99u}, {65u, 4u}, {67u, 6u}, {67u, 0x40u}, {69u, 8u}, {68u, 1u}, {64u, 1u}};
    for (unsigned k = 0u; k < sizeof invalid / sizeof invalid[0]; k++) {
        size_t size = map_header(4u, 0x440u, 1u, MAP_BODY);
        map_image[69] = 3u;
        map_image[invalid[k].offset] = invalid[k].value;
        map_load(m, size);
        CHECK((m->cart.info.warnings & MUHTARI_CART_WARN_HEADER_V4_IGNORED) != 0u);
        CHECK_EQ_U64(m->cart.info.a78_irq, k == 4u ? 8u : k == 5u ? 259u : 3u);
        CHECK_EQ_U64(m->cart.irq_mask, 0u);
        CHECK_EQ_U64(m->synth.pokeys, 2u);
        CHECK_EQ_HEX(m->cart.pokey_at[0], 0x0450u);
        CHECK_EQ_HEX(m->cart.pokey_at[1], 0x0440u);
        CHECK_EQ_U64(muhtari_power_cycle(m), MUHTARI_STATUS_OK);
        CHECK_EQ_U64(m->cart.irq_mask, 0u);
    }
    /* Below v4 those bytes are reserved; above4 follows validated v4. */
    size_t size = map_header(3u, 0x400u, 1u, MAP_BODY);
    map_image[69] = 3u;
    map_load(m, size);
    CHECK_EQ_U64(m->cart.irq_mask, 0u);
    CHECK_EQ_U64(m->cart.info.a78_irq, 0u);
    map_image[0] = 5u;
    map_load(m, size);
    CHECK_EQ_U64(m->cart.irq_mask, 3u);
    CHECK((m->cart.info.warnings & MUHTARI_CART_WARN_VERSION_ABOVE_4) != 0u);
    map_load(m, map_header(4u, 0x440u, 0u, MAP_BODY));
    CHECK_EQ_U64(m->cart.irq_mask, 0u);
    CHECK_EQ_U64(m->synth.pokeys, 0u);
    for (unsigned source = 2u; source < 10u; source++)
        CHECK_EQ_U64(m->synth.raw[source], 0u);
    muhtari_destroy(m);
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        return 2;
    }
    if (strcmp(argv[1], "clock") == 0) {
        clock_vectors();
    } else if (strcmp(argv[1], "logic") == 0) {
        independence_vectors();
        volume_override_vectors();
        polynomial_vectors();
        init_vectors();
        joined_init_vectors();
        distortion_vectors();
        silent_state_vectors();
        joined_pcm_trace();
        period_vectors();
        reload_vectors();
        two_tone_vectors();
        timer_vectors();
        logic_vectors();
        high_pass_vectors();
    } else if (strcmp(argv[1], "maps") == 0) {
        map_windows();
        map_mono();
        map_accesses();
        map_overlaps();
        map_normalization();
        map_wiring();
    } else {
        return 2;
    }
    CHECK_DONE();
}
