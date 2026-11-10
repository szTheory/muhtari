# Phase 15 PRD — Measured milestone tune-up

## Goal

Start v4.0 with a reproducible delivery baseline and one small evidence-supported improvement, while preserving every required check and its fault detection. If the available evidence cannot support a safe improvement, record the exact missing evidence and bounded revisit condition rather than guessing.

## User need

The owner wants the v3.0 host-permission deadlock closed truthfully and core development moving again. The tune-up must be finite and useful: measure current friction, act on one defensible local candidate, and proceed to the independent WebAssembly phase without asking for another host permission or remote attempt.

## Scope

- Follow `.planning/seeds/SEED-007-milestone-tune-up.md` and the current tune-up guidance in `.planning/preparation/ENGINEERING.md` §§4, 7, 8, 11 and 12.
- Reconcile current local CI evidence, documented run metrics, queue/critical-path/cancellation data that is already available, docs freshness and the existing quality ratchet.
- Rate the weakest quality dimension with source links; select at most one small pipeline, quality or documentation improvement supported by current evidence.
- Preserve all required pass/fault checks. Do not alter deadlines, caching, job topology or advisory/required status from censored attempts or a single timing result.
- If no candidate can be safely measured and improved with local evidence, document the concrete evidence gap, bounded revisit condition and why the phase can advance.
- Keep prior Phase 12/14 host and remote stops binding. No permission request, RetroArch launch, remote dispatch, publication or merge is part of this phase.

## Acceptance

1. The baseline and selected quality dimension are reproducible from named local commands or existing receipts, with incomplete/cancelled observations labelled accurately.
2. The phase produces one small measured improvement with representative pass/fault evidence, or a bounded evidence-backed deferral with a named revisit trigger.
3. Updated documentation and planning records leave the v3.0 gaps and v4.0 Phase 16 next step clear; local CI, hygiene and review pass when source/check behavior changes.

## Constraints

- Three Muhtari deliverables remain the C library, headless runner and libretro core.
- Prefer local, offline evidence; no new dependency unless evidence requires one.
- The required implementation and verification workload is expected to stay under one hour; if the evidence points to a larger remedy, defer it rather than silently expanding the phase.
