/* CRC-32 (reflected polynomial 0xEDB88320), owned code for the runner. */
#ifndef MUHTARI_RUNNER_CRC32_H
#define MUHTARI_RUNNER_CRC32_H

#include <stddef.h>
#include <stdint.h>

/* Continues a CRC-32 over n bytes at p. Start with crc = 0; pass the result
   back in to extend it. */
uint32_t muhtari__crc32(uint32_t crc, const uint8_t *p, size_t n);

#endif /* MUHTARI_RUNNER_CRC32_H */
