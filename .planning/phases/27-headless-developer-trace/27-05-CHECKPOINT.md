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
- The owner accepted only the existing correction `2160cfd4bde9040c93e9addb914a3733972a8934` for the separate publication decision.
- Remote authorization status: granted
- Recovery push count: 0
- Historical source grant remains **UNVERIFIED**; the commit message and prior evidence artifact do not establish it.
- Existing local command results were reconciled from `27-CI-EVIDENCE.md`; no product build, CTest, CI, hygiene, or review command was rerun. The full-CI-before-formatting limitation and the later final-source checks remain explicit.
- `python3 build/phase27-evidence/validate-trace-cost.py sanity` and `local` both passed (exit 0, N/A-local); the recorded results and source reconciled without replaying product commands.
- TRACE-06 remains pending until an authentic same-source Linux Cachegrind receipt passes.

## Next action

Both owner gates are recorded separately; the remote allowance is unused (push count 0). Before any push, rerun the read-only evidence and authority judges and confirm PR 77 is still open, draft, based on `main`, and at the expected remote head. If any source, PR, identity, or authorization check differs, stop before publishing. Do not run a manual dispatch, rerun, second push, merge, or release.
