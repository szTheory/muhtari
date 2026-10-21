---
phase: 02-the-cpu-matches-the-public-vectors
plan: 14
subsystem: testing
tags: [singlesteptests, vectors-full, sparse-checkout, nightly, libfuzzer, ci]

requires:
  - phase: 02-the-cpu-matches-the-public-vectors
    provides: plan 3's per-file list tests/vectors/6502-v1.sha256 and the sst-6502-v1 pin, plan 6's decimal mode (all 256 opcodes pass), plan 11's fuzz preset
provides:
  - tests/cmake/fetch_vectors.cmake, a sparse blobless fetch of 6502/v1 at the pin, verified per file
  - tests/cmake/vectors_sample_match.cmake, the committed sample proven to be the full set's prefix
  - tests/cpu/full.cmake, cpu.vectors-full.fetch, .sample_match and .00 to .ff
  - the option MUHTARI_VECTORS_FULL, the cache path MUHTARI_VECTORS_DIR and the vectors-full presets
  - tests/inventory/vectors-full.txt, 258 lines
  - nightly.yml jobs vectors-full and loader-fuzz, in report's needs, and one pull_request path union
affects: [plan 16's timeout measurement and docs, the phase 2 ship step]

actuals:
  tokens: 8760
  tasks: 2
  commits: 2
plan_head_before: ce8c87a6074d11f7e540a0efcdcd8771f49e267d
plan_head_after: b94c146192ba6ceec82474dfc13b5271037baf72

tech-stack:
  added: []
  patterns:
    - "A network fixture: a CMake -P script that does nothing when its files verify, fetches into the build tree otherwise, and is the FIXTURES_SETUP test its dependents require, so a failed fetch leaves them not run and the JUnit check fails"

key-files:
  created:
    - tests/cmake/fetch_vectors.cmake
    - tests/cmake/vectors_sample_match.cmake
    - tests/cpu/full.cmake
    - tests/inventory/vectors-full.txt
  modified:
    - CMakeLists.txt
    - CMakePresets.json
    - tests/tests.cmake
    - .github/workflows/nightly.yml
    - docs/testing.md

key-decisions:
  - "The checkout's GIT_HTTP_LOW_SPEED_TIME is 240 s, the fetch's 60 s: GitHub sends nothing for 75 to 80 s while it packs the blobs, so 60 s failed the checkout with Operation too slow"
  - "full.cmake reads the commit and URL from the sst-6502-v1 row of pins.txt, so the pin has one source"
  - "Hosted nightly run 39195320935: all 2,560,000 vectors and the loader fuzz target are green; vectors-full took 1 min 43 s (fetch 83 s), loader-fuzz 1 min 30 s"

patterns-established:
  - "fetch_vectors.cmake deletes DIR only when it holds the .muhtari-vectors marker it wrote, or DIR is empty"

requirements-completed: [CPU-02, CPU-05]

coverage:
  - id: D1
    description: "fetch_vectors.cmake checks 6502/v1 out at 2f6980a2 by git init, a cone sparse checkout and a depth-1 blobless fetch, with both low-speed variables set before the fetch and the checkout, verifies all 256 files with file(SHA256), deletes .git, and fetches nothing when every file verifies"
    requirement: CPU-02
    verification:
      - kind: integration
        ref: "cmake --workflow --preset vectors-full (exit 0; cpu.vectors-full.fetch passed in 87.10 s, then 99.44 s after a forced re-fetch)"
        status: pass
      - kind: other
        ref: "second cmake --workflow --preset vectors-full (exit 0, 3.07 s; fetch log: 256 files verify at 2f6980a2...; nothing fetched)"
        status: pass
    human_judgment: false
  - id: D2
    description: "A DIR inside the source tree outside build/ is refused before anything is created"
    requirement: CPU-02
    verification:
      - kind: other
        ref: "cmake -DDIR=$PWD/tests/vf ... -P tests/cmake/fetch_vectors.cmake (exit 1, inside the source tree but not under build/; tests/vf not created)"
        status: pass
    human_judgment: false
  - id: D3
    description: "Every one of the 2,560,000 full-set tests matches on final state and every bus cycle, locally and in the nightly job"
    requirement: CPU-02
    verification:
      - kind: integration
        ref: "cmake --workflow --preset vectors-full && cmake -DJUNIT=build/vectors-full/junit.xml -DINVENTORY=tests/inventory/vectors-full.txt -P tests/cmake/check_junit.cmake (exit 0, 258 tests ran once each)"
        status: pass
      - kind: integration
        ref: "https://github.com/szTheory/muhtari/actions/runs/39195320935"
        status: pass
    human_judgment: false
  - id: D4
    description: "The committed sample equals the first 100 tests of each fetched file, joined 00 to ff, byte for byte"
    requirement: CPU-02
    verification:
      - kind: integration
        ref: "cpu.vectors-full.sample_match (passed, 0.87 s locally and in run 39195320935)"
        status: pass
    human_judgment: false
  - id: D5
    description: "A failed fetch leaves the 257 dependent tests not run, and check_junit.cmake then fails"
    requirement: CPU-02
    verification:
      - kind: other
        ref: "one fetched file altered, HTTPS_PROXY=http://127.0.0.1:9 ctest --test-dir build/vectors-full --output-junit edge.xml (exit 8, fetch failed, 257 Not Run); check_junit on edge.xml (exit 1)"
        status: pass
    human_judgment: false
  - id: D6
    description: "nightly.yml runs vectors-full and loader-fuzz on ubuntu-26.04, each with its JUnit check and a provisional timeout; loader-fuzz uploads its crash directory on failure; both are in report's needs; the pull_request paths are one union"
    requirement: CPU-05
    verification:
      - kind: integration
        ref: "https://github.com/szTheory/muhtari/actions/runs/39195320935 (vectors-full and loader-fuzz success)"
        status: pass
      - kind: integration
        ref: "https://github.com/szTheory/muhtari/actions/runs/39195320937 (ci.yml success, hygiene.timeout_comments among it)"
        status: pass
    human_judgment: false

duration: 15min
completed: 2026-10-19
status: complete
---

# Phase 2 Plan 14: The full vector set and the loader fuzz target, nightly Summary

**`cmake --workflow --preset vectors-full` checks SingleStepTests 65x02 `6502/v1` out at 2f6980a2 (sparse, blobless,
depth 1) and verifies each file against the committed per-file list. It proves the committed sample is the first 100
tests of each file, then runs all 2,560,000 tests through the per-PR harness. Locally and in the new nightly job, all
pass. The loader fuzz target now runs nightly too.**

## Performance

- **Duration:** 15 min
- **Started:** 2026-10-19T03:21:15Z
- **Completed:** 2026-10-19T03:36:49Z
- **Tasks:** 2
- **Files modified:** 9

## Accomplishments

- **Fetch (`tests/cmake/fetch_vectors.cmake`).**
  - Arguments: `-DDIR -DSHA -DURL -DLIST -DSOURCE_DIR -DGIT`.
  - The in-source guard compares `cmake_path IS_PREFIX` before and after `REAL_PATH`, and allows `build/`.
  - It deletes `DIR` only when `DIR` holds the marker the script wrote or is empty.
  - The git steps are `init`, `sparse-checkout set --cone 6502/v1`, `fetch -q --depth 1 --filter=blob:none origin <sha>`
    and `checkout -q FETCH_HEAD`. Then `rev-parse HEAD` must equal the pin, `.git` is deleted, and all 256 files are
    verified.
  - No size, log or diff command runs in the blobless tree.
- **Sample match (`tests/cmake/vectors_sample_match.cmake`).**
  - `muhtari-vecconv <DIR>/6502/v1/xx.json <WORK>/xx.n65v xx --first 100` runs for each opcode.
  - `cmake -E cat` joins the 256 chunks and `cmake -E compare_files` compares the result with the sample.
  - On a difference it names the first chunk that differs, with its offset in the sample.
  - `-DWRITE=ON` copies the joined file over the sample instead.
- **Registration (`tests/cpu/full.cmake`).**
  - The commit and URL come from the `sst-6502-v1` row of `pins.txt`.
  - `MUHTARI_VECTORS_DIR` defaults to `build/vectors-full/vectors-full`.
  - `cpu.vectors-full.fetch` is the `vectors_full` fixture (TIMEOUT 600, provisional). `.sample_match` (TIMEOUT 120)
    and `.00` to `.ff` (TIMEOUT 60) require it. All carry the label `vectors-full`.
- **Option, inventory and presets.** `MUHTARI_VECTORS_FULL` selects the `vectors-full` inventory, and
  `tests/tests.cmake` returns early after including only `full.cmake`. `tests/inventory/vectors-full.txt` holds 258
  sorted lines. The `vectors-full` configure preset inherits `ci`, and there are matching build, test and workflow
  presets. The test preset filters on `^vectors-full$`, writes `build/vectors-full/junit.xml` and sets
  `noTestsAction: error`.
- **Nightly (`.github/workflows/nightly.yml`).**
  - Job `vectors-full`: 30 minutes, provisional.
  - Job `loader-fuzz`: 10 minutes, provisional. It installs `clang-21 lld-21 libclang-rt-21-dev` with apt, and uploads
    `build/fuzz/fuzz/crash/` on failure.
  - Each job runs its JUnit check with `if: always()`, and both are in `report`'s needs.
  - The `pull_request` paths are one union: the bench, `src/cpu.c`, `tests/cpu/**`, `tests/vectors/**`, both vector
    scripts, `src/cart.c`, `src/cartdb.c`, `src/sha1.c`, `tests/cart/**`, `tests/fuzz/**` and the workflow itself.
  - The header comment now says what runs nightly.

## Recorded runs (AGENTS rules 6 and 10)

| Run | Command | Exit | Notes |
|---|---|---|---|
| Local, first try | `cmake --workflow --preset vectors-full` | 8 | The checkout failed after 60 s with "Operation too slow". The 257 dependents were Not Run and `check_junit` failed. |
| Local, profile | the same git steps by hand, `.git` size sampled every 5 s | 0 | `.git` stayed at 136 KiB for 75 s, then grew to 180 MiB by 90 s. Checkout took 85 s. |
| Local, fixed | `/usr/bin/time -p cmake --workflow --preset vectors-full` | 0 | 88.06 s wall. The fetch took 87.10 s and the slowest opcode test 0.02 s. All 258 passed. |
| Local, JUnit | `cmake -DJUNIT=build/vectors-full/junit.xml -DINVENTORY=tests/inventory/vectors-full.txt -P tests/cmake/check_junit.cmake` | 0 | 258 tests ran once each. |
| Local, second run | `cmake --workflow --preset vectors-full` | 0 | 3.07 s wall. The fetch took 2.11 s ("nothing fetched") with no network transfer. |
| Local, empty edge | one file altered, then ctest with `HTTPS_PROXY=http://127.0.0.1:9` | 8 | The fetch failed, 257 tests were Not Run and `check_junit` exited 1. |
| Local, re-fetch | `cmake --workflow --preset vectors-full` | 0 | 100.48 s wall, fetch 99.44 s. |
| Local, lanes | `cmake --workflow --preset ci`; `cmake --workflow --preset hygiene` | 0, 0 | 405 + 4 and 22 tests passed. |
| Hosted nightly | https://github.com/szTheory/muhtari/actions/runs/39195320935 | 0 | `vectors-full` took **1 min 43 s** (fetch 83.21 s, 258 of 258 passed, `check_junit` 258). `loader-fuzz` took **1 min 30 s** (fuzz.loader 61.08 s, 3 of 3 passed, `check_junit` 3). The four bench jobs passed. `report` was skipped, as on every pull request. |
| Hosted CI | https://github.com/szTheory/muhtari/actions/runs/39195320937 | 0 | Every job succeeded, so `hygiene.timeout_comments` accepts the two new timeouts. |

The fetch writes 1,057,064 KiB to `build/vectors-full/vectors-full`: 256 files plus the root `LICENSE` and `README.md`
that cone mode checks out. Over the network it moves about 180 MiB of packed blobs. The plan's filter over the head
commit's nightly run printed `["success"]`.

## Task Commits

1. **Task 1: fetch, sample match, 258 tests, presets, inventory** - `465b6b4` (test)
2. **Task 2: the vectors-full and loader-fuzz nightly jobs, the path union, docs** - `b94c146` (ci)

## Files Created/Modified

- `tests/cmake/fetch_vectors.cmake`: the pinned fetch, verified per file (new)
- `tests/cmake/vectors_sample_match.cmake`: the sample as the full set's prefix (new)
- `tests/cpu/full.cmake`: the 258 registrations (new)
- `tests/inventory/vectors-full.txt`: 258 lines (new)
- `CMakeLists.txt`, `tests/tests.cmake`: `MUHTARI_VECTORS_FULL` and the early return
- `CMakePresets.json`: the `vectors-full` configure, build, test and workflow presets
- `.github/workflows/nightly.yml`: the two jobs, `report`'s needs, the path union
- `docs/testing.md`: the `vectors-full` lane row, the fifth inventory, the nightly paragraph

## Decisions Made

See `key-decisions` above.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 3 - Blocking] The checkout's low-speed limit**
- **Found during:** Task 1
- **Issue:** With `GIT_HTTP_LOW_SPEED_TIME=60` the checkout failed every time with `curl 28 Operation too slow`.
  GitHub sends nothing for 75 to 80 s while it packs about 1 GiB of blobs, then sends 180 MiB in about 15 s (measured
  above). C8 assumed the bytes would flow from the start.
- **Fix:** Both variables are set before the fetch (60 s, trees only) and set again before the checkout (240 s, three
  times the silence). The comment records the measurement. The 600 s TIMEOUT is unchanged.
- **Files modified:** tests/cmake/fetch_vectors.cmake
- **Verification:** three local fetches passed (87, 99 s) and so did the hosted one (83 s).
- **Committed in:** 465b6b4

**2. [Rule 1 - Bug] The crash directory path**
- **Found during:** Task 2
- **Issue:** The plan names `build/fuzz/crash/`. The fuzz test runs in `${PROJECT_BINARY_DIR}/fuzz` with
  `-artifact_prefix=crash/`, so crashes land in `build/fuzz/fuzz/crash/`, as docs/testing.md already says.
- **Fix:** The upload path is `build/fuzz/fuzz/crash/`.
- **Committed in:** b94c146

**3. [Rule 2 - Missing critical] docs/testing.md**
- **Found during:** Task 2
- **Issue:** AGENTS rule 5 requires docs with each behaviour change, and no task listed the doc.
- **Fix:** A `vectors-full` lane row (the fetch, `MUHTARI_VECTORS_DIR`, the in-source refusal, `-DWRITE=ON`, the
  network and disk needs), `vectors-full.txt` among the lane inventories, and the two nightly jobs. The full vector
  docs remain plan 16's.
- **Committed in:** b94c146

---

**Total deviations:** 3 auto-fixed (1 Rule 1, 1 Rule 2, 1 Rule 3). **Impact on plan:** none on scope.

- `fetch_vectors.cmake` also takes `-DURL`, `-DSOURCE_DIR` and `-DGIT` alongside the plan's `-DDIR -DSHA -DLIST`. The
  URL comes from the pin row, so the script holds no repository address.
- Each job's JUnit step runs with `if: always()`, so a failed workflow step still reports which tests did not run.
- Nothing is cached and nothing fetched is committed (DEC.47). `git status` stays clean after the runs, since
  `build/` is ignored.

## Issues Encountered

None beyond the low-speed limit above.

## Known Stubs

None.

## User Setup Required

None.

## Next Phase Readiness

- Plan 16 measures the provisional timeouts: TIMEOUT 600, 120 and 60 here, and `timeout-minutes` 30 and 10. One
  hosted sample so far: 1 min 43 s and 1 min 30 s.
- Probe PR #9 stays draft.

## Self-Check: PASSED

- Files present: tests/cmake/fetch_vectors.cmake, tests/cmake/vectors_sample_match.cmake, tests/cpu/full.cmake and
  tests/inventory/vectors-full.txt.
- Commits 465b6b4 and b94c146 are ancestors of HEAD, and `git rev-list --count ce8c87a..HEAD` is 2.
- Acceptance: the inventory has 258 non-comment lines, `GIT_HTTP_LOW_SPEED_TIME` appears on two `set(ENV)` lines
  before the two `execute_process` git steps, and `"noTestsAction": "error"` is in the vectors-full test preset. Every
  `uses:` line in nightly.yml has a 40-hex SHA and a `# v` comment.

---
*Phase: 02-the-cpu-matches-the-public-vectors*
*Completed: 2026-10-19*
