# muhtari

## What This Is

muhtari is an Atari 7800 ProSystem emulator core written in C. It ships as a library other programs embed, a
headless command-line runner for automation, and a libretro core so that RetroArch and other hosts can play 7800
games with it. It is for people who build emulator hosts and want an accurate, fast, permissively licensed 7800
core; for players, homebrew developers, ROM hackers, speedrunners, TAS authors and AI researchers who use those
hosts; and for the owner, who tries every release on a Mac in RetroArch with cartridges from a personal collection.

## Core Value

7800 games behave as they do on the console, identically on every platform, from a small MIT-licensed C library
that any host can embed.

## Requirements

### Validated

- ✓ `cmake --workflow --preset ci` (`ci-msvc` on Windows) builds and tests everything on Linux, macOS and
  Windows, x64 and arm64, and CI runs the same commands behind one required check — Phase 1
- ✓ One command installs the latest release into RetroArch on an Apple Silicon Mac, and a hosted test shows
  RetroArch displays the runner's frame — Phase 1; confirmed when v0.1.0 published (Release run 38983378877
  passed install-line-latest)
- ✓ Every behaviour-changing merge to `main` publishes a release (library, runner, libretro core) with no
  manual step — Phase 1; confirmed by v0.1.0 (release pull request #8 merged itself, Release run 38983378877)
- ✓ The 6502C "SALLY" CPU matches the public 6502 vectors on every opcode and every bus cycle, with the 7800's
  slow cycles, HALT and RDY — Phase 2
- ✓ The runner probes cartridge images and the core loads one only after validating it, with no allocation on a
  refusal and a loader fuzz target run nightly — Phase 2

- ✓ Linear and SuperGame cartridges render with exact MARIA DMA timing, boot fast or through OpenBIOS,
  take joystick and console-switch input, and play in RetroArch with cross-platform frame hashes — Phase 3

### Active

- [ ] TIA and POKEY sound plays in RetroArch with the rendered cartridges.
- [ ] Frame and audio hashes are identical on every supported platform, including a 32-bit big-endian lane.

### Out of Scope

- A GUI, or any window, audio-device or input-device code — RetroArch (and later Playstead) is the host.
- Shaders, menus, input remapping, cheats, patching, rewind buffers and netplay lobbies — these belong to the host;
  the core provides deterministic state and memory maps.
- Any Atari BIOS, High Score Cartridge ROM, BIOS-loader bytes or game ROM in the repository or its releases —
  players supply their own; the default boot needs no BIOS.
- Code from GPL, LGPL or unlicensed emulators — the project is MIT; such emulators run only as test oracles.
- A separate fast or low-accuracy mode — one accurate core keeps one set of bugs and one set of hashes.
- Signed or notarised macOS artifacts — the script install path does not need them.
- A test framework dependency — plain CTest executables with an in-repo check header cover the need.
- Atari 2600 cartridges, the remaining boards, peripherals, saves, PAL, save states, movie recording, measured-accuracy
  oracles, developer tools and ports — planned for later milestones; see `.planning/seeds/` and
  `.planning/PRODUCT-ROADMAP.md`.

## Context

- **Ecosystem.** The only libretro 7800 core is ProSystem (GPL-2, per-line timing, many homebrew failures); the
  accuracy references are a7800 (MAME-derived, last release 2022), BizHawk's A7800Hawk (C#) and the MiSTer FPGA
  core (MIT since 2026-08-30). No permissive, accurate, embeddable C core exists, and every software emulator runs
  at 60/50 Hz or the 2600's rate instead of the 7800's own 59.958/49.920 Hz.
- **Sibling projects.** The owner's NES (nesturbator), Game Boy (gabbaboy) and Neo Geo (glueyneo) cores share
  this project's API conventions and engineering; nesturbator's own MIT code seeds the CPU, synthesiser, state,
  runner, libretro adapter and CI. Playstead, the owner's host, launches emulator processes and needs
  deterministic input replay and safe battery saves from them.
- **How releases are tried.** In RetroArch (Metal build) on an Apple Silicon Mac, installed by a one-line script.
- **Reference material.** Each file states facts with sources and one recommendation per choice. Open the one that
  matches the work before designing or building. Index: `.planning/preparation/README.md`.
  - `.planning/preparation/ARCHITECTURE.md` — time model, modules, public API, I/O contracts, observability
  - `.planning/preparation/DECISIONS.md` — choices already made, one line each
  - `.planning/preparation/A7800-SYSTEM.md` — clocks, memory map, SALLY, HALT/RDY, INPTCTRL, RIOT, boot
  - `.planning/preparation/A7800-MARIA.md` — display lists, read modes, DMA timing, video output, palettes
  - `.planning/preparation/A7800-AUDIO.md` — TIA audio, POKEY, mixing, synthesis, YM2151
  - `.planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md` — A78 header, boards, controllers, saves
  - `.planning/preparation/CONFORMANCE.md` — test ROMs, oracles, diagnostics, the ledger, CI tiers
  - `.planning/preparation/HOSTS-AND-INTEGRATION.md` — libretro, RetroAchievements, RetroArch, runner, Playstead
  - `.planning/preparation/ENGINEERING.md` — C rules, presets, tests, CI, release, repository settings
  - `.planning/preparation/A7800-ECOSYSTEM-AND-USERS.md` — other emulators, complaints, personas, device budgets
  - `PROVENANCE.md`, `ASSET_POLICY.md` — what code and which bytes may enter the repository

## Constraints

- **Deliverables**: a C library, a headless runner and a libretro core only — the host owns windows, audio devices
  and input devices.
- **Language**: C17 without extensions; the core links only the C memory functions — it has to embed anywhere.
- **Reuse**: permissive code enters only as a seed with its notice; GPL, LGPL and unlicensed source is never opened
  — the repository is public and MIT.
- **Determinism**: integer-only core with all state in the instance — the same inputs give identical frame and
  audio hashes on every platform; every call is bounded in emulated time.
- **Efficiency**: efficient enough for battery-powered handhelds and for the browser through WebAssembly, which the
  owner ranks very important — work per emulated frame sets both speed and power draw, so the instructions-per-frame
  ratchet guards it from phase 1, and nothing enters the core that a wasm32 build cannot carry (threads, files,
  floating point, allocation after load). The browser build is SEED-008; handheld budgets are SEED-005.
- **Legal**: no ROM, BIOS or firmware bytes beyond licence-checked fixtures; Atari names only to say what the core
  emulates.
- **Privacy**: no personal paths, addresses or names in tracked files, commits or release artifacts.
- **Verification**: every behaviour is shown by a command; nothing waits on a person except identity-bound
  consent.
- **Delivery**: each phase is one branch and one pull request into a green `main`, released automatically — each
  phase ends in something to download and run.

## Key Decisions

| Decision | Rationale | Outcome |
|----------|-----------|---------|
| Time in MARIA clock ticks; the CPU drives the bus; MARIA DMA runs as one exact burst per line | Every documented edge is an integer; cheaper than per-tick stepping on low-power targets | — Phase 2: the CPU drives the bus in MARIA ticks (4- and 6-tick cycles, HALT, RDY), and the bus takes one HALT window and one NMI edge at a time and returns 1 or 0 for each request (what the NMI's 1 promises is settled with MARIA); the per-line DMA burst comes with MARIA in phase 3 |
| Integer-only core, no globals, bounded run calls | Bit-identical hashes everywhere; hostile ROMs cannot hang a host | ✓ Phase 1: tests enforce no floating point, no globals and the no-FP lanes; Phase 2: a step on a jammed CPU still makes one bus read, so emulated time keeps moving |
| Native MARIA byte per pixel; palettes and composite filter outside the hash | Hash stability; hosts choose presentation | ✓ Phase 1: frames are colour numbers; the runner and the libretro core apply the palette |
| Audio hashed as a transition stream; PCM secondary | Determinism independent of the mixer and output rate | — Pending |
| Reuse the owner's nesturbator code; MARIA, TIA, RIOT, POKEY and boards written from documentation and cross-checked | Fastest correct path; existing permissive chip models are lumped or diverge from documentation | — Phase 2: SALLY, seeded from nesturbator 9f9386c661f6, passes all 2,560,000 public vectors; the RIOT is written from the datasheet; MARIA, TIA, POKEY and the boards follow |
| Fast boot by default; CC0 OpenBIOS for the BIOS path in CI; no Atari bytes | Redistributable and legal | — Phase 2: release zips and tracked files are scanned for BIOS, HSC and known-ROM digests before publish; fast boot and OpenBIOS come with the boot path |
| Tests on licence-checked inputs: public 6502 vectors, 7800basic samples built in CI, own diagnostic ROMs | Reproducible without a network or private ROMs | ✓ Phase 2: the SingleStepTests sample runs on every PR and the full set nightly; 7800basic's `simple` sample builds byte-identical on all nine legs |
| One compatibility ledger in which a pass stays a pass | A regression is a one-line diff | ✓ Phase 3: the ledger, ratchet and generated compatibility page run in CI |
| One entrypoint: CMake workflow presets; one required CI check | No script to keep in step with CI | ✓ Phase 1: CI runs the presets on six platforms behind `CI required` |
| release-please with a GitHub App token; version-free asset names; unsigned macOS artifacts installed by script | Automatic releases with no owner step | ✓ v0.1.0 published by release-please with no owner step (Release run 38983378877) |
| With no cartridge loaded the core outputs a built-in test card | A build or a release can be checked without any ROM | ✓ Phase 1: the card's hash is pinned and RetroArch shows it pixel for pixel |
| Linear and SuperGame boards first, then POKEY | 187 of 204 known titles; 195 with POKEY | — Pending |
| The owner verifies nothing by default: tests, CI and the agent close every check, post-merge truths are confirmed at ship (DEC.09, 2026-10-16) | The owner's time goes to consent, not to reading docs or watching runs | ✓ Phase 1: UAT closed with no owner check |
| Public API version 2 adds `muhtari_probe`; a cartridge is validated before anything is allocated (D-01, D-02, D-27) | A host can identify an image without loading it; a hostile image cannot make the core allocate | ✓ Phase 2: `cart.load.no_alloc_on_reject` covers every refusal class; the loader fuzz target runs nightly |
| The CPU, bus and cartridge tables build once as OBJECT libraries inside the one shipped archive (D-14) | Tests run the CPU object on a flat bus without the library; one archive still ships | ✓ Phase 2: Cachegrind measured a 0.31 % Ir gap against a single translation unit |

The full list with sources is `.planning/preparation/DECISIONS.md`.

## Evolution

This document evolves at phase transitions and milestone boundaries.

**After each phase transition** (via `/gsd-transition`):
1. Requirements invalidated? → Move to Out of Scope with reason
2. Requirements validated? → Move to Validated with phase reference
3. New requirements emerged? → Add to Active
4. Decisions to log? → Add to Key Decisions
5. "What This Is" still accurate? → Update if drifted

**After each milestone** (via `/gsd-complete-milestone`):
1. Full review of all sections
2. Core Value check — still the right priority?
3. Review Out of Scope — reasons still valid?
4. Update Context with current state

---
*Last updated: 2026-10-20 after Phase 3 re-verification; release confirmation remains at ship*
