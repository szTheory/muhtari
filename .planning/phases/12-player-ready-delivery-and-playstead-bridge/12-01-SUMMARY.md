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
    - tests/input/pixels.c
  modified:
    - .github/workflows/ci.yml
    - tests/diag/tests.cmake
    - tests/retroarch/compare.c
    - tests/retroarch/run_retroarch.cmake
    - tests/retroarch/test.cfg.in
    - tests/input/test_input.c
    - tests/input/tests.cmake
    - docs/testing.md
decisions:
  - Move the generic BMP reader to tests/common so tests/retroarch remains within its 300-line bound.
  - Keep this plan halted after its sole hosted attempt failed before the RetroArch host job could run.
  - Permit one scoped corrected-source acceptance after complete failed-job audit and local preflight; halt again when actual held input fails.
metrics:
  duration: "Original halt at least 7m22s; scoped recovery includes the roughly 40-minute revised hosted run."
  completed_date: 2026-10-30
status: halted
actuals:
  tasks: 2
  commits: 3
commits: 3
plan_head_before: 0ca7c9aa92152b09a277388e0fb2c977601016fa
plan_head_after: 28e13e1cb7da122d72d1c83d438861001b503504
requirements-completed: []
coverage:
  - id: PLAY-03-packaged-host-input
    description: The exact candidate archive passes held/released input pixel checks in pinned RetroArch while retaining existing DMA and battery host checks.
    requirement: PLAY-03
    verification:
      kind: e2e
      ref: "gh run watch 41062201311 --exit-status (exit 1); https://github.com/szTheory/muhtari/actions/runs/41062201311; original pre-host failure 41056151292 retained below"
      status: fail
    human_judgment: false
  - id: PLAY-03-input-oracles
    description: Existing C input case independently checks held/released registers, idle unrelated port and input-band fault predicates.
    requirement: PLAY-03
    verification:
      kind: unit
      ref: "ctest --test-dir build/ci -R '^(input[.]|runner[.]movie[.]replay$|libretro[.]host[.]input$)' --no-tests=error --output-on-failure --output-junit phase12-input.xml (exit 0, 19/19); cmake --workflow --preset ci (exit 0, 673+6); N/A-local"
      status: pass
    human_judgment: false
---

# Phase 12 Plan 01: Player-ready delivery and Playstead bridge Summary

The original run failed before host execution. After scoped recovery, corrected
source builds and local input-oracle controls pass, but actual pinned RetroArch
produces idle pixels during the held-input interval. The independent judge rejects
them; the plan remains halted and PLAY-03 remains unverified. Both failed hosted
receipts are retained below. No retry or downstream plan ran.

## Original Plan Status (historical)

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

## Original Orchestrator Closeout Verification (historical)

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

## Scoped recovery decision (2026-10-30)

The original feasibility gate is spent and remains failed. This recovery revises
only its stop boundary under ENGINEERING §11/GSD-HANDOFF §3.1: permit **one new
exact corrected-source hosted acceptance**, after complete local preflight. It is
not a transient retry or a retroactive pass. No further hosted attempt is
authorized here if actual host semantics, required consumers or existing bounds
fail; retain the new failure and halt with its concrete next action.

New evidence: complete job/step audit of run 41056151292 attempt 1 shows all six
native builds, ASan, BE32 and advisory library failing the same stale BMP source
path before compilation; hygiene fails exactly two comparator format lines.
Quality rejects missing producer artifacts and CI required rejects failed needs.
There is no host-driver verdict because RetroArch never executes. The complete
response audit (`gh run view 41056151292 --json
attempt,status,conclusion,headSha,url,jobs` then existing `check_ci_run.cmake`
with exact bf21fc3 SHA) exits 1 as required; transport exits 0. Logs stay scratch.

Doing nothing preserves an honest unverified halt. Changing host input seams or
raising quotas has no new supporting evidence. The smallest useful action is to
preflight corrected direct consumers, close the independently reproduced oracle
gap below, and run the existing CI once. A bounded independent read-only review
found no ownership, oracle or bound defect in this choice. Any actual-host semantic
failure overturns feasibility and stops recovery; no blind retry follows.

### Semantic correction and local evidence

The old independent predicate accepted equally wrong Up/button-2 images on both
operands (exit 0), because it sampled SWCHA and the ORed INPT4 alone. The cartridge
already exposes distinct INPT0/1 values. A shared pure `tests/input/pixels.c` judge
now independently checks all eight register bands, exact dimensions and driven
INPT D7. This is C test semantics shared with the comparator, not host automation
outside its bound. Existing `input.proline_one_button` uses actual cartridge frames
to require held/released numeric values, an idle unrelated port, and named
missing/wrong-port/wrong-button/stuck-release/pixel/dimension fault rejection.
No fixture mode, inventory item, installed surface or dependency was added.

- Replaying all three CI standalone `cc -std=c17 -O2 -Wall -Wextra -Werror`
  commands with the local library into `build/recovery-preflight`, then generating
  `input.a78` and running `proline_one_button`: exit 0, N/A-local. All direct
  consumers include the shared oracle/palette; the diagnostic CMake target builds.
- `ctest --test-dir build/ci -R
  '^(input[.]|runner[.]movie[.]replay$|libretro[.]host[.]input$)'
  --no-tests=error --output-on-failure --output-junit phase12-input.xml`: exit 0,
  19/19. Focused report is separate from full inventory evidence.
- Old comparator compiled from b360017 accepts the wrong-button image twice
  (exit 0); corrected comparator rejects it with `INPT0 mismatch` (exit 1), and
  accepts the actual held image (exit 0). This counterfactual preserves both
  observations without reverting shared working files.
- Existing unsafe-input commands with missing/directory/nested schedules: each
  refused with exit 1 before cleanup, retained sentinel; control harness exit 0.
- First fresh full local workflow exited 8 solely on added prose exceeding
  `docs/testing.md`'s unchanged 40960-byte ceiling. Condensed host documentation;
  absolute-root budget check now exits 0. No ceiling changed.
- Fresh `cmake --workflow --preset ci`: exit 0, all 673 normal tests, three ZIPs
  and 6 release tests. `tests/retroarch/*` totals 268 lines; two cases, 50-second
  launch and five-minute job bounds remain unchanged. Run URL N/A-local.

### Revised acceptance receipt: semantic halt

The one corrected-source acceptance is **failed and spent**. Command
`gh run watch 41062201311 --exit-status` exits 1.
[Run 41062201311](https://github.com/szTheory/muhtari/actions/runs/41062201311),
attempt 1, tests source `28e13e1cb7da122d72d1c83d438861001b503504`.
This is a distinct corrected-source run, not a rerun of the original attempt.

| Identity | Verified value |
| --- | --- |
| Actual host job | `124101716926`, failure in held/released input step |
| App | RetroArch 1.22.2, git `69a4f0ea` |
| DMG SHA256 | `81b79121ba26d539064ae13b4d0419a120c3d165afbe656cf5f5412b15fdb434` |
| Candidate archive | `muhtari-libretro-macos-arm64.zip`, artifact `12981635606` |
| Candidate ZIP SHA256 | `2cfdd05fd0314ae960b67d53e1a02d4c9905793499af39c753d7b9ea77b12fd7` |
| Owned screenshots | `retroarch-frames`, artifact `12984065197` |
| Frames artifact SHA256 | `627468475c06a3ba6be7abf414e886f3a3a5a1b63e35b910b2bcc3cd88228b7e` |

The actual job validates the app version/DMG digest, extracts the exact candidate
ZIP with the digest above, and builds all three standalone consumers. DMA runs
120 frames successfully: retained hash
`145799ed1868d43e10f18fdcd1529120274fe5c142957962760c02da6cb67955`
matches the ledger and every screenshot pixel equals the runner. The held-input
launch runs 40 frames, exits 0 and writes its 320×242 screenshot. The independent
comparison then exits 1 with `input-control: SWCHA mismatch`.

The agent downloaded and visually inspected both held PNGs. `sips` conversion
and standard-library BMP/palette sampling exit 0 and establish these values at
the independently specified band samples:

| Sample | Actual host held | Expected runner held |
| --- | --- | --- |
| SWCHA | FF | EF |
| SWCHB | 3F | 3F |
| INPT0 | 08 (D7 low) | 88 (D7 high) |
| INPT1/2/3 | 09/0A/0B (D7 low) | 09/0A/0B (D7 low) |
| INPT4 | 8C (D7 high) | 0C (D7 low) |
| INPT5 | 8D (D7 high) | 8D (D7 high) |

Replaying `build/ci/tests/muhtari-test-retroarch-compare --input-controls held`
on the downloaded runner/host BMPs exits 1; `--input-controls released` on the
actual held image twice exits 0. The host image independently matches idle, so
neither scheduled Up nor button 1 reached this visible observation. The exact
driver/config/event cause is **not diagnosed**. Existing failure output cats only
DMA/battery logs; its `mfi` driver messages belong to DMA, not the input launch.
The input launch log/config were not uploaded, so they cannot establish whether
the test driver is unavailable, unselected or receiving an unusable schedule.

The fail-fast step never reaches the released iteration. The battery roundtrip,
unsafe-path host controls and final DMA flipped-pixel step are skipped. Their
source remains intact and local C tests pass, but this run supplies no current
hosted result for those skipped controls. The host job lasts 32s; no line/case/
process/job bound was exceeded or raised.

Complete terminal response command `gh run view 41062201311 --json
attempt,status,conclusion,headSha,url,jobs` exits 0. Running
`cmake -DINPUT=/tmp/muhtari-recovery-run.json
-DEXPECTED_SHA=28e13e1cb7da122d72d1c83d438861001b503504
-P tools/check_ci_run.cmake` exits 1. Every job was audited: all six native builds,
ASan, hygiene, BE32, both nofp lanes, dispatch perf, quality, native hash equality,
advisory library and advisory WASM succeed; title is the allowed dispatch skip.
WASM job `124101717068` completes 604/604 with exact inventory. The only failed
jobs are actual RetroArch `124101716926` and aggregate `124102011116`; the latter's
log identifies RetroArch as the sole failed need. Advisory status was preserved.
Completed-job logs were fetched with `gh api --allow-escape-sequences
repos/szTheory/muhtari/actions/jobs/JOB_ID/logs` into scratch (exit 0); this avoids
the local `gh run view --log` cache-path write refusal. No logs/media are committed.

### Durable stopping point

ENGINEERING §11 finding: the build-consumer fault is repaired, but the pinned
test-joypad seam has now actually failed its intended held-input oracle. Preserved
timeouts and test bounds are not the failure. No second revised acceptance,
same-source retry, alternative seam, owner test or quota change is authorized by
this receipt. Task-2 independent C oracles/docs are implemented and locally/native
verified; the full host acceptance remains unmet, so neither task closes the plan.

Next recovery action: in a separate scoped debug session, inspect the pinned
released binary's test-joypad availability and official configuration/event
contract, and include the actual input launch's log/config in existing failure
evidence before any newly authorized host feasibility test. Decide a concrete
viable correction or seam within the retained bounds, or retain the boundary
halt. Do not repeat this schedule without new evidence and an explicit scoped
decision. No GPL/LGPL/unlicensed emulator source is opened.

Accepted **0/4**; PLAY-03 stays pending and 12-02 through 12-04 stay blocked.
Candidate results establish no published-byte or actual Playstead acceptance.
Dispatch perf remains same-head repeatability; PR-base comparison is still later.
Next separate command: `$gsd-debug continue phase12-packaged-input-tracer`.

### Recovery orchestrator handoff

Fresh complete run response saved as `/tmp/muhtari-recovery-final-run.json`
with `gh run view 41062201311 --repo szTheory/muhtari --json
headSha,status,conclusion,attempt,url,jobs` exits0. Existing `check_ci_run.cmake`
against that response and exact source28e13e1 exits1, naming only allowed title
skip, RetroArch failure and required-aggregate failure.
[Run41062201311](https://github.com/szTheory/muhtari/actions/runs/41062201311).
Fresh focused19-test CTest with distinct `phase12-final-input.xml` exits0.
Replaying downloaded images through `--input-controls held` exits1 with SWCHA
mismatch; the idle/released predicate on the same host image twice exits0.
Local replay URLs N/A-local. Absolute-root docs budgets and `git diff --check`
exit0. No code changed after hosted acceptance; STATE/ROADMAP retain0/4 and
all dependents blocked. Active debug checkpoint preserves the undiagnosed cause
and spent gates for the separate continuation; no later GSD step ran.
