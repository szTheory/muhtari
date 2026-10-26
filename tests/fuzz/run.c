/* The run fuzz target: bounded linear or explicitly selected retail images
   (run_cart.h), loaded with the fast boot into two instances and run for 2 frames
   in each, call by call. Any broken invariant calls abort(), which libFuzzer
   and the replay both report:
   1. both loads return the same status;
   2. each frame returns the same status, audio count and picture in both;
   3. each frame of a loaded cartridge returns within its bound: at the first
      instruction boundary at or after its deadline, and less than RUN_BOUND
      ticks past it, so no program can hold a call (docs/hardware/bus.md,
      The run loop).
   Sanitizer findings and libFuzzer's -timeout catch the rest. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"
#include "muhtari.h"
#include "run_cart.h"
#include "../alloc.h"

/* Four lines. WSYNC can hold an instruction to the end of a line and DMA
   most of the next; the worst seen over 3,000 random cartridges run for 60
   frames each was 794 ticks. */
#define RUN_BOUND (4u * MUHTARI__LINE_TICKS)
#define RUN_FRAMES 2u
#define RUN_AUDIO 962u
#define RUN_PICTURE (MUHTARI__WIDTH * 292u)

/* libFuzzer's -max_len in tests.cmake is the cartridge's size. */
_Static_assert(RUN_CART_BYTES == 1048704u, "-max_len=1048704");

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size);

static void require_at(int cond, int line)
{
    if (!cond) {
        fprintf(stderr, "run invariant failed at line %d\n", line);
        abort();
    }
}
#define require(cond) require_at((cond), __LINE__)

static muhtari *run_create(alloc_counts *counts)
{
    muhtari_config cfg = alloc_config(counts);
    muhtari *m = NULL;
    require(muhtari_create(&cfg, &m) == MUHTARI_STATUS_OK);
    return m;
}

static void run_traffic(muhtari *m)
{
    uint8_t *rom = m->cart.rom, *ram = m->cart.ram, *expansion = m->cart.expansion.ram;
    if (m->cart.info.board == MUHTARI_BOARD_BANKSET) {
        static const uint16_t controls[] = {0x046Fu, 0x0470u, 0x0471u, 0x0472u, 0x0477u, 0x0478u,
                                            0x0479u, 0x047Bu, 0x047Cu, 0x047Du, 0x047Fu, 0x0480u};
        static const uint16_t edges[] = {0x3FFFu, 0x4000u, 0x5FFFu, 0x6000u, 0x7FFFu,
                                         0x8000u, 0xBFFFu, 0xC000u, 0xFFFFu};
        for (uint32_t v = 0u; v < 256u; v++) {
            for (size_t n = 0u; n < sizeof controls / sizeof controls[0]; n++) {
                uint16_t at = controls[n];
                uint8_t before = m->cart.expansion.control;
                muhtari__cart_write(m, at, (uint8_t)v);
                muhtari__cart_write(m, at, (uint8_t)v);
                if (m->cart.expansion.kind == MUHTARI_EXPANSION_OFF ||
                    (m->cart.expansion.kind == MUHTARI_EXPANSION_XM && at != 0x0470u))
                    require(m->cart.expansion.control == before);
            }
            muhtari__cart_write(m, 0x0470u, (uint8_t)v);
            if (m->cart.expansion.kind == MUHTARI_EXPANSION_XM) {
                muhtari__cart_write(m, 0x0478u, (uint8_t)v);
                muhtari__cart_write(m, 0x047Cu, (uint8_t)~v);
            }
            for (size_t n = 0u; n < sizeof edges / sizeof edges[0]; n++) {
                uint16_t at = edges[n];
                muhtari__cart_write(m, at, (uint8_t)v);
                if (m->cart.expansion.kind == MUHTARI_EXPANSION_XBOARD && (v & 8u) != 0u &&
                    at >= 0x4000u && at < 0x8000u)
                    require(expansion[(v & 7u) * 16384u + (at & 16383u)] == (uint8_t)v);
                if (m->cart.expansion.kind == MUHTARI_EXPANSION_XM && at >= 0x4000u &&
                    at < 0x8000u && (v & (at < 0x6000u ? 32u : 64u)) != 0u) {
                    unsigned shift = at < 0x6000u ? 0u : 4u;
                    require(expansion[((v >> shift) & 15u) * 8192u + (at & 8191u)] == (uint8_t)v);
                }
                (void)muhtari__cart_read(m, at, 0);
                (void)muhtari__cart_read(m, at, 1);
                uint8_t bank = m->cart.bank;
                uint64_t ticks = m->bus.ticks;
                (void)muhtari__cart_peek(m, at);
                require(m->cart.bank == bank && m->bus.ticks == ticks);
            }
            if (m->cart.info.rom_size / 2u >= 131072u)
                require(m->cart.bank < m->cart.info.rom_size / 2u / 16384u);
        }
        require(m->cart.rom == rom && m->cart.ram == ram && m->cart.expansion.ram == expansion);
        /* Actual VBE and HALT requests traverse the scheduler while the
           generated fixed-window program loops or jams (bus.md). */
        m->maria.reg[0x1Cu] = 0x40u;
        m->maria.reg[0x0Cu] = 0x18u;
        m->maria.reg[0x10u] = 0x80u;
        for (unsigned zone = 0u; zone < 20u; zone++) {
            m->ram[0x80u + 3u * zone] = 15u;
            m->ram[0x81u + 3u * zone] = 0x19u;
            m->ram[0x82u + 3u * zone] = 0u;
        }
    }
    static const uint16_t neighbors[] = {0x7FFFu, 0x8001u, 0xFF7Fu, 0xFF90u};
    for (uint32_t value = 0u; value < 256u; value++) {
        uint16_t at = m->cart.info.board == MUHTARI_BOARD_ACTIVISION
                          ? (uint16_t)(0xFF80u + value % 16u)
                          : 0x8000u;
        muhtari__cart_write(m, at, (uint8_t)value);
        uint8_t bank = m->cart.bank;
        for (size_t i = 0u; i < sizeof neighbors / sizeof neighbors[0]; i++) {
            muhtari__cart_write(m, neighbors[i], (uint8_t)value);
            if (m->cart.info.board == MUHTARI_BOARD_ABSOLUTE ||
                m->cart.info.board == MUHTARI_BOARD_ACTIVISION)
                require(m->cart.bank == bank);
        }
        if (m->cart.info.board == MUHTARI_BOARD_FRACTALUS) {
            for (uint32_t page = 0u; page < 64u; page++) {
                muhtari__cart_write(m, (uint16_t)(0x4000u + page * 256u + value), (uint8_t)value);
            }
        }
        (void)muhtari__cart_read(m, at, 0);
        (void)muhtari__cart_read(m, at, 1);
        (void)muhtari__cart_peek(m, at);
        require(m->cart.bank == bank);
    }
}

/* Runs one frame into video and audio and checks its bound. */
static muhtari_status run_one(muhtari *m, uint8_t *video, int16_t *audio, uint32_t *count)
{
    muhtari_frame io;
    memset(&io, 0, sizeof io);
    io.size = (uint32_t)sizeof io;
    io.video_pitch = MUHTARI__WIDTH;
    io.video = video;
    io.audio = audio;
    io.audio_capacity = RUN_AUDIO;
    uint8_t loaded = m->loaded;
    uint64_t deadline = m->frame_deadline;
    muhtari_status st = muhtari_run_frame(m, &io);
    if (loaded && st == MUHTARI_STATUS_OK) {
        require(m->bus.ticks >= deadline && m->bus.ticks - deadline < RUN_BOUND);
    }
    *count = io.audio_count;
    return st;
}

/* Nonneutral real devices, rather than digital fields masquerading as
   extended coverage. Old cartridges and their first two frames stay above
   this additional replay. All traffic remains bounded by fixed loop sizes. */
static void run_typed(muhtari *m[2], alloc_counts counts[2], muhtari_media *media,
                      const uint8_t *data, size_t size, uint8_t *video, int16_t *audio)
{
    static const uint32_t devices[] = {
        MUHTARI_PORT_LIGHTGUN,    MUHTARI_PORT_PADDLE,     MUHTARI_PORT_KEYPAD,
        MUHTARI_PORT_DRIVING,     MUHTARI_PORT_TRAKBALL,   MUHTARI_PORT_ST_MOUSE,
        MUHTARI_PORT_AMIGA_MOUSE, MUHTARI_PORT_SNES2ATARI, MUHTARI_PORT_MEGA7800};
    uint8_t selector = size != 0u ? data[0] : 0u;
    uint8_t value = size > 1u ? data[1] : 0xA5u;
    unsigned serial = (selector & 16u) != 0u;
    unsigned save_port = (selector >> 5) & 1u;
    muhtari_input_ex in = {0};
    in.size = (uint32_t)sizeof in;
    for (unsigned p = 0u; p < 2u; p++) {
        muhtari_port_input *port = &in.port[p];
        port->device = devices[(selector % 9u + p * 4u) % 9u];
        if (serial != 0u && p == save_port) {
            port->device = MUHTARI_PORT_SAVEKEY;
            media->port[p] = MUHTARI_PORT_SAVEKEY;
            continue;
        }
        if (port->device == MUHTARI_PORT_LIGHTGUN) {
            port->gun_x = value != 0u ? 319u : 1u;
            port->gun_y = value % 242u;
            port->flags = value & 1u;
            port->buttons = MUHTARI_BUTTON_1;
        } else if (port->device == MUHTARI_PORT_PADDLE) {
            port->paddle[0] = 65535u;
            port->paddle[1] = (uint32_t)value * 257u;
            port->buttons = MUHTARI_BUTTON_1 | MUHTARI_BUTTON_2;
        } else if (port->device == MUHTARI_PORT_KEYPAD) {
            port->keypad = 3u | ((uint32_t)value << 4);
        } else if (port->device == MUHTARI_PORT_SNES2ATARI ||
                   port->device == MUHTARI_PORT_MEGA7800) {
            port->buttons = 0xA00u | value;
        } else {
            port->dx = (value & 1u) != 0u ? INT32_MIN : INT32_MAX;
            if (port->device != MUHTARI_PORT_DRIVING)
                port->dy = (value & 2u) != 0u ? INT32_MAX : INT32_MIN;
            port->buttons = MUHTARI_BUTTON_1;
        }
    }
    uint8_t *battery[2];
    for (unsigned j = 0u; j < 2u; j++) {
        require(muhtari_load(m[j], media) == MUHTARI_STATUS_OK);
        counts[j].fail_from = counts[j].allocs + 1u;
        battery[j] = m[j]->battery.data;
        require(muhtari_set_input_ex(m[j], &in) == MUHTARI_STATUS_OK);
        muhtari_input_ex bad = in;
        bad.port[1].flags = 2u;
        struct muhtari__ports before = m[j]->ports;
        require(muhtari_set_input_ex(m[j], &bad) == MUHTARI_STATUS_ERR_ARGUMENT);
        for (unsigned n = 0u; n < 8u; n++)
            (void)muhtari__bus_peek(m[j], (uint16_t)(8u + n % 6u));
        require(memcmp(&before, &m[j]->ports, sizeof before) == 0);
    }
    for (unsigned f = 0u; f < 2u; f++) {
        uint32_t count[2];
        for (unsigned j = 0u; j < 2u; j++) {
            for (unsigned n = 0u; n < 16u; n++) {
                uint8_t byte = size != 0u ? data[n % size] : (uint8_t)(n * 17u);
                muhtari__bus_write(m[j], 0x0280u, byte);
                muhtari__bus_write(m[j], 0x0281u, (uint8_t)~byte);
                muhtari__bus_write(m[j], 1u, (uint8_t)(byte & 0xC0u));
            }
            require(run_one(m[j], video + j * RUN_PICTURE, audio + j * RUN_AUDIO, &count[j]) ==
                    MUHTARI_STATUS_OK);
            for (unsigned p = 0u; p < 2u; p++) {
                for (unsigned axis = 0u; axis < 2u; axis++) {
                    int32_t residual = m[j]->ports.motion[p][axis].residual;
                    require(residual >= -255 && residual <= 255);
                }
            }
            require(m[j]->battery.data == battery[j]);
        }
        require(count[0] == count[1] && count[0] <= RUN_AUDIO);
        require(memcmp(video, video + RUN_PICTURE, MUHTARI__WIDTH * muhtari__timing(m[0]).height) ==
                0);
        require(memcmp(audio, audio + RUN_AUDIO, count[0] * sizeof *audio) == 0);
        require(memcmp(&m[0]->ports, &m[1]->ports, sizeof m[0]->ports) == 0);
        require(memcmp(m[0]->battery.data, m[1]->battery.data, m[0]->battery.bytes) == 0);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    uint8_t *cart = (uint8_t *)malloc(RUN_CART_BYTES);
    uint8_t *video = (uint8_t *)malloc(2u * RUN_PICTURE);
    int16_t *audio = (int16_t *)malloc(2u * RUN_AUDIO * sizeof *audio);
    require(cart != NULL && video != NULL && audio != NULL);
    struct run_cart_info built = run_cart(data, size, cart);
    muhtari_cart_info info = {0};
    info.size = (uint32_t)sizeof info;
    require(built.bytes <= RUN_CART_BYTES &&
            muhtari_probe(cart, built.bytes, &info) == MUHTARI_STATUS_OK);
    require(info.board == built.board);

    muhtari_media media;
    memset(&media, 0, sizeof media);
    media.size = (uint32_t)sizeof media;
    media.image = cart;
    media.image_size = built.bytes;
    media.hsc_mode = MUHTARI_HSC_OFF;
    if (built.board == MUHTARI_BOARD_BANKSET && size >= 128u)
        media.expansion = data[62] >= MUHTARI_EXPANSION_OFF && data[62] <= MUHTARI_EXPANSION_XM
                              ? data[62]
                              : MUHTARI_EXPANSION_OFF;
    alloc_counts counts[2];
    muhtari *m[2] = {run_create(&counts[0]), run_create(&counts[1])};
    require(muhtari_load(m[0], &media) == MUHTARI_STATUS_OK);
    require(muhtari_load(m[1], &media) == MUHTARI_STATUS_OK);
    uint32_t loaded_allocs[2] = {counts[0].allocs, counts[1].allocs};
    for (unsigned i = 0u; i < 2u; i++)
        counts[i].fail_from = counts[i].allocs + 1u;
    run_traffic(m[0]);
    run_traffic(m[1]);
    for (uint32_t f = 0; f < RUN_FRAMES; f++) {
        uint32_t count[2];
        muhtari_status st[2];
        for (uint32_t i = 0; i < 2u; i++) {
            st[i] = run_one(m[i], video + i * RUN_PICTURE, audio + i * RUN_AUDIO, &count[i]);
        }
        require(st[0] == st[1] && count[0] == count[1]);
        require(count[0] <= RUN_AUDIO);
        size_t picture = MUHTARI__WIDTH * muhtari__timing(m[0]).height;
        require(memcmp(video, video + RUN_PICTURE, picture) == 0);
        require(m[0]->bus.ticks == m[1]->bus.ticks);
        require(memcmp(audio, audio + RUN_AUDIO, count[0] * sizeof *audio) == 0);
    }
    for (unsigned i = 0u; i < 2u; i++) {
        require(counts[i].allocs == loaded_allocs[i]);
        if (built.board == MUHTARI_BOARD_BANKSET) {
            unsigned dma = 0u;
            for (unsigned line = 0u; line < muhtari__timing(m[i]).lines; line++)
                dma += m[i]->maria.stats[line].cost;
            require(dma != 0u);
        }
        counts[i].fail_from = 0u;
    }
    run_typed(m, counts, &media, data, size, video, audio);
    for (uint32_t i = 0u; i < 2u; i++) {
        require(counts[i].allocs == 3u && counts[i].frees == 1u);
        require(muhtari_power_cycle(m[i]) == MUHTARI_STATUS_OK);
        require(counts[i].allocs == 3u && counts[i].frees == 1u);
    }
    muhtari_destroy(m[0]);
    muhtari_destroy(m[1]);
    free(cart);
    free(video);
    free(audio);
    return 0;
}

#include "replay_main.h"
