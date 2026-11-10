---
phase: 05-pipeline-tune-up
scope: 05-06 measured allocation halt only
kind: planner-extracted-gap-input
verifier_ran: false
status: recovery_planned
source_summary: 05-06-SUMMARY.md
source_handoff: 05-06-HANDOFF.md
replacement_plan: 05-11
requirements: [TUNE-03, TUNE-04]
gaps:
  - truth: The central performance and compatibility policy migration fits its existing allocation while retaining all semantic controls.
    status: failed
    reason: The retained four-file draft observes 14888 physical lines and quality967, exceeding C14853 and quality920; no GREEN or completed task exists.
    artifacts:
      - path: quality.json
        issue: Uncommitted proposed performance domain declaration contributes eight lines.
      - path: tests/cmake/quality.cmake
        issue: Retained reader and reason extraction contributes fifty net lines; complete domain invariants and trusted callers remain unproved.
      - path: tests/cmake/perf_compare.cmake
        issue: Passing uncommitted migration removes twenty-seven lines but production caller acceptance is unrun.
      - path: tests/perf/tests.cmake
        issue: RED is committed; retained draft contributes sixteen net lines relative to the accepted starting tree.
    missing:
      - Measured fault-preserving simplification sufficient to restore quality920 before affected expansion, with no cap or allocation change.
      - Complete compatibility policy migration, exact current-result equality and passing-base retention controls, with the spent POKEY migration retired.
      - Trusted production policy inputs, both old stores and their inventory entries retired, and current local plus hosted acceptance.
---

# Narrow planner extraction for the measured allocation halt

This is planning input, not a verifier report. `verifier_ran: false` is deliberate.
It makes no phase-wide verification claim and does not supersede historical evidence.
The historical `05-VERIFICATION.md` concerns the old 05-03 recovery accepted through
05-10; its two resolved gaps are not reopened. The halted 05-06 SUMMARY and HANDOFF
remain immutable history. Completed 05-01,05-02,05-10,05-04 and05-05 remain accepted.

## New construction evidence

The preceding execution committed only RED `df6b095`; 05-06 has zero of three tasks
complete. Its retained draft is not a GREEN implementation. All four dirty paths were
read without modification during this planning step. The fresh read-only status check
still lists only those four implementation paths as modified.

| Command | Exit | Observation and limit |
|---|---:|---|
| `git status --short` | 0 | Retained modifications in quality.json, quality.cmake, perf_compare.cmake and tests/perf/tests.cmake; no recovery implementation was written. |
| `git ls-files quality.json tests/cmake/quality.cmake tests/cmake/quality_join.cmake tests/cmake/perf_compare.cmake tests/perf/tests.cmake tests/cmake/compat_ratchet.cmake tests/compat/library.cmake tests/compat/allow.txt tests/perf/allow.txt tests/policy/quality.cmake tests/policy/gate-lines.txt .github/workflows/ci.yml` | 0 | Recovery implementation targets are existing tracked paths. |
| `cmake -DROOT=. -DINVENTORY=tests/policy/gate-lines.txt -DOUT=build/plan-evidence/recovery-planner-count.json -P tests/cmake/gate_count.cmake` | 0 | Observes14888, full/authored source8580/6533; exit0 means the counter ran, not that the budget passes. |
| `cmake -DROOT=. -DINVENTORY=tests/policy/gate-lines.txt -DOUT=build/plan-evidence/recovery-count.json -P tests/cmake/gate_count.cmake && jq -e '.gate_lines <= 14841 and .gate_lines <= 14853' build/plan-evidence/recovery-count.json` | 1 | The replacement's exact gate reports14888 then false, proving its current fail-first numeric baseline. |
| `cmake -DROOT=. -DINVENTORY=tests/policy/gate-lines.txt -DOUT=build/plan-evidence/recovery-planner-budget-baseline.json -P tests/cmake/gate_count.cmake && jq -e '.gate_lines <= 14821' build/plan-evidence/recovery-planner-budget-baseline.json` | 1 | Reports14888 then false. This stronger prospective saving target was explored and is not claimed achieved or imposed by the replacement plan. |

No hosted write, push, dispatch, full ci/library workflow or new semantic acceptance
ran in this planning step. Current recovery run URL: unavailable. Prior05-05 green URLs
remain prior evidence and cannot accept this draft. The halt record supplies the
already-run eight-test exit0 and the intentional RED exit8; they are not rerun or
relabeled as fresh phase verification here.

Frozen B12885/S2/N1970/C14853 and count368/BE205/digest447/quality920/fixes30/reserve0
remain unchanged. Accepted pre-draft total14841 includes quality920 and BE193/205.
Consequently, if all non-quality components remain unchanged, total minus13921 is
the quality incremental use: draft14888 minus13921 equals967. A replacement must
reach total14841 or lower as well as C14853; the remaining BE12 is not reserve.
No forecast beyond the already measured four-line draft reduction is credited.

## Why this recovery is permitted

The new evidence is the measured construction overrun, first observed during05-06.
AGENTS rule7 and D-20 permit a bounded new-evidence recovery; they do not authorize a
renewed broad research loop, another automatic gap allowance, a cap increase or a
scope reduction. Existing research is reused. Replacement05-11 tests a specific
consolidation of duplicated policy-pair and observation validation, then completes
the original migration only while actual counts stay within the fixed allocation.
If that construction misses the gate, retain its exact deltas and stop affected
expansion; no further recovery is presumed approved.

## Unresolved assumptions retained

The supplied spec-less edge report contains seven unresolved rows, four unclassified.
TUNE-01 unclassified stays with04/09; TUNE-02 adjacency/empty/ordering stay with02/10/09;
TUNE-03 unclassified stays with01/05/11/09; TUNE-04 unclassified stays with07/11/09;
TUNE-05 unclassified stays with08/09. These are seven flagged assumptions, not seven
new requirements or resolved classifications. The descriptorless TUNE-03 physical
boundary prohibition remains flagged-unverified. No wired descriptor or owner chore
is invented. The active assumption-delta contribution was read verbatim; its scan
returned `detected:false`, exit0. No identity-model checkpoint fires.
