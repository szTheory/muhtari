/* Spectral measurement contract, real TIA/POKEY and all-DAC stress:
   docs/hardware/audio.md and A7800-AUDIO sections 1-4, 8. Test-only math. */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"
#include "audio_tables.h"

#define N 65536u
#define SETTLE 12000u
#define PI 3.14159265358979323846
#define RATE 48000.0

struct tone {
    unsigned tia, divisor, chips, channels, offset;
};

struct spectrum {
    double db, frequency, reference, bin_db;
};

static double real_part[N], imaginary[N], window[N], window_sum;
static int16_t pcm[N];
static int failures;
static unsigned pal;
static uint32_t sample_num = 88u, sample_den = 13125u;

static void source_level(muhtari *m, const struct tone *t, uint64_t tick, unsigned high);

#define REQUIRE(condition)                                                                         \
    do {                                                                                           \
        if (!(condition)) {                                                                        \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition);                        \
            failures++;                                                                            \
        }                                                                                          \
    } while (0)

/* Owned radix-2 transform. No external FFT code or package. */
static void transform(void)
{
    for (unsigned i = 1u, j = 0u; i < N; i++) {
        unsigned bit = N / 2u;
        for (; (j & bit) != 0u; bit >>= 1u) {
            j ^= bit;
        }
        j ^= bit;
        if (i < j) {
            double tmp = real_part[i];
            real_part[i] = real_part[j];
            real_part[j] = tmp;
        }
    }
    for (unsigned length = 2u; length <= N; length *= 2u) {
        double angle = -2.0 * PI / length;
        double wr = cos(angle), wi = sin(angle);
        for (unsigned base = 0; base < N; base += length) {
            double r = 1.0, im = 0.0;
            for (unsigned j = 0; j < length / 2u; j++) {
                unsigned a = base + j, b = a + length / 2u;
                double br = r * real_part[b] - im * imaginary[b];
                double bi = r * imaginary[b] + im * real_part[b];
                real_part[b] = real_part[a] - br;
                imaginary[b] = imaginary[a] - bi;
                real_part[a] += br;
                imaginary[a] += bi;
                double next = r * wr - im * wi;
                im = r * wi + im * wr;
                r = next;
            }
        }
    }
}

static double fundamental(const struct tone *t)
{
    const double tick_rate = pal != 0u ? 7093788.0 : 315000000.0 / 44.0;
    return t->tia != 0u ? tick_rate / (456u * (t->divisor + 1u))
                        : tick_rate / (8u * (t->divisor + 4u));
}

/* Only real in-band harmonic lines, never folded frequencies. Odd TIA
   divisors alternate 224/232 B-edge spacings; even ones are 50% square.
   BH4 has a four-bin main lobe. Five bins include that lobe and its edge;
   the off-bin and multi-line checks below bound the remaining leakage. */
static int intended(unsigned bin, const struct tone *t)
{
    double f = fundamental(t);
    for (unsigned harmonic = 1u; harmonic * f < 16000.0; harmonic++) {
        if (((t->tia == 0u || (t->divisor + 1u) % 2u == 0u) && harmonic % 2u == 0u)) {
            continue;
        }
        if (fabs((double)bin - harmonic * f * N / RATE) <= 5.0) {
            return 1;
        }
    }
    return 0;
}

/* Actual register-programmed POKEY. Only1/3 have a direct fast clock.
   Four/eight-channel maximum stress keeps2/4 at volume-only15 and aligns
   the two fast squares by delaying channel3's first period one lap.
   The resulting legal signal retains the known physical fast frequency;
   independent all-DAC stress above remains a separate stronger schedule. */
static void capture_pokey(const struct tone *t, unsigned slow, unsigned word, unsigned length,
                          unsigned hp)
{
    muhtari *m = calloc(1u, sizeof *m);
    muhtari *reference = calloc(1u, sizeof *reference);
    if (m == NULL || reference == NULL) {
        exit(1);
    }
    m->effective_region = reference->effective_region =
        pal != 0u ? MUHTARI_REGION_PAL : MUHTARI_REGION_NTSC;
    muhtari__synth_reset(m, 0u, (uint8_t)t->chips);
    muhtari__synth_reset(reference, 0u, (uint8_t)t->chips);
    unsigned reference_high = 0u;
    if (length == 0u) {
        length = 1u;
        word = slow;
    }
    m->bus.clock_origin_tick = t->offset;
    m->bus.ticks = t->offset;
    m->bus.inptctrl = 7u;
    for (unsigned chip = 0; chip < 2u; chip++) {
        muhtari__pokey_reset(m, chip);
    }
    for (unsigned chip = 0; chip < t->chips; chip++) {
        muhtari__pokey_write(m, chip, 8u, hp != 0u ? 0x64u : 0x60u, t->offset);
        muhtari__pokey_write(m, chip, 0u, (uint8_t)t->divisor, t->offset);
        muhtari__pokey_write(m, chip, 1u, 0xAFu, t->offset);
        if (t->channels == 4u) {
            muhtari__pokey_write(m, chip, 4u, (uint8_t)(2u * t->divisor + 4u), t->offset);
            muhtari__pokey_write(m, chip, 5u, 0xAFu, t->offset);
            muhtari__pokey_write(m, chip, 3u, 0x1Fu, t->offset);
            muhtari__pokey_write(m, chip, 7u, 0x1Fu, t->offset);
        }
        muhtari__pokey_write(m, chip, 9u, 0u, t->offset);
    }
    if (hp != 0u) {
        for (unsigned chip = 0; chip < t->chips; chip++) {
            muhtari__pokey_write(m, chip, 4u, 0u, t->offset);
            muhtari__pokey_write(m, chip, 9u, 0u, t->offset);
        }
    }
    if (hp >= 2u) {
        for (unsigned chip = 0; chip < t->chips; chip++) {
            muhtari__pokey_write(m, chip, 8u, hp == 2u ? 0x72u : 0x64u, t->offset);
            muhtari__pokey_write(m, chip, 0u, 0u, t->offset);
            muhtari__pokey_write(m, chip, 4u, 0u, t->offset);
            muhtari__pokey_write(m, chip, 1u, hp == 2u ? 0u : 0xAFu, t->offset);
            muhtari__pokey_write(m, chip, 3u, hp == 2u ? 0xAFu : 0u, t->offset);
            if (hp == 3u)
                muhtari__pokey_write(m, chip, 15u, 11u, t->offset);
            muhtari__pokey_write(m, chip, 9u, 0u, t->offset);
        }
    }
    unsigned observed = hp == 2u ? 3u : 2u;
    uint64_t start = t->offset, next = start + 2u, horizon = 0u;
    unsigned cycles = 0u, samples = 0u, falling = 0u, transitions = 0u;
    while (samples < SETTLE + N) {
        uint64_t end = horizon + 13125u;
        while (next < end) {
            unsigned before = m->synth.raw[observed];
            muhtari__pokey_half_edge(m, next, (int)falling);
            transitions += before != m->synth.raw[observed];
            if (falling != 0u) {
                cycles++;
                /* Independent falling-cycle arithmetic: initial reload then
                   N+8 first output, N+4 subsequent periods (pokey.md). */
                if (t->channels == 1u && hp == 0u && cycles >= t->divisor + 8u &&
                    (cycles - t->divisor - 8u) % (t->divisor + 4u) == 0u) {
                    reference_high ^= 1u;
                    source_level(reference, t, next, reference_high);
                }
                if (hp == 1u && cycles == 4u) {
                    for (unsigned chip = 0; chip < t->chips; chip++) {
                        muhtari__pokey_write(m, chip, 4u, 2u, next);
                    }
                }
                if (cycles == 4u && t->channels == 4u) {
                    for (unsigned chip = 0; chip < t->chips; chip++) {
                        muhtari__pokey_write(m, chip, 4u, (uint8_t)t->divisor, next);
                    }
                }
                start += ((word >> ((cycles - 1u) % length)) & 1u) != 0u ? 6u : 4u;
                next = start + 2u;
            } else {
                next = start + (((word >> (cycles % length)) & 1u) != 0u ? 5u : 4u);
            }
            falling ^= 1u;
        }
        /* Do not emit the next rise speculatively across an output cut.
           TIA is silent here; bus integration tests exercise shared ties. */
        int16_t chunk[89];
        uint32_t count =
            (uint32_t)(end * sample_num / sample_den - horizon * sample_num / sample_den);
        REQUIRE(count >= 88u && count <= 89u);
        REQUIRE(pal != 0u || count == 88u);
        REQUIRE(muhtari__synth_render(m, end, chunk) == count);
        if (t->channels == 1u && hp == 0u) {
            /* Independent documented edge arithmetic, not chip-derived
               timestamps. A miss shared here localizes reconstruction. */
            int16_t expected[89];
            REQUIRE(muhtari__synth_render(reference, end, expected) == count);
            REQUIRE(memcmp(chunk, expected, count * sizeof chunk[0]) == 0);
        }
        for (unsigned i = 0; i < count && samples < SETTLE + N; i++, samples++) {
            if (samples >= SETTLE) {
                pcm[samples - SETTLE] = chunk[i];
            }
        }
        horizon = end;
    }
    REQUIRE(m->synth.clipped_samples == 0u);
    REQUIRE(m->synth.kernel_adds_per_sample <= 150u);
    REQUIRE(m->pokey[0].channel[0].fires > 1000u);
    REQUIRE(transitions > 1000u && m->synth.kernel_adds_per_sample > 0u);
    if (t->channels == 4u) {
        REQUIRE(m->synth.raw[2] == m->synth.raw[4]);
        REQUIRE(m->synth.raw[3] == 206u && m->synth.raw[5] == 206u);
    }
    free(m);
    free(reference);
}

/* Evaluate the window peak at the known physical frequency, not at its
   nearest FFT bin. The known-sine controls validate this off-bin fit. */
static double fitted_amplitude(double frequency)
{
    double r = 0.0, im = 0.0;
    for (unsigned n = 0; n < N; n++) {
        double angle = 2.0 * PI * frequency * n / RATE;
        r += pcm[n] * window[n] * cos(angle);
        im -= pcm[n] * window[n] * sin(angle);
    }
    return 2.0 * hypot(r, im) / window_sum;
}

static struct spectrum measure(const struct tone *t)
{
    for (unsigned n = 0; n < N; n++) {
        real_part[n] = pcm[n] * window[n];
        imaginary[n] = 0.0;
    }
    transform();
    double peak = 0.0;
    unsigned peak_bin = 0;
    for (unsigned k = 1u; k * RATE / N < 16000.0; k++) {
        if (intended(k, t) == 0) {
            double amplitude = 2.0 * hypot(real_part[k], imaginary[k]) / window_sum;
            if (amplitude > peak) {
                peak = amplitude;
                peak_bin = k;
            }
        }
    }
    double reference = fundamental(t) < 16000.0 ? fitted_amplitude(fundamental(t)) : 32767.0;
    double bin_reference = reference;
    if (fundamental(t) < 16000.0) {
        unsigned bin = (unsigned)lround(fundamental(t) * N / RATE);
        bin_reference = 2.0 * hypot(real_part[bin], imaginary[bin]) / window_sum;
    }
    struct spectrum result = {20.0 * log10(peak / reference), peak_bin * RATE / N, reference,
                              20.0 * log10(peak / bin_reference)};
    /* An ultrasonic signal may quantize to exact silence. Its measured
       peak is zero (-infinity dBFS), not an unevaluated case. */
    REQUIRE(!isnan(result.db) && result.db != INFINITY && reference > 0.0);
    return result;
}

static void source_level(muhtari *m, const struct tone *t, uint64_t tick, unsigned high)
{
    for (unsigned chip = 0; chip < t->chips; chip++) {
        for (unsigned ch = 0; ch < t->channels; ch++) {
            muhtari__synth_level(m, tick, 2u + 4u * chip + ch, (uint16_t)(high * 206u));
        }
    }
}

static uint64_t next_edge(const struct tone *t, uint64_t edge)
{
    if (t->tia == 0u) {
        return t->offset + edge * 4u * (t->divisor + 4u);
    }
    /* Zero pulse state first goes high on its fourth enabled B. The
       writes below occur after A0's comparison, enabling that first B. */
    uint64_t b = (edge + 3u) * (t->divisor + 1u);
    return t->offset + (b / 2u) * 456u + (b % 2u == 0u ? 64u : 288u);
}

/* Independent current equation cross-checks the shipped mixer. There is
   no amplitude parameter: declared hardware and active DACs fix the level. */
static int32_t hardware_level(const struct tone *t)
{
    uint64_t current = t->tia != 0u ? (uint64_t)MUHTARI__MIX_Q24 * MUHTARI__TIA_CURRENT
                                    : (uint64_t)t->chips * muhtari__pokey_mix[t->channels * 206u] *
                                          MUHTARI__POKEY_CURRENT;
    uint64_t maximum = (uint64_t)MUHTARI__MIX_Q24 * MUHTARI__TIA_CURRENT +
                       (uint64_t)t->chips * muhtari__pokey_mix[824] * MUHTARI__POKEY_CURRENT;
    REQUIRE(MUHTARI__CART_GAIN_Q12 == 4096u && MUHTARI__SYNTH_MAX_LEVEL == 18360);
    return (int32_t)((current * MUHTARI__SYNTH_MAX_LEVEL + maximum / 2u) / maximum);
}

static void capture(const struct tone *t, int alias)
{
    muhtari *m = calloc(1u, sizeof *m);
    if (m == NULL) {
        fprintf(stderr, "spectral instance allocation failed\n");
        exit(1);
    }
    m->effective_region = pal != 0u ? MUHTARI_REGION_PAL : MUHTARI_REGION_NTSC;
    muhtari__synth_reset(m, 0u, (uint8_t)t->chips);
    if (t->tia != 0u) {
        m->profile.tia_phase = (uint16_t)((456u - t->offset) % 456u);
        muhtari__tia_reset(m);
        for (unsigned ch = 0u; ch < 2u; ch++) {
            muhtari__tia_write(m, (uint16_t)(0x15u + ch), 4u, t->offset);
            muhtari__tia_write(m, (uint16_t)(0x17u + ch), (uint8_t)t->divisor, t->offset);
            muhtari__tia_write(m, (uint16_t)(0x19u + ch), 15u, t->offset);
        }
    }
    uint64_t edge = 0, horizon = 0;
    unsigned samples = 0, high = 0;
    int32_t amplitude = hardware_level(t);
    int64_t previous = 0, blocker = 0;
    while (samples < SETTLE + N) {
        if (alias == 0) {
            uint64_t end = horizon + 13125u;
            if (t->tia != 0u) {
                muhtari__tia_catch_up(m, end - 1u);
                high = m->tia.channel[0].output;
                REQUIRE(m->tia.channel[1].output == high);
            } else {
                while (next_edge(t, edge) < end) {
                    uint64_t tick = next_edge(t, edge++);
                    high ^= 1u;
                    source_level(m, t, tick, high);
                }
            }
            int16_t chunk[89];
            uint32_t count = muhtari__synth_render(m, end, chunk);
            REQUIRE(count == end * sample_num / sample_den - horizon * sample_num / sample_den);
            REQUIRE(count >= 88u && count <= 89u);
            REQUIRE(pal != 0u || count == 88u);
            REQUIRE(m->synth.mixed_level == (high != 0u ? amplitude : 0));
            for (uint32_t i = 0; i < count && samples < SETTLE + N; i++, samples++) {
                if (samples >= SETTLE) {
                    pcm[samples - SETTLE] = chunk[i];
                }
            }
            horizon = end;
        } else {
            /* Nearest-sample kernel bypass. Event rounding alone changes;
               gain, Q30 feedback truncation and int16 rounding match synth.c. */
            while ((next_edge(t, edge) * sample_num + sample_den / 2u) / sample_den <= samples) {
                high ^= 1u;
                edge++;
            }
            int64_t x = (high != 0u ? amplitude : 0) * MUHTARI__SYNTH_Q30;
            blocker = x - previous + (blocker / MUHTARI__SYNTH_Q30) * INT64_C(1070934451) +
                      ((blocker % MUHTARI__SYNTH_Q30) * INT64_C(1070934451)) / MUHTARI__SYNTH_Q30;
            previous = x;
            int64_t value = muhtari__synth_round(blocker, MUHTARI__SYNTH_Q30);
            REQUIRE(value >= INT16_MIN && value <= INT16_MAX);
            if (samples >= SETTLE) {
                pcm[samples - SETTLE] = (int16_t)value;
            }
            samples++;
        }
    }
    REQUIRE(m->synth.clipped_samples == 0u);
    free(m);
}

static void analyzer_controls(void)
{
    struct tone t = {1u, 7u, 0u, 0u, 0u};
    double frequency = fundamental(&t);
    for (unsigned offset = 0; offset < 4u; offset++) {
        /* Four off-bin phases and a physical multi-line square-like sum. */
        for (unsigned n = 0; n < N; n++) {
            double angle = 2.0 * PI * frequency * n / RATE + offset * 0.37;
            pcm[n] = (int16_t)lround(10000.0 * sin(angle));
        }
        REQUIRE(fabs(fitted_amplitude(frequency) / 10000.0 - 1.0) < 0.00001);
        REQUIRE(measure(&t).db < -90.0);
        for (unsigned n = 0; n < N; n++) {
            double angle = 2.0 * PI * frequency * n / RATE + offset * 0.37;
            double value = 0.0;
            for (unsigned h = 1u; h * frequency < 16000.0; h += 2u) {
                value += 10000.0 * sin(h * angle) / h;
            }
            pcm[n] = (int16_t)lround(value);
        }
        REQUIRE(measure(&t).db < -80.0);
    }
    /* Just beyond the five-bin mask, at -60 dBc. */
    for (unsigned n = 0; n < N; n++) {
        pcm[n] = (int16_t)lround(10000.0 * sin(2.0 * PI * frequency * n / RATE) +
                                 10.0 * sin(2.0 * PI * (frequency + 6.0 * RATE / N) * n / RATE));
    }
    REQUIRE(measure(&t).db > -80.0);
    printf("analyzer: off-bin normalization, aggregate physical-line leakage and near-mask spur "
           "checked\n");
}

/* Coordinate-derived identity detects duplicate or missing schedules
   independently of the iteration counter. */
static unsigned case_id(const struct tone *t)
{
    unsigned offset = t->offset == 0u ? 0u : t->offset == 7u ? 1u : 2u;
    REQUIRE(t->offset == 0u || t->offset == 7u || t->offset == 137u);
    if (t->tia != 0u) {
        REQUIRE(t->divisor < 32u && t->chips == 0u && t->channels == 0u);
        return t->divisor * 3u + offset;
    }
    REQUIRE(t->divisor == 0u || t->divisor == 1u || t->divisor == 3u);
    REQUIRE(t->chips == 1u || t->chips == 2u);
    REQUIRE(t->channels == 1u || t->channels == 4u);
    unsigned divisor = t->divisor == 0u ? 0u : t->divisor == 1u ? 1u : 2u;
    return 96u + ((divisor * 2u + t->chips - 1u) * 2u + (t->channels == 1u ? 0u : 1u)) * 3u +
           offset;
}

static void evaluate(const struct tone *t, int controls, uint8_t seen[132], unsigned *rejected,
                     double worst[3])
{
    unsigned id = case_id(t);
    REQUIRE(id < 132u);
    if (id >= 132u) {
        return;
    }
    REQUIRE(seen[id] == 0u);
    seen[id]++;
    capture(t, controls);
    struct spectrum result = measure(t);
    printf("%s AUDF%u chips%u channels%u offset%u: peak %.4f Hz reference %.5f %s %.5f dB margin "
           "%.5f dB\n",
           t->tia != 0u ? "TIA" : "POKEY-equivalent", t->divisor, t->chips, t->channels, t->offset,
           result.frequency, result.reference, fundamental(t) < 16000.0 ? "dBc" : "dBFS", result.db,
           -80.0 - result.db);
    unsigned kind = t->tia != 0u ? 0u : 1u;
    if (result.db > worst[kind]) {
        worst[kind] = result.db;
    }
    if (t->tia != 0u && result.bin_db > worst[2]) {
        worst[2] = result.bin_db;
    }
    if (controls != 0) {
        REQUIRE(result.db >= -80.0);
        *rejected += result.db >= -80.0;
    } else {
        REQUIRE(result.db < -80.0);
    }
}

int main(int argc, char **argv)
{
    if (argc != 2 || (strcmp(argv[1], "production") != 0 && strcmp(argv[1], "controls") != 0)) {
        return 2;
    }
    for (unsigned n = 0; n < N; n++) {
        double a = 2.0 * PI * n / N;
        window[n] = 0.35875 - 0.48829 * cos(a) + 0.14128 * cos(2.0 * a) - 0.01168 * cos(3.0 * a);
        window_sum += window[n];
    }
    REQUIRE(fabs(window_sum - 0.35875 * N) < 0.000001);
    int controls = strcmp(argv[1], "controls") == 0;
    for (pal = 0u; pal < 2u; pal++) {
        sample_num = pal != 0u ? 4000u : 88u;
        sample_den = pal != 0u ? 591149u : 13125u;
        printf("region %s: physical clock %.9f Hz, samples %u/%u\n", pal != 0u ? "PAL" : "NTSC",
               pal != 0u ? 7093788.0 : 315000000.0 / 44.0, sample_num, sample_den);
        if (controls != 0) {
            analyzer_controls();
        }
        const unsigned offsets[] = {0u, 7u, 137u};
        const unsigned divisors[] = {0u, 1u, 3u};
        uint8_t seen[132] = {0};
        double worst[3] = {-1000.0, -1000.0, -1000.0};
        unsigned evaluated = 0, rejected = 0;
        for (unsigned divisor = 0; divisor < 32u; divisor++) {
            for (unsigned offset = 0; offset < 3u; offset++) {
                struct tone t = {1u, divisor, 0u, 0u, offsets[offset]};
                evaluate(&t, controls, seen, &rejected, worst);
            }
        }
        for (unsigned divisor = 0; divisor < 3u; divisor++) {
            for (unsigned chips = 1u; chips <= 2u; chips++) {
                for (unsigned channels = 1u; channels <= 4u; channels += 3u) {
                    for (unsigned offset = 0; offset < 3u; offset++) {
                        struct tone t = {0u, divisors[divisor], chips, channels, offsets[offset]};
                        evaluate(&t, controls, seen, &rejected, worst);
                    }
                }
            }
        }
        for (unsigned id = 0; id < 132u; id++) {
            REQUIRE(seen[id] == 1u);
            evaluated += seen[id] == 1u;
        }
        REQUIRE(evaluated == 132u);
        REQUIRE(controls == 0 || rejected == 132u);
        if (controls == 0) {
            double actual_worst = -INFINITY;
            unsigned actual_cases = 0u;
            for (unsigned divisor = 0; divisor < 3u; divisor++) {
                for (unsigned chips = 1u; chips <= 2u; chips++) {
                    for (unsigned channels = 1u; channels <= 4u; channels += 3u) {
                        for (unsigned offset = 0; offset < 3u; offset++) {
                            for (unsigned slow = 0; slow < 2u; slow++) {
                                struct tone t = {0u, divisors[divisor], chips, channels,
                                                 offsets[offset]};
                                capture_pokey(&t, slow, 0u, 0u, 0u);
                                struct spectrum result = measure(&t);
                                REQUIRE(result.db < -80.0);
                                if (result.db > actual_worst) {
                                    actual_worst = result.db;
                                }
                                printf(
                                    "actual POKEY AUDF%u chips%u active%u offset%u %s: %.5f dBFS "
                                    "spur %.5f Hz\n",
                                    t.divisor, chips, chips * channels, t.offset,
                                    slow != 0u ? "slow" : "fast", result.db, result.frequency);
                                actual_cases++;
                            }
                        }
                    }
                }
            }
            REQUIRE(actual_cases == 72u);
            printf("actual POKEY: %u cases, worst %.5f dBFS\n", actual_cases, actual_worst);
            /* Compact representatives from the bounded mixed-clock recovery
               exploration. Both physical raw periods remain ultrasonic, so
               the existing judge masks no intended lines. */
            for (unsigned hp = 0u; hp < 2u; hp++) {
                struct tone t = {0u, hp != 0u ? 2u : 1u, 2u, 1u, hp != 0u ? 661u : 29u};
                capture_pokey(&t, 0u, 3u, 3u, hp);
                struct spectrum result = measure(&t);
                REQUIRE(result.db < -80.0);
                printf("mixed representative HP%u: %.8f dBFS\n", hp, result.db);
            }
        }
        if (controls == 0u) {
            /* Joined $0000+HP and two-tone AUDF0+HP repeat within28
               clocks (manual chapter5). Even wholly slow gives >=42kHz:
               no physical line is in the judged band and none is masked. */
            for (unsigned mode = 2u; mode <= 3u; mode++) {
                for (unsigned slow = 0u; slow < 2u; slow++) {
                    struct tone t = {0u, 0u, 2u, 1u, 29u};
                    capture_pokey(&t, slow, 0u, 0u, mode);
                    struct spectrum result = measure(&t);
                    REQUIRE(result.db < -80.0);
                    printf("joined/two-tone HP mode%u slow%u: %.8f dBFS\n", mode, slow, result.db);
                }
            }
        }
        printf("worst TIA %.5f dBc, POKEY-equivalent %.5f dBFS\n", worst[0], worst[1]);
        printf("research nearest-bin reference cross-check: TIA %.5f dBc\n", worst[2]);
        printf("%u evaluated, %u alias controls rejected, %d failures\n", evaluated, rejected,
               failures);
    }
    return failures == 0 ? 0 : 1;
}
