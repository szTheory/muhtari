/* TIA documentary conformance against the shipped archive; tia.md. */
#include <stdlib.h>
#include <string.h>
#include "internal.h"
#include "../alloc.h"
#include "../check.h"

static muhtari *make(void)
{
    static alloc_counts counts;
    muhtari_config cfg = alloc_config(&counts);
    muhtari *m = NULL;
    CHECK_EQ_U64(muhtari_create(&cfg, &m), MUHTARI_STATUS_OK);
    return m;
}

static uint64_t tia_b_tick(unsigned n)
{
    return (uint64_t)(n / 2u) * 456u + ((n & 1u) == 0u ? 64u : 288u);
}

/* Documentary expectations, A7800-AUDIO section 2. Volume stays zero:
   hidden waveform state must keep running without synthesis events. */
static void test_tia_modes(void)
{
    static const unsigned periods[16] = {1u,   15u, 465u, 465u, 2u, 2u, 31u, 31u,
                                         511u, 31u, 31u,  1u,   6u, 6u, 93u, 93u};
    static const unsigned ones[16] = {1u,   8u,  248u, 248u, 1u, 1u, 18u, 16u,
                                      256u, 16u, 18u,  1u,   3u, 3u, 49u, 47u};
    /* Independently derived from TIA-1A revision E sheet 4; see tia.md.
       Sample after each complete enabled A/B pair from N=P=0. */
    static const char startup[16][41] = {
        "0001111111111111111111111111111111111111", "0001001101011110001001101011110001001101",
        "0000000000000000000000000000000000000000", "0000000000011100110000100111111000000000",
        "0001010101010101010101010101010101010101", "0001010101010101010101010101010101010101",
        "0000000000000000000000000000000000000000", "0000000000011101010000100101100111110001",
        "0000000010000100011000010011100101010110", "0000000010010110011111000110111010100001",
        "0000000000000000000000000000000000000000", "0000000000011111111111111111111111111111",
        "0001110001110001110001110001110001110001", "0001110001110001110001110001110001110001",
        "0000000000000000000000000000000000000000", "0000000000011111000000111100000111111111",
    };
    static const unsigned dividers[] = {0u, 1u, 5u, 31u};
    muhtari *m = make();
    for (unsigned mode = 0u; mode < 16u; mode++) {
        for (unsigned d = 0u; d < 4u; d++) {
            muhtari__tia_reset(m);
            muhtari__synth_reset(m, 0u, 0u);
            m->tia.channel[0].audc = (uint8_t)mode;
            m->tia.channel[0].audf = (uint8_t)dividers[d];
            unsigned scale = dividers[d] + 1u;
            unsigned warmup = 1024u * scale;
            unsigned period = periods[mode] * scale;
            uint8_t first[511u * 32u];
            unsigned sum = 0u;
            if (d == 0u) {
                for (unsigned i = 0u; i < 40u; i++) {
                    muhtari__tia_catch_up(m, tia_b_tick(i));
                    CHECK_EQ_U64(m->tia.channel[0].output, (unsigned)(startup[mode][i] - '0'));
                }
            }
            muhtari__tia_catch_up(m, tia_b_tick(warmup - 1u));
            for (unsigned i = 0u; i < period; i++) {
                muhtari__tia_catch_up(m, tia_b_tick(warmup + i));
                first[i] = m->tia.channel[0].output;
                sum += first[i];
            }
            CHECK_EQ_U64(sum, ones[mode] * scale);
            for (unsigned i = 0u; i < period; i++) {
                muhtari__tia_catch_up(m, tia_b_tick(warmup + period + i));
                if (first[i] != m->tia.channel[0].output) {
                    fprintf(stderr, "TIA mode %u AUDF %u period mismatch at %u\n", mode,
                            dividers[d], i);
                    check_failures++;
                    break;
                }
            }
            CHECK_EQ_U64(m->synth.pending, 0u);
        }
    }
    /* A different joint phase retains the other duty attractor. This seed
       is explanatory, not a claim about physical power-on (tia.md). */
    static const unsigned alternate_modes[] = {6u, 7u, 14u, 15u};
    static const unsigned alternate_ones[] = {13u, 15u, 44u, 46u};
    for (unsigned j = 0u; j < 4u; j++) {
        unsigned mode = alternate_modes[j], sum = 0u;
        muhtari__tia_reset(m);
        m->tia.channel[0].audc = (uint8_t)mode;
        m->tia.channel[0].noise = 21u;
        muhtari__tia_catch_up(m, tia_b_tick(1023u));
        for (unsigned i = 0u; i < periods[mode]; i++) {
            muhtari__tia_catch_up(m, tia_b_tick(1024u + i));
            sum += m->tia.channel[0].output;
        }
        CHECK_EQ_U64(sum, alternate_ones[j]);
    }
    muhtari__tia_reset(m);
    m->tia.channel[0].audc = 11u;
    m->tia.channel[0].noise = 21u;
    muhtari__tia_catch_up(m, tia_b_tick(8u));
    CHECK_EQ_U64(m->tia.channel[0].output, 0u);
    muhtari__tia_catch_up(m, tia_b_tick(9u));
    CHECK_EQ_U64(m->tia.channel[0].pulse, 15u);
    muhtari_destroy(m);
}

static void test_tia_controls(void)
{
    muhtari *m = make();
    muhtari__tia_reset(m);
    muhtari__synth_reset(m, 0u, 0u);
    m->tia.channel[0].audc = 4u;
    m->tia.channel[0].audf = 20u;
    muhtari__tia_catch_up(m, tia_b_tick(11u));
    CHECK_EQ_U64(m->tia.channel[0].count, 12u);
    muhtari__tia_write(m, 0x17u, 5u, tia_b_tick(11u) + 1u);
    muhtari__tia_catch_up(m, tia_b_tick(36u));
    CHECK_EQ_U64(m->tia.channel[0].pulse, 0u);
    CHECK_EQ_U64(m->tia.channel[0].count, 5u);
    muhtari__tia_catch_up(m, tia_b_tick(37u));
    CHECK_EQ_U64(m->tia.channel[0].pulse, 1u);
    CHECK_EQ_U64(m->tia.channel[0].count, 0u);

    /* A latches noise input and gate. A between-phase AUDC change affects
       B's pulse feedback, without changing those captured values. */
    muhtari__tia_reset(m);
    m->tia.channel[0].audc = 2u;
    muhtari__tia_catch_up(m, 0u);
    muhtari__tia_write(m, 0x15u, 0u, 1u);
    muhtari__tia_catch_up(m, 64u);
    CHECK_EQ_U64(m->tia.channel[0].noise, 1u);
    CHECK_EQ_U64(m->tia.channel[0].pulse, 0u);
    muhtari__tia_reset(m);
    m->tia.channel[0].audc = 1u;
    m->tia.channel[0].noise = 16u;
    muhtari__tia_catch_up(m, 0u);
    muhtari__tia_write(m, 0x15u, 8u, 1u);
    muhtari__tia_catch_up(m, 64u);
    CHECK_EQ_U64(m->tia.channel[0].noise, 1u);
    CHECK_EQ_U64(m->tia.channel[0].pulse, 1u);

    /* A due at the write tick uses the old AUDF; B still executes even
       when the register was lowered immediately after that compare. */
    muhtari__tia_reset(m);
    m->tia.channel[0].audc = 4u;
    muhtari__tia_write(m, 0x17u, 31u, 0u);
    muhtari__tia_catch_up(m, 64u);
    CHECK_EQ_U64(m->tia.channel[0].pulse, 1u);
    muhtari__tia_catch_up(m, 144u);
    CHECK_EQ_U64(m->tia.channel[0].enabled, 0u);

    /* Both channels, masked registers, immediate volume, no unchanged or
       initial-zero event. Same-tick B precedes the AUDV mutation. */
    muhtari__tia_reset(m);
    muhtari__synth_reset(m, 0u, 0u);
    for (unsigned ch = 0u; ch < 2u; ch++) {
        muhtari__tia_write(m, (uint16_t)(0x55u + ch), 0xF4u, 0u);
        muhtari__tia_write(m, (uint16_t)(0x57u + ch), 0xE0u, 0u);
        muhtari__tia_write(m, (uint16_t)(0x59u + ch), 0xFFu, 0u);
        CHECK_EQ_U64(m->tia.channel[ch].audc, 4u);
        CHECK_EQ_U64(m->tia.channel[ch].audf, 0u);
        CHECK_EQ_U64(m->tia.channel[ch].audv, 15u);
        m->tia.channel[ch].pulse = 4u;
    }
    CHECK_EQ_U64(m->synth.pending, 0u);
    muhtari__tia_write(m, 0x19u, 7u, 64u);
    CHECK_EQ_U64(m->synth.raw[0], 7u);
    CHECK_EQ_U64(m->synth.raw[1], 15u);
    CHECK_EQ_U64(m->synth.pending_tick, 64u);
    (void)muhtari__synth_render(m, 65u, NULL);
    muhtari__tia_write(m, 0x19u, 7u, 66u);
    CHECK_EQ_U64(m->synth.pending, 0u);

    /* Zero volume suppresses raw changes, not either hidden register.
       Enabling volume resumes exactly the already-running waveform. */
    muhtari *audible = make();
    muhtari__tia_reset(m);
    muhtari__tia_reset(audible);
    muhtari__synth_reset(m, 0u, 0u);
    muhtari__synth_reset(audible, 0u, 0u);
    m->tia.channel[0].audc = audible->tia.channel[0].audc = 8u;
    audible->tia.channel[0].audv = 15u;
    for (unsigned i = 0u; i < 70u; i++) {
        uint64_t tick = tia_b_tick(i);
        muhtari__tia_catch_up(m, tick);
        muhtari__tia_catch_up(audible, tick);
        CHECK_EQ_U64(m->tia.channel[0].noise, audible->tia.channel[0].noise);
        CHECK_EQ_U64(m->tia.channel[0].pulse, audible->tia.channel[0].pulse);
        CHECK_EQ_U64(m->tia.channel[0].count, audible->tia.channel[0].count);
        CHECK_EQ_U64(m->synth.pending, 0u);
    }
    muhtari__tia_write(m, 0x19u, 15u, tia_b_tick(69u) + 1u);
    CHECK_EQ_U64(m->synth.raw[0], audible->synth.raw[0]);
    muhtari_destroy(audible);

    /* RSYNC completes the old B at 64, then skips the new A at zero.
       Only future phases run; WSYNC uses that same new epoch. */
    muhtari__tia_reset(m);
    muhtari__synth_reset(m, 0u, 0u);
    m->tia.channel[0].audc = 4u;
    muhtari__tia_write(m, 0x03u, 0u, 64u);
    CHECK_EQ_U64(m->tia.channel[0].pulse, 1u);
    CHECK_EQ_U64(m->tia.next_tick, 128u);
    muhtari__tia_catch_up(m, 127u);
    CHECK_EQ_U64(m->tia.channel[0].pulse, 1u);
    muhtari__tia_catch_up(m, 128u);
    CHECK_EQ_U64(m->tia.channel[0].pulse, 2u);
    m->bus.rdy_release = 0u;
    muhtari__tia_write(m, 0x02u, 0u, 129u);
    CHECK_EQ_U64(m->bus.rdy_release, 520u);
    m->bus.rdy_release = 600u;
    muhtari__tia_write(m, 0x02u, 0u, 130u);
    CHECK_EQ_U64(m->bus.rdy_release, 600u);
    m->bus.rdy_release = 0u;
    muhtari__tia_write(m, 0x02u, 0u, 520u);
    CHECK_EQ_U64(m->bus.rdy_release, 976u);
    for (unsigned tick = 63u; tick <= 65u; tick++) {
        muhtari__tia_reset(m);
        m->tia.channel[0].audc = 4u;
        muhtari__tia_write(m, 0x03u, 0u, tick);
        CHECK_EQ_U64(m->tia.channel[0].pulse, tick < 64u ? 0u : 1u);
        muhtari__tia_catch_up(m, tick + 64u);
        CHECK_EQ_U64(m->tia.channel[0].pulse, tick < 64u ? 1u : 2u);
    }
    muhtari__tia_reset(m);
    m->tia.channel[0].audf = 1u;
    muhtari__tia_write(m, 0x03u, 0u, 64u);
    CHECK_EQ_U64(m->tia.channel[0].count, 1u);
    muhtari__tia_catch_up(m, 128u);
    CHECK_EQ_U64(m->tia.channel[0].count, 1u);
    CHECK_EQ_U64(m->tia.channel[0].pulse, 0u);
    muhtari__tia_catch_up(m, 352u);
    CHECK_EQ_U64(m->tia.channel[0].pulse, 1u);

    muhtari__tia_reset(m);
    m->bus.inptctrl = 7u;
    m->bus.rdy_release = 0u;
    m->bus.ticks = 59u;
    muhtari__bus_write(m, 0x03u, 0u);
    CHECK_EQ_U64(m->tia.epoch_tick, 64u);
    muhtari__bus_write(m, 0x02u, 0u);
    CHECK_EQ_U64(m->bus.rdy_release, 520u);
    (void)muhtari__bus_read(m, 0x1800u);
    CHECK_EQ_U64(m->bus.ticks, 527u);
    muhtari_destroy(m);
}

static void test_bus_pcm(void)
{
    int16_t audio[88];
    /* A decoded bus tone reaches production PCM. B phases alternate
       224/232 ticks; slow writes reach the DAC at +5, not +6
       (A7800-AUDIO section 2; docs/hardware/bus.md, Clocks). */
    muhtari *tone = make();
    muhtari__synth_reset(tone, 0u, 0u);
    tone->bus.inptctrl = 0x07u;
    muhtari__bus_write(tone, 0x15u, 4u);
    muhtari__bus_write(tone, 0x17u, 0u);
    muhtari__bus_write(tone, 0x19u, 15u);
    CHECK_EQ_U64(tone->synth.raw[0], 0u);
    muhtari__tia_catch_up(tone, 520u);
    CHECK_EQ_U64(tone->synth.raw[0], 0u);
    tone->bus.ticks = 739u;
    muhtari__bus_write(tone, 0x19u, 15u);
    CHECK_EQ_U64(tone->bus.ticks, 745u);
    CHECK_EQ_U64(tone->synth.raw[0], 15u);
    CHECK_EQ_U64(tone->synth.pending_tick, 744u);
    tone->bus.ticks = 745u;
    muhtari__bus_write(tone, 0x19u, 7u);
    CHECK_EQ_U64(tone->synth.raw[0], 7u);
    CHECK_EQ_U64(tone->synth.pending_tick, 750u);
    CHECK_EQ_U64(tone->bus.ticks, 751u);
    tone->bus.ticks = 971u;
    muhtari__bus_write(tone, 0x19u, 7u);
    CHECK_EQ_U64(tone->synth.raw[0], 0u);
    CHECK_EQ_U64(tone->synth.pending_tick, 976u);
    tone->bus.ticks = 1195u;
    muhtari__bus_write(tone, 0x19u, 7u);
    CHECK_EQ_U64(tone->synth.raw[0], 7u);
    CHECK_EQ_U64(tone->synth.pending_tick, 1200u);
    uint32_t tone_count = muhtari__synth_render(tone, 13125u, audio);
    int audible = 0;
    for (uint32_t i = 0u; i < tone_count; i++) {
        audible |= audio[i] != 0;
    }
    CHECK(audible);
    muhtari_destroy(tone);
}

int main(void)
{
    test_tia_modes();
    test_tia_controls();
    test_bus_pcm();
    return check_failures == 0 ? 0 : 1;
}
