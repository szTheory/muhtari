/* API version 1, as released in 0.1.0. FROZEN: once released this file never
   changes. It calls every function, assigns every field of every struct and
   checks every constant of version 1, so a rename, a removal or a changed
   value fails to build or fails here. Each later API version adds its own
   file, v2.c and so on, beside this one. */
#include <stdlib.h>
#include <string.h>

#include "muhtari.h"
#include "../check.h"

static void *v1_alloc(void *user, size_t size)
{
    (void)user;
    return malloc(size);
}

static void v1_free(void *user, void *ptr, size_t size)
{
    (void)user;
    (void)size;
    free(ptr);
}

int main(void)
{
    CHECK_EQ_U64(MUHTARI_API_VERSION, 1);
    CHECK_EQ_U64(MUHTARI_STATUS_OK, 0);
    CHECK_EQ_U64(MUHTARI_STATUS_ERR_ARGUMENT, 1);
    CHECK_EQ_U64(MUHTARI_STATUS_ERR_STRUCT_SIZE, 2);
    CHECK_EQ_U64(MUHTARI_STATUS_ERR_NO_MEMORY, 3);
    CHECK_EQ_U64(MUHTARI_STATUS_ERR_BUFFER_TOO_SMALL, 4);
    CHECK_EQ_U64(sizeof(muhtari_status), 4);

    muhtari_versions v;
    memset(&v, 0, sizeof v);
    v.size = (uint32_t)sizeof v;
    v.library_major = 0u;
    v.library_minor = 0u;
    v.library_patch = 0u;
    v.api = 0u;
    v.abi = 0u;
    v.behaviour_revision = 0u;
    CHECK_EQ_U64(muhtari_get_versions(&v), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(v.api, 1);
    CHECK(v.abi != 0u);
    CHECK(v.behaviour_revision != 0u);

    muhtari_allocator allocator;
    memset(&allocator, 0, sizeof allocator);
    allocator.alloc = v1_alloc;
    allocator.free = v1_free;
    allocator.user = NULL;
    muhtari_config cfg;
    memset(&cfg, 0, sizeof cfg);
    cfg.size = (uint32_t)sizeof cfg;
    cfg.allocator = allocator;
    muhtari *m = NULL;
    CHECK_EQ_U64(muhtari_create(&cfg, &m), MUHTARI_STATUS_OK);

    muhtari_info info;
    memset(&info, 0, sizeof info);
    info.size = (uint32_t)sizeof info;
    info.width = 0u;
    info.height = 0u;
    info.fps_num = 0u;
    info.fps_den = 0u;
    info.par_num = 0u;
    info.par_den = 0u;
    info.sample_rate_num = 0u;
    info.sample_rate_den = 0u;
    info.max_audio_samples = 0u;
    CHECK_EQ_U64(muhtari_get_info(m, &info), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(info.width, 320);
    CHECK_EQ_U64(info.height, 242);
    CHECK_EQ_U64(info.max_audio_samples, 801);

    uint8_t *video = (uint8_t *)malloc((size_t)info.width * info.height);
    int16_t *audio = (int16_t *)malloc(info.max_audio_samples * sizeof *audio);
    CHECK(video != NULL && audio != NULL);
    muhtari_frame io;
    memset(&io, 0, sizeof io);
    io.size = (uint32_t)sizeof io;
    io.video_pitch = info.width;
    io.video = video;
    io.audio = audio;
    io.audio_capacity = 1u;
    io.audio_count = 0u;
    CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_ERR_BUFFER_TOO_SMALL);
    io.audio_capacity = info.max_audio_samples;
    CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_OK);
    CHECK(io.audio_count == 800u || io.audio_count == 801u);

    free(video);
    free(audio);
    muhtari_destroy(m);
    muhtari_destroy(NULL);
    CHECK_DONE();
}
