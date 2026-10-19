---
phase: 02-the-cpu-matches-the-public-vectors
plan: 03
subsystem: testing
tags: [singlesteptests, n65v, vecconv, fixtures, pins, vectors]

requires:
  - phase: 02-the-cpu-matches-the-public-vectors
    provides: plan 1's converter, reader and harness, plan 2's 256 per-opcode tests
provides:
  - tests/vectors/fixtures/a9.json, 02.json and a9-bad-a.json, with manifest rows, notice entries and a README
  - muhtari-test-vectors FILE XX [--first N], and single-chunk N65V files
  - the N65V reader refuses a chunk with no tests
  - muhtari-test-reader and cpu.vectors.reader
  - cpu.vectors.convert.a9, .02, .tail and .reject, and cpu.vectors.self_test
  - tests/vectors/6502-v1.sha256, the pins.txt row sst-6502-v1 and cpu.vectors.pin
affects: [02-04 policy.rom_hash_coverage over the fixture rows, the vectors-full plan's fetch_vectors.cmake and sample match, 02-16 docs/testing.md]

actuals:
  tokens: 13900
  tasks: 3
  commits: 3
plan_head_before: 3653dc0c9ccd98ea6ce435e6d25164b7b5d4b31e
plan_head_after: a496e2ad4f309a7dad9e215a642ecdc245aeb83f

tech-stack:
  added: []
  patterns:
    - "tests/cpu/tests.cmake is also the -P script for the vector tooling checks (CASE dispatch, EMULATOR passed through), the same shape as tests/runner/tests.cmake"
    - "Inputs that must be malformed are written into the build directory at test time, never committed"

key-files:
  created:
    - tests/vectors/fixtures/a9.json
    - tests/vectors/fixtures/02.json
    - tests/vectors/fixtures/a9-bad-a.json
    - tests/vectors/fixtures/README.md
    - tests/vectors/6502-v1.sha256
    - tests/cmake/vectors_pin.cmake
    - tests/cpu/test_reader.c
  modified:
    - tests/cpu/test_vectors.c
    - tests/cpu/tests.cmake
    - tests/cpu/targets.cmake
    - tests/vectors/n65v.c
    - tests/vectors/n65v.h
    - tests/roms/manifest.txt
    - THIRD-PARTY-NOTICES.md
    - .gitattributes
    - pins.txt
    - docs/testing.md
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt

key-decisions:
  - "The harness takes --first N for a .json file, so a byte prefix or a one-test array runs; without it a .json must hold all 10,000 tests"
  - "An N65V file whose first chunk is opcode 00 is the 256-chunk sample; any other N65V file is one chunk"
  - "The N65V reader refuses a chunk whose count is 0, naming the count's offset"

patterns-established:
  - "Vector tooling checks: cmake -DCASE=convert|tail|reject|self_test -P tests/cpu/tests.cmake"

requirements-completed: []

coverage:
  - id: D1
    description: "The a9 byte prefix (spaced layout) converts with --first 5, and its five tests pass from the converted chunk and from the JSON directly"
    requirement: CPU-01
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^cpu\\.vectors\\.convert\\.a9$'"
        status: pass
      - kind: other
        ref: "cmake -DCASE=convert -DFIRST=6 ... -P tests/cpu/tests.cmake on a9.json (exit 1, the sixth test is cut)"
        status: pass
    human_judgment: false
  - id: D2
    description: "The 02 byte prefix (compact layout) converts and passes both ways, with the harness's JAM check"
    requirement: CPU-01
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^cpu\\.vectors\\.convert\\.02$'"
        status: pass
    human_judgment: false
  - id: D3
    description: "The reader refuses an empty buffer, a zero-test chunk, wrong chunk and test counts and every cut of a valid chunk, naming the offset"
    requirement: CPU-01
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^cpu\\.vectors\\.reader$'"
        status: pass
      - kind: other
        ref: "test_reader.c built against n65v.c without the count 0 check (exit 1, zero tests case fails)"
        status: pass
    human_judgment: false
  - id: D4
    description: "A prefix converts with --first equal to its whole tests, and one more fails at the offset where the input ends; a malformed byte, a leading zero and a negative number each fail naming offset 30"
    requirement: CPU-01
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^cpu\\.vectors\\.convert\\.(tail|reject)$'"
        status: pass
    human_judgment: false
  - id: D5
    description: "The harness fails a9-bad-a.json with exit 1 and a9.json[0]: a expected"
    requirement: CPU-01
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^cpu\\.vectors\\.self_test$'"
        status: pass
      - kind: other
        ref: "cmake -DCASE=self_test -DIN=tests/vectors/fixtures/a9.json -P tests/cpu/tests.cmake (fails: the unedited test passes)"
        status: pass
    human_judgment: false
  - id: D6
    description: "The per-file list holds 256 lines 00.json to ff.json in path order, its SHA-256 is the sst-6502-v1 row's digest, and the row's commit is the sample's manifest pin"
    requirement: CPU-02
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^cpu\\.vectors\\.pin$'"
        status: pass
      - kind: other
        ref: "cmake -P tests/cmake/vectors_pin.cmake with lines 1 and 2 swapped, with 255 lines, and with a changed row commit (exit 1 each, naming the fault)"
        status: pass
    human_judgment: false
  - id: D7
    description: "The probe PR's ci.yml run on the head is green on every leg with the new tests"
    requirement: CPU-01
    verification:
      - kind: integration
        ref: "https://github.com/szTheory/muhtari/actions/runs/39181184945"
        status: pass
    human_judgment: false

duration: 10min
completed: 2026-10-19
status: complete
---

# Phase 2 Plan 3: The vector tooling under test, and the full set pinned Summary

**Three fixtures from SingleStepTests at 2f6980a2 now put the vector tooling under test. Two are byte prefixes (a9 spaced, 02 compact) and one is an a9 test with its final A changed. They drive `cpu.vectors.convert.{a9,02,tail,reject}`, `cpu.vectors.self_test` and a crafted-buffer `cpu.vectors.reader`. `tests/vectors/6502-v1.sha256` (digest 22df1429...d1e1) is pinned as `sst-6502-v1`, and `cpu.vectors.pin` keeps the list, the pin and the sample's manifest row in step. Probe PR #9 is green on every leg.**

## Performance

- **Duration:** 10 min
- **Started:** 2026-10-19T00:23:33Z
- **Completed:** 2026-10-19T00:34:09Z
- **Tasks:** 3
- **Files modified:** 19

## Accomplishments

- **Fixtures and their cut offsets:**
  - `a9.json` is bytes 0 to 1,799 of `6502/v1/a9.json`: five whole tests (the fifth ends at byte 1,662), then the sixth cut inside its initial RAM list.
  - `02.json` is bytes 0 to 2,899 of `6502/v1/02.json`: five whole tests (the fifth ends at byte 2,637), then the sixth cut inside its final RAM list.
  - `a9-bad-a.json` is a9's first test (bytes 2 to 333) as a one-test array, with final `a` changed from 204 to 205.
  - Each fixture has a manifest row (MIT, pin 2f6980a2, its SHA-256, notice `singlesteptests`) and an entry on the notice's applies-to line. `/tests/vectors/fixtures/*.json -text` is in `.gitattributes`.
- **Harness:** `muhtari-test-vectors FILE XX [--first N]`. `--first` is for `.json` only and runs a prefix or a short array. An N65V file whose first chunk is not opcode 00 is read as one chunk, so the converter's output runs directly.
- **Reader:** a chunk with count 0 is refused (`offset 6: count 0`). `muhtari-test-reader` reads a 37-byte valid chunk back field by field. It then requires an error naming an offset for an empty buffer, a zero-test chunk, a wrong test count, a missing and an extra chunk, and every one of the 37 cuts.
- **Converter edges:** `convert.tail` converts the a9 prefix with `--first 5`; `--first 6` fails with `offset 1800: input ends`. `convert.reject` writes three inputs into the build directory, a stray `?`, `01` and `-1` where `pc` is expected. They fail with `offset 30: expected a number`, `number with a leading zero` and `signed number`.
- **Pin:** `cpu.vectors.pin` (label `vectors`) fails on a reordered, short or CR-bearing list, a digest mismatch, a missing or doubled row, or a commit other than the sample's manifest pin.

## Recorded one-off checks (AGENTS rule 6)

- **Scratch checkout:** the plan 1 sparse checkout of `6502/v1` was still in the session scratch directory at HEAD 2f6980a2. Its per-file list, rebuilt with `shasum -a 256` in path order, has SHA-256 22df14296ebc6a02d69648467b52ab23feb4083f97aca1174a77d2902a5bd1e1, so it was used without a re-fetch.
- **Negative controls:**
  - `cmake -DCASE=convert -DFIRST=6 ... -P tests/cpu/tests.cmake` on a9.json exited non-zero.
  - `-DCASE=self_test` on the unedited a9.json exited non-zero.
  - The reader test built against `n65v.c` without the count 0 check exited 1 on `zero tests`.
  - `vectors_pin.cmake` exited 1 on a list with lines 1 and 2 swapped (it names lines 1 and 2), on a 255-line list (count and digest), and on a pins row with another commit.
- **Local lanes:** `cmake --workflow --preset ci` exit 0. `cmake --workflow --preset hygiene` exit 0. `scripts/hygiene.sh --tree` exit 0. `shasum -a 256 tests/vectors/6502-v1.sha256` is 22df1429...d1e1.

## Hosted evidence

- **Run on head `a496e2a`:** https://github.com/szTheory/muhtari/actions/runs/39181184945. `gh run watch --exit-status` exited 0, and `CI required` concluded `success`. Every job succeeded.
- The seven new tests passed on linux-x64, macos-arm64, macos-x64, windows-x64, windows-arm64 and wasm32 (the converter and the harness run under wasmtime there).

## Task Commits

1. **Task 1: the a9 prefix end to end:** `07c5f1c` (test)
2. **Task 2: the 02 prefix, the edited fixture, reader, converter edges, self test:** `cefe977` (test)
3. **Task 3: the per-file list, the pin row, cpu.vectors.pin:** `a496e2a` (test)

## Files Created/Modified

- `tests/vectors/fixtures/{a9,02,a9-bad-a}.json`, `README.md`: the fixtures and where each was cut
- `tests/cpu/test_vectors.c`: `--first N` and single-chunk N65V files
- `tests/cpu/test_reader.c`, `tests/cpu/targets.cmake`: `muhtari-test-reader`
- `tests/vectors/n65v.{c,h}`: the count 0 refusal
- `tests/cpu/tests.cmake`: the `-P` cases and the seven registrations
- `tests/vectors/6502-v1.sha256`, `tests/cmake/vectors_pin.cmake`, `pins.txt`: the list, the check and the row
- `tests/roms/manifest.txt`, `THIRD-PARTY-NOTICES.md`, `.gitattributes`: rows, applies-to entries, `-text`
- `docs/testing.md`: one paragraph on what the `sst-6502-v1` row is
- `tests/inventory/{ci,wasm32}.txt`: seven lines each

## Decisions Made

- The harness gets `--first N` rather than a "whatever the array holds" mode. The converter already has that contract, and a plain `.json` argument still means a whole 10,000-test file, as the nightly run needs.
- The sample is told apart from a single chunk by its first opcode (00). A one-chunk file of opcode 00 would be misread as the sample and fail loudly, at the second header. No test needs that file.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 3 - Blocking] The harness could not run a prefix or a single chunk**
- **Found during:** Task 1
- **Issue:** `muhtari-test-vectors` converted a `.json` with `first` 0, so it needed all 10,000 tests. It also read every N65V file as the 256-chunk sample. Neither the a9 prefix nor vecconv's one-chunk output could run, as `convert.a9` requires.
- **Fix:** an optional `--first N` for `.json`, and a first-opcode rule for N65V files.
- **Files modified:** tests/cpu/test_vectors.c
- **Verification:** `cpu.vectors.convert.a9` and the 256 `cpu.vectors.XX` pass. `--first` on an `.n65v` exits 2.
- **Committed in:** 07c5f1c

**2. [Rule 2 - Missing critical] The reader accepted a chunk with no tests**
- **Found during:** Task 2
- **Issue:** with `tests_per_chunk` 0, a zero-count chunk read as a clean end. The plan requires the reader to refuse it naming the offset.
- **Fix:** `get_header` fails `count 0` at the count's offset. The N65V format is unchanged.
- **Files modified:** tests/vectors/n65v.c, tests/vectors/n65v.h
- **Verification:** `cpu.vectors.reader`, plus the mutation run above
- **Committed in:** cefe977

**3. [Rule 2 - Missing critical] docs/testing.md said every pins row is a download**
- **Found during:** Task 3
- **Issue:** the pinned-downloads section described every row as served by `fetch-pin.sh`, which `sst-6502-v1` is not.
- **Fix:** one paragraph naming the row and `cpu.vectors.pin` (AGENTS rule 5). The full vector docs stay with plan 16.
- **Files modified:** docs/testing.md
- **Verification:** `docs.budgets` in `cmake --workflow --preset ci`
- **Committed in:** a496e2a

---

**Total deviations:** 3 auto-fixed (1 blocking, 2 missing critical). **Impact:** test code and one doc paragraph only. No core change and no scope change.

## Issues Encountered

- clang-format 21 reflowed four statements in `test_reader.c`. `clang-format -i` fixed them before the Task 2 commit, and `hygiene.format` then passed.

## Known Stubs

None.

## User Setup Required

None.

## Next Phase Readiness

- The `pins.txt` comment names `tests/cmake/fetch_vectors.cmake`, which a later plan of this phase adds. That plan's fetch should verify against `tests/vectors/6502-v1.sha256`.
- Plan 4's `policy.rom_hash_coverage` will find three fixture rows here, each with a correct SHA-256.
- CPU-01 and CPU-02 are not marked complete; later plans in this phase finish them.
- Probe PR #9 stays draft.

## Self-Check: PASSED

- Files: tests/vectors/fixtures/{a9,02,a9-bad-a}.json, README.md, tests/vectors/6502-v1.sha256, tests/cmake/vectors_pin.cmake, tests/cpu/test_reader.c: all present
- Commits 07c5f1c, cefe977, a496e2a: all ancestors of HEAD; `git rev-list --count 3653dc0..HEAD` = 3

---
*Phase: 02-the-cpu-matches-the-public-vectors*
*Completed: 2026-10-19*
