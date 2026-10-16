---
status: complete
phase: 01-a-test-frame-in-retroarch
source: [01-VERIFICATION.md]
started: 2026-10-16T20:45:00Z
updated: 2026-10-16T21:47:05.690Z
---

## Current Test

[testing complete]

## Tests

### 1. Read docs/runner.md's "## Test card" section next to the card (muhtari-run --frames 1 --dump-frame 1:card.png) and decide whether a user can tell what each element checks
expected: Each element (corner block, border, overscan lines, 16x16 colour chart, the two checkers, red and green bands) is described clearly and correctly
result: pass
source: automated
evidence: "The owner asked for this check to be automated (2026-10-16). Every position, size and colour the section states matches card.png from muhtari-run --frames 1 --dump-frame 1:card.png pixel by pixel: the top-left block, the four border edges, $08 on rows 8 and 231, 256 single-colour 14x12 cells at rows 25-216 and columns 48-271 with cell (r, c) = $rc, both checkers, $46 and $C6 bands (decoding probe, exit 0); 960:847 follows from 320x242 at 6:7. The agent read the section against the card and found each element's purpose stated plainly."

### 2. Follow the README's "Quick start: RetroArch" steps in RetroArch 1.22.2 on the Mac, and read README.md and docs/embedding.md as a new user and as an embedder
expected: The menu names (Main Menu, Load Core, "Atari - 7800 (muhtari)", Start Core) match what RetroArch shows, and the embedding guide is accurate and complete
result: pass
source: automated
evidence: "Main Menu, Load Core and Start Core are each exact strings in the RetroArch 1.22.2 binary on this Mac; the README's core name equals the .info display_name; the .info and the core (SET_SUPPORT_NO_GAME) declare no-game support, which is what offers Start Core. CI run https://github.com/szTheory/muhtari/actions/runs/38971166983 (success on 24cf9fc): the retroarch job ran RetroArch 1.22.2 on macos-15 with the archive's core and no content and its screenshot equals the runner's frame. The new docs.api_names check (cmake --workflow --preset ci, exit 0) failed on the guide as it stood: five public names were never mentioned, so the guide now covers them. The flags muhtari.pc resolves to build and run tests/consumer (exit 0). The agent checked every other claim in both files against the code and tests."

### 3. Ship-step confirmation of the two post-merge items in 01-VERIFICATION.md behavior_unverified_items (release-please PR auto-merged, v0.1.0 published with 19 verified assets, install-line-latest green)
expected: All jobs of the push Release run on main succeed; record the run URL
result: skipped
reason: "Deferred follow-up: not a human check. Only the merge can show it, so the ship step confirms the push Release run on main from its run URL (AGENTS rules 6 and 9); 01-VERIFICATION.md carries both truths as overrides."

## Summary

total: 3
passed: 2
issues: 0
pending: 0
skipped: 1
blocked: 0

## Gaps

## Deferred Follow-Ups

- test: 3
  idea: "Confirm at ship that the push Release run on main succeeds: the release pull request auto-merged, v0.1.0 published with 19 verified assets, install-line-latest green; record the run URL"
  deferred_at: 2026-10-16
