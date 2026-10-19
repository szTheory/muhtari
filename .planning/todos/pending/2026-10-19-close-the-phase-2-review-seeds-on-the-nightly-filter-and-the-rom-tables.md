---
created: 2026-10-19T18:53:14.000Z
title: Close the phase 2 review seeds on the nightly filter and the ROM tables
area: ci
severity: minor
files:
  - .github/workflows/nightly.yml
  - tools/cartdb.cmake
  - tests/cmake/firmware_scan.cmake
  - include/muhtari.h
---

## Problem

Phase 2's first code review (02-REVIEW.md as committed in 4f620d2, 2026-10-19) left one warning and three info
findings open after its first fix round. WR-01, IN-02 and IN-03 are about tooling, so AGENTS rule 7 makes them seeds,
not requirements. IN-01 is an info finding outside the fix round's scope. The re-review of the fix round reused the ids
WR-01 and IN-01 for new findings, so 02-REVIEW-DISPOSITION.md keeps only IN-02 and IN-03, 02-REVIEW-FIX.md as
committed in 99482b2 records WR-01 as skipped, and this file is the record of all four.

- WR-01: the `paths` filter on nightly.yml's `pull_request` trigger leaves out inputs of the vectors-full and
  loader-fuzz jobs: `src/internal.h`, `src/instance.c`, `src/riot.c`, `src/cartdb.h`, `src/sha1.h`,
  `tools/cartdb.cmake`, `tests/alloc.h`, `tests/inventory/vectors-full.txt`, `tests/inventory/fuzz.txt`,
  `tests/cmake/check_junit.cmake`, `CMakePresets.json`, `CMakeLists.txt` and `pins.txt`. A pull request that changes
  one of them merges without the full vector run or the 60 s fuzz run, and the break shows up as a red night after
  the merge. The bench job has the same gap for `runner/main.c`, `src/frame.c` and `src/testcard.c`.
- IN-01: `include/muhtari.h` still says `uint32_t api; /* 1 */`, but the API version is 2.
- IN-02: `tools/cartdb.cmake` accepts a `loadflag="reload"` chip whose size differs from the chip it repeats. The row
  it writes can never match by slices.
- IN-03: `tests/cmake/firmware_scan.cmake` compares whole-file hashes, so a multi-chip or half-swapped image passes the
  "no known ROM ships" gate.

## Solution

Fold each item into the next pull request that touches its file, or close them together in one `ci:` pull request:

- add the missing inputs to nightly.yml's `paths`, the bench job's too, or narrow its comment to what it covers;
- make the header comment `/* 2 */`, or `/* MUHTARI_API_VERSION */` so it cannot drift;
- record each keyed chip's size in cartdb.cmake, and `cartdb_fail` on a reload of another size;
- give the firmware scan the slice and swap passes `cart_lookup` uses, or narrow its header to single-chip images.
