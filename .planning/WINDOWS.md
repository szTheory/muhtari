---
schema_version: 1
open_count: 17
waived_count: 0
fixed_count: 20
total_count: 37
last_updated: 2026-10-23T19:56:15.118Z
---

# Broken Windows Ledger

> Cross-phase defect register. With `workflow.windows_enforce` enabled, `/gsd-ship` blocks while `open_count > 0`.
> Waive with `gsd-tools windows waive <id> "<reason>"` (reason required).
> Mark fixed with `gsd-tools windows fixed <id>`.

| id | phase | kind | file | line | description | status | reason | recorded_at | resolved_at |
|----|-------|------|------|------|-------------|--------|--------|-------------|-------------|
| 1 | 02 | stub | src/cart.c |  | muhtari__cart_identify ignores the layouts argument: no slice hashes and no 128 KiB half-swapped hash yet; the identification plan that follows adds them | fixed |  | 2026-10-19T02:21:16.103Z | 2026-10-19T02:35:09.642Z |
| 2 | 03 | stub | src/maria.c | 68 | MARIA's DMA fetch floats for cartridge space until the board read is routed (plan 03-02) | fixed |  | 2026-10-19T23:56:08.197Z | 2026-10-20T00:14:40.459Z |
| 3 | 03 | stub | src/frame.c |  | With content loaded every frame row is written $00: MARIA renders no row until span rendering lands (plan 03-03) | fixed |  | 2026-10-20T00:16:07.865Z | 2026-10-20T00:33:28.582Z |
| 4 | 03 | todo | src/maria.c |  | A DLI set on zone 0's DLL entry, which VBE reads with no HALT window, raises no NMI; its timing waits for the diagnostic ROMs (MiSTer and A7800Hawk fire it after VBE) | fixed |  | 2026-10-20T00:33:28.665Z | 2026-10-20T02:01:16.233Z |
| 5 | 3 | stub | src/instance.c |  | muhtari_load copies a valid BIOS into the instance but still fast-boots; the BIOS boot itself is not run | fixed |  | 2026-10-20T00:49:04.503Z | 2026-10-20T03:55:25.400Z |
| 6 | 04 | deviation | tests/audio/test_spectral.c |  | Test-only spectral plan adds verified tests against existing production behavior without a formal RED/GREEN commit pair; no missing production behavior is claimed. | open |  | 2026-10-20T20:26:27.505Z |  |
| 7 | 04 | deviation | .planning/preparation/A7800-AUDIO.md |  | Corrected zero-start TIA duty and fill expectations after primary-source joint-state reconciliation | fixed |  | 2026-10-20T20:52:42.338Z | 2026-10-20T20:53:48.808Z |
| 8 | 04 | deviation | tests/frame/test_frame.c |  | WSYNC fixtures apply changed power-on profile through TIA reset before checking the stored epoch | fixed |  | 2026-10-20T20:52:42.414Z | 2026-10-20T20:53:48.887Z |
| 9 | 04 | unmet-truth | tests/audio/test_spectral.c | 472 | Wholly slow actual POKEY AUDF1 produces a 15136.23 Hz spur up to -78.28741 dBFS through the fixed kernel, failing the -80 dBFS spectral gate. | fixed |  | 2026-10-20T21:22:40.462Z | 2026-10-20T22:24:38.241Z |
| 10 | 05 | deviation | .github/workflows/be-equality.yml |  | Extracted read-only reusable equality because GitHub rejects the skipped nightly issue-write reporter in a read-only call. | fixed |  | 2026-10-21T18:32:19.333Z | 2026-10-21T18:32:24.687Z |
| 11 | 05 | unmet-truth | tests/cmake/abi.cmake |  | Full PowerPC inventory rejects __divdi3 and writable cartridge-database count symbols; same-head full inventory and equality are not verified. | fixed |  | 2026-10-21T18:32:19.409Z | 2026-10-21T20:21:17.820Z |
| 12 | 05 | unrun-verify | .github/workflows/be-equality.yml |  | PowerPC archive inventory and same-head equality cannot complete while the preceding full normal ABI inventory fails. | fixed |  | 2026-10-21T18:33:05.691Z | 2026-10-21T20:21:17.934Z |
| 13 | 05 | unmet-truth | .github/workflows/be-equality.yml | 66 | Complete native/PowerPC inventories pass, but hosted equality rejects relative evidence directories; absolute-path replay of the authentic pair passes. | fixed |  | 2026-10-21T19:22:57.834Z | 2026-10-21T20:21:18.042Z |
| 14 | 05 | unrun-verify | .github/workflows/be-equality.yml | 38 | Temporary target diagnostic remains; its removal and complete final-head equality proof are unfinished at the mandatory recovery halt. | fixed |  | 2026-10-21T19:22:57.900Z | 2026-10-21T20:21:18.153Z |
| 15 | 05 | unrun-verify | .github/workflows/ci.yml |  | Required typed bound adoption and hosted aggregate remain unfinished; Windows current/base producers are restored in run39699892851. | fixed | Task2 partial; Task3 unstarted. | 2026-10-21T23:40:11.664Z | 2026-10-22T01:53:34.511Z |
| 16 | 05 | unrun-verify | .github/workflows/ci.yml |  | Performance/compatibility store cutover, full ci/library workflows and hosted caller acceptance remain unverified after the measured allocation halt. | open |  | 2026-10-22T02:33:42.066Z |  |
| 17 | 05 | deviation | tests/cmake/quality.cmake |  | Central policy migration draft measures14888 against fixed C14853 and quality967 against allocation920; only RED is committed and cutover is halted. | open |  | 2026-10-22T02:33:42.157Z |  |
| 18 | 05 | deviation | tests/cmake/quality.cmake |  | Replacement05-11 named reader consolidation observes14879 and quality958; strict gate14841/quality920 misses38 lines, so Task1 and downstream cutover remain halted with no GREEN commit. | open |  | 2026-10-22T15:41:21.851Z |  |
| 19 | 05 | unrun-verify | .github/workflows/ci.yml |  | Replacement05-11 Task2/3 full ci/library, direct perf/library/quality, release-called CI and authentic artifact acceptance are unrun after Task1 allocation failure; no current run URL exists. | open |  | 2026-10-22T15:41:21.957Z |  |
| 20 | 05 | deviation | tests/cmake/quality.cmake |  | 05-12 Task1 halted at total14842/quality921, one above strict14841/quality920; unchanged other components and no GREEN. | open |  | 2026-10-22T17:00:29.052Z |  |
| 21 | 05 | unrun-verify | .planning/phases/05-pipeline-tune-up/05-12-PLAN.md |  | 05-12 Task2/3 full ci, generated-corpus library, POKEY audit and current direct/called hosted acceptance unrun after Task1 fit halt. | open |  | 2026-10-22T17:00:29.145Z |  |
| 22 | 05 | lint-warning | .github/workflows/be-equality.yml | 63 | 05-13 broad diff check exits2 on inherited blank EOF lines in be-equality.yml:63, gate-lines.txt:148 and historical05-01-SUMMARY.md:421; recovery diff check exits0 and historical/scope constraints leave the broad check unresolved. | open |  | 2026-10-22T18:49:34.389Z |  |
| 23 | 05 | unrun-verify | .github/workflows/ci.yml |  | Advisory release-called WASM run39832530253 attempt2/job120020998725 loses a pinned-tool fetch with curl35 before tests; its inventory and rehearsal/install/cleanup skip. Required called CI and quality plus authentic8 join pass; direct WASM passes. Overall Release remains failure; this is a tooling seed, not required recovery evidence. | open |  | 2026-10-22T18:53:28.751Z |  |
| 24 | 05 | unmet-truth | tests/policy/publish.cmake | 50 | Release signer RED reaches synthetic PATCH; Task1 is halted at14906 lines/fixes96 against strict14841/fixes30 before production repair. | open |  | 2026-10-22T20:05:27.962Z |  |
| 25 | 05 | unrun-verify | .planning/phases/05-pipeline-tune-up/05-08-PLAN.md |  | Release-safety GREEN/full CI/hosted rehearsal, complete lookup-error controls and dependent Tasks2-3 remain unrun after the Task1 fixed-allocation stop. | open |  | 2026-10-22T20:05:28.037Z |  |
| 26 | 05 | deviation | scripts/publish.sh |  | 05-14 Task2 construction halts at15152 physical lines: safety312/fixes342 exceed300/330 by12 and strict15141 by11; Task1 accepted, Task2 draft uncommitted,05-09 blocked. | open |  | 2026-10-22T21:17:55.944Z |  |
| 27 | 05 | unmet-truth | scripts/publish.sh | 162 | 05-14 draft api_response loses the empty204 header/body boundary through command substitution; the first successful cleanup queue control exits1 with invalid HTTP response. No Task2 GREEN or current release acceptance. | fixed |  | 2026-10-22T21:17:56.017Z | 2026-10-22T22:46:49.319Z |
| 28 | 05 | unrun-verify | .planning/phases/05-pipeline-tune-up/05-14-PLAN.md |  | 05-14 Task2 full matrix/provisioning and Task3 full local ci/library/inventories, current direct/called authentic9/8 joins, patched-gh rehearsal/install/confirmed cleanup and measured conservative cost remain unrun after the fixed-allocation stop. | open |  | 2026-10-22T21:17:56.089Z |  |
| 29 | 05 | deviation | tests/policy/publish.cmake |  | 05-15 Task2 halts at15231: safety391/fixes421 exceed384/414 by7 and strict15225 by6 before provisioning. Task1 accepted at15163;05-09 remains blocked; no further construction or GREEN. | open |  | 2026-10-22T22:46:49.123Z |  |
| 30 | 05 | unmet-truth | .github/workflows/release.yml |  | 05-15 real-script controls reach and reproduce missing verified patched CLI provisioning; CTest8 and RED_EVIDENCE_OK0. Pin/workflow/output repair and remaining complete-matrix audit are unimplemented at the allocation stop. | open |  | 2026-10-22T22:46:49.189Z |  |
| 31 | 05 | unrun-verify | .planning/phases/05-pipeline-tune-up/05-15-PLAN.md |  | 05-15 Task3 full local ci/library/inventories, current direct/called authentic9/8 joins, actual patched-gh rehearsal/publication/download/install/confirmed cleanup and cost/docs remain unrun after the designed Task2 halt. Accepted7/9 unchanged. | open |  | 2026-10-22T22:46:49.254Z |  |
| 32 | 06 | unmet-truth | .planning/phases/06-retail-cartridge-boards/06-01-PLAN.md |  | Complete disposable counted construction15952 exceeds fixed15479 by473; plan01 halted and02-09 blocked; no installed policy or board changes. | fixed |  | 2026-10-23T16:58:10.879Z | 2026-10-23T18:32:31.759Z |
| 33 | 06 | unrun-verify | .planning/phases/06-retail-cartridge-boards/06-01-PLAN.md |  | Task2 full CI and future six-native board/archive acceptance not run because Task1 complete counted fit failed; no completed requirement claimed. | open |  | 2026-10-23T16:58:10.951Z |  |
| 34 | 06 | deviation | tests/cart/test_cart.c |  | Updated the existing Absolute probe case from refusal to acceptance when the board became executable; resolved in27627f1. | fixed |  | 2026-10-23T19:41:17.397Z | 2026-10-23T19:41:21.710Z |
| 35 | 06 | deviation | tests/built/expected.txt |  | Advanced built/audio expectation revision metadata to3 without changing pinned hashes; resolved inf0e1659. | fixed |  | 2026-10-23T19:41:17.464Z | 2026-10-23T19:41:21.821Z |
| 36 | 06 | deviation | tests/header/header.c |  | API4 required updating C17 and C++17 header-only current-version assertions from3 to4; resolved in0e42dff. | fixed |  | 2026-10-23T19:45:55.060Z | 2026-10-23T19:46:54.036Z |
| 37 | 06 | deviation | tests/cart/test_cart.c |  | Activision warning vectors still expected unsupported after mapper acceptance; updated accepted status and assumed-layout warning, focused10 and full611+6 pass. | fixed |  | 2026-10-23T19:55:35.527Z | 2026-10-23T19:56:15.118Z |

````json
[
  {
    "id": 1,
    "kind": "stub",
    "phase": "02",
    "file": "src/cart.c",
    "line": null,
    "description": "muhtari__cart_identify ignores the layouts argument: no slice hashes and no 128 KiB half-swapped hash yet; the identification plan that follows adds them",
    "status": "fixed",
    "reason": "",
    "recorded_at": "2026-10-19T02:21:16.103Z",
    "resolved_at": "2026-10-19T02:35:09.642Z",
    "milestone": "v1.0"
  },
  {
    "id": 2,
    "kind": "stub",
    "phase": "03",
    "file": "src/maria.c",
    "line": 68,
    "description": "MARIA's DMA fetch floats for cartridge space until the board read is routed (plan 03-02)",
    "status": "fixed",
    "reason": "",
    "recorded_at": "2026-10-19T23:56:08.197Z",
    "resolved_at": "2026-10-20T00:14:40.459Z",
    "milestone": "v1.0"
  },
  {
    "id": 3,
    "kind": "stub",
    "phase": "03",
    "file": "src/frame.c",
    "line": null,
    "description": "With content loaded every frame row is written $00: MARIA renders no row until span rendering lands (plan 03-03)",
    "status": "fixed",
    "reason": "",
    "recorded_at": "2026-10-20T00:16:07.865Z",
    "resolved_at": "2026-10-20T00:33:28.582Z",
    "milestone": "v1.0"
  },
  {
    "id": 4,
    "kind": "todo",
    "phase": "03",
    "file": "src/maria.c",
    "line": null,
    "description": "A DLI set on zone 0's DLL entry, which VBE reads with no HALT window, raises no NMI; its timing waits for the diagnostic ROMs (MiSTer and A7800Hawk fire it after VBE)",
    "status": "fixed",
    "reason": "",
    "recorded_at": "2026-10-20T00:33:28.665Z",
    "resolved_at": "2026-10-20T02:01:16.233Z",
    "milestone": "v1.0"
  },
  {
    "id": 5,
    "kind": "stub",
    "phase": "3",
    "file": "src/instance.c",
    "line": null,
    "description": "muhtari_load copies a valid BIOS into the instance but still fast-boots; the BIOS boot itself is not run",
    "status": "fixed",
    "reason": "",
    "recorded_at": "2026-10-20T00:49:04.503Z",
    "resolved_at": "2026-10-20T03:55:25.400Z",
    "milestone": "v1.0"
  },
  {
    "id": 6,
    "kind": "deviation",
    "phase": "04",
    "file": "tests/audio/test_spectral.c",
    "line": null,
    "description": "Test-only spectral plan adds verified tests against existing production behavior without a formal RED/GREEN commit pair; no missing production behavior is claimed.",
    "status": "open",
    "reason": "",
    "recorded_at": "2026-10-20T20:26:27.505Z",
    "resolved_at": null,
    "milestone": "v1.0"
  },
  {
    "id": 7,
    "kind": "deviation",
    "phase": "04",
    "file": ".planning/preparation/A7800-AUDIO.md",
    "line": null,
    "description": "Corrected zero-start TIA duty and fill expectations after primary-source joint-state reconciliation",
    "status": "fixed",
    "reason": "",
    "recorded_at": "2026-10-20T20:52:42.338Z",
    "resolved_at": "2026-10-20T20:53:48.808Z",
    "milestone": "v1.0"
  },
  {
    "id": 8,
    "kind": "deviation",
    "phase": "04",
    "file": "tests/frame/test_frame.c",
    "line": null,
    "description": "WSYNC fixtures apply changed power-on profile through TIA reset before checking the stored epoch",
    "status": "fixed",
    "reason": "",
    "recorded_at": "2026-10-20T20:52:42.414Z",
    "resolved_at": "2026-10-20T20:53:48.887Z",
    "milestone": "v1.0"
  },
  {
    "id": 9,
    "kind": "unmet-truth",
    "phase": "04",
    "file": "tests/audio/test_spectral.c",
    "line": 472,
    "description": "Wholly slow actual POKEY AUDF1 produces a 15136.23 Hz spur up to -78.28741 dBFS through the fixed kernel, failing the -80 dBFS spectral gate.",
    "status": "fixed",
    "reason": "",
    "recorded_at": "2026-10-20T21:22:40.462Z",
    "resolved_at": "2026-10-20T22:24:38.241Z",
    "milestone": "v1.0"
  },
  {
    "id": 10,
    "kind": "deviation",
    "phase": "05",
    "file": ".github/workflows/be-equality.yml",
    "line": null,
    "description": "Extracted read-only reusable equality because GitHub rejects the skipped nightly issue-write reporter in a read-only call.",
    "status": "fixed",
    "reason": "",
    "recorded_at": "2026-10-21T18:32:19.333Z",
    "resolved_at": "2026-10-21T18:32:24.687Z",
    "milestone": "v2.0"
  },
  {
    "id": 11,
    "kind": "unmet-truth",
    "phase": "05",
    "file": "tests/cmake/abi.cmake",
    "line": null,
    "description": "Full PowerPC inventory rejects __divdi3 and writable cartridge-database count symbols; same-head full inventory and equality are not verified.",
    "status": "fixed",
    "reason": "",
    "recorded_at": "2026-10-21T18:32:19.409Z",
    "resolved_at": "2026-10-21T20:21:17.820Z",
    "milestone": "v2.0"
  },
  {
    "id": 12,
    "kind": "unrun-verify",
    "phase": "05",
    "file": ".github/workflows/be-equality.yml",
    "line": null,
    "description": "PowerPC archive inventory and same-head equality cannot complete while the preceding full normal ABI inventory fails.",
    "status": "fixed",
    "reason": "",
    "recorded_at": "2026-10-21T18:33:05.691Z",
    "resolved_at": "2026-10-21T20:21:17.934Z",
    "milestone": "v2.0"
  },
  {
    "id": 13,
    "kind": "unmet-truth",
    "phase": "05",
    "file": ".github/workflows/be-equality.yml",
    "line": 66,
    "description": "Complete native/PowerPC inventories pass, but hosted equality rejects relative evidence directories; absolute-path replay of the authentic pair passes.",
    "status": "fixed",
    "reason": "",
    "recorded_at": "2026-10-21T19:22:57.834Z",
    "resolved_at": "2026-10-21T20:21:18.042Z",
    "milestone": "v2.0"
  },
  {
    "id": 14,
    "kind": "unrun-verify",
    "phase": "05",
    "file": ".github/workflows/be-equality.yml",
    "line": 38,
    "description": "Temporary target diagnostic remains; its removal and complete final-head equality proof are unfinished at the mandatory recovery halt.",
    "status": "fixed",
    "reason": "",
    "recorded_at": "2026-10-21T19:22:57.900Z",
    "resolved_at": "2026-10-21T20:21:18.153Z",
    "milestone": "v2.0"
  },
  {
    "id": 15,
    "kind": "unrun-verify",
    "phase": "05",
    "file": ".github/workflows/ci.yml",
    "line": null,
    "description": "Required typed bound adoption and hosted aggregate remain unfinished; Windows current/base producers are restored in run39699892851.",
    "status": "fixed",
    "reason": "Task2 partial; Task3 unstarted.",
    "recorded_at": "2026-10-21T23:40:11.664Z",
    "resolved_at": "2026-10-22T01:53:34.511Z",
    "milestone": "v2.0"
  },
  {
    "id": 16,
    "kind": "unrun-verify",
    "phase": "05",
    "file": ".github/workflows/ci.yml",
    "line": null,
    "description": "Performance/compatibility store cutover, full ci/library workflows and hosted caller acceptance remain unverified after the measured allocation halt.",
    "status": "open",
    "reason": "",
    "recorded_at": "2026-10-22T02:33:42.066Z",
    "resolved_at": null,
    "milestone": "v2.0"
  },
  {
    "id": 17,
    "kind": "deviation",
    "phase": "05",
    "file": "tests/cmake/quality.cmake",
    "line": null,
    "description": "Central policy migration draft measures14888 against fixed C14853 and quality967 against allocation920; only RED is committed and cutover is halted.",
    "status": "open",
    "reason": "",
    "recorded_at": "2026-10-22T02:33:42.157Z",
    "resolved_at": null,
    "milestone": "v2.0"
  },
  {
    "id": 18,
    "kind": "deviation",
    "phase": "05",
    "file": "tests/cmake/quality.cmake",
    "line": null,
    "description": "Replacement05-11 named reader consolidation observes14879 and quality958; strict gate14841/quality920 misses38 lines, so Task1 and downstream cutover remain halted with no GREEN commit.",
    "status": "open",
    "reason": "",
    "recorded_at": "2026-10-22T15:41:21.851Z",
    "resolved_at": null,
    "milestone": "v2.0"
  },
  {
    "id": 19,
    "kind": "unrun-verify",
    "phase": "05",
    "file": ".github/workflows/ci.yml",
    "line": null,
    "description": "Replacement05-11 Task2/3 full ci/library, direct perf/library/quality, release-called CI and authentic artifact acceptance are unrun after Task1 allocation failure; no current run URL exists.",
    "status": "open",
    "reason": "",
    "recorded_at": "2026-10-22T15:41:21.957Z",
    "resolved_at": null,
    "milestone": "v2.0"
  },
  {
    "id": 20,
    "kind": "deviation",
    "phase": "05",
    "file": "tests/cmake/quality.cmake",
    "line": null,
    "description": "05-12 Task1 halted at total14842/quality921, one above strict14841/quality920; unchanged other components and no GREEN.",
    "status": "open",
    "reason": "",
    "recorded_at": "2026-10-22T17:00:29.052Z",
    "resolved_at": null,
    "milestone": "v2.0"
  },
  {
    "id": 21,
    "kind": "unrun-verify",
    "phase": "05",
    "file": ".planning/phases/05-pipeline-tune-up/05-12-PLAN.md",
    "line": null,
    "description": "05-12 Task2/3 full ci, generated-corpus library, POKEY audit and current direct/called hosted acceptance unrun after Task1 fit halt.",
    "status": "open",
    "reason": "",
    "recorded_at": "2026-10-22T17:00:29.145Z",
    "resolved_at": null,
    "milestone": "v2.0"
  },
  {
    "id": 22,
    "kind": "lint-warning",
    "phase": "05",
    "file": ".github/workflows/be-equality.yml",
    "line": 63,
    "description": "05-13 broad diff check exits2 on inherited blank EOF lines in be-equality.yml:63, gate-lines.txt:148 and historical05-01-SUMMARY.md:421; recovery diff check exits0 and historical/scope constraints leave the broad check unresolved.",
    "status": "open",
    "reason": "",
    "recorded_at": "2026-10-22T18:49:34.389Z",
    "resolved_at": null,
    "milestone": "v2.0"
  },
  {
    "id": 23,
    "kind": "unrun-verify",
    "phase": "05",
    "file": ".github/workflows/ci.yml",
    "line": null,
    "description": "Advisory release-called WASM run39832530253 attempt2/job120020998725 loses a pinned-tool fetch with curl35 before tests; its inventory and rehearsal/install/cleanup skip. Required called CI and quality plus authentic8 join pass; direct WASM passes. Overall Release remains failure; this is a tooling seed, not required recovery evidence.",
    "status": "open",
    "reason": "",
    "recorded_at": "2026-10-22T18:53:28.751Z",
    "resolved_at": null,
    "milestone": "v2.0"
  },
  {
    "id": 24,
    "kind": "unmet-truth",
    "phase": "05",
    "file": "tests/policy/publish.cmake",
    "line": 50,
    "description": "Release signer RED reaches synthetic PATCH; Task1 is halted at14906 lines/fixes96 against strict14841/fixes30 before production repair.",
    "status": "open",
    "reason": "",
    "recorded_at": "2026-10-22T20:05:27.962Z",
    "resolved_at": null,
    "milestone": "v2.0"
  },
  {
    "id": 25,
    "kind": "unrun-verify",
    "phase": "05",
    "file": ".planning/phases/05-pipeline-tune-up/05-08-PLAN.md",
    "line": null,
    "description": "Release-safety GREEN/full CI/hosted rehearsal, complete lookup-error controls and dependent Tasks2-3 remain unrun after the Task1 fixed-allocation stop.",
    "status": "open",
    "reason": "",
    "recorded_at": "2026-10-22T20:05:28.037Z",
    "resolved_at": null,
    "milestone": "v2.0"
  },
  {
    "id": 26,
    "kind": "deviation",
    "phase": "05",
    "file": "scripts/publish.sh",
    "line": null,
    "description": "05-14 Task2 construction halts at15152 physical lines: safety312/fixes342 exceed300/330 by12 and strict15141 by11; Task1 accepted, Task2 draft uncommitted,05-09 blocked.",
    "status": "open",
    "reason": "",
    "recorded_at": "2026-10-22T21:17:55.944Z",
    "resolved_at": null,
    "milestone": "v2.0"
  },
  {
    "id": 27,
    "kind": "unmet-truth",
    "phase": "05",
    "file": "scripts/publish.sh",
    "line": 162,
    "description": "05-14 draft api_response loses the empty204 header/body boundary through command substitution; the first successful cleanup queue control exits1 with invalid HTTP response. No Task2 GREEN or current release acceptance.",
    "status": "fixed",
    "reason": "",
    "recorded_at": "2026-10-22T21:17:56.017Z",
    "resolved_at": "2026-10-22T22:46:49.319Z",
    "milestone": "v2.0"
  },
  {
    "id": 28,
    "kind": "unrun-verify",
    "phase": "05",
    "file": ".planning/phases/05-pipeline-tune-up/05-14-PLAN.md",
    "line": null,
    "description": "05-14 Task2 full matrix/provisioning and Task3 full local ci/library/inventories, current direct/called authentic9/8 joins, patched-gh rehearsal/install/confirmed cleanup and measured conservative cost remain unrun after the fixed-allocation stop.",
    "status": "open",
    "reason": "",
    "recorded_at": "2026-10-22T21:17:56.089Z",
    "resolved_at": null,
    "milestone": "v2.0"
  },
  {
    "id": 29,
    "kind": "deviation",
    "phase": "05",
    "file": "tests/policy/publish.cmake",
    "line": null,
    "description": "05-15 Task2 halts at15231: safety391/fixes421 exceed384/414 by7 and strict15225 by6 before provisioning. Task1 accepted at15163;05-09 remains blocked; no further construction or GREEN.",
    "status": "open",
    "reason": "",
    "recorded_at": "2026-10-22T22:46:49.123Z",
    "resolved_at": null,
    "milestone": "v2.0"
  },
  {
    "id": 30,
    "kind": "unmet-truth",
    "phase": "05",
    "file": ".github/workflows/release.yml",
    "line": null,
    "description": "05-15 real-script controls reach and reproduce missing verified patched CLI provisioning; CTest8 and RED_EVIDENCE_OK0. Pin/workflow/output repair and remaining complete-matrix audit are unimplemented at the allocation stop.",
    "status": "open",
    "reason": "",
    "recorded_at": "2026-10-22T22:46:49.189Z",
    "resolved_at": null,
    "milestone": "v2.0"
  },
  {
    "id": 31,
    "kind": "unrun-verify",
    "phase": "05",
    "file": ".planning/phases/05-pipeline-tune-up/05-15-PLAN.md",
    "line": null,
    "description": "05-15 Task3 full local ci/library/inventories, current direct/called authentic9/8 joins, actual patched-gh rehearsal/publication/download/install/confirmed cleanup and cost/docs remain unrun after the designed Task2 halt. Accepted7/9 unchanged.",
    "status": "open",
    "reason": "",
    "recorded_at": "2026-10-22T22:46:49.254Z",
    "resolved_at": null,
    "milestone": "v2.0"
  },
  {
    "id": 32,
    "kind": "unmet-truth",
    "phase": "06",
    "file": ".planning/phases/06-retail-cartridge-boards/06-01-PLAN.md",
    "line": null,
    "description": "Complete disposable counted construction15952 exceeds fixed15479 by473; plan01 halted and02-09 blocked; no installed policy or board changes.",
    "status": "fixed",
    "reason": "",
    "recorded_at": "2026-10-23T16:58:10.879Z",
    "resolved_at": "2026-10-23T18:32:31.759Z",
    "milestone": "v2.0"
  },
  {
    "id": 33,
    "kind": "unrun-verify",
    "phase": "06",
    "file": ".planning/phases/06-retail-cartridge-boards/06-01-PLAN.md",
    "line": null,
    "description": "Task2 full CI and future six-native board/archive acceptance not run because Task1 complete counted fit failed; no completed requirement claimed.",
    "status": "open",
    "reason": "",
    "recorded_at": "2026-10-23T16:58:10.951Z",
    "resolved_at": null,
    "milestone": "v2.0"
  },
  {
    "id": 34,
    "kind": "deviation",
    "phase": "06",
    "file": "tests/cart/test_cart.c",
    "line": null,
    "description": "Updated the existing Absolute probe case from refusal to acceptance when the board became executable; resolved in27627f1.",
    "status": "fixed",
    "reason": "",
    "recorded_at": "2026-10-23T19:41:17.397Z",
    "resolved_at": "2026-10-23T19:41:21.710Z",
    "milestone": "v2.0"
  },
  {
    "id": 35,
    "kind": "deviation",
    "phase": "06",
    "file": "tests/built/expected.txt",
    "line": null,
    "description": "Advanced built/audio expectation revision metadata to3 without changing pinned hashes; resolved inf0e1659.",
    "status": "fixed",
    "reason": "",
    "recorded_at": "2026-10-23T19:41:17.464Z",
    "resolved_at": "2026-10-23T19:41:21.821Z",
    "milestone": "v2.0"
  },
  {
    "id": 36,
    "kind": "deviation",
    "phase": "06",
    "file": "tests/header/header.c",
    "line": null,
    "description": "API4 required updating C17 and C++17 header-only current-version assertions from3 to4; resolved in0e42dff.",
    "status": "fixed",
    "reason": "",
    "recorded_at": "2026-10-23T19:45:55.060Z",
    "resolved_at": "2026-10-23T19:46:54.036Z",
    "milestone": "v2.0"
  },
  {
    "id": 37,
    "kind": "deviation",
    "phase": "06",
    "file": "tests/cart/test_cart.c",
    "line": null,
    "description": "Activision warning vectors still expected unsupported after mapper acceptance; updated accepted status and assumed-layout warning, focused10 and full611+6 pass.",
    "status": "fixed",
    "reason": "",
    "recorded_at": "2026-10-23T19:55:35.527Z",
    "resolved_at": "2026-10-23T19:56:15.118Z",
    "milestone": "v2.0"
  }
]
````
