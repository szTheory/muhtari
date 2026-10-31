/* Synthetic in-process frontend: no platform/input-device APIs or content launch. */
#define _POSIX_C_SOURCE 200809L
#include "libretro.h"
#include "input_trace.h"
#include <dlfcn.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#define REQUIRE(x)                                                                                 \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "trace host line%d: %s\n", __LINE__, #x);                              \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
typedef struct {
    void (*set_environment)(retro_environment_t);
    void (*set_input_poll)(retro_input_poll_t);
    void (*set_input_state)(retro_input_state_t);
    void (*set_video_refresh)(retro_video_refresh_t);
    void (*set_audio_sample_batch)(retro_audio_sample_batch_t);
    void (*set_controller_port_device)(unsigned, unsigned);
    void (*init)(void), (*deinit)(void), (*run)(void);
    bool (*load_game)(const struct retro_game_info *);
} api;
typedef struct {
    uint64_t calls, video, audio, queries, polls, frames, samples;
} result;
static result observed;
static unsigned scenario, frame;
static void hash(uint64_t *h, uint64_t n)
{
    *h = (*h ^ n) * UINT64_C(1099511628211);
}
static bool environment(unsigned command, void *data)
{
    if (command == RETRO_ENVIRONMENT_SET_PIXEL_FORMAT)
        return *(enum retro_pixel_format *)data == RETRO_PIXEL_FORMAT_XRGB8888;
    return false;
}
static void poll(void)
{
    hash(&observed.calls, 1);
    observed.polls++;
}
static int16_t state(unsigned port, unsigned device, unsigned index, unsigned id)
{
    int16_t value = 0;
    if (scenario == 0 && port == 0 && (id == 4 || id == 0))
        value = frame < 2 ? 1 : frame < 4 ? 0 : -7;
    if (scenario == 1 && port == 1 && (id == 4 || id == 0))
        value = 1;
    if (scenario == 2 && port == 0 && id == RETRO_DEVICE_ID_JOYPAD_A)
        value = 1;
    hash(&observed.calls, 2);
    hash(&observed.calls, port);
    hash(&observed.calls, device);
    hash(&observed.calls, index);
    hash(&observed.calls, id);
    hash(&observed.calls, (uint16_t)value);
    observed.queries++;
    return value;
}
static void video(const void *pixels, unsigned width, unsigned height, size_t pitch)
{
    hash(&observed.calls, 3);
    hash(&observed.video, width);
    hash(&observed.video, height);
    for (unsigned y = 0; y < height; y++) {
        const uint8_t *row = (const uint8_t *)pixels + y * pitch;
        for (unsigned x = 0; x < width * 4u; x++)
            hash(&observed.video, row[x]);
    }
    observed.frames++;
}
static size_t audio(const int16_t *data, size_t count)
{
    hash(&observed.calls, 4);
    for (size_t i = 0; i < count * 2u; i++)
        hash(&observed.audio, (uint16_t)data[i]);
    observed.samples += count;
    return count;
}
static uint64_t nanoseconds(void)
{
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) != 0)
        abort();
    return (uint64_t)now.tv_sec * UINT64_C(1000000000) + (uint64_t)now.tv_nsec;
}
static int load(void *module, api *a)
{
#define LOAD(name)                                                                                 \
    do {                                                                                           \
        void *symbol = dlsym(module, "retro_" #name);                                              \
        REQUIRE(symbol != NULL);                                                                   \
        memcpy(&a->name, &symbol, sizeof a->name);                                                 \
    } while (0)
    LOAD(set_environment);
    LOAD(set_input_poll);
    LOAD(set_input_state);
    LOAD(set_video_refresh);
    LOAD(set_audio_sample_batch);
    LOAD(set_controller_port_device);
    LOAD(init);
    LOAD(deinit);
    LOAD(run);
    LOAD(load_game);
#undef LOAD
    return 0;
}
static int session(const char *module_path, const char *trace_path, result *out, uint64_t *elapsed)
{
    int fd = open(trace_path, O_CREAT | O_EXCL | O_RDWR, 0600);
    REQUIRE(fd >= 3);
    int child_fd = dup(fd);
    REQUIRE(child_fd >= 3);
    char text[32];
    snprintf(text, sizeof text, "%d", child_fd);
    REQUIRE(setenv("MUHTARI_INPUT_TRACE_FD", text, 1) == 0);
    void *module = dlopen(module_path, RTLD_NOW | RTLD_LOCAL);
    REQUIRE(module != NULL);
    api a;
    REQUIRE(load(module, &a) == 0);
    memset(&observed, 0, sizeof observed);
    a.set_environment(environment);
    a.set_input_poll(scenario == 3 || scenario == 7 ? NULL : poll);
    a.set_input_state(scenario == 3 ? NULL : state);
    a.set_video_refresh(video);
    a.set_audio_sample_batch(audio);
    a.init();
    if (scenario != 4) {
        REQUIRE(a.load_game(NULL));
        if (scenario == 5)
            a.set_controller_port_device(0, RETRO_DEVICE_NONE);
    }
    uint64_t start = nanoseconds();
    for (frame = 0; frame < (scenario == 6 ? 0u : 6u); frame++) {
        /* First completed nonnull poll occurs after three already-run frames. */
        if (scenario == 7 && frame == 3)
            a.set_input_poll(poll);
        a.run();
    }
    *elapsed = nanoseconds() - start;
    a.deinit();
    *out = observed;
    REQUIRE(dlclose(module) == 0);
    if (fcntl(child_fd, F_GETFD) >= 0)
        close(child_fd);
    close(fd);
    REQUIRE(unsetenv("MUHTARI_INPUT_TRACE_FD") == 0);
    return 0;
}
static int oracle(const it_trace *t)
{
    REQUIRE(t->status == IT_COMPLETE && t->clock_ok);
    unsigned polls = 0, states = 0, unavailable = 0, held = 0, released = 0, changed = 0;
    for (uint32_t i = 0; i < t->count; i++) {
        const it_record *r = &t->records[i];
        if (r->type == IT_POLL) {
            REQUIRE(r->a == (scenario == 3 || (scenario == 7 && polls == 0) ? 0u : 1u));
            polls++;
        }
        if (r->type == IT_STATE) {
            states++;
            REQUIRE(r->c == 0 && (r->a == 4 || r->a == 0));
            if (r->b == 1)
                held++;
            else if (r->b == 0)
                released++;
            else if (r->b == (uint16_t)-7)
                changed++;
            else
                REQUIRE(0);
        }
        if (r->type == IT_UNAVAILABLE) {
            REQUIRE(r->a == (scenario == 3 ? 2u : scenario == 4 ? 1u : 3u));
            unavailable++;
        }
    }
    REQUIRE(polls == (scenario == 6 ? 0u : scenario == 7 ? 2u : 1u));
    REQUIRE(unavailable == (scenario >= 3 && scenario <= 5 ? 1u : 0u));
    REQUIRE(states == (scenario == 0                                     ? 6u
                       : scenario == 1 || scenario == 2 || scenario == 7 ? 2u
                                                                         : 0u));
    REQUIRE(held == (scenario == 0 ? 2u : 0u) && changed == held);
    REQUIRE(released == (states ? 2u : 0u));
    return 0;
}
int main(int argc, char **argv)
{
    REQUIRE(argc == 4);
    uint64_t normal_ns = 0, trace_ns = 0;
    for (scenario = 0; scenario < 8; scenario++) {
        char normal_path[4096], trace_path[4096];
        snprintf(normal_path, sizeof normal_path, "%s/normal-%u.trace", argv[3], scenario);
        snprintf(trace_path, sizeof trace_path, "%s/module-%u.trace", argv[3], scenario);
        unlink(normal_path);
        unlink(trace_path);
        result normal, diagnostic;
        uint64_t elapsed;
        REQUIRE(session(argv[1], normal_path, &normal, &elapsed) == 0);
        normal_ns += elapsed;
        REQUIRE(session(argv[2], trace_path, &diagnostic, &elapsed) == 0);
        trace_ns += elapsed;
        REQUIRE(memcmp(&normal, &diagnostic, sizeof normal) == 0);
        REQUIRE(normal.polls == (scenario == 3 || scenario == 6 ? 0u : scenario == 7 ? 3u : 6u));
        REQUIRE(normal.frames == (scenario == 4 || scenario == 6 ? 0u : 6u));
        if (normal.frames)
            REQUIRE(normal.samples > 0);
        it_trace t;
        it_read(normal_path, IT_CORE, &t);
        REQUIRE(t.status == IT_EMPTY);
        it_read(trace_path, IT_CORE, &t);
        REQUIRE(oracle(&t) == 0);
        unlink(normal_path);
        unlink(trace_path);
    }
    printf("same callbacks/order/return-values/frame/audio; normal_ns=%llu instrumented_ns=%llu "
           "delta_ns=%lld (local elapsed only)\n",
           (unsigned long long)normal_ns, (unsigned long long)trace_ns,
           (long long)trace_ns - (long long)normal_ns);
    return 0;
}
