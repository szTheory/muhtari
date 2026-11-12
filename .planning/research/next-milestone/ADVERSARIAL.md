---
title: "Adversarial review: proposed YM2151 successor scope"
summary: "Tests whether YM2151 support is a present Muhtari product need and identifies the profile and compatibility gates that constrain an honest release."
read_when: "Selecting the successor milestone after the upstream research fan-out."
updated: 2026-11-11
---

# Adversarial review: YM2151 homebrew sound

## Verdict: modify, then select the bounded YM2151 capability

YM2151 is a more defensible next Muhtari product slice than an accuracy fix,
and more directly belongs here than LibFuturo's PAL browser qualification. The
gap is not purely hypothetical: [AtariAge's current Block'em Sock'em product
listing](https://store.atariage.com/products/blockem-sockem-atari-7800) names
its developer and says the cartridge supports TIA, Yamaha and POKEY audio. The
current core parses a YM2151 request but refuses to load it before allocation
([loader](../../../src/instance.c), [refusal contract](../../../tests/core/test_core.c)).
This is source-backed intended use from a sold homebrew product, not a
reproduced Muhtari gameplay failure. The listing does not say whether Yamaha
audio is required, identify the image's exact header/map, or demonstrate how
the game behaves without it. The [Pac-Man Collection 40th developer thread](https://forums.atariage.com/topic/304268-pac-man-collection-40th-anniversary-edition/)
also says that game works without XM using TIA audio; Yamaha is an enhancement
there, not a demonstrated load or gameplay dependency. Do not claim Muhtari can
play a named title until that exact behavior is established.

Recommend one core capability: **support YM2151 on the existing XM expansion
path only**, through the existing C library, runner and libretro core, with a
generated model diagnostic and existing XM POKEY/RAM/HSC behavior retained.
Require the image's YM declaration and XM selected or auto-detected; use the
documented `$0470 = $84` enable for this profile. Keep directly mapped
YM-declared cartridges and other decoder forms refused until separately
supported by stronger evidence. This makes the proposed outcome one topology,
not a mixture of direct-cartridge and XM semantics. Scope the public claim to
this deterministic digital model; do not claim full XM emulation, broad game
compatibility, or hardware-calibrated audio.

## Alternatives

| Choice | Adversarial finding |
|---|---|
| Keep refusing YM | Safest for existing behavior and state files, but it leaves a presently marketed homebrew audio capability unavailable. A deliberate defer is valid if the profile cannot be grounded; it is not evidence that no current consumer exists. |
| Correct a hardware-accuracy/gameplay defect | No reproduced mismatch or lawful, independent gameplay oracle is identified in [ACCURACY](ACCURACY.md). The DLI disagreement is conditional on hardware evidence. Untested compatibility rows and filename tags are not failures. |
| Select YM2151 | Best current Muhtari scope. AtariAge's product page establishes intended Yamaha use; the local loader establishes the refusal. Neither proves the exact candidate image's declaration or that a game works after the load refusal is removed. |
| Finish LibFuturo PAL/browser work | A real downstream gap with partial local evidence, but it is consumer packaging, profile selection and browser qualification owned by LibFuturo. Its current PAL report is macOS-local; native Linux and exact-source hosted qualification remain open. It does not identify a missing Muhtari API or package contract. |

The PROPack filename tally (at least 35 YM-tagged images among non-retail
entries) is a lead, not a count of unique playable titles or users. The existing
claims about 1942 and Pac-Man Collection 40th are lower-tier reports, not proof
that either image was tested here. The Block'em product page and Pac-Man
developer thread establish intended Yamaha use, but do not identify the exact
image declaration/profile or demonstrate playback in Muhtari. This evidence is
sufficient to justify a bounded capability, but not to promote those titles to
acceptance fixtures.

## Gates that change the proposed scope

1. **Use one XM topology only.** Require a YM declaration and XM selected or
   auto-detected; expose `$0460/$0461` only after the selected `$0470 = $84`
   enable. Keep direct YM-declared cartridges refused. Existing records
   conflict on decoder/enable (exact `$84` versus bit-7/header declarations)
   and clock (fixed regional clock versus CPU-phase clock). The XM developer
   source remains unavailable; the Atari 7800 wiki's YM programming page is
   useful but is not the primary XM specification. The [audio preparation
   note §6](../../preparation/A7800-AUDIO.md) recommends φ2-counter ×2 pending
   clock evidence, while [HOMEBREW](HOMEBREW.md) proposes fixed regional
   crystal/2. For the bounded XM profile, adopt the fixed regional clock as a
   confidence-qualified digital model (3,579,545 Hz NTSC; 3,546,894 Hz PAL),
   citing the available timer/model sources; do not claim hardware calibration.
   Resolve and document that model before code, including why CPU slow-cycle or
   HALT phase does not retime it. DEC.11 does not require a console measurement
   before implementation; revise only if better source evidence changes the
   model. If no bounded model can be supported from available documents, defer
   the feature rather than widening it to direct carts or full-XM behavior.
2. **Carry all required device behavior.** YM timer/status behavior and its
   declared IRQ source are part of the request, not optional polish. A78 v4
   documents IRQ bit 2 for YM, while installation currently retains only bits
   0–1. Busy polling and timer deadlines must follow the core bus timeline and
   integrate with the existing dispatcher; do not add an independent scheduler.
   The `$0470` enable also shares the existing XM control path: preserve its
   current RAM/HSC/POKEY behavior while enabling YM. Keep POKEY coexistence
   only for the supported header/topology combinations.
3. **Keep Muhtari's PCM mono.** The YM chip generates stereo internally, but
   Muhtari's settled public PCM contract is mono; libretro duplicates those
   mono samples into its two host channels. Define an integer stereo-to-mono
   downmix/gain policy, transition source ordering, headroom and clipping
   behavior inside the existing mono mixer. The board-mix notes do not establish
   measured XM analog levels. YM/FM reference agreement can establish port
   equivalence, not independent chip correctness or the Atari board's analog
   mix.
4. **Keep provenance and portability explicit.** The approved seed is BSD-3
   ymfm OPM. A specialized C17 port is consistent with the runtime contract,
   but its 1,500–2,000 line estimate is untested, not a cost guarantee. Retain
   the exact notice and pin under [provenance policy](../../../PROVENANCE.md),
   keep the runtime integer-only and allocation-free after load, and measure
   native/Wasm cost on representative YM traffic. A C++ wrapper would conflict
   with the project's C17 portability target and expand toolchain requirements.
5. **Decide state compatibility explicitly.** Loading a new device changes
   behavior identity and may add chip, IRQ/timer and audio state. Phase
   discussion must decide the behavior revision, state-format consequence, and
   how existing non-YM states behave; a format bump is likely if the serialized
   body changes, but is not an already-approved schema change. No API/ABI bump
   follows if public structures stay unchanged. Old-state behavior and
   LibFuturo's exact-build envelopes are distinct contracts; do not imply
   migration.

## Acceptance boundary and non-claims

A useful release outcome is a generated diagnostic declaring the selected XM
profile that loads, exercises the `$84` gate and timer/status path, produces
non-silent independently checked numeric mono audio, survives save/load
continuation, and runs through the public library, runner and loaded libretro
path. Compare C-port output against the pinned ymfm seed to show port
equivalence, and compare separate documented/chip-derived expectations before
hashes to test the chosen model. Neither result is physical hardware evidence.
Retain refusals for direct YM and other unsupported topologies plus non-YM/POKEY
regressions, and qualify the portable artifact if changed behavior reaches it.
No console, RetroArch, browser or owner-run check is required by this scope.

The milestone cannot honestly ship a claim that Block'em Sock'em, 1942 or
Pac-Man Collection 40th is playable without a lawful fixture and a game-level
oracle. It also cannot ship “YM2151 hardware accuracy,” complete XM support,
measured analog balance, or LibFuturo PAL browser acceptance from generated
model tests. Current research performed no build, test, CI, emulator, host,
asset or hardware run.

**Confidence:** high that the core refuses declared YM requests and that a
currently sold 7800 homebrew advertises Yamaha sound; medium that this is the
best next feature; low on the exact XM map/clock and any named-game result.
**Revisit:** a primary XM specification or an independently measured clock/map
changes the supported profile; a lawful game-level oracle changes the title
acceptance claim; or profile evidence cannot support a bounded implementation.
