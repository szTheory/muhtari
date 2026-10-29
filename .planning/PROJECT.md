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

## Current State

Planning milestone **v1.0** shipped 2026-10-21 with package **v0.4.0**.
The currently published package is [v0.7.0](https://github.com/szTheory/muhtari/releases/tag/v0.7.0),
published2026-10-25 after Phase8 merged in [PR31](https://github.com/szTheory/muhtari/pull/31). It has19 assets; no release pull request is pending at Phase9 closeout.
Four phases, 73 plans and 149 recorded tasks deliver games with sound in RetroArch.
[Release publication and latest-install run](https://github.com/szTheory/muhtari/actions/runs/39533130210) completed successfully; v0.4.0 is published with 19 assets.
At that release, the C/header tree had 38,684 tracked lines, including 8,577 in src/;
its public API was version 3, ABI 1, behaviour revision 2. The library remains integer-only,
instance-owned and bounded, without allocation after load.

Closeout is **override_closeout**: 15 items acknowledged as deferred (six todos, eight dormant seeds and one compatibility-test output race); four stale phase verification fingerprints explicitly accepted.
The [audit](milestones/v1.0-MILESTONE-AUDIT.md) records 25/25 scoped requirements
and no integration gaps. At that historical closeout, gate code was 15.94% of src/ and performance comparison
was report-only. Those values are superseded by Phase 5 below; the old audit stays historical.

Phase 5 is implemented, verified and merged in PR #20: 20/20 substantive truths and
nine active outcomes accepted. Direct CI 39879193530 and Release 39879193669 pass,
including genuine publish/download/install/confirmed cleanup. Its historical gate audit
recorded15,287 lines across147 files and retired an unused78-line allowance. The accepted
2026-10-23 policy subsequently retired maintenance-line admission and its exclusive machinery;
these historical quantities are not current limits. The controlled
test-card performance comparison is armed; WASM/library/property remain advisory.
Matched local scheduling improves wall time 18.24%; no global hosted-cost reduction
is claimed. WR-01 and COST-01 remain deferred tooling seeds; raw monthly numeric
cost totals are untrusted. See [Phase 5 verification](phases/05-pipeline-tune-up/05-VERIFICATION.md).
Phase 5 shipped at6d57d69. Phase 6 is verified complete: Activision, Absolute and
Fractalus execute through all three deliverables, with identified-layout equivalence,
physical RAM aliases, bounded hostile-input evidence and honest host maps. That phase introduced
API4/ABI1/behavior3. Verification passes33/33; all six native archive sums fit
the unchanged399937-byte limit (worst375270). Direct CI40031550434, Nightly40031550533
and Release40031550534 attempt2 pass, including confirmed rehearsal cleanup.
Phase 6 merged in [PR24](https://github.com/szTheory/muhtari/pull/24); v0.5.0 is published.

Phase 7 is verified complete and shipped in PR28: API5/ABI1/behavior4 delivers
HSC/SaveKey battery storage, checked runner persistence and real two-launch RetroArch saves.
All nine plans and SAVE-01–06 pass35/35 must-haves. Final CI40104749040 attempt2 and
Nightly40104750452 pass on source54485d6, with all six actual native archive sums
within399937 (worst393952). Full native/PPC641+6 and69-row equality, WASM580,
10000-case properties, fuzz and actual frontend/Windows controls pass. Review is clean
after both allowed fix rounds; no owner verification remains. See
[Phase 7 verification](phases/07-battery-saves/07-VERIFICATION.md).

Phase8 is verified complete and shipped in PR31: API6/ABI1/behavior5 adds
extended typed input, immutable movie2 replay and all in-scope controller models.
All14 plans and INPUT-01–05 pass49/49 truths; one review-fix round corrected a
short-packet aim boundary and physical button labels. Exact-source CI40157749294
passes native649+6 across six current/base lanes, full native/PPC649+6, WASM587,
342-row equality and actual RetroArch. All18 candidate ZIPs and authenticated
current/base measurements pass; DEC.61 keeps archive bytes report-only.
Nightly40157750598 retains an explicitly unaccepted advisory10000-case isolation
timeout; five other10000-case properties pass. Required isolation passes. Hardware
fits remain unmeasured; the quoted execution evidence retains its original scope.
See [Phase8 verification](phases/08-controllers-at-the-pins/08-VERIFICATION.md).

Phase9 is verified complete and ready for `$gsd-ship 9`: all8 plans and PAL-01–03
pass42/42 truths. Selected313-line timing,320x292 geometry/rational rates and warm
PAL presentation reach the library, runner and both libretro formats. Independent
startup plus600 full-frame evidence precedes956-row equality; all342 prior NTSC
observations remain exact. API6/ABI1/behavior5 remains unchanged. One review-fix
round adds16KiB private BIOS staging, resolving borrowed-media lifetime without
extra allocation or per-frame work; a poison-based regression proves failure
preservation and successful boot in both regions. Re-review is clean.

Fresh CI40233948646 and Nightly40233948516 attempt2 accept the reviewed source:
six native current654/base649+6, ASan637, full native/PPC654+6, WASM592, full vectors,
fuzz, authenticated package/performance evidence and the two real frontend cases.
One targeted PPC retry followed a29-minute external provisioning delay; the
unchanged retry setup completed in13 seconds. The advisory isolation property
still times out and remains unaccepted; model handoff/optical/RC fits and85:342
analog balance retain explicit confidence limits. No routine owner UAT remains.
See [Phase9 verification](phases/09-pal-games-and-audio/09-VERIFICATION.md).

Phase 11 is verified complete: all12 plans and COMP-01–03 pass16/16 truths.
The204 pinned CC0 entries remain201 unknown and3 skipped, with no metadata
counted as a gameplay pass. Public title-screen evidence, owned model conformance,
and unsupported scope retain their explicit labels. Fresh local CI673+6 and
public library47 pass; the counted run has0/204 present at0e86575f561b, with
planning-only dirty state. Two carried claim/output faults are accepted through
existing controls; no new review round or owner UAT. WR-02 helper/source-only
coverage and COMP-03 unclassified assumption remain explicit; hosted outcomes
are confirmed at ship. See [Phase11 verification](phases/11-full-library-acceptance/11-VERIFICATION.md).
Next separate command: $gsd-ship 11; milestone audit/archive follows shipping.

## Current Milestone: v2.0 The whole library

**Goal:** Expand 7800 library breadth with the remaining in-scope cartridge boards, persistent saves,
controllers and full PAL, beginning with a measured pipeline tune-up.

**Target features:**
- Activision, Absolute, Fractalus and BankSet boards; XBoard/XM RAM, POKEY and HSC support.
- HSC and SaveKey/AtariVox EEPROM with stable battery memory, runner saves and RetroArch persistence.
- XG-1 light gun, paddles, keypad, quadrature devices, SNES2Atari and Mega7800 at the pins.
- PAL timing, boot, video and audio across all three deliverables.
- Weekly triage/monthly CI-efficiency reporting, a quality ratchet and honest whole-library evidence.

The owner approved the 27 requirements and seven phases (5–11) on 2026-10-21. Souper/BupChip remains
with the later accuracy milestone; XM excludes YM2151 and AtariVox excludes speech. The browser player,
save states and other later horizons remain deferred. Planning v2.0 does not select a package version.

## Requirements

### Validated

- ✓ `cmake --workflow --preset ci` (`ci-msvc` on Windows) builds and tests everything on Linux, macOS and
  Windows, x64 and arm64, and CI runs the same commands behind one required check — v1.0, Phase 1
- ✓ One command installs the latest release into RetroArch on an Apple Silicon Mac, and a hosted test shows
  RetroArch displays the runner's frame — v1.0, Phase 1; confirmed when v0.1.0 published (Release run 38983378877
  passed install-line-latest)
- ✓ Every behaviour-changing merge to `main` publishes a release (library, runner, libretro core) with no
  manual step — v1.0, Phase 1; confirmed by v0.1.0 (release pull request #8 merged itself, Release run 38983378877)
- ✓ The 6502C "SALLY" CPU matches the public 6502 vectors on every opcode and every bus cycle, with the 7800's
  slow cycles, HALT and RDY — v1.0, Phase 2
- ✓ The runner probes cartridge images and the core loads one only after validating it, with no allocation on a
  refusal and a loader fuzz target run nightly — v1.0, Phase 2

- ✓ Linear and SuperGame cartridges render with exact MARIA DMA timing, boot fast or through OpenBIOS,
  take joystick and console-switch input, and play in RetroArch with cross-platform frame hashes — v1.0, Phase 3

- ✓ TIA and declared cartridge POKEY deliver sound through the library, canonical runner hashes and actual
  libretro batch/fallback paths; all eight sound samples are non-silent and clipping-free — v1.0, Phase 4
- ✓ Owned exact-q23-16x64-dc20 synthesis passes the production spectral/alias judge with renewed integer
  headroom/width/carry proofs; hostile legal chip traffic stays bounded — v1.0, Phase 4

- ✓ Reliable, measured pipeline and current release docs — TUNE-01–05, Phase 5 verified and shipped in PR #20.
- ✓ Activision, Absolute and Fractalus banking, physical RAM and three-host behavior — BOARD-01–03, Phase 6 shipped in PR24.
- ✓ HSC/SaveKey battery data survives host and frontend restarts — SAVE-01–06, Phase7 shipped in PR28.
- ✓ Source-qualified lightgun, analog/matrix, timed motion and complete adapter inputs across all three deliverables — INPUT-01–05, Phase8 shipped in PR31.
- ✓ Selected PAL timing, presentation and audio across all three deliverables — PAL-01–03, Phase9 verified42/42; shipping next.
- ✓ BankSet and XBoard/XM memory, POKEY and HSC with correct CPU/MARIA ownership — BOARD-04–05, Phase 10 verified 5/5; shipping next.

- ✓ Evidence-backed library accounting and isolated local evaluation — COMP-01–03, Phase 11 verified 16/16; ship next.

### Active

All v2.0 phase requirements are verified; Phase 11 shipping and milestone audit/archive remain.

Full requirements and scope boundaries: [REQUIREMENTS.md](REQUIREMENTS.md).

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
- Atari 2600 cartridges, Souper/BupChip, YM2151, Covox/ADPCM, ACE, full OpenHSC, AtariVox speech, save states,
  movie recording, measured-accuracy oracles, the browser player, developer tools and ports — later milestones;
  see `.planning/seeds/` and `.planning/PRODUCT-ROADMAP.md`.

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
  comparison measures it from phase 1 and is armed after the Phase 5 controls, and nothing enters the core that a wasm32 build cannot carry (threads, files,
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
| One load-selected regional profile | Keep integer timing, geometry and audio carry consistent across existing owners | ✓ Phase9: PAL and retained NTSC behavior verified across all hosts/platforms |
| Judge numeric PAL behavior before hash agreement | Independent expectations catch errors shared by hosts | ✓ Phase9:601 calls,577879 samples,CRC52a5 precede956-row equality |
| Stage borrowed firmware before load commit | Installation can invalidate public input spans while failure must preserve the live machine | ✓ Phase9:16KiB private BIOS staging, poison-based regression and both-region lifecycle proof |
| Separate typed input from frozen legacy input | Preserve existing linkage while adding transactional selection and once-only motion | ✓ Phase8: API6,88-byte extended packet and linked old clients verified |
| Sense native emission through existing chip/deadline owners | Retain historical light and bounded unpolled behavior independently of host buffers | ✓ Phase8: explicit unmeasured optical/RC/matrix/motion fits, complete adapter edges |
| Judge independent controller numbers before hash agreement | Equal implementations alone can share an error | ✓ Phase8: thirteen scenarios,342 rows, actual library/runner/module and cross-platform evidence |
| One load-stable raw battery span and frozen attachment | Hosts restore directly before execution without mirrored storage or hot resizing | ✓ Phase 7: API5, layout1, exact HSC/EEPROM/combined lengths and old-layout linkage |
| Core commits hardware bytes; runner and frontend own persistence | Keep deterministic emulation separate from host files and controls | ✓ Phase 7: real serial deadlines, checked replacement, native controls and fresh-process byte equality |
| Require independent complete media and pixel expectations | Equal renderers or a successful exit alone cannot prove restoration | ✓ Phase 7: two fresh launches, both counters, all34816 bytes and oracle faults verified |
| Freeze Phase 5 at existing 15/09; tighten scoped additions to actual size (D-35) | Stop repeated budget/replanning cycles while preserving every required check | ✓ Nine active outcomes accepted; actual 15287, 78 retired; no plan16 or allowance reset |
| Keep conservative release rehearsals and advisory portability lanes (D-31) | Reuse needs independently proven input identity; lane promotion needs qualifying main history | ✓ Genuine current rehearsal accepted; no reuse state or unearned promotion |
| Tooling review findings remain seeds (AGENTS rule 7) | Bound phase closure without hiding known limitations | ✓ WR-01/COST-01 deferred explicitly; no trusted raw monthly cost claim |
| Time in MARIA clock ticks; the CPU drives the bus; MARIA DMA runs as one exact burst per line | Every documented edge is an integer; cheaper than per-tick stepping on low-power targets | ✓ v1.0: 4/6-tick CPU cycles, holds and queued NMI integrate with exact MARIA bursts and diagnostic timing |
| Integer-only core, no globals, bounded run calls | Bit-identical hashes everywhere; hostile ROMs cannot hang a host | ✓ Phase 1: tests enforce no floating point, no globals and the no-FP lanes; Phase 2: a step on a jammed CPU still makes one bus read, so emulated time keeps moving |
| Native MARIA byte per pixel; palettes and composite filter outside the hash | Hash stability; hosts choose presentation | ✓ Phase 1: frames are colour numbers; the runner and the libretro core apply the palette |
| Audio hashed as a transition stream; PCM secondary | Determinism independent of the mixer and output rate | ✓ Phase 4: canonical11-byte raw records and LE mono PCM have separate completed-run digests; fixed identities and half-open ownership |
| Reuse the owner's nesturbator code; MARIA, TIA, RIOT, POKEY and boards written from documentation and cross-checked | Fastest correct path; existing permissive chip models are lumped or diverge from documentation | ✓ v1.0: seeded SALLY passes all vectors; documented RIOT, MARIA, TIA, POKEY and linear/SuperGame implementations ship |
| Fast boot by default; CC0 OpenBIOS for the BIOS path in CI; no Atari bytes | Redistributable and legal | ✓ v1.0: fast/OpenBIOS boot and firmware scans run in CI; release artifacts contain no BIOS |
| Tests on licence-checked inputs: public 6502 vectors, 7800basic samples built in CI, own diagnostic ROMs | Reproducible without a network or private ROMs | ✓ Phase 2: the SingleStepTests sample runs on every PR and the full set nightly; 7800basic's `simple` sample builds byte-identical on all nine legs |
| One compatibility ledger in which a pass stays a pass | A regression is a one-line diff | ✓ Phase 3: the ledger, ratchet and generated compatibility page run in CI |
| One entrypoint: CMake workflow presets; one required CI check | No script to keep in step with CI | ✓ Phase 1: CI runs the presets on six platforms behind `CI required` |
| release-please with a GitHub App token; version-free asset names; unsigned macOS artifacts installed by script | Automatic releases with no owner step | ✓ v1.0: package v0.4.0 published automatically and install-line-latest passed, run 39533130210 |
| With no cartridge loaded the core outputs a built-in test card | A build or a release can be checked without any ROM | ✓ Phase 1: the card's hash is pinned and RetroArch shows it pixel for pixel |
| Linear and SuperGame boards first, then POKEY | 187 of 204 known titles; 195 with POKEY | ✓ Phase 3 boards; Phase 4 declared primary/secondary POKEY maps, clocking and accepted-v4 timer IRQs |
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
*Last updated: 2026-10-29 after Phase 11 acceptance and verification; ship Phase 11 next*
