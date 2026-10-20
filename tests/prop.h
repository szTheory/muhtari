/* Property tests without a framework. prop_run runs a property, a function
   of a random stream and a size that returns 1 when it holds, over the case
   count from the seed that PROP_ENV_* name, else 100 cases from a fixed seed.
   Case c's stream depends only on the seed and c. A failing case is retried
   with the size halved until it passes; the seed, the case and the smallest
   failing size are printed (docs/emulation-tests.md). */
#ifndef MUHTARI_PROP_H
#define MUHTARI_PROP_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define PROP_SIZE_MAX 8u
#define PROP_SEED 20261020u
#define PROP_ENV_CASES "MUHTARI_PROP_CASES"
#define PROP_ENV_SEED "MUHTARI_PROP_SEED"
#define PROP_GAMMA 0x9E3779B97F4A7C15ull

/* splitmix64: the next value of the stream *s. */
static inline uint64_t prop_next(uint64_t *s)
{
    uint64_t z = (*s += PROP_GAMMA);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
    return z ^ (z >> 31);
}

/* A value below n, n >= 1. */
static inline uint32_t prop_below(uint64_t *s, uint32_t n)
{
    return (uint32_t)(prop_next(s) % n);
}

/* A decimal environment variable, fallback when unset or empty; a typo exits. */
static inline uint64_t prop_env(const char *name, uint64_t fallback)
{
    const char *text = getenv(name);
    char *end = NULL;
    if (text == NULL || *text == '\0') {
        return fallback;
    }
    unsigned long long v = strtoull(text, &end, 10);
    if (*end != '\0' || *text == '-') {
        fprintf(stderr, "%s=%s: not a decimal number\n", name, text);
        exit(2);
    }
    return (uint64_t)v;
}

/* Runs fn over every case; returns the exit status, 0 when all pass. */
static inline int prop_run(const char *name, int (*fn)(uint64_t *rng, uint32_t size))
{
    unsigned long long seed = prop_env(PROP_ENV_SEED, PROP_SEED);
    unsigned long long cases = prop_env(PROP_ENV_CASES, 100u);
    for (unsigned long long c = 0; c < cases; c++) {
        uint64_t at = seed + c * PROP_GAMMA;
        uint64_t start = prop_next(&at);
        uint64_t rng = start;
        uint32_t smallest = PROP_SIZE_MAX;
        if (fn(&rng, smallest)) {
            continue;
        }
        for (uint32_t size = PROP_SIZE_MAX / 2u; size > 0u; size /= 2u) {
            rng = start;
            if (fn(&rng, size)) {
                break;
            }
            smallest = size;
        }
        fprintf(stderr, "%s: failed: seed %llu case %llu size %u\n", name, seed, c,
                (unsigned)smallest);
        fprintf(stderr, "rerun: " PROP_ENV_SEED "=%llu " PROP_ENV_CASES "=%llu\n", seed, c + 1u);
        return 1;
    }
    printf("%s: %llu cases passed, seed %llu\n", name, cases, seed);
    return 0;
}

#endif /* MUHTARI_PROP_H */
