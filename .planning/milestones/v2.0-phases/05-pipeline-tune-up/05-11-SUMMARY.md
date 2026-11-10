---
phase: 05-pipeline-tune-up
plan: "11"
status: halted
subsystem: infra
tags: [cmake, quality, allocation, performance]
requires:
  - phase: 05-05
    provides: Accepted typed observations, authenticated joins and fixed quality allocation
provides:
  - Measured shared-reader construction with original selected faults retained
  - Explicit Task1 allocation halt and preserved uncommitted draft
affects: [05-07, 05-08, 05-09]
tech-stack:
  added: []
  patterns: [shared typed readers, original-item validation before extrema]
key-files:
  created:
    - .planning/phases/05-pipeline-tune-up/05-11-HANDOFF.md
  modified:
    - quality.json
    - tests/cmake/quality.cmake
    - tests/cmake/quality_join.cmake
    - tests/cmake/perf_compare.cmake
    - tests/perf/tests.cmake
    - tests/policy/quality.cmake
key-decisions:
  - Halt expansion at quality958 versus920; no GREEN commit or downstream acceptance.
requirements-completed: []
requirements-contributed: [TUNE-03, TUNE-04]
coverage:
  - id: selected-controls
    description: Seven exact perf cases and original aggregate and join controls retain their faults
    requirement: TUNE-03
    verification:
      - kind: unit
        ref: cmake --preset ci && ctest --preset ci -R '^(perf[.]gate[.]|policy[.]quality[.]controls$)' --no-tests=error --output-on-failure; exit0;8/8
        status: pass
    human_judgment: false
  - id: allocation
    description: Construction fits strict total14841 and quality920 with unchanged other components
    requirement: TUNE-03
    verification:
      - kind: other
        ref: gate_count observes14879; jq explicit14841/14853 gate exits1; quality958 misses38
        status: fail
    human_judgment: false
  - id: current-callers
    description: Central compatibility and production callers have current local and hosted acceptance
    requirement: TUNE-04
    verification:
      - kind: integration
        ref: Task2/3 unrun after Task1 allocation failure; no current run URL
        status: unknown
    human_judgment: false
actuals:
  tokens: 4101
  tasks: 0
  commits: 0
plan_head_before: 91c31654a5c655b3f1dd0aa99f53bcbfc8c1e462
plan_head_after: 91c31654a5c655b3f1dd0aa99f53bcbfc8c1e462
duration: approx9min halted
completed: 2026-10-22
---

# Phase 5 Plan 11: Shared-reader allocation recovery Summary

**The named reader construction preserves eight selected controls but recovers9 of47
required lines, halting at quality958/920 and total14879.**

Zero of three tasks are complete. Task1's explicit allocation gate failed; Task2 and
Task3 did not begin. No task implementation commit, GREEN, push, dispatch or hosted
acceptance exists. The six implementation files remain an uncommitted draft.

## Lineage and TDD

Starting HEAD91c31654a5c655b3f1dd0aa99f53bcbfc8c1e462 remains on the existing phase branch.
Accepted reference5535b57168ef31a8993c28646920a8b88ed0e98a remains the14841 baseline.
Inherited RED df6b0959dfbb31f843ada7fe3cbfc7e1b1f24c25 is an ancestor. Its intentional
central-reason assertion failure and RED_EVIDENCE_OK remain recorded in05-06-SUMMARY;
they were not recreated or relabelled as a new05-11 RED. The retained four-file draft
was preserved. Before editing, its eight selected tests passed and the counter
reproduced14888. No syntax or allocation failure authorized GREEN.

The attempted refactor extracted quality_bound and quality_observation into the
existing quality.cmake. Independent ordinary introductions, narrow structural gate
initialization, domain floor defaults, inherited unwaived values and current/prior
reason validation remain. Join reads each original item before extrema; exact
membership, trusted SHA/policy, environments and identity checks remain. Aggregate
still owns actual-bound judgement and required/advisory/status/unit/source reporting.
It does not manufacture ordinary Ir evidence. The seven original perf cases retain
exact+1.00/+1.01, armed/report-only, two-base, positive/missing and NO_BASE assertions.

One intermediate rerun failed because CMake wrapped the expanded SHA diagnostic.
Whitespace-tolerant matching of the same exact observed/expected SHA values corrected
that test expectation. Final8/8 pass, including missing-original-item and smaller-lane
forgery. No generic failure assertion replaced the forged-SHA fault. No control,
comment, inventory row, legacy store or test name was deleted to buy savings.

## Measured costs

All146 inventory paths remain whole-file counted. Every changed counted path belongs
to quality. Other counted paths are byte-unchanged by this attempt.

| Path | Accepted | Retained draft | Final draft | Change from draft | Change from accepted |
|---|---:|---:|---:|---:|---:|
| quality.json |101|109|109|0|8|
| tests/cmake/quality.cmake |230|280|284|4|54|
| tests/cmake/quality_join.cmake |114|114|101|-13|-13|
| tests/cmake/perf_compare.cmake |95|68|68|0|-27|
| tests/perf/tests.cmake |67|83|83|0|16|
| tests/policy/quality.cmake |201|201|201|0|0|
| Complete inventory |14841|14888|14879|-9|38|

The control changes one exact diagnostic assertion with zero line delta. All helper,
replacement and call costs are included: quality reader+4 and join-13 recover9 net.
The required47-line recovery misses by38. Forecast reader savings were not realized;
unimplemented fixture/parser/store retirement is not credited. Only the named reader
construction was attempted; there was no unrelated savings hunt.

| Component | Fixed allocation | Accepted actual | Final draft actual |
|---|---:|---:|---:|
| Count/inventory |368|368|368|
| BE relocation/equality |205|193|193|
| Digest |447|447|447|
| Quality including controls/glue |920|920|958|
| Reproduced fixes |30|30|30|
| Reserve |0|0|0|

B12885-S2+N1970=C14853 is frozen. Quality14879-13921=958 exceeds920 by38.
Total14879 exceeds strict14841 by38 and C14853 by26. BE12 is not reserve.
Source8580/authored6533 are unchanged; ratios remain advisory. No cap, allocation,
classification, counting boundary, dependency or waiver changed to conceal the miss.

## Verification

All checks are local; current recovery run URL is unavailable.

- `cmake --preset ci && ctest --preset ci -R '^(perf[.]gate[.]|policy[.]quality[.]controls$)' --no-tests=error --output-on-failure` — exit0 before edits and after final implementation edit;8/8, seven perf cases plus quality.controls.
- `cmake -DROOT=. -DINVENTORY=tests/policy/gate-lines.txt -DOUT=build/plan-evidence/recovery-count.json -P tests/cmake/gate_count.cmake` — exit0; before14888, final14879, source8580/6533.
- `jq -e '.gate_lines <= 14841 and .gate_lines <= 14853' build/plan-evidence/recovery-count.json` — exit1, false.
- `python3` physical-path comparison using saved draft counts and `git show5535b57:path` — exit0; accepted14841/draft14888/final14879, only scoped quality differences.
- `git diff --check` and `scripts/hygiene.sh --tree` — exit0 after final implementation edit.
- `git merge-base --is-ancestor df6b095 HEAD` — exit0; historical RED preserved.

Task2's compatibility/generated-corpus library verification and Task3's full ci/library,
budget/caller/store retirement, direct numeric perf/library/quality, release-called CI
and authentic artifact joins are unrun because Task1 forbids expansion after this miss.
No current producer/base SHA or hosted URL exists. Prior05-05 URLs are predecessor
evidence only. Both allow stores and production callers remain unchanged. WINDOWS18
records the new deficit and WINDOWS19 unrun acceptance; historical16/17 remain open.

## Commits and actuals

No task commit exists. The persisted05-11 ledger measures0 from its before head to
the recorded plan_head_after: an empty implementation interval at a halt, not committed
code or completion. The following metadata-only halt commit is separate. All six code
changes remain uncommitted. Tokens4101 are ceil(16404 realized implementation-diff
characters/4) against starting HEAD, including retained draft lineage, not harness tokens.

## Decisions and remaining scope

The plan's explicit Task1 stop was followed. No new allocation, unrelated simplification,
additional gap round, later GSD step or owner UAT was authorized. Possible fixture/store
retirement cannot retroactively finance the gate. Concrete alternatives require a scoped
decision: explicitly defer centralization while retaining the existing semantic stores,
or choose another demonstrably smaller fault-preserving construction within the same
allocation. Neither is accepted here.

No introduced goal-blocking stub or skipped test was found. Unfinished compatibility
and caller work is explicitly unimplemented/unrun and blocks completion. Private CMake
input validation adds no endpoint or product schema; trusted domain wiring remains
unproved. All seven unresolved assumption rows and the descriptorless TUNE-03 physical
boundary prohibition retain their previous scope/lifecycle. TUNE-03/TUNE-04 stay pending.

Stop this execution step.05-07/08/09 remain blocked; the phase stays5/9 complete.
Next command in a fresh session: `$gsd-discuss-phase 5` for the concrete bounded scope
decision. Read05-11-HANDOFF.md first; no additional gap round is automatically authorized.

## Self-Check: PASSED

The six draft files, this summary and handoff exist. RED df6b095 is an ancestor; the
persisted ledger's0 implementation commits match the recorded heads.05-06 records and
accepted predecessor histories remain unchanged. Final8-case verification, hygiene and
diff check pass while the numeric gate fails. This self-check verifies the halt only.
