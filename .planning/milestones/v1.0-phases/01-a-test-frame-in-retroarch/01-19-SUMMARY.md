---
phase: 01-a-test-frame-in-retroarch
plan: 19
subsystem: ci
tags: [cachegrind, valgrind, perf-gate, nightly, bench, wasmtime, github-actions]

requires:
  - phase: 01-a-test-frame-in-retroarch
    provides: "CI required roll-up with the perf clauses (plans 14, 15), release.yml's nested ci calls with ref (plans 17, 18), muhtari-run --bench (plan 06), the wasm32 lane and pins (plan 13)"
provides:
  - "perf job: Cachegrind Ir/frame of the test card, merge commit against its first parent measured twice, report-only until MUHTARI_PERF_ARMED"
  - "tests/cmake/perf_compare.cmake with PASS, FAIL, UNSTABLE, NO_BASE and dated allow lines"
  - "seven perf.gate.* tests"
  - "nightly.yml: bench on three platforms plus a wasm32 row, rolling nightly issue, force_failure input"
  - "bench preset; Performance section in docs/testing.md; arming todo"
affects: [ci, perf, nightly, release]

actuals:
  tokens: 7900
  tasks: 3
  commits: 3

plan_head_before: 003fe76b0ec054f48573afb6fd9731242f3ebcfb
plan_head_after: 947dc258d86c0ef4ba84c01d3034d39030b7efb7

tech-stack:
  added: [valgrind (apt, perf job), GNU time (apt, nightly)]
  patterns:
    - "A CMake -P gate prints one verdict line; input errors exit 1 whatever the arming state"
    - "Reporters decide from toJSON(needs) through jq on a here-string, never a piped status"

key-files:
  created:
    - tests/cmake/perf_compare.cmake
    - tests/perf/allow.txt
    - tests/perf/tests.cmake
    - .github/workflows/nightly.yml
    - .planning/todos/pending/2026-10-16-arm-the-instructions-per-frame-gate.md
  modified:
    - .github/workflows/ci.yml
    - CMakePresets.json
    - tests/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - docs/testing.md
    - .planning/STATE.md

key-decisions:
  - "perf runs on direct pull_request and workflow_dispatch runs only; push to main and release.yml's nested call (inputs.ref) skip it, which the roll-up already accepts"
  - "Runners are installed with cmake --install --component runner under $RUNNER_TEMP/perf/head and .../base, equal-length paths"
  - "The pass rule is exact: (head - base) * 10000 > limit * base fails; delta_bp is printed truncated"
  - "An allow line outside today..today+60 days simply does not apply (the expired case gives FAIL, not an input error); a malformed line is an input error"
  - "The force_failure step is the last step of bench-wasm32, after its upload"
  - "requirements-completed lists FRAME-09 only: FRAME-08's Ir gate is armed after the merge (todo)"

patterns-established:
  - "Script-mode test files dispatch on -DCASE and route every case through expect_fail.cmake, including exit 0 cases"

requirements-completed: [FRAME-09]

coverage:
  - id: D1
    description: "perf job reports an Ir/frame verdict for the test card on the probe pull request (NO_BASE while main has no tree) and is green"
    requirement: FRAME-08
    verification:
      - kind: integration
        ref: "gh run view 38950166892 --log --job <perf> | grep -E 'perf testcard head=[0-9]+ ... verdict=NO_BASE$' (exit 0) https://github.com/szTheory/muhtari/actions/runs/38950166892"
        status: pass
    human_judgment: false
  - id: D2
    description: "In release.yml's nested ci call perf is skipped and the nested CI required succeeds"
    requirement: FRAME-08
    verification:
      - kind: integration
        ref: "gh run view 38950166718 --json jobs (ci / perf skipped, ci / CI required success) https://github.com/szTheory/muhtari/actions/runs/38950166718"
        status: pass
    human_judgment: false
  - id: D3
    description: "perf_compare.cmake verdicts: +1.00 % passes, +1.01 % fails, unstable base, allow line current and expired, NO_BASE armed and unarmed, missing measurement is an error"
    requirement: FRAME-08
    verification:
      - kind: unit
        ref: "cmake --workflow --preset ci && ctest --test-dir build/ci -L perf --output-on-failure (exit 0, 8 tests)"
        status: pass
      - kind: integration
        ref: "build and asan jobs, Every test ran with tests/inventory/ci.txt https://github.com/szTheory/muhtari/actions/runs/38950166892"
        status: pass
    human_judgment: false
  - id: D4
    description: "Nightly bench: 300,000 frames on ubuntu-26.04, ubuntu-26.04-arm, macos-15 with max_rss_kb, plus the wasm32-wasmtime-48.0.6 row, in job summaries and bench-* artifacts"
    requirement: FRAME-09
    verification:
      - kind: integration
        ref: "gh run watch 38950166780 --exit-status (exit 0), 4 successful bench jobs https://github.com/szTheory/muhtari/actions/runs/38950166780"
        status: pass
    human_judgment: false
  - id: D5
    description: "Two workflow_dispatch control runs of ci.yml on main show spread=0"
    requirement: FRAME-08
    verification:
      - kind: integration
        ref: "post-merge: gh workflow run ci.yml --ref main, twice; perf job summary spread=0 (todo arm-the-instructions-per-frame-gate)"
        status: unknown
    human_judgment: false
  - id: D6
    description: "A forced-failure nightly dispatch writes the nightly issue and a normal dispatch closes it"
    requirement: FRAME-09
    verification:
      - kind: integration
        ref: "post-merge: gh workflow run nightly.yml --ref main -f force_failure=true, then a normal dispatch (todo arm-the-instructions-per-frame-gate)"
        status: unknown
    human_judgment: false
  - id: D7
    description: "A pull request opened after the merge shows a numeric base= line"
    requirement: FRAME-08
    verification:
      - kind: integration
        ref: "post-merge: the first pull request's perf job log (todo arm-the-instructions-per-frame-gate)"
        status: unknown
    human_judgment: false
  - id: D8
    description: "docs/testing.md Performance section and perf job row, within the docs budget"
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R docs.budgets (exit 0)"
        status: pass
    human_judgment: false

duration: 21min
completed: 2026-10-16
status: complete
---

# Phase 1 Plan 19: Instructions-per-frame gate and nightly bench Summary

**Cachegrind Ir/frame gate (report-only, NO_BASE until the first merge) with seven verdict tests, and a nightly 300,000-frame bench on three platforms plus wasmtime with a rolling `nightly` issue**

## Performance

- **Duration:** 21 min
- **Started:** 2026-10-16T18:39:35Z
- **Completed:** 2026-10-16T19:00:14Z
- **Tasks:** 3
- **Files modified:** 12

## Accomplishments

- `perf` job in `ci.yml`: builds the runner at the merge commit and, when `HEAD^1:CMakePresets.json` exists, at
  the first parent; measures Ir/frame = (Ir360 − Ir60) / 300 under `env -i valgrind --tool=cachegrind
  --cache-sim=no`; head once, base twice; `perf_compare.cmake` writes the verdict to the log and job summary.
  `if:` starts with `!inputs.ref`, so release.yml's nested call skips it. On `workflow_dispatch` it measures one
  commit three times and prints the spread. `MUHTARI_PERF_ARMED: "false"` at workflow level; `perf` joined
  `CI required`'s needs (roll-up program unchanged).
- `tests/cmake/perf_compare.cmake` (126 lines, integer only) and `tests/perf/allow.txt` (header only).
- `perf.gate.{allowed,identical,missing,no_base,plus_1_00,plus_1_01,unstable}` in `tests/perf/tests.cmake`,
  listed in `ci.txt` and `wasm32.txt`; five mutations of the gate (boundary, UNSTABLE, expiry, missing base,
  armed exit) each fail at least one test.
- `nightly.yml`: schedule, `workflow_dispatch` with `force_failure`, `pull_request` on the bench paths; jobs
  `bench` ×3, `bench-wasm32`, `report` (`issues: write`, not on pull requests, decides from `toJSON(needs)`).
  No be32 jobs: plan 14 did not flip be32.
- `bench` preset (Release, runner target only); "Performance" section and `perf` row in `docs/testing.md`;
  arming todo listed in STATE.md.

## Measured values

Test card Ir/frame on ubuntu-26.04 (GCC, `ci` preset): **3,837,643**, identical in runs 38949454075 and
38950166892 (verdict `NO_BASE`: `origin/main` has no `CMakePresets.json`).

Nightly bench, run 38950166780 (https://github.com/szTheory/muhtari/actions/runs/38950166780), 300,000 frames:

| Row | fps_milli | p50 | p90 | p99 | p99.9 | p99.99 | p99.999 | max (µs) | max_rss_kb |
|---|---|---|---|---|---|---|---|---|---|
| ubuntu-26.04 | 3824399 | 257 | 266 | 289 | 493 | 561 | 593 | 620 | 3864 |
| ubuntu-26.04-arm | 5264052 | 189 | 194 | 198 | 202 | 217 | 236 | 245 | 3368 |
| macos-15 | 4718237 | 185 | 264 | 462 | 1100 | 6331 | 24720 | 46601 | 2480 |
| wasm32-wasmtime-48.0.6 | 4929899 | 196 | 223 | 238 | 266 | 367 | 507 | 1795 | n/a |

Gate-code tally: `tests/cmake/*.cmake` 1,166 lines (this plan adds 126) and `scripts/*.sh` 547 lines, against
315 lines in `src/`. AGENTS rule 7's one-tenth ceiling applies from phase 3.

## Evidence

- `cmake --workflow --preset ci` exit 0 (63 tests, package, 4 release tests); `ctest --test-dir build/ci -L perf`
  exit 0, 8 tests; `cmake --workflow --preset hygiene` exit 0, 15 tests.
- Tracer: `gh run watch 38949454075 --exit-status` exit 0 and the NO_BASE verdict line in its perf job log
  (https://github.com/szTheory/muhtari/actions/runs/38949454075).
- Head 947dc25: CI run 38950166892 all jobs success including `perf` and `CI required`
  (https://github.com/szTheory/muhtari/actions/runs/38950166892); Release rehearsals 38949454125 and 38950166718
  success with `ci / perf` skipped and `ci / CI required` success, cleanup success.
- Nightly: `gh run watch 38950166780 --exit-status` exit 0, 4 bench jobs success, `report` skipped (pull request).
- `gh release list` shows no release and `git ls-remote --tags origin 'rehearsal-*'` prints nothing; no
  `nightly` issue was created.

## Task Commits

1. **Task 1: perf job, perf_compare.cmake, allow.txt** - `5f70bd6` (ci)
2. **Task 2: the seven gate self tests (and the allow-date fix)** - `8787e5b` (test)
3. **Task 3: nightly bench, bench preset, docs, arming todo** - `947dc25` (ci)

## Decisions Made

See `key-decisions` above. The allow file's 60-day window is measured from the check date (the plan's "after its
own date" read as the date the check runs, as in the seed).

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Bug] Allow-line day read from a cleared regex match**
- **Found during:** Task 2 (perf.gate.allowed)
- **Issue:** in `perf_days`, the first `string(REGEX REPLACE)` reset `CMAKE_MATCH_3`, so every date's day became 0
  and a line expired yesterday still applied.
- **Fix:** copy the three captures into variables before stripping leading zeros.
- **Files modified:** tests/cmake/perf_compare.cmake
- **Verification:** perf.gate.allowed passes; day counts checked against Python's date arithmetic.
- **Committed in:** 8787e5b

**2. [Rule 1 - Bug] shellcheck SC1007 on `base1= base2=` in the perf step**
- **Found during:** Task 3 (actionlint)
- **Fix:** `base1='' base2=''`.
- **Committed in:** 947dc25

**3. [Rule 2 - Missing] wasm32 inventory**
- **Found during:** Task 2
- **Issue:** the plan names only `ci.txt`, but the wasm32 preset registers the same non-libretro tests and its
  configure fails on an inventory mismatch.
- **Fix:** the seven names added to `tests/inventory/wasm32.txt` too.
- **Committed in:** 8787e5b

**4. [Scope] perf skipped on push to main**
- The plan's condition was `!inputs.ref && <heavy>`; the job also requires `pull_request` or `workflow_dispatch`,
  since the plan defines no push behaviour and the roll-up already accepts the skip outside pull requests.

---

**Total deviations:** 4 (2 bugs, 1 missing critical, 1 scope narrowing). **Impact:** none on the plan's truths.

## Issues Encountered

- One `gh run view --log` call failed with a stream error; a retry succeeded.
- Requirement FRAME-08 stays pending: its gate is armed after the merge.

## Known Stubs

None.

## User Setup Required

None.

## Next Phase Readiness

Ready for 01-20. After the phase merges, the arming todo records the control runs, the first numeric `base=` line,
the arming `ci:` pull request and the forced nightly failure.

## Self-Check: PASSED

- FOUND: tests/cmake/perf_compare.cmake, tests/perf/allow.txt, tests/perf/tests.cmake, .github/workflows/nightly.yml,
  .planning/todos/pending/2026-10-16-arm-the-instructions-per-frame-gate.md
- FOUND commits: 5f70bd6, 8787e5b, 947dc25
