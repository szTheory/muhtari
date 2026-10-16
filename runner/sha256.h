/* SHA-256 (FIPS 180-4), owned code for the runner's frame hashes. */
#ifndef MUHTARI_RUNNER_SHA256_H
#define MUHTARI_RUNNER_SHA256_H

#include <stddef.h>
#include <stdint.h>

typedef struct muhtari__sha256 {
    uint32_t h[8];     /* intermediate hash value */
    uint8_t block[64]; /* bytes of the current, unfinished block */
    uint32_t used;     /* bytes in block */
    uint64_t total;    /* message length in bytes */
} muhtari__sha256;

void muhtari__sha256_init(muhtari__sha256 *s);
void muhtari__sha256_update(muhtari__sha256 *s, const void *data, size_t len);
void muhtari__sha256_final(muhtari__sha256 *s, uint8_t digest[32]);

/* Writes the digest as 64 lowercase hex digits and a terminating NUL. */
void muhtari__sha256_hex(const uint8_t digest[32], char hex[65]);

#endif /* MUHTARI_RUNNER_SHA256_H */
