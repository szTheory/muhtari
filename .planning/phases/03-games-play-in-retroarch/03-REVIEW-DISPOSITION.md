---
phase: 03
review: 03-REVIEW.md
titles: json
findings:
  - id: WR-01
    severity: warning
    disposition: fixed
    title: "A headerless image matched by hash skips the board geometry check, and the banking code assumes it passed"
  - id: WR-02
    severity: warning
    disposition: fixed
    title: "The public header's contract comments contradict the behaviour a host will see"
  - id: WR-03
    severity: warning
    disposition: fixed
    title: "When the libretro core fails a frame, nothing reaches the screen or the frontend's frame pacing"
  - id: WR-04
    severity: warning
    disposition: fixed
    title: "The libretro BIOS lookup gives up early and misreports failures"
  - id: WR-05
    severity: warning
    disposition: fixed
    title: "A planning ID in core source breaks the project rule"
  - id: IN-01
    severity: info
    disposition: open
    title: "The latch for the first BIOS-boot frame is detected by a coincidence of lengths"
  - id: IN-02
    severity: info
    disposition: open
    title: "`muhtari__riot_peek` duplicates `muhtari__riot_read`'s dispatch"
  - id: IN-03
    severity: info
    disposition: open
    title: "A movie's `bios-sha256` is ignored silently when the movie boots fast"
  - id: IN-04
    severity: info
    disposition: open
    title: "The libretro core falls back to the PAL BIOS on an NTSC-only machine"
  - id: IN-05
    severity: info
    disposition: open
    title: "The fuzz run target's bound is justified empirically, not structurally"
open: 5
total: 10
recorded: 2026-10-20T15:55:37.859Z
---

# Phase 03: Code Review Disposition

| Finding | Severity | Disposition | Source |
|---------|----------|-------------|--------|
| WR-01 | warning | fixed | 03-REVIEW-FIX.md |
| WR-02 | warning | fixed | 03-REVIEW-FIX.md |
| WR-03 | warning | fixed | 03-REVIEW-FIX.md |
| WR-04 | warning | fixed | 03-REVIEW-FIX.md |
| WR-05 | warning | fixed | 03-REVIEW-FIX.md |
| IN-01 | info | open | - |
| IN-02 | info | open | - |
| IN-03 | info | open | - |
| IN-04 | info | open | - |
| IN-05 | info | open | - |

Dispositions: `open` (recorded, not yet triaged), `fixed`, `skipped`, `deferred`.
Set `deferred` by hand and put the reason in the Source cell; both are preserved. A `|` in the reason is kept as prose and escaped on the next run.
Re-running the gate keeps every row it can. A row the current review no longer reports is kept and its Source cell flagged, so a finding does not leave this record silently. ONE exception: when a finding id is REUSED by a different finding, the earlier decision cannot keep a row — the id is taken — and it is dropped. A RECORDED decision (anything but `open`) is named on the console when that happens; a row still at `open` is replaced silently, because `open` records no decision to lose.
