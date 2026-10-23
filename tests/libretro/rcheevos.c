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

#include "../check.h"
#include "internal.h"
#include "libretro.h"
#include "rc_consoles.h"
#include "rc_libretro.h"

muhtari *muhtari__libretro_instance(void);

static const struct retro_memory_map *seen_map;

static bool RETRO_CALLCONV env(unsigned cmd, void *data)
{
    if (cmd == RETRO_ENVIRONMENT_SET_MEMORY_MAPS) {
        seen_map = (const struct retro_memory_map *)data;
        return true;
    }
    return cmd == RETRO_ENVIRONMENT_SET_PIXEL_FORMAT ||
           cmd == RETRO_ENVIRONMENT_SET_SUPPORT_ACHIEVEMENTS;
}

static void RETRO_CALLCONV video(const void *data, unsigned w, unsigned h, size_t pitch)
{
    (void)data;
    (void)w;
    (void)h;
    (void)pitch;
}

static size_t RETRO_CALLCONV audio(const int16_t *data, size_t frames)
{
    (void)data;
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
        return (c->board_options & MUHTARI_BOARD_BANKED_RAM) == 0u && addr - 0x4000u < c->ram_size;
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
    for (uint32_t addr = 0u; addr < 0x10000u; addr++) {
        const uint8_t *p = rc_libretro_memory_find(&regions, addr);
        int ok;
        if (addr >= 0x2800u && addr < 0x3000u) {
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
    free(bytes);
}

int main(int argc, char **argv)
{
    int i;
    if (argc < 2) {
        fprintf(stderr, "usage: muhtari-test-rcheevos <cartridge>...\n");
        return 2;
    }
    for (i = 1; i < argc; i++) {
        check_cart(argv[i]);
    }
    CHECK_DONE();
}
