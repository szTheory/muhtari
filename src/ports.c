/* The controller ports as the host sets them, the memory spans and the
   frame counters. */
#include "internal.h"

muhtari_status muhtari_set_input(muhtari *m, const muhtari_input *in)
{
    (void)m;
    (void)in;
    return MUHTARI_STATUS_ERR_ARGUMENT;
}

void muhtari__ports_latch(muhtari *m)
{
    (void)m;
}

uint32_t muhtari__ports_device(const muhtari *m, uint32_t port)
{
    (void)m;
    (void)port;
    return 0u;
}

void muhtari__ports_reset_counters(muhtari *m)
{
    (void)m;
}

muhtari_status muhtari_get_memory(muhtari *m, uint32_t kind, muhtari_span *out)
{
    (void)m;
    (void)kind;
    (void)out;
    return MUHTARI_STATUS_ERR_ARGUMENT;
}

muhtari_status muhtari_get_counters(const muhtari *m, muhtari_counters *out)
{
    (void)m;
    (void)out;
    return MUHTARI_STATUS_ERR_ARGUMENT;
}

uint8_t muhtari__bus_peek(const muhtari *m, uint16_t addr)
{
    (void)addr;
    return m->bus.open_bus;
}

uint8_t muhtari__riot_peek(const muhtari *m, uint16_t addr)
{
    (void)addr;
    return m->bus.open_bus;
}
