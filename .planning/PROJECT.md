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

(None yet — ship to validate)

### Active

- [ ] One command installs the latest release into RetroArch on an Apple Silicon Mac, and a hosted test shows
      RetroArch displays the runner's frame.
- [ ] `cmake --workflow --preset ci` (`ci-msvc` on Windows) builds and tests everything on Linux, macOS and
      Windows, x64 and arm64, and CI runs the same commands behind one required check.
- [ ] Every behaviour-changing merge to `main` publishes a release (library, runner, libretro core) with no
      manual step.
- [ ] The 6502C "SALLY" CPU matches the public 6502 vectors on every opcode and every bus cycle, with the 7800's
      slow cycles, HALT and RDY.
- [ ] Linear and SuperGame cartridges render with exact MARIA DMA timing, take joystick and console-switch input,
      and play with TIA and POKEY sound in RetroArch.
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
| Time in MARIA clock ticks; the CPU drives the bus; MARIA DMA runs as one exact burst per line | Every documented edge is an integer; cheaper than per-tick stepping on low-power targets | — Pending |
| Integer-only core, no globals, bounded run calls | Bit-identical hashes everywhere; hostile ROMs cannot hang a host | — Pending |
| Native MARIA byte per pixel; palettes and composite filter outside the hash | Hash stability; hosts choose presentation | — Pending |
| Audio hashed as a transition stream; PCM secondary | Determinism independent of the mixer and output rate | — Pending |
| Reuse the owner's nesturbator code; MARIA, TIA, RIOT, POKEY and boards written from documentation and cross-checked | Fastest correct path; existing permissive chip models are lumped or diverge from documentation | — Pending |
| Fast boot by default; CC0 OpenBIOS for the BIOS path in CI; no Atari bytes | Redistributable and legal | — Pending |
| Tests on licence-checked inputs: public 6502 vectors, 7800basic samples built in CI, own diagnostic ROMs | Reproducible without a network or private ROMs | — Pending |
| One compatibility ledger in which a pass stays a pass | A regression is a one-line diff | — Pending |
| One entrypoint: CMake workflow presets; one required CI check | No script to keep in step with CI | — Pending |
| release-please with a GitHub App token; version-free asset names; unsigned macOS artifacts installed by script | Automatic releases with no owner step | — Pending |
| With no cartridge loaded the core outputs a built-in test card | A build or a release can be checked without any ROM | — Pending |
| Linear and SuperGame boards first, then POKEY | 187 of 204 known titles; 195 with POKEY | — Pending |

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
*Last updated: 2026-10-15 after the kickoff (efficiency constraint)*
