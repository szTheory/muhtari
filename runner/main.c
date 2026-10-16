/* muhtari-run: the headless runner.
 *
 *   muhtari-run --frames N [--hash-frame N]... [--dump-frame N:FILE]...
 *
 * Runs N frames of one instance with no content; --frames is required. For
 * each --hash-frame N it prints, after frame N has run:
 *
 *   frame <N> sha256 <64 lowercase hex digits>
 *
 * The hash is SHA-256 over the 320x242 native colour bytes, row-major, top
 * row first. For each --dump-frame N:FILE it writes frame N to FILE as a
 * 320x242 RGB PNG in the warm NTSC palette.
 *
 * Exit status: 0 done, 1 failure, 2 usage.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "muhtari.h"
#include "palette.h"
#include "png.h"
#include "sha256.h"

#define WIDTH 320u
#define HEIGHT 242u
#define AUDIO_CAPACITY 801u

/* The allocator the runner hands the core: the C library's malloc and free. */
static void *run_alloc(void *user, size_t size)
{
    (void)user;
    return malloc(size);
}

static void run_free(void *user, void *ptr, size_t size)
{
    (void)user;
    (void)size;
    free(ptr);
}

static int usage(const char *why)
{
    fprintf(stderr, "muhtari-run: %s\n", why);
    fprintf(stderr, "usage: muhtari-run --frames N [--hash-frame N]... [--dump-frame N:FILE]...\n"
                    "  --frames N           run N frames (N >= 1)\n"
                    "  --hash-frame N       print the SHA-256 of frame N (1 <= N <= --frames)\n"
                    "  --dump-frame N:FILE  write frame N to FILE as a PNG (1 <= N <= --frames)\n");
    return 2;
}

/* Parses the first len characters of text as a decimal number from 1 to
   4294967295. Returns 0 on any other text. */
static int parse_count(const char *text, size_t len, uint32_t *out)
{
    uint64_t v = 0;
    if (text == NULL || len == 0u) {
        return 0;
    }
    for (const char *p = text; p < text + len; p++) {
        if (*p < '0' || *p > '9') {
            return 0;
        }
        v = v * 10u + (uint64_t)(*p - '0');
        if (v > 0xffffffffu) {
            return 0;
        }
    }
    if (v == 0u) {
        return 0;
    }
    *out = (uint32_t)v;
    return 1;
}

/* SHA-256 of the native colour bytes, which are bytes, so the result does
   not depend on the host's byte order. */
static void hash_frame(const uint8_t *video, char hex[65])
{
    muhtari__sha256 s;
    uint8_t digest[32];
    muhtari__sha256_init(&s);
    muhtari__sha256_update(&s, video, (size_t)WIDTH * HEIGHT);
    muhtari__sha256_final(&s, digest);
    muhtari__sha256_hex(digest, hex);
}

/* Writes the native frame to path as a PNG, converting each byte through the
   warm palette into R, G and B one row at a time. row has room for one row
   of RGB. Returns 0, or 1 after a message when the file cannot be written. */
static int dump_frame(const uint8_t *video, const char *path, uint8_t *row)
{
    FILE *f = fopen(path, "wb");
    if (f == NULL) {
        fprintf(stderr, "muhtari-run: cannot open %s for writing\n", path);
        return 1;
    }
    muhtari__png png;
    int failed = muhtari__png_begin(&png, f, WIDTH, HEIGHT) != 0;
    for (uint32_t y = 0; y < HEIGHT && !failed; y++) {
        for (uint32_t x = 0; x < WIDTH; x++) {
            uint32_t rgb = muhtari__palette_rgb(video[(size_t)y * WIDTH + x]);
            row[3u * x] = (uint8_t)(rgb >> 16);
            row[3u * x + 1u] = (uint8_t)(rgb >> 8);
            row[3u * x + 2u] = (uint8_t)rgb;
        }
        failed = muhtari__png_row(&png, row) != 0;
    }
    if (!failed) {
        failed = muhtari__png_end(&png) != 0;
    }
    if (fclose(f) != 0) {
        failed = 1;
    }
    if (failed) {
        fprintf(stderr, "muhtari-run: cannot write %s\n", path);
        return 1;
    }
    return 0;
}

/* The runner's options. Each list has room for one entry per argument. */
typedef struct options {
    uint32_t frames;
    uint32_t *hash_frames;
    uint32_t hash_count;
    uint32_t *dump_frames;
    const char **dump_paths;
    uint32_t dump_count;
} options;

/* Returns 0 when the options are valid, 2 after a usage message, 1 when out
   of memory. */
static int parse_options(int argc, char **argv, options *o)
{
    o->hash_frames = (uint32_t *)malloc((size_t)argc * sizeof *o->hash_frames);
    o->dump_frames = (uint32_t *)malloc((size_t)argc * sizeof *o->dump_frames);
    o->dump_paths = (const char **)malloc((size_t)argc * sizeof *o->dump_paths);
    if (o->hash_frames == NULL || o->dump_frames == NULL || o->dump_paths == NULL) {
        fprintf(stderr, "muhtari-run: out of memory\n");
        return 1;
    }
    /* Every option takes one value, so options sit at odd positions. */
    for (int i = 1; i < argc; i += 2) {
        const char *arg = argv[i];
        const char *value = i + 1 < argc ? argv[i + 1] : NULL;
        uint32_t n;
        if (strcmp(arg, "--frames") == 0 || strcmp(arg, "--hash-frame") == 0) {
            if (!parse_count(value, value == NULL ? 0u : strlen(value), &n)) {
                return usage("option needs a whole number of 1 or more");
            }
            if (strcmp(arg, "--frames") == 0) {
                o->frames = n;
            } else {
                o->hash_frames[o->hash_count++] = n;
            }
        } else if (strcmp(arg, "--dump-frame") == 0) {
            const char *colon = value == NULL ? NULL : strchr(value, ':');
            if (colon == NULL || colon[1] == '\0' ||
                !parse_count(value, (size_t)(colon - value), &n)) {
                return usage("--dump-frame needs N:FILE with N a whole number of 1 or more");
            }
            o->dump_frames[o->dump_count] = n;
            o->dump_paths[o->dump_count++] = colon + 1;
        } else {
            return usage(arg[0] == '-' ? "unknown option" : "unexpected argument");
        }
    }
    if (o->frames == 0u) {
        return usage("--frames N is required");
    }
    for (uint32_t k = 0; k < o->hash_count; k++) {
        if (o->hash_frames[k] > o->frames) {
            return usage("--hash-frame is beyond --frames");
        }
    }
    for (uint32_t k = 0; k < o->dump_count; k++) {
        if (o->dump_frames[k] > o->frames) {
            return usage("--dump-frame is beyond --frames");
        }
    }
    return 0;
}

static int listed(const uint32_t *list, uint32_t count, uint32_t f)
{
    for (uint32_t k = 0; k < count; k++) {
        if (list[k] == f) {
            return 1;
        }
    }
    return 0;
}

/* Runs the frames into heap buffers (a 77,440-byte local would overflow a
   64 KiB stack, as wasm32's is). Returns the exit status. */
static int run(const options *opt)
{
    uint8_t *video = (uint8_t *)malloc((size_t)WIDTH * HEIGHT);
    int16_t *audio = (int16_t *)malloc(AUDIO_CAPACITY * sizeof *audio);
    uint8_t *row = (uint8_t *)malloc(3u * WIDTH);
    if (video == NULL || audio == NULL || row == NULL) {
        fprintf(stderr, "muhtari-run: out of memory\n");
        free(video);
        free(audio);
        free(row);
        return 1;
    }

    muhtari_config cfg;
    memset(&cfg, 0, sizeof cfg);
    cfg.size = (uint32_t)sizeof cfg;
    cfg.allocator.alloc = run_alloc;
    cfg.allocator.free = run_free;
    muhtari *m = NULL;
    muhtari_status st = muhtari_create(&cfg, &m);
    if (st != MUHTARI_STATUS_OK) {
        fprintf(stderr, "muhtari-run: muhtari_create failed with status %lu\n", (unsigned long)st);
        free(video);
        free(audio);
        free(row);
        return 1;
    }

    int status = 0;
    /* A 64-bit count, so the loop ends when --frames is 4294967295. */
    for (uint64_t n = 1; n <= opt->frames; n++) {
        uint32_t f = (uint32_t)n;
        muhtari_frame io;
        memset(&io, 0, sizeof io);
        io.size = (uint32_t)sizeof io;
        io.video = video;
        io.video_pitch = WIDTH;
        io.audio = audio;
        io.audio_capacity = AUDIO_CAPACITY;
        st = muhtari_run_frame(m, &io);
        if (st != MUHTARI_STATUS_OK) {
            fprintf(stderr, "muhtari-run: muhtari_run_frame failed with status %lu\n",
                    (unsigned long)st);
            status = 1;
            break;
        }
        if (listed(opt->hash_frames, opt->hash_count, f)) {
            char hex[65];
            hash_frame(video, hex);
            printf("frame %lu sha256 %s\n", (unsigned long)f, hex);
        }
        for (uint32_t k = 0; k < opt->dump_count && status == 0; k++) {
            if (opt->dump_frames[k] == f) {
                status = dump_frame(video, opt->dump_paths[k], row);
            }
        }
        if (status != 0) {
            break;
        }
    }

    muhtari_destroy(m);
    free(video);
    free(audio);
    free(row);
    return status;
}

int main(int argc, char **argv)
{
    options opt;
    memset(&opt, 0, sizeof opt);
    int status = parse_options(argc, argv, &opt);
    if (status == 0) {
        status = run(&opt);
    }
    free(opt.hash_frames);
    free(opt.dump_frames);
    free(opt.dump_paths);
    if (fflush(stdout) != 0) {
        fprintf(stderr, "muhtari-run: cannot write output\n");
        return 1;
    }
    return status;
}
