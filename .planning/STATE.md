---
gsd_state_version: "1.0"
milestone: v2.0
milestone_name: The whole library
current_phase: 05
current_phase_name: Pipeline tune-up
status: blocked
stopped_at: Halted05-06 Task1 at measured allocation gate; RED committed, draft uncommitted
last_updated: "2026-10-22T02:36:10.029Z"
last_activity: 2026-10-22
last_activity_desc: Plan05-06 halted at measured allocation gate
state_head: b0fc4d8166d43e7185ae7ca232d5a51f510e1291
progress:
  total_phases: 7
  completed_phases: 0
  total_plans: 9
  completed_plans: 5
  percent: 0
total_phases: 7
---

# Project State

## Project Reference

See PROJECT.md, REQUIREMENTS.md, ROADMAP.md and preparation/DECISIONS.md.
Core value: 7800 games behave as on the console, identically across platforms,
from a small MIT-licensed C library hosts can embed.

## Current Position

Phase: 05 (Pipeline tune-up) — BLOCKED
Plan: 6 of 9, halted during Task1; five plans complete
Status: Measured allocation halt; recovery plan required
Last activity: 2026-10-22 — Plan05-06 RED committed; draft over allocation

Progress: [░░░░░░░░░░] 0% of milestone phases — 5/9 Phase5 plans complete; 0/7 phases complete.
Implementation: 05-01,05-02,05-10,05-04,05-05 complete. Plan05-06 halted;05-07 blocked.
No downstream plan or additional GSD step ran in this session.

## Current acceptance

Plan05-06 has0/3 tasks complete. RED df6b095 is committed; the four-file implementation
draft is uncommitted. Seven perf cases and existing quality.controls pass, exit0.
Draft counter14888 exceeds C14853 by35; quality967 exceeds allocation920 by47.
No larger allocation, BE12 transfer, control/comment removal or cap change is authorized.
Full ci/library and hosted caller acceptance are unrun; no current run URL exists.
Both old allow stores/callers remain. WINDOWS16/17 track the halt and unrun acceptance.
Read phases/05-pipeline-tune-up/05-06-SUMMARY.md and05-06-HANDOFF.md for exact evidence.

Prior accepted05-05 implementation51a3f95 remains unchanged remotely:
[Direct CI39706507168](https://github.com/szTheory/muhtari/actions/runs/39706507168)
and [Release39706506980](https://github.com/szTheory/muhtari/actions/runs/39706506980)
pass, with local609+6 and accepted counter14841. Called attempt2 preserved unchanged
Windows current609+6/base605+6 after an OpenBIOS timeout. These are prior evidence,
not acceptance of the new draft. Details remain in05-05-SUMMARY.md; WINDOW15 is fixed.
Shared TUNE-03/TUNE-04 remain pending; no requirement was marked complete.

## Decisions

- Absolute audited ratchet replaces the former hard source ratio; ratios remain advisory.
- Frozen B12885 at f0202e1, retained-fault savings S2, fixed N1970 and C14853.
- Before Task3 additions, unused fixes70 moved to quality850 ->920. Final allocations:
  count368/BE205/digest447/quality920/fixes30/reserve0; no total or scope increase.
- Actual incremental scope1958 gives14841 total; quality920/920, fixes30/30,
  count368/368, digest447/447 and BE193/205. Remaining BE12 is not new reserve.
- Ordinary bounds use authenticated independent measurements. Compressed base archive
  variation caused strict tightening399952 ->399937, the observed comparable-base minimum.
- Exact artifact membership and identity precede extrema reduction; one typed judge
  owns actual bounds/reasons. Release permission unions add only actions:read.
- Unavailable coverage/open-crash/backlog/duration dimensions remain gaps.
  Library/WASM/property and unarmed test-card Ir remain advisory; no promotion.
- Preserve eight audio pairs, exact inventories and fixture prerequisites.
  Image-specific timing cohorts below ten samples stay provisional.
- Digest447 retains all pages/attempts, safe read-only previews and serialized
  default-branch issue publication. Actual issue publication is still a ship check.

## Performance Metrics

| Plan | Duration | Tasks | Files |
|------|----------|-------|-------|
| Phase05 P01 |12min|2|6|
| Phase05 P02 |17min|3|9|
| Phase05 P10 |83min wall including queues|2|8|
| Phase05 P04 |11min|2|7|
| Phase05 P05 prior halt |14min|1 partial|8|
| Phase05 P05 continuation |67min through closeout checks|3 total complete|14|
| Phase 05 P06 | 5min halted | 0 tasks | 4 files |

## Blockers and seeds

Plan05-06 is halted at its explicit allocation gate. Bounded new-evidence recovery
must fit quality920/C14853 or explicitly revise migration scope before05-07.
The descriptorless TUNE-03 boundary prohibition remains flagged/unverified;
phase-wide requirements, final allocation retirement and acceptance are unfinished.
Default-branch digest publication and merge-dependent truths remain ship checks.
Tooling seeds stay in the phase deferred-items ledger and do not create requirements.
Historical05-03 halt and05-10 original failures remain historical facts.

## Preserved earlier evidence

- Accepted Windows digest repair: CI39699892851 at tested1abc98a/merge011fb5b,
  base65b360d, Windows current609+6/base605+6. Final207s calibration4af6e9b
  preserves complete controls; observed arm64 cohorts103.28s/96.22s stay separate.
- PowerPC recovery: implementation0901c877; release-called39663281026 and
  direct Nightly39663280880 accepted full inventories and native/BE equality.
- Digest implementation7d524edc; read-only preview39685745578 succeeds with
  publish skipped. The live partial window retained seven runs/attempts,
  73 jobs and91.5833 runner minutes; unavailable dimensions were not invented.
- Full preparation/research history and exact failures remain in PROJECT.md,
  phase CONTEXT/RESEARCH/SUMMARY files, resolved debug sessions and Git history.
  The old857-line prerequisite is superseded; failed historical prototypes
  are not new measurements or current prerequisites.

## Milestone history

v1.0 planning shipped2026-10-21; packagev0.4.0 has19 published assets.
Publication/latest-install run39533130210 succeeded. Phases1–4 retain the
owner-accepted stale fingerprints and preserved audit reports; the audit is
tech_debt, with15 acknowledged historical deferrals. Their facts and seeds
remain in milestones/v1.0-MILESTONE-AUDIT.md, v1.0-phases and PROJECT.md.
v2.0 is The whole library:27 approved requirements, seven phases5–11.
Initialization PR18 merged752916f after CI39614799435. No new milestone or
hardware verification ran here; Souper/BupChip and other settled exclusions stand.

## Session Continuity

Last session: 2026-10-22T02:36:10.003Z
Stopped at: Halted05-06 Task1 at measured allocation gate; RED committed, draft uncommitted
Resume file: .planning/phases/05-pipeline-tune-up/05-06-HANDOFF.md
Next command: $gsd-plan-phase 5 --gaps
Stop after this execution step; recovery needs a bounded scope decision before05-06 resumes.

### Blockers

- Plan05-06 halted during Task1: draft14888 exceeds C14853 and quality967 exceeds allocation920; only RED is committed. Fixed cap/allocations unchanged; bounded new-evidence recovery required before05-07.
