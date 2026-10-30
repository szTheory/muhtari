---
phase: 12-player-ready-delivery-and-playstead-bridge
plan: "01"
subsystem: RetroArch host input integration
tags: [retroarch, input, diagnostics]
dependency_graph:
  requires: []
  provides:
    - Candidate held/released input trace implementation; packaged-host verification is unmet.
  affects:
    - Existing RetroArch diagnostic case
    - Diagnostic comparator build target
tech_stack:
  added: []
  patterns:
    - Reuse the shared BMP reader and host palette library in the diagnostic comparator.
key_files:
  created:
    - tests/common/bmp.c
  modified:
    - .github/workflows/ci.yml
    - tests/diag/tests.cmake
    - tests/retroarch/compare.c
    - tests/retroarch/run_retroarch.cmake
    - tests/retroarch/test.cfg.in
decisions:
  - Move the generic BMP reader to tests/common so tests/retroarch remains within its 300-line bound.
  - Keep this plan halted after its sole hosted attempt failed before the RetroArch host job could run.
metrics:
  duration: "At least 7m22s from the first task commit to halt; pre-commit work began earlier."
  completed_date: 2026-10-30
status: halted
actuals:
  tokens: 2983
  tasks: 1
  commits: 2
commits: 2
plan_head_before: 0ca7c9aa92152b09a277388e0fb2c977601016fa
plan_head_after: b36001767b97060d9c5830bec504cba797a8255e
requirements-completed: []
coverage:
  - id: PLAY-03-packaged-host-input
    description: The exact candidate archive passes held/released input pixel checks in pinned RetroArch while retaining existing DMA and battery host checks.
    requirement: PLAY-03
    verification:
      kind: e2e
      ref: "gh run watch 41056151292 --exit-status (exit 1); https://github.com/szTheory/muhtari/actions/runs/41056151292"
      status: fail
    human_judgment: false
---

# Phase 12 Plan 01: Player-ready delivery and Playstead bridge Summary

The held/released input tracer was implemented, but its only hosted run failed during CMake generation before RetroArch launched, so PLAY-03 remains unverified.

## Plan Status

- Task 1 implementation was committed as `bf21fc3`, then a CMake target regression and formatting issues found in its hosted run were corrected in `b360017`.
- The hosted run tested exact source SHA `bf21fc3ae7d89f4e03f61f1f7a2426dc4af5df29`, attempt 1. The actual `retroarch` job was skipped because `build (macos-arm64)` failed.
- Task 2 was not started because the tracer gate failed. The correction commit is local and unpushed; `origin/phase/12-player-ready-delivery-and-playstead-bridge` remains at `bf21fc3ae7d89f4e03f61f1f7a2426dc4af5df29`. No second hosted attempt was dispatched.
- No published-core or Playstead acceptance is established.

## Hosted Run Receipt

The pinned RetroArch package was version 1.22.2 with DMG SHA256 `81b79121ba26d539064ae13b4d0419a120c3d165afbe656cf5f5412b15fdb434`. The candidate archive digest was not produced because the build failed during CMake generation.

`gh run watch 41056151292 --exit-status` exited 1. The existing run auditor was invoked with `cmake -DINPUT=build/ci-run.json -DEXPECTED_SHA=bf21fc3ae7d89f4e03f61f1f7a2426dc4af5df29 -P tools/check_ci_run.cmake`; it exited 1 because the required aggregate was not completed successfully. [Hosted run 41056151292](https://github.com/szTheory/muhtari/actions/runs/41056151292).

| Job | Outcome |
| --- | --- |
| hygiene | failure |
| changes | success |
| title | skipped |
| be32 | failure |
| library | failure |
| perf | success |
| nofp (ubuntu-26.04-arm) | success |
| asan | failure |
| build (linux-x64) | failure |
| nofp (ubuntu-26.04) | success |
| build (macos-arm64) | failure |
| build (windows-x64) | failure |
| build (linux-arm64) | failure |
| build (windows-arm64) | failure |
| build (macos-x64) | failure |
| quality | failure |
| wasm32 | skipped |
| retroarch | skipped |
| hash-equality | skipped |
| CI required | failure |

The macOS arm64 and library logs reported that `tests/diag/tests.cmake` still referenced the moved `tests/retroarch/bmp.c`. The hygiene job also reported comparator formatting violations. Since the required RetroArch job did not execute, there are no hosted held/released pixel verdicts, candidate archive digest, or retained DMA/battery results from this run.

## Correction and Local Verification

Commit `b360017` points the diagnostic target at `tests/common/bmp.c`, links `muhtari_host` for palette decoding, and formats the comparator. After the correction:

- `/opt/homebrew/opt/llvm@21/bin/clang-format --dry-run --Werror tests/retroarch/compare.c` — exit 0.
- `cmake --preset ci` — exit 0.
- `cmake --build --preset ci --target muhtari-test-retroarch-compare` — exit 0.
- `git diff --check` — exit 0.

These local checks verify the comparator build only; they do not substitute for the required packaged-core RetroArch run. The only hosted attempt remains failed, and its correction has not been pushed or hosted-verified.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 3 - Build blocker] Updated the diagnostic comparator target after relocating the BMP reader.**
- **Found during:** Task 1 hosted run and local correction build.
- **Issue:** The existing target referenced the old BMP path and did not link the host palette implementation used by the comparator.
- **Fix:** Pointed the target at `tests/common/bmp.c` and linked `muhtari_host`; formatted the comparator to satisfy the hygiene check.
- **Files modified:** `tests/diag/tests.cmake`, `tests/retroarch/compare.c`.
- **Commit:** `b360017`.

## Self-Check: PASSED

- Summary file exists at the required path.
- Task commits `bf21fc3` and `b360017` are ancestors of the current branch head.
- `tests/retroarch` contains 293 lines, within the 300-line bound.

## Orchestrator closeout verification

The halt remains open. On corrected source `b360017` (with summary-only successor
`31b12c3`), `cmake --workflow --preset ci` exits 0: all 673 normal tests, three ZIP
packages and all 6 release tests pass. The existing `check_junit.cmake` consumers
with `-DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release`
and `-DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release`
each exit 0, accepting exactly 673 and 6 tests once each. Run URL N/A-local.
`cmake --build --preset ci --target muhtari-test-retroarch-compare` and
`git diff --check` also exit 0.

A fresh complete response saved with `gh run view 41056151292 --repo
szTheory/muhtari --json headSha,status,conclusion,attempt,url,jobs` exits 0.
`cmake -DINPUT=build/phase12-halt-run.json
-DEXPECTED_SHA=bf21fc3ae7d89f4e03f61f1f7a2426dc4af5df29
-P tools/check_ci_run.cmake` exits 1 and prints every failed/skipped job before
rejecting the required aggregate. [Run 41056151292](https://github.com/szTheory/muhtari/actions/runs/41056151292),
attempt 1, remains the sole hosted attempt. Raw responses/logs remain scratch.

ENGINEERING §11 boundary finding: the generic BMP reader relocation requires
all existing build consumers to follow its new path and link dependencies.
The corrected local consumer now builds; no driver, scheduling, elapsed-host-work
or host-bound feasibility conclusion follows because RetroArch never executed.
MU-GSD-009 records this reproduced standalone-consumer preflight lesson.

Accepted 0/4; PLAY-03 remains pending. Plans 12-02, 12-03 and 12-04 are blocked by
this canonical halt. No phase verification, completion or later GSD step ran.
Next separate command: `$gsd-debug "Recover phase 12-01 halted packaged RetroArch input tracer"`.
