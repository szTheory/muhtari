/* The run target's seed inputs, through the seeds.h interface: 48 KiB
   images of all $00, all $02 (JAM on every opcode), all $FF and 13 filled
   from splitmix64, then one empty input, which the target pads to 48 KiB of
   $00. Eleven additional inputs explicitly select retail boards, malformed
   geometries and unsupported A8 combinations; one more retains an accidental
   header prefix as a linear-builder regression. The property tests draw
   their cartridges from the same images. */
#include "seeds.h"

#include <string.h>

#include "../prop.h"
#include "run_cart.h"

#define RUN_SEED_BYTES 49152u
#define RUN_SEEDS 122u

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
                                             "bankset-a8",
                                             "magic-linear",
                                             "typed-lightgun",
                                             "typed-paddle",
                                             "typed-keypad",
                                             "typed-driving",
                                             "typed-trakball",
                                             "typed-st-mouse",
                                             "typed-amiga-mouse",
                                             "typed-snes",
                                             "typed-mega",
                                             "bankset-32k-0k-off-ntsc",
                                             "bankset-32k-16k-off-ntsc",
                                             "bankset-32k-32k-off-ntsc",
                                             "bankset-48k-0k-off-ntsc",
                                             "bankset-52k-0k-off-ntsc",
                                             "bankset-128k-0k-off-ntsc",
                                             "bankset-128k-16k-off-ntsc",
                                             "bankset-128k-32k-off-ntsc",
                                             "bankset-256k-0k-off-ntsc",
                                             "bankset-256k-16k-off-ntsc",
                                             "bankset-256k-32k-off-ntsc",
                                             "bankset-512k-0k-off-ntsc",
                                             "bankset-512k-16k-off-ntsc",
                                             "bankset-512k-32k-off-ntsc",
                                             "bankset-32k-0k-xboard-ntsc",
                                             "bankset-32k-16k-xboard-ntsc",
                                             "bankset-32k-32k-xboard-ntsc",
                                             "bankset-48k-0k-xboard-ntsc",
                                             "bankset-52k-0k-xboard-ntsc",
                                             "bankset-128k-0k-xboard-ntsc",
                                             "bankset-128k-16k-xboard-ntsc",
                                             "bankset-128k-32k-xboard-ntsc",
                                             "bankset-256k-0k-xboard-ntsc",
                                             "bankset-256k-16k-xboard-ntsc",
                                             "bankset-256k-32k-xboard-ntsc",
                                             "bankset-512k-0k-xboard-ntsc",
                                             "bankset-512k-16k-xboard-ntsc",
                                             "bankset-512k-32k-xboard-ntsc",
                                             "bankset-32k-0k-xm-ntsc",
                                             "bankset-32k-16k-xm-ntsc",
                                             "bankset-32k-32k-xm-ntsc",
                                             "bankset-48k-0k-xm-ntsc",
                                             "bankset-52k-0k-xm-ntsc",
                                             "bankset-128k-0k-xm-ntsc",
                                             "bankset-128k-16k-xm-ntsc",
                                             "bankset-128k-32k-xm-ntsc",
                                             "bankset-256k-0k-xm-ntsc",
                                             "bankset-256k-16k-xm-ntsc",
                                             "bankset-256k-32k-xm-ntsc",
                                             "bankset-512k-0k-xm-ntsc",
                                             "bankset-512k-16k-xm-ntsc",
                                             "bankset-512k-32k-xm-ntsc",
                                             "bankset-32k-0k-off-pal",
                                             "bankset-32k-16k-off-pal",
                                             "bankset-32k-32k-off-pal",
                                             "bankset-48k-0k-off-pal",
                                             "bankset-52k-0k-off-pal",
                                             "bankset-128k-0k-off-pal",
                                             "bankset-128k-16k-off-pal",
                                             "bankset-128k-32k-off-pal",
                                             "bankset-256k-0k-off-pal",
                                             "bankset-256k-16k-off-pal",
                                             "bankset-256k-32k-off-pal",
                                             "bankset-512k-0k-off-pal",
                                             "bankset-512k-16k-off-pal",
                                             "bankset-512k-32k-off-pal",
                                             "bankset-32k-0k-xboard-pal",
                                             "bankset-32k-16k-xboard-pal",
                                             "bankset-32k-32k-xboard-pal",
                                             "bankset-48k-0k-xboard-pal",
                                             "bankset-52k-0k-xboard-pal",
                                             "bankset-128k-0k-xboard-pal",
                                             "bankset-128k-16k-xboard-pal",
                                             "bankset-128k-32k-xboard-pal",
                                             "bankset-256k-0k-xboard-pal",
                                             "bankset-256k-16k-xboard-pal",
                                             "bankset-256k-32k-xboard-pal",
                                             "bankset-512k-0k-xboard-pal",
                                             "bankset-512k-16k-xboard-pal",
                                             "bankset-512k-32k-xboard-pal",
                                             "bankset-32k-0k-xm-pal",
                                             "bankset-32k-16k-xm-pal",
                                             "bankset-32k-32k-xm-pal",
                                             "bankset-48k-0k-xm-pal",
                                             "bankset-52k-0k-xm-pal",
                                             "bankset-128k-0k-xm-pal",
                                             "bankset-128k-16k-xm-pal",
                                             "bankset-128k-32k-xm-pal",
                                             "bankset-256k-0k-xm-pal",
                                             "bankset-256k-16k-xm-pal",
                                             "bankset-256k-32k-xm-pal",
                                             "bankset-512k-0k-xm-pal",
                                             "bankset-512k-16k-xm-pal",
                                             "bankset-512k-32k-xm-pal"};

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
    if (i >= 38u) {
        static const uint32_t halves[] = {32u,  32u,  32u,  48u,  52u,  128u, 128u,
                                          128u, 256u, 256u, 256u, 512u, 512u, 512u};
        static const uint8_t ram[] = {0u, 1u, 3u, 0u, 0u, 0u, 1u, 3u, 0u, 1u, 3u, 0u, 1u, 3u};
        unsigned profile = (i - 38u) % 14u;
        uint32_t body = halves[profile] * 2048u;
        size_t n = 128u + body;
        memset(buf, (i & 1u) != 0u ? 0x02 : 0xEA, n);
        memset(buf, 0, 128u);
        buf[0] = 4u;
        memcpy(buf + 1u, "ATARI7800", 9u);
        buf[49] = (uint8_t)(body >> 24);
        buf[50] = (uint8_t)(body >> 16);
        buf[51] = (uint8_t)(body >> 8);
        buf[52] = (uint8_t)body;
        buf[57] = (uint8_t)((i - 38u) / 42u);
        /* Byte 62 is a test-input selector, consumed as an explicit host
           override; it is not presented as an A78 expansion declaration. */
        buf[62] = (uint8_t)(MUHTARI_EXPANSION_OFF + ((i - 38u) / 14u) % 3u);
        buf[64] = halves[profile] >= 128u ? 1u : 0u;
        buf[65] = 0x80u | ram[profile];
        buf[67] = 3u;
        for (unsigned set = 0u; set < 2u; set++) {
            uint8_t *code = buf + 128u + (set + 1u) * (body / 2u) - 8192u;
            if ((i & 1u) == 0u) {
                code[0] = 0x4Cu;
                code[1] = 0u;
                code[2] = 0xE0u;
            }
        }
        return n;
    }
    if (i >= 29u) {
        memset(buf, 0x02, RUN_SEED_BYTES);
        buf[0] = (uint8_t)(i - 29u);
        buf[1] = (uint8_t)(0xA5u + i);
        return RUN_SEED_BYTES;
    }
    static const uint8_t fills[3] = {0x00u, 0x02u, 0xFFu};
    if (i < 3u) {
        memset(buf, fills[i], RUN_SEED_BYTES);
        return RUN_SEED_BYTES;
    }
    if (i == 16u) {
        return 0u;
    }
    if (i == 28u) {
        /* Minimized hosted fuzz input: an accidental A78 prefix in bytes
           intended for the headerless linear builder. No ROM fixture. */
        buf[0] = 0u;
        memcpy(buf + 1u, "ATARI7800", 9u);
        return 10u;
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
            if (i == 21u || n > RUN_CART_BYTES)
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
