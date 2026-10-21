---
phase: 04-sound
plan: "07"
subsystem: pokey-audio
tags: [pokey, timers, polynomial, random, spectral]
requires:
  - phase: 04-06
    provides: Verified actual-clock chip path and exact-q23-16x64-dc20 reconstruction at M18360
provides:
  - Focused documentary timer and real-bus PCM acceptance
  - Joined counters and delayed two-tone resynchronization
  - Polynomial distortion, RANDOM and initialization semantics
  - Muted-state resumption, independent chips and narrow-pulse spectral coverage
affects: [pokey-mapping, irq-integration, audio-hashes]
tech-stack:
  added: []
  patterns:
    - Small instance-local XNOR registers and channel history pipelines
key-files:
  created: []
  modified:
    - src/pokey.c
    - src/internal.h
    - tests/audio/test_pokey.c
    - tests/audio/test_spectral.c
    - tests/core/test_core.c
    - docs/hardware/pokey.md
    - .planning/preparation/A7800-AUDIO.md
key-decisions:
  - Follow figure8 joined AUDF deadline T+9 and shared latch T+10; preserve the conflicting prose as a documentary uncertainty.
  - Use owned small LFSRs instead of proposed polynomial tables; no emulator source or dependency was used.
requirements-completed:
  - SND-02
  - SND-03
  - SND-05
coverage:
  - id: D1
    description: Focused portable selection retains N+4 vectors, IRQ values, alternate startup and nonzero production PCM
    requirement: SND-02
    verification:
      - kind: integration
        ref: tests/audio/test_pokey.c#pokey.logic
        status: pass
    human_judgment: false
  - id: D2
    description: Actual fast slow and held clocks retain sourced timing and three-half-edge high-pass delay
    requirement: SND-02
    verification:
      - kind: integration
        ref: tests/audio/test_pokey.c#pokey.clock
        status: pass
      - kind: unit
        ref: tests/audio/test_pokey.c#high_pass_vectors
        status: pass
    human_judgment: false
  - id: D3
    description: Both joined pairs cover N+7 and base periods, low pulse counts, AUDF reload deadlines and STIMER preemption
    requirement: SND-02
    verification:
      - kind: unit
        ref: tests/audio/test_pokey.c#period_vectors,reload_vectors
        status: pass
    human_judgment: false
  - id: D4
    description: Two-tone produces 8 14 20 pulses, retains adjacent resyncs and obeys force break
    requirement: SND-02
    verification:
      - kind: unit
        ref: tests/audio/test_pokey.c#two_tone_vectors
        status: pass
    human_judgment: false
  - id: D5
    description: Joined high-pass bus trace matches independently scheduled raw edges and production PCM without clipping
    requirement: SND-03
    verification:
      - kind: integration
        ref: tests/audio/test_pokey.c#joined_pcm_trace
        status: pass
    human_judgment: false
  - id: D6
    description: Literal short polynomial strings, independent long recurrence, all distortion settings and RANDOM values are verified
    requirement: SND-02
    verification:
      - kind: unit
        ref: tests/audio/test_pokey.c#polynomial_vectors,distortion_vectors
        status: pass
    human_judgment: false
  - id: D7
    description: Init drains noise and holds base clocks while fast joined timers continue; silent state and separate chips remain independent
    requirement: SND-02
    verification:
      - kind: unit
        ref: tests/audio/test_pokey.c#init_vectors,joined_init_vectors,silent_state_vectors,independence_vectors,volume_override_vectors
        status: pass
    human_judgment: false
  - id: D8
    description: Production spectral gate covers joined and two-tone high-pass pulses at fast and wholly slow clocks
    requirement: SND-05
    verification:
      - kind: integration
        ref: tests/audio/test_spectral.c#audio.spectral
        status: pass
    human_judgment: false
actuals:
  tokens: 9880
  tasks: 3
  commits: 6
plan_head_before: 8927a57a3136e04898b32f8011a2a76ddcc01222
plan_head_after: 5caa02f5ad3af60170987d2bb2c0eae02360cf86
duration: 10min
completed: 2026-10-20
status: complete
---

# Phase 4 Plan 7: POKEY music semantics Summary

**Joined timers, two-tone resync, staggered polynomial distortion and RANDOM/init run through the actual-clock production chip path.**

## Performance

- Measured implementation/verification window: 2026-10-20T22:51:47Z to 2026-10-20T23:01:07Z, approximately10min; initial context loading precedes this window.
- Tasks: 3. Product/test/documentation files: 7. No new test registrations, packages or policy checks.
- Actual token estimate is final non-planning diff characters39520 divided by4, not harness token consumption.

## Accomplishments

- Moved existing documentary N0/1/255, IRQ, startup and PCM assertions from core.audio.silence into the already registered pokey.logic executable. CI and WASM inventories already contain pokey.logic/pokey.clock and need no changes.
- Implemented both linked pairs: low wrap without automatic reload, delayed carry, shared reload and high output three clocks after the last low output. Tests cover fast N+7 and both base divisors for N0/1/255/$0140, including two low pulses per high period for $0140.
- Preserved AUDF-at-next-reload behavior, exact T+3/T+7 unjoined latches and T+4 versus T+5 STIMER preemption. Implemented timer2 unconditional and timer1 mark-conditional two-tone resync, adjacent double-resync and force-break.
- Joined+HP real-bus trace compares every completed cycle's DAC value and all88 PCM samples with an independent arithmetic edge schedule through the same production synthesizer. Existing6/9/7-tick HP delays and new override/disabled-latch tests remain portable.
- Added owned poly4/poly5/long XNOR counters and per-channel one-clock stagger, all16 distortion modes, init drain, short-mode RANDOM and retained hidden state under zero volume/volume-only. Whole-chip comparisons across different catch-up partitions prove phase preservation; separate chips and machines remain independent.
- Graduated adopted functional facts to pokey.md. Confidence limits for cold state, STIMER polarity, analog loading and sub-cycle races remain explicit.

## Task Commits

| Task | Commit | Outcome |
|---|---|---|
| 1 | f4c3b52 | Focus existing documentary timer and production PCM tests |
| 2 RED | 4917189 | Joined+HP bus tracer exposes absent joined behavior |
| 2 RED expansion | f6db01a | Documentary periods, reload/preemption and two-tone vectors |
| 2 GREEN | bbd5125 | Joined/resync pipelines and production narrow-pulse spectra |
| 3 RED | cc0101e | Polynomial/RANDOM/init/hidden-state vectors and private state slots |
| 3 GREEN | 5caa02f | Owned noise/init model, isolation tests and adopted documentation |

The first joined tracer passed before the mode matrix expanded. RED focused selections exited8 for observable missing behavior; GREEN selections and complete workflows exited0. Task1 extracts existing behavior rather than adding a product behavior, so no artificial RED was manufactured.

## Commands and exits

All commands ran at repository root against shipped objects after the final source/test changes:

| Command | Exit | Evidence |
|---|---|---|
| cmake --workflow --preset ci | 0 | 594 non-release and6 release tests; all600 pass |
| ctest --test-dir build/ci -L '^pokey$' --output-on-failure --no-tests=error | 0 | Both registered focused cases pass |
| ctest --test-dir build/ci -R '^audio.spectral' --output-on-failure --no-tests=error | 0 | Production judge and negative controls pass |
| cmake --workflow --preset hygiene | 0 | All24 checks, including format, analyzer and tree |
| cmake --preset dev && cmake --build --preset dev && ctest --test-dir build/dev -L '^pokey$' --output-on-failure --no-tests=error | 0 | Both cases with assertions; build/test repeated after final test additions |
| scripts/hygiene.sh --tree | 0 | Tree check and normal commit hooks |
| git diff --check | 0 | No whitespace errors |

No hosted workflow was dispatched for this partial wave; no run URL exists. Platform/release-path evidence remains with the later planned slice.

The spectral matrix retains132 equivalent schedules,132 alias negative controls and72 actual chip captures, plus the existing mixed/HP representatives. Four new two-chip joined+HP/two-tone+HP cases use wholly fast/slow clocks. Their physical periods are at most28 clocks, so even slow fundamentals exceed42kHz and no in-band physical line is masked. Three new settled captures quantize to silence; the wholly slow two-tone+HP capture measures−103.26565420dBFS. Nonzero raw activity and kernel work are asserted even for silent PCM. No coefficient, M18360 scale,85:342 current, Q12gain4096, clock/frame/cadence, API or behavior-revision change was made.

## Decisions and deviations

1. The manual's reload prose callsT+9 a joined reload, but figure8 defines its F marker as the last write deadline. Follow the figure's T+9 deadline, T+10 shared latch and T+11 output; retain the conflict in pokey.md instead of silently presenting both as consistent. This is a documentary qualification of the adopted model, not new hardware evidence.
2. Small per-chip shift registers/history use less state than the preparation's proposed polynomial tables. No outside source code was opened, copied or vendored.
3. Internal state declarations and extraction from tests/core/test_core.c were necessary adjacent changes beyond the plan's frontmatter list; both are named by the plan's artifacts/actions. Reused test registrations and inventories without churn.
4. The initial tracer check mistakenly inspected the current sample's kernel-add counter after rendering; that counter resets each sample. Corrected it to the existing high-water counter, retaining independent raw and PCM comparisons. During implementation, compiler signed-conversion diagnostics were resolved with explicit unsigned arithmetic. No suppressed warnings or relaxed gates.

## Issues Encountered

None outstanding. Manual disagreement and physical confidence limits remain documented model qualifications. No owner action or setup is needed.

## Next Plan Readiness

Ready for wave8. SND-02/03/05 entries above describe this plan's contributions: shared requirements remain globally incomplete while mapping, IRQ, observation, sample/hash, host, workload and platform plans remain. Do not verify or complete phase4 after this partial wave.

Next command: `$gsd-execute-phase 4 --wave 8`.

## Self-Check: PASSED

All seven named product/test/doc files exist. All six task commits are on the phase branch. Current full and focused verification passed; no test registration was removed. Phase4 remains in progress.
