---
phase: 05-pipeline-tune-up
plan: "02"
status: complete
subsystem: infra
tags: [ctest, scheduling, timeouts, github-actions]
requires:
  - phase: 05-01
    provides: Audited baseline and fixed component allocations
provides:
  - Independent compatibility freshness and fault-control outputs
  - Measured slow-test bounds and explicit scheduling costs with unchanged inventories
  - Comparable hosted job cohorts and provisional bounds without caching or check removal
affects: [05-03, 05-04, 05-05, 05-08, 05-09]
tech-stack:
  added: []
  patterns: [separate generated writers, image-specific timing cohorts, censored timeout evidence]
key-files:
  created: []
  modified:
    - tests/policy/tests.cmake
    - tests/audio/tests.cmake
    - tests/built/tests.cmake
    - tests/runner/tests.cmake
    - CMakePresets.json
    - .github/workflows/ci.yml
    - .github/workflows/nightly.yml
    - .github/workflows/release.yml
    - docs/testing.md
key-decisions:
  - Keep eight full driver/runner audio comparisons, fixtures, exact inventories and every existing safety check.
  - Separate runner-image and direct/called-workflow cohorts; do not count copied rerun timestamps as new executions.
  - Treat sub-ten-sample bounds and lane scales as provisional; refresh affected cohorts after later pipeline changes.
  - Charge the one added freshness-writer line to the fixed 100-line reproduced-correctness allocation; all other executable edits have zero net growth.
duration: 17min
completed: 2026-10-21
actuals:
  tokens: 13455
  tasks: 3
  commits: 3
plan_head_before: 160a272774bccf9de0751793cb39a714a3ec8e19
plan_head_after: 93af701f283291d1487860dfb014a7bb11414044
requirements-completed: []
requirements-contributed: [TUNE-02, TUNE-05]
coverage:
  - id: isolated-writers
    description: Freshness and its exact stale-page control own distinct output files
    requirement: TUNE-02
    verification:
      - kind: integration
        ref: ctest --preset ci -R freshness selection --parallel 2 --schedule-random --repeat until-fail:20, exit 0
        status: pass
    human_judgment: false
  - id: complete-scheduling
    description: Equal-cost and randomized schedules execute each of the 606 required regular tests once
    requirement: TUNE-02
    verification:
      - kind: integration
        ref: python3 /tmp/muhtari-tune-schedules.py, exit 0; exact Counter comparison and check_junit judge
        status: pass
      - kind: unit
        ref: Existing check_junit judge rejects empty required inventory with exit 1 and exact no-test diagnostic
        status: pass
    human_judgment: false
  - id: bounded-workload
    description: Measured bounds and costs retain all eight audio pairs and fixture prerequisites
    requirement: TUNE-05
    verification:
      - kind: integration
        ref: cmake --workflow --preset ci, exit 0; 606 regular tests and six package checks
        status: pass
      - kind: other
        ref: Ten local full-corpus audio repetitions, exit 0; hosted successes and censored failures below
        status: pass
    human_judgment: false
  - id: measured-job-bounds
    description: Successful job cohorts set bounds with image identities and honest provisional status
    requirement: TUNE-02
    verification:
      - kind: other
        ref: Paginated read-only gh API history and all-attempt job reads, exit 0; cohort tables below
        status: pass
      - kind: integration
        ref: cmake -DDIR=.github/workflows -P tests/cmake/timeouts.cmake, exit 0; 26 timeout declarations
        status: pass
    human_judgment: false
---

# Phase 5 Plan 2: Isolated outputs and measured scheduling Summary

**Separate compatibility-page writers and measured audio/basic/movie bounds preserve all work, with 606 regular tests and six package checks passing.**

All three tasks are complete. TUNE-02 and TUNE-05 remain phase-wide requirements:
later BE relocation, quality, release safety and final acceptance plans still own
parts of their contracts. They are contributed to here, not checked complete.

## Task commits

1. `d9672c4` — give the stale-page control its own generated output.
2. `ac0c2f5` — measured slow-test bounds, costs and provisional lane scales.
3. `93af701` — comparable job bounds and concise testing guidance.

Hooks ran on every commit with the repository's noreply identity. No tracked file
was deleted. TDD applicability was resolved false for this execution; existing
behavioral and fault controls verify these registration/configuration changes.

## Writer ownership and schedule verification

The real freshness test keeps `policy/compatibility.md`. The control writes
`policy/compat_doc_fixture/generated.md` and reads the separately mutated
`policy/compat_doc_fixture/compatibility.md`. The status-count mutation, expected
exit 1 and exact `compat_doc_fixture/compatibility.md.+is stale` regex are unchanged.
There is no lock, retry or blanket serialization.

`cmake --preset ci && ctest --preset ci -R '^policy[.]compat_doc_fresh([.]self_test)?$'
--parallel 2 --schedule-random --repeat until-fail:20 --output-on-failure` — exit 0,
20 repetitions of each check, both before and after the remaining changes. These
local checks have no hosted URL. A selected invocation uses a separate JUnit
destination when its output must coexist with the full inventory report.

`python3 /tmp/muhtari-tune-schedules.py` — exit 0. This one-off experiment temporarily
sets every generated CTest COST to 1, runs the full regular selection, then restores
the generated file and runs random order. Each schedule executes exactly the same
606-name multiset once. The existing `check_junit.cmake` independently accepts both
reports; a Python Counter additionally rejects any extra execution. No ordering of
ties is assumed. With an empty required inventory, the same judge exits 1 with
`no test ran (0 selected from the inventory)`; the driver requires that exact fault.
No maintained judge, no-skip rule or required artifact contract changes.

## Sampling method and comparability

Read-only commands, all exit 0 after requesting terminal-escape output explicitly
for logs:

- `gh api --paginate --slurp 'repos/szTheory/muhtari/actions/runs?per_page=100'`
- `gh api --paginate --slurp 'repos/szTheory/muhtari/actions/runs/RUN/jobs?filter=all&per_page=100'`
- `gh api --allow-escape-sequences repos/szTheory/muhtari/actions/jobs/JOB/logs`
- `python3 /tmp/muhtari-tune-sample.py`, `python3 /tmp/muhtari-tune-logs.py` and
  `python3 /tmp/muhtari-tune-analyze.py` — exit 0, synthesized cohorts from those reads.

All available history was paginated: 133 CI, 77 Nightly and 30 Release runs,
**240 runs / 3,567 all-attempt API job records**. Collection stopped there; there
are fewer than 40 comparable successes for every affected current workload. It
does not backfill with older product workloads or unrelated jobs. The sound-workload
window begins at 2026-10-21 01:48 UTC (`59b8c66`), whose source/audio/built expectations
match the final sound source in `0e68eb5` and this plan. Earlier no-sound cohorts
are invalid for these final workload limits. Registering the counter and fixing
the writer add policy work; this is a before-scheduling hosted cohort, not hosted
measurement of the completed phase DAG. Later changed jobs require a fresh cohort.

280 relevant completed job logs were read in that bounded window, including
non-runtime jobs needed to identify their runner images. Runner-image revisions
are split, not silently pooled, even when compiler version is unchanged. Below,
each role uses its newest observed image cohort. Direct CI, release-called CI and
Nightly remain separate. A completed successful individual test remains valid
timing evidence when another test fails its job; job timeout calculations require
completed-success jobs. Cancelled, skipped and failed jobs never count as successes.

Re-run-failed-jobs records can carry successful earlier jobs forward under a new
attempt/job ID with unchanged start/end timestamps. The audit reads every record
but counts `(run, job name, start, end, conclusion)` once as an actual execution.
For example, the two ASan API records in run 39611531779 have identical execution
timestamps. Inflating their sample count would manufacture repeated measurements.
Actual repeated failed audio executions are retained as censored observations.

### Tool and runner identities

All sampled runtime builds use pinned CMake 4.4.3, wasmtime 48.0.6, 7800basic 0.41
and OpenBIOS assembler pin `b8bc745a8744`. Native Release is distinct from Clang
21.1.8 ASan/UBSan Debug/O1 with configured short runs and GNU 15.2.0 qemu-ppc Release.
WASM uses Clang 21.1.8 and pinned wasmtime. The final runtime source is unchanged.

| Role | Newest observed image revision | C compiler |
|---|---|---|
| Linux x64, BE, ASan, WASM and ordinary CI Linux x64 jobs | ubuntu-26.04 / 20261018.168.1 | GNU 15.2.0; Clang 21.1.8 for ASan/WASM |
| Linux arm64 | ubuntu-26.04-arm / 20261018.155.1 | GNU 15.2.0 |
| macOS arm64 / RetroArch | macos-15-arm64 / 20260907.0337.1 | AppleClang 17.0.0.17000013 |
| macOS x64 | macos-15 / 20260824.0482.1 | AppleClang 17.0.0.17000013 |
| Windows x64 | windows-2025-vs2026 / 20261007.264.1 | MSVC 19.51.36260.0 |
| Windows arm64 | windows-11-vs2026-arm64 / 20261013.183.4 | MSVC 19.51.36260.0 |
| Nightly bench/fuzz/prop/report and release publish | ubuntu-26.04 / 20261012.163.1 | GNU 15.2.0; Clang 21.1.8 for fuzz |
| Nightly bench arm64 | ubuntu-26.04-arm / 20261012.150.1 | GNU 15.2.0 |

The latest Windows job really used the earlier listed image; image version does
not follow run-ID ordering consistently. Older Ubuntu/Windows image groups were
excluded from the newest-image figures rather than assumed identical. CMake/CTest
default parallelism remains max(processors, 2); hosted effective utilization is
measured below rather than inferring a processor count from that rule.

## Slow tests and scales

Audio remains exactly **eight 600-frame cases through both driver and runner**, with
eight FIXTURES_REQUIRED and the exact eight-row corpus assertion. Neither emulated
workload nor comparison/hash contract was shortened. Successful final-corpus audio:

| Lane | Test successes | Maximum seconds | Run containing maximum |
|---|---:|---:|---|
| Linux x64 | 6, provisional | 48.12 | [39614595363](https://github.com/szTheory/muhtari/actions/runs/39614595363) |
| Linux arm64 | 9, provisional | 23.77 | [39530116644](https://github.com/szTheory/muhtari/actions/runs/39530116644) |
| macOS arm64 | 12 | 34.45 | [39614799435](https://github.com/szTheory/muhtari/actions/runs/39614799435) |
| macOS x64 | 11 | 124.92 | [39617761785](https://github.com/szTheory/muhtari/actions/runs/39617761785) |
| Windows x64 | 7, provisional | 66.52 | [39617761785](https://github.com/szTheory/muhtari/actions/runs/39617761785) |
| Windows arm64 | 8, provisional | 34.29 | [39532369201](https://github.com/szTheory/muhtari/actions/runs/39532369201) |
| ASan | 5, provisional | 100.84 | [39529469630](https://github.com/szTheory/muhtari/actions/runs/39529469630) |
| BE | 6, provisional | 627.76 | [39614799435](https://github.com/szTheory/muhtari/actions/runs/39614799435) |
| WASM | 3, provisional | 76.01 | [39533130210](https://github.com/szTheory/muhtari/actions/runs/39533130210) |

The base audio bound becomes **ceil(2 x 124.92) = 250 seconds**, COST 125. It is
not an average of the earlier 120-second timeouts in both attempts of
[39611531779](https://github.com/szTheory/muhtari/actions/runs/39611531779).
Those failures establish only censored lower bounds.

`ctest --preset ci -R '^audio[.]samples$' --parallel 2 --repeat until-fail:10
--output-on-failure --output-junit <separate absolute report>` — exit 0. Ten local
full-corpus audio executions have maximum **15.35 s**, with their real build
fixtures also executed, on arm64 Darwin 25.6.0 / AppleClang Release / CMake 4.4.3.
The repeated selection's wall is 153.55 s including repeated fixtures. These local
samples neither replace hosted Intel measurements nor establish low-power speed.

| Registration | Successful measurement | Final base bound / COST |
|---|---|---|
| sample builds | macOS x64 built.backuptest max 6.12 s; 13 executions, run 39530116644 | Existing 5/16 s bounds retained; COST 7 starts fixture producers early |
| basic sample checks | macOS x64 basic.pokey max 9.35 s; 12 successes, run 39614799435 | ceil(2 x 9.35) = 19 s / COST 10 |
| basic.coverage | macOS x64 max 27.10 s; 13 successes, run 39614799435 | ceil(2 x 27.10) = 55 s / COST 28 |
| basic.coverage.self_test | macOS x64 max 26.15 s; 13 successes, run 39614799435 | ceil(2 x 26.15) = 53 s / COST 27 |
| common movie cases | Windows x64 family max 1.05 s (lag); seven executions, run 39617761785, provisional | ceil(2 x 1.05) = 3 s; common process-startup family bound |

The Intel `basic.pokey` failure in run 39617761785 timed out at displayed 10.10 s;
it does not enter the 9.35-second successful maximum. The Windows replay in the
same run is displayed as a 0.71-second timeout but timestamps span **1.060443 s**
from 14:34:59.5773211 to 14:35:00.6377638. Nearby successful movie.lag takes 1.05 s
and runner.version 1.58 s, showing process/scheduling contention under the one-second
family limit. The failure is censored, not a measured 0.71-second success, nor proof
of a specific CTest internal accounting cause. Its family gets the measured 3 s
bound; there is no retry or semantic replay change.

Matched duration-sum ratios use only same-run common successful test names and
matching lane image cohorts. Sum ratios describe the configured lane workload;
ASan's short runs remain explicit. Native release-only checks are excluded.

| Lane | Pairs | Common names | Largest numerator / denominator seconds | Ratio / scale | Maximum run |
|---|---:|---:|---:|---:|---|
| ASan | 4, provisional | 593 | 330.43 / 192.96 | 1.712427 / 2 | [39617761785](https://github.com/szTheory/muhtari/actions/runs/39617761785) |
| BE | 4, provisional | 567 | 2263.58 / 186.67 | 12.126105 / 13 | [39614799435](https://github.com/szTheory/muhtari/actions/runs/39614799435) |
| WASM | 2, provisional | 555 | 288.06 / 178.79 | 1.611164 / 2 | [39533130210](https://github.com/szTheory/muhtari/actions/runs/39533130210) |

ASan/BE paired runs: 39617761785, 39614595363, 39611531779 plus 39533130206
(ASan) or 39614799435 (BE). WASM paired runs: 39614799435, 39533130210.
The measured sums come from timestamped CTest output rounded to hundredths;
hosted JUnit XML is not uploaded, so exact XML duration sums are unavailable.
Costs use rounded successful elapsed maxima as scheduling hints, not new ordering
assertions. [CTest COST](https://cmake.org/cmake/help/latest/prop_test/COST.html)
and [fixture prerequisites](https://cmake.org/cmake/help/v4.3/prop_test/FIXTURES_REQUIRED.html)
retain their existing meanings.

## Successful job cohorts and bounds

Every row is a completed-success job cohort, separately identified by role, image
above and the final sound workload window. Formula: ceil(2 x maximum / 60), with
the established five-minute network/roll-up floor. **P means provisional n<10**.
All supplied samples in a row are attempt 1 after copied executions are collapsed;
real failed/censored attempt 2 work was read but contributes no successful sample.
URLs name the run holding each maximum; all sample identities remain recoverable
with the paginated commands and explicit window/image criteria above.

| Direct CI job/runner | n | Max seconds | Run | Calculated minutes / declaration |
|---|---:|---:|---|---|
| changes | 4 P | 7 | [39611531779](https://github.com/szTheory/muhtari/actions/runs/39611531779) | 5 / 5 |
| hygiene | 4 P | 40 | [39531241714](https://github.com/szTheory/muhtari/actions/runs/39531241714) | 5 / 5 |
| title | 3 P | 6 | [39617860376](https://github.com/szTheory/muhtari/actions/runs/39617860376) | 5 / 5 |
| build Linux x64 | 5 P | 90 | [39611531779](https://github.com/szTheory/muhtari/actions/runs/39611531779) | 5 / matrix 7 |
| build Linux arm64 | 8 P | 122 | [39614799435](https://github.com/szTheory/muhtari/actions/runs/39614799435) | 5 / matrix 7 |
| build macOS arm64 | 11 | 84 | [39614799435](https://github.com/szTheory/muhtari/actions/runs/39614799435) | 5 / matrix 7 |
| build macOS x64 | 9 P | 186 | [39614799435](https://github.com/szTheory/muhtari/actions/runs/39614799435) | 7 / matrix 7 |
| build Windows x64 | 6 P | 129 | [39531241714](https://github.com/szTheory/muhtari/actions/runs/39531241714) | 5 / matrix 7 |
| build Windows arm64 | 7 P | 121 | [39532469256](https://github.com/szTheory/muhtari/actions/runs/39532469256) | 5 / matrix 7 |
| ASan | 5 P | 131 | [39529469630](https://github.com/szTheory/muhtari/actions/runs/39529469630) | 5 / 5 |
| nofp x64 | 3 P | 24 | [39617761785](https://github.com/szTheory/muhtari/actions/runs/39617761785) | 5 / 5 network floor |
| nofp arm64 | 9 P | 40 | [39614799435](https://github.com/szTheory/muhtari/actions/runs/39614799435) | 5 / 5 network floor |
| BE full portable execution | 6 P | 691 | [39614595363](https://github.com/szTheory/muhtari/actions/runs/39614595363) | 24 / 24 |
| hash-equality | 3 P | 6 | [39614799435](https://github.com/szTheory/muhtari/actions/runs/39614799435) | 5 / 5 |
| WASM | 2 P | 97 | [39614799435](https://github.com/szTheory/muhtari/actions/runs/39614799435) | 5 / 5 |
| library | 3 P | 31 | [39617761785](https://github.com/szTheory/muhtari/actions/runs/39617761785) | 5 / 5 |
| RetroArch | 9 P | 31 | [39614799435](https://github.com/szTheory/muhtari/actions/runs/39614799435) | 5 / 5 |
| perf | 1 P | 47 | [39614799435](https://github.com/szTheory/muhtari/actions/runs/39614799435) | 5 / 5 |
| CI required | 4 P | 4 | [39617860376](https://github.com/szTheory/muhtari/actions/runs/39617860376) | 5 / 5 |

Native cohort run identities: Linux x64 {39617761785,39614799435,39614595363,
39611531779,39533130206}; Linux arm64 {39617761785,39614799435,39614595363,
39611531779,39532369201,39531241714,39530145463,39530116644}; macOS arm64
{39617761785,39614799435,39614595363,39611531779,39533130206,39532469256,
39532369201,39531241714,39530145463,39530116644,39529469630}; macOS x64
{39614799435,39614595363,39533130206,39532469256,39532369201,39531241714,
39530145463,39530116644,39529469630}; Windows x64 {39611531779,39532469256,
39532369201,39531241714,39530116644,39529469630}; Windows arm64 {39617761785,
39533130206,39532469256,39532369201,39531241714,39530116644,39529469630}.
BE: {39617761785,39614799435,39614595363,39611531779,39532469256,39532369201}.
These identity sets are evidence summaries, not raw logs or executable policy.

| Nightly job/runner | n | Max seconds | Run | Bound minutes |
|---|---:|---:|---|---:|
| bench Linux x64 | 2 P | 109 | [39529469620](https://github.com/szTheory/muhtari/actions/runs/39529469620) | 5 |
| bench Linux arm64 | 2 P | 69 | [39529469620](https://github.com/szTheory/muhtari/actions/runs/39529469620) | 5 |
| bench macOS arm64 | 2 P | 81 | [39529469620](https://github.com/szTheory/muhtari/actions/runs/39529469620) | 5 |
| bench WASM | 2 P | 116 | [39544379935](https://github.com/szTheory/muhtari/actions/runs/39544379935) | 5 |
| vectors-full, fetch included | 1 P | 100 | [39544379935](https://github.com/szTheory/muhtari/actions/runs/39544379935) | 5 |
| fuzz | 1 P | 76 | [39544379935](https://github.com/szTheory/muhtari/actions/runs/39544379935) | 5 |
| property 10,000 cases | 2 P | 246 | [39529469620](https://github.com/szTheory/muhtari/actions/runs/39529469620) | 9 |
| report | 1 P | 5 | [39544379935](https://github.com/szTheory/muhtari/actions/runs/39544379935) | 5 |

Two-sample Nightly rows contain runs 39544379935 and 39529469620. Other rows
contain only their linked run; the other image is excluded. This keeps the real
fetch and 60-second fuzz work: no download cache or shortened replay appears.

| Release job | n | Max seconds | Run | Bound minutes |
|---|---:|---:|---|---:|
| release-please | 3 P | 23 | [39530116768](https://github.com/szTheory/muhtari/actions/runs/39530116768) | 5 |
| publish | 1 P | 379 | [39533130210](https://github.com/szTheory/muhtari/actions/runs/39533130210) | 13 |
| install-line-latest | 1 P | 7 | [39533130210](https://github.com/szTheory/muhtari/actions/runs/39533130210) | 5 |
| rehearsal | 0 P current sound-archive samples | unavailable | Historical [38951834012](https://github.com/szTheory/muhtari/actions/runs/38951834012), max 399 s | retain 14 |
| install-line / cleanup | 0 P current rehearsals | unavailable | Historical [38943087443](https://github.com/szTheory/muhtari/actions/runs/38943087443), max 9/17 s | retain 5 |

Release-please identities: 39617761503, 39532369255, 39530116768. The shared
CI workflow also serves release-called CI. On run 39533130210 its matched current
image cohorts each have n=1 P: Linux x64 71 s, Linux arm64 52 s, macOS arm64 62 s,
macOS x64 103 s, Windows arm64 118 s, RetroArch 26 s and WASM 103 s. Older-image
called jobs in that run remain historical evidence: Windows x64 118 s, BE 670 s,
ASan 142 s, hygiene 26 s, library 31 s, nofp x64/arm64 7/8 s, changes 3 s,
hash-equality 6 s and CI required 2 s. These do not inflate the direct cohorts;
none needs a larger declaration than the shared measured bound. Zero current
rehearsal samples are explicit, rather than re-labelling pre-sound archives as
current success. Later release-safety edits refresh the relevant release cohorts.

## Utilization, cache and retention decisions

For [39617761785](https://github.com/szTheory/muhtari/actions/runs/39617761785),
the log-derived successful duration sums / regular CTest real times are below.
They estimate occupied test slots, not CPU utilization or an exact JUnit sum.
Failed Intel/Windows lanes omit the censored test from the numerator, so those
figures are lower bounds. Package tests are excluded from regular numerators.

| Lane | Successful test-seconds | CTest wall seconds | Ratio |
|---|---:|---:|---:|
| Linux x64 | 193.58 | 54.32 | 3.564 |
| Linux arm64 | 101.93 | 27.83 | 3.663 |
| macOS arm64 | 135.15 | 46.31 | 2.918 |
| macOS x64, failed basic.pokey | 475.18 | 138.92 | at least 3.421 |
| Windows x64, failed replay | 307.63 | 78.82 | at least 3.903 |
| Windows arm64 | 171.24 | 45.04 | 3.802 |
| ASan | 330.43 | 94.94 | 3.480 |
| BE | 2193.87 | 623.69 | 3.518 |

The BE job is the critical path: 656 s in that run. Its build-step wall is
9.945 s (14:33:24.4066497 through 14:33:34.3516428), **1.52%** of the job;
configuration plus build is 11.829 s, **1.80%**. These are measured step-wall
proxies, not summed compiler CPU time. Even including configuration stays far
below the 40% compiler-cache trigger. There is **no new cache**, and no untested
cached/cold equivalence claim. Existing pinned downloads still run and verify hashes.
Queue/dependency waits, exact hosted JUnit XML sums and a 90-day redundancy history
are unavailable, not zero. No check is removed: no named fault has a demonstrated
stronger executing replacement. All six native legs and existing full BE work
remain at this slice; relocation belongs to the next plan.

The initial local workflow took 16.04 s for regular tests; final local workflows
remain roughly 15–18 s with the same inventory plus unchanged packaging. The
machine and incidental concurrent read-only sampling vary. These are correctness
checks, **not evidence of a latency gain**. There is no after-change hosted run
on this branch yet. The completed DAG must be remeasured during final acceptance;
only changed jobs need refreshed cohorts after BE/quality/report additions.

## Fixed-budget impact

B=12,885, S=2, N=1,970 and C=14,853 remain unchanged. The counter observes
**13,252** physical lines, broad sensitivity **13,311**, source denominators
8,577 / 6,530; advisory ratios **154.5062% / 202.9402%**. No ratio compliance is
claimed. Current headroom is **1,601**, allocated to accepted later work.

| Component | Allocation | Prior use | This slice net | Current use |
|---|---:|---:|---:|---:|
| Count/inventory | 500 | 368 | 0 | 368 |
| Reproduced safety/correctness | 100 | 0 | 1, independent writer declaration in tests/policy/tests.cmake | 1 |
| Digest/monthly report | 650 | 0 | 0 | 0 |
| Quality evidence/ratchet | 450 | 0 | 0 | 0 |
| BE relocation/equality | 120 | 0 | 0 | 0 |
| Integration reserve | 150 | 0 | 0 | 0 |

All inherited test-registration, preset and workflow edits are charged in the
same whole-file boundary and have **zero net growth** together. The ASan comment
adds one physical line while the audio comment removes one; no line golf or
control is removed. Maintainer prose and planning evidence remain non-executable
documentation under the existing boundary. There is no allocation transfer,
new ceiling, new path inventory entry, funding from hypothetical savings, or
policy weakening. The remaining 99 safety lines still cover the later signer/API
fixes; count/inventory retains its original 132-line headroom.

`cmake -DROOT=. -DINVENTORY=tests/policy/gate-lines.txt -DOUT=build/ci/gate-count.json
-P tests/cmake/gate_count.cmake` — exit 0. `docs/testing.md` is 40,901 / 40,960 bytes.

## Final verification, deviations and deferred seed

- `cmake --workflow --preset ci` — final exit 0: **606 regular tests + six package checks**.
- Full JUnit judges with `-DEXCLUDE=release` and `-DLABELS=release` — exit 0: 606/6 exactly once.
- `cmake -DDIR=.github/workflows -P tests/cmake/timeouts.cmake` — exit 0: all 26 declarations have provenance.
- `scripts/hygiene.sh --tree` and `git diff --check` — exit 0.
- Repeated freshness, both exact schedule inventories, ten full audio samples and
  the expected empty-inventory rejection pass as recorded above. Local checks have no hosted URL.

The first final workflow correctly rejected expanded testing prose at 43,332
bytes. Prose was shortened to 40,901 within the unchanged 40,960-byte cap and
the full workflow passed. No control, budget or scope was relaxed. Detailed
cohort evidence is here rather than in the already nearly full public testing page.
There is no substantive plan deviation or authentication gate.

One pre-existing out-of-scope tooling seed is recorded in `deferred-items.md`:
direct `budgets.cmake -DROOT=.` observes only README/header due to its relative
glob. The registered test supplies the absolute source root and correctly caught
the oversized document; final direct checks also use the absolute root. This is
a seed under the repository's proportional review rule, not a new requirement.

No goal-blocking stub, skipped test or unrun plan verification remains. No new
endpoint, auth path, trust-boundary schema or external write is introduced.
Phase completion, hosted latency improvement, lane promotion and current
rehearsal/publication safety are not claimed by this plan.

## Self-Check: PASSED

All nine changed implementation/configuration/doc paths and this summary exist.
All three task commits are ancestors of HEAD. The persisted ledger measures
three commits between the recorded heads. No accidental tracked deletion occurred.
