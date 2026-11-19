---
phase: 28-measured-milestone-tune-up
reviewed: 2026-11-19T19:08:57Z
depth: standard
files_reviewed: 9
files_reviewed_list:
  - .github/workflows/ci.yml
  - scripts/classify-changes.sh
  - scripts/classify-pr-paths.jq
  - tests/hygiene/tests.cmake
  - tests/inventory/hygiene.txt
  - tests/cmake/quality_evidence.cmake
  - tests/policy/quality.cmake
  - docs/maintaining.md
  - docs/testing.md
findings:
  critical: 0
  warning: 0
  info: 0
  total: 0
  necessity: 0
status: clean
---

# Phase 28: Code Review Report

**Reviewed:** 2026-11-19T19:08:57Z
**Depth:** standard
**Files Reviewed:** 9
**Status:** clean

## Summary

The initial review found that line-feed filenames could spoof the release
metadata allowlist. The follow-up filter checks both current and prior names
before emitting newline-delimited paths. If either contains a line feed, it
emits only `(listing failed)`, which forces the heavy matrix. Regression cases
for embedded, trailing, and prior-name line feeds passed. The exact three-path
allowlist, uniqueness/count checks, trusted workflow guard, and release
publication gates remain intact.

The follow-up baseline-root change selects `RUNNER_TEMP` only for the
`ci-msvc` baseline when the variable is present. Linux/macOS and local
baselines retain `${BUILD}/quality-base`. It changes only the independent
Windows checkout location: the existing five workflow steps, serialized
CTest, report locations, source/toolchain/fixture comparisons and package
evidence checks are unchanged. A policy control checks both the Windows path
and non-Windows fallback; its producer cases continue using the local
fallback. Windows CI jobs each run one producer on isolated hosted runners,
so the temporary path does not collide in the checked-in topology. No findings
remain.

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

The path helper uses `RUNNER_TEMP/muhtari-quality-base` only for
`CONFIG=ci-msvc` when `RUNNER_TEMP` is nonempty; other configurations, and
`ci-msvc` without that environment variable, retain `${BUILD}/quality-base`
(`tests/cmake/quality_evidence.cmake:62-68,245-249`). The policy control
explicitly verifies the Windows selection and that a non-Windows config keeps
the fallback (`tests/policy/quality.cmake:420-429`). Existing producer cases
continue on the fallback path, assert the checkout exists there, and remove it
plus prune the fake repository's worktree registration before switching bases
(`tests/policy/quality.cmake:1074,1102-1108`). The actual workflow invokes the
producer once per hosted job, including separate Windows matrix jobs
(`.github/workflows/ci.yml:97-110,255-268,329-342`); the fixed Windows temp
path therefore does not collide within the checked-in topology. Successful
hosted jobs discard that temporary runner state. The five Windows steps and
serialized tests are still checked and executed unchanged.
The documentation accurately describes this path and its scope
(`docs/maintaining.md:84-85`); `docs/testing.md` continues to describe the
classifier and roll-up exception accurately (`docs/testing.md:217-245`). The
parent-reported focused policy-quality and full hygiene passes were not rerun
in this read-only review, as requested. No remaining issues were found in the
requested follow-up.

---

_Reviewed: 2026-11-19T19:08:57Z_
_Reviewer: the agent (gsd-code-reviewer)_
_Depth: standard_
