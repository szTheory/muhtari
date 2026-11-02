# Phase 13: Portable save/load across three interfaces - Research

**Researched:** 2026-11-01  
**Domain:** Portable emulator state in C; library, command-line runner and libretro  
**Confidence:** HIGH for owners/integration seams; MEDIUM for proposed encoding and validation limits pending implementation proof.

<user_constraints>
## User Constraints (from CONTEXT.md)

### Locked Decisions

### Complete portable machine point

- **D-01 (STATE-01/04):** Keep DEC.26/35: one library-owned typed visitor, explicit
  byte-wise little-endian fields, checked SIZE/WRITE/VALIDATE/APPLY passes, and an
  exact size fixed for a successfully loaded machine until its next successful
  content load or destruction. Different installed topologies may have different
  sizes. Size is zero without loaded content. Save/restore occurs between public
  calls and neither advances emulated time nor allocates after content load.
  Use stable offsets, canonical unused/reserved bytes and an uncompressed format;
  no struct dump, pointer values, native widths, compression dependency or schema
  framework. The planner owns exact bytes, offsets and maximum size calculations.
- **D-02 (STATE-01/03):** Capture everything needed to reproduce the next public
  observation: CPU/bus/chip/board state; volatile and expansion RAM; boot status;
  regional timing; partial interrupt/HALT work; lazy-device clocks/deadlines;
  MARIA working memory/cursors; oscillator, mixer, PCM carry and pending audio;
  controller electrical/timed state; active and pending core-owned input, including
  once-only motion; emulated battery bytes; and unfinished EEPROM protocol,
  buffered page and busy work. Preserve observable counters where they affect the
  public continuation contract. Build the field/invariant inventory from current
  owners and prove it; this list alone is not that inventory.
- **D-03 (STATE-01/02):** Exclude immutable media bytes, allocator/callback state,
  borrowed spans, derived pointers, host output destinations, staging storage,
  filesystem paths and host persistence metadata. Reconstruct derived references
  from the already installed machine. Core-owned pending input is included even
  though old preparation prose and an internal comment describe it as host input;
  the frontend callback, movie cursor and future input source remain host-owned.
  Reconcile those stale descriptions in implementation. No sub-frame capture API,
  external movie cursor or full recording protocol is selected.

### Compatibility, refusal and resource safety

- **D-04 (STATE-02/03):** Require the same supported state-format version and
  behavior revision, effective cartridge bytes and machine-affecting configuration:
  resolved board/options/RAM, region, sound, controller and battery attachment,
  expansion, boot mode, and applicable BIOS/HSC firmware or explicit stub/absence.
  Bind immutable bytes with SHA-256, reusing the existing owned implementation
  through a small private shared owner. Bind effective choices rather than paths,
  titles, ambiguous Auto requests or package/build timestamps. Do not serialize
  ROM/firmware into the state or require a particular operating system or frontend.
  Exact identity encoding and equivalent-media normalization belong to planning.
- **D-05 (STATE-02/04):** Keep corruption detection distinct from identity and
  trust. A CRC/checksum is not authentication, and matching SHA-256 identities do
  not make payload fields trustworthy. Validate the complete envelope, expected
  length/topology, versions, reserved values, scalar ranges, counts/indices and
  cross-field clock/deadline relationships before any live mutation. APPLY must
  be infallible after validation. Hostile lengths never select allocations or
  unbounded loops. Validate against installed topology, not declarations supplied
  by the state. Test checksum-repaired hostile payloads, not only random corruption.
- **D-06 (STATE-02/04):** Refusal preserves subsequent execution, pending input,
  battery bytes/generation, installed media/topology and public span identity.
  Save errors preserve the caller's output buffer. Define input/output overlap
  behavior explicitly: either support it through bounded staging or reject it
  before mutation; never allow an apply pass to overwrite bytes still being read.
  Use existing instance ownership and the least necessary preallocated scratch.
  Exact tick caps, safe arithmetic and scratch/stack/work costs must follow from
  current producer invariants and measured complete work under ENGINEERING §11;
  do not invent a numeric quota, enlarge existing limits or tune timeouts by guess.
- **D-07 (STATE-01/02, FRONT-01):** Keep state-format, API, ABI, behavior and raw
  battery-layout versions separate. Additive public functions/size-tagged fields
  follow the existing API increment and ABI 1 conventions; append statuses without
  renumbering. Keep the battery layout unchanged unless its bytes actually change.
  Version existing execution behavior only when its contract changes. Refuse
  unsupported format/behavior instead of implementing migration. No cross-version
  compatibility, netplay or universal future state promise is made.

### Emulated progress and host durability

- **D-08 (STATE-05):** State rollback includes emulated battery bytes and pending
  programming, but never restores the old host dirty token. Compare restored
  persistent bytes with the live bytes: a changed successful restore advances the
  live generation once under its existing unsigned equality-token semantics;
  equal bytes preserve the token. Refusal preserves both. Restoring pending work
  without changed committed bytes does not itself mark bytes dirty; later actual
  programming uses the existing commit path. Preserve the stable raw battery span.
- **D-09 (STATE-05/06):** Library serialization never reads or writes durable
  files. Runner and frontend retain persistence ownership. A successful explicit
  load intentionally restores older emulated progress and may subsequently cause
  a committed host save to replace newer battery bytes; document that consequence.
  Refused loads and failed state-file replacement must not overwrite prior files.
  Speculative branches must never cause durable writes: test the library/runner
  ownership boundary here, and leave actual RetroArch speculative-mode proof to
  Phase 14. Do not equate a live SRAM pointer or dirty notification with disk I/O,
  or promise that an arbitrary frontend cannot flush speculative memory. Recovery
  history and preserving newer save revisions in Playstead remain separate work.

### Library, runner and libretro consumers

- **D-10 (STATE-01/02, FRONT-01):** Add a small public size/save/load surface using
  caller-owned buffers and the existing status convention; document behavior in
  `docs/embedding.md`, keeping the header terse. Library loading consumes one exact
  canonical state, with no ignored payload extension. A save accepts sufficient
  output capacity, writes the defined bytes and leaves excess caller capacity
  outside the state. Specify NULL, short-buffer, no-content and failed-content-load
  behavior and prove it through the existing lifecycle tests.
- **D-11 (STATE-06):** Runner state load/save/hash is required by the accepted
  requirement; older research calling it optional is superseded. Reuse existing
  argument, exit-status, digest and checked battery-file mechanisms. Load content,
  effective options and any configured initial battery first, then validate/apply
  the optional state before execution. Define save points by completed frame calls
  in this invocation, including a zero-frame point; do not use ambiguous host time
  or an implicit frame number from an unrelated movie. SHA-256 state hashing covers the
  canonical bytes, excluding dirty metadata, paths and wrapper data. Fresh-process
  continuation must consume the intended remaining input sequence explicitly.
  Select exact option names and incompatible-mode rules against the current parser
  during planning; reject ambiguity rather than inventing a continuation protocol.
- **D-12 (STATE-06):** Bound state reads by the installed expected size and detect
  short input, trailing data and I/O failure before restore. Check all write/close
  and replacement results; reuse the existing sibling-temporary checked replacement
  strategy rather than truncating the destination up front. Give invalid arguments,
  state refusal and file failure useful diagnostics and existing nonzero status
  categories (failure 1, usage 2); use diagnostics to distinguish incompatible or
  malformed state from file failure. Do not repurpose unsupported-game exit 77 or
  introduce a dedicated state-refusal exit without a concrete consumer need.
  Preserve existing target files on refusal/failure and exercise actual file
  failure controls. No new state file envelope, compression or sidecar is needed.
- **D-13 (FRONT-01):** Libretro delegates to the same library bytes; it gets no
  second serializer or host-file I/O. Report a stable nonzero size only for loaded
  content with supported serialization; unloaded/failed-load paths are safe and
  expose no stale content. Accept larger serialize capacity as the official API
  requires; write the canonical prefix and leave excess capacity unchanged.
  Restore consumes the exact state length: official guidance provides no equivalent
  oversized-unserialize promise. Prove short/extra/null input refusal. Keep the same
  validated portable format for ordinary and run-ahead contexts; do not skip
  validation because a frontend requests fast states. The pinned MIT header's
  rollback-netplay context 3 requires big-endian integers; when explicitly requested,
  refuse that unsupported context rather than silently emitting the LE format or
  adding a second codec. An unavailable context query follows ordinary behavior.
  Update `.info` and its existing claim guard with proven callback portability and
  determinism; do not announce rewind/run-ahead or actual RetroArch acceptance here.

### Independent evidence and proportional execution cost

- **D-14 (STATE-03/04):** Pair split-run equality with an independent owned
  semantic observation (mailbox, pins/register values, expected pixels or numeric
  audio/timing), so a field omitted by both save and load cannot pass by agreement.
  Compare resumed state bytes, frames, raw transition audio and PCM against an
  uninterrupted machine; cover all supported topology/region classes and targeted
  pending input/device/audio/storage boundaries. Use a coverage matrix with a
  reason for each representative combination rather than an unmeasured Cartesian
  explosion. Unsupported features remain unsupported and unmeasured hardware fits
  retain their confidence labels.
- **D-15 (STATE-02/04/05):** Extend existing unit, allocator, seeded property/fuzz,
  runner-file and loaded-module tests. Include truncation/extra bytes, wrong
  versions/media/firmware/effective config, checksum-valid invalid fields and
  overflowing or inconsistent clocks/deadlines/lengths; after rejection compare
  next execution and host-visible metadata, not just reserialized bytes. Prove
  changed/equal/refused battery restore and pending write replay. Mutations must
  demonstrably exercise their intended fault. Reuse visitor-derived field bounds
  only where they reduce duplicated knowledge; an auto-generated bounds sweep
  cannot replace cross-field invariants or an independent semantic oracle.
- **D-16 (STATE-03):** Require exact-source executed native/big-endian state-byte
  and continuation equality for Phase 13 acceptance, preserving all existing lane
  labels. Current PR `CI required` consumes a compile-only `be32` job; required
  `hash-equality` compares six native lanes. Executed PowerPC equality lives in
  `.github/workflows/be-equality.yml`, called by nightly and release workflows;
  nightly is advisory and release equality gates publication. Planning must name
  how the existing executed path supplies the necessary source-bound receipt.
  Compile success is not execution, nightly is not a required PR gate, and future
  release execution cannot be claimed in advance. No lane promotion or structural
  workflow redesign is selected by this discussion.
- **D-17:** Add meaningful behavior checks to `cmake --workflow --preset ci` through
  existing registrations/inventories; extend existing equality/fuzz consumers.
  Keep generated state bytes ephemeral and use only admitted owned/permissive
  inputs. Retain no post-load core allocation, bounded work, required correctness,
  packaging/provenance checks and advisory distinctions. Measure completed work
  where resource choices need evidence. No generic audit framework, dependency,
  new recurring actual-host case or unrelated CI tune-up is selected. Preserve the
  existing two review-fix rounds and one gap round limit.

### Agent's Discretion

Research/planning owns exact public declarations/status mapping, canonical layout,
field/invariant inventory, checksum coverage, identity normalization, pointer/alias
strategy, tick bounds, scratch placement, CLI spelling/frame-input interactions,
test fixtures and measured costs within these decisions. Reconcile stale source
comments and affected preparation/user docs alongside implementation. A concrete
new conflict with an existing required contract is a blocker to resolve, not an
optional seed; do not ask the owner to choose routine technical details.

### Deferred Ideas (OUT OF SCOPE)

Actual RetroArch save/load, rewind and run-ahead qualification remains Phase 14.
Cross-version migration, compressed/extensible state schemas, full movies/TAS/AI,
netplay, browser delivery, native Playstead hosting/Continue/history and revision
recovery retain existing product triggers. Revisit only for a demonstrated consumer
or measured cost, rather than widening the state foundation now.

### Reviewed Todos (not folded)

The matcher returned generic keyword overlaps; none adds a state requirement:
- `.planning/todos/pending/2026-10-16-close-the-re-review-findings-on-the-dependabot-gate-and-the-publish-checks.md`
  — existing CI/publisher dispositions remain outside the state foundation.
- `.planning/todos/pending/2026-10-19-close-the-phase-2-re-review-findings-when-maria-and-the-run-loop-land.md`
  — preserve historical resolutions and remaining optional hardware seeds.
- `.planning/todos/pending/2026-10-19-close-the-phase-2-review-seeds-on-the-nightly-filter-and-the-rom-tables.md`
  — retain current filter/ROM/tooling dispositions.
- `.planning/todos/pending/2026-10-29-keep-the-phase-11-first-review-info-findings.md`
  — optional compatibility wording/local-driver work does not become state scope.
- `.planning/todos/pending/2026-10-16-promote-the-wasm32-lane-into-ci-required.md`
  — no advisory-lane promotion without independently qualifying evidence.
</user_constraints>

<phase_requirements>
## Phase Requirements

| ID | Description | Research Support |
|----|-------------|------------------|
| STATE-01 | A library caller can query a state size fixed for the loaded machine and save/restore a complete versioned byte-wise little-endian snapshot, including mutable supported chip/device/board/region state, pending work and emulated battery data, using caller-owned buffers. | Owner inventory, typed visitor, size formula, API and buffer outcomes. |
| STATE-02 | A caller can reject truncated, corrupt, unsupported or checksum-valid hostile states and incompatible media/firmware/configuration before any live-machine mutation; a failed restore preserves subsequent execution and host-visible storage metadata. | Preflight validation, identity, alias policy and refusal tests. |
| STATE-03 | A caller can save, restore and continue supported machine topologies with the same independent semantic observations, frame output and raw/PCM audio as an uninterrupted run, with portable bytes and equality across the existing required native/big-endian lanes; advisory lanes remain labeled. | Independent oracle, split-run comparisons and executed source-bound BE equality receipt. |
| STATE-04 | A host can use save/load repeatedly without core allocation after content load or unbounded work; hostile restored clocks, device deadlines and lengths cannot hang or overrun the core. | Fixed-size ownership, no-allocation visitor, field-specific validation and clock/work derivation. |
| STATE-05 | A host can distinguish restored emulated battery bytes from durable save files: changed restores notify dirty tracking without restoring an old generation token, equal/refused restores preserve the appropriate metadata, and speculative execution cannot commit battery writes to host files. | Separate emulated bytes/generation/paths; pending-write and changed/equal/refused tests. |
| STATE-06 | A runner user can save at a chosen frame, load a state in a fresh process and hash state for repeatable continuation, with checked file I/O, explicit failure status and preservation of live state/durable files on refusal. | CLI frame points, bounded reads, checked replacement and runner failure tests. |
| FRONT-01 | A libretro host can query stable serialization size and save/load through the same library state contract, with truthful version/feature declarations and safe unloaded/failed-load behavior. | Delegate callbacks, capacity/exact-length semantics, context refusal and loaded-host proof. |
</phase_requirements>

## Summary

Implement one private C state owner around the current muhtari instance, then delegate both hosts. The instance centralizes behavioral state; libretro callbacks currently report no state and the runner has no state flags. SHA-256 and CRC32 implementations currently live under runner; promote the owned SHA-256 to a private shared core owner and reuse CRC32 rather than adding a package. [VERIFIED: src/internal.h:450-481, libretro/libretro.c:1038-1055, runner/sha256.c, runner/crc32.c, runner/main.c:78-111]

Use one 64-byte LE header followed by a stable typed payload and SIZE/WRITE/VALIDATE/APPLY visitor modes. Bind format, behavior revision, exact total length, whole-state CRC32 and SHA-256 effective identity. Rebuild pointers and derived scheduler indexes through current owners. Exact offsets, maximum size, producer-derived clock limits and complete-work costs require implementation proof and measurement. [RECOMMENDATION: CONTEXT D-01/D-04/D-05; src/internal.h, src/bus.c:97-118, runner/sha256.c, runner/crc32.c]

## Architectural Responsibility Map

| Capability | Primary tier | Secondary tier | Rationale |
|---|---|---|---|
| Canonical serialization, validation, identity, battery generation | Core library | — | Core owns the instance and emulated state. [src/internal.h:450-481; D-01/D-04/D-08] |
| State files, frame points, parser and hash output | Runner | Core library | Runner owns invocation/files; core owns bytes and validation. [runner/main.c:1385-1399, :1435-1518; D-09/D-11/D-12] |
| Libretro callbacks and capability claims | Libretro adapter | Core library | Callbacks use library format; .info claims have test guard. [libretro/libretro.c:1038-1055, libretro/muhtari_libretro.info.in:16-20, tests/cmake/info_claims.cmake:13-23] |
| Semantic and architecture evidence | Existing CMake/CI/test owners | Core and both hosts | Extend registrations and inventories. [tests/core/tests.cmake:17-43, tests/prop/tests.cmake:18-35, .github/workflows/be-equality.yml:39-63] |

## Project Constraints (from AGENTS.md)

- Use only permissive source seeds; never open or retain GPL/LGPL/unlicensed emulator source. Keep the three deliverables and add no window/audio/input-device code. [AGENTS.md:33-36]
- Keep core integer-only, instance-owned, deterministic and bounded. [AGENTS.md:40-42]
- Every behavior change needs a test in the existing CI workflow; docs own behavior and public header stays terse. [AGENTS.md:43-45]
- Use proportional checks and ENGINEERING §11 for resource/architecture review; a changed check identifies a fault and exercises representative pass/fault behavior. Preserve review/gap limits. [AGENTS.md:55-62]
- Do not commit ROM/BIOS/save bytes except admitted fixtures; evidence is command, exit status and run URL, not a committed log. [AGENTS.md:37-39, :83]
- Public names use muhtari prefix and fixed-width types; prefer current helpers and standard library over dependencies. [AGENTS.md:95-103]
- Preserve one-major-step/session, branch/PR and spent-host-allowance boundaries. [AGENTS.md:63-82; Phase 13 CONTEXT]
- Nyquist and security enforcement are false in config, so omit Validation Architecture; hostile-state checks remain explicitly required. [.planning/config.json:32-45, :60; REQUIREMENTS.md:24-29]

## Standard Stack

| Component | Guidance |
|---|---|
| Existing C library/fixed-width types | Add one private state module with explicit LE field encoding; no schema package. [AGENTS.md:97-103; D-01] |
| Existing SHA-256 | Reuse runner implementation through a small private shared core owner; currently linked to runner/libretro, not core. [runner/sha256.c, runner/sha256.h, CMakeLists.txt:168; D-04] |
| Existing CRC32 | Reuse runner implementation for accidental corruption checks; CRC is not authentication. [runner/crc32.c; CITED: https://csrc.nist.gov/glossary/term/cyclic_redundancy_check] |
| Existing CMake/CTest/property/fuzz/host tests | Extend registrations and inventories; no new dependency/framework. [tests/core/tests.cmake:5-43, tests/prop/tests.cmake:18-35, tests/fuzz/tests.cmake:19-53, tests/libretro/tests.cmake:102-124] |

No external package/service is recommended.

## Architecture Patterns

### Byte path

Installed content/options/initial battery -> cache effective identity and size -> SIZE/WRITE canonical bytes for runner or libretro. Load bytes -> exact length, identity, checksum and full VALIDATE without mutation -> APPLY only on success -> restore live battery generation semantics and rearm derived indexes. [RECOMMENDATION: D-01/D-04/D-05/D-09/D-13; src/internal.h:450-481]

### Owner field inventory

| Owner | Include | Exclude/reconstruct and validation |
|---|---|---|
| CPU | poll_phi2, registers/status, JAM and IRQ sampled state. | Validate poll_phi2 against bus phi2 and derive status invariants from producer. [src/internal.h:140-151; src/cpu.c] |
| Bus | Clock origins, phi2/ticks, NMI/RDY/HALT/MARIA intervals, open bus, IRQ pipeline and SYNC. | Exclude/rebuild next_deadline. Exact source comment: “cycle_len is the length in ticks of the cycle in flight, 4 or 6.” [src/internal.h:153-194; src/bus.c:97-118] |
| RIOT | Timer history/registers/flags and RAM. | Validate timer history from producer. Source: “#define MUHTARI__RIOT_RAM_SIZE 128u”; “uint8_t ram[MUHTARI__RIOT_RAM_SIZE]”. [src/internal.h:196-215] |
| MARIA | Registers, display-list cursors, line RAM, stats/features, origin, DMA and pixel cursor. | Exclude video rows/pitch pointers; restore idle null/zero. Validate cursors by region. [src/internal.h:271-347] |
| Cart/expansion | Dynamic bank/control/page selectors and mutable RAM. | Exclude ROM/info pointers/allocation addresses; reconstruct from installed topology. Cart database has a 131072-byte RAM case; expansion initialization clears 131072u bytes. [src/internal.h:248-269; src/cartdb.c:433-441; src/cart.c:912-914] |
| Ports/input | Active and pending legacy/extended inputs, modes, device/electrical state, motion, light/pot/adapter/keypad and counters. | Include pending core input despite stale comment “Pending input is not machine state.” Future host input/movie cursor stays external. [src/internal.h:349-399; D-02/D-03] |
| Boot/timing/counters | Boot handoff/rejection, frame timing/audio remainder, public frame/lag counters. | Public fields are “uint64_t frames;” and “uint64_t lag_frames;”. [src/internal.h:404-418, :450-481; include/muhtari.h:305-313] |
| TIA/POKEY/synth | Oscillator/timer/filter/mixer/PCM history and behavioral indexes. | Exclude diagnostics unless public continuation observation is found. POKEY legitimately initializes “p->channel[ch].fire = UINT64_MAX;”. [src/internal.h:61-120; src/pokey.c:24-43; tests/core/test_core.c:347, :464-465] |
| Battery/SaveKey | Persistent battery bytes and pending page/protocol/busy fields. | Exclude host generation token/path. Source: “uint8_t data[34816];”, “uint64_t generation;”, “uint8_t page[64], touched[64], page_offset, pending, ack;”. [src/internal.h:426-448; src/ports.c:374-399; src/instance.c:430-445] |
| Instance/immutable fields | None serialized. | Exclude allocator/callback/observer, loaded flag, immutable ROM/BIOS/HSC firmware, staging, audio_work, output rows, all pointers. Bind immutable bytes/config in identity. [src/internal.h:450-481] |
| Derived indexes | None serialized. | Rebuild bus deadline with bus owner and expose narrow ports rearm hook rather than duplicating static calculation. [src/bus.c:97-118; src/ports.c:189-219] |

### Proposed byte layout and size

Recommend an initial 64-byte header, explicit LE fields:

| Offset | Width | Content |
|---:|---:|---|
| 0 | 8 | New fixed magic |
| 8 | 4 | State-format version |
| 12 | 4 | Header length |
| 16 | 4 | Exact total length |
| 20 | 4 | Behavior revision |
| 24 | 4 | CRC32 of entire state with this field zero |
| 28 | 32 | Effective machine SHA-256 identity |
| 60 | 4 | Reserved zero |

This is a proposal, not an existing format. Follow with stable owner-ordered fixed fields and topology-dependent memory blocks. No struct dump, padding, pointers, native-width fields, compression or runner wrapper. [D-01/D-04/D-05/D-12]

Checked size formula:

64 + fixed_payload_bytes + cart.info.ram_size + installed_expansion_ram_bytes + m->battery.bytes

Cart/expansion terms are zero when absent; fixed payload includes fixed arrays/system/RIOT RAM; battery length is installed persistent span. Source declarations include “#define MUHTARI__RAM_SIZE 0x1000u”, “#define MUHTARI__RIOT_RAM_SIZE 128u”, “uint8_t data[34816];”; current cartdb includes a 131072-byte RAM row and expansion clear is 131072u. Derive maximum from those topology sources, not an invented quota. [src/internal.h:196-226, :426-436; src/cartdb.c:433-441; src/cart.c:912-914]

Size stays fixed after a successful load until next successful load/destruction; checked size_t additions and header conversion; failed replacement keeps previous machine because loading stages before commit. [src/instance.c:316-409; D-01]

### Effective identity, validation, alias and bounds

Create domain-separated, length-delimited LE identity for effective installed cart bytes, board/options/RAM, region, sound, controllers/battery attachment, expansion, boot and applicable BIOS/HSC firmware or stub/absence. Omit path/title/Auto spelling/frontend/build timestamps. Cache SHA-256 on successful install; public max image constant is “#define MUHTARI_IMAGE_SIZE_MAX 1048704u”, so do not rescan a maximum image on every save/hash. [D-04; src/internal.h:248-269, :426-480; include/muhtari.h:12-13]

Promote existing SHA-256 to private shared core ownership; do not substitute HSC SHA-1. Reuse CRC32 for accidental corruption detection. Neither hash makes payload trustworthy. [runner/sha256.c, runner/crc32.c, src/internal.h:426-436; NIST CRC glossary]

Use one typed visitor: SIZE checked, WRITE infallible after preflight, VALIDATE exact length/header/identity/CRC/reserved/scalars/indexes/cross-field invariants with no mutation, APPLY infallible after full validation. No untrusted allocations or loops proportional to claimed length. [D-01/D-05/D-06]

Recommended alias policy is reject overlapping input/output with instance, cartridge allocation or any mutable storage before state reads/writes. Use checked half-open ranges and address arithmetic; no staging allocation. Include expansion actual range even if separately allocated. This protects input bytes from apply and machine state from output. [src/internal.h:254-268, :450-481; src/instance.c:436-439; D-06]

Save rejects alias/short capacity before touching caller bytes; writes only canonical prefix. Load accepts exact state length; short/trailing are invalid before mutation. NULL is argument error; no content size is zero and load/save return appended unavailable. Short save uses existing too-small. Failed initial content load remains unavailable; failed replacement retains prior content and size. [include/muhtari.h:15-32; src/instance.c:316-409; D-01/D-10]

Do not copy stale prep’s global 2^62 tick limit. POKEY uses UINT64_MAX as valid no-event sentinel for fire/reload/carry/resync; bus/ports scheduling indexes are derived. No global absolute clock cap is source-enforced. Derive field-specific bounds/order and checked producer arithmetic; preserve sentinels. This is the primary execution derivation gate. [src/pokey.c:24-43, :90-168; src/bus.c:97-118; src/ports.c:184-219]

At minimum validate CPU poll_phi2 vs bus phi2, CPU status, IRQ/NMI/RDY/HALT/cycle fields and clock order; RIOT timer, TIA phase/epoch; POKEY counters/polys/sentinels; MARIA line/event/DMA indexes; synth phase/sample config/PCM bounds/horizon; input/device/motion/electrical deadlines; SaveKey page/address/protocol/busy consistency; and frame/bus/MARIA/chip/audio clock relationships. Derive ranges from owners, not guessed generic checks. Source quote: “cycle_len is the length in ticks of the cycle in flight, 4 or 6.” The existing synth overshoot proof is in internal.h and frame.c; do not invent a future deadline horizon. [src/internal.h:39-78, :140-194, :312-399, :438-448; src/frame.c:15-131; src/ports.c]

Measure complete maximum-topology SIZE/WRITE/VALIDATE/APPLY work under ENGINEERING §11. Incomplete/censored timings are not complete-work evidence. [AGENTS.md:55-60; ENGINEERING.md §11; LESSONS.md MU-CI-007]

## API and version recommendation

Proposed additions to include/muhtari.h:

size_t muhtari_state_size(const muhtari *m);
muhtari_status muhtari_save_state(const muhtari *m, void *buffer, size_t capacity);
muhtari_status muhtari_load_state(muhtari *m, const void *buffer, size_t bytes);

Append state_format to muhtari_versions, preserving old size-tag prefix. Existing source values: “#define MUHTARI_API_VERSION 7”, “#define MUHTARI__ABI 1u”, “#define MUHTARI__BEHAVIOUR_REVISION 6u”; the current version struct ends with api/abi/behaviour_revision and first-size macro ends there. Recommend API 8, ABI 1, state format 1, raw battery layout 1, behavior revision 6 if frame/audio behavior is unchanged. [include/muhtari.h:12, :47-53; src/instance.c:12-13; src/internal.h:489-495]

Append (do not renumber) four statuses:
- MUHTARI_STATUS_ERR_STATE_UNAVAILABLE
- MUHTARI_STATUS_ERR_INVALID_STATE
- MUHTARI_STATUS_ERR_UNSUPPORTED_STATE
- MUHTARI_STATUS_ERR_STATE_MISMATCH

Existing enum ends “MUHTARI_STATUS_ERR_UNSUPPORTED_REGION = 12”, “MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE = 13”, “MUHTARI_STATUS_ERR_INVALID_HSC = 14”. Map absent content to unavailable; malformed/truncated/extra/checksum-bad to invalid; unknown format/behavior to unsupported; media/config/firmware mismatch to mismatch; keep NULL argument and short save as existing statuses. [include/muhtari.h:15-32; proposed mapping]

Source comment says behavior revision changes when same-input frame/audio/state hash changes, while D-07 keeps format and behavior separate. Clarify the comment under locked separation; do not bump execution revision solely for first format if frame/audio outputs do not change. This is a documentation ambiguity, not confirmed blocker. [src/instance.c:9-13; D-07]

## Runner integration

Current parser lists --frames, --hash-frame, --dump-frame N:FILE, --movie and --save-dir; frames currently require N>=1 and frame output points require 1..frames. Parser and option checks: runner/main.c:78-111 and :647-728. [VERIFIED: those source lines]

Recommend:
- --load-state FILE once.
- --state-at-frame N:FILE repeatable; allow N=0 through --frames.
- --hash-state N repeatable; same frame-point range.
- Keep --frames as frame calls in this invocation. Point zero is after setup/state load before next input. Point N is after Nth successful run_frame and completion increment, before setting next input. Existing loop order is visible at runner/main.c:1392-1399 and :1435-1502.
- Load content/options/initial battery first, then state before execution. Existing battery import follows successful content/info at :1385-1399. State can roll emulated battery back but state code never writes durable files. [D-09/D-11]
- Movie cursor remains host-owned. Require explicit remaining sequence/suffix; no inferred old frame cursor. [D-03/D-11; runner/main.c:1447-1467]
- Reject with bench/probe/no-content and duplicate/ambiguous points or outputs. Keep existing frames/movie semantics; do not invent a movie protocol.
- SHA-256 hash only canonical bytes, no wrapper/path/generation; allocate one host buffer after state-size query and reuse.
- Bounded exact read plus one-byte trailing check before restore. Reuse checked sibling-temp write/flush/sync/close/replace and preserve target. Keep current failure=1, usage=2, unsupported game=77 categories; diagnostics distinguish file errors/state refusal. [D-12; runner/main.c:479-510]

## Libretro integration

Current serialize callbacks return 0/false; delegate to library only with loaded content. Unloaded/failed content reports size 0 and false. [VERIFIED: libretro/libretro.c:1038-1055, :1493-1573]

Official docs: size zero means unsupported; serialize may accept len >= size; size stable between load/unload; offsets should remain stable. No oversized-unserialize promise, so exact restore length. [CITED: https://docs.libretro.com/development/cores/developing-cores/]

- serialize_size returns library state size.
- serialize accepts non-null and len>=size; writes prefix, preserves extra tail.
- unserialize requires non-null exact length and delegates validation.
- Query pinned context; refuse rollback context 3 because pinned header says “integers must be saved in big-endian format.” No second BE codec; unavailable query means ordinary LE. [VERIFIED: libretro/libretro.h:5649-5707; PROVENANCE.md §8]
- Set .info savestate true only after loaded-host proof, basic feature only; do not claim rewind/runahead. Current file has savestate false and libretro_saves true; info guard maps true claims to tests. [VERIFIED: libretro/muhtari_libretro.info.in:16-20; tests/cmake/info_claims.cmake:13-23; CITED: https://github.com/libretro/libretro-super/blob/master/dist/info/b2_libretro.info]
- Extend existing host test and inventory for lifecycle, exact/short/extra/null, tail, context and continuation. [tests/libretro/tests.cmake:102-124; tests/inventory/ci.txt:514-533]

## Test and evidence plan

Extend current owners, not a new framework:
- Core public API/lifecycle/allocator/generated image tests: tests/core/tests.cmake:5-43.
- Seeded split-run, input-at-vblank, isolation, battery lifecycle/observer props: tests/prop/tests.cmake:18-35.
- Fuzz replay: tests/fuzz/tests.cmake:19-53; generate valid states and repair checksum after field mutation to reach hostile validators.
- Runner battery-file fault controls: tests/runner/test_save.c; extend file read and temp replacement failure coverage.
- Loaded module and capability claim: tests/libretro/host.c, tests/libretro/tests.cmake, tests/inventory/ci.txt.
- Independent semantic oracle from owned diagnostics/generated fixtures, paired with uninterrupted/restored state bytes, video, raw transition audio, PCM and public counters. Two runs agreeing is not sufficient. [tests/diag/driver.c; tests/diag/expected.csv; tests/cart/images.c; LESSONS.md MU-TEST-003]

Representative matrix: base NTSC/fast boot; PAL + BIOS/POKEY; banked cartridge RAM; XBoard expansion RAM; XM/POKEY/HSC; SaveKey pending page/busy and commit; pending input/motion; MARIA DMA/NMI/HALT and audio carry where public boundaries allow. Add cross rows only for justified interactions; do not form full Cartesian product. Use generated in-memory or manifest-admitted fixtures; state bytes ephemeral. [tests/cart/images.c; tests/roms/manifest.txt; D-14/D-15]

Faults: short/extra/corrupt/version/media/firmware/config, checksum-repaired invalid enum/index/count/clock/deadline, sentinel misuse and overflow boundary. After refusal compare next frame/audio, pending input, battery bytes/generation, topology and span identity. Prove changed/equal/refused battery restores and pending EEPROM without committed-byte change. Save buffer and preexisting file must remain untouched on failure. Mutations must reach intended checks. [D-05/D-06/D-08/D-15]

### Required executed BE receipt

PR CI calls be32 “Required PowerPC compilation” and runs be32-compile; hash-equality compares native hash files. [VERIFIED: .github/workflows/ci.yml:291-331]

Executed path is .github/workflows/be-equality.yml: immutable source ref, native ci and BE be32-full inventories, commit/identity/hash evidence, lane-equality comparison and lane-equality artifact. [VERIFIED: .github/workflows/be-equality.yml:1-63]

Nightly invokes it advisory; release invokes it as publication gate. [VERIFIED: .github/workflows/nightly.yml:47-53; .github/workflows/release.yml:52-58; D-16]

Acceptance should name the exact-source lane-equality receipt with matching commit.txt, identity.txt and hashes.txt and include state bytes plus continuation results. No workflow run/receipt is claimed here. [VERIFIED: .github/workflows/be-equality.yml:39-63; D-16]

## Don't Hand-Roll

| Problem | Use | Reason |
|---|---|---|
| Encoding | One typed core visitor, explicit LE fields | One trust boundary/format for hosts. [D-01/D-13] |
| Identity | Existing SHA-256 shared privately with core | No new dependency; bind effective bytes. [runner/sha256.c; D-04] |
| Corruption | CRC32 plus full field validation | CRC is not authentication. [runner/crc32.c; NIST] |
| Scheduler restore | Existing bus owner and narrow ports rearm hook | Rebuild derived indexes without duplicated rules. [src/bus.c:97-118; src/ports.c:189-219] |
| Aliasing | Preflight overlap refusal | No allocation and no unread-input corruption. [D-06] |
| File wrapper | Exact state bytes and existing temp replacement | No new wrapper/sidecar; canonical hash. [D-09/D-12] |
| Tests/evidence | Existing core/prop/fuzz/runner/libretro/lane consumers | Reuse fault-detection mechanism. [registrations above] |

## Common Pitfalls

1. Same-mode round trip can omit same field on both sides; require independent semantics. [LESSONS.md MU-TEST-003]
2. Hostile clocks can overflow/hang; derive producer limits and preserve UINT64_MAX sentinels. No global cap verified. [src/pokey.c:24-43; src/bus.c:97-118; src/ports.c:184-219]
3. Pending input source comment is stale; include core pending input and motion. [src/internal.h:349-399; D-02/D-03]
4. EEPROM pending page/busy is distinct from committed battery; include it and dirty only on actual byte change. [src/internal.h:426-448; src/ports.c:374-399; D-08]
5. Rebuild derived deadlines/pointers after APPLY. [src/bus.c:97-118; src/ports.c:189-219]
6. Preserve size-tag prefix/status values; current prefix macro ends at behaviour_revision. [src/internal.h:489-495; include/muhtari.h:15-53]
7. State does not include future movie cursor; require explicit input continuation. [D-03/D-11; runner/main.c:1447-1467]
8. Do not infer rewind/runahead/context-3 support from callbacks. [.info/header; D-13]
9. Compile-only BE is not executed portability proof. [.github/workflows/ci.yml:291-331; .github/workflows/be-equality.yml:39-63; D-16]
10. Do not invent quotas; derive producer limits and measure completed work. [AGENTS.md:55-60; ENGINEERING §11]

## Security Domain

Security enforcement is explicitly false in config, so no ASVS gate applies. STATE-02/04 still require hostile-input validation, exact lengths, no attacker-sized allocation and checksum-repaired hostile tests. [.planning/config.json:60; REQUIREMENTS.md:24-29; D-05/D-15]

## Assumptions Log

| # | Claim | Risk if wrong |
|---|---|---|
| A1 | Initial proposed state-format value is 1. | Revise if another convention exists. |
| A2 | Keep behavior revision 6 if frame/audio behavior is unchanged. | Resolve source comment mentioning state hash. |
| A3 | Checked uintptr_t ranges cover supported alias checks. | Else use bounded preallocated scratch and measure cost. |
| A4 | CRC32 can move into private shared core ownership. | Confirm CMake/provenance during planning. |

## Open Questions

1. Derive clock bounds from producer arithmetic/sentinels and measure complete maximum-topology visitor work. Do not copy stale 2^62 or invent a quota.
2. Clarify behavior revision wording while retaining locked version separation; no frame/audio behavior change is evidenced here.
3. Define canonical identity order/equivalence for effective media/config, excluding paths/titles/Auto/build metadata.
4. Confirm actual expansion allocation ranges for alias checks.

## Environment Availability

No external package/service required. BE equality uses existing pinned CI cross-compiler/qemu setup; no hosted workflow was contacted or run. [.github/workflows/be-equality.yml:31-63; D-16]

## Sources

### Primary local sources

- Phase 13 CONTEXT — decisions, boundaries, evidence and deferred ideas.
- REQUIREMENTS.md:22-35 — STATE-01..06, FRONT-01 and Phase 14 boundary.
- src/internal.h, src/instance.c, src/frame.c, src/bus.c, src/ports.c, src/cart.c, src/cartdb.c, src/maria.c, src/tia.c, src/pokey.c, src/riot.c, src/synth.c — owners/producers.
- include/muhtari.h; runner/main.c, runner/sha256.c, runner/crc32.c; libretro/libretro.c, libretro/libretro.h, PROVENANCE.md §8 — API and host contracts.
- Existing tests/core, tests/prop, tests/fuzz, tests/runner, tests/libretro, tests/diag and CI workflows — registrations/evidence.
- AGENTS.md; preparation DECISIONS.md DEC.26/35; ENGINEERING.md §11; LESSONS.md MU-TEST-003/MU-CI-007 — constraints and lessons.

### External primary sources

- [Libretro serialization guidance](https://docs.libretro.com/development/cores/developing-cores/) — callback size/capacity/stable offsets.
- [Pinned libretro.info specification](https://github.com/libretro/libretro-super/blob/master/dist/info/b2_libretro.info) — feature labels.
- [NIST CRC glossary](https://csrc.nist.gov/glossary/term/cyclic_redundancy_check) — CRC is error detection, not authentication.

## Metadata

**Confidence:** Source inventory/integration HIGH; layout MEDIUM until implementation proof; resource bounds MEDIUM until derivation and completed-work measurement; Libretro API HIGH from official documentation/pinned header; BE workflow path HIGH, no run claimed.

**Research date:** 2026-11-01  
**Valid until:** Recheck source/workflow files if the worktree changes.

