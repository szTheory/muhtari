---
phase: 10-homebrew-expansion-memory
plan: "13"
subsystem: testing
tags: [acceptance, ci, inventories, equality, fuzz, wasm32, retroarch, hygiene]
requires:
  - phase: 10-homebrew-expansion-memory
    provides: Plans 01-12 implementation, diagnostics, equality, lifetime and qualified documentation
provides:
  - Exact portable inventories and complete local final-source acceptance
  - Hosted exact-source acceptance on the pull request's genuine main base
  - Root-cause fixes for every failure the first hosted attempts exposed
affects: [phase-10-verification, BOARD-04, BOARD-05]
tech-stack:
  added: []
  patterns:
    - Resolve a relative directory before a GLOB RELATIVE listing
    - Coverage expectations in fuzz targets hold only for harness-controlled state
    - Deadlines sized from complete hosted runs (DEC.47), never from a timeout
key-files:
  created:
    - tests/hygiene/history_control.sh
    - .planning/seeds/SEED-010-nightly-prop-calibration.md
  modified:
    - tests/cmake/hashes.cmake
    - tests/policy/tests.cmake
    - tests/runner/test_save.c
    - .github/workflows/ci.yml
    - tests/fuzz/run.c
    - tests/fuzz/run_seeds.c
    - scripts/hygiene.sh
    - tests/hygiene/tests.cmake
    - tests/inventory/hygiene.txt
    - docs/testing.md
    - tests/libretro/tests.cmake
    - tests/runner/tests.cmake
key-decisions:
  - Carry the minimal CTRL-off hostile program as a built-in run seed, not a tracked fuzz binary, because the trusted-manifest comparison rejects a new regression-fixture row.
  - Exclude deletions from the history scan's binary check so a listed fixture and its row can be removed in a pushed commit.
  - Set the two near-deadline Windows tests to ceil(2 x slowest complete hosted run) with workloads and assertions unchanged.
  - Leave the advisory nightly property job's deadlines untouched; its complete measurement does not exist yet (SEED-010).
requirements-completed: [BOARD-04, BOARD-05]
coverage:
  - id: D1
    description: Exact final inventories and complete local final-source acceptance (workflow, package, release tests, JUnit, hygiene)
    requirement: BOARD-04
    verification:
      - kind: integration
        ref: "cmake --workflow --preset ci at ae4e9b7: 661 ordinary tests passed, 6 release tests passed"
        status: pass
      - kind: integration
        ref: "cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake: 661 tests ran once each"
        status: pass
      - kind: integration
        ref: "cmake --build --preset ci --target package; ctest --preset ci-release; scripts/hygiene.sh --tree; cmake --workflow --preset hygiene (25 tests)"
        status: pass
    human_judgment: false
  - id: D2
    description: Exact-source hosted CI on the pull request with every required and report-only job successful
    requirement: BOARD-05
    verification:
      - kind: e2e
        ref: "gh run view 40702203420 --json attempt,status,conclusion,headSha,url,jobs > build/ci-run.json && cmake -DINPUT=build/ci-run.json -DEXPECTED_SHA=$(git rev-parse HEAD) -P tools/check_ci_run.cmake (attempt 1, source ae4e9b7, 20 jobs success)"
        status: pass
    human_judgment: false
  - id: D3
    description: Full native and PowerPC equality over all 3088 observations, with six authenticated native artifacts
    requirement: BOARD-04
    verification:
      - kind: e2e
        ref: "cmake -DDIRECTORY=hashes -DEXPECTED_SHA=2cb0f16 -DMODE=compare -P tests/cmake/hashes.cmake on the six native artifacts of run 40702203420: 6 identical files"
        status: pass
      - kind: e2e
        ref: "cmake -DNATIVE=evidence/native -DBE=evidence/be -DEXPECTED_SHA=2cb0f16 -P tests/cmake/lane_equality.cmake on lane-equality of Nightly run 40702203633: 2 identical files, 3088 lines each"
        status: pass
    human_judgment: false
  - id: D4
    description: Authenticated genuine current and main-base three-component packages on all six lanes, with the test-card guard armed and equal to its base
    requirement: BOARD-05
    verification:
      - kind: e2e
        ref: "CI run 40702203420 quality job: archive_bytes for six lanes, base 9dcb224; perf job success; test-card Ir 3838448 against baseline 3838448, MUHTARI_PERF_ARMED true"
        status: pass
      - kind: integration
        ref: "sh build/ci/task13-resume-quality.sh locally at 4b5a367 against genuine main base 9dcb224"
        status: pass
    human_judgment: false
  - id: D5
    description: Run fuzz target holds for hostile programs that disable DMA, and the nightly fuzz lane passes
    requirement: BOARD-04
    verification:
      - kind: unit
        ref: "build/ci/tests/muhtari-fuzz-run-replay --seeds: 123 inputs including bankset-ctrl-dma-off; the replaced check fails on the new seed"
        status: pass
      - kind: e2e
        ref: "Nightly run 40702203633: fuzz job success (fuzz.run and fuzz.regress.run)"
        status: pass
    human_judgment: false
  - id: D6
    description: Exactly the two existing actual RetroArch cases pass, and the portable WASI lane passes all tests
    requirement: BOARD-05
    verification:
      - kind: e2e
        ref: "CI run 40702203420 retroarch job (dma and battery cases) and wasm32 job, both success"
        status: pass
    human_judgment: false
  - id: D7
    description: Repository controls for the faults the first hosted attempts exposed (relative artifact directory, history scan on deletions)
    requirement: BOARD-04
    verification:
      - kind: unit
        ref: "ctest --preset ci -R policy.lane_equality.controls (relative case fails without the fix)"
        status: pass
      - kind: unit
        ref: "ctest -R hygiene.history.self_test in the hygiene workflow (old script exits 1 on the deletion case)"
        status: pass
    human_judgment: false
duration: 91m
completed: 2026-10-28
status: complete
actuals:
  tokens: 15037
  tasks: 2
  commits: 14
plan_head_before: af0143a9eff606326a24e3d22edeab4f3d43313d
plan_head_after: ae4e9b7040fd44159ab9fbcb2acf83e37b8a79ec
commits: 14
---

# Phase 10 Plan 13: Final Exact-Source Acceptance Summary

**BankSet/XBoard/XM final source accepted: 661+6 local tests, the pull request's hosted CI on a genuine main base, native and PowerPC equality over 3088 observations, authenticated packages, an armed performance guard and two RetroArch cases, after six root-cause fixes to controls and harnesses that earlier attempts exposed.**

## Performance

- **Duration:** 91 min for this resume (the earlier session's time is not counted)
- **Started:** 2026-10-28T13:10Z
- **Completed:** 2026-10-28T14:42Z
- **Tasks:** 2
- **Commits in range:** 14 (13 plan commits plus `2609719`, a planning-configuration commit made before the resume)

## Accomplishments

- Task 1: inventories reconciled to 661 ordinary and 6 release tests, 25 hygiene tests; the full local workflow, package, release, JUnit and hygiene commands pass on the final source.
- Task 2: pull request 36 (head `ae4e9b7`, tested merge `2cb0f16` of head onto main `9dcb224`) passed the complete hosted CI at attempt 1, audited with the existing CMake tool, plus the nightly fuzz and PowerPC equality jobs on the same head.
- Four independent hosted-only faults (artifact listing, wasm32 stack, retroarch link, nightly fuzz) were reproduced, root-caused and fixed in their owning modules with controls, then two more that only the next hosted attempts could show (trusted-manifest comparison, history scan, Windows deadlines).

## Task Commits

Earlier session (verified, not redone): `e6982ed` inventory, `492f6e1` receipts docs, `cab6a7c`, `005db68`, `5967943` portable consumers and workload calibration, `6984607` Windows genuine-base scheduling.

This resume:

1. **Equality consumer** - `ca5a3f1` (fix): `hashes_native_artifacts` resolves a relative directory before `GLOB RELATIVE`; a `relative` case joins `policy.lane_equality.controls`.
2. **wasm32** - `820c513` (fix): the 104 KiB fixture image in `expansion_images` becomes static; the lane's stack-first layout has a 64 KiB stack.
3. **RetroArch job** - `6047357` (ci): the standalone diagnostic driver compile adds `tests/runner/png_read.c`.
4. **Fuzz harness** - `3722633` (fix), then `85a9e66` (fix): BankSet DMA coverage comes from a re-armed, jammed extra frame; the minimal CTRL-off program is the last built-in run seed (123 inputs).
5. **Hygiene scan** - `4b5a367` (fix): the history check excludes deletions; `hygiene.history.self_test` and `tests/hygiene/history_control.sh` pass a deleted listed binary and fail an added unlisted one.
6. **Deadlines** - `ae4e9b7` (test): `libretro.host.*` and `runner.region.pal`, `runner.usage.port`, `runner.usage.boot_bios_no_file` use 2 s, from the complete Windows runs 40414104123 and 40419374588.

**Plan metadata:** the commit that adds this file.

## Files Created/Modified

See the frontmatter lists. No core source, public header, assertion, workload, inventory membership, quota or dependency changed in this resume. `docs/emulation-tests.md` was kept at its byte budget by leaving it unchanged.

## Decisions Made

See `key-decisions`. The built-in seed replaces a tracked regression binary because the trusted-manifest comparison (`tests/cmake/quality.cmake`, the lane authority) admits only named new rows; widening it was rejected as an authority change.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Bug] Relative artifact directory listed no lanes**
- **Found during:** Task 2, CI run 40419374588 `hash-equality` (`hashes.cmake:168`)
- **Issue:** `file(GLOB RELATIVE)` does not resolve a relative base against the working directory, so the workflow's `-DDIRECTORY=hashes` saw zero artifacts. The existing control passed an absolute path, so the six-native consumer had never run hosted.
- **Fix:** `file(REAL_PATH)` first; relative control added. Reproduced locally from the downloaded artifacts; removing the fix makes the control fail with the same message.
- **Commit:** `ca5a3f1`

**2. [Rule 1 - Bug] wasm32 stack overflow in the expansion fixture writer**
- **Found during:** Task 2, run 40419374588 `wasm32` (`runner.region.pal`, out of bounds memory access in `expansion_images`)
- **Fix:** static storage. No wasm toolchain exists locally; verified by the hosted wasm32 job of run 40702203420 (success).
- **Commit:** `820c513`

**3. [Rule 3 - Blocking] RetroArch job linked the driver without the PNG reader**
- **Found during:** Task 2, run 40419374588 `retroarch`
- **Fix:** add `tests/runner/png_read.c` to the job's compile line; the old line reproduces the undefined symbols locally, the new line links.
- **Commit:** `6047357`

**4. [Rule 1 - Bug] Unsound fuzz invariant on hostile programs**
- **Found during:** Nightly run 40419374584 `fuzz.run` (`run.c:291`)
- **Issue:** The target required MARIA DMA cost after hostile frames, but a program may write CTRL with DMA off (crash input: CTRL `$77`). Reproduced locally with the uploaded input; the core is correct.
- **Fix:** re-arm DMA, jam the CPU and require the scheduler to run in one more frame; the minimal program is seed `bankset-ctrl-dma-off`. The pre-fix harness fails on that seed.
- **Commits:** `3722633`, `85a9e66`

**5. [Rule 1 - Bug] History scan rejected deleting a listed binary**
- **Found during:** the push of `85a9e66` (pre-push hook refused it)
- **Issue:** the scan checked deleted binaries against a manifest that no longer listed them.
- **Fix:** `--diff-filter=d` and a fixture-repository control; the old script fails the control.
- **Commit:** `4b5a367`. The hook was not bypassed.

**6. [Rule 3 - Blocking] Two sub-second-calibrated deadlines timed out on Windows**
- **Found during:** run 40698007354 (`libretro.host.devices` 1.09 s on Windows x64; `runner.region.pal` 1.03 s on Windows arm64)
- **Fix:** ceil(2 x slowest complete hosted run) per DEC.47; the 1 s values came from macOS x64. Workloads and assertions unchanged.
- **Commit:** `ae4e9b7`

**Total deviations:** 6 auto-fixed (4 Rule 1, 2 Rule 3).
**Impact:** all in test, CI and hygiene tooling. The library, runner and libretro core behavior is unchanged from plan 12, and no assertion, workload or inventory was weakened.

## Fix-Round Accounting

The two review-fix rounds were already spent before this resume: hosted attempts on `492f6e1` (run 40411221332), `cab6a7c` (40412835933), `005db68` (40414104123) and `6984607` (40419374588) all failed, and `cab6a7c`, `005db68`/`5967943` and `6984607` each answered one. The owner then chose to resume in place. This resume used the one evidence-backed gap round: failures root-caused above from runs 40419374588 and 40419374584, fixed in `ca5a3f1` to `6047357` and `3722633`, then two follow-ups found only by the next hosted attempts (`85a9e66`/`4b5a367`, `ae4e9b7`). Three pushes and three hosted attempts for this gap: 40695755471 (cancelled after the quality gate failed), 40698007354 (cancelled after both Windows deadlines failed) and 40702203420 (success). The budget is exceeded by the earlier session's third answer and by two gap follow-ups; each followed a distinct, newly observed, reproduced or logged fault, and no assertion, workload, inventory or deadline basis was weakened.

## Verification and Failed Attempts

Local commands have run URL **N/A-local**. No log is committed.

| Command | Exit | Observation |
|---|---:|---|
| `cmake --workflow --preset ci` at `ae4e9b7` | 0 | 661 ordinary and 6 release tests pass |
| `cmake --workflow --preset ci` at `3722633` | 8 | `docs.budgets`: `docs/emulation-tests.md` 41206 over 40960; the prose was dropped |
| `cmake --build --preset ci --target package`; `ctest --preset ci-release` | 0; 0 | three actual ZIPs per lane built; 6 release tests |
| `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` | 0 | 661 tests ran once each |
| `scripts/hygiene.sh --tree`; `cmake --workflow --preset hygiene` | 0; 0 | clean tree; 25 hygiene tests, inventory exact |
| `sh build/ci/task13-resume-quality.sh` at `4b5a367` against main `9dcb224` | 0 | 661+6 current, 656 base ordinary tests; see packages |
| `git push origin phase/10-homebrew-expansion-memory` for `85a9e66` | 1 | pre-push history scan refused the deletion (fixed by `4b5a367`) |
| hosted CI 40695755471 at `3722633` | cancelled | hygiene, asan and four builds failed: trusted-manifest comparison rejected the new fuzz fixture row |
| hosted CI 40698007354 at `4b5a367` | cancelled | `libretro.host.devices` and `runner.region.pal` timed out on Windows; downstream jobs could not run |
| `gh run view 40702203420 ... > build/ci-run.json && cmake -DINPUT=build/ci-run.json -DEXPECTED_SHA=ae4e9b7... -P tools/check_ci_run.cmake` | 0 | attempt 1, completed/success, all 20 jobs success, none failed or skipped |

Hosted evidence (all `https://github.com/szTheory/muhtari/actions/runs/N`):

- Acceptance CI **40702203420**, head `ae4e9b7040fd44159ab9fbcb2acf83e37b8a79ec`, tested merge `2cb0f16e7cfb7950d154dcb6c32c0a2289e09770` (parents main `9dcb224c0fb538e1b529fdaa2964adee3ea4405c` and the head), conclusion **success**. Includes six native builds with 661+6 tests, ASan (643), nofp, be32, hygiene, library, wasm32, retroarch, perf, quality, hash-equality and `CI required`.
- Nightly **40702203633** on the same head: `fuzz`, `be-equality / equality`, `vectors-full`, four `bench` jobs success; `prop` cancelled (advisory, below).
- Earlier failures kept as evidence: CI 40419374588 (hash-equality, wasm32, retroarch), Nightly 40419374584 (fuzz.run), CI 40414104123, 40412835933, 40411221332.

Equality and packages on the exact tested merge `2cb0f16`: the six native artifacts compare as 6 identical files and `lane-equality` as 2 identical files of 3088 lines each; the native Linux x64 and PowerPC files are byte-identical. Archive bytes, three ZIPs per lane (current/base): linux-arm64 336304/329023, linux-x64 327930/321739, macos-arm64 271113/264838, macos-x64 272785/267860, windows-arm64 403626/398068, windows-x64 426154/420844. Archive bytes are report-only under DEC.61. The test-card guard is armed (`MUHTARI_PERF_ARMED`) and measures 3838448 Ir/frame against an equal 3838448 baseline at base `9dcb224`.

Local macOS arm64 three-component measurement through the existing producer at `4b5a367` against the genuine main base `9dcb224` (AppleClang 21.0.0, Darwin arm64, matching manifest and sample identities):

| Component | Candidate bytes | Main-base bytes |
|---|---:|---:|
| Library | 83216 | 81760 |
| Libretro | 89706 | 87534 |
| Runner | 98522 | 96361 |
| Sum | 271444 | 265655 |

After the deadline-only commit the rebuilt library ZIP is 83214 bytes. The base already omits phase 10 and is a phase-versus-main comparison.

## Architecture Review

Bounded review of the changed owners: the equality consumer keeps one directory resolution at its entry (locality); the fuzz target separates harness-controlled coverage from hostile-program properties (test independence); the history scan change is one filter with its own control (ownership, no new registry); deadlines cite complete runs (recurring cost). No unresolved required contract. The review found no new resource bound.

## Known Stubs

None. No placeholder, empty-value or unwired data was added.

## Issues Encountered and Deferred

- **Nightly `prop` job (advisory):** at 10,000 cases under scale 10 it reaches its 390 s test deadlines and the nine-minute job limit (Nightly 40419374584 and 40702203633). It is `continue-on-error` and outside the required aggregate, and no complete run exists to calibrate from (DEC.47), so no deadline was changed. Recorded as SEED-010.
- **Unresolved assumptions retained:** the two spec-less unclassified edge rows from plan 01 are unchanged and unresolved; XM physical specification confidence stays medium; the separate v0.8.0 publication failure and Phase 11 whole-library accounting are outside this plan.
- The shared STATE, ROADMAP and REQUIREMENTS counts were not written by this executor, as the plan assigns them to the orchestrator.

## Threat Flags

None. No network endpoint, auth path or trust-boundary schema was added.

## Next Phase Readiness

Phase 10 has all 13 plans summarized and the exact source accepted. The next separate command is `/gsd-execute-phase 10`, which reconciles the shared counts and runs the phase's review and verification. Do not merge or ship from this step.

## Self-Check: PASSED
