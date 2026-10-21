---
phase: 04-sound
plan: "02"
subsystem: audio-testing
tags: [spectral, q23, fft, blackman-harris, alias-controls]
requires:
  - phase: 04-01
    provides: Production Q23 reconstruction, nonlinear hardware mix and complete headroom proof
provides:
  - Actual-production 132-case hardware-scaled spectral matrix
  - Same-gain nearest-sample alias rejection and validated physical-only spectral judge
affects: [tia, pokey, audio-parity, sound-pins]
tech-stack:
  added: []
  patterns:
    - Test-only owned radix-2 FFT with exact-frequency window-peak reference
    - Coordinate-derived schedule identities reject missing and duplicate evaluations
key-files:
  created:
    - tests/audio/test_spectral.c
    - tests/audio/tests.cmake
  modified:
    - tests/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - docs/hardware/audio.md
key-decisions:
  - Retain fixed hardware gain M=18361 and exact-q23-16x32-dc20; accept strictly below -80 dB using only physical in-band masks.
  - Use the validated exact-frequency fundamental reference for acceptance and separately report the research nearest-bin reference for direct comparison.
requirements-completed:
  - SND-03
  - SND-05
coverage:
  - id: D1
    description: Production mixer, Q23 kernel, integer blocker and int16 output pass all 132 hardware-scaled equivalent schedules
    requirement: SND-05
    verification:
      - kind: integration
        ref: ctest --test-dir build/ci -R '^audio.spectral$' --output-on-failure --no-tests=error
        status: pass
    human_judgment: false
  - id: D2
    description: All 132 same-gain nearest-sample controls are rejected and off-bin normalization, physical-line leakage and near-mask spur controls pass
    requirement: SND-05
    verification:
      - kind: unit
        ref: ctest --test-dir build/ci -R '^audio.spectral.controls$' --output-on-failure --no-tests=error
        status: pass
    human_judgment: false
  - id: D3
    description: Production arithmetic and complete headroom proof remain green alongside spectral captures with zero clipped samples
    requirement: SND-03
    verification:
      - kind: integration
        ref: cmake --workflow --preset ci
        status: pass
      - kind: unit
        ref: core.audio.silence
        status: pass
    human_judgment: false
actuals:
  tokens: 4848
  tasks: 2
  commits: 2
plan_head_before: d319c448bc6517ce2ca1aaebb274b7da99aa6b37
plan_head_after: f7fcbb96c09b5e70aa11777d48c70255cf4c656c
duration: 7min
completed: 2026-10-20
status: complete
---

# Phase 4 Plan 2: Production Spectral Matrix Summary

**The shipped Q23 audio path passes 132 hardware-scaled schedules, while the same judge rejects all 132 nearest-sample alias controls.**

## Performance

- Duration: approximately 7 minutes
- Tasks: 2
- Files modified: 6
- Actual tokens: ceiling of 19,389 realized task-diff characters divided by four; measured task commits: 2

## Accomplishments

- Proved the two-case tracer end to end before expansion: true uneven TIA B-spacing AUDF0 and synchronous two-chip POKEY-equivalent AUDF0 both passed production rendering. Same-gain aliased controls and a near-mask spur were rejected.
- Expanded to 96 TIA schedules (AUDF0..31, offsets 0/7/137) and 36 POKEY-equivalent schedules (AUDF0/1/3, one/two declared chips, one/all channels, offsets 0/7/137). Every coordinate is evaluated exactly once in each path.
- Kept the production mixer, kernel, blocker and int16 arithmetic unchanged. Each production capture asserts the independently calculated hardware-derived mixed level and zero clipped samples.
- Registered `audio.spectral` and `audio.spectral.controls` in CI and wasm32 inventories; test-only math links the platform math library outside the core. No package or outside source was added.
- Documented the measurement, reference, physical-line masks, control path, matrix and model limits in `docs/hardware/audio.md`.

## Task Commits

1. Task 1: Prove one production Q23 tone and an aliased counterexample — `dc13557` (test)
2. Task 2: Reproduce the complete 132-case hardware-scaled feasibility matrix — `f7fcbb9` (test)

## Measurements and Research Comparison

| Measurement | Production result | Research reference |
|---|---|---|
| Maximum mixed level M | 18361 | 18361 |
| Worst TIA, exact-frequency fundamental reference | -82.64927 dBc | Research used nearest-bin amplitude |
| Worst TIA, research nearest-bin reference | -82.10380 dBc | -82.10380 dBc |
| Worst POKEY-equivalent | -94.71708 dBFS | -94.71708 dBFS |
| Production acceptance | 132 evaluated, 0 failures | 132/132 |
| Aliased controls | 132 evaluated, 132 rejected, 0 failures | 132/132 rejected |

The TIA acceptance-reference difference is 0.54547 dB. Evaluating the validated window peak at the exact physical fundamental avoids nearest-bin scalloping; reporting the research reference separately reproduces its value exactly. No gain reduction or mask expansion explains the difference. Minimum acceptance margin is 2.64927 dB for TIA and 14.71708 dB for POKEY-equivalent stress.

Both paths settle for 12,000 delivered samples and capture 65,536 actual int16 samples. The periodic BH4 coefficients are 0.35875/0.48829/0.14128/0.01168, with amplitude `2|DFT|/sum(window)`. Search is strictly `0 < f < 16000 Hz`. Audible fundamentals use dBc; ultrasonic-only signals use 32767 as the dBFS reference. Masks include only physical in-band lines within five bins, never folded harmonics. Off-bin known sines at four phases verify amplitude to 0.001%; physical multi-line leakage stays below acceptance and the six-bin, -60 dBc spur fails it.

## Verification Evidence

| Command | Exit | Result |
|---|---|---|
| cmake --workflow --preset ci | 0 | Tracer and final matrix runs passed all 590 ordinary tests plus 6 release tests |
| `ctest --test-dir build/ci -R '^audio.spectral' --output-on-failure --no-tests=error` | 0 | Both spectral tests pass, including 132/132 alias rejection |
| `ctest --test-dir build/ci -R '^(audio.spectral\|core.audio.silence)' --output-on-failure --no-tests=error` | 0 | Three tests pass, including full-path headroom and width assertions |
| `build/ci/tests/muhtari-test-spectral production` | 0 | 132 evaluated, 0 failures; research comparison reproduced |
| `build/ci/tests/muhtari-test-spectral controls` | 0 | 132 alias controls rejected, 0 failures; analyzer controls pass |
| python3 /tmp/muhtari-generate-q23.py | 0 | Simpson128=Simpson256, 512 coefficients, row sum 8388608, Cmax 7283992 |
| `scripts/hygiene.sh --tree` | 0 | Repository hygiene passes |
| clang-format --dry-run --Werror tests/audio/test_spectral.c | 0 | LLVM 21 formatting passes |
| `git diff --check` | 0 | No whitespace errors |

Hosted run URL: unavailable for these local commits; hosted platform validation remains a later phase obligation. No execution log is committed.

## Decisions Made

Applied the fixed hardware scale, physical-only masks and strict threshold as planned. The owned test-only FFT avoids a source seed or package. Exact-frequency reference normalization is validated against known off-bin sines; the research's conservative reference is also reported for comparison.

## Deviations from Plan

- Added the measurement contract to `docs/hardware/audio.md` alongside the test changes under AGENTS.md rule 5. This documentation addition changes no production behavior.
- Both tasks are test-only against the already implemented production slice. They produced test commits, without a fabricated failing production RED or a production implementation commit. The configured phase-wide TDD gate is disabled. See TDD Gate Compliance below.

## TDD Gate Compliance

The task-level `tdd=true` annotations did not result in a formal RED/GREEN commit pair: production functionality already existed, and this plan adds its analyzer and tests. The tracer passed against existing production code; investigation confirmed that both deliberately aliased controls fail the same acceptance judge. During matrix expansion, a real failing analyzer assertion rejected exactly silent ultrasonic captures, then passed after its correction. That was a harness bug, not evidence of missing production behavior, and no classifier-certified RED is claimed. No production source was changed to manufacture one. The two task commits remain atomic and independently verified.

This procedural deviation is recorded as open entry 6 in `.planning/WINDOWS.md`; the ship ledger requires its disposition before shipping. It does not represent an unimplemented sound behavior or a skipped spectral verification.

## Issues Encountered

- The first matrix run exited 8 because eight ultrasonic captures have exactly zero unintended energy after int16 quantization. The analyzer incorrectly required a finite dB result. It now accepts negative infinity for zero peak while rejecting NaN, positive infinity and invalid references. Coordinate-derived coverage and actual sample delivery checks still reject missing evaluations. The final 132-case run passes with no relaxed acceptance threshold.
- Git ledger/index writes required narrow sandbox escalation. Both task commits succeeded with repository hooks enabled and the GitHub noreply identity. No bypass was set.

## Scope and Model Limits

Frontmatter requirement IDs identify this plan's contributions. Whole-requirement completion remains subject to the shared-ID gate while later plans implement chip/bus timing, runner hashes, actual-chip spectral sweeps and hosted arithmetic/parity proofs. Equivalent DAC stress does not establish which real channels select fast clocks, timer polarity, or physical loading. The unmeasured cartridge node/load limits remain unchanged. There are no known stubs or new security surfaces.

## User Setup Required

None.

## Next Phase Readiness

The production spectral judge and negative controls are ready for the actual TIA and POKEY schedules in later plans. Remain on `phase/04-sound`; this executor starts no other plan or workflow.

## Self-Check: PASSED

Both created test files and this summary exist. Both task commits are ancestors of HEAD. The persisted ledger measures two task commits, and neither deletes a tracked file.
