---
phase: 11-full-library-acceptance
plan: "10"
subsystem: compatibility-docs
tags: [compat-doc, generator, claims, self-test, gap-closure]
status: complete
requires:
  - phase: 11-full-library-acceptance
    provides: "The compatibility generator, its freshness test and self test (earlier plans of this phase), the verification gaps 2 and 3 (11-VERIFICATION.md)"
provides:
  - "A 7800basic title@ pass row is labelled and classed 'public title screen' in the generator, its self test and the published page; nothing calls a frame hash gameplay"
  - "Claims table backing tests that load an image carrying the claim: supergame names diag.sg144 and basic.rmtramdemo, ram names basic.rmtramdemo and basic.rmtramcompressiondemo, pokey450 names basic.pokey, basic.rmtromdemo and diag.pal"
  - "A generator guard that fails naming the claim, the test and the sample when a sample-backed test runs a sample whose ledger row lacks the claim's token"
  - "Self test case sample, nine cases in all"
  - "A dated D-17 amendment in 11-CONTEXT.md renaming the evidence kind"
affects: [compatibility-docs, phase-11-gap-round]
actuals:
  tokens: 1600
  tasks: 2
  commits: 3
plan_head_before: fcf46baa68156abbb9559dcabd9d22c5c8f69120
plan_head_after: 0233c04bd364bee3721263b713978e0fcdd24d13
commits: 3
tech-stack:
  added: []
  patterns:
    - "A claim's sample-backed test is checked against the ledger row of the sample it runs, so the backing column cannot name a test whose image lacks the claim"
key-files:
  created: []
  modified:
    - tools/compat_doc.cmake
    - tests/cmake/compat_doc_controls.cmake
    - docs/compatibility.md
    - .planning/phases/11-full-library-acceptance/11-CONTEXT.md
key-decisions:
  - "The evidence kind for a 7800basic title pass is named public title screen, not gameplay; the rule is unchanged (D-17 amendment, 2026-10-29). D-17's premise misread the title@ predicate, a frame hash, as gameplay evidence; the verification pointed it out and brought no new data."
  - "The sample guard stays in this gap round: it enforces the page's published sentence that each claim names the tests that exercise it, and it is the root-cause fix for gap 3 (the generator checked only that a named test exists, so a linear sample's test backed the supergame claim). Limit: it checks only sample-backed tests (basic.<sample>, title.<sample>.fast, title.<sample>.bios); diag-backed claims remain existence-only."
requirements-completed: [COMP-01]
coverage:
  - id: C1
    description: The page, generator and self test call a 7800basic title@ pass a public title screen and say it is not gameplay; the owned evidence-kind table shows 27 public title screen rows
    requirement: COMP-01
    verification:
      - kind: integration
        ref: ctest --preset ci -R '^(policy[.]compat_doc_fresh([.]self_test)?|policy[.]claims_wording|docs[.]budgets)$' --no-tests=error, exit 0, 4 of 4 passed (self test case kinds requires the new row text); N/A-local, no run URL
        status: pass
      - kind: other
        ref: grep -c 'public gameplay' printed 0 for docs/compatibility.md, tools/compat_doc.cmake and tests/cmake/compat_doc_controls.cmake (baseline 5, 4, 1); grep -cF '| public title screen | 27 |' and grep -cF 'which is not gameplay' on the page printed 1 each
        status: pass
    human_judgment: false
  - id: C2
    description: Each claim's backing-test column names tests that exercise it, and no claim with an evidence class prints none
    requirement: COMP-01
    verification:
      - kind: integration
        ref: policy.compat_doc_fresh passes on the regenerated page; grep -cF 'diag.sg144, title.simple.fast' printed 0 (baseline 1); the three new rows (supergame, ram, pokey450) present verbatim; grep -cE for a none test cell beside a class printed 0 (baseline 2); N/A-local, no run URL
        status: pass
    human_judgment: false
  - id: C3
    description: The generator refuses a claim whose sample-backed test runs a sample lacking the claim, with the message naming the claim, the test and the sample
    requirement: COMP-02
    verification:
      - kind: integration
        ref: ctest --preset ci -R '^policy[.]compat_doc_fresh[.]self_test$' -V lists nine cases including 'compat_doc_controls: sample ok'; N/A-local, no run URL
        status: pass
      - kind: other
        ref: fault direction - with the guard's FATAL_ERROR turned into a STATUS message locally, ctest --preset ci -R '^policy[.]compat_doc_fresh[.]self_test$' exited 8 and named case sample as unexpected; restored, ctest --preset ci -R '^policy[.]compat_doc_fresh' exited 0
        status: pass
    human_judgment: false
  - id: C4
    description: The agent read the regenerated page against the ledger and the title@ definition and judged that every class names what its evidence shows and every backing test loads an image carrying its claim
    requirement: COMP-01
    verification:
      - kind: other
        ref: read of docs/compatibility.md lines 1-100 against tests/compat/ledger.csv board, region and peripherals columns and docs/emulation-tests.md (title); MUHTARI_ROM_DIR=build/ci/built cmake --workflow --preset library, exit 0, 47 of 47 passed including library.compat_doc
        status: pass
    human_judgment: false
---

# Phase 11 Plan 10: Compatibility page labels and claim backing tests Summary

The compatibility page now calls a title-frame hash a public title screen, each claim names tests that load an image carrying it, and the generator refuses a sample-backed test whose sample lacks the claim.

## Accomplishments

- Task 1 (tracer): the title@ label, flag test, class and owned kind pair, the generator comment, the evidence-kind heading line and the Claims paragraph now say public title screen; the self test's kinds fixture row and required text follow. The page shows `| public title screen | 27 |`; the header sentence that owned diagnostics never show gameplay is kept. 11-CONTEXT.md gains a dated D-17 amendment after D-17's bullet list.
- Task 2 (TDD): RED commit adds self test case `sample` (a ledger whose `7800basic-0.41/simple` board is `supergame`), which failed with exit 0 and no message before the guard existed. GREEN commit corrects the three claims entries and adds the guard (per-sample board, peripherals and region columns remembered in the ledger loop, checked after it; controller claims and tests of other shapes are not checked). No refactor commit was needed.
- Page size: 25,813 bytes (budget 40,960).

## Backing-test substitution

None. The ledger confirms rmtramdemo and rmtramcompressiondemo carry `supergame+ram`, pokey and rmtromdemo carry `pokey450`, and basic.pokey, basic.rmtramdemo, basic.rmtramcompressiondemo, basic.rmtromdemo, diag.pal and diag.sg144 are in `tests/inventory/ci.txt`. title.pal.fast has no `7800basic-0.41/pal` row, so it is not checked (diag-backed and unmatched tests stay existence-only).

## Commands

| Command | Exit | Run URL |
|---|---|---|
| cmake -DLEDGER=tests/compat/ledger.csv -DEXPECTED=tests/diag/expected.csv -DINVENTORY=tests/inventory/ci.txt -DOUT=docs/compatibility.md -P tools/compat_doc.cmake | 0 | N/A-local |
| ctest --preset ci -R '^(policy[.]compat_doc_fresh([.]self_test)?|policy[.]claims_wording|docs[.]budgets)$' --no-tests=error --output-on-failure | 0 (4 of 4) | N/A-local |
| ctest --preset ci -R '^policy[.]compat_doc_fresh[.]self_test$' -V (nine cases, sample ok) | 0 | N/A-local |
| guard disabled locally, same self test command | 8 (fault detected), then restored and re-run: 0 | N/A-local |
| MUHTARI_ROM_DIR=build/ci/built cmake --workflow --preset library | 0 (47 of 47) | N/A-local |

The phase gate `cmake --workflow --preset ci` runs in plan 11-11.

## Deviations from Plan

None - plan executed exactly as written. The one recorded deviation from a locked decision (D-17's evidence-kind name) is the plan's own and is amended in 11-CONTEXT.md.

## Known Stubs

None.

## Threat Flags

None.

## Next

Next separate allowed command: `/gsd-execute-phase 11` for plan 11-11 (stop after this step).

## Self-Check: PASSED

- FOUND: tools/compat_doc.cmake, tests/cmake/compat_doc_controls.cmake, docs/compatibility.md, 11-CONTEXT.md amendment
- FOUND commits: a1fdba7, e9998d4, 0233c04
