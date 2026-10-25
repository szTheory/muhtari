/* The controller ports as the host sets them, the pins their devices drive,
   the memory spans and the frame counters (docs/embedding.md, Input, Memory
   and Counters; docs/hardware/controllers.md). src/riot.c and src/tia.c read
   the pins. */
#include <string.h>

#include "internal.h"

/* The bits a host may set. */
#define PORTS_BUTTONS 0x3Fu  /* MUHTARI_BUTTON_UP .. MUHTARI_BUTTON_2 */
#define PORTS_SWITCHES 0x1Fu /* MUHTARI_SWITCH_RESET .. _RIGHT_DIFFICULTY_A */

/* Compatibility policy, not measured wheel calibration; controllers.md,
   Driving gives the one-line cadence and discarded excess rationale. */
#define PORTS_MOTION_CADENCE_TICKS 454u
#define PORTS_MOTION_RESIDUAL_MAX 255

#define PORTS_CHARGED (UINT64_C(1) << 32)

static void ports_rearm(muhtari *m);

/* Input DDR bits release high. Response pins never feed host clocks
   (controllers.md, SNES2Atari). */
static uint8_t ports_host(const muhtari *m, unsigned p)
{
    uint8_t drive = (uint8_t)(m->riot.ora | (uint8_t)~m->riot.ddra);
    return (uint8_t)((drive >> (p == 0u ? 4u : 0u)) & 3u);
}

void muhtari__ports_host_pins(muhtari *m, uint64_t tick)
{
    muhtari__ports_catch_up(m, tick);
    for (unsigned p = 0u; p < 2u; p++) {
        struct muhtari__adapter *s = &m->ports.adapter[p];
        uint8_t host = ports_host(m, p);
        if (muhtari__ports_device(m, p) == MUHTARI_PORT_SNES2ATARI) {
            if ((host & 2u) != 0u && (s->host & 2u) == 0u) {
                s->snapshot = m->ports.buttons[p];
                s->position = 0u;
            } else if ((host & 2u) == 0u && (host & 1u) != 0u && (s->host & 1u) == 0u &&
                       s->position < 16u) {
                s->position++;
            }
        } else if (muhtari__ports_device(m, p) == MUHTARI_PORT_MEGA7800) {
            /* Pin1 releases/aborts, pin2 rising advances. Low duplicates
               cannot reenter a scan (controllers.md, Mega7800). */
            if ((host & 1u) != 0u) {
                s->scanning = 0u;
                s->position = 0u;
            } else if (s->scanning == 0u && host == 0u && s->host != 0u) {
                s->scanning = 1u;
                s->position = 0u;
            } else if (s->scanning != 0u && (host & 2u) != 0u && (s->host & 2u) == 0u &&
                       s->position < 6u) {
                s->position++;
            }
        }
        s->host = host;
    }
}

/* Four rows and three columns, without diodes. Low dominates mixed
   driven rows; this is the compatibility policy in controllers.md, Keypad.
   A low can traverse at most six edges of this seven-node network. */
void muhtari__ports_rows(muhtari *m, uint64_t tick)
{
    for (unsigned p = 0u; p < 2u; p++) {
        if (muhtari__ports_device(m, p) != MUHTARI_PORT_KEYPAD)
            continue;
        unsigned shift = p == 0u ? 4u : 0u;
        uint8_t low = (uint8_t)((m->riot.ddra & (uint8_t)~m->riot.ora) >> shift) & 15u;
        if (m->tia.input_dump != 0u)
            low |= 0x30u;
        for (unsigned pass = 0u; pass < 7u; pass++) {
            uint8_t before = low;
            for (unsigned key = 0u; key < 12u; key++) {
                uint8_t pair = (uint8_t)((1u << (key / 3u)) | (1u << (4u + key % 3u)));
                if ((m->ports.keypad_keys[p] & (1u << key)) != 0u && (low & pair) != 0u)
                    low |= pair;
            }
            if (low == before)
                break;
        }
        for (unsigned n = 0u; n < 2u; n++) {
            uint8_t bit = (uint8_t)(0x10u << n);
            struct muhtari__pot *pot = &m->ports.pot[p][n];
            if ((low & bit) != 0u) {
                pot->action = 0u;
            } else if ((m->ports.keypad_low[p] & bit) != 0u) {
                pot->crossing_tick = tick + 2864u;
            }
        }
        m->ports.keypad_low[p] = low;
    }
    ports_rearm(m);
}

/* 7800 10k series + 1M pot, 68nF and fitted comparator threshold:
   controllers.md, Paddles. Ceil to whole master ticks. */
static uint32_t ports_pot_duration(uint32_t position)
{
    uint64_t resistance = 10000u + (uint64_t)position * 1000000u / 65535u;
    return (uint32_t)((resistance * 187747u + 999999u) / 1000000u);
}

static void ports_pot_project(struct muhtari__pot *pot, uint64_t tick)
{
    uint64_t remaining = PORTS_CHARGED - pot->action;
    pot->crossing_tick = tick + (remaining * pot->duration + PORTS_CHARGED - 1u) / PORTS_CHARGED;
}

/* Materialize only on electrical/setting changes. Bound elapsed before
   multiplying; reads never round or move this epoch (controllers.md). */
static void ports_pot_materialize(struct muhtari__pot *pot, uint64_t tick, uint8_t dump)
{
    if (dump == 0u && pot->duration != 0u) {
        if (tick >= pot->crossing_tick) {
            pot->action = PORTS_CHARGED;
        } else {
            pot->action += (tick - pot->last_tick) * PORTS_CHARGED / pot->duration;
        }
    }
    pot->last_tick = tick;
}

void muhtari__ports_dump(muhtari *m, uint64_t tick)
{
    for (unsigned p = 0u; p < 2u; p++) {
        for (unsigned n = 0u; n < 2u; n++) {
            struct muhtari__pot *pot = &m->ports.pot[p][n];
            pot->action = 0u;
            pot->last_tick = tick;
            ports_pot_project(pot, tick);
        }
    }
    muhtari__ports_rows(m, tick);
}

void muhtari__ports_reset(muhtari *m)
{
    memset(m->ports.motion, 0, sizeof m->ports.motion);
    memset(m->ports.light, 0, sizeof m->ports.light);
    memset(m->ports.pot, 0, sizeof m->ports.pot);
    memset(m->ports.keypad_low, 0, sizeof m->ports.keypad_low);
    memset(m->ports.adapter, 0, sizeof m->ports.adapter);
    for (unsigned p = 0u; p < 2u; p++) {
        m->ports.adapter[p].host = 3u;
        for (unsigned n = 0u; n < 2u; n++) {
            m->ports.pot[p][n].duration = ports_pot_duration(m->ports.applied_ex.port[p].paddle[n]);
            ports_pot_project(&m->ports.pot[p][n], 0u);
            if (muhtari__ports_device(m, p) == MUHTARI_PORT_KEYPAD)
                m->ports.pot[p][n].crossing_tick = 2864u;
        }
    }
    m->ports.next_tick = 0u;
    /* Power-on clears TIA dump after resetting ports. An explicit load
       choice must charge even before the first host packet. */
    for (unsigned p = 0u; p < 2u; p++) {
        uint32_t device = muhtari__ports_device(m, p);
        if (device == MUHTARI_PORT_PADDLE || device == MUHTARI_PORT_KEYPAD) {
            for (unsigned n = 0u; n < 2u; n++) {
                uint64_t at = m->ports.pot[p][n].crossing_tick;
                if (m->ports.next_tick == 0u || at < m->ports.next_tick)
                    m->ports.next_tick = at;
            }
        }
    }
}

uint64_t muhtari__ports_next_event(const muhtari *m)
{
    return m->ports.next_tick != 0u ? m->ports.next_tick : UINT64_MAX;
}

static void ports_rearm(muhtari *m)
{
    uint64_t next = UINT64_MAX;
    for (unsigned p = 0u; p < 2u; p++) {
        for (unsigned n = 0u; n < 2u; n++) {
            uint64_t at = m->ports.motion[p][n].next_tick;
            if (at != 0u && at < next)
                next = at;
        }
        const struct muhtari__light *light = &m->ports.light[p];
        uint32_t device = muhtari__ports_device(m, p);
        if ((device == MUHTARI_PORT_PADDLE || device == MUHTARI_PORT_KEYPAD) &&
            m->tia.input_dump == 0u) {
            for (unsigned n = 0u; n < 2u; n++) {
                const struct muhtari__pot *pot = &m->ports.pot[p][n];
                if (pot->action < PORTS_CHARGED && pot->crossing_tick < next &&
                    (device != MUHTARI_PORT_KEYPAD ||
                     (m->ports.keypad_low[p] & (0x10u << n)) == 0u)) {
                    next = pot->crossing_tick;
                }
            }
        }
        const uint64_t deadlines[] = {light->sample_tick, light->response_tick, light->low_until};
        for (unsigned i = 0u; i < 3u; i++) {
            if (deadlines[i] != 0u && deadlines[i] < next) {
                next = deadlines[i];
            }
        }
    }
    m->ports.next_tick = next != UINT64_MAX ? next : 0u;
}

/* One relevant emitted pixel per eligible raster. The centered aperture
   clips to native rows; all optical constants are fits (controllers.md). */
static void ports_light_rearm(muhtari *m, unsigned p, uint64_t after)
{
    struct muhtari__light *light = &m->ports.light[p];
    const muhtari_port_input *in = &m->ports.applied_ex.port[p];
    light->sample_tick = 0u;
    if (muhtari__ports_device(m, p) != MUHTARI_PORT_LIGHTGUN ||
        (in->flags & MUHTARI_INPUT_GUN_OFFSCREEN) != 0u || light->lines == 0u) {
        return;
    }
    uint32_t half = light->lines / 2u;
    uint32_t first = 16u + (in->gun_y > half ? in->gun_y - half : 0u);
    uint32_t last = 16u + in->gun_y + half;
    if (last >= 16u + muhtari__timing(m).height) {
        last = 15u + muhtari__timing(m).height;
    }
    uint64_t origin = m->maria.origin;
    uint64_t row = after >= origin ? (after - origin) / 454u : 0u;
    if (row < first) {
        row = first;
    }
    uint64_t at = origin + row * 454u + 135u + in->gun_x;
    if (at <= after) {
        row++;
        at += 454u;
    }
    if (row > last) {
        origin += muhtari__timing(m).frame_ticks;
        row = first;
        at = origin + row * 454u + 135u + in->gun_x;
    }
    light->sample_raster = (uint16_t)row;
    light->sample_x = (uint16_t)in->gun_x;
    light->sample_tick = at;
}

/* Historical native brightness, before host conversion. One pending
   delayed response and one active interval suffice: D<one raster and
   VBLANK aim updates cannot create a second disjoint pending response. */
void muhtari__ports_light_sample(muhtari *m, uint32_t raster, uint32_t x, uint64_t tick,
                                 uint8_t color, int visible)
{
    for (unsigned p = 0u; p < 2u; p++) {
        struct muhtari__light *light = &m->ports.light[p];
        if (light->sample_tick != tick || light->sample_raster != raster || light->sample_x != x ||
            !visible || (color & 15u) < light->threshold) {
            continue;
        }
        uint64_t start = tick + light->delay;
        uint64_t end = start + light->width;
        if (light->low != 0u && start <= light->low_until) {
            if (end > light->low_until) {
                light->low_until = end;
            }
        } else {
            if (light->response_tick == 0u || start < light->response_tick) {
                light->response_tick = start;
            }
            if (end > light->response_until) {
                light->response_until = end;
            }
        }
    }
    ports_rearm(m);
}

void muhtari__ports_catch_up(muhtari *m, uint64_t tick)
{
    uint64_t next = muhtari__ports_next_event(m);
    while (next <= tick && next != UINT64_MAX) {
        for (unsigned p = 0u; p < 2u; p++) {
            uint32_t device = muhtari__ports_device(m, p);
            if ((device == MUHTARI_PORT_PADDLE || device == MUHTARI_PORT_KEYPAD) &&
                m->tia.input_dump == 0u) {
                for (unsigned n = 0u; n < 2u; n++) {
                    struct muhtari__pot *pot = &m->ports.pot[p][n];
                    if (pot->crossing_tick == next &&
                        (device != MUHTARI_PORT_KEYPAD ||
                         (m->ports.keypad_low[p] & (0x10u << n)) == 0u)) {
                        pot->action = PORTS_CHARGED;
                    }
                }
            }
            struct muhtari__light *light = &m->ports.light[p];
            if (light->sample_tick == next) {
                muhtari__maria_flush(m, next);
                ports_light_rearm(m, p, next);
            }
            if (light->response_tick == next) {
                light->low = 1u;
                if (light->response_until > light->low_until) {
                    light->low_until = light->response_until;
                }
                light->response_tick = 0u;
                light->response_until = 0u;
            }
            if (light->low_until == next) {
                light->low = 0u;
                light->low_until = 0u;
            }
            for (unsigned n = 0u; n < 2u; n++) {
                struct muhtari__motion *axis = &m->ports.motion[p][n];
                if (axis->next_tick == next) {
                    if (axis->residual != 0) {
                        int32_t step = axis->residual > 0 ? 1 : -1;
                        if (device == MUHTARI_PORT_TRAKBALL) {
                            axis->negative = (uint8_t)(step < 0);
                            axis->phase ^= 1u;
                        } else {
                            axis->phase = (uint8_t)(((int32_t)axis->phase + step + 4) & 3);
                        }
                        axis->residual -= step;
                    }
                    axis->next_tick = axis->residual != 0 ? next + PORTS_MOTION_CADENCE_TICKS : 0u;
                }
            }
        }
        /* Deliver every intermediate resolved level, even when an entire
           cycle returns to its starting tuple (controllers.md, Driving). */
        muhtari__riot_pins_changed(m);
        muhtari__tia_pins_changed(m);
        ports_rearm(m);
        next = muhtari__ports_next_event(m);
    }
}

static int32_t ports_motion_clamp(int32_t delta)
{
    if (delta > PORTS_MOTION_RESIDUAL_MAX) {
        return PORTS_MOTION_RESIDUAL_MAX;
    }
    return delta < -PORTS_MOTION_RESIDUAL_MAX ? -PORTS_MOTION_RESIDUAL_MAX : delta;
}

enum {
    SAVEKEY_IDLE,
    SAVEKEY_CONTROL,
    SAVEKEY_ADDRESS_HIGH,
    SAVEKEY_ADDRESS_LOW,
    SAVEKEY_WRITE,
    SAVEKEY_ACK_SETUP,
    SAVEKEY_ACK_CLOCK,
    SAVEKEY_ACK_RELEASE,
    SAVEKEY_READ,
    SAVEKEY_MASTER_ACK,
    SAVEKEY_MASTER_RELEASE,
    SAVEKEY_IGNORE
};

/* Programming touches at most one page, and publishes only at its due
   tick. A same-value page still has the busy interval but stays clean
   (Microchip DS20001203Y sections 6-7; controllers.md, Serial EEPROM). */
void muhtari__savekey_catch_up(muhtari *m, uint64_t tick)
{
    struct muhtari__savekey *s = &m->savekey;
    if (s->pending == 0u || tick < s->busy_until) {
        return;
    }
    size_t offset = m->battery.hsc_attached != 0u ? 2048u : 0u;
    uint8_t changed = 0u;
    for (unsigned i = 0u; i < 64u; i++) {
        size_t at = offset + s->page_base + i;
        if (s->touched[i] != 0u && m->battery.data[at] != s->page[i]) {
            m->battery.data[at] = s->page[i];
            changed = 1u;
        }
    }
    m->battery.generation += changed;
    s->pending = 0u;
    memset(s->touched, 0, sizeof s->touched);
}

void muhtari__savekey_reset(muhtari *m)
{
    memset(&m->savekey, 0, sizeof m->savekey);
    m->savekey.host_scl = 1u;
    m->savekey.host_sda = 1u;
}

/* START discards an incomplete packet, retaining the current byte
   pointer. Pending programming is independent of packet recovery. */
static void savekey_start(struct muhtari__savekey *s)
{
    s->phase = SAVEKEY_CONTROL;
    s->bits = 0u;
    s->shift = 0u;
    s->sda_low = 0u;
    if (s->pending == 0u) {
        memset(s->touched, 0, sizeof s->touched);
    }
}

static void savekey_stop(struct muhtari__savekey *s, uint64_t tick)
{
    if (s->pending == 0u) {
        for (unsigned i = 0u; i < 64u; i++) {
            if (s->touched[i] != 0u) {
                s->pending = 1u;
                s->busy_until = tick + MUHTARI__SAVEKEY_BUSY_TICKS;
                break;
            }
        }
    }
    s->phase = SAVEKEY_IDLE;
    s->bits = 0u;
    s->sda_low = 0u;
}

/* The selected control byte, two word-address bytes and page data all
   finish with a device ACK. Unsupported addresses and busy selection
   NAK, then ignore clocks until a host STOP or START (DS20001203Y 5-8). */
static void savekey_byte(struct muhtari__savekey *s)
{
    s->ack = 1u;
    switch (s->phase) {
    case SAVEKEY_CONTROL:
        s->ack = (uint8_t)(s->pending == 0u && (s->shift & 0xFEu) == 0xA0u);
        s->next_phase = s->ack == 0u
                            ? SAVEKEY_IGNORE
                            : ((s->shift & 1u) != 0u ? SAVEKEY_READ : SAVEKEY_ADDRESS_HIGH);
        break;
    case SAVEKEY_ADDRESS_HIGH:
        s->address_high = (uint8_t)(s->shift & 0x7Fu);
        s->next_phase = SAVEKEY_ADDRESS_LOW;
        break;
    case SAVEKEY_ADDRESS_LOW:
        s->address = (uint16_t)(((unsigned)s->address_high << 8) | s->shift);
        s->page_base = (uint16_t)(s->address & 0x7FC0u);
        s->page_offset = (uint8_t)(s->address & 63u);
        s->next_phase = SAVEKEY_WRITE;
        break;
    default:
        s->page[s->page_offset] = s->shift;
        s->touched[s->page_offset] = 1u;
        s->page_offset = (uint8_t)((s->page_offset + 1u) & 63u);
        s->address = (uint16_t)(s->page_base | s->page_offset);
        s->next_phase = SAVEKEY_WRITE;
        break;
    }
    s->bits = 0u;
    s->shift = 0u;
    s->phase = SAVEKEY_ACK_SETUP;
}

static void savekey_read_bit(muhtari *m)
{
    struct muhtari__savekey *s = &m->savekey;
    if (s->bits == 0u) {
        size_t offset = m->battery.hsc_attached != 0u ? 2048u : 0u;
        s->read_byte = m->battery.data[offset + s->address];
    }
    s->sda_low = (uint8_t)(((s->read_byte >> (7u - s->bits)) & 1u) == 0u);
}

static void savekey_rise(struct muhtari__savekey *s, uint8_t sda)
{
    if (s->phase >= SAVEKEY_CONTROL && s->phase <= SAVEKEY_WRITE) {
        s->shift = (uint8_t)(((unsigned)s->shift << 1) | sda);
        s->bits++;
        if (s->bits == 8u) {
            savekey_byte(s);
        }
    } else if (s->phase == SAVEKEY_ACK_CLOCK) {
        s->phase = SAVEKEY_ACK_RELEASE;
    } else if (s->phase == SAVEKEY_READ) {
        s->bits++;
    } else if (s->phase == SAVEKEY_MASTER_ACK) {
        s->next_phase = sda != 0u ? SAVEKEY_IGNORE : SAVEKEY_READ;
        s->phase = SAVEKEY_MASTER_RELEASE;
    }
}

static void savekey_fall(muhtari *m)
{
    struct muhtari__savekey *s = &m->savekey;
    if (s->phase == SAVEKEY_ACK_SETUP) {
        s->sda_low = s->ack;
        s->phase = SAVEKEY_ACK_CLOCK;
    } else if (s->phase == SAVEKEY_ACK_RELEASE || s->phase == SAVEKEY_MASTER_RELEASE) {
        s->sda_low = 0u;
        s->phase = s->next_phase;
        s->bits = 0u;
        if (s->phase == SAVEKEY_READ) {
            savekey_read_bit(m);
        }
    } else if (s->phase == SAVEKEY_READ) {
        if (s->bits == 8u) {
            s->sda_low = 0u;
            s->address = (uint16_t)((s->address + 1u) & 0x7FFFu);
            s->phase = SAVEKEY_MASTER_ACK;
        } else {
            savekey_read_bit(m);
        }
    }
}

/* Observe host drive only; device ACK/data changes cannot synthesize
   START/STOP. ORA and DDRA both reach here at the actual data tick. Input
   bits release high; the EEPROM can only pull SDA low (controllers.md). */
void muhtari__savekey_pins(muhtari *m, uint64_t tick)
{
    if (m->battery.eeprom_port == 0u) {
        return;
    }
    muhtari__savekey_catch_up(m, tick);
    struct muhtari__savekey *s = &m->savekey;
    unsigned shift = m->battery.eeprom_port == 1u ? 4u : 0u;
    uint8_t drive = (uint8_t)(m->riot.ora | (uint8_t)~m->riot.ddra);
    uint8_t scl = (uint8_t)((drive >> (shift + 3u)) & 1u);
    uint8_t sda = (uint8_t)((drive >> (shift + 2u)) & 1u);
    if (scl != 0u && s->host_scl != 0u && sda != s->host_sda && s->sda_low == 0u) {
        if (sda == 0u) {
            savekey_start(s);
        } else {
            savekey_stop(s, tick);
        }
    } else if (scl != s->host_scl) {
        if (scl != 0u) {
            savekey_rise(s, sda);
        } else {
            savekey_fall(m);
        }
    }
    s->host_scl = scl;
    s->host_sda = sda;
}

/* A device a host may name: 0 for the cartridge's, or one this version
   models. Returns OK, ERR_UNSUPPORTED_DEVICE for an A78 device it does not
   model, or ERR_ARGUMENT for a value that names no device. */
static muhtari_status ports_check_device(uint32_t device)
{
    if (device == 0u || device == MUHTARI_PORT_PROLINE || device == MUHTARI_PORT_JOYSTICK ||
        device == MUHTARI_PORT_UNPLUGGED || device == MUHTARI_PORT_SAVEKEY ||
        device == MUHTARI_PORT_ATARIVOX) {
        return MUHTARI_STATUS_OK;
    }
    if (device <= MUHTARI_PORT_MEGA7800) {
        return MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE;
    }
    return MUHTARI_STATUS_ERR_ARGUMENT;
}

/* New models opt in here without changing the released legacy validator. */
muhtari_status muhtari__ports_check_device_ex(uint32_t device)
{
    if (device == 0u || device == MUHTARI_PORT_PROLINE || device == MUHTARI_PORT_JOYSTICK ||
        device == MUHTARI_PORT_DRIVING || device == MUHTARI_PORT_LIGHTGUN ||
        device == MUHTARI_PORT_PADDLE || device == MUHTARI_PORT_KEYPAD ||
        device == MUHTARI_PORT_TRAKBALL || device == MUHTARI_PORT_ST_MOUSE ||
        device == MUHTARI_PORT_AMIGA_MOUSE || device == MUHTARI_PORT_UNPLUGGED ||
        device == MUHTARI_PORT_SNES2ATARI || device == MUHTARI_PORT_MEGA7800 ||
        device == MUHTARI_PORT_SAVEKEY || device == MUHTARI_PORT_ATARIVOX) {
        return MUHTARI_STATUS_OK;
    }
    return device <= MUHTARI_PORT_MEGA7800 ? MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE
                                           : MUHTARI_STATUS_ERR_ARGUMENT;
}

static uint32_t ports_resolve_ex(const muhtari *m, unsigned p, uint32_t device)
{
    if (device == 0u) {
        device = m->ports.load_device[p];
    }
    if (device == 0u) {
        device = p == 0u ? m->cart.info.port1 : m->cart.info.port2;
        if (device == MUHTARI_PORT_NONE) {
            device = m->cart.rom != NULL ? MUHTARI_PORT_UNPLUGGED : MUHTARI_PORT_PROLINE;
        }
    }
    /* The attachment is immutable even when a save hint, rather than the
       descriptive port code, selected it (embedding.md, Battery). */
    if (m->battery.eeprom_port == p + 1u) {
        return m->battery.eeprom_device;
    }
    return device;
}

muhtari_status muhtari_set_input_ex(muhtari *m, const muhtari_input_ex *in)
{
    if (m == NULL || in == NULL) {
        return MUHTARI_STATUS_ERR_ARGUMENT;
    }
    muhtari_status st = muhtari__check_size(in, MUHTARI__INPUT_EX_SIZE_FIRST, (uint32_t)sizeof *in);
    if (st != MUHTARI_STATUS_OK) {
        return st;
    }
    if (in->size < sizeof *in && in->size % 4u != 0u) {
        return MUHTARI_STATUS_ERR_STRUCT_SIZE;
    }
    muhtari_input_ex normalized;
    memset(&normalized, 0, sizeof normalized);
    /* Every payload field is one complete four-byte scalar. Fixed-size
       copies preserve unaligned input and the restricted C memory surface. */
    uint32_t end = in->size < sizeof normalized ? in->size : (uint32_t)sizeof normalized;
    for (uint32_t at = 4u; at < end; at += 4u) {
        memcpy((uint8_t *)&normalized + at, (const uint8_t *)in + at, 4u);
    }
    normalized.size = (uint32_t)sizeof normalized;
    if ((normalized.switches & ~PORTS_SWITCHES) != 0u) {
        return MUHTARI_STATUS_ERR_ARGUMENT;
    }
    for (unsigned p = 0u; p < 2u; p++) {
        muhtari_port_input *port = &normalized.port[p];
        uint32_t requested = port->device;
        if ((m->battery.eeprom_port == p + 1u && requested != 0u &&
             requested != m->battery.eeprom_device) ||
            (m->battery.eeprom_port != p + 1u &&
             (requested == MUHTARI_PORT_SAVEKEY || requested == MUHTARI_PORT_ATARIVOX))) {
            return MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE;
        }
        st = muhtari__ports_check_device_ex(requested);
        if (st != MUHTARI_STATUS_OK) {
            return st;
        }
        port->device = ports_resolve_ex(m, p, requested);
        st = muhtari__ports_check_device_ex(port->device);
        if (st != MUHTARI_STATUS_OK) {
            return st;
        }
        int gun = port->device == MUHTARI_PORT_LIGHTGUN;
        int paddle = port->device == MUHTARI_PORT_PADDLE;
        int keypad = port->device == MUHTARI_PORT_KEYPAD;
        int mouse = port->device == MUHTARI_PORT_TRAKBALL ||
                    port->device == MUHTARI_PORT_ST_MOUSE ||
                    port->device == MUHTARI_PORT_AMIGA_MOUSE;
        if (gun && in->size < offsetof(muhtari_input_ex, port) + p * sizeof(muhtari_port_input) +
                                  offsetof(muhtari_port_input, gun_y) + sizeof port->gun_y) {
            port->flags = MUHTARI_INPUT_GUN_OFFSCREEN;
        }
        uint32_t buttons =
            (port->device == MUHTARI_PORT_SNES2ATARI || port->device == MUHTARI_PORT_MEGA7800)
                ? 4095u
            : keypad ? 0u
            : paddle ? MUHTARI_BUTTON_1 | MUHTARI_BUTTON_2
                     : (port->device == MUHTARI_PORT_DRIVING || gun ||
                                port->device == MUHTARI_PORT_TRAKBALL
                            ? MUHTARI_BUTTON_1
                            : (mouse ? MUHTARI_BUTTON_1 | MUHTARI_BUTTON_2 : PORTS_BUTTONS));
        if ((port->buttons & ~buttons) != 0u ||
            (paddle ? (port->paddle[0] > 65535u || port->paddle[1] > 65535u)
                    : (port->paddle[0] != 0u || port->paddle[1] != 0u)) ||
            (keypad ? port->keypad > 4095u : port->keypad != 0u) ||
            (port->device != MUHTARI_PORT_DRIVING && !mouse && port->dx != 0) ||
            (!mouse && port->dy != 0) ||
            (gun ? (port->gun_x >= MUHTARI__WIDTH || port->gun_y >= muhtari__timing(m).height ||
                    (port->flags & ~(uint32_t)MUHTARI_INPUT_GUN_OFFSCREEN) != 0u)
                 : (port->gun_x != 0u || port->gun_y != 0u || port->flags != 0u)) ||
            ((port->device == MUHTARI_PORT_UNPLUGGED || port->device == MUHTARI_PORT_SAVEKEY ||
              port->device == MUHTARI_PORT_ATARIVOX) &&
             port->buttons != 0u)) {
            return MUHTARI_STATUS_ERR_ARGUMENT;
        }
    }
    m->ports.pending_ex = normalized;
    m->ports.input_mode = 1u;
    m->ports.pending = 1u;
    return MUHTARI_STATUS_OK;
}

muhtari_status muhtari_set_input(muhtari *m, const muhtari_input *in)
{
    if (m == NULL || in == NULL) {
        return MUHTARI_STATUS_ERR_ARGUMENT;
    }
    muhtari_status st = muhtari__check_size(in, MUHTARI__INPUT_SIZE_FIRST, (uint32_t)sizeof *in);
    if (st != MUHTARI_STATUS_OK) {
        return st;
    }
    if ((in->buttons[0] & ~PORTS_BUTTONS) != 0u || (in->buttons[1] & ~PORTS_BUTTONS) != 0u ||
        (in->switches & ~PORTS_SWITCHES) != 0u) {
        return MUHTARI_STATUS_ERR_ARGUMENT;
    }
    for (uint32_t p = 0; p < 2u; p++) {
        st = ports_check_device(in->device[p]);
        if (st != MUHTARI_STATUS_OK) {
            return st;
        }
        /* Save-bearing placement is frozen at load. Validate the whole
           request before changing pending joystick state (embedding.md). */
        uint32_t device = in->device[p];
        if (m->battery.eeprom_port == p + 1u) {
            if (device != 0u && device != m->battery.eeprom_device) {
                return MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE;
            }
        } else if (device == MUHTARI_PORT_SAVEKEY || device == MUHTARI_PORT_ATARIVOX) {
            return MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE;
        }
    }
    /* Opposing directions are kept: they reach the pins as pressed. */
    struct muhtari__ports *ports = &m->ports;
    for (uint32_t p = 0; p < 2u; p++) {
        ports->pending_device[p] = in->device[p];
        ports->pending_buttons[p] = in->buttons[p];
    }
    ports->pending_switches = in->switches;
    ports->input_mode = 0u;
    ports->pending = 1u;
    return MUHTARI_STATUS_OK;
}

/* The latest input set reaches the machine at a VBLANK start, so a whole
   frame sees one input (docs/embedding.md, Input). */
void muhtari__ports_latch(muhtari *m)
{
    struct muhtari__ports *ports = &m->ports;
    if (ports->pending == 0u) {
        return;
    }
    muhtari__ports_catch_up(m, m->bus.ticks);
    for (uint32_t p = 0; p < 2u; p++) {
        uint32_t previous = muhtari__ports_device(m, p);
        if (ports->input_mode != 0u) {
            ports->device[p] = ports->pending_ex.port[p].device;
            ports->buttons[p] = ports->pending_ex.port[p].buttons;
        } else {
            ports->device[p] = ports->pending_device[p];
            ports->buttons[p] = ports->pending_buttons[p];
        }
        uint32_t device = muhtari__ports_device(m, p);
        ports->keypad_keys[p] =
            ports->input_mode != 0u ? (uint16_t)ports->pending_ex.port[p].keypad : 0u;
        if (previous != device)
            ports->keypad_low[p] = 0u;
        for (unsigned n = 0u; n < 2u; n++) {
            struct muhtari__pot *pot = &ports->pot[p][n];
            uint32_t position = ports->input_mode != 0u ? ports->pending_ex.port[p].paddle[n] : 0u;
            if (previous != device) {
                memset(pot, 0, sizeof *pot);
                pot->last_tick = m->bus.ticks;
            } else if (position == ports->applied_ex.port[p].paddle[n]) {
                continue;
            } else {
                ports_pot_materialize(pot, m->bus.ticks, m->tia.input_dump);
            }
            pot->duration = ports_pot_duration(position);
            ports_pot_project(pot, m->bus.ticks);
            if (device == MUHTARI_PORT_KEYPAD)
                pot->crossing_tick = m->bus.ticks + 2864u;
        }
        if (previous != device) {
            memset(ports->motion[p], 0, sizeof ports->motion[p]);
            memset(&ports->light[p], 0, sizeof ports->light[p]);
            memset(&ports->adapter[p], 0, sizeof ports->adapter[p]);
            ports->adapter[p].host = ports_host(m, p);
        }
        if (device == MUHTARI_PORT_LIGHTGUN && ports->light[p].delay == 0u) {
            ports->light[p].delay = 64u;
            ports->light[p].width = 30u;
            ports->light[p].lines = 9u;
            ports->light[p].threshold = 8u;
        }
        if (ports->input_mode != 0u &&
            (device == MUHTARI_PORT_DRIVING || device == MUHTARI_PORT_TRAKBALL ||
             device == MUHTARI_PORT_ST_MOUSE || device == MUHTARI_PORT_AMIGA_MOUSE)) {
            for (unsigned n = 0u; n < 2u; n++) {
                struct muhtari__motion *axis = &ports->motion[p][n];
                int32_t delta = ports_motion_clamp(n == 0u ? ports->pending_ex.port[p].dx
                                                           : ports->pending_ex.port[p].dy);
                axis->residual = ports_motion_clamp(axis->residual + delta);
                if (axis->next_tick == 0u && axis->residual != 0) {
                    axis->next_tick = m->bus.ticks + PORTS_MOTION_CADENCE_TICKS;
                }
            }
        }
    }
    if (ports->input_mode != 0u) {
        ports->initialized = 1u;
        ports->applied_ex = ports->pending_ex;
        ports->switches = ports->pending_ex.switches;
        for (unsigned p = 0u; p < 2u; p++) {
            ports->pending_ex.port[p].dx = 0;
            ports->pending_ex.port[p].dy = 0;
        }
    } else {
        memset(&ports->applied_ex, 0, sizeof ports->applied_ex);
        ports->switches = ports->pending_switches;
    }
    ports->pending = 0u;
    muhtari__ports_rows(m, m->bus.ticks);
    for (unsigned p = 0u; p < 2u; p++) {
        ports_light_rearm(m, p, m->bus.ticks);
    }
    ports_rearm(m);
    /* The new pins reach PA7's edge detector at this tick. */
    muhtari__riot_pins_changed(m);
    muhtari__tia_pins_changed(m);
    muhtari__bus_rearm(m);
}

/* A header naming no controller leaves the port unplugged; one naming a
   device this version does not model gets a ProLine, until that device is
   modelled (docs/embedding.md, Input). */
uint32_t muhtari__ports_device(const muhtari *m, uint32_t port)
{
    if (m->battery.eeprom_port == port + 1u) {
        return m->battery.eeprom_device;
    }
    uint32_t device = m->ports.device[port];
    if (device != 0u) {
        return device;
    }
    device = port == 0u ? m->cart.info.port1 : m->cart.info.port2;
    if (device == MUHTARI_PORT_NONE && m->cart.rom != NULL) {
        return MUHTARI_PORT_UNPLUGGED;
    }
    return device == MUHTARI_PORT_JOYSTICK ? MUHTARI_PORT_JOYSTICK : MUHTARI_PORT_PROLINE;
}

/* SWCHA's input pins: port 0 in bits 7-4 as right, left, down and up, port
   1 in bits 3-0. A pressed direction closes its switch and pulls the pin to
   0; an unplugged port's pins read high (A7800-CARTRIDGES-AND-PERIPHERALS
   section 3.1; docs/hardware/controllers.md, Directions). */
uint8_t muhtari__ports_swcha(const muhtari *m)
{
    uint8_t pins = 0xFFu;
    for (uint32_t p = 0; p < 2u; p++) {
        uint32_t device = muhtari__ports_device(m, p);
        if (device == MUHTARI_PORT_UNPLUGGED || device == MUHTARI_PORT_SAVEKEY ||
            device == MUHTARI_PORT_ATARIVOX) {
            continue;
        }
        uint32_t b = m->ports.buttons[p];
        if (device == MUHTARI_PORT_SNES2ATARI)
            continue;
        if (device == MUHTARI_PORT_MEGA7800 && m->ports.adapter[p].scanning != 0u) {
            unsigned pos = m->ports.adapter[p].position;
            uint8_t low = 0u;
            if (pos < 2u) {
                low = 12u; /* six-button class00 and identification00 */
            } else if (pos < 6u) {
                /* Pin4 then pin3: C/B, Start/A, Y/Z, Mode/X. */
                static const uint16_t pin4[4] = {64u, 2048u, 256u, 1024u};
                static const uint16_t pin3[4] = {16u, 32u, 512u, 128u};
                low = (uint8_t)(((b & pin4[pos - 2u]) != 0u ? 8u : 0u) |
                                ((b & pin3[pos - 2u]) != 0u ? 4u : 0u));
            }
            pins &= (uint8_t)~(p == 0u ? low << 4 : low);
            continue;
        }
        if (device == MUHTARI_PORT_KEYPAD) {
            uint8_t low = m->ports.keypad_low[p] & 15u;
            pins &= (uint8_t)~(p == 0u ? low << 4 : low);
            continue;
        }
        if (device == MUHTARI_PORT_PADDLE) {
            uint8_t low = (uint8_t)(((b & MUHTARI_BUTTON_1) != 0u ? 8u : 0u) |
                                    ((b & MUHTARI_BUTTON_2) != 0u ? 4u : 0u));
            pins &= (uint8_t)~(p == 0u ? low << 4 : low);
            continue;
        }
        if (device == MUHTARI_PORT_LIGHTGUN) {
            if ((b & MUHTARI_BUTTON_1) == 0u) {
                pins &= (uint8_t)~(p == 0u ? 0x10u : 0x01u);
            }
            continue;
        }
        if (device == MUHTARI_PORT_DRIVING) {
            /* Connector (1,2): 11,10,00,01. Pin 1 is the low
               SWCHA bit in its nibble (controllers.md, Driving). */
            static const uint8_t closed[4] = {0u, 2u, 3u, 1u};
            uint8_t low = closed[m->ports.motion[p][0].phase];
            pins = (uint8_t)(pins & (uint8_t)~(p == 0u ? low << 4 : low));
            continue;
        }
        if (device == MUHTARI_PORT_TRAKBALL || device == MUHTARI_PORT_ST_MOUSE ||
            device == MUHTARI_PORT_AMIGA_MOUSE) {
            const struct muhtari__motion *x = &m->ports.motion[p][0];
            const struct muhtari__motion *y = &m->ports.motion[p][1];
            uint8_t low;
            /* Connector-specific closures, controllers.md, Motion. ST's
               RIGHT/UP convention is converted to API right/down here. */
            if (device == MUHTARI_PORT_TRAKBALL) {
                low =
                    (uint8_t)(x->negative | (x->phase << 1) | (y->negative << 2) | (y->phase << 3));
            } else if (device == MUHTARI_PORT_ST_MOUSE) {
                static const uint8_t horizontal[4] = {0u, 2u, 3u, 1u};
                static const uint8_t vertical_down[4] = {0u, 8u, 12u, 4u};
                low = horizontal[x->phase] | vertical_down[y->phase];
            } else {
                static const uint8_t horizontal[4] = {0u, 2u, 10u, 8u};
                static const uint8_t vertical[4] = {0u, 1u, 5u, 4u};
                low = horizontal[x->phase] | vertical[y->phase];
            }
            pins &= (uint8_t)~(p == 0u ? low << 4 : low);
            continue;
        }
        uint8_t closed = (uint8_t)(((b & MUHTARI_BUTTON_UP) != 0u ? 0x1u : 0u) |
                                   ((b & MUHTARI_BUTTON_DOWN) != 0u ? 0x2u : 0u) |
                                   ((b & MUHTARI_BUTTON_LEFT) != 0u ? 0x4u : 0u) |
                                   ((b & MUHTARI_BUTTON_RIGHT) != 0u ? 0x8u : 0u));
        pins = (uint8_t)(pins & ~(uint32_t)(p == 0u ? closed << 4 : closed));
    }
    return pins;
}

/* Two-button mode on a port: its PB2 [PB4] an output driven 0, which puts
   +5 V on pin 6, with INPTCTRL's TIA-EN clear (A7800-CARTRIDGES-AND-
   PERIPHERALS section 3.3, section 3.1). */
static int ports_two_button(const muhtari *m, uint32_t port)
{
    uint8_t bit = port == 0u ? 0x04u : 0x10u;
    return (m->riot.ddrb & bit) != 0u && (m->riot.orb & bit) == 0u &&
           (m->bus.inptctrl & MUHTARI__INPTCTRL_TIA_EN) == 0u;
}

/* SWCHB's input pins: Reset, Select and Pause pull D0, D1 and D3 to 0
   while held, and a difficulty switch at A reads 1 on D6 (left) and D7
   (right). D2, D4 and D5 have nothing driving them and read high
   (docs/hardware/controllers.md, Switches). */
uint8_t muhtari__ports_swchb(const muhtari *m)
{
    uint32_t s = m->ports.switches;
    uint8_t closed = (uint8_t)(((s & MUHTARI_SWITCH_RESET) != 0u ? 0x01u : 0u) |
                               ((s & MUHTARI_SWITCH_SELECT) != 0u ? 0x02u : 0u) |
                               ((s & MUHTARI_SWITCH_PAUSE) != 0u ? 0x08u : 0u));
    uint8_t a = (uint8_t)(((s & MUHTARI_SWITCH_LEFT_DIFFICULTY_A) != 0u ? 0x40u : 0u) |
                          ((s & MUHTARI_SWITCH_RIGHT_DIFFICULTY_A) != 0u ? 0x80u : 0u));
    return (uint8_t)((0x3Fu ^ closed) | a);
}

/* The level, 0 or 1, on INPT0-INPT5's D7. INPT0 and INPT1 [INPT2, INPT3]
   are pins 5 and 9: a ProLine's right and left buttons, 1 while pressed,
   in either mode, as MAME, a7800 and MiSTer have it. INPT4 [INPT5] is pin
   6: in two-button mode a ProLine leaves it at 1, and in one-button mode
   either button pulls it to 0; a 2600 stick's fire pulls it to 0 in
   either mode. Nothing connected reads 0 on INPT0-INPT3 and 1 on INPT4
   and INPT5 (A7800-CARTRIDGES-AND-PERIPHERALS section 3.1, section 3.2). */
uint8_t muhtari__ports_inpt(const muhtari *m, uint32_t n)
{
    uint32_t port = n < 4u ? n / 2u : n - 4u;
    uint32_t device = muhtari__ports_device(m, port);
    uint32_t b = m->ports.buttons[port];
    /* Mega B/C are ordinary ProLine fire buttons; A is extended-only
       (controllers.md, Mega7800). */
    if (device == MUHTARI_PORT_MEGA7800) {
        b = (b & MUHTARI_BUTTON_1) | ((b & MUHTARI_MEGA_BUTTON_C) != 0u ? MUHTARI_BUTTON_2 : 0u);
        device = MUHTARI_PORT_PROLINE;
    }
    if (device == MUHTARI_PORT_SNES2ATARI) {
        if (n < 4u)
            return 0u;
        if (ports_two_button(m, port))
            return 1u;
        const struct muhtari__adapter *s = &m->ports.adapter[port];
        /* Driver order and high16/low17 detection: controllers.md. */
        static const uint16_t button[12] = {16u, 64u, 1024u, 2048u, 1u,   2u,
                                            4u,  8u,  32u,   128u,  256u, 512u};
        return (uint8_t)(s->position < 16u &&
                         (s->position >= 12u || (s->snapshot & button[s->position]) == 0u));
    }
    if (n < 4u) {
        if (device == MUHTARI_PORT_PADDLE || device == MUHTARI_PORT_KEYPAD) {
            const struct muhtari__pot *pot = &m->ports.pot[port][n & 1u];
            return (uint8_t)(m->tia.input_dump == 0u && pot->action == PORTS_CHARGED);
        }
        if (device == MUHTARI_PORT_ST_MOUSE || device == MUHTARI_PORT_AMIGA_MOUSE) {
            return (uint8_t)((n & 1u) != 0u && m->tia.input_dump == 0u &&
                             (b & MUHTARI_BUTTON_2) == 0u);
        }
        uint32_t button = (n & 1u) == 0u ? MUHTARI_BUTTON_1 : MUHTARI_BUTTON_2;
        return (uint8_t)(device == MUHTARI_PORT_PROLINE && (b & button) != 0u);
    }
    if (device == MUHTARI_PORT_PROLINE && !ports_two_button(m, port)) {
        return (uint8_t)((b & (MUHTARI_BUTTON_1 | MUHTARI_BUTTON_2)) == 0u);
    }
    if (device == MUHTARI_PORT_JOYSTICK || device == MUHTARI_PORT_DRIVING ||
        device == MUHTARI_PORT_TRAKBALL || device == MUHTARI_PORT_ST_MOUSE ||
        device == MUHTARI_PORT_AMIGA_MOUSE) {
        return (uint8_t)((b & MUHTARI_BUTTON_1) == 0u);
    }
    if (device == MUHTARI_PORT_LIGHTGUN) {
        return (uint8_t)(m->ports.light[port].low == 0u);
    }
    if (device == MUHTARI_PORT_KEYPAD)
        return (uint8_t)((m->ports.keypad_low[port] & 0x40u) == 0u);
    return 1u;
}

/* A frame that ends with a 2600 stick's fire held on a port in two-button
   mode adds 1 to io_hazard: the fire shorts the pin the console drives
   (A7800-CARTRIDGES-AND-PERIPHERALS section 3.3). */
void muhtari__ports_count_hazard(muhtari *m)
{
    for (uint32_t p = 0; p < 2u; p++) {
        if (muhtari__ports_device(m, p) == MUHTARI_PORT_JOYSTICK &&
            (m->ports.buttons[p] & MUHTARI_BUTTON_1) != 0u && ports_two_button(m, p)) {
            m->ports.io_hazard++;
            return;
        }
    }
}

void muhtari__ports_reset_counters(muhtari *m)
{
    muhtari__ports_reset(m);
    m->frames = 0u;
    m->lag_frames = 0u;
    m->ports.input_read = 0u;
    m->ports.io_hazard = 0u;
}

/* Live pointers into the instance, the one deliberate exception to the rule
   that no struct carries a pointer to core state: libretro memory maps need
   them (docs/embedding.md, Memory). */
muhtari_status muhtari_get_memory(muhtari *m, uint32_t kind, muhtari_span *out)
{
    if (m == NULL || out == NULL) {
        return MUHTARI_STATUS_ERR_ARGUMENT;
    }
    muhtari_status st = muhtari__check_size(out, MUHTARI__SPAN_SIZE_FIRST, (uint32_t)sizeof *out);
    if (st != MUHTARI_STATUS_OK) {
        return st;
    }
    muhtari_span span;
    memset(&span, 0, sizeof span);
    span.size = out->size;
    switch (kind) {
    case MUHTARI_MEMORY_SYSTEM_RAM:
        span.data = m->ram;
        span.bytes = sizeof m->ram;
        break;
    case MUHTARI_MEMORY_RIOT_RAM:
        span.data = m->riot.ram;
        span.bytes = sizeof m->riot.ram;
        break;
    case MUHTARI_MEMORY_CART_RAM:
        span.data = m->cart.ram; /* NULL, and 0 bytes, on a board without RAM */
        span.bytes = m->cart.ram != NULL ? m->cart.info.ram_size : 0u;
        break;
    default:
        return MUHTARI_STATUS_ERR_ARGUMENT;
    }
    muhtari__write_out(out, &span, (uint32_t)sizeof span);
    return MUHTARI_STATUS_OK;
}

muhtari_status muhtari_get_counters(const muhtari *m, muhtari_counters *out)
{
    if (m == NULL || out == NULL) {
        return MUHTARI_STATUS_ERR_ARGUMENT;
    }
    muhtari_status st =
        muhtari__check_size(out, MUHTARI__COUNTERS_SIZE_FIRST, (uint32_t)sizeof *out);
    if (st != MUHTARI_STATUS_OK) {
        return st;
    }
    muhtari_counters c;
    memset(&c, 0, sizeof c);
    c.size = out->size;
    c.frames = m->frames;
    c.lag_frames = m->lag_frames;
    muhtari__write_out(out, &c, (uint32_t)sizeof c);
    return MUHTARI_STATUS_OK;
}
