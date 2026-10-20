/* muhtari-run: the headless runner.
 *
 *   muhtari-run --frames N [--hash-frame N]... [--dump-frame N:FILE]... [[--rom] FILE]
 *   muhtari-run --bench N [--hash-frame N]... [--dump-frame N:FILE]... [[--rom] FILE]
 *   muhtari-run --probe FILE
 *   muhtari-run --help | --version
 *
 * Runs N frames of one instance, with the cartridge FILE loaded through
 * muhtari_load and fast-booted, or with no content when there is no FILE;
 * --frames or --bench is required. For each --hash-frame N it prints, after frame N has run:
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
 * --probe FILE identifies a cartridge image with muhtari_probe and prints
 * its fields and eight hashes as "key value" lines; it runs no frames.
 * docs/runner.md lists the lines and their formats.
 *
 * Exit statuses are listed in exit_status.h and printed by --help.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bench.h"
#include "clock.h"
#include "crc32.h"
#include "exit_status.h"
#include "md5.h"
#include "movie.h"
#include "muhtari.h"
#include "palette.h"
#include "png.h"
#include "sha1.h"
#include "sha256.h"
#include "status_text.h"
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
    "usage: muhtari-run --frames N [--hash-frame N]... [--dump-frame N:FILE]... [--rom] FILE\n"
    "       muhtari-run --frames N [--hash-frame N]... [--dump-frame N:FILE]...\n"
    "       muhtari-run --bench N [--hash-frame N]... [--dump-frame N:FILE]... [[--rom] FILE]\n"
    "       muhtari-run --probe FILE\n"
    "       muhtari-run --help | --version\n"
    "  --frames N           run N frames (N >= 1)\n"
    "  --bench N            run N frames and report the time per frame\n"
    "  --hash-frame N       print the SHA-256 of frame N (1 <= N <= --frames)\n"
    "  --dump-frame N:FILE  write frame N to FILE as a PNG (1 <= N <= --frames)\n"
    "  [--rom] FILE         load the cartridge image FILE; without it no content runs\n"
    "  --probe FILE         print the cartridge image's fields and hashes\n"
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
        printf(" %2d  %-7s  %s\n", muhtari__exit_statuses[i].code, muhtari__exit_statuses[i].name,
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

/* Writes n bytes as 2n lowercase hex digits and a terminating NUL. */
static void to_hex(const uint8_t *digest, size_t n, char *hex)
{
    static const char digits[] = "0123456789abcdef";
    for (size_t i = 0; i < n; i++) {
        hex[2u * i] = digits[digest[i] >> 4];
        hex[2u * i + 1u] = digits[digest[i] & 0x0fu];
    }
    hex[2u * n] = '\0';
}

/* Prints the SHA-1, SHA-256, MD5 and CRC-32 of n bytes at p, each key
   followed by suffix. */
static void print_hashes(const char *suffix, const uint8_t *p, size_t n)
{
    uint8_t digest[32];
    char hex[65];
    struct muhtari__sha1 sha1;
    muhtari__sha1_init(&sha1);
    muhtari__sha1_update(&sha1, p, n);
    muhtari__sha1_final(&sha1, digest);
    to_hex(digest, 20u, hex);
    printf("sha1%s %s\n", suffix, hex);
    muhtari__sha256 sha256;
    muhtari__sha256_init(&sha256);
    muhtari__sha256_update(&sha256, p, n);
    muhtari__sha256_final(&sha256, digest);
    to_hex(digest, 32u, hex);
    printf("sha256%s %s\n", suffix, hex);
    muhtari__md5 md5;
    muhtari__md5_init(&md5);
    muhtari__md5_update(&md5, p, n);
    muhtari__md5_final(&md5, digest);
    to_hex(digest, 16u, hex);
    printf("md5%s %s\n", suffix, hex);
    printf("crc32%s %08lx\n", suffix, (unsigned long)muhtari__crc32(0u, p, n));
}

/* Prints the eight hash lines: the file as given, then headerless. The
   headerless input follows rcheevos's 7800 rule (rcheevos @1433173220a7,
   src/rhash/hash_rom.c): a file longer than 128 bytes whose bytes 1 to 9
   are "ATARI7800" loses its first 128 bytes; any other file is hashed
   whole. */
static void print_all_hashes(const uint8_t *image, size_t n)
{
    size_t skip = n > 128u && memcmp(image + 1, "ATARI7800", 9u) == 0 ? 128u : 0u;
    print_hashes("", image, n);
    print_hashes("_headerless", image + skip, n - skip);
}

/* Prints key and the names of the bits set in bits, separated by commas, or
   "none". */
static void print_bits(const char *key, muhtari__bit_set set, uint32_t bits)
{
    printf("%s ", key);
    if (bits == 0u) {
        puts("none");
        return;
    }
    const char *separator = "";
    for (uint32_t bit = 0; bit < 32u; bit++) {
        if ((bits >> bit) & 1u) {
            printf("%s%s", separator, muhtari__bit_name(set, bit));
            separator = ",";
        }
    }
    putchar('\n');
}

/* Prints the identity lines of an image probe described, in the order
   docs/runner.md gives: the fields, the raw A78 header fields when the image
   has a header, then one line per warning. */
static void print_identity(const muhtari_cart_info *c)
{
    printf("kind %s\n", muhtari__kind_name(c->kind));
    printf("source %s\n", muhtari__source_name(c->source));
    printf("board %s\n", muhtari__board_name(c->board));
    print_bits("board_options", MUHTARI__BITS_BOARD_OPTIONS, c->board_options);
    printf("rom_size %lu\n", (unsigned long)c->rom_size);
    printf("ram_size %lu\n", (unsigned long)c->ram_size);
    print_bits("sound", MUHTARI__BITS_SOUND, c->sound);
    print_bits("devices", MUHTARI__BITS_DEVICES, c->devices);
    printf("port1 %s\n", muhtari__port_name(c->port1));
    printf("port2 %s\n", muhtari__port_name(c->port2));
    printf("region %s\n", muhtari__region_name(c->region));
    if (c->flags & MUHTARI_CART_HEADER) {
        printf("title %s\n", (const char *)c->title);
        printf("a78_version 0x%02lx\n", (unsigned long)c->a78_version);
        printf("a78_rom_size %lu\n", (unsigned long)c->a78_rom_size);
        printf("a78_cart_type 0x%04lx\n", (unsigned long)c->a78_cart_type);
        printf("a78_port1 0x%02lx\n", (unsigned long)c->a78_port1);
        printf("a78_port2 0x%02lx\n", (unsigned long)c->a78_port2);
        printf("a78_tv 0x%02lx\n", (unsigned long)c->a78_tv);
        printf("a78_save 0x%02lx\n", (unsigned long)c->a78_save);
        printf("a78_xm 0x%02lx\n", (unsigned long)c->a78_xm);
        printf("a78_mapper 0x%02lx\n", (unsigned long)c->a78_mapper);
        printf("a78_mapper_options 0x%02lx\n", (unsigned long)c->a78_mapper_options);
        printf("a78_audio 0x%04lx\n", (unsigned long)c->a78_audio);
        printf("a78_irq 0x%02lx\n", (unsigned long)c->a78_irq);
    }
    for (uint32_t bit = 0; bit < 32u; bit++) {
        if ((c->warnings >> bit) & 1u) {
            printf("warning %s\n", muhtari__bit_name(MUHTARI__BITS_WARNINGS, bit));
        }
    }
}

/* Reads the file at path into one new buffer of the image cap plus one
   byte, looping on fread until it returns 0. Nothing seeks in the stream or
   asks its position, so a pipe works and no long overflows; a longer file
   stops one byte past the cap, which the core refuses by size. Returns NULL
   after a message when the file cannot be read; the caller frees the
   buffer. */
static uint8_t *read_image(const char *path, size_t *out_n)
{
    const size_t cap = (size_t)MUHTARI_IMAGE_SIZE_MAX + 1u;
    uint8_t *image = (uint8_t *)malloc(cap);
    if (image == NULL) {
        fprintf(stderr, "muhtari-run: out of memory\n");
        return NULL;
    }
    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        fprintf(stderr, "muhtari-run: %s: cannot open\n", path);
        free(image);
        return NULL;
    }
    size_t n = 0;
    for (;;) {
        size_t got = fread(image + n, 1u, cap - n, f);
        if (got == 0u) {
            break;
        }
        n += got;
    }
    int unreadable = ferror(f) != 0;
    (void)fclose(f);
    if (unreadable) {
        fprintf(stderr, "muhtari-run: %s: cannot read\n", path);
        free(image);
        return NULL;
    }
    *out_n = n;
    return image;
}

/* Identifies the image at path and prints what probe found. Returns the
   exit status. */
static int probe(const char *path)
{
    size_t n = 0;
    uint8_t *image = read_image(path, &n);
    if (image == NULL) {
        return MUHTARI__EXIT_FAILURE;
    }

    muhtari_cart_info info;
    memset(&info, 0, sizeof info);
    info.size = (uint32_t)sizeof info;
    muhtari_status st = muhtari_probe(image, n, &info);
    /* Which lines print follows what probe wrote: everything for OK and an
       unsupported board, the four fields it fills for a 2600 image, only
       the hashes when it wrote nothing, and nothing at all for an image
       too large to read whole. */
    int status = MUHTARI__EXIT_FAILURE;
    if (st == MUHTARI_STATUS_OK || st == MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD) {
        print_identity(&info);
        status = st == MUHTARI_STATUS_OK ? MUHTARI__EXIT_DONE : MUHTARI__EXIT_REFUSED;
    } else if (st == MUHTARI_STATUS_ERR_UNSUPPORTED_2600) {
        printf("kind %s\n", muhtari__kind_name(info.kind));
        printf("source %s\n", muhtari__source_name(info.source));
        printf("rom_size %lu\n", (unsigned long)info.rom_size);
        status = MUHTARI__EXIT_REFUSED;
    }
    if (st != MUHTARI_STATUS_ERR_UNSUPPORTED_SIZE) {
        print_all_hashes(image, n);
    }
    if (st == MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD) {
        fprintf(stderr, "muhtari-run: %s: %s: %s\n", path, muhtari__status_text(st),
                muhtari__board_name(info.board));
    } else if (st != MUHTARI_STATUS_OK) {
        fprintf(stderr, "muhtari-run: %s: %s\n", path, muhtari__status_text(st));
    }
    free(image);
    return status;
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
    const char *probe; /* --probe FILE, the whole command line */
    const char *rom;   /* [--rom] FILE, or NULL for no content */
    const char *movie; /* --movie FILE, or NULL */
    int printed;       /* --help or --version ran; nothing else to do */
} options;

/* A movie read and checked whole: its header and one input per frame. */
typedef struct replay {
    muhtari__movie_header header;
    muhtari_input *frames;
    uint32_t count;
} replay;

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
    /* --probe takes one file and is the whole command line; a second
       option in the file's place is a usage error. */
    if (argc == 3 && strcmp(argv[1], "--probe") == 0 && strncmp(argv[2], "--", 2u) != 0) {
        o->probe = argv[2];
        return MUHTARI__EXIT_DONE;
    }
    /* Every other option takes one value; an argument that is not an
       option is the cartridge FILE, given at most once. */
    for (int i = 1; i < argc; i++) {
        const char *arg = argv[i];
        if (strncmp(arg, "--", 2u) != 0) {
            if (o->rom != NULL) {
                return usage("unexpected argument: give one FILE");
            }
            o->rom = arg;
            continue;
        }
        const char *value = i + 1 < argc ? argv[i + 1] : NULL;
        i++;
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
        } else if (strcmp(arg, "--rom") == 0) {
            if (value == NULL || strncmp(value, "--", 2u) == 0) {
                return usage("--rom needs a FILE");
            }
            if (o->rom != NULL) {
                return usage("unexpected argument: give one FILE");
            }
            o->rom = value;
        } else if (strcmp(arg, "--movie") == 0) {
            if (value == NULL || strncmp(value, "--", 2u) == 0 || o->movie != NULL) {
                return usage("--movie needs one FILE");
            }
            o->movie = value;
        } else if (strcmp(arg, "--probe") == 0) {
            return usage("--probe takes one file and no other option");
        } else if (strcmp(arg, "--help") == 0 || strcmp(arg, "--version") == 0) {
            return usage("--help and --version take no other arguments");
        } else {
            return usage("unknown option");
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

/* Reads the file at path into a new buffer, at most one byte past the
   movie limit, so the reader refuses a longer file by its size without
   this reading it whole. Sequential reads only, as read_image. Returns
   NULL after a message; the caller frees the buffer. */
static uint8_t *read_movie_file(const char *path, size_t *out_n)
{
    const size_t cap = (size_t)MUHTARI__MOVIE_BYTES_MAX + 1u;
    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        fprintf(stderr, "muhtari-run: %s: cannot open\n", path);
        return NULL;
    }
    size_t size = 4096u;
    size_t n = 0;
    uint8_t *bytes = (uint8_t *)malloc(size);
    while (bytes != NULL) {
        size_t got = fread(bytes + n, 1u, size - n, f);
        n += got;
        if (got == 0u || n == cap) {
            break;
        }
        if (n == size) {
            size_t grown = size * 2u < cap ? size * 2u : cap;
            uint8_t *more = (uint8_t *)realloc(bytes, grown);
            if (more == NULL) {
                free(bytes);
                bytes = NULL;
                break;
            }
            bytes = more;
            size = grown;
        }
    }
    int unreadable = ferror(f) != 0;
    (void)fclose(f);
    if (bytes == NULL) {
        fprintf(stderr, "muhtari-run: out of memory\n");
        return NULL;
    }
    if (unreadable) {
        fprintf(stderr, "muhtari-run: %s: cannot read\n", path);
        free(bytes);
        return NULL;
    }
    *out_n = n;
    return bytes;
}

/* Reads and checks the whole movie at path before any frame runs: one
   pass counts the frames, the second fills them. Returns the exit status;
   a malformed movie exits 1 with its line and column. */
static int read_movie(const char *path, replay *mv)
{
    size_t n = 0;
    uint8_t *bytes = read_movie_file(path, &n);
    if (bytes == NULL) {
        return MUHTARI__EXIT_FAILURE;
    }
    muhtari__movie_error err;
    int rc = muhtari__movie_parse(bytes, n, &mv->header, NULL, 0u, &mv->count, &err);
    if (rc == 0) {
        /* At most 1,000,000 frames of 24 bytes, so the size fits size_t. */
        mv->frames = (muhtari_input *)malloc(((size_t)mv->count + 1u) * sizeof *mv->frames);
        if (mv->frames == NULL) {
            fprintf(stderr, "muhtari-run: out of memory\n");
            free(bytes);
            return MUHTARI__EXIT_FAILURE;
        }
        rc = muhtari__movie_parse(bytes, n, &mv->header, mv->frames, mv->count, &mv->count, &err);
    }
    free(bytes);
    if (rc != 0) {
        fprintf(stderr, "muhtari-run: %s:%lu:%lu: %s, expected %s\n", path, (unsigned long)err.line,
                (unsigned long)err.column, err.what, err.expected);
        return MUHTARI__EXIT_FAILURE;
    }
    return MUHTARI__EXIT_DONE;
}

/* Loads the cartridge image at path into m and fast-boots it. A refusal by
   kind, board or region exits 77, a malformed image or any other failure
   1. Returns the exit status. */
static int load(muhtari *m, const char *path)
{
    size_t n = 0;
    uint8_t *image = read_image(path, &n);
    if (image == NULL) {
        return MUHTARI__EXIT_FAILURE;
    }
    muhtari_media media;
    memset(&media, 0, sizeof media);
    media.size = (uint32_t)sizeof media;
    media.image = image;
    media.image_size = n;
    muhtari_status st = muhtari_load(m, &media);
    free(image);
    if (st == MUHTARI_STATUS_OK) {
        return MUHTARI__EXIT_DONE;
    }
    fprintf(stderr, "muhtari-run: %s: %s\n", path, muhtari__status_text(st));
    if (st == MUHTARI_STATUS_ERR_UNSUPPORTED_2600 || st == MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD ||
        st == MUHTARI_STATUS_ERR_UNSUPPORTED_REGION) {
        return MUHTARI__EXIT_REFUSED;
    }
    return MUHTARI__EXIT_FAILURE;
}

/* Runs the frames into heap buffers (a 77,440-byte local would overflow a
   64 KiB stack, as wasm32's is). Returns the exit status. */
static int run(const options *opt, const replay *mv)
{
    uint8_t *video = (uint8_t *)malloc((size_t)WIDTH * HEIGHT);
    int16_t *audio = (int16_t *)malloc(AUDIO_CAPACITY * sizeof *audio);
    uint8_t *row = (uint8_t *)malloc(3u * WIDTH);
    /* One microsecond sample per frame for --bench, on the heap: 300,000
       frames take 1.2 MB. A count whose buffer size does not fit in size_t
       is out of memory, not a wrapped, too-small allocation. */
    size_t sample_bytes = opt->bench ? muhtari__bench_bytes(opt->frames) : 0u;
    uint32_t *samples = sample_bytes != 0u ? (uint32_t *)malloc(sample_bytes) : NULL;
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

    int status = opt->rom != NULL ? load(m, opt->rom) : MUHTARI__EXIT_DONE;
    uint32_t next_hash = 0;
    uint32_t timed = 0;
    uint64_t total_ns = 0;
    /* A 64-bit count, so the loop ends when --frames is 4294967295. */
    for (uint64_t n = 1; status == MUHTARI__EXIT_DONE && n <= opt->frames; n++) {
        uint32_t f = (uint32_t)n;
        muhtari_frame io;
        memset(&io, 0, sizeof io);
        io.size = (uint32_t)sizeof io;
        io.video = video;
        io.video_pitch = WIDTH;
        io.audio = audio;
        io.audio_capacity = AUDIO_CAPACITY;
        /* Movie line f is the input set before frame f: the core latches
           it at the VBLANK start that opens the frame (docs/runner.md,
           Movies). */
        if (mv != NULL && f <= mv->count &&
            muhtari_set_input(m, &mv->frames[f - 1u]) != MUHTARI_STATUS_OK) {
            fprintf(stderr, "muhtari-run: muhtari_set_input failed\n");
            status = MUHTARI__EXIT_FAILURE;
            break;
        }
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

    if (status == MUHTARI__EXIT_DONE && mv != NULL) {
        muhtari_counters c;
        memset(&c, 0, sizeof c);
        c.size = (uint32_t)sizeof c;
        if (muhtari_get_counters(m, &c) != MUHTARI_STATUS_OK) {
            fprintf(stderr, "muhtari-run: muhtari_get_counters failed\n");
            status = MUHTARI__EXIT_FAILURE;
        } else {
            printf("lag_frames %llu\n", (unsigned long long)c.lag_frames);
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
    if (status == MUHTARI__EXIT_DONE && opt.probe != NULL) {
        status = probe(opt.probe);
    } else if (status == MUHTARI__EXIT_DONE && !opt.printed) {
        replay mv;
        memset(&mv, 0, sizeof mv);
        if (opt.movie != NULL) {
            status = read_movie(opt.movie, &mv);
        }
        if (status == MUHTARI__EXIT_DONE) {
            muhtari__stop_install();
            status = run(&opt, opt.movie != NULL ? &mv : NULL);
        }
        free(mv.frames);
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
