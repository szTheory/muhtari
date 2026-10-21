---
phase: 04-sound
plan: "04"
subsystem: frame-audio
tags: [tia, pcm, boot, carry, conformance, tdd]
requires:
  - phase: 04-03
    provides: Documented TIA feedback and shared horizontal epoch
provides:
  - Focused portable tia.logic selection linked against the shipped archive
  - Cartridge TIA PCM through the public frame API
  - Released boot sample grids with absolute chip timing and bounded future carry
affects: [audio-contracts, pokey-clock, transition-observer, libretro-audio]
tech-stack:
  added: []
  patterns:
    - Fixed instance PCM staging publishes only successful logical frame prefixes
    - Actual cartridge clock segment origin is separate from absolute TIA time
key-files:
  created:
    - tests/audio/test_tia.c
  modified:
    - tests/audio/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - tests/core/test_core.c
    - tests/bus/test_bus.c
    - src/frame.c
    - src/boot.c
    - src/instance.c
    - src/internal.h
    - docs/embedding.md
key-decisions:
  - Fast boot advances hidden TIA on absolute ticks but starts the output grid and actual cartridge clock segment at tick108984 with phi2 zero.
  - The bus owns clock_origin_tick and clock_origin_phi2 for the later POKEY integration; BIOS retains the zero origin through actual reset cycles and handoff.
  - Reuse the existing synthesis FIFO and pending mixed tick for overshoot; no additional raw-event array or allocation is needed for this TIA slice.
requirements-completed:
  - SND-01
  - SND-03
coverage:
  - id: D1
    description: Focused shipped-object selection checks every AUDC startup pattern and period plus writes, lattice, RSYNC and bus-to-PCM wiring
    requirement: SND-01
    verification:
      - kind: integration
        ref: tia.logic
        status: pass
    human_judgment: false
  - id: D2
    description: Generated cartridge produces nonzero public PCM equal to explicitly stepped and drained production synthesis across24 frames
    requirement: SND-03
    verification:
      - kind: integration
        ref: core.audio.silence
        status: pass
    human_judgment: false
  - id: D3
    description: Fast boot retains8190 ticks and54 samples while BIOS retains117174 ticks and785 samples with real clock origins
    requirement: SND-03
    verification:
      - kind: integration
        ref: core.audio.silence and frame.first_frame_partial and bios.handoff_simple
        status: pass
    human_judgment: false
  - id: D4
    description: Boundary events stay pending and held reads plus HALT and interrupt entry preserve prefix and future PCM without changing CPU completion
    requirement: SND-03
    verification:
      - kind: integration
        ref: core.audio.silence
        status: pass
    human_judgment: false
  - id: D5
    description: NULL audio preserves subsequent samples, power cycle repeats first output, and audible BIOS refusal publishes no caller PCM
    requirement: SND-03
    verification:
      - kind: integration
        ref: core.audio.silence
        status: pass
    human_judgment: false
actuals:
  tokens: 10934
  tasks: 2
  commits: 3
plan_head_before: 975014a0434dc8a400af02820ff5f5405e45b391
plan_head_after: 67f7e048bf190479a0dfc9481244f7b62b5ff3e4
duration: 6min
completed: 2026-10-20
status: complete
---

# Phase 4 Plan 4: Public TIA Frame Audio Summary

**Cartridge TIA writes now produce public mono PCM on the released boot grids, with focused conformance and retained instruction carry.**

## Performance

- Started: 2026-10-20T20:55:50Z
- Completed: 2026-10-20T21:01:56Z
- Tasks: 2
- Files changed before summary: 11
- Actual tokens: ceiling of43733 realized diff characters divided by4.
- Commits: measured3 from the persisted plan ledger before summary metadata.

## Accomplishments

- Extracted all existing TIA documentary vectors and the decoded bus-to-production-PCM tracer into `tia.logic`, labelled `portable,tia` in exactly the ci and wasm32 inventories. All16 startup vectors,64 divider cases, alternate attractors, write timing and resynchronization assertions remain intact. Public no-content silence stays in the core test.
- Frame execution fixes the logical output end before running the CPU, catches TIA through actual completion, and drains only the successful prefix into an instance-owned801-sample work buffer. NULL audio consumes the same samples. The existing940-cell FIFO retains up to139 future samples, with its final coalesced raw tick pending when appropriate.
- Power-on resets TIA, reconstruction, pending PCM, blocker and fraction. Fast boot advances silent hidden TIA from absolute zero through108984; the sample grid then begins with remainder zero. Actual bus.phi2 stays zero. The authoritative cartridge clock segment origin is stored in bus.clock_origin_tick/clock_origin_phi2. BIOS uses zero/zero and real RES/BIOS cycles without a handoff reset.
- Generated in-memory cartridges prove nonzero public PCM, exact per-frame counts and phase against explicit production stepping/draining, discard equality, and repeatable power cycle. A held final instruction with overlapping RDY/HALT and NMI entry proves future prefix/tail retention and unchanged CPU completion. A deadline event remains pending until the next interval. Audible BIOS work followed by refusal leaves caller samples untouched on the first and sticky error calls.
- Existing frame chains, title predicates and video hashes pass without a pin migration. API3 and ABI1 remain unchanged; the phase's intentional behavior-revision migration remains with mapped POKEY work.

## Task Commits

1. Task1 — `ebbf6f3` — `test(04-04): expose focused TIA conformance selection`
2. Task2 RED — `04d65ef` — `test(04-04): require cartridge tone through public frame PCM`
3. Task2 GREEN — `67f7e04` — `feat(04-04): return cartridge TIA PCM at logical frame horizons`

## TDD Gate Compliance

Task1 extracted existing passing behavior. Investigation confirmed that the chip vectors and decoded PCM tracer were already implemented by04-03; only their executable/selection changed. This was treated as a test refactor rather than fabricating a failing chip assertion. The full workflow and focused selection passed before the tracer expanded. There is no Task1 RED/GREEN pair; this exception is explicit.

Task2's new target compiled and ran against the old implementation, returning CTest exit8. Its assertions found zero public PCM, synth origin0 instead of108984, and unadvanced hidden boot state. The unchanged current-run JUnit report passed `check tdd-red-evidence` with `RED_EVIDENCE_OK`, followed by semantic inspection: the named target executed and failed the intended observable output/origin assertions, without setup, compilation or discovery failure. The record is runtime output in ignored build/ci, not a committed log. The committed RED precedes GREEN. Additional carry/refusal/origin assertions remain in the same GREEN task; no separate refactor was needed.

## Evidence

All commands below exited0 unless explicitly identified as RED:

- `cmake --workflow --preset ci` —591 non-release tests and6 release checks passed; includes unchanged video/sample regression pins, allocation, properties, fuzz regression, spectral and public/core tests.
- `ctest --test-dir build/ci -L '^tia$' --output-on-failure --no-tests=error` —1 real conformance target passed; run again at the tracer gate before Task2 expansion.
- `ctest --test-dir build/ci -R '^core.audio.silence$|^frame\.|^bios\.|^tia\.' --output-on-failure --no-tests=error` —22 tests including fixture setup passed.
- `cmake --preset dev && cmake --build --preset dev && ctest --test-dir build/dev -R '^core.audio.silence$|^frame\.|^bios\.|^tia\.|^bus.rdy' --output-on-failure --no-tests=error` —24 checks passed with debug assertions active, including FIFO capacity assertions in held/interrupt coverage.
- `cmake --workflow --preset hygiene` —24 checks passed, including format, analyzer, integer-only and tree hygiene.
- `scripts/hygiene.sh --tree` —0.
- `git diff --check` —0.
- Task2 RED command below exited8 as expected; the classifier exited0 with `RED_EVIDENCE_OK`.

```sh
ctest --test-dir build/ci -R '^core.audio.silence$' --no-tests=error --output-junit /tmp/muhtari-04-04-red.xml
node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/ci/tdd-red-04-04.json --raw
```

No hosted run was started by this plan, so no run URL is claimed. Cross-platform equality remains the phase's later platform-pin work. No owner UAT was requested.

## Deviations from Plan

1. Task1 is an extraction of already passing tests, documented above instead of claiming an artificial RED.
2. [Rule2 - Missing critical functionality] Added the fixed PCM work buffer and explicit clock-segment origin to `src/internal.h`, necessary storage for the assigned staging/boot contracts. The existing synthesis carry implementation already supplies the proven capacity; no new buffering architecture was introduced. Commit: `67f7e04`.
3. [Rule3 - Blocking test fixture] Explicit power-on TIA initialization made the bus test's post-reset profile mutation stale. Its profile100 case now calls the existing TIA reset after changing the profile, preserving the original362-tick expectation. File: `tests/bus/test_bus.c`. Commit: `67f7e04`.

An initial implementation build caught a local variable placed in the wrong block; its scope was corrected before tests or commit. Two new test expectations were tightened to the actual lazy contract: future state can be a pending raw tick rather than already expanded PCM, and a refused frame can leave an unflushed positive raw level. Neither requires eager reconstruction or weakens prefix/output assertions.

## Limits and Remaining Phase Work

Requirements-completed records this plan's assigned requirement coverage; the global Sound requirements remain pending dependent work. POKEY execution/mappings, global observer ordering, runner hashes, host parity and cross-platform sound pins are later plans. The bus origin fields are the authoritative seam for POKEY; no POKEY implementation or simulated pre-handoff cycles were added here.

The adopted all-zero TIA startup and source-derived feedback are retained. Modes6/7/14/15 use18/16/49/47 zero-start ones, with alternate attractors documented in04-03; no stale counts were restored. Physical power-on, analog races, BIOS residue and cartridge loading remain confidence-qualified. The20730-tick/139-sample overshoot bound is conservative arithmetic, not an observed worst case. Tests exercise actual held/interrupt carry, while the prior complete-width/headroom proofs remain in force. No new endpoint, external input format or trust boundary was introduced. No known stub or unrun verification blocks this plan.

## Self-Check: PASSED

All11 changed task files exist. All three task commits are ancestors of HEAD. The tree is clean after the production commits; the on-disk summary is the canonical execution record.
