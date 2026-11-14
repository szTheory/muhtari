# Roadmap: muhtari

## Milestones

- [x] **v1.0 — 7800 games play with sound in RetroArch** — phases 1–4; shipped 2026-10-21. [Archive](milestones/v1.0-ROADMAP.md)
- [x] **v2.0 — The whole library** — phases 5–11; shipped 2026-10-29. [Archive](milestones/v2.0-ROADMAP.md)
- [x] **v3.0 — Playstead-ready play and states** — phases 12–14; closed 2026-11-10 with deferred scope (`override_closeout`). Phase 13 passed; Phase 12 remains accepted 0/4 and Phase 14 remains `gaps_found` at 20/27. Nine requirements remain unchecked. [Audit](milestones/v3.0-MILESTONE-AUDIT.md) · [Archive](milestones/v3.0-ROADMAP.md)
- [x] **v4.0 — Browser/WebAssembly core readiness** — phases 15–16; shipped through PR #47 and package v0.12.0. [Audit](milestones/v4.0-MILESTONE-AUDIT.md) · [Archive](milestones/v4.0-ROADMAP.md) · [Ship receipt](milestones/v4.0-SHIP.md). Earlier failed combined-source evidence and host stops remain preserved.

- [x] **v5.0 — Downloadable WebAssembly core** — phases 17–18; shipped as v0.13.1 on 2026-11-11; public-byte verification and current-main CI passed. [Audit](milestones/v5.0-MILESTONE-AUDIT.md) · [Archive](milestones/v5.0-ROADMAP.md) · [Requirements](milestones/v5.0-REQUIREMENTS.md)
- [x] **v6.0 — YM2151 homebrew sound** — phases 19–20; shipped as v0.14.0 on 2026-11-13. [Audit](milestones/v6.0-MILESTONE-AUDIT.md) · [Archive](milestones/v6.0-ROADMAP.md) · [Ship receipt](milestones/v6.0-SHIP.md)
- [ ] **v7.0 — Souper homebrew mapper** — phases 21–22; selected 2026-11-13. [Requirements](REQUIREMENTS.md) · [Recommendation and sources](research/next-milestone/v7-SOUPER.md)

## v7.0 — Souper homebrew mapper

Homebrew developers can run cartridges using one published Souper mapper-only
software profile through Muhtari's existing C library, runner and libretro core,
including the applicable portable consumer. The owner selected this recommendation
after source, consumer and adversarial review. All eight milestone requirements
are mapped below; the [rolling roadmap](PRODUCT-ROADMAP.md) retains later horizons.

The profile is bounded to 512 KiB ROM and 32 KiB EXRAM, distinct CPU/MARIA maps,
independent mapping controls, reset and snapshot continuation. The claim is
conformance with the cited software model. BupChip/BupBoop audio, Rikki & Vikki
compatibility, unqualified extra image payloads, flash variants and physical
cartridge accuracy remain outside this scope.

Before implementation, Phase 22 must record the concrete register-reset and
zero-filled EXRAM model policy, accepted image geometry and state-format/version
consequences under DEC.26 and DEC.36. Routine consequences within the settled
compatibility policy proceed without another approval; a materially new consumer
consequence remains an owner decision. No new API/ABI, dependency or host is
selected. Preserve C17 integer-only instance ownership and finite calls, and use
existing resource, provenance, test and review mechanisms.

## Phases

- [x] **Phase 21: Measured milestone tune-up** — Maintainers start Souper work from current release/docs and an evidenced, proportionate quality baseline. (completed 2026-11-13)
- [ ] **Phase 22: Complete Souper homebrew mapper** — One documented mapper profile delivers CPU/MARIA mapping, deterministic lifecycle/state and verified existing consumers.

## Phase Details

### Phase 21: Measured milestone tune-up

**Goal:** Maintainers can identify the current shipped baseline and begin Souper work with a measured small correction or bounded no-change disposition that preserves required checks and prior failures.
**Depends on:** v6.0 shipped; no unfinished predecessor phase.
**Requirements:** TUNE-11
**Success Criteria** (what must be TRUE):
  1. Maintainers can follow current release and documentation records to the accepted publication and source-specific baseline without mistaking an earlier failed source for accepted evidence or reopening closed shipping recovery.
  2. Maintainers can inspect a source-specific quality/efficiency review and either one evidence-backed low-risk correction or a bounded no-change disposition; required pass/fault coverage, workload limits and historical failures remain intact.

**Plans:** 1/1 plans complete
- [x] 21-01-PLAN.md — Trace the shipped baseline and record a source-qualified tune-up

### Phase 22: Complete Souper homebrew mapper

**Goal:** A homebrew developer can load the documented Souper mapper-only profile and obtain deterministic CPU/MARIA mapping and snapshot continuation through every existing deliverable and the applicable portable consumer.
**Depends on:** Phase 21
**Requirements:** SOUPER-01, SOUPER-02, SOUPER-03, SOUPER-04, SOUPER-05, SOUPER-06, SOUPER-07
**Success Criteria** (what must be TRUE):
  1. A host can load the exact supported A78 v3/v4 Souper declaration and geometry, identify its documented software-model limits, and receive refusal for malformed, oversized or unsupported variants before allocation or live-machine mutation.
  2. A generated patterned workload running real CPU accesses observes the documented ROM banks, register decode/aliases and flat/segmented EXRAM behavior at mapping boundaries and mode transitions, matching literal expected bytes authored independently from production mapping logic.
  3. The same diagnostic exercises real MARIA fetches and distinguishes them from CPU reads, including character remapping and the fetch-trap transition through the existing tick-aware bus/HALT path, against independently authored expected values.
  4. A host can reset, unload/reload, isolate instances and restore every Souper register and EXRAM byte with identical subsequent observations; invalid snapshots preserve the live machine under the concrete compatibility policy recorded before code.
  5. The owned diagnostic passes through the actual public C library, headless runner, loaded libretro core and applicable portable consumer, with malformed-profile and non-Souper controls retained; existing docs state the reset/EXRAM policy and unsupported image/audio forms, and no BupChip audio is claimed or emitted.

**Plans:** 2/3 plans executed
- [x] 22-01-PLAN.md
- [x] 22-02-PLAN.md
- [ ] 22-03-PLAN.md

Mapper behavior, loader refusals, lifecycle/state, independent diagnostics,
existing-consumer acceptance, documentation and bounded resource evidence belong
to Phase 22. Do not split out a validation-only tail or treat cross-route hash
agreement as an independent mapping oracle. Reuse the saved
[recommendation](research/next-milestone/v7-SOUPER.md), [lessons](LESSONS.md),
[preparation index](preparation/README.md), [decisions](preparation/DECISIONS.md)
and [engineering rules](preparation/ENGINEERING.md) before new research.

## Progress

Phase 21 is complete, and Phase 22 plan 22-01 is complete. TUNE-11 and four
Souper requirements are complete; three Souper requirements remain pending.
All eight requirements map to exactly one phase; no requirement is orphaned or
accepted ahead of verification.

| Phase | Plans Complete | Status | Completed |
|---|---|---|---|
| 21. Measured milestone tune-up | 1/1 | Complete    | 2026-11-13 |
| 22. Complete Souper homebrew mapper | 2/3 | In Progress | — |

## Next boundary

Continue Phase 22 with `$gsd-execute-phase 22`; plans 22-02 and 22-03 remain
before phase acceptance. Publication, merge, remote dispatch and
host/device/identity-bound actions remain separate boundaries.
Original Phase 12/14 denied/spent scopes, all nine deferred v3.0 requirements and
unaccepted hardware/platform claims remain unchanged.
