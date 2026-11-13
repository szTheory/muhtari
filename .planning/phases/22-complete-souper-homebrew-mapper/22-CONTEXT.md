# Phase 22: Complete Souper homebrew mapper - Context

**Gathered:** 2026-11-13
**Status:** Ready for planning

<domain>
## Phase Boundary

Deliver one documented Souper mapper software profile for homebrew developers: a 512 KiB ROM body with 32 KiB volatile EXRAM, CPU and MARIA mapping, deterministic lifecycle and snapshot continuation, and the same owned diagnostic through the C library, runner, loaded libretro core, and applicable portable consumer. Refuse malformed or unsupported profiles before allocation or live-machine mutation. Describe the result as conformance with the selected software model, not retail Rikki & Vikki compatibility or physical-cartridge accuracy.

This phase does not add BupChip/BupBoop audio, flash variants, extra image payloads, a new host, a dependency, or a public API/ABI surface. Existing supported POKEY declarations continue through their current path; this phase makes no new POKEY-plus-Souper hardware claim.
</domain>

<decisions>
## Implementation Decisions

### Supported profile and refusals
- **D-01:** Accept the existing public Souper identity through A78 v3 cart-type bit 12 or a valid A78 v4 mapper value 4, preserving DEC.38's v4 precedence. The mapped ROM body is exactly 512 KiB; the mapper owns a separate 32 KiB volatile EXRAM span. Refuse wrong geometry, malformed declarations, unsupported options, and payloads whose layout is not covered by the selected profile before allocation or mutation.
- **D-02:** Keep Souper mapper-only. `$8007` writes do not synthesize, queue, or expose BupChip audio. This phase adds no Souper-specific YM2151, Covox, ADPCM, BupChip/BupBoop, Rikki & Vikki, or extra music-payload support, and makes no unqualified audio claims. Preserve existing POKEY declaration handling without adding new Souper-specific POKEY wiring or quality claims.
- **D-03:** Implement the public mapping description in Muhtari's own C code. Keep the low-three-bit register aliases and the independent EXRAM-segmentation, character-remap, and MARIA-fetch-trap controls. Keep CPU and MARIA views separate and route MARIA reads through the existing tick-aware HALT path. Treat undefined register `$8006` as unsupported/no effect; do not infer BupChip behavior from `$8007`.

### Reset and resource policy
- **D-04:** Reset mapper latches to zero. The resulting initial map is flat EXRAM at `$4000–$7FFF`, ROM bank 0 at `$8000–$BFFF`, and the last ROM bank at `$C000–$FFFF`. Zero-fill all 32 KiB of EXRAM at load and power cycle. This is a deterministic emulator policy because physical EXRAM power-on contents are not established. A console Reset input remains distinct from power cycling the cartridge.
- **D-05:** Keep ROM and EXRAM instance-owned in the existing checked cartridge allocation path. Include all mapper latches and every EXRAM byte in snapshot size, validation, and application. Keep all invalid snapshot checks before mutation and do not allocate after load.

### Compatibility and consumers
- **D-06:** Apply the established additive state policy. Souper snapshots use state format 3 / behavior revision 8; `muhtari_get_versions` reports latest supported format 3 / behavior 8. Preserve existing non-YM snapshots at format 1 / behavior 6 and YM snapshots at format 2 / behavior 7, including their existing bytes and identities. Do not add general cross-build migration. Keep API 8 and ABI 1; the Souper public board value and load/probe APIs already exist.
- **D-07:** Exercise the owned diagnostic through the public C API, runner, loaded libretro module, and existing portable test consumer. Keep the existing libretro memory-map contract: publish stable CPU windows and leave dynamic/banked windows without a descriptor. This phase makes no new RetroAchievements memory-map claim for segmented Souper pages and does not require an actual RetroArch session.
- **D-08:** Author literal expected mapping bytes independently from production mapping code. Check malformed/unsupported controls and retain existing non-Souper observations. Cross-route hash agreement is integration evidence, not the independent mapper oracle.
- **D-09:** Keep all three deliverables and document the exact profile, reset/EXRAM policy, unsupported audio/payload forms, and confidence limits. Use existing code and checks; no new dependency is justified.

### the agent's Discretion
- Choose the smallest integration that follows the decisions above and current module patterns.
- Select generated diagnostic patterns and boundary cases that distinguish every supported mapping state with independent expected values.
- Use the existing source references as a software-model contract; do not present them as physical validation.

### Reviewed Todos
- `Close the re-review's findings on the Dependabot gate and the publish checks` — historical CI/publishing work; outside this mapper phase.
- `Promote the wasm32 lane into CI required` — historical CI policy work; portable mapper acceptance uses the existing consumer path.
- `Close the phase 2 re-review findings when MARIA and the run-loop land` — its remaining items are old test/documentation seeds; Phase 22's MARIA checks are defined by its own requirements.
- `Close the phase 2 review seeds on the nightly filter and the ROM tables` — historical nightly/cart database maintenance; outside this mapper phase.
- `Keep the phase 11 first review's info findings whose ids the gap-round review reused` — compatibility-documentation review findings; outside this mapper phase.
</decisions>

<canonical_refs>
## Canonical References

**Downstream agents MUST read these before planning or implementing.**

### Phase scope and settled choices
- `.planning/ROADMAP.md` — v7.0 boundary, Phase 22 goal and success criteria.
- `.planning/REQUIREMENTS.md` — SOUPER-01–07 and explicit exclusions.
- `.planning/research/next-milestone/v7-SOUPER.md` — selected mapper profile, evidence register, alternatives, limits, and public-source links.
- `.planning/research/fan-out/prompt.txt` — role lenses, evidence grades, adversarial synthesis, and dependency policy for this phase.
- `.planning/LESSONS.md` — prior reproduced lessons and the project lesson protocol.
- `.planning/preparation/DECISIONS.md` — DEC.26 versioning, DEC.36 profile fill policy, DEC.38 A78 precedence, and the relevant bus/audio contracts.

### Mapping, timing, state, and host contracts
- `.planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md` §1–2 — A78 parsing and the Souper profile, including CART.25/CART.42 source limits.
- `.planning/preparation/A7800-SYSTEM.md` — CPU/MARIA bus handoff, HALT, and reset ownership.
- `.planning/preparation/A7800-MARIA.md` §4 and §10 — tick-aware DMA and cartridge read integration.
- `.planning/preparation/ARCHITECTURE.md` §4, §6–7 — public versions, deterministic profiles, state identity, and transactional state loading.
- `.planning/preparation/HOSTS-AND-INTEGRATION.md` §2 — libretro consumers and stable memory-map policy.
- `.planning/preparation/CONFORMANCE.md` §4 — independently authored diagnostics and oracle limits.
- `.planning/preparation/ENGINEERING.md` §2–3 and §11 — C, test, resource, architecture, and documentation rules.
- `.planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-CONTEXT.md` D-07/D-08 — additive topology-specific snapshot compatibility and preservation evidence.

### Existing interfaces and documentation
- `include/muhtari.h` — existing Souper board enum and public probe/load/state interfaces.
- `docs/hardware/cartridge.md` — current A78 declaration, geometry, and refusal contract; update its Souper status as the phase lands.
- `docs/libretro.md` — loaded-core state and achievement memory-map behavior.
- `PROVENANCE.md` — approved source policy and the read-only status of the pinned mapper cross-check.
</canonical_refs>

<code_context>
## Existing Code Insights

### Reusable Assets
- `MUHTARI_BOARD_SOUPER` is already a public enum value; v3 bit-12 and v4 mapper-4 parsing and probe metadata already exist. Loading is currently refused.
- `src/cart.c` centralizes board dispatch, cart allocation/install, reset latches, and shared CPU/peek/DMA helpers.
- `muhtari__cart_read_at()` and `muhtari__cart_dma_read()` already carry the timing and reader identity needed by the Souper CPU/MARIA views.
- `src/state.c` has one ordered visitor with topology checks, state identity, full validation, and apply-after-validation behavior. Existing cartridge state tests cover RAM, latch continuation, and transactional refusal.
- Existing lifecycle tests cover zero-filled cart RAM, power-cycle behavior, stable allocation ownership, and rejection before allocation. Existing bus `halt_views()` tests provide a CPU/MARIA transition analogue.
- The generated BankSet diagnostic and loaded-libretro portable host tests provide patterns for end-to-end mapper checks without copyrighted game images.

### Established Patterns
- Boards use a `switch` in `src/cart.c`; no function-pointer dispatch table or global mapper state.
- ROM, cart RAM, and expansion RAM share a checked instance allocation. Validation precedes allocation and install; board reset zero-fills volatile cart RAM.
- MARIA DMA reads receive the actual fetch tick and run through the tick-aware bus/HALT path. Preserve this timing and avoid a global DMA flag.
- State bytes use fixed-width little-endian fields, a topology-specific size and identity, and validation before application.
- The libretro memory-map publisher reports only session-stable CPU spans; banked windows remain unmapped because the descriptor cannot identify the active bank.

### Integration Points
- `src/cart.c`: exact Souper geometry/refusal, register writes, CPU mapping, MARIA view, and reset policy.
- `src/internal.h` and `src/instance.c`: instance-owned mapper state, allocation extent, reset/power-cycle lifecycle, and latest reported versions.
- `src/state.c`: per-profile identity, mapper-register validation, EXRAM serialization, and the Souper envelope.
- `tests/cart`, `tests/bus`, `tests/core`, `tests/state`, `tests/diag`, `tests/runner`, and `tests/libretro`: focused refusal/mapping/lifecycle/state/consumer coverage using existing mechanisms.
- `libretro/libretro.c` and `docs/hardware/cartridge.md`: loaded-core metadata and user-facing support/refusal description. No new public API or ABI is required.
</code_context>

<specifics>
## Specific Ideas

The A78 header reference identifies Souper as v3 bit 12 and v4 mapper 4. The GameDrive homebrew code example documents the 512 KiB ROM, 32 KiB EXRAM, register map, segmented windows, character remapping, and MARIA fetch trapping. The mapper author's public HALT explanation describes the software model while explicitly limiting its physical certainty. The original Souper source page timed out during the saved research; retain the project's cited software-model scope and do not treat another emulator's agreement as hardware proof.

No specific gameplay image, retail compatibility target, host UI, or physical cartridge measurement is selected.
</specifics>

<deferred>
## Deferred Ideas

### Reviewed Todos (not folded)
The five historical TODOs above remain outside Phase 22, as directed. No unrelated CI, publishing, or old review work is pulled into this phase.

No additional deferred ideas were raised.
</deferred>

---

*Phase: 22-complete-souper-homebrew-mapper*
*Context gathered: 2026-11-13*
