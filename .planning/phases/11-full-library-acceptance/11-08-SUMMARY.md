---
phase: 11-full-library-acceptance
plan: "08"
subsystem: planning-docs
tags: [planning, seeds, local-run, ratchet, is-experimental]
status: complete
requires:
  - phase: 11-full-library-acceptance
    provides: "The user doc for the inline library command and the counted library.local pass (plan 11-05), the wording checks (plan 11-07)"
provides:
  - "CONFORMANCE section 5, ENGINEERING lane table and section 13, GSD-HANDOFF and PROMPT-COVERAGE rows 19 and 27 describing the inline MUHTARI_ROM_DIR command, each rewritten decision marked superseded by Phase 11 on 2026-10-29"
  - "CONFORMANCE section 5 protected-baseline bullet naming the judge that exists (the pull request's tests/cmake/compat_ratchet.cmake), saying the baseline checkout is not built, pointing to SEED-014"
  - "HOSTS-AND-INTEGRATION is_experimental row and SEED-001 requiring pinned-list (commercial) evidence from a future milestone"
  - "SEED-011 to SEED-014, dormant: movie-driven device checks, the is_experimental flip criterion, a locally observed doc section, ratchet judge hardening"
affects: [planning-docs, seeds]
tech-stack:
  added: []
  patterns:
    - "A superseded planning decision keeps its Rec/Alt/Flip text and gains a dated 'superseded by Phase N' note"
key-files:
  created:
    - .planning/seeds/SEED-011-movie-driven-device-checks.md
    - .planning/seeds/SEED-012-experimental-flag-criterion.md
    - .planning/seeds/SEED-013-locally-observed-section.md
    - .planning/seeds/SEED-014-ratchet-judge-hardening.md
  modified:
    - .planning/preparation/CONFORMANCE.md
    - .planning/preparation/ENGINEERING.md
    - .planning/preparation/GSD-HANDOFF.md
    - .planning/preparation/PROMPT-COVERAGE.md
    - .planning/preparation/HOSTS-AND-INTEGRATION.md
    - .planning/seeds/SEED-001-whole-library.md
key-decisions:
  - "The flip of is_experimental waits for pinned-list (commercial) evidence a future milestone defines; Phase 11 accounts every pinned entry as not evaluated or skipped, so the .info keeps is_experimental true (SEED-012)."
  - "The ratchet's remaining gaps (push events with an empty base ledger, a judge taken from the pull request rather than the base commit) are a dormant seed, not a task (SEED-014, AGENTS rule 7)."
requirements-completed: [COMP-01, COMP-03]
coverage:
  - id: C1
    description: No tracked file outside phase history names a dotenv file, the BIOS-folder variable or the wrapper and ratchet shell scripts; each rewritten planning decision names the inline library command and is marked superseded by Phase 11 on 2026-10-29
    requirement: COMP-03
    verification:
      - kind: other
        ref: git grep for the six retired names outside .planning/phases, .planning/milestones and LESSONS.md printed nothing (14 lines at HEAD before plan 11-05, 8 before this task)
        status: pass
      - kind: other
        ref: grep -cF 'cmake --workflow --preset library' on CONFORMANCE.md and GSD-HANDOFF.md printed 1 each; agent read of docs/emulation-tests.md against the rewritten lines, same command, same variables, same local.csv
        status: pass
    human_judgment: false
  - id: C2
    description: CONFORMANCE section 5 names the judge that exists, says the baseline checkout was not built and points to SEED-014
    requirement: COMP-01
    verification:
      - kind: other
        ref: grep -c SEED-014 .planning/preparation/CONFORMANCE.md printed 1; ls tests/cmake/compat_ratchet.cmake found the judge; agent read the bullet
        status: pass
    human_judgment: false
  - id: C3
    description: The is_experimental flip needs pinned-list evidence from a future milestone in HOSTS-AND-INTEGRATION and SEED-001, with a pointer to SEED-012
    requirement: COMP-01
    verification:
      - kind: other
        ref: grep -q pinned-list on HOSTS-AND-INTEGRATION.md and SEED-001; grep -q SEED-012 on HOSTS-AND-INTEGRATION.md, exit 0
        status: pass
    human_judgment: false
  - id: C4
    description: The four deferred ideas exist as dormant seeds only, each saying it is an optional improvement under AGENTS rule 7, and no shipped file changed
    requirement: COMP-01
    verification:
      - kind: other
        ref: ls .planning/seeds/SEED-01[1-4]-*.md gives 4 files and grep -l '^status: dormant$' gives 4; git diff --stat over both task commits lists only .planning files
        status: pass
    human_judgment: false
  - id: C5
    description: The whole gate and the hygiene lane pass after the planning edits
    requirement: COMP-03
    verification:
      - kind: integration
        ref: cmake --workflow --preset ci (exit 0, 673 tests plus 6 release tests passed)
        status: pass
      - kind: integration
        ref: cmake --workflow --preset hygiene (exit 0, 30 tests passed)
        status: pass
    human_judgment: false
actuals:
  tokens: 4500
  tasks: 2
  commits: 2
plan_head_before: eba5b1c07b0990149f750023e7706464632e8e06
plan_head_after: c4483b8d44cd7d20dfce4468f067cfa6cdc60613
duration: 20m
completed: 2026-10-29
---

# Phase 11 Plan 08: Planning docs and deferred seeds Summary

**The preparation docs now describe the real local library run (an inline `MUHTARI_ROM_DIR=<dump dir> [MUHTARI_BIOS=<file>] cmake --workflow --preset library` and the counted `library.local` pass), the unbuilt baseline checkout is marked, and the is_experimental flip plus three other deferred ideas are dormant seeds.**

## What changed

- `CONFORMANCE.md` section 5: the local-only preset bullet names the inline command, `build/library/local.csv` and that it never enters the ledger; the protected-baseline bullet names the pull request's `tests/cmake/compat_ratchet.cmake` as the judge (the shell script it used to name was never implemented), says the baseline checkout is not built, that the judge comes from the pull request, and points to SEED-014.
- `ENGINEERING.md`: the `library` lane-table row and section 13's first bullet use the inline command; section 13's guard bullet keeps its description of the secret-read guard and ends with "No dotenv file is read or tracked, because the local run takes its variables inline." Rec/Alt/Flip text stays readable, each rewritten line marked superseded by Phase 11 on 2026-10-29.
- `GSD-HANDOFF.md` (owner table row) and `PROMPT-COVERAGE.md` rows 19 and 27: same command, same superseded mark.
- `HOSTS-AND-INTEGRATION.md` and `SEED-001`: the flip needs pinned-list (commercial) evidence a future milestone defines (SEED-012); Phase 11 accounts every pinned entry as not evaluated or skipped.
- New dormant seeds, SEED-010's frontmatter shape: SEED-011 movie-driven device checks on the CC0 samples, SEED-012 the flip criterion, SEED-013 a non-ratcheted locally observed section, SEED-014 push-event and base-commit judge hardening.

## Deviations from Plan

None - plan executed exactly as written. One note: the first draft of the CONFORMANCE bullet named the retired files to explain what was superseded and tripped the plan's own grep gate; the final text describes them by purpose ("dotenv reader, dotenv template, wrapper script or BIOS-folder variable").

## Deferred Issues

- The `diag.bankset_tracer` / `diag.bankset` output-directory race in `deferred-items.md` did not occur in this plan's `cmake --workflow --preset ci` run.

## Commands and exit status (all local, run URL N/A-local)

| Command | Exit |
|---|---|
| Task 1 gate: `test -z "$(git grep -n -e MUHTARI_BIOS_DIR -e 'scripts/library[.]sh' -e 'compat-update[.]sh' -e 'compat-ratchet[.]sh' -e 'env[.]local' -e 'env[.]example' -- . ':!.planning/phases' ':!.planning/milestones' ':!.planning/LESSONS.md')" && grep -q SEED-014 .planning/preparation/CONFORMANCE.md` | 0 |
| Task 2 gate: four seeds, four dormant, pinned-list and SEED-012 in HOSTS-AND-INTEGRATION, pinned-list in SEED-001 | 0 |
| `cmake --workflow --preset ci` | 0, 673 tests plus 6 release tests passed |
| `cmake --workflow --preset hygiene` | 0, 30 tests passed |

## For /gsd-ship

- Neutral pull request title (no release-as override, D-22): `feat: account for the pinned library in the compatibility ledger`.
- Optional owner command from plan 11-05 (not a gate, not UAT, no committed file changes), after the `ci` build has made `build/ci/built`:

```sh
MUHTARI_ROM_DIR=<dump dir> [MUHTARI_BIOS=<file>] cmake --workflow --preset library
ctest --test-dir build/library -R '^library[.]local$' -V | grep -E 'local: [0-9]+ of [0-9]+ present'
```

## Next

This is the last plan of Phase 11. The orchestrator runs phase verification next. This executor ran no `/gsd-` chaining command and pushed nothing.

## Self-Check: PASSED

- Seeds SEED-011 to SEED-014 and this SUMMARY: FOUND
- Commits 767ba49, c4483b8: FOUND on phase/11-full-library-acceptance
