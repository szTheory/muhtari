/* The muhtari-movie 1 reader: a whole file checked before its first frame
   runs (docs/runner.md, Movies). */
#ifndef MUHTARI_RUNNER_MOVIE_H
#define MUHTARI_RUNNER_MOVIE_H

#include <stddef.h>
#include <stdint.h>

#include "muhtari.h"

/* The limits every movie is held to. */
#define MUHTARI__MOVIE_BYTES_MAX (32u * 1024u * 1024u)
#define MUHTARI__MOVIE_FRAMES_MAX 1000000u
#define MUHTARI__MOVIE_LINE_MAX 64u
#define MUHTARI__MOVIE_HEADER_LINES_MAX 32u

/* The boot key's values. */
enum { MUHTARI__MOVIE_BOOT_FAST = 0, MUHTARI__MOVIE_BOOT_BIOS = 1 };

/* Each optional key has a has_ flag; devices are explicit joystick,
   unplugged, SaveKey or AtariVox selections. Save devices have empty frame fields. */
typedef struct muhtari__movie_header {
    uint32_t version;
    uint32_t device[2];
    uint8_t has_rom_sha256;
    uint8_t rom_sha256[32];
    uint8_t has_region;
    uint32_t region; /* MUHTARI_REGION_NTSC */
    uint8_t has_boot;
    uint32_t boot; /* MUHTARI__MOVIE_BOOT_* */
    uint8_t has_bios_sha256;
    uint8_t bios_sha256[32];
    uint8_t has_behaviour;
    uint32_t behaviour;
} muhtari__movie_header;

/* Where and why a movie was refused: line and column count from 1, and
   what and expected are static strings, printed as
   "LINE:COL: <what>, expected <expected>". */
typedef struct muhtari__movie_error {
    uint32_t line;
    uint32_t column;
    const char *what;
    const char *expected;
} muhtari__movie_error;

/* Checks the n bytes as a movie and fills *h. With frames NULL it only
   counts the frame lines into *count; otherwise it also writes up to cap
   frames, each with size, the header's devices, buttons and switches set.
   Returns 0, or -1 with *err filled. It allocates nothing and opens no
   file. */
int muhtari__movie_parse(const uint8_t *bytes, size_t n, muhtari__movie_header *h,
                         muhtari_input *frames, uint32_t cap, uint32_t *count,
                         muhtari__movie_error *err);

#endif /* MUHTARI_RUNNER_MOVIE_H */
