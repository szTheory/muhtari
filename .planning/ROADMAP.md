# Roadmap: muhtari

## Milestones

- [x] **v1.0 — 7800 games play with sound in RetroArch** — phases 1–4; shipped 2026-10-21. [Archive](milestones/v1.0-ROADMAP.md)
- [x] **v2.0 — The whole library** — phases 5–11; shipped 2026-10-29. [Archive](milestones/v2.0-ROADMAP.md)
- [x] **v3.0 — Playstead-ready play and states** — phases 12–14; closed 2026-11-10 with deferred scope (`override_closeout`). Phase 13 passed; Phase 12 remains accepted 0/4 and Phase 14 remains `gaps_found` at 20/27. Nine requirements remain unchecked. [Audit](milestones/v3.0-MILESTONE-AUDIT.md) · [Archive](milestones/v3.0-ROADMAP.md)
- [x] **v4.0 — Browser/WebAssembly core readiness** — phases 15–16; shipped through PR #47 and package v0.12.0. [Audit](milestones/v4.0-MILESTONE-AUDIT.md) · [Archive](milestones/v4.0-ROADMAP.md) · [Ship receipt](milestones/v4.0-SHIP.md). Earlier failed combined-source evidence and host stops remain preserved.

- [ ] **v5.0 — Downloadable WebAssembly core** — phases 17–18; local package and release-contract readiness. [Research](research/v5.0/SUMMARY.md)

## v5.0 — Downloadable WebAssembly core

Host integrators can obtain a source-bound Emscripten libretro bundle, verified from its extracted contents and ready for the existing release pipeline. Begin with the mandatory measured tune-up, then deliver the existing core as one portable libretro archive. Reuse existing checks, the pinned SDK, Node consumer, native oracle and publisher; add no dependency or core/ABI/state-format change.

Acceptance is local and offline. Public rehearsal, publication, merge, new remote dispatch and actual Mac/browser/device checks are outside this execution scope. No browser player or device code is included. TUNE-06/07, PLAY-01–04 and FRONT-02–04 remain deferred; the denied Phase 12 diagnostic-app request and all original Phase 12/14 host/remote stops stay closed. New package evidence cannot replace those exact-source gates.

## Phases

- [x] **Phase 17: Measured milestone tune-up** — maintainers have current release/docs and a verified, measured baseline before packaging work. (completed 2026-11-10)
- [x] **Phase 18: Source-bound Emscripten release bundle** — integrators have one matched, documented archive exercised from clean extraction and covered by existing release contracts. (completed 2026-11-11)

## Phase Details

### Phase 17: Measured milestone tune-up

**Goal**: Maintainers can begin bundle work from current publication/docs and a source-specific quality/efficiency baseline without weakening required checks or obscuring historical failures.
**Depends on**: Archived Phase 16 (verified and shipped in v4.0)
**Requirements**: TUNE-09
**Success Criteria** (what must be TRUE):
  1. Maintainers can follow current project records to the shipped v4.0 PR #47/package v0.12.0 receipts and distinguish them from the earlier failed combined-source and deferred host evidence.
  2. Maintainers can reproduce the current local baseline using existing checks and inspect source-specific quality and efficiency measurements with queue, critical-path, job-duration and cancelled-work limits stated for the existing hosted receipts.
  3. Maintainers have a bounded measured disposition: carry the reproduced stale-shipping-record correction through baseline verification and explicitly retain the current pipeline where evidence supports no safe optimization; required fault coverage, advisory failures and host stops remain intact.
**Plans**: TBD

### Phase 18: Source-bound Emscripten release bundle

**Goal**: Host integrators have a source-bound, permissively distributable Emscripten libretro ZIP that works from clean extraction and is locally ready for the existing release pipeline.
**Depends on**: Phase 17
**Requirements**: WASM-05, WASM-06, WASM-07, WASM-08, WASM-09
**Success Criteria** (what must be TRUE):
  1. An integrator can extract one architecture-independent ZIP with the matched loader/Wasm, core metadata, exact commit and core source identity, pinned toolchain identity, loader/Wasm SHA-256 digests, host contract and required notices; exact membership excludes ROM, BIOS, fixture, save and measurement bytes.
  2. The existing offline lifecycle and independent native numeric parity consumer passes against a clean extraction without build-output fallback; representative missing, mismatched and tampered package controls reject the faulty archives.
  3. Maintainers can trace generated runtime/stdlib distribution rights and required archive notices to the exact pinned SDK inputs; unresolved or incompatible licensing blocks distribution.
  4. Existing local release-contract checks bind the ZIP to the exact release source and cover inventories, checksums, attestations, tamper/download validation and rehearsal selection for 19 ZIPs/20 assets, preserving all 18 native ZIPs and required rejection behavior. No remote rehearsal or publication is needed for this local readiness verdict.
  5. An integrator can follow concise reproducible download, extraction and loading instructions, including companion-file resolution, and see the explicit limits of offline Node/core evidence versus unverified real-browser/device behavior.
**Plans**: TBD

## Progress

Execution order: Phase 17 → Phase 18. Plan counts are assigned during phase planning; no requirement is accepted until verified.

| Phase | Plans Complete | Status | Completed |
|-------|----------------|--------|-----------|
| 17. Measured milestone tune-up | 1/1 | Complete    | 2026-11-10 |
| 18. Source-bound Emscripten release bundle | 2/2 | Complete    | 2026-11-11 |
