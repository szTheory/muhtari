---
phase: 12-player-ready-delivery-and-playstead-bridge
plan: "23"
subsystem: testing
tags: [libretro, input-trace, cleanup]
requires:
  - phase: 12-22
    provides: passive diagnostic launcher and paired input-trace controls
provides:
  - Ownership-aware rollback of partial trace files after refused passive preflight
affects: [phase-12]
actuals:
  tokens: 5000
  tasks: 2
  commits: 4
tech-stack:
  added: []
  patterns:
    - "Anchor rollback to the opened directory and unlink only entries matching the invocation-owned file identity."
key-files:
  created:
    - .planning/phases/12-player-ready-delivery-and-playstead-bridge/12-23-LOCAL-EVIDENCE.md
    - .planning/phases/12-player-ready-delivery-and-playstead-bridge/12-23-SUMMARY.md
  modified:
    - tests/libretro/input_trace_host.c
    - tests/libretro/input_trace.cmake
    - .planning/phases/12-player-ready-delivery-and-playstead-bridge/12-REVIEW-DISPOSITION.md
key-decisions:
  - "Keep the existing five-second test deadline and complete inventory; cap local CTest parallelism at two after repeated default-parallel timeout-only failures."
requirements-completed: []
coverage:
  - id: D1
    description: "A refused passive preflight removes only its newly created partial trace, preserves existing files and links, starts no child, and permits a complete paired-trace retry in the same directory."
    verification:
      - kind: integration
        ref: "policy.retroarch.preflight_api"
        status: pass
      - kind: integration
        ref: "ctest --preset ci --parallel 2"
        status: pass
    human_judgment: false
duration: 43min
completed: 2026-11-09
status: complete
---

# Phase 12 Plan 23: Passive Trace Rollback Summary

**The passive launcher now removes only its owned partial trace files when preflight refuses setup, while preserving foreign traces and successful captures.**

## Performance

- **Duration:** 43 min
- **Started:** 2026-11-09T19:33:07Z
- **Completed:** 2026-11-09T20:16:34Z
- **Tasks:** 2
- **Files modified:** 5

## Accomplishments

- Added ownership-aware rollback using the existing directory descriptor, exclusive opens, and device/inode identity checks.
- Strengthened the existing preflight consumer to cover existing helper/core files, a symlink obstruction, zero-child refusal, and successful same-directory retry with complete paired traces.
- Fixed review warning WR-01 and recorded source-bound local evidence.

## Task Commits

1. **Task 1 regression control:** 44ebfef57b066d5e9564112c84032bddf92fc0fd
2. **Task 1 ownership fix:** 28b2e60e78153519887ba06b4b896f7cc8d55e90

## Files Created/Modified

- tests/libretro/input_trace_host.c — ownership-aware trace setup and rollback.
- tests/libretro/input_trace.cmake — assertions for refusal cleanup and synthetic retry.
- 12-REVIEW-DISPOSITION.md — marks only WR-01 fixed; WR-02 and IN-01 retain their prior dispositions.
- 12-23-LOCAL-EVIDENCE.md — command, exit, source, review, and preservation evidence.

## Gate Results

- The strengthened old-source consumer failed on the expected leaked-helper assertion; the repaired focused consumer passed 1/1.
- Four default-parallel CI workflow attempts reached the ordinary test suite but exited 8 on timeout-only failures. With the same preset and unchanged test deadlines/inventory, the complete ordinary suite passed 700/700 using two CTest workers.
- CPack generated all three library, libretro, and runner packages. The six release checks passed.
- The hygiene workflow passed all 30 checks.
- The evidence ledger records each command and result. The frozen verification report checksum passed, and only plan-owned documentation paths were staged for the metadata commit.

## Decisions Made

Kept all existing deadlines, labels, test registrations, inventories, and independent controls. Used explicit local CTest parallelism of two after repeated default-parallel runs timed out only under the full workload; no project CI or test policy was changed.

## Issues Encountered

The default all-worker CMake workflow repeatedly hit five-second test deadlines under full-suite contention. The failed subsets passed serially, and the complete unchanged ordinary inventory passed with two workers. No test was skipped or weakened.

## Next Phase Readiness

Plan 12-23 is complete. Phase 12 remains blocked at accepted 0/4 with 2/5 roadmap truths verified. Release identity, complete released-core RetroArch evidence, and a measured pipeline improvement remain open. Plan 12-24 and the original acceptance successors are still blocked by their dependencies. No host or remote action is authorized.

## Code Review Follow-up — 2026-11-09

The refreshed standard review found and the local fix round corrected two lifecycle faults in the offline passive launcher: a child reaped at the wall deadline could be reported as successful, and a failed cleanup-clock read could skip child termination. Added deterministic synthetic controls for a successful late exit and a cleanup-clock failure. The existing five-second test timeout and the full test inventory remain unchanged. RED/GREEN commands, full CI result and scope limits are recorded in [12-23-LOCAL-EVIDENCE.md](12-23-LOCAL-EVIDENCE.md).

Verification: `cmake --workflow --preset ci` exited 0 (700/700 tests; three packages; six release checks). This only verifies local build/test/release controls; Phase 12 acceptance remains blocked at 0/4.

The final standard re-review found no remaining issues. A post-create `fstat` failure now rolls back helper/core traces in the already-validated owner-only directory. Deterministic controls keep `umask(0077)` through the same-directory retry and assert both paired trace files are owner-only regular files with mode `0600`. The focused preflight test passed 1/1, and the final `cmake --workflow --preset ci` run passed all 700 tests, generated three packages, and passed six release checks. Full red/green and review evidence is in [12-23-LOCAL-EVIDENCE.md](12-23-LOCAL-EVIDENCE.md).

## Self-Check: PASSED

Both plan tasks and their acceptance criteria passed. The complete ordinary, package, release, hygiene, protected-path, staged-path, bounded review, and review-fix results are recorded in 12-23-LOCAL-EVIDENCE.md.
