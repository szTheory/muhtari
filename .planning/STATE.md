---
gsd_state_version: "1.0"
milestone: v2.0
milestone_name: The whole library
status: planning
last_updated: "2026-10-21T14:29:05.791Z"
last_activity: 2026-10-21
progress:
  total_phases: 0
  completed_phases: 0
  total_plans: 0
  completed_plans: 0
  percent: 0
---

# Project State

## Project Reference

See: .planning/PROJECT.md (updated 2026-10-21)

**Core value:** 7800 games behave as they do on the console, identically on every platform, from a small MIT-licensed C library that any host can embed.
**Current focus:** Planning the next milestone.

## Current Position

Phase: Not started (defining requirements)
Plan: —
Status: Defining requirements
Last activity: 2026-10-21 — Milestone v2.0 started

## Accumulated Context

Decisions and detailed execution history live in PROJECT.md,
preparation/DECISIONS.md and milestones/v1.0-phases/. Requirements and the audit
are archived under milestones/. No next milestone scope is active.

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

## Deferred Items

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

Known verification overrides: 15 newly acknowledged, 0 carried forward from a prior close.
Acknowledgment preserves verdicts and lapses when an artifact changes. Future
seeds are still dormant, todos still pending; acknowledgment does not resolve them.

## Session Continuity

Last session: 2026-10-21
Stopped at: v1.0 milestone archived; no further GSD step run.
Resume file: None
Next command: `$gsd-new-milestone`

## Closeout verification

`cmake --workflow --preset ci` — exit 0: 605 regular tests and six release/archive
checks passed after archival. `scripts/hygiene.sh --tree` and `git diff --check` —
exit 0. Archive integrity check — exit 0: all 177 phase files retained, including
73 plans, 73 summaries and four unchanged verification reports; only the deferred
item acknowledgment modifies a moved phase artifact. `query audit-open --json` —
exit 0: zero open, 15 acknowledged. Hosted release evidence is linked above.
