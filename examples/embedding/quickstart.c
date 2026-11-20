/* A ROM-free first frame using only the installed public API. */
#include <muhtari.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static void *quickstart_alloc(void *user, size_t size)
{
    (void)user;
    return malloc(size);
}

static void quickstart_free(void *user, void *ptr, size_t size)
{
    (void)user;
    (void)size;
    free(ptr);
}

int main(void)
{
    muhtari_config config = {0};
    muhtari_info info = {0};
    muhtari_frame frame = {0};
    muhtari *instance = NULL;
    uint8_t *video = NULL;
    int16_t *audio = NULL;
    muhtari_status status;
    int result = EXIT_FAILURE;

    config.size = (uint32_t)sizeof config;
    config.allocator.alloc = quickstart_alloc;
    config.allocator.free = quickstart_free;
    status = muhtari_create(&config, &instance);
    if (status != MUHTARI_STATUS_OK) {
        fprintf(stderr, "muhtari_create failed with status %u\n", (unsigned)status);
        goto cleanup;
    }

    info.size = (uint32_t)sizeof info;
    status = muhtari_get_info(instance, &info);
    if (status != MUHTARI_STATUS_OK) {
        fprintf(stderr, "muhtari_get_info failed with status %u\n", (unsigned)status);
        goto cleanup;
    }
    if (info.width == 0u || info.height == 0u || info.max_audio_samples == 0u ||
        (size_t)info.width > SIZE_MAX / info.height ||
        (size_t)info.max_audio_samples > SIZE_MAX / sizeof *audio) {
        fputs("muhtari_get_info returned invalid buffer sizes\n", stderr);
        goto cleanup;
    }

    video = (uint8_t *)malloc((size_t)info.width * info.height);
    audio = (int16_t *)malloc((size_t)info.max_audio_samples * sizeof *audio);
    if (video == NULL || audio == NULL) {
        fputs("could not allocate frame buffers\n", stderr);
        goto cleanup;
    }

    frame.size = (uint32_t)sizeof frame;
    frame.video = video;
    frame.video_pitch = info.width;
    frame.audio = audio;
    frame.audio_capacity = info.max_audio_samples;
    status = muhtari_run_frame(instance, &frame);
    if (status != MUHTARI_STATUS_OK) {
        fprintf(stderr, "muhtari_run_frame failed with status %u\n", (unsigned)status);
        goto cleanup;
    }
    if (frame.audio_count != 800u) {
        fprintf(stderr, "expected 800 audio samples, got %u\n", (unsigned)frame.audio_count);
        goto cleanup;
    }

    puts("quickstart: one frame, 800 samples");
    result = EXIT_SUCCESS;

cleanup:
    free(audio);
    free(video);
    muhtari_destroy(instance);
    return result;
}
