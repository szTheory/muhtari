---
phase: 11-full-library-acceptance
plan: "05"
subsystem: compat-docs
tags: [docs, compat, library, local-run, asset-policy]
status: complete
requires:
  - phase: 11-full-library-acceptance
    provides: "The library preset search order, library.local and its counts line (plan 11-04)"
provides:
  - "docs/emulation-tests.md (ledger): search order, size skip, the private command with its ci prerequisite, what library.local counts and the follow-up command that shows the counts line"
  - "docs/testing.md: library.txt also lists library.local; the lane row names the search order"
  - "ASSET_POLICY.md section 4 naming MUHTARI_ROM_DIR and MUHTARI_BIOS inline, with no dotenv template and no unimplemented firmware window scan"
affects: [11-08, compat-ledger, library-preset]
tech-stack:
  added: []
  patterns:
    - "A documented command is run exactly as printed (prerequisite, workflow, follow-up) before the doc is committed"
key-files:
  created: []
  modified:
    - docs/emulation-tests.md
    - docs/testing.md
    - ASSET_POLICY.md
    - .gitignore
  deleted:
    - .env.example
key-decisions:
  - "The counts line is read with a second command because the library test preset sets outputOnFailure; the grep keeps the test command line, which names the dump directory, out of the terminal."
  - "The dotenv template is deleted rather than corrected (no reader exists); local values are passed inline."
requirements-completed: [COMP-03]
coverage:
  - id: C1
    description: The ledger section gives the private command, its named ci prerequisite, the search order, the size skip, what library.local counts, that local.csv never enters results.csv or the ledger and that a fail there is data
    requirement: COMP-03
    verification:
      - kind: unit
        ref: grep -cF on docs/emulation-tests.md for the private command (1), the prerequisite (1) and the follow-up command (1); grep -c 'library[.]local' docs/testing.md (1)
        status: pass
      - kind: other
        ref: agent read of the ledger section against tests/compat/library.cmake and tests/compat/local.cmake, exit 0; the search order, prerequisite message and counts line format match
        status: pass
    human_judgment: false
  - id: C2
    description: The documented prerequisite, then the documented command with MUHTARI_ROM_DIR=build/ci/built, exits 0 without printing the counts line, and the documented follow-up prints exactly one counts line and no path
    requirement: COMP-03
    verification:
      - kind: integration
        ref: cmake --preset ci && cmake --build --preset ci && ctest --preset ci -L '^built$' (exit 0); MUHTARI_ROM_DIR=build/ci/built cmake --workflow --preset library (exit 0, no counts line in its output); ctest --test-dir build/library -R '^library[.]local$' -V piped to grep -E 'local: [0-9]+ of [0-9]+ present' (exit 0, one line, local: 0 of 204 present, 0 pass, 0 fail, 204 absent, commit=28689dffd4a6 tree=dirty)
        status: pass
    human_judgment: false
  - id: C3
    description: No dotenv template, no dotenv reader and no unimplemented firmware window scan is described in user-facing files, and ASSET_POLICY.md names MUHTARI_ROM_DIR and MUHTARI_BIOS inline
    requirement: COMP-03
    verification:
      - kind: unit
        ref: test -z of git ls-files .env.example and of git grep for the dotenv names, MUHTARI_BIOS_DIR and firmware_window_scan over ASSET_POLICY.md, .gitignore, docs and README.md (exit 0, nothing printed)
        status: pass
      - kind: unit
        ref: ctest --preset ci -R '^(docs[.]budgets|policy[.]notices)$'
        status: pass
    human_judgment: false
  - id: C4
    description: The docs stay within the 40,960-byte budget and the whole regression gate passes
    requirement: COMP-03
    verification:
      - kind: unit
        ref: docs.budgets
        status: pass
      - kind: integration
        ref: cmake --workflow --preset ci
        status: pass
    human_judgment: false
actuals:
  tokens: 10300
  tasks: 2
  commits: 2
plan_head_before: 28689dffd4a666bead0ca06a0377f4bfba2e723e
plan_head_after: 54d5326139fca0c5b17458e8e6f1cf9b44af27c6
duration: 45m
completed: 2026-10-29
---

# Phase 11 Plan 05: The documented local run Summary

**The private library run is reproducible from the docs alone: the ci build it needs, the one command with `<dump dir>`, the follow-up that shows the counts line, and no dotenv template or window scan that nothing implements.**

## What changed

- `docs/emulation-tests.md` (ledger): the library preset paragraph states the search order (`MUHTARI_ROM_DIR`, `build/ci/built`, `tests/diag`) and that an empty file or one above the image maximum is skipped before hashing. The closing "A run over your own dumps" line became a short section with the prerequisite, the private command, the follow-up `ctest ... -V | grep` command, what `library.local` counts, why a passing workflow hides the counts line (the library test preset sets `outputOnFailure`), that `local.csv` never enters `results.csv` or the ledger, and that a fail there is data.
- `docs/testing.md`: the inventory paragraph says `library.txt` also lists `library.local`; the `library` lane row names the three search roots.
- `ASSET_POLICY.md`: section 4's dotenv bullet is one bullet naming `MUHTARI_ROM_DIR` and `MUHTARI_BIOS` inline with a link to the ledger section; section 5's untested `local.firmware_window_scan` bullet is gone.
- `.env.example` is deleted with its `.gitignore` negation; the comment above the `.env` rules says values are passed inline. The ignore rules stay.

## Trimmed to pay for the added bytes

Both pages were at the budget (40,933 and 40,957 of 40,960). Now 40,946 and 40,956. No behaviour claim was dropped; sentences were tightened or a duplicate removed.

- `docs/emulation-tests.md`, `## ledger`: statuses wording, the CI paragraph and the new text itself.
- `docs/emulation-tests.md`, `## fuzz` (the largest section, which also holds the audio-sample and battery paragraphs): the run-seed paragraph, the whole-run audio paragraph, `audio.samples`, the pin-update paragraph, the ten-row paragraph, the battery paragraph and the closing hash-equality paragraph, reworded; the `run` row's wording tightened.
- Beyond the plan's named order, because `## ledger` and `## fuzz` alone did not cover the addition: `## prop` (one sentence that repeated the Cases paragraph and `prop.instances_isolated`), `## basic`, `## bios`, `## input`, `## diag`, `## title`-neighbouring prose and the boot-to-play negative-control lines, each by a few bytes.
- `docs/testing.md`, `## The inventory`: one sentence shortened and the `library` label row loses "afresh", which the lane row already says.

## Commands and exit status (all local, run URL N/A-local)

| Command | Exit |
|---|---|
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -L '^built$' --no-tests=error` | 0 |
| `ctest --preset ci -R '^docs[.]budgets$' --no-tests=error --output-on-failure` | 0 |
| `MUHTARI_ROM_DIR=build/ci/built cmake --workflow --preset library` (the documented command, run as printed) | 0, no counts line in its output |
| `ctest --test-dir build/library -R '^library[.]local$' -V \| grep -E 'local: [0-9]+ of [0-9]+ present'` | 0, `local: 0 of 204 present, 0 pass, 0 fail, 204 absent, commit=28689dffd4a6 tree=dirty` |
| `grep -cF` for the private command, the prerequisite and the follow-up in `docs/emulation-tests.md`; `grep -c 'library[.]local' docs/testing.md` | 1, 1, 1; 1 |
| `ctest --preset ci -R '^(docs[.]budgets\|policy[.]notices)$'` | 0 |
| `test -z "$(git ls-files .env.example)"`; `git grep` for the dotenv names and for `MUHTARI_BIOS_DIR`, `firmware_window_scan` over `ASSET_POLICY.md .gitignore docs README.md` | 0, nothing printed |
| `cmake --workflow --preset ci` | 0, 670 tests plus 6 release tests passed on the first run |

The agent read the ledger section against `tests/compat/library.cmake` and `tests/compat/local.cmake` (rule 6): the search order, the named prerequisite and the counts line format in the doc are those the code prints.

## The one local run (D-16)

`MUHTARI_ROM_DIR` is not set in this environment, and the agent does not search for ROMs. Result: **no private content available**.

The public-bytes run above (`MUHTARI_ROM_DIR=build/ci/built`, 0 of 204 present) is the only run made; it proves the command and its output path, not any private image.

## Ship note: optional owner run (not a gate, not UAT, not a ledger update)

Only the owner's machine holds the dumps, so this run is theirs and nothing waits on it. After the `ci` build has made `build/ci/built`:

```sh
MUHTARI_ROM_DIR=<dump dir> [MUHTARI_BIOS=<file>] cmake --workflow --preset library
ctest --test-dir build/library -R '^library[.]local$' -V | grep -E 'local: [0-9]+ of [0-9]+ present'
```

The second command prints the one counts line. It changes no committed file.

## Deviations from Plan

**1. [Rule 3 - Blocking] Budget trims reached beyond the two named sections.** The addition (about 1.1 KB to `docs/emulation-tests.md`) exceeded what `## ledger` and `## fuzz` could give without dropping a claim, so a few bytes were also taken from `## prop`, `## basic`, `## bios`, `## input`, `## diag` and the boot-to-play lines. No claim was dropped. `docs/testing.md` also needed a few bytes back for the longer lane row.

Otherwise the plan executed as written.

## Deferred Issues

- `.planning/preparation/GSD-HANDOFF.md` (line ~193), `CONFORMANCE.md` (section 5), `ENGINEERING.md` (sections 4 and 13) and `PROMPT-COVERAGE.md` still name `.env.local`, `.env.example`, `MUHTARI_BIOS_DIR` and `scripts/library.sh`. They are outside this plan's declared files and not user-facing; plan 11-08's drift grep names the same strings.
- `diag.bankset_tracer` / `diag.bankset` output-directory race is logged in `deferred-items.md` from plan 11-04; it did not occur in this plan's `cmake --workflow --preset ci` run.

## Known Stubs

None.

## Threat Flags

None. Documentation and a deleted template only; no endpoint, auth path or file access was added, and no private path, name, hash or frame was printed or recorded.

## Self-Check: PASSED

- docs/emulation-tests.md, docs/testing.md, ASSET_POLICY.md, .gitignore: present and committed; `.env.example`: absent from the index.
- Commits 362434e and 54d5326 are ancestors of HEAD.
