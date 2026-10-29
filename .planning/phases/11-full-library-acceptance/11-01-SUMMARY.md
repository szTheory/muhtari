---
phase: 11-full-library-acceptance
plan: "01"
subsystem: compat-ledger
tags: [ledger, pin-recipe, predicate, pal, diagnostics, cmake]
status: complete
requires:
  - phase: 10-homebrew-expansion-memory
    provides: Owned retail-board diagnostics (activision, absolute, fractalus) and the library preset
provides:
  - A pin recipe that runs on the committed ledger, skips mailbox@ rows, passes bare ids through and can check its own output
  - Three protected tier-pr title@120 rows for the owned retail-board diagnostics, passing on the fast boot and through OpenBIOS
  - A predicate that hashes and scans the loaded region's frame, so PAL images evaluate on both paths
  - An honest skipped row for the banksets sample
affects: [11-02, 11-03, 11-04, 11-05, 11-06, 11-07, 11-08, compat-ledger]
tech-stack:
  added: []
  patterns:
    - Recipe checks run the real script on build-tree copies of the committed ledger and compare with the source file
key-files:
  created: []
  modified:
    - tests/compat/pin.cmake
    - tests/compat/predicate.c
    - tests/compat/tests.cmake
    - tests/compat/ledger.csv
    - tests/inventory/library.txt
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - tests/built/expected.txt
    - docs/emulation-tests.md
    - docs/compatibility.md
key-decisions:
  - The recipe copies a row whose id has no slash, and any mailbox@ row, unchanged; only title@ and boots@ rows are evaluated.
  - The new rows were pinned without COMMIT and DATE so the 39 unrelated rows keep their evidence fields; the three new rows got the HEAD short hash and today's date by hand.
  - Hashing uses the loaded region's width x height from the instance, so no NTSC title hash moved.
requirements-completed: []
coverage:
  - id: C1
    description: The pin recipe runs to completion on the committed ledger, leaves mailbox@ rows byte-identical and passes bare ids through
    requirement: COMP-03
    verification:
      - kind: integration
        ref: title.pin.recipe
        status: pass
      - kind: integration
        ref: title.pin.order
        status: pass
    human_judgment: false
  - id: C2
    description: Recipe order and file rules are enforced, and the enforcement fails on a swapped ledger
    requirement: COMP-03
    verification:
      - kind: integration
        ref: title.pin.order.self_test
        status: pass
    human_judgment: false
  - id: C3
    description: diag/activision, diag/absolute and diag/fractalus are tier-pr title@120 rows judged on both boot paths
    requirement: COMP-02
    verification:
      - kind: e2e
        ref: library.diag.activision, library.diag.absolute, library.diag.fractalus
        status: pass
    human_judgment: false
  - id: C4
    description: The predicate runs a PAL image on both boot paths without a crash and every NTSC title hash is unchanged
    requirement: COMP-03
    verification:
      - kind: integration
        ref: title.pal.fast and title.pal.bios
        status: pass
      - kind: e2e
        ref: MUHTARI_ROM_DIR=build/ci/built cmake --workflow --preset library
        status: pass
    human_judgment: false
  - id: C5
    description: 7800basic-0.41/banksets is skipped, tier local, because the recipe does not assemble it; the generated page and prose agree
    requirement: COMP-03
    verification:
      - kind: unit
        ref: policy.compat_doc_fresh
        status: pass
      - kind: unit
        ref: docs.budgets
        status: pass
    human_judgment: false
  - id: C6
    description: The whole regression gate passes on the plan head
    requirement: COMP-03
    verification:
      - kind: integration
        ref: cmake --workflow --preset ci
        status: pass
    human_judgment: false
actuals:
  tokens: 3000
  tasks: 3
  commits: 3
plan_head_before: 71d81177ec0b4bd00627bf74c67ec4d762b8011a
plan_head_after: f56e1a61d5bd7f4a987c1c3732127c9c9a85cca8
duration: 35m
completed: 2026-10-29
---

# Phase 11 Plan 01: Ledger tooling and owned retail diagnostics Summary

**The pin recipe now runs on the committed ledger, three owned retail-board diagnostics are protected pr rows on both boot paths, and the predicate no longer crashes on PAL images.**

## What changed

- `tests/compat/pin.cmake`: a row whose id has no `/` is copied unchanged before any file rule; the BIOS evaluation runs only for `title@` and `boots@` rows, so `mailbox@` rows are never touched; optional `-DCHECK=<file>` fails with `pin: <LEDGER> differs from <CHECK>`.
- `tests/compat/ledger.csv`: rows `diag/activision`, `diag/absolute`, `diag/fractalus` (board named as the image declares it, `title@120`, pass, pr), filled by the recipe from sha1/sha256 tool output; `7800basic-0.41/banksets` is now board `bankset`, `boots@120;why=recipe-does-not-assemble`, skipped, local. The first recipe run moved the four `mailbox@` rows into pin order once (the committed file was not in pin order); their text is unchanged.
- `tests/compat/predicate.c`: the frame buffer holds the PAL frame; the hash, the single-colour check and the DMA line scan are bounded by the loaded region's width x height and line count.
- `tests/compat/tests.cmake` and both inventories: `title.pal.fast`, `title.pal.bios`, `title.pin.order`, `title.pin.order.self_test`, `title.pin.recipe`; `tests/inventory/library.txt`: three `library.diag.*` rows.

## Commands and exit status (all local, run URL N/A-local)

| Command | Exit |
|---|---|
| recipe: `cmake -DPREDICATE=build/ci/tests/muhtari-test-predicate -DLEDGER=tests/compat/ledger.csv -DBUILT=build/ci/built -DDIAG=tests/diag -DBIOS=build/ci/built/openbios/openbios.bin -P tests/compat/pin.cmake` | 0 |
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -L '^built$' && ctest --preset ci -R '^(title[.]pin[.]\|policy[.]compat_doc_fresh$)' && MUHTARI_ROM_DIR=build/ci/built cmake --workflow --preset library` | 0 (46 library tests, 42 row tests) |
| `ctest --preset ci -R '^title[.]'` (11 tests) | 0 |
| `MUHTARI_ROM_DIR=build/ci/built cmake --workflow --preset library` after the predicate change and again after the banksets row | 0 |
| `ctest --preset ci -R '^(policy[.]compat_doc_fresh\|title[.]pin[.]order\|docs[.]budgets)$'` | 0 |
| `cmake --workflow --preset ci` | 0 |

Red before green for the PAL tests: `title.pal.fast` and `title.pal.bios` both ended in SEGFAULT before the predicate fix and passed after. The recipe test was checked against the previous `pin.cmake`, which stops with `pin: fixture: no file rule for this id`.

## Docs budget

`docs/emulation-tests.md` ended at 40,948 of 40,960 bytes. Trimmed only within `## title`: the duplicate `bios.splash_skip` sentence, the old banksets sentence and "Old passing predicates are unchanged"; the `## fuzz` section was not needed.

## Deviations from Plan

None - plan executed as written. The new tests' TIMEOUT lines say provisional because no hosted run has measured them; the PAL BIOS test took 0.63 s locally against a 2 s budget.

## Known Stubs

None.

## Threat Flags

None. The change adds no network, auth or file-access surface; it reads only owned diagnostics under `tests/diag`.

## Next

Plan 11-02 is the next separate allowed command (`/gsd-execute-phase 11`, one plan per step).

## Self-Check: PASSED

- `tests/compat/pin.cmake`, `predicate.c`, `tests.cmake`, `ledger.csv`: FOUND
- commits 10a7187, 7b0c863, f56e1a6: FOUND on HEAD
