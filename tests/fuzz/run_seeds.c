/* The run target's seed inputs, through the seeds.h interface: 48 KiB
   images of all $00, all $02 (JAM on every opcode), all $FF and 13 filled
   from splitmix64, then one empty input, which the target pads to 48 KiB of
   $00. The property tests draw their cartridges from the same images. */
#include "seeds.h"

#include <string.h>

#include "../prop.h"

#define RUN_SEED_BYTES 49152u
#define RUN_SEEDS 17u

static const char *const names[RUN_SEEDS] = {
    "zero-48k",    "jam-48k",     "ff-48k",      "splitmix-01", "splitmix-02", "splitmix-03",
    "splitmix-04", "splitmix-05", "splitmix-06", "splitmix-07", "splitmix-08", "splitmix-09",
    "splitmix-10", "splitmix-11", "splitmix-12", "splitmix-13", "empty"};

uint32_t seeds_count(void)
{
    return RUN_SEEDS;
}

const char *seeds_name(uint32_t i)
{
    return names[i];
}

size_t seeds_build(uint32_t i, uint8_t *buf)
{
    static const uint8_t fills[3] = {0x00u, 0x02u, 0xFFu};
    if (i < 3u) {
        memset(buf, fills[i], RUN_SEED_BYTES);
        return RUN_SEED_BYTES;
    }
    if (i == RUN_SEEDS - 1u) {
        return 0u;
    }
    uint64_t s = i;
    for (uint32_t at = 0; at < RUN_SEED_BYTES; at += 8u) {
        uint64_t v = prop_next(&s);
        for (uint32_t b = 0; b < 8u; b++) {
            buf[at + b] = (uint8_t)(v >> (8u * b));
        }
    }
    return RUN_SEED_BYTES;
}
