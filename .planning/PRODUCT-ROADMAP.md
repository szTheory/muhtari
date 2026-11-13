---
title: "Product roadmap (rolling)"
summary: "Immediate / Near / Mid / Long direction for muhtari with evidence and promotion triggers."
read_when: "At every new-milestone and complete-milestone step, and when a finding changes priorities."
updated: 2026-11-13
---

# Product roadmap (rolling)

`ROADMAP.md` is the execution contract. This file carries direction beyond it;
seeds hold the supporting narratives. Planning milestones are separate from
package versions, which continue to follow the existing release workflow.

At every milestone boundary, reconcile shipped and partial work, keep unresolved
debt visible, re-rank the horizons, record why the order changed, promote only
scoped work, and snapshot this file at close.

## Completed — ship the YM2151 milestone

v6.0 is implemented, verified, archived and shipped: one source-qualified XM/YM2151
profile, integer deterministic mono sound, complete state/lifecycle, actual native
and portable deliverables/packages, genuine old/current legacy compatibility and
measured bounded cost. [Audit](milestones/v6.0-MILESTONE-AUDIT.md) and
[requirements](milestones/v6.0-REQUIREMENTS.md) retain the accepted scope and limits.
The source-qualified map/clock/version policy is settled; no blanket approval or
owner UAT is pending for local implementation.

v6.0 shipped as [v0.14.0](https://github.com/szTheory/muhtari/releases/tag/v0.14.0).
The [release run](https://github.com/szTheory/muhtari/actions/runs/43441765548),
including latest installation, passed. PR [#61](https://github.com/szTheory/muhtari/pull/61)
merged and current-main CI passed. See the [ship receipt](milestones/v6.0-SHIP.md).

No retail compatibility, physical XM clock/gain/audibility, browser/player,
handheld or unavailable-platform acceptance is inferred. Muhtari retains exactly
three deliverables and no device/UI code. No new numerical quota, dependency,
cache, workflow topology or unrelated deadline was introduced.

## Next — select a bounded successor milestone

The next implementation milestone is unselected pending the cross-project
feedback recorded in [continue.md](continue.md). A lawful discriminating 7800
correctness counterexample remains a useful trigger. Seed synthesis parity is
not hardware calibration. Independently, LibFuturo owns its underway PAL
browser/profile work and the missing qualification/adoption outcomes. No new
upstream API/package requirement is established by that work.

The nine v3.0 gaps remain open: TUNE-06/07, PLAY-01–04 and FRONT-02–04. Keep the
published-core RetroArch/input/battery proof, paired Playstead flow and actual
RetroArch state/rewind/run-ahead results tied to their archived phase records.
The exact diagnostic app request was denied before preflight/content; no retry is
authorized by this planning decision. Resume those claims only under a separately
scoped, newly authorized host/identity-bound action or qualifying external evidence.

The sibling `libfuturo` project now implements the core/host interface and a
reference browser consumer using Muhtari. The earlier
[browser-player](seeds/SEED-261030-tt4-browser-libretro-player.md) and
[interface](seeds/SEED-261030-zk3-emulator-interface.md) ideas must be reconciled
with that implementation before proposing another host or interface. Neither
adds a deliverable or UI to Muhtari.

### Sibling responsibility boundaries — inspected 2026-11-11

Read-only inspection of LibFuturo's local checkout found an implemented
`libfuturo::web` Emscripten package, a direct Muhtari C adapter, and a JavaScript
browser consumer with rendering, keyboard input, audio, lifecycle and IndexedDB
battery persistence. Its browser-host v2.0 audit records passing local pinned
Chrome NTSC diagnostic qualification; current-source hosted qualification,
broader browser/game support and browser snapshots remain unclaimed. Its saved
v2.1 browser PAL work has now advanced into Phase 6 implementation in the
primary checkout. The successor inspection at local commit `a295baa` distinguishes
committed profile-set/contract work from dirty consumer and planning changes;
that is not completed PAL browser acceptance. Recent native PAL fixes bind input
hashes to each route log, anchor identity records and validate PAL size
independently of NTSC; Linux/hosted qualification remains open. Shared PROJECT.md
lags workstream closeouts and contains an older snapshot-capability statement.
Read [CONSUMERS](research/next-milestone/CONSUMERS.md) for exact source paths,
commits, evidence classes and recheck triggers. These are local source findings,
not a published LibFuturo release claim.

| Project | Responsibility |
|---|---|
| Muhtari | Atari 7800 machine behavior, deterministic timing/audio/input/state/battery semantics, C API, runner, libretro adapter, and core portability/release artifacts. |
| LibFuturo | Cross-core contract, Muhtari-to-`lf_*` adapter, native/browser SDK packaging, reference hosts and consumer qualification; browser devices, scheduling, lifecycle and persistence in its host. |
| Playstead | Player library, application UI, delivery and progress policy, and its own integration with a qualified host. |
| twentysixhundo | Atari 2600 emulation. |

Muhtari's published Emscripten ZIP exposes `retro_*` for libretro consumers.
LibFuturo's current browser path instead acquires pinned Muhtari source, links
`muhtari::muhtari` with its adapter, and builds a consumer of `lf_*`; it does not
consume that ZIP. These are separate interface routes over the same machine
implementation. Retain libretro support; no new shared ABI, package migration
or dependency is selected by this inspection. Put browser product work in
LibFuturo and promote Muhtari work only for a concrete core/API/portability gap.

Inspection sources in the sibling checkout: `docs/browser-host.md`,
`CMakeLists.txt` web build block, `cmake/MuhtariProbe.cmake`,
`src/muhtari_adapter.c`, `tests/web-consumer/`,
`.planning/workstreams/browser-host/milestones/v2.0-MILESTONE-AUDIT.md`, and
`.planning/workstreams/browser-host/MILESTONE-CONTEXT.md`. No sibling edits,
builds, host runs or CI were performed.

## Mid-term — measured accuracy and qualified progress

Hardware/FPGA/diagnostic comparisons, timing/POKEY corrections, complete
Souper/BupChip and the composite filter retain SEED-003. The qualified YM2151
profile shipped in v6.0; broader expansion hardware remains deferred.
Promote a specific measured gap sooner if it blocks a title people actually play.
Keep Playstead-managed state history and exact Continue behind a supported host
capture/load boundary and recovery policy.

## Long-term — tools, emulator family and more devices

Inspection/hooks, Python/AI environments, Lua on demand, full recording, BK2/TAS
and speedrun tooling follow demonstrated users (SEED-002/004). Handheld ports
retain SEED-005 and require measured frame-time/power budgets. Atari 2600 emulation
belongs to the sibling project `twentysixhundo`; SEED-006 is retired by owner
direction on 2026-11-11. Muhtari remains the current emulator-family
example; reproduce sibling lessons locally before adopting them.

## Decision log

| Date | Change | Why |
|---|---|---|
| 2026-10-15 | Set initial horizons; keep browser WebAssembly as SEED-008 | Owner ranked efficient browser play highly; three-deliverable architecture keeps UI in hosts. |
| 2026-10-21 | Promoted v2.0 whole-library scope with tune-up first | Owner approved 27 requirements and seven phases; library breadth preceded browser work. |
| 2026-10-29 | Promoted v3.0 Playstead-ready play and states | Owner prioritized player access and safe states over AI/TAS; actual paired Playstead proof remained separate. |
| 2026-11-10 | Closed v3.0 with nine requirements deferred and selected WebAssembly core portability for v4.0 | Phase 13 passed; Phase 12's exact app permission request was denied before any diagnostic and Phase 14 host proof remains absent. Continuing offline core work preserves the evidence stops without freezing independent development. |
| 2026-11-11 | Removed Atari 2600 emulation from Muhtari's future scope and retired SEED-006 | The owner started the separate `twentysixhundo` project for that system; Muhtari remains focused on Atari 7800. |
| 2026-11-11 | Recommended one YM2151/XM audio profile after bounded specialist fan-out and adversarial review | Loader refusal is concrete; no current accuracy defect or new LibFuturo API/package need is established. Preserve mono output, legal diagnostics, compatibility decisions and independent source-qualified acceptance. Scope remains a proposal. |

| 2026-11-10 | Completed the offline v4.0 core slice; retain ship and real-browser work as distinct next decisions | All five scoped requirements and integration flows passed locally; measured SDK results are explicitly headless and emulated. |

## Debt and triggers

- SEED-007 tune-up is the first phase of every milestone after v1.0; use measured
  inputs rather than repeating unsupported timeout/cache hypotheses.
- v3.0 host acceptance gaps and the Playstead paired flow remain deferred; this
  closeout renewed no host or remote allowance.
- COST-01 monthly totals, WR-01 delayed-release base selection, advisory property
  calibration, historical optional reviews and hardware/HSC limits retain their
  source dispositions in the archived v3.0 audit and earlier milestone records.
- SEED-003 accuracy, SEED-005 low-power ports and SEED-002/004 tooling keep their
  existing promotion triggers. SEED-006 is retired; Atari 2600 emulation belongs
  to `twentysixhundo`.

| 2026-11-10 | Promoted v5.0 downloadable Emscripten libretro packaging after successful v4.0 publication | Three specialist reviews and an adversarial synthesis identified the concrete CI-artifact-only gap; local package/release integrity preparation advances browser integrators without UI or renewed host authority. |

| 2026-11-11 | Closed v5.0 shipping and held successor selection for cross-project feedback | v0.13.1 public-byte verification and current-main CI passed. LibFuturo implements consumer/browser routes; its qualification and adoption gaps remain separate from Muhtari core scope. |

| 2026-11-12 | Completed and archived the qualified YM2151 milestone locally; shipping is next | All nine requirements, both phases and actual native/Wasm/package consumers pass; original host stops and unaccepted platform/hardware claims remain unchanged. |
