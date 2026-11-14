---
phase: 22-complete-souper-homebrew-mapper
plan: "04"
subsystem: testing
tags: [libretro, souper, state, ctest]
requires:
  - phase: 22-complete-souper-homebrew-mapper
    provides: Souper mapper implementation, generated diagnostic, and complete public snapshot support from plans 22-01 through 22-03
provides:
  - Runtime-loaded native libretro coverage of the generated Souper diagnostic with literal CPU/MARIA observations
  - Loaded-module snapshot continuation, malformed-state preservation, and observation fault controls
affects: [phase-22-verification, libretro-acceptance]
actuals:
  tokens: 3001
  tasks: 2
  commits: 2
commits: 2
plan_head_before: 5c31a8592cb7ef6fa6f39a156d2e3652e4ed1aee
plan_head_after: 4c91217c9d5855d5d575667d3e4668d1fd2c7526
tech-stack:
  added: []
  patterns:
    - Runtime-resolved libretro entry points drive an integration test against the built module.
    - Continuation observations are captured per frame and compared after restoring the module's own snapshot.
key-files:
  created:
    - .planning/phases/22-complete-souper-homebrew-mapper/22-04-SUMMARY.md
  modified:
    - tests/libretro/host.c
    - tests/libretro/tests.cmake
    - tests/inventory/ci.txt
key-decisions:
  - Keep the new loaded-module diagnostic case separate from the existing loaded memory-map descriptor case.
  - Keep all changes test-only; no dependency or production behavior was added.
requirements-completed:
  - SOUPER-04
  - SOUPER-06
coverage:
  - id: D1
    description: The generated Souper diagnostic passes literal CPU mailbox and MARIA pixel checks through the runtime-loaded native libretro module; the existing descriptor case remains registered.
    requirement: SOUPER-06
    verification:
      - kind: integration
        ref: "ctest --preset ci --output-on-failure --no-tests=error -R '^libretro[.]host[.]souper(_trace)?$'"
        status: pass
      - kind: integration
        ref: "cmake --workflow --preset ci (722/722 ordinary tests; 8/8 release tests)"
        status: pass
    human_judgment: false
  - id: D2
    description: A loaded-module midpoint snapshot reproduces frames seven through twelve; null, short, long, and corrupt snapshots preserve state, and mailbox/pixel fault controls reject altered observations.
    requirement: SOUPER-04
    verification:
      - kind: integration
        ref: "ctest --preset ci --verbose --no-tests=error -R '^libretro[.]host[.]souper(_trace)?$'"
        status: pass
      - kind: integration
        ref: "ctest --preset asan --output-on-failure --no-tests=error -R '^libretro[.]host[.]souper(_trace)?$'"
        status: pass
    human_judgment: false
duration: "not recorded; executor start timestamp was not retained across continuation"
completed: 2026-11-14
status: complete
---

# Phase 22 Plan 04: Complete Souper homebrew mapper Summary

**The generated Souper diagnostic now exercises the production native libretro module, with literal observations, deterministic snapshot continuation, and refusal controls.**

## Performance

- **Duration:** Not measured; the executor start timestamp was not retained across continuation.
- **Completed:** 2026-11-14
- **Tasks:** 2
- **Files modified:** 3 source/test files; this summary records the plan result.

## Accomplishments

- Added `libretro.host.souper_trace`, which loads the generated diagnostic through the actual built module and checks video callbacks, literal system-RAM mailbox values, and the discriminating MARIA pixel row.
- Added midpoint format-3 / behavior-8 save-and-restore coverage through the module's public entry points. The restored six-frame continuation matches per-frame observations and final module-owned state bytes.
- Verified four malformed snapshots are refused without changing the saved midpoint, and representative altered mailbox/pixel observations fail their independent judges.
- Registered the loaded trace test in the ordinary native CI inventory while preserving the existing loaded `memory_map` test and portable/static observer cases.

## Task Commits

1. **Task 1: Judge the generated diagnostic through the actual loaded module** - `6201416` (test)
2. **Task 2: Prove loaded-module midpoint restore and refusal preservation** - `4c91217` (test)

## Files Created/Modified

- `tests/libretro/host.c` - Loaded diagnostic case, per-frame observations, restore checks, malformed-state controls, and faulted-oracle controls.
- `tests/libretro/tests.cmake` - Registered the new runtime-loaded test with the generated diagnostic fixture.
- `tests/inventory/ci.txt` - Added the exact ordinary native CI inventory row.

## Decisions Made

- Keep `souper_trace` separate from `souper` because they prove distinct loaded-module contracts.
- Keep all additions test-only and use existing host, CMake fixture, and CTest mechanisms; no dependency or product code was added.
- Preserve the current CI topology. The full native run took 45.67 seconds for the ordinary test phase; `policy.publish.controls` was the longest observed test at 44.75 seconds and exercises publication fault controls. A single run does not support a safe change to that coverage or pipeline structure.

## Deviations from Plan

None - plan executed as written.

## Issues Encountered

The first ASan CTest tool response exceeded its output limit. The recorded `build/asan/Testing/Temporary/LastTest.log` showed the required fixture chain and both focused loaded-module cases passed, so no retry was needed.

## Verification

- `git diff --check` — exit 0.
- Task 1: `cmake --preset ci` — exit 0; required target build — exit 0; focused `libretro.host.souper(_trace)?` CTest — exit 0 (5 tests including fixtures; 0.43 seconds).
- Task 2 Release: required host target build — exit 0; verbose focused `libretro.host.souper(_trace)?` CTest — exit 0 (5 tests including fixtures; 0.43 seconds).
- Task 2 ASan: `cmake --preset asan -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++` — exit 0; required target build — exit 0; focused CTest — exit 0 (fixture chain and both loaded-module cases passed; the new loaded trace case took 0.11 seconds).
- `cmake --workflow --preset ci` — exit 0; build, all 722 ordinary tests, packaging, and all 8 release tests passed. The ordinary CTest phase took 45.67 seconds; the release test phase took 1.08 seconds.
- `sh scripts/hygiene.sh` — exit 0 (about 31 seconds); no personal paths, unmanifested binary, or other hygiene finding.
- Stub scan over the three changed source/test files — no TODO, FIXME, placeholder, or equivalent stub markers found.

All evidence is from local commands; no hosted CI URL applies. No committed test log was added.

## Next Phase Readiness

The 22-04 plan is complete and locally verified. Phase 22 acceptance remains for the parent executor's final goal verification and review; this summary does not declare the phase or milestone complete.

## Self-Check: PASSED

- Summary file exists at the required phase path.
- Task commits `6201416` and `4c91217` are ancestors of the current branch head.
- Task 2 commit contains no tracked-file deletions.

---
*Phase: 22-complete-souper-homebrew-mapper*
*Completed: 2026-11-14*
