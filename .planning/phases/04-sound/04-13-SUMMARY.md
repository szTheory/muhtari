---
phase: 04-sound
plan: "13"
subsystem: audio
requires:
  - phase: 04-12
    provides: Completed predecessor sound slice
provides:
  - Canonical separate raw and mono PCM streaming hashes with completed-run counts and deterministic signal and failure suppression
affects: [sound-integration]
tech-stack:
  added: []
  patterns: [private-observation, portable-contracts]
key-files:
  created:
    - tests/runner/test_audio.c
  modified:
    - docs/runner.md
    - runner/main.c
    - tests/audio/test_contracts.c
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - tests/runner/tests.cmake
requirements-completed:
  - SND-03
coverage:
  - id: D1
    description: Canonical separate raw and mono PCM streaming hashes with completed-run counts and deterministic signal and failure suppression
    requirement: SND-03
    verification:
      - kind: integration
        ref: runner.audio.contract
        status: pass
    human_judgment: false
  - id: D2
    description: Portable regression selection runner.audio.stop_failure
    requirement: SND-03
    verification:
      - kind: integration
        ref: runner.audio.stop_failure
        status: pass
    human_judgment: false
duration: ongoing full-phase execution
completed: 2026-10-21
head_after: 3e4a0f7f095bef4260937a19049f30bc2cbfb33f
---

# Phase 04 Plan 13: Canonical completed-run audio hashes Summary

**Canonical separate raw and mono PCM streaming hashes with completed-run counts and deterministic signal and failure suppression.**

## Accomplishments

- Canonical separate raw and mono PCM streaming hashes with completed-run counts and deterministic signal and failure suppression.
- Automated selections: runner.audio.contract,runner.audio.stop_failure.

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

RED: runner.audio.contract exited8 on unknown --hash-audio. The first parity check exposed a silent fixture; soundtest requires the generic fire input on INPT1. Both ProLine buttons are now held by a deterministic60-frame movie and independently by the core reference. The test rejects the empty raw digest before comparing all seven fields. Byte-order controls use explicit >32-bit ticks and negative int16 bytes. Actual runner source and signal handler cover stops at0/1/3 completed frames, partial-frame refusal and dump errors, with no success audio labels on errors. Release CI600 checks plus6 archive tests and hygiene24 passed.

## Task Commits

| Commit | Change |
| --- | --- |
| 0c446d924c | test(04-13): require canonical runner audio hashes |
| 3e4a0f7f09 | feat(04-13): hash completed raw transitions and mono PCM canonically |

## Self-Check: PASSED

Named artifacts exist, task commits are on this branch, and reported selections passed.
