# Phase 14 Plan 25: Local preflight evidence

**Outcome:** The offline diagnostic controls passed, but the single complete native CI gate failed on the independent R checkout. The plan stops before hygiene, the frozen-source gate, and its owner decision; no publication or host action occurred.

## Offline controls

The TDD RED commit is `9b1a642176e74fda93a3e6b4efe44bcb61309731`; the GREEN commit and candidate R are `5f0b4d42f076a8a84385209fa1a31383e3f05461`. The new receipt cases exercise the real `run_retroarch.cmake` launcher with a deterministic fake driver. The new endpoint-owner controls exercise the same pure decision used by the host path with synthetic observations. No sandbox, UDP helper, process-enumeration probe, or RetroArch process was launched by these controls.

| Command | Exit | Result |
| --- | ---: | --- |
| `cmake -DROOT=. -DEVIDENCE=build/phase14-host25-evidence -DMODE=controls -P build/phase14-host25-evidence/local-gates.cmake` (RED) | 0 | The wrapper recorded the intended CTest failure; the registered `retroarch.states.controls` assertion failed because the launcher receipt had only the existing sandbox-helper diagnostic instead of the new sandbox-exec diagnostic. CTest exit 8; run URL `N/A-local`. |
| `gsd_run check tdd-red-evidence build/phase14-host25-evidence/tdd-red.json --raw` | 0 | `RED_EVIDENCE_OK`; the named CTest target failed on the planned receipt assertion. |
| `cmake -DROOT=. -DEVIDENCE=build/phase14-host25-evidence -DMODE=controls -P build/phase14-host25-evidence/local-gates.cmake` (GREEN) | 0 | `retroarch.states.controls` passed. The five named prelaunch failures and endpoint ownership failure retained raw driver exit 7, failed outcome, no screenshot/save success, and their unrelated sentinel. Endpoint controls cover exact owner, no owner, multiple owners, foreign owner, exited child, process mismatch, and owner-scan error. Run URL `N/A-local`. |

The RED and GREEN JUnit reports are ignored runtime evidence. The target changed only offline fixtures and synthetic observations; it is not actual-host evidence.

## Native inputs and source identity

The independent detached checkout is `.worktrees/phase-14-host25-parent`, at candidate R `5f0b4d42f076a8a84385209fa1a31383e3f05461`. It remained clean. R's actual first parent is RED commit `9b1a642176e74fda93a3e6b4efe44bcb61309731`.

The existing pinned recipes acquired both MAME XML inputs, 7800Basic, OpenBIOS, and rcheevos in the detached checkout; each command exited 0. The first parent-path `wasmtime-aarch64-macos` fetch exited 56 after HTTP 504. After the owner clarified the one allowed parent-only retry, that exact pinned command exited 0. Its extracted executable is Mach-O arm64 with SHA-256 `9dad9691c32e097241d1f70371e36bea39ec3d51499134133340f77ecd9e6758`, matching the already recorded Plan 14-27 artifact identity. The original Plan 14-27 fetch and workflow were not repeated.

## Terminal native CI failure

| Command | Exit | Result |
| --- | ---: | --- |
| `cmake -DROOT="$PHASE14_PARENT_DIR" -DEVIDENCE="$PWD/build/phase14-host25-evidence/parent" -DMODE=full -P build/phase14-host25-evidence/local-gates.cmake` with `PHASE14_PARENT_DIR=.worktrees/phase-14-host25-parent` | 1 | The wrapper stopped after its one CI workflow failed. No retry was made. Run URL `N/A-local`. |
| `cmake --workflow --preset ci` in `.worktrees/phase-14-host25-parent` | 8 | Configure and build completed; the ordinary suite ran 695 tests, with 694 passing and `title.pin.self_test` timing out at its existing one-second limit. The output includes `sh: /bin/ps: Operation not permitted`. Packaging and release tests were not reached. Run URL `N/A-local`. |
| `cmake -DJUNIT=build/phase14-host25-evidence/parent/junit-ci.xml -DINVENTORY=.worktrees/phase-14-host25-parent/tests/inventory/ci.txt -DEXCLUDE=release -P .worktrees/phase-14-host25-parent/tests/cmake/check_junit.cmake` | 1 | Saved-only inventory validation confirmed the failed `title.pin.self_test` receipt. No tests were launched. |

The immutable ignored parent receipts are `build/phase14-host25-evidence/parent/full-ci.log` (SHA-256 `56d4ad6e1ba50e8800e469c8f930a166d89c0d4022cc447481f58247e2af5034`) and `junit-ci.xml` (SHA-256 `18505b7d7ded91d7123ad3e604dcef4aeddb5a5b0fa251ca700f598fcc12a7e5`). No release or hygiene JUnit was produced. The complete parent/source gate failed, so the frozen E workflow, decision packet, and owner checkpoint were not reached. This attempt is terminal under its scope.

Remote push, workflow dispatch, host start, Docker, and RetroArch counts are all zero. No source was pushed, no remote workflow was created, and no actual-host observation was made. FRONT-02, FRONT-03, FRONT-04 and Phase 14 acceptance remain pending. No claim of frontend qualification follows from the offline controls or partial local workflow.
