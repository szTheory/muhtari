/* muhtari-test-prop NAME BUILT_DIR: runs one property (tests/prop.h) over
   its cases. Each case draws its cartridge from a pool: the run fuzz
   target's explicit linear/retail seed images and 48 KiB of random bytes
   from the case's stream, each made a cartridge as that target makes it
   (tests/fuzz/run_cart.h), and the built simple and eightysix samples under
   BUILT_DIR. Every load is a fast boot, and a case runs at most 8 frames.
   Two runs are equal when every frame's status, audio count and picture
   match and so does the state digest: SHA-256 over struct muhtari with its
   pointers cleared, then the cartridge's ROM and RAM. Exits 0 when the
   property holds, 1 on a failure and 2 on a usage error. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../fuzz/run_cart.h"
#include "../fuzz/seeds.h"
#include "../prop.h"
#include "internal.h"
#include "muhtari.h"
#include "sha256.h"

#define WIDTH 320u
#define HEIGHT 242u
#define WIDE 336u  /* a second pitch, wider than the picture */
#define SAMPLES 2u /* simple and eightysix */

/* One frame as the caller sees it. */
struct frame_out {
    muhtari_status status;
    uint32_t audio_count;
    uint8_t picture[32];
    uint8_t pcm[32];
};

static uint8_t *samples[SAMPLES];
static size_t sample_sizes[SAMPLES];
static uint8_t *image;
static uint8_t *raw;
static uint8_t video[WIDE * HEIGHT];
static int16_t audio[MUHTARI__MAX_AUDIO_SAMPLES];
static struct muhtari scratch;
static uint32_t allocation_calls, fail_allocation, case_index, retail_seen;
static uint32_t retail_pool[3];

static void *prop_alloc(void *user, size_t size)
{
    (void)user;
    allocation_calls++;
    return fail_allocation != 0u ? NULL : malloc(size);
}

static void prop_free(void *user, void *ptr, size_t size)
{
    (void)user;
    (void)size;
    free(ptr);
}

static muhtari *create(void)
{
    muhtari_config cfg;
    muhtari *m = NULL;
    memset(&cfg, 0, sizeof cfg);
    cfg.size = (uint32_t)sizeof cfg;
    cfg.allocator.alloc = prop_alloc;
    cfg.allocator.free = prop_free;
    if (muhtari_create(&cfg, &m) != MUHTARI_STATUS_OK) {
        fputs("muhtari-test-prop: create failed\n", stderr);
        exit(1);
    }
    return m;
}

static uint32_t pool_count(void)
{
    return seeds_count() + 1u + SAMPLES;
}

/* Builds pool entry p into image and returns its size; the random cart
   takes its bytes from rng. */
static size_t pool_build(uint32_t p, uint64_t *rng)
{
    if (p < seeds_count()) {
        return run_cart(raw, seeds_build(p, raw), image).bytes;
    }
    if (p == seeds_count()) {
        for (uint32_t at = 0; at < RUN_LINEAR_BYTES; at += 8u) {
            uint64_t v = prop_next(rng);
            for (uint32_t b = 0; b < 8u; b++) {
                raw[at + b] = (uint8_t)(v >> (8u * b));
            }
        }
        return run_cart(raw, RUN_LINEAR_BYTES, image).bytes;
    }
    uint32_t s = p - seeds_count() - 1u;
    memcpy(image, samples[s], sample_sizes[s]);
    return sample_sizes[s];
}

/* Loads the n bytes in image with the fast boot; the status is part of the
   comparison, since a refused image leaves the test card running. */
static muhtari_status load(muhtari *m, size_t n)
{
    muhtari_media media;
    memset(&media, 0, sizeof media);
    media.size = (uint32_t)sizeof media;
    media.image = image;
    media.image_size = n;
    return muhtari_load(m, &media);
}

/* Runs one frame at the given pitch and records it. */
static void run(muhtari *m, uint32_t pitch, struct frame_out *out)
{
    muhtari_frame io;
    memset(&io, 0, sizeof io);
    io.size = (uint32_t)sizeof io;
    io.video_pitch = pitch;
    io.video = video;
    io.audio = audio;
    io.audio_capacity = MUHTARI__MAX_AUDIO_SAMPLES;
    memset(out, 0, sizeof *out);
    uint32_t allocations = allocation_calls;
    out->status = muhtari_run_frame(m, &io);
    if (allocation_calls != allocations) {
        fputs("property frame allocated after load\n", stderr);
        exit(1);
    }
    out->audio_count = io.audio_count;
    muhtari__sha256 s;
    muhtari__sha256_init(&s);
    for (uint32_t y = 0; y < HEIGHT; y++) {
        muhtari__sha256_update(&s, video + (size_t)y * pitch, WIDTH);
    }
    muhtari__sha256_final(&s, out->picture);
    muhtari__sha256_init(&s);
    for (uint32_t i = 0u; i < io.audio_count; i++) {
        uint8_t bytes[2] = {(uint8_t)audio[i], (uint8_t)((uint16_t)audio[i] >> 8)};
        muhtari__sha256_update(&s, bytes, sizeof bytes);
    }
    muhtari__sha256_final(&s, out->pcm);
}

/* The state digest. Without pending, the input waiting for the next VBLANK
   start is left out too: it is not machine state. */
static void digest(const muhtari *m, int pending, uint8_t out[32])
{
    memcpy(&scratch, m, sizeof scratch);
    memset(&scratch.allocator, 0, sizeof scratch.allocator);
    scratch.cart.rom = NULL;
    scratch.cart.ram = NULL;
    scratch.maria.rows = NULL;
    if (!pending) {
        memset(scratch.ports.pending_device, 0, sizeof scratch.ports.pending_device);
        memset(scratch.ports.pending_buttons, 0, sizeof scratch.ports.pending_buttons);
        scratch.ports.pending_switches = 0u;
        scratch.ports.pending = 0u;
    }
    muhtari__sha256 s;
    muhtari__sha256_init(&s);
    muhtari__sha256_update(&s, &scratch, sizeof scratch);
    if (m->cart.rom != NULL) {
        muhtari__sha256_update(&s, m->cart.rom,
                               (size_t)m->cart.info.rom_size + m->cart.info.ram_size);
    }
    muhtari__sha256_final(&s, out);
}

/* Reports a mismatch; returns 0 so a property can return it. */
static int differ(const char *what, uint32_t frame)
{
    fprintf(stderr, "  %s differs at frame %u\n", what, (unsigned)frame);
    return 0;
}

/* Two runs are equal: every frame record, then the state digests. */
static int same_run(const struct frame_out *one, const struct frame_out *two, uint32_t frames,
                    const muhtari *a, const muhtari *b)
{
    uint8_t d1[32];
    uint8_t d2[32];
    for (uint32_t i = 0; i < frames; i++) {
        if (memcmp(&one[i], &two[i], sizeof one[i]) != 0) {
            return differ("frame", i + 1u);
        }
    }
    digest(a, 1, d1);
    digest(b, 1, d2);
    return memcmp(d1, d2, sizeof d1) == 0 ? 1 : differ("state", frames);
}

/* Builds pool entry p and loads it into two new instances, *a and *b, whose
   load statuses must match. */
static uint32_t select_pool(uint64_t *rng, uint32_t size)
{
    /* Shrinking replays the same case and RNG stream at a smaller size. */
    uint32_t index = size == PROP_SIZE_MAX ? case_index++ : case_index - 1u;
    return index < 3u ? retail_pool[index] : prop_below(rng, pool_count());
}

static int load_pair(uint32_t p, uint64_t *rng, muhtari **a, muhtari **b)
{
    size_t n = pool_build(p, rng);
    *a = create();
    *b = create();
    muhtari_status first = load(*a, n), second = load(*b, n);
    if (first != MUHTARI_STATUS_OK || first != second)
        return differ("load status", 0u);
    uint32_t board = (*a)->cart.info.board;
    if (p < seeds_count()) {
        uint32_t expected = run_cart_board(raw, seeds_build(p, raw));
        if (board != expected || (*b)->cart.info.board != expected)
            return differ("selected board", 0u);
    }
    if (board == MUHTARI_BOARD_ACTIVISION || board == MUHTARI_BOARD_ABSOLUTE ||
        board == MUHTARI_BOARD_FRACTALUS) {
        retail_seen |= board == MUHTARI_BOARD_ACTIVISION ? 1u
                       : board == MUHTARI_BOARD_ABSOLUTE ? 2u
                                                         : 4u;
        uint32_t bank = 1u + prop_below(rng, board == MUHTARI_BOARD_ACTIVISION ? 7u : 1u);
        for (uint32_t i = 0u; i < 2u; i++) {
            muhtari *m = i == 0u ? *a : *b;
            if (board == MUHTARI_BOARD_ACTIVISION)
                muhtari__cart_write(m, (uint16_t)(0xFF80u + bank), 0u);
            if (board == MUHTARI_BOARD_ABSOLUTE)
                muhtari__cart_write(m, 0x8000u, 2u);
            if (board == MUHTARI_BOARD_FRACTALUS) {
                for (uint32_t byte = 0u; byte < 2048u; byte++) {
                    m->cart.ram[byte] = (uint8_t)(byte * 31u + 17u);
                }
            }
        }
        /* A real frame establishes chip/CPU history before the failed load;
           later property comparisons prove resumed frame and audio equality. */
        struct frame_out warm[2];
        uint32_t allocations = allocation_calls;
        run(*a, WIDTH, &warm[0]);
        run(*b, WIDTH, &warm[1]);
        if (memcmp(&warm[0], &warm[1], sizeof warm[0]) != 0 || allocation_calls != allocations) {
            return differ("warm execution/allocation", 0u);
        }
        uint8_t before[32], after[32];
        digest(*a, 1, before);
        struct run_cart_info bad = run_cart_control(raw, board);
        muhtari_media media = {0};
        media.size = (uint32_t)sizeof media;
        media.image = raw;
        media.image_size = bad.bytes - 1u;
        if (muhtari_load(*a, &media) != MUHTARI_STATUS_ERR_TRUNCATED_IMAGE ||
            allocation_calls != allocations)
            return differ("geometry refusal", 0u);
        digest(*a, 1, after);
        if (memcmp(before, after, sizeof before) != 0)
            return differ("deep refused state", 0u);
        fail_allocation = 1u;
        muhtari_status failed = load(*a, n);
        fail_allocation = 0u;
        if (failed != MUHTARI_STATUS_ERR_NO_MEMORY || allocation_calls != allocations + 1u) {
            return differ("allocation failure", 0u);
        }
        digest(*a, 1, after);
        if (memcmp(before, after, sizeof before) != 0)
            return differ("deep allocation-failure state", 0u);
    }
    return 1;
}

static int finish(muhtari *a, muhtari *b, int ok)
{
    muhtari_destroy(a);
    muhtari_destroy(b);
    return ok;
}

/* An input muhtari_set_input accepts. */
static void random_input(uint64_t *rng, muhtari_input *in)
{
    static const uint32_t devices[4] = {0u, MUHTARI_PORT_PROLINE, MUHTARI_PORT_JOYSTICK,
                                        MUHTARI_PORT_UNPLUGGED};
    memset(in, 0, sizeof *in);
    in->size = (uint32_t)sizeof *in;
    for (uint32_t p = 0; p < 2u; p++) {
        in->device[p] = devices[prop_below(rng, 4u)];
        in->buttons[p] = prop_below(rng, 64u);
    }
    in->switches = prop_below(rng, 32u);
}

/* Frames 1..k, then a pause in which the host queries the instance, then
   frames k+1..N, equal one sequence of N frames. The first part draws at a
   wider pitch, so neither the split nor the caller's buffer reaches the
   machine. */
static int split_run(uint64_t *rng, uint32_t size)
{
    struct frame_out one[PROP_SIZE_MAX];
    struct frame_out two[PROP_SIZE_MAX];
    muhtari *a;
    muhtari *b;
    int ok = load_pair(select_pool(rng, size), rng, &a, &b);
    size = size > 1u ? size - 1u : 1u;
    uint32_t frames = 1u + prop_below(rng, size);
    uint32_t k = prop_below(rng, frames + 1u);
    for (uint32_t i = 0; i < frames; i++) {
        run(a, WIDTH, &one[i]);
    }
    for (uint32_t i = 0; i < k; i++) {
        run(b, WIDE, &two[i]);
    }
    muhtari_info info;
    muhtari_counters counters;
    muhtari_span span;
    memset(&info, 0, sizeof info);
    memset(&counters, 0, sizeof counters);
    info.size = (uint32_t)sizeof info;
    counters.size = (uint32_t)sizeof counters;
    (void)muhtari_get_info(b, &info);
    (void)muhtari_get_counters(b, &counters);
    for (uint32_t kind = MUHTARI_MEMORY_SYSTEM_RAM; kind <= MUHTARI_MEMORY_CART_RAM; kind++) {
        memset(&span, 0, sizeof span);
        span.size = (uint32_t)sizeof span;
        (void)muhtari_get_memory(b, kind, &span);
    }
    for (uint32_t i = k; i < frames; i++) {
        run(b, WIDTH, &two[i]);
    }
    return finish(a, b, ok && same_run(one, two, frames, a, b));
}

/* Every address through muhtari__peek and muhtari__bus_peek, summed. */
static uint32_t peek_all(const muhtari *m)
{
    uint32_t sum = 0;
    for (uint32_t addr = 0; addr < 0x10000u; addr++) {
        sum = sum * 31u + muhtari__peek(m, (uint16_t)addr);
        sum = sum * 31u + muhtari__bus_peek(m, (uint16_t)addr);
    }
    return sum;
}

/* Peeking every address after k frames changes neither the state nor any
   later frame, and a second pass reads the same bytes. */
static int peek_pure(uint64_t *rng, uint32_t size)
{
    struct frame_out one[PROP_SIZE_MAX];
    struct frame_out two[PROP_SIZE_MAX];
    uint8_t before[32];
    uint8_t after[32];
    muhtari *a;
    muhtari *b;
    int ok = load_pair(select_pool(rng, size), rng, &a, &b);
    size = size > 1u ? size - 1u : 1u;
    uint32_t frames = 1u + prop_below(rng, size);
    uint32_t k = prop_below(rng, frames + 1u);
    for (uint32_t i = 0; i < frames; i++) {
        run(b, WIDTH, &two[i]);
    }
    for (uint32_t i = 0; i < k; i++) {
        run(a, WIDTH, &one[i]);
    }
    digest(a, 1, before);
    if (peek_all(a) != peek_all(a)) {
        ok = differ("a second peek", k);
    }
    digest(a, 1, after);
    if (memcmp(before, after, sizeof before) != 0) {
        ok = differ("state after peeking", k);
    }
    for (uint32_t i = k; i < frames; i++) {
        run(a, WIDTH, &one[i]);
    }
    return finish(a, b, ok && same_run(one, two, frames, a, b));
}

/* Two instances on different cartridges, loaded, given input and run
   alternately call by call, equal each run alone. */
static int instances_isolated(uint64_t *rng, uint32_t size)
{
    struct frame_out out[4][PROP_SIZE_MAX];
    muhtari_input in[2][PROP_SIZE_MAX];
    muhtari *m[4];
    uint32_t p = select_pool(rng, size);
    uint32_t q = (p + 1u + prop_below(rng, pool_count() - 1u)) % pool_count();
    /* m[0] and m[1] interleave; m[2] and m[3] run alone on the same. */
    int ok = load_pair(p, rng, &m[0], &m[2]);
    ok = load_pair(q, rng, &m[1], &m[3]) && ok;
    size = size > 1u ? size - 1u : 1u;
    uint32_t frames = 1u + prop_below(rng, size);
    for (uint32_t i = 0; i < frames; i++) {
        for (uint32_t j = 0; j < 2u; j++) {
            random_input(rng, &in[j][i]);
            ok = muhtari_set_input(m[j], &in[j][i]) == MUHTARI_STATUS_OK && ok;
        }
        run(m[0], WIDTH, &out[0][i]);
        run(m[1], WIDTH, &out[1][i]);
    }
    for (uint32_t j = 2u; j < 4u; j++) {
        for (uint32_t i = 0; i < frames; i++) {
            ok = muhtari_set_input(m[j], &in[j - 2u][i]) == MUHTARI_STATUS_OK && ok;
            run(m[j], WIDTH, &out[j][i]);
        }
    }
    ok = ok && same_run(out[0], out[2], frames, m[0], m[2]) &&
         same_run(out[1], out[3], frames, m[1], m[3]);
    return finish(m[0], m[2], finish(m[1], m[3], ok));
}

/* The input registers through muhtari__bus_peek: SWCHA ($280), SWCHB
   ($282) and INPT0-INPT5 ($08-$0D) (docs/hardware/controllers.md). */
static void peek_inputs(const muhtari *m, uint8_t regs[8])
{
    regs[0] = muhtari__bus_peek(m, 0x0280u);
    regs[1] = muhtari__bus_peek(m, 0x0282u);
    for (uint16_t n = 0; n < 6u; n++) {
        regs[2u + n] = muhtari__bus_peek(m, (uint16_t)(0x08u + n));
    }
}

/* Input set between frames is pending, not state: one or more
   muhtari_set_input calls after frame k leave the digest without the
   pending copy, and the input registers, unchanged. The next frame applies
   the last call at its VBLANK start, and it and the frame after equal those
   of an instance given only the last call. No call can come during a run,
   since muhtari_run_frame returns first. k is at least 1: the first frame
   after a fast-boot load starts at the hand-off, which latches nothing. */
static int input_at_vblank(uint64_t *rng, uint32_t size)
{
    struct frame_out one[2];
    struct frame_out two[2];
    struct frame_out skip;
    uint8_t d0[32];
    uint8_t d1[32];
    uint8_t r0[8];
    uint8_t r1[8];
    muhtari_input in;
    muhtari *a;
    muhtari *b;
    int ok = load_pair(select_pool(rng, size), rng, &a, &b);
    size = size > 1u ? size - 1u : 1u;
    uint32_t k = 1u + prop_below(rng, size > 2u ? size - 2u : 1u);
    uint32_t calls = 1u + prop_below(rng, 3u);
    for (uint32_t i = 0; i < k; i++) {
        run(a, WIDTH, &skip);
        run(b, WIDTH, &skip);
    }
    digest(a, 0, d0);
    peek_inputs(a, r0);
    for (uint32_t c = 0; c < calls; c++) {
        random_input(rng, &in);
        ok = muhtari_set_input(a, &in) == MUHTARI_STATUS_OK && ok;
    }
    digest(a, 0, d1);
    peek_inputs(a, r1);
    if (memcmp(d0, d1, sizeof d0) != 0 || memcmp(r0, r1, sizeof r0) != 0) {
        ok = differ("pending input reached the machine", k);
    }
    ok = muhtari_set_input(b, &in) == MUHTARI_STATUS_OK && ok;
    run(a, WIDTH, &one[0]);
    run(b, WIDTH, &two[0]);
    const struct muhtari__ports *ports = &a->ports;
    if (ports->device[0] != in.device[0] || ports->device[1] != in.device[1] ||
        ports->buttons[0] != in.buttons[0] || ports->buttons[1] != in.buttons[1] ||
        ports->switches != in.switches) {
        ok = differ("applied input", k + 1u);
    }
    run(a, WIDTH, &one[1]);
    run(b, WIDTH, &two[1]);
    return finish(a, b, ok && same_run(one, two, 2u, a, b));
}

/* These properties use an owned idle cartridge. The six combinations cover
   absent, HSC, EEPROM on either port, and both combined placements. */
static uint8_t battery_expected[34816];
static uint8_t battery_firmware[4096];

static muhtari_battery_info battery_info(const muhtari *m)
{
    muhtari_battery_info b = {0};
    b.size = (uint32_t)sizeof b;
    if (muhtari_get_battery(m, &b) != MUHTARI_STATUS_OK) {
        fputs("battery property query failed\n", stderr);
        exit(1);
    }
    return b;
}

static void battery_media(muhtari_media *media, unsigned layout, uint64_t *rng)
{
    memset(image, 0, 128u + 4096u);
    image[0] = 3u;
    memcpy(image + 1u, "ATARI7800", 9u);
    image[51] = 0x10u;
    image[55] = image[56] = MUHTARI_PORT_PROLINE;
    image[128] = 0x4Cu; /* JMP $F000 */
    image[130] = 0xF0u;
    image[128u + 4092u] = 0u;
    image[128u + 4093u] = 0xF0u;
    memset(media, 0, sizeof *media);
    media->size = (uint32_t)sizeof *media;
    media->image = image;
    media->image_size = 128u + 4096u;
    unsigned hsc = layout == 1u || layout >= 4u;
    unsigned port = layout >= 2u ? 1u + (layout & 1u) : 0u;
    media->hsc_mode = hsc != 0u ? MUHTARI_HSC_ON : MUHTARI_HSC_OFF;
    if (port != 0u) {
        if (prop_below(rng, 2u) != 0u) {
            image[54u + port] = MUHTARI_PORT_ATARIVOX;
        } else {
            media->port[port - 1u] = MUHTARI_PORT_SAVEKEY;
        }
    }
    /* A non-save override suppresses a declared save device before load. */
    if (layout < 2u) {
        image[58] = 2u;
        media->port[1] = MUHTARI_PORT_UNPLUGGED;
    }
    if (hsc != 0u && prop_below(rng, 2u) != 0u) {
        memset(battery_firmware, 0xFF, sizeof battery_firmware);
        battery_firmware[0x900] = 0xC6u;
        battery_firmware[0x904] = 0xFEu;
        media->hsc_rom = battery_firmware;
        media->hsc_rom_size = sizeof battery_firmware;
    }
}

/* Actual bus cycles carry every pin transition; getters never make edges.
   Open-drain wiring and page semantics: docs/hardware/controllers.md. */
static void battery_pins(muhtari *m, unsigned scl, unsigned sda)
{
    unsigned shift = m->battery.eeprom_port == 1u ? 4u : 0u;
    muhtari__bus_write(m, 0x0281u,
                       (uint8_t)(((scl == 0u ? 8u : 0u) | (sda == 0u ? 4u : 0u)) << shift));
}

static void battery_start(muhtari *m)
{
    battery_pins(m, 0u, 1u);
    battery_pins(m, 1u, 1u);
    battery_pins(m, 1u, 0u);
    battery_pins(m, 0u, 0u);
}

static void battery_send(muhtari *m, uint8_t value)
{
    for (unsigned bit = 0u; bit < 8u; bit++) {
        unsigned high = ((unsigned)value >> (7u - bit)) & 1u;
        battery_pins(m, 0u, high);
        battery_pins(m, 1u, high);
        battery_pins(m, 0u, high);
    }
    battery_pins(m, 0u, 1u);
    battery_pins(m, 1u, 1u);
    battery_pins(m, 0u, 1u);
}

static void battery_stop(muhtari *m)
{
    battery_pins(m, 0u, 0u);
    battery_pins(m, 1u, 0u);
    battery_pins(m, 1u, 1u);
}

static int battery_pair(uint64_t *rng, uint32_t size, muhtari **a, muhtari **b,
                        muhtari_media *media)
{
    uint32_t index = size == PROP_SIZE_MAX ? case_index++ : case_index - 1u;
    unsigned layout = index % 6u;
    battery_media(media, layout, rng);
    *a = create();
    *b = create();
    if (muhtari_load(*a, media) != MUHTARI_STATUS_OK ||
        muhtari_load(*b, media) != MUHTARI_STATUS_OK)
        return differ("battery load", 0u);
    muhtari_battery_info first = battery_info(*a), second = battery_info(*b);
    size_t bytes = (layout == 1u || layout >= 4u ? 2048u : 0u) + (layout >= 2u ? 32768u : 0u);
    if (first.bytes != bytes || second.bytes != bytes || first.layout_version != 1u ||
        second.layout_version != 1u || first.generation != 0u || second.generation != 0u ||
        (bytes == 0u ? first.data != NULL || second.data != NULL : first.data == second.data))
        return differ("battery layout", 0u);
    for (size_t at = 0u; at < bytes; at++) {
        battery_expected[at] = (uint8_t)(at * 17u + prop_below(rng, 256u));
        first.data[at] = second.data[at] = battery_expected[at];
    }
    return battery_info(*a).generation == 0u && battery_info(*b).generation == 0u;
}

static int battery_lifecycle(uint64_t *rng, uint32_t size)
{
    muhtari *a, *b;
    muhtari_media media;
    int ok = battery_pair(rng, size, &a, &b, &media);
    muhtari_battery_info original = battery_info(a);
    uint64_t expected_generation = UINT64_MAX;
    a->battery.generation = b->battery.generation = expected_generation;
    if (original.hsc_attached != 0u) {
        size_t at = prop_below(rng, 2048u);
        uint8_t value = (uint8_t)(battery_expected[at] ^ 0x80u);
        muhtari__bus_write(a, (uint16_t)(0x1000u + at), value);
        muhtari__bus_write(b, (uint16_t)(0x1000u + at), value);
        muhtari__bus_write(a, (uint16_t)(0x1000u + at), value);
        muhtari__bus_write(b, (uint16_t)(0x1000u + at), value);
        battery_expected[at] = value;
        expected_generation++;
    }
    if (original.eeprom_port != 0u) {
        uint16_t address = (uint16_t)prop_below(rng, 32768u);
        size_t offset = original.hsc_attached != 0u ? 2048u : 0u;
        unsigned count = 1u + prop_below(rng, 8u * size);
        unsigned abort_write = prop_below(rng, 3u) == 0u;
        uint8_t values[64];
        for (unsigned i = 0u; i < count; i++)
            values[i] = (uint8_t)prop_below(rng, 256u);
        for (unsigned peer = 0u; peer < 2u; peer++) {
            muhtari *m = peer == 0u ? a : b;
            muhtari__bus_write(m, 0x0280u, 0u);
            battery_start(m);
            battery_send(m, 0xA0u);
            battery_send(m, (uint8_t)(address >> 8));
            battery_send(m, (uint8_t)address);
            for (unsigned i = 0u; i < count; i++)
                battery_send(m, values[i]);
            if (abort_write != 0u)
                battery_start(m);
            battery_stop(m);
        }
        if (memcmp(a->battery.data, battery_expected, original.bytes) != 0)
            ok = differ("pending page became persistent", 0u);
        if (abort_write == 0u) {
            int changed = 0;
            for (unsigned i = 0u; i < count; i++) {
                size_t at = offset + (address & 0x7FC0u) + ((address + i) & 63u);
                changed |= battery_expected[at] != values[i];
                battery_expected[at] = values[i];
            }
            expected_generation += changed != 0 ? 1u : 0u;
        }
    }
    struct frame_out first[2], second[2];
    for (unsigned frame = 0u; frame < 2u; frame++) {
        run(a, WIDTH, &first[frame]);
        run(b, WIDTH, &second[frame]);
    }
    ok = same_run(first, second, 2u, a, b) && ok;
    muhtari_battery_info after = battery_info(a);
    if (after.data != original.data || after.bytes != original.bytes ||
        after.generation != expected_generation ||
        memcmp(a->battery.data, battery_expected, original.bytes) != 0)
        ok = differ("committed battery", 2u);
    /* Every refusal is transactional, including staging explicit firmware. */
    uint8_t before[32], refused[32];
    digest(a, 1, before);
    muhtari_media bad = media;
    bad.image_size = 127u;
    uint32_t calls = allocation_calls;
    if (muhtari_load(a, &bad) != MUHTARI_STATUS_ERR_TRUNCATED_IMAGE || allocation_calls != calls)
        ok = differ("malformed battery refusal", 2u);
    bad = media;
    bad.port[0] = bad.port[1] = MUHTARI_PORT_SAVEKEY;
    if (muhtari_load(a, &bad) != MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE || allocation_calls != calls)
        ok = differ("dual EEPROM refusal", 2u);
    fail_allocation = 1u;
    muhtari_status failed = muhtari_load(a, &media);
    fail_allocation = 0u;
    if (failed != MUHTARI_STATUS_ERR_NO_MEMORY || allocation_calls != calls + 1u)
        ok = differ("battery allocation refusal", 2u);
    digest(a, 1, refused);
    if (memcmp(before, refused, sizeof before) != 0 || battery_info(a).data != original.data)
        ok = differ("battery refusal state", 2u);
    run(a, WIDTH, first);
    run(b, WIDTH, second);
    ok = same_run(first, second, 1u, a, b) && ok;
    calls = allocation_calls;
    if (muhtari_power_cycle(a) != MUHTARI_STATUS_OK ||
        muhtari_power_cycle(b) != MUHTARI_STATUS_OK || allocation_calls != calls ||
        battery_info(a).generation != expected_generation ||
        battery_info(a).data != original.data ||
        memcmp(a->battery.data, battery_expected, original.bytes) != 0)
        ok = differ("battery reset retention", 3u);
    run(a, WIDTH, first);
    run(b, WIDTH, second);
    ok = same_run(first, second, 1u, a, b) && ok;
    if (muhtari_load(a, &media) != MUHTARI_STATUS_OK || battery_info(a).generation != 0u)
        ok = differ("battery reload", 0u);
    for (size_t at = 0u; at < original.bytes; at++) {
        if (a->battery.data[at] != 0xFFu) {
            ok = differ("battery reload baseline", 0u);
            break;
        }
    }
    if (memcmp(b->battery.data, battery_expected, original.bytes) != 0)
        ok = differ("battery instance isolation", 0u);
    return finish(a, b, ok);
}

static int battery_observers(uint64_t *rng, uint32_t size)
{
    muhtari *a, *b;
    muhtari_media media;
    int ok = battery_pair(rng, size, &a, &b, &media);
    if (a->battery.eeprom_port != 0u) {
        for (unsigned peer = 0u; peer < 2u; peer++) {
            muhtari *m = peer == 0u ? a : b;
            muhtari__bus_write(m, 0x0280u, 0u);
            battery_start(m);
            battery_send(m, 0xA0u);
            /* Leave an incomplete address in flight during observation. */
            battery_send(m, 0x7Fu);
        }
    }
    uint8_t before[32], after[32];
    digest(a, 1, before);
    uint32_t calls = allocation_calls;
    for (unsigned n = 0u; n < size; n++) {
        muhtari_battery_info info = battery_info(a);
        muhtari_const_span rom = {0};
        rom.size = (uint32_t)sizeof rom;
        if (muhtari_get_hsc_rom(a, &rom) != MUHTARI_STATUS_OK || info.generation != 0u)
            ok = differ("battery observer status", 0u);
        (void)peek_all(a);
    }
    digest(a, 1, after);
    if (memcmp(before, after, sizeof before) != 0 || allocation_calls != calls)
        ok = differ("battery observer changed state or allocated", 0u);
    struct frame_out first, second;
    run(a, WIDTH, &first);
    run(b, WIDTH, &second);
    return finish(a, b, same_run(&first, &second, 1u, a, b) && ok);
}

/* Reads BUILT_DIR/name/name.bas.a78 into samples[i]. */
static int read_sample(const char *dir, const char *name, uint32_t i)
{
    size_t len = strlen(dir) + 2u * strlen(name) + 16u;
    char *path = (char *)malloc(len);
    if (path == NULL) {
        return 0;
    }
    (void)snprintf(path, len, "%s/%s/%s.bas.a78", dir, name, name);
    FILE *f = fopen(path, "rb");
    samples[i] = (uint8_t *)malloc(MUHTARI_IMAGE_SIZE_MAX + 1u);
    size_t got = 0u;
    if (f != NULL && samples[i] != NULL) {
        got = fread(samples[i], 1u, MUHTARI_IMAGE_SIZE_MAX + 1u, f);
    }
    int ok =
        f != NULL && samples[i] != NULL && !ferror(f) && got > 0u && got <= MUHTARI_IMAGE_SIZE_MAX;
    if (f != NULL) {
        (void)fclose(f);
    }
    if (!ok) {
        fprintf(stderr, "muhtari-test-prop: cannot read %s\n", path);
    }
    sample_sizes[i] = got;
    free(path);
    return ok;
}

static const struct {
    const char *name;
    int (*fn)(uint64_t *rng, uint32_t size);
} props[] = {
    {"split_run", split_run},
    {"peek_pure", peek_pure},
    {"instances_isolated", instances_isolated},
    {"input_at_vblank", input_at_vblank},
    {"battery_lifecycle", battery_lifecycle},
    {"battery_observers", battery_observers},
};

int main(int argc, char **argv)
{
    if (argc != 3) {
        fputs("usage: muhtari-test-prop NAME BUILT_DIR\n", stderr);
        return 2;
    }
    image = (uint8_t *)malloc(SEEDS_SIZE_MAX);
    raw = (uint8_t *)malloc(SEEDS_SIZE_MAX);
    if (image == NULL || raw == NULL || !read_sample(argv[2], "simple", 0u) ||
        !read_sample(argv[2], "eightysix", 1u)) {
        return 1;
    }
    for (size_t i = 0; i < sizeof props / sizeof props[0]; i++) {
        if (strcmp(argv[1], props[i].name) == 0) {
            for (uint32_t b = 0u; b < 3u; b++)
                retail_pool[b] = UINT32_MAX;
            for (uint32_t p = 0u; p < seeds_count(); p++) {
                if (strcmp(seeds_name(p), "activision") == 0)
                    retail_pool[0] = p;
                if (strcmp(seeds_name(p), "absolute") == 0)
                    retail_pool[1] = p;
                if (strcmp(seeds_name(p), "fractalus") == 0)
                    retail_pool[2] = p;
            }
            for (uint32_t b = 0u; b < 3u; b++)
                if (retail_pool[b] == UINT32_MAX)
                    return 1;
            int result = prop_run(props[i].name, props[i].fn);
            if (strncmp(props[i].name, "battery_", 8u) == 0) {
                return result;
            }
            if (retail_seen != 7u) {
                fprintf(stderr, "property omitted a retail board: mask %u\n", retail_seen);
                return 1;
            }
            printf("retail board coverage: Activision, Absolute, Fractalus\n");
            return result;
        }
    }
    fprintf(stderr, "muhtari-test-prop: unknown property %s\n", argv[1]);
    return 2;
}
