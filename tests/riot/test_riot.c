/* The RIOT through the shipped archive's bus, one case per run:

     muhtari-test-riot <case>

   Each case acts as the CPU: it calls the two bus functions on an instance
   in its power-on state, so every access takes the bus's own cycle and the
   timer sees the bus's own phi2 count. Every check also requires that the
   nearest wrong model would give a different result, and a failure prints
   the expected value, the actual one and the wrong model's
   (docs/hardware/riot.md). Exit status: 0 when the case passes, 1 when it
   fails, 2 on a usage error or an unknown case. */
#include <stdio.h>
#include <string.h>

#include "internal.h"

static muhtari machine;

/* MEN set and INPTCTRL locked, as a 7800 game leaves it. */
#define LOCKED_7800 0x07u

/* The RIOT's registers as a 7800 game addresses them. */
#define SWCHA 0x0280u
#define CTLSWA 0x0281u
#define SWCHB 0x0282u
#define CTLSWB 0x0283u
#define INTIM 0x0284u
#define TIMINT 0x0285u
#define TIM1T 0x0294u

/* TIMINT's two flags. */
#define FLAG_TIMER 0x80u
#define FLAG_PA7 0x40u

/* The power-on state under the default profile, with INPTCTRL set to
   inptctrl. Both difficulty switches are at A and nothing is pressed, so
   every input pin of both ports reads high (docs/hardware/controllers.md,
   Switches). */
static muhtari *fresh(uint8_t inptctrl)
{
    memset(&machine, 0, sizeof machine);
    machine.profile.riot_orb_power_on = 0xFFu;
    machine.ports.switches = MUHTARI_SWITCH_LEFT_DIFFICULTY_A | MUHTARI_SWITCH_RIGHT_DIFFICULTY_A;
    muhtari__power_on(&machine);
    machine.bus.inptctrl = inptctrl;
    return &machine;
}

static int expect_count(const char *what, uint64_t got, uint64_t expected, const char *model,
                        uint64_t wrong)
{
    if (expected == wrong) {
        printf("%s: %s also gives %llu\n", what, model, (unsigned long long)expected);
        return 0;
    }
    if (got != expected) {
        printf("%s: expected %llu got %llu (%s gives %llu)\n", what, (unsigned long long)expected,
               (unsigned long long)got, model, (unsigned long long)wrong);
        return 0;
    }
    return 1;
}

static int expect_byte(const char *what, uint8_t got, uint8_t expected, const char *model,
                       uint8_t wrong)
{
    if (expected == wrong) {
        printf("%s: %s also gives $%02x\n", what, model, (unsigned)expected);
        return 0;
    }
    if (got != expected) {
        printf("%s: expected $%02x got $%02x (%s gives $%02x)\n", what, (unsigned)expected,
               (unsigned)got, model, (unsigned)wrong);
        return 0;
    }
    return 1;
}

/* Runs n cycles that touch nothing but system RAM. */
static void idle(muhtari *m, uint64_t n)
{
    for (uint64_t i = 0u; i < n; i++) {
        (void)muhtari__bus_read(m, 0x1800u);
    }
}

/* The four intervals, as the timer write's A1-A0 select them. */
static const struct {
    const char *name;
    uint16_t addr;
    uint64_t cycles;
} intervals[] = {
    {"1T", TIM1T, 1u},
    {"8T", TIM1T + 1u, 8u},
    {"64T", TIM1T + 2u, 64u},
    {"1024T", TIM1T + 3u, 1024u},
};

#define INTERVALS (sizeof intervals / sizeof intervals[0])

/* The datasheet's worked example writes 52. */
#define N 52u

/* Writes N at interval i on a fresh machine: the writing cycle is p = 0,
   and the next access is p = 1. */
static muhtari *written(size_t i)
{
    muhtari *m = fresh(LOCKED_7800);
    muhtari__bus_write(m, intervals[i].addr, N);
    return m;
}

/* INTIM read at p cycles after the write of N at interval i. */
static uint8_t intim_at(size_t i, uint64_t p)
{
    muhtari *m = written(i);
    idle(m, p - 1u);
    return muhtari__bus_read(m, INTIM);
}

/* A read right after the write gives N - 1, where a timer that loads N and
   counts on the interval would give N; a read at p = I still gives N - 1,
   where counting p / I intervals gives N - 2; and a read at p = N I gives
   0, where the same model gives $FF. */
static int timer_reads_n_minus_1(void)
{
    int ok = 1;
    char what[64];
    for (size_t i = 0u; i < INTERVALS; i++) {
        uint64_t cycles = intervals[i].cycles;
        snprintf(what, sizeof what, "%s: INTIM at p = 1", intervals[i].name);
        ok &= expect_byte(what, intim_at(i, 1u), N - 1u, "reading N at once", N);
        if (cycles > 1u) {
            snprintf(what, sizeof what, "%s: INTIM at p = I", intervals[i].name);
            ok &= expect_byte(what, intim_at(i, cycles), N - 1u, "N - 1 - p / I", N - 2u);
        }
        snprintf(what, sizeof what, "%s: INTIM at p = N I", intervals[i].name);
        ok &= expect_byte(what, intim_at(i, N * cycles), 0x00u, "N - 1 - p / I", 0xFFu);
    }
    return ok;
}

/* The p the next access will be, for a timer written in phi2 cycle w. */
static uint64_t next_p(const muhtari *m, uint64_t w)
{
    return m->bus.phi2 + 1u - w;
}

/* Runs RAM cycles until the next access is p = target, and returns how
   many it ran. Every access before target must be at an earlier p. */
static uint64_t idle_to(muhtari *m, uint64_t w, uint64_t target)
{
    uint64_t n = 0u;
    while (next_p(m, w) < target) {
        (void)muhtari__bus_read(m, 0x1800u);
        n++;
    }
    return n;
}

/* TIMINT read at p cycles after the write of N at interval i. */
static uint8_t timint_at(size_t i, uint64_t p)
{
    muhtari *m = written(i);
    idle(m, p - 1u);
    return muhtari__bus_read(m, TIMINT);
}

/* The flag is clear at p = N I, as the timer reads 0, and set at N I + 1,
   the datasheet's "(52 x 8) + 1 = 417T". */
static int timer_flag_at_n_interval_plus_1(void)
{
    int ok = 1;
    char what[64];
    for (size_t i = 0u; i < INTERVALS; i++) {
        uint64_t full = N * intervals[i].cycles;
        snprintf(what, sizeof what, "%s: TIMINT at p = N I", intervals[i].name);
        ok &= expect_byte(what, timint_at(i, full), 0x00u, "the flag at N I", FLAG_TIMER);
        snprintf(what, sizeof what, "%s: TIMINT at p = N I + 1", intervals[i].name);
        ok &= expect_byte(what, timint_at(i, full + 1u), FLAG_TIMER, "the flag at N I + 2", 0x00u);
    }
    return ok;
}

/* From the flag the timer reads $FF and counts one per cycle, on through
   0 and every later wrap. The datasheet's step 5 reads $AC at p = 500 on
   8T, where counting from its figure's "#416" gives $AB. */
static int timer_counts_past_underflow(void)
{
    int ok = 1;
    char what[64];
    for (size_t i = 0u; i < INTERVALS; i++) {
        uint64_t full = N * intervals[i].cycles;
        snprintf(what, sizeof what, "%s: INTIM at p = N I + 1", intervals[i].name);
        ok &= expect_byte(what, intim_at(i, full + 1u), 0xFFu, "holding 0", 0x00u);
        if (intervals[i].cycles > 1u) {
            snprintf(what, sizeof what, "%s: INTIM at p = N I + 2", intervals[i].name);
            ok &= expect_byte(what, intim_at(i, full + 2u), 0xFEu, "keeping the interval", 0xFFu);
        }
        snprintf(what, sizeof what, "%s: INTIM at p = N I + 257", intervals[i].name);
        ok &= expect_byte(what, intim_at(i, full + 257u), 0xFFu, "stopping at $00", 0x00u);
    }
    ok &= expect_byte("8T: INTIM at p = 500", intim_at(1u, 500u), 0xACu, "the flag at #416", 0xABu);
    return ok;
}

/* A timer read in the cycle the flag sets, p = N I + 1, leaves it set. */
static int timer_read_in_setting_cycle_keeps_flag(void)
{
    int ok = 1;
    char what[64];
    for (size_t i = 0u; i < INTERVALS; i++) {
        uint64_t full = N * intervals[i].cycles;
        muhtari *m = written(i);
        idle(m, full);
        (void)muhtari__bus_read(m, INTIM);
        snprintf(what, sizeof what, "%s: TIMINT after INTIM at p = N I + 1", intervals[i].name);
        ok &= expect_byte(what, muhtari__bus_read(m, TIMINT), FLAG_TIMER, "every read clearing",
                          0x00u);
    }
    return ok;
}

/* A timer read after the setting cycle clears the flag, and so does a
   timer write; reading TIMINT does not. A write after a clear starts a
   timer whose flag sets again. */
static int timer_access_clears_flag(void)
{
    int ok = 1;
    char what[64];
    for (size_t i = 0u; i < INTERVALS; i++) {
        uint64_t full = N * intervals[i].cycles;
        muhtari *m = written(i);
        idle(m, full + 1u);
        snprintf(what, sizeof what, "%s: TIMINT twice from p = N I + 2", intervals[i].name);
        (void)muhtari__bus_read(m, TIMINT);
        ok &= expect_byte(what, muhtari__bus_read(m, TIMINT), FLAG_TIMER, "TIMINT clearing it",
                          0x00u);
        (void)muhtari__bus_read(m, INTIM);
        snprintf(what, sizeof what, "%s: TIMINT after a later INTIM read", intervals[i].name);
        ok &= expect_byte(what, muhtari__bus_read(m, TIMINT), 0x00u, "reads not clearing",
                          FLAG_TIMER);

        m = written(i);
        idle(m, full + 1u);
        muhtari__bus_write(m, intervals[i].addr, N);
        snprintf(what, sizeof what, "%s: TIMINT after a timer write", intervals[i].name);
        ok &= expect_byte(what, muhtari__bus_read(m, TIMINT), 0x00u, "writes not clearing",
                          FLAG_TIMER);

        /* A read clears the flag; the next write's timer sets it again. */
        m = written(i);
        idle(m, full + 1u);
        (void)muhtari__bus_read(m, INTIM);
        muhtari__bus_write(m, intervals[i].addr, N);
        idle(m, full);
        snprintf(what, sizeof what, "%s: TIMINT at N I + 1 after a clear and a write",
                 intervals[i].name);
        ok &= expect_byte(what, muhtari__bus_read(m, TIMINT), FLAG_TIMER,
                          "the clear outliving the write", 0x00u);
    }
    return ok;
}

/* With HALT open (two unlocked TIA-range writes), a HALT for half the
   timer's cycles from the write freezes the third read after it; the held
   cycles count, so the flag sets at p = N I + 1 after fewer accesses, where
   counting accesses leaves it clear. */
static int timer_counts_through_halt(void)
{
    int ok = 1;
    char what[64];
    for (size_t i = 0u; i < INTERVALS; i++) {
        uint64_t full = N * intervals[i].cycles;
        uint64_t held = full / 2u;
        muhtari *m = fresh(0x02u);
        muhtari__bus_write(m, 0x0001u, 0x02u);
        muhtari__bus_write(m, 0x0001u, 0x02u);
        muhtari__bus_write(m, intervals[i].addr, N);
        uint64_t w = m->bus.phi2;
        muhtari__bus_request_halt(m, m->bus.ticks, m->bus.ticks + 4u * held);
        uint64_t reads = idle_to(m, w, full);
        snprintf(what, sizeof what, "%s: TIMINT at p = N I through HALT", intervals[i].name);
        ok &= expect_byte(what, muhtari__bus_read(m, TIMINT), 0x00u, "the flag at N I", FLAG_TIMER);
        snprintf(what, sizeof what, "%s: TIMINT at p = N I + 1 through HALT", intervals[i].name);
        ok &=
            expect_byte(what, muhtari__bus_read(m, TIMINT), FLAG_TIMER, "counting accesses", 0x00u);
        snprintf(what, sizeof what, "%s: accesses to p = N I through HALT", intervals[i].name);
        ok &= expect_count(what, reads + 1u, full - held, "no HALT", full);
    }
    return ok;
}

/* STA $02 holds the next read until the TIA's line wraps, over 100 cycles;
   the held cycles count, so with 200 written the flag sets at p = 200 I + 1
   after fewer accesses, where counting accesses leaves it clear. */
static int timer_counts_through_wsync(void)
{
    int ok = 1;
    char what[64];
    for (size_t i = 0u; i < INTERVALS; i++) {
        uint64_t full = 200u * intervals[i].cycles;
        muhtari *m = fresh(LOCKED_7800);
        muhtari__bus_write(m, intervals[i].addr, 200u);
        uint64_t w = m->bus.phi2;
        muhtari__bus_write(m, 0x0002u, 0x00u);
        uint64_t reads = idle_to(m, w, full);
        snprintf(what, sizeof what, "%s: TIMINT at p = 200 I through WSYNC", intervals[i].name);
        ok &= expect_byte(what, muhtari__bus_read(m, TIMINT), 0x00u, "the flag at N I", FLAG_TIMER);
        snprintf(what, sizeof what, "%s: TIMINT at p = 200 I + 1 through WSYNC", intervals[i].name);
        ok &=
            expect_byte(what, muhtari__bus_read(m, TIMINT), FLAG_TIMER, "counting accesses", 0x00u);
        if (reads + 2u >= full) {
            printf("%s: %llu accesses to p = 200 I through WSYNC: no cycle was held\n",
                   intervals[i].name, (unsigned long long)(reads + 2u));
            ok = 0;
        }
    }
    return ok;
}

/* A 6-tick cycle counts one, like a 4-tick one: after N I - 1 slow reads
   of RIOT RAM the flag is still clear at p = N I and sets at N I + 1, where
   counting 4-tick time sets it a third of the way early. */
static int timer_slow_cycle_counts_one(void)
{
    int ok = 1;
    char what[64];
    for (size_t i = 0u; i < INTERVALS; i++) {
        uint64_t full = N * intervals[i].cycles;
        muhtari *m = written(i);
        uint64_t start = m->bus.ticks;
        for (uint64_t k = 1u; k < full; k++) {
            (void)muhtari__bus_read(m, 0x0480u);
        }
        snprintf(what, sizeof what, "%s: ticks of N I - 1 slow reads", intervals[i].name);
        ok &= expect_count(what, m->bus.ticks - start, 6u * (full - 1u), "4-tick cycles",
                           4u * (full - 1u));
        snprintf(what, sizeof what, "%s: TIMINT at p = N I after slow reads", intervals[i].name);
        ok &= expect_byte(what, muhtari__bus_read(m, TIMINT), 0x00u, "counting 4-tick time",
                          FLAG_TIMER);
        snprintf(what, sizeof what, "%s: TIMINT at p = N I + 1 after slow reads",
                 intervals[i].name);
        ok &= expect_byte(what, muhtari__bus_read(m, TIMINT), FLAG_TIMER, "the flag at N I + 2",
                          0x00u);
    }
    return ok;
}

/* TIMINT read with $ff left on the bus, so undriven bits would show. */
static uint8_t timint(muhtari *m)
{
    muhtari__bus_write(m, 0x1900u, 0xFFu);
    return muhtari__bus_read(m, TIMINT);
}

/* PA7 driven as an output: ORA then DDRA make it a high output, which is
   no edge. Driving it low sets the PA7 flag (D6, with D5-D0 = 0), and
   reading TIMINT clears it. After the write to $0285 selects the rising
   edge and TIMINT is read, driving it high sets the flag and driving it
   low again does not. */
static int pa7_edge_flag(void)
{
    int ok = 1;
    muhtari *m = fresh(LOCKED_7800);
    muhtari__bus_write(m, SWCHA, 0x80u);
    muhtari__bus_write(m, CTLSWA, 0x80u);
    ok &= expect_byte("TIMINT after PA7 becomes a high output", timint(m), 0x00u, "an edge on DDRA",
                      FLAG_PA7);
    muhtari__bus_write(m, SWCHA, 0x00u);
    uint8_t v = timint(m);
    ok &= expect_byte("TIMINT after PA7 falls", v, FLAG_PA7, "no edge on an output", 0x00u);
    ok &= expect_byte("TIMINT after PA7 falls", v, FLAG_PA7, "D5-D0 from the bus", 0x7Fu);
    ok &= expect_byte("TIMINT read again", timint(m), 0x00u, "reads not clearing", FLAG_PA7);
    muhtari__bus_write(m, 0x0285u, 0x00u);
    (void)timint(m);
    muhtari__bus_write(m, SWCHA, 0x80u);
    ok &= expect_byte("TIMINT after PA7 rises on the rising edge", timint(m), FLAG_PA7,
                      "a falling edge only", 0x00u);
    muhtari__bus_write(m, SWCHA, 0x00u);
    ok &= expect_byte("TIMINT after PA7 falls on the rising edge", timint(m), 0x00u, "either edge",
                      FLAG_PA7);
    return ok;
}

/* SWCHA reads ORA on its output pins and high on its input pins. */
static int swcha_ddr(void)
{
    int ok = 1;
    muhtari *m = fresh(LOCKED_7800);
    muhtari__bus_write(m, CTLSWA, 0xF0u);
    muhtari__bus_write(m, SWCHA, 0xA5u);
    uint8_t v = muhtari__bus_read(m, SWCHA);
    ok &= expect_byte("SWCHA, DDRA $f0, ORA $a5", v, 0xAFu, "reading ORA", 0xA5u);
    ok &= expect_byte("SWCHA, DDRA $f0, ORA $a5", v, 0xAFu, "inputs reading low", 0xA0u);
    ok &= expect_byte("CTLSWA", muhtari__bus_read(m, CTLSWA), 0xF0u, "reading the pins", 0xFFu);
    return ok;
}

/* SWCHB reads ORB on its output pins and high on its input pins. */
static int swchb_ddr(void)
{
    int ok = 1;
    muhtari *m = fresh(LOCKED_7800);
    muhtari__bus_write(m, CTLSWB, 0x14u);
    muhtari__bus_write(m, SWCHB, 0x00u);
    uint8_t v = muhtari__bus_read(m, SWCHB);
    ok &= expect_byte("SWCHB, DDRB $14, ORB $00", v, 0xEBu, "reading ORB", 0x00u);
    ok &= expect_byte("SWCHB, DDRB $14, ORB $00", v, 0xEBu, "reading the pins", 0xFFu);
    ok &= expect_byte("CTLSWB", muhtari__bus_read(m, CTLSWB), 0x14u, "reading the pins", 0xFFu);
    return ok;
}

/* With no write at all: the timer reads $FF and counts on 1024T, both flags
   are clear, DDRA, ORA and DDRB are $00 and ORB is $FF, so CTLSWB $14 alone
   leaves SWCHB $FF; under a profile whose ORB powers on $00 it reads $EB. */
static int power_on_defaults(void)
{
    int ok = 1;
    muhtari *m = fresh(LOCKED_7800);
    ok &=
        expect_byte("INTIM at p = 1", muhtari__bus_read(m, INTIM), 0xFFu, "a zeroed timer", 0x00u);
    idle(m, 1023u);
    ok &= expect_byte("INTIM at p = 1025", muhtari__bus_read(m, INTIM), 0xFEu, "1T", 0xFFu);
    ok &= expect_byte("TIMINT", muhtari__bus_read(m, TIMINT), 0x00u, "flags set", 0xC0u);
    ok &= expect_byte("CTLSWA", muhtari__bus_read(m, CTLSWA), 0x00u, "outputs", 0xFFu);
    ok &= expect_byte("CTLSWB", muhtari__bus_read(m, CTLSWB), 0x00u, "outputs", 0xFFu);
    muhtari__bus_write(m, CTLSWA, 0xFFu);
    ok &= expect_byte("SWCHA after DDRA $ff", muhtari__bus_read(m, SWCHA), 0x00u, "ORA $ff", 0xFFu);
    muhtari__bus_write(m, CTLSWB, 0x14u);
    ok &= expect_byte("SWCHB after CTLSWB $14", muhtari__bus_read(m, SWCHB), 0xFFu,
                      "the datasheet's zero ORB", 0xEBu);

    m->profile.riot_orb_power_on = 0x00u;
    muhtari__power_on(m);
    m->bus.inptctrl = LOCKED_7800;
    muhtari__bus_write(m, CTLSWB, 0x14u);
    ok &= expect_byte("SWCHB after CTLSWB $14, ORB powering on $00", muhtari__bus_read(m, SWCHB),
                      0xEBu, "ignoring the profile", 0xFFu);
    return ok;
}

/* $0480-$04FF hold 128 bytes, mirrored at $0580, in 6-tick cycles, and
   power-on zeroes them. */
static int ram_window(void)
{
    int ok = 1;
    muhtari *m = fresh(LOCKED_7800);
    for (uint16_t i = 0u; i < 128u; i++) {
        muhtari__bus_write(m, (uint16_t)(0x0480u + i), (uint8_t)(0x80u + i));
    }
    uint8_t mismatched = 0u;
    for (uint16_t i = 0u; i < 128u; i++) {
        if (muhtari__bus_read(m, (uint16_t)(0x0580u + i)) != (uint8_t)(0x80u + i)) {
            mismatched++;
        }
    }
    ok &= expect_count("bytes at $0580-$05ff differing from $0480-$04ff", mismatched, 0u,
                       "open bus", 128u);
    uint64_t before = m->bus.ticks;
    (void)muhtari__bus_read(m, 0x04FFu);
    ok &= expect_count("ticks of a read of $04ff", m->bus.ticks - before, 6u, "a fast cycle", 4u);
    muhtari__power_on(m);
    m->bus.inptctrl = LOCKED_7800;
    muhtari__bus_write(m, 0x1900u, 0xFFu);
    ok &= expect_byte("$0480 after power-on", muhtari__bus_read(m, 0x0480u), 0x00u, "RAM kept",
                      0x80u);
    return ok;
}

static const struct {
    const char *name;
    int (*run)(void);
} cases[] = {
    {"timer.reads_n_minus_1", timer_reads_n_minus_1},
    {"timer.flag_at_n_interval_plus_1", timer_flag_at_n_interval_plus_1},
    {"timer.counts_past_underflow", timer_counts_past_underflow},
    {"timer.read_in_setting_cycle_keeps_flag", timer_read_in_setting_cycle_keeps_flag},
    {"timer.access_clears_flag", timer_access_clears_flag},
    {"timer.counts_through_halt", timer_counts_through_halt},
    {"timer.counts_through_wsync", timer_counts_through_wsync},
    {"timer.slow_cycle_counts_one", timer_slow_cycle_counts_one},
    {"pa7.edge_flag", pa7_edge_flag},
    {"swcha.ddr", swcha_ddr},
    {"swchb.ddr", swchb_ddr},
    {"power_on.defaults", power_on_defaults},
    {"ram.window", ram_window},
};

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: muhtari-test-riot <case>\n");
        return 2;
    }
    for (size_t i = 0u; i < sizeof cases / sizeof cases[0]; i++) {
        if (strcmp(argv[1], cases[i].name) == 0) {
            return cases[i].run() ? 0 : 1;
        }
    }
    fprintf(stderr, "muhtari-test-riot: unknown case %s\n", argv[1]);
    return 2;
}
