---
phase: 05-pipeline-tune-up
plan: "13"
status: complete
halted: false
tasks_completed: 3
subsystem: infra
tags: [cmake, quality, retained-stores, authenticated-observations, ci]
requires:
  - phase: 05-05
    provides: Accepted original-item authentication, ordinary aggregate and fixed allocation
provides:
  - Consumer-owned typed numeric reads with preserved authentication and unavailable handling
  - Retained compatibility equality and legacy lifecycle proof with spent POKEY row retirement
  - Current full local and required direct/called acceptance with authentic nine/eight-artifact joins
affects: [05-07, 05-08, 05-09]
tech-stack:
  added: []
  patterns: [authenticate before reduction, measured-consumer numeric ownership, retained legacy domain authorities]
key-files:
  created:
    - .planning/phases/05-pipeline-tune-up/05-13-SUMMARY.md
  modified:
    - tests/cmake/quality.cmake
    - tests/cmake/quality_join.cmake
    - tests/policy/quality.cmake
    - tests/compat/allow.txt
    - docs/quality.md
    - .planning/phases/05-pipeline-tune-up/deferred-items.md
    - .planning/WINDOWS.md
key-decisions:
  - Accept only the finite remove3/add2 read movement; measured Task1 fit is14841/quality920 and final spent-row retirement yields14840/quality919.
  - Retain ordinary quality.json and both legacy stores/judges; no new immutable domain grants or automatic restoration are claimed.
  - Treat inherited whitespace exit2 as an explicit verification-scope warning under AGENTS6/7, preserving historical output and current-plan diff0.
  - Accept release-called required CI and authentic8 evidence while reporting overall Release failure from the advisory WASM fetch; no release/rehearsal success or promotion is claimed.
requirements-completed: []
requirements-contributed: [TUNE-03, TUNE-04]
coverage:
  - id: authenticated-consumer-reads
    description: Original identity and status precede measured numeric reads, reduction and actual bounds
    requirement: TUNE-03
    verification:
      - kind: unit
        ref: ctest --preset ci -R '^(perf[.]gate[.]|policy[.]quality[.]controls$)' --no-tests=error --output-on-failure; exit0;8/8
        status: pass
      - kind: integration
        ref: python3 build/plan-evidence/recovery-focus.py; exit0; malformed values reject and unavailable-after-measured paths succeed without a value
        status: pass
    human_judgment: false
  - id: retained-performance-contract
    description: Seven perf cases, four added assertions and production-shaped ALLOW invocation retain their exact faults
    requirement: TUNE-04
    verification:
      - kind: unit
        ref: Final full ci and selected seven perf cases; exit0
        status: pass
      - kind: other
        ref: cmake -DROM=testcard -DHEAD=1010000 -DBASE1=1000000 -DBASE2=1000000 -DALLOW=tests/perf/allow.txt -DARMED=true -DNO_BASE=false -P tests/cmake/perf_compare.cmake; exit0;delta_bp=100 verdict=PASS
        status: pass
    human_judgment: false
  - id: retained-compatibility-lifecycle
    description: Fresh bidirectional equality remains unconditional while the weaker merged-nonpass legacy lifecycle is explicit
    requirement: TUNE-03
    verification:
      - kind: integration
        ref: python3 build/plan-evidence/recovery-compat.py; exit0; exact mismatch, waiver and merged-base assertions through the real judge
        status: pass
    human_judgment: false
  - id: pokey-retirement
    description: Accepted and trusted bases contain the passing replacement and fresh fast/OpenBIOS results prove the old allowance spent
    requirement: TUNE-03
    verification:
      - kind: integration
        ref: Final generated-corpus library workflow against origin/main; exit0;43/43;POKEY pass frames120/328 and replacement evidence dcef4b423da52d04
        status: pass
    human_judgment: false
  - id: strict-physical-fit
    description: Full counted inventory fits quality920 and strict14841 with unchanged other component use
    requirement: TUNE-03
    verification:
      - kind: other
        ref: gate_count exit0 observes14840; explicit jq14841/14853 gate exit0; all146 paths compared to5535b57;quality919
        status: pass
      - kind: other
        ref: cmake -DROOT="$PWD" -P tests/cmake/budgets.cmake; exit0;19 files
        status: pass
    human_judgment: false
  - id: current-local-inventories
    description: Final full local ci and generated-corpus library execute their exact inventories after the last implementation edit
    requirement: TUNE-03
    verification:
      - kind: integration
        ref: cmake --workflow --preset ci and generated-corpus library; exit0;check_junit ci609/release6/library43 each exit0
        status: pass
    human_judgment: false
  - id: current-required-hosted-callers
    description: Current direct numeric perf and executed library/quality plus release-called required CI accept authentic exact artifact sets
    requirement: TUNE-04
    verification:
      - kind: integration
        ref: https://github.com/szTheory/muhtari/actions/runs/39832530167
        status: pass
      - kind: integration
        ref: https://github.com/szTheory/muhtari/actions/runs/39832530253/job/120021278635
        status: pass
      - kind: other
        ref: Real quality_join on authentic direct9/called8 sets with observed producer/base/source identities; both exit0
        status: pass
    human_judgment: false
actuals:
  tokens: 3644
  tasks: 3
  commits: 2
plan_head_before: c7e84a91f097ae1d9def016ce6e50b1e2d9b0c2f
plan_head_after: 31c52d93ade6e290593e570c1fb3c972cce576a3
duration: 24min plus closeout
started: 2026-10-22T18:29:49Z
completed: 2026-10-22
---

# Phase 5 Plan 13: Consumer-owned numeric reads and retained-store recovery Summary

**Authenticated measured consumers retain every existing fault, fit14840/quality919 and pass current local and required direct/called evidence.**

All three tasks are accepted. Active completion becomes6/9:01,02,10,04,05,13.
Historical06/11/12 remain halted0/3; none was relabelled complete. TUNE-03/TUNE-04 remain
pending with sibling work. No07 arming/promotion,08 release work,09 final tightening,
owner UAT, ship or merge ran. The overall Release workflow is **failure**, not green:
its advisory WASM fetch failed before tests even though required called CI and quality passed.

## Task commits and lineage

1. Task1 — `8fd04db040ff09cb64c74b1ceb67bbce0d82fbcf`: read measured values at authenticated consumers.
2. Task2 — `31c52d93ade6e290593e570c1fb3c972cce576a3`: document retained authorities and retire only the spent allowance row.
3. Task3 — existing callers required no implementation edit; acceptance evidence is recorded in this canonical summary commit.

The persisted ledger measures two production commits through the recorded plan_head_after;
metadata closeout commits follow that measured boundary. Actual tokens are ceil14574/4 over
the realized implementation/documentation/defect-note diff before this summary, not harness tokens.
No tracked file was deleted. Hooks and the existing GitHub noreply identity were used.

RED `df6b0959dfbb31f843ada7fe3cbfc7e1b1f24c25` and
`94d5c49b01feafb96833d5d1ce61786ab81dc261` remain ancestors. Their intentional RED/halt
records are unchanged. The current selected8/8 already passed before the read movement;
this plan preserves that behavior and does not fabricate another RED or claim those earlier
plans completed. Workflow tdd_mode is false; no new behavior, test registration or framework
was needed. Task1 is a refactor; Task2 is an evidence-backed spent-row/docs change.

## Finite construction and controls

Removed exactly the shared reader's three-line conditional numeric block and added one typed
read in each measured consumer. Join reads before extrema; aggregate reads after structural
initialization/advisory-unavailable continues and required-status rejection, before bounds.
quality_get is unchanged. Authentication, status rejection, extrema, comments and controls
remain. The exact forged-SHA expectation retains observed/expected values and whitespace
tolerance. Its equal-valued fixture proves original identity rejection; it does not prove a
smaller observation hidden by extrema. That stronger tooling claim stays deferred.

Focused agent-run mutations through production scripts reject measured JSON strings/missing
values with typed NUMBER diagnostics and negative/fractional values with the nonnegative
integer diagnostic. Advisory-unavailable after measured, structural pre-policy unavailable
after measured, and the real join's permitted unavailable base gate_lines all pass without
a stale or synthesized value. Existing aggregate introduction/base/from/to/grant/calendar/
expiry/renewal/conversion/actual-value controls pass. All seven named perf cases and the
wrong-ROM, malformed-allowance, zero-HEAD and negative-BASE2 assertions survive unchanged.

## Full physical and component measurement

All146 inventory paths were compared byte-for-byte and physically against accepted
`5535b57168ef31a8993c28646920a8b88ed0e98a`. Only these five counted paths differ:

| Path | Accepted | Start | Final | Final minus accepted |
|---|---:|---:|---:|---:|
| tests/cmake/quality.cmake |230|238|236|6|
| tests/cmake/quality_join.cmake |114|101|102|-12|
| tests/policy/quality.cmake |201|201|201|0|
| tests/perf/tests.cmake |67|73|73|6|
| tests/compat/allow.txt |8|8|7|-1|
| Complete inventory |14841|14842|14840|-1|

Task1 actually measured14841/quality920 before expansion; remove3/add2 is no longer a
forecast. Task2 removes only the proven spent row, yielding14840/quality919. Every other
counted path is byte-identical to accepted5535b57, including quality.json, both domain
judges, library caller, workflows, inventories and controls. Docs are outside that inventory
and pass the unchanged separate document budget. No required compatibility/caller/control
repair or additional counted cost was discovered or silently absorbed.

| Component | Fixed allocation | Final actual |
|---|---:|---:|
| Count/inventory |368|368|
| BE relocation/equality |205|193|
| Digest |447|447|
| Quality including controls/glue |920|919|
| Reproduced fixes |30|30|
| Reserve |0|0|
| Total incremental scope |1970|1957|

B12885-S2+N1970=C14853 remains fixed. Actual12885-2+1957=14840 fits strict14841
and C14853; quality14840-13921=919 fits920. BE12 is not reserve. Source8580/6533
is unchanged and ratios remain advisory. No allocation, cap, waiver, padding, line density,
hidden exclusion, comment/control removal, fallback construction or gap round was used.
Final tightening and retirement of unused allocation remain09 work.

## Retained compatibility authority and POKEY audit

The real compat_ratchet rejects missing/extra/status/evidence/malformed fresh results even
with a matching allowance. Exact live waiver identity/window can authorize trusted-base
pass loss or disappearance; absent/wrong-id/wrong-predicate/expired allowances reject.
An accepted merged-nonpass base permits that nonpass without an allowance, but still rejects
fresh mismatches. Both original judges/stores, BASE/ALLOW wiring, fixture prerequisites,
flipped-row self-test and exact inventories remain. No behavior repair was necessary.

Verified `origin/main^{commit}` is65b360d1afd37b7dad9f94dcff497c85df84fd1d, also the
observed PR event base. Both it and accepted5535b57 contain only the replacement POKEY
predicate `title@120=dcef4b423da52d04a824def3dfa1f0167c5b9e5bdfbf5af55906d21b46464244`,
status pass, evidence dcef4b423da52d04. Fresh generated-corpus library results contain that
same pass/evidence; its real predicate command prints pass frame120 and frame328 for fast
boot and OpenBIOS respectively. The old
`title@120=8fed977457666af8e363f6bd4e5f37375e1f457d67156ef326042fc684f1ed9e`
row is absent from both bases. Therefore only its spent allowance row was removed; this is
not retirement by expiry date or renewal.

docs/quality.md now assigns ordinary bounds to quality.json and each retained domain to its
legacy ALLOW store/judge. It states the weaker merged-nonpass limitation and promises no
immutable domain grants, renewal prevention, automatic restoration or unified policy.

## Current commands and exits

Local checks have no hosted URL. Generated fixtures, JUnit, API snapshots and logs remain
ignored under build/plan-evidence; no log is committed. Final local verification follows
the last implementation edit; closing metadata does not change that implementation.

| Command | Exit | Evidence |
|---|---:|---|
| cmake --preset ci && ctest --preset ci -R '^(perf[.]gate[.]\|policy[.]quality[.]controls$)' --no-tests=error --output-on-failure |0| Selected8/8 before/after edit and tracer reverify |
| cmake -DROM=testcard -DHEAD=1010000 -DBASE1=1000000 -DBASE2=1000000 -DALLOW=tests/perf/allow.txt -DARMED=true -DNO_BASE=false -P tests/cmake/perf_compare.cmake |0| delta_bp=100 verdict=PASS |
| python3 build/plan-evidence/recovery-focus.py |0| Eight typed malformed-number rejections, three unavailable successes, copied unavailable has no value; all146-path comparison |
| python3 build/plan-evidence/recovery-compat.py |0| Real judge's equality, transition, waiver and merged-base assertions; accepted/current POKEY base audit |
| cmake --workflow --preset ci |0| Final609 regular and6 release/archive tests |
| git cat-file -e 'origin/main^{commit}' |0| Verified65b360d base |
| cmake -E env MUHTARI_ROM_DIR="$PWD/build/ci/built" MUHTARI_LEDGER_BASE=origin/main cmake --workflow --preset library |0| Final43 tests, fresh results, flipped-row control and POKEY fast/OpenBIOS |
| cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake |0|609 executed once each |
| cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake |0|6 executed once each |
| cmake -DJUNIT=build/library/junit.xml -DINVENTORY=tests/inventory/library.txt -P tests/cmake/check_junit.cmake |0|43 executed once each |
| cmake -DROOT=. -DINVENTORY=tests/policy/gate-lines.txt -DOUT=build/plan-evidence/recovery-count.json -P tests/cmake/gate_count.cmake |0|14840 physical lines |
| jq -e '.gate_lines <= 14841 and .gate_lines <= 14853' build/plan-evidence/recovery-count.json |0|true;quality919;other components unchanged |
| cmake -DROOT="$PWD" -P tests/cmake/budgets.cmake |0|19 files within budget |
| git diff --check c7e84a9..HEAD; git diff --check HEAD |0/0| Current recovery and working tree have no whitespace faults |
| git diff --check origin/main...HEAD |2| Three inherited blank EOF lines; explicit scope deviation below |
| git push origin phase/05-pipeline-tune-up |0| Existing PR20, no merge |
| gh run rerun 39832530167 --failed; gh run rerun 39832530253 --failed |0/0| One same-commit failed-job rerun each; no bounds/source edit |
| gh run view 39832530167/39832530253 and attempt/job/artifact API reads |0| Actual conclusions and identities below; API exit0 is not a passing Release verdict |
| gh run download 39832530167 -p 'quality-*' -D build/plan-evidence/recovery-direct |0| Authentic exact9 set |
| gh run download 39832530253 -p 'quality-*' -D build/plan-evidence/recovery-called |0| Authentic exact8 set |

Each final real join uses -DDIRECTORY=build/plan-evidence/recovery-{direct,called},
-DOUT=build/plan-evidence/recovery-{direct,called}-joined, -DPOLICY=quality.json,
-DBASE_POLICY=build/plan-evidence/recovery-{direct,called}-base.json, -DBASE_SHA=65b360d1afd37b7dad9f94dcff497c85df84fd1d
and -P tests/cmake/quality_join.cmake. Direct EXPECTED_SHA=b0a80659d776c7084322bd435eb478a5901b3795,
SOURCE=https://github.com/szTheory/muhtari/actions/runs/39832530167, PERF_REQUIRED=ON exits0.
Called EXPECTED_SHA=31c52d93ade6e290593e570c1fb3c972cce576a3,
SOURCE=https://github.com/szTheory/muhtari/actions/runs/39832530253, PERF_REQUIRED=OFF exits0.
git ls-tree confirms this exact trusted tree predates quality.json, so each trusted adoption
file is the existing empty-metrics policy. No candidate policy substitutes for a base.

## Hosted identity, attempts and authentic artifacts

PR20 head/implementation is31c52d93ade6e290593e570c1fb3c972cce576a3.
Direct producer/tested merge isb0a80659d776c7084322bd435eb478a5901b3795; commit API parents
are trusted base65b360d1afd37b7dad9f94dcff497c85df84fd1d and that head, in that order.
Called producer is the head itself. Both run APIs report the head as head_sha, which is
distinct from the direct artifact's actual merge producer. The real joins authenticate
every original SHA/source/identity/environment/base-policy item before extrema.

- [Direct CI39832530167](https://github.com/szTheory/muhtari/actions/runs/39832530167), attempt2: completed success.
  Required rollup120018032385 succeeds. Actual quality120017745448 bounds and Complete job
  steps succeed; its job-level API status lags in_progress/null despite the terminal-success
  run, successful required rollup and all completed-success steps. The independent authentic9
  join exits0. All six native, hygiene, ASan and library Every test ran steps succeed.
  Library Judge the ledger also succeeds (original attempt1 job120012226556, retained in attempt2).
- [Release39832530253](https://github.com/szTheory/muhtari/actions/runs/39832530253), attempt2: completed **failure**.
  Required called quality120020998852 and CI required120021278635 both complete success;
  authentic8 join exits0. All six native, hygiene, ASan and library inventories actually
  execute successfully; library Judge the ledger succeeds (original attempt1 job120012202380).
  Called perf intentionally skips. Full native/PowerPC equality120012138755 succeeds on attempt1.
  Advisory called WASM120020998725 fails a pinned-tool fetch with curl35 connection reset
  before tests; Every test ran skips, and rehearsal/install-line/cleanup skip. No WASM
  execution, rehearsal, installation, release publication or overall Release success is claimed.

Direct perf is measured3837262 Ir/frame; independent baseline and repeat are both3837262.
Its artifact names GCC15.2.0/Valgrind3.26.0 and testcard;360-60/300;equal-paths;stripped-env.
The production ALLOW caller, first-parent base, duplicate measurements, tools, environment
and advisory/unarmed state remain. Direct/called archive peaks399354/399360 fit399937;
raw independent bases399949/399943 remain reported without normalization. Both sets report
native609/base605, ASan597/base593, hygiene24, ABI12, fuzz4, title3 and zero findings.

Immutable artifact IDs below were read from each run's API with created_at/digest/workflow_run
metadata and downloaded into fresh separate directories. The same-run recovery reuses
successful attempt1 producers; created_at and attempt-specific job execution windows identify
the retried lanes explicitly. It does not relabel every artifact attempt2.

| Artifact suffix | Direct ID | Producer attempt | Called ID | Producer attempt |
|---|---:|---:|---:|---:|
| asan |12450150883|1|12449928308|2|
| hygiene |12448518688|1|12449706375|1|
| linux-arm64 |12449961186|1|12449736384|1|
| linux-x64 |12448608797|1|12449094283|1|
| macos-arm64 |12448349575|1|12450230385|1|
| macos-x64 |12449423087|2|12449124604|1|
| windows-arm64 |12450041453|1|12449503768|2|
| windows-x64 |12448494512|1|12448719096|1|
| perf |12449761589|1|intentionally absent|—|

First-attempt failures remain evidence: direct macOS x64 job120012226551 times out
cpu.decimal.clark at3.17s, CTest8; called Windows arm64 job120012202401 times out
built.samples.self_test at4.03s in unchanged trusted-base605, CTest8/producer1; called
ASan120012202359 cancels at its five-minute job bound during independent-base evidence.
Both initial quality jobs reject missing exact artifact sets with exit1. One unchanged-head
failed-job rerun each recovers required evidence. A rerun pass is a flake candidate, not
a source repair; bounds, workloads and controls were not weakened.

## Deviations, warnings and remaining scope

- Verification-scope deviation: required broad git diff --check exits2 on inherited EOF
  blanks in be-equality.yml:63, gate-lines.txt:148 and historical05-01-SUMMARY.md:421.
  git show at starting c7e84a9 reproduces those preexisting endings; current-plan and working
  tree checks exit0. Under AGENTS6/7, agent judgment records this as a tooling seed rather
  than expanding implementation scope, rewriting history or adding an owner/gap gate.
  The broad command has not passed. WINDOWS22 and deferred-items preserve the warning.
- Advisory called WASM fetch failure and skipped follow-on rehearsal are an explicit tooling
  seed, WINDOWS23. Required called CI and exact8 acceptance passed; the plan retains WASM
  advisory and leaves release/rehearsal scope to08. There is no additional rerun or repair.
- Initial hosted timeouts and the direct quality API status lag are reported above. No
  authentication gate occurred. No new endpoint, auth path, schema or other threat surface
  was introduced; no goal-blocking stub was found in the changed implementation.

All seven supplied assumptions remain unresolved: TUNE-01 unclassified; TUNE-02 adjacency,
empty and ordering; TUNE-03 unclassified; TUNE-04 unclassified; TUNE-05 unclassified.
Concrete controls do not resolve the classifier lifecycle. The descriptorless TUNE-03
physical-boundary prohibition remains flagged/unverified: no hidden controls, moved
predicates, padding, density or dependency may buy apparent fit. No fabricated descriptor
or owner homework was added. The lexical assumption-delta signal changed no architecture,
metric definition, evidence identity, policy authority or one-way-door contract.

Only accepted13 releases the existing07/08/09 dependency chain. Remaining arming/promotion,
release safety and final tightening predicates retain their separate scopes. Historical
SUMMARY/HANDOFF/VERIFICATION files remain byte-for-byte unchanged. Next command:
`$gsd-execute-phase 5`; this executor runs no other plan or GSD step.

## Self-Check: PASSED

Canonical summary exists; both task commits are ancestors of HEAD; the persisted ledger
measures2 production commits; final local inventories, strict physical/component/doc fit,
current required hosted steps and authentic9/8 joins are verified above. Historical records
and the seven unresolved assumptions remain unchanged in status.
