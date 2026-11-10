---
phase: 15-measured-milestone-tune-up
plan: 01
subsystem: testing
tags: [cmake, ctest, policy, documentation]

requires: []
provides:
  - Relative and absolute roots cover the same document-budget files.
  - A recurring synthetic control checks exact limits, named over-limit faults, and missing roots.
  - A source-linked tune-up evidence report with bounded dispositions for unavailable hosted data.
affects: [phase-16, documentation, ci-policy]

actuals:
  tasks: 2
  commits: 3
commits: 3
plan_head_before: bd3612ebe869c4d6c88dcbd0be31a10b3b28ea91
plan_head_after: 3118dcc

tech-stack:
  added: []
  patterns:
    - CMake policy controls require exact exit codes and attributable diagnostics through expect_fail.cmake.

key-files:
  created:
    - tests/policy/budgets.cmake
    - .planning/phases/15-measured-milestone-tune-up/15-TUNEUP.md
  modified:
    - tests/cmake/budgets.cmake
    - tests/policy/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - docs/testing.md
    - .planning/LESSONS.md

key-decisions:
  - Selected the reproducible relative-root document-coverage defect as the tune-up improvement.
  - Preserved all existing size limits, quality floors, deadlines, CI lanes, and host/remote stops.
  - Deferred timing, settings, and pin changes until comparable or authenticated evidence is available.

patterns-established:
  - Resolve the caller's root once before globbing and checking sizes.
  - Exercise budget limits and diagnostics with synthetic fixtures under both root forms.

requirements-completed:
  - TUNE-08

coverage:
  - id: D1
    description: The budget checker enforces the same four-file contract with relative and absolute source roots, including exact limits and each representative over-limit fault.
    requirement: TUNE-08
    verification:
      - kind: unit
        ref: "cmake -DROOT=... -DWORK=... -P tests/policy/budgets.cmake; ctest --test-dir build/ci --output-on-failure -R '^docs\\.budgets(\\.controls)?$'"
        status: pass
    human_judgment: false
  - id: D2
    description: Maintainers can review a current local baseline, six evidence-linked dimension ratings, bounded evidence gaps, and the Phase 16 handoff.
    requirement: TUNE-08
    verification:
      - kind: other
        ref: .planning/phases/15-measured-milestone-tune-up/15-TUNEUP.md
        status: pass
    human_judgment: false

duration: "about 16 min"
completed: 2026-11-10
status: complete
---

# Phase 15 Plan 01: Measured milestone tune-up Summary

**Relative and absolute budget roots now enforce identical coverage, backed by synthetic exact-limit and fault controls plus a passing local CI workflow.**

## Performance

- **Duration:** about 16 min; start time was not captured at plan entry.
- **Completed:** 2026-11-10.
- **Tasks:** 2.
- **Files changed:** 8.

## Accomplishments

- Normalized the budget checker root once for globbing and byte-size checks.
- Added native and wasm32 registration for a synthetic four-file control with eight over-limit cases and two missing-root cases.
- Updated maintainer documentation and recorded a reproduced lesson; produced the [tune-up evidence report](./15-TUNEUP.md).
- Passed the final local workflow (701/701 tests, three packages, 6/6 release checks), tracked-tree hygiene, and whitespace checks.

## TDD evidence

- **RED:** `docs.budgets.controls` failed on the planned relative-root assertion: the unchanged checker reported two files where the fixture required four. CTest exited 8; the JUnit report was classified `RED_EVIDENCE_OK`, and semantic inspection confirmed the target assertion failed for the intended reason.
- **GREEN:** The corrected checker passed the direct control and registered policy tests; the final full local workflow passed.
- **REFACTOR:** None needed.

## Task Commits

The parent created these commits after reconciling the executor's preserved edits and RED/GREEN receipts; the executor sandbox blocked Git writes.

1. **Task 1 RED:** `be5a857` — failing document budget root controls.
2. **Task 1 GREEN:** `f6b7c4a` — normalize document budget root paths.
3. **Task 2:** `3118dcc` — measured tune-up documentation and lesson.

## Files Created/Modified

- `tests/cmake/budgets.cmake` — resolves relative roots consistently before globbing and sizing.
- `tests/policy/budgets.cmake` — checks synthetic limits, diagnostics, relative and absolute roots, and missing roots.
- `tests/policy/tests.cmake`, `tests/inventory/ci.txt`, `tests/inventory/wasm32.txt` — register and inventory the recurring policy control.
- `docs/testing.md` — documents the direct command and root-form contract.
- `.planning/LESSONS.md` — records the reproduced CMake relative-root lesson.
- `.planning/phases/15-measured-milestone-tune-up/15-TUNEUP.md` — captures scores, receipts, and bounded dispositions.

## Decisions Made

Selected the document-budget root-resolution defect because it reproduced locally and had a narrow, fault-tested correction. Kept the existing byte limits, `quality.json` bounds, timeouts, workflow topology, and advisory/required status unchanged. The local run supplies no hosted queue, per-job critical-path, cancellation, or runner-minute evidence, so no pipeline adjustment was inferred.

## Deviations from Plan

None. The first full workflow attempt found the expanded `docs/testing.md` wording 144 bytes over its existing limit. The wording was shortened within the planned documentation task, and the full workflow passed on the cause-fixed rerun.

## Issues Encountered

- The first `cmake --workflow --preset ci` attempt exited 8 because `docs/testing.md` measured 41,104 bytes against the unchanged 40,960-byte budget. The paragraph was tightened to 40,953 bytes; the final workflow then passed all stages.
- The sandbox denied writes to `.git`, so this executor could not create the plan ledger or task commits. Source changes and evidence remain in the working tree for the parent orchestrator's Git writes.

## User Setup Required

None.

## Next Phase Readiness

Local evidence is complete. The root orchestrator owns phase review, verification, and STATE/ROADMAP acceptance. After that review, Phase 16 planning is the next development action. Phase 12/14 host and remote stops and the nine deferred v3.0 requirements remain unchanged.

## Self-Check: PASSED

The implementation files and tune-up report exist. Parent reconciliation confirmed all three task commits on this branch, in RED/GREEN/documentation order. Direct controls and the full local workflow passed after the last implementation edit. The independent source review reports no findings. Phase acceptance remains owned by the phase verifier and orchestrator.

---
*Phase: 15-measured-milestone-tune-up*
*Completed: 2026-11-10*

## Approved v4.0-only publication source

The owner approved publication of the isolated candidate on current main on 2026-11-10. Historical task commits and earlier run receipts above describe the preserved combined branch, not this source's ancestry or host acceptance. The candidate carries the same reviewed v4.0 implementations and controls, adapting only their CMake insertion points. It excludes the deferred v3.0 input tracing, frontend-state fixture and host tooling. The earlier combined-source quality-fixture correction is therefore not needed here; main's fixture manifest and quality policy are unchanged.

Current-source native workflow, packages/release tests, hygiene and exact native/release/hygiene inventories pass. Fresh scoped budget/observer controls and the pinned Emscripten lane pass, including the two-case inventory, 14 parity frames, state replay and instance isolation. Local commands and limits are recorded in ../../v4.0-SHIP-SCOPE.md. No prior host failure, denied request or spent allowance is reopened.
