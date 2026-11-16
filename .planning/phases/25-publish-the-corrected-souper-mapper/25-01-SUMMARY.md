---
phase: 25-publish-the-corrected-souper-mapper
plan: "01"
subsystem: release
tags: [souper, release-please, ci, documentation]
requires: []
provides:
  - Published the corrected Souper mapper in the stable v0.16.0 package.
  - Verified version-neutral availability documentation on main.
affects: [release, documentation, souper]
actuals:
  tokens: 11338
  tasks: 2
  commits: 3
  plan_head_before: 4649c299b29550dd90c8688fe350d3ac0f8f8743
  plan_head_after: f3e1a24d8a1372bfc689c19cb1d91dfad0575d97
tech-stack:
  added: []
  patterns:
    - Release and final-main verification are recorded against their separate exact source commits.
key-files:
  created:
    - .planning/phases/25-publish-the-corrected-souper-mapper/25-RELEASE.md
    - .planning/phases/25-publish-the-corrected-souper-mapper/25-01-SUMMARY.md
  modified:
    - README.md
    - docs/status.md
key-decisions:
  - Used the ordinary pre-1.0 release-please feature path; release-please supplied the published package source at a0319a9c.
  - Kept public package copy version-neutral and changed it only after publication and latest-install verification.
  - Retained existing CI and release gates after timing review found no evidence-backed pipeline change.
requirements-completed:
  - TUNE-13
  - REL-01
  - REL-02
  - DOC-01
coverage:
  - id: TUNE-13
    description: Reviewed release and CI queue, critical path, duration, skips, and cancellations; recorded a bounded no-change disposition.
    requirement: TUNE-13
    verification:
      kind: integration
      ref: .planning/phases/25-publish-the-corrected-souper-mapper/25-RELEASE.md#final-queue-and-critical-path-review; runs 43813049138 and 43818074765
      status: pass
    human_judgment: false
  - id: REL-01
    description: Published stable v0.16.0 from the release-please source and confirmed the v0.15.0 release, tag, notes, and assets stayed identical.
    requirement: REL-01
    verification:
      kind: e2e
      ref: .planning/phases/25-publish-the-corrected-souper-mapper/25-RELEASE.md#preserved-release-baseline; release run 43801428444; old-release cmp exit 0
      status: pass
    human_judgment: false
  - id: REL-02
    description: Exact-source required CI, release equality, archive integrity and provenance, publication, and latest installation passed for a0319a9c.
    requirement: REL-02
    verification:
      kind: e2e
      ref: .planning/phases/25-publish-the-corrected-souper-mapper/25-RELEASE.md#release-acceptance-record; run 43801428444; latest install and all required gates passed
      status: pass
    human_judgment: false
  - id: DOC-01
    description: README and status describe all three deliverables, published Souper availability, generated-fixture evidence, compatibility limits, and scoped completion.
    requirement: DOC-01
    verification:
      kind: integration
      ref: README.md; docs/status.md; PR 73 CI 43813049138; main CI 43818074765; single-version checks and document budgets passed
      status: pass
    human_judgment: false
duration: 5h
completed: 2026-11-16
status: complete
---

# Phase 25 Plan 01: Publish the Corrected Souper Mapper Summary

**Corrected Souper is available in the published v0.16.0 package, with the old v0.15.0 draft preserved and version-neutral user documentation verified on green main.**

## Performance

- **Duration:** 5h, measured from the first recorded plan check at 2026-11-16T02:22:02Z through final main CI success at 2026-11-16T07:22:04Z.
- **Started:** 2026-11-16T02:22:02Z (first recorded plan check; executor start preceded this receipt timestamp).
- **Completed:** 2026-11-16T07:22:04Z (final main CI completed successfully).
- **Tasks:** 2.
- **Files modified by plan tasks:** 3 tracked paths: README.md, docs/status.md, and the release receipt.
- **Measured main commits after the plan ledger:** 3 (`git rev-list --count 4649c299..f3e1a24`); original task commits and their squash merges are listed separately below.

## Accomplishments

- Published stable v0.16.0 from exact release source `a0319a9c29886d776e37b818d38d36fe810f486d`. All required source-bound CI/equality, 19 ZIP plus checksum asset admission, integrity/provenance, publication, and latest-install gates passed.
- Re-fetched and normalized the v0.15.0 release, full paginated assets, and tag after final documentation merge. Before/after comparison passed; canonical SHA-256 remained `4aa0ea4e2d5090918e6d4c03c60bc8dd8efa7090bba050846f802e7266e2c9ad`.
- Merged the docs follow-up and verified final main CI on `f3e1a24d8a1372bfc689c19cb1d91dfad0575d97`. Eighteen jobs passed, with only the expected push-event title and perf jobs skipped.
- Updated README/status with version-neutral latest-package wording, the unchanged install command, all three deliverables, generated-fixture scope, compatibility limits, and the finite completion standard.
- Reviewed queue and critical-path evidence. No safe workflow, cache, deadline, test-inventory, quality-floor, or job-membership change was supported; existing Windows ARM64 independent-base coverage remains intact.

## Task Commits and Source Provenance

1. **Task 1: Deliver corrected Souper through the existing release path** — original task commit `e0df7b5d7e0810185be790fff1e77fb3b7a18b89`. Feature PR #71 merged as `4486a25e20020b6eec9949c5628cbbdfd686f6a4`; ordinary release-please PR #72 then produced accepted release source `a0319a9c29886d776e37b818d38d36fe810f486d`.
2. **Task 2: Publish accurate availability documentation and finish on green main** — original task commit `1a715441544676394e9bb16cccdd1acd5c2ba75a`; docs PR #73 merged as `f3e1a24d8a1372bfc689c19cb1d91dfad0575d97`.

The release source `a0319a9c` is distinct from the documentation merge and final-main CI source `f3e1a24d`. The receipt's later metadata-only updates are not represented as independently product-tested sources.

## Files Created/Modified

- [README.md](../../../README.md) — concise deliverables and verified latest-package availability; install line unchanged.
- [docs/status.md](../../../docs/status.md) — published support, evidence scope, compatibility limits, and completion standard.
- [25-RELEASE.md](./25-RELEASE.md) — source-bound release gates, old-release fingerprints, PR/run evidence, and timing review.
- `25-01-SUMMARY.md` — plan outcome and requirement coverage.

## Decisions Made

- Used the normal release-please feature trigger and let it own version/tag creation. No manual version override, tag, or release was made.
- Changed public availability copy only after the release and unchanged latest-install check passed.
- Kept all existing required/advisory CI membership and release controls unchanged because the measured runs supported no safe correction.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 3 - Blocking] Restored the required compatibility disclaimer in README**
- **Found during:** Task 1 local CI.
- **Issue:** The existing claims-wording policy rejected the first documentation edit because its required scope disclaimer was missing.
- **Fix:** Added the repository's exact required sentence; focused policy and subsequent full CI passed.
- **Files modified:** `README.md`.
- **Verification:** Focused `policy.claims_wording` passed; full CI passed 723/723 standard tests and all 8 release-package tests.
- **Committed in:** Task 1 delivery history before the release merge.

**Total deviations:** 1 auto-fixed blocking issue.
**Impact on plan:** Required to satisfy an existing documentation contract; no product or compatibility scope was added.

## Issues Encountered

- The clean docs checkout initially lacked six pinned build/test inputs. They were fetched with the existing SHA-verifying `scripts/fetch-pin.sh`, then the CI and hygiene presets configured successfully. No package or dependency was added.
- An initial CTest selector targeted the wrong configured inventory and ran no tests; `ctest -N` identified the owning inventory, and the corrected command passed.
- The first GitHub run response omitted the attempt field needed by the exact-source checker. A complete `gh run view` query returned the attempt and job data, after which the checker passed. No hosted job was retried.
- The stub-pattern scan matched “not available” in the status page's explicit OpenHSC/AtariVox compatibility limits; this is an intentional supported-scope statement, not a stub.

## User Setup Required

None. No owner-run UI, RetroArch, or hardware check was needed; the authorized hosted CI integration passed.

## Next Phase Readiness

Both plan tasks and their source-bound evidence are complete. The orchestrator owns shared security/review/verifier gates, STATE/ROADMAP/REQUIREMENTS updates, and the final phase closeout PR. The milestone worktree's dirty STATE update remains preserved; this closeout checkout changed no shared tracking files.

---
*Phase: 25-publish-the-corrected-souper-mapper*
*Completed: 2026-11-16*

## Self-Check: PASSED

- Summary and release receipt exist in the closeout checkout.
- Original task commits `e0df7b5` and `1a71544` exist; their main integration commits `4486a25`, `a0319a9`, and `f3e1a24` are present on the accepted main history.
- Final main CI run 43818074765 completed successfully on the exact `f3e1a24d` source; release run 43801428444 and old-release equality are recorded in the receipt.
- The closeout branch changes only the receipt and this summary; STATE, ROADMAP, and REQUIREMENTS remain untouched.
