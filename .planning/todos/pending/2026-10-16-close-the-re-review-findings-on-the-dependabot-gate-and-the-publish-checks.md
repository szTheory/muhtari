---
created: 2026-10-16T22:10:00.000Z
title: Close the re-review's findings on the Dependabot gate and the publish checks
area: ci
severity: minor
files:
  - .github/workflows/dependabot-automerge.yml
  - docs/maintaining.md
  - scripts/publish.sh
  - docs/testing.md
audit_acknowledged:
  milestone: v1.0
  at: 2026-10-21
---

## Problem

The re-review after phase 1's first fix round (01-REVIEW.md, 2026-10-16) left three release and CI warnings and one
docs gap open. All four touch files the phase's verification covers, so they wait for a pull request after the
phase merges:

- WR-01: auto-merge that Dependabot's own run armed stays armed after a person pushes to the branch, and a re-run
  keeps `github.actor` as Dependabot, so `docs/maintaining.md`'s "never merges unattended" claims more than the
  workflow does.
- WR-02: `publish.sh rehearsal-delete` reads a failed release lookup as "gone", so cleanup can pass on an API error.
- WR-03: `publish.sh attested`, the check before the publish that cannot be undone, pins `--source-ref` but not
  `--signer-workflow`; only the later `verify` checks the signer.
- IN-02: `docs/testing.md` does not list `docs.api_names` or its self test, Negative controls included.

WR-04 (no boundary test at the 4096-byte size tag) and the remaining info items are test and tooling seeds under
AGENTS rule 7; 01-REVIEW-DISPOSITION.md keeps them open.

## Solution

One `ci:` pull request after the phase merges; its own rehearsal exercises the publish.sh changes:

- gate the merge on `github.triggering_actor` too, pass `--match-head-commit` to `gh pr merge`, and turn auto-merge
  off when anyone but Dependabot pushes, or narrow the claim in docs/maintaining.md to what the gate does;
- make `release_id` and the other lookups tell a 404 from a failed request;
- pass `--signer-workflow` in `attested` as `verify` does;
- list the check in docs/testing.md.
