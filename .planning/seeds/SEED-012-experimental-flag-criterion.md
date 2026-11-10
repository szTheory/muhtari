---
id: SEED-012
status: dormant
planted: 2026-10-29
planted_during: phase 11 full library acceptance
trigger_when: a milestone defines pinned-list (commercial) evidence, or the ratchet has passing rows for most evaluable pinned-list entries
scope: small
audit_acknowledged:
  milestone: v2.0
  at: 2026-10-29
  status: dormant
---

# Criterion for flipping is_experimental

The libretro `.info` keeps `is_experimental` until a stated criterion is met. Phase 11 accounts every pinned entry as not evaluated or skipped, so it supplies no pinned-list (commercial) evidence.

Candidate criterion: a ratio of passing rows over the evaluable pinned-list rows, with zero `fail` rows among them, measured by the existing ledger and judged by the existing ratchet. When the criterion holds, the flip is a one-line change to the `.info` and its test. Pick the ratio from a measurement, not in advance.

This is an optional improvement under AGENTS rule 7, not a blocker for any phase.
