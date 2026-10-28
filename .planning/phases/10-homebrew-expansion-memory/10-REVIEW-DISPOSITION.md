# Phase 10: Code Review Disposition

Review: 10-REVIEW.md (8b76704). Every finding is open until a fix round or a recorded decision resolves it.

| Finding | Severity | Disposition | Source |
|---------|----------|-------------|--------|
| CR-01 | critical | open | runner/main.c:1195 exits 1 instead of the documented refused status 77 for a refused image with --hsc-rom; confirmed by reading the code |
| WR-01 | warning | open | runner/main.c and libretro/libretro.c report every UNSUPPORTED_DEVICE as an unsupported sound configuration |
| WR-02 | warning | open | scripts/hygiene.sh by-name history scan still flags a deleted listed ROM; control test uses a non-ROM name |
| IN-01 | info | open | tests/fuzz/run_seeds.c hard-coded seed index thresholds |
| IN-02 | info | open | tests/diag/driver.c reads the last character of an unchecked name |
| IN-03 | info | open | CI identity configuration line is a constant |
| IN-04 | info | open | refused 52 KiB BankSet plus HSC reports only a board refusal |
