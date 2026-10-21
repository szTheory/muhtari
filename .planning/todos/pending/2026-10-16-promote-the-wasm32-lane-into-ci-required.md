---
created: 2026-10-16T17:10:44.000Z
title: Promote the wasm32 lane into CI required
area: ci
severity: minor
files:
  - .github/workflows/ci.yml
  - .planning/phases/01-a-test-frame-in-retroarch/01-CONTEXT.md
audit_acknowledged:
  milestone: v1.0
  at: 2026-10-21
---

## Problem

The `wasm32` job in `ci.yml` is report-only: it is outside the needs of `hash-equality` and `CI required`, and
`hash-equality` deletes `hashes-wasm32` before it compares seven files. As the owner chose (D-22), the lane reports
only for its first 10 runs on `main`. Until it is promoted, a pull request that breaks the wasm32 build or changes its
hashes still merges.

## Solution

After 10 green `wasm32` runs on `main`, one `ci:` pull request (D-22):

- adds `wasm32` to the `needs` of `hash-equality` and of `CI required`;
- drops the `rm -rf hashes/hashes-wasm32` line and raises `-DCOUNT=7` to `-DCOUNT=8`, so `hash-equality` compares
  eight files and asserts that count, and a lane that stops uploading cannot drop out silently;
- removes `-DREPORT_ONLY=wasm32` from `policy.ci_required` in `tests/hygiene/tests.cmake`;
- updates the job table and the roll-up paragraph in `docs/testing.md`.
