---
phase: 27-headless-developer-trace
plan: "03"
subsystem: testing
tags: [runner, cpu-trace, CMake, CTest, Cachegrind, JSONL]
requires:
  - phase: 27-headless-developer-trace
    provides: Dual-compiled trace loop, generated CPU diagnostic, bounded capture and atomic output from plans 27-01 and 27-02
provides:
  - CPU-backed hooks-off instruction-cost gate wired into the existing Linux perf job
  - Trace-on/off deterministic compatibility matrix for generated legacy, XM, YM and Souper configurations
  - Runner trace contract documentation and source-qualified acceptance evidence
affects: [runner, perf CI, testing, TRACE-01, TRACE-02, TRACE-03, TRACE-04, TRACE-05, TRACE-06, TRACE-07]
actuals:
  tokens: 19931.25
  tasks: 2
  commits: 5
commits: 5
plan_head_before: 22aa7d97afabefc85a34b53844e666d8cb41ccee
plan_head_after: 0926e845c4f9eb19deb3159aa1d1f7edd2c63d61
tech-stack:
  added: []
  patterns:
    - Compare ordinary and trace-loop-off runners on the same generated CPU workload using repeated Cachegrind Ir samples
    - Validate trace-on/off output, frame hashes and canonical state bytes across generated topologies and regions
key-files:
  created:
    - tests/perf/trace.cmake
    - .planning/phases/27-headless-developer-trace/27-TRACE-ACCEPTANCE.md
  modified:
    - .github/workflows/ci.yml
    - runner/main.c
    - src/internal.h
    - tests/perf/tests.cmake
    - tests/runner/test_save.c
    - tests/runner/test_trace.c
    - tests/runner/trace.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - docs/runner.md
    - docs/testing.md
key-decisions:
  - Keep TRACE-06 pending until a normal authorized Linux perf job provides an authentic source-qualified Cachegrind receipt; local controls are not a substitute.
  - Preserve public API 8, ABI 1 and state formats 1/6, 2/7 and 3/8 while adding generated compatibility coverage.
patterns-established:
  - Cost gates consume repeated raw instruction counts and compare the raw 300-frame delta before normalization.
  - Generated build-tree diagnostics provide deterministic trace-on/off compatibility checks without adding runtime media.
requirements-completed: [TRACE-01, TRACE-02, TRACE-03, TRACE-04, TRACE-05, TRACE-07]
requirements-pending: [TRACE-06]
coverage:
  - id: D1
    description: A generated CPU workload runs in same-source ordinary and trace-loop-off builds, with a repeat-stable raw instruction delta capped at 32 Ir per frame.
    requirement: TRACE-06
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci --output-on-failure -R '^perf[.]trace[.]controls$' (exit 0; comparator controls only; run URL: N/A-local)"
        status: pass
      - kind: integration
        ref: "Linux Cachegrind measurement and source-qualified receipt (not run locally: Darwin arm64, command -v valgrind exit 1; authorized delivery CI receipt pending)"
        status: unknown
    human_judgment: false
  - id: D2
    description: Trace-on/off deterministic stdout, requested frame and state hashes, and saved canonical state bytes match across generated legacy, XM, YM and Souper cases in NTSC and PAL.
    requirement: TRACE-04
    verification:
      - kind: e2e
        ref: "ctest --test-dir build/ci --output-on-failure -R '^runner[.]trace[.]equality$' (exit 0; run URL: N/A-local)"
        status: pass
      - kind: integration
        ref: "cmake --workflow --preset ci (exit 0; final source; run URL: N/A-local)"
        status: pass
    human_judgment: false
  - id: D3
    description: Trace arguments, event ordering, finite limits, truncation and failure paths remain covered by the runner and generated numeric acceptance suites.
    requirement: TRACE-01
    verification:
      - kind: integration
        ref: "cmake --workflow --preset ci (exit 0; runner.trace.* and core.trace.edges coverage; run URL: N/A-local)"
        status: pass
    human_judgment: false
  - id: D6
    description: Numeric trace fields, bus access semantics, sampling ticks and beam positions remain pinned by generated edge and runner tests.
    requirement: TRACE-02
    verification:
      - kind: integration
        ref: "cmake --workflow --preset ci (exit 0; core.trace.edges and runner.trace oracle coverage; run URL: N/A-local)"
        status: pass
    human_judgment: false
  - id: D7
    description: Event/output bounds, complete versus truncated output, invalid requests and atomic file failures remain covered.
    requirement: TRACE-03
    verification:
      - kind: integration
        ref: "cmake --workflow --preset ci (exit 0; runner.trace.bounds, limits, arguments, paths and I/O coverage; run URL: N/A-local)"
        status: pass
    human_judgment: false
  - id: D4
    description: Public API, ABI and canonical state compatibility consumers pass without changing public or serialized formats.
    requirement: TRACE-05
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci --output-on-failure -R '^abi[.](api_v8|symbols)$|^runner[.]state[.]' (exit 0; five tests; run URL: N/A-local)"
        status: pass
      - kind: other
        ref: "Public header and state serializer no-change check (exit 0; command in Verification section)"
        status: pass
    human_judgment: false
  - id: D5
    description: The runner and testing guides describe the tested invocation, numeric JSONL contract, bounds, failure behavior and recurring checks within existing documentation budgets.
    requirement: TRACE-07
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci --output-on-failure -R '^docs[.]budgets$|^docs[.]budgets[.]controls$' (exit 0; two checks; run URL: N/A-local)"
        status: pass
      - kind: other
        ref: "cmake --workflow --preset hygiene (exit 0; run URL: N/A-local)"
        status: pass
    human_judgment: false
metrics:
  duration: 48min
  completed: 2026-11-16
status: complete
---

# Phase 27 Plan 03: CPU trace cost and compatibility Summary

**The trace cost gate and compatibility matrix are wired into recurring checks, with the Linux Cachegrind receipt explicitly pending its authorized delivery run.**

## Performance

- **Duration:** 48 minutes
- **Started:** 2026-11-16T22:21:22Z
- **Completed:** 2026-11-16T23:09:17Z
- **Tasks:** 2
- **Files modified:** 13

## Accomplishments

- Added fault-sensitive controls and a CPU-backed Cachegrind measurement script. The existing Linux perf job now compares same-source ordinary and `MUHTARI_TRACE_LOOP=OFF` Release builds before its workflow-dispatch early return, and records source, tool, fixture, raw samples, delta and size observations in the existing ephemeral receipt.
- Added generated trace-on/off equality cases for deterministic stdout, selected frame hashes, canonical state hashes/bytes, restored states, movie defaults, truncation and `--bench` compatibility across legacy, XM, YM and Souper configurations in NTSC and PAL.
- Documented the tested runner invocation, JSONL fields and sampling contract, bounds, truncation and error behavior, CPU-only scope, and recurring checks. Added the detailed source-qualified acceptance matrix in [`27-TRACE-ACCEPTANCE.md`](./27-TRACE-ACCEPTANCE.md), which references the bounds derivation in [`27-BOUNDS.md`](./27-BOUNDS.md).
- Preserved public API 8, ABI 1, state formats 1/6, 2/7 and 3/8, and the existing CI job topology. No runtime ROM, BIOS, save or trace files were added.

## Task Commits

1. **Task 1: Measure a real CPU workload through ordinary and trace-loop-OFF builds** — `607eb42` (test), `6342616` (feat), `970e3a1` (style).
2. **Task 2: Close deterministic compatibility acceptance and publish the runner contract** — `1b56832` (test), `0926e84` (docs).

**Measured plan task commits:** 5 (`22aa7d97afabefc85a34b53844e666d8cb41ccee` through `0926e845c4f9eb19deb3159aa1d1f7edd2c63d61`).

## TDD Notes

- Task 1 followed RED/GREEN: `607eb42` added the failing cost controls, then `6342616` added the comparator and CI wiring. The tracer verification gate passed before expanding the work.
- Task 2's first full equality matrix passed against the behavior delivered by plans 27-01 and 27-02, so there was no valid new RED or behavior implementation commit to claim. An initial configure exposed missing inventory entries; those entries were added before testing. One early expanded footer assertion searched after removing the footer from its list; inspection confirmed the runner correctly emitted a truncated footer, and the assertion was corrected.

## Verification

- `ctest --test-dir build/ci --output-on-failure -R '^perf[.]trace[.]controls$'` — exit 0; local comparison controls passed. These are control tests, not a performance measurement.
- `ctest --test-dir build/ci --output-on-failure -R '^runner[.]trace[.]equality$'` — exit 0; final equality matrix passed.
- `ctest --test-dir build/ci --output-on-failure -R '^abi[.](api_v8|symbols)$|^runner[.]state[.]'` — exit 0; five API, ABI and state tests passed.
- `ctest --test-dir build/ci --output-on-failure -R '^docs[.]budgets$|^docs[.]budgets[.]controls$'` — exit 0; both documentation budget checks passed.
- `/opt/homebrew/opt/llvm@21/bin/clang-format --style=file --dry-run --Werror runner/main.c src/internal.h tests/runner/test_save.c tests/runner/test_trace.c` — exit 0.
- `git diff --check` — exit 0.
- `cmake --workflow --preset ci` — exit 0 on the reviewed source after code-review fixes; 736 tests, all 8 release checks and the package step passed. Run URL: N/A-local.
- `cmake --workflow --preset hygiene` — exit 0 on the final source; all 30 checks passed. Run URL: N/A-local.
- git diff --exit-code origin/main...HEAD -- include/muhtari.h src/state.c — exit 0; public header and canonical state serializer have no changes.
- `uname -s && uname -m` — exit 0, `Darwin arm64`; `command -v valgrind` — exit 1, unavailable locally. Therefore the authentic Linux Cachegrind measurement is N/A-local, and no hosted CI run URL or receipt exists yet.

The real measurement remains wired into the existing Linux perf job and must produce its source-qualified receipt before TRACE-06 can be marked complete. No remote workflow was dispatched or manually triggered.

## Decisions Made

- Local control cases prove the comparator rejects faulty samples and threshold violations; only the normal Linux Cachegrind job can satisfy the authentic cost-evidence requirement.
- Keep the equality fixtures generated in the build tree and retain the previously accepted public and state formats.

## Deviations from Plan

### Auto-fixed Issues

**1. Added missing CTest inventory entries required for the new equality matrix**
- **Found during:** Task 2
- **Issue:** The newly registered equality test was absent from the CI and wasm32 inventory lists, preventing the repository's configure-time inventory checks from accepting it.
- **Fix:** Added `runner.trace.equality` to both existing inventory files.
- **Files modified:** `tests/inventory/ci.txt`, `tests/inventory/wasm32.txt`
- **Verification:** Full CI and hygiene workflows passed.
- **Committed in:** `1b56832`

**2. Applied repository formatting to trace-related files flagged by hygiene**
- **Found during:** Broad hygiene verification
- **Issue:** Existing edited lines in the runner, internal header and actual-runner test harness did not meet the pinned formatter.
- **Fix:** Formatted the affected files with the repository-pinned clang-format, then reran hygiene and final CI.
- **Files modified:** `runner/main.c`, `src/internal.h`, `tests/runner/test_save.c`
- **Verification:** Formatter dry-run, full hygiene and final full CI passed.
- **Committed in:** `970e3a1`

**Total deviations:** 2 auto-fixes. The performance receipt is pending by design at the authorized delivery boundary; no local or hosted measurement was represented as complete.

## Issues Encountered

- The expanded test's first footer assertion inspected a list after the footer had been removed. Actual JSONL inspection showed the expected truncated footer; the test assertion was corrected and the final equality test passed.
- The optional `WINDOWS.md` append for the pending Cachegrind verification was declined because existing ledger frontmatter counts (`open/fixed/total` 17/35/52) disagree with the entries (16/36/52). The append made no change; the shared ledger was left intact. The pending source-specific gate is recorded here and in the acceptance file.
- The docs budget check initially found `docs/runner.md` over its existing limit; the content was tightened and both docs budget checks passed.

## User Setup Required

None for local use or verification. TRACE-06 still needs the normal authorized Linux delivery CI run to provide a genuine Cachegrind receipt; this plan did not dispatch or trigger that run.

## Next Phase Readiness

The local implementation, compatibility matrix, documentation and required local checks are complete. Preserve TRACE-06 as pending until the ordinary delivery pipeline produces a source-qualified passing receipt. The acceptance evidence records the exact source revision and local environment limits.

## Post-review closeout

Code review fixes are recorded in [`27-REVIEW-FIX.md`](27-REVIEW-FIX.md), with per-finding dispositions in [`27-REVIEW-DISPOSITION.md`](27-REVIEW-DISPOSITION.md). The final local CI run passed 736 tests and the final hygiene run passed all 30 checks. A Windows extended-path false-positive warning is deferred as a compatibility follow-up; Windows-specific tests still require the ordinary Windows CI lane. TRACE-06 remains pending the authentic Linux Cachegrind receipt.

## Self-Check: PASSED

The summary and acceptance files exist, all five task commits are ancestors of the current plan head, and the measured commit count is 5.

---
*Phase: 27-headless-developer-trace*
*Completed: 2026-11-16*
