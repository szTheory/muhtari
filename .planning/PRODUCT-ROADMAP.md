---
title: "Product roadmap (rolling)"
summary: "Immediate / Near / Mid / Long direction for muhtari with evidence and promotion triggers."
read_when: "At every new-milestone and complete-milestone step, and when a finding changes priorities."
updated: 2026-11-10
---

# Product roadmap (rolling)

`ROADMAP.md` is the execution contract. This file carries direction beyond it;
seeds hold the supporting narratives. Planning milestones are separate from
package versions, which continue to follow the existing release workflow.

At every milestone boundary, reconcile shipped and partial work, keep unresolved
debt visible, re-rank the horizons, record why the order changed, promote only
scoped work, and snapshot this file at close.

## Immediate — v4.0 Browser/WebAssembly core readiness

Selected 2026-11-10 after the v3.0 override closeout. The old milestone's state
API is verified; actual host acceptance for Phase 12 and Phase 14 remains deferred
with the original stops intact. The owner delegated scope selection and asked to
keep core development moving despite the denied identity-bound request.

Phase 15 starts with recurring [SEED-007](seeds/SEED-007-milestone-tune-up.md):
measure CI and pipeline friction, review the weakest quality dimension, and refresh
docs and pins using evidence. Phase 16 targets a browser-consumable WebAssembly
build of the existing libretro core with an offline deterministic host shim that
compares representative frame/audio/state behavior against native results.
[SEED-008](seeds/SEED-008-browser.md) motivates the direction; its older canvas,
input, audio and browser-rate sketch is not an acceptance contract.

Muhtari remains a library, headless runner and libretro core. This scope includes
no browser page/player, GUI, window, audio-device or input-device code. The test
harness can prove compilation, imports/callback use and deterministic output; it
does not prove real-browser UX, device performance or Playstead integration.
Those need a later explicit scope grounded in evidence. No host permission or
remote attempt is needed for the offline core slice.

## Next — host acceptance when evidence and authority are available

The nine v3.0 gaps remain open: TUNE-06/07, PLAY-01–04 and FRONT-02–04. Keep the
published-core RetroArch/input/battery proof, paired Playstead flow and actual
RetroArch state/rewind/run-ahead results tied to their archived phase records.
The exact diagnostic app request was denied before preflight/content; no retry is
authorized by this planning decision. Resume those claims only under a separately
scoped, newly authorized host/identity-bound action or qualifying external evidence.

The separate browser player for Playstead remains a different project idea
([seed](seeds/SEED-261030-tt4-browser-libretro-player.md)); the optional interface
idea remains dormant ([seed](seeds/SEED-261030-zk3-emulator-interface.md)). Neither
adds a deliverable or UI to this milestone.

## Mid-term — measured accuracy and qualified progress

Hardware/FPGA/diagnostic comparisons, timing/POKEY corrections, deferred
YM2151 and complete Souper/BupChip, and the composite filter retain SEED-003.
Promote a specific measured gap sooner if it blocks a title people actually play.
Keep Playstead-managed state history and exact Continue behind a supported host
capture/load boundary and recovery policy.

## Long-term — tools, emulator family and more devices

Inspection/hooks, Python/AI environments, Lua on demand, full recording, BK2/TAS
and speedrun tooling follow demonstrated users (SEED-002/004). Handheld ports
retain SEED-005 and require measured frame-time/power budgets. Atari 2600 mode
remains owner-promoted under SEED-006. Muhtari remains the current emulator-family
example; reproduce sibling lessons locally before adopting them.

## Decision log

| Date | Change | Why |
|---|---|---|
| 2026-10-15 | Set initial horizons; keep browser WebAssembly as SEED-008 | Owner ranked efficient browser play highly; three-deliverable architecture keeps UI in hosts. |
| 2026-10-21 | Promoted v2.0 whole-library scope with tune-up first | Owner approved 27 requirements and seven phases; library breadth preceded browser work. |
| 2026-10-29 | Promoted v3.0 Playstead-ready play and states | Owner prioritized player access and safe states over AI/TAS; actual paired Playstead proof remained separate. |
| 2026-11-10 | Closed v3.0 with nine requirements deferred and selected WebAssembly core portability for v4.0 | Phase 13 passed; Phase 12's exact app permission request was denied before any diagnostic and Phase 14 host proof remains absent. Continuing offline core work preserves the evidence stops without freezing independent development. |

## Debt and triggers

- SEED-007 tune-up is the first phase of every milestone after v1.0; use measured
  inputs rather than repeating unsupported timeout/cache hypotheses.
- v3.0 host acceptance gaps and the Playstead paired flow remain deferred; this
  closeout renewed no host or remote allowance.
- COST-01 monthly totals, WR-01 delayed-release base selection, advisory property
  calibration, historical optional reviews and hardware/HSC limits retain their
  source dispositions in the archived v3.0 audit and earlier milestone records.
- SEED-003 accuracy, SEED-005 low-power ports, SEED-002/004 tooling, and SEED-006
  Atari 2600 keep their existing promotion triggers.
