# Phase 5: Pipeline tune-up — Research

**Date:** 2026-10-21
**Status:** BLOCKED — D-17 feasibility not established
**Confidence:** HIGH for measured local counts and hosted run status; LOW for achievable rewrite size.

## Planning disposition

Research found useful simplifications, but did not establish a defensible path to the
existing tenth-of-src limit while preserving the required faults and adding the
digest and quality ratchet. D-17 explicitly requires that path during formal
planning. Do not mark this phase ready to execute or turn an unproven allocation
into a completion criterion that merely postpones this question.

This is an unresolved feasibility result, not proof that the limit is impossible.
The independent researcher supplied findings through interim messages but did not
complete a durable report before interruption. This file preserves the confirmed
findings and separates measurements from candidate approaches. No planner or
plan checker has run. No implementation, control dispatch, issue publication,
settings mutation, or lane promotion was performed.

## Scope and authoritative inputs

TUNE-01–05 and D-01–18 remain unchanged. Read `05-CONTEXT.md` in full before
continuing, together with `AGENTS.md`, `.planning/REQUIREMENTS.md`,
`.planning/preparation/README.md`, `ENGINEERING.md`, `DECISIONS.md`,
`GSD-HANDOFF.md`, and `.planning/seeds/SEED-007-milestone-tune-up.md`.
The context contains the delegated decisions and canonical implementation paths.

The stack remains CMake `-P`, POSIX sh, existing `gh` and `jq`; no dependency or
service addition is justified. [VERIFIED: project context and config]

## Verified budget baseline

Physical lines, including comments and blank lines, at the unchanged planning head:

| Inventory | Lines | Interpretation |
|---|---:|---|
| `src/*.c` and `src/*.h` | 8,577 | Settled full denominator |
| Same, excluding `src/cartdb.c` | 6,530 | Required authored-only sensitivity |
| `tests/cmake/*.cmake` | 2,053 | Whole-directory broad sensitivity |
| `scripts/*.sh` | 631 | All six shell scripts |
| `tools/compat_doc.cmake` | 80 | Existing compatibility generator |
| Broad subtotal | 2,764 | Before embedded workflow/configure checks |
| Whole-line cap using full denominator | 857 | A tenth, rounded down |

The historical narrow 1,367-line figure in SEED-007 omits enforcement that D-17
requires counted. Even the broad subtotal needs a 1,907-line reduction before
adding new checks. This subtotal is a sensitivity inventory, not a final audited
classification of every executable line. Embedded `.github/workflows/*.yml`
logic and configure-time enforcement must also be inventoried; moving checks
there does not make them free. [VERIFIED: local file counts; D-17]

Reproduction: `wc -l src/*.c src/*.h` — exit 0; directory totals were also
recomputed with a one-off Python `Path.read_text().splitlines()` count — exit 0.
`wc -l tests/cmake/*.cmake scripts/*.sh tools/compat_doc.cmake` — exit 0
reproduces the 2,764-line subtotal. No counting script or log is committed.

## Candidate simplifications and fault boundaries

These are research candidates, not a validated allocation or permission to remove
checks. Each needs a conservative resulting-size estimate and behavioral proof.

| Candidate | Fault that must remain covered | Constraint |
|---|---|---|
| Share archive extraction for archive contents, release floor and firmware checks | Wrong members, unsupported binaries, forbidden firmware | Preserve independent pre-publication validation |
| Share exception/date parsing for performance and compatibility policies | Unexplained regression, expired or unrelated waiver | Keep each semantic comparator and exact threshold arithmetic |
| Consolidate no-skip checking with inventory registration and JUnit checks | Missing, skipped or unexecuted tests | Preserve exact executed inventory and negative controls |
| Replace duplicated pin shape inspection with fetch-path validation | Wrong or corrupted downloaded input | Fetch integrity must remain fail-closed |
| Remove redundant workflow shape checks where an executed predicate is stronger | Required jobs bypassed or silently advisory | Name the stronger remaining signal for each deletion |

Do not merge distinct predicates merely because their names overlap: public ABI,
source floating-point restrictions and no-FP builds detect different faults.
The hygiene script already shares its staged/tree scanner; repository settings
already reuse desired merge/label definitions. Their entire current lengths
cannot be claimed as duplicate savings. [VERIFIED: researcher code inspection;
parent inspection of `scripts/hygiene.sh` and `tests/cmake/floor.cmake`]

The researcher's aggressive allocation estimate remained around 1,600 lines
including new reporting/quality work. That is an unimplemented estimate, not a
lower bound or a measured result. It supplies no valid <=857 allocation.

## Reproduced reliability and hosted evidence

The compatibility freshness check and its self-test share the output selected by
`policy_compat_cmd` in `tests/policy/tests.cmake:80–97`. The context records a
reproduced race. Give each writer its own output and retain the stale-page
negative control; then exercise the affected checks together under parallel and
random scheduling. [VERIFIED: registration code; CITED: D-05]

`gh run view 39617761785 -R szTheory/muhtari --json status,conclusion,url,headSha,jobs`
— exit 0: main commit `752916f87e9ed8f42cfbc4b88e76089b74c7509e` completed with
failure. macOS x64 and Windows x64 build/test jobs failed; BE completed and WASM
was skipped. [Run](https://github.com/szTheory/muhtari/actions/runs/39617761785).
This supersedes the discussion's then-incomplete status. [VERIFIED: hosted API]

The researcher read failure logs and reported `basic.pokey` timing out at 10.10 s
on macOS x64 and `runner.movie.replay` at 0.71 s on Windows x64. It also reported
`audio.samples` passing at 124.92 s and 66.52 s respectively. These log findings
were not independently re-read by the parent; re-fetch the named run's logs
before using exact timings to establish bounds. A follow-up log download did
not complete during the parent check. [CITED: researcher interim findings]

Preserve D-06's comparable 40-success window, twice-maximum rule, provisional
small-sample marking, and workload/toolchain identities. A timeout is censored
evidence; no fixed increase alone proves reliability. D-07's required BE
cross-compilation and nightly full same-commit native/BE equality remain locked.
Do not promote WASM/library/property lanes without the lane-specific completed
success history required by D-13. [CITED: phase context]

## Remaining implementation contracts

The completed research did not resolve these into executable task allocations:

- D-01–04: one serialized digest publisher, safe preview, complete pagination and
  attempts, retained monthly section, honest missing-data/queue semantics.
- D-09–11: one quality policy with trusted-base comparison, measured effective
  bounds, bounded specific exceptions, required evidence and negative controls.
- D-12–13: fresh performance controls, numeric PR base, failure/recovery evidence,
  honest fixture scope and conditional lane promotion.
- D-14–16: evidence-linked six-dimension baseline, measured CI improvement,
  conservative release rehearsal reuse decision, current docs/install/settings.
- D-17–18: complete enforcement inventory, retained-fault mapping and feasible
  readable size allocation before adding enforcement; fixed scope/review budgets.

## Resume condition

Resume ordinary planning only with new evidence supporting a fault-preserving
<=857-line allocation, or an explicit owner revision of the conflicting contract.
A bounded scratch feasibility experiment is the smallest research approach to
replace estimates with evidence; it has not run and is not an additional GSD step
authorized by this invocation. No budget relaxation or feature deferral is implied.

Re-run `$gsd-plan-phase 5 --research` after the feasibility evidence or scope
decision is available. Do not reuse this blocked report as completed research.
