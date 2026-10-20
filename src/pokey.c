/* POKEY timer interface, owned from the hardware manual (pokey.md). */
#include <string.h>
#include "internal.h"

void muhtari__pokey_reset(muhtari *m, unsigned chip)
{
    memset(&m->pokey[chip], 0, sizeof m->pokey[chip]);
}
void muhtari__pokey_catch_up(muhtari *m, uint64_t tick)
{
    (void)m;
    (void)tick;
}
void muhtari__pokey_write(muhtari *m, unsigned chip, uint8_t reg, uint8_t v, uint64_t tick)
{
    (void)m;
    (void)chip;
    (void)reg;
    (void)v;
    (void)tick;
}
uint8_t muhtari__pokey_read(muhtari *m, unsigned chip, uint8_t reg, uint64_t tick)
{
    (void)m;
    (void)chip;
    (void)reg;
    (void)tick;
    return 0u;
}
int muhtari__pokey_irq(const muhtari *m, unsigned chip)
{
    (void)m;
    (void)chip;
    return 0;
}
