/* Integer reconstruction and blocker; docs/hardware/audio.md derives the
   generated coefficients, rounding, normalization and storage bounds. */
#include <assert.h>
#include <string.h>

#include "internal.h"
#include "audio_tables.h"

/* The compact Nyquist-sinc/Kaiser-beta8 integrals are regenerated at Q23;
   the interpolation and impulse-ring architecture follows the MIT sibling. */

const int32_t muhtari__synth_kernel[MUHTARI__SYNTH_PHASES][16] = {
    {1957, -7011, 17811, -37820, 75090, -164986, 667267, 7283992, 667267, -164986, 75090, -37820,
     17811, -7011, 1957, 0},
    {1791, -6135, 15015, -30727, 59262, -130421, 562788, 7281341, 774721, -200003, 91109, -45008,
     20652, -7905, 2141, -13},
    {1629, -5280, 12270, -23745, 43661, -96384, 461367, 7273400, 885060, -235396, 107283, -52275,
     23531, -8815, 2330, -28},
    {1473, -4445, 9583, -16889, 28321, -62945, 363081, 7260167, 998189, -271082, 123573, -59602,
     26443, -9740, 2524, -43},
    {1323, -3635, 6957, -10173, 13274, -30173, 268000, 7241676, 1114008, -306980, 139939, -66973,
     29379, -10677, 2722, -59},
    {1178, -2848, 4399, -3611, -1448, 1867, 176188, 7217945, 1232409, -343002, 156342, -74370,
     32334, -11624, 2925, -76},
    {1039, -2088, 1913, 2784, -15816, 33114, 87706, 7189010, 1353281, -379060, 172740, -81772,
     35298, -12578, 3131, -94},
    {906, -1355, -497, 9001, -29802, 63510, 2603, 7154916, 1476506, -415063, 189090, -89162, 38265,
     -13537, 3339, -112},
    {779, -649, -2826, 15027, -43379, 92999, -79073, 7115710, 1601961, -450917, 205349, -96518,
     41226, -14499, 3550, -132},
    {658, 27, -5072, 20852, -56523, 121532, -157285, 7071452, 1729518, -486526, 221472, -103821,
     44173, -15460, 3763, -152},
    {543, 674, -7229, 26466, -69211, 149061, -231998, 7022203, 1859043, -521793, 237415, -111050,
     47097, -16418, 3977, -172},
    {434, 1291, -9297, 31861, -81422, 175541, -303187, 6968042, 1990398, -556618, 253131, -118184,
     49990, -17370, 4191, -193},
    {332, 1876, -11271, 37029, -93136, 200933, -370830, 6909035, 2123443, -590899, 268576, -125201,
     52843, -18312, 4405, -215},
    {236, 2431, -13150, 41962, -104334, 225200, -434914, 6845279, 2258028, -624535, 283701, -132081,
     55646, -19242, 4618, -237},
    {146, 2954, -14932, 46654, -115003, 248309, -495431, 6776860, 2394005, -657420, 298461, -138800,
     58391, -20156, 4829, -259},
    {62, 3446, -16614, 51098, -125125, 270231, -552379, 6703878, 2531217, -689450, 312809, -145338,
     61068, -21052, 5038, -281},
    {-15, 3905, -18197, 55291, -134691, 290940, -605762, 6626443, 2669506, -720519, 326697, -151672,
     63668, -21925, 5243, -304},
    {-87, 4334, -19679, 59228, -143687, 310414, -655589, 6544659, 2808710, -750519, 340079, -157781,
     66181, -22772, 5443, -326},
    {-153, 4730, -21058, 62907, -152106, 328635, -701878, 6458647, 2948665, -779344, 352908,
     -163642, 68598, -23591, 5638, -348},
    {-213, 5095, -22336, 66323, -159939, 345587, -744650, 6368535, 3089201, -806886, 365136,
     -169234, 70909, -24376, 5826, -370},
    {-267, 5429, -23512, 69477, -167181, 361259, -783931, 6274443, 3230149, -833037, 376719,
     -174534, 73104, -25125, 6007, -392},
    {-316, 5733, -24585, 72366, -173828, 375642, -819756, 6176511, 3371335, -857690, 387610,
     -179522, 75175, -25835, 6180, -412},
    {-360, 6006, -25557, 74991, -179878, 388732, -852161, 6074877, 3512584, -880737, 397765,
     -184177, 77112, -26501, 6344, -432},
    {-398, 6249, -26429, 77352, -185329, 400528, -881190, 5969683, 3653720, -902072, 407140,
     -188476, 78905, -27120, 6496, -451},
    {-432, 6464, -27201, 79450, -190182, 411030, -906891, 5861079, 3794563, -921589, 415692,
     -192401, 80545, -27688, 6638, -469},
    {-460, 6650, -27875, 81287, -194439, 420245, -929318, 5749218, 3934934, -939183, 423378,
     -195931, 82023, -28202, 6767, -486},
    {-485, 6808, -28452, 82866, -198104, 428179, -948528, 5634259, 4074652, -954751, 430157,
     -199046, 83331, -28659, 6882, -501},
    {-505, 6940, -28934, 84190, -201182, 434844, -964583, 5516354, 4213537, -968190, 435992,
     -201729, 84459, -29054, 6983, -514},
    {-520, 7045, -29324, 85262, -203680, 440253, -977551, 5395677, 4351407, -979400, 440842,
     -203960, 85400, -29385, 7067, -525},
    {-532, 7126, -29622, 86086, -205605, 444423, -987502, 5272391, 4488080, -988283, 444672,
     -205724, 86145, -29648, 7135, -534},
    {-541, 7182, -29832, 86668, -206967, 447374, -994511, 5146667, 4623375, -994742, 447447,
     -207002, 86686, -29840, 7185, -541},
    {-545, 7215, -29956, 87013, -207775, 449126, -998655, 5018672, 4757113, -998684, 449135,
     -207779, 87015, -29957, 7215, -545},
    {-547, 7226, -29997, 87127, -208042, 449704, -1000018, 4889113, 4888589, -1000018, 449704,
     -208042, 87127, -29997, 7226, -547},
    {-545, 7215, -29957, 87015, -207779, 449135, -998684, 4757113, 5018672, -998655, 449126,
     -207775, 87013, -29956, 7215, -545},
    {-541, 7185, -29840, 86686, -207002, 447447, -994742, 4623375, 5146667, -994511, 447374,
     -206967, 86668, -29832, 7182, -541},
    {-534, 7135, -29648, 86145, -205724, 444672, -988283, 4488080, 5272391, -987502, 444423,
     -205605, 86086, -29622, 7126, -532},
    {-525, 7067, -29385, 85400, -203960, 440842, -979400, 4351407, 5395677, -977551, 440253,
     -203680, 85262, -29324, 7045, -520},
    {-514, 6983, -29054, 84459, -201729, 435992, -968190, 4213537, 5516354, -964583, 434844,
     -201182, 84190, -28934, 6940, -505},
    {-501, 6882, -28659, 83331, -199046, 430157, -954751, 4074652, 5634259, -948528, 428179,
     -198104, 82866, -28452, 6808, -485},
    {-486, 6767, -28202, 82023, -195931, 423378, -939183, 3934934, 5749218, -929318, 420245,
     -194439, 81287, -27875, 6650, -460},
    {-469, 6638, -27688, 80545, -192401, 415692, -921589, 3794563, 5861079, -906891, 411030,
     -190182, 79450, -27201, 6464, -432},
    {-451, 6496, -27120, 78905, -188476, 407140, -902072, 3653720, 5969683, -881190, 400528,
     -185329, 77352, -26429, 6249, -398},
    {-432, 6344, -26501, 77112, -184177, 397765, -880737, 3512584, 6074877, -852161, 388732,
     -179878, 74991, -25557, 6006, -360},
    {-412, 6180, -25835, 75175, -179522, 387610, -857690, 3371335, 6176511, -819756, 375642,
     -173828, 72366, -24585, 5733, -316},
    {-392, 6007, -25125, 73104, -174534, 376719, -833037, 3230149, 6274443, -783931, 361259,
     -167181, 69477, -23512, 5429, -267},
    {-370, 5826, -24376, 70909, -169234, 365136, -806886, 3089201, 6368535, -744650, 345587,
     -159939, 66323, -22336, 5095, -213},
    {-348, 5638, -23591, 68598, -163642, 352908, -779344, 2948665, 6458647, -701878, 328635,
     -152106, 62907, -21058, 4730, -153},
    {-326, 5443, -22772, 66181, -157781, 340079, -750519, 2808710, 6544659, -655589, 310414,
     -143687, 59228, -19679, 4334, -87},
    {-304, 5243, -21925, 63668, -151672, 326697, -720519, 2669506, 6626443, -605762, 290940,
     -134691, 55291, -18197, 3905, -15},
    {-281, 5038, -21052, 61068, -145338, 312809, -689450, 2531217, 6703878, -552379, 270231,
     -125125, 51098, -16614, 3446, 62},
    {-259, 4829, -20156, 58391, -138800, 298461, -657420, 2394005, 6776860, -495431, 248309,
     -115003, 46654, -14932, 2954, 146},
    {-237, 4618, -19242, 55646, -132081, 283701, -624535, 2258028, 6845279, -434914, 225200,
     -104334, 41962, -13150, 2431, 236},
    {-215, 4405, -18312, 52843, -125201, 268576, -590899, 2123443, 6909035, -370830, 200933, -93136,
     37029, -11271, 1876, 332},
    {-193, 4191, -17370, 49990, -118184, 253131, -556618, 1990398, 6968042, -303187, 175541, -81422,
     31861, -9297, 1291, 434},
    {-172, 3977, -16418, 47097, -111050, 237415, -521793, 1859043, 7022203, -231998, 149061, -69211,
     26466, -7229, 674, 543},
    {-152, 3763, -15460, 44173, -103821, 221472, -486526, 1729518, 7071452, -157285, 121532, -56523,
     20852, -5072, 27, 658},
    {-132, 3550, -14499, 41226, -96518, 205349, -450917, 1601961, 7115710, -79073, 92999, -43379,
     15027, -2826, -649, 779},
    {-112, 3339, -13537, 38265, -89162, 189090, -415063, 1476506, 7154916, 2603, 63510, -29802,
     9001, -497, -1355, 906},
    {-94, 3131, -12578, 35298, -81772, 172740, -379060, 1353281, 7189010, 87706, 33114, -15816,
     2784, 1913, -2088, 1039},
    {-76, 2925, -11624, 32334, -74370, 156342, -343002, 1232409, 7217945, 176188, 1867, -1448,
     -3611, 4399, -2848, 1178},
    {-59, 2722, -10677, 29379, -66973, 139939, -306980, 1114008, 7241676, 268000, -30173, 13274,
     -10173, 6957, -3635, 1323},
    {-43, 2524, -9740, 26443, -59602, 123573, -271082, 998189, 7260167, 363081, -62945, 28321,
     -16889, 9583, -4445, 1473},
    {-28, 2330, -8815, 23531, -52275, 107283, -235396, 885060, 7273400, 461367, -96384, 43661,
     -23745, 12270, -5280, 1629},
    {-13, 2141, -7905, 20652, -45008, 91109, -200003, 774721, 7281341, 562788, -130421, 59262,
     -30727, 15015, -6135, 1791},
};

/* Q30 exp(-2*pi*20/48000), evaluated offline. */
#define SYNTH_POLE INT64_C(1070934451)

/* Divide unsigned magnitude, then restore sign: nearest, ties away from
   zero. The documented bounds keep the rounded quotient in int64_t. */
int64_t muhtari__synth_round(int64_t value, int64_t divisor)
{
    uint64_t magnitude = value < 0 ? UINT64_C(0) - (uint64_t)value : (uint64_t)value;
    uint64_t denominator = (uint64_t)divisor;
    int64_t rounded = (int64_t)((magnitude + denominator / 2u) / denominator);
    return value < 0 ? -rounded : rounded;
}

void muhtari__synth_coefficients(uint32_t phase, int32_t out[16])
{
    muhtari__synth_coefficients_for_period(phase, 13125u, out);
}

void muhtari__synth_coefficients_for_period(uint32_t phase, uint32_t period, int32_t out[16])
{
    uint32_t numerator = phase * MUHTARI__SYNTH_PHASES;
    unsigned row = numerator / period;
    uint32_t fraction = numerator % period;
    int32_t sum = 0;
    for (unsigned j = 0; j < 16u; j++) {
        int32_t a = muhtari__synth_kernel[row][j];
        int32_t b = row + 1u < MUHTARI__SYNTH_PHASES
                        ? muhtari__synth_kernel[row + 1u][j]
                        : (j == 0u ? 0 : muhtari__synth_kernel[0][j - 1u]);
        out[j] = a + (int32_t)muhtari__synth_round((int64_t)(b - a) * fraction, period);
        sum += out[j];
    }
    out[phase < period / 2u ? 7u : 8u] += (int32_t)MUHTARI__SYNTH_Q23 - sum;
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
        uint32_t to_sample = (s->sample_den - s->phase + s->sample_num - 1u) / s->sample_num;
        uint64_t remaining = end - s->tick;
        uint32_t delta = remaining < to_sample ? (uint32_t)remaining : to_sample;
        s->tick += delta;
        s->phase += delta * s->sample_num;
        if (s->phase >= s->sample_den) {
            s->phase -= s->sample_den;
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
            raw += s->raw[2u + 4u * (chip + s->secondary) + ch];
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
        muhtari__synth_coefficients_for_period(s->phase, s->sample_den, coefficients);
        for (unsigned j = 0; j < 16u; j++) {
            s->impulse[(s->head + j) & 15u] += (int64_t)delta * coefficients[j];
        }
        s->kernel_adds++;
        s->mixed_level = level;
    }
    s->pending = 0u;
}

/* At most ten source changes per master tick, including both half-edge
   subevents and a CPU write. Only instruction overshoot is retained;
   docs/hardware/audio.md derives the shared tick bound. */
#define OBS_CARRY (10u * (MUHTARI__SYNTH_OVERSHOOT_TICKS + 1u))
#define OBS_PENDING 32u
struct raw_record {
    uint8_t source, level;
};
struct carry_record {
    uint16_t offset;
    uint8_t source, level;
};
struct muhtari__observation {
    uint64_t end, tick, base;
    uint32_t count, carried;
    uint8_t raw[10];
    struct raw_record pending[OBS_PENDING];
    struct carry_record carry[OBS_CARRY];
};

muhtari_status muhtari__synth_observe(muhtari *m, muhtari__audio_observer callback, void *user)
{
    if (callback != NULL && m->observation == NULL) {
        /* Private host setup must precede load; no late allocation. */
        if (m->loaded != 0u)
            return MUHTARI_STATUS_ERR_ARGUMENT;
        m->observation = m->allocator.alloc(m->allocator.user, sizeof *m->observation);
        if (m->observation == NULL)
            return MUHTARI_STATUS_ERR_NO_MEMORY;
        memset(m->observation, 0, sizeof *m->observation);
        m->observation->end = UINT64_MAX;
    }
    if (callback == NULL && m->observation != NULL) {
        m->allocator.free(m->allocator.user, m->observation, sizeof *m->observation);
        m->observation = NULL;
    }
    m->observer = callback;
    m->observer_user = user;
    return MUHTARI_STATUS_OK;
}

static void observe_flush(muhtari *m)
{
    struct muhtari__observation *o = m->observation;
    /* Stable source order preserves multiple subevents on one source. */
    for (unsigned source = 0; source < 10u; source++) {
        for (uint32_t i = 0; i < o->count; i++) {
            struct raw_record r = o->pending[i];
            if (r.source != source)
                continue;
            if (o->tick < o->end) {
                m->observer(m->observer_user, o->tick, r.source, r.level);
            } else {
                assert(o->carried < OBS_CARRY);
                assert(o->tick - o->end <= MUHTARI__SYNTH_OVERSHOOT_TICKS);
                o->base = o->end;
                struct carry_record *c = &o->carry[o->carried++];
                c->offset = (uint16_t)(o->tick - o->end);
                c->source = r.source;
                c->level = r.level;
            }
        }
    }
    o->count = 0u;
}

void muhtari__synth_raw(muhtari *m, uint64_t tick, unsigned source, uint8_t raw)
{
    if (m->observer == NULL)
        return;
    struct muhtari__observation *o = m->observation;
    assert(source < 10u && raw <= 15u);
    if (o->raw[source] == raw)
        return;
    if (o->count != 0u && o->tick != tick) {
        assert(tick > o->tick);
        observe_flush(m);
    }
    assert(o->count < OBS_PENDING);
    o->tick = tick;
    o->raw[source] = raw;
    o->pending[o->count].source = (uint8_t)source;
    o->pending[o->count++].level = raw;
}

void muhtari__observe_begin(muhtari *m, uint64_t end)
{
    if (m->observer == NULL)
        return;
    struct muhtari__observation *o = m->observation;
    o->end = end;
    for (uint32_t i = 0; i < o->carried; i++) {
        struct carry_record c = o->carry[i];
        assert(o->base + c.offset < end);
        m->observer(m->observer_user, o->base + c.offset, c.source, c.level);
    }
    o->carried = 0u;
}

void muhtari__observe_end(muhtari *m)
{
    if (m->observer != NULL)
        observe_flush(m);
}

void muhtari__synth_reset(muhtari *m, uint64_t origin, uint8_t pokeys)
{
    assert(pokeys <= 2u);
    if (m->observation != NULL) {
        memset(m->observation, 0, sizeof *m->observation);
        m->observation->end = UINT64_MAX;
    }
    memset(&m->synth, 0, sizeof m->synth);
    m->synth.origin = origin;
    m->synth.tick = origin;
    m->synth.horizon = origin;
    m->synth.pokeys = pokeys;
    struct muhtari__timing timing = muhtari__timing(m);
    m->synth.sample_num = timing.sample_num;
    m->synth.sample_den = timing.sample_den;
    m->synth.max_samples = timing.max_samples;
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
    uint64_t count =
        (b / s->sample_den) * s->sample_num + (b % s->sample_den) * s->sample_num / s->sample_den -
        ((a / s->sample_den) * s->sample_num + (a % s->sample_den) * s->sample_num / s->sample_den);
    assert(count <= s->max_samples && count <= s->pcm_count);
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
