---
phase: 06-retail-cartridge-boards
plan: "10"
status: complete
subsystem: testing
tags: [cmake, quality, fixtures, provenance]
requires:
  - phase: 05-pipeline-tune-up
    provides: Authenticated ordinary quality producers, exact join and independent domain judges
provides:
  - Narrow additive owned-fixture relation with unchanged trusted manifest identity
  - Actual event-base producer recursion and unchanged join exercised by fault controls
  - Matching quality documentation and unchanged local package acceptance
affects: [06-02, 06-03, 06-04, 06-05, 06-06, 06-07, 06-08, 06-09]
tech-stack:
  added: []
  patterns:
    - Prove the additive manifest relation before deriving the shared comparison identity
    - Exercise real current-script producer recursion and exact join using temporary controlled observations
key-files:
  created: []
  modified:
    - tests/cmake/quality.cmake
    - tests/cmake/quality_evidence.cmake
    - tests/policy/quality.cmake
    - docs/quality.md
key-decisions:
  - Resolve the same event-base Git manifest independently on both observation sides; dispatch recursion uses the event head first parent.
  - Preserve the complete compiler/platform/sample environment and exact join, raw base-policy authority and ordinary metrics.
requirements-completed: []
requirements-contributed: [BOARD-01, BOARD-02, BOARD-03]
coverage:
  - id: trusted-manifest-relation
    description: Only unique named owned board rows may be added while every trusted old row and other byte remains protected
    requirement: BOARD-01
    verification:
      - kind: integration
        ref: "policy.quality.controls through actual quality_evidence producer and quality_join; focused CTest exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: authenticated-producer-join
    description: Event-base Git authority and current-script recursion preserve surviving metrics and exact compiler/platform/sample equality
    requirement: BOARD-02
    verification:
      - kind: integration
        ref: "policy.quality.controls PR/push/dispatch, partial later-base, forged identity and compiler/platform/sample controls; full CI exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: documented-local-acceptance
    description: Documentation matches the exercised boundary and all local tests, release inventories and three-component ZIP sum pass
    requirement: BOARD-03
    verification:
      - kind: integration
        ref: "cmake --workflow --preset ci; exit 0; 609 non-release and 6 release checks; URL N/A-local"
        status: pass
      - kind: other
        ref: "Both exact check_junit invocations, scripts/hygiene.sh --tree, exactly-three wc -c ZIP sum 212216 <= 399937; exit 0 each; URL N/A-local"
        status: pass
    human_judgment: false
actuals:
  tokens: 5181
  tasks: 2
  commits: 2
plan_head_before: b970bd4aed93ffe4d09071f2c650c643282a6b90
plan_head_after: 94fcb906b8d821dc3c8c5b5e06082e7c9854e424
duration: 7min
started: 2026-10-23T18:39:19Z
completed: 2026-10-23
---

# Phase 6 Plan 10: Trusted fixture comparability Summary

**Authenticated additive fixture manifests share the original event-base comparison digest through actual producer recursion and the unchanged exact join.**

## Accomplishments

- Added the private `quality_fixture_identity` helper before the existing functions-only return. Only unique new Activision, Absolute and Fractalus owned rows are stripped for comparison; all remaining manifest bytes must equal the independent trusted original. Existing and later-base board paths cannot change or repeat.
- Integrated event-base Git extraction before environment production. Both sides resolve the same event and object, including dispatches through the event head's first parent. Original raw base policy/SHA, independent base workflow, current-script recursion, completed JUnit/cache/archive checks and all ordinary metric production remain authoritative. `quality_join.cmake` is byte-unchanged.
- Extended the existing policy dispatcher with a tiny temporary Git repository and controlled completed-workflow observations. Real producer output reaches the real join for unchanged/all-three additions, push/dispatch and partial later-base cases. No fixture bytes or product ROM was installed. Documentation describes the exact boundary and its distinction from legal admission, rebuilds, content hashes and board behavior.

## Task Commits

1. Task 1, trusted fixture relation through the real producer and join: `701d6b4`.
2. Task 2, comparability documentation and unchanged local acceptance: `94fcb90`.

The persisted plan ledger measures two task commits over the recorded before/after interval. Actual tokens are realized task-diff characters divided by four, rounded up; summary and subsequent state metadata are excluded.

## Commands, exits and run URLs

Every command here ran locally; run URL is N/A-local. No logs are committed.

| Command | Exit | Result |
|---|---:|---|
| `cmake --preset ci && ctest --preset ci -R '^policy[.]quality[.]controls$' --no-tests=error --output-on-failure` | 0 | 1/1; final pre-commit 3.83 s and post-commit tracer 3.89 s |
| `cmake '-DCMD=cmake;-DROOT=/tmp/muhtari-quality-before;-DWORK=/tmp/muhtari-quality-before-control;-P;tests/policy/quality.cmake' -DCODE=1 '-DREGEX=quality_evidence: incomparable base toolchain or fixtures' -P tests/cmake/expect_fail.cmake` | 0 | Control with previous producer exits 1 for the expected comparability fault; temporary root contains the previous producer and unchanged required consumers |
| `cmake --workflow --preset ci` | 0 | 609/609 non-release tests, all three component packages, 6/6 release checks; quality controls included |
| `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` | 0 | Exact 609 executed once, full workflow output replaced focused JUnit |
| `cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake` | 0 | Exact 6 release checks executed once |
| `scripts/hygiene.sh --tree` | 0 | Repository hygiene, with hooks retained for task and metadata commits |
| Exactly-three ZIP expansion and `wc -c` sum below | 0 | library 68292 + libretro 67400 + runner 76524 = 212216 <= 399937 |
| `git diff --check` | 0 | No whitespace errors |
| `git diff --exit-code -- tests/cmake/quality_join.cmake tests/roms/manifest.txt tests/built/expected.txt` | 0 | Join and product fixture/sample inputs unchanged |
| `node <gsd-tools> check evaluation-scope --plan 06-10 --commits-only --raw` | 0 | Both task commits resolved on this branch |

The package measurement command was:

```sh
set -- build/ci/muhtari-*.zip
test "$#" -eq 3 || exit 1
bytes=0
for archive do
  size=$(wc -c < "$archive")
  printf '%s %s bytes\n' "$archive" "$size"
  bytes=$((bytes + size))
done
printf 'archive_bytes=%s ceiling=399937\n' "$bytes"
test "$bytes" -le 399937
```

The consumer controls reject changed/deleted/duplicate old paths, changed or duplicated later-base board paths, duplicate new paths, unrelated paths, malformed fields/source/pin/license/digest, added comments and candidate-supplied trust inputs. Actual produced compiler/version, platform and sample changes fail exact join equality. Existing retirement, unrelated-policy deletion, typed/missing/unavailable evidence, source/SHA/configuration, reasons, exact lane membership and smaller/worst-lane controls still execute.

## Decisions Made

Followed the approved narrow relation. Event-base extraction moved before environment construction so the parent and current-script base recursion independently derive the same identity. Dispatch resolution uses the event head rather than the recursive worktree's HEAD; the dedicated dispatch positive exercises this distinction. No configurable ignore list, new dependency, new resource bound or maintenance allocation was introduced.

## Deviations from Plan

None - plan executed as written. Initial test-construction failures (missing temporary pin directory and stale temporary worktree registration) were corrected in the controlled repository before acceptance; product code or policy was not weakened.

## Issues Encountered

No unresolved implementation issue. The sandbox required escalation for Git metadata writes; normal hooks executed successfully. No authentication gate or owner verification was needed.

## Limits and Next Readiness

This is infrastructure only. No BOARD requirement is complete and no board diagnostic, cartridge behavior, API change or fixture byte was installed. Local packages establish only current macOS arm64 acceptance. Future board six-native current-head package/equality, sanitizer/fuzz/property and host evidence remains pending, agent-owned work in plans 02–09, especially 09. Genuine hosted retirement acceptance remains pending under the existing todo; no hosted result is claimed here.

No known stub, skipped test or unrun verify belonging to this plan remains. No new unplanned security surface or codebase structural change was introduced. No review-fix or gap allowance was consumed. Historical `06-01-SUMMARY.md` remains byte-identical, SHA-256 `3d45c86a60f886dcb2b6c571b7ba139dda8415d6c003d7cb44d9df7df6f50b26`.

Next incomplete plan: **06-02**, the Absolute tracer. Next execution command: `$gsd-execute-phase 6`; this executor stops after 06-10 and leaves later phase dispatch/gates to the parent.

## Self-Check: PASSED

All four changed task files exist. Both task commits are ancestors of HEAD and the branch-scoped evaluation resolver finds them. All task acceptance checks and the full local verification above passed. Join/product fixture inputs and the historical 01 summary remain unchanged. No stubs, generated untracked files or unexpected deletions remain.
