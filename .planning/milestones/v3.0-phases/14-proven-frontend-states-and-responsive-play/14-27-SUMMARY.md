---
phase: 14-proven-frontend-states-and-responsive-play
plan: "27"
subsystem: infra
tags: [wasmtime, local-recovery, prerequisite]
requires: []
provides:
  - The existing pinned Darwin arm64 Wasmtime executable at the ignored path used by the native build consumer
  - Read-only acquisition, native-identity, and execution-start preservation evidence
affects: [phase-14-acceptance, 14-25-local-preparation]
actuals:
  tokens: 8737.0
  tasks: 2
  commits: 2
tech-stack:
  added: []
  patterns:
    - Reuse the existing archive-checking pinned fetch script
    - Separate the archive SHA-256 from the extracted executable SHA-256
key-files:
  created:
    - .planning/phases/14-proven-frontend-states-and-responsive-play/14-27-EVIDENCE.md
  modified:
    - .planning/STATE.md
    - .planning/ROADMAP.md
key-decisions:
  - "Use only the existing wasmtime-aarch64-macos pin and its checksum-validating fetch path."
  - "Keep FRONT-02/03/04 pending; execute-phase ran the one authorized local workflow after the summary and preservation checks, and it passed."
patterns-established:
  - "Capture tracked, staged, untracked, and protected-file baselines before replacing an ignored local tool."
  - "Identify native executable bytes read-only without running the executable."
requirements-completed: []
coverage:
  - id: FRONT-02
    description: Actual pinned-host restored picture and progress remain unverified after local tool recovery
    requirement: FRONT-02
    verification:
      - kind: other
        ref: .planning/phases/14-proven-frontend-states-and-responsive-play/14-27-EVIDENCE.md#preserved-acceptance-and-follow-on-boundaries
        status: unknown
    human_judgment: false
  - id: FRONT-03
    description: Repeated rewind and deterministic input, audio, timing, and battery behavior remain unverified here
    requirement: FRONT-03
    verification:
      - kind: other
        ref: .planning/phases/14-proven-frontend-states-and-responsive-play/14-27-EVIDENCE.md#preserved-acceptance-and-follow-on-boundaries
        status: unknown
    human_judgment: false
  - id: FRONT-04
    description: Restore/replay isolation and speculative durable-write safety remain unverified here
    requirement: FRONT-04
    verification:
      - kind: other
        ref: .planning/phases/14-proven-frontend-states-and-responsive-play/14-27-EVIDENCE.md#preserved-acceptance-and-follow-on-boundaries
        status: unknown
    human_judgment: false
duration: 20min
completed: 2026-11-05
status: complete
commits: 2
plan_head_before: 6d143498bcd03661edf997c980891c58b18828f1
plan_head_after: 77e995407133958ec3d5aeeabbcade89d19e4097
---

# Phase 14 Plan 27: Pinned native Wasmtime recovery Summary

**The checksum-validated Darwin arm64 Wasmtime artifact now occupies the existing ignored build input path, and its single automatic CI gate passed; frontend acceptance remains pending.**

## Performance

- **Duration:** 20 min
- **Started:** 2026-11-05T13:39:32Z
- **Completed:** 2026-11-05T13:59:53Z
- **Tasks:** 2
- **Files modified:** 4 tracked files including the evidence and summary plus sequential STATE/ROADMAP bookkeeping

## Accomplishments

- Invoked the existing pin fetch exactly once. Its archive checksum validation passed and it replaced only the ignored Linux executable with the pinned Darwin arm64 tool.
- Read-only inspection identified the extracted file as a native Mach-O arm64 executable and recorded its digest separately from the archive digest.
- Verified tracked, staged, and untracked starting state plus all 17 protected files. The owner’s starting `.planning/STATE.md` change remained unchanged through both task verification stages.
- Preserved the terminal Plan 14-24 and 14-26 records and Plan 14-25’s existing dependency, native gates, and blocking owner decision.

## Task Commits

1. **Task 1: Restore the pinned artifact-to-native-build input path once** — `932dd27` (docs)
2. **Task 2: Prove tracked preservation and close the prerequisite evidence** — `77e9954` (docs)

The summary, STATE, and ROADMAP closeout is committed separately after self-checks.

## Files Created/Modified

- `.planning/phases/14-proven-frontend-states-and-responsive-play/14-27-EVIDENCE.md` — records acquisition identity, starting-state receipts, preservation checks, historical halts, and the passing automatic gate.
- `.planning/phases/14-27-SUMMARY.md` — records task outcomes and keeps all three frontend requirements pending.
- `.planning/STATE.md` and `.planning/ROADMAP.md` — sequential progress bookkeeping; Plan 14-25 is now selected for local work through its owner checkpoint.

## Verification

| Command/check | Exit | Result |
| --- | ---: | --- |
| `env -u PINS sh scripts/fetch-pin.sh wasmtime-aarch64-macos build/pins/wasmtime` | 0 | Exactly one invocation; the existing script validated the pinned archive SHA-256 before extraction. Run URL: `N/A-local`. |
| Task 1 read-only host/pin/destination preflight and Mach-O arm64 identity check | 0 | Host is Darwin arm64; the ignored executable is a native 64-bit Mach-O arm64 executable. Its observed SHA-256 is in the evidence file. |
| Task 2 tracked, cached, untracked, protected-byte, dependency, and whitespace checks | 0 | All checks passed against the execution-start snapshots. |
| Automatic post-wave `cmake --workflow --preset ci` | 0 | Execute-phase invoked it once after the summary and preservation checks. Configure/build/package completed, CTest passed 695/695, release tests passed 6/6, and three expected macOS arm64 packages were produced. Run URL: `N/A-local`. |

Remote CI, push, workflow dispatch, Docker, RetroArch, and host actions were `N/A-local` and not attempted.

## Decisions Made

- Reused the existing exact Darwin arm64 pin, release URL, archive checksum, and fetch behavior; made no source, test, pin, preset, inventory, deadline, or CI-policy changes.
- Preserved `requirements-completed: []` and unknown coverage because a local tool recovery cannot establish actual frontend behavior.
- Kept Plan 14-25 separate. Its existing blocking-human checkpoint remains before any push, remote dispatch, or host attempt.

## Deviations from Plan

None. The single automatic workflow passed under execute-phase as specified; no duplicate was run.

## Issues Encountered

- The first shell wrapper for three Python preservation checks had unmatched quoting and exited before Python ran. Corrected read-only wrappers passed all three checks; repository state was unchanged by the malformed wrapper.
- Git metadata writes were denied under the default sandbox. The orchestrator restored the required plan ledger, and normal Git task commits with hooks enabled succeeded using the approved escalation path.

## User Setup Required

None.

## Next Phase Readiness

The local tool prerequisite, preservation checks, and one post-wave workflow are complete. Plan 14-25 is now selected for local preparation through its existing blocking owner checkpoint. Phase 14 acceptance and FRONT-02/03/04 remain blocked; this summary grants no remote or host authorization.

## Self-Check: PASSED

- The evidence and summary files exist.
- Both task commits are ancestors of the current phase branch.
- All Task 1 and Task 2 acceptance checks and the single execute-phase automatic workflow passed.

---
*Phase: 14-proven-frontend-states-and-responsive-play*
*Completed: 2026-11-05*
