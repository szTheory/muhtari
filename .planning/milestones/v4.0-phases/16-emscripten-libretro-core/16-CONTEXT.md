# Phase 16: Emscripten libretro core — Context

**Gathered:** 2026-11-10
**Status:** Ready for planning
**Authority:** Explicit `$gsd-autonomous --from 15`; ordinary decisions synthesized from the project fan-out brief.

<domain>
Produce a browser-consumable form of the existing libretro core and prove its lifecycle, input, callbacks and state continuation through an offline Node host. Report size and warmed headless throughput. This remains one core deliverable; no player UI or device code.
</domain>

<decisions>
### Module and tools
- Use an Emscripten no-entry executable-style link producing a modularized ES-module loader (`.mjs`) and companion `.wasm`; retain native MODULE and WASI behavior. A static archive alone is not acceptance.
- Use official Emscripten 7.0.0 with the immutable multi-platform OCI index and platform digests recorded in TOOLCHAIN-RESEARCH. Its bundled Node is the pinned runtime; record actual runtime/compiler versions during execution.
- The SDK is absent and image uncached here. First acquisition is approximately 800 MB compressed; download/build time is unmeasured. Use existing Docker CLI locally, no global package installation. Investigate any acquisition failure before retrying; stop if evidence makes a local workload of about an hour or more likely.
- Keep ordinary native builds independent of Docker. Prefer a required browser-artifact step after the existing Linux native build, reusing its checkout, fixture and native observation. Do not silently skip missing tools or artifact tests. Preserve the six native lanes and WASI advisory status.

### Acceptance and consumer
- Node loads the production artifact itself through its generated async factory, with local Wasm bytes and owned content. Register actual libretro callbacks with explicit signatures; copy callback bytes synchronously and reacquire memory views after calls that can grow memory.
- Reuse the generated input diagnostic and native library/host expectations. Assert independently derived numeric input, video geometry and exact audio counts before hash equality; match native/Wasm scripted observations and restored continuation.
- Exercise load/run/serialize/unserialize/unload, no-game behavior, malformed image/state refusal and state preservation after rejection. Use representative fault controls to prove the observer rejects missing callbacks, exports, mismatched evidence or skipped coverage as applicable.
- Keep recurring behavior coverage in the required CMake/CI mechanisms under AGENTS rule 5. Dedicated Emscripten configuration must fail clearly if required tools are absent; native tests must retain meaningful behavior/fault coverage without requiring Docker on every platform.
- Report `.mjs`, `.wasm` and combined bytes plus warmed fixed-workload native/Node throughput from the same source, fixture, inputs and callback work. Record runtime identities, architecture and timing limitations. No speed floor, size ceiling, browser/device acceptance or pipeline-speedup claim is inferred.

### Boundaries
- Integer-only bounded core; existing library/runner/libretro outputs and public contracts remain. Reuse CMake/POSIX sh and Node builtins; no npm dependency or new ROM/BIOS/save bytes.
- Phase 15 is verified. Phase 12's denied request is terminal, Phase 12/14 host/remote stops and nine deferred requirements remain. No host UI, RetroArch, remote dispatch, publication or merge is authorized here.
- Agent owns document judgments and automated verification. Continue local implementation, required builds/tests, at most two code-review fix rounds and one evidence-supported gap round. Ordinary decisions need no owner acknowledgment.
</decisions>

<canonical_refs>
- AGENTS.md; .planning/ROADMAP.md; .planning/REQUIREMENTS.md (WASM-01 through WASM-04).
- .planning/research/fan-out/prompt.txt; .planning/LESSONS.md; .planning/preparation/README.md and referenced HOSTS-AND-INTEGRATION, ARCHITECTURE, ENGINEERING §11 and DECISIONS.
- 16-RESEARCH.md; 16-TOOLCHAIN-RESEARCH.md; 16-HARNESS-RESEARCH.md.
- Phase 15 verified summary/report; current libretro target, host tests, generated input fixture and exact inventories.
</canonical_refs>

<deferred>
Browser player, Chrome/Firefox/Safari/device behavior, persistence UI, Playstead, host permission scopes, remote execution, cache/topology optimization, and a broad JS port of every native host test.
</deferred>
