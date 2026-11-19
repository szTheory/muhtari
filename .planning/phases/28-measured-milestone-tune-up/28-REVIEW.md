---
phase: 28-measured-milestone-tune-up
reviewed: 2026-11-19T18:29:26Z
depth: standard
files_reviewed: 5
files_reviewed_list:
  - .github/workflows/ci.yml
  - scripts/classify-changes.sh
  - scripts/classify-pr-paths.jq
  - tests/hygiene/tests.cmake
  - tests/inventory/hygiene.txt
findings:
  critical: 0
  warning: 0
  info: 0
  total: 0
  necessity: 0
status: clean
---

# Phase 28: Code Review Report

**Reviewed:** 2026-11-19T18:29:26Z
**Depth:** standard
**Files Reviewed:** 5
**Status:** clean

## Summary

The initial review found that line-feed filenames could spoof the release
metadata allowlist. The follow-up filter checks both current and prior names
before emitting newline-delimited paths. If either contains a line feed, it
emits only `(listing failed)`, which forces the heavy matrix. Regression cases
for embedded, trailing, and prior-name line feeds passed. The exact three-path
allowlist, uniqueness/count checks, trusted workflow guard, and release
publication gates remain intact. No findings remain.

## Narrative Findings (AI reviewer)

The previously reported CR-01 is closed by `scripts/classify-pr-paths.jq:1-5`
and the workflow wiring at `.github/workflows/ci.yml:75-77`. When a current or
prior filename contains LF, the filter emits only the failure sentinel; the
classifier returns `heavy=true` for that sentinel. The classifier tests cover
embedded, trailing, and previous-filename LF cases.

The API pipeline retains fail-closed behavior on partial failure. With Bash
`pipefail`, a simulated `gh api` that emits one valid partial page and exits
nonzero causes the group to append `(listing failed)` after the partial path;
the classifier returned `heavy=true`. The exact-source release CI and equality
requirements in `release.yml:273-299` remain unchanged. The focused CTest run
passed both classifier and workflow-guard tests.

---

_Reviewed: 2026-11-19T18:29:26Z_
_Reviewer: the agent (gsd-code-reviewer)_
_Depth: standard_
