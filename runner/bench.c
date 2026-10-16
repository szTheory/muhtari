#include "bench.h"

#include <stdio.h>
#include <stdlib.h>

/* Nearest rank: the smallest sample with at least p of the samples at or
   below it, so the rank is ceil(n * p) (NIST/SEMATECH e-Handbook of
   Statistical Methods, 7.2.6.2; no interpolation). */
uint32_t muhtari__bench_rank(uint32_t n, uint32_t ppm)
{
    uint64_t rank = ((uint64_t)n * ppm + 999999u) / 1000000u;
    if (rank < 1u) {
        rank = 1u;
    }
    if (rank > n) {
        rank = n;
    }
    return (uint32_t)rank;
}

uint32_t muhtari__bench_us(uint64_t ns)
{
    uint64_t us = ns / 1000u;
    return us > 0xffffffffu ? 0xffffffffu : (uint32_t)us;
}

static int compare_u32(const void *a, const void *b)
{
    uint32_t x = *(const uint32_t *)a;
    uint32_t y = *(const uint32_t *)b;
    return (x > y) - (x < y);
}

/* frames * 10^12 / total_ns by long division, one decimal digit at a time,
   so no intermediate exceeds 10 * total_ns. */
static uint64_t fps_milli(uint32_t frames, uint64_t total_ns)
{
    if (total_ns == 0u) {
        return 0u;
    }
    uint64_t q = frames / total_ns;
    uint64_t r = frames % total_ns;
    for (int digit = 0; digit < 12; digit++) {
        r *= 10u;
        q = q * 10u + r / total_ns;
        r %= total_ns;
    }
    return q;
}

/* The reported percentiles in parts per million, in report order. */
static const struct {
    const char *key;
    uint32_t ppm;
} percentiles[] = {
    {"p50_us", 500000u},   {"p90_us", 900000u},    {"p99_us", 990000u},
    {"p99_9_us", 999000u}, {"p99_99_us", 999900u}, {"p99_999_us", 999990u},
};

size_t muhtari__bench_format(char *buf, size_t cap, uint32_t *us, uint32_t n, uint64_t total_ns)
{
    qsort(us, n, sizeof us[0], compare_u32);
    int w = snprintf(buf, cap, "frames %lu\nfps_milli %llu\n", (unsigned long)n,
                     (unsigned long long)fps_milli(n, total_ns));
    size_t len = w < 0 ? cap : (size_t)w;
    for (size_t i = 0; i < sizeof percentiles / sizeof percentiles[0] && len < cap; i++) {
        uint32_t v = us[muhtari__bench_rank(n, percentiles[i].ppm) - 1u];
        w = snprintf(buf + len, cap - len, "%s %lu\n", percentiles[i].key, (unsigned long)v);
        len += w < 0 ? cap : (size_t)w;
    }
    if (len < cap) {
        w = snprintf(buf + len, cap - len, "max_us %lu\n", (unsigned long)us[n - 1u]);
        len += w < 0 ? cap : (size_t)w;
    }
    return len < cap ? len : 0u;
}
