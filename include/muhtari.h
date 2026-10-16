/* muhtari: an Atari 7800 emulator core. Behaviour is described in docs/. */
#ifndef MUHTARI_H
#define MUHTARI_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MUHTARI_API_VERSION 1

typedef uint32_t muhtari_status;
enum {
    MUHTARI_STATUS_OK = 0,
    MUHTARI_STATUS_ERR_ARGUMENT = 1,
    MUHTARI_STATUS_ERR_STRUCT_SIZE = 2,
    MUHTARI_STATUS_ERR_NO_MEMORY = 3,
    MUHTARI_STATUS_ERR_BUFFER_TOO_SMALL = 4
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
    uint32_t api;                /* 1 */
    uint32_t abi;                /* 1 */
    uint32_t behaviour_revision; /* 1 */
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
    uint32_t audio_count;    /* out: samples this frame (800 or 801) */
} muhtari_frame;

muhtari_status muhtari_get_versions(muhtari_versions *out);
muhtari_status muhtari_create(const muhtari_config *cfg, muhtari **out);
void muhtari_destroy(muhtari *m); /* NULL-safe */
muhtari_status muhtari_get_info(const muhtari *m, muhtari_info *out);
muhtari_status muhtari_run_frame(muhtari *m, muhtari_frame *io);

#ifdef __cplusplus
}
#endif

#endif /* MUHTARI_H */
