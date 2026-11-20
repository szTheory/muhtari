---
gsd_state_version: "1.0"
milestone: v12.0
milestone_name: First-use readiness
current_phase: 31
current_plan: Not started
status: completed
stopped_at: v12.0 milestone audit complete — ship pending
last_updated: "2026-11-20T06:00:56Z"
last_activity: 2026-11-20
last_activity_desc: v12.0 milestone audit complete
state_head: 49adcef4582a70eecb1331cde8abf9d5dfc0a45c
progress:
  total_phases: 2
  completed_phases: 2
  total_plans: 3
  completed_plans: 3
  percent: 100
---

# Project State

## Project Reference
See [PROJECT.md](PROJECT.md), the active [requirements](REQUIREMENTS.md) and [roadmap](ROADMAP.md), the [rolling product roadmap](PRODUCT-ROADMAP.md), and the [v12.0 scope review](research/next-milestone/2026-11-19-first-use-readiness-review.md). The completed v11.0 requirements and delivery records remain in the [archive](milestones/v11.0-REQUIREMENTS.md).
**Core value:** 7800 games behave as on the console, identically across platforms, from a small MIT-licensed C library hosts can embed.
**Current focus:** v12.0 milestone audit complete; final ship pending

## Current Position

Phase: 31 (last phase)
Current Plan: Not started
Total Plans in Phase: 2
Status: Phase work and milestone audit complete; ship is pending
Last activity: 2026-11-20 — v12.0 milestone audit complete

## Performance Metrics
No v12.0 plan metrics exist yet. v11.0's 19h 8m Phase 29 figure is elapsed time across implementation, handoffs and hosted CI, not a test run; the longest individual final CI job was 44m04. The final main run had about 10 seconds of queue time and no cancellations. The [v11.0 audit](milestones/v11.0-MILESTONE-AUDIT.md#cicd-efficiency-review) records the measurements and why they support no safe workflow change.
**Per-Plan Metrics:**

| Plan | Duration | Tasks | Files |
|------|----------|-------|-------|
| Phase 30 P01 | 34m | 3 tasks | 9 files |
| Phase 31 P01 | 33min | 3 tasks | 9 files |
| Phase 31 P02 | 12min | 2 tasks | 3 files |

## Accumulated Context
### Decisions
- Owner selected support for explicit absolute Windows drive-letter `\\?\` and extended UNC `\\?\UNC\` paths throughout `muhtari-run`; Unicode, literal suffixes and long-path claims require the complete file-operation chain and real Windows CI evidence.
- Keep public API 8, ABI 1, existing state formats (legacy 1/6, YM 2/7, Souper 3/8 format/behavior), emulated behavior and all three deliverables unchanged. New namespaces, dependencies and host/UI work are outside this runner-only scope.
- Phase 28 maps TUNE-15/16: SEED-007/009's source-qualified measured tune-up plus count-only reconciliation to 52 total, 16 open, 0 waived, 36 fixed. Preserve every register entry and verdict. Phase 29 maps WINPATH-01–04; implementation, Windows CI acceptance, documentation and review stay together.
- Selected milestone work follows the owner's autonomous local follow-through preference. Normal Git publication, PR merge and required CI/release automation are authorized at natural delivery points; original host/remote stops remain intact.
- Run cheap source-specific consistency checks before long suites. Preserve required fault detection and measured-evidence rules; no unsupported timeout, cache, topology or quota changes. Ship the final phase before archiving its active roadmap rows.
- [Phase 28]: Preserve five hosted receipts as source-specific historical evidence.
- [Phase 28]: Make no CI topology, timeout, cache, resource, or membership change without comparable completed measurements.
- [Phase 28]: TUNE-15 and TUNE-16 passed phase verification; all register verdicts were preserved.
- [Phase 29]: Keep Windows path conversion and file operations on existing C and Win32 APIs; no dependency or public interface change.
- [Phase 29]: WINPATH-01–04 passed actual Windows x64 and ARM64 runner and SMB acceptance; the documented path limits remain evidence-bound.
- [Phase 29]: Apply one identity preflight across all runner input/output roles, using fail-closed filesystem evidence.
- [Phase 29]: Stream frame PNGs through the existing checked sibling-temp publisher to preserve prior output on failure.
- [Phase 29]: Accept WINPATH-01 through WINPATH-04 from c3 actual Windows x64 and ARM64 evidence; the synthetic CI merge tree matches reviewed source.
- [Phase 29]: The final main/Release runs confirmed Windows ARM64 as the CI critical path; no case-level profile supports a safe ordering change.
- [Phase 29]: Keep CI and Release coverage/topology intact; overlapping equality runner cost alone does not justify a changed-file classifier.
- [v12.0]: Keep player, runner/homebrew-developer and C host-integrator jobs tied to current deliverables; mark research date, evidence type, confidence and inference.
- [v12.0]: Improve first-use guidance for RetroArch, `muhtari-run` and the installed C package; focus cleanup on concrete journey friction and correctness debt.
- [v12.0]: The finite gameplay replay remains next only after exact fixture rights and an independent expected gameplay result are established.
- [Phase 30]: Keep Atari 2600 recognition and refusal; twentysixhundo owns Atari 2600 execution.
- [Phase 30]: Keep public release references version-free and link the exact historical package receipt.
- [Phase 30]: Use a fix squash title for the normal milestone delivery PR so release-please selects a patch.
- [Phase 31]: One ROM-free C17 source serves the archive CMake and pkg-config routes and staged package checks.
- [Phase 31]: Enable pkg-config archive verification only on the existing Ubuntu 26.04 x64 CI lane.
- [Phase 31]: Require the missing-export negative control to match its specific CMake diagnostic.
- [Phase 31]: Keep all fault injection in the staged test driver; leave the documented example and public API unchanged.
- [Phase 31]: Require setup and result fault checks before emitting the staged quickstart success marker.

### Pending Todos
The [Windows path follow-up](todos/completed/2026-11-17-define-extended-windows-output-path-support.md) was completed in Phase 29. Other todos/seeds retain their dispositions. Root remains canonical and older dirty worktrees require content/ancestry reconciliation before import. See [workspace reconciliation](debug/workspace-reconciliation-2026-11-10.md).
### Blockers/Concerns
- v10.0 is complete and shipped as stable v0.17.0. PR #77 and release PR #78 merged; main CI [44214206424](https://github.com/szTheory/muhtari/actions/runs/44214206424) and Release [44214206354](https://github.com/szTheory/muhtari/actions/runs/44214206354) passed on source `333380b118f62b618cb6df29cdfdd4388efc7b22`. The two newly acknowledged v10.0 items are recorded as deferred; 50 earlier acknowledgments retain their original verdicts. Nine deferred v3.0 requirements remain open: TUNE-06/07, PLAY-01–04 and FRONT-02–04. The denied Phase 12 diagnostic-app request must not be retried; original Phase 12/14 host/remote stops, spent allowances, expired windows and no-retry limits remain binding.
- GameDrive implementation remains NO-GO pending authoritative timing/framing, a lawful independent exact-profile PCM oracle and finite resource facts. Preserve the [v8.0 qualification](milestones/v8.0-phases/24-qualify-the-gamedrive-audio-contract/24-QUALIFICATION.md); no unlicensed sample/encoder source is permitted.
- Phase 21's spec-less probe remains unresolved and nonblocking. No retail, physical hardware or owner-device acceptance is inferred. LibFuturo owns browser/host routes; Playstead owns player integration; twentysixhundo owns Atari 2600.
- The original Phase 27 PR 77 one-push observation cap is spent; it authorizes no further update or retry. The historical source-authorization claim on `2160cfd` remains unverified. Subsequent PR 77 merge and v0.17.0 release evidence do not reopen that scope; see the [v10.0 records](milestones/v10.0-phases/27-headless-developer-trace/).
- v11.0 shipped as stable v0.18.0. PR #84 merged; final main CI [44627149615](https://github.com/szTheory/muhtari/actions/runs/44627149615) and Release [44627149696](https://github.com/szTheory/muhtari/actions/runs/44627149696) passed, with all 20 assets and the latest-install check. No owner-device or real-host run was required or claimed.

## Deferred Items
At v11.0 close, no new items were acknowledged; 51 items acknowledged at earlier closes remain suppressed with their original verdicts. At v10.0 close, two items were newly acknowledged; 50 earlier items remained suppressed. At v9.0 close, no new items were acknowledged. At v8.0 close, no new items were acknowledged; at v7.0 close, one debug-session item was newly acknowledged and 49 were carried forward. Its `unknown` verdict is preserved; suppression expires if the artifact changes. Earlier disclosures from v3.0 and prior closes remain below.

| Category | Item | Status | Deferred At | Milestone |
|----------|------|--------|-------------|-----------|
| todos | 2026-11-17-define-extended-windows-output-path-support.md (presence-only) | acknowledged | 2026-11-17 | v10.0 |
| deferred_items | 27/deferred-items.md: - The optional GSD WINDOWS deviation append was rejected because the existing register's frontmatter counts disagree with its entries: frontmatter reports 17 open, 0 waived, 35 fixed, 52 total; entries count 16 open, 0 waived, 36 fixed, 52 total. Reconcile the shared register before the next append. No repair was attempted because the count mismatch predates and is outside this plan. | acknowledged | 2026-11-17 | v10.0 |
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
Last session: 2026-11-20T06:00:56Z
Stopped at: v12.0 milestone audit complete — ship pending
Resume file: None

Next action: run $gsd-ship 31 from the canonical root checkout

## Operator Next Steps

- Run `$gsd-ship 31` from the canonical root checkout.
