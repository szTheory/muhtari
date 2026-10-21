---
gsd_state_version: "1.0"
milestone: v2.0
milestone_name: The whole library
current_phase: 5
current_phase_name: Pipeline tune-up
status: executing
stopped_at: Phase 5 research and nine plans verified; ready to execute
last_updated: "2026-10-21T17:35:10.215Z"
last_activity: 2026-10-21
last_activity_desc: Phase 5 research refreshed; nine plans verified, ready to execute
state_head: c7b17edf5b8fc1bed7f8c01d5eea14e6481772c1
progress:
  total_phases: 7
  completed_phases: 0
  total_plans: 9
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

Phase: 5 (Pipeline tune-up) — READY TO EXECUTE
Plan: 0 of 9 plans executed
Status: Ready to execute
Last activity: 2026-10-21 — Phase 5 research refreshed; nine plans verified, ready to execute

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

Last session: 2026-10-21T17:34:35.542Z
Stopped at: Phase 5 research and nine plans verified; ready to execute
Resume file: .planning/phases/05-pipeline-tune-up/05-01-PLAN.md
Next command: `$gsd-execute-phase 5`

Current planning contract: D-17 and D-19–20 now use an audited absolute ratchet.
The prior 857-line prerequisite is explicitly superseded; no exact new cap or
compliance result is claimed. Targeted research is refreshed and nine plans
passed independent checking with all five requirements and twenty decisions covered.
The first execution slice freezes B, proves nonzero net S, and fixes N/C before additions.

Prior research blocker (historical): refreshed research executed 250 lines of scratch shared primitives,
archive and roll-up replacements, and an incomplete quality prototype. Their positive
and fault-specific negative controls passed. The tested strategy's optimistic broad
construction is 2,262 lines before embedded/configure/reporting enforcement, against
the full-source whole-line cap of 857. This failed construction is not proof that all
possible implementations are impossible. No supported complete allocation was found.
The 2,764-line baseline and broader sensitivity remain explicit; no enforcement was
hidden or budget relaxed. The researcher returned RESEARCH BLOCKED; no planner,
checker, production changes, hosted writes or additional GSD step ran.

`python3 /tmp/muhtari-p5-controls.py` and
`python3 /tmp/muhtari-p5-policy-controls.py` — exit 0 on the one-off scratch
experiments; `wc -l /tmp/muhtari-p5-{common,archive,quality,rollup}.cmake` — exit 0,
250 lines. Scratch prototypes are not production or CI verification. Fresh hosted
failure evidence and read-only lane audit are recorded in the research report,
including [run 39617761785](https://github.com/szTheory/muhtari/actions/runs/39617761785).
WASM has nine qualifying successful main-push runs and library five; neither is
eligible for promotion at that audit. The owner subsequently delegated the policy
revision recorded in CONTEXT.md; no lane promotion or implementation occurred.

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

## Decisions

- [Phase 5]: Replace hard gate/source ratio with audited absolute ratchet; fix scoped allocations before additions and tighten to final actual. Planning uses a bounded initial measurement task, not a full prototype. — Owner delegated adoption after refreshed failed construction and three advisory reviews; preserve safety controls and all TUNE outcomes.
