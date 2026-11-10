---
status: executed-terminal-failure
date: 2026-10-31
scope: Publish verified three-file repair and observe one instrumented keyboard-only CI experiment
audit_acknowledged:
  milestone: v3.0
  at: 2026-11-10
  status: executed-terminal-failure
---

# Diagnostic publication and one host experiment

**Execution is terminal and fully audited.** The approved commit, push, dispatch
and one actual child are spent; unused conditional cancellation expired when the
run completed. No retry, further source repair, publication/live allowance or
phase promotion follows. Preparation-time statements below are historical.

Owner answered **"yes"** to the preceding concrete publication/live-scope
question, including exclusive branch use across owner sessions until
reconciliation. This approves only this proposal's one three-file commit/push,
one329configured-minute keyboard-only dispatch,120+10-minute window, one
conditional ordinary cancellation and one bounded actual child. Prior
allowances stay spent. Earlier proposal-time no-authority text below is
historical. Execution begins with fresh agent-owned checks; no retry, expanded
scope or phase promotion follows.

This is a new proposal, not execution authority. Previous source-publication,
dispatch, actual-child and cancellation allowances remain spent or expired.
The [capture proposal](phase12-capture-live-proposal.md) and every failed receipt
remain historical. No commit, staging, push, dispatch, rerun, cancellation or
actual frontend/content launch occurred during this preparation.

Recommend publishing the already verified local portability and failure-reporting
repair, then allowing one bounded keyboard-only experiment. The useful new
observation is the helper's exact stopping reason. This does not repair or prove
frontend readiness, input routing, hotkey gating or the historical capture cause.
Another no-ack result alone does not justify a retry or a larger timing window.

## Exact source to publish

Repository `szTheory/muhtari`; phase branch
`phase/12-player-ready-delivery-and-playstead-bridge`; workflow
`.github/workflows/ci.yml` (`CI`). Local HEAD and advertised remote both remain
`26c8268f9c56db39308287eaceb935e97eaf3a77`. Its genuine parent is
`186c0cc4b39c12fb149ddc0ef3f43897fba55865` and tree is
`a0e62cff0d62f8d3153b554001c7e1f58a595dc8`.

Request one genuine source/test/documentation commit, with required parent
**26c8268**, containing exactly these three complete working-tree files:

| Path | SHA256 of proposed bytes |
|---|---|
| `tests/retroarch/keyboard_preflight.c` | `8f201fb1c7199c4a59e61523c1af4508384049f84e628b5079cb6842c505d288` |
| `tests/policy/retroarch_preflight.c` | `3dccd5c0728f5b10f5505aaa6f683c1f4d3b1178c23f4e8794f3ee5132ef6d77` |
| `docs/testing.md` | `94ab98be9297cf94aa7f0f1eee126b4fc473bcf64e95716eea32c6b93609415b` |

The mock preserves the real clock identifier type and missing-clock fallback.
The helper names stopping conditions, retains saved relevant API errors and last
valid observations, and reports observed child status only before cleanup.
Deterministic tests exercise valid/fault outcomes and simultaneous-fault guard
precedence. These are the tested bytes from the
[approved local batch](phase12-held-capture-diagnosis.md#approved-local-clock-type-and-failure-reporting-batch).
All source outside those paths, workflows, pins, permission handling, predicate
order, return values, five/45/50-second bounds and image judges stay unchanged.

Every other existing working-tree edit and untracked file stays outside the
source commit, including this proposal, active/historical debug files, STATE,
LESSONS, canonical summary, original DECISIONS/config/GSD-HANDOFF edits and
browser-player/interface research. Use exact-path staging, the noreply identity
and ordinary hooks; no amend, force-push, documentation-only parent or extra
source commit. Record actual full new SHA/tree/parent and compare staged and
committed blobs with this manifest before the sole normal push. Stop on drift.

Current/base quality production and its independent join will use genuine
26c8268 as base, not the older parent186c0cc. Both have the same registered
ordinary/release inventory. The old advisory WASI compile defect remains in that
historical base; quality's base producers are native/hygiene/ASan, not WASI.
Dispatch performance repeats the new HEAD and proves no phase-versus-main
performance comparison. Publishing source and CI candidate artifacts makes them
available in the public repository; this grants no release, PR, merge or issue
publication.

## Trigger closure and coordination

Require an owner-exclusive branch window for this agent, including every other
owner session/agent, from approved execution through terminal reconciliation or
an unresolved live-state checkpoint. No owner/other-session push, PR creation,
dispatch or rerun on this branch during that window. This is coordination, not
a mechanical GitHub lock. A no-open-PR query alone is non-atomic: a concurrent PR
could trigger ordinary CI and Nightly with additional host processes outside
this proposal. The prior review's race finding is retained, not relabeled fixed.

Fresh preparation finds no open phase PR, no nonterminal CI in the latest100
records or the five explicit nonterminal-status listings, only the owner with
write/admin access, and unchanged six repository
workflows plus independent provider-managed Dependabot Updates. CI/Release push
triggers admit main only; no workflow_run trigger or outgoing reusable-workflow
call extends this manual CI run. Scheduled Nightly/Digest and Dependabot work are
independent and not authorized here. Refresh complete active-status listings,
PR/branch/source/trigger state and official service health before mutation.
Stop if exclusivity or the inspected closure cannot be established. After push,
await completion and independently verify the advertised new SHA, no unexpected
automatic run and no nonterminal same-source run before submission.

Sole dispatch, with no workflow_call ref:

```sh
gh workflow run ci.yml --repo szTheory/muhtari \
  --ref phase/12-player-ready-delivery-and-playstead-bridge \
  -f keyboard_preflight_only=false -f keyboard_probe_only=true
```

Full dependent closure is changes/hygiene; six native build legs, ASan, two nofp
legs, be32, library and perf; hash-equality, advisory WASM, RetroArch and quality;
then the authoritative CI required aggregate. Title skips outside PR. Native
fail-fast=false leaves submitted peers running after another fails. A native
producer can consume the attempt without reaching the host. Concurrency group
is `ci-CI-<new-full-SHA>`, cancel-in-progress=false; default pending replacement
still applies to a concurrent same-group submission. No such submission is
allowed by this proposal.

## Cost, queue window and cancellation

| Job / expanded legs | Configured minutes |
|---|---:|
| changes / hygiene / skipped-title reserve | 5 + 5 + 5 |
| native linux-x64 / linux-arm64 / macos-arm64 | 19 + 19 + 19 |
| native macos-x64 / windows-x64 / windows-arm64 | 37 + 68 + 78 |
| ASan / two nofp / be32 | 14 + 5 + 5 + 15 |
| hash-equality / advisory WASM / library / RetroArch / perf / quality / CI required | 7 x 5 |
| Total20 possible job records | **329** |

Runnable19 jobs sum324 minutes; the skipped-title reserve contributes5. These
are summed configured allocations, not predicted elapsed time, billing units,
dollars or a strict spending cap. No deadline change is proposed.

After commit/push and fresh checks, reserve actual UTC T0 immediately before
dispatch, recording T0+120-minute queue-inclusive cancellation-request cutoff
and T0+130-minute final observation endpoint before action. No window or quota
starts during preparation. An uncertain response spends the sole submission;
reconcile read-only rather than resubmit. Confirm unique full source, branch,
workflow_dispatch event and attempt1; preserve exact dispatched input provenance.

Request at most one ordinary cancellation of only that uniquely identified new
run if still nonterminal at the120-minute cutoff, or earlier on an observed
source/event/input/closure mismatch. Refresh identity/status first; no guessed
newest-run target, historical/unrelated cancellation, force-cancel, second
cancellation, extension or fallback. If unique identity is unavailable, preserve
an unresolved live-state stop. Observe only to the original130-minute endpoint.
Cancellation is asynchronous and eligible always/failure cleanup may continue;
termination and a strict cost cap cannot be guaranteed. This follows the fresh
[GitHub cancellation reference](https://docs.github.com/en/actions/reference/workflows-and-actions/workflow-cancellation)
and [concurrency reference](https://docs.github.com/en/actions/how-tos/write-workflows/choose-when-workflows-run/control-workflow-concurrency).

## Actual process and discriminating outcomes

At most one actual RetroArch child on the existing macos-15 arm64 identity after
fresh nonprompting event-access preflight, with600frames,45-second/450-iteration
helper bound inside50-second wrapper and five-minute host job. Preserve pinned
RetroArch1.22.2/git69a4f0ea, DMG SHA256
`81b79121ba26d539064ae13b4d0419a120c3d165afbe656cf5f5412b15fdb434`,
candidate archive source/digest binding and intended PID targeting. Denial77
ends the attempt; no prompts, grants, settings changes or global event target.
Ordinary DMA/input-loop/battery host launches skip. The owned test diagnostic
and existing headless image judges are the only content in scope.

| Observation | What it decides |
|---|---|
| acknowledgment-deadline with capture0 | New run hit the existing guard; readiness/routing/hotkey cause and unobserved child state remain unknown. |
| clock/directory/event/wait/sleep/fork error | A different immediate stopping path falsifies the no-ack-guard hypothesis for this attempt. |
| child-completion before guard | Status was actually observed; cleanup reaping cannot provide retrospective status. |
| timely isolated capture then held/released pixels and independent equality pass | Feasibility for this attempt only; raw-file existence alone is insufficient. |
| WASI compiles and reaches execution | Local portability repair progressed beyond this attempt's compile failure; runtime success remains independently required and advisory. |

Exact return/status/order behavior remains. Added logging has overhead and cannot
identify the old run's branch retrospectively. Missing/extra/disappearing capture,
failed pixels, preflight denial or any producer failure spends the attempt; no
automatic retry. Both image judges passing still reaches the existing deliberate
scope-only exit1 and failed required aggregate. Accepted0/4, PLAY-03 pending and
plans12-02 through12-04 blocked remain in every outcome. No phase promotion,
debug archival/resolution, published-byte replay or next GSD step follows.

## Evidence and recommendation

`python3 /tmp/muhtari-diagnostic-proposal.7VvQPJ/prepare.py` exits0 at
2026-10-31T01:54:40Z. Fresh connector jobs/artifacts plus CLI/run/attempt APIs
reconcile all20jobs/191steps/20artifacts against the completed
[run41150939552](https://github.com/szTheory/muhtari/actions/runs/41150939552).
Results remain16success/1skipped/3failure, with no frame artifact. Existing
`check_ci_run.cmake` exits1 for the failed required aggregate. Source/tree/remote,
all proposed tested hashes and all workflow hashes match; official health is
operational and the old incident resolved. Remote inventory/default workflow,
PR, recent/source CI and collaborator reads each exit0. Raw responses and
manifests stay in scratch; no logs are committed. Local run URL N/A-local.
`python3 /tmp/muhtari-diagnostic-proposal.7VvQPJ/active.py` separately checks
queued/in_progress/requested/waiting/pending CI listings: each exits0 with zero
runs, independent of the newest100-record limit.

Prior full local workflow, exact ordinary/release inventories, WASI compile-only,
native/fallback fault executions, standalone links and bounded code review are
reused only because every tested source byte is unchanged. They establish no
fresh hosted success. This continuation changes proposal/bookkeeping only.

Two bounded read-only specialists independently assessed diagnostic value and
publication/closure; parent synthesis retains the known branch race and limited
causal value. No new deterministic blocker was found. Alternatives: preserve
halt at no cost; publish only and learn no new host branch; or design readiness/
routing instrumentation, requiring a different local design/verification scope.
Recommend this one instrumented attempt only if the owner wants that specific
host observation at the stated cost. More synthetic tests cannot supply it.

Await the owner's new concrete publication/live-scope and branch-coordination
decision. No user action is needed merely to retain the stop. Context may be
cleared; next separate command remains
`$gsd-debug continue phase12-packaged-input-tracer`. The agent owns fresh health,
source, active-run and receipt checks after any approval; invocation by itself
renews no allowance.

Final preparation checks: `python3 /tmp/muhtari-diagnostic-proposal.7VvQPJ/verify.py`,
`sh scripts/hygiene.sh --tree`, `git diff --check`,
`cmake -DROOT="$PWD" -P tests/cmake/budgets.cmake` and GSD
`query summary-extract .planning/phases/12-player-ready-delivery-and-playstead-bridge/12-01-SUMMARY.md --fields requirements_completed`
each exit0, N/A-local. Only this new proposal and three shared handoff records
changed during preparation; tested source, workflows, all other existing edits,
HEAD and index are preserved. All relative file links resolve; the summary still
declares no completed requirements and the phase remains blocked0/4. This is
proposal/bookkeeping verification, not a new code-test or hosted acceptance run.

## Approved execution receipt

Sole approved three-file commit and normal push exit0; sourcecee6dea491918282d034f8c6e9a90087d8213439, genuine parent26c8268f9c56db39308287eaceb935e97eaf3a77, treeea4b7b7425da353e31e008070458549756cb6ee1. Exact reviewed source manifest/index/committed-blob assertions0; protected owner edits preserved. Fresh pre/postpush gates0, advertised full SHA0, noPR/noactiveCI/noautomatic-newsource run. Sole dispatch RESERVED 2026-10-31T02:08:46Z; cutoff 2026-10-31T04:08:46Z; observation endpoint 2026-10-31T04:18:46Z. Command: `gh workflow run ci.yml --repo szTheory/muhtari --ref phase/12-player-ready-delivery-and-playstead-bridge -f keyboard_preflight_only=false -f keyboard_probe_only=true`. Reservation spends the one attempt even if response is uncertain.

## Terminal outcome

[Run41158640754](https://github.com/szTheory/muhtari/actions/runs/41158640754)
is completed/failure, exact sourcecee6dea/attempt1, terminal2026-10-31T02:45:31Z.
All20jobs/191steps and all20source-bound unexpired artifacts reconcile:
16success, allowed title skip and3failure (RetroArch, advisory WASM, required
aggregate). Every executed job log is retained once: host through the successful
connector,18other logs through direct CLI; no skipped-title log invented.
The existing required audit exits1 solely for the failed required aggregate.
No cancellation or retry occurred; cancellation authority expired at terminal.

The host passed fresh nonprompting event-access preflight and used the unchanged
pinned DMG/package and macos-15-arm64 image20261028.0489.1. Actual candidate
core ZIP SHA256 is
`32ba11544d44a0d12315c93d8139d09b435f8f2d97deffe225a871a875296a63`;
the source-bound parent artifact digest and ZIP CRC match. Exactly one child,
PID1933, logged35F8-down observations through4988ms, allcapture0, plus initial
Up/Z down. The explicit stop is **acknowledgment-deadline at5090ms,
capture0, errno0, child_status=unknown**. This confirms the immediate guard for
this attempt; the timer/guard still precedes child polling. Cleanup status is
not retrospective child-state evidence. Repeated failure-report output is the
same process, not another launch.

No raw/terminal PNG or frames artifact exists. Release, expected-image generation
and pixel judgments were not reached; agent image review is impossible without
images. Ordinary DMA/input-loop/battery launches skip as scoped. Frontend audio
initialization errors remain visible without causal attribution. Readiness,
routing/hotkey gating and historical failed-run cause remain unproved. A repeat
no-ack result does not justify another run or widening the window.

All six native current/base677ordinary+6release suites, package production and
three uploads per lane pass. Intel completes596s and Windows-arm642020s under
unchanged37m/78m allocations. Other required peers pass; quality uses genuine
26c8268 base, while dispatch performance remains same-head. These are not
phase-versus-main performance or full host acceptance claims.

WASI now compiles and its ordinary deterministic preflight mock test passes:
the pointer-clock compiler defect is repaired on the actual target. The lane
remains advisory/unaccepted:605of606tests pass, but the separate child-exit
negative control expects127 while the runtime returns1 with the explicit
`exit with invalid exit status outside of [0..126)` diagnostic and the expected
argv/no-input/no-wait marker. This re-exposes the earlier runtime boundary after
the compile fix; no successful full WASI inventory/hash or runtime repair is
claimed. No native exit contract, runtime pin or assertion was changed.

| Command / action | Exit | What it establishes |
|---|---:|---|
| `python3 /tmp/muhtari-diagnostic-exec.Hak1cS/prepare.py` and `active.py` | 0 / 0 | Fresh source/old complete receipts/official health/noPR/noactiveCI/workflow closure before mutation. |
| `python3 /tmp/muhtari-diagnostic-exec.Hak1cS/checks/check.py` | 0 | Fresh strict native/fallback valid/fault executions, WASI compile-only, real helper and standalone links, existing focused policies, docs/format/diff. No actual local helper/frontend/content launch. |
| Exact ordinary/release `check_junit.cmake` commands from approved local batch | 0 / 0 | Existing unchanged full-workflow677/6 inventories retained; full suite was not redundantly rerun. |
| Exact-path staging, staged hygiene, hook, committed-blob/parent/path assertions and sole commit | 0 | Only three reviewed files published with genuine parent; protected worktree bytes preserved. |
| `gates.py prepush`, sole normal `git push`, `gates.py postpush` | 0 / 0 / 0 | Approved remote/source/tree, no automatic new-source run, operational/resolved and unchanged closure. |
| `dispatch.py`, `identify.py`, bounded read-only `observe.py` | 0 / 0 / 0 | One reserved dispatch and unique exact-source/event/ref/attempt1, original window through terminal. |
| `python3 /tmp/muhtari-diagnostic-exec.Hak1cS/terminal.py` | 0 | All jobs/steps/artifact identities, all executed logs and protected source reconciled. |
| `cmake -DINPUT=/tmp/muhtari-diagnostic-exec.Hak1cS/terminal-view.out -DEXPECTED_SHA=cee6dea491918282d034f8c6e9a90087d8213439 -P tools/check_ci_run.cmake` | 1 | Required hosted verdict stays failed; not observer error. |
| `python3 /tmp/muhtari-diagnostic-exec.Hak1cS/media.py` | 0 | No frames artifact; actual candidate archive source/digest/CRC/core ZIP identity. |
| `python3 /tmp/muhtari-diagnostic-exec.Hak1cS/audit_outcome.py` | 0 | Six complete native productions, exact new helper reason, separate WASI runtime failure and required membership. |

Hosted run URL is the exact run above; local commands are N/A-local. Raw logs
and response bodies remain only in scratch. Shared handoff is uncommitted;
no additional documentation/source commit is authorized or needed here.
Accepted0/4, PLAY-03 pending and all downstream blocks remain.

This approved step is finished; the debug issue is unresolved and unarchived.
Preserve the stop automatically. Context may be cleared. Exact next separate
command: `$gsd-debug continue phase12-packaged-input-tracer` for read-only
assessment of a minimal readiness/routing observation and the WASI127 runtime
test boundary only. The agent owns fresh service/source/receipt checks. No
source repair, timing/pin/permission change, content launch, commit/publication,
dispatch/rerun/cancellation, renewed window or phase promotion is authorized.
No user acknowledgment or verification is required merely to remain halted.

Final handoff receipts: `python3 /tmp/muhtari-diagnostic-exec.Hak1cS/final_refresh.py`
exits0 at2026-10-31T02:50:35Z, refreshing exact terminal/source/attempt/all20jobs/
all20artifacts, advertised remote, unique source run/noPR and primary official
operational/resolved health; its required checker remains1. `active.py` exits0
with all five nonterminal CI listings empty. `verify_handoff.py`,
`sh scripts/hygiene.sh --tree`, `git diff --check`,
`cmake -DROOT="$PWD" -P tests/cmake/budgets.cmake` and GSD
`query summary-extract .planning/phases/12-player-ready-delivery-and-playstead-bridge/12-01-SUMMARY.md --fields requirements_completed`
each exit0,N/A-local. Only five authorized handoff/lesson records changed after
the approved three-file source snapshot; all other owner edits, workflows,
ROADMAP, HEAD/parent, committed source bytes and empty index are preserved.
Relative file links resolve; halted summary/blocked0of4/no-live-authority agree.
