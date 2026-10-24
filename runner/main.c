/* muhtari-run: the headless runner.
 *
 *   muhtari-run --frames N [--hash-frame N]... [--dump-frame N:FILE]... [[--rom] FILE]
 *   muhtari-run --bench N [--hash-frame N]... [--dump-frame N:FILE]... [[--rom] FILE]
 *   muhtari-run --frames N --boot bios --bios FILE [--hash-frame N]... [--rom] FILE
 *   muhtari-run --probe FILE
 *   muhtari-run --help | --version
 *
 * Runs N frames of one instance, with the cartridge FILE loaded through
 * muhtari_load and fast-booted, or booted through the BIOS with --boot bios
 * --bios FILE, or with no content when there is no FILE; --frames or
 * --bench is required. For each --hash-frame N it prints, after frame N has run:
 *
 *   frame <N> sha256 <64 lowercase hex digits>
 *
 * The hash is SHA-256 over the 320x242 native colour bytes, row-major, top
 * row first. For each --dump-frame N:FILE it writes frame N to FILE as a
 * 320x242 RGB PNG in the warm NTSC palette. --bench N also times each
 * frame and prints the report described in bench.h. With --boot bios it
 * prints the BIOS's SHA-256 to standard error and, after the frames,
 * "handoff_frame K", the frame of the first cartridge opcode fetch.
 * SIGINT or SIGTERM ends the run after the frame in progress; output is
 * flushed. Required save failure returns 1; earlier output failure keeps its status.
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
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <direct.h>
#include <io.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "bench.h"
#include "clock.h"
#include "crc32.h"
#include "exit_status.h"
#include "internal.h"
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
    "       muhtari-run --movie FILE [--frames N | --bench N] [--hash-frame N]...\n"
    "                   [--dump-frame N:FILE]... [[--rom] FILE]\n"
    "       muhtari-run --frames N --boot bios --bios FILE [--hash-frame N]... [--rom] FILE\n"
    "       muhtari-run --probe FILE\n"
    "       muhtari-run --help | --version\n"
    "  --frames N           run N frames (N >= 1)\n"
    "  --bench N            run N frames and report the time per frame\n"
    "  --hash-audio         print whole-run raw transition and mono PCM SHA-256 hashes\n"
    "  --diagnostic         report the fixed 256-byte diagnostic mailbox after --frames\n"
    "  --hash-frame N       print the SHA-256 of frame N (1 <= N <= --frames)\n"
    "  --dump-frame N:FILE  write frame N to FILE as a PNG (1 <= N <= --frames)\n"
    "  [--rom] FILE         load the cartridge image FILE; without it no content runs\n"
    "  --movie FILE         replay the muhtari-movie 1 FILE, one line per frame;\n"
    "                       --frames defaults to its frame count\n"
    "  --port1 D, --port2 D auto, proline, joystick, none, savekey or atarivox\n"
    "  --region R           the region: auto or ntsc\n"
    "  --boot B             how the cartridge starts: fast (the default) or bios\n"
    "  --bios FILE          the BIOS image --boot bios runs\n"
    "  --save-dir DIR      restore and save raw battery RAM in an existing directory\n"
    "  --save-interval F   dirty save cadence in completed frames (default 60)\n"
    "  --hsc M             high score cartridge: auto, on or off\n"
    "  --hsc-rom FILE      optional exact 4096-byte HSC firmware\n"
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
    if ((c->flags & MUHTARI_CART_ACTIVISION_HALF_SWAPPED) != 0u) {
        printf("flag %s\n", muhtari__bit_name(MUHTARI__BITS_FLAGS, 2u));
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
    int hash_audio;
    int diagnostic;
    uint32_t *hash_frames;
    uint32_t hash_count;
    uint32_t *dump_frames;
    const char **dump_paths;
    uint32_t dump_count;
    const char *probe; /* --probe FILE, the whole command line */
    const char *rom;   /* [--rom] FILE, or NULL for no content */
    const char *movie; /* --movie FILE, or NULL */
    uint32_t port[2];  /* --port1, --port2: 0 for auto, else MUHTARI_PORT_* */
    uint32_t region;   /* --region: 0 for auto, else MUHTARI_REGION_NTSC */
    int has_boot;      /* --boot given */
    uint32_t boot;     /* --boot: MUHTARI__MOVIE_BOOT_*, then the boot that runs */
    const char *bios;  /* --bios FILE, or NULL */
    const char *save_dir;
    uint32_t save_interval;
    uint32_t hsc_mode;
    int has_hsc;
    const char *hsc_rom;
    int printed; /* --help or --version ran; nothing else to do */
} options;

/* The values --port1 and --port2 take, and the movie's port keys. */
static const struct {
    const char *name;
    uint32_t device;
} port_values[] = {
    {"auto", 0u},
    {"proline", MUHTARI_PORT_PROLINE},
    {"joystick", MUHTARI_PORT_JOYSTICK},
    {"none", MUHTARI_PORT_UNPLUGGED},
    {"savekey", MUHTARI_PORT_SAVEKEY},
    {"atarivox", MUHTARI_PORT_ATARIVOX},
};

static const char *port_name(uint32_t device)
{
    for (size_t i = 0; i < sizeof port_values / sizeof port_values[0]; i++) {
        if (port_values[i].device == device) {
            return port_values[i].name;
        }
    }
    return "?";
}

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

/* Returns 0 when the options are spelled right, 2 after a usage message, 1
   when out of memory. check_options checks them against each other once a
   movie, if any, has been read. */
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
        } else if (strcmp(arg, "--diagnostic") == 0) {
            if (o->diagnostic) {
                return usage("duplicate --diagnostic");
            }
            o->diagnostic = 1;
            i--;
        } else if (strcmp(arg, "--hash-audio") == 0) {
            if (o->hash_audio)
                return usage("duplicate --hash-audio");
            o->hash_audio = 1;
            i--;
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
        } else if (strcmp(arg, "--port1") == 0 || strcmp(arg, "--port2") == 0) {
            int p = arg[6] - '1';
            size_t v = 0;
            while (v < sizeof port_values / sizeof port_values[0] &&
                   (value == NULL || strcmp(value, port_values[v].name) != 0)) {
                v++;
            }
            if (v == sizeof port_values / sizeof port_values[0]) {
                return usage(
                    p == 0 ? "--port1 needs auto, proline, joystick, none, savekey or atarivox"
                           : "--port2 needs auto, proline, joystick, none, savekey or atarivox");
            }
            o->port[p] = port_values[v].device;
        } else if (strcmp(arg, "--region") == 0) {
            /* PAL is refused everywhere until the core emulates it. */
            if (value != NULL && strcmp(value, "pal") == 0) {
                return usage("--region pal is not supported: give auto or ntsc");
            }
            if (value == NULL || (strcmp(value, "auto") != 0 && strcmp(value, "ntsc") != 0)) {
                return usage("--region needs auto or ntsc");
            }
            o->region = strcmp(value, "ntsc") == 0 ? (uint32_t)MUHTARI_REGION_NTSC : 0u;
        } else if (strcmp(arg, "--boot") == 0) {
            if (value == NULL || (strcmp(value, "fast") != 0 && strcmp(value, "bios") != 0)) {
                return usage("--boot needs fast or bios");
            }
            o->has_boot = 1;
            o->boot = strcmp(value, "bios") == 0 ? (uint32_t)MUHTARI__MOVIE_BOOT_BIOS
                                                 : (uint32_t)MUHTARI__MOVIE_BOOT_FAST;
        } else if (strcmp(arg, "--bios") == 0) {
            if (value == NULL || strncmp(value, "--", 2u) == 0 || o->bios != NULL) {
                return usage("--bios needs one FILE");
            }
            o->bios = value;
        } else if (strcmp(arg, "--save-interval") == 0) {
            if (o->save_interval != 0u ||
                !parse_count(value, value == NULL ? 0u : strlen(value), &o->save_interval))
                return usage("--save-interval needs one whole number of 1 or more");
        } else if (strcmp(arg, "--save-dir") == 0 || strcmp(arg, "--hsc-rom") == 0) {
            const char **out = strcmp(arg, "--save-dir") == 0 ? &o->save_dir : &o->hsc_rom;
            if (value == NULL || value[0] == '\0' || strncmp(value, "--", 2u) == 0 ||
                *out != NULL) {
                return usage("--save-dir and --hsc-rom each need one path");
            }
            *out = value;
        } else if (strcmp(arg, "--hsc") == 0) {
            if (o->has_hsc || value == NULL ||
                (strcmp(value, "auto") != 0 && strcmp(value, "on") != 0 &&
                 strcmp(value, "off") != 0)) {
                return usage("--hsc needs auto, on or off once");
            }
            o->has_hsc = 1;
            o->hsc_mode = strcmp(value, "on") == 0    ? MUHTARI_HSC_ON
                          : strcmp(value, "off") == 0 ? MUHTARI_HSC_OFF
                                                      : MUHTARI_HSC_AUTO;
        } else if (strcmp(arg, "--probe") == 0) {
            return usage("--probe takes one file and no other option");
        } else if (strcmp(arg, "--help") == 0 || strcmp(arg, "--version") == 0) {
            return usage("--help and --version take no other arguments");
        } else {
            return usage("unknown option");
        }
    }
    return MUHTARI__EXIT_DONE;
}

/* Checks the options against each other and against the movie header h,
   if any: the header wins, so an option that names another device or
   region is a usage error, and --frames defaults to the movie's frame
   count. Returns 0, or 2 after a usage message. The hash frames come back
   sorted with duplicates removed. */
static int check_options(options *o, const muhtari__movie_header *h, uint32_t movie_frames)
{
    char why[128];
    if (o->save_interval != 0u && o->save_dir == NULL)
        return usage("--save-interval needs --save-dir");
    if (o->save_interval == 0u)
        o->save_interval = 60u;
    if ((o->save_dir != NULL || o->hsc_rom != NULL || o->has_hsc) && o->rom == NULL) {
        return usage("save and HSC options need a cartridge FILE");
    }
    if (o->diagnostic && (o->rom == NULL || o->frames == 0u || o->bench)) {
        return usage("--diagnostic needs content and explicit --frames N");
    }
    if (h != NULL) {
        for (int p = 0; p < 2; p++) {
            if (o->port[p] != 0u && o->port[p] != h->device[p]) {
                (void)snprintf(why, sizeof why, "--port%d %s conflicts with the movie's port%d %s",
                               p + 1, port_name(o->port[p]), p + 1, port_name(h->device[p]));
                return usage(why);
            }
        }
        if (o->region != 0u && h->has_region && o->region != h->region) {
            return usage("--region conflicts with the movie's region");
        }
        if (h->has_boot) {
            static const char *const boots[] = {"fast", "bios"};
            if (o->has_boot && o->boot != h->boot) {
                (void)snprintf(why, sizeof why, "--boot %s conflicts with the movie's boot %s",
                               boots[o->boot], boots[h->boot]);
                return usage(why);
            }
            if (h->boot == MUHTARI__MOVIE_BOOT_BIOS && o->bios == NULL) {
                return usage("the movie's boot bios needs --bios FILE");
            }
            o->boot = h->boot;
        }
        if (o->frames == 0u) {
            if (movie_frames == 0u) {
                return usage("the movie has no frames: give --frames N");
            }
            o->frames = movie_frames;
        }
    }
    if (o->frames == 0u) {
        return usage("--frames N, --bench N or --movie FILE is required");
    }
    if (o->boot == MUHTARI__MOVIE_BOOT_BIOS && o->bios == NULL) {
        return usage("--boot bios needs --bios FILE");
    }
    if (o->boot != MUHTARI__MOVIE_BOOT_BIOS && o->bios != NULL) {
        return usage("--bios needs --boot bios");
    }
    if (o->bios != NULL && o->rom == NULL) {
        return usage("--boot bios needs a cartridge FILE");
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

/* Reads the BIOS image at path into a new buffer and prints its SHA-256
   to standard error as "bios_sha256 <hex>". When the movie header h names
   a bios-sha256, the file's SHA-256 must match it. Returns NULL after a
   message; the caller frees the buffer. */
static uint8_t *read_bios(const char *path, const char *movie_path, const muhtari__movie_header *h,
                          size_t *out_n)
{
    uint8_t *bios = read_image(path, out_n);
    if (bios == NULL) {
        return NULL;
    }
    muhtari__sha256 s;
    uint8_t digest[32];
    char hex[65];
    muhtari__sha256_init(&s);
    muhtari__sha256_update(&s, bios, *out_n);
    muhtari__sha256_final(&s, digest);
    to_hex(digest, 32u, hex);
    fprintf(stderr, "bios_sha256 %s\n", hex);
    if (h != NULL && h->has_bios_sha256 && memcmp(digest, h->bios_sha256, sizeof digest) != 0) {
        char want[65];
        to_hex(h->bios_sha256, 32u, want);
        fprintf(stderr, "muhtari-run: %s: bios-sha256 %s does not match %s's SHA-256 %s\n",
                movie_path, want, path, hex);
        free(bios);
        return NULL;
    }
    return bios;
}

/* Runner-owned host files; the core only owns the stable raw span. */
typedef struct save_context {
    char *path;
    uint8_t *data;
    size_t bytes;
    uint64_t persisted_generation;
    int armed;
    int failed;
    uint64_t completed_frames;
} save_context;

static int save_directory(const char *path)
{
#ifdef _WIN32
    struct _stat64 st;
    return _stat64(path, &st) == 0 && (st.st_mode & _S_IFMT) == _S_IFDIR && _access(path, 6) == 0;
#else
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode) && access(path, R_OK | W_OK | X_OK) == 0;
#endif
}

static FILE *save_read_open(const char *path)
{
#ifdef _WIN32
    struct _stat64 st;
    if (_stat64(path, &st) != 0)
        return NULL;
    if ((st.st_mode & _S_IFMT) != _S_IFREG) {
        errno = EINVAL;
        return NULL;
    }
    int fd = _open(path, _O_RDONLY | _O_BINARY);
    if (fd < 0)
        return NULL;
    FILE *f = _fdopen(fd, "rb");
    if (f == NULL) {
        int error = errno;
        (void)_close(fd);
        errno = error;
    }
    return f;
#else
    /* Nonblocking open plus descriptor stat refuses FIFOs even after a path race. */
    int fd = open(path, O_RDONLY | O_NONBLOCK);
    if (fd < 0)
        return NULL;
    struct stat st;
    if (fstat(fd, &st) != 0 || !S_ISREG(st.st_mode)) {
        (void)close(fd);
        errno = EINVAL;
        return NULL;
    }
    FILE *f = fdopen(fd, "rb");
    if (f == NULL) {
        int error = errno;
        (void)close(fd);
        errno = error;
    }
    return f;
#endif
}

static int save_import(save_context *save, muhtari *m, const options *opt)
{
    if (opt->save_dir == NULL)
        return 0;
    if (!save_directory(opt->save_dir)) {
        fprintf(stderr, "muhtari-run: save directory is not accessible\n");
        return MUHTARI__EXIT_FAILURE;
    }
    muhtari_battery_info battery = {0};
    battery.size = (uint32_t)sizeof battery;
    if (muhtari_get_battery(m, &battery) != MUHTARI_STATUS_OK)
        return MUHTARI__EXIT_FAILURE;
    if (battery.bytes == 0u)
        return 0;
    const char *base = opt->rom;
    for (const char *p = base; *p != '\0'; p++) {
        if (*p == '/'
#ifdef _WIN32
            || *p == '\\'
#endif
        )
            base = p + 1;
    }
    const char *dot = strrchr(base, '.');
    size_t stem = dot != NULL && dot != base ? (size_t)(dot - base) : strlen(base);
    size_t dir = strlen(opt->save_dir);
    if (stem == 0u || dir > SIZE_MAX - 6u || stem > SIZE_MAX - dir - 6u)
        return MUHTARI__EXIT_FAILURE;
    save->path = (char *)malloc(dir + stem + 6u);
    uint8_t *staging = (uint8_t *)malloc(battery.bytes + 1u);
    if (save->path == NULL || staging == NULL) {
        free(staging);
        return MUHTARI__EXIT_FAILURE;
    }
    memcpy(save->path, opt->save_dir, dir);
    save->path[dir] = '/';
    memcpy(save->path + dir + 1u, base, stem);
    memcpy(save->path + dir + stem + 1u, ".sav", 5u);
    memset(staging, 0xFF, battery.bytes + 1u);
    errno = 0;
    FILE *f = save_read_open(save->path);
    int status = 0;
    if (f == NULL) {
        if (errno != ENOENT)
            status = MUHTARI__EXIT_FAILURE;
    } else {
        size_t n = 0u;
        while (n < battery.bytes + 1u) {
            size_t got = fread(staging + n, 1u, battery.bytes + 1u - n, f);
            n += got;
            if (got == 0u)
                break;
        }
        int bad = ferror(f) != 0;
        if (fclose(f) != 0)
            bad = 1;
        status = bad ? MUHTARI__EXIT_FAILURE : n > battery.bytes ? MUHTARI__EXIT_SAVE_SIZE : 0;
    }
    if (status == 0) {
        memcpy(battery.data, staging, battery.bytes);
        save->data = battery.data;
        save->bytes = battery.bytes;
        save->persisted_generation = battery.generation;
        save->armed = 1;
    } else {
        fprintf(stderr, "muhtari-run: %s: %s\n", save->path,
                status == MUHTARI__EXIT_SAVE_SIZE ? "save exceeds battery size"
                                                  : "cannot import save");
    }
    free(staging);
    return status;
}

static int save_replace(const char *temporary, const char *destination)
{
#ifdef _WIN32
    return MoveFileExA(temporary, destination, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)
               ? 0
               : -1;
#else
    return rename(temporary, destination);
#endif
}

static int save_flush(save_context *save, muhtari *m)
{
    if (!save->armed || save->failed)
        return save->failed ? MUHTARI__EXIT_FAILURE : 0;
    muhtari_battery_info battery = {0};
    battery.size = (uint32_t)sizeof battery;
    if (muhtari_get_battery(m, &battery) != MUHTARI_STATUS_OK) {
        save->failed = 1;
        return MUHTARI__EXIT_FAILURE;
    }
    if (battery.generation == save->persisted_generation)
        return 0;
    size_t len = strlen(save->path);
    char *temporary = len <= SIZE_MAX - 48u ? (char *)malloc(len + 48u) : NULL;
    FILE *f = NULL;
    int owned = 0;
    for (unsigned attempt = 0; temporary != NULL && attempt < 32u; attempt++) {
#ifdef _WIN32
        unsigned long pid = (unsigned long)GetCurrentProcessId();
#elif defined(__wasi__)
        /* WASI has no process IDs; exclusive creation still owns each candidate. */
        unsigned long pid = 0;
#else
        unsigned long pid = (unsigned long)getpid();
#endif
        (void)snprintf(temporary, len + 48u, "%s.tmp.%lu.%u", save->path, pid, attempt);
#ifdef _WIN32
        int fd = _open(temporary, _O_CREAT | _O_EXCL | _O_WRONLY | _O_BINARY, _S_IREAD | _S_IWRITE);
#else
        int fd = open(temporary, O_CREAT | O_EXCL | O_WRONLY, 0600);
#endif
        if (fd < 0) {
            if (errno == EEXIST)
                continue;
            break;
        }
        owned = 1;
#ifdef _WIN32
        f = _fdopen(fd, "wb");
        if (f == NULL)
            (void)_close(fd);
#else
        f = fdopen(fd, "wb");
        if (f == NULL)
            (void)close(fd);
#endif
        break;
    }
    int bad = f == NULL;
    if (f != NULL) {
        if (fwrite(save->data, 1u, save->bytes, f) != save->bytes || fflush(f) != 0)
            bad = 1;
        if (!bad) {
#ifdef _WIN32
            bad = _commit(_fileno(f)) != 0;
#else
            bad = fsync(fileno(f)) != 0;
#endif
        }
        if (fclose(f) != 0)
            bad = 1;
    }
    if (!bad && save_replace(temporary, save->path) != 0)
        bad = 1;
    if (owned && bad)
        (void)remove(temporary);
    free(temporary);
    if (bad) {
        save->failed = 1;
        fprintf(stderr, "muhtari-run: %s: cannot commit save\n", save->path);
        return MUHTARI__EXIT_FAILURE;
    }
    save->persisted_generation = battery.generation;
    return 0;
}

/* Loads the cartridge image at path into m, in region (0 for the
   cartridge's), fast-booted, or booted through the BIOS image at
   bios_path when that is not NULL. When the movie header h names a
   rom-sha256, the image's headerless SHA-256, the --probe
   sha256_headerless, must match it. A refusal by kind, board or region
   exits 77, a mismatch, a malformed image, an invalid BIOS or any other
   failure 1. Returns the exit status. */
static int load(muhtari *m, const char *path, uint32_t region, const char *movie_path,
                const muhtari__movie_header *h, const char *bios_path, const options *opt)
{
    size_t n = 0;
    uint8_t *image = read_image(path, &n);
    if (image == NULL) {
        return MUHTARI__EXIT_FAILURE;
    }
    if (h != NULL && h->has_rom_sha256) {
        size_t skip = n > 128u && memcmp(image + 1, "ATARI7800", 9u) == 0 ? 128u : 0u;
        muhtari__sha256 s;
        uint8_t digest[32];
        muhtari__sha256_init(&s);
        muhtari__sha256_update(&s, image + skip, n - skip);
        muhtari__sha256_final(&s, digest);
        if (memcmp(digest, h->rom_sha256, sizeof digest) != 0) {
            char want[65];
            char got[65];
            to_hex(h->rom_sha256, 32u, want);
            to_hex(digest, 32u, got);
            fprintf(stderr,
                    "muhtari-run: %s: rom-sha256 %s does not match %s's headerless SHA-256 %s\n",
                    movie_path, want, path, got);
            free(image);
            return MUHTARI__EXIT_FAILURE;
        }
    }
    size_t bios_n = 0;
    uint8_t *bios = NULL;
    if (bios_path != NULL) {
        bios = read_bios(bios_path, movie_path, h, &bios_n);
        if (bios == NULL) {
            free(image);
            return MUHTARI__EXIT_FAILURE;
        }
    }
    muhtari_media media;
    memset(&media, 0, sizeof media);
    media.size = (uint32_t)sizeof media;
    media.region = region;
    media.image = image;
    media.image_size = n;
    media.bios = bios;
    media.bios_size = bios_n;
    media.hsc_mode = opt->hsc_mode;
    media.port[0] = h != NULL ? h->device[0] : opt->port[0];
    media.port[1] = h != NULL ? h->device[1] : opt->port[1];
    uint8_t *firmware = NULL;
    if (opt->hsc_rom != NULL) {
        firmware = (uint8_t *)malloc(4097u);
        FILE *f = save_read_open(opt->hsc_rom);
        int bad = firmware == NULL || f == NULL;
        size_t used = 0u;
        if (f != NULL) {
            while (firmware != NULL && used < 4097u) {
                size_t got = fread(firmware + used, 1u, 4097u - used, f);
                used += got;
                if (got == 0u)
                    break;
            }
            if (ferror(f) != 0 || used != 4096u)
                bad = 1;
            if (fclose(f) != 0)
                bad = 1;
        }
        if (bad) {
            fprintf(stderr, "muhtari-run: %s: HSC firmware must be a readable 4096-byte file\n",
                    opt->hsc_rom);
            free(firmware);
            free(image);
            free(bios);
            return MUHTARI__EXIT_FAILURE;
        }
        media.hsc_rom = firmware;
        media.hsc_rom_size = used;
    }
    muhtari_status st = muhtari_load(m, &media);
    free(image);
    free(bios);
    free(firmware);
    if (st == MUHTARI_STATUS_OK) {
        muhtari_battery_info b = {0};
        b.size = (uint32_t)sizeof b;
        if (muhtari_get_battery(m, &b) != MUHTARI_STATUS_OK)
            return MUHTARI__EXIT_FAILURE;
        if (b.hsc_attached) {
            if (b.firmware_flags & MUHTARI_HSC_FIRMWARE_STUB) {
                fprintf(stderr, "muhtari-run: HSC signature-only stub; no firmware routines\n");
            } else {
                char hex[41];
                to_hex(b.hsc_sha1, 20u, hex);
                fprintf(stderr, "hsc_sha1 %s\n", hex);
                if (b.firmware_flags & MUHTARI_HSC_FIRMWARE_WARN_UNKNOWN)
                    fprintf(stderr, "muhtari-run: unknown HSC firmware identity\n");
                if (b.firmware_flags & MUHTARI_HSC_FIRMWARE_WARN_SIGNATURE)
                    fprintf(stderr, "muhtari-run: HSC firmware lacks C6/FE signatures\n");
            }
        }
        return MUHTARI__EXIT_DONE;
    }
    fprintf(stderr, "muhtari-run: %s: %s\n",
            st == MUHTARI_STATUS_ERR_INVALID_BIOS ? bios_path : path, muhtari__status_text(st));
    if (st == MUHTARI_STATUS_ERR_UNSUPPORTED_2600 || st == MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD ||
        st == MUHTARI_STATUS_ERR_UNSUPPORTED_REGION) {
        return MUHTARI__EXIT_REFUSED;
    }
    return MUHTARI__EXIT_FAILURE;
}

/* Fills *in with frame f's input and returns 1, or returns 0 when the run
   sets no input. Movie line f is the input set before frame f; the core
   latches it at the VBLANK start that opens the frame (docs/runner.md,
   Movies). Past the movie's end every button is released and the last
   line's difficulty switches are kept. Without a movie, --port1 and
   --port2 set the devices. */
static int frame_input(const options *opt, const replay *mv, uint32_t f, muhtari_input *in)
{
    if (mv != NULL && f <= mv->count) {
        *in = mv->frames[f - 1u];
        return 1;
    }
    memset(in, 0, sizeof *in);
    in->size = (uint32_t)sizeof *in;
    if (mv != NULL) {
        in->device[0] = mv->header.device[0];
        in->device[1] = mv->header.device[1];
        if (mv->count > 0u) {
            in->switches = mv->frames[mv->count - 1u].switches &
                           (MUHTARI_SWITCH_LEFT_DIFFICULTY_A | MUHTARI_SWITCH_RIGHT_DIFFICULTY_A);
        }
        return 1;
    }
    in->device[0] = opt->port[0];
    in->device[1] = opt->port[1];
    return opt->port[0] != 0u || opt->port[1] != 0u;
}

/* Canonical audio evidence: explicit little-endian fields, no host structs. */
static void audio_raw(void *user, uint64_t tick, uint8_t source, uint16_t raw)
{
    uint8_t bytes[11];
    for (unsigned i = 0u; i < 8u; i++)
        bytes[i] = (uint8_t)(tick >> (i * 8u));
    bytes[8] = source;
    bytes[9] = (uint8_t)raw;
    bytes[10] = (uint8_t)(raw >> 8);
    muhtari__sha256_update(user, bytes, sizeof bytes);
}
static void audio_pcm(muhtari__sha256 *hash, const int16_t *samples, uint32_t count)
{
    for (uint32_t i = 0u; i < count; i++) {
        uint16_t bits = (uint16_t)samples[i];
        uint8_t bytes[2] = {(uint8_t)bits, (uint8_t)(bits >> 8)};
        muhtari__sha256_update(hash, bytes, sizeof bytes);
    }
}
static void audio_digest(const char *key, muhtari__sha256 *hash)
{
    uint8_t bytes[32];
    char hex[65];
    muhtari__sha256_final(hash, bytes);
    muhtari__sha256_hex(bytes, hex);
    printf("%s %s\n", key, hex);
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

    const muhtari__movie_header *h = mv != NULL ? &mv->header : NULL;
    uint32_t region = h != NULL && h->has_region ? h->region : opt->region;
    int status = MUHTARI__EXIT_DONE;
    save_context save = {0};
    muhtari__sha256 raw_hash, pcm_hash, tentative;
    muhtari__sha256_init(&raw_hash);
    muhtari__sha256_init(&pcm_hash);
    tentative = raw_hash;
    if (status == 0 && opt->hash_audio && muhtari__synth_observe(m, audio_raw, &tentative) != 0) {
        fprintf(stderr, "muhtari-run: audio observation allocation failed\n");
        status = MUHTARI__EXIT_FAILURE;
    }
    if (status == 0 && opt->rom != NULL) {
        status = load(m, opt->rom, region, opt->movie, h, opt->bios, opt);
    } else if (h != NULL && h->has_rom_sha256) {
        fprintf(stderr, "muhtari-run: %s: rom-sha256 names a cartridge, and none was given\n",
                opt->movie);
        status = MUHTARI__EXIT_FAILURE;
    }
    if (status == 0 && opt->rom != NULL)
        status = save_import(&save, m, opt);
    uint64_t audio_frames = 0u, audio_samples = 0u;
    uint32_t next_hash = 0;
    uint32_t timed = 0;
    uint64_t total_ns = 0;
    uint32_t save_interval = opt->save_interval != 0u ? opt->save_interval : 60u;
    /* A 64-bit count, so the loop ends when --frames is 4294967295. */
    for (uint64_t n = 1;
         status == MUHTARI__EXIT_DONE && n <= opt->frames && !muhtari__stop_requested(); n++) {
        uint32_t f = (uint32_t)n;
        muhtari_frame io;
        memset(&io, 0, sizeof io);
        io.size = (uint32_t)sizeof io;
        io.video = video;
        io.video_pitch = WIDTH;
        io.audio = audio;
        io.audio_capacity = AUDIO_CAPACITY;
        muhtari_input in;
        if (frame_input(opt, mv, f, &in) && muhtari_set_input(m, &in) != MUHTARI_STATUS_OK) {
            fprintf(stderr, "muhtari-run: muhtari_set_input failed\n");
            status = MUHTARI__EXIT_FAILURE;
            break;
        }
        uint64_t start_ns = opt->bench ? muhtari__clock_ns() : 0u;
        tentative = raw_hash;
        st = muhtari_run_frame(m, &io);
        if (opt->bench) {
            uint64_t ns = muhtari__clock_ns() - start_ns;
            samples[timed++] = muhtari__bench_us(ns);
            total_ns += ns;
        }
        if (st == MUHTARI_STATUS_ERR_BIOS_REJECTED_CART) {
            save.armed = 0;
            fprintf(stderr, "muhtari-run: %s: %s\n", opt->rom, muhtari__status_text(st));
            status = MUHTARI__EXIT_REFUSED;
            break;
        }
        if (st != MUHTARI_STATUS_OK) {
            fprintf(stderr, "muhtari-run: muhtari_run_frame failed with status %lu\n",
                    (unsigned long)st);
            status = MUHTARI__EXIT_FAILURE;
            break;
        }
        save.completed_frames++;
        if (opt->hash_audio) {
            raw_hash = tentative;
            audio_pcm(&pcm_hash, audio, io.audio_count);
            audio_frames++;
            audio_samples += io.audio_count;
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
        /* Consume before I/O so a request arriving during a save remains pending. */
        int requested = muhtari__flush_requested();
        if ((requested || save.completed_frames % save_interval == 0u) &&
            save_flush(&save, m) != 0 && status == 0)
            status = MUHTARI__EXIT_FAILURE;
        /* A stop request ends the run only between whole frames, after the
           frame's outputs are written. */
        if (status != 0 || muhtari__stop_requested()) {
            break;
        }
    }

    /* The hand-off frame is read through an internal call, not the public
       API: the public header carries only what an embedder needs, and K is
       a test and diagnostic observable. The runner is built in this tree
       against the static core, and already includes src/ for SHA-1. */
    if (status == MUHTARI__EXIT_DONE && opt->bios != NULL) {
        uint64_t k = muhtari__boot_handoff_frame(m);
        if (k == 0u) {
            puts("handoff_frame none");
        } else {
            printf("handoff_frame %llu\n", (unsigned long long)k);
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

    if (status == 0 && opt->hash_audio) {
        muhtari_versions v = {0};
        v.size = sizeof v;
        (void)muhtari_get_versions(&v);
        audio_digest("audio_transitions_sha256", &raw_hash);
        audio_digest("audio_pcm_sha256", &pcm_hash);
        printf("audio_frames %llu\naudio_samples %llu\naudio_rate "
               "48000/1\naudio_behaviour_revision %u\naudio_synthesis exact-q23-16x64-dc20\n",
               (unsigned long long)audio_frames, (unsigned long long)audio_samples,
               v.behaviour_revision);
    }
    if (status == MUHTARI__EXIT_DONE && opt->diagnostic) {
        muhtari_span span = {0};
        char hex[513];
        span.size = (uint32_t)sizeof span;
        if (muhtari_get_memory(m, MUHTARI_MEMORY_SYSTEM_RAM, &span) != MUHTARI_STATUS_OK ||
            span.data == NULL || span.bytes < 0xB00u) {
            fprintf(stderr, "muhtari-run: cannot read diagnostic mailbox\n");
            status = MUHTARI__EXIT_FAILURE;
        } else {
            to_hex((const uint8_t *)span.data + 0xA00u, 256u, hex);
            if (printf("diagnostic %s\n", hex) < 0) {
                status = MUHTARI__EXIT_FAILURE;
            }
        }
    }
    if ((fflush(stdout) != 0 || ferror(stdout) != 0) && status == 0)
        status = MUHTARI__EXIT_FAILURE;
    if (save.completed_frames != 0u && save_flush(&save, m) != 0 && status == 0)
        status = MUHTARI__EXIT_FAILURE;
    free(save.path);
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
            status = check_options(&opt, opt.movie != NULL ? &mv.header : NULL, mv.count);
        }
        if (status == MUHTARI__EXIT_DONE && mv.header.has_behaviour) {
            /* A movie from another behaviour revision may still replay
               the same; it is warned about once, not refused. */
            muhtari_versions v;
            memset(&v, 0, sizeof v);
            v.size = (uint32_t)sizeof v;
            (void)muhtari_get_versions(&v);
            if (mv.header.behaviour != v.behaviour_revision) {
                fprintf(stderr,
                        "muhtari-run: %s: behaviour %lu, this core's is %lu; replaying anyway\n",
                        opt.movie, (unsigned long)mv.header.behaviour,
                        (unsigned long)v.behaviour_revision);
            }
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
