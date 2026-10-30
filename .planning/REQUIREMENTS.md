# Requirements: muhtari v3.0 — Playstead-ready play and states

**Defined:** 2026-10-29
**Core value:** 7800 games behave as on the console, identically across platforms,
from a small MIT-licensed C library hosts can embed.

Owner approved researched recommendations, then prioritized ordinary gaming through
Playstead and save/load states over recording, AI and TAS. This supersedes the
initial broad “States and movies” label. The existing three deliverables remain.

## v3.0 Requirements

### Player-ready delivery and bounded tune-up

- [ ] **TUNE-06**: Maintainers can ship a small improvement to demonstrated pipeline friction selected from the recurring tune-up seeds, with representative pass/fault evidence and preserved required contracts; unselected COST-01, WR-01 and optional debt keep explicit dispositions rather than becoming a general audit.
- [ ] **TUNE-07**: Maintainers can distinguish complete successful nightly-property measurements from censored or cancelled work, and receive evidence-based deadlines or a bounded explicit deferral while every case/oracle and advisory status is preserved.
- [ ] **PLAY-01**: A gamer can find the current binaries, install the correct RetroArch core, load a local game through GUI or CLI, understand controls and optional BIOS, and find honest save/compatibility limits from a short current README whose install command is automatically checked.
- [ ] **PLAY-02**: A frontend integrator can consume a release-qualified external RetroArch/muhtari launch contract covering exact artifact identity, per-game content/save paths, optional firmware, usable input, process exit and refusal reporting, without treating the headless runner as an interactive player.
- [ ] **PLAY-03**: A frontend integrator has automated owned-fixture evidence of visible output, input delivery and fresh-process battery continuity through the released libretro core and actual RetroArch; final Playstead launch/input/exit acceptance is separately identified and never inferred from these host-readiness checks.
- [ ] **PLAY-04**: The Playstead team can use a source-linked handoff that identifies its adapter/pin/readiness/state responsibilities, the quickest playable route, a native-host north star and its promotion triggers, and the exact evidence still needed for the actual cross-project flow.

### Portable and safe machine state

- [ ] **STATE-01**: A library caller can query a state size fixed for the loaded machine and save/restore a complete versioned byte-wise little-endian snapshot, including mutable supported chip/device/board/region state, pending work and emulated battery data, using caller-owned buffers.
- [ ] **STATE-02**: A caller can reject truncated, corrupt, unsupported or checksum-valid hostile states and incompatible media/firmware/configuration before any live-machine mutation; a failed restore preserves subsequent execution and host-visible storage metadata.
- [ ] **STATE-03**: A caller can save, restore and continue supported machine topologies with the same independent semantic observations, frame output and raw/PCM audio as an uninterrupted run, with portable bytes and equality across the existing required native/big-endian lanes; advisory lanes remain labeled.
- [ ] **STATE-04**: A host can use save/load repeatedly without core allocation after content load or unbounded work; hostile restored clocks, device deadlines and lengths cannot hang or overrun the core.
- [ ] **STATE-05**: A host can distinguish restored emulated battery bytes from durable save files: changed restores notify dirty tracking without restoring an old generation token, equal/refused restores preserve the appropriate metadata, and speculative execution cannot commit battery writes to host files.
- [ ] **STATE-06**: A runner user can save at a chosen frame, load a state in a fresh process and hash state for repeatable continuation, with checked file I/O, explicit failure status and preservation of live state/durable files on refusal.

### Frontend save/load and responsive play

- [ ] **FRONT-01**: A libretro host can query stable serialization size and save/load through the same library state contract, with truthful version/feature declarations and safe unloaded/failed-load behavior.
- [ ] **FRONT-02**: A gamer can save and load through the pinned RetroArch host and resume the expected picture and progress, demonstrated by an automated owned-fixture round trip rather than only an in-process callback test.
- [ ] **FRONT-03**: A gamer can use RetroArch rewind with repeated restores preserving deterministic input, audio, timing and battery semantics; rewind storage and UI remain frontend-owned.
- [ ] **FRONT-04**: A gamer can use the supported RetroArch run-ahead path with verified restore/replay and instance isolation, no speculative persistent writes and measured completed-work performance; capability claims match the pinned host evidence.

## Cross-project acceptance boundary

This repository owns muhtari capabilities, release artifacts and host-readiness
evidence. Playstead owns its adapter/installer, verified cache, `a7800` readiness,
controller/focus handoff, session lifecycle and progress UI. It must demonstrate
the actual Playstead → RetroArch → muhtari launch and orderly return using current
sources/pins; this is an external integration dependency, not a muhtari-only pass.
Initial state UI is RetroArch-managed; Playstead-managed exact Continue/history
requires a supported capture/load boundary and recovery of newer durable progress.
Only read-only Playstead research and a handoff occur in this milestone-start step.

## Future Requirements

- Browser/WebAssembly player: preferred next milestone candidate, reweighed at v3.0 close.
- Playstead-owned native libretro hosting, integrated state history and one-app Continue: promote from measured external-host friction or an explicit product requirement.
- Full movie recording, generalized continuation, BK2 import, AI/Gymnasium, TAS and speedrun tools: real consumer need; existing replay can serve state verification.
- Measured hardware accuracy, deferred audio/boards, ports, power budgets and 2600 mode: retain existing horizon/seed triggers.

## Out of Scope

| Feature | Reason |
|---|---|
| muhtari GUI, audio/input devices, rewind buffers or frontend state UI | Host-owned; preserve three deliverables. |
| Implementation in the Playstead checkout during milestone scoping | Separate session/owner; handoff and explicit paired acceptance only. |
| Full recording/AI/TAS before ordinary play and save/load | Gamer-first owner steering; not needed for first viable integration. |
| Cross-version state migration or universal netplay/state compatibility | No demonstrated consumer; refuse incompatible formats honestly. |
| Bundled ROMs, BIOS, personal saves/states or commercial captures | Existing asset/provenance policy. |
| New general audit, metrics framework, quotas or extra review rounds | Proportional gates and existing bounded review/gap policy. |
| Automatic advisory-lane promotion or universal gameplay claims | Requires independent qualifying evidence, not milestone scope alone. |

## Traceability

Filled by the milestone roadmapper. Every requirement maps to exactly one phase.

| Requirement | Phase | Status |
|---|---|---|

---
Research: [summary](research/SUMMARY.md); owner intent: [INTENT.md](INTENT.md).
