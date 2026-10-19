/* The vector harness: runs SingleStepTests 6502 vectors through the CPU
   object on the flat test bus, and compares each test's final registers,
   raw P with B, RAM and every bus cycle.

     muhtari-test-vectors FILE XX [--first N]

   FILE is an N65V file or one upstream XX.json file. An N65V file whose
   first chunk is opcode 00 is the sample, 256 chunks for opcodes 00 to ff;
   any other N65V file is one chunk. A .json file must hold all 10000 tests,
   or with --first N its first N tests are run and what follows is not read,
   so a byte prefix of an upstream file runs. Only opcode XX runs, but every
   test read is validated. Each failing test prints one line,
   "6502/XX.json[i]: <first mismatch>", in file order; the first failing test
   also prints its expected and actual cycles. Every test also checks the
   CPU's NMI poll mark (docs/hardware/sally.md, NMI): after a JAM, with an
   edge latched during its reads, that the CPU took no NMI and left the mark
   alone; after any other opcode, that the mark is the phi2 count of the
   deciding cycle. The last line is
   "6502/XX: F of N failed". Exit status: 0 when every test passed, 1 on a
   failed test or a bad file, 2 on a usage error. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"
#include "n65v.h"
#include "sst_json.h"
#include "vector_bus.h"

static struct vector_machine machine;
static struct n65v_test test;
static struct n65v_reader reader;

static int usage(void)
{
    fprintf(stderr, "usage: muhtari-test-vectors <file.n65v|XX.json> <opcode-hex> [--first N]\n");
    return 2;
}

static const char *kind_name(uint8_t kind)
{
    return kind == N65V_KIND_WRITE ? "write" : "read";
}

/* Prints "6502/XX.json[i]: <field> expected $e got $g", with width hex
   digits. */
static void mismatch(uint8_t opcode, const char *field, int width, unsigned expected, unsigned got)
{
    printf("6502/%02x.json[%u]: %s expected $%0*x got $%0*x\n", (unsigned)opcode,
           (unsigned)test.index, field, width, expected, width, got);
}

static void print_cycles(uint8_t opcode, const char *which, const struct n65v_cycle *c,
                         uint32_t count)
{
    printf("6502/%02x.json[%u]: %s cycles:\n", (unsigned)opcode, (unsigned)test.index, which);
    for (uint32_t i = 0u; i < count; i++) {
        printf("  %2u $%04x $%02x %s\n", (unsigned)i, (unsigned)c[i].addr, (unsigned)c[i].value,
               kind_name(c[i].kind));
    }
}

/* Compares the machine with the test's final state and cycles; prints the
   first mismatch and returns 0, or returns 1 when everything matches. */
static int compare(uint8_t opcode)
{
    const struct n65v_state *out = &test.final;
    const struct muhtari__cpu *cpu = &machine.m.cpu;
    char field[48];

    if (cpu->pc != out->pc) {
        mismatch(opcode, "pc", 4, out->pc, cpu->pc);
        return 0;
    }
    const struct {
        const char *name;
        uint8_t expected, got;
    } regs[] = {{"s", out->s, cpu->s},
                {"a", out->a, cpu->a},
                {"x", out->x, cpu->x},
                {"y", out->y, cpu->y},
                {"p", out->p, cpu->p}};
    for (size_t i = 0u; i < sizeof regs / sizeof regs[0]; i++) {
        if (regs[i].expected != regs[i].got) {
            mismatch(opcode, regs[i].name, 2, regs[i].expected, regs[i].got);
            return 0;
        }
    }
    for (uint32_t i = 0u; i < out->ram_count; i++) {
        uint8_t got = machine.ram[out->ram_addr[i]];
        if (got != out->ram_value[i]) {
            snprintf(field, sizeof field, "ram[$%04x]", (unsigned)out->ram_addr[i]);
            mismatch(opcode, field, 2, out->ram_value[i], got);
            return 0;
        }
    }
    if (machine.log_overflow) {
        printf("6502/%02x.json[%u]: more than %u cycles\n", (unsigned)opcode, (unsigned)test.index,
               VECTOR_LOG_CAPACITY);
        return 0;
    }
    if (machine.log_count != test.cycle_count) {
        printf("6502/%02x.json[%u]: cycle count expected %u got %u\n", (unsigned)opcode,
               (unsigned)test.index, (unsigned)test.cycle_count, (unsigned)machine.log_count);
        return 0;
    }
    for (uint32_t i = 0u; i < test.cycle_count; i++) {
        const struct n65v_cycle *e = &test.cycles[i];
        const struct n65v_cycle *g = &machine.log[i];
        if (e->addr != g->addr) {
            snprintf(field, sizeof field, "cycle %u addr", (unsigned)i);
            mismatch(opcode, field, 4, e->addr, g->addr);
            return 0;
        }
        if (e->value != g->value) {
            snprintf(field, sizeof field, "cycle %u value", (unsigned)i);
            mismatch(opcode, field, 2, e->value, g->value);
            return 0;
        }
        if (e->kind != g->kind) {
            printf("6502/%02x.json[%u]: cycle %u kind expected %s got %s\n", (unsigned)opcode,
                   (unsigned)test.index, (unsigned)i, kind_name(e->kind), kind_name(g->kind));
            return 0;
        }
    }
    return 1;
}

/* The twelve JAM opcodes: the only ones that leave the CPU jammed. */
static int is_jam(uint8_t opcode)
{
    return (opcode & 0x0Fu) == 0x02u && opcode != 0x82u && opcode != 0xA2u && opcode != 0xC2u &&
           opcode != 0xE2u;
}

/* Prints "6502/XX.json[i]: <field> expected K actual J" for a phi2 count
   check, such as "6502/48.json[0]: poll expected 2 actual 3", and returns
   0. */
static int count_mismatch(uint8_t opcode, const char *field, uint64_t expected, uint64_t actual)
{
    printf("6502/%02x.json[%u]: %s expected %llu actual %llu\n", (unsigned)opcode,
           (unsigned)test.index, field, (unsigned long long)expected, (unsigned long long)actual);
    return 0;
}

/* The deciding cycle, counted from the opcode fetch as 1: cycle 4 for BRK,
   cycle 1 for a taken branch that stays in its page (3 cycles), cycle N - 1
   for any other opcode (docs/hardware/sally.md, NMI). */
static uint32_t deciding_cycle(uint8_t opcode, uint32_t cycles)
{
    if (opcode == 0x00u) {
        return 4u;
    }
    if ((opcode & 0x1Fu) == 0x10u && cycles == 3u) {
        return 1u;
    }
    return cycles - 1u;
}

/* The NMI checks after the step that started at phi2 count before. A JAM
   was given an edge at its third read: it must still be latched, and the
   poll mark must be what it was before the step. */
static int check_poll(uint8_t opcode, uint64_t before, uint64_t mark_before, uint64_t edge)
{
    const muhtari *m = &machine.m;
    if (is_jam(opcode)) {
        if (m->bus.nmi_phi2 != edge) {
            return count_mismatch(opcode, "nmi_phi2", edge, m->bus.nmi_phi2);
        }
        if (m->cpu.poll_phi2 != mark_before) {
            return count_mismatch(opcode, "poll_phi2", mark_before, m->cpu.poll_phi2);
        }
        return 1;
    }
    uint64_t expected = deciding_cycle(opcode, test.cycle_count);
    if (m->cpu.poll_phi2 - before != expected) {
        return count_mismatch(opcode, "poll", expected, m->cpu.poll_phi2 - before);
    }
    return 1;
}

/* Runs the current test; returns 1 if it matched. Every test starts from a
   cleared CPU, so a jammed CPU is never stepped. */
static int run_test(uint8_t opcode)
{
    const struct n65v_state *in = &test.initial;
    struct muhtari__cpu *cpu = &machine.m.cpu;

    /* The vectors were recorded with an ANE and LXA constant of $EE; the
       harness sets it rather than relying on the default profile. */
    machine.m.profile.ane_lxa_constant = 0xEEu;
    memset(cpu, 0, sizeof *cpu);
    cpu->pc = in->pc;
    cpu->s = in->s;
    cpu->a = in->a;
    cpu->x = in->x;
    cpu->y = in->y;
    cpu->p = in->p;
    for (uint32_t i = 0u; i < in->ram_count; i++) {
        machine.ram[in->ram_addr[i]] = in->ram_value[i];
    }
    vector_machine_reset_log(&machine);
    /* No edge is latched unless this test scripts one: a JAM gets an edge
       at the third of its eleven reads, which a jammed CPU must not take.
       Its stale poll mark is set past the edge and past the JAM's last
       read, so a JAM that checked for an NMI would take it, and a mark it
       stored would show. */
    uint64_t before = machine.m.bus.phi2;
    machine.m.bus.nmi_phi2 = 0u;
    machine.nmi_at = 0u;
    if (is_jam(opcode)) {
        machine.nmi_at = before + 3u;
        cpu->poll_phi2 = before + 12u;
    }
    uint64_t mark_before = cpu->poll_phi2;

    muhtari__cpu_step(&machine.m);

    if ((cpu->jammed != 0u) != is_jam(opcode)) {
        printf("6502/%02x.json[%u]: jammed expected %d got %d\n", (unsigned)opcode,
               (unsigned)test.index, is_jam(opcode), cpu->jammed != 0u);
        return 0;
    }
    int ok = compare(opcode) && check_poll(opcode, before, mark_before, machine.nmi_at);
    machine.nmi_at = 0u;
    machine.m.bus.nmi_phi2 = 0u;
    return ok;
}

/* Zeroes RAM after a test, so the next test starts from zeroed RAM. After a
   pass the cycle compare shows that only the named addresses can have been
   written, so only those are zeroed. A failed test may have written any
   address, so all of RAM is zeroed. */
static void clear_ram(int passed)
{
    if (!passed) {
        memset(machine.ram, 0, sizeof machine.ram);
        return;
    }
    for (uint32_t i = 0u; i < test.initial.ram_count; i++) {
        machine.ram[test.initial.ram_addr[i]] = 0u;
    }
    for (uint32_t i = 0u; i < test.final.ram_count; i++) {
        machine.ram[test.final.ram_addr[i]] = 0u;
    }
}

static int ends_with(const char *s, const char *suffix)
{
    size_t n = strlen(s);
    size_t k = strlen(suffix);
    return n >= k && strcmp(s + n - k, suffix) == 0;
}

/* Parses --first's N: 1 to SST_JSON_FULL_COUNT, decimal digits only.
   Returns 0 for anything else. */
static uint32_t parse_first(const char *s)
{
    uint32_t n = 0u;
    if (*s == '\0' || strlen(s) > 5u) {
        return 0u;
    }
    for (; *s != '\0'; s++) {
        if (*s < '0' || *s > '9') {
            return 0u;
        }
        n = n * 10u + (uint32_t)(*s - '0');
    }
    return n <= SST_JSON_FULL_COUNT ? n : 0u;
}

int main(int argc, char **argv)
{
    if (argc != 3 && argc != 5) {
        return usage();
    }
    int hi = n65v_hex_digit(argv[2][0]);
    int lo = hi < 0 ? -1 : n65v_hex_digit(argv[2][1]);
    if (lo < 0 || argv[2][2] != '\0') {
        return usage();
    }
    uint8_t want = (uint8_t)(hi * 16 + lo);
    int json = ends_with(argv[1], ".json");
    uint32_t first = 0u;
    if (argc == 5) {
        first = strcmp(argv[3], "--first") == 0 ? parse_first(argv[4]) : 0u;
        if (first == 0u || !json) {
            return usage();
        }
    }

    size_t len = 0u;
    uint8_t *buf = n65v_read_file(argv[1], &len);
    if (buf == NULL) {
        fprintf(stderr, "muhtari-test-vectors: %s: cannot read\n", argv[1]);
        return 1;
    }

    if (json) {
        /* One upstream file becomes one chunk of opcode XX. */
        uint8_t *chunk = NULL;
        size_t chunk_len = 0u;
        uint32_t count = 0u;
        char err[160];
        int ok =
            sst_json_convert(buf, len, want, first, &chunk, &chunk_len, &count, err, sizeof err);
        free(buf);
        if (!ok) {
            fprintf(stderr, "muhtari-test-vectors: %s: %s\n", argv[1], err);
            return 1;
        }
        buf = chunk;
        len = chunk_len;
        n65v_init(&reader, buf, len, want, 1u, count);
    } else {
        /* Every chunk has the count the first header gives, and the reader
           checks every header against it. A first chunk of opcode 00 starts
           the 256-chunk sample; any other is the file's only chunk. */
        uint8_t first_op = len >= 6u ? buf[5] : 0u;
        uint32_t per_chunk = len >= 8u ? (uint32_t)buf[6] | ((uint32_t)buf[7] << 8) : 1u;
        n65v_init(&reader, buf, len, first_op, first_op == 0u ? 256u : 1u, per_chunk);
    }

    uint32_t run = 0u;
    uint32_t failed = 0u;
    uint8_t opcode = 0u;
    int rc;
    while ((rc = n65v_next(&reader, &test, &opcode)) == 1) {
        if (opcode != want) {
            continue;
        }
        run++;
        int passed = run_test(opcode);
        if (!passed) {
            if (failed == 0u) {
                print_cycles(opcode, "expected", test.cycles, test.cycle_count);
                uint32_t got = machine.log_count;
                print_cycles(opcode, "actual", machine.log, got);
            }
            failed++;
        }
        clear_ram(passed);
    }
    free(buf);
    if (rc < 0) {
        fprintf(stderr, "muhtari-test-vectors: %s: %s\n", argv[1], reader.error);
    }
    printf("6502/%02x: %u of %u failed\n", (unsigned)want, (unsigned)failed, (unsigned)run);
    if (rc == 0 && run == 0u) {
        fprintf(stderr, "muhtari-test-vectors: %s: no tests for opcode %02x\n", argv[1],
                (unsigned)want);
    }
    return (failed != 0u || rc < 0 || run == 0u) ? 1 : 0;
}
