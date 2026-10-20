/* muhtari-test-coverage COVERAGE BUILT_DIR DIAG_DIR: checks that the ROMs
   COVERAGE names exercise the features it claims for them
   (docs/emulation-tests.md, coverage).

   Each line of COVERAGE is "feature rom..." ("#" starts a comment). A ROM is
   7800basic/<name>, read from BUILT_DIR/<name>/<name>.bas.a78, or
   diag/<name>, read from DIAG_DIR/<name>.a78. A board feature is checked
   against muhtari_probe's board, options, ROM size and RAM size; a MARIA
   feature against the core's sticky feature mask after FRAMES fast-boot
   frames. Each ROM is loaded and run once. Only what the probe and the core
   report counts, never a ROM's source or name.

   Every feature in the table below is required: one with no claim fails.
   One line is printed per claim; exit 0 when all pass, 1 when any fails,
   and 2 on a usage error, an unreadable COVERAGE or an unknown feature. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"

#define FRAMES 600u
#define LINES_MAX 64u
#define CLAIMS_MAX 8u
#define ROMS_MAX 64u
#define NAME_MAX 64u
#define PATH_MAX_LEN 1024u

enum kind { KIND_BOARD, KIND_MASK };

/* A board claim: the probe's board, the options it must have, the ROM size
   (0 for any) and whether cartridge RAM is required (1), refused (0) or
   either (-1). */
struct feature {
    const char *name;
    enum kind kind;
    uint32_t bit;
    uint32_t board;
    uint32_t options;
    uint32_t rom_size;
    int ram;
};

static const struct feature features[] = {
    {"board-linear16", KIND_BOARD, 0u, MUHTARI_BOARD_LINEAR, 0u, 16384u, 0},
    {"board-linear32", KIND_BOARD, 0u, MUHTARI_BOARD_LINEAR, 0u, 32768u, 0},
    {"board-linear48", KIND_BOARD, 0u, MUHTARI_BOARD_LINEAR, 0u, 49152u, 0},
    {"board-supergame", KIND_BOARD, 0u, MUHTARI_BOARD_SUPERGAME, 0u, 0u, 0},
    {"board-supergame-ram", KIND_BOARD, 0u, MUHTARI_BOARD_SUPERGAME, 0u, 0u, 1},
    {"board-bank6", KIND_BOARD, 0u, MUHTARI_BOARD_SUPERGAME, MUHTARI_BOARD_BANK6_AT_4000, 0u, -1},
    {"board-sg144", KIND_BOARD, 0u, MUHTARI_BOARD_SUPERGAME, MUHTARI_BOARD_ROM_AT_4000, 147456u,
     -1},
    {"rm0", KIND_MASK, MUHTARI__FEAT_RM0, 0u, 0u, 0u, 0},
    {"rm2", KIND_MASK, MUHTARI__FEAT_RM2, 0u, 0u, 0u, 0},
    {"rm3", KIND_MASK, MUHTARI__FEAT_RM3, 0u, 0u, 0u, 0},
    {"write-mode", KIND_MASK, MUHTARI__FEAT_WRITE_MODE, 0u, 0u, 0u, 0},
    {"kangaroo", KIND_MASK, MUHTARI__FEAT_KANGAROO, 0u, 0u, 0u, 0},
    {"holey8", KIND_MASK, MUHTARI__FEAT_HOLEY8, 0u, 0u, 0u, 0},
    {"holey16", KIND_MASK, MUHTARI__FEAT_HOLEY16, 0u, 0u, 0u, 0},
    {"indirect1", KIND_MASK, MUHTARI__FEAT_IND1, 0u, 0u, 0u, 0},
    {"indirect2", KIND_MASK, MUHTARI__FEAT_IND2, 0u, 0u, 0u, 0},
    {"dli", KIND_MASK, MUHTARI__FEAT_DLI, 0u, 0u, 0u, 0},
};
#define FEATURES (sizeof features / sizeof features[0])

/* A ROM's evidence: what the probe said and the mask after FRAMES frames.
   ok is 0 when the ROM could not be read, probed, loaded or run. */
struct rom {
    char name[NAME_MAX];
    int ok;
    muhtari_cart_info info;
    uint32_t mask;
};

struct claim {
    const struct feature *feature;
    uint32_t rom[CLAIMS_MAX];
    uint32_t nrom;
    uint32_t line;
};

static struct rom roms[ROMS_MAX];
static uint32_t nroms;
static struct claim claims[LINES_MAX];
static uint32_t nclaims;

static void *cov_alloc(void *user, size_t size)
{
    (void)user;
    return malloc(size);
}

static void cov_free(void *user, void *ptr, size_t size)
{
    (void)user;
    (void)size;
    free(ptr);
}

static const struct feature *find_feature(const char *name)
{
    for (uint32_t i = 0; i < FEATURES; i++) {
        if (strcmp(features[i].name, name) == 0) {
            return &features[i];
        }
    }
    return NULL;
}

/* Returns the index of ROM name, adding it, or ROMS_MAX when the table is
   full. */
static uint32_t find_rom(const char *name)
{
    uint32_t i;
    for (i = 0; i < nroms && strcmp(roms[i].name, name) != 0; i++) {
    }
    if (i == nroms && nroms < ROMS_MAX) {
        memcpy(roms[i].name, name, strlen(name) + 1u);
        nroms++;
    }
    return i;
}

/* Returns the next blank-separated word at *p, NUL-terminated in place, and
   moves *p past it; NULL at the end of the line. */
static char *next_word(char **p)
{
    char *s = *p;
    while (*s == ' ' || *s == '\t' || *s == '\r' || *s == '\n') {
        s++;
    }
    if (*s == '\0') {
        *p = s;
        return NULL;
    }
    char *word = s;
    while (*s != '\0' && *s != ' ' && *s != '\t' && *s != '\r' && *s != '\n') {
        s++;
    }
    if (*s != '\0') {
        *s++ = '\0';
    }
    *p = s;
    return word;
}

/* Reads COVERAGE into claims. Returns 0, or 2 after a message. */
static int read_coverage(const char *path)
{
    char text[512];
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        fprintf(stderr, "muhtari-test-coverage: cannot open %s\n", path);
        return 2;
    }
    for (uint32_t ln = 1; fgets(text, sizeof text, f) != NULL; ln++) {
        char *hash = strchr(text, '#');
        char *p = text;
        if (hash != NULL) {
            *hash = '\0';
        }
        char *word = next_word(&p);
        if (word == NULL) {
            continue;
        }
        const struct feature *ft = find_feature(word);
        if (ft == NULL || nclaims == LINES_MAX) {
            fprintf(stderr, "%s:%lu: unknown feature %s, or too many lines\n", path,
                    (unsigned long)ln, word);
            fclose(f);
            return 2;
        }
        struct claim *c = &claims[nclaims++];
        c->feature = ft;
        c->nrom = 0;
        c->line = ln;
        while ((word = next_word(&p)) != NULL) {
            uint32_t r = strlen(word) < NAME_MAX ? find_rom(word) : ROMS_MAX;
            if (r == ROMS_MAX || c->nrom == CLAIMS_MAX) {
                fprintf(stderr, "%s:%lu: too many ROMs, or a name too long\n", path,
                        (unsigned long)ln);
                fclose(f);
                return 2;
            }
            c->rom[c->nrom++] = r;
        }
    }
    fclose(f);
    return 0;
}

/* Builds the file path of ROM name. Returns 0, or 1 for a name with no
   known prefix or a path too long. */
static int rom_path(const char *name, const char *built, const char *diag, char *out)
{
    int n = -1;
    if (strncmp(name, "7800basic/", 10u) == 0 && strchr(name + 10, '/') == NULL) {
        n = snprintf(out, PATH_MAX_LEN, "%s/%s/%s.bas.a78", built, name + 10, name + 10);
    } else if (strncmp(name, "diag/", 5u) == 0 && strchr(name + 5, '/') == NULL) {
        n = snprintf(out, PATH_MAX_LEN, "%s/%s.a78", diag, name + 5);
    }
    return n > 0 && (size_t)n < PATH_MAX_LEN ? 0 : 1;
}

/* Reads the whole file into a malloc'd buffer. Returns NULL. */
static uint8_t *read_file(const char *path, size_t *n)
{
    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        return NULL;
    }
    uint8_t *buf = (uint8_t *)malloc(MUHTARI_IMAGE_SIZE_MAX + 1u);
    size_t got = buf != NULL ? fread(buf, 1u, MUHTARI_IMAGE_SIZE_MAX + 1u, f) : 0u;
    int failed = buf == NULL || ferror(f) || got > MUHTARI_IMAGE_SIZE_MAX;
    fclose(f);
    if (failed) {
        free(buf);
        return NULL;
    }
    *n = got;
    return buf;
}

/* Probes, loads and runs ROM r for FRAMES frames, recording its evidence.
   A failure is printed and leaves r->ok 0. */
static void observe(struct rom *r, const char *built, const char *diag)
{
    char path[PATH_MAX_LEN];
    size_t n = 0;
    uint8_t *image = rom_path(r->name, built, diag, path) == 0 ? read_file(path, &n) : NULL;
    muhtari *m = NULL;
    muhtari_status st = MUHTARI_STATUS_OK;
    uint32_t f = 0;
    if (image == NULL) {
        printf("%s: cannot read the ROM\n", r->name);
        return;
    }
    memset(&r->info, 0, sizeof r->info);
    r->info.size = (uint32_t)sizeof r->info;
    st = muhtari_probe(image, n, &r->info);
    if (st == MUHTARI_STATUS_OK) {
        muhtari_config cfg;
        memset(&cfg, 0, sizeof cfg);
        cfg.size = (uint32_t)sizeof cfg;
        cfg.allocator.alloc = cov_alloc;
        cfg.allocator.free = cov_free;
        st = muhtari_create(&cfg, &m);
    }
    if (st == MUHTARI_STATUS_OK) {
        muhtari_media media;
        memset(&media, 0, sizeof media);
        media.size = (uint32_t)sizeof media;
        media.image = image;
        media.image_size = n;
        st = muhtari_load(m, &media);
    }
    for (f = 1; st == MUHTARI_STATUS_OK && f <= FRAMES; f++) {
        muhtari_frame io;
        memset(&io, 0, sizeof io);
        io.size = (uint32_t)sizeof io;
        st = muhtari_run_frame(m, &io);
    }
    if (st == MUHTARI_STATUS_OK) {
        r->mask = m->maria.features;
        r->ok = 1;
    } else {
        printf("%s: probe, load or frame %lu failed with status %lu\n", r->name, (unsigned long)f,
               (unsigned long)st);
    }
    muhtari_destroy(m);
    free(image);
}

/* Returns 1 when ROM r shows feature ft. */
static int shows(const struct rom *r, const struct feature *ft)
{
    const muhtari_cart_info *ci = &r->info;
    if (ft->kind == KIND_MASK) {
        return (r->mask & ft->bit) != 0u;
    }
    return ci->board == ft->board &&
           (ft->options == 0u ? ci->board_options == 0u
                              : (ci->board_options & ft->options) == ft->options) &&
           (ft->rom_size == 0u || ci->rom_size == ft->rom_size) &&
           (ft->ram < 0 || (ci->ram_size != 0u) == (ft->ram != 0));
}

int main(int argc, char **argv)
{
    int failures = 0;
    if (argc != 4) {
        fprintf(stderr, "usage: muhtari-test-coverage COVERAGE BUILT_DIR DIAG_DIR\n");
        return 2;
    }
    if (read_coverage(argv[1]) != 0) {
        return 2;
    }
    for (uint32_t i = 0; i < nroms; i++) {
        observe(&roms[i], argv[2], argv[3]);
    }
    for (uint32_t k = 0; k < FEATURES; k++) {
        uint32_t i;
        for (i = 0; i < nclaims && (claims[i].feature != &features[k] || claims[i].nrom == 0u);
             i++) {
        }
        if (i == nclaims) {
            printf("%s: FAIL no ROM claims it\n", features[k].name);
            failures++;
        }
    }
    for (uint32_t i = 0; i < nclaims; i++) {
        const struct claim *c = &claims[i];
        if (c->nrom == 0u) {
            printf("%s: FAIL no ROM on line %lu\n", c->feature->name, (unsigned long)c->line);
            failures++;
        }
        for (uint32_t j = 0; j < c->nrom; j++) {
            const struct rom *r = &roms[c->rom[j]];
            int ok = r->ok && shows(r, c->feature);
            printf("%s %s: board %lu options %lu rom %lu ram %lu mask %04lx %s\n", r->name,
                   c->feature->name, (unsigned long)r->info.board,
                   (unsigned long)r->info.board_options, (unsigned long)r->info.rom_size,
                   (unsigned long)r->info.ram_size, (unsigned long)r->mask, ok ? "PASS" : "FAIL");
            failures += ok ? 0 : 1;
        }
    }
    return failures == 0 ? 0 : 1;
}
