/* Frame-audio contracts through the shipped archive. Cartridges and BIOS
   programs are generated in memory; timing follows docs/embedding.md. */
#include <stdint.h>
#include <stdio.h>
#include "../../runner/sha256.h"
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
        CHECK_EQ_U64(a->synth.mixed_level, offset == 0u ? 12240u : 0u);
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
    muhtari *before = malloc(sizeof *before);
    CHECK(before != NULL);
    if (before == NULL) {
        muhtari_destroy(muted);
        muhtari_destroy(reference);
        muhtari_destroy(discard);
        muhtari_destroy(m);
        return;
    }
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
        memcpy(before, m, sizeof *before);
        frame_io(&io, NULL, 0u, actual, count - 1u);
        CHECK_EQ_U64(muhtari_run_frame(m, &io), MUHTARI_STATUS_ERR_BUFFER_TOO_SMALL);
        CHECK_EQ_U64(io.audio_count, 0u);
        CHECK(memcmp(m, before, sizeof *before) == 0);
        if (f % 37u == 0u) {
            frame_io(&io, NULL, 0u, NULL, 0u);
            CHECK_EQ_U64(muhtari_run_frame(muted, &io), MUHTARI_STATUS_OK);
            CHECK(memcmp(m, before, sizeof *before) == 0);
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
    for (unsigned interrupt = 0u; interrupt < 2u; interrupt++) {
        CHECK_EQ_U64(muhtari_power_cycle(m), MUHTARI_STATUS_OK);
        CHECK_EQ_U64(muhtari_power_cycle(reference), MUHTARI_STATUS_OK);
        uint64_t end = m->frame_deadline;
        for (unsigned instance = 0u; instance < 2u; instance++) {
            muhtari *held = instance == 0u ? m : reference;
            if (interrupt != 0u) {
                held->cart.pokey_at[0] = 0x450u;
                held->cart.irq_mask = 1u;
                held->synth.pokeys = 1u;
                muhtari__pokey_write(held, 0u, 8u, 64u, held->bus.ticks);
            }
            while (held->bus.ticks < end - 40u) {
                muhtari__cpu_step(held);
            }
            held->ram[0] = 0x6Cu; /* JMP ($1805), also the interrupt handler */
            held->ram[1] = 0x05u;
            held->ram[2] = 0x18u;
            held->ram[5] = 0u;
            held->ram[6] = 0x18u;
            held->cpu.pc = 0x1800u;
            held->cpu.s = 0xFFu;
            held->cart.rom[CART_BODY - 6u] = 0x00u;
            held->cart.rom[CART_BODY - 5u] = 0x18u;
            muhtari__bus_write(held, 0x0002u, 0u); /* real TIA WSYNC */
            held->bus.rdy_release = end + 400u;
            CHECK_EQ_U64(muhtari__bus_request_halt(held, end + 28u, end + 450u), 1u);
            if (interrupt == 0u) {
                CHECK_EQ_U64(muhtari__bus_raise_nmi(held, end - 4u), 1u);
            } else {
                held->cpu.p &= 0xFBu;
                held->cart.rom[CART_BODY - 2u] = 0u;
                held->cart.rom[CART_BODY - 1u] = 0x18u;
                muhtari__pokey_write(held, 0u, 14u, 1u, held->bus.ticks);
            }
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
        CHECK_EQ_U64(m->cpu.s, 0xFCu);
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
    }

    /* JAM must still clock audio and return; no instruction can hang a
       frame or consume beyond the separately derived carry bound. */
    m->cpu.jammed = 1u;
    reference->cpu.jammed = 1u;
    uint64_t end = m->frame_deadline;
    while (reference->bus.ticks < end) {
        muhtari__cpu_step(reference);
    }
    muhtari__tia_catch_up(reference, reference->bus.ticks);
    uint32_t count = muhtari__synth_render(reference, end, expected);
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
    free(before);
    muhtari_destroy(muted);
    muhtari_destroy(reference);
    muhtari_destroy(discard);
    muhtari_destroy(m);
}

static unsigned observed;
static uint64_t observed_tick;
static void record_one(void *user, uint64_t tick, uint8_t source, uint16_t raw)
{
    CHECK(user == &observed);
    CHECK_EQ_U64(source, 0u);
    CHECK_EQ_U64(raw, 9u);
    observed++;
    observed_tick = tick;
}
static void test_observer_one(void)
{
    static const uint8_t program[] = {0xA9, 7, 0x85, 1, 0xA9, 9, 0x85, 0x19, 0x02};
    muhtari *m = make(), *off = make();
    CHECK_EQ_U64(muhtari__synth_observe(m, record_one, &observed), 0);
    load_program(m, program, sizeof program);
    load_program(off, program, sizeof program);
    CHECK_EQ_U64(muhtari__synth_observe(off, record_one, &observed), MUHTARI_STATUS_ERR_ARGUMENT);
    CHECK(off->observation == NULL);
    /* Fast boot has already filled the constant waveform's pulse register. */
    m->tia.channel[0].output = off->tia.channel[0].output = 1u;
    muhtari_frame a, b;
    int16_t pcm[801], reference[801];
    frame_io(&a, NULL, 0u, pcm, 801u);
    frame_io(&b, NULL, 0u, reference, 801u);
    a.audio_capacity = 0u;
    CHECK_EQ_U64(muhtari_run_frame(m, &a), MUHTARI_STATUS_ERR_BUFFER_TOO_SMALL);
    CHECK_EQ_U64(observed, 0u);
    a.audio_capacity = 801u;
    CHECK_EQ_U64(muhtari_run_frame(m, &a), 0);
    CHECK_EQ_U64(muhtari_run_frame(off, &b), 0);
    CHECK_EQ_U64(observed, 1u);
    CHECK(observed_tick >= m->synth.origin && observed_tick < m->frame_start);
    CHECK_EQ_U64(a.audio_count, b.audio_count);
    CHECK(memcmp(pcm, reference, a.audio_count * sizeof pcm[0]) == 0);
    CHECK(memcmp(&m->cpu, &off->cpu, sizeof m->cpu) == 0);
    CHECK(memcmp(&m->synth, &off->synth, sizeof m->synth) == 0);
    CHECK(memcmp(m->ram, off->ram, sizeof m->ram) == 0);
    CHECK_EQ_U64(muhtari_power_cycle(m), 0);
    CHECK(m->observer == record_one && m->observer_user == &observed);
    muhtari_destroy(off);
    muhtari_destroy(m);
}

struct event {
    uint64_t tick;
    uint16_t level;
    uint8_t source;
};
struct events {
    unsigned count;
    struct event item[20000];
};
static struct events streams[2];
static void collect(void *user, uint64_t tick, uint8_t source, uint16_t raw)
{
    struct events *e = user;
    CHECK(e->count < 20000u);
    if (e->count >= 20000u)
        return;
    e->item[e->count].tick = tick;
    e->item[e->count].source = source;
    e->item[e->count++].level = raw;
}
static void test_observer_order(void)
{
    muhtari *a = make(), *b = make();
    for (unsigned lone = 0u; lone < 2u; lone++) {
        memset(streams, 0, sizeof streams);
        for (unsigned k = 0u; k < 2u; k++) {
            muhtari *m = k == 0u ? a : b;
            muhtari__synth_reset(m, 0u, lone != 0u ? 1u : 2u);
            muhtari__tia_reset(m);
            m->synth.secondary = (uint8_t)lone;
            muhtari__pokey_reset(m, 0u);
            muhtari__pokey_reset(m, 1u);
            CHECK_EQ_U64(muhtari__synth_observe(m, collect, &streams[k]), 0);
            muhtari__observe_begin(m, 4000u);
            muhtari__tia_write(m, 0x19u, 15u, 0u);
            muhtari__tia_write(m, 0x1Au, 11u, 0u);
            for (unsigned chip = lone; chip < 2u; chip++) {
                muhtari__pokey_write(m, chip, 8u, 0x64u, 0u);
                for (unsigned ch = 0; ch < 4u; ch++) {
                    muhtari__pokey_write(m, chip, (uint8_t)(2u * ch), 1u, 0u);
                    muhtari__pokey_write(m, chip, (uint8_t)(2u * ch + 1u), 0xAFu, 0u);
                }
                muhtari__pokey_write(m, chip, 9u, 0u, 0u);
            }
            if (k == 0u)
                muhtari__pokey_catch_up(m, 3999u);
            else
                for (uint64_t t = 1u; t < 4000u; t++)
                    muhtari__pokey_catch_up(m, t);
            muhtari__observe_end(m);
        }
        CHECK(streams[0].count > 100u);
        CHECK_EQ_U64(streams[0].count, streams[1].count);
        for (unsigned n = 0u; n < streams[0].count; n++) {
            struct event x = streams[0].item[n], y = streams[1].item[n];
            CHECK_EQ_U64(x.tick, y.tick);
            CHECK_EQ_U64(x.source, y.source);
            CHECK_EQ_U64(x.level, y.level);
            CHECK(x.level <= 15u);
            if (lone != 0u)
                CHECK(x.source < 2u || x.source >= 6u);
            if (n != 0u) {
                struct event prev = streams[0].item[n - 1u];
                CHECK(x.tick >= prev.tick);
                if (x.tick == prev.tick)
                    CHECK(x.source >= prev.source);
            }
        }
    }
    /* Deliberately non-source-ordered equal-tick submissions preserve
       both changes on a source even when the final level returns to zero. */
    muhtari__synth_reset(a, 0u, 0u);
    streams[0].count = 0u;
    muhtari__observe_begin(a, 10u);
    muhtari__synth_raw(a, 9u, 7u, 5u);
    muhtari__synth_raw(a, 9u, 1u, 3u);
    muhtari__synth_raw(a, 9u, 1u, 0u);
    muhtari__synth_raw(a, 10u, 0u, 4u);
    muhtari__synth_raw(a, 11u, 6u, 15u);
    muhtari__observe_end(a);
    CHECK_EQ_U64(streams[0].count, 3u);
    CHECK_EQ_U64(streams[0].item[0].source, 1u);
    CHECK_EQ_U64(streams[0].item[0].level, 3u);
    CHECK_EQ_U64(streams[0].item[1].level, 0u);
    CHECK_EQ_U64(streams[0].item[2].source, 7u);
    muhtari__observe_begin(a, 20u);
    CHECK_EQ_U64(streams[0].count, 5u);
    CHECK_EQ_U64(streams[0].item[3].tick, 10u);
    CHECK_EQ_U64(streams[0].item[4].tick, 11u);
    CHECK_EQ_U64(muhtari__synth_observe(a, NULL, NULL), 0);
    CHECK(a->observation == NULL);
    muhtari_destroy(a);
    muhtari_destroy(b);
}

static void reference_raw(void *user, uint64_t tick, uint8_t source, uint16_t raw)
{
    uint8_t bytes[11];
    for (unsigned i = 0; i < 8u; i++)
        bytes[i] = (uint8_t)(tick >> (8u * i));
    bytes[8] = source;
    bytes[9] = (uint8_t)raw;
    bytes[10] = (uint8_t)(raw >> 8);
    muhtari__sha256_update(user, bytes, sizeof bytes);
}
static void print_digest(const char *key, muhtari__sha256 *hash)
{
    uint8_t digest[32];
    char hex[65];
    muhtari__sha256_final(hash, digest);
    muhtari__sha256_hex(digest, hex);
    printf("%s %s\n", key, hex);
}
static int reference_hashes(const char *path, const char *count, int fire)
{
    char *end = NULL;
    unsigned long frames = strtoul(count, &end, 10);
    if (*count == '\0' || *end != '\0' || frames == 0u || frames > 100000u)
        return 1;
    FILE *f = fopen(path, "rb");
    if (f == NULL)
        return 1;
    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        return 1;
    }
    long length = ftell(f);
    if (length <= 0 || length > 1048704) {
        fclose(f);
        return 1;
    }
    rewind(f);
    uint8_t *image = malloc((size_t)length);
    if (image == NULL) {
        fclose(f);
        return 1;
    }
    size_t got = fread(image, 1, (size_t)length, f);
    int closed = fclose(f);
    if (got != (size_t)length || closed != 0) {
        free(image);
        return 1;
    }
    muhtari *m = make();
    muhtari__sha256 raw, pcm;
    muhtari__sha256_init(&raw);
    muhtari__sha256_init(&pcm);
    if (muhtari__synth_observe(m, reference_raw, &raw) != 0) {
        free(image);
        muhtari_destroy(m);
        return 1;
    }
    muhtari_media media = media_for(image, got);
    muhtari_status st = muhtari_load(m, &media);
    free(image);
    if (st != 0) {
        muhtari_destroy(m);
        return 1;
    }
    uint64_t samples = 0u;
    for (unsigned long n = 0; n < frames; n++) {
        muhtari_input input = {0};
        input.size = sizeof input;
        if (fire)
            input.buttons[0] = MUHTARI_BUTTON_1 | MUHTARI_BUTTON_2;
        CHECK_EQ_U64(muhtari_set_input(m, &input), 0);
        int16_t audio[801];
        muhtari_frame io;
        frame_io(&io, NULL, 0, audio, 801);
        if (muhtari_run_frame(m, &io) != 0) {
            muhtari_destroy(m);
            return 1;
        }
        for (uint32_t j = 0; j < io.audio_count; j++) {
            uint16_t bits = (uint16_t)audio[j];
            uint8_t bytes[2] = {(uint8_t)bits, (uint8_t)(bits >> 8)};
            muhtari__sha256_update(&pcm, bytes, 2);
        }
        samples += io.audio_count;
    }
    muhtari_versions v = {0};
    v.size = sizeof v;
    CHECK_EQ_U64(muhtari_get_versions(&v), 0);
    print_digest("audio_transitions_sha256", &raw);
    print_digest("audio_pcm_sha256", &pcm);
    printf("audio_frames %lu\naudio_samples %llu\naudio_rate 48000/1\naudio_behaviour_revision "
           "%u\naudio_synthesis exact-q23-16x64-dc20\n",
           frames, (unsigned long long)samples, v.behaviour_revision);
    muhtari_destroy(m);
    return 0;
}

int main(int argc, char **argv)
{
    if ((argc == 4 || argc == 5) && strcmp(argv[1], "--reference-hashes") == 0)
        return reference_hashes(argv[2], argv[3], argc == 5);
    if (argc == 2 && strcmp(argv[1], "observer") == 0) {
        test_observer_order();
        test_observer_one();
        CHECK_DONE();
    }
    if (argc != 1)
        return 1;
    test_observer_one();
    test_audio_boundaries();
    test_public_audio();
    CHECK_DONE();
}
