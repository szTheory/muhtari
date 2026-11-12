/* SPDX-License-Identifier: MIT AND BSD-3-Clause
 * Copyright (c) 2026 muhtari contributors
 * Derived from aaron-giles/ymfm src/ymfm_opm.cpp, src/ymfm_opm.h,
 * src/ymfm_fm.ipp, src/ymfm_fm.h and src/ymfm.h at
 * 81aec25ccbb98f4873a255f7551ac4dadac59b4a (BSD-3-Clause), copyright
 * Aaron Giles. Changes: OPM-only fixed-width C17 specialization, per-instance
 * state, bounded tick interface and Muhtari mono-source integration. Texts:
 * LICENSE, THIRD-PARTY-NOTICES.md "YMFM OPM".
 */
#include "internal.h"

#include <assert.h>
#include <limits.h>
#include <string.h>

#define YM_CHANNELS 8u
#define YM_OPERATORS 32u
#define YM_ALL_CHANNELS UINT32_C(0xff)
#define YM_OUTPUTS 2u
#define YM_WAVEFORM_LENGTH 1024u
#define YM_EG_QUIET 0x380u
#define YM_EG_ATTACK 1u
#define YM_EG_DECAY 2u
#define YM_EG_SUSTAIN 3u
#define YM_EG_RELEASE 4u
#define YM_PHASE_STEP_DYNAMIC 1u
#define YM_CLOCKS_PER_SAMPLE 64u
#define YM_BUSY_CLOCKS 68u /* Yamaha YM2151 Application Manual, section 2.1.1 */

/* These lookup values are transcribed from the approved OPM-only pin. */
static const uint16_t ym_sin_table[256] = {
    2137, 1731, 1543, 1419, 1326, 1252, 1190, 1137, 1091, 1050, 1013, 979, 949, 920, 894, 869,
    846, 825, 804, 785, 767, 749, 732, 717, 701, 687, 672, 659, 646, 633, 621, 609,
    598, 587, 576, 566, 556, 546, 536, 527, 518, 509, 501, 492, 484, 476, 468, 461,
    453, 446, 439, 432, 425, 418, 411, 405, 399, 392, 386, 380, 375, 369, 363, 358,
    352, 347, 341, 336, 331, 326, 321, 316, 311, 307, 302, 297, 293, 289, 284, 280,
    276, 271, 267, 263, 259, 255, 251, 248, 244, 240, 236, 233, 229, 226, 222, 219,
    215, 212, 209, 205, 202, 199, 196, 193, 190, 187, 184, 181, 178, 175, 172, 169,
    167, 164, 161, 159, 156, 153, 151, 148, 146, 143, 141, 138, 136, 134, 131, 129,
    127, 125, 122, 120, 118, 116, 114, 112, 110, 108, 106, 104, 102, 100, 98, 96,
    94, 92, 91, 89, 87, 85, 83, 82, 80, 78, 77, 75, 74, 72, 70, 69,
    67, 66, 64, 63, 62, 60, 59, 57, 56, 55, 53, 52, 51, 49, 48, 47,
    46, 45, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30,
    29, 28, 27, 26, 25, 24, 23, 23, 22, 21, 20, 20, 19, 18, 17, 17,
    16, 15, 15, 14, 13, 13, 12, 12, 11, 10, 10, 9, 9, 8, 8, 7,
    7, 7, 6, 6, 5, 5, 5, 4, 4, 4, 3, 3, 3, 2, 2, 2,
    2, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
};

static const uint16_t ym_power_table[256] = {
    8168, 8148, 8124, 8104, 8080, 8060, 8040, 8016, 7996, 7972, 7952, 7932, 7908, 7888, 7864, 7844,
    7824, 7804, 7780, 7760, 7740, 7720, 7696, 7676, 7656, 7636, 7616, 7592, 7572, 7552, 7532, 7512,
    7492, 7472, 7452, 7432, 7412, 7392, 7372, 7352, 7332, 7312, 7292, 7272, 7252, 7232, 7212, 7192,
    7176, 7156, 7136, 7116, 7096, 7076, 7060, 7040, 7020, 7000, 6984, 6964, 6944, 6928, 6908, 6888,
    6868, 6852, 6832, 6816, 6796, 6776, 6760, 6740, 6724, 6704, 6688, 6668, 6652, 6632, 6616, 6596,
    6580, 6560, 6544, 6524, 6508, 6492, 6472, 6456, 6436, 6420, 6404, 6384, 6368, 6352, 6336, 6316,
    6300, 6284, 6264, 6248, 6232, 6216, 6200, 6180, 6164, 6148, 6132, 6116, 6100, 6080, 6064, 6048,
    6032, 6016, 6000, 5984, 5968, 5952, 5936, 5920, 5904, 5888, 5872, 5856, 5840, 5824, 5808, 5792,
    5776, 5760, 5744, 5732, 5716, 5700, 5684, 5668, 5652, 5636, 5624, 5608, 5592, 5576, 5564, 5548,
    5532, 5516, 5504, 5488, 5472, 5456, 5444, 5428, 5412, 5400, 5384, 5368, 5356, 5340, 5328, 5312,
    5296, 5284, 5268, 5256, 5240, 5228, 5212, 5200, 5184, 5168, 5156, 5144, 5128, 5116, 5100, 5088,
    5072, 5060, 5044, 5032, 5020, 5004, 4992, 4976, 4964, 4952, 4936, 4924, 4912, 4896, 4884, 4872,
    4856, 4844, 4832, 4820, 4804, 4792, 4780, 4768, 4752, 4740, 4728, 4716, 4704, 4688, 4676, 4664,
    4652, 4640, 4628, 4616, 4600, 4588, 4576, 4564, 4552, 4540, 4528, 4516, 4504, 4492, 4480, 4468,
    4456, 4444, 4432, 4420, 4408, 4396, 4384, 4372, 4360, 4348, 4336, 4324, 4312, 4300, 4288, 4276,
    4264, 4256, 4244, 4232, 4220, 4208, 4196, 4184, 4176, 4164, 4152, 4140, 4128, 4120, 4108, 4096,
};

static const uint32_t ym_increment_table[64] = {
    0, 0, 269488144, 269488144, 269488144, 269488144, 286265616, 286265616,
    269488144, 269553680, 286265616, 286331152, 269488144, 269553680, 286265616, 286331152,
    269488144, 269553680, 286265616, 286331152, 269488144, 269553680, 286265616, 286331152,
    269488144, 269553680, 286265616, 286331152, 269488144, 269553680, 286265616, 286331152,
    269488144, 269553680, 286265616, 286331152, 269488144, 269553680, 286265616, 286331152,
    269488144, 269553680, 286265616, 286331152, 269488144, 269553680, 286265616, 286331152,
    286331153, 554770705, 555819297, 572596769, 572662306, 1109541410, 1111638594, 1145193538,
    1145324612, 2219082820, 2223277188, 2290387076, 2290649224, 2290649224, 2290649224, 2290649224,
};

static const uint8_t ym_detune_table[32][4] = {
    {0, 0, 1, 2},
    {0, 0, 1, 2},
    {0, 0, 1, 2},
    {0, 0, 1, 2},
    {0, 1, 2, 2},
    {0, 1, 2, 3},
    {0, 1, 2, 3},
    {0, 1, 2, 3},
    {0, 1, 2, 4},
    {0, 1, 3, 4},
    {0, 1, 3, 4},
    {0, 1, 3, 5},
    {0, 2, 4, 5},
    {0, 2, 4, 6},
    {0, 2, 4, 6},
    {0, 2, 5, 7},
    {0, 2, 5, 8},
    {0, 3, 6, 8},
    {0, 3, 6, 9},
    {0, 3, 7, 10},
    {0, 4, 8, 11},
    {0, 4, 8, 12},
    {0, 4, 9, 13},
    {0, 5, 10, 14},
    {0, 5, 11, 16},
    {0, 6, 12, 17},
    {0, 6, 13, 19},
    {0, 7, 14, 20},
    {0, 8, 16, 22},
    {0, 8, 16, 22},
    {0, 8, 16, 22},
    {0, 8, 16, 22},
};

static const uint32_t ym_phase_step[768] = {
    41568, 41600, 41632, 41664, 41696, 41728, 41760, 41792, 41856, 41888, 41920, 41952, 42016, 42048, 42080, 42112,
    42176, 42208, 42240, 42272, 42304, 42336, 42368, 42400, 42464, 42496, 42528, 42560, 42624, 42656, 42688, 42720,
    42784, 42816, 42848, 42880, 42912, 42944, 42976, 43008, 43072, 43104, 43136, 43168, 43232, 43264, 43296, 43328,
    43392, 43424, 43456, 43488, 43552, 43584, 43616, 43648, 43712, 43744, 43776, 43808, 43872, 43904, 43936, 43968,
    44032, 44064, 44096, 44128, 44192, 44224, 44256, 44288, 44352, 44384, 44416, 44448, 44512, 44544, 44576, 44608,
    44672, 44704, 44736, 44768, 44832, 44864, 44896, 44928, 44992, 45024, 45056, 45088, 45152, 45184, 45216, 45248,
    45312, 45344, 45376, 45408, 45472, 45504, 45536, 45568, 45632, 45664, 45728, 45760, 45792, 45824, 45888, 45920,
    45984, 46016, 46048, 46080, 46144, 46176, 46208, 46240, 46304, 46336, 46368, 46400, 46464, 46496, 46528, 46560,
    46656, 46688, 46720, 46752, 46816, 46848, 46880, 46912, 46976, 47008, 47072, 47104, 47136, 47168, 47232, 47264,
    47328, 47360, 47392, 47424, 47488, 47520, 47552, 47584, 47648, 47680, 47744, 47776, 47808, 47840, 47904, 47936,
    48032, 48064, 48096, 48128, 48192, 48224, 48288, 48320, 48384, 48416, 48448, 48480, 48544, 48576, 48640, 48672,
    48736, 48768, 48800, 48832, 48896, 48928, 48992, 49024, 49088, 49120, 49152, 49184, 49248, 49280, 49344, 49376,
    49440, 49472, 49504, 49536, 49600, 49632, 49696, 49728, 49792, 49824, 49856, 49888, 49952, 49984, 50048, 50080,
    50144, 50176, 50208, 50240, 50304, 50336, 50400, 50432, 50496, 50528, 50560, 50592, 50656, 50688, 50752, 50784,
    50880, 50912, 50944, 50976, 51040, 51072, 51136, 51168, 51232, 51264, 51328, 51360, 51424, 51456, 51488, 51520,
    51616, 51648, 51680, 51712, 51776, 51808, 51872, 51904, 51968, 52000, 52064, 52096, 52160, 52192, 52224, 52256,
    52384, 52416, 52448, 52480, 52544, 52576, 52640, 52672, 52736, 52768, 52832, 52864, 52928, 52960, 52992, 53024,
    53120, 53152, 53216, 53248, 53312, 53344, 53408, 53440, 53504, 53536, 53600, 53632, 53696, 53728, 53792, 53824,
    53920, 53952, 54016, 54048, 54112, 54144, 54208, 54240, 54304, 54336, 54400, 54432, 54496, 54528, 54592, 54624,
    54688, 54720, 54784, 54816, 54880, 54912, 54976, 55008, 55072, 55104, 55168, 55200, 55264, 55296, 55360, 55392,
    55488, 55520, 55584, 55616, 55680, 55712, 55776, 55808, 55872, 55936, 55968, 56032, 56064, 56128, 56160, 56224,
    56288, 56320, 56384, 56416, 56480, 56512, 56576, 56608, 56672, 56736, 56768, 56832, 56864, 56928, 56960, 57024,
    57120, 57152, 57216, 57248, 57312, 57376, 57408, 57472, 57536, 57568, 57632, 57664, 57728, 57792, 57824, 57888,
    57952, 57984, 58048, 58080, 58144, 58208, 58240, 58304, 58368, 58400, 58464, 58496, 58560, 58624, 58656, 58720,
    58784, 58816, 58880, 58912, 58976, 59040, 59072, 59136, 59200, 59232, 59296, 59328, 59392, 59456, 59488, 59552,
    59648, 59680, 59744, 59776, 59840, 59904, 59936, 60000, 60064, 60128, 60160, 60224, 60288, 60320, 60384, 60416,
    60512, 60544, 60608, 60640, 60704, 60768, 60800, 60864, 60928, 60992, 61024, 61088, 61152, 61184, 61248, 61280,
    61376, 61408, 61472, 61536, 61600, 61632, 61696, 61760, 61824, 61856, 61920, 61984, 62048, 62080, 62144, 62208,
    62272, 62304, 62368, 62432, 62496, 62528, 62592, 62656, 62720, 62752, 62816, 62880, 62944, 62976, 63040, 63104,
    63200, 63232, 63296, 63360, 63424, 63456, 63520, 63584, 63648, 63680, 63744, 63808, 63872, 63904, 63968, 64032,
    64096, 64128, 64192, 64256, 64320, 64352, 64416, 64480, 64544, 64608, 64672, 64704, 64768, 64832, 64896, 64928,
    65024, 65056, 65120, 65184, 65248, 65312, 65376, 65408, 65504, 65536, 65600, 65664, 65728, 65792, 65856, 65888,
    65984, 66016, 66080, 66144, 66208, 66272, 66336, 66368, 66464, 66496, 66560, 66624, 66688, 66752, 66816, 66848,
    66944, 66976, 67040, 67104, 67168, 67232, 67296, 67328, 67424, 67456, 67520, 67584, 67648, 67712, 67776, 67808,
    67904, 67936, 68000, 68064, 68128, 68192, 68256, 68288, 68384, 68448, 68512, 68544, 68640, 68672, 68736, 68800,
    68896, 68928, 68992, 69056, 69120, 69184, 69248, 69280, 69376, 69440, 69504, 69536, 69632, 69664, 69728, 69792,
    69920, 69952, 70016, 70080, 70144, 70208, 70272, 70304, 70400, 70464, 70528, 70560, 70656, 70688, 70752, 70816,
    70912, 70976, 71040, 71104, 71136, 71232, 71264, 71360, 71424, 71488, 71552, 71616, 71648, 71744, 71776, 71872,
    71968, 72032, 72096, 72160, 72192, 72288, 72320, 72416, 72480, 72544, 72608, 72672, 72704, 72800, 72832, 72928,
    72992, 73056, 73120, 73184, 73216, 73312, 73344, 73440, 73504, 73568, 73632, 73696, 73728, 73824, 73856, 73952,
    74080, 74144, 74208, 74272, 74304, 74400, 74432, 74528, 74592, 74656, 74720, 74784, 74816, 74912, 74944, 75040,
    75136, 75200, 75264, 75328, 75360, 75456, 75488, 75584, 75648, 75712, 75776, 75840, 75872, 75968, 76000, 76096,
    76224, 76288, 76352, 76416, 76448, 76544, 76576, 76672, 76736, 76800, 76864, 76928, 77024, 77120, 77152, 77248,
    77344, 77408, 77472, 77536, 77568, 77664, 77696, 77792, 77856, 77920, 77984, 78048, 78144, 78240, 78272, 78368,
    78464, 78528, 78592, 78656, 78688, 78784, 78816, 78912, 78976, 79040, 79104, 79168, 79264, 79360, 79392, 79488,
    79616, 79680, 79744, 79808, 79840, 79936, 79968, 80064, 80128, 80192, 80256, 80320, 80416, 80512, 80544, 80640,
    80768, 80832, 80896, 80960, 80992, 81088, 81120, 81216, 81280, 81344, 81408, 81472, 81568, 81664, 81696, 81792,
    81952, 82016, 82080, 82144, 82176, 82272, 82304, 82400, 82464, 82528, 82592, 82656, 82752, 82848, 82880, 82976,
};

static uint32_t ym_bits(uint32_t value, unsigned start, unsigned count)
{
    return (value >> start) & ((UINT32_C(1) << count) - 1u);
}

static int32_t ym_asr32(int32_t value, unsigned shift)
{
    if (shift == 0u)
        return value;
    if (value >= 0)
        return value / (int32_t)(UINT32_C(1) << shift);
    uint32_t magnitude = UINT32_C(0) - (uint32_t)value;
    uint32_t divisor = UINT32_C(1) << shift;
    return -(int32_t)((magnitude + divisor - 1u) / divisor);
}

static uint32_t ym_min_u32(uint32_t a, uint32_t b)
{
    return a < b ? a : b;
}

static int32_t ym_clamp_i32(int32_t value, int32_t minimum, int32_t maximum)
{
    if (value < minimum)
        return minimum;
    if (value > maximum)
        return maximum;
    return value;
}

static uint32_t ym_register_byte(const struct muhtari__ym2151 *ym, uint32_t address,
                                 unsigned start, unsigned count, uint32_t offset)
{
    return ym_bits(ym->registers[address + offset], start, count);
}

static uint32_t ym_channel_frequency(const struct muhtari__ym2151 *ym, uint32_t channel)
{
    uint32_t code = ym_register_byte(ym, 0x28u, 0u, 7u, channel);
    uint32_t fraction = ym_register_byte(ym, 0x30u, 2u, 6u, channel);
    return (code << 6) | fraction;
}

static uint32_t ym_effective_rate(uint32_t raw, uint32_t key_scale)
{
    if (raw == 0u)
        return 0u;
    return ym_min_u32(raw + key_scale, 63u);
}

static uint32_t ym_abs_sin_attenuation(uint32_t phase)
{
    uint32_t index = phase & (YM_WAVEFORM_LENGTH - 1u);
    if (ym_bits(index, 8u, 1u) != 0u)
        index = ~index;
    return ym_sin_table[index & 0xffu];
}

static uint32_t ym_attenuation_to_volume(uint32_t attenuation)
{
    return ym_power_table[attenuation & 0xffu] >> (attenuation >> 8);
}

static uint32_t ym_attenuation_increment(uint32_t rate, uint32_t index)
{
    return ym_bits(ym_increment_table[rate], 4u * index, 4u);
}

static int32_t ym_detune_adjustment(uint32_t detune, uint32_t keycode)
{
    int32_t result = ym_detune_table[keycode][detune & 3u];
    return ym_bits(detune, 2u, 1u) != 0u ? -result : result;
}

static uint32_t ym_key_code_phase_step(uint32_t block_frequency, int32_t delta)
{
    uint32_t block = ym_bits(block_frequency, 10u, 3u);
    uint32_t key = ym_bits(block_frequency, 6u, 4u) - ym_bits(block_frequency, 8u, 2u);
    int32_t effective = (int32_t)((key << 6) | ym_bits(block_frequency, 0u, 6u));
    effective += delta;
    if (effective < 0 || effective >= 768) {
        if (effective < 0) {
            effective += 768;
            if (block-- == 0u)
                return ym_phase_step[0] >> 7;
        } else {
            effective -= 768;
            if (effective >= 768) {
                block++;
                effective -= 768;
            }
            if (block++ >= 7u)
                return ym_phase_step[767];
        }
    }
    return ym_phase_step[(uint32_t)effective] >> (block ^ 7u);
}

static uint32_t ym_operator_phase_step(const struct muhtari__ym2151 *ym, uint32_t channel,
                                       uint32_t operator_offset,
                                       const struct muhtari__ym_cache *cache,
                                       int32_t lfo_raw_pm)
{
    static const int16_t detune2_delta[4] = {0, 384, 500, 608};
    int32_t delta = detune2_delta[ym_register_byte(ym, 0xc0u, 6u, 2u, operator_offset)];
    uint32_t sensitivity = ym_register_byte(ym, 0x38u, 4u, 3u, channel);
    if (sensitivity != 0u) {
        if (sensitivity < 6u)
            delta += ym_asr32(lfo_raw_pm, 6u - sensitivity);
        else
            delta += lfo_raw_pm * (int32_t)(UINT32_C(1) << (sensitivity - 5u));
    }
    uint32_t step = ym_key_code_phase_step(cache->block_freq, delta);
    step += (uint32_t)cache->detune;
    return (step * cache->multiple) >> 1;
}

static void ym_cache_operator(struct muhtari__ym2151 *ym, uint32_t channel,
                              uint32_t operator_index)
{
    struct muhtari__ym_operator *op = &ym->operators[operator_index];
    struct muhtari__ym_cache *cache = &op->cache;
    uint32_t offset = op->offset;
    uint32_t block_frequency = ym_channel_frequency(ym, channel);
    uint32_t keycode = ym_bits(block_frequency, 8u, 5u);
    cache->block_freq = block_frequency;
    cache->detune = ym_detune_adjustment(
        ym_register_byte(ym, 0x40u, 4u, 3u, offset), keycode);
    cache->multiple = ym_register_byte(ym, 0x40u, 0u, 4u, offset) * 2u;
    if (cache->multiple == 0u)
        cache->multiple = 1u;
    if (ym_register_byte(ym, 0x1au, 0u, 7u, 0u) == 0u ||
        ym_register_byte(ym, 0x38u, 4u, 3u, channel) == 0u)
        cache->phase_step = ym_operator_phase_step(ym, channel, offset, cache, 0);
    else
        cache->phase_step = YM_PHASE_STEP_DYNAMIC;
    cache->total_level = ym_register_byte(ym, 0x60u, 0u, 7u, offset) << 3;
    cache->eg_sustain = ym_register_byte(ym, 0xe0u, 4u, 4u, offset);
    cache->eg_sustain |= (cache->eg_sustain + 1u) & 0x10u;
    cache->eg_sustain <<= 5;
    uint32_t key_scale = keycode >> (ym_register_byte(ym, 0x80u, 6u, 2u, offset) ^ 3u);
    cache->eg_rate[YM_EG_ATTACK] = (uint8_t)ym_effective_rate(
        ym_register_byte(ym, 0x80u, 0u, 5u, offset) * 2u, key_scale);
    cache->eg_rate[YM_EG_DECAY] = (uint8_t)ym_effective_rate(
        ym_register_byte(ym, 0xa0u, 0u, 5u, offset) * 2u, key_scale);
    cache->eg_rate[YM_EG_SUSTAIN] = (uint8_t)ym_effective_rate(
        ym_register_byte(ym, 0xc0u, 0u, 5u, offset) * 2u, key_scale);
    cache->eg_rate[YM_EG_RELEASE] = (uint8_t)ym_effective_rate(
        ym_register_byte(ym, 0xe0u, 0u, 4u, offset) * 4u + 2u, key_scale);
}

static void ym_start_attack(struct muhtari__ym_operator *op)
{
    if (op->envelope_state == YM_EG_ATTACK)
        return;
    op->envelope_state = YM_EG_ATTACK;
    op->phase = 0u;
    if (op->cache.eg_rate[YM_EG_ATTACK] >= 62u)
        op->attenuation = 0u;
}

static void ym_start_release(struct muhtari__ym_operator *op)
{
    if (op->envelope_state >= YM_EG_RELEASE)
        return;
    op->envelope_state = YM_EG_RELEASE;
}

static void ym_operator_keyon(struct muhtari__ym_operator *op, uint32_t on, unsigned type)
{
    uint8_t mask = (uint8_t)(1u << type);
    op->keyon_live = (uint8_t)((op->keyon_live & (uint8_t)~mask) |
                               (uint8_t)((on & 1u) << type));
}

static void ym_operator_clock_keystate(struct muhtari__ym_operator *op, uint32_t state)
{
    assert(state <= 1u);
    if ((state ^ op->key_state) == 0u)
        return;
    op->key_state = (uint8_t)state;
    if (state != 0u)
        ym_start_attack(op);
    else
        ym_start_release(op);
}

static void ym_operator_prepare(struct muhtari__ym2151 *ym, uint32_t channel,
                                uint32_t operator_index)
{
    struct muhtari__ym_operator *op = &ym->operators[operator_index];
    ym_cache_operator(ym, channel, operator_index);
    ym_operator_clock_keystate(op, op->keyon_live != 0u ? 1u : 0u);
    op->keyon_live &= (uint8_t)~4u;
}

static void ym_operator_clock_envelope(struct muhtari__ym_operator *op, uint32_t env_counter)
{
    if (op->envelope_state == YM_EG_ATTACK && op->attenuation == 0u)
        op->envelope_state = YM_EG_DECAY;
    if (op->envelope_state == YM_EG_DECAY && op->attenuation >= op->cache.eg_sustain)
        op->envelope_state = YM_EG_SUSTAIN;

    uint32_t rate = op->cache.eg_rate[op->envelope_state];
    uint32_t rate_shift = rate >> 2;
    env_counter <<= rate_shift;
    if (ym_bits(env_counter, 0u, 11u) != 0u)
        return;
    uint32_t relevant = ym_bits(env_counter, rate_shift <= 11u ? 11u : rate_shift, 3u);
    uint32_t increment = ym_attenuation_increment(rate, relevant);
    if (op->envelope_state == YM_EG_ATTACK) {
        if (rate < 62u) {
            int32_t adjustment = ym_asr32((~(int32_t)op->attenuation) * (int32_t)increment, 4u);
            int32_t result = (int32_t)op->attenuation + adjustment;
            assert(result >= 0 && result <= 0x3ff);
            op->attenuation = (uint16_t)result;
        }
    } else {
        uint32_t result = (uint32_t)op->attenuation + increment;
        op->attenuation = (uint16_t)(result >= 0x400u ? 0x3ffu : result);
    }
}

static void ym_operator_clock(struct muhtari__ym2151 *ym, uint32_t operator_index,
                              uint32_t env_counter, int32_t lfo_raw_pm)
{
    struct muhtari__ym_operator *op = &ym->operators[operator_index];
    if (ym_bits(env_counter, 0u, 2u) == 0u)
        ym_operator_clock_envelope(op, env_counter >> 2);
    uint32_t step = op->cache.phase_step;
    if (step == YM_PHASE_STEP_DYNAMIC)
        step = ym_operator_phase_step(ym, op->channel, op->offset, &op->cache, lfo_raw_pm);
    op->phase += step;
}

static uint32_t ym_envelope_attenuation(const struct muhtari__ym2151 *ym,
                                       const struct muhtari__ym_operator *op,
                                       uint32_t am_offset)
{
    uint32_t result = op->attenuation >> op->cache.eg_shift;
    if (ym_register_byte(ym, 0xa0u, 7u, 1u, op->offset) != 0u)
        result += am_offset;
    result += op->cache.total_level;
    return result > 0x3ffu ? 0x3ffu : result;
}

static int32_t ym_operator_volume(const struct muhtari__ym2151 *ym,
                                  const struct muhtari__ym_operator *op, uint32_t phase,
                                  uint32_t am_offset)
{
    if (op->attenuation > YM_EG_QUIET)
        return 0;
    uint32_t wave = ym_abs_sin_attenuation(phase);
    uint32_t envelope = ym_envelope_attenuation(ym, op, am_offset) << 2;
    int32_t result = (int32_t)ym_attenuation_to_volume((wave & 0x7fffu) + envelope);
    return ym_bits(phase, 9u, 1u) != 0u ? -result : result;
}

static int32_t ym_operator_noise_volume(const struct muhtari__ym2151 *ym,
                                        const struct muhtari__ym_operator *op,
                                        uint32_t am_offset)
{
    int32_t result = (int32_t)((ym_envelope_attenuation(ym, op, am_offset) ^ 0x3ffu) << 1);
    return ym_bits(ym->noise_state, 0u, 1u) != 0u ? -result : result;
}

static int32_t ym_lfo_am_offset(const struct muhtari__ym2151 *ym, uint32_t channel)
{
    uint32_t sensitivity = ym_register_byte(ym, 0x38u, 0u, 2u, channel);
    return sensitivity == 0u ? 0 : (int32_t)((uint32_t)ym->lfo_am << (sensitivity - 1u));
}

static int32_t ym_clock_noise_and_lfo(struct muhtari__ym2151 *ym)
{
    uint32_t frequency = ym_register_byte(ym, 0x0fu, 0u, 5u, 0u) ^ 0x1fu;
    for (unsigned rep = 0; rep < 2u; rep++) {
        ym->noise_lfsr <<= 1;
        ym->noise_lfsr |= ym_bits(ym->noise_lfsr, 17u, 1u) ^
                          ym_bits(ym->noise_lfsr, 14u, 1u) ^ 1u;
        if (ym->noise_counter++ >= frequency) {
            ym->noise_counter = 0u;
            ym->noise_state = (uint8_t)ym_bits(ym->noise_lfsr, 17u, 1u);
        }
    }

    uint32_t rate = ym_register_byte(ym, 0x18u, 0u, 8u, 0u);
    ym->lfo_counter += (0x10u | ym_bits(rate, 0u, 4u)) << ym_bits(rate, 4u, 4u);
    if (ym_register_byte(ym, 0x01u, 1u, 1u, 0u) != 0u)
        ym->lfo_counter = 0u;
    uint32_t index = ym_bits(ym->lfo_counter, 22u, 8u);
    uint8_t noise = (uint8_t)ym_bits(ym->noise_lfsr, 17u, 8u);
    ym->lfo_noise[(index + 1u) & 0xffu] = (uint16_t)((uint16_t)noise * 0x0101u);

    uint32_t waveform = ym_register_byte(ym, 0x1bu, 0u, 2u, 0u);
    uint8_t am;
    uint8_t pm;
    if (waveform == 0u) {
        am = (uint8_t)(index ^ 0xffu);
        pm = (uint8_t)index;
    } else if (waveform == 1u) {
        am = ym_bits(index, 7u, 1u) != 0u ? 0u : 0xffu;
        pm = (uint8_t)(am ^ 0x80u);
    } else if (waveform == 2u) {
        am = (uint8_t)(ym_bits(index, 7u, 1u) != 0u ? (index << 1) : ((index ^ 0xffu) << 1));
        pm = ym_bits(index, 6u, 1u) != 0u ? am : (uint8_t)~am;
    } else {
        uint16_t packed = ym->lfo_noise[index];
        am = (uint8_t)packed;
        pm = (uint8_t)(packed >> 8);
    }
    uint32_t am_depth = ym_register_byte(ym, 0x19u, 0u, 7u, 0u);
    uint32_t pm_bits = ym_register_byte(ym, 0x1au, 0u, 7u, 0u);
    int32_t signed_pm = (pm & 0x80u) != 0u ? (int32_t)pm - 256 : pm;
    ym->lfo_am = (uint8_t)(((uint32_t)am * am_depth) >> 7);
    return ym_asr32(signed_pm * (int32_t)pm_bits, 7u);
}

static void ym_prepare(struct muhtari__ym2151 *ym)
{
    ym->active_channels = 0u;
    for (uint32_t channel = 0; channel < YM_CHANNELS; channel++) {
        static const uint8_t order[4] = {0u, 16u, 8u, 24u};
        int active = 0;
        for (unsigned slot = 0; slot < 4u; slot++) {
            uint32_t index = (uint32_t)order[slot] + channel;
            struct muhtari__ym_operator *op = &ym->operators[index];
            ym_operator_prepare(ym, channel, index);
            if (op->envelope_state != YM_EG_RELEASE || op->attenuation < YM_EG_QUIET)
                active = 1;
        }
        if (active != 0)
            ym->active_channels |= UINT32_C(1) << channel;
    }
    ym->modified_channels = 0u;
    ym->prepare_count = 0u;
}

static void ym_channel_clock(struct muhtari__ym2151 *ym, uint32_t channel,
                             uint32_t env_counter, int32_t lfo_raw_pm)
{
    struct muhtari__ym_channel *state = &ym->channels[channel];
    state->feedback[0] = state->feedback[1];
    state->feedback[1] = state->feedback_in;
    static const uint8_t order[4] = {0u, 16u, 8u, 24u};
    for (unsigned slot = 0; slot < 4u; slot++)
        ym_operator_clock(ym, (uint32_t)order[slot] + channel, env_counter, lfo_raw_pm);
}

static int32_t ym_round_half(int32_t value)
{
    return ym_asr32(value, 1u);
}

static void ym_channel_output(struct muhtari__ym2151 *ym, uint32_t channel, int32_t output[2])
{
    struct muhtari__ym_channel *state = &ym->channels[channel];
    static const uint8_t order[4] = {0u, 16u, 8u, 24u};
    uint32_t op_index[4];
    for (unsigned slot = 0; slot < 4u; slot++)
        op_index[slot] = (uint32_t)order[slot] + channel;
    uint32_t feedback = ym_register_byte(ym, 0x20u, 3u, 3u, channel);
    int32_t opmod = 0;
    if (feedback != 0u)
        opmod = ym_asr32((int32_t)state->feedback[0] + state->feedback[1], 10u - feedback);
    uint32_t am_offset = (uint32_t)ym_lfo_am_offset(ym, channel);
    int32_t op1 = ym_operator_volume(ym, &ym->operators[op_index[0]],
                                     (ym->operators[op_index[0]].phase >> 10) + (uint32_t)opmod,
                                     am_offset);
    state->feedback_in = (int16_t)op1;
    if (ym_register_byte(ym, 0x20u, 6u, 2u, channel) == 0u)
        return;

    static const uint16_t algorithms[8] = {
        0x0035u, 0x003au, 0x0064u, 0x0071u, 0x0131u, 0x0313u, 0x0301u, 0x0380u};
    uint32_t algorithm = ym_register_byte(ym, 0x20u, 0u, 3u, channel);
    uint16_t map = algorithms[algorithm];
    int32_t values[8] = {0};
    values[1] = op1;
    opmod = ym_round_half(values[ym_bits(map, 0u, 1u)]);
    values[2] = ym_operator_volume(ym, &ym->operators[op_index[1]],
                                   (ym->operators[op_index[1]].phase >> 10) + (uint32_t)opmod,
                                   am_offset);
    values[5] = values[1] + values[2];
    opmod = ym_round_half(values[ym_bits(map, 1u, 3u)]);
    values[3] = ym_operator_volume(ym, &ym->operators[op_index[2]],
                                   (ym->operators[op_index[2]].phase >> 10) + (uint32_t)opmod,
                                   am_offset);
    values[6] = values[1] + values[3];
    values[7] = values[2] + values[3];
    int32_t result;
    if (channel == 7u && ym_register_byte(ym, 0x0fu, 7u, 1u, 0u) != 0u) {
        result = ym_operator_noise_volume(ym, &ym->operators[op_index[3]], am_offset);
    } else {
        opmod = ym_round_half(values[ym_bits(map, 4u, 3u)]);
        result = ym_operator_volume(ym, &ym->operators[op_index[3]],
                                    (ym->operators[op_index[3]].phase >> 10) + (uint32_t)opmod,
                                    am_offset);
    }
    for (unsigned slot = 0; slot < 3u; slot++) {
        if (ym_bits(map, 7u + slot, 1u) != 0u)
            result = ym_clamp_i32(result + ym_asr32(values[slot + 1u], 0u), -32768, 32767);
    }
    if (ym_register_byte(ym, 0x20u, 6u, 1u, channel) != 0u)
        output[0] += result;
    if (ym_register_byte(ym, 0x20u, 7u, 1u, channel) != 0u)
        output[1] += result;
}

static uint32_t ym_count_leading_zeros(uint32_t value)
{
    uint32_t count = 0u;
    if (value == 0u)
        return 32u;
    for (uint32_t mask = UINT32_C(0x80000000); (value & mask) == 0u; mask >>= 1)
        count++;
    return count;
}

static int16_t ym_roundtrip_dac(int32_t value)
{
    if (value < INT16_MIN)
        return INT16_MIN;
    if (value > INT16_MAX)
        return INT16_MAX;
    uint32_t sign_mask = value < 0 ? UINT32_MAX : 0u;
    uint32_t scan = (uint32_t)value ^ sign_mask;
    int32_t exponent = 7 - (int32_t)ym_count_leading_zeros(scan << 17);
    if (exponent < 1)
        exponent = 1;
    exponent--;
    uint32_t divisor = UINT32_C(1) << (uint32_t)exponent;
    if (value < 0) {
        uint32_t magnitude = UINT32_C(0) - (uint32_t)value;
        return (int16_t)-(int32_t)(((magnitude + divisor - 1u) / divisor) * divisor);
    }
    return (int16_t)((value / (int32_t)divisor) * (int32_t)divisor);
}

static void ym_check_irq(struct muhtari__ym2151 *ym)
{
    ym->irq_state = (uint8_t)((ym->status & ym->irq_mask) != 0u);
}

static void ym_set_reset_status(struct muhtari__ym2151 *ym, uint8_t set, uint8_t reset)
{
    ym->status = (uint8_t)((ym->status | set) & (uint8_t)~(reset | 0x80u));
    ym_check_irq(ym);
}

static void ym_update_timer(struct muhtari__ym2151 *ym, unsigned timer, int32_t delta)
{
    uint32_t mode = ym->registers[0x14u];
    uint32_t load = timer == 0u ? mode & 1u : (mode >> 1) & 1u;
    if (load != 0u && ym->timer_running[timer] == 0u) {
        uint32_t period = timer == 0u
                              ? 1024u - ((uint32_t)ym->registers[0x10u] << 2) -
                                    (uint32_t)(ym->registers[0x11u] & 3u)
                              : 16u * (256u - (uint32_t)ym->registers[0x12u]);
        int64_t adjusted = (int64_t)period + delta;
        assert(adjusted > 0 && adjusted <= INT32_MAX / YM_CLOCKS_PER_SAMPLE);
        uint64_t duration = (uint64_t)adjusted * YM_CLOCKS_PER_SAMPLE;
        ym->timer_deadline[timer] =
            duration > UINT64_MAX - ym->clocks ? UINT64_MAX : ym->clocks + duration;
        ym->timer_running[timer] = 1u;
    } else if (load == 0u) {
        ym->timer_deadline[timer] = 0u;
        ym->timer_running[timer] = 0u;
    }
}

static void ym_mode_write(struct muhtari__ym2151 *ym, uint8_t data)
{
    ym->modified_channels = YM_ALL_CHANNELS;
    ym->registers[0x14u] = data;
    uint8_t reset_mask = 0u;
    if ((data & 0x20u) != 0u)
        reset_mask |= 0x02u;
    if ((data & 0x10u) != 0u)
        reset_mask |= 0x01u;
    ym_set_reset_status(ym, 0u, reset_mask);
    ym_update_timer(ym, 1u, -(int32_t)(ym->total_clocks & 15u));
    ym_update_timer(ym, 0u, 0);
}

static void ym_timer_expired(struct muhtari__ym2151 *ym, unsigned timer)
{
    uint8_t mode = ym->registers[0x14u];
    if (timer == 0u && (mode & 0x04u) != 0u)
        ym_set_reset_status(ym, 0x01u, 0u);
    else if (timer == 1u && (mode & 0x08u) != 0u)
        ym_set_reset_status(ym, 0x02u, 0u);
    if (timer == 0u && (mode & 0x80u) != 0u) {
        for (unsigned channel = 0; channel < YM_CHANNELS; channel++) {
            static const uint8_t order[4] = {0u, 16u, 8u, 24u};
            for (unsigned slot = 0; slot < 4u; slot++)
                ym_operator_keyon(&ym->operators[(uint32_t)order[slot] + channel], 1u, 2u);
        }
        ym->modified_channels = YM_ALL_CHANNELS;
    }
    ym->timer_running[timer] = 0u;
    ym_update_timer(ym, timer, 0);
}

static void ym_engine_sample(muhtari *m, uint64_t tick)
{
    struct muhtari__ym2151 *ym = &m->ym;
    ym->total_clocks++;
    if (ym->modified_channels != 0u || ++ym->prepare_count >= 4096u)
        ym_prepare(ym);

    if (ym_bits(++ym->env_counter, 0u, 2u) == 3u)
        ym->env_counter++;
    int32_t lfo_raw_pm = ym_clock_noise_and_lfo(ym);
    for (uint32_t channel = 0; channel < YM_CHANNELS; channel++)
        ym_channel_clock(ym, channel, ym->env_counter, lfo_raw_pm);

    int32_t output[YM_OUTPUTS] = {0, 0};
    for (uint32_t channel = 0; channel < YM_CHANNELS; channel++) {
        if ((ym->active_channels & (UINT32_C(1) << channel)) != 0u)
            ym_channel_output(ym, channel, output);
    }
    ym->output[0] = ym_roundtrip_dac(output[0]);
    ym->output[1] = ym_roundtrip_dac(output[1]);
    /* Preserve the signed DAC word as an explicit two's-complement bit
       pattern for the private observer; the mixer decodes it separately. */
    muhtari__synth_raw(m, tick, 10u, (uint16_t)ym->output[0]);
    muhtari__synth_level(m, tick, 10u, (uint16_t)ym->output[0]);
    muhtari__synth_raw(m, tick, 11u, (uint16_t)ym->output[1]);
    muhtari__synth_level(m, tick, 11u, (uint16_t)ym->output[1]);
}

static void ym_clock_one(muhtari *m, uint64_t tick)
{
    struct muhtari__ym2151 *ym = &m->ym;
    if (ym->clocks == UINT64_MAX)
        return;
    ym->clocks++;
    for (unsigned timer = 0; timer < 2u; timer++) {
        if (ym->timer_running[timer] != 0u && ym->timer_deadline[timer] <= ym->clocks)
            ym_timer_expired(ym, timer);
    }
    ym->sample_divider++;
    if (ym->sample_divider >= YM_CLOCKS_PER_SAMPLE) {
        ym->sample_divider = 0u;
        ym_engine_sample(m, tick);
    }
}

static uint8_t ym_status(const struct muhtari__ym2151 *ym)
{
    uint8_t status = (uint8_t)(ym->status & 0x7fu);
    if (ym->clocks < ym->busy_end)
        status |= 0x80u;
    return status;
}

void muhtari__ym2151_reset(muhtari *m)
{
    struct muhtari__ym2151 *ym = &m->ym;
    uint8_t initialized = ym->initialized;
    uint8_t address = ym->address;
    uint8_t clock_remainder = ym->clock_remainder;
    uint16_t sample_divider = ym->sample_divider;
    uint64_t clocks = ym->clocks;
    uint32_t lfo_counter = ym->lfo_counter;
    uint32_t noise_lfsr = ym->noise_lfsr;
    uint8_t noise_counter = ym->noise_counter;
    uint8_t noise_state = ym->noise_state;
    uint8_t noise_lfo = ym->noise_lfo;
    uint8_t lfo_am = ym->lfo_am;
    uint16_t lfo_noise[256];
    if (initialized != 0u)
        memcpy(lfo_noise, ym->lfo_noise, sizeof lfo_noise);
    else
        memset(lfo_noise, 0, sizeof lfo_noise);
    memset(ym, 0, sizeof *ym);
    ym->initialized = 1u;
    ym->address = address;
    ym->clock_remainder = clock_remainder;
    ym->sample_divider = sample_divider;
    ym->clocks = clocks;
    ym->last_tick = m->bus.ticks;
    ym->lfo_counter = lfo_counter;
    ym->noise_lfsr = initialized != 0u ? noise_lfsr : 1u;
    ym->noise_counter = noise_counter;
    ym->noise_state = noise_state;
    ym->noise_lfo = noise_lfo;
    ym->lfo_am = lfo_am;
    memcpy(ym->lfo_noise, lfo_noise, sizeof lfo_noise);
    ym->clock_prescale = 2u;
    ym->irq_mask = 3u;
    ym->active_channels = YM_ALL_CHANNELS;
    ym->modified_channels = YM_ALL_CHANNELS;
    for (uint32_t index = 0; index < YM_OPERATORS; index++) {
        ym->operators[index].offset = index;
        ym->operators[index].channel = index & 7u;
        ym->operators[index].attenuation = 0x3ffu;
        ym->operators[index].envelope_state = YM_EG_RELEASE;
    }
    for (uint32_t channel = 0; channel < YM_CHANNELS; channel++)
        ym->registers[0x20u + channel] = 0xc0u;
}

void muhtari__ym2151_catch_up(muhtari *m, uint64_t tick)
{
    struct muhtari__ym2151 *ym = &m->ym;
    if (tick <= ym->last_tick)
        return;
    uint64_t elapsed = tick - ym->last_tick;
    uint8_t phase = ym->clock_remainder;
    uint64_t count = elapsed / 2u;
    if (phase != 0u && (elapsed & 1u) != 0u)
        count++;
    if (count != 0u) {
        uint64_t first_clock = ym->last_tick + (phase != 0u ? 1u : 2u);
        for (uint64_t index = 0u; index < count; index++)
            ym_clock_one(m, first_clock + index * 2u);
    }
    ym->clock_remainder = (uint8_t)((phase + (elapsed & 1u)) & 1u);
    ym->last_tick = tick;
}

/* The bus uses this absolute tick to stop held-cycle batches at timer edges.
   Clock phase is retained across odd MARIA ticks; compute from a distance so
   the final legal timer period cannot wrap an absolute deadline. */
uint64_t muhtari__ym2151_next_timer_tick(const muhtari *m)
{
    const struct muhtari__ym2151 *ym = &m->ym;
    if (m->synth.ym_installed == 0u)
        return 0u;

    uint64_t nearest = 0u;
    for (unsigned timer = 0u; timer < 2u; timer++) {
        if (ym->timer_running[timer] == 0u || ym->timer_deadline[timer] <= ym->clocks)
            continue;
        uint64_t clocks = ym->timer_deadline[timer] - ym->clocks;
        uint64_t first = ym->clock_remainder != 0u ? 1u : 2u;
        uint64_t tail = clocks - 1u;
        if (tail > (UINT64_MAX - first) / 2u)
            continue;
        uint64_t delta = first + tail * 2u;
        if (delta > UINT64_MAX - ym->last_tick)
            continue;
        uint64_t tick = ym->last_tick + delta;
        if (nearest == 0u || tick < nearest)
            nearest = tick;
    }
    return nearest;
}

uint8_t muhtari__ym2151_read(muhtari *m, uint16_t addr, uint64_t tick)
{
    muhtari__ym2151_catch_up(m, tick);
    return (addr & 1u) != 0u ? ym_status(&m->ym) : 0xffu;
}

uint8_t muhtari__ym2151_peek(const muhtari *m, uint16_t addr)
{
    return (addr & 1u) != 0u ? ym_status(&m->ym) : 0xffu;
}

void muhtari__ym2151_write(muhtari *m, uint16_t addr, uint8_t value, uint64_t tick)
{
    struct muhtari__ym2151 *ym = &m->ym;
    muhtari__ym2151_catch_up(m, tick);
    if ((addr & 1u) == 0u) {
        ym->address = value;
        return;
    }
    uint32_t index = ym->address;
    if (index == 0x14u) {
        ym_mode_write(ym, value);
    } else {
        ym->modified_channels = YM_ALL_CHANNELS;
        if (index == 0x19u)
            ym->registers[index + ym_bits(value, 7u, 1u)] = value;
        else if (index != 0x1au)
            ym->registers[index] = value;
        if (index == 0x08u) {
            uint32_t channel = ym_bits(value, 0u, 3u);
            uint32_t mask = ym_bits(value, 3u, 4u);
            static const uint8_t order[4] = {0u, 16u, 8u, 24u};
            for (unsigned slot = 0; slot < 4u; slot++)
                ym_operator_keyon(&ym->operators[(uint32_t)order[slot] + channel],
                                  ym_bits(mask, slot, 1u), 0u);
        }
    }
    ym->busy_end = YM_BUSY_CLOCKS > UINT64_MAX - ym->clocks
                       ? UINT64_MAX
                       : ym->clocks + YM_BUSY_CLOCKS;
}

int muhtari__ym2151_irq(const muhtari *m)
{
    return m->ym.irq_state != 0u;
}
