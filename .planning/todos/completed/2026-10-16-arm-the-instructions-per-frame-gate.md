---
created: 2026-10-16T19:00:00.000Z
title: Arm the instructions-per-frame gate
area: ci
resolves_phase: 5
closed: 2026-10-22
severity: minor
files:
  - .github/workflows/ci.yml
  - .github/workflows/nightly.yml
  - docs/testing.md
audit_acknowledged:
  milestone: v1.0
  at: 2026-10-21
---

## Problem

The `perf` job in `ci.yml` reports only: `MUHTARI_PERF_ARMED` is `"false"`, so a pull request more than 1.00 %
slower than its merge base still merges. Arming needs evidence that only runs after the phase pull request merges:
`workflow_dispatch` fires only for workflow files on the default branch (research R-01), and until the first merge
every direct pull request run reports `NO_BASE`, because `main` has no `CMakePresets.json`. The nightly issue
writer has also not yet been seen writing and closing the `nightly` issue.

## Solution

After the phase pull request merges:

1. Dispatch `ci.yml` on `main` twice (`gh workflow run ci.yml --ref main`). Each `perf` job measures one commit
   three times; both job summaries must show `spread=0`.
2. Confirm that a pull request opened after the merge shows a numeric `base=` line in its `perf` job, not
   `verdict=NO_BASE`.
3. Open one `ci:` pull request that sets `MUHTARI_PERF_ARMED` to `"true"` in `ci.yml` and updates the
   "Performance" section of `docs/testing.md`; cite the two control runs in its body.
4. Dispatch `nightly.yml` once with `force_failure=true` (`gh workflow run nightly.yml --ref main -f
   force_failure=true`) and confirm one open issue labelled `nightly` names the run; then dispatch it once
   normally and confirm that issue closes.

## Phase 5 resolution

Resolved in Phase 5 plan07: two fresh zero-spread triples, a numeric PR base and actual nightly failure/recovery precede arming. Current controlled test-card comparison is ARMED/PASS. Final verification confirms these commands and run URLs; no gameplay performance claim is added.
