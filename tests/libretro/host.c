/* Loads the built libretro module at run time and calls it in the order a
 * frontend such as RetroArch does, then compares every frame it receives
 * with the library's own frame through the same palette.
 *
 *   muhtari-test-libretro-host <module> xrgb8888|rgb565
 *
 * Pixels are read as native-endian integers, so a swapped channel order, 555
 * for 565, or a swapped byte order fails here. In rgb565 mode the host
 * registers no batch audio callback, so the single-sample path runs. */
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../check.h"
#include "libretro.h"
#include "muhtari.h"
#include "palette.h"

#define W 320u
#define H 242u
#define RUNS 3u

/* a == b as doubles, printed with every digit. */
#define CHECK_EQ_DBL(a, b)                                                                         \
    do {                                                                                           \
        double check_a_ = (double)(a);                                                             \
        double check_b_ = (double)(b);                                                             \
        if (check_a_ != check_b_) {                                                                \
            fprintf(stderr, "%s:%d: %s == %s failed: %.17g != %.17g\n", __FILE__, __LINE__, #a,   \
                    #b, check_a_, check_b_);                                                       \
            check_failures++;                                                                      \
        }                                                                                          \
    } while (0)

/* ---- module loading ---- */

#ifdef _WIN32
static HMODULE module;
static int module_open(const char *path)
{
    module = LoadLibraryA(path);
    return module != NULL;
}
static void *module_sym(const char *name)
{
    FARPROC f = GetProcAddress(module, name);
    void *p = NULL;
    memcpy(&p, &f, sizeof p);
    return p;
}
static void module_close(void)
{
    FreeLibrary(module);
}
#else
static void *module;
static int module_open(const char *path)
{
    module = dlopen(path, RTLD_NOW);
    if (module == NULL) {
        fprintf(stderr, "dlopen: %s\n", dlerror());
    }
    return module != NULL;
}
static void *module_sym(const char *name)
{
    return dlsym(module, name);
}
static void module_close(void)
{
    (void)dlclose(module);
}
#endif

/* The 25 entry points, typed as libretro.h declares them (L7511-7840). */
static void(RETRO_CALLCONV *p_set_environment)(retro_environment_t);
static void(RETRO_CALLCONV *p_set_video_refresh)(retro_video_refresh_t);
static void(RETRO_CALLCONV *p_set_audio_sample)(retro_audio_sample_t);
static void(RETRO_CALLCONV *p_set_audio_sample_batch)(retro_audio_sample_batch_t);
static void(RETRO_CALLCONV *p_set_input_poll)(retro_input_poll_t);
static void(RETRO_CALLCONV *p_set_input_state)(retro_input_state_t);
static void(RETRO_CALLCONV *p_init)(void);
static void(RETRO_CALLCONV *p_deinit)(void);
static unsigned(RETRO_CALLCONV *p_api_version)(void);
static void(RETRO_CALLCONV *p_get_system_info)(struct retro_system_info *);
static void(RETRO_CALLCONV *p_get_system_av_info)(struct retro_system_av_info *);
static void(RETRO_CALLCONV *p_set_controller_port_device)(unsigned, unsigned);
static void(RETRO_CALLCONV *p_reset)(void);
static void(RETRO_CALLCONV *p_run)(void);
static size_t(RETRO_CALLCONV *p_serialize_size)(void);
static bool(RETRO_CALLCONV *p_serialize)(void *, size_t);
static bool(RETRO_CALLCONV *p_unserialize)(const void *, size_t);
static void(RETRO_CALLCONV *p_cheat_reset)(void);
static void(RETRO_CALLCONV *p_cheat_set)(unsigned, bool, const char *);
static bool(RETRO_CALLCONV *p_load_game)(const struct retro_game_info *);
static bool(RETRO_CALLCONV *p_load_game_special)(unsigned, const struct retro_game_info *, size_t);
static void(RETRO_CALLCONV *p_unload_game)(void);
static unsigned(RETRO_CALLCONV *p_get_region)(void);
static void *(RETRO_CALLCONV *p_get_memory_data)(unsigned);
static size_t(RETRO_CALLCONV *p_get_memory_size)(unsigned);

static int missing = 0;

/* ISO C has no conversion from void * to a function pointer, so the
   address is copied byte for byte. */
#define RESOLVE(var, name)                                                                         \
    do {                                                                                           \
        void *p_ = module_sym(name);                                                               \
        if (p_ == NULL) {                                                                          \
            fprintf(stderr, "missing symbol %s\n", name);                                          \
            missing++;                                                                             \
        } else {                                                                                   \
            memcpy(&(var), &p_, sizeof(var));                                                      \
        }                                                                                          \
    } while (0)

static void resolve_all(void)
{
    RESOLVE(p_set_environment, "retro_set_environment");
    RESOLVE(p_set_video_refresh, "retro_set_video_refresh");
    RESOLVE(p_set_audio_sample, "retro_set_audio_sample");
    RESOLVE(p_set_audio_sample_batch, "retro_set_audio_sample_batch");
    RESOLVE(p_set_input_poll, "retro_set_input_poll");
    RESOLVE(p_set_input_state, "retro_set_input_state");
    RESOLVE(p_init, "retro_init");
    RESOLVE(p_deinit, "retro_deinit");
    RESOLVE(p_api_version, "retro_api_version");
    RESOLVE(p_get_system_info, "retro_get_system_info");
    RESOLVE(p_get_system_av_info, "retro_get_system_av_info");
    RESOLVE(p_set_controller_port_device, "retro_set_controller_port_device");
    RESOLVE(p_reset, "retro_reset");
    RESOLVE(p_run, "retro_run");
    RESOLVE(p_serialize_size, "retro_serialize_size");
    RESOLVE(p_serialize, "retro_serialize");
    RESOLVE(p_unserialize, "retro_unserialize");
    RESOLVE(p_cheat_reset, "retro_cheat_reset");
    RESOLVE(p_cheat_set, "retro_cheat_set");
    RESOLVE(p_load_game, "retro_load_game");
    RESOLVE(p_load_game_special, "retro_load_game_special");
    RESOLVE(p_unload_game, "retro_unload_game");
    RESOLVE(p_get_region, "retro_get_region");
    RESOLVE(p_get_memory_data, "retro_get_memory_data");
    RESOLVE(p_get_memory_size, "retro_get_memory_size");
}

/* ---- frontend callbacks: record what the core does ---- */

static int rgb565; /* the mode: 0 xrgb8888, 1 rgb565 */
static int no_game_calls, no_game_value;
static int pixel_format_calls;
static enum retro_pixel_format pixel_format = RETRO_PIXEL_FORMAT_0RGB1555;
static int video_calls, video_null;
static unsigned video_width, video_height;
static size_t video_pitch;
static unsigned char frame[W * H * 4u];
static size_t sample_frames;
static int sample_nonzero;
static int batch_calls;
static size_t batch_frames;
static int batch_nonzero;

static bool RETRO_CALLCONV env(unsigned cmd, void *data)
{
    if (cmd == RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME) {
        no_game_calls++;
        no_game_value = *(const bool *)data ? 1 : 0;
        return true;
    }
    if (cmd == RETRO_ENVIRONMENT_SET_PIXEL_FORMAT) {
        pixel_format_calls++;
        pixel_format = *(const enum retro_pixel_format *)data;
        return true;
    }
    return false;
}

/* Keeps the rows of the last frame, packed, in the module's own byte order. */
static void RETRO_CALLCONV video(const void *data, unsigned width, unsigned height, size_t pitch)
{
    size_t row = (size_t)W * (rgb565 ? 2u : 4u);
    unsigned y;
    video_calls++;
    video_width = width;
    video_height = height;
    video_pitch = pitch;
    if (data == NULL) {
        video_null++;
        return;
    }
    if (width != W || height != H || pitch < row) {
        return;
    }
    for (y = 0; y < H; y++) {
        memcpy(&frame[y * row], (const unsigned char *)data + y * pitch, row);
    }
}

static void RETRO_CALLCONV sample(int16_t left, int16_t right)
{
    sample_frames++;
    if (left != 0 || right != 0) {
        sample_nonzero++;
    }
}

static size_t RETRO_CALLCONV batch(const int16_t *data, size_t frames)
{
    size_t i;
    batch_calls++;
    batch_frames += frames;
    for (i = 0; i < frames * 2u; i++) {
        if (data[i] != 0) {
            batch_nonzero++;
        }
    }
    return frames;
}

static void RETRO_CALLCONV on_poll(void)
{
}

static int16_t RETRO_CALLCONV input(unsigned port, unsigned device, unsigned index, unsigned id)
{
    (void)port;
    (void)device;
    (void)index;
    (void)id;
    return 0;
}

/* got must equal want; prints both. */
static void check_string(const char *what, const char *got, const char *want)
{
    if (got == NULL || strcmp(got, want) != 0) {
        fprintf(stderr, "%s: got \"%s\", expected \"%s\"\n", what, got != NULL ? got : "(null)",
                want);
        check_failures++;
    }
}

/* ---- the library's own frame, the expected image ---- */

static void *host_alloc(void *user, size_t size)
{
    (void)user;
    return malloc(size);
}

static void host_free(void *user, void *ptr, size_t size)
{
    (void)user;
    (void)size;
    free(ptr);
}

/* Compares the captured frame with the palette of each native byte, read as
   a native-endian uint32_t, or uint16_t in rgb565 mode. Prints the first
   mismatch. */
static void compare_frame(const uint8_t *native)
{
    uint32_t i, mismatches = 0;
    for (i = 0; i < W * H; i++) {
        uint32_t want, got;
        if (rgb565) {
            uint16_t px;
            memcpy(&px, &frame[2u * i], sizeof px);
            got = px;
            want = muhtari__palette_rgb565(native[i]);
        } else {
            memcpy(&got, &frame[4u * i], sizeof got);
            want = muhtari__palette_rgb(native[i]);
        }
        if (got != want) {
            if (mismatches == 0) {
                fprintf(stderr, "first mismatch at (%u,%u): got 0x%08x, expected 0x%08x\n",
                        (unsigned)(i % W), (unsigned)(i / W), (unsigned)got, (unsigned)want);
            }
            mismatches++;
        }
    }
    CHECK_EQ_U64(mismatches, 0);
}

int main(int argc, char **argv)
{
    struct retro_system_info sys;
    struct retro_system_av_info av;
    struct retro_game_info dummy;
    static const unsigned char dummy_bytes[16] = {0};
    static uint8_t native[W * H];
    static int16_t audio[801];
    char version[32];
    muhtari_versions v;
    muhtari_config cfg;
    muhtari_frame io;
    muhtari *m = NULL;
    unsigned run;

    if (argc != 3 || (strcmp(argv[2], "xrgb8888") != 0 && strcmp(argv[2], "rgb565") != 0)) {
        fprintf(stderr, "usage: muhtari-test-libretro-host <module> xrgb8888|rgb565\n");
        return 2;
    }
    rgb565 = strcmp(argv[2], "rgb565") == 0;
    if (!module_open(argv[1])) {
        fprintf(stderr, "cannot load %s\n", argv[1]);
        return 1;
    }
    resolve_all();
    CHECK_EQ_U64(missing, 0);
    if (missing != 0) {
        module_close();
        CHECK_DONE();
    }

    memset(&cfg, 0, sizeof cfg);
    cfg.size = (uint32_t)sizeof cfg;
    cfg.allocator.alloc = host_alloc;
    cfg.allocator.free = host_free;
    CHECK_EQ_U64(muhtari_create(&cfg, &m), MUHTARI_STATUS_OK);
    memset(&v, 0, sizeof v);
    v.size = (uint32_t)sizeof v;
    CHECK_EQ_U64(muhtari_get_versions(&v), MUHTARI_STATUS_OK);
    snprintf(version, sizeof version, "%u.%u.%u", (unsigned)v.library_major,
             (unsigned)v.library_minor, (unsigned)v.library_patch);

    CHECK_EQ_U64(p_api_version(), RETRO_API_VERSION);

    p_set_environment(env);
    CHECK_EQ_U64(no_game_calls, 1);
    CHECK_EQ_U64(no_game_value, 1);

    p_init();

    memset(&sys, 0, sizeof sys);
    p_get_system_info(&sys);
    check_string("library_name", sys.library_name, "muhtari");
    check_string("library_version", sys.library_version, version);
    CHECK_EQ_U64(sys.need_fullpath, 0);

    p_set_video_refresh(video);
    p_set_audio_sample(sample);
    p_set_audio_sample_batch(rgb565 ? NULL : batch);
    p_set_input_poll(on_poll);
    p_set_input_state(input);

    /* Content is refused; only the test card runs in this release. */
    memset(&dummy, 0, sizeof dummy);
    dummy.data = dummy_bytes;
    dummy.size = sizeof dummy_bytes;
    CHECK_EQ_U64(p_load_game(&dummy), 0);
    CHECK_EQ_U64(pixel_format_calls, 0);

    /* No content: the pixel format is set before load returns (L861-865). */
    CHECK_EQ_U64(p_load_game(NULL), 1);
    CHECK_EQ_U64(pixel_format_calls, 1);
    CHECK_EQ_U64(pixel_format,
                 rgb565 ? RETRO_PIXEL_FORMAT_RGB565 : RETRO_PIXEL_FORMAT_XRGB8888);

    memset(&av, 0, sizeof av);
    p_get_system_av_info(&av);
    CHECK_EQ_U64(av.geometry.base_width, 320);
    CHECK_EQ_U64(av.geometry.base_height, 242);
    CHECK_EQ_U64(av.geometry.max_width, 320);
    CHECK_EQ_U64(av.geometry.max_height, 293);
    CHECK_EQ_DBL(av.geometry.aspect_ratio, (float)(960.0 / 847.0));
    CHECK_EQ_DBL(av.timing.fps, 39375000.0 / 656711.0);
    CHECK_EQ_DBL(av.timing.sample_rate, 48000.0);

    for (run = 0; run < RUNS; run++) {
        memset(&io, 0, sizeof io);
        io.size = (uint32_t)sizeof io;
        io.video = native;
        io.video_pitch = W;
        io.audio = audio;
        io.audio_capacity = 801u;
        CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_OK);

        video_calls = 0;
        video_null = 0;
        batch_calls = 0;
        batch_frames = 0;
        sample_frames = 0;
        memset(frame, 0, sizeof frame);
        p_run();
        CHECK_EQ_U64(video_calls, 1);
        CHECK_EQ_U64(video_null, 0);
        CHECK_EQ_U64(video_width, W);
        CHECK_EQ_U64(video_height, H);
        CHECK_EQ_U64(video_pitch, W * (rgb565 ? 2u : 4u));
        compare_frame(native);
        /* 800 or 801 stereo frames, as many as the library wrote, through
           the one path this mode allows. */
        if (io.audio_count != 800u && io.audio_count != 801u) {
            fprintf(stderr, "library audio_count %u, expected 800 or 801\n",
                    (unsigned)io.audio_count);
            check_failures++;
        }
        CHECK_EQ_U64(batch_calls, rgb565 ? 0u : 1u);
        CHECK_EQ_U64(batch_frames, rgb565 ? 0u : io.audio_count);
        CHECK_EQ_U64(sample_frames, rgb565 ? io.audio_count : 0u);
    }
    CHECK_EQ_U64(batch_nonzero, 0);
    CHECK_EQ_U64(sample_nonzero, 0);

    p_unload_game();
    p_deinit();
    module_close();
    muhtari_destroy(m);
    CHECK_DONE();
}
