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

## Cross-Milestone Trends

| Milestone | Phases | Plans | Requirements | Test evidence | Closeout |
|-----------|--------|-------|--------------|---------------|----------|
| v1.0 | 4 | 73 | 25/25 scoped | 605 regular + 6 archive checks in milestone audit; green published-release run | Override: 15 deferred artifacts and four stale fingerprints |

Only one milestone has completed; no cross-milestone lesson is yet established.
