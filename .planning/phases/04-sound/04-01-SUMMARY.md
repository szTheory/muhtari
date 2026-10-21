---
phase: 04-sound
plan: "01"
subsystem: audio
tags: [fixed-point, reconstruction, headroom, nonlinear-mix]
requires:
  - phase: 03-games-play-in-retroarch
    provides: CPU, bus, frame deadlines and plain-C core tests
provides:
  - Production Q23 compact reconstruction, ring integration and integer DC blocker
  - Owned nonlinear mixer tables and complete transient/intermediate-width proofs
affects: [sound, spectral-tests, tia, pokey, frame-audio]
tech-stack:
  added: []
  patterns:
    - Instance-owned streaming synthesis with logical output horizon and bounded future carry
    - Production interpolation enumeration and extremizing-sequence headroom tests
key-files:
  created:
    - src/synth.c
    - src/audio_tables.h
    - docs/hardware/audio.md
  modified:
    - src/internal.h
    - CMakeLists.txt
    - tests/core/test_core.c
    - .planning/preparation/A7800-AUDIO.md
    - .planning/preparation/ARCHITECTURE.md
key-decisions:
  - Retain the approved 16-tap/32-phase architecture with regenerated Q23 coefficients, exact sum 8388608 and Q23-to-Q30 multiplication by 128.
  - Fix maximum mixed level at 18361 using the complete reconstruction/blocker/rounding bound; retain current weights 85:342 and Q12 cartridge gain 4096.
  - Bound instruction overshoot conservatively at 20730 ticks and future PCM at 139 samples, separately from 801 current-frame samples.
requirements-completed:
  - SND-03
  - SND-05
coverage:
  - id: D1
    description: Exact-rate nonzero raw-level reconstruction, partition equality, signed rounding and silent public test-card output
    requirement: SND-03
    verification:
      - kind: unit
        ref: tests/core/test_core.c#test_silence via core.audio.silence
        status: pass
    human_judgment: false
  - id: D2
    description: Compact Q23 coefficient convergence and normalization
    requirement: SND-05
    verification:
      - kind: other
        ref: python3 /tmp/muhtari-generate-q23.py
        status: pass
      - kind: unit
        ref: tests/core/test_core.c#test_synth_bound and test_synth_mix
        status: pass
    human_judgment: false
  - id: D3
    description: Nonlinear tables, complete-path headroom, pre-clamp stress and intermediate-width bounds
    requirement: SND-03
    verification:
      - kind: unit
        ref: tests/core/test_core.c#test_synth_bound and test_synth_mix via core.audio.silence
        status: pass
    human_judgment: false
  - id: D4
    description: Logical-prefix drain, discarded-output equivalence and bounded future PCM retention
    requirement: SND-03
    verification:
      - kind: unit
        ref: tests/core/test_core.c#test_silence future-carry and partition checks
        status: pass
    human_judgment: false
actuals:
  tokens: 14714
  tasks: 2
  commits: 4
plan_head_before: caff9c19710605c36ac605a4a6fc01461af806ee
plan_head_after: a47582cdc8cb903f967932c9f85a0a16663657c3
duration: 11min
completed: 2026-10-20
status: complete
---

# Phase 4 Plan 1: Production Q23 reconstruction and complete mix bounds Summary

**Integer 16-tap/32-phase Q23 reconstruction with nonlinear node-current mixing, exact 48 kHz phase, bounded PCM carry and a complete ≥1 dB headroom proof.**

## Performance

- Started: 2026-10-20T20:04:19Z
- Completed: 2026-10-20T20:15:24Z
- Duration: 11 minutes
- Tasks: 2
- Implementation/documentation files: 8
- Actual tokens use realized diff characters divided by four, rounded up; commits are measured from the persisted plan ledger before metadata commits.

## Accomplishments

- Compiled `synth.c` into the shipped object library. All mutable history lives in the instance. The internal raw-level path reconstructs nonzero PCM while public no-content output retains its existing silence contract.
- Regenerated all 512 int32 coefficients from the compact sinc/Kaiser-beta8 integral. Simpson128/256 produce identical Q23 rows. Every stored row and all 13,125 interpolated phases normalize to 8,388,608. Phase 32 retains the shifted endpoint. Identity: `exact-q23-16x32-dc20`.
- Added owned 31-cell TIA and 825-cell POKEY Q24 tables. Tests independently check every formula value, monotonicity, endpoints, dimensional normalization and node-current ratio. Declared one/two-POKEY all-channel levels are 14,706/16,332; gain is fixed by hardware and headroom, without a spectral amplitude adjustment.
- Enumerated production step differences over all 88 sample alignments, then drove both extremizing 0/M sequences through actual event ordering, ring integration and blocker. Positive/negative masses are 10,865,191/2,476,583; L1 is 13,341,774/8,388,608. M=18,361 gives conservative peak 29,203.498486 including rounding, below the −1 dBFS ceiling 29,203.619490.
- Reproduced Cmax=7,283,992 and pending bound `16*150*18361*7283992=320979305068800 < 2^49`. Documented Q23 integration, ×128 conversion, Q30 feedback quotient/remainder and signed-rounding widths separately. Maximum-declaration and rapid-transition stress report zero clipped samples.
- Documented logical horizon, half-open boundary handling, NULL discard equivalence, 801 current samples plus separately derived 139 future samples, and corrected both preparation clipping arguments and the stale approved-seed citation.

## Task Commits

1. Task 1 RED: `a3d15cb` — require a nonzero raw step through production synthesis.
2. Task 1 GREEN: `81f4bcc` — reconstruct raw levels with the Q23 integer kernel.
3. Task 2 RED: `6c31a10` — pin nonlinear TIA full-current normalization.
4. Task 2 GREEN: `a47582c` — prove nonlinear mixing and complete-path audio headroom.

The tracer's full automated command was rerun successfully before expansion.

## Verification

| Command | Exit | Result |
|---|---:|---|
| python3 /tmp/muhtari-generate-q23.py | 0 | Simpson128=Simpson256, 512 Q23 coefficients, corrected row sum and Cmax reproduced |
| python3 /tmp/muhtari-generate-mix.py | 0 | Owned tables and one/two-chip amplitudes generated from documented formulas |
| cmake --workflow --preset ci | 0 | Final run: all 588 regular tests and all six archive tests passed |
| `ctest --test-dir build/ci -R '^core.audio.silence$' --output-on-failure --no-tests=error` | 0 | Production path, proofs, adversarial sequences, carry, discard and silence passed |
| cmake --workflow --preset hygiene | 0 | All 24 hygiene/policy checks, including format, analyzer and integer-only scan, passed |
| `scripts/hygiene.sh --tree` | 0 | Clean-tree policy passed; also passed as hygiene.tree in the final hygiene workflow |
| `git diff --check` | 0 | No whitespace errors |

No hosted run was created by this local plan; hosted URLs are unavailable. Logs and RED reports remain outside committed evidence artifacts.

## TDD Gate Compliance

- Task 1 RED: the existing target executed and failed only `CHECK(nonzero)` against silence (CTest exit 8). The unchanged JUnit report was accepted as `RED_EVIDENCE_OK`. Semantic assessment: the target reached the planned PCM assertion; there was no compile, collection, fixture or unrelated failure.
- Task 2 RED: the target executed and failed only the nonlinear current assertion, observed 9180 versus expected 12241 (CTest exit 8). Its unchanged JUnit report was accepted as `RED_EVIDENCE_OK`. Semantic assessment: the bootstrap linear mixer failed the intended nonlinear behavior assertion.
- GREEN commits follow each RED commit. No separate refactor was needed. The optional phase-wide TDD-mode gate is disabled in config; task-level test-first execution was retained.

## Decisions Made

Applied the dated D-08 Q23 amendment and D-09 through D-12 as planned. The approved MIT seed's license/file were read at `9f9386c661f6`; the task-1 commit retains the required same-author seed attribution. No NES mixer/filter, external package or public API change was introduced.

## Deviations from Plan

None. A half-open boundary check was tightened while implementing future-carry tests: a pending event exactly at the output end remains pending for the next interval. This is the planned horizon contract.

## Issues Encountered

- One intermediate full CI run exited 8 solely on `policy.compat_doc_fresh`. Its positive/negative controls share a generated output path and can race; the isolated check exited 0, and subsequent complete workflow runs passed all 588+6 tests. No compatibility source, ledger or test code changed. The tooling seed is recorded in `deferred-items.md` under the proportional-gates rule.
- An intermediate hygiene run interpreted decimal voltage literals in a new C comment as floating-point code. Rewriting the comment with equivalent integer ratios fixed it; the final full hygiene workflow passed all 24 tests.
- Optional local ASan/UBSan verification was inconclusive: the configured compiler aliases were absent, so the installed LLVM 21 paths were supplied. Both the focused audio test and an unrelated `info` invocation stalled in the local sanitizer runtime; focused CTest timed out at 30 seconds (exit 8), including outside the process-inspection sandbox with leak detection disabled. This does not establish a sanitizer pass or a production defect. The required CI/hygiene/production proof checks passed. Sanitized hosted-platform coverage remains part of the existing later platform validation.
- Git index/ledger writes needed sandbox escalation; authorized task commits succeeded on `phase/04-sound`, using the GitHub noreply identity and enabled hooks. No hygiene bypass was used.

## Scope and Model Limits

The requirements listed in frontmatter identify this plan's contributions. Whole-requirement completion stays blocked by the shared-ID gate while later sound plans remain: chip/bus integration, real chip schedules, runner hashes, spectral acceptance and hosted parity are not claimed here. The next plan supplies the production spectral tracer. Physical cartridge loading and the node ratio remain unmeasured; the adopted POKEY fit is from an 800XL load. No owner UAT is required.

## User Setup Required

None.

## Next Phase Readiness

Ready for `04-02-PLAN.md`, using the shipped synthesis seams and fixed hardware-derived amplitudes. Remain on `phase/04-sound`; this executor invokes no additional GSD step.

## Self-Check: PASSED

All three created implementation/documentation files and the summary exist. All four task commits are ancestors of HEAD. The measured production range contains four commits and no file deletions.
