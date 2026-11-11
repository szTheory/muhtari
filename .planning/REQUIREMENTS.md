# Requirements: muhtari v5.0 — Downloadable WebAssembly core

**Defined:** 2026-11-10
**Core value:** 7800 games behave as on the console, identically across platforms, from a small MIT-licensed C library hosts can embed.

## v5.0 Requirements

### Measured milestone tune-up

- [ ] **TUNE-09**: Maintainers can begin feature work from current publication/docs and a source-specific quality/efficiency baseline, with one evidenced small correction or an explicit bounded no-change disposition that preserves required checks and historical failures.

### Downloadable Emscripten libretro variant

- [ ] **WASM-05**: A host integrator can obtain one architecture-independent libretro ZIP containing the matching Emscripten loader/Wasm, core metadata, exact commit and core source identity, pinned toolchain identity, loader/Wasm SHA-256 digests, concise host contract and required notices, with exact allowed members and no ROM, BIOS, fixture, save or measurement bytes.
- [ ] **WASM-06**: A clean extraction of that ZIP passes the existing offline lifecycle and independent native numeric parity consumer without falling back to build outputs; representative missing, mismatched and tampered package controls fail.
- [ ] **WASM-07**: Maintainers can verify the distribution rights and notice obligations of generated runtime/stdlib code at the exact pinned SDK inputs, and the archive carries the required permissive notices and provenance.
- [ ] **WASM-08**: The existing release pipeline binds the new ZIP to the exact release source and includes it in exact inventories, checksums, attestations, tamper/download checks and rehearsal selection, while preserving all 18 native ZIPs and their required rejection behavior.
- [ ] **WASM-09**: Host integrators have reproducible download/extraction/loading instructions for the bundle, including companion-file resolution and explicit offline Node versus real-browser/device evidence limits.

## Deferred Requirements

The nine v3.0 gaps remain deferred: TUNE-06/07, PLAY-01–04 and FRONT-02–04. Original denied/spent host and remote scopes remain closed; later evidence from another scope cannot replace them. Measured accuracy, Souper/BupChip/YM2151, browser player and optional emulator interface, handheld ports and AI/TAS tooling retain their seed triggers.

## Out of Scope

| Feature/action | Reason |
|---|---|
| Browser player, UI or device handling | Host responsibility; Muhtari retains three deliverables. |
| Browser/device gameplay, performance or power acceptance | Offline artifact/Node checks cannot establish these claims. |
| Reopening Phase 12/14 or importing old worktree source | No new host authority; source/evidence remain separate. |
| New core behavior, ABI/state format or framework dependency | Existing libretro artifact suffices for this distribution slice. |
| Public rehearsal, publication, merge or new remote dispatch during local execution | Separate ship/authority boundary; prepare concrete reviewable changes first. |

## Traceability

| Requirement | Phase | Status |
|---|---|---|
| TUNE-09 | Phase 17 | Pending |
| WASM-05 | Phase 18 | Pending |
| WASM-06 | Phase 18 | Pending |
| WASM-07 | Phase 18 | Pending |
| WASM-08 | Phase 18 | Pending |
| WASM-09 | Phase 18 | Pending |

Coverage: 6/6 requirements mapped exactly once; zero accepted until verified. Acceptance is local package/release-contract readiness; actual release outcomes are confirmed only at ship.
