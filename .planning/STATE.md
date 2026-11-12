---
gsd_state_version: "1.0"
milestone: v6.0
status: milestone_complete
stopped_at: v6.0 verified and archived locally; shipping next
last_updated: "2026-11-12T13:35:52.107Z"
last_activity: 2026-11-12
last_activity_desc: "Phase 20 shipped as PR #54"
state_head: 9056b682960c19e287e8e6abdd77f0547ffac6c0
progress:
  total_phases: 2
  completed_phases: 2
  total_plans: 8
  completed_plans: 8
  percent: 100
milestone_name: YM2151 homebrew sound
current_phase: 20
---

# Project State

## Project Reference

See [PROJECT.md](PROJECT.md), [archived requirements](milestones/v6.0-REQUIREMENTS.md), [ROADMAP.md](ROADMAP.md) and [milestone synthesis](research/next-milestone/SUMMARY.md).
**Core value:** 7800 games behave as on the console, identically across platforms, from a small MIT-licensed C library hosts can embed.
**Current focus:** v6.0 locally complete; separate shipping boundary next

## Current Position

Phase: Milestone v6.0 complete
Plan: —
Status: Locally complete; publication pending
Last activity: 2026-11-12 -- Phase 20 shipped as PR #54

## Performance Metrics

v6.0 has two verified phases, eight completed plans and nineteen actual plan task blocks. All nine current requirements, required local gates and independent review/integration pass. Current publication remains the v5.0/package result.
**Per-Plan Metrics:**

| Plan | Duration | Tasks | Files |
|------|----------|-------|-------|
| Phase 19 P01 | 76min | 1 task | 1 file |
| Phase 20 P01 | 80min | 2 tasks | 20 files |
| Phase 20 P02 | 84min | 3 tasks | 9 files |

## Accumulated Context

- v3.0 closed with deferred scope: only Phase 13 verified; nine requirements remain unchecked in the [archive](milestones/v3.0-MILESTONE-AUDIT.md). v4.0 promotes no old host acceptance claim.
- Root remains canonical; stale dirty worktrees stay preserved and require content/ancestry reconciliation before import. See [workspace reconciliation](debug/workspace-reconciliation-2026-11-10.md).
- Owner-selected v6.0 keeps one source-qualified XM/YM2151 profile, public mono PCM and C17 integer-only instance ownership. Profile, finite resource bounds and concrete API/behavior/native-format compatibility consequences are qualified before implementation; materially new consequences remain owner decisions.
- Required chip/bus/audio/state, library/runner/libretro/Wasm/package acceptance, tests, docs and cost stay together in Phase 20. All seven plans, required local gates, goal verification and milestone integration are complete. YM state now uses format 2/behavior 7 while no-YM snapshots retain format 1/behavior 6; independent genuine old/new continuation remains in Plan 20-06. Reuse saved research, current mechanisms and [fan-out guidance](research/fan-out/prompt.txt); do not add a validation-tail phase or repeat upstream research.

Inherited todos and optional seeds retain their dispositions except selected YM/XM and recurring tune-up contributions. Atari 2600 belongs to twentysixhundo; LibFuturo owns its existing host/browser routes. [PRODUCT-ROADMAP.md](PRODUCT-ROADMAP.md) carries future horizons.

### Blockers/Concerns

- No current local roadmap blocker. The exact profile, compatibility policy, final native/portable/package evidence and clean bounded review are complete; phase goal verification and local milestone audit/archival are complete. The optional GCC13 no-FP attempt is unqualified because it lacks a required warning flag; no unavailable platform pass is claimed.
- Phase 12's exact diagnostic-app request was denied before preflight/content and must not be retried. Original Phase 12 host/remote stops, Phase 14's spent host scope, expired windows and specific no-retry limits remain binding. No v6.0 action renews them or accepts the nine deferred v3.0 requirements.
- Publication, merge, new remote dispatch and host/device/identity-bound actions remain outside autonomous local authority. Browser/player, Playstead, physical audibility, calibrated hardware accuracy, public stereo and full XM are excluded. Earlier combined-source/failed release evidence remains source-specific in archived receipts.

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

Last session: 2026-11-12T08:32:40.865Z
Stopped at: Phase 20 complete — all phases complete
Resume file: None

Next action: After clearing context, run `$gsd-autonomous` for v6.0. It will reconcile the live plan index and continue Phase 20 planning and authorized local work. No user action is needed for routine continuation.

## Operator Next Steps


- Start the next milestone with $gsd-new-milestone


## Decisions

- [Phase 19]: Reused the fef5691 v0.13.1 record correction; current records need no second edit.
- [Phase 19]: Retain CI deadlines, caches, topology, quality floors and required/advisory membership; the few exact-source samples do not support a safe pipeline change.
- [Phase 19]: Preserve the hygiene tree timeout and same-source outside-sandbox passes; qualify execution context without claiming a specific root cause or changing the deadline.
- [Phase 20]: YM2151 production busy follows Yamaha's 68 phiM specification; the exact seed's 64-clock convention is separately reported as a four-clock parity difference.
- [Phase 20]: Timer IRQ flags remain latched when carry-enable bits are cleared; reset bits clear them, and mode-write/carry ordering is covered against the exact seed.
- [Phase 20]: Phase 20 Plan 20-02: The public API remains at 48 kHz; integrated YM PCM checks cover NTSC and PAL at that supported rate.
- [Phase 20]: Phase 20 Plan 20-02: Power-cycle clears clocked YM state and derived caches to a deterministic fresh-instance baseline.
- [Phase 20]: Phase 20 Plan 20-03: YM snapshots use format 2/behavior 7; no-YM snapshots retain format 1/behavior 6, and global latest reporting is API 8/ABI 1/behavior 7/format 2.
- [Phase 20]: Phase 20 Plan 20-03: YM clock history is validated from its existing last-tick, clock-count and remainder tuple; no extra runtime epoch field is needed.

## Next action

Context can be cleared. Run `$gsd-ship 20` from the current root checkout. The agent owns service-health, current receipt and source checks. Shipping authority remains separate; no push, merge, new remote run, tag or publication was performed here. All original host/remote stops remain binding.
