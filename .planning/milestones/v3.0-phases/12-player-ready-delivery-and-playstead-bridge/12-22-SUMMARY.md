---
phase: 12-player-ready-delivery-and-playstead-bridge
plan: "22"
subsystem: testing
tags: [retroarch, input-trace, ctest, posix-shell]
requires:
  - phase: 12-10
    provides: owned input-history cartridge generation and validation
provides:
  - owner-run isolated RetroArch keyboard diagnostic wrapper
  - bounded passive child launcher and manual callback validator
  - offline fault controls in the existing policy preflight CTest
affects: [PLAY-03, phase-12-acceptance]
actuals:
  tokens: 19379
  tasks: 3
  commits: 1
plan_head_before: 353d1afe8979cabc4c179af3036ebab1164cdf46
plan_head_after: fa567595d68535d56013f8dd054057f1503a715c
tech-stack:
  added: []
  patterns: [bounded exact-child supervision, offline physical-input trace controls]
key-files:
  created:
    - scripts/retroarch-manual-input.sh
    - .planning/phases/12-player-ready-delivery-and-playstead-bridge/12-22-LOCAL-EVIDENCE.md
  modified:
    - tests/libretro/input_trace_host.c
    - tests/libretro/test_input_trace.c
    - tests/libretro/input_trace.cmake
    - tests/policy/tests.cmake
    - docs/testing.md
    - AGENTS.md
key-decisions:
  - "The agent prepares and verifies the diagnostic locally; only the owner runs its physical input path."
  - "Callback delivery and visible cartridge progression remain separate observations."
patterns-established:
  - "Manual input verdicts require complete, clock-valid traces and an ordered neutral/held/released sample."
  - "Child cleanup polls the exact PID within monotonic deadlines and reports an unreaped child distinctly."
requirements-completed: []
coverage:
  - id: D1
    description: "Isolated RetroArch diagnostic wrapper with safe preparation mode."
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci -R '^policy\\.retroarch\\.preflight_api$' --output-on-failure"
        status: pass
      - kind: other
        ref: "scripts/retroarch-manual-input.sh --prepare-only"
        status: pass
    human_judgment: false
  - id: D2
    description: "Offline parser, path-safety, argv, EINTR deadline and exact-child cleanup controls."
    verification:
      - kind: integration
        ref: "policy.retroarch.preflight_api: manual-control and --manual-core cases"
        status: pass
      - kind: other
        ref: "bounded independent review: SECURED"
        status: pass
    human_judgment: false
  - id: D3
    description: "Owner's physical Up+Z callback result and separate visible cartridge observation."
    requirement: PLAY-03
    verification:
      - kind: manual_procedural
        ref: "scripts/retroarch-manual-input.sh --run; owner reported callback success and observed the yellow complete background"
        status: pass
    human_judgment: true
    rationale: "Only the owner can perform the requested physical keyboard action and observe the actual RetroArch window."
duration: 58min
completed: 2026-11-08
status: complete
---

# Phase 12 Plan 22: Owner-run RetroArch input diagnostic Summary

**An isolated, bounded RetroArch keyboard diagnostic now has offline coverage; the physical callback and visible-color observations remain with the owner.**

## Performance

- **Duration:** about 58 minutes from the first recorded implementation artifact
- **Started:** 2026-11-08T11:54:50-05:00
- **Completed:** 2026-11-08T12:52:41-05:00
- **Tasks:** 3/3 implementation and verification tasks complete
- **Files modified:** 9 plan files, excluding orchestrator-owned state

## Accomplishments

- Added `scripts/retroarch-manual-input.sh` with explicit `--run` and `--prepare-only` modes. It uses the existing history-cart generator, private RetroArch settings and output paths, a 3600-frame cap, and the existing trace consumers.
- Added `--manual-launch` and `--manual-core` plus synthetic valid/fault controls to the existing CTest path. Trace-file/symlink refusals preserve sentinels; input verdicts require a complete clock-valid trace with active polling and neutral → both-held → both-released values.
- Preserved owner control and phase boundaries in `AGENTS.md` and `docs/testing.md`. The owner later completed the one-time diagnostic run; it does not complete PLAY-03 or Phase 12.

## Task Commits

The implementation and its offline controls share files across Tasks 1 and 2, so they were committed together with the Task 3 documentation as one atomic feature commit: `fa567595d68535d56013f8dd054057f1503a715c` (`feat(12-22): add bounded owner-run RetroArch input diagnostic`). The first normal GSD staging attempt was denied because the sandbox exposed `.git` read-only; the root executor then used a scoped local repository-write escalation. Nothing was pushed or merged.

## Files Created/Modified

- `scripts/retroarch-manual-input.sh` - one-command isolated owner diagnostic and safe preparation mode.
- `tests/libretro/input_trace_host.c` - passive exact-child launch, bounded cleanup, and offline interruption/path controls.
- `tests/libretro/test_input_trace.c` - complete ordered callback sequence validator and fault fixtures.
- `tests/libretro/input_trace.cmake` and `tests/policy/tests.cmake` - controls nested in the existing preflight CTest.
- `docs/testing.md` - owner command, what each observation proves, and diagnostic limits.
- `AGENTS.md` - 2026-11-08 preference for offline/CLI evidence and avoiding repeated Mac control.
- `12-22-LOCAL-EVIDENCE.md` - exact checks, review disposition, timing history and pending owner action.

## Decisions Made

- The owner performs the one physical run because only it can establish what their installed RetroArch receives from their keyboard; agent automation and synthetic input cannot prove that path.
- The callback trace cannot establish visible emulated-pin progression, so the owner reports the two observations separately.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 2 - Security] Kept all cleanup waits bounded across EINTR and the post-SIGKILL path**
- **Found during:** Task 2, independent threat review
- **Issue:** An unconditional blocking `waitpid` could exceed the stated bound, and a TERM-grace deadline reached through repeated EINTR could skip SIGKILL.
- **Fix:** Use monotonic deadline checks after interrupted `poll`, `read`, `waitpid` and `nanosleep`; after TERM grace, send SIGKILL to the exact unreaped PID and poll only until the original cleanup deadline. Report `MANUAL_UNREAPED` with the PID when the child is still unreaped.
- **Files modified:** `tests/libretro/input_trace_host.c`, `tests/libretro/input_trace.cmake`
- **Verification:** Interrupted synthetic child continues SIGALRM through SIGTERM, then test asserts SIGKILL status and exact-child reaping; focused CTest and full CI pass.

**2. [Rule 2 - Missing critical integration] Passed the existing build path into the policy consumer**
- **Found during:** Task 1
- **Issue:** The wrapper integration control needs the configured build directory to exercise the actual `--run` wrapper without relying on an unrelated path.
- **Fix:** Pass `BUILD` into the existing policy preflight consumer; no new CTest inventory entry was added.
- **Files modified:** `tests/policy/tests.cmake`, `tests/libretro/input_trace.cmake`
- **Verification:** The wrapper's synthetic run and `--prepare-only` path pass inside `policy.retroarch.preflight_api`.

**3. [Rule 1 - Test reliability] Kept synthetic cleanup cases inside the existing five-second consumer budget**
- **Found during:** Task 3, full CI
- **Issue:** The preflight test first exceeded its fixed five-second timeout at 5.02s under full parallel load.
- **Fix:** Reduced only the three synthetic child budgets to 150ms wall plus 150ms cleanup grace. Production remains 75s plus 2s; the CTest timeout was not changed.
- **Files modified:** `tests/libretro/input_trace_host.c`
- **Verification:** Final focused run 3.03s; final full CI run's focused consumer 4.79s and passed.

**4. [Rule 3 - Documentation budget] Condensed the hosted RetroArch section to meet the existing docs limit**
- **Found during:** Task 3
- **Issue:** Added instructions pushed `docs/testing.md` above its 40,960-byte budget.
- **Fix:** Shortened the existing hosted RetroArch text; final size is 40,940 bytes.
- **Files modified:** `docs/testing.md`
- **Verification:** `docs.budgets` passes.

## Issues Encountered

- The first format check found style drift. `clang-format-21` was absent from `PATH`; Xcode's Apple clang-format 21 was available through `xcrun`. Formatting, explicit dry-run and `hygiene.format` then passed.
- The first parallel hygiene workflow timed out only `hygiene.tree` at 60.01s. The direct tree scan passed in 43.74s and the complete hygiene workflow rerun passed all 30 checks in 46.93s without changing its timeout.
- Independent bounded review returned **SECURED** for all three declared threats. It found no remaining unbounded EINTR or blocking cleanup path.
- No RetroArch process was launched, no permission/event/Accessibility/TCC action was attempted, and no remote operation occurred.

## Repository Commit Boundary

The first GSD staging attempt failed before staging because `.git/index.lock` creation was denied by the sandbox. A scoped escalation then allowed the local feature commit on the existing Phase 12 branch. No remote operation occurred.

## User Setup Required

No installation or configuration change was required. The owner completed the one-time physical check from a normal GUI-backed Terminal.

## Next Phase Readiness

The owner-run diagnostic passed: RetroArch sampled neutral → Up+Z held → released, and the owner saw the yellow complete background. Phase 12 remains accepted 0/4 and PLAY-03 remains pending because this diagnostic does not replace released-core visible input, DMA, battery, aggregate or paired Playstead evidence.

**Owner command:** `scripts/retroarch-manual-input.sh --run` (completed once; no repeat needed)

Reconcile the owner result through `$gsd-verify-work 12`. The executor must not run the physical command.

## Self-Check: PASSED

- `12-22-SUMMARY.md` and `12-22-LOCAL-EVIDENCE.md` exist.
- Local command results and the bounded review are recorded in the evidence file.
- The feature implementation, documentation, and owner-run diagnostic result are recorded; the plan is complete.

---
*Phase: 12-player-ready-delivery-and-playstead-bridge*
*Completed: 2026-11-08*
