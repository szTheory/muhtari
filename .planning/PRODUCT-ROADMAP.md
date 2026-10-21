---
title: "Product roadmap (rolling)"
summary: "Immediate / Near / Mid / Long direction for muhtari with the evidence and promotion trigger for each horizon; ROADMAP.md is the execution contract, seeds hold the source narratives."
read_when: "At every new-milestone and complete-milestone step, and when a finding changes priorities."
updated: 2026-10-21
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

## Immediate — v2.0 "The whole library" (committed)

v1.0 shipped games with TIA/POKEY sound in package v0.4.0. The owner approved 27 v2.0 requirements and
seven phases (5–11): pipeline tune-up; Activision/Absolute/Fractalus; HSC and SaveKey persistence;
light gun and the other controllers; PAL; BankSet and XBoard/XM memory/POKEY/HSC; full-library acceptance.
The tune-up addresses measured CI reliability/cost, a weekly triage digest with monthly efficiency reporting,
a quality ratchet, the existing gate-code budget and documentation freshness.

[Requirements](REQUIREMENTS.md) and [execution roadmap](ROADMAP.md) are the active contract. SEED-001 and
SEED-007 supplied the scope and remain historical seeds. Whole-library accounting does not mean untested
entries pass. Souper/BupChip stays with the later accuracy milestone; XM excludes YM2151, AtariVox excludes
speech, and a signature-only HSC stub is not complete firmware.

**Browser disposition:** considered at this boundary and deferred by owner choice on 2026-10-21. Keep the
existing WASM portability evidence; a browser player is not part of v2.0. SEED-008 retains its priority for
the next boundary or an explicit earlier request.

## Near — v3.0 "States and movies" (SEED-002)

Validated fixed-size save states, rewind/run-ahead support, deterministic movie recording and Playstead's
continuation protocol follow library breadth. A sub-frame input queue requires demonstrated need.
**Trigger:** v2.0 shipped, or a frontend needs save states first. **Why next:** persistence and device state
will then be settled across the supported boards and regions.

## Mid — v4.0 "Measured accuracy" (SEED-003)

FPGA-RTL reference comparisons, diagnostic results from hardware, measured POKEY/timing improvements, XM
YM2151, complete Souper/BupChip and the composite artifact filter. Full clean-room OpenHSC and AtariVox speech
remain candidates, not commitments.
**Trigger:** v3.0 shipped, or a measured accuracy gap blocks a known game. Souper/BupChip stays here by the
owner's explicit v2.0 scope decision; memory mapping alone is not complete game/audio support.

## Long — v5.0 "Tools" (SEED-004), v6.0 "Everywhere" (SEED-005), 2600 mode (SEED-006)

v5.0: inspection and hook APIs, breakpoints, DMA/display-list views, traces with symbols, homebrew lint, Python
bindings with a vectorised AI environment, Lua scripting in the runner, speedrun and TAS aids.
v6.0: console and handheld ports with measured frame-time and battery budgets, a supported big-endian tier, a
Raspberry Pi budget, Playstead integration (the browser build moved to SEED-008).
2600 mode: the 7800's backward compatibility with Atari 2600 cartridges — owner decision 2026-10-15: after the
7800 library is complete. **Trigger:** v2.0 closed and the owner promotes it.

## Decision log

| Date | Change | Why |
|---|---|---|
| 2026-10-15 | Initial horizons set during preparation | Library share from the CC0 MAME list; sibling velocity evidence; owner decisions (2600 later, RetroArch frontend, no GUI in the core repository) |
| 2026-10-15 | The browser build leaves v6.0 as SEED-008, weighed at the v1.0 close; a wasm32 hash lane is a phase 1 discuss question; efficiency becomes a PROJECT constraint | Owner, at the kickoff: running efficiently in the browser through WebAssembly is very important, and battery life on handhelds matters |

| 2026-10-21 | v1.0 reconciled as shipped; v2.0 whole-library scope promoted with tune-up first, phases 5–11 | Owner approved 27 requirements and seven phases. Library breadth/saves/controllers/PAL precede a browser player. Souper/BupChip deferred with later accuracy; unsupported and untested entries stay explicit. |
