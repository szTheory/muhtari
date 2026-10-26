/* Documentary timer status and CPU-visible propagation through the shipped
   bus. Altirra manual sections3.4/5.7; hardware/pokey.md, Interrupts. */
#include <string.h>
#include "internal.h"
#include "../check.h"
#include <stdlib.h>
#include "../cart/images.h"

#define CART_SIZE (IMAGES_HEADER + 4096u)
static uint8_t cart[CART_SIZE], video[320u * 242u];
static int16_t audio[801];
#define WIDTH 320u
#define AUDIO_CAP 801u
static void *alloc(void *u, size_t n)
{
    (void)u;
    return malloc(n);
}
static void release(void *u, void *p, size_t n)
{
    (void)u;
    (void)n;
    free(p);
}
static muhtari *make(void)
{
    muhtari_config c = {0};
    c.size = sizeof c;
    c.allocator.alloc = alloc;
    c.allocator.free = release;
    muhtari *m = NULL;
    CHECK_EQ_U64(muhtari_create(&c, &m), 0);
    return m;
}
static muhtari_media media_for(const void *p, size_t n)
{
    muhtari_media d = {0};
    d.size = sizeof d;
    d.image = p;
    d.image_size = n;
    return d;
}
static void frame_io(muhtari_frame *f, uint8_t *v, uint32_t pitch, int16_t *a, uint32_t cap)
{
    memset(f, 0, sizeof *f);
    f->size = sizeof *f;
    f->video = v;
    f->video_pitch = pitch;
    f->audio = a;
    f->audio_capacity = cap;
}

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
            CHECK_EQ_U64(m->bus.irq_level, 0u); /* held clocks do not synchronize CPU */
            cycles(m, 2u);
            CHECK_EQ_U64(m->bus.irq_level, (wiring & (1u << chip)) != 0u);
            CHECK_EQ_U64(m->pokey[chip].irqst, 254u);
        }
    }
}

static void shared_line(void)
{
    muhtari *m = fresh(0u, 3u);
    muhtari__pokey_write(m, 1u, 8u, 64u, 0u);
    write_enable(m, 0u, 1u);
    write_enable(m, 1u, 1u);
    cycles(m, 10u);
    CHECK_EQ_U64(m->bus.irq_level, 1u);
    write_enable(m, 0u, 0u);
    cycles(m, 3u);
    CHECK_EQ_U64(m->bus.irq_level, 1u);
    CHECK_EQ_U64(m->pokey[0].irqst, 255u);
    CHECK_EQ_U64(m->pokey[1].irqst, 254u);
    write_enable(m, 1u, 0u);
    cycles(m, 3u);
    CHECK_EQ_U64(m->bus.irq_level, 0u);
}

static void test_cart_irq(void)
{
    static const uint8_t base_program[] = {
        0x78,                      /* SEI */
        0xA9, 7,    0x85, 1,       /* lock 7800 */
        0xA9, 64,   0x8D, 0x58, 4, /* AUDCTL: fast timer1 */
        0xA9, 20,   0x8D, 0x50, 4, /* AUDF1 */
        0xA9, 0xAF, 0x8D, 0x51, 4, /* audible square */
        0xA9, 1,    0x8D, 0x5E, 4, /* IRQEN */
        0x8D, 0x59, 4,             /* STIMER */
        0x58, 0xEA,                /* CLI; NOP */
        0x4C, 0x1E, 0xF0           /* JMP self */
    };
    static const uint8_t base_handler[] = {
        0xEE, 0, 0x18,          /* INC $1800 */
        0xA9, 0, 0x8D, 0x5E, 4, /* acknowledge */
        0x40                    /* RTI */
    };
    for (unsigned layout = 0u; layout < 4u; layout++)
        for (unsigned source = 0u; source < 3u; source++)
            for (unsigned mode = 0; mode < 4u; mode++) {
                unsigned chip = layout == 1u || layout == 3u;
                uint8_t program[sizeof base_program], handler[sizeof base_handler];
                memcpy(program, base_program, sizeof program);
                memcpy(handler, base_handler, sizeof handler);
                program[6] = 0x60u;
                program[21] = (uint8_t)(1u << source);
                if (chip != 0u) {
                    static const unsigned operands[] = {8u, 13u, 18u, 23u, 26u};
                    for (unsigned i = 0u; i < 5u; i++)
                        program[operands[i]] = (uint8_t)(program[operands[i]] - 16u);
                    handler[6] = (uint8_t)(handler[6] - 16u);
                }
                muhtari *m = make();
                images_header(cart, 3u, "IRQ", 4096u, 0u);
                memset(cart + IMAGES_HEADER, 0xEA, 4096u);
                memcpy(cart + IMAGES_HEADER, program, sizeof program);
                cart[CART_SIZE - 4u] = 0;
                cart[CART_SIZE - 3u] = 0xF0;
                cart[0] = mode == 2u ? 3u : 4u;
                cart[53] = chip != 0u ? 4u : 0u;
                cart[54] = chip != 0u ? 0u : 0x40u; /* legacy fallback at the selected chip */
                cart[67] = layout >= 2u ? 3u : chip != 0u ? 1u : 2u;
                cart[69] = mode == 1u ? 0u : (uint8_t)(1u << chip);
                cart[65] = mode == 3u ? 0xFFu : 0u; /* ignored v4 */
                memcpy(cart + IMAGES_HEADER + 0x100u, handler, sizeof handler);
                cart[CART_SIZE - 2u] = 0u;
                cart[CART_SIZE - 1u] = 0xF1u;
                muhtari_media media = media_for(cart, sizeof cart);
                CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
                m->cpu.s = 0xFDu; /* controlled stack for documentary pushes */
                m->ram[0] = 0u;
                /* Stop exactly after entry to inspect hardware pushes. */
                for (unsigned step = 0; step < 200u && m->cpu.pc != 0xF100u; step++)
                    muhtari__cpu_step(m);
                CHECK_EQ_U64(m->cpu.pc == 0xF100u, mode == 0u);
                if (mode == 0u) {
                    CHECK_EQ_U64(m->cpu.s, 0xFAu);
                    CHECK_EQ_U64(muhtari__bus_peek(m, 0x1FDu), 0xF0u);
                    CHECK_EQ_U64(muhtari__bus_peek(m, 0x1FCu), 0x1Eu);
                    CHECK_EQ_U64(muhtari__bus_peek(m, 0x1FBu) & 0x34u, 0x20u);
                }
                muhtari_frame io;
                frame_io(&io, video, WIDTH, audio, AUDIO_CAP);
                CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_OK);
                CHECK_EQ_U64(io.audio_count, 54u);
                CHECK_EQ_U64(muhtari__bus_peek(m, 0x1800u), mode == 0u ? 1u : 0u);
                int nonzero = 0;
                for (uint32_t i = 0; i < io.audio_count; i++)
                    nonzero |= audio[i] != 0;
                CHECK(nonzero);
                CHECK(m->bus.ticks - m->frame_start <= MUHTARI__SYNTH_OVERSHOOT_TICKS);
                muhtari_destroy(m);
            }
}

static void expansion_irq(void)
{
    /* A real CPU enables timer1, hides its register window, then takes its
       declared interrupt. The handler leaves it pending for observation. */
    static const uint8_t program[] = {
        0x78, 0xA9, 16, 0x8D, 0x70, 4,
        0xA9, 64, 0x8D, 0x58, 4, 0xA9, 20, 0x8D, 0x50, 4,
        0xA9, 1, 0x8D, 0x5E, 4, 0x8D, 0x59, 4,
        0xA9, 0, 0x8D, 0x70, 4, 0x58, 0xEA, 0x4C, 0x1F, 0xF0
    };
    for (uint32_t region = MUHTARI_REGION_NTSC; region <= MUHTARI_REGION_PAL; region++) {
        for (unsigned wired = 0u; wired < 2u; wired++) {
            muhtari *m = make();
            images_header(cart, 4u, "Gated timer", 4096u, 0u);
            memset(cart + IMAGES_HEADER, 0xEA, 4096u);
            memcpy(cart + IMAGES_HEADER, program, sizeof program);
            cart[67] = 2u;
            cart[69] = (uint8_t)wired;
            cart[CART_SIZE - 4u] = 0u;
            cart[CART_SIZE - 3u] = 0xF0u;
            cart[CART_SIZE - 2u] = 0u;
            cart[CART_SIZE - 1u] = 0xF1u;
            muhtari_media media = media_for(cart, sizeof cart);
            media.region = region;
            media.expansion = MUHTARI_EXPANSION_XM;
            CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
            for (unsigned step = 0u; step < 200u && m->cpu.pc != 0xF100u; step++)
                muhtari__cpu_step(m);
            CHECK_EQ_U64(m->cpu.pc == 0xF100u, wired);
            CHECK_EQ_U64(m->cart.expansion.control, 0u);
            CHECK_EQ_U64(m->pokey[0].irqst, 0xFEu);
            CHECK_EQ_U64(m->bus.irq_level, wired);
            muhtari__bus_write(m, 0x045Eu, 0u);
            CHECK_EQ_U64(m->pokey[0].irqst, 0xFEu);
            muhtari__bus_write(m, 0x0470u, 16u);
            CHECK_EQ_U64(muhtari__bus_read(m, 0x045Eu), 0xFEu);
            muhtari_destroy(m);
        }
    }
}

int main(void)
{
    expansion_irq();
    test_cart_irq();
    timer_sources();
    enable_edges();
    wired_and_held();
    shared_line();
    CHECK_DONE();
}
