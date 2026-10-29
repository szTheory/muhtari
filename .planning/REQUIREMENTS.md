# Requirements: muhtari v2.0 — The whole library

Defined and approved: 2026-10-21. Milestone v2.0; Phase 5 shipped in PR #20. BOARD-01–03 implementation acceptance is recorded in Phase 6 summaries; phases 7–11 are not started.
Core value: 7800 games behave as they do on the console, identically on every platform, from a small MIT-licensed C library that any host can embed.

## Milestone requirements

### Pipeline tune-up

- [x] **TUNE-01**: Maintainers receive a weekly triage digest with a monthly CI-efficiency section derived from hosted runs, without a new service dependency.
- [x] **TUNE-02**: CI uses measured timeout and workload bounds, resolves reproduced flakes and overlapping-output races, and records the stronger signal for any removed check.
- [x] **TUNE-03**: One aggregate quality.json ratchet consumes measured existing outputs, rejects unexplained regressions and permits explicit dated exceptions for its metrics; performance and compatibility retain their independent semantic guards and existing domain exception stores. Repository checks preserve concrete contracts and use the bounded architecture review in ENGINEERING §11; maintenance-line allocations and their exclusive accounting machinery are retired by the accepted 2026-10-23 policy revision.
- [x] **TUNE-04**: Performance comparisons are armed only after the documented deterministic controls pass; the resulting state and supported/advisory portability lanes are explicit.
- [x] **TUNE-05**: User and maintainer docs, install instructions and repository-setting checks match the released behavior; the measured weakest quality dimension receives a shipped improvement.

### Cartridge boards and expansion memory

- [x] **BOARD-01**: Activision cartridges run with correct banking and equivalent mapped behavior for the two identified dump layouts, while raw image identity remains inspectable.
- [x] **BOARD-02**: Absolute cartridges run with documented fixed/switched windows and power-on banks.
- [x] **BOARD-03**: Fractalus cartridges run with the documented mirrored physical RAM and bounded alias accesses.
- [x] **BOARD-04**: BankSet cartridges select the documented CPU/MARIA ROM and RAM views at both HALT transitions, with supported image geometries validated before load changes state.
- [x] **BOARD-05**: XBoard and the XM RAM/POKEY/HSC subset run with documented control registers and CPU/MARIA banking; unsupported sound expansions remain explicit.

Souper/BupChip stays deferred by owner decision on 2026-10-21; no partial board support is counted as complete game support.

### Battery saves

- [x] **SAVE-01**: HSC-compatible software can use its 2 KB SRAM and a validated user-supplied 4 KB ROM; the ROM-free signature stub supports direct-SRAM callers with its firmware limitation documented.
- [x] **SAVE-02**: SaveKey and AtariVox EEPROM users can read/write the 32 KB device through pin-level I2C, including repeated START, page wrap, sequential reads and bounded busy polling.
- [x] **SAVE-03**: Embedders receive a load-stable battery span with layout version 1 and a generation counter reflecting actual byte changes; power cycle retains battery bytes.
- [x] **SAVE-04**: The runner reloads per-game raw .sav data from --save-dir, pads short media with FF, rejects oversized data, and never silently discards a load error.
- [x] **SAVE-05**: Runner periodic, on-demand and orderly-exit flushing atomically replaces saves and preserves the previous valid file when persistence fails.
- [x] **SAVE-06**: RetroArch retains HSC/SaveKey bytes through SAVE_RAM across fresh launches, proven by the bounded second hosted save-round-trip case; raw .srm and runner .sav media agree.

### Controllers

- [x] **INPUT-01**: Hosts can select header defaults or explicit device overrides and supply the new device inputs through append-only API, runner replay and applicable libretro abstractions without changing old caller behavior.
- [x] **INPUT-02**: XG-1 software sees trigger and brightness-sensitive beam-timed sensor pins, with dark/off-screen/blanking controls and fitted latency parameters identified.
- [x] **INPUT-03**: Paddle and keypad software sees documented buttons, pot dump/charge and matrix behavior at access time, with fitted analog timing identified.
- [x] **INPUT-04**: Driving, CX22 trak-ball, ST mouse and Amiga mouse software sees their documented quadrature sequences and buttons, with bounded delta handling.
- [x] **INPUT-05**: SNES2Atari and Mega7800 software sees documented serial/select handshakes and button states through RIOT output/DDR pin transitions.

### PAL

- [x] **PAL-01**: PAL content and an explicit PAL override use 313-line timing, PAL MSTAT/DMA/frame boundaries and documented fast/BIOS boot hand-off rather than NTSC defaults.
- [x] **PAL-02**: Library, runner and libretro expose the correct PAL geometry, rational frame rate, palette and aspect information.
- [x] **PAL-03**: Sustained PAL runs keep transition timestamps and PCM sample carry consistent with the PAL clock, with cross-platform frame/audio hashes.

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
| TUNE-01 | Phase 5 | Complete |
| TUNE-02 | Phase 5 | Complete |
| TUNE-03 | Phase 5 | Complete |
| TUNE-04 | Phase 5 | Complete |
| TUNE-05 | Phase 5 | Complete |
| BOARD-01 | Phase 6 | Complete |
| BOARD-02 | Phase 6 | Complete |
| BOARD-03 | Phase 6 | Complete |
| SAVE-01 | Phase 7 | Complete |
| SAVE-02 | Phase 7 | Complete |
| SAVE-03 | Phase 7 | Complete |
| SAVE-04 | Phase 7 | Complete |
| SAVE-05 | Phase 7 | Complete |
| SAVE-06 | Phase 7 | Complete |
| INPUT-01 | Phase 8 | Complete |
| INPUT-02 | Phase 8 | Complete |
| INPUT-03 | Phase 8 | Complete |
| INPUT-04 | Phase 8 | Complete |
| INPUT-05 | Phase 8 | Complete |
| PAL-01 | Phase 9 | Complete |
| PAL-02 | Phase 9 | Complete |
| PAL-03 | Phase 9 | Complete |
| BOARD-04 | Phase 10 | Complete |
| BOARD-05 | Phase 10 | Complete |
| COMP-01 | Phase 11 | Gaps Found |
| COMP-02 | Phase 11 | Gaps Found |
| COMP-03 | Phase 11 | Gaps Found |

Coverage: 27 approved requirements;27 mapped;0 unmapped. Phases5–9 verify22 complete requirements;5 remain pending in Phases10–11. Phase9 verifies PAL-01–03 with42/42 truths, source-fresh native/PPC and all-host execution, retained NTSC predicates and authenticated package evidence. See [Phase9 verification](phases/09-pal-games-and-audio/09-VERIFICATION.md). Scope boundaries and historical policy revisions below remain unchanged.
Phase 5 merged at 6d57d69; ci-only work cuts no package release. See [Phase 5 verification](phases/05-pipeline-tune-up/05-VERIFICATION.md) for commands, exit statuses
and run URLs. WR-01 and COST-01 remain deferred tooling seeds under AGENTS rule 7; raw monthly
cost totals are untrusted, and no global hosted-cost reduction is claimed.

Historical 2026-10-21 policy revision: the owner delegated adoption of recommendations after refreshed Phase 5 research.
TUNE-03 replaces the hard tenth-of-source ceiling with the absolute ratchet specified in
`.planning/phases/05-pipeline-tune-up/05-CONTEXT.md` D-17, D-19–20. All five pipeline outcomes remain required;
this is an explicit budget-contract change, not a claim that the old limit was met or proved impossible.

Historical 2026-10-22 scope revision: after two measured migration halts, the owner delegated recommendation adoption.
TUNE-03 explicitly defers performance/compatibility domain-policy centralization and retirement of their two
legacy stores. It does not promise the central aggregate's immutable grant/from/to/renewal protections for
those legacy domains; a waived compatibility loss can become the next base's nonpass state. Existing semantic
faults remain guarded; TUNE-04 readiness, aggregate controls, fixed allocations and final acceptance remain
required. See Phase 5 CONTEXT D-21–23. This historical scope discussion alone marked no requirement complete; final Phase 5 verification now accepts TUNE-01–05.

2026-10-23 accepted policy revision: the owner accepted the [proportional-gates recommendation](research/proportional-gates/RECOMMENDATION.md)
and authorized its bounded migration. ENGINEERING §11 replaces maintenance LOC ceilings, allocation/admission,
counting and final tightening with concrete contract checks and bounded architecture review. This supersedes
the historical fixed-allocation obligation above and Phase 6's N654 proposal; it does not approve that grant.
Phase 5's accepted completion and its original evidence remain historical facts. Ordinary aggregate bounds,
dated exceptions and independent performance/compatibility authorities remain binding; no BOARD requirement
is completed by this revision.
