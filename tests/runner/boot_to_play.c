/* The judge of a game played from boot through muhtari-run
   (tests/runner/boot_to_play.cmake, docs/emulation-tests.md, boot to play).

     muhtari-test-boot-to-play play|static|late DIR F H L BASE RUN
     muhtari-test-boot-to-play mirrored DIR F H BASE RUN MIRROR up|down|left|right

   DIR/<name>.txt is a run's --hash-frame output for every frame from 1 to
   F + H - 1, and DIR/<name>-<N>.png its dumps at the four frames dump_frame
   gives. BASE is the run with no input; RUN held an input from frame F for
   H frames. play judges three predicates and prints one line,
   "mask <bits>: <details>", with bit 0 for (a), 1 for (b) and 2 for (c):

     (a) the first frame whose hash differs from BASE's lies in [F, F + L];
     (b) the pixels that differ from BASE's at the three dumps in the hold
         window are there at each, and their centroid moves at least 4 px
         along one axis from the first to the last;
     (c) the picture is not frozen: RUN's hashes change in the window's
         last third.

   (b) and (c) judge a response, so they are judged only when some frame in
   the window differs from BASE's; a run that never responds fails (a)
   alone. static and late judge pure mutants of RUN's recorded output:
   static repeats RUN's dump before the window, recoloured, at every window
   dump, and late moves RUN's hashes from frame F one frame later.
   mirrored requires RUN's centroid to move mostly along one axis, in the
   direction named, and MIRROR's to move the other way along it. Exit
   status 0 when the judgement passes, 1 when it fails, 2 on a usage or file
   error. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "png_read.h"
#include "../../runner/sha256.h"
#include "muhtari.h"

#define HASH_LEN 64u
#define PATH_MAX_LEN 4096u
/* The longest run judged, in frames, so the hash table stays small. */
#define FRAMES_MAX 100000u
#define MIN_SHIFT 4

typedef struct timing {
    uint32_t f;    /* the first frame with input */
    uint32_t h;    /* frames held */
    uint32_t l;    /* the last frame after F where (a) accepts a response */
    uint32_t last; /* F + H - 1, the last frame run */
} timing;

/* The pixels that differ between two dumps: how many, and their coordinate
   sums. */
typedef struct diff {
    int64_t count;
    int64_t sx;
    int64_t sy;
} diff;

/* A centroid shift as numerators over one positive denominator. */
typedef struct shift {
    int64_t x;
    int64_t y;
    int64_t den;
} shift;

static int usage(void)
{
    fprintf(stderr, "usage: muhtari-test-boot-to-play play|static|late DIR F H L BASE RUN\n"
                    "       muhtari-test-boot-to-play mirrored DIR F H BASE RUN MIRROR "
                    "up|down|left|right\n");
    return 2;
}

static int parse_u32(const char *s, uint32_t *out)
{
    char *end = NULL;
    unsigned long v = strtoul(s, &end, 10);
    if (s[0] < '0' || s[0] > '9' || end == NULL || *end != '\0' || v == 0u || v > FRAMES_MAX) {
        return -1;
    }
    *out = (uint32_t)v;
    return 0;
}

/* Dump k, 0 to 3: the frame before the input, then the three window
   frames, the last being the last frame run. */
static uint32_t dump_frame(const timing *t, uint32_t k)
{
    return k == 0u ? t->f - 1u : t->f + k * t->h / 3u - 1u;
}

static int make_path(char *out, const char *dir, const char *name, const char *suffix)
{
    int n = snprintf(out, PATH_MAX_LEN, "%s/%s%s", dir, name, suffix);
    if (n < 0 || (unsigned)n >= PATH_MAX_LEN) {
        fprintf(stderr, "path too long: %s/%s%s\n", dir, name, suffix);
        return -1;
    }
    return 0;
}

/* Reads DIR/name.txt into hashes: HASH_LEN + 1 bytes per frame, frame k at
   k * (HASH_LEN + 1), from 1 to t->last, each required once. */
static int read_hashes(const char *dir, const char *name, const timing *t, char *hashes)
{
    char path[PATH_MAX_LEN];
    if (make_path(path, dir, name, ".txt") != 0) {
        return -1;
    }
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        fprintf(stderr, "cannot open %s\n", path);
        return -1;
    }
    char line[256];
    uint32_t seen = 0;
    int rc = 0;
    while (rc == 0 && fgets(line, sizeof line, f) != NULL) {
        /* "frame N sha256 HEX"; any other line, such as lag_frames, is
           skipped. */
        if (strncmp(line, "frame ", 6) != 0) {
            continue;
        }
        char *end = NULL;
        unsigned long frame = strtoul(line + 6, &end, 10);
        int ok = end != line + 6 && strncmp(end, " sha256 ", 8) == 0;
        const char *hex = ok ? end + 8 : line;
        if (!ok || frame == 0u || frame > t->last || strspn(hex, "0123456789abcdef") != HASH_LEN ||
            (hex[HASH_LEN] != '\n' && hex[HASH_LEN] != '\r') ||
            hashes[frame * (HASH_LEN + 1u)] != '\0') {
            fprintf(stderr, "%s: unexpected line %s", path, line);
            rc = -1;
        } else {
            memcpy(hashes + frame * (HASH_LEN + 1u), hex, HASH_LEN);
            seen++;
        }
    }
    fclose(f);
    if (rc == 0 && seen != t->last) {
        fprintf(stderr, "%s: %u frame hashes, expected %u\n", path, seen, t->last);
        rc = -1;
    }
    return rc;
}

static const char *hash_at(const char *hashes, uint32_t frame)
{
    return hashes + frame * (HASH_LEN + 1u);
}

/* Reads the four dumps of run name into raw, PNG_READ_RAW_SIZE bytes each. */
static int read_dumps(const char *dir, const char *name, const timing *t, uint8_t *raw)
{
    for (uint32_t k = 0; k < 4u; k++) {
        char suffix[32];
        char path[PATH_MAX_LEN];
        snprintf(suffix, sizeof suffix, "-%u.png", (unsigned)dump_frame(t, k));
        if (make_path(path, dir, name, suffix) != 0 ||
            png_read_file(path, raw + k * PNG_READ_RAW_SIZE) != 0) {
            return -1;
        }
    }
    return 0;
}

static diff diff_pixels(const uint8_t *a, const uint8_t *b)
{
    diff d = {0, 0, 0};
    for (uint32_t y = 0; y < PNG_READ_HEIGHT; y++) {
        const uint8_t *ra = a + y * PNG_READ_ROW + 1u;
        const uint8_t *rb = b + y * PNG_READ_ROW + 1u;
        for (uint32_t x = 0; x < PNG_READ_WIDTH; x++) {
            if (memcmp(ra + 3u * x, rb + 3u * x, 3) != 0) {
                d.count++;
                d.sx += x;
                d.sy += y;
            }
        }
    }
    return d;
}

/* The centroid shift from diff p to diff q; both must be non-empty. */
static shift centroid_shift(diff p, diff q)
{
    shift s;
    s.x = q.sx * p.count - p.sx * q.count;
    s.y = q.sy * p.count - p.sy * q.count;
    s.den = p.count * q.count;
    return s;
}

static int64_t abs64(int64_t v)
{
    return v < 0 ? -v : v;
}

/* The larger axis of s in whole pixels, rounded down. */
static int64_t shift_px(shift s)
{
    int64_t big = abs64(s.x) > abs64(s.y) ? abs64(s.x) : abs64(s.y);
    return big / s.den;
}

/* The differences of run from base at dumps 1 to 3. Returns 0 and sets *s
   to the centroid shift from dump 1 to dump 3 when every dump has one,
   else the first dump, 1 to 3, with none. */
static uint32_t window_shift(const uint8_t *base, const uint8_t *run, shift *s)
{
    diff d[4];
    for (uint32_t k = 1; k < 4u; k++) {
        d[k] = diff_pixels(base + k * PNG_READ_RAW_SIZE, run + k * PNG_READ_RAW_SIZE);
        if (d[k].count == 0) {
            return k;
        }
    }
    *s = centroid_shift(d[1], d[3]);
    return 0;
}

/* Recolours dump 0 by inverting each channel and copies it over dumps 1 to
   3: a picture that differs from the base but never moves. */
static void make_static(uint8_t *raw)
{
    for (uint32_t i = 0; i < PNG_READ_RAW_SIZE; i++) {
        if (i % PNG_READ_ROW != 0u) {
            raw[i] = (uint8_t)(255u - raw[i]);
        }
    }
    for (uint32_t k = 1; k < 4u; k++) {
        memcpy(raw + k * PNG_READ_RAW_SIZE, raw, PNG_READ_RAW_SIZE);
    }
}

/* Moves the hash list one frame later from frame F, as a latch one frame
   late would: frame k, from F on, gets frame k - 1's hash. The frames
   before the input stay as recorded. */
static void make_late(char *hashes, const timing *t)
{
    for (uint32_t k = t->last; k >= t->f; k--) {
        memcpy(hashes + k * (HASH_LEN + 1u), hashes + (k - 1u) * (HASH_LEN + 1u), HASH_LEN + 1u);
    }
}

/* Completed-run evidence is independent of image/control predicates. */
static int audible(const char *dir, const char *name, const timing *t, int muted)
{
    char path[PATH_MAX_LEN];
    if (make_path(path, dir, name, ".txt") != 0)
        return 0;
    FILE *f = fopen(path, "r");
    if (f == NULL)
        return 0;
    char line[256], pcm[65] = "", raw[65] = "";
    unsigned long long frames = 0, samples = 0;
    unsigned revision = 0, rate = 0, synthesis = 0;
    unsigned seen = 0;
    while (fgets(line, sizeof line, f) != NULL) {
        if (sscanf(line, "audio_pcm_sha256 %64s", pcm) == 1)
            seen |= 1u;
        else if (sscanf(line, "audio_transitions_sha256 %64s", raw) == 1)
            seen |= 2u;
        else if (sscanf(line, "audio_frames %llu", &frames) == 1)
            seen |= 4u;
        else if (sscanf(line, "audio_samples %llu", &samples) == 1)
            seen |= 8u;
        else if (sscanf(line, "audio_behaviour_revision %u", &revision) == 1)
            seen |= 16u;
        else if (strcmp(line, "audio_rate 48000/1\n") == 0)
            rate = 1;
        else if (strcmp(line, "audio_synthesis exact-q23-16x64-dc20\n") == 0)
            synthesis = 1;
    }
    fclose(f);
    muhtari_versions v = {0};
    v.size = sizeof v;
    (void)muhtari_get_versions(&v);
    uint64_t expected = (UINT64_C(8190) + (uint64_t)(t->last - 1u) * 119402u) * 88u / 13125u;
    if (seen != 31u || frames != t->last || samples != expected || samples == 0u || !rate ||
        !synthesis || revision != v.behaviour_revision || strlen(pcm) != 64u ||
        strspn(pcm, "0123456789abcdef") != 64u || strlen(raw) != 64u)
        return 0;
    muhtari__sha256 hash;
    uint8_t zero[2] = {0, 0}, digest[32];
    char silent[65];
    muhtari__sha256_init(&hash);
    for (uint64_t i = 0; i < samples; i++)
        muhtari__sha256_update(&hash, zero, 2);
    muhtari__sha256_final(&hash, digest);
    muhtari__sha256_hex(digest, silent);
    if (muted)
        memcpy(pcm, silent, sizeof pcm);
    return strcmp(pcm, silent) != 0;
}

/* Judges (a), (b) and (c). Returns the exit status. */
static int judge(const timing *t, const char *base_h, const char *run_h, const uint8_t *base_png,
                 const uint8_t *run_png, int sound)
{
    char details[512];
    size_t used = 0;
    uint32_t mask = 0;
    details[0] = '\0';

    uint32_t first = 0;
    int responded = 0;
    for (uint32_t k = 1; k <= t->last; k++) {
        if (strcmp(hash_at(base_h, k), hash_at(run_h, k)) != 0) {
            if (first == 0u) {
                first = k;
            }
            responded |= k >= t->f;
        }
    }
    if (first == 0u) {
        mask |= 1u;
        used += (size_t)snprintf(details + used, sizeof details - used,
                                 " (a) no frame differs from the base run;");
    } else if (first < t->f || first > t->f + t->l) {
        mask |= 1u;
        used += (size_t)snprintf(details + used, sizeof details - used,
                                 " (a) the first difference is at frame %u, not in %u to %u;",
                                 (unsigned)first, (unsigned)t->f, (unsigned)(t->f + t->l));
    } else {
        used += (size_t)snprintf(details + used, sizeof details - used,
                                 " the first difference is at frame %u;", (unsigned)first);
    }

    if (responded) {
        shift s;
        uint32_t empty = window_shift(base_png, run_png, &s);
        if (empty != 0u) {
            mask |= 2u;
            used += (size_t)snprintf(details + used, sizeof details - used,
                                     " (b) no pixel differs at frame %u;",
                                     (unsigned)dump_frame(t, empty));
        } else if (shift_px(s) < MIN_SHIFT) {
            mask |= 2u;
            used += (size_t)snprintf(details + used, sizeof details - used,
                                     " (b) the difference moved %lld px, under %d;",
                                     (long long)shift_px(s), MIN_SHIFT);
        } else {
            used += (size_t)snprintf(details + used, sizeof details - used,
                                     " the difference moved %lld px;", (long long)shift_px(s));
        }

        int changes = 0;
        for (uint32_t k = dump_frame(t, 2) + 1u; k <= t->last; k++) {
            changes |= strcmp(hash_at(run_h, k), hash_at(run_h, k - 1u)) != 0;
        }
        if (!changes) {
            mask |= 4u;
            used += (size_t)snprintf(details + used, sizeof details - used,
                                     " (c) the picture is frozen from frame %u;",
                                     (unsigned)dump_frame(t, 2));
        }
    }
    if (!sound) {
        mask |= 8u;
        snprintf(details + used, sizeof details - used,
                 " (d) completed audio is silent or invalid;");
    }
    printf("mask %u:%s\n", (unsigned)mask, details);
    return mask == 0u ? 0 : 1;
}

/* The sign of s along its larger axis, as a direction name. */
static const char *direction(shift s, int along_y)
{
    if (along_y) {
        return s.y < 0 ? "up" : s.y > 0 ? "down" : "nowhere";
    }
    return s.x < 0 ? "left" : s.x > 0 ? "right" : "nowhere";
}

/* Judges RUN against its mirror. Returns the exit status. */
static int judge_mirrored(const uint8_t *base_png, const uint8_t *run_png,
                          const uint8_t *mirror_png, const char *expect)
{
    shift s, m;
    if (window_shift(base_png, run_png, &s) != 0u || window_shift(base_png, mirror_png, &m) != 0u) {
        printf("sign did not flip: a run shows no difference in the window\n");
        return 1;
    }
    int along_y = abs64(s.y) > abs64(s.x);
    const char *went = direction(s, along_y);
    const char *back = direction(m, along_y);
    if (strcmp(went, expect) != 0) {
        printf("the run moved %s, expected %s\n", went, expect);
        return 1;
    }
    int64_t sv = along_y ? s.y : s.x;
    int64_t mv = along_y ? m.y : m.x;
    if (sv == 0 || mv == 0 || (sv < 0) == (mv < 0)) {
        printf("sign did not flip: the run moved %s, the mirror %s\n", went, back);
        return 1;
    }
    printf("sign flipped: the run moved %s %lld px, the mirror %s %lld px\n", went,
           (long long)(abs64(sv) / s.den), back, (long long)(abs64(mv) / m.den));
    return 0;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        return usage();
    }
    const char *mode = argv[1];
    int mirrored = strcmp(mode, "mirrored") == 0;
    int play = strcmp(mode, "play") == 0 || strcmp(mode, "muted") == 0 ||
               strcmp(mode, "static") == 0 || strcmp(mode, "late") == 0;
    timing t;
    if ((!mirrored && !play) || argc != (mirrored ? 9 : 8) || parse_u32(argv[3], &t.f) != 0 ||
        parse_u32(argv[4], &t.h) != 0 || t.f < 2u || t.h < 3u || t.f + t.h - 1u > FRAMES_MAX) {
        return usage();
    }
    t.last = t.f + t.h - 1u;
    t.l = 0;
    if (play && (parse_u32(argv[5], &t.l) != 0 || t.l >= t.h)) {
        return usage();
    }
    if (mirrored && strcmp(argv[8], "up") != 0 && strcmp(argv[8], "down") != 0 &&
        strcmp(argv[8], "left") != 0 && strcmp(argv[8], "right") != 0) {
        return usage();
    }
    const char *dir = argv[2];
    const char *base = mirrored ? argv[5] : argv[6];
    const char *run = mirrored ? argv[6] : argv[7];

    size_t table = (size_t)(t.last + 1u) * (HASH_LEN + 1u);
    char *base_h = (char *)calloc(table, 1);
    char *run_h = (char *)calloc(table, 1);
    uint8_t *png = (uint8_t *)malloc((size_t)12u * PNG_READ_RAW_SIZE);
    int status = 2;
    if (base_h == NULL || run_h == NULL || png == NULL) {
        fprintf(stderr, "out of memory\n");
    } else if (read_dumps(dir, base, &t, png) == 0 &&
               read_dumps(dir, run, &t, png + 4u * PNG_READ_RAW_SIZE) == 0) {
        uint8_t *base_png = png;
        uint8_t *run_png = png + 4u * PNG_READ_RAW_SIZE;
        if (mirrored) {
            if (read_dumps(dir, argv[7], &t, png + 8u * PNG_READ_RAW_SIZE) == 0) {
                status = judge_mirrored(base_png, run_png, png + 8u * PNG_READ_RAW_SIZE, argv[8]);
            }
        } else if (read_hashes(dir, base, &t, base_h) == 0 &&
                   read_hashes(dir, run, &t, run_h) == 0) {
            if (strcmp(mode, "static") == 0) {
                make_static(run_png);
            } else if (strcmp(mode, "late") == 0) {
                make_late(run_h, &t);
            }
            status = judge(&t, base_h, run_h, base_png, run_png,
                           audible(dir, run, &t, strcmp(mode, "muted") == 0));
        }
    }
    free(base_h);
    free(run_h);
    free(png);
    return status;
}
