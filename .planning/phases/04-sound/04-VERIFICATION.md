---
phase: 04-sound
verified: 2026-10-21T01:25:24.380779+00:00
status: passed
score: 71/71 execution truths resolved; hosted equality and release outcomes remain at ship
behavior_unverified: 0
human_verification: []
covered_files:
  - .github/workflows/ci.yml
  - .github/workflows/release.yml
  - .planning/REQUIREMENTS.md
  - .planning/config.json
  - .planning/phases/04-sound/04-01-PLAN.md
  - .planning/phases/04-sound/04-01-SUMMARY.md
  - .planning/phases/04-sound/04-02-PLAN.md
  - .planning/phases/04-sound/04-02-SUMMARY.md
  - .planning/phases/04-sound/04-03-PLAN.md
  - .planning/phases/04-sound/04-03-SUMMARY.md
  - .planning/phases/04-sound/04-04-PLAN.md
  - .planning/phases/04-sound/04-04-SUMMARY.md
  - .planning/phases/04-sound/04-05-PLAN.md
  - .planning/phases/04-sound/04-05-SUMMARY.md
  - .planning/phases/04-sound/04-06-PLAN.md
  - .planning/phases/04-sound/04-06-SUMMARY.md
  - .planning/phases/04-sound/04-07-PLAN.md
  - .planning/phases/04-sound/04-07-SUMMARY.md
  - .planning/phases/04-sound/04-08-PLAN.md
  - .planning/phases/04-sound/04-08-SUMMARY.md
  - .planning/phases/04-sound/04-09-PLAN.md
  - .planning/phases/04-sound/04-09-SUMMARY.md
  - .planning/phases/04-sound/04-10-PLAN.md
  - .planning/phases/04-sound/04-10-SUMMARY.md
  - .planning/phases/04-sound/04-11-PLAN.md
  - .planning/phases/04-sound/04-11-SUMMARY.md
  - .planning/phases/04-sound/04-12-PLAN.md
  - .planning/phases/04-sound/04-12-SUMMARY.md
  - .planning/phases/04-sound/04-13-PLAN.md
  - .planning/phases/04-sound/04-13-SUMMARY.md
  - .planning/phases/04-sound/04-14-PLAN.md
  - .planning/phases/04-sound/04-14-SUMMARY.md
  - .planning/phases/04-sound/04-15-PLAN.md
  - .planning/phases/04-sound/04-15-SUMMARY.md
  - .planning/phases/04-sound/04-16-PLAN.md
  - .planning/phases/04-sound/04-16-SUMMARY.md
  - .planning/phases/04-sound/04-17-PLAN.md
  - .planning/phases/04-sound/04-17-SUMMARY.md
  - .planning/phases/04-sound/04-CONTEXT.md
  - .planning/phases/04-sound/04-RESEARCH.md
  - .planning/preparation/A7800-AUDIO.md
  - .planning/preparation/DECISIONS.md
  - ASSET_POLICY.md
  - CMakeLists.txt
  - CMakePresets.json
  - PROVENANCE.md
  - docs/embedding.md
  - docs/emulation-tests.md
  - docs/hardware/audio.md
  - docs/hardware/cartridge.md
  - docs/hardware/pokey.md
  - docs/hardware/sally.md
  - docs/hardware/tia.md
  - docs/libretro.md
  - docs/runner.md
  - docs/testing.md
  - include/muhtari.h
  - libretro/libretro.c
  - runner/main.c
  - src/audio_tables.h
  - src/boot.c
  - src/bus.c
  - src/cart.c
  - src/cpu.c
  - src/frame.c
  - src/instance.c
  - src/internal.h
  - src/pokey.c
  - src/synth.c
  - src/tia.c
  - tests/audio/expected.txt
  - tests/audio/test_contracts.c
  - tests/audio/test_irq.c
  - tests/audio/test_pokey.c
  - tests/audio/test_samples.c
  - tests/audio/test_spectral.c
  - tests/audio/test_stress.c
  - tests/audio/test_tia.c
  - tests/audio/tests.cmake
  - tests/boot/test_boot.c
  - tests/built/expected.txt
  - tests/bus/test_bus.c
  - tests/cmake/hashes.cmake
  - tests/compat/allow.txt
  - tests/compat/ledger.csv
  - tests/core/test_core.c
  - tests/cpu/test_cpu.c
  - tests/cpu/vector_bus.c
  - tests/cpu/vector_bus.h
  - tests/frame/test_frame.c
  - tests/inventory/ci.txt
  - tests/inventory/wasm32.txt
  - tests/libretro/audio.cmake
  - tests/libretro/host.c
  - tests/libretro/tests.cmake
  - tests/runner/boot_to_play.c
  - tests/runner/boot_to_play.cmake
  - tests/runner/test_audio.c
  - tests/runner/tests.cmake
  - tests/tests.cmake
  - tools/audio_coefficients.c
covered_digest: "v3:sha256:84d9ea4c0e5d6c9d83867a9007b02eee6c9f694989b42f9f7f7b322dcc3b636c"
---

# Phase 04: Sound Verification Report

**Goal:** Games play with TIA and POKEY sound through the library, runner and
libretro core, with canonical deterministic evidence feeding every platform
comparison and the established automatic release path.

**Verdict:** Pre-ship execution is verified. All17 plans are summarized and all
five sound requirements have their executable contracts. Hosted platform equality,
PR-head CI and actual merge/release outcomes are expressly reserved for ship by
repository rules6/9 and plan17; this report does not claim those runs occurred.
There is no owner UAT. Verification was performed inline; no independent verifier
agent or external reviewer was used.

## Roadmap Criteria

| Criterion | Execution evidence | Remaining ship confirmation |
| --- | --- | --- |
| TIA conformance and declared POKEY maps | tia.logic, pokey.logic/clock/maps, IRQ and actual bus tests pass | Full hosted lane results |
| Canonical raw/PCM hashes and no clips | Eight600-frame sample rows match actual runner; observer on/off video/PCM, discard/reset, empty/canonical/stop/error controls pass; every sample clips0 and peak≤11555 | Six native plus be32 hashes identical and WASM matches Linux |
| Non-silent libretro and boot-to-play | Actual loaded module's L/R samples equal public-core reference and runner; batch/fallback/missing/reset/reload paths pass; muted gameplay control fails mask8 | Existing RetroArch job on PR head |
| Production spectral rejection | Full production matrix and aliases, actual chip/mixed representatives and owned coefficients pass unchanged judge | No owner listening or new calibration claim |
| Automatic release on merge | Three archives/notices and existing App/release-please/CI/publish path remain wired | Merge, green release PR and published release under rule9 |

## Plan Truth Resolution

Historical plan01's32-phase wording is superseded by the explicit64-phase recovery.
Plan08's anticipated five frame/four title changes were investigated: only pokey's
detection-dependent video/title changed, and other33 chains reproduced unchanged.
Keeping those pins is the required evidence discipline, not an omitted migration.

| Truth | Contract | Resolution | Executed evidence |
| --- | --- | --- | --- |
| 01.1 | Raw level changes produce deterministic mono int16 at exactly 48000/1 Hz and 88/13125 samples per master tick. | Verified | core.audio.silence; audio.frames; ABI/no-FP |
| 01.2 | The complete reconstruction and blocker path has a conservative ≥1 dB headroom bound, including rounding, for the maximum declared hardware. | Verified | core.audio.silence; audio.frames; ABI/no-FP |
| 01.3 | The D-08 amendment uses 512 int32 Q23 coefficients, corrected sum 8388608 and Q23→Q30×128, preserving 16 taps/32 phases/support and hardware-derived gain. | Superseded by D-25–D-30;64 phases verified | core.audio.silence; audio.frames; ABI/no-FP |
| 02.1 | An intentionally aliased production-tone control is rejected; physical aliases are never included in the intended-line mask. | Verified | audio.spectral; audio.spectral.controls |
| 02.2 | All132 Q23 hardware-scaled production-equivalent schedules pass strict−80 dB acceptance and 132/132 same-gain aliased controls are rejected before pins. | Verified | audio.spectral; audio.spectral.controls |
| 02.3 | Hardware/current/headroom equations determine amplitude; actual chip spectra and platform arithmetic remain mandatory production proofs. | Verified | audio.spectral; audio.spectral.controls |
| 03.1 | TIA A phases occur at 0/144 and B at 64/288 of its independent 456-tick epoch; TIA writes reach it regardless of INPTCTRL. | Verified | tia.logic; core.audio.silence; audio.spectral |
| 03.2 | All AUDC modes match sourced periods, ones counts and startup bits, including AUDF lowering/wrap and immediate AUDV writes. | Verified | tia.logic; core.audio.silence; audio.spectral |
| 03.3 | SND-01 adjacency: clock edges due at a register's data tick execute on the old schedule before that write; RSYNC starts future events without duplicating past ones. | Verified | tia.logic; core.audio.silence; audio.spectral |
| 03.4 | SND-01 empty: zero AUDV suppresses output changes while hidden divider/polynomial state continues; known initial zero levels emit no synthetic change. | Verified | tia.logic; core.audio.silence; audio.spectral |
| 03.5 | SND-01 ordering: simultaneous channels are stable in TIA0/TIA1 source order and meaningful same-channel changes keep their order. | Verified | tia.logic; core.audio.silence; audio.spectral |
| 04.1 | ctest -L tia runs every documented AUDC/lattice/write/resync assertion against shipped objects. | Verified | tia.logic; audio.frames; core.audio.silence |
| 04.2 | The fast-boot first interval remains 8190 ticks/54 samples; BIOS first interval remains117174 ticks; subsequent frames retain exact 800/801 cadence. | Verified | tia.logic; audio.frames; core.audio.silence |
| 04.3 | SND-03 adjacency: an event at frame_deadline belongs to the next half-open interval, and CPU instruction/interrupt overshoot is retained without changing CPU/frame timing. | Verified | tia.logic; audio.frames; core.audio.silence |
| 04.4 | Fast boot keeps actual POKEY origin(BOOT_TICK,0), advances hidden TIA over absolute pre-handoff ticks and starts output with audio_rem0. BIOS uses actual origin(0,0), with no fabricated skipped cycles. | Verified | tia.logic; audio.frames; core.audio.silence |
| 05.1 | SND-03 adjacency: an event at frame_deadline belongs to the next half-open interval, and CPU instruction/interrupt overshoot is retained without changing CPU/frame timing. | Verified | audio.frames; audio.observer; runner.audio.stop_failure |
| 05.2 | NULL audio advances chips, history, counts and future PCM exactly like a written buffer; power cycle clears audio machine state while Reset input remains a software switch. | Verified | audio.frames; audio.observer; runner.audio.stop_failure |
| 05.3 | Ordinary invalid calls change no machine/observer state; sticky BIOS refusal is tested as a post-execution exception with audio_count0 and no failed-frame output. | Verified | audio.frames; audio.observer; runner.audio.stop_failure |
| 06.1 | POKEY clocks on actual bus phi2 through fast, slow, HALT and RDY segments; silent channels keep timer/poly/RANDOM state. | Verified | pokey.clock; pokey.logic; audio.spectral; core.audio.silence; coefficient generator --check |
| 06.2 | D-25: Task1 stays complete; execution resumes Task2 from its committed RED and retained GREEN attempt. A blocked SUMMARY never unblocks plans04-07 onward. | Verified | pokey.clock; pokey.logic; audio.spectral; core.audio.silence; coefficient generator --check |
| 06.3 | D-26: The selected64-phase16-tap Q23 reconstruction passes spectra at comparison M18361 and final headroom-derived M18360, preserving current85:342 and Q12 gain4096. | Verified | pokey.clock; pokey.logic; audio.spectral; core.audio.silence; coefficient generator --check |
| 06.4 | D-27: All132 equivalent cases,132 aliased controls and72 actual-chip cases remain checked, including independent arithmetic-schedule PCM equality and compact mixed/HP representatives. | Verified | pokey.clock; pokey.logic; audio.spectral; core.audio.silence; coefficient generator --check |
| 06.5 | D-28: Worst peaks have independent actual-frequency refinement with unchanged measurement/masks; ultrasonic silence is supported by continuing raw transitions and kernel activity. | Verified | pokey.clock; pokey.logic; audio.spectral; core.audio.silence; coefficient generator --check |
| 06.6 | D-29: Owned generation reproduces64x16 rows at Simpson128/256/512, all13125 phases and88 alignments establish fresh headroom/width proofs, and identity becomes exact-q23-16x64-dc20. | Verified | pokey.clock; pokey.logic; audio.spectral; core.audio.silence; coefficient generator --check |
| 06.7 | D-30: Recovery preserves hardware timestamps, bus/CPU behavior, raw identities/serialization, video hashes, API3/ABI,48kHz and logical frame intervals; only synthesis PCM may change. | Verified | pokey.clock; pokey.logic; audio.spectral; core.audio.silence; coefficient generator --check |
| 06.8 | SND-02 adjacency: pipeline/reload and access ties use explicit documented phi2/half-phase ordering and preserve narrow high-pass pulses. | Verified | pokey.clock; pokey.logic; audio.spectral; core.audio.silence; coefficient generator --check |
| 06.9 | SND-02 empty: absent chips leave existing ROM/open-bus behavior; present zero-volume channels still advance hidden state. | Verified | pokey.clock; pokey.logic; audio.spectral; core.audio.silence; coefficient generator --check |
| 06.10 | SND-02 ordering: simultaneous channel edges use stable channel order and documented delayed pipeline ordering. | Verified | pokey.clock; pokey.logic; audio.spectral; core.audio.silence; coefficient generator --check |
| 06.11 | Fast/held rise/fall/completion are s+2/s+4/s+4; slow are s+2/s+5/s+6, with next rise following the actual prior fall. | Verified | pokey.clock; pokey.logic; audio.spectral; core.audio.silence; coefficient generator --check |
| 06.12 | HP delay follows three actual CLK2 half edges:6 fast ticks, up to 9 wholly slow, actual mixed sequence. Only channels 1/3 select the direct fast clock. | Verified | pokey.clock; pokey.logic; audio.spectral; core.audio.silence; coefficient generator --check |
| 07.1 | Joined timers, STIMER/reload/preemption and two-tone resynchronization match the adopted manual values. | Verified | pokey.logic; pokey.clock; audio.spectral |
| 07.2 | High-pass retains its approximately1.5-cycle pulse delay, volume-only overrides it, and channel polynomial bits are staggered. | Verified | pokey.logic; pokey.clock; audio.spectral |
| 07.3 | RANDOM and prescalers obey init/exit semantics while fast timers and hidden counters continue as documented. | Verified | pokey.logic; pokey.clock; audio.spectral |
| 07.4 | ctest -L pokey exercises sourced values and actual bus-to-PCM traces, including legal channel clocks and stretched half-edge pulses. | Verified | pokey.logic; pokey.clock; audio.spectral |
| 08.1 | All existing sound codes map at most two reused chip instances at 4000–7FFF,0450–045F,0800–0FFF and 0440–044F, including dual440+450. | Verified | pokey.maps; basic/title/ledger tests; documented atomic revision2 migration |
| 08.2 | CPU reads/writes, side-effect-free peeks and MARIA fetches obey distinct documented overlap priorities; write-only overlaps preserve ROM/RAM reads. | Verified | pokey.maps; basic/title/ledger tests; documented atomic revision2 migration |
| 08.3 | Effective IRQ wiring comes only from an accepted validated v4 block; a lone440 retains secondary identity and IRQ bit1. | Verified | pokey.maps; basic/title/ledger tests; documented atomic revision2 migration |
| 08.4 | The commit enabling0450 increments behavior revision exactly once for the phase and carries five changed POKEY frame chains plus four title120 migrations, with other frame chains investigated if they drift. | Reconciled with measured migration; no unjustified re-pins | pokey.maps; basic/title/ledger tests; documented atomic revision2 migration |
| 09.1 | All existing sound codes map at most two reused chip instances at 4000–7FFF,0450–045F,0800–0FFF and 0440–044F, including dual440+450. | Verified | pokey.maps; sally.irq |
| 09.2 | CPU reads/writes, side-effect-free peeks and MARIA fetches obey distinct documented overlap priorities; write-only overlaps preserve ROM/RAM reads. | Verified | pokey.maps; sally.irq |
| 09.3 | Effective IRQ wiring comes only from an accepted validated v4 block; a lone440 retains secondary identity and IRQ bit1. | Verified | pokey.maps; sally.irq |
| 10.1 | Only effective accepted-v4 timer wiring reaches SALLY;440 owns bit1 even as slot0, and v3/invalid-v4 stays unwired. | Verified | sally.irq; cpu.irq.timing; cpu.nmi tests; audio.frames |
| 10.2 | CPU accepts level-sensitive timer IRQs using the sampled I flag at its existing poll mark, with documented entry bus cycles and persistent level behavior. | Verified | sally.irq; cpu.irq.timing; cpu.nmi tests; audio.frames |
| 10.3 | NMI retains priority and documented BRK/IRQ vector hijack timing; CLI/SEI/PLP/RTI and held poll timing match the adopted NMOS model. | Verified | sally.irq; cpu.irq.timing; cpu.nmi tests; audio.frames |
| 11.1 | sally.irq proves real accepted-v4 timer→CPU handler behavior and rejects unwired/invalid controls. | Verified | sally.irq; audio.observer; audio.samples; runner.audio.stop_failure |
| 11.2 | Observation preserves machine state, frame hashes, PCM and timing; NULL observation performs no serialization, hashing or observation-only event storage. | Verified | sally.irq; audio.observer; audio.samples; runner.audio.stop_failure |
| 11.3 | SND-03 empty: known initial zero levels emit no synthetic record; unchanged levels emit no record; a silent transition stream is the SHA-256 empty input at the runner boundary. | Verified | sally.irq; audio.observer; audio.samples; runner.audio.stop_failure |
| 11.4 | Observer host context survives power cycle while chip/synth/carry/fraction state resets; callbacks never reenter core. | Verified | sally.irq; audio.observer; audio.samples; runner.audio.stop_failure |
| 11.5 | A real raw DAC change reaches the private callback with absolute tick, fixed identity and gated code 0..15; future events retain half-open ownership. | Verified | sally.irq; audio.observer; audio.samples; runner.audio.stop_failure |
| 12.1 | Observation preserves machine state, frame hashes, PCM and timing; NULL observation performs no serialization, hashing or observation-only event storage. | Verified | audio.observer; audio.frames; audio.samples |
| 12.2 | SND-03 ordering: raw events are globally chronological, equal ticks use source IDs0..9, and meaningful same-source order survives even when synthesis merges the tick. | Verified | audio.observer; audio.frames; audio.samples |
| 12.3 | SND-03 empty: known initial zero levels emit no synthetic record; unchanged levels emit no record; a silent transition stream is the SHA-256 empty input at the runner boundary. | Verified | audio.observer; audio.frames; audio.samples |
| 12.4 | Observer host context survives power cycle while chip/synth/carry/fraction state resets; callbacks never reenter core. | Verified | audio.observer; audio.frames; audio.samples |
| 13.1 | --hash-audio prints separate audio_transitions_sha256 and audio_pcm_sha256 values over exactly the whole completed run. | Verified | runner.audio.contract; runner.audio.stop_failure |
| 13.2 | Transition bytes are LEu64 tick,u8 source,LEu16 raw level (11 bytes); mono PCM contributes two LE bytes per actual sample only. | Verified | runner.audio.contract; runner.audio.stop_failure |
| 13.3 | Successful signal stop reports only completed frames/samples; execution or dump failure emits no success audio digest. | Verified | runner.audio.contract; runner.audio.stop_failure |
| 13.4 | Rate48000/1, completed counts, behavior revision and exact-q23-16x64-dc20 accompany evidence in the existing key/value stdout style after04-06 recovery. | Verified | runner.audio.contract; runner.audio.stop_failure |
| 14.1 | Batch is preferred when both callbacks exist; single-sample fallback delivers identical L/R duplicated mono samples and actual counts. | Verified | libretro.host.audio; runner.boot_to_play and its negative controls |
| 14.2 | Partial first frame, missing callbacks, discarded output, reset and reload preserve the same future PCM as the core/runner reference. | Verified | libretro.host.audio; runner.boot_to_play and its negative controls |
| 14.3 | runner.boot_to_play requires non-silent completed PCM and rejects a muted control while retaining its existing picture/control predicates. | Verified | libretro.host.audio; runner.boot_to_play and its negative controls |
| 15.1 | Soundtest,soundeffects,trackertest and the five POKEY samples have non-silent completed-run transition and PCM pins tied to rate/behavior/synthesis identity. | Verified | audio.samples; runner.hashes_file; compare count/byte controls |
| 15.2 | Every real built sample checked for audio has zero pre-clamp clipping and delivered peak≤−1 dBFS, independently of the analytic bound. | Verified | audio.samples; runner.hashes_file; compare count/byte controls |
| 15.3 | Pins remain regression evidence; documentary chip values and alias-sensitive spectral tests establish their meaning. | Verified | audio.samples; runner.hashes_file; compare count/byte controls |
| 15.4 | Existing hashes.txt contains actual non-silent soundtest/pokey raw,PCM,count and identity fields with required built fixture dependencies. | Verified | audio.samples; runner.hashes_file; compare count/byte controls |
| 16.1 | A hostile dual-chip POKEY workload with hardware-legal fast/base/join choices program with active TIA returns bounded frames, preserves raw/PCM identity and stays within the proven carry/storage/intermediate bounds. | Verified | audio.stress; fuzz.regress.run; stress --bench 600 |
| 16.2 | Kernel additions are bounded by one merged change per master tick and at most150 tick positions per output interval; no quality switch depends on workload. | Verified | audio.stress; fuzz.regress.run; stress --bench 600 |
| 16.3 | The exact-only performance measurement records its target/limits; any conditional fallback finding preserves raw transitions and must independently pass the same SND-05 judge before adoption. | Verified | audio.stress; fuzz.regress.run; stress --bench 600 |
| 17.1 | Six native lanes and be32 compare actual non-silent audio hashes in the existing seven-file comparison; applicable WASM output compares separately to Linux. | Implementation verified; hosted equality at ship | full CI/archives; no-FP; sanitizer; hygiene; docs.budgets; existing CI/release links |
| 17.2 | Every required behavior is in existing CI, the three deliverables remain API3/ABI-compatible, and the existing merge-to-release pipeline remains wired. | Verified | full CI/archives; no-FP; sanitizer; hygiene; docs.budgets; existing CI/release links |
| 17.3 | D-25–D-29: Recovery is accepted only through completed04-06 Task2 production generation/convergence, common/final-scale spectra and fresh headroom/width/frame proofs; isolated64-phase measurements are insufficient. | Verified | full CI/archives; no-FP; sanitizer; hygiene; docs.budgets; existing CI/release links |
| 17.4 | D-30: Final identity is exact-q23-16x64-dc20 with unchanged16-tap support, clocks, raw/video evidence, API3/ABI and48kHz cadence; platform PCM equality is verified after recovery. | Implementation verified; hosted equality at ship | full CI/archives; no-FP; sanitizer; hygiene; docs.budgets; existing CI/release links |

## Artifacts and Key Links

| Path | Substantive behavior and runtime link |
| --- | --- |
| src/tia.c | Independent456-tick phases and real CPU-write DAC changes feed synthesis and private observation |
| src/pokey.c and src/bus.c | Actual fast/slow/held half edges, legal timer/poly/join/HP behavior and fixed chip identity feed levels and wired IRQ |
| src/cpu.c | Existing poll marks sample IRQ/I; shared7-cycle entry retains NMI priority and hijack, BRK and JAM contracts |
| src/synth.c and src/frame.c | Owned64-phase Q23 interpolation, bounded reconstruction/blocker/FIFO, half-open raw/PCM publication and reset; NULL bypasses observation work |
| src/cart.c | Accepted metadata maps physical chip slots/windows; peeks/DMA remain side-effect-free and overlaps preserve memory priorities |
| runner/main.c | Registers before load; stages raw hash per frame; serializes11 LE raw bytes and2 LE mono bytes per actual sample; prints only on whole-run success |
| tests/libretro/host.c and boot_to_play.c | Actual module sample parity and independent same-count silent judge retain picture/input controls |
| tests/audio/expected.txt and hashes.cmake | Eight non-silent production pins, clip/peak/observer/video audits and actual sound/POKEY rows reach existing platform comparisons |
| include/muhtari.h and public docs | API3/ABI1 unchanged, reported behavior revision2; rate, partial counts, errors, reset, callback and canonical identity match code |

## Requirements Coverage

| Requirement | Evidence | Status |
| --- | --- | --- |
| SND-01 | Every TIA mode/lattice/write/resync case and real PCM output | Verified |
| SND-02 | POKEY timer/poly/RANDOM/init/join/HP/clock/map and real CPU IRQ cases | Verified |
| SND-03 | Canonical runner bytes/counts/errors, eight pins, zero clipping, video/PCM/raw invariance and wired platform comparators | Implementation verified; hosted results at ship |
| SND-04 | Both actual callback paths equal core/runner, including missing/reset/reload; audible gameplay and muted counterexample | Verified |
| SND-05 | Alias-sensitive production spectral/reference controls and renewed integer headroom/width proofs | Verified within documented finite matrix |

## Fresh Commands and Exit Statuses

- node "$GSD_TOOLS" run-with-timeout 60 -- bash -c 'cmake --workflow --preset ci' — exit0;605 ordinary and6 archive/release checks. This full suite includes prior-phase tests and is the regression gate.
- cmake --workflow --preset hygiene — exit0;24 checks, including format/analyzer/no-float/tree.
- cmake --build --preset asan; ctest --preset asan — exit0;593 checks, after the lifetime fix. AppleClang with address/undefined sanitizers; no skipped tests or disabled sanitizer. The Homebrew runtime stalled at initialization and compiler-switch caches were refreshed as recorded in plan17.
- GCC15.3.0 aarch64-linux-gnu container: cmake --build build/nofp-linux; ctest --test-dir build/nofp-linux -L ^abi$ --output-on-failure --no-tests=error — exit0;13 ABI/no-FP checks after the fix.
- ctest --test-dir build/ci -L ^maria$ --output-on-failure --no-tests=error — exit0;11 checks.
- Debug observer/stress/runner.audio selection — exit0;5 checks including fixture setup after the fix.
- build/ci/tools/muhtari-audio-coefficients --check — exit0; all1024 cells agree at Simpson128/256/512, LE-int32 SHA2568fdfc75effc7565eff4996b71ad966f4c0d327909ede230d052dc58bc239d66e.
- build/ci/tests/muhtari-test-audio-stress --bench 600 — exit0; each workload600 frames/479591 samples, dual26,636,321 raw records, kernel max27, carry1, clips0; final desktop elapsed1,413,979,000ns versus65,814,000ns baseline, milli ratio21484. Exact PCM check6145531612794415913 is unchanged.
- scripts/hygiene.sh --tree; git diff --check — exit0.

No hosted run URL exists for this execution. No logs, ROM/audio bytes or personal
paths are committed. The benchmark is whole-core work including observation, not
DSP-only cost or calibrated low-power evidence. The >10% low-power trigger has
not been established on an applicable target; no quality fallback was adopted.

## Review and Assumptions

WR-01 (first observer allocation after load) was resolved in one review fix round:
RED11482da proves the late call violated the contract; GREEN8c933e1 moves setup
before load and rejects late allocation. Final re-review is clean. Two permitted
rounds were not exceeded; no gap round was needed. Test/tooling debt is not promoted
to sound requirements. Existing SEED-007 proportion debt remains for ship.

All30 phase decisions retain their plan17 source/decision assignments. The three
unclassified edge-classifier rows remain classifier limitations, not automatic
passes or owner tasks: explicit raw ordering/empty/half-open/error/platform,
host callback and production spectral contracts are exercised independently.
The adopted TIA startup/recurrence, POKEY propagation convention, analog calibration,
finite schedule spectra and unmeasured low-power performance retain their
published confidence limits. No universal hardware or spectral claim is inferred
from regression pins. No new package, device code, job or policy gate was added.

## Human Verification Required

None. Only the identity-bound ship/merge consent and hosted outcomes remain;
they are not UAT and are not a reason to run another execution wave.
