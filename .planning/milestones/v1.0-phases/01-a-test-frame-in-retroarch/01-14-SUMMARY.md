---
phase: 01-a-test-frame-in-retroarch
plan: 14
subsystem: ci
tags: [github-actions, ci, msvc, vcvarsall, qemu, wasmtime, junit, hashes]

requires:
  - phase: 01-13
    provides: be32 and wasm32 presets, hashes.cmake write and compare
  - phase: 01-12
    provides: asan and nofp presets, fetch-pin.sh and the wasmtime pin
  - phase: 01-10
    provides: abi.cmake and floor.cmake, whose Windows branches ran here first
provides:
  - ".github/workflows/ci.yml: changes, hygiene, build x6, asan, nofp x2, be32, hash-equality, wasm32 (report-only) and the CI required roll-up"
  - "scripts/classify-changes.sh: heavy=false only when every changed path, old and new, is under .planning/"
  - "Draft probe pull request #6 with a green run on every lane"
  - "Windows floor rows filled from observed llvm-readobj output"
  - "docs/testing.md CI section"
affects: [01-15 policy tests for the roll-up and classifier, 01-17 release.yml workflow_call, 01-19 nightly, 01-21 timeouts and probe close]

actuals:
  tokens: 5240
  tasks: 2
  commits: 5
plan_head_before: 398e20d81f0a7811725a03c01f4b778cfc6ae609
plan_head_after: 0a1120e

tech-stack:
  added: [GitHub Actions (checkout v7.0.1, upload-artifact v7.0.2, download-artifact v8.0.2, all SHA-pinned)]
  patterns:
    - "Every CI job runs a workflow preset, then check_junit.cmake on that step's JUnit file with the lane's inventory and labels"
    - "Heavy jobs carry if: needs.changes.outputs.heavy == 'true'; CI required decides what a skip means"
    - "Files compared across hosts are written with NEWLINE_STYLE UNIX"

key-files:
  created:
    - .github/workflows/ci.yml
    - scripts/classify-changes.sh
  modified:
    - tests/cmake/floor.cmake
    - tests/cmake/hashes.cmake
    - tests/abi/tests.cmake
    - tests/hygiene/tests.cmake
    - docs/testing.md

key-decisions:
  - "No be32 flip: the hosted be32 job took 24 to 33 s against the five-minute rule, so hash-equality compares seven files with be32 in both needs lists"
  - "R-10 settled: MSVC /W4 /WX builds clean on both Windows legs; the /MT runner and DLL import only KERNEL32.dll; llvm-nm reads the COFF .lib; muhtari.lib carries no /DEFAULTLIB"
  - "hashes.txt is written with Unix newlines: CMake file(WRITE) wrote CRLF on Windows"
  - "hygiene.analyzer leaves out clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling, which clang-tidy 21 enables against glibc; the core may call only the C memory functions"
  - "Windows build steps run under the default bash shell after the pwsh vcvarsall step; CMake found MSVC's link.exe ahead of Git's"

patterns-established:
  - "Probe PR iteration: push, watch the run for the pushed head commit, read the failed step with gh run view --log-failed"

requirements-completed: [FRAME-01, FRAME-03, FRAME-05]

coverage:
  - id: D1
    description: "ci.yml runs on the draft probe pull request, and CI required concludes success with every needed job green"
    requirement: FRAME-01
    verification:
      - kind: e2e
        ref: "gh run watch 38935647258 --exit-status (exit 0), https://github.com/szTheory/muhtari/actions/runs/38935647258, 15 of 15 jobs success"
        status: pass
    human_judgment: false
  - id: D2
    description: "Each build leg runs cmake --workflow --preset ci (ci-msvc on Windows) on the six runner labels, then check_junit on both test steps (56 and 4 tests)"
    requirement: FRAME-01
    verification:
      - kind: e2e
        ref: "run 38935647258 build jobs: check_junit 56 then 4 tests ran once each on linux-x64, linux-arm64, macos-arm64, macos-x64, windows-x64, windows-arm64"
        status: pass
    human_judgment: false
  - id: D3
    description: "hash-equality compares exactly seven hashes.txt files (six legs and be32) byte-identical; be32 is in its needs and in CI required's"
    requirement: FRAME-03
    verification:
      - kind: e2e
        ref: "run 38935647258 hash-equality: hashes: 7 identical files"
        status: pass
      - kind: other
        ref: "run 38934803292 hash-equality failed naming hashes-windows-arm64 (CRLF), the negative case before the newline fix"
        status: pass
    human_judgment: false
  - id: D4
    description: "hygiene, asan, nofp on x64 and arm64, be32 and wasm32 each run their preset and a JUnit completeness check with the lane's inventory and filter"
    requirement: FRAME-05
    verification:
      - kind: e2e
        ref: "run 38935647258: check_junit hygiene 7, asan 46, nofp 11 and 11, be32 30, wasm32 28 tests ran once each"
        status: pass
    human_judgment: false
  - id: D5
    description: "wasm32 runs report-only, outside CI required's needs, and its hashes.txt equals Linux x64's"
    requirement: FRAME-03
    verification:
      - kind: e2e
        ref: "run 38935647258 wasm32 step 'Same hashes as Linux x64' (cmp) success"
        status: pass
    human_judgment: false
  - id: D6
    description: "release.floor passes on all six legs; Windows imports exactly KERNEL32.dll and muhtari.lib has no /DEFAULTLIB, and the Windows branch fails on an empty import or export list"
    requirement: FRAME-01
    verification:
      - kind: e2e
        ref: "run 38935647258 release.floor Passed on windows-x64 and windows-arm64 and the other four legs"
        status: pass
      - kind: other
        ref: "cmake -DDIR=<downloaded archives-windows-<arch>> -DSUFFIX=windows-<arch> -DWORK=<scratch> -P tests/cmake/floor.cmake exit 0 for x64 and arm64; with -DFLOOR_OVERRIDE= it fails naming KERNEL32.dll for both binaries"
        status: pass
    human_judgment: false
  - id: D7
    description: "The macOS arm64 build job runs the README install line against its built libretro zip"
    requirement: FRAME-01
    verification:
      - kind: e2e
        ref: "run 38935647258 build (macos-arm64) 'README install line': installed cores/muhtari_libretro.dylib and info/muhtari_libretro.info"
        status: pass
    human_judgment: false
  - id: D8
    description: "scripts/classify-changes.sh prints heavy=false only for a non-empty all-.planning/ list and reads all of its input"
    requirement: FRAME-05
    verification:
      - kind: other
        ref: "printf on empty, src/a.c, .planning/x.md, a mixed list and 100000 src paths: heavy=true, true, false, true, true; the changes job succeeds on run 38935647258"
        status: pass
    human_judgment: false
  - id: D9
    description: "Concurrency cancels in-progress runs only for pull requests, grouped by workflow name and pull request or commit"
    requirement: FRAME-01
    verification:
      - kind: other
        ref: "grep -n 'cancel-in-progress' .github/workflows/ci.yml shows ${{ github.event_name == 'pull_request' }} under group ci-${{ github.workflow }}-${{ github.event.pull_request.number || github.sha }}"
        status: pass
      - kind: e2e
        ref: "release run https://github.com/szTheory/muhtari/actions/runs/38942043999 (plan 01-17): its nested ci / changes started 2026-10-16T17:44:52Z, the same second as changes in CI run 38942043783 on the same commit e6b87b0, and both runs succeeded"
        status: pass
    human_judgment: false
  - id: D10
    description: "Every uses: line in ci.yml pins a 40-hex SHA with a # v comment"
    verification:
      - kind: other
        ref: "grep 'uses:' .github/workflows/ci.yml | grep -cvE '@[0-9a-f]{40} # v' prints 0"
        status: pass
    human_judgment: false

duration: 16min
completed: 2026-10-16
status: complete
---

# Phase 1 Plan 14: The required CI workflow, green on six platforms plus be32 Summary

**`ci.yml` runs the same preset commands as a local build on six runner labels and in the hygiene, asan, nofp ×2, be32 and wasm32 lanes. A JUnit completeness check follows every test step, seven `hashes.txt` files compare byte-identical, and the `CI required` roll-up is green on draft probe PR #6.**

## Performance

- **Duration:** about 16 min
- **Started:** 2026-10-16T16:41:26Z
- **Completed:** 2026-10-16T16:57:31Z
- **Tasks:** 2
- **Files modified:** 7 (358 insertions, 5 deletions)

## Accomplishments

- **Probe PR.** [#6](https://github.com/szTheory/muhtari/pull/6), draft, titled `ci: probe the phase workflows`.
- **Green run.** https://github.com/szTheory/muhtari/actions/runs/38935647258 on head `0a1120e`. All 15 jobs succeeded, `CI required` included.
- **Windows.** Both Windows legs passed on their first run, with MSVC set up by vswhere and vcvarsall and no third-party action.
- **Three faults found only on hosted runners.** Each was fixed in the next push:
  - Windows wrote `hashes.txt` with CRLF.
  - GCC on the arm64 runner prints UTF-8 quotes.
  - clang-tidy 21 checks glibc for Annex K buffer handling.
- **be32.** Stays per pull request, with no flip. The comparison in force is seven files with `-DCOUNT=7`.

## Per-job wall times (seed for plan 21's timeouts)

| Job | Run 38934803292 | Run 38935217538 | Run 38935647258 |
|---|---|---|---|
| changes | 4 s | 6 s | 5 s |
| hygiene | 11 s (failed) | 14 s | 10 s |
| build linux-x64 | 15 s | 14 s | 10 s |
| build linux-arm64 | 20 s | 9 s | 12 s |
| build macos-arm64 | 18 s | 20 s | 27 s |
| build macos-x64 | 57 s | 40 s | 42 s |
| build windows-x64 | 43 s | 60 s | 37 s |
| build windows-arm64 | 50 s | 51 s | 45 s |
| asan | 27 s | 9 s | 18 s |
| nofp x64 | 6 s | 4 s | 6 s |
| nofp arm64 | 6 s (failed) | 18 s | 7 s |
| be32 | 24 s | 25 s | 33 s |
| hash-equality | 8 s (failed) | 8 s | 8 s |
| wasm32 | 21 s | 17 s | 29 s |
| CI required | 3 s | 3 s | 4 s |

The whole run took 89 s from creation to completion. Every `timeout-minutes` still says `# provisional`.

## R-10 outcomes (MSVC unknowns)

1. **`/W4 /WX` and enum constants.** Silent. The `ci-msvc` build has `CMAKE_COMPILE_WARNING_AS_ERROR` on and passed on x64 and arm64. Assumption A1 holds.
2. **`/MT` imports.** `llvm-readobj --coff-imports` lists `Name: KERNEL32.dll` alone for `muhtari-run.exe` and for `muhtari_libretro.dll`, on both architectures. A2 holds, and D-15 is unchanged.
3. **`llvm-nm` on the `.lib`.** It reads the COFF archive. Symbol types seen:
   - `T` and `U`
   - `t` for `$LN` labels
   - `r` for `$pdata` and `$unwind`
   - `a` for `@comp.id` and `@feat.00`

   There is no writable `B`, `b`, `D`, `d` or `C` symbol. `abi.undefined`, `abi.globals` and `abi.symbols` pass. The only undefined name outside the archive is `memset`.
4. **`/DEFAULTLIB`.** The scan of `muhtari.lib` finds none (`/Zl`).
5. **Exports.** `retro_*` only, 25 names.

## Gate-code tally

- **New this plan:**
  - `.github/workflows/ci.yml`: 277 lines
  - `scripts/classify-changes.sh`: 20 lines
  - `floor.cmake`: +10 lines
  - `hashes.cmake`: +2 lines
- **All gate scripts:**
  - `tests/cmake/*.cmake` plus `cmake/MuhtariTests.cmake`: 985 lines
  - `scripts/*.sh`: 355 lines
- **Code under test:** `src/` is 315 lines. The tenth-of-`src/` ceiling applies from phase 3.

## Evidence (command, exit status, run URL)

- **Task 1 verify.** Watched the head commit's run with `gh run watch 38934486704 --exit-status`. Exit 0, and `CI required` concluded `success`: https://github.com/szTheory/muhtari/actions/runs/38934486704
- **Task 2 verify.** `gh run watch 38935647258 --exit-status`, exit 0. The job count was `[.jobs[] | select(.conclusion == "success")] | length` = 15: https://github.com/szTheory/muhtari/actions/runs/38935647258
- **PR state.** `gh pr view --json isDraft,title -q '.isDraft,.title'` printed `true` and `ci: probe the phase workflows`.
- **Local checks.**
  - `cmake --workflow --preset ci`: exit 0, 56 then 4 tests.
  - `cmake --workflow --preset hygiene`: exit 0, 7 tests, after the fixes.
- **Windows floor on the Mac.** `floor.cmake` ran on the downloaded `archives-windows-x64` and `archives-windows-arm64`, with `llvm-readobj` 21. Exit 0 for both, and the empty-override probe failed as expected.
- **Hash files.** `gh run download 38934803292 -p 'hashes-*'`: six files were 215 bytes (SHA-256 `f0651781…`). The two Windows files were 218 bytes and ended in `\r\n`.
- **Workflow lint.** `actionlint .github/workflows/ci.yml` reports only that its built-in label list lacks `ubuntu-26.04` and `ubuntu-26.04-arm`. The live runs prove both labels exist.

## Task Commits

1. **Task 1: End-to-end CI, one Linux build leg and the roll-up** - `c19652d` (ci), with fix `e6f6b18`: the classifier closed its pipe early, and `gh` died of SIGPIPE under pipefail.
2. **Task 2: All lanes** - `3276a72` (ci). Fixes: `1b0863b` (CRLF, nofp regex, analyzer check). Docs and floor: `0a1120e`.

## Files Created/Modified

- `.github/workflows/ci.yml`: the required workflow and its `CI required` roll-up
- `scripts/classify-changes.sh`: the planning-only classifier
- `tests/cmake/floor.cmake`: Windows rows from observed output; an empty import or export list fails
- `tests/cmake/hashes.cmake`: Unix newlines on every host
- `tests/abi/tests.cmake`: the nofp regex accepts GCC's UTF-8 quotes
- `tests/hygiene/tests.cmake`: the analyzer leaves out the Annex K check
- `docs/testing.md`: the CI section
- Coverage entries in `01-10`, `01-11`, `01-12` and `01-13-SUMMARY.md` now read `pass`, each with the green run URL.

## Decisions Made

See `key-decisions` in the frontmatter.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Bug] The classifier closed its input pipe early**
- **Found during:** Task 1, first run 38934312891
- **Issue:** `changes` failed. The script exited at the first heavy path, so `gh api` hit a closed pipe, and `pipefail` failed the step.
- **Fix:** The script reads all of its input, then prints the result.
- **Files modified:** `scripts/classify-changes.sh`
- **Committed in:** e6f6b18

**2. [Rule 1 - Bug] Windows hashes.txt had CRLF line endings**
- **Found during:** Task 2, run 38934803292
- **Issue:** `file(WRITE)` writes CRLF on Windows. The values were identical, but the files were not.
- **Fix:** `file(CONFIGURE ... NEWLINE_STYLE UNIX)`.
- **Files modified:** `tests/cmake/hashes.cmake`, which is not in the plan's file list
- **Committed in:** 1b0863b

**3. [Rule 1 - Bug] abi.nofp_fixture regex and a UTF-8 locale**
- **Found during:** Task 2
- **Issue:** The hosted arm64 runner's GCC prints `‘-mgeneral-regs-only’` with three-byte quotes, and the regex allowed one byte. The containers ran in the C locale.
- **Fix:** `general-regs-only[^ ]* is incompatible with the use of floating-point.types`.
- **Files modified:** `tests/abi/tests.cmake`
- **Committed in:** 1b0863b

**4. [Rule 3 - Blocking] clang-tidy 21's Annex K check on glibc**
- **Found during:** Task 2
- **Issue:** `hygiene.analyzer` flagged every `memcpy` and `memset` in `src/` as insecure. The Mac's clang-tidy does not enable that check.
- **Fix:** That one check is turned off, with a comment giving the reason. The core's allowed calls are fixed by `abi.undefined`, and `memset_s` is optional in C11 and absent from glibc.
- **Files modified:** `tests/hygiene/tests.cmake`
- **Committed in:** 1b0863b

**5. [Rule 2 - Missing critical] The Windows floor could pass on empty tool output**
- **Found during:** Task 2, R-10
- **Issue:** The import and export checks only rejected names they read. Unparsed `llvm-readobj` output would have passed.
- **Fix:** An empty import list or export list now fails. The floor values are unchanged.
- **Files modified:** `tests/cmake/floor.cmake`
- **Committed in:** 0a1120e

**6. [Verify command] `--workflow ci.yml` returns 404 before the file is on main**
- **Issue:** `gh run list --workflow ci.yml` asks the API for the workflow on the default branch. It returned 404 until a run existed on the branch.
- **Fix:** The watch loops used `--workflow CI`, which resolves once the first run exists. The plan's form now works too, because it returns run 38934486704.

---

**Total deviations:** 5 auto-fixed (3 bugs, 1 blocking, 1 missing critical) and 1 verify-command note.
**Impact on plan:** All were needed for a true green run. No locked decision changed, and no floor was loosened.

## Issues Encountered

- None beyond the deviations. The probe PR stays a draft and open. Plan 21 closes it before `/gsd-ship`.

## Threat Flags

| Flag | File | Description |
|------|------|-------------|
| threat_flag: candidate-controlled-classifier | .github/workflows/ci.yml | The `changes` job runs `scripts/classify-changes.sh` from the pull request's own head. A pull request that edits the script could mark itself planning-only and skip the heavy jobs. Plan 15's `ci.classify` test checks the script, and branch review sees the edit. Fork pull requests are not trusted for anything else in this workflow. |

## Seeds

- Seeded from nesturbator 9f9386c661f6 `.github/workflows/ci.yml` (436 lines), fetched with `gh api` into a scratch directory. Taken from it: the vswhere and vcvarsall step, the matrix shape and the roll-up. Planning IDs were dropped, and the rest was rewritten for this repository's lanes.
- 3200aa868371 is not on GitHub. The seed came from published main.

## User Setup Required

None.

## Next Phase Readiness

- Plan 15 can add `policy.ci_required`, `ci.classify` and `hygiene.timeout_comments`. The roll-up's jq program sits in one single-quoted string on the `CI required` step's `run:`. Its arguments are `--arg event`, `--arg heavy` and `--arg called`.
- Ruleset: `CI required` has now reported on the probe PR, so the ruleset can be applied (research R-14).
- The `wasm32` job is green and report-only. Plan 01-17's release workflow calls `ci.yml` with `ref`.

---
*Phase: 01-a-test-frame-in-retroarch*
*Completed: 2026-10-16*

## Self-Check: PASSED

- FOUND: .github/workflows/ci.yml, scripts/classify-changes.sh, tests/cmake/floor.cmake, docs/testing.md
- FOUND commits: c19652d, e6f6b18, 3276a72, 1b0863b, 0a1120e (ancestors of HEAD)
