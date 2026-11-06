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
  commits: 24
commits: 24
plan_head_before: 0ca7c9aa92152b09a277388e0fb2c977601016fa
plan_head_after: 55f30d64ad1999231a97ea05a96e6a29d061c116
requirements-completed: []
coverage:
  - id: PLAY-03-packaged-host-input
    description: The exact candidate archive passes held/released input pixel checks in pinned RetroArch while retaining existing DMA and battery host checks.
    requirement: PLAY-03
    verification:
      kind: e2e
      ref: "Latest diagnostic run 41208847091 failed before RetroArch (zero host steps); https://github.com/szTheory/muhtari/actions/runs/41208847091; prior actual host failure 41062201311 and original pre-host failure 41056151292 retained below"
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

Latest approved local repair (2026-10-31): the [narrow observer continuation](../../debug/phase12-portability-repair-proposal.md#completed-observer-continuation-and-local-repair-2026-10-31)
finished remaining fault consumers, unmodified full local workflow and sole
independent repair review. The two C repairs remain reviewed/uncommitted;
optional consumer unchanged, no source edits this continuation. Permitted
owned-PID observation confirms cleanup; child/parent omissions and capacity/
guard faults are detected. Full workflow0/about54s, original677+6 inventories
0/0, three-package diagnostic exclusions/format/hygiene/budgets/diff0; review
PASS/no findings and zero correction rounds. Prior11compiler/native/focused
passes and old supervisor failure remain immutable, with no passed-stage replay
or compiler reserve used. Fresh complete hosted receipts reconcile0; both
required audits remain1. This accepts local test repairs only, not Linux runtime
or original-host input acceptance. Final scoped handoff0 preserves exact two
C/five planning paths, optional consumer/HEAD/index/all unrelated bytes/old
receipts and halt; no owned child/container remains. This step is stopped.
Context may be cleared; exact next separate command
`$gsd-debug continue phase12-packaged-input-tracer` for read-only preparation
of a concrete publication/diagnostic proposal. Agent owns fresh checks; no
owner verification or acknowledgment is needed to keep the halt. Accepted0/4,
PLAY-03 pending/downstream blocks, both failed audits and all spent allowances
persist; no live/publication/archive/promotion or allowance renewal follows.
Earlier latest/next-action entries below are historical where superseded.

Latest read-only continuation (2026-10-31): the
[narrower tooling-only proposal](../../debug/phase12-portability-repair-proposal.md#narrower-tooling-verification-decision-2026-10-31)
is ready for a NEW scope decision. Fresh complete original/latest receipts and
every executed log reconcile0; both required audits remain1, source/index and
protected bytes are unchanged, service operational/resolved/noPR/noactiveCI.
Primary glibc headers explain the cached compiler's missing annotation, while
hosted logs do not establish an exact effective fortification level. One
independent bounded tooling specialist supports a fixed unchanged-source
baseline/fortify2 comparison, with unchanged warnings and no download.
No container/compiler/target or source repair occurred. Proposed success stops
before repair/runtime/full workflow; original hosted cause remains unknown.
Await the owner's tooling-only decision. Context may be cleared; exact next
separate command `$gsd-debug continue phase12-packaged-input-tracer` with that
decision. Agent owns later checks and comparison if approved; no owner
verification or acknowledgment is required to preserve the halt. Accepted0/4,
PLAY-03 pending, downstream blocks and spent allowances remain unchanged.
Earlier read-only/approval entries below are historical where superseded.

Latest local execution (2026-10-31): the approved
[portability batch](../../debug/phase12-portability-repair-proposal.md#local-execution-stopped-before-repair-2026-10-31)
stopped before source repair at its mandatory compiler-discrimination gate.
Fresh complete receipts exit0, both required hosted audits remain1. Cached
immutable offline GCC15.3.0/glibc2.41 reproduces all three helper truncation
errors (object1) but compiles the old ignored write (object0); effective
fortification0 leaves its declaration without warn_unused_result. All three
test hashes remain frozen. No source edit, warning/flag/fortification tuning,
NEW comparison, runtime verification, full workflow or independent repair review
followed. Original actual-host cause remains unresolved, accepted0/4 and
PLAY-03 pending; downstream halt and spent allowances persist automatically.
Context may be cleared. Exact next separate command
`$gsd-debug continue phase12-packaged-input-tracer` prepares a narrower tooling
proposal read-only; next agent owns fresh checks. No owner verification or
acknowledgment is needed to retain stop, and no source/publication/live action,
download/install, allowance renewal, archive or phase promotion is authorized.
Earlier next-session implementation wording below is historical.

Owner approval recorded (2026-10-31): "Yes, I approve it." authorizes the exact
[local portability-repair batch](../../debug/phase12-portability-repair-proposal.md)
for the next session. Agent-owned fresh checks, implementation, offline
verification and one bounded review require no further scope confirmation.
No implementation occurred in this completed read-only step, and no
publication/live allowance, requirement completion or phase acceptance changes.
Earlier awaiting-decision wording below is historical.

Latest read-only follow-up (2026-10-31): the
[local portability-repair proposal](../../debug/phase12-portability-repair-proposal.md)
is ready for a new local-only scope decision. Fresh complete original/latest
receipts and all executed logs agree; required audits remain failed, source
and index are preserved, and no host observation was added. The proposal fixes
the two test-source blockers and verifies them through the existing offline
consumer, native workflow and a cached immutable GCC image whose actual fault
sensitivity remains untested. No source, target, container or live action was
performed; no allowance, requirement completion or phase acceptance changes.
The agent owns later checks; the owner acts next only on the proposed scope.

Latest continuation (2026-10-31): the approved frozen17-path source was
published as commit `55f30d64ad1999231a97ea05a96e6a29d061c116` and dispatched
once as [run41208847091](https://github.com/szTheory/muhtari/actions/runs/41208847091).
The terminal run failed: Linux x64/arm64 builds reject an unchecked `write`,
and x64 also rejects potentially truncated fixture paths. The required
aggregate failed; RetroArch had zero executed steps, so there was no host
preflight, child, trace or image. Mac and Windows producers passed, including
the arm64 diagnostic artifact, but packaged-host acceptance remains unmet.
The original failed receipts, PLAY-03 pending state, accepted0/4 and downstream
halt remain. No repair, retry, archive or promotion authority remains.
The approved local wiring and its offline checks are recorded in the
[proposal](../../debug/phase12-hosted-input-tracer-proposal.md#approved-local-wiring-outcome-2026-10-31);
the full publication/run audit and frozen hashes are in the
[terminal outcome](../../debug/phase12-hosted-input-tracer-proposal.md#approved-frozen-publication-terminal-outcome-2026-10-31).
Next separate command is `$gsd-debug continue phase12-packaged-input-tracer`
for a read-only narrow portability-repair proposal after fresh checks. No
owner verification is needed to preserve the halt.

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

## Keyboard feasibility continuation: halt retained (2026-10-30)

Later inspection of the exact pinned released executable established that the
optional test joypad is absent. The launch reporter now retains each actual
launch's config/log rather than attributing DMA logs to input. These findings do
not make either historical failed receipt pass.

The owner authorized pinned-release keyboard feasibility: nonprompting access
preflight under the intended hosted identity, no permission request/grant/change,
and at most **one actual RetroArch content process**. Local denial was not treated
as a hosted verdict. The keyboard scope produced **zero content processes**:
permission succeeded, but two successive producer faults prevented the host job.
The final live window is spent; no further dispatch, rerun, owner test or later
plan is authorized by this handoff. Keeping the halt needs no acknowledgment.

### Hosted receipts and complete audits

All three runs below are attempt1 and are preserved as failures. Each complete
`gh run view RUN --json attempt,status,conclusion,headSha,url,jobs` response was
retrieved with exit0. The existing `tools/check_ci_run.cmake` consumer, with its
matching full `EXPECTED_SHA`, exits1 for each failed required aggregate. Direct
completed-job logs were retrieved with `gh api --allow-escape-sequences
repos/szTheory/muhtari/actions/jobs/JOB/logs` (exit0). Logs and responses remain
scratch; no logs, ROM bytes or screenshots are committed.

| Scope/run | Exact source | Decisive outcome |
| --- | --- | --- |
| [Permission only,41075600234](https://github.com/szTheory/muhtari/actions/runs/41075600234) | `cb68bd0c6fe1d83ab1165479b4242256219de674` | Actual macos-15-arm64 preflight reports permitted, then intentionally exits1 before all host/content operations |
| [First content dispatch,41086569269](https://github.com/szTheory/muhtari/actions/runs/41086569269) | `a7aa88994f6f2d5df5c3dcc9c22cc1887bd76fc5` | Both Windows producers reject an unreachable child-exec fallback; RetroArch skips |
| [Final corrected dispatch,41090838885](https://github.com/szTheory/muhtari/actions/runs/41090838885) | `abc4f570d2ccc6d7198c41d47ba0a1ba6797decf` | Both Windows policy checks fail; Intel Mac independently times out; RetroArch skips |

Permission-only [job124150451124](https://github.com/szTheory/muhtari/actions/runs/41075600234/job/124150451124)
uses macos-15-arm64 image20261028.0489.1. Its native helper returns0 with
`post-event-access=permitted`; the explicit permission-only stop then exits1.
Every artifact extraction, app install, DMA, input and battery step skips.
The reporter confirms no launch evidence. All other jobs succeed apart from the
allowed title skip and required aggregate. This establishes permission only.

In41086569269, Windows x64/arm64 jobs124169251845/124169251932 report MSVC
C4702 at the helper's `_exit(127)`: the test mock for `execv` always aborted,
incorrectly making a failed exec nonreturning. Quality rejects the missing exact
producer artifact set; the required aggregate rejects failed build/quality and
skipped dependencies. All other executed jobs succeed, including advisory
library. RetroArch, advisory WASM and equality skip. No new permission or input
verdict follows from this pre-host failure.

The manager authorized one final corrected-source dispatch within the still
unconsumed one-process scope. Genuine code/test commits39879d8 andabc4f57 correct
the mock and exercise the actual failed-child ownership boundary. The actual
dispatch parent is corrected39879d8, without history rewrite or a documentation
fence. The final allowance explicitly forbids any further dispatch, even after
another pre-host failure.

All20 jobs in41090838885 were audited before attributing the outcome:

- Windows [x64 job124184107734](https://github.com/szTheory/muhtari/actions/runs/41090838885/job/124184107734)
  and [arm64 job124184107661](https://github.com/szTheory/muhtari/actions/runs/41090838885/job/124184107661)
  compile successfully and pass the API and child-exec tests. Their sole native
  failure is `policy.retroarch.preflight_gate`: `missed short_movie:0`.
  The fault is generated on every platform, but its recipe-execution validation
  was Unix-only. Thus the MSVC correction is verified; the later portable-check
  fault is distinct.
- Intel Mac [job124184107566](https://github.com/szTheory/muhtari/actions/runs/41090838885/job/124184107566)
  independently fails only `title.pal.bios`, timeout8.72s. This required failure
  remains unresolved and blocking. Its censored duration does not calibrate a new
  deadline; no timeout, case or assertion was changed.
- Linux x64/arm64, Apple arm64, ASan, hygiene, nofp, BE32, perf and advisory
  library succeed. Quality rejects the absent exact native artifact set.
  Required aggregate fails on build/quality and skipped required dependencies.
  Actual RetroArch, advisory WASM and equality skip; advisory outcomes remain
  advisory, with no invented WASM or host success.

`gh run watch` for both content dispatches uses `--interval120 --exit-status`
and exits1 on their actual failures. The earlier permission-only observer's API
rate-limit exit was kept separate; its completed job and full audit were later
retrieved successfully. No job was rerun or canceled.

### Concrete implementation and local-only repair

The candidate keyboard path remains within the existing two ordinary host cases,
296/300 support lines, 50-second process and five-minute host-job limits. A
dispatch-only `keyboard_probe_only` branch excludes all ordinary DMA/input-loop/
battery launches, invokes one private RetroArch child, targets only that PID with
Up/Z, requests an F8 software screenshot while held, releases at5s and requires a
terminal600-frame released observation. Early exit before release fails. Raw F8
images would be retained even if a later judge failed. This path has **no actual
keyboard image or input-delivery verdict** because the host never ran.

The pure battery RGB oracle and its existing faults moved to the diagnostic test
owner, with independent executed-pixel/count/dimension faults added. No launch or
event automation moved outside the counted host consumer. Both standalone and
CMake comparator consumers were compiled locally. The existing independent
SWCHA/SWCHB/six-INPT input predicates and ordinary DMA/battery assertions remain.

Scoped review caught a one-row held runner movie before dispatch. An actual
600-frame headless replay rejects the old movie with SWCHA mismatch (exit1),
while the workflow's600-row producer passes the independent held judge (exit0).
The actual producer and one-row fault are covered by the existing policy test.
No actual RetroArch process was used for these local checks.

Local commit `7e07c4cb8890227eea1eeef1eda1a5d7cad58217` repairs the portable
policy check without a new hosted attempt. Common validation derives row count
from the actual producer's start/stop/stride and append destination, compares it
with the actual runner-frame window, and checks actual held/released bindings.
Unix recipe execution is retained. The original checker forced through its
non-Unix branch fails1; the corrected branch passes0. An equivalent shifted
600-row window passes0, while truncated/binding/window/stride faults each fail1.
This models the checker branch, not a new native Windows verdict.

Final local evidence for that repair (run URL N/A-local):

- `cmake --workflow --preset ci`: exit0,677 ordinary tests, three archives and6
  release tests; both existing exact JUnit/inventory consumers exit0.
- Focused `policy.retroarch` tests and native clang-cl-compatible fixture
  compile/run: exit0. The latter uses the native Apple SDK/ABI and is not an
  MSVC-backend substitute. Real final-run Windows results above verify the prior
  exec-mock correction separately.
- Failed-exec subprocess contract requires complete argv, exit127 and no input,
  wait or kill; wrong-argv, child-fallthrough and child-input mutations each make
  the existing expected-code/marker consumer fail1.
- `git diff --check`, `scripts/hygiene.sh --tree`, unchanged documentation budgets
  and formatting checks: exit0. Reproduced lessons remain in the local ledger.

### Preserved boundary and next action

The local portable-check repair and this append-only handoff are not a hosted
acceptance or a renewed budget. The independent Intel Mac PAL BIOS timeout also
remains blocking. Active `.planning/debug/phase12-packaged-input-tracer.md` stays
nonterminal for a genuine future scope/window decision; no owner verification or
acknowledgment is needed merely to retain the halt. No further push, dispatch,
rerun, permission change, host-pin change or later GSD step ran after the final
stop. No GPL/LGPL/unlicensed emulator source was opened or retained.

Accepted **0/4**; PLAY-03 stays pending and plans12-02 through12-04 stay blocked.
No published-byte or actual Playstead acceptance is established. A future
`$gsd-debug continue phase12-packaged-input-tracer` must begin with an explicitly
renewed host scope/window and the unresolved required Intel result; the command
alone does not renew this exhausted allowance.

## Local Intel deadline calibration after the halt

The owner agreed to follow the recommendations. This continuation investigates
and prepares local corrections only; it does not infer renewed push, dispatch,
rerun, cancel or RetroArch authority from that general agreement.

The Intel lane already applies scale2 to the PAL BIOS test's former base4s.
Historical complete current/base observations of the unchanged workload are:

| Run / Intel job | Current seconds | Base seconds |
|---|---:|---:|
| [40946368330 / 123702751369](https://github.com/szTheory/muhtari/actions/runs/40946368330/job/123702751369) | 3.05 | Not comparable: preceding phase lacks this test |
| [40956113335 / 123736612675](https://github.com/szTheory/muhtari/actions/runs/40956113335/job/123736612675) | 3.61 | 4.11 |
| [41062201311 / 124084546980](https://github.com/szTheory/muhtari/actions/runs/41062201311/job/124084546980) | 4.24 | 5.66 |
| [41086569269 / 124169251753](https://github.com/szTheory/muhtari/actions/runs/41086569269/job/124169251753) | 3.53 | 3.09 |

These seven completed observations support the existing twice-observed-max
policy: ceil(2 x5.66)=12s effective Intel deadline, hence base6s with unchanged
scale2. The later8.72s timeout remains failed and censored, excluded from that
calculation. `git diff 28e13e1..HEAD -- src tests/compat/predicate.c
tests/diag/pal.a78 tests/built/openbios.cmake tests/compat/tests.cmake` was empty
before the calibration (exit0,N/A-local); the predicate/ROM/OpenBIOS diff from
`e0dc556` was also empty. No core failure has been reproduced locally and no
claim is made that the failed Intel execution would finish within12s.

Only the owning timeout registration and its measurement comment change.
The command,120-frame predicate, BIOS fixture, workload, labels, inventory,
lane multipliers and all assertions remain. One bounded independent evidence
review initially rejected a deadline change on the smaller supplied sample,
then accepted the arithmetic and source comparability after the5.66s completed
observation was found. No new workflow, check or dependency is introduced.

Verification (all exit0, run URL N/A-local):

- `ctest --test-dir build/ci -R '^title.pal.bios$' --no-tests=error
  --output-on-failure --repeat until-fail:3`: complete predicate passes after
  calibration; before-calibration focused repetitions also pass locally.
- `cmake --preset ci` and a separate scratch configure with
  `-DMUHTARI_TIMEOUT_SCALE=2`, followed by `ctest --show-only=json-v1` and
  property assertions: timeout6/12s, same BIOS fixture and command,683 registered
  tests including release on both configurations. Scratch output remains local.
- `cmake --workflow --preset ci`:677 ordinary tests, three archives and6
  release tests pass. Both existing `check_junit.cmake` consumers verify exact
  ordinary/release inventories. Focused reports did not overwrite full JUnit.

This is local calibration, with native Intel and Windows acceptance outstanding
and keyboard delivery still unproved. Accepted0/4, PLAY-03 pending and all
dependent plan blocks remain. Prior failed receipts and the exhausted live
window remain authoritative. The next proposed decision is one push of the
batched local corrections and one `ci.yml` dispatch with
`keyboard_probe_only=true`, no rerun/cancel or repeat dispatch even after a
producer failure, at most one actual RetroArch child after nonprompting access
preflight, no permission/pin changes and no ordinary content launches. Audit
every job and retain the designed scoped stop even if keyboard pixels pass;
this would establish feasibility only, not full host or phase acceptance.
That proposal is pending owner authorization, not an active allowance.

## Renewed one-attempt receipt: native corrections pass, host skipped

The owner explicitly answered **yes** to the proposal above: one push and one
`keyboard_probe_only=true` dispatch, at most one RetroArch process and no retry,
including after a producer failure. This superseded the pending proposal only
for this one attempt. No later phase, ordinary host acceptance, permission/pin
change, rerun or cancellation was authorized.

The pushed and independently advertised source is
`a0834db21b2b22f30d2147458aeec27df6ae1530`; its genuine parent is
`753bf4acd7ec7a438dd7ce0da87bf5879ca9f0f3`. No history rewrite or extra source
fence was used. [Run41099366366](https://github.com/szTheory/muhtari/actions/runs/41099366366),
attempt1, independently reports that exact source.

Commands and outcomes:

- Fresh `cmake --workflow --preset ci`: exit0,677 ordinary tests, three archives
  and6 release tests. Both ordinary/release `check_junit.cmake` consumers exit0;
  `git diff --check` exits0. Run URL N/A-local for these pre-push checks.
- `git push origin HEAD:refs/heads/phase/12-player-ready-delivery-and-playstead-bridge`:
  exit0; `git ls-remote` independently confirms the full source above (exit0).
- `gh workflow run ci.yml --ref phase/12-player-ready-delivery-and-playstead-bridge
  -f keyboard_probe_only=true`: exit0, the single run linked above. The allowance
  was reserved before sending; no second dispatch, rerun or cancel followed.
- `gh run view 41099366366 --json attempt,status,conclusion,headSha,url,jobs`:
  exit0, completed/cancelled, exact source/attempt1, all20 jobs terminal.
  Independent source/attempt/job-count/terminal assertions exit0.
- `cmake -DINPUT=<saved-complete-response> -DEXPECTED_SHA=a0834db21b2b22f30d2147458aeec27df6ae1530
  -P tools/check_ci_run.cmake`: exit1, rejecting the failed required aggregate.
  The complete response remains scratch evidence, not a committed log.

### Every job accounted for

- [Intel Mac124213483910](https://github.com/szTheory/muhtari/actions/runs/41099366366/job/124213483910)
  is **cancelled**. Its direct log passes current677+6 and independent-base677+6;
  `title.pal.bios` completes in4.85s/current and4.34s/base. Artifact uploads and
  orphan cleanup end17:43:29Z after a17:30:23Z job start. Its check annotation
  nevertheless says `The job has exceeded the maximum execution time of 19m0s`,
  with completed_at17:50:00Z. This outer-job failure remains required and
  blocking; passing test logs do not turn the cancelled job green.
- [Windows x64124213483752](https://github.com/szTheory/muhtari/actions/runs/41099366366/job/124213483752)
  and [Windows arm64124213483909](https://github.com/szTheory/muhtari/actions/runs/41099366366/job/124213483909)
  are successful. Both current/base677+6 workflows and API/child-failure/portable
  preflight checks pass. This supplies the previously missing native verdict
  for the portable movie-span repair, retaining its faults and strict warnings.
- Apple arm64, Linux x64/arm64, hygiene, ASan, both nofp jobs, PowerPC compile,
  changes, perf and advisory library succeed. The dispatch perf mode repeats
  the head only; it establishes no phase-versus-main performance improvement.
- [Quality124227569556](https://github.com/szTheory/muhtari/actions/runs/41099366366/job/124227569556)
  succeeds with the required native/current-base observations. Its native base
  is the genuine parent above; comparable perf is not requested in this dispatch.
- [CI required124227785689](https://github.com/szTheory/muhtari/actions/runs/41099366366/job/124227785689)
  fails, exit1. Its actual `NEEDS` names build/cancelled, hash-equality/skipped
  and retroarch/skipped; the remaining required consumers succeed. Title's
  non-PR skip is allowed. Advisory WASM skips and stays advisory.
- [RetroArch124228103951](https://github.com/szTheory/muhtari/actions/runs/41099366366/job/124228103951)
  skips before preflight/content. There is **no new actual RetroArch process,
  keyboard screenshot or delivery verdict**. Hash equality also skips, so no
  new equality acceptance is inferred from the native artifacts alone.

All direct Intel/Windows/quality/aggregate log reads and Intel check annotation
reads exit0. Both read-only `gh run watch --interval120 --exit-status` observers
exit1 on GitHub job-list502 errors; those are observer failures, separate from
the final run verdict. API status also lagged completed executor logs before
the complete terminal response became available.

GitHub's [Actions/Webhooks incident](https://www.githubstatus.com/incidents/pwxsjgnvrk6b)
was reported at17:50Z during this attempt. This supports an infrastructure
hypothesis for the contradictory completion/cancellation evidence; the specific
causal mechanism is **not established**. No timeout was raised from this
cancelled job, no failure was waived and no service recovery was assumed.

### Preserved stop

The Windows portable correction and Intel predicate calibration now have direct
native passing evidence. Historical failures remain failed receipts. Actual
keyboard feasibility and ordinary DMA/input/battery host acceptance remain
unproved. Accepted **0/4**, PLAY-03 pending and plans12-02 through12-04 blocked.

The one push and one dispatch are spent, even though this attempt launched zero
content processes. No further push, dispatch, rerun or cancel is authorized.
This local receipt and STATE handoff stay unpushed; the active debug session
stays nonterminal/untracked and the original three planning edits remain intact.
No acknowledgment is needed merely to preserve the halt. Next separate command:
`$gsd-debug continue phase12-packaged-input-tracer`, beginning with service-status
and receipt reconciliation; another live attempt needs an explicit new bounded
scope/window after the external incident is assessed. No later GSD step ran.

## Targeted retry receipt: Intel cancels again after service recovery

The owner explicitly answered **yes** to one targeted Intel retry plus its five
automatic dependent jobs at the original source and inputs. The existing limits
remained: at most44 summed runner-minutes excluding queue, one50s RetroArch child
after nonprompting access preflight, no further retry or cancellation, and no
deadline, permission, pin or phase-acceptance change. The sole submission was
reserved before sending. No push or new dispatch occurred.

Commands and outcomes for
[attempt2](https://github.com/szTheory/muhtari/actions/runs/41099366366/attempts/2):

- Fresh official status/incident `curl --fail --silent --show-error --max-time 30`
  reads: exit0 each. Actions was operational; incident `pwxsjgnvrk6b` was resolved
  at19:23:40UTC. Exact source/attempt1/job/artifact/workflow reads and scope
  assertions: exit0. Original event inputs bind the durable dispatch command
  (`keyboard_probe_only=true`), not an independently exposed fresh API payload.
- `gh run rerun 41099366366 --repo szTheory/muhtari --job 124213483910`:
  exit0, the sole live command. Exact source remains
  `a0834db21b2b22f30d2147458aeec27df6ae1530`, attempt2.
- `gh run view 41099366366 --repo szTheory/muhtari --json
  headSha,attempt,url,status,conclusion,jobs` and attempt-specific job/attempt
  APIs: exit0. `python3 /tmp/muhtari-targeted-retry.AyJcIO/audit.py`: exit0;
  all20 terminal jobs reconcile, with14success/4skipped/1cancelled/1failure.
  Fourteen peer receipts have new IDs but identical original completed times,
  runner identities and steps; these are copied results, not new executions.
  The initial name-only scope assertion rejected copied IDs; the corrected
  execution-time/step assertion passes without another live action.
- `cmake -DINPUT=<complete-attempt2-response>
  -DEXPECTED_SHA=a0834db21b2b22f30d2147458aeec27df6ae1530
  -P tools/check_ci_run.cmake`: exit1, correctly rejecting required CI failure.
- Direct Intel, quality and required-aggregate logs plus Intel check/annotations:
  exit0 each. A log read while Intel was still active returned404/exit1; the
  terminal log read succeeds. Raw logs/responses remain scratch only.

### Every new execution and retained artifact accounted for

- [Intel124275528706](https://github.com/szTheory/muhtari/actions/runs/41099366366/job/124275528706)
  is cancelled. Current677+6 passes, packages and exact inventories pass; base677+6
  also passes and packages. Ordinary-suite real times are533.41s/current and
  545.41s/base; PAL BIOS7.10s/6.03s passes. No individual test failure appears.
  The current workflow step runs19:43:21–19:53:20UTC; the base/quality step is
  cancelled at20:03:11UTC after base release inventory output. The annotation
  names the19m job limit. Quality production does not complete; all three
  artifact uploads skip. Complete tests are not a completed native producer.
- [Quality124284846572](https://github.com/szTheory/muhtari/actions/runs/41099366366/job/124284846572)
  succeeds using the retained original same-source observations. All20 original
  artifact IDs/digests/sizes remain unchanged/nonexpired, with no new artifact:
  six archives, six hashes and eight quality. This is not fresh Intel production
  or evidence that the cancelled job succeeded.
- [Required aggregate124284917898](https://github.com/szTheory/muhtari/actions/runs/41099366366/job/124284917898)
  fails, exit1, on build/cancelled and required RetroArch/hash-equality skips.
  Advisory WASM skips and remains advisory. Title remains an allowed non-PR skip.
- [RetroArch124284849352](https://github.com/szTheory/muhtari/actions/runs/41099366366/job/124284849352),
  hash-equality and WASM skip. No access preflight, actual RetroArch child,
  screenshot or keyboard delivery verdict occurs in this attempt.

Official health still reports operational while this retry cancels. The former
transient-only hypothesis is insufficient as a recovery strategy. This attempt
demonstrates outer-budget exhaustion during real unfinished producer work;
underlying slowdown and the first attempt's distinct completion discrepancy
remain unexplained. The cancelled whole job is a censored timing sample, not a
complete-job measurement for selecting a new deadline. The next local review
should calibrate the Intel-only outer budget from complete comparable producer
work under DEC.47, preserving all tests, required joins and failure detection.
Do not widen unrelated lanes or request another identical retry.

The single targeted allowance is spent. Accepted0/4, PLAY-03 pending and all
dependent blocks remain. No source/deadline/pin/permission change, ordinary host
launch, additional retry/cancel or later GSD step occurred. The receipt stays
unpushed; the debug checkpoint stays nonterminal. No acknowledgment is needed
to preserve the stop. Context may be cleared; next separate command:
`$gsd-debug continue phase12-packaged-input-tracer`. The agent owns the local
budget review and receipt checks; that command grants no live allowance.

Local handoff validation: `sh scripts/hygiene.sh --tree`,
`cmake -DROOT="$PWD" -P tests/cmake/budgets.cmake`, `git diff --check` and
`node <GSD_TOOLS> query summary-extract <this-summary> --fields requirements_completed`
each exit0 (run URL N/A-local). Summary completion remains empty; append-only
history, original planning edits, exact tested source, blocked counts and spent
allowance assertions pass. No code or workflow behavior changed in this session.

## Read-only Intel outer-budget review

The [focused review](../../debug/phase12-intel-budget-review.md) selected40
distinct successful complete Intel jobs from72 inspected latest-job lists within
100 newest CI run records. The maximum1085s is
[run40783431377/job123150624076](https://github.com/szTheory/muhtari/actions/runs/40783431377/job/123150624076),
giving37m under DEC.47. Direct maximum/newest successful logs establish complete
current/base tests, quality production and uploads. Actual maximum merge-tree
identity, image revision/compiler and unchanged producer contracts are checked.
The maximum ran662+6; the newest success ran676+6. No complete successful677+6
job exists, so this supports historical reliability margin, provisional for the
expanded workload. Neither cancelled attempt calibrates the bound. Attempt1's
completion discrepancy, slowdown cause and actual keyboard delivery remain
unproved. One other historical log returned503 twice; its contents are not inferred.

`python3 /tmp/muhtari-intel-budget.VY2XZc/audit.py` exits0 for selected sample
ordering, complete producer receipts, arithmetic, source comparisons and available
direct logs; the linked report retains command/exit/run evidence. Raw responses
and logs stay scratch only. Fresh official status API reports operational; no
causal claim follows. No source/workflow change, commit or remote mutation occurred.

The next proposed scope is local-only Intel37m timeout/provenance correction,
existing offline consumers, full local workflow and bounded review. All other
deadlines, tests and joins stay unchanged. The saved stop explicitly excludes
deadline changes, so that local correction awaits owner authorization. A future
live request is separate and must account for18 additional Intel runner-minutes;
no new live total or window is proposed here. Accepted0/4, pending requirements,
downstream blocks and every failed receipt remain authoritative.

Waiting for the owner on this new local scope decision. Context may be cleared;
after authorization the exact next command is
`$gsd-debug continue phase12-packaged-input-tracer`. The agent owns implementation,
verification and service-health/receipt checks; the command grants no live allowance.

## Approved local Intel deadline correction

The owner answered **"yes i approve"** to the Intel-only19m-to37m timeout/comment
correction, existing offline checks/full workflow and one bounded review. This
supersedes only the prior local deadline-change stop. The implementation in
`.github/workflows/ci.yml` gives Intel37m, Windows68m and other native lanes19m;
the comment cites1085s/n=40/run40783431377 and preserves provisional status for
the expanded workload. Commands, tests, per-test timeouts/scales, matrix, producer,
joins, pins and permissions are unchanged. The
[appended verification receipt](../../debug/phase12-intel-budget-review.md)
contains the exact commands/exits and historical source/run evidence.

Local commands, all exit0, run URL N/A-local:

- `python3 /tmp/muhtari-intel-local.748pDp/verify.py`: actual six-lane expression,
  old/wrong-runner/changed-Windows-budget variants, complete workflow comparison,
  original planning edits, canonical prefix and blocked-count assertions.
- `cmake -DDIR=.github/workflows -P tests/cmake/timeouts.cmake`, existing
  `hygiene.timeout_comments` plus self-test, and `policy.ci_run.controls` plus
  `policy.retroarch.preflight_gate`: existing pass/fault consumers; focused JUnit
  reports stay scratch and separate from full inventories.
- `actionlint -shellcheck= -ignore 'label ".*" is unknown'
  .github/workflows/ci.yml`: workflow schema/expression check, with only inherited
  unsupported labels/shellcheck diagnostics excluded.
- `python3 /tmp/muhtari-intel-local.748pDp/workflow.py`: foreground
  `cmake --workflow --preset ci` with four-worker FIFO jobserver; full677 ordinary
  tests, packaging and6 release tests pass. Both exact `check_junit.cmake`
  consumers pass with the existing `tests/inventory/ci.txt` and ordinary/release
  filters, proving every required test ran exactly once.
- Existing document budgets, tree hygiene and `git diff --check` pass. One bounded
  independent diff review returns PASS with no substantive findings.

Only the approved local correction is complete. No push, dispatch, rerun, cancel
or actual content process occurred; no live spending window was renewed. All
failed receipts, accepted0/4, pending requirements and downstream blocks remain.
No exact-current-workload successful hosted Intel producer or keyboard delivery
is claimed, and the historical margin remains provisional for expanded work.
MU-CI-013 in LESSONS records the reproduced pooled-budget limitation and scope.

Context may be cleared. Next separate command:
`$gsd-debug continue phase12-packaged-input-tracer`. The agent owns read-only
preparation of a concrete corrected-source/parent, dependent-job closure, runner
budget/window and content proposal, plus fresh service-health/receipt checks;
no owner review is a prerequisite. A later live proposal must account for18 extra
Intel runner-minutes and receive separate authority. This command grants none.

## Approved corrected-source keyboard feasibility result

Owner approved the [concrete scope](../../debug/phase12-live-proposal.md): one
existing-source push, one probe-only dispatch,319 configured runner-minutes,
120m queue-inclusive cancellation-request cutoff plus10m observation, and one
50s keyboard process after nonprompting preflight. Exact push and dispatch each
exit0; remote/run independently bind186c0cc4b39c12fb149ddc0ef3f43897fba55865,
genuine parentd8dd74b6598da71e30434dad380b79c2d784aa27.
[Run41133992538](https://github.com/szTheory/muhtari/actions/runs/41133992538)
attempt1 completes/failure; all20jobs reconcile16success/1allowedskip/3failure.
All six native current/base677+6 producers/uploads, quality and equality pass.
Intel's complete1107s supports37m under DEC.47. Windows-arm64's complete2207s
is new calibration evidence:68m lacks twice-this-sample margin (74m arithmetic).
No deadline change or phase-versus-main performance claim follows.

The actual host passes fresh permission preflight and exact pinned package/core
identity. One PID-targeted child logs Up/Z down/up and completes600frames, saving
only the final released PNG. Agent visual inspection and the existing independent
released-pixel predicate pass0. No F8 held screenshot exists, so the probe fails
before held/released comparisons and the deliberate scope-only stop. Event
posting and released pixels do not establish held delivery; missing-capture cause
remains unproved. Ordinary DMA/input-loop/battery launches skip as scoped.
WASM separately fails child_failure (expected127/runtime1;605of606pass); it
remains advisory. The required aggregate fails solely on RetroArch.

Evidence: `python3 /tmp/muhtari-live-exec.y0WqX4/terminal.py` exit0 reconciles
source/attempt/all jobs,19executed logs and21exact-source artifacts;
`media.py` exit0 checks archive/frame integrity and released pixels.
`gh run view 41133992538 --repo szTheory/muhtari --json attempt,status,conclusion,headSha,url,jobs`
exits0. `cmake -DINPUT=/tmp/muhtari-live-exec.y0WqX4/terminal-view.out
-DEXPECTED_SHA=186c0cc4b39c12fb149ddc0ef3f43897fba55865
-P tools/check_ci_run.cmake` exits1, preserving required failure.
Raw logs/media remain scratch; linked scope retains artifact digests and receipts.

Push, submission and one-process allowances are spent. Cancellation was unused
and expires with this terminal run. No retry, source/deadline/pin/permission
change, full acceptance or phase promotion is authorized. Accepted0/4 and plans
12-02 through12-04 blocked remain unchanged; every historical failure above stays.
Context may be cleared. Next separate command:
`$gsd-debug continue phase12-packaged-input-tracer`. Agent owns read-only
missing-held-evidence diagnosis, Windows calibration preparation and health/
receipt checks. No owner verification or acknowledgment is needed; no live
allowance is renewed. Local handoff edits remain uncommitted.

Final handoff checks: `git diff --check`, absolute-root document budgets,
`sh scripts/hygiene.sh --tree` and the existing summary-extract consumer each
exit0, N/A-local. Source/workflow/original-edit hash assertions and append-only
summary/accepted-count assertions exit0. Fresh canonical run view exits0;
`check_ci_run.cmake` against parent-final-view.json still exits1. This validates
the halted handoff, not plan acceptance.

## Read-only held-capture diagnosis and Windows calibration preparation

The [diagnostic report](../../debug/phase12-held-capture-diagnosis.md) preserves
the failed run and separates the missing capture from its unproved cause. The
exact pinned released binary maps the helper's keys correctly. Its single100ms
F8 pulse has no readiness or capture acknowledgment; event ordering, routing and
effective hotkey gating remain possible causes. No host process was launched.

Latest40 complete comparable Windows-arm64 executions include a2324s successful
whole job, supporting78m under DEC.47 rather than latest-only74m. Existing x6468m
already exceeds its selected margin. Historical workloads have fewer cases;
calibration remains provisional for expanded work. No deadline changed.

`python3 /tmp/muhtari-held-diagnosis.lOayAx/refresh.py --fresh` exits0:
primary health operational, incident resolved, exact latest source/attempt/all20
jobs/all21 artifacts unchanged. Existing required audit exits1 solely on the
host-blocked aggregate; WASM stays advisory. `windows_review.py` and
`static_keys.py` in the same scratch directory each exit0; the linked report
records commands and direct job URLs. Raw logs and disassembly stay scratch-only.

Recommend a separately authorized local-only capture acknowledgment within the
existing held/process bounds, monotonic phase timing and existing deterministic
mock/wrapper coverage, plus arm64-only68to78m provenance/calibration checks.
This is a proposed experiment protocol, not a proven input fix. No code/workflow,
pin/permission, commit or remote mutation occurred. Accepted0/4, downstream
blocks and all spent allowances remain. Waiting for the owner's scope decision;
no owner verification is needed. Context may be cleared. Exact next command:
`$gsd-debug continue phase12-packaged-input-tracer`; invocation alone renews
no source, deadline or live authority.

Parent handoff verification: `windows_review.py`, `static_keys.py` and cached
receipt reconciliation `refresh.py` each exit0; required aggregate remains1.
Absolute-root document budgets exit0 (22files). Source/workflow paths have no
diff from the same HEAD; no acceptance or live authorization is claimed.

## Local capture acknowledgment implementation; support cap still blocks

The owner's exact continuation was "I accidentally quit my terminal. Please
continue." This was interpreted narrowly as proceeding with the previously
presented local-only capture/timing/tests and arm64 calibration scope, without
renewing live allowances or authorizing another contract change.

The helper now waits for exactly one isolated raw capture within the existing
held window, repeats F8-down until acknowledgment, sends F8-up once, records
monotonic timing and retains owned-child cleanup and all runtime/frame bounds.
The wrapper validates explicit raw-shot/terminal separation before cleanup.
Existing mock/wrapper/policy consumers reject delayed/missing/multiple/vanishing
capture, argv, release, child-exec/exit and clock faults. Only Windowsarm64 changes
68to78m from the complete historical2324s maximum; other lanes/scales/joins stay.
Historical workload differences and the unproved actual-host cause remain.

`python3 /tmp/muhtari-capture-local.8_5_au7r/workflow.py` invokes the full
`cmake --workflow --preset ci` and exits0; exact677 ordinary/6 release inventory
consumers each exit0. Focused actual consumers and meaningful counterfactuals,
standalone compile, actual six-lane expression/faults and timeout provenance
pass. One bounded review found the cap plus two documentation omissions; the
omissions were corrected before the final workflow. The
[report](../../debug/phase12-held-capture-diagnosis.md) records all command/exit
receipts,N/A-local; raw logs remain scratch-only.

The actual required support-cap expression exits1:347lines versus300. No cap
change, packing or relocation occurred; this remains a real acceptance blocker.
Local test success is not source readiness or hosted proof. Recommend a separate
decision retiring only this numeric proxy while preserving two cases, runtime
bounds, independent judges, isolated ownership and existing architecture review;
no arbitrary replacement quota. The report contains the concrete scope and
alternatives. Waiting for the owner's cap decision, not owner verification.

Source and handoff remain uncommitted; no remote mutation, actual content launch
or phase promotion. Accepted0/4, dependent-plan blocks and every spent live
allowance remain. Context may be cleared. Exact next separate command:
`$gsd-debug continue phase12-packaged-input-tracer`; cap resolution requires
separate authority, and later live-proposal preparation is read-only.

## Approved support-ceiling retirement; local repair verified

The owner answered "yes" to the specific separate cap proposal. DEC.56 now
retires only the numeric300-line ceiling, without a replacement quota; support
totals are reported and two host cases, all runtime/frame/ownership/pixel controls
and ordinary architecture review remain binding. Active host-reference/D-06/task2
clauses and downstream coverage are synchronized; initial research has a scoped
amendment note. The historical347/300 rejection above remains failed evidence.

The only workflow change from the reviewed local repair is replacement of that
numeric test with `wc -l tests/retroarch/*`, reporting347lines. The existing
policy consumer accepts301line reporting, refuses missing inputs and rejects
restoration of the old cap, retaining the earlier preflight/capture/movie faults.
No host implementation byte or actual process/frame/pixel bound changed in this
amendment. The earlier cap review warning is resolved by explicit scope revision;
it is not a retroactive pass of the old contract.

`python3 /tmp/muhtari-cap-amendment.1L3jBd/workflow.py` invokes the full
`cmake --workflow --preset ci`, exit0; exact ordinary/release JUnit consumers
each exit0, proving677/6 tests once each. Focused actual reporting/fault policy,
forced-common-platform branch, actionlint with existing allowances, timeout
provenance and protected-source/HEAD assertions exit0. The
[report](../../debug/phase12-held-capture-diagnosis.md) records commands and
exits,N/A-local. Raw logs remain scratch-only.

Local repair and approved policy amendment are verified and uncommitted. Actual
host held/released input and missing-F8 cause remain unproved. Prior hosted run
failure, advisory WASM failure, accepted0/4 and downstream blocks remain; all live
allowances are spent. No commit, publication, content launch, remote mutation or
phase promotion occurred. No owner verification or acknowledgment is needed.
Context may be cleared. Exact next separate command:
`$gsd-debug continue phase12-packaged-input-tracer` for read-only future
live-proposal preparation; the agent owns source/health/receipt/budget checks.
Invocation alone authorizes no local commit or new live scope.

Final amendment handoff: `sh scripts/hygiene.sh --tree`, `git diff --check`
and absolute-root `tests/cmake/budgets.cmake` each exit0,N/A-local. The full
ordinary/release inventories remain exact and all three packages were produced.

## Capture acknowledgment experiment: no images; halt retained

The owner approved the distinct [capture proposal](../../debug/phase12-capture-live-proposal.md),
including exclusive branch use, one genuine scoped commit/push/dispatch,329
configured runner-minutes,120m cutoff plus10m observation, one conditional
cancellation and at most one50s host child. Commit26c8268 has genuine parent186c0cc;
its14path projected manifest, protected original edits, normal push and independent
remote advertisement all pass. No extra source or documentation fence was added.

[Run41150939552](https://github.com/szTheory/muhtari/actions/runs/41150939552),
exact26c8268/attempt1, completed/failure at2026-10-31T01:14:32Z. Complete audit
reconciles20jobs/191steps, all19executed direct logs and20source-bound artifacts.
Six native current/base677/6/677/6 suites and production/uploads pass, as do
quality/equality and required peers. Windows-arm64 complete2334s fits78m;
Intel complete630s is this attempt's observation, not product improvement proof.

The exact pinned host and candidate archive pass identity checks and fresh
nonprompting event preflight. One childPID3819 logs35F8-down observations from0
through4925ms, all capture0, then generic helper failure around5.2s. Release,
terminal600frame screenshot, expected-image generation and pixel judges are
never reached. No raw or terminal PNG and no frames artifact exists. The5s
no-acknowledgment guard fits this timing, but the generic error does not identify
its exact branch. Frontend audio initialization errors are preserved without
causal attribution; routing/readiness/configuration and missing capture remain
unproved. Repeated failure-report output is not a second process. Ordinary host
launches all skip; no held/released input acceptance is inferred.

WASM independently exposes a new advisory compile regression before tests:
`keyboard_clock(int, ...)` conflicts with the pointer-valued WASI clock identifier,
producing three unchanged-Werror diagnostics. The prior127/runtime1 failure
remains historical. No source repair or further execution followed this finding.

`python3 /tmp/muhtari-capture-exec.Uy5Wh9/terminal.py`, `media.py`,
`audit_outcome.py` and `final_refresh.py` each exit0. Existing
`cmake -DINPUT=/tmp/muhtari-capture-exec.Uy5Wh9/final-view.out
-DEXPECTED_SHA=26c8268f9c56db39308287eaceb935e97eaf3a77
-P tools/check_ci_run.cmake` exits1: real required aggregate fails solely
RetroArch; WASM remains advisory. Full command/exits/run URLs and artifact
bindings are in the proposal; raw responses/logs/media stay scratch-only.

Commit/push/dispatch and actualchild1of1 are spent; unused cancellation expired
at terminal completion. No retry, archive, acceptance or phase promotion occurred.
Accepted0/4 and12-02 through12-04 blocks remain. Shared handoff changes are
uncommitted. No owner verification or acknowledgment is needed. Context may be
cleared. Exact next separate command:
`$gsd-debug continue phase12-packaged-input-tracer`, restricted to read-only
capture/startup/WASI diagnosis and agent-owned health/receipt checks. No source
repair, publication, deadline/pin/permission change or live allowance follows.

Parent handoff checks: exact committed manifest/genuine parent/protected original
edits/empty-index/halted-count assertions0; `audit_outcome.py`0;
`query summary-extract`0 with no completed requirement; hygiene0, diff0 and
document-budget check0. Run URL above; local bookkeeping checks N/A-local.

## Read-only startup and WASI diagnosis; local scope decision pending (historical)

The separate continuation refreshes the unchanged exact run/attempt/all20jobs,
191steps and20artifacts. Existing required audit remains1, solely host failure;
official service is operational and the earlier incident resolved. No source,
workflow, pin, permission, runtime bound or actual host execution changed.

The helper starts its timer before fork and evaluates the5s missing-capture
guard before observing child status. Its shared generic failure message and
buffered frontend output cannot identify the actual branch or input readiness.
The cutoff remains consistent with timing, not a proven startup cause. Repeated
F8 posting still establishes no capture or consumed input; audio errors retain
no causal attribution.

The WASI compiler regression is now independently reproduced with exact-version
headers and the complete original mock: int versus pointer clock identifier,
three errors. A scratch-only conditional private int/clockid_t alias compiles
under target/native/no-clock-fallback headers. Unconditional clockid_t fails the
fallback fixture. `python3 /tmp/muhtari-wasi-clock.SSDai9/check.py` exits0;
original target compile1/corrected0 are expected outcomes, not a repository fix,
WASI runtime success or hosted acceptance.

The [concrete recommendation](../../debug/phase12-held-capture-diagnosis.md#concrete-local-only-recommendation-awaiting-authority)
requests a new local-only scope: portable mock signature, explicit helper
stop-reason/elapsed/capture/error/observed-child-status reports, existing fault
assertions/docs and local verification. Preserve predicate order, statuses,
schedules and all existing lifetime/ownership/pixel controls. Diagnostics cannot
recover a historical branch or establish readiness; no wider window, focus or
driver experiment is justified here. Stop after local verification if approved.

`python3 /tmp/muhtari-startup-diagnosis.igtqEs/receipts.py` exits0; its existing
`check_ci_run.cmake` child exits1 as required for the unchanged failed run.
Hosted URL remains https://github.com/szTheory/muhtari/actions/runs/41150939552;
local compile/bookkeeping URL N/A-local. Raw receipts stay in scratch.

Waiting for the owner's local-scope decision, not owner verification. No source
repair, commit/publication, live allowance, archive or acceptance is authorized;
accepted0/4/downstream blocks and every failed receipt remain. Context may be
cleared. Exact next separate command:
`$gsd-debug continue phase12-packaged-input-tracer`; without explicit approval
of the local batch it preserves the stop. The agent owns continuation checks.

## Approved local portable-clock and helper-diagnostics batch

Owner explicitly approved the concrete local-only recommendation in the next
continuation. The mock now preserves the target clock identifier with its
existing missing-clock integer fallback. The helper reports named stopping
conditions, last valid elapsed/capture observations, saved relevant API errors
and child status only when observed before cleanup. Tests retain valid outcomes,
distinct fault reports and simultaneous-fault guard precedence. Original return
statuses, event schedule, five/45/50-second bounds, ownership and every workflow,
pixel, pin and permission control remain unchanged. Three source/test/doc paths
change; the repair and shared bookkeeping remain uncommitted.

`python3 /tmp/muhtari-clock-diagnostics.HhNyhn/check.py` exits0: actual-source
native/fallback valid/fault executions, actual-source WASI compile-only, real
helper and all standalone consumer links, affected existing policies, formatting
and docs budget. `python3 /tmp/muhtari-approved-local-workflow.py` runs
`cmake --workflow --preset ci`, exit0 through full ordinary tests, three packages
and release tests; both exact `check_junit.cmake` ordinary/release inventories
exit0. One bounded independent review finds no blocking issues. Commands and
limits are retained in the [batch receipt](../../debug/phase12-held-capture-diagnosis.md#approved-local-clock-type-and-failure-reporting-batch);
local run URL N/A-local.

The saved hosted run remains
[41150939552](https://github.com/szTheory/muhtari/actions/runs/41150939552),
exact26c8268/attempt1/terminalfailure; the previous full audit and required
aggregate failure stand. Remote source is unchanged and official service remains
operational. No actual frontend/content launch, commit, push, dispatch, rerun or
cancellation occurred. Historical capture/readiness/routing/audio cause, full
WASI runtime and native MSVC acceptance remain unproved; no image was recovered.

The local batch is finished, not the plan. Accepted0/4, PLAY-03 pending and all
downstream blocks remain. Owner-requested browser-player/optional-interface
research is indexed through dormant seeds, outside this phase's execution scope.
Context may be cleared; exact next separate command:
`$gsd-debug continue phase12-packaged-input-tracer` for read-only preparation of
a concrete future publication/host-experiment scope. The agent owns all fresh
checks; invocation alone grants no live allowance. No user action is needed to
preserve the halt or finish the approved local batch.

## Read-only diagnostic publication proposal

The [new proposal](../../debug/phase12-diagnostics-live-proposal.md) narrows future
publication to the three verified source/test/doc files, with genuine26c8268
parent/base, and one unchanged bounded keyboard-only diagnostic experiment.
Fresh source/remote/workflow/health/PR/active-run checks and complete latest
20job/191step/20artifact reconciliation pass; the existing required audit remains
exit1 for the failed aggregate. `python3 /tmp/muhtari-diagnostic-proposal.7VvQPJ/prepare.py`
and `active.py` exit0; hosted URL remains
https://github.com/szTheory/muhtari/actions/runs/41150939552, local URL N/A-local.
Two bounded specialists and parent synthesis retain limited causal value and
the non-atomic PR race, requiring a new explicit exclusive-branch decision.

Preparation stages/commits/publishes/launches nothing. New329configured-minute
dispatch,120+10-minute queue/observation window, uniquely-new-run conditional
ordinary cancellation and one5/45/50-second child are proposed only. Wait for
owner scope decision; every old allowance stays spent. No acceptance, archive
or next GSD step follows. Context may be cleared; next separate command remains
`$gsd-debug continue phase12-packaged-input-tracer`, with fresh checks owned by
the agent. No acknowledgment is needed merely to preserve halt.

## Approved diagnostic publication and terminal host failure

Owner answered "yes" to the concrete three-file publication/one329configured-
minute dispatch/120+10-minute window/conditional cancellation/exclusive-branch
scope. Commitcee6dea491918282d034f8c6e9a90087d8213439 has genuine26c8268
parent and only the verified clock mock, helper diagnostics and testing guide.
Exact-path staging/committed-blob assertions, hooks/hygiene and sole push pass;
all unrelated owner/planning/research edits stay outside the commit. Fresh
affected compile/link/mock/fault/policy checks and exact local inventories pass;
unchanged previous full-workflow evidence is retained without another full run.

Sole [run41158640754](https://github.com/szTheory/muhtari/actions/runs/41158640754)
attempt1 is terminal/failure2026-10-31T02:45:31Z. All20jobs/191steps/all20source-
bound artifacts and every executed log are audited:16success, allowed title skip,
3failure (host/advisoryWASM/requiredaggregate). Existing required checker exits1
solely because the actual host job fails. All six native current/base677+6
suites, package production and uploads pass. Intel596s/Windows-arm642020s
retain existing37m/78m; quality base is genuine26c8268 and dispatch performance
remains same-head, not phase-versus-main evidence.

Fresh nonprompting event preflight passes and the unchanged pinned package/core
identity is reconciled. One actualPID1933 posts35F8downs through4988ms/capture0.
New reason reporting confirms **acknowledgment-deadline at5090ms, capture0,
errno0, child status unknown**. The guard precedes child polling. This proves
this immediate stop, not historical branch, readiness/routing/hotkey/audio cause
or input delivery. No raw/terminal PNG or frames artifact exists; release and
pixel judges are not reached. Agent image review is impossible without images.
Ordinary host launches skip. No retry or wider window is inferred.

Actual WASI target compilation and ordinary mock now pass, fixing the observed
clock-type regression. Its separate child-exit negative control prints the right
marker but the runtime rejects127 as outside[0..126) and returns1:605of606
tests pass, full WASI remains advisory/unaccepted. Native127 contract and runtime
pins/assertions stay unchanged. This re-exposes the older runtime failure rather
than claiming full portability acceptance.

`python3 /tmp/muhtari-diagnostic-exec.Hak1cS/terminal.py`, `media.py` and
`audit_outcome.py` each exit0; the exact-source `check_ci_run.cmake` exits1.
[Full commands, source/artifact identities and limits](../../debug/phase12-diagnostics-live-proposal.md#terminal-outcome)
retain evidence; raw logs/responses stay scratch-only, local URL N/A-local.
Commit/push/dispatch/oneactualchild are spent, cancellation unused/expired at
terminal. Shared handoff remains uncommitted; no second commit/publication.

Approved step finished, debug unresolved/unarchived; accepted0/4, PLAY-03 pending
and all downstream blocks remain. Context may be cleared. Exact next separate
command: `$gsd-debug continue phase12-packaged-input-tracer` for read-only
readiness/routing observation and WASI127 runtime-test-boundary assessment only;
agent owns fresh checks. No source repair/live allowance/window renewal or
phase promotion is authorized. No user acknowledgment or verification is needed
merely to preserve halt.

## Read-only readiness and WASI boundary assessment

The [latest assessment](../../debug/phase12-held-capture-diagnosis.md#read-only-readiness-and-wasi-exit-assessment-2026-10-31)
finishes the separately invoked read-only continuation. Fresh complete run,
attempt, job, step and artifact receipts match the retained failed diagnostic;
the required auditor stays1. Service health is operational/resolved, source
and remote unchanged, no open phase PR or active CI. Commands/exits/run URL are
in the linked report; raw logs remain scratch only.

Two disjoint source assessments and one adversarial synthesis separate the
confirmed WASI test-result transport defect from unresolved hosted input.
The mock checks the actual helper's127 exit argument and ownership invariants,
then exports127 through a pinned runtime that rejects>=126. Recommend a new
local-only two-test-file plus docs batch: WASI-only outward125 after unchanged
semantic assertions, native127 retained, exact-status/marker observer unchanged.
No source repair or new test execution occurred in this assessment.

Existing host logs already report native-window/display/draw initialization.
Those self-reports still do not establish focus, input-ready processing, held
input or F8 handling. A new independent window-only snapshot offers limited
decision value and adds API/mock surface; defer it and any live retry.

Await the owner's new local-only repair scope decision; preserving the halt
needs no acknowledgment or owner verification. Source/publication/content/live
allowances remain spent or expired, PLAY-03 pending, accepted0/4 and all
downstream blocks unchanged. Context may be cleared. Exact next separate command
is `$gsd-debug continue phase12-packaged-input-tracer` with that scope decision;
the agent owns fresh source/service/receipt checks. Invocation alone grants no
repair, publication, new window or phase promotion.

## Approved local WASI mock transport repair

Owner approved the concrete local-only repair and checks. The
[completed batch](../../debug/phase12-held-capture-diagnosis.md#approved-local-wasi-transport-batch-2026-10-31)
changes only the mock's WASI outward transport to125 after preserving the actual
helper127/argv/ownership assertions, the owning WASI expected status, and the
test documentation. Native127, real helper, observer, clocks, inventory,
workflows/pins/permissions, event ordering/bounds and pixel judges remain.
Source is uncommitted; no actual helper/frontend/content or remote action ran.

Strict native/fallback/native-executed WASI-branch valid/fault controls,
actual WASI compilation, helper link-only, owning registration simulations,
semantic mutations and original-source counterfactual pass their expected
outcomes. Independent fixtures in the pinned runtime confirm125 plus marker
passes and127 plus marker/runtime error or125 without marker rejects.
These do not establish complete C-mock WASI execution; the local WASM linker
is missing, and no tool was fetched or installed. Hosted WASI remains advisory
and unaccepted; actual held/released host input remains unresolved.

Initial full local workflow exits8 solely on the new documentation's size;
the same description was condensed within the unchanged budget. Final
`cmake --workflow --preset ci` exits0 with all three packages, ordinary and
release suites; exact inventory consumers both exit0. The single bounded
source review returns PASS, with the runtime limitation explicit. Fresh
original complete run/source/attempt/jobs/steps/artifacts and health/active-run
checks match the preserved terminal failure; required audit remains1.
Commands/exits and run URL are in the linked batch report; no logs committed.

This authorized local step is finished. Keep summary halted, PLAY-03 pending,
accepted0/4/downstream blocks and spent live allowances automatically. No owner
verification or acknowledgment is needed. Context may be cleared. Exact next
separate command `$gsd-debug continue phase12-packaged-input-tracer` prepares
a concrete scratch WASI linker/target-execution proposal read-only, with fresh
agent-owned source/service/receipt checks. No fetch/install, further repair,
publication/content/live action, renewed window or phase promotion is granted.

## Read-only scratch WASI tooling proposal

The [finite proposal](../../debug/phase12-wasi-local-proposal.md) is prepared;
the prior request to prepare it is finished. Fresh source/HEAD/index and original
exact attempt/all-job/step/artifact/service checks agree; required hosted audit
remains failed, with no active CI or open phase PR. Driver-only inspection
reproduces missing WASI builtins as well as linker. Two disjoint bounded
assessments and one parent adversarial synthesis recommend a single official
checksum-frozen SDK28 arm64-macOS bundle privately in scratch, unchanged pinned
runtime and actual owning two-test registration plus ten WASI counterfactuals.
Its bundled sysroot differs from CI; a pass would be focused local evidence
only. Exact asset, trust, time/storage bounds, commands/exits and stop conditions
are in the proposal. No package was fetched, binary generated, target/helper/
frontend/content executed, source changed or remote action taken in this step.

Waiting for a NEW owner decision on that concrete local tooling/execution
scope. No acknowledgment is required merely to preserve the halt. Repair stays
uncommitted; complete-C WASI execution, hosted WASI acceptance and actual host
input remain unresolved, PLAY-03 pending, accepted0/4/downstream blocks and
spent live allowances unchanged. Context may be cleared. Exact next separate
command `$gsd-debug continue phase12-packaged-input-tracer` with approval or
revision of the proposal; agent owns fresh source/service/receipt checks and
all permitted verification. No source repair, publication, new host allowance,
archive or phase promotion is granted by this preparation or invocation alone.

## Approved focused complete-C WASI execution

Owner approved the [finite scratch SDK batch](../../debug/phase12-wasi-local-proposal.md#approved-execution-outcome-2026-10-31).
It is finished/PASS: exact sole SDK28 asset/private extraction and official
component notices, actual Clang/LLD21.1.4 plus bundled sysroot and unchanged
Wasmtime48.0.6, complete actual C mock WASI link/execution. Direct ordinary0
and child125 plus marker pass; both actual owning registrations pass with
unchanged status/marker observer and deadlines. All ten linked WASI controls
reject, including pre-repair127 runtime rejection despite its marker.
The single bounded evidence reviewPASS and parent synthesis agree. Sources,
runtime, HEAD/index, other edits and full native reports remain unchanged;
the prior three-file repair remains uncommitted. No actual helper/frontend/
content, persistent installation, publication or hosted action occurred.

Initial scratch driver exits1 on an incorrect21.1.0 compiler-description
assertion before target compilation. Exact approved SDK tools identify21.1.4
at the SDK's recorded LLVM submodule; documented prebuild continuation0 uses
saved versions and original deadline without another target attempt/download.
Failure is preserved, actual semantic conditions unchanged. Commands/exits,
tool/source/module identities, bounds, review and complete limits are in the
linked outcome; raw logs stay scratch.

This closes the focused local complete-C WASI runtime gap. Bundled libc differs
from CI; full hosted WASI remains advisory/unaccepted. Actual hosted input
failure/readiness/routing cause stays unresolved, PLAY-03 pending, accepted0/4,
downstream blocks, failed history and spent live allowances unchanged. No local
batch decision or owner verification remains. Context may be cleared. Exact
next separate command `$gsd-debug continue phase12-packaged-input-tracer`
assesses the smallest useful hosted-input readiness/routing experiment and
concrete next scope read-only; agent owns fresh source/service/receipt checks.
No further tool/target/helper/content/source repair/commit/publication/dispatch/
rerun/cancel/new window/archive/phase promotion authority. Stop this GSD step.

## Approved local input-tracer batch

The owner-approved [concrete batch](../../debug/phase12-input-tracer-proposal.md#approved-local-execution-outcome-2026-10-31)
is finished/uncommitted. A separate noninstalled POSIX module observes completed/
null polls and existing port1 Up/B callback returns; fixed64-byte/max64-record
streams use exclusive helper/core files outside disposable paths and a dedicated
inherited descriptor. Actual normal/instrumented synthetic callback order/count/
returns/frame/audio agree. Deterministic faults, state changes, descriptor ownership,
forced child kill and pre-error collection through the actual wrapper pass.

One typed independent review found unfinished-helper acceptance and ignored
late-write errors. One correction round requires helper END/saturation and rejects
known writer-failure logs. Actual helper-prefix and post-INIT EBADF consumers,
plus reintroduced old guards, verify rejection. Four compiled omitted-poll/state,
extra-callback and wrong-value faults also reject; baseline without hooks fails
and current diagnostic passes. Unfinished writer health remains unknown, and no
missing-input inference is made. Timing sample deltas−0.139..+0.698ms are noisy,
not a real-time/latency guarantee or demonstrated improvement.

Final unchanged `cmake --workflow --preset ci` exits0 through all three packages;
ordinary/release exact inventories, affected tests, hygiene/explicit new-source
format and protected-byte/index checks exit0, N/A-local. Original complete hosted
receipt remains unchanged/failure/required1. Initial setup/test failures and the
review's original blockers are preserved in the linked outcome with commands,
exits and correction evidence. HEAD/tree/index, unrelated owner bytes and prior
WASI source/policy/docs repair remain protected. No stage/commit/publication,
actual helper/platform/frontend/content/input, fetch or hosted action occurred.

This establishes local diagnostic reliability only. Actual host readiness/routing/
input cause is unresolved; PLAY-03 pending, accepted0/4, downstream blocks, failed
history and spent allowances remain. No owner verification or acknowledgment.
Context may be cleared; exact next separate command
`$gsd-debug continue phase12-packaged-input-tracer` prepares a concrete hosted
diagnostic proposal read-only, with agent-owned fresh source/service/full-receipt
checks. No live allowance or phase promotion follows. Stop this GSD step.

## Revised read-only local portability-repair proposal

The separately invoked proposal step is finished. The [current local scope](../../debug/phase12-portability-repair-proposal.md#revised-narrow-local-repair-scope-2026-10-31)
uses the completed seven-invocation fortify0/2 actual-source discrimination and
retained old-helper truncation evidence. It recommends checked acknowledgment/
owned-child cleanup and suffix-derived capacities in the two owning test
sources, with their existing CMake consumer optional for negative modes.
Fixed compiler gate: eleven initial invocations and only one conditional
three-repaired-object correction set, fourteen maximum/no retry; native
synthetic controls, original full workflow/inventories/package exclusions and
one bounded repair review would follow only after a NEW separate decision.
Earlier broader repair approval remains stopped and does not resume.

`python3 /private/tmp/muhtari-portability-preflight.py` exits0: complete original
and latest attempts/all jobs/steps/artifacts/executed logs agree, retained
diagnostic identity/source/member hashes0, operational/resolved/noPR/noactiveCI,
advertised source/HEAD/index/protected bytes unchanged. Both required
`tools/check_ci_run.cmake` audits remain1; failed run URLs are linked in the
proposal. Private receipts `muhtari-portability-proposal.q6edwrwr`, N/A-local;
no committed logs. One bounded independent read-only proposal review clarified
the correction allowance; no further proposal fan-out. No source repair,
compiler/container/target/program run or actual frontend/content/input occurred.
Focused complete-C WASI execution already passed separately; older SDK28
linker-only proposals remain historical, hosted WASI advisory/unaccepted.

Preserve accepted0/4, PLAY-03 pending, downstream blocks, both failed audits and
all spent allowances. Await only the new local repair scope decision, not owner
verification or acknowledgment to retain the stop. Context may be cleared;
exact next separate command `$gsd-debug continue phase12-packaged-input-tracer`
with that decision. Agent owns fresh source/service/complete-receipt checks and
later execution only if approved. No stage/commit/publication/hosted action,
allowance renewal/archive/promotion follows. Earlier next actions below are
historical where superseded.
Final `python3 /private/tmp/muhtari-portability-stop-check.py`0 and
`git diff --check`0 verify frozen source/HEAD/index/all unrelated bytes,
five allowed planning paths only, links/halt/both failed audits/no active
compiler container. These checks establish proposal/handoff preservation,
not local repair or actual packaged-input acceptance.

## Approved fixed offline tooling comparison

The owner selected option1 for the [fixed seven-invocation batch](../../debug/phase12-portability-repair-proposal.md#approved-fixed-tooling-comparison-outcome-2026-10-31).
It finished with local compiler sensitivity established: baseline effective
fortify0/unannotated real `write`/actual protocol object0, versus fixed scratch
fortify2/annotated real declaration/object1 with only the intended ignored-write
error at line247. GCC15.3.0/aarch64-linux-gnu/glibc2.41 metadata and both macro/
declaration commands exit0. Exactly seven immutable cached-image offline
containers used unchanged read-only source, original strict warnings, private
outputs,60s deadlines and no retry; every owned ID/cleanup is recorded.

`python3 /private/tmp/muhtari-portability-preflight.py` exits0 for complete
original/latest run/attempt/jobs/steps/artifacts/all executed logs, retained
diagnostic identity, operational/resolved service, noPR/noactiveCI/remote
source/HEAD/index/protected bytes. Both required audits remain1 and their
original hosted run URLs are linked in the proposal. `python3
/private/tmp/muhtari-portability-fortify-comparison.py` exits0; individual
compiler exits and oracles above establish the comparison, not collector0
alone. Raw diagnostics/argv/IDs/hashes stay private; N/A-local.

All three frozen test hashes/HEAD/index/unrelated bytes remain unchanged and
all owned containers stopped. No program was linked/executed, helper recompiled,
source repaired, runtime/native/full workflow or repair review run. This
neither establishes exact hosted toolchain equivalence nor resolves original
packaged-input readiness/routing/F8/audio. Preserve accepted0/4, PLAY-03 pending,
downstream halt, both failed hosted audits and all spent allowances. No stage,
commit/publication/hosted action, download/install, archive or promotion follows.

Stop this completed tooling GSD step after final preservation/handoff checks.
`python3 /private/tmp/muhtari-portability-fortify-handoff-check.py` exits0 for
exact seven payloads/individual exits/oracles/output hashes, no owned container,
unchanged frozen source/HEAD/index/all unrelated bytes, five planning paths only,
links/halt/both failed audits and `git diff --check`0.
No owner verification or acknowledgment is needed. Context may be cleared;
exact next separate command `$gsd-debug continue phase12-packaged-input-tracer`
prepares a revised narrow local portability-repair proposal read-only using
this evidence. The agent owns fresh source/service/complete-receipt checks;
a later separate decision is required before repair. Earlier repair approval
does not resume automatically, and invocation renews nothing.

## Commit-window reconciliation (2026-11-01)

The recorded interval `0ca7c9aa92152b09a277388e0fb2c977601016fa..55f30d64ad1999231a97ea05a96e6a29d061c116`
contains 24 commits (`git rev-list --count` exit 0). The broad historical
boundary is retained; no commits are omitted to manufacture a narrower plan
window. The original four-commit 12-01 implementation/halt subset is the first
four commits below. The remaining twenty are later phase recoveries and
handoffs. Both `actuals.commits` and the top-level `commits` now report the
measured 24 for the recorded interval. This reconciles G-12-12 only; it does
not change the halted acceptance result, requirements, or earlier failures.

| Commit | Recorded work |
| --- | --- |
| `bf21fc3ae7d89f4e03f61f1f7a2426dc4af5df29` | Original 12-01 held/released RetroArch input schedule and comparator wiring. |
| `b36001767b97060d9c5830bec504cba797a8255e` | Original 12-01 diagnostic comparator target/link correction. |
| `31b12c39b02f477ad8777fee4363b7e325cab460` | Original 12-01 halted summary. |
| `2b5f36d6348f53ebed97b9552e58ddb27ef16398` | Original phase halt and recovery handoff bookkeeping. |
| `28e13e1cb7da122d72d1c83d438861001b503504` | Later packaged-input band verification and pixel oracle correction. |
| `fa8fafa845d7a55278da6300059a470826cde19a` | Semantic halt record for the packaged-input correction. |
| `258f285acf0dec03e615ea3705177c5c9b5c07e4` | Actual packaged-input failure and continuation record. |
| `19121b1109df267497360e5c85a82b1621895666` | CI failure evidence retention and reporting consumers. |
| `f9da764d711600ab554d8a74aeb70ebd8561644f` | Communication preference documentation. |
| `cb68bd0c6fe1d83ab1165479b4242256219de674` | Hosted keyboard-access preflight without content launch. |
| `b89b05fdb322f2f87b2fdf85bd5c4c483857137b` | Workflow-momentum handoff guidance. |
| `a7aa88994f6f2d5df5c3dcc9c22cc1887bd76fc5` | Bounded keyboard feasibility through one RetroArch process. |
| `39879d8f06cf65a8452ec11211a4ad6d597d6f7c` | Failed child-exec fixture correction. |
| `abc4f570d2ccc6d7198c41d47ba0a1ba6797decf` | Failed keyboard-child ownership regression control. |
| `7e07c4cb8890227eea1eeef1eda1a5d7cad58217` | Portable keyboard movie-span validation. |
| `753bf4acd7ec7a438dd7ce0da87bf5879ca9f0f3` | Bounded keyboard-feasibility halt record. |
| `a0834db21b2b22f30d2147458aeec27df6ae1530` | Intel-completed-run PAL BIOS deadline calibration. |
| `18e05d48b58d72e81c784f4a98501cf900df8c78` | Renewed keyboard-feasibility halt record. |
| `ef5357cb2b6cb259ed096f4c8d2d5928a650230d` | Context handoff and agent-owned check guidance. |
| `d8dd74b6598da71e30434dad380b79c2d784aa27` | Targeted Intel retry halt and durable handoff. |
| `186c0cc4b39c12fb149ddc0ef3f43897fba55865` | Intel job timeout margin restored from completed evidence. |
| `26c8268f9c56db39308287eaceb935e97eaf3a77` | Held keyboard capture acknowledgement before release. |
| `cee6dea491918282d034f8c6e9a90087d8213439` | Clock-type preservation and keyboard-probe failure reporting. |
| `55f30d64ad1999231a97ea05a96e6a29d061c116` | Bounded hosted input-tracing diagnostics. |
