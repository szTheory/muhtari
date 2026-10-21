---
phase: 04-sound
plan: "05"
subsystem: frame-audio-contracts
tags: [tia, pcm, carry, reset, conformance]
requires:
  - phase: 04-04
    provides: Public TIA PCM with logical frame horizons and retained instruction carry
provides:
  - Portable audio.frames conformance target linked against the shipped archive
  - Active-audio buffer atomicity and allocation regression coverage
  - Long-run discard, partition, reset and exact boundary proofs
affects: [pokey-irq, transition-observer, runner-audio, platform-audio]
tech-stack:
  added: []
  patterns:
    - Generated in-memory cartridge and BIOS programs avoid committed image bytes
    - Whole-instance snapshots verify ordinary invalid calls before execution
key-files:
  created:
    - tests/audio/test_contracts.c
  modified:
    - tests/audio/tests.cmake
    - tests/core/test_core.c
    - tests/boot/test_boot.c
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - docs/embedding.md
    - src/frame.c
key-decisions:
  - Existing runtime satisfies the expanded contracts; retain its timing and synthesis unchanged and record test-only TDD exceptions.
requirements-completed:
  - SND-01
  - SND-03
coverage:
  - id: D1
    description: Generated audible cartridge preserves exact counts and PCM through1000 full frames after the54-sample first frame
    requirement: SND-03
    verification:
      - kind: integration
        ref: audio.frames
        status: pass
    human_judgment: false
  - id: D2
    description: Discarded and unevenly partitioned rendering preserves PCM, chip state, kernel tails, blocker history and rational phase
    requirement: SND-03
    verification:
      - kind: integration
        ref: audio.frames
        status: pass
    human_judgment: false
  - id: D3
    description: Exact deadline-minus-one, deadline and deadline-plus-one events obey half-open ownership with bounded future PCM
    requirement: SND-03
    verification:
      - kind: unit
        ref: audio.frames
        status: pass
    human_judgment: false
  - id: D4
    description: Power cycle reproduces startup while Reset remains a read switch and interleaved silent instances do not affect audible output
    requirement: SND-01
    verification:
      - kind: integration
        ref: audio.frames
        status: pass
    human_judgment: false
  - id: D5
    description: Active-audio invalid arguments, pitch, tags and zero or short capacity leave all instance state and caller buffers unchanged
    requirement: SND-03
    verification:
      - kind: integration
        ref: core.frame.validate_first and audio.frames
        status: pass
    human_judgment: false
  - id: D6
    description: Audible BIOS refusal publishes no failed PCM, sticky calls execute nothing further, and active audio allocates nothing after load
    requirement: SND-03
    verification:
      - kind: integration
        ref: audio.frames and bios.rejects_cart and core.no_alloc_after_load
        status: pass
    human_judgment: false
actuals:
  tokens: 8330
  tasks: 2
  commits: 2
plan_head_before: d7f1aae41bfd34c950a97841b0054dd160b86a6f
plan_head_after: 8c4335ee71ae5475e0266beddc6a952e24972dc0
duration: 5min
completed: 2026-10-20
status: complete
---

# Phase 4 Plan 5: Frame Audio Contracts Summary

**Portable audible-frame tests prove discard, reset, validation and half-open output ownership through the shipped library.**

## Performance

- Started: 2026-10-20T21:05:30Z
- Completed: 2026-10-20T21:10:32Z
- Tasks: 2
- Files changed: 8
- Actual tokens: ceiling of33319 realized diff characters divided by4.
- Task commits: measured2 from the persisted per-plan ledger before summary metadata.

## Accomplishments

- Added `audio.frames`, labelled `audio,portable`, with the exact native and WASM inventories updated. Its executable links the shipped archive and existing in-memory image builder. No cartridge or BIOS image is committed. Existing core/API validation, no-content silence, synthesis arithmetic and allocator assertions retain their entries; the OpenBIOS-specific sticky-refusal assertions remain with their fixtures.
- A cartridge that programs a steady TIA tone runs a54-sample partial first frame followed by1000 full frames. Written output, alternating NULL output with capacity0, and reference execution drained at uneven internal cuts agree sample-for-sample. Every frame checks the retained impulse ring, integrator, blocker, TIA state and fractional phase. A cumulative closed-form count checks the entire duration.
- Holding console Reset during the second half of that sequence changes the SWCHB read bit without resetting sound state. Power cycle reproduces a fresh loaded instance's complete TIA/synthesis state and first54 samples. Interleaved frames on a silent cartridge leave an audible instance byte-identical.
- Exact synthesis-tick injections at deadline−1/deadline/deadline+1 establish pending ownership and subsequent audible tails. A full119402-tick prefix plus the entire20730-tick conservative overshoot exercises finite FIFO storage with test assertions in release builds. Held execution includes a real TIA WSYNC write, a DMA-style HALT window, NMI entry and subsequent JAM frames; public prefixes and later tails match explicit production stepping/draining without changing CPU completion.
- Active audio validation covers NULL instance/frame, small/oversized tags, low/high pitches and zero/one-short capacity. Whole-instance snapshots include pending input, synthesis and all currently unobserved audio state; caller video and audio remain sentinel-filled. Exact capacity succeeds. The allocation check now runs a loaded audible cartridge and observes no allocations after load.
- An audible synthetic BIOS refusal returns count0 and leaves caller PCM untouched. The real OpenBIOS sticky call also leaves the entire refused instance unchanged. Documentation and the frame comment distinguish validation before execution from the BIOS refusal that follows partial execution; no rollback promise is added.

## Task Commits

1. Task1 — `e49c72e` — `test(04-05): prove active frame audio contracts`
2. Task2 — `8c4335e` — `test(04-05): publish portable frame audio conformance`

## TDD Gate Compliance

Both tasks were investigated as test/documentation work against existing runtime behavior. The added assertions passed without a production behavior change; `src/frame.c` changes only its inaccurate error comment and `src/synth.c` needs no change. Task2 extracts passing production proofs into a focused target and adds further assertions for exact capacity, short-call atomicity and instance isolation. No RED/GREEN behavior pair is claimed or fabricated, and no implementation step was authorized by a setup failure.

One initial held-path assertion failed because the new WSYNC write was inserted after CPU execution had already crossed the deadline. Moving the test setup earlier restored the intended held final instruction. The extracted target initially omitted the existing image-builder source and failed to link; adding that test-helper source fixed the target. Neither failure was a missing production contract or valid behavioral RED evidence. The initially unqualified formatter command was unavailable; the existing hygiene cache supplied the installed pinned formatter path.

The tracer's full CI workflow and40 focused checks passed before its task commit, and both were rerun successfully at the tracer feedback gate before Task2 expansion. No owner verification was requested.

## Evidence

All final commands below exited0:

- `cmake --workflow --preset ci` —592 non-release tests and6 release checks passed after extraction. The tracer used591 non-release tests and the same6 release checks before the additional entry existed.
- `ctest --test-dir build/ci -R '^core\.|^frame\.|^bios\.' --output-on-failure --no-tests=error` —40 tests including fixture setup passed at the tracer gate.
- `ctest --test-dir build/ci -R '^audio.frames$' --output-on-failure --no-tests=error` —1 focused test passed, approximately0.43 seconds.
- `cmake --preset dev && cmake --build --preset dev && ctest --test-dir build/dev -R '^audio.frames$|^core\.|^frame\.|^bios\.' --output-on-failure --no-tests=error` —41 selected checks passed with debug capacity assertions enabled; audio.frames approximately1.47 seconds.
- `cmake --workflow --preset hygiene` —24 checks passed, including formatting, analyzer, integer-only and tree hygiene.
- `scripts/hygiene.sh --tree` —0.
- `git diff --check` —0.

No hosted run was dispatched, so no hosted run URL is claimed. Native and WASM inventories are exact registration contracts; execution on be32/WASM and the other native hosts remains the existing hosted lanes' responsibility. This local run proves macOS arm64 release/debug behavior.

## Deviations from Plan

- Runtime conformance was already delivered by04-04. This plan therefore adds coverage and clarifying prose without a synthetic production fix or artificial RED/GREEN pair, as detailed above.
- Requirement IDs in this summary describe the assigned contribution. Global SND-03 completion remains pending dependent transition/hash/platform work; marking the whole requirement complete before `--hash-audio` exists would overstate the evidence.

## Limits and Remaining Phase Work

The20730-tick and139-sample carry limits remain conservative arithmetic, not measured worst-case hardware behavior. Test injection covers the complete synthetic time bound; actual execution covers WSYNC/HALT/NMI/JAM paths. IRQ acceptance does not exist yet: the later IRQ implementation must extend the held/interrupt case without changing the bound. Transition observation is also later work; current snapshots cover the disabled path's complete existing machine state, not an unimplemented observer.

Adopted zero TIA startup, exact fast/BIOS output origins and compatibility-model qualifications remain unchanged. The corrected18/16/49/47 duty counts from04-03 are not reopened. Physical startup, analog mix and asynchronous races remain qualified as in the source documentation. No new external endpoint, file access, public API, dependency, or trust boundary was introduced. No known stub, skipped test, or unrun plan verification blocks this plan.

## Self-Check: PASSED

The summary and focused test file exist, all8 task files are present, both task commits are ancestors of HEAD, and the current-branch evaluation scope resolves both commits. Final verification commands above passed after the last test change.
