/* Owned chip vectors from hardware/pokey.md, through shipped objects. */
#include <string.h>
#include "internal.h"
#include "../check.h"

static muhtari machine;

static muhtari *fresh(void)
{
    memset(&machine, 0, sizeof machine);
    muhtari__power_on(&machine);
    machine.bus.inptctrl = 7u;
    muhtari__synth_reset(&machine, 0u, 2u);
    muhtari__pokey_reset(&machine, 0u);
    muhtari__pokey_reset(&machine, 1u);
    return &machine;
}

static void clock_vectors(void)
{
    muhtari *m = fresh();
    (void)muhtari__bus_read(m, 0x0280u);
    CHECK_EQ_U64(m->bus.ticks, 6u);
    CHECK_EQ_U64(m->pokey[0].tick, 5u);
    CHECK_EQ_U64(m->pokey[0].half, 2u);
    CHECK_EQ_U64(m->pokey[0].cycle, 1u);
    (void)muhtari__bus_read(m, 0x1800u);
    CHECK_EQ_U64(m->pokey[0].tick, 10u);
    CHECK_EQ_U64(m->pokey[0].half, 4u);
    CHECK_EQ_U64(m->pokey[0].cycle, 2u);
    for (unsigned slow = 0; slow < 2u; slow++) {
        m = fresh();
        muhtari__pokey_write(m, 0u, 8u, 64u, 0u);
        muhtari__pokey_write(m, 0u, 0u, 100u, 0u);
        muhtari__pokey_write(m, 0u, 1u, 0xAFu, 0u);
        muhtari__pokey_write(m, 0u, 9u, 0u, 0u);
        for (unsigned n = 0; n < 100u; n++) {
            (void)muhtari__bus_read(m, slow != 0u ? 0x0280u : 0x1800u);
        }
        for (unsigned n = 0; n < 8u; n++) {
            (void)muhtari__bus_read(m, 0x1800u);
        }
        CHECK_EQ_U64(m->pokey[0].channel[0].last_fire, 432u + slow * 200u);
        CHECK_EQ_U64(m->pokey[0].channel[0].fires, 1u);
    }
    m = fresh();
    m->bus.rdy_release = 100u;
    (void)muhtari__bus_read(m, 0x1800u);
    CHECK_EQ_U64(m->pokey[0].cycle, 26u);
    CHECK_EQ_U64(m->pokey[0].half, 52u);
    CHECK_EQ_U64(m->pokey[0].tick, 104u);
    CHECK_EQ_U64(m->pokey[1].cycle, 26u);
}

static void logic_vectors(void)
{
    muhtari *m = fresh();
    muhtari__pokey_write(m, 0u, 8u, 0x60u, 0u);
    for (unsigned ch = 0; ch < 4u; ch++) {
        muhtari__pokey_write(m, 0u, (uint8_t)(ch * 2u), 0u, 0u);
        muhtari__pokey_write(m, 0u, (uint8_t)(ch * 2u + 1u), 0xAFu, 0u);
    }
    muhtari__pokey_write(m, 0u, 14u, 7u, 0u);
    muhtari__pokey_write(m, 0u, 9u, 0u, 0u);
    muhtari__pokey_catch_up(m, 96u);
    CHECK_EQ_U64(m->pokey[0].channel[0].fires, 5u);
    CHECK_EQ_U64(m->pokey[0].channel[2].fires, 5u);
    CHECK_EQ_U64(m->pokey[0].channel[1].last_fire, 96u);
    CHECK_EQ_U64(m->pokey[0].channel[3].last_fire, 96u);
    CHECK_EQ_U64(m->pokey[0].irqst, 248u);
    CHECK_EQ_U64(m->pokey[0].poly17 == 0u, 0u);
    CHECK_EQ_U64(m->pokey[1].channel[2].audc, 0u);
    muhtari__pokey_write(m, 0u, 14u, 0u, 96u);
    CHECK(!muhtari__pokey_irq(m, 0u));
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        return 2;
    }
    if (strcmp(argv[1], "clock") == 0) {
        clock_vectors();
    } else if (strcmp(argv[1], "logic") == 0) {
        logic_vectors();
    } else {
        return 2;
    }
    CHECK_DONE();
}
