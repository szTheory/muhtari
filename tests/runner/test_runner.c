/* The runner's bench statistics and stop flag, one case per argv[1]:
   percentile, format or stop. */
#include <signal.h>
#include <stdint.h>
#include <string.h>

#include "../../runner/bench.h"
#include "../../runner/stop.h"
#include "../check.h"

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
    CHECK_EQ_U64(raise(SIGINT), 0u);
    CHECK_EQ_U64(muhtari__stop_requested(), 1u);
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: muhtari-test-runner percentile|format|stop\n");
        return 2;
    }
    if (strcmp(argv[1], "percentile") == 0) {
        percentile();
    } else if (strcmp(argv[1], "format") == 0) {
        format();
    } else if (strcmp(argv[1], "stop") == 0) {
        stop();
    } else {
        fprintf(stderr, "unknown case %s\n", argv[1]);
        return 2;
    }
    CHECK_DONE();
}
