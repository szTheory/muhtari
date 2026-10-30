---
title: "Architecture: portable machine state and host boundaries"
summary: "v3.0 state ownership, portable serialization, battery restore semantics, and the library/libretro/runner boundaries."
read_when: "Planning or implementing save states, rewind/run-ahead, runner resume, or state persistence."
updated: 2026-10-29
---

# Architecture Patterns

**Domain:** Deterministic Atari 7800 emulator state across an embeddable library, libretro host, and headless runner.

## Current shape and evidence

The live core has no state API or `src/state.c`: `retro_serialize_size()` returns 0, serialize/unserialize fail, and the `.info` file says `savestate = "false"`. The library runs a full frame per call and owns its CPU, timing, devices, mutable memory, and audio history in `struct muhtari`; the runner and libretro adapter own file/callback integration. The checked-out header defines API 7 and `src/instance.c` reports behavior revision 6; `PROJECT.md` still says API 6 / revision 5 and should be reconciled by the milestone state owner.

Current mutable state is deliberately spread among its hardware owners: `cpu.c`, `bus.c`, `riot.c`, `maria.c`, `tia.c`, `pokey.c`, `synth.c`, `ports.c`, `cart.c`, and `instance.c` own their named state. `ports` contains both active and pending extended input, controller motion/light/paddle deadlines, and lag counters. The bus stores absolute tick deadlines and partial HALT/NMI/IRQ pipeline state. Sound includes pending mixed transitions and a PCM FIFO. SaveKey stores an unfinished page write and busy deadline. Cartridge RAM, expansion RAM, HSC SRAM and EEPROM bytes are mutable; ROM, BIOS and HSC firmware are copied on load and remain immutable for that load. The visible frame/audio buffers, MARIA row pointer, palette conversion, callbacks, allocator, staging buffers, and observation pointers are host or lifetime state, not machine state. [SRC.01–05]

The existing battery API returns one stable raw byte span and a generation token that advances on committed byte changes; power cycle retains the bytes. The runner imports/flushes raw `.sav` files itself, and libretro exposes save RAM for frontend-owned `.srm` I/O. Neither host delegates file access to the core. Current Playstead is an external GUI process launcher, not a libretro host; the smallest gamer path is a Playstead-owned adapter/pin that launches RetroArch with muhtari's libretro core. State bytes made from commercial ROM, real BIOS or HSC firmware are prohibited repository assets; state tests should use owned diagnostics and keep generated state data ephemeral. [SRC.02, SRC.06, SRC.07, PS.01]

## Recommendation

Build state in the library first, then expose the same byte format through libretro and, if a real host needs it, the runner. Keep `state.c` as the one format/traversal owner, using the already-settled typed visitor passes (SIZE, WRITE, VALIDATE, APPLY), byte-wise little-endian fields, and explicit bounds. Use one visitor per state-bearing structure; do not dump C structs, padding, pointers, or `size_t`, and do not add a generic serializer framework or runtime dependency. The repository already has an owned SHA-256 implementation used by runner and libretro; reuse that implementation as one private shared utility for media identity rather than creating a second digest or using CRC alone for ROM matching. This follows settled DEC.26/35 and the existing private utility pattern. [DEC.26, DEC.35, SRC.08]

The public state size is zero before content and otherwise exact and constant from successful load through unload. It may differ between loaded machines because cartridge/expansion RAM and battery topology differ; compute it with checked `size_t` arithmetic from the installed topology. State loading requires the same loaded media and machine configuration, validates the entire buffer before mutation, then applies a pass that cannot fail. A state contains versioned metadata, a checksum for corruption detection, behavior revision, region/board/configuration identity, and strong identities for the cartridge, optional BIOS and HSC firmware or stub. It carries machine RAM and mutable cartridge/expansion/battery bytes, but not ROM or firmware bytes: the target host loads those first. CRC is corruption detection, not authentication; every field remains untrusted after checksum validation.

Capture all emulated state required to continue at the library call boundary: registers and RAM, board banks and IRQ state, profile, absolute clock/deadline and partial event pipelines, MARIA line RAM/cursors/statistics, controller electrical and timed state, pending input packet and active input, audio oscillator/mixer history and pending PCM, boot hand-off/refusal counters, and uncommitted EEPROM protocol/page/busy state. Do not serialize host callback state, output buffers, derived pointers or allocator configuration. Saving and restoring must not advance emulated time or call the allocator.

Battery bytes belong in a complete machine snapshot, so a restored game sees the same persistent RAM and an EEPROM operation can be rolled back/replayed. Battery `generation` is host dirty-tracking metadata, not emulated state: never serialize or rewind it. After a valid restore, if battery bytes differ, increment the live generation once; if equal, leave it alone. A rejected restore leaves both bytes and generation unchanged. This can cause a harmless extra runner flush after a rewind branch, but cannot hide a restored battery change from a host. The library state boundary stays separate from persistence: state calls only read/write buffers; runner and frontend continue to own disk files. [SRC.02, SRC.03, SRC.06]

No backward state migration is needed for v3.0: reject unsupported state format, behavior revision, media identity, topology or exact-size mismatch with no mutation. Keep the state format version independent of API, ABI, behavior revision and battery layout. Add state functions and an appended state-format field to the size-tagged version result in one public API increment; adding functions/fields does not require an ABI break under the existing append-only struct rule. [DEC.26]

## Component boundaries

| Component | Responsibility | State responsibility |
|---|---|---|
| Core library (`include/muhtari.h`, new `src/state.c`) | Opaque instance, deterministic fixed-size bytes, validation and application | Public size/save/load calls; format, CRC, media binding, visitor order and transactional restore |
| Existing chip/device owners | CPU/bus/MARIA/TIA/RIOT/POKEY/synth/controller/board behavior | Remain owners of the fields listed in `src/internal.h`; state visitor records their values and invariants without copying owner logic |
| `libretro/libretro.c` and `.info` | Frontend callback and descriptor surface | `serialize_size`/`serialize`/`unserialize` delegate to library buffers; no file I/O. Advertise savestate only when the full contract passes. No serialization quirks are needed for complete portable states. |
| `runner/main.c` | CLI and local filesystem | Bounded state file read/write and atomic replacement; load media and battery before applying state; flush battery using the live generation on exit. The core never sees a path. |
| Playstead adapter/launcher | Product launch, process lifetime and resume choice | Playstead owns the adapter and pins that launch RetroArch plus muhtari. Keep runner state-file I/O optional until the actual adapter proves it is needed; do not embed a GUI or process launcher in muhtari. |

Recommended flow: load identical content and options into a fresh instance → restore raw battery if configured → validate and apply optional state → run frames → save an optional state at a completed frame boundary → host flushes changed battery bytes. In libretro, RetroArch calls serialize/unserialize and owns the state files; Playstead launches this existing host path through its own adapter/pins. This gives one tested machine format to casual save/load, rewind, and run-ahead without making the runner or libretro adapter a second serializer.

## Libretro contract and implication

Official Libretro docs require the frontend to size its buffer from `retro_serialize_size`; that size may not increase during a load/unload session, a serialize buffer may be larger than required, and fixed field offsets reduce rewind memory. Unsupported cores report size zero. The run-ahead guide describes in-memory save/restore and says states must be clean and fast enough. These are direct API facts; supporting deterministic states is the inference needed to claim reliable rewind/run-ahead. [LR.01, LR.02]

Use an exact per-load library size and keep serialization offsets stable. `retro_serialize_size()` returns that size only with loaded content; `retro_serialize()` accepts at least that much buffer space, writes the defined state, and ignores or zero-fills any tail; `retro_unserialize()` requires the expected state length and returns false on any refusal. Advertise `savestate = "true"` and the `basic`, `serialized`, and `deterministic` features only after tests demonstrate completeness. Do not mark states incomplete, session-only, platform-dependent, endian-dependent, or variable-size. [LR.01, LR.03]

## Phase ordering and acceptance

1. **Library state foundation.** Add the public API and format, then cover all supported board/expansion/battery/controller topologies in both NTSC and PAL where valid. Save → restore → continue must equal an uninterrupted instance in video, audio transition stream, PCM and canonical state bytes. Check deterministic bytes on the existing six native CI lanes and the big-endian PowerPC lane. A counting allocator must observe no post-load allocation.
2. **Host value.** Wire libretro serialization and promote `.info` claims with in-process host coverage plus a bounded RetroArch save/load proof. Drive Playstead's first-party play path through its own adapter/pins for RetroArch and muhtari. Do not route play through the headless runner.
3. **Runner and recording.** Add runner state-file input/output only if the actual Playstead adapter or a documented headless user workflow consumes it; keep size caps, wrong-media refusal, and temp-file/fsync/rename writes at that host boundary. Keep state support independent of recording/movie authoring and general continuation protocols, which remain later unless measured Playstead use requires them.

Independent failure checks: flip header/body bytes and recompute CRC with out-of-range fields; try truncation, extra bytes, invalid enum/bank/phase/PCM bounds, overflowing sizes, impossible deadline relationships, wrong ROM/BIOS/HSC firmware, wrong board/region/topology, and rejected states after a battery write. Each refusal must preserve a deep instance digest and battery generation. Exercise successful continuation at pending device/deadline/audio/input boundaries for every supported topology/region. Keep state files out of committed fixtures and release artifacts; self-authored test ROMs may be used, but generated state buffers should stay in test memory or temporary build output. [AP.01–03, PREP.01]

## Revisit triggers and unknowns

- Current Playstead uses an external GUI process and has no libretro host. Product research identifies RetroArch plus muhtari through Playstead-owned adapter/pins as the gamer path. The exact resume ownership/command boundary remains downstream product research; do not assume the older runner-only continuation protocol still applies.
- Revisit state identity fields if a host needs restore into a reconfigured machine rather than reloading the same media and options first.
- Revisit a smaller state representation only if measured rewind memory use is material; fixed offsets and uncompressed state are the simplest reliable baseline.
- The official libretro docs do not promise persistent state compatibility across core versions. State format and behavior checks should therefore refuse incompatible states explicitly; migration is a later product requirement if players need it.

## Sources and confidence

| ID | Source | Confidence | Supports |
|---|---|---|---|
| SRC.01 | Current `src/internal.h`; `src/frame.c`, `src/bus.c`, `src/cpu.c`, `src/maria.c`, `src/tia.c`, `src/riot.c`, `src/pokey.c`, `src/synth.c`, `src/ports.c`, `src/cart.c` | HIGH — direct code | Instance-owned hardware state, pending events, output pointer boundary and module ownership |
| SRC.02 | Current `include/muhtari.h`, `src/instance.c` | HIGH — direct code | API 7, behavior revision 6, load/copy, power-cycle, battery data/generation and versioning |
| SRC.03 | Current `src/ports.c`, `src/bus.c`, SaveKey fields in `src/internal.h` | HIGH — direct code | Controller, bus, EEPROM deadline and pending protocol state |
| SRC.04 | Current `src/synth.c`, `src/pokey.c`, `src/tia.c` | HIGH — direct code | Oscillator, mixer, pending sample and PCM state ownership |
| SRC.05 | Current `libretro/libretro.c`, `libretro/muhtari_libretro.info.in` | HIGH — direct code | No serialization implementation; current false capability; host callback boundary |
| SRC.06 | Current `runner/main.c`, `docs/runner.md`, `.planning/preparation/HOSTS-AND-INTEGRATION.md` §4 | HIGH — direct code/docs | Runner owns battery file I/O; no state file commands are implemented |
| SRC.07 | `ASSET_POLICY.md` §1–3 | HIGH — project policy | Prohibited game-derived/firmware-derived state artifacts and safe diagnostic fixtures |
| SRC.08 | `.planning/preparation/DECISIONS.md` DEC.26/.35; `PROVENANCE.md` §4–5 | HIGH — settled project decisions | Size-tagged versions, fixed-size byte-wise LE state, visitor, no external dependency |
| SRC.09 | Current `runner/sha256.c`, `runner/sha256.h`, `libretro/libretro.c` | HIGH — direct code | Existing owned SHA-256 reused by runner and libretro; candidate for a shared private core utility |
| PS.01 | Current Playstead architecture, per this milestone's read-only product research | HIGH — direct sibling-project inspection | External GUI process launcher; Playstead-owned adapter/pins can launch RetroArch and muhtari; no libretro host |
| LR.01 | [Libretro core development: serialization](https://docs.libretro.com/development/cores/developing-cores/) | MEDIUM — official current documentation, cross-checked against the vendored API header, accessed 2026-10-29 | Size/buffer semantics, fixed offsets, rewind/netplay use |
| LR.02 | [Libretro run-ahead guide](https://docs.libretro.com/guides/runahead/) | MEDIUM — official documentation, accessed 2026-10-29 | In-memory state save/restore and clean/fast state requirement |
| LR.03 | `libretro/libretro.h` (MIT, v1.22.2; pinned by `PROVENANCE.md` §8) | HIGH — vendored official API header | Serialization quirk flags and core/frontend ownership of quirk bits |
| AP.01–03 | `ASSET_POLICY.md` §1–3; `PROVENANCE.md` §1, §4–5 | HIGH — project policy | Keep generated state bytes out of repository/releases; prefer existing in-tree code and preserve notices |
| PREP.01 | `.planning/preparation/ENGINEERING.md` §11 | HIGH — settled project policy | Review present boundaries and concrete faults; no speculative framework or new score |
