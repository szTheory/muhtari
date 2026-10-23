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
#ifdef DIAG_LIBRETRO
#include "libretro.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <dlfcn.h>
#endif
#endif

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
static int alloc_fail;
static uint32_t alloc_calls;

static void *diag_alloc(void *user, size_t size)
{
    (void)user;
    alloc_calls++;
    return alloc_fail ? NULL : malloc(size);
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

/* Retail observations use only the fixed mailbox, on every host. IDs are
   owned diagnostic identities, independent of the cartridge implementation. */
static uint32_t retail_id(const char *rom)
{
    return strcmp(rom, "smoke") == 0 ? 9u : strcmp(rom, "activision") == 0 ? 10u :
           strcmp(rom, "absolute") == 0 ? 11u : strcmp(rom, "fractalus") == 0 ? 12u : 0u;
}

static uint16_t retail_crc(const uint8_t mb[256])
{
    uint32_t crc = 0xFFFFu;
    for (uint32_t i = 16u; i < 256u; i++) {
        crc ^= (uint32_t)mb[i] << 8;
        for (unsigned b = 0; b < 8u; b++) {
            crc = ((crc << 1) ^ ((crc & 0x8000u) != 0u ? 0x1021u : 0u)) & 0xFFFFu;
        }
    }
    return (uint16_t)crc;
}

static int retail_judge(const uint8_t mb[256], const char *rom)
{
    uint32_t count = strcmp(rom, "smoke") == 0 ? 1u : nrows;
    if (retail_id(rom) == 0u || count == 0u || count > 27u ||
        memcmp(mb, "M7DG", 4u) != 0 || mb[4] != 1u || mb[5] != 0u ||
        mb[6] != count || mb[7] != retail_id(rom) ||
        ((uint32_t)mb[8] | ((uint32_t)mb[9] << 8)) != retail_crc(mb)) {
        fprintf(stderr, "%s: invalid mailbox header/count/CRC\n", rom);
        return 1;
    }
    for (uint32_t i = 0; i < count; i++) {
        const uint8_t *r = mb + 16u + 4u * i;
        uint32_t value = (uint32_t)r[2] | ((uint32_t)r[3] << 8);
        int valid = strcmp(rom, "smoke") == 0 ? value == 90u : in_range(&rows[i].value, value);
        if (r[0] != i + 1u || r[1] != 1u || !valid) {
            fprintf(stderr, "%s: invalid record %u value %u\n", rom, i + 1u, value);
            return 1;
        }
        printf("%s record %u value %u PASS\n", rom, i + 1u, value);
    }
    return 0;
}

static int retail_expected(const char *path, const char *rom)
{
    nrows = 0u;
    return strcmp(rom, "smoke") == 0 ? 0 : read_expected(path, rom);
}

static int retail_mailbox(const char *path, const char *rom, const char *expected)
{
    /* Exactly one bounded lowercase report, with no trailing output. */
    char line[525];
    uint8_t mb[256];
    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        return 1;
    }
    size_t n = fread(line, 1u, sizeof line, f);
    int failed = ferror(f) || n != 524u || memcmp(line, "diagnostic ", 11u) != 0 ||
                 line[523] != '\n';
    fclose(f);
    if (failed) {
        return 1;
    }
    for (uint32_t i = 0; i < 512u; i++) {
        unsigned char c = (unsigned char)line[11u + i];
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'))) {
            return 1;
        }
        uint8_t v = (uint8_t)(c <= '9' ? c - '0' : c - 'a' + 10);
        if ((i & 1u) == 0u) {
            mb[i / 2u] = (uint8_t)(v << 4);
        } else {
            mb[i / 2u] |= v;
        }
    }
    return retail_expected(expected, rom) != 0 || retail_judge(mb, rom) != 0;
}

static int retail_library(const uint8_t *image, size_t n, const char *rom)
{
    muhtari_config cfg = {0};
    muhtari_media media = {0};
    muhtari_cart_info info = {0};
    muhtari *m = NULL;
    cfg.size = (uint32_t)sizeof cfg;
    cfg.allocator.alloc = diag_alloc;
    cfg.allocator.free = diag_free;
    media.size = (uint32_t)sizeof media;
    media.image = image;
    media.image_size = n;
    info.size = (uint32_t)sizeof info;
    uint32_t board = strcmp(rom, "activision") == 0 ? MUHTARI_BOARD_ACTIVISION :
                     strcmp(rom, "fractalus") == 0 ? MUHTARI_BOARD_FRACTALUS : MUHTARI_BOARD_ABSOLUTE;
    if (muhtari_probe(image, n, &info) != MUHTARI_STATUS_OK || info.board != board ||
        muhtari_create(&cfg, &m) != MUHTARI_STATUS_OK || muhtari_load(m, &media) != MUHTARI_STATUS_OK) {
        muhtari_destroy(m);
        return 1;
    }
    int failed = run(m, 0) < 0;
    uint8_t mb[256];
    for (uint32_t i = 0; i < 256u; i++) {
        mb[i] = muhtari__peek(m, (uint16_t)(MB + i));
    }
    failed |= retail_judge(mb, rom);
    muhtari_destroy(m);
    return failed;
}

/* The tracer also protects the existing load transaction. Refusals cannot
   allocate or change any live state, ROM bytes or subsequent execution. */
static int retail_transaction(uint8_t *image)
{
    muhtari_config cfg = {0};
    muhtari_media media = {0};
    muhtari *m = NULL, *control = NULL;
    muhtari *before = (muhtari *)malloc(sizeof *before);
    uint8_t *rom_before = (uint8_t *)malloc(65536u);
    cfg.size = (uint32_t)sizeof cfg;
    cfg.allocator.alloc = diag_alloc;
    cfg.allocator.free = diag_free;
    media.size = (uint32_t)sizeof media;
    media.image = image;
    media.image_size = 65664u;
    int failed = before == NULL || rom_before == NULL ||
                 muhtari_create(&cfg, &m) != MUHTARI_STATUS_OK ||
                 muhtari_create(&cfg, &control) != MUHTARI_STATUS_OK ||
                 muhtari_load(m, &media) != MUHTARI_STATUS_OK ||
                 muhtari_load(control, &media) != MUHTARI_STATUS_OK;
    if (!failed) {
        /* Independent expected latch, all data values and selector neighbors. */
        uint8_t bank = 0u;
        static const uint16_t addresses[] = {0x7FFFu, 0x8000u, 0x8001u, 0xFFFFu};
        for (uint32_t v = 0; v < 256u; v++) {
            for (size_t a = 0; a < sizeof addresses / sizeof addresses[0]; a++) {
                muhtari__cart_write(m, addresses[a], (uint8_t)v);
                if (addresses[a] == 0x8000u) {
                    bank = (v & 1u) != 0u ? 0u : (v & 2u) != 0u ? 1u : bank;
                }
                uint8_t tag = bank == 0u ? 90u : 165u;
                failed |= muhtari__cart_peek(m, 0x4000u) != tag ||
                          muhtari__cart_read(m, 0x4000u, 0) != tag ||
                          muhtari__cart_read(m, 0x4000u, 1) != tag ||
                          muhtari__cart_peek(m, 0xC000u) != 0x78u;
            }
        }
        muhtari__cart_write(m, 0x8000u, 2u);
        muhtari__cart_write(control, 0x8000u, 2u);
        memcpy(before, m, sizeof *m);
        memcpy(rom_before, m->cart.rom, 65536u);
        uint32_t calls = alloc_calls;
        media.image_size--;
        failed |= muhtari_load(m, &media) != MUHTARI_STATUS_ERR_TRUNCATED_IMAGE;
        failed |= alloc_calls != calls || memcmp(m, before, sizeof *m) != 0 ||
                  memcmp(m->cart.rom, rom_before, 65536u) != 0;
        image[50] = 0u;
        image[51] = 0xC0u;
        media.image_size = 65664u;
        failed |= muhtari_load(m, &media) != MUHTARI_STATUS_ERR_INVALID_IMAGE;
        failed |= alloc_calls != calls || memcmp(m, before, sizeof *m) != 0 ||
                  memcmp(m->cart.rom, rom_before, 65536u) != 0;
        image[50] = 1u;
        image[51] = 0u;
        alloc_fail = 1;
        failed |= muhtari_load(m, &media) != MUHTARI_STATUS_ERR_NO_MEMORY;
        alloc_fail = 0;
        failed |= alloc_calls != calls + 1u || memcmp(m, before, sizeof *m) != 0 ||
                  memcmp(m->cart.rom, rom_before, 65536u) != 0;
        muhtari_frame io = {0}, other = {0};
        io.size = (uint32_t)sizeof io;
        other.size = (uint32_t)sizeof other;
        failed |= muhtari_run_frame(m, &io) != MUHTARI_STATUS_OK ||
                  muhtari_run_frame(control, &other) != MUHTARI_STATUS_OK;
        for (uint32_t a = 0x1800u; a < 0x2800u; a++) {
            failed |= muhtari__peek(m, (uint16_t)a) != muhtari__peek(control, (uint16_t)a);
        }
        failed |= memcmp(&m->cpu, &control->cpu, sizeof m->cpu) != 0;
        failed |= muhtari_power_cycle(m) != MUHTARI_STATUS_OK || m->cart.bank != 0u;
        muhtari__cart_write(m, 0x8000u, 2u);
        failed |= muhtari_load(m, &media) != MUHTARI_STATUS_OK || m->cart.bank != 0u;
        /* v4 permits a write-only POKEY over the low ROM window. CPU,
           MARIA and peek still read ROM; audio takes writes first. */
        image[0] = 4u;
        image[64] = 3u;
        image[67] = 5u;
        failed |= muhtari_load(m, &media) != MUHTARI_STATUS_OK;
        failed |= muhtari__cart_read(m, 0x4000u, 0) != 90u ||
                  muhtari__cart_read(m, 0x4000u, 1) != 90u;
        muhtari__cart_write(m, 0x4000u, 0xFFu);
        failed |= muhtari__cart_peek(m, 0x4000u) != 90u || m->cart.bank != 0u;
        image[0] = 3u;
        image[64] = 0u;
        image[67] = 0u;
    }
    muhtari_destroy(m);
    muhtari_destroy(control);
    free(before);
    free(rom_before);
    return failed;
}

/* An owned disposable program: copy a complete mailbox from fixed $C100,
   replace the record with a real bank-zero read, then publish status last.
   The expected tag and CRC are derived from the mailbox protocol, never
   from the production mapper. No ROM bytes enter the tracked workspace. */
static int retail_smoke(const char *path)
{
    uint8_t *image = (uint8_t *)calloc(65664u, 1u);
    if (image == NULL) {
        return 1;
    }
    image[0] = 3u;
    memcpy(image + 1u, "ATARI7800", 9u);
    image[50] = 1u;
    image[53] = 2u;
    uint8_t *body = image + 128u;
    body[0] = 90u;
    body[0x4000] = 165u;
    static const uint8_t code[] = {
        0x78, 0xD8, 0xA9, 0x07, 0x85, 0x01, 0xA9, 0x60, 0x85, 0x3C,
        0xA2, 0x00, 0xBD, 0x00, 0xC1, 0x9D, 0x00, 0x22, 0xE8, 0xD0, 0xF7,
        0xAD, 0x00, 0x40, 0x8D, 0x12, 0x22, 0xA9, 0x00, 0x8D, 0x05, 0x22,
        0x4C, 0x20, 0xC0
    };
    memcpy(body + 0xC000u, code, sizeof code);
    uint8_t *mb = body + 0xC100u;
    memcpy(mb, "M7DG", 4u);
    mb[4] = 1u;
    mb[5] = 0x80u;
    mb[6] = 1u;
    mb[7] = 9u;
    mb[16] = 1u;
    mb[17] = 1u;
    mb[18] = 90u;
    uint16_t crc = retail_crc(mb);
    mb[8] = (uint8_t)crc;
    mb[9] = (uint8_t)(crc >> 8);
    body[0xFFF8] = 0xFFu;
    body[0xFFF9] = 0xC7u;
    body[0xFFFD] = 0xC0u;
    FILE *f = fopen(path, "wb");
    int failed = f == NULL;
    if (f != NULL) {
        failed = fwrite(image, 1u, 65664u, f) != 65664u;
        failed |= fclose(f) != 0;
    }
    if (!failed) {
        failed = retail_library(image, 65664u, "smoke");
        failed |= retail_transaction(image);
    }
    /* Exercise protocol faults even when the record CRC is internally valid. */
    mb[5] = 0u;
    static const uint8_t corrupt_at[] = {0u, 4u, 5u, 6u, 7u, 8u, 16u, 17u, 18u};
    for (size_t i = 0; i < sizeof corrupt_at; i++) {
        uint8_t bad[256];
        memcpy(bad, mb, sizeof bad);
        bad[corrupt_at[i]] ^= 1u;
        if (corrupt_at[i] >= 16u) {
            crc = retail_crc(bad);
            bad[8] = (uint8_t)crc;
            bad[9] = (uint8_t)(crc >> 8);
        }
        failed |= retail_judge(bad, "smoke") == 0;
    }
    mb[5] = 0x80u;
    free(image);
    return failed;
}

#ifdef DIAG_LIBRETRO
static bool RETRO_CALLCONV retail_env(unsigned cmd, void *data)
{
    return cmd == RETRO_ENVIRONMENT_SET_PIXEL_FORMAT || cmd == RETRO_ENVIRONMENT_SET_MEMORY_MAPS ||
           cmd == RETRO_ENVIRONMENT_SET_SUPPORT_ACHIEVEMENTS ||
           (cmd == RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE && (*(bool *)data = false, true));
}
static void RETRO_CALLCONV retail_video(const void *data, unsigned w, unsigned h, size_t pitch)
{
    (void)data; (void)w; (void)h; (void)pitch;
}
static size_t RETRO_CALLCONV retail_audio(const int16_t *data, size_t frames)
{
    (void)data;
    return frames;
}
static void RETRO_CALLCONV retail_poll(void) {}
static int16_t RETRO_CALLCONV retail_input(unsigned p, unsigned d, unsigned i, unsigned id)
{
    (void)p; (void)d; (void)i; (void)id;
    return 0;
}

static int retail_libretro(const char *module_path, const char *path, const char *expected)
{
    char rom[FIELD_MAX];
    size_t n = 0;
    rom_name(path, rom);
    uint8_t *image = read_file(path, &n);
    if (image == NULL || retail_expected(expected, rom) != 0) {
        free(image);
        return 1;
    }
#ifdef _WIN32
    HMODULE module = LoadLibraryA(module_path);
#define DIAG_SYMBOL(name) FARPROC symbol = GetProcAddress(module, "retro_" #name)
#define DIAG_CLOSE() FreeLibrary(module)
#else
    void *module = dlopen(module_path, RTLD_NOW);
#define DIAG_SYMBOL(name) void *symbol = dlsym(module, "retro_" #name)
#define DIAG_CLOSE() ((void)dlclose(module))
#endif
    if (module == NULL) {
        free(image);
        return 1;
    }
#define DIAG_LOAD(name, result, args) \
    result (RETRO_CALLCONV *name) args; \
    { DIAG_SYMBOL(name); \
      if (symbol == NULL) { DIAG_CLOSE(); free(image); return 1; } \
      memcpy(&name, &symbol, sizeof name); }
    DIAG_LOAD(set_environment, void, (retro_environment_t));
    DIAG_LOAD(set_video_refresh, void, (retro_video_refresh_t));
    DIAG_LOAD(set_audio_sample_batch, void, (retro_audio_sample_batch_t));
    DIAG_LOAD(set_input_poll, void, (retro_input_poll_t));
    DIAG_LOAD(set_input_state, void, (retro_input_state_t));
    DIAG_LOAD(init, void, (void));
    DIAG_LOAD(deinit, void, (void));
    DIAG_LOAD(load_game, bool, (const struct retro_game_info *));
    DIAG_LOAD(unload_game, void, (void));
    DIAG_LOAD(run, void, (void));
    DIAG_LOAD(get_memory_data, void *, (unsigned));
    DIAG_LOAD(get_memory_size, size_t, (unsigned));
#undef DIAG_LOAD
    set_environment(retail_env);
    set_video_refresh(retail_video);
    set_audio_sample_batch(retail_audio);
    set_input_poll(retail_poll);
    set_input_state(retail_input);
    init();
    struct retro_game_info game = {path, image, n, NULL};
    int loaded = load_game(&game);
    int failed = 1;
    if (loaded) {
        for (uint32_t frame = 0; frame < FRAME_CAP; frame++) {
            run();
            const uint8_t *ram = (const uint8_t *)get_memory_data(RETRO_MEMORY_SYSTEM_RAM);
            if (ram != NULL && get_memory_size(RETRO_MEMORY_SYSTEM_RAM) >= 0xB00u &&
                memcmp(ram + 0xA00u, "M7DG", 4u) == 0 && ram[0xA05] == 0u) {
                failed = retail_judge(ram + 0xA00u, rom);
                break;
            }
        }
        unload_game();
    }
    deinit();
    DIAG_CLOSE();
#undef DIAG_SYMBOL
#undef DIAG_CLOSE
    free(image);
    return failed;
}
#endif

int main(int argc, char **argv)
{
    char rom[FIELD_MAX];
    if (argc == 3 && strcmp(argv[1], "--smoke") == 0) {
        return retail_smoke(argv[2]);
    }
    if (argc == 5 && strcmp(argv[1], "--mailbox") == 0) {
        return retail_mailbox(argv[2], argv[3], argv[4]);
    }
#ifdef DIAG_LIBRETRO
    if (argc == 5 && strcmp(argv[1], "--libretro") == 0) {
        return retail_libretro(argv[2], argv[3], argv[4]);
    }
#endif
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
    if (retail_id(rom) != 0u) {
        int failed = retail_library(image, n, rom);
        free(image);
        return failed;
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
