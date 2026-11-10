---
audit_acknowledged:
  milestone: v3.0
  at: 2026-11-10
  status: unknown
---

# Phase 12 OpenBIOS output-observability scope

Status: local implementation and bounded controls complete; uncommitted. No hosted run is authorized.

## Objective

Make a future Windows ARM64 `built.openbios` timeout retain enough evidence to
tell whether CMake attempted the Wasmtime command and what the child emitted,
without changing the one-second CTest timeout or the OpenBIOS image bytes.

## Evidence and current limit

- Same-source run [41323089043](https://github.com/szTheory/muhtari/actions/runs/41323089043) passed in 0.14 seconds with a complete stage trace.
- Same-source run [41329055050](https://github.com/szTheory/muhtari/actions/runs/41329055050) timed out at 1.74 seconds with the stage trace ending at `assemble` and no CTest output.
- The authenticated failure artifact contains `openbios-stage.txt`, `LastTest.log`, and `LastTestsFailed.log`, but no `assemble.log`.
- `tests/built/openbios.cmake` records `assemble` before `execute_process`, redirects child stdout to `assemble.log`, and retains stderr in a CMake variable until the call returns. The trace does not prove Wasmtime started.
- The root cause remains unknown. Preserve both outcomes and do not derive a new timeout from them.

## Bounded local scope

1. Read the latest closeout in `phase12-packaged-input-tracer.md`, this scope, current `STATE.md`, and the Phase 12 summary before editing.
2. Change only the OpenBIOS diagnostic capture and its Windows ARM64 artifact paths. Persist the command identity and child stdout/stderr where a CTest timeout can leave them available. Preserve the normal success/failure status and exact firmware digest checks.
3. Extend the existing OpenBIOS self-test or an equally narrow CMake control to verify successful completion and a representative failing child leave useful diagnostic output. Keep changes dependency-free.
4. Run only the bounded local validation required for these changes, record exact commands and exits, and review the diff. Preserve unrelated dirty planning and documentation work.

## Stop conditions and exclusions

- Keep `TIMEOUT 1`; do not add a timeout grace period or alter the test workload.
- Do not dispatch, rerun, cancel, or otherwise contact hosted CI. The previous hosted allowance is spent.
- Do not claim the timeout cause, input acceptance, or phase acceptance from local controls.
- Any future hosted observation needs a separate proposal with an explicit run cap, runner-minute allowance, observation window, and owner authorization after local controls pass.

## Completion evidence

The local capture control preserves a useful artifact for both a completed
assembler invocation and a representative child failure; normal firmware
hashes and existing OpenBIOS pass/fault behavior remain intact. Record the
evidence in the checkpoint and phase summary, then stop at the hosted-approval
boundary.

## Local outcome (2026-11-01)

The recipe now writes `assemble-command.txt` before `execute_process` and sends
stdout/stderr directly to explicit `assemble.log`/`assemble.stderr.log` paths
beside `openbios.bin`. Ordinary child failures still report the nonzero status
and stderr. Windows ARM64's existing always-run artifact step includes all
three files, with its condition and artifact identity unchanged. A prepared
command and empty/partial streams do not prove launch or completion.

The existing self-test now checks successful capture and a real Wasmtime
missing-module failure, while retaining the duplicate-incbin fault. Local
controls reproduce the missing capture before the repair and pass afterward;
the adjacent BIOS handoff passes, both 16 KiB firmware digests are unchanged,
and generated test properties remain `TIMEOUT 1` for both OpenBIOS tests.
Exact commands/exits and scratch baseline/repaired comparison are recorded in
the [phase summary](../phases/12-player-ready-delivery-and-playstead-bridge/12-07-SUMMARY.md#local-openbios-output-capture-2026-11-01)
and active debug session. No full workflow or hosted contact occurred.

The local capture gap is closed within these controls. Actual Windows timeout
and input causes remain unknown; original halt, accepted 0/4, PLAY-03 pending,
blocked successors, failed receipts and all spent allowances persist. Stop at
the boundary: any hosted publication or observation needs a separate concrete
scope and owner authorization. No owner verification is needed for the local
result.
