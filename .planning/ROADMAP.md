# Roadmap: muhtari

## Milestones

- [ ] **v12.0 — First-use readiness** — phases 30–31; scoped 2026-11-19. [Requirements](REQUIREMENTS.md) · [Product roadmap](PRODUCT-ROADMAP.md) · [Scope review](research/next-milestone/2026-11-19-first-use-readiness-review.md)
- [x] **v11.0 — Windows extended-path support** — phases 28–29; shipped as v0.18.0 on 2026-11-19. [Audit](milestones/v11.0-MILESTONE-AUDIT.md) · [Archive](milestones/v11.0-ROADMAP.md) · [Requirements](milestones/v11.0-REQUIREMENTS.md) · [Product-roadmap snapshot](milestones/v11.0-PRODUCT-ROADMAP.md)
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

## Current position

v12.0 is scoped to make the current library, runner and libretro core easier to
approach. Phase 30 refreshes evidence-backed user jobs and improves first-use
paths for RetroArch and `muhtari-run`; Phase 31 makes the installed C package
example complete and reproducible. The user-facing scope and evidence gates are
in [REQUIREMENTS.md](REQUIREMENTS.md) and the [successor review](research/next-milestone/2026-11-19-first-use-readiness-review.md).

The [rolling product roadmap](PRODUCT-ROADMAP.md) keeps the conditional gameplay
replay next, pending exact fixture rights and an independent in-game result.
v11.0 shipped as stable v0.18.0; its exact-source receipts and archive remain in
the [v11.0 audit](milestones/v11.0-MILESTONE-AUDIT.md).

## v12.0 — First-use readiness

**Goal:** A player, runner user and C host integrator can reach a first
useful result from a current Muhtari release through clear, accurate docs and
maintained user-job context.

| Phase | Name | Goal | Requirements | Status |
|-------|------|------|--------------|--------|
| 30 | User jobs and player/runner first use | Current user jobs and concise player/runner paths match the released product; stale first-use docs are corrected. | PERSONA-01, PLAYER-01, RUNNER-01, DOCS-01 | Complete    |
| 31 | Installed C library quickstart | A fresh installed-package consumer builds and runs the documented minimal C example. | EMBED-01 | In Progress |

### Phase 30: User jobs and player/runner first use

**Goal**: Players and runner users can reach a first useful result from the
current release using accurate, concise instructions, while maintainers can
trace these paths to current evidence-backed user jobs.

**Requirements**: PERSONA-01, PLAYER-01, RUNNER-01, DOCS-01

**Success Criteria** (what must be TRUE):
  1. The existing persona/JTBD material describes current player, runner/homebrew-developer and C-integrator jobs, maps each to an actual deliverable, and marks evidence date, type, confidence and sibling ownership.
  2. The README explains how to choose/install a supported release core and load a lawfully obtained game in RetroArch, including prerequisites, expected first screen, optional BIOS behavior, useful controls and honest compatibility limits.
  3. The README or a directly linked beginner guide gives `muhtari-run` users a first test-card command and a first own-cartridge path with expected output, refusal guidance and links to the full reference.
  4. The status page and user-facing local links touched by the phase match the current release/archive; no stale Phase 25 path remains in those entry documents.

### Phase 31: Installed C library quickstart

**Goal**: A C host integrator can build and run a complete minimal example
against an installed Muhtari package with the required resources and a known
result.

**Requirements**: EMBED-01

**Success Criteria** (what must be TRUE):
  1. One complete, small C example creates a Muhtari instance, uses the required allocator and frame buffers, runs a frame and checks a known result without copyrighted media.
  2. A clean consumer can compile and run the example against the installed release archive using the documented CMake package route and pkg-config route.
  3. The existing installed-consumer verification exercises the documented example or the same source and catches a broken package target or public header through a representative negative control.
  4. The getting-started text links to the complete API reference and states platform/package requirements and the behavior established by the example without implying gameplay compatibility.

**Plans:** 1/2 plans executed

Plans:
**Wave 1**
- [x] 31-01-PLAN.md — Document and verify the shared installed C quickstart through both package routes

**Wave 2** *(blocked on Wave 1 completion)*
- [ ] 31-02-PLAN.md — Verify that injected quickstart failures report the error and release acquired resources

## Completed phase history

<details>
<summary>✅ v11.0 — Windows extended-path support (phases 28–29) — SHIPPED 2026-11-19</summary>

- [x] Phase 28: Measured milestone tune-up (2/2 plans) — TUNE-15 and TUNE-16 verified.
- [x] Phase 29: Windows extended-path runner support (3/3 plans) — WINPATH-01–04 verified.

Stable v0.18.0 shipped after exact-source CI and Release passed; all 20 assets
and the latest-install check passed. The audit records six requirements
satisfied, four complete flows, and no v11.0 blockers. See the [audit](milestones/v11.0-MILESTONE-AUDIT.md),
[requirements](milestones/v11.0-REQUIREMENTS.md), [phase records](milestones/v11.0-phases/)
and [archived roadmap](milestones/v11.0-ROADMAP.md).

</details>

Earlier milestones remain historical in [MILESTONES.md](MILESTONES.md) and
their linked archives. The active acceptance document is created with the next
selected milestone; completed v11.0 requirements are preserved in its archive.
