/* Declarations shared by the core's source files. Not installed. */
#ifndef MUHTARI_INTERNAL_H
#define MUHTARI_INTERNAL_H

#include <stddef.h>
#include <stdint.h>

#include "muhtari.h"

/* The NTSC frame the core emits: 320 pixels by 242 rows, frame rows 0-241
   being rasters 16-257 (A7800-MARIA section 6). */
#define MUHTARI__WIDTH 320u
#define MUHTARI__HEIGHT_NTSC 242u

/* One NTSC frame is 263 lines of 454 ticks, 119,402 ticks. At 48 kHz that is
   88 samples per 13,125 ticks, 800 or 801 per frame (A7800-AUDIO, timing
   table). */
#define MUHTARI__TICKS_PER_FRAME_NTSC 119402u
#define MUHTARI__SAMPLES_PER_PERIOD 88u
#define MUHTARI__TICKS_PER_PERIOD 13125u
#define MUHTARI__MAX_AUDIO_SAMPLES 801u

/* The bounds of muhtari_frame.video_pitch. The cap keeps row offsets inside a
   32-bit size_t. */
#define MUHTARI__PITCH_MIN 320u
#define MUHTARI__PITCH_MAX 65536u

struct muhtari {
    muhtari_allocator allocator; /* the caller's, copied at create */
    uint32_t audio_rem;          /* ticks carried into the next frame's sample count */
};

/* The size of each struct as first released: the end of its last field. A
   caller's size tag below this is rejected. */
#define MUHTARI__CONFIG_SIZE_FIRST                                                                 \
    ((uint32_t)(offsetof(muhtari_config, allocator) + sizeof(muhtari_allocator)))
#define MUHTARI__VERSIONS_SIZE_FIRST                                                               \
    ((uint32_t)(offsetof(muhtari_versions, behaviour_revision) + sizeof(uint32_t)))
#define MUHTARI__INFO_SIZE_FIRST                                                                   \
    ((uint32_t)(offsetof(muhtari_info, max_audio_samples) + sizeof(uint32_t)))
#define MUHTARI__FRAME_SIZE_FIRST                                                                  \
    ((uint32_t)(offsetof(muhtari_frame, audio_count) + sizeof(uint32_t)))

/* The public layouts, pinned on every target. P is the pointer size; every
   field sits where the previous one ends, except that muhtari_config's
   allocator starts at P, so 64-bit targets carry 4 alignment bytes after
   its size field. */
#define MUHTARI__P sizeof(void *)
_Static_assert(offsetof(muhtari_allocator, alloc) == 0, "allocator.alloc");
_Static_assert(offsetof(muhtari_allocator, free) == MUHTARI__P, "allocator.free");
_Static_assert(offsetof(muhtari_allocator, user) == 2 * MUHTARI__P, "allocator.user");
_Static_assert(sizeof(muhtari_allocator) == 3 * MUHTARI__P, "allocator size");

_Static_assert(offsetof(muhtari_config, size) == 0, "config.size");
_Static_assert(offsetof(muhtari_config, allocator) == MUHTARI__P, "config.allocator");
_Static_assert(sizeof(muhtari_config) == 4 * MUHTARI__P, "config size");

_Static_assert(offsetof(muhtari_versions, size) == 0, "versions.size");
_Static_assert(offsetof(muhtari_versions, library_major) == 4, "versions.library_major");
_Static_assert(offsetof(muhtari_versions, library_minor) == 8, "versions.library_minor");
_Static_assert(offsetof(muhtari_versions, library_patch) == 12, "versions.library_patch");
_Static_assert(offsetof(muhtari_versions, api) == 16, "versions.api");
_Static_assert(offsetof(muhtari_versions, abi) == 20, "versions.abi");
_Static_assert(offsetof(muhtari_versions, behaviour_revision) == 24, "versions.behaviour_revision");
_Static_assert(sizeof(muhtari_versions) == 28, "versions size");

_Static_assert(offsetof(muhtari_info, size) == 0, "info.size");
_Static_assert(offsetof(muhtari_info, width) == 4, "info.width");
_Static_assert(offsetof(muhtari_info, height) == 8, "info.height");
_Static_assert(offsetof(muhtari_info, fps_num) == 12, "info.fps_num");
_Static_assert(offsetof(muhtari_info, fps_den) == 16, "info.fps_den");
_Static_assert(offsetof(muhtari_info, par_num) == 20, "info.par_num");
_Static_assert(offsetof(muhtari_info, par_den) == 24, "info.par_den");
_Static_assert(offsetof(muhtari_info, sample_rate_num) == 28, "info.sample_rate_num");
_Static_assert(offsetof(muhtari_info, sample_rate_den) == 32, "info.sample_rate_den");
_Static_assert(offsetof(muhtari_info, max_audio_samples) == 36, "info.max_audio_samples");
_Static_assert(sizeof(muhtari_info) == 40, "info size");

_Static_assert(offsetof(muhtari_frame, size) == 0, "frame.size");
_Static_assert(offsetof(muhtari_frame, video_pitch) == 4, "frame.video_pitch");
_Static_assert(offsetof(muhtari_frame, video) == 8, "frame.video");
_Static_assert(offsetof(muhtari_frame, audio) == 8 + MUHTARI__P, "frame.audio");
_Static_assert(offsetof(muhtari_frame, audio_capacity) == 8 + 2 * MUHTARI__P,
               "frame.audio_capacity");
_Static_assert(offsetof(muhtari_frame, audio_count) == 12 + 2 * MUHTARI__P, "frame.audio_count");
_Static_assert(sizeof(muhtari_frame) == 16 + 2 * MUHTARI__P, "frame size");

/* Returns OK when the size tag at s is at least first and every byte past
   ours is zero; ERR_STRUCT_SIZE otherwise. */
muhtari_status muhtari__check_size(const void *s, uint32_t first, uint32_t ours);

/* The test card's native colour at (x, y) in a frame h rows tall. */
uint8_t muhtari__testcard_pixel(uint32_t x, uint32_t y, uint32_t h);

#endif /* MUHTARI_INTERNAL_H */
