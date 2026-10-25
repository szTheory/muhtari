/* The runner's bench statistics, stop flag and MD5, one case per argv[1]:
   percentile, bytes, format, stop or md5. */
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../../runner/bench.h"
#include "../../runner/md5.h"
#include "../../runner/stop.h"
#include "../check.h"
#include "../../runner/stop.c"
#include "internal.h"

/* Reuse the actual runner owner, with finite allocation and frame seams.
   Child-process refusal/identity checks remain in tests.cmake. */
static struct {
    void *ptr;
    size_t bytes;
} movie_allocations[64];
static size_t movie_live, movie_peak, movie_largest;
static unsigned movie_attempts, movie_fail_at, movie_loads, movie_frame_calls, movie_inputs;
static muhtari_input_ex movie_packets[4];
static uint32_t movie_first_device;
static const char *movie_mutate;

static void movie_track(void *ptr, size_t n)
{
    if (ptr == NULL)
        return;
    for (unsigned i = 0u; i < 64u; i++) {
        if (movie_allocations[i].ptr == NULL) {
            movie_allocations[i].ptr = ptr;
            movie_allocations[i].bytes = n;
            movie_live += n;
            if (movie_live > movie_peak)
                movie_peak = movie_live;
            if (n > movie_largest)
                movie_largest = n;
            return;
        }
    }
    CHECK(0);
}
static void *movie_alloc(size_t n)
{
    movie_attempts++;
    if (movie_attempts == movie_fail_at)
        return NULL;
    void *p = malloc(n);
    movie_track(p, n);
    return p;
}
static void movie_free(void *p)
{
    if (p == NULL)
        return;
    for (unsigned i = 0u; i < 64u; i++) {
        if (movie_allocations[i].ptr == p) {
            movie_live -= movie_allocations[i].bytes;
            movie_allocations[i].ptr = NULL;
            free(p);
            return;
        }
    }
    CHECK(0);
}
static void *movie_realloc(void *p, size_t n)
{
    movie_attempts++;
    if (movie_attempts == movie_fail_at)
        return NULL;
    for (unsigned i = 0u; i < 64u; i++) {
        if (movie_allocations[i].ptr == p) {
            void *next = realloc(p, n);
            if (next != NULL) {
                movie_live -= movie_allocations[i].bytes;
                movie_allocations[i].ptr = NULL;
                movie_track(next, n);
            }
            return next;
        }
    }
    CHECK(0);
    return NULL;
}
static muhtari_status movie_load(muhtari *m, const muhtari_media *media)
{
    movie_loads++;
    return muhtari_load(m, media);
}
static muhtari_status movie_input(muhtari *m, const muhtari_input_ex *in)
{
    if (movie_inputs < 4u)
        movie_packets[movie_inputs] = *in;
    movie_inputs++;
    muhtari_status st = muhtari_set_input_ex(m, in);
    if (movie_mutate != NULL) {
        FILE *f = fopen(movie_mutate, "wb");
        CHECK(f != NULL);
        if (f != NULL) {
            CHECK(fputs("replaced after validation\n", f) >= 0);
            CHECK_EQ_U64(fclose(f), 0);
        }
        movie_mutate = NULL;
    }
    return st;
}
static muhtari_status movie_frame(muhtari *m, muhtari_frame *io)
{
    if (movie_frame_calls == 0u)
        movie_first_device = m->ports.pending_ex.port[0].device;
    movie_frame_calls++;
    return muhtari_run_frame(m, io);
}
int runner_entry(int argc, char **argv);
#define main runner_entry
#define malloc movie_alloc
#define realloc movie_realloc
#define free(p) movie_free(p)
#define muhtari_load movie_load
#define muhtari_set_input_ex movie_input
#define muhtari_run_frame movie_frame
#include "../../runner/main.c"
#undef muhtari_run_frame
#undef muhtari_set_input_ex
#undef muhtari_load
#undef free
#undef realloc
#undef malloc
#undef main

static void movie_reset(void)
{
    CHECK_EQ_U64(movie_live, 0u);
    movie_attempts = movie_fail_at = movie_loads = movie_frame_calls = movie_inputs = 0u;
    movie_peak = movie_largest = 0u;
    movie_first_device = 0u;
    movie_mutate = NULL;
    memset(movie_packets, 0, sizeof movie_packets);
}

static void movie_write(const char *path, const char *text)
{
    FILE *f = fopen(path, "wb");
    CHECK(f != NULL);
    if (f != NULL) {
        CHECK_EQ_U64(fwrite(text, 1u, strlen(text), f), strlen(text));
        CHECK_EQ_U64(fclose(f), 0);
    }
}

static void movie_owner(char *dir, char *rom)
{
    char path[1024];
    snprintf(path, sizeof path, "%s/owner.txt", dir);
    static const char text[] = "muhtari-movie 2\nport1 driving\nport2 none\n"
                               "|driving:4,0,1|none:|R..AB|\n";
    movie_write(path, text);
    movie_reset();
    movie_mutate = path;
    char *args[] = {"muhtari-run", "--movie", path, "--frames", "3", rom};
    CHECK_EQ_U64(runner_entry(6, args), 0);
    CHECK_EQ_U64(movie_loads, 1u);
    CHECK_EQ_U64(movie_frame_calls, 3u);
    CHECK_EQ_U64(movie_inputs, 3u);
    CHECK_EQ_U64(movie_packets[0].port[0].dx, 4u);
    CHECK_EQ_U64(movie_packets[0].port[0].buttons, MUHTARI_BUTTON_1);
    CHECK_EQ_U64(movie_packets[0].switches,
                 MUHTARI_SWITCH_RESET | MUHTARI_SWITCH_LEFT_DIFFICULTY_A);
    CHECK_EQ_U64(movie_packets[1].port[0].dx, 0u);
    CHECK_EQ_U64(movie_packets[1].port[0].buttons, 0u);
    CHECK_EQ_U64(movie_packets[1].switches, MUHTARI_SWITCH_LEFT_DIFFICULTY_A);
    CHECK_EQ_U64(movie_packets[2].port[0].dx, 0u);
    CHECK_EQ_U64(movie_live, 0u);

    /* Modern neutral Auto resolves the load override in the short first frame. */
    movie_reset();
    char *automatic[] = {"muhtari-run", "--frames", "1", "--port1", "driving", rom};
    CHECK_EQ_U64(runner_entry(6, automatic), 0);
    CHECK_EQ_U64(movie_inputs, 1u);
    CHECK_EQ_U64(movie_first_device, MUHTARI_PORT_DRIVING);
    CHECK_EQ_U64(movie_live, 0u);

    /* Actual runner decoding submits every adapter bit, then EOF releases
       all native/momentary controls while retaining difficulty options. */
    static char adapters[2][11] = {"snes2atari", "mega7800"};
    static const uint32_t devices[2] = {MUHTARI_PORT_SNES2ATARI, MUHTARI_PORT_MEGA7800};
    for (unsigned family = 0u; family < 2u; family++) {
        for (unsigned port = 0u; port < 2u; port++) {
            char typed[256];
            const char *left = port == 0u ? adapters[family] : "none";
            const char *right = port == 1u ? adapters[family] : "none";
            snprintf(typed, sizeof typed,
                     "muhtari-movie 2\nport1 %s\nport2 %s\n|%s:%s|%s:%s|RSPAB|\n", left, right,
                     left, port == 0u ? "4095" : "", right, port == 1u ? "4095" : "");
            movie_write(path, typed);
            movie_reset();
            char *adapter_args[] = {"muhtari-run", "--movie", path, "--frames", "3", rom};
            CHECK_EQ_U64(runner_entry(6, adapter_args), 0u);
            CHECK_EQ_U64(movie_inputs, 3u);
            for (unsigned frame = 0u; frame < 3u; frame++) {
                CHECK_EQ_U64(movie_packets[frame].port[port].device, devices[family]);
                CHECK_EQ_U64(movie_packets[frame].port[port].buttons, frame == 0u ? 4095u : 0u);
                CHECK_EQ_U64(movie_packets[frame].port[1u - port].device, MUHTARI_PORT_UNPLUGGED);
                CHECK_EQ_U64(movie_packets[frame].switches,
                             MUHTARI_SWITCH_LEFT_DIFFICULTY_A |
                                 (frame == 0u ? MUHTARI_SWITCH_RESET | MUHTARI_SWITCH_SELECT |
                                                    MUHTARI_SWITCH_PAUSE
                                              : 0u));
            }
            CHECK_EQ_U64(movie_live, 0u);
        }
        movie_reset();
        char *neutral_args[] = {"muhtari-run", "--frames", "1", "--port1", adapters[family], rom};
        CHECK_EQ_U64(runner_entry(6, neutral_args), 0u);
        CHECK_EQ_U64(movie_first_device, devices[family]);
        CHECK_EQ_U64(movie_live, 0u);
    }

    movie_write(path, text);
    movie_reset();
    char save_path[1024];
    snprintf(save_path, sizeof save_path, "%s/input.sav", dir);
    static const char old_save[] = "old save bytes";
    movie_write(save_path, old_save);
    char *saved[] = {"muhtari-run", "--movie", path,         "--frames", "3",
                     "--hsc",       "on",      "--save-dir", dir,        rom};
    CHECK_EQ_U64(runner_entry(10, saved), 0);
    unsigned allocations = movie_attempts;
    for (unsigned fail = 1u; fail <= allocations; fail++) {
        movie_reset();
        movie_fail_at = fail;
        CHECK_EQ_U64(runner_entry(10, saved), 1);
        CHECK_EQ_U64(movie_live, 0u);
        FILE *save_file = fopen(save_path, "rb");
        CHECK(save_file != NULL);
        if (save_file != NULL) {
            char bytes[sizeof old_save] = {0};
            CHECK_EQ_U64(fread(bytes, 1u, sizeof bytes, save_file), sizeof old_save - 1u);
            CHECK(memcmp(bytes, old_save, sizeof old_save) == 0);
            CHECK_EQ_U64(fclose(save_file), 0);
        }
    }

    /* Read/validate at the exact byte ceiling without a frame array. Header
       comments account for the remainder, each within its old 64-byte cap. */
    static const char head[] = "muhtari-movie 2\nport1 amiga-mouse\nport2 amiga-mouse\n";
    static const char row[] =
        "|amiga-mouse:-2147483648,-2147483648,3|amiga-mouse:-2147483648,-2147483648,3|...BB|\n";
    size_t padding = (MUHTARI__MOVIE_BYTES_MAX - (sizeof head - 1u)) % (sizeof row - 1u);
    uint32_t rows =
        (uint32_t)((MUHTARI__MOVIE_BYTES_MAX - (sizeof head - 1u) - padding) / (sizeof row - 1u));
    FILE *f = fopen(path, "wb");
    CHECK(f != NULL);
    if (f == NULL)
        return;
    CHECK_EQ_U64(fwrite(head, 1u, sizeof head - 1u, f), sizeof head - 1u);
    while (padding != 0u) {
        size_t line = padding > 65u ? 65u : padding;
        for (size_t i = 1u; i < line; i++)
            CHECK(fputc(i == 1u ? '#' : 'x', f) != EOF);
        CHECK(fputc('\n', f) != EOF);
        padding -= line;
    }
    for (uint32_t i = 0u; i < rows; i++)
        CHECK_EQ_U64(fwrite(row, 1u, sizeof row - 1u, f), sizeof row - 1u);
    CHECK_EQ_U64(fclose(f), 0);
    movie_reset();
    replay mv = {0};
    uint64_t start = muhtari__clock_ns();
    CHECK_EQ_U64(read_movie(path, &mv), 0);
    CHECK_EQ_U64(mv.count, rows);
    CHECK_EQ_U64(mv.bytes_count, MUHTARI__MOVIE_BYTES_MAX);
    CHECK(mv.frames == NULL);
    CHECK_EQ_U64(movie_live, MUHTARI__MOVIE_BYTES_MAX + 1u);
    CHECK_EQ_U64(movie_largest, MUHTARI__MOVIE_BYTES_MAX + 1u);
    printf("movie2 bytes %zu frames %u allocation_calls %u live %zu peak %zu cursor %zu packet %zu "
           "elapsed_ns %llu\n",
           mv.bytes_count, (unsigned)mv.count, movie_attempts, movie_live, movie_peak,
           sizeof mv.cursor, sizeof(muhtari_input_ex),
           (unsigned long long)(muhtari__clock_ns() - start));
    movie_free(mv.bytes);
    CHECK_EQ_U64(movie_live, 0u);
    unsigned file_allocations = movie_attempts;
    for (unsigned fail = 1u; fail <= file_allocations; fail++) {
        movie_reset();
        movie_fail_at = fail;
        memset(&mv, 0, sizeof mv);
        CHECK_EQ_U64(read_movie(path, &mv), 1);
        CHECK_EQ_U64(movie_live, 0u);
    }
    f = fopen(path, "ab");
    CHECK(f != NULL);
    if (f != NULL) {
        CHECK(fputc('x', f) != EOF);
        CHECK_EQ_U64(fclose(f), 0);
    }
    memset(&mv, 0, sizeof mv);
    movie_reset();
    CHECK_EQ_U64(read_movie(path, &mv), 1);
    CHECK_EQ_U64(movie_loads, 0u);
    CHECK_EQ_U64(movie_live, 0u);

    static const char small_head[] = "muhtari-movie 2\nport1 none\nport2 none\n";
    static const char small_row[] = "|none:|none:|...BB|\n";
    f = fopen(path, "wb");
    CHECK(f != NULL);
    if (f == NULL)
        return;
    CHECK_EQ_U64(fwrite(small_head, 1u, sizeof small_head - 1u, f), sizeof small_head - 1u);
    for (uint32_t i = 0u; i < MUHTARI__MOVIE_FRAMES_MAX; i++)
        CHECK_EQ_U64(fwrite(small_row, 1u, sizeof small_row - 1u, f), sizeof small_row - 1u);
    CHECK_EQ_U64(fclose(f), 0);
    movie_reset();
    memset(&mv, 0, sizeof mv);
    start = muhtari__clock_ns();
    CHECK_EQ_U64(read_movie(path, &mv), 0);
    CHECK_EQ_U64(mv.count, MUHTARI__MOVIE_FRAMES_MAX);
    CHECK(mv.frames == NULL);
    CHECK(movie_live <= (size_t)MUHTARI__MOVIE_BYTES_MAX + 1u);
    muhtari_input_ex decoded;
    muhtari__movie_error err;
    for (uint32_t i = 0u; i < mv.count; i++) {
        CHECK_EQ_U64(muhtari__movie_next_ex(mv.bytes, mv.bytes_count, &mv.header, &mv.cursor,
                                            &decoded, &err),
                     1);
    }
    CHECK_EQ_U64(
        muhtari__movie_next_ex(mv.bytes, mv.bytes_count, &mv.header, &mv.cursor, &decoded, &err),
        0);
    CHECK_EQ_U64(movie_attempts, 14u);
    printf("movie2 bytes %zu frames %u allocation_calls %u live %zu peak %zu cursor %zu packet %zu "
           "elapsed_ns %llu\n",
           mv.bytes_count, (unsigned)mv.count, movie_attempts, movie_live, movie_peak,
           sizeof mv.cursor, sizeof decoded, (unsigned long long)(muhtari__clock_ns() - start));
    movie_free(mv.bytes);
    f = fopen(path, "ab");
    CHECK(f != NULL);
    if (f != NULL) {
        CHECK_EQ_U64(fwrite(small_row, 1u, sizeof small_row - 1u, f), sizeof small_row - 1u);
        CHECK_EQ_U64(fclose(f), 0);
    }
    movie_reset();
    memset(&mv, 0, sizeof mv);
    CHECK_EQ_U64(read_movie(path, &mv), 1);
    CHECK_EQ_U64(movie_live, 0u);
}

static const uint32_t all_ppm[] = {500000u, 900000u, 990000u, 999000u, 999900u, 999990u, 1000000u};

/* Nearest rank is ceil(n * p), clamped to 1..n. */
static void percentile(void)
{
    for (size_t i = 0; i < sizeof all_ppm / sizeof all_ppm[0]; i++) {
        CHECK_EQ_U64(muhtari__bench_rank(1u, all_ppm[i]), 1u);
    }
    CHECK_EQ_U64(muhtari__bench_rank(300000u, 999990u), 299997u);
    CHECK_EQ_U64(muhtari__bench_rank(300000u, 500000u), 150000u);
    CHECK_EQ_U64(muhtari__bench_rank(300000u, 1000000u), 300000u);
    CHECK_EQ_U64(muhtari__bench_rank(7u, 900000u), 7u);
    CHECK_EQ_U64(muhtari__bench_rank(7u, 500000u), 4u);
    CHECK_EQ_U64(muhtari__bench_rank(1000u, 999000u), 999u);
    CHECK_EQ_U64(muhtari__bench_rank(0xffffffffu, 999990u), 4294924346u);
    CHECK_EQ_U64(muhtari__bench_us(1999u), 1u);
    CHECK_EQ_U64(muhtari__bench_us(0xffffffffffffffffu), 0xffffffffu);
}

/* The sample buffer's size is exact, or 0 where it does not fit in size_t,
   never a wrapped product: on 32 bits, 2^30 frames would wrap to 0 bytes. */
static void bytes(void)
{
    CHECK_EQ_U64(muhtari__bench_bytes(1u), 4u);
    CHECK_EQ_U64(muhtari__bench_bytes(300000u), 1200000u);
    CHECK_EQ_U64(muhtari__bench_bytes(0x3fffffffu), 0xfffffffcu);
#if SIZE_MAX > 0xffffffffu
    CHECK_EQ_U64(muhtari__bench_bytes(0x40000000u), 0x100000000u);
    CHECK_EQ_U64(muhtari__bench_bytes(0xffffffffu), 0x3fffffffcu);
#else
    CHECK_EQ_U64(muhtari__bench_bytes(0x40000000u), 0u);
    CHECK_EQ_U64(muhtari__bench_bytes(0xffffffffu), 0u);
#endif
}

/* The report is nine "key value" lines in a fixed order, from sorted
   samples. */
static void format(void)
{
    uint32_t us[4] = {40u, 10u, 30u, 20u};
    char buf[512];
    const char *want = "frames 4\n"
                       "fps_milli 40000\n"
                       "p50_us 20\n"
                       "p90_us 40\n"
                       "p99_us 40\n"
                       "p99_9_us 40\n"
                       "p99_99_us 40\n"
                       "p99_999_us 40\n"
                       "max_us 40\n";
    size_t len = muhtari__bench_format(buf, sizeof buf, us, 4u, 100000000u);
    CHECK_EQ_U64(len, strlen(want));
    CHECK(len == strlen(want) && strcmp(buf, want) == 0);
    if (strcmp(buf, want) != 0) {
        fprintf(stderr, "want:\n%sgot:\n%s", want, buf);
    }
    CHECK_EQ_U64(us[0], 10u);
    CHECK_EQ_U64(muhtari__bench_format(buf, 20u, us, 4u, 100000000u), 0u);
}

/* A fresh process has no stop request; SIGINT records one. */
static void stop(void)
{
    CHECK_EQ_U64(muhtari__stop_requested(), 0u);
#if !defined(_WIN32) && !defined(__wasi__)
    CHECK(signal(SIGINT, SIG_IGN) != SIG_ERR);
    CHECK(signal(SIGTERM, SIG_IGN) != SIG_ERR);
    muhtari__stop_install();
    CHECK_EQ_U64(raise(SIGINT), 0u);
    CHECK_EQ_U64(raise(SIGTERM), 0u);
    CHECK_EQ_U64(muhtari__stop_requested(), 0u);
    CHECK(signal(SIGINT, SIG_DFL) != SIG_ERR);
    CHECK(signal(SIGTERM, SIG_DFL) != SIG_ERR);
#endif
    muhtari__stop_install();
    CHECK_EQ_U64(muhtari__stop_requested(), 0u);
#ifdef _WIN32
    CHECK(on_console(CTRL_BREAK_EVENT));
    CHECK(on_console(CTRL_BREAK_EVENT));
    CHECK_EQ_U64(muhtari__flush_requested(), 1u);
    CHECK_EQ_U64(muhtari__flush_requested(), 0u);
    CHECK_EQ_U64(muhtari__stop_requested(), 0u);
    CHECK(on_console(CTRL_C_EVENT));
#else
#ifdef SIGUSR1
    CHECK(signal(SIGUSR1, SIG_IGN) != SIG_ERR);
    muhtari__stop_install();
    CHECK_EQ_U64(raise(SIGUSR1), 0u);
    CHECK_EQ_U64(muhtari__flush_requested(), 0u);
    CHECK(signal(SIGUSR1, SIG_DFL) != SIG_ERR);
    muhtari__stop_install();
    CHECK_EQ_U64(raise(SIGUSR1), 0u);
    CHECK_EQ_U64(raise(SIGUSR1), 0u);
    CHECK_EQ_U64(muhtari__flush_requested(), 1u);
    CHECK_EQ_U64(muhtari__flush_requested(), 0u);
    CHECK_EQ_U64(raise(SIGUSR1), 0u);
    CHECK_EQ_U64(muhtari__flush_requested(), 1u);
#endif
    CHECK_EQ_U64(raise(SIGINT), 0u);
#endif
    CHECK_EQ_U64(muhtari__stop_requested(), 1u);
}

/* RFC 1321 appendix A.5's test suite. Each message is hashed whole, then
   one byte per update, which crosses every block boundary. */
static void md5(void)
{
    static const char *const cases[][2] = {
        {"", "d41d8cd98f00b204e9800998ecf8427e"},
        {"a", "0cc175b9c0f1b6a831c399e269772661"},
        {"abc", "900150983cd24fb0d6963f7d28e17f72"},
        {"message digest", "f96b697d7cb7938d525a2f31aaf161d0"},
        {"abcdefghijklmnopqrstuvwxyz", "c3fcd3d76192e4007dfb496cca67e13b"},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
         "d174ab98d277d9f5a5611c2c9f419d9f"},
        {"1234567890123456789012345678901234567890"
         "1234567890123456789012345678901234567890",
         "57edf4a22be3c955ac49da2e2107b67a"},
    };
    static const char digits[] = "0123456789abcdef";
    for (size_t c = 0; c < sizeof cases / sizeof cases[0]; c++) {
        const char *msg = cases[c][0];
        size_t len = strlen(msg);
        for (int bytewise = 0; bytewise < 2; bytewise++) {
            muhtari__md5 s;
            uint8_t digest[16];
            char hex[33];
            muhtari__md5_init(&s);
            if (bytewise) {
                for (size_t i = 0; i < len; i++) {
                    muhtari__md5_update(&s, msg + i, 1u);
                }
            } else {
                muhtari__md5_update(&s, msg, len);
            }
            muhtari__md5_final(&s, digest);
            for (size_t i = 0; i < 16u; i++) {
                hex[2u * i] = digits[digest[i] >> 4];
                hex[2u * i + 1u] = digits[digest[i] & 0x0fu];
            }
            hex[32] = '\0';
            CHECK(strcmp(hex, cases[c][1]) == 0);
            if (strcmp(hex, cases[c][1]) != 0) {
                fprintf(stderr, "MD5(\"%s\"): want %s, got %s\n", msg, cases[c][1], hex);
            }
        }
    }
}

int main(int argc, char **argv)
{
    if (argc == 4 && strcmp(argv[1], "movie") == 0) {
        movie_owner(argv[2], argv[3]);
        CHECK_DONE();
    }
    if (argc != 2) {
        fprintf(stderr, "usage: muhtari-test-runner percentile|bytes|format|stop|md5\n");
        return 2;
    }
    if (strcmp(argv[1], "percentile") == 0) {
        percentile();
    } else if (strcmp(argv[1], "bytes") == 0) {
        bytes();
    } else if (strcmp(argv[1], "format") == 0) {
        format();
    } else if (strcmp(argv[1], "stop") == 0) {
        stop();
    } else if (strcmp(argv[1], "md5") == 0) {
        md5();
    } else {
        fprintf(stderr, "unknown case %s\n", argv[1]);
        return 2;
    }
    CHECK_DONE();
}
