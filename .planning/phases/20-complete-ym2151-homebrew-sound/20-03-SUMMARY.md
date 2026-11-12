---
phase: 20-complete-ym2151-homebrew-sound
plan: "03"
subsystem: audio-state
tags: [ym2151, portable-state, compatibility]
requires:
  - phase: 20-02
    provides: YM timing, bus, mix, and source producers
provides:
  - Conditional YM format-2/behavior-7 state with complete continuation data
  - Validation-before-apply and transactional refusal for YM state
  - Global latest version reporting with machine-dependent envelope documentation
affects: [20-04, 20-06, 20-07]
commits: 2
plan_head_before: 0120fbdc3d681c4d04221ced0cec740d501edf97
plan_head_after: ae016864f5c6306e67f11c89e4a5bb90438e9f23
actuals:
  tokens: 18774
  tasks: 2
  commits: 2
tech-stack:
  added: []
  patterns:
    - Conditional canonical state visitor for per-machine schemas
    - Full semantic validation before any state application
key-files:
  created:
    - .planning/phases/20-complete-ym2151-homebrew-sound/20-03-SUMMARY.md
  modified:
    - src/state.c
    - tests/core/test_state.c
    - tests/audio/test_ym2151.c
    - src/instance.c
    - include/muhtari.h
    - docs/embedding.md
    - docs/runner.md
key-decisions:
  - Keep unchanged non-YM snapshots at format 1 / behavior 6; use format 2 / behavior 7 only for the YM profile.
  - Keep API 8 / ABI 1 and report global latest-supported behavior 7 / state format 2.
  - Derive YM clock-history bounds from existing serialized values without adding a runtime origin field.
patterns-established:
  - YM state is appended conditionally after the historical payload and identity fields.
  - State rejection checks preserve machine, battery, spans, observers, and caller buffers.
requirements-completed: []
coverage:
  - id: D1
    description: YM state resumes exact midstream output and refuses invalid or incompatible snapshots transactionally.
    requirement: YM-05
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci --output-on-failure --no-tests=error -R '^(ym[.]tracer|core[.]state[.](ym|fields|payload|topologies|aliases|lifecycle))$' (exit 0)"
        status: pass
      - kind: unit
        ref: "core.state.ym: repaired-CRC semantic, future-version, identity, truncation, and CRC refusal cases (exit 0)"
        status: pass
    human_judgment: false
  - id: D2
    description: Global latest versions and actual per-machine envelopes are documented while API and ABI shape remain fixed.
    requirement: YM-08
    verification:
      - kind: integration
        ref: "Task 2 full build and selected state/API/runner CTest gate; 91 tests passed"
        status: pass
      - kind: integration
        ref: "Focused built/basic/audio/quality-policy CTest gate; 78 tests passed with frame/audio hashes unchanged"
        status: pass
    human_judgment: false
metrics:
  duration: 57min
  completed: 2026-11-12
status: complete
---

# Phase 20 Plan 03: YM state continuation and version reporting Summary

**YM-only format-2/revision-7 snapshots now preserve complete midstream continuation and transactional refusal while unchanged non-YM format-1/revision-6 snapshots and API 8/ABI 1 remain intact.**

## Performance

- **Duration:** 57 min from the Plan 20-02 handoff to completion.
- **Started:** 2026-11-12T07:31:16Z
- **Completed:** 2026-11-12T08:28:33Z
- **Tasks:** 2
- **Files modified:** 19

## Accomplishments

- Added a conditional canonical visitor for YM registers, operators, timers, bus clock phase, signed outputs, synth/raw carry, and cached future-affecting values. Validation checks scalar domains and timing relationships before applying any destination or battery field.
- Replaced the YM state-unavailable tracer with public save/load/resume and refusal coverage across NTSC and PAL. Tests compare subsequent frames, audio, YM/bus state, counters, battery bytes, spans, observers, and allocation counts.
- Reported global latest behavior revision 7 and state format 2 while retaining the exact legacy non-YM format-1/revision-6 path and public API/ABI shape. Frame and audio digest rows remain unchanged.

## Task Commits

1. **Task 1: Save and transactionally restore complete YM continuation** - `51636df` (`feat`).
2. **Task 2: Refuse semantic faults and document global-latest version reporting** - `ae01686` (`feat`).

## Verification

- Task 1 focused configure/build and seven selected tracer/state tests passed.
- Task 2 full build and selected state/API/runner gate passed all 91 tests.
- Focused built/basic/audio/quality-policy gate passed all 78 tests; the existing frame, transition, and PCM digests matched.
- Documentation budget checks passed both tests.

An additional `cmake --workflow --preset ci` was started before the coordinator directed that the full workflow remain for Plan 20-07. It was interrupted with exit 130 before completion. The partial run exposed stale global-revision-6 metadata checks and several unrelated process-check timeouts where `/bin/ps` was denied. The direct metadata consumers were updated without changing digest rows; the focused 78-test suite then passed. The full workflow has not been claimed as passing or rerun here.

## Decisions Made

- Kept API 8 / ABI 1 and selected format 2 / behavior 7 only for the YM profile; no-YM state remains format 1 / behavior 6.
- Kept the YM clock-history validator within existing serialized fields, using a nonnegative history bound instead of adding runtime origin state.

## Deviations from Plan

The plan omitted five current version consumers discovered during implementation: `tests/cmake/hashes.cmake`, `tests/audio/expected.txt`, `tests/built/expected.txt`, `tests/cmake/quality.cmake`, and `tests/policy/quality.cmake`. The first three now consume revision-7 reporting while preserving all pinned frame/audio digests and sample counts. The quality guard accepts the adjacent 6-to-7 metadata-only transition and retains a forged-frame-pin refusal control. No historical digest was regenerated.

## Issues Encountered

Early focused tests exposed valid reset cache values, reachable high bits in the YM noise LFSR, and the chip's nonzero bus epoch; the final validator uses producer-backed ranges and the existing clock tuple. A tracer assertion also assumed a fixed initial sample count after inserting a midstream checkpoint; it now checks real paired continuation. The extra broad workflow was interrupted as described above; its remaining full-CI work belongs to the later Phase 20 gate.

## User Setup Required

None.

## Next Phase Readiness

Plan 20-03 is complete. This summary does not mark YM-05 or another phase requirement complete; phase-level acceptance and the independent old/new state compatibility proof remain for the later Phase 20 plans. The coordinator owns the shared STATE/ROADMAP update and next sequential dispatch.

---
*Phase: 20-complete-ym2151-homebrew-sound*
*Completed: 2026-11-12*

## Self-Check: PASSED

- Summary file exists at the required path.
- Task commits `51636df` and `ae01686` are ancestors of the current plan head.
- No personal filesystem paths or untracked test artifacts were found; only this new summary is untracked.
