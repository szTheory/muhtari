---
title: "Homebrew hardware scope: YM2151 versus Souper"
summary: "Bounded evidence for whether Muhtari should add YM2151 sound, pursue Souper/BupChip, or defer new cartridge hardware."
read_when: "Selecting and scoping the next Muhtari homebrew-hardware outcome."
updated: 2026-11-13
---

> **Historical comparison.** This document compared YM2151 with Souper before
> v6.0 shipped. The YM2151 recommendation was adopted and shipped as v0.14.0;
> its Souper defer was revisited after new first-party and mapper-profile
> evidence. The current v7.0 recommendation, source confidence and limits are
> recorded in [v7-SOUPER.md](v7-SOUPER.md).

# Homebrew hardware candidate

## Recommendation

Choose **one declared YM2151 audio profile** as the next concrete Muhtari feature, subject to a pre-implementation resolution of the map/clock boundary below. This is a missing capability with a legal route to useful repeatable acceptance: Muhtari already recognizes YM declarations but refuses to load them, and a self-authored register diagnostic can exercise the new behavior without distributing game or firmware bytes. No defect in existing YM playback is claimed because playback is not implemented.

Keep the claim to a YM2151 register/audio path. Do not promise full XM support or that *Pac-Man Collection 40th* “plays” or is compatible. Its developer described it as an XM/Yamaha title and also said it works without XM using TIA audio; this supports the intended music benefit, not a game-dependency or gameplay claim. The consumer evidence and its limits are also summarized in [CONSUMERS](CONSUMERS.md).

## Current boundary and candidates

| Choice | Evidence and user outcome | Cost and limitation | Assessment |
|---|---|---|---|
| Defer new hardware | Current YM refusal is explicit and tested. No behavior, state, or audio risk is added. | Declared YM carts and XM/Yamaha music remain unavailable. | Safe fallback if the profile cannot be bounded. |
| **YM2151 (recommend)** | First-party homebrew material names XM/Yamaha use; the programming reference documents `$0460/$0461` and YM status. A legal generated diagnostic can cover register access and sound. | A C17 OPM port is estimated at 1,500–2,000 lines (estimate, not measured), plus synthesis, audio/hash, lifecycle and state integration across the library, runner, libretro and Wasm. Clock, enable details and analog mix remain uncertain. | Best bounded new core capability, if its profile is settled before coding. |
| Souper / BupChip | *Rikki & Vikki* is a named Souper consumer. Its developer says a MiSTer route runs without BupChip but loses music. Souper has identifiable bank/mode registers and is a genuine gameplay-board alternative. | The game uses Souper MARIA remapping and custom BupChip music firmware; the developer reports no standalone raw image. A mapper-only slice has no ready lawful game-level fixture, while full music adds unavailable/proprietary firmware behavior. | Strongest hardware alternative, but materially broader and less independently testable now. |
| Accuracy/gameplay fix | Could improve an existing title if a reproduced mismatch and lawful discriminating scenario existed. | The parallel accuracy review found no demonstrated defect or ready lawful gameplay oracle. A measurement-only milestone has no guaranteed player-visible change. See [ACCURACY](ACCURACY.md). | Revisit when a specific counterexample exists. |

## Exact supported/refused scope

**Implemented today:** the cart probe preserves the v3 YM2151 cart-type declaration and v4 audio bit 3 as `MUHTARI_SOUND_YM2151`; the loader rejects YM2151, Covox and ADPCM before allocation. The partial XM implementation covers its documented RAM/HSC/POKEY behavior; it does not expose YM detection or YM registers. Souper is recognized as a board but remains refused. See [cart declaration parsing](../../../src/cart.c#L185), [load refusal](../../../src/instance.c#L355), [refusal test](../../../tests/core/test_core.c#L2379), and [cartridge behavior docs](../../../docs/hardware/cartridge.md).

**Proposed boundary, narrowed in synthesis:** one YM2151 within the existing selected/autodetected XM expansion topology, with address/data at `$0460/$0461` and one explicit enable profile. The candidate profile is YM-visible only after `$0470` receives `$84`; that latch action must be reconciled with the already implemented XM RAM/HSC controls before implementation. Direct YM declarations outside the qualified topology continue to refuse; they are not a second implicit profile. The profile supports the chip’s address/data, busy/status, timers and documented IRQ behavior only where the owning board source establishes the route. Keep current XM RAM/HSC/POKEY behavior intact. Continue refusing Covox, ADPCM, Souper/BupChip, other YM topologies and unsupported XM aliases/services.

**Clock assumption to evaluate:** fixed regional crystal/2, i.e. 3,579,545 Hz NTSC and 3,546,894 Hz PAL, independent of CPU slow-cycle stretch; one stereo sample per 64 YM clocks. The implied NTSC output is about 55,930 Hz. This is a declared digital model, not proven XM hardware timing. Existing preparation records disagreement: MAME’s permitted implementation uses 2× slot clock and `$84`, while MiSTer’s facts-only decoder uses header bit 11 or control bit 7 and clocks from CPU phase enables. Physical XM clock and level are unverified. If a higher-confidence map or clock source changes the enable, slow-cycle/HALT behavior, IRQ route, or bounded call schedule, stop before implementation and rescope; do not silently widen to full XM.

## Minimum meaningful vertical slice and acceptance

1. **Library:** load an image declaring the selected one-chip/XM profile; a generated program writes YM address/data through the declared bus map, observes independently specified busy/timer/status behavior, and emits non-silent mono PCM through the existing public contract. Test enable gating and preserve fail-before-mutation behavior for unsupported declarations. Use the project’s existing instance and allocation model.
2. **Audio and determinism:** fold internal YM left/right output into the documented integer mono mixer and existing runner audio-observation path; retain libretro's duplicated-mono output. A public stereo API is excluded. Update source identity/hash accounting without changing TIA/POKEY-only output. Check generated PCM numerically before freezing hashes; repeat the same inputs and require identical transition and PCM hashes across supported native platforms and Wasm.
3. **State and lifecycle:** capture all YM synthesis/timer/phase state in validated state; save and restore mid-register stream and compare the subsequent audio stream byte-for-byte. Cover reset/power-cycle behavior, instance isolation, partial frames, and corrupt/unsupported state rejection without mutating a live machine. State-format/behavior compatibility consequences must be explicit; current state is format 1, behavior revision 6.
4. **Three deliverables:** drive the same generated fixture through the public C library, `muhtari-run --hash-audio`, and the loaded libretro core. Verify libretro callback cadence and stereo output through the existing adapter. Include the existing portable Wasm consumer; use current CI and test mechanisms, with no new framework or workflow topology. Retain the required test-card Ir gate, but do not treat it as a YM workload bound.
5. **Correctness separation:** compare the C specialization with the exact BSD-3 `ymfm` pin (`81aec25ccbb9`) as a port-equivalence oracle. Separately test busy timing, timer periods/flags and any established IRQ route against Yamaha documentation and the selected board profile. Port parity is not independent hardware accuracy. Analog board gain and hardware listening remain unmeasured and must be documented as such.
6. **Bound and resource evidence:** measure ordinary playback and worst legal register-write/timer activity through complete frames on the lowest supported native target and Wasm. Record frame-time headroom, memory/state growth and integer bounds before setting an acceptance threshold. The test-card Ir result and an unmeasured sample-rate estimate do not establish this workload’s cost. Run the existing `cmake --workflow --preset ci` only during a later authorized implementation phase.

## Provenance, confidence, and revisit triggers

| Claim | Evidence class and source | Confidence / limit |
|---|---|---|
| Muhtari recognizes but refuses YM; XM is partial; Souper is refused | Direct repository code and tests: `src/cart.c`, `src/instance.c`, `tests/core/test_core.c`; [cartridge docs](../../../docs/hardware/cartridge.md). | High for current behavior. |
| `$0460/$0461`, busy bit, XM enable and timer diagnostic | Curated 7800 developer wiki, [YM2151 Programming](https://7800.8bitdev.org/index.php/YM2151_Programming) and [YM2151 Detection](https://7800.8bitdev.org/index.php/YM2151_Detection), revisions 734/869, indexed in [A7800-AUDIO §6](../../preparation/A7800-AUDIO.md). | Medium: useful protocol facts; original XM specification is inaccessible and alternate decoders disagree. |
| Chip busy/timer/IRQ behavior | Yamaha [YM2151 application manual](https://map.grauw.nl/resources/sound/yamaha_ym2151_synthesis.pdf); regional crystal/clock derivation in [A7800-AUDIO §1, §6](../../preparation/A7800-AUDIO.md). | High for documented chip behavior; medium/low for how XM wires and clocks it. |
| YM seed legality and maintenance | Repository [PROVENANCE.md](../../../PROVENANCE.md) audits `ymfm` `81aec25ccbb9` as BSD-3 and approves `src/ymfm_opm.*`, `ymfm.h`, `ymfm_fm.*`. The source may seed a specialized integer C17 module with notice retained. | High for this pin and these files only; recheck exact license/file headers if the pin changes. Avoid GPL/LGPL/unlicensed emulator code. |
| Player use | First-party [Pac-Man Collection 40th developer thread](https://forums.atariage.com/topic/304268-pac-man-collection-40th-anniversary-edition/); preparation’s metadata inventory records lower-bound YM/XM tags, not verified use. | Medium for intended YM music use; not proof of gameplay compatibility or required chip. |
| Souper fallback cost | First-party [Rikki & Vikki developer thread](https://forums.atariage.com/topic/285896-rikki-vikki/); `A7800-CARTRIDGES-AND-PERIPHERALS.md` §2. | Medium for the developer’s distribution and firmware statements; no raw lawful game fixture established here. |

Revisit the recommendation if a trusted XM schematic/spec resolves the map, clock, or IRQ conflict differently; if porting cannot meet measured target headroom within the C17 integer/bounded design; or if a lawful, discriminating existing-game reproducer makes another feature more valuable. No ROM or firmware bytes are needed or retained for the proposed diagnostic acceptance.
