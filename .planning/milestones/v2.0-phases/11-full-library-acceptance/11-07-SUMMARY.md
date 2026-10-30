---
phase: 11-full-library-acceptance
plan: "07"
subsystem: user-facing-claims
tags: [readme, libretro-info, policy, wording, claims]
status: complete
requires:
  - phase: 11-full-library-acceptance
    provides: "The regenerated compatibility page carrying the canonical disclaimer (plan 11-06)"
provides:
  - "tests/cmake/claims_wording.cmake: policy.claims_wording requires the disclaimer in README.md and the .info and rejects five overclaiming phrases in README.md, docs/*.md and libretro/*.info.in"
  - "policy.claims_wording.self_test: a fixture tree whose README drops the disclaimer and whose page overclaims in mixed case, both files named"
  - "tests/cmake/info_claims.cmake: a description over 512 characters fails, with libretro.info_claims.description.self_test on a 513-character fixture"
  - "README.md status in a few lines linking docs/compatibility.md and docs/libretro.md#devices; a trimmed .info description ending with the short disclaimer"
  - "docs/maintaining.md: milestone names are planning labels, not release names"
affects: [11-08, readme, libretro-info, docs]
tech-stack:
  added: []
  patterns:
    - "A wording check names its deny list in the script only; user docs describe it by purpose and point to the script, so the check does not fail on its own documentation"
key-files:
  created:
    - tests/cmake/claims_wording.cmake
  modified:
    - README.md
    - libretro/muhtari_libretro.info.in
    - tests/cmake/info_claims.cmake
    - tests/policy/tests.cmake
    - tests/libretro/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - docs/maintaining.md
    - docs/testing.md
key-decisions:
  - "The .info description is 317 characters (was 467): diagnostics are described as model conformance only, the refused-feature list is shortened to YM2151, Covox/ADPCM, Souper/BupChip and full-XM services, and it ends with the short disclaimer. is_experimental stays true and no capability key changed."
  - "The description length check reads the file with file(READ) and a regex rather than file(STRINGS), so a semicolon in a description cannot split it into list items."
requirements-completed: [COMP-02]
coverage:
  - id: C1
    description: README.md status says in four lines that evidence covers the rows listed with no claim of universal title compatibility, links docs/compatibility.md and docs/libretro.md#devices, and no longer enumerates boards, controllers or PAL
    requirement: COMP-01
    verification:
      - kind: unit
        ref: grep -c on README.md for the exact canonical sentence (1); policy.claims_wording on the real tree (exit 0, 13 files checked)
        status: pass
      - kind: other
        ref: agent read of README.md against the status vocabulary, exit 0 (no supported, compatible, works or partial for a board or device; wording is evidence, listed rows and pointers only)
        status: pass
    human_judgment: false
  - id: C2
    description: The .info description contains the short disclaimer, is at most 512 characters, says diagnostics give model conformance only, and leaves is_experimental true and every capability key unchanged
    requirement: COMP-01
    verification:
      - kind: unit
        ref: grep -c '^is_experimental = "true"$' libretro/muhtari_libretro.info.in (1); git diff -U0 changes only the description line; libretro.info_claims passes on the configured .info
        status: pass
    human_judgment: false
  - id: C3
    description: policy.claims_wording fails when README.md or the .info drops its disclaimer or when README.md, docs/*.md or libretro/*.info.in uses one of the five overclaiming phrases in any case, and its self test names both files on a fixture tree
    requirement: COMP-01
    verification:
      - kind: unit
        ref: ctest --preset ci -R '^policy[.]claims_wording([.]self_test)?$' (exit 0); the self test requires README.md and docs/example.md named, the latter holding a capitalised phrase
        status: pass
    human_judgment: false
  - id: C4
    description: info_claims fails a description longer than 512 characters, proven by a 513-character fixture, while a file with no description line keeps its existing message
    requirement: COMP-02
    verification:
      - kind: unit
        ref: ctest --preset ci -R '^libretro[.]info_claims' runs three tests, all pass; libretro.info_claims.description.self_test requires "description is 513 characters, over 512"
        status: pass
    human_judgment: false
  - id: C5
    description: docs/maintaining.md says milestone names are planning labels, not release names, with a neutral phase pull request title and no release-as override or package rename
    requirement: COMP-01
    verification:
      - kind: unit
        ref: grep -c 'planning labels, not release names' docs/maintaining.md (1); docs.budgets passes
        status: pass
    human_judgment: false
  - id: C6
    description: The whole gate and the hygiene lane pass with the new tests registered in the inventories and the touched docs within budget
    requirement: COMP-02
    verification:
      - kind: integration
        ref: cmake --workflow --preset ci (exit 0, 673 tests plus 6 release tests passed)
        status: pass
      - kind: integration
        ref: cmake --workflow --preset hygiene (30 tests passed)
        status: pass
    human_judgment: false
actuals:
  tokens: 9500
  tasks: 2
  commits: 2
plan_head_before: 5e5876c0ed3b0729fd28e9c1168589e86b4f0745
plan_head_after: a5f7f9db29a7923314473c097df701210109e318
duration: 30m
completed: 2026-10-29
---

# Phase 11 Plan 07: Wording that matches the evidence Summary

**README, the generated page and the RetroArch core description now carry one disclaimer, a small cmake check keeps it there and rejects overclaiming phrases in any case, and the .info description can no longer outgrow RetroArch's 512-character limit.**

## What changed

- `tests/cmake/claims_wording.cmake` (`-DROOT=<tree>`): requires the exact canonical sentence in `README.md` and `No universal title compatibility.` in `libretro/muhtari_libretro.info.in`; lowercases `README.md`, every `docs/*.md` and every `libretro/*.info.in` and matches five regexes; collects every finding and fails once with `claims_wording:` and each file and phrase; on success prints the count of files checked (13 on the real tree). `policy.claims_wording` and its self test are in `tests/policy/tests.cmake`, `ci.txt` and `wasm32.txt`.
- `README.md` status is four lines: the early-status flag, the sentence, a link to the compatibility page, a link to the device docs, and the release note. The enumerated board, controller and PAL prose is gone; the facts it listed stay in `docs/hardware/cartridge.md`, `docs/pal-tests.md`, `docs/libretro.md` and the generated page.
- `libretro/muhtari_libretro.info.in`: only the `description` line changed (467 to 317 characters).
- `tests/cmake/info_claims.cmake` checks the description length; `libretro.info_claims.description.self_test` feeds a 513-character description with a mapped claim, so only the length check can fail. Registered in `tests/libretro/tests.cmake` and `ci.txt` only, like the other libretro tests.
- `docs/maintaining.md` (How a release happens, step 1) carries the naming rule. `docs/testing.md` describes `policy.claims_wording`, `policy.compat_ratchet.controls`, `title.local.fixture` and the description bound in the existing test-family lines, plus two self-test table rows.

## Trimmed to pay for the added bytes

`docs/testing.md` went from 40,956 to 40,951 of 40,960 bytes after about 600 added. Trimmed, with no behaviour claim dropped except as listed: the "after a build", "locally" and "in the install list" fillers; the example line under Exceptions and "from the day it is checked"; the duplicate wasmtime fetch command under the wasm32 note (the entry-point block already shows it); "No cache is added or check removed" and "Neither caches its vectors or corpus" (nightly prose, not enforced behaviour); "The artifact name names the lane" in the hash-equality failure bullet; tighter wording in the oldest-systems, pinned-downloads and Docker paragraphs; "Four hosted support files" now reads "Hosted support files total at most 300 lines", which is what `ci.yml` enforces. `docs/maintaining.md` has room (18.8 KB of 40,960). `docs/emulation-tests.md` was not touched.

## Deviations from Plan

None - plan executed exactly as written. The plan's `<action>` names the second deny-list phrase for the mixed-case fixture; the fixture uses "Every game" in `docs/example.md`.

## Self-check: wording against D-21 and D-12

The agent read README.md, `docs/compatibility.md` and the .info against the status vocabulary (pass, fail, not evaluated, unsupported with a reason, skipped with a reason) and the claim wording (rule 6, judgement, not owner UAT). README says "evidence" and "listed" and points to the page; the .info says "diagnostic evidence for model conformance only" and "No universal title compatibility."; neither calls a board or device supported, compatible, working or partial. The only match of the overclaiming list or its looser cousins is the status word `unsupported` in the generated page. Command: `cmake -DROOT=$PWD -P tests/cmake/claims_wording.cmake`, exit 0, `13 files checked`.

## Commands and exit status (all local, run URL N/A-local)

| Command | Exit |
|---|---|
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(policy[.]claims_wording([.]self_test)?\|docs[.]budgets\|libretro[.]info_claims)' --no-tests=error --output-on-failure` | 0, six tests |
| `grep -c` canonical sentence in README.md; `^is_experimental = "true"$` in the .info; `planning labels, not release names` in docs/maintaining.md | 1, 1, 1 |
| `git diff -U0 libretro/muhtari_libretro.info.in` (read once) | only the `description = ` line differs |
| `cmake --workflow --preset ci` | 0, 673 tests plus 6 release tests passed |
| `cmake --workflow --preset hygiene` | 0, 30 tests passed |

## Next

Plan 11-08 (planning docs and seeds) is the next plan in this phase; the orchestrator dispatches it within this step. This executor ran no `/gsd-` chaining command and pushed nothing.

## Self-Check: PASSED

- tests/cmake/claims_wording.cmake, 11-07-SUMMARY.md: FOUND
- Commits 40820de, a5f7f9d: FOUND on phase/11-full-library-acceptance
