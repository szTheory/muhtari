/* Versioned regression evidence over actual built sample cartridges.
   Documentary chip and spectral tests establish meaning independently. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "internal.h"
#include "../../runner/sha256.h"
#include "../check.h"

static void *allocate(void *u, size_t n)
{
    (void)u;
    return malloc(n);
}
static void release(void *u, void *p, size_t n)
{
    (void)u;
    (void)n;
    free(p);
}
static muhtari *make(const uint8_t *rom, size_t size)
{
    muhtari_config c = {0};
    c.size = sizeof c;
    c.allocator.alloc = allocate;
    c.allocator.free = release;
    muhtari *m = NULL;
    CHECK_EQ_U64(muhtari_create(&c, &m), 0);
    muhtari_media d = {0};
    d.size = sizeof d;
    d.image = rom;
    d.image_size = size;
    CHECK_EQ_U64(muhtari_load(m, &d), 0);
    return m;
}
struct trace {
    muhtari__sha256 hash;
    uint64_t count, tick;
    uint8_t source;
};
static void raw(void *user, uint64_t tick, uint8_t source, uint16_t level)
{
    struct trace *t = user;
    CHECK(level <= 15u && source < 10u);
    if (t->count != 0u) {
        CHECK(tick >= t->tick);
        if (tick == t->tick)
            CHECK(source >= t->source);
    }
    t->tick = tick;
    t->source = source;
    t->count++;
    uint8_t b[11];
    for (unsigned j = 0; j < 8u; j++)
        b[j] = (uint8_t)(tick >> (8u * j));
    b[8] = source;
    b[9] = (uint8_t)level;
    b[10] = (uint8_t)(level >> 8);
    muhtari__sha256_update(&t->hash, b, sizeof b);
}
static void finish(muhtari__sha256 *h, char hex[65])
{
    uint8_t d[32];
    muhtari__sha256_final(h, d);
    muhtari__sha256_hex(d, hex);
}
static int valid_pin(const char *got, const char *pin)
{
    return strcmp(got, pin) == 0;
}
int main(int argc, char **argv)
{
    if (argc != 5 || (strcmp(argv[1], "--check") != 0 && strcmp(argv[1], "--measure") != 0))
        return 2;
    int measure = strcmp(argv[1], "--measure") == 0;
    FILE *f = fopen(argv[2], "rb");
    if (f == NULL)
        return 1;
    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        return 1;
    }
    long length = ftell(f);
    if (length <= 0 || length > 1048704) {
        fclose(f);
        return 1;
    }
    rewind(f);
    uint8_t *rom = malloc((size_t)length);
    if (rom == NULL) {
        fclose(f);
        return 1;
    }
    size_t size = fread(rom, 1, (size_t)length, f);
    int close_rc = fclose(f);
    if (size != (size_t)length || close_rc != 0) {
        free(rom);
        return 1;
    }
    muhtari *m = make(rom, size), *off = make(rom, size), *discard = make(rom, size);
    free(rom);
    struct trace t;
    memset(&t, 0, sizeof t);
    muhtari__sha256_init(&t.hash);
    CHECK_EQ_U64(muhtari__synth_observe(m, raw, &t), 0);
    muhtari__sha256 pcm;
    muhtari__sha256_init(&pcm);
    uint64_t samples = 0, nonzero = 0, peak = 0;
    int16_t first[54];
    for (unsigned frame = 0; frame < 600u; frame++) {
        int16_t audio[3][801];
        uint32_t count = 0;
        for (unsigned k = 0; k < 3u; k++) {
            muhtari *instance = k == 0u ? m : k == 1u ? off : discard;
            muhtari_input in = {0};
            in.size = sizeof in;
            in.buttons[0] = frame < 60u ? MUHTARI_BUTTON_1 | MUHTARI_BUTTON_2 : 0u;
            CHECK_EQ_U64(muhtari_set_input(instance, &in), 0);
            muhtari_frame io = {0};
            io.size = sizeof io;
            if (k != 2u || (frame & 1u) != 0u) {
                io.audio = audio[k];
                io.audio_capacity = 801;
            }
            CHECK_EQ_U64(muhtari_run_frame(instance, &io), 0);
            if (k == 0u)
                count = io.audio_count;
            else
                CHECK_EQ_U64(io.audio_count, count);
        }
        CHECK(memcmp(audio[0], audio[1], count * sizeof(int16_t)) == 0);
        if ((frame & 1u) != 0u)
            CHECK(memcmp(audio[0], audio[2], count * sizeof(int16_t)) == 0);
        CHECK(memcmp(&m->synth, &off->synth, sizeof m->synth) == 0);
        CHECK(memcmp(&m->synth, &discard->synth, sizeof m->synth) == 0);
        CHECK_EQ_U64(m->synth.clipped_samples, 0);
        CHECK(m->synth.pre_clip_peak <= 29204u);
        if (frame == 0u) {
            CHECK_EQ_U64(count, 54);
            memcpy(first, audio[0], sizeof first);
        }
        for (uint32_t j = 0; j < count; j++) {
            int32_t v = audio[0][j];
            uint64_t abs = (uint64_t)(v < 0 ? -v : v);
            if (abs > peak)
                peak = abs;
            nonzero += v != 0;
            uint16_t bits = (uint16_t)audio[0][j];
            uint8_t b[2] = {(uint8_t)bits, (uint8_t)(bits >> 8)};
            muhtari__sha256_update(&pcm, b, 2);
        }
        samples += count;
    }
    CHECK(nonzero > 0u && t.count > 0u);
    CHECK(peak <= 29204u);
    char rh[65], ph[65];
    finish(&t.hash, rh);
    finish(&pcm, ph);
    muhtari_versions v = {0};
    v.size = sizeof v;
    CHECK_EQ_U64(muhtari_get_versions(&v), 0);
    char row[400];
    snprintf(row, sizeof row, "%s 600 %llu 48000/1 %u exact-q23-16x64-dc20 %s %s", argv[3],
             (unsigned long long)samples, v.behaviour_revision, rh, ph);
    /* A judge that ignores either pin or identity must fail these controls. */
    CHECK(!valid_pin(row, "wrong"));
    char bad[400];
    memcpy(bad, row, strlen(row) + 1u);
    bad[strlen(bad) - 1u] = bad[strlen(bad) - 1u] == '0' ? '1' : '0';
    CHECK(!valid_pin(row, bad));
    if (!measure)
        CHECK(valid_pin(row, argv[4]));
    if (measure)
        printf("%s\n", row);
    else
        printf("audio_transitions_sha256 %s\naudio_pcm_sha256 %s\naudio_frames 600\naudio_samples "
               "%llu\naudio_rate 48000/1\naudio_behaviour_revision %u\naudio_synthesis "
               "exact-q23-16x64-dc20\n",
               rh, ph, (unsigned long long)samples, v.behaviour_revision);
    uint64_t raw_count = t.count, pre_peak = m->synth.pre_clip_peak;
    /* Power cycle resets raw/carry/fraction history but retains the host tap. */
    CHECK_EQ_U64(muhtari_power_cycle(m), 0);
    CHECK(m->observer == raw);
    memset(&t, 0, sizeof t);
    muhtari__sha256_init(&t.hash);
    muhtari_input in = {0};
    in.size = sizeof in;
    in.buttons[0] = MUHTARI_BUTTON_1 | MUHTARI_BUTTON_2;
    CHECK_EQ_U64(muhtari_set_input(m, &in), 0);
    int16_t reset[801];
    muhtari_frame io = {0};
    io.size = sizeof io;
    io.audio = reset;
    io.audio_capacity = 801;
    CHECK_EQ_U64(muhtari_run_frame(m, &io), 0);
    CHECK_EQ_U64(io.audio_count, 54);
    CHECK(memcmp(reset, first, sizeof first) == 0);
    fprintf(stderr, "%s raw=%llu peak=%llu pre_clip=%llu clips=%llu\n", argv[3],
            (unsigned long long)raw_count, (unsigned long long)peak, (unsigned long long)pre_peak,
            (unsigned long long)m->synth.clipped_samples);
    muhtari_destroy(m);
    muhtari_destroy(off);
    muhtari_destroy(discard);
    CHECK_DONE();
}
