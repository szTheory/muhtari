---
audit_acknowledged:
  milestone: v3.0
  at: 2026-11-10
  status: unknown
---

# Phase 12 publication and single input diagnostic proposal

> **Current state (2026-10-31):** The reconciliation below supersedes the
> publication and hosted-observation plan retained later in this file. Treat
> that earlier plan as historical evidence only; do not execute it.

> **Diagnostic outcome:** The failed Windows ARM64 lane hit the configured
> one-second CTest limit, not the Actions job/step deadline. One matching
> successful lane log completed the test in 0.15s and 0.06s. The setting is
> measurement-based; keep it unchanged for now. The failed run's 1.53s is
> censored, so its slowdown cause remains unknown. No CI/source/live change or
> additional log sweep follows.

## Current reconciliation and timeout decision

The source mismatch is resolved by the history: the reviewed input-tracing
repair is already published at the current phase-branch HEAD
`7b6d8832058a89ea3ffc1dde18add4f6b832d35b` (`fix(12-06): capture held and
released input in one packaged host process`). The four reviewed source/docs
hashes still match, but the former uncommitted-publication premise is obsolete.
`docs/testing.md` remains locally uncommitted.

Fresh complete terminal records reconcile. Run
[41224305264](https://github.com/szTheory/muhtari/actions/runs/41224305264)
failed Linux preflight. Run
[41255515829](https://github.com/szTheory/muhtari/actions/runs/41255515829)
passed both Linux producers, then Windows ARM64 `built.openbios` timed out
after about 1.53 seconds with exit 8, before RetroArch ran. It produced no
acceptance captures or comparator results. No open PR or active run was found;
GitHub Actions was operational. The read-only reconciliation changed no files
or refs.

The owner authorized a bounded read-only Windows ARM64 timeout diagnosis. It
inspected the failed job and recipe, then made one corrected read of the
source-matched successful candidate. It did not start a 40-log sweep.

### Completed diagnostic scope

- Target: run 41255515829, Windows ARM64 job 124697769109, complete terminal
  log, annotations, recipe and pinned inputs.
- Comparison: run 41224305264, Windows ARM64 job 124605180515. Six relevant
  recipe/pin/preset files matched. One successful log was retrieved and read;
  its two `built.openbios` observations passed in 0.15s and 0.06s.
- The original `gh api` read was refused because the response contained terminal
  escapes. After approval, one corrected read used `--allow-escape-sequences`
  with output redirected to private scratch. The response was filtered locally;
  raw escape sequences were not printed. No further log was fetched.

This diagnostic authorizes no builds, workflow reruns, dispatches, cancellations,
timeout increases, source or workflow edits, commits, publication, live input,
acceptance claims, allowance renewal, or archive. It cannot establish the
original host-side input cause because RetroArch did not run in the latest
attempt. Preserve accepted 0/4, PLAY-03 pending, the canonical halt, downstream
blocks, failed receipts, and all spent allowances.

**Current boundary:** the approved single corrected comparison is complete.
The evidence supports keeping base `TIMEOUT 1` for now: the registration was
set to twice the historical 0.40s maximum from 16 Windows ARM64 runs, rounded
up from 0.80s; the successful comparison passed in 0.15s and 0.06s. The latest
failure is a real outlier, but CTest killed it at the deadline, so 1.53s is not
its completion time. It does not justify a global increase or identify the
cause. Do not fetch more logs or change the timeout in this scope.

## Timeout policy assessment

`tests/built/tests.cmake` records the one-second base as `2 x 0.40 s`, the
slowest of 16 Windows ARM64 measurements, with integer rounding. The shared
`muhtari_add_test` helper multiplies the base by `MUHTARI_TIMEOUT_SCALE`; the
base change would therefore affect other lanes as well. The audited scale is 1
for this Windows ARM64 lane, 2 for Intel macOS, ASan and WASM, and 13 for the
full big-endian suite.

The failed run proves only that the process exceeded the configured one-second
CTest deadline. CTest then kills that test process and continues the suite;
GitHub Actions' step/job timeout is a separate, much larger bound. [CMake's
`TIMEOUT` documentation](https://cmake.org/cmake/help/latest/prop_test/TIMEOUT.html)
describes this per-test kill behavior; [GitHub's workflow syntax](https://docs.github.com/en/actions/reference/workflows-and-actions/workflow-syntax#jobsjob_idsteps-timeout-minutes)
defines a separate step timeout in whole minutes. One successful
Windows ARM64 log with two passed observations at 0.15s and 0.06s supports the
existing headroom, but does not explain the one failed attempt. Keep the current
base. Reconsider only if comparable CI runs repeat the timeout or the pinned
tool/workload changes, using completed per-lane timings rather than censored
kill times. Do not add a timeout-only code change or another log sweep now.

This is evidence for updating the existing timeout lesson, not a new timeout
root cause. No new root-cause lesson is claimed from a single censored failure.

## Read-only diagnostic report (2026-10-31)

### What the complete failed records establish

Run [41255515829](https://github.com/szTheory/muhtari/actions/runs/41255515829)
is terminal `completed/cancelled`, attempt 1, source
`7b6d8832058a89ea3ffc1dde18add4f6b832d35b`. Its Windows ARM64
[job 124697769109](https://github.com/szTheory/muhtari/actions/runs/41255515829/job/124697769109)
is separately terminal `completed/failure`; the overall cancellation must not
replace that job's actual failure. Both Linux producers succeeded. RetroArch
job `124701313784` is cancelled with no executed steps; no acceptance capture
or comparator exists.

The failed deadline belongs to **CTest test #103, `built.openbios`**, inside
[step 5, Build, test, package](https://github.com/szTheory/muhtari/actions/runs/41255515829/job/124697769109#step:5:1).
The complete decoded job log has 2,315 lines. It records:

- `00:11:17.6259187Z`: `Start 103: built.openbios`.
- `00:11:19.1531264Z`: `***Timeout 1.53 sec`.
- `00:24:17Z`: suite ends after `786.49 sec`, with one timeout and fifteen
  fixture dependents not run; the other 661 of 677 tests pass.
- `00:24:17.2555470Z`: `Process completed with exit code 8.`

All timestamps above are 2026-11-01 UTC (the owner's 2026-10-31 local date).
The only annotation is the exit-8 message (`.github`, line 1893), not a maximum
job-time annotation. Its annotation line is a GitHub check coordinate, not the
decoded log's line numbering. The build step continues for almost thirteen
minutes after the test timeout. Packaging, release tests, independent base and
later Windows uploads do not run after the failed workflow test phase.

| Recorded job step | Number | UTC start–end | Seconds | Result |
|---|---:|---|---:|---|
| Set up job | 1 | 00:09:59–00:10:01 | 2 | success |
| Checkout | 2 | 00:10:01–00:10:18 | 17 | success |
| MSVC environment | 3 | 00:10:18–00:10:29 | 11 | success |
| Fetch pinned tools/source | 4 | 00:10:29–00:10:36 | 7 | success |
| Build, test, package | 5 | 00:10:36–00:24:17 | 821 | failure, exit 8 |
| Post checkout | 30 | 00:24:17–00:24:21 | 4 | success |
| Complete job | 31 | 00:24:21–00:24:21 | 0 | success |

Whole job: `00:09:59–00:24:23Z`, 864 seconds. Its unchanged Actions limit is
78 minutes (4,680 seconds); step 5 has no separate Actions timeout. The
immediate failure is therefore **not an Actions step/job limit**. Successful
setup and fetch steps also exclude a recorded setup/pin-download failure;
they do not prove that later host scheduling or process startup was fast.

### Source, command and environment identity

Current HEAD equals the retained run SHA. Local `git diff HEAD` for the
workflow and top-level CMake file is empty. `git show` plus direct byte
comparison establishes the current bytes of `pins.txt`, `tests/built/tests.cmake`,
`tests/built/openbios.cmake`, `cmake/MuhtariTests.cmake`, `CMakePresets.json` and
`scripts/fetch-pin.sh` equal that source. `docs/testing.md` remains independently
dirty and was not changed by this diagnostic.

Step 5's logged command is `cmake --workflow --preset ci-msvc`, exit 8.
The unchanged recipe registers this test command (source-derived, not a
verbose executed argv capture):

```text
${CMAKE_COMMAND} -DWASMTIME=${MUHTARI_WASMTIME} -DPKG=${MUHTARI_7800BASIC}
  -DASM=${MUHTARI_OPENBIOS_ASM} -DFILL=FF
  -DOUT=${MUHTARI_BUILT_DIR}/openbios/openbios.bin -P tests/built/openbios.cmake
```

`tests/built/tests.cmake` gives it `TIMEOUT 1`, citing an older complete 0.40s
maximum from 16 runs (run 39381266556). `muhtari_add_test` multiplies that value
by `MUHTARI_TIMEOUT_SCALE`; this Windows lane logs scale 1. The recipe creates
a fresh output directory, fills excluded `incbin` spans, invokes
`wasmtime run --dir=. <package>/dasm.wasm 7800openbios.asm -f3 -oopenbios.bin`,
then requires the expected output digest. Assembler stdout goes to
`assemble.log`; no internal stage timing or successful completion message
appears in the job log. There is no recorded assembler exit or digest failure.
The log cannot place the lost time in CMake startup, filesystem work, Wasmtime
startup/compilation, assembly, hashing or timeout cleanup. The reported 1.53s
is censored time including timeout handling, **not completed-work timing**.

The runner image is observed `windows-11-vs2026-arm64`, version
`20261025.196.1`, Windows 11 `10.0.26200`, runner `2.338.2`, provisioner
`20261021.618`, Azure `eastus`. The job selects native `HostARM64/ARM64` via
`vswhere -latest`/`vcvarsall.bat arm64`; it logs MSVC `19.51.36260.0`, tools
`14.51.36231`, Windows SDK `10.0.26100.0`. The runner label and latest-toolchain
selection are mutable; these are observed versions, not immutable source pins.
The actual CMake executable version is not printed in the inspected job log.

| Source pin | Version/revision | SHA256 |
|---|---|---|
| Wasmtime aarch64 Windows | 48.0.6 | `a2d54979f5819a1e620b78fa3d5e359517e2de45cbf0fc578331d995188302c4` |
| 7800basic WASM package | 0.41 | `62ef0a562541bff3f813c038a4609abc89ab125ebda3a371f57de2821e7a7fe7` |
| OpenBIOS assembly | b8bc745a8744ae3e0d24cc151e63f669f6a87c24 | `ff04da42db52480cf4cd5e11d415a6c55a54b72ab24af9b461cd4dff4e422109` |

The unchanged fetch script verifies download digests before unpacking. No
repository `actions/cache` restore/save step exists in this lane. The runner
banner's `Cache mode: write` does not identify a Wasmtime cache hit, miss or
configuration. Four tests are co-scheduled when this one starts; that is
context, not proof of contention. No recorded telemetry distinguishes cache,
CPU load, disk/antivirus work or child-startup behavior. A toolchain/cache/runner
cause and a multi-condition cause remain hypotheses, not findings.

### Comparison method, count and required stop

One official completed-CI listing returns 100 recent run metadata records,
newest first. This is metadata, not 100 log comparisons. The method selects
only a terminal successful `build (windows-arm64)` job and requires the six
recipe/pin/preset files above to match the failed run byte for byte before
requesting its complete log. The maximum is 40 successful lane logs, including
current/base observations within each log; no extra log is requested after
the maximum or a failed read.

Only one successful candidate log was retrieved for this scope:
[41224305264/job 124605180515](https://github.com/szTheory/muhtari/actions/runs/41224305264/job/124605180515),
attempt 1, source `7b83b3b914d083a0f49719acfbdb47c359cd342f`, terminal success,
`2026-10-31T17:23:47Z–17:58:32Z` (2,085 seconds). Its six compared source files
match. This is consistent with that prior run failing a separate Linux
preflight; its Windows job is not itself failed.

The first successful-lane log request was exactly:

```sh
gh api repos/szTheory/muhtari/actions/jobs/124605180515/logs
```

It exited **1**, with empty stdout and stderr:
`the response contains terminal escape sequences; pass --allow-escape-sequences to output it anyway`.
The local CLI refused the content; this was a transport failure, not evidence
of GitHub service degradation or a new hosted failure. After the owner approved
one corrected file-safe read, exactly one corrected request was made:

```sh
gh api --allow-escape-sequences repos/szTheory/muhtari/actions/jobs/124605180515/logs
```

The response was redirected to a private file and exited 0. It contained 5,356
lines of UTF-8 job-log text with escape sequences (not a ZIP archive). A local
filter removed the escapes before extracting the two CTest results:
`built.openbios` passed in 0.15s and 0.06s. No raw log was printed. **One
successful lane log retrieved; two test observations; one failed initial read;
no further candidate requests.** This is a single comparison, not a runtime
distribution.

Fresh official [GitHub status](https://www.githubstatus.com/api/v2/summary.json)
was operational during the diagnosis. The connector supplied the complete failed
job log successfully; its job summaries omit timestamps/annotations and it
cannot list the needed general workflow history, so targeted read-only CLI
APIs supplied those fields. The workflows skill's `ci_monitor.cjs` wrapper
is absent from this repository. Private command receipts retain each argv,
exit, stdout/stderr and metadata in
`/private/tmp/muhtari-arm64-readonly-diagnosis-20261031/commands.json`.
The metadata collector exits 0; the initial comparison read exits 1 on terminal
escapes, and the one corrected read exits 0. Retained target evidence remains in
the existing `muhtari-local-oracle-owned-reap-fix-20261031` bundle. No raw log
enters Git.

**Diagnosis status:** the immediate CTest timeout is identified, but the
reason this run exceeded it remains inconclusive. The one successful comparison
shows normal completion under the same relevant recipe, not why the later run
was slow. No timeout increase or local performance measurement is justified by
these data. Keep the existing 1s value and wait for naturally occurring
comparable CI evidence before revisiting; no new run is authorized here.
Accepted 0/4, PLAY-03 pending, canonical halt, downstream blocks, failed
receipts and all spent allowances remain unchanged.

The investigator's preservation verification exited 0 before the follow-up
lesson-ledger update: at that point only the active debug record and this
proposal differed from its baseline; HEAD/index and other enumerated non-build
repository files/symlinks were unchanged. The owner-requested lesson update
also modifies `.planning/LESSONS.md`; it contains no source, workflow, CMake or
test code changes. The check excluded ordinary `build/` contents and `.git/`.

Ancillary local discovery also encountered an unrelated protected temporary
directory during a broad filename inventory and a nonexistent guessed
`tests/built/build_bios.cmake` filename. Neither read was retried; the recipe's
actual `openbios.cmake` reference was subsequently read. These are local
discovery limitations, not missing terminal target evidence or hosted failures.
The required successful-log refusal above is the diagnostic's terminal blocker.

---

## Historical proposal (superseded; retained for evidence only)

Prepared read-only on 2026-10-31. This is a new scope decision, not execution
authority. The local portability repair is reviewed and uncommitted; actual
hosted input remains unresolved. Keep accepted 0/4, PLAY-03 pending, the canonical
halt, downstream blocks, both failed audits and every spent allowance.

## Recommendation and ordinary phase route

Approve taking the concrete scope below into one ordinary Phase 12 gap-planning
step: `$gsd-plan-phase 12 --gaps`. That step should produce a finite reviewed plan
for publishing the two existing test repairs and making one diagnostic attempt.
It grants no publication or live allowance by itself. A later explicit execution
decision must authorize the exact reviewed plan and the finite limits below.
That plan's eventual execution route is `$gsd-execute-phase 12 --gaps-only`,
limited to its newly reviewed gap plan, not the blocked original successors.
Do not create another debug-tooling workstream or repeat passed local controls.

The saved roadmap places 12-01 in wave 1 and blocks 12-02/03/04 behind it. The
installed execute-phase discovery skips plans with summaries and preserves the
halted predecessor block; plain execute-phase therefore cannot resume 12-01.
Gap planning is the ordinary route back to implementation without removing its
honest halted summary. Do not start 12-02 or claim phase verification/completion.
The gap plan must explicitly scope the diagnostic as an intermediate observation:
even a successful diagnostic does not satisfy the original packaged-core truth.
The command requires a discoverable Phase 12 `*-VERIFICATION.md` gap source:
the handoff must record the retained diagnosed failure with truth/reason/artifacts/
missing and `status: gaps_found` before routing. This is evidence bookkeeping,
not a passed phase verification, new source execution or invented owner UAT.

## Exact proposed publication boundary

One genuine commit whose parent is current HEAD
`55f30d64ad1999231a97ea05a96e6a29d061c116`, on the existing phase branch
`phase/12-player-ready-delivery-and-playstead-bridge`, followed by one normal push.
Use normal hooks and the repository noreply identity. No amend, force push, PR,
merge, release or fabricated documentation-only base commit. Stage only these
two reviewed source blobs; preserve all unrelated tracked and untracked edits.

| Path | SHA256 |
|---|---|
| `tests/libretro/test_input_trace.c` | `6032417aeaf3f46138ef262d7a5bffc3f3aee7e7ca2a9bc2e4809ae57482091e` |
| `tests/retroarch/test_input_trace_helper.c` | `e492133dd5d81618a2db24a578c7e8596eb9e26596044a3cd24b0d05b7686475` |

The optional `tests/libretro/input_trace.cmake` remains unchanged at
`d01eff92bb57a864129e3687c9f9de309972f3b52807d5680a56b1f5602d538e`.
The prior 17-path tracer/WASI/wiring publication is already in the parent; it is
not another uncommitted payload. Workflow, pins, runtime bounds, inventories,
normal packages and required CI membership remain the parent's bytes.
Gap-planning documents are separate planning scope, not permission to stage
the dirty planning tree in this proposed source commit. A later planner must
identify any required documentation commit separately before execution approval.

The retained [local outcome](phase12-portability-repair-proposal.md#completed-observer-continuation-and-local-repair-2026-10-31)
establishes strict old/new compiler discrimination, owned-child cleanup and
path-capacity fault detection, one unmodified full workflow, original inventories,
all three package exclusions and an independent repair review with no findings.
Do not replay those passed controls solely to refresh age. Fresh exact source,
HEAD/index/protected-byte and receipt checks remain required before mutation.
Any changed source, new defect or required failed hook stops for a new decision;
this proposal contains no repair reserve or warning suppression.

## Exact proposed single hosted observation

Only after separate execution authorization, reserve one submission, record T0
and its source/branch identity, and run exactly:

```sh
gh workflow run ci.yml --repo szTheory/muhtari \
  --ref phase/12-player-ready-delivery-and-playstead-bridge \
  -f keyboard_preflight_only=false -f keyboard_probe_only=true \
  -f input_trace_diagnostic=true
```

Await push completion and confirm advertised remote SHA before dispatch. Refresh
official service health, complete original/latest receipts, branch/default-workflow
trigger closure, all active-status listings and no open phase PR before publication
and again before dispatch. Require exclusive phase-branch coordination through
terminal reconciliation; no concurrent push, PR or dispatch by another session.
Submission uncertainty spends the single allowance: reconcile, never resubmit.
Bind the unique resulting run to the genuine new source, ref, dispatch event and
attempt 1. Preserve the original input provenance and every historical receipt.

Keep the unchanged 20-job closure and 329 configured runner-minutes (324 runnable
excluding the skipped title check). This is an allocation bound, not predicted
duration, a billing estimate or strict dollar cap. The same-source diagnostic
producer must supply its exact run/source/attempt identity, source manifest and
verified module/parser digests before use. Reconcile all jobs, executed logs,
steps and artifacts, including advisory WASI; do not demand speculative historical
artifact/step counts after skipped or failed producers. Missing evidence is not
reconstructed success. The existing required aggregate remains authoritative.

At most one actual RetroArch child, only after the existing nonprompting event
access preflight under the hosted identity. Denial stops before content. Keep
RetroArch 1.22.2/git69a4f0ea and DMG SHA256
`81b79121ba26d539064ae13b4d0419a120c3d165afbe656cf5f5412b15fdb434`,
the owned fixture, 600 frames, five-second acknowledgment, 45-second helper,
50-second wrapper and five-minute job. Retain existing bounded pin fetching;
no extra acquisition/install, permission prompt/grant/settings change, alternate
content/host/input/audio, second control child, retry, rerun or fallback.

Use a new queue-inclusive cancellation-request cutoff T0+120 minutes and final
observation endpoint T0+130 minutes only if explicitly authorized later. At most
one ordinary cancellation of the uniquely reconciled new run if still active at
cutoff or on an earlier identity/closure mismatch, after fresh identity/status
checks. No guessed newest-run, historical or force cancellation. Cancellation is
asynchronous and cannot guarantee a strict spending cap; at the final endpoint,
retain any still-active run as unresolved and stop without renewal.

The helper/core trace must retain its existing prefix/completeness/writer-health
limitations. Valid nonzero STATE proves returned held values only at recorded
instants; missing records alone do not prove absent input. Clock comparison needs
same-host clock provenance and owned-child identity. INIT-only, invalid, saturated
or unfinished records remain qualified observations. Judge available images as
agent-owned evidence; ask the owner for no verification. Even correct held/released
diagnostic pixels preserve the deliberate scope-only exit 1, accepted 0/4,
PLAY-03 pending and downstream halt. Diagnostic-module evidence cannot substitute
for the extracted ordinary package, DMA/battery controls or Playstead integration.

## Choice, risks and stopping rule

Doing nothing preserves the halt at no new live cost. The recommended smallest
useful next step is the gap plan for this two-source publication and one unchanged
diagnostic route. Its cheapest discriminating hosted observation is whether the
required Linux producers now compile and, if dependencies permit, whether the
same-source diagnostic records show actual returned input before the unchanged
capture deadline. The strongest alternative is reopening host/input architecture;
no new evidence currently justifies that broader change.

The original readiness/routing/F8/audio cause remains unknown. The local cached
GCC/glibc differs from the hosted toolchain, Linux runtime is unverified, producer
failure or queue delay can consume the attempt before input runs, and the
diagnostic module is not shipped-core acceptance. A fresh failure inconsistent
with the predicted repaired producer outcome, source drift, receipt mismatch,
service incident, unexpected CI closure or cleanup uncertainty overturns readiness
to proceed and stops without repair/retry/extra commit. New evidence may inform
the next ordinary gap decision; it does not renew authority automatically.

## Evidence and handoff

`python3 /private/tmp/muhtari-publication-proposal-preflight.py` exits 0.
Fresh complete run/attempt/job/step/artifact/all-executed-log receipts match
both retained failures: [original run](https://github.com/szTheory/muhtari/actions/runs/41158640754)
and [latest run](https://github.com/szTheory/muhtari/actions/runs/41208847091).
The respective complete inventories are 20 jobs/191 steps/20 artifacts/19 logs
and 20 jobs/170 steps/15 artifacts/16 logs. Both fresh required-aggregate jobs
remain failed; both retained repository audit exits remain 1 and were inspected,
not replayed. The diagnostic ZIP digest/source/run/attempt/source-manifest and
member hashes reconcile. Official health is operational and the historical
incident resolved; remote source agrees, no open phase PR or active CI/new source
run exists. HEAD/index/protected bytes agree. Private scratch receipts are
`muhtari-publication-proposal.qsyrudw9`; no raw logs enter Git.

`python3 /private/tmp/muhtari-publication-proposal-preservation.py` exits 0,
N/A-local: only this new proposal path differs from this step's baseline;
HEAD/index/every pre-existing tracked/untracked byte remains unchanged,
including the two repaired C sources, all pre-existing planning changes,
optional unchanged consumer and retained local/first-failure receipts.
Retained local full-workflow/package evidence and sole repair review PASS
reconcile. Proposal links and `git diff --check` exit 0. Detailed command/exit/
run URLs are in private `report.json` and `preservation.json`.

One CLI-resolved independent proposal review (`gsd-plan-checker`, luna/xhigh)
identified one blocker: absent discoverable Phase 12 gap input. All other reviewed
scope, hashes, failure retention and finite limits were materially sound. The
route above is explicitly conditional on recording that truthful gap input;
the blocker remains open until that artifact exists. No further review fan-out
or source-repair review is proposed.

Retained tests and audits are inspected only;
no repository check, compiler, target, workflow or source program is run in this
proposal step. No source change, staging, commit, push, dispatch, input activity,
allowance renewal, archive or promotion is authorized here.

Owner acts next only on the new gap-planning scope decision. Context may be
cleared after this proposal handoff. Exact next separate command after that
decision and the gap-source prerequisite: `$gsd-plan-phase 12 --gaps`.
The next agent owns fresh service/source/
complete-receipt checks and must preserve the existing halt. No acknowledgment
is needed merely to keep stopped, and execution remains a later concrete decision.
