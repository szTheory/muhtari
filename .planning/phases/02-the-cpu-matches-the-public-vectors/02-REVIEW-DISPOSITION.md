---
phase: 02
review: 02-REVIEW.md
titles: json
findings:
  - id: WR-01
    severity: warning
    disposition: fixed
    title: "A HALT window or NMI edge that is ignored is dropped without a word, and the caller cannot tell"
  - id: WR-02
    severity: warning
    disposition: skipped
    title: "`cpu.jam.step_reads_ffff` cannot catch a jammed step that polls for an NMI"
  - id: IN-01
    severity: info
    disposition: deferred
    title: "The JAM doc says a frame still ends, but no frame loop runs the CPU yet"
  - id: WR-03
    severity: warning
    disposition: fixed
    title: "A second HALT request or NMI edge silently replaces one still in flight"
  - id: WR-04
    severity: warning
    disposition: fixed
    title: "`muhtari__cpu_step` does not enforce the jammed invariant, and a jammed CPU advances no time"
  - id: IN-02
    severity: info
    disposition: deferred
    title: "The cartdb generator accepts a `reload` slice whose size differs from the chip it repeats"
  - id: IN-03
    severity: info
    disposition: deferred
    title: "The firmware scan cannot recognise multi-chip or half-swapped 7800 images"
open: 0
total: 7
recorded: 2026-10-19T20:15:17.290Z
---

# Phase 02: Code Review Disposition

| Finding | Severity | Disposition | Source |
|---------|----------|-------------|--------|
| WR-01 | warning | fixed | 02-REVIEW-FIX.md |
| WR-02 | warning | skipped | 02-REVIEW-FIX.md |
| IN-01 | info | deferred | todo 2026-10-19-close-the-phase-2-re-review-findings-when-maria-and-the-run-loop-land.md: waits for phase 3's run loop |
| WR-03 | warning | fixed | 02-REVIEW-FIX.md as committed in 99482b2 (not in the current review) |
| WR-04 | warning | fixed | 02-REVIEW-FIX.md as committed in 99482b2 (not in the current review) |
| IN-02 | info | deferred | todo 2026-10-19-close-the-phase-2-review-seeds-on-the-nightly-filter-and-the-rom-tables.md (not in the current review) |
| IN-03 | info | deferred | todo 2026-10-19-close-the-phase-2-review-seeds-on-the-nightly-filter-and-the-rom-tables.md (not in the current review) |

Dispositions: `open` (recorded, not yet triaged), `fixed`, `skipped`, `deferred`.
Set `deferred` by hand and put the reason in the Source cell; both are preserved. A `|` in the reason is kept as prose and escaped on the next run.
Re-running the gate keeps every row it can. A row the current review no longer reports is kept and its Source cell flagged, so a finding does not leave this record silently. ONE exception: when a finding id is REUSED by a different finding, the earlier decision cannot keep a row — the id is taken — and it is dropped. A RECORDED decision (anything but `open`) is named on the console when that happens; a row still at `open` is replaced silently, because `open` records no decision to lose.
