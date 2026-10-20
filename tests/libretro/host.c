/* Loads the built libretro module at run time and calls it in the order a
 * frontend such as RetroArch does.
 *
 *   muhtari-test-libretro-host <module> xrgb8888|rgb565
 *   muhtari-test-libretro-host <module> <case> <file>...
 *
 * xrgb8888 and rgb565 run the test card with no content and compare every
 * frame received with the library's own frame through the same palette.
 * Pixels are read as native-endian integers, so a swapped channel order, 555
 * for 565, or a swapped byte order fails here. In rgb565 mode the host
 * registers no batch audio callback, so the single-sample path runs.
 *
 * The other cases load content the way a frontend does and drive the
 * RetroPad. The input cartridge (tests/input/test_input.c) stores what it
 * reads from SWCHA, SWCHB and INPT0-INPT5 each frame in a ring in system
 * RAM, which the host reads back through retro_get_memory_data. */
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include <stdarg.h>
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
#include "sha256.h"

#define W 320u
#define H 242u
#define RUNS 3u

/* a == b as doubles, printed with every digit. */
#define CHECK_EQ_DBL(a, b)                                                                         \
    do {                                                                                           \
        double check_a_ = (double)(a);                                                             \
        double check_b_ = (double)(b);                                                             \
        if (check_a_ != check_b_) {                                                                \
            fprintf(stderr, "%s:%d: %s == %s failed: %.17g != %.17g\n", __FILE__, __LINE__, #a,    \
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
static uint32_t pad[2]; /* the RetroPad buttons held, bit n for id n */
static char log_text[4096];
static size_t log_used;

/* What the core told the frontend through the environment. */
static const struct retro_controller_info *controller_info;
static int input_descriptors; /* entries before the terminator, -1 if unset */
static const struct retro_input_descriptor *input_list;
static const struct retro_memory_map *memory_map;
static int achievements = -1;
static char message[256];
static unsigned message_calls;
static unsigned options_version = 2u; /* what GET_CORE_OPTIONS_VERSION answers */
static const char *option_keys[16];   /* the keys the core declared */
static int option_set_with;           /* 2 for SET_CORE_OPTIONS_V2, 1 for v1 */
static const char *system_dir;

/* The frontend's option values, changed by the cases. */
static struct {
    const char *key;
    const char *value;
} vars[] = {
    {"muhtari_boot", "fast"},
    {"muhtari_palette", "ntsc"},
    {"muhtari_left_difficulty", "B"},
    {"muhtari_right_difficulty", "B"},
};
static bool vars_updated;

static void set_var(const char *key, const char *value)
{
    size_t i;
    for (i = 0; i < sizeof vars / sizeof vars[0]; i++) {
        if (strcmp(vars[i].key, key) == 0) {
            vars[i].value = value;
        }
    }
    vars_updated = true;
}

/* Keeps every log line, and prints it. */
static void RETRO_CALLCONV host_log(enum retro_log_level level, const char *fmt, ...)
{
    va_list ap;
    int n;
    (void)level;
    va_start(ap, fmt);
    n = vsnprintf(log_text + log_used, sizeof log_text - log_used, fmt, ap);
    va_end(ap);
    if (n > 0) {
        fputs(log_text + log_used, stderr);
        log_used += (size_t)n < sizeof log_text - log_used ? (size_t)n : 0u;
    }
}

static bool RETRO_CALLCONV env(unsigned cmd, void *data)
{
    if (cmd == RETRO_ENVIRONMENT_GET_LOG_INTERFACE) {
        ((struct retro_log_callback *)data)->log = host_log;
        return true;
    }
    if (cmd == RETRO_ENVIRONMENT_SET_CONTROLLER_INFO) {
        controller_info = (const struct retro_controller_info *)data;
        return true;
    }
    if (cmd == RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS) {
        input_list = (const struct retro_input_descriptor *)data;
        for (input_descriptors = 0; input_list[input_descriptors].description != NULL;) {
            input_descriptors++;
        }
        return true;
    }
    if (cmd == RETRO_ENVIRONMENT_SET_MEMORY_MAPS) {
        memory_map = (const struct retro_memory_map *)data;
        return true;
    }
    if (cmd == RETRO_ENVIRONMENT_SET_SUPPORT_ACHIEVEMENTS) {
        achievements = *(const bool *)data ? 1 : 0;
        return true;
    }
    if (cmd == RETRO_ENVIRONMENT_SET_MESSAGE) {
        message_calls++;
        snprintf(message, sizeof message, "%s", ((const struct retro_message *)data)->msg);
        return true;
    }
    if (cmd == RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY) {
        *(const char **)data = system_dir;
        return system_dir != NULL;
    }
    if (cmd == RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION) {
        *(unsigned *)data = options_version;
        return true;
    }
    if (cmd == RETRO_ENVIRONMENT_SET_CORE_OPTIONS_V2 && options_version >= 2u) {
        const struct retro_core_option_v2_definition *d =
            ((const struct retro_core_options_v2 *)data)->definitions;
        int k;
        for (k = 0; k < 15 && d[k].key != NULL; k++) {
            option_keys[k] = d[k].key;
        }
        option_set_with = 2;
        return true;
    }
    if (cmd == RETRO_ENVIRONMENT_SET_CORE_OPTIONS && options_version >= 1u) {
        const struct retro_core_option_definition *d =
            (const struct retro_core_option_definition *)data;
        int k;
        for (k = 0; k < 15 && d[k].key != NULL; k++) {
            option_keys[k] = d[k].key;
        }
        option_set_with = 1;
        return true;
    }
    if (cmd == RETRO_ENVIRONMENT_GET_VARIABLE) {
        struct retro_variable *v = (struct retro_variable *)data;
        size_t i;
        for (i = 0; i < sizeof vars / sizeof vars[0]; i++) {
            if (strcmp(vars[i].key, v->key) == 0) {
                v->value = vars[i].value;
                return true;
            }
        }
        v->value = NULL;
        return false;
    }
    if (cmd == RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE) {
        *(bool *)data = vars_updated;
        vars_updated = false;
        return true;
    }
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
    if (port > 1u || (device & RETRO_DEVICE_MASK) != RETRO_DEVICE_JOYPAD || index != 0u ||
        id > 15u) {
        return 0;
    }
    return (int16_t)((pad[port] >> id) & 1u);
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

static void card(void)
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

    /* Bytes that are no cartridge image are refused, and set no pixel
       format. */
    memset(&dummy, 0, sizeof dummy);
    dummy.data = dummy_bytes;
    dummy.size = sizeof dummy_bytes;
    CHECK_EQ_U64(p_load_game(&dummy), 0);
    CHECK_EQ_U64(pixel_format_calls, 0);

    /* No content: the pixel format is set before load returns (L861-865). */
    CHECK_EQ_U64(p_load_game(NULL), 1);
    CHECK_EQ_U64(pixel_format_calls, 1);
    CHECK_EQ_U64(pixel_format, rgb565 ? RETRO_PIXEL_FORMAT_RGB565 : RETRO_PIXEL_FORMAT_XRGB8888);

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
    muhtari_destroy(m);
}

/* ---- content cases ---- */

/* Where the input cartridge keeps the offset of the entry last written, as
   an index into system RAM ($2041), and the order of an entry's bytes
   (tests/input/test_input.c). */
#define LAST_ENTRY 0x0841u
enum { E_SWCHA, E_SWCHB, E_INPT0, E_INPT1, E_INPT2, E_INPT3, E_INPT4, E_INPT5 };

static unsigned char *files[3];
static size_t file_sizes[3];

/* Reads path into files[slot]; 0 on failure. */
static int read_file(const char *path, int slot)
{
    FILE *f = fopen(path, "rb");
    long n;
    if (f == NULL || fseek(f, 0, SEEK_END) != 0 || (n = ftell(f)) <= 0 ||
        fseek(f, 0, SEEK_SET) != 0) {
        fprintf(stderr, "cannot read %s\n", path);
        if (f != NULL) {
            (void)fclose(f);
        }
        return 0;
    }
    files[slot] = (unsigned char *)malloc((size_t)n);
    file_sizes[slot] = (size_t)n;
    if (files[slot] == NULL || fread(files[slot], 1u, (size_t)n, f) != (size_t)n) {
        fprintf(stderr, "cannot read %s\n", path);
        (void)fclose(f);
        return 0;
    }
    (void)fclose(f);
    return 1;
}

/* Starts the core as a frontend does and loads files[slot]. */
static void start(int slot)
{
    struct retro_game_info game;
    memset(&game, 0, sizeof game);
    game.data = files[slot];
    game.size = file_sizes[slot];
    p_set_environment(env);
    p_init();
    p_set_video_refresh(video);
    p_set_audio_sample(sample);
    p_set_audio_sample_batch(batch);
    p_set_input_poll(on_poll);
    p_set_input_state(input);
    CHECK_EQ_U64(p_load_game(&game), 1);
}

static void stop(void)
{
    p_unload_game();
    p_deinit();
}

static void run_frames(unsigned n)
{
    while (n-- > 0u) {
        p_run();
    }
}

/* The entry the cartridge wrote last, read through the memory interface. */
static const uint8_t *last_entry(void)
{
    static const uint8_t none[8] = {0};
    const uint8_t *ram = (const uint8_t *)p_get_memory_data(RETRO_MEMORY_SYSTEM_RAM);
    CHECK_EQ_U64(p_get_memory_size(RETRO_MEMORY_SYSTEM_RAM), 4096u);
    if (ram == NULL) {
        check_failures++;
        return none;
    }
    return ram + ram[LAST_ENTRY];
}

/* Holds the buttons in mask on port for three frames, then releases them for
   three, and returns the entry the cartridge read while they were held. */
static void press(unsigned port, uint32_t mask, uint8_t got[8])
{
    pad[port] = mask;
    run_frames(3u);
    memcpy(got, last_entry(), 8u);
    pad[port] = 0u;
    run_frames(3u);
}

#define BIT(id) (1u << RETRO_DEVICE_ID_JOYPAD_##id)
#define HI 0x80u /* TIA drives D7 alone of INPT0-INPT5 */

/* Port 1's and port 2's Up reach SWCHA's bit 4 and bit 0, pulled to 0
   while held (docs/hardware/controllers.md). */
static void case_input(void)
{
    start(0);
    run_frames(3u);
    CHECK_EQ_HEX(last_entry()[E_SWCHA], 0xFFu);
    pad[0] = 1u << RETRO_DEVICE_ID_JOYPAD_UP;
    run_frames(3u);
    CHECK_EQ_HEX(last_entry()[E_SWCHA], 0xEFu);
    pad[0] = 0u;
    pad[1] = 1u << RETRO_DEVICE_ID_JOYPAD_UP;
    run_frames(3u);
    CHECK_EQ_HEX(last_entry()[E_SWCHA], 0xFEu);
    pad[1] = 0u;
    stop();
}

/* ProSystem's RetroPad layout (HOSTS-AND-INTEGRATION section 1, Input) on
   the input cartridge, which leaves both ports in one-button mode: button 1
   reads 1 on INPT0 [INPT2] and button 2 on INPT1 [INPT3], and either pulls
   INPT4 [INPT5] to 0 (docs/hardware/controllers.md). Select, Start and X
   on port 1 are Select, Pause and Reset, read 0 on SWCHB D1, D3 and D0;
   port 2's do nothing. L and R flip the left and right difficulty on each
   press, read 1 on SWCHB D6 and D7 at A. A Joystick's B and A both fire. */
static void case_layout(void)
{
    uint8_t e[8];
    start(0);
    run_frames(3u);
    press(0u, BIT(B), e);
    CHECK_EQ_HEX(e[E_INPT0] & HI, HI);
    CHECK_EQ_HEX(e[E_INPT1] & HI, 0u);
    CHECK_EQ_HEX(e[E_INPT4] & HI, 0u);
    press(0u, BIT(A), e);
    CHECK_EQ_HEX(e[E_INPT0] & HI, 0u);
    CHECK_EQ_HEX(e[E_INPT1] & HI, HI);
    CHECK_EQ_HEX(e[E_INPT4] & HI, 0u);
    press(1u, BIT(B), e);
    CHECK_EQ_HEX(e[E_INPT2] & HI, HI);
    CHECK_EQ_HEX(e[E_INPT5] & HI, 0u);
    press(1u, BIT(A), e);
    CHECK_EQ_HEX(e[E_INPT3] & HI, HI);
    CHECK_EQ_HEX(e[E_INPT5] & HI, 0u);
    press(0u, BIT(SELECT), e);
    CHECK_EQ_HEX(e[E_SWCHB], 0x3Du);
    press(0u, BIT(START), e);
    CHECK_EQ_HEX(e[E_SWCHB], 0x37u);
    press(0u, BIT(X), e);
    CHECK_EQ_HEX(e[E_SWCHB], 0x3Eu);
    press(1u, BIT(SELECT) | BIT(START) | BIT(X) | BIT(L) | BIT(R), e);
    CHECK_EQ_HEX(e[E_SWCHB], 0x3Fu);
    /* A press edge flips the switch, and it stays there once released. */
    message[0] = '\0';
    press(0u, BIT(L), e);
    CHECK_EQ_HEX(e[E_SWCHB], 0x7Fu);
    CHECK_EQ_HEX(last_entry()[E_SWCHB], 0x7Fu);
    CHECK(strstr(message, "Left difficulty: A") != NULL);
    press(0u, BIT(R), e);
    CHECK_EQ_HEX(e[E_SWCHB], 0xFFu);
    CHECK(strstr(message, "Right difficulty: A") != NULL);
    press(0u, BIT(L), e);
    CHECK_EQ_HEX(last_entry()[E_SWCHB], 0xBFu);
    CHECK(strstr(message, "Left difficulty: B") != NULL);
    press(0u, BIT(R), e);
    CHECK_EQ_HEX(last_entry()[E_SWCHB], 0x3Fu);
    /* Joystick: no INPT0-INPT3, and A fires like B. */
    p_set_controller_port_device(0u, RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_JOYPAD, 1));
    press(0u, BIT(A), e);
    CHECK_EQ_HEX(e[E_INPT1] & HI, 0u);
    CHECK_EQ_HEX(e[E_INPT4] & HI, 0u);
    press(0u, BIT(B), e);
    CHECK_EQ_HEX(e[E_INPT0] & HI, 0u);
    CHECK_EQ_HEX(e[E_INPT4] & HI, 0u);
    stop();
}

/* The controller list on both ports is Auto, ProLine, Joystick and None;
   None unplugs the port, whose pins then read released. The input
   descriptors name the 11 inputs of port 1 and the 6 of port 2. */
static void case_devices(void)
{
    static const char *const names[4] = {"Auto (cartridge header)", "ProLine", "Joystick", "None"};
    static const unsigned ids[4] = {
        RETRO_DEVICE_JOYPAD, RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_JOYPAD, 0),
        RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_JOYPAD, 1), RETRO_DEVICE_NONE};
    uint8_t e[8];
    unsigned port, t;
    start(0);
    CHECK(controller_info != NULL);
    for (port = 0; controller_info != NULL && port < 2u; port++) {
        CHECK_EQ_U64(controller_info[port].num_types, 4u);
        for (t = 0; t < 4u && t < controller_info[port].num_types; t++) {
            check_string("controller", controller_info[port].types[t].desc, names[t]);
            CHECK_EQ_U64(controller_info[port].types[t].id, ids[t]);
        }
    }
    if (controller_info != NULL) {
        CHECK(controller_info[2].types == NULL && controller_info[2].num_types == 0u);
    }
    CHECK_EQ_U64(input_descriptors, 17);
    for (t = 0; input_list != NULL && (int)t < input_descriptors; t++) {
        CHECK(input_list[t].port < 2u && input_list[t].device == RETRO_DEVICE_JOYPAD);
        CHECK(input_list[t].id <= RETRO_DEVICE_ID_JOYPAD_R);
    }
    run_frames(3u);
    press(0u, BIT(UP), e);
    CHECK_EQ_HEX(e[E_SWCHA], 0xEFu);
    p_set_controller_port_device(0u, RETRO_DEVICE_NONE);
    press(0u, BIT(UP) | BIT(B) | BIT(A), e);
    CHECK_EQ_HEX(e[E_SWCHA], 0xFFu);
    CHECK_EQ_HEX(e[E_INPT0] & HI, 0u);
    CHECK_EQ_HEX(e[E_INPT4] & HI, HI);
    p_set_controller_port_device(0u, RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_JOYPAD, 0));
    press(0u, BIT(UP), e);
    CHECK_EQ_HEX(e[E_SWCHA], 0xEFu);
    stop();
}

/* FNV-1a over the last frame received. */
static uint64_t frame_hash(void)
{
    uint64_t h = 14695981039346656037u;
    size_t i;
    for (i = 0; i < sizeof frame; i++) {
        h = (h ^ frame[i]) * 1099511628211u;
    }
    return h;
}

/* Runs n frames and keeps each frame's hash. */
static void hash_frames(uint64_t *out, unsigned n)
{
    unsigned i;
    for (i = 0; i < n; i++) {
        p_run();
        out[i] = frame_hash();
    }
}

/* Restart is a power cycle: after some play, a flipped difficulty and a
   held button, retro_reset gives the frames of a fresh load. */
static void case_reset(void)
{
    uint64_t fresh[8], again[8];
    uint8_t e[8];
    unsigned i;
    start(0);
    hash_frames(fresh, 8u);
    press(0u, BIT(L), e);
    CHECK_EQ_HEX(e[E_SWCHB], 0x7Fu);
    run_frames(20u);
    p_reset();
    hash_frames(again, 8u);
    for (i = 0; i < 8u; i++) {
        if (fresh[i] != again[i]) {
            fprintf(stderr, "frame %u after reset differs from a fresh load\n", i + 1u);
            check_failures++;
        }
    }
    stop();
}

/* Writes bytes from files[1] to dir/name. */
static void put_bios(const char *dir, const char *name, size_t bytes)
{
    char path[1024];
    FILE *f;
    snprintf(path, sizeof path, "%s/%s", dir, name);
    f = fopen(path, "wb");
    CHECK(f != NULL);
    if (f != NULL) {
        CHECK_EQ_U64(fwrite(files[1], 1u, bytes, f), bytes);
        CHECK_EQ_U64(fclose(f), 0);
    }
}

static void drop_bios(const char *dir, const char *name)
{
    char path[1024];
    snprintf(path, sizeof path, "%s/%s", dir, name);
    (void)remove(path);
}

/* A BIOS-rejected cart still delivers one video callback per run, and
   reports the failure once, including again after Restart. */
static void frame_failure(void)
{
    uint8_t footer = files[0][file_sizes[0] - 8u];
    files[0][file_sizes[0] - 8u] &= 0xFEu;
    start(0);
    for (unsigned restart = 0; restart < 2u; restart++) {
        unsigned messages = message_calls;
        unsigned run;
        for (run = 0; run < 30u && message_calls == messages; run++) {
            int videos = video_calls;
            int batches = batch_calls;
            int nulls = video_null;
            uint64_t previous = frame_hash();
            p_run();
            CHECK_EQ_U64(video_calls, videos + 1);
            if (message_calls != messages) {
                CHECK_EQ_U64(video_null, nulls);
                CHECK_EQ_U64(frame_hash(), previous);
                CHECK_EQ_U64(batch_calls, batches);
            }
        }
        CHECK(run < 30u);
        CHECK_EQ_U64(message_calls, messages + 1u);
        CHECK(strstr(message, "BIOS rejected") != NULL);
        CHECK(strstr(log_text, "frame failed:") != NULL);
        size_t logged = log_used;
        uint64_t previous = frame_hash();
        int videos = video_calls, nulls = video_null, batches = batch_calls;
        size_t samples = sample_frames;
        run_frames(3u);
        CHECK_EQ_U64(video_calls, videos + 3);
        CHECK_EQ_U64(video_null, nulls);
        CHECK_EQ_U64(frame_hash(), previous);
        CHECK_EQ_U64(video_width, W);
        CHECK_EQ_U64(video_height, H);
        CHECK_EQ_U64(video_pitch, W * 4u);
        CHECK_EQ_U64(batch_calls, batches);
        CHECK_EQ_U64(sample_frames, samples);
        CHECK_EQ_U64(message_calls, messages + 1u);
        CHECK_EQ_U64(log_used, logged);
        p_reset();
    }
    p_set_video_refresh(NULL);
    run_frames(30u);
    stop();
    files[0][file_sizes[0] - 8u] = footer;
}

/* muhtari_boot = bios boots through a BIOS file in the system directory,
   under either name, and logs its SHA-256: frame 120 shows the BIOS, not
   the title the fast boot shows there. With no file the core says so and
   gives the fast boot's frames. */
#define BIOS_FRAMES 120u
static void case_bios(const char *dir)
{
    static const char *const names[2] = {"7800 BIOS (U).rom", "7800 BIOS (E).rom"};
    static uint64_t fast[BIOS_FRAMES], bios[BIOS_FRAMES], fallback[BIOS_FRAMES];
    uint8_t digest[32];
    char hex[65];
    muhtari__sha256 sha;
    unsigned n, i;

    muhtari__sha256_init(&sha);
    muhtari__sha256_update(&sha, files[1], file_sizes[1]);
    muhtari__sha256_final(&sha, digest);
    muhtari__sha256_hex(digest, hex);
    system_dir = dir;
    drop_bios(dir, names[0]);
    drop_bios(dir, names[1]);

    start(0);
    hash_frames(fast, BIOS_FRAMES);
    stop();
    for (n = 0; n < 2u; n++) {
        put_bios(dir, names[n], file_sizes[1]);
        set_var("muhtari_boot", "bios");
        log_used = 0u;
        log_text[0] = '\0';
        start(0);
        hash_frames(bios, BIOS_FRAMES);
        stop();
        drop_bios(dir, names[n]);
        CHECK(strstr(log_text, names[n]) != NULL);
        CHECK(strstr(log_text, hex) != NULL);
        CHECK(bios[BIOS_FRAMES - 1u] != fast[BIOS_FRAMES - 1u]);
    }
    /* A damaged first file must not hide the usable second file. */
    put_bios(dir, names[0], 17u);
    put_bios(dir, names[1], file_sizes[1]);
    log_used = 0u;
    log_text[0] = message[0] = '\0';
    start(0);
    hash_frames(fallback, BIOS_FRAMES);
    stop();
    CHECK(strstr(log_text, "BIOS (U).rom: not a 4 KiB or 16 KiB BIOS image") != NULL);
    CHECK(strstr(log_text, "BIOS (E).rom, SHA-256") != NULL);
    CHECK(strstr(log_text, "fast boot") == NULL);
    CHECK_EQ_U64(message[0], 0u);
    for (i = 0; i < BIOS_FRAMES; i++) {
        CHECK_EQ_U64(fallback[i], bios[i]);
    }
    drop_bios(dir, names[1]);
    put_bios(dir, names[0], file_sizes[1]);
    frame_failure();
    drop_bios(dir, names[0]);
    log_used = 0u;
    log_text[0] = '\0';
    start(0);
    hash_frames(fallback, BIOS_FRAMES);
    stop();
    CHECK(strstr(log_text, "fast boot") != NULL);
    for (i = 0; i < BIOS_FRAMES; i++) {
        CHECK_EQ_U64(fallback[i], fast[i]);
    }
    set_var("muhtari_boot", "fast");
    system_dir = NULL;
}

/* The core declares its four options through SET_CORE_OPTIONS_V2, or the
   v1 call when the frontend has only that. The difficulty options set the
   switches at load and at Restart; a change mid-game sets them at once. */
static void check_keys(void)
{
    static const char *const keys[4] = {"muhtari_boot", "muhtari_palette",
                                        "muhtari_left_difficulty", "muhtari_right_difficulty"};
    unsigned k;
    for (k = 0; k < 4u; k++) {
        check_string("option key", option_keys[k], keys[k]);
    }
    CHECK(option_keys[4] == NULL);
}

static void case_options(void)
{
    uint8_t e[8];
    size_t k;
    options_version = 1u;
    p_set_environment(env);
    CHECK_EQ_U64(option_set_with, 1);
    check_keys();
    p_deinit();
    for (k = 0; k < sizeof option_keys / sizeof option_keys[0]; k++) {
        option_keys[k] = NULL;
    }
    options_version = 2u;

    set_var("muhtari_left_difficulty", "A");
    set_var("muhtari_right_difficulty", "A");
    start(0);
    CHECK_EQ_U64(option_set_with, 2);
    check_keys();
    run_frames(3u);
    CHECK_EQ_HEX(last_entry()[E_SWCHB], 0xFFu);
    set_var("muhtari_right_difficulty", "B");
    run_frames(3u);
    CHECK_EQ_HEX(last_entry()[E_SWCHB], 0x7Fu);
    press(0u, BIT(L), e);
    CHECK_EQ_HEX(last_entry()[E_SWCHB], 0x3Fu);
    p_reset();
    run_frames(3u);
    CHECK_EQ_HEX(last_entry()[E_SWCHB], 0x7Fu);
    stop();
    set_var("muhtari_left_difficulty", "B");
}

/* Where the map must put a span of CPU addresses: no memory, system RAM
   from byte at, the RIOT's RAM, cartridge RAM, or the content file from
   byte at. */
enum { NOTHING, RAM, RIOT, CART_RAM, ROM };
typedef struct want_span {
    uint32_t start, end;
    int kind;
    size_t at;
    uint64_t flags;
} want_span;

/* The spans every cartridge shares (HOSTS-AND-INTEGRATION section 2): page
   0's and page 1's RAM shadows at $2040 and $2140, the RIOT's RAM, system
   RAM, and its second half again at $2800. */
#define SHARED_SPANS                                                                               \
    {0x0040u, 0x0100u, RAM, 0x0840u, 0u}, {0x0140u, 0x0200u, RAM, 0x0940u, 0u},                    \
        {0x0480u, 0x0500u, RIOT, 0u, 0u}, {0x1800u, 0x2800u, RAM, 0u, RETRO_MEMDESC_SYSTEM_RAM},   \
        {0x2800u, 0x3000u, RAM, 0x0800u, 0u}

/* The byte the map gives for CPU address addr, or NULL; *flags gets its
   descriptor's flags. Exactly one descriptor must cover each address. */
static const uint8_t *resolve(uint32_t addr, uint64_t *flags)
{
    const struct retro_memory_descriptor *hit = NULL;
    unsigned i, n = 0;
    *flags = 0u;
    for (i = 0; i < memory_map->num_descriptors; i++) {
        const struct retro_memory_descriptor *d = &memory_map->descriptors[i];
        if (addr >= d->start && addr - d->start < d->len) {
            hit = d;
            n++;
        }
    }
    if (n != 1u) {
        fprintf(stderr, "address $%04x is in %u descriptors\n", (unsigned)addr, n);
        check_failures++;
        return NULL;
    }
    *flags = hit->flags;
    return hit->ptr != NULL ? (const uint8_t *)hit->ptr + hit->offset + (addr - hit->start) : NULL;
}

/* Checks every descriptor's shape and every address of the CPU bus against
   want: the right byte, no memory outside the spans, the right flags. A
   descriptor with select 0 has a power-of-two length (libretro.h L3634),
   here aligned to its start, and one with no memory has no flags (L3582). */
static void check_map(const want_span *want, unsigned n, const unsigned char *file)
{
    const uint8_t *ram = (const uint8_t *)p_get_memory_data(RETRO_MEMORY_SYSTEM_RAM);
    const uint8_t *first[8] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
    unsigned i, bad = 0;
    uint32_t addr;
    CHECK(memory_map != NULL && ram != NULL);
    if (memory_map == NULL || ram == NULL) {
        return;
    }
    for (i = 0; i < memory_map->num_descriptors; i++) {
        const struct retro_memory_descriptor *d = &memory_map->descriptors[i];
        CHECK(d->select == 0u && d->disconnect == 0u && d->len != 0u);
        CHECK((d->len & (d->len - 1u)) == 0u && d->start % (d->len != 0u ? d->len : 1u) == 0u);
        CHECK(d->ptr != NULL || d->flags == 0u);
    }
    for (addr = 0; addr < 0x10000u; addr++) {
        const want_span *w = NULL;
        const uint8_t *expect = NULL;
        unsigned at = 0u;
        uint64_t flags;
        const uint8_t *got = resolve(addr, &flags);
        int ok;
        for (i = 0; i < n; i++) {
            if (addr >= want[i].start && addr < want[i].end) {
                w = &want[i];
                at = i;
            }
        }
        if (w != NULL && w->kind != RAM && first[at] == NULL && got != NULL) {
            first[at] = got - (addr - w->start); /* the span's own base */
        }
        if (w == NULL) {
            ok = got == NULL && flags == 0u;
        } else if (w->kind == RAM) {
            expect = ram + w->at + (addr - w->start);
            ok = got == expect && flags == w->flags;
        } else {
            expect = first[at] + (addr - w->start);
            ok = got != NULL && got == expect && flags == w->flags &&
                 (w->kind != ROM || *got == file[w->at + (addr - w->start)]) &&
                 (w->kind == ROM || got < ram || got >= ram + 4096);
        }
        if (!ok && bad++ == 0u) {
            fprintf(stderr, "address $%04x: flags %llx, %s\n", (unsigned)addr,
                    (unsigned long long)flags, got == NULL ? "no memory" : "wrong byte");
        }
    }
    CHECK_EQ_U64(bad, 0u);
}

/* The achievement map (HOSTS-AND-INTEGRATION section 2): the shared spans,
   unbanked cartridge RAM at $4000 and the fixed ROM as CONST. Registers,
   $3000-$3FFF and banked windows have no memory. Linear: the input
   cartridge's 48 KiB from $4000. SuperGame with RAM: doublebuffer's RAM at
   $4000 and its last bank at $C000. SuperGame 2^n + 1: sg144's bank 0 at
   $4000 and its last bank at $C000. */
static void case_memory_map(void)
{
    static const want_span linear[] = {SHARED_SPANS,
                                       {0x4000u, 0x10000u, ROM, 128u, RETRO_MEMDESC_CONST}};
    want_span ram_cart[] = {SHARED_SPANS,
                            {0x4000u, 0x8000u, CART_RAM, 0u, 0u},
                            {0xC000u, 0x10000u, ROM, 0u, RETRO_MEMDESC_CONST}};
    want_span bank0[] = {SHARED_SPANS,
                         {0x4000u, 0x8000u, ROM, 128u, RETRO_MEMDESC_CONST},
                         {0xC000u, 0x10000u, ROM, 0u, RETRO_MEMDESC_CONST}};
    int slot;
    ram_cart[6].at = file_sizes[1] - 0x4000u;
    bank0[6].at = file_sizes[2] - 0x4000u;
    for (slot = 0; slot < 3; slot++) {
        memory_map = NULL;
        achievements = -1;
        start(slot);
        CHECK_EQ_U64(achievements, 1);
        if (slot == 0) {
            check_map(linear, 6u, files[0]);
        } else {
            check_map(slot == 1 ? ram_cart : bank0, 7u, files[slot]);
        }
        stop();
    }
}

int main(int argc, char **argv)
{
    static const struct {
        const char *name;
        void (*run)(void);
        int files;
    } cases[] = {
        {"input", case_input, 1}, {"layout", case_layout, 1},   {"devices", case_devices, 1},
        {"reset", case_reset, 1}, {"options", case_options, 1}, {"memory_map", case_memory_map, 3},
    };
    size_t i;
    int found = 0;

    if (argc < 3) {
        fprintf(stderr,
                "usage: muhtari-test-libretro-host <module> xrgb8888|rgb565|<case> <file>...\n");
        return 2;
    }
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
    if (argc == 3 && (strcmp(argv[2], "xrgb8888") == 0 || strcmp(argv[2], "rgb565") == 0)) {
        rgb565 = strcmp(argv[2], "rgb565") == 0;
        card();
        found = 1;
    }
    /* bios <cart> <bios file> <system directory> */
    if (argc == 6 && strcmp(argv[2], "bios") == 0) {
        found = 1;
        if (!read_file(argv[3], 0) || !read_file(argv[4], 1)) {
            return 1;
        }
        case_bios(argv[5]);
    }
    for (i = 0; !found && i < sizeof cases / sizeof cases[0]; i++) {
        if (strcmp(argv[2], cases[i].name) == 0) {
            int f;
            found = 1;
            if (argc != 3 + cases[i].files) {
                fprintf(stderr, "%s needs %d file(s)\n", cases[i].name, cases[i].files);
                return 2;
            }
            for (f = 0; f < cases[i].files; f++) {
                if (!read_file(argv[3 + f], f)) {
                    return 1;
                }
            }
            cases[i].run();
        }
    }
    if (!found) {
        fprintf(stderr, "unknown case %s\n", argv[2]);
        module_close();
        return 2;
    }
    free(files[0]);
    free(files[1]);
    free(files[2]);
    module_close();
    CHECK_DONE();
}
