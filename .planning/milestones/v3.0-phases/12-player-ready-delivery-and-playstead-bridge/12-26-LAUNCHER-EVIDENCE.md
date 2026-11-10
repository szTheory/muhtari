# Plan 12-26 Launcher Evidence

**Outcome: halted during offline control setup after a mock executable called the real CoreGraphics permission API.**

## What happened

The private launcher built `MuhtariInputDiagnostic.app`, verified its plist and
ad-hoc signature, and recorded the bundle identity. The app itself was not
launched. During the offline mock controls, the mock executable made one real
`CGRequestPostEventAccess` call. The captured receipt reports `denied` with
exit 77. The prompt's visibility and the system's attribution are unknown.

The receipt's `bundle_identifier` is not proof that the call came from the app:
the caller was the unbundled `controls/MuhtariInputDiagnostic-mock`, and its
test identity was supplied through mock environment variables. The fake-open
adapter was invoked; `/usr/bin/open` and LaunchServices were not. RetroArch,
content, and the prepared app were not launched. No Terminal restart, settings
inspection, or settings change occurred.

## Cause and limits

The compiled mock still has an undefined `_CGRequestPostEventAccess` symbol.
The source was then corrected so mock API aliases cover the request dispatcher,
but the old binary was not rebuilt. `build_controls()` only compiled when the
mock path was absent, so it reused that stale binary. The original denied
receipt and binary are kept in ignored `build/phase12-gap26-evidence/` as
incident evidence.

After stopping that path, the controls were repaired locally: a fresh binary is
built at a new path, and `nm -u` must show no unresolved `CG*`, `AX*`, or
`CFRelease` symbols before any mock is executed. `python3
build/phase12-gap26-evidence/launcher.py recheck-offline-controls` exited 0;
all 11 offline controls passed. The symbol table has no live permission or
event API symbols. This recovery made zero additional permission requests,
preflights, app launches, RetroArch launches or content launches. The original
mock and receipt remain unchanged.

This incident establishes a defect in the private offline-test harness. It
does not establish whether macOS displayed a prompt, which process TCC
attributed, whether permission was granted, or why the earlier hosted
diagnostic failed. It is not packaged-core or Playstead evidence.

## Stop and recovery boundary

Plan 12-26's zero-real-request preparation condition failed, so its owner
checkpoint is not ready even though the repaired offline controls now pass.
Do not execute the old mock, `owner-run`, any permission API, the prepared app,
or RetroArch under this plan. The failed receipt and counters are immutable
history. Any future permission request must be covered by a fresh bounded plan,
use a newly reviewed app identity, and remain an owner-controlled action. The
separate-app route can still avoid restarting the active Terminal; its
permission behavior has not yet been demonstrated.

Phase 12 remains accepted 0/4. Released-core input/DMA/battery evidence, paired
Playstead evidence, published-byte replay, and the Windows performance
comparison remain open. Windows support is unchanged.
