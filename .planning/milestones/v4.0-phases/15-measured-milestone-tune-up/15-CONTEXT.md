# Phase 15: Measured milestone tune-up - Context

**Gathered:** 2026-11-10
**Status:** Ready for planning
**Source:** PRD Express Path (`.planning/phases/15-measured-milestone-tune-up/15-PRD.md`)

<domain>
## Phase Boundary

Start v4.0 with a reproducible delivery baseline and one small evidence-supported improvement, or a precise bounded disposition if current evidence cannot justify a safe change. Keep the v3.0 Phase 12/14 host and remote stops intact. Phase 16 WebAssembly work is the next feature phase. This phase does not retry identity-bound permission, RetroArch, host, remote CI, publication, or merge actions.

</domain>

<decisions>
## Implementation Decisions

### Evidence and improvement scope
- Follow `.planning/seeds/SEED-007-milestone-tune-up.md` and the current tune-up guidance in `.planning/preparation/ENGINEERING.md` sections 4, 7, 8, 11 and 12.
- Reconcile current local CI evidence and existing receipts, queue/critical-path/cancellation data already available, docs freshness and the quality ratchet. Label incomplete or cancelled observations accurately.
- Score the weakest quality dimension against linked evidence. Select at most one small pipeline, quality or documentation improvement supported by current evidence.
- Preserve every required check and its representative fault detection. Do not alter deadlines, caching, job topology or required/advisory status based on censored attempts or one timing observation.
- If no safe candidate is measurable from current evidence, record the exact missing evidence, bounded revisit condition and why Phase 16 can proceed.
- Keep the implementation and verification workload under one hour; defer a larger remedy instead of expanding this phase.

### Project and verification boundaries
- The three deliverables remain the C library, headless runner and libretro core. Prefer offline, local evidence and existing mechanisms; do not add a dependency without evidence that it is needed.
- Do not reopen or retry the denied Phase 12 permission request. Do not start RetroArch, dispatch remote CI, publish, merge, or claim Phase 12/14 acceptance.
- If source or check behavior changes, include representative pass/fault evidence plus required local CI, hygiene and review in the implementation plan. Do not turn this tune-up into a broad historical diagnosis.
- Update planning and user-facing maintainer documentation only where evidence shows it is stale or needed to explain the result and remaining debt.

### the agent's Discretion
- Choose the narrowest evidence-supported quality dimension and one eligible candidate, or choose the bounded deferral path.
- Select existing local commands and receipts that reproduce the baseline; keep measurement comparisons source-linked and distinguish observed from inferred results.
- Split the work into the fewest executable plans that preserve dependency order and stay within the stated workload bound.

</decisions>

<canonical_refs>
## Canonical References

**Downstream agents MUST read these before planning or implementing.**

### Scope and acceptance
- `.planning/ROADMAP.md` — Phase 15 goal, success criteria, dependency and Phase 16 handoff.
- `.planning/REQUIREMENTS.md` — TUNE-08 acceptance and v3.0 deferred scope.
- `.planning/phases/15-measured-milestone-tune-up/15-PRD.md` — approved finite scope, workload and safety boundaries.
- `.planning/milestones/v3.0-MILESTONE-AUDIT.md` — exact deferred requirements and preserved acceptance stops.

### Tune-up evidence and workflow
- `.planning/seeds/SEED-007-milestone-tune-up.md` — required recurring tune-up areas and decision bounds.
- `.planning/preparation/ENGINEERING.md` — current CI design, triage, performance ratchet, documentation and recurring-loop guidance (sections 4, 7, 8, 11 and 12).
- `.planning/preparation/GSD-HANDOFF.md` — bounded recovery, phase/milestone handoff and current project workflow constraints.
- `.planning/LESSONS.md` — reproduced lessons; in particular MU-GSD-026 on keeping denied host evidence separate from offline development.
- `AGENTS.md` — project deliverables, local validation, safety stops and GSD boundaries.

</canonical_refs>

<specifics>
## Specific Ideas

- Start from current-source local CI and retained receipts. Do not spend time trying to infer or repair the old hosted timeout/denial within this phase.
- Carry any justified improvement into the Phase 16 baseline and state exactly what its evidence does and does not establish.
</specifics>

<deferred>
## Deferred Ideas

- The nine unmet v3.0 requirements remain deferred: TUNE-06, TUNE-07, PLAY-01 through PLAY-04, and FRONT-02 through FRONT-04.
- Remote-hosted pipeline changes without comparable evidence; real-host input/permission evidence; Windows virtualization; browser UI, device input/audio, Playstead, actual-browser compatibility and device-performance acceptance.
- Any tune-up remedy whose expected implementation/verification workload is an hour or more.
</deferred>

---

*Phase: 15-measured-milestone-tune-up*
*Context gathered: 2026-11-10 via PRD Express Path*
