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
#include "palette.h"
#include "../../runner/sha256.h"
#include "../runner/png_read.h"
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
static uint32_t battery_count = 1u, battery_marker = 255u;
static const char *battery_import, *battery_export, *battery_firmware, *battery_bmp;

/* Independent raw layout oracle: no header or firmware belongs in save RAM. */
static int battery_bytes(const uint8_t *bytes, size_t n, uint32_t count)
{
    if (bytes == NULL || n != 34816u) {
        fprintf(stderr, "battery: media length %zu, expected 34816\n", n);
        return 1;
    }
    for (size_t i = 0; i < n; i++) {
        uint8_t want = i == 256u || i == 26624u   ? (uint8_t)count
                       : i == 257u || i == 26625u ? (uint8_t)(count >> 8)
                                                  : 255u;
        if (bytes[i] != want) {
            fprintf(stderr, "battery: media byte %zu is %u, expected %u\n", i, (unsigned)bytes[i],
                    (unsigned)want);
            return 1;
        }
    }
    printf("battery: all 34816 bytes match independent count %u\n", count);
    return 0;
}

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
        if (strcmp(rom, "battery") == 0) {
            if (strcmp(field[1], "hsc_count") == 0 || strcmp(field[1], "eeprom_count") == 0)
                r->value.lo = r->value.hi = battery_count;
            if (strcmp(field[1], "firmware_marker") == 0)
                r->value.lo = r->value.hi = battery_marker;
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

static int battery_media_file(const char *path, uint32_t count)
{
    size_t n = 0;
    uint8_t *bytes = read_file(path, &n);
    int failed = bytes == NULL || battery_bytes(bytes, n, count);
    free(bytes);
    return failed;
}

static int battery_controls(const char *path, uint32_t count)
{
    size_t n = 0;
    uint8_t *bytes = read_file(path, &n);
    if (bytes == NULL || battery_bytes(bytes, n, count)) {
        free(bytes);
        return 1;
    }
    int failed = battery_bytes(bytes, n - 1u, count) == 0;
    uint8_t oversized[34817];
    memcpy(oversized, bytes, n);
    oversized[n] = 255u;
    failed |= battery_bytes(oversized, sizeof oversized, count) == 0;
    static const size_t positions[] = {0u, 256u, 257u, 26624u, 26625u, 34815u};
    for (size_t i = 0; i < sizeof positions / sizeof positions[0]; i++) {
        bytes[positions[i]] ^= 1u;
        failed |= battery_bytes(bytes, n, count) == 0;
        bytes[positions[i]] ^= 1u;
    }
    failed |= battery_bytes(bytes, n, count + 1u) == 0;
    free(bytes);
    return failed;
}

static int battery_restore(uint8_t *bytes, size_t n)
{
    if (n != 34816u || bytes == NULL)
        return 1;
    if (battery_import != NULL) {
        size_t size = 0;
        uint8_t *input = read_file(battery_import, &size);
        int failed = input == NULL || battery_bytes(input, size, battery_count - 1u);
        if (!failed)
            memcpy(bytes, input, size);
        free(input);
        return failed;
    }
    for (size_t i = 0; i < n; i++)
        if (bytes[i] != 255u)
            return 1;
    return 0;
}

static int battery_observe(const uint8_t *bytes, size_t n)
{
    if (battery_bytes(bytes, n, battery_count))
        return 1;
    if (battery_export != NULL) {
        FILE *f = fopen(battery_export, "wb");
        if (f == NULL)
            return 1;
        int failed = fwrite(bytes, 1, n, f) != n;
        failed |= fclose(f) != 0;
        return failed;
    }
    return 0;
}

/* Portable screenshot for the shared BMP judge, from executed pixels rather
   than the independent expected glyphs. */
static int battery_picture(muhtari *m)
{
    uint8_t *pixels = malloc(320u * 242u);
    if (pixels == NULL)
        return 1;
    muhtari_frame frame = {0};
    frame.size = (uint32_t)sizeof frame;
    frame.video = pixels;
    frame.video_pitch = 320u;
    int failed = 0;
    for (unsigned i = 0; i < 4u; i++)
        failed |= muhtari_run_frame(m, &frame) != MUHTARI_STATUS_OK;
    FILE *f = failed ? NULL : fopen(battery_bmp, "wb");
    failed |= f == NULL;
    if (f != NULL) {
        uint8_t header[54] = {0};
        static const unsigned offsets[] = {2u, 10u, 14u, 18u, 22u};
        static const uint32_t values[] = {54u + 320u * 242u * 3u, 54u, 40u, 320u, 0u - 242u};
        header[0] = 'B';
        header[1] = 'M';
        header[26] = 1u;
        header[28] = 24u;
        for (unsigned i = 0; i < 5u; i++)
            for (unsigned b = 0; b < 4u; b++)
                header[offsets[i] + b] = (uint8_t)(values[i] >> (8u * b));
        failed |= fwrite(header, 1, sizeof header, f) != sizeof header;
        for (size_t i = 0; i < 320u * 242u && !failed; i++) {
            uint32_t color = muhtari__palette_rgb(pixels[i]);
            uint8_t bgr[] = {(uint8_t)color, (uint8_t)(color >> 8), (uint8_t)(color >> 16)};
            failed |= fwrite(bgr, 1, 3, f) != 3u;
        }
        failed |= fclose(f) != 0;
    }
    free(pixels);
    return failed;
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
    return strcmp(rom, "smoke") == 0          ? 9u
           : strcmp(rom, "activision") == 0   ? 10u
           : strcmp(rom, "absolute") == 0     ? 11u
           : strcmp(rom, "fractalus") == 0    ? 12u
           : strcmp(rom, "battery") == 0      ? 13u
           : strcmp(rom, "pal") == 0          ? 19u
           : strncmp(rom, "bankset", 7u) == 0 ? 20u
           : (strlen(rom) == 8u && strncmp(rom, "motion", 6u) == 0 &&
              (rom[6] == '4' || rom[6] == '5') && rom[7] >= 'a' && rom[7] <= 'c')
               ? 14u
           : (strncmp(rom, "motion", 6u) == 0 && strlen(rom) == 7u && rom[6] >= '0' &&
              rom[6] <= '3')
               ? 14u
           : strcmp(rom, "light0") == 0                                   ? 15u
           : (strcmp(rom, "analog0") == 0 || strcmp(rom, "analog1") == 0) ? 16u
                                                                          : 0u;
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
    if (retail_id(rom) == 0u || count == 0u || count > 27u || memcmp(mb, "M7DG", 4u) != 0 ||
        mb[4] != 1u || mb[5] != 0u || mb[6] != count || mb[7] != retail_id(rom) ||
        ((uint32_t)mb[8] | ((uint32_t)mb[9] << 8)) != retail_crc(mb)) {
        fprintf(stderr,
                "%s: invalid mailbox header/count/CRC (count %u expected %u, identity %u expected "
                "%u)\n",
                rom, (unsigned)mb[6], (unsigned)count, (unsigned)mb[7], (unsigned)retail_id(rom));
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
    if (strncmp(rom, "bankset", 7u) == 0) {
        for (unsigned i = 10u; i < 256u; i++) {
            if ((i < 16u || i >= 16u + 4u * count) && mb[i] != 0u) {
                fprintf(stderr, "bankset: nonzero reserved or extra record bytes\n");
                return 1;
            }
        }
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
    int failed =
        ferror(f) || n != 524u || memcmp(line, "diagnostic ", 11u) != 0 || line[523] != '\n';
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

/* Fixed stimulus, derived before execution in controllers.md, Motion diagnostic.
   The two connectors move in opposite directions and fire on different frames. */
#define CONTROLLER_FRAMES 12u
static int bankset_pal, bankset_refusal;
static char bankset_name[FIELD_MAX];
#define CONTROLLER_HEIGHT (bankset_pal ? 292u : 242u)
#define CONTROLLER_PIXELS (320u * CONTROLLER_HEIGHT)
static uint8_t *controller_pixels;
static uint8_t *bankset_module_pixels;
static int16_t bankset_module_pcm[CONTROLLER_FRAMES][963];
static uint32_t bankset_module_counts[CONTROLLER_FRAMES];
static int16_t controller_pcm[CONTROLLER_FRAMES][963];
static uint32_t controller_counts[CONTROLLER_FRAMES];
static muhtari__sha256 controller_raw;
static int controller_light;
static int controller_analog;
static int controller_keypad;
static int bankset_tracer;
static unsigned controller_motion;
static unsigned controller_adapter, controller_group;

static const char *controller_motion_tag(void)
{
    return controller_motion == 1u   ? "trakball"
           : controller_motion == 2u ? "st-mouse"
                                     : "amiga-mouse";
}

static uint32_t controller_motion_device(void)
{
    return controller_motion == 1u   ? MUHTARI_PORT_TRAKBALL
           : controller_motion == 2u ? MUHTARI_PORT_ST_MOUSE
                                     : MUHTARI_PORT_AMIGA_MOUSE;
}

static const char *controller_name(void)
{
    static char adapter_name[9];
    if (bankset_tracer)
        return bankset_name;
    if (controller_adapter != 0u) {
        snprintf(adapter_name, sizeof adapter_name, "motion%u%c", controller_adapter + 3u,
                 (int)('a' + controller_group));
        return adapter_name;
    }
    return controller_motion == 1u   ? "motion1"
           : controller_motion == 2u ? "motion2"
           : controller_motion == 3u ? "motion3"
           : controller_keypad       ? "analog1"
           : controller_analog       ? "analog0"
           : controller_light        ? "light0"
                                     : "motion0";
}

static void controller_stimulus(unsigned frame, muhtari_input_ex *in)
{
    memset(in, 0, sizeof *in);
    in->size = (uint32_t)sizeof *in;
    if (controller_adapter != 0u) {
        static const uint32_t buttons[2][12] = {
            {16u, 64u, 1024u, 2048u, 1u, 2u, 4u, 8u, 32u, 128u, 256u, 512u},
            {64u, 16u, 2048u, 32u, 256u, 512u, 1024u, 128u, 1u, 2u, 4u, 8u}};
        in->port[0].device = in->port[1].device =
            controller_adapter == 1u ? MUHTARI_PORT_SNES2ATARI : MUHTARI_PORT_MEGA7800;
        in->switches = (frame <= 2u ? MUHTARI_SWITCH_PAUSE |
                                          (controller_adapter == 2u ? MUHTARI_SWITCH_RESET : 0u)
                                    : 0u) |
                       (controller_group == 1u ? MUHTARI_SWITCH_LEFT_DIFFICULTY_A : 0u) |
                       (controller_group == 2u ? MUHTARI_SWITCH_RIGHT_DIFFICULTY_A : 0u);
        for (unsigned p = 0u; p < 2u; p++)
            in->port[p].buttons =
                frame >= 3u && frame <= 6u
                    ? buttons[controller_adapter - 1u]
                             [controller_group * 4u + (p == 0u ? frame - 3u : 6u - frame)]
                : frame == 8u ? 4095u
                              : 0u;
        return;
    }
    if (controller_motion != 0u) {
        in->port[0].device = in->port[1].device = controller_motion_device();
        in->switches = frame <= 2u ? controller_motion : 0u;
        if (frame == 3u) {
            in->port[0].dx = 4;
            in->port[1].dx = -4;
            in->port[0].buttons = MUHTARI_BUTTON_1;
        } else if (frame == 4u || frame == 5u) {
            in->port[0].dx = frame == 4u ? -1 : 0;
            in->port[1].dx = frame == 4u ? 1 : 0;
            in->port[1].buttons = MUHTARI_BUTTON_1;
        } else if (frame == 6u || frame == 7u) {
            in->port[0].dy = frame == 6u ? 4 : -1;
            in->port[1].dy = frame == 6u ? -4 : 1;
            if (controller_motion != 1u)
                in->port[0].buttons = in->port[1].buttons = MUHTARI_BUTTON_2;
        }
        return;
    }
    if (controller_keypad) {
        in->port[0].device = in->port[1].device = MUHTARI_PORT_KEYPAD;
        in->port[0].keypad = in->port[1].keypad = frame <= 3u   ? 15u
                                                  : frame == 5u ? 4095u
                                                  : frame == 6u ? 36u
                                                                : 0u;
        in->switches = frame <= 2u ? MUHTARI_SWITCH_RESET : 0u;
        return;
    }
    if (controller_analog) {
        in->port[0].device = in->port[1].device = MUHTARI_PORT_PADDLE;
        in->port[0].paddle[0] = frame < 6u ? 0u : 65535u;
        in->port[0].paddle[1] = frame < 6u ? 65535u : 0u;
        in->port[1].paddle[0] = frame < 6u ? 32767u : 0u;
        in->port[1].paddle[1] = frame < 6u ? 32768u : 65535u;
        if (frame == 3u || frame == 4u) {
            in->port[0].buttons = MUHTARI_BUTTON_1;
            in->port[1].buttons = MUHTARI_BUTTON_2;
        } else if (frame == 5u || frame == 6u) {
            in->port[0].buttons = MUHTARI_BUTTON_2;
            in->port[1].buttons = MUHTARI_BUTTON_1;
        }
        return;
    }
    if (controller_light) {
        in->port[0].device = in->port[1].device = MUHTARI_PORT_LIGHTGUN;
        for (unsigned p = 0u; p < 2u; p++) {
            in->port[p].gun_x = frame == 6u ? (p == 0u ? 0u : 319u) : 159u;
            in->port[p].gun_y = frame == 6u ? (p == 0u ? 0u : 241u) : 120u;
            in->port[p].flags = frame == 7u || (p == 1u && (frame == 4u || frame == 5u))
                                    ? MUHTARI_INPUT_GUN_OFFSCREEN
                                    : 0u;
        }
        in->port[0].buttons = frame == 3u ? MUHTARI_BUTTON_1 : 0u;
        in->port[1].buttons = frame == 4u || frame == 5u ? MUHTARI_BUTTON_1 : 0u;
        return;
    }
    in->port[0].device = in->port[1].device = MUHTARI_PORT_DRIVING;
    if (frame == 3u) {
        in->port[0].dx = 4;
        in->port[1].dx = -4;
        in->port[0].buttons = MUHTARI_BUTTON_1;
    } else if (frame == 4u || frame == 5u) {
        in->port[0].dx = frame == 4u ? -1 : 0;
        in->port[1].dx = frame == 4u ? 1 : 0;
        in->port[1].buttons = MUHTARI_BUTTON_1;
    }
}

static void controller_raw_sample(void *user, uint64_t tick, uint8_t source, uint16_t raw)
{
    uint8_t bytes[11];
    for (unsigned i = 0u; i < 8u; i++)
        bytes[i] = (uint8_t)(tick >> (8u * i));
    bytes[8] = source;
    bytes[9] = (uint8_t)raw;
    bytes[10] = (uint8_t)(raw >> 8);
    muhtari__sha256_update(user, bytes, sizeof bytes);
}

static void controller_digest(FILE *f, const char *key, muhtari__sha256 *hash)
{
    uint8_t bytes[32];
    char hex[65];
    muhtari__sha256_final(hash, bytes);
    muhtari__sha256_hex(bytes, hex);
    fprintf(f, "%s %s\n", key, hex);
}

static int bankset_native_judge(const uint8_t *frames)
{
    /* DLL zone0 OFFSET15: DMA line32 reads OFFSET0 and raster33 plays it,
       hence native row17 (hardware/maria.md, Rendering). The bounded CPU
       sweeps and CRC finish by call4; calls5..12 are complete display frames. */
    unsigned mode = (unsigned)bankset_name[strlen(bankset_name) - 1u];
    uint8_t first = mode == 's' ? 0x16u : 0x38u;
    uint8_t last = mode == 's' ? 0x27u : 0x38u;
    for (unsigned f = 4u; f < CONTROLLER_FRAMES; f++) {
        const uint8_t *p = frames + f * CONTROLLER_PIXELS + 17u * 320u;
        if (p[0] != first || p[1] != first || p[2] != 0u || p[16] != last ||
            p[20] != (mode == 's' ? 0u : 0x27u)) {
            fprintf(stderr, "bankset: wrong native graphics sentinel on call %u\n", f + 1u);
            return 1;
        }
    }
    return 0;
}

static int controller_library(const char *path, const char *expected, const char *out)
{
    size_t n = 0u;
    uint8_t *image = read_file(path, &n);
    muhtari_config cfg = {0};
    muhtari_media media = {0};
    muhtari *m = NULL;
    cfg.size = (uint32_t)sizeof cfg;
    cfg.allocator.alloc = diag_alloc;
    cfg.allocator.free = diag_free;
    media.size = (uint32_t)sizeof media;
    media.image = image;
    media.image_size = n;
    if (bankset_refusal) {
        int failed = image == NULL || muhtari_create(&cfg, &m) != MUHTARI_STATUS_OK;
        if (!failed)
            failed = muhtari_load(m, &media) != (bankset_refusal == 2
                                                     ? MUHTARI_STATUS_ERR_TRUNCATED_IMAGE
                                                     : MUHTARI_STATUS_ERR_INVALID_IMAGE);
        muhtari_destroy(m);
        free(image);
        return failed;
    }
    media.port[0] = media.port[1] =
        controller_adapter != 0u
            ? (controller_adapter == 1u ? MUHTARI_PORT_SNES2ATARI : MUHTARI_PORT_MEGA7800)
        : controller_motion != 0u ? controller_motion_device()
        : controller_keypad       ? MUHTARI_PORT_KEYPAD
        : controller_analog       ? MUHTARI_PORT_PADDLE
        : controller_light        ? MUHTARI_PORT_LIGHTGUN
                                  : MUHTARI_PORT_DRIVING;
    muhtari__sha256_init(&controller_raw);
    int failed =
        image == NULL || retail_expected(expected, controller_name()) != 0 ||
        muhtari_create(&cfg, &m) != MUHTARI_STATUS_OK ||
        muhtari__synth_observe(m, controller_raw_sample, &controller_raw) != MUHTARI_STATUS_OK ||
        muhtari_load(m, &media) != MUHTARI_STATUS_OK;
    for (unsigned f = 0u; f < CONTROLLER_FRAMES && !failed; f++) {
        muhtari_input_ex in;
        controller_stimulus(f + 1u, &in);
        muhtari_frame io = {0};
        io.size = (uint32_t)sizeof io;
        io.video = controller_pixels + f * CONTROLLER_PIXELS;
        io.video_pitch = 320u;
        io.audio = controller_pcm[f];
        io.audio_capacity = 963u;
        failed = ((f != 4u || controller_analog || controller_keypad || controller_adapter) &&
                  muhtari_set_input_ex(m, &in) != MUHTARI_STATUS_OK) ||
                 muhtari_run_frame(m, &io) != MUHTARI_STATUS_OK;
        controller_counts[f] = io.audio_count;
        if (bankset_tracer && f == 3u && muhtari__peek(m, MB_STATUS) != 0u) {
            fprintf(stderr, "bankset: measurement did not finish by call4\n");
            failed = 1;
        }
    }
    uint8_t mb[256];
    if (!failed) {
        for (unsigned i = 0u; i < 256u; i++)
            mb[i] = muhtari__peek(m, (uint16_t)(MB + i));
        failed = retail_judge(mb, controller_name());
        if (bankset_tracer)
            failed |= bankset_native_judge(controller_pixels);
    }
    /* Numeric judgment precedes all equality evidence. */
    if (!failed) {
        char name[4096];
        /* Real report consumers must reject even CRC-valid numeric faults. */
        for (unsigned fault = 0u; fault < (controller_adapter != 0u  ? 8u
                                           : controller_motion != 0u ? 9u
                                           : controller_keypad       ? 7u
                                           : bankset_tracer          ? 12u
                                                                     : 5u);
             fault++) {
            uint8_t bad[256];
            memcpy(bad, mb, sizeof bad);
            static const unsigned offsets[] = {4u, 6u, 20u, 38u, 8u, 122u, 120u, 86u, 38u};
            if ((controller_adapter == 0u && !bankset_tracer) || fault < 5u)
                bad[offsets[fault]] ^= 1u;
            if (controller_adapter != 0u && fault >= 5u) {
                if (fault == 5u) { /* Missing 17th bit / wrong identification. */
                    bad[controller_adapter == 1u ? 30u : 27u] ^= 1u;
                } else if (fault == 6u) { /* Adjacent pair/serial fields exchanged. */
                    if (controller_adapter == 2u) {
                        if (controller_group == 2u)
                            bad[34] = (uint8_t)((bad[34] << 4) | (bad[34] >> 4));
                        else if (controller_group == 0u)
                            bad[26] = (uint8_t)(((bad[26] & 192u) >> 2) | ((bad[26] & 48u) << 2) |
                                                (bad[26] & 15u));
                        else
                            bad[26] = (uint8_t)(((bad[26] & 12u) >> 2) | ((bad[26] & 3u) << 2) |
                                                (bad[26] & 240u));
                    } else {
                        unsigned other = controller_adapter == 1u ? 34u : 30u;
                        uint8_t tmp = bad[26];
                        bad[26] = bad[other];
                        bad[other] = tmp;
                        tmp = bad[27];
                        bad[27] = bad[other + 1u];
                        bad[other + 1u] = tmp;
                    }
                } else { /* Last scan omitted, even with recomputed CRC. */
                    bad[6]--;
                }
            }
            if (controller_motion != 0u && fault == 7u)
                bad[86] = 0xD7u; /* X tuple substituted for independently expected Y. */
            if (controller_motion != 0u && fault == 8u)
                bad[38] = controller_motion == 3u ? 0xDEu : 0xD7u; /* Wrong family. */
            if (bankset_tracer && fault >= 5u) {
                if (fault == 5u)
                    bad[7] ^= 1u;
                else if (fault == 6u)
                    bad[6]++; /* Extra record. */
                else if (fault == 7u)
                    memcpy(bad + 20u, bad + 16u, 4u); /* Duplicate record. */
                else if (fault == 8u) {
                    uint8_t tmp[4];
                    memcpy(tmp, bad + 16u, 4u);
                    memcpy(bad + 16u, bad + 20u, 4u);
                    memcpy(bad + 20u, tmp, 4u);
                } else if (fault == 9u)
                    bad[18] ^= 1u; /* CRC-valid wrong numeric value. */
                else if (fault == 10u)
                    bad[255] = 1u; /* Extra hidden record data with unchanged count. */
                else
                    bad[17] = 0u; /* Missing validity flag. */
            }
            if (fault != 4u) {
                uint16_t crc = retail_crc(bad);
                bad[8] = (uint8_t)crc;
                bad[9] = (uint8_t)(crc >> 8);
            }
            snprintf(name, sizeof name, "%s/fault-%u.txt", out, fault);
            FILE *bad_file = fopen(name, "wb");
            if (bad_file == NULL) {
                failed = 1;
                break;
            }
            fputs("diagnostic ", bad_file);
            for (unsigned i = 0u; i < 256u; i++)
                fprintf(bad_file, "%02x", (unsigned)bad[i]);
            fputc('\n', bad_file);
            failed |= ferror(bad_file) != 0;
            failed |= fclose(bad_file) != 0;
        }
        snprintf(name, sizeof name, "%s/reference.txt", out);
        FILE *f = fopen(name, "wb");
        if (f == NULL)
            failed = 1;
        else {
            muhtari__sha256 pcm, raw = controller_raw;
            muhtari__sha256_init(&pcm);
            uint32_t samples = 0u;
            for (unsigned i = 0u; i < CONTROLLER_FRAMES; i++) {
                char key[64];
                muhtari__sha256 video_hash;
                muhtari__sha256_init(&video_hash);
                muhtari__sha256_update(&video_hash, controller_pixels + i * CONTROLLER_PIXELS,
                                       CONTROLLER_PIXELS);
                snprintf(key, sizeof key, "frame %u sha256", i + 1u);
                controller_digest(f, key, &video_hash);
                samples += controller_counts[i];
                for (unsigned j = 0u; j < controller_counts[i]; j++) {
                    uint16_t v = (uint16_t)controller_pcm[i][j];
                    uint8_t bytes[2] = {(uint8_t)v, (uint8_t)(v >> 8)};
                    muhtari__sha256_update(&pcm, bytes, sizeof bytes);
                }
            }
            controller_digest(f, "audio_transitions_sha256", &raw);
            controller_digest(f, "audio_pcm_sha256", &pcm);
            fprintf(f, "audio_frames %u\naudio_samples %u\n", CONTROLLER_FRAMES, samples);
            failed |= ferror(f) != 0;
            failed |= fclose(f) != 0;
        }
        snprintf(name, sizeof name, "%s/input.txt", out);
        f = fopen(name, "wb");
        if (f == NULL)
            failed = 1;
        else {
            fprintf(
                f, "muhtari-movie 2\nport1 %s\nport2 %s\n",
                controller_adapter != 0u  ? (controller_adapter == 1u ? "snes2atari" : "mega7800")
                : controller_motion != 0u ? controller_motion_tag()
                : controller_keypad       ? "keypad"
                : controller_analog       ? "paddle"
                : controller_light        ? "lightgun"
                                          : "driving",
                controller_adapter != 0u  ? (controller_adapter == 1u ? "snes2atari" : "mega7800")
                : controller_motion != 0u ? controller_motion_tag()
                : controller_keypad       ? "keypad"
                : controller_analog       ? "paddle"
                : controller_light        ? "lightgun"
                                          : "driving");
            for (unsigned i = 1u; i <= CONTROLLER_FRAMES; i++) {
                muhtari_input_ex in;
                controller_stimulus(i, &in);
                if (controller_adapter != 0u) {
                    const char *tag = controller_adapter == 1u ? "snes2atari" : "mega7800";
                    fprintf(f, "|%s:%u|%s:%u|%c.%c%c%c|\n", tag, in.port[0].buttons, tag,
                            in.port[1].buttons, (in.switches & MUHTARI_SWITCH_RESET) ? 'R' : '.',
                            (in.switches & MUHTARI_SWITCH_PAUSE) ? 'P' : '.',
                            controller_group == 1u ? 'A' : 'B', controller_group == 2u ? 'A' : 'B');
                } else if (controller_motion != 0u) {
                    static const char *switches[] = {"...BB", "R..BB", ".S.BB", "RS.BB"};
                    fprintf(f, "|%s:%d,%d,%u|%s:%d,%d,%u|%s|\n", controller_motion_tag(),
                            in.port[0].dx, in.port[0].dy, in.port[0].buttons >> 4,
                            controller_motion_tag(), in.port[1].dx, in.port[1].dy,
                            in.port[1].buttons >> 4, switches[in.switches]);
                } else if (controller_keypad) {
                    fprintf(f, "|keypad:%u|keypad:%u|%s|\n", in.port[0].keypad, in.port[1].keypad,
                            in.switches != 0u ? "R..BB" : "...BB");
                } else if (controller_analog) {
                    fprintf(f, "|paddle:%u,%u,%u|paddle:%u,%u,%u|...BB|\n", in.port[0].paddle[0],
                            in.port[0].paddle[1], (in.port[0].buttons >> 4) & 3u,
                            in.port[1].paddle[0], in.port[1].paddle[1],
                            (in.port[1].buttons >> 4) & 3u);
                } else if (controller_light) {
                    fprintf(f, "|lightgun:%u,%u,%u|lightgun:%u,%u,%u|...BB|\n", in.port[0].gun_x,
                            in.port[0].gun_y,
                            in.port[0].flags * 2u + (in.port[0].buttons != 0u ? 1u : 0u),
                            in.port[1].gun_x, in.port[1].gun_y,
                            in.port[1].flags * 2u + (in.port[1].buttons != 0u ? 1u : 0u));
                } else
                    fprintf(f, "|driving:%d,0,%u|driving:%d,0,%u|...BB|\n", in.port[0].dx,
                            in.port[0].buttons != 0u ? 1u : 0u, in.port[1].dx,
                            in.port[1].buttons != 0u ? 1u : 0u);
            }
            failed |= ferror(f) != 0;
            failed |= fclose(f) != 0;
        }
    }
    muhtari_destroy(m);
    free(image);
    return failed;
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
    size_t firmware_n = 0;
    uint8_t *firmware = battery_firmware != NULL ? read_file(battery_firmware, &firmware_n) : NULL;
    media.hsc_rom = firmware;
    media.hsc_rom_size = firmware_n;
    uint32_t board = strcmp(rom, "battery") == 0      ? MUHTARI_BOARD_LINEAR
                     : strcmp(rom, "activision") == 0 ? MUHTARI_BOARD_ACTIVISION
                     : strcmp(rom, "fractalus") == 0  ? MUHTARI_BOARD_FRACTALUS
                                                      : MUHTARI_BOARD_ABSOLUTE;
    if (muhtari_probe(image, n, &info) != MUHTARI_STATUS_OK || info.board != board ||
        muhtari_create(&cfg, &m) != MUHTARI_STATUS_OK ||
        muhtari_load(m, &media) != MUHTARI_STATUS_OK) {
        free(firmware);
        muhtari_destroy(m);
        return 1;
    }
    free(firmware);
    muhtari_battery_info battery = {0};
    battery.size = (uint32_t)sizeof battery;
    if (strcmp(rom, "battery") == 0 && (muhtari_get_battery(m, &battery) != MUHTARI_STATUS_OK ||
                                        battery_restore(battery.data, battery.bytes))) {
        muhtari_destroy(m);
        return 1;
    }
    int failed = run(m, 0) < 0;
    uint8_t mb[256];
    for (uint32_t i = 0; i < 256u; i++) {
        mb[i] = muhtari__peek(m, (uint16_t)(MB + i));
    }
    failed |= retail_judge(mb, rom);
    if (strcmp(rom, "battery") == 0)
        failed |= battery_observe(battery.data, battery.bytes);
    if (!failed && battery_bmp != NULL)
        failed |= battery_picture(m);
    muhtari_destroy(m);
    return failed;
}

/* The tracer also protects the existing load transaction. Refusals cannot
   allocate or change any live state, ROM bytes or subsequent execution. */
static int retail_transaction(uint8_t *image)
{
    muhtari_versions versions = {0};
    versions.size = (uint32_t)sizeof versions;
    if (muhtari_get_versions(&versions) != MUHTARI_STATUS_OK || versions.abi != 1u ||
        versions.behaviour_revision != 6u) {
        return 1;
    }
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
        failed |=
            muhtari__cart_read(m, 0x4000u, 0) != 90u || muhtari__cart_read(m, 0x4000u, 1) != 90u;
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

/* Only this immutable owned template may seed transient geometry variants.
   Byte construction follows the documented layout; expected.csv is independent.
   Code stays at the same upper CPU addresses in every declared geometry. */
static int bankset_image(const char *template_path, const char *path, unsigned kib, unsigned mode,
                         unsigned pal, unsigned fault)
{
    size_t n = 0u;
    uint8_t *template = read_file(template_path, &n);
    uint8_t digest[32];
    char hex[65];
    muhtari__sha256 hash;
    muhtari__sha256_init(&hash);
    if (template == NULL)
        return 1;
    muhtari__sha256_update(&hash, template, n);
    muhtari__sha256_final(&hash, digest);
    muhtari__sha256_hex(digest, hex);
    if (n != 65664u ||
        strcmp(hex, "32f5870fbfcc4745967b8b94c69f87dac4879587c4b0561256cc5160d61a3390") != 0 ||
        memcmp(template + 128u + 0x76F0u, "BANKSET1", 8u) != 0 ||
        (kib != 32u && kib != 48u && kib != 52u && kib != 128u && kib != 256u && kib != 512u) ||
        mode > 2u || pal > 1u || fault > 3u) {
        free(template);
        return 1;
    }
    size_t half = (size_t)kib * 1024u;
    size_t size = 128u + 2u * half;
    uint8_t *image = calloc(size, 1u);
    if (image == NULL) {
        free(template);
        return 1;
    }
    memcpy(image, template, 128u);
    image[49] = (uint8_t)((size - 128u) >> 24);
    image[50] = (uint8_t)((size - 128u) >> 16);
    image[51] = (uint8_t)((size - 128u) >> 8);
    image[52] = (uint8_t)(size - 128u);
    image[57] = (uint8_t)pal;
    image[64] = kib < 128u ? 0u : 1u;
    image[65] = (uint8_t)(0x80u | (mode == 2u ? 3u : mode));
    image[67] = mode != 0u ? 5u : 0u; /* Write-only POKEY overlap. */
    for (unsigned set = 0u; set < 2u; set++) {
        uint8_t *body = image + 128u + set * half;
        size_t extra = kib == 52u ? 4096u : 0u;
        memset(body, set == 0u ? 0x30 : 0x90, extra);
        for (size_t bank = 0u; bank < (half - extra) / 16384u; bank++) {
            uint8_t *p = body + extra + bank * 16384u;
            memset(p, (int)((set == 0u ? 0x40u : 0xA0u) + bank), 16384u);
            p[16383u] = (uint8_t)((set == 0u ? 0x80u : 0xB0u) + bank);
        }
        memcpy(body + half - 16384u, template + 128u + set * 32768u + 16384u, 16384u);
    }
    uint8_t *upper = image + 128u + half - 16384u;
    upper[0x3700u] = mode != 0u || kib >= 128u ? 0x40u
                     : kib == 52u              ? 0x30u
                     : kib == 48u              ? 0x40u
                                               : 0x80u;
    upper[0x3701u] = mode != 0u || kib >= 128u || kib == 48u ? 0x7Fu : kib == 52u ? 0x3Fu : 0xBFu;
    upper[0x3702u] = (uint8_t)(kib >= 128u ? kib / 16u : 0u);
    upper[0x3703u] = (uint8_t)mode;
    if (fault == 1u)
        image[128u + 2u * half - 8192u] = 0x40u; /* CPU graphics in MARIA ROM. */
    if (fault == 2u)
        image[51] ^= 1u; /* Header/body mismatch. */
    if (fault == 3u)
        size--; /* Odd/truncated body. */
    if (fault == 0u && (mode == 0u || (kib != 48u && kib != 52u))) {
        muhtari_cart_info info = {0};
        info.size = (uint32_t)sizeof info;
        if (muhtari_probe(image, size, &info) != MUHTARI_STATUS_OK ||
            info.board != MUHTARI_BOARD_BANKSET || info.rom_size != 2u * half ||
            info.ram_size != (mode == 0u   ? 0u
                              : mode == 1u ? 16384u
                                           : 32768u) ||
            info.a78_mapper_options != (0x80u | (mode == 2u ? 3u : mode)) ||
            info.a78_mapper != (kib < 128u ? 0u : 1u) || info.a78_tv != pal ||
            memcmp(upper, template + 128u + 16384u, 0x3700u) != 0 ||
            memcmp(upper + 0x3704u, template + 128u + 16384u + 0x3704u, 0x8FCu) != 0) {
            free(image);
            free(template);
            return 1;
        }
    }
    FILE *f = fopen(path, "wb");
    int failed = f == NULL;
    if (f != NULL) {
        failed = fwrite(image, 1u, size, f) != size;
        failed |= fclose(f) != 0;
    }
    free(image);
    free(template);
    return failed;
}

/* The existing independent PNG reader verifies the actual runner dump. */
static int bankset_png(const char *path, unsigned mode, unsigned pal, const char *filter_fault)
{
    size_t n = 0u;
    uint8_t *image = read_file(path, &n);
    uint32_t height = pal != 0u ? 292u : 242u;
    uint8_t *raw = malloc((size_t)height * PNG_READ_ROW);
    int failed = image == NULL || raw == NULL || png_read_height(image, n, raw, height) < 0;
    static const unsigned positions[] = {0u, 1u, 2u, 16u, 20u};
    if (!failed) {
        for (unsigned y = 0u; y < height; y++) {
            if (raw[y * PNG_READ_ROW] != 0u) {
                fprintf(stderr, "bankset: PNG row filter is not zero\n");
                failed = 1;
            }
        }
    }
    if (!failed) {
        for (unsigned i = 0u; i < 5u; i++) {
            uint8_t code = i < 2u    ? (mode == 1u ? 0x16u : 0x38u)
                           : i == 2u ? 0u
                           : i == 3u ? (mode == 1u ? 0x27u : 0x38u)
                                     : (mode == 1u ? 0u : 0x27u);
            uint32_t rgb = muhtari__palette_rgb_region(code, pal != 0u ? MUHTARI_REGION_PAL
                                                                       : MUHTARI_REGION_NTSC);
            const uint8_t *p = raw + 17u * PNG_READ_ROW + 1u + positions[i] * 3u;
            failed |=
                p[0] != (uint8_t)(rgb >> 16) || p[1] != (uint8_t)(rgb >> 8) || p[2] != (uint8_t)rgb;
        }
        /* Same bytes with a different supported or forbidden height refuse. */
        failed |= png_read_height(image, n, raw, height == 242u ? 292u : 242u) >= 0;
        failed |= png_read_height(image, n, raw, 243u) >= 0;
        image[n - 1u] ^= 1u;
        failed |= png_read_height(image, n, raw, height) >= 0;
        image[n - 1u] ^= 1u;
    }
    if (!failed && filter_fault != NULL) {
        /* A valid IDAT CRC and Adler-32 must not hide a changed row filter.
           Row17 lies in the first stored block of the actual runner PNG. */
        uint32_t first = image[44] | ((uint32_t)image[45] << 8);
        uint32_t zn = ((uint32_t)image[33] << 24) | ((uint32_t)image[34] << 16) |
                      ((uint32_t)image[35] << 8) | image[36];
        if (first <= 17u * PNG_READ_ROW) {
            failed = 1;
        } else {
            raw[17u * PNG_READ_ROW] = 1u;
            image[48u + 17u * PNG_READ_ROW] = 1u;
            uint32_t a = 1u, b = 0u;
            for (uint32_t i = 0u; i < height * PNG_READ_ROW; i++) {
                a = (a + raw[i]) % 65521u;
                b = (b + a) % 65521u;
            }
            uint32_t adler = (b << 16) | a;
            for (unsigned i = 0u; i < 4u; i++)
                image[41u + zn - 4u + i] = (uint8_t)(adler >> (24u - 8u * i));
            uint32_t crc = png_read_crc(image + 37u, zn + 4u);
            for (unsigned i = 0u; i < 4u; i++)
                image[41u + zn + i] = (uint8_t)(crc >> (24u - 8u * i));
            FILE *f = fopen(filter_fault, "wb");
            failed = f == NULL;
            if (f != NULL) {
                failed |= fwrite(image, 1u, n, f) != n;
                failed |= fclose(f) != 0;
            }
        }
    }
    free(raw);
    free(image);
    return failed;
}

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
    static const uint8_t code[] = {0x78, 0xD8, 0xA9, 0x07, 0x85, 0x01, 0xA9, 0x60, 0x85,
                                   0x3C, 0xA2, 0x00, 0xBD, 0x00, 0xC1, 0x9D, 0x00, 0x22,
                                   0xE8, 0xD0, 0xF7, 0xAD, 0x00, 0x40, 0x8D, 0x12, 0x22,
                                   0xA9, 0x00, 0x8D, 0x05, 0x22, 0x4C, 0x20, 0xC0};
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
static const struct retro_memory_map *retail_map;
static bool RETRO_CALLCONV retail_env(unsigned cmd, void *data)
{
    if (controller_adapter != 0u && cmd == RETRO_ENVIRONMENT_GET_VARIABLE) {
        struct retro_variable *v = data;
        if (strcmp(v->key, "muhtari_left_difficulty") == 0) {
            v->value = controller_group == 1u ? "A" : "B";
            return true;
        }
        if (strcmp(v->key, "muhtari_right_difficulty") == 0) {
            v->value = controller_group == 2u ? "A" : "B";
            return true;
        }
    }
    if (cmd == RETRO_ENVIRONMENT_SET_MEMORY_MAPS) {
        retail_map = (const struct retro_memory_map *)data;
        return true;
    }
    return cmd == RETRO_ENVIRONMENT_SET_PIXEL_FORMAT || cmd == RETRO_ENVIRONMENT_SET_MEMORY_MAPS ||
           cmd == RETRO_ENVIRONMENT_SET_SUPPORT_ACHIEVEMENTS ||
           (cmd == RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE && (*(bool *)data = false, true));
}
static void RETRO_CALLCONV retail_video(const void *data, unsigned w, unsigned h, size_t pitch)
{
    (void)data;
    (void)w;
    (void)h;
    (void)pitch;
}
static size_t RETRO_CALLCONV retail_audio(const int16_t *data, size_t frames)
{
    (void)data;
    return frames;
}
static void RETRO_CALLCONV retail_poll(void)
{
}
static int16_t RETRO_CALLCONV retail_input(unsigned p, unsigned d, unsigned i, unsigned id)
{
    (void)p;
    (void)d;
    (void)i;
    (void)id;
    return 0;
}

static int retail_absolute_map(const uint8_t *image, size_t n)
{
    if (retail_map == NULL || retail_map->num_descriptors > 64u || n != 65664u) {
        return 1;
    }
    for (uint32_t address = 0x4000u; address < 0x10000u; address++) {
        unsigned hits = 0u;
        for (unsigned i = 0; i < retail_map->num_descriptors; i++) {
            const struct retro_memory_descriptor *d = &retail_map->descriptors[i];
            if (address < d->start || address - d->start >= d->len) {
                continue;
            }
            hits++;
            if (d->select != 0u || d->disconnect != 0u) {
                return 1;
            }
            if (address < 0x8000u) {
                if (d->ptr != NULL || d->flags != 0u) {
                    return 1;
                }
            } else if (d->ptr == NULL || d->flags != RETRO_MEMDESC_CONST ||
                       ((const uint8_t *)d->ptr)[d->offset + address - d->start] !=
                           image[128u + address]) {
                return 1;
            }
        }
        if (hits != 1u) {
            return 1;
        }
    }
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
#define DIAG_LOAD(name, symbol_name, result, args)                                                 \
    result(RETRO_CALLCONV *name) args;                                                             \
    {                                                                                              \
        DIAG_SYMBOL(symbol_name);                                                                  \
        if (symbol == NULL) {                                                                      \
            DIAG_CLOSE();                                                                          \
            free(image);                                                                           \
            return 1;                                                                              \
        }                                                                                          \
        memcpy(&name, &symbol, sizeof name);                                                       \
    }
    DIAG_LOAD(set_environment, set_environment, void, (retro_environment_t));
    DIAG_LOAD(set_video_refresh, set_video_refresh, void, (retro_video_refresh_t));
    DIAG_LOAD(set_audio_sample_batch, set_audio_sample_batch, void, (retro_audio_sample_batch_t));
    DIAG_LOAD(set_input_poll, set_input_poll, void, (retro_input_poll_t));
    DIAG_LOAD(set_input_state, set_input_state, void, (retro_input_state_t));
    DIAG_LOAD(init, init, void, (void));
    DIAG_LOAD(deinit, deinit, void, (void));
    DIAG_LOAD(load_game, load_game, bool, (const struct retro_game_info *));
    DIAG_LOAD(unload_game, unload_game, void, (void));
    DIAG_LOAD(module_run, run, void, (void));
    DIAG_LOAD(get_memory_data, get_memory_data, void *, (unsigned));
    DIAG_LOAD(get_memory_size, get_memory_size, size_t, (unsigned));
#undef DIAG_LOAD
    set_environment(retail_env);
    set_video_refresh(retail_video);
    set_audio_sample_batch(retail_audio);
    set_input_poll(retail_poll);
    set_input_state(retail_input);
    init();
    retail_map = NULL;
    struct retro_game_info game = {path, image, n, NULL};
    int loaded = load_game(&game);
    int failed = 1;
    if (loaded) {
        if (strcmp(rom, "battery") == 0 &&
            battery_restore((uint8_t *)get_memory_data(RETRO_MEMORY_SAVE_RAM),
                            get_memory_size(RETRO_MEMORY_SAVE_RAM))) {
            unload_game();
            deinit();
            DIAG_CLOSE();
            free(image);
            return 1;
        }
        for (uint32_t frame = 0; frame < FRAME_CAP; frame++) {
            module_run();
            const uint8_t *ram = (const uint8_t *)get_memory_data(RETRO_MEMORY_SYSTEM_RAM);
            if (ram != NULL && get_memory_size(RETRO_MEMORY_SYSTEM_RAM) >= 0xB00u &&
                memcmp(ram + 0xA00u, "M7DG", 4u) == 0 && ram[0xA05] == 0u) {
                failed = retail_judge(ram + 0xA00u, rom);
                if (strcmp(rom, "battery") == 0)
                    failed |=
                        battery_observe((const uint8_t *)get_memory_data(RETRO_MEMORY_SAVE_RAM),
                                        get_memory_size(RETRO_MEMORY_SAVE_RAM));
                if (strcmp(rom, "smoke") == 0 || strcmp(rom, "absolute") == 0) {
                    failed |= retail_absolute_map(image, n);
                }
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

static unsigned controller_frame, controller_errors, controller_videos, controller_audios;
static const uint8_t *(*controller_native)(void);
static int controller_gun_edges;
static int16_t controller_gun_values[17];
static unsigned controller_gun_queries[2][17];
static int controller_paddle_edges;
static int16_t controller_paddle_values[2];
static int controller_keypad_edges;
static uint32_t controller_keypad_mask;
static int controller_mouse_edges;
static int16_t controller_mouse_values[11];
static unsigned controller_mouse_queries[2][11];

static void RETRO_CALLCONV controller_poll(void)
{
    controller_frame++;
}

static int16_t RETRO_CALLCONV controller_input(unsigned p, unsigned d, unsigned i, unsigned id)
{
    muhtari_input_ex in;
    controller_stimulus(controller_frame, &in);
    if (controller_adapter != 0u) {
        static const unsigned ids[2][12] = {
            {RETRO_DEVICE_ID_JOYPAD_B, RETRO_DEVICE_ID_JOYPAD_Y, RETRO_DEVICE_ID_JOYPAD_SELECT,
             RETRO_DEVICE_ID_JOYPAD_START, RETRO_DEVICE_ID_JOYPAD_UP, RETRO_DEVICE_ID_JOYPAD_DOWN,
             RETRO_DEVICE_ID_JOYPAD_LEFT, RETRO_DEVICE_ID_JOYPAD_RIGHT, RETRO_DEVICE_ID_JOYPAD_A,
             RETRO_DEVICE_ID_JOYPAD_X, RETRO_DEVICE_ID_JOYPAD_L, RETRO_DEVICE_ID_JOYPAD_R},
            {RETRO_DEVICE_ID_JOYPAD_A, RETRO_DEVICE_ID_JOYPAD_B, RETRO_DEVICE_ID_JOYPAD_START,
             RETRO_DEVICE_ID_JOYPAD_Y, RETRO_DEVICE_ID_JOYPAD_X, RETRO_DEVICE_ID_JOYPAD_R,
             RETRO_DEVICE_ID_JOYPAD_SELECT, RETRO_DEVICE_ID_JOYPAD_L, RETRO_DEVICE_ID_JOYPAD_UP,
             RETRO_DEVICE_ID_JOYPAD_DOWN, RETRO_DEVICE_ID_JOYPAD_LEFT,
             RETRO_DEVICE_ID_JOYPAD_RIGHT}};
        static const uint32_t masks[2][12] = {
            {16u, 64u, 1024u, 2048u, 1u, 2u, 4u, 8u, 32u, 128u, 256u, 512u},
            {64u, 16u, 2048u, 32u, 256u, 512u, 1024u, 128u, 1u, 2u, 4u, 8u}};
        if (p >= 2u || d != RETRO_DEVICE_JOYPAD || i != 0u) {
            controller_errors++;
            return 0;
        }
        for (unsigned k = 0u; k < 12u; k++)
            if (id == ids[controller_adapter - 1u][k])
                return (in.port[p].buttons & masks[controller_adapter - 1u][k]) != 0u;
        return p == 0u &&
               ((id == RETRO_DEVICE_ID_JOYPAD_L2 && (in.switches & MUHTARI_SWITCH_PAUSE)) ||
                (id == RETRO_DEVICE_ID_JOYPAD_L3 && (in.switches & MUHTARI_SWITCH_RESET)));
    }
    if (controller_keypad) {
        static const unsigned ids[12] = {RETRO_DEVICE_ID_JOYPAD_Y, RETRO_DEVICE_ID_JOYPAD_UP,
                                         RETRO_DEVICE_ID_JOYPAD_X, RETRO_DEVICE_ID_JOYPAD_LEFT,
                                         RETRO_DEVICE_ID_JOYPAD_B, RETRO_DEVICE_ID_JOYPAD_RIGHT,
                                         RETRO_DEVICE_ID_JOYPAD_L, RETRO_DEVICE_ID_JOYPAD_DOWN,
                                         RETRO_DEVICE_ID_JOYPAD_R, RETRO_DEVICE_ID_JOYPAD_SELECT,
                                         RETRO_DEVICE_ID_JOYPAD_A, RETRO_DEVICE_ID_JOYPAD_START};
        if (p >= 2u || d != RETRO_DEVICE_JOYPAD || i != 0u) {
            controller_errors++;
            return 0;
        }
        uint32_t mask = controller_keypad_edges ? controller_keypad_mask : in.port[p].keypad;
        for (unsigned key = 0u; key < 12u; key++)
            if (id == ids[key])
                return (mask & (1u << key)) != 0u;
        return id == RETRO_DEVICE_ID_JOYPAD_L3 && in.switches != 0u && !controller_keypad_edges;
    }
    if (controller_analog) {
        if (p >= 2u) {
            controller_errors++;
            return 0;
        }
        if (d == RETRO_DEVICE_ANALOG && i <= RETRO_DEVICE_INDEX_ANALOG_RIGHT &&
            id == RETRO_DEVICE_ID_ANALOG_X) {
            if (controller_paddle_edges)
                return controller_paddle_values[i];
            return (int16_t)((int32_t)in.port[p].paddle[i] - 32768);
        }
        if (d != RETRO_DEVICE_JOYPAD || i != 0u) {
            controller_errors++;
            return 0;
        }
        if (id == RETRO_DEVICE_ID_JOYPAD_B)
            return (in.port[p].buttons & MUHTARI_BUTTON_1) != 0u;
        if (id == RETRO_DEVICE_ID_JOYPAD_A)
            return (in.port[p].buttons & MUHTARI_BUTTON_2) != 0u;
        return 0;
    }
    if (controller_light) {
        if (p >= 2u || d != RETRO_DEVICE_LIGHTGUN || i != 0u || id >= 17u) {
            controller_errors++;
            return 0;
        }
        controller_gun_queries[p][id]++;
        if (id == RETRO_DEVICE_ID_LIGHTGUN_X || id == RETRO_DEVICE_ID_LIGHTGUN_Y ||
            id == RETRO_DEVICE_ID_LIGHTGUN_PAUSE)
            controller_errors++;
        if (controller_gun_edges)
            return controller_gun_values[id];
        if (id == RETRO_DEVICE_ID_LIGHTGUN_SCREEN_X)
            return in.port[p].gun_x == 0u ? -32767 : in.port[p].gun_x == 319u ? 32767 : 0;
        if (id == RETRO_DEVICE_ID_LIGHTGUN_SCREEN_Y)
            return in.port[p].gun_y == 0u ? -32767 : in.port[p].gun_y == 241u ? 32767 : 0;
        if (id == RETRO_DEVICE_ID_LIGHTGUN_TRIGGER)
            return in.port[p].buttons != 0u ? 1 : 0;
        return id == RETRO_DEVICE_ID_LIGHTGUN_IS_OFFSCREEN && in.port[p].flags != 0u ? 1 : 0;
    }
    if (p >= 2u || d != RETRO_DEVICE_MOUSE || i != 0u || id >= 11u) {
        controller_errors++;
        return 0;
    }
    controller_mouse_queries[p][id]++;
    if (controller_mouse_edges)
        return controller_mouse_values[id];
    if (id == RETRO_DEVICE_ID_MOUSE_X)
        return (int16_t)in.port[p].dx;
    if (id == RETRO_DEVICE_ID_MOUSE_Y)
        return (int16_t)in.port[p].dy;
    if (id == RETRO_DEVICE_ID_MOUSE_LEFT)
        return (in.port[p].buttons & MUHTARI_BUTTON_1) != 0u;
    if (controller_motion != 0u && id == RETRO_DEVICE_ID_MOUSE_RIGHT)
        return (in.port[p].buttons & MUHTARI_BUTTON_2) != 0u;
    if (controller_motion != 0u && p == 0u) {
        if (id == RETRO_DEVICE_ID_MOUSE_BUTTON_4)
            return (in.switches & MUHTARI_SWITCH_SELECT) != 0u;
        if (id == RETRO_DEVICE_ID_MOUSE_BUTTON_5)
            return (in.switches & MUHTARI_SWITCH_RESET) != 0u;
    }
    return 0;
}

static void RETRO_CALLCONV controller_video(const void *data, unsigned w, unsigned h, size_t pitch)
{
    controller_videos++;
    if (controller_frame == 0u || controller_frame > CONTROLLER_FRAMES || data == NULL ||
        w != 320u || h != CONTROLLER_HEIGHT || pitch != 320u * sizeof(uint32_t)) {
        controller_errors++;
        return;
    }
    const uint8_t *expected = controller_pixels + (controller_frame - 1u) * CONTROLLER_PIXELS;
    const uint8_t *native_bytes = controller_native();
    if (native_bytes == NULL) {
        controller_errors++;
        return;
    }
    if (bankset_tracer) {
        /* Capture now; compare hosts only after this host's numeric judgment. */
        memcpy(bankset_module_pixels + (controller_frame - 1u) * CONTROLLER_PIXELS, native_bytes,
               CONTROLLER_PIXELS);
        expected = native_bytes;
    } else if (memcmp(native_bytes, expected, CONTROLLER_PIXELS) != 0)
        controller_errors++;
    /* Public pixels are judged separately from the actual native bytes. */
    for (unsigned y = 0u; y < CONTROLLER_HEIGHT; y++)
        for (unsigned x = 0u; x < 320u; x++) {
            uint32_t pixel;
            memcpy(&pixel, (const uint8_t *)data + y * pitch + x * sizeof pixel, sizeof pixel);
            if (pixel !=
                muhtari__palette_rgb_region(expected[y * 320u + x],
                                            bankset_pal ? MUHTARI_REGION_PAL : MUHTARI_REGION_NTSC))
                controller_errors++;
        }
}

static size_t RETRO_CALLCONV controller_audio(const int16_t *data, size_t frames)
{
    controller_audios++;
    if (controller_frame == 0u || controller_frame > CONTROLLER_FRAMES || frames > 963u ||
        (!bankset_tracer && frames != controller_counts[controller_frame - 1u])) {
        controller_errors++;
        return frames;
    }
    for (size_t i = 0u; i < frames; i++) {
        if (bankset_tracer) {
            bankset_module_counts[controller_frame - 1u] = (uint32_t)frames;
            bankset_module_pcm[controller_frame - 1u][i] = data[2u * i];
            if (data[2u * i] != data[2u * i + 1u])
                controller_errors++;
            continue;
        }
        int16_t expected = controller_pcm[controller_frame - 1u][i];
        if (data[2u * i] != expected || data[2u * i + 1u] != expected)
            controller_errors++;
    }
    return frames;
}

static int controller_module(const char *module_path, const char *path)
{
    size_t n = 0u;
    uint8_t *image = read_file(path, &n);
#ifdef _WIN32
    HMODULE module = LoadLibraryA(module_path);
#define CONTROLLER_SYMBOL(key) FARPROC symbol = GetProcAddress(module, key)
#define CONTROLLER_CLOSE() FreeLibrary(module)
#else
    void *module = dlopen(module_path, RTLD_NOW);
#define CONTROLLER_SYMBOL(key) void *symbol = dlsym(module, key)
#define CONTROLLER_CLOSE() ((void)dlclose(module))
#endif
    if (module == NULL || image == NULL) {
        if (module != NULL)
            CONTROLLER_CLOSE();
        free(image);
        return 1;
    }
#define CONTROLLER_LOAD(name, key, result, args)                                                   \
    result(RETRO_CALLCONV *name) args;                                                             \
    {                                                                                              \
        CONTROLLER_SYMBOL(key);                                                                    \
        if (symbol == NULL) {                                                                      \
            CONTROLLER_CLOSE();                                                                    \
            free(image);                                                                           \
            return 1;                                                                              \
        }                                                                                          \
        memcpy(&name, &symbol, sizeof name);                                                       \
    }
    CONTROLLER_LOAD(set_environment, "retro_set_environment", void, (retro_environment_t));
    CONTROLLER_LOAD(set_video, "retro_set_video_refresh", void, (retro_video_refresh_t));
    CONTROLLER_LOAD(set_audio, "retro_set_audio_sample_batch", void, (retro_audio_sample_batch_t));
    CONTROLLER_LOAD(set_poll, "retro_set_input_poll", void, (retro_input_poll_t));
    CONTROLLER_LOAD(set_input, "retro_set_input_state", void, (retro_input_state_t));
    CONTROLLER_LOAD(set_device, "retro_set_controller_port_device", void, (unsigned, unsigned));
    CONTROLLER_LOAD(init, "retro_init", void, (void));
    CONTROLLER_LOAD(deinit, "retro_deinit", void, (void));
    CONTROLLER_LOAD(load, "retro_load_game", bool, (const struct retro_game_info *));
    CONTROLLER_LOAD(unload, "retro_unload_game", void, (void));
    CONTROLLER_LOAD(run_frame, "retro_run", void, (void));
    CONTROLLER_LOAD(instance, "muhtari__libretro_instance", muhtari *, (void));
    CONTROLLER_LOAD(observe, "muhtari__libretro_observe", void, (muhtari__audio_observer, void *));
    CONTROLLER_LOAD(native_bytes, "muhtari__libretro_native", const uint8_t *, (void));
#undef CONTROLLER_LOAD
    controller_native = native_bytes;
    controller_frame = controller_errors = controller_videos = controller_audios = 0u;
    muhtari__sha256 raw;
    muhtari__sha256_init(&raw);
    set_environment(retail_env);
    set_video(controller_video);
    set_audio(controller_audio);
    set_poll(controller_poll);
    set_input(controller_input);
    init();
    observe(controller_raw_sample, &raw);
    unsigned selected = controller_adapter != 0u
                            ? RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_JOYPAD, controller_adapter + 4u)
                        : controller_motion != 0u
                            ? RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_MOUSE, controller_motion)
                        : controller_keypad ? RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_JOYPAD, 4)
                        : controller_analog ? RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_ANALOG, 0)
                        : controller_light  ? RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_LIGHTGUN, 0)
                                            : RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_MOUSE, 0);
    controller_gun_edges = 0;
    controller_paddle_edges = 0;
    controller_keypad_edges = 0;
    controller_mouse_edges = 0;
    set_device(0u, selected);
    set_device(1u, selected);
    struct retro_game_info game = {path, image, n, NULL};
    int failed = !load(&game);
    if (bankset_refusal) {
        int accepted = !failed;
        if (accepted)
            unload();
        deinit();
        CONTROLLER_CLOSE();
        free(image);
        return accepted;
    }
    if (!failed) {
        for (unsigned i = 0u; i < CONTROLLER_FRAMES; i++)
            run_frame();
        muhtari *m = instance();
        uint8_t mb[256];
        if (m == NULL)
            failed = 1;
        else {
            for (unsigned i = 0u; i < 256u; i++)
                mb[i] = muhtari__peek(m, (uint16_t)(MB + i));
            failed = retail_judge(mb, controller_name());
        }
        if (!failed && bankset_tracer) {
            failed |= bankset_native_judge(bankset_module_pixels);
            failed |= memcmp(bankset_module_pixels, controller_pixels,
                             CONTROLLER_FRAMES * CONTROLLER_PIXELS) != 0;
            for (unsigned i = 0u; i < CONTROLLER_FRAMES; i++) {
                failed |= bankset_module_counts[i] != controller_counts[i];
                failed |= memcmp(bankset_module_pcm[i], controller_pcm[i],
                                 controller_counts[i] * sizeof(int16_t)) != 0;
            }
        }
        /* Raw transition stream is observed, including load-time events;
           silence means the actual empty stream, never an inferred pass. */
        uint8_t a[32], b[32];
        muhtari__sha256 expected_raw = controller_raw;
        muhtari__sha256_final(&raw, a);
        muhtari__sha256_final(&expected_raw, b);
        failed |= memcmp(a, b, sizeof a) != 0 || controller_errors != 0u ||
                  controller_videos != CONTROLLER_FRAMES || controller_audios != CONTROLLER_FRAMES;
        if (!failed && controller_motion != 0u) {
            set_video(NULL);
            set_audio(NULL);
            controller_mouse_edges = 1;
            for (unsigned v = 0u; v < 12u; v++) {
                memset(controller_mouse_values, 0, sizeof controller_mouse_values);
                controller_mouse_values[RETRO_DEVICE_ID_MOUSE_X] = v == 0u   ? INT16_MIN
                                                                   : v == 1u ? INT16_MAX
                                                                             : -1;
                controller_mouse_values[RETRO_DEVICE_ID_MOUSE_Y] = v == 0u   ? INT16_MAX
                                                                   : v == 1u ? INT16_MIN
                                                                             : 1;
                if (v >= 2u)
                    controller_mouse_values[v - 2u] = 1;
                memset(controller_mouse_queries, 0, sizeof controller_mouse_queries);
                unsigned before = controller_frame;
                run_frame();
                failed |= controller_frame != before + 1u;
                bool left = controller_mouse_values[RETRO_DEVICE_ID_MOUSE_LEFT] != 0;
                bool right = controller_mouse_values[RETRO_DEVICE_ID_MOUSE_RIGHT] != 0;
                uint32_t buttons = controller_motion == 1u ? (left || right ? MUHTARI_BUTTON_1 : 0u)
                                                           : (left ? MUHTARI_BUTTON_1 : 0u) |
                                                                 (right ? MUHTARI_BUTTON_2 : 0u);
                for (unsigned p = 0u; p < 2u; p++) {
                    const muhtari_port_input *in = &m->ports.applied_ex.port[p];
                    failed |= in->device != controller_motion_device() ||
                              in->dx != controller_mouse_values[RETRO_DEVICE_ID_MOUSE_X] ||
                              in->dy != controller_mouse_values[RETRO_DEVICE_ID_MOUSE_Y] ||
                              in->buttons != buttons;
                    for (unsigned id = 0u; id < 11u; id++) {
                        bool used =
                            id == RETRO_DEVICE_ID_MOUSE_X || id == RETRO_DEVICE_ID_MOUSE_Y ||
                            id == RETRO_DEVICE_ID_MOUSE_LEFT || id == RETRO_DEVICE_ID_MOUSE_RIGHT ||
                            (p == 0u && (id == RETRO_DEVICE_ID_MOUSE_MIDDLE ||
                                         id == RETRO_DEVICE_ID_MOUSE_BUTTON_4 ||
                                         id == RETRO_DEVICE_ID_MOUSE_BUTTON_5));
                        failed |= controller_mouse_queries[p][id] != (used ? 1u : 0u);
                    }
                }
                uint32_t switches =
                    (controller_mouse_values[RETRO_DEVICE_ID_MOUSE_MIDDLE] ? MUHTARI_SWITCH_PAUSE
                                                                           : 0u) |
                    (controller_mouse_values[RETRO_DEVICE_ID_MOUSE_BUTTON_4] ? MUHTARI_SWITCH_SELECT
                                                                             : 0u) |
                    (controller_mouse_values[RETRO_DEVICE_ID_MOUSE_BUTTON_5] ? MUHTARI_SWITCH_RESET
                                                                             : 0u);
                failed |= m->ports.switches != switches;
            }
            set_input(NULL);
            run_frame();
            for (unsigned p = 0u; p < 2u; p++)
                failed |= m->ports.applied_ex.port[p].dx != 0 ||
                          m->ports.applied_ex.port[p].dy != 0 ||
                          m->ports.applied_ex.port[p].buttons != 0u;
            failed |= m->ports.switches != 0u;
            controller_mouse_edges = 0;
        }
        if (!failed && controller_keypad) {
            controller_keypad_edges = 1;
            set_video(NULL);
            set_audio(NULL);
            for (unsigned v = 0u; v < 14u; v++) {
                controller_keypad_mask = v < 12u ? 1u << v : v == 12u ? 15u : 4095u;
                run_frame();
                for (unsigned p = 0u; p < 2u; p++) {
                    const muhtari_port_input *in = &m->ports.applied_ex.port[p];
                    failed |= in->device != MUHTARI_PORT_KEYPAD ||
                              in->keypad != controller_keypad_mask || in->buttons != 0u;
                }
                failed |= m->ports.switches != 0u;
            }
            set_input(NULL);
            run_frame();
            failed |= m->ports.applied_ex.port[0].keypad != 0u ||
                      m->ports.applied_ex.port[1].keypad != 0u;
            controller_keypad_edges = 0;
        }
        if (!failed && controller_analog) {
            static const struct {
                int16_t left, right;
                uint32_t pot0, pot1;
            } vectors[] = {{INT16_MIN, INT16_MAX, 0u, 65535u},
                           {-1, 0, 32767u, 32768u},
                           {0, -1, 32768u, 32767u},
                           {INT16_MAX, INT16_MIN, 65535u, 0u}};
            controller_paddle_edges = 1;
            set_video(NULL);
            set_audio(NULL);
            for (unsigned v = 0u; v < sizeof vectors / sizeof vectors[0]; v++) {
                controller_paddle_values[0] = vectors[v].left;
                controller_paddle_values[1] = vectors[v].right;
                run_frame();
                for (unsigned p = 0u; p < 2u; p++) {
                    const muhtari_port_input *in = &m->ports.applied_ex.port[p];
                    failed |= in->device != MUHTARI_PORT_PADDLE ||
                              in->paddle[0] != vectors[v].pot0 ||
                              in->paddle[1] != vectors[v].pot1 || in->buttons != 0u || in->dy != 0;
                }
            }
            set_input(NULL);
            run_frame();
            for (unsigned p = 0u; p < 2u; p++) {
                const muhtari_port_input *in = &m->ports.applied_ex.port[p];
                failed |= in->paddle[0] != 0u || in->paddle[1] != 0u || in->buttons != 0u;
            }
            controller_paddle_edges = 0;
            failed |= controller_errors != 0u;
        }
        if (!failed && controller_light) {
            /* The actual loaded module's accepted packets, independent
               numeric endpoints and center; no conversion helper oracle. */
            static const struct {
                int16_t x, y, off, reload, trigger;
                uint32_t px, py, flags, buttons;
            } vectors[] = {
                {-32767, -32767, 0, 0, 0, 0u, 0u, 0u, 0u},
                {0, 0, 0, 0, 1, 159u, 120u, 0u, MUHTARI_BUTTON_1},
                {32767, 32767, 0, 0, 0, 319u, 241u, 0u, 0u},
                {-32768, 0, 0, 0, 1, 0u, 0u, MUHTARI_INPUT_GUN_OFFSCREEN, MUHTARI_BUTTON_1},
                {0, -32768, 0, 0, 0, 0u, 0u, MUHTARI_INPUT_GUN_OFFSCREEN, 0u},
                {0, 0, 1, 0, 0, 159u, 120u, MUHTARI_INPUT_GUN_OFFSCREEN, 0u},
                {0, 0, 0, 1, 0, 159u, 120u, MUHTARI_INPUT_GUN_OFFSCREEN, MUHTARI_BUTTON_1},
            };
            static const unsigned ids[] = {
                RETRO_DEVICE_ID_LIGHTGUN_SCREEN_X, RETRO_DEVICE_ID_LIGHTGUN_SCREEN_Y,
                RETRO_DEVICE_ID_LIGHTGUN_IS_OFFSCREEN, RETRO_DEVICE_ID_LIGHTGUN_RELOAD,
                RETRO_DEVICE_ID_LIGHTGUN_TRIGGER};
            controller_gun_edges = 1;
            set_video(NULL);
            set_audio(NULL);
            for (unsigned v = 0u; v < sizeof vectors / sizeof vectors[0]; v++) {
                controller_gun_values[ids[0]] = vectors[v].x;
                controller_gun_values[ids[1]] = vectors[v].y;
                controller_gun_values[ids[2]] = vectors[v].off;
                controller_gun_values[ids[3]] = vectors[v].reload;
                controller_gun_values[ids[4]] = vectors[v].trigger;
                memset(controller_gun_queries, 0, sizeof controller_gun_queries);
                run_frame();
                for (unsigned p = 0u; p < 2u; p++) {
                    const muhtari_port_input *applied = &m->ports.applied_ex.port[p];
                    failed |= applied->device != MUHTARI_PORT_LIGHTGUN ||
                              applied->gun_x != vectors[v].px || applied->gun_y != vectors[v].py ||
                              applied->flags != vectors[v].flags ||
                              applied->buttons != vectors[v].buttons;
                    for (unsigned id = 0u; id < sizeof ids / sizeof ids[0]; id++)
                        failed |= controller_gun_queries[p][ids[id]] != 1u;
                }
            }
            failed |= controller_errors != 0u;
            controller_gun_edges = 0;
        }
        unload();
    }
    deinit();
    CONTROLLER_CLOSE();
#undef CONTROLLER_SYMBOL
#undef CONTROLLER_CLOSE
    free(image);
    if (failed)
        fprintf(stderr, "controller module: %u video, %u audio, %u mismatches\n", controller_videos,
                controller_audios, controller_errors);
    return failed;
}
#endif

/* Sustained PAL is a separate interval from the fast handoff. The literals
   below are independently derived in docs/emulation-tests.md, PAL diagnostic;
   no NTSC diagnostic buffer or frame cap is widened. */
#define PAL_CALLS 601u
#define PAL_PIXELS (320u * 292u)
struct pal_trace {
    muhtari__sha256 hash;
    muhtari *machine;
    uint64_t tick, tia_tick[2], count[10], pokey_tick;
    uint8_t source, raw[2];
    uint8_t pokey_raw;
    unsigned errors, pokey_periods;
};
static uint8_t pal_pixels[PAL_PIXELS];
static int16_t pal_pcm[962];
static uint32_t pal_count;
#ifdef DIAG_LIBRETRO
static const uint8_t *(*pal_native)(void);
static unsigned pal_format, pal_videos, pal_audios, pal_errors;
#endif

static void pal_raw(void *user, uint64_t tick, uint8_t source, uint16_t raw)
{
    struct pal_trace *t = user;
    controller_raw_sample(&t->hash, tick, source, raw);
    if (source >= 10u || raw > 15u || tick < t->tick || (tick == t->tick && source < t->source)) {
        t->errors++;
        return;
    }
    t->count[source]++;
    /* AUDC4/AUDF0 changes on TIA's B commits, independent of PAL lines.
       Initial volume writes are excluded by the separately checked startup. */
    if (source < 2u && tick >= 281976u) {
        uint64_t phase = tick % 456u;
        if (phase != 64u && phase != 288u)
            t->errors++;
        if (t->tia_tick[source] != 0u &&
            (tick - t->tia_tick[source] != 224u && tick - t->tia_tick[source] != 232u))
            t->errors++;
        if (t->tia_tick[source] != 0u && raw == t->raw[source])
            t->errors++;
        if (raw != 0u && raw != 15u)
            t->errors++;
        t->tia_tick[source] = tick;
        t->raw[source] = (uint8_t)raw;
    }
    /* Once the owned ROM has finished its CRC it runs only fast JMP
       cycles. AUDF100 at the 28-phi2 prescaler toggles every11312 ticks.
       The first done observation may straddle earlier six-tick IO. */
    if (source == 2u && t->machine != NULL && muhtari__peek(t->machine, MB_STATUS) == 0u) {
        if (t->pokey_tick != 0u) {
            if (tick - t->pokey_tick != 11312u || raw == t->pokey_raw || (raw != 0u && raw != 15u))
                t->errors++;
            t->pokey_periods++;
        }
        t->pokey_tick = tick;
        t->pokey_raw = (uint8_t)raw;
    }
    t->tick = tick;
    t->source = source;
}

static void pal_pcm_hash(muhtari__sha256 *h, uint32_t count)
{
    for (uint32_t i = 0u; i < count; i++) {
        uint16_t bits = (uint16_t)pal_pcm[i];
        uint8_t bytes[2] = {(uint8_t)bits, (uint8_t)(bits >> 8)};
        muhtari__sha256_update(h, bytes, sizeof bytes);
    }
}

static void pal_mailbox_line(FILE *f, const uint8_t mb[256])
{
    fputs("diagnostic ", f);
    for (unsigned i = 0u; i < 256u; i++)
        fprintf(f, "%02x", mb[i]);
    fputc('\n', f);
}

/* The same bounded parser judges the real runner and malformed reports.
   It validates every native frame, not only the final two equal hashes. */
static int pal_report(const char *path, const char *expected)
{
    FILE *f = fopen(path, "rb");
    char line[526], prefix[80];
    int failed = f == NULL;
    for (unsigned frame = 1u; frame <= PAL_CALLS && !failed; frame++) {
        snprintf(prefix, sizeof prefix, "frame %u sha256 ", frame);
        size_t n = strlen(prefix);
        failed = fgets(line, sizeof line, f) == NULL || strlen(line) != n + 65u ||
                 memcmp(line, prefix, n) != 0 || line[n + 64u] != '\n' ||
                 strspn(line + n, "0123456789abcdef") != 64u;
    }
    static const char *const fields[] = {"audio_transitions_sha256 ",
                                         "audio_pcm_sha256 ",
                                         "audio_frames 601\n",
                                         "audio_samples 577879\n",
                                         "audio_rate 48000/1\n",
                                         "audio_behaviour_revision 6\n",
                                         "audio_synthesis exact-q23-16x64-dc20\n"};
    for (unsigned i = 0u; i < 7u && !failed; i++) {
        size_t n = strlen(fields[i]);
        failed = fgets(line, sizeof line, f) == NULL;
        if (!failed && i < 2u)
            failed = strlen(line) != n + 65u || memcmp(line, fields[i], n) != 0 ||
                     strspn(line + n, "0123456789abcdef") != 64u || line[n + 64u] != '\n';
        else if (!failed)
            failed = strcmp(line, fields[i]) != 0;
    }
    uint8_t mb[256] = {0};
    if (!failed) {
        failed = fgets(line, sizeof line, f) == NULL || strlen(line) != 524u ||
                 memcmp(line, "diagnostic ", 11u) != 0 || line[523] != '\n' ||
                 strspn(line + 11u, "0123456789abcdef") != 512u || fgetc(f) != EOF;
        for (unsigned i = 0u; i < 512u && !failed; i++) {
            unsigned char c = (unsigned char)line[11u + i];
            uint8_t v = (uint8_t)(c <= '9' ? c - '0' : c - 'a' + 10);
            mb[i / 2u] = (uint8_t)((i & 1u) == 0u ? v << 4 : mb[i / 2u] | v);
        }
    }
    if (f != NULL) {
        failed |= ferror(f) != 0;
        failed |= fclose(f) != 0;
    }
    if (!failed)
        failed = retail_expected(expected, "pal") != 0 || retail_judge(mb, "pal") != 0;
    if (failed)
        fprintf(stderr, "pal: incomplete or invalid 601-call numeric report\n");
    return failed;
}

static int pal_faults(const char *out, const uint8_t mb[256])
{
    for (unsigned fault = 0u; fault < 11u; fault++) {
        uint8_t bad[256];
        memcpy(bad, mb, sizeof bad);
        if (fault < 8u)
            bad[18u + fault * 4u] ^= 1u; /* CRC-valid wrong independent number. */
        else if (fault == 8u)
            bad[6]--;
        else if (fault == 9u)
            bad[4]++;
        else
            bad[8] ^= 1u;
        if (fault < 8u) {
            uint16_t crc = retail_crc(bad);
            bad[8] = (uint8_t)crc;
            bad[9] = (uint8_t)(crc >> 8);
        }
        char name[4096];
        snprintf(name, sizeof name, "%s/fault-%u.txt", out, fault);
        FILE *f = fopen(name, "wb");
        if (f == NULL)
            return 1;
        pal_mailbox_line(f, bad);
        if (fclose(f) != 0 || retail_judge(bad, "pal") == 0)
            return 1;
    }
    return 0;
}

#ifdef DIAG_LIBRETRO
static bool RETRO_CALLCONV pal_env(unsigned cmd, void *data)
{
    if (cmd == RETRO_ENVIRONMENT_SET_PIXEL_FORMAT) {
        pal_format = *(const unsigned *)data;
        return pal_format == RETRO_PIXEL_FORMAT_XRGB8888 || pal_format == RETRO_PIXEL_FORMAT_RGB565;
    }
    return retail_env(cmd, data);
}
static void RETRO_CALLCONV pal_video(const void *data, unsigned w, unsigned h, size_t pitch)
{
    pal_videos++;
    unsigned bytes = pal_format == RETRO_PIXEL_FORMAT_RGB565 ? 2u : 4u;
    const uint8_t *native = pal_native();
    if (data == NULL || native == NULL || w != 320u || h != 292u || pitch != 320u * bytes ||
        memcmp(native, pal_pixels, PAL_PIXELS) != 0) {
        pal_errors++;
        return;
    }
    for (unsigned i = 0u; i < PAL_PIXELS; i++) {
        uint32_t rgb = muhtari__palette_rgb_region(pal_pixels[i], MUHTARI_REGION_PAL);
        /* Derive channel truncation independently from the 24-bit colour. */
        uint16_t rgb565 =
            (uint16_t)(((rgb >> 19) & 31u) << 11 | ((rgb >> 10) & 63u) << 5 | ((rgb >> 3) & 31u));
        if (bytes == 2u ? ((const uint16_t *)data)[i] != rgb565
                        : ((const uint32_t *)data)[i] != rgb)
            pal_errors++;
    }
}
static size_t RETRO_CALLCONV pal_audio(const int16_t *data, size_t frames)
{
    pal_audios++;
    if (data == NULL || frames != pal_count) {
        pal_errors++;
        return frames;
    }
    for (unsigned i = 0u; i < frames; i++)
        if (data[2u * i] != pal_pcm[i] || data[2u * i + 1u] != pal_pcm[i])
            pal_errors++;
    return frames;
}
#endif

static int pal_case(const char *path, const char *expected, const char *out,
                    const char *module_path)
{
    size_t size = 0u;
    uint8_t *image = read_file(path, &size);
    muhtari_config cfg = {0};
    muhtari_media media = {0};
    muhtari *m = NULL;
    cfg.size = (uint32_t)sizeof cfg;
    cfg.allocator.alloc = diag_alloc;
    cfg.allocator.free = diag_free;
    media.size = (uint32_t)sizeof media;
    media.image = image;
    media.image_size = size;
    struct pal_trace trace = {0}, module_trace = {0};
    muhtari__sha256 pcm;
    muhtari__sha256_init(&trace.hash);
    muhtari__sha256_init(&module_trace.hash);
    muhtari__sha256_init(&pcm);
    int failed = image == NULL || retail_expected(expected, "pal") != 0 || nrows != 8u ||
                 muhtari_create(&cfg, &m) != MUHTARI_STATUS_OK;
    if (!failed) {
        trace.machine = m;
        failed = muhtari__synth_observe(m, pal_raw, &trace) != MUHTARI_STATUS_OK ||
                 muhtari_load(m, &media) != MUHTARI_STATUS_OK;
    }
    if (!failed)
        failed = m->effective_region != MUHTARI_REGION_PAL || m->bus.ticks != 140016u ||
                 m->frame_start != 140016u || m->frame_deadline != 281976u ||
                 m->synth.origin != 140016u;
    muhtari_info info = {0};
    info.size = (uint32_t)sizeof info;
    if (!failed)
        failed = muhtari_get_info(m, &info) != MUHTARI_STATUS_OK || info.width != 320u ||
                 info.height != 292u || info.max_audio_samples != 962u ||
                 info.fps_num != 3546894u || info.fps_den != 71051u;
    char name[4096];
    snprintf(name, sizeof name, "%s/reference.txt", out);
    FILE *reference = failed ? NULL : fopen(name, "wb");
    failed |= reference == NULL;
#ifdef DIAG_LIBRETRO
#ifdef _WIN32
    HMODULE module = NULL;
#define PAL_SYMBOL(key) FARPROC symbol = GetProcAddress(module, key)
#define PAL_CLOSE() FreeLibrary(module)
#else
    void *module = NULL;
#define PAL_SYMBOL(key) void *symbol = dlsym(module, key)
#define PAL_CLOSE() ((void)dlclose(module))
#endif
    void(RETRO_CALLCONV * unload)(void) = NULL;
    void(RETRO_CALLCONV * deinit)(void) = NULL;
    void(RETRO_CALLCONV * module_run)(void) = NULL;
    muhtari *(RETRO_CALLCONV * instance)(void) = NULL;
    if (module_path != NULL && !failed) {
#ifdef _WIN32
        module = LoadLibraryA(module_path);
#else
        module = dlopen(module_path, RTLD_NOW);
#endif
        if (module == NULL)
            failed = 1;
    }
#define PAL_LOAD(name_, key, result, args)                                                         \
    result(RETRO_CALLCONV *name_) args;                                                            \
    {                                                                                              \
        PAL_SYMBOL(key);                                                                           \
        if (symbol == NULL) {                                                                      \
            failed = 1;                                                                            \
            goto pal_cleanup;                                                                      \
        }                                                                                          \
        memcpy(&name_, &symbol, sizeof name_);                                                     \
    }
    if (module != NULL && !failed) {
        PAL_LOAD(set_env, "retro_set_environment", void, (retro_environment_t));
        PAL_LOAD(set_video, "retro_set_video_refresh", void, (retro_video_refresh_t));
        PAL_LOAD(set_audio, "retro_set_audio_sample_batch", void, (retro_audio_sample_batch_t));
        PAL_LOAD(set_poll, "retro_set_input_poll", void, (retro_input_poll_t));
        PAL_LOAD(set_input, "retro_set_input_state", void, (retro_input_state_t));
        PAL_LOAD(init, "retro_init", void, (void));
        PAL_LOAD(load, "retro_load_game", bool, (const struct retro_game_info *));
        PAL_LOAD(get_av, "retro_get_system_av_info", void, (struct retro_system_av_info *));
        PAL_LOAD(native_bytes, "muhtari__libretro_native", const uint8_t *, (void));
        PAL_LOAD(observe, "muhtari__libretro_observe", void, (muhtari__audio_observer, void *));
        {
            PAL_SYMBOL("retro_unload_game");
            memcpy(&unload, &symbol, sizeof unload);
        }
        {
            PAL_SYMBOL("retro_deinit");
            memcpy(&deinit, &symbol, sizeof deinit);
        }
        {
            PAL_SYMBOL("retro_run");
            memcpy(&module_run, &symbol, sizeof module_run);
        }
        {
            PAL_SYMBOL("muhtari__libretro_instance");
            memcpy(&instance, &symbol, sizeof instance);
        }
        if (unload == NULL || deinit == NULL || module_run == NULL || instance == NULL) {
            failed = 1;
            goto pal_cleanup;
        }
        pal_native = native_bytes;
        pal_videos = pal_audios = pal_errors = 0u;
        set_env(pal_env);
        set_video(pal_video);
        set_audio(pal_audio);
        set_poll(retail_poll);
        set_input(retail_input);
        init();
        observe(pal_raw, &module_trace);
        struct retro_game_info game = {path, image, size, NULL};
        failed = !load(&game);
        module_trace.machine = instance();
        struct retro_system_av_info av = {0};
        get_av(&av);
        failed |= av.geometry.base_width != 320u || av.geometry.base_height != 292u ||
                  av.timing.fps != 3546894.0 / 71051.0 || av.timing.sample_rate != 48000.0;
    }
#undef PAL_LOAD
#else
    if (module_path != NULL)
        failed = 1;
#endif
    uint64_t samples = 0u;
    uint32_t remainder = 0u, small = 0u, large = 0u;
    for (unsigned frame = 1u; frame <= PAL_CALLS && !failed; frame++) {
        uint32_t elapsed = frame == 1u ? 141960u : 142102u;
        uint64_t acc = (uint64_t)remainder + (uint64_t)elapsed * 4000u;
        uint32_t count = (uint32_t)(acc / 591149u);
        remainder = (uint32_t)(acc % 591149u);
        uint64_t start = m->frame_start;
        muhtari_frame io = {0};
        io.size = (uint32_t)sizeof io;
        io.video = pal_pixels;
        io.video_pitch = 320u;
        io.audio = pal_pcm;
        io.audio_capacity = 962u;
        failed =
            m->frame_deadline - start != elapsed || muhtari_run_frame(m, &io) != MUHTARI_STATUS_OK;
        pal_count = io.audio_count;
        failed |= io.audio_count != count || m->audio_rem != remainder ||
                  m->frame_start != start + elapsed || m->frames != frame || trace.errors != 0u ||
                  m->pokey[0].cycle != m->bus.phi2 ||
                  m->pokey[0].half != 2u * (m->bus.phi2 - m->bus.clock_origin_phi2);
        if (frame == 1u)
            failed |= io.audio_count != 960u || remainder != 336960u;
        else {
            small += io.audio_count == 961u;
            large += io.audio_count == 962u;
            unsigned nonzero = 0u;
            for (unsigned i = 0u; i < io.audio_count; i++)
                nonzero += pal_pcm[i] != 0;
            failed |= nonzero == 0u;
            /* The last requested DMA is raster308; raster309 is blank. */
            failed |= m->maria.stats[308].cost == 0u || m->maria.stats[309].cost != 0u;
            /* VBE16 loads the DLL; first DMA17 feeds raster18. */
            for (unsigned y = 2u; y < 292u; y++)
                failed |= pal_pixels[y * 320u] != 0x2Fu || pal_pixels[y * 320u + 319u] != 0x2Fu;
        }
#ifdef DIAG_LIBRETRO
        if (module != NULL && !failed) {
            module_run();
            muhtari *other = instance();
            failed |= other == NULL || pal_errors != 0u || module_trace.errors != 0u;
            if (other != NULL)
                failed |= other->frames != frame || other->frame_start != m->frame_start ||
                          other->audio_rem != m->audio_rem;
        }
#endif
        if (failed) {
            fprintf(stderr, "pal frame%u count%u rem%u trace%u dma%u/%u lower%u/%u\n", frame,
                    io.audio_count, m->audio_rem, trace.errors, m->maria.stats[308].cost,
                    m->maria.stats[309].cost, pal_pixels[291u * 320u], pal_pixels[PAL_PIXELS - 1u]);
            break;
        }
        samples += io.audio_count;
        pal_pcm_hash(&pcm, io.audio_count);
        muhtari__sha256 native;
        muhtari__sha256_init(&native);
        muhtari__sha256_update(&native, pal_pixels, PAL_PIXELS);
        char key[40];
        snprintf(key, sizeof key, "frame %u sha256", frame);
        controller_digest(reference, key, &native);
    }
    uint8_t mb[256] = {0};
    if (!failed) {
        for (unsigned i = 0u; i < 256u; i++)
            mb[i] = muhtari__peek(m, (uint16_t)(MB + i));
        failed = samples != 577879u || remainder != 47029u || small != 281u || large != 319u ||
                 m->frame_start != 85543176u || m->frame_deadline != 85685278u ||
                 trace.count[0] < 300000u || trace.count[1] != trace.count[0] ||
                 trace.count[2] < 1000u || trace.pokey_periods < 2u || retail_judge(mb, "pal") != 0;
        if (!failed)
            failed = pal_faults(out, mb);
    }
#ifdef DIAG_LIBRETRO
    if (module != NULL && !failed) {
        uint8_t a[32], b[32];
        muhtari__sha256 copy = trace.hash;
        muhtari__sha256_final(&copy, a);
        muhtari__sha256_final(&module_trace.hash, b);
        failed = memcmp(a, b, sizeof a) != 0 || pal_videos != PAL_CALLS || pal_audios != PAL_CALLS;
        muhtari *other = instance();
        for (unsigned i = 0u; i < 256u && !failed; i++)
            failed |= muhtari__peek(other, (uint16_t)(MB + i)) != mb[i];
    }
#endif
    if (!failed) {
        controller_digest(reference, "audio_transitions_sha256", &trace.hash);
        controller_digest(reference, "audio_pcm_sha256", &pcm);
        fputs("audio_frames 601\naudio_samples 577879\naudio_rate 48000/1\n"
              "audio_behaviour_revision 6\naudio_synthesis exact-q23-16x64-dc20\n",
              reference);
        pal_mailbox_line(reference, mb);
        printf("pal: startup960/rem336960 +600 full142102-tick frames; 281x961+319x962; "
               "601 calls577879/rem47029 tick85543176; TIA%llu/%llu POKEY%llu ordered\n",
               (unsigned long long)trace.count[0], (unsigned long long)trace.count[1],
               (unsigned long long)trace.count[2]);
    }
#ifdef DIAG_LIBRETRO
pal_cleanup:
    if (module != NULL) {
        if (unload != NULL)
            unload();
        if (deinit != NULL)
            deinit();
        PAL_CLOSE();
    }
#undef PAL_SYMBOL
#undef PAL_CLOSE
#endif
    if (reference != NULL)
        failed |= fclose(reference) != 0;
    muhtari_destroy(m);
    free(image);
    if (!failed)
        failed = pal_report(name, expected);
    return failed;
}

int main(int argc, char **argv)
{
    char rom[FIELD_MAX];
    if ((argc == 5 || argc == 6) && strcmp(argv[1], "--bankset-png") == 0)
        return bankset_png(argv[2], (unsigned)strtoul(argv[3], NULL, 10),
                           (unsigned)strtoul(argv[4], NULL, 10), argc == 6 ? argv[5] : NULL);
    if ((argc == 4 || argc == 5) && strcmp(argv[1], "--bankset-refuse") == 0) {
        bankset_refusal = strcmp(argv[3], "truncated") == 0 ? 2 : 1;
        int failed = controller_library(argv[2], "", "");
#ifdef DIAG_LIBRETRO
        if (argc == 5 && argv[4][0] != 0)
            failed |= controller_module(argv[4], argv[2]);
#endif
        return failed;
    }
    if (argc == 8 && strcmp(argv[1], "--bankset-image") == 0)
        return bankset_image(argv[2], argv[3], (unsigned)strtoul(argv[4], NULL, 10),
                             (unsigned)strtoul(argv[5], NULL, 10),
                             (unsigned)strtoul(argv[6], NULL, 10),
                             (unsigned)strtoul(argv[7], NULL, 10));
    if ((argc == 6 || argc == 7) && strcmp(argv[1], "--bankset-case") == 0) {
        bankset_tracer = 1;
        snprintf(bankset_name, sizeof bankset_name, "%s", argv[5]);
        bankset_pal = strstr(argv[2], "-pal.") != NULL;
        controller_pixels = malloc(CONTROLLER_FRAMES * CONTROLLER_PIXELS);
        bankset_module_pixels = malloc(CONTROLLER_FRAMES * CONTROLLER_PIXELS);
        if (controller_pixels == NULL || bankset_module_pixels == NULL)
            return 1;
        int failed = controller_library(argv[2], argv[4], argv[3]);
#ifdef DIAG_LIBRETRO
        if (!failed && argc == 7 && argv[6][0] != 0)
            failed = controller_module(argv[6], argv[2]);
#else
        if (argc == 7 && argv[6][0] != 0)
            failed = 1;
#endif
        free(controller_pixels);
        free(bankset_module_pixels);
        return failed;
    }
    if (argc == 4 && strcmp(argv[1], "--pal-report") == 0)
        return pal_report(argv[2], argv[3]);
    if ((argc == 5 || argc == 6) && strcmp(argv[1], "--pal-case") == 0)
        return pal_case(argv[2], argv[3], argv[4], argc == 6 ? argv[5] : NULL);

    if ((argc == 7 || argc == 8) && strcmp(argv[1], "--controller-case") == 0 &&
        (strcmp(argv[2], "driving") == 0 || strcmp(argv[2], "lightgun") == 0 ||
         strcmp(argv[2], "paddle") == 0 || strcmp(argv[2], "keypad") == 0 ||
         strcmp(argv[2], "trakball") == 0 || strcmp(argv[2], "st-mouse") == 0 ||
         strcmp(argv[2], "amiga-mouse") == 0 || strcmp(argv[2], "snes2atari") == 0 ||
         strcmp(argv[2], "mega7800") == 0) &&
        (strcmp(argv[3], "both") == 0 || strcmp(argv[3], "0") == 0 || strcmp(argv[3], "1") == 0 ||
         strcmp(argv[3], "2") == 0)) {
        controller_adapter = strcmp(argv[2], "snes2atari") == 0 ? 1u
                             : strcmp(argv[2], "mega7800") == 0 ? 2u
                                                                : 0u;
        controller_group =
            argv[3][0] >= '0' && argv[3][0] <= '2' ? (unsigned)(argv[3][0] - '0') : 0u;
        controller_motion = strcmp(argv[2], "trakball") == 0      ? 1u
                            : strcmp(argv[2], "st-mouse") == 0    ? 2u
                            : strcmp(argv[2], "amiga-mouse") == 0 ? 3u
                                                                  : 0u;
        controller_light = strcmp(argv[2], "lightgun") == 0;
        controller_analog = strcmp(argv[2], "paddle") == 0;
        controller_keypad = strcmp(argv[2], "keypad") == 0;
        controller_pixels = malloc(CONTROLLER_FRAMES * CONTROLLER_PIXELS);
        if (controller_pixels == NULL)
            return 1;
        int failed = controller_library(argv[4], argv[5], argv[6]);
#ifdef DIAG_LIBRETRO
        if (!failed && argc == 8)
            failed = controller_module(argv[7], argv[4]);
#else
        if (argc == 8)
            failed = 1;
#endif
        free(controller_pixels);
        return failed;
    }
    if (argc == 3 && strcmp(argv[1], "--battery-firmware") == 0) {
        uint8_t firmware[4096];
        memset(firmware, 255, sizeof firmware);
        firmware[0x10] = 0x5Au;
        firmware[0x900] = 0xC6u;
        firmware[0x904] = 0xFEu;
        FILE *f = fopen(argv[2], "wb");
        if (f == NULL)
            return 1;
        int failed = fwrite(firmware, 1, sizeof firmware, f) != sizeof firmware;
        failed |= fclose(f) != 0;
        return failed;
    }
    int battery_hang = argc == 4 && strcmp(argv[1], "--battery-timeout") == 0;
    if (battery_hang) {
        argc--;
        argv++;
    }
    while (argc > 2 &&
           (strcmp(argv[1], "--battery-count") == 0 || strcmp(argv[1], "--battery-marker") == 0 ||
            strcmp(argv[1], "--battery-import") == 0 || strcmp(argv[1], "--battery-export") == 0 ||
            strcmp(argv[1], "--battery-bmp") == 0 || strcmp(argv[1], "--hsc-rom") == 0)) {
        if (strcmp(argv[1], "--battery-count") == 0 || strcmp(argv[1], "--battery-marker") == 0) {
            struct range r;
            if (parse_range(argv[2], &r, 0) || r.lo != r.hi || r.lo > 65535u)
                return 2;
            if (strcmp(argv[1], "--battery-count") == 0)
                battery_count = r.lo;
            else
                battery_marker = r.lo;
        } else if (strcmp(argv[1], "--battery-import") == 0)
            battery_import = argv[2];
        else if (strcmp(argv[1], "--battery-export") == 0)
            battery_export = argv[2];
        else if (strcmp(argv[1], "--battery-bmp") == 0)
            battery_bmp = argv[2];
        else
            battery_firmware = argv[2];
        argc -= 2;
        argv += 2;
    }
    if (argc == 4 &&
        (strcmp(argv[1], "--battery-media") == 0 || strcmp(argv[1], "--battery-controls") == 0)) {
        struct range r;
        if (parse_range(argv[3], &r, 0) || r.lo != r.hi || r.lo > 65535u)
            return 2;
        return strcmp(argv[1], "--battery-media") == 0 ? battery_media_file(argv[2], r.lo)
                                                       : battery_controls(argv[2], r.lo);
    }
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
    if (battery_hang && n >= 16384u + 128u) {
        image[128] = 0x4Cu;
        image[129] = 0x00u;
        image[130] = 0xC0u;
        image[n - 4u] = 0x00u;
        image[n - 3u] = 0xC0u;
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
