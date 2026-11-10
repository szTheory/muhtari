---
audit_acknowledged:
  milestone: v3.0
  at: 2026-11-10
  status: unknown
---

# Phase 12 input acknowledgment: minimum new observation

Prepared 2026-11-01. **Proposal only; no execution authority.** The active
debug record remains investigating. STATE and the post-halt 12-07 closeout
supersede the older timeout-review and publication proposals.

## Finding and blocker

[Run 41323089043](https://github.com/szTheory/muhtari/actions/runs/41323089043)
attempt 1 tested commit `2a78ad8936ca87e479e15201a5337ac60a8f4c6e` on
`phase/12-player-ready-delivery-and-playstead-bridge`. Its complete receipt has
20 terminal jobs and 23 unexpired source-bound artifacts: 17 jobs succeeded,
title skipped, and RetroArch plus the required aggregate failed. Windows ARM64
OpenBIOS passed in 0.14 seconds with a complete stage trace. This does not
explain the historical OpenBIOS timeout.

The failed input step was the **ordinary held/released path**. The trace
producer, diagnostic-helper verification, one-process diagnostic, trace dump
and trace upload all skipped. Neither diagnostic artifact exists. The retained
ordinary log records 34 F8-down posts to one child, the last at 4961 ms,
capture count zero, and `acknowledgment-deadline` at 5061 ms with errno zero and
unknown child status. The DMA screenshot equality passed earlier in that job;
it does not establish the input child's polling or keyboard consumption.

The immediate stop is known; its cause is not. Remaining branches include
frontend startup/polling, callback-returned input/routing, and F8 capture
processing. Draw-observer initialization and audio errors do not distinguish
those branches. The five-second guard precedes `waitpid`, so cleanup status
cannot reconstruct child status at the stopping instant.

Current source already provides an opt-in trace route. No new source repair,
publication, timeout adjustment or local execution is justified by this read.
The missing evidence is actual core-boundary trace data from a permitted host
run. All previous live allowances, including the OpenBIOS diagnostic, are spent.

## Recommended NEW finite scope

Authorize **one new same-source input-trace diagnostic dispatch**, using the
existing unchanged route below, with complete agent-owned gates and terminal
receipt/artifact analysis. This is a new observation, not a retry of the old
OpenBIOS diagnostic or acceptance of ordinary packaged input.

```sh
gh workflow run ci.yml --repo szTheory/muhtari \
  --ref phase/12-player-ready-delivery-and-playstead-bridge \
  -f keyboard_preflight_only=false \
  -f keyboard_probe_only=true \
  -f input_trace_diagnostic=true
```

The fixed source is `2a78ad8936ca87e479e15201a5337ac60a8f4c6e`.
Before submission, the agent must freshly confirm local HEAD, advertised branch
and relevant committed source match that identity, preserve unrelated dirty
work and existing receipt bytes, verify service health and absence of conflicting
active phase runs/open publication, and reconcile the complete newest existing
run/attempt/jobs/artifacts. Any changed source, new conflicting run, failed or
ambiguous required gate stops before dispatch; no automatic migration to a new
HEAD. Use a fresh exclusive private receipt directory and reserve the dispatch
before submission. Submission uncertainty spends the allowance.

No commit, push, source/configuration edit or new local build/test/compiler/
helper/RetroArch run is included. Existing CI performs its unchanged builds,
tests and diagnostic-helper compilation. Its producer verifies the same
source/run/attempt and source-tree manifest before selecting the separate
noninstalled diagnostic module; ordinary archives remain ordinary. The scoped
path skips ordinary DMA and battery frontend launches: at most **one new actual
RetroArch input/content child** is allowed, with the existing nonprompting
permission preflight. No permission prompt/grant, new dependency or pin change.

Keep the existing complete workflow closure: at most 20 job records including
the skipped title and 329 configured runner-minutes. That sum includes build
limits 19/19/19/37/68/78 minutes, ASan 14, two nofp jobs 5 each, be32 15, and ten
other 5-minute jobs including title. Keep every existing per-test/step/job limit,
45-second helper maximum, 450 iterations, five-second acknowledgment guard,
50-second launch wrapper and five-second trace-parser limit. No timeout-policy
review or enlargement is included.

Use a NEW 120-minute observation window starting immediately before the sole
dispatch, covering queue and execution. Allow at most one ordinary cancellation
of the uniquely identified new run at that cutoff or earlier on a scope breach;
observe only through minute 130 for terminal reconciliation. No retry, duplicate
dispatch, job rerun, cancellation retry or window extension. A missing/ambiguous
new-run identity stops further live actions; no broad cancellation. Unresolved
terminal/cleanup state at the endpoint remains unconfirmed and blocks more work.
The agent owns monitoring and source/service/receipt checks; no owner UAT step.

Collect the complete new attempt/jobs/steps/artifact inventory and inspect all
failed jobs, retaining the authoritative required aggregate verdict. Preserve
the source/run-bound diagnostic identity and hashes, raw helper/core traces,
decoded bounded dump, runtime identity, actual input log/config and any images.
No failed or partial receipt is overwritten. Stop after interpreting this one
observation, even if it is inconclusive; no follow-on repair or host attempt.

## What the observation can establish

- A validated positive `POLL a=1` records a completed nonnull frontend callback
  at that instant. Positive port-0 `STATE` records for Up (`a=4`) and B (`a=0`)
  record actual returned signed-16-bit values; nonzero held values alongside
  capture zero separate some input delivery from missing F8 capture. They do
  not identify why F8 failed or prove visible/emulated held behavior.
- Positive `POLL a=0` or `UNAVAILABLE` records identify the observed missing
  callback/instance or nonordinary selected device at that instant. Initial
  zero `STATE` values establish only that observation; they do not prove zero
  throughout the window. Poll records describe callback availability changes,
  not every frame or continued frame progress.
- An absent/empty/invalid/truncated stream, unknown descriptor inheritance,
  clock/pairing failure or known write error cannot prove absent polling or
  input. Killed-core unfinished prefixes have unknown writer health. Valid
  positive records remain observations; missing later records remain unknown.
  Each writer caps at 64 records/4096 bytes; the helper can saturate before the
  acknowledgment deadline, so its suffix is unavailable. Regular-file latency
  is not bounded by the byte cap. Trace overhead can change timing; no diagnosis
  is retroactively assigned to old runs from a new observation.

Even successful diagnostic pixel checks end in the existing intentional scoped
exit 1; they cannot promote PLAY-03 or ordinary package acceptance. Preserve
accepted 0/4, the canonical 12-01 halt, blocked successors and every failed
receipt/spent allowance. No archive, merge, publication or new major phase step.

## Read-only preparation evidence

Private receipts: `/private/tmp/muhtari-input-gap-readonly-20261101-c_lsso_k/`.
No logs are committed. `commands.json` retains exact commands and exits;
`analysis.json` retains source/run/completeness checks and timing observations.

| Read or inspection | Result |
|---|---|
| `git status --short`, `git rev-parse HEAD`, branch and exact retained artifact inventory/hashes | Exit 0; current HEAD agrees with STATE and the hosted source. |
| `gh api repos/szTheory/muhtari/actions/runs/41323089043` | Exit 0; complete terminal attempt-1 run metadata. |
| `gh api 'repos/szTheory/muhtari/actions/runs/41323089043/attempts/1/jobs?per_page=100'` | Exit 0; 20 returned equals total count; every job terminal and source-bound. |
| `gh api 'repos/szTheory/muhtari/actions/runs/41323089043/artifacts?per_page=100'` | Exit 0; 23 returned equals total count; no diagnostic artifacts. |
| `git ls-remote origin refs/heads/phase/12-player-ready-delivery-and-playstead-bridge` | Exit 0; advertised source matches. |
| `curl --fail --silent --show-error --max-time 30 https://www.githubstatus.com/api/v2/summary.json` | Exit 0; Actions/API/Git operational, no active incidents. |
| GitHub connector `fetch_workflow_job_logs` for jobs 124906668581 and 124907544711, once each | Successful tool results (`isError=false`); complete decoded responses retained privately. No shell exit is invented. Actual host step and aggregate both report exit 1. |
| Python source/run/artifact/log inspection | Exit 0; ordinary path/no trace, stopping condition and complete outcome identity validated. |
| Final Python enumerated-source/artifact/content inspection and inner 16-path `git diff --exit-code HEAD` | Exit 0; HEAD and enumerated bytes unchanged; retained input log exactly embedded in fresh source-bound job log after timestamp/ANSI removal. Private `preservation.json`. |
| Required CMake receipt-audit script | Not run: this scope excludes CMake/check execution. The existing required failure is preserved directly from exact metadata and its job log; no passing script-audit claim. |

Preservation covers the enumerated owning source and seven retained artifact
files, not an unperformed whole-tree/index snapshot. Extracted artifact ZIP bytes
were not redownloaded or authenticated anew; the complete retained input log was
matched against the fresh source-bound job log. Only active debug/proposal and
new private bookkeeping were written. No old receipt was overwritten.

The run lasted 40m18s; Windows ARM64 was the longest job at 36m36s. Mac ARM64
queued 10m25s before its 5m34s build; RetroArch then queued 2m46s behind the
matrix barrier and ran 38s. No jobs were cancelled. This one run supports no
safe structural/cache/timeout change; retain the full closure and fault coverage.

The next actor is the owner for this NEW hosted scope decision. The exact
continuation, only after that authority arrives, is:
`$gsd-debug continue phase12-packaged-input-tracer`.
Without it, preserve the stop automatically; repeating the command renews
nothing. Context may be cleared after the handoff.
