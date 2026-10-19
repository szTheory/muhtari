---
phase: 02-the-cpu-matches-the-public-vectors
plan: 10
subsystem: cart
tags: [probe, cartdb, sha1, link-time-substitution, activision, multi-chip, 2600]

requires:
  - phase: 02-the-cpu-matches-the-public-vectors
    provides: plan 9's muhtari__cart_identify with the tables as arguments, tests/cart/images.{c,h} and muhtari-test-cart linked from the core's objects
provides:
  - tests/cart/test_cartdb.c - the test hash list, defining the bundled table names, linked into muhtari-test-cart in place of the muhtari_cartdb objects
  - src/cart.c cart_lookup - the whole-body pass, one pass per slice of each layout whose sizes sum to the body, and the half-swapped pass for a 128 KiB miss
  - tests/cmake/no_test_entries.cmake and cart.identify.bundled_has_no_test_entries with its self test
  - seven cart.identify.* tests and the "Hash list" section of docs/hardware/cartridge.md
affects: [the sample tests that use the test list's 7800basic key, the runner's --probe, the loader fuzz target, the internal loader]

actuals:
  tokens: 10761
  tasks: 2
  commits: 2
plan_head_before: ce8431272a0aadecf898e1d5d6b404d266e416a3
plan_head_after: 9e887f293ede7f7f0e60a5a7222d0df023c59c6b

tech-stack:
  added: []
  patterns:
    - "A test table defines the bundled table's names and is linked in its place; a CMake -P check proves no test key reaches the bundled file"
    - "A derived hash (swapped halves) is fed to SHA-1 in pieces in the derived order, never through a copy"

key-files:
  created:
    - tests/cart/test_cartdb.c
    - tests/cmake/no_test_entries.cmake
  modified:
    - src/cart.c
    - tests/cart/test_cart.c
    - tests/cart/images.c
    - tests/cart/images.h
    - tests/cart/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - docs/hardware/cartridge.md
    - .planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md

key-decisions:
  - "A header hash match reports source hash (the list identified the board), keeping the HEADER flag"
  - "The header names no region when TV bit 2 (multi-region) is set; only then does a hash match take the entry's region. Otherwise TV bit 0 decides, as before"
  - "The half-swapped pass runs only for a 128 KiB body that missed the whole-body and slice passes, and only an Activision entry may match it"
  - "Slice matching: the first slice's digest selects candidate keys; an entry matches when its layout is this layout and every slice's digest is keyed as that entry's part for the slice, so a chip mirrored into two slices works"
  - "The leak check compares full 20-byte rows and every row's first 8 bytes, so a test key pasted as a 7800 key or as a 2600 prefix both fail"

patterns-established:
  - "Synthetic list keys are named by seed in images.h (IMAGES_SEED_*) and computed outside the tree by a scratch program built from images.c and src/sha1.c"

requirements-completed: []

coverage:
  - id: D1
    description: "muhtari-test-cart links tests/cart/test_cartdb.c in place of the muhtari_cartdb objects; the test list holds the 7800basic sample's key 1aa9559b74eab35d4c5443b56bc4dfd7a445ac4a and synthetic keys, and the eight plan 9 cart.probe cases still pass on it"
    requirement: CPU-05
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^cart\\.' --output-on-failure"
        status: pass
      - kind: other
        ref: "grep -v '^ *#' tests/cart/tests.cmake | grep -n 'TARGET_OBJECTS:muhtari_cartdb' (exit 1, no line)"
        status: pass
    human_judgment: false
  - id: D2
    description: "A hash match overrides board, RAM and sound, sets HEADER_CORRECTED only when the header disagreed, reports source hash, keeps the header's ports, and keeps the header's region unless TV bit 2 says multi-region"
    requirement: CPU-05
    verification:
      - kind: unit
        ref: "tests/cart/test_cart.c#identify.hash_override"
        status: pass
    human_judgment: false
  - id: D3
    description: "A multi-chip body is identified by its slices in order, after a same-sized layout with no entry, including a chip mirrored into two slices; one changed byte in any slice loses the match"
    requirement: CPU-05
    verification:
      - kind: unit
        ref: "tests/cart/test_cart.c#identify.slices"
        status: pass
      - kind: other
        ref: "mutant: stop after the first layout whose sizes sum to the body; identify.slices fails (source 3 != 2); cart.c restored, cmp exit 0"
        status: pass
    human_judgment: false
  - id: D4
    description: "A 128 KiB Activision body in the other layout matches through the half-swapped pass, headerless and headered, as ERR_UNSUPPORTED_BOARD with board ACTIVISION; a 64 KiB body whose swapped bytes are listed gets no swapped pass"
    requirement: CPU-05
    verification:
      - kind: unit
        ref: "tests/cart/test_cart.c#identify.activision_swap"
        status: pass
      - kind: other
        ref: "mutant: swapped pass for any size; identify.activision_swap fails (64 KiB: expected status 0, got 9); cart.c restored, cmp exit 0"
        status: pass
    human_judgment: false
  - id: D5
    description: "A 2600 prefix hit is ERR_UNSUPPORTED_2600 though its footer passes; a not-a-cartridge hit is ERR_INVALID_IMAGE with out untouched, headered and headerless; a headerless hit's ports follow the light-gun flag"
    requirement: CPU-05
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^cart\\.identify\\.(2600_prefix|not_a_cartridge|headerless_ports)$'"
        status: pass
    human_judgment: false
  - id: D6
    description: "cart.identify.bundled_has_no_test_entries passes on src/cartdb.c and its self test fails, naming the key, on a copy with the test list's first key appended"
    requirement: CPU-05
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^cart\\.identify\\.bundled_has_no_test_entries'"
        status: pass
    human_judgment: false
  - id: D7
    description: "docs/hardware/cartridge.md has a Hash list section (contents, bounded passes, run-time slices and swap, precedence, the 2600 prefix trade-off, not-a-cartridge entries); the cartridge notes' section 1.4 says run time, with its updated date"
    requirement: CPU-05
    verification:
      - kind: other
        ref: "grep -n '^## Hash list' docs/hardware/cartridge.md (line 133); grep -n -i 'run time' A7800-CARTRIDGES-AND-PERIPHERALS.md (line 78, section 1.4)"
        status: pass
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^docs\\.budgets$'"
        status: pass
    human_judgment: false
  - id: D8
    description: "The probe PR's head commit is green on every hosted leg"
    requirement: CPU-05
    verification:
      - kind: integration
        ref: "https://github.com/szTheory/muhtari/actions/runs/39190472256"
        status: pass
    human_judgment: false

duration: 11min
completed: 2026-10-19
status: complete
---

# Phase 2 Plan 10: Table-driven identification through a test list Summary

**The probe now hashes each slice of a multi-chip layout and, for a 128 KiB miss, the body with each bank's 8 KiB
halves swapped, all as SHA-1 passes with no copy. Every table path runs against `tests/cart/test_cartdb.c`, which is
linked in place of the bundled lists, and a CMake check fails if any of its keys shows up in `src/cartdb.c`.**

## Performance

- **Duration:** 11 min
- **Started:** 2026-10-19T02:24:18Z
- **Completed:** 2026-10-19T02:35:04Z
- **Tasks:** 2
- **Files modified:** 11

## Accomplishments

- **Test list, substituted at link time.**
  - `tests/cart/test_cartdb.c` defines the same `muhtari__cartdb_*` names and counts as `src/cartdb.c`, in the same
    row form and order: 11 keys, 9 entries, 3 layouts and one 2600 prefix.
  - `muhtari-test-cart` builds from `muhtari_objs`, `muhtari_cpu` and the test list. The shipped archive and runner
    still link `muhtari_cartdb`.
  - The entries cover:
    - the stripped 7800basic `simple` sample (linear, NTSC);
    - SuperGame with RAM (PAL, light gun) under a header that says linear;
    - a light-gun entry and a plain entry;
    - two layouts: 16 + 32 KiB, and 16 KiB plus one 8 KiB chip mirrored twice. A 32 + 16 KiB decoy layout has no
      entry;
    - an Activision entry for a 128 KiB body, and an Activision decoy keyed on a swapped 64 KiB body;
    - a not-a-cartridge entry;
    - a 2600 prefix for an image whose footer passes.
- **Lookup (`cart_lookup` in `src/cart.c`).** It makes these passes in order, stopping at the first match:
  1. the whole body;
  2. each slice of every layout whose sizes sum to the body;
  3. for a 128 KiB miss, the bank halves swapped, fed to `muhtari__sha1_update` as [8K, 16K) then [0, 8K).

  The worst case is a fixed handful of passes over at most `MUHTARI_IMAGE_SIZE_MAX` bytes. The tables now travel as
  a stack `struct cart_tables`, never a global, so `abi.globals` is unaffected.
- **Precedence.**
  - A header hash match reports `MUHTARI_SOURCE_HASH`. The ports stay the header's.
  - The region stays the header's unless TV bit 2 (multi-region) leaves it naming none.
- **Tests.** `cart.identify.hash_override`, `.slices`, `.activision_swap`, `.2600_prefix`, `.not_a_cartridge`,
  `.headerless_ports`, `.bundled_has_no_test_entries` and `.bundled_has_no_test_entries.self_test`, all labelled
  `cart;portable`, are in the ci and wasm32 inventories.

## Recorded one-off checks (AGENTS rule 6)

- **Synthetic SHA-1s.** A scratch program outside the tree computed and sorted the keys, exit 0:
  `cc -std=c17 -Itests/cart -Isrc keys.c tests/cart/images.c src/sha1.c -o keys && ./keys`.
  - The program builds each `images_seeded` / `images_mirrored` body and hashes the slices.
  - It swaps the 64 KiB decoy before hashing it.
  - It prints the rows sorted by (SHA-1, entry).
- `cmake --workflow --preset ci`: exit 0, 385 tests.
- `cmake --workflow --preset hygiene`: exit 0, 22 tests.
- **Mutation runs on `src/cart.c`**, each restored and checked with `cmp` (exit 0):
  - The swapped pass runs for any size: `identify.activision_swap` fails.
  - The lookup stops after the first layout of the body's size: `identify.slices` fails.
- **Hosted run on head `9e887f2`:** https://github.com/szTheory/muhtari/actions/runs/39190472256.
  - `gh run watch --exit-status` exited 0.
  - The conclusion was `success`, and so was every job, including build on all six legs, nofp on both, be32, wasm32,
    asan and retroarch.

## Task Commits

1. **Task 1: The test list substituted at link time, cart.identify.hash_override** - `069b660` (feat)
2. **Task 2: Slices, the Activision swap, 2600 prefixes, not-a-cartridge entries, headerless ports, the leak check** - `9e887f2` (feat)

## Files Created/Modified

- `tests/cart/test_cartdb.c`: the test hash list (new)
- `tests/cmake/no_test_entries.cmake`: the leak check (new)
- `src/cart.c`: `cart_tables`, `cart_first_key`, `cart_has_part`, `cart_find_slices`, `cart_lookup`,
  `cart_entry_region`; the header match's source and region
- `tests/cart/images.{c,h}`: `IMAGES_SEED_*`, `images_seeded`, `images_mirrored`, `images_swap_halves`
- `tests/cart/test_cart.c`: the six identify cases
- `tests/cart/tests.cmake`: the substitution, the cases, the leak check and its self test
- `tests/inventory/{ci,wasm32}.txt`: eight names
- `docs/hardware/cartridge.md`: the Hash list section, and the probe order points to it
- `.planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md`: section 1.4 now says run time, and `updated` is
  2026-10-19

## Decisions Made

See `key-decisions` above.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 3 - Blocking] The header match's source and region needed a code change in Task 1**
- **Found during:** Task 1
- **Issue:** Task 1's file list does not name `src/cart.c`, but `cart.identify.hash_override` requires source hash
  and the entry's region for a header that names none. Plan 9's code kept source header and never took an entry's
  region under a header. RED: `out.source == MUHTARI_SOURCE_HASH failed: 1 != 2` and
  `out.region == MUHTARI_REGION_PAL failed: 1 != 2`.
- **Fix:** On a match, `cart_header` sets the source to hash. It takes `cart_entry_region` (factored out of the
  headerless path) when TV bit 2 is set. The probe-order line in `docs/hardware/cartridge.md` now says so.
- **Files modified:** src/cart.c, docs/hardware/cartridge.md
- **Committed in:** 069b660

**2. [Rule 2 - Missing critical] A mirrored-chip layout and a decoy layout in the test list**
- **Found during:** Task 1
- **Issue:** The plan names one two-part layout. The bundled table has a layout with a repeated part
  (16 + 8 + 8 KiB, parts 0, 1, 1), and layouts that share a body size (144 KiB, 32 KiB). One plain layout would
  leave both untested.
- **Fix:** The list gains the mirrored layout and a same-sized decoy with no entry, and `identify.slices` covers
  both. The second mutant above shows the decoy catching an early stop.
- **Committed in:** 069b660, 9e887f2

---

**Total deviations:** 2 auto-fixed (1 Rule 3, 1 Rule 2). **Impact on plan:** none on scope.

## TDD Gate Compliance

Task 2 carries `tdd="true"`, and `workflow.tdd_mode` is false.

- **RED.** The five identify cases ran against Task 1's `cart.c`.
  - `cart.identify.slices` failed on its planned assertions, for example
    `out.source == MUHTARI_SOURCE_HASH failed: 3 != 2` and `out.sound == MUHTARI_SOUND_POKEY_450 failed: 0 != 2`.
  - `cart.identify.activision_swap` failed with `swapped layout: expected status 9, got 5`.
  - `2600_prefix`, `not_a_cartridge` and `headerless_ports` passed at RED. They exercise single-chip lookup paths
    plan 9 built, now driven by real test-list entries instead of one-entry lists.
  - The leak check and its self test passed at RED; the self test is the negative control.
- **Semantic assessment:** each failing target ran and failed on the field or status it exists for. There was no
  build, load or fixture fault.
- **GREEN:** all 385 tests pass.
- **Commits:** following plans 5 to 9, each task's tests and implementation landed in one `feat(02-10)` commit, and
  there is no separate `test(02-10)` commit.

## Issues Encountered

None.

## Known Stubs

None. Plan 9's stub (`muhtari__cart_identify` ignoring `layouts`) is closed: `.planning/WINDOWS.md` entry 1 is
marked fixed.

## User Setup Required

None.

## Next Phase Readiness

- The sample tests can probe the stripped `simple` sample through `muhtari-test-cart`'s list, or through a runner
  linked with `tests/cart/test_cartdb.c`.
- Probe PR #9 stays draft.

## Self-Check: PASSED

- Files present: tests/cart/test_cartdb.c and tests/cmake/no_test_entries.cmake.
- Commits 069b660 and 9e887f2 are ancestors of HEAD, and `git rev-list --count ce84312..HEAD` is 2.

---
*Phase: 02-the-cpu-matches-the-public-vectors*
*Completed: 2026-10-19*
