---
phase: 02
review: 02-REVIEW.md
titles: json
findings:
  - id: WR-01
    severity: warning
    disposition: skipped
    title: "Nightly pull-request path filter misses inputs of the vectors-full and loader-fuzz jobs"
  - id: WR-02
    severity: warning
    disposition: fixed
    title: "The size-tag cap does not stop reads past the caller's object, as its comment says it does"
  - id: WR-03
    severity: warning
    disposition: fixed
    title: "A second HALT request or NMI edge silently replaces one still in flight"
  - id: WR-04
    severity: warning
    disposition: fixed
    title: "`muhtari__cpu_step` does not enforce the jammed invariant, and a jammed CPU advances no time"
  - id: IN-01
    severity: info
    disposition: open
    title: "Stale value comment on `muhtari_versions.api`"
  - id: IN-02
    severity: info
    disposition: open
    title: "The cartdb generator accepts a `reload` slice whose size differs from the chip it repeats"
  - id: IN-03
    severity: info
    disposition: open
    title: "The firmware scan cannot recognise multi-chip or half-swapped 7800 images"
open: 3
total: 7
recorded: 2026-10-19T18:54:37.412Z
---

# Phase 02: Code Review Disposition

| Finding | Severity | Disposition | Source |
|---------|----------|-------------|--------|
| WR-01 | warning | skipped | 02-REVIEW-FIX.md |
| WR-02 | warning | fixed | 02-REVIEW-FIX.md |
| WR-03 | warning | fixed | 02-REVIEW-FIX.md |
| WR-04 | warning | fixed | 02-REVIEW-FIX.md |
| IN-01 | info | open | - |
| IN-02 | info | open | - |
| IN-03 | info | open | - |

Dispositions: `open` (recorded, not yet triaged), `fixed`, `skipped`, `deferred`.
Set `deferred` by hand and put the reason in the Source cell; both are preserved. A `|` in the reason is kept as prose and escaped on the next run.
Re-running the gate keeps every row it can. A row the current review no longer reports is kept and its Source cell flagged, so a finding does not leave this record silently. ONE exception: when a finding id is REUSED by a different finding, the earlier decision cannot keep a row — the id is taken — and it is dropped. A RECORDED decision (anything but `open`) is named on the console when that happens; a row still at `open` is replaced silently, because `open` records no decision to lose.
