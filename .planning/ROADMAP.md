# Roadmap: muhtari

## Milestones

- [x] **v1.0 — 7800 games play with sound in RetroArch** — phases 1–4; shipped 2026-10-21. [Archive](milestones/v1.0-ROADMAP.md)
- [x] **v2.0 — The whole library** — phases 5–11; shipped 2026-10-29. [Archive](milestones/v2.0-ROADMAP.md)
- [x] **v3.0 — Playstead-ready play and states** — phases 12–14; closed 2026-11-10 with deferred scope (`override_closeout`). Phase 13 passed; Phase 12 remains accepted 0/4 and Phase 14 remains `gaps_found` at 20/27. Nine requirements remain unchecked. [Audit](milestones/v3.0-MILESTONE-AUDIT.md) · [Archive](milestones/v3.0-ROADMAP.md)
- [x] **v4.0 — Browser/WebAssembly core readiness** — phases 15–16; shipped through PR #47 and package v0.12.0. [Audit](milestones/v4.0-MILESTONE-AUDIT.md) · [Archive](milestones/v4.0-ROADMAP.md) · [Ship receipt](milestones/v4.0-SHIP.md). Earlier failed combined-source evidence and host stops remain preserved.

- [x] **v5.0 — Downloadable WebAssembly core** — phases 17–18; shipped as v0.13.1 on 2026-11-11; public-byte verification and current-main CI passed. [Audit](milestones/v5.0-MILESTONE-AUDIT.md) · [Archive](milestones/v5.0-ROADMAP.md) · [Requirements](milestones/v5.0-REQUIREMENTS.md)
- [x] **v6.0 — YM2151 homebrew sound** — phases19–20; verified locally2026-11-12; publication pending. [Audit](milestones/v6.0-MILESTONE-AUDIT.md) · [Archive](milestones/v6.0-ROADMAP.md)

## Completed phases

- [x] **Phase 19** (Measured milestone tune-up) — 1/1 plans; verified complete2026-11-11.
- [x] **Phase 20** (Complete YM2151 homebrew sound) — 7/7 plans; verified complete2026-11-12.

## Next boundary

All current implementation requirements are complete. Shipping is separate:
clear context and run `$gsd-ship 20` from this root checkout. The agent owns the
next service-health and receipt checks. Earlier Phase12/14 stops, nine deferred
v3 requirements and unaccepted hardware/platform evidence remain unchanged.
