/* The libretro core's achievement map through rcheevos' own resolver, the
 * code RetroArch runs (rcheevos v12.1.0, MIT, fetched by pins.txt and never
 * vendored).
 *
 *   muhtari-test-rcheevos <cartridge>...
 *
 * libretro/libretro.c is compiled into this program, so the instance behind
 * the map is at hand and each address rcheevos resolves is compared with
 * the core's own view of the bus, muhtari__peek. Each cartridge loads as a
 * frontend loads it and runs 30 frames; then system RAM, the RIOT's RAM and
 * any cartridge RAM are filled with patterns through the public spans, so a
 * pointer one byte off reads a different value. */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include "../check.h"
#include "../cart/images.h"
#include "internal.h"
#include "libretro.h"
#include "rc_consoles.h"
#include "rc_libretro.h"
#include "sha256.h"

muhtari *muhtari__libretro_instance(void);

static const struct retro_memory_map *seen_map;
static muhtari__sha256 observed_video, observed_pcm, observed_raw;
static uint64_t observed_samples, observed_events;
static uint32_t observed_frames;
static int capture;
static bool hsc_enabled;
static const char *hsc_directory;
static const char *expansion_option;
static bool hsc_off;

static void raw_audio(void *user, uint64_t tick, uint8_t source, uint16_t value)
{
    (void)user;
    uint8_t bytes[11];
    for (unsigned i = 0u; i < 8u; i++) {
        bytes[i] = (uint8_t)(tick >> (8u * i));
    }
    bytes[8] = source;
    bytes[9] = (uint8_t)value;
    bytes[10] = (uint8_t)(value >> 8);
    muhtari__sha256_update(&observed_raw, bytes, sizeof bytes);
    observed_events++;
}

static void pcm_audio(int16_t sample)
{
    uint16_t bits = (uint16_t)sample;
    uint8_t bytes[2] = {(uint8_t)bits, (uint8_t)(bits >> 8)};
    muhtari__sha256_update(&observed_pcm, bytes, sizeof bytes);
    observed_samples++;
}

static bool RETRO_CALLCONV env(unsigned cmd, void *data)
{
    if (cmd == RETRO_ENVIRONMENT_GET_VARIABLE && expansion_option != NULL) {
        struct retro_variable *v = (struct retro_variable *)data;
        if (strcmp(v->key, "muhtari_expansion") == 0) {
            v->value = expansion_option;
            return true;
        }
        if (hsc_off && strcmp(v->key, "muhtari_hsc") == 0) {
            v->value = "off";
            return true;
        }
    }
    if (cmd == RETRO_ENVIRONMENT_GET_VARIABLE && hsc_enabled) {
        struct retro_variable *v = (struct retro_variable *)data;
        if (strcmp(v->key, "muhtari_hsc") == 0)
            v->value = "on";
        else if (strcmp(v->key, "muhtari_battery_port2") == 0)
            v->value = "savekey";
        else
            return false;
        return true;
    }
    if (cmd == RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY && hsc_enabled) {
        *(const char **)data = hsc_directory;
        return hsc_directory != NULL;
    }
    if (cmd == RETRO_ENVIRONMENT_SET_MEMORY_MAPS) {
        seen_map = (const struct retro_memory_map *)data;
        return true;
    }
    return cmd == RETRO_ENVIRONMENT_SET_PIXEL_FORMAT ||
           cmd == RETRO_ENVIRONMENT_SET_SUPPORT_ACHIEVEMENTS;
}

static void RETRO_CALLCONV video(const void *data, unsigned w, unsigned h, size_t pitch)
{
    if (capture) {
        CHECK(data != NULL && w == 320u && h == 242u && pitch >= w * 4u);
        if (data != NULL && pitch >= w * 4u) {
            for (unsigned y = 0u; y < h; y++) {
                muhtari__sha256_update(&observed_video, (const uint8_t *)data + y * pitch, w * 4u);
            }
        }
        observed_frames++;
    }
}

static size_t RETRO_CALLCONV audio(const int16_t *data, size_t frames)
{
    if (capture) {
        for (size_t i = 0u; i < frames; i++) {
            CHECK_EQ_U64(data[2u * i], data[2u * i + 1u]);
            pcm_audio(data[2u * i]);
        }
    }
    return frames;
}

static void RETRO_CALLCONV poll_input(void)
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

/* With a map rcheevos never asks the core for a flat buffer. */
static void RC_CCONV no_memory(uint32_t id, rc_libretro_core_memory_info_t *info)
{
    (void)id;
    info->data = NULL;
    info->size = 0u;
}

/* Fills a public span with a pattern seeded by seed. */
static void fill(muhtari *m, uint32_t kind, uint32_t seed)
{
    muhtari_span span;
    size_t i;
    memset(&span, 0, sizeof span);
    span.size = (uint32_t)sizeof span;
    if (muhtari_get_memory(m, kind, &span) == MUHTARI_STATUS_OK && span.data != NULL) {
        for (i = 0; i < span.bytes; i++) {
            span.data[i] = (uint8_t)((i * 7u + seed) ^ (i >> 8));
        }
    }
}

/* Reads the file at path; NULL on failure. */
static unsigned char *read_file(const char *path, size_t *n)
{
    FILE *f = fopen(path, "rb");
    unsigned char *buf = NULL;
    long len;
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) == 0 && (len = ftell(f)) > 0 && fseek(f, 0, SEEK_SET) == 0) {
        buf = (unsigned char *)malloc((size_t)len);
        if (buf != NULL && fread(buf, 1u, (size_t)len, f) != (size_t)len) {
            free(buf);
            buf = NULL;
        }
        *n = (size_t)len;
    }
    (void)fclose(f);
    return buf;
}

/* Whether addr has no memory in the map (HOSTS-AND-INTEGRATION section 2):
   the TIA and MARIA registers and their page 1 copies, the RIOT's registers
   and the space around its RAM, $0500-$17FF, and $3000-$3FFF, which
   rcheevos calls mirrored RAM but the Software Guide gives to external
   devices. */
static int unmapped(uint32_t addr)
{
    return (addr < 0x0200u && (addr & 0xC0u) == 0u) || (addr >= 0x0200u && addr < 0x0480u) ||
           (addr >= 0x0500u && addr < 0x1800u) || (addr >= 0x3000u && addr < 0x4000u);
}

/* The deliberately visible cartridge ranges, independently of descriptors.
   Switched windows and the higher Fractalus mirrors must remain holes. */
static int cart_visible(const muhtari *m, uint32_t addr)
{
    const muhtari_cart_info *c = &m->cart.info;
    if (m->cart.expansion.kind != MUHTARI_EXPANSION_OFF && addr < 0x8000u)
        return 0;
    if (c->board == MUHTARI_BOARD_BANKSET) {
        if (addr < 0x8000u && c->ram_size != 0u)
            return (c->board_options & MUHTARI_BOARD_HALT_BANKED_RAM) == 0u;
        uint32_t half = c->rom_size / 2u;
        return half <= 53248u ? addr >= 0x10000u - half
                             : addr < 0x8000u || addr >= 0xC000u;
    }
    if (c->board == MUHTARI_BOARD_ACTIVISION) {
        return addr < 0xA000u || addr >= 0xE000u;
    }
    if (c->board == MUHTARI_BOARD_ABSOLUTE) {
        return addr >= 0x8000u;
    }
    if (c->board == MUHTARI_BOARD_FRACTALUS) {
        return addr < 0x5000u || addr >= 0x8000u;
    }
    if (addr < 0x8000u && c->ram_size != 0u) {
        return (c->board_options & (MUHTARI_BOARD_BANKED_RAM | MUHTARI_BOARD_HALT_BANKED_RAM)) == 0u && addr - 0x4000u < c->ram_size;
    }
    if (c->board == MUHTARI_BOARD_SUPERGAME) {
        return addr < 0x8000u || addr >= 0xC000u;
    }
    return addr >= 0x10000u - c->rom_size;
}

static uint32_t check_regions(muhtari *m, const struct retro_memory_map *map, int report)
{
    rc_libretro_memory_regions_t regions;
    memset(&regions, 0, sizeof regions);
    if (rc_libretro_memory_init(&regions, map, no_memory, RC_CONSOLE_ATARI_7800) == 0 ||
        regions.count >= RC_LIBRETRO_MAX_MEMORY_REGIONS || regions.total_size != 65536u) {
        return 65536u;
    }
    muhtari_span ram = {0};
    ram.size = (uint32_t)sizeof ram;
    if (muhtari_get_memory(m, MUHTARI_MEMORY_SYSTEM_RAM, &ram) != MUHTARI_STATUS_OK) {
        return 65536u;
    }
    uint32_t mapped = 0u, wrong = 0u;
    muhtari_battery_info battery = {0};
    muhtari_const_span rom = {0};
    battery.size = (uint32_t)sizeof battery;
    rom.size = (uint32_t)sizeof rom;
    CHECK_EQ_U64(muhtari_get_battery(m, &battery), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari_get_hsc_rom(m, &rom), MUHTARI_STATUS_OK);
    for (uint32_t addr = 0u; addr < 0x10000u; addr++) {
        const uint8_t *p = rc_libretro_memory_find(&regions, addr);
        int ok;
        if (m->cart.expansion.kind == MUHTARI_EXPANSION_XM &&
            ((addr >= 0x1000u && addr < 0x1800u) || (addr >= 0x3000u && addr < 0x4000u))) {
            ok = p == NULL;
        } else if (battery.hsc_attached != 0u && addr >= 0x1000u && addr < 0x1800u) {
            ok = p == battery.data + (addr - 0x1000u);
        } else if (rom.data != NULL && addr >= 0x3000u && addr < 0x4000u) {
            ok = p == rom.data + (addr - 0x3000u) && *p == muhtari__peek(m, (uint16_t)addr);
        } else if (m->cart.info.board == MUHTARI_BOARD_BANKSET && addr >= 0x3000u &&
                   addr < 0x4000u && m->cart.info.rom_size == 106496u) {
            ok = p != NULL && *p == m->cart.rom[addr - 0x3000u];
        } else if (addr >= 0x2800u && addr < 0x3000u) {
            ok = p == ram.data + (addr - 0x2000u);
        } else if (unmapped(addr) || (addr >= 0x4000u && !cart_visible(m, addr))) {
            ok = p == NULL;
        } else {
            ok = p != NULL && *p == muhtari__peek(m, (uint16_t)addr);
        }
        mapped += p != NULL ? 1u : 0u;
        if (!ok && wrong++ == 0u && report) {
            fprintf(stderr, "$%04x resolves to %s\n", (unsigned)addr,
                    p == NULL ? "no memory" : "a byte outside the intended bus view");
        }
    }
    if (report) {
        printf("%u regions, %u mapped bytes, %u wrong\n", (unsigned)regions.count, (unsigned)mapped,
               (unsigned)wrong);
    }
    return wrong;
}

/* Accepted per-set geometries and RAM declarations, independently constructed
   by the shared image builders. Every sweep includes every byte and hole. */
static void check_expansion_maps(void)
{
    static const uint32_t sizes[] = {32u, 48u, 52u, 128u, 256u, 512u};
    static const uint8_t ram_codes[] = {0u, 1u, 3u};
    static const char *const profiles[] = {"off", "xboard", "xm"};
    uint8_t *image = malloc(128u + 1048576u);
    CHECK(image != NULL);
    if (image == NULL)
        return;
    retro_set_environment(env);
    retro_init();
    for (unsigned g = 0u; g < sizeof sizes / sizeof sizes[0]; g++) {
        for (unsigned r = 0u; r < 3u; r++) {
            if ((sizes[g] == 48u || sizes[g] == 52u) && r != 0u)
                continue;
            for (unsigned profile = 0u; profile < 3u; profile++) {
                struct retro_game_info game = {0};
                game.size = images_bankset(image, sizes[g], ram_codes[r]);
                game.data = image;
                expansion_option = profiles[profile];
                hsc_off = sizes[g] == 52u;
                seen_map = NULL;
                CHECK(retro_load_game(&game));
                muhtari *m = muhtari__libretro_instance();
                CHECK(m != NULL && seen_map != NULL);
                if (m != NULL && seen_map != NULL) {
                    fill(m, MUHTARI_MEMORY_CART_RAM, 3u);
                    printf("BankSet %u KiB RAM %u %s: ", sizes[g], ram_codes[r], profiles[profile]);
                    CHECK_EQ_U64(check_regions(m, seen_map, 1), 0u);
                }
                retro_unload_game();
            }
        }
    }
    retro_deinit();
    expansion_option = NULL;
    hsc_off = false;
    free(image);
}

/* Loads path, resolves the map with rcheevos for console 51 and checks it. */
static void check_cart(const char *path)
{
    struct retro_game_info game;
    muhtari_span ram;
    muhtari *m;
    size_t size = 0u;
    unsigned char *bytes = read_file(path, &size);
    int i;

    if (bytes == NULL) {
        fprintf(stderr, "cannot read %s\n", path);
        check_failures++;
        return;
    }
    seen_map = NULL;
    retro_set_environment(env);
    retro_init();
    retro_set_video_refresh(video);
    retro_set_audio_sample_batch(audio);
    retro_set_input_poll(poll_input);
    retro_set_input_state(input);
    memset(&game, 0, sizeof game);
    game.path = path;
    game.data = bytes;
    game.size = size;
    CHECK(retro_load_game(&game));
    for (i = 0; i < 30; i++) {
        retro_run();
    }
    m = muhtari__libretro_instance();
    CHECK(m != NULL && seen_map != NULL);
    if (m == NULL || seen_map == NULL) {
        free(bytes);
        return;
    }
    fill(m, MUHTARI_MEMORY_SYSTEM_RAM, 1u);
    fill(m, MUHTARI_MEMORY_RIOT_RAM, 2u);
    fill(m, MUHTARI_MEMORY_CART_RAM, 3u);
    memset(&ram, 0, sizeof ram);
    ram.size = (uint32_t)sizeof ram;
    CHECK_EQ_U64(muhtari_get_memory(m, MUHTARI_MEMORY_SYSTEM_RAM, &ram), MUHTARI_STATUS_OK);

    printf("%s: ", path);
    CHECK_EQ_U64(check_regions(m, seen_map, 1), 0u);
    if (m->cart.info.board == MUHTARI_BOARD_FRACTALUS) {
        muhtari_span physical = {0};
        physical.size = (uint32_t)sizeof physical;
        CHECK_EQ_U64(muhtari_get_memory(m, MUHTARI_MEMORY_CART_RAM, &physical), MUHTARI_STATUS_OK);
        CHECK(physical.data != NULL);
        CHECK_EQ_U64(physical.bytes, 2048u);
        CHECK_EQ_U64(retro_get_memory_size(RETRO_MEMORY_SAVE_RAM), 0u);
        CHECK_EQ_U64(m->cart.info.ram_size, 2048u);
    }
    /* Representative fault: dropping intended fixed ROM must fail even
       though the resolver accepts its pointer-less descriptor. */
    CHECK(seen_map->num_descriptors <= 64u);
    if (seen_map->num_descriptors <= 64u) {
        struct retro_memory_descriptor descriptors[64];
        memcpy(descriptors, seen_map->descriptors,
               seen_map->num_descriptors * sizeof descriptors[0]);
        struct retro_memory_map bad = {descriptors, seen_map->num_descriptors};
        for (unsigned d = 0u; d < bad.num_descriptors; d++) {
            if (descriptors[d].ptr != NULL && descriptors[d].start >= 0x8000u) {
                descriptors[d].ptr = NULL;
                break;
            }
        }
        CHECK(check_regions(m, &bad, 0) != 0u);
    }

    retro_unload_game();
    retro_deinit();
    /* Repeat every retained board with both media and each installed ROM form.
       The source file is deleted before resolver reads to prove core ownership. */
    if (hsc_directory != NULL) {
        char firmware_path[4096];
        CHECK(snprintf(firmware_path, sizeof firmware_path, "%s/highscre.bin", hsc_directory) > 0);
        for (unsigned supplied = 0u; supplied < 2u; supplied++) {
            if (supplied != 0u) {
                FILE *f = fopen(firmware_path, "wb");
                CHECK(f != NULL);
                if (f != NULL) {
                    for (unsigned n = 0u; n < 4096u; n++)
                        CHECK(fputc((int)((n * 13u + (n >> 8)) & 255u), f) != EOF);
                    CHECK_EQ_U64(fclose(f), 0u);
                }
            }
            hsc_enabled = true;
            retro_set_environment(env);
            CHECK(retro_load_game(&game));
            m = muhtari__libretro_instance();
            CHECK(m != NULL);
            if (m == NULL)
                break;
            if (supplied != 0u)
                CHECK_EQ_U64(remove(firmware_path), 0u);
            muhtari_battery_info b = {0};
            muhtari_const_span hsc = {0};
            b.size = (uint32_t)sizeof b;
            hsc.size = (uint32_t)sizeof hsc;
            CHECK_EQ_U64(muhtari_get_battery(m, &b), MUHTARI_STATUS_OK);
            CHECK_EQ_U64(muhtari_get_hsc_rom(m, &hsc), MUHTARI_STATUS_OK);
            CHECK_EQ_U64(b.bytes, 34816u);
            CHECK(retro_get_memory_data(RETRO_MEMORY_SAVE_RAM) == b.data);
            CHECK_EQ_U64(retro_get_memory_size(RETRO_MEMORY_SAVE_RAM), b.bytes);
            for (size_t n = 0u; n < b.bytes; n++)
                b.data[n] = (uint8_t)(n * 7u + (n >> 8));
            for (size_t n = 0u; n < hsc.bytes; n++)
                CHECK_EQ_U64(hsc.data[n], supplied      ? (uint8_t)(n * 13u + (n >> 8))
                                          : n == 0x900u ? 0xC6u
                                          : n == 0x904u ? 0xFEu
                                                        : 0xFFu);
            CHECK_EQ_U64(check_regions(m, seen_map, 1), 0u);
            for (unsigned d = 0u; d < seen_map->num_descriptors; d++) {
                const struct retro_memory_descriptor *desc = &seen_map->descriptors[d];
                if (desc->start == 0x1000u)
                    CHECK(desc->ptr == b.data && desc->flags == 0u);
                if (desc->start == 0x3000u)
                    CHECK(desc->ptr == hsc.data && desc->flags == RETRO_MEMDESC_CONST);
            }
            retro_unload_game();
            retro_deinit();
        }
        hsc_enabled = false;
    }
    free(bytes);
}

struct observation {
    uint8_t video[32], pcm[32], raw[32], mapped[32], mailbox[256];
    uint64_t samples, events;
    uint32_t frames;
};

static void begin_capture(void)
{
    muhtari__sha256_init(&observed_video);
    muhtari__sha256_init(&observed_pcm);
    muhtari__sha256_init(&observed_raw);
    observed_samples = observed_events = 0u;
    observed_frames = 0u;
    capture = 1;
}

static void finish_capture(muhtari *m, struct observation *out)
{
    capture = 0;
    muhtari__sha256_final(&observed_video, out->video);
    muhtari__sha256_final(&observed_pcm, out->pcm);
    muhtari__sha256_final(&observed_raw, out->raw);
    muhtari__sha256 mapped;
    muhtari__sha256_init(&mapped);
    for (uint32_t addr = 0x4000u; addr < 0x10000u; addr++) {
        uint8_t byte = muhtari__cart_peek(m, (uint16_t)addr);
        muhtari__sha256_update(&mapped, &byte, 1u);
    }
    muhtari__sha256_final(&mapped, out->mapped);
    memcpy(out->mailbox, m->ram + 0xA00u, sizeof out->mailbox);
    out->samples = observed_samples;
    out->events = observed_events;
    out->frames = observed_frames;
    CHECK_EQ_U64(out->frames, 600u);
    CHECK(out->samples > 0u);
    CHECK(memcmp(out->mailbox, "M7DG", 4u) == 0 && out->mailbox[5] == 0u &&
          out->mailbox[6] == 27u && out->mailbox[7] == 10u);
}

static void hash_bytes(const void *bytes, size_t n, uint8_t digest[32])
{
    muhtari__sha256 hash;
    muhtari__sha256_init(&hash);
    muhtari__sha256_update(&hash, bytes, n);
    muhtari__sha256_final(&hash, digest);
}

static void *library_alloc(void *user, size_t n)
{
    (void)user;
    return malloc(n);
}

static void library_free(void *user, void *ptr, size_t n)
{
    (void)user;
    (void)n;
    free(ptr);
}

static void library_observe(const uint8_t *bytes, size_t n, struct observation *out)
{
    muhtari_config cfg = {0};
    cfg.size = (uint32_t)sizeof cfg;
    cfg.allocator.alloc = library_alloc;
    cfg.allocator.free = library_free;
    muhtari_media media = {0};
    media.size = (uint32_t)sizeof media;
    media.image = bytes;
    media.image_size = n;
    muhtari *m = NULL;
    CHECK_EQ_U64(muhtari_create(&cfg, &m), MUHTARI_STATUS_OK);
    if (m == NULL) {
        return;
    }
    begin_capture();
    CHECK_EQ_U64(muhtari__synth_observe(m, raw_audio, &observed_raw), MUHTARI_STATUS_OK);
    if (muhtari_load(m, &media) != MUHTARI_STATUS_OK) {
        CHECK(0);
        muhtari_destroy(m);
        return;
    }
    uint8_t pixels[320u * 242u];
    int16_t samples[801];
    muhtari_frame io = {0};
    io.size = (uint32_t)sizeof io;
    io.video = pixels;
    io.video_pitch = 320u;
    io.audio = samples;
    io.audio_capacity = 801u;
    for (uint32_t f = 0u; f < 600u; f++) {
        CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_OK);
        muhtari__sha256_update(&observed_video, pixels, sizeof pixels);
        for (uint32_t sample = 0u; sample < io.audio_count; sample++) {
            pcm_audio(samples[sample]);
        }
        observed_frames++;
    }
    finish_capture(m, out);
    muhtari_destroy(m);
}

/* Actual testdb module, loaded with the same platform primitives as host.c.
   The accessor and raw observer exist only in this test build. */
static void module_observe(const char *path, const uint8_t *bytes, size_t n,
                           struct observation *out)
{
#ifdef _WIN32
    HMODULE module = LoadLibraryA(path);
#define MODULE_SYMBOL(name) FARPROC symbol = GetProcAddress(module, name)
#define MODULE_CLOSE() FreeLibrary(module)
#else
    void *module = dlopen(path, RTLD_NOW);
#define MODULE_SYMBOL(name) void *symbol = dlsym(module, name)
#define MODULE_CLOSE() ((void)dlclose(module))
#endif
    CHECK(module != NULL);
    if (module == NULL) {
        return;
    }
#define MODULE_LOAD(name, result, args)                                                            \
    result(RETRO_CALLCONV *name) args;                                                             \
    {                                                                                              \
        MODULE_SYMBOL("retro_" #name);                                                             \
        if (symbol == NULL) {                                                                      \
            CHECK(0);                                                                              \
            MODULE_CLOSE();                                                                        \
            return;                                                                                \
        }                                                                                          \
        memcpy(&name, &symbol, sizeof name);                                                       \
    }
    MODULE_LOAD(set_environment, void, (retro_environment_t));
    MODULE_LOAD(set_video_refresh, void, (retro_video_refresh_t));
    MODULE_LOAD(set_audio_sample_batch, void, (retro_audio_sample_batch_t));
    MODULE_LOAD(set_input_poll, void, (retro_input_poll_t));
    MODULE_LOAD(set_input_state, void, (retro_input_state_t));
    MODULE_LOAD(init, void, (void));
    MODULE_LOAD(deinit, void, (void));
    MODULE_LOAD(load_game, bool, (const struct retro_game_info *));
    MODULE_LOAD(unload_game, void, (void));
    MODULE_LOAD(run, void, (void));
#undef MODULE_LOAD
    muhtari *(*instance)(void);
    {
        MODULE_SYMBOL("muhtari__libretro_instance");
        CHECK(symbol != NULL);
        if (symbol == NULL) {
            MODULE_CLOSE();
            return;
        }
        memcpy(&instance, &symbol, sizeof instance);
    }
    void (*observe)(muhtari__audio_observer, void *);
    {
        MODULE_SYMBOL("muhtari__libretro_observe");
        CHECK(symbol != NULL);
        if (symbol == NULL) {
            MODULE_CLOSE();
            return;
        }
        memcpy(&observe, &symbol, sizeof observe);
    }
    set_environment(env);
    set_video_refresh(video);
    set_audio_sample_batch(audio);
    set_input_poll(poll_input);
    set_input_state(input);
    init();
    begin_capture();
    observe(raw_audio, &observed_raw);
    struct retro_game_info game = {NULL, bytes, n, NULL};
    int loaded = load_game(&game);
    CHECK(loaded);
    muhtari *m = loaded ? instance() : NULL;
    CHECK(m != NULL && seen_map != NULL);
    if (m != NULL && seen_map != NULL) {
        CHECK_EQ_U64(m->cart.info.source, MUHTARI_SOURCE_HASH);
        CHECK_EQ_U64(m->cart.info.board, MUHTARI_BOARD_ACTIVISION);
        CHECK_EQ_U64(check_regions(m, seen_map, 0), 0u);
        /* A normalized OM offset into retained AM input is a concrete
           pointer error; the exact all-byte resolver check must reject it. */
        if ((m->cart.info.flags & MUHTARI_CART_ACTIVISION_HALF_SWAPPED) != 0u &&
            seen_map->num_descriptors <= 64u) {
            struct retro_memory_descriptor descriptors[64];
            memcpy(descriptors, seen_map->descriptors,
                   seen_map->num_descriptors * sizeof descriptors[0]);
            struct retro_memory_map bad = {descriptors, seen_map->num_descriptors};
            for (unsigned d = 0u; d < bad.num_descriptors; d++) {
                if (descriptors[d].ptr != NULL && descriptors[d].start == 0x4000u) {
                    descriptors[d].offset ^= 0x2000u;
                    break;
                }
            }
            CHECK(check_regions(m, &bad, 0) != 0u);
        }
        for (uint32_t f = 0u; f < 600u; f++) {
            run();
        }
        finish_capture(m, out);
        CHECK_EQ_U64(check_regions(m, seen_map, 0), 0u);
    }
    if (loaded) {
        unload_game();
    }
    deinit();
    MODULE_CLOSE();
#undef MODULE_CLOSE
#undef MODULE_SYMBOL
}

static void compare_observations(const struct observation *a, const struct observation *b,
                                 int same_video)
{
    CHECK(memcmp(a->pcm, b->pcm, sizeof a->pcm) == 0);
    CHECK(memcmp(a->raw, b->raw, sizeof a->raw) == 0);
    CHECK(memcmp(a->mapped, b->mapped, sizeof a->mapped) == 0);
    CHECK(memcmp(a->mailbox, b->mailbox, sizeof a->mailbox) == 0);
    CHECK_EQ_U64(a->samples, b->samples);
    CHECK_EQ_U64(a->events, b->events);
    CHECK_EQ_U64(a->frames, b->frames);
    if (same_video) {
        CHECK(memcmp(a->video, b->video, sizeof a->video) == 0);
    }
}

static void write_pair_file(const char *dir, const char *name, const uint8_t *bytes, size_t n)
{
    char path[4096];
    int length = snprintf(path, sizeof path, "%s/%s", dir, name);
    CHECK(length > 0 && (size_t)length < sizeof path);
    if (length <= 0 || (size_t)length >= sizeof path) {
        return;
    }
    FILE *f = fopen(path, "wb");
    CHECK(f != NULL);
    if (f != NULL) {
        CHECK_EQ_U64(fwrite(bytes, 1u, n, f), n);
        CHECK_EQ_U64(fclose(f), 0);
    }
}

static void check_pair(const char *path, const char *module, const char *dir)
{
    size_t n = 0u;
    uint8_t *om = read_file(path, &n);
    CHECK(om != NULL && n == 131200u);
    if (om == NULL || n != 131200u) {
        free(om);
        return;
    }
    uint8_t *am = (uint8_t *)malloc(n);
    CHECK(am != NULL);
    if (am == NULL) {
        free(om);
        return;
    }
    memcpy(am, om, n);
    /* Independent input construction, not the production normalizer. */
    for (uint32_t bank = 0u; bank < 8u; bank++) {
        memcpy(am + 128u + bank * 16384u, om + 128u + bank * 16384u + 8192u, 8192u);
        memcpy(am + 128u + bank * 16384u + 8192u, om + 128u + bank * 16384u, 8192u);
    }
    struct observation library_om = {0}, module_om = {0};
    for (unsigned header = 0u; header < 2u; header++) {
        size_t skip = header != 0u ? 0u : 128u;
        uint8_t raw_om[32], raw_am[32], after[32];
        hash_bytes(om + skip, n - skip, raw_om);
        hash_bytes(am + skip, n - skip, raw_am);
        CHECK(memcmp(raw_om, raw_am, sizeof raw_om) != 0);
        write_pair_file(dir, header != 0u ? "activision.om.a78" : "activision.om.bin", om + skip,
                        n - skip);
        write_pair_file(dir, header != 0u ? "activision.am.a78" : "activision.am.bin", am + skip,
                        n - skip);
        for (unsigned swapped = 0u; swapped < 2u; swapped++) {
            const uint8_t *bytes = swapped != 0u ? am + skip : om + skip;
            muhtari_cart_info info = {0};
            info.size = (uint32_t)sizeof info;
            CHECK_EQ_U64(muhtari_probe(bytes, n - skip, &info), MUHTARI_STATUS_OK);
            CHECK_EQ_U64(info.source, MUHTARI_SOURCE_HASH);
            CHECK_EQ_U64(info.flags,
                         MUHTARI_CART_HASH_MATCH | (header != 0u ? MUHTARI_CART_HEADER : 0u) |
                             (swapped != 0u ? MUHTARI_CART_ACTIVISION_HALF_SWAPPED : 0u));
            struct observation library = {0}, loaded = {0};
            library_observe(bytes, n - skip, &library);
            module_observe(module, bytes, n - skip, &loaded);
            compare_observations(&library, &loaded, 0);
            if (header == 0u && swapped == 0u) {
                library_om = library;
                module_om = loaded;
            } else {
                compare_observations(&library_om, &library, 1);
                compare_observations(&module_om, &loaded, 1);
            }
            hash_bytes(bytes, n - skip, after);
            CHECK(memcmp(after, swapped != 0u ? raw_am : raw_om, sizeof after) == 0);
            printf("Activision %s %s: 600 frames, %llu PCM samples, %llu raw events; equal "
                   "mapped/mailbox/video/audio, unchanged input\n",
                   header != 0u ? "headered" : "headerless", swapped != 0u ? "AM" : "OM",
                   (unsigned long long)loaded.samples, (unsigned long long)loaded.events);
        }
    }
    free(am);
    free(om);
}

int main(int argc, char **argv)
{
    int i = 1;
    if (argc == 5 && strcmp(argv[1], "--pair") == 0) {
        check_pair(argv[2], argv[3], argv[4]);
        CHECK_DONE();
    }
    if (argc < 2) {
        fprintf(stderr, "usage: muhtari-test-rcheevos <cartridge>...\n");
        return 2;
    }
    check_expansion_maps();
    if (argc > 3 && strcmp(argv[1], "--hsc-dir") == 0) {
        hsc_directory = argv[2];
        i = 3;
    }
    for (; i < argc; i++) {
        check_cart(argv[i]);
    }
    CHECK_DONE();
}
