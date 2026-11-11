---
phase: 18-source-bound-emscripten-release-bundle
reviewed: 2026-11-11T05:41:58Z
depth: standard
review_scope: round2_recheck
snapshot_commit: 26df012daa8cd61c3f32669a5a7feb45fef8e30b
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

**Reviewed:** 2026-11-11T05:41:58Z
**Depth:** standard
**Files Reviewed:** 17
**Status:** issues_found

## Summary

Rechecked the README checker change at `26df012`. It now requires the exact Markdown link syntax and excludes backtick-fenced examples, closing the original WR-01 plain-text/fenced-example false positive. A narrower false positive remains: inline-code link syntax and tilde-fenced examples are not removed before matching. The earlier bounded recheck at `c590569` remains valid: the publisher validates and bounds the portable ZIP before generic firmware extraction; the per-member 8 MiB and aggregate 16 MiB uncompressed limits have a stated scratch-disk objective, use the measured 321,034-byte current package (191,040-byte Wasm) as their basis, and have pass plus over-limit controls that assert rejection before extraction. WORK and extraction symlink controls preserve outside sentinels. The packaged paths and Node example agree, and documentation limits its claims to offline Node/native behavior.

The prior interim blockers CR-01 through CR-03 and WR-01 are resolved. One warning remains. This is a code-review disposition only; the final current-source native and SDK gates remain with the executor.

## Warnings

### WR-02: README checker accepts link syntax inside inline or tilde-fenced code

**File:** `tests/cmake/install_line.cmake:39-60,81-82`
**Issue:** The checker removes only triple-backtick fences, then searches for the literal Markdown link syntax. A README with the required link only inside inline code (for example, `` `[version-free download URL](...)` ``) still matches, even though Markdown renders it as code rather than a clickable link. Tilde-fenced examples are also left in `portable_text`. The current README links are correct, but these cases can pass without user-facing links.
**Fix:** Exclude inline-code spans and both supported Markdown fence forms before matching, or use a parser that recognizes links outside code. Add an isolated README control with the expected link syntax only inside inline code and tilde fences and require named failures through the existing checker.

## Resolved Interim Findings

These IDs and descriptions are retained for auditability; they are not active findings in this recheck.

### WR-01: README contract check accepted plain text in place of links — fixed

Commit `26df012` now requires the exact portable URL and guide Markdown-link forms outside triple-backtick fences. Its fix report records expected rejection for missing markup, plain URL/path, wrong ZIP target, and links confined to a backtick-fenced example. The original finding is closed; WR-02 records the remaining code-span/fence edge case.

### CR-01: Required publisher hygiene fixture still targets 18 archives — fixed

`tests/hygiene/tests.cmake` now constructs a valid synthetic source-bound portable ZIP and matching producer evidence, retains six native commit receipts, asserts 19 ordered checksums, and runs the real firmware scan over all 19 ZIPs. The omission control still targets a missing native ZIP and checks its diagnostic.

### CR-02: Portable ZIP was extracted before validation and lacked expansion bounds — fixed

`scripts/publish.sh` calls strict bundle validation before the firmware scan. `tests/emscripten/bundle.cmake` reads the uncompressed sizes before `file(ARCHIVE_EXTRACT)`, rejects a member above 8 MiB or a total above 16 MiB, and tests both faults before an extraction directory is created. This is an uncompressed expansion bound; it does not reintroduce the retired compressed archive quota.

### CR-03: Lexical WORK containment permitted cleanup through a symlink — fixed

The validator rejects symlinked WORK path components, verifies physical containment, and checks the existing extraction tree before cleanup. Controls exercise both an outside WORK symlink and an outside extraction symlink and verify their outside sentinels remain unchanged.

---

_Reviewed: 2026-11-11T05:41:58Z_
_Reviewer: the agent (gsd-code-reviewer)_
_Depth: standard_
