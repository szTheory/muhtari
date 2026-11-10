---
id: SEED-014
status: dormant
planted: 2026-10-29
planted_during: phase 11 full library acceptance
trigger_when: the ledger or its judge is edited in a way a pull request base check would have caught, or the ratchet is relied on outside pull requests
scope: small
audit_acknowledged:
  milestone: v2.0
  at: 2026-10-29
  status: dormant
---

# Ratchet protection beyond pull-request bases

The ratchet judges a pull request's results against the base ledger, using the pull request's own `tests/cmake/compat_ratchet.cmake`. Two gaps remain. On push events `MUHTARI_LEDGER_BASE` is empty, so no base ledger is compared. And the judge comes from the pull request rather than the base commit, so a pull request that edits the judge also edits the rule it is held to; the baseline checkout is not built.

Hardening: on push events take the base from the previous commit on the branch, and run the judge from the base commit checkout. Weigh this against a pull request that must legitimately change the judge, which needs a reviewed, dated path.

This is an optional improvement under AGENTS rule 7, not a blocker for any phase.
