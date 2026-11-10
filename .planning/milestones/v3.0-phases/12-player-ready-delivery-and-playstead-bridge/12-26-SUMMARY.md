---
phase: 12-player-ready-delivery-and-playstead-bridge
plan: "26"
subsystem: local-diagnostic-tooling
tags: [accessibility, launcher, permission, diagnostic]
requires: []
provides:
  - "A private app bundle identity was prepared, but its offline qualification halted after a mock reached the real permission API."
affects: [PLAY-03, phase-12-host-acceptance]
status: halted
requirements-completed: []
coverage:
  - id: PLAY-03-separate-app-route
    description: "Prepare an isolated app identity and offline controls before any permission request or host launch."
    requirement: PLAY-03
    verification:
      kind: other
      ref: "12-26-LAUNCHER-EVIDENCE.md; one real CGRequestPostEventAccess call escaped from the unbundled stale mock, returned denied/77, and failed the zero-call preparation condition."
      status: fail
    human_judgment: false
  - id: PLAY-03-owner-checkpoint
    description: "Present an exact app and owner command after safe local qualification."
    requirement: PLAY-03
    verification:
      kind: manual_procedural
      ref: "Not reached; no owner command or app request was presented."
      status: unknown
    human_judgment: true
metrics:
  status: halted
  completed: 2026-11-10
---

# Phase 12 Plan 12-26: Separate App Launcher Summary

**A separately identified app bundle was prepared, but offline controls halted after the stale mock called the real permission API once.**

## Recorded Results

- App bundle compilation, plist validation, ad-hoc signing and identity capture completed locally.
- The offline mock receipt reports `request_result=denied` and `request_exit=77`.
- Static inspection of the mock binary finds `_CGRequestPostEventAccess`; its caller was an unbundled mock executable, not the signed app.
- The first mock source was corrected after compilation, but the launcher reused the old binary because it only rebuilt when the output was absent. The old binary remains unexecuted after the incident and is preserved.
- The recovery build creates a fresh mock at a different path and statically checks its undefined-symbol table before executing it. `python3 build/phase12-gap26-evidence/launcher.py recheck-offline-controls` exited 0; all 11 offline controls passed and no live `CG*`, `AX*` or `CFRelease` symbol remained. The old binary and receipt were preserved.
- That recovery run made zero additional permission requests, preflights, app launches, RetroArch launches or content launches. It does not undo Plan 12-26's failed zero-call condition.
- No app launch, LaunchServices dispatch, RetroArch launch, content launch, owner action, Terminal restart, settings inspection or permission change was performed by the agent.
- The prompt visibility and TCC attribution are unknown. The exact failed receipt and binary remain preserved in ignored local evidence.

## Acceptance Effect and Next Action

The plan is halted before its owner checkpoint. Its zero-real-request preparation condition failed, so it cannot claim a ready launcher. Phase 12 remains accepted 0/4, and all released-core, paired Playstead, published-byte and Windows-performance gates remain open. The repaired offline mock now passes the symbol gate and all 11 controls. A fresh bounded plan must retain that gate, use a newly reviewed app identity and bind the owner command to the exact runner code before any owner-controlled request. No permission retry or host action is authorized by this summary.

## Self-Check: PASSED

- The incident record distinguishes the unbundled mock from the prepared app and preserves the denied receipt without asserting prompt visibility or TCC attribution.
- `requirements-completed` is empty; PLAY-03 remains pending.
- No prior halt, host allowance, remote allowance, Windows criterion or phase acceptance count changed.
