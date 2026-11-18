---
phase: 27-headless-developer-trace
plan: "06"
subsystem: ci-maintenance
tags:
  - trace-cost
  - linux-perf
  - ci-fixtures
requires:
  - phase: 27-headless-developer-trace
    provides:
      - Existing trace-cost workflow, generated CPU workload, and accepted local fixture prerequisites.
provides:
  - Linux perf job provisions its four existing pinned configure inputs.
  - Source-qualified Linux hooks-off cost receipt for TRACE-06.
affects:
  - Phase 27 verification and closeout
actuals:
  tokens: 6000
  tasks: 3
  commits: 3
tech-stack:
  added: []
  patterns:
    - Reuse pinned CI inputs and bind performance receipts to exact source and merge revisions.
key-files:
  created:
    - .planning/phases/27-headless-developer-trace/27-06-SUMMARY.md
  modified:
    - .github/workflows/ci.yml
    - .planning/phases/27-headless-developer-trace/27-06-CHECKPOINT.md
    - .planning/phases/27-headless-developer-trace/27-CI-EVIDENCE.md
    - .planning/phases/27-headless-developer-trace/27-TRACE-ACCEPTANCE.md
key-decisions:
  - Accept TRACE-06 only from the authentic Linux perf artifact tied to the authorized PR head, base, and merge checkout.
  - Keep the one-run CI timing evidence as a baseline; it does not support timeout, cache, or topology changes.
patterns-established:
  - Record a performance result together with its producer, source revision, workload digest, sample stability, and threshold calculation.
requirements-completed:
  - TRACE-06
coverage:
  - id: TRACE-06
    description: The hooks-off instruction-cost gate passes on an authentic Linux Cachegrind receipt.
    requirement: TRACE-06
    verification:
      kind: integration
      ref: .planning/phases/27-headless-developer-trace/27-CI-EVIDENCE.md#linux-perf-receipt
      status: pass
    human_judgment: false
duration: 80min
completed: 2026-11-17
status: complete
---

# Phase 27 Plan 06: Linux trace-cost receipt Summary

**The Linux perf job now configures from its pinned inputs, and its source-qualified hooks-off cost receipt passes.**

## Performance

- **Duration:** 80 minutes
- **Started:** 2026-11-17T15:45:50Z
- **Completed:** 2026-11-17T17:05:41Z
- **Tasks:** 3
- **Files modified:** 5

## Accomplishments

- Added the four established pinned-input fetches before the perf job's tests-on configure, preserving its five-minute cap and measurement contract.
- The fresh native Release configure, target build, generated CPU fixture, diagnostic marker check, and hygiene workflow passed locally. The native smoke check established fixture reachability only.
- After the plan's owner checkpoint, a separate bounded authorization enabled one push and observation of automatic checks. The Linux perf artifact reports stable samples and a raw hooks-off delta of 900 against the 9,600 limit (3 Ir/frame), satisfying TRACE-06 for that exact source.
- Recorded the receipt, source provenance, workflow outcomes, and acceptance update in the phase evidence artifacts.

## Task Commits

1. **Task 1: Provision the perf fixture's existing pinned configure inputs** — `4630288` (`ci(27-06): provision perf fixture pins`)
2. **Task 2: Review the local remedy and record its bounded evidence** — `8fa493b` (`docs(27-06): record local perf pin remedy`)
3. **Task 3: Stop at the new hosted-evidence boundary** — the checkpoint was recorded; its later, separately authorized receipt update is `a1748cb` (`docs(27-06): record authorized trace receipt`).

The Phase 27 state checkpoint was recorded in `18204ba` as orchestrator bookkeeping.

## Local Checks

| Command | Exit | Result |
|---|---:|---|
| `git diff --check` | 0 | Changed evidence and acceptance documents have no whitespace errors. |
| Fresh `cmake --preset ci` Release configure with tests and tracing enabled | 0 | Existing native fixture prerequisites configured successfully. |
| Build `muhtari-run` and `muhtari-test-trace` | 0 | Both requested targets built. |
| Generated `trace-cost.a78` and ran one diagnostic frame | 0 | The runner produced the expected `diagnostic a5` marker. |
| `cmake --workflow --preset hygiene` | 0 | Existing pin, inventory, artifact, and tree checks passed. |
| Prior-phase regression gate | N/A | Phase 26 is documentation-only and identifies no prior-phase test files. |

## Hosted Receipt

- Linux perf job from [CI run 44123425194](https://github.com/szTheory/muhtari/actions/runs/44123425194) produced artifact `quality-perf` for authorized PR 77 head `4630288…`, base `89b6ff8…`, and merge checkout `fa72f56…`.
- The artifact's workload digest matched the producer; all four repeated pairs were stable. The validation command passed, and the measured raw delta was 900 against 9,600.
- CI and Release workflows still failed in unrelated required Linux/BE32 compiler lanes (`mismatched-dealloc`) and ASan/Nightly Clang lanes (`realpath` declaration); quality roll-ups consequently failed. No release was published, and no further push, dispatch, rerun, merge, or release was authorized.
- Queue time was negligible. The longest required job time was driven by the quality-observation step; this single run provides no basis for structural workflow changes.

## Deviations from Plan

The plan stopped at its documented owner checkpoint. The owner then separately authorized exactly one push and a bounded observation of its automatic checks. That continuation supplied the required authentic receipt; it did not authorize another remote action.

## Next Phase Readiness

TRACE-06's measurement gap is closed for the recorded PR source. The normal phase verifier still needs to refresh the phase verdict. The full CI and Release workflows remain red on the recorded unrelated build failures, so shipping is not ready.

---
*Phase: 27-headless-developer-trace*
*Completed: 2026-11-17*
