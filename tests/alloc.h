/* A counting allocator for the tests and the fuzz harness: it records every
   call and the last sizes it saw, and can be made to fail. Pass it as a
   muhtari_allocator with a struct alloc_counts as user. */
#ifndef MUHTARI_TEST_ALLOC_H
#define MUHTARI_TEST_ALLOC_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "muhtari.h"

typedef struct alloc_counts {
    uint32_t allocs, frees;
    size_t alloc_size, free_size;
    /* Nonzero: the call numbered fail_from (counting from 1) and every later
       one returns NULL. */
    uint32_t fail_from;
} alloc_counts;

static inline void *count_alloc(void *user, size_t size)
{
    alloc_counts *c = (alloc_counts *)user;
    c->allocs++;
    c->alloc_size = size;
    if (c->fail_from != 0u && c->allocs >= c->fail_from) {
        return NULL;
    }
    return malloc(size);
}

static inline void count_free(void *user, void *ptr, size_t size)
{
    alloc_counts *c = (alloc_counts *)user;
    c->frees++;
    c->free_size = size;
    free(ptr);
}

/* A zeroed config whose allocator counts into *c, itself zeroed. */
static inline muhtari_config alloc_config(alloc_counts *c)
{
    muhtari_config cfg;
    memset(c, 0, sizeof *c);
    memset(&cfg, 0, sizeof cfg);
    cfg.size = (uint32_t)sizeof cfg;
    cfg.allocator.alloc = count_alloc;
    cfg.allocator.free = count_free;
    cfg.allocator.user = c;
    return cfg;
}

#endif /* MUHTARI_TEST_ALLOC_H */
