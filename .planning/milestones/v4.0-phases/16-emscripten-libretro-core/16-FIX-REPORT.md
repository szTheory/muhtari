---
phase: 16-emscripten-libretro-core
review: 16-REVIEW.md
round: 1
fixed: 5
skipped: 0
status: all_fixed
---

# Phase 16 first-round review fix report

All five in-scope findings were fixed. No findings were skipped.

## Fixed findings

### CR-01: Core source identity omitted implementation headers

The source identity now hashes every src/*.h and host/*.{c,h} input, plus the public/libretro/runner SHA-256 inputs and version-generation inputs. The generated version header is hashed under a build-independent name and participates in configure dependencies.

**Verification:** Disposable-copy header-only identity control (append to src/internal.h, rebuild, restore, rebuild): exit 0; identity changed and then returned to its original value. cmake --workflow --preset ci: exit 0. sh scripts/emscripten.sh build/ci: exit 0. Run URL: N/A-local.

### CR-02: Perl timeout fallback accepted signaled child termination and stale output

The fallback now decodes wait status and maps signal termination to 128 + signal. The lane deliberately checks normal exit 0, exit 37, SIGTERM (143), and timeout (124). Old JUnit, measurement, and Wasm-surface outputs are removed before the container starts.

**Verification:** Fallback controls ran through sh scripts/emscripten.sh build/ci: exit 0, all four expected statuses observed. The required lane rebuilt and validated fresh JUnit, measurement, and Wasm-surface outputs. Run URL: N/A-local.

### WR-01: Observer report replacement failed on Windows

The observer now uses MoveFileExA with replace-existing and write-through flags on Windows, and rename on POSIX.

**Verification:** Ran the native observer twice against the same existing report path; both runs exited 0. Reviewed the Windows branch against the existing MoveFileExA replacement in runner/main.c. Windows execution: N/A-local. Run URL: N/A-local.

### WR-02: Required production exports were incomplete

The Node consumer checks all 24 retained factory functions and all four retained runtime exports (addFunction, removeFunction, HEAPU8, HEAPU32). Its control mode omits each required item individually and requires the named diagnostic.

**Verification:** sh scripts/emscripten.sh build/ci: exit 0; both exact Emscripten inventory tests passed, including every per-export omission control. Run URL: N/A-local.

### WR-03: Container source mount was writable

Both source mounts are read-only. The build and evidence paths are mounted separately as the only writable subdirectories, and the container checks that the source write probe fails while both output mounts remain writable.

**Verification:** sh scripts/emscripten.sh build/ci: exit 0 with the read-only mount probe and writable-output checks enabled. Run URL: N/A-local.

## Other required gates

| Command | Result |
|---|---|
| sh -n scripts/emscripten.sh | Exit 0 |
| node --check tests/emscripten/host.mjs | Exit 0 |
| cmake --workflow --preset ci | Exit 0; native workflow and release workflow passed |
| sh scripts/emscripten.sh build/ci | Exit 0; pinned artifact, exact inventory, fresh receipts, and mount controls passed |
| cmake -DROOT=. -P tests/cmake/budgets.cmake | Exit 0; 22 files within budget |
| sh scripts/hygiene.sh --tree | Exit 0 |
| git diff --check b2c6234a6cb8a74a2dd99edc2eb60c3b9760b540 -- | Exit 0 |

All runs were local; run URL: N/A-local. Fix commits and phase summary/state bookkeeping remain parent-owned.
