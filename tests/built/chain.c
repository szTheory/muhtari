/* muhtari-test-chain FILE FRAMES: loads FILE with muhtari_load (fast boot),
   runs FRAMES frames and prints the library's behaviour revision and the
   SHA-256 chain of the frames,
     h_0 = 32 zero bytes, h_i = SHA256(h_{i-1} || frame_i),
   where frame_i is the 320 x 242 native colour bytes of frame i. The chain
   binds frame order, so two swapped frames change it. These historical
   pins describe the no-EEPROM topology: explicit unplugged port choices
   suppress newly implemented metadata save hints. Automatic attachment
   and enabled EEPROM execution are checked by the serial device tests.
   muhtari-run
   --hash-frame finds the first frame that differs. Exits 0, 1 when the file
   cannot be read, loaded or run, and 2 on a usage error. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "muhtari.h"
#include "sha256.h"

#define WIDTH 320u
#define HEIGHT 242u
#define AUDIO_CAPACITY 801u

static void *chain_alloc(void *user, size_t size)
{
    (void)user;
    return malloc(size);
}

static void chain_free(void *user, void *ptr, size_t size)
{
    (void)user;
    (void)size;
    free(ptr);
}

/* Reads the whole file into a malloc'd buffer. Returns NULL after a message. */
static uint8_t *read_file(const char *path, size_t *n)
{
    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        fprintf(stderr, "muhtari-test-chain: cannot open %s\n", path);
        return NULL;
    }
    uint8_t *buf = (uint8_t *)malloc(MUHTARI_IMAGE_SIZE_MAX + 1u);
    size_t got = buf != NULL ? fread(buf, 1u, MUHTARI_IMAGE_SIZE_MAX + 1u, f) : 0u;
    int failed = buf == NULL || ferror(f) || got > MUHTARI_IMAGE_SIZE_MAX;
    fclose(f);
    if (failed) {
        fprintf(stderr, "muhtari-test-chain: cannot read %s\n", path);
        free(buf);
        return NULL;
    }
    *n = got;
    return buf;
}

/* Parses a decimal frame count from 1 to 1,000,000. Returns 0 otherwise. */
static uint32_t parse_frames(const char *text)
{
    uint32_t v = 0;
    if (*text == '\0') {
        return 0;
    }
    for (const char *p = text; *p != '\0'; p++) {
        if (*p < '0' || *p > '9' || v > 100000u) {
            return 0;
        }
        v = v * 10u + (uint32_t)(*p - '0');
    }
    return v <= 1000000u ? v : 0u;
}

static int run_chain(muhtari *m, uint32_t frames, uint8_t *video, int16_t *audio)
{
    uint8_t h[32];
    memset(h, 0, sizeof h);
    for (uint32_t f = 1; f <= frames; f++) {
        muhtari_frame io;
        memset(&io, 0, sizeof io);
        io.size = (uint32_t)sizeof io;
        io.video = video;
        io.video_pitch = WIDTH;
        io.audio = audio;
        io.audio_capacity = AUDIO_CAPACITY;
        muhtari_status st = muhtari_run_frame(m, &io);
        if (st != MUHTARI_STATUS_OK) {
            fprintf(stderr, "muhtari-test-chain: frame %lu failed with status %lu\n",
                    (unsigned long)f, (unsigned long)st);
            return 1;
        }
        muhtari__sha256 s;
        muhtari__sha256_init(&s);
        muhtari__sha256_update(&s, h, sizeof h);
        muhtari__sha256_update(&s, video, (size_t)WIDTH * HEIGHT);
        muhtari__sha256_final(&s, h);
    }
    char hex[65];
    muhtari__sha256_hex(h, hex);
    printf("chain %s\n", hex);
    return 0;
}

int main(int argc, char **argv)
{
    uint32_t frames = argc == 3 ? parse_frames(argv[2]) : 0u;
    if (frames == 0u) {
        fprintf(stderr, "usage: muhtari-test-chain FILE FRAMES (1 <= FRAMES <= 1000000)\n");
        return 2;
    }
    muhtari_versions v;
    memset(&v, 0, sizeof v);
    v.size = (uint32_t)sizeof v;
    if (muhtari_get_versions(&v) != MUHTARI_STATUS_OK) {
        fprintf(stderr, "muhtari-test-chain: muhtari_get_versions failed\n");
        return 1;
    }
    printf("behaviour_revision %lu\n", (unsigned long)v.behaviour_revision);

    size_t n = 0;
    uint8_t *image = read_file(argv[1], &n);
    /* Frame and audio buffers on the heap: wasm32's stack is 64 KiB. */
    uint8_t *video = (uint8_t *)malloc((size_t)WIDTH * HEIGHT);
    int16_t *audio = (int16_t *)malloc(AUDIO_CAPACITY * sizeof *audio);
    muhtari *m = NULL;
    int status = 1;
    if (image != NULL && video != NULL && audio != NULL) {
        muhtari_config cfg;
        memset(&cfg, 0, sizeof cfg);
        cfg.size = (uint32_t)sizeof cfg;
        cfg.allocator.alloc = chain_alloc;
        cfg.allocator.free = chain_free;
        muhtari_media media;
        memset(&media, 0, sizeof media);
        media.size = (uint32_t)sizeof media;
        media.image = image;
        media.image_size = n;
        media.port[0] = MUHTARI_PORT_UNPLUGGED;
        media.port[1] = MUHTARI_PORT_UNPLUGGED;
        muhtari_status st = muhtari_create(&cfg, &m);
        if (st == MUHTARI_STATUS_OK) {
            st = muhtari_load(m, &media);
        }
        if (st == MUHTARI_STATUS_OK) {
            status = run_chain(m, frames, video, audio);
        } else {
            fprintf(stderr, "muhtari-test-chain: %s: create or load failed with status %lu\n",
                    argv[1], (unsigned long)st);
        }
    } else if (image != NULL) {
        fprintf(stderr, "muhtari-test-chain: out of memory\n");
    }
    muhtari_destroy(m);
    free(image);
    free(video);
    free(audio);
    return status;
}
