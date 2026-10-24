/* The runner's bench statistics, stop flag and MD5, one case per argv[1]:
   percentile, bytes, format, stop or md5. */
#include <signal.h>
#include <stdint.h>
#include <string.h>

#include "../../runner/bench.h"
#include "../../runner/md5.h"
#include "../../runner/stop.h"
#include "../check.h"
#include "../../runner/stop.c"

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
