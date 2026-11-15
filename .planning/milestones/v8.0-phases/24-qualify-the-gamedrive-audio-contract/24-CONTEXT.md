# Phase 24: Qualify the GameDrive audio contract - Context

**Gathered:** 2026-11-15
**Status:** Ready for planning

<domain>
## Phase Boundary

Produce a source-qualified go/no-go for future GameDrive streamed ADPCM support. The qualification covers the published command/register surface, sidecar discovery and identity, stream framing, playback clock, output and mixing, lifecycle and errors, state continuation, legal generated fixtures, finite resource needs, and effects on Muhtari's existing consumers. A no-go is a valid result when the documented profile or independent expected-PCM threshold is incomplete.

This phase records evidence and compatibility consequences only. It does not implement a decoder, select or change a public API/ABI or state format, add dependencies, fetch firmware, use copyrighted game data, access physical hardware, or infer device behavior from unlicensed sample/encoder or emulator source.

</domain>

<decisions>
## Implementation Decisions

### Evidence and source authority
- Use dated first-party RetroHQ firmware/product material and its published developer instructions as the source basis. Label developer prose as a partial profile when it lacks normative transaction details.
- For every required contract field, record source revision/date, evidence class, confidence, status, and exact citation. Distinguish documented fact, partial fact, inference, unknown, and unresolved contradiction.
- Leave fields unknown when the checked sources do not specify them. Do not infer a codec variant, bus semantics, timing, mixing rule, failure response, or state tuple from a common convention or implementation.
- Do not open or retain unlicensed sample/encoder code or emulator source. Do not download firmware or claim hardware behavior from documentation.

### Fixture and expected-PCM threshold
- Record that legally authored synthetic fixtures and an independent scalar PCM reference are feasible in principle under repository policy.
- Treat a fixture/oracle for the actual GameDrive profile as unqualified until a normative stream format, command surface, timing, and output mix allow expected public PCM to be derived independently.
- If those details remain missing, name the exact gaps and record no-go for implementation readiness; do not create or test guessed `.bup` data.
- Keep generated inputs and expected samples independent from any future production decoder. Do not use sample music, retail data, or unlicensed encoder code.

### Compatibility and resource assessment
- Evaluate current caller-owned cartridge media, fixed 48 kHz mono output, deterministic per-instance state, finite work/memory, lifecycle, and portable save/load behavior across the library, runner, libretro core, and portable consumers.
- Identify possible media identity, buffer ownership, bounds, refusal, save/load, API/ABI, and state-format consequences without choosing versions, adding fields, or changing code.
- Preserve the existing ADPCM refusal and all current compatibility contracts throughout this phase.

### Recommendation and stop condition
- Recommend no-go for implementing the current incomplete profile unless source-qualified evidence and an independent fixture/oracle pass close the required fields.
- Compare the strongest alternative: keep the feature horizon open until an authoritative profile or a lawful, reproducible title-blocking issue appears.
- Record contradictions, rejected options, the evidence threshold that would reopen the decision, and any remaining unknowns. Do not describe missing authority as passed.

### The agent's discretion
- Use existing project documentation and a one-pass, source-focused research method. Add no dependency or recurring test for this documentation-only phase.
- Choose a concise evidence ledger and compatibility note that jointly satisfy GDA-01 through GDA-04. Keep any local verification offline and tied to a concrete documentation or traceability fault.

</decisions>

<code_context>
## Existing Code Insights

### Reusable Assets
- `include/muhtari.h` has size-tagged `muhtari_media`, the `MUHTARI_SOUND_ADPCM` declaration, caller-supplied media buffers, 48 kHz audio metadata, and state size/save/load calls.
- `docs/embedding.md` documents caller-owned media, mono PCM output, and portable snapshot behavior; `docs/hardware/audio.md` records current audio timing and mixing contracts.
- `src/cart.c` recognizes the A78 ADPCM declaration, while `src/instance.c` refuses loading a cartridge that requires unsupported ADPCM. This phase preserves that refusal.
- `src/state.c` provides fixed-size, topology-versioned snapshots bound to the installed machine identity. `runner/main.c` and `libretro/libretro.c` are the existing host boundaries to assess.

### Established Patterns
- The core owns no file or device I/O; hosts supply bytes, calls remain bounded and deterministic, and state lives in the instance.
- Public structs are size-tagged and append-only; behavior, ABI, and state versions are distinct.
- Audio is integer `int16_t` mono at 48 kHz. Existing state snapshots identify the effective machine configuration and validate before applying a restore.
- Generated fixtures must follow the asset and provenance policies, and expected behavior must be independent of production code.

### Integration Points
- A future sidecar would cross caller-owned media delivery, host file handling, deterministic audio output, and portable state continuation. The current library, runner, libretro, and Wasm consumers must be assessed together, but no integration route is selected here.
- Any API, ABI, version, sidecar ownership, or serialized-state change belongs to a separate compatibility decision after the qualification gate.

</code_context>

<specifics>
## Specific Ideas

The current firmware history lists 26.01.2266 with a potential playback-glitch fix and ADPCM save/load notes. The developer post names `$420`, `adpcm@420`, `AudioStreamReset`, `AudioStreamPlayTrack`, and numbered `.bup` sidecars, but does not publish the full stream or playback contract. Keep the exact sources and limitations in `24-RESEARCH.md`.

</specifics>

<deferred>
## Deferred Ideas

Implementation of GameDrive playback, any public media/API/ABI/state-format change, device or host integration, firmware or sample-code inspection, physical-hardware testing, and other cartridge audio such as BupChip remain outside this phase. Reopen implementation only after the documented profile and independent expected-PCM threshold are met and the compatibility consequences receive a separate decision.

</deferred>
