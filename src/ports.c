/* The controller ports as the host sets them, the pins their devices drive,
   the memory spans and the frame counters (docs/embedding.md, Input, Memory
   and Counters; docs/hardware/controllers.md). src/riot.c and src/tia.c read
   the pins. */
#include <string.h>

#include "internal.h"

/* The bits a host may set. */
#define PORTS_BUTTONS 0x3Fu  /* MUHTARI_BUTTON_UP .. MUHTARI_BUTTON_2 */
#define PORTS_SWITCHES 0x1Fu /* MUHTARI_SWITCH_RESET .. _RIGHT_DIFFICULTY_A */

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

muhtari_status muhtari_set_input_ex(muhtari *m, const muhtari_input_ex *in)
{
    (void)m;
    (void)in;
    return MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE;
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
    for (uint32_t p = 0; p < 2u; p++) {
        ports->device[p] = ports->pending_device[p];
        ports->buttons[p] = ports->pending_buttons[p];
    }
    ports->switches = ports->pending_switches;
    ports->pending = 0u;
    /* The new pins reach PA7's edge detector at this tick. */
    muhtari__riot_pins_changed(m);
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
    if (n < 4u) {
        uint32_t button = (n & 1u) == 0u ? MUHTARI_BUTTON_1 : MUHTARI_BUTTON_2;
        return (uint8_t)(device == MUHTARI_PORT_PROLINE && (b & button) != 0u);
    }
    if (device == MUHTARI_PORT_PROLINE && !ports_two_button(m, port)) {
        return (uint8_t)((b & (MUHTARI_BUTTON_1 | MUHTARI_BUTTON_2)) == 0u);
    }
    if (device == MUHTARI_PORT_JOYSTICK) {
        return (uint8_t)((b & MUHTARI_BUTTON_1) == 0u);
    }
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
