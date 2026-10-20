---
id: SEED-007
status: dormant
planted: 2026-10-15
planted_during: preparation, before milestone v1.0
trigger_when: every milestone after the first, as its first phase
scope: small
---

# SEED-007: Each milestone starts with a faster, more trustworthy pipeline and docs that match the release

## Why This Matters

CI time, flakiness, documentation and quality drift a little with every phase. One short phase at the start of
each milestone removes what built up and raises the bar the rest of the milestone must hold. This is where the
owner's recurring CI-efficiency audit and "weakest quality dimension" review happen, without being asked.

## When to Surface

**Trigger:** every milestone after the first. Keep this seed after it is used; it applies again next time.

## Scope Estimate

**Small** — one phase whose results are merged changes:
- CI efficiency, from the monthly report: slowest job faster; timeouts re-derived (twice the slowest of the last
  40 runs, run ID beside it); any flaky test fixed or removed with the stronger signal named; cores used; caching
  decided by measurement; lowest-value tests removed only when a stronger test covers them.
- Weakest dimension: score performance, CI/CD, ease of use, architecture, docs/README and upgrade path 1–5 with
  links; the lowest one gets this phase's main change.
- Ratchet: `quality.json` ceilings tightened to measured values; floors raised.
- Freshness: docs whose code changed, size budgets, the README install line, `scripts/apply-repo-settings.sh
  --check`, Dependabot majors, runner-image changes, pins of fetched suites.
- Direction: refresh `.planning/PRODUCT-ROADMAP.md` and `.planning/preparation/PROMPT-COVERAGE.md`; export lessons.
- v2.0's tune-up first builds the weekly triage digest, the monthly CI-efficiency report and `quality.json`.

## Breadcrumbs

- `.planning/preparation/ENGINEERING.md` sections 4, 7, 8, 11 and 12: CI design, triage, the ratchet, docs budgets,
  recurring loops.
- `.planning/preparation/DECISIONS.md`: the open questions at its end.

## Notes

The seeds are this project's rolling roadmap: the current milestone is `.planning/ROADMAP.md`; direction beyond it
is `.planning/PRODUCT-ROADMAP.md`; SEED-001 to SEED-006 hold each horizon's narrative.

## Gate-code budget after phase 3 (2026-10-20)

AGENTS rule 7: repository-check code stays under a tenth of `src/` lines from phase 3. Phase 3 does not meet it,
and the gap is an owner scoping item for `/gsd-ship 3`. It does not block the phase.

**Definition (the narrowest).** The `tests/cmake` scripts that hygiene and policy tests register (api_names,
budgets, ci_required, dependabot, firmware_scan, no_float, no_skip, notices, pins, rom_hash_coverage,
single_version, timeouts), `scripts/{hygiene,check-title,classify-changes,apply-repo-settings}.sh`, and this phase's
`tests/cmake/compat_ratchet.cmake` and `tools/compat_doc.cmake`. Test drivers, `expect_fail.cmake` and generators
are not counted.

**Commands (exit 0).** `cat src/*.c src/*.h | wc -l`; `cat $(ls src/*.c src/*.h | grep -v 'src/cartdb.c$') | wc -l`;
`cat <the 18 files above> | wc -l`; for the broad count, `cat tests/cmake/*.cmake scripts/*.sh tools/compat_doc.cmake | wc -l`.

| Count | Before 03-19 | After 03-19 |
|---|---|---|
| `src/*.c src/*.h` | 7,415 | 7,415 |
| without the generated `src/cartdb.c` | 5,368 | 5,368 |
| narrow gate code | 1,369 | 1,367 |
| broad (`tests/cmake`, `scripts`, `compat_doc`) | 2,772 | 2,739 |

Narrow gate code is 18.4 % of `src/`, or 25.5 % without `src/cartdb.c`. The tenth is 741 lines, or 536, so the
shortfall is 626 lines, or 831. Phase 3 added only the ledger ratchet (100 lines) and the doc generator (80 lines).

**Trimmed in 03-19, with no named fault lost.** `perf_compare.cmake` went from 126 to 95 lines. It reads today from
`string(TIMESTAMP)` with `SOURCE_DATE_EPOCH`, as the ratchet does, in place of its own day arithmetic and
`-DTODAY`. Its seven `perf.gate.*` tests still pass. `firmware_scan.cmake` went from 225 to 223 lines, dropping a
guard that `find_program` already makes. `apply-repo-settings.sh` is unchanged: it is an admin tool no CI job runs,
and any change could only be checked against the live repository.

**Cuts that need the owner's call:**
1. Whether generated data (`src/cartdb.c`, 2,047 lines) counts in the denominator.
2. Whether `scripts/apply-repo-settings.sh` (152 lines) is repository-check code. Its `--check` half (about 60
   lines) feeds the tune-up's drift loop, and no CI job runs it.
3. `tests/cmake/firmware_scan.cmake` (223 lines): its release-zip scan is the only check that keeps firmware out of a
   published zip. Its tracked-tree mode overlaps `policy.rom_hash_coverage` and the hygiene tree scan, but it also
   catches known ROMs by their cartdb hashes.
4. Restate the rule as a ratio of the narrow count that does not grow (about 18 %), or a fixed line cap, in place of
   the tenth.
