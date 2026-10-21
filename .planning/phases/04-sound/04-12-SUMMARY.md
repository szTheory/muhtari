---
phase: 04-sound
plan: "12"
subsystem: audio
requires:
  - phase: 04-11
    provides: Completed predecessor sound slice
provides:
  - Stable raw ordering across lazy and continuous dual-chip advancement with half-open carry and independent canonical hash reference
affects: [sound-integration]
tech-stack:
  added: []
  patterns: [private-observation, portable-contracts]
key-files:
  created: []
  modified: []
requirements-completed:
  - SND-03
coverage:
  - id: D1
    description: Stable raw ordering across lazy and continuous dual-chip advancement with half-open carry and independent canonical hash reference
    requirement: SND-03
    verification:
      - kind: integration
        ref: audio.observer
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
head_after: cd55cff96d5ba41f4ed71df67132f7f33e74207a
---

# Phase 04 Plan 12: Global observer order and hash reference Summary

**Stable raw ordering across lazy and continuous dual-chip advancement with half-open carry and independent canonical hash reference.**

## Accomplishments

- Stable raw ordering across lazy and continuous dual-chip advancement with half-open carry and independent canonical hash reference.
- Automated selections: audio.observer,audio.frames.

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
