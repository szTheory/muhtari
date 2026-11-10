# Roadmap: muhtari

## Milestones

- [x] **v1.0 — 7800 games play with sound in RetroArch** — phases 1–4; shipped 2026-10-21. [Archive](milestones/v1.0-ROADMAP.md)
- [x] **v2.0 — The whole library** — phases 5–11; shipped 2026-10-29. [Archive](milestones/v2.0-ROADMAP.md)
- [x] **v3.0 — Playstead-ready play and states** — phases 12–14; closed 2026-11-10 with deferred scope (`override_closeout`). Phase 13 passed; Phase 12 remains accepted 0/4 and Phase 14 remains `gaps_found` at 20/27. Nine requirements remain unchecked. [Audit](milestones/v3.0-MILESTONE-AUDIT.md) · [Archive](milestones/v3.0-ROADMAP.md)
- [x] **v4.0 — Browser/WebAssembly core readiness** — phases 15–16; verified locally and archived 2026-11-10. [Audit](milestones/v4.0-MILESTONE-AUDIT.md) · [Archive](milestones/v4.0-ROADMAP.md). Shipping is tracked in [PR #46](https://github.com/szTheory/muhtari/pull/46).

## Current position

v4.0 local implementation, verification, audit and archival are complete. No next milestone is started. Shipping and hosted receipts are tracked in PR #46; browser/player work and earlier host acceptance require their own scope.

## Archived v4.0 phase status

- [x] **Phase 15: Measured milestone tune-up** — verified and archived locally; [records](milestones/v4.0-phases/15-measured-milestone-tune-up/).
- [x] **Phase 16: Emscripten libretro core** — verified and archived locally; [records](milestones/v4.0-phases/16-emscripten-libretro-core/).

### Phase 15: Measured milestone tune-up

**Goal:** Reproducible tune-up baseline with corrected document-budget fault detection.
**Requirements:** TUNE-08
**Plans:** 1/1 complete; verified and archived in milestones/v4.0-phases/15-measured-milestone-tune-up/.

### Phase 16: Emscripten libretro core

**Goal:** Browser-targeted libretro artifact with direct offline Node lifecycle and independently checked native parity.
**Depends on:** Phase 15
**Requirements:** WASM-01, WASM-02, WASM-03, WASM-04
**Plans:** 1/1 complete; verified and archived in milestones/v4.0-phases/16-emscripten-libretro-core/.
