---
phase: 12-player-ready-delivery-and-playstead-bridge
plan: "20"
subsystem: testing
tags: [c17, cmake, wasi, wasmtime, portable-tests]
requires: []
provides:
  - Build-owned exclusive host-file fixture with passing native and WASI battery tracers
  - Exact local WASI portable-lane failure evidence for the next phase decision
affects: [phase-12-player-ready-delivery, local-wasi-test-lane]
actuals:
  tokens: 6056
  tasks: 2
  commits: 2
commits: 2
plan_head_before: 624390c03e9e929da501f1faaeffecb94d202587
plan_head_after: a949a69c947569c55cdceddbf089d70b076a6e06
tech-stack:
  added: []
  patterns: [target-private build path, C11 exclusive test fixture]
key-files:
  created:
    - .planning/phases/12-player-ready-delivery-and-playstead-bridge/12-20-LOCAL-EVIDENCE.md
    - .planning/phases/12-player-ready-delivery-and-playstead-bridge/12-20-SUMMARY.md
  modified:
    - tests/core/test_state.c
    - tests/core/tests.cmake
    - .planning/LESSONS.md
key-decisions:
  - "Keep the file observer in the existing battery case and bind its fixture to the current binary directory."
  - "Leave the unrelated WASI emulator-wrapper and timeout failures outside this plan's source owners."
patterns-established:
  - "A WASI-facing host-file assertion uses an exclusive build-owned fixture and explicit content/cursor fault controls."
requirements-completed: []
coverage:
  - id: D1
    description: "The existing battery case checks an exclusive build-owned file and rejects cursor/content faults on native and WASI."
    requirement: PLAY-03
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^core[.]state[.]battery$'"
        status: pass
      - kind: unit
        ref: "ctest --test-dir build/wasm32 -R '^core[.]state[.]battery$'"
        status: pass
    human_judgment: false
  - id: D2
    description: "The complete local WASI portable workflow and exact JUnit audit pass for the repaired source."
    requirement: PLAY-03
    verification:
      - kind: integration
        ref: "cmake --workflow --preset wasm32; check_junit.cmake against build/wasm32/junit.xml"
        status: fail
    human_judgment: false
duration: 32min
completed: 2026-11-07
status: halted
---

# Phase 12 Plan 12-20: WASI Battery Fixture Portability Summary

**The battery file check now passes on native and WASI, while three unrelated failures keep the complete local WASI lane blocked.**

## Performance

- **Duration:** 32 min
- **Started:** 2026-11-07T14:42:12Z
- **Completed:** 2026-11-07T15:13:42Z
- **Tasks:** 2
- **Files modified:** 5

## Accomplishments

- Replaced the unsupported `tmpfile()` canary with a target-private file under the preset build directory, using C11 exclusive creation, checked content/cursor observation, wrong-cursor and changed-marker controls, and checked cleanup.
- The final native battery case and final actual WASI battery case passed. The full native workflow passed 700 ordinary tests, package generation, and all 6 release tests; formatter and tree hygiene passed.
- Preserved exact local SDK/runtime identities and the full WASI report. Its 622-case portable JUnit report has three failures, so the complete WASI prerequisite remains unmet and Phase 12 stays accepted 0/4.

## Task Commits

1. **Task 1: Run the battery/state file canary through an explicit build-owned fixture** - `c5b8395` (`test`).
2. **Task 2: Complete the corrected-source local lanes and record the remaining boundary** - `a949a69` (`docs`).

## Files Created/Modified

- `tests/core/test_state.c` - File-local observer, C11 exclusive fixture, and cursor/content fault controls.
- `tests/core/tests.cmake` - Target-private build-owned fixture path.
- `.planning/LESSONS.md` - Reproduced local WASI file-fixture lesson.
- [12-20-LOCAL-EVIDENCE.md](12-20-LOCAL-EVIDENCE.md) - Tool, source, tracer, full-lane, and audit results.

## Decisions Made

- The test-only file remains outside core storage and uses the existing source/build preopen.
- No registration, inventory, timeout, emulator wrapper, or runtime setting was changed. The three full-lane failures need a separately scoped decision before the Phase 12-16 local prerequisite can pass.

## Deviations from Plan

None in tracked source scope. The formatter gate first identified wrapping-only issues in the new fixture code; `clang-format` corrected them, and the final native workflow and WASI tracer were rerun against the formatted source.

## Issues Encountered

- The first native workflow evidence monitor raced a transient build-file replacement and lost its exit status. The ignored monitor was corrected; the subsequent full native workflow exited 0.
- The full WASI workflow exited 8. `retroarch.states.controls` directly executes a WASI module without its configured emulator and reports `exec format error`. `runner.movie.v2_edges` and `runner.region.pal` reached their existing 6-second and 4-second limits. The exact JUnit consumer exited 1 on those three cases. They are outside this plan's source owners, and no timeout or unrelated test changes were made.
- The `.git` directory is read-only under the ordinary worker profile. The two task commits used the authorized GSD commit path with the configured staged-hygiene hook.

## User Setup Required

None. No permission, settings, host, content, remote CI or publication action occurred.

## Next Phase Readiness

Plan 12-16 remains blocked: its complete local WASI prerequisite did not pass, and its 12-18/12-19 dependencies remain unresolved. The phase orchestrator must decide whether a separately scoped local plan may repair the WASI test wrapper and assess the existing timeout limits. No Phase 12 acceptance or hosted/Playstead readiness is established.

## Self-Check: PASSED

- `12-20-LOCAL-EVIDENCE.md` and this summary exist.
- Task commits `c5b8395` and `a949a69` are ancestors of `plan_head_after`.
- The preserved Plan 12-17 failure receipt remains available and unchanged.

---
*Phase: 12-player-ready-delivery-and-playstead-bridge*
*Completed: 2026-11-07*
