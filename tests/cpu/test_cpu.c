/* CPU rules on the flat test bus, one case per run:

     muhtari-test-cpu unstable.sally_constant
     muhtari-test-cpu decimal.clark FILE EXPECTED

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

static const struct {
    const char *name;
    int args; /* arguments after the case name */
    int (*run)(char **args);
} cases[] = {
    {"unstable.sally_constant", 0, run_unstable_sally_constant},
    {"decimal.clark", 2, decimal_clark},
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
