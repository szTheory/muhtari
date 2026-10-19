/* SHA-1 (FIPS 180-4), the core's one hash, which identifies cartridge
   images. The runner uses the same code. */
#ifndef MUHTARI_SHA1_H
#define MUHTARI_SHA1_H

#include <stddef.h>
#include <stdint.h>

struct muhtari__sha1 {
    uint32_t h[5];     /* intermediate hash value */
    uint64_t bits;     /* message length in bits, unsigned */
    uint8_t block[64]; /* bytes of the current, unfinished block */
    uint32_t used;     /* bytes in block */
};

void muhtari__sha1_init(struct muhtari__sha1 *ctx);
void muhtari__sha1_update(struct muhtari__sha1 *ctx, const void *p, size_t n);
void muhtari__sha1_final(struct muhtari__sha1 *ctx, uint8_t out[20]);

#endif /* MUHTARI_SHA1_H */
