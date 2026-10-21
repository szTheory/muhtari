# Requirements: muhtari v2.0 — The whole library

Defined and approved: 2026-10-21. Milestone v2.0; implementation not started.
Core value: 7800 games behave as they do on the console, identically on every platform, from a small MIT-licensed C library that any host can embed.

## Milestone requirements

### Pipeline tune-up

- [ ] **TUNE-01**: Maintainers receive a weekly triage digest with a monthly CI-efficiency section derived from hosted runs, without a new service dependency.
- [ ] **TUNE-02**: CI uses measured timeout and workload bounds, resolves reproduced flakes and overlapping-output races, and records the stronger signal for any removed check.
- [ ] **TUNE-03**: A single quality.json ratchet consumes measured existing outputs, rejects unexplained regressions, and permits explicit dated exceptions; repository-check code meets the tenth-of-src budget without hiding its count.
- [ ] **TUNE-04**: Performance comparisons are armed only after the documented deterministic controls pass; the resulting state and supported/advisory portability lanes are explicit.
- [ ] **TUNE-05**: User and maintainer docs, install instructions and repository-setting checks match the released behavior; the measured weakest quality dimension receives a shipped improvement.

### Cartridge boards and expansion memory

- [ ] **BOARD-01**: Activision cartridges run with correct banking and equivalent mapped behavior for the two identified dump layouts, while raw image identity remains inspectable.
- [ ] **BOARD-02**: Absolute cartridges run with documented fixed/switched windows and power-on banks.
- [ ] **BOARD-03**: Fractalus cartridges run with the documented mirrored physical RAM and bounded alias accesses.
- [ ] **BOARD-04**: BankSet cartridges select the documented CPU/MARIA ROM and RAM views at both HALT transitions, with supported image geometries validated before load changes state.
- [ ] **BOARD-05**: XBoard and the XM RAM/POKEY/HSC subset run with documented control registers and CPU/MARIA banking; unsupported sound expansions remain explicit.

Souper/BupChip stays deferred by owner decision on 2026-10-21; no partial board support is counted as complete game support.

### Battery saves

- [ ] **SAVE-01**: HSC-compatible software can use its 2 KB SRAM and a validated user-supplied 4 KB ROM; the ROM-free signature stub supports direct-SRAM callers with its firmware limitation documented.
- [ ] **SAVE-02**: SaveKey and AtariVox EEPROM users can read/write the 32 KB device through pin-level I2C, including repeated START, page wrap, sequential reads and bounded busy polling.
- [ ] **SAVE-03**: Embedders receive a load-stable battery span with layout version 1 and a generation counter reflecting actual byte changes; power cycle retains battery bytes.
- [ ] **SAVE-04**: The runner reloads per-game raw .sav data from --save-dir, pads short media with FF, rejects oversized data, and never silently discards a load error.
- [ ] **SAVE-05**: Runner periodic, on-demand and orderly-exit flushing atomically replaces saves and preserves the previous valid file when persistence fails.
- [ ] **SAVE-06**: RetroArch retains HSC/SaveKey bytes through SAVE_RAM across fresh launches, proven by the bounded second hosted save-round-trip case; raw .srm and runner .sav media agree.

### Controllers

- [ ] **INPUT-01**: Hosts can select header defaults or explicit device overrides and supply the new device inputs through append-only API, runner replay and applicable libretro abstractions without changing old caller behavior.
- [ ] **INPUT-02**: XG-1 software sees trigger and brightness-sensitive beam-timed sensor pins, with dark/off-screen/blanking controls and fitted latency parameters identified.
- [ ] **INPUT-03**: Paddle and keypad software sees documented buttons, pot dump/charge and matrix behavior at access time, with fitted analog timing identified.
- [ ] **INPUT-04**: Driving, CX22 trak-ball, ST mouse and Amiga mouse software sees their documented quadrature sequences and buttons, with bounded delta handling.
- [ ] **INPUT-05**: SNES2Atari and Mega7800 software sees documented serial/select handshakes and button states through RIOT output/DDR pin transitions.

### PAL

- [ ] **PAL-01**: PAL content and an explicit PAL override use 313-line timing, PAL MSTAT/DMA/frame boundaries and documented fast/BIOS boot hand-off rather than NTSC defaults.
- [ ] **PAL-02**: Library, runner and libretro expose the correct PAL geometry, rational frame rate, palette and aspect information.
- [ ] **PAL-03**: Sustained PAL runs keep transition timestamps and PCM sample carry consistent with the PAL clock, with cross-platform frame/audio hashes.

### Compatibility and acceptance

- [ ] **COMP-01**: Every entry of the pinned CC0 library is accounted for in the ledger with explicit pass/fail/unknown/unsupported status and evidence; metadata inclusion is never counted as a gameplay pass.
- [ ] **COMP-02**: Each newly claimed board/device/region has legally admissible automated behavioral evidence in public CI, and prior passing predicates remain protected by the existing ratchet.
- [ ] **COMP-03**: Available private cartridges run only through the isolated local library preset with reproducible command/exit/commit evidence, while public CI requires no private bytes or owner verification.

## Deferred

Browser player; state serialization, rewind/run-ahead and movie recording; YM2151, Covox/ADPCM, ACE; full clean-room OpenHSC; AtariVox speech; 2600 mode; ports and developer/AI tooling. Souper/BupChip remains deferred to the later accuracy milestone.

## Constraints inherited

Three deliverables; no device I/O in core; integer-only, instance-owned, bounded and no allocation after load; permissive provenance; no unlicensed emulator source or firmware bytes; behavior changes tested and documented in the same phase; at most two review fix rounds and one gap round; two hosted RetroArch cases total and at most 300 test lines. Existing timing decisions change only with new evidence.

## Traceability

| Requirement | Phase | Status |
|---|---|---|
| TUNE-01 | Phase 5 | Pending |
| TUNE-02 | Phase 5 | Pending |
| TUNE-03 | Phase 5 | Pending |
| TUNE-04 | Phase 5 | Pending |
| TUNE-05 | Phase 5 | Pending |
| BOARD-01 | Phase 6 | Pending |
| BOARD-02 | Phase 6 | Pending |
| BOARD-03 | Phase 6 | Pending |
| SAVE-01 | Phase 7 | Pending |
| SAVE-02 | Phase 7 | Pending |
| SAVE-03 | Phase 7 | Pending |
| SAVE-04 | Phase 7 | Pending |
| SAVE-05 | Phase 7 | Pending |
| SAVE-06 | Phase 7 | Pending |
| INPUT-01 | Phase 8 | Pending |
| INPUT-02 | Phase 8 | Pending |
| INPUT-03 | Phase 8 | Pending |
| INPUT-04 | Phase 8 | Pending |
| INPUT-05 | Phase 8 | Pending |
| PAL-01 | Phase 9 | Pending |
| PAL-02 | Phase 9 | Pending |
| PAL-03 | Phase 9 | Pending |
| BOARD-04 | Phase 10 | Pending |
| BOARD-05 | Phase 10 | Pending |
| COMP-01 | Phase 11 | Pending |
| COMP-02 | Phase 11 | Pending |
| COMP-03 | Phase 11 | Pending |

Coverage: 27 approved requirements; 27 mapped; 0 unmapped. Phase mapping approved on 2026-10-21. Nothing here claims implementation or verification of the new requirements.
