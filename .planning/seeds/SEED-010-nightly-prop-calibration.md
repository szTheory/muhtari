---
id: SEED-010
status: dormant
planted: 2026-10-28
planted_during: phase 10 final acceptance
trigger_when: next milestone tune-up, or the nightly property job is promoted from advisory
scope: small
audit_acknowledged:
  milestone: v2.0
  at: 2026-10-29
  status: dormant
---

# Nightly property job exceeds its deadlines

The nightly `prop` job runs the four-way parallel 10,000-case property tests under
`MUHTARI_TIMEOUT_SCALE=10` with a nine-minute job limit. Since the BankSet and
expansion seeds joined the run pool, `prop.peek_pure`, `prop.instances_isolated` and
`prop.input_at_vblank` reach their 390 s scaled deadlines and the job is cancelled
(Nightly runs 40419374584 and 40702203633; `split_run` passed at 276.8 s and 383.6 s).
The job is `continue-on-error` and outside the report's needs, so it stays advisory.

Do not raise a deadline from a censored run (DEC.47). Take one complete hosted run with
a generous job limit, then set each test's deadline and the job limit from that
measurement, or reduce the per-case image size if the pool no longer pays for itself.
