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
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libretro.h"
#include "muhtari.h"
#include "muhtari_version.h"
#include "palette.h"
#include "sha256.h"
#include "status_text.h"

#define LR_WIDTH 320u
#define LR_HEIGHT 242u
#define LR_MAX_HEIGHT 293u
#define LR_PIXELS (LR_WIDTH * LR_HEIGHT)
/* A frame yields 800 or 801 samples (muhtari_info.max_audio_samples). */
#define LR_AUDIO_CAPACITY 801u
/* The larger BIOS image, 16 KiB; one byte more is read so that a longer
   file reaches muhtari_load, which refuses it. */
#define LR_BIOS_MAX 16384u
/* A 64 KiB CPU bus in aligned power-of-two pieces needs at most two
   descriptors per address bit for each of the map's spans. */
#define LR_MAX_DESCRIPTORS 64u
#define LR_JOYSTICK RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_JOYPAD, 1)
#define LR_PROLINE RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_JOYPAD, 0)

#ifdef USE_RGB565
typedef uint16_t lr_pixel;
#define LR_PIXEL_FORMAT RETRO_PIXEL_FORMAT_RGB565
#define LR_PALETTE muhtari__palette_rgb565
#else
typedef uint32_t lr_pixel;
#define LR_PIXEL_FORMAT RETRO_PIXEL_FORMAT_XRGB8888
#define LR_PALETTE muhtari__palette_rgb
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
static bool run_failed;        /* a failed frame was logged */
static struct retro_memory_descriptor descs[LR_MAX_DESCRIPTORS];
static struct retro_memory_map memory_map; /* outlives SET_MEMORY_MAPS */
static unsigned desc_count;
static uint32_t desc_next; /* the first CPU address no descriptor covers yet */

/* Writes a message to the frontend's log, when it has one (L1243). */
#define LR_LOG(level, ...)                                                                         \
    do {                                                                                           \
        if (log_cb != NULL) {                                                                      \
            log_cb(level, __VA_ARGS__);                                                            \
        }                                                                                          \
    } while (0)

/* ---- the frontend's view: devices, inputs and options ---- */

/* SET_CONTROLLER_INFO (L1506): the same four choices on both ports. Auto
   follows the cartridge header's controller byte. */
static const struct retro_controller_description pad_types[] = {
    {"Auto (cartridge header)", RETRO_DEVICE_JOYPAD},
    {"ProLine", LR_PROLINE},
    {"Joystick", LR_JOYSTICK},
    {"None", RETRO_DEVICE_NONE},
};
static struct retro_controller_info ports[] = {
    {pad_types, 4u},
    {pad_types, 4u},
    {NULL, 0u},
};

/* SET_INPUT_DESCRIPTORS (L882): ProSystem's layout (HOSTS-AND-INTEGRATION
   section 1, Input). */
static struct retro_input_descriptor inputs[] = {
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

/* The options (SET_CORE_OPTIONS_V2, L2339). No region option: PAL is
   refused in this release. */
static struct retro_core_option_v2_category option_categories[] = {{NULL, NULL, NULL}};
static struct retro_core_option_v2_definition options_v2[] = {
    {.key = "muhtari_boot",
     .desc = "Boot",
     .info = "Fast starts the cartridge at once. BIOS boots through 7800 BIOS (U).rom or 7800 "
             "BIOS (E).rom in the system directory, or fast when neither is there. Applies when "
             "content is loaded.",
     .values = {{"fast", "Fast"}, {"bios", "BIOS"}, {NULL, NULL}},
     .default_value = "fast"},
    {.key = "muhtari_palette",
     .desc = "Palette",
     .info = "The colours of the native frame.",
     .values = {{"ntsc", "NTSC (warm)"}, {NULL, NULL}},
     .default_value = "ntsc"},
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
    memset(descs, 0, sizeof descs);
    memset(&memory_map, 0, sizeof memory_map);
    desc_count = 0u;
    desc_next = 0u;
    run_failed = false;
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
}

/* L7673: one of the four ids of pad_types; any other id means Auto. */
void retro_set_controller_port_device(unsigned port, unsigned id)
{
    if (port < 2u) {
        device[port] = id;
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
    default:
        return 0u;
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
static uint32_t lr_buttons(unsigned port)
{
    uint32_t d = lr_port_device(port);
    bool joystick;
    uint32_t b = 0u;
    if (d == 0u) {
        d = port == 0u ? cart.port1 : cart.port2;
    }
    joystick = d == MUHTARI_PORT_JOYSTICK;
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

/* Port 1's L and R flip the left and right difficulty switches on each
   press, with an on-screen message. */
static void lr_difficulty_edges(void)
{
    static const unsigned ids[2] = {RETRO_DEVICE_ID_JOYPAD_L, RETRO_DEVICE_ID_JOYPAD_R};
    static const char *const text[2][2] = {{"Left difficulty: B", "Left difficulty: A"},
                                           {"Right difficulty: B", "Right difficulty: A"}};
    unsigned i;
    for (i = 0; i < 2u; i++) {
        bool held = lr_held(0u, ids[i]);
        if (held && !edge_held[i]) {
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
    muhtari_input in;
    bool updated = false;
    if (env_cb != NULL && env_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated) {
        lr_read_difficulty(false);
    }
    lr_difficulty_edges();
    memset(&in, 0, sizeof in);
    in.size = (uint32_t)sizeof in;
    in.device[0] = lr_port_device(0u);
    in.device[1] = lr_port_device(1u);
    in.buttons[0] = lr_buttons(0u);
    in.buttons[1] = lr_buttons(1u);
    in.switches |= lr_held(0u, RETRO_DEVICE_ID_JOYPAD_SELECT) ? MUHTARI_SWITCH_SELECT : 0u;
    in.switches |= lr_held(0u, RETRO_DEVICE_ID_JOYPAD_START) ? MUHTARI_SWITCH_PAUSE : 0u;
    in.switches |= lr_held(0u, RETRO_DEVICE_ID_JOYPAD_X) ? MUHTARI_SWITCH_RESET : 0u;
    in.switches |= difficulty_a[0] ? MUHTARI_SWITCH_LEFT_DIFFICULTY_A : 0u;
    in.switches |= difficulty_a[1] ? MUHTARI_SWITCH_RIGHT_DIFFICULTY_A : 0u;
    (void)muhtari_set_input(inst, &in);
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
   duplicates the last picture without audio; its first failure is reported. */
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
    io.video_pitch = LR_WIDTH;
    io.audio = mono;
    io.audio_capacity = LR_AUDIO_CAPACITY;
    st = muhtari_run_frame(inst, &io);
    if (st != MUHTARI_STATUS_OK) {
        if (!run_failed) {
            LR_LOG(RETRO_LOG_ERROR, "muhtari: frame failed: %s\n", muhtari__status_text(st));
            lr_message(muhtari__status_text(st));
            run_failed = true;
        }
        if (video_cb != NULL) {
            video_cb(NULL, LR_WIDTH, LR_HEIGHT, (size_t)LR_WIDTH * sizeof pixels[0]);
        }
        return;
    }
    for (i = 0; i < LR_PIXELS; i++) {
        pixels[i] = LR_PALETTE(native[i]);
    }
    if (video_cb != NULL) {
        video_cb(pixels, LR_WIDTH, LR_HEIGHT, (size_t)LR_WIDTH * sizeof pixels[0]);
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

/* Loads the content copy into the instance, through a BIOS file when the
   boot option asks for one and there is one. Logs the BIOS file and its
   SHA-256, or the fast-boot fallback, or a refusal with its status text,
   and returns false on a refusal. */
static bool lr_load_content(const struct retro_game_info *game)
{
    muhtari_media media;
    muhtari_status st;

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
        if (lr_option_is("muhtari_boot", "bios")) {
            st = lr_load_bios(&media);
        } else {
            st = muhtari_load(inst, &media);
        }
    }
    if (st != MUHTARI_STATUS_OK) {
        LR_LOG(RETRO_LOG_ERROR, "muhtari: content refused: %s%s%s\n", muhtari__status_text(st),
               st == MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD ? ": " : "",
               st == MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD ? muhtari__board_name(cart.board) : "");
        return false;
    }
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
   bus there stays undecoded; registers, $3000-$3FFF and banked windows have
   no memory behind them. Unbanked cartridge RAM sits at $4000 and the fixed
   ROM is CONST. */
static void lr_memory_maps(void)
{
    bool yes = true;
    size_t ram_bytes = 0u, riot_bytes = 0u, cart_bytes = 0u;
    uint8_t *ram = lr_memory(MUHTARI_MEMORY_SYSTEM_RAM, &ram_bytes);
    uint8_t *riot = lr_memory(MUHTARI_MEMORY_RIOT_RAM, &riot_bytes);
    uint8_t *cart_ram = lr_memory(MUHTARI_MEMORY_CART_RAM, &cart_bytes);
    uint8_t *rom = content + ((cart.flags & MUHTARI_CART_HEADER) != 0u ? 128u : 0u);
    uint32_t rom_from = cart_ram != NULL ? 0x8000u : 0x4000u;

    desc_count = 0u;
    desc_next = 0u;
    lr_span(0x0040u, 0x0100u, ram, 0x0840u, 0u);
    lr_span(0x0140u, 0x0200u, ram, 0x0940u, 0u);
    lr_span(0x0480u, 0x0500u, riot, 0u, 0u);
    lr_span(0x1800u, 0x2800u, ram, 0u, RETRO_MEMDESC_SYSTEM_RAM);
    lr_span(0x2800u, 0x3000u, ram, 0x0800u, 0u);
    if (cart_ram != NULL && (cart.board_options & MUHTARI_BOARD_BANKED_RAM) == 0u) {
        lr_span(0x4000u, 0x4000u + (uint32_t)(cart_bytes < 0x4000u ? cart_bytes : 0x4000u),
                cart_ram, 0u, 0u);
    }
    if (cart.board == MUHTARI_BOARD_LINEAR) {
        uint32_t base = 0x10000u - cart.rom_size;
        uint32_t from = base > rom_from ? base : rom_from;
        lr_span(from, 0x10000u, rom, from - base, RETRO_MEMDESC_CONST);
    } else if (cart.board == MUHTARI_BOARD_SUPERGAME) {
        /* The fixed windows of A7800-CARTRIDGES-AND-PERIPHERALS section 2.1:
           the last bank at $C000, and at $4000 bank 0 on the 2^n + 1 board,
           bank 6 with that option, else the second-to-last. */
        uint32_t banks = cart.rom_size / 0x4000u;
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
    if ((game != NULL && !lr_load_content(game)) ||
        !env_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &format)) {
        lr_release();
        return false;
    }
    (void)env_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, inputs);
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
/* The adapter's instance, for tests/libretro/rcheevos.c, which compiles this
   file into itself to compare the memory map with the core's own view of
   the bus. Never in the shipped module. */
muhtari *muhtari__libretro_instance(void);
muhtari *muhtari__libretro_instance(void)
{
    return inst;
}
#endif

/* L7812: 0 is NTSC, the only region this release runs. */
unsigned retro_get_region(void)
{
    return RETRO_REGION_NTSC;
}

/* L7826: system RAM, the 4 KiB at $1800-$27FF, from the instance. */
void *retro_get_memory_data(unsigned id)
{
    size_t bytes = 0u;
    if (inst == NULL || id != RETRO_MEMORY_SYSTEM_RAM) {
        return NULL;
    }
    return lr_memory(MUHTARI_MEMORY_SYSTEM_RAM, &bytes);
}

size_t retro_get_memory_size(unsigned id)
{
    size_t bytes = 0u;
    if (inst == NULL || id != RETRO_MEMORY_SYSTEM_RAM) {
        return 0u;
    }
    return lr_memory(MUHTARI_MEMORY_SYSTEM_RAM, &bytes) != NULL ? bytes : 0u;
}
