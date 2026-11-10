---
phase: 05-pipeline-tune-up
plan: "09"
status: complete
halted: false
tasks_completed: 2
subsystem: infra
tags: [closeout, documentation, physical-budget, verified-evidence]
requires:
  - phase: 05-15
    provides: Accepted3/3 actual release-safety continuation and genuine pinned-CLI lifecycle
provides:
  - Final docs/install/settings and source-linked outcome audit
  - Tightened15287-line absolute gate metric with78 lines retired
  - One current acceptance record and exact ship handoff instead of repeated replanning
requirements-completed: [TUNE-01, TUNE-02, TUNE-03, TUNE-04, TUNE-05]
coverage:
  - id: triage-and-monthly-delivery
    description: Public source-linked triage/monthly infrastructure and actual preview exist with the advisory cost defect explicitly deferred
    requirement: TUNE-01
    verification:
      - kind: integration
        ref: sh scripts/digest.sh preview with fixed UTC2026-10-01 to2026-10-22T23:51:14Z; exit0;450 runs,447 attempts,5088 raw job records; cost totals untrusted under COST-01
        status: pass
      - kind: integration
        ref: https://github.com/szTheory/muhtari/actions/runs/39879193514; final-source PR preview succeeds; default-branch publication is a ship check
        status: pass
    human_judgment: false
  - id: bounded-independent-work
    description: Freshness writers are independent, complete membership survives reordering and workload bounds retain real failed observations
    requirement: TUNE-02
    verification:
      - kind: integration
        ref: ctest --preset ci freshness selection --parallel2 --schedule-random --repeat until-fail20 with separate JUnit; exit0; final full/inventory ci610+6, library43 and native/BE610+6 each pass
        status: pass
      - kind: other
        ref: python3 build/plan-evidence/release-continuation/compare-publisher-order.py; exit0; matched ABBA two-worker610-case schedules,18.24% lower local wall; no lost execution
        status: pass
    human_judgment: false
  - id: ordinary-policy-and-fixed-count
    description: Ordinary measured aggregate, retained domain guards and honest whole-file accounting fit the approved allocation and tightened actual
    requirement: TUNE-03
    verification:
      - kind: integration
        ref: Existing quality_join with final authenticated direct9/called8 and independent actual base; both exit0; policy/compatibility/quality controls and all required observations pass
        status: pass
      - kind: other
        ref: gate_count plus complete147-entry audit; exit0; B12885/S2/N2404/C15287, safety447, fixes477, reserve0, broad15346; retired78
        status: pass
    human_judgment: false
  - id: armed-fixture-and-advisory-lanes
    description: Performance is actually armed only at the accepted fixture scope and no unearned lane promotion occurs
    requirement: TUNE-04
    verification:
      - kind: integration
        ref: https://github.com/szTheory/muhtari/actions/runs/39879193530; actual ARMED=true/PASS, testcard3837262 Ir/frame at head/base/repeated base; six required native hashes and actual WASM checks pass
        status: pass
      - kind: other
        ref: Accepted07 distinct-main audit WASM9/library5/property2 versus10 required; no PR, copied retry or skipped record creates promotion eligibility
        status: pass
    human_judgment: false
  - id: docs-install-settings
    description: Documentation matches code and real built/released installations and repository settings are agent-verified
    requirement: TUNE-05
    verification:
      - kind: e2e
        ref: Actual README install_line against built and latest v0.4.0 archives; exit0; genuine rehearsal install job120185505590 succeeds in https://github.com/szTheory/muhtari/actions/runs/39879193669
        status: pass
      - kind: other
        ref: sh scripts/apply-repo-settings.sh --check, budgets19, tree hygiene and git diff --check; all exit0; docs/testing.md40936 bytes within40960
        status: pass
    human_judgment: false
  - id: weakest-dimension-improvement
    description: Reproduced CI reliability faults and measured same-workload scheduling latency receive implemented fixes
    requirement: TUNE-05
    verification:
      - kind: integration
        ref: Accepted02 isolated-output/audio bounds and actual final source098bb5e; complete CI and genuine lifecycle succeed after preserved immutable-base retry
        status: pass
      - kind: other
        ref: Same source/fixtures/tools/two workers, current-cost samples47.281/44.074s versus publisher-first37.910/36.785s; every sample has610 exact executions; mean improvement18.24%
        status: pass
    human_judgment: false
plan_head_before: 592656c
plan_head_after: 098bb5e767b35e34e8f6ffb9545612a4d9d594e8
completed: 2026-10-22
---

# Final pipeline outcome and loop closure

Plan09 is **accepted2/2**, after actual accepted15 recorded atcaee608. All nine
active slots are accepted:01,02,10,04,05,13,07,15,09. Historical14 remains halted1/3
with accepted Task1;03/06/11/12/08 and the original15 halt remain in their original
records/Git. Runtime canonical counts include14 and must not be mistaken for ten
accepted outcomes. Phase-goal verification follows this plan record; ship is the
next GSD step, not another discuss/plan/execute cycle.

## What caused the loop and what changed

The repeated cycle was fixed estimate → construction overrun → halt → discuss/plan
replacement, while stale state/roadmap text kept prescribing discussion and a
SUMMARY filename could look like acceptance. Estimates did not include enough of
the remaining real lifecycle/caller/provisioning controls. Accepting before later
policy/docs edits also forced another complete acceptance pass.

Owner-approved D-35 freezes15/09, allocates the concrete512-line safety maximum,
resumes15 instead of creating16, and stages09 final policy/docs before shared
acceptance. The final implementation at098bb5e now has actual current local,
direct/called, attestation/publication/download/install and confirmed-cleanup proof.
Final actual is tightened once; old halt instructions are removed from current
routing and retained as history. New tooling findings are explicit seeds under
AGENTS7, not another set of phase requirements. No scope/review/gap allowance resets.

## Documentation and actual installation

The agent read README, testing, emulation-tests, runner, libretro, maintaining,
quality and relevant ENGINEERING/DECISIONS against implementation. README now
accurately describes the CPU/graphics/TIA/POKEY core and limited board support.
Native hash count is six; complete BE equality is a separate full nightly/release
contract. Current audio/hash/field/synthesis and API limitations remain accurate;
runner/libretro docs required no cosmetic edit.

Release docs distinguish archive commit from workflow-trigger identity, require
full repository/signer/ref and supported patched verification, describe retained-ID
release-before-tag cleanup, five conservative selectors, all-jobs versus failed-jobs
recovery, and narrow Dependabot eligibility/arming without promising revocation
after later human pushes. No rehearsal-reuse service/state machine is introduced.

Commands and exits:

- `cmake -DREADME=README.md "-DBASE_URL=file://$PWD/build/ci/" -DHOME_DIR="$PWD/build/plan-evidence/release-continuation/install-built-final" -P tests/cmake/install_line.cmake`:0, actual built macOS arm64 core/info installed.
- `cmake -DREADME=README.md -DBASE_URL=https://github.com/szTheory/muhtari/releases/latest/download/ -DHOME_DIR="$PWD/build/plan-evidence/release-continuation/install-latest-final" -P tests/cmake/install_line.cmake`:0, actual released v0.4.0 at40ce425099e16a286d5ac7f96ff697c45bc48138.
- `sh scripts/apply-repo-settings.sh --check`:0, no drift; no admin mutation or owner procedure.
- `cmake -DROOT=. -P tests/cmake/budgets.cmake`:0,19 files; testing40936/40960 bytes.

Actual temporary-release README installation and deletion also pass; see15's
immutable job IDs/digests and [Release39879193669](https://github.com/szTheory/muhtari/actions/runs/39879193669).
No product release or owner hardware/GUI verification is invented for this pipeline step.

## Final complete accounting

| Quantity | Final |
|---|---:|
| Frozen whole-file B |12885|
| Retained real S |2|
| Approved maximum N/C |2482 /15365|
| Final N / sole absolute ceiling C |2404 /15287|
| Actual / strict approved bound |15287 /15353|
| Safety / fixes |447 /477|
| Count / BE / digest / ordinary quality / reserve |368 /193 /447 /919 /0|
| Retired from maximum |78: safety65, BE12, quality1|
| Full / authored source denominators |8580 /6533|
| Advisory full / authored ratios |178.1701% /233.9966%|
| Broad sensitivity |15346, including59-line product RetroArch wrapper|

`cmake -DROOT=. -DINVENTORY=tests/policy/gate-lines.txt -DOUT=<scratch> -P tests/cmake/gate_count.cmake`
and complete component audit exit0. All147 entries are whole files: no ranges,
blank/comment exclusions, moved predicates, hidden controls, source padding or
ratio compliance claim. Correctness/release safety uses447 additional lines versus
accepted7ed793e. Zero-delta CI/policy/caller edits remain audited. No unused BE/
quality allowance funds safety. The final metric/provenance are15287/N2404, not
permission to spend the retired78 later. Future permanent growth needs a new scoped
allocation before implementation under AGENTS7.

The full final local workflows pass:ci610 regular+6 release, hygiene24 and generated-
corpus library43. All three check_junit selections pass exact executed membership.
Final source [direct CI39879193530](https://github.com/szTheory/muhtari/actions/runs/39879193530)
succeeds at tested merge bbc4da9172ee3983d361f5c7e67dadda50d4ef79; final called/rehearsal
succeeds at raw098bb5e767b35e34e8f6ffb9545612a4d9d594e8, attempt2. Each authentic item
is checked before extrema. Direct nine include numeric comparable perf; called
eight intentionally omit perf. Downloaded full native/BE reports each run610+6
once at the same raw commit; both identities and hash files compare. Six native
hash archives independently agree. No predecessor success accepts this source.

Concrete final hosted receipt commands all exit0:

- `python3 build/plan-evidence/release-continuation/authenticate-artifacts.py 39879193530 direct`
- `python3 build/plan-evidence/release-continuation/authenticate-artifacts.py 39879193669 called`
- `python3 build/plan-evidence/release-continuation/check-execution-artifacts.py 39879193530 direct`
- `python3 build/plan-evidence/release-continuation/check-execution-artifacts.py 39879193669 called`
- `python3 build/plan-evidence/release-continuation/confirm-absence.py`

## Six dimensions and measured limits

The baseline rubric in01 is retained:1 repeatedly broken/absent;2 usable with
reproduced gaps;3 functional with material gaps;4 automated/documented/bounded with
a named limitation;5 representative current evidence without material observed gap.

| Dimension | Before | After | Evidence and remaining limitation |
|---|---:|---:|---|
| Performance |3|4|Actual armed deterministic testcard3837262/3837262/3837262 Ir/frame,PASS; fixture-limited, no whole-library or low-power claim|
| CI/CD |2|4|Output race/audio bounds, typed measured aggregate, full native/BE/genuine lifecycle and matched scheduling gain; provisional cohorts, substantial cost and COST-01 remain|
| Ease of use |4|4|Built/latest/rehearsal README installs and all three deliverables; private cartridges remain user-supplied|
| Architecture |4|4|Instance-owned integer bounded core and preserved ABI/no-FP/semantic guards; legacy domain-policy centralization explicitly deferred|
| Docs/README |3|4|Agent source comparison, budgets, correct lanes/audio/release recovery and read-only settings evidence|
| Upgrade path/semver |4|4|Actual immutable non-latest prerelease with patched CLI and confirmed cleanup; real main release and deferred delayed-base edge remain distinct|

CI/CD remains the selected weakest dimension; no replacement dimension or new scope
is introduced. Current-code matched ABBA scheduling uses Darwin25.6.0 arm64,
AppleClang21.0.0.21000101, CMake/CTest4.4.3, identical source/fixtures, two workers
and fresh cost history. Before47.281/44.074s, after37.910/36.785s: mean45.6775→37.3475s,
**18.24% lower local wall**. All610 names execute exactly once in every sample;
COST725 starts the new longest control early. Mean summed testcase durations are
77.102396s before and74.412062s after: an occupied-test-time proxy, not CPU/billed
work. This measures both elapsed latency and test-duration work on the same inventory. Generated experiments restore their
files; the committed registration applies the measured choice. An earlier equal-
cost experiment did not improve latency and is not advertised as a gain.

Historical matched pre-change CI runs39533130206/39532369201 have693/704s overall
wall and1519/1541 job-seconds; source/tests/workflows/pins match between that pair,
but their images were not recovered. Current direct observes957s creation-to-last-
completion,955s execution envelope and3230 job-seconds. Those pipelines have added
independent-base and safety work and different images; **no overall hosted cost or
speed reduction is claimed**. Scoped BE execution moved to full same-head nightly/
release equality, preserving coverage. Slow compiler downloads remain visible.
Local same-workload scheduling is the comparable causal latency improvement.

Final native/control/job cohorts are sub-ten and provisional, split by image,
compiler/tool, workload and direct/called role. Complete control times include
678.10/695.95s Windows arm64 and55.97/73.26s Intel Mac on final source; prior measured
maxima724.35/63.21s informed1450/130s construction bounds. This is not a claim that
every new sample retains exactly twice-max margin. Failed/cancelled/censored runs,
missing metadata and copied retry timestamps do not become successful samples.
WASM9/library5/property2 distinct eligible main successes remain below10; no lane
promotion or unsupported hardware assurance is earned from these PR/retry runs.

## Conservative lifecycle cost and advisory reporter defect

Fresh patched-gh successful-publication cohort is n=1, provisional. Source098bb5e
uses verified gh2.102.0, raw archive commit098bb5e and intended workflow source-ref
refs/pull/20/merge. Rehearsal372s + install7s + cleanup11s =390 observed job-seconds
for those stages. Full called workflow4729 job-seconds includes both attempts and
the failed immutable-base job; envelope2531s/overall2533s includes dependencies and
retry wait, not a separately measured queue or DAG-path estimate. Source15 records
retained ID421353911, immutable publication and fresh complete-list/exact-ID/tag404.

All construction Release runs and attempts were paginated too:

| Run | Source | Disposition | Observed job-seconds |
|---|---|---|---:|
| [39874135839](https://github.com/szTheory/muhtari/actions/runs/39874135839) | 5800051 | failure | 1855 |
| [39874751241](https://github.com/szTheory/muhtari/actions/runs/39874751241) | 265722d | cancelled | 2398 |
| [39875580861](https://github.com/szTheory/muhtari/actions/runs/39875580861) | 55a301b | cancelled | 0 |
| [39876385482](https://github.com/szTheory/muhtari/actions/runs/39876385482) | db24f4a | failure | 3440 |
| [39879193669](https://github.com/szTheory/muhtari/actions/runs/39879193669) | 098bb5e | success | 4729 |

Cancelled metadata-only rehearsal/install rows are not genuine executions or cohort
successes. The cancelled265722d run still records8s always-cleanup work; the queued
55a301b cancellation has no executed jobs. Real failed/cancelled work is retained.

COST-01 was reproduced from real original/copied API records using the actual digest
monthly filter: two209s records become418s although the retry copy was created after
the original completed. Negative skipped placeholders are also possible. The raw
450-run/447-attempt/5088-job-record monthly preview is source-linked evidence, but
its4124.1333-minute total is **untrusted**, not an accepted execution-cost figure.
The one-off closeout audit excludes confirmed copies and nonexecuted skipped/
negative placeholders. Reporter repair is a deferred tooling seed under AGENTS7,
not an extra phase requirement, new gap or falsely claimed fix. WR-01 delayed-release
base selection is the other explicit seed; review disposition has0 open/2 deferred.

## Four-source outcome audit and retained flags

| Source | Current disposition |
|---|---|
| REQUIREMENTS TUNE-01–05 | Delivery/preview with explicit advisory cost seed; bounded independent work; typed ordinary aggregate/fixed count; controlled perf/advisory roles; actual docs/install/settings and weakest-dimension gain |
| ROADMAP four success criteria | Current implementations and actual receipts above; first default-branch publication and merge-only truths are ship checks |
| CONTEXT D-01–08 | Measured bounds, isolated writers, ordering/empty controls, full BE relocation and cache/check-removal restraint retained |
| CONTEXT D-09–13 | Ordinary policy/availability/exceptions and retained legacy semantic guards; fixture-only perf and honest lane eligibility retained |
| CONTEXT D-14–20 | Six-dimension baseline/current audit, matched latency gain, conservative cost, agent docs/settings/install, full accounting and tightening proved |
| CONTEXT D-21–26 | Domain centralization stays deferred; finite ordinary recovery/current caller proof retained; original strict14841 acceptance remains historical |
| CONTEXT D-27–34 | CLI floor/full signer/ref, independent archive identity, complete HTTP/ID/lifecycle/caller controls, authentic current joins and genuine lifecycle proved; old amounts superseded |
| CONTEXT D-35 | Existing15/09 only; approved allocation precedes affected GREEN; final docs/policy precede shared acceptance; actual15287 retires78; no reset/new plan |
| RESEARCH | Complete boundary/consumer accounting, actual all-attempt evidence, no invented missing metrics, same-head full BE, finite release controls and no service/reuse framework |

Three TUNE-02 probe edges now have concrete proof: concurrent writers own distinct
outputs; valid complete empty input differs from missing/malformed/failed required
evidence; equal-cost/random order preserves exact membership and release absence
precedes tag mutation. Four unclassified probe rows remain explicitly flagged as
metadata, not auto-dismissed or invented behavior requirements. The three authored
bespoke prohibitions retain descriptorless/flagged status; no fictitious wired
checks are added. Actual counts/receipts establish their substantive boundaries.
Legacy waived compatibility loss may become the next base's nonpass state; no
immutable ordinary-grant guarantee is falsely extended to legacy domain stores.

## Handoff

The owner verifies nothing by default. No hardware request, admin mutation, new
framework/service, promotion, extra review-fix/gap round or plan16 is introduced.
Task/source evidence is complete; phase-goal verification and tracking reconciliation
finish this same execute step. At ship, confirm final head CI, actual default-branch
digest publication and post-merge green release PR. Those are agent ship checks,
not UAT debt or a reason to repeat this execution. Exact next command: `$gsd-ship 5`.
