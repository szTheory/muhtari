/* The controller ports as the host sets them, the memory spans and the
   frame counters (docs/embedding.md, Input, Memory and Counters). The pins
   the devices drive are read in src/riot.c and src/tia.c. */
#include <string.h>

#include "internal.h"

/* The bits a host may set. */
#define PORTS_BUTTONS 0x3Fu  /* MUHTARI_BUTTON_UP .. MUHTARI_BUTTON_2 */
#define PORTS_SWITCHES 0x1Fu /* MUHTARI_SWITCH_RESET .. _RIGHT_DIFFICULTY_A */

/* A device a host may name: 0 for the cartridge's, or one this version
   models. Returns OK, ERR_UNSUPPORTED_DEVICE for an A78 device it does not
   model, or ERR_ARGUMENT for a value that names no device. */
static muhtari_status ports_check_device(uint32_t device)
{
    if (device == 0u || device == MUHTARI_PORT_PROLINE || device == MUHTARI_PORT_JOYSTICK ||
        device == MUHTARI_PORT_UNPLUGGED) {
        return MUHTARI_STATUS_OK;
    }
    if (device <= MUHTARI_PORT_MEGA7800) {
        return MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE;
    }
    return MUHTARI_STATUS_ERR_ARGUMENT;
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
   frame sees one input (DEC.25). */
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
}

/* A header naming no controller leaves the port unplugged; one naming a
   device this version does not model gets a ProLine, until that device is
   modelled (docs/embedding.md, Input). */
uint32_t muhtari__ports_device(const muhtari *m, uint32_t port)
{
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

void muhtari__ports_reset_counters(muhtari *m)
{
    m->frames = 0u;
    m->lag_frames = 0u;
    m->ports.input_read = 0u;
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
