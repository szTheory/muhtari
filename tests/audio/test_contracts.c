/* Frame-audio contracts through the shipped archive. Cartridges and BIOS
   programs are generated in memory; timing follows docs/embedding.md. */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "internal.h"
#include "../cart/images.h"
#include "../check.h"

#define AUDIO_CAP 801u
#define AUDIO_SENTINEL 0x5A5A
#define TICKS_PER_FRAME 119402u

/* The plain malloc allocator, as every host defines it. */
static void *test_alloc(void *user, size_t size)
{
    (void)user;
    return malloc(size);
}

static void test_free(void *user, void *ptr, size_t size)
{
    (void)user;
    (void)size;
    free(ptr);
}

static muhtari *make(void)
{
    muhtari_config cfg;
    memset(&cfg, 0, sizeof cfg);
    cfg.size = (uint32_t)sizeof cfg;
    cfg.allocator.alloc = test_alloc;
    cfg.allocator.free = test_free;
    muhtari *m = NULL;
    CHECK_EQ_U64(muhtari_create(&cfg, &m), MUHTARI_STATUS_OK);
    return m;
}

/* Fills *io in place after a memset, so no padding is left unspecified. */
static void frame_io(muhtari_frame *io, uint8_t *v, uint32_t pitch, int16_t *a, uint32_t cap)
{
    memset(io, 0, sizeof *io);
    io->size = (uint32_t)sizeof *io;
    io->video = v;
    io->video_pitch = pitch;
    io->audio = a;
    io->audio_capacity = cap;
}

/* A 4 KiB linear cartridge built in memory: an A78 version 3 header with
   ProLine on both ports and the TV byte tv, over a body that runs program,
   n bytes at $F000, which the reset vector points at. */
#define CART_BODY 4096u
#define CART_SIZE (IMAGES_HEADER + CART_BODY)

static void make_cart(uint8_t *buf, const uint8_t *program, size_t n, uint8_t tv)
{
    images_header(buf, 3u, "Core", CART_BODY, 0u);
    buf[57] = tv;
    uint8_t *body = buf + IMAGES_HEADER;
    memset(body, 0xEA, CART_BODY); /* NOP */
    memcpy(body, program, n);
    body[CART_BODY - 4u] = 0x00u; /* reset vector $F000 */
    body[CART_BODY - 3u] = 0xF0u;
}

static uint8_t cart[CART_SIZE];
static uint8_t bios[16384];

static muhtari_media media_for(const void *image, size_t n)
{
    muhtari_media media;
    memset(&media, 0, sizeof media);
    media.size = (uint32_t)sizeof media;
    media.image = image;
    media.image_size = n;
    return media;
}

/* Loads the cartridge running program into m. */
static void load_program(muhtari *m, const uint8_t *program, size_t n)
{
    make_cart(cart, program, n, 0u);
    muhtari_media media = media_for(cart, sizeof cart);
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
}

static const uint8_t tone[] = {
    0xA9, 0x07, 0x85, 0x01, /* lock the 7800 decode */
    0xA9, 0x04, 0x85, 0x15, /* AUDC0 div2 */
    0xA9, 0x05, 0x85, 0x17, /* AUDF0 */
    0xA9, 0x0F, 0x85, 0x19, /* AUDV0 */
    0x4C, 0x10, 0xF0        /* JMP $F010 */
};

static void load_audio_program(muhtari *m)
{
    load_program(m, tone, sizeof tone);
}

/* Deadline ownership and the finite FIFO bound use exact tick injections,
   including a full frame followed by the entire conservative overshoot. */
static void test_audio_boundaries(void)
{
    muhtari *a = make(), *b = make();
    int16_t whole[AUDIO_CAP], split[AUDIO_CAP];
    for (unsigned offset = 0u; offset < 3u; offset++) {
        const uint64_t end = TICKS_PER_FRAME;
        const uint64_t event = end - 1u + offset;
        muhtari__synth_reset(a, 0u, 0u);
        muhtari__synth_reset(b, 0u, 0u);
        muhtari__synth_level(a, event, 0u, 15u);
        muhtari__synth_level(b, event, 0u, 15u);
        CHECK_EQ_U64(muhtari__synth_render(a, end, whole), 800u);
        CHECK_EQ_U64(muhtari__synth_render(b, end, NULL), 800u);
        CHECK_EQ_U64(a->synth.pending, offset == 0u ? 0u : 1u);
        CHECK_EQ_U64(a->synth.mixed_level, offset == 0u ? 12241u : 0u);
        for (unsigned i = 0u; i < 800u; i++) {
            CHECK_EQ_U64(whole[i], 0u);
        }
        uint32_t n = muhtari__synth_render(a, end + 13125u, whole);
        CHECK_EQ_U64(muhtari__synth_render(b, end + 13125u, split), n);
        CHECK_EQ_U64(n, 88u);
        CHECK(memcmp(whole, split, n * sizeof whole[0]) == 0);
        CHECK(a->synth.pre_clip_peak > 0u);
        CHECK(memcmp(&a->synth, &b->synth, sizeof a->synth) == 0);
    }
    const uint32_t future = TICKS_PER_FRAME + MUHTARI__SYNTH_OVERSHOOT_TICKS;
    for (unsigned path = 0u; path < 2u; path++) {
        muhtari *m = path == 0u ? a : b;
        muhtari__synth_reset(m, 0u, 0u);
        for (uint32_t tick = 0u; tick < future; tick += 17u) {
            /* Before each flush, account for all samples it could append.
               This fails as a test assertion even in NDEBUG builds. */
            uint64_t produced = (uint64_t)tick * 88u / 13125u;
            CHECK(produced <= MUHTARI__SYNTH_PCM_CAPACITY);
            if (produced > MUHTARI__SYNTH_PCM_CAPACITY) {
                break;
            }
            muhtari__synth_level(m, tick, 0u, (uint16_t)((tick / 17u & 1u) * 15u));
        }
        CHECK(m->synth.pcm_count <= MUHTARI__SYNTH_PCM_CAPACITY);
    }
    CHECK_EQ_U64(muhtari__synth_render(a, TICKS_PER_FRAME, whole), 800u);
    CHECK_EQ_U64(muhtari__synth_render(b, TICKS_PER_FRAME, NULL), 800u);
    CHECK(a->synth.pcm_count > 0u);
    CHECK(a->synth.pcm_count <= MUHTARI__SYNTH_CARRY_SAMPLES);
    uint32_t n = muhtari__synth_render(a, future, whole);
    CHECK_EQ_U64(muhtari__synth_render(b, future, split), n);
    CHECK(memcmp(whole, split, n * sizeof whole[0]) == 0);
    CHECK(memcmp(&a->synth, &b->synth, sizeof a->synth) == 0);
    muhtari_destroy(b);
    muhtari_destroy(a);
}

/* Public PCM must follow cartridge writes even when the steady loop never
   touches the TIA again. The absolute chip epoch and output grid have
   different fast-boot origins (docs/embedding.md, The first frame). */
static void test_public_audio(void)
{
    muhtari *m = make(), *discard = make(), *reference = make(), *muted = make();
    load_program(m, tone, sizeof tone);
    load_program(discard, tone, sizeof tone);
    load_program(reference, tone, sizeof tone);
    static const uint8_t idle[] = {0x4C, 0x00, 0xF0};
    load_program(muted, idle, sizeof idle);
    const uint64_t origin = 240u * 454u + 24u;
    CHECK_EQ_U64(m->bus.ticks, origin);
    CHECK_EQ_U64(m->bus.phi2, 0u);
    CHECK_EQ_U64(m->bus.clock_origin_tick, origin);
    CHECK_EQ_U64(m->bus.clock_origin_phi2, 0u);
    CHECK_EQ_U64(m->synth.origin, origin);
    /* Independently advance the hidden zero-volume startup to the handoff. */
    muhtari *hidden = make();
    muhtari__tia_reset(hidden);
    muhtari__tia_catch_up(hidden, origin);
    CHECK(memcmp(&m->tia, &hidden->tia, sizeof m->tia) == 0);
    muhtari_destroy(hidden);

    int16_t expected[AUDIO_CAP], actual[AUDIO_CAP], after_discard[AUDIO_CAP], first[54];
    uint32_t rem = 0u;
    uint64_t total = 0u;
    for (unsigned f = 0u; f <= 1000u; f++) {
        uint64_t end = m->frame_deadline;
        uint32_t elapsed = f == 0u ? 8190u : TICKS_PER_FRAME;
        uint32_t count = (rem + elapsed * 88u) / 13125u;
        rem = (rem + elapsed * 88u) % 13125u;
        while (reference->bus.ticks < end) {
            muhtari__cpu_step(reference);
        }
        muhtari__tia_catch_up(reference, reference->bus.ticks);
        uint32_t written = 0u;
        const uint64_t start = reference->synth.horizon;
        static const uint32_t cuts[] = {1u, 149u, 150u, 2387u, 8190u};
        for (unsigned c = 0u; c < sizeof cuts / sizeof cuts[0]; c++) {
            if (start + cuts[c] < end) {
                written += muhtari__synth_render(reference, start + cuts[c], expected + written);
            }
        }
        written += muhtari__synth_render(reference, end, expected + written);
        CHECK_EQ_U64(written, count);
        muhtari_frame io;
        struct muhtari before = *m;
        frame_io(&io, NULL, 0u, actual, count - 1u);
        CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_ERR_BUFFER_TOO_SMALL);
        CHECK_EQ_U64(io.audio_count, 0u);
        CHECK(memcmp(m, &before, sizeof before) == 0);
        if (f % 37u == 0u) {
            frame_io(&io, NULL, 0u, NULL, 0u);
            CHECK_EQ_U64(muhtari_run_frame(muted, &io), MUHTARI_STATUS_OK);
            CHECK(memcmp(m, &before, sizeof before) == 0);
            CHECK_EQ_U64(muted->synth.pre_clip_peak, 0u);
        }
        frame_io(&io, NULL, 0u, actual, count);
        CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_OK);
        CHECK_EQ_U64(io.audio_count, count);
        total += io.audio_count;
        CHECK_EQ_U64(m->audio_rem, rem);
        CHECK(memcmp(actual, expected, count * sizeof actual[0]) == 0);
        int nonzero = 0;
        for (uint32_t i = 0u; i < count; i++) {
            nonzero |= actual[i] != 0;
        }
        CHECK(nonzero);
        if (f == 0u) {
            CHECK_EQ_U64(count, 54u);
            memcpy(first, actual, sizeof first);
        }
        if (f == 500u) {
            muhtari_input in;
            memset(&in, 0, sizeof in);
            in.size = (uint32_t)sizeof in;
            in.switches = MUHTARI_SWITCH_RESET;
            CHECK_EQ_U64(muhtari_set_input(discard, &in), MUHTARI_STATUS_OK);
        }
        frame_io(&io, NULL, 0u, f % 2u == 0u ? NULL : after_discard, f % 2u == 0u ? 0u : count);
        CHECK_EQ_U64(muhtari_run_frame(discard, &io), MUHTARI_STATUS_OK);
        CHECK_EQ_U64(io.audio_count, count);
        if (f % 2u != 0u) {
            CHECK(memcmp(actual, after_discard, count * sizeof actual[0]) == 0);
        }
        CHECK_EQ_U64(m->bus.ticks, reference->bus.ticks);
        CHECK_EQ_U64(m->bus.phi2, reference->bus.phi2);
        CHECK_EQ_U64(m->synth.phase, reference->synth.phase);
        CHECK_EQ_U64(m->synth.blocker_q30, reference->synth.blocker_q30);
        CHECK(memcmp(m->synth.impulse, reference->synth.impulse, sizeof m->synth.impulse) == 0);
        CHECK_EQ_U64(m->synth.integrator_q23, reference->synth.integrator_q23);
        CHECK(memcmp(&m->synth, &discard->synth, sizeof m->synth) == 0);
        CHECK(memcmp(&m->tia, &discard->tia, sizeof m->tia) == 0);
        if (f >= 500u) {
            CHECK_EQ_U64(muhtari__bus_peek(discard, 0x0282u) & 1u, 0u);
            CHECK_EQ_U64(muhtari__bus_peek(m, 0x0282u) & 1u, 1u);
        }
        CHECK(m->synth.pcm_count <= MUHTARI__SYNTH_CARRY_SAMPLES);
    }
    CHECK_EQ_U64(total, (UINT64_C(8190) + UINT64_C(1000) * TICKS_PER_FRAME) * 88u / 13125u);
    CHECK_EQ_U64(muhtari_power_cycle(m), MUHTARI_STATUS_OK);
    muhtari *fresh = make();
    load_audio_program(fresh);
    CHECK(memcmp(&m->tia, &fresh->tia, sizeof m->tia) == 0);
    CHECK(memcmp(&m->synth, &fresh->synth, sizeof m->synth) == 0);
    CHECK_EQ_U64(m->audio_rem, 0u);
    muhtari_destroy(fresh);
    muhtari_frame io;
    frame_io(&io, NULL, 0u, actual, AUDIO_CAP);
    CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(io.audio_count, 54u);
    CHECK(memcmp(actual, first, sizeof first) == 0);

    /* A held final instruction plus interrupt entry crosses the horizon.
       Compare the public prefix/tail with an explicitly drained reference;
       both WSYNC-style RDY and a DMA-style HALT window run real bus cycles. */
    CHECK_EQ_U64(muhtari_power_cycle(m), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(muhtari_power_cycle(reference), MUHTARI_STATUS_OK);
    uint64_t end = m->frame_deadline;
    for (unsigned instance = 0u; instance < 2u; instance++) {
        muhtari *held = instance == 0u ? m : reference;
        while (held->bus.ticks < end - 40u) {
            muhtari__cpu_step(held);
        }
        held->ram[0] = 0x4Cu; /* JMP $1800, also the NMI handler */
        held->ram[1] = 0x00u;
        held->ram[2] = 0x18u;
        held->cpu.pc = 0x1800u;
        held->cpu.s = 0xFFu;
        held->cart.rom[CART_BODY - 6u] = 0x00u;
        held->cart.rom[CART_BODY - 5u] = 0x18u;
        muhtari__bus_write(held, 0x0002u, 0u); /* real TIA WSYNC */
        held->bus.rdy_release = end + 400u;
        CHECK_EQ_U64(muhtari__bus_request_halt(held, end + 28u, end + 450u), 1u);
        CHECK_EQ_U64(muhtari__bus_raise_nmi(held, end - 4u), 1u);
    }
    while (reference->bus.ticks < end) {
        muhtari__cpu_step(reference);
    }
    muhtari__tia_catch_up(reference, reference->bus.ticks);
    CHECK_EQ_U64(muhtari__synth_render(reference, end, expected), 54u);
    CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(io.audio_count, 54u);
    CHECK(memcmp(actual, expected, 54u * sizeof actual[0]) == 0);
    CHECK(m->bus.ticks > end);
    CHECK(m->bus.ticks - end <= MUHTARI__SYNTH_OVERSHOOT_TICKS);
    CHECK_EQ_U64(m->bus.ticks, reference->bus.ticks);
    CHECK_EQ_U64(m->bus.phi2, reference->bus.phi2);
    CHECK_EQ_U64(m->cpu.pc, 0x1800u);
    CHECK_EQ_U64(m->bus.nmi_phi2, 0u);
    CHECK(m->synth.pcm_count > 0u || (m->synth.pending != 0u && m->synth.pending_tick >= end));
    CHECK(m->synth.pcm_count <= MUHTARI__SYNTH_CARRY_SAMPLES);
    end = m->frame_deadline;
    while (reference->bus.ticks < end) {
        muhtari__cpu_step(reference);
    }
    muhtari__tia_catch_up(reference, reference->bus.ticks);
    uint32_t count = muhtari__synth_render(reference, end, expected);
    CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(io.audio_count, count);
    CHECK(memcmp(actual, expected, count * sizeof actual[0]) == 0);

    /* JAM must still clock audio and return; no instruction can hang a
       frame or consume beyond the separately derived carry bound. */
    m->cpu.jammed = 1u;
    reference->cpu.jammed = 1u;
    end = m->frame_deadline;
    while (reference->bus.ticks < end) {
        muhtari__cpu_step(reference);
    }
    muhtari__tia_catch_up(reference, reference->bus.ticks);
    count = muhtari__synth_render(reference, end, expected);
    CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(io.audio_count, count);
    CHECK(memcmp(actual, expected, count * sizeof actual[0]) == 0);
    CHECK_EQ_U64(m->bus.ticks, reference->bus.ticks);
    CHECK(m->bus.ticks - end <= MUHTARI__SYNTH_OVERSHOOT_TICKS);

    /* A tiny in-memory BIOS selects cartridge space without resetting the
       audio clock or reconstruction at the handoff. RES is real bus time. */
    memset(bios, 0xEA, sizeof bios);
    static const uint8_t boot[] = {0xA9, 0x07, 0x85, 0x01};
    memcpy(bios + 0x3000u, boot, sizeof boot);
    make_cart(cart, tone, sizeof tone, 0u);
    bios[sizeof bios - 4u] = 0x00u;
    bios[sizeof bios - 3u] = 0xF0u;
    muhtari_media media = media_for(cart, sizeof cart);
    media.bios = bios;
    media.bios_size = sizeof bios;
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(m->synth.origin, 0u);
    CHECK_EQ_U64(m->bus.clock_origin_tick, 0u);
    CHECK_EQ_U64(m->bus.clock_origin_phi2, 0u);
    CHECK_EQ_U64(m->audio_rem, 0u);
    CHECK_EQ_U64(m->bus.phi2, 7u);
    CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_OK);
    CHECK_EQ_U64(io.audio_count, 785u);
    CHECK_EQ_U64(m->synth.origin, 0u);
    int nonzero = 0;
    for (uint32_t i = 0u; i < io.audio_count; i++) {
        nonzero |= actual[i] != 0;
    }
    CHECK(nonzero);

    /* Audible BIOS work may precede the sticky 2600 refusal. No tentative
       samples may overwrite the caller buffer on either refused call. */
    static const uint8_t refusal[] = {
        0xA9, 0x00, 0x85, 0x15,       /* AUDC0 constant, EXT remains clear */
        0xA9, 0x0A, 0x85, 0x19,       /* audible even volume, no INPTCTRL lock */
        0xA2, 0xFF, 0xCA, 0xD0, 0xFD, /* delay */
        0xA9, 0xFD, 0x85, 0x01        /* lock MEN clear: refusal */
    };
    memcpy(bios + 0x3000u, refusal, sizeof refusal);
    CHECK_EQ_U64(muhtari_load(m, &media), MUHTARI_STATUS_OK);
    for (uint32_t i = 0u; i < AUDIO_CAP; i++) {
        actual[i] = AUDIO_SENTINEL;
    }
    for (unsigned call = 0u; call < 2u; call++) {
        CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_ERR_BIOS_REJECTED_CART);
        CHECK_EQ_U64(io.audio_count, 0u);
        for (uint32_t i = 0u; i < AUDIO_CAP; i++) {
            CHECK_EQ_U64(actual[i], AUDIO_SENTINEL);
        }
    }
    CHECK(m->synth.raw[0] > 0u);
    muhtari_destroy(muted);
    muhtari_destroy(reference);
    muhtari_destroy(discard);
    muhtari_destroy(m);
}

int main(void)
{
    test_audio_boundaries();
    test_public_audio();
    CHECK_DONE();
}
