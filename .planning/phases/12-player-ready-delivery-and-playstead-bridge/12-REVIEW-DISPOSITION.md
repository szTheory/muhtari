---
phase: 12
review: 12-REVIEW.md
titles: json
findings:
  - id: WR-01
    severity: warning
    disposition: fixed
    title: "Partial trace rollback and same-directory retry under owner-only umask"
  - id: WR-02
    severity: warning
    disposition: fixed
    title: "Generated history cartridge is not consumed by any test"
  - id: IN-01
    severity: info
    disposition: fixed
    title: "History response codes are duplicated across translation units"
  - id: CR-01
    severity: critical
    disposition: fixed
    title: "Wall-timeout child can be reported as successful"
  - id: CR-02
    severity: critical
    disposition: fixed
    title: "Monotonic-clock failure can leave the launched child alive"
open: 0
total: 5
recorded: 2026-11-09T23:29:53Z
---

# Phase 12: Code Review Disposition

| Finding | Severity | Disposition | Source |
|---------|----------|-------------|--------|
| WR-01 | warning | fixed | Rollback plus restrictive-umask retry verified in 12-23-LOCAL-EVIDENCE.md (not in the current review) |
| WR-02 | warning | fixed | db134c4 (not in the current review) |
| IN-01 | info | fixed | db134c4 (not in the current review) |
| CR-01 | critical | fixed | Local fix and red/green evidence in 12-23-LOCAL-EVIDENCE.md (not in the current review) |
| CR-02 | critical | fixed | Local fix and red/green evidence in 12-23-LOCAL-EVIDENCE.md (not in the current review) |

Dispositions: `open` (recorded, not yet triaged), `fixed`, `skipped`, `deferred`.
Set `deferred` by hand and put the reason in the Source cell; both are preserved. A `|` in the reason is kept as prose and escaped on the next run.
Re-running the gate keeps every row it can. A row the current review no longer reports is kept and its Source cell flagged, so a finding does not leave this record silently. ONE exception: when a finding id is REUSED by a different finding, the earlier decision cannot keep a row — the id is taken — and it is dropped. A RECORDED decision (anything but `open`) is named on the console when that happens; a row still at `open` is replaced silently, because `open` records no decision to lose.
