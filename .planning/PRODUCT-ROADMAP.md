---
title: "Product roadmap (rolling)"
summary: "Immediate / Near / Mid / Long direction for muhtari with the evidence and promotion trigger for each horizon; ROADMAP.md is the execution contract, seeds hold the source narratives."
read_when: "At every new-milestone and complete-milestone step, and when a finding changes priorities."
updated: 2026-10-15
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

## Immediate — v1.0 "7800 games play with sound in RetroArch" (committed)

Four phases, each ending in an automated release: (1) a test frame in RetroArch — the whole build, CI and release
pipeline with a ROM-free test card; (2) the 6502C "SALLY" CPU matches the public 6502 vectors on every bus cycle,
with the 7800's slow cycles, HALT and RDY; (3) games play — MARIA graphics with exact DMA timing, `.a78` loading,
linear and SuperGame cartridges (187 of the 204 known titles), joysticks and console switches, fast boot and an
open-BIOS boot path, hash-pinned homebrew on six platforms; (4) sound — TIA audio and cartridge POKEY (195 of
204), audio hashes on six platforms, one integrated play session.
Requirements: `.planning/REQUIREMENTS.md` v1. Evidence: `.planning/research/SUMMARY.md`.

## Near — v2.0 "The whole library" (SEED-001)

Every remaining cartridge board, the XG-1 light gun, paddles and the other controllers, SaveKey/AtariVox and
High Score Cartridge saves, full PAL, `.srm` saves, and a compatibility ledger over the whole known library.
Opens with the milestone tune-up (SEED-007), which this time also adds the weekly triage digest, the monthly
CI-efficiency report and the `quality.json` ratchet.
**Trigger:** v1.0 shipped. **Why next:** cartridge coverage and saves are what players notice first.

## Mid — v3.0 "States and movies" (SEED-002), v4.0 "Measured accuracy" (SEED-003)

v3.0: validated fixed-size save states, rewind and run-ahead in RetroArch, deterministic input movies with
recording, a sub-frame input queue only if a game or TAS is shown to need it, and Playstead's continuation
protocol in the runner.
**Trigger:** v2.0 shipped, or a frontend (Playstead) needs save states first.
v4.0: an FPGA-RTL reference oracle, diagnostics run on real hardware, our POKEY checked against the RTL, the XM
expansion (YM2151), the composite artifact filter.
**Trigger:** v3.0 shipped, or a measured accuracy gap blocks a known game.

## Long — v5.0 "Tools" (SEED-004), v6.0 "Everywhere" (SEED-005), 2600 mode (SEED-006)

v5.0: inspection and hook APIs, breakpoints, DMA/display-list views, traces with symbols, homebrew lint, Python
bindings with a vectorised AI environment, Lua scripting in the runner, speedrun and TAS aids.
v6.0: console and handheld ports, a supported big-endian tier, a Raspberry Pi budget, a WASM web demo, Playstead
integration.
2600 mode: the 7800's backward compatibility with Atari 2600 cartridges — owner decision 2026-10-15: after the
7800 library is complete. **Trigger:** v2.0 closed and the owner promotes it.

## Decision log

| Date | Change | Why |
|---|---|---|
| 2026-10-15 | Initial horizons set during preparation | Library share from the CC0 MAME list; sibling velocity evidence; owner decisions (2600 later, RetroArch frontend, no GUI in the core repository) |
