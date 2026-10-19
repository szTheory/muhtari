/* Converts one file of the SingleStepTests 65x02 vectors (6502/v1/<xx>.json)
   to one N65V v1 chunk (n65v.h) in memory. Test code only.

   The tokenizer knows only the vector schema: any JSON whitespace, keys
   matched by name (name, initial, final, cycles; pc, s, a, x, y, p, ram),
   unsigned decimal integers without a leading zero, as JSON requires, and
   the kinds "read" and "write". The file is one JSON array of objects;
   `name` is dropped. Anything else, a value out of range, a list longer than
   255, a missing or repeated key, or a first cycle that is not a read of the
   opcode at the initial pc, is an error naming the byte offset. Both upstream
   layouts, the compact one and the spaced one, are the same to it. */
#ifndef MUHTARI_SST_JSON_H
#define MUHTARI_SST_JSON_H

#include <stddef.h>
#include <stdint.h>

/* The number of tests in every upstream file. */
#define SST_JSON_FULL_COUNT 10000u

/* Converts buf[0..len), the JSON for opcode. With first 0 the array must
   close after exactly SST_JSON_FULL_COUNT tests, with nothing but whitespace
   after it; with first N only the first N tests are converted and whatever
   follows is not read, so a byte prefix of an upstream file converts.

   On success returns 1, sets *out to a malloc'd chunk of *out_len bytes the
   caller frees, and *count to its number of tests. On failure returns 0 and
   writes "offset <n>: <reason>" into err (err_size bytes, NUL-terminated);
   *out is then NULL. */
int sst_json_convert(const uint8_t *buf, size_t len, uint8_t opcode, uint32_t first, uint8_t **out,
                     size_t *out_len, uint32_t *count, char *err, size_t err_size);

#endif
