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

/* With MEN set, the TIA and its mirrors, the RIOT's ports and its RAM take
   6 ticks, where a model with one cycle length gives 4; $0240, decoded by
   nothing, takes 4, where slowing the RIOT's whole page would give 6. */
static int cycle_slow_6_ticks(void)
{
    static const uint16_t slow[] = {0x0000u, 0x001Fu, 0x0100u, 0x0300u, 0x0280u, 0x0480u};
    muhtari *m = fresh(LOCKED_7800);
    int ok = 1;
    for (size_t i = 0u; i < sizeof slow / sizeof slow[0]; i++) {
        ok &= cycle_ticks(m, slow[i], 6u, "every cycle fast", 4u);
    }
    ok &= cycle_ticks(m, 0x0240u, 4u, "a slow RIOT page", 6u);
    return ok;
}

/* With MEN clear every cycle takes 6 ticks, where the 7800 decode would
   give 4 for these addresses. Writing 0 keeps MEN clear. */
static int cycle_men_off_all_slow(void)
{
    static const uint16_t addrs[] = {0x0040u, 0x1800u, 0x2000u, 0x0020u, 0x8000u, 0x0240u};
    muhtari *m = fresh(0x00u);
    int ok = 1;
    for (size_t i = 0u; i < sizeof addrs / sizeof addrs[0]; i++) {
        ok &= cycle_ticks(m, addrs[i], 6u, "the 7800 decode", 4u);
    }
    return ok;
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

/* Loads program at $1800 with S = $FD and points the CPU at it. */
static void load(muhtari *m, const uint8_t *program, size_t size)
{
    memcpy(&m->ram[0], program, size);
    m->cpu.pc = MUHTARI__RAM_BASE;
    m->cpu.s = 0xFDu;
}

/* Every TIA-range write writes INPTCTRL until LOCK is set; bits 4-7 are
   not stored, so $16 acts as $06. After the lock write, writes change
   nothing, and MEN stays set. */
static int inptctrl_lock(void)
{
    muhtari *m = fresh(0x00u);
    int ok = 1;
    muhtari__bus_write(m, 0x0001u, 0x16u);
    ok &= expect_byte("INPTCTRL after $16", m->bus.inptctrl, 0x06u, "storing 8 bits", 0x16u);
    muhtari__bus_write(m, 0x0100u, 0x07u);
    ok &= expect_byte("INPTCTRL after $07 at the $0100 mirror", m->bus.inptctrl, 0x07u,
                      "ignoring mirrors", 0x06u);
    muhtari__bus_write(m, 0x0001u, 0x00u);
    ok &= expect_byte("INPTCTRL after the lock", m->bus.inptctrl, 0x07u, "no lock", 0x00u);
    ok &= cycle_ticks(m, 0x1800u, 4u, "MEN cleared after the lock", 6u);
    return ok;
}

/* LDA $0240 reads an address nothing drives, so it returns the last byte on
   the bus: the operand's high byte, $02. A read after a write returns the
   written byte. A model of zeros gives $00 both times. */
static int open_bus_unmapped(void)
{
    /* clang-format off */
    static const uint8_t program[] = {
        0xAD, 0x40, 0x02, /* $1800 LDA $0240 */
    };
    /* clang-format on */
    muhtari *m = fresh(LOCKED_7800);
    int ok = 1;
    load(m, program, sizeof program);
    muhtari__cpu_step(m);
    ok &= expect_byte("LDA $0240", m->cpu.a, 0x02u, "zeros", 0x00u);
    muhtari__bus_write(m, 0x1900u, 0x5Au);
    ok &= expect_byte("read $0240 after writing $5a", muhtari__bus_read(m, 0x0240u), 0x5Au, "zeros",
                      0x00u);
    return ok;
}

/* Reads the TIA (or MARIA) register at addr right after a write of bus to
   RAM leaves bus on the data bus. */
static uint8_t read_after(muhtari *m, uint8_t bus, uint16_t addr)
{
    muhtari__bus_write(m, 0x1900u, bus);
    return muhtari__bus_read(m, addr);
}

/* The TIA drives D7-D6 of its collision registers and D7 of CXBLPF and
   INPT0-INPT5, with idle inputs: D7 = 0, except INPT4 and INPT5, whose
   buttons are up. Every other bit is the open-bus byte. MARIA's addresses
   return the open-bus byte. */
static int open_bus_write_only(void)
{
    muhtari *m = fresh(LOCKED_7800);
    int ok = 1;
    ok &= expect_byte("read $01 (CXM1P) after $ff", read_after(m, 0xFFu, 0x0001u), 0x3Fu,
                      "open bus", 0xFFu);
    ok &= expect_byte("read $0101 (CXM1P) after $ff", read_after(m, 0xFFu, 0x0101u), 0x3Fu,
                      "open bus", 0xFFu);
    ok &= expect_byte("read $06 (CXBLPF) after $ff", read_after(m, 0xFFu, 0x0006u), 0x7Fu,
                      "D7-D6 driven", 0x3Fu);
    ok &= expect_byte("read $08 (INPT0) after $ff", read_after(m, 0xFFu, 0x0008u), 0x7Fu,
                      "open bus", 0xFFu);
    ok &= expect_byte("read $0c (INPT4) after $00", read_after(m, 0x00u, 0x000Cu), 0x80u,
                      "open bus", 0x00u);
    ok &= expect_byte("read $28 (MARIA) after $a5", read_after(m, 0xA5u, 0x0028u), 0xA5u,
                      "the TIA's drivers", 0x25u);
    return ok;
}

/* A write to WSYNC ($02) holds RDY low until the TIA's line wraps: the
   first tick after the write's data tick, tick 5, at which tick plus
   tia_phase is a multiple of 456. A read then repeats whole 4-tick cycles
   until one starts at or after that tick, and runs in it; a write is not
   held. From tick 6 the read starts at 458 and ends at 462 with phi2 115
   (1 + 113 held + 1). */
static int rdy_holds_reads_only(void)
{
    int ok = 1;
    muhtari *m = fresh(LOCKED_7800);
    muhtari__bus_write(m, 0x0002u, 0x00u);
    (void)muhtari__bus_read(m, 0x1800u);
    ok &= expect_ticks("read after STA $02", m->bus.ticks, 462u, "no hold", 10u);
    ok &= expect_ticks("phi2 after the held read", m->bus.phi2, 115u, "held cycles uncounted", 2u);

    m = fresh(LOCKED_7800);
    muhtari__bus_write(m, 0x0002u, 0x00u);
    muhtari__bus_write(m, 0x1800u, 0x00u);
    ok &= expect_ticks("write after STA $02", m->bus.ticks, 10u, "writes held", 462u);
    (void)muhtari__bus_read(m, 0x1800u);
    ok &= expect_ticks("read after STA $02 and a write", m->bus.ticks, 462u, "the write releasing",
                       14u);

    /* With the TIA line 100 ticks in at tick 0, it wraps at tick 356; the
       read starts at 358. */
    m = fresh(LOCKED_7800);
    m->profile.tia_phase = 100u;
    muhtari__bus_write(m, 0x0002u, 0x00u);
    (void)muhtari__bus_read(m, 0x1800u);
    ok &= expect_ticks("read after STA $02, tia_phase 100", m->bus.ticks, 362u,
                       "ignoring tia_phase", 462u);
    return ok;
}

/* INC $02 reads $02, writes it back, then writes the result: both writes
   reach WSYNC and neither is held, so it takes 4 + 4 + 6 + 6 + 6 ticks and
   leaves its result, $03, on the bus. The next opcode fetch is held: NOP
   from tick 26 fetches at 458 and ends at 466. */
static int rdy_rmw_completes_both_writes(void)
{
    /* clang-format off */
    static const uint8_t program[] = {
        0xE6, 0x02, /* $1800 INC $02 */
        0xEA,       /* $1802 NOP */
    };
    /* clang-format on */
    muhtari *m = fresh(LOCKED_7800);
    int ok = 1;
    load(m, program, sizeof program);
    muhtari__cpu_step(m);
    ok &= expect_ticks("INC $02", m->bus.ticks, 26u, "the final write held", 462u);
    ok &= expect_byte("the bus after INC $02", m->bus.open_bus, 0x03u, "one write", 0x02u);
    muhtari__cpu_step(m);
    ok &= expect_ticks("NOP after INC $02", m->bus.ticks, 466u, "no hold", 34u);
    return ok;
}

static const struct {
    const char *name;
    int (*run)(void);
} cases[] = {
    {"cycle.fast_4_ticks", cycle_fast_4_ticks},
    {"cycle.slow_6_ticks", cycle_slow_6_ticks},
    {"cycle.men_off_all_slow", cycle_men_off_all_slow},
    {"rdy.holds_reads_only", rdy_holds_reads_only},
    {"rdy.rmw_completes_both_writes", rdy_rmw_completes_both_writes},
    {"inptctrl.lock", inptctrl_lock},
    {"open_bus.unmapped", open_bus_unmapped},
    {"open_bus.write_only", open_bus_write_only},
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
