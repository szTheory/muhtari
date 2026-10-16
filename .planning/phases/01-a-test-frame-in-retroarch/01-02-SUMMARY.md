---
phase: 01-a-test-frame-in-retroarch
plan: 02
subsystem: testing
tags: [ctest, cmake, inventory, test-card, size-tags, allocator, api-v1]

requires:
  - phase: 01-01
    provides: "The 0.1.0 header, core, muhtari-run --hash-frame and the ci preset"
provides:
  - "cmake/MuhtariTests.cmake: muhtari_add_test, MUHTARI_TIMEOUT_SCALE, and a deferred check that the registered tests equal tests/inventory/<MUHTARI_INVENTORY>.txt"
  - "tests/inventory/ci.txt: the 13 registered tests as sorted name<TAB>labels lines"
  - "tests/check.h: the comparison macros, no framework"
  - "muhtari-test-core: one executable running 12 core cases by name"
  - "tests/api/v1.c: the frozen API v1 program"
  - "cmake --workflow --preset ci passes with 13 tests"
affects: [01-03, 01-04, 01-05, 01-06, 01-07, 01-08, 01-09]

actuals:
  tokens: 8300
  tasks: 2
  commits: 2
plan_head_before: a9de18d1fd63e2cd21e0ae6d5900bdaef7e73082
plan_head_after: dd5722c129ce8c0fb2244c594bce2595647335a8

tech-stack:
  added: [CTest]
  patterns:
    - "Tests register through muhtari_add_test in tests/<subsystem>/tests.cmake, include()d from tests/tests.cmake, never add_subdirectory"
    - "Inventory lines are name<TAB>labels with labels sorted and comma-separated, so the file reads as a CMake list without semicolons"
    - "A tests.cmake file can also be its own -P check script, branching on CMAKE_SCRIPT_MODE_FILE"
    - "Each test program defines its own allocator; the counting allocator lives in test_core.c"

key-files:
  created:
    - cmake/MuhtariTests.cmake
    - tests/tests.cmake
    - tests/check.h
    - tests/inventory/ci.txt
    - tests/core/tests.cmake
    - tests/core/test_core.c
    - tests/runner/tests.cmake
    - tests/api/v1.c
  modified:
    - CMakeLists.txt

key-decisions:
  - "The inventory compares name and labels, not names alone, so a test moved to another lane's label also fails configure"
  - "The inventory file must itself be sorted (strcmp order); configure says so if it is not"
  - "runner.hash is the -P script mode of tests/runner/tests.cmake, so there is no separate script file"
  - "The core needed no change: plan 01 already met the contract, and mutation runs stood in for a failing RED"

patterns-established:
  - "Every check failure prints expected and actual values"
  - "Test names are <subsystem>.<behaviour>; the core executable takes the name after core. as its argument"

requirements-completed: []

coverage:
  - id: D1
    description: "cmake --workflow --preset ci configures, builds and passes all 13 registered tests"
    requirement: FRAME-01
    verification:
      - kind: integration
        ref: "cmake --workflow --preset ci"
        status: pass
    human_judgment: false
  - id: D2
    description: "Configure fails when the registered tests differ from tests/inventory/ci.txt, compared as sorted sets"
    requirement: FRAME-01
    verification:
      - kind: other
        ref: "append a dummy line to tests/inventory/ci.txt, then cmake --preset ci exits 1 naming it; drop runner.hash, then it exits 1 naming runner.hash as unlisted; swap the include order in tests/tests.cmake, then it exits 0"
        status: pass
    human_judgment: false
  - id: D3
    description: "The library renders the pinned NTSC test card with all 256 colours, static over 600 frames, with every rule edge correct"
    requirement: FRAME-03
    verification:
      - kind: unit
        ref: "tests/core/test_core.c#testcard.pinned_hash"
        status: pass
      - kind: unit
        ref: "tests/core/test_core.c#testcard.all_colours"
        status: pass
      - kind: unit
        ref: "tests/core/test_core.c#testcard.static"
        status: pass
      - kind: unit
        ref: "tests/core/test_core.c#testcard.boundaries"
        status: pass
    human_judgment: false
  - id: D4
    description: "muhtari-run --frames 1 --hash-frame 1 exits 0 and prints exactly the pinned line"
    requirement: FRAME-03
    verification:
      - kind: e2e
        ref: "tests/runner/tests.cmake#runner.hash"
        status: pass
    human_judgment: false
  - id: D5
    description: "Pitch 336 leaves row padding untouched; refused run_frame calls return their status, write audio_count and change no state"
    requirement: FRAME-03
    verification:
      - kind: unit
        ref: "tests/core/test_core.c#frame.pitch_336"
        status: pass
      - kind: unit
        ref: "tests/core/test_core.c#frame.validate_first"
        status: pass
    human_judgment: false
  - id: D6
    description: "Silence is 800 or 801 zero samples per frame, starting 800, 801, with no drift over 13,125 frames"
    requirement: FRAME-03
    verification:
      - kind: unit
        ref: "tests/core/test_core.c#audio.silence"
        status: pass
    human_judgment: false
  - id: D7
    description: "Size tags, the required allocator, the info constants and the frozen API v1 behave as the header promises"
    requirement: FRAME-01
    verification:
      - kind: unit
        ref: "tests/core/test_core.c#api.size_tags"
        status: pass
      - kind: unit
        ref: "tests/core/test_core.c#create.allocator"
        status: pass
      - kind: unit
        ref: "tests/core/test_core.c#info"
        status: pass
      - kind: unit
        ref: "tests/api/v1.c#abi.api_v1"
        status: pass
    human_judgment: false
  - id: D8
    description: "A counting allocator sees 1 alloc at create, 0 calls in 600 frames and 1 free at destroy"
    requirement: FRAME-09
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -L perf (core.no_alloc_after_load)"
        status: pass
    human_judgment: false

duration: 6min
completed: 2026-10-16
status: complete
---

# Phase 1 Plan 02: Exact-inventory tests and the core contract Summary

**`cmake --workflow --preset ci` now builds and passes 13 CTest tests behind an exact `name<TAB>labels` inventory that fails configure on any drift. The tests pin the NTSC test-card hash in the library and through `muhtari-run`, and they prove pitch, validation, silence, size tags, the allocator, zero allocations after create, and the frozen API v1.**

## Performance

- **Duration:** 6 min
- **Started:** 2026-10-16T14:43:02Z
- **Completed:** 2026-10-16T14:49:00Z
- **Tasks:** 2
- **Files modified:** 9 (8 created, 1 modified)

## Accomplishments
- `cmake/MuhtariTests.cmake` (69 lines) adds `muhtari_add_test(NAME COMMAND LABELS TIMEOUT [COST])`, which scales the timeout by `MUHTARI_TIMEOUT_SCALE`. A `cmake_language(DEFER)` check compares the directory's `TESTS` and labels with `tests/inventory/${MUHTARI_INVENTORY}.txt` as sorted sets and names every missing and unexpected entry.
- `core.testcard.pinned_hash` links `runner/sha256.c` and matches `9eb011e2…`. `runner.hash` requires exit 0 and exactly `frame 1 sha256 9eb011e2…\n`.
- Eleven more tests cover the core: `core.testcard.{all_colours,static,boundaries}`, `core.frame.{pitch_336,validate_first}`, `core.audio.silence`, `core.api.size_tags`, `core.create.allocator`, `core.info`, `core.no_alloc_after_load` (label `perf`) and `abi.api_v1`.
- `tests/api/v1.c` assigns every field of all five structs, calls all five functions and asserts `MUHTARI_API_VERSION == 1` and each status value from 0 to 4. Its header comment marks it as frozen.

## Task Commits

1. **Task 1: End-to-end exact-inventory CTest: pinned hash in the library and through the runner** - `68079ef` (test)
2. **Task 2: Complete the core contract and its tests** - `dd5722c` (test). The core needed no change, so there is no `feat` commit.

## Files Created/Modified
- `cmake/MuhtariTests.cmake` - registration helper and the deferred inventory check
- `CMakeLists.txt` - `MUHTARI_TIMEOUT_SCALE` and `MUHTARI_INVENTORY` cache variables, `enable_testing()`, includes (top-level only)
- `tests/tests.cmake` - includes each subsystem's `tests.cmake`
- `tests/check.h` - `CHECK`, `CHECK_EQ_U64`, `CHECK_EQ_HEX`, `CHECK_DONE`
- `tests/inventory/ci.txt` - the 13 tests, sorted
- `tests/core/tests.cmake` - `muhtari-test-core` and `muhtari-test-api-v1`, 12 registrations
- `tests/core/test_core.c` - 11 cases with their own malloc and counting allocators
- `tests/runner/tests.cmake` - `runner.hash`, and the check itself in `-P` mode
- `tests/api/v1.c` - the frozen API v1 program

## Seed lines used in commit bodies
- `68079ef`: `Seeded from nesturbator 9f9386c661f6 tests/check.h (same author, MIT).` Only the include guard was renamed.
- `tests/core/test_frame.c` and `test_api.c` from the same commit were read for shape only. Their API differs, and no code from them was copied.

## Gate-code line tally
- Gate code: `cmake/MuhtariTests.cmake` 69, the `runner.hash` script block in `tests/runner/tests.cmake` 15, `tests/cmake/*.cmake` none, `scripts/*.sh` 283. Total 367.
- `src/*.c src/*.h`: 315 lines, unchanged by this plan.
- The tenth-of-`src/` ceiling applies from phase 3. These numbers are kept for the trend.

## Evidence (command, exit status)
- `cmake --workflow --preset ci` exited 0: "100% tests passed out of 13". CMake 4.4.3 prints this shorter line instead of "…, 0 tests failed out of 13".
- `printf 'zz.dummy\tcore,portable\n' >> tests/inventory/ci.txt; cmake --preset ci` exited 1, listing `zz.dummy` under "listed but not registered". The file was restored.
- With `runner.hash` removed from the inventory, `cmake --preset ci` exited 1, listing it under "registered but not listed". The file was restored.
- With the includes in `tests/tests.cmake` swapped, `cmake --preset ci` exited 0. The file was restored.
- `cmake -DRUN=/usr/bin/true -P tests/runner/tests.cmake` exited 1, printing the expected and actual output.
- `grep -v '^#' tests/inventory/ci.txt | cut -f1 | sort -c` exited 0, under both `LC_ALL=C` and the default locale. The list equals `ctest --test-dir build/ci -N` (13 names).
- `ctest --test-dir build/ci -L perf` exited 0 (1 test).
- `grep -rnE '\b[DR]-[0-9]{2}\b|FRAME-[0-9]{2}' cmake tests src` exited 1: no planning IDs.
- Mutation runs: each mutant was built with `cmake --build --preset ci` and run with `ctest --test-dir build/ci`, then `src/` was restored with `git checkout -- <file>`:

| Mutation in `src/` | Test that failed |
|---|---|
| rule 3 returns `$07` | `core.testcard.pinned_hash`, `core.testcard.boundaries`, `runner.hash` |
| no `audio_count` write on a capacity error | `core.frame.validate_first` |
| audio remainder committed before the capacity check | `core.frame.validate_first` |
| capacity compared off by one | `core.frame.validate_first` |
| pitch 65537 accepted | `core.frame.validate_first` |
| pitch 319 accepted | `core.frame.validate_first` |
| rows written across the whole pitch | `core.frame.pitch_336` |
| one extra sample zeroed | `core.audio.silence` |
| non-zero tail bytes not checked | `core.api.size_tags` |
| bytes past the struct written to `$FF` | `core.api.size_tags` |
| size one below the first-release size accepted | `core.api.size_tags` |
| `free` given size 0 | `core.create.allocator` |
| `free` hook not required | `core.create.allocator` |
| `par_den` set to 6 | `core.info` |
| one allocation per frame | `core.no_alloc_after_load` |

## Decisions Made
- The inventory compares labels too. A test is only "on every lane" if it carries the label each lane's test step selects, so a label change is drift just as a missing name is.
- Labels are joined with commas in the file. `file(STRINGS)` would split a `core;portable` line into two list items.
- The inventory file must be sorted, or configure says so. ENGINEERING §3 asks for sorted names, and this keeps the diffs of later additions clean.
- `runner.hash` is the `-P` mode of `tests/runner/tests.cmake` itself, which matches the plan's file list (no separate script).

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Bug] The pitch-65537 refusal was only caught by a crash**
- **Found during:** Task 2 (mutation runs)
- **Issue:** The first version passed the 77,440-byte frame buffer with pitch 65537. A core that wrongly accepted that pitch overran it, so the test failed with a segfault rather than a clean status mismatch, and the failure could vary by lane.
- **Fix:** The test allocates 65537 × 242 bytes for the 65537 case and reuses that buffer to show pitch 65536 is accepted.
- **Files modified:** tests/core/test_core.c
- **Verification:** The pitch-65537 mutant now fails `core.frame.validate_first` with "Failed", not "Exception".
- **Committed in:** dd5722c

### TDD note
- Task 2 is `tdd="true"`, but every test passed on first run, because plan 01 had already implemented the contract. Following the TDD reference ("feature may already exist, investigate"), the 15 mutation runs above confirm that each test fails when its behaviour breaks. With no core change to make, there is one `test(01-02)` commit and no `feat` commit. `src/instance.c`, `src/frame.c` and `src/internal.h` were in the plan's file list but did not change.

---

**Total deviations:** 1 auto-fixed (1 bug in a test), plus the TDD note
**Impact on plan:** None on scope. Every planned test exists with its planned name and labels.

## Issues Encountered
- `clang-format` is not installed on this Mac, so the new C files were not format-checked locally. They follow the 01-01 style by hand. `hygiene.format` (clang-format 21) checks them in CI once that lane lands.
- CMake 4.4.3's CTest summary reads "100% tests passed out of N". The plan's `fails_when` text expects the older "…, 0 tests failed out of N" wording. The exit status is the actual signal.

## User Setup Required

None - no external service configuration required.

## Next Phase Readiness
- Every later plan registers its tests with `muhtari_add_test` in `tests/<subsystem>/tests.cmake` and adds a sorted line to `tests/inventory/ci.txt`.
- `check_junit.cmake`, `expect_fail.cmake` and the other inventories (`nofp`, `hygiene`, `wasm32`) are still to be written. `MUHTARI_INVENTORY` already selects the file.

---
*Phase: 01-a-test-frame-in-retroarch*
*Completed: 2026-10-16*

## Self-Check: PASSED

- All 8 created files exist, and `CMakeLists.txt` is modified. Commits `68079ef` and `dd5722c` are ancestors of HEAD.
- Plan verification re-run: `cmake --workflow --preset ci` exited 0 with 13 tests.
