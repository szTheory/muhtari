/* Frozen API-4 declarations: deliberately independent of the current header.
   This executable links the current archive with an actual old media object. */
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../check.h"

typedef uint32_t muhtari_status;
typedef struct muhtari muhtari;
typedef struct muhtari_allocator {
    void *(*alloc)(void *user, size_t size);
    void (*free)(void *user, void *ptr, size_t size);
    void *user;
} muhtari_allocator;
typedef struct muhtari_config {
    uint32_t size;
    muhtari_allocator allocator;
} muhtari_config;
typedef struct muhtari_versions {
    uint32_t size;
    uint32_t library_major, library_minor, library_patch;
    uint32_t api, abi, behaviour_revision;
} muhtari_versions;
typedef struct muhtari_media {
    uint32_t size, region;
    const void *image;
    size_t image_size;
    const void *bios;
    size_t bios_size;
} muhtari_media;
typedef struct muhtari_frame {
    uint32_t size, video_pitch;
    uint8_t *video;
    int16_t *audio;
    uint32_t audio_capacity, audio_count;
} muhtari_frame;
muhtari_status muhtari_create(const muhtari_config *, muhtari **);
void muhtari_destroy(muhtari *);
muhtari_status muhtari_load(muhtari *, const muhtari_media *);
muhtari_status muhtari_run_frame(muhtari *, muhtari_frame *);
muhtari_status muhtari_get_versions(muhtari_versions *);

static void *old_alloc(void *user, size_t n)
{
    (void)user;
    return malloc(n);
}
static void old_free(void *user, void *p, size_t n)
{
    (void)user;
    (void)n;
    free(p);
}

int main(void)
{
    muhtari_config cfg;
    memset(&cfg, 0, sizeof cfg);
    cfg.size = (uint32_t)sizeof cfg;
    cfg.allocator.alloc = old_alloc;
    cfg.allocator.free = old_free;
    muhtari *m = NULL;
    CHECK_EQ_U64(muhtari_create(&cfg, &m), 0);
    uint8_t image[128 + 4096];
    memset(image, 0, sizeof image);
    image[0] = 3u;
    memcpy(image + 1, "ATARI7800", 9);
    image[51] = 0x10u;
    image[55] = 1u;
    image[56] = 1u;
    memset(image + 128, 0xEA, 4096);
    image[128] = 0x4Cu;
    image[129] = 0u;
    image[130] = 0xF0u;
    image[128 + 4092] = 0u;
    image[128 + 4093] = 0xF0u;
    uint32_t sizes[] = {(uint32_t)sizeof(muhtari_media),
                        (uint32_t)(offsetof(muhtari_media, bios_size) + sizeof(size_t))};
    for (unsigned i = 0; i < 2u; i++) {
        struct {
            muhtari_media media;
            uint8_t canary[32];
        } old;
        memset(&old, 0, sizeof old);
        memset(old.canary, 0xA5, sizeof old.canary);
        old.media.size = sizes[i];
        old.media.image = image;
        old.media.image_size = sizeof image;
        CHECK_EQ_U64(muhtari_load(m, &old.media), 0);
        for (unsigned k = 0; k < sizeof old.canary; k++) {
            CHECK_EQ_U64(old.canary[k], 0xA5);
        }
        muhtari_frame frame;
        memset(&frame, 0, sizeof frame);
        frame.size = (uint32_t)sizeof frame;
        CHECK_EQ_U64(muhtari_run_frame(m, &frame), 0);
    }
    muhtari_versions versions;
    memset(&versions, 0, sizeof versions);
    versions.size = (uint32_t)sizeof versions;
    CHECK_EQ_U64(muhtari_get_versions(&versions), 0);
    CHECK(versions.api >= 4u);
    CHECK_EQ_U64(versions.abi, 1);
    muhtari_destroy(m);
    CHECK_DONE();
}
