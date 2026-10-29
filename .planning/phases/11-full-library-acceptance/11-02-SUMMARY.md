---
phase: 11-full-library-acceptance
plan: "02"
subsystem: compat-ledger
tags: [ledger, softlist, generator, hygiene, cmake, notices]
status: complete
requires:
  - phase: 11-full-library-acceptance
    provides: "Pin recipe that passes bare ids through and sorts by id then predicate (plan 01)"
provides:
  - "tools/softlist_ledger.cmake: one derived ledger row per entry of the pinned MAME a7800 list, plus a -DCHECK mode"
  - "204 list-owned rows in tests/compat/ledger.csv (201 unknown, 3 skipped expansion devices), pin-sorted"
  - "policy.softlist_ledger and four fault fixtures on the hygiene lane"
  - "Notices and ledger column docs that cover the titles and SHA-1s now kept in the test tree"
affects: [11-03, 11-04, 11-05, 11-06, 11-07, 11-08, compat-ledger]
tech-stack:
  added: []
  patterns:
    - "Regenerate-and-compare for derived ledger rows, with the cartridge table cross-checked against the generator's slot table"
key-files:
  created:
    - tools/softlist_ledger.cmake
  modified:
    - tests/compat/ledger.csv
    - tests/hygiene/tests.cmake
    - tests/inventory/hygiene.txt
    - docs/compatibility.md
    - THIRD-PARTY-NOTICES.md
    - PROVENANCE.md
    - docs/emulation-tests.md
key-decisions:
  - "Every list-owned row is boots@120, unknown, local, except hiscore, xboard and xm, which are skipped with boots@120;why=expansion-device; no list-owned row is unsupported at this pin."
  - "The generator cuts the XML into entry blocks by string search, not by CMake list, so a stray semicolon, bracket or backslash in a description is seen and named instead of silently splitting."
  - "A pin-bump failure names every id gained or lost in one error, not only the first, so a rename shows both the old and the new id."
  - "Multi-chip sha1_raw joins SHA-1s in XML part order; the first field is the same as slice order."
requirements-completed: []
coverage:
  - id: C1
    description: The ledger holds exactly one row per entry of the pinned list (204), id the bare MAME short name, derived by tools/softlist_ledger.cmake and equal to its output
    requirement: COMP-01
    verification:
      - kind: unit
        ref: policy.softlist_ledger
        status: pass
      - kind: integration
        ref: title.pin.order
        status: pass
    human_judgment: false
  - id: C2
    description: Every list-owned row is unknown, except hiscore, xboard and xm, which are skipped with why=expansion-device (the requirement's pass/fail/unknown/unsupported plus skipped, used only for these three expansion devices that are not cartridges); none is unsupported or pass
    requirement: COMP-01
    verification:
      - kind: unit
        ref: policy.softlist_ledger
        status: pass
    human_judgment: false
  - id: C3
    description: A list-owned row set to pass, or carrying evidence, fails naming its id (metadata is never a pass)
    requirement: COMP-01
    verification:
      - kind: unit
        ref: policy.softlist_ledger.self_test
        status: pass
    human_judgment: false
  - id: C4
    description: An entry added, removed or renamed by a pin bump fails naming the id
    requirement: COMP-01
    verification:
      - kind: unit
        ref: policy.softlist_ledger.rename.self_test
        status: pass
    human_judgment: false
  - id: C5
    description: An unknown slot fails naming the entry and slot; a description holding a character that would split a field or list, or a non-ASCII byte, fails naming the entry; an XML with no entry fails
    requirement: COMP-01
    verification:
      - kind: unit
        ref: policy.softlist_ledger.slot.self_test
        status: pass
      - kind: other
        ref: cmake -P tools/softlist_ledger.cmake over one-off XML copies holding a semicolon, brackets, a backslash, an ampersand, a non-ASCII byte, and no entry (exit 1, each named)
        status: pass
    human_judgment: false
  - id: C6
    description: The generator's slot table agrees with src/cartdb.c entry by entry (board, options, sound, light-gun, not-a-cartridge and TV flags), so the two tables cannot drift
    requirement: COMP-01
    verification:
      - kind: unit
        ref: policy.softlist_ledger.board.self_test
        status: pass
      - kind: unit
        ref: policy.softlist_ledger
        status: pass
    human_judgment: false
  - id: C7
    description: Notices and ledger column docs cover the derived rows within the docs budget
    requirement: COMP-01
    verification:
      - kind: unit
        ref: policy.notices and docs.budgets
        status: pass
    human_judgment: false
  - id: C8
    description: The whole regression gate and the hygiene workflow pass on the plan head
    requirement: COMP-01
    verification:
      - kind: integration
        ref: cmake --workflow --preset ci
        status: pass
      - kind: integration
        ref: cmake --workflow --preset hygiene
        status: pass
    human_judgment: false
actuals:
  tokens: 13000
  tasks: 3
  commits: 3
plan_head_before: a426fadca8ea1579c3e6173ab34abf5164416482
plan_head_after: 2a7468028f04751420b4eb0c94ff8c17a1b9eca1
duration: 45m
completed: 2026-10-29
---

# Phase 11 Plan 02: Derived softlist ledger rows Summary

**All 204 entries of the pinned list now have an explicit, generator-owned ledger row that can only be unknown or skipped, and a hygiene check regenerates them, fails any claimed pass, and cross-checks the slot table against the cartridge table.**

## What changed

- `tools/softlist_ledger.cmake` (new): reads the pinned `a7800.xml`, the ledger and `src/cartdb.c`; copies rows whose id contains `/`; derives id, title, SHA-1s, board, region and peripherals for the rest through one slot table; sorts with `pin.cmake`'s key and `<semicolon>` placeholder. With `-DCHECK` it also rejects a pinned-list row that is not unknown, skipped or unsupported or that carries evidence, names ids gained or lost, requires the bare-row count to equal the entry count, and compares bytes.
- `tests/compat/ledger.csv`: +204 rows (254 lines). 201 unknown/local, 3 skipped/local (`hiscore`, `xboard`, `xm`, `boots@120;why=expansion-device`). `xboard` has `sha1_raw` `-`; `hiscore` and `xm` keep their shared SHA-1 as separate rows. Owned rows are unchanged (the diff has no deletion).
- `tests/hygiene/tests.cmake` and `tests/inventory/hygiene.txt`: `policy.softlist_ledger` plus `.self_test` (a row set to pass), `.rename.self_test`, `.slot.self_test` and `.board.self_test`, each on a configure-time fixture whose replacement must change the text.
- `docs/compatibility.md`: regenerated with the existing generator.
- `THIRD-PARTY-NOTICES.md`, `PROVENANCE.md`: the MAME software list notice and the data-licence sentence now name the generator, the ledger and the page and say what the test tree keeps; `src/cartdb.c` still keeps no name or title.
- `docs/emulation-tests.md` (ledger, Columns): list-owned rows, the regenerate command, board/peripherals repeat the list, `any` region.

## Commands and exit status (all local, run URL N/A-local)

| Command | Exit |
|---|---|
| `cmake -DXML=build/pins/mame/a7800.xml -DLEDGER=tests/compat/ledger.csv -DCARTDB=src/cartdb.c -DOUT=tests/compat/ledger.csv -P tools/softlist_ledger.cmake` | 0 |
| `cmake --preset hygiene && cmake --build --preset hygiene && ctest --preset hygiene -R '^policy[.](softlist_ledger\|cartdb_generated)$'` | 0 |
| `ctest --preset ci -R '^(title[.]pin[.]order\|policy[.]compat_doc_fresh)$'` and `-R '^title[.]pin'` | 0 |
| `MUHTARI_ROM_DIR=build/ci/built cmake --workflow --preset library` (46 tests) | 0 |
| `ctest --preset hygiene -R '^policy[.]softlist_ledger'` (5 tests) | 0 |
| `ctest --preset ci -R '^(policy[.]notices\|docs[.]budgets)$'` | 0 |
| generator over XML copies with no entry, a `;`, `[`/`]`, a `\`, an `&` and a non-ASCII byte | 1 each, entry named |
| `cmake --workflow --preset hygiene` (30 tests) | 0 |
| `cmake --workflow --preset ci` | 0 |

Acceptance counts: 204 bare rows, 254 lines, 201 unknown and 3 skipped, 3 expansion rows match the pinned regex, `git diff --stat tools/cartdb.cmake src/cartdb.c` is empty.

## Docs budget

`docs/emulation-tests.md` ended at 40,952 of 40,960 bytes. Trimmed within `## ledger`: the nightly-tier sentence, the ratchet self-test bullet, "so no row is skipped", the CI self-test sentence and the "nothing waits on it" clause. Trimmed within `## fuzz`: the storage-bound sentence (already in the table's `-max_len`), the duplicated "both fuzz durations remain 60 seconds" sentence and "Existing" from two control sentences, plus one sentence in the battery paragraph. No behaviour claim was dropped.

## Deviations from Plan

The slot fixture changes the first slot value to `a78_fixture_<slot>` rather than exactly `a78_fixture`; the test regex matches the unknown-slot message either way. The error text for ids gained or lost is `<id>: entry has no row in <CHECK>` and `<id>: row has no entry in the pinned list`, shortened so CMake's message wrapping cannot split the phrase a test matches. Otherwise none; the plan executed as written.

The commit ledger (`gsd-plan-head-before-11-02`) was not written before the first commit; `plan_head_before` is the plan's starting HEAD `a426fad`, and `git rev-list --count` over it gives 3.

## Known Stubs

None.

## Threat Flags

None. The generator reads the pinned list and repository files and writes one build-tree or ledger file; it adds no network, auth or trust-boundary surface.

## Next

Plan 11-03 is the next separate allowed command (`/gsd-execute-phase 11`, one plan per step).

## Self-Check: PASSED

- `tools/softlist_ledger.cmake`, `tests/compat/ledger.csv`, `tests/hygiene/tests.cmake`, `tests/inventory/hygiene.txt`: FOUND
- commits 10a16db, 6181108, 2a74680: FOUND on HEAD
