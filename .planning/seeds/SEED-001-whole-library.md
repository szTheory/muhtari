---
id: SEED-001
status: dormant
planted: 2026-10-15
planted_during: preparation, before milestone v1.0
trigger_when: milestone v1.0 has shipped (games play with sound in RetroArch) and the next milestone is about which games and peripherals work
scope: large
---

# SEED-001: The whole library plays — every board, every peripheral, saves and PAL (v2.0)

## Why This Matters

After v1.0, linear, SuperGame and POKEY cartridges run: 195 of the 204 titles in MAME's CC0 list and 105 of the
110 commercial releases. The rest need Activision and Absolute banking, the XG-1 light gun (10 titles), paddles
and other controllers, High Score Cartridge and SaveKey saves, and full PAL. Saves and peripherals are what players
notice first, and homebrew uses BankSet and Souper boards the commercial list never shows.

## When to Surface

**Trigger:** v1.0 shipped and the next milestone is about library breadth. Opens with SEED-007 (the tune-up), which
this time also builds the weekly triage digest, the monthly CI-efficiency report and the `quality.json` ratchet.

## Scope Estimate

**Large** — a milestone. One phase per thing a player can see: banking (Activision with its two image layouts
normalised by hash, Absolute, Fractalus, BankSet, Souper, EXRAM); saves (HSC 2 KB from the user's ROM or a stub,
SaveKey 32 KB, a versioned `.srm`, runner `--save-dir` with atomic writes and on-demand flush, hosted save round
trip); the light gun and the other controllers at pin level; full PAL timing, palette and hand-off; the
compatibility ledger over the whole CC0 list, after which the libretro `.info` drops `is_experimental`.

## Breadcrumbs

- `.planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md` sections 1–5: header precedence, boards and their
  library share, the Activision layout problem, controllers, light-gun model, HSC/SaveKey, the commercial list.
- `.planning/preparation/HOSTS-AND-INTEGRATION.md` sections 1 and 4: `.srm` layout, light-gun and paddle input,
  the runner's save options, Playstead's save contract.
- `.planning/preparation/CONFORMANCE.md` section 5: the ledger and its ratchet.
- `.planning/preparation/A7800-SYSTEM.md`: PAL timing and hand-off state.

## Notes

BankSet and Souper depend on the exact HALT phase; schedule them after the v1.0 DMA timing has held for a phase.
