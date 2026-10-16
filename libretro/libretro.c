/* The libretro adapter: the 25 entry points of libretro.h, written as a thin
 * client of muhtari.h. "Lnnn" is a line of the vendored libretro.h
 * (RetroArch v1.22.2, see PROVENANCE.md).
 *
 * This release loads no content: retro_load_game(NULL) creates the instance,
 * which shows the built-in test card, and any content is refused. */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "libretro.h"
#include "muhtari.h"
#include "muhtari_version.h"
#include "palette.h"

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
static muhtari *inst;
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

/* Destroys the instance and its frame buffers. */
static void lr_release(void)
{
    muhtari_destroy(inst);
    inst = NULL;
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
    memset(mono, 0, sizeof mono);
    memset(stereo, 0, sizeof stereo);
}

unsigned retro_api_version(void)
{
    return RETRO_API_VERSION;
}

/* L7627: static strings. No content is loaded yet, so the extension list is
   empty. */
void retro_get_system_info(struct retro_system_info *info)
{
    if (info == NULL) {
        return;
    }
    memset(info, 0, sizeof *info);
    info->library_name = "muhtari";
    info->library_version = MUHTARI_VERSION_STRING;
    info->valid_extensions = "";
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

/* L7761. A NULL game starts the test card; content is refused. XRGB8888, or
   RGB565 in the USE_RGB565 build, is chosen here (L861); the default
   0RGB1555 is deprecated (L5620-5647). */
bool retro_load_game(const struct retro_game_info *game)
{
    enum retro_pixel_format format = LR_PIXEL_FORMAT;
    muhtari_config cfg;

    if (game != NULL) {
        return false;
    }
    if (env_cb == NULL || !env_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &format)) {
        return false;
    }
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

/* L7826: no memory regions are exposed yet. */
void *retro_get_memory_data(unsigned id)
{
    (void)id;
    return NULL;
}

size_t retro_get_memory_size(unsigned id)
{
    (void)id;
    return 0u;
}
