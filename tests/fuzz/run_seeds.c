/* The run target's seed inputs, through the seeds.h interface: 48 KiB
   images of all $00, all $02 (JAM on every opcode), all $FF and 13 filled
   from splitmix64, then one empty input, which the target pads to 48 KiB of
   $00. Eleven additional inputs explicitly select retail boards, malformed
   geometries and unsupported A8 combinations. The property tests draw
   their cartridges from the same images. */
#include "seeds.h"

#include <string.h>

#include "../prop.h"
#include "run_cart.h"

#define RUN_SEED_BYTES 49152u
#define RUN_SEEDS 28u

static const char *const names[RUN_SEEDS] = {"zero-48k",
                                             "jam-48k",
                                             "ff-48k",
                                             "splitmix-01",
                                             "splitmix-02",
                                             "splitmix-03",
                                             "splitmix-04",
                                             "splitmix-05",
                                             "splitmix-06",
                                             "splitmix-07",
                                             "splitmix-08",
                                             "splitmix-09",
                                             "splitmix-10",
                                             "splitmix-11",
                                             "splitmix-12",
                                             "splitmix-13",
                                             "empty",
                                             "activision",
                                             "absolute",
                                             "fractalus",
                                             "activision-short",
                                             "activision-truncated",
                                             "absolute-short",
                                             "absolute-long",
                                             "fractalus-short",
                                             "fractalus-long",
                                             "sg-a8",
                                             "bankset-a8"};

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
    if (i == 16u) {
        return 0u;
    }
    if (i >= 17u) {
        static const uint32_t boards[] = {MUHTARI_BOARD_ACTIVISION, MUHTARI_BOARD_ABSOLUTE,
                                          MUHTARI_BOARD_FRACTALUS};
        uint32_t board = i < 20u   ? boards[i - 17u]
                         : i < 26u ? boards[(i - 20u) / 2u]
                                   : MUHTARI_BOARD_FRACTALUS;
        struct run_cart_info info = run_cart_control(buf, board);
        muhtari_cart_info probed = {0};
        probed.size = (uint32_t)sizeof probed;
        if (muhtari_probe(buf, info.bytes, &probed) != MUHTARI_STATUS_OK || probed.board != board)
            abort();
        if (i >= 20u && i < 26u) {
            size_t n = (i & 1u) == 0u ? info.bytes - 1u : info.bytes + 1u;
            /* The larger Activision neighbor belongs to the loader corpus
               (whose old cap is larger). Execution seeds stay <=131200;
               this case declares the full geometry over a truncated body. */
            if (n > RUN_CART_BYTES)
                n = info.bytes - 1u;
            uint32_t body = i == 21u ? 131072u : (uint32_t)n - 128u;
            buf[50] = (uint8_t)(body >> 16);
            buf[51] = (uint8_t)(body >> 8);
            buf[52] = (uint8_t)body;
            buf[n - 1u] = 0u;
            probed.size = (uint32_t)sizeof probed;
            muhtari_status status = muhtari_probe(buf, n, &probed);
            muhtari_status expected =
                i == 21u ? MUHTARI_STATUS_ERR_TRUNCATED_IMAGE : MUHTARI_STATUS_ERR_INVALID_IMAGE;
            if (n > RUN_CART_BYTES || status != expected)
                abort();
            return n;
        }
        if (i >= 26u) {
            buf[0] = 4u;
            buf[64] = 1u;
            buf[65] = i == 26u ? 2u : 0x82u;
            probed.size = (uint32_t)sizeof probed;
            if (muhtari_probe(buf, info.bytes, &probed) != MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD)
                abort();
        }
        return info.bytes;
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
