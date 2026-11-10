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

The current immutable muhtari release is v0.11.1: C library, headless runner and
libretro core. It includes core state serialization and raw HSC/SaveKey battery
memory. Use the [libretro integration contract](../../docs/libretro.md#external-frontend-contract)
for the published macOS arm64 archive identity, extracted core/info digests,
attestation receipt and separately pinned RetroArch app. The linked release run
checks DMA output and two-process battery continuity, but it has no held/released
gameplay-input step. It does not establish the complete player path or a Playstead
integration result.

RetroArch owns its save/state files and initial state UI. Core serialization is
available; actual pinned RetroArch save/load, rewind and run-ahead behavior remains
unqualified. Battery persistence and an emulator state are different things, and
neither makes a recent-play entry an exact restore point.

Quickest playable path: Playstead launches a separately verified RetroArch app
with the separately verified muhtari libretro core and a verified local 7800 ROM.
Use `RetroArch --config <local-config> -L <core> <rom>` with a Playstead-controlled
per-game content/save/state directory and optional firmware directory. RetroArch
owns the window, audio, input devices and file persistence. Playstead owns adapter
pins/install verification, `a7800` readiness, controller and keyboard mappings,
focus and the child lifecycle/return. `muhtari-run` is headless automation, not a
player. Keep the convenient [latest release](https://github.com/szTheory/muhtari/releases/latest)
link for README users; adapter pins must record immutable tested identities. The
muhtari-side boundary and current released identities are in the
[external frontend contract](../../docs/libretro.md#external-frontend-contract).

Read-only inspection on 2026-11-09 found the Playstead monorepo at
`67d25183468d3cbc774b6c91946f8a0c25518d4b`. Relevant working-tree changes were
present in `playstead-mac/Playstead/Adapter/AdapterCatalog.swift`,
`playstead-mac/Playstead/Adapter/AdapterHost.swift`,
`playstead-mac/Playstead/Adapter/AdapterInstaller.swift`,
`playstead-mac/Playstead/Adapter/AdapterPin.json`,
`playstead-mac/Playstead/Adapter/StateSupportMatrix.swift`,
`playstead-mac/Playstead/Library/GameRowView.swift`, and
`playstead-mac/docs/STATE-SUPPORT.md`; `playstead-mac/docs/CONTINUATION-PROTOCOL.md`
was untracked. Treat this as an inspected dirty snapshot, not committed integration
or acceptance evidence, and re-inspect the exact source revision before implementing
against it. The catalog and pin still describe one GBA/mGBA 0.10.5 adapter; there is
no 7800/muhtari pin or paired launch flow. Recent-play launch does not restore an
emulator state. The currently qualified state UI covers Game Boy MBC1 with plain
8 KiB SRAM, not 7800. Do not assume the mGBA/SDL config bridge configures RetroArch.
Keep adapter/cache/session/UI changes in Playstead; keep emulation/state capability
changes in muhtari.

The paired Playstead -> RetroArch -> muhtari result remains pending. Close it only
with a current Playstead revision and dirty-source record, exact immutable app/core
pins and installed-asset verification, owned fixture/content identity, expected
visible response, usable gameplay input with keyboard/controller focus, graceful
child exit and return, and battery continuity across two fresh processes. Existing
core/RetroArch host evidence does not prove the Playstead launch or input path.
Keep personal ROMs/BIOS/saves/captures local and out of Git.

Distinguish battery progress, an emulator save state and Playstead's Continue UI.
`playstead-mac/Playstead/Curation/ContinueShelfView.swift` labels the shelf
"Recently Played"; opening it launches the game row and is not exact state restore.
The current manual state support is qualified for a narrow GB MBC1/8 KiB SRAM
configuration, not 7800. Begin with RetroArch-managed state files after separately
qualifying the released core in the pinned frontend. A Playstead-managed exact
Continue/history needs a supported capture/load boundary, strong ROM/core/config/
firmware identity and recovery of newer durable saves when loading an older state.
Relaunch alone does not resume the interrupted game point.

North star: one Playstead play/Continue flow with native presentation, responsive
keyboard/controller/accessibility support, good sound/pacing, safe automatic state
capture and clear recovery. A Playstead-owned in-process libretro host is a
candidate, not a settled implementation: compare isolation, A/V/input/pacing work,
core lifecycle and state UX against the external route. Keep the current
RetroArch-managed state UI as the first route. Promote a focused host spike when
measured external-window/controller/state friction or an explicit one-app release
requirement justifies it. Muhtari stays a core with three existing deliverables;
it gets no GUI/audio-device/input-device code.

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

- [FEATURES.md](FEATURES.md) records the product/brand baseline. The current
  source snapshot and dirty-path limits are refreshed above; Playstead was read
  only and no sibling source was changed.
- [SUMMARY.md](SUMMARY.md) links state safety, battery custody, host acceptance and
  sources. The libretro API is the seam; copying a GPL frontend is not proposed.
- [Requirements](../REQUIREMENTS.md) define muhtari readiness, not completion of
  Playstead's adapter. Concrete state layout, protocol, host pins and detailed plans
  remain later decisions against current consumers and source evidence.
