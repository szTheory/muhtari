---
id: SEED-010
status: dormant
planted: 2026-10-28
planted_during: phase 10 final acceptance
trigger_when: Phase 12 close; next milestone tune-up if unchanged; earlier with complete comparable evidence
scope: small
audit_acknowledged:
  milestone: v2.0
  at: 2026-10-29
  status: dormant
---

# Nightly property job exceeds its deadlines

## Property calibration deferral

The current `prop` selection has eight registrations. The six Phase 12 properties
remain the TUNE-07 cohort: `split_run`, `peek_pure`, `instances_isolated`,
`input_at_vblank`, `battery_lifecycle` and `battery_observers`. The later state
additions are `state_continuation` and `state_refusal`; any new current-lane
measurement must report all eight. The nightly lane uses four workers, 10,000
cases per property, a run-ID seed, `MUHTARI_TIMEOUT_SCALE=10` (390 seconds per
registered 39-second test) and a nine-minute job cap. It remains `continue-on-error`
and outside `report.needs`.

Complete evidence must match source, run, attempt, runner and workload identities;
the exact selected tests and required fixtures; successful CTest/JUnit with no
missing, duplicate, skipped, errored or failed entries; and completed 10,000-case
observations with the recorded seed for every selected property. A terminal job,
five original successes or ordinary 100-case CI is not a cohort. The historical
390.08-second isolation timeout in Nightly 40157750598 and cancellations
40419374584 and 40702203633 are censored and unaccepted. Earlier `split_run`
passes at 276.8 and 383.6 seconds also do not establish a complete cohort. Under
DEC.47, only complete successes calibrate deadlines; do not change the bound or
reduce workload from these receipts.

This is an allowed explicit deferral, not a scheduled experiment or promotion.
Revisit it at Phase 12 close; if unchanged, at the next milestone tune-up; or
earlier when complete qualifying comparable evidence is supplied. The close
revisit remains with the blocked Phase 12 acceptance plan. No case, seed, fixture,
oracle, timeout, advisory status or report routing changes here.
