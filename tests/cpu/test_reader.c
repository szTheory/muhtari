/* The N65V reader on crafted buffers: one valid chunk is read back field by
   field, and each malformed buffer must be refused with an error that names
   the byte offset at fault. Exit status: 0 when every case passes, 1
   otherwise. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "n65v.h"

/* One chunk of opcode $A9 with one test: the initial and final states each
   hold one RAM pair, and the test has one cycle. */
static const uint8_t chunk[] = {
    /* clang-format off */
    'N', '6', '5', 'V', 0x01, 0xA9, 0x01, 0x00, /* header, count 1 */
    0x00, 0x00,                                 /* index 0 */
    0x00, 0x02, 0xFD, 0x11, 0x22, 0x33, 0x24,   /* initial pc s a x y p */
    0x01, 0x00, 0x02, 0xA9,                     /* one RAM pair */
    0x02, 0x02, 0xFD, 0x7F, 0x22, 0x33, 0x24,   /* final pc s a x y p */
    0x01, 0x01, 0x02, 0x7F,                     /* one RAM pair */
    0x01, 0x00, 0x02, 0xA9, 0x00,               /* one cycle: read $A9 at $0200 */
    /* clang-format on */
};

#define CHUNK_LEN sizeof chunk

static struct n65v_test test;
static int failures;

/* Reads every test of buf; returns n65v_next's final result and leaves the
   reader for its error. *count gets the number of tests read. */
static int read_all(struct n65v_reader *r, const uint8_t *buf, size_t len, uint32_t chunks,
                    uint32_t tests, uint32_t *count)
{
    uint8_t opcode = 0u;
    int rc;
    n65v_init(r, buf, len, 0xA9u, chunks, tests);
    *count = 0u;
    while ((rc = n65v_next(r, &test, &opcode)) == 1) {
        (*count)++;
    }
    return rc;
}

/* The buffer must be refused with an error that starts with want. */
static void expect_error(const char *name, const uint8_t *buf, size_t len, uint32_t chunks,
                         uint32_t tests, const char *want)
{
    static struct n65v_reader r;
    uint32_t count = 0u;
    int rc = read_all(&r, buf, len, chunks, tests, &count);
    if (rc != -1 || strncmp(r.error, want, strlen(want)) != 0) {
        printf("reader: %s: expected an error starting \"%s\", got %d \"%s\"\n", name, want, rc,
               rc == -1 ? r.error : "");
        failures++;
    }
}

/* The valid chunk reads back as one test with every field intact. */
static void valid(void)
{
    static struct n65v_reader r;
    uint32_t count = 0u;
    int rc = read_all(&r, chunk, CHUNK_LEN, 1u, 1u, &count);
    if (rc != 0 || count != 1u || test.index != 0u || test.initial.pc != 0x0200u ||
        test.initial.a != 0x11u || test.initial.ram_count != 1u ||
        test.initial.ram_addr[0] != 0x0200u || test.initial.ram_value[0] != 0xA9u ||
        test.final.pc != 0x0202u || test.final.a != 0x7Fu || test.final.ram_addr[0] != 0x0201u ||
        test.cycle_count != 1u || test.cycles[0].addr != 0x0200u || test.cycles[0].value != 0xA9u ||
        test.cycles[0].kind != N65V_KIND_READ) {
        printf("reader: valid: the chunk did not read back (%d, %u tests, %s)\n", rc,
               (unsigned)count, r.error);
        failures++;
    }
}

/* Every prefix of the valid chunk is refused, naming an offset within the
   field that the cut falls in: at or before the cut, by less than the
   four-byte magic. */
static void cuts(void)
{
    static struct n65v_reader r;
    for (size_t k = 0u; k < CHUNK_LEN; k++) {
        uint32_t count = 0u;
        int rc = read_all(&r, chunk, k, 1u, 1u, &count);
        char *end = NULL;
        unsigned long at =
            strncmp(r.error, "offset ", 7u) == 0 ? strtoul(r.error + 7, &end, 10) : 0;
        if (rc != -1 || end == NULL || *end != ':' || at > k || k - at >= 4u) {
            printf(
                "reader: cut at %zu: expected an error naming an offset near %zu, got %d \"%s\"\n",
                k, k, rc, rc == -1 ? r.error : "");
            failures++;
        }
    }
}

int main(void)
{
    uint8_t two[2u * CHUNK_LEN];
    uint8_t zero[8];

    valid();
    cuts();
    expect_error("empty", NULL, 0u, 1u, 1u, "offset 0: truncated magic");

    memcpy(zero, chunk, sizeof zero);
    zero[6] = 0x00u;
    expect_error("zero tests", zero, sizeof zero, 1u, 0u, "offset 6: count 0");
    expect_error("zero tests, one expected", zero, sizeof zero, 1u, 1u, "offset 6: count 0");

    expect_error("one test, two expected", chunk, CHUNK_LEN, 1u, 2u,
                 "offset 6: count 1, expected 2");
    expect_error("one chunk, two expected", chunk, CHUNK_LEN, 2u, 1u, "offset 37: truncated magic");
    memcpy(two, chunk, CHUNK_LEN);
    memcpy(two + CHUNK_LEN, chunk, CHUNK_LEN);
    two[CHUNK_LEN + 5u] = 0xAAu;
    expect_error("two chunks, one expected", two, sizeof two, 1u, 1u,
                 "offset 37: 37 trailing bytes");

    if (failures != 0) {
        printf("reader: %d cases failed\n", failures);
        return 1;
    }
    printf("reader: every case passed\n");
    return 0;
}
