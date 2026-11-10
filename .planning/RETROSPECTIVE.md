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

## Milestone: v3.0 — Playstead-ready play and states (closed with deferred scope)

**Closeout:** 2026-11-10, `override_closeout`; not a package shipment.
**Phases:** 3 | **Recorded plans:** 64 | **Requirements:** 7/16 complete.

### What Was Built

Phase 13 delivered portable validated state through the library, runner and
libretro core. Its refreshed verification passed 44/44 and the local CI workflow
passed. The player documentation, package checks, launch contract and host-test
paths also leave reusable local artifacts, but they do not satisfy the open host
acceptance requirements.

### What Worked

Local deterministic checks and explicit phase evidence extracted useful state
functionality without needing external Playstead changes. The archive keeps the
successful state API, the two incomplete phase reports, failed attempts and the
denied owner request distinguishable.

### What Was Inefficient

Phase 12 accumulated many plans around a released-core host path whose remaining
permission request was denied before preflight. That blocked acceptance, but it
did not prevent unrelated core development. Repeatedly treating phase acceptance
as the only useful next action delayed a clear scope choice. Phase 14 similarly
has strong local wiring but no host launch evidence; source-level tests cannot
substitute for that boundary.

### Patterns Established

When an identity-bound host action is denied, preserve the exact stop and evidence,
close the milestone transparently with unmet requirements still unchecked, and
move independent development to a separately verifiable scope. Prefer a single
offline harness over additional owner-run setup when it can answer the question.

### Key Lessons

- A milestone's acceptance blocker is not automatically a project-wide development blocker.
- Override closeout is valid bookkeeping only when failed verdicts, unmet requirements and unrenewed stops stay explicit.
- The next scope should reuse an owner-ranked independent direction and start with the recurring SEED-007 tune-up.

### Cost Observations

No trustworthy per-milestone cost total was available. Do not infer productivity from
64 recorded plans or 100 recorded tasks; only one of three phase goals passed.

## Milestone: v4.0 — Browser/WebAssembly core readiness

**Completed locally:** 2026-11-10. Two phases, two plans, five tasks; all five scoped requirements verified. No package release, hosted dispatch or merge occurred.

### What Was Built

Relative-root budget correctness, the production Emscripten module, direct offline Node/native evidence and required Linux integration, with size and warmed headless reports.

### What Worked

Independent toolchain and harness research led to one vertical slice. Existing fixture/oracles, exact inventories and fault controls caught real loader and evidence bugs. One source-review fix round closed all five findings. Automation and agent document judgment needed no owner UAT.

### What Was Inefficient

Long delegated closeout/report turns and repeated status requests lengthened the critical path. Keep research and reports concise, and return terminal results as soon as required evidence exists. The archive CLI omitted one summary's accomplishments and undercounted tasks; reconcile its output against actual plan blocks rather than repeating planning. A later inventory change made the earlier fingerprint stale; refresh that exact verification before auditing.

### Patterns Established

Hash implementation headers and generated compile inputs, preserve signal failures, invalidate previous receipts, validate every retained export, and mount source read-only. Keep native numeric expectations before cross-artifact hashes and actual-emulation limits next to measurements.

### Cost Observations

No reliable model-cost or hosted pipeline-cost series was collected. Local workload receipts support correctness, not global speedup or browser/device performance. Existing v3.0 acknowledgments and denied/spent scopes remain unchanged.

## Cross-Milestone Trends

| Milestone | Phases | Plans | Requirements | Test evidence | Closeout |
|-----------|--------|-------|--------------|---------------|----------|
| v1.0 | 4 | 73 | 25/25 scoped | 605 regular + 6 archive checks in milestone audit; green published-release run | Override: 15 deferred artifacts and four stale fingerprints |

| v2.0 | 7 | 75 accepted; 82 historical summaries | 27/27 scoped | 673 regular + 6 package checks in final phase; 55 audit integration checks; published-release run | Override: 15 newly deferred, 13 carried, seven stale fingerprints |
| v3.0 | 3 (1 verified) | 64 recorded plans | 7/16 complete | Local CI passed; Phase 13 verified 44/44; Phase 12 accepted 0/4; Phase 14 20/27 | Override: 22 newly acknowledged, 28 carried; nine requirements deferred |

Both milestones required explicit historical-fingerprint overrides and post-merge
publication checks. Repeated output/deadline friction favors existing independent
output paths and measured calibration; aggregate record counts never prove acceptance.

| v4.0 | 2 | 2 plans, 5 tasks | 5/5 scoped | Native and pinned SDK workflows passed; reviews and integration passed | Verified locally; zero new acknowledgments, 50 carried; shipping pending |
