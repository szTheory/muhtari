---
phase: 04-sound
plan: "11"
subsystem: audio
requires:
  - phase: 04-10
    provides: Completed predecessor sound slice
provides:
  - Real cartridge IRQ acceptance and private raw DAC observation preserve PCM and machine state
affects: [sound-integration]
tech-stack:
  added: []
  patterns: [private-observation, portable-contracts]
key-files:
  created: []
  modified:
    - docs/hardware/audio.md
    - src/frame.c
    - src/instance.c
    - src/internal.h
    - src/pokey.c
    - src/synth.c
    - src/tia.c
    - tests/audio/test_contracts.c
    - tests/audio/test_irq.c
    - tests/audio/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
requirements-completed:
  - SND-02
  - SND-03
coverage:
  - id: D1
    description: Real cartridge IRQ acceptance and private raw DAC observation preserve PCM and machine state
    requirement: SND-02
    verification:
      - kind: integration
        ref: sally.irq
        status: pass
    human_judgment: false
  - id: D2
    description: Portable regression selection audio.frames
    requirement: SND-03
    verification:
      - kind: integration
        ref: audio.frames
        status: pass
    human_judgment: false
duration: ongoing full-phase execution
completed: 2026-10-21
head_after: 328dbaf0301fdfb3856d62583953c69e730378fb
---

# Phase 04 Plan 11: Focused IRQ and private observation Summary

**Real cartridge IRQ acceptance and private raw DAC observation preserve PCM and machine state.**

## Accomplishments

- Real cartridge IRQ acceptance and private raw DAC observation preserve PCM and machine state.
- Automated selections: sally.irq,audio.frames.

## Automated Checks

- cmake --workflow --preset ci — exit 0.
- cmake --workflow --preset hygiene — exit 0.
- scripts/hygiene.sh --tree — exit 0.
- Assertion-enabled focused tests — exit 0.
- No hosted run URL in this local execution; cross-platform and merge evidence belong to the final lane audit and ship.

## Decisions and limits

API3 and ABI1 remain unchanged; behavior revision2, exact-q23-16x64-dc20,
16 taps, M18360, 85:342 current weights and Q12 gain4096 remain fixed.
The functional timing model retains its documented hardware confidence limits.

## Deviations

Reuse predecessor coverage where already shipped; add the missing contracts without duplicating implementation.

## Execution Details

Verified through the shipped archive and existing CI. Continue within this invocation to the next ready wave.

Observer tracer RED: build succeeded; ctest --test-dir build/ci -R ^audio.frames$ --output-on-failure --no-tests=error exited8 with missing callback assertions. GREEN passed in Release and Debug. IRQ task reused wave10 coverage and expanded the focused selection with its 48 real cartridge cases. Registration allocates bounded private storage only while enabled; frame execution allocates nothing.

## Task Commits

| Commit | Change |
| --- | --- |
| a6abe5fb32 | test(04-11): publish real cartridge IRQ conformance |
| d6db6749e7 | test(04-11): require a raw observer callback without changing PCM |
| 328dbaf030 | feat(04-11): observe raw DAC changes on half-open frame horizons |

## Self-Check: PASSED

Named artifacts exist, task commits are on this branch, and reported selections passed.
