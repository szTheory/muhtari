---
title: "Product roadmap (rolling)"
summary: "Immediate / Near / Mid / Long direction for muhtari with the evidence and promotion trigger for each horizon; ROADMAP.md is the execution contract, seeds hold the source narratives."
read_when: "At every new-milestone and complete-milestone step, and when a finding changes priorities."
updated: 2026-10-29
---

# Product roadmap (rolling)

`ROADMAP.md` (created at kickoff) is GSD's execution contract for the current milestone. This file carries the
direction beyond it; each horizon names the seed that holds its narrative and the trigger that promotes it.
Planning milestones (v1.0, v2.0, …) are not package versions: packages follow pre-1.0 semver and every phase that
adds a capability publishes a new minor release.

**Refresh rule.** At each milestone boundary the agent reconciles shipped and partial work, keeps unresolved debt
visible, re-ranks the horizons, records why the order changed or stayed, presents the preferred next scope with
alternatives, and moves only owner-approved scope into Immediate. The file is snapshotted as
`milestones/vX.Y-PRODUCT-ROADMAP.md` at close. Future horizons stay less detailed than the current one.

## Immediate — v3.0 Playstead-ready play and states

**Approved 2026-10-29.** Ordinary gaming through Playstead and safe save/load take
priority over movie authoring, AI and TAS. The active [requirements](REQUIREMENTS.md)
and [roadmap](ROADMAP.md) are the execution contract; no implementation is claimed.

Start with a bounded measured tune-up and gamer-facing delivery/readiness, then
portable validated states across library/runner/libretro, followed by frontend
save/load, rewind and run-ahead. Use existing replay where it supplies repeatable
verification; do not make full recording a launch prerequisite.

The quickest Playstead route is its adapter launching separately pinned RetroArch
and muhtari artifacts. Muhtari supplies core/release readiness; Playstead owns its
installer, cache, controller handoff, lifecycle and progress UI. Actual Playstead
launch/return/save acceptance remains a separate cross-project dependency, not a
result inferred from a runner or RetroArch-only test. See the [handoff](research/PLAYSTEAD-HANDOFF.md).

Shipped baseline remains package v0.10.0, API7/ABI1/behavior6: scoped boards, PAL,
controllers, sound and battery storage. Formal pinned-list evidence remains
201 unknown/3 skipped; two voluntary local gameplay reports are separate evidence.
Retain COST-01/WR-01, advisory property evidence and hardware/HSC limits until new
acceptance changes their dispositions. The [v2.0 archive](milestones/v2.0-ROADMAP.md)
and [research snapshot](milestones/v2.0-research/SUMMARY.md) preserve prior context.

## Next — easier access and tighter Playstead play

**Preferred next candidate:** efficient browser/WebAssembly play (SEED-008),
reweighed at v3.0 close. It lowers installation friction and reuses the core's
portability; its existing WASM equality evidence stays advisory. No date or fixed
future milestone number is promised.

**Competing priority:** Playstead's native one-app play/Continue experience. A
Playstead-owned in-process libretro host is the candidate north star: library to
play in one calm flow, responsive controls, sound/picture and safe state capture
without exposing emulator plumbing. Promote a focused Playstead host spike if
external-window/focus/controller friction recurs, state capture cannot serve the
chosen Continue promise, or a one-app flow becomes an explicit release requirement.
This does not add GUI/device code or another deliverable to muhtari.

## Mid-term — measured accuracy and qualified progress

Measured hardware/FPGA/diagnostic comparisons, timing/POKEY corrections, deferred
YM2151 and complete Souper/BupChip, and the composite filter retain SEED-003.
Full clean-room OpenHSC and AtariVox speech stay candidates. Promote a specific
measured gap sooner if it blocks an actually played title.

Playstead-managed state history/exact Continue follows a supported host capture/
load boundary and a policy preserving newer durable progress. Cross-device state
custody needs identity/compatibility/recovery evidence. A recent-play shelf or
fresh process alone does not establish restoration of the interrupted game point.

## Long-term — tools, emulator family and more devices

Inspection/hooks, Python/AI environments, Lua on demand, full recording, BK2/TAS
and speedrun tooling follow demonstrated users (SEED-002/004). Ports, handheld
frame-time/power budgets and broader platforms retain SEED-005. 2600 mode remains
owner-promoted scope under SEED-006; metadata accounting is not gameplay proof.

Muhtari is the current leading example for a family of console cores sharing
[product/engineering intent](INTENT.md). Exchange high-signal evidence and lessons
bidirectionally with NES, Game Boy, Neo Geo and later siblings; reproduce mechanisms
locally and keep console-specific facts distinct. No shared framework is presumed.

## Decision log

| Date | Change | Why |
|---|---|---|
| 2026-10-15 | Initial horizons set during preparation | Library share from the CC0 MAME list; sibling velocity evidence; owner decisions (2600 later, RetroArch frontend, no GUI in the core repository) |
| 2026-10-15 | The browser build leaves v6.0 as SEED-008, weighed at the v1.0 close; a wasm32 hash lane is a phase 1 discuss question; efficiency becomes a PROJECT constraint | Owner, at the kickoff: running efficiently in the browser through WebAssembly is very important, and battery life on handhelds matters |

| 2026-10-21 | v1.0 reconciled as shipped; v2.0 whole-library scope promoted with tune-up first, phases 5–11 | Owner approved 27 requirements and seven phases. Library breadth/saves/controllers/PAL precede a browser player. Souper/BupChip deferred with later accuracy; unsupported and untested entries stay explicit. |

| 2026-10-29 | Reconciled merged v2.0 breadth and accounting; keep closeout Immediate and surface tune-up/states/browser choices | Audit27/27 with known reporting/property/tooling debt; no universal gameplay claim. States retain their place because device/save/PAL ownership is established; browser stays an explicit alternative until the next scope decision. v0.10.0 publication was pending at audit time. |

| 2026-10-29 | Archived v2.0; confirmed v0.10.0 publication and retained horizon order | Release publication/latest-install and merged CI pass. Tune-up remains first in the next milestone; states/movies are preferred and browser remains the high-priority alternative. No future scope promoted. |
| 2026-10-29 | Approved v3.0 gamer-first Playstead readiness and states; browser remains preferred next candidate, native Playstead hosting is an explicit competing priority | Owner approved researched recommendations and prioritized actual Playstead play/save-load over AI/TAS. Read-only Playstead inspection found external-process adapters and narrow existing state support; quick route and north star now have separate ownership and triggers. Refresh this horizon at every milestone start and close. |

## Debt and fired triggers at v2.0 closeout

- SEED-007 fires for the next milestone's first phase; SEED-009/010 carry measured
  maintenance friction, publication-control cost and advisory property deadlines.
- Preserve COST-01's untrusted raw monthly totals and WR-01's delayed-release
  equal-base edge; neither was repaired by later capability phases.
- Phase 10's nine optional review findings and Phase 11's twelve optional info
  identities retain their existing dispositions. The current audit links them.
- SEED-002 state work is now scoped in v3.0; full recording remains later. SEED-008 is the preferred next candidate, reweighed at closeout.
  SEED-006's promotion condition is eligible to discuss, but complete7800
  gameplay coverage is not proven and2600 scope is not approved.
- SEED-011–014 retain their own triggers: richer device evidence, a later legally
  pinned-list acceptance criterion, a requested local-results view and ratchet
  changes or use beyond pull-request bases.
  Existing public diagnostics satisfy the current milestone without promoting
  these optional additions. Historical v1.0 debt stays in its audit/todos.
