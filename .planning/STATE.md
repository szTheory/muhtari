---
gsd_state_version: "1.0"
milestone: v2.0
milestone_name: The whole library
current_phase: 5
current_phase_name: Pipeline tune-up
status: Ready for planning
stopped_at: Phase 5 research blocked on D-17 gate-budget feasibility; no plans created
last_updated: "2026-10-21T14:51:35.409Z"
last_activity: 2026-10-21
last_activity_desc: Phase 5 research recorded; gate-budget feasibility unresolved
state_head: 812cdbb3adeb9b95d8f7ab952b1d37f5e545c583
progress:
  total_phases: 7
  completed_phases: 0
  total_plans: 0
  completed_plans: 0
  percent: 0
total_phases: 7
---

# Project State

## Project Reference

See: .planning/PROJECT.md (updated 2026-10-21)

**Core value:** 7800 games behave as they do on the console, identically on every platform, from a small MIT-licensed C library that any host can embed.
**Current focus:** Phase 5 — Pipeline tune-up.

## Current Position

Phase: 5 of 11 (Pipeline tune-up), first of seven v2.0 phases
Plan: Not started
Status: Ready for planning
Last activity: 2026-10-21 — Phase 5 research recorded; D-17 feasibility unresolved, no plans created

Implementation: 0/7 phases; no plans executed in v2.0. Prior implementation totals belong to the archived v1.0 milestone.

## Accumulated Context

Decisions and detailed execution history live in PROJECT.md,
preparation/DECISIONS.md and milestones/v1.0-phases/. Prior requirements and the audit
are archived under milestones/. The v2.0 scope and phases 5–11 are active; no implementation has started.

### Release evidence

Phase 4 PR #15 merged; CI budget fix #17 merged; release PR #16 merged as 40ce425.
[Release publication and latest-install run](https://github.com/szTheory/muhtari/actions/runs/39533130210) completed successfully; v0.4.0 is published with 19 assets.
`gh run view 39533130210 -R szTheory/muhtari --json status,conclusion,url` — exit 0,
completed success. `gh release view v0.4.0 -R szTheory/muhtari --json isDraft,publishedAt,url`
— exit 0, published and not a draft. Ship-time native/be32/WASM equality, publication
and latest-install checks have completed; they no longer await owner verification.

### Known verification overrides

Phases 1, 2, 3 and 4 have stale canonical fingerprints from later covered-file
changes. The owner explicitly accepted all four at closeout. Reports and digests
were preserved. The audit verdict remains tech_debt; no clean verification is claimed.
Gate-code proportion is 1,367/8,577 lines (15.94%, above the 10% budget).
The performance gate is unarmed and WASM/library jobs remain advisory.

## Deferred Items — v1.0 closeout history

Items acknowledged and deferred at milestone close, most recent first:

| Category | Item | Status | Deferred At | Milestone |
|----------|------|--------|-------------|-----------|
| seeds | SEED-001 | dormant | 2026-10-21 | v1.0 |
| seeds | SEED-002 | dormant | 2026-10-21 | v1.0 |
| seeds | SEED-003 | dormant | 2026-10-21 | v1.0 |
| seeds | SEED-004 | dormant | 2026-10-21 | v1.0 |
| seeds | SEED-005 | dormant | 2026-10-21 | v1.0 |
| seeds | SEED-006 | dormant | 2026-10-21 | v1.0 |
| seeds | SEED-007 | dormant | 2026-10-21 | v1.0 |
| seeds | SEED-008 | dormant | 2026-10-21 | v1.0 |
| todos | 2026-10-16-arm-the-instructions-per-frame-gate.md | presence-only | 2026-10-21 | v1.0 |
| todos | 2026-10-16-close-the-re-review-findings-on-the-dependabot-gate-and-the-publish-checks.md | presence-only | 2026-10-21 | v1.0 |
| todos | 2026-10-16-limit-release-rehearsals-to-pushes-that-change-release-inputs.md | presence-only | 2026-10-21 | v1.0 |
| todos | 2026-10-16-promote-the-wasm32-lane-into-ci-required.md | presence-only | 2026-10-21 | v1.0 |
| todos | 2026-10-19-close-the-phase-2-re-review-findings-when-maria-and-the-run-loop-land.md | presence-only | 2026-10-21 | v1.0 |
| todos | 2026-10-19-close-the-phase-2-review-seeds-on-the-nightly-filter-and-the-rom-tables.md | presence-only | 2026-10-21 | v1.0 |
| deferred_items | 04/deferred-items.md: Tooling seed: `policy.compat_doc_fresh` and its self-test share `build/ci/policy/compatibility.md` as output. One full CI invocation reported a stale page while both ran concurrently; the generated and committed pages were identical afterward, the isolated freshness check passed, and a complete workflow retry passed all 588 regular and six archive tests. The compatibility source, ledger and tests were unchanged by this plan. Give the self-test a separate output during the later tooling tune-up; this is a test/tooling seed under AGENTS rule 7, not a sound requirement or an additional review round. | acknowledged | 2026-10-21 | v1.0 |

At v1.0 closeout: 15 newly acknowledged, 0 carried forward from a prior close.
Acknowledgment preserves verdicts and lapses when an artifact changes. Future
seeds are still dormant, todos still pending; acknowledgment does not resolve them.

## Session Continuity

Last session: 2026-10-21T14:51:35.396Z
Stopped at: Phase 5 research blocked on D-17 gate-budget feasibility; no plans created
Resume file: .planning/phases/05-pipeline-tune-up/05-RESEARCH.md
Next command: `$gsd-plan-phase 5 --research` after new feasibility evidence or an explicit scope decision

Planning blocker: the confirmed broad check inventory is 2,764 physical lines before
embedded workflow/configure checks, against a full-source whole-line cap of 857.
Useful consolidation candidates exist, but no defensible fault-preserving allocation
within that cap was established. The research report records measured facts and
unproven estimates separately. Phase 5 remains unplanned; no readiness override,
budget relaxation, implementation or additional GSD step was performed.

## v1.0 closeout verification

`cmake --workflow --preset ci` — exit 0: 605 regular tests and six release/archive
checks passed after archival. `scripts/hygiene.sh --tree` and `git diff --check` —
exit 0. Archive integrity check — exit 0: all 177 phase files retained, including
73 plans, 73 summaries and four unchanged verification reports; only the deferred
item acknowledgment modifies a moved phase artifact. `query audit-open --json` —
exit 0: zero open, 15 acknowledged. Hosted release evidence is linked above.

## v2.0 initialization decisions

- Owner approved whole-library scope, research first, planning version v2.0 and continuing at Phase 5.
- Souper/BupChip remains deferred; XM covers RAM/POKEY/HSC, AtariVox covers EEPROM; no full OpenHSC.
- All 27 requirements and the seven-phase roadmap were explicitly approved on 2026-10-21.
- Historical v1.0 overrides remain recorded above; initialization makes no new implementation or accuracy claim.

## Initialization verification

`query init.new-milestone` — exit 0: v2.0 The whole library, seven phase directories.
`cmake -DROOT="$PWD" -P tests/cmake/budgets.cmake`, `scripts/hygiene.sh --tree` and
`git diff --check` — exit 0. One-off structural integrity check (`python3 -`) —
exit 0: 27 unique requirements, exact phase coverage, seven phases with 2–5
criteria, 15 historical deferral rows, unchanged outgoing snapshots and seeds.
Archive PR #18 merged as 752916f after [required PR CI](https://github.com/szTheory/muhtari/actions/runs/39614799435)
passed; no release-please pull request was pending after the archive merge.
No phase implementation or owner hardware verification ran during initialization.
