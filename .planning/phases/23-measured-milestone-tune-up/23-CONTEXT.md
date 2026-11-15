# Phase 23: Measured milestone tune-up - Context

**Gathered:** 2026-11-15
**Status:** Ready for planning

<domain>
## Phase Boundary

Record the current v8.0 source, publication, CI and quality baseline; inspect current CI timing and reproduced project lessons; run applicable cheap consistency checks before any longer local workflow; and make the smallest evidence-supported correction or document a bounded no-change result. This phase does not reopen the unpublished v0.15.0 source decision, change quality gates, or authorize host, hardware, release, or remote-dispatch work.

</domain>

<decisions>
## Implementation Decisions

### Source and CI evidence
- Anchor the report to merged `main` commit `5a1336f91bf401bab96a8e868e99c661cdca8dae` and retain the immediately preceding `bb70dcf86a3872d3981304bd8c6f664dc58dc401` main run as a separate comparison.
- Use the two complete same-topology main CI receipts, runs 43720418561 and 43725190953, for queue, job-duration, critical-path and cancellation observations; keep each run's source and exact outcome attached to its measurements.
- Keep the Release workflow result distinct from CI. The current release run passed its target check and skipped publication; the v0.15.0 draft remains on its earlier failed exact source.

### Tune-up disposition
- Inspect the current quality floors, required/report-only job membership, and recent lessons before proposing any check or workflow change.
- Treat Windows ARM64's repeated long baseline comparison as the current CI performance bottleneck, but do not change the serial baseline, job deadline, matrix, topology, caching, or required/advisory membership from two observations.
- Make a low-risk correction only if a fault reproduces and the existing fault-detection contract remains intact; otherwise record the bounded no-change result and specific revisit trigger.

### Local verification order
- Run cheap checks first: `git diff --check`, the GSD roadmap analyzer, the existing tree hygiene check, and the CMake hygiene workflow.
- After those four checks and report tasks, run the GSD execute-phase post-plan test gate `cmake --workflow --preset ci` once. Record its local command and exit separately from hosted receipts; this workflow gate does not create a new product-source acceptance claim.
- Do not dispatch or rerun hosted CI. Do not run any additional product suite beyond the single GSD post-plan gate.
- Keep the evidence in the phase report as commands and exits plus hosted run URLs; do not commit logs or generated ROM, firmware, save, or screenshot bytes.

### the agent's Discretion
- Choose the report structure and exact score labels from the evidence, preserve uncertainty and source attribution, and use existing checks and documents without adding dependencies or a new recurring metric system.

</decisions>

<code_context>
## Existing Code Insights

### Reusable Assets
- `.planning/milestones/v7.0-SHIP.md` records the current release decision and source-specific main CI evidence.
- Phase 21's tune-up report provides a source-bounded reporting pattern; `SEED-007` and `SEED-009` define the recurring tune-up and proportional maintenance review.
- Existing checks include `scripts/hygiene.sh --tree`, `cmake --workflow --preset hygiene`, the GSD `roadmap.analyze` query, and the configured `cmake --workflow --preset ci` product workflow.

### Established Patterns
- Keep hosted receipts tied to the exact run SHA and do not treat a later green source as repairing an older release source.
- Use completed measurements only; censored, failed or cancelled work is not timing evidence. One or two observations do not support changing job limits or CI structure.
- Keep required and advisory/report-only jobs visibly distinct and preserve named positive and fault controls.

### Integration Points
- Phase 23 contributes TUNE-12 evidence in `.planning/phases/23-measured-milestone-tune-up/` and updates STATE/ROADMAP progress through the normal GSD closeout.
- Phase 24 remains dependent on this phase and owns the GameDrive source qualification; Phase 23 should not research or plan its contract work prematurely.

</code_context>

<specifics>
## Specific Ideas

The two latest comparable post-merge main CI runs both pass and have no cancelled jobs. Their longest lane is Windows ARM64, with long work in the independent-base quality comparison. Keep the measurements per run, inspect the dependency chain before naming the full critical path, and avoid inferring a trend or a safe CI reduction.

</specifics>

<deferred>
## Deferred Ideas

GameDrive contract research, fixture/oracle decisions, product/API/state implementation, publishing v0.15.0, and all host or hardware checks remain outside Phase 23.

</deferred>
