---
title: "Successor scope: consumer and upstream boundaries"
summary: "Evidence for what Muhtari consumers already use, what remains host qualification, and which next Muhtari slice has user value."
read_when: "Selecting Muhtari's successor milestone or deciding whether a consumer request needs core/API work."
updated: 2026-11-11
---

# Consumer boundaries and next Muhtari scope

## Recommendation

Prefer one **YM2151 cartridge-audio support slice** for Muhtari, bounded to a declared YM2151 request and generated diagnostic coverage. This is an explicit capability gap, not a correctness regression: Muhtari's loader intentionally refuses the YM2151 sound flag before staging/allocation, and `tests/core/test_core.c` already asserts that refusal. The [hardware guide](../../../docs/hardware/cartridge.md#boards) and [runner guide](../../../docs/runner.md) document this behavior. The audio preparation record notes homebrew/feature use, including a PROPack filename-tag lower bound and reports that `1942` and *Pac-Man Collection 40th* need XM/YM; these are leads, not proof that any image has been tested here. No current title-blocking emulation defect is established.

Keep the YM milestone narrow: one supported YM2151 cartridge path with an owned, generated diagnostic and end-to-end coverage through existing Muhtari consumers. Do not bundle full XM services, BupChip/Souper, Covox or ADPCM. The specification and enable/clock model still have unresolved sources, so implementation must first settle a lawful, bounded model and state clearly whether it is model conformance or hardware-measured. If that boundary cannot be resolved, defer code and choose an actual player-visible compatibility case only when its expectation and sequence are known.

The strongest alternative is to finish the existing PAL browser-player slice in LibFuturo. Its missing work is profile selection, rendering/pacing/save-capability handling and qualification through that project's relocated browser consumer. The v0.12.0 Muhtari pin already exposes the PAL profile. This alternative belongs to LibFuturo and must not become a second browser host or a new ABI in Muhtari.

**Confidence:** high that no upstream Muhtari API/package addition is currently demanded and that the declared YM2151 capability is intentionally unsupported; medium that this is the best next product scope because map, clock and independent acceptance evidence remain uncertain. Revisit if a minimal consumer reproducer reveals an API/package/portability barrier, better primary hardware evidence contradicts the preparation record, or another named feature has stronger user value and an independently supportable oracle.

## What the inspected consumers establish

| Route | Established evidence | Limit that affects scope |
|---|---|---|
| Muhtari public release | Muhtari v0.13.1 is published with the existing C library, headless runner and libretro core. LibFuturo's API 8 PAL evidence is specifically against its older v0.12.0 source pin. | Do not infer PAL qualification against v0.13.1 from the older pin. A release or profile declaration is not broad retail-game compatibility evidence. Keep the three deliverables and current API/ABI policy. |
| LibFuturo native PAL consumer | Core-profiles Phase 1 records a passing macOS arm64 source and relocated-installed public consumer against Muhtari v0.12.0, source `363b2e338f69ad7a71c4c7df93f76eea6e6fed0a`. It checks the full 320x292 output, independent literal video/audio expectations, cadence, bounds, NTSC regression, and refusal of PAL snapshots/media. | This is pinned-source local evidence, not native Linux or hosted qualification. Core-profiles Phase 2 explicitly remains partial on those gates. PAL saves and snapshots are unsupported by contract. |
| LibFuturo browser host v1.9/v2.0 | The archived routes exercise the relocated `lf_*` consumer and pinned Chrome with Muhtari v0.11.1/API 8, NTSC profile, and the named fixtures. v1.9 has a passing historical hosted candidate; v2.0 has passing local pinned-browser save/reload evidence. | Neither qualifies PAL browser playback or current-source v2.0 hosted execution. Storage is best-effort; physical audio, broad browser/game support, snapshots and Playstead embedding are not established. |
| LibFuturo browser PAL work in progress | At LibFuturo HEAD `a295baa0275df3587b76a7d67d4300b610797f29`, commit `3b6cf3b4ce8d49333c36c19d3385064b77748c79` binds the ordered NTSC/PAL profile set into the web package identity; `a295baa` adds profile-bridge contract tests. The selected v2.1 work pins Muhtari v0.12.0. | The primary checkout was dirty at inspection: `tests/web-consumer/consumer.c`, browser-host ROADMAP/STATE and other untracked files were present. The dirty C file adds profile-selection/descriptor work, while committed `consumer-host.js` and `consumer.html` still hard-code NTSC. The v2.1 phase plan and commits are not PAL browser acceptance; no test or browser run was started for this research. |

The separate machine-state workstream likewise shows an established API 8/NTSC snapshot route, with its own exact-source/native-Linux gate. It creates no evidence that the browser needs a new Muhtari snapshot API or that the spent Muhtari Phase 12/14 host scopes can be reopened.

## Product evidence for a core slice

The current [compatibility ledger](../../../docs/compatibility.md) accounts for 204 pinned-list entries: 201 are not evaluated and three are skipped. “Not evaluated” is untested, not a failure. The 49 owned sample/diagnostic rows have 46 passes, three skips and no failures; the 27 public-title-screen rows do not establish gameplay. Most remaining owned evidence is boot-only or model conformance. These counts identify coverage uncertainty, not a bug or a reason to raise a compatibility score.

Therefore do not turn the untested pinned-list rows into a generic “test more games” project or claim a known gameplay defect. The YM2151 refusal is a concrete unsupported feature with documented possible users, so it is a better candidate than speculative accuracy work; however its incomplete physical model is a real pre-implementation risk. Existing open-ROM inventory can suggest a useful scenario, but a filename tag is not usage evidence and protected ROM/BIOS bytes stay outside the repository except fixtures explicitly allowed by the manifest.

## Smallest useful proof

For the YM2151 slice, require before implementation:

1. A source-audited register/enable/clock/mix model for one YM2151 use. The current notes distinguish 7800.8bitdev's `$0460/$0461` map and busy flag, MAME's `$0470 == $84` enable, and MiSTer's bit-7/header alternative; the real XM source is unverified. This ambiguity must be resolved or explicitly bounded before claiming compatibility.
2. An independent generated diagnostic with literal register/audio expectations and refusal, boundary and non-YM regressions. This proves the selected model and consumer path; it is not hardware calibration. Do not use agreement between two emulators as hardware truth, and do not open prohibited emulator source.
3. The same fixture through the library and `muhtari-run`, plus loaded libretro only if changed behavior reaches that adapter. Preserve existing return/refusal semantics for Covox, ADPCM and other unsupported declarations.
4. A permissive, minimal implementation path with retained notices. Preparation estimates a specialized C OPM port at roughly 1,500–2,000 lines (untested); upstream `ymfm` is BSD-3 but C++ would add a runtime/build burden across ports. Reassess this against a small bounded copy before selecting implementation.
5. A generated or manifest-approved fixture with no ROM, BIOS, save bytes, private path or content bytes in reports. Describe support as model-tested unless an independent physical reference is added.

If the selected issue is actually outside the core, route it to its owner: browser UI, scheduling, IndexedDB and qualification to LibFuturo; Playstead UX and delivery to Playstead. Do not create a Muhtari host, device code, new shared ABI, or package slice without a minimal failing consumer that proves the need.

## Alternative acceptance boundary

The LibFuturo PAL alternative is concrete but downstream: one relocated installed package with an explicit NTSC/PAL selection; PAL full-raster output, rational cadence and bounded PCM; visible refusal of PAL battery saves with zero media/IndexedDB calls; and the existing NTSC save/reload journey retained. It must be bound to the exact Muhtari pin, LibFuturo source, package, fixture, browser and served artifacts. The selected v2.1 plan pins v0.12.0; moving to Muhtari v0.13.1 would be a separate pin change requiring fresh qualification. Its hosted route remains conditional on an authorized exact-source/ref/workflow mapping.

## Provenance, UX and revisit triggers

- Keep Muhtari focused on Atari 7800 and its three existing deliverables. Preserve 2600 ownership in `twentysixhundo`; shared tooling/lessons may transfer, emulation scope does not.
- Do not reopen the Phase 12 denied diagnostic-app request or Phase 14 spent RetroArch acceptance. They are unrelated to a new offline core slice.
- Keep selection and claims honest: a boot or title-screen match is not gameplay; no browser cadence claim is physical display refresh; no Web Audio graph pass proves physical audibility. Browser users need an explicit region choice and plain save-availability guidance.
- If a future request is only “LibFuturo cannot consume a Muhtari package/profile,” ask for the smallest installed-consumer reproducer and exact source/pin. Current evidence shows an adapter and package route already exist; absent a reproducer, treat the gap as consumer UX/qualification, not a missing Muhtari contract.

## Sources and snapshot identity

- Muhtari source pin for LibFuturo PAL: [`363b2e338f69ad7a71c4c7df93f76eea6e6fed0a`](https://github.com/szTheory/muhtari/tree/363b2e338f69ad7a71c4c7df93f76eea6e6fed0a); LibFuturo records: [core-profiles Phase 1 verification](../../../../libfuturo/.planning/workstreams/core-profiles/phases/01-pal-public-consumer-proof/01-VERIFICATION.md) and [Phase 2 verification](../../../../libfuturo/.planning/workstreams/core-profiles/phases/02-canonical-native-qualification/02-VERIFICATION.md).
- LibFuturo browser history and current records, inspected at local HEAD `a295baa0275df3587b76a7d67d4300b610797f29`: [v1.9 audit](../../../../libfuturo/.planning/workstreams/browser-host/milestones/v1.9-MILESTONE-AUDIT.md), [v2.0 audit](../../../../libfuturo/.planning/workstreams/browser-host/milestones/v2.0-MILESTONE-AUDIT.md), [v2.1 Phase 6 context](../../../../libfuturo/.planning/workstreams/browser-host/phases/06-multi-profile-browser-consumer/06-CONTEXT.md), plus workstream ROADMAP/STATE. The worktree was dirty; local commits `3b6cf3b4ce8d49333c36c19d3385064b77748c79` and `a295baa` identify committed changes, not hosted refs.
- LibFuturo machine-state qualification records, inspected in the same checkout: [local qualification](../../../../libfuturo/.planning/workstreams/machine-state/phases/16-muhtari-api-8-linux-qualification/16-LOCAL-QUALIFICATION.md) and [hosted gate](../../../../libfuturo/.planning/workstreams/machine-state/phases/16-muhtari-api-8-linux-qualification/16-HOSTED-CI.md).
- Muhtari compatibility status is generated from `tests/compat/ledger.csv`; see [`docs/compatibility.md`](../../../docs/compatibility.md) and the ledger predicates/limits described there. YM2151 sources and unsettled questions are in `.planning/preparation/A7800-AUDIO.md` §6 and its source table (`AUD.03`, `AUD.09`, `AUD.17`, `AUD.28`); see also `.planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md` §2.3.
- The local refusal boundary is [`src/instance.c`](../../../src/instance.c) and [`tests/core/test_core.c`](../../../tests/core/test_core.c); the compatibility counts link to generated `docs/compatibility.md` rather than direct ROM results.

No browser, build, test, CI, emulator, package, sibling mutation or asset acquisition was performed for this research.
