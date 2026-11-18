---
phase: 29-windows-extended-path-runner-support
reviewed: 2026-11-18T22:41:57Z
depth: standard
files_reviewed: 2
files_reviewed_list:
  - runner/main.c
  - tests/runner/test_runner.c
findings:
  critical: 0
  warning: 0
  info: 0
  total: 0
  necessity: 0
status: clean
---

# Phase 29: Code Review Report

**Reviewed:** 2026-11-18T21:27:29Z
**Depth:** standard
**Files Reviewed:** 7
**Status:** clean

## Historical Review Round — 2026-11-18T21:27:29Z

The final fixes resolve the prior findings. The test-only `realpath` wrapper and macro mapping are both under `#ifndef _WIN32`; the Windows BIOS and HSC process cases use valid `--frames 1` invocations; and the output-collision matrix keeps its selected frames in range and checks for a `conflicts` diagnostic. No critical or warning findings remain. The parent-reported `runner.movie.owner`, `runner.save.faults`, and `runner.trace.paths` selection passed 4/4 including fixture setup; tests were not rerun during this read-only review.

All reviewed files meet quality standards. No issues found.

---

_Reviewed: 2026-11-18T21:27:29Z_
_Reviewer: the agent (gsd-code-reviewer)_
_Depth: standard_

## Follow-up Review — 2026-11-18T22:24:36Z

**Scope:** Current changes in `runner/main.c`, `tests/runner/test_save.c`, and `tests/runner/windows_paths.c` only.
**Depth:** standard
**Status:** issues_found

The path conversion error split is internally consistent: `runner_path_to_wide` sets `ENOMEM` only for allocation failure and maps malformed or overlong paths to other errors. The PNG fault wrapper fails after one complete row and leaves the production PNG writer and runner cleanup path in place. Two warnings remain: the volume-limit fixture can query a different volume than the test directory, and the new Windows path-validation OOM branch lacks a regression test. Tests were not run, as requested.

## Narrative Findings (AI reviewer)

### WR-01: Component limit may come from the wrong mounted volume

**File:** `tests/runner/windows_paths.c:567-576, 935-937`
**Issue:** `drive_volume_root` always truncates a path to its drive-letter root (`C:\` or `\\?\C:\`). If `work_dir` is under a volume mount point, the SMB share and generated files use the mounted volume, while `GetVolumeInformationW` reads the parent drive's component limit. `verify_component_boundaries` then builds its boundary name from that unrelated limit, which can make this integration test fail spuriously or test the wrong boundary when the volumes have different limits.
**Fix:** Resolve the containing volume root for the supplied `drive_path` (for example with `GetVolumePathNameW`) and query that root, rather than deriving only the drive-letter root.

### WR-02: New Windows path-validation OOM result is not exercised

**File:** `runner/main.c:1480-1482, 2804-2808`; `tests/runner/test_save.c:122-130`
**Issue:** The changed path validation now distinguishes allocation failure from malformed paths and returns the normal failure status with an OOM diagnostic. The runner fault allocator currently injects only the trace-capture allocation failure; no test forces `runner_path_to_wide`'s allocation to fail, so the new result path and its distinction from the usage-error path are unguarded. This leaves a changed behavior without the regression coverage required by the project instructions.
**Fix:** Add a Windows-only fault case that fails the validation conversion allocation after option parsing, then assert `MUHTARI__EXIT_FAILURE`, the OOM diagnostic, and that execution does not proceed to file or emulator work.

---

_Follow-up reviewed: 2026-11-18T22:24:36Z_
_Reviewer: the agent (gsd-code-reviewer)_
_Depth: standard_

## Follow-up Review — 2026-11-18T22:36:41Z

**Scope:** Current changes in `runner/main.c`, `tests/runner/test_runner.c`, `tests/runner/test_save.c`, and `tests/runner/windows_paths.c` only.
**Depth:** standard
**Status:** issues_found

WR-01 is resolved: `local_volume_root` calls `GetVolumePathNameW` on the actual local path for the drive namespace and on the local SMB backing path for the UNC namespace, then passes the resolved trailing-slash volume root to `GetVolumeInformationW`. WR-02 remains: the added Windows assertion targets allocation failure 1, which is an option-array allocation in `parse_options`; it exits before path validation and cannot emit the path-validation OOM diagnostic. The test also has no file-operation counter, so its emulator counters alone do not assert the requested no-file-work condition. Tests were not run, as requested.

## Narrative Findings (AI reviewer)

### WR-02: OOM regression test fails before reaching path validation

**File:** `tests/runner/test_runner.c:282-291`; `runner/main.c:1267-1275, 2803-2809`
**Issue:** In `movie_owner`, `movie_fail_at = 1` fails the first `movie_alloc` call. `parse_options` makes its five option-array allocations first and returns `MUHTARI__EXIT_FAILURE` with the generic `out of memory` message when any is null. That skips `runner_path_arguments_supported`, so `movie_path_oom_reports` remains zero and the new Windows assertion expecting one report fails. The assertions that no core load/frame/input occurred therefore exercise parser OOM, not validation OOM; no file-operation counter is checked either.
**Fix:** Target the allocation made by `runner_path_to_wide` after the five parser allocations (or add a dedicated validation-allocation fault seam), then assert the path-validation OOM diagnostic, zero emulator callbacks, and zero file opens/reads.

---

_Follow-up reviewed: 2026-11-18T22:36:41Z_
_Reviewer: the agent (gsd-code-reviewer)_
_Depth: standard_

## Follow-up Review — 2026-11-18T22:41:57Z

**Scope:** Current changes in `runner/main.c` and `tests/runner/test_runner.c` only.
**Depth:** standard
**Status:** clean

WR-02 is resolved. The revised `movie_owner` loop tries each allocation index observed in the successful run and requires `saw_path_oom`, so it cannot silently skip the path-conversion failure. `parse_options` performs five option-array allocations before `runner_path_arguments_supported` reaches `runner_path_to_wide`; a failure at one of the later conversion allocations reaches the diagnostic branch. For that failure, the test checks exactly one path-validation OOM diagnostic, zero `_wfopen` calls, and zero load/frame/input callbacks. The caller flow returns before `read_movie` and `run`, so there is no input open or emulator execution. WR-01 remains resolved by the preceding follow-up's `GetVolumePathNameW` lookup. Tests were not run, as requested.

## Narrative Findings (AI reviewer)

No new or open findings in this follow-up.

---

_Follow-up reviewed: 2026-11-18T22:41:57Z_
_Reviewer: the agent (gsd-code-reviewer)_
_Depth: standard_
