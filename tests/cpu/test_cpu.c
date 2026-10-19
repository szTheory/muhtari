/* CPU rules on the flat test bus, one case per run:

     muhtari-test-cpu unstable.sally_constant
     muhtari-test-cpu decimal.clark FILE EXPECTED
     muhtari-test-cpu nmi.<case>

   Each case runs a program through the CPU object and checks what it leaves.
   decimal.clark loads FILE, an assembled tests/cpu/decimal/clark.s, at $2000,
   calls it and requires its ERROR byte to equal EXPECTED (0 or 1). Exit
   status: 0 when the case passes, 1 when it fails, 2 on a usage error or an
   unknown case. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"
#include "vector_bus.h"

static struct vector_machine machine;

/* Where every program is loaded and starts. */
#define PROGRAM_ORIGIN 0x0200u

/* Clears the machine, sets the ANE and LXA constant, loads the program at
   PROGRAM_ORIGIN and runs count instructions. */
static void run(uint8_t constant, const uint8_t *program, size_t size, uint32_t count)
{
    memset(&machine, 0, sizeof machine);
    machine.m.profile.ane_lxa_constant = constant;
    memcpy(&machine.ram[PROGRAM_ORIGIN], program, size);
    machine.m.cpu.pc = PROGRAM_ORIGIN;
    for (uint32_t i = 0u; i < count; i++) {
        vector_machine_reset_log(&machine);
        muhtari__cpu_step(&machine.m);
    }
}

static int expect_a(const char *program, uint8_t expected)
{
    if (machine.m.cpu.a != expected) {
        printf("%s: a expected $%02x got $%02x\n", program, (unsigned)expected,
               (unsigned)machine.m.cpu.a);
        return 0;
    }
    return 1;
}

/* ANE and LXA with the default profile's constant leave A = $00 where three
   real SALLYs read $00 (docs/hardware/sally.md, Unstable opcodes). The
   public vectors' $EE would leave $EE in both. */
static int unstable_sally_constant(void)
{
    /* One instruction per line. */
    /* clang-format off */
    static const uint8_t ane[] = {
        0xA9, 0x00, /* LDA #$00 */
        0xA2, 0xFF, /* LDX #$FF */
        0x8B, 0xFF, /* ANE #$FF */
    };
    static const uint8_t lxa[] = {
        0xA9, 0x00, /* LDA #$00 */
        0xAB, 0xFF, /* LXA #$FF */
    };
    /* clang-format on */
    int ok = 1;
    run(MUHTARI__ANE_LXA_SALLY, ane, sizeof ane, 3u);
    ok &= expect_a("LDA #$00; LDX #$FF; ANE #$FF", 0x00u);
    run(MUHTARI__ANE_LXA_SALLY, lxa, sizeof lxa, 2u);
    ok &= expect_a("LDA #$00; LXA #$FF", 0x00u);
    return ok;
}

static int run_unstable_sally_constant(char **args)
{
    (void)args;
    return unstable_sally_constant();
}

/* Bruce Clark's decimal mode test (tests/cpu/decimal/clark.s). Its 17 bytes
   of variables sit at $00-$10; ERROR is $04, N1 $07 and N2 $0A, and Y holds
   the carry in of the case under test. The listing says it takes about a
   minute at 1 MHz, so the limit is well above its run. */
#define CLARK_ORIGIN 0x2000u
#define CLARK_ERROR 0x04u
#define CLARK_N1 0x07u
#define CLARK_N2 0x0Au
#define CLARK_CYCLE_LIMIT 150000000u
/* TEST ends in RTS, which pulls the address below and adds one. */
#define CLARK_RETURN 0xFFF0u

static int decimal_clark(char **args)
{
    const char *path = args[0];
    char *end = NULL;
    unsigned long expected = strtoul(args[1], &end, 10);
    if (*args[1] == '\0' || *end != '\0' || expected > 1u) {
        fprintf(stderr, "decimal.clark: EXPECTED must be 0 or 1, got %s\n", args[1]);
        return 0;
    }
    memset(&machine, 0, sizeof machine);
    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        printf("decimal.clark: cannot open %s\n", path);
        return 0;
    }
    size_t cap = sizeof machine.ram - CLARK_ORIGIN;
    size_t size = fread(&machine.ram[CLARK_ORIGIN], 1u, cap, f);
    int extra = fgetc(f) != EOF;
    fclose(f);
    if (size == 0u || extra) {
        printf("decimal.clark: %s must hold 1 to %u bytes\n", path, (unsigned)cap);
        return 0;
    }
    /* The return address a JSR to TEST would have pushed: high byte first. */
    uint16_t pushed = (uint16_t)(CLARK_RETURN - 1u);
    machine.ram[0x01FFu] = (uint8_t)(pushed >> 8);
    machine.ram[0x01FEu] = (uint8_t)(pushed & 0xFFu);
    machine.m.cpu.s = 0xFDu;
    machine.m.cpu.pc = CLARK_ORIGIN;
    while (machine.m.cpu.pc != CLARK_RETURN) {
        if (machine.m.cpu.jammed || machine.m.bus.phi2 >= CLARK_CYCLE_LIMIT) {
            printf("decimal.clark: no return after %llu cycles (PC $%04x%s)\n",
                   (unsigned long long)machine.m.bus.phi2, (unsigned)machine.m.cpu.pc,
                   machine.m.cpu.jammed ? ", jammed" : "");
            return 0;
        }
        /* The flat bus logs every cycle; only the count matters here. */
        vector_machine_reset_log(&machine);
        muhtari__cpu_step(&machine.m);
    }
    uint8_t error = machine.ram[CLARK_ERROR];
    printf("decimal.clark: %s: ERROR %u after %llu cycles\n", path, (unsigned)error,
           (unsigned long long)machine.m.bus.phi2);
    if (error != expected) {
        printf("decimal.clark: ERROR expected %lu; stopped at N1 $%02x N2 $%02x C %u\n", expected,
               (unsigned)machine.ram[CLARK_N1], (unsigned)machine.ram[CLARK_N2],
               (unsigned)machine.m.cpu.y);
        return 0;
    }
    return 1;
}

/* NMI timing (docs/hardware/sally.md, NMI). A program runs from its origin
   with S = $FD while the flat bus latches an edge at the phi2 count nmi_at;
   with no holds, cycle k of the first instruction completes at phi2 k. Bus
   call hold_call, counted from 1, may be held for hold_count more phi2
   counts, as RDY or HALT would hold it. The
   NMI handler and the BRK handler are NOPs. Each case checks the return
   address the NMI pushed, and that the nearest wrong model would push a
   different one. */
#define NMI_HANDLER 0x0500u
#define BRK_HANDLER 0x0600u
#define NMI_STEP_LIMIT 8u

struct nmi_result {
    uint16_t ret; /* the return address the NMI pushed, 0 when not taken */
    uint8_t p;    /* the P it pushed */
};

static struct nmi_result nmi_run(uint16_t origin, const uint8_t *program, size_t size,
                                 uint64_t nmi_at, uint64_t hold_call, uint64_t hold_count)
{
    struct nmi_result r = {0u, 0u};
    memset(&machine, 0, sizeof machine);
    memcpy(&machine.ram[origin], program, size);
    memset(&machine.ram[NMI_HANDLER], 0xEA, 16u);
    memset(&machine.ram[BRK_HANDLER], 0xEA, 16u);
    machine.ram[0xFFFAu] = (uint8_t)(NMI_HANDLER & 0xFFu);
    machine.ram[0xFFFBu] = (uint8_t)(NMI_HANDLER >> 8);
    machine.ram[0xFFFEu] = (uint8_t)(BRK_HANDLER & 0xFFu);
    machine.ram[0xFFFFu] = (uint8_t)(BRK_HANDLER >> 8);
    machine.m.cpu.s = 0xFDu;
    machine.m.cpu.pc = origin;
    machine.nmi_at = nmi_at;
    machine.hold_call = hold_call;
    machine.hold_count = hold_count;
    for (uint32_t i = 0u; i < NMI_STEP_LIMIT; i++) {
        vector_machine_reset_log(&machine);
        muhtari__cpu_step(&machine.m);
        if (machine.m.cpu.pc == NMI_HANDLER) {
            uint8_t s = machine.m.cpu.s;
            r.p = machine.ram[0x0100u | (uint8_t)(s + 1u)];
            r.ret = (uint16_t)(machine.ram[0x0100u | (uint8_t)(s + 2u)] |
                               ((uint32_t)machine.ram[0x0100u | (uint8_t)(s + 3u)] << 8));
            return r;
        }
    }
    return r;
}

/* Fails when got is not expected, or when the wrong model named would push
   the same address, so that the case could not tell the two apart. */
static int expect_return(const char *what, uint16_t got, uint16_t expected, const char *model,
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

/* LDA abs takes 4 cycles; an edge in cycle 3 is taken after it. Polling
   only the edges latched before the instruction began would take it after
   the NOP. */
static int nmi_penultimate_taken_after(char **args)
{
    (void)args;
    /* clang-format off */
    static const uint8_t program[] = {
        0xAD, 0x34, 0x12, /* $0200 LDA $1234 */
        0xEA,             /* $0203 NOP */
        0xEA,             /* $0204 NOP */
    };
    /* clang-format on */
    struct nmi_result r = nmi_run(0x0200u, program, sizeof program, 3u, 0u, 0u);
    return expect_return("LDA $1234, edge in cycle 3", r.ret, 0x0203u,
                         "polling at the instruction's start", 0x0204u);
}

/* An edge in LDA abs's last cycle, cycle 4, is taken after the next
   instruction. Polling every edge latched by the instruction's end would
   take it after LDA. */
static int nmi_last_cycle_taken_after_next(char **args)
{
    (void)args;
    /* clang-format off */
    static const uint8_t program[] = {
        0xAD, 0x34, 0x12, /* $0200 LDA $1234 */
        0xEA,             /* $0203 NOP */
        0xEA,             /* $0204 NOP */
    };
    /* clang-format on */
    struct nmi_result r = nmi_run(0x0200u, program, sizeof program, 4u, 0u, 0u);
    return expect_return("LDA $1234, edge in cycle 4", r.ret, 0x0204u,
                         "polling at the instruction's end", 0x0203u);
}

/* A taken branch within its page decides at cycle 1 of 3, so an edge in its
   cycle 2 waits for the next instruction, where cycle N - 1 would take it
   after the branch. LDA zp, also 3 cycles, takes the same edge after LDA.
   A branch across a page decides at cycle 3 of 4. */
static int nmi_branch_no_cross_polls_early(char **args)
{
    (void)args;
    /* clang-format off */
    static const uint8_t no_cross[] = {
        0x90, 0x00, /* $0200 BCC $0202 (C is clear: taken, same page) */
        0xEA,       /* $0202 NOP */
        0xEA,       /* $0203 NOP */
    };
    static const uint8_t lda_zp[] = {
        0xA5, 0x10, /* $0200 LDA $10 */
        0xEA,       /* $0202 NOP */
        0xEA,       /* $0203 NOP */
    };
    static const uint8_t cross[] = {
        0x90, 0x01, /* $02FD BCC $0300 (taken, crosses into page 3) */
        0xEA,       /* $02FF NOP (skipped) */
        0xEA,       /* $0300 NOP */
        0xEA,       /* $0301 NOP */
    };
    /* clang-format on */
    static const struct {
        const char *what;
        uint16_t origin;
        const uint8_t *program;
        size_t size;
        uint64_t nmi_at;
        uint16_t expected;
        const char *model;
        uint16_t wrong;
    } cases[] = {
        {"BCC within the page, edge in cycle 1", 0x0200u, no_cross, sizeof no_cross, 1u, 0x0202u,
         "polling at the instruction's start", 0x0203u},
        {"BCC within the page, edge in cycle 2", 0x0200u, no_cross, sizeof no_cross, 2u, 0x0203u,
         "polling at cycle N - 1", 0x0202u},
        {"LDA $10, edge in cycle 2", 0x0200u, lda_zp, sizeof lda_zp, 2u, 0x0202u,
         "the branch's cycle 1 rule", 0x0203u},
        {"BCC across a page, edge in cycle 2", 0x02FDu, cross, sizeof cross, 2u, 0x0300u,
         "the branch's cycle 1 rule", 0x0301u},
        {"BCC across a page, edge in cycle 3", 0x02FDu, cross, sizeof cross, 3u, 0x0300u,
         "the branch's cycle 1 rule", 0x0301u},
        {"BCC across a page, edge in cycle 4", 0x02FDu, cross, sizeof cross, 4u, 0x0301u,
         "polling at the instruction's end", 0x0300u},
    };
    int ok = 1;
    for (size_t i = 0u; i < sizeof cases / sizeof cases[0]; i++) {
        struct nmi_result r =
            nmi_run(cases[i].origin, cases[i].program, cases[i].size, cases[i].nmi_at, 0u, 0u);
        ok &=
            expect_return(cases[i].what, r.ret, cases[i].expected, cases[i].model, cases[i].wrong);
    }
    return ok;
}

/* BRK decides at cycle 4 of 7. An edge by then turns it into the NMI: B
   stays set in the pushed P, the return address is BRK + 2 and the vector
   is $FFFA. An edge in cycle 5 runs the BRK handler's first instruction,
   then the NMI, which pushes P with B clear. Comparing at cycle 6 would
   hijack that BRK too. */
static int nmi_hijacks_brk(char **args)
{
    (void)args;
    /* clang-format off */
    static const uint8_t program[] = {
        0x00, 0xEA, /* $0200 BRK (and its skipped byte) */
        0xEA,       /* $0202 NOP */
    };
    /* clang-format on */
    int ok = 1;
    struct nmi_result r = nmi_run(0x0200u, program, sizeof program, 4u, 0u, 0u);
    ok &= expect_return("BRK, edge in cycle 4", r.ret, 0x0202u, "comparing at BRK's cycle 3",
                        0x0601u);
    if ((r.p & 0x10u) == 0u) {
        printf("BRK, edge in cycle 4: pushed P $%02x has B clear\n", (unsigned)r.p);
        ok = 0;
    }
    if (machine.m.bus.nmi_phi2 != 0u) {
        printf("BRK, edge in cycle 4: the edge was not consumed\n");
        ok = 0;
    }
    r = nmi_run(0x0200u, program, sizeof program, 5u, 0u, 0u);
    ok &= expect_return("BRK, edge in cycle 5", r.ret, 0x0601u, "comparing at BRK's cycle 6",
                        0x0202u);
    if ((r.p & 0x10u) != 0u) {
        printf("BRK, edge in cycle 5: the NMI pushed P $%02x with B set\n", (unsigned)r.p);
        ok = 0;
    }
    return ok;
}

/* CLC's opcode fetch is its deciding cycle. Held for 3 more phi2 counts,
   it completes at phi2 4, so an edge at phi2 2, in the hold, is taken after
   CLC. Counting from the instruction's start, cycle N - 1 would be phi2 1,
   and the edge would wait for the NOP. */
static int nmi_held_deciding_cycle_taken_after(char **args)
{
    (void)args;
    /* clang-format off */
    static const uint8_t program[] = {
        0x18, /* $0200 CLC */
        0xEA, /* $0201 NOP */
        0xEA, /* $0202 NOP */
    };
    /* clang-format on */
    struct nmi_result r = nmi_run(0x0200u, program, sizeof program, 2u, 1u, 3u);
    return expect_return("CLC, opcode fetch held, edge at phi2 2", r.ret, 0x0201u,
                         "counting from the instruction's start", 0x0202u);
}

/* LDA zp's deciding cycle is cycle 2, done at phi2 2. Its last cycle, held
   for 3 more counts, runs to phi2 6, so an edge at phi2 4, in the hold, is
   taken after the NOP. phi2 - 1 at LDA's end is 5, which would take it
   after LDA. */
static int nmi_held_last_cycle_taken_after_next(char **args)
{
    (void)args;
    /* clang-format off */
    static const uint8_t program[] = {
        0xA5, 0x10, /* $0200 LDA $10 */
        0xEA,       /* $0202 NOP */
        0xEA,       /* $0203 NOP */
    };
    /* clang-format on */
    struct nmi_result r = nmi_run(0x0200u, program, sizeof program, 4u, 3u, 3u);
    return expect_return("LDA $10, last cycle held, edge at phi2 4", r.ret, 0x0203u,
                         "phi2 - 1 at the instruction's end", 0x0202u);
}

static const struct {
    const char *name;
    int args; /* arguments after the case name */
    int (*run)(char **args);
} cases[] = {
    {"unstable.sally_constant", 0, run_unstable_sally_constant},
    {"decimal.clark", 2, decimal_clark},
    {"nmi.penultimate_taken_after", 0, nmi_penultimate_taken_after},
    {"nmi.last_cycle_taken_after_next", 0, nmi_last_cycle_taken_after_next},
    {"nmi.branch_no_cross_polls_early", 0, nmi_branch_no_cross_polls_early},
    {"nmi.hijacks_brk", 0, nmi_hijacks_brk},
    {"nmi.held_deciding_cycle_taken_after", 0, nmi_held_deciding_cycle_taken_after},
    {"nmi.held_last_cycle_taken_after_next", 0, nmi_held_last_cycle_taken_after_next},
};

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: muhtari-test-cpu <case> [args]\n");
        return 2;
    }
    for (size_t i = 0u; i < sizeof cases / sizeof cases[0]; i++) {
        if (strcmp(argv[1], cases[i].name) == 0) {
            if (argc - 2 != cases[i].args) {
                fprintf(stderr, "muhtari-test-cpu: %s takes %d arguments\n", argv[1],
                        cases[i].args);
                return 2;
            }
            return cases[i].run(&argv[2]) ? 0 : 1;
        }
    }
    fprintf(stderr, "muhtari-test-cpu: unknown case %s\n", argv[1]);
    return 2;
}
