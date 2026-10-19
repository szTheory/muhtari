/* The bus through the shipped archive, one case per run:

     muhtari-test-bus <case>

   Each case acts as the CPU: it calls the two bus functions on an instance
   in its power-on state and reads m->bus.ticks around each call, or runs a
   hand-assembled program through the CPU from RAM at $1800. Every timing
   check also requires that the nearest wrong model would give a different
   result, so a failure prints the expected value, the actual one and the
   wrong model's (docs/hardware/bus.md). Exit status: 0 when the case
   passes, 1 when it fails, 2 on a usage error or an unknown case. */
#include <stdio.h>
#include <string.h>

#include "internal.h"

static muhtari machine;

/* The power-on state with INPTCTRL set to inptctrl and the HALT gate's
   count at 0. */
static muhtari *fresh(uint8_t inptctrl)
{
    memset(&machine, 0, sizeof machine);
    muhtari__power_on(&machine);
    machine.bus.inptctrl = inptctrl;
    return &machine;
}

/* MEN set and INPTCTRL locked, as a 7800 game leaves it. */
#define LOCKED_7800 0x07u

/* Fails when got is not expected, or when the wrong model named would give
   the same value, so that the case could not tell the two apart. */
static int expect_ticks(const char *what, uint64_t got, uint64_t expected, const char *model,
                        uint64_t wrong)
{
    if (expected == wrong) {
        printf("%s: %s also gives %llu ticks\n", what, model, (unsigned long long)expected);
        return 0;
    }
    if (got != expected) {
        printf("%s: expected %llu ticks got %llu (%s gives %llu)\n", what,
               (unsigned long long)expected, (unsigned long long)got, model,
               (unsigned long long)wrong);
        return 0;
    }
    return 1;
}

/* The ticks one read and one write at addr take. */
static int cycle_ticks(muhtari *m, uint16_t addr, uint64_t expected, const char *model,
                       uint64_t wrong)
{
    char what[48];
    int ok = 1;
    uint64_t before = m->bus.ticks;
    (void)muhtari__bus_read(m, addr);
    snprintf(what, sizeof what, "read $%04x", (unsigned)addr);
    ok &= expect_ticks(what, m->bus.ticks - before, expected, model, wrong);
    before = m->bus.ticks;
    muhtari__bus_write(m, addr, 0x00u);
    snprintf(what, sizeof what, "write $%04x", (unsigned)addr);
    ok &= expect_ticks(what, m->bus.ticks - before, expected, model, wrong);
    return ok;
}

/* With MEN set, RAM, its page-0 window, MARIA and the cartridge space take
   4 ticks; a model with every cycle slow gives 6. */
static int cycle_fast_4_ticks(void)
{
    static const uint16_t fast[] = {0x0040u, 0x1800u, 0x2000u, 0x0020u, 0x8000u};
    muhtari *m = fresh(LOCKED_7800);
    int ok = 1;
    for (size_t i = 0u; i < sizeof fast / sizeof fast[0]; i++) {
        ok &= cycle_ticks(m, fast[i], 4u, "every cycle slow", 6u);
    }
    return ok;
}

static const struct {
    const char *name;
    int (*run)(void);
} cases[] = {
    {"cycle.fast_4_ticks", cycle_fast_4_ticks},
};

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: muhtari-test-bus <case>\n");
        return 2;
    }
    for (size_t i = 0u; i < sizeof cases / sizeof cases[0]; i++) {
        if (strcmp(argv[1], cases[i].name) == 0) {
            return cases[i].run() ? 0 : 1;
        }
    }
    fprintf(stderr, "muhtari-test-bus: unknown case %s\n", argv[1]);
    return 2;
}
