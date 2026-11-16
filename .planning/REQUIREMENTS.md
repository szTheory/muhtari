# Requirements: muhtari v10.0 — Headless developer trace

**Defined:** 2026-11-16
**Core value:** 7800 games behave as they do on the console, identically on every platform, from a small MIT-licensed C library that any host can embed.
**Milestone goal:** Give homebrew authors and Muhtari maintainers a bounded, reproducible instruction and bus trace through the existing headless runner, without changing emulated behavior or the public API.

## v1 Requirements

### Measured milestone tune-up

- [x] **TUNE-14**: Review current release and CI evidence, queue time, critical path, job durations, cancellations and reproduced lessons; run applicable cheap consistency checks before longer work; and record an evidence-backed low-risk correction or bounded no-change result without weakening required coverage.

### Headless developer trace

- [x] **TRACE-01**: A developer can opt in to an instruction and bus-access trace from `muhtari-run` for a selected invocation frame and inclusive address range, and the trace contains only matching events in deterministic order.
- [x] **TRACE-02**: Each trace record identifies its event kind, master tick, beam position and CPU context; bus-access records also identify the address, direction and value, with instruction and bus address filtering semantics documented.
- [x] **TRACE-03**: Trace output has a finite record limit, reports when that limit truncates the trace, rejects invalid frame/range/limit arguments before execution, and reports file-output failures without claiming a complete trace.
- [x] **TRACE-04**: A generated or owned diagnostic fixture produces an exact numeric trace oracle, and enabling tracing leaves the fixture's frame and canonical state hashes unchanged.
- [x] **TRACE-05**: The trace remains runner-only, preserves public API 8, ABI 1 and existing state formats, and keeps callback state and output buffers outside emulated machine state.
- [ ] **TRACE-06**: With tracing disabled, the core preserves the existing hooks-off instruction-cost gate; instrumentation is selected only for calls that have tracing installed.
- [ ] **TRACE-07**: Runner documentation describes trace invocation, filtering, record fields, output bounds, truncation and known limits; committed fixtures and logs contain no ROM, BIOS or save bytes beyond the manifest.

## v2 Requirements

Deferred pending demonstrated users and a separate scope decision.

### Broader developer tools

- **TRACE-08**: Developers can step interactively or stop on bank-aware breakpoints and watchpoints.
- **TRACE-09**: Developers can correlate traces with symbol files and inspect display-list or per-line DMA output.
- **TRACE-10**: Users can automate or extend the runner with Python/Gymnasium, Lua, BK2, patching, lint or AI/TAS facilities.

## Out of Scope

| Feature | Reason |
|---------|--------|
| Public observer API, public API version bump, ABI change or state-format change | The selected consumer is `muhtari-run`; keep the change runner-only. If an external caller requires a public hook, pause for a separate compatibility decision. |
| GUI, interactive debugger, symbol parsing, DMA overlays, patching, Lua, Python/Gymnasium, BK2, AI or TAS | These are broader SEED-004 work, not required to establish one useful, bounded trace slice. |
| Broad title compatibility expansion or retail ROM fixtures | No named, rights-cleared gameplay set with independent outcomes is qualified. The pinned unknown rows are not known failures. |
| GameDrive or BupChip streamed audio | Their source-qualified media, timing, independent-oracle and resource requirements remain open. |
| RetroArch, real-console, host/device or owner-hardware validation | The selected runner trace is accepted by deterministic local automation; existing host stops remain unchanged. |
| New runtime dependency, test framework or CI topology | The current C17, CMake/CTest and CI mechanisms can verify the selected feature. |

## Traceability

| Requirement | Phase | Status |
|-------------|-------|--------|
| TUNE-14 | Phase 26 | Complete |
| TRACE-01 | Phase 27 | Complete |
| TRACE-02 | Phase 27 | Complete |
| TRACE-03 | Phase 27 | Complete |
| TRACE-04 | Phase 27 | Complete |
| TRACE-05 | Phase 27 | Complete |
| TRACE-06 | Phase 27 | Pending |
| TRACE-07 | Phase 27 | Pending |

**Coverage:**
- v1 requirements: 8 total
- Mapped to phases: 8
- Unmapped: 0
- Duplicate mappings: 0

---
*Requirements defined: 2026-11-16*
*Last updated: 2026-11-16 after v10.0 roadmap creation and complete phase mapping*
