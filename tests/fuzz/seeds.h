/* The loader's seed inputs, built in memory by tests/cart/images.c: one or
   more images of every identification outcome, so no image bytes are
   tracked. */
#ifndef MUHTARI_FUZZ_SEEDS_H
#define MUHTARI_FUZZ_SEEDS_H

#include <stddef.h>
#include <stdint.h>

#include "muhtari.h"

/* The largest seed: one byte past the supported 1 MiB ROM plus header.
   Expansion backing is allocated by load, never appended to an image. */
#define SEEDS_SIZE_MAX (MUHTARI_IMAGE_SIZE_MAX + 1u)

/* The number of seeds. */
uint32_t seeds_count(void);

/* Seed i's stable name, a file name with no directory. */
const char *seeds_name(uint32_t i);

/* Builds seed i into buf, which holds SEEDS_SIZE_MAX bytes, and returns its
   size. */
size_t seeds_build(uint32_t i, uint8_t *buf);

#endif /* MUHTARI_FUZZ_SEEDS_H */
