---
phase: 04-sound
plan: "14"
subsystem: audio
requires:
  - phase: 04-13
    provides: Completed predecessor sound slice
provides:
  - Actual module sample parity across callback modes and reset and reload plus audio-aware gameplay with a muted control
affects: [sound-integration]
tech-stack:
  added: []
  patterns: [private-observation, portable-contracts]
key-files:
  created: []
  modified: []
requirements-completed:
  - SND-04
  - SND-03
coverage:
  - id: D1
    description: Actual module sample parity across callback modes and reset and reload plus audio-aware gameplay with a muted control
    requirement: SND-04
    verification:
      - kind: integration
        ref: libretro.host.audio
        status: pass
    human_judgment: false
  - id: D2
    description: Portable regression selection runner.boot_to_play
    requirement: SND-04
    verification:
      - kind: integration
        ref: runner.boot_to_play
        status: pass
    human_judgment: false
duration: ongoing full-phase execution
completed: 2026-10-21
head_after: 41e0e1a3c933170caccbb1550b00987b233c13b7
---

# Phase 04 Plan 14: Audible libretro parity and boot-to-play Summary

**Actual module sample parity across callback modes and reset and reload plus audio-aware gameplay with a muted control.**

## Accomplishments

- Actual module sample parity across callback modes and reset and reload plus audio-aware gameplay with a muted control.
- Automated selections: libretro.host.audio,runner.boot_to_play.

## Verification

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

## Self-Check: PASSED

Verified through the shipped archive and existing CI. Continue within this invocation to the next ready wave.

Host evidence: each L/R sample equals a separate public-core instance, including54-sample first/reset frames and missing callback intervals. The30-frame initial mono digest equals the runner movie. Batch-only, both, fallback-only, absent and resumed delivery are covered. The existing spacephysics window is audible; muted substitution fails mask8 with identical image evidence. No-input/wrong-port controls produce mask9, retaining response failure bit1 and adding legitimate silence bit8. Static/late/mirror controls retain their original predicates. Full CI601 plus6 archive checks and hygiene24 passed. The SND-04 edge classifier remains unclassified; explicit callback/count/reset/error predicates are automated rather than silently declaring classifier completeness.
