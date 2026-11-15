---
title: "Phase 24: Qualify the GameDrive audio contract — Research"
summary: "Qualifies the published RetroHQ GameDrive streamed-audio profile and records the remaining contract gaps without proposing an API or state change."
read_when: "Planning Phase 24 and any later decision about supporting GameDrive streamed ADPCM."
updated: 2026-11-15
---

# Phase 24: Qualify the GameDrive audio contract — Research

**Researched:** 2026-11-15<br>
**Domain:** RetroHQ Atari 7800 GameDrive streamed ADPCM documentation and Muhtari compatibility<br>
**Confidence:** MEDIUM

<phase_requirements>
## Phase Requirements

| ID | Description | Research Support |
|----|-------------|------------------|
| GDA-01 | The qualification records an evidence-backed status for each needed GameDrive audio contract field, including command/register behavior, sidecar discovery and identity, stream framing, playback clock, sample/output mixing, pause/resume/reset, error behavior and state continuation; it separates sourced facts, inference and unknowns and includes source revision and confidence. | The evidence ledger below covers each field and distinguishes first-party documentation, local Muhtari contracts, inference, unknowns, and the source revision/access date. |
| GDA-02 | The qualification establishes whether legally authored generated fixtures and an independent expected-PCM oracle can exercise the documented profile without using unlicensed sample/encoder code or copyrighted game data; if not, it records the precise missing evidence and a no-go result. | Generated fixtures and a separately authored scalar oracle are feasible in principle. The current public documents do not define enough of the `.bup` profile to generate and independently predict a conforming stream. This is a profile-specification gap, not a legal block on authoring fixtures. |
| GDA-03 | The qualification evaluates caller-owned media identity and bounds, deterministic mono output, finite work/memory, lifecycle behavior and save/load continuation against existing Muhtari consumers; it documents possible public API/ABI and state-format consequences without implementing or selecting them. | The compatibility note records the current media, audio, state and consumer surfaces and the questions a later decision must settle. It selects no API, ABI, state format, limit, or implementation. |
| GDA-04 | The milestone ends with an evidence-backed go/no-go recommendation, strongest alternative, unresolved contradictions, rejected options and concrete revisit triggers; a no-go is accepted when the source or independent-oracle threshold cannot be met. | Recommendation: no-go for implementation against the current public profile; keep the horizon open until a normative profile and independent fixture/oracle pass are available. |
</phase_requirements>

## Summary

RetroHQ’s public developer post documents a GameDrive streamed-audio feature at `$420`, enabled in a ROM using `adpcm@420`. It describes stereo ADPCM sidecar tracks named `0.bup` through `31.bup` in a sibling folder named after the A78 file, and names `AudioStreamReset` and `AudioStreamPlayTrack` at a helper-API level. The official firmware history records ADPCM homebrew support, stream error-recovery work, audio-mix changes, pause-on-break behavior, and later save/load support for ADPCM audio. These are documented product statements, not physical-hardware measurements. [CITED: [RetroHQ developer post](https://forums.atariage.com/topic/353011-homebrew-code-examples/), [RetroHQ firmware history](https://www.retrohq.co.uk/FW/DW6CQ1Q4J6VCK)]

The public material still does not specify the actual command values or bus transaction semantics, `.bup` framing/codec variant, per-sample cadence, output gain/mix equation, exact failure handling, game-side pause/resume behavior, or state fields needed to continue playback. The linked sample/encoder source is excluded as unlicensed per the selected phase scope. A legal synthetic fixture and an independent scalar PCM oracle are feasible in principle once a normative profile exists; they cannot yet exercise this incomplete public profile without guessing.

**Primary recommendation:** mark the current implementation gate **NO-GO**. Do not infer or select an audio contract from the sample/encoder source, an emulator implementation, a common ADPCM convention, or hardware that was not tested. Reopen when first-party material closes the missing fields and a clean-room generated fixture produces independently expected PCM. The strongest alternative is to keep the feature horizon open until that evidence or a lawful, reproduced title-blocking problem appears.

## Project Constraints (from AGENTS.md)

- Reuse only permissively licensed code; do not open or retain GPL, LGPL, or unlicensed emulator source. This phase also excludes the unlicensed sample/encoder source. [VERIFIED: `AGENTS.md:56-58`; exact directive: “GPL, LGPL and unlicensed emulator source is never opened and no copy of it is kept in the workspace”.]
- Keep Muhtari to its C library, headless runner, and libretro core; add no window, audio-device, or input-device code. [VERIFIED: `AGENTS.md:46-47,59`; exact values: “a C library, a headless command-line runner (`muhtari-run`), and a libretro core” and “No window, audio-device or input-device code.”]
- Keep the core deterministic, integer-only, instance-owned, and bounded. Any future ADPCM work must preserve the same-input/same-hash contract and return within a bounded emulated interval. [VERIFIED: `AGENTS.md:63-65`; exact directive: “Integer-only. All state lives in the instance.” and “Every call returns within a bounded amount of emulated time”.]
- Every future behavior change needs a test in `cmake --workflow --preset ci`; keep behavior documentation in `docs/` and the public header terse. [VERIFIED: `AGENTS.md:66-68`; exact test command: `cmake --workflow --preset ci`.]
- Prefer offline and automated evidence. This phase has no authorization for a physical-console, cartridge, RetroArch-host, or owner-device run. [VERIFIED: `AGENTS.md:69-80`; exact limits include “It runs RetroArch only for an authorized check with a concrete evidence value” and “do not take over the owner's Mac by default”.]
- Keep checks proportional and name concrete faults for any future new check; follow the existing resource-bound and review rules. [VERIFIED: `AGENTS.md:81-88`.]
- Use the dependency ladder and avoid a package for this documentation-only qualification. [VERIFIED: `AGENTS.md:163-168`.]

## Architectural Responsibility Map

| Capability | Primary Tier | Secondary Tier | Rationale |
|------------|-------------|----------------|-----------|
| Published `$420` command surface and sidecar discovery | External cartridge firmware / storage | Game software | RetroHQ’s public description assigns the register and file convention to GameDrive; it does not define a Muhtari integration route. |
| Future sidecar bytes and identity in an emulator | Unresolved between core API and consumer adapter | Runner, libretro, portable host | The existing library accepts cartridge bytes through `muhtari_media`; a later scope decision must decide who supplies adjacent files and how they are identified. No tier/API choice is made here. |
| Deterministic mono output and playback continuation | Core/API behavior | State serializer and consumer save/load path | Muhtari owns its current PCM and canonical state contracts. Matching a stereo stream and save/load behavior would need an explicit future compatibility decision. |
| Legal generated fixture and expected PCM | Test/reference layer | — | Fixture bytes can be authored independently; the normative stream description required to make them representative is not yet available in the inspected material. |

## Standard Stack

This is a source-qualification phase, not an implementation phase. No external package, codec library, firmware binary, emulator source, or new runtime dependency is recommended or installed. Do not add a package legitimacy audit or installation task to the plan.

## Architecture Patterns

### Documented data path (not an implementation design)

```text
A78 declaration `adpcm@420`
        |
        v
Game calls the published GameDrive helper surface
(`AudioStreamReset`, `AudioStreamPlayTrack`; transaction values unknown)
        |
        v
Sibling folder named for the A78, containing numbered `.bup` tracks
        |
        v
RetroHQ describes stereo ADPCM decode/playback
        |
        v
Output cadence, gain/mixing, errors, and state tuple: not specified here
```

The diagram restates only the published developer instructions. It does not assert the physical device’s internal implementation or behavior.

### Recommended qualification record

Keep each contract field in the evidence ledger with the source’s date/revision, evidence class, confidence, and one of: documented fact, partial, inference, or unknown. Treat firmware notes as evidence that a feature or correction was announced; they do not define the media format or prove hardware behavior. Use only the first-party materials and in-repo contracts cited below.

### Anti-patterns to avoid

- Do not derive register values, timing, codec layout, or error semantics from an unlicensed sample/encoder source.
- Do not fill missing fields from MAME, MiSTer, or another emulator’s code; emulator source is out of scope and is not an authoritative GameDrive specification.
- Do not equate GameDrive’s firmware pause-on-break or save/load notes with a published game-side pause/resume ABI or an exact Muhtari state tuple.
- Do not infer the ADPCM rate, predictor format, or channel order from the word “ADPCM”.

## Evidence Ledger

Confidence refers to confidence in what the cited material says. It does not certify physical hardware behavior. `[CITED]` findings use current first-party documentation retrieved with the web-search tool; the GSD confidence seam classified cross-checked web-search evidence as MEDIUM. In-repo discrete values are quoted verbatim beside their source-of-truth line citations.

| Contract field | Status | Source revision / evidence class | Evidence and limit |
|---|---|---|---|
| `$420` device address and ROM declaration | **Documented fact, partial** | RetroHQ-authored developer post, posted/edited 2023-07-14; supporting author comment 2023-08-10. First-party developer documentation; MEDIUM. | The post says the streamed device maps at `$420` and is enabled with `adpcm@420`. In the address-space discussion, SainT answers that it is “Just `$420`”; the author then says other `$42x` addresses may hold future GameDrive features. This supports the named address/declaration only. It does not define reads, writes, bus width, aliases, access timing, values, or acknowledgement. [CITED: [developer post](https://forums.atariage.com/topic/353011-homebrew-code-examples/), [address discussion](https://forums.atariage.com/topic/353011-homebrew-code-examples/#findComment-5296865), [future `$42x` discussion](https://forums.atariage.com/topic/353011-homebrew-code-examples/#findComment-5296935)] |
| Command/helper behavior | **Documented fact, partial** | Same 2023-07-14 developer post; MEDIUM. | The prose names `AudioStreamReset` as initialization and `AudioStreamPlayTrack` for selecting the numbered track; it also says to call the appropriate `AudioStreamXXX` functions. Exact bus command bytes, call parameters, stop/volume/pause/resume helper names, status/error reads, and return semantics are unknown in this prose. The linked sample is excluded and was not opened. [CITED: [developer post](https://forums.atariage.com/topic/353011-homebrew-code-examples/)] |
| Sidecar path, track identity, and discovery | **Documented fact, partial** | Same 2023-07-14 developer post; MEDIUM. | The post describes a same-basename directory beside the A78 and numbered `0–31.bup` files; `AudioStreamPlayTrack` receives the number to play. It does not specify filename case rules, missing-track behavior, path traversal rules, duplicate/alias handling, whether all tracks must exist, or how sidecar bytes bind to a loaded ROM/state. [CITED: [developer post](https://forums.atariage.com/topic/353011-homebrew-code-examples/)] |
| Stream encoding and framing | **Partial; normative framing unknown** | Same 2023-07-14 developer post; MEDIUM. | The post calls the files “stereo ADPCM” streams and points developers to a WAV-to-stream encoder. It does not identify an ADPCM variant, file header, block layout, predictor/index representation, nibble/byte order, channel interleave, sample count, loop marker, or end-of-track rule. “Stereo ADPCM” is not enough to produce a conforming independent stream. [CITED: [developer post](https://forums.atariage.com/topic/353011-homebrew-code-examples/)] |
| Playback clock and cadence | **Unknown in checked first-party sources** | Firmware page accessed 2026-11-15; developer post 2023-07-14; MEDIUM for the checked pages. | Neither the checked firmware entries nor the post states a sample rate, playback clock, cadence source, track duration relation, or per-sample timing rule. Firmware’s “potential fix” for playback glitching does not specify timing. This is scoped to these pages; it is not a claim that no private or unpublished specification exists. [CITED: [firmware history](https://www.retrohq.co.uk/FW/DW6CQ1Q4J6VCK), [developer post](https://forums.atariage.com/topic/353011-homebrew-code-examples/)] |
| Output and mixing | **Partial; ADPCM level/mix unknown** | RetroHQ Instruction Leaflet PDF, 10 pages, accessed 2026-11-15, with no printed revision/date identified in inspected text; firmware history accessed 2026-11-15. MEDIUM. | The developer post identifies stereo stream data. The leaflet says per-source volume can be adjusted and distinguishes TIA volume from other AV OUT/console volume controls. Firmware 23.08.2562 says audio mixing was overhauled. None of these sources supplies GameDrive ADPCM gain, mix weights, sum order, clipping/rounding, console-versus-AV routing for this stream, or an exact output PCM contract. [CITED: [developer post](https://forums.atariage.com/topic/353011-homebrew-code-examples/), [Instruction Leaflet PDF](https://www.retrohq.co.uk/product/7800gd/7800GD.pdf), [firmware history](https://www.retrohq.co.uk/FW/DW6CQ1Q4J6VCK)] |
| Errors and recovery | **Documented firmware changes; error contract unknown** | Firmware 23.06.0865, 23.07.0363, and 26.01.2266 in current RetroHQ history; page accessed 2026-11-15. MEDIUM. | The history says 23.06 improved ADPCM stability when removing/reinserting a memory card; 23.07 added a scrub table and improved error recovery; 26.01 lists a *potential* playback-glitch fix. These are maintenance notes, not error codes or normative behavior for corrupt/truncated data, absent files, read failures, underruns, or recovery state. [CITED: [firmware history](https://www.retrohq.co.uk/FW/DW6CQ1Q4J6VCK)] |
| Reset, pause, and resume | **Reset documented; other game-side commands unknown** | Developer post 2023-07-14; firmware 23.08.2562; MEDIUM. | `AudioStreamReset` is named as the initialization call. The firmware history says streamed BupChip audio pauses correctly “on break”; that is a device menu/break behavior, not evidence of a game-visible pause/resume command or resume cursor semantics. No exact reset state, stop behavior, fade, or game-side pause/resume call contract is stated in the checked prose. [CITED: [developer post](https://forums.atariage.com/topic/353011-homebrew-code-examples/), [firmware history](https://www.retrohq.co.uk/FW/DW6CQ1Q4J6VCK)] |
| Save/load continuation | **Firmware feature announced; state schema unknown** | Firmware 26.01.2266; official RetroHQ news entry dated 2026-01-23; firmware page accessed 2026-11-15. MEDIUM. | The release history says it added save and load state of ADPCM audio. This establishes that the firmware feature is announced; the sources do not state which playback/decoder/mixer fields are captured, whether resume is sample-exact, how sidecars are identified, or how state versions/rejection/migration work. This is not a Muhtari state-format decision. [CITED: [firmware history](https://www.retrohq.co.uk/FW/DW6CQ1Q4J6VCK), [RetroHQ news](https://www.retrohq.co.uk/)] |
| Documented source identity and freshness | **Partial; no immutable document revision** | Dynamic RetroHQ firmware page and forum thread as accessed 2026-11-15. | The firmware page currently lists 26.01.2266 and the official RetroHQ news entry dates its announcement 2026-01-23. The developer post is dated/edited 2023-07-14; the `$420` answer is dated 2023-08-10. The product’s linked 10-page leaflet has no printed revision/date in the inspected text. These are page/version/date pins, not firmware binaries or hardware test receipts. [CITED: [firmware history](https://www.retrohq.co.uk/FW/DW6CQ1Q4J6VCK), [RetroHQ news](https://www.retrohq.co.uk/), [developer post](https://forums.atariage.com/topic/353011-homebrew-code-examples/), [leaflet](https://www.retrohq.co.uk/product/7800gd/7800GD.pdf)] |

### Firmware history relevant to streamed audio

| Firmware entry | First-party note | What it supports | What it does not establish |
|---|---|---|---|
| 23.06.0865 | Added homebrew ADPCM streaming; improved stability when the memory card is removed/reinserted. | Feature existence and a storage-change robustness concern. | Codec format, timing, or an error/status protocol. |
| 23.07.0363 | Added a scrub table to the ADPCM stream and improved error recovery. | Recovery behavior received firmware work. | Scrub-table format, error classification, or recovery guarantees. |
| 23.08.2562 | Overhauled audio mixing; streamed BupChip audio pauses correctly on break. | Firmware mix behavior and break-menu lifecycle have changed over time. | Mix arithmetic or game-side pause/resume semantics. |
| 26.01.2266 | Potential playback-glitch fix; added save/load state of ADPCM audio. | Current public notes announce a possible playback fix and state continuation support. | A guarantee, exact fix, state tuple, media identity, or serialization format. |

The update page has no per-entry release date; the official news entry on 2026-01-23 separately announces 26.01.2266. No firmware payload was downloaded or inspected. [CITED: [RetroHQ firmware history](https://www.retrohq.co.uk/FW/DW6CQ1Q4J6VCK), [RetroHQ news](https://www.retrohq.co.uk/)]

## Generated Fixture and Independent Oracle Feasibility

**Legal fixture authoring: feasible in principle.** Muhtari’s asset policy allows its own generated fixtures when listed in the test manifest. The phase’s selected scope excludes the linked sample/encoder source and copyrighted game data; this does not prevent writing fresh synthetic PCM, a tiny stream fixture, or an independent reference decoder after a normative profile is available. [VERIFIED: `ASSET_POLICY.md:23-28`; exact permission: “Our own builds: diagnostic ROMs assembled from this repository's MIT sources (their cc65 runtime adds Zlib), fuzz seeds and generated fixtures.”]

**Independent expected PCM: feasible in principle, but not for the current incomplete profile.** Inference: a small, separately authored scalar decoder/reference can calculate expected PCM for generated input once the file layout, predictor rules, channel order, rate, and rounding are specified. The current first-party prose does not supply those details, so creating a `.bup` that claims to exercise the documented profile would require guessing. Keep the fixture/oracle gate open; this is a source-completeness gap, not an asserted license prohibition. [CITED: [developer post](https://forums.atariage.com/topic/353011-homebrew-code-examples/)]

The saved milestone review records the sample and encoder pins as having no declared license grant: `7800GD-Homebrew` at `0ef62a554354c39d2a6647b7513b23c6729c4d6e` and `7800GD-AudioEncode` at `e55371a49445e41f82f9dc21ad01ad7cf98c3f13`. That evidence is repository metadata only; no source files were opened, downloaded, cloned, or used. [CITED: [Phase 24 milestone review](../../research/next-milestone/2026-11-15-game-drive-qualification.md#current-source-evidence)]

## Muhtari Compatibility Consequences (evaluation only)

### Current media and output contracts

The current public media structure is:

```c
typedef struct muhtari_media {
    uint32_t size;
    uint32_t region;   /* 0: the cartridge's; MUHTARI_REGION_* */
    const void *image; /* copied at load */
    size_t image_size;
    const void *bios; /* NULL: fast boot; copied at load */
    size_t bios_size;
    uint32_t hsc_mode;   /* 0 automatic, 1 on, 2 off */
    const void *hsc_rom; /* NULL: signature-only stub; copied at load */
    size_t hsc_rom_size;
    uint32_t port[2];   /* 0 automatic; explicit device override */
    uint32_t expansion; /* MUHTARI_EXPANSION_* */
} muhtari_media;
```

This is the full current declaration and contains no separate sidecar member. [VERIFIED: `include/muhtari.h:189-201`; verbatim source quote above.] The existing ROM bound is `#define MUHTARI_IMAGE_SIZE_MAX 1048704u /* 1 MiB of ROM plus a 128-byte header */`. That bound is for the ROM image and does not provide a sidecar size or duration limit. [VERIFIED: `include/muhtari.h:13`; exact value and comment quoted.]

Muhtari’s current audio output is mono signed int16 at exactly 48,000/1 Hz. [VERIFIED: `docs/hardware/audio.md:3-4`; exact quote: “The output is mono signed int16 at exactly 48,000/1 Hz.”] A GameDrive stream is described as stereo, so a future integration would have to specify conversion into this output or deliberately change a public contract; this is an inference, not a selected mix rule. Any gain, downmix, rounding, clipping, or source-priority rule remains undecided.

The current version surface is API 8, ABI 1, latest behavior 8, state format 3, with the installed machine selecting its envelope; legacy and YM paths remain 6/1 and 7/2. [VERIFIED: `docs/embedding.md:617-621`; exact quote: “The current interface is API 8, ABI 1, with latest behaviour 8 and state format 3; an installed machine selects its envelope.”] These version values are a baseline, not a recommendation to change them. Existing decisions require separate API/ABI, behavior, state, and battery versions, and a failed public operation must not mutate state. [VERIFIED: `.planning/preparation/DECISIONS.md:53`; exact decision: “append-only statuses, caller-owned buffers, an error changes no state, separate ABI/behaviour/state/battery versions”.]

### Media identity and resource bounds

The documented sidecar folder is named from the A78 basename, but the documents do not define whether basename is a sufficient identity for two files with the same name, whether content hashes participate, or how a save state binds to the selected track bytes. Inference: the future contract needs stable caller-owned media identity and finite sidecar byte/duration limits before allocation, decoder work, or state size can be bounded. No numeric limit is selected here.

Current consumers need the same media profile to be representable without relying on a native filesystem:

| Consumer | Current contract evidence | Unresolved sidecar consequence |
|---|---|---|
| C library | `muhtari_load` receives a `muhtari_media`; the structure above identifies the current image and optional firmware/storage inputs. | Later decision must choose whether sidecar bytes are caller-provided and how track bytes/identity are associated with image data. |
| `muhtari-run` | Headless runner accepts a cartridge image file and options; state operations use canonical bytes. | Later decision must specify a deterministic path/identity rule or supplied bytes and behavior for missing/unreadable/corrupt files. |
| libretro | Documentation says it loads A78/bin images from memory; save-state callbacks use the library’s canonical bytes. | Frontend/VFS/path availability differs. A later design must define an optional portable media route without assuming host filesystem access. |
| Portable/WebAssembly consumers | The same core/library contracts are shipped for portable consumers. | Sidecar input must be available through the embedding consumer’s chosen byte-delivery route; no filesystem, browser, or host design is selected. |

Evidence: `docs/libretro.md` says the core loads `.a78` and `.bin` “from memory” (`docs/libretro.md:3-5`); `docs/runner.md` describes a single cartridge image input (`docs/runner.md:7-12,19-33`). The consumer consequence above is an integration inference, not a claim that a particular frontend cannot expose sidecars. [VERIFIED: `docs/libretro.md:3-5`; exact quote: “It loads `.a78` images and headerless `.bin` images from memory”.]

### Lifecycle and save/load

The only library-level continuation requirement justified by the firmware note is that a later emulator profile must investigate playback state. Inference: deterministic continuation likely depends on the active track identity, decoder predictor/index, stream cursor, playback phase, and any output/mix history. Those are candidate questions only; the current sources do not prove which fields GameDrive saves, and this phase does not choose a serialized tuple.

Current library state is canonical, fixed-size for the loaded topology, versioned, and matched to effective machine identity. Libretro uses the same bytes and refuses unsupported revisions without migration. [VERIFIED: `src/state.c:1495-1535,1538-1575` and `docs/libretro.md:207-218`; exact status/value examples are left to source-linked implementation details.] A future external audio profile could affect state compatibility if continuation state or sidecar identity becomes part of machine state. Whether that requires a state-format change or how to migrate/reject older states remains for a later compatibility decision.

### Finite work and refusal behavior

No `.bup` maximum size, maximum duration, supported-track completeness rule, or decoder work rate is stated in the checked RetroHQ documents. Without those inputs, Phase 24 cannot derive a defensible memory budget or worst-case per-frame work bound. Existing Muhtari policy requires bounded calls and validation-before-apply; a future profile would need deterministic refusal behavior for at least unavailable, malformed, oversized, and incompatible media, but no status codes or fallback behavior are selected here. This is a planning implication from project contracts, not an external GameDrive error specification.

## Don't Hand-Roll

| Problem | Don't Build | Use Instead | Why |
|---|---|---|---|
| `.bup` decoding rules | An ADPCM variant guessed from common codecs or a port of the linked encoder/sample | A first-party normative stream specification, then a fresh independent scalar reference | The checked prose does not define file framing, predictor, channel order, or clock. A guessed decoder would not be source-qualified. |
| Expected PCM | Golden output extracted from a copyrighted game or copied from the production decoder | Generated legal inputs and independently computed reference PCM after the profile is fixed | Keeps provenance clean and prevents implementation/reference agreement from hiding a shared error. |
| State identity | A guessed basename-only identity or silent continuation after media changes | Reopen as a compatibility decision once sidecar identity and state semantics are documented | Basename collision, content change, or missing media could produce an invalid restore; current documentation does not settle behavior. |

## Common Pitfalls

- Treating the firmware’s “potential fix” as proof playback is glitch-free.
- Treating “save and load state of ADPCM audio” as an exact, documented save-state schema.
- Conflating BupChip’s menu-break pause note with homebrew API pause/resume calls.
- Treating the “stereo ADPCM” label as a complete codec and timing specification.
- Treating ROM size limits as sidecar limits. The quoted `MUHTARI_IMAGE_SIZE_MAX` only describes ROM plus header.
- Treating the old preparation-table entry “device unidentified” as proof that RetroHQ’s separate `$420` GameDrive device does not exist. The relationship between those entries is not documented in the sources checked.

## Code Examples

No implementation example is included. The first-party post links to sample and encoder source that the phase explicitly excludes, and the prose alone does not give enough register or file details for a faithful example.

## Contradictions and Source Boundaries

1. `.planning/preparation/A7800-AUDIO.md:153` lists an A78 v4 ADPCM device at `$0420` and labels the device unidentified/unverified. RetroHQ’s developer post separately describes a GameDrive streamed device at `$420`, declared with `adpcm@420`. The checked sources do not establish whether these labels describe the same standardized device or distinct mechanisms. Treat this as an unresolved mapping, not a hardware contradiction. [VERIFIED: `.planning/preparation/A7800-AUDIO.md:153`; exact row: “ADPCM stream @$0420 | A78 v4 audio bit 5 [AUD.14]; device unidentified (unverified) | — | when documented”.] [CITED: [RetroHQ developer post](https://forums.atariage.com/topic/353011-homebrew-code-examples/)]
2. The 23.08.2562 firmware note says streamed BupChip audio pauses on break; the developer prose names reset and track selection but does not document game-side pause/resume calls. These refer to different control layers, so the current evidence is incomplete rather than directly contradictory. [CITED: [firmware history](https://www.retrohq.co.uk/FW/DW6CQ1Q4J6VCK), [developer post](https://forums.atariage.com/topic/353011-homebrew-code-examples/)]
3. The 26.01.2266 note announces a “potential” playback glitch fix and also state support. The qualifier means the release note itself is not a correctness guarantee; no hardware or emulator test was run in this research. [CITED: [firmware history](https://www.retrohq.co.uk/FW/DW6CQ1Q4J6VCK)]

No physical hardware, owner device, ROM, firmware payload, emulator source, sample source, or encoder source was used. The source evidence is documentation-only and cannot establish physical timing or analog mixing.

## State of the Art

| Earlier public firmware note | Later public note | Planning impact |
|---|---|---|
| ADPCM streaming introduced for homebrew in 23.06.0865. | 23.07.0363 added scrub/error recovery; 23.08.2562 changed mixing and break behavior. | Error and mix details changed over time; pin a firmware profile rather than assuming timeless behavior. |
| — | 26.01.2266 notes a possible glitch fix and ADPCM state save/load. | Playback recovery and continuation belong in future qualification, but the release note is not a format or state specification. |

**Deprecated/outdated:** None established. The linked developer post is older than current firmware history, but the sources checked do not say it has been superseded or publish a newer normative audio specification.

## Assumptions Log

| # | Claim | Section | Risk if Wrong |
|---|-------|---------|---------------|

No training-only claim is used. Inferences are marked explicitly; fields without positive evidence remain unknown.

## Open Questions

1. What exact reads/writes and command values make up the `$420` profile, and which addresses are reserved for later GameDrive functions?
2. What are the `.bup` header/block layout, ADPCM variant, predictor/index rules, channel interleave/order, sample rate, track end/loop convention, and maximum file size/duration?
3. What clock/cadence produces samples, and how does it relate to emulated ticks and partial-frame output?
4. What are the stereo output levels, gain, mix/routing order, rounding and clipping rules?
5. What deterministic response applies to missing, truncated, malformed, unreadable, oversized, or out-of-range tracks?
6. Which game-callable commands exist for stop, volume, pause and resume? What state does reset establish?
7. Which decoder/playback/output fields and sidecar identity does firmware save/load preserve, and what happens when matching media is unavailable or changed?

**Recommendation:** no-go for implementation until the first-party profile or a permission-qualified, independently inspectable specification answers the fields needed for a bounded decoder and oracle. The strongest alternative is to hold the horizon open; do not treat missing public detail as an invitation to infer a contract.

**Revisit triggers:**

- RetroHQ publishes a versioned or dated profile covering register transactions, track/media identity, stream framing, clock, output/mix, error handling, lifecycle, and state continuation.
- A legally authored generated stream can be checked by an independent expected-PCM reference without the sample/encoder source or copyrighted game data.
- Sidecar size/duration bounds and existing-consumer media delivery can be evaluated with that profile.
- A distinct lawful and reproducible title-blocking case appears; it is evaluated as separate evidence, not as proof of this undocumented profile.

## Environment Availability

Skipped: this is documentation research only; no external runtime, package, host, or device is required. The project config also has Nyquist validation explicitly disabled (`"nyquist_validation": false,` at `.planning/config.json:36`), so no Validation Architecture section is applicable. Security-domain checks are not required because the project config explicitly sets `"security_enforcement": false,` at `.planning/config.json:60`.

## Sources

### Primary first-party documentation (MEDIUM confidence through the cross-checked web-search seam)

- [RetroHQ 7800 GameDrive firmware history](https://www.retrohq.co.uk/FW/DW6CQ1Q4J6VCK) — dynamic page accessed 2026-11-15; highest listed entry 26.01.2266; relevant changelog entries 23.06.0865, 23.07.0363, 23.08.2562, and 26.01.2266.
- [RetroHQ news](https://www.retrohq.co.uk/) — official news feed entry dated 2026-01-23 announcing 26.01.2266.
- [RetroHQ-authored GameDrive homebrew code examples](https://forums.atariage.com/topic/353011-homebrew-code-examples/) — developer prose posted/edited 2023-07-14; an author reply on 2023-08-10 clarifies the single `$420` address.
- [RetroHQ Instruction Leaflet PDF](https://www.retrohq.co.uk/product/7800gd/7800GD.pdf) — linked by the official product page, 10 pages, accessed 2026-11-15; no printed revision/date identified in inspected text.
- [RetroHQ July 2023 news page](https://www.retrohq.co.uk/news/2023-07/) — official post on 2023-07-14 directs developers to the homebrew examples thread.

### In-repo source of truth (HIGH confidence for current repository contracts)

- `AGENTS.md:46-88,137-150` — project deliverables, licensing/provenance, bounded determinism, verification, quality, and branch constraints.
- `.planning/ROADMAP.md:26-35,50-64` and `.planning/REQUIREMENTS.md:13-25` — selected v8 scope, Phase 24 success criteria, requirements and exclusions.
- `.planning/research/next-milestone/2026-11-15-game-drive-qualification.md:60-73,85-95` — prior source pins, recommendation, and accepted no-go threshold.
- `.planning/preparation/README.md:45-64,77-96`, `.planning/preparation/A7800-AUDIO.md:148-155`, `.planning/preparation/DECISIONS.md:53-62` — prep source policy, existing expansion-audio context, API/audio/state decisions.
- `include/muhtari.h:12-13,51-77,189-201,240-242`, `docs/hardware/audio.md:1-10`, `docs/embedding.md:617-640` — current API, media, size, output and version surfaces.
- `docs/runner.md:7-12,19-33,70-89`, `docs/libretro.md:3-6,207-239`, `src/state.c:1495-1575` — current runner, libretro and state behavior.
- `ASSET_POLICY.md:23-28` — project policy allowing own generated fixtures.

### Metadata-only license evidence

- [`7800GD-Homebrew` pinned repository tree](https://github.com/RetroHQ/7800GD-Homebrew/tree/0ef62a554354c39d2a6647b7513b23c6729c4d6e) and [`7800GD-AudioEncode` pinned repository tree](https://github.com/RetroHQ/7800GD-AudioEncode/tree/e55371a49445e41f82f9dc21ad01ad7cf98c3f13) — metadata/tree inspection recorded in the supplied 2026-11-15 milestone review; no source files accessed in this research.

## Metadata

**Confidence breakdown:**

- Published `$420` and sidecar convention: MEDIUM — first-party developer prose is specific but not a normative media/bus specification.
- Firmware feature history: MEDIUM — first-party release notes establish what RetroHQ announced; they do not prove a playback or hardware guarantee.
- Existing Muhtari contracts: HIGH — current source-of-truth headers/docs were read directly this session and quoted where discrete values are used.
- Fixture/oracle feasibility: MEDIUM — legal self-authored fixtures are allowed; exercising this exact profile remains conditional on missing normative codec/timing details.

**Research date:** 2026-11-15<br>
**Valid until:** Recheck before planning an implementation or when RetroHQ publishes a new GameDrive firmware entry or audio specification.
