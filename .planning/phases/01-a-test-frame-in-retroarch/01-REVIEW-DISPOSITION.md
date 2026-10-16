---
phase: 01
review: 01-REVIEW.md
titles: json
findings:
  - id: CR-01
    severity: critical
    disposition: fixed
    title: "`--bench N` overflows the sample buffer on 32-bit targets [product]"
  - id: WR-01
    severity: warning
    disposition: fixed
    title: "The size tag has no upper bound, so a garbage tag reads out of bounds and loops up to 4 GiB [product]"
  - id: WR-02
    severity: warning
    disposition: fixed
    title: "The docs say `muhtari_run_frame` always writes `audio_count`, but it does not [product]"
  - id: WR-03
    severity: warning
    disposition: fixed
    title: "Pull-request rehearsals produce attestations the release checks cannot tell apart from release attestations [release/CI]"
  - id: WR-04
    severity: warning
    disposition: fixed
    title: "Dependabot action updates auto-merge with no cooldown into workflows that hold the App key and the attestation identity [release/CI]"
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
  - id: IN-01
    severity: info
    disposition: open
    title: "Wrong window size in the zlib header comment [product]"
  - id: IN-02
    severity: info
    disposition: open
    title: "The bench silently reports nothing, or saturated times, when its inputs go bad [product]"
  - id: IN-03
    severity: info
    disposition: open
    title: "`argc == 0` is reported as out of memory [product]"
  - id: IN-04
    severity: info
    disposition: open
    title: "The App client ID is defined twice [release/CI]"
  - id: IN-05
    severity: info
    disposition: open
    title: "`check_junit.cmake` matches labels as an unanchored regex [test/tooling]"
  - id: IN-06
    severity: info
    disposition: open
    title: "The presets require CMake 4.3 but `CMakeLists.txt` declares 3.25 [release/CI]"
open: 6
total: 15
recorded: 2026-10-16T21:00:03.837Z
---

# Phase 01: Code Review Disposition

| Finding | Severity | Disposition | Source |
|---------|----------|-------------|--------|
| CR-01 | critical | fixed | 01-REVIEW-FIX.md |
| WR-01 | warning | fixed | 01-REVIEW-FIX.md |
| WR-02 | warning | fixed | 01-REVIEW-FIX.md |
| WR-03 | warning | fixed | 01-REVIEW-FIX.md |
| WR-04 | warning | fixed | 01-REVIEW-FIX.md |
| WR-05 | warning | fixed | 01-REVIEW-FIX.md |
| WR-06 | warning | fixed | 01-REVIEW-FIX.md |
| WR-07 | warning | fixed | 01-REVIEW-FIX.md |
| WR-08 | warning | fixed | 01-REVIEW-FIX.md |
| IN-01 | info | open | - |
| IN-02 | info | open | - |
| IN-03 | info | open | - |
| IN-04 | info | open | - |
| IN-05 | info | open | - |
| IN-06 | info | open | - |

Dispositions: `open` (recorded, not yet triaged), `fixed`, `skipped`, `deferred`.
Set `deferred` by hand and put the reason in the Source cell; both are preserved. A `|` in the reason is kept as prose and escaped on the next run.
Re-running the gate keeps every row it can. A row the current review no longer reports is kept and its Source cell flagged, so a finding does not leave this record silently. ONE exception: when a finding id is REUSED by a different finding, the earlier decision cannot keep a row — the id is taken — and it is dropped. A RECORDED decision (anything but `open`) is named on the console when that happens; a row still at `open` is replaced silently, because `open` records no decision to lose.
