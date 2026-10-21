---
schema_version: 1
open_count: 2
waived_count: 0
fixed_count: 13
total_count: 15
last_updated: 2026-10-21T23:40:11.664Z
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
| 15 | 05 | unrun-verify | .github/workflows/ci.yml |  | Required hosted quality aggregate and all-lane baselines remain unverified: pre-existing Windows digest control fails before quality observations. | open |  | 2026-10-21T23:40:11.664Z |  |

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
    "description": "Required hosted quality aggregate and all-lane baselines remain unverified: pre-existing Windows digest control fails before quality observations.",
    "status": "open",
    "reason": "",
    "recorded_at": "2026-10-21T23:40:11.664Z",
    "resolved_at": null,
    "milestone": "v2.0"
  }
]
````
