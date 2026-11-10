---
phase: 11-full-library-acceptance
plan: "12"
subsystem: compatibility-acceptance
tags: [compatibility, CMake, CTest, public-fixtures]
requires:
  - phase: 11-full-library-acceptance
    provides: "Committed generator and local-output fixes, public acceptance fixtures, and the existing CI and library workflows"
provides:
  - "Current-source acceptance receipt for CR-01, CR-02, WR-01, WR-02 and WR-03"
  - "Public local-library counts and full-workflow results for the phase verifier"
affects: [phase-11-verification, compatibility-ledger, library-preset]
actuals:
  tokens: 4549
  tasks: 2
  commits: 0
plan_head_before: 0e86575f561bf9482ade97f2d1088e418256d51c
plan_head_after: 0e86575f561bf9482ade97f2d1088e418256d51c
tech-stack:
  added: []
  patterns:
    - "Acceptance reuses existing pass/fault controls and public workflows without changing source or test inventory"
key-files:
  created:
    - .planning/phases/11-full-library-acceptance/11-12-SUMMARY.md
  modified: []
key-decisions:
  - "Accept the five fixed findings from current-source consumers; retain the documented helper/source-only boundary for WR-02"
  - "Keep COMP-03's unclassified category as a flagged assumption and leave hosted evidence to ship"
requirements-completed:
  - COMP-01
  - COMP-02
  - COMP-03
coverage:
  - id: D1
    description: "The generator rejects a wrong token for both title.simple.fast and basic.rmtramdemo, naming the claim, test and sample"
    requirement: COMP-02
    verification:
      - kind: integration
        ref: "cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(policy[.]compat_doc_fresh([.]self_test)?|title[.]local[.]fixture|docs[.]budgets)$' --no-tests=error --output-on-failure -V; exit 0 on both runs; ten controls include sample ok and basic ok; N/A-local"
        status: pass
    human_judgment: false
  - id: D2
    description: "A semicolon in OUT preserves its victim sentinel, and directory OUT is refused before creating a scratch tree"
    requirement: COMP-03
    verification:
      - kind: integration
        ref: "title.local.fixture runs 12 and 13 assert victim/keep survives and outdir.rows is absent; focused CTest exit 0 twice; N/A-local"
        status: pass
    human_judgment: false
  - id: D3
    description: "The unread-entry note and library scanner source control accept the WR-02 fix while preserving its end-to-end limitation"
    requirement: COMP-03
    verification:
      - kind: integration
        ref: "title.local.fixture checks compat_unread_note counts 0 and 2 and library source fault copy d; focused CTest exit 0 twice; missing-file configure branch not forced end to end; N/A-local"
        status: pass
    human_judgment: false
  - id: D4
    description: "Both local and library scanners reject wildcard roots under the existing shared rule"
    requirement: COMP-03
    verification:
      - kind: integration
        ref: "title.local.fixture checks dir*x and dir?x under each prefix; focused CTest exit 0 twice; N/A-local"
        status: pass
    human_judgment: false
  - id: D5
    description: "The local fixture keeps path, name and hash data out of its output and reports its unread-entry count"
    requirement: COMP-03
    verification:
      - kind: integration
        ref: "title.local.fixture reports local: 3 files unread (links or unreadable), thirteen runs as expected, library scans as expected, unread note as expected, and library source scans only through the shared rule; focused CTest exit 0 twice; N/A-local"
        status: pass
    human_judgment: false
  - id: D6
    description: "The generated compatibility page describes title-frame hashes as title-screen evidence, keeps pinned metadata untested, and retains model-only and no-universal-compatibility limits"
    requirement: COMP-01
    verification:
      - kind: integration
        ref: "policy.compat_doc_fresh, policy.claims_wording and docs.budgets pass in cmake --workflow --preset ci, exit 0; page read against the generator, ledger and title predicate documentation; N/A-local"
        status: pass
    human_judgment: false
  - id: D7
    description: "The complete existing CI workflow retains its full inventory and release package checks"
    requirement: COMP-02
    verification:
      - kind: e2e
        ref: "cmake --workflow --preset ci; exit 0; 673 of 673 CI tests and 6 of 6 release tests passed; N/A-local"
        status: pass
    human_judgment: false
  - id: D8
    description: "The public built-sample library workflow passes and its verbose local follow-up accounts for every pinned row"
    requirement: COMP-03
    verification:
      - kind: e2e
        ref: "MUHTARI_ROM_DIR=build/ci/built cmake --workflow --preset library; exit 0; 47 of 47 tests passed; public build/ci/built inputs only; N/A-local"
        status: pass
      - kind: integration
        ref: "bash -o pipefail -c \"ctest --test-dir build/library -R '^library[.]local$' --no-tests=error -V | grep -E 'local: [0-9]+ of [0-9]+ present'\"; exit 0; exactly one line: local: 0 of 204 present, 0 pass, 0 fail, 204 absent, commit=0e86575f561b tree=dirty; N/A-local"
        status: pass
    human_judgment: false
  - id: D9
    description: "Pinned-list entries retain separate row identity when chip hashes match, including hiscore and xm, and empty XML retains its fatal source guard"
    requirement: COMP-01
    verification:
      - kind: other
        ref: "11-02-SUMMARY records policy.softlist_ledger and title.pin.order passing; one-off empty-XML invocation of tools/softlist_ledger.cmake exited 1 naming the missing entry population; no dedicated empty-input CTest is claimed; N/A-local"
        status: pass
    human_judgment: false
  - id: D10
    description: "Pin regeneration preserves the title.pin.order ledger ordering and the current page freshness comparison preserves ledger order"
    requirement: COMP-01
    verification:
      - kind: integration
        ref: "cmake --workflow --preset ci; exit 0; existing title.pin.order and policy.compat_doc_fresh consumers are in the full inventory; N/A-local"
        status: pass
    human_judgment: false
  - id: D11
    description: "Ratchet expiry accepts today and rejects yesterday; canonical diagnostics bind mailbox CRC evidence; missing-row and missing-line fatal guards remain explicit"
    requirement: COMP-02
    verification:
      - kind: integration
        ref: "cmake --workflow --preset ci; exit 0; policy.compat_ratchet.controls and diagnostic CRC consumers pass; 11-03-SUMMARY records the today/yesterday controls and one-off missing-row and missing-line failures; diag.ledger_crc.self_test covers CRC mismatch only; N/A-local"
        status: pass
    human_judgment: false
  - id: D12
    description: "Compatibility page freshness preserves ledger and claim ordering deterministically"
    requirement: COMP-02
    verification:
      - kind: integration
        ref: "cmake --workflow --preset ci; exit 0; policy.compat_doc_fresh passes; 11-06-SUMMARY records byte-identical ledger ordering and sorted claims; N/A-local"
        status: pass
    human_judgment: false
  - id: D13
    description: "The existing COMP-03 domain controls remain the stated scope; generic unclassified cases stay a flagged assumption"
    requirement: COMP-03
    verification:
      - kind: other
        ref: "Current 13-run public local fixture and prior 11-04/11-09 coverage retain the enumerated identity, refusal, malformed-input, output and path cases; no generic probe suite or completeness claim is added; N/A-local"
        status: pass
    human_judgment: false
  - id: D14
    description: "The agent confirmed the three flagged prohibitions: metadata and title screens are not gameplay, backing tests must carry the claimed sample token, and local acceptance output contains no private data"
    requirement: COMP-01
    verification:
      - kind: other
        ref: "Current docs/compatibility.md and tools/compat_doc.cmake read against the predicate; focused generator and local fixture consumers pass twice; public-only library counts; N/A-local"
        status: pass
    human_judgment: false
duration: 7min
completed: 2026-10-29
status: complete
---

# Phase 11 Plan 12: Public compatibility acceptance receipt

**Fresh current-source controls and public CI/library workflows accept the five fixed compatibility and local-output findings, while retaining the receipt's source and platform limits.**

## Acceptance identity

- Tested HEAD: `0e86575f561bf9482ade97f2d1088e418256d51c` on `phase/11-full-library-acceptance`.
- All six implemented-fix ancestors are present: CR-01 `e4356c6`, CR-02 `3890f3a`, WR-01 `3810fa9`, WR-02 `6f0a1d4`, WR-03 `e19eb6b`, and the documentation-budget trim `639db89`.
- The pre-run tracked source diff was empty. Before acceptance, the only dirty paths were `.planning/STATE.md`, `.planning/config.json`, `.planning/preparation/DECISIONS.md`, and `.planning/preparation/GSD-HANDOFF.md`. Those shared edits were preserved. The orchestrator-owned `11-REVIEW-DISPOSITION.md` became dirty after the runs and remains outside this receipt's ownership.
- Host: Darwin arm64. The focused fixture reported `local: 3 files unread (links or unreadable)` and emitted no platform limitation note. This local run does not establish behavior on Windows, a root-run lane, or a host unable to create links.
- Outputs remained in ignored `build/ci/` and `build/library/`. Only public built samples were supplied; no private paths, bytes, results or owner UAT were used.

## Acceptance results

| Finding | Current fix and consumer | Acceptance |
|---|---|---|
| CR-01 | `e4356c6`; separate generator matches; control cases `sample` and `basic` mutate the title and basic ledgers | Accepted: both controls fail for the wrong-token ledger and name the claim, test and sample; the ten-case self test passes |
| CR-02 | `3890f3a`; fixture run 12 supplies semicolon `OUT` and checks `victim/keep` | Accepted: the fixed error occurs before the split command can remove the sentinel; the fixture passes |
| WR-01 | `3810fa9`; fixture run 13 passes an existing directory as `OUT` | Accepted: the fixed error occurs and no `outdir.rows` tree is made |
| WR-02 | `6f0a1d4`, with doc trim `639db89`; helper counts 0 and 2, source fault copy `d`, and library paragraph | Accepted within the helper/source-check boundary. The missing-file configure branch was not forced end to end because the public library workflow finds all required row files. The check does not prove arbitrary equivalent CMake spellings |
| WR-03 | `e19eb6b`; fixture rejects `dir*x` and `dir?x` for both local and library prefixes | Accepted: both prefixes stop before globbing; the fixture passes |

`docs/emulation-tests.md` is 40,942 bytes against the existing 40,960-byte limit; `docs.budgets` passes. The full CI and library results below also ran the current freshness, claims-wording and local fixture consumers.

## Task Commits

This acceptance-only plan made no per-task source commits. Both tasks ran existing consumers and their evidence is recorded together in this receipt; the receipt commit hash is returned to the orchestrator.

## Commands and evidence

| Command | Exit | Result | Run URL |
|---|---:|---|---|
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(policy[.]compat_doc_fresh([.]self_test)?|title[.]local[.]fixture|docs[.]budgets)$' --no-tests=error --output-on-failure -V` | 0 twice | Each run passed all 7 selected/setup tests. The self test printed all ten controls, including `sample ok` and `basic ok`; the fixture printed all four required completion lines and the unread count. | N/A-local |
| `cmake --workflow --preset ci` | 0 | 673/673 CI tests and 6/6 release tests passed; package stage completed. | N/A-local |
| `MUHTARI_ROM_DIR=build/ci/built cmake --workflow --preset library` | 0 | 47/47 library tests passed over public built samples. | N/A-local |
| `bash -o pipefail -c "ctest --test-dir build/library -R '^library[.]local$' --no-tests=error -V \| grep -E 'local: [0-9]+ of [0-9]+ present'"` | 0 | Exactly one line: `local: 0 of 204 present, 0 pass, 0 fail, 204 absent, commit=0e86575f561b tree=dirty`. | N/A-local |

The library counts line identifies the tested 12-hex source and dirty tree without revealing a private directory or file. The pre-run tree had only the four shared planning edits listed above; no tracked source changed during acceptance. No hosted run URL is claimed. Hosted required-check and merge evidence remains a ship-time observation.

## Page and claim judgment

The current generated page says the pinned board, region and peripheral fields are metadata, shows pinned rows as untested rather than passes, and states that there is no universal title-compatibility claim. The `title@` evidence is labeled `public title screen`, matching the documented frame predicate; it is not gameplay evidence. The claim table names sample-backed tests whose ledger rows carry the relevant tokens. Model-only controller and fitted behavior remain labeled as such; the page also says no real HSC ROM is exercised and PAL is not retail-verified. The current README and `.info` wording controls passed in full CI.

## Existing edge coverage retained

| Requirement edge | Current assessment and evidence |
|---|---|
| COMP-01 adjacency | `tools/softlist_ledger.cmake` derives one row per software entry. The 11-02 receipt and `policy.softlist_ledger` acceptance preserve separate ids even when chip hashes match; `hiscore` and `xm` remain distinct rows. |
| COMP-01 empty | The generator retains its fatal guard when XML has no software entry. The prior one-off empty-input invocation exited 1 with the named error; no dedicated empty-input CTest is claimed. |
| COMP-01 ordering | `title.pin.order` compares regeneration with the committed ledger; it passed in the full CI workflow. Page freshness separately checks generated output. |
| COMP-02 adjacency | `policy.compat_ratchet.controls` retains the inclusive expiry edge: an exception expiring today passes and one expired yesterday fails. CRC comparison is attached to the canonical `diag/bankset`, `diag/xboard`, `diag/xm` and `diag/pal` cases per tag. |
| COMP-02 empty | `tests/diag/ledger_crc.cmake` retains fatal guards for a missing ledger row and missing diagnostic line. The 11-03 one-off checks exited 1 for those inputs; the CRC-mismatch self test does not exercise these guards. |
| COMP-02 ordering | `policy.compat_doc_fresh` passed; the 11-06 acceptance records byte-identical ledger ordering on regeneration and sorted claim ordering. |
| COMP-03 unclassified | The existing identity, refusal, malformed-input, path and output cases remain the declared domain coverage. A generic unclassified case remains a flagged assumption; this receipt does not claim a generic classifier or universal edge completeness. |

The three flagged prohibitions were judged against the same current consumers: metadata or a title-screen hash is not gameplay evidence; a named backing test must load a sample carrying the claim token; and the local fixture plus public counts expose no private path, name, content or result. These are agent judgments under AGENTS.md rule 6; no owner verification is needed.

## Historical disposition assessment for the orchestrator

| Historical row | Current recommendation |
|---|---|
| WR-04 | Fixed by 11-10: title-frame evidence now says `public title screen`; current page and wording consumers pass. |
| WR-05 | Fixed by 11-10 and accepted here: claim tests are sample-backed and the fresh control covers both `title.*` and `basic.*`. |
| WR-06 | Fixed by 11-11: new budgets name measurements and `title.pal.bios` has the 4 s floor; current `docs.budgets` passes. |
| IN-05 | Defer optional parsing hardening for noncanonical `.info` description spellings; the committed description and 513-character fault control pass. |
| IN-06 | Defer the historical test-label table cleanup; the existing test inventory and required workflows remain authoritative. |
| IN-07 | Defer explicit canonical CRC call-count guards; the canonical CRC consumers and mismatch control pass in current CI. |
| IN-08 | Defer an additional negative control for pinned-row evidence fields; the generator guard remains in source and the current ten-control freshness test passes. |

The original finding history is unchanged. Current 11-REVIEW.md info items were not reopened, and this receipt is not a new general review round. The executor recommends that the orchestrator record these stale rows as fixed or deferred with their original review ancestry.

## Limitations and next gate

- The acceptance is local to Darwin arm64; all hosted run URLs are `N/A-local`.
- WR-02's missing-file configure error remains covered by the helper and source control, not by an end-to-end missing-file library configure.
- COMP-01 empty and COMP-02 empty predicates remain explicit source guards with prior one-off failure evidence; neither is represented as a dedicated CTest.
- COMP-03's unclassified category remains a flagged assumption.
- The historical `11-VERIFICATION.md` 14/16 failure is preserved. This receipt does not itself mark the phase accepted; the orchestrator's built-in verifier must evaluate it during execute-phase.

## Deviations from Plan

None. Both acceptance tasks passed without source, test, workflow, generated-page or disposition changes.

## Issues Encountered

None affecting acceptance. Only the planned ignored build outputs were produced.

## User Setup Required

None.

## Next Phase Readiness

The receipt is ready for the orchestrator's built-in phase verifier in the current execute-phase step. Hosted evidence remains for ship; no owner UAT is pending.

## Self-Check: PASSED

- The receipt exists and records the tested HEAD, all six fix ancestors, both task results, actual commands and exits, the public counts line, and the unresolved limits.
- Current source diff is empty. No ignored acceptance output is staged.
- No source task commits were required; this acceptance-only plan's measured task-commit count is zero.

---
*Phase: 11-full-library-acceptance*
*Completed: 2026-10-29*
