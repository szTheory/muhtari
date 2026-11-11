---
phase: 18-source-bound-emscripten-release-bundle
reviewed: 2026-11-11T05:04:23Z
depth: standard
review_scope: interim_code
snapshot_commit: 082a7fbac36bf446473a4e3b0d716c720b1f9783
diff_base: 744c499
files_reviewed: 11
files_reviewed_list:
  - scripts/emscripten.sh
  - tests/emscripten/bundle-members.txt
  - tests/emscripten/bundle.cmake
  - tests/emscripten/tests.cmake
  - tests/inventory/emscripten.txt
  - THIRD-PARTY-NOTICES.md
  - docs/emscripten-hosting.md
  - scripts/publish.sh
  - tests/policy/publish.cmake
  - .github/workflows/ci.yml
  - .github/workflows/release.yml
findings:
  critical: 3
  warning: 0
  info: 0
  total: 3
  necessity: 0
status: issues_found
---

# Phase 18: Code Review Report

**Reviewed:** 2026-11-11T05:04:23Z  
**Depth:** standard  
**Files Reviewed:** 11  
**Status:** issues_found  
**Review scope:** interim code review of committed snapshot `082a7fb`; integrator documentation/checker review is still pending. This is not phase acceptance or the final review.

## Summary

The publisher now requires a nineteenth ZIP and validates the portable bundle, but the existing required hygiene publisher fixture still constructs and expects the former eighteen-ZIP contract. The new archive path also needs bounded, contained extraction before any scan extracts it. The remaining integrator documentation/checker review is pending; this report covers only the listed committed source snapshot.

## Critical Issues

### CR-01: [BLOCKER] Required publisher hygiene fixture still targets 18 archives

**File:** `scripts/publish.sh:29,53,64-74`; affected existing test `tests/hygiene/tests.cmake:236-291`  
**Issue:** `names()` adds `muhtari-libretro-emscripten.zip`, `prepare()` now requires 19 archives and invokes the bundle validator with the Emscripten evidence tree. The existing `policy.publish_set` fixture still creates 18 ZIPs and expects `prepare` to succeed with 18 checksums; it now fails first because the portable ZIP is missing. Its negative control still expects the missing `muhtari-runner-windows-x64.zip` diagnostic, but `prepare` reports the newly missing portable ZIP instead. This makes the required hygiene test suite fail on the reviewed change.
**Fix:** Update the hygiene fixture to include a valid synthetic portable ZIP and matching evidence/native receipt, assert 19 sorted checksums, and target an omission whose expected diagnostic is still the first failure.

### CR-02: [BLOCKER] Portable ZIP is extracted without a pre-extraction size bound

**File:** `scripts/publish.sh:64-74`; `tests/emscripten/bundle.cmake:245-296`; existing scan extraction at `tests/cmake/firmware_scan.cmake:198-199`  
**Issue:** `prepare` sends all ZIPs, including the new portable archive, through `firmware_scan.cmake` before invoking the new bundle validator. That scan extracts the archive before the portable member checks run. The bundle validator also calls `file(ARCHIVE_EXTRACT)` after checking paths and file types, without checking per-member or aggregate uncompressed sizes. A crafted ZIP with the required member names but an extreme declared expansion can consume runner disk before hash and source-identity checks reject its contents; path checks in the later validator cannot protect the earlier extraction.
**Fix:** Run strict portable-bundle preflight before the portable ZIP reaches the existing scan, and reject excessive per-member/aggregate uncompressed sizes before either extraction. Derive the accepted sizes from the measured build outputs and fixed notice/document inputs, and cover the bound with an over-limit fault control.

### CR-03: [BLOCKER] Lexical WORK containment permits extraction cleanup through a symlink

**File:** `tests/emscripten/bundle.cmake:25-39,293-296`  
**Issue:** `_muhtari_bundle_normalize` checks that `WORK` is lexically below `BUILD` using normalized path strings, but does not resolve or reject symlinked path components. `muhtari_bundle_validate` then removes `${WORK}/extract` and extracts into it. If a `WORK` path below `BUILD` is a symlink to an outside directory, the prefix check passes and the cleanup/extraction path resolves outside the claimed build-owned area, risking deletion or overwrite of unrelated files.
**Fix:** Resolve the existing `BUILD` and `WORK` path components before the containment check, reject symlinked `WORK`/extraction paths, and create/use only a fresh private extraction directory whose resolved path is inside the resolved build directory.

---

_Reviewed: 2026-11-11T05:04:23Z_  
_Reviewer: the agent (gsd-code-reviewer)_  
_Depth: standard_
