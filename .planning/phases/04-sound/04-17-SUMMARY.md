---
phase: 04-sound
plan: "17"
subsystem: audio
requires:
  - phase: 04-16
    provides: Completed predecessor sound slice
provides:
  - Final API-compatible sound contracts and existing native be32 WASM and release pipeline reachability with local sanitizer and no-FP verification
affects: [sound-integration]
tech-stack:
  added: []
  patterns: [private-observation, portable-contracts]
key-files:
  created: []
  modified:
    - .planning/config.json
    - docs/embedding.md
    - docs/emulation-tests.md
    - docs/hardware/audio.md
    - docs/libretro.md
    - include/muhtari.h
requirements-completed:
  - SND-01
  - SND-02
  - SND-03
  - SND-04
  - SND-05
coverage:
  - id: D1
    description: Final API-compatible sound contracts and existing native be32 WASM and release pipeline reachability with local sanitizer and no-FP verification
    requirement: SND-01
    verification:
      - kind: integration
        ref: docs.budgets
        status: pass
    human_judgment: false
  - id: D2
    description: Portable regression selection runner.hashes_file
    requirement: SND-01
    verification:
      - kind: integration
        ref: runner.hashes_file
        status: pass
    human_judgment: false
  - id: D3
    description: Portable regression selection abi.api_version
    requirement: SND-01
    verification:
      - kind: integration
        ref: abi.api_version
        status: pass
    human_judgment: false
duration: ongoing full-phase execution
completed: 2026-10-21
head_after: 00fe4d22ed0e7706b910c49afad1e693532d93a8
---

# Phase 04 Plan 17: Final sound contracts and lane reachability Summary

**Final API-compatible sound contracts and existing native be32 WASM and release pipeline reachability with local sanitizer and no-FP verification.**

## Accomplishments

- Final API-compatible sound contracts and existing native be32 WASM and release pipeline reachability with local sanitizer and no-FP verification.
- Automated selections: docs.budgets,runner.hashes_file,abi.api_version.

## Automated Checks

- cmake --workflow --preset ci — exit 0.
- cmake --workflow --preset hygiene — exit 0.
- scripts/hygiene.sh --tree — exit 0.
- Assertion-enabled focused tests — exit 0.
- No hosted run URL in this local execution; cross-platform and merge evidence belong to the final lane audit and ship.

## Decisions and limits

API3 and ABI1 remain unchanged; behavior revision2, exact-q23-16x64-dc20,
16 taps, M18360, 85:342 current weights and Q12 gain4096 remain fixed.
The functional timing model retains its documented hardware confidence limits.

## Deviations

Reuse predecessor coverage where already shipped; add the missing contracts without duplicating implementation.

## Execution Details

Verified through the shipped archive and existing CI. Continue within this invocation to the next ready wave.

## Final lane evidence

- cmake --workflow --preset ci — exit0;605 checks and6 archive/release checks.
- ctest --test-dir build/ci -L ^maria$ --output-on-failure --no-tests=error — exit0;11 checks.
- cmake --workflow --preset hygiene — exit0;24 checks.
- scripts/hygiene.sh --tree and git diff --check — exit0.
- GCC15.3.0 aarch64-linux-gnu container: cmake -S . -B build/nofp-linux -G Ninja -DCMAKE_BUILD_TYPE=Release -DMUHTARI_NOFP=ON -DCMAKE_COMPILE_WARNING_AS_ERROR=ON; cmake --build build/nofp-linux; ctest --test-dir build/nofp-linux -L ^abi$ --output-on-failure --no-tests=error — all exit0,13 checks. The native nofp preset correctly refused AppleClang; no gate was weakened.
- cmake --preset asan -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++; cmake --build --preset asan; ctest --preset asan — exit0,593 checks in the final full pass. Address/undefined sanitizers and every test remain enabled. Homebrew LLVM21's runtime stalled during initialization; the agent stopped only task-owned test processes and used Apple's working runtime. Switching compiler invalidated three consumer caches; after those refreshed, the final full pass was green. No source fix or skipped test was used for these environment issues.

The expanded shared hashes file has17 lines: card frame1/frame600/PNG plus seven non-silent soundtest and seven pokey fields. Existing CI retains six native lanes plus be32 COUNT7 and a separate WASM/Linux comparison. Exact inventories include sound samples and stress on native/WASM; dynamic host tests stay native. No workflow change was necessary. The three deliverables, archive notice tests and merge-to-release path remain wired. No hosted run URL exists for this local execution; hosted equality, PR-head CI, merge and release outcomes are confirmed at ship under repository rule9, never owner UAT.

The document budget caught422 excess bytes in testing.md; duplicate platform prose was removed rather than raising the budget. The GSD regression runner now interprets test_gate_timeout in seconds; its historical4-second value was insufficient for the measured19.89-second CI test suite. Set60 seconds to cover twice that plus configure/package overhead. This is an execution timeout adjustment, not a new gate.

## Source and decision coverage

The owned TIA model, POKEY chapter5 timer/poly/HP/IRQ model, actual slow/held clock segments, validated cartridge metadata and fixed source identities remain intact. Recovery04-06 is completed with owned64-phase generation, common/final-gain spectral and fresh headroom/width/frame proofs; isolated candidates are not accepted as production evidence. Final M18360,16 taps, exact-q23-16x64-dc20,85:342/Q12 gain4096,48kHz,API3/ABI1 and behavior revision2 are unchanged in this invocation. All30 phase decisions retain the plan17 audit's assigned tests. The three unclassified edge-classifier rows remain classifier limitations; explicit deterministic requirements are exercised by chip, frame, observer, runner, host, pin and spectral checks. No additional requirement or owner check is invented. Physical analog/startup calibration, universal spectra beyond the finite matrix and low-power target performance remain qualified, not measured claims.

## Self-Check: PASSED

Final named checks passed; committed artifacts and required pipeline links exist.
