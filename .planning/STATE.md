---
gsd_state_version: "1.0"
milestone: v4.0
milestone_name: Browser/WebAssembly core readiness
current_phase: 16
current_phase_name: Emscripten libretro core
status: planning
stopped_at: Phase 15 complete, ready to plan Phase 16
last_updated: "2026-11-10T18:14:10.130Z"
last_activity: 2026-11-10
last_activity_desc: Phase 15 complete, transitioned to Phase 16
state_head: a8027435db0e11b2f14ff64c0c5e1875aed09a97
progress:
  total_phases: 2
  completed_phases: 1
  total_plans: 1
  completed_plans: 1
  percent: 50
---

# Project State

## Project Reference

See [PROJECT.md](PROJECT.md), [REQUIREMENTS.md](REQUIREMENTS.md) and [ROADMAP.md](ROADMAP.md), updated 2026-11-10.
**Core value:** 7800 games behave as on the console, identically across platforms, from a small MIT-licensed C library hosts can embed.
**Current focus:** Phase 16 Emscripten libretro core with offline Node acceptance; Phase 15 verified.

## Current Position

Phase: 16 — Emscripten libretro core
Plan: Not started
Status: Ready to plan
Last activity: 2026-11-10 — Phase 15 complete, transitioned to Phase 16
Progress: [█████░░░░░] 50%

## Performance Metrics

Phase 15 completed one plan. The local workflow, fault controls and source review passed; see its [summary](phases/15-measured-milestone-tune-up/15-01-SUMMARY.md). Historical metrics remain in [MILESTONES.md](MILESTONES.md).

## Accumulated Context

- v3.0 closed with deferred scope: only Phase 13 verified; nine requirements remain unchecked in the [archive](milestones/v3.0-MILESTONE-AUDIT.md). v4.0 promotes no old host acceptance claim.
- Root remains canonical; stale dirty worktrees stay preserved and require content/ancestry reconciliation before import. See [workspace reconciliation](debug/workspace-reconciliation-2026-11-10.md).

Inherited todos and optional seeds retain their dispositions; no extra v4.0 requirement is inferred. [PRODUCT-ROADMAP.md](PRODUCT-ROADMAP.md) carries future horizons.

### Blockers/Concerns

- No blocker prevents Phase 16 local research. Phase 12's exact diagnostic-app request was denied before preflight/content and must not be retried; its host/remote stops and Phase 14's spent host scope remain binding. This milestone renews none of them.
- Phase 16 research must settle Emscripten output/loader/link form; existing WASI/Wasmtime evidence remains advisory and cannot establish browser compatibility. Browser UI, device behavior, Playstead and actual-browser performance stay outside scope.

## Deferred Items

At v3.0 close, 22 items were newly acknowledged and 28 were carried forward. Source verdicts remain unchanged and suppression expires when the underlying artifact changes. The earlier v1.0 disclosure remains in [MILESTONES.md](MILESTONES.md); the previously disclosed rows are preserved below.

| Category | Item | Status | Deferred At | Milestone |
|----------|------|--------|-------------|-----------|
| debug_sessions | phase12-capture-live-proposal | executed-terminal-failure | 2026-11-10 | v3.0 |
| debug_sessions | phase12-diagnostics-live-proposal | executed-terminal-failure | 2026-11-10 | v3.0 |
| debug_sessions | phase12-held-capture-diagnosis | local-repair-and-approved-cap-amendment-verified-host-proof-pending | 2026-11-10 | v3.0 |
| debug_sessions | phase12-hosted-input-tracer-proposal | unknown | 2026-11-10 | v3.0 |
| debug_sessions | phase12-input-gap-next-scope | unknown | 2026-11-10 | v3.0 |
| debug_sessions | phase12-input-tracer-proposal | unknown | 2026-11-10 | v3.0 |
| debug_sessions | phase12-intel-budget-review | local-correction-verified-live-closed | 2026-11-10 | v3.0 |
| debug_sessions | phase12-live-proposal | terminal-failed-allowance-spent | 2026-11-10 | v3.0 |
| debug_sessions | phase12-openbios-observability-scope | unknown | 2026-11-10 | v3.0 |
| debug_sessions | phase12-packaged-input-tracer | investigating | 2026-11-10 | v3.0 |
| debug_sessions | phase12-portability-repair-proposal | unknown | 2026-11-10 | v3.0 |
| debug_sessions | phase12-publication-diagnostic-proposal | unknown | 2026-11-10 | v3.0 |
| debug_sessions | phase12-wasi-local-proposal | complete-local | 2026-11-10 | v3.0 |
| debug_sessions | phase14-title-pin-self-test | investigating | 2026-11-10 | v3.0 |
| debug_sessions | workspace-reconciliation-2026-11-10 | unknown | 2026-11-10 | v3.0 |
| seeds | SEED-261030-tt4 | dormant | 2026-11-10 | v3.0 |
| seeds | SEED-261030-zk3 | dormant | 2026-11-10 | v3.0 |
| uat_gaps | 12/12-UAT.md | partial | 2026-11-10 | v3.0 |
| verification_gaps | 12/12-VERIFICATION.md | gaps_found | 2026-11-10 | v3.0 |
| verification_gaps | 14/14-VERIFICATION.md | gaps_found | 2026-11-10 | v3.0 |
| deferred_items | 13/deferred-items.md: Windows ledger mismatch, unavailable rcheevos source, test-first evidence, docs budget | acknowledged | 2026-11-10 | v3.0 |
| deferred_items | 14/deferred-items.md: Windows ledger mismatch, lint notes, prior host failures and pending frontend proof | acknowledged | 2026-11-10 | v3.0 |
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

## Session Continuity

Last session: 2026-11-10T16:22:35Z
Stopped at: Phase 15 complete, ready to plan Phase 16
Resume file: None
Next command: $gsd-autonomous --from 16
