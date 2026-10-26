/* The libretro adapter: the 25 entry points of libretro.h, written as a thin
 * client of muhtari.h. "Lnnn" is a line of the vendored libretro.h
 * (RetroArch v1.22.2, see PROVENANCE.md). docs/libretro.md describes what a
 * user sees: the RetroPad layout, the devices, the options, the BIOS files
 * and the achievement map.
 *
 * retro_load_game(NULL) creates the instance, which shows the built-in test
 * card; content is loaded through muhtari_load, by the fast boot or through
 * a BIOS file. The adapter copies the content, and the copy lives until the
 * game unloads: the ROM descriptors of the memory map point into it. */
/* Ubuntu's PPC32 headers default to the newer time64 stat ABI. Request
   the legacy layout before any headers so __xstat version 3 uses the
   matching LSB Core PPC32 struct, retaining the GLIBC 2.17 floor.
   This adapter inspects file type only, and reads at most 16 KiB firmware. */
#if defined(__linux__) && defined(__powerpc__) && !defined(__powerpc64__)
#undef _TIME_BITS
#undef _FILE_OFFSET_BITS
#define _TIME_BITS 32
#define _FILE_OFFSET_BITS 32
#endif
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#endif

/* LSB Core, __xstat: the stable binary stat interface. Modern glibc's
   public stat symbols need 2.33/2.34; retain the existing 2.17 floor.
   Test builds that substitute stat keep that same file-fault seam. */
#if defined(__GLIBC__) &&                                                                          \
    (defined(__x86_64__) || defined(__aarch64__) ||                                                \
     (defined(__powerpc__) && !defined(__powerpc64__))) &&                                         \
    !defined(stat)
extern int __xstat(int version, const char *path, struct stat *info);
#endif

#include "libretro.h"
#include "muhtari.h"
#include "muhtari_version.h"
#include "palette.h"
#include "sha256.h"
#include "status_text.h"
#ifdef MUHTARI_LIBRETRO_TEST
#include "internal.h"
static muhtari__audio_observer lr_test_observer;
static void *lr_test_observer_user;
RETRO_API void muhtari__libretro_observe(muhtari__audio_observer callback, void *user);
RETRO_API void muhtari__libretro_observe(muhtari__audio_observer callback, void *user)
{
    lr_test_observer = callback;
    lr_test_observer_user = user;
}
#endif

#define LR_WIDTH 320u
#define LR_HEIGHT 242u
#define LR_MAX_HEIGHT 293u
#define LR_PIXELS (LR_WIDTH * 292u)
/* PAL needs up to962 samples; selected.max_audio_samples bounds each run. */
#define LR_AUDIO_CAPACITY 962u
/* The larger BIOS image, 16 KiB; one byte more is read so that a longer
   file reaches muhtari_load, which refuses it. */
#define LR_BIOS_MAX 16384u
/* A 64 KiB CPU bus in aligned power-of-two pieces needs at most two
   descriptors per address bit for each of the map's spans. */
#define LR_MAX_DESCRIPTORS 64u
#define LR_JOYSTICK RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_JOYPAD, 1)
#define LR_PROLINE RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_JOYPAD, 0)
#define LR_SAVEKEY RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_JOYPAD, 2)
#define LR_ATARIVOX RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_JOYPAD, 3)
#define LR_DRIVING RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_MOUSE, 0)
#define LR_CX22 RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_MOUSE, 1)
#define LR_ST_MOUSE RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_MOUSE, 2)
#define LR_AMIGA_MOUSE RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_MOUSE, 3)
#define LR_LIGHTGUN RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_LIGHTGUN, 0)
#define LR_PADDLE RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_ANALOG, 0)
#define LR_KEYPAD RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_JOYPAD, 4)
#define LR_SNES2ATARI RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_JOYPAD, 5)
#define LR_MEGA7800 RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_JOYPAD, 6)

#ifdef USE_RGB565
typedef uint16_t lr_pixel;
#define LR_PIXEL_FORMAT RETRO_PIXEL_FORMAT_RGB565
#define LR_PALETTE muhtari__palette_rgb565_region
#else
typedef uint32_t lr_pixel;
#define LR_PIXEL_FORMAT RETRO_PIXEL_FORMAT_XRGB8888
#define LR_PALETTE muhtari__palette_rgb_region
#endif

/* All adapter state. A frontend may keep the module loaded between sessions,
   so retro_deinit returns every one of these to its initial value (L7594). */
static retro_environment_t env_cb;
static retro_video_refresh_t video_cb;
static retro_audio_sample_t audio_cb;
static retro_audio_sample_batch_t audio_batch_cb;
static retro_input_poll_t input_poll_cb;
static retro_input_state_t input_state_cb;
static retro_log_printf_t log_cb;
static muhtari *inst;
static muhtari_cart_info cart;
static muhtari_info selected;
static unsigned selected_region;
static uint32_t selected_palette = MUHTARI_REGION_NTSC;
static uint8_t *content;
static size_t content_size;
static uint8_t *native;
static lr_pixel *pixels;
static int16_t mono[LR_AUDIO_CAPACITY];
static int16_t stereo[LR_AUDIO_CAPACITY * 2u];
static unsigned device[2] = {RETRO_DEVICE_JOYPAD, RETRO_DEVICE_JOYPAD};
static bool difficulty_a[2];   /* the left and right switches, true at A */
static bool difficulty_opt[2]; /* the difficulty options when last read */
static bool edge_held[2];      /* port 1's L and R at the last poll */
static unsigned console_port;
static uint32_t console_device;
static unsigned console_ids[3];
static bool console_changed;
static void lr_refresh_inputs(void);
static bool run_failed; /* a failed frame was logged */
static struct retro_memory_descriptor descs[LR_MAX_DESCRIPTORS];
static struct retro_memory_map memory_map; /* outlives SET_MEMORY_MAPS */
static unsigned desc_count;
static uint32_t desc_next; /* the first CPU address no descriptor covers yet */
static muhtari_battery_info battery;
static uint32_t loaded_port[2], last_request[4];
static bool request_seen;

/* Writes a message to the frontend's log, when it has one (L1243). */
#define LR_LOG(level, ...)                                                                         \
    do {                                                                                           \
        if (log_cb != NULL) {                                                                      \
            log_cb(level, __VA_ARGS__);                                                            \
        }                                                                                          \
    } while (0)

/* ---- the frontend's view: devices, inputs and options ---- */

/* SET_CONTROLLER_INFO (L1506): the same choices on both ports. Auto
   follows the cartridge header's controller byte. */
static const struct retro_controller_description pad_types[] = {
    {"Auto (cartridge header)", RETRO_DEVICE_JOYPAD},
    {"ProLine", LR_PROLINE},
    {"Joystick", LR_JOYSTICK},
    {"None", RETRO_DEVICE_NONE},
    {"SaveKey", LR_SAVEKEY},
    {"AtariVox EEPROM", LR_ATARIVOX},
    {"Driving", LR_DRIVING},
    {"XG-1", LR_LIGHTGUN},
    {"Paddle pair", LR_PADDLE},
    {"Keypad", LR_KEYPAD},
    {"CX22", LR_CX22},
    {"ST mouse", LR_ST_MOUSE},
    {"Amiga mouse", LR_AMIGA_MOUSE},
    {"SNES2Atari", LR_SNES2ATARI},
    {"Mega7800", LR_MEGA7800},
};
static struct retro_controller_info ports[] = {
    {pad_types, 15u},
    {pad_types, 15u},
    {NULL, 0u},
};

/* SET_INPUT_DESCRIPTORS (L882): ProSystem's layout (HOSTS-AND-INTEGRATION
   section 1, Input). */
static const struct retro_input_descriptor pad_inputs[] = {
    {0u, RETRO_DEVICE_JOYPAD, 0u, RETRO_DEVICE_ID_JOYPAD_UP, "Up"},
    {0u, RETRO_DEVICE_JOYPAD, 0u, RETRO_DEVICE_ID_JOYPAD_DOWN, "Down"},
    {0u, RETRO_DEVICE_JOYPAD, 0u, RETRO_DEVICE_ID_JOYPAD_LEFT, "Left"},
    {0u, RETRO_DEVICE_JOYPAD, 0u, RETRO_DEVICE_ID_JOYPAD_RIGHT, "Right"},
    {0u, RETRO_DEVICE_JOYPAD, 0u, RETRO_DEVICE_ID_JOYPAD_B, "Button 1"},
    {0u, RETRO_DEVICE_JOYPAD, 0u, RETRO_DEVICE_ID_JOYPAD_A, "Button 2"},
    {0u, RETRO_DEVICE_JOYPAD, 0u, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select"},
    {0u, RETRO_DEVICE_JOYPAD, 0u, RETRO_DEVICE_ID_JOYPAD_START, "Pause"},
    {0u, RETRO_DEVICE_JOYPAD, 0u, RETRO_DEVICE_ID_JOYPAD_X, "Reset"},
    {0u, RETRO_DEVICE_JOYPAD, 0u, RETRO_DEVICE_ID_JOYPAD_L, "Left difficulty"},
    {0u, RETRO_DEVICE_JOYPAD, 0u, RETRO_DEVICE_ID_JOYPAD_R, "Right difficulty"},
    {1u, RETRO_DEVICE_JOYPAD, 0u, RETRO_DEVICE_ID_JOYPAD_UP, "Up"},
    {1u, RETRO_DEVICE_JOYPAD, 0u, RETRO_DEVICE_ID_JOYPAD_DOWN, "Down"},
    {1u, RETRO_DEVICE_JOYPAD, 0u, RETRO_DEVICE_ID_JOYPAD_LEFT, "Left"},
    {1u, RETRO_DEVICE_JOYPAD, 0u, RETRO_DEVICE_ID_JOYPAD_RIGHT, "Right"},
    {1u, RETRO_DEVICE_JOYPAD, 0u, RETRO_DEVICE_ID_JOYPAD_B, "Button 1"},
    {1u, RETRO_DEVICE_JOYPAD, 0u, RETRO_DEVICE_ID_JOYPAD_A, "Button 2"},
    {0u, 0u, 0u, 0u, NULL},
};
/* Row-major keypad1..9,*,0,#. These are base RetroPad identities,
   not a keyboard or raw physical-device abstraction (docs/libretro.md). */
static const unsigned keypad_ids[12] = {
    RETRO_DEVICE_ID_JOYPAD_Y,      RETRO_DEVICE_ID_JOYPAD_UP,   RETRO_DEVICE_ID_JOYPAD_X,
    RETRO_DEVICE_ID_JOYPAD_LEFT,   RETRO_DEVICE_ID_JOYPAD_B,    RETRO_DEVICE_ID_JOYPAD_RIGHT,
    RETRO_DEVICE_ID_JOYPAD_L,      RETRO_DEVICE_ID_JOYPAD_DOWN, RETRO_DEVICE_ID_JOYPAD_R,
    RETRO_DEVICE_ID_JOYPAD_SELECT, RETRO_DEVICE_ID_JOYPAD_A,    RETRO_DEVICE_ID_JOYPAD_START};
static struct retro_input_descriptor inputs[28];
/* Conventional RetroPad layouts; the console uses separate supplemental
   controls. See docs/libretro.md and libretro.h's base-device contract. */
static const unsigned adapter_ids[12] = {
    RETRO_DEVICE_ID_JOYPAD_UP,    RETRO_DEVICE_ID_JOYPAD_DOWN,   RETRO_DEVICE_ID_JOYPAD_LEFT,
    RETRO_DEVICE_ID_JOYPAD_RIGHT, RETRO_DEVICE_ID_JOYPAD_B,      RETRO_DEVICE_ID_JOYPAD_A,
    RETRO_DEVICE_ID_JOYPAD_Y,     RETRO_DEVICE_ID_JOYPAD_X,      RETRO_DEVICE_ID_JOYPAD_L,
    RETRO_DEVICE_ID_JOYPAD_R,     RETRO_DEVICE_ID_JOYPAD_SELECT, RETRO_DEVICE_ID_JOYPAD_START};
static const uint32_t adapter_buttons[2][12] = {
    {MUHTARI_BUTTON_UP, MUHTARI_BUTTON_DOWN, MUHTARI_BUTTON_LEFT, MUHTARI_BUTTON_RIGHT,
     MUHTARI_BUTTON_1, MUHTARI_BUTTON_2, MUHTARI_SNES_BUTTON_Y, MUHTARI_SNES_BUTTON_X,
     MUHTARI_SNES_BUTTON_L, MUHTARI_SNES_BUTTON_R, MUHTARI_SNES_BUTTON_SELECT,
     MUHTARI_SNES_BUTTON_START},
    {MUHTARI_BUTTON_UP, MUHTARI_BUTTON_DOWN, MUHTARI_BUTTON_LEFT, MUHTARI_BUTTON_RIGHT,
     MUHTARI_BUTTON_1, MUHTARI_MEGA_BUTTON_C, MUHTARI_BUTTON_2, MUHTARI_MEGA_BUTTON_Y,
     MUHTARI_MEGA_BUTTON_X, MUHTARI_MEGA_BUTTON_Z, MUHTARI_MEGA_BUTTON_MODE,
     MUHTARI_MEGA_BUTTON_START}};

/* The options (SET_CORE_OPTIONS_V2, L2339). Region is captured at load;
   presentation may change while the selected console keeps running. */
static struct retro_core_option_v2_category option_categories[] = {{NULL, NULL, NULL}};
static struct retro_core_option_v2_definition options_v2[] = {
    {.key = "muhtari_boot",
     .desc = "Boot",
     .info = "Fast starts the cartridge at once. BIOS boots through 7800 BIOS (U).rom or 7800 "
             "BIOS (E).rom in the system directory, or fast when neither is there. Applies when "
             "content is loaded.",
     .values = {{"fast", "Fast"}, {"bios", "BIOS"}, {NULL, NULL}},
     .default_value = "fast"},
    {.key = "muhtari_region",
     .desc = "Region",
     .info = "Auto follows cartridge metadata. Changes require content reload; Restart keeps "
             "the loaded region.",
     .values = {{"auto", "Auto"}, {"ntsc", "NTSC"}, {"pal", "PAL"}, {NULL, NULL}},
     .default_value = "auto"},
    {.key = "muhtari_palette",
     .desc = "Palette",
     .info = "Auto follows the loaded region. Explicit colours change presentation only.",
     .values = {{"auto", "Auto"}, {"ntsc", "NTSC (warm)"}, {"pal", "PAL (warm)"}, {NULL, NULL}},
     .default_value = "auto"},
    {.key = "muhtari_left_difficulty",
     .desc = "Left difficulty",
     .info = "The left difficulty switch at load and Restart; a change sets it at once. Port 1's "
             "L flips it.",
     .values = {{"B", "B"}, {"A", "A"}, {NULL, NULL}},
     .default_value = "B"},
    {.key = "muhtari_right_difficulty",
     .desc = "Right difficulty",
     .info = "The right difficulty switch at load and Restart; a change sets it at once. Port 1's "
             "R flips it.",
     .values = {{"B", "B"}, {"A", "A"}, {NULL, NULL}},
     .default_value = "B"},
    {.key = "muhtari_hsc",
     .desc = "High Score Cartridge",
     .info = "Storage attachment; changes require content reload.",
     .values = {{"auto", "Auto"}, {"on", "On"}, {"off", "Off"}, {NULL, NULL}},
     .default_value = "auto"},
    {.key = "muhtari_expansion",
     .desc = "Expansion hardware",
     .info = "Load-time selection; changes require content reload. XM RAM/POKEY/HSC subset, "
             "without YM2151, Covox/ADPCM, internal BIOS or full-XM services.",
     .values = {{"auto", "Auto (cartridge request)"},
                {"off", "Off"},
                {"xboard", "XBoard"},
                {"xm", "XM RAM/POKEY/HSC subset"},
                {NULL, NULL}},
     .default_value = "auto"},
    {.key = "muhtari_battery_port1",
     .desc = "Port 1 battery device",
     .info = "Overrides controller selection at content load; changes require reload.",
     .values = {{"auto", "Auto"},
                {"none", "None"},
                {"savekey", "SaveKey"},
                {"atarivox", "AtariVox EEPROM"},
                {NULL, NULL}},
     .default_value = "auto"},
    {.key = "muhtari_battery_port2",
     .desc = "Port 2 battery device",
     .info = "Overrides controller selection at content load; changes require reload.",
     .values = {{"auto", "Auto"},
                {"none", "None"},
                {"savekey", "SaveKey"},
                {"atarivox", "AtariVox EEPROM"},
                {NULL, NULL}},
     .default_value = "auto"},
    {.key = NULL},
};
static struct retro_core_options_v2 options = {option_categories, options_v2};
/* The v1 form of the same options (SET_CORE_OPTIONS, L1922), filled from
   options_v2 when a frontend has no v2. */
static struct retro_core_option_definition options_v1[sizeof options_v2 / sizeof options_v2[0]];
static const char *const difficulty_keys[2] = {"muhtari_left_difficulty",
                                               "muhtari_right_difficulty"};

/* The allocator the adapter hands the core: the C library's malloc and free. */
static void *lr_alloc(void *user, size_t size)
{
    (void)user;
    return malloc(size);
}

static void lr_free(void *user, void *ptr, size_t size)
{
    (void)user;
    (void)size;
    free(ptr);
}

/* Destroys the instance, the content copy, the frame buffers and the map. */
static void lr_release(void)
{
    muhtari_destroy(inst);
    inst = NULL;
    free(content);
    content = NULL;
    content_size = 0u;
    free(native);
    native = NULL;
    free(pixels);
    pixels = NULL;
    memset(&cart, 0, sizeof cart);
    memset(&selected, 0, sizeof selected);
    selected_region = RETRO_REGION_NTSC;
    selected_palette = MUHTARI_REGION_NTSC;
    memset(descs, 0, sizeof descs);
    memset(&memory_map, 0, sizeof memory_map);
    desc_count = 0u;
    desc_next = 0u;
    run_failed = false;
    memset(&battery, 0, sizeof battery);
    memset(loaded_port, 0, sizeof loaded_port);
    memset(last_request, 0, sizeof last_request);
    request_seen = false;
}

/* Declares the options: v2 when the frontend has it (L1848), else v1. */
static void lr_set_options(void)
{
    unsigned version = 0u;
    size_t i;
    if (!env_cb(RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION, &version)) {
        version = 0u;
    }
    if (version >= 2u && env_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_V2, &options)) {
        return;
    }
    if (version >= 1u) {
        for (i = 0; i < sizeof options_v2 / sizeof options_v2[0]; i++) {
            options_v1[i].key = options_v2[i].key;
            options_v1[i].desc = options_v2[i].desc;
            options_v1[i].info = options_v2[i].info;
            memcpy(options_v1[i].values, options_v2[i].values, sizeof options_v1[i].values);
            options_v1[i].default_value = options_v2[i].default_value;
        }
        (void)env_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS, options_v1);
    }
}

/* Whether option key has value (GET_VARIABLE, L966). */
static bool lr_option_is(const char *key, const char *value)
{
    struct retro_variable var;
    var.key = key;
    var.value = NULL;
    return env_cb != NULL && env_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value != NULL &&
           strcmp(var.value, value) == 0;
}

/* Resolve presentation against the successful session, never a pending region
   option. Missing/unknown values use Auto. */
static void lr_read_palette(void)
{
    selected_palette = lr_option_is("muhtari_palette", "pal")    ? MUHTARI_REGION_PAL
                       : lr_option_is("muhtari_palette", "ntsc") ? MUHTARI_REGION_NTSC
                       : selected_region == RETRO_REGION_PAL     ? MUHTARI_REGION_PAL
                                                                 : MUHTARI_REGION_NTSC;
}

/* Reads the difficulty options. At load and Restart (all) they set the
   switches; otherwise only an option whose value changed sets its switch,
   so a flip by L or R stays until the option itself is changed. */
static void lr_read_difficulty(bool all)
{
    unsigned i;
    for (i = 0; i < 2u; i++) {
        bool a = lr_option_is(difficulty_keys[i], "A");
        if (all || a != difficulty_opt[i]) {
            difficulty_a[i] = a;
        }
        difficulty_opt[i] = a;
    }
}

/* Shows text on screen for two seconds (SET_MESSAGE, L803). */
static void lr_message(const char *text)
{
    struct retro_message msg;
    msg.msg = text;
    msg.frames = 120u;
    if (env_cb != NULL) {
        (void)env_cb(RETRO_ENVIRONMENT_SET_MESSAGE, &msg);
    }
}

/* L7511: called before retro_init. Declaring no-game support (18, L1051)
   makes the frontend call retro_load_game(NULL) (L1040-1051). The device
   list and the options are declared here, as L1506 and L2339 ask. */
void retro_set_environment(retro_environment_t cb)
{
    bool no_game = true;
    env_cb = cb;
    if (cb == NULL) {
        return;
    }
    (void)cb(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &no_game);
    (void)cb(RETRO_ENVIRONMENT_SET_CONTROLLER_INFO, ports);
    lr_set_options();
}

void retro_set_video_refresh(retro_video_refresh_t cb)
{
    video_cb = cb;
}
/* L7453: the frontend may provide either audio callback; retro_run prefers
   the batch callback when both are registered. */
void retro_set_audio_sample(retro_audio_sample_t cb)
{
    audio_cb = cb;
}
void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
    audio_batch_cb = cb;
}
void retro_set_input_poll(retro_input_poll_t cb)
{
    input_poll_cb = cb;
}
void retro_set_input_state(retro_input_state_t cb)
{
    input_state_cb = cb;
}

/* L7580: the instance is created at load, so there is nothing to do here. */
void retro_init(void)
{
}

/* L7594: reset every global; the module may be used again. */
void retro_deinit(void)
{
    lr_release();
    env_cb = NULL;
    video_cb = NULL;
    audio_cb = NULL;
    audio_batch_cb = NULL;
    input_poll_cb = NULL;
    input_state_cb = NULL;
    log_cb = NULL;
    memset(mono, 0, sizeof mono);
    memset(stereo, 0, sizeof stereo);
    device[0] = RETRO_DEVICE_JOYPAD;
    device[1] = RETRO_DEVICE_JOYPAD;
    memset(difficulty_a, 0, sizeof difficulty_a);
    memset(difficulty_opt, 0, sizeof difficulty_opt);
    memset(edge_held, 0, sizeof edge_held);
    console_port = 0u;
    console_device = 0u;
    console_changed = false;
    memset(options_v1, 0, sizeof options_v1);
}

unsigned retro_api_version(void)
{
    return RETRO_API_VERSION;
}

/* L7627: static strings. The content is passed in memory (need_fullpath
   false), and the core keeps its own copy. */
void retro_get_system_info(struct retro_system_info *info)
{
    if (info == NULL) {
        return;
    }
    memset(info, 0, sizeof *info);
    info->library_name = "muhtari";
    info->library_version = MUHTARI_VERSION_STRING;
    info->valid_extensions = "a78|bin";
    info->need_fullpath = false;
    info->block_extract = false;
}

/* L7642, L6247-6305: geometry and timing. The base size is the NTSC frame,
   rows 16-257 (HOSTS-AND-INTEGRATION §1); 293 rows cover the PAL frame. A
   7800 pixel is 6:7, so the 320x242 frame shows at 960/847. The rate is
   (315/44 MHz) / (454 x 263) = 39375000/656711 Hz. Dividing here is the only
   floating point in the shipped code. */
void retro_get_system_av_info(struct retro_system_av_info *info)
{
    if (info == NULL) {
        return;
    }
    memset(info, 0, sizeof *info);
    info->geometry.base_width = LR_WIDTH;
    info->geometry.base_height = LR_HEIGHT;
    info->geometry.max_width = LR_WIDTH;
    info->geometry.max_height = LR_MAX_HEIGHT;
    info->geometry.aspect_ratio = (float)(320.0 * 6.0 / (242.0 * 7.0));
    info->timing.fps = 39375000.0 / 656711.0;
    info->timing.sample_rate = 48000.0;
    if (selected.height != 0u) {
        info->geometry.base_width = selected.width;
        info->geometry.base_height = selected.height;
        info->geometry.aspect_ratio = (float)((double)selected.width * selected.par_num /
                                              ((double)selected.height * selected.par_den));
        info->timing.fps = (double)selected.fps_num / selected.fps_den;
        info->timing.sample_rate = (double)selected.sample_rate_num / selected.sample_rate_den;
    }
}

/* L7673: unknown identities safely unplug ordinary input. */
void retro_set_controller_port_device(unsigned port, unsigned id)
{
    if (port < 2u) {
        bool known = false;
        for (unsigned i = 0u; i < sizeof pad_types / sizeof pad_types[0]; i++)
            known |= pad_types[i].id == id;
        if (!known)
            LR_LOG(RETRO_LOG_WARN, "muhtari: unknown controller %u; selecting None\n", id);
        device[port] = known ? id : RETRO_DEVICE_NONE;
        if (inst != NULL)
            lr_refresh_inputs();
    }
}

/* The core's device for a port: 0 (the cartridge's), ProLine, Joystick or
   unplugged. */
static uint32_t lr_port_device(unsigned port)
{
    switch (device[port]) {
    case LR_PROLINE:
        return MUHTARI_PORT_PROLINE;
    case LR_JOYSTICK:
        return MUHTARI_PORT_JOYSTICK;
    case RETRO_DEVICE_NONE:
        return MUHTARI_PORT_UNPLUGGED;
    case LR_SAVEKEY:
        return MUHTARI_PORT_SAVEKEY;
    case LR_ATARIVOX:
        return MUHTARI_PORT_ATARIVOX;
    case LR_DRIVING:
        return MUHTARI_PORT_DRIVING;
    case LR_CX22:
        return MUHTARI_PORT_TRAKBALL;
    case LR_ST_MOUSE:
        return MUHTARI_PORT_ST_MOUSE;
    case LR_AMIGA_MOUSE:
        return MUHTARI_PORT_AMIGA_MOUSE;
    case LR_LIGHTGUN:
        return MUHTARI_PORT_LIGHTGUN;
    case LR_PADDLE:
        return MUHTARI_PORT_PADDLE;
    case LR_KEYPAD:
        return MUHTARI_PORT_KEYPAD;
    case LR_SNES2ATARI:
        return MUHTARI_PORT_SNES2ATARI;
    case LR_MEGA7800:
        return MUHTARI_PORT_MEGA7800;
    default:
        return 0u;
    }
}

static bool lr_serial(uint32_t d)
{
    return d == MUHTARI_PORT_SAVEKEY || d == MUHTARI_PORT_ATARIVOX;
}

/* Load options take precedence over controller requests, then cartridge metadata
   (docs/libretro.md). An explicit none keeps an ordinary non-save controller. */
static void lr_battery_request(muhtari_media *media)
{
    static const char *const keys[2] = {"muhtari_battery_port1", "muhtari_battery_port2"};
    media->expansion = lr_option_is("muhtari_expansion", "off")      ? MUHTARI_EXPANSION_OFF
                       : lr_option_is("muhtari_expansion", "xboard") ? MUHTARI_EXPANSION_XBOARD
                       : lr_option_is("muhtari_expansion", "xm")     ? MUHTARI_EXPANSION_XM
                                                                     : MUHTARI_EXPANSION_AUTO;
    media->hsc_mode = lr_option_is("muhtari_hsc", "on")    ? MUHTARI_HSC_ON
                      : lr_option_is("muhtari_hsc", "off") ? MUHTARI_HSC_OFF
                                                           : MUHTARI_HSC_AUTO;
    for (unsigned p = 0u; p < 2u; p++) {
        uint32_t d = lr_port_device(p);
        if (lr_option_is(keys[p], "savekey"))
            d = MUHTARI_PORT_SAVEKEY;
        else if (lr_option_is(keys[p], "atarivox"))
            d = MUHTARI_PORT_ATARIVOX;
        else if (lr_option_is(keys[p], "none")) {
            if (d == 0u || lr_serial(d))
                d = MUHTARI_PORT_PROLINE;
        }
        media->port[p] = d;
    }
}

static bool lr_mouse(uint32_t d)
{
    return d == MUHTARI_PORT_TRAKBALL || d == MUHTARI_PORT_ST_MOUSE ||
           d == MUHTARI_PORT_AMIGA_MOUSE;
}

static bool lr_adapter(uint32_t d)
{
    return d == MUHTARI_PORT_SNES2ATARI || d == MUHTARI_PORT_MEGA7800;
}

static uint32_t lr_active_port(unsigned p, const muhtari_media *request)
{
    if (battery.eeprom_port == p + 1u)
        return battery.eeprom_device;
    uint32_t d = lr_serial(request->port[p]) ? loaded_port[p] : request->port[p];
    if (d != 0u)
        return d;
    /* Match the core's Auto rules (docs/embedding.md, Input), rather than
       passing an unimplemented header device as an explicit request. */
    d = p == 0u ? cart.port1 : cart.port2;
    if (d == MUHTARI_PORT_NONE && content != NULL)
        return MUHTARI_PORT_UNPLUGGED;
    return d == MUHTARI_PORT_JOYSTICK || d == MUHTARI_PORT_DRIVING || d == MUHTARI_PORT_LIGHTGUN ||
                   d == MUHTARI_PORT_PADDLE || d == MUHTARI_PORT_KEYPAD || lr_mouse(d) ||
                   lr_adapter(d)
               ? d
               : MUHTARI_PORT_PROLINE;
}

static bool lr_ordinary(uint32_t d)
{
    return d == MUHTARI_PORT_PROLINE || d == MUHTARI_PORT_JOYSTICK;
}

static unsigned lr_console_base(void)
{
    return (console_device == MUHTARI_PORT_DRIVING || lr_mouse(console_device)) ? RETRO_DEVICE_MOUSE
           : console_device == MUHTARI_PORT_LIGHTGUN ? RETRO_DEVICE_LIGHTGUN
                                                     : RETRO_DEVICE_JOYPAD;
}

/* Descriptor storage outlives the environment callback. IDs are the base
   abstractions actually polled (libretro.h, input descriptor contract). */
static void lr_refresh_inputs(void)
{
    muhtari_media request = {0};
    uint32_t active[2];
    unsigned provider = 0u, n = 0u;
    lr_battery_request(&request);
    for (unsigned p = 0u; p < 2u; p++)
        active[p] = lr_active_port(p, &request);
    if (!lr_ordinary(active[0])) {
        if (lr_ordinary(active[1]))
            provider = 1u;
        else if (!lr_mouse(active[0]) && active[0] != MUHTARI_PORT_DRIVING &&
                 active[0] != MUHTARI_PORT_LIGHTGUN && active[0] != MUHTARI_PORT_PADDLE &&
                 active[0] != MUHTARI_PORT_KEYPAD && !lr_adapter(active[0]) &&
                 (lr_mouse(active[1]) || active[1] == MUHTARI_PORT_DRIVING ||
                  active[1] == MUHTARI_PORT_LIGHTGUN || active[1] == MUHTARI_PORT_PADDLE ||
                  active[1] == MUHTARI_PORT_KEYPAD || lr_adapter(active[1])))
            provider = 1u;
    }
    uint32_t kind = lr_ordinary(active[provider]) || active[provider] == MUHTARI_PORT_DRIVING ||
                            active[provider] == MUHTARI_PORT_LIGHTGUN ||
                            active[provider] == MUHTARI_PORT_PADDLE ||
                            active[provider] == MUHTARI_PORT_KEYPAD || lr_mouse(active[provider]) ||
                            lr_adapter(active[provider])
                        ? active[provider]
                        : MUHTARI_PORT_UNPLUGGED;
    if (console_port != provider || console_device != kind)
        console_changed = true;
    console_port = provider;
    console_device = kind;
    bool driving = kind == MUHTARI_PORT_DRIVING;
    bool mouse = lr_mouse(kind);
    bool gun = kind == MUHTARI_PORT_LIGHTGUN;
    bool pad_console = lr_ordinary(kind) || kind == MUHTARI_PORT_PADDLE;
    console_ids[0] = pad_console        ? RETRO_DEVICE_ID_JOYPAD_START
                     : driving || mouse ? RETRO_DEVICE_ID_MOUSE_MIDDLE
                     : gun              ? RETRO_DEVICE_ID_LIGHTGUN_START
                                        : RETRO_DEVICE_ID_JOYPAD_L2;
    console_ids[1] = pad_console        ? RETRO_DEVICE_ID_JOYPAD_SELECT
                     : driving || mouse ? RETRO_DEVICE_ID_MOUSE_BUTTON_4
                     : gun              ? RETRO_DEVICE_ID_LIGHTGUN_SELECT
                                        : RETRO_DEVICE_ID_JOYPAD_R2;
    console_ids[2] = pad_console ? RETRO_DEVICE_ID_JOYPAD_X
                     : mouse     ? RETRO_DEVICE_ID_MOUSE_BUTTON_5
                     : driving   ? RETRO_DEVICE_ID_MOUSE_RIGHT
                     : gun       ? RETRO_DEVICE_ID_LIGHTGUN_AUX_A
                                 : RETRO_DEVICE_ID_JOYPAD_L3;
    for (unsigned p = 0u; p < 2u; p++) {
        if (lr_ordinary(active[p])) {
            for (unsigned i = 0u; i < 6u; i++) {
                inputs[n] = pad_inputs[i];
                inputs[n++].port = p;
            }
        } else if (lr_adapter(active[p])) {
            static const char *const names[2][12] = {
                {"Up", "Down", "Left", "Right", "B", "A", "Y", "X", "L", "R", "Select", "Start"},
                {"Up", "Down", "Left", "Right", "B", "C", "A", "Y", "X", "Z", "Mode", "Start"}};
            unsigned family = active[p] == MUHTARI_PORT_MEGA7800 ? 1u : 0u;
            for (unsigned i = 0u; i < 12u; i++)
                inputs[n++] = (struct retro_input_descriptor){p, RETRO_DEVICE_JOYPAD, 0u,
                                                              adapter_ids[i], names[family][i]};
        } else if (active[p] == MUHTARI_PORT_KEYPAD) {
            static const char *const names[12] = {"Key 1", "Key 2", "Key 3", "Key 4",
                                                  "Key 5", "Key 6", "Key 7", "Key 8",
                                                  "Key 9", "Key *", "Key 0", "Key #"};
            for (unsigned i = 0u; i < 12u; i++)
                inputs[n++] = (struct retro_input_descriptor){p, RETRO_DEVICE_JOYPAD, 0u,
                                                              keypad_ids[i], names[i]};
        } else if (active[p] == MUHTARI_PORT_PADDLE) {
            inputs[n++] = (struct retro_input_descriptor){
                p, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_X,
                "Paddle 1 position"};
            inputs[n++] = (struct retro_input_descriptor){
                p, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_X,
                "Paddle 2 position"};
            inputs[n++] = (struct retro_input_descriptor){
                p, RETRO_DEVICE_JOYPAD, 0u, RETRO_DEVICE_ID_JOYPAD_B, "Paddle 1 button"};
            inputs[n++] = (struct retro_input_descriptor){
                p, RETRO_DEVICE_JOYPAD, 0u, RETRO_DEVICE_ID_JOYPAD_A, "Paddle 2 button"};
        } else if (active[p] == MUHTARI_PORT_LIGHTGUN) {
            static const unsigned ids[] = {
                RETRO_DEVICE_ID_LIGHTGUN_SCREEN_X, RETRO_DEVICE_ID_LIGHTGUN_SCREEN_Y,
                RETRO_DEVICE_ID_LIGHTGUN_TRIGGER, RETRO_DEVICE_ID_LIGHTGUN_IS_OFFSCREEN,
                RETRO_DEVICE_ID_LIGHTGUN_RELOAD};
            static const char *const names[] = {"Aim X", "Aim Y", "Trigger", "Offscreen", "Reload"};
            for (unsigned i = 0u; i < sizeof ids / sizeof ids[0]; i++)
                inputs[n++] =
                    (struct retro_input_descriptor){p, RETRO_DEVICE_LIGHTGUN, 0u, ids[i], names[i]};
        } else if (lr_mouse(active[p])) {
            static const unsigned ids[] = {RETRO_DEVICE_ID_MOUSE_X, RETRO_DEVICE_ID_MOUSE_Y,
                                           RETRO_DEVICE_ID_MOUSE_LEFT, RETRO_DEVICE_ID_MOUSE_RIGHT};
            const char *names[] = {"Motion X", "Motion Y", "Left fire", "Right fire"};
            if (active[p] == MUHTARI_PORT_TRAKBALL)
                names[2] = names[3] = "Fire";
            for (unsigned i = 0u; i < 4u; i++)
                inputs[n++] =
                    (struct retro_input_descriptor){p, RETRO_DEVICE_MOUSE, 0u, ids[i], names[i]};
        } else if (active[p] == MUHTARI_PORT_DRIVING) {
            inputs[n++] = (struct retro_input_descriptor){
                p, RETRO_DEVICE_MOUSE, 0u, RETRO_DEVICE_ID_MOUSE_X, "Driving motion"};
            inputs[n++] = (struct retro_input_descriptor){p, RETRO_DEVICE_MOUSE, 0u,
                                                          RETRO_DEVICE_ID_MOUSE_LEFT, "Fire"};
        }
    }
    if (lr_ordinary(kind)) {
        for (unsigned i = 6u; i < 11u; i++) {
            inputs[n] = pad_inputs[i];
            inputs[n++].port = provider;
        }
    } else {
        inputs[n++] = (struct retro_input_descriptor){provider, lr_console_base(), 0u,
                                                      console_ids[0], "Pause"};
        inputs[n++] = (struct retro_input_descriptor){provider, lr_console_base(), 0u,
                                                      console_ids[1], "Select"};
        inputs[n++] = (struct retro_input_descriptor){provider, lr_console_base(), 0u,
                                                      console_ids[2], "Reset"};
    }
    memset(&inputs[n], 0, sizeof inputs[n]);
    (void)env_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, inputs);
}

/* Same load-time rule as the core (docs/libretro.md, Options). Firmware
   discovery and reload checks must use physical attachment, not CPU visibility. */
static uint32_t lr_expansion(const muhtari_media *request)
{
    if (request->expansion != MUHTARI_EXPANSION_AUTO)
        return request->expansion;
    return (cart.devices & MUHTARI_DEVICE_XM) != 0u ? MUHTARI_EXPANSION_XM : MUHTARI_EXPANSION_OFF;
}

static bool lr_hsc_attached(const muhtari_media *request)
{
    return request->hsc_mode == MUHTARI_HSC_ON || (request->hsc_mode == MUHTARI_HSC_AUTO &&
                                                   ((cart.devices & MUHTARI_DEVICE_HSC) != 0u ||
                                                    lr_expansion(request) == MUHTARI_EXPANSION_XM));
}

static void lr_check_battery_request(const muhtari_media *request)
{
    uint32_t key[4] = {request->hsc_mode, request->port[0], request->port[1], request->expansion};
    if (request_seen && memcmp(key, last_request, sizeof key) == 0)
        return;
    memcpy(last_request, key, sizeof key);
    request_seen = true;
    bool hsc = lr_hsc_attached(request);
    unsigned count = 0u, port = 0u;
    uint32_t serial = 0u;
    for (unsigned p = 0u; p < 2u; p++) {
        uint32_t d = request->port[p] != 0u ? request->port[p] : p == 0u ? cart.port1 : cart.port2;
        if (request->port[p] == 0u && battery.eeprom_port == p + 1u)
            d = battery.eeprom_device;
        if (lr_serial(d)) {
            count++;
            port = p + 1u;
            serial = d;
        }
    }
    if (count == 0u && (cart.devices & MUHTARI_DEVICE_SAVEKEY) != 0u && request->port[1] == 0u) {
        count = 1u;
        port = 2u;
        serial = MUHTARI_PORT_SAVEKEY;
    }
    if (lr_expansion(request) != selected.expansion || hsc != (battery.hsc_attached != 0u) ||
        count > 1u || port != battery.eeprom_port || serial != battery.eeprom_device) {
        LR_LOG(RETRO_LOG_WARN,
               "muhtari: expansion or battery device change requires content reload\n");
        lr_message("Expansion or battery device change requires content reload");
    }
}

/* Whether RetroPad button id is held on port. */
static bool lr_held(unsigned port, unsigned id)
{
    return input_state_cb != NULL && input_state_cb(port, RETRO_DEVICE_JOYPAD, 0, id) != 0;
}

/* The 7800 buttons held on a port: the D-pad is the stick, B button 1 and
   A button 2. A 2600 joystick has one button, so there A fires too
   (HOSTS-AND-INTEGRATION section 1, Input). */
static uint32_t lr_buttons(unsigned port, uint32_t d)
{
    bool joystick = d == MUHTARI_PORT_JOYSTICK;
    uint32_t b = 0u;
    b |= lr_held(port, RETRO_DEVICE_ID_JOYPAD_UP) ? MUHTARI_BUTTON_UP : 0u;
    b |= lr_held(port, RETRO_DEVICE_ID_JOYPAD_DOWN) ? MUHTARI_BUTTON_DOWN : 0u;
    b |= lr_held(port, RETRO_DEVICE_ID_JOYPAD_LEFT) ? MUHTARI_BUTTON_LEFT : 0u;
    b |= lr_held(port, RETRO_DEVICE_ID_JOYPAD_RIGHT) ? MUHTARI_BUTTON_RIGHT : 0u;
    b |= lr_held(port, RETRO_DEVICE_ID_JOYPAD_B) ? MUHTARI_BUTTON_1 : 0u;
    if (lr_held(port, RETRO_DEVICE_ID_JOYPAD_A)) {
        b |= joystick ? MUHTARI_BUTTON_1 : MUHTARI_BUTTON_2;
    }
    return b;
}

/* Absolute base LIGHTGUN coordinates use the reported native dimensions.
   Treat the -32768 sentinel as offscreen; exclude it before
   arithmetic. Integer rounding and reload behavior: docs/libretro.md. */
static void lr_gun(unsigned p, muhtari_port_input *in)
{
    in->flags = MUHTARI_INPUT_GUN_OFFSCREEN;
    if (input_state_cb == NULL)
        return;
    int32_t x = input_state_cb(p, RETRO_DEVICE_LIGHTGUN, 0u, RETRO_DEVICE_ID_LIGHTGUN_SCREEN_X);
    int32_t y = input_state_cb(p, RETRO_DEVICE_LIGHTGUN, 0u, RETRO_DEVICE_ID_LIGHTGUN_SCREEN_Y);
    bool trigger =
        input_state_cb(p, RETRO_DEVICE_LIGHTGUN, 0u, RETRO_DEVICE_ID_LIGHTGUN_TRIGGER) != 0;
    bool offscreen =
        input_state_cb(p, RETRO_DEVICE_LIGHTGUN, 0u, RETRO_DEVICE_ID_LIGHTGUN_IS_OFFSCREEN) != 0;
    bool reload =
        input_state_cb(p, RETRO_DEVICE_LIGHTGUN, 0u, RETRO_DEVICE_ID_LIGHTGUN_RELOAD) != 0;
    in->buttons = trigger || reload ? MUHTARI_BUTTON_1 : 0u;
    muhtari_info info = {0};
    info.size = (uint32_t)sizeof info;
    if (x == INT16_MIN || y == INT16_MIN || muhtari_get_info(inst, &info) != MUHTARI_STATUS_OK ||
        info.width == 0u || info.height == 0u)
        return;
    in->gun_x = (uint32_t)((uint64_t)(x + 32767) * (info.width - 1u) / 65534u);
    in->gun_y = (uint32_t)((uint64_t)(y + 32767) * (info.height - 1u) / 65534u);
    in->flags = offscreen || reload ? MUHTARI_INPUT_GUN_OFFSCREEN : 0u;
}

/* Port 1's L and R flip the left and right difficulty switches on each
   press, with an on-screen message. */
static void lr_difficulty_edges(void)
{
    static const unsigned ids[2] = {RETRO_DEVICE_ID_JOYPAD_L, RETRO_DEVICE_ID_JOYPAD_R};
    static const char *const text[2][2] = {{"Left difficulty: B", "Left difficulty: A"},
                                           {"Right difficulty: B", "Right difficulty: A"}};
    unsigned i;
    for (i = 0; i < 2u; i++) {
        bool held = lr_held(console_port, ids[i]);
        if (held && !edge_held[i] && !console_changed) {
            difficulty_a[i] = !difficulty_a[i];
            lr_message(text[i][difficulty_a[i] ? 1 : 0]);
        }
        edge_held[i] = held;
    }
}

/* Hands the RetroPads' state to the instance; the core applies it at the
   next VBLANK start (docs/embedding.md, Input). Select, Start and X on
   port 1 are the console's Select, Pause and Reset. */
static void lr_input(void)
{
    muhtari_input_ex in;
    muhtari_media request = {0};
    bool updated = false;
    if (env_cb != NULL && env_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated) {
        lr_read_difficulty(false);
        lr_read_palette();
        lr_refresh_inputs();
    }
    if (lr_ordinary(console_device))
        lr_difficulty_edges();
    console_changed = false;
    lr_battery_request(&request);
    if (content != NULL)
        lr_check_battery_request(&request);
    memset(&in, 0, sizeof in);
    in.size = (uint32_t)sizeof in;
    for (unsigned p = 0u; p < 2u; p++) {
        in.port[p].device = lr_active_port(p, &request);
        if (lr_ordinary(in.port[p].device))
            in.port[p].buttons = lr_buttons(p, in.port[p].device);
        else if (lr_adapter(in.port[p].device)) {
            unsigned family = in.port[p].device == MUHTARI_PORT_MEGA7800 ? 1u : 0u;
            for (unsigned i = 0u; i < 12u; i++)
                if (lr_held(p, adapter_ids[i]))
                    in.port[p].buttons |= adapter_buttons[family][i];
        } else if (in.port[p].device == MUHTARI_PORT_KEYPAD) {
            for (unsigned i = 0u; i < 12u; i++)
                if (lr_held(p, keypad_ids[i]))
                    in.port[p].keypad |= 1u << i;
        } else if (in.port[p].device == MUHTARI_PORT_LIGHTGUN)
            lr_gun(p, &in.port[p]);
        else if (in.port[p].device == MUHTARI_PORT_PADDLE && input_state_cb != NULL) {
            in.port[p].paddle[0] =
                (uint32_t)((int32_t)input_state_cb(p, RETRO_DEVICE_ANALOG,
                                                   RETRO_DEVICE_INDEX_ANALOG_LEFT,
                                                   RETRO_DEVICE_ID_ANALOG_X) +
                           32768);
            in.port[p].paddle[1] =
                (uint32_t)((int32_t)input_state_cb(p, RETRO_DEVICE_ANALOG,
                                                   RETRO_DEVICE_INDEX_ANALOG_RIGHT,
                                                   RETRO_DEVICE_ID_ANALOG_X) +
                           32768);
            in.port[p].buttons = (lr_held(p, RETRO_DEVICE_ID_JOYPAD_B) ? MUHTARI_BUTTON_1 : 0u) |
                                 (lr_held(p, RETRO_DEVICE_ID_JOYPAD_A) ? MUHTARI_BUTTON_2 : 0u);
        } else if (lr_mouse(in.port[p].device) && input_state_cb != NULL) {
            in.port[p].dx = input_state_cb(p, RETRO_DEVICE_MOUSE, 0u, RETRO_DEVICE_ID_MOUSE_X);
            in.port[p].dy = input_state_cb(p, RETRO_DEVICE_MOUSE, 0u, RETRO_DEVICE_ID_MOUSE_Y);
            bool left = input_state_cb(p, RETRO_DEVICE_MOUSE, 0u, RETRO_DEVICE_ID_MOUSE_LEFT) != 0;
            bool right =
                input_state_cb(p, RETRO_DEVICE_MOUSE, 0u, RETRO_DEVICE_ID_MOUSE_RIGHT) != 0;
            in.port[p].buttons =
                in.port[p].device == MUHTARI_PORT_TRAKBALL
                    ? (left || right ? MUHTARI_BUTTON_1 : 0u)
                    : (left ? MUHTARI_BUTTON_1 : 0u) | (right ? MUHTARI_BUTTON_2 : 0u);
        } else if (in.port[p].device == MUHTARI_PORT_DRIVING && input_state_cb != NULL) {
            in.port[p].dx = input_state_cb(p, RETRO_DEVICE_MOUSE, 0u, RETRO_DEVICE_ID_MOUSE_X);
            in.port[p].buttons =
                input_state_cb(p, RETRO_DEVICE_MOUSE, 0u, RETRO_DEVICE_ID_MOUSE_LEFT) != 0
                    ? MUHTARI_BUTTON_1
                    : 0u;
        }
    }
    static const uint32_t switches[3] = {MUHTARI_SWITCH_PAUSE, MUHTARI_SWITCH_SELECT,
                                         MUHTARI_SWITCH_RESET};
    for (unsigned i = 0u; i < 3u; i++) {
        if (input_state_cb != NULL &&
            input_state_cb(console_port, lr_console_base(), 0u, console_ids[i]) != 0)
            in.switches |= switches[i];
    }
    in.switches |= difficulty_a[0] ? MUHTARI_SWITCH_LEFT_DIFFICULTY_A : 0u;
    in.switches |= difficulty_a[1] ? MUHTARI_SWITCH_RIGHT_DIFFICULTY_A : 0u;
    if (muhtari_set_input_ex(inst, &in) != MUHTARI_STATUS_OK) {
        LR_LOG(RETRO_LOG_ERROR, "muhtari: input request refused\n");
    }
}

/* Restart is a power cycle (HOSTS-AND-INTEGRATION section 1): the console's
   own Reset is a switch the game reads, on X. The difficulty options set
   the switches again. */
void retro_reset(void)
{
    if (inst == NULL || content == NULL) {
        return;
    }
    (void)muhtari_power_cycle(inst);
    lr_read_difficulty(true);
    run_failed = false;
}

/* L7694: one frame. Input is polled once (L7685), the video callback is
   called once with the converted rows, and mono samples are duplicated into
   left/right pairs. Prefer the batch callback (L7465); use the per-sample
   callback only when no batch callback is registered (L7453). A failed frame
   presents the last picture (black before the first frame) without audio;
   its first failure is reported. */
void retro_run(void)
{
    muhtari_frame io;
    muhtari_status st;
    uint32_t i;

    if (input_poll_cb != NULL) {
        input_poll_cb();
    }
    if (inst == NULL) {
        return;
    }
    lr_input();
    memset(&io, 0, sizeof io);
    io.size = (uint32_t)sizeof io;
    io.video = native;
    io.video_pitch = selected.width;
    io.audio = mono;
    io.audio_capacity = selected.max_audio_samples;
    st = muhtari_run_frame(inst, &io);
    if (st != MUHTARI_STATUS_OK) {
        if (!run_failed) {
            LR_LOG(RETRO_LOG_ERROR, "muhtari: frame failed: %s\n", muhtari__status_text(st));
            lr_message(muhtari__status_text(st));
            run_failed = true;
        }
        if (video_cb != NULL) {
            video_cb(pixels, selected.width, selected.height,
                     (size_t)selected.width * sizeof pixels[0]);
        }
        return;
    }
    for (i = 0; i < selected.width * selected.height; i++) {
        pixels[i] = LR_PALETTE(native[i], selected_palette);
    }
    if (video_cb != NULL) {
        video_cb(pixels, selected.width, selected.height,
                 (size_t)selected.width * sizeof pixels[0]);
    }
    for (i = 0; i < io.audio_count; i++) {
        stereo[2u * i] = mono[i];
        stereo[2u * i + 1u] = mono[i];
    }
    if (audio_batch_cb != NULL) {
        (void)audio_batch_cb(stereo, io.audio_count);
    } else if (audio_cb != NULL) {
        for (i = 0; i < io.audio_count; i++) {
            audio_cb(stereo[2u * i], stereo[2u * i + 1u]);
        }
    }
}

/* L7707: no save states yet; the size is 0 and nothing loads. */
size_t retro_serialize_size(void)
{
    return 0u;
}

bool retro_serialize(void *data, size_t len)
{
    (void)data;
    (void)len;
    return false;
}

bool retro_unserialize(const void *data, size_t len)
{
    (void)data;
    (void)len;
    return false;
}

void retro_cheat_reset(void)
{
}

void retro_cheat_set(unsigned index, bool enabled, const char *code)
{
    (void)index;
    (void)enabled;
    (void)code;
}

/* ---- loading ---- */

/* Try each BIOS in the system directory (L850), under the names of
   libretro's System.dat (HOSTS-AND-INTEGRATION section 1, Firmware).
   Unusable files allow the next candidate; allocation or content errors
   stop the load. Only when no BIOS works do we fall back to fast boot. */
static muhtari_status lr_load_bios(muhtari_media *media)
{
    static const char *const names[2] = {"7800 BIOS (U).rom", "7800 BIOS (E).rom"};
    const char *dir = NULL;
    char path[4096];
    unsigned i;
    bool unusable = false;
    if (!env_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, (void *)&dir)) {
        dir = NULL;
    }
    for (i = 0; dir != NULL && i < 2u; i++) {
        uint8_t *buf;
        FILE *f;
        size_t size;
        muhtari_status st;
        int written = snprintf(path, sizeof path, "%s/%s", dir, names[i]);
        if (written < 0 || (size_t)written >= sizeof path) {
            LR_LOG(RETRO_LOG_WARN, "muhtari: %s: BIOS path too long\n", names[i]);
            unusable = true;
            continue;
        }
        f = fopen(path, "rb");
        if (f == NULL) {
            if (errno != ENOENT) {
                LR_LOG(RETRO_LOG_WARN, "muhtari: %s: cannot open BIOS file\n", names[i]);
                unusable = true;
            }
            continue;
        }
        buf = (uint8_t *)malloc(LR_BIOS_MAX + 1u);
        if (buf == NULL) {
            (void)fclose(f);
            lr_message(muhtari__status_text(MUHTARI_STATUS_ERR_NO_MEMORY));
            return MUHTARI_STATUS_ERR_NO_MEMORY;
        }
        size = fread(buf, 1u, LR_BIOS_MAX + 1u, f);
        int read_error = ferror(f);
        (void)fclose(f);
        if (read_error) {
            LR_LOG(RETRO_LOG_WARN, "muhtari: %s: BIOS read error\n", names[i]);
            free(buf);
            unusable = true;
            continue;
        }
        media->bios = buf;
        media->bios_size = size;
        st = muhtari_load(inst, media);
        if (st == MUHTARI_STATUS_OK) {
            muhtari__sha256 sha;
            uint8_t digest[32];
            char hex[65];
            muhtari__sha256_init(&sha);
            muhtari__sha256_update(&sha, buf, size);
            muhtari__sha256_final(&sha, digest);
            muhtari__sha256_hex(digest, hex);
            LR_LOG(RETRO_LOG_INFO, "muhtari: BIOS %s, SHA-256 %s\n", names[i], hex);
        }
        free(buf);
        media->bios = NULL;
        media->bios_size = 0u;
        if (st != MUHTARI_STATUS_ERR_INVALID_BIOS) {
            return st;
        }
        LR_LOG(RETRO_LOG_WARN, "muhtari: %s: %s\n", names[i], muhtari__status_text(st));
        unusable = true;
    }
    if (unusable) {
        LR_LOG(RETRO_LOG_WARN, "muhtari: no usable 7800 BIOS file; fast boot\n");
        lr_message("No usable 7800 BIOS file: fast boot");
    } else {
        LR_LOG(RETRO_LOG_WARN, "muhtari: no 7800 BIOS (U).rom or 7800 BIOS (E).rom in the system "
                               "directory; fast boot\n");
        lr_message("No 7800 BIOS file: fast boot");
    }
    return muhtari_load(inst, media);
}

/* Optional HSC firmware has one name and exact size. A present bad file
   refuses loading; only absence selects the signature stub (docs/libretro.md). */
static muhtari_status lr_load_hsc(muhtari_media *media, uint8_t **owned)
{
    const char *dir = NULL;
    char path[4096];
    FILE *f;
    *owned = NULL;
    bool attached = lr_hsc_attached(media);
    if (!attached || !env_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, (void *)&dir) || dir == NULL)
        return MUHTARI_STATUS_OK;
    int n = snprintf(path, sizeof path, "%s/highscre.bin", dir);
    if (n < 0 || (size_t)n >= sizeof path)
        return MUHTARI_STATUS_ERR_INVALID_HSC;
#ifdef _WIN32
    /* CreateFile without BACKUP_SEMANTICS refuses directories; GetFileType
       distinguishes disk files from pipes/devices. No timestamps are needed.
       Microsoft Learn: CreateFileA, GetFileType, CloseHandle. */
    HANDLE probe = CreateFileA(path, 0u, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                               NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (probe == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        return error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND
                   ? MUHTARI_STATUS_OK
                   : MUHTARI_STATUS_ERR_INVALID_HSC;
    }
    DWORD type = GetFileType(probe);
    BOOL probe_closed = CloseHandle(probe);
    if (type != FILE_TYPE_DISK || !probe_closed)
        return MUHTARI_STATUS_ERR_INVALID_HSC;
#else
    struct stat info;
#if defined(__GLIBC__) &&                                                                          \
    (defined(__x86_64__) || defined(__aarch64__) ||                                                \
     (defined(__powerpc__) && !defined(__powerpc64__))) &&                                         \
    !defined(stat)
    /* LSB stat layouts: x86-64 1, AArch64 0, legacy PPC32 3. */
#ifdef __x86_64__
    int found = __xstat(1, path, &info);
#elif defined(__aarch64__)
    int found = __xstat(0, path, &info);
#else
    int found = __xstat(3, path, &info);
#endif
#else
    int found = stat(path, &info);
#endif
    if (found != 0)
        return errno == ENOENT ? MUHTARI_STATUS_OK : MUHTARI_STATUS_ERR_INVALID_HSC;
    if (!S_ISREG(info.st_mode))
        return MUHTARI_STATUS_ERR_INVALID_HSC;
#endif
    f = fopen(path, "rb");
    if (f == NULL)
        return MUHTARI_STATUS_ERR_INVALID_HSC;
    uint8_t *buf = (uint8_t *)malloc(4097u);
    if (buf == NULL) {
        (void)fclose(f);
        return MUHTARI_STATUS_ERR_NO_MEMORY;
    }
    size_t bytes = fread(buf, 1u, 4097u, f);
    int failed = ferror(f);
    int closed = fclose(f);
    if (bytes != 4096u || failed || closed != 0) {
        free(buf);
        return MUHTARI_STATUS_ERR_INVALID_HSC;
    }
    media->hsc_rom = buf;
    media->hsc_rom_size = bytes;
    *owned = buf;
    return MUHTARI_STATUS_OK;
}

/* Loads the content copy into the instance, through a BIOS file when the
   boot option asks for one and there is one. Logs the BIOS file and its
   SHA-256, or the fast-boot fallback, or a refusal with its status text,
   and returns false on a refusal. */
static bool lr_load_content(const struct retro_game_info *game)
{
    muhtari_media media;
    muhtari_status st;
    uint8_t *hsc = NULL;

    if (game->data == NULL || game->size == 0u || game->size > MUHTARI_IMAGE_SIZE_MAX) {
        LR_LOG(RETRO_LOG_ERROR, "muhtari: no content bytes, or more than %u\n",
               (unsigned)MUHTARI_IMAGE_SIZE_MAX);
        return false;
    }
    memset(&cart, 0, sizeof cart);
    cart.size = (uint32_t)sizeof cart;
    st = muhtari_probe(game->data, game->size, &cart);
    if (st == MUHTARI_STATUS_OK) {
        content = (uint8_t *)malloc(game->size);
        if (content == NULL) {
            st = MUHTARI_STATUS_ERR_NO_MEMORY;
        }
    }
    if (st == MUHTARI_STATUS_OK) {
        memcpy(content, game->data, game->size);
        content_size = game->size;
        memset(&media, 0, sizeof media);
        media.size = (uint32_t)sizeof media;
        media.image = content;
        media.image_size = content_size;
        media.region = lr_option_is("muhtari_region", "pal")    ? MUHTARI_REGION_PAL
                       : lr_option_is("muhtari_region", "ntsc") ? MUHTARI_REGION_NTSC
                                                                : 0u;
        lr_battery_request(&media);
        loaded_port[0] = media.port[0];
        loaded_port[1] = media.port[1];
        st = lr_load_hsc(&media, &hsc);
        if (st == MUHTARI_STATUS_OK && lr_option_is("muhtari_boot", "bios")) {
            st = lr_load_bios(&media);
        } else if (st == MUHTARI_STATUS_OK) {
            st = muhtari_load(inst, &media);
        }
        free(hsc);
    }
    if (st != MUHTARI_STATUS_OK) {
        LR_LOG(RETRO_LOG_ERROR, "muhtari: content refused: %s%s%s\n", muhtari__status_text(st),
               st == MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD ? ": " : "",
               st == MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD ? muhtari__board_name(cart.board) : "");
        if (st == MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE) {
            bool named = false;
            uint32_t refused =
                cart.sound & (MUHTARI_SOUND_YM2151 | MUHTARI_SOUND_COVOX | MUHTARI_SOUND_ADPCM);
            if (refused == 0u)
                refused = cart.sound;
            for (uint32_t bit = 0u; bit < 32u; bit++) {
                if ((refused >> bit) & 1u) {
                    const char *name = muhtari__bit_name(MUHTARI__BITS_SOUND, bit);
                    LR_LOG(RETRO_LOG_ERROR, "muhtari: unavailable sound configuration: %s\n", name);
                    if (!named) {
                        char text[128];
                        (void)snprintf(text, sizeof text, "Unavailable sound configuration: %s",
                                       name);
                        lr_message(text);
                    }
                    named = true;
                }
            }
            if (!named)
                lr_message("Unsupported device");
        }
        return false;
    }
    battery.size = (uint32_t)sizeof battery;
    (void)muhtari_get_battery(inst, &battery);
    if ((battery.firmware_flags & MUHTARI_HSC_FIRMWARE_STUB) != 0u) {
        LR_LOG(RETRO_LOG_WARN,
               "muhtari: highscre.bin absent; signature-only HSC stub, no firmware routines\n");
        lr_message("HSC signature-only stub: no firmware routines");
    } else if (battery.hsc_attached != 0u) {
        char hex[41];
        static const char digits[] = "0123456789abcdef";
        for (unsigned i = 0u; i < 20u; i++) {
            hex[i * 2u] = digits[battery.hsc_sha1[i] >> 4];
            hex[i * 2u + 1u] = digits[battery.hsc_sha1[i] & 15u];
        }
        hex[40] = '\0';
        LR_LOG(RETRO_LOG_INFO, "muhtari: highscre.bin SHA-1 %s\n", hex);
        if ((battery.firmware_flags &
             (MUHTARI_HSC_FIRMWARE_WARN_UNKNOWN | MUHTARI_HSC_FIRMWARE_WARN_SIGNATURE)) != 0u) {
            LR_LOG(RETRO_LOG_WARN, "muhtari: HSC firmware unknown identity%s\n",
                   (battery.firmware_flags & MUHTARI_HSC_FIRMWARE_WARN_SIGNATURE) != 0u
                       ? "; missing signatures"
                       : "");
            lr_message((battery.firmware_flags & MUHTARI_HSC_FIRMWARE_WARN_SIGNATURE) != 0u
                           ? "HSC firmware unknown identity; missing signatures"
                           : "HSC firmware unknown identity");
        }
    }
    selected.size = (uint32_t)sizeof selected;
    (void)muhtari_get_info(inst, &selected);
    lr_check_battery_request(&media);
    LR_LOG(RETRO_LOG_INFO,
           "muhtari: requested devices 0x%x, sound 0x%x; effective expansion %s, HSC %u\n",
           cart.devices, cart.sound,
           selected.expansion == MUHTARI_EXPANSION_XM       ? "xm"
           : selected.expansion == MUHTARI_EXPANSION_XBOARD ? "xboard"
                                                            : "off",
           battery.hsc_attached);
    if (selected.expansion == MUHTARI_EXPANSION_XM)
        LR_LOG(RETRO_LOG_INFO, "muhtari: XM RAM/POKEY/HSC subset; no full-XM services\n");
    for (uint32_t bit = 0u; bit < 32u; bit++)
        if ((cart.warnings >> bit) & 1u)
            LR_LOG(RETRO_LOG_WARN, "muhtari: cartridge warning: %s\n",
                   muhtari__bit_name(MUHTARI__BITS_WARNINGS, bit));
    uint32_t effective = media.region != 0u ? media.region : cart.region;
    selected_region = effective == MUHTARI_REGION_PAL ? RETRO_REGION_PAL : RETRO_REGION_NTSC;
    return true;
}

/* Adds the CPU addresses [start, end) to the map, backed by ptr + offset,
   or by nothing when ptr is NULL. A descriptor whose select is 0 must have
   a power-of-two length (L3634), so the span goes in aligned power-of-two
   pieces. */
static void lr_map(uint32_t start, uint32_t end, uint8_t *ptr, size_t offset, uint64_t flags)
{
    while (start < end && desc_count < LR_MAX_DESCRIPTORS) {
        struct retro_memory_descriptor *d = &descs[desc_count++];
        uint32_t len = start != 0u ? start & (0u - start) : 0x10000u;
        while (len > end - start) {
            len >>= 1;
        }
        memset(d, 0, sizeof *d);
        d->flags = ptr != NULL ? flags : 0u; /* L3582: no flags without memory */
        d->ptr = ptr;
        d->offset = ptr != NULL ? offset : 0u;
        d->start = start;
        d->len = len;
        offset += len;
        start += len;
    }
}

/* Maps [start, end), in increasing address order, after a pointer-less
   descriptor for any addresses since the last span. rcheevos fills the
   rest of a console region with nothing at the first address no
   descriptor covers (rc_libretro.c, rc_libretro_memory_init_from_memory_map),
   so a hole before $0040 or $C000 would hide what follows it. */
static void lr_span(uint32_t start, uint32_t end, uint8_t *ptr, size_t offset, uint64_t flags)
{
    lr_map(desc_next, start, NULL, 0u, 0u);
    lr_map(start, end, ptr, offset, flags);
    desc_next = end;
}

/* The span of memory kind, or NULL. */
static uint8_t *lr_memory(uint32_t kind, size_t *bytes)
{
    muhtari_span span;
    memset(&span, 0, sizeof span);
    span.size = (uint32_t)sizeof span;
    if (muhtari_get_memory(inst, kind, &span) != MUHTARI_STATUS_OK) {
        return NULL;
    }
    *bytes = span.bytes;
    return span.data;
}

/* The achievement map (HOSTS-AND-INTEGRATION section 2): the CPU bus as
   rcheevos' console 51 reads it. Page 0's and page 1's RAM shadow $2040-
   $20FF and $2140-$21FF; $2800-$2FFF repeats RAM's second half while the
   bus there stays undecoded; registers, absent HSC and banked windows have
   no memory behind them. Only session-stable CPU windows are published;
   expansion overlays and XM HSC gates remain holes (docs/libretro.md). */
static void lr_memory_maps(void)
{
    bool yes = true;
    size_t ram_bytes = 0u, riot_bytes = 0u, cart_bytes = 0u;
    uint8_t *ram = lr_memory(MUHTARI_MEMORY_SYSTEM_RAM, &ram_bytes);
    uint8_t *riot = lr_memory(MUHTARI_MEMORY_RIOT_RAM, &riot_bytes);
    uint8_t *cart_ram = lr_memory(MUHTARI_MEMORY_CART_RAM, &cart_bytes);
    uint8_t *rom = content + ((cart.flags & MUHTARI_CART_HEADER) != 0u ? 128u : 0u);
    bool overlay = selected.expansion != MUHTARI_EXPANSION_OFF;
    bool xm = selected.expansion == MUHTARI_EXPANSION_XM;
    uint32_t rom_bytes = cart.board == MUHTARI_BOARD_BANKSET ? cart.rom_size / 2u : cart.rom_size;
    uint32_t rom_from = cart_ram != NULL || overlay ? 0x8000u : 0x4000u;
    muhtari_const_span hsc_rom = {0};
    hsc_rom.size = (uint32_t)sizeof hsc_rom;
    (void)muhtari_get_hsc_rom(inst, &hsc_rom);

    desc_count = 0u;
    desc_next = 0u;
    lr_span(0x0040u, 0x0100u, ram, 0x0840u, 0u);
    lr_span(0x0140u, 0x0200u, ram, 0x0940u, 0u);
    lr_span(0x0480u, 0x0500u, riot, 0u, 0u);
    if (battery.hsc_attached != 0u && !xm)
        lr_span(0x1000u, 0x1800u, battery.data, 0u, 0u);
    lr_span(0x1800u, 0x2800u, ram, 0u, RETRO_MEMDESC_SYSTEM_RAM);
    lr_span(0x2800u, 0x3000u, ram, 0x0800u, 0u);
    if (hsc_rom.data != NULL && !xm)
        lr_span(0x3000u, 0x4000u, (uint8_t *)(uintptr_t)hsc_rom.data, 0u, RETRO_MEMDESC_CONST);
    if (cart.board == MUHTARI_BOARD_BANKSET && rom_bytes == 53248u && !xm)
        lr_span(0x3000u, 0x4000u, rom, 0u, RETRO_MEMDESC_CONST);
    if (!overlay && cart.board == MUHTARI_BOARD_FRACTALUS && cart_ram != NULL) {
        /* Sixteen 256-byte aliases expose the eight physical pages twice.
           Higher bus mirrors remain descriptor holes to fit the pinned
           resolver (cartridge preparation 2.1; docs/libretro.md). */
        for (uint32_t page = 0u; page < 16u; page++) {
            uint32_t start = 0x4000u + page * 0x100u;
            lr_span(start, start + 0x100u, cart_ram, (size_t)(page / 2u) * 0x100u, 0u);
        }
    } else if (!overlay && cart_ram != NULL &&
               (cart.board_options & (MUHTARI_BOARD_BANKED_RAM | MUHTARI_BOARD_HALT_BANKED_RAM)) ==
                   0u) {
        lr_span(0x4000u, 0x4000u + (uint32_t)(cart_bytes < 0x4000u ? cart_bytes : 0x4000u),
                cart_ram, 0u, 0u);
    }
    if (cart.board == MUHTARI_BOARD_LINEAR ||
        (cart.board == MUHTARI_BOARD_BANKSET && rom_bytes <= 53248u)) {
        uint32_t base = 0x10000u - rom_bytes;
        uint32_t from = base > rom_from ? base : rom_from;
        lr_span(from, 0x10000u, rom, from - base, RETRO_MEMDESC_CONST);
    } else if (cart.board == MUHTARI_BOARD_ACTIVISION) {
        static const uint32_t windows[4] = {0x4000u, 0x6000u, 0x8000u, 0xE000u};
        static const uint32_t offsets[4] = {0x18000u, 0x1A000u, 0x1C000u, 0x1E000u};
        /* Descriptors retain original host content, so translate each OM
           half back to the identified input order (cartridge notes 1.4). */
        uint32_t half = (cart.flags & MUHTARI_CART_ACTIVISION_HALF_SWAPPED) != 0u ? 0x2000u : 0u;
        for (uint32_t window = 0u; window < 4u; window++) {
            if (overlay && windows[window] < 0x8000u)
                continue;
            lr_span(windows[window], windows[window] + 0x2000u, rom, offsets[window] ^ half,
                    RETRO_MEMDESC_CONST);
        }
    } else if (cart.board == MUHTARI_BOARD_FRACTALUS) {
        lr_span(0x8000u, 0x10000u, rom, 0u, RETRO_MEMDESC_CONST);
    } else if (cart.board == MUHTARI_BOARD_ABSOLUTE) {
        /* Original-content offsets equal CPU addresses in the fixed upper
           32 KiB (docs/hardware/cartridge.md, Absolute). */
        lr_span(0x8000u, 0x10000u, rom, 0x8000u, RETRO_MEMDESC_CONST);
    } else if (cart.board == MUHTARI_BOARD_SUPERGAME || cart.board == MUHTARI_BOARD_BANKSET) {
        /* The fixed windows of A7800-CARTRIDGES-AND-PERIPHERALS section 2.1:
           the last bank at $C000, and at $4000 bank 0 on the 2^n + 1 board,
           bank 6 with that option, else the second-to-last. */
        uint32_t banks = rom_bytes / 0x4000u;
        if (rom_from == 0x4000u) {
            uint32_t bank = banks - 2u;
            if ((cart.board_options & MUHTARI_BOARD_ROM_AT_4000) != 0u) {
                bank = 0u;
            } else if ((cart.board_options & MUHTARI_BOARD_BANK6_AT_4000) != 0u) {
                bank = 6u % banks;
            }
            lr_span(0x4000u, 0x8000u, rom, (size_t)bank * 0x4000u, RETRO_MEMDESC_CONST);
        }
        lr_span(0xC000u, 0x10000u, rom, (size_t)(banks - 1u) * 0x4000u, RETRO_MEMDESC_CONST);
    }
    lr_span(0x10000u, 0x10000u, NULL, 0u, 0u);
    memory_map.descriptors = descs;
    memory_map.num_descriptors = desc_count;
    (void)env_cb(RETRO_ENVIRONMENT_SET_MEMORY_MAPS, &memory_map);
    (void)env_cb(RETRO_ENVIRONMENT_SET_SUPPORT_ACHIEVEMENTS, &yes);
}

/* L7761. A NULL game starts the test card; content is copied and loaded.
   XRGB8888, or RGB565 in the USE_RGB565 build, is chosen here (L861) once
   the content is accepted; the default 0RGB1555 is deprecated
   (L5620-5647). The input descriptors, the difficulty options and, with
   content, the memory map follow. */
bool retro_load_game(const struct retro_game_info *game)
{
    enum retro_pixel_format format = LR_PIXEL_FORMAT;
    struct retro_log_callback log;
    muhtari_config cfg;

    if (env_cb == NULL) {
        return false;
    }
    memset(&log, 0, sizeof log);
    log_cb = env_cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &log) ? log.log : NULL;
    lr_release();
    memset(&cfg, 0, sizeof cfg);
    cfg.size = (uint32_t)sizeof cfg;
    cfg.allocator.alloc = lr_alloc;
    cfg.allocator.free = lr_free;
    native = (uint8_t *)malloc(LR_PIXELS);
    pixels = (lr_pixel *)malloc(LR_PIXELS * sizeof *pixels);
    if (native == NULL || pixels == NULL || muhtari_create(&cfg, &inst) != MUHTARI_STATUS_OK) {
        inst = NULL;
        lr_release();
        return false;
    }
    memset(pixels, 0, LR_PIXELS * sizeof *pixels);
#ifdef MUHTARI_LIBRETRO_TEST
    /* Test-only raw observation follows the existing setup-before-load
       contract, so no observer allocation occurs during emulation. */
    if (lr_test_observer != NULL &&
        muhtari__synth_observe(inst, lr_test_observer, lr_test_observer_user) !=
            MUHTARI_STATUS_OK) {
        lr_release();
        return false;
    }
#endif
    if ((game != NULL && !lr_load_content(game)) ||
        !env_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &format)) {
        lr_release();
        return false;
    }
    selected.size = (uint32_t)sizeof selected;
    if (muhtari_get_info(inst, &selected) != MUHTARI_STATUS_OK) {
        lr_release();
        return false;
    }
    lr_read_palette();
    lr_refresh_inputs();
    muhtari_input_ex initial = {0};
    muhtari_media request = {0};
    initial.size = (uint32_t)sizeof initial;
    lr_battery_request(&request);
    for (unsigned p = 0u; p < 2u; p++) {
        initial.port[p].device = lr_active_port(p, &request);
        if (initial.port[p].device == MUHTARI_PORT_LIGHTGUN)
            initial.port[p].flags = MUHTARI_INPUT_GUN_OFFSCREEN;
    }
    if (muhtari_set_input_ex(inst, &initial) != MUHTARI_STATUS_OK) {
        lr_release();
        return false;
    }
    console_changed = false;
    lr_read_difficulty(true);
    if (content != NULL) {
        lr_memory_maps();
    }
    return true;
}

bool retro_load_game_special(unsigned game_type, const struct retro_game_info *info,
                             size_t num_info)
{
    (void)game_type;
    (void)info;
    (void)num_info;
    return false;
}

/* L7790: the instance and its buffers go with the game. */
void retro_unload_game(void)
{
    lr_release();
}

#ifdef MUHTARI_LIBRETRO_TEST
/* Test-only native buffer, independent of palette conversion. The frame core
   clears maria.rows before the frontend callback, so the instance alone cannot
   expose these bytes. This symbol is absent from installed modules. */
RETRO_API const uint8_t *muhtari__libretro_native(void);
RETRO_API const uint8_t *muhtari__libretro_native(void)
{
    return native;
}
/* The adapter's instance, for tests/libretro/rcheevos.c, which compiles this
   file into itself to compare the memory map with the core's own view of
   the bus. Never in the shipped module. */
RETRO_API muhtari *muhtari__libretro_instance(void);
RETRO_API muhtari *muhtari__libretro_instance(void)
{
    return inst;
}
#endif

/* Region follows the successful load; release restores no-content NTSC. */
unsigned retro_get_region(void)
{
    return selected_region;
}

/* L7826: system RAM, the 4 KiB at $1800-$27FF, from the instance. */
void *retro_get_memory_data(unsigned id)
{
    size_t bytes = 0u;
    if (inst != NULL && id == RETRO_MEMORY_SAVE_RAM)
        return battery.data;
    if (inst == NULL || id != RETRO_MEMORY_SYSTEM_RAM) {
        return NULL;
    }
    return lr_memory(MUHTARI_MEMORY_SYSTEM_RAM, &bytes);
}

size_t retro_get_memory_size(unsigned id)
{
    size_t bytes = 0u;
    if (inst != NULL && id == RETRO_MEMORY_SAVE_RAM)
        return battery.bytes;
    if (inst == NULL || id != RETRO_MEMORY_SYSTEM_RAM) {
        return 0u;
    }
    return lr_memory(MUHTARI_MEMORY_SYSTEM_RAM, &bytes) != NULL ? bytes : 0u;
}
