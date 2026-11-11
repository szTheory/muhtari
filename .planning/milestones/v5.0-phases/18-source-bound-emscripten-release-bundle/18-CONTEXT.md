# Phase 18: Source-bound Emscripten release bundle - Context

**Gathered:** 2026-11-10
**Status:** Ready for planning

<domain>
## Phase Boundary

Deliver local/offline release readiness for one matched architecture-independent Emscripten libretro ZIP, using the existing pinned producer, consumer, independent native oracle and publisher. All five WASM-05–09 requirements are in scope. Actual remote rehearsal, publication, push, dispatch, merge and browser/device/host checks remain separate authority boundaries.
</domain>

<decisions>
## Implementation Decisions

### Bundle and identity
- **D-01:** Add one portable libretro release variant within the existing three deliverables. Retain the matched loader/Wasm, core metadata, exact commit, core-source identity, pinned SDK identity, SHA-256 pair digests, concise host contract and required notices. Exact member paths and prohibited content remain enforced.
- **D-02:** Reuse the existing Linux x64 pinned Emscripten producer; add no second SDK job, dependency, browser UI, device code or ABI/core-state behavior change. Include `muhtari-source-id.txt`, which the existing consumer requires but current evidence upload omits.

### Extracted consumer and provenance
- **D-03:** Exercise only a clean extraction through the existing offline Node lifecycle and independent native numeric oracle; forbid fallback to build outputs. Representative missing, mismatched and tampered package inputs must fail through recurring existing CI checks.
- **D-04:** Audit generated runtime/stdlib distribution rights and notice obligations at the exact pinned SDK inputs using primary sources before distribution. Carry required permissive notices and provenance; unresolved or incompatible licensing blocks distribution.

### Release contracts and instructions
- **D-05:** Extend existing exact-source inventories, checksums, attestations, downloaded/tampered archive validation and rehearsal selection from 18 ZIPs/19 assets to 19 ZIPs/20 assets. Preserve all native archives, native/core gates and required rejection behavior.
- **D-06:** Keep download/extraction/loading and companion-file resolution instructions concise and reproducible. Clearly distinguish local/offline Node/core evidence from unverified real-browser/device behavior.
- **D-07:** Keep all nine deferred v3.0 requirements and denied/spent Phase 12/14 host/remote stops unchanged. No public rehearsal, publication, remote dispatch, push, merge or actual host/device action is authorized here.

### the agent's Discretion
- Choose exact file placement, minimal manifest format and smallest existing CMake/sh integration that satisfy the locked contracts. Reuse existing mechanisms and fault controls before writing new ones.
- Plan useful early license, identity, exact-member and extracted-consumer checks before full workflows; keep review/verification proportional and avoid duplicated workloads. Local builds/retries under one hour are preauthorized after causal diagnosis.
- Adopt ordinary synthesized recommendations under explicit autonomous authorization without routine acceptance prompts. Escalate only a material scope/authority/risk/cost or irreversible-action boundary.
</decisions>

<code_context>
## Existing Code Insights

### Reusable Assets
- `scripts/emscripten.sh` pins the SDK image and Linux amd64 leaf, uses read-only sources and network-free bounded container execution.
- `tests/emscripten/host.mjs`, native `libretro.observe`, source identity sidecar and Emscripten test inventory already prove lifecycle and numeric parity.
- `scripts/publish.sh`, release workflow, archive contents/firmware scans and publish policy controls enforce the existing native release boundary.

### Established Patterns
- Plain POSIX sh and CMake `-P` checks, exact inventories and representative pass/fault controls; no new dependency or parallel publisher.

### Integration Points
- Existing Linux x64 CI producer emits the portable archive; existing release publisher consumes the same source-bound ZIP with native archives.
- Current docs/libretro host contract remains the integrator guide; user-facing README stays short.
</code_context>

<specifics>
## Specific Ideas

The milestone's three independent scope reviews and adversarial synthesis are complete in `.planning/research/v5.0/`. Phase research now answers only exact pinned licensing, archive/consumer design and release-contract seams. Phase 17 verified baseline is retained; do not repeat its report process or reopen scope.
</specifics>

<deferred>
## Deferred Ideas

Browser player, device performance, Playstead, prior host evidence and actual public release actions remain outside this local phase.
</deferred>
