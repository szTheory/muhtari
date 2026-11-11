---
phase: 18-source-bound-emscripten-release-bundle
fixed_at: 2026-11-11T05:25:52Z
review_path: .planning/phases/18-source-bound-emscripten-release-bundle/18-REVIEW.md
iteration: 1
findings_in_scope: 3
fixed: 3
skipped: 0
status: all_fixed
---

# Phase 18: Code Review Fix Report

**Fixed at:** 2026-11-11T05:25:52Z
**Source review:** `.planning/phases/18-source-bound-emscripten-release-bundle/18-REVIEW.md`
**Iteration:** 1

**Summary:** 3 findings in scope; 3 fixed; none skipped. Fixes are ready for the orchestrator's atomic commit.

## Fixed Issues

### CR-01: Required publisher hygiene fixture still targets 18 archives

**Files modified:** `tests/hygiene/tests.cmake`
**Applied fix:** The positive hygiene fixture now includes a synthetic source-bound portable ZIP, its matching sidecar/native receipt, and six native commit receipts. It asserts 19 sorted checksums, delegates the real firmware scan over all 19 ZIPs, and keeps the native omission control's expected diagnostic.
**Commit:** Pending orchestrator commit.

### CR-02: Portable ZIP is extracted without a pre-extraction size bound

**Files modified:** `scripts/publish.sh`, `tests/emscripten/bundle.cmake`, `tests/policy/publish.cmake`
**Applied fix:** Publisher preparation validates the portable bundle before the existing firmware scan extracts any ZIP; the scan still receives all 19 archives. Bundle validation reads each uncompressed member size before extraction and rejects members over 8 MiB or totals over 16 MiB. The controls pass a valid bundle and reject an 8 MiB + 1 byte member and an aggregate above 16 MiB before creating the extraction directory.
**Resource basis:** The available 14-member ZIP measured 321,034 uncompressed bytes: 191,040-byte Wasm, 57,902-byte loader, 58,270 bytes across five pinned notices, and 13,822 bytes of remaining metadata/docs. The 16 MiB scratch extraction ceiling allows over 50× current aggregate growth, while the 8 MiB per-member ceiling prevents one member from consuming most of that allowance. These are uncompressed expansion bounds; DEC.61's retired compressed-size quota remains retired.
**Status:** Fixed and confirmed by the independent code-review recheck; no owner validation is required.
**Commit:** Pending orchestrator commit.

### CR-03: Lexical WORK containment permits extraction cleanup through a symlink

**Files modified:** `tests/emscripten/bundle.cmake`
**Applied fix:** WORK components are checked for symlinks and resolved against the physical BUILD path. The validator checks the extraction target and descendants before cleanup, then creates a fresh contained extraction directory. Controls point both WORK and `WORK/extract` at outside directories and verify rejection preserves each outside sentinel.
**Status:** Fixed and confirmed by the independent code-review recheck; no owner validation is required.
**Commit:** Pending orchestrator commit.

## Verification

All runs were local; run URL: N/A.

| Command | Exit |
|---|---:|
| `cmake --preset hygiene` | 0 |
| `ctest --test-dir build/hygiene -R '^policy\.publish_set(\.self_test)?$' --output-on-failure` | 0 |
| `cmake -DROOT="$PWD" -DWORK="$PWD/build/phase18-round1-bundle-controls-final" -DMODE=controls -P tests/emscripten/bundle.cmake` | 0 |
| `sh -n scripts/publish.sh` | 0 |
| `git diff --check` | 0 |
| `cmake -DROOT="$PWD" -DBUILD="$PWD/build/emscripten" -DWORK="$PWD/build/emscripten/tests/phase18-round1-production-validate" -DMODE=validate -DEXPECTED_SHA=63e5e19ab3f3b96564fdc20c01e6217ae3900506 -DARCHIVE="$PWD/build/emscripten/muhtari-libretro-emscripten.zip" -DNATIVE="$PWD/build/ci/tests/emscripten/native.json" -DFIXTURE="$PWD/tests/roms/manifest.txt" -P tests/emscripten/bundle.cmake` | 0 |
| `cmake -DROOT="$PWD" -DWORK="$PWD/build/phase18-round1-production-publish-controls" -DPRODUCTION_ARCHIVE="$PWD/build/emscripten/muhtari-libretro-emscripten.zip" -DEXPECTED_SHA=63e5e19ab3f3b96564fdc20c01e6217ae3900506 -P tests/policy/publish.cmake` | 0 |

The cached ZIP was checked against its own embedded commit (`63e5e19…`), not the newer checkout HEAD. The production-publisher control passed preparation and rejected mutations of all five qualified notices through the real validator. The full native and SDK workflows were not rerun in this fix round; the phase executor owns those final gates.
