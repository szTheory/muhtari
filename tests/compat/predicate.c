/* muhtari-test-predicate ROM PREDICATE [--bios FILE] [--pin]
     [--id ID --result FILE]: evaluates one compatibility-ledger predicate
   on ROM (docs/emulation-tests.md, title).

     title@F=H  fast boot: frame F's SHA-256 equals H. With --bios: the
                hand-off frame K comes by frame 600, lies in [300, 340],
                and a frame in [K, K + 300) hashes to H.
     boots@F    the weak predicate: from the hand-off (frame 1 on the fast
                boot, K with --bios) to F frames later, DMA ran, at least
                two distinct frames appeared and the CPU is not jammed.

   A ";why=<reason>" suffix is the ledger's note and is not evaluated. A
   frame's hash is the SHA-256 of the selected region's native colour bytes,
   320 x 242 for NTSC and 320 x 292 for PAL (docs/emulation-tests.md, title).
   --pin prints title@F=<hex> for the fast boot's frame F, and fails with
   "single colour" when every byte of that frame is the same, so no blank
   screen is ever pinned. Prints "pass" or "fail <reason>"; exits 0 on a
   pass, 1 on a fail, and 2 on a usage error or a file that cannot be read
   or loaded.

   With --result the library preset's form runs: the fast boot, then, with
   --bios, the BIOS path too, and FILE gets the one line
   "ID,PREDICATE,STATUS,EVIDENCE", STATUS pass only when every path passed,
   EVIDENCE the first 16 hex digits of the fast boot's frame F for a title
   predicate and - otherwise. FILE is removed first, so a run that stops
   early leaves none. It exits 0 once FILE is written, pass or fail, since
   the ratchet judges the line. The program includes src/internal.h to
   read K and the CPU's jam flag and MARIA's line statistics. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"
#include "sha256.h"

#define AUDIO_CAPACITY (2u * MUHTARI__MAX_AUDIO_SAMPLES)

/* OpenBIOS shows its Fuji for 320 frames, then hands off at the next
   VBLANK start (tests/boot/test_boot.c gives the window's reasons). A
   title must appear within 300 frames of the hand-off, which must come by
   frame 600. */
#define K_MIN 300u
#define K_MAX 340u
#define K_LIMIT 600u
#define TITLE_WINDOW 300u

/* The largest F a predicate may name. */
#define F_MAX 100000u

static uint8_t *video;
static int16_t *audio;

/* The bytes of the loaded region's frame, width x height, read after each
   load; hashing and scanning stop there, whatever the buffer holds. */
static size_t frame_bytes;

static void *pred_alloc(void *user, size_t size)
{
    (void)user;
    return malloc(size);
}

static void pred_free(void *user, void *ptr, size_t size)
{
    (void)user;
    (void)size;
    free(ptr);
}

/* Reads at most cap bytes of path into a malloc'd buffer; NULL after a
   message when the file cannot be read, is empty or is longer. */
static uint8_t *read_file(const char *path, size_t cap, size_t *n)
{
    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        fprintf(stderr, "muhtari-test-predicate: cannot open %s\n", path);
        return NULL;
    }
    uint8_t *buf = (uint8_t *)malloc(cap + 1u);
    size_t got = buf != NULL ? fread(buf, 1u, cap + 1u, f) : 0u;
    int bad = buf == NULL || ferror(f) != 0 || got == 0u || got > cap;
    (void)fclose(f);
    if (bad) {
        fprintf(stderr, "muhtari-test-predicate: cannot read %s\n", path);
        free(buf);
        return NULL;
    }
    *n = got;
    return buf;
}

/* Parses a decimal frame number from 1 to F_MAX at *p, advancing *p; 0
   otherwise. */
static uint32_t parse_frame(const char **p)
{
    uint32_t v = 0u;
    const char *s = *p;
    while (*s >= '0' && *s <= '9' && v <= F_MAX) {
        v = v * 10u + (uint32_t)(*s - '0');
        s++;
    }
    *p = s;
    return v <= F_MAX ? v : 0u;
}

/* A parsed predicate. hash is empty for boots@F and for title@F=-. */
struct predicate {
    int title; /* title@F=H, else boots@F */
    uint32_t f;
    char hash[65];
};

/* Parses text; returns 0 on a malformed predicate. */
static int parse_predicate(const char *text, struct predicate *pr)
{
    memset(pr, 0, sizeof *pr);
    const char *p = text;
    if (strncmp(p, "title@", 6u) == 0) {
        pr->title = 1;
        p += 6;
    } else if (strncmp(p, "boots@", 6u) == 0) {
        p += 6;
    } else {
        return 0;
    }
    pr->f = parse_frame(&p);
    if (pr->f == 0u) {
        return 0;
    }
    if (pr->title) {
        if (*p++ != '=') {
            return 0;
        }
        if (*p == '-') {
            p++;
        } else {
            for (size_t i = 0; i < 64u; i++, p++) {
                if (!((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'f'))) {
                    return 0;
                }
                pr->hash[i] = *p;
            }
        }
    }
    return *p == '\0' || strncmp(p, ";why=", 5u) == 0;
}

/* Runs one frame into video; returns its status. */
static muhtari_status frame(muhtari *m)
{
    muhtari_frame io;
    memset(&io, 0, sizeof io);
    io.size = (uint32_t)sizeof io;
    io.video = video;
    io.video_pitch = MUHTARI__WIDTH;
    io.audio = audio;
    io.audio_capacity = AUDIO_CAPACITY;
    return muhtari_run_frame(m, &io);
}

static void frame_hash(char hex[65])
{
    uint8_t d[32];
    muhtari__sha256 s;
    muhtari__sha256_init(&s);
    muhtari__sha256_update(&s, video, frame_bytes);
    muhtari__sha256_final(&s, d);
    muhtari__sha256_hex(d, hex);
}

/* Whether the last frame's lines ran a DMA burst: a line with DMA off or
   refused keeps zero cost (docs/hardware/maria.md, Statistics). The lines
   scanned are the selected region's, bounded by the statistics array. */
static int dma_ran(const muhtari *m)
{
    uint32_t lines = muhtari__timing(m).lines;
    if (lines > sizeof m->maria.stats / sizeof m->maria.stats[0]) {
        lines = (uint32_t)(sizeof m->maria.stats / sizeof m->maria.stats[0]);
    }
    for (uint32_t line = 0u; line < lines; line++) {
        if (m->maria.stats[line].cost != 0u) {
            return 1;
        }
    }
    return 0;
}

/* Runs to the hand-off: frame 1 on the fast boot, K with a BIOS, which
   must come by K_LIMIT and lie in [K_MIN, K_MAX]. Returns the frames run,
   K, or 0 after printing the failure. */
static uint64_t run_to_handoff(muhtari *m, int bios)
{
    while (muhtari__boot_handoff_frame(m) == 0u && m->frames < K_LIMIT) {
        muhtari_status st = frame(m);
        if (st != MUHTARI_STATUS_OK) {
            printf("fail frame %llu status %lu\n", (unsigned long long)m->frames + 1u,
                   (unsigned long)st);
            return 0u;
        }
    }
    uint64_t k = muhtari__boot_handoff_frame(m);
    if (k == 0u) {
        printf("fail no hand-off by frame %u\n", K_LIMIT);
        return 0u;
    }
    if (bios && (k < K_MIN || k > K_MAX)) {
        printf("fail hand-off frame %llu outside [%u, %u]\n", (unsigned long long)k, K_MIN, K_MAX);
        return 0u;
    }
    return k;
}

/* Runs frames until the frames run reach last, the hand-off frame's
   window included; at each frame from first on, test(m) decides. Returns
   1 when test passed, 0 after a failure line, -1 when the frames ran
   out. */
static int run_window(muhtari *m, uint64_t first, uint64_t last, int (*test)(muhtari *, int))
{
    if (m->frames >= first && test(m, m->frames == last)) {
        return 1;
    }
    while (m->frames < last) {
        muhtari_status st = frame(m);
        if (st != MUHTARI_STATUS_OK) {
            printf("fail frame %llu status %lu\n", (unsigned long long)m->frames + 1u,
                   (unsigned long)st);
            return 0;
        }
        if (m->frames >= first && test(m, m->frames == last)) {
            return 1;
        }
    }
    return -1;
}

/* The title test's target and the last frame it hashed, and the boots
   test's running state. */
static const char *want;
static char seen[65];
static char first_hash[65];
static int saw_dma, saw_two;

/* Stops at the window's last frame. */
static int at_last(muhtari *m, int last)
{
    (void)m;
    return last;
}

static int title_test(muhtari *m, int last)
{
    (void)m;
    (void)last;
    frame_hash(seen);
    return strcmp(seen, want) == 0;
}

static int boots_test(muhtari *m, int last)
{
    char hex[65];
    frame_hash(hex);
    if (first_hash[0] == '\0') {
        memcpy(first_hash, hex, sizeof hex);
    } else if (strcmp(hex, first_hash) != 0) {
        saw_two = 1;
    }
    saw_dma |= dma_ran(m);
    return last;
}

static int evaluate(muhtari *m, const struct predicate *pr, int bios, int pin)
{
    uint64_t k = run_to_handoff(m, bios);
    if (k == 0u) {
        return 1;
    }
    if (pin) {
        if (run_window(m, pr->f, pr->f, at_last) == 0) {
            return 1;
        }
        for (size_t i = 1u; i < frame_bytes; i++) {
            if (video[i] != video[0]) {
                char hex[65];
                frame_hash(hex);
                printf("title@%lu=%s\n", (unsigned long)pr->f, hex);
                return 0;
            }
        }
        printf("fail single colour %02x at frame %lu\n", (unsigned)video[0], (unsigned long)pr->f);
        return 1;
    }
    if (pr->title) {
        if (pr->hash[0] == '\0') {
            printf("fail title@%lu is not pinned\n", (unsigned long)pr->f);
            return 1;
        }
        want = pr->hash;
        uint64_t first = bios ? k : pr->f;
        uint64_t last = bios ? k + TITLE_WINDOW - 1u : pr->f;
        int r = run_window(m, first, last, title_test);
        if (r == 1) {
            printf("pass frame %llu\n", (unsigned long long)m->frames);
            return 0;
        }
        if (r == -1) {
            printf("fail title not in frames %llu-%llu\n", (unsigned long long)first,
                   (unsigned long long)last);
        }
        return 1;
    }
    first_hash[0] = '\0';
    saw_dma = 0;
    saw_two = 0;
    if (run_window(m, k, k + pr->f - 1u, boots_test) == 0) {
        return 1;
    }
    const char *why = !saw_dma        ? "no DMA"
                      : !saw_two      ? "one distinct frame"
                      : m->cpu.jammed ? "CPU jammed"
                                      : NULL;
    if (why != NULL) {
        printf("fail %s by frame %llu\n", why, (unsigned long long)m->frames);
        return 1;
    }
    printf("pass\n");
    return 0;
}

/* Loads rom, with bios when it is not NULL, and evaluates pr; returns
   evaluate's status, or 2 after a message when create or load fails. */
static int run_path(const char *name, const uint8_t *rom, size_t rom_n, const uint8_t *bios,
                    size_t bios_n, const struct predicate *pr, int pin)
{
    muhtari_config cfg;
    memset(&cfg, 0, sizeof cfg);
    cfg.size = (uint32_t)sizeof cfg;
    cfg.allocator.alloc = pred_alloc;
    cfg.allocator.free = pred_free;
    muhtari_media media;
    memset(&media, 0, sizeof media);
    media.size = (uint32_t)sizeof media;
    media.image = rom;
    media.image_size = rom_n;
    media.bios = bios;
    media.bios_size = bios_n;
    muhtari *m = NULL;
    int status = 2;
    muhtari_status st = muhtari_create(&cfg, &m);
    if (st == MUHTARI_STATUS_OK) {
        st = muhtari_load(m, &media);
    }
    muhtari_info info;
    memset(&info, 0, sizeof info);
    info.size = (uint32_t)sizeof info;
    if (st == MUHTARI_STATUS_OK) {
        st = muhtari_get_info(m, &info);
    }
    if (st == MUHTARI_STATUS_OK) {
        frame_bytes = (size_t)info.width * info.height;
        status = evaluate(m, pr, bios != NULL, pin);
    } else {
        fprintf(stderr, "muhtari-test-predicate: %s: create or load failed with status %lu\n", name,
                (unsigned long)st);
    }
    muhtari_destroy(m);
    return status;
}

/* The --result form: the fast boot, then the BIOS path when bios is not
   NULL, and one line written to path. Returns 0 once the line is written. */
static int run_result(const char *name, const char *text, const uint8_t *rom, size_t rom_n,
                      const uint8_t *bios, size_t bios_n, const struct predicate *pr,
                      const char *id, const char *path)
{
    seen[0] = '\0';
    int fast = run_path(name, rom, rom_n, NULL, 0u, pr, 0);
    char evidence[17] = "-";
    if (pr->title && seen[0] != '\0') {
        memcpy(evidence, seen, 16u);
        evidence[16] = '\0';
    }
    int slow = bios != NULL ? run_path(name, rom, rom_n, bios, bios_n, pr, 0) : 0;
    if (fast == 2 || slow == 2) {
        return 2;
    }
    FILE *f = fopen(path, "wb");
    if (f == NULL) {
        fprintf(stderr, "muhtari-test-predicate: cannot write %s\n", path);
        return 2;
    }
    int bad = fprintf(f, "%s,%s,%s,%s\n", id, text, fast == 0 && slow == 0 ? "pass" : "fail",
                      evidence) < 0;
    bad |= fclose(f) != 0;
    if (bad) {
        fprintf(stderr, "muhtari-test-predicate: cannot write %s\n", path);
        (void)remove(path);
        return 2;
    }
    return 0;
}

int main(int argc, char **argv)
{
    const char *bios_path = NULL;
    const char *id = NULL;
    const char *result = NULL;
    int pin = 0;
    int usage = argc < 3;
    for (int i = 3; i < argc && !usage; i++) {
        if (strcmp(argv[i], "--pin") == 0) {
            pin = 1;
        } else if (strcmp(argv[i], "--bios") == 0 && i + 1 < argc) {
            bios_path = argv[++i];
        } else if (strcmp(argv[i], "--id") == 0 && i + 1 < argc) {
            id = argv[++i];
        } else if (strcmp(argv[i], "--result") == 0 && i + 1 < argc) {
            result = argv[++i];
        } else {
            usage = 1;
        }
    }
    struct predicate pr;
    if (usage || !parse_predicate(argv[2], &pr) || (pin && (!pr.title || bios_path != NULL)) ||
        (id == NULL) != (result == NULL) || (pin && result != NULL)) {
        fprintf(stderr, "usage: muhtari-test-predicate ROM PREDICATE [--bios FILE] [--pin]\n"
                        "         [--id ID --result FILE]\n"
                        "  PREDICATE: title@F=H, title@F=- with --pin, or boots@F, F >= 1;\n"
                        "  --pin takes no --bios and no --result\n");
        return 2;
    }
    if (result != NULL) {
        (void)remove(result);
    }
    size_t rom_n = 0u;
    size_t bios_n = 0u;
    uint8_t *rom = read_file(argv[1], MUHTARI_IMAGE_SIZE_MAX, &rom_n);
    uint8_t *bios =
        bios_path != NULL ? read_file(bios_path, MUHTARI__BIOS_SIZE_MAX, &bios_n) : NULL;
    /* Frame and audio buffers on the heap: wasm32's stack is 64 KiB. The
       video buffer holds the taller (PAL) region's frame. */
    video = (uint8_t *)malloc((size_t)MUHTARI__WIDTH *
                              muhtari__timing_for_region(MUHTARI_REGION_PAL).height);
    audio = (int16_t *)malloc(AUDIO_CAPACITY * sizeof *audio);
    int status = 2;
    if (rom != NULL && (bios_path == NULL || bios != NULL) && video != NULL && audio != NULL) {
        status = result != NULL
                     ? run_result(argv[1], argv[2], rom, rom_n, bios, bios_n, &pr, id, result)
                     : run_path(argv[1], rom, rom_n, bios, bios_n, &pr, pin);
    }
    free(rom);
    free(bios);
    free(video);
    free(audio);
    return status;
}
