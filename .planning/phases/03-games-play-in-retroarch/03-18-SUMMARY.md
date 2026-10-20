---
phase: 03-games-play-in-retroarch
plan: 18
subsystem: testing
tags: [compat, ledger, ratchet, library-preset, ci, docs-generation]

requires:
  - phase: 03-games-play-in-retroarch
    provides: "the ledger, muhtari-test-predicate and pin.cmake (plan 03-14), the built samples and OpenBIOS (plans 03-08 and 03-12), the diagnostics"
provides:
  - "the library preset (MUHTARI_LIBRARY, inventory library.txt): library.<id> per pr-tier row on both boot paths, library.results, library.ratchet, library.ratchet.self_test, library.compat_doc"
  - "muhtari-test-predicate --id ID --result FILE: fast boot then BIOS path, one id,predicate,status,evidence line"
  - "tests/cmake/compat_ratchet.cmake: results equal the ledger both ways; with a base commit, no pass row falls without a dated allow line"
  - "tests/compat/allow.txt with its format header"
  - "tools/compat_doc.cmake and the generated docs/compatibility.md, kept fresh by policy.compat_doc_fresh and its self test"
  - "ci.yml library job, report-only, on every pull request"
affects: [03-19, ship, compatibility doc, ci-required promotion]

actuals:
  tokens: 10127
  tasks: 3
  commits: 3
plan_head_before: 9c88b411d29dd0dccd94d068ada38d8bbdac7837
plan_head_after: 47da7bc926f0ca91ca556ac8c453d6285da0a351

tech-stack:
  added: []
  patterns:
    - "A test-driver .cmake file registers tests when included and does one job when run with -P (library.cmake joins the results)"
    - "The ratchet reads today and the 60-day limit from string(TIMESTAMP) with SOURCE_DATE_EPOCH, so a test can fix the date"
    - "Values with arbitrary characters are kept in CMake variables named by a key and read back through a name variable (${${name}})"

key-files:
  created:
    - tests/compat/library.cmake
    - tests/compat/targets.cmake
    - tests/compat/allow.txt
    - tests/cmake/compat_ratchet.cmake
    - tests/inventory/library.txt
    - tools/compat_doc.cmake
    - docs/compatibility.md
  modified:
    - CMakeLists.txt
    - CMakePresets.json
    - tests/tests.cmake
    - tests/compat/tests.cmake
    - tests/compat/predicate.c
    - tests/cmake/ci_required.cmake
    - tests/hygiene/tests.cmake
    - tests/policy/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - .github/workflows/ci.yml
    - docs/emulation-tests.md
    - docs/testing.md
    - README.md

key-decisions:
  - "A row's file is found by its whole-file SHA-1: sha1_a78 for a headered file first, then sha1_raw for a headerless one. CMake cannot hash a byte range, so a headered dump whose header differs from the ledger's is not found"
  - "library.<id> exits 0 once its result line is written, pass or fail; library.ratchet is the judge, so a ledger fail row needs no special test handling"
  - "The ratchet compares status and evidence (the title hash prefix), not status alone"
  - "The base ledger comes from MUHTARI_LEDGER_BASE, read at configure time, not from a variable named BASE; on a push to main it is empty and only the equality check runs"
  - "A base whose tree has no ledger protects nothing; a base missing from the clone fails (cat-file -e with COMMAND_ERROR_IS_FATAL)"
  - "policy.ci_required takes a comma-separated report-only list (wasm32,library), one added line"
  - "The library build preset builds only muhtari-test-predicate, and the preset turns the libretro core off, since the lane registers no libretro test"

patterns-established:
  - "A new report-only CI job goes into REPORT_ONLY in tests/hygiene/tests.cmake and joins CI required after 10 green runs on main"
  - "After a ledger change, regenerate docs/compatibility.md with tools/compat_doc.cmake in the same commit"

requirements-completed: [GAME-06, GAME-03]

coverage:
  - id: D1
    description: "The library preset registers library.<id> for each of the 39 pr-tier rows that are neither unsupported nor skipped, found by SHA-1 under MUHTARI_ROM_DIR and tests/diag; each evaluates the fast boot and the OpenBIOS path; library.results writes build/library/results.csv"
    requirement: GAME-06
    verification:
      - kind: integration
        ref: "MUHTARI_ROM_DIR=$PWD/build/ci/built cmake --workflow --preset library (exit 0, 43 tests) and test -s build/library/results.csv"
        status: pass
      - kind: e2e
        ref: "https://github.com/szTheory/muhtari/actions/runs/39380164010"
        status: pass
    human_judgment: false
  - id: D2
    description: "Configure of the library preset fails naming the missing piece when MUHTARI_ROM_DIR is unset, when the BIOS image is missing, and when a row has no file"
    requirement: GAME-06
    verification:
      - kind: other
        ref: "MUHTARI_ROM_DIR=$PWD/build/ci/built MUHTARI_BIOS=$PWD/build/no-such-bios.bin cmake --preset library (exit 1, output names no-such-bios.bin); MUHTARI_ROM_DIR set to an empty directory (exit 1, names 7800basic-0.41/adventurer and both SHA-1s); MUHTARI_ROM_DIR unset (exit 1)"
        status: pass
    human_judgment: false
  - id: D3
    description: "compat_ratchet.cmake: results equal the ledger in both directions by status and evidence; a quote, CR or wrong field count fails; library.ratchet.self_test fails naming the flipped row"
    requirement: GAME-06
    verification:
      - kind: unit
        ref: "tests/compat/library.cmake#library.ratchet.self_test (expect_fail CODE 1, REGEX names 7800basic-0.41/adventurer)"
        status: pass
      - kind: other
        ref: "cmake -DLEDGER=tests/compat/ledger.csv -DRESULTS=<copy> -P tests/cmake/compat_ratchet.cmake: a flipped diag/dma, a missing diag/sg144 with an extra row, and a quoted field each exit 1 naming the row"
        status: pass
    human_judgment: false
  - id: D4
    description: "With a base commit, a pass row that falls or changes its predicate needs an allow line expiring from today through 60 days on; at 60 days it is accepted, at 61 or expired refused, a malformed line refused; a base without a ledger protects nothing, a base not in the clone fails"
    requirement: GAME-06
    verification:
      - kind: other
        ref: "SOURCE_DATE_EPOCH=1792454400 cmake -DLEDGER=build/ratchet-test/ledger.csv -DRESULTS=build/ratchet-test/results.csv -DALLOW=build/ratchet-test/allow.txt -DBASE=HEAD -P tests/cmake/compat_ratchet.cmake: no line exit 1; expiry 2026-12-19 exit 0; 2026-12-20 exit 1; 2026-10-19 exit 1; 2026-10-20 exit 0; malformed exit 1; changed predicate exit 1; -DBASE=origin/main exit 0; unknown commit exit 1"
        status: pass
    human_judgment: false
  - id: D5
    description: "The ci.yml library job builds the ci preset's samples and OpenBIOS with full history, runs the library preset with the pull request's base, checks JUnit, and stays out of CI required's needs"
    requirement: GAME-03
    verification:
      - kind: e2e
        ref: "https://github.com/szTheory/muhtari/actions/runs/39379798527 (library success, 34 s, MUHTARI_LEDGER_BASE set, 42 tests)"
        status: pass
      - kind: e2e
        ref: "https://github.com/szTheory/muhtari/actions/runs/39380164010 (library success, 30 s, 43 tests; every job success)"
        status: pass
      - kind: unit
        ref: "tests/hygiene/tests.cmake#policy.ci_required (REPORT_ONLY wasm32,library; hygiene preset exit 0)"
        status: pass
    human_judgment: false
  - id: D6
    description: "tools/compat_doc.cmake generates docs/compatibility.md (counts by status, board and region, 8 weak rows, 42 title rows); policy.compat_doc_fresh and its self test on ci and wasm32; library.compat_doc after the ratchet"
    requirement: GAME-06
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^(policy\\.compat_doc_fresh|docs\\.)' --output-on-failure (exit 0, 5 tests)"
        status: pass
      - kind: integration
        ref: "tests/compat/library.cmake#library.compat_doc (library preset exit 0)"
        status: pass
    human_judgment: false
  - id: D7
    description: "docs/emulation-tests.md ledger section (columns, statuses, tiers, the library preset, the ratchet and allow.txt, personal dumps optional), docs/testing.md lane and job rows, and a README line pointing at docs/compatibility.md"
    requirement: GAME-06
    verification:
      - kind: unit
        ref: "tests/policy/tests.cmake#docs.budgets (exit 0); scripts/hygiene.sh --tree (exit 0)"
        status: pass
    human_judgment: false

duration: 22min
completed: 2026-10-20
status: complete
---

# Phase 3 Plan 18: The ledger in CI Summary

**A `library` preset now judges every `pr` row of the compatibility ledger afresh, on the fast boot and through OpenBIOS. A 100-line ratchet holds the results equal to the ledger and stops a passing row from falling without a dated allow line. `docs/compatibility.md` is generated from the ledger and checked for freshness. One report-only Linux job runs all of it on every pull request.**

## Performance

- **Duration:** 22 min of work, plus about 7 min of hosted runs
- **Started:** 2026-10-20T06:07:55Z
- **Completed:** 2026-10-20T06:29:30Z
- **Tasks:** 3
- **Files modified:** 21

## Accomplishments

- **The library preset.** `cmake --workflow --preset library` needs `MUHTARI_ROM_DIR` and takes `MUHTARI_BIOS` (default `build/ci/built/openbios/openbios.bin`). It registers 39 `library.<id>` tests from the ledger, plus `library.results`, `library.ratchet`, `library.ratchet.self_test` and `library.compat_doc`. Its inventory is `tests/inventory/library.txt`, 43 lines. No `ci*` preset defines `MUHTARI_ROM_DIR` or registers a library test.
- **The driver.** `muhtari-test-predicate ... --bios FILE --id ID --result FILE` runs both paths and writes one line. The driver's target moved to `tests/compat/targets.cmake`, so the library lane builds it without the `title.*` tests.
- **The ratchet.** `tests/cmake/compat_ratchet.cmake` is 100 lines. It checks equality in both directions, and with `MUHTARI_LEDGER_BASE` it checks for falls against `git show <base>:tests/compat/ledger.csv` and `tests/compat/allow.txt`.
- **The page.** `tools/compat_doc.cmake` is 80 lines. The generated page shows 39 pass, 2 skipped and 1 unsupported; 33 linear rows; all 42 NTSC; 8 `boots@` rows.
- **CI.** The `library` job passed on both pushes: 34 s on run 39379798527 and 30 s on run 39380164010. On 39380164010 all 19 jobs succeeded.

## Task Commits

1. **Task 1: library preset, inventory, library.<id>, results, equality check (tracer)**: `3d3e82f` (test)
2. **Task 2: ratchet against the base, allow file, negative control, CI job**: `927045b` (ci)
3. **Task 3: generated compatibility page and its freshness checks**: `47da7bc` (docs)

**Plan metadata:** the SUMMARY commit follows.

## Tracer gate

`human_verify_mode` is end-of-phase and Task 1's `<verify>` is automated only, so I re-ran it and synthesized no checkpoint. `cmake --workflow --preset ci` passed 585 tests and 6 release tests. The library workflow passed 41 tests and wrote a 40-line `results.csv`. The hygiene workflow passed 24 tests, and `scripts/hygiene.sh --tree` exited 0. The tracer was verified end to end before expanding.

## Files Created/Modified

- `tests/compat/library.cmake`: the library lane: env checks, SHA-1 lookup, per-row tests, results join (`-P` mode), ratchet, self test, page check
- `tests/compat/targets.cmake`: `muhtari-test-predicate` only
- `tests/compat/predicate.c`: `run_path`, `run_result`, `--id`/`--result`
- `tests/cmake/compat_ratchet.cmake`: the ratchet (gate code)
- `tools/compat_doc.cmake`: the page generator (gate code) with `-DCHECK`
- `tests/compat/allow.txt`: the dated-exception format header
- `docs/compatibility.md`: generated
- `tests/inventory/library.txt`: new; `ci.txt` and `wasm32.txt` get `policy.compat_doc_fresh` and its self test
- `CMakeLists.txt`, `CMakePresets.json`, `tests/tests.cmake`: the option, presets and lane branch
- `tests/cmake/ci_required.cmake`, `tests/hygiene/tests.cmake`: comma-separated report-only list with `library`
- `tests/policy/tests.cmake`: the two page tests
- `.github/workflows/ci.yml`: the `library` job
- `docs/emulation-tests.md`, `docs/testing.md`, `README.md`: ledger section, lane and job rows, one link

## Decisions Made

See `key-decisions`. The two that matter most:

- **The ratchet judges and `library.<id>` evaluates.** Each row test passes once it writes its line, so a ledger `fail` row stays a plain test. `library.ratchet` fails by name on any disagreement.
- **Files are matched on the whole file's SHA-1**, against `sha1_a78` and then `sha1_raw`. See the deviations below.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 3 - Blocking] The headerless SHA-1 cannot be computed in CMake**
- **Found during:** Task 1
- **Issue:** The plan matches files by headerless SHA-1, applying the probe's rule. CMake can hash only a whole file; it cannot hash a byte range or write a binary slice.
- **Fix:** A file matches a row when its whole SHA-1 equals `sha1_a78`, which is a headered file as identified, or `sha1_raw`, which is a headerless file. Every CI row is found. The one case missed is a personal dump whose header differs from the ledger's, which is local and optional.
- **Files modified:** tests/compat/library.cmake
- **Commit:** 3d3e82f

**2. [Rule 2 - Correctness] The driver also takes `--id`**
- **Issue:** The result line starts with the row id, which the driver cannot know from the ROM.
- **Fix:** `--id ID --result FILE`, both or neither. The result file is removed first, so a crashed row cannot leave a stale line, and `library.results` also checks the count.
- **Commit:** 3d3e82f

**3. [Rule 1 - Bug] Semicolons and match variables in the ratchet**
- **Found during:** Task 2
- **Issue:** `file(STRINGS)` would split a `;why=` predicate in an allow line. A nested `MATCHES` reset `CMAKE_MATCH_2`. A `;` in a message split it in two.
- **Fix:** The allow file is read with the same placeholder as the ledger, the match variables are used before any other `MATCHES`, and the message has no `;`.
- **Commit:** 927045b

**4. [Rule 3 - Blocking] The base name and the date input**
- **Issue:** The plan passes `BASE` through. A variable that generic could collide with a user's environment, and the edge test needed a fixed date.
- **Fix:** `MUHTARI_LEDGER_BASE` is read at configure time and passed as `-DBASE`. Today comes from `string(TIMESTAMP)`, which honours `SOURCE_DATE_EPOCH`.
- **Commit:** 927045b

**5. [Rule 2 - Docs] docs/testing.md**
- **Issue:** The plan does not list docs/testing.md, but it documents every lane, inventory and CI job, and `policy.ci_required`'s report-only list.
- **Fix:** It gains the `library` lane row, the inventory sentence, the job row, and the roll-up and ci_required lines.
- **Commit:** 927045b

**6. [Rule 1 - Bug] The self-test regex and the wrapping of CMake messages**
- **Found during:** Task 3
- **Issue:** `message(FATAL_ERROR)` wraps a long path onto its own line, so `policy.compat_doc_fresh.self_test`'s regex `... is stale` failed.
- **Fix:** The regex is `compat_doc_fixture/compatibility.md.+is stale`.
- **Commit:** 47da7bc

**Total deviations:** 6 auto-fixed (2 bugs, 2 blocking, 2 correctness or docs). **Impact:** every truth holds, except that file lookup uses the whole-file SHA-1 (deviation 1).

## Gate code (rule 7)

This plan adds the two gates it names: `compat_ratchet.cmake` at 100 lines, the plan's upper bound but above its target of about 80, and `compat_doc.cmake` at 80 lines. It also adds one line to `ci_required.cmake`. `library.cmake` is a test driver. Each gate's header names the fault it prevents.

## Unreleased Pins

No frame or title pin moved, and the core is unchanged. The ledger is unchanged.

## Notes for 03-19

Provisional limits this plan wrote, each marked `# provisional, phase 3`:
- TIMEOUT 60 on each `library.<id>`, and 30 on `library.results`, `library.ratchet`, `library.ratchet.self_test` and `library.compat_doc`;
- `timeout-minutes: 10` on the `library` job.

The hosted job took 34 s and 30 s. Locally every row took 0.06 to 0.29 s. `policy.compat_doc_fresh` and its self test reuse the policy lane's existing TIMEOUT 10.

## Known Stubs

None. 03-14's note that docs/emulation-tests.md named a library job before it existed is closed in this plan's SUMMARY commit.

## Threat Flags

None. The job is `contents: read`, `persist-credentials: false`, and uploads no built ROM.

## Issues Encountered

None beyond the deviations.

## User Setup Required

None.

## Next Phase Readiness

Ready for 03-19. The `library` job joins CI required after 10 green runs on main. Probe PR #12 is still a draft. It was pushed to twice, and it is not marked ready and has not been merged.

## Self-Check: PASSED

- FOUND: tests/compat/library.cmake, tests/compat/targets.cmake, tests/compat/allow.txt, tests/cmake/compat_ratchet.cmake, tests/inventory/library.txt, tools/compat_doc.cmake, docs/compatibility.md
- FOUND: 3d3e82f, 927045b and 47da7bc are ancestors of HEAD
- Acceptance results:
  - the `"name": "library"` grep printed lines, and `MUHTARI_ROM_DIR` appears only in the library preset's comment;
  - 44 `library.` lines in library.txt, with one each for `library.ratchet.self_test` and `library.compat_doc`;
  - one `add_executable` in targets.cmake and two `targets.cmake` includes;
  - the missing-BIOS configure exited 1 and named `no-such-bios.bin`;
  - `Fault prevented` appears once in the first 12 lines;
  - `wc -l` printed 100 for the ratchet and 80 for compat_doc;
  - the `library:` job is present, and `needs:` lists no library;
  - README names `compatibility.md` once, and ci.txt has the two `policy.compat_doc_fresh` lines.
- Local: `cmake --workflow --preset ci` exited 0 with 587 tests plus 6 release tests. The library workflow exited 0 with 43 tests. The hygiene workflow exited 0 with 24 tests. `scripts/hygiene.sh --tree` exited 0, and `cmake -DDIR=.github/workflows -P tests/cmake/timeouts.cmake` exited 0 with 26 timeouts.
- Hosted: https://github.com/szTheory/muhtari/actions/runs/39380164010 concluded success on all 19 jobs.
