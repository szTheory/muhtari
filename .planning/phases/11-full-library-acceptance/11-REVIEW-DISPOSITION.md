---
phase: 11
review: 11-REVIEW.md
titles: json
findings:
  - id: CR-01
    severity: critical
    disposition: open
    title: "The sample-backed claim check never runs for `basic.*` tests, so most of its entries are unchecked"
  - id: CR-02
    severity: critical
    disposition: open
    title: "A `;` in `OUT` splits the `rm -rf` argument and deletes an unrelated directory"
  - id: WR-01
    severity: warning
    disposition: open
    title: "An `OUT` that is an existing directory passes the writability probe and then leaks the full path"
  - id: WR-02
    severity: warning
    disposition: open
    title: "The library preset now silently skips symlinked files, and its failure message and docs do not say so"
  - id: WR-03
    severity: warning
    disposition: open
    title: "A root name containing `*` or `?` is accepted and matches other directories"
  - id: IN-01
    severity: info
    disposition: open
    title: "`library_source_fault` is bypassed by `file (GLOB ...)` with a space before the parenthesis"
  - id: IN-02
    severity: info
    disposition: open
    title: "The budget evidence is a local macOS run and an unreferenced \"full ci run\""
  - id: IN-03
    severity: info
    disposition: open
    title: "The image maximum is a bare literal in two places, and `local.cmake` does not check that `DIR` exists"
  - id: IN-04
    severity: info
    disposition: open
    title: "Unused outputs of the new scans"
  - id: WR-04
    severity: warning
    disposition: open
  - id: WR-05
    severity: warning
    disposition: open
  - id: WR-06
    severity: warning
    disposition: open
  - id: IN-05
    severity: info
    disposition: open
  - id: IN-06
    severity: info
    disposition: open
  - id: IN-07
    severity: info
    disposition: open
  - id: IN-08
    severity: info
    disposition: open
open: 16
total: 16
recorded: 2026-10-29T16:43:22.289Z
---

# Phase 11: Code Review Disposition

| Finding | Severity | Disposition | Source |
|---------|----------|-------------|--------|
| CR-01 | critical | open | - |
| CR-02 | critical | open | - |
| WR-01 | warning | open | - |
| WR-02 | warning | open | - |
| WR-03 | warning | open | - |
| IN-01 | info | open | - |
| IN-02 | info | open | - |
| IN-03 | info | open | - |
| IN-04 | info | open | - |
| WR-04 | warning | open | contradicts phase disclaimer (not in the current review) |
| WR-05 | warning | open | SuperGame names title.simple.fast (not in the current review) |
| WR-06 | warning | open | title.pal.fast, title.pin.*, title.local.fixture (not in the current review) |
| IN-05 | info | open | - (not in the current review) |
| IN-06 | info | open | - (not in the current review) |
| IN-07 | info | open | - (not in the current review) |
| IN-08 | info | open | - (not in the current review) |

Dispositions: `open` (recorded, not yet triaged), `fixed`, `skipped`, `deferred`.
Set `deferred` by hand and put the reason in the Source cell; both are preserved. A `|` in the reason is kept as prose and escaped on the next run.
Re-running the gate keeps every row it can. A row the current review no longer reports is kept and its Source cell flagged, so a finding does not leave this record silently. ONE exception: when a finding id is REUSED by a different finding, the earlier decision cannot keep a row — the id is taken — and it is dropped. A RECORDED decision (anything but `open`) is named on the console when that happens; a row still at `open` is replaced silently, because `open` records no decision to lose.
