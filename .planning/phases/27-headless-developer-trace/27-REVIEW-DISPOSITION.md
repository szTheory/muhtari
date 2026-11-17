---
phase: 27
review: 27-REVIEW.md
titles: json
findings:
  - id: WR-01
    severity: warning
    disposition: deferred
    title: "Extended Windows paths are compared with ordinary path semantics"
open: 0
total: 1
recorded: 2026-11-17T17:18:46.140Z
---

# Phase 27: Code Review Disposition

| Finding | Severity | Disposition | Source |
|---------|----------|-------------|--------|
| WR-01 | warning | deferred | Explicit `\\?\` paths preserve trailing suffixes, but the current collision check strips them. The check fails closed and rejects only certain path pairs; no overwrite is possible. Extended namespace paths are not documented as a runner contract. See the follow-up seed for the Windows-runtime test and support decision. (not in the current review) |

Dispositions: `open` (recorded, not yet triaged), `fixed`, `skipped`, `deferred`.
Set `deferred` by hand and put the reason in the Source cell; both are preserved. A `|` in the reason is kept as prose and escaped on the next run.
Re-running the gate keeps every row it can. A row the current review no longer reports is kept and its Source cell flagged, so a finding does not leave this record silently. ONE exception: when a finding id is REUSED by a different finding, the earlier decision cannot keep a row — the id is taken — and it is dropped. A RECORDED decision (anything but `open`) is named on the console when that happens; a row still at `open` is replaced silently, because `open` records no decision to lose.
