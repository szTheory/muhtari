---
phase: 05-pipeline-tune-up
plan: "14"
status: halted
halted: true
tasks_completed: 1
subsystem: infra
tags: [release, attestations, gh, tdd, allocation-stop]
requires:
  - phase: 05-13
    provides: Accepted authenticated ordinary quality evidence and retained domain authorities
  - phase: 05-07
    provides: Accepted armed test-card gate and advisory lane disposition
provides:
  - Accepted first guarded attested-to-synthetic-publication slice and reviewed structural adoption
  - Reproduced false cleanup success and retained lifecycle repair/control construction
  - Exact fixed-allocation halt before lifecycle GREEN or current release acceptance
affects: [05-09]
tech-stack:
  added: []
  patterns: [explicit CLI floor, full signer and ref constraints, real-script response queue]
key-files:
  created:
    - .planning/phases/05-pipeline-tune-up/05-14-SUMMARY.md
  modified:
    - quality.json
    - tests/cmake/quality_join.cmake
    - scripts/publish.sh
    - tests/policy/publish.cmake
    - tests/policy/quality.cmake
    - .planning/WINDOWS.md
key-decisions:
  - Accept Task1 only at14965/safety125/fixes155 after trusted adoption and repeated tracer verification.
  - Stop Task2 at15152/safety312/fixes342; strict15141 misses11 and safety300/fixes330 miss12 despite C15153 passing.
  - Preserve the uncommitted publisher draft and failing queue; no provisioning, savings search, scope increase or further acceptance follows the fit stop.
requirements-completed: []
requirements-contributed: [TUNE-03, TUNE-05]
coverage:
  - id: trusted-structural-adoption
    description: Independently identified absent-policy event base permits the reviewed15153 ceiling while ordinary present-base anti-loosening remains
    requirement: TUNE-03
    verification:
      - kind: integration
        ref: gh pr view and remote main/commit API queries; exit0; PR20/base65b360d/tree0f712e7 match local origin/main and quality.json is absent
        status: pass
      - kind: integration
        ref: Real quality_join against predecessor direct9 artifacts, current policy and actual trusted empty base policy; exit0; structural initialization15153
        status: pass
      - kind: unit
        ref: Task1 policy.quality.controls including new actual-caller adoption and retained gate_existing unexplained-loosening rejection; exit0
        status: pass
    human_judgment: false
  - id: guarded-publication-slice
    description: Supported stable CLI, full signer/ref constraints and changed-byte rejection guard the production attested path before synthetic PATCH
    requirement: TUNE-05
    verification:
      - kind: unit
        ref: Task1 ctest policy.publish.controls and policy.quality.controls; exit0; repeated tracer verification after bae83c1
        status: pass
      - kind: other
        ref: Original e0c18a0 signer RED and new runtime RED both reexecuted; CTest8, classifier0 RED_EVIDENCE_OK, intended assertions inspected
        status: pass
    human_judgment: false
  - id: lifecycle-construction
    description: False cleanup success is reproduced but the lifecycle repair and complete response matrix are unaccepted
    requirement: TUNE-05
    verification:
      - kind: unit
        ref: Cleanup unavailable-list RED at ec95123; CTest8/classifier0; old script attempts deletion and prints gone with exit0 after503 failures
        status: pass
      - kind: unit
        ref: Final selected controls CTest8; successful delete baseline fails invalid HTTP response after empty204; matrix rows beyond it are unrun
        status: fail
    human_judgment: false
  - id: complete-allocation
    description: Full147-entry comparison includes retained66 and all caller/control deltas without borrowing other components
    requirement: TUNE-03
    verification:
      - kind: other
        ref: gate_count exit0 observes15152; explicit strict15141/C15153 jq gate exits1; audit safety312/fixes342 exceeds300/330
        status: fail
    human_judgment: false
  - id: fresh-release-acceptance
    description: Provisioning, full current local/direct/called evidence and actual patched-gh rehearsal/install/confirmed cleanup remain blocked
    requirement: TUNE-02
    verification:
      - kind: e2e
        ref: Task2 provisioning and Task3 acceptance unrun after fixed-allocation stop; no current hosted run or artifact identity exists
        status: unknown
    human_judgment: false
actuals:
  tokens: 5837
  tasks: 1
  commits: 3
plan_head_before: 1773f945f4a2a8529e24e9526684ad9214177895
plan_head_after: ec95123ea49b8731dc16591dbf9e8315643d76a0
duration: 12min through halt checks
started: 2026-10-22T21:07:42Z
completed: 2026-10-22
---

# Phase 5 Plan 14: Guarded publication slice and lifecycle construction halt Summary

**Task1 proves guarded synthetic publication and trusted15153 adoption; Task2 stops at15152/safety312, with an uncommitted lifecycle draft and a failing empty204 response control.**

This continuation is **halted1/3**, not accepted. Active plan acceptance remains7/9.
05-09 stays blocked. Accepted05-13/05-07 and halted05-08/06/11/12 records are unchanged.
No requirement, assumption, promotion, final tightening, ship or merge is completed.

## Task commits and preserved draft

1. `99ceff0` — new runtime and actual-caller adoption RED controls.
2. `bae83c1` — accepted Task1 runtime/signer repair and reviewed allocation/caller.
3. `ec95123` — Task2 cleanup RED and response-queue control construction.

Normal hooks and noreply identity were retained. The original signer RED `e0c18a0`
remains an ancestor. Task2's55-line net publisher repair remains **uncommitted**;
it has no GREEN and is preserved for inspection. The complete current queue is also
unaccepted: its positive cleanup baseline fails before later matrix rows execute.
The ledger measures three pre-summary commits. Realized task diff23348 characters
divided by4 gives5837 estimate-scale tokens, including the retained uncommitted draft;
this is not a harness-token measurement.

## Trusted adoption and accepted Task1

Read-only `gh pr view --json number,url,baseRefName,baseRefOid,headRefOid` identifies
[PR20](https://github.com/szTheory/muhtari/pull/20), actual base
`65b360d1afd37b7dad9f94dcff497c85df84fd1d`. Remote main API, local origin/main and
the actual commit API/local tree agree on tree `0f712e7a9df091a3dbaf5bc176a538a0a5869cd5`.
`git ls-tree` proves quality.json genuinely absent there. Empty metrics are therefore
the established absent-file representation, not a substituted present policy.

Only the reviewed gate ceiling14853→15153, provenance N1970→2270 and existing join
caller constant change. The ordinary judge is unchanged. New actual-caller adoption
and retained present-base unexplained-loosening controls pass. The existing real
aggregate/join accepts15153 with actual absent-base metadata using the authentic nine
predecessor artifacts from [CI39832530167](https://github.com/szTheory/muhtari/actions/runs/39832530167),
producer `b0a80659d776c7084322bd435eb478a5901b3795`, base65b360d. This proves adoption
only; those predecessor measurements are explicitly **not changed-path acceptance**.

Task1 first reexecuted the original signer RED: correct/tamper baselines pass, wrong
signer exits0 and reaches PATCH. Expanded RED then observes old2.101.0 reaching
attestation/PATCH. Both unchanged JUnit records pass the classifier and semantic
inspection. The repair adds the shared supported stable numeric tuple guard and
full repository/workflow/ref constraints to genuine prepublication verification.
It preserves separate changed-byte rejection, downloaded signer/ref constraints and
the workflow-trigger/archive-commit distinction; no source-digest constraint is added.

At Task1, correct and2.102.0/later patch/minor/major cases reach the synthetic PATCH;
wrong signer/prefix/ref/case and changed-byte acceptance stop it. Old, failed, empty,
partial, nonnumeric, unknown and prerelease version cases stop before attestation in
direct attested and inherited verify entrypoints. These model argument placement and
failure propagation, not upstream certificates. The tracer command passed again
after its production commit, at14965/safety125/fixes155. No actual release is accepted.

## Task2 RED, construction and exact stop

The new unavailable-transport cleanup control returns503/failure for every gh call.
The pre-repair production cleanup nevertheless attempts release/tag deletion, exits0
and prints that both are gone. CTest exits8 on that intended assertion; unchanged
JUnit passes `RED_EVIDENCE_OK`, exit0. This is distinct from the preserved signer RED.

The retained response queue adds listing, typed schema/ID, status/header, create,
release/tag deletion and confirmation cases. The draft adds one HTTP helper,
successful complete-page validation, explicit substitution checks and identity/absence
confirmations. Its **first measured recount** is15152, failing the fixed construction
gate. No compression, control removal, cap change, allocation transfer, waiver,
additional gap round or generalized savings search follows.

A post-stop diagnostic execution makes the draft's actual state explicit: quality
controls pass; publisher controls fail the positive deletion baseline at empty204.
Command substitution strips trailing newlines from the header-only response, losing
the header/body separator before parsing. This is a new unaccepted draft defect,
not an API outage or a passing cleanup claim. It is recorded in WINDOWS27 and is
not repaired after the allocation stop. Remaining queue cases, known-ID/caller/
already-absent/download coverage and provisioning cannot be claimed complete.

No pins.txt or release.yml edit, official asset revalidation, installed patched CLI,
workflow-output repair or Task3 documentation change was reached. Local gh remains
the previously observed2.101.0; the new guard will reject it for actual verification.
No new hosted release, release/tag deletion, push or rehearsal was performed.

## Complete physical and component audit

All147 manifest entries were compared by content and physical lines against unchanged
accepted `7ed793e81ccf07a07642681b79c08122be641996`; generated audit stays ignored.
The comparison includes changed files with zero line delta and the retained66.

| Changed manifest path | Accepted | Current | Delta |
|---|---:|---:|---:|
|quality.json|101|101|0|
|scripts/publish.sh|183|248|65|
|tests/cmake/quality_join.cmake|102|102|0|
|tests/inventory/ci.txt|617|618|1|
|tests/policy/gate-lines.txt|148|149|1|
|tests/policy/publish.cmake|0|225|225|
|tests/policy/quality.cmake|201|217|16|
|tests/policy/tests.cmake|259|263|4|

Every other entry is unchanged.65+225+16+1+1+4=312 safety lines. The retained66 is
included once: fixture60, dispatcher4, normal inventory1 and manifest1. Of remaining
growth246, accepted Task1 adds59 and Task2 construction adds187. Pin/provisioning,
caller changes and any required further controls would still need this same budget.

| Component | Fixed | Current |
|---|---:|---:|
|Count/inventory|368|368|
|BE relocation/equality|205|193|
|Digest|447|447|
|Quality|920|919|
|Reproduced fixes|330|342|
|Global reserve|0|0|

B12885/S2 remain fixed; reviewed N2270 gives C15153. Actual2269 incremental lines
give15152. The C gate passes with1 line, but **strict15141 misses11 and safety300/
fixes330 miss12**. Unused BE12 and quality1 are not safety funding. Source8580/6533
and ratios stay advisory. Research A1 is not proved: this construction already misses
before provisioning/integration. No universal impossibility claim is made.

## Verification commands and exits

Local commands have no hosted run URL. Generated evidence stays ignored under
build/plan-evidence/release-continuation; no logs are committed.

| Command | Exit | Result |
|---|---:|---|
|git merge-base --is-ancestor e0c18a0 HEAD|0|Original RED retained; historical summaries unchanged|
|gh pr view; gh api remote main/actual base commit; git rev-parse/ls-tree|0|Actual PR/main/base tree identity and genuine absent policy above|
|ctest --preset ci -R '^policy[.]publish[.]controls$' --output-junit original-red.xml|8|Original wrong-signer assertion reproduced after correct/tamper baselines|
|node gsd-tools.cjs check tdd-red-evidence original-red.json --raw|0|RED_EVIDENCE_OK; intended assertion inspected|
|ctest --preset ci -R '^policy[.](publish\|quality)[.]controls$' --output-junit expanded-red.xml|8|Runtime reaches PATCH; old caller cannot admit unavailable base|
|node gsd-tools.cjs check tdd-red-evidence expanded-red.json --raw|0|RED_EVIDENCE_OK|
|cmake -DDIRECTORY=build/plan-evidence/recovery-direct -DOUT=build/plan-evidence/release-continuation/adoption -DPOLICY=quality.json -DBASE_POLICY=build/plan-evidence/recovery-direct-base.json -DEXPECTED_SHA=b0a80659d776c7084322bd435eb478a5901b3795 -DBASE_SHA=65b360d1afd37b7dad9f94dcff497c85df84fd1d -DSOURCE=https://github.com/szTheory/muhtari/actions/runs/39832530167 -DPERF_REQUIRED=true -P tests/cmake/quality_join.cmake|0|Real existing9-artifact join proves reviewed adoption, not current acceptance|
|Task1 cmake --preset ci and selected publish/quality controls, counter and strict/C jq gate|0|2/2;14965; repeated post-commit tracer verification also0|
|ctest --preset ci -R '^policy[.]publish[.]controls$' --output-junit lifecycle-red.xml|8|503 failures still permit old cleanup exit0/deletion/success message|
|node gsd-tools.cjs check tdd-red-evidence lifecycle-red.json --raw|0|RED_EVIDENCE_OK|
|cmake -DROOT=. -DINVENTORY=tests/policy/gate-lines.txt -DOUT=build/plan-evidence/release-continuation-count.json -P tests/cmake/gate_count.cmake|0|15152, full current inventory|
|jq -e '.gate_lines <= 15141 and .gate_lines <= 15153' build/plan-evidence/release-continuation-count.json|1|false; designed construction halt|
|Final ctest --preset ci -R '^policy[.](publish\|quality)[.]controls$' --no-tests=error --output-on-failure|8|Quality passes; publish empty204 positive baseline fails|
|sh -n scripts/publish.sh|0|Syntax only; no lifecycle correctness claim|
|cmake -DROOT="$PWD" -P tests/cmake/budgets.cmake|0|All19 documents within budgets; no docs change|
|sh scripts/hygiene.sh --tree|0|Current tree hygiene|
|git diff --check; git diff --check 1773f94..HEAD|0/0|Current draft and plan commits clean|
|git diff --check origin/main...HEAD|2|Inherited EOF blanks remain in be-equality.yml:63, gate-lines.txt:149 and historical05-01-SUMMARY.md:421|

## Deferred verification and retained limits

WINDOWS26 records the allocation stop,27 the draft204 defect and28 the unrun checks.
The phase deferred-items ledger retains inherited whitespace and advisory-fetch seeds.
Full final ci/library, exact normal/release/library inventories, current direct/called
numeric armed perf and authentic9/8 joins, producer/base/attempt/artifact identities,
current patched-gh rehearsal/publication/download/install/confirmed cleanup and
measured conservative rehearsal cost are **unrun**. No historical or synthetic proof
substitutes for them. Six native required hashes, BE equality, five rehearsal selectors,
permissions, advisory lane contracts and all user docs remain unchanged.

All seven supplied assumptions and descriptorless prohibitions remain unresolved.
Default-branch digest publication and merge-dependent truths remain ship obligations.
No next executor or05-09 is authorized by this summary's existence; a new scoped
decision grounded in the measured miss is needed before affected expansion.
The exact next command for resolving that decision is `$gsd-discuss-phase 5`.

## Self-Check: PASSED

This is a self-check of the truthful partial record, not plan acceptance. All three
listed commits and original e0c18a0 are ancestors; required files exist; historical
08/13/07 summaries and REQUIREMENTS.md have no changes. Final current count, failing
publisher/fit gates, uncommitted draft and every unrun acceptance obligation are
explicitly reported. The plan stays halted1/3 and05-09 stays blocked.
