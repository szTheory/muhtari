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
   the same value, so that the case could not tell the two apart. unit names
   what is counted. */
static int expect_count(const char *what, const char *unit, uint64_t got, uint64_t expected,
                        const char *model, uint64_t wrong)
{
    if (expected == wrong) {
        printf("%s: %s also gives %llu %s\n", what, model, (unsigned long long)expected, unit);
        return 0;
    }
    if (got != expected) {
        printf("%s: expected %llu %s got %llu (%s gives %llu)\n", what,
               (unsigned long long)expected, unit, (unsigned long long)got, model,
               (unsigned long long)wrong);
        return 0;
    }
    return 1;
}

static int expect_ticks(const char *what, uint64_t got, uint64_t expected, const char *model,
                        uint64_t wrong)
{
    return expect_count(what, "ticks", got, expected, model, wrong);
}

/* What request_halt or raise_nmi returned: 1 taken, 0 ignored. */
static int expect_taken(const char *what, int got, int expected, const char *model)
{
    return expect_count(what, "as return", (uint64_t)got, (uint64_t)expected, model,
                        (uint64_t)!expected);
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

/* Where a stack-page address sits in m->ram: $0140-$01FF is $2140-$21FF. */
static uint16_t ram_index_of(uint16_t addr)
{
    return (uint16_t)(addr + 0x2000u - MUHTARI__RAM_BASE);
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
   buttons are up. Every other bit is the open-bus byte. MARIA's MSTAT at
   $28 drives D7 alone, VBLANK, which is set on line 0 at power-on. */
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

/* The ticks one read at $1800 takes. */
static uint64_t read_ticks(muhtari *m)
{
    uint64_t before = m->bus.ticks;
    (void)muhtari__bus_read(m, 0x1800u);
    return m->bus.ticks - before;
}

/* Requests HALT now for 3 cycles and returns the ticks the next three reads
   take: 12 when the request is ignored, 4 + 4 + (4 + 12) when it is taken. */
static uint64_t halt_three_reads(muhtari *m)
{
    uint64_t before = m->bus.ticks;
    muhtari__bus_request_halt(m, m->bus.ticks, m->bus.ticks + 12u);
    for (int i = 0; i < 3; i++) {
        (void)muhtari__bus_read(m, 0x1800u);
    }
    return m->bus.ticks - before;
}

/* HALT reaches SALLY only once two unlocked writes in the TIA range have
   been counted, whatever their data and at any mirror; writes after LOCK do
   not count. */
static int halt_gate_two_writes(void)
{
    int ok = 1;
    muhtari *m = fresh(0x02u);
    ok &= expect_taken("a request with the gate shut", muhtari__bus_request_halt(m, 0u, 12u), 0,
                       "an open gate");
    ok &= expect_ticks("HALT after no write", halt_three_reads(m), 12u, "an open gate", 24u);
    muhtari__bus_write(m, 0x0001u, 0x02u);
    ok &= expect_ticks("HALT after one write", halt_three_reads(m), 12u, "one write opening", 24u);
    muhtari__bus_write(m, 0x0101u, 0xFEu);
    ok &= expect_ticks("HALT after a second write at $0101", halt_three_reads(m), 24u,
                       "mirrors not counted", 12u);

    m = fresh(0x02u);
    muhtari__bus_write(m, 0x0001u, 0x03u);
    muhtari__bus_write(m, 0x0001u, 0x02u);
    ok &= expect_ticks("HALT after a lock write and one more", halt_three_reads(m), 12u,
                       "locked writes counted", 24u);
    return ok;
}

/* INC $06 from $1800 reads $06 (CXBLPF: D7 idle 0, the rest the operand
   left on the bus, $06), writes $06 back, then writes $07. Both writes are
   unlocked TIA writes, so the gate counts 2 and INPTCTRL ends $07. LDA $06
   writes nothing and STA $06 writes once. */
static int halt_gate_counts_rmw_dummy(void)
{
    /* clang-format off */
    static const uint8_t inc[] = {
        0xE6, 0x06, /* $1800 INC $06 */
    };
    static const uint8_t lda[] = {
        0xA5, 0x06, /* $1800 LDA $06 */
    };
    static const uint8_t sta[] = {
        0x85, 0x06, /* $1800 STA $06 */
    };
    /* clang-format on */
    int ok = 1;
    muhtari *m = fresh(0x02u);
    load(m, inc, sizeof inc);
    muhtari__cpu_step(m);
    ok &= expect_count("gate after INC $06", "writes", m->bus.halt_gate_writes, 2u,
                       "the dummy write not counted", 1u);
    ok &=
        expect_byte("INPTCTRL after INC $06", m->bus.inptctrl, 0x07u, "INPTCTRL left alone", 0x02u);
    m = fresh(0x02u);
    load(m, lda, sizeof lda);
    muhtari__cpu_step(m);
    ok &= expect_count("gate after LDA $06", "writes", m->bus.halt_gate_writes, 0u, "reads counted",
                       1u);
    m = fresh(0x02u);
    load(m, sta, sizeof sta);
    m->cpu.a = 0x02u;
    muhtari__cpu_step(m);
    ok &= expect_count("gate after STA $06", "writes", m->bus.halt_gate_writes, 1u,
                       "the operand fetch counted", 2u);
    return ok;
}

/* A request at a phi2 fall: the cycle starting there (c0) and the next (c1)
   run in 4 ticks, and c2 is frozen for the m = 5 cycles HALT is held, 4 + 20
   ticks, each counted in phi2. */
static int halt_latency_two_cycles(void)
{
    int ok = 1;
    muhtari *m = fresh(LOCKED_7800);
    m->bus.halt_gate_writes = 2u;
    (void)muhtari__bus_read(m, 0x1800u);
    muhtari__bus_request_halt(m, 4u, 24u);
    ok &= expect_ticks("c0", read_ticks(m), 4u, "no latency", 24u);
    ok &= expect_ticks("c1", read_ticks(m), 4u, "one cycle of latency", 24u);
    ok &= expect_ticks("c2", read_ticks(m), 24u, "no freeze", 4u);
    ok &= expect_count("phi2 after c2", "cycles", m->bus.phi2, 9u, "frozen cycles uncounted", 4u);
    return ok;
}

/* A request at tick 2, inside a 6-tick cycle, takes effect at that cycle's
   end: c0 starts at 6, and c2 is frozen for the 3 cycles from 6 to 18.
   Counting the cycle in flight as c0 would freeze the second fast read. */
static int halt_waits_for_slow_fall(void)
{
    int ok = 1;
    muhtari *m = fresh(LOCKED_7800);
    m->bus.halt_gate_writes = 2u;
    muhtari__bus_request_halt(m, 2u, 18u);
    uint64_t before = m->bus.ticks;
    (void)muhtari__bus_read(m, 0x0000u);
    ok &= expect_ticks("the slow cycle in flight", m->bus.ticks - before, 6u,
                       "freezing the cycle in flight", 18u);
    ok &= expect_ticks("c0", read_ticks(m), 4u, "the in-flight cycle as c0", 16u);
    ok &= expect_ticks("c1", read_ticks(m), 4u, "the in-flight cycle as c0", 16u);
    ok &= expect_ticks("c2", read_ticks(m), 16u, "no freeze", 4u);
    return ok;
}

/* HALT freezes a write as it freezes a read: c2 as a write takes 4 + 20
   ticks. */
static int halt_freezes_writes(void)
{
    int ok = 1;
    muhtari *m = fresh(LOCKED_7800);
    m->bus.halt_gate_writes = 2u;
    (void)muhtari__bus_read(m, 0x1800u);
    muhtari__bus_request_halt(m, 4u, 24u);
    ok &= expect_ticks("c0", read_ticks(m), 4u, "no latency", 24u);
    ok &= expect_ticks("c1", read_ticks(m), 4u, "one cycle of latency", 24u);
    uint64_t before = m->bus.ticks;
    muhtari__bus_write(m, 0x1800u, 0x00u);
    ok &= expect_ticks("c2, a write", m->bus.ticks - before, 24u, "writes never frozen", 4u);
    return ok;
}

/* One window at a time. A request made after /HALT falls, before c2's hold,
   is ignored and returns 0: c2 still freezes for the first window's 5
   cycles. A request made before the first window's fall is ignored too, so
   /HALT falls at tick 8, not 4. Once a window ends, the next request is
   taken. */
static int halt_one_window_at_a_time(void)
{
    int ok = 1;
    muhtari *m = fresh(LOCKED_7800);
    m->bus.halt_gate_writes = 2u;
    (void)muhtari__bus_read(m, 0x1800u);
    ok &= expect_taken("the first request", muhtari__bus_request_halt(m, 4u, 24u), 1,
                       "every request ignored");
    ok &= expect_ticks("c0", read_ticks(m), 4u, "no latency", 24u);
    ok &= expect_taken("a second request after the fall",
                       muhtari__bus_request_halt(m, m->bus.ticks, m->bus.ticks + 40u), 0,
                       "the second request replacing the first");
    ok &= expect_ticks("c1, a second request after the fall", read_ticks(m), 4u,
                       "one cycle of latency", 24u);
    ok &= expect_ticks("c2, a second request after the fall", read_ticks(m), 24u,
                       "the second request replacing the first", 4u);
    ok &= expect_ticks("a request after the window", halt_three_reads(m), 24u,
                       "requests ignored after a window", 12u);

    m = fresh(LOCKED_7800);
    m->bus.halt_gate_writes = 2u;
    (void)muhtari__bus_read(m, 0x1800u);
    ok &= expect_taken("the first request, at tick 8", muhtari__bus_request_halt(m, 8u, 28u), 1,
                       "every request ignored");
    ok &= expect_taken("a second request before the fall", muhtari__bus_request_halt(m, 4u, 24u), 0,
                       "the second request replacing the first");
    ok &= expect_ticks("the cycle before the fall", read_ticks(m), 4u,
                       "freezing the cycle before the fall", 24u);
    ok &=
        expect_ticks("c0, a second request before the fall", read_ticks(m), 4u, "no latency", 24u);
    ok &= expect_ticks("c1, a second request before the fall", read_ticks(m), 4u,
                       "the second request replacing the first", 24u);
    ok &= expect_ticks("c2, a second request before the fall", read_ticks(m), 24u,
                       "the second request replacing the first", 4u);
    return ok;
}

/* An edge raised at a tick is latched by the first cycle whose data tick
   falls at or after it, and nmi_phi2 names that cycle. With MEN clear
   nothing is latched. */
static int nmi_latched_in_cycle(void)
{
    static const struct {
        const char *what;
        uint64_t at;
        uint64_t expected;
        const char *model;
        uint64_t wrong;
    } cases[] = {
        {"edge at tick 6, inside cycle 2", 6u, 2u, "latching at the next cycle", 3u},
        {"edge at tick 8, cycle 2's fall", 8u, 2u, "latching after the fall", 3u},
        {"edge at tick 13, the slow cycle 3's fall", 13u, 3u, "a slow cycle's data at its tick 4",
         4u},
        {"edge at tick 14, the slow cycle 3's end", 14u, 4u, "latching by the cycle's end", 3u},
    };
    int ok = 1;
    for (size_t i = 0u; i < sizeof cases / sizeof cases[0]; i++) {
        muhtari *m = fresh(LOCKED_7800);
        muhtari__bus_raise_nmi(m, cases[i].at);
        (void)muhtari__bus_read(m, 0x1800u);
        (void)muhtari__bus_read(m, 0x1800u);
        (void)muhtari__bus_read(m, 0x0000u);
        (void)muhtari__bus_read(m, 0x1800u);
        ok &= expect_count(cases[i].what, "as nmi_phi2", m->bus.nmi_phi2, cases[i].expected,
                           cases[i].model, cases[i].wrong);
    }
    muhtari *m = fresh(0x00u);
    ok &= expect_taken("edge with MEN clear", muhtari__bus_raise_nmi(m, 2u), 0, "no MEN gate");
    (void)muhtari__bus_read(m, 0x1800u);
    ok &=
        expect_count("edge with MEN clear", "as nmi_phi2", m->bus.nmi_phi2, 0u, "no MEN gate", 1u);
    return ok;
}

/* One edge at a time: an edge at tick 14, raised while the edge at tick 6
   is not yet latched, is ignored and returns 0, so cycle 2 latches the
   first. */
static int nmi_one_edge_at_a_time(void)
{
    int ok = 1;
    muhtari *m = fresh(LOCKED_7800);
    ok &= expect_taken("the first edge", muhtari__bus_raise_nmi(m, 6u), 1, "every edge ignored");
    ok &= expect_taken("a second edge before the first is latched", muhtari__bus_raise_nmi(m, 14u),
                       0, "the second edge replacing the first");
    for (int i = 0; i < 4; i++) {
        (void)muhtari__bus_read(m, 0x1800u);
    }
    ok &= expect_count("a second edge before the first is latched", "as nmi_phi2", m->bus.nmi_phi2,
                       2u, "the second edge replacing the first", 4u);
    return ok;
}

static int expect_address(const char *what, uint16_t got, uint16_t expected, const char *model,
                          uint16_t wrong)
{
    if (expected == wrong) {
        printf("%s: %s also gives $%04x\n", what, model, (unsigned)expected);
        return 0;
    }
    if (got != expected) {
        printf("%s: return address expected $%04x got $%04x (%s gives $%04x)\n", what,
               (unsigned)expected, (unsigned)got, model, (unsigned)wrong);
        return 0;
    }
    return 1;
}

/* The return address an NMI entry pushed, read from the stack page's RAM,
   or 0 when S shows no entry. */
static uint16_t nmi_return(const muhtari *m)
{
    if (m->cpu.s != 0xFAu) {
        return 0u;
    }
    uint16_t lo = ram_index_of(0x01FCu);
    uint16_t hi = ram_index_of(0x01FDu);
    return (uint16_t)(m->ram[lo] | ((uint32_t)m->ram[hi] << 8));
}

/* LDA $1900 takes cycles 1-4 from tick 0; an edge at tick 14 falls in
   cycle 4, its last, so it is taken after the NOP that follows. */
static int nmi_last_cycle_after_next(void)
{
    /* clang-format off */
    static const uint8_t program[] = {
        0xAD, 0x00, 0x19, /* $1800 LDA $1900 */
        0xEA,             /* $1803 NOP */
        0xEA,             /* $1804 NOP */
    };
    /* clang-format on */
    muhtari *m = fresh(LOCKED_7800);
    load(m, program, sizeof program);
    muhtari__bus_raise_nmi(m, 14u);
    muhtari__cpu_step(m);
    muhtari__cpu_step(m);
    return expect_address("LDA $1900, edge at tick 14", nmi_return(m), 0x1804u,
                          "taking it after LDA", 0x1803u);
}

/* STA $02 from $1800 runs from tick 0 to 14 and pulls RDY low until tick
   456, so CLC's opcode fetch is held in cycles 4 to 114. An edge at tick
   100 falls in held cycle 25, before the fetch completes at phi2 115, so it
   is taken after CLC. Counting from CLC's start would compare against phi2
   4 and take it after the NOP. */
static int nmi_held_fetch_after_wsync(void)
{
    /* clang-format off */
    static const uint8_t program[] = {
        0x85, 0x02, /* $1800 STA $02 */
        0x18,       /* $1802 CLC */
        0xEA,       /* $1803 NOP */
        0xEA,       /* $1804 NOP */
    };
    /* clang-format on */
    int ok = 1;
    muhtari *m = fresh(LOCKED_7800);
    (void)muhtari__bus_read(m, 0x1800u);
    (void)muhtari__bus_read(m, 0x1801u);
    muhtari__bus_write(m, 0x0002u, 0x00u);
    muhtari__bus_raise_nmi(m, 100u);
    (void)muhtari__bus_read(m, 0x1802u);
    ok &= expect_count("edge at tick 100 in the held fetch", "as nmi_phi2", m->bus.nmi_phi2, 25u,
                       "the completing cycle's count", 115u);

    m = fresh(LOCKED_7800);
    load(m, program, sizeof program);
    muhtari__bus_raise_nmi(m, 100u);
    muhtari__cpu_step(m);
    muhtari__cpu_step(m);
    ok &= expect_address("STA $02; CLC, edge at tick 100", nmi_return(m), 0x1803u,
                         "counting from CLC's start", 0x1804u);
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
    {"halt.gate_two_writes", halt_gate_two_writes},
    {"halt.gate_counts_rmw_dummy", halt_gate_counts_rmw_dummy},
    {"halt.latency_two_cycles", halt_latency_two_cycles},
    {"halt.waits_for_slow_fall", halt_waits_for_slow_fall},
    {"halt.freezes_writes", halt_freezes_writes},
    {"halt.one_window_at_a_time", halt_one_window_at_a_time},
    {"nmi.latched_in_cycle", nmi_latched_in_cycle},
    {"nmi.last_cycle_after_next", nmi_last_cycle_after_next},
    {"nmi.held_fetch_after_wsync", nmi_held_fetch_after_wsync},
    {"nmi.one_edge_at_a_time", nmi_one_edge_at_a_time},
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
