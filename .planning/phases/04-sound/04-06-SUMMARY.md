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
  - Retained failing legal slow-clock spectral vector and isolated cause
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
  modified:
    - src/internal.h
    - src/bus.c
    - src/boot.c
    - src/instance.c
    - tests/core/test_core.c
    - tests/audio/test_spectral.c
key-decisions:
  - Follow manual figure8 with first AUDF0 IRQ at8 after STIMER, then12 and16; initial forced reload at3 produces no pulse.
  - Preserve the fixed gain and physical-only masks; block on the independently reproduced wholly slow AUDF1 spectral failure.
requirements-completed: []
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
        status: fail
    human_judgment: false
actuals:
  tokens: 8820
  tasks: 1
  commits: 3
plan_head_before: 1dfa0791f8ae4df0193f8c92c01c8505cbd1580c
plan_head_after: 62a3761884b88da55dc17faf85dbdc13e092c098
duration: 9min
completed: 2026-10-20
status: blocked
---

# Phase 4 Plan 6: Owned POKEY Tracer and Quality Blocker Summary

**The fast POKEY tracer reaches production PCM; an independently reproduced slow-clock spectral miss blocks expansion completion.**

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

## Self-Check: PASSED

All13 task files exist; all three listed commits are ancestors of HEAD.
This check verifies the partial execution record, not plan completion.
