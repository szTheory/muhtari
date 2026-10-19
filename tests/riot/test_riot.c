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
#define INTIM 0x0284u
#define TIM1T 0x0294u

/* The power-on state under the default profile, with INPTCTRL set to
   inptctrl. */
static muhtari *fresh(uint8_t inptctrl)
{
    memset(&machine, 0, sizeof machine);
    machine.profile.riot_orb_power_on = 0xFFu;
    muhtari__power_on(&machine);
    machine.bus.inptctrl = inptctrl;
    return &machine;
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

static const struct {
    const char *name;
    int (*run)(void);
} cases[] = {
    {"timer.reads_n_minus_1", timer_reads_n_minus_1},
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
