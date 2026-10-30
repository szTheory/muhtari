---
created: 2026-10-29T13:00:00.000Z
title: Keep the phase 11 first review's info findings whose ids the gap-round review reused
area: tests
severity: minor
files:
  - tools/compat_doc.cmake
  - tests/compat/local.cmake
  - tests/cmake/claims_wording.cmake
audit_acknowledged:
  milestone: v2.0
  at: 2026-10-29
---

## Problem

The gap-round review of phase 11 (11-REVIEW.md as committed in 443cdeb, 2026-10-29) restarted numbering at CR-01,
WR-01 and IN-01, so the disposition ledger (efb1285) replaced these rows from the first review (11-REVIEW.md as
committed in b21f31d). The gap round resolved the first review's CR-01 and WR-01 to WR-03; its WR-04 to WR-06 and
IN-05 to IN-08 are still in the ledger under their own ids. These four info findings were open and are no longer
recorded anywhere else:

- IN-01 (first review): the "N of N accounted for" headline on the compatibility page cannot report a gap.
- IN-02 (first review): commas in titles become a double space on the compatibility page.
- IN-03 (first review): the local driver header disagrees with the code, and skipped rows read as absent.
- IN-04 (first review): the wording deny list is substring-based and skips subdirectories.

## Solution

Triage each one when the phase 11 review findings are next dispositioned. Info findings are optional under AGENTS
rule 7, so each becomes a seed or is closed with a reason. The gap-round review's IN-03 also touches the local
driver, so check whether its fix covers the first review's IN-03.
