---
schema_version: 1
open_count: 0
waived_count: 0
fixed_count: 1
total_count: 1
last_updated: 2026-10-19T02:35:09.642Z
---

# Broken Windows Ledger

> Cross-phase defect register. With `workflow.windows_enforce` enabled, `/gsd-ship` blocks while `open_count > 0`.
> Waive with `gsd-tools windows waive <id> "<reason>"` (reason required).
> Mark fixed with `gsd-tools windows fixed <id>`.

| id | phase | kind | file | line | description | status | reason | recorded_at | resolved_at |
|----|-------|------|------|------|-------------|--------|--------|-------------|-------------|
| 1 | 02 | stub | src/cart.c |  | muhtari__cart_identify ignores the layouts argument: no slice hashes and no 128 KiB half-swapped hash yet; the identification plan that follows adds them | fixed |  | 2026-10-19T02:21:16.103Z | 2026-10-19T02:35:09.642Z |

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
  }
]
````
