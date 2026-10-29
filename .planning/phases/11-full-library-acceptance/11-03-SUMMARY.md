---
phase: 11-full-library-acceptance
plan: "03"
subsystem: compat-ratchet
tags: [ratchet, ledger, ci-required, diagnostics, cmake]
status: complete
requires:
  - phase: 11-full-library-acceptance
    provides: "Derived softlist ledger rows and the pin recipe (plans 01 and 02)"
provides:
  - "tests/cmake/compat_ratchet.cmake keeps a passing pr row in tier pr on the BASE path, naming a demotion to local"
  - "policy.compat_ratchet.controls: the real ratchet against a fixture git repository (lowered, excepted today, expired yesterday, demoted, unchanged)"
  - "tests/diag/ledger_crc.cmake and diag.ledger_crc.self_test: the four local mailbox rows' evidence equals the CRC their canonical diagnostic produces"
  - "The library job is in CI required's needs and jq key list, with roll-up controls for a failed or wrongly skipped library job"
affects: [11-04, 11-05, 11-06, 11-07, 11-08, compat-ledger, ci]
tech-stack:
  added: []
  patterns:
    - "A control that runs the real check script against a fixture git repository with a fixed SOURCE_DATE_EPOCH, so date edges are exact"
    - "A compare-only script called from existing diag tests at the canonical case, with no new predicate support"
key-files:
  created:
    - tests/cmake/compat_ratchet_controls.cmake
    - tests/diag/ledger_crc.cmake
  modified:
    - tests/cmake/compat_ratchet.cmake
    - tests/policy/tests.cmake
    - tests/diag/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - .github/workflows/ci.yml
    - tests/cmake/ci_required.cmake
    - tests/hygiene/tests.cmake
    - docs/testing.md
    - docs/emulation-tests.md
key-decisions:
  - "Tier is protected only for base rows that were pass and pr; base pass rows of tier local stay protected on status alone."
  - "The controls fix one clock (SOURCE_DATE_EPOCH) for the ratchet and for the dates written into the allow files, so a run across midnight UTC cannot split them."
  - "ledger_crc.cmake reads the ledger as text, protects the ';' in predicates, and splits on commas, because CMake regular expressions have no {n} repeat."
  - "The CRC is compared only at the canonical call per tag (xboard 1 0 0 0 0 2, xm 2 0 0 0 0 2, bankset 32n, the PAL 601-frame run); predicate.c gains no mailbox support."
  - "library is promoted to required on a recount of 18 distinct successful push-to-main runs, above the bar of ten."
requirements-completed: []
coverage:
  - id: C1
    description: A pass/pr row at BASE is still pass and pr unless a valid allow line names it; a lowered row and a pr-to-local demotion are named
    requirement: COMP-02
    verification:
      - kind: unit
        ref: policy.compat_ratchet.controls
        status: pass
      - kind: integration
        ref: MUHTARI_LEDGER_BASE=$(git merge-base HEAD origin/main) MUHTARI_ROM_DIR=build/ci/built cmake --workflow --preset library
        status: pass
    human_judgment: false
  - id: C2
    description: The allow window is inclusive at its edges, expiry today passes and expiry yesterday fails
    requirement: COMP-02
    verification:
      - kind: unit
        ref: policy.compat_ratchet.controls
        status: pass
    human_judgment: false
  - id: C3
    description: Reverting only the tier comparison makes the controls fail naming the demotion case
    requirement: COMP-02
    verification:
      - kind: other
        ref: "ctest --preset ci -R policy.compat_ratchet.controls with the tier branch replaced by elseif(FALSE) (exit 1, \"demotion to local is named: want exit 1, got 0\"), then restored"
        status: pass
    human_judgment: false
  - id: C4
    description: Each local mailbox row's evidence equals the mailbox CRC of its canonical diagnostic run (diag/bankset ee7c, diag/xboard f157, diag/xm 9354, diag/pal 52a5); a stale CRC fails while the diagnostic still passes
    requirement: COMP-02
    verification:
      - kind: integration
        ref: diag.bankset_tracer, diag.bankset, diag.expansion, diag.pal
        status: pass
      - kind: unit
        ref: diag.ledger_crc.self_test
        status: pass
    human_judgment: false
  - id: C5
    description: ledger_crc fails, rather than passes, on a report with no diagnostic line and on an id with no ledger row; only the canonical case per tag is compared; predicate.c has no mailbox@ support
    requirement: COMP-02
    verification:
      - kind: other
        ref: cmake -P tests/diag/ledger_crc.cmake over a report holding no diagnostic line, over diag/none, and over a scratch ledger with diag/xm evidence 0000 (exit 1, each named); grep -c 'mailbox@' tests/compat/predicate.c is 0
        status: pass
    human_judgment: false
  - id: C6
    description: The library job is in CI required's needs and jq key list; a missing, failed or wrongly skipped library job fails the roll-up controls; only wasm32 stays report-only
    requirement: COMP-02
    verification:
      - kind: unit
        ref: policy.ci_required
        status: pass
      - kind: unit
        ref: policy.ci_required.self_test
        status: pass
    human_judgment: false
  - id: C7
    description: The docs say local rows are not judged by the library preset and the library promotion cites a recounted run count, within the docs budget
    requirement: COMP-02
    verification:
      - kind: unit
        ref: docs.budgets
        status: pass
    human_judgment: false
  - id: C8
    description: The whole regression gate and the hygiene workflow pass on the plan head
    requirement: COMP-02
    verification:
      - kind: integration
        ref: cmake --workflow --preset ci
        status: pass
      - kind: integration
        ref: cmake --workflow --preset hygiene
        status: pass
    human_judgment: false
actuals:
  tokens: 7800
  tasks: 3
  commits: 3
plan_head_before: 04428fabb8bff596607afc3687085a909e1eb1c6
plan_head_after: 239ea56842b644facd04a5c26bd3a15c4ee2f313
duration: 30m
completed: 2026-10-29
---

# Phase 11 Plan 03: A pass stays a pass Summary

**The ratchet now names a passing pr row demoted to local, proven against a real fixture commit; the four local mailbox CRCs are compared with the diagnostics that produce them; and the library job is part of CI required.**

## What changed

- `tests/cmake/compat_ratchet.cmake`: records each current row's tier; in the BASE loop a pass row of tier pr that no valid allow line names must be pass and pr, else `<id> <predicate>: pr at <BASE>, now '<tier>' with no allow line`. Local base rows keep the status-only protection.
- `tests/cmake/compat_ratchet_controls.cmake` (new) and `policy.compat_ratchet.controls` (label `policy`, `ci` and `wasm32` inventories): builds a fixture repository as `fixture` / `fixture@users.noreply.github.com`, commits two pr pass rows as the base and runs the real ratchet through five cases: lowered, excepted today, expired yesterday, demoted to local, unchanged. Results equal the edited ledger, so only the base comparison can fail.
- `tests/diag/ledger_crc.cmake` (new): swaps the two mailbox CRC bytes (hex characters 16 to 19 of the `diagnostic` line) and compares them with the row's evidence; fails on a mismatch, a missing diagnostic line, a short line or a missing row. `tests/diag/tests.cmake` calls it at the canonical case of each tag; `diag.ledger_crc.self_test` expects `diag/pal: ledger evidence 52a5, mailbox CRC 0000` from an all-zero mailbox.
- `.github/workflows/ci.yml`, `tests/cmake/ci_required.cmake`, `tests/hygiene/tests.cmake`: `library` joins `needs` and the jq keys; roll-up cases `library failed` and `library skipped when heavy=true` plus `library=skipped` in the light set; the three `REPORT_ONLY` sites now name only `wasm32`; the omission loop covers `library`.
- `docs/testing.md`, `docs/emulation-tests.md`: only `wasm32` stays advisory; `library` is required (18, 2026-10-29); local rows are not judged by the library preset and the four `mailbox@` rows are run by `diag.bankset`, `diag.expansion` and `diag.pal`; the ratchet bullet says a base `pr` row stays `pr`.

## Library recount (rule 10)

`gh run list --workflow ci.yml --branch main --event push --limit 100 --json databaseId,headSha,createdAt` (exit 0) returned 24 runs. For each, `gh api repos/szTheory/muhtari/actions/runs/<id>/jobs --paginate --jq '.jobs[] | select(.name == "library") | .conclusion'` (exit 0) gave 18 `success` on 18 distinct head SHAs, 2 `failure` and 4 runs with no library job. The bar of ten is met. Counted run IDs:

40854285272, 40777253646, 40753756094, 40733547062, 40249027703, 40203183179, 40201154961, 40114033103, 40112001036, 40043172203, 40040634219, 39890940026, 39890107162, 39617761785, 39533130206, 39532369201, 39470886147, 39470341335

The `ci.yml` comment cites 18, 2026-10-29 and the newest run 40854285272.

## Commands and exit status (all local, run URL N/A-local)

| Command | Exit |
|---|---|
| `ctest --preset ci -R '^policy[.]compat_ratchet[.]controls$'` (five cases listed) | 0 |
| same, with only the tier branch of `compat_ratchet.cmake` replaced by `elseif(FALSE)`, then restored | 1 (demotion case named) |
| `ctest --preset ci -L '^built$'` | 0 |
| `MUHTARI_LEDGER_BASE=$(git merge-base HEAD origin/main) MUHTARI_ROM_DIR=build/ci/built cmake --workflow --preset library` (46 tests) | 0 |
| `ctest --preset ci -R '^diag[.](bankset\|expansion\|pal)$'` before wiring | 0 |
| `cmake -DLEDGER=tests/compat/ledger.csv -DID=diag/bankset -DREPORT=build/ci/tests/diag/bankset/32n/report.txt -P tests/diag/ledger_crc.cmake` (ee7c) | 0 |
| same for diag/xboard on `expansion/1-0-0-0-0-2/report.txt` (f157), diag/xm on `expansion/2-0-0-0-0-2/report.txt` (9354), diag/pal on `pal/runner.txt` (52a5) | 0 each |
| ledger_crc over a scratch ledger with diag/xm evidence `0000`; over a report with no diagnostic line; over id `diag/none` | 1 each, each named |
| `grep -c 'mailbox@' tests/compat/predicate.c` | 0 matches (prints 0) |
| `ctest --preset ci -R '^diag[.](ledger_crc[.]self_test\|bankset_tracer\|bankset\|expansion\|pal)$'` and `docs[.]budgets` (6 tests, four `ledger_crc:` pass lines) | 0 |
| `ctest --preset hygiene -R '^(policy[.]ci_required\|policy[.]ci_required[.]self_test\|hygiene[.]timeout_comments)$'` | 0 |
| `cmake --workflow --preset ci` (first three runs, host load average 20 to 68) | 8, `title.pal.bios` timed out (see Deferred Issues) |
| `ctest --preset ci -j 3` (675 tests) | 0 |
| `cmake --workflow --preset ci` (load average 19) | 0 |
| `cmake --workflow --preset hygiene` (first run, `hygiene.tree` timed out under load) | 8 |
| `cmake --workflow --preset hygiene` (rerun) | 0 |

Hosted proof that CI required now waits on `library` is confirmed at ship from the pull request's checks.

## Docs budget

`docs/emulation-tests.md` ended at 40,933 of 40,960 bytes and `docs/testing.md` at 40,957. Trimmed in `## ledger`: the `library.compat_doc` bullet and one sentence of `In CI` ("reports only until 10 green runs", replaced). Trimmed in `## fuzz`: the run-target seed paragraph (joined wrapped lines, dropped "so a linear payload remains headerless", lowercased a clause), the boot-to-play audio paragraph, "itself", "also", and the regression reconfigure sentence. Trimmed in testing.md `## CI`: "more than" to "over", "job" in "synthetic job results", a comma, and the library count phrased as "(18, 2026-10-29)". No behaviour claim was dropped.

## Deviations from Plan

**1. [Rule 1 - Bug] The first fixture results file was read with its first row as the header.** The ratchet drops the first line of a results file, so the first control run failed for the wrong reason; the file now carries the header `id,predicate,status,evidence`.

**2. [Rule 3 - Blocking] `ledger_crc.cmake` splits on commas instead of using a field-count regular expression.** CMake regular expressions have no `{n}` repeat, so the row match found nothing; the script uses the ratchet's `<semicolon>` and comma split.

**3. [Docs] The ratchet bullet in `docs/emulation-tests.md` does not name `policy.compat_ratchet.controls`.** The byte budget left no room, and the test is documented by its own header.

Otherwise the plan executed as written; no ledger evidence, diagnostic image or CRC extraction was changed.

## Deferred Issues

`title.pal.bios` (2 s provisional timeout, "not yet measured in CI") and `hygiene.tree` exceeded their timeouts on a host whose load average was 20 to 68 from unrelated processes; both pass alone (0.8 s) and the full suites pass once the load dropped. The timeouts predate this plan; they are not changed here.

## Known Stubs

None.

## Threat Flags

None. The plan adds a check script, a compare-only script and a roll-up dependency; it adds no endpoint, auth path or file access outside the repository and build tree.

## Next

Plan 11-04 is the next separate allowed command (`/gsd-execute-phase 11`, one plan per step). COMP-02 completes when plans 11-01, 11-03, 11-06 and 11-07 are all done; plan 11-07 closes it.

## Self-Check: PASSED

- `tests/cmake/compat_ratchet_controls.cmake`, `tests/diag/ledger_crc.cmake`: FOUND
- commits b0bfe1b, ed4e771, 239ea56: FOUND on HEAD
