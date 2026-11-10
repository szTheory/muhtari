---
id: SEED-013
status: dormant
planted: 2026-10-29
planted_during: phase 11 full library acceptance
trigger_when: someone asks to see local-run results next to the ledger counts
scope: small
audit_acknowledged:
  milestone: v2.0
  at: 2026-10-29
  status: dormant
---

# Locally observed section in the compatibility doc

Add a doc section generated from the local-only `build/library/local.csv` that shows what a contributor's own dumps did, kept outside the ratchet and outside the committed ledger. It must be regenerated, never committed with dump-derived rows, so hygiene still finds no personal or dump data in the tree.

This is an optional improvement under AGENTS rule 7, not a blocker for any phase.
