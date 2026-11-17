---
phase: 27-headless-developer-trace
reviewed: 2026-11-17T00:04:25Z
depth: standard
files_reviewed: 19
files_reviewed_list:
  - .github/workflows/ci.yml
  - CMakeLists.txt
  - cmake/TraceLoopConfig.cmake
  - docs/runner.md
  - docs/testing.md
  - runner/main.c
  - src/bus.c
  - src/frame.c
  - src/internal.h
  - src/maria.c
  - tests/cmake/trace_loop_config.cmake
  - tests/inventory/ci.txt
  - tests/inventory/wasm32.txt
  - tests/perf/tests.cmake
  - tests/perf/trace.cmake
  - tests/runner/test_save.c
  - tests/runner/test_trace.c
  - tests/runner/tests.cmake
  - tests/runner/trace.cmake
findings:
  critical: 0
  warning: 1
  info: 0
  total: 1
  necessity: 0
status: issues_found
---

# Phase 27: Code Review Report

**Reviewed:** 2026-11-17T00:04:25Z  
**Depth:** standard  
**Files Reviewed:** 19  
**Status:** issues_found

## Summary

The prior junction collision, trace-off test-build, and ordinary Windows trailing-dot/space findings are fixed. The new Windows controls cover junctions and ordinary Win32 trailing suffixes. The trace-off config control and runner path tests passed locally (`ctest --test-dir build/ci --output-on-failure -R '^runner[.]trace[.](paths|disabled_config)$'`, exit 0). One compatibility warning remains for explicit extended-length Windows paths. This host has no Windows compiler/runtime, so the Windows-specific code and controls were reviewed from source but not executed here.

## Warnings

### WR-01: Extended Windows paths are compared with ordinary path semantics

**Severity:** WARNING  
**File:** `runner/main.c:781-785, 895-903, 1772-1775`  
**Issue:** The comparator strips trailing ASCII dots and spaces from every basename. A caller can use an extended path such as `\\?\C:\out\trace.jsonl.` to name a real file whose trailing period is preserved; the `\\?\` namespace disables ordinary Win32 path parsing, and `MoveFileExA` accepts this extended path form. Therefore two distinct outputs such as `\\?\C:\out\trace.jsonl.` and `\\?\C:\out\trace.jsonl` compare equal and are rejected as a conflict. See Microsoft's [extended path naming rules](https://learn.microsoft.com/en-us/windows/win32/fileio/naming-a-file) and [MoveFileExA path rules](https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-movefileexa).
**Fix:** Preserve basename suffixes when the caller supplied an extended namespace path, or explicitly reject that namespace with a clear diagnostic. If extended paths remain supported, add a Windows runner control proving distinct trailing-dot and ordinary destinations are accepted while ordinary-namespace aliases are still rejected.

---

_Reviewed: 2026-11-17T00:04:25Z_  
_Reviewer: the agent (gsd-code-reviewer)_  
_Depth: standard_
