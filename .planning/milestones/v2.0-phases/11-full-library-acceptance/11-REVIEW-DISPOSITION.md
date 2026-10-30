---
phase: 11
review: 11-REVIEW.md
titles: json
findings:
  - id: CR-01
    severity: critical
    disposition: fixed
    title: "The sample-backed claim check never runs for `basic.*` tests, so most of its entries are unchecked"
  - id: CR-02
    severity: critical
    disposition: fixed
    title: "A `;` in `OUT` splits the `rm -rf` argument and deletes an unrelated directory"
  - id: WR-01
    severity: warning
    disposition: fixed
    title: "An `OUT` that is an existing directory passes the writability probe and then leaks the full path"
  - id: WR-02
    severity: warning
    disposition: fixed
    title: "The library preset now silently skips symlinked files, and its failure message and docs do not say so"
  - id: WR-03
    severity: warning
    disposition: fixed
    title: "A root name containing `*` or `?` is accepted and matches other directories"
  - id: IN-01
    severity: info
    disposition: deferred
    title: "`library_source_fault` is bypassed by `file (GLOB ...)` with a space before the parenthesis"
  - id: IN-02
    severity: info
    disposition: deferred
    title: "The budget evidence is a local macOS run and an unreferenced \"full ci run\""
  - id: IN-03
    severity: info
    disposition: deferred
    title: "The image maximum is a bare literal in two places, and `local.cmake` does not check that `DIR` exists"
  - id: IN-04
    severity: info
    disposition: deferred
    title: "Unused outputs of the new scans"
  - id: WR-04
    severity: warning
    disposition: fixed
  - id: WR-05
    severity: warning
    disposition: fixed
  - id: WR-06
    severity: warning
    disposition: fixed
  - id: IN-05
    severity: info
    disposition: deferred
  - id: IN-06
    severity: info
    disposition: deferred
  - id: IN-07
    severity: info
    disposition: deferred
  - id: IN-08
    severity: info
    disposition: deferred
open: 0
total: 16
recorded: 2026-10-29T17:05:51.143Z
---

# Phase 11: Code Review Disposition

| Finding | Severity | Disposition | Source |
|---------|----------|-------------|--------|
| CR-01 | critical | fixed | 11-REVIEW-FIX.md |
| CR-02 | critical | fixed | 11-REVIEW-FIX.md |
| WR-01 | warning | fixed | 11-REVIEW-FIX.md |
| WR-02 | warning | fixed | 11-REVIEW-FIX.md |
| WR-03 | warning | fixed | 11-REVIEW-FIX.md |
| IN-01 | info | deferred | Optional equivalent-spelling source-check hardening; 11-12 accepts the current spelling and records its limit. |
| IN-02 | info | deferred | Provisional local measurements remain identified in 11-11-SUMMARY; hosted calibration stays a ship observation. |
| IN-03 | info | deferred | Optional constant consolidation and missing-DIR direct-script hardening; documented preset and current fixture pass. |
| IN-04 | info | deferred | WR-02 now consumes the aggregate unread count; removing the unused per-directory output is optional cleanup. |
| WR-04 | warning | fixed | 11-10 title-screen wording; fresh 11-12 page freshness, wording and library checks pass. |
| WR-05 | warning | fixed | 11-10 claim tests plus e4356c6 basic/sample token guard; fresh 11-12 controls pass. |
| WR-06 | warning | fixed | 11-11 measured budgets and PAL BIOS 4 s floor remain; fresh 11-12 full CI passes. |
| IN-05 | info | deferred | Original b21f31d: optional malformed-description spelling hardening; current generated .info consumers pass. |
| IN-06 | info | deferred | Original b21f31d: optional docs label-map cleanup; current required inventories and docs checks pass. |
| IN-07 | info | deferred | Original b21f31d: optional future loop-change CRC-call guard; current canonical CRC consumers pass. |
| IN-08 | info | deferred | Original b21f31d: optional dedicated evidence-field fault control; current generator rejection guard remains. |

Dispositions: `open` (recorded, not yet triaged), `fixed`, `skipped`, `deferred`.
Set `deferred` by hand and put the reason in the Source cell; both are preserved. A `|` in the reason is kept as prose and escaped on the next run.
Re-running the gate keeps every row it can. A row the current review no longer reports is kept and its Source cell flagged, so a finding does not leave this record silently. ONE exception: when a finding id is REUSED by a different finding, the earlier decision cannot keep a row — the id is taken — and it is dropped. A RECORDED decision (anything but `open`) is named on the console when that happens; a row still at `open` is replaced silently, because `open` records no decision to lose.

## Acceptance disposition, 2026-10-29

This is disposition of the existing two reviews, not another review round. The
original nine gap-review findings (two critical, three warning, four info) and
seven carried first-review rows remain above. Fresh 11-12 acceptance supplies
commands, exits, source identity and N/A-local URLs. CR-01/02 and WR-01..03 stay
fixed with their existing fix-report provenance; WR-04..06 now have acceptance
references instead of stale open rows. No required-contract violation is waived.
All eight remaining info rows are deferred optional improvements; there are zero
untriaged rows. WR-02 retains its helper/source-control evidence boundary; a
missing-file library configure failure was not forced end to end. Hosted outcomes
remain for ship. The historical 14/16 failed verification remains in Git and is
replaced only by execute-phase's verifier after accepting this receipt.

The first review reused IN-01..04 identifiers later assigned to different gap
findings. Preserve their original identities from b21f31d separately, as captured
in the existing pending todo `2026-10-29-keep-the-phase-11-first-review-info-findings.md`:

| Historical identity | Disposition | Reason |
|---------------------|-------------|--------|
| b21f31d IN-01, accounted headline | deferred | Optional presentation simplification; independent pinned XML derivation and exact ledger checks remain authoritative. |
| b21f31d IN-02, doubled title spacing | deferred | Cosmetic spacing normalization; no entry identity or evidence classification changes. |
| b21f31d IN-03, local header/skipped counts | deferred | Optional direct-driver wording/count taxonomy; current pinned population and isolated counted public acceptance remain explicit. |
| b21f31d IN-04, wording deny-list scope | deferred | Optional alternate wording and nested-document coverage; current published claims were judged against evidence and required wording tests pass. |

These identities do not change the current review's nine-finding count or the
16-row carried ledger. They remain deferred in the existing todo rather than
being silently replaced by the gap review's reused numbers. This bounded
reconciliation adds no source change, audit round, test, resource bound or owner
verification request.
