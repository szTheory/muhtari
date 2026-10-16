/* A program that knows only the public header: it supplies its own allocator,
   creates an instance, runs one frame into heap buffers and destroys it.
   Exits 0 and prints one fixed line on success, 1 otherwise. */
#include <muhtari.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* The core has no default allocator; the caller passes one. */
static void *consumer_alloc(void *user, size_t size)
{
    (void)user;
    return malloc(size);
}

static void consumer_free(void *user, void *ptr, size_t size)
{
    (void)user;
    (void)size;
    free(ptr);
}

int main(void)
{
    muhtari_config cfg;
    muhtari_info info;
    muhtari_frame io;
    muhtari *m = NULL;
    uint8_t *video = NULL;
    int16_t *audio = NULL;
    muhtari_status st;
    int ok = 0;

    /* Zero every struct, then set its size, as the header requires. */
    memset(&cfg, 0, sizeof cfg);
    cfg.size = (uint32_t)sizeof cfg;
    cfg.allocator.alloc = consumer_alloc;
    cfg.allocator.free = consumer_free;
    st = muhtari_create(&cfg, &m);
    if (st != MUHTARI_STATUS_OK) {
        printf("muhtari_create: status %u\n", (unsigned)st);
        return 1;
    }

    memset(&info, 0, sizeof info);
    info.size = (uint32_t)sizeof info;
    st = muhtari_get_info(m, &info);
    if (st == MUHTARI_STATUS_OK) {
        video = (uint8_t *)malloc((size_t)info.width * info.height);
        audio = (int16_t *)malloc((size_t)info.max_audio_samples * sizeof *audio);
    }
    if (video != NULL && audio != NULL) {
        memset(&io, 0, sizeof io);
        io.size = (uint32_t)sizeof io;
        io.video = video;
        io.video_pitch = info.width;
        io.audio = audio;
        io.audio_capacity = info.max_audio_samples;
        st = muhtari_run_frame(m, &io);
        /* The first frame carries 800 samples. */
        ok = st == MUHTARI_STATUS_OK && io.audio_count == 800u;
        if (!ok) {
            printf("muhtari_run_frame: status %u, audio_count %u\n", (unsigned)st,
                   (unsigned)io.audio_count);
        }
    } else {
        printf("setup failed: status %u\n", (unsigned)st);
    }

    muhtari_destroy(m);
    free(video);
    free(audio);
    if (!ok) {
        return 1;
    }
    printf("consumer: one frame, 800 samples\n");
    return 0;
}
