---
phase: 04-sound
plan: "06"
subsystem: pokey-audio
tags: [pokey, timers, phi2, spectral]
requires:
  - phase: 04-05
    provides: Public frame PCM and retained synthesis carry
provides:
  - Committed owned fast timer to production PCM tracer
  - Verified actual bus-clock and high-pass timer paths
  - Reproducible64-phase Q23 reconstruction and renewed signed headroom proofs
  - Preserved historical slow-clock spectral failure and independent recovery evidence
affects: [pokey-expansion, synthesis, sound-integration]
tech-stack:
  added: []
  patterns:
    - Owned documentary timer model with instance-local chip slots
key-files:
  created:
    - src/pokey.c
    - tests/audio/test_pokey.c
    - docs/hardware/pokey.md
    - tools/audio_coefficients.c
  modified:
    - src/internal.h
    - src/bus.c
    - src/boot.c
    - src/instance.c
    - tests/core/test_core.c
    - tests/audio/test_spectral.c
    - src/synth.c
    - CMakeLists.txt
    - tests/audio/test_contracts.c
    - docs/hardware/audio.md
    - .planning/preparation/A7800-AUDIO.md
    - .planning/preparation/ARCHITECTURE.md
key-decisions:
  - Follow manual figure8 with first AUDF0 IRQ at8 after STIMER, then12 and16; initial forced reload at3 produces no pulse.
  - Preserve physical-only masks and85:342 currents; use selected64-phase rows and headroom-derivedM18360 after commonM18361 acceptance.
requirements-completed:
  - SND-01
  - SND-02
  - SND-03
  - SND-05
coverage:
  - id: D1
    description: Fast independent timer periods N+4 for N0,1,255 yield IRQ state and nonzero production PCM in independent fixed chip slots
    requirement: SND-02
    verification:
      - kind: integration
        ref: core.audio.silence
        status: pass
    human_judgment: false
  - id: D2
    description: Actual fast, slow and held half edges advance timers and100 slow cycles stretch the selected edge by200 ticks
    requirement: SND-02
    verification:
      - kind: integration
        ref: pokey.clock
        status: pass
    human_judgment: false
  - id: D3
    description: Legal direct/base channel clocks and actual three-half-edge high-pass paths preserve6,9,7tick delays and narrow pulses
    requirement: SND-02
    verification:
      - kind: unit
        ref: pokey.logic
        status: pass
    human_judgment: false
  - id: D4
    description: Actual fast and wholly slow AUDF0,1,3 chip captures at fixed hardware gain satisfy the spectral gate
    requirement: SND-05
    verification:
      - kind: integration
        ref: audio.spectral
        status: pass
    human_judgment: false
  - id: D5
    description: Owned coefficient generation converges at128/256/512 and reproduces every production cell and canonical fingerprint
    requirement: SND-05
    verification:
      - kind: other
        ref: build/ci/tools/muhtari-audio-coefficients --check
        status: pass
    human_judgment: false
  - id: D6
    description: Final signed-lobe bound covers13125 phases,88alignments,176production extremizers, one-dB complete headroom and safe widths
    requirement: SND-03
    verification:
      - kind: unit
        ref: core.audio.silence
        status: pass
    human_judgment: false
  - id: D7
    description: Logical frame cadence, discard, reset, carry and buffer contracts survive the reconstruction change
    requirement: SND-01
    verification:
      - kind: integration
        ref: audio.frames
        status: pass
    human_judgment: false
  - id: D8
    description: Compact mixed-clock and high-pass representatives retain raw activity, independent square PCM and spectral rejection
    requirement: SND-05
    verification:
      - kind: integration
        ref: audio.spectral
        status: pass
    human_judgment: false
actuals:
  tokens: 21323
  tasks: 2
  commits: 8
plan_head_before: 1dfa0791f8ae4df0193f8c92c01c8505cbd1580c
plan_head_after: 2a2f2c60ba1ec4bda4af5337b19e8a26ad51c464
duration: 5min measured recovery implementation; prior9min
completed: 2026-10-20
status: complete
---

# Phase 4 Plan 6: Actual-clock POKEY and Synthesis Recovery Summary

**Actual bus half edges and high-pass timing now pass through the selected64-phase production kernel, with reproducible coefficients and fresh complete-path numerical proofs.**

## Accomplishments and resume reconciliation

Task1 and its RED/GREEN commits survive unchanged. Task2 resumes its committed
RED, retains all eight original dirty files, and finishes GREEN in `2a2f2c6`.
WINDOWS9 is fixed. The historical blocked record below preserves all six
measurements and prior commands; its old unfinished statements describe that
prior execution, not current status. Plans04-01–04-05 were not reimplemented.
Plan04-07 is now eligible; this session stops after wave6, with6/17 plans done.
Global SND completion remains pending later expansion, maps/IRQ, raw observation,
hash migration, host parity, workload and platform work. The requirement IDs in
frontmatter describe this plan's verified contribution, not whole-SND completion.

Production keeps16 taps/slots, Q23 rounding and exact normalization,48kHz,
85:342 currents and Q12gain4096. Only synthesis PCM changes; no video/raw pin,
source identity, serialized record, API3/ABI, bus/CPU timing or frame interval
was repinned. The full tests include existing video/ABI/clock pins. Transition
observation and its canonical serialization are still later work.

## Measured recovery

- Baseline production32: exactly six slow AUDF1 failures, worst−78.28741dBFS;
  `ctest --test-dir build/ci -R '^audio.spectral$' --output-on-failure --no-tests=error` exited8.
- CommonM18361 production64:132 equivalent schedules and72 actual cases pass;
  all132 aliased controls reject. Refined worstTIA−89.80738dBc and
  POKEY-equivalent−94.97775dBFS. All72 settled actual captures quantize to silence.
- FinalM18360 production64: same132/72/132 evidence, refined worstTIA−89.93812dBc,
  POKEY-equivalent−94.97775dBFS. All72 settled actual captures remain silent,
  with over1000 continuing raw changes and nonzero kernel applications asserted.
  One-channel PCM equals independent falling-cycle N+8, then N+4 schedules.
- Both gains pass the bounded84mixed/HP exploration: words1/2,1/3,3/3,1/4,
  3/4,5/4,7/4 (low bit first,1=slow), origins1/29/661, two chips,
  AUDF0/1/3 squares and equal-period AUDF2 high-pass. Square PCM matches
  arithmetic schedules chunk-for-chunk. Actual periods are lcm(word length,
  2*(N+4)) or lcm(word length,12) cycles, translated through4/6-tick words.
  Maximum square period224ticks; every fundamental exceeds16kHz, so none
  receives a mask. Worst word3/length3,AUDF1,origin29 is−90.99157514dBFS.
  Compact square and HP/origin661 representatives remain in `audio.spectral`.
- One-off peak refinement uses the unchanged windowed continuous DFT within
  ±0.6FFTbin,28ternary iterations,12000 settling and65536capture samples,
  BH4 window,48kHz and32767full scale. No mask, gain or quality workaround.

The owned C17 maintainer target generates64/128phases with128/256/512Simpson
subdivisions. All1024 selected production cells agree at all subdivisions;
canonical phase-major/tap-major LE-int32 SHA256 is
`8fdfc75effc7565eff4996b71ad966f4c0d327909ede230d052dc58bc239d66e`.
The128-phase fingerprint also reproduces
`998e669a3aba82ccf2494ca22600ef06751a6921482e93cbeed468318af4f514`.
Prior measured128 success is comparison evidence;64 is the smallest measured
passing candidate. The generator is committed and explicitly built; no
transient tool is needed to regenerate or verify production coefficients.

Final production enumeration reproduces Cmax7283992,P10865505,N2476897,
L1numerator13342402, all13125phases/88alignments and176production extremizers.
At commonM18361 the conservative complete bound plus one LSB is
29204.873052835464, above the one-dB limit29203.619489828441. FinalM18360 gives
29203.282514572144. This headroom derivation alone requires the one-unit
scale correction; spectral comparison passed before it. Pending ring cells
are320961823488000<2^49; integrator199490671800<2^38; Q30conversion
25534805990400<2^45. Temporary sum<2^50, blocker terms<2^47,
quotient/remainder product<2^60,current<2^34,gain intermediates<2^49 and
phase numerator839936<2^20 are asserted. The20730tick/139sample carry and
940sample FIFO remain structurally unchanged. Pre-clamp clipping stays zero.
`docs/hardware/audio.md` and preparation now contain the fresh formula,
proof and identity `exact-q23-16x64-dc20`;32-phase proof is historical.

## Commands and exits (recovery)

All final checks ran after the last behavior change against production objects.

- `cmake --workflow --preset ci` —0;594 non-release and6release tests pass.
- `cmake --build build/ci --target muhtari-audio-coefficients` —0.
- `build/ci/tools/muhtari-audio-coefficients --check` —0;1024 cells, convergence and fingerprint.
- `build/ci/tools/muhtari-audio-coefficients --emit 64 128` —0; generate selected rows.
  Independent emit64/128 at128/256/512 and packed LE-int32 fingerprints —0.
- `ctest --test-dir build/ci -R '^audio.spectral' --output-on-failure --no-tests=error`
  at commonM18361 —0;2tests.
- `ctest --test-dir build/ci -R '^core.audio.silence$|^audio.spectral|^audio.frames$|^pokey\.|^tia\.|^bus\.' --output-on-failure --no-tests=error`
  —0;25tests.
- `cmake --preset dev && cmake --build --preset dev` —0; same focused selection
  in `build/dev` —0;25tests with assertions enabled.
- `ctest --test-dir build/ci -L '^perf$' --output-on-failure --no-tests=error`
  —0;8existing perf/contract checks. Full hostile workload benchmarking remains04-16.
- `cmake --workflow --preset hygiene` —0;24checks including analyzer/format.
- `scripts/hygiene.sh --tree` —0; `git diff --check` —0.
- One-off python3 /tmp/muhtari-production-probes.py —0 at common and final gain;
  compiles refinement and84mixed/HP probes against `build/ci/libmuhtari.a`.
  Final rerun after the last capture change —0. Individual refined production,
  controls and mixed probe executables each exit0. No isolated synthesizer
  object replaces production in these probes.
- One-off `/tmp/muhtari-final-bound` linked against the production archive —0;
  prints the fresh masses, bound, M and zero failures. The permanent existing
  `core.audio.silence` independently asserts the same complete enumeration.

No hosted run was dispatched, so there is no hosted run URL for this step.
This is local macOS arm64 release/debug evidence;04-17 still owns platform
parity. No execution logs or scratch analyzer sources are committed.

## TDD and commits

| Task boundary | Commit | Result |
| --- | --- | --- |
| Task1 RED | ffc84aa | Preserved |
| Task1 GREEN | 2241d78 | Preserved |
| Task2 RED | 62a3761 | Preserved |
| Task2 GREEN recovery | 2a2f2c6 | Full final verification passes |

The persisted ledger begins at1dfa079 and measures8commits through the final
task head, including intervening blocked-summary/discussion/planning history;
this is not a claim of eight task implementations. Actual tokens are the
ceiling of the four task-commit diff character counts divided by4, excluding
metadata. Recovery implementation interval is5minutes measured from generator file
creation to summary finalization, excluding initial resume reads; prior execution
recorded9minutes.

## Deviations from Plan

[Rule1 — stale numeric expectation] Full CI found the boundary test's literal
12241 still reflected formerM18361. Updated it to12240 in
`tests/audio/test_contracts.c`; timing, buffer assertions and judge remain.
The same derived update changes one/two-chip level expectations to14705/16331.
Final full CI and release/debug focused checks pass; commit `2a2f2c6`.

The one-off continuous-frequency analyzer improvement is a tooling seed:
consider graduating refinement if future evidence justifies recurring coverage.
The84-case search remains one-off as planned. It introduces no permanent gate
or exhaustive configuration framework. No dependency, API or trust-boundary
change; no authentication or owner action was needed.

## Limits and next step

The finite spectral matrix does not certify every legal register stream or
analog silicon. Power-on defaults, STIMER polarity, subcycle races and cartridge
loading remain confidence-qualified. Later slices retain joined/noise/full IRQ,
maps, hashes, host, workload and cross-platform obligations. Stop here; next:
`$gsd-execute-phase 4 --wave 7`.

## Historical blocked execution record — superseded by verified recovery above

## Execution status

Task1 is complete and committed. Task2 RED is committed, but its GREEN work
is deliberately retained uncommitted because the required spectral gate fails.
The plan is blocked, not complete. No global Sound requirement is checked off,
no plan counter is advanced, and no downstream plan is authorized by this file.

## Completed task and commits

1. Task1 RED — `ffc84aa` — require the new fast POKEY timer through production PCM.
2. Task1 GREEN — `2241d78` — implement the owned fast timer and PCM tracer.
3. Task2 RED — `62a3761` — require piecewise bus clocks and legal channel timers.

The three task commits are measured from the persisted ledger. Actual tokens
are ceiling of35280 realized diff characters divided by4, measured before this
summary/metadata and including retained Task2 work. No deletions were found.

## Documentary timing reconciliation

Only the pinned January2,2026 Altirra Hardware Reference Manual was opened,
as documentation; no excluded emulator source or facts-only TIA RTL was read.
Chapter5 pp104,108 and the viewed figure8 on printed p110 establish N+4
periods, the forced initial reload at3, and AUDF0 IRQs at8,12,16. The first
forced reload sends no pulse; the next underflow/reload pipeline completes
before the first IRQ. This resolves the initial first-event reading rather
than substituting a self-generated expectation. The tracer checks N0,1,255,
IRQEN acknowledgement, independent slots, RUNNING/INIT defaults and nonzero PCM.

Fast/BIOS cartridge origins remain108984/phi2zero and0/phi2zero. The retained
Task2 implementation resets chip state once, then retimes only the unclocked
cursor at fast handoff. The bus emits rise s+2, fall/data s+4 fast/held;
rise s+2, fall/data s+5 and completion s+6 slow. It merges TIA before the
chip edges and retains actual held cycles. HP captures commit on the third
subsequent half edge: fast6, wholly slow9, mixed7. The tests preserve the
two-tick/three-tick narrow pulse. These are confidence-qualified adopted
functional conventions, not physical measurements.

## Blocking evidence

`audio.spectral` retains the original132-case equivalent-DAC/TIA matrix,
all132 aliased negative controls, unchanged hardware gain M18361, Q12 gain4096,
physical-only masks and the existing -80 dB threshold. It adds72 actual chip
cases: AUDF0/1/3, origins0/7/137, one/two chips, one/four active channels per
chip, fast/wholly slow clock sequences. Four-channel maximum stress is legal:
channels1/3 are phase-aligned fast squares and2/4 use volume-only15. Direct
fast selection on2/4 is never invented. Independent all-DAC stress remains
distinct from this legal programming.

All36 actual fast cases and30 slow cases pass. Six wholly slow AUDF1
one-channel-per-chip cases fail below16kHz at15136.23047Hz:

| Chips | Origin | Peak dBFS |
| --- | --- | --- |
| 1 | 0 | -79.42839 |
| 1 | 7 | -79.41831 |
| 1 | 137 | -79.35717 |
| 2 | 0 | -78.31037 |
| 2 | 7 | -78.28741 |
| 2 | 137 | -78.40384 |

For AUDF1, the wholly slow clock emits the first output at53, then every30
ticks. The fundamental is7159090.909/(2*30)=119318.1818Hz, so no real
fundamental or harmonic is in the judged band. The measured spur is not
an intended line. An independent arithmetic edge schedule computes these
timestamps without reading the chip counters; every one-channel actual PCM
chunk equals its production-synthesis reference byte-for-byte. Thus the
miss is reproduced through the existing kernel with a documented periodic
input, rather than caused by wrong clock expansion or by a masked harmonic.

This refutes the fast-only feasibility sweep as evidence for all slow
clock cases. D-08 currently fixes the Nyquist-sinc/Kaiser-beta8 compact
16-tap/32-phase Q23 coefficients and support; D-20 forbids gain/mask changes.
Resolving that locked reconstruction choice requires an evidence-backed
synthesis amendment/replan. No alternate kernel, rate, filter, gain or mask
has been substituted. WINDOWS entry9 records the unmet truth.

## TDD Gate Compliance

Task1's named target compiled and executed against the new interface
scaffold, failing its startup, timer output, IRQ and nonzero PCM assertions.
CTest exited8; its unchanged JUnit report passed `check tdd-red-evidence`
with `RED_EVIDENCE_OK`, followed by semantic inspection. Task1 GREEN passed
the full workflow and focused target before expansion. Its startup assertion
was corrected against the viewed manual figure before GREEN was committed.

Task2's target compiled and executed against the fast-only implementation,
failing actual bus/held cursor assertions. CTest exited8; its unchanged
JUnit passed the same machine and semantic checks. The temporary initial
inventory configuration error was fixed and was not treated as RED. A
later reference-helper declaration build error was likewise fixed before
using the successful build's spectral evidence. No fabricated report or
passing Task2 GREEN is claimed. Evidence records remain ignored runtime
output, not committed logs.

## Commands and exits

- Task1 `cmake --workflow --preset ci` —0,592 non-release tests plus6 release checks passed.
- Task1 `ctest --test-dir build/ci -R '^core.audio.silence$' --output-on-failure --no-tests=error` —0.
- Final `cmake --workflow --preset ci` —8,593/594 non-release tests pass; only audio.spectral fails with the six retained vectors. Release checks after the failed step do not run.
- `build/ci/tests/muhtari-test-spectral production` —1, six failures with independent reference PCM equality passing.
- `cmake --preset dev && cmake --build --preset dev && ctest --test-dir build/dev -R '^pokey\.|^core.audio.silence$|^bus\.' --output-on-failure --no-tests=error` —0,21 focused checks pass with assertions enabled.
- `cmake --workflow --preset hygiene` —0,24 checks pass, including analyzer and formatting.
- `scripts/hygiene.sh --tree` —0.
- `git diff --check` —0.
- Both RED commands exited8 as expected; both `check tdd-red-evidence` commands exited0.

No hosted run was started; no hosted URL is claimed. No owner UAT is needed
to establish this deterministic failure.

## Deferred issues and continuation

Task2 remains open. Its source, tests and hardware documentation are retained
in the working tree, while its RED test/inventories are committed. Do not
discard the six failures, lower gain, fold alias masks or mark the task done.
Resolve the synthesis choice with this evidence, reproduce the complete
current/new spectral matrix and headroom proof, then finish Task2 and its
verification before proceeding to chip expansion. Joined counters, other
distortions/init pipelines, mapping, CPU IRQ acceptance, transition observation
and platform evidence remain their already planned downstream slices.

No authentication gate, new dependency, network endpoint, schema or public
API was introduced. No source stub is used to claim this plan's success;
the blocking required quality truth is explicitly failed.

## Historical artifact check: PASSED

All13 task files exist; all three listed commits are ancestors of HEAD.
This check verifies the partial execution record, not plan completion.

## Self-Check: PASSED

All declared task outputs and the intentional coefficient-generator file exist;
the four task commits are ancestors of HEAD. No tracked file was deleted.
Final full CI, hygiene, focused release/debug, generator and offline production
proofs passed. The historical record above is retained for auditability.
