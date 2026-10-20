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
    } else {
        return 2;
    }
    CHECK_DONE();
}
