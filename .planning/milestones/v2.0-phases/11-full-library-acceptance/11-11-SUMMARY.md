---
phase: 11-full-library-acceptance
plan: "11"
subsystem: test-budgets
tags: [ctest, timeouts, bankset, gap-closure]
status: complete
requires:
  - phase: 11-full-library-acceptance
    provides: "Plans 11-09 and 11-10 in the tree (title.local.fixture and the compat-doc self test measured after both), verification gap 1 (11-VERIFICATION.md), review WR-03 (11-REVIEW.md)"
provides:
  - "diag.bankset writes build/<preset>/tests/diag/bankset-matrix and diag.bankset_tracer keeps build/<preset>/tests/diag/bankset, so neither reads the other's half-written files"
  - "title.pal.bios TIMEOUT 4 with a comment naming the 4 s floor and its reason"
  - "A measurement comment (command shape, n, slowest time) on each of title.pal.fast, title.pal.bios, title.pin.order, title.pin.order.self_test, title.pin.recipe, title.local.fixture and policy.compat_doc_fresh.self_test"
affects: [phase-11-ship]
actuals:
  tokens: 1100
  tasks: 2
  commits: 2
plan_head_before: 9101b3138f0b04a1b060ecb27a3bb4b11b83ea91
plan_head_after: 916bbe2369232b731b84889cfc9909efdda0824d
commits: 2
tech-stack:
  added: []
  patterns:
    - "A generated test script sets OUT only when the including script has not, so a matrix wrapper can redirect the shared body to its own tree"
key-files:
  created: []
  modified:
    - tests/diag/tests.cmake
    - tests/compat/tests.cmake
    - tests/policy/tests.cmake
    - .planning/phases/11-full-library-acceptance/deferred-items.md
key-decisions:
  - "A separate output tree rather than a RESOURCE_LOCK for the two bankset tests: it removes the shared state instead of serialising two long tests."
  - "No budget is lowered. Six of the seven stay at their current value because it already exceeds ceil(2 x slowest); only title.pal.bios rises (2 s to 4 s)."
requirements-completed: [COMP-02, COMP-03]
coverage:
  - id: C1
    description: diag.bankset and diag.bankset_tracer write separate output trees and both pass when run concurrently from empty trees
    requirement: COMP-02
    verification:
      - kind: integration
        ref: rm -rf build/ci/tests/diag/bankset build/ci/tests/diag/bankset-matrix then ctest --preset ci -R '^diag[.]bankset(_tracer)?$' -j2 --repeat until-fail:3 --no-tests=error --output-on-failure, exit 0; build/ci/tests/diag/bankset/32n/report.txt and build/ci/tests/diag/bankset-matrix/32n/report.txt both exist; N/A-local, no run URL
        status: pass
    human_judgment: false
  - id: C2
    description: Every new phase-11 CI test budget names the measurement it came from and holds at least twice the slowest observed run, with title.pal.bios at 4 s or more
    requirement: COMP-03
    verification:
      - kind: other
        ref: ctest --preset ci -R '^(title[.](pal[.](fast|bios)|pin[.](order|order[.]self_test|recipe)|local[.]fixture)|policy[.]compat_doc_fresh[.]self_test)$' -j1 -FS '.*' --no-tests=error ten times, exit 0 each; plus the per-test times of one full cmake --workflow --preset ci, exit 0; table in the body; N/A-local, no run URL
        status: pass
      - kind: other
        ref: grep -c 'not yet measured in CI' tests/compat/tests.cmake printed 0 (baseline 6); grep -c 'slowest of 10 runs alone' printed 6 for tests/compat/tests.cmake and 1 for tests/policy/tests.cmake; grep -cF 'floor 4 s, as for eightysix' printed 1; the awk TIMEOUT check for title.pal.bios exited 0
        status: pass
    human_judgment: false
  - id: C3
    description: The ci workflow passes with plans 11-09, 11-10 and 11-11 in place
    requirement: COMP-02
    verification:
      - kind: e2e
        ref: cmake --workflow --preset ci, exit 0, 673 of 673 tests passed and the package stage 6 of 6; N/A-local, no run URL
        status: pass
    human_judgment: false
---

# Phase 11 Plan 11: Measured test budgets and the bankset output trees Summary

The new phase-11 CI tests carry measured budgets with their evidence named, title.pal.bios has a 4 s floor, and diag.bankset no longer shares an output tree with diag.bankset_tracer.

## Accomplishments

- Task 1 (tracer): the tracer script sets OUT only `if(NOT DEFINED OUT)`; the matrix script sets OUT to `bankset-matrix` before including it, with a comment saying each test writes its own tree. deferred-items.md records the plan-04 item as resolved. Proven from empty trees: three concurrent `-j2` repeats passed and both 32n report files exist.
- Task 2: the seven tests were measured alone (ten runs, `-j1`) and under load (one full `cmake --workflow --preset ci`, all cores, alongside diag.expansion). Each TIMEOUT is the larger of its current value and ceil(2 x slowest). title.pal.bios rises from 2 s to 4 s (floor as for eightysix, the BIOS-path analogue). The six provisional comments in tests/compat/tests.cmake and the one in tests/policy/tests.cmake were replaced.

## Measurements (macOS arm64, local)

| Test | Alone max of 10 (s) | Full ci run (s) | ceil(2 x slowest) (s) | HEAD TIMEOUT | New TIMEOUT |
|---|---|---|---|---|---|
| title.pal.fast | 0.24 | 0.35 | 1 | 2 | 2 |
| title.pal.bios | 0.69 | 1.49 | 3 (floor 4) | 2 | 4 |
| title.pin.order | 0.02 | 0.02 | 1 | 1 | 1 |
| title.pin.order.self_test | 0.02 | 0.02 | 1 | 1 | 1 |
| title.pin.recipe | 0.12 | 0.15 | 1 | 3 | 3 |
| title.local.fixture | 1.53 | 2.15 | 5 | 20 | 20 |
| policy.compat_doc_fresh.self_test | 0.32 | 0.38 | 1 | 10 | 10 |

No loaded value exceeded half of its new budget (the nearest is title.pal.bios, 1.49 s of 4 s, against half = 2 s), so no re-run was needed. For reference, in the same full ci run diag.bankset took 2.66 s (budget 42 s) and diag.bankset_tracer 0.18 s (budget 30 s).

## Commands

| Command | Exit | Run URL |
|---|---|---|
| cmake --preset ci && cmake --build --preset ci && rm -rf build/ci/tests/diag/bankset build/ci/tests/diag/bankset-matrix && ctest --preset ci -R '^diag[.]bankset(_tracer)?$' -j2 --repeat until-fail:3 --no-tests=error --output-on-failure && test -f both 32n/report.txt | 0 | N/A-local |
| ctest --preset ci -R '<seven tests>' -j1 -FS '.*' --no-tests=error (x10) | 0 each | N/A-local |
| cmake --workflow --preset ci (loaded measurement, before the edits) | 0 (673 of 673) | N/A-local |
| Task 2 verify chain: configure, build, ctest on title., policy.compat_doc_fresh.self_test and diag.bankset (15 of 15), the two greps, then cmake --workflow --preset ci | 0 (673 of 673, package 6 of 6) | N/A-local |

Hosted CI times for these tests are confirmed at ship; replacing the local measurements with a run ID is outside this round.

## Deviations from Plan

None - plan executed exactly as written. Where the current budget already exceeds ceil(2 x slowest), the comment says "kept" and gives both numbers instead of the plan's `2 x <slowest>` wording, because the plan forbids lowering a budget.

## Known Stubs

None.

## Threat Flags

None.

## Next

The orchestrator owns the next step.

## Self-Check: PASSED

- FOUND: tests/diag/tests.cmake, tests/compat/tests.cmake, tests/policy/tests.cmake, deferred-items.md edits
- FOUND commits: b889363, 916bbe2
