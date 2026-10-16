# Requirements: muhtari

**Defined:** 2026-10-15
**Core Value:** 7800 games behave as they do on the console, identically on every platform, from a small
MIT-licensed C library that any host can embed.

## v1 Requirements

Milestone v1.0. Each category is one phase; its pull request builds the release archives, and merging it publishes
them (FRAME-06). A platform is one of six: Linux, macOS and Windows, each on x64 and arm64; "every platform" also
includes the 32-bit big-endian PowerPC lane run under qemu. Output from the libretro core equals the runner's when
it matches after the adapter's fixed conversion: the palette table for pixels, both channels for audio. Expected
values in tests cite their source document.

### A test frame in RetroArch

- [x] **FRAME-01**: `cmake --workflow --preset ci` (`ci-msvc` on Windows) builds the library, `muhtari-run` and the
  libretro core and runs every test on all six platforms; CI runs the same commands, and its one required check,
  `CI required`, passes.
- [x] **FRAME-02**: `ctest -R 'install.consumer|embed.subdirectory'` shows that a C program including only the
  public header creates an instance, runs one frame and destroys it, built against the installed package and as an
  embedded subdirectory; the header also compiles as C++.
- [x] **FRAME-03**: With no cartridge loaded the core outputs a built-in 320×242 test card showing all 256 MARIA
  colours; `muhtari-run --frames 1 --dump-frame 1:FILE --hash-frame 1` writes it as an image and prints its
  SHA-256, and the hash is the same on every platform.
- [x] **FRAME-04**: The libretro core loads in an in-process test host that calls it the way RetroArch does,
  reports 39375000/656711 frames per second and 320×242 with aspect ratio 960/847 (pixels 6:7), and delivers a
  frame equal to the runner's; a hosted macOS job runs RetroArch, pinned by SHA-256, unattended, and its
  screenshot equals the runner's frame.
- [x] **FRAME-05**: The `asan`, `nofp` and `hygiene` presets pass: no sanitizer finding; no floating point, no
  mutable global and no undefined symbol beyond the C memory functions in the core; no personal data, no unlisted
  binary and no GPL text in the tree.
- [x] **FRAME-06**: Every pull request builds the 18 archives (library, runner and libretro core for each
  platform), each holding `LICENSE` and `THIRD-PARTY-NOTICES.md`; a behaviour-changing merge to `main` publishes
  them with `SHA256SUMS` and verified attestations as a GitHub release, through a release pull request the
  release App merges, with no manual step; a rehearsal run exercised every publish step before the first tag.
  Uses the release App `muhtari-release` and its secrets, set up 2026-10-15 (GSD-HANDOFF 7).
- [x] **FRAME-07**: The README's one-line install, run against the latest release's version-free archive, puts the
  core and its `.info` into RetroArch on an Apple Silicon Mac; a test runs that exact line against the built
  archive on macOS arm64.
- [x] **FRAME-08**: `scripts/apply-repo-settings.sh` applies the repository settings and the committed ruleset;
  pull-request titles are checked as Conventional Commits; `ctest -R policy.dependabot` shows the Dependabot
  auto-merge condition on synthetic events; the instructions-per-frame gate compares every pull request with its
  merge base and, once armed by two zero-spread control runs, fails one that is more than 1 % slower.
- [x] **FRAME-09**: `ctest -L perf` shows zero allocations after load; the nightly `muhtari-run --bench` records
  frames per second, p50 to p99.999 and maximum microseconds per frame over at least 300,000 frames, and maximum
  RSS, as trend data.

### The CPU matches the public vectors

- [ ] **CPU-01**: `ctest -L vectors` runs the committed sample of the SingleStepTests `6502` set (the first 100
  tests of every opcode, at its pinned commit) and every opcode matches on final state and on every bus cycle,
  decimal mode included, with the unstable-opcode constant set to the suite's value.
- [ ] **CPU-02**: `ctest -L vectors-full` runs the full set, fetched at its pinned commit, with every test matching;
  CI runs it nightly.
- [ ] **CPU-03**: `ctest -L sally` shows that a bus cycle lasts 4 ticks, or 6 when it touches TIA or RIOT space;
  that HALT reaches the CPU only after two INPTCTRL writes and stops read and write cycles alike; that RDY holds
  only read cycles, so a read-modify-write completes both writes; and that an NMI raised in an instruction's last
  cycle is taken after the next instruction.
- [ ] **CPU-04**: `ctest -L riot` shows the RIOT timer counting through HALT and WSYNC at each interval, its
  interrupt flag, SWCHA and SWCHB with their direction registers and profile power-on values, and RIOT RAM at
  $0480–$04FF; INPTCTRL locks after its lock write; reads of write-only or unmapped addresses return the open-bus
  byte.
- [ ] **CPU-05**: `muhtari-run --probe FILE` prints the A78 v3/v4 fields, board, region, peripherals and the
  headered and headerless SHA-1, SHA-256, MD5 and CRC-32, and identifies a header-stripped image through the hash
  list (shown with a 7800basic sample and a test list; real dumps use the bundled CC0 list); a 2600 image, an
  unsupported board, and a malformed, truncated or oversized file each get a distinct status and message; a
  counting-allocator test shows a rejected file allocates nothing; `ctest -R fuzz.regress.loader` replays the
  loader corpus on every platform and its fuzz target runs nightly.

### Games play in RetroArch

- [ ] **GAME-01**: `ctest -L basic` builds the pinned 7800basic samples, checks each ROM's SHA-256, and runs each for
  600 frames; every frame hash equals its pinned value on every platform. Together they cover linear 16–48 KB and
  SuperGame boards (with RAM, bank 6 and 144 KB variants), every read mode, kangaroo mode, holey and indirect DMA,
  and display-list interrupts.
- [ ] **GAME-02**: `ctest -L diag` runs the project's own diagnostic ROMs; the DMA cost per line by header and byte
  counts, the overrun abort, the DLI tick, the WSYNC release and the slow-access cost they report equal the values
  in A7800-MARIA and A7800-SYSTEM, as do the per-line DMA statistics the core keeps.
- [ ] **GAME-03**: The default fast boot, with no BIOS, starts a cartridge in the documented hand-off state;
  `--boot bios` boots through the CC0 OpenBIOS built in CI; every sample reaches its title-screen predicate within
  300 frames on both paths.
- [ ] **GAME-04**: `muhtari-run --movie FILE` replays per-frame input that drives a two-button ProLine stick and a
  one-button stick on both ports, plus Pause, Select, Reset and both difficulty switches; `ctest -L input` shows
  the SWCHA, SWCHB and INPT0–5 values the game reads match A7800-CARTRIDGES-AND-PERIPHERALS, and frames with no
  input read are counted as lag.
- [ ] **GAME-05**: `ctest -R runner.boot_to_play` shows one 7800basic game played from boot through the runner — a
  control changes play and the picture moves — and fails its negative controls; the libretro core's memory
  descriptors pass rcheevos' own resolver; the hosted RetroArch job boots a diagnostic ROM with a screenshot equal
  to the runner's frame.
- [ ] **GAME-06**: `ctest -R fuzz.regress.run` replays the random-ROM corpus without a crash, sanitizer finding or
  timeout on every platform, and its fuzz target runs nightly; `ctest -L prop` runs the property tests (split runs
  equal one run, peeking changes nothing, two instances never interact, input applies only at VBLANK start) with
  100 seeded cases per pull request and 10,000 nightly; the `library` preset runs in CI over the built samples and
  regenerates `docs/compatibility.md` — the owner's run over personal dumps is optional and never a verification
  item.

### Sound

- [ ] **SND-01**: `ctest -L tia` steps every AUDC setting through the period and bit pattern A7800-AUDIO tabulates,
  on the TIA's own 456-tick lattice, with TIA writes ungated by INPTCTRL.
- [ ] **SND-02**: `ctest -L pokey` checks POKEY at $4000, $0450 and $0800 as the header or hash list places it:
  16-bit joins, 1.79 MHz mode, two-tone with the high-pass filter, the polynomial counters, RANDOM, SKCTL
  initialisation, and its clock stretching on slow cycles.
- [ ] **SND-03**: `muhtari-run --hash-audio` prints the transition-stream and PCM hashes for the 7800basic sound
  and POKEY samples, identical on every platform, and no sample clips.
- [ ] **SND-04**: The libretro batch and single-sample paths deliver stereo-duplicated PCM equal to the runner's,
  and `runner.boot_to_play` also requires non-silent audio.
- [ ] **SND-05**: A test renders reference tones through the synthesiser and shows the largest non-harmonic peak
  below 16 kHz is under −80 dB.

## v2 Requirements

Later milestones. Each has a seed in `.planning/seeds/` that says when it comes back.

### The whole library (SEED-001)

- **LIB-01**: Every board in MAME's CC0 list loads and plays, including Activision (both image layouts),
  Absolute, Fractalus, BankSet, Souper and EXRAM.
- **LIB-02**: The XG-1 light gun, paddles, driving controller, keypad, trak-ball, mice, SNES2Atari and Mega7800
  pads work at pin level in the runner and RetroArch.
- **LIB-03**: High Score Cartridge and SaveKey saves persist across runs through the runner's save directory and
  RetroArch's `.srm`, proven by a hosted save round trip.
- **LIB-04**: PAL games run at PAL timing with PAL colours and hand-off state.
- **LIB-05**: The compatibility ledger covers every entry of the CC0 list.

### States and movies (SEED-002)

- **STATE-01**: Save states, rewind and run-ahead work in RetroArch under a deterministic `.info` claim.
- **STATE-02**: `muhtari-run --record FILE` writes a movie that `--movie` replays identically on every platform.
- **STATE-03**: The runner honours Playstead's continuation protocol (qualify, initial-run, continue).

### Measured accuracy (SEED-003)

- **ACC-01**: An FPGA-RTL oracle, the nightly a7800 oracle and owner-console diagnostics confirm or refit every
  fitted timing constant.
- **ACC-02**: The XM expansion (YM2151, RAM, POKEY) plays.

### Tools (SEED-004)

- **TOOL-01**: Hooks, bounded runs, breakpoints, traces, display-list and DMA views and symbols serve homebrew
  developers, with hooks-off cost unchanged.
- **TOOL-02**: Python bindings and a vectorised Gymnasium-style environment serve AI agents.

### Everywhere (SEED-005), the browser (SEED-008) and 2600 mode (SEED-006)

- **PORT-01**: Console and handheld builds with measured frame-time and battery budgets; libretro buildbot;
  Playstead integration.
- **WEB-01**: An efficient WebAssembly build plays in the browser with sound, with frame and audio hashes equal to
  native.
- **A2600-01**: Atari 2600 cartridges run in 7800 mode.

## Out of Scope

| Feature | Reason |
|---------|--------|
| GUI, window, audio or input devices | The host provides them (RetroArch now, Playstead later) |
| Bundled BIOS, HSC ROM, game ROMs | Copyright; players supply their own dumps |
| GPL/LGPL code | MIT project; GPL emulators are oracles only |
| A fast/low-accuracy mode | One accurate core, one set of hashes |
| Signed/notarised macOS builds | Not needed for the script install |

## Traceability

| Requirement | Phase | Status |
|-------------|-------|--------|
| FRAME-01 | Phase 1 | Complete |
| FRAME-02 | Phase 1 | Complete |
| FRAME-03 | Phase 1 | Complete |
| FRAME-04 | Phase 1 | Complete |
| FRAME-05 | Phase 1 | Complete |
| FRAME-06 | Phase 1 | Complete |
| FRAME-07 | Phase 1 | Complete |
| FRAME-08 | Phase 1 | Complete |
| FRAME-09 | Phase 1 | Complete |
| CPU-01 | Phase 2 | Pending |
| CPU-02 | Phase 2 | Pending |
| CPU-03 | Phase 2 | Pending |
| CPU-04 | Phase 2 | Pending |
| CPU-05 | Phase 2 | Pending |
| GAME-01 | Phase 3 | Pending |
| GAME-02 | Phase 3 | Pending |
| GAME-03 | Phase 3 | Pending |
| GAME-04 | Phase 3 | Pending |
| GAME-05 | Phase 3 | Pending |
| GAME-06 | Phase 3 | Pending |
| SND-01 | Phase 4 | Pending |
| SND-02 | Phase 4 | Pending |
| SND-03 | Phase 4 | Pending |
| SND-04 | Phase 4 | Pending |
| SND-05 | Phase 4 | Pending |

**Coverage:** v1 requirements: 25 total; 25 mapped to phases; 0 unmapped.

---
*Requirements defined: 2026-10-15*
*Last updated: 2026-10-15 after roadmap creation and the release App set-up*
