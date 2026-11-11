---
phase: 18-source-bound-emscripten-release-bundle
reviewed: 2026-11-11T05:35:33Z
depth: standard
review_scope: round1_recheck_plus_unreviewed_delta
snapshot_commit: c590569ccac329a70a593d49d07286445cae2674
diff_base: 744c499
files_reviewed: 17
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
  - tests/hygiene/tests.cmake
  - README.md
  - docs/libretro.md
  - PROVENANCE.md
  - tests/cmake/install_line.cmake
  - docs/testing.md
findings:
  critical: 0
  warning: 1
  info: 0
  total: 1
  necessity: 0
status: issues_found
---

# Phase 18: Code Review Report

**Reviewed:** 2026-11-11T05:35:33Z  
**Depth:** standard  
**Files Reviewed:** 17  
**Status:** issues_found

## Summary

Rechecked the first-round fixes and the remaining release, test, provenance, and integrator-documentation delta at `c590569`. The publisher now validates and bounds the portable ZIP before the generic firmware scan extracts any archive. The validator's per-member 8 MiB and aggregate 16 MiB uncompressed limits have a stated scratch-disk objective, use the measured 321,034-byte current package (191,040-byte Wasm) as their basis, and have pass plus over-limit controls that assert rejection before extraction. WORK and extraction symlink controls preserve outside sentinels. The packaged paths and the Node example in `docs/libretro.md` agree, and its evidence claims stop at offline Node/native behavior.

The prior interim blockers CR-01 through CR-03 are fixed and rechecked below. One warning remains: the README checker looks for URL text, so it does not prove that the URL and guide remain clickable Markdown links. This is a code-review disposition only; the final current-source native and SDK gates remain with the executor.

## Warnings

### WR-01: README contract check accepts plain text in place of links

**File:** `tests/cmake/install_line.cmake:36,44`  
**Issue:** The checker uses `string(FIND)` for the expected URL and guide path anywhere in the README section. A change from Markdown links to plain text, or to a fenced example containing the expected text, still passes even though the README no longer provides the required clickable download and guide links. The current README has both links; the recurring check does not protect that contract.  
**Fix:** Match the Markdown link forms (for example, `[version-free download URL](${portable_url})` and `[portable host instructions](${portable_guide})`) and exercise a copy with link markup removed through the existing checker, expecting a named failure.

## Resolved Interim Findings

These IDs and descriptions are retained from the interim report at snapshot `082a7fb` for auditability; they are not active findings in this recheck.

### CR-01: Required publisher hygiene fixture still targets 18 archives — fixed

`tests/hygiene/tests.cmake` now constructs a valid synthetic source-bound portable ZIP and matching producer evidence, retains six native commit receipts, asserts 19 ordered checksums, and runs the real firmware scan over all 19 ZIPs. The omission control still targets a missing native ZIP and checks its diagnostic.

### CR-02: Portable ZIP was extracted before validation and lacked expansion bounds — fixed

`scripts/publish.sh` calls strict bundle validation before the firmware scan. `tests/emscripten/bundle.cmake` reads the uncompressed sizes before `file(ARCHIVE_EXTRACT)`, rejects a member above 8 MiB or a total above 16 MiB, and tests both faults before an extraction directory is created. This is an uncompressed expansion bound; it does not reintroduce the retired compressed archive quota.

### CR-03: Lexical WORK containment permitted cleanup through a symlink — fixed

The validator rejects symlinked WORK path components, verifies physical containment, and checks the existing extraction tree before cleanup. Controls exercise both an outside WORK symlink and an outside extraction symlink and verify their outside sentinels remain unchanged.

---

_Reviewed: 2026-11-11T05:35:33Z_  
_Reviewer: the agent (gsd-code-reviewer)_  
_Depth: standard_
