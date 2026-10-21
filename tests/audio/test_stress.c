/* Hardware-legal dual-POKEY/TIA traffic through the shipped CPU and bus.
   Timing/clock choices and finite carry: hardware/audio.md and pokey.md. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "internal.h"
#include "../cart/images.h"
#include "../../runner/clock.h"
#include "../check.h"
static unsigned allocations;
static void *allocate(void *u, size_t n)
{
    (void)u;
    allocations++;
    return malloc(n);
}
static void release(void *u, void *p, size_t n)
{
    (void)u;
    (void)n;
    free(p);
}
static uint8_t cart[128u + 4096u];
static size_t cursor;
static void byte(uint8_t v)
{
    CHECK(cursor < 4000u);
    cart[128u + cursor++] = v;
}
static void write_reg(uint16_t at, uint8_t v)
{
    byte(0xA9);
    byte(v);
    byte(0x8D);
    byte((uint8_t)at);
    byte((uint8_t)(at >> 8));
}
static void program(unsigned seed, int dual)
{
    images_header(cart, 4u, "Hostile audio", 4096u, 0x40u);
    cart[67] = dual ? 3u : 0u;
    cart[69] = dual ? 1u : 0u;
    if (!dual) {
        cart[53] = 0;
        cart[54] = 0;
    }
    memset(cart + 128u, 0xEA, 4096u);
    cursor = 0u;
    byte(0x78);
    write_reg(1u, 7u);
    static const uint8_t control[] = {0x64u, 0x78u, 0x06u, 0x66u};
    for (unsigned chip = 0; chip < 2u; chip++) {
        uint16_t at = (uint16_t)(chip == 0u ? 0x450u : 0x440u);
        write_reg((uint16_t)(at + 8u), control[seed & 3u]);
        for (unsigned ch = 0; ch < 4u; ch++) {
            write_reg((uint16_t)(at + 2u * ch), (uint8_t)(seed & 1u));
            write_reg((uint16_t)(at + 2u * ch + 1u), 0xAFu);
        }
        write_reg((uint16_t)(at + 9u), 0u);
    }
    write_reg(0x15u, 4u);
    write_reg(0x16u, 8u);
    write_reg(0x17u, 0u);
    write_reg(0x18u, 1u);
    write_reg(0x45Eu, 1u);
    byte(0x58);
    uint16_t loop = (uint16_t)(0xF000u + cursor);
    byte(0xAD);
    byte(1u);
    byte(0x18); /* LDA owned RAM */
    byte(0x49);
    byte(15u); /* EOR volume nibble */
    byte(0x8D);
    byte(1u);
    byte(0x18);
    byte(0x85);
    byte(0x19);
    byte(0x85);
    byte(0x1A);
    byte(0x09);
    byte(0xA0); /* square, direct channels 1 and 3 */
    byte(0x8D);
    byte(0x51);
    byte(4u);
    byte(0x8D);
    byte(0x45);
    byte(4u);
    byte(0x85);
    byte(2u); /* slow TIA WSYNC, including real RDY hold */
    byte(0x4C);
    byte((uint8_t)loop);
    byte((uint8_t)(loop >> 8));
    cursor = 0x200u;
    byte(0xEE);
    byte(0);
    byte(0x18); /* handler count */
    write_reg(0x45Eu, 0u);
    write_reg(0x45Eu, 1u);
    byte(0x40);
    cart[128u + 4092u] = 0;
    cart[128u + 4093u] = 0xF0;
    cart[128u + 4094u] = 0;
    cart[128u + 4095u] = 0xF2;
}
static muhtari *make(void)
{
    muhtari_config c = {0};
    c.size = sizeof c;
    c.allocator.alloc = allocate;
    c.allocator.free = release;
    muhtari *m = NULL;
    CHECK_EQ_U64(muhtari_create(&c, &m), 0);
    muhtari_media d = {0};
    d.size = sizeof d;
    d.image = cart;
    d.image_size = sizeof cart;
    CHECK_EQ_U64(muhtari_load(m, &d), 0);
    m->ram[0] = 0;
    m->ram[1] = 0;
    return m;
}
struct trace {
    uint64_t count, hash, tick;
    uint8_t source;
};
static void observe(void *u, uint64_t tick, uint8_t source, uint16_t raw)
{
    struct trace *t = u;
    CHECK(raw <= 15u && source < 10u);
    if (t->count) {
        CHECK(tick >= t->tick);
        if (tick == t->tick)
            CHECK(source >= t->source);
    }
    t->tick = tick;
    t->source = source;
    t->count++;
    t->hash = t->hash * UINT64_C(1099511628211) ^ tick ^ ((uint64_t)source << 8) ^ raw;
}
static uint64_t exercise(unsigned frames, unsigned seed, int dual, int checks, uint64_t *raw_count,
                         uint64_t *pcm_hash)
{
    program(seed, dual);
    muhtari *m = make(), *off = checks ? make() : NULL, *split = checks ? make() : NULL;
    struct trace trace = {0};
    CHECK_EQ_U64(muhtari__synth_observe(m, observe, &trace), 0);
    unsigned initial_allocations = allocations;
    uint64_t start = muhtari__clock_ns(), samples = 0;
    for (unsigned frame = 0; frame < frames; frame++) {
        int16_t audio[3][801];
        uint32_t count = 0;
        for (unsigned k = 0; k < (checks ? 3u : 1u); k++) {
            muhtari *a = k == 0u ? m : k == 1u ? off : split;
            uint64_t end = a->frame_deadline;
            if (checks && frame == 3u) {
                muhtari__observe_begin(a, end);
                while (a->bus.ticks < end - 40u)
                    muhtari__cpu_step(a);
                a->bus.rdy_release = end + 400u;
                CHECK_EQ_U64(muhtari__bus_request_halt(a, end + 28u, end + 450u), 1);
            }
            if (checks && frame == 8u)
                a->cpu.jammed = 1u;
            if (k == 2u) {
                while (a->bus.ticks < end)
                    muhtari__cpu_step(a);
                muhtari__tia_catch_up(a, a->bus.ticks);
                uint64_t begin = a->synth.horizon;
                uint32_t n = 0;
                if (begin + 149u < end)
                    n = muhtari__synth_render(a, begin + 149u, audio[k]);
                n += muhtari__synth_render(a, end, audio[k] + n);
                CHECK_EQ_U64(n, count);
                a->frame_start = end;
                a->frame_deadline += 119402u;
            } else {
                muhtari_frame io = {0};
                io.size = sizeof io;
                if (k == 0u || (frame & 1u)) {
                    io.audio = audio[k];
                    io.audio_capacity = 801;
                }
                CHECK_EQ_U64(muhtari_run_frame(a, &io), 0);
                if (k == 0u)
                    count = io.audio_count;
                else
                    CHECK_EQ_U64(io.audio_count, count);
            }
            CHECK(a->bus.ticks - a->frame_start <= MUHTARI__SYNTH_OVERSHOOT_TICKS);
            CHECK(a->synth.pcm_count <= MUHTARI__SYNTH_CARRY_SAMPLES);
            CHECK(a->synth.carry_high_water <=
                  (k == 2u ? MUHTARI__SYNTH_PCM_CAPACITY : MUHTARI__SYNTH_CARRY_SAMPLES));
            CHECK(a->synth.kernel_adds_per_sample <= 150u);
            CHECK_EQ_U64(a->synth.clipped_samples, 0);
            CHECK(a->synth.pre_clip_peak <= 29204u);
        }
        if (checks) {
            CHECK(memcmp(audio[0], audio[2], count * sizeof(int16_t)) == 0);
            if (frame & 1u)
                CHECK(memcmp(audio[0], audio[1], count * sizeof(int16_t)) == 0);
            CHECK(memcmp(&m->synth, &off->synth, sizeof m->synth) == 0);
            CHECK(memcmp(&m->tia, &split->tia, sizeof m->tia) == 0);
            CHECK(memcmp(m->pokey, split->pokey, sizeof m->pokey) == 0);
        }
        for (uint32_t j = 0; j < count; j++)
            *pcm_hash = *pcm_hash * UINT64_C(1099511628211) ^ (uint16_t)audio[0][j];
        samples += count;
    }
    uint64_t elapsed = muhtari__clock_ns() - start;
    CHECK_EQ_U64(allocations, initial_allocations);
    CHECK(trace.count > 0u && *pcm_hash != 0u);
    CHECK_EQ_U64(samples, (UINT64_C(8190) + (uint64_t)(frames - 1u) * 119402u) * 88u / 13125u);
    if (checks && dual)
        CHECK(m->ram[0] != 0u);
    *raw_count = trace.count;
    printf("workload %s frames %u samples %llu raw_records %llu elapsed_ns %llu kernel_max %u "
           "carry_max %u peak %llu\n",
           dual ? "dual-pokey-tia" : "tia-baseline", frames, (unsigned long long)samples,
           (unsigned long long)trace.count, (unsigned long long)elapsed,
           m->synth.kernel_adds_per_sample, m->synth.carry_high_water,
           (unsigned long long)m->synth.pre_clip_peak);
    muhtari_destroy(m);
    muhtari_destroy(off);
    muhtari_destroy(split);
    return elapsed;
}
int main(int argc, char **argv)
{
    uint64_t raw_count = 0, hash = 0;
    if (argc == 3 && strcmp(argv[1], "--bench") == 0 && strcmp(argv[2], "600") == 0) {
        uint64_t exact = exercise(600, 0, 1, 0, &raw_count, &hash);
        uint64_t exact_raw = raw_count, exact_hash = hash;
        hash = 0;
        uint64_t baseline = exercise(600, 0, 0, 0, &raw_count, &hash);
        CHECK(baseline > 0u);
#ifdef __aarch64__
        const char *target = "aarch64";
#elif defined(__x86_64__) || defined(_M_X64)
        const char *target = "x86_64";
#else
        const char *target = "other";
#endif
        printf("target %s compiler %s exact_ratio_milli %llu exact_raw %llu exact_pcm_check %llu "
               "synthesis exact-q23-16x64-dc20\n",
               target,
#ifdef __clang__
               "clang",
#elif defined(_MSC_VER)
               "msvc",
#else
               "c17",
#endif
               (unsigned long long)(baseline ? exact * 1000u / baseline : 0u),
               (unsigned long long)exact_raw, (unsigned long long)exact_hash);
    } else if (argc == 1) {
        for (unsigned seed = 0; seed < 4u; seed++) {
            hash = 0;
            (void)exercise(12, seed, 1, 1, &raw_count, &hash);
        }
    } else
        return 2;
    CHECK_DONE();
}
