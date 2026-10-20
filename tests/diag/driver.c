/* muhtari-test-diag ROM EXPECTED [STATS]: runs a diagnostic ROM and judges
   what it reports (docs/emulation-tests.md, diag).

   It loads ROM with muhtari_load (fast boot) and, at the end of each frame,
   the VBLANK start, reads the mailbox at $2200 through muhtari__peek, which
   changes no state. It stops when the mailbox has its magic and status $00,
   and fails as "hang" after 600 frames. It then checks the mailbox's version
   and CRC-16 and judges record n against the n-th row of EXPECTED for the
   ROM, named by ROM's file name without its extension:

     rom,test,value,source     value is N or LO-HI, source hw|guide|rtl|fit

   A missing or extra record fails. With STATS, each record is also checked
   against the core's statistics on the frame that measured it: the first
   line from 17 to 258 that ran DMA must show the row's cost, by its kind,
   and the row's objects:

     rom,test,ordinary,last,objects      each N, LO-HI or - (not expected)

   One line is printed per record; exit 0 when all pass, 1 when any fails,
   and 2 on a usage error or a file that cannot be read. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"

#define FRAME_CAP 600u
#define ROWS_MAX 64u
#define FIELD_MAX 32u
#define MB 0x2200u
#define MB_VERSION 0x2204u
#define MB_STATUS 0x2205u
#define MB_COUNT 0x2206u
#define MB_ROMID 0x2207u
#define MB_CRC 0x2208u
#define MB_REC 0x2210u
#define MB_END 0x2300u
#define FIRST_DMA_LINE 17u
#define LAST_DMA_LINE 258u

/* A value or range from a CSV field; none is set for "-". */
struct range {
    uint32_t lo, hi;
    int none;
};

struct row {
    char test[FIELD_MAX];
    struct range value;
    char source[FIELD_MAX];
    struct range ordinary, last, objects;
    int has_stats;
};

static struct row rows[ROWS_MAX];
static uint32_t nrows;

static void *diag_alloc(void *user, size_t size)
{
    (void)user;
    return malloc(size);
}

static void diag_free(void *user, void *ptr, size_t size)
{
    (void)user;
    (void)size;
    free(ptr);
}

/* Parses "N", "LO-HI" or, when dash_ok, "-". Returns 0 on success. */
static int parse_range(const char *s, struct range *r, int dash_ok)
{
    uint32_t v[2] = {0u, 0u};
    int part = 0;
    int digits = 0;
    r->none = 0;
    if (dash_ok && strcmp(s, "-") == 0) {
        r->none = 1;
        return 0;
    }
    for (const char *p = s; *p != '\0'; p++) {
        if (*p >= '0' && *p <= '9' && v[part] < 100000u) {
            v[part] = v[part] * 10u + (uint32_t)(*p - '0');
            digits++;
        } else if (*p == '-' && part == 0 && digits > 0) {
            part = 1;
            digits = 0;
        } else {
            return 1;
        }
    }
    if (digits == 0) {
        return 1;
    }
    r->lo = v[0];
    r->hi = part == 1 ? v[1] : v[0];
    return r->lo <= r->hi ? 0 : 1;
}

static int in_range(const struct range *r, uint32_t v)
{
    return v >= r->lo && v <= r->hi;
}

static void print_range(const struct range *r)
{
    if (r->lo == r->hi) {
        printf("%lu", (unsigned long)r->lo);
    } else {
        printf("%lu-%lu", (unsigned long)r->lo, (unsigned long)r->hi);
    }
}

/* Splits a CSV line in place into at most max fields. Returns the count. */
static uint32_t split(char *line, char **field, uint32_t max)
{
    uint32_t n = 0;
    line[strcspn(line, "\r\n")] = '\0';
    for (char *p = line; n < max;) {
        field[n++] = p;
        p = strchr(p, ',');
        if (p == NULL) {
            break;
        }
        *p++ = '\0';
    }
    return n;
}

/* Reads EXPECTED's rows for rom. Returns 0, or 2 after a message. */
static int read_expected(const char *path, const char *rom)
{
    char line[256];
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        fprintf(stderr, "muhtari-test-diag: cannot open %s\n", path);
        return 2;
    }
    for (uint32_t ln = 1; fgets(line, sizeof line, f) != NULL; ln++) {
        char *field[5];
        if (ln == 1 || line[0] == '\n') {
            continue;
        }
        uint32_t n = split(line, field, 5);
        if (n != 4 || strcmp(field[0], rom) != 0) {
            if (n != 4) {
                fprintf(stderr, "%s:%lu: want rom,test,value,source\n", path, (unsigned long)ln);
                fclose(f);
                return 2;
            }
            continue;
        }
        struct row *r = &rows[nrows];
        if (nrows == ROWS_MAX || strlen(field[1]) >= FIELD_MAX ||
            parse_range(field[2], &r->value, 0) != 0 ||
            (strcmp(field[3], "hw") != 0 && strcmp(field[3], "guide") != 0 &&
             strcmp(field[3], "rtl") != 0 && strcmp(field[3], "fit") != 0)) {
            fprintf(stderr, "%s:%lu: bad row\n", path, (unsigned long)ln);
            fclose(f);
            return 2;
        }
        memcpy(r->test, field[1], strlen(field[1]) + 1u);
        memcpy(r->source, field[3], strlen(field[3]) + 1u);
        nrows++;
    }
    fclose(f);
    return 0;
}

/* Reads STATS's rows for rom onto the expected rows they name. */
static int read_stats(const char *path, const char *rom)
{
    char line[256];
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        fprintf(stderr, "muhtari-test-diag: cannot open %s\n", path);
        return 2;
    }
    for (uint32_t ln = 1; fgets(line, sizeof line, f) != NULL; ln++) {
        char *field[6];
        uint32_t i;
        if (ln == 1 || line[0] == '\n') {
            continue;
        }
        uint32_t n = split(line, field, 6);
        if (n != 5) {
            fprintf(stderr, "%s:%lu: want rom,test,ordinary,last,objects\n", path,
                    (unsigned long)ln);
            fclose(f);
            return 2;
        }
        if (strcmp(field[0], rom) != 0) {
            continue;
        }
        for (i = 0; i < nrows && strcmp(rows[i].test, field[1]) != 0; i++) {
        }
        if (i == nrows || parse_range(field[2], &rows[i].ordinary, 1) != 0 ||
            parse_range(field[3], &rows[i].last, 1) != 0 ||
            parse_range(field[4], &rows[i].objects, 1) != 0) {
            fprintf(stderr, "%s:%lu: bad row or a test with no expected row\n", path,
                    (unsigned long)ln);
            fclose(f);
            return 2;
        }
        rows[i].has_stats = 1;
    }
    fclose(f);
    return 0;
}

/* Checks the frame's first DMA line against row r's statistics. Returns 0
   when they agree. */
static int check_stats(const muhtari *m, const struct row *r)
{
    for (uint32_t line = FIRST_DMA_LINE; line <= LAST_DMA_LINE; line++) {
        const struct muhtari__line_stats *st = &m->maria.stats[line];
        if ((st->flags & MUHTARI__LINE_DMA_OFF) != 0u) {
            continue;
        }
        int last = (st->flags & MUHTARI__LINE_LAST) != 0u;
        const struct range *cost = last ? &r->last : &r->ordinary;
        int ok = (st->flags & MUHTARI__LINE_DMA_BLOCKED) == 0u && !cost->none &&
                 in_range(cost, st->cost) &&
                 (r->objects.none || in_range(&r->objects, st->objects));
        printf("  %s: stats %s: line %lu %s, cost %lu, objects %lu\n", r->test,
               ok ? "agree" : "disagree", (unsigned long)line, last ? "last" : "ordinary",
               (unsigned long)st->cost, (unsigned long)st->objects);
        return ok ? 0 : 1;
    }
    printf("  %s: stats disagree: no line ran DMA\n", r->test);
    return 1;
}

/* CRC-16, polynomial 0x1021, initial 0xFFFF, most significant bit first, no
   final XOR, over $2210-$22FF: lib.s's crc16. */
static uint16_t mailbox_crc(const muhtari *m)
{
    uint32_t crc = 0xFFFFu;
    for (uint32_t a = MB_REC; a < MB_END; a++) {
        crc ^= (uint32_t)muhtari__peek(m, (uint16_t)a) << 8;
        for (int b = 0; b < 8; b++) {
            crc = (crc & 0x8000u) != 0u ? ((crc << 1) ^ 0x1021u) & 0xFFFFu : (crc << 1) & 0xFFFFu;
        }
    }
    return (uint16_t)crc;
}

static int has_magic(const muhtari *m)
{
    return muhtari__peek(m, MB) == 'M' && muhtari__peek(m, MB + 1u) == '7' &&
           muhtari__peek(m, MB + 2u) == 'D' && muhtari__peek(m, MB + 3u) == 'G';
}

/* Runs to status $00, checking each new record's statistics on the frame
   it appears. Returns the failures, or -1 on a hang or a run error. */
static int run(muhtari *m, int with_stats)
{
    int failures = 0;
    uint32_t seen = 0;
    for (uint32_t f = 1; f <= FRAME_CAP; f++) {
        muhtari_frame io;
        memset(&io, 0, sizeof io);
        io.size = (uint32_t)sizeof io;
        if (muhtari_run_frame(m, &io) != MUHTARI_STATUS_OK) {
            printf("frame %lu failed\n", (unsigned long)f);
            return -1;
        }
        if (!has_magic(m)) {
            continue;
        }
        uint32_t count = muhtari__peek(m, MB_COUNT);
        for (; seen < count && seen < nrows; seen++) {
            if (with_stats && rows[seen].has_stats) {
                failures += check_stats(m, &rows[seen]);
            }
        }
        if (muhtari__peek(m, MB_STATUS) == 0u) {
            printf("done at frame %lu\n", (unsigned long)f);
            return failures;
        }
    }
    printf("hang: no status $00 within %lu frames\n", (unsigned long)FRAME_CAP);
    return -1;
}

/* Judges the records against the rows. Returns the failures. */
static int judge(const muhtari *m, const char *rom)
{
    int failures = 0;
    uint32_t count = muhtari__peek(m, MB_COUNT);
    uint32_t n = count > nrows ? count : nrows;
    for (uint32_t i = 0; i < n; i++) {
        uint16_t at = (uint16_t)(MB_REC + 4u * i);
        if (i >= count) {
            printf("%s %s: FAIL missing record %lu\n", rom, rows[i].test, (unsigned long)(i + 1u));
            failures++;
            continue;
        }
        uint32_t id = muhtari__peek(m, at);
        uint32_t flags = muhtari__peek(m, (uint16_t)(at + 1u));
        uint32_t v = muhtari__peek(m, (uint16_t)(at + 2u)) |
                     ((uint32_t)muhtari__peek(m, (uint16_t)(at + 3u)) << 8);
        if (i >= nrows) {
            printf("%s: FAIL extra record %lu, id %lu, value %lu\n", rom, (unsigned long)(i + 1u),
                   (unsigned long)id, (unsigned long)v);
            failures++;
            continue;
        }
        int ok = id == i + 1u && (flags & 1u) != 0u && in_range(&rows[i].value, v);
        printf("%s %s: %lu expected ", rom, rows[i].test, (unsigned long)v);
        print_range(&rows[i].value);
        printf(" (%s) id %lu flags %02lx %s\n", rows[i].source, (unsigned long)id,
               (unsigned long)flags, ok ? "PASS" : "FAIL");
        failures += ok ? 0 : 1;
    }
    return failures;
}

/* Reads the whole file into a malloc'd buffer. Returns NULL after a message. */
static uint8_t *read_file(const char *path, size_t *n)
{
    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        fprintf(stderr, "muhtari-test-diag: cannot open %s\n", path);
        return NULL;
    }
    uint8_t *buf = (uint8_t *)malloc(MUHTARI_IMAGE_SIZE_MAX + 1u);
    size_t got = buf != NULL ? fread(buf, 1u, MUHTARI_IMAGE_SIZE_MAX + 1u, f) : 0u;
    int failed = buf == NULL || ferror(f) || got > MUHTARI_IMAGE_SIZE_MAX;
    fclose(f);
    if (failed) {
        fprintf(stderr, "muhtari-test-diag: cannot read %s\n", path);
        free(buf);
        return NULL;
    }
    *n = got;
    return buf;
}

/* The ROM's name: its file name up to the first dot. */
static void rom_name(const char *path, char *out)
{
    const char *base = path;
    for (const char *p = path; *p != '\0'; p++) {
        if (*p == '/' || *p == '\\') {
            base = p + 1;
        }
    }
    size_t len = strcspn(base, ".");
    len = len < FIELD_MAX - 1u ? len : FIELD_MAX - 1u;
    memcpy(out, base, len);
    out[len] = '\0';
}

int main(int argc, char **argv)
{
    char rom[FIELD_MAX];
    if (argc != 3 && argc != 4) {
        fprintf(stderr, "usage: muhtari-test-diag ROM EXPECTED [STATS]\n");
        return 2;
    }
    rom_name(argv[1], rom);
    if (read_expected(argv[2], rom) != 0 || (argc == 4 && read_stats(argv[3], rom) != 0)) {
        return 2;
    }
    if (nrows == 0u) {
        fprintf(stderr, "muhtari-test-diag: %s has no rows for %s\n", argv[2], rom);
        return 2;
    }
    size_t n = 0;
    uint8_t *image = read_file(argv[1], &n);
    if (image == NULL) {
        return 2;
    }
    muhtari_config cfg;
    memset(&cfg, 0, sizeof cfg);
    cfg.size = (uint32_t)sizeof cfg;
    cfg.allocator.alloc = diag_alloc;
    cfg.allocator.free = diag_free;
    muhtari_media media;
    memset(&media, 0, sizeof media);
    media.size = (uint32_t)sizeof media;
    media.image = image;
    media.image_size = n;
    muhtari *m = NULL;
    muhtari_status st = muhtari_create(&cfg, &m);
    if (st == MUHTARI_STATUS_OK) {
        st = muhtari_load(m, &media);
    }
    free(image);
    if (st != MUHTARI_STATUS_OK) {
        printf("%s: load failed with status %lu\n", rom, (unsigned long)st);
        muhtari_destroy(m);
        return 1;
    }
    int failures = run(m, argc == 4);
    if (failures >= 0) {
        uint16_t crc = mailbox_crc(m);
        uint16_t stored =
            (uint16_t)(muhtari__peek(m, MB_CRC) | ((uint16_t)muhtari__peek(m, MB_CRC + 1u) << 8));
        uint32_t version = muhtari__peek(m, MB_VERSION);
        printf("%s: rom id %lu, version %lu, %lu records\n", rom,
               (unsigned long)muhtari__peek(m, MB_ROMID), (unsigned long)version,
               (unsigned long)muhtari__peek(m, MB_COUNT));
        if (version != 1u) {
            printf("%s: FAIL mailbox version %lu, not 1\n", rom, (unsigned long)version);
            failures++;
        }
        if (crc != stored) {
            printf("%s: FAIL CRC mismatch: mailbox %04x, computed %04x\n", rom, stored, crc);
            failures++;
        }
        failures += judge(m, rom);
    }
    muhtari_destroy(m);
    return failures == 0 ? 0 : 1;
}
