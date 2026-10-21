---
created: 2026-10-16T18:30:00.000Z
title: Limit release rehearsals to pushes that change release inputs
area: ci
resolves_phase: 5
severity: minor
files:
  - .github/workflows/release.yml
audit_acknowledged:
  milestone: v1.0
  at: 2026-10-21
---

## Problem

The `pull_request` paths filter on `release.yml` matches the whole pull request's diff, not the push. Once a pull
request touches a release input, every later push to it runs a full rehearsal: the nested CI plus about 4 minutes of
publishing, and one public prerelease created and deleted. Plan 01-17 measured this on pull request #6. It is safe,
and it stays for now: such pull requests are rare after phase 1, and a narrower filter adds gate code.

## Solution

Revisit if the runner time or the release notifications become a nuisance. Options:

- compare the push's own `before..after` paths in a first job and skip the rehearsal when no release input changed;
- run the rehearsal only on a pull request with a label.
