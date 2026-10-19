/* CPU rules on the flat test bus, one case per run:

     muhtari-test-cpu CASE

   Each case runs a hand-assembled program through the CPU object and checks
   the registers it leaves. Exit status: 0 when the case passes, 1 when it
   fails, 2 on a usage error or an unknown case. */
#include <stdio.h>
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

static const struct {
    const char *name;
    int (*run)(void);
} cases[] = {
    {"unstable.sally_constant", unstable_sally_constant},
};

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: muhtari-test-cpu <case>\n");
        return 2;
    }
    for (size_t i = 0u; i < sizeof cases / sizeof cases[0]; i++) {
        if (strcmp(argv[1], cases[i].name) == 0) {
            return cases[i].run() ? 0 : 1;
        }
    }
    fprintf(stderr, "muhtari-test-cpu: unknown case %s\n", argv[1]);
    return 2;
}
