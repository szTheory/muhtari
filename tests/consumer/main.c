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

static int consumer_ym(const char *path)
{
    uint8_t image[16512];
    FILE *file = fopen(path, "rb");
    if (file == NULL)
        return 1;
    size_t bytes = fread(image, 1u, sizeof image, file);
    int extra = fgetc(file);
    int read_error = ferror(file);
    int close_error = fclose(file);
    if (bytes != sizeof image || extra != EOF || read_error || close_error)
        return 1;
    muhtari_cart_info cart = {0};
    cart.size = (uint32_t)sizeof cart;
    if (muhtari_probe(image, bytes, &cart) != MUHTARI_STATUS_OK || cart.a78_xm != 1u ||
        cart.a78_audio != 8u || cart.a78_irq != 4u)
        return 1;
    for (unsigned pal = 0u; pal < 2u; pal++) {
        muhtari_config cfg = {0};
        cfg.size = (uint32_t)sizeof cfg;
        cfg.allocator.alloc = consumer_alloc;
        cfg.allocator.free = consumer_free;
        muhtari *m = NULL;
        if (muhtari_create(&cfg, &m) != MUHTARI_STATUS_OK)
            return 1;
        muhtari_media media = {0};
        media.size = (uint32_t)sizeof media;
        media.region = pal ? MUHTARI_REGION_PAL : MUHTARI_REGION_NTSC;
        media.image = image;
        media.image_size = bytes;
        media.expansion = MUHTARI_EXPANSION_XM;
        if (muhtari_load(m, &media) != MUHTARI_STATUS_OK) {
            muhtari_destroy(m);
            return 1;
        }
        muhtari_info info = {0};
        info.size = (uint32_t)sizeof info;
        if (muhtari_get_info(m, &info) != MUHTARI_STATUS_OK || info.width != 320u ||
            info.height != (pal ? 292u : 242u) || info.sample_rate_num != 48000u ||
            info.sample_rate_den != 1u || info.max_audio_samples > 962u) {
            muhtari_destroy(m);
            return 1;
        }
        size_t video_size = (size_t)info.width * info.height;
        uint8_t *video = malloc(video_size);
        uint8_t *last_video = malloc(video_size);
        size_t state_size = muhtari_state_size(m);
        uint8_t *saved = malloc(state_size);
        uint8_t *ending = malloc(state_size);
        uint8_t *replay = malloc(state_size + 16u);
        int16_t audio[962], last_audio[962];
        uint32_t last_count = 0u;
        int ok = video && last_video && saved && ending && replay;
        int minimum = 32767, maximum = -32768;
        size_t total = 0u;
        for (unsigned i = 0u; ok && i < 6u; i++) {
            muhtari_frame frame = {0};
            frame.size = (uint32_t)sizeof frame;
            frame.video = video;
            frame.video_pitch = 320u;
            frame.audio = audio;
            frame.audio_capacity = 962u;
            ok = muhtari_run_frame(m, &frame) == MUHTARI_STATUS_OK;
            if (!ok)
                break;
            ok = i == 0u ? frame.audio_count == (pal ? 960u : 54u)
                 : pal   ? frame.audio_count == 961u || frame.audio_count == 962u
                         : frame.audio_count == 800u || frame.audio_count == 801u;
            if (!ok)
                break;
            total += frame.audio_count;
            for (uint32_t j = 0u; j < frame.audio_count; j++) {
                if (audio[j] < minimum)
                    minimum = audio[j];
                if (audio[j] > maximum)
                    maximum = audio[j];
            }
            if (i == 1u)
                ok = ok && muhtari_save_state(m, saved, state_size) == MUHTARI_STATUS_OK;
            if (i == 5u) {
                memcpy(last_video, video, video_size);
                memcpy(last_audio, audio, frame.audio_count * sizeof audio[0]);
                last_count = frame.audio_count;
            }
        }
        muhtari_span ram = {0};
        ram.size = (uint32_t)sizeof ram;
        ok = ok && total == (pal ? 5768u : 4057u) && minimum == (pal ? -4742 : -4741) &&
             maximum == (pal ? 4582 : 4577) &&
             muhtari_get_memory(m, MUHTARI_MEMORY_SYSTEM_RAM, &ram) == MUHTARI_STATUS_OK &&
             ram.bytes >= 0xb00u && ram.data[0xa00u] == 128u && ram.data[0xa01u] == 1u &&
             ram.data[0xa02u] != 0u && ram.data[0xa03u] == 2u &&
             muhtari_save_state(m, ending, state_size) == MUHTARI_STATUS_OK &&
             muhtari_load_state(m, saved, state_size) == MUHTARI_STATUS_OK;
        for (unsigned i = 0u; ok && i < 4u; i++) {
            muhtari_frame frame = {0};
            frame.size = (uint32_t)sizeof frame;
            frame.video = video;
            frame.video_pitch = 320u;
            frame.audio = audio;
            frame.audio_capacity = 962u;
            ok = muhtari_run_frame(m, &frame) == MUHTARI_STATUS_OK;
            if (i == 3u)
                ok = ok && frame.audio_count == last_count &&
                     memcmp(video, last_video, video_size) == 0 &&
                     memcmp(audio, last_audio, last_count * sizeof audio[0]) == 0;
        }
        if (ok) {
            memset(replay, 0xa5, state_size + 16u);
            ok = muhtari_save_state(m, replay, state_size + 16u) == MUHTARI_STATUS_OK &&
                 memcmp(ending, replay, state_size) == 0;
            for (unsigned i = 0u; i < 16u; i++)
                ok = ok && replay[state_size + i] == 0xa5u;
            replay[24] ^= 1u;
            ok = ok && muhtari_load_state(m, replay, state_size) != MUHTARI_STATUS_OK &&
                 muhtari_save_state(m, replay, state_size) == MUHTARI_STATUS_OK &&
                 memcmp(ending, replay, state_size) == 0;
        }
        free(video);
        free(last_video);
        free(saved);
        free(ending);
        free(replay);
        muhtari_destroy(m);
        if (!ok)
            return 1;
    }
    puts("consumer: YM NTSC and PAL numeric, state and buffer checks");
    return 0;
}

int main(int argc, char **argv)
{
    if (argc == 3 && strcmp(argv[1], "--ym-fixture") == 0)
        return consumer_ym(argv[2]);
    if (argc != 1)
        return 2;
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
