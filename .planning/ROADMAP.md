# Roadmap: muhtari

## Milestones

- [x] **v1.0 — 7800 games play with sound in RetroArch** — phases 1–4; shipped 2026-10-21. [Archive](milestones/v1.0-ROADMAP.md)
- [x] **v2.0 — The whole library** — phases 5–11; shipped 2026-10-29. [Archive](milestones/v2.0-ROADMAP.md)
- [x] **v3.0 — Playstead-ready play and states** — phases 12–14; closed 2026-11-10 with deferred scope (`override_closeout`). Phase 13 passed; Phase 12 remains accepted 0/4 and Phase 14 remains `gaps_found` at 20/27. Nine requirements remain unchecked. [Audit](milestones/v3.0-MILESTONE-AUDIT.md) · [Archive](milestones/v3.0-ROADMAP.md)
- [x] **v4.0 — Browser/WebAssembly core readiness** — phases 15–16; shipped through PR #47 and package v0.12.0. [Audit](milestones/v4.0-MILESTONE-AUDIT.md) · [Archive](milestones/v4.0-ROADMAP.md) · [Ship receipt](milestones/v4.0-SHIP.md). Earlier failed combined-source evidence and host stops remain preserved.

- [x] **v5.0 — Downloadable WebAssembly core** — phases 17–18; verified locally 2026-11-11, publication pending separate ship authority. [Audit](milestones/v5.0-MILESTONE-AUDIT.md) · [Archive](milestones/v5.0-ROADMAP.md) · [Requirements](milestones/v5.0-REQUIREMENTS.md)

## Completed phase records

### Phase 17: Measured milestone tune-up

**Goal:** Current publication/docs and source-specific measured baseline without weakening checks or historical failures.
**Requirements:** TUNE-09
**Plans:** 1/1 complete; verification passed. [Records](milestones/v5.0-phases/17-measured-milestone-tune-up/)

### Phase 18: Source-bound Emscripten release bundle

**Goal:** Exact-source, permissively distributable portable ZIP, clean-extracted offline oracle and local readiness for the existing release pipeline.
**Requirements:** WASM-05, WASM-06, WASM-07, WASM-08, WASM-09
**Plans:** 2/2 complete; verification passed. [Records](milestones/v5.0-phases/18-source-bound-emscripten-release-bundle/)

## Next boundary

All six v5.0 requirements passed local acceptance. `$gsd-ship 18` is the next useful command when remote shipping is authorized. Public rehearsal, publication and merge were not run. The agent owns service/receipt checks and resolves archived phase paths with the selected runtime's `find-phase`; no owner directory switch is needed. Prior denied/spent host scopes and nine deferred v3.0 requirements remain unchanged.
