# Roadmap: muhtari

## Milestones

- [x] **v9.0 — Publish the corrected Souper mapper package** — shipped as v0.16.0 and archived 2026-11-16. [Audit](milestones/v9.0-MILESTONE-AUDIT.md) · [Archive](milestones/v9.0-ROADMAP.md) · [Requirements](milestones/v9.0-REQUIREMENTS.md) · [Product roadmap snapshot](milestones/v9.0-PRODUCT-ROADMAP.md)

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
<summary>✅ v9.0 — Publish the corrected Souper mapper package — SHIPPED 2026-11-16</summary>

- [x] Phase 25: Publish the corrected Souper mapper (1/1 plan) — all four requirements verified.

Stable v0.16.0 was published from corrected source `a0319a9`; the failed v0.15.0 tag and draft were preserved. README/status PR #73 and phase closeout PR #74 merged. The milestone audit passed, and exact-main CI on the closeout merge passed in [run 43841380444](https://github.com/szTheory/muhtari/actions/runs/43841380444). See the [audit](milestones/v9.0-MILESTONE-AUDIT.md) and [release receipt](milestones/v9.0-phases/25-publish-the-corrected-souper-mapper/25-RELEASE.md).

</details>

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

## Successor scope

No successor milestone is selected. The recommendation, strongest alternative, evidence threshold, sibling ownership and deferred host scope are in the [rolling product roadmap](PRODUCT-ROADMAP.md). Select scope at the next milestone boundary before creating new requirements or implementation work.
