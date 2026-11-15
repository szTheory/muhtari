# GameDrive streamed audio qualification

Assessed 2026-11-15. This is a source qualification, not implementation, device, or hardware acceptance.

**Local source reviewed:** commit `5350b9777294909fc83f2add73a1f25350373d9f` on `milestone/v8-gamedrive-qualification`; the working tree held phase-start bookkeeping only. Product source and consumer documentation were unchanged during this assessment. Source-document revision dates and access dates are listed separately below.

## Contract evidence

The ledger classifies the checked public profile field by field. `partial` means the cited source establishes the fact named in the finding but leaves the listed contract decisions open. `unknown` is scoped to the cited documents checked on the stated date. Confidence describes the evidence for what the source says; it does not measure physical behavior.

| Field | Requirement | Status | Source revision | Evidence class | Confidence | Citation | Finding / limit |
|---|---|---|---|---|---|---|---|
| registers | GDA-01 | partial | Developer post posted/edited 2023-07-14, author clarification 2023-08-10 | First-party developer prose, cross-checked web search | medium | [Post](https://forums.atariage.com/topic/353011-homebrew-code-examples/), [address reply](https://forums.atariage.com/topic/353011-homebrew-code-examples/#findComment-5296865) | The post describes the GameDrive stream device at `$420` and the ROM declaration `adpcm@420`, and the author reply clarifies the only named address is `$420`. These establish the named address/declaration. The checked prose gives no read/write transactions, data/address width, aliases, values, access timing, acknowledgement, or status semantics. |
| commands | GDA-01 | partial | Developer post posted/edited 2023-07-14 | First-party developer prose, cross-checked web search | medium | [Post](https://forums.atariage.com/topic/353011-homebrew-code-examples/) | `AudioStreamReset` is named as initialization and `AudioStreamPlayTrack` selects a numbered track. The prose does not define helper signatures, parameter width/values, bus commands, stop/volume commands, return values, status reads, whether selecting a new track stops or restarts an active one, or game-callable pause/resume. Linked sample source was excluded. |
| sidecar-discovery | GDA-01 | partial | Developer post posted/edited 2023-07-14 | First-party developer prose, cross-checked web search | medium | [Post](https://forums.atariage.com/topic/353011-homebrew-code-examples/) | The post describes a sibling directory named after the A78 basename, with `0.bup` through `31.bup`, and selecting a numbered track. It does not state case rules, path lookup or traversal rules, missing-track behavior, whether all 32 tracks are required, or how directory discovery behaves for each consumer. |
| media-identity | GDA-01 | partial | Developer post posted/edited 2023-07-14 | First-party developer prose, cross-checked web search | medium | [Post](https://forums.atariage.com/topic/353011-homebrew-code-examples/) | The basename convention identifies a directory name, not stable media bytes. The checked post does not define collision handling for same-basename images, track/content digests, changed or unavailable sidecars, track identity, or how state binds the ROM to selected sidecar bytes. |
| framing | GDA-01 | partial | Developer post posted/edited 2023-07-14, firmware 23.07.0363, history accessed 2026-11-15 | First-party developer prose and firmware changelog, cross-checked web search | medium | [Post](https://forums.atariage.com/topic/353011-homebrew-code-examples/), [firmware history](https://www.retrohq.co.uk/FW/DW6CQ1Q4J6VCK) | The post calls the sidecars stereo ADPCM; 23.07.0363 announces a scrub table. Neither specifies header/block or scrub-table layout, ADPCM variant, predictor/index, nibble/byte order, channel order/interleave, rate, sample count, looping, or end-of-track rules. |
| clock | GDA-01 | unknown | Developer post 2023-07-14, firmware history accessed 2026-11-15 | First-party documents checked through cross-checked web search | medium | [Post](https://forums.atariage.com/topic/353011-homebrew-code-examples/), [firmware history](https://www.retrohq.co.uk/FW/DW6CQ1Q4J6VCK) | These checked sources do not give sample cadence, playback clock, ticks per sample, track duration relation, or partial-frame output behavior. This is a source-scoped unknown, not a claim that no private specification exists. The 26.01.2266 potential glitch fix does not specify timing. |
| output-mix | GDA-01 | partial | Developer post 2023-07-14, leaflet revision/date unidentified and accessed 2026-11-15, firmware 23.08.2562, history accessed 2026-11-15 | First-party documents checked through cross-checked web search | medium | [Post](https://forums.atariage.com/topic/353011-homebrew-code-examples/), [leaflet PDF](https://www.retrohq.co.uk/product/7800gd/7800GD.pdf), [firmware history](https://www.retrohq.co.uk/FW/DW6CQ1Q4J6VCK) | Stereo stream data and a firmware mixing overhaul are documented; the leaflet discusses adjustable per-source volume. These sources do not give ADPCM downmix, gain, routing, source order, sum order, rounding, clipping, or output PCM. The leaflet has no printed revision/date identified in the inspected text. |
| errors | GDA-01 | partial | Firmware 23.06.0865, 23.07.0363, 26.01.2266, history accessed 2026-11-15 | First-party firmware changelog, cross-checked web search | medium | [Firmware history](https://www.retrohq.co.uk/FW/DW6CQ1Q4J6VCK) | The notes announce card-removal/reinsertion stability work, a scrub table and improved error recovery, then a potential glitch fix. They define no result for missing, unreadable, truncated, corrupt, oversized, or out-of-range tracks, storage failure, underrun, recovery cursor, error reporting, or retry. |
| reset | GDA-01 | partial | Developer post posted/edited 2023-07-14 | First-party developer prose, cross-checked web search | medium | [Post](https://forums.atariage.com/topic/353011-homebrew-code-examples/) | `AudioStreamReset` is named as an initialization call. The checked prose does not define reset values, its relation to power-up, whether it stops a track, clears errors, or what state is observable afterward. |
| pause-resume | GDA-01 | unknown | Developer post 2023-07-14, firmware 23.08.2562, history accessed 2026-11-15 | First-party developer prose and firmware changelog, cross-checked web search | medium | [Post](https://forums.atariage.com/topic/353011-homebrew-code-examples/), [firmware history](https://www.retrohq.co.uk/FW/DW6CQ1Q4J6VCK) | The firmware note says streamed BupChip audio pauses correctly on break, which is firmware break-control behavior. The checked developer prose does not document a game-callable pause/resume API, resume position, fade, or game-to-menu interaction. |
| state-continuation | GDA-01 | partial | Firmware 26.01.2266, news announcement 2026-01-23, history accessed 2026-11-15 | First-party firmware changelog and news, cross-checked web search | medium | [Firmware history](https://www.retrohq.co.uk/FW/DW6CQ1Q4J6VCK), [official news](https://www.retrohq.co.uk/) | The note announces ADPCM audio save/load support. The checked pages provide no decoder predictor/index, playback cursor/phase, pending output or mix history, track/media identity, serialization schema, version, refusal, or migration semantics. An announcement is not a normative state tuple or measured continuation result. |

The checked material has no immutable document revision for the dynamic firmware page. Its access date is 2026-11-15. The developer post is dated/edited 2023-07-14; its `$420` reply is dated 2023-08-10. The linked 10-page leaflet is undated and has no printed revision/date identified in inspected text, and was accessed 2026-11-15. These are page/version/date pins, not firmware binaries or hardware measurements. The source ledger and original links are retained in [Phase 24 research](24-RESEARCH.md).

### Firmware notes and their limits

| Firmware entry | Public note | Evidence limit |
|---|---|---|
| 23.06.0865 | Homebrew ADPCM streaming; stability improved when removing/reinserting the memory card. | Establishes feature/recovery work was announced, not its protocol or guarantees. |
| 23.07.0363 | Scrub table added to the ADPCM stream; error recovery improved. | No scrub-table layout, error classes, or recovery guarantee is specified. |
| 23.08.2562 | Audio mixing overhauled; streamed BupChip audio pauses correctly on break. | No mix arithmetic is stated; menu-break pause does not establish game pause/resume commands. |
| 26.01.2266 | Potential playback-glitch fix; save/load state for ADPCM audio. | “Potential” remains a qualifier. The note announces state support, not its fields or exactness. The separate RetroHQ news announcement is dated 2026-01-23, which is not the firmware version or a per-entry release date. |

The saved milestone review records metadata-only pins for [`7800GD-Homebrew`](https://github.com/RetroHQ/7800GD-Homebrew/tree/0ef62a554354c39d2a6647b7513b23c6729c4d6e) and [`7800GD-AudioEncode`](https://github.com/RetroHQ/7800GD-AudioEncode/tree/e55371a49445e41f82f9dc21ad01ad7cf98c3f13), with no license grant. No sample/encoder source or firmware payload was opened, downloaded, copied, cloned, or retained. This does not restrict original generated fixtures allowed by the project asset policy.

## Fixture and oracle feasibility

**Legal authorship: feasible in principle.** The project permits its own generated fixtures when listed in the ROM fixture manifest; see [ASSET_POLICY.md §2](../../../ASSET_POLICY.md). Fresh synthetic PCM and a tiny stream may be authored from a future normative profile. The sample/encoder pins above remain metadata-only, with no license grant; no third-party source or copyrighted game data is needed to author new material.

**Independent expected PCM: feasible in principle after the profile is specified.** A separately written scalar reference can decode newly generated stream values and calculate public PCM independently of the production decoder. Small controlled predictor, channel, timing, and output cases would make shared implementation mistakes less likely. This phase creates no stream bytes, decoder, or oracle.

**This exact profile: unqualified.** The current documents omit transaction values and behavior, framing/codec rules, channel order, rate/cadence, and public output/mix rules. Without those facts a fixture purporting to exercise the documented profile would encode guesses, and a reference could not establish the profile's expected PCM. This is a source-completeness gap, not a legal block on self-authored fixtures.

## Implementation readiness

Implementation readiness: NO-GO

Normative profile: unqualified — the checked first-party post, firmware history and undated leaflet do not close register transactions, media identity, stream framing, clock, mix, error, game lifecycle, or state schema.

Independent profile oracle: unqualified — the required stream, transaction, timing and public output inputs are missing, so no exact-profile fixture or expected PCM result can be justified.

This verdict qualifies the research outcome only. It does not accept GameDrive playback, hardware behavior, an implementation, or any compatibility change.

## Local verification evidence

Commands and exit statuses are local receipts. Run URLs are N/A-local. The checker verifies document completeness and local links only; passing it does not establish external contract sufficiency.

| Evidence | Command | Exit | Run URL | Result / limit |
|---|---|---:|---|---|
| Planning baseline | `test -s .planning/phases/24-qualify-the-gamedrive-audio-contract/24-QUALIFICATION.md` before the file existed | 1 | N/A-local | Expected absence at plan start; no qualification claim existed yet. |
| Report presence | `test -s .planning/phases/24-qualify-the-gamedrive-audio-contract/24-QUALIFICATION.md` after creation | 0 | N/A-local | The tracked qualification record exists and is nonempty. |
| Checker development | `cmake -DREPORT="$PWD/.planning/phases/24-qualify-the-gamedrive-audio-contract/24-QUALIFICATION.md" -P build/phase24/qualification-check.cmake` | 1 on nine initial runs | N/A-local | New-checker row parsing failed closed during development. The CMake parser was corrected before acceptance; these exits are not report passes or product failures. |
| Original report | `cmake -DREPORT="$PWD/.planning/phases/24-qualify-the-gamedrive-audio-contract/24-QUALIFICATION.md" -P build/phase24/qualification-check.cmake` | 0 on both final runs | N/A-local | Final report passed before and after the fault controls. |
| Missing field control | `cmake -DREPORT="$PWD/build/phase24/fault-missing-row.md" -P build/phase24/qualification-check.cmake` | 1 | N/A-local | Removed `framing`; expected diagnostic `missing or duplicate ledger field: framing`. |
| Empty citation control | `cmake -DREPORT="$PWD/build/phase24/fault-empty-citation.md" -P build/phase24/qualification-check.cmake` | 1 | N/A-local | Emptied the `errors` citation cell; expected diagnostic `empty citation: errors`. |
| Unsupported GO control | `cmake -DREPORT="$PWD/build/phase24/fault-go.md" -P build/phase24/qualification-check.cmake` | 1 | N/A-local | Changed readiness to GO while both gates remained unqualified; expected diagnostic `GO claim while qualification gates remain unqualified`. |

Fault copies are disposable under ignored `build/phase24`; they are not report evidence or tracked files. The checker accepts cited unknowns and only checks schema, required fields, readiness labels, and repository-relative Markdown destinations.
