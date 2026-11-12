# Phase 19: Measured milestone tune-up - Context

**Gathered:** 2026-11-11
**Status:** Ready for planning

<domain>
## Phase Boundary

Give Phase 20 maintainers a current v5.0 publication/docs baseline and a proportionate, source-specific quality and efficiency report. Confirm the v6.0 kickoff's current-record correction, assess the six SEED-007 quality dimensions, reconcile the retained v5.0 ship evidence and run the unchanged local native workflow. Use an evidence-backed small correction only if a current discrepancy is reproduced; otherwise record a bounded no-change disposition. Preserve required checks, old failed/cancelled evidence and every Phase 12/14 stop. No YM implementation, new pipeline policy, remote dispatch, host/device run, publication or merge belongs here.

</domain>

<decisions>
## Implementation Decisions

### Current records and evidence
- **D-01:** Treat `fef5691` as the phase's one already-landed current-publication correction. Verify that current project, state, roadmap and product-roadmap records agree with the final v5.0 receipt; do not make a second correction unless a present mismatch is reproduced. If the records are consistent, document no further correction.
- **D-02:** Use the retained v5.0 ship receipt as the only hosted source. Keep the published v0.13.1 source and the later current-main CI source distinct, cite their exact recorded run URLs, and do not refresh remote metadata or substitute another source.
- **D-03:** Score Performance, CI/CD, Ease of use, Architecture, Docs/README and Upgrade path from 1–5 as explicit judgments. Link the supporting observations and label missing, censored or non-comparable data so scores cannot be mistaken for measured product grades.

### Efficiency and proportionality
- **D-04:** Review the retained job durations, longest path, queue-time evidence and cancelled work. Separate observed job time from end-to-end wall time and runner billing; when queue creation timestamps are absent, say queue time is unavailable. Preserve failed and cancelled source-specific outcomes as history.
- **D-05:** Keep caches, deadlines, topology, test membership, required/advisory policy and quality floors unchanged unless the retained evidence demonstrates a concrete safe correction. SEED-009 calls for a small remedy or a reasoned no-change disposition, not a new audit system.
- **D-06:** Run the existing bounded native CMake workflow through `build/ship-local-gate.py` and reuse existing hygiene evidence/mechanisms. Keep raw run output in ignored `build/`; record command, exit, elapsed time and `N/A-local`. This report-only phase adds no behavior test or test framework.

### Scope and agency
- **D-07:** Preserve all nine deferred v3.0 requirements and the original Phase 12/14 host and remote stops. This local work does not authorize any live, remote, identity-bound, publication or merge action.
- **D-08:** Adopt ordinary in-scope recommendations under the owner's autonomous direction; no routine acceptance question is needed. The single-writer orchestrator owns every Phase 19 artifact and shared planning-state update.

### the agent's Discretion

Choose the six scores and strongest evidence-based rationales. Prefer the smallest bounded correction or no-change disposition. Reuse current receipts and checks; do not broaden into a code, dependency, test-inventory or CI architecture audit.

### Reviewed Todos (not folded)

- The pending v1.0 Dependabot/publish review todo and the old wasm32 promotion todo are acknowledged historical pipeline questions; they do not establish a current defect or change this phase's acceptance.
- The pending v1.0 Nightly/ROM-table and MARIA/run-loop review todos concern old implementation findings, including items already recorded as folded or optional seeds. They are outside this documentation/baseline phase.
- The pending v2.0 Phase 11 info-finding todo is optional review bookkeeping, not a current Phase 19 requirement.

</decisions>

<canonical_refs>
## Canonical References

### Scope and tune-up policy
- `.planning/ROADMAP.md` — Phase 19 goal, requirements and acceptance criteria.
- `.planning/REQUIREMENTS.md` — TUNE-10 and deferred v3.0 requirements.
- `.planning/seeds/SEED-007-milestone-tune-up.md` — six quality dimensions and recurring evidence topics.
- `.planning/seeds/SEED-009-proportional-maintenance-review.md` — small evidence-led remedy or bounded no-change rule.
- `.planning/preparation/ENGINEERING.md` §11 — docs standards, review evidence and proportional gates.
- `.planning/preparation/GSD-HANDOFF.md` §3.1 — execution hand-off and bounded retry behavior.

### Current publication and product records
- `.planning/milestones/v5.0-SHIP.md` — authoritative exact-source publication, CI, failure and efficiency receipt.
- `.planning/PROJECT.md`, `.planning/STATE.md`, `.planning/PRODUCT-ROADMAP.md` and `.planning/ROADMAP.md` — current project and milestone records.
- `README.md` and `docs/libretro.md` — user-facing release/install guidance.
- `quality.json`, `CMakePresets.json`, `.github/workflows/ci.yml` and `.github/workflows/release.yml` — current quality bounds and pipeline shape.
- `.planning/milestones/v5.0-phases/17-measured-milestone-tune-up/17-TUNEUP.md` — previous tune-up pattern for score judgments, timing limits and no-change disposition.

### Reusable local checks
- `build/ship-local-gate.py` — existing one-token FIFO-jobserver wrapper around `cmake --workflow --preset ci`.
- `build/phase18-hygiene-gate.py` and `scripts/hygiene.sh` — existing hygiene workflow and tree-scan consumers.
- `tools/check_ci_run.cmake` — source-bound CI receipt auditor, if a retained raw receipt is available.

</canonical_refs>

<code_context>
## Existing Code Insights

### Reusable Assets
- The retained `v5.0-SHIP.md` receipt provides the source-specific release and current-main run identities, historical failures, final-job observations and exact recorded limits. Use it without network refresh.
- `build/ship-local-gate.py` runs the unchanged native CI workflow with a bounded temporary FIFO jobserver. Existing hygiene checks remain the repository's source of truth.

### Established Patterns
- Phase 17's tune-up report separates judgment scores from observations, records missing queue data, preserves cancelled work and uses one bounded no-change disposition.
- The owner-approved hygiene path and staged hook remain authoritative for current tree evidence; no new check is needed for this report-only change.

### Integration Points
- The report links current user guidance and planning records to the terminal v5.0 ship receipt so Phase 20 begins from the correct published source and explicit evidence limits.

</code_context>

<specifics>
## Specific Ideas

Use `fef5691` and the exact-source v5.0 ship receipt as the baseline. No second current-record correction is preselected; confirm the current files before deciding no change.

</specifics>

<deferred>
## Deferred Ideas

No new implementation ideas are added. Phase 20 owns the YM2151 feature; CI changes remain deferred absent a reproduced failure or comparable measurement that supports one.

</deferred>

---

*Phase: 19-measured-milestone-tune-up*
*Context gathered: 2026-11-11*
