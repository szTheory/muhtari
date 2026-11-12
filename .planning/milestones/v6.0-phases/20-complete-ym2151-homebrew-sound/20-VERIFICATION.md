---
phase: 20-complete-ym2151-homebrew-sound
verified: 2026-11-12T11:32:16Z
status: passed
score: 5/5 goal truths verified
covered_files:
  - .github/workflows/ci.yml
  - .planning/milestones/v6.0-phases/19-measured-milestone-tune-up/19-01-SUMMARY.md
  - .planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-01-PLAN.md
  - .planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-01-SUMMARY.md
  - .planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-02-PLAN.md
  - .planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-02-SUMMARY.md
  - .planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-03-PLAN.md
  - .planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-03-SUMMARY.md
  - .planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-04-PLAN.md
  - .planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-04-SUMMARY.md
  - .planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-05-PLAN.md
  - .planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-05-SUMMARY.md
  - .planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-06-PLAN.md
  - .planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-06-SUMMARY.md
  - .planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-07-PLAN.md
  - .planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-07-SUMMARY.md
  - .planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-CONTEXT.md
  - .planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-COST-EVIDENCE.md
  - .planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-RESEARCH.md
  - .planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-REVIEW-DISPOSITION.md
  - .planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-REVIEW.md
  - .planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-SOURCE-AUDIT.md
  - .worktrees/ym-state-baseline/include/muhtari.h
  - CMakeLists.txt
  - PROVENANCE.md
  - README.md
  - THIRD-PARTY-NOTICES.md
  - build/ci/libmuhtari.a
  - build/ci/libretro/muhtari_libretro.dylib
  - build/ci/runner/muhtari-run
  - build/ci/tests/emscripten/ym-measurement.json
  - build/ci/tests/emscripten/ym-native.json
  - build/ci/tests/emscripten/ym-worst-measurement.json
  - build/ci/tests/emscripten/ym-worst-native.json
  - build/ci/tests/muhtari-test-libretro-observe
  - build/emscripten/libretro/muhtari_libretro.mjs
  - build/emscripten/libretro/muhtari_libretro.wasm
  - build/emscripten/muhtari-libretro-emscripten.zip
  - build/v6-instance-footprint-new.json
  - build/v6-phase20-final-compat-controls.log
  - build/v6-phase20-final-compat.log
  - build/v6-phase20-final-gates.json
  - build/v6-phase20-final-hygiene.log
  - build/v6-phase20-final-identities.json
  - build/v6-phase20-final-native-reviewed.log
  - build/v6-phase20-final-nofp.log
  - build/v6-phase20-final-oracle.log
  - build/v6-phase20-final-portable.log
  - build/v6-phase20-final-publisher.log
  - build/v6-phase20-final-quality.log
  - build/v6-phase20-final-sanitizer-fault-heap_oob.log
  - build/v6-phase20-final-sanitizer-fault-signed_overflow.log
  - build/v6-phase20-final-sanitizer.junit.xml
  - build/v6-phase20-final-sanitizer.log
  - build/v6-state-baseline-receipt.json
  - build/v6-state-baseline/libmuhtari.a
  - build/v6-state-compat/result.json
  - build/v6-ym-extents-new.json
  - build/v6-ym-seed/source-receipt.json
  - build/v6-ym-seed/ymfm_opm.o
  - docs/embedding.md
  - docs/hardware/audio.md
  - docs/hardware/cartridge.md
  - docs/hardware/ym2151.md
  - docs/libretro.md
  - docs/maintaining.md
  - docs/runner.md
  - docs/testing.md
  - include/muhtari.h
  - libretro/libretro.c
  - libretro/muhtari_libretro.info.in
  - scripts/emscripten.sh
  - src/bus.c
  - src/cart.c
  - src/instance.c
  - src/internal.h
  - src/state.c
  - src/synth.c
  - src/tia.c
  - src/ym2151.c
  - tests/api/v8.c
  - tests/audio/expected.txt
  - tests/audio/test_ym2151.c
  - tests/audio/tests.cmake
  - tests/audio/ym_oracle.cmake
  - tests/audio/ym_oracle.cpp
  - tests/audio/ym_vectors.h
  - tests/built/expected.txt
  - tests/cart/images.c
  - tests/cart/images.h
  - tests/cmake/hashes.cmake
  - tests/cmake/quality.cmake
  - tests/cmake/quality_evidence.cmake
  - tests/consumer/main.c
  - tests/core/state_compat.c
  - tests/core/state_compat.cmake
  - tests/core/test_core.c
  - tests/core/test_state.c
  - tests/core/tests.cmake
  - tests/diag/driver.c
  - tests/emscripten/bundle.cmake
  - tests/emscripten/host.mjs
  - tests/emscripten/tests.cmake
  - tests/emscripten/ym.cmake
  - tests/install/tests.cmake
  - tests/inventory/ci.txt
  - tests/inventory/emscripten.txt
  - tests/inventory/wasm32.txt
  - tests/libretro/host.c
  - tests/libretro/observe.c
  - tests/libretro/tests.cmake
  - tests/libretro/ym_observe.cmake
  - tests/policy/publish.cmake
  - tests/policy/quality.cmake
  - tests/policy/tests.cmake
  - tests/release/tests.cmake
  - tests/release/ym_consumers.cmake
  - tests/runner/tests.cmake
  - tests/state-compat/CMakeLists.txt
covered_digest: "v4:sha256:6d00506814c78dfbdb2df775ae502a13f4c4ff6f4ac939ff3f09a9af9b2c9cb1"
behavior_unverified: 0
overrides_applied: 0
decision_coverage:
  honored: 16
  total: 16
  not_honored: []
---

# Complete YM2151 homebrew sound verification

The selected one-profile digital sound capability meets the phase goal through the existing library, runner, native/portable libretro and clean package consumers. This is local feature acceptance; publication and hardware/host acceptance are separate. The executed source is daab4c82c0bb3393b3e93ee2b23ead09236f16ec, with production reviewed at5158bb6. Current product files are unchanged from that executed snapshot. Later changes are planning prose only.

## Goal truths

| Truth | Status | Concrete evidence |
|---|---|---|
| The documented selected/header-auto XM/YM request follows the qualified ports, enable, clock, status, timers and IRQ policy, and unsupported requests refuse transactionally. | Verified | src/cart.c, bus.c, ym2151.c and libretro.c implement the declared profile. Actual ym.profile/clock/irq, runner and loaded-module tests cover map/gate/status/manual numeric periods, refusal before allocation and supported/unsupported reload behavior in both regions. |
| The homebrew produces independently checked non-silent, bounded mono PCM and canonical observations, with retained legacy output. | Verified | C17 OPM specialization and frozen exact-pin BSD-3 oracle agree on synthesis/nonbusy status/IRQ; manual busy68 versus seed64 is explicitly checked separately. ym.mix/observer/hostile exercise signed gain, rounding, headroom, FIFO/carry, all12 actual mixed sources and both regions. Historical equality aggregates remain pinned, and native/Wasm libretro duplicate mono. |
| Complete reset/reload/isolation and YM state continuation remain bounded and transactional. | Verified | Lifecycle/hostile cases check allocator and output guards, real IRQ/JAM traffic and reset. Full YM visitor/prevalidation in state.c includes reset-relative clocks; repaired-CRC semantic faults preserve live state/battery. Genuine independently linked kickoff/current public drivers prove exact non-YM checkpoint bytes and both continuation directions in10 topology/region cases, including pending SaveKey, plus actual old/new cross-capability refusals. |
| The same owned diagnostic reaches every required native/portable/clean-package consumer with identity and existing controls intact. | Verified | ym.public_runner compares public C with fresh runner processes. libretro.host.ym loads the actual module. Schema2 native/Node observers judge independent numeric records before hashes. Actual pinned emitted Wasm and clean extracted ZIP execute six exact-inventory tests, callbacks/fallback/reset/state/isolation/refusal controls and both workloads. Native release.ym_consumers executes extracted/installed library, runner and loaded module. Actual-archive offline publisher validates transferred evidence and integrity/signer/ref faults. |
| Maintainers have accurate support, compatibility, provenance, numerical/resource and measured cost documentation. | Verified | Hardware/profile, audio/cartridge, embedding, runner/libretro, testing/maintaining and notices match implementation. No public API/ABI change; global7/2 and non-YM6/1 envelopes are distinguished. Final exact-source ordinary/adverse native/Node reports contain12-frame warmup plus3 restored60-frame windows for each region. Actual public allocator/private observer probes give concrete extents. Existing doc budgets pass; no new numerical quota or hardware/retail claim is introduced. |

## Requirements

| Requirement | Source plans | Status | Evidence |
|---|---|---|---|
| YM-01 | 20-01,02 | Satisfied | Numeric profile/clock/IRQ tests, transactional loader and loaded-module refusal; final native log. |
| YM-02 | 20-01,07 | Satisfied | Unchanged six-file seed/object receipt; exact oracle exit0; signed numeric tests and full notices. |
| YM-03 | 20-02,03,04,07 | Satisfied | Integer mix/carry and all-source chronology; pinned retained digests; actual duplicated mono; final sanitizer and native/Node results. |
| YM-04 | 20-02,03,04,07 | Satisfied | Lifecycle, hostile IRQ/JAM, no post-load allocation and borrowed-buffer guards; native and portable reset/isolation/refusal. |
| YM-05 | 20-03,06,07 | Satisfied | Full YM state/repaired-integrity semantics and BIOS origin replay; genuine old/current independent ten-case bidirectional matrix and controls. |
| YM-06 | 20-04,07 | Satisfied | Public C, real runner and loaded libretro module, independent numeric checks and callback controls before equality hashes. |
| YM-07 | 20-05,07 | Satisfied | Actual emitted/extracted portable artifacts, installed/extracted native consumers, retained notices/integrity and transferred-evidence publisher controls. |
| YM-08 | 20-01..07 | Satisfied | Final source-bound cost reports, extents and arithmetic/bounded-work proof; current docs and clean required review. |

All eight appear in plan requirement sets, the final summary's requirements-completed and current REQUIREMENTS checkboxes/traceability. Phase19's separate TUNE-10 result and original receipts remain unchanged. No requirement is orphaned or deferred from this phase.

## Artifact and wiring checks

All seven plans' declared artifact checks pass. The generic key-link matcher reports false negatives for some C/CMake source-path patterns; direct source tracing and actual consumer execution establish those links instead of treating an unattempted text pattern as a product failure. CMake builds ym2151.c into the same shipped object/archive/module path; cartridge decode reaches the bus and YM instance; existing clock edges merge TIA/YM/POKEY before synthesis; state visitor/prevalidation owns the conditional YM payload; runner and libretro call the public library; package CMake, native host and Node tests execute their actual installed/extracted products. Source, fixture, exported surface, notices and upload receipt identities are judged by the corresponding recurring controls.

The final identity receipt binds64 product paths, seven actual artifacts and four measurement reports; all75 hashes match actual files. The product tree is byte-identical to the executed source. All16 locked CONTEXT decisions have implementation/evidence coverage; the selected runtime's explicit phase-directory/context query reports16/16 honored.

## Executable evidence

Exact commands, exits, source and artifact identities are in 20-COST-EVIDENCE.md and ignored build/v6-phase20-final-gates.json plus final-identities.json. All URLs are N/A-local. Required final native713main+8release, unchanged hygiene30, exact frozen oracle, genuine compatibility and controls, qualified Apple combined ASan/UBSan17focused, full-JUnit production quality, actual pinned portable6exact tests/measurements and actual-archive offline publisher all exit0 after final production edits. Deliberate sanitizer memory/overflow faults abort with their expected diagnostics. Direct named lifecycle, hostile and YM-state spot checks also pass after the planning-only changes.

Original failed attempts remain failures: stale hash-row assertion, public state/open-bus/reload faults and mixed-chip chronology are separately reproduced before fixes and pass afterward. The mixed-source Release regression now observes all12 sources and catches the error without relying on assertions. Independent standard review is clean after two bounded fix rounds, with no required finding open and no formal gap round used. No full gate was repeated solely for planning prose.

## Scope and unclaimed evidence

Available GCC13 cannot recognize the existing required warning flag; its optional no-FP attempt exits2 before product compilation. No warning was dropped and no no-FP pass is inferred. Qualified PowerPC/qemu and WASI/Wasmtime are absent. Homebrew sanitizer startup hangs before main in an unrelated probe; qualified AppleClang uses the same combined flags and passes actual fault controls and final tests. Plan07 requires available qualified supplemental lanes, so these honest availability limits do not override a failed required contract. Actual emitted Emscripten Wasm is proven separately.

Owned diagnostic conformance, source-qualified digital modeling, seed translation parity, actual native/Wasm/package execution and observed-host cost retain distinct evidence. There is no physical XM oscillator/gain/audibility, retail library, browser/player or handheld acceptance claim. API8/ABI1, public mono, bounded instance ownership and all original unrelated bounds remain. The nine deferred v3 requirements and original Phase12/14 denied/spent host/remote stops remain untouched. No UI/device run, remote dispatch, push, merge or publication was used. No owner judgment item is needed for the selected local scope.

## Verdict

Passed for all five phase goal truths and all eight requirements. No blocking gap, acceptance override or human verification item remains in this phase. The orchestrator owns phase completion, milestone audit and local archival; shipping retains its separate authority boundary.


## Archive relocation audit — 2026-11-12

The local archive moved phase files byte-for-byte, then rebased live Markdown links. Product and execution-receipt bytes remain unchanged. The verifier remapped covered paths and recomputed the fingerprint through the selected runtime; the original source-specific verdict, qualifications and failed history remain intact. Previous fingerprint: `v4:sha256:8a3299ffede327b678794a46e59f560211798652e3d29bc25912afbce0ec632c`. No product gate was repeated for relocation.
