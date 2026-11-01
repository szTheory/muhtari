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

#define STATE_HEADER_BYTES 64u
#define STATE_FIXED_BYTES 10908u
#define STATE_EXPANSION_BYTES 131072u
#define STATE_MAX_BYTES 307932u

/* The RED harness for plan 13-03 substitutes the not-yet-implemented public
   size query with an unavailable result. This keeps the test executable and
   makes the behavioral assertion, rather than a link error, fail first. */
#ifdef MUHTARI_STATE_RED_STUB
size_t muhtari_state_size(const muhtari *m);
size_t muhtari_state_size(const muhtari *m)
{
    (void)m;
    return 0u;
}
#endif

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
static uint8_t first[STATE_MAX_BYTES];
static uint8_t second[STATE_MAX_BYTES];
static uint8_t invalid_image[4] = {0u, 0u, 0u, 0u};
static muhtari snapshot;

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

static size_t expected_payload_size(const muhtari *m)
{
    size_t expansion = m->cart.expansion.kind == MUHTARI_EXPANSION_OFF
                           ? 0u
                           : STATE_EXPANSION_BYTES;
    return STATE_FIXED_BYTES + m->cart.info.ram_size + expansion + m->battery.bytes;
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
    uint32_t allocations = counts.allocs;
    uint32_t frees = counts.frees;
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
    CHECK_EQ_U64(counts.allocs, allocations);
    CHECK_EQ_U64(counts.frees, frees);

    first[inactive] = 1u;
    CHECK(!muhtari__state_payload_validate(m, first, bytes));
    memcpy(&snapshot, m, sizeof snapshot);
    muhtari__state_payload_apply(m, first, bytes);
    CHECK(memcmp(&snapshot, m, sizeof snapshot) == 0);
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
    muhtari_destroy(m);
}

static void test_lifecycle(void)
{
    alloc_counts counts;
    muhtari *m = state_create(&counts);
    if (m == NULL)
        return;
    CHECK_EQ_U64(muhtari__state_payload_size(m), 0u);
    uint8_t empty_identity[sizeof m->state_identity] = {0};
    CHECK(memcmp(m->state_identity, empty_identity, sizeof empty_identity) == 0);
    muhtari_media refused;
    memset(&refused, 0, sizeof refused);
    refused.size = (uint32_t)sizeof refused;
    refused.image = invalid_image;
    refused.image_size = sizeof invalid_image;
    CHECK(muhtari_load(m, &refused) != MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari__state_payload_size(m), 0u);
    CHECK(memcmp(m->state_identity, empty_identity, sizeof empty_identity) == 0);
    CHECK_EQ_U64(state_load(m, STATE_LINEAR, 0), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari_state_size(m), muhtari__state_payload_size(m) + STATE_HEADER_BYTES);
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
}

static void test_topologies(void)
{
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
