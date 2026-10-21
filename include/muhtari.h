/* muhtari: an Atari 7800 emulator core. Behaviour is described in docs/. */
#ifndef MUHTARI_H
#define MUHTARI_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MUHTARI_API_VERSION 3
#define MUHTARI_IMAGE_SIZE_MAX 1048704u /* 1 MiB of ROM plus a 128-byte header */

typedef uint32_t muhtari_status;
enum {
    MUHTARI_STATUS_OK = 0,
    MUHTARI_STATUS_ERR_ARGUMENT = 1,
    MUHTARI_STATUS_ERR_STRUCT_SIZE = 2,
    MUHTARI_STATUS_ERR_NO_MEMORY = 3,
    MUHTARI_STATUS_ERR_BUFFER_TOO_SMALL = 4,
    MUHTARI_STATUS_ERR_INVALID_IMAGE = 5,
    MUHTARI_STATUS_ERR_TRUNCATED_IMAGE = 6,
    MUHTARI_STATUS_ERR_UNSUPPORTED_SIZE = 7,
    MUHTARI_STATUS_ERR_UNSUPPORTED_2600 = 8,
    MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD = 9,
    MUHTARI_STATUS_ERR_BIOS_REJECTED_CART = 10, /* run_frame: the BIOS took its 2600 path */
    MUHTARI_STATUS_ERR_INVALID_BIOS = 11,       /* not 4,096 or 16,384 bytes, or a bad vector */
    MUHTARI_STATUS_ERR_UNSUPPORTED_REGION = 12, /* resolves to PAL */
    MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE = 13  /* an explicit device this version lacks */
};

typedef struct muhtari muhtari;

typedef struct muhtari_allocator {
    void *(*alloc)(void *user, size_t size); /* max_align_t-aligned */
    void (*free)(void *user, void *ptr, size_t size);
    void *user;
} muhtari_allocator;

typedef struct muhtari_config {
    uint32_t size;
    muhtari_allocator allocator; /* alloc and free required */
} muhtari_config;

typedef struct muhtari_versions {
    uint32_t size;
    uint32_t library_major, library_minor, library_patch;
    uint32_t api;                /* MUHTARI_API_VERSION */
    uint32_t abi;                /* 1 */
    uint32_t behaviour_revision; /* reported by muhtari_get_versions */
} muhtari_versions;

typedef struct muhtari_info {
    uint32_t size;
    uint32_t width, height;                    /* 320, 242 */
    uint32_t fps_num, fps_den;                 /* 39375000, 656711 */
    uint32_t par_num, par_den;                 /* 6, 7 */
    uint32_t sample_rate_num, sample_rate_den; /* 48000, 1 */
    uint32_t max_audio_samples;                /* per frame: 801 */
} muhtari_info;

typedef struct muhtari_frame {
    uint32_t size;
    uint32_t video_pitch;    /* in: bytes per row, 320..65536 */
    uint8_t *video;          /* in: NULL skips video */
    int16_t *audio;          /* in: NULL skips audio */
    uint32_t audio_capacity; /* in */
    uint32_t audio_count;    /* out: samples this frame, at most max_audio_samples */
} muhtari_frame;

muhtari_status muhtari_get_versions(muhtari_versions *out);
muhtari_status muhtari_create(const muhtari_config *cfg, muhtari **out);
void muhtari_destroy(muhtari *m); /* NULL-safe */
muhtari_status muhtari_get_info(const muhtari *m, muhtari_info *out);
muhtari_status muhtari_run_frame(muhtari *m, muhtari_frame *io);

enum { MUHTARI_KIND_7800 = 1, MUHTARI_KIND_2600 = 2 };
enum {
    MUHTARI_SOURCE_HEADER = 1,
    MUHTARI_SOURCE_HASH = 2,
    MUHTARI_SOURCE_FOOTER = 3,
    MUHTARI_SOURCE_SIZE = 4
};
enum {
    MUHTARI_BOARD_LINEAR = 1,
    MUHTARI_BOARD_SUPERGAME = 2,
    MUHTARI_BOARD_ACTIVISION = 3,
    MUHTARI_BOARD_ABSOLUTE = 4,
    MUHTARI_BOARD_FRACTALUS = 5,
    MUHTARI_BOARD_BANKSET = 6,
    MUHTARI_BOARD_SOUPER = 7
};
enum {
    MUHTARI_BOARD_ROM_AT_4000 = 1u,
    MUHTARI_BOARD_BANK6_AT_4000 = 2u,
    MUHTARI_BOARD_BANKED_RAM = 4u,
    MUHTARI_BOARD_HALT_BANKED_RAM = 8u
};
enum {
    MUHTARI_SOUND_POKEY_4000 = 1u,
    MUHTARI_SOUND_POKEY_450 = 2u,
    MUHTARI_SOUND_POKEY_440 = 4u,
    MUHTARI_SOUND_POKEY_800 = 8u,
    MUHTARI_SOUND_YM2151 = 16u,
    MUHTARI_SOUND_COVOX = 32u
};
enum { MUHTARI_DEVICE_HSC = 1u, MUHTARI_DEVICE_SAVEKEY = 2u, MUHTARI_DEVICE_XM = 4u };
enum { /* the A78 byte 55/56 codes */
       MUHTARI_PORT_NONE = 0,
       MUHTARI_PORT_PROLINE = 1,
       MUHTARI_PORT_LIGHTGUN = 2,
       MUHTARI_PORT_PADDLE = 3,
       MUHTARI_PORT_TRAKBALL = 4,
       MUHTARI_PORT_JOYSTICK = 5,
       MUHTARI_PORT_DRIVING = 6,
       MUHTARI_PORT_KEYPAD = 7,
       MUHTARI_PORT_ST_MOUSE = 8,
       MUHTARI_PORT_AMIGA_MOUSE = 9,
       MUHTARI_PORT_ATARIVOX = 10,
       MUHTARI_PORT_SNES2ATARI = 11,
       MUHTARI_PORT_MEGA7800 = 12,
       MUHTARI_PORT_UNPLUGGED = 256 /* outside the A78 byte range */
};
enum { MUHTARI_REGION_NTSC = 1, MUHTARI_REGION_PAL = 2 };
enum { MUHTARI_CART_HEADER = 1u, MUHTARI_CART_HASH_MATCH = 2u };
enum {
    MUHTARI_CART_WARN_HEADER_V4_IGNORED = 1u,
    MUHTARI_CART_WARN_HEADER_CORRECTED = 2u,
    MUHTARI_CART_WARN_VERSION_ABOVE_4 = 4u,
    MUHTARI_CART_WARN_SIZE_FIELD_MISMATCH = 8u,
    MUHTARI_CART_WARN_TITLE_NONPRINTABLE = 16u,
    MUHTARI_CART_WARN_UNKNOWN_PORT = 32u,
    MUHTARI_CART_WARN_LEGACY_SAVE_BIT = 64u,
    MUHTARI_CART_WARN_XM_VALUE = 128u,
    MUHTARI_CART_WARN_RESERVED_NONZERO = 256u,
    MUHTARI_CART_WARN_CART_TYPE_CONFLICT = 512u
};

typedef struct muhtari_cart_info {
    uint32_t size;
    uint32_t kind;
    uint32_t source;
    uint32_t flags;
    uint32_t warnings;
    uint32_t rom_size; /* bytes mapped, header excluded */
    uint32_t board;
    uint32_t board_options;
    uint32_t ram_size;
    uint32_t sound;
    uint32_t devices;
    uint32_t port1;
    uint32_t port2;
    uint32_t region;
    uint32_t a78_version;
    uint32_t a78_rom_size;
    uint32_t a78_cart_type;
    uint32_t a78_port1;
    uint32_t a78_port2;
    uint32_t a78_tv;
    uint32_t a78_save;
    uint32_t a78_xm;
    uint32_t a78_mapper;
    uint32_t a78_mapper_options;
    uint32_t a78_audio;
    uint32_t a78_irq;
    uint8_t title[36]; /* printable ASCII, NUL-terminated, zero-padded */
} muhtari_cart_info;

muhtari_status muhtari_probe(const void *image, size_t n, muhtari_cart_info *out);

typedef struct muhtari_media {
    uint32_t size;
    uint32_t region;   /* 0: the cartridge's; MUHTARI_REGION_* */
    const void *image; /* copied at load */
    size_t image_size;
    const void *bios; /* NULL: fast boot; copied at load */
    size_t bios_size;
} muhtari_media;

muhtari_status muhtari_load(muhtari *m, const muhtari_media *media);
muhtari_status muhtari_power_cycle(muhtari *m);

enum {
    MUHTARI_BUTTON_UP = 1u,
    MUHTARI_BUTTON_DOWN = 2u,
    MUHTARI_BUTTON_LEFT = 4u,
    MUHTARI_BUTTON_RIGHT = 8u,
    MUHTARI_BUTTON_1 = 16u,
    MUHTARI_BUTTON_2 = 32u
};
enum {
    MUHTARI_SWITCH_RESET = 1u,
    MUHTARI_SWITCH_SELECT = 2u,
    MUHTARI_SWITCH_PAUSE = 4u,
    MUHTARI_SWITCH_LEFT_DIFFICULTY_A = 8u,
    MUHTARI_SWITCH_RIGHT_DIFFICULTY_A = 16u
};

typedef struct muhtari_input {
    uint32_t size;
    uint32_t device[2];  /* 0: the cartridge's; MUHTARI_PORT_PROLINE, _JOYSTICK, _UNPLUGGED */
    uint32_t buttons[2]; /* MUHTARI_BUTTON_*, pressed = 1 */
    uint32_t switches;   /* MUHTARI_SWITCH_*: held = 1; a difficulty bit set = A */
} muhtari_input;

enum { MUHTARI_MEMORY_SYSTEM_RAM = 1, MUHTARI_MEMORY_RIOT_RAM = 2, MUHTARI_MEMORY_CART_RAM = 3 };

typedef struct muhtari_span {
    uint32_t size;
    uint8_t *data; /* until the next load or destroy; NULL when absent */
    size_t bytes;
} muhtari_span;

typedef struct muhtari_counters {
    uint32_t size;
    uint64_t frames;     /* since load or power cycle */
    uint64_t lag_frames; /* frames with no CPU read of SWCHA, SWCHB or INPT0-5 */
} muhtari_counters;

muhtari_status muhtari_set_input(muhtari *m, const muhtari_input *in);
muhtari_status muhtari_get_memory(muhtari *m, uint32_t kind, muhtari_span *out);
muhtari_status muhtari_get_counters(const muhtari *m, muhtari_counters *out);

#ifdef __cplusplus
}
#endif

#endif /* MUHTARI_H */
