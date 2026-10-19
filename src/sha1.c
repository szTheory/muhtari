/* SHA-1 as specified in FIPS 180-4: functions in section 4.1.1,
   constants in section 4.2.1, padding in section 5.1.1, the initial value
   in section 5.3.1 and the computation in section 6.1. Written from the
   standard. Words are assembled from bytes, so the result does not depend
   on the host's byte order. */
#include "sha1.h"

#include <string.h>

static uint32_t rotl(uint32_t x, uint32_t n)
{
    return (x << n) | (x >> (32u - n));
}

/* FIPS 180-4 section 6.1.2: process one 512-bit block. The round function
   and constant change every 20 rounds (section 4.1.1, section 4.2.1). */
static void compress(uint32_t h[5], const uint8_t block[64])
{
    uint32_t w[80];
    for (uint32_t t = 0; t < 16u; t++) {
        w[t] = ((uint32_t)block[4u * t] << 24) | ((uint32_t)block[4u * t + 1u] << 16) |
               ((uint32_t)block[4u * t + 2u] << 8) | (uint32_t)block[4u * t + 3u];
    }
    for (uint32_t t = 16; t < 80u; t++) {
        w[t] = rotl(w[t - 3u] ^ w[t - 8u] ^ w[t - 14u] ^ w[t - 16u], 1);
    }
    uint32_t a = h[0], b = h[1], c = h[2], d = h[3], e = h[4];
    for (uint32_t t = 0; t < 80u; t++) {
        uint32_t f, k;
        if (t < 20u) {
            f = (b & c) ^ (~b & d); /* Ch */
            k = 0x5a827999u;
        } else if (t < 40u) {
            f = b ^ c ^ d; /* Parity */
            k = 0x6ed9eba1u;
        } else if (t < 60u) {
            f = (b & c) ^ (b & d) ^ (c & d); /* Maj */
            k = 0x8f1bbcdcu;
        } else {
            f = b ^ c ^ d; /* Parity */
            k = 0xca62c1d6u;
        }
        uint32_t temp = rotl(a, 5) + f + e + k + w[t];
        e = d;
        d = c;
        c = rotl(b, 30);
        b = a;
        a = temp;
    }
    h[0] += a;
    h[1] += b;
    h[2] += c;
    h[3] += d;
    h[4] += e;
}

void muhtari__sha1_init(struct muhtari__sha1 *ctx)
{
    /* FIPS 180-4 section 5.3.1. */
    static const uint32_t H0[5] = {0x67452301u, 0xefcdab89u, 0x98badcfeu, 0x10325476u, 0xc3d2e1f0u};
    memcpy(ctx->h, H0, sizeof H0);
    ctx->bits = 0;
    ctx->used = 0;
}

void muhtari__sha1_update(struct muhtari__sha1 *ctx, const void *p, size_t n)
{
    const uint8_t *bytes = (const uint8_t *)p;
    ctx->bits += (uint64_t)n << 3;
    while (n > 0u) {
        size_t take = 64u - (size_t)ctx->used;
        if (take > n) {
            take = n;
        }
        memcpy(ctx->block + ctx->used, bytes, take);
        ctx->used += (uint32_t)take;
        bytes += take;
        n -= take;
        if (ctx->used == 64u) {
            compress(ctx->h, ctx->block);
            ctx->used = 0;
        }
    }
}

void muhtari__sha1_final(struct muhtari__sha1 *ctx, uint8_t out[20])
{
    /* FIPS 180-4 section 5.1.1: a 1 bit, zeros, then the 64-bit length in
       bits, big-endian, ending on a block boundary. */
    ctx->block[ctx->used++] = 0x80u;
    if (ctx->used > 56u) {
        memset(ctx->block + ctx->used, 0, 64u - (size_t)ctx->used);
        compress(ctx->h, ctx->block);
        ctx->used = 0;
    }
    memset(ctx->block + ctx->used, 0, 56u - (size_t)ctx->used);
    for (uint32_t i = 0; i < 8u; i++) {
        ctx->block[56u + i] = (uint8_t)(ctx->bits >> (56u - 8u * i));
    }
    compress(ctx->h, ctx->block);
    for (uint32_t i = 0; i < 5u; i++) {
        out[4u * i] = (uint8_t)(ctx->h[i] >> 24);
        out[4u * i + 1u] = (uint8_t)(ctx->h[i] >> 16);
        out[4u * i + 2u] = (uint8_t)(ctx->h[i] >> 8);
        out[4u * i + 3u] = (uint8_t)ctx->h[i];
    }
}
