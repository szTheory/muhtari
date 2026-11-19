---
phase: 29-windows-extended-path-runner-support
plan: "03"
subsystem: runner
tags: [windows, extended-paths, ctest, smb, ci]
requires:
  - phase: 29-01
    provides: Windows wide-path boundary and bounded runner path conversion
  - phase: 29-02
    provides: Invocation-wide path identity checks and atomic output publication
provides:
  - Source-qualified Windows x64 and ARM64 drive/SMB UNC acceptance for WINPATH-01 through WINPATH-04
  - Measured path, derived-save and sibling-temporary length boundaries documented for the runner
  - Local and hosted CI evidence with queue, critical-path and runner-cost review
affects: [runner, Windows CI, release, phase verification]
actuals:
  tokens: 40088
  tasks: 2
  commits: 34
commits: 34
plan_head_before: 95bd4ff564a60bc9a7ad0179cab22ffa5700d8ca
plan_head_after: 0536fa280277c1d0d9b16d4fc734dd9a962faed9
tech-stack:
  added: []
  patterns:
    - Windows path acceptance uses the existing x64 and ARM64 CI/Release lanes and a temporary SMB share, without new jobs or dependencies.
    - CTest cost metadata may move a measured slow test earlier while retaining its test membership and timeout.
key-files:
  created: []
  modified:
    - runner/main.c
    - tests/runner/windows_paths.c
    - tests/runner/windows_paths.ps1
    - tests/runner/trace.cmake
    - docs/runner.md
    - .planning/phases/29-windows-extended-path-runner-support/29-ACCEPTANCE.md
    - .planning/phases/29-windows-extended-path-runner-support/29-REVIEW.md
key-decisions:
  - "Accept the four Windows path requirements from the c3 source-qualified behavioral evidence on both Windows architectures; the synthetic CI merge tree exactly matches the reviewed candidate tree."
  - "Treat CTest COST 50 as a scheduling-only follow-up. The next hosted run must measure its ordering effect; no improvement is claimed from simulated registration."
  - "Keep CI/Release coverage and topology intact. The observed overlapping Release/Nightly equality work does not yet justify adding changed-file classifier machinery."
patterns-established:
  - "Record source identity, command, exit status, run URL, matrix measurements and cleanup evidence together in the acceptance matrix."
  - "Review queue delay, job durations, critical path, cancellations and duplicated work before proposing CI changes; require comparable runs for structural changes."
requirements-completed:
  - WINPATH-01
  - WINPATH-02
  - WINPATH-03
  - WINPATH-04
coverage:
  - id: D1
    description: "The actual runner accepted documented drive and extended UNC paths across cartridge, probe, movie, BIOS, HSC, state, save-directory, PNG, trace and derived battery roles, including Unicode, literal dot/space suffixes and paths beyond MAX_PATH."
    requirement: WINPATH-01
    verification:
      kind: e2e
      ref: "PR CI x64/ARM64 jobs https://github.com/szTheory/muhtari/actions/runs/44543122949/job/135626560630 and https://github.com/szTheory/muhtari/actions/runs/44543122949/job/135626560798; Release jobs https://github.com/szTheory/muhtari/actions/runs/44543122965/job/135626571973 and https://github.com/szTheory/muhtari/actions/runs/44543122965/job/135626571891"
      status: pass
    human_judgment: false
  - id: D2
    description: "Windows identity checks rejected suffix-distinct and aliased existing, hard-link, junction and mixed ordinary/extended input/output targets before mutation."
    requirement: WINPATH-02
    verification:
      kind: e2e
      ref: "Windows x64/ARM64 matrix and independent-base checks in PR CI https://github.com/szTheory/muhtari/actions/runs/44543122949 and Release https://github.com/szTheory/muhtari/actions/runs/44543122965"
      status: pass
    human_judgment: false
  - id: D3
    description: "Windows state, PNG, trace and derived battery outputs passed success/replacement cases; failures preserved prior bytes and cleaned owned temporary files."
    requirement: WINPATH-03
    verification:
      kind: e2e
      ref: "Windows x64/ARM64 output matrix in PR CI https://github.com/szTheory/muhtari/actions/runs/44543122949 and Release https://github.com/szTheory/muhtari/actions/runs/44543122965"
      status: pass
    human_judgment: false
  - id: D4
    description: "Ordinary paths and temporary SMB share lifecycle passed, measured filesystem/path limits were documented, and the full configured local workflow passed."
    requirement: WINPATH-04
    verification:
      kind: integration
      ref: "Windows x64/ARM64 SMB matrix in PR CI https://github.com/szTheory/muhtari/actions/runs/44543122949 and Release https://github.com/szTheory/muhtari/actions/runs/44543122965; local `cmake --workflow --preset ci` on snapshot 83a1709 exited 0"
      status: pass
    human_judgment: false
duration: "19h 8m"
started: 2026-11-18T20:29:51Z
completed: 2026-11-19
status: complete
---

# Phase 29 Plan 03: Windows extended-path acceptance Summary

**The real runner passed the drive and SMB UNC path matrix on Windows x64 and ARM64, with measured path limits and source-specific evidence recorded for all four requirements.**

## Performance

- **Duration:** 19h 8m elapsed across implementation, hosted runs and context handoffs.
- **Started:** 2026-11-18T20:29:51Z
- **Completed:** 2026-11-19T15:37:56Z
- **Tasks:** 2
- **Files modified:** 18

## Accomplishments

- Completed the actual-process drive and accessible SMB UNC matrix for every documented runner role on Windows x64 and ARM64. The matrix covers Unicode, literal dot/space names, paths beyond `MAX_PATH`, aliases and overlaps, successful replacement, failure preservation, and owned temporary-file cleanup.
- Recorded tested NTFS component limit 255; final output path lengths 500/492 UTF-16 units, `.sav` lengths 489/481, maximum battery-temp candidates 507/499, and all-temp candidates 518/510. The documented values are measured results, not theoretical Windows limits.
- c3 PR CI, Release and Nightly all completed successfully. The CI synthetic merge `ff5beaef5fde97d51a8858f485620669290495fe` has the same complete tree as candidate `be331d2f82666d3f5e6c1f1b307a8fb9f4aa0235`; base `f709d493e6660b3b53a98b49b693ab91a714dcb0` is an ancestor. Both Windows architectures passed the full 736-test inventory, release inventory, independent-base comparison and path/share checks.
- CI WASI passed 650/650 in 147.61 seconds; Release WASI passed 650/650 in 195.14 seconds. RetroArch, hash equality, required CI, Release rehearsal, install-line and cleanup checks passed. BE32 setup passed in CI and Release; neither run experienced a network retry.
- Local `cmake --workflow --preset ci` on the final local snapshot `83a1709` passed 736/736 tests, generated all three macOS packages and passed 8/8 release checks. The related hygiene and diff checks passed. Review of the scheduling-only follow-up was clean.

## Task Commits

Plan changes were committed as they were completed. The measured plan history contains 34 commits, from `ff8c312` through `0536fa2`; see `git log --reverse 95bd4ff..0536fa2` for the full atomic sequence. The first task committed the actual Windows matrix and its correctness fixes; the second qualified local/hosted results and recorded the measured contract.

## CI Efficiency Review

- CI completed in 48m16s, Release in 56m13s and Nightly in 21m21s. Queue delay was under about 40 seconds, the Windows ARM64 jobs were the critical path at 45m02s in CI and 44m52s in Release, and no c3 jobs were cancelled.
- Release and Nightly ran overlapping equality work for 52m53s combined. This is runner cost, not a demonstrated critical-path reduction. A changed-file classifier adds maintenance and alignment risk without evidence across independent PRs, so no CI topology or coverage change is recommended.
- The Windows path test took 8.68/5.25 seconds on x64 and 47.02/45.78 seconds on ARM64 in CI/Release. Its existing default CTest cost placed it near the end of the inventories (729/736 x64 and 735/736 ARM64). The follow-up sets Windows-only `COST 50`, retaining the same test, timeout, fixtures and coverage. Simulated registration confirmed the configured cost and timeout at scaling factors 1 and 2; it does not show hosted ordering or time saved. Measure that effect on the next hosted PR run before claiming improvement.

## Files Created/Modified

- `tests/runner/windows_paths.c` and `.ps1` - Real runner role/path matrix, SMB lifecycle, alias checks, measurements and cleanup.
- `tests/runner/trace.cmake` - Existing Windows matrix registration, calibrated timeout and scheduling cost.
- `runner/main.c` - Bounded path handling and output behavior exercised by the matrix.
- `docs/runner.md` - Measured supported Windows path forms and behavior.
- `29-ACCEPTANCE.md` - Source-qualified local, CI, Release and Nightly evidence.
- `29-REVIEW.md` and `.planning/LESSONS.md` - Review and reproduced engineering/CI lessons.

## Decisions Made

- The c3 behavioral run completes WINPATH-01 through WINPATH-04. Later CTest cost metadata does not alter test membership or behavior, and does not invalidate the source-qualified product evidence.
- The COST50 scheduling experiment remains unmeasured until c4. It is a follow-up efficiency observation, not a pending product requirement or phase acceptance gap.
- Preserve the existing CI matrix and all coverage. No cache, topology, timeout, or workflow-deadline change was supported by these runs.

## Deviations from Plan

Several in-scope failures exposed necessary fixes during the plan: Windows path allocation and derived-battery cases, GCC/MSVC test diagnostics, WASI path normalization and stack use, a compile-only PowerPC QEMU setup issue, and transient Ubuntu archive-fetch handling. These were corrected and followed by passing local configured CI or the c3 hosted checks. No required fault case or coverage was removed. The user-requested reusable fan-out and CI-efficiency guidance was also recorded in project instructions.

## Issues Encountered

Earlier local and hosted attempts are preserved in `29-ACCEPTANCE.md`, including the original local 735/736 failure and intermediate Windows/WASI/toolchain failures. Their causes were repaired or the runs were superseded by the successful c3 evidence. The final c3 results pass; COST50 hosted ordering remains unmeasured as described above.

## User Setup Required

None. No owner hardware or manual Windows check is needed.

## Next Phase Readiness

All four product requirements have local and source-qualified Windows evidence. The phase verifier owns the phase-level acceptance record; this plan does not create or edit `29-VERIFICATION.md`. The next hosted PR run can measure the COST50 ordering effect.

---
*Phase: 29-windows-extended-path-runner-support*
*Completed: 2026-11-19*

## Self-Check: PASSED

- Summary file exists at the plan output path.
- Task and acceptance commits `83a1709` and `0536fa2` are ancestors of the current plan head.
- The persisted plan ledger measures 34 commits from `95bd4ff564a60bc9a7ad0179cab22ffa5700d8ca` through `0536fa280277c1d0d9b16d4fc734dd9a962faed9`.
