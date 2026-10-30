---
phase: 10
review: 10-REVIEW.md
titles: json
findings:
  - id: WR-03
    severity: warning
    disposition: open
    title: "Documented \"accepted, so exit 1\" branch for a 52 KiB BankSet is untested"
  - id: IN-05
    severity: info
    disposition: open
    title: "`load()` grew another nesting level and duplicates its cleanup"
  - id: IN-06
    severity: info
    disposition: open
    title: "The `--hsc-rom` table cell is a single 900-character paragraph"
  - id: CR-01
    severity: critical
    disposition: fixed
  - id: WR-01
    severity: warning
    disposition: open
  - id: WR-02
    severity: warning
    disposition: open
  - id: IN-01
    severity: info
    disposition: open
  - id: IN-02
    severity: info
    disposition: open
  - id: IN-03
    severity: info
    disposition: open
  - id: IN-04
    severity: info
    disposition: open
open: 9
total: 10
recorded: 2026-10-28T16:18:12.286Z
---

# Phase 10: Code Review Disposition

| Finding | Severity | Disposition | Source |
|---------|----------|-------------|--------|
| WR-03 | warning | open | - |
| IN-05 | info | open | - |
| IN-06 | info | open | - |
| CR-01 | critical | fixed | fixed in e8a35a5 (runner asks the library for the load decision and HSC attachment before reading firmware; runner.hsc_rom.refused covers it); resolution confirmed by the e45590c re-review |
| WR-01 | warning | open | runner/main.c and libretro/libretro.c report every UNSUPPORTED_DEVICE as an unsupported sound configuration (not in the current review) |
| WR-02 | warning | open | scripts/hygiene.sh by-name history scan still flags a deleted listed ROM; control test uses a non-ROM name (not in the current review) |
| IN-01 | info | open | tests/fuzz/run_seeds.c hard-coded seed index thresholds (not in the current review) |
| IN-02 | info | open | tests/diag/driver.c reads the last character of an unchecked name (not in the current review) |
| IN-03 | info | open | CI identity configuration line is a constant (not in the current review) |
| IN-04 | info | open | refused 52 KiB BankSet plus HSC reports only a board refusal (not in the current review) |

Dispositions: `open` (recorded, not yet triaged), `fixed`, `skipped`, `deferred`.
Set `deferred` by hand and put the reason in the Source cell; both are preserved. A `|` in the reason is kept as prose and escaped on the next run.
Re-running the gate keeps every row it can. A row the current review no longer reports is kept and its Source cell flagged, so a finding does not leave this record silently. ONE exception: when a finding id is REUSED by a different finding, the earlier decision cannot keep a row — the id is taken — and it is dropped. A RECORDED decision (anything but `open`) is named on the console when that happens; a row still at `open` is replaced silently, because `open` records no decision to lose.
