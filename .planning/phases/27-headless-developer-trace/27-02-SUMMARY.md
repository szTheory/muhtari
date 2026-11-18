---
phase: 27-headless-developer-trace
plan: "02"
subsystem: testing
tags: [runner, cpu-trace, jsonl, bounded-capture, atomic-output, ctest]
requires:
  - phase: 27-headless-developer-trace
    provides: Private CPU trace observer, runner capture and generated numeric consumer from plan 27-01
provides:
  - Source-grounded finite capture and JSONL output bounds with NTSC/PAL measurements
  - Pre-execution trace option and input/output alias rejection
  - Actual-runner atomic trace I/O fault coverage and terminal-footer consumer validation
affects: [27-03, runner, trace-acceptance]
actuals:
  tokens: 20130
  tasks: 3
  commits: 3
commits: 3
plan_head_before: 07f42aac55fc0d50fc7a6dfb1efb3a7d1cbb1f3f
plan_head_after: 9a3a1b0d47ce3459f4018c6ab7d5d3dc2e0f41a0
tech-stack:
  added: []
  patterns:
    - Reuse the runner-owned sibling-temp atomic writer for bounded trace streaming
    - Exercise runner failures through the existing actual-main I/O wrapper harness
key-files:
  created:
    - .planning/phases/27-headless-developer-trace/27-BOUNDS.md
  modified:
    - runner/main.c
    - src/internal.h
    - tests/runner/test_save.c
    - tests/runner/test_trace.c
    - tests/runner/tests.cmake
    - tests/runner/trace.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
key-decisions:
  - Use 65,536 default and 131,072 hard maximum trace events after checking PAL frame timing, instruction overshoot, actual 24-byte event size, 149-byte maximum record and generated workload measurements.
  - Keep trace path normalization specific to trace destinations and reuse the existing exclusive sibling-temp replacement lifecycle; no public observer, API, ABI or state-format change was needed.
patterns-established:
  - Trace fault tests assert old-file preservation, ownership-only temporary cleanup, diagnostic reporting and observer detachment through actual runner entry.
  - The trace consumer requires a valid terminal footer and checks complete and truncated outcomes separately.
requirements-completed: [TRACE-01, TRACE-03, TRACE-04, TRACE-05]
coverage:
  - id: D1
    description: Bounded trace capture distinguishes exactly-full complete output from additional-event truncation while emulation continues.
    requirement: TRACE-03
    verification:
      kind: integration
      ref: "ctest --test-dir build/ci --output-on-failure -R '^runner[.]trace[.](limits|bounds)$' (exit 0, 2 tests; N/A-local)"
      status: pass
    human_judgment: false
  - id: D2
    description: Invalid selectors and trace path aliases fail before emulation without replacing an input or another output.
    requirement: TRACE-03
    verification:
      kind: integration
      ref: "ctest --test-dir build/ci --output-on-failure -R '^runner[.]trace[.](arguments|paths)$' (exit 0; N/A-local)"
      status: pass
    human_judgment: false
  - id: D3
    description: Generated numeric records and trace-on/off frame and state hashes remain consistent.
    requirement: TRACE-04
    verification:
      kind: integration
      ref: "ctest --test-dir build/ci --output-on-failure -R '^runner[.]trace[.](oracle|limits)$' (exit 0; N/A-local)"
      status: pass
    human_judgment: false
  - id: D4
    description: Trace output failures preserve prior bytes, remove only owned temporaries, detach the observer and reject missing terminal footers.
    requirement: TRACE-05
    verification:
      kind: integration
      ref: "ctest --test-dir build/ci --output-on-failure -R '^runner[.]trace[.]io$|^core[.]trace[.]edges$|^runner[.]trace[.]memory$' (exit 0; N/A-local)"
      status: pass
    human_judgment: false
metrics:
  duration: 83min
  completed: 2026-11-16
status: complete
---

# Phase 27 Plan 02: Bounded trace capture and atomic output Summary

**`muhtari-run` now provides source-validated finite trace limits, rejects unsafe trace invocations before execution, and proves atomic failure handling through its actual runner and JSONL consumer.**

## Performance

- **Duration:** 83 minutes
- **Started:** 2026-11-16T20:42:00Z (continuation after plan 27-01)
- **Completed:** 2026-11-16T22:05:08Z
- **Tasks:** 3
- **Files modified:** 9

## Accomplishments

- Justified the 65,536-event default and 131,072-event maximum against the PAL frame deadline and bounded instruction/interrupt overshoot. The event measures 24 bytes, the maximum synthetic record line is 149 bytes, and generated NTSC/PAL captures confirmed storage and output measurements in [`27-BOUNDS.md`](./27-BOUNDS.md).
- Expanded actual-main CLI and path ownership acceptance for malformed selectors, effective movie/state frame numbering and collisions with inputs or requested outputs. Trace-only normalized and identity checks prevent aliases from replacing protected paths.
- Exercised trace allocation, exclusive temporary creation, `fdopen`, short/full writes, flush, sync, close and replacement faults. Each failure preserved the previous destination, left no owned temp, reported on stderr and detached the observer. Complete and truncated replacements pass the same consumer; an interrupted prefix is rejected.
- Verified refusal and early stop before the selected frame preserve the prior file, while a later unrelated refusal leaves the already committed selected-frame trace valid.
- Preserved D-07: capture remains runner-owned and private; public API 8, ABI 1 and machine state formats were not changed.

## Task Commits

1. **Task 1: Prove a bounded complete-or-truncated capture through the actual runner** — `aaf22d4` (`feat`)
2. **Task 2: Reject invalid trace invocations and input/output collisions before execution** — `ea0486d` (`fix`)
3. **Task 3: Prove trace file failure and replacement behavior at each real I/O boundary** — `9a3a1b0` (`test`)

GSD planning metadata is recorded in the separate metadata commit that includes this summary, STATE and ROADMAP updates.

## Files Created/Modified

- `.planning/phases/27-headless-developer-trace/27-BOUNDS.md` — source derivation, selected bounds, maximum-line proof and comparable generated NTSC/PAL measurements.
- `runner/main.c`, `src/internal.h` — private bounded capture, checked serialization and trace-specific path checks.
- `tests/runner/test_save.c`, `tests/runner/test_trace.c`, `tests/runner/trace.cmake`, `tests/runner/tests.cmake` — actual-main fault injection, output validation, argument/path acceptance and CTest registration.
- `tests/inventory/ci.txt`, `tests/inventory/wasm32.txt` — registered portable `runner.trace.io` coverage.

## Decisions Made

- Adopted finite limits only after checking the maximum PAL frame with instruction/interrupt overshoot and measuring actual event/line sizes and generated workload output. The hard cap is above the source-derived 81,416-event frame bound; allocation is 1.5 MiB by default and 3 MiB maximum.
- The shared sibling-temp writer already met trace atomicity requirements, so Task 3 extended its existing fault harness instead of adding runner failure flags or a second writer.
- Trace path alias safety stays narrow to trace destinations and uses existing platform primitives; unrelated runner path comparison behavior was left as-is.

## Verification Evidence

- Task 1: `cmake --preset ci && cmake --build --preset ci --target muhtari-run muhtari-test-trace && ctest --test-dir build/ci --output-on-failure -R '^runner[.]trace[.](limits|bounds)$'` — exit 0, 2 tests passed.
- Task 2: `git diff --check && cmake --preset ci && cmake --build --preset ci --target muhtari-run muhtari-test-trace muhtari-test-save-runner && ctest --test-dir build/ci --output-on-failure -R '^runner[.]trace[.]|^core[.]trace[.]edges$'` — exit 0, 9 tests passed.
- Task 3: `git diff --check && cmake --build --preset ci --target muhtari-run muhtari-test-save-runner muhtari-test-trace && ctest --test-dir build/ci --output-on-failure -R '^runner[.]trace[.]io$'` — exit 0, 1 test passed.
- Final focused regression: `git diff --check && ctest --test-dir build/ci --output-on-failure -R '^runner[.](trace[.]|save[.]|state[.])|^core[.]trace[.]edges$'` — exit 0, 20 tests passed.
- All checks were local. Run URL: N/A-local; no RetroArch, device or external-host checks were in scope.
- The hooks-off CPU cost gate remains for plan 27-03; it is not claimed by this summary.

### Post-Wave Integration Gate

- `cmake --workflow --preset ci` — exit 0; 733 CI tests and 8 release-preset checks passed, and all three package targets were produced. Run URL: N/A-local.

## Issues Encountered

- The first strict build rejected casts from `const char *` in the new test argument helper; the helper now copies option tokens into bounded local buffers.
- The first allocation-fault assertion expected a machine instance to be destroyed. Source inspection showed capture allocation runs before machine creation; the corrected test proves zero frames and no instance on that path. The complete `runner.trace.io` case then passed.
- No unresolved issues.

## Deviations from Plan

None — the tests reproduced the planned alias-safety gap and trace-specific normalization addressed it; the existing sibling-temp writer passed Task 3 without a production change.

## User Setup Required

None — no external service or device configuration is required.

## Next Phase Readiness

Plan 27-02 is complete. Plan 27-03 remains to prove the hooks-off CPU cost gate and complete the remaining compatibility and runner documentation work. It has not been started.

## Self-Check: PASSED

- Summary file exists at the required phase path.
- Task commits `aaf22d4`, `ea0486d` and `9a3a1b0` are ancestors of HEAD.

---
*Phase: 27-headless-developer-trace*
*Completed: 2026-11-16*
