---
id: SEED-011
status: dormant
planted: 2026-10-29
planted_during: phase 11 full library acceptance
trigger_when: a milestone that asks for peripheral or save evidence beyond the boot predicates
scope: small
audit_acknowledged:
  milestone: v2.0
  at: 2026-10-29
  status: dormant
---

# Movie-driven independent-consumer device checks

Drive the CC0 samples with scripted input movies and judge them with an independent consumer of the core's output: mousedemo, snescontrollers and mega7800controllers for the controller devices, and hiscoretable for SaveKey writes. The ledger already has the `movie:<file>/` predicate prefix for scripted input.

A sample that still fails after one investigation narrows its device to "owned model only" in the docs rather than growing into a research task.

This is an optional improvement under AGENTS rule 7, not a blocker for any phase.
