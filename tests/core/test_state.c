/* Private state-payload tests. State bytes stay in process-owned scratch. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"
#include "muhtari.h"
#include "../alloc.h"
#include "../cart/images.h"
#include "../check.h"
#include "../../runner/crc32.h"

#define STATE_HEADER_BYTES 64u
#define STATE_FIXED_BYTES 10908u
#define STATE_EXPANSION_BYTES 131072u
#define STATE_MAX_BYTES 307932u

enum state_fixture {
    STATE_LINEAR,
    STATE_CART_RAM,
    STATE_XBOARD,
    STATE_XM,
    STATE_LARGEST_GENERATED
};

static uint8_t image[IMAGES_HEADER + 1024u * IMAGES_KIB];
static uint8_t identity_image[IMAGES_HEADER + 32u * IMAGES_KIB];
static uint8_t identity_other[IMAGES_HEADER + 32u * IMAGES_KIB];
static uint8_t identity_raw[32u * IMAGES_KIB];
static uint8_t identity_bios[4096u];
static uint8_t identity_firmware[4096u];
static uint8_t identity_other_firmware[4096u];
static uint8_t battery_program[1024u];
static uint8_t first[STATE_MAX_BYTES];
static uint8_t second[STATE_MAX_BYTES];
static uint8_t public_first[STATE_MAX_BYTES];
static uint8_t public_second[STATE_MAX_BYTES];
static uint8_t public_snapshot[STATE_MAX_BYTES];
static uint8_t public_control[STATE_MAX_BYTES];
static uint8_t public_video_a[320u * 292u];
static uint8_t public_video_b[320u * 292u];
static uint8_t public_video_reference[3u][320u * 292u];
static int16_t public_audio_a[962u];
static int16_t public_audio_b[962u];
static int16_t public_audio_reference[3u][962u];
static uint8_t invalid_image[4] = {0u, 0u, 0u, 0u};
static uint8_t battery_control[34816u];
static muhtari snapshot;

enum boundary_mode { BOUNDARY_AFTER_INSTANCE, BOUNDARY_AFTER_CART };
struct boundary_allocator {
    enum boundary_mode mode;
    uint32_t calls;
    muhtari *instance;
    uint8_t *cart;
    _Alignas(max_align_t) uint8_t storage[2u * STATE_MAX_BYTES + sizeof(muhtari) + 8192u];
};
static struct boundary_allocator boundary;

static size_t payload_index(size_t absolute_offset)
{
    return absolute_offset - STATE_HEADER_BYTES;
}

static void put_u32(uint8_t *bytes, uint32_t value)
{
    bytes[0] = (uint8_t)value;
    bytes[1] = (uint8_t)(value >> 8);
    bytes[2] = (uint8_t)(value >> 16);
    bytes[3] = (uint8_t)(value >> 24);
}

static void put_u16(uint8_t *bytes, uint16_t value)
{
    bytes[0] = (uint8_t)value;
    bytes[1] = (uint8_t)(value >> 8);
}

static void put_u64(uint8_t *bytes, uint64_t value)
{
    put_u32(bytes, (uint32_t)value);
    put_u32(bytes + 4u, (uint32_t)(value >> 32));
}

static uint64_t get_u64(const uint8_t *bytes)
{
    uint64_t lo = (uint64_t)bytes[0] | ((uint64_t)bytes[1] << 8) |
                  ((uint64_t)bytes[2] << 16) | ((uint64_t)bytes[3] << 24);
    uint64_t hi = (uint64_t)bytes[4] | ((uint64_t)bytes[5] << 8) |
                  ((uint64_t)bytes[6] << 16) | ((uint64_t)bytes[7] << 24);
    return lo | (hi << 32);
}

static uint32_t get_u32(const uint8_t *bytes)
{
    return (uint32_t)bytes[0] | ((uint32_t)bytes[1] << 8) | ((uint32_t)bytes[2] << 16) |
           ((uint32_t)bytes[3] << 24);
}

static uint32_t state_crc(const uint8_t *bytes, size_t size)
{
    static const uint8_t zero_crc[4] = {0u, 0u, 0u, 0u};
    uint32_t crc = muhtari__crc32(0u, bytes, 24u);
    crc = muhtari__crc32(crc, zero_crc, sizeof zero_crc);
    return muhtari__crc32(crc, bytes + 28u, size - 28u);
}

static void state_rechecksum(uint8_t *bytes, size_t size)
{
    put_u32(bytes + 24u, 0u);
    put_u32(bytes + 24u, state_crc(bytes, size));
}

static size_t public_battery_offset(const muhtari *m)
{
    size_t expansion = m->cart.expansion.kind == MUHTARI_EXPANSION_OFF
                           ? 0u
                           : STATE_EXPANSION_BYTES;
    return STATE_HEADER_BYTES + STATE_FIXED_BYTES + m->cart.info.ram_size + expansion;
}

static void expect_state_refusal(muhtari *m, uint8_t *candidate, size_t bytes,
                                 muhtari_status expected)
{
    size_t total = muhtari_state_size(m);
    CHECK_EQ_U64(muhtari_save_state(m, first, total), MUHTARI_STATUS_OK);
    memcpy(public_second, candidate, bytes);
    CHECK_EQ_U64(muhtari_load_state(m, candidate, bytes), expected);
    CHECK(memcmp(public_second, candidate, bytes) == 0);
    CHECK_EQ_U64(muhtari_save_state(m, public_second, total), MUHTARI_STATUS_OK);
    CHECK(memcmp(first, public_second, total) == 0);
}

static void check_public_roundtrip(muhtari *m)
{
    size_t bytes = muhtari_state_size(m);
    CHECK(bytes != 0u);
    if (bytes == 0u)
        return;
    CHECK_EQ_U64(muhtari_save_state(m, public_first, bytes), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari_save_state(m, public_second, bytes), MUHTARI_STATUS_OK);
    CHECK(memcmp(public_first, public_second, bytes) == 0);
    CHECK_EQ_U64(muhtari_load_state(m, public_first, bytes), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari_save_state(m, public_second, bytes), MUHTARI_STATUS_OK);
    CHECK(memcmp(public_first, public_second, bytes) == 0);
}

static void *boundary_alloc(void *user, size_t bytes)
{
    struct boundary_allocator *context = (struct boundary_allocator *)user;
    context->calls++;
    if (context->calls == 1u) {
        if (context->mode == BOUNDARY_AFTER_CART)
            context->instance = (muhtari *)malloc(bytes);
        return context->instance;
    }
    if (context->calls == 2u)
        return context->cart;
    return NULL;
}

static void boundary_free(void *user, void *ptr, size_t bytes)
{
    struct boundary_allocator *context = (struct boundary_allocator *)user;
    (void)bytes;
    if (ptr == context->cart ||
        (context->mode == BOUNDARY_AFTER_INSTANCE && ptr == context->instance))
        return;
    free(ptr);
}

static void expect_invalid_u8(muhtari *m, size_t bytes, size_t absolute_offset, uint8_t value)
{
    size_t at = payload_index(absolute_offset);
    uint8_t saved = first[at];
    first[at] = value;
    int accepted = muhtari__state_payload_validate(m, first, bytes);
    if (accepted != 0)
        fprintf(stderr, "u8 mutation accepted: offset=%zu value=%u\n", absolute_offset, value);
    CHECK(!accepted);
    first[at] = saved;
}

static void expect_invalid_u32(muhtari *m, size_t bytes, size_t absolute_offset, uint32_t value)
{
    size_t at = payload_index(absolute_offset);
    uint8_t saved[4];
    memcpy(saved, first + at, sizeof saved);
    put_u32(first + at, value);
    CHECK(!muhtari__state_payload_validate(m, first, bytes));
    memcpy(first + at, saved, sizeof saved);
}

static void expect_invalid_u16(muhtari *m, size_t bytes, size_t absolute_offset, uint16_t value)
{
    size_t at = payload_index(absolute_offset);
    uint8_t saved[2];
    memcpy(saved, first + at, sizeof saved);
    put_u16(first + at, value);
    CHECK(!muhtari__state_payload_validate(m, first, bytes));
    memcpy(first + at, saved, sizeof saved);
}

static void expect_invalid_u64(muhtari *m, size_t bytes, size_t absolute_offset, uint64_t value)
{
    size_t at = payload_index(absolute_offset);
    uint8_t saved[8];
    memcpy(saved, first + at, sizeof saved);
    put_u64(first + at, value);
    CHECK(!muhtari__state_payload_validate(m, first, bytes));
    memcpy(first + at, saved, sizeof saved);
}

static muhtari *state_create(alloc_counts *counts)
{
    muhtari_config config = alloc_config(counts);
    muhtari *m = NULL;
    CHECK_EQ_U64(muhtari_create(&config, &m), MUHTARI_STATUS_OK);
    return m;
}

static muhtari_media state_media_for_image(const void *source, size_t bytes)
{
    muhtari_media media;
    memset(&media, 0, sizeof media);
    media.size = (uint32_t)sizeof media;
    media.image = source;
    media.image_size = bytes;
    return media;
}

static size_t state_make_identity_image(uint8_t *target, const char *title, uint8_t version,
                                        uint16_t cart_type)
{
    const uint32_t body = 32u * 1024u;
    images_header(target, version, title, body, cart_type);
    images_fill(target + IMAGES_HEADER, body, 0x1302u);
    images_footer(target + IMAGES_HEADER, body);
    return IMAGES_HEADER + body;
}

static muhtari_status state_load(muhtari *m, enum state_fixture fixture, int savekey)
{
    size_t image_bytes;
    muhtari_media media;
    memset(&media, 0, sizeof media);
    media.size = (uint32_t)sizeof media;
    if (fixture == STATE_CART_RAM) {
        image_bytes = images_bankset(image, 32u, 3u);
    } else if (fixture == STATE_LARGEST_GENERATED) {
        image_bytes = images_bankset(image, 512u, 3u);
        media.expansion = MUHTARI_EXPANSION_XM;
    } else if (fixture == STATE_XBOARD) {
        image_bytes = images_linear(image, 3u, 49152u);
        media.expansion = MUHTARI_EXPANSION_XBOARD;
    } else {
        image_bytes = images_linear(image, 4u, 4096u);
        if (fixture == STATE_XM)
            media.expansion = MUHTARI_EXPANSION_XM;
    }
    media.image = image;
    media.image_size = image_bytes;
    if (savekey != 0 || fixture == STATE_LARGEST_GENERATED)
        media.port[1] = MUHTARI_PORT_SAVEKEY;
    return muhtari_load(m, &media);
}

static size_t battery_program_bytes;
static uint16_t battery_stop_pc;

static void battery_emit(uint16_t reg, uint8_t value)
{
    const uint8_t code[] = {0xA9u, value, 0x8Du, (uint8_t)reg, (uint8_t)(reg >> 8)};
    CHECK(battery_program_bytes + sizeof code < sizeof battery_program);
    memcpy(battery_program + battery_program_bytes, code, sizeof code);
    battery_program_bytes += sizeof code;
}

static void battery_pin(unsigned port, unsigned scl, unsigned sda)
{
    uint8_t value = (uint8_t)(((scl == 0u ? 8u : 0u) | (sda == 0u ? 4u : 0u))
                              << (port == 0u ? 4u : 0u));
    battery_emit(0x0281u, value);
}

static void battery_byte(unsigned port, uint8_t value)
{
    for (unsigned i = 0u; i < 8u; i++) {
        unsigned bit = (unsigned)(value >> (7u - i)) & 1u;
        battery_pin(port, 0u, bit);
        battery_pin(port, 1u, bit);
        battery_pin(port, 0u, bit);
    }
    battery_pin(port, 0u, 1u);
    battery_pin(port, 1u, 1u);
    battery_pin(port, 0u, 1u);
}

static muhtari_status state_load_hsc_savekey_program(muhtari *m, uint8_t value, int execute)
{
    size_t image_bytes = images_linear(image, 4u, 4096u);
    battery_program_bytes = 0u;
    battery_emit(0x0280u, 0u);
    battery_pin(1u, 1u, 1u);
    battery_pin(1u, 1u, 0u);
    battery_pin(1u, 0u, 0u);
    battery_byte(1u, 0xA0u);
    battery_byte(1u, 0u);
    battery_byte(1u, 3u);
    battery_byte(1u, value);
    battery_pin(1u, 0u, 0u);
    battery_pin(1u, 1u, 0u);
    battery_pin(1u, 1u, 1u);
    battery_stop_pc = (uint16_t)(0xF000u + battery_program_bytes);
    battery_program[battery_program_bytes++] = 0x4Cu;
    battery_program[battery_program_bytes++] = (uint8_t)battery_stop_pc;
    battery_program[battery_program_bytes++] = (uint8_t)(battery_stop_pc >> 8);
    memcpy(image + IMAGES_HEADER, battery_program, battery_program_bytes);

    muhtari_media media = state_media_for_image(image, image_bytes);
    media.hsc_mode = MUHTARI_HSC_ON;
    media.expansion = MUHTARI_EXPANSION_XM;
    media.port[1] = MUHTARI_PORT_SAVEKEY;
    muhtari_status status = muhtari_load(m, &media);
    if (status != MUHTARI_STATUS_OK)
        return status;
    CHECK_EQ_U64(m->battery.bytes, 34816u);
    CHECK_EQ_U64(m->battery.hsc_attached, 1u);
    CHECK_EQ_U64(m->battery.eeprom_port, 2u);
    m->battery.data[0] = 0xC1u;
    m->battery.data[2051u] = 0xFFu;
    m->battery.generation = 0u;

    if (execute != 0) {
        for (unsigned steps = 0u; steps < 1000u && m->cpu.pc != battery_stop_pc; steps++)
            muhtari__cpu_step(m);
        CHECK_EQ_U64(m->cpu.pc, battery_stop_pc);
        CHECK_EQ_U64(m->savekey.pending, 1u);
        CHECK_EQ_HEX(m->battery.data[2051u], 0xFFu);
        CHECK_EQ_U64(m->battery.generation, 0u);
    }
    return status;
}

static void count_state_observation(void *user, uint64_t tick, uint8_t source, uint16_t raw)
{
    (void)tick;
    (void)source;
    (void)raw;
    (*(uint32_t *)user)++;
}

static uint32_t state_run_frame(muhtari *m, uint8_t *video, int16_t *audio)
{
    muhtari_frame frame;
    memset(&frame, 0, sizeof frame);
    frame.size = (uint32_t)sizeof frame;
    frame.video = video;
    frame.video_pitch = 320u;
    frame.audio = audio;
    frame.audio_capacity = 962u;
    CHECK_EQ_U64(muhtari_run_frame(m, &frame), MUHTARI_STATUS_OK);
    return frame.audio_count;
}

static size_t expected_payload_size(const muhtari *m)
{
    size_t expansion = m->cart.expansion.kind == MUHTARI_EXPANSION_OFF
                           ? 0u
                           : STATE_EXPANSION_BYTES;
    return STATE_FIXED_BYTES + m->cart.info.ram_size + expansion + m->battery.bytes;
}

struct continuation_row {
    const char *name;
    uint32_t board;
    uint32_t cart_type;
    uint32_t rom_bytes;
    uint32_t region;
    uint32_t expansion;
    uint32_t hsc_mode;
    uint32_t port[2];
    uint32_t input[2];
    uint32_t legacy;
    uint32_t bios;
    uint32_t firmware;
};

/* Cart type fields follow docs/hardware/cartridge.md, Cart type. The synthetic
   HSC/BIOS vectors below are generated in memory and are not firmware assets. */
static size_t continuation_image(const struct continuation_row *row)
{
    if (row->board == MUHTARI_BOARD_BANKSET) {
        size_t bytes = images_bankset(image, 32u, 3u);
        image[67] = 3u; /* Both documented POKEY windows. */
        return bytes;
    }
    if (row->board == MUHTARI_BOARD_SUPERGAME && row->rom_bytes == 0u) {
        return images_headerless(image, 64u * IMAGES_KIB);
    }
    size_t body = row->rom_bytes != 0u ? row->rom_bytes : 4096u;
    uint16_t cart_type = (uint16_t)row->cart_type;
    size_t bytes = images_a78(image, 3u, (uint32_t)body, cart_type, (uint32_t)body);
    images_footer(image + IMAGES_HEADER, body);
    return bytes;
}

static int continuation_observation_matches(const muhtari *m, const struct continuation_row *row,
                                            uint8_t ram_byte, uint8_t cart_byte,
                                            uint8_t expansion_byte)
{
    if (m->loaded == 0u || m->cart.info.board != row->board || m->ram[0x40u] != ram_byte)
        return 0;
    if (m->cart.ram != NULL && m->cart.ram[0] != cart_byte)
        return 0;
    if (m->cart.ram != NULL &&
        m->cart.ram[m->cart.info.ram_size - 1u] != (uint8_t)(cart_byte + 1u))
        return 0;
    if (m->cart.expansion.ram != NULL && m->cart.expansion.ram[0] != expansion_byte)
        return 0;
    if (m->cart.expansion.ram != NULL &&
        m->cart.expansion.ram[STATE_EXPANSION_BYTES - 1u] !=
            (uint8_t)(expansion_byte + 1u))
        return 0;
    if (m->battery.bytes != 0u &&
        (m->battery.data[0] != 0x39u || m->battery.data[m->battery.bytes - 1u] != 0x93u))
        return 0;
    if (m->synth.raw[0] != 15u || m->synth.pending == 0u || m->synth.pcm_read != 5u ||
        m->synth.pcm_count != 2u || m->synth.pcm[5] != -1234 || m->synth.pcm[6] != 2345)
        return 0;
    if (row->legacy != 0u &&
        (m->ports.pending == 0u || m->ports.pending_device[0] != MUHTARI_PORT_JOYSTICK ||
         m->ports.pending_buttons[0] != (MUHTARI_BUTTON_UP | MUHTARI_BUTTON_2) ||
         m->ports.pending_switches != MUHTARI_SWITCH_SELECT))
        return 0;
    if (row->input[0] != 0u) {
        const muhtari_port_input *pending = &m->ports.pending_ex.port[0];
        if (m->ports.pending == 0u || pending->device != row->input[0])
            return 0;
        switch (row->input[0]) {
        case MUHTARI_PORT_LIGHTGUN:
            if (pending->gun_x != 77u || pending->gun_y != 41u ||
                pending->buttons != MUHTARI_BUTTON_1)
                return 0;
            break;
        case MUHTARI_PORT_PADDLE:
            if (pending->paddle[0] != 0u || pending->paddle[1] != 32768u)
                return 0;
            break;
        case MUHTARI_PORT_KEYPAD:
            if (pending->keypad != 0xA5u)
                return 0;
            break;
        case MUHTARI_PORT_ST_MOUSE:
        case MUHTARI_PORT_AMIGA_MOUSE:
            if (pending->dx != 13 || pending->dy != -7 ||
                pending->buttons != MUHTARI_BUTTON_1)
                return 0;
            break;
        case MUHTARI_PORT_DRIVING:
            if (pending->dx != 13 || pending->dy != 0 ||
                pending->buttons != MUHTARI_BUTTON_1)
                return 0;
            break;
        case MUHTARI_PORT_TRAKBALL:
            if (pending->dx != 13 || pending->dy != -7 ||
                pending->buttons != MUHTARI_BUTTON_1)
                return 0;
            break;
        default:
            if (pending->buttons != (MUHTARI_BUTTON_1 | MUHTARI_BUTTON_2))
                return 0;
            break;
        }
        if (row->input[1] != 0u &&
            (m->ports.pending_ex.port[1].device != row->input[1] ||
             m->ports.pending_ex.port[1].buttons !=
                 (MUHTARI_BUTTON_1 | MUHTARI_BUTTON_2 | MUHTARI_SNES_BUTTON_START)))
            return 0;
    }
    return 1;
}

static void continuation_row_run(const struct continuation_row *row, muhtari *source,
                                 muhtari *restored, alloc_counts *source_counts,
                                 alloc_counts *restored_counts)
{
    size_t image_bytes = continuation_image(row);
    muhtari_media media;
    memset(&media, 0, sizeof media);
    media.size = (uint32_t)sizeof media;
    media.image = image;
    media.image_size = image_bytes;
    media.region = row->region;
    media.expansion = row->expansion;
    media.hsc_mode = row->hsc_mode;
    media.port[0] = row->port[0];
    media.port[1] = row->port[1];
    if (row->bios != 0u) {
        memset(identity_bios, 0xEA, sizeof identity_bios);
        identity_bios[sizeof identity_bios - 4u] = 0x00u;
        identity_bios[sizeof identity_bios - 3u] = 0xF0u;
        media.bios = identity_bios;
        media.bios_size = sizeof identity_bios;
    }
    if (row->firmware != 0u) {
        memset(identity_firmware, 0x31, sizeof identity_firmware);
        media.hsc_rom = identity_firmware;
        media.hsc_rom_size = sizeof identity_firmware;
    }
    muhtari_status source_load = muhtari_load(source, &media);
    muhtari_status restored_load = muhtari_load(restored, &media);
    if (source_load != MUHTARI_STATUS_OK || restored_load != MUHTARI_STATUS_OK) {
        fprintf(stderr, "continuation row %s load failed: %u/%u\n", row->name,
                (unsigned)source_load, (unsigned)restored_load);
        CHECK_EQ_U64(source_load, MUHTARI_STATUS_OK);
        CHECK_EQ_U64(restored_load, MUHTARI_STATUS_OK);
        return;
    }
    CHECK_EQ_U64(source->cart.info.board, row->board);
    CHECK_EQ_U64(source->effective_region, row->region == 0u ? MUHTARI_REGION_NTSC : row->region);
    CHECK(memcmp(source->state_identity, restored->state_identity,
                 sizeof source->state_identity) == 0);

    uint8_t ram_byte = 0x65u;
    uint8_t cart_byte = 0xA7u;
    uint8_t expansion_byte = 0xD3u;
    source->ram[0x40u] = ram_byte;
    if (source->cart.ram != NULL) {
        source->cart.ram[0] = cart_byte;
        source->cart.ram[source->cart.info.ram_size - 1u] = (uint8_t)(cart_byte + 1u);
    }
    if (source->cart.expansion.ram != NULL) {
        source->cart.expansion.ram[0] = expansion_byte;
        source->cart.expansion.ram[STATE_EXPANSION_BYTES - 1u] = (uint8_t)(expansion_byte + 1u);
    }
    if (source->battery.bytes != 0u) {
        source->battery.data[0] = 0x39u;
        source->battery.data[source->battery.bytes - 1u] = 0x93u;
    }
    if (row->legacy != 0u) {
        muhtari_input input;
        memset(&input, 0, sizeof input);
        input.size = (uint32_t)sizeof input;
        input.device[0] = MUHTARI_PORT_JOYSTICK;
        input.device[1] = MUHTARI_PORT_JOYSTICK;
        input.buttons[0] = MUHTARI_BUTTON_UP | MUHTARI_BUTTON_2;
        input.switches = MUHTARI_SWITCH_SELECT;
        muhtari_status input_status = muhtari_set_input(source, &input);
        if (input_status != MUHTARI_STATUS_OK) {
            fprintf(stderr, "continuation row %s legacy input failed: %u\n", row->name,
                    (unsigned)input_status);
            CHECK_EQ_U64(input_status, MUHTARI_STATUS_OK);
            return;
        }
    } else if (row->input[0] != 0u) {
        muhtari_input_ex input;
        memset(&input, 0, sizeof input);
        input.size = (uint32_t)sizeof input;
        input.port[0].device = row->input[0];
        input.port[1].device = row->input[1];
        input.switches = MUHTARI_SWITCH_SELECT;
        if (input.port[0].device == MUHTARI_PORT_LIGHTGUN) {
            input.port[0].gun_x = 77u;
            input.port[0].gun_y = 41u;
            input.port[0].buttons = MUHTARI_BUTTON_1;
        } else if (input.port[0].device == MUHTARI_PORT_PADDLE) {
            input.port[0].paddle[0] = 0u;
            input.port[0].paddle[1] = 32768u;
        } else if (input.port[0].device == MUHTARI_PORT_KEYPAD) {
            input.port[0].keypad = 0xA5u;
        } else if (input.port[0].device == MUHTARI_PORT_ST_MOUSE ||
                   input.port[0].device == MUHTARI_PORT_AMIGA_MOUSE) {
            input.port[0].dx = 13;
            input.port[0].dy = -7;
            input.port[0].buttons = MUHTARI_BUTTON_1;
        } else if (input.port[0].device == MUHTARI_PORT_DRIVING) {
            input.port[0].dx = 13;
            input.port[0].buttons = MUHTARI_BUTTON_1;
        } else if (input.port[0].device == MUHTARI_PORT_TRAKBALL) {
            input.port[0].dx = 13;
            input.port[0].dy = -7;
            input.port[0].buttons = MUHTARI_BUTTON_1;
        } else {
            input.port[0].buttons = MUHTARI_BUTTON_1 | MUHTARI_BUTTON_2;
        }
        if (input.port[1].device == MUHTARI_PORT_MEGA7800 ||
            input.port[1].device == MUHTARI_PORT_SNES2ATARI)
            input.port[1].buttons = MUHTARI_BUTTON_1 | MUHTARI_BUTTON_2 | MUHTARI_SNES_BUTTON_START;
        muhtari_status input_status = muhtari_set_input_ex(source, &input);
        if (input_status != MUHTARI_STATUS_OK) {
            fprintf(stderr, "continuation row %s extended input failed: %u\n", row->name,
                    (unsigned)input_status);
            CHECK_EQ_U64(input_status, MUHTARI_STATUS_OK);
            return;
        }
    }

    uint64_t halt_at = 0u;
    uint64_t halt_until = 0u;
    uint64_t nmi_at = 0u;
    if (row->board == MUHTARI_BOARD_SUPERGAME) {
        source->bus.inptctrl |= MUHTARI__INPTCTRL_MEN;
        source->bus.halt_gate_writes = 2u;
        halt_at = source->bus.ticks + 8u;
        halt_until = halt_at + 20u;
        nmi_at = halt_at + 4u;
        CHECK(muhtari__bus_request_halt(source, halt_at, halt_until));
        CHECK(muhtari__bus_raise_nmi(source, nmi_at));
    }
    muhtari__synth_level(source, source->synth.tick, 0u, 15u);
    CHECK_EQ_U64(source->synth.pending, 1u);
    CHECK_EQ_U64(source->synth.raw[0], 15u);
    /* Seed a valid active FIFO carry. The public resume path must classify
       active slots from the saved cursor, not the destination's live cursor. */
    source->synth.pcm_read = 5u;
    source->synth.pcm_count = 2u;
    source->synth.pcm[5] = -1234;
    source->synth.pcm[6] = 2345;
    muhtari_span spans[3];
    const uint32_t kinds[] = {MUHTARI_MEMORY_SYSTEM_RAM, MUHTARI_MEMORY_CART_RAM,
                              MUHTARI_MEMORY_EXPANSION_RAM};
    for (unsigned i = 0u; i < 3u; i++) {
        memset(&spans[i], 0, sizeof spans[i]);
        spans[i].size = (uint32_t)sizeof spans[i];
        CHECK_EQ_U64(muhtari_get_memory(restored, kinds[i], &spans[i]), MUHTARI_STATUS_OK);
    }
    size_t bytes = muhtari_state_size(source);
    CHECK(bytes > STATE_HEADER_BYTES && bytes <= STATE_MAX_BYTES);
    CHECK_EQ_U64(muhtari_save_state(source, public_first, bytes), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari_save_state(source, public_second, bytes), MUHTARI_STATUS_OK);
    CHECK(memcmp(public_first, public_second, bytes) == 0);
    uint32_t source_allocations = source_counts->allocs;
    uint32_t restored_allocations = restored_counts->allocs;
    CHECK_EQ_U64(muhtari_load_state(restored, public_first, bytes), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(source_counts->allocs, source_allocations);
    CHECK_EQ_U64(restored_counts->allocs, restored_allocations);
    CHECK(continuation_observation_matches(restored, row, ram_byte, cart_byte, expansion_byte));
    if (row->board == MUHTARI_BOARD_SUPERGAME) {
        CHECK_EQ_U64(restored->bus.halt_at, halt_at);
        CHECK_EQ_U64(restored->bus.halt_until, halt_until);
        CHECK_EQ_U64(restored->bus.nmi_at, nmi_at);
        CHECK_EQ_U64(restored->bus.nmi_pending, 1u);
    }

    /* The independent mailbox oracle must reject a split run with one omitted field. */
    uint8_t saved = restored->ram[0x40u];
    restored->ram[0x40u] ^= 1u;
    CHECK(!continuation_observation_matches(restored, row, ram_byte, cart_byte, expansion_byte));
    restored->ram[0x40u] = saved;

    for (unsigned i = 0u; i < 3u; i++) {
        muhtari_span after;
        memset(&after, 0, sizeof after);
        after.size = (uint32_t)sizeof after;
        CHECK_EQ_U64(muhtari_get_memory(restored, kinds[i], &after), MUHTARI_STATUS_OK);
        CHECK(after.data == spans[i].data);
        CHECK_EQ_U64(after.bytes, spans[i].bytes);
    }

    for (unsigned frame = 0u; frame < 2u; frame++) {
        memset(public_video_a, 0, sizeof public_video_a);
        memset(public_video_b, 0, sizeof public_video_b);
        memset(public_audio_a, 0, sizeof public_audio_a);
        memset(public_audio_b, 0, sizeof public_audio_b);
        uint32_t a_count = state_run_frame(source, public_video_a, public_audio_a);
        uint32_t b_count = state_run_frame(restored, public_video_b, public_audio_b);
        CHECK_EQ_U64(a_count, b_count);
        CHECK(memcmp(public_video_a, public_video_b, 320u *
                     (size_t)muhtari__timing(source).height) == 0);
        CHECK(memcmp(public_audio_a, public_audio_b,
                     (size_t)a_count * sizeof public_audio_a[0]) == 0);
        if (frame == 0u) {
            CHECK(public_audio_a[0] == -1234);
            CHECK(public_audio_a[1] == 2345);
        }
        CHECK_EQ_U64(source->synth.pcm_count, restored->synth.pcm_count);
        CHECK_EQ_U64(source->synth.pcm_read, restored->synth.pcm_read);
        if (row->board == MUHTARI_BOARD_LINEAR && frame == 0u) {
            /* With MARIA DMA disabled, the untouched border is background 0
               (docs/hardware/maria.md, Rendering). */
            CHECK_EQ_U64(public_video_a[0], 0u);
        }
        if (frame == 0u) {
            if (row->input[0] == MUHTARI_PORT_PADDLE) {
                /* At NTSC, position 0's 10 kOhm path charges before the
                   first handoff frame ends; position 32768 does not. */
                CHECK_EQ_U64(muhtari__ports_inpt(source, 0u), 1u);
                CHECK_EQ_U64(muhtari__ports_inpt(source, 1u), 0u);
            }
            if (row->input[0] == MUHTARI_PORT_KEYPAD) {
                CHECK_EQ_U64(source->ports.keypad_keys[0], 0xA5u);
                CHECK_EQ_U64(muhtari__ports_inpt(source, 4u), 1u);
            }
            if (row->input[0] == MUHTARI_PORT_LIGHTGUN) {
                uint64_t expected_sample = source->maria.origin + 53u * 454u + 135u + 77u;
                uint64_t frame_ticks = muhtari__timing(source).frame_ticks;
                while (expected_sample <= source->bus.ticks)
                    expected_sample += frame_ticks;
                CHECK_EQ_U64(source->ports.light[0].sample_x, 77u);
                CHECK_EQ_U64(source->ports.light[0].sample_raster, 53u);
                CHECK_EQ_U64(source->ports.light[0].sample_tick, expected_sample);
            }
            if (row->input[0] == MUHTARI_PORT_SNES2ATARI) {
                static const uint16_t scan[] = {0x0280u, 0x0281u, 0x0280u, 0x0280u, 0x0280u};
                static const uint8_t values[] = {0u, 0x30u, 0x20u, 0u, 0x10u};
                for (unsigned write = 0u; write < sizeof scan / sizeof scan[0]; write++) {
                    muhtari__bus_write(source, scan[write], values[write]);
                    muhtari__bus_write(restored, scan[write], values[write]);
                }
                CHECK_EQ_U64(source->ports.adapter[0].snapshot,
                             MUHTARI_BUTTON_1 | MUHTARI_BUTTON_2);
                CHECK_EQ_U64(source->ports.adapter[0].position, 1u);
                CHECK_EQ_U64(muhtari__ports_inpt(source, 4u), 1u);
            }
            if (row->input[1] == MUHTARI_PORT_MEGA7800) {
                muhtari__bus_write(source, 0x0281u, 0x03u);
                muhtari__bus_write(restored, 0x0281u, 0x03u);
                CHECK_EQ_U64(source->ports.adapter[1].scanning, 1u);
                CHECK_EQ_U64(source->ports.adapter[1].position, 0u);
            }
            /* Snapshot the now-active optical/pot/adapter/mixed-audio state and
               restore it again before the second resumed frame. */
            size_t snapshot_bytes = muhtari_state_size(source);
            CHECK_EQ_U64(muhtari_save_state(source, public_first, snapshot_bytes),
                         MUHTARI_STATUS_OK);
            CHECK_EQ_U64(muhtari_save_state(source, public_second, snapshot_bytes),
                         MUHTARI_STATUS_OK);
            CHECK(memcmp(public_first, public_second, snapshot_bytes) == 0);
            uint32_t source_allocs = source_counts->allocs;
            uint32_t restored_allocs = restored_counts->allocs;
            CHECK_EQ_U64(muhtari_load_state(restored, public_first, snapshot_bytes),
                         MUHTARI_STATUS_OK);
            CHECK_EQ_U64(source_counts->allocs, source_allocs);
            CHECK_EQ_U64(restored_counts->allocs, restored_allocs);
            CHECK_EQ_U64(restored->synth.pcm_count, source->synth.pcm_count);
            CHECK_EQ_U64(restored->synth.pcm_read, source->synth.pcm_read);
            CHECK_EQ_U64(restored->ports.adapter[0].position,
                         source->ports.adapter[0].position);
            CHECK_EQ_U64(restored->ports.light[0].sample_tick,
                         source->ports.light[0].sample_tick);
        }
        if (row->input[0] == MUHTARI_PORT_DRIVING ||
            row->input[0] == MUHTARI_PORT_TRAKBALL ||
            row->input[0] == MUHTARI_PORT_ST_MOUSE ||
            row->input[0] == MUHTARI_PORT_AMIGA_MOUSE) {
            CHECK_EQ_U64(source->ports.motion[0][0].residual, 0u);
            CHECK_EQ_U64(source->ports.motion[0][1].residual, 0u);
            CHECK_EQ_U64(source->ports.motion[0][0].next_tick, 0u);
            CHECK_EQ_U64(source->ports.motion[0][1].next_tick, 0u);
        }
        if (row->legacy != 0u && frame == 1u) {
            CHECK_EQ_HEX(muhtari__ports_swcha(source), 0xEFu);
            CHECK_EQ_HEX(muhtari__ports_swchb(source), 0x3Du);
        }
    }
    muhtari_counters source_counters, restored_counters;
    memset(&source_counters, 0, sizeof source_counters);
    memset(&restored_counters, 0, sizeof restored_counters);
    source_counters.size = (uint32_t)sizeof source_counters;
    restored_counters.size = (uint32_t)sizeof restored_counters;
    CHECK_EQ_U64(muhtari_get_counters(source, &source_counters), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari_get_counters(restored, &restored_counters), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(source_counters.frames, restored_counters.frames);
    CHECK_EQ_U64(source_counters.frames, 2u);
    CHECK_EQ_U64(source_counters.lag_frames, restored_counters.lag_frames);
    CHECK_EQ_U64(muhtari_save_state(source, public_first, bytes), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari_save_state(restored, public_second, bytes), MUHTARI_STATUS_OK);
    CHECK(memcmp(public_first, public_second, bytes) == 0);
}

static void test_payload(void)
{
    alloc_counts counts;
    muhtari *m = state_create(&counts);
    if (m == NULL)
        return;
    CHECK_EQ_U64(state_load(m, STATE_LINEAR, 0), MUHTARI_STATUS_OK);
    size_t bytes = muhtari__state_payload_size(m);
    CHECK_EQ_U64(bytes, expected_payload_size(m));

    /* NTSC has no raster row 300. The packed form writes its inactive
       statistics as zero while the live producer may retain old counters. */
    m->maria.stats[300].cost = 1u;
    uint32_t public_allocations = counts.allocs;
    uint32_t public_frees = counts.frees;
    CHECK(muhtari__state_payload_write(m, first, bytes));
    CHECK(muhtari__state_payload_validate(m, first, bytes));
    size_t inactive = payload_index(4826u + 300u * 8u);
    for (size_t i = 0u; i < 8u; i++)
        CHECK_EQ_U64(first[inactive + i], 0u);
    size_t inactive_pcm = payload_index(8126u);
    for (size_t i = 0u; i < sizeof m->synth.pcm; i++)
        CHECK_EQ_U64(first[inactive_pcm + i], 0u);
    CHECK(muhtari__state_payload_write(m, second, bytes));
    CHECK(memcmp(first, second, bytes) == 0);
    muhtari__state_payload_apply(m, first, bytes);
    CHECK_EQ_U64(m->maria.stats[300].cost, 0u);
    CHECK_EQ_U64(counts.allocs, public_allocations);
    CHECK_EQ_U64(counts.frees, public_frees);

    first[inactive] = 1u;
    CHECK(!muhtari__state_payload_validate(m, first, bytes));
    memcpy(&snapshot, m, sizeof snapshot);
    muhtari__state_payload_apply(m, first, bytes);
    CHECK(memcmp(&snapshot, m, sizeof snapshot) == 0);

    size_t total = muhtari_state_size(m);
    CHECK_EQ_U64(total, bytes + STATE_HEADER_BYTES);
    memset(public_first, 0x3Cu, total);
    memcpy(public_snapshot, public_first, total);
    CHECK_EQ_U64(muhtari_save_state(m, public_first, total - 1u),
                 MUHTARI_STATUS_ERR_BUFFER_TOO_SMALL);
    CHECK(memcmp(public_first, public_snapshot, total) == 0);
    muhtari_span system_before;
    memset(&system_before, 0, sizeof system_before);
    system_before.size = (uint32_t)sizeof system_before;
    CHECK_EQ_U64(muhtari_get_memory(m, MUHTARI_MEMORY_SYSTEM_RAM, &system_before),
                 MUHTARI_STATUS_OK);
    memset(public_first, 0xA5, total + 4u);
    uint32_t allocations = counts.allocs;
    uint32_t frees = counts.frees;
    CHECK_EQ_U64(muhtari_save_state(m, public_first, total + 4u), MUHTARI_STATUS_OK);
    CHECK(memcmp(public_first, "MUHTST01", 8u) == 0);
    CHECK_EQ_U64(get_u32(public_first + 8u), 1u);
    CHECK_EQ_U64(get_u32(public_first + 12u), STATE_HEADER_BYTES);
    CHECK_EQ_U64(get_u32(public_first + 16u), total);
    CHECK_EQ_U64(get_u32(public_first + 20u), 6u);
    CHECK_EQ_U64(get_u32(public_first + 24u), state_crc(public_first, total));
    CHECK(memcmp(public_first + 28u, m->state_identity, sizeof m->state_identity) == 0);
    CHECK_EQ_U64(get_u32(public_first + 60u), 0u);
    CHECK_EQ_HEX(public_first[total], 0xA5u);
    CHECK_EQ_HEX(public_first[total + 1u], 0xA5u);
    CHECK_EQ_HEX(public_first[total + 2u], 0xA5u);
    CHECK_EQ_HEX(public_first[total + 3u], 0xA5u);
    CHECK_EQ_U64(muhtari_save_state(m, public_second, total), MUHTARI_STATUS_OK);
    CHECK(memcmp(public_first, public_second, total) == 0);
    CHECK_EQ_U64(muhtari_load_state(m, public_first, total), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari_save_state(m, public_second, total), MUHTARI_STATUS_OK);
    CHECK(memcmp(public_first, public_second, total) == 0);
    muhtari_span system_after;
    memset(&system_after, 0, sizeof system_after);
    system_after.size = (uint32_t)sizeof system_after;
    CHECK_EQ_U64(muhtari_get_memory(m, MUHTARI_MEMORY_SYSTEM_RAM, &system_after),
                 MUHTARI_STATUS_OK);
    CHECK(system_after.data == system_before.data);
    CHECK_EQ_U64(system_after.bytes, system_before.bytes);
    CHECK_EQ_U64(counts.allocs, allocations);
    CHECK_EQ_U64(counts.frees, frees);

    alloc_counts peer_counts;
    muhtari *peer = state_create(&peer_counts);
    if (peer != NULL) {
        CHECK_EQ_U64(state_load(peer, STATE_LINEAR, 0), MUHTARI_STATUS_OK);
        CHECK_EQ_U64(muhtari_load_state(peer, public_first, total), MUHTARI_STATUS_OK);
        CHECK_EQ_U64(muhtari_save_state(peer, public_second, total), MUHTARI_STATUS_OK);
        CHECK(memcmp(public_first, public_second, total) == 0);

        muhtari_frame frame;
        memset(&frame, 0, sizeof frame);
        frame.size = (uint32_t)sizeof frame;
        frame.video_pitch = 320u;
        frame.video = public_video_a;
        frame.audio = public_audio_a;
        frame.audio_capacity = (uint32_t)(sizeof public_audio_a / sizeof public_audio_a[0]);
        CHECK_EQ_U64(muhtari_run_frame(m, &frame), MUHTARI_STATUS_OK);
        CHECK_EQ_U64(muhtari_save_state(m, public_first, total), MUHTARI_STATUS_OK);
        CHECK_EQ_U64(muhtari_load_state(peer, public_first, total), MUHTARI_STATUS_OK);

        memset(public_video_a, 0, sizeof public_video_a);
        memset(public_video_b, 0, sizeof public_video_b);
        memset(public_audio_a, 0, sizeof public_audio_a);
        memset(public_audio_b, 0, sizeof public_audio_b);
        frame.video = public_video_a;
        frame.audio = public_audio_a;
        CHECK_EQ_U64(muhtari_run_frame(m, &frame), MUHTARI_STATUS_OK);
        uint32_t samples = frame.audio_count;
        frame.video = public_video_b;
        frame.audio = public_audio_b;
        CHECK_EQ_U64(muhtari_run_frame(peer, &frame), MUHTARI_STATUS_OK);
        CHECK_EQ_U64(frame.audio_count, samples);
        CHECK(memcmp(public_video_a, public_video_b, sizeof public_video_a) == 0);
        CHECK(memcmp(public_audio_a, public_audio_b, (size_t)samples * sizeof public_audio_a[0]) ==
              0);
        muhtari_destroy(peer);
    }
    muhtari_destroy(m);
}

static void test_fields(void)
{
    alloc_counts counts;
    muhtari *m = state_create(&counts);
    if (m == NULL)
        return;
    CHECK_EQ_U64(state_load(m, STATE_LINEAR, 0), MUHTARI_STATUS_OK);

    muhtari_input_ex input;
    memset(&input, 0, sizeof input);
    input.size = (uint32_t)sizeof input;
    input.port[0].device = MUHTARI_PORT_DRIVING;
    input.port[0].buttons = MUHTARI_BUTTON_1;
    input.port[0].dx = -8;
    CHECK_EQ_U64(muhtari_set_input_ex(m, &input), MUHTARI_STATUS_OK);

    m->ports.motion[0][0].residual = -255;
    m->riot.timer_n = 256u;
    m->riot.timer_shift = 10u;
    m->pokey[0].carry[0] = UINT64_MAX;
    m->pokey[0].resync[1] = UINT64_MAX;
    m->pokey[0].channel[2].fire = UINT64_MAX;
    m->pokey[0].channel[3].reload = UINT64_MAX;
    m->bus.halt_at = UINT64_MAX;

    size_t bytes = muhtari__state_payload_size(m);
    CHECK(muhtari__state_payload_write(m, first, bytes));
    CHECK(muhtari__state_payload_validate(m, first, bytes));

    /* IRQEN and its three-stage propagation history preserve raw register
       bytes. Their high bits are legal even though only selected bits signal. */
    m->pokey[0].irqen = 0xFFu;
    m->pokey[0].irq_enable[0] = 0xFFu;
    int full_irq_bytes = muhtari__state_payload_write(m, first, bytes);
    CHECK(full_irq_bytes);
    if (full_irq_bytes == 0) {
        muhtari_destroy(m);
        return;
    }
    CHECK(muhtari__state_payload_validate(m, first, bytes));
    CHECK_EQ_HEX(first[payload_index(10447u)], 0xFFu);
    CHECK_EQ_HEX(first[payload_index(10454u)], 0xFFu);
    size_t residual = payload_index(7371u);
    CHECK_EQ_HEX(first[residual], 0x01u);
    CHECK_EQ_HEX(first[residual + 1u], 0xFFu);
    CHECK_EQ_HEX(first[residual + 2u], 0xFFu);
    CHECK_EQ_HEX(first[residual + 3u], 0xFFu);
    CHECK_EQ_U64(get_u64(first + payload_index(10400u)), UINT64_MAX);
    CHECK_EQ_U64(get_u64(first + payload_index(10416u + 8u)), UINT64_MAX);
    CHECK_EQ_U64(get_u64(first + payload_index(10466u)), UINT64_MAX);
    CHECK_EQ_U64(get_u64(first + payload_index(10563u)), UINT64_MAX);

    expect_invalid_u8(m, bytes, 233u, 11u);       /* RIOT uses 1, 8, 64, 1024. */
    expect_invalid_u8(m, bytes, 219u, 2u);        /* Bus cycles are 4 or 6 ticks. */
    expect_invalid_u8(m, bytes, 115u, 2u);        /* CPU jammed is a boolean. */
    expect_invalid_u8(m, bytes, 116u, 2u);        /* CPU IRQ sample is a boolean. */
    expect_invalid_u8(m, bytes, 214u, 0x10u);     /* INPTCTRL has four defined bits. */
    expect_invalid_u8(m, bytes, 215u, 3u);       /* HALT gate count has three states. */
    expect_invalid_u8(m, bytes, 218u, 2u);       /* Bus NMI pending is a boolean. */
    expect_invalid_u8(m, bytes, 221u, 8u);       /* Bus IRQ pipe has three stages. */
    expect_invalid_u8(m, bytes, 222u, 2u);       /* Bus IRQ level is a boolean. */
    expect_invalid_u8(m, bytes, 234u, 2u);       /* RIOT timer clear is a boolean. */
    expect_invalid_u8(m, bytes, 239u, 2u);       /* RIOT PA7 flag is a boolean. */
    expect_invalid_u8(m, bytes, 240u, 2u);       /* RIOT PA7 polarity is a boolean. */
    expect_invalid_u8(m, bytes, 241u, 2u);       /* RIOT PA7 detect is a boolean. */
    expect_invalid_u8(m, bytes, 4504u, 2u);      /* MARIA write mode is a boolean. */
    expect_invalid_u8(m, bytes, 4505u, 2u);      /* MARIA write buffer has two slots. */
    expect_invalid_u16(m, bytes, 4826u, 455u);   /* MARIA line cost is at most 454 ticks. */
    expect_invalid_u32(m, bytes, 7330u, UINT32_MAX); /* MARIA feature mask has reserved bits. */
    expect_invalid_u16(m, bytes, 7350u, 313u);  /* MARIA line index is region-bounded. */
    expect_invalid_u16(m, bytes, 7352u, 313u);  /* MARIA DMA line is region-bounded. */
    expect_invalid_u8(m, bytes, 7355u, 1u);      /* MARIA status only drives VBLANK. */
    expect_invalid_u16(m, bytes, 7356u, 321u);  /* MARIA pixel cursor cannot exceed width. */
    expect_invalid_u8(m, bytes, 7358u, 1u);      /* Linear carts have no bank register. */
    expect_invalid_u8(m, bytes, 7359u, 1u);      /* Linear carts have no RAM bank. */
    expect_invalid_u8(m, bytes, 7360u, 1u);      /* No expansion latch when absent. */
    expect_invalid_u8(m, bytes, 7361u, 1u);      /* No XM page selector when absent. */
    expect_invalid_u8(m, bytes, 7375u, 4u);      /* Motion phase has four positions. */
    expect_invalid_u8(m, bytes, 7376u, 2u);      /* Motion direction is a boolean. */
    expect_invalid_u16(m, bytes, 7451u, 313u);  /* Light sensor raster is bounded. */
    expect_invalid_u16(m, bytes, 7453u, 320u);  /* Light sensor x is within the frame. */
    expect_invalid_u8(m, bytes, 7459u, 2u);      /* Light sensor low state is a boolean. */
    expect_invalid_u64(m, bytes, 7501u, (UINT64_C(1) << 32) + 1u);
    expect_invalid_u8(m, bytes, 7617u, 4u);      /* Adapter host has two pins. */
    expect_invalid_u8(m, bytes, 7618u, 17u);     /* Adapter position is at most 16. */
    expect_invalid_u8(m, bytes, 7619u, 2u);      /* Adapter scan state is a boolean. */
    expect_invalid_u16(m, bytes, 7627u, 0x1000u); /* Keypad keys are twelve bits. */
    expect_invalid_u8(m, bytes, 7631u, 0x80u);   /* Keypad low state is seven bits. */
    expect_invalid_u32(m, bytes, 7633u, UINT32_MAX); /* Unknown active device. */
    expect_invalid_u32(m, bytes, 7641u, 0x1000u); /* Legacy buttons are twelve bits. */
    expect_invalid_u32(m, bytes, 7653u, UINT32_MAX); /* Unknown pending legacy device. */
    expect_invalid_u8(m, bytes, 10436u, 16u);    /* POKEY poly4 is four bits. */
    expect_invalid_u32(m, bytes, 10432u, 0x20000u); /* POKEY poly17 is seventeen bits. */
    expect_invalid_u8(m, bytes, 10437u, 32u);    /* POKEY poly5 is five bits. */
    expect_invalid_u16(m, bytes, 10443u, 115u);  /* POKEY /114 prescaler. */
    expect_invalid_u8(m, bytes, 10449u, 2u);     /* POKEY initialized is a boolean. */
    expect_invalid_u8(m, bytes, 7354u, 4u);      /* MARIA has four line events. */
    expect_invalid_u32(m, bytes, 7673u, UINT32_MAX); /* Unknown pending device. */
    expect_invalid_u32(m, bytes, 7677u, MUHTARI_BUTTON_UP); /* Driving supports fire only. */
    expect_invalid_u32(m, bytes, 7681u, 1u);      /* Driving has no paddle values. */
    expect_invalid_u32(m, bytes, 7689u, 1u);      /* Driving has no keypad values. */
    expect_invalid_u32(m, bytes, 7697u, 1u);     /* Driving has no Y delta. */
    expect_invalid_u32(m, bytes, 7701u, 1u);     /* Driving has no gun coordinates. */
    expect_invalid_u32(m, bytes, 7709u, 2u);     /* Extended input flags are device-bound. */
    expect_invalid_u32(m, bytes, 7753u, 0x20u);  /* Reserved switch bit. */
    expect_invalid_u16(m, bytes, 231u, 257u);    /* RIOT interval is at most 256. */
    expect_invalid_u16(m, bytes, 10348u, 456u);  /* TIA line phase is 0..455. */
    expect_invalid_u16(m, bytes, 10441u, 29u);   /* POKEY /28 prescaler. */
    expect_invalid_u8(m, bytes, 10356u, 16u);    /* TIA AUDC is four bits. */
    expect_invalid_u8(m, bytes, 10364u, 2u);     /* TIA noise input is boolean. */
    expect_invalid_u8(m, bytes, 10450u, 4u);     /* High-pass delay is at most three. */
    expect_invalid_u8(m, bytes, 8122u, 16u);     /* Synth ring head selects 16 cells. */
    expect_invalid_u16(m, bytes, 8102u, 16u);    /* TIA synth source is 0..15. */
    expect_invalid_u16(m, bytes, 8106u, 207u);   /* POKEY synth source is 0..206. */
    expect_invalid_u8(m, bytes, 8125u, 1u);      /* No secondary chip without one POKEY. */
    expect_invalid_u8(m, bytes, 7861u, 2u);      /* Boot rejection flag is a boolean. */
    expect_invalid_u64(m, bytes, 7894u, UINT64_C(320961867552001));
    expect_invalid_u64(m, bytes, 8022u, UINT64_C(274877906945));
    expect_invalid_u64(m, bytes, 8030u, UINT64_C(35184372088833));
    expect_invalid_u64(m, bytes, 8038u, UINT64_C(140737488355329));
    expect_invalid_u32(m, bytes, 8062u, m->synth.sample_den);
    expect_invalid_u32(m, bytes, 8066u, MUHTARI__SYNTH_PCM_CAPACITY);
    expect_invalid_u32(m, bytes, 8070u, MUHTARI__SYNTH_PCM_CAPACITY + 1u);
    expect_invalid_u32(m, bytes, 8074u, 1u);      /* Sample clock is identity-bound. */
    expect_invalid_u32(m, bytes, 8098u, 18361u);  /* Mixed level is within DAC scale. */
    expect_invalid_u8(m, bytes, 8123u, 2u);      /* Synth pending is a boolean. */
    expect_invalid_u8(m, bytes, 8124u, 3u);      /* At most two installed POKEYs. */
    expect_invalid_u8(m, bytes, 7841u, 2u);      /* Input mode is a boolean. */
    expect_invalid_u8(m, bytes, 7842u, 2u);      /* Port timing state is initialized or not. */
    expect_invalid_u8(m, bytes, 7843u, 2u);      /* Pending-input state is a boolean. */
    expect_invalid_u8(m, bytes, 7844u, 2u);      /* Input-read state is a boolean. */
    expect_invalid_u8(m, bytes, 10354u, 2u);     /* TIA input latch is a boolean. */
    expect_invalid_u8(m, bytes, 10357u, 32u);    /* TIA AUDF is five bits. */
    expect_invalid_u8(m, bytes, 10358u, 16u);    /* TIA AUDV is four bits. */
    expect_invalid_u8(m, bytes, 10359u, 32u);    /* TIA divider is five bits. */
    expect_invalid_u8(m, bytes, 10360u, 2u);     /* TIA enabled is a boolean. */
    expect_invalid_u8(m, bytes, 10361u, 2u);     /* TIA output is a boolean. */
    expect_invalid_u8(m, bytes, 10362u, 32u);    /* TIA noise state is five bits. */
    expect_invalid_u8(m, bytes, 10363u, 16u);    /* TIA pulse state is four bits. */
    expect_invalid_u8(m, bytes, 10365u, 2u);     /* TIA pulse gate is a boolean. */
    expect_invalid_u8(m, bytes, 10452u, 2u);    /* POKEY high-pass capture is a boolean. */
    expect_invalid_u8(m, bytes, 10457u, 2u);    /* POKEY IRQ output is a boolean. */
    expect_invalid_u16(m, bytes, 10486u, 256u); /* POKEY channel counter is eight bits. */
    expect_invalid_u8(m, bytes, 10490u, 2u);    /* POKEY output is a boolean. */
    expect_invalid_u8(m, bytes, 10491u, 2u);    /* POKEY high-pass state is a boolean. */
    expect_invalid_u8(m, bytes, 10492u, 2u);    /* POKEY start state is a boolean. */

    size_t deadline = payload_index(76u);
    expect_invalid_u64(m, bytes, 68u, get_u64(first + deadline) + 1u);

    /* The accepted clock relationships use inclusive u64 boundaries and
       do not impose a global tick ceiling. */
    memcpy(second, first, bytes);
    static const size_t near_max_fields[] = {68u,  76u,  100u, 117u, 125u, 133u,
                                             149u, 7334u, 7862u, 7870u, 7878u, 7886u};
    for (size_t i = 0u; i < sizeof near_max_fields / sizeof near_max_fields[0]; i++)
        put_u64(second + payload_index(near_max_fields[i]), UINT64_MAX);
    CHECK(muhtari__state_payload_validate(m, second, bytes));

    memcpy(&snapshot, m, sizeof snapshot);
    m->ports.pending_ex.port[0].dx = 100;
    m->ports.pending_ex.size = 0u;
    muhtari__state_payload_apply(m, first, bytes);
    CHECK(m->ports.pending_ex.port[0].dx == -8);
    CHECK_EQ_U64(m->ports.pending_ex.size, sizeof m->ports.pending_ex);
    CHECK_EQ_U64(m->bus.halt_at, UINT64_MAX);
    CHECK_EQ_U64(m->pokey[0].carry[0], UINT64_MAX);

    size_t total = muhtari_state_size(m);
    CHECK_EQ_U64(total, bytes + STATE_HEADER_BYTES);
    CHECK_EQ_U64(muhtari_save_state(m, public_snapshot, total), MUHTARI_STATUS_OK);

    memset(public_first, 0xC3, total);
    memcpy(public_second, public_first, total);
    uint8_t jammed = m->cpu.jammed;
    m->cpu.jammed = 2u;
    CHECK_EQ_U64(muhtari_save_state(m, public_first, total), MUHTARI_STATUS_ERR_INVALID_STATE);
    CHECK(memcmp(public_first, public_second, total) == 0);
    m->cpu.jammed = jammed;

    CHECK_EQ_U64(muhtari_load_state(m, public_snapshot, 1u), MUHTARI_STATUS_ERR_INVALID_STATE);
    CHECK_EQ_U64(muhtari_load_state(m, public_snapshot, STATE_HEADER_BYTES),
                 MUHTARI_STATUS_ERR_INVALID_STATE);
    CHECK_EQ_U64(muhtari_load_state(m, public_snapshot, SIZE_MAX), MUHTARI_STATUS_ERR_INVALID_STATE);

    memcpy(public_first, public_snapshot, total);
    public_first[0] ^= 1u;
    expect_state_refusal(m, public_first, total, MUHTARI_STATUS_ERR_INVALID_STATE);

    memcpy(public_first, public_snapshot, total);
    put_u32(public_first + 12u, STATE_HEADER_BYTES - 1u);
    state_rechecksum(public_first, total);
    expect_state_refusal(m, public_first, total, MUHTARI_STATUS_ERR_INVALID_STATE);

    memcpy(public_first, public_snapshot, total);
    put_u32(public_first + 8u, 2u);
    state_rechecksum(public_first, total);
    expect_state_refusal(m, public_first, total, MUHTARI_STATUS_ERR_UNSUPPORTED_STATE);

    memcpy(public_first, public_snapshot, total);
    put_u32(public_first + 20u, 7u);
    state_rechecksum(public_first, total);
    expect_state_refusal(m, public_first, total, MUHTARI_STATUS_ERR_UNSUPPORTED_STATE);

    memcpy(public_first, public_snapshot, total);
    put_u32(public_first + 16u, (uint32_t)(total + 1u));
    state_rechecksum(public_first, total);
    expect_state_refusal(m, public_first, total, MUHTARI_STATUS_ERR_INVALID_STATE);

    memcpy(public_first, public_snapshot, total);
    put_u32(public_first + 60u, 1u);
    state_rechecksum(public_first, total);
    expect_state_refusal(m, public_first, total, MUHTARI_STATUS_ERR_INVALID_STATE);

    memcpy(public_first, public_snapshot, total);
    public_first[40] ^= 1u;
    expect_state_refusal(m, public_first, total, MUHTARI_STATUS_ERR_INVALID_STATE);

    memcpy(public_first, public_snapshot, total);
    public_first[28] ^= 1u;
    state_rechecksum(public_first, total);
    expect_state_refusal(m, public_first, total, MUHTARI_STATUS_ERR_STATE_MISMATCH);

    memcpy(public_first, public_snapshot, total);
    public_first[115] = 2u; /* A checksum-valid non-boolean CPU field remains untrusted. */
    state_rechecksum(public_first, total);
    expect_state_refusal(m, public_first, total, MUHTARI_STATUS_ERR_INVALID_STATE);
    expect_state_refusal(m, public_snapshot, total - 1u, MUHTARI_STATUS_ERR_INVALID_STATE);
    public_snapshot[total] = 0u;
    expect_state_refusal(m, public_snapshot, total + 1u, MUHTARI_STATUS_ERR_INVALID_STATE);

    /* The private payload keeps unrestricted u64 values, while public restore
       refuses a checksummed clock tuple that would make audio catch-up grow
       without bound. Other clocks stay mutually ordered and in this frame. */
    memcpy(public_first, public_snapshot, total);
    const uint64_t far_tick = UINT64_C(1) << 40;
    put_u64(public_first + 68u, far_tick);
    put_u64(public_first + 76u,
            far_tick + muhtari__timing(m).frame_ticks);
    put_u64(public_first + 117u, far_tick);
    put_u64(public_first + 149u, far_tick);
    put_u64(public_first + 7334u, far_tick);
    put_u64(public_first + 7862u, 0u);
    put_u64(public_first + 10332u, far_tick + 64u);
    put_u64(public_first + 10340u, far_tick);
    state_rechecksum(public_first, total);
    expect_state_refusal(m, public_first, total, MUHTARI_STATUS_ERR_INVALID_STATE);

    /* A stale TIA clock is independently bounded even when synthesis is
       aligned with the restored bus clock. */
    memcpy(public_first, public_snapshot, total);
    put_u64(public_first + 68u, far_tick);
    put_u64(public_first + 76u, far_tick + muhtari__timing(m).frame_ticks);
    put_u64(public_first + 117u, far_tick);
    put_u64(public_first + 149u, far_tick);
    put_u64(public_first + 7334u, far_tick);
    put_u64(public_first + 7862u, far_tick);
    put_u64(public_first + 7870u, far_tick);
    put_u64(public_first + 7878u, far_tick);
    put_u64(public_first + 10332u, 0u);
    put_u64(public_first + 10340u, far_tick);
    state_rechecksum(public_first, total);
    expect_state_refusal(m, public_first, total, MUHTARI_STATUS_ERR_INVALID_STATE);

    /* Absolute time itself has no format ceiling, but public restore refuses
       a tuple without enough producer-derived room for the next calls. */
    memcpy(public_first, public_snapshot, total);
    uint64_t margin = 2u * muhtari__timing(m).frame_ticks +
                      MUHTARI__SYNTH_OVERSHOOT_TICKS + UINT32_MAX;
    uint64_t near_limit = UINT64_MAX - margin + 1u;
    put_u64(public_first + 68u, near_limit);
    put_u64(public_first + 76u, near_limit + muhtari__timing(m).frame_ticks);
    put_u64(public_first + 117u, near_limit);
    put_u64(public_first + 149u, near_limit);
    put_u64(public_first + 7334u, near_limit);
    put_u64(public_first + 7862u, near_limit);
    put_u64(public_first + 7870u, near_limit);
    put_u64(public_first + 7878u, near_limit);
    put_u64(public_first + 10332u, near_limit + 64u);
    put_u64(public_first + 10340u, near_limit);
    state_rechecksum(public_first, total);
    expect_state_refusal(m, public_first, total, MUHTARI_STATUS_ERR_INVALID_STATE);

    /* A pending HALT with a hostile far-future release would keep frame
       execution inside the hold loop; the finite deadline is source-bounded. */
    memcpy(public_first, public_snapshot, total);
    public_first[216u] = 1u;
    put_u64(public_first + 165u, UINT64_MAX);
    state_rechecksum(public_first, total);
    expect_state_refusal(m, public_first, total, MUHTARI_STATUS_ERR_INVALID_STATE);

    CHECK_EQ_U64(muhtari_save_state(m, public_second, total), MUHTARI_STATUS_OK);
    CHECK(memcmp(public_second, public_snapshot, total) == 0);
    muhtari_destroy(m);
}

static void test_aliases(void)
{
    alloc_counts counts;
    muhtari *m = state_create(&counts);
    if (m == NULL)
        return;
    CHECK_EQ_U64(state_load(m, STATE_XBOARD, 0), MUHTARI_STATUS_OK);
    size_t bytes = muhtari__state_payload_size(m);
    CHECK(muhtari__state_payload_write(m, first, bytes));
    memcpy(&snapshot, m, sizeof snapshot);

    CHECK(!muhtari__state_payload_write(m, (uint8_t *)m, bytes));
    CHECK(!muhtari__state_payload_validate(m, (const uint8_t *)m, bytes));
    muhtari__state_payload_apply(m, (const uint8_t *)m, bytes);
    CHECK(memcmp(&snapshot, m, sizeof snapshot) == 0);

    CHECK(!muhtari__state_payload_write(m, m->cart.rom, bytes));
    CHECK(!muhtari__state_payload_validate(m, m->cart.rom, bytes));
    muhtari__state_payload_apply(m, m->cart.rom, bytes);
    CHECK(memcmp(&snapshot, m, sizeof snapshot) == 0);

    size_t total = muhtari_state_size(m);
    CHECK_EQ_U64(muhtari_save_state(m, public_snapshot, total), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari_save_state(m, m, total), MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK_EQ_U64(muhtari_save_state(m, m->cart.rom, total), MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK_EQ_U64(muhtari_load_state(m, m, total), MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK_EQ_U64(muhtari_load_state(m, m->cart.rom, total), MUHTARI_STATUS_ERR_ARGUMENT);
    uintptr_t wrapped_address = UINTPTR_MAX - (uintptr_t)total + 1u;
    CHECK_EQ_U64(muhtari_save_state(m, (void *)wrapped_address, total),
                 MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK_EQ_U64(muhtari_load_state(m, (const void *)wrapped_address, total),
                 MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK_EQ_U64(muhtari_save_state(m, public_second, total), MUHTARI_STATUS_OK);
    CHECK(memcmp(public_snapshot, public_second, total) == 0);
    muhtari_destroy(m);

    for (unsigned mode = BOUNDARY_AFTER_INSTANCE; mode <= BOUNDARY_AFTER_CART; mode++) {
        memset(&boundary, 0, sizeof boundary);
        boundary.mode = (enum boundary_mode)mode;
        uintptr_t align = (uintptr_t)_Alignof(max_align_t);
        if (boundary.mode == BOUNDARY_AFTER_INSTANCE) {
            uintptr_t candidate = (uintptr_t)boundary.storage + STATE_MAX_BYTES;
            uintptr_t instance = (candidate + align - 1u) & ~(align - 1u);
            boundary.instance = (muhtari *)instance;
            uintptr_t cart = instance + sizeof(muhtari) + STATE_MAX_BYTES;
            cart = (cart + align - 1u) & ~(align - 1u);
            boundary.cart = (uint8_t *)cart;
        } else {
            boundary.cart = boundary.storage;
        }

        muhtari_config config;
        memset(&config, 0, sizeof config);
        config.size = (uint32_t)sizeof config;
        config.allocator.alloc = boundary_alloc;
        config.allocator.free = boundary_free;
        config.allocator.user = &boundary;
        muhtari *adjacent = NULL;
        CHECK_EQ_U64(muhtari_create(&config, &adjacent), MUHTARI_STATUS_OK);
        if (adjacent != NULL) {
            CHECK_EQ_U64(state_load(adjacent, STATE_LINEAR, 0), MUHTARI_STATUS_OK);
            size_t adjacent_size = muhtari_state_size(adjacent);
            uint8_t *adjacent_bytes = boundary.mode == BOUNDARY_AFTER_INSTANCE
                                          ? (uint8_t *)adjacent + sizeof *adjacent
                                          : adjacent->cart.rom + adjacent->cart.allocation_bytes;
            CHECK_EQ_U64(muhtari_save_state(adjacent, adjacent_bytes, adjacent_size),
                         MUHTARI_STATUS_OK);
            CHECK_EQ_U64(muhtari_load_state(adjacent, adjacent_bytes, adjacent_size),
                         MUHTARI_STATUS_OK);
            muhtari_destroy(adjacent);
        }
    }
}

static void test_lifecycle(void)
{
    alloc_counts counts;
    muhtari *m = state_create(&counts);
    if (m == NULL)
        return;
    CHECK_EQ_U64(muhtari__state_payload_size(m), 0u);
    CHECK_EQ_U64(muhtari_state_size(NULL), 0u);
    CHECK_EQ_U64(muhtari_state_size(m), 0u);
    memset(public_first, 0x5Au, sizeof public_first);
    CHECK_EQ_U64(muhtari_save_state(NULL, public_first, 0u), MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK_EQ_U64(muhtari_save_state(m, NULL, 0u), MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK_EQ_U64(muhtari_load_state(NULL, public_first, 0u), MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK_EQ_U64(muhtari_load_state(m, NULL, 0u), MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK_EQ_U64(muhtari_save_state(m, public_first, sizeof public_first),
                 MUHTARI_STATUS_ERR_STATE_UNAVAILABLE);
    CHECK_EQ_U64(muhtari_load_state(m, public_first, 0u), MUHTARI_STATUS_ERR_STATE_UNAVAILABLE);
    CHECK_EQ_HEX(public_first[0], 0x5Au);
    uint8_t empty_identity[sizeof m->state_identity] = {0};
    CHECK(memcmp(m->state_identity, empty_identity, sizeof empty_identity) == 0);
    muhtari_media refused;
    memset(&refused, 0, sizeof refused);
    refused.size = (uint32_t)sizeof refused;
    refused.image = invalid_image;
    refused.image_size = sizeof invalid_image;
    CHECK(muhtari_load(m, &refused) != MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari__state_payload_size(m), 0u);
    CHECK_EQ_U64(muhtari_state_size(m), 0u);
    CHECK_EQ_U64(muhtari_save_state(m, public_first, sizeof public_first),
                 MUHTARI_STATUS_ERR_STATE_UNAVAILABLE);
    CHECK(memcmp(m->state_identity, empty_identity, sizeof empty_identity) == 0);
    CHECK_EQ_U64(state_load(m, STATE_LINEAR, 0), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari_state_size(m), muhtari__state_payload_size(m) + STATE_HEADER_BYTES);
    size_t linear_state_bytes = muhtari_state_size(m);
    CHECK_EQ_U64(muhtari_save_state(m, public_snapshot, linear_state_bytes), MUHTARI_STATUS_OK);
    alloc_counts peer_counts;
    muhtari *peer = state_create(&peer_counts);
    if (peer != NULL) {
        size_t other_image_bytes = state_make_identity_image(identity_other, "Other state", 3u, 0u);
        identity_other[IMAGES_HEADER + 37u] ^= 1u;
        muhtari_media other_media = state_media_for_image(identity_other, other_image_bytes);
        CHECK_EQ_U64(muhtari_load(peer, &other_media), MUHTARI_STATUS_OK);
        CHECK_EQ_U64(muhtari_state_size(peer), linear_state_bytes);
        CHECK_EQ_U64(muhtari_save_state(peer, public_first, linear_state_bytes),
                     MUHTARI_STATUS_OK);
        CHECK_EQ_U64(muhtari_load_state(peer, public_snapshot, linear_state_bytes),
                     MUHTARI_STATUS_ERR_STATE_MISMATCH);
        CHECK_EQ_U64(muhtari_save_state(peer, public_second, linear_state_bytes),
                     MUHTARI_STATUS_OK);
        CHECK(memcmp(public_first, public_second, linear_state_bytes) == 0);
        muhtari_destroy(peer);
    }
    uint8_t identity[sizeof m->state_identity];
    memcpy(identity, m->state_identity, sizeof identity);
    CHECK(memcmp(identity, empty_identity, sizeof identity) != 0);
    size_t first_size = muhtari__state_payload_size(m);
    CHECK_EQ_U64(first_size, expected_payload_size(m));
    CHECK(muhtari__state_payload_write(m, first, first_size));

    /* Once copied, caller storage is no longer an identity source. Repeated
       state queries use the same cache and retain the no-allocation contract. */
    uint8_t saved_rom_byte = image[IMAGES_HEADER + 17u];
    uint32_t allocations = counts.allocs;
    uint32_t frees = counts.frees;
    image[IMAGES_HEADER + 17u] ^= 0xFFu;
    CHECK(memcmp(identity, m->state_identity, sizeof identity) == 0);
    CHECK_EQ_U64(muhtari__state_payload_size(m), first_size);
    CHECK(muhtari__state_payload_write(m, second, first_size));
    CHECK(muhtari__state_payload_validate(m, second, first_size));
    CHECK(memcmp(identity, m->state_identity, sizeof identity) == 0);
    CHECK_EQ_U64(counts.allocs, allocations);
    CHECK_EQ_U64(counts.frees, frees);
    image[IMAGES_HEADER + 17u] = saved_rom_byte;

    memcpy(&snapshot, m, sizeof snapshot);
    muhtari_media invalid;
    memset(&invalid, 0, sizeof invalid);
    invalid.size = (uint32_t)sizeof invalid;
    invalid.image = invalid_image;
    invalid.image_size = sizeof invalid_image;
    CHECK(muhtari_load(m, &invalid) != MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari__state_payload_size(m), first_size);
    CHECK(memcmp(&snapshot, m, sizeof snapshot) == 0);

    invalid.image = image;
    invalid.image_size = images_linear(image, 4u, 4096u);
    invalid.bios = invalid_image;
    invalid.bios_size = sizeof invalid_image;
    CHECK_EQ_U64(muhtari_load(m, &invalid), MUHTARI_STATUS_ERR_INVALID_BIOS);
    CHECK(memcmp(&snapshot, m, sizeof snapshot) == 0);

    CHECK_EQ_U64(state_load(m, STATE_XM, 1), MUHTARI_STATUS_OK);
    CHECK(memcmp(identity, m->state_identity, sizeof identity) != 0);
    size_t replacement_size = muhtari__state_payload_size(m);
    CHECK_EQ_U64(replacement_size, expected_payload_size(m));
    CHECK(replacement_size > first_size);
    CHECK(!muhtari__state_payload_validate(m, first, first_size));
    CHECK(muhtari__state_payload_write(m, second, replacement_size));
    CHECK(muhtari__state_payload_validate(m, second, replacement_size));
    memcpy(identity, m->state_identity, sizeof identity);
    CHECK_EQ_U64(muhtari_power_cycle(m), MUHTARI_STATUS_OK);
    CHECK(memcmp(identity, m->state_identity, sizeof identity) == 0);
    muhtari_destroy(m);
}

static void test_battery(void)
{
    alloc_counts counts;
    muhtari *m = state_create(&counts);
    if (m == NULL)
        return;
    CHECK_EQ_U64(state_load(m, STATE_XM, 1), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(m->battery.bytes, 34816u);
    m->battery.data[0] = 0x5Au;
    m->battery.data[m->battery.bytes - 1u] = 0xA5u;
    m->battery.generation = 7u;
    size_t bytes = muhtari__state_payload_size(m);
    CHECK_EQ_U64(bytes, STATE_FIXED_BYTES + STATE_EXPANSION_BYTES + m->battery.bytes);
    CHECK(muhtari__state_payload_write(m, first, bytes));
    size_t battery = STATE_FIXED_BYTES + STATE_EXPANSION_BYTES;
    CHECK_EQ_HEX(first[battery], 0x5Au);
    CHECK_EQ_HEX(first[battery + m->battery.bytes - 1u], 0xA5u);
    CHECK(muhtari__state_payload_validate(m, first, bytes));
    expect_invalid_u16(m, bytes, 10828u, 0x8000u); /* SaveKey address is fifteen bits. */
    expect_invalid_u16(m, bytes, 10830u, 0x7FC1u); /* Page base is aligned within 32 KiB. */
    expect_invalid_u8(m, bytes, 10832u, 2u);      /* Host SCL is a boolean. */
    expect_invalid_u8(m, bytes, 10833u, 2u);      /* Host SDA is a boolean. */
    expect_invalid_u8(m, bytes, 10834u, 2u);      /* Driven SDA is a boolean. */
    expect_invalid_u8(m, bytes, 10835u, 12u);     /* EEPROM phase has twelve states. */
    expect_invalid_u8(m, bytes, 10836u, 12u);     /* Next EEPROM phase has twelve states. */
    expect_invalid_u8(m, bytes, 10837u, 9u);      /* I2C byte transfer has eight bits. */
    expect_invalid_u8(m, bytes, 10839u, 0x80u);   /* SaveKey address-high is seven bits. */
    expect_invalid_u8(m, bytes, 10905u, 2u);      /* Touched-page flags are booleans. */
    expect_invalid_u8(m, bytes, 10969u, 64u);     /* EEPROM page offset selects 64 bytes. */
    expect_invalid_u8(m, bytes, 10970u, 2u);      /* Pending EEPROM write is a boolean. */
    expect_invalid_u8(m, bytes, 10971u, 2u);      /* EEPROM acknowledge is a boolean. */

    m->battery.data[0] = 0u;
    m->battery.data[m->battery.bytes - 1u] = 0u;
    muhtari__state_payload_apply(m, first, bytes);
    CHECK_EQ_HEX(m->battery.data[0], 0x5Au);
    CHECK_EQ_HEX(m->battery.data[m->battery.bytes - 1u], 0xA5u);
    CHECK_EQ_U64(m->battery.generation, 8u);
    muhtari__state_payload_apply(m, first, bytes);
    CHECK_EQ_U64(m->battery.generation, 8u);
    muhtari_destroy(m);

    alloc_counts source_counts;
    alloc_counts peer_counts;
    muhtari *source = state_create(&source_counts);
    muhtari *peer = state_create(&peer_counts);
    if (source == NULL || peer == NULL) {
        muhtari_destroy(source);
        muhtari_destroy(peer);
        return;
    }

    uint32_t source_observations = 0u;
    uint32_t peer_observations = 0u;
    CHECK_EQ_U64(muhtari__synth_observe(source, count_state_observation, &source_observations),
                 MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari__synth_observe(peer, count_state_observation, &peer_observations),
                 MUHTARI_STATUS_OK);
    CHECK_EQ_U64(state_load_hsc_savekey_program(source, 0x5Au, 1), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(state_load_hsc_savekey_program(peer, 0x5Au, 0), MUHTARI_STATUS_OK);
    CHECK(memcmp(source->state_identity, peer->state_identity, sizeof source->state_identity) == 0);

    uint8_t *source_battery = source->battery.data;
    uint8_t *peer_battery = peer->battery.data;
    uint8_t *peer_expansion = peer->cart.expansion.ram;
    uint8_t *peer_rom = peer->cart.rom;
    struct muhtari__observation *peer_observation = peer->observation;
    CHECK(source_battery != peer_battery);
    CHECK(source->cart.expansion.ram != peer_expansion);
    CHECK_EQ_U64(source->battery.eeprom_port, 2u);
    CHECK_EQ_U64(source->battery.data[0], 0xC1u);

    uint32_t source_calls = source_observations;
    size_t total = muhtari_state_size(source);
    CHECK_EQ_U64(source_observations, source_calls);
    size_t battery_at = public_battery_offset(source);
    CHECK(total >= battery_at + source->battery.bytes);
    CHECK_EQ_U64(muhtari_save_state(source, public_snapshot, total), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(source_observations, source_calls);
    struct muhtari__savekey pending_snapshot = source->savekey;

    static const uint8_t file_marker[] = "host owns battery persistence";
    FILE *host_file = tmpfile();
    CHECK(host_file != NULL);
    long host_position = -1;
    if (host_file != NULL) {
        CHECK_EQ_U64(fwrite(file_marker, 1u, sizeof file_marker, host_file), sizeof file_marker);
        CHECK_EQ_U64(fflush(host_file), 0u);
        host_position = ftell(host_file);
    }

    peer->battery.generation = UINT64_MAX;
    source_calls = source_observations;
    uint32_t peer_calls = peer_observations;
    CHECK_EQ_U64(muhtari_load_state(peer, public_snapshot, total), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(source_observations, source_calls);
    CHECK_EQ_U64(peer_observations, peer_calls);
    CHECK(peer->observer == count_state_observation);
    CHECK(peer->observer_user == &peer_observations);
    CHECK(peer->observation == peer_observation);
    CHECK(peer->battery.data == peer_battery);
    CHECK(peer->cart.expansion.ram == peer_expansion);
    CHECK(peer->cart.rom == peer_rom);
    CHECK_EQ_U64(peer->battery.generation, UINT64_MAX);
    CHECK_EQ_HEX(peer->battery.data[0], 0xC1u);
    CHECK_EQ_HEX(peer->battery.data[2051u], 0xFFu);
    CHECK(memcmp(&peer->savekey, &pending_snapshot, sizeof pending_snapshot) == 0);
    CHECK_EQ_U64(peer->riot.ora, source->riot.ora);
    CHECK_EQ_U64(peer->riot.ddra, source->riot.ddra);

    /* The private observer seam only checks state-call isolation. Branch and
       replay execution below run without any host callback installed. */
    CHECK_EQ_U64(muhtari__synth_observe(source, NULL, NULL), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari__synth_observe(peer, NULL, NULL), MUHTARI_STATUS_OK);

    uint32_t replay_audio_counts[3] = {0u, 0u, 0u};
    uint32_t source_audio_count =
        state_run_frame(source, public_video_a, public_audio_a);
    replay_audio_counts[0] = source_audio_count;
    uint32_t peer_audio_count = state_run_frame(peer, public_video_b, public_audio_b);
    CHECK_EQ_U64(peer_audio_count, source_audio_count);
    size_t video_bytes = (size_t)320u * muhtari__timing(source).height;
    CHECK(memcmp(public_video_a, public_video_b, video_bytes) == 0);
    CHECK(memcmp(public_audio_a, public_audio_b,
                 (size_t)source_audio_count * sizeof public_audio_a[0]) == 0);
    memcpy(public_video_reference[0], public_video_a, video_bytes);
    memcpy(public_audio_reference[0], public_audio_a,
           (size_t)source_audio_count * sizeof public_audio_a[0]);

    unsigned replay_frames = 1u;
    while ((source->savekey.pending != 0u || peer->savekey.pending != 0u) &&
           replay_frames < 3u) {
        source_audio_count = state_run_frame(source, public_video_a, public_audio_a);
        replay_audio_counts[replay_frames] = source_audio_count;
        peer_audio_count = state_run_frame(peer, public_video_b, public_audio_b);
        CHECK_EQ_U64(peer_audio_count, source_audio_count);
        CHECK(memcmp(public_video_a, public_video_b, video_bytes) == 0);
        CHECK(memcmp(public_audio_a, public_audio_b,
                     (size_t)source_audio_count * sizeof public_audio_a[0]) == 0);
        memcpy(public_video_reference[replay_frames], public_video_a, video_bytes);
        memcpy(public_audio_reference[replay_frames], public_audio_a,
               (size_t)source_audio_count * sizeof public_audio_a[0]);
        replay_frames++;
    }
    CHECK(replay_frames <= 3u);
    CHECK_EQ_HEX(source->battery.data[2051u], 0x5Au);
    CHECK_EQ_HEX(peer->battery.data[2051u], 0x5Au);
    CHECK_EQ_U64(source->battery.generation, 1u);
    CHECK_EQ_U64(peer->battery.generation, 0u);
    CHECK_EQ_U64(source->savekey.pending, 0u);
    CHECK_EQ_U64(peer->savekey.pending, 0u);

    peer_calls = peer_observations;
    CHECK_EQ_U64(muhtari_save_state(peer, public_second, total), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(peer_observations, peer_calls);
    CHECK_EQ_U64(muhtari_load_state(peer, public_second, total), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari_load_state(peer, public_second, total), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(peer_observations, peer_calls);
    CHECK_EQ_U64(peer->battery.generation, 0u);
    CHECK_EQ_HEX(peer->battery.data[2051u], 0x5Au);

    memcpy(public_first, public_second, total);
    public_first[battery_at + 2051u] = 0xA5u;
    public_first[115u] = 2u;
    state_rechecksum(public_first, total);
    memcpy(public_control, public_first, total);
    memcpy(battery_control, peer->battery.data, peer->battery.bytes);
    peer_calls = peer_observations;
    CHECK_EQ_U64(muhtari_load_state(peer, public_first, total), MUHTARI_STATUS_ERR_INVALID_STATE);
    CHECK_EQ_U64(peer_observations, peer_calls);
    CHECK(memcmp(public_first, public_control, total) == 0);
    CHECK_EQ_U64(peer->battery.generation, 0u);
    CHECK(memcmp(peer->battery.data, battery_control, peer->battery.bytes) == 0);
    CHECK(peer->battery.data == peer_battery);

    /* Rewind the accepted page snapshot over newer committed data. The live
       generation is host-owned and wraps once for this changed restore. */
    peer->battery.generation = UINT64_MAX;
    peer_calls = peer_observations;
    CHECK_EQ_U64(muhtari_load_state(peer, public_snapshot, total), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(peer_observations, peer_calls);
    CHECK_EQ_U64(peer->battery.generation, 0u);
    CHECK_EQ_HEX(peer->battery.data[2051u], 0xFFu);
    CHECK_EQ_U64(peer->savekey.pending, 1u);
    CHECK_EQ_U64(muhtari_load_state(peer, public_snapshot, total), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(peer->battery.generation, 0u);
    CHECK_EQ_HEX(peer->battery.data[0], 0xC1u);

    for (unsigned replay = 0u; replay < replay_frames; replay++) {
        peer_audio_count = state_run_frame(peer, public_video_b, public_audio_b);
        CHECK_EQ_U64(peer_audio_count, replay_audio_counts[replay]);
        CHECK(memcmp(public_video_reference[replay], public_video_b, video_bytes) == 0);
        CHECK(memcmp(public_audio_reference[replay], public_audio_b,
                     (size_t)peer_audio_count * sizeof public_audio_b[0]) == 0);
    }
    CHECK_EQ_HEX(peer->battery.data[2051u], 0x5Au);
    CHECK_EQ_U64(peer->battery.generation, 1u);
    CHECK_EQ_U64(peer->savekey.pending, 0u);
    CHECK(peer->battery.data == peer_battery);
    CHECK(peer->cart.expansion.ram == peer_expansion);

    (void)state_run_frame(source, public_video_a, public_audio_a);
    (void)state_run_frame(peer, public_video_b, public_audio_b);
    CHECK(memcmp(public_video_a, public_video_b, video_bytes) == 0);
    CHECK_EQ_U64(source->battery.generation, 1u);
    CHECK_EQ_U64(peer->battery.generation, 1u);
    CHECK_EQ_HEX(peer->battery.data[2051u], 0x5Au);

    if (host_file != NULL) {
        CHECK_EQ_U64(ftell(host_file), host_position);
        CHECK_EQ_U64(fseek(host_file, 0L, SEEK_SET), 0u);
        uint8_t file_after[sizeof file_marker];
        CHECK_EQ_U64(fread(file_after, 1u, sizeof file_after, host_file), sizeof file_marker);
        CHECK(memcmp(file_marker, file_after, sizeof file_marker) == 0);
        CHECK_EQ_U64(fclose(host_file), 0u);
    }

    muhtari_destroy(peer);
    muhtari_destroy(source);
}

static void test_topologies(void)
{
    static const struct continuation_row continuation_rows[] = {
        {"linear-ntsc-legacy", MUHTARI_BOARD_LINEAR, 0x0440u, 4u * IMAGES_KIB,
         MUHTARI_REGION_NTSC, MUHTARI_EXPANSION_OFF, MUHTARI_HSC_OFF,
         {MUHTARI_PORT_PROLINE, MUHTARI_PORT_PROLINE}, {0u, 0u}, 1u, 0u, 0u},
        {"supergame-pal-lightgun", MUHTARI_BOARD_SUPERGAME, 0x0462u, 64u * IMAGES_KIB,
         MUHTARI_REGION_PAL, MUHTARI_EXPANSION_OFF, MUHTARI_HSC_OFF,
         {MUHTARI_PORT_PROLINE, MUHTARI_PORT_PROLINE},
         {MUHTARI_PORT_LIGHTGUN, MUHTARI_PORT_MEGA7800}, 0u, 1u, 0u},
        {"activision-pokey", MUHTARI_BOARD_ACTIVISION, 0x0101u, 128u * IMAGES_KIB,
         MUHTARI_REGION_NTSC, MUHTARI_EXPANSION_OFF, MUHTARI_HSC_OFF,
         {MUHTARI_PORT_PROLINE, MUHTARI_PORT_PROLINE}, {MUHTARI_PORT_PADDLE, 0u}, 0u, 0u, 0u},
        {"absolute-keypad", MUHTARI_BOARD_ABSOLUTE, 0x0240u, 64u * IMAGES_KIB,
         MUHTARI_REGION_PAL, MUHTARI_EXPANSION_OFF, MUHTARI_HSC_OFF,
         {MUHTARI_PORT_PROLINE, MUHTARI_PORT_PROLINE}, {MUHTARI_PORT_KEYPAD, 0u}, 0u, 0u, 0u},
        {"fractalus-motion", MUHTARI_BOARD_FRACTALUS, 0x0480u, 32u * IMAGES_KIB,
         MUHTARI_REGION_NTSC, MUHTARI_EXPANSION_OFF, MUHTARI_HSC_OFF,
         {MUHTARI_PORT_PROLINE, MUHTARI_PORT_PROLINE}, {MUHTARI_PORT_ST_MOUSE, 0u}, 0u, 0u, 0u},
        {"bankset-xboard", MUHTARI_BOARD_BANKSET, 0u, 0u, MUHTARI_REGION_NTSC,
         MUHTARI_EXPANSION_XBOARD, MUHTARI_HSC_OFF,
         {MUHTARI_PORT_PROLINE, MUHTARI_PORT_PROLINE}, {MUHTARI_PORT_SNES2ATARI, 0u}, 0u, 0u, 0u},
        {"bankset-xm-savekey-port2", MUHTARI_BOARD_BANKSET, 0u, 0u, MUHTARI_REGION_PAL,
         MUHTARI_EXPANSION_XM, MUHTARI_HSC_ON,
         {MUHTARI_PORT_PROLINE, MUHTARI_PORT_SAVEKEY}, {MUHTARI_PORT_JOYSTICK, 0u}, 0u, 0u, 1u},
        {"bankset-hsc-stub-savekey-port1", MUHTARI_BOARD_BANKSET, 0u, 0u, MUHTARI_REGION_NTSC,
         MUHTARI_EXPANSION_OFF, MUHTARI_HSC_ON,
         {MUHTARI_PORT_SAVEKEY, MUHTARI_PORT_PROLINE}, {0u, 0u}, 0u, 0u, 0u},
        {"linear-bios", MUHTARI_BOARD_LINEAR, 0u, 4u * IMAGES_KIB, MUHTARI_REGION_PAL,
         MUHTARI_EXPANSION_OFF, MUHTARI_HSC_OFF,
         {MUHTARI_PORT_PROLINE, MUHTARI_PORT_PROLINE}, {MUHTARI_PORT_AMIGA_MOUSE, 0u}, 0u, 1u, 0u},
        {"bankset-hsc-firmware", MUHTARI_BOARD_BANKSET, 0u, 0u, MUHTARI_REGION_NTSC,
         MUHTARI_EXPANSION_OFF, MUHTARI_HSC_ON,
         {MUHTARI_PORT_PROLINE, MUHTARI_PORT_SAVEKEY}, {MUHTARI_PORT_KEYPAD, 0u}, 0u, 0u, 1u},
        {"linear-proline", MUHTARI_BOARD_LINEAR, 0x0440u, 4u * IMAGES_KIB,
         MUHTARI_REGION_NTSC, MUHTARI_EXPANSION_OFF, MUHTARI_HSC_OFF,
         {MUHTARI_PORT_PROLINE, MUHTARI_PORT_PROLINE}, {MUHTARI_PORT_PROLINE, 0u}, 0u, 0u, 0u},
        {"linear-driving", MUHTARI_BOARD_LINEAR, 0x0440u, 4u * IMAGES_KIB,
         MUHTARI_REGION_NTSC, MUHTARI_EXPANSION_OFF, MUHTARI_HSC_OFF,
         {MUHTARI_PORT_PROLINE, MUHTARI_PORT_PROLINE}, {MUHTARI_PORT_DRIVING, 0u}, 0u, 0u, 0u},
        {"linear-trakball", MUHTARI_BOARD_LINEAR, 0x0440u, 4u * IMAGES_KIB,
         MUHTARI_REGION_NTSC, MUHTARI_EXPANSION_OFF, MUHTARI_HSC_OFF,
         {MUHTARI_PORT_PROLINE, MUHTARI_PORT_PROLINE}, {MUHTARI_PORT_TRAKBALL, 0u}, 0u, 0u, 0u},
    };
    alloc_counts counts;
    alloc_counts peer_counts;
    muhtari *m = state_create(&counts);
    if (m == NULL)
        return;
    muhtari *peer = state_create(&peer_counts);
    if (peer == NULL) {
        muhtari_destroy(m);
        return;
    }
    CHECK_EQ_U64(state_load(m, STATE_LINEAR, 0), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari__state_payload_size(m), STATE_FIXED_BYTES);

    CHECK_EQ_U64(state_load(m, STATE_CART_RAM, 0), MUHTARI_STATUS_OK);
    size_t bytes = muhtari__state_payload_size(m);
    CHECK_EQ_U64(bytes, STATE_FIXED_BYTES + m->cart.info.ram_size);
    CHECK_EQ_U64(m->cart.info.ram_size, 32768u);
    m->cart.ram[0] = 0xA1u;
    m->cart.ram[m->cart.info.ram_size - 1u] = 0xB2u;
    CHECK(muhtari__state_payload_write(m, first, bytes));
    CHECK_EQ_HEX(first[STATE_FIXED_BYTES], 0xA1u);
    CHECK_EQ_HEX(first[bytes - 1u], 0xB2u);
    CHECK(muhtari__state_payload_validate(m, first, bytes));

    CHECK_EQ_U64(state_load(m, STATE_XBOARD, 0), MUHTARI_STATUS_OK);
    bytes = muhtari__state_payload_size(m);
    CHECK_EQ_U64(bytes, STATE_FIXED_BYTES + STATE_EXPANSION_BYTES);
    m->cart.expansion.ram[0] = 0xC3u;
    m->cart.expansion.ram[STATE_EXPANSION_BYTES - 1u] = 0xD4u;
    CHECK(muhtari__state_payload_write(m, first, bytes));
    CHECK_EQ_HEX(first[STATE_FIXED_BYTES], 0xC3u);
    CHECK_EQ_HEX(first[bytes - 1u], 0xD4u);
    m->cart.expansion.ram[0] = 0u;
    m->cart.expansion.ram[STATE_EXPANSION_BYTES - 1u] = 0u;
    muhtari__state_payload_apply(m, first, bytes);
    CHECK_EQ_HEX(m->cart.expansion.ram[0], 0xC3u);
    CHECK_EQ_HEX(m->cart.expansion.ram[STATE_EXPANSION_BYTES - 1u], 0xD4u);

    CHECK_EQ_U64(state_load(m, STATE_LARGEST_GENERATED, 0), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(m->cart.info.ram_size, 32768u);
    CHECK_EQ_U64(m->cart.expansion.kind, MUHTARI_EXPANSION_XM);
    CHECK_EQ_U64(m->battery.bytes, 34816u);
    bytes = muhtari__state_payload_size(m);
    CHECK_EQ_U64(bytes, STATE_FIXED_BYTES + 32768u + STATE_EXPANSION_BYTES + 34816u);
    CHECK_EQ_U64(bytes + STATE_HEADER_BYTES, 209628u);
    CHECK(bytes + STATE_HEADER_BYTES < STATE_MAX_BYTES);
    CHECK(muhtari__state_payload_write(m, first, bytes));
    CHECK(muhtari__state_payload_validate(m, first, bytes));

    /* Each supported board/topology row gets a distinct machine and an
       independently checked RAM mailbox before resumed output comparison. */
    for (size_t i = 0u; i < sizeof continuation_rows / sizeof continuation_rows[0]; i++) {
        continuation_row_run(&continuation_rows[i], m, peer, &counts, &peer_counts);
    }

    /* Header metadata, caller representation and Auto spelling do not alter
       an otherwise identical installed machine. */
    size_t identity_bytes =
        state_make_identity_image(identity_image, "Identity A", 3u, 0u);
    muhtari_media media = state_media_for_image(identity_image, identity_bytes);
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    uint8_t baseline[sizeof m->state_identity];
    memcpy(baseline, m->state_identity, sizeof baseline);
    static const uint8_t identity_vector[32] = {
        0x81u, 0xC9u, 0xA4u, 0xA6u, 0xA7u, 0x1Cu, 0x18u, 0x54u,
        0xEDu, 0xA7u, 0x45u, 0x45u, 0xFBu, 0x95u, 0x56u, 0x3Au,
        0x98u, 0x69u, 0xFFu, 0xFCu, 0xC3u, 0x3Fu, 0x9Du, 0x38u,
        0x73u, 0x95u, 0x19u, 0x32u, 0xC8u, 0x2Fu, 0x08u, 0xE0u,
    };
    CHECK(memcmp(baseline, identity_vector, sizeof identity_vector) == 0);
    memcpy(identity_raw, identity_image + IMAGES_HEADER, identity_bytes - IMAGES_HEADER);
    media = state_media_for_image(identity_raw, identity_bytes - IMAGES_HEADER);
    media.expansion = MUHTARI_EXPANSION_OFF;
    CHECK_EQ_U64(muhtari_load(peer, &media), MUHTARI_STATUS_OK);
    CHECK(memcmp(baseline, peer->state_identity, sizeof baseline) == 0);

    size_t equivalent_bytes =
        state_make_identity_image(identity_other, "Identity B", 3u, 0u);
    media = state_media_for_image(identity_other, equivalent_bytes);
    media.expansion = MUHTARI_EXPANSION_OFF;
    media.port[0] = MUHTARI_PORT_PROLINE;
    media.port[1] = MUHTARI_PORT_PROLINE;
    CHECK_EQ_U64(muhtari_load(peer, &media), MUHTARI_STATUS_OK);
    CHECK(memcmp(baseline, peer->state_identity, sizeof baseline) == 0);

    /* Every encoded immutable attachment/configuration change below has a
       separate comparison against the baseline. */
    identity_other[IMAGES_HEADER + 21u] ^= 1u;
    media = state_media_for_image(identity_other, equivalent_bytes);
    media.expansion = MUHTARI_EXPANSION_OFF;
    CHECK_EQ_U64(muhtari_load(peer, &media), MUHTARI_STATUS_OK);
    CHECK(memcmp(baseline, peer->state_identity, sizeof baseline) != 0);

    equivalent_bytes =
        state_make_identity_image(identity_other, "Identity B", 3u, 0x0002u);
    media = state_media_for_image(identity_other, equivalent_bytes);
    CHECK_EQ_U64(muhtari_load(peer, &media), MUHTARI_STATUS_OK);
    CHECK(memcmp(baseline, peer->state_identity, sizeof baseline) != 0);

    equivalent_bytes =
        state_make_identity_image(identity_other, "Identity B", 3u, 0x0010u);
    media = state_media_for_image(identity_other, equivalent_bytes);
    CHECK_EQ_U64(muhtari_load(peer, &media), MUHTARI_STATUS_OK);
    CHECK(memcmp(baseline, peer->state_identity, sizeof baseline) != 0);

    equivalent_bytes =
        state_make_identity_image(identity_other, "Identity B", 3u, 0x0004u);
    media = state_media_for_image(identity_other, equivalent_bytes);
    CHECK_EQ_U64(muhtari_load(peer, &media), MUHTARI_STATUS_OK);
    CHECK(memcmp(baseline, peer->state_identity, sizeof baseline) != 0);

    equivalent_bytes =
        state_make_identity_image(identity_other, "Identity B", 3u, 0x0001u);
    media = state_media_for_image(identity_other, equivalent_bytes);
    CHECK_EQ_U64(muhtari_load(peer, &media), MUHTARI_STATUS_OK);
    CHECK(memcmp(baseline, peer->state_identity, sizeof baseline) != 0);

    equivalent_bytes =
        state_make_identity_image(identity_other, "Identity B", 4u, 0u);
    identity_other[69] = 1u; /* Accepted v4 IRQ mask bit zero. */
    media = state_media_for_image(identity_other, equivalent_bytes);
    CHECK_EQ_U64(muhtari_load(peer, &media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(peer->cart.irq_mask, 1u);
    CHECK(memcmp(baseline, peer->state_identity, sizeof baseline) != 0);

    equivalent_bytes =
        state_make_identity_image(identity_other, "Identity B", 3u, 0u);
    media = state_media_for_image(identity_other, equivalent_bytes);
    media.region = MUHTARI_REGION_PAL;
    CHECK_EQ_U64(muhtari_load(peer, &media), MUHTARI_STATUS_OK);
    CHECK(memcmp(baseline, peer->state_identity, sizeof baseline) != 0);

    media = state_media_for_image(identity_other, equivalent_bytes);
    media.port[0] = MUHTARI_PORT_JOYSTICK;
    CHECK_EQ_U64(muhtari_load(peer, &media), MUHTARI_STATUS_OK);
    CHECK(memcmp(baseline, peer->state_identity, sizeof baseline) != 0);

    media = state_media_for_image(identity_other, equivalent_bytes);
    media.expansion = MUHTARI_EXPANSION_XBOARD;
    CHECK_EQ_U64(muhtari_load(peer, &media), MUHTARI_STATUS_OK);
    CHECK(memcmp(baseline, peer->state_identity, sizeof baseline) != 0);

    media = state_media_for_image(identity_other, equivalent_bytes);
    media.port[1] = MUHTARI_PORT_SAVEKEY;
    CHECK_EQ_U64(muhtari_load(peer, &media), MUHTARI_STATUS_OK);
    CHECK(memcmp(baseline, peer->state_identity, sizeof baseline) != 0);

    memset(identity_bios, 0xA6, sizeof identity_bios);
    identity_bios[sizeof identity_bios - 4u] = 0x00u;
    identity_bios[sizeof identity_bios - 3u] = 0xF0u;
    media = state_media_for_image(identity_other, equivalent_bytes);
    media.bios = identity_bios;
    media.bios_size = sizeof identity_bios;
    CHECK_EQ_U64(muhtari_load(peer, &media), MUHTARI_STATUS_OK);
    uint8_t bios_identity[sizeof peer->state_identity];
    memcpy(bios_identity, peer->state_identity, sizeof bios_identity);
    CHECK(memcmp(baseline, bios_identity, sizeof baseline) != 0);
    identity_bios[0] ^= 1u;
    CHECK(memcmp(bios_identity, peer->state_identity, sizeof bios_identity) == 0);
    identity_bios[0] ^= 1u;

    media = state_media_for_image(identity_other, equivalent_bytes);
    media.hsc_mode = MUHTARI_HSC_ON;
    CHECK_EQ_U64(muhtari_load(peer, &media), MUHTARI_STATUS_OK);
    uint8_t stub_identity[sizeof peer->state_identity];
    memcpy(stub_identity, peer->state_identity, sizeof stub_identity);
    CHECK(memcmp(baseline, stub_identity, sizeof baseline) != 0);
    memset(identity_firmware, 0x31, sizeof identity_firmware);
    media.hsc_rom = identity_firmware;
    media.hsc_rom_size = sizeof identity_firmware;
    CHECK_EQ_U64(muhtari_load(peer, &media), MUHTARI_STATUS_OK);
    uint8_t firmware_identity[sizeof peer->state_identity];
    memcpy(firmware_identity, peer->state_identity, sizeof firmware_identity);
    CHECK(memcmp(stub_identity, firmware_identity, sizeof stub_identity) != 0);
    identity_firmware[0] ^= 1u;
    CHECK(memcmp(firmware_identity, peer->state_identity, sizeof firmware_identity) == 0);
    identity_firmware[0] ^= 1u;
    memset(identity_other_firmware, 0x32, sizeof identity_other_firmware);
    media.hsc_rom = identity_other_firmware;
    CHECK_EQ_U64(muhtari_load(peer, &media), MUHTARI_STATUS_OK);
    CHECK(memcmp(firmware_identity, peer->state_identity, sizeof firmware_identity) != 0);

    /* A modified private profile is still installed machine behavior and is
       included even when all media choices match. */
    peer->profile.ane_lxa_constant ^= 1u;
    media = state_media_for_image(identity_other, equivalent_bytes);
    CHECK_EQ_U64(muhtari_load(peer, &media), MUHTARI_STATUS_OK);
    CHECK(memcmp(baseline, peer->state_identity, sizeof baseline) != 0);

    /* The accepted maximum input hashes its full installed 1 MiB ROM once;
       subsequent state queries only visit mutable payload fields. */
    const uint32_t max_rom_bytes = 1024u * IMAGES_KIB;
    images_header(image, 3u, "Maximum ROM", max_rom_bytes, 0x0002u);
    images_fill(image + IMAGES_HEADER, max_rom_bytes, 0x1303u);
    images_footer(image + IMAGES_HEADER, max_rom_bytes);
    media = state_media_for_image(image, IMAGES_HEADER + max_rom_bytes);
    CHECK_EQ_U64(media.image_size, MUHTARI_IMAGE_SIZE_MAX);
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(m->cart.info.rom_size, max_rom_bytes);
    uint8_t max_identity[sizeof m->state_identity];
    memcpy(max_identity, m->state_identity, sizeof max_identity);
    uint32_t max_allocations = counts.allocs;
    uint32_t max_frees = counts.frees;
    uint8_t saved_max_byte = image[IMAGES_HEADER + 1024u];
    image[IMAGES_HEADER + 1024u] ^= 1u;
    size_t max_payload = muhtari__state_payload_size(m);
    CHECK(muhtari__state_payload_write(m, first, max_payload));
    CHECK(muhtari__state_payload_validate(m, first, max_payload));
    CHECK(memcmp(max_identity, m->state_identity, sizeof max_identity) == 0);
    CHECK_EQ_U64(counts.allocs, max_allocations);
    CHECK_EQ_U64(counts.frees, max_frees);
    image[IMAGES_HEADER + 1024u] = saved_max_byte;

    CHECK_EQ_U64(state_load(m, STATE_LARGEST_GENERATED, 0), MUHTARI_STATUS_OK);
    check_public_roundtrip(m);

    /* Exercise the source-supported 128 KiB cart-RAM extent without carrying
       a database ROM fixture: the internal installer receives the same
       normalized linear-cart shape, then adds XM and SaveKey storage. */
    muhtari_cart_info largest = m->cart.info;
    largest.kind = MUHTARI_KIND_7800;
    largest.board = MUHTARI_BOARD_LINEAR;
    largest.board_options = 0u;
    largest.rom_size = 4096u;
    largest.ram_size = 131072u;
    largest.sound = 0u;
    CHECK_EQ_U64(muhtari__cart_install(m, image, &largest, MUHTARI_EXPANSION_XM),
                 MUHTARI_STATUS_OK);
    CHECK_EQ_U64(m->cart.info.ram_size, 131072u);
    CHECK_EQ_U64(m->battery.bytes, 34816u);
    bytes = muhtari__state_payload_size(m);
    CHECK_EQ_U64(bytes, STATE_FIXED_BYTES + 131072u + STATE_EXPANSION_BYTES + 34816u);
    CHECK_EQ_U64(bytes + STATE_HEADER_BYTES, STATE_MAX_BYTES);
    CHECK(muhtari__state_payload_write(m, first, bytes));
    CHECK(muhtari__state_payload_validate(m, first, bytes));
    muhtari_destroy(peer);
    muhtari_destroy(m);
}

static const struct {
    const char *name;
    void (*run)(void);
} cases[] = { {"payload", test_payload},       {"fields", test_fields},
              {"aliases", test_aliases},       {"lifecycle", test_lifecycle},
              {"battery", test_battery},       {"topologies", test_topologies} };

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: muhtari-test-state <case>\n");
        return 2;
    }
    for (size_t i = 0u; i < sizeof cases / sizeof cases[0]; i++) {
        if (strcmp(argv[1], cases[i].name) == 0) {
            cases[i].run();
            CHECK_DONE();
        }
    }
    fprintf(stderr, "muhtari-test-state: unknown case %s\n", argv[1]);
    return 2;
}
