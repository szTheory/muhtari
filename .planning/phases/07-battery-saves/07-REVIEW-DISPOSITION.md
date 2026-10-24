---
phase: 07
review: 07-REVIEW.md
titles: json
findings:
  - id: CR-03
    severity: critical
    disposition: open
    title: "Resolving Auto to unsupported header codes rejects the whole frontend input update"
  - id: CR-01
    severity: critical
    disposition: fixed
    title: "Broken stdout pipe terminates the runner before its required final save"
  - id: CR-02
    severity: critical
    disposition: fixed
    title: "A refused late save-device request drops joystick A-button fire"
open: 1
total: 3
recorded: 2026-10-24T14:05:06.692Z
---

# Phase 07: Code Review Disposition

| Finding | Severity | Disposition | Source |
|---------|----------|-------------|--------|
| CR-03 | critical | open | - |
| CR-01 | critical | fixed | 07-REVIEW-FIX.md (not in the current review) |
| CR-02 | critical | fixed | 07-REVIEW-FIX.md (not in the current review) |

Dispositions: `open` (recorded, not yet triaged), `fixed`, `skipped`, `deferred`.
Set `deferred` by hand and put the reason in the Source cell; both are preserved. A `|` in the reason is kept as prose and escaped on the next run.
Re-running the gate keeps every row it can. A row the current review no longer reports is kept and its Source cell flagged, so a finding does not leave this record silently. ONE exception: when a finding id is REUSED by a different finding, the earlier decision cannot keep a row — the id is taken — and it is dropped. A RECORDED decision (anything but `open`) is named on the console when that happens; a row still at `open` is replaced silently, because `open` records no decision to lose.
