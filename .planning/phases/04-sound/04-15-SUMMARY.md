---
phase: 04-sound
plan: "15"
subsystem: audio
requires:
  - phase: 04-14
    provides: Completed predecessor sound slice
provides:
  - Eight non-silent 600-frame canonical audio pins with zero clipping and observer and discard equality feed existing platform comparisons
affects: [sound-integration]
tech-stack:
  added: []
  patterns: [private-observation, portable-contracts]
key-files:
  created: []
  modified: []
requirements-completed:
  - SND-03
  - SND-05
coverage:
  - id: D1
    description: Eight non-silent 600-frame canonical audio pins with zero clipping and observer and discard equality feed existing platform comparisons
    requirement: SND-03
    verification:
      - kind: integration
        ref: audio.samples
        status: pass
    human_judgment: false
  - id: D2
    description: Portable regression selection runner.hashes_file
    requirement: SND-03
    verification:
      - kind: integration
        ref: runner.hashes_file
        status: pass
    human_judgment: false
duration: ongoing full-phase execution
completed: 2026-10-21
head_after: 1ebf7b5d401f9958a23cfc4dadf337b896dc310d
---

# Phase 04 Plan 15: Versioned non-silent sample pins Summary

**Eight non-silent 600-frame canonical audio pins with zero clipping and observer and discard equality feed existing platform comparisons.**

## Accomplishments

- Eight non-silent 600-frame canonical audio pins with zero clipping and observer and discard equality feed existing platform comparisons.
- Automated selections: audio.samples,runner.hashes_file.

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

Production spectral/alias controls and TIA/POKEY/IRQ selections passed before pin acceptance (7 checks, exit0). Eight measured rows each contain479591 mono samples at48000/1, revision2 and exact-q23-16x64-dc20. Source/PCM checks compare separate core instances with observation on/off and alternate discarded buffers, and reset repeats the first54 samples. Largest delivered/pre-clamp peak11555 (eightysix), below29204; all clips0. Observed transition counts range1059 to139350.

The corpus holds both port1 ProLine buttons for60 frames then releases for540. rmtromdemo waits for release before starting music; an all-held control correctly failed non-silence. Soundeffects was absent from the old fixture table; its pinned source and generated ROM are byte-identical to soundtest, so the existing recipe and proven identical frame pin now cover both names. No old raw or video pin changed. A deliberately corrupted PCM pin returns1 through the real judge. Existing compare count/byte controls pass.

Release CI604 checks plus6 archive tests, hygiene24, and assertion-enabled audio.samples with8 setup fixtures (52.72 seconds total) passed. Expanded hashes.txt preserves the three card/PNG rows and adds14 audio fields for soundtest/pokey. Native/be32/WASM hosted equality is deferred to ship; this plan proves their shared input and fixture reachability.
