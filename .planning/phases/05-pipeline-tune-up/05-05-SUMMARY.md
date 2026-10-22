---
phase: 05-pipeline-tune-up
plan: "05"
status: complete
subsystem: infra
tags: [cmake, quality, trusted-baselines, ci]
requires:
  - phase: 05-01
    provides: Audited physical inventory and fixed ceiling
  - phase: 05-04
    provides: Reporting implementation and scoped allocation
provides:
  - Typed policy and actual-value ratchet with bounded reasons and independent introductions
  - Attributable native, hygiene, ASan and advisory test-card Ir observations
  - Required exact-artifact aggregate in direct and release-called CI
affects: [05-06, 05-07, 05-08, 05-09]
tech-stack:
  added: []
  patterns: [typed CMake JSON, unchanged event-base builds, authenticated artifact extrema]
key-files:
  created:
    - tests/cmake/quality.cmake
    - tests/cmake/quality_evidence.cmake
    - tests/cmake/quality_join.cmake
    - tests/policy/quality.cmake
    - docs/quality.md
  modified:
    - quality.json
    - .github/workflows/ci.yml
    - .github/workflows/release.yml
    - tests/cmake/ci_required.cmake
    - tests/hygiene/tests.cmake
    - tests/policy/tests.cmake
    - tests/inventory/ci.txt
    - tests/policy/gate-lines.txt
    - docs/testing.md
key-decisions:
  - Transfer only unused fixes70 to quality before Task3 additions; count368, BE205, digest447, quality920, fixes30 and reserve0 retain N1970 and C14853.
  - Seed ordinary bounds from authenticated independent base observations; tighten archive bytes to the observed comparable-base minimum399937.
  - Keep unavailable dimensions explicit and library, WASM, property and unarmed test-card Ir advisory.
requirements-completed: [TUNE-03, TUNE-04]
coverage:
  - id: typed-quality-judge
    description: Actual bounds, trusted policy identities, independent introductions and bounded inherited reasons fail closed
    requirement: TUNE-03
    verification:
      - kind: unit
        ref: cmake --workflow --preset ci; exit 0; policy.quality.controls within 609 regular and six archive tests
        status: pass
    human_judgment: false
  - id: independent-producers
    description: Complete current and unchanged event-base observations carry matching configuration and fixture identities
    requirement: TUNE-03
    verification:
      - kind: integration
        ref: https://github.com/szTheory/muhtari/actions/runs/39706507168
        status: pass
      - kind: other
        ref: gh run download 39706507168 and 39706506980 with quality-*; authenticated artifact joins; exit 0
        status: pass
    human_judgment: false
  - id: required-aggregate
    description: Exact artifact membership and measured bounds are required by direct and release-called CI roll-ups
    requirement: TUNE-03
    verification:
      - kind: unit
        ref: ci_required.cmake; 18 cases; exit 0
        status: pass
      - kind: integration
        ref: https://github.com/szTheory/muhtari/actions/runs/39706507168/job/119589728007
        status: pass
      - kind: integration
        ref: https://github.com/szTheory/muhtari/actions/runs/39706506980/job/119592656905
        status: pass
      - kind: integration
        ref: https://github.com/szTheory/muhtari/actions/runs/39706506980/job/119592816991
        status: pass
    human_judgment: false
  - id: honest-reporting
    description: Reports show actual units, effective bounds and reasons; called perf and unqualified dimensions remain explicit gaps or advisory
    requirement: TUNE-04
    verification:
      - kind: unit
        ref: policy.quality.controls validates positive unit reporting and fault-specific omissions, identities and regressions; exit 0
        status: pass
      - kind: integration
        ref: Final direct and called quality job reports and authenticated advisory Ir artifact; exit 0
        status: pass
    human_judgment: false
actuals:
  tokens: 21213
  tasks: 3
  commits: 20
plan_head_before: 3c68002e0f85d063ec62e71408f5fa48966b8c0c
plan_head_after: 51a3f9592bbb69355896ada41901a5818bfa1c9b
duration: 45min continuation
completed: 2026-10-22
---

# Phase 5 Plan 5: Trusted quality ratchet Summary

**One required quality job applies typed bounds to real, independently based observations and rejects incomplete direct or release-called evidence.**

All three tasks are complete. Task1 and the accepted Windows digest repair were preserved.
This continuation finished Task2 and Task3. Shared phase-wide requirements remain pending:
the sibling-plan readiness check returns no ready IDs for TUNE-03/TUNE-04.

## Performance and commits

The measured continuation runs from the first recorded clock at 2026-10-22T01:08:48Z
to closeout after 01:53 UTC, approximately45 minutes. The earlier14-minute partial halt
and separately accepted Windows debug are historical work, not a new completed task.

| Task | Task commits |
|---|---|
| 1: Typed judge and fault controls | RED edd49c2; GREEN 3a02e6c |
| 2: Real producers and independent base | RED 8ac740f; adapter e6d725f; initialization cf755c9; adoption79e9e8c; strict archive adoption51a3f95 |
| 3: Required aggregate and release wiring | ed31959; actual-unit/reason reporting d2426a2 |

The ledger measures20 commits from its persisted before head to51a3f95. This includes
the nine task commits above, historical handoff/docs commits and accepted Windows debug
commits. It is not a narrated count of just this continuation. Actual tokens are
ceil(84852 realized task-diff characters /4), on the estimate's scale, not harness tokens.
Fourteen implementation/documentation files changed; allocation and execution metadata
are recorded separately.

## Behavior and trusted inputs

The judge checks floor/ceiling equality and actual regression, typed numeric values,
metric removal, producer/configuration/unit/availability changes, independent introductions
and exact reasons. Reasons have valid UTC calendar dates, nonfuture grants, at most60 days,
nonexpired inherited expiry and exact key/from/to. Renewal and waiver-to-baseline conversion
reject. Temporary gate growth requires a credible exit. Only the already reviewed exact
structural ceiling can initialize missing pre-policy gate evidence; it cannot reset an
existing gate metric or initialize an ordinary metric.

Native, hygiene and ASan adapters reuse the completed JUnit inventory judge and actual
ABI, fatal-warning, analyzer, sanitizer, fuzz/title and archive outputs. Zero findings
require their real successful judges. Three validated archives supply actual byte totals.
Per-lane compiler/system and fixture fingerprints bind current/base pairs. Event metadata
selects the unchanged base; its Git commit and policy presence are checked independently.
Candidate measurements cannot become their own introduction baseline.

The aggregate requires exactly six native artifacts plus hygiene and ASan, with exactly
the four specified producer files per lane. It verifies event-base SHA/policy, current
SHA, source run, types, identities and pair environments before keeping the worst value
per bound. One existing typed judge owns threshold/reason decisions. A complete direct PR
also requires its actual advisory Ir artifact. Release-called and push CI explicitly report
unexecuted comparable Ir. The required roll-up checks exact dependency membership, including
quality failures, omissions and wrongly skipped called quality. Planning-only exceptions
and the existing title/perf exceptions remain tested. Both release callers grant the
necessary read-only actions permission union.

## Independent baseline and archive calibration

Initial complete evidence is run39699892851, implementation1abc98a and actual merge
011fb5b9d0286a311508d0097fd122663b2d6748, trusted base
65b360d1afd37b7dad9f94dcff497c85df84fd1d. API artifact identities, SHA/source metadata,
identical base policies and all eight environment pairs authenticate; download and checks
exit0. The independent base measures native605, ABI12, fuzz4, title3, hygiene24,
ASan593 and zero observed findings. These establish the ordinary count/finding bounds.

The first archive ceiling399952 came from that independently rebuilt base. Superseded
called run39705309624/job119585345059 correctly rejected it against a fresh unchanged
base399937, while its candidate was399361. Comparable MSVC19.51.36260.0 WindowsAMD64
and fixture fingerprints match. Other fresh independent bases399949/399953 demonstrate
slight compressed-byte variation. The bound was tightened to the actual minimum399937,
without adding slack, changing baseline identities or excusing a candidate regression.
Final direct/called archive peaks399362/399365 are below it; their independent base peaks
399948/399952 remain above it. Raw observations are retained without normalization.

The archive bound covers the largest three-archive total across native platforms, rather
than a separate growth percentage per platform. Existing archive-member, firmware and
release predicates remain required. The base predates the audited gate inventory; only
gate lines use the narrow fixed structural initialization.

## Final acceptance evidence

Implementation is51a3f9592bbb69355896ada41901a5818bfa1c9b on the existing phase branch.

- cmake --workflow --preset ci — exit0,609 regular tests and six release/archive tests.
- cmake --workflow --preset hygiene — exit0,24 tests; the final shell/tree scan also exits0.
- cmake -DCI_YML=.github/workflows/ci.yml -DJQ=jq -DREPORT_ONLY=wasm32,library -P tests/cmake/ci_required.cmake — exit0,18 cases. Configure-time omission mutations for BE and quality also reject.
- cmake -DROOT="$PWD" -P tests/cmake/budgets.cmake — exit0,19 files within unchanged budgets.
- Counter,30 workflow timeout comments, focused actionlint and git diff --check — exit0.
- gh run view 39706507168 — exit0, completed success; direct quality119589728007 and CI required119589874959 pass. [Direct CI](https://github.com/szTheory/muhtari/actions/runs/39706507168).
- gh run view 39706506980 and attempt/job API reads — exit0; called quality119592656905, CI required119592816991 and Windows arm64 retry119591355796 pass on the same implementation. [Release-called CI](https://github.com/szTheory/muhtari/actions/runs/39706506980). Its broader rehearsal is separate from this plan's required-CI acceptance.
- gh run download 39706507168 -p 'quality-*' -D build/quality-tight-final-direct and gh run download 39706506980 -p 'quality-*' -D build/quality-tight-final-called plus artifact metadata API reads — exit0. All nine direct/eight called artifacts authenticate. Direct producer SHA is merge0b7197e1576563394c3dabddc6a21a0675b85007; called producer SHA is the implementation SHA; both trusted bases are65b360d.
- quality_join.cmake on the authentic called set with expected implementation/base/source and PERF_REQUIRED=OFF — exit0, explicitly unavailable comparable perf.
- python3 build/quality-fault-replay.py build/quality-tight-final-direct 0b7197e1576563394c3dabddc6a21a0675b85007 https://github.com/szTheory/muhtari/actions/runs/39706507168 — driver exit0. The intact authentic set returns0; deleting the Windows arm64 artifact returns1 with exact-set rejection; changing a real warning observation to1 returns1 with measured-regression rejection.

Direct test-card Ir is3837262 and both independently measured base repeats are3837262,
with compiler/Valgrind identity, equal-path/warm-up configuration and source metadata.
It remains advisory and unarmed. Local checks have no hosted URL; the two linked runs
provide final hosted equivalents. Build logs/evidence files stay ignored and uncommitted.

The original Task1/Task2 RED records remain preserved: both named target assertions executed
and failed intentionally, CTest exit8; the SDK classifier returned RED_EVIDENCE_OK,
exit0. They were assertion failures, not discovery, syntax or fixture failures. Pipeline
test/ci commit types retain the repository's no-product-release policy.

## Historical failures and retained gaps

Original run39694430448 stopped at the pre-existing Windows digest preview failure.
Accepted repair run39699892851 restored Windows current609+6/base605+6 with unchanged
trusted-base sources. The final207s timeout calibration and its103.28s/96.22s arm64
observations remain preserved; this continuation did not reroute or weaken that control.

Final called run39706506980 attempt1 hit unchanged built.openbios at1.24s against1s,
causing12 fixture-dependent tests not to run and native exit8. The missing artifact
correctly failed the aggregate. After independent equality completed successfully,
gh run rerun 39706506980 --failed — exit0 — retried the same commit. Attempt2 Windows
current609+6/base605+6 and the dependent quality/roll-up pass. No timeout or source repair
is claimed; the timing observation remains a tooling seed.

Coverage, open crashes, backlog and comparable historical duration remain explicit gaps.
Library, WASM and property observations remain advisory; no lane promotion is implied.
Public compatibility means the executed public title predicates, not whole-library coverage.
The descriptorless TUNE-03 boundary prohibition remains flagged/unverified for later
phase acceptance. Shared TUNE-03/TUNE-04 remain pending outside this completed plan.

No goal-blocking stub, intentionally skipped test or unrun plan verification remains.
WINDOWS entry15 is fixed by final hosted acceptance. Existing unrelated open defects and
tooling seeds are preserved, including the relative direct-budget invocation, older
actionlint queue syntax and inherited shellcheck diagnostics. No dependency was installed
to silence them.

## Allocation and measured count

Before Task3 additions the unused fixes70 transferred to quality850 ->920, leaving
fixes30. This was written into the existing initialization/allocation authority first.
Count368/BE205/digest447, reserve0, totalN1970 and C14853 did not change.

| Component | Allocation | Actual incremental lines |
|---|---:|---:|
| Count/inventory |368|368|
| BE relocation/equality |205|193|
| Digest |447|447|
| Quality, all controls/declarations/workflow glue |920|920|
| Reproduced fixes |30|30|
| Reserve |0|0|
| Total |1970|1958|

B12885 - S2 + actual1958 =14841 physical lines, within C14853. All new join code,
controls, policy fields and embedded glue are counted as whole files. Full/authored
source counts8580/6533 and ratios172.97%/227.17% are advisory. There is no source padding,
line-budget exclusion, cap increase, new allowance or weakened safety control. Remaining
BE12 is still its scoped allocation, not replenished reserve; final phase retirement and
tightening belong to the later acceptance plan.

## Deviations from Plan

- [Rule2 - Missing critical integration] The narrow quality_join.cmake file supplies
  exact artifact membership/provenance and extrema reduction, counted from creation.
  It reuses quality_get and the existing judge instead of introducing a policy framework.
- [Rule3 - Documentation budget] The testing page exceeded40KB after the additions.
  Quality prose moved to docs/quality.md and the testing CI table/link was condensed.
  The unchanged absolute-root CI budget check passes; no budget increased.
- [Rule1 - Reporting bug] The judge displayed configuration in place of units.
  A typed units read and positive report assertion fix that; active reason JSON is visible.
- [Rule1 - Initialization evidence] Actual independent archive variation required a
  tighter supported ceiling, not a baseline exception.51a3f95 applies399937.

Authentication gates: none. No owner verification, downstream plan, new GSD step,
settings mutation, ship or merge ran.

## Threat Flags

| Flag | File | Description |
|---|---|---|
| threat_flag: trusted-base-filesystem | tests/cmake/quality_evidence.cmake | Reads and builds an event-selected verified Git base in a separate directory. |
| threat_flag: artifact-join | tests/cmake/quality_join.cmake | Consumes exact same-run producer paths and validates SHA/source/configuration/environment before numeric reduction. |

## Next action

Continue the next unfinished plan in a fresh session with $gsd-execute-phase 5.
This execution stops after its summary and sequential state/roadmap updates.

## Self-Check: PASSED

All five created implementation/documentation files exist. All nine task commits
are ancestors of HEAD; the evaluation-scope resolver finds the preserved plan
history. The on-disk ledger measures20 commits between the recorded heads.
Coverage classification and diff checks pass. No tracked file was deleted.
The fresh classifier reports four auto-covered deliverables and zero errors;
file existence and ancestry checks pass before any state advancement.
