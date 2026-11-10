---
phase: 12-player-ready-delivery-and-playstead-bridge
plan: "07"
subsystem: packaged-host diagnostics
tags: [ci, openbios, preservation, commit-ledger]

requires:
  - phase: "12-09"
    provides: "Local source-identity and documentation prerequisite"
provides:
  - "Measured 12-01 commit-window reconciliation"
  - "Bounded source-bound review of the Windows ARM64 timeout, with cause still undiagnosed"
  - "Locally validated OpenBIOS command and stream retention for a future diagnostic"
affects: [PLAY-03, phase-12-host-acceptance]

actuals:
  tokens: 3947
  tasks: 2
  commits: 0

tech-stack:
  added: []
  patterns: [private standard-library preservation snapshot, object-graph commit accounting]

key-files:
  created:
    - .planning/phases/12-player-ready-delivery-and-playstead-bridge/12-07-SUMMARY.md
  modified:
    - .planning/phases/12-player-ready-delivery-and-playstead-bridge/12-01-SUMMARY.md
    - .planning/phases/12-player-ready-delivery-and-playstead-bridge/12-UAT.md
    - .planning/LESSONS.md
    - AGENTS.md

key-decisions:
  - "Retain the recorded 12-01 interval and report its measured 24 commits instead of manufacturing a narrower boundary."
  - "Use only the owner-authorized failed-run receipt and the two already-referenced Windows ARM64 job logs; do not widen historical log retrieval."
  - "Keep the Windows timeout undiagnosed because matching source inputs and a passing comparison do not explain the censored failure."
  - "Keep PLAY-03 blocked and preserve the existing UAT failures and totals."

patterns-established: []

requirements-completed: []
coverage:
  - id: G-12-12
    description: "The 12-01 summary's measured commit count matches its recorded interval and commit ledger."
    requirement: PLAY-03
    verification:
      kind: other
      ref: "git rev-list --count 0ca7c9aa92152b09a277388e0fb2c977601016fa..55f30d64ad1999231a97ea05a96e6a29d061c116 (exit 0, 24); every commit mapped in 12-01-SUMMARY.md"
      status: pass
    human_judgment: false
  - id: PLAY-03-causal-diagnosis
    description: "The Windows ARM64 built.openbios failure has an evidence-supported causal diagnosis."
    requirement: PLAY-03
    verification:
      kind: other
      ref: "Owner-authorized receipt/log review, exact six-file source comparison, and expected-failure audit; the timeout is still censored and its cause remains unknown."
      status: unknown
    human_judgment: false
  - id: OpenBIOS-local-output-capture
    description: "OpenBIOS retains the prepared assembler command and both child streams; local success/failure controls pass without changing firmware hashes or timeouts."
    requirement: PLAY-03
    verification:
      kind: integration
      ref: "Local output-capture closeout below; focused CTest, scratch baseline/repaired comparison and exact source/artifact-path assertions exit 0, N/A-local."
      status: pass
    human_judgment: false

duration: "About 20m; exact start timestamp was not captured"
completed: 2026-11-01
status: halted
plan_head_before: 2658469ec25db3b6be6f1b197cad8ca94222bc5d
plan_head_after: 2658469ec25db3b6be6f1b197cad8ca94222bc5d
commits: 0
---

# Phase 12 Plan 07: Packaged Input Failure Trace Summary

**The 12-01 ledger now matches its 24-commit interval; the authorized receipt and logs confirm the Windows timeout but still do not identify its cause.**

## Outcome

The local preservation snapshot recorded HEAD `2658469ec25db3b6be6f1b197cad8ca94222bc5d`, branch `phase/12-player-ready-delivery-and-playstead-bridge`, the remote metadata, the index, and 892 tracked/non-ignored paths. The existing source commit `7b6d8832058a89ea3ffc1dde18add4f6b832d35b` is an ancestor of HEAD (exit 0). The two in-scope source hashes at snapshot time were `tests/built/openbios.cmake` `55cd28ab7b5de7adbaa8b3ffd7070080a9f6a2e57043e6b3866b32bd8caa9a2d` and `tests/built/tests.cmake` `4d1e29814d52a09b2effaf947f06cdfa2ed100991fa42c85af6f1d664f301ebc`.

At the initial Task 1 gate, the 12-06 summary and debug ledger identified run 41255515829, failed Windows ARM64 job 124697769109, and the matched successful job from run 41224305264/job 124605180515, but did not name local receipt/log paths. The preservation freeze therefore recorded the three private inputs as unresolved and exited 1. The owner later authorized a read-only retrieval of exactly those three items; their identities and content hashes are recorded below, and raw files remain in private scratch.

The retrieved run receipt matches the expected URL, source SHA, branch, event, and attempt. The run status is completed with overall conclusion `cancelled`; every job is terminal, the Windows ARM64 build failed, and the unique `CI required` aggregate failed. This explains the top-level cancellation without converting the run into a pass. The failed log records `built.openbios` timing out at 1.53 seconds under scale 1; the matched log has two passing observations at 0.15 and 0.06 seconds. The six recipe/pin inputs compare byte-identical between the successful and failed source commits, and the exact failed-source registration retains `TIMEOUT 1` from the prior 16-run 0.40-second maximum. The process was killed at the timeout, so the logs contain no completed-work measurement or discriminating cause.

The expected-failure audit wrapper exited 0 after confirming its inner `tools/check_ci_run.cmake` exit 1 and the sole expected `required aggregate is not completed/success` fatal diagnostic. This independently confirms that the required aggregate failed. No causal observation or repair was established, so Task 2 was not run: no CTest, source edit, local workflow, review, commit, push, hosted dispatch, or cancellation occurred. `TIMEOUT=1` and the spent hosted allowance remain unchanged.

The independent commit-ledger issue was reconciled. The recorded interval `0ca7c9aa92152b09a277388e0fb2c977601016fa..55f30d64ad1999231a97ea05a96e6a29d061c116` contains 24 commits, and all 24 are mapped in the dated appendix of 12-01-SUMMARY.md. The original four-commit 12-01 subset is distinguished from the later twenty recoveries. The historical interval is retained, and both 12-01 count fields now say 24. The UAT addendum records this bookkeeping correction while leaving its historical failed row and totals unchanged.

PLAY-03 remains blocked. The original failed host evidence, accepted 0/4 state, downstream blocks, and all spent allowances remain unchanged. The receipt and logs close the missing-input gate but do not explain the slowdown or establish input acceptance. There is no user verification step; further diagnosis needs a separately scoped action that can produce a new observation distinguishing the remaining causes.

## Performance

- **Duration:** About 20 minutes; the exact start timestamp was not captured.
- **Completed:** 2026-11-01.
- **Tasks:** Task 1 reconciled the ledger and reviewed the exact retained failure evidence, then stopped undiagnosed; Task 2 was not run; Task 3 bookkeeping completed.
- **Repository files:** Four existing planning/memory records updated and this summary created.

## Task Commits

No commits. The plan requires its summary and scoped ledger updates to remain uncommitted; no source repair was justified.

## Files Created/Modified

- Created `12-07-SUMMARY.md` with the diagnosis stop, ledger result, commands, and limits.
- Modified `12-01-SUMMARY.md` to report the measured interval and map its 24 commits.
- Modified `12-UAT.md` with append-only bookkeeping and evidence follow-ups.
- Modified `LESSONS.md` and `AGENTS.md` to preserve the reproduced stale-resume lesson as future behavior.

## Task Results

| Task | Result |
| --- | --- |
| 1. Trace retained failure and reconcile commit ledger | Ledger reconciled to 24; owner-authorized receipt/log review and source/property comparison completed. The failure remains undiagnosed; no test/source work followed. |
| 2. Apply justified repair and observe acceptance | Not executed; the causal gate was not met. |
| 3. Reconcile outcome and stop at phase boundary | Completed bounded bookkeeping and preserved the failed acceptance state. |

## Commands and Evidence

| Command | Exit | Result |
| --- | ---: | --- |
| `python3 "$RECOVERY_PRIVATE/preservation.py" freeze --root "$PWD" --baseline "$RECOVERY_PRIVATE/baseline.json" --evidence "$RECOVERY_PRIVATE/retained-evidence.json"` | 1 | Baseline created before repository edits; 892 paths and 868 index entries recorded. Required receipt/log locations were unresolved at that initial gate. |
| `python3 /private/tmp/muhtari-phase12-wave3.2qX0Z3Rz/preservation.py compare --baseline /private/tmp/muhtari-phase12-wave3.2qX0Z3Rz/baseline.json --allowed-final /private/tmp/muhtari-phase12-wave3.2qX0Z3Rz/allowed-final-prewrite.json --evidence /private/tmp/muhtari-phase12-wave3.2qX0Z3Rz/retained-evidence.json --output /private/tmp/muhtari-phase12-wave3.2qX0Z3Rz/prewrite-comparison.json` (pre-edit) | 0 | No differences from the frozen repository/index snapshot before edits. |
| `git merge-base --is-ancestor 7b6d8832058a89ea3ffc1dde18add4f6b832d35b HEAD` | 0 | Required source commit is an ancestor of the snapshotted HEAD. |
| `git rev-list --count 0ca7c9aa92152b09a277388e0fb2c977601016fa..55f30d64ad1999231a97ea05a96e6a29d061c116` | 0 | Recorded interval contains 24 commits. |
| `python3 -` (compare the appendix hashes with `git log --reverse`; check both summary counters and preserved UAT totals/failed row) | 0 | All 24 ordered hashes match; both summary fields report 24; original UAT row and totals remain unchanged. |
| `git diff --check HEAD -- tests/built/openbios.cmake tests/built/tests.cmake .planning/phases/12-player-ready-delivery-and-playstead-bridge/12-01-SUMMARY.md .planning/phases/12-player-ready-delivery-and-playstead-bridge/12-UAT.md` | 0 | No tracked whitespace errors. |
| `git diff --no-index --check /dev/null .planning/phases/12-player-ready-delivery-and-playstead-bridge/12-07-SUMMARY.md` | 1, no diagnostics | The no-index comparison reports the expected new-file difference; the separate Python content inspection exited 0 and found no trailing whitespace, a final newline, and a nonempty summary. |
| `python3 -` (inspect `12-07-SUMMARY.md` for trailing whitespace, final newline, and nonempty content) | 0 | Final content inspection result is recorded in the self-check below. |
| `python3 /private/tmp/muhtari-phase12-wave3.2qX0Z3Rz/preservation.py compare --baseline /private/tmp/muhtari-phase12-wave3.2qX0Z3Rz/baseline.json --allowed-final /private/tmp/muhtari-phase12-wave3.2qX0Z3Rz/allowed-final.json --evidence /private/tmp/muhtari-phase12-wave3.2qX0Z3Rz/retained-evidence.json --output /private/tmp/muhtari-phase12-wave3.2qX0Z3Rz/comparison.json` | 0 | The final private report confirms only the three scoped planning-file changes, unchanged HEAD/index, and unchanged retained evidence. |
| `gh run view 41255515829 --repo szTheory/muhtari --json attempt,status,conclusion,headSha,url,jobs,event,headBranch > "$RECOVERY_PRIVATE/failed-run.json"` | 0 | Receipt SHA256 `99f79ab5a0e44227627abe5e5f793603b24c472b18ffe26f4ece1a42e0a98ad8`; source/identity matches, top-level conclusion is cancelled, Windows ARM64 and the unique required aggregate failed, all jobs are terminal. |
| `shasum -a 256 "$RECOVERY_PRIVATE/failed-run.json"` | 0 | Confirms the private receipt digest recorded above. |
| `gh api --allow-escape-sequences repos/szTheory/muhtari/actions/jobs/124697769109/logs > "$RECOVERY_PRIVATE/failed-windows-arm64.zip"` | 0 | Failed job response retained privately; despite its temporary `.zip` suffix it is UTF-8 text with terminal escapes, SHA256 `1c1a650033e0b961001d73e49e48142b9183360d26a49a955804f49e9c105812`. |
| `gh api --allow-escape-sequences repos/szTheory/muhtari/actions/jobs/124605180515/logs > "$RECOVERY_PRIVATE/matched-success-windows-arm64.zip"` | 0 | Existing matched successful job response retained privately, SHA256 `9ed8bef0db68e7d426b97b7078c4b5314ec3eaf7b540e8160365fd3a0acbbcac`. |
| Local receipt identity guard (first version required top-level conclusion `failure`) | 1 | Over-strict local assertion; the receipt correctly reports overall `cancelled` because downstream jobs were cancelled. No remote read was repeated. |
| Corrected local receipt guard (job-level contract) | 0 | URL/source/branch/event/attempt match; every job terminal; one failed `CI required` aggregate and failed Windows ARM64 job. |
| `git diff --exit-code 7b83b3b914d083a0f49719acfbdb47c359cd342f 7b6d8832058a89ea3ffc1dde18add4f6b832d35b -- tests/pins.txt tests/built/tests.cmake tests/built/openbios.cmake cmake/MuhtariTests.cmake CMakePresets.json scripts/fetch-pin.sh` | 0 | Not accepted as source-comparison evidence because `tests/pins.txt` does not exist at either commit. |
| `git show 7b6d8832058a89ea3ffc1dde18add4f6b832d35b:tests/pins.txt` | 1 | Exposed the incorrect pin-manifest path; the tracked manifest is the repository-root `pins.txt`. |
| `git diff --exit-code 7b83b3b914d083a0f49719acfbdb47c359cd342f 7b6d8832058a89ea3ffc1dde18add4f6b832d35b -- pins.txt tests/built/tests.cmake tests/built/openbios.cmake cmake/MuhtariTests.cmake CMakePresets.json scripts/fetch-pin.sh` | 0 | All six existing recipe, test-registration, timeout-helper, preset, and pin inputs are identical between the matched successful and failed source commits. |
| `git show 7b6d8832058a89ea3ffc1dde18add4f6b832d35b:tests/built/tests.cmake | rg -n -C 5 'built\.openbios|MUHTARI_TIMEOUT_SCALE|TIMEOUT'` | 0 | The owning registration uses outer CTest `TIMEOUT 1`; its comment cites 16 Windows ARM64 successes with a 0.40-second maximum. The failed job log reports timeout scale 1. |
| `git show 7b6d8832058a89ea3ffc1dde18add4f6b832d35b:tests/built/openbios.cmake` | 0 | The recipe removes/recreates the output directory, reads and transforms the pinned source, invokes the pinned assembler through Wasmtime, then verifies output existence and SHA. No internal progress/timing instrumentation distinguishes a slow stage before CTest kills the process. |
| `git show 7b6d8832058a89ea3ffc1dde18add4f6b832d35b:pins.txt | rg -n -C 2 '7800openbios-asm'` | 0 | Confirms the assembler source revision and digest recorded for the failed source. |
| `sh "$RECOVERY_PRIVATE/expected-failed-audit.sh" "$FAILED_RECEIPT" "$RECOVERY_PRIVATE"` | 0 (inner audit 1) | Exact source-bound receipt audit produced only the expected failed required-aggregate diagnostic. |
| `unzip -l "$RECOVERY_PRIVATE/failed-windows-arm64.zip"` | 9 | The endpoint returned UTF-8 text, not a ZIP; the raw response stayed intact and was filtered locally. |
| Local ANSI-stripped target-line inspection of both job logs | 0 | Failed test is timed out at 1.53 seconds; matched success has two passes at 0.15 and 0.06 seconds. No causal signal appeared. |

Run URL: [41255515829](https://github.com/szTheory/muhtari/actions/runs/41255515829). Its required failure remains historical evidence, not acceptance.

## Deviations from Plan

After the initial missing-path stop, the owner explicitly authorized a read-only retrieval of only the planned run receipt and two named job logs. Those reads cleared the evidence-availability gate; the bounded trace remained undiagnosed. Task 2 was not run because no causal explanation or justified repair emerged. The expected-failure audit confirmed the already-failed aggregate. No source, publication, new run, cancellation, or other hosted side effect occurred.

## Preservation

The original private snapshot/compare records are under `/private/tmp/muhtari-phase12-wave3.2qX0Z3Rz/`. Its final compare exited 0 for the first three planning-file changes and confirmed unchanged HEAD, branch, remote metadata, index, and then-known retained evidence. The freeze's exit 1 records that the evidence paths were missing at that time. The later owner-authorized retrieval wrote only under separate private scratch; the user-requested follow-up changed this summary, UAT, AGENTS.md and LESSONS.md. Those later document changes are outside the earlier comparison and were checked separately for scope and whitespace. No baseline was overwritten. All changes remain uncommitted; no source or index changes occurred.

## Next Phase Readiness

G-12-12 bookkeeping is reconciled; the exact receipt and two named logs are now read and audited. The producer cause and PLAY-03 remain unresolved. Further work requires a new bounded diagnostic scope that produces a discriminating observation; this execution authorizes no additional log sweep, test, repair, publication, or hosted run. No user verification is needed to preserve the stop.

## Self-Check: PASSED

The 24-hash appendix matches `git log --reverse`, both summary counters are 24, and the UAT original failure row/totals remain unchanged. The prior preservation comparison exited 0 for its three-file bookkeeping scope. The receipt and both authorized logs were validated and hashed in private scratch; source/recipe inputs match between the two runs, and the required aggregate audit remains correctly failed. The tracked and follow-up document whitespace checks exited 0; the new-file `git diff --no-index --check` exited 1 with no diagnostics, and the content inspection exited 0 with a final newline and no trailing whitespace. The resume-loop lesson is recorded in AGENTS.md and LESSONS.md. PLAY-03 remains blocked; no source, publication, dispatch, or allowance state changed.

---
*Phase: 12-player-ready-delivery-and-playstead-bridge*
*Completed: 2026-11-01*

## Post-halt OpenBIOS stage diagnostic (2026-11-01)

After this plan halted, the owner separately authorized one bounded diagnostic:
add per-stage OpenBIOS markers and failure-artifact capture, keep the existing
one-second CTest timeout and workload, validate locally, then use at most one
hosted CI run. This later batch does not change the plan's task counts or close
its halted causal diagnosis.

Commit `2a78ad8936ca87e479e15201a5337ac60a8f4c6e` records stages
`prepare-output`, `transform-source`, `assemble`, `verify-output`, and
`complete`. Its self-test checks both the full successful trace and a
duplicate-incbin fault, with a fixture dependency so the successful trace is
available under parallel CTest scheduling. CI uploads the stage trace and
CTest logs from Windows ARM64 even when the workflow step fails. The timeout
and test workload are unchanged.

Local parallel check:

| Command | Exit | Result |
|---|---:|---|
| `cmake --preset ci && ctest --test-dir build/ci -R '^built[.]openbios([.]self_test)?$' --output-on-failure --parallel 4` | 0 | Both OpenBIOS tests pass under parallel scheduling; the fixture orders the pass-trace consumer after the successful build. |

The full local workflow output showed all 677 ordinary tests and all 6 release
tests passing and generated the configured packages; the tool did not surface
its final shell exit code, so no numeric exit is claimed here.

The single hosted dispatch ran on commit `2a78ad8936ca87e479e15201a5337ac60a8f4c6e`:
[run 41323089043](https://github.com/szTheory/muhtari/actions/runs/41323089043).
Windows ARM64's `built.openbios` passed in 0.14 seconds, and the downloaded
`openbios-stage-windows-arm64` artifact ends at `stage=complete`. The full
matrix and quality jobs passed, including the Windows ARM64 independent base.
The overall workflow failed because the separate macOS RetroArch keyboard
diagnostic reached `acknowledgment-deadline` at 5061 ms with
`child_status=unknown`; `CI required` therefore failed. This failure is in the
separate RetroArch job and does not identify the prior OpenBIOS timeout cause.

This run did not reproduce the historical Windows timeout or explain its
cause. PLAY-03 remains blocked, accepted remains 0/4, and the historical failed
evidence remains intact. The one-run diagnostic allowance is spent; no retry,
repair, or acceptance is authorized. The run lasted about 40 minutes: Windows
ARM64 was the longest job at about 36.5 minutes, macOS ARM64 waited about 10.5
minutes for a runner, and no jobs were cancelled. A single run supports no safe
workflow, timeout, or cache change. For the separate input gap, the next
read-only preparation command is `$gsd-debug continue phase12-packaged-input-tracer`;
any further hosted run needs a new concrete scope.

## Post-halt input-trace attempt (2026-11-01)

After that diagnostic, the owner authorized one fixed-source input-trace run
under the separate scope in
[`phase12-input-gap-next-scope.md`](../../debug/phase12-input-gap-next-scope.md).
The dispatch command exited 0 and returned
[run 41329055050](https://github.com/szTheory/muhtari/actions/runs/41329055050):

```sh
gh workflow run ci.yml --repo szTheory/muhtari --ref phase/12-player-ready-delivery-and-playstead-bridge -f keyboard_preflight_only=false -f keyboard_probe_only=true -f input_trace_diagnostic=true
```

It ran source `2a78ad8936ca87e479e15201a5337ac60a8f4c6e`, attempt 1. The
workflow ended with 20 terminal jobs: 13 succeeded, 4 skipped and 3 failed.
Windows ARM64 `built.openbios` timed out at 1.74 seconds with `TIMEOUT 1`
(CTest exit 8); its stage trace ended at `assemble`. The quality join and
required aggregate then failed because required artifacts were missing.
RetroArch was skipped, so this run produced no input child or input trace.
No retry, timeout change, source edit, cancellation or acceptance occurred.
The single-run allowance is spent; PLAY-03 remains blocked at 0/4.

The authenticated Windows ARM64 failure ZIP contains only the stage trace,
`LastTest.log`, and `LastTestsFailed.log`; it does not contain `assemble.log`.
`LastTest.log` records 1.74 seconds and an empty output section. In the owning
script, `record_stage("assemble")` runs immediately before `stage()` calls
`execute_process`; stdout goes to `assemble.log`, while stderr is held in a
CMake variable and printed only if the process returns. The stage marker
therefore does not prove Wasmtime launched, and this artifact cannot establish
where time was spent. The same-source pass in run 41323089043 (0.14 seconds,
complete trace) establishes variable outcomes, not their cause. Keep the
one-second timeout; no timeout-policy change is supported by these samples.

The run took 19m04s; macOS ARM64 queued 12m41s and built 6m03s, while Windows
x64 was the longest active job at 17m14s. No jobs were cancelled. This single
run supports no safe workflow or cache change. The current artifact-only review
is exhausted. The next useful technical scope is local-only: retain the
assembler command and output streams in the always-uploaded diagnostic
artifact, validate the capture with bounded local controls, and leave hosted
execution out of scope. Any later hosted observation needs a new concrete
scope and authorization.

## Local OpenBIOS output capture (2026-11-01)

The separately authorized [local scope](../../debug/phase12-openbios-observability-scope.md)
is complete and uncommitted. This closes the local diagnostic-retention gap;
the original Windows timeout and input causes remain unknown. It does not
change this plan's halted status, task/commit totals or requirements acceptance.

`tests/built/openbios.cmake` writes a prepared argv/working-directory record
once before launch and redirects stdout/stderr directly to explicit files
beside the output image. Ordinary failure still reports child status and
stderr. The existing Windows ARM64 always-upload step includes the command
record and both streams; no other workflow behavior changed. The existing
self-test verifies exact command identity, successful stdout and a real
Wasmtime missing-module failure with persistent stderr and unchanged fatal
reporting; the duplicate-incbin fault remains covered. A command record proves
preparation only, and an empty or partial stream cannot prove launch or progress.

Private evidence root is `/private/tmp/muhtari-openbios-capture-20261101.Sfcq0c`.
It retains exact argv, exits, bounds, JUnit and baseline/repaired comparison;
no execution logs are committed. Run URL for every check is N/A-local.

| Command | Exit | Establishes |
|---|---:|---|
| `cmake --preset ci` | 0 | Existing configure/inventory accepts the extended self-test without adding a registration. |
| `ctest --test-dir build/ci -R '^built[.]openbios([.]self_test)?$' --output-on-failure --parallel 4 --output-junit <private>/capture-red.xml` before recipe repair | 8 | Normal assembly passes; new control rejects missing `assemble-command.txt`. |
| Same focused CTest selector after repair, distinct `capture-green.xml` | 0 | Existing pair passes; source fault and child failure are correctly rejected internally. |
| `ctest --test-dir build/ci -R '^(built[.]openbios([.]self_test)?|bios[.]handoff_simple)$' --output-on-failure --parallel 4 --output-junit <private>/capture-adjacent.xml` | 0 | Final capture checks, adjacent BIOS handoff and required simple fixture pass under unchanged bounds. |
| `cmake -DWASMTIME=<existing-pin> -DPKG=<existing-pin> -DASM=<existing-pin> -DFILL=00 -DOUT=<ignored-build>/openbios.bin -DTRACE=<ignored-build>/stage.txt -P tests/built/openbios.cmake` | 0 | Existing zero-fill digest remains exact. |
| `python3 /private/tmp/muhtari-openbios-capture-20261101.Sfcq0c/differential.py` | 0 | Exact baseline builds0/control1; repaired recipe builds0/control0 in the same isolated namespace. |
| `python3 /private/tmp/muhtari-openbios-capture-20261101.Sfcq0c/static-checks.py` | 0 | Both 16 KiB firmware digests, production transform/argv/hash guards, generated `TIMEOUT 1`/`1`, exact upload paths and all other workflow bytes are preserved. |

Inline diff review finds no behavior deletion or weakened guard. Formal
mutation tooling is unavailable for this CMake project and was skipped; no
dependency was installed. Baseline/repaired differential supplies the recorded
revert-and-reconfirm signal without stashing unrelated work. Full workflow,
Windows execution and CTest-kill reproduction were outside this local batch;
local controls establish retained command/output on completed and failed child
invocations, not the historical hosted outcome. Final whitespace/content and
unrelated-file preservation checks are recorded in the active debug closeout.

Final focused CTest exits 0 (OpenBIOS 0.07s, self-test 0.03s). Scoped
`git diff --check`, new-content whitespace/privacy/state assertions and the
actual GSD `summary-extract --fields requirements_completed` consumer exit 0;
requirements remain empty. Final
`python3 /private/tmp/muhtari-openbios-capture-20261101.Sfcq0c/preserve.py final`
exits 0: HEAD, branch, index, raw path inventory and 887 unrelated
tracked/nonignored paths match baseline. This excludes the nine authorized
files and does not claim a snapshot of the entire ignored/generated tree or
all historical external receipt roots. Prior failed receipts were not changed.

No hosted read, dispatch, retry, cancellation, input/RetroArch attempt, install,
commit, push, archive or acceptance occurred. Original halt, accepted 0/4,
PLAY-03 pending, downstream blocks, failed receipts and every spent allowance
remain intact. Stop at the new hosted-authority boundary. The recommended next
step is a separate finite hosted observability proposal with exact source and
publication scope, run cap, runner-minute allowance and observation window;
the owner must authorize it before hosted action. No owner verification is
needed for this local result. Context may clear after handoff; the exact next
command is `$gsd-debug continue phase12-packaged-input-tracer` with that new
concrete scope, and the command alone renews no allowance.

## Post-capture hosted-scope review (2026-11-01)

Two independent read-only reviews checked whether another hosted run would
produce enough new evidence to justify its cost. Existing `workflow_dispatch`
does not filter the six-platform build matrix to Windows ARM64. Its
`keyboard_preflight_only` path deliberately fails the RetroArch job before
content, while `keyboard_probe_only` launches the actual keyboard process. The
full dispatch reserves up to 329 configured runner-minutes; its artifact upload
would verify only that completed Windows invocation's files, not explain the
historical timeout. A new standalone workflow would add duplicate runner setup,
pin, configure, test, timeout and artifact logic for one observation.

Recommendation: do not run CI or add a dedicated workflow now. Preserve the
locally verified capture change and consider it only within a future separately
authorized source-publication/full-CI scope with independent value. No hosted
read, source publication, test/build, or workflow change occurred in this review.
The original timeout/input causes remain unknown; acceptance remains 0/4.
Evidence source: `.github/workflows/ci.yml` and the local scope; run URL
N/A-local.
