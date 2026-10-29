---
phase: 11-full-library-acceptance
plan: "04"
subsystem: compat-local-run
tags: [compat, library, local, identification, cmake]
status: complete
requires:
  - phase: 11-full-library-acceptance
    provides: "Derived softlist ledger rows, the pin recipe, the ratchet tier and the required library job (plans 01 to 03)"
provides:
  - "muhtari__cart_identify_entry: the matched list entry's index, kept for an identified image the core then refuses"
  - "muhtari-test-predicate --identify and exit 3 (the core refused the image), told apart from exit 2 (a usage, read, BIOS or instance fault)"
  - "tests/compat/local.cmake and library.local: every softlist row counted as pass, fail or absent against MUHTARI_ROM_DIR at test time, ids and counts only"
  - "title.local.fixture: the whole mechanism on public bytes in the ci and wasm32 lanes"
  - "library preset search order MUHTARI_ROM_DIR, build/ci/built, tests/diag with a size guard and a named prerequisite"
affects: [11-05, compat-ledger, library-preset, ci]
tech-stack:
  added: []
  patterns:
    - "Link-time substitution of the test hash list for a second test executable (muhtari-test-predicate-testdb)"
    - "A cmake -P driver whose predicate and git streams are all captured, so only its own counted lines reach the log"
key-files:
  created:
    - tests/compat/local.cmake
    - tests/compat/local_fixture.cmake
  modified:
    - src/cart.c
    - src/internal.h
    - tests/compat/predicate.c
    - tests/compat/tests.cmake
    - tests/compat/library.cmake
    - tests/cart/test_cart.c
    - tests/cart/write_images.c
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - tests/inventory/library.txt
    - CMakePresets.json
    - .github/workflows/ci.yml
key-decisions:
  - "Detection of private files happens at test time; configure only counts candidates (to scale the library.local timeout), so configure is the same for zero, some or all files."
  - "A file is matched to a row only through the core's own identification; the predicate prints the matched entry's part-0 chip key, never a path or a name."
  - "An identified image the loader refuses (predicate exit 3) is a fail; a bad BIOS, a malformed predicate, a crash, a timeout, an unwritable output and a duplicate key are named harness faults."
  - "Skipped rows are not mapped, which is why the two skipped rows that share one chip SHA-1 (hiscore and xm) are not a duplicate-key fault."
requirements-completed: []
coverage:
  - id: C1
    description: A file is matched to a row only through the core's own identification, and --identify prints the part-0 chip SHA-1 or a dash with no path; a multi-chip, half-swapped, mirrored-chip and refused image keeps its entry index
    requirement: COMP-03
    verification:
      - kind: unit
        ref: cart.identify.slices, cart.identify.activision_swap, cart.identify.hash_override, cart.identify.not_a_cartridge
        status: pass
      - kind: integration
        ref: build/ci/tests/muhtari-test-predicate-testdb --identify over the sample (headerless and headered), the not-a-cartridge image and README.md printed the two sample keys, cc0437c32b3703a5388f4883d56e05eaf419a428 and a dash (exit 0)
        status: pass
    human_judgment: false
  - id: C2
    description: A refused image exits 3 and a malformed predicate, an unreadable ROM and an unusable BIOS exit 2
    requirement: COMP-03
    verification:
      - kind: integration
        ref: muhtari-test-predicate-testdb on not-a-cartridge-4k boots@120 (exit 3, also with --bios --id --result, no file left); boots@0, /nonexistent and a 3-byte BIOS (exit 2 each)
        status: pass
    human_judgment: false
  - id: C3
    description: One present row that passes and one absent row are counted, with the oversized file, the text file, the unidentified file and the second name for one image changing nothing
    requirement: COMP-03
    verification:
      - kind: integration
        ref: title.local.fixture run 1
        status: pass
    human_judgment: false
  - id: C4
    description: A failing predicate on a present row is a fail, and an identified image the loader refuses is a fail, both with exit 0
    requirement: COMP-03
    verification:
      - kind: integration
        ref: title.local.fixture runs 2 and 3
        status: pass
    human_judgment: false
  - id: C5
    description: A bad BIOS, a malformed ledger predicate and an unwritable output are named harness faults with no path, and the BIOS is checked before any evaluation
    requirement: COMP-03
    verification:
      - kind: integration
        ref: title.local.fixture runs 4, 5 and 6
        status: pass
      - kind: other
        ref: ctest --preset ci -R title.local.fixture with the exit-3 branch of local.cmake replaced by if(FALSE) (exit 8, "refused: exit 1, not 0"), then restored
        status: pass
    human_judgment: false
  - id: C6
    description: No predicate or git text, file name, path or image hash reaches the log; every predicate spawn carries the emulator wrapper
    requirement: COMP-03
    verification:
      - kind: unit
        ref: title.local.fixture
        status: pass
      - kind: other
        ref: ctest --preset ci -R title.local.fixture -FS '.*' -V piped to grep -cE 'simple[.]bas|not-a-cartridge' (prints 0); grep -v '^#' tests/compat/local.cmake | grep -c ERROR_VARIABLE (2) and grep -cF '${EMULATOR} ${PREDICATE}' (2); the fixture run with -DEMULATOR set to /usr/bin/env;LC_ALL=C (six runs as expected)
        status: pass
    human_judgment: false
  - id: C7
    description: A PAL image is covered on both boot paths, and MUHTARI_ROM_DIR unset, not a directory or a missing BIOS keep the library preset's configure failures
    requirement: COMP-03
    verification:
      - kind: integration
        ref: title.pal.fast, title.pal.bios
        status: pass
    human_judgment: false
  - id: C8
    description: pr rows are found in MUHTARI_ROM_DIR, then build/ci/built, then tests/diag, files above the image maximum are not hashed, and a row file found nowhere names every searched root and the ci build that creates build/ci/built
    requirement: COMP-03
    verification:
      - kind: integration
        ref: MUHTARI_ROM_DIR=tests/diag MUHTARI_BIOS=build/scratch-bios.bin cmake --preset library with build/ci/built renamed away (exit 1, names tests/diag, build/ci/built and ctest --preset ci -L '^built$'), then restored and the scratch copy deleted
        status: pass
    human_judgment: false
  - id: C9
    description: library.local accounts all 204 softlist rows with zero present files and keeps the library inventory complete
    requirement: COMP-03
    verification:
      - kind: integration
        ref: MUHTARI_ROM_DIR=build/ci/built cmake --workflow --preset library (exit 0, 47 tests); ctest --test-dir build/library -R '^library[.]local$' -V printed "local: 0 of 204 present, 0 pass, 0 fail, 204 absent, commit=6c23a463bc01 tree=dirty"
        status: pass
      - kind: unit
        ref: cmake -DJUNIT=build/library/junit.xml -DINVENTORY=tests/inventory/library.txt -P tests/cmake/check_junit.cmake (47 tests ran once each)
        status: pass
    human_judgment: false
  - id: C10
    description: hygiene.firmware_scan, which passes -DTRACKED=1, stays registered and passes in the hygiene lane that the ci.yml hygiene job runs
    requirement: COMP-03
    verification:
      - kind: unit
        ref: hygiene.firmware_scan
        status: pass
      - kind: other
        ref: grep -cE '^hygiene[.]firmware_scan[[:space:]]hygiene$' tests/inventory/hygiene.txt (1); grep -c -- '-DTRACKED=1' tests/hygiene/tests.cmake (1); ci.yml line 91 runs cmake --workflow --preset hygiene
        status: pass
    human_judgment: false
  - id: C11
    description: The library job's timeout comment carries the re-measured margin; the hosted duration is read at ship
    requirement: COMP-03
    verification:
      - kind: unit
        ref: hygiene.timeout_comments
        status: pass
      - kind: other
        ref: three local runs of the ci build step plus the fresh library workflow took 9.2, 8.3 and 8.6 s; library.local alone 0.13, 0.12 and 0.12 s; the ci.yml comment records local max 9.2 s, +0.2 s, margin 290 s of 300 s; ship note, read the pull request's library job duration (hosted)
        status: pass
    human_judgment: false
  - id: C12
    description: The whole regression gate and the hygiene workflow pass on the plan head
    requirement: COMP-03
    verification:
      - kind: integration
        ref: cmake --workflow --preset ci
        status: pass
      - kind: integration
        ref: cmake --workflow --preset hygiene
        status: pass
    human_judgment: false
actuals:
  tokens: 12200
  tasks: 3
  commits: 4
plan_head_before: f692e7ec9050a3c8ca0042dfcc7954c33244e89c
plan_head_after: 6183c39f4ec1b90d508a6166091d3f15903ded82
duration: 70m
completed: 2026-10-29
---

# Phase 11 Plan 04: The counted local run Summary

**A private directory is judged by one command: files are matched to softlist rows through the core's own identification, every one of the 204 rows is counted as pass, fail or absent with ids and counts only, and a public fixture proves the mechanism, its refused-image and harness-fault classes included, in the ci and wasm32 lanes.**

## What changed

- `src/cart.c`, `src/internal.h`: `muhtari__cart_identify_entry` takes `muhtari__cart_identify`'s parameters plus `uint32_t *entry`, the matched list entry's index or `UINT32_MAX`, kept when the core then refuses the image. `muhtari__cart_identify` calls it with `NULL`; no status, field or flag changed. The accessor adds no state.
- `tests/compat/predicate.c`: `--identify FILE...` prints, per file, the 40 hex digits of the matched entry's part-0 chip key or `-`, and nothing else on stdout. The load-failure code is split: exit 3 when `muhtari_create` worked and `muhtari_load` refused the image (invalid, truncated, unsupported size, 2600, board, region or device), exit 2 for a create failure, an unreadable file, a BIOS the core cannot use and every other load status.
- `tests/compat/tests.cmake`, `tests/cart/write_images.c`: `muhtari-test-predicate-testdb` links the test hash list in place of `src/cartdb.c`, as `muhtari-test-cart` does; `muhtari-test-images` writes `not-a-cartridge-4k`, an image the list identifies and the core refuses.
- `tests/compat/local.cmake` (new): checks the BIOS first, probes the output, builds the population (ids with no `/`) and a key map from non-skipped rows' first chip SHA-1, identifies the regular files up to the image maximum in batches of 64, evaluates each present row on the fast boot and through the BIOS, writes `local.csv` with every row, and prints one counts line with commit and tree state. Every predicate and git stream is captured; the only output is `local:` lines with ids and counts.
- `tests/compat/local_fixture.cmake` (new) and `title.local.fixture`: six runs on public bytes (base, failing predicate, refused identified image, bad BIOS, malformed predicate, unwritable output), each checked for exit code, exact `local:` line and counts file, and for any file name, path or hash in the output.
- `tests/compat/library.cmake`, `tests/inventory/library.txt`, `CMakePresets.json`: pr rows are found in `MUHTARI_ROM_DIR`, then `build/ci/built` when it exists, then `tests/diag`; empty and oversized files are not hashed; a row file found nowhere names every root and, when `build/ci/built` is missing, the ci build that creates it. `library.local` is registered with a timeout of 60 s plus 8 s per candidate file counted at configure.
- `tests/cart/test_cart.c`: the identify cases assert the accessor's entry index (chips 4, mirrored chip 5, Activision 6 whole and half-swapped, override 1, not a cartridge 8 headerless and headered, `UINT32_MAX` for misses and for the 64 KiB swap case, entry 0 kept for every refused geometry) and that its status and info equal `muhtari__cart_identify`'s.
- `.github/workflows/ci.yml`: the library job's timeout comment records the re-measured margin.

## Commands and exit status (all local, run URL N/A-local)

| Command | Exit |
|---|---|
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -L '^built$'` | 0 |
| `ctest --preset ci -R '^(cart[.]\|title[.]simple[.]\|runner[.]probe[.]images$)'` (25 tests) | 0 |
| `muhtari-test-predicate-testdb --identify` over the sample twice, `not-a-cartridge-4k` and `README.md` | 0, four expected lines |
| `muhtari-test-predicate-testdb not-a-cartridge-4k boots@120` (and with `--bios --id --result`, no file left) | 3 |
| `boots@0`, an unreadable ROM, a 3-byte BIOS | 2 each |
| `ctest --preset ci -R '^(title[.]local[.]fixture\|cart[.]identify[.]test_list\|cart[.]identify[.]bundled_has_no_test_entries)$'` | 0 |
| fixture with the exit-3 branch of `local.cmake` replaced by `if(FALSE)`, then restored | 8, refused run named |
| `-DEMULATOR=/usr/bin/env;LC_ALL=C` fixture run by hand | 0 |
| `ctest --preset ci -R '^cart[.]'` with `*entry = UINT32_MAX` in the accessor, then restored | 8, four identify cases failed; 0 after restoring |
| `MUHTARI_ROM_DIR=build/ci/built cmake --workflow --preset library` | 0 |
| `ctest --test-dir build/library -R '^library[.]local$' -V` | 0, `local: 0 of 204 present, 0 pass, 0 fail, 204 absent, commit=6c23a463bc01 tree=dirty` |
| `cmake -DJUNIT=build/library/junit.xml -DINVENTORY=tests/inventory/library.txt -P tests/cmake/check_junit.cmake` | 0 |
| prerequisite message run (build/ci/built renamed, `MUHTARI_ROM_DIR=tests/diag MUHTARI_BIOS=build/scratch-bios.bin cmake --preset library`) | 1, names `build/ci/built` and the ci build; restored, copy deleted |
| `ctest --preset hygiene -R '^(hygiene[.]firmware_scan\|hygiene[.]timeout_comments)$'` | 0 |
| `cmake --workflow --preset ci` (first run) | 8, `title.pal.bios` timed out at 2 s on a loaded host; alone 0.52 s, passes |
| `ctest --preset ci -j 3` (670 tests) | 0 |
| `cmake --workflow --preset hygiene` (first run) | 8, `hygiene.format` flagged the new code; formatted with the pinned clang-format |
| `cmake --workflow --preset hygiene` (rerun) | 0 |
| `cmake --workflow --preset ci` (head 6183c39, first run) | 8, `diag.bankset_tracer` raced `diag.bankset` (see Deferred Issues); alone passes |
| `cmake --workflow --preset ci` (rerun) | 0, 670 tests |

## Timeout margin

Local timings of the library job's two build-and-judge steps, from a fresh build/library: 9.2, 8.3 and 8.6 s (built step 6.7, 5.9, 6.2 s; library workflow 2.4 to 2.5 s). `library.local` alone: 0.13, 0.12 and 0.12 s over 620 candidate files in build/ci/built. The `ci.yml` comment reads `local max 9.2 s with library.local (+0.2 s), n=3, margin 290 s; provisional`, above half the 300 s timeout, so no stop was needed. The hosted duration of the pull request's library job is read at ship.

## Deviations from Plan

**1. [Rule 3 - Blocking] Formatting.** `hygiene.format` rejected the new accessor and its callers; the pinned clang-format (llvm@21) was applied and committed as a style commit. No behaviour changed.

**2. [Rule 1 - Bug] The first prerequisite message repeated a root and joined its hint with semicolons.** When `MUHTARI_ROM_DIR` is `tests/diag` the roots list named it twice, and a multi-argument `set` glued `;` into the text. The roots are de-duplicated and the hint is one string; the message was rerun and read.

Otherwise the plan executed as written. The `48K-on-64K` case the plan names has no case of that name in `test_cart.c`; the geometry cases that list a 48 KiB image on a ROM-at-`$4000` board, which the core refuses or accepts by size, assert that the entry index is kept for every refused geometry.

## Deferred Issues

- `title.pal.bios` (2 s provisional timeout) timed out once on a host loaded by unrelated processes; it passes alone in 0.52 s and in `ctest --preset ci -j 3`. The timeout predates this plan.
- `diag.bankset_tracer` failed once with `png: chunk longer than the file` on `build/ci/tests/diag/bankset/32n/pixels.png` while `diag.bankset` ran at the same time: both tests write the same output directory and have no resource lock. It passes alone and on the rerun. Logged in `deferred-items.md`; it is outside this plan's files.

## Known Stubs

None.

## Threat Flags

None. The plan adds an internal accessor and test tooling; it adds no endpoint, no auth path and no file access beyond the directory a user names, and nothing private is printed, written outside `build/library/` or committed.

## Next

Plan 11-05 documents the command and records the one run; the owner's optional run is a ship note, never a verification step. Ship note: read the pull request's `library` job duration against the 5-minute provisional timeout.

## Self-Check: PASSED
