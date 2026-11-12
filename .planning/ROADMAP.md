# Roadmap: muhtari

## Milestones

- [x] **v1.0 — 7800 games play with sound in RetroArch** — phases 1–4; shipped 2026-10-21. [Archive](milestones/v1.0-ROADMAP.md)
- [x] **v2.0 — The whole library** — phases 5–11; shipped 2026-10-29. [Archive](milestones/v2.0-ROADMAP.md)
- [x] **v3.0 — Playstead-ready play and states** — phases 12–14; closed 2026-11-10 with deferred scope (`override_closeout`). Phase 13 passed; Phase 12 remains accepted 0/4 and Phase 14 remains `gaps_found` at 20/27. Nine requirements remain unchecked. [Audit](milestones/v3.0-MILESTONE-AUDIT.md) · [Archive](milestones/v3.0-ROADMAP.md)
- [x] **v4.0 — Browser/WebAssembly core readiness** — phases 15–16; shipped through PR #47 and package v0.12.0. [Audit](milestones/v4.0-MILESTONE-AUDIT.md) · [Archive](milestones/v4.0-ROADMAP.md) · [Ship receipt](milestones/v4.0-SHIP.md). Earlier failed combined-source evidence and host stops remain preserved.

- [x] **v5.0 — Downloadable WebAssembly core** — phases 17–18; shipped as v0.13.1 on 2026-11-11; public-byte verification and current-main CI passed. [Audit](milestones/v5.0-MILESTONE-AUDIT.md) · [Archive](milestones/v5.0-ROADMAP.md) · [Requirements](milestones/v5.0-REQUIREMENTS.md)
- [ ] **v6.0 — YM2151 homebrew sound** — phases 19–20; selected 2026-11-11 for autonomous local follow-through.

## v6.0 — YM2151 homebrew sound

A declared, qualified XM/YM2151 cartridge profile produces deterministic FM sound
through the C library, headless runner and libretro core, including the portable
Wasm artifact. The owner selected this milestone and authorized autonomous local
follow-through on 2026-11-11. Scope is the nine current
[requirements](REQUIREMENTS.md), informed by the saved
[synthesis](research/next-milestone/SUMMARY.md),
[proposal](research/next-milestone/PROPOSAL.md) and
[operations boundaries](research/next-milestone/OPERATIONS.md).

Public mono PCM, C17 integer-only instance ownership, bounded calls and no
post-load allocation remain binding. The exact source-qualified profile and
concrete compatibility policy must be settled before implementation. A new
material compatibility consequence requires an owner decision; routine in-scope
recommendations and local verification proceed without another approval.

## Phases

- [x] **Phase 19: Measured milestone tune-up** — Maintainers start from current publication/docs and an evidenced, proportionate quality baseline. (completed 2026-11-11)
- [ ] **Phase 20: Complete YM2151 homebrew sound** — One qualified profile delivers FM sound, complete lifecycle/state and verified native/Wasm/package consumers.

## Phase Details

### Phase 19: Measured milestone tune-up

**Goal:** Maintainers can identify the current shipped baseline and begin YM work with a measured small correction or bounded no-change disposition that preserves required checks and historical failures.
**Depends on:** v5.0 shipped; no unfinished predecessor phase.
**Requirements:** TUNE-10
**Success Criteria** (what must be TRUE):
  1. Maintainers can follow current release/install/docs records to the verified publication and source-specific baseline without mistaking earlier failed sources for accepted evidence or restarting closed shipping recovery.
  2. Maintainers can reproduce the relevant quality/efficiency baseline and inspect either one measured small correction or a bounded no-change disposition; required pass/fault coverage, workload limits and prior failed receipts remain intact.
**Plans:** 1/1 plans complete
- [x] 19-01-PLAN.md
**UI hint:** no

### Phase 20: Complete YM2151 homebrew sound

**Goal:** A host can load one documented XM/YM2151 profile and obtain deterministic FM sound with complete lifecycle/state behavior through every existing deliverable and portable/package consumer.
**Depends on:** Phase 19
**Requirements:** YM-01, YM-02, YM-03, YM-04, YM-05, YM-06, YM-07, YM-08
**Success Criteria** (what must be TRUE):
  1. A host can load the qualified selected/autodetected XM/YM2151 profile and observe source-qualified map, enable, clock, busy/status, timer and established IRQ behavior; unsupported requests refuse before allocation or live-machine mutation.
  2. An owned homebrew workload produces independently checked non-silent, bounded mono PCM and canonical audio observations at supported rates/regions, including partial-frame carry and maximum legal mix. Exact-pin BSD-3 OPM synthesis parity is identified separately from hardware accuracy, and existing TIA/POKEY-only observations and duplicated-mono libretro output remain unchanged.
  3. A host can reset, unload/reload, isolate instances and save/restore every YM state component mid-stream with identical subsequent audio, no post-load allocation, finite work and bounded output writes; invalid or incompatible state preserves the live machine and committed battery bytes under the compatibility policy documented before implementation.
  4. The same owned diagnostic passes through the actual public C library, headless runner, loaded libretro core, portable Wasm artifact and clean extracted/installed package consumers. Independent numeric expectations precede cross-route hashes; source/package identity, notices and existing callback/batch/fallback/refusal/integrity fault controls remain effective.
  5. Integrators can identify the exact supported request/profile and evidence limits in existing docs; maintainers can inspect justified finite work, integer headroom and memory/state bounds plus representative ordinary/worst-legal native/Wasm cost measurements using current mechanisms. The C17 seed specialization retains its license, provenance pin and update duty without adding a runtime C++ dependency.
**Plans:** 6/7 plans executed
Plans:
**Wave 1**
- [x] 20-01-PLAN.md — Public YM tracer, fail-closed state guard and exact-seed OPM parity

**Wave 2** *(blocked on Wave 1 completion)*
- [x] 20-02-PLAN.md — YM bus timing, signed audio mix, observations and lifecycle bounds

**Wave 3** *(blocked on Wave 2 completion)*
- [x] 20-03-PLAN.md — Complete YM save-state continuation and version compatibility

**Wave 4** *(blocked on Wave 3 completion)*
- [x] 20-04-PLAN.md — Public C, runner and loaded native libretro diagnostic consumers

**Wave 5** *(blocked on Wave 4 completion)*
- [x] 20-05-PLAN.md — Emitted Wasm and clean installed/extracted package consumers
- [x] 20-06-PLAN.md — Genuine old/new state compatibility and legacy byte identity

**Wave 6** *(blocked on Wave 5 completion)*
- [ ] 20-07-PLAN.md — Measured costs, documentation, full local gates and review
**UI hint:** no

Chip, bus, audio, state, all deliverables, package acceptance, tests, documentation
and measured cost belong to Phase 20. No chip-only intermediate or separate
validation-tail phase completes this outcome. Phase planning reuses
[project lessons](LESSONS.md), the
[preparation index](preparation/README.md),
[settled decisions](preparation/DECISIONS.md) and
[engineering rules](preparation/ENGINEERING.md).

## Progress

**Execution order:** 19 → 20. All nine requirements are assigned exactly once;
Phase 19 is complete and Phase 20 is executing with Plans 20-01 through 20-03
complete. Plan 20-04 is next; Phase 20 remains unaccepted until all implementation
and verification gates pass.

| Phase | Plans Complete | Status | Completed |
|---|---|---|---|
| 19. Measured milestone tune-up | 1/1 | Complete    | 2026-11-11 |
| 20. Complete YM2151 homebrew sound | 6/7 | In Progress | - |

## Next boundary

The agent continues the owner-authorized autonomous local run into Phase 20
execution using the saved plans and integration evidence; no routine roadmap
acceptance is pending. After clearing context, resume with `$gsd-execute-phase 20`
and reconcile the live plan index before selecting the next runnable wave.

Publication, merge, new remote dispatch and host/device/identity-bound actions
remain separate boundaries. Phase 12's exact diagnostic-app request was denied
before preflight/content and must not be retried. All original Phase 12/14
spent attempts, expired windows and specific no-retry conditions remain binding;
the nine deferred v3.0 requirements are not accepted or renewed by v6.0. Keep
full XM, extra sound devices, public stereo, browser/player work and calibrated
hardware/gameplay claims outside this milestone. If source qualification,
licensing or bounded integer feasibility fails, resolve that material scope
decision instead of silently widening the feature or accepting a chip-only result.
