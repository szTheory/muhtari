# Roadmap: muhtari

## Milestones

- [ ] **v11.0 — Windows extended-path support** — phases 28–29; selected 2026-11-18. [Requirements](REQUIREMENTS.md) · [Scope review](research/next-milestone/2026-11-18-future-roadmap-review.md)
- [x] **v10.0 — Headless developer trace** — phases 26–27; shipped as v0.17.0 on 2026-11-17. [Audit](milestones/v10.0-MILESTONE-AUDIT.md) · [Archive](milestones/v10.0-ROADMAP.md) · [Requirements](milestones/v10.0-REQUIREMENTS.md)
- [x] **v9.0 — Publish the corrected Souper mapper package** — shipped as v0.16.0 on 2026-11-16. [Audit](milestones/v9.0-MILESTONE-AUDIT.md) · [Archive](milestones/v9.0-ROADMAP.md)
- [x] **v8.0 — GameDrive streamed-audio contract qualification** — closed locally 2026-11-15; implementation readiness NO-GO. [Audit](milestones/v8.0-MILESTONE-AUDIT.md) · [Archive](milestones/v8.0-ROADMAP.md)
- [x] **v7.0 — Souper homebrew mapper** — closed locally 2026-11-14. [Audit](milestones/v7.0-MILESTONE-AUDIT.md) · [Archive](milestones/v7.0-ROADMAP.md)
- [x] **v6.0 — YM2151 homebrew sound** — shipped as v0.14.0. [Archive](milestones/v6.0-ROADMAP.md)
- [x] **v5.0 — Downloadable WebAssembly core** — shipped as v0.13.1. [Archive](milestones/v5.0-ROADMAP.md)
- [x] **v4.0 — Browser/WebAssembly core readiness** — shipped through PR #47 and package v0.12.0. [Archive](milestones/v4.0-ROADMAP.md)
- [x] **v3.0 — Playstead-ready play and states** — closed with nine requirements deferred. [Audit](milestones/v3.0-MILESTONE-AUDIT.md) · [Archive](milestones/v3.0-ROADMAP.md)
- [x] **v2.0 — The whole library** — shipped. [Archive](milestones/v2.0-ROADMAP.md)
- [x] **v1.0 — 7800 games play with sound in RetroArch** — shipped. [Archive](milestones/v1.0-ROADMAP.md)

## Overview

v11.0 first establishes a source-qualified release/CI and maintenance baseline
and reconciles the known Phase 27 register counts. It then delivers explicit
Windows extended-path support throughout `muhtari-run`, with real Windows CI
evidence for path identity, successful I/O and safe failure. The selected scope
and rejected alternatives are in the [successor review](research/next-milestone/2026-11-18-future-roadmap-review.md).

## Phases

- [ ] **Phase 28: Measured milestone tune-up** - Qualify current delivery evidence and reconcile the Phase 27 register counts without changing verdicts.
- [ ] **Phase 29: Windows extended-path runner support** - Use explicit local and UNC extended paths across runner inputs and outputs with preserved collision and replacement safety.

## Phase Details

### Phase 28: Measured milestone tune-up

**Goal**: Maintainers can rely on current source-qualified release/CI evidence and an accurate deferred-items register before extending runner path behavior.
**Depends on**: Phase 27 (v10.0 shipped and archived); no uncompleted active prerequisite.
**Requirements**: TUNE-15, TUNE-16
**Success Criteria** (what must be TRUE):
  1. Maintainers can identify the exact current release and reviewed CI sources, distinguish historical failures from accepted results, and inspect queue time, critical path, job durations, cancelled work and cheap preflight results through commands, exit statuses and run links.
  2. Maintainers can review a proportionate quality/maintenance disposition covering performance, CI/CD, ease of use, architecture, documentation and upgrade path; a justified small correction or bounded no-change result preserves required fault coverage, existing limits and evidence distinctions.
  3. The register identified by Phase 27's deferred item reports 52 total entries: 16 open, 0 waived and 36 fixed. Every entry's text and verdict is unchanged; only the mismatched aggregate counts are reconciled.
**Plans**: 1 plan

Plans:
- [ ] 28-01-PLAN.md — Qualify current delivery evidence, preserve pipeline contracts and reconcile only the shared register counts.

This is the recurring [SEED-007 tune-up](seeds/SEED-007-milestone-tune-up.md),
including [SEED-009's proportional maintenance review](seeds/SEED-009-proportional-maintenance-review.md).
The known count discrepancy is recorded in the [v10.0 audit](milestones/v10.0-MILESTONE-AUDIT.md)
and [Phase 27 deferred item](milestones/v10.0-phases/27-headless-developer-trace/deferred-items.md).
Use current source-specific evidence and [reproduced lessons](LESSONS.md), with
cheap consistency checks before long suites. A single run pair does not justify
topology, cache or deadline changes; no new audit program or quota is required.

### Phase 29: Windows extended-path runner support

**Goal**: Windows users can use explicit absolute extended paths throughout `muhtari-run` while retaining literal names, collision protection and safe output replacement.
**Depends on**: Phase 28; its source-qualified baseline and count reconciliation precede runner changes.
**Requirements**: WINPATH-01, WINPATH-02, WINPATH-03, WINPATH-04
**Success Criteria** (what must be TRUE):
  1. Users can read cartridge, movie, BIOS, HSC firmware and state inputs through explicit drive-letter `\\?\` and extended UNC `\\?\UNC\` paths. The complete argument-to-file-operation chain preserves supported Unicode names, literal trailing dots/spaces and qualified long paths.
  2. Users can select the save directory and produce state, frame-image, trace and battery outputs through both selected namespaces, including replacement of existing supported destinations; output bytes and names match the requested literal destinations.
  3. Distinct extended-namespace destinations, including suffix-distinct names, succeed; destinations naming the same file or overlapping an input or another output are refused before modification. Existing-file identity, parent-junction aliases and ordinary/extended mixed-form controls retain ordinary Windows protections.
  4. A failed create, write or replacement produces a clear error and preserves the prior destination bytes for state, frame-image, trace and battery outputs; successful writes retain safe replacement behavior.
  5. Windows CI exercises the actual runner on the supported Windows architectures with local paths longer than `MAX_PATH`, Unicode/literal suffix names and extended UNC paths through an accessible test share, alongside distinct-output, true-alias, overlap, ordinary-path and failure-preservation controls. User documentation states the supported absolute forms, measured path-length boundary and clear failure behavior for unsupported operations.
**Plans**: TBD

The path contract is grounded in the [selected review](research/next-milestone/2026-11-18-future-roadmap-review.md)
and Microsoft's [namespace](https://learn.microsoft.com/en-us/windows/win32/fileio/naming-a-file)
and [path-length](https://learn.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation)
references recorded there. Qualify the whole argument, conversion, comparison,
open, temporary-file and replacement chain; changing suffix comparison alone
does not establish support. Test fixtures must be lawful and generated or owned
under existing policy. Keep acceptance in the existing Windows CI path;
non-Windows checks do not establish Windows support, and an unavailable UNC
share leaves UNC acceptance pending rather than silently skipped or waived.

## Scope and preserved stops

- Runner-only compatibility change: public API 8, ABI 1, serialized-state formats, emulated behavior and all three deliverables remain unchanged. No GUI/device code, dependency, new namespace family or unlimited-path claim is selected.
- Explicit absolute drive-letter and extended UNC forms are in scope. Relative extended paths, native NT devices, volume-GUID forms and lengths/names beyond verified filesystem behavior remain outside the promised contract.
- The nine deferred v3.0 requirements, denied Phase 12 diagnostic-app request, original Phase 12/14 host/remote stops, spent allowances, expired windows and no-retry limits remain binding. The spent Phase 27 PR 77 observation is not reopened. No owner hardware or real-host check is required by this milestone.
- GameDrive/BupChip implementation remains outside scope; the [GameDrive NO-GO](milestones/v8.0-phases/24-qualify-the-gamedrive-audio-contract/24-QUALIFICATION.md), failed v0.15.0 tag/draft and older dirty worktrees retain their existing dispositions.

## Progress

**Execution Order:** Phase 28 → Phase 29. Implementation, Windows CI acceptance,
documentation and review stay together in Phase 29; there is no separate
validation phase. Ship the final phase before milestone archival.

| Phase | Plans Complete | Status | Completed |
|-------|----------------|--------|-----------|
| 28. Measured milestone tune-up | 0/TBD | Not started | - |
| 29. Windows extended-path runner support | 0/TBD | Not started | - |

**Coverage:** All 6 v1 requirements map to exactly one phase; none are unmapped.
See [requirements traceability](REQUIREMENTS.md#traceability).

## Completed phase history

<details>
<summary>✅ v10.0 — Headless developer trace (phases 26–27) — SHIPPED 2026-11-17</summary>

- [x] Phase 26: Measured milestone tune-up (3/3 plans) — TUNE-14 complete.
- [x] Phase 27: Headless developer trace (5/5 active plans) — TRACE-01–07 complete. The superseded partial 27-05 plan and its checkpoint remain in the archive; 27-06 closed the cost-receipt gap.

Stable v0.17.0 is published. Main CI and Release passed on exact main source `333380b118f62b618cb6df29cdfdd4388efc7b22`: [CI 44214206424](https://github.com/szTheory/muhtari/actions/runs/44214206424), [Release 44214206354](https://github.com/szTheory/muhtari/actions/runs/44214206354). The closeout audit found no blockers and records two acknowledged deferrals. See the [audit](milestones/v10.0-MILESTONE-AUDIT.md) and [v10.0 archive](milestones/v10.0-ROADMAP.md).

</details>

<details>
<summary>✅ v9.0 — Publish the corrected Souper mapper package — SHIPPED 2026-11-16</summary>

- [x] Phase 25: Publish the corrected Souper mapper (1/1 plan) — all four requirements verified.

Stable v0.16.0 was published from source `a0319a9c`; its audit and archived phase records retain exact-source CI and Release evidence.

</details>

Earlier milestone records remain historical. v11.0 is selected; Phase 28 is
ready for planning. Direction beyond v11.0 remains in [PRODUCT-ROADMAP.md](PRODUCT-ROADMAP.md).
