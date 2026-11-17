---
gsd_state_version: "1.0"
milestone: v10.0
milestone_name: Headless developer trace
current_phase: 27
status: completed
stopped_at: Phase 27 complete — all phases complete
last_updated: "2026-11-17T17:30:51.146Z"
last_activity: 2026-11-17
last_activity_desc: Phase 27 complete
state_head: 0d0cd0d66e8d8381fe5fe02f37faca43a97830d7
progress:
  total_phases: 2
  completed_phases: 2
  total_plans: 8
  completed_plans: 8
  percent: 100
---

# Project State

## Project Reference
See [PROJECT.md](PROJECT.md) (updated 2026-11-17), [ROADMAP.md](ROADMAP.md), [REQUIREMENTS.md](REQUIREMENTS.md), [product roadmap](PRODUCT-ROADMAP.md) and [successor review](research/next-milestone/2026-11-16-future-roadmap-review.md).
**Core value:** 7800 games behave as on the console, identically across platforms, from a small MIT-licensed C library hosts can embed.
**Current focus:** Phase 27 is verified complete; the remaining delivery blocker is failed required CI/Release compiler lanes.

## Current Position
Phase: 27 (Headless developer trace) — Complete
Plan: All 5 active plans complete
Status: Verification passed 15/15; TRACE-06 is complete from the source-qualified Linux perf receipt. Draft PR 77 remains open with failed CI and Release workflows, so shipping is blocked.
Last activity: 2026-11-17 — Phase 27 verification and closeout completed
Progress: [██████████] 100%

## Performance Metrics
Phase 26 completed three documentation-only plans; product implementation timing and trend are not yet measured.
| Phase | Plans | Total | Avg/Plan |
|-------|-------|-------|----------|
| 26 | 3 | - | - |
| 27 | 5 | - | - |
Prior milestone metrics and exact-source receipts remain in the [v9.0 archive](milestones/v9.0-ROADMAP.md), [audit](milestones/v9.0-MILESTONE-AUDIT.md) and earlier milestone records. Stable v0.16.0 is published; failed v0.15.0 remains unchanged and unpublished.
**Per-Plan Metrics:**

| Plan | Duration | Tasks | Files |
|------|----------|-------|-------|
| Phase 27 P01 | 70min | 2 tasks | 11 files |
| Phase 27-headless-developer-trace P02 | 83min | 3 tasks | 9 files |
| Phase 27 P03 | 48min | 2 tasks | 13 files |
| Phase 27 P04 | 107min | 2 tasks | 1 files |

## Accumulated Context
### Decisions
- Owner selected one bounded, opt-in `muhtari-run` instruction/bus trace, with frame/address filters, tick/beam/CPU context, finite output and generated/owned numeric acceptance.
- Keep public API 8, ABI 1 and existing state formats unchanged: legacy 1/6, YM 2/7 and Souper 3/8 (format/behavior). Trace callbacks/buffers remain outside machine state; observation must preserve frame/state hashes and the existing hooks-off cost gate. A necessary public observer API is an explicit compatibility decision boundary.
- Phase 26 maps TUNE-14; Phase 27 maps TRACE-01–07. Keep implementation, deterministic acceptance, docs and review together; add no validation-tail phase. Use current project/review facts over the historical API 7 research SUMMARY.
- Selected milestone work follows the owner's autonomous local follow-through preference. Normal Git publication, PR merge and required CI/release automation are authorized at natural delivery points; original host/remote stops remain intact.
- [Phase 27]: Keep trace callbacks private and per-instance, outside serialized state and public compatibility surfaces.
- [Phase 27]: Select traced execution at frame entry; reserve the measured hooks-off cost gate for plan 27-03.
- [Phase 27-headless-developer-trace]: Selected 65,536 default and 131,072 hard trace limits after source-grounded PAL overshoot analysis, actual record sizing and comparable NTSC/PAL captures.
- [Phase 27-headless-developer-trace]: Kept alias checks trace-specific and reused the existing sibling-temp atomic writer; public API, ABI and state formats remain unchanged.
- [Phase 27]: Keep TRACE-06 pending until the normal Linux perf job provides an authentic source-qualified Cachegrind receipt; local controls are not a substitute. This condition was satisfied by the accepted PR 77 receipt in run 44123425194.
- [Phase 27]: Preserve public API 8, ABI 1 and state formats 1/6, 2/7 and 3/8 across trace acceptance.
- [Phase 27]: Exact-source Windows x64 and arm64 path receipts established TRACE-03; the Linux receipt later established TRACE-06.
- [Phase 27]: Keep the Phase 27 acceptance matrix unchanged until all required exact-source receipts pass; the phase receipts now pass, while separate required CI/Release build failures remain.
- [Phase 27]: One initial CI run does not support a topology, timeout, or cache change.
- [Phase 27 gap closure]: Commit `2160cfd` contains the local trace-limit comparison correction; its earlier source-authorization claim remains unverified. The owner later authorized one push of reviewed descendant `4630288` to draft PR 77 and a bounded observation, which produced the accepted TRACE-06 receipt. That one-use remote scope is spent and authorizes no further push or run.

### Pending Todos
Inherited todos and optional seeds retain their dispositions; no unrelated work is promoted. Root remains canonical and older dirty worktrees require content/ancestry reconciliation before import. See [workspace reconciliation](debug/workspace-reconciliation-2026-11-10.md).
### Blockers/Concerns
- Phase 26 is complete. Phase 27 is verified 15/15, and TRACE-06 is complete from the accepted Linux perf receipt. Draft PR 77 remains open; its CI and Release workflows failed in separate required build lanes: GCC `-Werror=mismatched-dealloc` at `tests/runner/test_save.c:308` and Clang's missing `realpath` declaration at `runner/main.c:841`. Release publish/install steps were skipped. Plan 27-05 remains terminal after its one-use observation ended without a receipt; the later 27-06 authorization covered only one push and one bounded automatic-check observation, and is now spent. A new local diagnosis/remediation scope is needed before shipping; no further push, dispatch, rerun, merge or release is authorized. Nine deferred v3.0 requirements remain open: TUNE-06/07, PLAY-01–04 and FRONT-02–04. The denied Phase 12 diagnostic-app request must not be retried; original Phase 12/14 host/remote stops, spent allowances, expired windows and no-retry limits remain binding.
- GameDrive implementation remains NO-GO pending authoritative timing/framing, a lawful independent exact-profile PCM oracle and finite resource facts. Preserve the [v8.0 qualification](milestones/v8.0-phases/24-qualify-the-gamedrive-audio-contract/24-QUALIFICATION.md); no unlicensed sample/encoder source is permitted.
- Phase 21's spec-less probe remains unresolved and nonblocking. No retail, physical hardware or owner-device acceptance is inferred. LibFuturo owns browser/host routes; Playstead owns player integration; twentysixhundo owns Atari 2600.

## Deferred Items
At v9.0 close, no new items were acknowledged; 50 items acknowledged at earlier closes remain suppressed with their original verdicts. At v8.0 close, no new items were acknowledged; at v7.0 close, one debug-session item was newly acknowledged and 49 were carried forward. Its `unknown` verdict is preserved; suppression expires if the artifact changes. Earlier disclosures from v3.0 and prior closes remain below.

| Category | Item | Status | Deferred At | Milestone |
|----------|------|--------|-------------|-----------|
| debug_sessions | workspace-reconciliation-2026-11-13 | unknown | 2026-11-14 | v7.0 |
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
Last session: 2026-11-17T17:39:40.274Z
Stopped at: Phase 27 complete; v10.0 shipping held by failed required CI/Release compiler lanes
Resume file: None

Next action: Start a separately scoped local diagnosis with `$gsd-debug "GCC -Werror=mismatched-dealloc at tests/runner/test_save.c:308 and Clang undeclared realpath at runner/main.c:841; diagnose locally only, no push or hosted rerun."` No further remote action is authorized by the spent one-push observation; obtain fresh remote scope after local remediation before shipping.
