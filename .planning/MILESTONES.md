# Project Milestones: muhtari

## v1.0 — 7800 games play with sound in RetroArch (Shipped: 2026-10-21)

**Delivered:** NTSC linear and SuperGame cartridges play with TIA and POKEY audio through all three deliverables.
**Package release:** v0.4.0. Planning milestone v1.0 does not change the package version.
**Closeout type:** override_closeout, explicitly approved at milestone close.
**Phases completed:** 1–4; 73 plans, 149 recorded tasks; 25/25 milestone requirements covered.

**Key accomplishments:**

- Shipped an embeddable C17 library, headless runner and libretro core with automated attested releases and one-line RetroArch installation.
- Matched all 2,560,000 public 6502 vectors and implemented bounded SALLY bus timing, RIOT and validated cartridge loading.
- Rendered linear and SuperGame cartridges with measured MARIA DMA timing, diagnostic ROMs and cross-platform frame hashes.
- Integrated fast boot, CC0 OpenBIOS, controller movies, achievement memory maps and automated RetroArch gameplay checks.
- Delivered integer TIA and POKEY sound with canonical raw/PCM hashes, libretro callback parity and clipping-free sample pins.
- Verified the production 64-phase Q23 synthesiser against spectral and alias controls, with bounded allocation-free frame execution.

**Stats:** 492 files changed, 97,449 insertions and 369 deletions from initial preparation
`8ab05fa` to shipped release commit `40ce425`; 38,684 tracked C/header lines,
including 8,577 in `src/`. Timeline: 2026-10-15 to 2026-10-21 (six elapsed calendar days).
Counts exclude this closeout. Task count is the recorded summary count, not an estimate.

### Known verification overrides

Known verification overrides: 15 newly acknowledged, 0 carried forward from a prior close
(see STATE.md Deferred Items).

Separately, phase 1, 2, 3 and 4 verification fingerprints remain stale after later
covered-file changes. The owner explicitly accepted all four overrides. Historical
reports and digests retain their original verdicts; this is not a verified_closeout.
The milestone audit found no functional, requirements or integration gaps. 15 items acknowledged as deferred (six todos, eight dormant seeds and one compatibility-test output race); four stale phase verification fingerprints explicitly accepted.

The instructions-per-frame gate remains report-only, WASM/library jobs remain
advisory, and gate code is 1,367/8,577 source lines (15.94%, above the 10% budget).
Documented title-predicate and finite spectral-matrix limits remain in force.

**Release evidence:** [Release publication and latest-install run](https://github.com/szTheory/muhtari/actions/runs/39533130210) completed successfully; v0.4.0 is published with 19 assets.

**Archives:** [Roadmap](milestones/v1.0-ROADMAP.md),
[requirements and outcomes](milestones/v1.0-REQUIREMENTS.md),
[audit](milestones/v1.0-MILESTONE-AUDIT.md),
[phase records](milestones/v1.0-phases/).

**Next:** `$gsd-new-milestone` selects the next scope and defines fresh requirements.
No next milestone is started by this closeout. Tag creation is disabled by project configuration.

## Closeout verification

`cmake --workflow --preset ci` — exit 0: 605 regular tests and six release/archive
checks passed after archival. `scripts/hygiene.sh --tree` and `git diff --check` —
exit 0. Archive integrity check — exit 0: all 177 phase files retained, including
73 plans, 73 summaries and four unchanged verification reports; only the deferred
item acknowledgment modifies a moved phase artifact. `query audit-open --json` —
exit 0: zero open, 15 acknowledged. Hosted release evidence is linked above.
