# Project Research Summary

**Project:** muhtari
**Domain:** Atari 7800 emulator core in C (embeddable library, headless runner, libretro core)
**Researched:** 2026-10-15
**Confidence:** HIGH for structure, engineering and licences; MEDIUM for a few fitted timing constants (named,
cited and testable — see Gaps)

## Executive Summary

muhtari is an emulator core, not an application. The 7800's difficulty is timing: the MARIA graphics chip halts
the 6502C "SALLY" CPU to fetch display lists every line, TIA and RIOT accesses stretch CPU cycles, and games race
that budget. Accurate designs count time in MARIA clock ticks, let the CPU drive the bus, and charge MARIA's DMA
fetch by fetch. muhtari does that with one exact DMA burst per line and lazily caught-up devices, in integer
arithmetic only, so its output hashes are identical on every platform and it stays cheap enough for low-power
hardware.

No permissive, accurate, embeddable 7800 core exists: the only libretro core is GPL ProSystem with per-line timing.
muhtari reuses the owner's own MIT NES core for the CPU, synthesiser, state, runner, libretro adapter and CI, and
writes the 7800 chips from the Software Guide, schematics, FPGA-RTL facts and hardware measurements. It releases
from the first phase, so each phase's result can be downloaded and played.

## Key Findings

### Recommended Stack

C17 without extensions, CMake workflow presets, Ninja and CTest, no runtime dependencies; pinned build tools for
fixtures. See `.planning/research/STACK.md` and `.planning/preparation/ENGINEERING.md`.

### Expected Features

v1.0: test card through every host, the CPU against public vectors, linear and SuperGame cartridges with exact
MARIA timing, joysticks and console switches, fast boot and an open-BIOS boot path, TIA and POKEY sound, hashes
identical on every platform, automated attested releases. Later: every board and peripheral, saves and PAL (v2.0);
states and movies (v3.0); measured accuracy and XM (v4.0); tools, scripting and AI environments (v5.0); ports
(v6.0); 2600 cartridges after the 7800 library. See `.planning/preparation/A7800-ECOSYSTEM-AND-USERS.md`.

### Architecture Approach

One timeline counted in MARIA clock ticks (4 per CPU cycle, 6 for TIA/RIOT). The CPU drives the bus; a deadline
dispatcher fires line events; MARIA runs each line's DMA as one burst with per-fetch ticks and renders pixel spans
up to the present before register writes; TIA and POKEY emit timed level transitions into an integer
band-limited synthesiser. The public API is an opaque instance with caller-owned buffers. Details:
`.planning/preparation/ARCHITECTURE.md`.

**Major components:**
1. Core library — SALLY CPU, bus, MARIA, TIA, RIOT, ports, cartridge boards, POKEY, synthesiser, boot, state
2. Runner — loads cartridges, replays input, prints frame/audio hashes, probes images, benchmarks
3. libretro core — converts the native output for RetroArch; memory maps for achievements

### Critical Pitfalls

1. Fitted, conflicting DMA/HALT timing — named, cited constants; one `halt_window()`; own diagnostic ROMs;
   behaviour-revision bump per refit.
2. Licence contamination (GPL emulators, Atari firmware inside permissive repositories) — PROVENANCE seeds only;
   GPL never opened; firmware never committed.
3. Process overhead outgrowing the emulator (two siblings stalled) — proportional gates, bounded real-host e2e.
4. Release automation blocking on tokens or formatting — App token, version-free assets, release PR checked at
   every phase close.

## Implications for Roadmap

Milestone heading for ROADMAP.md: `## v1.0 7800 games play with sound in RetroArch`. Four phases. Merging each one
publishes a release.

### Phase 1: A test frame in RetroArch
**Rationale:** The first release already runs in RetroArch, so every later phase ships by merging.
**Delivers:** The library, the runner and the libretro core showing a built-in test card; build and test presets;
CI on six platforms plus the big-endian lane; repository settings and ruleset; automatic attested releases; the
one-line RetroArch install.
**Addresses:** FRAME-01 to FRAME-09
**UI hint**: no
**Canonical refs:** `.planning/preparation/ENGINEERING.md`, `.planning/preparation/HOSTS-AND-INTEGRATION.md`, `.planning/preparation/ARCHITECTURE.md`, `PROVENANCE.md`

### Phase 2: The CPU matches the public vectors
**Rationale:** Public vectors judge the 6502C on its own before any other chip exists; the 7800's bus timing (slow
cycles, HALT, RDY) and RIOT come with it.
**Delivers:** The SALLY CPU seeded from the owner's NES core with decimal mode and per-cycle NMI; the bus with
4/6-tick cycles, HALT and RDY; RIOT, INPTCTRL and open bus; the vector sample per pull request and the full set
nightly; the cartridge probe and loader with the CC0 hash list and its fuzz target.
**Addresses:** CPU-01 to CPU-05
**UI hint**: no
**Canonical refs:** `.planning/preparation/A7800-SYSTEM.md`, `.planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md`, `.planning/preparation/ARCHITECTURE.md`, `.planning/preparation/CONFORMANCE.md`

### Phase 3: Games play in RetroArch
**Rationale:** MARIA, the cartridge loader and controller input together make the first playable result.
**Delivers:** MARIA with exact DMA timing; linear and SuperGame boards; fast
boot and the OpenBIOS path; joysticks and console switches; 7800basic samples and own diagnostic ROMs pinned on
every platform; boot-to-play proof; input movies; property tests; achievement memory maps; the library preset.
**Addresses:** GAME-01 to GAME-06
**Uses:** the DMA-burst and span-rendering model in `ARCHITECTURE.md`
**UI hint**: no
**Canonical refs:** `.planning/preparation/A7800-MARIA.md`, `.planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md`, `.planning/preparation/A7800-SYSTEM.md`, `.planning/preparation/CONFORMANCE.md`, `.planning/preparation/HOSTS-AND-INTEGRATION.md`

### Phase 4: Sound
**Rationale:** TIA and POKEY depend on the exact bus timing of the earlier phases, and sound completes a playable
game.
**Delivers:** TIA audio on its own lattice; an own integer POKEY at the header's address; the integer mix and
band-limited synthesis; transition and PCM hashes on every platform; audible boot-to-play.
**Addresses:** SND-01 to SND-05
**UI hint**: no
**Canonical refs:** `.planning/preparation/A7800-AUDIO.md`, `.planning/preparation/ARCHITECTURE.md`, `.planning/preparation/CONFORMANCE.md`

### Phase Ordering Rationale

Release pipeline first (every later phase ships by merging), then the component that public vectors can judge
alone, then the first playable picture, then sound, which needs the finished bus timing. Each phase opens with a
tracer plan through every layer before widening.

### Research Flags

Phases likely needing deeper research during planning:
- **Phase 3:** DMA edge cases (indirect holes, the HALT hand-shake split, first-pixel tick) — measured values are
  partly fitted; the diagnostic ROMs pin our model.
- **Phase 4:** POKEY power-on state and the TIA:POKEY level ratio.

Phases with standard patterns:
- **Phase 1:** the owner's NES sibling shipped this skeleton in a day.
- **Phase 2:** exhaustive public vectors.

## Confidence Assessment

| Area | Confidence | Notes |
|------|------------|-------|
| Stack | HIGH | Re-verified runner images, actions, release-please on 2026-10-15 |
| Features | HIGH | Library shares counted from MAME's CC0 list; personas from community sources |
| Architecture | HIGH | Reproduces the Software Guide and an RTL re-simulation of MARIA's DMA logic |
| Pitfalls | HIGH | Four sibling projects' histories |

**Overall confidence:** HIGH

### Gaps to Address

- HALT latency, the DLI recognition tick and indirect hole costs are fitted (A7800-SYSTEM, A7800-MARIA).
- RIOT port B power-on value (sources support ORB = $FF; local predicates confirm) and the $2800 mirror (DECISIONS, open questions).
- 242 vs 243 visible rows (A7800-MARIA §6).

## Sources

The files under `.planning/preparation/`, each with its own Sources table of pinned commits, wiki revisions and
access dates; `.planning/preparation/README.md` is the index.

---
*Research completed: 2026-10-15*
*Ready for roadmap: yes*
