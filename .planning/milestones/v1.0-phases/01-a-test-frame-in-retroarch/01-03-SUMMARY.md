---
phase: 01-a-test-frame-in-retroarch
plan: 03
subsystem: testing
tags: [ctest, cmake, junit, negative-control, no-skip, policy]

requires:
  - phase: 01-02
    provides: "muhtari_add_test, the exact inventory check and tests/inventory/ci.txt"
provides:
  - "tests/cmake/expect_fail.cmake: negative-control runner, -DCMD= -DCODE= -DREGEX=, passes only on that exit code and a matching output"
  - "tests/cmake/no_skip.cmake: -DROOT= scan that fails naming file and line for any skip or disable test property"
  - "tests/cmake/check_junit.cmake: -DJUNIT= -DINVENTORY= [-DLABELS=] [-DEXCLUDE=] completeness check for a test step"
  - "tests/policy/tests.cmake: policy.no_skip and policy.no_skip.self_test"
  - "cmake --workflow --preset ci passes with 15 tests"
affects: [01-04, 01-05, 01-06, 01-07, 01-08, 01-09, 01-14]

actuals:
  tokens: 1835
  tasks: 2
  commits: 2
plan_head_before: 121c3c599bac96bda8005db38e47ca599323c569
plan_head_after: bc94fb6b14bcd41c73fe41f70b247c40d0367b9f

tech-stack:
  added: []
  patterns:
    - "A self test runs its gate through expect_fail.cmake with an exact CODE and a REGEX naming the rule; bare WILL_FAIL is never used"
    - "A command list passed to a -P script through add_test escapes its semicolons as $<SEMICOLON>"
    - "Scripts that number source lines read the file and neutralise ; [ ] before splitting on newlines, because file(STRINGS) merges lines after an unbalanced ["
    - "A file that must not match its own scan builds the forbidden words from pieces"

key-files:
  created:
    - tests/cmake/expect_fail.cmake
    - tests/cmake/no_skip.cmake
    - tests/cmake/check_junit.cmake
    - tests/policy/tests.cmake
  modified:
    - tests/tests.cmake
    - tests/inventory/ci.txt

key-decisions:
  - "no_skip scans source text (tests/**/*.cmake, cmake/*.cmake, CMakeLists.txt) as the plan specifies, not CTest's JSON listing as the nesturbator seed did, so it needs no build and names the line"
  - "check_junit treats a testcase without status=\"run\" as not run, in addition to failure, error and skipped elements"
  - "check_junit reports every failed testcase in the file, selected or not, and counts only selected inventory names for exactly-once"

patterns-established:
  - "Negative-control pair: x plus x.self_test through expect_fail.cmake, with a configure-time fixture under the build tree"
  - "Every gate script opens with a usage line and a 'Fault prevented:' comment"

requirements-completed: [FRAME-01]

coverage:
  - id: D1
    description: "expect_fail.cmake passes only on the exact exit code and an output matching the regex; a wrong code, a wrong regex or a missing command fails"
    requirement: FRAME-01
    verification:
      - kind: integration
        ref: "tests/policy/tests.cmake#policy.no_skip.self_test"
        status: pass
      - kind: other
        ref: "cmake -DCMD=cmake;-DROOT=build/ci/policy/no_skip_fixture;-P;tests/cmake/no_skip.cmake -DCODE=2 -DREGEX=CMakeLists.txt:1: SKIP_RETURN_CODE -P tests/cmake/expect_fail.cmake exits 1; with REGEX=REGULAR_EXPRESSION exits 1; with CMD=nonexistent-binary exits 1"
        status: pass
    human_judgment: false
  - id: D2
    description: "A skip or disable test property anywhere in the test CMake files fails policy.no_skip, naming file and line; the repository itself passes"
    requirement: FRAME-01
    verification:
      - kind: integration
        ref: "tests/policy/tests.cmake#policy.no_skip"
        status: pass
      - kind: other
        ref: "cmake -DROOT=<scratch tree with an unbalanced [ line, an empty line and a ; line before the hits> -P tests/cmake/no_skip.cmake exits 1 naming tests/x/t.cmake:4 and cmake/m.cmake:3, and does not flag MY_DISABLED_FLAG"
        status: pass
    human_judgment: false
  - id: D3
    description: "check_junit.cmake fails naming the test when a selected inventory name is missing, duplicated, failed or skipped, and fails when no test ran; it passes on the ci JUnit file"
    requirement: FRAME-01
    verification:
      - kind: integration
        ref: "cmake --workflow --preset ci && cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake"
        status: pass
      - kind: other
        ref: "check_junit on copies of build/ci/junit.xml with runner.hash removed, zero testcases, one duplicated, one failure element, one skipped element, and with LABELS=zzz: each exits 1 naming the test or no test ran"
        status: pass
    human_judgment: false
  - id: D4
    description: "cmake --workflow --preset ci configures, builds and passes all 15 inventory tests"
    requirement: FRAME-01
    verification:
      - kind: integration
        ref: "cmake --workflow --preset ci"
        status: pass
    human_judgment: false

duration: 3min
completed: 2026-10-16
status: complete
---

# Phase 1 Plan 03: Negative controls, no-skip and JUnit completeness Summary

**Three CMake -P gates: expect_fail.cmake (exact exit code plus message regex), no_skip.cmake (file:line scan for skip or disable properties, proven by its own self test) and check_junit.cmake (every selected inventory name exactly once, run, not failed or skipped); the ci workflow now passes 15 tests**

## Performance

- **Duration:** 3 min
- **Started:** 2026-10-16T14:51:18Z
- **Completed:** 2026-10-16T14:54:25Z
- **Tasks:** 2
- **Files modified:** 6

## Accomplishments
- A negative control can no longer pass on an arbitrary failure: expect_fail.cmake requires the exact exit code and a matching message.
- A test that would skip itself or be disabled is caught at the source line by policy.no_skip, and policy.no_skip.self_test proves the scan fires.
- A test step that ran zero tests, skipped or failed one, or lacks or duplicates an inventory name fails check_junit.cmake, with label include and exclude filters for per-lane steps.

## Task Commits

1. **Task 1: End-to-end negative control: expect_fail, no_skip and its self test through CTest** - `5096a29` (test)
2. **Task 2: JUnit completeness check** - `bc94fb6` (test)

## Files Created/Modified
- `tests/cmake/expect_fail.cmake` - negative-control runner
- `tests/cmake/no_skip.cmake` - self-skip source scan
- `tests/cmake/check_junit.cmake` - JUnit completeness against the inventory
- `tests/policy/tests.cmake` - registers policy.no_skip and its self test, writes the fixture at configure time
- `tests/tests.cmake` - includes the policy tests
- `tests/inventory/ci.txt` - lists the two policy tests

## Gate-code tally

| Lines | File |
|---|---|
| 19 | tests/cmake/expect_fail.cmake |
| 38 | tests/cmake/no_skip.cmake |
| 58 | tests/cmake/check_junit.cmake |
| 20 | tests/policy/tests.cmake |
| 69 | cmake/MuhtariTests.cmake (plan 02) |
| 20 | tests/runner/tests.cmake (plan 02) |
| **224** | repository-check and registration code, against **315** lines in `src/` |

The tenth-of-`src/` budget in AGENTS rule 7 applies from phase 3; check_junit came out at 58 lines against the research estimate of 40, mostly the label filter and the per-name messages.

## Evidence

- `cmake --workflow --preset ci` exit 0; CTest reports "100% tests passed out of 15" (CMake 4.4.3 prints this wording, not "0 tests failed out of 15").
- `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` exit 0, "15 tests ran once each".
- `cmake -DCMD=<no_skip on the fixture> -DCODE=2 -DREGEX=... -P tests/cmake/expect_fail.cmake` exit 1.
- check_junit on the dropped, zero, duplicated, failed and skipped JUnit copies and with `-DLABELS=zzz`: exit 1 each.
- `cmake -DROOT=. -P tests/cmake/no_skip.cmake` exit 0.

## Decisions Made
- no_skip reads the source rather than CTest's JSON listing (the nesturbator 9f9386c661f6 seed's method), as the plan specifies; it needs no build tree and names the line. Commit 5096a29 cites the seed as rewritten, not ported.
- check_junit also counts a testcase whose status is not "run" as not run.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 3 - Blocking] Command list escaping through add_test**
- **Found during:** Task 1
- **Issue:** Escaping the CMD list as `\;` reached expect_fail.cmake literally, so execute_process got one bogus program name and exited "no such file or directory".
- **Fix:** tests/policy/tests.cmake replaces `;` with `$<SEMICOLON>`, which add_test evaluates to a real semicolon inside one argument.
- **Verification:** policy.no_skip.self_test passes; `ctest -V` shows `-DCMD=cmake;-DROOT=...;-P;...` as one argument.
- **Committed in:** 5096a29

**2. [Rule 1 - Bug] Line numbers after an unbalanced bracket**
- **Found during:** Task 1, before writing no_skip.cmake
- **Issue:** A probe showed `file(STRINGS)` merges the line after an unbalanced `[` into one list element, which would shift reported line numbers.
- **Fix:** no_skip.cmake (and check_junit.cmake for the JUnit text) read the file and replace `;`, `[` and `]` before splitting.
- **Verification:** the scratch-tree probe reports lines 4 and 3 correctly past `[`, `;` and empty lines.
- **Committed in:** 5096a29

---

**Total deviations:** 2 auto-fixed (1 blocking, 1 bug)
**Impact on plan:** Both were needed for the scripts to work as specified. No scope added.

## Issues Encountered
None beyond the deviations above.

## User Setup Required
None - no external service configuration required.

## Next Phase Readiness
- Later gates add `x` plus `x.self_test` through expect_fail.cmake with the same `$<SEMICOLON>` pattern.
- CI (plan 14) runs check_junit.cmake after every test step with the lane's label filter.

---
*Phase: 01-a-test-frame-in-retroarch*
*Completed: 2026-10-16*

## Self-Check: PASSED
