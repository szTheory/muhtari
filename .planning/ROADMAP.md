# Roadmap: muhtari

## Milestones

- [x] **v1.0 — 7800 games play with sound in RetroArch** — phases 1–4; shipped 2026-10-21. [Archive](milestones/v1.0-ROADMAP.md)
- [x] **v2.0 — The whole library** — phases 5–11; shipped 2026-10-29. [Archive](milestones/v2.0-ROADMAP.md)
- [x] **v3.0 — Playstead-ready play and states** — phases 12–14; closed 2026-11-10 with deferred scope (`override_closeout`). Phase 13 passed; Phase 12 remains accepted 0/4 and Phase 14 remains `gaps_found` at 20/27. Nine requirements remain unchecked. [Audit](milestones/v3.0-MILESTONE-AUDIT.md) · [Archive](milestones/v3.0-ROADMAP.md)
- [ ] **v4.0 — Browser/WebAssembly core readiness** — phases 15–16; scoped 2026-11-10, ready for Phase 15 planning.

## v4.0 — Browser/WebAssembly core readiness

Make the existing libretro core buildable for browser hosts and prove its
behavior through a pinned offline host. Start with the recurring measured
tune-up, then deliver the Emscripten artifact together with its first deterministic
consumer. Continuous phase numbering resumes at 15 after v3.0's phases 12–14.

The three deliverables remain the C library, headless runner and libretro core.
Browser pages, players, device handling, Playstead integration and actual-browser
UX or device-performance acceptance stay outside this milestone. The existing
WASI/Wasmtime lane remains separate and advisory; it does not establish the
Emscripten loader contract or browser behavior.

## Phases

- [ ] **Phase 15: Measured milestone tune-up** — Use current evidence to ship one small improvement or document the exact bounded reason no candidate safely qualifies.
- [ ] **Phase 16: Emscripten libretro core** — Build the browser-targeted core and verify lifecycle, deterministic callbacks and state continuation with an offline Node host.

## Phase Details

### Phase 15: Measured milestone tune-up

**Goal:** Maintainers can start v4.0 with a reproducible delivery baseline and one evidence-supported improvement or bounded disposition, while required checks retain their fault detection.
**Depends on:** Current verified source baseline and the v3.0 override closeout; no unresolved Phase 12 or Phase 14 host acceptance prerequisite.
**Requirements:** TUNE-08
**Success Criteria** (what must be TRUE):
1. A maintainer can reproduce the selected recurring pipeline, quality or documentation friction from current source-linked evidence; any CI comparison distinguishes queue time, critical path, completed work and cancelled attempts.
2. A maintainer can compare before/after observations for one shipped small improvement and find its retained representative pass/fault evidence, or reproduce the exact missing evidence and bounded revisit condition when no candidate safely qualifies.
3. Maintainers can find the resulting current documentation and remaining debt without interpreting a deferred or advisory result as passed acceptance; the required contracts and Phase 12/14 stops remain explicit.
**Plans:** TBD

### Phase 16: Emscripten libretro core

**Goal:** Integrators can build and load a browser-targeted form of the existing libretro core, and maintainers can verify its deterministic behavior with an offline host.
**Depends on:** Phase 15
**Requirements:** WASM-01, WASM-02, WASM-03, WASM-04
**Success Criteria** (what must be TRUE):
1. An integrator can reproduce the pinned Emscripten build, identify the emitted artifact and required loader/link form, and inspect its actual exports/imports against the documented browser loader contract.
2. A maintainer can run a pinned offline Node host that loads the emitted core, supplies an owned fixture and scripted input, captures video/audio callbacks, and completes libretro load/run/serialize/unserialize/unload lifecycle operations.
3. Identical scripted inputs yield independently expected native/WebAssembly frame, audio, input and restored-continuation observations, while representative refusal/fault controls preserve the applicable contracts without new ROM or BIOS bytes.
4. A maintainer can reproduce same-source artifact-size and warmed headless-throughput reports, with the runtime and measurement limits stated so the results do not imply actual-browser or device acceptance.
**Plans:** TBD

Phase 16 research must settle the module/loader/link form and pinned toolchain
before implementation. Keep artifact production, the first offline consumer,
parity/refusal checks and documentation in one vertical delivery slice.

## Coverage and boundaries

All five active v4.0 requirements map exactly once: TUNE-08 to Phase 15 and
WASM-01–04 to Phase 16. The nine unmet v3.0 requirements remain deferred in the
[v3.0 requirements archive](milestones/v3.0-REQUIREMENTS.md).

No host or remote allowance was renewed. The denied diagnostic-app request is
terminal, and the Phase 12/14 source-specific stops remain binding if those scopes
are revisited. They do not prevent the authorized offline v4.0 work.

## Progress

Execution order: Phase 15 → Phase 16.

| Phase | Plans Complete | Status | Completed |
|-------|----------------|--------|-----------|
| 15. Measured milestone tune-up | 0/TBD | Not started | - |
| 16. Emscripten libretro core | 0/TBD | Not started | - |
