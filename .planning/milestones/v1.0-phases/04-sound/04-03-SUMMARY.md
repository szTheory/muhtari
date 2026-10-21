---
phase: 04-sound
plan: "03"
subsystem: tia-audio
tags: [tia, feedback, timing, rsync, wsync, spectral, tdd]
requires:
  - phase: 04-02
    provides: Production Q23 synthesis and hardware-scaled spectral judge
provides:
  - All16 TIA modes with documented zero-start vectors and state-dependent attractors
  - One mutable epoch for audio, RSYNC and WSYNC with data-tick writes
  - Actual TIA register-driven spectral sweeps through production synthesis
affects: [tia-conformance, frame-audio, transition-ordering, sound-pins]
tech-stack:
  added: []
  patterns:
    - A captures noise feedback and pulse gate; B commits pulse feedback using old noise output
    - Primary schematic derivation reconciles aggregate expectations before implementation
key-files:
  created:
    - docs/hardware/tia.md
    - .planning/phases/04-sound/04-03-RECONCILIATION.md
    - .planning/phases/04-sound/04-03-CHECKPOINT.md
  modified:
    - src/tia.c
    - src/internal.h
    - src/bus.c
    - tests/core/test_core.c
    - tests/audio/test_spectral.c
    - tests/frame/test_frame.c
    - docs/hardware/audio.md
    - .planning/preparation/A7800-AUDIO.md
    - .planning/phases/04-sound/04-03-PLAN.md
key-decisions:
  - Keep documented recurrence and locked N=P=0 startup; correct modes6/7/14/15 to18/16/49/47 ones and mode11 fill12.
  - Record alternate N=21/P=0 results without claiming that the lost earlier simulation used that seed.
  - RSYNC completes old due phases and schedules only future phases on the shared epoch.
requirements-completed:
  - SND-01
  - SND-03
  - SND-05
coverage:
  - id: D1
    description: Bus-driven TIA tone reaches production PCM with slow writes at start+5 and uneven B spacing
    requirement: SND-03
    verification:
      - kind: integration
        ref: core.audio.silence
        status: pass
    human_judgment: false
  - id: D2
    description: All16 first40 zero-start outputs and64 AUDF0/1/5/31 period and ones cases match documentary expectations
    requirement: SND-01
    verification:
      - kind: unit
        ref: tests/core/test_core.c test_tia_modes
        status: pass
    human_judgment: false
  - id: D3
    description: Alternate state attractors, constant fills, divider wrap, register masks, immediate volume and split-phase writes are checked
    requirement: SND-01
    verification:
      - kind: unit
        ref: tests/core/test_core.c test_tia_modes and test_tia_controls
        status: pass
    human_judgment: false
  - id: D4
    description: RSYNC adjacency and subsequent WSYNC share an epoch and preserve original no-RSYNC timing assertions
    requirement: SND-01
    verification:
      - kind: integration
        ref: core.audio.silence and bus.rdy and frame.maria_wsync_releases_at_lrc
        status: pass
    human_judgment: false
  - id: D5
    description: Zero volume preserves hidden state, unchanged levels emit nothing and raw calls preserve channel and same-tick write order
    requirement: SND-01
    verification:
      - kind: unit
        ref: core.audio.silence
        status: pass
      - kind: other
        ref: cc -std=c17 -Isrc -Iinclude build/ci/tia-order-probe.c src/tia.c -o build/ci/tia-order-probe && build/ci/tia-order-probe
        status: pass
    human_judgment: false
  - id: D6
    description: Real TIA AUDF0..31 sweeps at three epochs pass production spectral acceptance and same-gain alias controls are rejected
    requirement: SND-05
    verification:
      - kind: integration
        ref: audio.spectral and audio.spectral.controls
        status: pass
    human_judgment: false
actuals:
  tokens: 14503
  tasks: 2
  commits: 6
plan_head_before: 95d348ecba5b129aa80d702e5806e18c66ecb791
plan_head_after: 38722095bb7c88cd533efded2ad2056a411e2fb9
duration: 23min
completed: 2026-10-20
status: complete
---

# Phase 4 Plan 3: TIA Feedback and Shared Epoch Summary

**All16 TIA modes run on the independent A/B lattice, with reconciled startup vectors, shared RSYNC/WSYNC timing and real-tone spectral acceptance.**

## Performance

- Started: 2026-10-20T20:29:47Z
- Completed: 2026-10-20T20:52:42Z
- Tasks: 2
- Files in realized pre-summary diff: 13
- Actual tokens: ceiling of58010 realized diff characters divided by4, measured from the persisted plan base.
- Commits measured before this execution-metadata commit: 6, including checkpoint and reconciliation documentation.

## Accomplishments

- Preserved the bus-to-TIA-to-production-PCM tracer and start+5 data timing; the full pulse-register model adds its documented three-low-output startup latency before the first div2 high.
- Implemented owned feedback for every AUDC, with A-captured noise input and pulse gate, B-selected pulse input, old-noise feedback, AUDF comparison/wrap, masked registers and immediate AUDV.
- Checked16 first40 vectors,64 mode/divider period/count cases, both alternate duty attractors and mode11's alternate fill10, the20→5/count12 wrap delay26, between-phase and same-tick writes, both channels and hidden-state advancement under silence.
- RSYNC completes old due events before resetting the epoch and schedules no retroactive A0. Before/on/after-B adjacency and actual bus RSYNC→WSYNC→held-read cases pass. MARIA's later pending release still wins, and original no-RSYNC timing values are retained.
- Replaced the96 injected TIA spectral schedules with real register programming and catch-up. The36 POKEY-equivalent schedules and validated analyzer/alias controls remain unchanged in purpose.
- Graduated equations, vectors, sources, phase conventions and physical limits to hardware docs. No external package, source seed, ROM bytes, host-device code or new policy gate was added.

## Task Commits

1. Task1 RED — `1aa96d9`, decoded tone and data ticks.
2. Task1 GREEN — `2060afa`, decoded TIA tone through production synthesis.
3. Task2 RED — `8f2cb44`, all-mode period and ones assertions.
4. Checkpoint — `521027c`, documentary model mismatch recorded without claiming completion.
5. Acceptance reconciliation — `c51da68`, source-derived zero-start correction committed before implementation.
6. Task2 GREEN — `3872209`, full feedback, shared epoch and actual TIA spectral sweeps.

## TDD Gate Compliance

Both tasks have committed RED followed by GREEN. Task2's original RED target
`core.audio.silence` executed and failed because unsupported modes produced
zero ones; it was not a build, discovery or fixture failure. After the
source-backed acceptance correction, a new JUnit run also failed on the
planned startup bits/totals. Both unchanged reports were accepted by
`check tdd-red-evidence` with `RED_EVIDENCE_OK`. The semantic assessment is
that the intended target ran and asserted missing waveform behavior.
No separate refactor was needed. The full task remains one coherent GREEN
commit following its prior RED; no mode assertions were dropped to pass.

## Reconciliation and Limits

Primary Atari TIA-1A revision E sheet4, independently checked against pinned
hardware documentation, supports the retained recurrence. Two independently
written mathematical representations agree across8192 mode/state
transitions. Exhaustive512-start orbit enumeration explains the earlier
aggregate mismatch: zero startup yields18/16/49/47 ones in6/7/14/15 and
mode11 fill12. N=21/P=0 yields13/15/44/46 and fill10 with exactly the same
equations. That counterexample does not establish the lost probe's seed.
The unconditional6/10 and7/9 complement claims were removed.

The production TIA source also agrees with the independent stage-array
equations across8192 transitions in the temporary probe. Its raw-call trace
asserts `(64,TIA0,15)`, `(64,TIA1,15)`, `(64,TIA0,7)`, unchanged suppression
and resumed output after silent hidden progression. That probe substitutes
only the synthesis sink; recurring waveform, bus, volume, epoch and PCM
checks run against the shipped archive in CI. Global multi-chip observer
ordering remains the later observer plan's responsibility.

All-zero startup, asynchronous write conventions and RSYNC edge treatment
are explicit adopted functional behavior. Physical startup, disputed chip
patterns, analog races, BIOS residue and cartridge loading remain
confidence-qualified; no physical measurement or owner listening is claimed.
The public frame audio hookup, focused `tia` label, whole-run hashes and
platform pins remain assigned to their dependent plans. Requirement IDs
above identify this plan's delivered coverage; phase-wide requirements are
not fully certified by this isolated slice.

## Spectral Results

| Measurement | Result |
|---|---:|
| Actual TIA worst physical-frequency reference | -82.64927 dBc |
| TIA conservative nearest-bin cross-check | -82.10380 dBc |
| POKEY-equivalent worst | -94.71708 dBFS |
| Production matrix | 132 evaluated,0 failures |
| Same-gain nearest-sample controls | 132 rejected,0 failures |
| Clipped samples | 0 |

TIA margin remains2.64927 dB at fixed hardware scale M=18361, with no
mask expansion or gain reduction. All32 AUDFs at offsets0/7/137 use both
real channels. Captures settle12000 samples and measure65536 int16 samples.

## Command Evidence

| Command | Exit | Result |
|---|---:|---|
| `cmake --workflow --preset ci` | 0 | 590 ordinary tests and6 release tests pass |
| `ctest --test-dir build/ci -R '^core.audio.silence$\|^audio.spectral\|^bus.rdy\|^frame.(maria_wsync\|hold_split)' --output-on-failure --no-tests=error` | 0 | Seven focused tests pass |
| `build/ci/tests/muhtari-test-spectral production` | 0 | 132 evaluated,0 failures |
| `build/ci/tests/muhtari-test-spectral controls` | 0 | 132 alias controls rejected |
| python3 /tmp/muhtari-tia-reconcile.py | 0 | Independent8192-transition and all512-start/mode reconciliation |
| `cc -std=c17 -Isrc -Iinclude build/ci/tia-order-probe.c src/tia.c -o build/ci/tia-order-probe && build/ci/tia-order-probe` | 0 | Exact raw-call order and8192 production transitions |
| `node .../gsd-tools.cjs check tdd-red-evidence build/ci/tia-modes-reconciled-red.json --raw` | 0 | RED_EVIDENCE_OK; actual failing target, exit8 |
| `scripts/hygiene.sh --tree` | 0 | Clean source/asset/privacy scan |
| `/opt/homebrew/opt/llvm@21/bin/clang-format --dry-run --Werror src/tia.c src/internal.h tests/core/test_core.c tests/frame/test_frame.c tests/audio/test_spectral.c` | 0 | Required formatting passes |
| `git diff --check` | 0 | No whitespace faults |

Hosted run URL: unavailable; no hosted run was started by this executor.
No execution log is committed. Temporary mathematical/raw-sink probes are
local evidence; recurring production checks are the existing CI tests.

## Deviations from Plan

1. **[Rule1 - Documentary bug] Corrected zero-start acceptance.** Task2
   exposed aggregate expectations inconsistent with the locked startup.
   Primary schematic reconciliation established both joint-state basins.
   Preparation and plan were amended together in `c51da68` before changing
   production; all alternate-state qualifications remain explicit.
2. **[Rule3 - Blocking fixture setup] Applied the power-on profile through
   TIA reset.** Full CI initially exited8 on
   `frame.maria_wsync_releases_at_lrc`: its internal fixture changed
   `profile.tia_phase` after an earlier TIA write had seeded the stored
   epoch. Both such fixture sites now reset TIA after assigning their
   test profile, preserving every original tick assertion. The corrected
   full CI exits0. Included in `3872209` with the related hardware-doc
   update distinguishing real TIA from equivalent POKEY spectra.

Both resolved deviations are recorded as fixed in WINDOWS.md. One directly
caused CI correction was needed; no unrelated warning or prior defect was
changed. Authentication gates: none. No task verification was skipped,
no blocking stub was introduced and no new trust-boundary surface exists.

## Next Work

Continue the existing execution step with04-04's focused TIA conformance
and public-frame audio integration. No new GSD workflow or owner UAT is
needed to close this checkpoint.

## Self-Check: PASSED

Created hardware/reconciliation/summary files exist, and every task,
checkpoint and reconciliation commit listed above is an ancestor of HEAD.
The persisted plan ledger measures6 commits from95d348e through3872209.
