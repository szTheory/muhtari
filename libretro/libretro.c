/* The libretro adapter: the 25 entry points of libretro.h, written as a thin
 * client of muhtari.h. "Lnnn" is a line of the vendored libretro.h
 * (RetroArch v1.22.2, see PROVENANCE.md).
 *
 * retro_load_game(NULL) creates the instance, which shows the built-in test
 * card; content is loaded through muhtari_load and its fast boot. The
 * adapter copies the content, and the copy lives until the game unloads. */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "libretro.h"
#include "muhtari.h"
#include "muhtari_version.h"
#include "palette.h"
#include "status_text.h"

#define LR_WIDTH 320u
#define LR_HEIGHT 242u
#define LR_MAX_HEIGHT 293u
#define LR_PIXELS (LR_WIDTH * LR_HEIGHT)
/* A frame yields 800 or 801 samples (muhtari_info.max_audio_samples). */
#define LR_AUDIO_CAPACITY 801u

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
static uint8_t *content;
static size_t content_size;
static uint8_t *native;
static lr_pixel *pixels;
static int16_t mono[LR_AUDIO_CAPACITY];
static int16_t stereo[LR_AUDIO_CAPACITY * 2u];

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

/* Destroys the instance, the content copy and the frame buffers. */
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
}

/* L7511: called before retro_init. Declaring no-game support (18, L1051)
   makes the frontend call retro_load_game(NULL) (L1040-1051). */
void retro_set_environment(retro_environment_t cb)
{
    bool no_game = true;
    env_cb = cb;
    if (cb != NULL) {
        (void)cb(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &no_game);
    }
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

void retro_set_controller_port_device(unsigned port, unsigned device)
{
    (void)port;
    (void)device;
}

/* The test card has no state to reset. */
void retro_reset(void)
{
}

/* Whether RetroPad button id is held on port. */
static bool lr_held(unsigned port, unsigned id)
{
    return input_state_cb != NULL && input_state_cb(port, RETRO_DEVICE_JOYPAD, 0, id) != 0;
}

/* The 7800 buttons held on a port: the D-pad is the stick, B button 1 and
   A button 2 (HOSTS-AND-INTEGRATION section 1, Input). */
static uint32_t lr_buttons(unsigned port)
{
    uint32_t b = 0u;
    b |= lr_held(port, RETRO_DEVICE_ID_JOYPAD_UP) ? MUHTARI_BUTTON_UP : 0u;
    b |= lr_held(port, RETRO_DEVICE_ID_JOYPAD_DOWN) ? MUHTARI_BUTTON_DOWN : 0u;
    b |= lr_held(port, RETRO_DEVICE_ID_JOYPAD_LEFT) ? MUHTARI_BUTTON_LEFT : 0u;
    b |= lr_held(port, RETRO_DEVICE_ID_JOYPAD_RIGHT) ? MUHTARI_BUTTON_RIGHT : 0u;
    b |= lr_held(port, RETRO_DEVICE_ID_JOYPAD_B) ? MUHTARI_BUTTON_1 : 0u;
    b |= lr_held(port, RETRO_DEVICE_ID_JOYPAD_A) ? MUHTARI_BUTTON_2 : 0u;
    return b;
}

/* Hands the RetroPads' state to the instance; the core applies it at the
   next VBLANK start (docs/embedding.md, Input). */
static void lr_input(void)
{
    muhtari_input in;
    memset(&in, 0, sizeof in);
    in.size = (uint32_t)sizeof in;
    in.buttons[0] = lr_buttons(0u);
    in.buttons[1] = lr_buttons(1u);
    (void)muhtari_set_input(inst, &in);
}

/* L7694: one frame. Input is polled once (L7685), the video callback is
   called once with the converted rows, and mono samples are duplicated into
   left/right pairs. Prefer the batch callback (L7465); use the per-sample
   callback only when no batch callback is registered (L7453). A failed frame
   sends no video or audio. */
void retro_run(void)
{
    muhtari_frame io;
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
    if (muhtari_run_frame(inst, &io) != MUHTARI_STATUS_OK) {
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

/* Writes a message to the frontend's log, when it has one (L1243). */
#define LR_LOG(level, ...)                                                                         \
    do {                                                                                           \
        if (log_cb != NULL) {                                                                      \
            log_cb(level, __VA_ARGS__);                                                            \
        }                                                                                          \
    } while (0)

/* Loads the content copy into the instance. Logs a refusal with its status
   text and returns false. */
static bool lr_load_content(const struct retro_game_info *game)
{
    muhtari_cart_info info;
    muhtari_media media;
    muhtari_status st;

    if (game->data == NULL || game->size == 0u || game->size > MUHTARI_IMAGE_SIZE_MAX) {
        LR_LOG(RETRO_LOG_ERROR, "muhtari: no content bytes, or more than %u\n",
               (unsigned)MUHTARI_IMAGE_SIZE_MAX);
        return false;
    }
    memset(&info, 0, sizeof info);
    info.size = (uint32_t)sizeof info;
    st = muhtari_probe(game->data, game->size, &info);
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
        st = muhtari_load(inst, &media);
    }
    if (st != MUHTARI_STATUS_OK) {
        LR_LOG(RETRO_LOG_ERROR, "muhtari: content refused: %s%s%s\n", muhtari__status_text(st),
               st == MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD ? ": " : "",
               st == MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD ? muhtari__board_name(info.board) : "");
        return false;
    }
    return true;
}

/* L7761. A NULL game starts the test card; content is copied and loaded.
   XRGB8888, or RGB565 in the USE_RGB565 build, is chosen here (L861) once
   the content is accepted; the default 0RGB1555 is deprecated
   (L5620-5647). */
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

/* L7812: 0 is NTSC. */
unsigned retro_get_region(void)
{
    return RETRO_REGION_NTSC;
}

/* L7826: system RAM, the 4 KiB at $1800-$27FF, from the instance. */
void *retro_get_memory_data(unsigned id)
{
    muhtari_span span;
    if (inst == NULL || id != RETRO_MEMORY_SYSTEM_RAM) {
        return NULL;
    }
    memset(&span, 0, sizeof span);
    span.size = (uint32_t)sizeof span;
    return muhtari_get_memory(inst, MUHTARI_MEMORY_SYSTEM_RAM, &span) == MUHTARI_STATUS_OK
               ? span.data
               : NULL;
}

size_t retro_get_memory_size(unsigned id)
{
    muhtari_span span;
    if (inst == NULL || id != RETRO_MEMORY_SYSTEM_RAM) {
        return 0u;
    }
    memset(&span, 0, sizeof span);
    span.size = (uint32_t)sizeof span;
    return muhtari_get_memory(inst, MUHTARI_MEMORY_SYSTEM_RAM, &span) == MUHTARI_STATUS_OK
               ? span.bytes
               : 0u;
}
