/* Public-API-only caller, separately linked to genuine old and current cores.
   All machine observations come from public spans, states, counters and frames. */
#include <muhtari.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "images.h"
#include "sha256.h"

#define REQUIRE(x)                                                                                 \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "compat:%d: %s\n", __LINE__, #x);                                      \
            exit(1);                                                                               \
        }                                                                                          \
    } while (0)
static unsigned allocation_calls;
static void *allocate(void *user, size_t bytes)
{
    (void)user;
    allocation_calls++;
    return malloc(bytes);
}
static void release(void *user, void *ptr, size_t bytes)
{
    (void)user;
    (void)bytes;
    free(ptr);
}
static void hash(const void *bytes, size_t size, char hex[65])
{
    muhtari__sha256 h;
    uint8_t digest[32];
    muhtari__sha256_init(&h);
    muhtari__sha256_update(&h, bytes, size);
    muhtari__sha256_final(&h, digest);
    muhtari__sha256_hex(digest, hex);
}
static void put32(uint8_t *p, uint32_t value)
{
    for (unsigned i = 0; i < 4; i++)
        p[i] = (uint8_t)(value >> (i * 8u));
}
static void write_file(const char *prefix, const char *suffix, const void *bytes, size_t size)
{
    char path[4096];
    int length = snprintf(path, sizeof path, "%s%s", prefix, suffix);
    REQUIRE(length > 0 && (size_t)length < sizeof path);
    FILE *f = fopen(path, "wb");
    REQUIRE(f != NULL);
    REQUIRE(fwrite(bytes, 1, size, f) == size);
    REQUIRE(fclose(f) == 0);
}
static uint8_t *read_file(const char *path, size_t *size)
{
    FILE *f = fopen(path, "rb");
    REQUIRE(f != NULL);
    REQUIRE(fseek(f, 0, SEEK_END) == 0);
    long length = ftell(f);
    REQUIRE(length > 0 && length <= 400000);
    *size = (size_t)length;
    REQUIRE(fseek(f, 0, SEEK_SET) == 0);
    uint8_t *p = malloc(*size);
    REQUIRE(p != NULL);
    REQUIRE(fread(p, 1, *size, f) == *size);
    REQUIRE(fclose(f) == 0);
    return p;
}
static size_t emit_at;
static uint8_t *program;
static void emit(uint16_t address, uint8_t value)
{
    REQUIRE(emit_at + 5 < 4096);
    program[emit_at++] = 0xa9;
    program[emit_at++] = value;
    program[emit_at++] = 0x8d;
    program[emit_at++] = (uint8_t)address;
    program[emit_at++] = (uint8_t)(address >> 8);
}
static void pin(unsigned scl, unsigned sda)
{
    emit(0x0281, (uint8_t)((scl ? 0 : 8) | (sda ? 0 : 4)));
}
static void serial_byte(uint8_t byte)
{
    for (unsigned bit = 0; bit < 8; bit++) {
        unsigned value = (byte >> (7u - bit)) & 1u;
        pin(0, value);
        pin(1, value);
        pin(0, value);
    }
    pin(0, 1);
    pin(1, 1);
    pin(0, 1);
}
static uint8_t image[IMAGES_HEADER + 65536];
static size_t make_image(const char *scenario)
{
    if (strcmp(scenario, "ym") == 0)
        return images_ym2151(image);
    int supergame = strcmp(scenario, "supergame") == 0;
    int xboard = strcmp(scenario, "xboard") == 0;
    int xm = strcmp(scenario, "xm") == 0;
    int savekey = strcmp(scenario, "savekey") == 0;
    size_t body = supergame ? 65536u : 16384u;
    size_t size =
        images_a78(image, supergame ? 3 : 4, (uint32_t)body, supergame ? 6 : 0, (uint32_t)body);
    if (xm)
        image[63] = 1;
    if (savekey)
        image[58] = 2;
    uint8_t *rom = image + IMAGES_HEADER;
    memset(rom, 0xea, body);
    program = rom + body - 16384u;
    emit_at = 0;
    program[emit_at++] = 0x78; /* SEI, CLD, LDX #$FD, TXS. */
    program[emit_at++] = 0xd8;
    program[emit_at++] = 0xa2;
    program[emit_at++] = 0xfd;
    program[emit_at++] = 0x9a;
    emit(1, 7);
    emit(0x15, 4);
    emit(0x17, 7);
    emit(0x19, 15);
    if (xboard || xm) {
        emit(0x0470, xboard ? 0x1b : 0x78);
        emit(0x045f, 3);
        emit(0x0450, 9);
        emit(0x0451, 0xaf);
    }
    if (xm)
        emit(0x1100, 0x51);
    uint16_t loop = (uint16_t)(0xc000u + emit_at);
    /* A real increment makes the execution and RAM continuation observable. */
    program[emit_at++] = 0xee;
    program[emit_at++] = 0x40;
    program[emit_at++] = 0x18;
    if (savekey) {
        emit(0x0280, 0);
        pin(1, 1);
        pin(1, 0);
        pin(0, 0);
        serial_byte(0xa0);
        serial_byte(0);
        serial_byte(3);
        serial_byte(0x5a);
        pin(0, 0);
        pin(1, 0);
        pin(1, 1);
    }
    program[emit_at++] = 0x4c;
    program[emit_at++] = (uint8_t)loop;
    program[emit_at++] = (uint8_t)(loop >> 8);
    images_footer(rom, body);
    rom[body - 4] = 0;
    rom[body - 3] = 0xc0;
    return size;
}
static muhtari *create_machine(const char *scenario, uint32_t region)
{
    muhtari_versions versions = {0};
    versions.size = sizeof versions;
    REQUIRE(muhtari_get_versions(&versions) == MUHTARI_STATUS_OK);
    REQUIRE(versions.api == 8 && versions.abi == 1 &&
            versions.behaviour_revision == EXPECTED_BEHAVIOUR);
    muhtari_config cfg = {0};
    cfg.size = sizeof cfg;
    cfg.allocator.alloc = allocate;
    cfg.allocator.free = release;
    muhtari *m = NULL;
    REQUIRE(muhtari_create(&cfg, &m) == MUHTARI_STATUS_OK);
    muhtari_media media = {0};
    media.size = sizeof media;
    media.image_size = make_image(scenario);
    media.image = image;
    media.region = region;
    media.expansion = strcmp(scenario, "xboard") == 0 ? MUHTARI_EXPANSION_XBOARD
                      : strcmp(scenario, "xm") == 0 || strcmp(scenario, "ym") == 0
                          ? MUHTARI_EXPANSION_XM
                          : MUHTARI_EXPANSION_OFF;
    media.hsc_mode = strcmp(scenario, "xm") == 0 || strcmp(scenario, "ym") == 0 ? MUHTARI_HSC_ON
                                                                                : MUHTARI_HSC_OFF;
    REQUIRE(muhtari_load(m, &media) == MUHTARI_STATUS_OK);
    for (uint32_t kind = MUHTARI_MEMORY_SYSTEM_RAM; kind <= MUHTARI_MEMORY_EXPANSION_RAM; kind++) {
        muhtari_span span = {0};
        span.size = sizeof span;
        REQUIRE(muhtari_get_memory(m, kind, &span) == MUHTARI_STATUS_OK);
        if (span.bytes) {
            span.data[span.bytes - 1] = 0x93;
            span.data[span.bytes / 2] = 0x39;
        }
    }
    muhtari_battery_info battery = {0};
    battery.size = sizeof battery;
    REQUIRE(muhtari_get_battery(m, &battery) == MUHTARI_STATUS_OK);
    if (battery.bytes) {
        battery.data[0] = 0x39;
        battery.data[battery.bytes - 1] = 0x93;
    }
    return m;
}
static uint8_t video[320u * 292u];
static int16_t audio[962];
static muhtari_frame run(muhtari *m, int post_input)
{
    if (post_input) {
        muhtari_input_ex input = {0};
        input.size = sizeof input;
        input.port[0].buttons = MUHTARI_BUTTON_UP;
        REQUIRE(muhtari_set_input_ex(m, &input) == MUHTARI_STATUS_OK);
    }
    muhtari_frame frame = {0};
    frame.size = sizeof frame;
    frame.video = video;
    frame.video_pitch = 320;
    frame.audio = audio;
    frame.audio_capacity = 962;
    REQUIRE(muhtari_run_frame(m, &frame) == MUHTARI_STATUS_OK && frame.audio_count <= 962);
    return frame;
}
static uint8_t *save(muhtari *m, size_t *size)
{
    *size = muhtari_state_size(m);
    uint8_t *bytes = malloc(*size);
    REQUIRE(bytes != NULL);
    REQUIRE(muhtari_save_state(m, bytes, *size) == MUHTARI_STATUS_OK);
    return bytes;
}
static unsigned capture(muhtari *m, const char *scenario)
{
    unsigned count;
    for (count = 1; count <= 12; count++) {
        (void)run(m, 1);
        if (count < 3)
            continue;
        if (strcmp(scenario, "savekey") != 0)
            break;
        size_t bytes;
        uint8_t *state = save(m, &bytes);
        /* Native format 1's final EEPROM pending flag; not a private API call. */
        REQUIRE(bytes > 10970);
        int pending = state[10970] == 1;
        free(state);
        if (pending)
            break;
    }
    REQUIRE(count <= 12);
    muhtari_input_ex input = {0};
    input.size = sizeof input;
    input.port[0].buttons = MUHTARI_BUTTON_DOWN | MUHTARI_BUTTON_1;
    REQUIRE(muhtari_set_input_ex(m, &input) == MUHTARI_STATUS_OK);
    return count;
}
static void continuation(muhtari *m, const char *prefix)
{
    char text[8192], digest[65];
    size_t at = 0;
    muhtari_info info = {0};
    info.size = sizeof info;
    REQUIRE(muhtari_get_info(m, &info) == MUHTARI_STATUS_OK);
    muhtari_battery_info battery = {0};
    battery.size = sizeof battery;
    REQUIRE(muhtari_get_battery(m, &battery) == MUHTARI_STATUS_OK);
    uint64_t generation = battery.generation;
    for (unsigned frame_index = 0; frame_index < 4; frame_index++) {
        muhtari_frame frame = run(m, frame_index != 0);
        hash(video, (size_t)info.width * info.height, digest);
        at += (size_t)snprintf(text + at, sizeof text - at, "frame %u video %s samples %u ",
                               frame_index, digest, frame.audio_count);
        uint8_t canonical[962u * 2u];
        unsigned nonzero = 0;
        for (uint32_t i = 0; i < frame.audio_count; i++) {
            uint16_t value = (uint16_t)audio[i];
            canonical[2u * i] = (uint8_t)value;
            canonical[2u * i + 1] = (uint8_t)(value >> 8);
            nonzero += audio[i] != 0;
        }
        REQUIRE(nonzero > 0);
        hash(canonical, (size_t)frame.audio_count * 2u, digest);
        at += (size_t)snprintf(text + at, sizeof text - at, "pcm %s\n", digest);
    }
    muhtari_counters counters = {0};
    counters.size = sizeof counters;
    REQUIRE(muhtari_get_counters(m, &counters) == MUHTARI_STATUS_OK);
    at += (size_t)snprintf(text + at, sizeof text - at, "frames %llu lag %llu\n",
                           (unsigned long long)counters.frames,
                           (unsigned long long)counters.lag_frames);
    for (uint32_t kind = MUHTARI_MEMORY_SYSTEM_RAM; kind <= MUHTARI_MEMORY_EXPANSION_RAM; kind++) {
        muhtari_span span = {0};
        span.size = sizeof span;
        REQUIRE(muhtari_get_memory(m, kind, &span) == MUHTARI_STATUS_OK);
        hash(span.data, span.bytes, digest);
        at += (size_t)snprintf(text + at, sizeof text - at, "memory %u bytes %lu %s\n", kind,
                               (unsigned long)span.bytes, digest);
    }
    REQUIRE(muhtari_get_battery(m, &battery) == MUHTARI_STATUS_OK &&
            battery.generation >= generation);
    hash(battery.data, battery.bytes, digest);
    at += (size_t)snprintf(text + at, sizeof text - at, "battery %lu generation_delta %llu %s\n",
                           (unsigned long)battery.bytes,
                           (unsigned long long)(battery.generation - generation), digest);
    REQUIRE(at < sizeof text);
    write_file(prefix, ".txt", text, at);
    size_t size;
    uint8_t *state = save(m, &size);
    write_file(prefix, ".after.state", state, size);
    free(state);
}
static void reject(muhtari *m, const uint8_t *bytes, size_t size, int content)
{
    size_t before_size;
    uint8_t *before = save(m, &before_size);
    muhtari_battery_info b = {0};
    b.size = sizeof b;
    REQUIRE(muhtari_get_battery(m, &b) == MUHTARI_STATUS_OK);
    uint8_t *battery = malloc(b.bytes ? b.bytes : 1);
    REQUIRE(battery != NULL);
    if (b.bytes)
        memcpy(battery, b.data, b.bytes);
    uint64_t generation = b.generation;
    unsigned allocations = allocation_calls;
    if (content) {
        muhtari_media media = {0};
        media.size = sizeof media;
        media.image = bytes;
        media.image_size = size;
        media.expansion = MUHTARI_EXPANSION_XM;
        REQUIRE(muhtari_load(m, &media) == MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE);
    } else
        REQUIRE(muhtari_load_state(m, bytes, size) != MUHTARI_STATUS_OK);
    REQUIRE(allocation_calls == allocations);
    size_t after_size;
    uint8_t *after = save(m, &after_size);
    REQUIRE(before_size == after_size && memcmp(before, after, before_size) == 0);
    REQUIRE(muhtari_get_battery(m, &b) == MUHTARI_STATUS_OK && b.generation == generation);
    REQUIRE(b.bytes == 0 || memcmp(b.data, battery, b.bytes) == 0);
    free(before);
    free(after);
    free(battery);
}
int main(int argc, char **argv)
{
    if (argc == 4 && strcmp(argv[1], "corrupt") == 0) {
        size_t size;
        uint8_t *state = read_file(argv[2], &size);
        REQUIRE(size >= 64);
        put32(state + 20, 8);
        put32(state + 24, 0);
        uint32_t crc = UINT32_MAX;
        for (size_t i = 0; i < size; i++) {
            crc ^= state[i];
            for (unsigned bit = 0; bit < 8; bit++)
                crc = (crc >> 1) ^ ((crc & 1u) ? UINT32_C(0xedb88320) : 0u);
        }
        put32(state + 24, crc ^ UINT32_MAX);
        write_file(argv[3], "", state, size);
        free(state);
        return 0;
    }
    REQUIRE(argc == 5 || argc == 6);
    const char *mode = argv[1], *scenario = argv[2];
    uint32_t region = strcmp(argv[3], "pal") == 0 ? MUHTARI_REGION_PAL : MUHTARI_REGION_NTSC;
    muhtari *m = create_machine(scenario, region);
    if (strcmp(mode, "resume") == 0) {
        REQUIRE(argc == 6);
        size_t size;
        uint8_t *state = read_file(argv[4], &size);
        REQUIRE(muhtari_load_state(m, state, size) == MUHTARI_STATUS_OK);
        free(state);
        continuation(m, argv[5]);
    } else {
        (void)capture(m, scenario);
        if (strcmp(mode, "produce") == 0) {
            size_t size;
            uint8_t *state = save(m, &size);
            write_file(argv[4], ".state", state, size);
            free(state);
            continuation(m, argv[4]);
        } else if (strcmp(mode, "refuse") == 0) {
            REQUIRE(argc == 6);
            size_t size;
            uint8_t *state = read_file(argv[4], &size);
            reject(m, state, size, 0);
            free(state);
            continuation(m, argv[5]);
        } else if (strcmp(mode, "refuse-ym") == 0) {
            REQUIRE(argc == 5);
            size_t size = images_ym2151(image);
            reject(m, image, size, 1);
            continuation(m, argv[4]);
        } else
            REQUIRE(0);
    }
    muhtari_destroy(m);
    return 0;
}
