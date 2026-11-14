/* Independent public-library and production-adapter observations for WASM. */
#if !defined(_WIN32)
#define _POSIX_C_SOURCE 200809L
#endif
#include <stdarg.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <time.h>
#endif

#include "libretro.h"
#include "muhtari.h"
#include "palette.h"
#include "sha256.h"

#ifndef MUHTARI_CORE_SOURCE_ID
#define MUHTARI_CORE_SOURCE_ID "missing"
#endif
#ifndef MUHTARI_NATIVE_COMPILER_ID
#define MUHTARI_NATIVE_COMPILER_ID "unknown"
#endif
#ifndef MUHTARI_NATIVE_COMPILER_VERSION
#define MUHTARI_NATIVE_COMPILER_VERSION "unknown"
#endif

enum {
    VIDEO_WIDTH = 320u,
    VIDEO_HEIGHT = 242u,
    VIDEO_BYTES = VIDEO_WIDTH * 292u * 4u,
    AUDIO_CAPACITY = 1024u,
    LAST_ENTRY = 0x0841u,
    ENTRY_BYTES = 8u,
    SCENARIO_COUNT = 5u,
    MAX_SCENARIO_FRAMES = 3u
};

enum { INPUT_SWCHA, INPUT_SWCHB };

typedef struct callback_frame {
    uint8_t video[VIDEO_BYTES];
    int16_t audio[AUDIO_CAPACITY * 2u];
    unsigned video_calls, batch_calls, sample_calls, poll_calls;
    unsigned width, height;
    size_t pitch;
    size_t audio_frames;
} callback_frame;

typedef struct numeric_observation {
    unsigned video_calls, audio_calls, poll_calls;
    unsigned width, height, audio_count;
    unsigned swcha;
} numeric_observation;

typedef struct frame_observation {
    unsigned index, swcha, audio_count;
    char video_sha256[65];
    char audio_sha256[65];
} frame_observation;

typedef struct scenario_observation {
    const char *name;
    unsigned index, frame_count, settled_swcha;
    frame_observation frames[MAX_SCENARIO_FRAMES];
} scenario_observation;

typedef struct performance_observation {
    double elapsed_seconds[3];
    double frames_per_second[3];
    double median_frames_per_second;
} performance_observation;

static callback_frame callbacks;
static uint16_t pad[2];
static const char *ym_region;

static int save_adapter_state(uint8_t **state, size_t *bytes);

#define OBS_CHECK(condition, ...)                                                                  \
    do {                                                                                           \
        if (!(condition)) {                                                                        \
            (void)fprintf(stderr, "libretro observer: ");                                          \
            (void)fprintf(stderr, __VA_ARGS__);                                                    \
            (void)fputc('\n', stderr);                                                             \
            return 0;                                                                              \
        }                                                                                          \
    } while (0)

static int fail_reason(char *reason, size_t capacity, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    (void)vsnprintf(reason, capacity, format, args);
    va_end(args);
    return 0;
}

static int replace_report(const char *temporary, const char *path)
{
#if defined(_WIN32)
    return MoveFileExA(temporary, path, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) ? 0
                                                                                            : -1;
#else
    return rename(temporary, path);
#endif
}

static int validate_numeric(const numeric_observation *observation, int expected_swcha,
                            int steady_audio, char *reason, size_t capacity)
{
    if (observation->video_calls != 1u)
        return fail_reason(reason, capacity, "missing video callback");
    if (observation->audio_calls != 1u)
        return fail_reason(reason, capacity, "missing audio batch callback");
    if (observation->poll_calls != 1u)
        return fail_reason(reason, capacity, "missing input poll callback");
    if (observation->width != VIDEO_WIDTH || observation->height != VIDEO_HEIGHT)
        return fail_reason(reason, capacity, "video geometry must be 320x242");
    if (steady_audio && observation->audio_count != 800u && observation->audio_count != 801u)
        return fail_reason(reason, capacity, "NTSC audio count must be 800 or 801, got %u",
                           observation->audio_count);
    if (expected_swcha >= 0 && observation->swcha != (unsigned)expected_swcha)
        return fail_reason(reason, capacity, "SWCHA must be %u", (unsigned)expected_swcha);
    reason[0] = '\0';
    return 1;
}

static const char *option_value(const char *key)
{
    if (ym_region != NULL && strcmp(key, "muhtari_region") == 0)
        return ym_region;
    if (strcmp(key, "muhtari_boot") == 0)
        return "fast";
    if (strcmp(key, "muhtari_palette") == 0 || strcmp(key, "muhtari_region") == 0 ||
        strcmp(key, "muhtari_hsc") == 0 || strcmp(key, "muhtari_expansion") == 0 ||
        strcmp(key, "muhtari_battery_port1") == 0 || strcmp(key, "muhtari_battery_port2") == 0)
        return "auto";
    if (strcmp(key, "muhtari_left_difficulty") == 0 || strcmp(key, "muhtari_right_difficulty") == 0)
        return "B";
    return NULL;
}

static bool RETRO_CALLCONV environment(unsigned command, void *data)
{
    switch (command) {
    case RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION:
        *(unsigned *)data = 1u;
        return true;
    case RETRO_ENVIRONMENT_SET_CORE_OPTIONS:
        return true;
    case RETRO_ENVIRONMENT_GET_VARIABLE: {
        struct retro_variable *variable = (struct retro_variable *)data;
        if (variable == NULL || variable->key == NULL)
            return false;
        variable->value = option_value(variable->key);
        return variable->value != NULL;
    }
    case RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE:
        *(bool *)data = false;
        return true;
    case RETRO_ENVIRONMENT_GET_SAVESTATE_CONTEXT:
        *(enum retro_savestate_context *)data = RETRO_SAVESTATE_CONTEXT_NORMAL;
        return true;
    case RETRO_ENVIRONMENT_SET_PIXEL_FORMAT:
        return *(enum retro_pixel_format *)data == RETRO_PIXEL_FORMAT_XRGB8888;
    case RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME:
    case RETRO_ENVIRONMENT_SET_CONTROLLER_INFO:
    case RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS:
    case RETRO_ENVIRONMENT_SET_MEMORY_MAPS:
    case RETRO_ENVIRONMENT_SET_SUPPORT_ACHIEVEMENTS:
    case RETRO_ENVIRONMENT_SET_MESSAGE:
        return true;
    default:
        return false;
    }
}

static void RETRO_CALLCONV video_refresh(const void *data, unsigned width, unsigned height,
                                         size_t pitch)
{
    callbacks.video_calls++;
    callbacks.width = width;
    callbacks.height = height;
    callbacks.pitch = pitch;
    if (data == NULL || width != VIDEO_WIDTH || (height != VIDEO_HEIGHT && height != 292u) ||
        pitch < (size_t)width * 4u) {
        return;
    }
    for (unsigned row = 0u; row < height; row++) {
        memcpy(callbacks.video + (size_t)row * width * 4u,
               (const uint8_t *)data + (size_t)row * pitch, (size_t)width * 4u);
    }
}

static void RETRO_CALLCONV audio_sample(int16_t left, int16_t right)
{
    size_t position = callbacks.audio_frames * 2u;
    callbacks.sample_calls++;
    if (position + 1u < AUDIO_CAPACITY * 2u) {
        callbacks.audio[position] = left;
        callbacks.audio[position + 1u] = right;
        callbacks.audio_frames++;
    }
}

static size_t RETRO_CALLCONV audio_batch(const int16_t *data, size_t frames)
{
    callbacks.batch_calls++;
    if (data == NULL || frames > AUDIO_CAPACITY)
        return 0u;
    memcpy(callbacks.audio, data, frames * 2u * sizeof data[0]);
    callbacks.audio_frames = frames;
    return frames;
}

static void RETRO_CALLCONV input_poll(void)
{
    callbacks.poll_calls++;
}

static int16_t RETRO_CALLCONV input_state(unsigned port, unsigned device, unsigned index,
                                          unsigned id)
{
    if (port >= 2u || device != RETRO_DEVICE_JOYPAD || index != 0u || id >= 16u)
        return 0;
    return (int16_t)((pad[port] >> id) & 1u);
}

static void *observer_alloc(void *user, size_t bytes)
{
    (void)user;
    return malloc(bytes);
}

static void observer_free(void *user, void *pointer, size_t bytes)
{
    (void)user;
    (void)bytes;
    free(pointer);
}

static int read_file(const char *path, uint8_t **data, size_t *bytes)
{
    FILE *file = fopen(path, "rb");
    long length;
    if (file == NULL || fseek(file, 0L, SEEK_END) != 0 || (length = ftell(file)) <= 0 ||
        (size_t)length > MUHTARI_IMAGE_SIZE_MAX || fseek(file, 0L, SEEK_SET) != 0) {
        if (file != NULL)
            (void)fclose(file);
        return 0;
    }
    *data = (uint8_t *)malloc((size_t)length);
    if (*data == NULL || fread(*data, 1u, (size_t)length, file) != (size_t)length) {
        free(*data);
        *data = NULL;
        (void)fclose(file);
        return 0;
    }
    (void)fclose(file);
    *bytes = (size_t)length;
    return 1;
}

static void hash_hex(const void *bytes, size_t length, char hex[65])
{
    muhtari__sha256 digest;
    uint8_t value[32];
    muhtari__sha256_init(&digest);
    muhtari__sha256_update(&digest, bytes, length);
    muhtari__sha256_final(&digest, value);
    muhtari__sha256_hex(value, hex);
}

static int pcm_matches(const int16_t *mono, const int16_t *stereo, size_t frames)
{
    for (size_t i = 0u; i < frames; i++) {
        if (mono[i] != stereo[2u * i] || mono[i] != stereo[2u * i + 1u])
            return 0;
    }
    return 1;
}

static void pcm_hash(const int16_t *samples, size_t frames, unsigned channels, char hex[65])
{
    muhtari__sha256 digest;
    uint8_t bytes[2];
    uint8_t value[32];
    muhtari__sha256_init(&digest);
    for (size_t i = 0u; i < frames * 2u; i++) {
        /* The public library is mono; libretro duplicates it into stereo. */
        uint16_t sample = (uint16_t)samples[channels == 1u ? i / 2u : i];
        bytes[0] = (uint8_t)sample;
        bytes[1] = (uint8_t)(sample >> 8);
        muhtari__sha256_update(&digest, bytes, sizeof bytes);
    }
    muhtari__sha256_final(&digest, value);
    muhtari__sha256_hex(value, hex);
}

static double monotonic_seconds(void)
{
#if defined(_WIN32)
    LARGE_INTEGER frequency, value;
    if (!QueryPerformanceFrequency(&frequency) || !QueryPerformanceCounter(&value))
        return -1.0;
    return (double)value.QuadPart / (double)frequency.QuadPart;
#else
    struct timespec value;
    if (clock_gettime(CLOCK_MONOTONIC, &value) != 0)
        return -1.0;
    return (double)value.tv_sec + (double)value.tv_nsec / 1000000000.0;
#endif
}

static int benchmark_adapter_frame(int steady_audio, char *reason, size_t reason_capacity)
{
    numeric_observation numeric = {0};
    muhtari__sha256 digest;
    uint8_t value[32], pixel_bytes[4];
    char video_sha256[65], audio_sha256[65];

    memset(&callbacks, 0, sizeof callbacks);
    pad[0] = 0u;
    pad[1] = 0u;
    retro_run();
    numeric.video_calls = callbacks.video_calls;
    numeric.audio_calls = callbacks.batch_calls;
    numeric.poll_calls = callbacks.poll_calls;
    numeric.width = callbacks.width;
    numeric.height = callbacks.height;
    numeric.audio_count = (unsigned)callbacks.audio_frames;
    if (!validate_numeric(&numeric, -1, steady_audio, reason, reason_capacity))
        return 0;
    if (callbacks.sample_calls != 0u || callbacks.audio_frames > AUDIO_CAPACITY)
        return fail_reason(reason, reason_capacity, "benchmark callback exceeded its audio bound");

    muhtari__sha256_init(&digest);
    for (size_t pixel = 0u; pixel < VIDEO_WIDTH * VIDEO_HEIGHT; pixel++) {
        uint32_t xrgb;
        memcpy(&xrgb, callbacks.video + pixel * 4u, sizeof xrgb);
        pixel_bytes[0] = (uint8_t)xrgb;
        pixel_bytes[1] = (uint8_t)(xrgb >> 8);
        pixel_bytes[2] = (uint8_t)(xrgb >> 16);
        pixel_bytes[3] = (uint8_t)(xrgb >> 24);
        muhtari__sha256_update(&digest, pixel_bytes, sizeof pixel_bytes);
    }
    muhtari__sha256_final(&digest, value);
    muhtari__sha256_hex(value, video_sha256);
    pcm_hash(callbacks.audio, callbacks.audio_frames, 2u, audio_sha256);
    if (video_sha256[0] == '\0' || audio_sha256[0] == '\0')
        return fail_reason(reason, reason_capacity, "benchmark callback hash was not produced");
    reason[0] = '\0';
    return 1;
}

static int benchmark_adapter(const struct retro_game_info *game,
                             performance_observation *observation, char *reason,
                             size_t reason_capacity)
{
    uint8_t *baseline = NULL;
    size_t baseline_bytes = 0u;
    double sorted[3];
    int result = 0;

    retro_unload_game();
    if (!retro_load_game(game))
        return fail_reason(reason, reason_capacity, "benchmark fixture reload failed");
    for (unsigned frame = 0u; frame < 12u; frame++) {
        if (!benchmark_adapter_frame(frame > 0u, reason, reason_capacity))
            goto done;
    }
    if (!save_adapter_state(&baseline, &baseline_bytes)) {
        (void)fail_reason(reason, reason_capacity, "cannot save benchmark baseline state");
        goto done;
    }
    for (unsigned window = 0u; window < 3u; window++) {
        if (!retro_unserialize(baseline, baseline_bytes)) {
            (void)fail_reason(reason, reason_capacity, "cannot restore benchmark baseline state");
            goto done;
        }
        double start = monotonic_seconds();
        if (start < 0.0) {
            (void)fail_reason(reason, reason_capacity, "monotonic clock is unavailable");
            goto done;
        }
        for (unsigned frame = 0u; frame < 60u; frame++) {
            if (!benchmark_adapter_frame(1, reason, reason_capacity))
                goto done;
        }
        double finish = monotonic_seconds();
        double elapsed = finish - start;
        if (!isfinite(elapsed) || elapsed <= 0.0) {
            (void)fail_reason(reason, reason_capacity,
                              "benchmark window duration is not finite and positive");
            goto done;
        }
        observation->elapsed_seconds[window] = elapsed;
        observation->frames_per_second[window] = 60.0 / elapsed;
    }
    memcpy(sorted, observation->frames_per_second, sizeof sorted);
    for (unsigned i = 0u; i < 3u; i++) {
        for (unsigned j = i + 1u; j < 3u; j++) {
            if (sorted[j] < sorted[i]) {
                double temporary = sorted[i];
                sorted[i] = sorted[j];
                sorted[j] = temporary;
            }
        }
    }
    observation->median_frames_per_second = sorted[1];
    result = 1;

done:
    free(baseline);
    return result;
}

static int last_swcha_library(muhtari *instance, unsigned *value)
{
    muhtari_span memory = {0};
    memory.size = (uint32_t)sizeof memory;
    if (muhtari_get_memory(instance, MUHTARI_MEMORY_SYSTEM_RAM, &memory) != MUHTARI_STATUS_OK ||
        memory.data == NULL || memory.bytes <= LAST_ENTRY)
        return 0;
    size_t entry = memory.data[LAST_ENTRY];
    if (entry + ENTRY_BYTES > memory.bytes)
        return 0;
    *value = memory.data[entry + INPUT_SWCHA];
    return 1;
}

static int last_swcha_adapter(unsigned *value)
{
    const uint8_t *memory = (const uint8_t *)retro_get_memory_data(RETRO_MEMORY_SYSTEM_RAM);
    size_t bytes = retro_get_memory_size(RETRO_MEMORY_SYSTEM_RAM);
    if (memory == NULL || bytes <= LAST_ENTRY)
        return 0;
    size_t entry = memory[LAST_ENTRY];
    if (entry + ENTRY_BYTES > bytes)
        return 0;
    *value = memory[entry + INPUT_SWCHA];
    return 1;
}

static int set_library_input(muhtari *instance, uint16_t first, uint16_t second)
{
    muhtari_input_ex input = {0};
    input.size = (uint32_t)sizeof input;
    input.port[0].device = MUHTARI_PORT_JOYSTICK;
    input.port[1].device = MUHTARI_PORT_JOYSTICK;
    if ((first & (uint16_t)(1u << RETRO_DEVICE_ID_JOYPAD_UP)) != 0u)
        input.port[0].buttons |= MUHTARI_BUTTON_UP;
    if ((second & (uint16_t)(1u << RETRO_DEVICE_ID_JOYPAD_UP)) != 0u)
        input.port[1].buttons |= MUHTARI_BUTTON_UP;
    return muhtari_set_input_ex(instance, &input) == MUHTARI_STATUS_OK;
}

static int run_pair_frame(muhtari *reference, uint16_t first, uint16_t second, unsigned frame_index,
                          int expected_swcha, int steady_audio, frame_observation *output,
                          char *reason, size_t reason_capacity)
{
    uint8_t native_video[VIDEO_WIDTH * VIDEO_HEIGHT];
    int16_t native_audio[AUDIO_CAPACITY];
    muhtari_frame frame = {0};
    uint32_t reference_count;
    unsigned reference_swcha, adapter_swcha;
    numeric_observation numeric = {0};
    muhtari__sha256 video_digest;
    uint8_t digest[32];

    if (!set_library_input(reference, first, second))
        return fail_reason(reason, reason_capacity, "public library refused scripted input");
    frame.size = (uint32_t)sizeof frame;
    frame.video_pitch = VIDEO_WIDTH;
    frame.video = native_video;
    frame.audio = native_audio;
    frame.audio_capacity = AUDIO_CAPACITY;
    if (muhtari_run_frame(reference, &frame) != MUHTARI_STATUS_OK)
        return fail_reason(reason, reason_capacity, "public library frame failed");
    reference_count = frame.audio_count;

    memset(&callbacks, 0, sizeof callbacks);
    pad[0] = first;
    pad[1] = second;
    retro_run();
    if (!last_swcha_library(reference, &reference_swcha) || !last_swcha_adapter(&adapter_swcha))
        return fail_reason(reason, reason_capacity, "system RAM input observation is unavailable");
    if (reference_swcha != adapter_swcha)
        return fail_reason(reason, reason_capacity, "public-library SWCHA differs from adapter");

    numeric.video_calls = callbacks.video_calls;
    numeric.audio_calls = callbacks.batch_calls;
    numeric.poll_calls = callbacks.poll_calls;
    numeric.width = callbacks.width;
    numeric.height = callbacks.height;
    numeric.audio_count = (unsigned)callbacks.audio_frames;
    numeric.swcha = reference_swcha;
    if (!validate_numeric(&numeric, expected_swcha, steady_audio, reason, reason_capacity))
        return 0;
    if (callbacks.sample_calls != 0u || callbacks.audio_frames != reference_count)
        return fail_reason(reason, reason_capacity,
                           "audio callback count differs from public library");
    if (reference_count > AUDIO_CAPACITY)
        return fail_reason(reason, reason_capacity,
                           "public-library audio count exceeds observer bound");

    muhtari__sha256_init(&video_digest);
    for (size_t pixel = 0u; pixel < VIDEO_WIDTH * VIDEO_HEIGHT; pixel++) {
        uint32_t expected = muhtari__palette_rgb_region(native_video[pixel], MUHTARI_REGION_NTSC);
        uint32_t actual;
        uint8_t little_endian[4];
        memcpy(&actual, callbacks.video + pixel * 4u, sizeof actual);
        if (actual != expected)
            return fail_reason(reason, reason_capacity, "XRGB video differs at pixel %lu",
                               (unsigned long)pixel);
        little_endian[0] = (uint8_t)expected;
        little_endian[1] = (uint8_t)(expected >> 8);
        little_endian[2] = (uint8_t)(expected >> 16);
        little_endian[3] = (uint8_t)(expected >> 24);
        muhtari__sha256_update(&video_digest, little_endian, sizeof little_endian);
    }
    muhtari__sha256_final(&video_digest, digest);
    muhtari__sha256_hex(digest, output->video_sha256);

    if (!pcm_matches(native_audio, callbacks.audio, reference_count))
        return fail_reason(reason, reason_capacity, "PCM audio differs from public library");
    pcm_hash(native_audio, reference_count, 1u, output->audio_sha256);
    output->index = frame_index;
    output->swcha = reference_swcha;
    output->audio_count = (unsigned)reference_count;
    return 1;
}

static int save_library_state(muhtari *instance, uint8_t **state, size_t *bytes)
{
    *bytes = muhtari_state_size(instance);
    *state = NULL;
    if (*bytes == 0u)
        return 0;
    *state = (uint8_t *)malloc(*bytes);
    return *state != NULL && muhtari_save_state(instance, *state, *bytes) == MUHTARI_STATUS_OK;
}

static int save_adapter_state(uint8_t **state, size_t *bytes)
{
    *bytes = retro_serialize_size();
    *state = NULL;
    if (*bytes == 0u)
        return 0;
    *state = (uint8_t *)malloc(*bytes);
    return *state != NULL && retro_serialize(*state, *bytes);
}

static int adapter_state_unchanged(const uint8_t *expected, size_t bytes)
{
    uint8_t *actual = (uint8_t *)malloc(bytes);
    int same =
        actual != NULL && retro_serialize(actual, bytes) && memcmp(actual, expected, bytes) == 0;
    free(actual);
    return same;
}

static int state_refusals(muhtari *reference, const uint8_t *library_state, size_t library_bytes,
                          const uint8_t *adapter_state, size_t adapter_bytes, char *reason,
                          size_t reason_capacity)
{
    uint8_t *library_after = NULL;
    uint8_t *adapter_long = NULL, *adapter_corrupt = NULL;
    uint8_t *library_long = NULL, *library_corrupt = NULL;
    size_t library_after_bytes = 0u;
    int result = 0;

    if (retro_unserialize(NULL, adapter_bytes) ||
        !adapter_state_unchanged(adapter_state, adapter_bytes)) {
        (void)fail_reason(reason, reason_capacity,
                          "null adapter state was accepted or changed live state");
        goto done;
    }
    if (retro_unserialize(adapter_state, 0u) ||
        !adapter_state_unchanged(adapter_state, adapter_bytes)) {
        (void)fail_reason(reason, reason_capacity,
                          "empty adapter state was accepted or changed live state");
        goto done;
    }
    if (retro_unserialize(adapter_state, adapter_bytes - 1u) ||
        !adapter_state_unchanged(adapter_state, adapter_bytes)) {
        (void)fail_reason(reason, reason_capacity,
                          "short adapter state was accepted or changed live state");
        goto done;
    }
    adapter_long = (uint8_t *)malloc(adapter_bytes + 1u);
    adapter_corrupt = (uint8_t *)malloc(adapter_bytes);
    library_long = (uint8_t *)malloc(library_bytes + 1u);
    library_corrupt = (uint8_t *)malloc(library_bytes);
    if (adapter_long == NULL || adapter_corrupt == NULL || library_long == NULL ||
        library_corrupt == NULL) {
        (void)fail_reason(reason, reason_capacity, "cannot allocate bounded state controls");
        goto done;
    }
    memcpy(adapter_long, adapter_state, adapter_bytes);
    adapter_long[adapter_bytes] = 0u;
    memcpy(adapter_corrupt, adapter_state, adapter_bytes);
    adapter_corrupt[adapter_bytes / 2u] ^= 0x80u;
    if (retro_unserialize(adapter_long, adapter_bytes + 1u) ||
        !adapter_state_unchanged(adapter_state, adapter_bytes)) {
        (void)fail_reason(reason, reason_capacity,
                          "long adapter state was accepted or changed live state");
        goto done;
    }
    if (retro_unserialize(adapter_corrupt, adapter_bytes) ||
        !adapter_state_unchanged(adapter_state, adapter_bytes)) {
        (void)fail_reason(reason, reason_capacity,
                          "corrupt adapter state was accepted or changed live state");
        goto done;
    }

    if (muhtari_load_state(reference, NULL, library_bytes) == MUHTARI_STATUS_OK ||
        !save_library_state(reference, &library_after, &library_after_bytes) ||
        library_after_bytes != library_bytes ||
        memcmp(library_after, library_state, library_bytes) != 0) {
        (void)fail_reason(reason, reason_capacity, "null public-library state changed live state");
        goto done;
    }
    free(library_after);
    library_after = NULL;
    if (muhtari_load_state(reference, library_state, library_bytes - 1u) == MUHTARI_STATUS_OK ||
        !save_library_state(reference, &library_after, &library_after_bytes) ||
        library_after_bytes != library_bytes ||
        memcmp(library_after, library_state, library_bytes) != 0) {
        (void)fail_reason(reason, reason_capacity, "short public-library state changed live state");
        goto done;
    }
    free(library_after);
    library_after = NULL;
    memcpy(library_long, library_state, library_bytes);
    library_long[library_bytes] = 0u;
    if (muhtari_load_state(reference, library_long, library_bytes + 1u) == MUHTARI_STATUS_OK ||
        !save_library_state(reference, &library_after, &library_after_bytes) ||
        library_after_bytes != library_bytes ||
        memcmp(library_after, library_state, library_bytes) != 0) {
        (void)fail_reason(reason, reason_capacity, "long public-library state changed live state");
        goto done;
    }
    free(library_after);
    library_after = NULL;
    memcpy(library_corrupt, library_state, library_bytes);
    library_corrupt[library_bytes / 2u] ^= 0x80u;
    if (muhtari_load_state(reference, library_corrupt, library_bytes) == MUHTARI_STATUS_OK ||
        !save_library_state(reference, &library_after, &library_after_bytes) ||
        library_after_bytes != library_bytes ||
        memcmp(library_after, library_state, library_bytes) != 0) {
        (void)fail_reason(reason, reason_capacity,
                          "corrupt public-library state changed live state");
        goto done;
    }
    result = 1;

done:
    free(library_after);
    free(adapter_long);
    free(adapter_corrupt);
    free(library_long);
    free(library_corrupt);
    return result;
}

static int same_frame(const frame_observation *left, const frame_observation *right)
{
    return left->swcha == right->swcha && left->audio_count == right->audio_count &&
           strcmp(left->video_sha256, right->video_sha256) == 0 &&
           strcmp(left->audio_sha256, right->audio_sha256) == 0;
}

static int self_test(void)
{
    numeric_observation clean = {1u, 1u, 1u, VIDEO_WIDTH, VIDEO_HEIGHT, 800u, 255u};
    char reason[128];
    const int16_t mono[8] = {1, -2, INT16_MAX, INT16_MIN, 71, 72, 73, 74};
    int16_t stereo[8] = {1, 1, -2, -2, INT16_MAX, INT16_MAX, INT16_MIN, INT16_MIN};
    char mono_hash[65], stereo_hash[65];
    if (!pcm_matches(mono, stereo, 4u)) {
        (void)fprintf(stderr, "libretro observer self-test: mono/stereo PCM control failed\n");
        return 1;
    }
    for (size_t channel = 0u; channel < 2u; channel++) {
        stereo[channel]++;
        if (pcm_matches(mono, stereo, 4u)) {
            (void)fprintf(stderr, "libretro observer self-test: changed PCM channel accepted\n");
            return 1;
        }
        stereo[channel]--;
    }
    pcm_hash(mono, 4u, 1u, mono_hash);
    pcm_hash(stereo, 4u, 2u, stereo_hash);
    if (strcmp(mono_hash, stereo_hash) != 0) {
        (void)fprintf(stderr, "libretro observer self-test: mono/stereo hashes differ\n");
        return 1;
    }
    if (!validate_numeric(&clean, 255, 1, reason, sizeof reason)) {
        (void)fprintf(stderr, "libretro observer self-test: clean observation failed: %s\n",
                      reason);
        return 1;
    }
    clean.video_calls = 0u;
    if (validate_numeric(&clean, 255, 1, reason, sizeof reason) ||
        strcmp(reason, "missing video callback") != 0) {
        (void)fprintf(stderr, "libretro observer self-test: missing callback control failed\n");
        return 1;
    }
    clean.video_calls = 1u;
    clean.width = VIDEO_WIDTH - 1u;
    if (validate_numeric(&clean, 255, 1, reason, sizeof reason) ||
        strcmp(reason, "video geometry must be 320x242") != 0) {
        (void)fprintf(stderr, "libretro observer self-test: changed numeric control failed\n");
        return 1;
    }
    (void)puts("clean observation passed; missing video callback and changed geometry rejected");
    return 0;
}

static int write_report(const char *path, const char *fixture_sha256,
                        const scenario_observation scenarios[SCENARIO_COUNT],
                        const char *state_sha256, const char *continuation_sha256,
                        size_t state_bytes, unsigned no_game_audio,
                        const performance_observation *performance, char *reason,
                        size_t reason_capacity)
{
    char temporary[4096];
    FILE *file;
    if (snprintf(temporary, sizeof temporary, "%s.tmp", path) < 0 ||
        strlen(temporary) >= sizeof temporary - 1u)
        return fail_reason(reason, reason_capacity, "output path is too long");
    file = fopen(temporary, "wb");
    if (file == NULL)
        return fail_reason(reason, reason_capacity, "cannot create observer output");
    (void)fprintf(
        file,
        "{\n  \"schema\": 1,\n  \"fixture_sha256\": \"%s\",\n"
        "  \"source_identity\": \"%s\",\n"
        "  \"numeric_oracles\": {\"width\": 320, \"height\": 242, "
        "\"sample_rate\": 48000, \"audio_counts\": [800, 801], "
        "\"swcha\": {\"neutral\": 255, \"port1_up\": 239, \"port2_up\": 254}},\n"
        "  \"abi\": {\"environment\": {\"get_core_options_version\": %u, "
        "\"set_core_options\": %u, \"get_variable\": %u, "
        "\"get_variable_update\": %u, \"set_pixel_format\": %u, "
        "\"set_support_no_game\": %u, \"set_controller_info\": %u, "
        "\"set_input_descriptors\": %u, \"set_memory_maps\": %u, "
        "\"set_support_achievements\": %u, \"set_message\": %u, "
        "\"get_savestate_context\": %u}, \"pixel_format_xrgb8888\": %u, "
        "\"device_joypad\": %u, \"joypad_up\": %u, \"memory_system_ram\": %u},\n"
        "  \"scenarios\": [\n",
        fixture_sha256, MUHTARI_CORE_SOURCE_ID,
        (unsigned)RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION,
        (unsigned)RETRO_ENVIRONMENT_SET_CORE_OPTIONS, (unsigned)RETRO_ENVIRONMENT_GET_VARIABLE,
        (unsigned)RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE,
        (unsigned)RETRO_ENVIRONMENT_SET_PIXEL_FORMAT,
        (unsigned)RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME,
        (unsigned)RETRO_ENVIRONMENT_SET_CONTROLLER_INFO,
        (unsigned)RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS,
        (unsigned)RETRO_ENVIRONMENT_SET_MEMORY_MAPS,
        (unsigned)RETRO_ENVIRONMENT_SET_SUPPORT_ACHIEVEMENTS,
        (unsigned)RETRO_ENVIRONMENT_SET_MESSAGE, (unsigned)RETRO_ENVIRONMENT_GET_SAVESTATE_CONTEXT,
        (unsigned)RETRO_PIXEL_FORMAT_XRGB8888, (unsigned)RETRO_DEVICE_JOYPAD,
        (unsigned)RETRO_DEVICE_ID_JOYPAD_UP, (unsigned)RETRO_MEMORY_SYSTEM_RAM);
    for (unsigned scenario = 0u; scenario < SCENARIO_COUNT; scenario++) {
        const scenario_observation *item = &scenarios[scenario];
        (void)fprintf(file,
                      "    {\"index\": %u, \"name\": \"%s\", "
                      "\"settled_swcha\": %u, \"frames\": [\n",
                      item->index, item->name, item->settled_swcha);
        for (unsigned frame = 0u; frame < item->frame_count; frame++) {
            const frame_observation *observed = &item->frames[frame];
            (void)fprintf(file,
                          "      {\"index\": %u, \"swcha\": %u, \"audio_count\": %u, "
                          "\"video_sha256\": \"%s\", \"audio_sha256\": \"%s\"}%s\n",
                          observed->index, observed->swcha, observed->audio_count,
                          observed->video_sha256, observed->audio_sha256,
                          frame + 1u == item->frame_count ? "" : ",");
        }
        (void)fprintf(file, "    ]}%s\n", scenario + 1u == SCENARIO_COUNT ? "" : ",");
    }
    (void)fprintf(
        file,
        "  ],\n  \"state\": {\"size\": %lu, \"saved_sha256\": \"%s\", "
        "\"continuation_sha256\": \"%s\", \"replays\": 2, "
        "\"refused_null_short_long_corrupt\": true, \"preserved_after_refusal\": true},\n"
        "  \"content_refusal\": {\"initial_invalid_rejected\": true, "
        "\"loaded_transition_rejected\": true, \"old_game_released\": true, "
        "\"loaded_library_invalid_preserved\": true},\n"
        "  \"no_game\": {\"loaded\": true, \"serialize_size\": 0, "
        "\"serialized\": false, \"unserialized\": false, \"video_calls\": 1, "
        "\"audio_calls\": 1, \"audio_count\": %u},\n"
        "  \"toolchain\": {\"compiler_id\": \"%s\", \"compiler_version\": \"%s\"},\n"
        "  \"performance\": {\"warmup_frames\": 12, \"frames_per_window\": 60, \"windows\": [\n",
        (unsigned long)state_bytes, state_sha256, continuation_sha256, no_game_audio,
        MUHTARI_NATIVE_COMPILER_ID, MUHTARI_NATIVE_COMPILER_VERSION);
    for (unsigned window = 0u; window < 3u; window++) {
        (void)fprintf(file, "    {\"elapsed_seconds\": %.9g, \"frames_per_second\": %.9g}%s\n",
                      performance->elapsed_seconds[window], performance->frames_per_second[window],
                      window == 2u ? "" : ",");
    }
    (void)fprintf(file, "  ], \"median_frames_per_second\": %.9g}\n}\n",
                  performance->median_frames_per_second);
    if (fclose(file) != 0 || replace_report(temporary, path) != 0) {
        (void)remove(temporary);
        return fail_reason(reason, reason_capacity, "cannot finish observer output");
    }
    return 1;
}

static int observe_fixture(const char *fixture_path, const char *output_path)
{
    uint8_t *image = NULL;
    size_t image_bytes = 0u;
    char fixture_sha256[65], saved_sha256[65], continuation_sha256[65];
    char library_continuation_sha256[65], reason[256];
    muhtari_config config = {0};
    muhtari *reference = NULL;
    muhtari_media media = {0};
    muhtari_media invalid_media = {0};
    struct retro_game_info game = {0}, invalid_game = {0};
    scenario_observation scenarios[SCENARIO_COUNT] = {
        {.name = "warmup", .index = 0u, .frame_count = 3u, .settled_swcha = 255u},
        {.name = "neutral", .index = 1u, .frame_count = 3u, .settled_swcha = 255u},
        {.name = "port1_up", .index = 2u, .frame_count = 3u, .settled_swcha = 239u},
        {.name = "continuation", .index = 3u, .frame_count = 2u, .settled_swcha = 255u},
        {.name = "port2_up", .index = 4u, .frame_count = 3u, .settled_swcha = 254u}};
    performance_observation performance = {0};
    uint8_t *library_state = NULL, *adapter_state = NULL;
    uint8_t *library_after = NULL, *adapter_after = NULL;
    size_t library_state_bytes = 0u, adapter_state_bytes = 0u;
    size_t library_after_bytes = 0u, adapter_after_bytes = 0u;
    uint8_t *no_game_state = NULL;
    size_t no_game_state_bytes = 0u;
    unsigned no_game_audio = 0u;

    OBS_CHECK(read_file(fixture_path, &image, &image_bytes), "cannot read generated fixture");
    hash_hex(image, image_bytes, fixture_sha256);

    config.size = (uint32_t)sizeof config;
    config.allocator.alloc = observer_alloc;
    config.allocator.free = observer_free;
    OBS_CHECK(muhtari_create(&config, &reference) == MUHTARI_STATUS_OK,
              "cannot create independent public-library instance");

    media.size = (uint32_t)sizeof media;
    media.image = image;
    media.image_size = image_bytes;
    invalid_media = media;
    invalid_media.image_size = 16u;
    OBS_CHECK(muhtari_load(reference, &invalid_media) != MUHTARI_STATUS_OK,
              "public library accepted truncated input fixture");
    OBS_CHECK(muhtari_load(reference, &media) == MUHTARI_STATUS_OK,
              "public library refused generated fixture");

    retro_set_environment(environment);
    retro_set_video_refresh(video_refresh);
    retro_set_audio_sample(audio_sample);
    retro_set_audio_sample_batch(audio_batch);
    retro_set_input_poll(input_poll);
    retro_set_input_state(input_state);
    retro_init();
    retro_set_controller_port_device(0u, RETRO_DEVICE_JOYPAD);
    retro_set_controller_port_device(1u, RETRO_DEVICE_JOYPAD);
    invalid_game.data = image;
    invalid_game.size = 16u;
    OBS_CHECK(!retro_load_game(&invalid_game), "adapter accepted truncated input fixture");
    game.data = image;
    game.size = image_bytes;
    OBS_CHECK(retro_load_game(&game), "adapter refused generated fixture");
    uint8_t *initial_adapter_state = NULL;
    size_t initial_adapter_state_bytes = 0u;
    OBS_CHECK(save_adapter_state(&initial_adapter_state, &initial_adapter_state_bytes),
              "cannot capture initial adapter state for malformed transition control");
    free(initial_adapter_state);
    OBS_CHECK(!retro_load_game(&invalid_game),
              "adapter accepted truncated image during loaded transition");
    OBS_CHECK(retro_serialize_size() == 0u &&
                  retro_get_memory_data(RETRO_MEMORY_SYSTEM_RAM) == NULL,
              "adapter malformed transition did not release the old game as documented");
    OBS_CHECK(retro_load_game(&game), "adapter refused fixture reload after malformed transition");

    struct retro_system_av_info av = {0};
    retro_get_system_av_info(&av);
    OBS_CHECK(av.geometry.base_width == VIDEO_WIDTH && av.geometry.base_height == VIDEO_HEIGHT,
              "adapter geometry is not 320x242");
    muhtari_info info = {0};
    info.size = (uint32_t)sizeof info;
    OBS_CHECK(muhtari_get_info(reference, &info) == MUHTARI_STATUS_OK &&
                  info.width == VIDEO_WIDTH && info.height == VIDEO_HEIGHT &&
                  info.sample_rate_num == 48000u,
              "public-library geometry or sample rate differs from numeric oracle");

    for (unsigned frame = 0u; frame < scenarios[0].frame_count; frame++) {
        int expected =
            frame + 1u == scenarios[0].frame_count ? (int)scenarios[0].settled_swcha : -1;
        OBS_CHECK(run_pair_frame(reference, 0u, 0u, frame, expected, 0, &scenarios[0].frames[frame],
                                 reason, sizeof reason),
                  "warmup frame %u: %s", frame, reason);
    }
    for (unsigned scenario = 1u; scenario < 3u; scenario++) {
        uint16_t first = scenario == 2u ? (uint16_t)(1u << RETRO_DEVICE_ID_JOYPAD_UP) : 0u;
        for (unsigned frame = 0u; frame < scenarios[scenario].frame_count; frame++) {
            int expected = frame + 1u == scenarios[scenario].frame_count
                               ? (int)scenarios[scenario].settled_swcha
                               : -1;
            OBS_CHECK(run_pair_frame(reference, first, 0u, frame, expected, 1,
                                     &scenarios[scenario].frames[frame], reason, sizeof reason),
                      "%s frame %u: %s", scenarios[scenario].name, frame, reason);
        }
    }

    OBS_CHECK(save_library_state(reference, &library_state, &library_state_bytes),
              "cannot save independent public-library state");
    OBS_CHECK(save_adapter_state(&adapter_state, &adapter_state_bytes),
              "cannot save production adapter state");
    hash_hex(adapter_state, adapter_state_bytes, saved_sha256);

    invalid_media.image_size = 16u;
    OBS_CHECK(muhtari_load(reference, &invalid_media) != MUHTARI_STATUS_OK,
              "public library accepted malformed image during loaded transition");
    OBS_CHECK(save_library_state(reference, &library_after, &library_after_bytes) &&
                  library_after_bytes == library_state_bytes &&
                  memcmp(library_after, library_state, library_state_bytes) == 0,
              "public-library malformed-image refusal changed live state");
    free(library_after);
    library_after = NULL;
    OBS_CHECK(state_refusals(reference, library_state, library_state_bytes, adapter_state,
                             adapter_state_bytes, reason, sizeof reason),
              "%s", reason);

    for (unsigned frame = 0u; frame < scenarios[3].frame_count; frame++) {
        OBS_CHECK(run_pair_frame(reference, 0u, 0u, frame, -1, 1, &scenarios[3].frames[frame],
                                 reason, sizeof reason),
                  "continuation frame %u: %s", frame, reason);
    }
    OBS_CHECK(save_library_state(reference, &library_after, &library_after_bytes) &&
                  save_adapter_state(&adapter_after, &adapter_after_bytes),
              "cannot capture uninterrupted continuation state");
    hash_hex(adapter_after, adapter_after_bytes, continuation_sha256);
    hash_hex(library_after, library_after_bytes, library_continuation_sha256);
    for (unsigned replay = 0u; replay < 2u; replay++) {
        OBS_CHECK(muhtari_load_state(reference, library_state, library_state_bytes) ==
                          MUHTARI_STATUS_OK &&
                      retro_unserialize(adapter_state, adapter_state_bytes),
                  "cannot restore saved state for replay %u", replay);
        for (unsigned frame = 0u; frame < scenarios[3].frame_count; frame++) {
            frame_observation observed = {0};
            OBS_CHECK(
                run_pair_frame(reference, 0u, 0u, frame, -1, 1, &observed, reason, sizeof reason),
                "replay %u frame %u: %s", replay, frame, reason);
            OBS_CHECK(same_frame(&observed, &scenarios[3].frames[frame]),
                      "replay %u frame %u differs from uninterrupted continuation", replay, frame);
        }
        free(library_after);
        library_after = NULL;
        free(adapter_after);
        adapter_after = NULL;
        OBS_CHECK(save_library_state(reference, &library_after, &library_after_bytes) &&
                      save_adapter_state(&adapter_after, &adapter_after_bytes),
                  "replay %u ending state capture failed", replay);
        char replay_state_sha256[65];
        char library_replay_state_sha256[65];
        hash_hex(adapter_after, adapter_after_bytes, replay_state_sha256);
        hash_hex(library_after, library_after_bytes, library_replay_state_sha256);
        OBS_CHECK(strcmp(replay_state_sha256, continuation_sha256) == 0 &&
                      strcmp(library_replay_state_sha256, library_continuation_sha256) == 0,
                  "replay %u ending state differs from uninterrupted continuation", replay);
    }

    OBS_CHECK(muhtari_load_state(reference, library_state, library_state_bytes) ==
                      MUHTARI_STATUS_OK &&
                  retro_unserialize(adapter_state, adapter_state_bytes),
              "cannot restore saved state before port-2 input scenario");
    for (unsigned frame = 0u; frame < scenarios[4].frame_count; frame++) {
        uint16_t second = (uint16_t)(1u << RETRO_DEVICE_ID_JOYPAD_UP);
        int expected =
            frame + 1u == scenarios[4].frame_count ? (int)scenarios[4].settled_swcha : -1;
        OBS_CHECK(run_pair_frame(reference, 0u, second, frame, expected, 1,
                                 &scenarios[4].frames[frame], reason, sizeof reason),
                  "port2_up frame %u: %s", frame, reason);
    }

    OBS_CHECK(benchmark_adapter(&game, &performance, reason, sizeof reason),
              "native fixed-workload measurement failed: %s", reason);

    retro_unload_game();
    OBS_CHECK(retro_load_game(NULL), "adapter refused no-game load");
    no_game_state_bytes = retro_serialize_size();
    OBS_CHECK(no_game_state_bytes == 0u, "no-game state size must be zero");
    uint8_t no_game_buffer[16];
    memset(no_game_buffer, 0xA5, sizeof no_game_buffer);
    OBS_CHECK(!retro_serialize(no_game_buffer, sizeof no_game_buffer) &&
                  !retro_unserialize(adapter_state, adapter_state_bytes),
              "no-game save or restore was unexpectedly accepted");
    for (size_t i = 0u; i < sizeof no_game_buffer; i++)
        OBS_CHECK(no_game_buffer[i] == 0xA5u, "no-game serialization changed output storage");
    memset(&callbacks, 0, sizeof callbacks);
    retro_run();
    numeric_observation no_game_numeric = {0};
    no_game_numeric.video_calls = callbacks.video_calls;
    no_game_numeric.audio_calls = callbacks.batch_calls;
    no_game_numeric.poll_calls = callbacks.poll_calls;
    no_game_numeric.width = callbacks.width;
    no_game_numeric.height = callbacks.height;
    no_game_numeric.audio_count = (unsigned)callbacks.audio_frames;
    no_game_audio = no_game_numeric.audio_count;
    OBS_CHECK(no_game_numeric.video_calls == 1u && no_game_numeric.audio_calls == 1u &&
                  no_game_numeric.poll_calls == 1u && no_game_numeric.width == VIDEO_WIDTH &&
                  no_game_numeric.height == VIDEO_HEIGHT,
              "no-game callback lifecycle differs from expected contract");

    OBS_CHECK(write_report(output_path, fixture_sha256, scenarios, saved_sha256,
                           continuation_sha256, adapter_state_bytes, no_game_audio, &performance,
                           reason, sizeof reason),
              "%s", reason);
    (void)puts("native observer numeric, callback, state and continuation checks passed");

    free(image);
    free(library_state);
    free(adapter_state);
    free(library_after);
    free(adapter_after);
    free(no_game_state);
    muhtari_destroy(reference);
    retro_unload_game();
    retro_deinit();
    return 1;
}

/* The same owned bytes feed native, portable and extracted-package consumers.
   Pin both identities so a plausible look-alike cannot create parity evidence. */
static const char *ym_workload(const char *hash)
{
    if (strcmp(hash, "65c25495be4a0d73ae83a9f3f742186ffda34bff4058c3a5bc304bed428258d6") == 0)
        return "ordinary";
    if (strcmp(hash, "11b6fa4251de4771cfcc19b86ebc9f8c6679ddab211f3b4db42221c47535290b") == 0)
        return "worst";
    return NULL;
}

static int ym_numeric(unsigned height, unsigned count, unsigned index, unsigned video,
                      unsigned audio, unsigned poll)
{
    if (video != 1u || audio != 1u || poll != 1u || (height != 242u && height != 292u))
        return 0;
    if (index == 0u)
        return height == 292u ? count == 960u : count == 54u;
    return height == 292u ? count == 961u || count == 962u : count == 800u || count == 801u;
}

static int ym_capture(unsigned height, unsigned index, frame_observation *out)
{
    memset(&callbacks, 0, sizeof callbacks);
    retro_run();
    OBS_CHECK(callbacks.width == VIDEO_WIDTH && callbacks.height == height &&
                  callbacks.audio_frames <= AUDIO_CAPACITY && callbacks.sample_calls == 0u &&
                  ym_numeric(height, (unsigned)callbacks.audio_frames, index, callbacks.video_calls,
                             callbacks.batch_calls, callbacks.poll_calls),
              "YM numeric/callback observation failed");
    /* XRGB integers have a canonical little-endian encoding in receipts. */
    muhtari__sha256 hash;
    uint8_t digest[32];
    muhtari__sha256_init(&hash);
    for (size_t pixel = 0u; pixel < (size_t)VIDEO_WIDTH * height; pixel++) {
        uint32_t value;
        memcpy(&value, callbacks.video + 4u * pixel, sizeof value);
        uint8_t bytes[4] = {(uint8_t)value, (uint8_t)(value >> 8), (uint8_t)(value >> 16),
                            (uint8_t)(value >> 24)};
        muhtari__sha256_update(&hash, bytes, sizeof bytes);
    }
    muhtari__sha256_final(&hash, digest);
    muhtari__sha256_hex(digest, out->video_sha256);
    for (size_t i = 0u; i < callbacks.audio_frames; i++)
        OBS_CHECK(callbacks.audio[2u * i] == callbacks.audio[2u * i + 1u],
                  "YM duplicated mono differs");
    pcm_hash(callbacks.audio, callbacks.audio_frames, 2u, out->audio_sha256);
    out->index = index;
    out->audio_count = (unsigned)callbacks.audio_frames;
    return 1;
}

static int ym_controls(void)
{
    char reason[256];
    OBS_CHECK(self_test() == 0, "existing observer controls failed");
    OBS_CHECK(ym_numeric(242u, 801u, 1u, 1u, 1u, 1u) && ym_numeric(292u, 962u, 1u, 1u, 1u, 1u),
              "valid YM numbers refused");
    OBS_CHECK(!ym_numeric(242u, 801u, 1u, 0u, 1u, 1u) && !ym_numeric(292u, 801u, 1u, 1u, 1u, 1u) &&
                  !ym_numeric(242u, 800u, 0u, 1u, 1u, 1u),
              "bad YM numbers accepted");
    OBS_CHECK(ym_workload("wrong") == NULL, "wrong fixture identity accepted");
    OBS_CHECK(fail_reason(reason, sizeof reason, "control") == 0, "reason control");
    return 1;
}

static int observe_ym(const char *fixture_path, const char *output_path)
{
    uint8_t *image = NULL;
    size_t image_bytes = 0u;
    char fixture_hash[65];
    OBS_CHECK(read_file(fixture_path, &image, &image_bytes), "cannot read YM fixture");
    hash_hex(image, image_bytes, fixture_hash);
    const char *workload = ym_workload(fixture_hash);
    OBS_CHECK(workload != NULL, "unrecognized YM fixture identity");
    FILE *out = fopen(output_path, "wb");
    OBS_CHECK(out != NULL, "cannot create YM receipt");
    fprintf(out,
            "{\"schema\":2,\"workload\":\"%s\",\"fixture_sha256\":\"%s\","
            "\"source_identity\":\"%s\",\"toolchain\":{\"compiler_id\":\"%s\","
            "\"compiler_version\":\"%s\"},\"abi\":{\"environment\":{"
            "\"get_core_options_version\":%u,\"set_core_options\":%u,\"get_variable\":%u,"
            "\"get_variable_update\":%u,\"set_pixel_format\":%u,\"set_support_no_game\":%u,"
            "\"set_controller_info\":%u,\"set_input_descriptors\":%u,\"set_memory_maps\":%u,"
            "\"set_support_achievements\":%u,\"set_message\":%u,\"get_savestate_context\":%u},"
            "\"pixel_format_xrgb8888\":%u,\"device_joypad\":%u,\"joypad_up\":%u,"
            "\"memory_system_ram\":%u},\"regions\":[\n",
            workload, fixture_hash, MUHTARI_CORE_SOURCE_ID, MUHTARI_NATIVE_COMPILER_ID,
            MUHTARI_NATIVE_COMPILER_VERSION, (unsigned)RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION,
            (unsigned)RETRO_ENVIRONMENT_SET_CORE_OPTIONS, (unsigned)RETRO_ENVIRONMENT_GET_VARIABLE,
            (unsigned)RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE,
            (unsigned)RETRO_ENVIRONMENT_SET_PIXEL_FORMAT,
            (unsigned)RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME,
            (unsigned)RETRO_ENVIRONMENT_SET_CONTROLLER_INFO,
            (unsigned)RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS,
            (unsigned)RETRO_ENVIRONMENT_SET_MEMORY_MAPS,
            (unsigned)RETRO_ENVIRONMENT_SET_SUPPORT_ACHIEVEMENTS,
            (unsigned)RETRO_ENVIRONMENT_SET_MESSAGE,
            (unsigned)RETRO_ENVIRONMENT_GET_SAVESTATE_CONTEXT,
            (unsigned)RETRO_PIXEL_FORMAT_XRGB8888, (unsigned)RETRO_DEVICE_JOYPAD,
            (unsigned)RETRO_DEVICE_ID_JOYPAD_UP, (unsigned)RETRO_MEMORY_SYSTEM_RAM);
    for (unsigned pal = 0u; pal < 2u; pal++) {
        unsigned height = pal ? 292u : 242u;
        uint32_t region = pal ? MUHTARI_REGION_PAL : MUHTARI_REGION_NTSC;
        ym_region = pal ? "pal" : "ntsc";
        retro_set_environment(environment);
        retro_set_video_refresh(video_refresh);
        retro_set_audio_sample(audio_sample);
        retro_set_audio_sample_batch(audio_batch);
        retro_set_input_poll(input_poll);
        retro_set_input_state(input_state);
        retro_init();
        retro_set_controller_port_device(0u, RETRO_DEVICE_JOYPAD);
        retro_set_controller_port_device(1u, RETRO_DEVICE_JOYPAD);
        struct retro_game_info game = {0};
        game.data = image;
        game.size = image_bytes;
        OBS_CHECK(retro_load_game(&game), "YM adapter load failed");
        muhtari *reference = NULL;
        muhtari_config cfg = {0};
        cfg.size = (uint32_t)sizeof cfg;
        cfg.allocator.alloc = observer_alloc;
        cfg.allocator.free = observer_free;
        OBS_CHECK(muhtari_create(&cfg, &reference) == MUHTARI_STATUS_OK, "YM create failed");
        muhtari_media media = {0};
        media.size = (uint32_t)sizeof media;
        media.image = image;
        media.image_size = image_bytes;
        media.region = region;
        media.expansion = MUHTARI_EXPANSION_XM;
        OBS_CHECK(muhtari_load(reference, &media) == MUHTARI_STATUS_OK, "YM public load failed");
        frame_observation frames[6] = {{0}};
        uint8_t *saved = NULL, *finish = NULL;
        size_t saved_bytes = 0u, finish_bytes = 0u;
        int minimum = 32767, maximum = -32768;
        for (unsigned i = 0u; i < 6u; i++) {
            uint8_t video[VIDEO_WIDTH * 292u];
            int16_t audio[AUDIO_CAPACITY];
            muhtari_frame frame = {0};
            frame.size = (uint32_t)sizeof frame;
            frame.video_pitch = VIDEO_WIDTH;
            frame.video = video;
            frame.audio = audio;
            frame.audio_capacity = AUDIO_CAPACITY;
            muhtari_input_ex input = {0};
            input.size = (uint32_t)sizeof input;
            OBS_CHECK(muhtari_set_input_ex(reference, &input) == MUHTARI_STATUS_OK &&
                          muhtari_run_frame(reference, &frame) == MUHTARI_STATUS_OK &&
                          ym_capture(height, i, &frames[i]),
                      "YM paired frame failed");
            OBS_CHECK(frame.audio_count == callbacks.audio_frames &&
                          pcm_matches(audio, callbacks.audio, frame.audio_count),
                      "YM PCM differs");
            for (size_t pixel = 0u; pixel < (size_t)VIDEO_WIDTH * height; pixel++) {
                uint32_t value;
                memcpy(&value, callbacks.video + pixel * 4u, sizeof value);
                OBS_CHECK(value == muhtari__palette_rgb_region(video[pixel], region),
                          "YM video differs");
            }
            for (unsigned j = 0u; j < frame.audio_count; j++) {
                if (audio[j] < minimum)
                    minimum = audio[j];
                if (audio[j] > maximum)
                    maximum = audio[j];
            }
            if (i == 1u)
                OBS_CHECK(save_adapter_state(&saved, &saved_bytes), "YM checkpoint failed");
        }
        uint8_t *ram = retro_get_memory_data(RETRO_MEMORY_SYSTEM_RAM);
        OBS_CHECK(ram != NULL && retro_get_memory_size(RETRO_MEMORY_SYSTEM_RAM) >= 0xb00u &&
                      ram[0xa00u] == 0x80u && ram[0xa01u] == 1u && ram[0xa02u] != 0u &&
                      ram[0xa03u] == 2u && minimum < 0 && maximum > 0,
                  "YM numeric mailbox/PCM failed");
        OBS_CHECK(save_adapter_state(&finish, &finish_bytes), "YM finish state failed");
        char saved_hash[65], finish_hash[65];
        hash_hex(saved, saved_bytes, saved_hash);
        hash_hex(finish, finish_bytes, finish_hash);
        OBS_CHECK(retro_unserialize(saved, saved_bytes), "YM restore failed");
        for (unsigned i = 2u; i < 6u; i++) {
            frame_observation replay = {0};
            OBS_CHECK(ym_capture(height, i, &replay) && same_frame(&replay, &frames[i]),
                      "YM replay differs");
        }
        uint8_t *replayed = NULL;
        size_t replay_bytes = 0u;
        OBS_CHECK(save_adapter_state(&replayed, &replay_bytes) && replay_bytes == finish_bytes &&
                      memcmp(replayed, finish, finish_bytes) == 0,
                  "YM ending state differs");
        free(replayed);
        fprintf(out,
                "%s{\"name\":\"%s\",\"height\":%u,\"sample_rate\":48000,\"minimum\":%d,"
                "\"maximum\":%d,\"mailbox\":[128,1,%u,2],\"state_bytes\":%lu,"
                "\"saved_sha256\":\"%s\",\"continuation_sha256\":\"%s\",\"frames\":[\n",
                pal ? ",\n" : "", ym_region, height, minimum, maximum, (unsigned)ram[0xa02u],
                (unsigned long)saved_bytes, saved_hash, finish_hash);
        for (unsigned i = 0u; i < 6u; i++)
            fprintf(out,
                    "%s{\"index\":%u,\"audio_count\":%u,\"video_sha256\":\"%s\","
                    "\"audio_sha256\":\"%s\"}",
                    i ? ",\n" : "", i, frames[i].audio_count, frames[i].video_sha256,
                    frames[i].audio_sha256);
        for (unsigned i = 6u; i < 12u; i++) {
            frame_observation ignored = {0};
            OBS_CHECK(ym_capture(height, i, &ignored), "YM warmup failed");
        }
        free(saved);
        saved = NULL;
        OBS_CHECK(save_adapter_state(&saved, &saved_bytes), "YM benchmark baseline failed");
        fprintf(out,
                "],\"performance\":{\"warmup_frames\":12,\"frames_per_window\":60,\"windows\":[");
        for (unsigned window = 0u; window < 3u; window++) {
            OBS_CHECK(retro_unserialize(saved, saved_bytes), "YM benchmark restore failed");
            double start = monotonic_seconds();
            for (unsigned i = 0u; i < 60u; i++) {
                frame_observation ignored = {0};
                OBS_CHECK(ym_capture(height, i + 12u, &ignored), "YM benchmark frame failed");
            }
            double elapsed = monotonic_seconds() - start;
            OBS_CHECK(isfinite(elapsed) && elapsed > 0.0, "YM benchmark duration invalid");
            fprintf(out, "%s{\"elapsed_seconds\":%.9g,\"frames_per_second\":%.9g}",
                    window ? "," : "", elapsed, 60.0 / elapsed);
        }
        fprintf(out, "]}}");
        free(saved);
        free(finish);
        muhtari_destroy(reference);
        retro_unload_game();
        retro_deinit();
    }
    ym_region = NULL;
    free(image);
    fprintf(out, "]}\n");
    OBS_CHECK(fclose(out) == 0, "YM receipt write failed");
    return 1;
}

static uint16_t souper_crc(const uint8_t mailbox[256])
{
    uint32_t crc = 0xFFFFu;
    for (unsigned i = 16u; i < 256u; i++) {
        crc ^= (uint32_t)mailbox[i] << 8;
        for (unsigned bit = 0u; bit < 8u; bit++)
            crc = ((crc << 1) ^ ((crc & 0x8000u) != 0u ? 0x1021u : 0u)) & 0xFFFFu;
    }
    return (uint16_t)crc;
}

static int souper_mailbox(const uint8_t *memory, size_t bytes, char *reason, size_t capacity)
{
    static const uint8_t values[5] = {0u, 0u, 64u, 64u, 224u};
    if (memory == NULL || bytes < 0xB00u)
        return fail_reason(reason, capacity, "Souper system RAM mailbox is unavailable");
    const uint8_t *mailbox = memory + 0xA00u;
    uint16_t crc = (uint16_t)((uint16_t)mailbox[8] | ((uint16_t)mailbox[9] << 8));
    if (memcmp(mailbox, "M7DG", 4u) != 0 || mailbox[4] != 1u || mailbox[5] != 0u ||
        mailbox[6] != 5u || mailbox[7] != 22u || crc != souper_crc(mailbox))
        return fail_reason(reason, capacity, "Souper mailbox header or CRC differs from literal oracle");
    for (unsigned i = 0u; i < 5u; i++) {
        const uint8_t *record = mailbox + 16u + 4u * i;
        if (record[0] != i + 1u || record[1] != 1u || record[2] != values[i] || record[3] != 0u)
            return fail_reason(reason, capacity, "Souper mailbox record %u differs from literal oracle",
                               i + 1u);
    }
    return 1;
}

static int observe_souper(const char *fixture_path, const char *output_path)
{
    uint8_t *image = NULL;
    size_t image_bytes = 0u;
    char fixture_hash[65], reason[256];
    muhtari_config config = {0};
    muhtari_media media = {0};
    muhtari *reference = NULL;
    struct retro_game_info game = {0};
    frame_observation frames[12] = {{0}};
    uint8_t unavailable[32];

    OBS_CHECK(read_file(fixture_path, &image, &image_bytes), "cannot read patterned Souper fixture");
    hash_hex(image, image_bytes, fixture_hash);
    config.size = (uint32_t)sizeof config;
    config.allocator.alloc = observer_alloc;
    config.allocator.free = observer_free;
    OBS_CHECK(muhtari_create(&config, &reference) == MUHTARI_STATUS_OK,
              "cannot create independent Souper library instance");
    media.size = (uint32_t)sizeof media;
    media.image = image;
    media.image_size = image_bytes;
    OBS_CHECK(muhtari_load(reference, &media) == MUHTARI_STATUS_OK,
              "public library refused patterned Souper fixture");

    retro_set_environment(environment);
    retro_set_video_refresh(video_refresh);
    retro_set_audio_sample(audio_sample);
    retro_set_audio_sample_batch(audio_batch);
    retro_set_input_poll(input_poll);
    retro_set_input_state(input_state);
    retro_init();
    retro_set_controller_port_device(0u, RETRO_DEVICE_JOYPAD);
    retro_set_controller_port_device(1u, RETRO_DEVICE_JOYPAD);
    game.data = image;
    game.size = image_bytes;
    OBS_CHECK(retro_load_game(&game), "production native libretro adapter refused Souper fixture");

    memset(unavailable, 0xA5, sizeof unavailable);
    OBS_CHECK(muhtari_state_size(reference) == 0u &&
                  muhtari_save_state(reference, unavailable, sizeof unavailable) ==
                      MUHTARI_STATUS_ERR_STATE_UNAVAILABLE &&
                  muhtari_load_state(reference, unavailable, sizeof unavailable) ==
                      MUHTARI_STATUS_ERR_STATE_UNAVAILABLE &&
                  retro_serialize_size() == 0u && !retro_serialize(unavailable, sizeof unavailable) &&
                  !retro_unserialize(unavailable, sizeof unavailable),
              "Souper state operations were not unavailable");
    for (size_t i = 0u; i < sizeof unavailable; i++)
        OBS_CHECK(unavailable[i] == 0xA5u, "Souper state refusal changed caller storage");

    for (unsigned frame = 0u; frame < 12u; frame++)
        OBS_CHECK(run_pair_frame(reference, 0u, 0u, frame, -1, frame > 0u, &frames[frame], reason,
                                 sizeof reason),
                  "Souper public/native libretro frame %u: %s", frame + 1u, reason);

    muhtari_span library_ram = {0};
    library_ram.size = (uint32_t)sizeof library_ram;
    OBS_CHECK(muhtari_get_memory(reference, MUHTARI_MEMORY_SYSTEM_RAM, &library_ram) ==
                      MUHTARI_STATUS_OK &&
                  souper_mailbox(library_ram.data, library_ram.bytes, reason, sizeof reason),
              "public library Souper mailbox: %s", reason);
    const uint8_t *adapter_ram = (const uint8_t *)retro_get_memory_data(RETRO_MEMORY_SYSTEM_RAM);
    size_t adapter_bytes = retro_get_memory_size(RETRO_MEMORY_SYSTEM_RAM);
    OBS_CHECK(souper_mailbox(adapter_ram, adapter_bytes, reason, sizeof reason),
              "native adapter Souper mailbox: %s", reason);
    OBS_CHECK(memcmp(library_ram.data + 0xA00u, adapter_ram + 0xA00u, 256u) == 0,
              "native adapter mailbox differs from public library");

    FILE *out = fopen(output_path, "wb");
    OBS_CHECK(out != NULL, "cannot create Souper native receipt");
    fprintf(out,
            "{\"schema\":3,\"workload\":\"souper\",\"fixture_sha256\":\"%s\","
            "\"source_identity\":\"%s\",\"numeric_oracles\":{\"width\":320,"
            "\"height\":242,\"sample_rate\":48000,\"mailbox_values\":[0,0,64,64,224],"
            "\"state_unavailable\":true},\"abi\":{\"environment\":{"
            "\"get_core_options_version\":%u,\"set_core_options\":%u,\"get_variable\":%u,"
            "\"get_variable_update\":%u,\"set_pixel_format\":%u,\"set_support_no_game\":%u,"
            "\"set_controller_info\":%u,\"set_input_descriptors\":%u,\"set_memory_maps\":%u,"
            "\"set_support_achievements\":%u,\"set_message\":%u,"
            "\"get_savestate_context\":%u},\"pixel_format_xrgb8888\":%u,"
            "\"device_joypad\":%u,\"joypad_up\":%u,\"memory_system_ram\":%u},\"frames\":[",
            fixture_hash, MUHTARI_CORE_SOURCE_ID,
            (unsigned)RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION,
            (unsigned)RETRO_ENVIRONMENT_SET_CORE_OPTIONS, (unsigned)RETRO_ENVIRONMENT_GET_VARIABLE,
            (unsigned)RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE,
            (unsigned)RETRO_ENVIRONMENT_SET_PIXEL_FORMAT,
            (unsigned)RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME,
            (unsigned)RETRO_ENVIRONMENT_SET_CONTROLLER_INFO,
            (unsigned)RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS,
            (unsigned)RETRO_ENVIRONMENT_SET_MEMORY_MAPS,
            (unsigned)RETRO_ENVIRONMENT_SET_SUPPORT_ACHIEVEMENTS,
            (unsigned)RETRO_ENVIRONMENT_SET_MESSAGE, (unsigned)RETRO_ENVIRONMENT_GET_SAVESTATE_CONTEXT,
            (unsigned)RETRO_PIXEL_FORMAT_XRGB8888, (unsigned)RETRO_DEVICE_JOYPAD,
            (unsigned)RETRO_DEVICE_ID_JOYPAD_UP, (unsigned)RETRO_MEMORY_SYSTEM_RAM);
    for (unsigned i = 0u; i < 12u; i++)
        fprintf(out, "%s{\"index\":%u,\"audio_count\":%u,\"video_sha256\":\"%s\","
                    "\"audio_sha256\":\"%s\"}",
                i == 0u ? "" : ",", i, frames[i].audio_count, frames[i].video_sha256,
                frames[i].audio_sha256);
    fputs("]}\n", out);
    OBS_CHECK(fclose(out) == 0, "cannot finish Souper native receipt");
    retro_unload_game();
    retro_deinit();
    muhtari_destroy(reference);
    free(image);
    return 1;
}

int main(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "--ym-self-test") == 0)
        return ym_controls() ? 0 : 1;
    if (argc == 5 && strcmp(argv[1], "--ym-fixture") == 0 && strcmp(argv[3], "--ym-output") == 0)
        return observe_ym(argv[2], argv[4]) ? 0 : 1;
    if (argc == 5 && strcmp(argv[1], "--souper-fixture") == 0 && strcmp(argv[3], "--output") == 0)
        return observe_souper(argv[2], argv[4]) ? 0 : 1;
    const char *fixture = NULL, *output = NULL;
    if (argc == 2 && strcmp(argv[1], "--self-test") == 0)
        return self_test();
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--fixture") == 0 && i + 1 < argc)
            fixture = argv[++i];
        else if (strcmp(argv[i], "--output") == 0 && i + 1 < argc)
            output = argv[++i];
        else {
            (void)fprintf(stderr, "usage: %s --fixture PATH --output PATH | --self-test\n",
                          argv[0]);
            return 2;
        }
    }
    if (fixture == NULL || output == NULL) {
        (void)fprintf(stderr, "usage: %s --fixture PATH --output PATH | --self-test\n", argv[0]);
        return 2;
    }
    return observe_fixture(fixture, output) ? 0 : 1;
}
