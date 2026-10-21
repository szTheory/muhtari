---
phase: 04-sound
reviewed: 2026-10-21T01:17:20.569580+00:00
depth: standard
files_reviewed: 55
files_reviewed_list:
  - CMakeLists.txt
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
findings:
  critical: 0
  warning: 0
  info: 0
  total: 0
status: clean
---

# Phase 04: Code Review Report

## Narrative Findings (AI reviewer)

One lifetime warning was found and resolved in fix round1. Final re-review has no open critical or warning findings.
Review performed inline by Codex under the skill's no-spawn fallback; no independent child or external reviewer was used.

Runtime review traced actual bus half edges and source merge through TIA/POKEY,
weighted DAC conversion, coalesced mixed deltas, phase interpolation, blocker and
bounded FIFO; validated cartridge wiring reaches sampled CPU IRQ entry. The IRQ
entry preserves the old-I rules and NMI priority/hijack, and keeps BRK/JAM cases
separate. Raw observation is outside reset machine storage, allocated only during
registration and freed on disable/destroy. Reset clears raw/staging/carry history
without losing host context. Half-open publication retains future records, and
stable source ordering preserves same-source pulses before mixing. No hash,
serialization, allocation or observation storage occurs on the disabled path.

Runner review checked flag-only argument consumption, duplicate/mode refusals,
11 explicit raw bytes, unsigned conversion for signed mono samples, actual counts,
tentative callback state and final success-only printing after dump work. Partial
first frames, no content, signal stops and sticky BIOS refusal are exercised by
actual runner-source tests. Public API3 and ABI1 are retained. The existing module
and core deliver identical non-silent samples across batch/fallback/absent callback
paths and resets; gameplay has an independent same-count silent control.

Generated numeric tables and finite width/headroom claims are supported by the
owned generator, complete phase tests, independent spectral reference and alias
controls. Pins are not their own hardware oracle. All eight built samples compare
independent core/runner canonical streams, observer/discard/reset behavior,
pre-clamp audits and delivered peaks. Hostile legal clock cases include held
instructions, IRQ and JAM, with bounded allocation-free frame execution.

CI reachability retains exact inventories, required sample fixtures, three
archives, six native plus be32 comparison and separate WASM/Linux comparison.
No new package, device code, CI job, policy gate or public observer was added.
The existing gate-code proportion debt remains in SEED-007; this review does not
turn tooling debt into sound requirements or spend a fix round.

## Evidence and limits

Release CI605 plus6 archive checks, hygiene24, GCC15 aarch64 no-FP13,
MARIA11, Debug observer/sample/stress checks and Apple address/undefined sanitizer
593 checks passed. No committed execution logs or ROM/audio bytes. Hosted full
platform equality and merge/release execution remain ship checks. Functional
hardware/startup/analog confidence and finite spectral/desktop performance limits
remain stated in the hardware docs and summaries.

The evaluation-scope owner used its phase-range fallback because older summaries
lack canonical task-commit rows. The resolved paths are all on this phase branch;
new summary metadata now names concrete artifacts and task commits. No unrelated
branch's work was treated as review evidence.

## Resolved in fix round 1

### WR-01: First observer registration allocated after load

**Files:** runner/main.c:901 and src/synth.c:275.
**Issue:** The runner registered its optional private tap after cartridge load,
contrary to the locked no-allocation-after-load contract. The helper also lacked
a guard against that first late registration.
**Resolution:** 8c933e1 moves registration before load in the runner and independent
test drivers; the helper rejects a first late registration before allocating.
NULL observation remains allocation-free. Load/reset preserve the host tap.
RED11482da compiles and fails audio.observer with CTest exit8 on the late
registration/storage assertions; GREEN passes after the fix. Corpus tests now
also compare every native video byte with observation enabled/disabled.
**Re-review:** The source guard, all callers and rejection cleanup were inspected.
Fresh full CI605+6, sanitizer593, GCC no-FP13, hygiene24 and Debug observer/stress/
runner selections pass. Raw/PCM pins and synthesis identity are unchanged. One
of two permitted review fix rounds is used; no gap round is used.
