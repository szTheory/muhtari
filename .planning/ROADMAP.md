# Roadmap: muhtari

## Milestones

- [x] **v1.0 — 7800 games play with sound in RetroArch** — phases 1–4; shipped 2026-10-21. [Archive](milestones/v1.0-ROADMAP.md)
- [x] **v2.0 — The whole library** — phases 5–11; shipped 2026-10-29. [Archive](milestones/v2.0-ROADMAP.md)
- [x] **v3.0 — Playstead-ready play and states** — phases 12–14; closed 2026-11-10 with deferred scope (`override_closeout`). Phase 13 passed; Phase 12 remains accepted 0/4 and Phase 14 remains `gaps_found` at 20/27; nine requirements remain unchecked. [Audit](milestones/v3.0-MILESTONE-AUDIT.md) · [Archive](milestones/v3.0-ROADMAP.md)
- [x] **v4.0 — Browser/WebAssembly core readiness** — phases 15–16; shipped through PR #47 and package v0.12.0. [Archive](milestones/v4.0-ROADMAP.md) · [Ship receipt](milestones/v4.0-SHIP.md)
- [x] **v5.0 — Downloadable WebAssembly core** — phases 17–18; shipped as v0.13.1 on 2026-11-11. [Archive](milestones/v5.0-ROADMAP.md) · [Ship receipt](milestones/v5.0-SHIP.md)
- [x] **v6.0 — YM2151 homebrew sound** — phases 19–20; shipped as v0.14.0 on 2026-11-13. [Archive](milestones/v6.0-ROADMAP.md) · [Ship receipt](milestones/v6.0-SHIP.md)
- [x] **v7.0 — Souper homebrew mapper** — phases 21–22; closed locally 2026-11-14 with `override_closeout`, all eight requirements passed. This is not a package shipment. [Audit](milestones/v7.0-MILESTONE-AUDIT.md) · [Archive](milestones/v7.0-ROADMAP.md) · [Requirements](milestones/v7.0-REQUIREMENTS.md)
- [x] **v8.0 — GameDrive streamed-audio contract qualification** — phases 23–24; closed locally 2026-11-15 with `override_closeout`, all five requirements covered by the audit. GameDrive implementation readiness is NO-GO. [Audit](milestones/v8.0-MILESTONE-AUDIT.md) · [Archive](milestones/v8.0-ROADMAP.md) · [Requirements](milestones/v8.0-REQUIREMENTS.md)

## Completed phase history

<details>
<summary>✅ v7.0 — Souper homebrew mapper (phases 21–22) — CLOSED LOCALLY 2026-11-14</summary>

- [x] Phase 21: Measured milestone tune-up (1/1 plan) — completed 2026-11-13; TUNE-11 verified.
- [x] Phase 22: Complete Souper homebrew mapper (4/4 plans) — completed 2026-11-14; SOUPER-01 through SOUPER-07 verified.

All five plans have summaries. Native integration found and closed the loaded-module test gap; the final verifier, integration review and local gates passed. One `unknown` workspace record was acknowledged as deferred, with 49 earlier acknowledgments carried forward. See the [milestone audit](milestones/v7.0-MILESTONE-AUDIT.md) for evidence and environment limits.

</details>

<details>
<summary>✅ v8.0 — GameDrive streamed-audio contract qualification (phases 23–24) — CLOSED LOCALLY 2026-11-15</summary>

- [x] Phase 23: Measured milestone tune-up (1/1 plan) — completed 2026-11-15; TUNE-12 verified.
- [x] Phase 24: Qualify the GameDrive audio contract (1/1 plan) — completed 2026-11-15; GDA-01–04 verified.

The milestone audit and both phase reports passed. The Phase 23 verification fingerprint predates its merged timeout follow-up; this closeout accepts that stale fingerprint under GSD-HANDOFF §3 after the milestone audit and exact-main CI pass (`override_closeout`). Implementation readiness is **NO-GO** because the authoritative stream profile and independent exact-profile PCM oracle remain incomplete. No product implementation, public compatibility change or hardware claim was made. See the [audit](milestones/v8.0-MILESTONE-AUDIT.md), [qualification](milestones/v8.0-phases/24-qualify-the-gamedrive-audio-contract/24-QUALIFICATION.md) and [timeout follow-up](milestones/v8.0-phases/23-measured-milestone-tune-up/23-CI-TIMEOUT-FOLLOWUP.md).

</details>

## Next boundary

No successor milestone is selected. Recommendation: keep GameDrive at NO-GO and hold scope open until an authoritative profile, a lawful independent PCM oracle and finite resource facts are available. The strongest alternative is a single source-qualified correction for a reproducible title-blocking 7800 defect if a lawful independent fixture and expected result appear first. No such defect is established now. Preserve the v0.15.0 exact-source release stop, the original Phase 12/14 stops and the nine deferred v3.0 requirements.
