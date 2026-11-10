---
phase: 05-pipeline-tune-up
plan: "08"
status: halted
halted: true
tasks_completed: 0
subsystem: infra
tags: [release, signer, tdd, allocation-stop]
requires:
  - phase: 05-13
    provides: Accepted retained-store reconciliation at total14840 and quality919
  - phase: 05-07
    provides: Accepted test-card arming and retained advisory lane disposition
provides:
  - Reproducible wrong-signer publication RED with correct-signer and tamper controls
  - Complete physical cost of the first required release-safety control
affects: [05-09]
tech-stack:
  added: []
  patterns: [real-script synthetic transport, exact publication boundary assertion]
key-files:
  created:
    - tests/policy/publish.cmake
    - .planning/phases/05-pipeline-tune-up/05-08-SUMMARY.md
  modified:
    - tests/policy/tests.cmake
    - tests/inventory/ci.txt
    - tests/policy/gate-lines.txt
    - .planning/WINDOWS.md
key-decisions:
  - Stop before GREEN because the first release-safety control adds66 counted lines with fixes30 already fully used and only1 line below strict14841.
  - Preserve production publisher and workflow behavior, fixed allocations, accepted predecessor evidence and historical halts; no additional gap round or savings search is authorized.
requirements-completed: []
requirements-contributed: []
coverage:
  - id: accepted-preconditions
    description: Accepted reconciliation and armed/advisory disposition are established before the release probe
    requirement: TUNE-03
    verification:
      - kind: other
        ref: Accepted05-13/05-07 summaries, current gate_count14840, and read-only predecessor run identity queries; exit0
        status: pass
    human_judgment: false
  - id: release-signer-red
    description: The real publisher accepts a wrong workflow signer and permits the synthetic publication operation
    requirement: TUNE-05
    verification:
      - kind: unit
        ref: ctest --preset ci -R '^policy[.]publish[.]controls$' --no-tests=error --output-on-failure --output-junit ../plan-evidence/release-red.xml; exit8
        status: fail
      - kind: other
        ref: node gsd-tools.cjs check tdd-red-evidence build/plan-evidence/release-red.json --raw; exit0; RED_EVIDENCE_OK
        status: pass
    human_judgment: false
  - id: release-allocation-stop
    description: The fully counted first control exceeds the fixed safety component and both total limits
    requirement: TUNE-03
    verification:
      - kind: other
        ref: gate_count observes14906; strict14841/C14853 jq fit check exits1; fixes96 exceeds30 by66
        status: fail
    human_judgment: false
  - id: remaining-release-acceptance
    description: Signer repair, lookup-error controls, full CI, rehearsal measurement and documentation reconciliation are unfinished
    requirement: TUNE-05
    verification:
      - kind: integration
        ref: Tasks1-3 remaining verification was not run after the binding Task1 allocation stop
        status: unrun
    human_judgment: false
actuals:
  tokens: 1096
  tasks: 0
  commits: 1
plan_head_before: 6769fcb39bcc1e0c6661bf6560b38b06c09ab44b
plan_head_after: e0c18a07bf2dac0b068442c988e804ea5499c86e
duration: 8min halted
started: 2026-10-22T20:00:32Z
completed: 2026-10-22
---

# Phase 5 Plan 8: Release-safety RED and fixed-allocation halt Summary

**The first counted release-safety control reproduces publication after a wrong signer, but its66-line cost stops Task1 at14906/fixes96 before production repair.**

This plan is halted, with **0/3 tasks complete**. RED commit `e0c18a0` is durable;
there is no GREEN and no release-safety acceptance. Active accepted completion stays7/9.
Plan05-09 remains blocked. Historical05-06/05-11/05-12 halts and all predecessor
summaries remain unchanged. Shared TUNE requirements remain pending.

## Preconditions

The accepted05-13 summary records full final local609+6, generated-corpus library43,
current direct numeric perf/executed library/quality, authentic9 evidence and
release-called required CI/authentic8 evidence at quality919 and total14840.
Its overall Release failure from the advisory WASM fetch is preserved; a successful
overall Release or rehearsal was never the accepted reconciliation claim.
Read-only `gh run view` queries confirm predecessor head31c52d9 and attempt2 identities:
[direct CI39832530167](https://github.com/szTheory/muhtari/actions/runs/39832530167),
[release-called39832530253](https://github.com/szTheory/muhtari/actions/runs/39832530253).

The accepted05-07 summary records both fresh same-commit control dispatches,
numeric PR base, actual nightly failure/recovery and armed test-card-only disposition.
[CI39836658273](https://github.com/szTheory/muhtari/actions/runs/39836658273) and
[CI39836675687](https://github.com/szTheory/muhtari/actions/runs/39836675687)
remain completed-success runs; WASM9/library5/prop2 remain advisory.
Those are prerequisite records, not fresh acceptance of this plan's changed tests.

Before edits, the real whole-file counter returned14840, exit0. Fixed allocations
remain count368/BE205/digest447/quality920/fixes30/reserve0. Accepted actuals were
count368/BE193/digest447/quality919/fixes30/reserve0. BE12 is not reserve.

## Partial Task1 and exact stop

The new ci-only dispatcher invokes the real POSIX `scripts/publish.sh` against18
generated synthetic asset files and a generated `gh` transport. No hosted write,
real release, firmware or ROM bytes are involved. Its short-circuit boundary models
the workflow's attested step followed only on success by publication. The fake
transport records the attempted PATCH without contacting GitHub.

The correct signer/ref case succeeds through the boundary. A transport that accepts
the changed copy produces the existing exact tamper error and prevents publication.
The wrong-signer case then returns0 from the publisher and records PATCH: the real
prepublication verifier omits the signer-workflow restriction already present in
download verification. The named assertion fails for that intended behavior.

The complete first-control cost is:

| Charged path | Added physical lines |
|---|---:|
| tests/policy/publish.cmake |60|
| tests/policy/tests.cmake |4|
| tests/inventory/ci.txt |1|
| tests/policy/gate-lines.txt |1|
| Total addition |66|

All147 inventory entries are whole files. There are no exclusions or moved predicates.
Actual **14906** exceeds strict14841 by65 and C14853 by53. Safety actual becomes
**fixes96/30**, exceeding its unchanged allocation by66; quality stays919 and all
other component use is unchanged. Advisory ratios are173.7296% of8580 full source
lines and228.1647% of6533 authored lines. They grant no budget.

The fit assertion returnedfalse/exit1. D-20/D-26 and Task1's fixed-allocation
acceptance stop affected expansion. The test is intentionally RED; its commitment
preserves the reproduction, not an accepted overrun. No signer implementation,
API/404 handling or additional controls were added after the measured stop.
This is a measured construction cost, not proof that every implementation is impossible.
There is no new allocation, automatic rebaseline, BE transfer, line golf, control
removal, generalized savings hunt or additional automatic gap round.

## Verification evidence

Commands below are local and have no hosted run URL unless explicitly linked above.
Generated JUnit, traces, synthetic files, JSON and console evidence remain ignored
build output; no log is committed.

- `cmake --preset ci` — exit0; exact inventory registration accepts the new target.
- `ctest --preset ci -R '^policy[.]publish[.]controls$' --no-tests=error --output-on-failure --output-junit ../plan-evidence/release-red.xml`
  — exit8; one executed target fails at its wrong-signer assertion after both baseline controls pass.
- `node gsd-tools.cjs check tdd-red-evidence build/plan-evidence/release-red.json --raw`
  — exit0, `RED_EVIDENCE_OK`; target JUnit is current and unmodified.
- `cmake -DROOT=. -DINVENTORY=tests/policy/gate-lines.txt -DOUT=build/plan-evidence/release-red-count.json -P tests/cmake/gate_count.cmake`
  — exit0;14906 counted lines,147 whole-file entries.
- `jq -e '.gate_lines <= 14841 and .gate_lines <= 14853' build/plan-evidence/release-red-count.json`
  — exit1, false; actual total does not fit.
- `git diff --check -- tests/policy/tests.cmake tests/policy/gate-lines.txt tests/inventory/ci.txt`
  and `sh scripts/hygiene.sh --tree` — exit0.
- Read-only predecessor `gh run view` identity/status queries — exit0; no new hosted run was dispatched.

## TDD Gate Compliance

RED is `e0c18a0`. The unchanged JUnit generated by the real current command passes
the format classifier. Semantic assessment confirms that discovery/setup succeeded,
the real script ran, correct-signer and tamper checks passed, and the intended
wrong-signer/no-PATCH assertion failed with actual exit0 plus recorded PATCH.
GREEN is not authorized by the separate budget contract despite valid RED evidence.
No implementation or refactor commit exists; no TDD completion is claimed.

## Deferred Issues and unrun verification

Task1 remains incomplete: prepublication signer restriction, explicit404 versus
API/auth/rate/network/malformed lookup behavior, surviving-object checks and full
local/hosted acceptance are unimplemented or unrun. Only the first signer control
was built; it does not claim complete source-ref, asset or lookup-error coverage.

Task2 rehearsal cost/input-identity measurement and its verification were not run.
The current conservative workflow selection remains unchanged; no measured reuse
decision or fresh rehearsal is claimed. Task3 release/bot/preparation documentation
reconciliation and its budget/broad-diff verification were not run. The inherited
broad-whitespace seed remains historical and is not the cause of this stop.

WINDOWS24 records the open signer/allocation truth; WINDOWS25 records remaining
unrun acceptance. All seven flagged assumptions, including the unclassified
TUNE-05 edge-probe category and descriptorless physical-boundary prohibition,
remain unresolved. No requirement was completed, owner UAT proposed, admin change,
merge, ship, product publication or next GSD step performed.

## Deviations from Plan

Execution stopped at the required allocation gate during Task1 RED. No automatic
fix beyond scope or structural change was attempted. The partial test is durable
evidence for an explicitly chosen next scope/construction decision, not permission
to resume failed Task1 or to fund additional work from another component.

## Known Stubs and threat surface

No production stub, skipped test or TODO was introduced. The intentionally failing
test and missing remaining verification are explicit above and in WINDOWS.
All synthetic assets/transport operations remain within the generated test directory.
No new production endpoint, auth path, file-access boundary or schema was introduced.

## Self-Check: PASSED

The created fixture and canonical partial SUMMARY exist. RED e0c18a0 is an ancestor
of HEAD, contains exactly the four declared test/inventory paths and deletes no file.
The persisted plan ledger measures1 task commit from6769fcb through e0c18a0.
The summary reports a halt and0/3 tasks, never GREEN, requirement or phase completion.
