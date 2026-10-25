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
#define RUN_PICTURE (MUHTARI__WIDTH * MUHTARI__HEIGHT_NTSC)

/* libFuzzer's -max_len in tests.cmake is the cartridge's size. */
_Static_assert(RUN_CART_BYTES == 131200u, "-max_len=131200");

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size);

static void require(int cond)
{
    if (!cond) {
        abort();
    }
}

static muhtari *run_create(alloc_counts *counts)
{
    muhtari_config cfg = alloc_config(counts);
    muhtari *m = NULL;
    require(muhtari_create(&cfg, &m) == MUHTARI_STATUS_OK);
    return m;
}

static void run_traffic(muhtari *m)
{
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
    io.audio_capacity = MUHTARI__MAX_AUDIO_SAMPLES;
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
            require(run_one(m[j], video + j * RUN_PICTURE, audio + j * MUHTARI__MAX_AUDIO_SAMPLES,
                            &count[j]) == MUHTARI_STATUS_OK);
            for (unsigned p = 0u; p < 2u; p++) {
                for (unsigned axis = 0u; axis < 2u; axis++) {
                    int32_t residual = m[j]->ports.motion[p][axis].residual;
                    require(residual >= -255 && residual <= 255);
                }
            }
            require(m[j]->battery.data == battery[j]);
        }
        require(count[0] == count[1] && count[0] <= MUHTARI__MAX_AUDIO_SAMPLES);
        require(memcmp(video, video + RUN_PICTURE, RUN_PICTURE) == 0);
        require(memcmp(audio, audio + MUHTARI__MAX_AUDIO_SAMPLES, count[0] * sizeof *audio) == 0);
        require(memcmp(&m[0]->ports, &m[1]->ports, sizeof m[0]->ports) == 0);
        require(memcmp(m[0]->battery.data, m[1]->battery.data, m[0]->battery.bytes) == 0);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    uint8_t *cart = (uint8_t *)malloc(RUN_CART_BYTES);
    uint8_t *video = (uint8_t *)malloc(2u * RUN_PICTURE);
    int16_t *audio = (int16_t *)malloc(2u * MUHTARI__MAX_AUDIO_SAMPLES * sizeof *audio);
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
    alloc_counts counts[2];
    muhtari *m[2] = {run_create(&counts[0]), run_create(&counts[1])};
    require(muhtari_load(m[0], &media) == MUHTARI_STATUS_OK);
    require(muhtari_load(m[1], &media) == MUHTARI_STATUS_OK);
    run_traffic(m[0]);
    run_traffic(m[1]);
    for (uint32_t f = 0; f < RUN_FRAMES; f++) {
        uint32_t count[2];
        muhtari_status st[2];
        for (uint32_t i = 0; i < 2u; i++) {
            st[i] = run_one(m[i], video + i * RUN_PICTURE, audio + i * MUHTARI__MAX_AUDIO_SAMPLES,
                            &count[i]);
        }
        require(st[0] == st[1] && count[0] == count[1]);
        require(memcmp(video, video + RUN_PICTURE, RUN_PICTURE) == 0);
        require(memcmp(audio, audio + MUHTARI__MAX_AUDIO_SAMPLES, count[0] * sizeof *audio) == 0);
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
