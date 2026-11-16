# Roadmap: muhtari

## Milestones

- [ ] **v9.0 — Publish the corrected Souper mapper package** — active scope; Phase 25 execution is verified, with the final phase PR and milestone audit pending. [Requirements](REQUIREMENTS.md) · [Product rationale](PRODUCT-ROADMAP.md)

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

## v9.0 — Publish the corrected Souper mapper package

**Goal:** Make the already-merged Souper mapper available in a tested package under a fresh release identity, preserving the unpublished v0.15.0 tag and draft.

**Scope boundary:** Publish the already-merged Souper feature as v0.16.0 under the settled pre-1.0 minor-bump policy. This avoids repeating the explicitly one-time CI-only v0.13.1 patch exception. Preserve the v0.15.0 tag and draft. Use the established source-bound release workflow; do not add emulator behavior, dependencies, host/device tests, GameDrive support or new compatibility claims.

### Phase 25: Publish the corrected Souper mapper

**Goal:** Publish the corrected mainline source with Souper support and leave the public README and status page accurate.
**Depends on:** None.
**Requirements:** TUNE-13, REL-01, REL-02, DOC-01
**Success Criteria:**
1. A fresh v0.16.0 release identity is created from corrected mainline source. The existing v0.15.0 tag and draft remain unchanged.
2. The established release workflow passes all required exact-source CI, artifact integrity, provenance/attestation, publication and latest-install checks. The release receipt records commands, exit statuses and run URLs.
3. Existing Souper generated-fixture checks pass on the release source and the released library, runner and libretro artifacts are built from that source.
4. The README and status page distinguish published v0.14.0 from merged-but-unpublished Souper support until publication succeeds, then accurately report the new release.
5. CI queue, critical path, job durations and canceled work are reviewed. Any proposed structural, timeout or cache change has repeated evidence and preserves all coverage; otherwise record no safe change.

- [x] **Phase 25:** Publish the corrected Souper mapper (1/1 plan). (completed 2026-11-16)
**Plans:** 1/1 plans complete.

## Next feature boundary

After v9.0 ships, keep implementation scope open until a source-qualified title-blocking defect or complete GameDrive profile, lawful independent PCM oracle and finite resource facts are available. Preserve the original Phase 12/14 host stops and the nine deferred v3.0 requirements.
