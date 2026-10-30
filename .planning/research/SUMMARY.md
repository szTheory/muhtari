# Project Research Summary

**Project:** muhtari
**Domain:** Deterministic Atari 7800 emulator core and Playstead host integration
**Researched:** 2026-10-29
**Confidence:** MEDIUM overall

## Executive Summary

muhtari is a bounded, deterministic C17 emulator delivered as a library, a headless runner, and a libretro core. The immediate product goal is real 7800 play from Playstead. Playstead currently launches standalone emulator processes; the shortest route is its adapter launching separately verified RetroArch and muhtari core artifacts. RetroArch owns the interactive window, audio, input, and initial state UI. A Playstead-owned in-process libretro host remains a later product direction, promoted only if the external route creates recurring friction.

Keep the existing CMake/CTest stack and add no dependency. Build one portable, validated machine-state format in the library and expose it through the runner and libretro, so manual save/load, rewind, and run-ahead share one source of truth. State loading must reject incompatible or hostile data without changing the live machine. Treat a snapshot as a complete emulated machine point, including live battery bytes and in-flight EEPROM work, while keeping durable save files and host dirty-generation metadata outside the core format. A state restore that changes battery bytes must make that change visible to the host.

The main risks are incomplete state capture, state rollback silently replacing newer battery progress, and mistaking a muhtari test for proof of the separate Playstead launch path. Bind snapshots to strong media and machine identity, validate before applying, and test continuation against an independent owned diagnostic. Track muhtari readiness separately from the final Playstead-to-RetroArch acceptance. Keep full movie recording, TAS, and AI tools out of the critical path unless an actual user or host workflow needs them.

## Key Findings

### Recommended Stack

Retain C17, the instance-owned integer-only core, CMake, CTest, current release packaging, and existing project utilities. There is no demonstrated need for a runtime, codec, compression, or test-framework dependency. A library-owned fixed-size state API is the seam shared by all three deliverables; the runner owns state-file I/O, while libretro delegates its serialization callbacks to the library.

The source baseline is API 7, ABI 1, behavior revision 6, and package v0.10.0. The initial brief used stale API 6 / behavior 5 figures; it is now corrected against the live header and implementation. Historical phase receipts retain their original versions. A state-format version remains independent of API, ABI, behavior revision, and battery layout. A public API addition advances the API version and can preserve ABI 1 under the append-only contract.

**Core technologies:**

- C17 instance-owned library: one deterministic state owner and no post-load allocation.
- Fixed-size, versioned, byte-wise little-endian state: portable across word sizes and endianness; rejects invalid data transactionally.
- Existing CMake/CTest and seeded property harness: already cover the supported native platforms and big-endian lane.
- Existing libretro API and release artifacts: let RetroArch host play without adding a GUI or another muhtari deliverable.

### Expected Features

**Candidate table stakes (final ownership and scope in REQUIREMENTS.md):**

- Playstead can launch a supported 7800 title through a pinned RetroArch app and the separately verified muhtari core; keyboard/controller handoff and orderly return are demonstrated.
- Ordinary cartridge-backed progress survives a fresh process when the game uses HSC or SaveKey. This is battery continuity, not exact gameplay resume.
- Validated save/load is available through the C library, runner, and libretro, with a real RetroArch round trip before state claims are advertised.
- A small measured tune-up ships while preserving the currently known evidence limits.

**Should have (competitive):**

- RetroArch rewind and run-ahead backed by the same complete, stable state representation, once correctness and speed are demonstrated.
- Strict state identity and replay checks for reproducible state-continuation verification. Existing movie2 replay can serve where it suffices.

**Defer:**

- Full movie recording, generalized continuation protocols, BK2 import, TAS authoring, AI/Gymnasium, and browser play as part of this milestone. Promote recording only for a real user or host workflow.
- Playstead-managed cross-device state history and the in-process libretro host until Playstead has a supported state consumer and the external path demonstrates a product need.
- HSC firmware expansion beyond the existing signature-only stub and unrelated hardware support.

### Architecture Approach

The library owns the machine snapshot and its validation. Each existing device remains the owner of its fields; one state traversal records those fields without serializing C layout, pointers, or host buffers. The state binds to strong cartridge identity, effective machine configuration, optional firmware identity, format version, and behavior revision. A checksum detects accidental damage but does not establish identity or trust.

A complete snapshot includes emulated battery contents and pending EEPROM protocol state. It excludes the host-facing battery generation token. On a successful restore, changed battery bytes advance the live generation once; equal bytes leave it unchanged; a rejected restore changes neither bytes nor generation. Raw .sav/.srm files remain durable host-owned artifacts separate from emulator snapshots. User-selected older states can rewind battery contents, so the owning host must preserve recovery of newer durable progress before presenting state history or exact Continue. Speculative run-ahead writes must not become durable.

**Major components:**

1. **Library:** fixed state size for each loaded topology, serialization, full validation, identity checks, and infallible application.
2. **Libretro adapter:** delegates size/save/load to the library; RetroArch owns the state files and frontend callbacks.
3. **Runner:** bounded file input/output using its existing host-side persistence pattern; no filesystem access enters the core.
4. **Playstead adapter:** owns the two artifact pins, local content path, process lifecycle, focus/input behavior, and initial launch acceptance. RetroArch owns the interactive game session and its save/state UI on this route.

### Critical Pitfalls

1. **Wrong-machine state:** CRC alone is insufficient for media identity. Bind strong media and effective configuration/firmware identity, and refuse mismatches before mutation.
2. **Checksum-valid hostile state:** bounds and checksums do not prove field validity. Validate all values and cross-field invariants before an apply pass; refusal must preserve the machine and future output.
3. **Battery rollback:** restoring a snapshot can replace newer durable bytes or let stale generation metadata hide a write. Snapshot live battery state, keep generation host-owned, dirty changed restores, and define recovery at the host boundary.
4. **Unproven Playstead path:** a runner or libretro unit test does not prove Playstead launches the pinned app and exact core with usable input. Require a separate cross-repository launch proof using an owned fixture.
5. **Agreement mistaken for correctness:** identical state bytes across platforms can repeat the same wrong omission. Pair cross-platform split-run equality with an independent diagnostic or visible host oracle.

## Accepted scope after research

The owner prioritized actual gaming through Playstead over early recording/AI/TAS.
The milestone is now **v3.0 Playstead-ready play and states**. The 16 requirements
in [REQUIREMENTS.md](../REQUIREMENTS.md) are authoritative. Muhtari must provide
release-qualified launch readiness and complete state capability across its three
deliverables; Playstead owns adapter implementation and final cross-project E2E.
Initial state UI is RetroArch-managed. Exact Playstead Continue/native hosting is
a later owned consumer outcome, not an implicit promise of this milestone. Full
recording and generalized continuation remain deferred. Detailed byte layouts,
state-field validation and host pins belong to downstream phase work.

## Implications for Roadmap

Research supports three coarse shipping phases continuing at Phase 12. Preserve the tune-up-first decision while making the first phase deliver the earliest real gamer value.

### Phase 12: Player-ready delivery and Playstead bridge

**Rationale:** A working Playstead-to-game path is the shortest route to the owner’s stated outcome. The launch must use Playstead’s actual external-process model, with RetroArch and the muhtari core pinned as separate artifacts.

**Delivers:** Muhtari release/readiness evidence and the Playstead adapter handoff for interactive 7800 launch, input, clean exit and battery continuity. The actual Playstead adapter is a separate project dependency; this phase must label its status explicitly rather than claim that sibling work shipped here. Complete a bounded evidence-led tune-up alongside this product outcome.

**Addresses:** Playstead launch, controller/keyboard path, battery continuity, bounded tune-up.

**Avoids:** Calling relaunch “exact resume”; treating the headless runner as a player; adding UI/device code to muhtari; claiming savings from untrusted numbers.

**Debt status:** COST-01 monthly duration totals remain untrusted; WR-01 delayed-release equal-base selection remains fail-closed and unresolved; neither is a product-launch blocker or proven fixed. The 10,000-case property workload has censored/cancelled evidence and remains advisory, not a required-CI pass or promotion. If the tune-up touches these areas, use the existing concrete fault controls and complete measurements; otherwise preserve the recorded deferral. Optional review follow-ups remain optional.

**Acceptance boundary:** Muhtari checks prove core and artifact readiness only. Final Playstead acceptance is a separate dependency: Playstead must launch RetroArch with the exact pinned muhtari core, show an owned diagnostic, pass input, exit cleanly, and demonstrate fresh-process battery continuity. A green runner check cannot stand in for that result.

### Phase 13: Portable save/load through all three deliverables

**Rationale:** State support is explicitly high priority and is the foundation for RetroArch save/load, rewind, and run-ahead. One library format avoids divergent host serializers.

**Delivers:** Complete snapshots through the public library API, runner state-file interface, and libretro serialization callbacks, with actual RetroArch save/load evidence. Require save → restore → continue to match uninterrupted execution, malformed or wrong-machine refusal to leave the machine unchanged, portable bytes across existing lanes, and no post-load allocation.

**Uses:** Existing C17/CMake/CTest, typed state visitor decision, owned SHA-256, diagnostics, and vendored libretro contract.

**Avoids:** Raw struct dumps; using CRC as identity; publishing savestate claims before semantics pass; confusing .sav/.srm persistence with an emulator state file or restoring a saved generation token.

### Phase 14: State consumers and continuation proof

**Rationale:** Rewind and run-ahead depend on complete, fast state operations. Their correctness and performance should be demonstrated after the common state contract exists.

**Delivers:** RetroArch rewind/run-ahead behavior backed by the state contract, with battery rollback/speculation behavior covered. Add strict identity-bound replay or recording only if needed to make state verification or a real host workflow repeatable. Full movie authoring and TAS remain deferred without that need.

**Avoids:** Treating a process exit or byte equality alone as proof of in-game continuation; letting speculative battery writes persist; expanding the movie parser/tool surface without a consumer.

### Phase Ordering Rationale

- Launch and ordinary battery continuity produce player value before advanced deterministic tooling.
- State correctness belongs in the library; runner and libretro are thin file/callback boundaries around the same contract.
- State consumers follow state validation so rewind/run-ahead cannot conceal incomplete snapshots.
- The cross-repository Playstead result stays explicit. Muhtari readiness and Playstead final E2E acceptance are separate evidence items.

### Rolling Horizons

- **Immediate:** v3.0 Playstead external launch, ordinary battery continuity, portable state save/load, then rewind/run-ahead evidence.
- **Next:** Browser/WebAssembly support (SEED-008) remains the high-priority follow-on after v3.0; reassess at the milestone boundary with the retained advisory WASM equality evidence.
- **Mid:** v4.0 measured accuracy and deferred hardware/expansion work. Promote the Playstead in-process host only if the external route fails the one-app flow, controller handoff, or state experience the product chooses to promise.
- **Long:** v5.0 inspection, scripting, AI/Gymnasium, speedrun and TAS tools; v6.0 handheld/console ports and measured power budgets; 2600 mode only after its explicit scope decision.

### Research Flags

Phases likely needing targeted research during planning:

- **Phase 12:** Revalidate the dirty Playstead integration sources and pin/install contract; establish RetroArch arguments, exact core verification, controller handoff, keyboard fallback, clean child lifecycle, and fixture strategy. Recheck COST-01, WR-01, and property-run evidence only if the bounded tune-up changes them.
- **Phase 13:** Resolve the full state field inventory and battery/state recovery boundary against current code. Preparation already settles the portable visitor pattern, but the current state-versus-durable-save semantics need a single acceptance contract.
- **Phase 14:** Revalidate current RetroArch rewind/run-ahead behavior and measure the completed state path. Core save/restore patterns are established; extra recording research is unnecessary unless a real continuation consumer is promoted.

Standard patterns (skip broad research-phase):

- The C17 dependency choice, caller-owned buffer boundary, and libretro serialization entry points are well documented in the repository and official libretro guidance.
- Existing runner atomic-file and movie2 parsing patterns are established. Research whether the runner must consume state files is already resolved by the current v3.0 scope.

## Confidence Assessment

| Area | Confidence | Notes |
|------|------------|-------|
| Stack | HIGH | Direct code and settled project decisions support reuse of the existing C17, CMake, and CTest stack. Libretro buffer guidance is official and cross-checked against the vendored header. |
| Features | MEDIUM | Playstead product and adapter behavior were inspected directly, but the proposed RetroArch launch and input handoff have not run end to end. |
| Architecture | MEDIUM | State ownership and host seams come from current source. State implementation is absent, and battery rollback/recovery must be frozen before acceptance. |
| Pitfalls | MEDIUM | Core contracts and parser bounds are directly evidenced; external launch, state-history recovery, and run-ahead battery behavior remain unproven. |

**Overall confidence:** MEDIUM.

### Gaps to Address

- The inspected Playstead tree had uncommitted changes, and its current adapter supports standalone mGBA rather than RetroArch. Re-read current sibling sources before fixing pins or claiming interoperability.
- Determine whether the first Playstead state outcome is RetroArch-managed manual save/load or Playstead-managed named state. Keep battery files and states distinct; require an explicit recovery policy before Playstead state history or exact Continue.
- Enumerate every mutable state field and pending boundary from the current source during state design; this summary does not prescribe a byte layout.
- Choose an owned or CI-built fixture that can prove visible launch, input, and battery continuity without storing game, firmware, save, or state bytes in releases.
- Existing project context must retain COST-01 and WR-01 as unresolved deferred tooling debt and the nightly property workload as advisory until complete evidence changes those statuses.

### Reusable Project Findings

Persist these cross-project engineering patterns with their source and limits: one state owner shared by library and host adapters; transactional parsing with a validate-before-apply boundary; distinguish emulated state from durable host files and host dirty tracking; pair cross-platform equality with an independent semantic oracle; and prove external process integration at the actual consumer boundary. They are reusable interface and verification lessons, not Atari 7800 hardware facts. The project lesson ledger remains the memory source; imported sibling lessons stay hypotheses until locally reproduced.

## Sources

### Primary (HIGH confidence)

- Current muhtari sources: include/muhtari.h; src/instance.c; src/internal.h and device owners; runner/main.c; libretro/libretro.c; libretro/muhtari_libretro.info.in; tests/libretro/host.c.
- Project contracts: AGENTS.md; .planning/preparation/DECISIONS.md (DEC.26, DEC.35, DEC.55–58); .planning/preparation/ENGINEERING.md; PROVENANCE.md; ASSET_POLICY.md.
- Detailed research: [STACK.md](STACK.md), [FEATURES.md](FEATURES.md), [ARCHITECTURE.md](ARCHITECTURE.md), [PITFALLS.md](PITFALLS.md).
- Playstead product and source inspected at revision 3e12f16a75ec, including its adapter, installer, GameRow, Continue, state support, and continuation protocol. The inspected working tree was dirty; the proposed RetroArch adapter is not yet verified.
- Project memory and horizon sources: .planning/LESSONS.md (MU-HOST-002; bounded evidence/recovery lessons); .planning/PRODUCT-ROADMAP.md; SEED-002, SEED-007–010.

### Official external sources (MEDIUM confidence)

- [Libretro core development guide](https://docs.libretro.com/development/cores/developing-cores/) — frontend/core boundary and serialization contract.
- [Libretro run-ahead guide](https://docs.libretro.com/guides/runahead/) — in-memory save/restore dependency and clean/fast state requirement.
- [Libretro core-info template](https://github.com/libretro/libretro-super/blob/master/dist/info/00_example_libretro.info) — savestate feature claims.

---
*Research completed: 2026-10-29*
*Ready for roadmap: yes*
