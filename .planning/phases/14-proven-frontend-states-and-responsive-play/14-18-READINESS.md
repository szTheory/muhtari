# Phase 14 prerequisite decision

**Status:** Phase 14 acceptance remains blocked. This packet prepares an environment decision for a separately scoped prerequisite investigation; it does not establish frontend behavior or authorize a diagnostic, test, CI run, host launch, or remote operation. FRONT-02, FRONT-03, and FRONT-04 remain pending.

## What the saved evidence establishes

- The independent parent’s complete `cmake --workflow --preset ci` ended with exit 8 after twelve ordinary-test timeouts across title and Maria DMA cases. Their cause remains unknown. Its frozen evidence child passed complete CI and hygiene, but cannot replace the required parent result. Neither workflow is authorized for a retry here. See [Plan 14-15](14-15-SUMMARY.md) and the [parent and frozen-source resource evidence](14-RESOURCE-EVIDENCE.md#plan-14-15-independent-parent-workflow-outcome-2026-11-03).
- Plan 14-17’s single prescribed `ps -axo pid=,ppid=,etime=,time=,command=` observation was denied with exit 126 (`Operation not permitted`). It returned no process rows; no fresh CTest metadata, supervisor, or project test followed, and both workload counters remained zero. That does not establish that no competing process existed. The stop prohibits retrying this observation or substituting another process tool in this environment. See [Plan 14-17](14-17-SUMMARY.md) and its [resource record](14-RESOURCE-EVIDENCE.md#plan-14-17-process-observation-preflight-stop-2026-11-04).
- The one authorized host attempt was [GitHub Actions run 41733613173](https://github.com/szTheory/muhtari/actions/runs/41733613173), attempt 1. The recorded dispatch command exited 0; `gh run watch 41733613173 --exit-status` exited 1. Its state driver failed at a compound preflight before media preparation and before state-mode RetroArch launched. The receipt does not identify which preflight member failed. The attempt is spent; no state-mode picture, progress, save, rewind, run-ahead, or containment result exists. Do not attribute the failure to sandbox access, UDP, load, or another single cause. See [host evidence](14-RESOURCE-EVIDENCE.md#plan-14-11-authorized-host-attempt-result-2026-11-03) and [verification gaps](14-VERIFICATION.md).
- Plan 14-14 separately reproduced and fixed an offline poll-error fail-open in the local containment control. No network request, sandbox, or RetroArch process ran. This local repair neither identifies the historical compound-preflight failure nor proves actual host containment. See [Plan 14-14](14-14-SUMMARY.md) and [MU-HOST-006](../../LESSONS.md).

The six remaining acceptance gaps, two prohibition flags, and actual process-containment/cleanup item are listed in [14-VERIFICATION.md](14-VERIFICATION.md). Historical local receipts and hosted artifacts do not satisfy fresh actual-host acceptance.

## Decision needed

**Recommendation:** designate an owner-controlled native macOS execution context in which the prescribed process enumeration and executable/working-directory attribution are permitted. The agent owns technical qualification and any future checks. The owner is not being asked to run a diagnostic or judge emulator behavior.

The current record does not identify an eligible context. Please provide:

1. The existing local execution context or checkout location that may be used.
2. Its permission boundary, including whether process enumeration and executable/working-directory attribution are available to the agent.
3. How the agent should receive access without disclosing credentials.

Selecting a context permits only planning a separately bounded prerequisite investigation. It does not prove the needed capability, clear the failed parent CI gate, renew CI or host allowances, authorize another inspection attempt, or authorize any workload. A concrete alternate prerequisite remedy may be supplied instead.

## Acceptance evidence still required

| Gap | Fresh evidence required |
| --- | --- |
| FRONT-02 save/load | Actual pinned-host save, advance, restore, and replay with independently judged picture, progress, and complete committed raw-save bytes. |
| FRONT-03 rewind | Repeated actual rewind reversals with deterministic continuation and the existing input, audio, timing, and battery semantics retained. Silent host output is not audible-quality evidence. |
| FRONT-04 run-ahead | One-frame, single-instance active/off observations with restore/replay, isolation, and no speculative durable writes. Compare committed storage only after acknowledged `SAVE_FILES`, then judge the complete committed bytes. |
| Exact source and artifacts | Authenticated implementation source and same-run app, core, runner, library, fixture, and state-mode outputs. |
| Completed work and resource use | Comparable completed baseline/enabled work and actual process, frame, and time observations; no invented performance threshold. |
| Required gates | Passing required aggregate and hosted ASan together with successful actual-host state observations. The passing frozen child cannot replace the independent parent. |

Actual loopback-only containment must be established before launch. Control and reap only the uniquely identified owned RetroArch child within bounded cleanup. Mocks cannot establish either property.

Keep both [prohibition flags](14-VERIFICATION.md#prohibition-review) unresolved until fresh host results rule out substituted callback/mock evidence, concealed committed battery loss, and claims for unsupported modes. Preserve the existing deterministic input/audio/timing/battery proofs and frontend ownership of rewind storage and state files.

## Required order for any successor scope

1. The owner designates the prerequisite environment and access boundary.
2. A separately bounded successor plan qualifies exact source/build/tool identities and process-attribution authority before charging any workload.
3. An independently authorized prerequisite investigation or remedy establishes the complete-parent gate; it cannot substitute the passing child or focused results.
4. The exact implementation source and independent parent pass their required complete local CI and hygiene gates.
5. Only then may a separate concrete checkpoint define publication, CI, and host scope and measured cost.

Any later host packet must freeze source and parent, branch/event/attempt, the RetroArch 1.22.2 macOS 15 pin, same-run artifact identity, admitted fixture and mode, pre-launch containment, unique-child cleanup, committed-storage checks, and the existing work/frame/50-second child/five-minute job bounds unless measured evidence supports an explicitly approved change. Count publication/dispatch cost separately from protected workload cost. A created remote run incurs its scoped cost even if the host workload never starts. Failed prelaunch qualification stops without an alternate tool, environment, or automatic retry; a failure or non-reproduction after workload launch consumes its authorized allowance and stops. Ambiguous launch evidence does not authorize another attempt.

## Assumption review

`detected=true`: the pinned RetroArch host, source/core/fixture identity, and existing bounds remain the same in the report. No second host, identity, or source of truth was introduced, so the assumption decision is **no change**. Every actual-host acceptance item remains pending.

**Documentation check:** `test -s` plus the plan’s trailing-whitespace check; this packet only, no project tests. Run URL: N/A-local.
