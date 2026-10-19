/* MD5 (RFC 1321), owned code for the runner's image hashes. */
#ifndef MUHTARI_RUNNER_MD5_H
#define MUHTARI_RUNNER_MD5_H

#include <stddef.h>
#include <stdint.h>

typedef struct muhtari__md5 {
    uint32_t h[4];     /* the buffer A, B, C, D */
    uint8_t block[64]; /* bytes of the current, unfinished block */
    uint32_t used;     /* bytes in block */
    uint64_t total;    /* message length in bytes */
} muhtari__md5;

void muhtari__md5_init(muhtari__md5 *s);
void muhtari__md5_update(muhtari__md5 *s, const void *data, size_t len);
void muhtari__md5_final(muhtari__md5 *s, uint8_t digest[16]);

#endif /* MUHTARI_RUNNER_MD5_H */
