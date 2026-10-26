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
    size_t live_bytes;
    uint32_t size_errors;
    uint32_t check_sizes;
    /* Optional delayed free makes reads from replaced backing deterministic. */
    uint32_t poison_frees;
    void *retired;
} alloc_counts;

typedef union alloc_header {
#if defined(_MSC_VER)
    /* MSVC's C headers omit max_align_t. Match malloc's fundamental alignment:
       learn.microsoft.com/cpp/c-runtime-library/reference/malloc */
#if defined(_WIN64)
    _Alignas(16) uint8_t alignment;
#else
    _Alignas(8) uint8_t alignment;
#endif
#else
    max_align_t alignment;
#endif
    size_t bytes;
} alloc_header;

static inline void *count_alloc(void *user, size_t size)
{
    alloc_counts *c = (alloc_counts *)user;
    c->allocs++;
    c->alloc_size = size;
    if (c->fail_from != 0u && c->allocs >= c->fail_from) {
        return NULL;
    }
    if (c->check_sizes == 0u)
        return malloc(size);
    if (size > SIZE_MAX - sizeof(alloc_header))
        return NULL;
    alloc_header *h = malloc(sizeof *h + size);
    if (h == NULL)
        return NULL;
    h->bytes = size;
    c->live_bytes += size;
    return h + 1;
}

static inline void count_free(void *user, void *ptr, size_t size)
{
    alloc_counts *c = (alloc_counts *)user;
    c->frees++;
    c->free_size = size;
    if (c->check_sizes == 0u) {
        free(ptr);
        return;
    }
    alloc_header *h = (alloc_header *)ptr - 1;
    if (h->bytes != size)
        c->size_errors++;
    c->live_bytes -= h->bytes;
    if (c->poison_frees != 0u) {
        free(c->retired);
        memset(ptr, 0xDD, h->bytes);
        c->retired = h;
    } else {
        free(h);
    }
}

static inline void count_release_retired(alloc_counts *c)
{
    free(c->retired);
    c->retired = NULL;
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
