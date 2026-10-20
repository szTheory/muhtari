/* Documentary timer status and CPU-visible propagation through the shipped
   bus. Altirra manual sections3.4/5.7; hardware/pokey.md, Interrupts. */
#include <string.h>
#include "internal.h"
#include "../check.h"

static muhtari machine;

static muhtari *fresh(unsigned chip, uint8_t wiring)
{
    muhtari *m = &machine;
    memset(m, 0, sizeof *m);
    muhtari__power_on(m);
    m->bus.inptctrl = 7u;
    muhtari__synth_reset(m, 0u, 2u);
    m->cart.pokey_at[0] = 0x450u;
    m->cart.pokey_at[1] = 0x440u;
    m->cart.irq_mask = wiring;
    muhtari__pokey_reset(m, 0u);
    muhtari__pokey_reset(m, 1u);
    muhtari__pokey_write(m, chip, 8u, 0x60u, 0u);
    muhtari__pokey_write(m, chip, 9u, 0u, 0u);
    return m;
}

static void cycles(muhtari *m, unsigned n)
{
    for (unsigned i = 0; i < n; i++)
        (void)muhtari__bus_read(m, 0x1800u);
}

static void write_enable(muhtari *m, unsigned chip, uint8_t mask)
{
    muhtari__pokey_write(m, chip, 14u, mask, m->bus.ticks);
}

static void timer_sources(void)
{
    for (unsigned chip = 0u; chip < 2u; chip++) {
        for (unsigned bit = 0u; bit < 3u; bit++) {
            uint8_t mask = (uint8_t)(1u << bit);
            unsigned fire = bit == 0u ? 8u : 24u;
            muhtari *m = fresh(chip, (uint8_t)(1u << chip));
            write_enable(m, chip, mask);
            for (unsigned t = 1u; t <= fire + 2u; t++) {
                cycles(m, 1u);
                CHECK_EQ_U64(muhtari__pokey_read(m, chip, 14u, m->bus.ticks),
                             t < fire ? 255u : (uint8_t)~mask);
                CHECK_EQ_U64(m->bus.irq_level, t >= fire + 2u);
            }
            uint8_t before = m->pokey[chip].irqst;
            (void)muhtari__pokey_read(m, chip, 14u, m->bus.ticks);
            CHECK_EQ_U64(m->pokey[chip].irqst, before); /* no read-to-ack */
            write_enable(m, chip, 0u);
            CHECK_EQ_U64(m->pokey[chip].irqst, 255u);
            cycles(m, 1u);
            CHECK_EQ_U64(m->bus.irq_level, 1u); /* signaled pipeline survives */
            cycles(m, 2u);
            CHECK_EQ_U64(m->bus.irq_level, 0u);
        }
    }
    /* Timer3 fires, but has no IRQ. Unsupported peripheral sources stay high. */
    muhtari *m = fresh(0u, 1u);
    write_enable(m, 0u, 0xF8u);
    cycles(m, 100u);
    CHECK(m->pokey[0].channel[2].fires > 0u);
    CHECK_EQ_U64(m->pokey[0].irqst, 255u);
    CHECK_EQ_U64(m->bus.irq_level, 0u);
}

static void enable_edges(void)
{
    /* Enable atT4 qualifies for the first T8 event; T5 is too late. */
    for (unsigned at = 4u; at <= 5u; at++) {
        muhtari *m = fresh(0u, 1u);
        cycles(m, at);
        write_enable(m, 0u, 1u);
        cycles(m, 8u - at);
        CHECK_EQ_U64(m->pokey[0].irqst, at == 4u ? 254u : 255u);
        cycles(m, 2u);
        CHECK_EQ_U64(m->bus.irq_level, at == 4u);
    }
    /* Disable two cycles before an event blocks it. One cycle before can
       still deliver an already propagating IRQ, even with IRQST cleared. */
    for (unsigned at = 6u; at <= 8u; at++) {
        muhtari *m = fresh(0u, 1u);
        write_enable(m, 0u, 1u);
        cycles(m, at);
        write_enable(m, 0u, 0u);
        cycles(m, 10u - at);
        CHECK_EQ_U64(m->pokey[0].irqst, 255u);
        CHECK_EQ_U64(m->bus.irq_level, at != 6u);
        cycles(m, 3u);
        CHECK_EQ_U64(m->bus.irq_level, 0u);
    }
    /* A masked event is lost; enabling afterward waits for another pulse. */
    muhtari *m = fresh(0u, 1u);
    cycles(m, 8u);
    write_enable(m, 0u, 1u);
    cycles(m, 3u);
    CHECK_EQ_U64(m->pokey[0].irqst, 255u);
    CHECK_EQ_U64(m->bus.irq_level, 0u);
    cycles(m, 3u);
    CHECK_EQ_U64(m->bus.irq_level, 1u);
}

static void wired_and_held(void)
{
    for (unsigned chip = 0u; chip < 2u; chip++) {
        for (unsigned wiring = 0u; wiring < 4u; wiring++) {
            muhtari *m = fresh(chip, (uint8_t)wiring);
            /* Lone $440 retains physical bit1 even with no primary. */
            if (chip != 0u) {
                m->cart.pokey_at[0] = 0u;
                m->synth.pokeys = 1u;
                m->synth.secondary = 1u;
            }
            write_enable(m, chip, 1u);
            muhtari__pokey_write(m, chip, 15u, 0u, 0u); /* init, muted fast timer runs */
            m->bus.rdy_release = 40u;
            cycles(m, 1u);
            CHECK_EQ_U64(m->bus.phi2, 11u);
            CHECK_EQ_U64(m->bus.irq_level, (wiring & (1u << chip)) != 0u);
            CHECK_EQ_U64(m->pokey[chip].irqst, 254u);
        }
    }
}

int main(void)
{
    timer_sources();
    enable_edges();
    wired_and_held();
    CHECK_DONE();
}
