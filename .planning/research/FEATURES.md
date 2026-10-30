# Feature Landscape: muhtari v3.0

**Domain:** Atari 7800 emulation and Playstead Mac launch integration
**Researched:** 2026-10-29
**Overall confidence:** MEDIUM — current code and product sources are direct evidence; the proposed Playstead-to-RetroArch launch has not yet been exercised end to end.

## Recommendation

Use two horizons. First, make Playstead launch its verified 7800 content in an external, pinned RetroArch process with the muhtari libretro core. This is the shortest path to real video, sound, and controller input without adding a fourth muhtari deliverable. Playstead must own the adapter change because its current adapter is a single standalone application; muhtari must provide the core and its release evidence. Then deliver portable, validated core states through the C library, runner, and libretro callbacks so RetroArch can provide save/load, rewind, and run-ahead.

The product-aligned destination is a Playstead-owned in-process libretro host. It could keep play, Continue, controller focus, and Playstead state history in one app. That is Playstead work: it would implement video, audio, input, pacing, and core lifecycle. Promote it after the external launch exposes recurring window, controller, or state-management friction, or when a single-app Playstead experience becomes a release requirement. Keep muhtari limited to the library, headless runner, and libretro core.

## What Exists Today

- muhtari already has the three deliverables: C library, headless `muhtari-run`, and libretro core. The inspected source reports API 7 and behaviour revision 6 (`include/muhtari.h:12`, `src/instance.c:9-13`). It has raw HSC/EEPROM persistence and typed movie2 replay, but no complete state serialization or recording yet (`docs/runner.md:3-12, 21-29, 44-51`; `docs/libretro.md:223-228`).
- `muhtari-run` is explicitly for fixed-frame automation. It has no window, audio device, or input device (`docs/runner.md:3-5`). Its movie playback is useful for reproducible tests, not an interactive player.
- The libretro core already gives a frontend the 7800 picture, audio, pin-level input, and raw battery RAM. The frontend restores and writes `.srm` files; muhtari does not perform host file I/O (`docs/libretro.md:223-228`). Save states remain unavailable.
- Playstead’s current Mac flow materializes one verified catalogue ROM, prepares a per-game save directory, performs readiness checks, and launches a verified emulator executable as a child process. `AdapterHost` passes the ROM and save paths through a pin-controlled argument list and observes process exit (`playstead-mac/Playstead/Library/GameRowView.swift:1032-1109, 1145-1203`; `playstead-mac/Playstead/Adapter/AdapterHost.swift:150-160, 418-510`).
- The current Playstead pin is only GBA + standalone mGBA 0.10.5 (`playstead-mac/Playstead/Adapter/AdapterPin.json:1-21`). Its catalog describes one adapter (`playstead-mac/Playstead/Adapter/AdapterCatalog.swift:3-7, 53-66`), and the installer mounts a DMG and copies one `.app` (`playstead-mac/Playstead/Adapter/AdapterInstaller.swift:351-399`). There is no muhtari/7800 pin or verified Playstead launch path in the inspected source.
- Playstead does not currently host libretro callbacks. Under the external-process route, RetroArch owns the game window and audio/input drivers; muhtari supplies emulation and callbacks. Libretro’s official developer guide describes that frontend/core boundary and the serialization contract: [Libretro core development guide](https://docs.libretro.com/development/cores/developing-cores/). The GSD confidence classifier returned MEDIUM for the verified `websearch` source.
- Playstead’s actual `Continue` list is derived from recent play and starts the same game row; it does not restore an emulator state (`playstead-mac/Playstead/Curation/ContinueShelfView.swift:3-24`; `PlaySessionRecorder.swift:3-7, 24-34`). Its own manual state UI is real but currently qualified only for a local GB/MBC1/8 KiB runtime, not 7800 (`playstead-mac/docs/STATE-SUPPORT.md:3-17`; `playstead-mac/Playstead/Adapter/StateSupportMatrix.swift:40-60`; `playstead-mac/Playstead/Saves/StateActionsView.swift:195-214`).
- Playstead product goals call for offline local play, keyboard/controller/assistive paths, and compatible progress (`PRODUCT.md:22-41, 57-67`). The brand promise is that Continue picks up where play stopped (`brand/BRAND.md:27-41, 78-101`); an ordinary recent-play entry alone does not meet that promise. Keep Playstead’s product copy honest until a resume mechanism is actually wired.

## Table Stakes

| Feature | Why expected | Complexity | Recommendation and boundary |
|---|---|---:|---|
| **Play a verified 7800 game from Playstead** | The immediate owner outcome is to choose a 7800 title in the actual library and reach an interactive game. A working runner or a separate RetroArch shortcut does not prove that path. | Medium | Add Playstead `a7800` readiness and adapter support. For the quick path, pin RetroArch and the muhtari core as two separately verified artifacts, launch RetroArch with the verified materialized ROM, and keep Playstead’s process/session lifecycle. RetroArch supplies the interactive window, sound, and input; muhtari stays a core. |
| **Controller and keyboard handoff** | Playstead expects controller-first navigation and complete keyboard/accessibility fallbacks. Its current Play action requires an assigned, attested controller (`GameRowView.swift:1040-1048`), while the external host owns gameplay input. | Medium | Verify how the assigned controller reaches RetroArch and how focus returns to Playstead on exit. Do not report a controller mapping as active until the host receives it. Preserve a usable keyboard route. The current mGBA/SDL config bridge is adapter-specific and should not be assumed to configure RetroArch. |
| **Per-game battery continuity** | Ordinary launch/relaunch must retain cartridge-backed progress when the game supports HSC or SaveKey. This is distinct from a savestate. | Medium | Give RetroArch the Playstead per-game save location and bind saves to the verified headerless ROM identity and declared medium. Verify a real two-launch restore using an allowed diagnostic fixture. muhtari exposes raw battery memory; Playstead owns the cache/save path and custody; RetroArch owns `.srm` I/O in the external route. Do not equate a recent-play event with resumed game progress. |
| **Portable, transactional emulator states** | Save/load states are a high-priority user capability and enable RetroArch save/load, rewind, and run-ahead. A state must not partially mutate a live instance when invalid or incompatible. | High | Add versioned deterministic state operations to the existing C API, runner, and libretro callbacks. Bind states to ROM and effective machine configuration; validate completely before apply; expose stable state size and metadata only after the supported contract is proved. Keep serialization integer-only, bounded, and cross-platform. The actual on-disk format and bindings belong to the architecture decision. |
| **Bounded measured tune-up** | This milestone includes recurring tune-up work, but prior pipeline and property debt must not expand into a general audit. | Low–Medium | Use selected tune-up seeds only: resolve demonstrated CI/pipeline friction and measure the weakest relevant quality dimension. Preserve known COST-01, WR-01, and advisory-property debt unless new evidence changes scope. Ship a measurable improvement rather than a new metrics system. |

## Differentiators

| Feature | Value | Complexity | When to pursue |
|---|---|---:|---|
| **Playstead-owned in-process libretro host** | One Playstead app can own launch, display, audio, controller focus, pause/exit, and an integrated state surface. It better fits Playstead’s calm one-press Continue than switching into a separate RetroArch window. | High, Playstead-owned | Keep as the north star. Promote when the external path demonstrates recurring context-switch/controller friction or cannot support the Playstead state experience the product requires. First establish the external launch and state contracts. The host implements libretro A/V/input callbacks; muhtari adds no device or window code. |
| **Playstead-managed state history for 7800** | A 7800 state could appear in Playstead’s existing history and compatibility flow rather than only in RetroArch’s slot UI. | High, Playstead-owned | After the core state contract and a working host can capture/load named state artifacts. The current history is local-only for its qualified GB row; cross-device state custody requires compatibility and recovery rules, not just syncing opaque bytes. Keep battery saves and emulator states as separate artifact types. |
| **Deterministic recording and continuation evidence** | Captured input plus a fixed state can make gameplay regressions and continuation reproducible. muhtari already replays movie2 input; recording is not present. | Medium–High | Add recording only when an actual player or host needs it. For early verification, use existing movie replay and fixed allowed fixtures where sufficient. The Playstead continuation test protocol currently reports blocked capability for its private adapter because deterministic input replay and a repeatable visible oracle are not qualified (`playstead-mac/docs/CONTINUATION-PROTOCOL.md:7-13, 38-67`); that is not a prerequisite for launching 7800. |

## Anti-Features

| Do not build | Why | Use instead |
|---|---|---|
| A muhtari GUI, window, audio device, or input device | Violates the three-deliverable boundary and duplicates frontend responsibilities. | RetroArch first; a Playstead-owned native host later, implemented in Playstead. |
| Treat `muhtari-run` as the Playstead game player | It is headless and has no live input path. | Launch the libretro core in RetroArch for the immediate playable route. Keep the runner for hashes, state file operations, replay, and automated checks. |
| Bundle or embed RetroArch source/binary in muhtari | RetroArch is a separate GPL frontend and is not a muhtari deliverable. | Playstead pins/installs a released RetroArch app separately from the MIT muhtari core; the future Playstead host consumes the libretro API. |
| Call Recent/Continue a restored emulator session before it actually restores one | Playstead’s current shelf is recency-based, and battery RAM only resumes games that persist progress themselves. | Describe what the current path restores; add a state-aware Continue only after Playstead owns and verifies that state operation. |
| AI play, TAS authoring, Gymnasium, browser play, or generalized scripting in v3.0 | These serve later specialist personas and do not shorten the immediate path to a real Playstead launch or reliable save/load. | Preserve deterministic APIs and defer those tools to their roadmap horizon. |

## Integration Options and Ownership

| Route | User result | Cost / limitation | Owner |
|---|---|---|---|
| **External RetroArch process (recommended first)** | Playstead’s library action opens a real interactive 7800 session quickly; RetroArch can consume the muhtari core and later provide its normal state UI. | Requires a Playstead adapter that verifies two artifacts (RetroArch app + core archive), passes the ROM/save/optional BIOS paths, and handles process/input lifecycle. Gameplay and state UI initially live in another window. | Playstead owns adapter pin, install/verification, `a7800` readiness, launch, controller handoff, per-game save custody, and any Playstead-facing state actions. muhtari owns the core, release artifact/digests, state serialization, runner state operations, and libretro metadata/callbacks. |
| **Playstead-owned in-process host (north star)** | One app/window with native Playstead focus, accessibility, and potential direct state-history integration. | Substantially more client engineering: dynamic core loading, real-time frame pacing, video presentation, audio buffering, controller/keyboard mapping, pause/exit, and error/lifecycle recovery. Core failures share the app process. | Playstead implements the host and native UX. muhtari maintains its existing C, runner, and libretro deliverables and provides the stable core contract. |
| **Headless runner** | Strong automation and repeatable content/state checks. | No interactive display/audio/input; not a gamer launch path. | muhtari runner; not the Playstead adapter target for play. |

**Promotion trigger for the in-process host:** the first real external launch works, then the separate RetroArch window prevents Playstead’s one-app Play/Continue flow, loses or misroutes the assigned controller, or blocks the Playstead-owned state experience that the product decides to promise. Reassess with one focused host spike that proves video, audio, input, lifecycle, and state capture; do not start by redesigning all emulator adapters.

### Playstead Handoff Checklist

- [ ] Add `a7800` as an explicitly supported system with readiness that names the actual missing remedy.
- [ ] Pin RetroArch and the muhtari core separately; verify each archive and installed executable/core digest. Keep RetroArch as a released external binary.
- [ ] Extend the adapter install/launch contract for the two artifacts, materialized ROM, optional user BIOS, isolated config, and per-game save directory.
- [ ] Prove controller handoff, keyboard fallback, window focus return, child cleanup, and launch refusal for a mismatched artifact.
- [ ] Keep battery `.srm`/raw HSC or SaveKey bytes distinct from emulator state files; prove a fresh-process battery restore.
- [ ] Describe initial manual state support as RetroArch-managed. Add Playstead state history only after its compatibility and preservation gates can consume the muhtari state contract.
- [ ] Use an allowed manifest fixture or CI-built fixture for recurring E2E; keep user ROMs, BIOS, saves, states, and screenshots out of releases.
- [ ] Revisit the in-process host only when the promotion trigger above fires.

## Feature Dependencies and Roadmap Order

```text
Playstead a7800 support + verified RetroArch/core artifacts
  → materialized ROM, optional BIOS, per-game save path, input handoff
  → actual Playstead → RetroArch → muhtari launch proof
  → two-launch battery persistence/resume proof
  → C-library state contract + runner state operations + libretro serialization
  → RetroArch save/load, rewind, and run-ahead backed by deterministic states
  → Playstead-native state history/in-process host if the promotion trigger fires
  → input recording/continuation tools when a real gamer or host outcome needs them
```

The earliest E2E proof should precede advanced deterministic tooling: prove Playstead can launch the core and preserve a supported cartridge save through process restart. Then deliver the high-priority state contract through the existing three muhtari deliverables. Movie replay already exists for bounded repeatable runs; movie recording and a new general-purpose continuation framework are not launch blockers.

## MVP Recommendation

Prioritize:

1. **Actual Playstead launch:** a pinned, verified RetroArch app and muhtari core can launch one supported 7800 fixture from Playstead’s verified local cache and return cleanly to the app.
2. **Ordinary progress continuity:** the same fixture demonstrates raw HSC/SaveKey save restoration in a fresh process; no ROM, BIOS, save, state, movie, or screenshot from a commercial title enters the repository or release.
3. **High-priority states:** portable save/load works through the C library, runner, and libretro interface; actual RetroArch load/save and deterministic feature declarations are tested before being advertised.
4. **Small measured tune-up:** fix only demonstrated budget or friction issues from the selected seeds.

Defer Playstead-managed cross-device state history until it can bind and validate the state and preserve the user’s current progress. Keep the in-process host as a named north star with the promotion trigger above, not an assumed fourth muhtari component.

## Gaps to Resolve During Phase Research

- Pin the first supported RetroArch release/architecture and core archive; prove the launcher arguments, isolated config, save directory, and process exit behavior on the actual Playstead Mac path.
- Resolve how Playstead’s assigned controller is handed to an external RetroArch process and ensure keyboard/accessibility paths remain viable. The current app requires controller assignment and its tested mapping bridge is mGBA-specific.
- Decide whether the first Playstead state outcome is RetroArch-managed save/load only or also Playstead-managed named states. These are different ownership and compatibility contracts.
- Define a state compatibility tuple from the actual supported ROM, region, board/devices, BIOS/boot, options/profile, state format version, and behaviour revision; reject mismatches transactionally.
- Choose a legal fixture already listed in `tests/roms/manifest.txt` or built/fetched in CI. `ASSET_POLICY.md:12-32, 54-56` forbids shipping game/firmware/save/state bytes outside its explicit fixture policy.

## Sources and Confidence

- **muhtari first-party source, checkout `e8ddf07cd07e`:** `AGENTS.md:3-4, 11-25`; `include/muhtari.h:12`; `src/instance.c:9-13`; `docs/runner.md:3-12, 21-29, 44-51`; `docs/libretro.md:223-228`; `.planning/preparation/HOSTS-AND-INTEGRATION.md:125-172, 198-210`; `.planning/preparation/DECISIONS.md:92-100`; `ASSET_POLICY.md:12-32, 54-56`. **Confidence: HIGH** for present repository contracts and capabilities.
- **Playstead first-party sources, checkout `3e12f16a75ec`:** `PRODUCT.md:22-41, 57-67`; `brand/BRAND.md:27-41, 78-101`; `playstead-mac/Playstead/Adapter/AdapterPin.json:1-21`; `playstead-mac/Playstead/Adapter/AdapterCatalog.swift:3-7, 53-66`; `playstead-mac/Playstead/Adapter/AdapterInstaller.swift:351-399`; `playstead-mac/Playstead/Adapter/AdapterHost.swift:150-160, 418-510`; `playstead-mac/Playstead/Library/GameRowView.swift:1032-1109, 1145-1203`; `playstead-mac/Playstead/Curation/ContinueShelfView.swift:3-24`; `playstead-mac/Playstead/Curation/PlaySessionRecorder.swift:3-7, 24-34`; `playstead-mac/docs/STATE-SUPPORT.md:3-17`; `playstead-mac/Playstead/Adapter/StateSupportMatrix.swift:40-60`; `playstead-mac/Playstead/Saves/StateActionsView.swift:195-214`; `playstead-mac/docs/CONTINUATION-PROTOCOL.md:7-13, 38-67`. **Confidence: HIGH** for directly observed behavior, MEDIUM for integration estimates. The Playstead working tree was dirty: `AdapterHost.swift`, `AdapterInstaller.swift`, `AdapterPin.json`, `StateSupportMatrix.swift`, `GameRowView.swift`, `playstead-mac/README.md`, and `docs/STATE-SUPPORT.md` were modified; `docs/CONTINUATION-PROTOCOL.md` was untracked. Those citations refer to the inspected worktree at that revision and should be rechecked before pinning an adapter.
- **Official external technical source:** [Libretro core development guide](https://docs.libretro.com/development/cores/developing-cores/) says the frontend owns A/V/input callbacks and documents `retro_serialize_size`, `retro_serialize`, and `retro_unserialize`. `gsd-tools query classify-confidence --provider websearch --verified` returned **MEDIUM**; cross-repo launch fit remains an inference until tested.
