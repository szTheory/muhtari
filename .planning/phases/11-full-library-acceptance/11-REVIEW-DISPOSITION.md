---
phase: 11-full-library-acceptance
review: 11-REVIEW.md
recorded: 2026-10-29
open: 15 of 15
---

# Phase 11: Review Disposition

Each finding defaults to `open` until a fix round (`/gsd-code-review 11 --fix`) records `fixed` or `skipped`.

| ID | Title | Disposition | Note |
|----|-------|-------------|------|
| CR-01 | title.pal.bios 2 s timeout is below observed PAL+POKEY BIOS cost | open | timed out 3+ times locally under load; precedent raised to 4 s |
| WR-01 | local pass leaks full paths and miscounts on dangling symlink, unreadable file, `;` or `[` in names | open | breaks the no-path contract of the local run |
| WR-02 | local.cmake rm -rf on path derived from unchecked OUT | open | bare file name targets /local |
| WR-03 | diag.bankset and diag.bankset_tracer race on one output tree; phase added a reader | open | race predates phase (949fc61), phase widened it |
| WR-04 | compatibility page labels title-frame hashes "public gameplay" | open | contradicts phase disclaimer |
| WR-05 | claims table backing test mismatch and gameplay classes with no backing test | open | SuperGame names title.simple.fast |
| WR-06 | new test budgets with no measurement | open | title.pal.fast, title.pin.*, title.local.fixture |
| IN-01 | "N of N accounted for" headline cannot report a gap | open | |
| IN-02 | commas in titles become a double space | open | |
| IN-03 | local driver header disagrees with code; skipped rows read as absent | open | |
| IN-04 | wording deny list is substring-based and skips subdirectories | open | |
| IN-05 | .info description length check passes silently on no regex match | open | |
| IN-06 | docs/testing.md lists a test under the wrong label | open | |
| IN-07 | expansion CRC check could stop running unnoticed | open | |
| IN-08 | generator guard without a control | open | |
