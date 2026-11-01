# Domain Pitfalls

**Domain:** Atari 7800 save states, deterministic replay, and Playstead integration
**Researched:** 2026-10-29
**Confidence:** HIGH for existing repository contracts and parser behavior; MEDIUM for the external RetroArch launch path; LOW/OPEN for state-versus-battery semantics until v3.0 fixes the contract.

## Critical Pitfalls

### 1. A valid state can belong to the wrong machine

**What goes wrong:** A state restores against a different cartridge dump, board/header interpretation, region, controller or expansion configuration, BIOS/HSC firmware, or behavior revision. Execution may continue plausibly while producing the wrong game or diverging later. CRC-32 is useful for accidental corruption detection, but it is not a collision-resistant content identity or an authenticity check.

**Why it happens:** The preparation design's state header uses ROM CRC-32 and a state CRC-32. Playstead needs to retain and select states across launches, where equal geometry or a copied filename is not adequate identity.

**Smallest useful change:** Keep the CRC for corruption detection, and bind states to a strong hash of the effective ROM bytes plus the effective machine configuration, state-format version, and behavior revision. Bind BIOS/firmware bytes when they remain part of the running machine. Reject a mismatch before applying any field. Keep state-format compatibility independent of API/ABI version.

**Stronger alternative:** Add a canonical, extensible compatibility manifest in Playstead that also pins the emulator/core build and host-specific execution options. That gives the frontend a stricter policy but should be added only for an actual cross-build state-sharing requirement; core state should remain host-neutral.

**Cheapest falsifiable check:** Generate a state from an in-repo diagnostic fixture, then attempt it against a different synthetic ROM with the same board and region. Also vary one bound firmware/configuration identity. Each refusal must leave the live frame and battery unchanged. A crafted CRC collision is a useful regression if the implementation still relies on CRC for identity.

**Revalidate when:** The state header or Playstead compatibility tuple is designed, or a new firmware/expansion/profile field changes machine behavior.

### 2. A checksum-valid hostile state can still corrupt or stall the core

**What goes wrong:** A crafted state uses an out-of-range bank, controller enum, DMA position, device protocol state, or timer value; a wrapped length or tick then causes an invalid access, divergent execution, or a run call that never reaches its deadline. A truncated or malformed file partially changes the live machine before rejection.

**Why it happens:** A checksum detects accidental damage only. A hostile writer can recompute it. State bodies encode internal machine values, and validating fields independently may miss invalid cross-field combinations.

**Smallest useful change:** Use the fixed-size caller buffer and full validate-then-apply visitor already settled in DEC.35. Check exact size and checked arithmetic before reading, validate ranges and cross-field invariants, store indices rather than process pointers, and make apply infallible. Do not allocate from the file's declared length. Preserve the existing tick/run bounds after every accepted load.

**Stronger alternative:** Add a versioned schema/field inventory that automatically mutates every recorded field to invalid values. Prefer this to hand-maintained lists if the visitor can expose bounds cheaply; do not add a second parser or generic serialization dependency.

**Cheapest falsifiable check:** `fuzz.state` with random, truncated, oversized, and checksum-repaired inputs; a negative control for a real invalid field; and a property that every refused load leaves state hash, battery bytes, pending host input, and subsequent output unchanged. Valid save-load-continue must equal uninterrupted execution on the existing platform lanes, including big-endian.

**Revalidate when:** Adding a serialized subsystem, changing state-size calculation, or changing a frame/run deadline. Current runner movie parsing already has explicit byte/frame/line limits; retain that bounded-input pattern for states.

### 3. Save-state rewind and run-ahead can lose or persist the wrong battery bytes

**What goes wrong:** Loading an older state rolls battery SRAM/EEPROM back without preserving the newer save, or restores a saved generation token that matches the host's last-flushed token and suppresses a needed write. Conversely, run-ahead can leak speculative writes into persistent storage.

**Why it happens:** Battery bytes are durable player progress while state restore and rewind intentionally revisit an earlier machine point. The core exposes a committed-change generation; runner and libretro hosts own persistence. Those are different lifetimes.

**Smallest useful change:** Decide explicitly whether a state includes battery bytes. If it does, a load that changes those bytes must mark them dirty using the live host-facing generation, not restore a snapshot of that generation. Before Playstead applies a player-selected rewind, preserve the current save revision and make recovery available. Ensure speculative instances never publish battery writes.

**Stronger alternative:** Make state restore and persistent-save revision a Playstead transaction with immutable pre-restore history and conflict handling. This is safer for player data but belongs in Playstead's save/history layer, not the emulator core.

**Cheapest falsifiable check:** With an owned diagnostic that changes battery bytes, flush a newer value, load an earlier state, and prove the older bytes become observably dirty and recoverable. Separately run the same battery-writing frame speculatively and prove it does not alter the committed span. Test state/load failure paths to preserve both bytes and generation.

**Unknown:** The current architecture defines raw battery layout and generation, but does not settle whether state payloads include durable bytes or how frontend rewind should surface rollback. Resolve before state acceptance is frozen.

### 4. Syntactically valid replay is mistaken for reproducible continuation

**What goes wrong:** A movie runs with another ROM/BIOS or incompatible behavior and produces a plausible but unrelated result; a recording grows without bound or truncates a previous movie when a run is interrupted. A successful process exit is reported as proof the game resumed at the intended in-game point.

**Why it happens:** Existing movie v2 accepts optional identity fields, and a behavior-revision mismatch warns while continuing. The parser is bounded (32 MiB, one million frame lines), but those bounds do not by themselves make recorded output reproducible or atomic.

**Smallest useful change:** Keep current movie compatibility, but have new recordings include strong ROM identity, region, boot/firmware identity, explicit devices/configuration, and behavior revision. A verification/continuation mode must refuse missing or mismatched identity before the first frame. Reuse existing movie byte/frame caps; stream to a sibling temporary and atomically replace the target only after a complete recording.

**Stronger alternative:** Version a new strict movie format with a complete machine manifest and checkpoints. Do this only if an actual host continuation or reproducibility workflow needs it; otherwise the existing immutable movie2 replay is enough.

**Cheapest falsifiable check:** Wrong ROM, BIOS, region and behavior each refuse before frame 1; oversized inputs refuse before execution; an interrupted recording leaves any prior target intact. Replay an owned diagnostic from a recorded split point and compare the resumed visible/mailbox oracle with uninterrupted execution.

**Revalidate when:** Playstead requires exact in-game Continue or CI uses replay as a cross-run proof. Do not use states, movies, screenshots, or traces made with commercial ROM or real firmware as committed fixtures or CI artifacts (ASSET_POLICY §§1–3).

## Moderate Pitfalls

### 5. External RetroArch launch is overclaimed as integrated play or exact resume

**What goes wrong:** Pinning RetroArch and the muhtari core, then stopping and relaunching the game, is described as restoring the interrupted gameplay point. It does not: a fresh launch normally starts the cartridge and can recover only game-managed battery progress. A green runner test also does not prove the Playstead launch path displays and controls the real core.

**Smallest useful change:** A verified RetroArch app plus separately verified muhtari core, launched through Playstead's adapter, can establish ordinary Playstead-to-game launch, visible play, orderly stop, and battery-backed game progress. Label restart-to-cartridge accurately; claim exact resume only after state restoration reaches a repeatable in-game oracle. Keep the first real-host check small and automated with the core test card or an owned diagnostic.

**Artifact trust:** Pin the RetroArch distribution and muhtari core as separate artifacts. A pinned app archive does not identify the module later passed through `-L`; resolve and verify the exact installed core too. Use the project's verified release digest/attestation for muhtari and a recorded trusted digest for RetroArch. A matching hash proves byte identity, not who published those bytes. Playstead owns installer provenance, live-path verification, and process launch; code signing is not required by this project's current delivery decision.

**Stronger alternative:** An in-process libretro host could make the session feel fully native, but it changes the frontend boundary and belongs to Playstead. It is unnecessary to prove launch/play/stop and does not remove the need for state identity or battery-history safeguards.

**Ownership:** Playstead owns `Process` lifecycle, foreground presentation, input/focus, app/core pins, Continue semantics, and save-history confirmation. muhtari owns portable state bytes, strict load behavior, runner interfaces, libretro state support, and deterministic core evidence. Do not add GUI, window, audio-device, or input-device code here.

**Cheapest falsifiable check:** One Playstead automated launch uses the pinned RetroArch executable and exact pinned core path, observes the diagnostic frame, stops cleanly, relaunches, and verifies the diagnostic's battery-backed progress. This qualifies ordinary play, not exact save-state continuation.

**Revalidate when:** Playstead's adapter changes from the current external emulator-process model, or Continue changes from recency/launch behavior to a promise to restore the exact gameplay point. Local Playstead sources inspected: `playstead-mac/Playstead/Adapter/AdapterHost.swift`, `Playstead/Curation/ContinueShelfView.swift`, `docs/STATE-SUPPORT.md`, and `docs/CONTINUATION-PROTOCOL.md`.

### 6. Cross-platform hash agreement is treated as independent semantic proof

**What goes wrong:** A shared bug serializes the same wrong field on every platform, so byte equality passes while a real host cannot continue correctly. Conversely, a host-specific endian or compiler issue is missed by same-platform round trips.

**Smallest useful change:** Retain state-byte and resumed-output equality across the existing six native lanes plus big-endian, and add one independent composition check through real RetroArch/Playstead using an owned fixture. Use the existing diagnostic mailbox or visible test card as the external oracle; keep public compatibility evidence separate from owned-model conformance.

**Stronger alternative:** Add an independent hardware or emulator state oracle. It is costly, state formats are not portable between emulators, and it is not necessary for a first state contract; revisit only where current documentation/model ambiguity blocks acceptance.

**Cheapest falsifiable check:** Compare uninterrupted and save-load-split diagnostic outcomes across platforms, then launch the same owned fixture through the real libretro host and check its visible/mailbox result. Do not require owner UAT for machine-observable criteria.

**Revalidate when:** A result is promoted from owned conformance to broad commercial compatibility, or a new platform/serialization representation lands.

## Phase-Specific Warnings

| Phase topic | Likely pitfall | Mitigation |
|---|---|---|
| State format and API | CRC treated as identity; incomplete machine state | Strong content/config binding, independent state-format revision, split-run equality |
| Battery, rewind, run-ahead | Durable data silently rolls back or speculative writes persist | Set state/battery semantics; test old-state restore and speculative execution with an owned battery diagnostic |
| Movie recording/continuation | Optional identities or process exit counted as successful resume | Strict identity for new verification runs, existing parser limits, repeatable in-game oracle; defer extra movie scope absent a consumer |
| Playstead/RetroArch integration | UI/process implementation leaks into muhtari or wrong core is launched | Pin app and core separately; Playstead owns adapter/UI; automate one real-host path |
| Release and evidence | Personal ROM, firmware, or resulting save/state bytes enter GitHub artifacts | Use only test-card/owned-fixture outputs; keep user media local; verify digest/provenance for both external app and core |

## Sources and Confidence

- **High, current project contracts/code:** `AGENTS.md` rules 2, 4, 6 and 7; `DECISIONS.md` DEC.26, DEC.35, DEC.55–58, DEC.60; `preparation/ARCHITECTURE.md` §§4, 6–7; `docs/embedding.md` battery semantics; `runner/movie.h`, `runner/movie.c`, and `docs/runner.md` movie limits/identity; `ASSET_POLICY.md` §§1–5.
- **High, current integration evidence:** `preparation/HOSTS-AND-INTEGRATION.md` §§1, 3–5; `docs/libretro.md`; Playstead source/docs listed under Pitfall 5. The local Playstead evidence covers its current process adapter and scoped GB states; it does not qualify the proposed RetroArch adapter or Atari save-state support.
- **Unknown until phase research:** Whether Playstead will persist muhtari state files, whether the first player outcome needs exact in-game resume rather than relaunch plus game save, whether state payload owns durable battery bytes, and which firmware/configuration facts remain required after boot. Re-check these at requirements/specification before locking the file format.
