# Playstead handoff: prioritize Atari 7800 with muhtari

**Owner intent, 2026-10-29:** use the owner's 7800 core as an early end-to-end
proof of ordinary gaming in Playstead. Save/load is high priority; AI play, TAS
and speedrun tooling are later. Aim for Playstead's calm “pick, play, keep your
place” experience. The immediate route and eventual native experience are distinct.

## Paste into the Playstead session

```text
Prioritize Atari 7800 play through muhtari as an early end-to-end proof of Playstead
launching an emulator core I built. The sibling repository is ~/projects/muhtari.
Read its AGENTS.md, then .planning/README.md, INTENT.md, PROJECT.md, STATE.md,
REQUIREMENTS.md, ROADMAP.md and PRODUCT-ROADMAP.md. For this integration, read
.planning/research/{SUMMARY,FEATURES,ARCHITECTURE,PITFALLS}.md and this handoff.
Use the relevant existing Playstead instructions, PRODUCT.md and brand/BRAND.md.

Current shipped muhtari is package v0.10.0: C library, headless runner and libretro
core, API7/ABI1/behavior6. It has NTSC/PAL, sound, scoped controllers/boards and
raw HSC/SaveKey battery persistence. User reports: Choplifter loaded via GUI and
played well with keyboard, good casual graphics/audio/speed; Ninja Golf NTSC
also played great. These are scoped voluntary observations, not a universal
compatibility claim or recurring manual verification task.

Save states, rewind and run-ahead do NOT ship yet. Muhtari's v3.0 is being scoped
to deliver safe portable states through library/runner/libretro and verified
frontend state consumers. Do not assume the new API exists before its release.

Quickest playable path: Playstead launches a separately verified RetroArch app
with the separately verified muhtari libretro core and a verified local 7800 ROM.
The command shape is RetroArch -L <core> <rom>, with a Playstead-controlled local
config, per-game save directory and optional firmware directory. RetroArch owns
the game window, audio and input. muhtari-run is headless automation, not a player.
Muhtari binaries: https://github.com/szTheory/muhtari/releases/latest
Use exact tested versions/digests for an adapter pin, not a moving latest URL.

Read the current Playstead AdapterHost/AdapterInstaller/AdapterPin/AdapterCatalog,
GameRow launch readiness/controller gates and state-support code. The inspected
tree at 3e12f16a75ec had relevant uncommitted changes and a standalone mGBA pin;
re-read current code before applying the findings. The existing installer copies
one app from a DMG. The RetroArch route needs verified app + core assets, a7800
readiness and actual controller/keyboard/focus/process handoff. Do not assume the
mGBA/SDL config bridge configures RetroArch. Keep adapter/cache/session/UI changes
in Playstead; keep emulation/state capability changes in muhtari.

Prove the actual Playstead -> RetroArch -> muhtari path on an owned legal fixture:
launch from the library, visible expected output, usable input, orderly exit and
two-fresh-process battery progress. Muhtari's host tests alone cannot prove that
flow. Coordinate the two projects' evidence explicitly, without waiting for me to
keep manually testing. Keep personal ROMs/BIOS/saves/captures local and out of Git.

Distinguish battery progress, an emulator save state and Playstead's Continue UI.
The inspected Continue shelf is recent play; it is not exact state restore.
Current manual state history is qualified for a narrow GB row, not 7800. Start
with honest RetroArch-managed save/load once muhtari ships serialization. A
Playstead-managed exact Continue/history needs a supported capture/load boundary,
strong ROM/core/config/firmware identity and recovery of newer durable saves when
loading an older state. Relaunch alone does not resume the interrupted game point.

North star: one Playstead play/Continue flow with native presentation, responsive
keyboard/controller/accessibility support, good sound/pacing, safe automatic state
capture and clear recovery. A Playstead-owned in-process libretro host is a
candidate, not a settled implementation: compare isolation, A/V/input/pacing work,
core lifecycle and state UX against the external route. Promote a focused host
spike when measured external-window/controller/state friction or an explicit
one-app release requirement justifies it. Muhtari stays a core with three existing
deliverables; it gets no GUI/audio-device/input-device code.

Use muhtari as the current leading example for shared emulator-family DNA, while
learning in both directions. Inspect rationale/source evidence, adapt locally and
treat transferred technical lessons as hypotheses until reproduced. Prefer small
understood permissive reuse and standard facilities to new dependencies. Fan out
only relevant independent decisions, do one adversarial synthesis, stop at
diminishing returns and retain high-signal findings/provenance/revisit triggers.
Refresh immediate/next/mid/long horizons at each milestone boundary. Preserve each
repo's workflow and current work; don't silently expand or chain GSD steps.
```

## Source map and limits

- [FEATURES.md](FEATURES.md) records exact Playstead product/brand/code paths and
  inspected dirty files at `3e12f16a75ec`. Playstead was read only in this session.
- [SUMMARY.md](SUMMARY.md) links state safety, battery custody, host acceptance and
  sources. The libretro API is the seam; copying a GPL frontend is not proposed.
- [Requirements](../REQUIREMENTS.md) define muhtari readiness, not completion of
  Playstead's adapter. Concrete state layout, protocol, host pins and detailed plans
  remain later decisions against current consumers and source evidence.
