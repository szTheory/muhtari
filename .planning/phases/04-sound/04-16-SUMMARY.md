---
phase: 04-sound
plan: "16"
subsystem: audio
requires:
  - phase: 04-15
    provides: Completed predecessor sound slice
provides:
  - Legal dual-chip and TIA stress proves bounded allocation-free frames and measured exact-only whole-core cost
affects: [sound-integration]
tech-stack:
  added: []
  patterns: [private-observation, portable-contracts]
key-files:
  created:
    - tests/audio/test_stress.c
  modified:
    - docs/testing.md
    - tests/audio/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
requirements-completed:
  - SND-02
  - SND-03
  - SND-05
coverage:
  - id: D1
    description: Legal dual-chip and TIA stress proves bounded allocation-free frames and measured exact-only whole-core cost
    requirement: SND-02
    verification:
      - kind: integration
        ref: audio.stress
        status: pass
    human_judgment: false
  - id: D2
    description: Portable regression selection fuzz.regress.run
    requirement: SND-02
    verification:
      - kind: integration
        ref: fuzz.regress.run
        status: pass
    human_judgment: false
duration: ongoing full-phase execution
completed: 2026-10-21
head_after: 0949c041de253cce6b3dcbb6bb436a566a2ba7d5
---

# Phase 04 Plan 16: Hostile legal sound bounds and exact cost Summary

**Legal dual-chip and TIA stress proves bounded allocation-free frames and measured exact-only whole-core cost.**

## Accomplishments

- Legal dual-chip and TIA stress proves bounded allocation-free frames and measured exact-only whole-core cost.
- Automated selections: audio.stress,fuzz.regress.run.

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

Four deterministic workloads cover direct fast channels1/3, legal base channels2/4, joins, HP, slow TIA writes/WSYNC, real timer IRQ handlers, additional held deadline crossing and JAM. Normal, discarded and uneven manual drains match. No allocations occur after observer registration/load. Zero clips; maximum kernel additions27, below150. Full frame carry≤3 in recurring cases; benchmark carry1, below139. First test failure was a test-bound error: a partial149-tick drain retains unconsumed current-frame PCM, so its high-water uses the940-cell FIFO limit while complete drains retain139. Current pcm_count after the final drain stays≤139. No runtime bound was weakened.

Benchmark command: build/ci/tests/muhtari-test-audio-stress --bench 600 — exit0. Apple clang21.0.0, arm64-apple-darwin25.6.0, Release. Dual-chip exact1,221,856,000ns; TIA baseline59,274,000ns; milli ratio20613. Each completed600 frames and479591 samples; dual raw26,636,321 and baseline274,741. Exact PCM check6145531612794415913. Peak11882 versus baseline20729; no clipping. This includes whole-core/chip/interrupt and observation work; it is not a DSP-only comparison or calibrated low-power measurement. No applicable low-power trigger was established and no quality switch/fallback was added. Exact identity/current weights/gain/M remain unchanged.

Full CI605 checks plus6 archive checks, hygiene24 and Debug stress/fuzz selections passed.

## Task Commits

| Commit | Change |
| --- | --- |
| 0949c041de | test(04-16): bound hostile legal audio and measure exact synthesis |

## Final lifetime reconciliation

Review WR-01 is closed by11482da (RED) and8c933e1 (GREEN). First observer
registration now precedes load; a late first registration is rejected without
allocation. Every production/reference caller follows this order. Video bytes
also match across observation on/off for all eight sample runs. Fresh bounded
regression CI605+6, sanitizer593, GCC no-FP13 and hygiene24 pass after the fix;
raw/PCM pins and synthesis identity remain unchanged. One review fix round used,
no gap round.

## Self-Check: PASSED

Named artifacts exist, task commits are on this branch, and reported selections passed.
