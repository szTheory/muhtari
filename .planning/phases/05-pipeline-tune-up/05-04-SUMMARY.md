---
phase: 05-pipeline-tune-up
plan: "04"
status: complete
subsystem: infra
tags: [github-actions, reporting, triage]
requires:
  - phase: 05-01
    provides: Frozen complete count and reporting allocation
  - phase: 05-10
    provides: Diagnostic-free full equality and release-called CI evidence
provides:
  - Public read-only calendar-window report with all pages and attempts
  - Serialized default-branch rolling digest publisher and offline fault controls
affects: [05-05, 05-09]
tech-stack:
  added: []
  patterns: [POSIX sh with gh and jq, structured evidence and body files]
key-files:
  created:
    - scripts/digest.sh
    - tests/policy/digest.cmake
    - .github/workflows/digest.yml
  modified:
    - tests/policy/tests.cmake
    - tests/inventory/ci.txt
    - tests/policy/gate-lines.txt
    - docs/maintaining.md
key-decisions:
  - Keep missing compile, CTest, image, queue and DAG evidence explicitly unavailable rather than inventing measurements.
  - Preserve an older monthly section during partial updates and serialize trusted publishers independently of read-only previews.
duration: 11min
completed: 2026-10-21
actuals:
  tokens: 8008
  tasks: 2
  commits: 4
plan_head_before: 265e6cdf779ad57a331163ecd17e8cd480919746
plan_head_after: 7d524edc77c977487a734243ac01364dcbd50272
requirements-completed: []
requirements-contributed: [TUNE-01, TUNE-02]
coverage:
  - id: calendar-report
    description: Public hosted run and attempt accounting with actionable triage and explicit evidence gaps
    requirement: TUNE-01
    verification:
      - kind: unit
        ref: policy.digest.controls, exit 0
        status: pass
      - kind: integration
        ref: sh scripts/digest.sh preview build/digest-current 2026-10-21T20:00:00Z 2026-10-21T22:15:00Z, exit 0
        status: pass
    human_judgment: false
  - id: safe-preview-publisher
    description: PR preview artifact and guarded serialized rolling issue publication
    requirement: TUNE-01
    verification:
      - kind: integration
        ref: https://github.com/szTheory/muhtari/actions/runs/39685745578
        status: pass
      - kind: unit
        ref: policy.digest.controls publisher denial, duplicate, write failure and month-retention cases, exit 0
        status: pass
    human_judgment: false
  - id: retained-cost-controls
    description: Required collection failures and cancellation or intentional-control cost remain visible
    requirement: TUNE-02
    verification:
      - kind: integration
        ref: cmake --workflow --preset ci, 608 regular and six release tests, exit 0
        status: pass
      - kind: other
        ref: gate_count.cmake, 13892 physical whole-file lines within fixed 14853 ceiling, exit 0
        status: pass
    human_judgment: false
---

# Phase 5 Plan 4: Public maintenance digest Summary

**One public evidence report now has a read-only PR preview and a serialized weekly
rolling-issue publisher, with required failures tested and missing metrics named.**

Both tasks are implemented. Phase-wide TUNE requirements remain pending for the
remaining quality, performance, release and final acceptance plans. No later plan,
ship, merge, settings mutation or message to another person ran.

## TDD and commits

- RED `9088bec`: executed `policy.digest.controls` failed, CTest exit 8, on
  `successful preview must persist structured evidence`. The reporter interface
  executed successfully but produced no evidence; this was an assertion failure,
  not a missing program or fixture failure.
- GREEN `18bcc66`: implemented checked public reads, all-page run IDs, every
  attempt's job IDs, UTC boundaries, deduplication, safe Markdown and evidence
  files. The same controls and real default preview exited 0.
- RED `57aa6b9`: trusted publication failed its executed create assertion with
  `publisher not implemented`, CTest exit 8.
- GREEN `7d524ed`: implemented publisher predicates, duplicate rejection,
  file-based issue bodies, retained monthly sections, workflow serialization,
  least permissions, recovery docs and controls. CTest and full CI exit 0.

No separate refactor commit was needed. This is an execute plan with task-level
TDD; phase-global TDD mode remains false. The four implementation commits and
their before/after ledger were measured before writing this summary.

## Hosted and local evidence

- `cmake --workflow --preset ci` — exit 0, **608** regular tests and six
  independent release/archive checks. Final scoped `ctest --preset ci -R
  '^policy[.]digest[.]controls$' --output-on-failure` — exit 0, 1.41 s.
- Controls exercise duplicate paginated IDs, both attempts, required API failure,
  truncated pages, UTC default boundaries, hostile literal titles, empty history,
  optional announcement failure, cancelled and intentional-control cost, forbidden
  events/refs/inputs, duplicate issue state, create/edit failures, unchanged
  last-success timestamp, partial-month preservation and newer-month replacement.
- `sh scripts/digest.sh preview build/digest-preview` — exit 0. Default window
  is **2026-09-01 to 2026-10-01 UTC**. The repository did not exist during that
  month: run counts are zero and timing samples explicitly unavailable.
- The real initial partial preview for **2026-10-21 00:00–22:15 UTC** — exit 0,
  71 matching runs, 70 attempts with in-window jobs and 883 jobs. It retained
  cancelled work and 918.9667 observed runner minutes; merged PR sample n=5.
- Final immutable script copy: `sh /tmp/muhtari-digest-preview-final.sh preview
  build/digest-current 2026-10-21T20:00:00Z 2026-10-21T22:15:00Z` — exit 0.
  Seven runs, seven attempts, 73 jobs and **91.5833 runner minutes**; direct CI
  20 jobs, release-called CI 21, planning/light 8 and other 24. No merged PR in
  this narrower window, so minutes/merged-PR is unavailable.
- `gh run view 39685745578 --json status,conclusion,url,jobs` — exit 0:
  [hosted PR preview](https://github.com/szTheory/muhtari/actions/runs/39685745578)
  succeeds at implementation `7d524edc77c977487a734243ac01364dcbd50272`.
  Preview job 119519220500 succeeds; publish job 119519305492 is skipped.
  `gh run download 39685745578 -n maintenance-digest -D build/digest-hosted`
  — exit 0; the agent read the downloaded report and structured evidence.
- The live triage includes age, reason, link and next action for official
  [runner-image announcements](https://github.com/actions/runner-images/issues/14745).
  Other empty triage categories are not represented as fabricated issues.
- Counter, `cmake -DDIR=.github/workflows -P tests/cmake/timeouts.cmake`,
  `scripts/hygiene.sh --tree` and `git diff --check` — exit 0. `actionlint
  -ignore 'label "ubuntu-26.04" is unknown' .github/workflows/digest.yml` — exit 0;
  the installed tool's runner list predates the already-used hosted image. No
  source or recurring policy was relaxed to accommodate it.

Reports, downloaded evidence and logs remain uncommitted under ignored build
output or temporary storage. No ROM, credentials, private-repository search or
private body/title result was collected. No tracked file was deleted.

## Budget

The fixed B12885/S2/N1970/C14853 contract is unchanged. Allocation remains
count400/BE300/digest650/quality450/fixes100/reserve70. Actual use is count368,
BE193, digest447, fixes1 and zero in quality/reserve: **13892 physical lines**,
961 below C. Digest447 comprises script184 + controls165 + workflow90 +
registration4 + exact inventory1 + charged-file inventory3. Every implementation,
control and workflow file is charged whole. No relocation, line golf, exclusions,
new dependency, reserve transfer or baseline change financed this addition.
Source denominators are 8580/6533; their ratios remain advisory.

## Remaining evidence and ship obligation

The weekly cron is Monday 06:00 UTC; native scheduling is best effort and manual
dispatch is the recovery. Issue publication is intentionally restricted to the
default branch. Actual create/edit under that token and the first scheduled
publication are confirmed after merge at ship, per AGENTS rule 6. They are not
owner UAT, an authorization request, or a claim that a live issue was already
published. The PR preview and deterministic publisher faults are verified now.

Compile-only share, matching JUnit/CTest utilization, runner-image/tool changes,
runner queue versus dependency wait, and workflow DAG critical path have no
matching producer in this collector; reports explicitly show them unavailable.
Observed job percentiles are grouped by job and runner labels with sample counts,
not claimed comparable across unknown tool/workload changes. Rerun passes remain
candidates, explicit failed control steps are excluded from candidacy and retain
cost, and unrecognized controls remain unclassified. No 90-day history is claimed.

The TUNE-01 unclassified edge assumption and descriptorless transparency
prohibition remain flagged for the phase verifier; fixtures do not resolve them.
No implementation stub, skipped test, new auth surface or product API was added.

## Deviations from Plan

No scope deviation. The initial live partial shell was interrupted while its
source file was being edited; rerunning from an immutable temporary copy yielded
the successful evidence above. No retry was added to the reporter or CI. Git
metadata writes used the sandbox's authorized escalation and ordinary hygiene
hooks on the existing phase branch.

## Self-Check: PASSED

All seven implementation/control/document paths exist. All four recorded task
commits are ancestors of HEAD; the persisted ledger measures four commits. The
hosted preview artifact exists and the publisher is skipped on the PR. All
task acceptance commands run before metadata closeout passed as recorded above.
