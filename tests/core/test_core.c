/* The core library through its public header: muhtari-test-core <case>.
   Expected test-card values follow the card's rules as docs/ describes them,
   and the pinned hash was computed by an independent model of those rules,
   not by this code. */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "muhtari.h"
#include "sha256.h"
#include "../check.h"

#define WIDTH 320u
#define HEIGHT 242u
#define FRAME_BYTES (WIDTH * HEIGHT)
#define AUDIO_CAP 801u

/* SHA-256 of the NTSC test card's 77,440 native colour bytes. */
#define PINNED_HASH "9eb011e22feb3f48da2c0abf3dd8d5236d4d0f5c0e67a732eb2172259e0a05c4"

static uint8_t video[FRAME_BYTES];
static int16_t audio[AUDIO_CAP];

static void *test_alloc(void *user, size_t size)
{
    (void)user;
    return malloc(size);
}

static void test_free(void *user, void *ptr, size_t size)
{
    (void)user;
    (void)size;
    free(ptr);
}

static muhtari *make(void)
{
    muhtari_config cfg;
    memset(&cfg, 0, sizeof cfg);
    cfg.size = (uint32_t)sizeof cfg;
    cfg.allocator.alloc = test_alloc;
    cfg.allocator.free = test_free;
    muhtari *m = NULL;
    CHECK_EQ_U64(muhtari_create(&cfg, &m), MUHTARI_STATUS_OK);
    return m;
}

/* Fills *io in place after a memset, so no padding is left unspecified. */
static void frame_io(muhtari_frame *io, uint8_t *v, uint32_t pitch, int16_t *a, uint32_t cap)
{
    memset(io, 0, sizeof *io);
    io->size = (uint32_t)sizeof *io;
    io->video = v;
    io->video_pitch = pitch;
    io->audio = a;
    io->audio_capacity = cap;
}

static void hash_hex(const uint8_t *bytes, size_t len, char hex[65])
{
    muhtari__sha256 s;
    uint8_t digest[32];
    muhtari__sha256_init(&s);
    muhtari__sha256_update(&s, bytes, len);
    muhtari__sha256_final(&s, digest);
    muhtari__sha256_hex(digest, hex);
}

static void check_hash(const char *got, const char *want)
{
    if (strcmp(got, want) != 0) {
        fprintf(stderr, "hash: expected %s, got %s\n", want, got);
        check_failures++;
    }
}

/* Frame 1 of a new instance hashes to the pinned value. */
static void test_pinned_hash(void)
{
    muhtari *m = make();
    muhtari_frame io;
    frame_io(&io, video, WIDTH, audio, AUDIO_CAP);
    CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_OK);
    char hex[65];
    hash_hex(video, FRAME_BYTES, hex);
    check_hash(hex, PINNED_HASH);
    muhtari_destroy(m);
}

static const struct {
    const char *name;
    void (*run)(void);
} cases[] = {
    {"pinned_hash", test_pinned_hash},
};

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: muhtari-test-core <case>\n");
        return 2;
    }
    for (size_t i = 0; i < sizeof cases / sizeof cases[0]; i++) {
        if (strcmp(argv[1], cases[i].name) == 0) {
            cases[i].run();
            CHECK_DONE();
        }
    }
    fprintf(stderr, "muhtari-test-core: unknown case %s\n", argv[1]);
    return 2;
}
