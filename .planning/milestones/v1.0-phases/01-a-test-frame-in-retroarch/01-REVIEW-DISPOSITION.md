---
phase: 01
review: 01-REVIEW.md
titles: json
findings:
  - id: WR-01
    severity: warning
    disposition: open
    title: "The Dependabot actor gate does not stop a person's commit from merging unattended"
  - id: WR-02
    severity: warning
    disposition: open
    title: "`rehearsal_delete` still reports success when the lookup call fails"
  - id: WR-03
    severity: warning
    disposition: open
    title: "The pre-publish `attested` gate is weaker than the post-publish `verify` gate"
  - id: WR-04
    severity: warning
    disposition: open
    title: "The documented 4096-byte size-tag limit has no boundary test"
  - id: IN-01
    severity: info
    disposition: open
    title: "The size-tag cap's comment claims more than the cap does"
  - id: IN-02
    severity: info
    disposition: open
    title: "The new guide check and its self test are missing from `docs/testing.md`"
  - id: IN-03
    severity: info
    disposition: open
    title: "`api_names.cmake` relies on CMake 4's policy defaults for `IN_LIST`"
  - id: IN-04
    severity: info
    disposition: open
    title: "The `api_names` self test leaves the `_`-prefix branch untested, and header comments count as declarations"
  - id: IN-05
    severity: info
    disposition: open
    title: "One embedding-guide paragraph mixes `muhtari_get_info` and `muhtari_run_frame`"
  - id: CR-01
    severity: critical
    disposition: fixed
    title: "`--bench N` overflows the sample buffer on 32-bit targets [product]"
  - id: WR-05
    severity: warning
    disposition: fixed
    title: "The \"planning only\" classification can skip CI for a pull request with more than 3,000 files [release/CI]"
  - id: WR-06
    severity: warning
    disposition: fixed
    title: "Dependabot auto-merge is gated on the pull request's author, not on who triggered the run [release/CI]"
  - id: WR-07
    severity: warning
    disposition: fixed
    title: "`publish.sh verify` downloads from `releases/latest/` for real tags, so a re-run of an older release fails or checks the wrong release [release/CI]"
  - id: WR-08
    severity: warning
    disposition: fixed
    title: "`release_id` reads only the first 100 releases, so rehearsal cleanup can report success while a release remains [release/CI]"
  - id: IN-06
    severity: info
    disposition: open
    title: "The presets require CMake 4.3 but `CMakeLists.txt` declares 3.25 [release/CI]"
open: 10
total: 15
recorded: 2026-10-16T21:56:13.840Z
---

# Phase 01: Code Review Disposition

| Finding | Severity | Disposition | Source |
|---------|----------|-------------|--------|
| WR-01 | warning | open | - |
| WR-02 | warning | open | - |
| WR-03 | warning | open | - |
| WR-04 | warning | open | - |
| IN-01 | info | open | - |
| IN-02 | info | open | - |
| IN-03 | info | open | - |
| IN-04 | info | open | - |
| IN-05 | info | open | - |
| CR-01 | critical | fixed | 01-REVIEW-FIX.md (not in the current review) |
| WR-05 | warning | fixed | 01-REVIEW-FIX.md (not in the current review) |
| WR-06 | warning | fixed | 01-REVIEW-FIX.md (not in the current review) |
| WR-07 | warning | fixed | 01-REVIEW-FIX.md (not in the current review) |
| WR-08 | warning | fixed | 01-REVIEW-FIX.md (not in the current review) |
| IN-06 | info | open | - (not in the current review) |

Dispositions: `open` (recorded, not yet triaged), `fixed`, `skipped`, `deferred`.
Set `deferred` by hand and put the reason in the Source cell; both are preserved. A `|` in the reason is kept as prose and escaped on the next run.
Re-running the gate keeps every row it can. A row the current review no longer reports is kept and its Source cell flagged, so a finding does not leave this record silently. ONE exception: when a finding id is REUSED by a different finding, the earlier decision cannot keep a row — the id is taken — and it is dropped. A RECORDED decision (anything but `open`) is named on the console when that happens; a row still at `open` is replaced silently, because `open` records no decision to lose.
