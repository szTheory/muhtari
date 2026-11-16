/* Generated trace oracle and private CPU edge checks. */
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"
#include "muhtari.h"
#include "../check.h"

#define TRACE_EVENT_CAP 131072u
#define TRACE_IMAGE_BYTES (128u + 4096u)
#define TRACE_ROM 0xF000u

typedef struct record {
    uint64_t tick;
    uint32_t kind, line, t, pc, a, x, y, s, p, addr, rw, value;
} record;

typedef struct trace_file {
    record *events;
    size_t count;
    size_t declared_count;
    char header[192];
    int truncated;
} trace_file;

typedef struct collector {
    struct muhtari__trace_event *events;
    size_t capacity;
    size_t count;
} collector;

static uint8_t fixture[TRACE_IMAGE_BYTES];
static const uint8_t trace_program[] = {
    0xA9u, 0x2Au,                         /* LDA #$2A */
    0x8Du, 0x00u, 0x18u,                  /* STA $1800 */
    0xADu, 0x00u, 0x18u,                  /* LDA $1800 */
    0xADu, 0x00u, 0x00u,                  /* LDA $0000 (slow TIA read) */
    0xADu, 0xFFu, 0xFFu,                  /* LDA $FFFF */
    0x4Cu, 0x0Eu, 0xF0u                   /* JMP $F00E */
};

static void collect_event(void *user, const struct muhtari__trace_event *event)
{
    collector *capture = (collector *)user;
    if (capture->count < capture->capacity)
        capture->events[capture->count] = *event;
    capture->count++;
}

static void make_fixture(void)
{
    memset(fixture, 0, sizeof fixture);
    fixture[0] = 4u;
    memcpy(fixture + 1u, "ATARI7800", 9u);
    fixture[50] = 0x10u;
    fixture[55] = fixture[56] = 1u;
    memcpy(fixture + 100u, "GENERATED TRACE FIXTURE", 23u);
    memset(fixture + 128u, 0xEA, 4096u);
    memcpy(fixture + 128u, trace_program, sizeof trace_program);
    fixture[128u + 0xFFCu] = 0u;
    fixture[128u + 0xFFDu] = 0xF0u;
}

static int write_fixture(const char *dir)
{
    char path[1024];
    int n = snprintf(path, sizeof path, "%s/trace.a78", dir);
    CHECK(n > 0 && (size_t)n < sizeof path);
    if (n <= 0 || (size_t)n >= sizeof path)
        return 1;
    make_fixture();
    FILE *f = fopen(path, "wb");
    CHECK(f != NULL);
    if (f == NULL)
        return 1;
    CHECK_EQ_U64(fwrite(fixture, 1u, sizeof fixture, f), sizeof fixture);
    CHECK_EQ_U64(fclose(f), 0);
    return check_failures != 0 ? 1 : 0;
}

static int write_busy_fixture(const char *dir)
{
    char path[1024];
    int n = snprintf(path, sizeof path, "%s/trace-busy.a78", dir);
    CHECK(n > 0 && (size_t)n < sizeof path);
    if (n <= 0 || (size_t)n >= sizeof path)
        return 1;
    make_fixture();
    memset(fixture + 128u, 0xEA, 4096u);
    fixture[128u + 0xFF0u] = 0x4Cu;
    fixture[128u + 0xFF1u] = 0x00u;
    fixture[128u + 0xFF2u] = 0xF0u;
    fixture[128u + 0xFFCu] = 0x00u;
    fixture[128u + 0xFFDu] = 0xF0u;
    FILE *f = fopen(path, "wb");
    CHECK(f != NULL);
    if (f == NULL)
        return 1;
    CHECK_EQ_U64(fwrite(fixture, 1u, sizeof fixture, f), sizeof fixture);
    CHECK_EQ_U64(fclose(f), 0u);
    return check_failures != 0 ? 1 : 0;
}

static int measure_trace(const char *path)
{
    FILE *f = fopen(path, "rb");
    CHECK(f != NULL);
    if (f == NULL)
        return 1;
    CHECK_EQ_U64(fseek(f, 0L, SEEK_END), 0u);
    long output_bytes = ftell(f);
    CHECK(output_bytes >= 0L);
    rewind(f);
    char line[192];
    char status[16] = {0};
    unsigned limit = 0u;
    size_t records = 0u, max_line = 0u;
    CHECK(fgets(line, (int)sizeof line, f) != NULL);
    CHECK_EQ_U64(sscanf(line,
                        "{\"format\":\"muhtari-trace\",\"version\":1,\"frame\":%*u,"
                        "\"lo\":%*u,\"hi\":%*u,\"limit\":%u}",
                        &limit), 1u);
    while (fgets(line, (int)sizeof line, f) != NULL) {
        size_t line_length = strlen(line);
        CHECK(line_length != 0u && line[line_length - 1u] == '\n');
        if (line_length != 0u && line[line_length - 1u] == '\n' &&
            strncmp(line, "{\"kind\":", 8u) == 0) {
            records++;
            if (line_length > max_line)
                max_line = line_length;
        } else if (strncmp(line, "{\"status\":", 10u) == 0) {
            CHECK_EQ_U64(sscanf(line, "{\"status\":\"%15[^\"]\"", status), 1u);
        }
    }
    CHECK_EQ_U64(fclose(f), 0u);
    printf("records=%zu status=%s limit=%u event_size=%zu capture_bytes=%zu max_line=%zu output_bytes=%ld\n",
           records, status, limit, sizeof(struct muhtari__trace_event),
           (size_t)limit * sizeof(struct muhtari__trace_event), max_line, output_bytes);
    return check_failures != 0 ? 1 : 0;
}

static int parse_record(const char *line, record *out)
{
    unsigned long long tick = 0u;
    int fields = sscanf(
        line,
        "{\"kind\":%u,\"tick\":%llu,\"line\":%u,\"t\":%u,\"pc\":%u,"
        "\"a\":%u,\"x\":%u,\"y\":%u,\"s\":%u,\"p\":%u,\"addr\":%u,"
        "\"rw\":%u,\"value\":%u}",
        &out->kind, &tick, &out->line, &out->t, &out->pc, &out->a, &out->x, &out->y,
        &out->s, &out->p, &out->addr, &out->rw, &out->value);
    out->tick = (uint64_t)tick;
    return fields == 13;
}

static int read_trace(const char *path, trace_file *out)
{
    memset(out, 0, sizeof *out);
    out->events = (record *)calloc(TRACE_EVENT_CAP, sizeof *out->events);
    CHECK(out->events != NULL);
    if (out->events == NULL)
        return 0;
    FILE *f = fopen(path, "rb");
    CHECK(f != NULL);
    if (f == NULL)
        return 0;
    CHECK(fgets(out->header, (int)sizeof out->header, f) != NULL);
    CHECK(strstr(out->header, "\"format\":\"muhtari-trace\"") != NULL);
    CHECK(strstr(out->header, "\"version\":1") != NULL);
    char line[192];
    int footer_seen = 0;
    while (fgets(line, (int)sizeof line, f) != NULL) {
        if (strncmp(line, "{\"status\":", 10u) == 0) {
            char status[16] = {0};
            CHECK_EQ_U64(sscanf(line, "{\"status\":\"%15[^\"]\",\"count\":%zu}",
                                status, &out->declared_count), 2);
            CHECK(strcmp(status, "complete") == 0 || strcmp(status, "truncated") == 0);
            out->truncated = strcmp(status, "truncated") == 0;
            footer_seen = 1;
            CHECK(fgets(line, (int)sizeof line, f) == NULL);
            break;
        }
        CHECK(out->count < TRACE_EVENT_CAP);
        if (out->count < TRACE_EVENT_CAP) {
            CHECK(parse_record(line, &out->events[out->count]));
            out->count++;
        }
    }
    CHECK(footer_seen);
    CHECK_EQ_U64(fclose(f), 0);
    CHECK_EQ_U64(out->declared_count, out->count);
    return footer_seen;
}

static void free_trace(trace_file *trace)
{
    free(trace->events);
    trace->events = NULL;
}

static void validate_trace(const char *path, const char *expected_status)
{
    trace_file trace;
    int valid = read_trace(path, &trace);
    CHECK(valid);
    if (valid && strcmp(expected_status, "complete") == 0)
        CHECK(!trace.truncated);
    else if (valid && strcmp(expected_status, "truncated") == 0)
        CHECK(trace.truncated);
    free_trace(&trace);
}

static void expect_record(const record *got, const record *want)
{
    CHECK_EQ_U64(got->tick, want->tick);
    CHECK_EQ_U64(got->kind, want->kind);
    CHECK_EQ_U64(got->line, want->line);
    CHECK_EQ_U64(got->t, want->t);
    CHECK_EQ_U64(got->pc, want->pc);
    CHECK_EQ_U64(got->a, want->a);
    CHECK_EQ_U64(got->x, want->x);
    CHECK_EQ_U64(got->y, want->y);
    CHECK_EQ_U64(got->s, want->s);
    CHECK_EQ_U64(got->p, want->p);
    CHECK_EQ_U64(got->addr, want->addr);
    CHECK_EQ_U64(got->rw, want->rw);
    CHECK_EQ_U64(got->value, want->value);
}

static void test_oracle(const char *path)
{
    static const record prefix[] = {
        {108988u, 0u, 240u, 28u, 0xF000u, 0u, 22u, 0u, 22u, 44u, 0xF000u, 0u, 0xA9u},
        {108988u, 1u, 240u, 28u, 0xF000u, 0u, 22u, 0u, 22u, 44u, 0xF000u, 0u, 0xA9u},
        {108992u, 1u, 240u, 32u, 0xF001u, 0u, 22u, 0u, 22u, 44u, 0xF001u, 0u, 0x2Au},
        {108996u, 0u, 240u, 36u, 0xF002u, 42u, 22u, 0u, 22u, 44u, 0xF002u, 0u, 0x8Du},
        {108996u, 1u, 240u, 36u, 0xF002u, 42u, 22u, 0u, 22u, 44u, 0xF002u, 0u, 0x8Du},
        {109000u, 1u, 240u, 40u, 0xF003u, 42u, 22u, 0u, 22u, 44u, 0xF003u, 0u, 0u},
        {109004u, 1u, 240u, 44u, 0xF004u, 42u, 22u, 0u, 22u, 44u, 0xF004u, 0u, 0x18u},
        {109008u, 1u, 240u, 48u, 0xF005u, 42u, 22u, 0u, 22u, 44u, 0x1800u, 1u, 42u},
        {109012u, 0u, 240u, 52u, 0xF005u, 42u, 22u, 0u, 22u, 44u, 0xF005u, 0u, 0xADu},
        {109012u, 1u, 240u, 52u, 0xF005u, 42u, 22u, 0u, 22u, 44u, 0xF005u, 0u, 0xADu},
        {109016u, 1u, 240u, 56u, 0xF006u, 42u, 22u, 0u, 22u, 44u, 0xF006u, 0u, 0u},
        {109020u, 1u, 240u, 60u, 0xF007u, 42u, 22u, 0u, 22u, 44u, 0xF007u, 0u, 0x18u},
        {109024u, 1u, 240u, 64u, 0xF008u, 42u, 22u, 0u, 22u, 44u, 0x1800u, 0u, 42u},
    };
    static const record tail[] = {
        {117174u, 0u, 258u, 42u, 0xF00Eu, 234u, 22u, 0u, 22u, 172u, 0xF00Eu, 0u, 0x4Cu},
        {117174u, 1u, 258u, 42u, 0xF00Eu, 234u, 22u, 0u, 22u, 172u, 0xF00Eu, 0u, 0x4Cu},
        {117178u, 1u, 258u, 46u, 0xF00Fu, 234u, 22u, 0u, 22u, 172u, 0xF00Fu, 0u, 0x0Eu},
        {117182u, 1u, 258u, 50u, 0xF010u, 234u, 22u, 0u, 22u, 172u, 0xF010u, 0u, 0xF0u},
    };
    trace_file trace;
    CHECK(read_trace(path, &trace));
    if (trace.events == NULL)
        return;
    CHECK(strstr(trace.header, "\"frame\":1") != NULL);
    CHECK(strstr(trace.header, "\"lo\":0,\"hi\":65535,\"limit\":65536") != NULL);
    CHECK(!trace.truncated);
    CHECK_EQ_U64(trace.count, 2731u);
    uint32_t instructions = 0u, buses = 0u;
    for (size_t i = 0u; i < trace.count; i++) {
        const record *event = &trace.events[i];
        CHECK(event->kind <= MUHTARI__TRACE_BUS);
        CHECK(event->line < MUHTARI__LINES_NTSC);
        CHECK(event->t < MUHTARI__MARIA_LINE_TICKS);
        CHECK(event->addr <= 65535u && event->rw <= 1u && event->value <= 255u);
        if (i != 0u)
            CHECK(trace.events[i - 1u].tick <= event->tick);
        if (event->kind == MUHTARI__TRACE_INSTRUCTION) {
            instructions++;
            CHECK(event->addr == event->pc);
            CHECK(event->rw == 0u);
            CHECK(i + 1u < trace.count);
            if (i + 1u < trace.count) {
                const record *bus = &trace.events[i + 1u];
                CHECK(bus->kind == MUHTARI__TRACE_BUS);
                CHECK(bus->tick == event->tick && bus->pc == event->pc && bus->addr == event->addr);
                CHECK(bus->a == event->a && bus->x == event->x && bus->y == event->y &&
                      bus->s == event->s && bus->p == event->p && bus->value == event->value);
            }
        } else {
            buses++;
        }
    }
    CHECK_EQ_U64(instructions, 682u);
    CHECK_EQ_U64(buses, 2049u);
    CHECK(trace.count >= sizeof prefix / sizeof prefix[0]);
    for (size_t i = 0u; i < sizeof prefix / sizeof prefix[0] && i < trace.count; i++)
        expect_record(&trace.events[i], &prefix[i]);
    CHECK(trace.count >= sizeof tail / sizeof tail[0]);
    for (size_t i = 0u; i < sizeof tail / sizeof tail[0] && i < trace.count; i++)
        expect_record(&trace.events[trace.count - sizeof tail / sizeof tail[0] + i], &tail[i]);

    free_trace(&trace);
}

static void verify_filter(const char *case_name, const char *path)
{
    trace_file trace;
    CHECK(read_trace(path, &trace));
    if (trace.events == NULL)
        return;
    if (strcmp(case_name, "pc") == 0) {
        CHECK(strstr(trace.header, "\"frame\":1") != NULL);
        CHECK(strstr(trace.header, "\"lo\":61440,\"hi\":61440") != NULL);
        CHECK_EQ_U64(trace.count, 2u);
        if (trace.count >= 2u) {
            CHECK_EQ_U64(trace.events[0].kind, MUHTARI__TRACE_INSTRUCTION);
            CHECK_EQ_U64(trace.events[1].kind, MUHTARI__TRACE_BUS);
            CHECK_EQ_U64(trace.events[0].addr, 0xF000u);
            CHECK_EQ_U64(trace.events[1].addr, 0xF000u);
            CHECK_EQ_U64(trace.events[0].tick, trace.events[1].tick);
        }
        CHECK(!trace.truncated);
    } else if (strcmp(case_name, "ram") == 0) {
        CHECK_EQ_U64(trace.count, 2u);
        if (trace.count >= 2u) {
            CHECK_EQ_U64(trace.events[0].kind, MUHTARI__TRACE_BUS);
            CHECK_EQ_U64(trace.events[0].tick, 109008u);
            CHECK_EQ_U64(trace.events[0].line, 240u);
            CHECK_EQ_U64(trace.events[0].t, 48u);
            CHECK_EQ_U64(trace.events[0].pc, 0xF005u);
            CHECK_EQ_U64(trace.events[0].addr, 0x1800u);
            CHECK_EQ_U64(trace.events[0].rw, 1u);
            CHECK_EQ_U64(trace.events[0].value, 42u);
            CHECK_EQ_U64(trace.events[1].kind, MUHTARI__TRACE_BUS);
            CHECK_EQ_U64(trace.events[1].tick, 109024u);
            CHECK_EQ_U64(trace.events[1].pc, 0xF008u);
            CHECK_EQ_U64(trace.events[1].addr, 0x1800u);
            CHECK_EQ_U64(trace.events[1].rw, 0u);
            CHECK_EQ_U64(trace.events[1].value, 42u);
        }
    } else if (strcmp(case_name, "zero") == 0) {
        CHECK_EQ_U64(trace.count, 1u);
        if (trace.count == 1u) {
            CHECK_EQ_U64(trace.events[0].kind, MUHTARI__TRACE_BUS);
            CHECK_EQ_U64(trace.events[0].addr, 0u);
            CHECK_EQ_U64(trace.events[0].pc, 0xF00Bu);
            CHECK_EQ_U64(trace.events[0].rw, 0u);
            CHECK_EQ_U64(trace.events[0].tick, 109041u);
            CHECK_EQ_U64(trace.events[0].t, 81u);
        }
    } else if (strcmp(case_name, "ffff") == 0) {
        CHECK_EQ_U64(trace.count, 1u);
        if (trace.count == 1u) {
            CHECK_EQ_U64(trace.events[0].kind, MUHTARI__TRACE_BUS);
            CHECK_EQ_U64(trace.events[0].addr, 65535u);
            CHECK_EQ_U64(trace.events[0].pc, 0xF00Eu);
            CHECK_EQ_U64(trace.events[0].rw, 0u);
            CHECK_EQ_U64(trace.events[0].value, 0xEAu);
        }
    } else if (strcmp(case_name, "empty") == 0) {
        CHECK_EQ_U64(trace.count, 0u);
        CHECK(!trace.truncated);
    } else if (strcmp(case_name, "truncated") == 0) {
        CHECK_EQ_U64(trace.count, 1u);
        CHECK(trace.truncated);
    } else if (strcmp(case_name, "exact") == 0) {
        CHECK_EQ_U64(trace.count, 2731u);
        CHECK(!trace.truncated);
        CHECK(strstr(trace.header, "\"limit\":2731") != NULL);
    } else if (strcmp(case_name, "one_extra") == 0) {
        CHECK_EQ_U64(trace.count, 2730u);
        CHECK(trace.truncated);
        CHECK(strstr(trace.header, "\"limit\":2730") != NULL);
    } else if (strcmp(case_name, "nonmatching_after_limit") == 0) {
        CHECK_EQ_U64(trace.count, 2u);
        CHECK(!trace.truncated);
    } else if (strcmp(case_name, "frame2") == 0) {
        CHECK(strstr(trace.header, "\"frame\":2") != NULL);
        CHECK(trace.count != 0u);
        if (trace.count != 0u)
            CHECK(trace.events[0].tick > 117174u);
        CHECK(!trace.truncated);
    } else {
        CHECK(0 && "unknown filter case");
    }
    free_trace(&trace);
}

static void *test_alloc(void *user, size_t size)
{
    (void)user;
    return malloc(size);
}

static void test_free(void *user, void *ptr, size_t size)
{
    (void)user;
    (void)size;
    free(ptr);
}

static muhtari *make_machine(void)
{
    muhtari_config config;
    memset(&config, 0, sizeof config);
    config.size = (uint32_t)sizeof config;
    config.allocator.alloc = test_alloc;
    config.allocator.free = test_free;
    muhtari *m = NULL;
    CHECK_EQ_U64(muhtari_create(&config, &m), MUHTARI_STATUS_OK);
    return m;
}

static collector make_collector(size_t capacity)
{
    collector capture = {0};
    capture.events = (struct muhtari__trace_event *)calloc(capacity, sizeof *capture.events);
    capture.capacity = capture.events == NULL ? 0u : capacity;
    CHECK(capture.events != NULL);
    return capture;
}

static void prepare_ram_program(muhtari *m, const uint8_t *program, size_t bytes)
{
    muhtari__power_on(m);
    m->bus.inptctrl = MUHTARI__INPTCTRL_LOCK | MUHTARI__INPTCTRL_MEN | MUHTARI__INPTCTRL_EXT;
    memcpy(m->ram, program, bytes);
    m->cpu.pc = MUHTARI__RAM_BASE;
    m->cpu.s = 0xFDu;
    m->cpu.p = 0x24u;
}

static void test_fast_slow_and_dummy(void)
{
    muhtari *m = make_machine();
    collector capture = make_collector(128u);
    static const uint8_t code[] = {
        0xA9u, 0x2Au,             /* LDA #$2A */
        0xADu, 0x00u, 0x00u,      /* LDA $0000, slow final cycle */
        0x8Du, 0x01u, 0x00u,      /* STA $0001, slow final cycle */
        0xBDu, 0xFFu, 0x00u       /* LDA $00FF,X, page-cross dummy at $0000 */
    };
    CHECK(m != NULL && capture.events != NULL);
    if (m == NULL || capture.events == NULL)
        goto done;
    prepare_ram_program(m, code, sizeof code);
    m->cpu.a = 0x11u;
    m->cpu.x = 1u;
    m->cpu.y = 0x33u;
    m->cpu.s = 0x44u;
    m->cpu.p = 0x24u;
    m->bus.ticks = 450u;
    m->trace_callback = collect_event;
    m->trace_user = &capture;
    muhtari__trace_cpu_step(m);
    CHECK_EQ_U64(capture.count, 3u);
    if (capture.count >= 3u) {
        const struct muhtari__trace_event *opcode = &capture.events[0];
        CHECK_EQ_U64(opcode->tick, 454u);
        CHECK_EQ_U64(opcode->line, 1u);
        CHECK_EQ_U64(opcode->t, 0u);
        CHECK_EQ_U64(opcode->pc, MUHTARI__RAM_BASE);
        CHECK_EQ_U64(opcode->a, 0x11u);
        CHECK_EQ_U64(opcode->x, 1u);
        CHECK_EQ_U64(opcode->y, 0x33u);
        CHECK_EQ_U64(opcode->s, 0x44u);
        CHECK_EQ_U64(opcode->p, 0x24u);
        CHECK_EQ_U64(opcode->value, 0xA9u);
        CHECK_EQ_U64(capture.events[1].kind, MUHTARI__TRACE_BUS);
        CHECK_EQ_U64(capture.events[1].tick, capture.events[0].tick);
        CHECK_EQ_U64(capture.events[2].tick, 458u);
        CHECK_EQ_U64(capture.events[2].line, 1u);
        CHECK_EQ_U64(capture.events[2].t, 4u);
        CHECK_EQ_U64(capture.events[2].pc, MUHTARI__RAM_BASE + 1u);
        CHECK_EQ_U64(capture.events[2].a, 0x11u);
        CHECK_EQ_U64(capture.events[2].addr, MUHTARI__RAM_BASE + 1u);
        CHECK_EQ_U64(capture.events[2].value, 0x2Au);
    }
    m->cpu.pc = (uint16_t)(MUHTARI__RAM_BASE + 2u);
    muhtari__trace_cpu_step(m);
    CHECK_EQ_U64(capture.count, 8u);
    if (capture.count >= 8u) {
        CHECK_EQ_U64(capture.events[7].tick, 475u);
        CHECK_EQ_U64(capture.events[7].line, 1u);
        CHECK_EQ_U64(capture.events[7].t, 21u);
        CHECK_EQ_U64(capture.events[7].addr, 0u);
        CHECK_EQ_U64(capture.events[7].rw, 0u);
    }
    m->ram[5] = 0x8Du;
    m->ram[6] = 1u;
    m->ram[7] = 0u;
    m->cpu.a = 0x5Au;
    m->cpu.pc = (uint16_t)(MUHTARI__RAM_BASE + 5u);
    muhtari__trace_cpu_step(m);
    CHECK_EQ_U64(capture.count, 13u);
    if (capture.count >= 13u) {
        CHECK_EQ_U64(capture.events[12].tick, 493u);
        CHECK_EQ_U64(capture.events[12].addr, 1u);
        CHECK_EQ_U64(capture.events[12].rw, 1u);
        CHECK_EQ_U64(capture.events[12].value, 0x5Au);
    }
    m->cpu.pc = (uint16_t)(MUHTARI__RAM_BASE + 8u);
    m->cpu.x = 1u;
    muhtari__trace_cpu_step(m);
    CHECK_EQ_U64(capture.count, 19u);
    if (capture.count >= 19u) {
        CHECK_EQ_U64(capture.events[17].tick, 511u);
        CHECK_EQ_U64(capture.events[17].addr, 0u);
        CHECK_EQ_U64(capture.events[17].rw, 0u);
        CHECK_EQ_U64(capture.events[18].tick, 517u);
        CHECK_EQ_U64(capture.events[18].addr, 0x0100u);
        CHECK_EQ_U64(capture.events[18].rw, 0u);
    }
done:
    if (m != NULL)
        muhtari_destroy(m);
    free(capture.events);
}

static void test_frame_origin_edge(void)
{
    muhtari *m = make_machine();
    collector capture = make_collector(8u);
    static const uint8_t code[] = {0xEAu};
    CHECK(m != NULL && capture.events != NULL);
    if (m != NULL && capture.events != NULL) {
        prepare_ram_program(m, code, sizeof code);
        m->bus.ticks = 119398u;
        m->bus.next_deadline = 119402u;
        m->maria.event_line = (uint16_t)MUHTARI__LINES_NTSC;
        m->maria.event_index = 0u;
        m->trace_callback = collect_event;
        m->trace_user = &capture;
        muhtari__trace_cpu_step(m);
        CHECK_EQ_U64(capture.count, 3u);
        if (capture.count >= 3u) {
            CHECK_EQ_U64(capture.events[0].tick, 119402u);
            CHECK_EQ_U64(capture.events[0].line, 0u);
            CHECK_EQ_U64(capture.events[0].t, 0u);
            CHECK_EQ_U64(m->maria.origin, 119402u);
            CHECK_EQ_U64(capture.events[2].tick, 119406u);
            CHECK_EQ_U64(capture.events[2].line, 0u);
            CHECK_EQ_U64(capture.events[2].t, 4u);
        }
    }
    if (m != NULL)
        muhtari_destroy(m);
    free(capture.events);
}

static void test_jam_and_interrupt(void)
{
    muhtari *jam = make_machine();
    collector jam_capture = make_collector(16u);
    static const uint8_t jam_code[] = {0x02u};
    CHECK(jam != NULL && jam_capture.events != NULL);
    if (jam != NULL && jam_capture.events != NULL) {
        prepare_ram_program(jam, jam_code, sizeof jam_code);
        jam->trace_callback = collect_event;
        jam->trace_user = &jam_capture;
        muhtari__trace_cpu_step(jam);
        CHECK_EQ_U64(jam_capture.count, 12u);
        muhtari__trace_cpu_step(jam);
        CHECK_EQ_U64(jam_capture.count, 13u);
        if (jam_capture.count >= 13u) {
            CHECK_EQ_U64(jam_capture.events[12].kind, MUHTARI__TRACE_BUS);
            CHECK_EQ_U64(jam_capture.events[12].addr, 0xFFFFu);
            CHECK_EQ_U64(jam_capture.events[12].rw, 0u);
        }
    }
    if (jam != NULL)
        muhtari_destroy(jam);
    free(jam_capture.events);

    muhtari *nmi = make_machine();
    collector nmi_capture = make_collector(32u);
    static const uint8_t nmi_code[] = {0xEAu};
    CHECK(nmi != NULL && nmi_capture.events != NULL);
    if (nmi != NULL && nmi_capture.events != NULL) {
        prepare_ram_program(nmi, nmi_code, sizeof nmi_code);
        nmi->trace_callback = collect_event;
        nmi->trace_user = &nmi_capture;
        CHECK_EQ_U64(muhtari__bus_raise_nmi(nmi, 0u), 1u);
        muhtari__trace_cpu_step(nmi);
        CHECK_EQ_U64(nmi_capture.count, 10u);
        unsigned writes = 0u;
        for (size_t i = 0u; i < nmi_capture.count && i < nmi_capture.capacity; i++)
            if (nmi_capture.events[i].kind == MUHTARI__TRACE_BUS &&
                nmi_capture.events[i].rw == 1u)
                writes++;
        CHECK_EQ_U64(writes, 3u);
        if (nmi_capture.count >= 2u) {
            CHECK_EQ_U64(nmi_capture.events[nmi_capture.count - 2u].addr, 0xFFFAu);
            CHECK_EQ_U64(nmi_capture.events[nmi_capture.count - 1u].addr, 0xFFFBu);
            CHECK(nmi_capture.events[nmi_capture.count - 2u].tick <
                  nmi_capture.events[nmi_capture.count - 1u].tick);
        }
    }
    if (nmi != NULL)
        muhtari_destroy(nmi);
    free(nmi_capture.events);
}

static void test_instances_state_and_detach(void)
{
    make_fixture();
    muhtari *first = make_machine(), *second = make_machine();
    collector a = make_collector(256u), b = make_collector(256u);
    CHECK(first != NULL && second != NULL && a.events != NULL && b.events != NULL);
    if (first == NULL || second == NULL || a.events == NULL || b.events == NULL)
        goto done;
    muhtari_media media;
    memset(&media, 0, sizeof media);
    media.size = (uint32_t)sizeof media;
    media.image = fixture;
    media.image_size = sizeof fixture;
    CHECK_EQ_U64(muhtari_load(first, &media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari_load(second, &media), MUHTARI_STATUS_OK);
    first->trace_callback = collect_event;
    first->trace_user = &a;
    second->trace_callback = collect_event;
    second->trace_user = &b;
    size_t state_bytes = muhtari_state_size(first);
    uint8_t *state = (uint8_t *)malloc(state_bytes);
    CHECK(state_bytes != 0u && state != NULL);
    if (state != NULL) {
        CHECK_EQ_U64(muhtari_save_state(first, state, state_bytes), MUHTARI_STATUS_OK);
        first->trace_callback = NULL;
        first->trace_user = NULL;
        CHECK_EQ_U64(muhtari_load_state(first, state, state_bytes), MUHTARI_STATUS_OK);
        CHECK(first->trace_callback == NULL && first->trace_user == NULL);
        muhtari__cpu_step(first);
        CHECK_EQ_U64(a.count, 0u);
        muhtari__trace_cpu_step(second);
        CHECK_EQ_U64(b.count, 3u);
        CHECK_EQ_U64(a.count, 0u);
        first->trace_callback = collect_event;
        first->trace_user = &a;
        muhtari__trace_cpu_step(first);
        CHECK_EQ_U64(a.count, 5u);
        free(state);
    }
    muhtari_frame io;
    memset(&io, 0, sizeof io);
    io.size = (uint32_t)sizeof io;
    CHECK_EQ_U64(muhtari_run_frame(first, &io), MUHTARI_STATUS_OK);
    size_t after_trace_frame = a.count;
    CHECK(after_trace_frame > 256u);
    first->trace_callback = NULL;
    first->trace_user = NULL;
    CHECK_EQ_U64(muhtari_run_frame(first, &io), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(a.count, after_trace_frame);
    first->trace_callback = collect_event;
    first->trace_user = &a;
    CHECK_EQ_U64(muhtari_run_frame(first, &io), MUHTARI_STATUS_OK);
    CHECK(a.count > after_trace_frame);
done:
    if (first != NULL)
        muhtari_destroy(first);
    if (second != NULL)
        muhtari_destroy(second);
    free(a.events);
    free(b.events);
}

static void test_edges(void)
{
    test_fast_slow_and_dummy();
    test_frame_origin_edge();
    test_jam_and_interrupt();
    test_instances_state_and_detach();
}

int main(int argc, char **argv)
{
    if (argc == 3 && strcmp(argv[1], "fixture") == 0)
        return write_fixture(argv[2]);
    if (argc == 3 && strcmp(argv[1], "busy_fixture") == 0)
        return write_busy_fixture(argv[2]);
    if (argc == 3 && strcmp(argv[1], "measure") == 0)
        return measure_trace(argv[2]);
    if (argc == 3 && strcmp(argv[1], "oracle") == 0) {
        test_oracle(argv[2]);
        CHECK_DONE();
    }
    if (argc == 4 && strcmp(argv[1], "validate") == 0) {
        validate_trace(argv[2], argv[3]);
        CHECK_DONE();
    }
    if (argc == 4 && strcmp(argv[1], "filter") == 0) {
        verify_filter(argv[2], argv[3]);
        CHECK_DONE();
    }
    if (argc == 2 && strcmp(argv[1], "edges") == 0) {
        test_edges();
        CHECK_DONE();
    }
    return 2;
}
