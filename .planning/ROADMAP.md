# Roadmap: muhtari

## Overview

muhtari starts by shipping the whole delivery path: a C library, the `muhtari-run` runner and a libretro core that
show a built-in test card, built and tested on six platforms plus the big-endian lane, and published as an
attested release that installs into RetroArch with one line. Every later phase then ships by merging. The SALLY
CPU, its 7800 bus timing, RIOT and the cartridge loader come next, judged by public vectors. MARIA, the linear and
SuperGame boards, both boot paths and controller input follow, so 7800 games play in RetroArch. TIA and POKEY sound
close the first release line. Later work (every board and peripheral, states and movies, measured accuracy, tools,
ports, 2600 mode) is planned in `.planning/PRODUCT-ROADMAP.md` and `.planning/seeds/`.

## v1.0 7800 games play with sound in RetroArch

Four phases. Each phase is one branch and one pull request into a green `main`; merging it publishes a release
with no manual step.

## Phases

**Phase Numbering:**
- Integer phases (1, 2, 3): Planned work
- Decimal phases (2.1, 2.2): Urgent insertions (marked with INSERTED)

Decimal phases appear between their surrounding integers in numeric order.

- [ ] **Phase 1: A test frame in RetroArch** - Library, runner and libretro core show a built-in test card; CI on every platform; the first automatic attested release installs into RetroArch with one line
- [ ] **Phase 2: The CPU matches the public vectors** - SALLY matches the public 6502 vectors on every cycle, with the 7800 bus timing, RIOT, INPTCTRL and the cartridge probe and loader
- [ ] **Phase 3: Games play in RetroArch** - MARIA with exact DMA timing, linear and SuperGame boards, fast and OpenBIOS boot, joysticks and console switches make 7800 games playable
- [ ] **Phase 4: Sound** - TIA and POKEY sound through an integer band-limited synthesiser, with audio hashes identical on every platform

## Phase Details

### Phase 1: A test frame in RetroArch

**Goal**: A downloadable release of the library, the runner and the libretro core shows a built-in test card in RetroArch, built and tested on every platform, and merging to `main` publishes the next release with no manual step
**Depends on**: Nothing (first phase)
**Requirements**: FRAME-01, FRAME-02, FRAME-03, FRAME-04, FRAME-05, FRAME-06, FRAME-07, FRAME-08, FRAME-09
**Success Criteria** (what must be TRUE):
  1. `cmake --workflow --preset ci` (`ci-msvc` on Windows) builds the library, `muhtari-run` and the libretro core and passes every test on all six platforms, CI runs the same commands behind the one required check `CI required`, `ctest -R 'install.consumer|embed.subdirectory'` passes for the installed package and the embedded subdirectory, and the `asan`, `nofp` and `hygiene` presets pass.
  2. With no cartridge loaded, `muhtari-run --frames 1 --dump-frame 1:FILE --hash-frame 1` writes the 320×242 test card with all 256 MARIA colours and prints a SHA-256 that is the same on every platform; the in-process libretro test host sees 39375000/656711 frames per second, 320×242 at aspect 960/847 and a frame equal to the runner's, and the hosted macOS RetroArch job's screenshot equals the runner's frame.
  3. Merging the phase's pull request publishes a GitHub release of the 18 archives, each holding `LICENSE` and `THIRD-PARTY-NOTICES.md`, with `SHA256SUMS` and verified attestations, through a release pull request the release App merges, after a rehearsal run exercised every publish step.
  4. The README's one-line install, run against the latest release's version-free archive, puts the core and its `.info` into RetroArch on an Apple Silicon Mac, and a CI test runs that exact line against the built archive on macOS arm64.
  5. `scripts/apply-repo-settings.sh` applies the settings and the committed ruleset, pull-request titles are checked as Conventional Commits, `ctest -R policy.dependabot` and `ctest -L perf` pass, the instructions-per-frame gate compares each pull request with its merge base, and the nightly `muhtari-run --bench` records its trend data.

**Plans**: 14/21 plans executed (01–13 local; 14–21 need GitHub runs)

Plans:
**Wave 1**
- [x] 01-01-PLAN.md — Tracer: header, core and runner print the pinned hash; presets and the version source

**Wave 2** *(blocked on Wave 1 completion)*
- [x] 01-02-PLAN.md — Exact-inventory test harness; the core's full 0.1.0 contract under test

**Wave 3** *(blocked on Wave 2 completion)*
- [x] 01-03-PLAN.md — Negative controls, the no-skip rule and the JUnit completeness check

**Wave 4** *(blocked on Wave 3 completion)*
- [x] 01-04-PLAN.md — Warm palette seed with its notice and `host.palette`

**Wave 5** *(blocked on Wave 4 completion)*
- [x] 01-05-PLAN.md — PNG writer and `--dump-frame`, with pinned PNG bytes

**Wave 6** *(blocked on Wave 5 completion)*
- [x] 01-06-PLAN.md — Runner bench, stop, exit table, usage; `docs/runner.md`

**Wave 7** *(blocked on Wave 6 completion)*
- [x] 01-07-PLAN.md — libretro core, vendored header, `.info`, in-process host in both pixel formats

**Wave 8** *(blocked on Wave 7 completion)*
- [x] 01-08-PLAN.md — Install, consumer, embedded subdirectory, header as C and C++

**Wave 9** *(blocked on Wave 8 completion)*
- [x] 01-09-PLAN.md — Archives, the single version source at the next release version, README install line, docs

**Wave 10** *(blocked on Wave 9 completion)*
- [x] 01-10-PLAN.md — ABI gates, `abi.api_version`, `release.floor`

**Wave 11** *(blocked on Wave 10 completion)*
- [x] 01-11-PLAN.md — The `hygiene` lane with self-locating tools; the notices policy

**Wave 12** *(blocked on Wave 11 completion)*
- [x] 01-12-PLAN.md — Pins and fetch script; Linux `ci`, `asan`, `nofp` in containers

**Wave 13** *(blocked on Wave 12 completion)*
- [x] 01-13-PLAN.md — `be32` and wasm32 lanes; four-lane local hash equality

**Wave 14** *(blocked on Wave 13 completion)*
- [x] 01-14-PLAN.md — `ci.yml` on six platforms behind `CI required`, via a draft probe PR

**Wave 15** *(blocked on Wave 14 completion)*
- [ ] 01-15-PLAN.md — Title check, CI policy tests, ENGINEERING amendment, wasm32 todos

**Wave 16** *(blocked on Wave 15 completion)*
- [ ] 01-16-PLAN.md — Hosted RetroArch job with pixel-exact compare

**Wave 17** *(blocked on Wave 16 completion)*
- [ ] 01-17-PLAN.md — Publish script and the published-and-deleted rehearsal on the pull request

**Wave 18** *(blocked on Wave 17 completion)*
- [ ] 01-18-PLAN.md — Real release path on `main`; DEC.49 and ENGINEERING §5 records; release runbook

**Wave 19** *(blocked on Wave 18 completion)*
- [ ] 01-19-PLAN.md — Instructions-per-frame gate (report-only) and nightly bench

**Wave 20** *(blocked on Wave 19 completion)*
- [ ] 01-20-PLAN.md — Settings `--check`, committed ruleset, Dependabot

**Wave 21** *(blocked on Wave 20 completion)*
- [ ] 01-21-PLAN.md — Measured timeouts, owner applies settings, probe PR closed, `feat:` ship title recorded

**Cross-cutting constraints:**
- release.yml's concurrency group has no cancel-in-progress and uses queue: max

**UI hint**: no
**Canonical refs:** `.planning/preparation/ENGINEERING.md`, `.planning/preparation/HOSTS-AND-INTEGRATION.md`, `.planning/preparation/ARCHITECTURE.md`, `PROVENANCE.md`

### Phase 2: The CPU matches the public vectors

**Goal**: The SALLY CPU matches the public 6502 vectors on every opcode and every bus cycle, the bus applies the 7800's slow cycles, HALT and RDY, RIOT and INPTCTRL behave as documented, and the runner probes and loads cartridge images safely; merging publishes the release
**Depends on**: Phase 1
**Requirements**: CPU-01, CPU-02, CPU-03, CPU-04, CPU-05
**Success Criteria** (what must be TRUE):
  1. `ctest -L vectors` matches every opcode of the committed SingleStepTests `6502` sample on final state and every bus cycle, decimal mode included, on every pull request, and `ctest -L vectors-full` matches the full set at its pinned commit in the nightly run.
  2. `ctest -L sally` passes: 4-tick and 6-tick bus cycles, HALT only after two INPTCTRL writes and stopping reads and writes, RDY holding only reads so a read-modify-write completes both writes, and an NMI raised in the last cycle taken after the next instruction.
  3. `ctest -L riot` passes: the RIOT timer counting through HALT and WSYNC at each interval, its interrupt flag, SWCHA and SWCHB with direction registers and power-on values, RIOT RAM at $0480–$04FF, INPTCTRL locking, and open-bus reads.
  4. `muhtari-run --probe FILE` prints the A78 fields, board, region, peripherals and all four hashes, identifies a header-stripped image through the hash list, and gives a 2600 image, an unsupported board and a malformed, truncated or oversized file each a distinct status and message; the counting-allocator test and `ctest -R fuzz.regress.loader` pass on every platform and the loader fuzz target runs nightly.
  5. Merging the phase's pull request publishes a release through the Phase 1 pipeline with no manual step.

**Plans**: TBD
**UI hint**: no
**Canonical refs:** `.planning/preparation/A7800-SYSTEM.md`, `.planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md`, `.planning/preparation/ARCHITECTURE.md`, `.planning/preparation/CONFORMANCE.md`

### Phase 3: Games play in RetroArch

**Goal**: Linear and SuperGame cartridges render with exact MARIA DMA timing, boot by the fast path or OpenBIOS, take joystick and console-switch input, and play in RetroArch with frame hashes identical on every platform; merging publishes the release
**Depends on**: Phase 2
**Requirements**: GAME-01, GAME-02, GAME-03, GAME-04, GAME-05, GAME-06
**Success Criteria** (what must be TRUE):
  1. `ctest -L basic` builds the pinned 7800basic samples, checks their SHA-256 and matches every pinned frame hash over 600 frames on every platform, covering the linear and SuperGame variants, every read mode, kangaroo mode, holey and indirect DMA and display-list interrupts; `ctest -L diag` reports DMA costs, the overrun abort, the DLI tick, the WSYNC release and the slow-access cost equal to A7800-MARIA and A7800-SYSTEM.
  2. Every sample reaches its title-screen predicate within 300 frames both by the default fast boot with no BIOS and by `--boot bios` through the CC0 OpenBIOS built in CI.
  3. `muhtari-run --movie FILE` drives both stick types on both ports and every console switch, `ctest -L input` shows the values the game reads match A7800-CARTRIDGES-AND-PERIPHERALS and counts lag frames, and `ctest -R runner.boot_to_play` plays one game from boot with a control changing play and fails its negative controls.
  4. The libretro core's memory descriptors pass rcheevos' own resolver, and the hosted RetroArch job boots a diagnostic ROM with a screenshot equal to the runner's frame.
  5. `ctest -R fuzz.regress.run` and `ctest -L prop` pass on every platform with their nightly runs, the `library` preset regenerates `docs/compatibility.md` in CI, and merging the phase's pull request publishes a release with no manual step.

**Plans**: TBD
**UI hint**: no
**Canonical refs:** `.planning/preparation/A7800-MARIA.md`, `.planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md`, `.planning/preparation/A7800-SYSTEM.md`, `.planning/preparation/CONFORMANCE.md`, `.planning/preparation/HOSTS-AND-INTEGRATION.md`

### Phase 4: Sound

**Goal**: Games play with TIA and POKEY sound in RetroArch, with transition-stream and PCM hashes identical on every platform; merging publishes the release
**Depends on**: Phase 3
**Requirements**: SND-01, SND-02, SND-03, SND-04, SND-05
**Success Criteria** (what must be TRUE):
  1. `ctest -L tia` steps every AUDC setting through the tabulated period and bit pattern on the 456-tick lattice with TIA writes ungated by INPTCTRL, and `ctest -L pokey` passes for POKEY at $4000, $0450 and $0800 as the header or hash list places it.
  2. `muhtari-run --hash-audio` prints transition-stream and PCM hashes for the 7800basic sound and POKEY samples that are identical on every platform, and no sample clips.
  3. The libretro batch and single-sample paths deliver stereo-duplicated PCM equal to the runner's, and `runner.boot_to_play` requires non-silent audio.
  4. A test renders reference tones through the synthesiser and shows the largest non-harmonic peak below 16 kHz under −80 dB.
  5. Merging the phase's pull request publishes a release with no manual step.

**Plans**: TBD
**UI hint**: no
**Canonical refs:** `.planning/preparation/A7800-AUDIO.md`, `.planning/preparation/ARCHITECTURE.md`, `.planning/preparation/CONFORMANCE.md`

## Progress

**Execution Order:**
Phases execute in numeric order: 1 → 2 → 3 → 4

| Phase | Plans Complete | Status | Completed |
|-------|----------------|--------|-----------|
| 1. A test frame in RetroArch | 14/21 | In Progress | - |
| 2. The CPU matches the public vectors | 0/TBD | Not started | - |
| 3. Games play in RetroArch | 0/TBD | Not started | - |
| 4. Sound | 0/TBD | Not started | - |
