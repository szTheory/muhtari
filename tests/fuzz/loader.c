/* The loader fuzz target: muhtari_probe and the internal load on the same
   bytes. Any broken invariant calls abort(), which libFuzzer and the replay
   both report:
   1. two probes of the same bytes give the same status and the same info;
   2. the load returns probe's status;
   3. through a counting allocator, a refusal allocates nothing and leaves
      the instance byte-identical, and an accepted image makes exactly one
      allocation of rom_size bytes holding the mapped bytes; with an
      allocator that fails every call after create, the status is the
      refusal's own, or ERR_NO_MEMORY for an accepted image, and the
      instance is unchanged;
   4. destroy frees everything allocated, and a second accepted load
      frees the first. */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../alloc.h"
#include "internal.h"
#include "muhtari.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size);

static struct muhtari before;

static void require(int cond)
{
    if (!cond) {
        abort();
    }
}

/* Loads data into a fresh instance; fail makes every allocation after
   create fail. Returns the load's status. */
static muhtari_status load_once(const uint8_t *data, size_t size, int fail,
                                const muhtari_cart_info *probed, muhtari_status want)
{
    alloc_counts c;
    muhtari_config cfg = alloc_config(&c);
    muhtari *m = NULL;
    require(muhtari_create(&cfg, &m) == MUHTARI_STATUS_OK);
    alloc_counts created = c;
    if (fail) {
        c.fail_from = c.allocs + 1u;
    }
    memcpy(&before, m, sizeof before);
    muhtari_status st = muhtari__cart_load(m, data, size);
    if (st == MUHTARI_STATUS_OK) {
        require(!fail && want == MUHTARI_STATUS_OK);
        require(c.allocs == created.allocs + 1u && c.frees == created.frees);
        require(c.alloc_size == probed->rom_size);
        require(memcmp(&m->cart.info, probed, sizeof *probed) == 0);
        size_t at = (probed->flags & MUHTARI_CART_HEADER) != 0u ? 128u : 0u;
        require(at + probed->rom_size <= size);
        require(memcmp(m->cart.rom, data + at, probed->rom_size) == 0);
        /* A second load replaces the first: one allocation, one free. */
        require(muhtari__cart_load(m, data, size) == MUHTARI_STATUS_OK);
        require(c.allocs == created.allocs + 2u && c.frees == created.frees + 1u);
        require(c.free_size == probed->rom_size);
    } else {
        /* A refusal makes no call at all; a failed allocation is the one
           call an accepted image makes. */
        if (want == MUHTARI_STATUS_OK) {
            require(fail && st == MUHTARI_STATUS_ERR_NO_MEMORY);
            require(c.allocs == created.allocs + 1u);
        } else {
            require(st == want);
            require(c.allocs == created.allocs);
        }
        require(c.frees == created.frees);
        require(memcmp(m, &before, sizeof before) == 0);
    }
    muhtari_destroy(m);
    require(c.frees == c.allocs - (st == MUHTARI_STATUS_ERR_NO_MEMORY ? 1u : 0u));
    return st;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    muhtari_cart_info a;
    muhtari_cart_info b;
    memset(&a, 0, sizeof a);
    memset(&b, 0, sizeof b);
    a.size = (uint32_t)sizeof a;
    b.size = (uint32_t)sizeof b;
    muhtari_status first = muhtari_probe(data, size, &a);
    muhtari_status second = muhtari_probe(data, size, &b);
    require(first == second && memcmp(&a, &b, sizeof a) == 0);

    require(load_once(data, size, 0, &a, first) == first);
    (void)load_once(data, size, 1, &a, first);
    return 0;
}

#include "replay_main.h"
