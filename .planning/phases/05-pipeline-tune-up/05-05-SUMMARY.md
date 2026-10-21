---
phase: 05-pipeline-tune-up
plan: "05"
status: halted
subsystem: infra
tags: [cmake, quality, trusted-baselines]
requires:
  - phase: 05-01
    provides: Audited physical inventory and fixed ceiling
  - phase: 05-04
    provides: Reporting implementation and current allocation
provides:
  - Typed policy and measured-value judge with deterministic fault controls
  - Partial hosted current and independent-base observation producers
affects: [05-05, 05-06, 05-07, 05-08, 05-09]
tech-stack:
  added: []
  patterns: [typed CMake JSON, unchanged event-base builds, separate observation artifacts]
key-files:
  created:
    - tests/cmake/quality.cmake
    - tests/cmake/quality_evidence.cmake
    - tests/policy/quality.cmake
  modified:
    - .github/workflows/ci.yml
    - tests/policy/tests.cmake
    - tests/inventory/ci.txt
    - tests/policy/gate-lines.txt
key-decisions:
  - Stop required integration when Windows producer evidence is absent; retain existing controls and do not seed incomplete bounds.
  - Redistribute only within fixed N1970 to count368, BE205, digest447, quality850, reproduced fixes100 and reserve0; C14853 is unchanged.
requirements-completed: []
requirements-contributed: [TUNE-03, TUNE-04]
coverage:
  - id: typed-quality-judge
    description: Actual floor and ceiling checks, trusted policy identities, independent introductions and bounded inherited reasons
    requirement: TUNE-03
    verification:
      - kind: unit
        ref: policy.quality.controls; full cmake --workflow --preset ci, exit 0
        status: pass
    human_judgment: false
  - id: partial-independent-producers
    description: Linux native and hygiene artifacts identify current and independently built trusted-base observations
    requirement: TUNE-03
    verification:
      - kind: integration
        ref: https://github.com/szTheory/muhtari/actions/runs/39694430448
        status: pass
      - kind: other
        ref: gh run download 39694430448 -n quality-linux-x64 and -n quality-hygiene; exit 0
        status: pass
    human_judgment: false
  - id: all-lane-required-integration
    description: Windows evidence, complete independently seeded bounds and aggregate integration remain blocked
    requirement: TUNE-03
    verification:
      - kind: integration
        ref: https://github.com/szTheory/muhtari/actions/runs/39694430448/job/119548291959
        status: fail
      - kind: integration
        ref: https://github.com/szTheory/muhtari/actions/runs/39694430448/job/119548291973
        status: fail
    human_judgment: false
actuals:
  tokens: 8077
  tasks: 1
  commits: 5
plan_head_before: 3c68002e0f85d063ec62e71408f5fa48966b8c0c
plan_head_after: cf755c968b8a9e7f8c982be62fe3fcd67037a0ed
duration: 14min
completed: 2026-10-21
---

# Phase 5 Plan 5: Trusted quality ratchet Summary

**The typed quality judge passes its controls, but required Windows producer evidence
is missing because the existing digest control fails before observations run.**

Only Task 1 is complete. Task 2 has a tested adapter and partial independent hosted
observations; its complete baseline and bound-seeding gate is unmet. Task 3 did not
start. This is a durable halt, not plan or phase acceptance. No metric was invented,
no missing native leg was made advisory, and no existing control was removed.

## Tasks and commits

| Task | Status | Commits |
|---|---|---|
| 1: Typed policy and actual measurements | Complete | RED edd49c2; GREEN 3a02e6c |
| 2: Real producers and independent base | Partial, blocked | RED 8ac740f; adapter e6d725f; initialization controls cf755c9 |
| 3: Required aggregate | Not started | None |

Both RED runs executed `policy.quality.controls` and failed its intended assertion,
CTest exit 8. The first observed an accepted value11 against ceiling10; the second
observed success with missing JUnit input. `check tdd-red-evidence` returned
`RED_EVIDENCE_OK`, exit 0, for both ignored build records. Semantic inspection found
executed assertions rather than syntax, fixture or discovery errors. GREEN replaced
both temporary command interfaces; no placeholder interface remains.

The judge checks numeric types, actual floor/ceiling equality and regression,
metric removal, identity/availability changes, independent new-metric observations,
commit/configuration mismatches, valid and inherited reasons, calendar dates,
grant-to-expiry lifetime, future grants, renewal and waiver-to-baseline conversion.
The structural gate initialization requires the caller's reviewed exact ceiling;
it cannot initialize another metric or reset an existing gate metric. Root
`quality.json` retains its original gate_lines policy during this halt.

## Evidence and blocker

`cmake --workflow --preset ci` — exit 0, **609 regular and six release/archive tests**.
`cmake -DROOT=. -DBUILD=build/ci -DOUT=build/ci/quality.json -DSHA="$(git rev-parse HEAD)"
-DCONFIG=ci -P tests/cmake/quality_evidence.cmake` — exit 0. The adapter reuses
`check_junit.cmake`, requires completed ABI/fuzz/title/archive judges and fatal-warning
configuration, reads actual archive bytes, and invokes the physical counter. These
are local observations, with source `local`, not falsely attributed hosted evidence.

`gh run view 39694430448 --json status,conclusion,jobs` and job-log API reads — exit 0.
At inspection the run still had other jobs executing, but both Windows native jobs
had completed with failure. Each `Build, test, package` step runs
`cmake --workflow --preset ci-msvc`, exit **8**. The failed test is the pre-existing
`policy.digest.controls`, at `tests/policy/digest.cmake:52`: `digest valid preview
failed: no such file or directory`. The new quality controls pass on both Windows
architectures. Their subsequent observation and independent-base steps do not run.
The digest file is unchanged by this plan; its tooling failure is recorded as a
seed, outside this executor's implementation scope. It is not weakened or converted
into a new product requirement.

The same run successfully completed Linux x64/arm64 native producer jobs and hygiene.
`gh run download 39694430448 -n quality-linux-x64 -D build/quality-hosted/quality-linux-x64`
and the corresponding hygiene download — exit 0. The agent read their current/base
JSON and source/configuration identities. Trusted event base is
`65b360d1afd37b7dad9f94dcff497c85df84fd1d`; hosted candidate is
`e6d725faef06b8753945795244ccf78eb1cb2f20`. Linux base reports 605 regular tests,
12 ABI tests, four fuzz tests, three title tests, zero ABI/warning findings and
261853 archive bytes; hygiene base reports24 tests and zero analyzer findings.
Compiler/system and fixture fingerprints agree within each current/base pair.
These partial measurements are not the six-platform policy baseline.

The adapter derives the baseline SHA from event metadata, verifies the Git commit,
builds that unchanged tree with the same preset and adapter on the same runner,
and keeps base outputs separate. Pre-policy absence is verified with `git ls-tree`;
an empty policy is not a candidate-controlled override. Artifact uploads name
exactly current/base JSON, trusted policy and base SHA, excluding the base worktree.

Release startup failure in this run is also expected unfinished integration:
Task 2 added CI actions:read, while the explicitly planned release permission-union
update belongs to Task 3 and has not run. No release-called success is claimed.

## Budget and checks

Final physical count **14499/C14853**, source8580/6533, both ratios advisory.
Quality uses **607/850** incremental whole-file lines, including controls, policy
registration, declarations and all workflow glue. Allocation is count368/BE205/
digest447/quality850/fixes100/reserve0, totalN1970. The visible transfer before
affected additions is count32 + digest203 + reserve70 + BE95 into quality400.
BE actual193 fits205; digest447 and count368 retire unused allocations. No source
padding, line golf, exclusion, new dependency, cap increase or lost control funds it.

Counter, `ci_required.cmake` (15 existing cases), timeout comments (29 jobs),
`scripts/hygiene.sh --tree` and `git diff --check` — exit 0. These roll-up cases
verify the unchanged roll-up, not the absent aggregate. `actionlint -shellcheck=''
-ignore 'label "ubuntu-26.04.*" is unknown' .github/workflows/ci.yml` — exit 0.
Full actionlint additionally reports inherited shellcheck SC2017/SC2059 in the
unchanged RetroArch pixel-control script; those are recorded as tooling seeds.
No source rule or recurring control was relaxed. Evidence/logs stay uncommitted.

## Deviations and unfinished work

The required-evidence gate stopped expansion. Missing Windows current/base evidence
prevents independent all-lane seed selection. Additional metric bounds, comparable
Ir artifact adaptation, aggregate artifact joins, aggregate omission/failure controls,
release permission unions and documentation are unfinished. Existing library/WASM/
property jobs remain advisory. Coverage, open crashes, backlog, historical duration
and comparable Ir observations are explicit gaps, not numeric successes. The
TUNE-03 boundary prohibition remains flagged and unresolved.

No implementation placeholder, skipped test or product stub remains. Planned but
unimplemented integration prevents this plan from being marked complete. The unrun
hosted aggregate/all-lane verification is recorded in the cross-phase defect ledger.

## Threat Flags

| Flag | File | Description |
|---|---|---|
| threat_flag: trusted-base-filesystem | tests/cmake/quality_evidence.cmake | Reads event-selected Git trees and builds them in an isolated base directory; verifies SHA, source-root, compiler and fixture identity before comparison. |

## Next action

Resolve or explicitly route the reproduced Windows tooling blocker, then resume
Task 2 of this plan. First route the blocker with
`$gsd-debug Windows policy.digest.controls fails in run 39694430448; unblock 05-05 Task2`;
after repair, resume with `$gsd-execute-phase 5`. No downstream plan,
phase-wide requirement completion, ship, merge, settings mutation or owner UAT ran.

## Self-Check: PASSED

All three created implementation/control files exist. All five task commits are
ancestors of HEAD, and `check evaluation-scope --plan 05-05 --commits-only --raw`
resolves those five commits, exit 0. The persisted ledger measures5 commits from
the recorded before/after heads. There are no tracked deletions. This check confirms
the preserved partial work and halt evidence; it does not certify unfinished tasks.
