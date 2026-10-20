---
schema_version: 1
open_count: 1
waived_count: 0
fixed_count: 2
total_count: 3
last_updated: 2026-10-20T00:16:07.865Z
---

# Broken Windows Ledger

> Cross-phase defect register. With `workflow.windows_enforce` enabled, `/gsd-ship` blocks while `open_count > 0`.
> Waive with `gsd-tools windows waive <id> "<reason>"` (reason required).
> Mark fixed with `gsd-tools windows fixed <id>`.

| id | phase | kind | file | line | description | status | reason | recorded_at | resolved_at |
|----|-------|------|------|------|-------------|--------|--------|-------------|-------------|
| 1 | 02 | stub | src/cart.c |  | muhtari__cart_identify ignores the layouts argument: no slice hashes and no 128 KiB half-swapped hash yet; the identification plan that follows adds them | fixed |  | 2026-10-19T02:21:16.103Z | 2026-10-19T02:35:09.642Z |
| 2 | 03 | stub | src/maria.c | 68 | MARIA's DMA fetch floats for cartridge space until the board read is routed (plan 03-02) | fixed |  | 2026-10-19T23:56:08.197Z | 2026-10-20T00:14:40.459Z |
| 3 | 03 | stub | src/frame.c |  | With content loaded every frame row is written $00: MARIA renders no row until span rendering lands (plan 03-03) | open |  | 2026-10-20T00:16:07.865Z |  |

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
    "status": "open",
    "reason": "",
    "recorded_at": "2026-10-20T00:16:07.865Z",
    "resolved_at": null,
    "milestone": "v1.0"
  }
]
````
