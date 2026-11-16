# muhtari

## What This Is

muhtari is an Atari 7800 ProSystem emulator core written in C. It ships as a library other programs embed, a
headless command-line runner for automation, and a libretro core so that RetroArch and other hosts can play 7800
games with it. It is for people who build emulator hosts and want an accurate, fast, permissively licensed 7800
core; for players, homebrew developers, ROM hackers, speedrunners, TAS authors and AI researchers who use those
hosts. The qualified XM/YM2151 sound and Souper mapper profiles add deterministic FM audio and supported
homebrew-cartridge mapping through the same deliverables and portable artifact.

## Core Value

7800 games behave as they do on the console, identically on every platform, from a small MIT-licensed C library
that any host can embed.

## Current State

Muhtari is not finished as a universal Atari 7800 compatibility project. It has
three deliverables: a C library, a headless runner and a libretro core. The
latest published package is v0.16.0 and includes the selected XM/YM2151 profile
and Souper mapper. The unpublished v0.15.0 draft and tag remain unchanged at the
source that failed its Wasm32 test.

The pinned title list has 204 entries: 201 are untested and 3 are skipped. Nine
older Playstead/frontend requirements remain unresolved. GameDrive and BupChip
streamed audio are not implemented. The public [status page](../docs/status.md)
summarizes these limits.

### Completion standard

A scoped feature is complete when its source-qualified behavior has deterministic,
bounded tests across the applicable library, runner and libretro paths; unsupported
cases are clearly refused; documentation is accurate; and the exact release
artifact passes required CI and installation checks. Project-wide compatibility
is complete only against a finite, explicitly selected title set with evidence
for every promised title. No universal title-compatibility target has been
selected, so do not claim the whole emulator is done.

v9.0 published the already-merged Souper support through the existing release
workflow. Its audit, exact-source release evidence and final main CI are recorded
in [the v9.0 audit](milestones/v9.0-MILESTONE-AUDIT.md), [the archive](milestones/v9.0-ROADMAP.md)
and [the phase release receipt](milestones/v9.0-phases/25-publish-the-corrected-souper-mapper/25-RELEASE.md).

## Latest Shipped Milestone: v9.0 Souper package publication

**Goal:** Publish the corrected, already-merged Souper mapper without changing
emulator behavior or moving the failed v0.15.0 tag.

**Outcome:** Stable v0.16.0 was published from release source `a0319a9c`. The
exact-source release, archive integrity/provenance, publication and latest-install
checks passed. The v0.15.0 draft and tag remained unchanged. All four milestone
requirements passed audit; the only review note is an informational README
repetition deferred until the overview is next revised.

**Delivery:** Stable v0.16.0 source
`a0319a9c29886d776e37b818d38d36fe810f486d` passed exact-source CI
[43801428495](https://github.com/szTheory/muhtari/actions/runs/43801428495)
and Release [43801428444](https://github.com/szTheory/muhtari/actions/runs/43801428444),
including publication and latest-install checks. Docs PR
[#73](https://github.com/szTheory/muhtari/pull/73) and phase closeout PR
[#74](https://github.com/szTheory/muhtari/pull/74) merged; their earlier main
receipts were CI [43841380444](https://github.com/szTheory/muhtari/actions/runs/43841380444)
and Release [43841380334](https://github.com/szTheory/muhtari/actions/runs/43841380334).
Archive PR #75 then merged source `bac988bea6a4ec070943c11b83298ac1eee5a8c3`;
its CI [43854355858](https://github.com/szTheory/muhtari/actions/runs/43854355858)
and Release [43854356086](https://github.com/szTheory/muhtari/actions/runs/43854356086)
passed. Post-archive state PR #76 is current source
`89b6ff82812cd5be873e9f90d32ac55fded8694b`; current main CI
[43866551295](https://github.com/szTheory/muhtari/actions/runs/43866551295)
passed and Release [43866551368](https://github.com/szTheory/muhtari/actions/runs/43866551368)
passed its target check with package work skipped because no new release was
created. The exact-source baseline and bounded no-change disposition are in the
[Phase 26 tune-up report](phases/26-measured-milestone-tune-up/26-TUNEUP.md).

**Closeout:** `verified_closeout`; no new artifact was acknowledged. Fifty
items acknowledged by earlier milestone closes remain suppressed with their
original verdicts. See [STATE.md](STATE.md) and the v9.0 audit.

**Current milestone:** v10.0 Headless developer trace. The scope selection and
strongest player-centered alternative are in the
[rolling product roadmap](PRODUCT-ROADMAP.md).

## Current Milestone: v10.0 Headless developer trace

**Goal:** Help homebrew authors and Muhtari maintainers diagnose Atari 7800
instruction and bus timing through an opt-in, reproducible headless trace.

**Target features:**
- Select trace output by frame and address range, with instruction/bus event,
  tick, beam position and CPU context.
- Bound trace output and verify its numeric event order using generated or
  owned test inputs.
- Prove observation leaves frame and state results unchanged and preserves the
  hooks-off performance gate.
- Document the runner workflow and trace record format.

**Compatibility boundary:** Keep the feature in `muhtari-run`, preserve public
API 8 and existing state formats, and keep callbacks/output host-owned. If a
public observer API or version change proves necessary, stop for an explicit
compatibility decision before expanding the scope. No GUI, host/device code,
interactive debugger, symbols, patching, scripting, Python/Gymnasium, BK2 or
AI/TAS work is selected. The milestone review and evidence are in
[the v9.0 successor review](research/next-milestone/2026-11-16-future-roadmap-review.md).

The 201 unevaluated titles and 3 skipped titles are not known failures. The
nine deferred v3.0 requirements and original Phase 12/14 stops remain open.

v8.0 GameDrive contract qualification is closed locally. The milestone audit covers all five requirements and passed; both phase verification reports retain passing verdicts. `override_closeout` accepts Phase 23's stale verification fingerprint because it predates the merged timeout follow-up, as allowed by GSD-HANDOFF §3 after merged-head CI passed. The qualification result is **NO-GO** for implementation because the authoritative stream profile, independent exact-profile PCM oracle and finite resource facts remain incomplete. PRs [#67](https://github.com/szTheory/muhtari/pull/67) and [#68](https://github.com/szTheory/muhtari/pull/68) are merged. Exact-main CI passed on the timeout-correction merge commit in [run 43757444707](https://github.com/szTheory/muhtari/actions/runs/43757444707); the Release workflow passed in [run 43757444676](https://github.com/szTheory/muhtari/actions/runs/43757444676) and skipped package publication. No GameDrive implementation, public compatibility change or hardware claim was made. The v0.15.0 tag/draft remains unpublished at its earlier failing source.

v6.0 YM2151 homebrew sound is implemented, verified and shipped as package v0.14.0. Both phases, all nine current requirements, required local gates and independent review/integration pass. The release workflow and latest-install check passed; PR #61 merged and main CI passed. See the [v6.0 ship receipt](milestones/v6.0-SHIP.md).

v7.0 Souper mapper support is implemented, phase-verified, audited and merged through PR #63. Its scope is one generated-fixture-testable mapper profile for homebrew developers; BupChip audio and Rikki & Vikki compatibility are excluded. All eight v7.0 requirements pass their phase verifications. The mapper follows the cited software profile; physical-cartridge accuracy and retail-game compatibility are not claimed. The v0.15.0 tag and draft release still point to `2b47656`, whose Wasm32 test failed; the test-only correction is merged as PR #65 and post-merge main CI passed, including Wasm32. The package is not published because the exact tag still identifies the failed source. See the [v7.0 shipping record](milestones/v7.0-SHIP.md), [v7.0 audit](milestones/v7.0-MILESTONE-AUDIT.md), [archived Phase 22 verification](milestones/v7.0-phases/22-complete-souper-homebrew-mapper/22-VERIFICATION.md) and [rolling product roadmap](PRODUCT-ROADMAP.md).

Phase 21 completed its source-qualified tune-up with a bounded no-change disposition. Phase 22 delivered and verified the Souper profile, deterministic CPU/MARIA mapping, snapshot continuation, and existing-consumer paths. The v7.0 local planning closeout remains historical; subsequent publication work merged the implementation, but the package release remains blocked on its exact-source gate.

Nine v3.0 gaps and every original denied/spent Phase12/14 host/remote scope remain unchanged. Older dirty worktrees stay preserved and root remains canonical. [Milestone audit](milestones/v6.0-MILESTONE-AUDIT.md) and [requirements](milestones/v6.0-REQUIREMENTS.md) retain the exact accepted scope.

## Prior Locally Completed Milestone: v8.0 GameDrive streamed-audio contract qualification

**Goal:** Decide whether GameDrive streamed ADPCM has a complete, bounded and lawfully testable contract for a future Muhtari implementation, without guessing at public media, audio or state compatibility.

**Outcome:** The public sources do not yet define the complete stream timing/framing contract or an independent exact-profile PCM oracle. Phase 24 therefore recommends **NO-GO** for implementation; no ADPCM playback, sidecar API, API/ABI or state-format change was made. Firmware 26.01.2266's potential glitch fix and save/load note establish a reason to qualify lifecycle behavior, not a complete implementation profile. The detailed evidence and reopen triggers are in the [qualification](milestones/v8.0-phases/24-qualify-the-gamedrive-audio-contract/24-QUALIFICATION.md).

**Closeout:** `override_closeout`; all five scoped requirements are covered by the passing milestone audit, and both phase verification reports retain passing verdicts. Phase 23's verification fingerprint predates the merged timeout follow-up; GSD-HANDOFF §3 accepts the stale fingerprint because the milestone audit covers the phase and exact-main CI passes. No new artifact was acknowledged; 50 prior acknowledgments remain suppressed with their original verdicts. The follow-up calibrated only the macOS x64 `title.pal.bios` deadline from 12 successful suite samples; other coverage and limits remain unchanged.

**Delivery:** PRs [#67](https://github.com/szTheory/muhtari/pull/67) and [#68](https://github.com/szTheory/muhtari/pull/68) are merged. Exact-main CI passed at [run 43757444707](https://github.com/szTheory/muhtari/actions/runs/43757444707). Release passed its target checks at [run 43757444676](https://github.com/szTheory/muhtari/actions/runs/43757444676), with package publication skipped. No milestone tag or package version was created; the separate v0.15.0 exact-source stop remains.

**At v8.0 closeout:** v9.0 published the corrected Souper source as v0.16.0.
The original Phase 12/14 stops, nine deferred v3.0 requirements and unaccepted
hardware/platform claims remain unchanged.

## Prior Locally Completed Milestone: v7.0 Souper homebrew mapper

**Goal:** Let homebrew developers load and run their own cartridges that use the published Souper mapper profile through Muhtari's existing C library, runner and libretro core.

**Delivered on the verified local source:**
- Measured recurring tune-up with current release/docs and evidence-specific quality findings.
- Souper v3/v4 recognition, validated 512 KiB ROM and 32 KiB EXRAM profile, bank/register behavior, distinct CPU/MARIA views and MARIA fetch trapping.
- Deterministic reset, transactional state continuation, generated numeric diagnostics, honest refusal boundaries and documentation across existing deliverables.

**Scope boundary:** Mapper-profile conformance only. No BupChip/BupBoop audio, Rikki & Vikki gameplay claim, unsupported extra payloads or flash variants, physical hardware calibration, host/UI work or new dependency. The result establishes the selected software model, not physical hardware or retail-game compatibility.

**Closeout:** Locally archived on 2026-11-14 with an override for one separately documented workspace-reconciliation item; the source verdict remains `unknown`. The later shipping result is recorded separately in [v7.0-SHIP.md](milestones/v7.0-SHIP.md); the v0.15.0 package remains unpublished. See the [v7.0 audit](milestones/v7.0-MILESTONE-AUDIT.md), [roadmap archive](milestones/v7.0-ROADMAP.md), [requirements archive](milestones/v7.0-REQUIREMENTS.md), [product roadmap snapshot](milestones/v7.0-PRODUCT-ROADMAP.md) and [phase archive](milestones/v7.0-phases/).

## Latest Shipped Milestone: v6.0 YM2151 homebrew sound

**Delivered:** One documented selected/header-auto XM/YM2151 profile; permissive C17 integer synthesis; deterministic bounded mono audio; complete transactional state/lifecycle; actual public C, runner, loaded libretro, emitted Wasm and clean installed/extracted consumers. Independent numeric and exact-seed evidence, genuine old/current legacy continuation, notices, source/fixture identity and fault controls pass.

API8/ABI1 remain unchanged. Global latest behavior7/format2 is distinct from unchanged non-YM machine snapshots6/1. Complete ordinary/adverse native/Node windows, memory/state extents and justified arithmetic/work bounds are recorded with their host-specific limits. Unavailable supplemental GCC/BE/WASI lanes have no inferred pass. There is no physical audibility/calibration, retail, browser/player or handheld acceptance claim.

**Publication:** [v0.14.0](https://github.com/szTheory/muhtari/releases/tag/v0.14.0) is published. The [release run](https://github.com/szTheory/muhtari/actions/runs/43441765548) and its latest-install check passed. PR [#61](https://github.com/szTheory/muhtari/pull/61) merged and current-main CI passed. See the [ship receipt](milestones/v6.0-SHIP.md).

**At the v6.0 closeout:** v7.0 had been audited locally and package shipping, publication, merge, remote dispatch, host/device and identity-bound actions remained separate boundaries. The current v8.0 selection is recorded above.

## Earlier Published Milestone: v5.0 Downloadable WebAssembly core

**Goal:** Host integrators can obtain a source-bound Emscripten libretro bundle, verified from its extracted contents and ready for the existing release pipeline.

**Target features:**
- Measured milestone tune-up with current release/docs and evidence-specific efficiency dispositions.
- One matched loader/Wasm archive with identity, integration instructions and audited notices.
- Extracted offline lifecycle/parity checks and strict existing release security contracts extended to the new variant.

The owner authorized autonomous local execution and ordinary recommendation following. Public rehearsal, publication, merge, new host/remote actions and the prior v3.0 stops keep their separate boundaries.

<details>
<summary>Historical v1.0 and v2.0 phase-position receipts (superseded at milestone close)</summary>

Planning milestone **v1.0** shipped 2026-10-21 with package **v0.4.0**.
The currently published package is [v0.7.0](https://github.com/szTheory/muhtari/releases/tag/v0.7.0),
published2026-10-25 after Phase8 merged in [PR31](https://github.com/szTheory/muhtari/pull/31). It has19 assets; no release pull request is pending at Phase9 closeout.
Four phases, 73 plans and 149 recorded tasks deliver games with sound in RetroArch.
[Release publication and latest-install run](https://github.com/szTheory/muhtari/actions/runs/39533130210) completed successfully; v0.4.0 is published with 19 assets.
At that release, the C/header tree had 38,684 tracked lines, including 8,577 in src/;
its public API was version 3, ABI 1, behaviour revision 2. The library remains integer-only,
instance-owned and bounded, without allocation after load.

Closeout is **override_closeout**: 15 items acknowledged as deferred (six todos, eight dormant seeds and one compatibility-test output race); four stale phase verification fingerprints explicitly accepted.
The [audit](milestones/v1.0-MILESTONE-AUDIT.md) records 25/25 scoped requirements
and no integration gaps. At that historical closeout, gate code was 15.94% of src/ and performance comparison
was report-only. Those values are superseded by Phase 5 below; the old audit stays historical.

Phase 5 is implemented, verified and merged in PR #20: 20/20 substantive truths and
nine active outcomes accepted. Direct CI 39879193530 and Release 39879193669 pass,
including genuine publish/download/install/confirmed cleanup. Its historical gate audit
recorded15,287 lines across147 files and retired an unused78-line allowance. The accepted
2026-10-23 policy subsequently retired maintenance-line admission and its exclusive machinery;
these historical quantities are not current limits. The controlled
test-card performance comparison is armed; WASM/library/property remain advisory.
Matched local scheduling improves wall time 18.24%; no global hosted-cost reduction
is claimed. WR-01 and COST-01 remain deferred tooling seeds; raw monthly numeric
cost totals are untrusted. See [Phase 5 verification](milestones/v2.0-phases/05-pipeline-tune-up/05-VERIFICATION.md).
Phase 5 shipped at6d57d69. Phase 6 is verified complete: Activision, Absolute and
Fractalus execute through all three deliverables, with identified-layout equivalence,
physical RAM aliases, bounded hostile-input evidence and honest host maps. That phase introduced
API4/ABI1/behavior3. Verification passes33/33; all six native archive sums fit
the unchanged399937-byte limit (worst375270). Direct CI40031550434, Nightly40031550533
and Release40031550534 attempt2 pass, including confirmed rehearsal cleanup.
Phase 6 merged in [PR24](https://github.com/szTheory/muhtari/pull/24); v0.5.0 is published.

Phase 7 is verified complete and shipped in PR28: API5/ABI1/behavior4 delivers
HSC/SaveKey battery storage, checked runner persistence and real two-launch RetroArch saves.
All nine plans and SAVE-01–06 pass35/35 must-haves. Final CI40104749040 attempt2 and
Nightly40104750452 pass on source54485d6, with all six actual native archive sums
within399937 (worst393952). Full native/PPC641+6 and69-row equality, WASM580,
10000-case properties, fuzz and actual frontend/Windows controls pass. Review is clean
after both allowed fix rounds; no owner verification remains. See
[Phase 7 verification](milestones/v2.0-phases/07-battery-saves/07-VERIFICATION.md).

Phase8 is verified complete and shipped in PR31: API6/ABI1/behavior5 adds
extended typed input, immutable movie2 replay and all in-scope controller models.
All14 plans and INPUT-01–05 pass49/49 truths; one review-fix round corrected a
short-packet aim boundary and physical button labels. Exact-source CI40157749294
passes native649+6 across six current/base lanes, full native/PPC649+6, WASM587,
342-row equality and actual RetroArch. All18 candidate ZIPs and authenticated
current/base measurements pass; DEC.61 keeps archive bytes report-only.
Nightly40157750598 retains an explicitly unaccepted advisory10000-case isolation
timeout; five other10000-case properties pass. Required isolation passes. Hardware
fits remain unmeasured; the quoted execution evidence retains its original scope.
See [Phase8 verification](milestones/v2.0-phases/08-controllers-at-the-pins/08-VERIFICATION.md).

Phase9 is verified complete and ready for `$gsd-ship 9`: all8 plans and PAL-01–03
pass42/42 truths. Selected313-line timing,320x292 geometry/rational rates and warm
PAL presentation reach the library, runner and both libretro formats. Independent
startup plus600 full-frame evidence precedes956-row equality; all342 prior NTSC
observations remain exact. API6/ABI1/behavior5 remains unchanged. One review-fix
round adds16KiB private BIOS staging, resolving borrowed-media lifetime without
extra allocation or per-frame work; a poison-based regression proves failure
preservation and successful boot in both regions. Re-review is clean.

Fresh CI40233948646 and Nightly40233948516 attempt2 accept the reviewed source:
six native current654/base649+6, ASan637, full native/PPC654+6, WASM592, full vectors,
fuzz, authenticated package/performance evidence and the two real frontend cases.
One targeted PPC retry followed a29-minute external provisioning delay; the
unchanged retry setup completed in13 seconds. The advisory isolation property
still times out and remains unaccepted; model handoff/optical/RC fits and85:342
analog balance retain explicit confidence limits. No routine owner UAT remains.
See [Phase9 verification](milestones/v2.0-phases/09-pal-games-and-audio/09-VERIFICATION.md).

Phase 11 is verified complete: all12 plans and COMP-01–03 pass16/16 truths.
The204 pinned CC0 entries remain201 unknown and3 skipped, with no metadata
counted as a gameplay pass. Public title-screen evidence, owned model conformance,
and unsupported scope retain their explicit labels. Fresh local CI673+6 and
public library47 pass; the counted run has0/204 present at0e86575f561b, with
planning-only dirty state. Two carried claim/output faults are accepted through
existing controls; no new review round or owner UAT. WR-02 helper/source-only
coverage and COMP-03 unclassified assumption remain explicit; hosted outcomes
are confirmed at ship. See [Phase11 verification](milestones/v2.0-phases/11-full-library-acceptance/11-VERIFICATION.md).
Next separate command: $gsd-ship 11; milestone audit/archive follows shipping.

</details>

## Earlier Milestone: v4.0 Browser/WebAssembly core readiness

**Goal:** Make the existing libretro core buildable for browser hosts and prove its deterministic behavior with offline, repeatable checks, while keeping the core small and portable.

**Verified phases:**
- Phase 15: corrected document-budget root coverage with measured pass/fault evidence.
- Phase 16: emitted Emscripten module, direct offline Node lifecycle and independently checked native parity with refusal/replay/isolation controls.

**Scope boundary:** No browser player, page, menu, window, audio-device or input-device code. A headless harness proves the core artifact and callback contract; it does not claim a shipped browser experience or measured performance on user devices. v3.0's actual RetroArch and Playstead gaps stay deferred with their original stops intact.

## Requirements

### Validated

- ✓ TUNE-13, REL-01, REL-02, DOC-01: published corrected Souper support as v0.16.0, preserved the v0.15.0 draft/tag, passed source-bound release and latest-install gates, and updated availability docs — v9.0, Phase 25; [audit](milestones/v9.0-MILESTONE-AUDIT.md).

- ✓ YM-01–08: one qualified XM/YM2151 profile, deterministic C17 mono synthesis, complete lifecycle/state, native/portable/package consumers and measured bounds — v6.0, Phase 20; shipped as v0.14.0.

- ✓ TUNE-09: current v4 publication records and source-specific measured baseline; required checks retained — v5.0, Phase 17.
- ✓ WASM-05–09: matched source-bound portable ZIP, clean-extracted offline oracle, pinned runtime notices, existing release contracts and host instructions — v5.0, Phase 18.

- ✓ TUNE-08: relative-root document budget coverage and representative controls — v4.0, Phase 15.
- ✓ WASM-01: pinned Emscripten ES module and Wasm with documented loader/export contract — v4.0, Phase 16.
- ✓ WASM-02: direct offline Node lifecycle, callbacks and state consumer — v4.0, Phase 16.
- ✓ WASM-03: independent native numeric parity, source/fixture identity and refusal/replay/isolation — v4.0, Phase 16.
- ✓ WASM-04: same-source size and warmed headless throughput reports with explicit limits — v4.0, Phase 16.

- ✓ `cmake --workflow --preset ci` (`ci-msvc` on Windows) builds and tests everything on Linux, macOS and
  Windows, x64 and arm64, and CI runs the same commands behind one required check — v1.0, Phase 1
- ✓ One command installs the latest release into RetroArch on an Apple Silicon Mac, and a hosted test shows
  RetroArch displays the runner's frame — v1.0, Phase 1; confirmed when v0.1.0 published (Release run 38983378877
  passed install-line-latest)
- ✓ Every behaviour-changing merge to `main` publishes a release (library, runner, libretro core) with no
  manual step — v1.0, Phase 1; confirmed by v0.1.0 (release pull request #8 merged itself, Release run 38983378877)
- ✓ The 6502C "SALLY" CPU matches the public 6502 vectors on every opcode and every bus cycle, with the 7800's
  slow cycles, HALT and RDY — v1.0, Phase 2
- ✓ The runner probes cartridge images and the core loads one only after validating it, with no allocation on a
  refusal and a loader fuzz target run nightly — v1.0, Phase 2

- ✓ Linear and SuperGame cartridges render with exact MARIA DMA timing, boot fast or through OpenBIOS,
  take joystick and console-switch input, and play in RetroArch with cross-platform frame hashes — v1.0, Phase 3

- ✓ TIA and declared cartridge POKEY deliver sound through the library, canonical runner hashes and actual
  libretro batch/fallback paths; all eight sound samples are non-silent and clipping-free — v1.0, Phase 4
- ✓ Owned exact-q23-16x64-dc20 synthesis passes the production spectral/alias judge with renewed integer
  headroom/width/carry proofs; hostile legal chip traffic stays bounded — v1.0, Phase 4

- ✓ Reliable, measured pipeline and current release docs — TUNE-01–05, Phase 5 verified and shipped in PR #20 — v2.0.
- ✓ Activision, Absolute and Fractalus banking, physical RAM and three-host behavior — BOARD-01–03, Phase 6 shipped in PR24 — v2.0.
- ✓ HSC/SaveKey battery data survives host and frontend restarts — SAVE-01–06, Phase7 shipped in PR28 — v2.0.
- ✓ Source-qualified lightgun, analog/matrix, timed motion and complete adapter inputs across all three deliverables — INPUT-01–05, Phase8 shipped in PR31 — v2.0.
- ✓ Selected PAL timing, presentation and audio across all three deliverables — PAL-01–03 — v2.0, Phase 9 merged in PR33.
- ✓ BankSet and XBoard/XM memory, POKEY and HSC with correct CPU/MARIA ownership — BOARD-04–05 — v2.0, Phase 10 merged in PR36.

- ✓ Evidence-backed library accounting and isolated local evaluation — COMP-01–03 — v2.0, Phase 11 merged in PR40.
- ✓ Portable validated machine state through the library, runner and libretro core — STATE-01–06 and FRONT-01 — v3.0 Phase 13, verification passed 44/44.
- ✓ Current publication/docs and proportionate measured quality/efficiency baseline — TUNE-10, v6.0 Phase 19.
- ✓ Source-qualified release/docs and quality/efficiency tune-up with a bounded no-change disposition — TUNE-11, v7.0 Phase 21.
- ✓ Exact Souper mapper-only software profile, CPU/MARIA mapping, deterministic lifecycle/state and existing consumers — SOUPER-01–07, v7.0 Phase 22; local verification passed.
- ✓ Source-qualified release, CI, hygiene and quality evidence with a bounded tune-up and a measured single-deadline correction — TUNE-12, v8.0 Phase 23.
- ✓ GameDrive contract evidence, legal generated-fixture/oracle feasibility, consumer/resource/state consequences and an evidence-backed no-go — GDA-01–04, v8.0 Phase 24.

### Active

- [ ] **TUNE-14**: Complete the recurring evidence-based release and engineering tune-up without weakening required checks.
- [ ] **TRACE-01–07**: Deliver and document one bounded, deterministic, runner-only instruction and bus-access trace while preserving API 8, existing state formats and the hooks-off cost gate.

The nine unmet v3.0 requirements remain deferred with their original evidence
and stops; see the [v3.0 archive](milestones/v3.0-MILESTONE-AUDIT.md).

### Out of Scope

- A GUI, or any window, audio-device or input-device code — RetroArch (and later Playstead) is the host.
- Shaders, menus, input remapping, cheats, patching, rewind buffers and netplay lobbies — these belong to the host;
  the core provides deterministic state and memory maps.
- Any Atari BIOS, High Score Cartridge ROM, BIOS-loader bytes or game ROM in the repository or its releases —
  players supply their own; the default boot needs no BIOS.
- Code from GPL, LGPL or unlicensed emulators — the project is MIT; such emulators run only as test oracles.
- A separate fast or low-accuracy mode — one accurate core keeps one set of bugs and one set of hashes.
- Signed or notarised macOS artifacts — the script install path does not need them.
- A test framework dependency — plain CTest executables with an in-repo check header cover the need.
- Atari 2600 emulation — owned by the sibling project `twentysixhundo` (owner correction, 2026-11-11);
  SEED-006 is retired. Existing image recognition/refusal remains supported.
- GameDrive/Covox ADPCM implementation until its source profile, legal independent oracle, finite bounds and public compatibility consequences are qualified; BupChip/BupBoop implementation, other YM2151 topologies, ACE, full OpenHSC,
  AtariVox speech, measured-accuracy oracles, the browser player, full movie recording/generalized continuation,
  advanced AI/TAS tools and ports — remain outside current scope; see `.planning/seeds/` and `.planning/PRODUCT-ROADMAP.md`.

## Context

- **v3.0 closeout.** Phase 13's portable machine-state path is verified; Phase 12's host and Windows claims and Phase 14's RetroArch state acceptance remain unresolved. The denied Phase 12 permission request ended before preflight/content and is not retried. See the v3.0 audit for exact scope and evidence.
- **Next portability slice.** The owner delegated downscoping and milestone selection after the v3.0 acceptance deadlock. v4.0 takes the already-ranked browser/WebAssembly direction but limits Muhtari work to the existing libretro core and offline deterministic checks; the browser host owns presentation and devices.
- **v7.0 evidence.** The original Souper author released the mapper so other developers could build on it. A public GameDrive code example and the read-only pinned RTL cross-check define one testable software profile; neither is physical-hardware evidence. The older Rikki & Vikki image-size claim is conflicting, and its game image is not a fixture. LibFuturo's GabbaBoy save work is sibling-owned and creates no demonstrated Muhtari requirement.
- **v8.0 result.** The qualification found that firmware 26.01.2266's potential playback-glitch fix and save/load note do not define stream timing, framing or independent expected audio. The sample and encoder repositories remain unlicensed; do not copy their code. Implementation remains NO-GO until an authoritative profile, legal independent oracle and finite bounds are available. See the [archived qualification](milestones/v8.0-phases/24-qualify-the-gamedrive-audio-contract/24-QUALIFICATION.md).

- **Ecosystem.** The only libretro 7800 core is ProSystem (GPL-2, per-line timing, many homebrew failures); the
  accuracy references are a7800 (MAME-derived, last release 2022), BizHawk's A7800Hawk (C#) and the MiSTer FPGA
  core (MIT since 2026-08-30). No permissive, accurate, embeddable C core exists, and every software emulator runs
  at 60/50 Hz or the 2600's rate instead of the 7800's own 59.958/49.920 Hz.
- **Sibling projects.** The owner's NES (nesturbator), Game Boy (gabbaboy) and Neo Geo (glueyneo) cores share
  this project's API conventions and engineering; nesturbator's own MIT code seeds the CPU, synthesiser, state,
  runner, libretro adapter and CI. Playstead, the owner's host, launches emulator processes and needs
  deterministic input replay and safe battery saves from them.
- **Project memory entry.** Start at [the planning index](README.md) and [shared intent](INTENT.md).
  Muhtari currently leads the owner's emulator family; learning flows both ways, with local
  reproduction before adopting sibling mechanisms or hardware claims.
- **Research recall.** Use `.planning/research/fan-out/prompt.txt` for milestone/advisory fan-out.
  Relevant roles examine real decisions, compare alternatives and perform one adversarial synthesis;
  expand only when new evidence could change a recommendation. Reuse permissive seeds and standard
  facilities before adding dependencies. Store source-backed takeaways with confidence and revisit
  triggers; keep untested ideas distinct from reproduced LESSONS. Detailed execution stays downstream.
- **How releases are tried.** In RetroArch (Metal build) on an Apple Silicon Mac, installed by a one-line script.
- **Reference material.** Each file states facts with sources and one recommendation per choice. Open the one that
  matches the work before designing or building. Index: `.planning/preparation/README.md`.
  - `.planning/preparation/ARCHITECTURE.md` — time model, modules, public API, I/O contracts, observability
  - `.planning/preparation/DECISIONS.md` — choices already made, one line each
  - `.planning/preparation/A7800-SYSTEM.md` — clocks, memory map, SALLY, HALT/RDY, INPTCTRL, RIOT, boot
  - `.planning/preparation/A7800-MARIA.md` — display lists, read modes, DMA timing, video output, palettes
  - `.planning/preparation/A7800-AUDIO.md` — TIA audio, POKEY, mixing, synthesis, YM2151
  - `.planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md` — A78 header, boards, controllers, saves
  - `.planning/preparation/CONFORMANCE.md` — test ROMs, oracles, diagnostics, the ledger, CI tiers
  - `.planning/preparation/HOSTS-AND-INTEGRATION.md` — libretro, RetroAchievements, RetroArch, runner, Playstead
  - `.planning/preparation/ENGINEERING.md` — C rules, presets, tests, CI, release, repository settings
  - `.planning/preparation/A7800-ECOSYSTEM-AND-USERS.md` — other emulators, complaints, personas, device budgets
  - `PROVENANCE.md`, `ASSET_POLICY.md` — what code and which bytes may enter the repository

## Constraints

- **Deliverables**: a C library, a headless runner and a libretro core only — the host owns windows, audio devices
  and input devices.
- **Language**: C17 without extensions; the core links only the C memory functions — it has to embed anywhere.
- **Reuse**: permissive code enters only as a seed with its notice; GPL, LGPL and unlicensed source is never opened
  — the repository is public and MIT.
- **Determinism**: integer-only core with all state in the instance — the same inputs give identical frame and
  audio hashes on every platform; every call is bounded in emulated time.
- **Efficiency**: efficient enough for battery-powered handhelds and for the browser through WebAssembly, which the
  owner ranks very important — work per emulated frame sets both speed and power draw, so the instructions-per-frame
  comparison measures it from phase 1 and is armed after the Phase 5 controls, and nothing enters the core that a wasm32 build cannot carry (threads, files,
  floating point, allocation after load). The browser build is SEED-008; handheld budgets are SEED-005.
- **Legal**: no ROM, BIOS or firmware bytes beyond licence-checked fixtures; Atari names only to say what the core
  emulates.
- **Privacy**: no personal paths, addresses or names in tracked files, commits or release artifacts.
- **Verification**: every behaviour is shown by a command; nothing waits on a person except identity-bound
  consent.
- **Onboarding**: reduce repeated player setup through verified install and launch commands, alongside a GUI
  path. Agents automate local setup and smoke checks, keep personal game data outside the repository, and
  turn reproduced friction into short user docs and project-local lessons (owner, 2026-10-29).
- **Delivery**: each phase is one branch and one pull request into a green `main`, released automatically — each
  phase ends in something to download and run.

## Key Decisions

| Decision | Rationale | Outcome |
|----------|-----------|---------|
| Publish corrected Souper through the ordinary feature-release path and keep CI coverage unchanged | A pre-1.0 feature release follows the minor-bump rule; release evidence supports no pipeline change | ✓ v9.0 Phase 25: stable v0.16.0 passed exact-source release and latest-install checks; v0.15.0 stayed unchanged; see [audit](milestones/v9.0-MILESTONE-AUDIT.md) |
| Keep GameDrive implementation at NO-GO until its profile, independent PCM oracle and finite resource facts are qualified | Prevents guessed stream timing/framing or unlicensed fixture reuse from becoming public media/API/state behavior | ✓ v8.0 Phase 24: all four consumer and compatibility consequences recorded; no implementation or public compatibility change selected |
| Calibrate only the macOS x64 PAL BIOS predicate deadline from completed matching-lane samples | The exact-source timeout and missing quality artifact share one failed matrix result; measured samples justify a bounded correction without reducing coverage | ✓ v8.0 Phase 23 follow-up: 12 complete suite samples, 5.88 s maximum, 12 s registered timeout; exact-main CI green |
| Keep CI topology, quality policy, Windows serial order and unrelated deadlines unchanged after v8.0's measured review | The new timeout has a bounded sample-based correction; one critical-path observation does not justify structural or cache changes | ✓ Phase 23: original coverage, fault behavior and job structure remain intact; no safe queue/critical-path change was supported by the reviewed runs |
| Keep required CI coverage, workload limits and quality floors unchanged after the measured tune-up | One accepted hosted release sample and different-source local measurements do not establish a safe reduction with equivalent fault detection | ✓ Phase 21: source-qualified report records bounded no change, preserves prior failures and leaves the spec-less probe unresolved |
| Deliver one mapper-only Souper software profile with additive state compatibility | A published mapping example supports generated CPU/MARIA oracles; physical and retail compatibility remain outside the evidence | ✓ Phase 22: exact 512 KiB ROM/32 KiB EXRAM profile, deterministic lifecycle, format 3/behavior 8 snapshots, and existing consumer paths verified; API 8/ABI 1 and legacy/YM state contracts preserved |
| One source-qualified XM/YM2151 digital profile and approved exact-pin BSD-3 OPM specialization | Preserve C17 integer instance ownership, existing scheduler and evidence boundaries | ✓ v6.0: native/portable/package diagnostics, exact-seed parity and maintained notices |
| Preserve non-YM native6/1 bytes while YM alone uses7/2; API8/ABI1 unchanged | Add full YM continuation without breaking existing machine envelopes | ✓ v6.0: genuine independent old/current two-way matrix and transactional semantic refusal |
| Merge every coinstalled audio source before advancing the mixer | Cross-route equality can share a clock-ordering bug | ✓ v6.0: Release monotonic all12-source stress and qualified sanitizer reproduce before/fix after; existing bounds retained |
| Measure actual named native/Wasm workloads without inventing speed or space quotas | Host timing and structural proofs support different claims | ✓ v6.0: complete both-region ordinary/adverse windows and concrete extents, unavailable platforms explicitly unaccepted |
| Install expansion topology once and share the electrical bus owner | Preserve physical storage and CPU/MARIA ownership without a second scheduler | ✓ v2.0 Phase 10: BankSet/XBoard/XM diagnostics and 3088 retained observations |
| Judge claims by public evidence class | Metadata and title screens cannot establish gameplay | ✓ v2.0 Phase 11: generated claims, 204 explicit dispositions and required ratchet |
| Load/refuse before optional firmware I/O in the runner | Preserve canonical refused-image exits and avoid irrelevant file reads | ✓ v2.0 Phase 10: exit77 regression coverage |
| Keep deferred tooling debt and historical failures visible | Bounded closure must preserve evidence | ⚠ v2.0 override closeout retains COST-01, WR-01 and optional follow-ups |
| One load-selected regional profile | Keep integer timing, geometry and audio carry consistent across existing owners | ✓ Phase9: PAL and retained NTSC behavior verified across all hosts/platforms |
| Judge numeric PAL behavior before hash agreement | Independent expectations catch errors shared by hosts | ✓ Phase9:601 calls,577879 samples,CRC52a5 precede956-row equality |
| Stage borrowed firmware before load commit | Installation can invalidate public input spans while failure must preserve the live machine | ✓ Phase9:16KiB private BIOS staging, poison-based regression and both-region lifecycle proof |
| Separate typed input from frozen legacy input | Preserve existing linkage while adding transactional selection and once-only motion | ✓ Phase8: API6,88-byte extended packet and linked old clients verified |
| Sense native emission through existing chip/deadline owners | Retain historical light and bounded unpolled behavior independently of host buffers | ✓ Phase8: explicit unmeasured optical/RC/matrix/motion fits, complete adapter edges |
| Judge independent controller numbers before hash agreement | Equal implementations alone can share an error | ✓ Phase8: thirteen scenarios,342 rows, actual library/runner/module and cross-platform evidence |
| One load-stable raw battery span and frozen attachment | Hosts restore directly before execution without mirrored storage or hot resizing | ✓ Phase 7: API5, layout1, exact HSC/EEPROM/combined lengths and old-layout linkage |
| Core commits hardware bytes; runner and frontend own persistence | Keep deterministic emulation separate from host files and controls | ✓ Phase 7: real serial deadlines, checked replacement, native controls and fresh-process byte equality |
| Require independent complete media and pixel expectations | Equal renderers or a successful exit alone cannot prove restoration | ✓ Phase 7: two fresh launches, both counters, all34816 bytes and oracle faults verified |
| Freeze Phase 5 at existing 15/09; tighten scoped additions to actual size (D-35) | Stop repeated budget/replanning cycles while preserving every required check | ✓ Nine active outcomes accepted; actual 15287, 78 retired; no plan16 or allowance reset |
| Keep conservative release rehearsals and advisory portability lanes (D-31) | Reuse needs independently proven input identity; lane promotion needs qualifying main history | ✓ Genuine current rehearsal accepted; no reuse state or unearned promotion |
| Tooling review findings remain seeds (AGENTS rule 7) | Bound phase closure without hiding known limitations | ✓ WR-01/COST-01 deferred explicitly; no trusted raw monthly cost claim |
| Time in MARIA clock ticks; the CPU drives the bus; MARIA DMA runs as one exact burst per line | Every documented edge is an integer; cheaper than per-tick stepping on low-power targets | ✓ v1.0: 4/6-tick CPU cycles, holds and queued NMI integrate with exact MARIA bursts and diagnostic timing |
| Integer-only core, no globals, bounded run calls | Bit-identical hashes everywhere; hostile ROMs cannot hang a host | ✓ Phase 1: tests enforce no floating point, no globals and the no-FP lanes; Phase 2: a step on a jammed CPU still makes one bus read, so emulated time keeps moving |
| Native MARIA byte per pixel; palettes and composite filter outside the hash | Hash stability; hosts choose presentation | ✓ Phase 1: frames are colour numbers; the runner and the libretro core apply the palette |
| Audio hashed as a transition stream; PCM secondary | Determinism independent of the mixer and output rate | ✓ Phase 4: canonical11-byte raw records and LE mono PCM have separate completed-run digests; fixed identities and half-open ownership |
| Reuse the owner's nesturbator code; MARIA, TIA, RIOT, POKEY and boards written from documentation and cross-checked | Fastest correct path; existing permissive chip models are lumped or diverge from documentation | ✓ v1.0: seeded SALLY passes all vectors; documented RIOT, MARIA, TIA, POKEY and linear/SuperGame implementations ship |
| Fast boot by default; CC0 OpenBIOS for the BIOS path in CI; no Atari bytes | Redistributable and legal | ✓ v1.0: fast/OpenBIOS boot and firmware scans run in CI; release artifacts contain no BIOS |
| Tests on licence-checked inputs: public 6502 vectors, 7800basic samples built in CI, own diagnostic ROMs | Reproducible without a network or private ROMs | ✓ Phase 2: the SingleStepTests sample runs on every PR and the full set nightly; 7800basic's `simple` sample builds byte-identical on all nine legs |
| One compatibility ledger in which a pass stays a pass | A regression is a one-line diff | ✓ Phase 3: the ledger, ratchet and generated compatibility page run in CI |
| One entrypoint: CMake workflow presets; one required CI check | No script to keep in step with CI | ✓ Phase 1: CI runs the presets on six platforms behind `CI required` |
| release-please with a GitHub App token; version-free asset names; unsigned macOS artifacts installed by script | Automatic releases with no owner step | ✓ v1.0: package v0.4.0 published automatically and install-line-latest passed, run 39533130210 |
| With no cartridge loaded the core outputs a built-in test card | A build or a release can be checked without any ROM | ✓ Phase 1: the card's hash is pinned and RetroArch shows it pixel for pixel |
| Linear and SuperGame boards first, then POKEY | 187 of 204 known titles; 195 with POKEY | ✓ Phase 3 boards; Phase 4 declared primary/secondary POKEY maps, clocking and accepted-v4 timer IRQs |
| The owner verifies nothing by default: tests, CI and the agent close every check, post-merge truths are confirmed at ship (DEC.09, 2026-10-16) | The owner's time goes to consent, not to reading docs or watching runs | ✓ Phase 1: UAT closed with no owner check |
| Public API version 2 adds `muhtari_probe`; a cartridge is validated before anything is allocated (D-01, D-02, D-27) | A host can identify an image without loading it; a hostile image cannot make the core allocate | ✓ Phase 2: `cart.load.no_alloc_on_reject` covers every refusal class; the loader fuzz target runs nightly |
| The CPU, bus and cartridge tables build once as OBJECT libraries inside the one shipped archive (D-14) | Tests run the CPU object on a flat bus without the library; one archive still ships | ✓ Phase 2: Cachegrind measured a 0.31 % Ir gap against a single translation unit |

The full list with sources is `.planning/preparation/DECISIONS.md`.

| Close v3.0 with nine requirements deferred; continue only the independently buildable next scope | Preserve all original Phase 12/14 verdicts and stops after the denied owner permission request; Phase 13 remains verified | ✓ 2026-11-10: override closeout recorded in the v3.0 archive; no host/remote renewal |
| Promote WebAssembly core portability after the recurring tune-up | Reuses the existing three-deliverable contract and the owner's high-priority SEED-008; avoids making a browser UI a Muhtari deliverable | ✓ v4.0 verified locally: emitted artifact, direct Node host and independent native checks; browser/device acceptance remains deferred |
| Select v7.0 mapper-only Souper support | Gives 7800 homebrew developers a core-owned capability with generated acceptance; current sources do not justify BupChip or named-game claims | ✓ Phase 22: implemented and verified on the local source; see [v7-SOUPER](research/next-milestone/v7-SOUPER.md) |

## Evolution

This document evolves at phase transitions and milestone boundaries.

**After each phase transition** (via `/gsd-transition`):
1. Requirements invalidated? → Move to Out of Scope with reason
2. Requirements validated? → Move to Validated with phase reference
3. New requirements emerged? → Add to Active
4. Decisions to log? → Add to Key Decisions
5. "What This Is" still accurate? → Update if drifted

**After each milestone** (via `/gsd-complete-milestone`):
1. Full review of all sections
2. Core Value check — still the right priority?
3. Review Out of Scope — reasons still valid?
4. Update Context with current state

---
*Last updated: 2026-11-16 as v10.0 milestone planning began*
