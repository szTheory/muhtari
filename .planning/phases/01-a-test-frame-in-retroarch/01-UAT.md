---
status: testing
phase: 01-a-test-frame-in-retroarch
source: [01-VERIFICATION.md]
started: 2026-10-16T20:45:00Z
updated: 2026-10-16T20:45:00Z
---

## Current Test

number: 1
name: Read docs/runner.md's "## Test card" section next to the card (muhtari-run --frames 1 --dump-frame 1:card.png) and decide whether a user can tell what each element checks
expected: |
  Each element (corner block, border, overscan lines, 16x16 colour chart, the two checkers, red and green bands) is described clearly and correctly
awaiting: user response

## Tests

### 1. Read docs/runner.md's "## Test card" section next to the card (muhtari-run --frames 1 --dump-frame 1:card.png) and decide whether a user can tell what each element checks
expected: Each element (corner block, border, overscan lines, 16x16 colour chart, the two checkers, red and green bands) is described clearly and correctly
result: [pending]

### 2. Follow the README's "Quick start: RetroArch" steps in RetroArch 1.22.2 on the Mac, and read README.md and docs/embedding.md as a new user and as an embedder
expected: The menu names (Main Menu, Load Core, "Atari - 7800 (muhtari)", Start Core) match what RetroArch shows, and the embedding guide is accurate and complete
result: [pending]

### 3. Ship-step confirmation of the two post-merge items in 01-VERIFICATION.md behavior_unverified_items (release-please PR auto-merged, v0.1.0 published with 19 verified assets, install-line-latest green)
expected: All jobs of the push Release run on main succeed; record the run URL
result: [pending]

## Summary

total: 3
passed: 0
issues: 0
pending: 3
skipped: 0
blocked: 0

## Gaps
