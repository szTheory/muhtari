---
phase: 13-portable-save-load-across-three-interfaces
plan: "08"
subsystem: testing
tags: [c, state, portability, cmake]
requires:
  - phase: 13-07
    provides: continuation scenarios and canonical state rows in the lane hash artifact
provides:
  - User and preparation documentation reconciled with the versioned state contract.
  - Largest generated topology exercises public apply, late refusal, and allocation stability.
  - Complete-work measurements and successful exact-source native/PowerPC/QEMU and sanitizer receipts.
affects: [portable-save-load, native-be-equality, state-acceptance]
actuals:
  tokens: 15306
  tasks: 3
  commits: 4
commits: 4
plan_head_before: d600394ca8fe31326a8cdb4401000c2f4d498c9e
plan_head_after: 01adc1af52ad4f15ce5bc48fcc9019f8217d04c2
tech-stack:
  added: []
  patterns:
    - Measure the largest generated topology separately from the synthetic independent-max bound.
    - Use the existing Docker Desktop Linux VM with Ubuntu ARM64, the pinned PowerPC cross toolchain and QEMU user-mode emulation for reproducible local acceptance.
key-files:
  created:
    - .planning/phases/13-portable-save-load-across-three-interfaces/13-RESOURCE-EVIDENCE.md
  modified:
    - docs/embedding.md
    - docs/runner.md
    - docs/libretro.md
    - .planning/preparation/ARCHITECTURE.md
    - .planning/preparation/DECISIONS.md
    - tests/core/test_state.c
    - docs/testing.md
    - .planning/preparation/HOSTS-AND-INTEGRATION.md
key-decisions:
  - Keep the shared little-endian visitor and cached effective machine identity; CRC remains corruption detection only.
  - Do not claim phase acceptance without exact-source executed native/PowerPC equality receipts.
requirements-completed: [STATE-01, STATE-02, STATE-03, STATE-04, STATE-05, STATE-06, FRONT-01]
coverage:
  - id: D1
    description: Public and preparation docs match the implemented state format and identity contract.
    requirement: STATE-01
    verification:
      - kind: other
        ref: "docs.budgets, docs.api_names, abi.api_version and libretro.vendored direct policy consumers"
        status: pass
    human_judgment: false
  - id: D2
    description: The largest generated state round-trips publicly and rejects a CRC-correct late invalid field without allocation.
    requirement: STATE-02
    verification:
      - kind: unit
        ref: "direct optimized core.state executable: aliases, battery, fields, lifecycle, payload, topologies and equality_report"
        status: pass
    human_judgment: false
  - id: D3
    description: Repeated state operations remain allocation-free and bounded under the required sanitizer lane.
    requirement: STATE-04
    verification:
      - kind: unit
        ref: "core.state.topologies direct run and measured state.c stack frames"
        status: pass
      - kind: other
        ref: "670-test ASan/UBSan preset and complete inventory passed on final source; see 13-RESOURCE-EVIDENCE.md"
        status: pass
    human_judgment: false
  - id: D4
    description: Battery state rollback keeps emulated bytes and host-visible metadata semantics distinct.
    requirement: STATE-05
    verification:
      - kind: unit
        ref: "direct core.state.battery case; host-file sentinel and battery token controls"
        status: pass
    human_judgment: false
  - id: D5
    description: Runner state-file and libretro callback consumers remain in the required normal inventory.
    requirement: STATE-06
    verification:
      - kind: integration
        ref: "689 normal and 6 release inventory entries passed in both native and PowerPC/QEMU workflows; see 13-RESOURCE-EVIDENCE.md"
        status: pass
    human_judgment: false
  - id: D6
    description: Exact-source native and big-endian runs produce equal state bytes and continuation rows.
    requirement: STATE-03
    verification:
      - kind: integration
        ref: "Final-source native and PowerPC/QEMU identity and canonical hash equality passed; see 13-RESOURCE-EVIDENCE.md"
        status: pass
    human_judgment: false
  - id: D7
    description: Libretro serialization callbacks expose the shared state contract with truthful feature declarations.
    requirement: FRONT-01
    verification:
      - kind: integration
        ref: "libretro.host.state passed as part of the complete native and PowerPC/QEMU inventories; see 13-RESOURCE-EVIDENCE.md"
        status: pass
    human_judgment: false
duration: 20min
completed: 2026-11-02
status: complete
---

# Phase 13 Plan 08: Complete State Contract and Acceptance Evidence Summary

**State documentation and maximum-topology restore coverage are complete; full native, PowerPC/QEMU, release, equality, and sanitizer gates passed on the final source commit.**

## Performance

- **Duration:** 20 minutes
- **Started:** 2026-11-01T23:10:56Z
- **Completed:** 2026-11-02T00:07:31Z
- **Tasks:** 3/3, with Task 3 continued after owner authorization and closed with complete local evidence
- **Files modified:** 9 planned source and documentation deliverables

## Accomplishments

- Reconciled the public state docs and settled preparation text with the shared little-endian format, SHA-256 machine identity, CRC-32 corruption check, producer-derived clock validation, and core-owned pending input.
- Extended `core.state.topologies` to check public APPLY and CRC-correct late rejection on the largest generated state, including allocation/free counters.
- Recorded complete-work byte counts, local runtime/stack/object measurements, architecture review, and exact-source native/PowerPC/QEMU and sanitizer receipts in [13-RESOURCE-EVIDENCE.md](./13-RESOURCE-EVIDENCE.md).

## Task Commits

1. **Task 1: Reconcile user and preparation contracts** — `8dd36f4` (`docs(13-08): reconcile portable state contracts`)
2. **Task 2: Measure completed serialization work and run local gates** — `d37f4cc` (`test(13-08): measure maximum state restore work`)
3. **Task 3: Record exact-source environment evidence** — `1eb79f8` (`docs(13-08): record state work and evidence checkpoint`) and `01adc1a` (`docs(13-08): retain unresolved state assumptions`); final local run and receipts are recorded below and in the resource evidence.

## Files Created or Modified

- `docs/embedding.md`, `docs/runner.md`, `docs/libretro.md` — public format, identity, corruption, clock, and interface behavior.
- `.planning/preparation/ARCHITECTURE.md`, `.planning/preparation/DECISIONS.md`, `.planning/preparation/HOSTS-AND-INTEGRATION.md` — current state model and accurately labeled evidence boundaries.
- `tests/core/test_state.c` — maximum generated topology apply, late rejection, and no-allocation checks.
- `docs/testing.md` — state coverage in the existing test inventory.
- `.planning/phases/13-portable-save-load-across-three-interfaces/13-RESOURCE-EVIDENCE.md` — resource measurements and completed local native/PowerPC/QEMU and sanitizer receipts.

## Decisions Made

- Retained the existing shared little-endian visitor and the distinction between corruption detection and machine identity.
- Kept the 307,932-byte conservative ceiling labeled as synthetic size/write/validate coverage; it does not prove public apply for a generated machine.
- Retained unresolved design assumptions as explicit evidence limits while completing the required executed native/PowerPC/QEMU equality and full inventories locally.

## Deviations from Plan

The plan's evidence checkpoint was first used when the host lacked the required toolchain and the cached CI build had tests disabled. After the owner authorized the documented local Linux setup, Task 3 continued and completed against the final source without changing product scope.

## Issues Encountered

The first direct late-field test attempt targeted the payload offset plus the header twice, so the raw battery byte remained valid. The direct run caught it; the test now mutates the final fixed field at its encoded public offset, repairs the envelope CRC, and passes. The docs budget initially rejected an overlong testing note; it was folded into the existing inventory rows, and the final budget check passes.

## Authorized local Linux execution

The owner authorized the documented local container route. The cached Ubuntu 26.04 ARM64 image ran on Docker Desktop's local Linux VM; no separate VM application was installed. Source commit `13bb1af4e7b63fb727854d42ad4431091f2083cc` passed complete native and PowerPC/QEMU normal and release inventories, exact-source equality, and the full ASan/UBSan inventory. Detailed commands, exits, image digest, and receipt paths are in [13-RESOURCE-EVIDENCE.md](./13-RESOURCE-EVIDENCE.md). This is Linux/QEMU execution evidence, not physical PowerPC hardware evidence. No hosted action was published, and Phase 12 allowances were not renewed.

## Next Phase Readiness

Plan 13-08's local deliverables and acceptance receipts are complete on final source `13bb1af4e7b63fb727854d42ad4431091f2083cc`. `STATE-02`'s separately recorded design assumptions remain unclassified where evidence does not establish them; the phase verifier must assess the requirement against the code and full test results without claiming those assumptions as resolved. No RetroArch run, publication, dispatch, or Phase 12 allowance renewal occurred. `.planning/WINDOWS.md` was not appended or retried because of the known mismatch instruction.

## Self-Check: PASSED

The summary and resource evidence files exist, all four plan commits are ancestors of the current branch head, and the persisted plan ledger measures four commits through `plan_head_after`. The pre-existing `.planning/STATE.md` change was not staged or committed; `.planning/ROADMAP.md` was not edited.
