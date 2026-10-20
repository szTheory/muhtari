/* Integer reconstruction and blocker; docs/hardware/audio.md derives the
   generated coefficients, rounding, normalization and storage bounds. */
#include <assert.h>
#include <string.h>

#include "internal.h"
#include "audio_tables.h"

/* The compact Nyquist-sinc/Kaiser-beta8 integrals are regenerated at Q23;
   the interpolation and impulse-ring architecture follows the MIT sibling. */

/* Q30 exp(-2*pi*20/48000), evaluated offline. */
const int32_t muhtari__synth_kernel[32][16] = {
    {1957, -7011, 17811, -37820, 75090, -164986, 667267, 7283992, 667267, -164986, 75090, -37820,
     17811, -7011, 1957, 0},
    {1629, -5280, 12270, -23745, 43661, -96384, 461367, 7273400, 885060, -235396, 107283, -52275,
     23531, -8815, 2330, -28},
    {1323, -3635, 6957, -10173, 13274, -30173, 268000, 7241676, 1114008, -306980, 139939, -66973,
     29379, -10677, 2722, -59},
    {1039, -2088, 1913, 2784, -15816, 33114, 87706, 7189010, 1353281, -379060, 172740, -81772,
     35298, -12578, 3131, -94},
    {779, -649, -2826, 15027, -43379, 92999, -79073, 7115710, 1601961, -450917, 205349, -96518,
     41226, -14499, 3550, -132},
    {543, 674, -7229, 26466, -69211, 149061, -231998, 7022203, 1859043, -521793, 237415, -111050,
     47097, -16418, 3977, -172},
    {332, 1876, -11271, 37029, -93136, 200933, -370830, 6909035, 2123443, -590899, 268576, -125201,
     52843, -18312, 4405, -215},
    {146, 2954, -14932, 46654, -115003, 248309, -495431, 6776860, 2394005, -657420, 298461, -138800,
     58391, -20156, 4829, -259},
    {-15, 3905, -18197, 55291, -134691, 290940, -605762, 6626443, 2669506, -720519, 326697, -151672,
     63668, -21925, 5243, -304},
    {-153, 4730, -21058, 62907, -152106, 328635, -701878, 6458647, 2948665, -779344, 352908,
     -163642, 68598, -23591, 5638, -348},
    {-267, 5429, -23512, 69477, -167181, 361259, -783931, 6274443, 3230149, -833037, 376719,
     -174534, 73104, -25125, 6007, -392},
    {-360, 6006, -25557, 74991, -179878, 388732, -852161, 6074877, 3512584, -880737, 397765,
     -184177, 77112, -26501, 6344, -432},
    {-432, 6464, -27201, 79450, -190182, 411030, -906891, 5861079, 3794563, -921589, 415692,
     -192401, 80545, -27688, 6638, -469},
    {-485, 6808, -28452, 82866, -198104, 428179, -948528, 5634259, 4074652, -954751, 430157,
     -199046, 83331, -28659, 6882, -501},
    {-520, 7045, -29324, 85262, -203680, 440253, -977551, 5395677, 4351407, -979400, 440842,
     -203960, 85400, -29385, 7067, -525},
    {-541, 7182, -29832, 86668, -206967, 447374, -994511, 5146667, 4623375, -994742, 447447,
     -207002, 86686, -29840, 7185, -541},
    {-547, 7226, -29997, 87127, -208042, 449704, -1000018, 4889113, 4888589, -1000018, 449704,
     -208042, 87127, -29997, 7226, -547},
    {-541, 7185, -29840, 86686, -207002, 447447, -994742, 4623375, 5146667, -994511, 447374,
     -206967, 86668, -29832, 7182, -541},
    {-525, 7067, -29385, 85400, -203960, 440842, -979400, 4351407, 5395677, -977551, 440253,
     -203680, 85262, -29324, 7045, -520},
    {-501, 6882, -28659, 83331, -199046, 430157, -954751, 4074652, 5634259, -948528, 428179,
     -198104, 82866, -28452, 6808, -485},
    {-469, 6638, -27688, 80545, -192401, 415692, -921589, 3794563, 5861079, -906891, 411030,
     -190182, 79450, -27201, 6464, -432},
    {-432, 6344, -26501, 77112, -184177, 397765, -880737, 3512584, 6074877, -852161, 388732,
     -179878, 74991, -25557, 6006, -360},
    {-392, 6007, -25125, 73104, -174534, 376719, -833037, 3230149, 6274443, -783931, 361259,
     -167181, 69477, -23512, 5429, -267},
    {-348, 5638, -23591, 68598, -163642, 352908, -779344, 2948665, 6458647, -701878, 328635,
     -152106, 62907, -21058, 4730, -153},
    {-304, 5243, -21925, 63668, -151672, 326697, -720519, 2669506, 6626443, -605762, 290940,
     -134691, 55291, -18197, 3905, -15},
    {-259, 4829, -20156, 58391, -138800, 298461, -657420, 2394005, 6776860, -495431, 248309,
     -115003, 46654, -14932, 2954, 146},
    {-215, 4405, -18312, 52843, -125201, 268576, -590899, 2123443, 6909035, -370830, 200933, -93136,
     37029, -11271, 1876, 332},
    {-172, 3977, -16418, 47097, -111050, 237415, -521793, 1859043, 7022203, -231998, 149061, -69211,
     26466, -7229, 674, 543},
    {-132, 3550, -14499, 41226, -96518, 205349, -450917, 1601961, 7115710, -79073, 92999, -43379,
     15027, -2826, -649, 779},
    {-94, 3131, -12578, 35298, -81772, 172740, -379060, 1353281, 7189010, 87706, 33114, -15816,
     2784, 1913, -2088, 1039},
    {-59, 2722, -10677, 29379, -66973, 139939, -306980, 1114008, 7241676, 268000, -30173, 13274,
     -10173, 6957, -3635, 1323},
    {-28, 2330, -8815, 23531, -52275, 107283, -235396, 885060, 7273400, 461367, -96384, 43661,
     -23745, 12270, -5280, 1629},
};

#define SYNTH_POLE INT64_C(1070934451)

/* C division truncates toward zero. Round signed halves away from zero;
   no right shift of a negative value or floating operation is used. */
int64_t muhtari__synth_round(int64_t value, int64_t divisor)
{
    return value >= 0 ? (value + divisor / 2) / divisor : -((-value + divisor / 2) / divisor);
}

void muhtari__synth_coefficients(uint32_t phase, int32_t out[16])
{
    uint32_t numerator = phase * MUHTARI__SYNTH_PHASES;
    unsigned row = numerator / MUHTARI__TICKS_PER_PERIOD;
    uint32_t fraction = numerator % MUHTARI__TICKS_PER_PERIOD;
    int32_t sum = 0;
    for (unsigned j = 0; j < 16u; j++) {
        int32_t a = muhtari__synth_kernel[row][j];
        int32_t b = row < 31u ? muhtari__synth_kernel[row + 1u][j]
                              : (j == 0u ? 0 : muhtari__synth_kernel[0][j - 1u]);
        out[j] = a + (int32_t)muhtari__synth_round((int64_t)(b - a) * fraction, 13125);
        sum += out[j];
    }
    out[phase < 13125u / 2u ? 7u : 8u] += (int32_t)MUHTARI__SYNTH_Q23 - sum;
}

static int64_t multiply_q30(int64_t value)
{
    return (value / MUHTARI__SYNTH_Q30) * SYNTH_POLE +
           ((value % MUHTARI__SYNTH_Q30) * SYNTH_POLE) / MUHTARI__SYNTH_Q30;
}

static void sample(struct muhtari__synth *s)
{
    s->integrator_q23 += s->impulse[s->head];
    s->impulse[s->head] = 0;
    s->head = (uint8_t)((s->head + 1u) & 15u);
    int64_t x = s->integrator_q23 * 128;
    s->blocker_q30 = x - s->previous_q30 + multiply_q30(s->blocker_q30);
    s->previous_q30 = x;
    int64_t pcm = muhtari__synth_round(s->blocker_q30, MUHTARI__SYNTH_Q30);
    uint64_t peak = (uint64_t)(pcm < 0 ? -pcm : pcm);
    if (peak > s->pre_clip_peak) {
        s->pre_clip_peak = peak;
    }
    if (pcm > INT16_MAX || pcm < INT16_MIN) {
        s->clipped_samples++;
        pcm = pcm > INT16_MAX ? INT16_MAX : INT16_MIN;
    }
    assert(s->pcm_count < MUHTARI__SYNTH_PCM_CAPACITY);
    uint32_t slot = (s->pcm_read + s->pcm_count) % MUHTARI__SYNTH_PCM_CAPACITY;
    s->pcm[slot] = (int16_t)pcm;
    s->pcm_count++;
    if (s->kernel_adds > s->kernel_adds_per_sample) {
        s->kernel_adds_per_sample = s->kernel_adds;
    }
    s->kernel_adds = 0;
}

/* Process [tick,end), as in the research event-before-sample convention:
   an event at t uses phase 88*t mod 13125 before that tick's increment. */
static void advance(struct muhtari__synth *s, uint64_t end)
{
    while (s->tick < end) {
        uint32_t to_sample = (13125u - s->phase + 87u) / 88u;
        uint64_t remaining = end - s->tick;
        uint32_t delta = remaining < to_sample ? (uint32_t)remaining : to_sample;
        s->tick += delta;
        s->phase += delta * 88u;
        if (s->phase >= 13125u) {
            s->phase -= 13125u;
            sample(s);
        }
    }
}

static void flush(struct muhtari__synth *s)
{
    if (s->pending == 0u) {
        return;
    }
    advance(s, s->pending_tick);
    /* Node-current weights are (25/180):(38/68) = 85:342;
       POKEY nonlinearity applies after each chip's weighted DAC sum
       (A7800-AUDIO section 3). The declared hardware fixes normalization. */
    uint64_t current = (uint64_t)muhtari__tia_mix[s->raw[0] + s->raw[1]] * MUHTARI__TIA_CURRENT;
    for (unsigned chip = 0; chip < s->pokeys; chip++) {
        unsigned raw = 0;
        for (unsigned ch = 0; ch < 4u; ch++) {
            raw += s->raw[2u + 4u * chip + ch];
        }
        current += (uint64_t)muhtari__pokey_mix[raw] * MUHTARI__POKEY_CURRENT *
                   MUHTARI__CART_GAIN_Q12 / 4096u;
    }
    uint64_t maximum = (uint64_t)MUHTARI__MIX_Q24 * MUHTARI__TIA_CURRENT +
                       (uint64_t)s->pokeys * muhtari__pokey_mix[824] * MUHTARI__POKEY_CURRENT *
                           MUHTARI__CART_GAIN_Q12 / 4096u;
    int32_t level = (int32_t)((current * MUHTARI__SYNTH_MAX_LEVEL + maximum / 2u) / maximum);
    int32_t delta = level - s->mixed_level;
    if (delta != 0) {
        int32_t coefficients[16];
        muhtari__synth_coefficients(s->phase, coefficients);
        for (unsigned j = 0; j < 16u; j++) {
            s->impulse[(s->head + j) & 15u] += (int64_t)delta * coefficients[j];
        }
        s->kernel_adds++;
        s->mixed_level = level;
    }
    s->pending = 0u;
}

void muhtari__synth_reset(muhtari *m, uint64_t origin, uint8_t pokeys)
{
    assert(pokeys <= 2u);
    memset(&m->synth, 0, sizeof m->synth);
    m->synth.origin = origin;
    m->synth.tick = origin;
    m->synth.horizon = origin;
    m->synth.pokeys = pokeys;
}

void muhtari__synth_level(muhtari *m, uint64_t tick, unsigned source, uint16_t level)
{
    struct muhtari__synth *s = &m->synth;
    assert(source < 10u);
    assert(level <= (source < 2u ? 15u : 206u));
    assert(tick >= s->tick);
    assert(s->pending == 0u || tick >= s->pending_tick);
    if (s->pending != 0u && tick != s->pending_tick) {
        flush(s);
    }
    assert(s->pending == 0u || tick == s->pending_tick);
    s->raw[source] = level;
    s->pending_tick = tick;
    s->pending = 1u;
}

uint32_t muhtari__synth_render(muhtari *m, uint64_t end, int16_t *out)
{
    struct muhtari__synth *s = &m->synth;
    assert(end >= s->horizon);
    /* A pending tick at the boundary belongs to the next interval. Earlier
       hardware catch-up may already have queued future PCM, which stays. */
    if (s->pending != 0u && s->pending_tick < end) {
        flush(s);
    }
    advance(s, end);
    uint64_t a = s->horizon - s->origin;
    uint64_t b = end - s->origin;
    uint64_t count = (b / 13125u) * 88u + (b % 13125u) * 88u / 13125u -
                     ((a / 13125u) * 88u + (a % 13125u) * 88u / 13125u);
    assert(count <= 801u && count <= s->pcm_count);
    for (uint32_t i = 0; i < (uint32_t)count; i++) {
        if (out != NULL) {
            out[i] = s->pcm[s->pcm_read];
        }
        s->pcm_read = (s->pcm_read + 1u) % MUHTARI__SYNTH_PCM_CAPACITY;
    }
    s->pcm_count -= (uint32_t)count;
    if (s->pcm_count > s->carry_high_water) {
        s->carry_high_water = s->pcm_count;
    }
    s->horizon = end;
    return (uint32_t)count;
}
