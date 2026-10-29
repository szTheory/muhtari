---
phase: 11-full-library-acceptance
plan: "06"
subsystem: compat-docs
tags: [docs, compat, generator, claims, guards]
status: complete
requires:
  - phase: 11-full-library-acceptance
    provides: "The 204-row pinned ledger, the documented library run and the status vocabulary (plans 11-01 to 11-05)"
provides:
  - "tools/compat_doc.cmake: two never-blended populations, a derived headline, status by tier tables, evidence kinds, a claims table and six guards"
  - "docs/compatibility.md regenerated: 204 of 204 pinned-list entries accounted for, 49 owned rows, 26 claims, every row listed in two tables"
  - "tests/cmake/compat_doc_controls.cmake: eight-case self test under the existing policy.compat_doc_fresh.self_test name"
  - "Board, expansion and PAL ledger prose in docs/bankset-tests.md, docs/expansion-tests.md and docs/pal-tests.md"
affects: [11-07, 11-08, compat-ledger, docs]
tech-stack:
  added: []
  patterns:
    - "A generated page derives its evidence class from read-only inputs (expected.csv sources, the test inventory) and fails on a ledger that claims more than it shows"
key-files:
  created:
    - tests/cmake/compat_doc_controls.cmake
  modified:
    - tools/compat_doc.cmake
    - docs/compatibility.md
    - docs/bankset-tests.md
    - docs/expansion-tests.md
    - docs/pal-tests.md
    - docs/emulation-tests.md
    - tests/policy/tests.cmake
    - tests/compat/library.cmake
key-decisions:
  - "Claim evidence class is computed: fit rows outnumbering guide, hw and rtl rows of the matching expected.csv ROMs is model-only, otherwise owned-oracle; with no matching row the class is the strongest ledger kind among pass rows carrying the token (public gameplay, public boot only, model-only for an owned diagnostic), else none."
  - "A pass row that is neither a 7800basic title nor a boots row, and every owned diagnostic, counts as model conformance, never gameplay."
  - "The claim map is a static table in the generator because controller families are not ledger columns; claims whose evidence is only in the library lane (ram, pokey450) print no backing test rather than a test that does not exercise them."
requirements-completed: []
coverage:
  - id: C1
    description: The page shows the pinned CC0 list and the owned rows as separate populations with a status by tier table each (all five statuses, zeros included), the derived headline 204 of 204 accounted for with 0 pass, 0 fail, 0 unsupported, 201 not evaluated, 3 skipped, and no percentage of the list
    requirement: COMP-01
    verification:
      - kind: unit
        ref: "grep -c on docs/compatibility.md for the exact headline line (1), 'Pinned CC0 list: 204 entries' (1), 'Owned samples and diagnostics: 49 rows' (1); grep -c 204 tools/compat_doc.cmake (0)"
        status: pass
      - kind: unit
        ref: policy.compat_doc_fresh.self_test case kinds (a six-row fixture yields the exact headline, tier tables and kind counts)
        status: pass
    human_judgment: false
  - id: C2
    description: The page carries the canonical disclaimer, says src/cartdb.c recognising an image is metadata not playability, renders statuses as pass, fail, not evaluated, unsupported (reason), skipped (reason), and never calls a board or peripheral by an overclaiming word; the skipped status covers exactly hiscore, xboard and xm (expansion devices) beside the requirement's pass/fail/unknown/unsupported
    requirement: COMP-01
    verification:
      - kind: unit
        ref: grep -c 'Evidence covers the rows listed; there is no claim of universal title compatibility.' docs/compatibility.md (1); grep -niE 'supported|compatible|works|partial|whole[ -]library|every game|all games|full compatibility' docs/compatibility.md matches only the status word unsupported (headline, tier and kind tables)
        status: pass
      - kind: other
        ref: agent read of the regenerated page and the moved prose against the status vocabulary and the claim wording, exit 0 (grep checks above, reading of the header, claims intro and the three added sections)
        status: pass
    human_judgment: false
  - id: C3
    description: The claims table lists 26 board, peripheral, region and controller claims, each with a computed evidence class (absolute and every controller family model-only, activision, fractalus and bankset owned-oracle) and backing tests that all exist in tests/inventory/ci.txt
    requirement: COMP-01
    verification:
      - kind: unit
        ref: grep -cF '| Kind | Claim | Evidence class | Backing tests |' docs/compatibility.md (1); grep -cE rows for board absolute model-only (1), board fractalus owned-oracle (1); grep -cE '^[|] (board|peripheral|region|controller) [|] ' (26 = 11 board, 7 peripheral, 3 region, 5 controller)
        status: pass
      - kind: unit
        ref: policy.compat_doc_fresh.self_test case inventory (an inventory without diag.sg144 fails naming it)
        status: pass
    human_judgment: false
  - id: C4
    description: The generator refuses a status outside the vocabulary, unsupported or skipped without why=, a pr row that is unknown, a pinned-list pass or fail or one carrying evidence, a ledger token with no claim entry and a backing test missing from the inventory, and a stale committed page is named
    requirement: COMP-02
    verification:
      - kind: unit
        ref: ctest --preset ci -R '^policy[.]compat_doc_fresh([.]self_test)?$' -V lists eight cases (stale, kinds, status, why, pr_unknown, pinned_pass, token, inventory), exit 0
        status: pass
      - kind: unit
        ref: mutation check, the why= guard disabled locally (AND why STREQUAL changed to a string no row has) made policy.compat_doc_fresh.self_test fail naming why, then restored
        status: pass
    human_judgment: false
  - id: C5
    description: Regeneration is byte-identical in ledger order with claims sorted, the library lane check agrees, the docs stay within the 40,960-byte budget and the whole regression gate passes
    requirement: COMP-02
    verification:
      - kind: unit
        ref: policy.compat_doc_fresh and docs.budgets
        status: pass
      - kind: integration
        ref: MUHTARI_ROM_DIR=build/ci/built cmake --workflow --preset library (exit 0, 47 tests including library.compat_doc)
        status: pass
      - kind: integration
        ref: cmake --workflow --preset ci
        status: pass
    human_judgment: false
actuals:
  tokens: 17600
  tasks: 3
  commits: 3
plan_head_before: b7c36473acc85f9372a98d892aadc18740d920a9
plan_head_after: 3243cfcab52e573fd9613ac06a4c9772919b28c3
duration: 50m
completed: 2026-10-29
---

# Phase 11 Plan 06: The regenerated compatibility page Summary

**The compatibility page now answers "is my game listed, and what was shown?": 204 pinned-list entries counted apart from 49 owned rows, 26 claims with a computed evidence class and real backing tests, and a generator that refuses a ledger that claims more than it shows.**

## What changed

- `tools/compat_doc.cmake` takes `-DLEDGER -DEXPECTED -DINVENTORY -DOUT [-DCHECK]`. It splits rows by whether the id has a `/`, counts each population by status and tier (all five statuses, zeros included), derives the headline from the pinned rows (nothing typed, 204 appears nowhere in the generator), counts evidence kinds by rule, prints the claims table, then every owned row and every pinned row in ledger order.
- Guards, each naming the row or claim and each exercised by a fixture: status outside the vocabulary, `unsupported` or `skipped` without `why=`, a `pr` row that is `unknown`, a pinned-list row that is `pass`, `fail` or carries `evidence_commit`, `evidence`, `oracle` or `checked`, a board, peripheral or region token with no claim entry, a backing test absent from the inventory.
- `docs/compatibility.md` is 25,572 bytes. The pass row of a `why=` predicate (the rng-seeded and similar boots rows) shows plain `pass`; only `unsupported` and `skipped` print their reason.
- The hand-written board, expansion and PAL prose left the generator. `docs/bankset-tests.md`, `docs/expansion-tests.md` and `docs/pal-tests.md` each gained a short "Ledger row(s)" section (what the local mailbox rows record, that they are model conformance, the unexecuted banksets sample, the XM profile limits, PAL not retail-verified). The sentence ending in "gameplay accounting" is reworded to "model-conformance observations, not gameplay results". The cartridge-behavior and host-memory-map paragraphs were already owned by `docs/hardware/cartridge.md` and `docs/libretro.md#achievements`, which the page links.
- `tests/cmake/compat_doc_controls.cmake` replaces the configure-time page rewrite behind `policy.compat_doc_fresh.self_test` (same name, same labels, TIMEOUT 10 kept provisional for eight generator runs of about 0.03 s each).
- `docs/emulation-tests.md` names the two new inputs in the regenerate command and says the page shows `unknown` as not evaluated. `library.compat_doc` and the two policy call sites pass both inputs.

## Trimmed to pay for the added bytes

`docs/emulation-tests.md` went from 40,946 to 40,934 of 40,960 bytes after the 95 added. Within `## ledger`: the ratchet bullet, the counts-line sentence under "Your own dumps" (the `outputOnFailure` explanation is kept, the run-time progress remark dropped) and the "nothing waits on it" aside; within `## title`: the local mailbox sentence now says CRC evidence and points to the compatibility page. No behaviour claim was dropped; `## fuzz` was not needed. `docs/testing.md` is untouched (40,956).

## Deviations from Plan

**1. [Rule 3 - Blocking] Tasks 1 and 2 share one commit.** The plan splits the generator into a tracer (Task 1) and the claims table with its two required inputs (Task 2). The generator needs `-DEXPECTED` and `-DINVENTORY` as soon as it exists in its final shape, and splitting the same file mid-structure would have meant committing a generator and three call sites that disagree. Commit 470a123 holds the generator, page, moved prose and call sites; commit 989344b holds Task 2's remaining file (`docs/emulation-tests.md`). Each task's acceptance greps and verify command were run and pass.

**2. [Rule 2 - Missing] The owned evidence-kind table also prints `fail`.** The plan lists the owned kinds without `fail`; without it the counts do not sum to the 49 rows if an owned row ever fails. Also, any pass row that is not a 7800basic title or a boots row counts as model conformance, the conservative choice for an unclassified pass.

No other deviations. No authentication gates. No new stubs.

## Self-check: the claims

Class for `pokey450` and `ram` is public gameplay from ledger pass rows, with `none` as backing test because their evidence runs in the library lane, not in a CI-lane test. `pokey4000` and region `any` print `none` and `none`. This is by rule, and the page says a class describes the evidence and not a promise about any title.

## Commands and exit status (all local, run URL N/A-local)

| Command | Exit |
|---|---|
| `cmake --preset ci && ctest --preset ci -R '^(policy[.]compat_doc_fresh([.]self_test)?\|docs[.]budgets)$' --no-tests=error --output-on-failure` | 0 |
| `cmake --build --preset ci && ctest --preset ci -L '^built$' --no-tests=error` | 0 |
| `MUHTARI_ROM_DIR=build/ci/built cmake --workflow --preset library` | 0, 47 tests including `library.compat_doc` |
| `ctest --preset ci -R '^policy[.]compat_doc_fresh([.]self_test)?$' --no-tests=error -V` | 0, eight `compat_doc_controls: <case> ok` lines |
| Mutation: `why=` guard disabled, same ctest | non-zero, `compat_doc_controls: why: unexpected result`; generator restored, ctest 0 |
| Task 1 and Task 2 acceptance `grep -c` commands (headline, disclaimer, two population lines, `204` in the generator, claims header, absolute, fractalus, 26 claim rows) | 1, 1, 1, 1, 0, 1, 1, 1, 26 |
| `cmake --workflow --preset ci` | 0, 670 tests plus 6 release tests passed |

The agent read the regenerated page against the ledger and the code (rule 6): the headline, tier counts (pinned 201 not evaluated and 3 skipped; owned 46 pass in 27 gameplay, 5 boot only, 14 model conformance, 3 skipped) match `tests/compat/ledger.csv`, the class column matches the `fit` against `guide`/`hw`/`rtl` counts of `tests/diag/expected.csv`, and the added prose in the three test docs states model conformance and no gameplay result. The only match for the overclaiming list is the status word `unsupported`.

## Next

Plan 11-07 (claims wording policy and description check) is the next plan in this phase; the orchestrator dispatches it within this step. This executor ran no `/gsd-` chaining command and pushed nothing.

## Self-Check: PASSED

- tools/compat_doc.cmake, tests/cmake/compat_doc_controls.cmake, docs/compatibility.md: FOUND
- Commits 470a123, 989344b, 3243cfc: FOUND on phase/11-full-library-acceptance
