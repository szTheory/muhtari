/* Exact outputs for the named OPM corpus. Hashes are filled only after the
   receipt-bound C-vs-pinned-object oracle has compared every sample. */
#ifndef MUHTARI_TEST_YM_VECTORS_H
#define MUHTARI_TEST_YM_VECTORS_H

#include <stdint.h>

#define YM_CASE_STEP UINT8_C(0xff)
#define YM_CASE_ALL UINT8_C(0xfe)
#define YM_ACTION_COUNT(actions) ((uint16_t)(sizeof(actions) / sizeof((actions)[0])))

struct ym_case_action {
    uint8_t chip;
    uint8_t reg;
    uint8_t value;
    uint16_t samples;
};

struct ym_case_vector {
    const char *name;
    const struct ym_case_action *actions;
    uint16_t action_count;
    uint8_t chip_count;
    uint32_t sample_count;
    uint64_t hash;
};

static const struct ym_case_action ym_common[] = {
    {YM_CASE_ALL, 0x20u, 0xc7u, 0u}, {YM_CASE_ALL, 0x28u, 0x4au, 0u},
    {YM_CASE_ALL, 0x30u, 0x00u, 0u}, {YM_CASE_ALL, 0x38u, 0x73u, 0u},
    {YM_CASE_ALL, 0x18u, 0xffu, 0u}, {YM_CASE_ALL, 0x19u, 0x7fu, 0u},
    {YM_CASE_ALL, 0x19u, 0xffu, 0u},
    {YM_CASE_ALL, 0x1bu, 0x00u, 0u}, {YM_CASE_ALL, 0x40u, 0x01u, 0u},
    {YM_CASE_ALL, 0x48u, 0x01u, 0u}, {YM_CASE_ALL, 0x50u, 0x01u, 0u},
    {YM_CASE_ALL, 0x58u, 0x01u, 0u}, {YM_CASE_ALL, 0x60u, 0x00u, 0u},
    {YM_CASE_ALL, 0x68u, 0x00u, 0u}, {YM_CASE_ALL, 0x70u, 0x00u, 0u},
    {YM_CASE_ALL, 0x78u, 0x00u, 0u}, {YM_CASE_ALL, 0x80u, 0x1fu, 0u},
    {YM_CASE_ALL, 0x88u, 0x1fu, 0u}, {YM_CASE_ALL, 0x90u, 0x1fu, 0u},
    {YM_CASE_ALL, 0x98u, 0x1fu, 0u}, {YM_CASE_ALL, 0xa0u, 0x00u, 0u},
    {YM_CASE_ALL, 0xa8u, 0x00u, 0u}, {YM_CASE_ALL, 0xb0u, 0x00u, 0u},
    {YM_CASE_ALL, 0xb8u, 0x00u, 0u}, {YM_CASE_ALL, 0xc0u, 0x00u, 0u},
    {YM_CASE_ALL, 0xc8u, 0x00u, 0u}, {YM_CASE_ALL, 0xd0u, 0x00u, 0u},
    {YM_CASE_ALL, 0xd8u, 0x00u, 0u}, {YM_CASE_ALL, 0xe0u, 0x0fu, 0u},
    {YM_CASE_ALL, 0xe8u, 0x0fu, 0u}, {YM_CASE_ALL, 0xf0u, 0x0fu, 0u},
    {YM_CASE_ALL, 0xf8u, 0x0fu, 0u}, {YM_CASE_ALL, 0x08u, 0x78u, 0u},
};

static const struct ym_case_action ym_algorithms[] = {
    {0u, 0x20u, 0xc0u, 256u}, {0u, 0x20u, 0xc1u, 256u},
    {0u, 0x20u, 0xc2u, 256u}, {0u, 0x20u, 0xc3u, 256u},
    {0u, 0x20u, 0xc4u, 256u}, {0u, 0x20u, 0xc5u, 256u},
    {0u, 0x20u, 0xc6u, 256u}, {0u, 0x20u, 0xc7u, 256u},
};

static const struct ym_case_action ym_operator_positions[] = {
    {0u, 0x08u, 0x08u, 256u}, {0u, 0x08u, 0x10u, 256u},
    {0u, 0x08u, 0x20u, 256u}, {0u, 0x08u, 0x40u, 256u},
};

static const struct ym_case_action ym_envelope[] = {
    {YM_CASE_STEP, YM_CASE_STEP, 0u, 512u}, {0u, 0x08u, 0x00u, 0u},
    {YM_CASE_STEP, YM_CASE_STEP, 0u, 512u},
};

static const struct ym_case_action ym_feedback[] = {
    {0u, 0x20u, 0xc7u, 128u}, {0u, 0x20u, 0xcfu, 128u},
    {0u, 0x20u, 0xd7u, 128u}, {0u, 0x20u, 0xdfu, 128u},
    {0u, 0x20u, 0xe7u, 128u}, {0u, 0x20u, 0xefu, 128u},
    {0u, 0x20u, 0xf7u, 128u}, {0u, 0x20u, 0xffu, 128u},
};

static const struct ym_case_action ym_noise[] = {
    {0u, 0x27u, 0xc7u, 0u}, {0u, 0x2fu, 0x4au, 0u},
    {0u, 0x9fu, 0x1fu, 0u}, {0u, 0x0fu, 0x9fu, 0u},
    {0u, 0x08u, 0x7fu, 0u}, {YM_CASE_STEP, YM_CASE_STEP, 0u, 512u},
};

static const struct ym_case_action ym_lfo[] = {
    {0u, 0x1bu, 0x00u, 128u}, {0u, 0x1bu, 0x01u, 128u},
    {0u, 0x1bu, 0x02u, 128u}, {0u, 0x1bu, 0x03u, 128u},
};

static const struct ym_case_action ym_panning[] = {
    {0u, 0x20u, 0x07u, 128u}, {0u, 0x20u, 0x47u, 128u},
    {0u, 0x20u, 0x87u, 128u}, {0u, 0x20u, 0xc7u, 128u},
};

static const struct ym_case_action ym_phase_wrap[] = {
    {0u, 0x28u, 0x7fu, 0u}, {0u, 0x30u, 0xfcu, 0u},
    {YM_CASE_STEP, YM_CASE_STEP, 0u, 2048u},
};

static const struct ym_case_action ym_repeated_writes[] = {
    {0u, 0x60u, 0x00u, 1u}, {0u, 0x60u, 0x01u, 1u},
    {0u, 0x60u, 0x02u, 1u}, {0u, 0x60u, 0x03u, 1u},
    {0u, 0x60u, 0x04u, 1u}, {0u, 0x60u, 0x05u, 1u},
    {0u, 0x60u, 0x06u, 1u}, {0u, 0x60u, 0x07u, 1u},
    {0u, 0x60u, 0x08u, 1u}, {0u, 0x60u, 0x09u, 1u},
    {0u, 0x60u, 0x0au, 1u}, {0u, 0x60u, 0x0bu, 1u},
    {0u, 0x60u, 0x0cu, 1u}, {0u, 0x60u, 0x0du, 1u},
    {0u, 0x60u, 0x0eu, 1u}, {0u, 0x60u, 0x0fu, 1u},
    {YM_CASE_STEP, YM_CASE_STEP, 0u, 496u},
};

static const struct ym_case_action ym_interleaved[] = {
    {0u, 0x60u, 0x00u, 32u}, {1u, 0x68u, 0x02u, 32u},
    {0u, 0x60u, 0x04u, 32u}, {1u, 0x68u, 0x06u, 32u},
    {0u, 0x60u, 0x08u, 32u}, {1u, 0x68u, 0x0au, 32u},
    {0u, 0x60u, 0x0cu, 32u}, {1u, 0x68u, 0x0eu, 32u},
    {0u, 0x60u, 0x10u, 32u}, {1u, 0x68u, 0x12u, 32u},
    {0u, 0x60u, 0x14u, 32u}, {1u, 0x68u, 0x16u, 32u},
    {0u, 0x60u, 0x18u, 32u}, {1u, 0x68u, 0x1au, 32u},
    {0u, 0x60u, 0x1cu, 32u}, {1u, 0x68u, 0x1eu, 32u},
};

static const struct ym_case_vector ym_cases[] = {
    {"algorithms", ym_algorithms, YM_ACTION_COUNT(ym_algorithms), 1u, 2048u, UINT64_C(0xf26c9e61240dbf1d)},
    {"operator_positions", ym_operator_positions, YM_ACTION_COUNT(ym_operator_positions), 1u, 1024u, UINT64_C(0x3513b26ae111f495)},
    {"key_envelope", ym_envelope, YM_ACTION_COUNT(ym_envelope), 1u, 1024u, UINT64_C(0xc6dd6f03c1955025)},
    {"feedback", ym_feedback, YM_ACTION_COUNT(ym_feedback), 1u, 1024u, UINT64_C(0x2e169c902c535751)},
    {"noise", ym_noise, YM_ACTION_COUNT(ym_noise), 1u, 512u, UINT64_C(0x52fe1af473cba8f9)},
    {"lfo", ym_lfo, YM_ACTION_COUNT(ym_lfo), 1u, 512u, UINT64_C(0x32881a206ef46b1d)},
    {"panning", ym_panning, YM_ACTION_COUNT(ym_panning), 1u, 512u, UINT64_C(0x05a46ec2de5b93fd)},
    {"phase_wrap", ym_phase_wrap, YM_ACTION_COUNT(ym_phase_wrap), 1u, 2048u, UINT64_C(0x398d0234008bac95)},
    {"repeated_writes", ym_repeated_writes, YM_ACTION_COUNT(ym_repeated_writes), 1u, 512u, UINT64_C(0xa56232bb4246a309)},
    {"interleaved_two", ym_interleaved, YM_ACTION_COUNT(ym_interleaved), 2u, 512u, UINT64_C(0x3c55a3c82fd3bb09)},
};

#define YM_CASE_COUNT (sizeof ym_cases / sizeof ym_cases[0])

#endif
