/* Frame-time statistics for --bench. Runner-only and integer-only.

   Percentiles use the nearest-rank method with no interpolation: the rank
   is ceil(n * p), with p in parts per million so p99.999 is exact. Each
   reported value is therefore a measured sample. */
#ifndef MUHTARI_RUNNER_BENCH_H
#define MUHTARI_RUNNER_BENCH_H

#include <stddef.h>
#include <stdint.h>

/* The one-based nearest rank of ppm parts per million in n >= 1 samples,
   clamped to 1..n. */
uint32_t muhtari__bench_rank(uint32_t n, uint32_t ppm);

/* Microseconds from nanoseconds, truncated and saturated at UINT32_MAX. */
uint32_t muhtari__bench_us(uint64_t ns);

/* Sorts n >= 1 microsecond samples in place and writes the report into buf,
   one "key value" line each:

     frames fps_milli p50_us p90_us p99_us p99_9_us p99_99_us p99_999_us max_us

   fps_milli is frames per second times 1000, truncated, from total_ns (0
   when total_ns is 0). Returns the length written, or 0 when cap is too
   small. */
size_t muhtari__bench_format(char *buf, size_t cap, uint32_t *us, uint32_t n, uint64_t total_ns);

#endif /* MUHTARI_RUNNER_BENCH_H */
