---
phase: 13-portable-save-load-across-three-interfaces
plan: "01"
subsystem: core-state
tags: [c, state, serialization, deterministic, tests]
requires: []
provides:
  - Private typed state payload visitor for installed machine state and memory spans.
  - Loaded-machine continuation tracer with bounded, allocation-free apply.
  - Field-to-producer ledger for the fixed layout and current validation domains.
affects: [phase-13-state-envelope, state-api, runner, libretro]
plan_head_before: 2a3c82aa764fb2b86dc4e1e937509c41de0a53ed
plan_head_after: 9a15d9a26a075a518a2847cbe7d594c735824d16
commits: 8
actuals:
  tokens: 25912
  tasks: 2
  commits: 8
tech-stack:
  added: []
  patterns:
    - One typed owner-ordered visitor for exact little-endian size, write, validate, and apply walks.
    - Installed lengths and fixed array bounds drive traversal; payload values do not select work.
key-files:
  created:
    - src/state.c
    - tests/core/test_state.c
    - .planning/phases/13-portable-save-load-across-three-interfaces/13-FIELD-PROOF.md
    - .planning/phases/13-portable-save-load-across-three-interfaces/13-01-SUMMARY.md
  modified:
    - src/internal.h
    - src/ports.c
    - CMakeLists.txt
    - tests/core/test_core.c
    - tests/core/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
key-decisions:
  - Keep state traversal private until the envelope, effective identity, public API, and interface plans are complete.
  - Use the design table's 10908-byte private payload plus the separate 64-byte envelope; the phrase 10972+C+E+B denotes the later total format length.
  - Preserve raw POKEY IRQ bytes and valid UINT64_MAX scheduler sentinels without adding a global time cap.
patterns-established:
  - Apply validates the full payload before in-place mutation, preserves live span addresses and battery generation semantics, then rebuilds only derived indexes.
  - Inactive MARIA statistics and PCM FIFO cells serialize as canonical zero; other serialized bytes retain their producer values.
requirements-completed: []
coverage:
  - id: D1
    description: Private payload writes canonical bytes and restores a loaded generated machine for the same next video/audio observations without allocation.
    requirement: STATE-01 (plan slice only; the phase requirement remains pending)
    verification:
      - kind: e2e
        ref: "core.no_alloc_after_load in /private/tmp/muhtari-state-13-01-harness/build"
        status: pass
      - kind: unit
        ref: "core.state.payload in /private/tmp/muhtari-state-13-01-harness/build"
        status: pass
    human_judgment: false
  - id: D2
    description: Private validation rejects invalid field domains, aliases, and lifecycle changes without partially applying state.
    requirement: STATE-02 (private field slice only; phase assumption remains unclassified)
    verification:
      - kind: unit
        ref: "core.state.fields, core.state.aliases, core.state.lifecycle in /private/tmp/muhtari-state-13-01-harness/build"
        status: pass
    human_judgment: false
  - id: D3
    description: Installed cart RAM, expansion RAM, and battery spans use exact topology lengths, including the static payload ceiling.
    requirement: STATE-04 (plan slice only; the phase requirement remains pending)
    verification:
      - kind: integration
        ref: "core.state.topologies, core.state.battery in /private/tmp/muhtari-state-13-01-harness/build"
        status: pass
    human_judgment: false
  - id: D4
    description: The packed field ledger maps every design inventory leaf to its producer, current accepted domain, canonical rule, and hostile/control evidence.
    requirement: STATE-01, STATE-02, STATE-04 (plan-scoped evidence; phase requirements remain pending)
    verification:
      - kind: other
        ref: ".planning/phases/13-portable-save-load-across-three-interfaces/13-FIELD-PROOF.md"
        status: pass
    human_judgment: false
metrics:
  duration: 35m
  completed: 2026-11-01
  status: complete
---

# Phase 13 Plan 01: Private State Payload Summary

**A private typed payload visitor restores loaded generated machines with canonical bytes, field validation, fixed topology bounds, and no allocations.**

## Performance

- **Duration:** 35 minutes
- **Started:** 2026-11-01T19:34:05Z
- **Completed:** 2026-11-01T20:10:19Z
- **Tasks:** 2
- **Files modified or created:** 11 plan artifacts, including this summary

## Accomplishments

- Added one private owner-ordered visitor for payload sizing, little-endian writes, hostile-field validation, and allocation-free in-place apply. It preserves installed memory span addresses and reconstructs derived input, MARIA, and bus indexes after applying all fields.
- Extended the loaded-machine tracer to save, advance, restore, and replay the next frame. It checks video, PCM, allocator activity, mailbox observation, ROM storage, and independent instance state.
- Added six registered state cases for payload canonicalization, hostile fields, aliases, lifecycle, battery, and topologies, plus the state case inventory entries for CI and wasm32.
- Added the producer proof ledger for the complete fixed field table and dynamic spans. A generated loaded topology reached 209628 bytes including the future header; a synthetic normalized installer shape exercised the 307932-byte static ceiling.

## Task Commits

Task 1 commits:

1. 403bda9 — add the loaded continuation assertion (RED)
2. 1df469b — implement the private payload visitor (GREEN)

Task 2 commits:

3. 124fdb3 — add hostile private field assertions (RED)
4. 52b301c — validate private field domains (GREEN)
5. 2337959 — cover remaining state field boundaries (RED)
6. c98bc15 — preserve POKEY array field ordering (GREEN)
7. 39c056d — exercise the static payload ceiling
8. 9a15d9a — add the producer proof ledger

The summary metadata is committed separately after this file is written. The measured plan commit range at summary creation is recorded in frontmatter.

## Files Created and Modified

- src/state.c — private fixed-layout typed state visitor and validation/apply logic.
- src/internal.h — private state visitor declarations and state-related layout declarations.
- src/ports.c — narrow derived-state rearm hook owned by the ports module.
- CMakeLists.txt — includes the private state implementation in the core objects.
- tests/core/test_core.c — loaded-machine continuation and allocation tracer assertions.
- tests/core/test_state.c — payload, hostile value, lifecycle, battery, alias, and topology cases.
- tests/core/tests.cmake — registers the six state cases.
- tests/inventory/ci.txt and tests/inventory/wasm32.txt — includes the six registered cases.
- .planning/phases/13-portable-save-load-across-three-interfaces/13-FIELD-PROOF.md — producer and validation ledger for every design inventory leaf.

## Decisions Made

- The exact design table separates the 64-byte envelope from the private visitor. The visitor is 10908 fixed bytes plus C, E, and B; 10972+C+E+B is the full future format total including the envelope.
- Raw POKEY IRQ-enable/register bytes retain all eight bits, and valid UINT64_MAX event sentinels remain unchanged.
- No public serialization claim is made by this plan. The envelope, checksum, effective machine identity, public API, runner, and libretro paths are later work.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Bug] Preserve unrestricted raw POKEY IRQ bytes**
- **Found during:** Task 2
- **Issue:** The first field validator rejected high bits in irqen/irq_enable even though the raw producer state preserves the full register byte.
- **Fix:** Kept the complete bytes and focused semantic validation on producer-defined boolean/derived fields; added a 0xff retention assertion.
- **Files modified:** src/state.c, tests/core/test_state.c
- **Verification:** The new RED case failed before the fix, then all seven scoped CTest cases passed.
- **Committed in:** 52b301c

**2. [Rule 1 - Bug] Match POKEY high-pass array order to the packed table**
- **Found during:** Task 2
- **Issue:** Interleaving each chip's hp_delay and hp_capture fields disagreed with the table, which stores both hp_delay bytes before both hp_capture bytes.
- **Fix:** Changed the visitor ordering and added an offset-specific invalid-field regression at absolute offset 10452.
- **Files modified:** src/state.c, tests/core/test_state.c
- **Verification:** The RED evidence identified the accepted mutation at offset 10452; the fixed visitor rejected it and all seven scoped cases passed.
- **Committed in:** c98bc15

**Total deviations:** 2 auto-fixed issues. The synthetic max-size case and the payload-length wording correction stayed within the plan's explicit topology and exact-layout requirements.

## Verification

- `cmake --build --preset ci` — exit 0.
- `cmake --workflow --preset ci` — configure and build passed; CTest exited 8 at the test step with “No tests were found”. `build/ci/CMakeCache.txt` had `MUHTARI_TESTS:BOOL=OFF`, so the preset had no registered repository tests in this worktree.
- The scoped harness compiled the repository's real `tests/core/test_core.c` and `tests/core/test_state.c` with the project's strict warning flags, then ran the tracer and all six registered state case names: 7/7 passed, exit 0. Receipt: `/private/tmp/muhtari-state-13-01-harness/junit-state-01-final.xml`.
- `git diff --check` — exit 0. All eight task commits passed the configured pre-commit hygiene hook.
- No repository test packages, pinned assets, hosted workflows, ROM files, BIOS files, RetroArch actions, or Phase 12 allowances were used.

## TDD Gate Compliance

- Task 1: RED commit 403bda9, followed by GREEN commit 1df469b; the task's red-evidence checker passed and the loaded continuation passed in the scoped harness.
- Task 2: RED commit 124fdb3, followed by GREEN commit 52b301c; the task's red-evidence checker passed for the raw IRQ-byte fault.
- Task 2 layout regression: RED commit 2337959 identified the incorrect byte mapping at absolute offset 10452; GREEN commit c98bc15 corrected it.
- The red-evidence JSON and JUnit receipts were kept out of the repository; no state bytes or test logs were committed.

## Issues Encountered

The configured CI build cache had `MUHTARI_TESTS=OFF`, and the standard CI workflow therefore stopped at CTest with no tests discovered. The implementation and all seven scoped test cases were still compiled and executed against the repository source in the temporary harness. Full repository inventory execution remains dependent on a CI configuration with `MUHTARI_TESTS=ON` and its pinned test assets available.

The best-effort broken-window entry for that missing CI receipt could not be appended because the existing `.planning/WINDOWS.md` has a rendered row 51 marked open while its fenced JSON source marks the same entry fixed. I left that cross-phase ledger unchanged; its inconsistency predates this plan.

## User Setup Required

None.

## Next Phase Readiness

Plan 13-01 is complete. The next sequential plan can build on the private visitor and field ledger. Public state support remains blocked on the planned envelope/identity/API work and the remaining operation-by-operation timestamp overflow proof; STATE-02 remains unclassified. No user action is needed for this plan.

---
*Phase: 13-portable-save-load-across-three-interfaces*
*Plan: 01*
*Completed: 2026-11-01*

## Self-Check: PASSED

The summary and field proof files exist, all eight plan commits are ancestors of the current branch HEAD, and `git diff --check` exits 0.
