/* vecconv: converts one file of the SingleStepTests 65x02 vectors
   (6502/v1/<xx>.json) to one N65V v1 chunk (n65v.h), then reads the output
   back through the reader the tests use. Test tool only: nothing shipped
   links it and it is not installed. Integer-only.

     vecconv <in.json> <out.n65v> <opcode-hex> [--first N]

   The input rules are sst_json.h's. With --first N it converts the first N
   tests and ignores what follows, so a byte prefix of an upstream file
   converts; without it the array must hold exactly 10000 tests. A bad input
   exits 1 with "vecconv: <in>: offset <n>: <reason>"; a usage error exits
   2. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "n65v.h"
#include "sst_json.h"

static int usage(void)
{
    fprintf(stderr, "usage: vecconv <in.json> <out.n65v> <opcode-hex> [--first N]\n");
    return 2;
}

static int write_file(const char *path, const uint8_t *data, size_t len)
{
    FILE *f = fopen(path, "wb");
    int ok;
    if (f == NULL)
        return 0;
    ok = fwrite(data, 1u, len, f) == len;
    if (fclose(f) != 0)
        ok = 0;
    return ok;
}

/* Reads path back: its bytes must equal what was written, and the shared
   reader must accept it as one chunk of count tests of opcode. */
static int reread(const char *path, const uint8_t *data, size_t len, uint8_t opcode, uint32_t count)
{
    static struct n65v_test t;
    struct n65v_reader r;
    size_t file_len = 0u;
    uint32_t n = 0u;
    uint8_t op = 0u;
    int rc;
    int ok = 1;
    uint8_t *file = n65v_read_file(path, &file_len);
    if (file == NULL) {
        fprintf(stderr, "vecconv: %s: cannot read back\n", path);
        return 0;
    }
    if (file_len != len || memcmp(file, data, len) != 0) {
        fprintf(stderr, "vecconv: %s: re-read: the file differs from what was written\n", path);
        free(file);
        return 0;
    }
    n65v_init(&r, file, file_len, opcode, 1u, count);
    while ((rc = n65v_next(&r, &t, &op)) == 1) {
        if (op != opcode || (uint32_t)t.index != n) {
            fprintf(stderr, "vecconv: %s: re-read: test %lu has opcode 0x%02x, index %u\n", path,
                    (unsigned long)n, (unsigned)op, (unsigned)t.index);
            ok = 0;
            break;
        }
        n++;
    }
    if (ok && rc < 0) {
        fprintf(stderr, "vecconv: %s: re-read: %s\n", path, r.error);
        ok = 0;
    }
    if (ok && n != count) {
        fprintf(stderr, "vecconv: %s: re-read: %lu tests, expected %lu\n", path, (unsigned long)n,
                (unsigned long)count);
        ok = 0;
    }
    free(file);
    return ok;
}

int main(int argc, char **argv)
{
    uint32_t first = 0u;
    uint32_t count = 0u;
    uint8_t *out = NULL;
    size_t out_len = 0u;
    char err[160];
    int hi, lo, ok;

    if (argc != 4 && argc != 6)
        return usage();
    hi = strlen(argv[3]) == 2u ? n65v_hex_digit(argv[3][0]) : -1;
    lo = strlen(argv[3]) == 2u ? n65v_hex_digit(argv[3][1]) : -1;
    if (hi < 0 || lo < 0)
        return usage();
    uint8_t opcode = (uint8_t)((hi << 4) | lo);
    if (argc == 6) {
        const char *s = argv[5];
        if (strcmp(argv[4], "--first") != 0 || *s == '\0' || strlen(s) > 5u)
            return usage();
        for (; *s != '\0'; s++) {
            if (*s < '0' || *s > '9')
                return usage();
            first = first * 10u + (uint32_t)(*s - '0');
        }
        if (first == 0u || first > SST_JSON_FULL_COUNT)
            return usage();
    }

    size_t in_len = 0u;
    uint8_t *in = n65v_read_file(argv[1], &in_len);
    if (in == NULL) {
        fprintf(stderr, "vecconv: %s: cannot read\n", argv[1]);
        return 1;
    }
    ok = sst_json_convert(in, in_len, opcode, first, &out, &out_len, &count, err, sizeof err);
    free(in);
    if (!ok) {
        fprintf(stderr, "vecconv: %s: %s\n", argv[1], err);
        return 1;
    }
    if (!write_file(argv[2], out, out_len)) {
        fprintf(stderr, "vecconv: %s: cannot write\n", argv[2]);
        ok = 0;
    }
    if (ok)
        ok = reread(argv[2], out, out_len, opcode, count);
    free(out);
    if (!ok)
        return 1;
    printf("vecconv: %02x: %lu tests written and re-read\n", (unsigned)opcode,
           (unsigned long)count);
    return 0;
}
