---
phase: 27-headless-developer-trace
plan: "05"
status: partial
requirements-completed: []
---

# Phase 27 plan 27-05 checkpoint

## Requirement coverage

coverage:
  - id: TRACE-06
    description: The hooks-off instruction-cost gate passes on an authentic Linux Cachegrind receipt.
    requirement: With tracing disabled, the core preserves the existing hooks-off instruction-cost gate; instrumentation is selected only for calls that have tracing installed.
    verification:
      kind: other
      ref: 27-CI-EVIDENCE.md
      status: pending
    human_judgment: false

## Entry baseline and current progress

- Plan-entry baseline: the existing local correction and all recorded local gates were present; source acceptance was pending; separate remote authorization was absent; the recovery push count was 0; the earlier hosted allowance was spent.
- Current source at entry: `2160cfd4bde9040c93e9addb914a3733972a8934`, runner blob `f634cfcc9b2739d44cc9a9d8de9f2a101f3e6d38`, SHA-256 `0dcf2d4eefcf701b73feeb43a0af9a46aa7abf49601d91c3315764c64010c4d4`.
- Source acceptance status: accepted
- The owner accepted the existing correction `2160cfd4bde9040c93e9addb914a3733972a8934` for the proposed PR update.
- Remote authorization status: read-only observation granted
- Recovery push count: 1
- Proposed push head: `961254179f9d9989d53dd5413a6b9a47f4ef27f0`; its product tree matches the reviewed correction.
- Read-only run queries associated the three reported automatic runs with commit `961254179f9d9989d53dd5413a6b9a47f4ef27f0`. The prior one-push authorization claim and remote ref were not verified in this observation.
- The Linux perf job failed during fixture setup, skipped measurement, and had no `quality-perf` artifact, confirmed through read-only GitHub APIs and its job log.
- Historical source grant remains **UNVERIFIED**; the commit message and prior evidence artifact do not establish it.
- Existing local command results were reconciled from `27-CI-EVIDENCE.md`; no product build, CTest, CI, hygiene, or review command was rerun. The full-CI-before-formatting limitation and the later final-source checks remain explicit.
- `python3 build/phase27-evidence/validate-trace-cost.py sanity` and `local` both passed (exit 0, N/A-local); the recorded results and source reconciled without replaying product commands.
- TRACE-06 remains pending until an authentic same-source Linux Cachegrind receipt passes.

## Next action

The owner accepted the exact source and later authorized read-only inspection of the three reported automatic runs and Linux perf evidence until 2026-11-17T14:41:47Z. The prior ledger reports one push starting at 13:26:47Z; this execution did not verify its authorization or the remote ref, so the reported one-push allowance remains treated as spent. Read-only run queries confirmed CI, Release and Nightly completed with failures. CI's Linux perf job failed before measurement because pinned fixture inputs were absent, skipped the cost steps, and produced no `quality-perf` artifact. TRACE-06 remains pending. No dispatch, rerun, second push, merge or release is authorized. A new gap plan and fresh authority are required before another source correction or hosted attempt.
