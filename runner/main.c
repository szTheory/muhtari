/* muhtari-run: the headless runner.
 *
 *   muhtari-run --frames N [--hash-frame N]... [--dump-frame N:FILE]...
 *   muhtari-run --bench N [--hash-frame N]... [--dump-frame N:FILE]...
 *   muhtari-run --help | --version
 *
 * Runs N frames of one instance with no content; --frames or --bench is
 * required. For each --hash-frame N it prints, after frame N has run:
 *
 *   frame <N> sha256 <64 lowercase hex digits>
 *
 * The hash is SHA-256 over the 320x242 native colour bytes, row-major, top
 * row first. For each --dump-frame N:FILE it writes frame N to FILE as a
 * 320x242 RGB PNG in the warm NTSC palette. --bench N also times each
 * frame and prints the report described in bench.h. SIGINT or SIGTERM
 * ends the run after the frame in progress; output is flushed and the
 * status is 0.
 *
 * Exit statuses are listed in exit_status.h and printed by --help.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bench.h"
#include "clock.h"
#include "exit_status.h"
#include "muhtari.h"
#include "palette.h"
#include "png.h"
#include "sha256.h"
#include "stop.h"

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

static const char synopsis[] =
    "usage: muhtari-run --frames N [--hash-frame N]... [--dump-frame N:FILE]...\n"
    "       muhtari-run --bench N [--hash-frame N]... [--dump-frame N:FILE]...\n"
    "       muhtari-run --help | --version\n"
    "  --frames N           run N frames (N >= 1)\n"
    "  --bench N            run N frames and report the time per frame\n"
    "  --hash-frame N       print the SHA-256 of frame N (1 <= N <= --frames)\n"
    "  --dump-frame N:FILE  write frame N to FILE as a PNG (1 <= N <= --frames)\n"
    "  --help               print this text and the exit statuses\n"
    "  --version            print the version\n";

static int usage(const char *why)
{
    fprintf(stderr, "muhtari-run: %s\n%s", why, synopsis);
    return MUHTARI__EXIT_USAGE;
}

static void help(void)
{
    fputs(synopsis, stdout);
    fputs("exit status:\n", stdout);
    for (size_t i = 0; i < sizeof muhtari__exit_statuses / sizeof muhtari__exit_statuses[0]; i++) {
        printf("  %d  %-7s  %s\n", muhtari__exit_statuses[i].code, muhtari__exit_statuses[i].name,
               muhtari__exit_statuses[i].meaning);
    }
}

static void version(void)
{
    muhtari_versions v;
    memset(&v, 0, sizeof v);
    v.size = (uint32_t)sizeof v;
    (void)muhtari_get_versions(&v);
    printf("muhtari-run %lu.%lu.%lu\n", (unsigned long)v.library_major,
           (unsigned long)v.library_minor, (unsigned long)v.library_patch);
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
        return MUHTARI__EXIT_FAILURE;
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
        return MUHTARI__EXIT_FAILURE;
    }
    return MUHTARI__EXIT_DONE;
}

/* The runner's options. Each list has room for one entry per argument. */
typedef struct options {
    uint32_t frames;
    int bench;
    uint32_t *hash_frames;
    uint32_t hash_count;
    uint32_t *dump_frames;
    const char **dump_paths;
    uint32_t dump_count;
    int printed; /* --help or --version ran; nothing else to do */
} options;

static int compare_u32(const void *a, const void *b)
{
    uint32_t x = *(const uint32_t *)a;
    uint32_t y = *(const uint32_t *)b;
    return (x > y) - (x < y);
}

/* Returns 0 when the options are valid, 2 after a usage message, 1 when out
   of memory. The hash frames come back sorted with duplicates removed. */
static int parse_options(int argc, char **argv, options *o)
{
    o->hash_frames = (uint32_t *)malloc((size_t)argc * sizeof *o->hash_frames);
    o->dump_frames = (uint32_t *)malloc((size_t)argc * sizeof *o->dump_frames);
    o->dump_paths = (const char **)malloc((size_t)argc * sizeof *o->dump_paths);
    if (o->hash_frames == NULL || o->dump_frames == NULL || o->dump_paths == NULL) {
        fprintf(stderr, "muhtari-run: out of memory\n");
        return MUHTARI__EXIT_FAILURE;
    }
    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        help();
        o->printed = 1;
        return MUHTARI__EXIT_DONE;
    }
    if (argc == 2 && strcmp(argv[1], "--version") == 0) {
        version();
        o->printed = 1;
        return MUHTARI__EXIT_DONE;
    }
    /* Every other option takes one value, so options sit at odd positions. */
    for (int i = 1; i < argc; i += 2) {
        const char *arg = argv[i];
        const char *value = i + 1 < argc ? argv[i + 1] : NULL;
        uint32_t n;
        if (strcmp(arg, "--frames") == 0 || strcmp(arg, "--bench") == 0 ||
            strcmp(arg, "--hash-frame") == 0) {
            if (!parse_count(value, value == NULL ? 0u : strlen(value), &n)) {
                return usage("option needs a whole number of 1 or more");
            }
            if (strcmp(arg, "--hash-frame") != 0) {
                if (o->frames != 0u) {
                    return usage("give --frames or --bench once");
                }
                o->frames = n;
                o->bench = strcmp(arg, "--bench") == 0;
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
        } else if (strcmp(arg, "--help") == 0 || strcmp(arg, "--version") == 0) {
            return usage("--help and --version take no other arguments");
        } else {
            return usage(arg[0] == '-' ? "unknown option" : "unexpected argument");
        }
    }
    if (o->frames == 0u) {
        return usage("--frames N or --bench N is required");
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
    if (o->hash_count > 0u) {
        qsort(o->hash_frames, o->hash_count, sizeof o->hash_frames[0], compare_u32);
        uint32_t kept = 1;
        for (uint32_t k = 1; k < o->hash_count; k++) {
            if (o->hash_frames[k] != o->hash_frames[kept - 1u]) {
                o->hash_frames[kept++] = o->hash_frames[k];
            }
        }
        o->hash_count = kept;
    }
    return MUHTARI__EXIT_DONE;
}

/* Runs the frames into heap buffers (a 77,440-byte local would overflow a
   64 KiB stack, as wasm32's is). Returns the exit status. */
static int run(const options *opt)
{
    uint8_t *video = (uint8_t *)malloc((size_t)WIDTH * HEIGHT);
    int16_t *audio = (int16_t *)malloc(AUDIO_CAPACITY * sizeof *audio);
    uint8_t *row = (uint8_t *)malloc(3u * WIDTH);
    /* One microsecond sample per frame for --bench, on the heap: 300,000
       frames take 1.2 MB. */
    uint32_t *samples = opt->bench ? (uint32_t *)malloc((size_t)opt->frames * sizeof *samples)
                                   : NULL;
    if (video == NULL || audio == NULL || row == NULL || (opt->bench && samples == NULL)) {
        fprintf(stderr, "muhtari-run: out of memory\n");
        free(video);
        free(audio);
        free(row);
        free(samples);
        return MUHTARI__EXIT_FAILURE;
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
        free(samples);
        return MUHTARI__EXIT_FAILURE;
    }

    int status = MUHTARI__EXIT_DONE;
    uint32_t next_hash = 0;
    uint32_t timed = 0;
    uint64_t total_ns = 0;
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
        uint64_t start_ns = opt->bench ? muhtari__clock_ns() : 0u;
        st = muhtari_run_frame(m, &io);
        if (opt->bench) {
            uint64_t ns = muhtari__clock_ns() - start_ns;
            samples[timed++] = muhtari__bench_us(ns);
            total_ns += ns;
        }
        if (st != MUHTARI_STATUS_OK) {
            fprintf(stderr, "muhtari-run: muhtari_run_frame failed with status %lu\n",
                    (unsigned long)st);
            status = MUHTARI__EXIT_FAILURE;
            break;
        }
        if (next_hash < opt->hash_count && opt->hash_frames[next_hash] == f) {
            next_hash++;
            char hex[65];
            hash_frame(video, hex);
            printf("frame %lu sha256 %s\n", (unsigned long)f, hex);
        }
        for (uint32_t k = 0; k < opt->dump_count && status == 0; k++) {
            if (opt->dump_frames[k] == f) {
                status = dump_frame(video, opt->dump_paths[k], row);
            }
        }
        /* A stop request ends the run only between whole frames, after the
           frame's outputs are written. */
        if (status != 0 || muhtari__stop_requested()) {
            break;
        }
    }

    if (status == MUHTARI__EXIT_DONE && opt->bench) {
        char report[512];
        size_t len = muhtari__bench_format(report, sizeof report, samples, timed, total_ns);
        fwrite(report, 1u, len, stdout);
    }

    muhtari_destroy(m);
    free(video);
    free(audio);
    free(row);
    free(samples);
    return status;
}

int main(int argc, char **argv)
{
    options opt;
    memset(&opt, 0, sizeof opt);
    int status = parse_options(argc, argv, &opt);
    if (status == MUHTARI__EXIT_DONE && !opt.printed) {
        muhtari__stop_install();
        status = run(&opt);
    }
    free(opt.hash_frames);
    free(opt.dump_frames);
    free(opt.dump_paths);
    if (fflush(stdout) != 0) {
        fprintf(stderr, "muhtari-run: cannot write output\n");
        return MUHTARI__EXIT_FAILURE;
    }
    return status;
}
