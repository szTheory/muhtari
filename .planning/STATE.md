---
gsd_state_version: "1.0"
milestone: v3.0
milestone_name: Playstead-ready play and states
status: planning
last_updated: "2026-10-30T01:44:59.893Z"
last_activity: 2026-10-29
progress:
  total_phases: 0
  completed_phases: 0
  total_plans: 0
  completed_plans: 0
  percent: 0
---

# Project State

## Project Reference

See [PROJECT.md](PROJECT.md), [ROADMAP.md](ROADMAP.md) and
[archived requirements](milestones/v2.0-REQUIREMENTS.md), updated 2026-10-29.
Core value: 7800 games behave as on the console, identically across platforms,
from a small MIT-licensed C library hosts can embed.
Current focus: select the next milestone and define fresh requirements.

## Current Position

Phase: Not started (defining requirements)
Plan: —
Status: Defining requirements
Last activity: 2026-10-29 — Milestone v3.0 started

## Decisions and Limits

- Keep COST-01 raw monthly totals untrusted and WR-01 delayed-release base edge deferred.
- Retain advisory property isolation calibration, optional Phase 10/11 findings and inherited todos.
- Keep fitted hardware confidence, PAL analog assumptions and HSC stub/unsupported sound limits explicit.
- Pinned library accounting is 201 unknown/3 skipped out of 204; model conformance/title screens are not gameplay.
- Read existing decisions and reproduced LESSONS before changing a subsystem; never reopen settled scope without evidence.
- SEED-007/009/010 fire for the next milestone's first tune-up. Discuss SEED-002 states/movies and SEED-008 browser next.
- SEED-006 is unpromoted; SEED-011–014 retain optional triggers. No future requirement is silently added.
- Tag creation is disabled; release-please owns package versions. Completed quick tasks remain in place.

## Deferred Items

Items acknowledged and deferred at milestone close. These markers preserve source
verdicts and expire when the underlying artifact changes. The 13 carried items
are four todos, eight seeds and one historical deferred item; v1.0's original
receipt remains in [MILESTONES.md](MILESTONES.md) and its audit.
Two newly acknowledged scanner entries already describe resolved historical faults;
acknowledgment suppresses the stale bullet without reopening or erasing that resolution.

| Category | Item | Status | Deferred At | Milestone |
|----------|------|--------|-------------|-----------|
| todos | 2026-10-29-keep-the-phase-11-first-review-info-findings.md | presence-only | 2026-10-29 | v2.0 |
| seeds | SEED-009 | dormant | 2026-10-29 | v2.0 |
| seeds | SEED-010 | dormant | 2026-10-29 | v2.0 |
| seeds | SEED-011 | dormant | 2026-10-29 | v2.0 |
| seeds | SEED-012 | dormant | 2026-10-29 | v2.0 |
| seeds | SEED-013 | dormant | 2026-10-29 | v2.0 |
| seeds | SEED-014 | dormant | 2026-10-29 | v2.0 |
| deferred_items | 05/deferred-items.md: Tooling seed: release-called Windows arm64 run39706506980/job119588102339 times out unchanged `built.openbios` at1.24s against1s, causing12 fixture-dependent tests not to run; the native workflow exits8 and produces no quality evidence. The native digest control is not the failure. Retain the timeout and missing-evidence rejection; retry the same commit and record both attempts rather than claiming a source repair. status: open | acknowledged; historical disposition retained | 2026-10-29 | v2.0 |
| deferred_items | 05/deferred-items.md: Advisory WASM seed: release-called run39832530253 attempt2/job120020998725 loses a pinned-tool fetch with `curl` exit35, connection reset, before tests; its inventory step and rehearsal/install/cleanup skip. Called `CI required` and quality pass, authentic eight-artifact join passes, and direct current WASM passes. Overall Release remains failure. Preserve the advisory status and failed evidence; no further rerun or source edit is part of recovery acceptance. status: open | acknowledged; historical disposition retained | 2026-10-29 | v2.0 |
| deferred_items | 05/deferred-items.md: Tooling seed: installed actionlint rejects the inherited `concurrency.queue: max` in release.yml, while GitHub accepts and executes that workflow. Focused schema validation suppresses only that unsupported syntax and the already known hosted runner labels. No workflow concurrency or safety control changes to satisfy the older validator. status: open | acknowledged; historical disposition retained | 2026-10-29 | v2.0 |
| deferred_items | 05/deferred-items.md: Tooling seed reproduced during quality integration: both Windows native workflows fail the existing `policy.digest.controls` at `tests/policy/digest.cmake:52` with `digest valid preview failed: no such file or directory`, exit8. The new quality controls pass; required Windows observation/base artifacts cannot be produced. Evidence: https://github.com/szTheory/muhtari/actions/runs/39694430448/job/119548291959 and job119548291973. The digest source is unchanged by this plan. Preserve its control and route a scoped tooling repair; no product requirement is added. status: fixed by scoped debug; CI39699892851 Windows current609+6/base605+6 accepted, tests retained | acknowledged; historical disposition retained | 2026-10-29 | v2.0 |
| deferred_items | 05/deferred-items.md: Intermittent tooling observation: run39697609787 x64 job119559093658 built trusted base65b360d successfully but its DLL-CRT consumer emitted no output. The identical unchanged base subsequently passes full605+6 on x64 and arm64 in runs39698420789/39699026057/39699892851. Reobserved in run39839399512 attempt1/job120036186384: current609+6 and inventories pass, then unchanged-base605 install.consumer emits no output and fails its required output regex after2.19s. One unchanged-head Windows job rerun succeeds in attempt2/job120039214618. No cause or source fix is claimed; failed-base rejection, timeouts and workloads remain. This is a seed, not a new requirement. status: open | acknowledged; historical disposition retained | 2026-10-29 | v2.0 |
| deferred_items | 05/deferred-items.md: Tooling seed: full actionlint reports SC2017 and SC2059 in the unchanged RetroArch pixel negative-control shell. Schema/action validation with shellcheck disabled passes. No pipeline control is weakened to silence these inherited advisory diagnostics. status: open | acknowledged; historical disposition retained | 2026-10-29 | v2.0 |
| deferred_items | 05/deferred-items.md: Tooling seed: `tests/cmake/budgets.cmake` accepts `-DROOT=.` but its relative document glob observes only the README/header (two files). The registered `docs.budgets` test supplies the absolute source root and correctly rejects oversized documents; use an absolute root for direct checks. Reproduced with `cmake -DROOT=. -P tests/cmake/budgets.cmake` versus `cmake -DROOT="$PWD" -P tests/cmake/budgets.cmake`, exits 0/1 while the candidate testing page exceeded its unchanged budget. Out of scope for this plan; no production check was weakened or removed. status: open | acknowledged; historical disposition retained | 2026-10-29 | v2.0 |
| deferred_items | 11/deferred-items.md: Found in plan 04  - `diag.bankset_tracer` and `diag.bankset` both write `build/ci/tests/diag/bankset/32n/` and carry no resource lock, so under `ctest -j` one can read the other's `pixels.png` half written (`bankset --bankset-png ... png: chunk longer than the file`). Seen once in `cmake --workflow --preset ci`; passes alone and on rerun. Not caused by plan 04; a fix is a `RESOURCE_LOCK` or a separate output directory in `tests/diag/tests.cmake`.  Resolved in the gap round: `diag.bankset` now writes `build/<preset>/tests/diag/bankset-matrix/` and `diag.bankset_tracer` keeps `build/<preset>/tests/diag/bankset/`, so the two tests share no output file. | acknowledged; historical disposition retained | 2026-10-29 | v2.0 |
