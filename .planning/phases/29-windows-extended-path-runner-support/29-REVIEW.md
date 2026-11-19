---
phase: 29-windows-extended-path-runner-support
reviewed: 2026-11-19T08:42:15Z
depth: standard
files_reviewed: 1
files_reviewed_list:
  - tests/runner/test_save.c
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

## Follow-up Review — 2026-11-18T23:08:24Z

**Scope:** Latest changes in `tests/runner/test_runner.c` and `tests/runner/windows_paths.c` only.
**Depth:** standard
**Status:** issues_found

The Windows movie allocation assertion correctly includes the one tracked UTF-8-to-wide allocation made by `runner_path_open`; the non-Windows count remains unchanged. The real-process output check now forces HSC attachment, and its missing-output diagnostic reports the matched output role and path. However, this does not make the fixture produce the expected 32 KiB SaveKey file: the fixture created by `trace_paths` has HSC only, and the process arguments do not select a SaveKey port. With HSC attachment alone the battery size is 2 KiB, and this fixture's program does not dirty HSC RAM, so `save_flush` has no changed battery to write. WR-01 and WR-02 remain resolved by the prior follow-ups. Tests were not run, as requested.

## Narrative Findings (AI reviewer)

### WR-03: Windows output matrix cannot produce its expected SaveKey battery

**File:** `tests/runner/windows_paths.c:1014-1052`
**Issue:** The real-process run passes `--hsc on`, but the `image` fixture comes from `trace_paths`, which writes an HSC-only cartridge (`devices=1`) with a non-mutating loop. The run supplies no `--port1 savekey`, and its fixture has no SaveKey header bit. HSC attachment therefore provides only 2,048 battery bytes; because the program never changes HSC RAM, `save_flush` sees the imported generation unchanged and creates no battery file. The matrix then requires the battery output to exist and be exactly 32,768 bytes, so this Windows path test fails before it can validate the intended output path. Evidence: `tests/runner/test_save.c:1101-1103`, `src/cart.c:601-610`, `src/instance.c:281-286`, and `runner/main.c:2150-2165`.
**Fix:** Keep HSC attached, explicitly select a SaveKey port for this process run, and use a fixture program that changes HSC RAM so the battery becomes dirty. Then assert the combined expected size (2,048 HSC bytes plus 32,768 SaveKey bytes), or change the scenario to expect and deliberately produce the HSC-only size.

---

_Follow-up reviewed: 2026-11-18T23:08:24Z_
_Reviewer: the agent (gsd-code-reviewer)_
_Depth: standard_

## Follow-up Review — 2026-11-18T23:12:04Z

**Scope:** Final Windows CI correction in `tests/runner/test_runner.c`, `tests/runner/test_save.c`, and `tests/runner/windows_paths.c`.
**Depth:** standard
**Status:** clean

WR-03 is resolved. The fixture now changes HSC RAM, the real-process command explicitly selects SaveKey on port 1, and its HSC-only cartridge header makes the default automatic HSC mode attach HSC. The battery therefore contains 2,048 HSC bytes plus 32,768 SaveKey bytes, matching the 34,816-byte assertion; the HSC write changes the generation so the final `save_flush` persists it. The role and path arrays also match the output array, so a missing-output diagnostic names the specific artifact and path. The Windows movie allocation count includes the single tracked allocation for `runner_path_open`'s UTF-8-to-wide conversion. WR-01, WR-02, and WR-03 are resolved; no open findings remain. Tests were not run, as requested.

## Narrative Findings (AI reviewer)

No new or open findings in this follow-up.

---

_Follow-up reviewed: 2026-11-18T23:12:04Z_
_Reviewer: the agent (gsd-code-reviewer)_
_Depth: standard_

## Follow-up Review — 2026-11-18T23:28:28Z

**Scope:** New `load_state_with_savekey` helper and call site in `tests/runner/windows_paths.c` only.
**Depth:** standard
**Status:** clean

No finding. The SaveKey-aware state was produced by the preceding real-process invocation with `--port1 savekey`, and the new load helper supplies the same port while using the same ROM and `--frames0`. This matches the state identity's port-device and EEPROM-port fields (`src/state.c:238-241`); the existing no-SaveKey `load_state` remains paired with its no-SaveKey state output. The owner-reported local round-trip supports the command pairing, but Windows CI remains unverified. Tests were not run in this review.

## Narrative Findings (AI reviewer)

No new or open findings in this follow-up.

---

_Follow-up reviewed: 2026-11-18T23:28:28Z_
_Reviewer: the agent (gsd-code-reviewer)_
_Depth: standard_

## Follow-up Review — 2026-11-19T00:07:21Z

**Scope:** Focused review of `tests/runner/windows_paths.c`, tracing the blocked state-output case and SaveKey state reload through `runner/main.c`.
**Depth:** standard
**Status:** clean

No finding. `verify_blocked_outputs` expects the exact diagnostic phrase `cannot commit state file`; `state_capture` prints `muhtari-run: %s: cannot commit state file` when the atomic state write fails, and the `MUHTARI__EXIT_FAILURE` status propagates out of the runner. The blocked-output helper checks the held destination's file identity, exact sentinel length and bytes, then checks the parent directory for a `<destination-leaf>.tmp.*` sibling after the child exits. The SaveKey state-producing invocation and `load_state_with_savekey` both pass `--port1 savekey`; the load helper retains the same default machine-layout options, while the creator's `--save-dir` only selects external battery storage. The state identity includes the selected port devices and EEPROM layout. This was a source review; the Windows process case was not run here, and platform acceptance remains pending as recorded above.

## Narrative Findings (AI reviewer)

No new or open findings in this follow-up.

---

_Follow-up reviewed: 2026-11-19T00:07:21Z_

_Reviewer: the agent (gsd-code-reviewer)_
_Depth: standard_

## Follow-up Review — 2026-11-19

**Scope:** `tests/runner/test_save.c`, `docs/runner.md`, and Phase 29 acceptance wording.
**Status:** clean

The `..` aliases remain in the runner arguments while the harness snapshots canonical output or ROM paths, avoiding WASI `stat()` on the alias without dropping the collision cases. The public path contract distinguishes created output and `.sav` lengths from calculated temporary candidates; no actionable issue remains. No tests were run as part of this review; verification is recorded in `29-ACCEPTANCE.md`.

## Follow-up Review — 2026-11-19

**Scope:** Remaining `dir/./option.sav` protected snapshot in `tests/runner/test_save.c`.
**Status:** clean

No remaining WASI protected-snapshot trap risk found. The `./` trace alias and both `../` aliases remain in runner arguments, while their snapshots use canonical paths. The `subdir` fixture is a real directory, so those aliases reach the same targets; canonical existence checks and ROM snapshots still catch unintended output mutation. No coverage regression found. No tests were run during review; verification is recorded in `29-ACCEPTANCE.md`.

_Follow-up reviewed: 2026-11-19_

## Follow-up Review — WASI identity check normalization

**Scope:** `runner/main.c` WASI path normalization and `runner_paths_equal()` branch in commit `fb53ef1`.
**Status:** clean

No actionable issue found. The branch normalizes both spellings before equality or filesystem identity checks, so raw `.`/`..` aliases do not reach wasi-libc `stat()`. Normalized equality is conservatively treated as a collision. For distinct spellings, any exact `..` component causes an unresolved result before `stat()`, avoiding false negatives where a symlink changes POSIX resolution; otherwise `stat()` receives only normalized paths and retains the device/inode hard-link check. Allocation failures are freed and fail closed. POSIX and Windows branches are unchanged. Final WASI execution remains pending the exact-source hosted receipt recorded in `29-ACCEPTANCE.md`.

Tests were not run as part of this review; local and hosted verification evidence is recorded in `29-ACCEPTANCE.md`.

## Follow-up Review — WASI branch local scope

**Scope:** Scope-only change in `runner/main.c` commit `9cd7878`.
**Status:** clean

The added compound block separates the WASI branch's `normalized_left` and `normalized_right` locals from the shared fallback declarations that follow the preprocessor branch. All existing returns, cleanup paths, and comparisons remain inside the same branch, and the POSIX and Windows branches are unchanged. No new control-flow, warning, allocation, or safety issue found. Tests were not run during this review; evidence is in `29-ACCEPTANCE.md`.

## Follow-up Review — 2026-11-19T07:23:01Z

**Scope:** WASI-only `trace_file_exists()` change in `tests/runner/test_save.c`.
**Depth:** standard
**Status:** clean

No actionable finding. On WASI the helper now probes the harness-owned regular fixture, snapshot, or temporary file with `fopen(path, "rb")` and closes a successfully opened stream. The collision cases still pass `./` and `../` aliases to the runner; only the protected snapshot path is canonicalized, so this helper change does not remove the runner alias cases. POSIX `stat()` and Windows `_stat64()` paths are unchanged.

This source review does not establish that the new probe passes in the WASI runtime. The recorded WASI failures exercised the earlier `stat()` probe, and the successful Nightly WASI job builds and benchmarks the runner without running `runner.trace.paths`. The acceptance record appropriately leaves exact-source WASI and Windows rows pending; this test-only change does not itself accept the Windows path behavior. Tests were not run, as requested.

_Follow-up reviewed: 2026-11-19T07:23:01Z_
_Reviewer: the agent (gsd-code-reviewer)_
_Depth: standard_

## Follow-up Review — WASI snapshot buffers

**Reviewed:** 2026-11-19T08:42:15Z
**Scope:** `tests/runner/test_save.c:742-743`, moving the two trace snapshots to static storage.
**Depth:** standard
**Status:** clean

No actionable finding. `trace_expect_usage()` is called sequentially by the path matrix and argument checks; it is not recursive or concurrently invoked. The two buffers are consumed entirely within each call, are fully overwritten by `trace_snapshot()` before comparison, and do not escape. Static storage therefore preserves the before/after length and byte checks while removing 16 KiB of automatic storage from the nested WASI call path. The bounded-memory tradeoff is small and consistent with the project's documented 64 KiB WASI stack guidance. The exact peak stack usage was not instrumented, so the review treats the stack explanation as the best fit for the reproduced failure and successful focused rerun, rather than a measured high-water mark.

The failure/refusal assertions remain unchanged. The focused WASI `runner.trace.paths` test and all 12 WASI trace/core trace tests pass on this source; native CI and release tests pass. No additional change is recommended.

_Follow-up reviewed: 2026-11-19T08:42:15Z_
_Reviewer: the agent (gsd-code-reviewer)_
_Depth: standard_

## Follow-up Review — 2026-11-19T10:21:08Z

**Scope:** Added successful replacement case and output-argument helper in `tests/runner/windows_paths.c`.
**Depth:** standard
**Status:** issues_found

The helper runs the output-producing process again with pre-existing extended drive and UNC destinations. State reload and the battery byte comparison meaningfully check replacement. One coverage gap remains for PNG and trace output:

### WR-04 (WARNING): PNG and trace checks allow stale trailing bytes

**File:** `tests/runner/windows_paths.c:1089-1092`
**Severity:** WARNING
**Issue:** After replacing the old PNG and trace contents with a short sentinel, the test checks only that the new PNG signature and trace header appear at byte zero. An implementation that writes a valid new prefix without truncating/replacing the full destination could leave the old sentinel or other stale bytes after the new data and still pass these checks. The test therefore does not establish that the complete PNG and trace files were replaced successfully on either namespace.
**Fix:** Validate each complete output, including PNG chunk termination at EOF and complete JSONL records with a final newline; treat read/parse errors as failure. Alternatively, compare against a fully validated expected artifact when the run's deterministic inputs make that appropriate.

The case is invoked once for the extended drive namespace and once for the extended UNC namespace by `tests/runner/windows_paths.ps1`. Allocation bounds, owned-file cleanup, and the added replacement snapshot cleanup are sound on the reviewed paths. Tests were not run during this review.

_Follow-up reviewed: 2026-11-19T10:21:08Z_
_Reviewer: the agent (gsd-code-reviewer)_
_Depth: standard_

## Follow-up Review — 2026-11-19T10:25:00Z

**Scope:** Updated successful replacement checks and bounded sentinel scanner in `tests/runner/windows_paths.c`.
**Depth:** standard
**Status:** issues_found

WR-04 is resolved. `file_contains_bytes` scans the complete file in bounded chunks, carries enough overlap to detect matches across chunk boundaries, and returns an error on open, read, or close failure; the call sites reject any nonzero result. The case continues to run for both drive and UNC namespaces. One remaining issue affects the battery replacement assertion:

### WR-05 (WARNING): Battery comparison treats read errors as byte differences

**File:** `tests/runner/windows_paths.c:378-410,1133`
**Severity:** WARNING
**Issue:** `equal_files` returns `0` both when file contents differ and when either open/read fails. The replacement assertion fails only when it returns `1`, so an I/O error while comparing the saved pre-run battery with the new battery is interpreted as proof that the output changed. In that case the test can pass without establishing battery replacement, including on the SMB-backed UNC path.
**Fix:** Make this comparison tri-state (equal, different, error) and fail the test on an error; require a completed byte comparison that reports different contents.

Tests were not run during this review.

_Follow-up reviewed: 2026-11-19T10:25:00Z_
_Reviewer: the agent (gsd-code-reviewer)_
_Depth: standard_

## Follow-up Review — 2026-11-19T10:26:12Z

**Scope:** Tri-state battery comparison in `tests/runner/windows_paths.c`.
**Depth:** standard
**Status:** clean

WR-05 is resolved. `compare_files` returns `-1` for open, size-query, read, short-read, or close failures; it returns `0` only after equal-sized files compare byte-for-byte and `1` only after a completed comparison establishes different sizes or bytes. The replacement assertion requires exactly `1`, so comparison errors now fail the test. Both acquired handles are closed on all paths; the legacy `equal_files` wrapper still treats only `0` as equal for its existing callers. Together with the preceding sentinel-scan change, no finding remains in the reviewed replacement coverage.

Tests were not run during this review.

_Follow-up reviewed: 2026-11-19T10:26:12Z_
_Reviewer: the agent (gsd-code-reviewer)_
_Depth: standard_

## Follow-up Review — 2026-11-19

**Scope:** New suffix-distinct output cases in `tests/runner/windows_paths.c`.
**Status:** clean

The drive and SMB UNC cases write a state file and PNG whose extended paths differ only by a literal trailing dot or space. The test verifies both outputs exist, the PNG signature and state reload succeed, and the paths resolve to different volume/file identities. File open, identity-query, close, and cleanup failures all fail the case. No actionable issue or MSVC compatibility risk found; the file already uses `FILE_ID_INFO` and `FileIdInfo`.

The exact-source Windows x64 and ARM64 CI and Release jobs subsequently passed these cases on source `c0c112e`; see `29-ACCEPTANCE.md`. The current follow-up includes the suffix paths in the maximum-length measurement. Its early existence check remains limited to persistent outputs; the suffix helper validates its temporary output pair before cleaning both files. The next source-qualified run confirms this measurement extension.
