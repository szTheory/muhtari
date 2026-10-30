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
