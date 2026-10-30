# Project Retrospective

## Milestone: v1.0 — 7800 games play with sound in RetroArch

**Shipped:** 2026-10-21
**Phases:** 4 | **Plans:** 73 | **Recorded tasks:** 149

### What Was Built

- Shipped an embeddable C17 library, headless runner and libretro core with automated attested releases and one-line RetroArch installation.
- Matched all 2,560,000 public 6502 vectors and implemented bounded SALLY bus timing, RIOT and validated cartridge loading.
- Rendered linear and SuperGame cartridges with measured MARIA DMA timing, diagnostic ROMs and cross-platform frame hashes.
- Integrated fast boot, CC0 OpenBIOS, controller movies, achievement memory maps and automated RetroArch gameplay checks.
- Delivered integer TIA and POKEY sound with canonical raw/PCM hashes, libretro callback parity and clipping-free sample pins.
- Verified the production 64-phase Q23 synthesiser against spectral and alias controls, with bounded allocation-free frame execution.

### What Worked

Preset-driven local/hosted verification, actual adapter/runner parity and negative
controls exposed integration faults without owner UAT. Source-derived diagnostics
and canonical hashes made timing and platform differences reproducible. App-driven
release publication and the latest-install check closed ship-time evidence.

### What Was Inefficient

Sequential audio plans shared files and required many wave invocations until the
whole-phase command was used. Later covered-file edits invalidated earlier phase
fingerprints. Hosted workload growth exceeded provisional CI timeouts. Gate-code
proportion remained over budget, and a shared test output caused a freshness race.

### Patterns Established

Three deliverables, integer instance state, bounded emulated calls, canonical
pixel/raw/PCM evidence and source-cited hardware tests remain the contracts.
One GSD step per session can execute every dependency-ordered wave in a phase;
it does not require a separate invocation per wave. Review budgets keep tooling
findings as seeds rather than extra requirements.

### Key Lessons

1. Execute a planned phase with the whole-phase command; reserve wave selection for targeted recovery.
2. Reconcile verification after the final covered-file change; distinguish a historical pass from a current fingerprint.
3. Measure CI limits against the final workload and give concurrent checks independent output paths.
4. Keep release completion evidence separate from pre-merge verification, then reconcile planning records at closeout.

### Cost Observations

Model mix and total session count are not reliably recorded; no estimate is asserted.
The summaries record 149 tasks across 73 plans. Shared-file dependencies made
the 17 sound waves sequential even when one command dispatched the full phase.

## Milestone: v2.0 — The whole library

**Shipped:** 2026-10-29
**Phases:** 7 | **Plans:** 75 accepted / 76 summarized active slots / 82 historical summaries
**Recorded tasks:** 192 across all summary records, including superseded partial work

### What Was Built

- Shipped Activision, Absolute, Fractalus, BankSet and the XBoard/XM RAM/POKEY/HSC subset through all three deliverables.
- Added load-stable HSC/SaveKey battery storage, checked runner saves and real fresh-launch RetroArch persistence.
- Delivered typed controller input and movie2 replay for lightgun, paddle/keypad, quadrature and modern adapters, with independent pin-level diagnostics.
- Added PAL timing, geometry, boot and sustained audio carry while retaining existing NTSC observations.
- Accounted for every pinned-list entry, bound compatibility claims to public evidence and made the library ratchet required in CI.
- Improved pipeline reliability, armed the controlled performance comparison and completed automated release/install verification.

### What Worked

Independent numeric/media/pixel judgments precede equality. Real runner/module and
fresh-process frontend tests expose host seams. Existing CMake inventories, fault
controls, scoped API additions and source-bound evidence keep three deliverables
aligned. Release publication and latest-install close the post-merge evidence.

### What Was Inefficient

Fixed LOC and compressed-byte cutoffs repeatedly halted otherwise coherent work
before their accepted retirement. Long publication controls and provisional bounds
exposed recurring platform friction. Historical fingerprints and prose-only task
counting required explicit closeout reconciliation; neither proves acceptance.

### Patterns Established

Preserve halted history, actual base/source/attempt identity and every required
fault control. Give concurrent consumers independent output paths; calibrate bounds
from complete work. Keep debt explicit and optional improvements in seeds. Metadata,
model conformance, public title screens and gameplay remain distinct evidence classes.

### Key Lessons

Use the actual metadata consumer, cross-reference requirements/verification/summaries,
and count accepted plans separately from records (LESSONS MU-GSD-012–014). Follow
bounded recovery rather than repeating general replanning. Keep final package
publication separate from pre-merge and audit-time receipts.

### Cost Observations

Model mix and session counts are not reliably recorded. COST-01 monthly numeric
totals remain untrusted. The 18.24% local same-workload scheduling improvement is
historical Phase 5 evidence; no global hosted-cost reduction is claimed.
Timeline: eight calendar days from initialized v2.0 to shipped package.

## Cross-Milestone Trends

| Milestone | Phases | Plans | Requirements | Test evidence | Closeout |
|-----------|--------|-------|--------------|---------------|----------|
| v1.0 | 4 | 73 | 25/25 scoped | 605 regular + 6 archive checks in milestone audit; green published-release run | Override: 15 deferred artifacts and four stale fingerprints |

| v2.0 | 7 | 75 accepted; 82 historical summaries | 27/27 scoped | 673 regular + 6 package checks in final phase; 55 audit integration checks; published-release run | Override: 15 newly deferred, 13 carried, seven stale fingerprints |

Both milestones required explicit historical-fingerprint overrides and post-merge
publication checks. Repeated output/deadline friction favors existing independent
output paths and measured calibration; aggregate record counts never prove acceptance.
