/* The private packed machine visitor. Its fields and order are specified in
   the Phase 13 execution design; this file exposes no host state API. */
#include <limits.h>
#include <stdint.h>
#include <string.h>

#include "crc32.h"
#include "internal.h"
#include "sha256.h"

#define STATE_HEADER_BYTES 64u
#define STATE_TOTAL_FIXED_BYTES 10972u
#define STATE_FIXED_BYTES (STATE_TOTAL_FIXED_BYTES - STATE_HEADER_BYTES)
#define STATE_FORMAT 1u
#define STATE_BEHAVIOUR_REVISION 6u
#define STATE_EXPANSION_BYTES 131072u
#define STATE_SYNTH_IMPULSE_LIMIT INT64_C(320961867552000)
#define STATE_SYNTH_INTEGRATOR_LIMIT INT64_C(274877906944)
#define STATE_SYNTH_PREVIOUS_Q30_LIMIT INT64_C(35184372088832)
#define STATE_SYNTH_BLOCKER_Q30_LIMIT INT64_C(140737488355328)

_Static_assert(STATE_TOTAL_FIXED_BYTES == STATE_HEADER_BYTES + STATE_FIXED_BYTES,
               "state fixed size includes exactly one envelope");

enum state_mode {
    STATE_SIZE,
    STATE_WRITE,
    STATE_VALIDATE,
    STATE_PUBLIC_VALIDATE,
    STATE_PUBLIC_LIVE,
    STATE_APPLY,
    STATE_LIVE
};
enum state_rule {
    STATE_ANY,
    STATE_BOOL,
    STATE_MASK,
    STATE_RANGE,
    STATE_SIGNED_RANGE,
    STATE_PORT_DEVICE,
    STATE_INPUT_DEVICE,
    STATE_INPUT_BUTTONS,
    STATE_INPUT_PADDLE,
    STATE_INPUT_KEYPAD,
    STATE_INPUT_DX,
    STATE_INPUT_DY,
    STATE_INPUT_GUN_X,
    STATE_INPUT_GUN_Y,
    STATE_INPUT_FLAGS,
    STATE_POT_ACTION,
    STATE_RIOT_TIMER_N,
    STATE_RIOT_TIMER_SHIFT,
    STATE_BUS_CYCLE_LEN,
    STATE_TIA_EPOCH_PHASE,
    STATE_CART_BANK,
    STATE_CART_RAM_BANK,
    STATE_EXPANSION_CONTROL,
    STATE_EXPANSION_PAGES,
    STATE_MARIA_LINE,
    STATE_MARIA_EVENT,
    STATE_MARIA_PIXEL,
    STATE_SYNTH_RAW,
    STATE_SYNTH_HEAD,
    STATE_SYNTH_PCM_READ,
    STATE_SYNTH_PCM_COUNT,
    STATE_POKEY_POLY17,
    STATE_POKEY_POLY4,
    STATE_POKEY_POLY5,
    STATE_SAVEKEY_PHASE,
    STATE_SAVEKEY_ADDRESS,
    STATE_SAVEKEY_PAGE_BASE,
    STATE_SAVEKEY_BITS
};
enum state_mark {
    MARK_AUDIO_REM,
    MARK_FRAME_START,
    MARK_FRAME_DEADLINE,
    MARK_FRAMES,
    MARK_LAG_FRAMES,
    MARK_CPU_PHI2,
    MARK_BUS_ORIGIN_TICK,
    MARK_BUS_ORIGIN_PHI2,
    MARK_BUS_PHI2,
    MARK_BUS_TICKS,
    MARK_MARIA_ORIGIN,
    MARK_SYNTH_TICK,
    MARK_SYNTH_ORIGIN,
    MARK_SYNTH_HORIZON,
    MARK_SYNTH_PENDING_TICK,
    MARK_SYNTH_PHASE,
    MARK_SYNTH_SAMPLE_NUM,
    MARK_SYNTH_SAMPLE_DEN,
    MARK_SYNTH_MAX_SAMPLES,
    MARK_SYNTH_PENDING,
    MARK_SYNTH_POKEYS,
    MARK_SYNTH_SECONDARY,
    MARK_BUS_RDY_RELEASE,
    MARK_BUS_HALT_AT,
    MARK_BUS_HALT_UNTIL,
    MARK_BUS_HALT_FALL,
    MARK_BUS_NMI_AT,
    MARK_BUS_HALT_STAGE,
    MARK_BUS_NMI_PENDING,
    MARK_RIOT_TIMER_PHI2,
    MARK_TIA_NEXT_TICK,
    MARK_TIA_EPOCH_TICK,
    MARK_POKEY_CYCLE_0,
    MARK_POKEY_CYCLE_1,
    MARK_POKEY_TICK_0,
    MARK_POKEY_TICK_1,
    MARK_SAVEKEY_BUSY_UNTIL,
    MARK_SAVEKEY_PENDING,
    MARK_COUNT
};

struct state_cursor {
    enum state_mode mode;
    const muhtari *source;
    const uint8_t *input;
    uint8_t *output;
    size_t limit;
    size_t offset;
    uint64_t marks[MARK_COUNT];
    uint32_t input_device;
    uint8_t input_port;
    uint8_t field_index;
    uint8_t valid;
};

static uint32_t state_get_u32(const uint8_t *bytes)
{
    return (uint32_t)bytes[0] | ((uint32_t)bytes[1] << 8) | ((uint32_t)bytes[2] << 16) |
           ((uint32_t)bytes[3] << 24);
}

static uint64_t state_get_u64(const uint8_t *bytes)
{
    return (uint64_t)state_get_u32(bytes) | ((uint64_t)state_get_u32(bytes + 4u) << 32);
}

static void state_put_u32(uint8_t *bytes, uint32_t value)
{
    bytes[0] = (uint8_t)value;
    bytes[1] = (uint8_t)(value >> 8);
    bytes[2] = (uint8_t)(value >> 16);
    bytes[3] = (uint8_t)(value >> 24);
}

static void state_put_u64(uint8_t *bytes, uint64_t value)
{
    state_put_u32(bytes, (uint32_t)value);
    state_put_u32(bytes + 4u, (uint32_t)(value >> 32));
}

static void state_identity_u32(muhtari__sha256 *hash, uint32_t tag, uint32_t value)
{
    uint8_t record[12];
    state_put_u32(record, tag);
    state_put_u32(record + 4u, 4u);
    state_put_u32(record + 8u, value);
    muhtari__sha256_update(hash, record, sizeof record);
}

static void state_identity_bytes(muhtari__sha256 *hash, uint32_t tag, const void *bytes,
                                 uint32_t size)
{
    uint8_t header[8];
    state_put_u32(header, tag);
    state_put_u32(header + 4u, size);
    muhtari__sha256_update(hash, header, sizeof header);
    muhtari__sha256_update(hash, bytes, size);
}

void muhtari__state_bind_identity(muhtari *m)
{
    static const uint8_t domain[] = "MUHTARI-STATE-IDENTITY";
    muhtari__sha256 hash;
    uint8_t sound[8];
    uint8_t profile[6];
    uint32_t boot_mode = m->bios_size == 0u ? 0u : 1u;
    uint32_t hsc_mode = m->battery.hsc_attached == 0u                                   ? 0u
                        : (m->battery.firmware_flags & MUHTARI_HSC_FIRMWARE_STUB) != 0u ? 1u
                                                                                        : 2u;

    muhtari__sha256_init(&hash);
    muhtari__sha256_update(&hash, domain, sizeof domain);

    state_put_u32(sound, m->cart.pokey_at[0]);
    state_put_u32(sound + 4u, m->cart.pokey_at[1]);
    profile[0] = m->profile.ane_lxa_constant;
    profile[1] = m->profile.riot_orb_power_on;
    profile[2] = m->profile.ram_fill;
    profile[3] = (uint8_t)m->profile.tia_phase;
    profile[4] = (uint8_t)(m->profile.tia_phase >> 8);
    profile[5] = m->profile.pokey_init;

    state_identity_bytes(&hash, 1u, m->cart.rom, m->cart.info.rom_size);
    state_identity_u32(&hash, 2u, m->cart.info.kind);
    state_identity_u32(&hash, 3u, m->cart.info.board);
    state_identity_u32(&hash, 4u, m->cart.info.board_options);
    state_identity_u32(&hash, 5u, m->cart.info.ram_size);
    state_identity_u32(&hash, 6u, m->effective_region);
    state_identity_bytes(&hash, 7u, sound, sizeof sound);
    state_identity_u32(&hash, 8u, m->cart.irq_mask);
    state_identity_u32(&hash, 9u, m->cart.expansion.kind);
    state_identity_u32(&hash, 10u, muhtari__ports_device(m, 0u));
    state_identity_u32(&hash, 11u, muhtari__ports_device(m, 1u));
    state_identity_u32(&hash, 12u, m->battery.hsc_attached);
    state_identity_u32(&hash, 13u, m->battery.eeprom_port);
    state_identity_u32(&hash, 14u, m->battery.eeprom_device);
    state_identity_u32(&hash, 15u, (uint32_t)m->battery.bytes);
    state_identity_u32(&hash, 16u, boot_mode);
    state_identity_bytes(&hash, 17u, m->bios, boot_mode == 0u ? 0u : m->bios_size);
    state_identity_u32(&hash, 18u, hsc_mode);
    state_identity_bytes(&hash, 19u, m->battery.rom, hsc_mode == 0u ? 0u : sizeof m->battery.rom);
    state_identity_bytes(&hash, 20u, profile, sizeof profile);
    state_identity_u32(&hash, 21u, m->synth.pokeys);
    state_identity_u32(&hash, 22u, m->synth.secondary);
    muhtari__sha256_final(&hash, m->state_identity);
}

static int32_t state_i32_from_bits(uint32_t bits)
{
    if (bits <= (uint32_t)INT32_MAX)
        return (int32_t)bits;
    return (int32_t)(-1 - (int32_t)~bits);
}

static int64_t state_i64_from_bits(uint64_t bits)
{
    if (bits <= (uint64_t)INT64_MAX)
        return (int64_t)bits;
    return -1 - (int64_t)~bits;
}

static uint64_t state_native_value(const void *source, unsigned width, int is_signed)
{
    if (width == 1u)
        return *(const uint8_t *)source;
    if (width == 2u) {
        uint16_t value;
        memcpy(&value, source, sizeof value);
        return value;
    }
    if (width == 4u) {
        if (is_signed != 0) {
            int32_t value;
            memcpy(&value, source, sizeof value);
            return (uint32_t)value;
        }
        uint32_t value;
        memcpy(&value, source, sizeof value);
        return value;
    }
    if (is_signed != 0) {
        int64_t value;
        memcpy(&value, source, sizeof value);
        return (uint64_t)value;
    }
    uint64_t value;
    memcpy(&value, source, sizeof value);
    return value;
}

static void state_store_native(void *target, unsigned width, int is_signed, uint64_t value)
{
    if (width == 1u) {
        uint8_t decoded = (uint8_t)value;
        memcpy(target, &decoded, sizeof decoded);
    } else if (width == 2u) {
        uint16_t decoded = (uint16_t)value;
        memcpy(target, &decoded, sizeof decoded);
    } else if (width == 4u && is_signed != 0) {
        int32_t decoded = state_i32_from_bits((uint32_t)value);
        memcpy(target, &decoded, sizeof decoded);
    } else if (width == 4u) {
        uint32_t decoded = (uint32_t)value;
        memcpy(target, &decoded, sizeof decoded);
    } else if (is_signed != 0) {
        int64_t decoded = state_i64_from_bits(value);
        memcpy(target, &decoded, sizeof decoded);
    } else {
        memcpy(target, &value, sizeof value);
    }
}

static int state_port_device_valid(uint64_t value)
{
    return value <= MUHTARI_PORT_MEGA7800 || value == MUHTARI_PORT_UNPLUGGED ||
           value == MUHTARI_PORT_SAVEKEY;
}

static int state_input_device_valid(const struct state_cursor *cursor, uint64_t value)
{
    const muhtari *m = cursor->source;
    if (state_port_device_valid(value) == 0 && value != MUHTARI_PORT_ATARIVOX)
        return 0;
    if (value == 0u)
        return 1;
    if (m->battery.eeprom_port == (uint32_t)cursor->input_port + 1u)
        return value == m->battery.eeprom_device;
    return value != MUHTARI_PORT_SAVEKEY && value != MUHTARI_PORT_ATARIVOX;
}

static uint32_t state_input_button_mask(uint32_t device)
{
    if (device == MUHTARI_PORT_SNES2ATARI || device == MUHTARI_PORT_MEGA7800)
        return 4095u;
    if (device == MUHTARI_PORT_KEYPAD || device == MUHTARI_PORT_UNPLUGGED ||
        device == MUHTARI_PORT_SAVEKEY || device == MUHTARI_PORT_ATARIVOX)
        return 0u;
    if (device == MUHTARI_PORT_PADDLE || device == MUHTARI_PORT_ST_MOUSE ||
        device == MUHTARI_PORT_AMIGA_MOUSE)
        return MUHTARI_BUTTON_1 | MUHTARI_BUTTON_2;
    if (device == MUHTARI_PORT_DRIVING || device == MUHTARI_PORT_LIGHTGUN ||
        device == MUHTARI_PORT_TRAKBALL)
        return MUHTARI_BUTTON_1;
    return 0x3Fu;
}

static int state_rule_valid(const struct state_cursor *cursor, enum state_rule rule, uint64_t value,
                            unsigned width, int is_signed, uint64_t lower, uint64_t upper)
{
    switch (rule) {
    case STATE_ANY:
        return 1;
    case STATE_BOOL:
        return value <= 1u;
    case STATE_MASK:
        return (value & ~lower) == 0u;
    case STATE_RANGE:
        return value >= lower && value <= upper;
    case STATE_SIGNED_RANGE:
        if (is_signed != 0) {
            int64_t signed_value =
                width == 4u ? state_i32_from_bits((uint32_t)value) : state_i64_from_bits(value);
            int64_t signed_lower = state_i64_from_bits(lower);
            int64_t signed_upper = state_i64_from_bits(upper);
            return signed_value >= signed_lower && signed_value <= signed_upper;
        }
        return 0;
    case STATE_PORT_DEVICE:
        return state_input_device_valid(cursor, value);
    case STATE_INPUT_DEVICE:
        return state_input_device_valid(cursor, value);
    case STATE_INPUT_BUTTONS:
        return (value & ~(uint64_t)state_input_button_mask(cursor->input_device)) == 0u;
    case STATE_INPUT_PADDLE:
        return cursor->input_device == MUHTARI_PORT_PADDLE ? value <= 65535u : value == 0u;
    case STATE_INPUT_KEYPAD:
        return cursor->input_device == MUHTARI_PORT_KEYPAD ? value <= 4095u : value == 0u;
    case STATE_INPUT_DX:
        return cursor->input_device == MUHTARI_PORT_DRIVING ||
                       cursor->input_device == MUHTARI_PORT_TRAKBALL ||
                       cursor->input_device == MUHTARI_PORT_ST_MOUSE ||
                       cursor->input_device == MUHTARI_PORT_AMIGA_MOUSE
                   ? 1
                   : value == 0u;
    case STATE_INPUT_DY:
        return cursor->input_device == MUHTARI_PORT_TRAKBALL ||
                       cursor->input_device == MUHTARI_PORT_ST_MOUSE ||
                       cursor->input_device == MUHTARI_PORT_AMIGA_MOUSE
                   ? 1
                   : value == 0u;
    case STATE_INPUT_GUN_X:
        return cursor->input_device == MUHTARI_PORT_LIGHTGUN ? value < MUHTARI__WIDTH : value == 0u;
    case STATE_INPUT_GUN_Y:
        return cursor->input_device == MUHTARI_PORT_LIGHTGUN
                   ? value < muhtari__timing(cursor->source).height
                   : value == 0u;
    case STATE_INPUT_FLAGS:
        return cursor->input_device == MUHTARI_PORT_LIGHTGUN ? (value & ~UINT64_C(1)) == 0u
                                                             : value == 0u;
    case STATE_POT_ACTION:
        return value <= (UINT64_C(1) << 32);
    case STATE_RIOT_TIMER_N:
        return value <= 256u;
    case STATE_RIOT_TIMER_SHIFT:
        return value == 0u || value == 3u || value == 6u || value == 10u;
    case STATE_BUS_CYCLE_LEN:
        return value == 0u || value == 4u || value == 6u;
    case STATE_TIA_EPOCH_PHASE:
        return value < 456u;
    case STATE_CART_BANK: {
        const muhtari_cart_info *info = &cursor->source->cart.info;
        if (info->board == MUHTARI_BOARD_BANKSET) {
            uint32_t half = info->rom_size / 2u;
            if (half < 128u * 1024u)
                return value == 0u;
            return value < half / 16384u;
        }
        if (info->board == MUHTARI_BOARD_ACTIVISION)
            return value <= 7u;
        if (info->board == MUHTARI_BOARD_ABSOLUTE)
            return value <= 1u;
        if (info->board == MUHTARI_BOARD_SUPERGAME) {
            uint32_t banks = info->rom_size / 16384u;
            uint32_t available =
                (info->board_options & MUHTARI_BOARD_ROM_AT_4000) != 0u ? banks - 1u : banks;
            return value < available;
        }
        return value == 0u;
    }
    case STATE_CART_RAM_BANK:
        return (cursor->source->cart.info.board_options & MUHTARI_BOARD_BANKED_RAM) != 0u
                   ? value <= 1u
                   : value == 0u;
    case STATE_EXPANSION_CONTROL:
        return cursor->source->cart.expansion.kind != MUHTARI_EXPANSION_OFF || value == 0u;
    case STATE_EXPANSION_PAGES:
        return cursor->source->cart.expansion.kind == MUHTARI_EXPANSION_XM || value == 0u;
    case STATE_MARIA_LINE:
        return value <= muhtari__timing(cursor->source).lines;
    case STATE_MARIA_EVENT:
        return value <= 3u;
    case STATE_MARIA_PIXEL:
        return value <= MUHTARI__WIDTH;
    case STATE_SYNTH_RAW:
        return value <= (cursor->field_index < 2u ? 15u : 206u);
    case STATE_SYNTH_HEAD:
        return value <= 15u;
    case STATE_SYNTH_PCM_READ:
        return value < MUHTARI__SYNTH_PCM_CAPACITY;
    case STATE_SYNTH_PCM_COUNT:
        return value <= MUHTARI__SYNTH_PCM_CAPACITY;
    case STATE_POKEY_POLY17:
        return (value & ~UINT64_C(0x1FFFF)) == 0u;
    case STATE_POKEY_POLY4:
        return value <= 15u;
    case STATE_POKEY_POLY5:
        return value <= 31u;
    case STATE_SAVEKEY_PHASE:
        return value <= 11u;
    case STATE_SAVEKEY_ADDRESS:
        return value <= 0x7FFFu;
    case STATE_SAVEKEY_PAGE_BASE:
        return value <= 0x7FC0u && (value & 63u) == 0u;
    case STATE_SAVEKEY_BITS:
        return value <= 8u;
    }
    return 0;
}

static int state_reserve(struct state_cursor *cursor, size_t bytes, size_t *at)
{
    if (bytes > SIZE_MAX - cursor->offset) {
        cursor->valid = 0u;
        return 0;
    }
    *at = cursor->offset;
    cursor->offset += bytes;
    if (cursor->mode == STATE_SIZE || cursor->mode == STATE_LIVE ||
        cursor->mode == STATE_PUBLIC_LIVE)
        return 1;
    if (*at > cursor->limit || bytes > cursor->limit - *at) {
        cursor->valid = 0u;
        return 0;
    }
    return 1;
}

static uint64_t state_scalar(struct state_cursor *cursor, const void *source, void *target,
                             unsigned width, int is_signed, enum state_rule rule, uint64_t lower,
                             uint64_t upper, enum state_mark mark, int force_zero)
{
    size_t at;
    if (state_reserve(cursor, (size_t)width, &at) == 0)
        return 0u;
    if (cursor->mode == STATE_SIZE)
        return 0u;

    uint64_t value = 0u;
    if (cursor->mode == STATE_WRITE) {
        value = force_zero != 0 ? 0u : state_native_value(source, width, is_signed);
        if (width == 1u) {
            cursor->output[at] = (uint8_t)value;
        } else if (width == 2u) {
            cursor->output[at] = (uint8_t)value;
            cursor->output[at + 1u] = (uint8_t)(value >> 8);
        } else if (width == 4u) {
            state_put_u32(cursor->output + at, (uint32_t)value);
        } else {
            state_put_u64(cursor->output + at, value);
        }
        return value;
    }

    if (cursor->mode == STATE_LIVE || cursor->mode == STATE_PUBLIC_LIVE) {
        value = state_native_value(source, width, is_signed);
    } else if (width == 1u) {
        value = cursor->input[at];
    } else if (width == 2u) {
        value = (uint64_t)cursor->input[at] | ((uint64_t)cursor->input[at + 1u] << 8);
    } else if (width == 4u) {
        value = state_get_u32(cursor->input + at);
    } else {
        value = state_get_u64(cursor->input + at);
    }

    if (((cursor->mode == STATE_VALIDATE || cursor->mode == STATE_PUBLIC_VALIDATE) &&
         force_zero != 0 && value != 0u) ||
        state_rule_valid(cursor, rule, value, width, is_signed, lower, upper) == 0)
        cursor->valid = 0u;
    if (mark < MARK_COUNT)
        cursor->marks[mark] = value;
    if (cursor->mode == STATE_APPLY && target != NULL) {
        if (force_zero != 0)
            value = 0u;
        state_store_native(target, width, is_signed, value);
    }
    return value;
}

static void state_bytes(struct state_cursor *cursor, const uint8_t *source, uint8_t *target,
                        size_t bytes, size_t zero_from)
{
    size_t at;
    if (state_reserve(cursor, bytes, &at) == 0 || bytes == 0u)
        return;
    if (cursor->mode == STATE_SIZE)
        return;
    if (cursor->mode == STATE_WRITE) {
        if (zero_from >= bytes) {
            memcpy(cursor->output + at, source, bytes);
        } else {
            memcpy(cursor->output + at, source, zero_from);
            memset(cursor->output + at + zero_from, 0, bytes - zero_from);
        }
    } else if (cursor->mode == STATE_LIVE || cursor->mode == STATE_PUBLIC_LIVE) {
        return;
    } else if (cursor->mode == STATE_VALIDATE || cursor->mode == STATE_PUBLIC_VALIDATE) {
        if (zero_from < bytes) {
            for (size_t i = zero_from; i < bytes; i++) {
                if (cursor->input[at + i] != 0u)
                    cursor->valid = 0u;
            }
        }
    } else if (target != NULL) {
        memcpy(target, cursor->input + at, bytes);
        if (zero_from < bytes)
            memset(target + zero_from, 0, bytes - zero_from);
    }
}

static void state_input_port(struct state_cursor *cursor, unsigned port,
                             const muhtari_port_input *source, muhtari_port_input *target)
{
    cursor->input_port = (uint8_t)port;
    cursor->input_device =
        (uint32_t)state_scalar(cursor, &source->device, target != NULL ? &target->device : NULL, 4u,
                               0, STATE_INPUT_DEVICE, 0u, 0u, MARK_COUNT, 0);
    state_scalar(cursor, &source->buttons, target != NULL ? &target->buttons : NULL, 4u, 0,
                 STATE_INPUT_BUTTONS, 0u, 0u, MARK_COUNT, 0);
    for (unsigned axis = 0u; axis < 2u; axis++)
        state_scalar(cursor, &source->paddle[axis], target != NULL ? &target->paddle[axis] : NULL,
                     4u, 0, STATE_INPUT_PADDLE, 0u, 0u, MARK_COUNT, 0);
    state_scalar(cursor, &source->keypad, target != NULL ? &target->keypad : NULL, 4u, 0,
                 STATE_INPUT_KEYPAD, 0u, 0u, MARK_COUNT, 0);
    state_scalar(cursor, &source->dx, target != NULL ? &target->dx : NULL, 4u, 1, STATE_INPUT_DX,
                 0u, 0u, MARK_COUNT, 0);
    state_scalar(cursor, &source->dy, target != NULL ? &target->dy : NULL, 4u, 1, STATE_INPUT_DY,
                 0u, 0u, MARK_COUNT, 0);
    state_scalar(cursor, &source->gun_x, target != NULL ? &target->gun_x : NULL, 4u, 0,
                 STATE_INPUT_GUN_X, 0u, 0u, MARK_COUNT, 0);
    state_scalar(cursor, &source->gun_y, target != NULL ? &target->gun_y : NULL, 4u, 0,
                 STATE_INPUT_GUN_Y, 0u, 0u, MARK_COUNT, 0);
    state_scalar(cursor, &source->flags, target != NULL ? &target->flags : NULL, 4u, 0,
                 STATE_INPUT_FLAGS, 0u, 0u, MARK_COUNT, 0);
}

static void state_pokey(struct state_cursor *cursor, const struct muhtari__pokey *source,
                        struct muhtari__pokey *target, unsigned chip)
{
#define POKEY_FIELD(name, width, is_signed, rule, lower, upper)                                    \
    state_scalar(cursor, &source->name, target != NULL ? &target->name : NULL, width, is_signed,   \
                 rule, lower, upper, MARK_COUNT, 0)
    state_scalar(cursor, &source->cycle, target != NULL ? &target->cycle : NULL, 8u, 0, STATE_ANY,
                 0u, 0u, chip == 0u ? MARK_POKEY_CYCLE_0 : MARK_POKEY_CYCLE_1, 0);
    state_scalar(cursor, &source->tick, target != NULL ? &target->tick : NULL, 8u, 0, STATE_ANY, 0u,
                 0u, chip == 0u ? MARK_POKEY_TICK_0 : MARK_POKEY_TICK_1, 0);
    POKEY_FIELD(half, 8u, 0, STATE_ANY, 0u, 0u);
    for (unsigned i = 0u; i < 2u; i++) {
        state_scalar(cursor, &source->carry[i], target != NULL ? &target->carry[i] : NULL, 8u, 0,
                     STATE_ANY, 0u, 0u, MARK_COUNT, 0);
    }
    for (unsigned i = 0u; i < 2u; i++) {
        state_scalar(cursor, &source->resync[i], target != NULL ? &target->resync[i] : NULL, 8u, 0,
                     STATE_ANY, 0u, 0u, MARK_COUNT, 0);
    }
    POKEY_FIELD(poly17, 4u, 0, STATE_POKEY_POLY17, 0u, 0u);
    POKEY_FIELD(poly4, 1u, 0, STATE_POKEY_POLY4, 0u, 0u);
    POKEY_FIELD(poly5, 1u, 0, STATE_POKEY_POLY5, 0u, 0u);
    state_bytes(cursor, source->noise, target != NULL ? target->noise : NULL, sizeof source->noise,
                SIZE_MAX);
    POKEY_FIELD(prescale28, 2u, 0, STATE_RANGE, 0u, 28u);
    POKEY_FIELD(prescale114, 2u, 0, STATE_RANGE, 0u, 114u);
    POKEY_FIELD(audctl, 1u, 0, STATE_ANY, 0u, 0u);
    POKEY_FIELD(skctl, 1u, 0, STATE_ANY, 0u, 0u);
    POKEY_FIELD(irqen, 1u, 0, STATE_ANY, 0u, 0u);
    POKEY_FIELD(irqst, 1u, 0, STATE_ANY, 0u, 0u);
    POKEY_FIELD(initialized, 1u, 0, STATE_BOOL, 0u, 0u);
    for (unsigned i = 0u; i < 2u; i++) {
        state_scalar(cursor, &source->hp_delay[i], target != NULL ? &target->hp_delay[i] : NULL, 1u,
                     0, STATE_RANGE, 0u, 3u, MARK_COUNT, 0);
    }
    for (unsigned i = 0u; i < 2u; i++) {
        state_scalar(cursor, &source->hp_capture[i], target != NULL ? &target->hp_capture[i] : NULL,
                     1u, 0, STATE_BOOL, 0u, 0u, MARK_COUNT, 0);
    }
    for (unsigned i = 0u; i < 3u; i++) {
        state_scalar(cursor, &source->irq_enable[i], target != NULL ? &target->irq_enable[i] : NULL,
                     1u, 0, STATE_ANY, 0u, 0u, MARK_COUNT, 0);
    }
    POKEY_FIELD(irq_signal, 1u, 0, STATE_BOOL, 0u, 0u);
    for (unsigned ch = 0u; ch < 4u; ch++) {
        const struct muhtari__pokey_channel *sc = &source->channel[ch];
        struct muhtari__pokey_channel *tc = target != NULL ? &target->channel[ch] : NULL;
#define CHANNEL_FIELD(name, width, rule, lower, upper)                                             \
    state_scalar(cursor, &sc->name, tc != NULL ? &tc->name : NULL, width, 0, rule, lower, upper,   \
                 MARK_COUNT, 0)
        CHANNEL_FIELD(reload, 8u, STATE_ANY, 0u, 0u);
        CHANNEL_FIELD(fire, 8u, STATE_ANY, 0u, 0u);
        CHANNEL_FIELD(last_fire, 8u, STATE_ANY, 0u, 0u);
        CHANNEL_FIELD(fires, 4u, STATE_ANY, 0u, 0u);
        CHANNEL_FIELD(count, 2u, STATE_RANGE, 0u, 255u);
        CHANNEL_FIELD(audf, 1u, STATE_ANY, 0u, 0u);
        CHANNEL_FIELD(audc, 1u, STATE_ANY, 0u, 0u);
        CHANNEL_FIELD(output, 1u, STATE_BOOL, 0u, 0u);
        CHANNEL_FIELD(hp, 1u, STATE_BOOL, 0u, 0u);
        CHANNEL_FIELD(starting, 1u, STATE_BOOL, 0u, 0u);
#undef CHANNEL_FIELD
    }
#undef POKEY_FIELD
}

static void state_visit(const muhtari *source, muhtari *target, struct state_cursor *cursor)
{
#define FIELD(member, width, is_signed, rule, lower, upper, mark)                                  \
    state_scalar(cursor, &source->member, target != NULL ? &target->member : NULL, width,          \
                 is_signed, rule, lower, upper, mark, 0)
#define U8(member) FIELD(member, 1u, 0, STATE_ANY, 0u, 0u, MARK_COUNT)
#define U16(member) FIELD(member, 2u, 0, STATE_ANY, 0u, 0u, MARK_COUNT)
#define U32(member) FIELD(member, 4u, 0, STATE_ANY, 0u, 0u, MARK_COUNT)
#define U64(member) FIELD(member, 8u, 0, STATE_ANY, 0u, 0u, MARK_COUNT)
#define CHECK_U8(member, rule, lower, upper) FIELD(member, 1u, 0, rule, lower, upper, MARK_COUNT)
#define CHECK_U16(member, rule, lower, upper) FIELD(member, 2u, 0, rule, lower, upper, MARK_COUNT)
#define CHECK_U32(member, rule, lower, upper) FIELD(member, 4u, 0, rule, lower, upper, MARK_COUNT)
#define CHECK_U64(member, rule, lower, upper, mark) FIELD(member, 8u, 0, rule, lower, upper, mark)

    FIELD(audio_rem, 4u, 0, STATE_RANGE, 0u, muhtari__timing(source).sample_den - 1u,
          MARK_AUDIO_REM);
    CHECK_U64(frame_start, STATE_ANY, 0u, 0u, MARK_FRAME_START);
    CHECK_U64(frame_deadline, STATE_ANY, 0u, 0u, MARK_FRAME_DEADLINE);
    CHECK_U64(frames, STATE_ANY, 0u, 0u, MARK_FRAMES);
    CHECK_U64(lag_frames, STATE_ANY, 0u, 0u, MARK_LAG_FRAMES);

    CHECK_U64(cpu.poll_phi2, STATE_ANY, 0u, 0u, MARK_CPU_PHI2);
    U16(cpu.pc);
    U8(cpu.a);
    U8(cpu.x);
    U8(cpu.y);
    U8(cpu.s);
    U8(cpu.p);
    CHECK_U8(cpu.jammed, STATE_BOOL, 0u, 0u);
    CHECK_U8(cpu.irq_sampled, STATE_BOOL, 0u, 0u);

    CHECK_U64(bus.clock_origin_tick, STATE_ANY, 0u, 0u, MARK_BUS_ORIGIN_TICK);
    CHECK_U64(bus.clock_origin_phi2, STATE_ANY, 0u, 0u, MARK_BUS_ORIGIN_PHI2);
    CHECK_U64(bus.phi2, STATE_ANY, 0u, 0u, MARK_BUS_PHI2);
    U64(bus.nmi_phi2);
    CHECK_U64(bus.ticks, STATE_ANY, 0u, 0u, MARK_BUS_TICKS);
    CHECK_U64(bus.rdy_release, STATE_ANY, 0u, 0u, MARK_BUS_RDY_RELEASE);
    CHECK_U64(bus.halt_at, STATE_ANY, 0u, 0u, MARK_BUS_HALT_AT);
    CHECK_U64(bus.halt_until, STATE_ANY, 0u, 0u, MARK_BUS_HALT_UNTIL);
    CHECK_U64(bus.halt_fall, STATE_ANY, 0u, 0u, MARK_BUS_HALT_FALL);
    U64(bus.maria_from);
    U64(bus.maria_until);
    CHECK_U64(bus.nmi_at, STATE_ANY, 0u, 0u, MARK_BUS_NMI_AT);
    U8(bus.open_bus);
    CHECK_U8(bus.inptctrl, STATE_MASK, MUHTARI__INPTCTRL_BITS, 0u);
    CHECK_U8(bus.halt_gate_writes, STATE_RANGE, 0u, 2u);
    FIELD(bus.halt_stage, 1u, 0, STATE_RANGE, 0u, 2u, MARK_BUS_HALT_STAGE);
    CHECK_U8(bus.halt_cycles, STATE_RANGE, 0u, 2u);
    FIELD(bus.nmi_pending, 1u, 0, STATE_BOOL, 0u, 0u, MARK_BUS_NMI_PENDING);
    CHECK_U8(bus.cycle_len, STATE_BUS_CYCLE_LEN, 0u, 0u);
    CHECK_U8(bus.sync, STATE_BOOL, 0u, 0u);
    CHECK_U8(bus.irq_pipe, STATE_MASK, 7u, 0u);
    CHECK_U8(bus.irq_level, STATE_BOOL, 0u, 0u);

    CHECK_U64(riot.timer_phi2, STATE_ANY, 0u, 0u, MARK_RIOT_TIMER_PHI2);
    CHECK_U16(riot.timer_n, STATE_RIOT_TIMER_N, 0u, 0u);
    CHECK_U8(riot.timer_shift, STATE_RIOT_TIMER_SHIFT, 0u, 0u);
    CHECK_U8(riot.timer_cleared, STATE_BOOL, 0u, 0u);
    U8(riot.ora);
    U8(riot.ddra);
    U8(riot.orb);
    U8(riot.ddrb);
    CHECK_U8(riot.pa7_flag, STATE_BOOL, 0u, 0u);
    CHECK_U8(riot.pa7_positive, STATE_BOOL, 0u, 0u);
    CHECK_U8(riot.pa7_detect, STATE_BOOL, 0u, 0u);
    state_bytes(cursor, source->riot.ram, target != NULL ? target->riot.ram : NULL,
                sizeof source->riot.ram, SIZE_MAX);
    state_bytes(cursor, source->ram, target != NULL ? target->ram : NULL, sizeof source->ram,
                SIZE_MAX);

    state_bytes(cursor, source->maria.reg, target != NULL ? target->maria.reg : NULL,
                sizeof source->maria.reg, SIZE_MAX);
    U16(maria.dll);
    U16(maria.dl);
    U8(maria.entry);
    U8(maria.offset);
    CHECK_U8(maria.write_mode, STATE_BOOL, 0u, 0u);
    CHECK_U8(maria.write_buf, STATE_RANGE, 0u, 1u);
    state_bytes(cursor, &source->maria.line_ram[0][0],
                target != NULL ? &target->maria.line_ram[0][0] : NULL,
                sizeof source->maria.line_ram, SIZE_MAX);
    for (unsigned i = 0u; i < 313u; i++) {
        const struct muhtari__line_stats *ss = &source->maria.stats[i];
        struct muhtari__line_stats *ts = target != NULL ? &target->maria.stats[i] : NULL;
        int inactive = i >= muhtari__timing(source).lines;
        state_scalar(cursor, &ss->cost, ts != NULL ? &ts->cost : NULL, 2u, 0, STATE_RANGE, 0u,
                     MUHTARI__LINE_TICKS, MARK_COUNT, inactive);
        state_scalar(cursor, &ss->bytes, ts != NULL ? &ts->bytes : NULL, 2u, 0, STATE_ANY, 0u, 0u,
                     MARK_COUNT, inactive);
        state_scalar(cursor, &ss->headers, ts != NULL ? &ts->headers : NULL, 1u, 0, STATE_ANY, 0u,
                     0u, MARK_COUNT, inactive);
        state_scalar(cursor, &ss->objects, ts != NULL ? &ts->objects : NULL, 1u, 0, STATE_ANY, 0u,
                     0u, MARK_COUNT, inactive);
        state_scalar(cursor, &ss->flags, ts != NULL ? &ts->flags : NULL, 2u, 0, STATE_MASK,
                     MUHTARI__LINE_LAST | MUHTARI__LINE_ABORTED | MUHTARI__LINE_DLI |
                         MUHTARI__LINE_DLI_QUEUED | MUHTARI__LINE_DMA_BLOCKED |
                         MUHTARI__LINE_DMA_OFF,
                     0u, MARK_COUNT, inactive);
    }
    CHECK_U32(maria.features, STATE_MASK,
              MUHTARI__FEAT_RM0 | MUHTARI__FEAT_RM2 | MUHTARI__FEAT_RM3 | MUHTARI__FEAT_WRITE_MODE |
                  MUHTARI__FEAT_KANGAROO | MUHTARI__FEAT_HOLEY8 | MUHTARI__FEAT_HOLEY16 |
                  MUHTARI__FEAT_IND1 | MUHTARI__FEAT_IND2 | MUHTARI__FEAT_DLI |
                  MUHTARI__FEAT_DLI_QUEUED | MUHTARI__FEAT_DMA_BLOCKED,
              0u);
    CHECK_U64(maria.origin, STATE_ANY, 0u, 0u, MARK_MARIA_ORIGIN);
    U64(maria.dma_lrc);
    CHECK_U16(maria.event_line, STATE_MARIA_LINE, 0u, 0u);
    CHECK_U16(maria.dma_line, STATE_MARIA_LINE, 0u, 0u);
    CHECK_U8(maria.event_index, STATE_MARIA_EVENT, 0u, 0u);
    CHECK_U8(maria.mstat, STATE_MASK, 0x80u, 0u);
    CHECK_U16(maria.px, STATE_MARIA_PIXEL, 0u, 0u);
    CHECK_U8(cart.bank, STATE_CART_BANK, 0u, 0u);
    CHECK_U8(cart.ram_bank, STATE_CART_RAM_BANK, 0u, 0u);
    CHECK_U8(cart.expansion.control, STATE_EXPANSION_CONTROL, 0u, 0u);
    CHECK_U8(cart.expansion.cpu_pages, STATE_EXPANSION_PAGES, 0u, 0u);
    CHECK_U8(cart.expansion.maria_pages, STATE_EXPANSION_PAGES, 0u, 0u);
    for (unsigned p = 0u; p < 2u; p++) {
        for (unsigned a = 0u; a < 2u; a++) {
            const struct muhtari__motion *sm = &source->ports.motion[p][a];
            struct muhtari__motion *tm = target != NULL ? &target->ports.motion[p][a] : NULL;
            state_scalar(cursor, &sm->next_tick, tm != NULL ? &tm->next_tick : NULL, 8u, 0,
                         STATE_ANY, 0u, 0u, MARK_COUNT, 0);
            state_scalar(cursor, &sm->residual, tm != NULL ? &tm->residual : NULL, 4u, 1,
                         STATE_SIGNED_RANGE, (uint64_t)(int64_t)-255, 255u, MARK_COUNT, 0);
            state_scalar(cursor, &sm->phase, tm != NULL ? &tm->phase : NULL, 1u, 0, STATE_RANGE, 0u,
                         3u, MARK_COUNT, 0);
            state_scalar(cursor, &sm->negative, tm != NULL ? &tm->negative : NULL, 1u, 0,
                         STATE_BOOL, 0u, 0u, MARK_COUNT, 0);
        }
    }
    for (unsigned p = 0u; p < 2u; p++) {
        const struct muhtari__light *sl = &source->ports.light[p];
        struct muhtari__light *tl = target != NULL ? &target->ports.light[p] : NULL;
#define LIGHT_FIELD(name, width, rule, lower, upper)                                               \
    state_scalar(cursor, &sl->name, tl != NULL ? &tl->name : NULL, width, 0, rule, lower, upper,   \
                 MARK_COUNT, 0)
        LIGHT_FIELD(sample_tick, 8u, STATE_ANY, 0u, 0u);
        LIGHT_FIELD(response_tick, 8u, STATE_ANY, 0u, 0u);
        LIGHT_FIELD(response_until, 8u, STATE_ANY, 0u, 0u);
        LIGHT_FIELD(low_until, 8u, STATE_ANY, 0u, 0u);
        LIGHT_FIELD(sample_raster, 2u, STATE_RANGE, 0u, 312u);
        LIGHT_FIELD(sample_x, 2u, STATE_RANGE, 0u, MUHTARI__WIDTH - 1u);
        LIGHT_FIELD(delay, 1u, STATE_ANY, 0u, 0u);
        LIGHT_FIELD(width, 1u, STATE_ANY, 0u, 0u);
        LIGHT_FIELD(lines, 1u, STATE_ANY, 0u, 0u);
        LIGHT_FIELD(threshold, 1u, STATE_ANY, 0u, 0u);
        LIGHT_FIELD(low, 1u, STATE_BOOL, 0u, 0u);
#undef LIGHT_FIELD
    }
    for (unsigned p = 0u; p < 2u; p++) {
        for (unsigned a = 0u; a < 2u; a++) {
            const struct muhtari__pot *sp = &source->ports.pot[p][a];
            struct muhtari__pot *tp = target != NULL ? &target->ports.pot[p][a] : NULL;
#define POT_FIELD(name, width, rule, lower, upper)                                                 \
    state_scalar(cursor, &sp->name, tp != NULL ? &tp->name : NULL, width, 0, rule, lower, upper,   \
                 MARK_COUNT, 0)
            POT_FIELD(action, 8u, STATE_POT_ACTION, 0u, 0u);
            POT_FIELD(last_tick, 8u, STATE_ANY, 0u, 0u);
            POT_FIELD(crossing_tick, 8u, STATE_ANY, 0u, 0u);
            POT_FIELD(duration, 4u, STATE_ANY, 0u, 0u);
#undef POT_FIELD
        }
    }
    for (unsigned p = 0u; p < 2u; p++) {
        const struct muhtari__adapter *sa = &source->ports.adapter[p];
        struct muhtari__adapter *ta = target != NULL ? &target->ports.adapter[p] : NULL;
        state_scalar(cursor, &sa->snapshot, ta != NULL ? &ta->snapshot : NULL, 4u, 0, STATE_ANY, 0u,
                     0u, MARK_COUNT, 0);
        state_scalar(cursor, &sa->host, ta != NULL ? &ta->host : NULL, 1u, 0, STATE_MASK, 3u, 0u,
                     MARK_COUNT, 0);
        state_scalar(cursor, &sa->position, ta != NULL ? &ta->position : NULL, 1u, 0, STATE_RANGE,
                     0u, 16u, MARK_COUNT, 0);
        state_scalar(cursor, &sa->scanning, ta != NULL ? &ta->scanning : NULL, 1u, 0, STATE_BOOL,
                     0u, 0u, MARK_COUNT, 0);
    }
    for (unsigned p = 0u; p < 2u; p++) {
        state_scalar(cursor, &source->ports.keypad_keys[p],
                     target != NULL ? &target->ports.keypad_keys[p] : NULL, 2u, 0, STATE_MASK,
                     0x0FFFu, 0u, MARK_COUNT, 0);
    }
    for (unsigned p = 0u; p < 2u; p++) {
        state_scalar(cursor, &source->ports.keypad_low[p],
                     target != NULL ? &target->ports.keypad_low[p] : NULL, 1u, 0, STATE_MASK, 0x7Fu,
                     0u, MARK_COUNT, 0);
    }
    for (unsigned p = 0u; p < 2u; p++) {
        cursor->input_port = (uint8_t)p;
        cursor->input_device = (uint32_t)state_scalar(
            cursor, &source->ports.device[p], target != NULL ? &target->ports.device[p] : NULL, 4u,
            0, STATE_PORT_DEVICE, 0u, 0u, MARK_COUNT, 0);
    }
    for (unsigned p = 0u; p < 2u; p++) {
        state_scalar(cursor, &source->ports.buttons[p],
                     target != NULL ? &target->ports.buttons[p] : NULL, 4u, 0, STATE_MASK, 4095u,
                     0u, MARK_COUNT, 0);
    }
    CHECK_U32(ports.switches, STATE_MASK, 0x1Fu, 0u);
    for (unsigned p = 0u; p < 2u; p++) {
        cursor->input_port = (uint8_t)p;
        state_scalar(cursor, &source->ports.pending_device[p],
                     target != NULL ? &target->ports.pending_device[p] : NULL, 4u, 0,
                     STATE_PORT_DEVICE, 0u, 0u, MARK_COUNT, 0);
    }
    for (unsigned p = 0u; p < 2u; p++) {
        state_scalar(cursor, &source->ports.pending_buttons[p],
                     target != NULL ? &target->ports.pending_buttons[p] : NULL, 4u, 0, STATE_MASK,
                     0x3Fu, 0u, MARK_COUNT, 0);
    }
    CHECK_U32(ports.pending_switches, STATE_MASK, 0x1Fu, 0u);
    for (unsigned p = 0u; p < 2u; p++)
        state_input_port(cursor, p, &source->ports.pending_ex.port[p],
                         target != NULL ? &target->ports.pending_ex.port[p] : NULL);
    CHECK_U32(ports.pending_ex.switches, STATE_MASK, 0x1Fu, 0u);
    for (unsigned p = 0u; p < 2u; p++)
        state_input_port(cursor, p, &source->ports.applied_ex.port[p],
                         target != NULL ? &target->ports.applied_ex.port[p] : NULL);
    CHECK_U32(ports.applied_ex.switches, STATE_MASK, 0x1Fu, 0u);
    CHECK_U8(ports.input_mode, STATE_BOOL, 0u, 0u);
    CHECK_U8(ports.initialized, STATE_BOOL, 0u, 0u);
    CHECK_U8(ports.pending, STATE_BOOL, 0u, 0u);
    CHECK_U8(ports.input_read, STATE_BOOL, 0u, 0u);
    U64(ports.io_hazard);
    U64(boot.handoff_frame);
    CHECK_U8(boot.rejected, STATE_BOOL, 0u, 0u);

    CHECK_U64(synth.tick, STATE_ANY, 0u, 0u, MARK_SYNTH_TICK);
    CHECK_U64(synth.origin, STATE_ANY, 0u, 0u, MARK_SYNTH_ORIGIN);
    CHECK_U64(synth.horizon, STATE_ANY, 0u, 0u, MARK_SYNTH_HORIZON);
    CHECK_U64(synth.pending_tick, STATE_ANY, 0u, 0u, MARK_SYNTH_PENDING_TICK);
    for (unsigned i = 0u; i < 16u; i++) {
        state_scalar(cursor, &source->synth.impulse[i],
                     target != NULL ? &target->synth.impulse[i] : NULL, 8u, 1, STATE_SIGNED_RANGE,
                     (uint64_t)(int64_t)-STATE_SYNTH_IMPULSE_LIMIT, STATE_SYNTH_IMPULSE_LIMIT,
                     MARK_COUNT, 0);
    }
    FIELD(synth.integrator_q23, 8u, 1, STATE_SIGNED_RANGE,
          (uint64_t)(int64_t)-STATE_SYNTH_INTEGRATOR_LIMIT, STATE_SYNTH_INTEGRATOR_LIMIT,
          MARK_COUNT);
    FIELD(synth.previous_q30, 8u, 1, STATE_SIGNED_RANGE,
          (uint64_t)(int64_t)-STATE_SYNTH_PREVIOUS_Q30_LIMIT, STATE_SYNTH_PREVIOUS_Q30_LIMIT,
          MARK_COUNT);
    FIELD(synth.blocker_q30, 8u, 1, STATE_SIGNED_RANGE,
          (uint64_t)(int64_t)-STATE_SYNTH_BLOCKER_Q30_LIMIT, STATE_SYNTH_BLOCKER_Q30_LIMIT,
          MARK_COUNT);
    U64(synth.pre_clip_peak);
    U64(synth.clipped_samples);
    FIELD(synth.phase, 4u, 0, STATE_RANGE, 0u, muhtari__timing(source).sample_den - 1u,
          MARK_SYNTH_PHASE);
    uint64_t saved_pcm_read = state_scalar(cursor, &source->synth.pcm_read,
                                           target != NULL ? &target->synth.pcm_read : NULL, 4u, 0,
                                           STATE_SYNTH_PCM_READ, 0u, 0u, MARK_COUNT, 0);
    uint64_t saved_pcm_count = state_scalar(cursor, &source->synth.pcm_count,
                                            target != NULL ? &target->synth.pcm_count : NULL, 4u, 0,
                                            STATE_SYNTH_PCM_COUNT, 0u, 0u, MARK_COUNT, 0);
    uint32_t pcm_read =
        cursor->mode == STATE_SIZE ? source->synth.pcm_read : (uint32_t)saved_pcm_read;
    uint32_t pcm_count =
        cursor->mode == STATE_SIZE ? source->synth.pcm_count : (uint32_t)saved_pcm_count;
    FIELD(synth.sample_num, 4u, 0, STATE_ANY, 0u, 0u, MARK_SYNTH_SAMPLE_NUM);
    FIELD(synth.sample_den, 4u, 0, STATE_ANY, 0u, 0u, MARK_SYNTH_SAMPLE_DEN);
    FIELD(synth.max_samples, 4u, 0, STATE_ANY, 0u, 0u, MARK_SYNTH_MAX_SAMPLES);
    U32(synth.kernel_adds);
    U32(synth.kernel_adds_per_sample);
    U32(synth.carry_high_water);
    FIELD(synth.mixed_level, 4u, 1, STATE_SIGNED_RANGE,
          (uint64_t)(int64_t)-MUHTARI__SYNTH_MAX_LEVEL, MUHTARI__SYNTH_MAX_LEVEL, MARK_COUNT);
    for (unsigned i = 0u; i < 10u; i++) {
        cursor->field_index = (uint8_t)i;
        state_scalar(cursor, &source->synth.raw[i], target != NULL ? &target->synth.raw[i] : NULL,
                     2u, 0, STATE_SYNTH_RAW, 0u, 0u, MARK_COUNT, 0);
    }
    CHECK_U8(synth.head, STATE_SYNTH_HEAD, 0u, 0u);
    FIELD(synth.pending, 1u, 0, STATE_BOOL, 0u, 0u, MARK_SYNTH_PENDING);
    FIELD(synth.pokeys, 1u, 0, STATE_RANGE, 0u, 2u, MARK_SYNTH_POKEYS);
    FIELD(synth.secondary, 1u, 0, STATE_RANGE, 0u, 1u, MARK_SYNTH_SECONDARY);
    for (unsigned i = 0u; i < MUHTARI__SYNTH_PCM_CAPACITY; i++) {
        /* During independent restore validation, the ring's active window
           belongs to the candidate bytes, not the destination's live cursor. */
        uint32_t distance =
            (i + MUHTARI__SYNTH_PCM_CAPACITY - pcm_read) % MUHTARI__SYNTH_PCM_CAPACITY;
        int inactive = distance >= pcm_count;
        state_scalar(cursor, &source->synth.pcm[i], target != NULL ? &target->synth.pcm[i] : NULL,
                     2u, 1, STATE_ANY, 0u, 0u, MARK_COUNT, inactive);
    }
    CHECK_U64(tia.next_tick, STATE_ANY, 0u, 0u, MARK_TIA_NEXT_TICK);
    CHECK_U64(tia.epoch_tick, STATE_ANY, 0u, 0u, MARK_TIA_EPOCH_TICK);
    CHECK_U16(tia.epoch_phase, STATE_TIA_EPOCH_PHASE, 0u, 0u);
    CHECK_U8(tia.phase, STATE_RANGE, 0u, 3u);
    CHECK_U8(tia.initialized, STATE_BOOL, 0u, 0u);
    CHECK_U8(tia.input_latch_enable, STATE_BOOL, 0u, 0u);
    CHECK_U8(tia.input_dump, STATE_BOOL, 0u, 0u);
    for (unsigned i = 0u; i < 2u; i++) {
        state_scalar(cursor, &source->tia.input_latch[i],
                     target != NULL ? &target->tia.input_latch[i] : NULL, 1u, 0, STATE_BOOL, 0u, 0u,
                     MARK_COUNT, 0);
    }
    for (unsigned ch = 0u; ch < 2u; ch++) {
        const struct muhtari__tia_channel *sc = &source->tia.channel[ch];
        struct muhtari__tia_channel *tc = target != NULL ? &target->tia.channel[ch] : NULL;
#define TIA_FIELD(name, rule, lower, upper)                                                        \
    state_scalar(cursor, &sc->name, tc != NULL ? &tc->name : NULL, 1u, 0, rule, lower, upper,      \
                 MARK_COUNT, 0)
        TIA_FIELD(audc, STATE_RANGE, 0u, 15u);
        TIA_FIELD(audf, STATE_RANGE, 0u, 31u);
        TIA_FIELD(audv, STATE_RANGE, 0u, 15u);
        TIA_FIELD(count, STATE_RANGE, 0u, 31u);
        TIA_FIELD(enabled, STATE_BOOL, 0u, 0u);
        TIA_FIELD(output, STATE_BOOL, 0u, 0u);
        TIA_FIELD(noise, STATE_RANGE, 0u, 31u);
        TIA_FIELD(pulse, STATE_RANGE, 0u, 15u);
        TIA_FIELD(noise_input, STATE_BOOL, 0u, 0u);
        TIA_FIELD(pulse_gate, STATE_BOOL, 0u, 0u);
#undef TIA_FIELD
    }

    state_pokey(cursor, &source->pokey[0], target != NULL ? &target->pokey[0] : NULL, 0u);
    state_pokey(cursor, &source->pokey[1], target != NULL ? &target->pokey[1] : NULL, 1u);
    CHECK_U64(savekey.busy_until, STATE_ANY, 0u, 0u, MARK_SAVEKEY_BUSY_UNTIL);
    CHECK_U16(savekey.address, STATE_SAVEKEY_ADDRESS, 0u, 0u);
    CHECK_U16(savekey.page_base, STATE_SAVEKEY_PAGE_BASE, 0u, 0u);
    CHECK_U8(savekey.host_scl, STATE_BOOL, 0u, 0u);
    CHECK_U8(savekey.host_sda, STATE_BOOL, 0u, 0u);
    CHECK_U8(savekey.sda_low, STATE_BOOL, 0u, 0u);
    CHECK_U8(savekey.phase, STATE_SAVEKEY_PHASE, 0u, 0u);
    CHECK_U8(savekey.next_phase, STATE_SAVEKEY_PHASE, 0u, 0u);
    CHECK_U8(savekey.bits, STATE_SAVEKEY_BITS, 0u, 0u);
    U8(savekey.shift);
    CHECK_U8(savekey.address_high, STATE_MASK, 0x7Fu, 0u);
    U8(savekey.read_byte);
    state_bytes(cursor, source->savekey.page, target != NULL ? target->savekey.page : NULL,
                sizeof source->savekey.page, SIZE_MAX);
    for (unsigned i = 0u; i < sizeof source->savekey.touched; i++) {
        state_scalar(cursor, &source->savekey.touched[i],
                     target != NULL ? &target->savekey.touched[i] : NULL, 1u, 0, STATE_BOOL, 0u, 0u,
                     MARK_COUNT, 0);
    }
    CHECK_U8(savekey.page_offset, STATE_RANGE, 0u, 63u);
    FIELD(savekey.pending, 1u, 0, STATE_BOOL, 0u, 0u, MARK_SAVEKEY_PENDING);
    CHECK_U8(savekey.ack, STATE_BOOL, 0u, 0u);
    if (cursor->offset != STATE_FIXED_BYTES)
        cursor->valid = 0u;
    size_t cart_ram = source->cart.info.ram_size;
    size_t expansion_ram =
        source->cart.expansion.kind == MUHTARI_EXPANSION_OFF ? 0u : STATE_EXPANSION_BYTES;
    state_bytes(cursor, source->cart.ram, target != NULL ? target->cart.ram : NULL, cart_ram,
                SIZE_MAX);
    state_bytes(cursor, source->cart.expansion.ram,
                target != NULL ? target->cart.expansion.ram : NULL, expansion_ram, SIZE_MAX);
    state_bytes(cursor, source->battery.data, target != NULL ? target->battery.data : NULL,
                source->battery.bytes, SIZE_MAX);

#undef CHECK_U64
#undef CHECK_U32
#undef CHECK_U16
#undef CHECK_U8
#undef U64
#undef U32
#undef U16
#undef U8
#undef FIELD
}

static int state_relationships(const struct state_cursor *cursor)
{
    const uint64_t *v = cursor->marks;
    if (v[MARK_LAG_FRAMES] > v[MARK_FRAMES] || v[MARK_FRAME_START] > v[MARK_FRAME_DEADLINE] ||
        v[MARK_FRAME_START] > v[MARK_BUS_TICKS] || v[MARK_BUS_TICKS] > v[MARK_FRAME_DEADLINE] ||
        v[MARK_CPU_PHI2] > v[MARK_BUS_PHI2] || v[MARK_BUS_ORIGIN_TICK] > v[MARK_BUS_TICKS] ||
        v[MARK_BUS_ORIGIN_PHI2] > v[MARK_BUS_PHI2] || v[MARK_MARIA_ORIGIN] > v[MARK_BUS_TICKS] ||
        v[MARK_SYNTH_ORIGIN] > v[MARK_SYNTH_HORIZON] ||
        v[MARK_SYNTH_HORIZON] > v[MARK_SYNTH_TICK] ||
        (v[MARK_SYNTH_PHASE] >= v[MARK_SYNTH_SAMPLE_DEN]) || v[MARK_SYNTH_SAMPLE_NUM] == 0u ||
        v[MARK_SYNTH_SAMPLE_DEN] == 0u || v[MARK_AUDIO_REM] >= v[MARK_SYNTH_SAMPLE_DEN])
        return 0;
    if (v[MARK_SYNTH_SAMPLE_NUM] != muhtari__timing(cursor->source).sample_num ||
        v[MARK_SYNTH_SAMPLE_DEN] != muhtari__timing(cursor->source).sample_den ||
        v[MARK_SYNTH_MAX_SAMPLES] != muhtari__timing(cursor->source).max_samples)
        return 0;
    if (v[MARK_SYNTH_PENDING] != 0u && v[MARK_SYNTH_PENDING_TICK] < v[MARK_SYNTH_TICK])
        return 0;
    if (v[MARK_SYNTH_POKEYS] != 1u && v[MARK_SYNTH_SECONDARY] != 0u)
        return 0;
    return 1;
}

/* The private payload visitor preserves the complete u64 representation.
   Public restore also has to prove that the next frame cannot turn those
   values into hostile catch-up work or unchecked clock overflow. Each
   margin below comes from a current producer: one frame plus the documented
   instruction overshoot, the u32 paddle-duration domain, and two frame
   deadlines so a restored frame can complete and advance its next deadline. */
static int state_public_resume_safe(const muhtari *m, const uint64_t *v)
{
    const struct muhtari__timing timing = muhtari__timing(m);
    const uint64_t frame = timing.frame_ticks;
    const uint64_t overshoot = MUHTARI__SYNTH_OVERSHOOT_TICKS;
    const uint64_t clock_margin = 2u * frame + overshoot + UINT32_MAX;
    const uint64_t tick = v[MARK_BUS_TICKS];
    const uint64_t phi2 = v[MARK_BUS_PHI2];
    const uint64_t frame_start = v[MARK_FRAME_START];
    const uint64_t deadline = v[MARK_FRAME_DEADLINE];

    if (deadline - frame_start > frame || deadline > UINT64_MAX - frame ||
        tick > UINT64_MAX - clock_margin || phi2 > UINT64_MAX - (clock_margin / 4u + 2u))
        return 0;
    if (tick - v[MARK_MARIA_ORIGIN] > frame + overshoot || v[MARK_TIA_EPOCH_TICK] > tick)
        return 0;

    uint64_t tia_next = v[MARK_TIA_NEXT_TICK];
    if ((tia_next > tick && tia_next - tick > 168u) || (tia_next <= tick && tick - tia_next > 168u))
        return 0;

    uint64_t synth_tick = v[MARK_SYNTH_TICK];
    if ((synth_tick > tick && synth_tick - tick > overshoot) ||
        (synth_tick <= tick && tick - synth_tick > frame + overshoot))
        return 0;
    if (v[MARK_SYNTH_PENDING] != 0u && v[MARK_SYNTH_PENDING_TICK] > tick &&
        v[MARK_SYNTH_PENDING_TICK] - tick > overshoot)
        return 0;

    if (v[MARK_SYNTH_POKEYS] != 0u) {
        uint64_t first_cycle = v[MARK_POKEY_CYCLE_0];
        uint64_t first_tick = v[MARK_POKEY_TICK_0];
        if ((first_cycle > phi2 && first_cycle - phi2 > 1u) ||
            (first_cycle <= phi2 && phi2 - first_cycle > 1u))
            return 0;
        if ((first_tick > tick && first_tick - tick > 2u) ||
            (first_tick <= tick && tick - first_tick > 2u) ||
            (v[MARK_SYNTH_POKEYS] > 1u &&
             ((v[MARK_POKEY_CYCLE_1] > phi2 && v[MARK_POKEY_CYCLE_1] - phi2 > 1u) ||
              (v[MARK_POKEY_CYCLE_1] <= phi2 && phi2 - v[MARK_POKEY_CYCLE_1] > 1u) ||
              (v[MARK_POKEY_TICK_1] > tick && v[MARK_POKEY_TICK_1] - tick > 2u) ||
              (v[MARK_POKEY_TICK_1] <= tick && tick - v[MARK_POKEY_TICK_1] > 2u))))
            return 0;
    }

    if (v[MARK_RIOT_TIMER_PHI2] > phi2 + 1u)
        return 0;
    if (v[MARK_BUS_RDY_RELEASE] > tick && v[MARK_BUS_RDY_RELEASE] - tick > 456u)
        return 0;
    if (v[MARK_BUS_NMI_PENDING] != 0u && v[MARK_BUS_NMI_AT] > tick &&
        v[MARK_BUS_NMI_AT] - tick > 454u)
        return 0;
    if (v[MARK_BUS_HALT_STAGE] == 1u && v[MARK_BUS_HALT_AT] > tick &&
        v[MARK_BUS_HALT_AT] - tick > 454u)
        return 0;
    if (v[MARK_BUS_HALT_STAGE] != 0u && v[MARK_BUS_HALT_UNTIL] != MUHTARI__HALT_UNTIL_MARIA &&
        v[MARK_BUS_HALT_UNTIL] > tick && v[MARK_BUS_HALT_UNTIL] - tick > 454u)
        return 0;
    if (v[MARK_BUS_HALT_STAGE] == 2u) {
        if (v[MARK_BUS_HALT_FALL] > tick || tick - v[MARK_BUS_HALT_FALL] > 12u)
            return 0;
    }

    if (v[MARK_SAVEKEY_PENDING] != 0u) {
        uint64_t busy = v[MARK_SAVEKEY_BUSY_UNTIL];
        uint64_t duration =
            m->effective_region == MUHTARI_REGION_PAL ? 35469u : MUHTARI__SAVEKEY_BUSY_TICKS;
        if (busy <= tick || busy - tick > duration)
            return 0;
    }
    return 1;
}

static int state_topology_valid(const muhtari *m)
{
    if (m == NULL || m->loaded == 0u || m->cart.rom == NULL || m->cart.info.rom_size == 0u ||
        m->battery.bytes > sizeof m->battery.data)
        return 0;
    if (m->cart.expansion.kind != MUHTARI_EXPANSION_OFF &&
        m->cart.expansion.kind != MUHTARI_EXPANSION_XBOARD &&
        m->cart.expansion.kind != MUHTARI_EXPANSION_XM)
        return 0;
    size_t expansion_bytes =
        m->cart.expansion.kind == MUHTARI_EXPANSION_OFF ? 0u : STATE_EXPANSION_BYTES;
    size_t expected_allocation = m->cart.info.rom_size;
    if (m->cart.info.ram_size > SIZE_MAX - expected_allocation)
        return 0;
    expected_allocation += m->cart.info.ram_size;
    if (expansion_bytes > SIZE_MAX - expected_allocation)
        return 0;
    expected_allocation += expansion_bytes;
    if (m->cart.allocation_bytes != expected_allocation)
        return 0;
    uintptr_t rom = (uintptr_t)m->cart.rom;
    if (expected_allocation > UINTPTR_MAX - rom)
        return 0;
    uintptr_t ram = rom + m->cart.info.rom_size;
    if ((m->cart.info.ram_size != 0u && (uintptr_t)m->cart.ram != ram) ||
        (m->cart.info.ram_size == 0u && m->cart.ram != NULL))
        return 0;
    if (m->cart.info.ram_size > UINTPTR_MAX - ram)
        return 0;
    uintptr_t expansion = ram + m->cart.info.ram_size;
    if ((expansion_bytes != 0u && (uintptr_t)m->cart.expansion.ram != expansion) ||
        (expansion_bytes == 0u && m->cart.expansion.ram != NULL))
        return 0;
    if (m->synth.sample_num != muhtari__timing(m).sample_num ||
        m->synth.sample_den != muhtari__timing(m).sample_den ||
        m->synth.max_samples != muhtari__timing(m).max_samples)
        return 0;
    return 1;
}

static int state_buffer_disjoint(const muhtari *m, const void *buffer, size_t bytes)
{
    uintptr_t start = (uintptr_t)buffer;
    if (bytes > UINTPTR_MAX - start)
        return 0;
    uintptr_t end = start + bytes;
    uintptr_t instance = (uintptr_t)m;
    if (sizeof *m > UINTPTR_MAX - instance)
        return 0;
    uintptr_t instance_end = instance + sizeof *m;
    if (start < instance_end && instance < end)
        return 0;
    if (m->cart.allocation_bytes != 0u) {
        uintptr_t allocation = (uintptr_t)m->cart.rom;
        if (m->cart.rom == NULL || m->cart.allocation_bytes > UINTPTR_MAX - allocation)
            return 0;
        uintptr_t allocation_end = allocation + m->cart.allocation_bytes;
        if (start < allocation_end && allocation < end)
            return 0;
    }
    return 1;
}

static int state_walk(const muhtari *m, muhtari *target, enum state_mode mode, const uint8_t *input,
                      uint8_t *output, size_t size)
{
    struct state_cursor cursor;
    memset(&cursor, 0, sizeof cursor);
    cursor.mode = mode;
    cursor.source = m;
    cursor.input = input;
    cursor.output = output;
    cursor.limit = size;
    cursor.valid = 1u;
    state_visit(m, target, &cursor);
    if (cursor.offset != size)
        cursor.valid = 0u;
    if (cursor.valid != 0u &&
        (mode == STATE_VALIDATE || mode == STATE_PUBLIC_VALIDATE || mode == STATE_LIVE ||
         mode == STATE_PUBLIC_LIVE) &&
        state_relationships(&cursor) == 0)
        cursor.valid = 0u;
    if (cursor.valid != 0u && (mode == STATE_PUBLIC_VALIDATE || mode == STATE_PUBLIC_LIVE) &&
        state_public_resume_safe(m, cursor.marks) == 0)
        cursor.valid = 0u;
    return cursor.valid != 0u;
}

size_t muhtari__state_payload_size(const muhtari *m)
{
    if (state_topology_valid(m) == 0)
        return 0u;
    struct state_cursor cursor;
    memset(&cursor, 0, sizeof cursor);
    cursor.mode = STATE_SIZE;
    cursor.source = m;
    cursor.valid = 1u;
    state_visit(m, NULL, &cursor);
    if (cursor.valid == 0u || cursor.offset > UINT32_MAX)
        return 0u;
    return cursor.offset;
}

int muhtari__state_payload_write(const muhtari *m, uint8_t *bytes, size_t size)
{
    size_t expected = muhtari__state_payload_size(m);
    if (expected == 0u || bytes == NULL || size != expected ||
        state_buffer_disjoint(m, bytes, size) == 0 ||
        state_walk(m, NULL, STATE_LIVE, NULL, NULL, size) == 0)
        return 0;
    return state_walk(m, NULL, STATE_WRITE, NULL, bytes, size);
}

int muhtari__state_payload_validate(const muhtari *m, const uint8_t *bytes, size_t size)
{
    if (state_topology_valid(m) == 0 || bytes == NULL || size != muhtari__state_payload_size(m) ||
        state_buffer_disjoint(m, bytes, size) == 0)
        return 0;
    return state_walk(m, NULL, STATE_VALIDATE, bytes, NULL, size);
}

void muhtari__state_payload_apply(muhtari *m, const uint8_t *bytes, size_t size)
{
    if (state_topology_valid(m) == 0 || muhtari__state_payload_validate(m, bytes, size) == 0)
        return;
    size_t battery_at =
        STATE_FIXED_BYTES + (size_t)m->cart.info.ram_size +
        (m->cart.expansion.kind == MUHTARI_EXPANSION_OFF ? 0u : STATE_EXPANSION_BYTES);
    int battery_changed = memcmp(m->battery.data, bytes + battery_at, m->battery.bytes) != 0;
    (void)state_walk(m, m, STATE_APPLY, bytes, NULL, size);
    m->ports.pending_ex.size = (uint32_t)sizeof m->ports.pending_ex;
    m->ports.applied_ex.size = (uint32_t)sizeof m->ports.applied_ex;
    m->maria.rows = NULL;
    m->maria.pitch = 0u;
    if (battery_changed != 0)
        m->battery.generation++;
    muhtari__ports_rearm(m);
    muhtari__bus_rearm(m);
}

static uint32_t state_crc32(const uint8_t *bytes, size_t size)
{
    static const uint8_t zero_crc[4] = {0u, 0u, 0u, 0u};
    uint32_t crc = muhtari__crc32(0u, bytes, 24u);
    crc = muhtari__crc32(crc, zero_crc, sizeof zero_crc);
    return muhtari__crc32(crc, bytes + 28u, size - 28u);
}

size_t muhtari_state_size(const muhtari *m)
{
    size_t payload = muhtari__state_payload_size(m);
    if (payload == 0u || payload > SIZE_MAX - STATE_HEADER_BYTES ||
        payload + STATE_HEADER_BYTES > UINT32_MAX)
        return 0u;
    return payload + STATE_HEADER_BYTES;
}

muhtari_status muhtari_save_state(const muhtari *m, void *buffer, size_t capacity)
{
    static const uint8_t magic[8] = {'M', 'U', 'H', 'T', 'S', 'T', '0', '1'};
    if (m == NULL || buffer == NULL)
        return MUHTARI_STATUS_ERR_ARGUMENT;
    if (m->loaded == 0u)
        return MUHTARI_STATUS_ERR_STATE_UNAVAILABLE;

    size_t total = muhtari_state_size(m);
    if (total == 0u)
        return MUHTARI_STATUS_ERR_INVALID_STATE;
    if (capacity < total)
        return MUHTARI_STATUS_ERR_BUFFER_TOO_SMALL;
    if (state_buffer_disjoint(m, buffer, total) == 0)
        return MUHTARI_STATUS_ERR_ARGUMENT;

    size_t payload = total - STATE_HEADER_BYTES;
    if (state_walk(m, NULL, STATE_PUBLIC_LIVE, NULL, NULL, payload) == 0)
        return MUHTARI_STATUS_ERR_INVALID_STATE;

    uint8_t *output = (uint8_t *)buffer;
    /* SIZE and LIVE have proved this fixed visitor's exact extent and every
       live field before WRITE touches caller memory. WRITE has no fallible
       branch for that same source-defined layout. */
    (void)state_walk(m, NULL, STATE_WRITE, NULL, output + STATE_HEADER_BYTES, payload);
    memcpy(output, magic, sizeof magic);
    state_put_u32(output + 8u, STATE_FORMAT);
    state_put_u32(output + 12u, STATE_HEADER_BYTES);
    state_put_u32(output + 16u, (uint32_t)total);
    state_put_u32(output + 20u, STATE_BEHAVIOUR_REVISION);
    state_put_u32(output + 24u, 0u);
    memcpy(output + 28u, m->state_identity, sizeof m->state_identity);
    state_put_u32(output + 60u, 0u);
    state_put_u32(output + 24u, state_crc32(output, total));
    return MUHTARI_STATUS_OK;
}

muhtari_status muhtari_load_state(muhtari *m, const void *buffer, size_t bytes)
{
    static const uint8_t magic[8] = {'M', 'U', 'H', 'T', 'S', 'T', '0', '1'};
    if (m == NULL || buffer == NULL)
        return MUHTARI_STATUS_ERR_ARGUMENT;
    if (m->loaded == 0u)
        return MUHTARI_STATUS_ERR_STATE_UNAVAILABLE;

    size_t expected = muhtari_state_size(m);
    if (expected == 0u || bytes != expected)
        return MUHTARI_STATUS_ERR_INVALID_STATE;
    if (state_buffer_disjoint(m, buffer, expected) == 0)
        return MUHTARI_STATUS_ERR_ARGUMENT;

    const uint8_t *input = (const uint8_t *)buffer;
    if (memcmp(input, magic, sizeof magic) != 0 ||
        state_get_u32(input + 12u) != STATE_HEADER_BYTES || state_get_u32(input + 16u) != expected)
        return MUHTARI_STATUS_ERR_INVALID_STATE;
    if (state_get_u32(input + 8u) != STATE_FORMAT ||
        state_get_u32(input + 20u) != STATE_BEHAVIOUR_REVISION)
        return MUHTARI_STATUS_ERR_UNSUPPORTED_STATE;
    if (state_get_u32(input + 60u) != 0u ||
        state_get_u32(input + 24u) != state_crc32(input, expected))
        return MUHTARI_STATUS_ERR_INVALID_STATE;
    if (memcmp(input + 28u, m->state_identity, sizeof m->state_identity) != 0)
        return MUHTARI_STATUS_ERR_STATE_MISMATCH;

    size_t payload = expected - STATE_HEADER_BYTES;
    if (state_walk(m, NULL, STATE_PUBLIC_VALIDATE, input + STATE_HEADER_BYTES, NULL, payload) == 0)
        return MUHTARI_STATUS_ERR_INVALID_STATE;
    muhtari__state_payload_apply(m, input + STATE_HEADER_BYTES, payload);
    return MUHTARI_STATUS_OK;
}
