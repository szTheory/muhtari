---
title: "Decisions already made"
summary: "One line per settled choice for muhtari, with its reason and the file and section that support it; open questions at the end."
read_when: "Before reopening any choice, and whenever a phase needs to know what is settled. Reopen a decision only with new evidence, and record the change here with its date."
updated: 2026-10-23
---

# Decisions already made

Each row is settled for v1.0 unless a phase finds new evidence; a change keeps the ID, gives the date and links the
evidence. IDs use a dot (`DEC.07`) so they never collide with GSD's `D-NN` phase decisions.

## Product and scope
| ID | Decision | Because | See |
|---|---|---|---|
| DEC.01 | Three deliverables: C library, headless runner `muhtari-run`, libretro core; no GUI, window, audio-device or input-device code | Owner rule that core repositories own no GUI; RetroArch is the Mac frontend; Playstead launches processes | HOSTS-AND-INTEGRATION 1, 4 |
| DEC.02 | MIT licence | Compatible with every frontend's licence; same as the siblings | PROVENANCE 1 |
| DEC.03 | Atari 2600 cartridges come after the 7800 library (SEED-006); until then they are recognised and refused with "unsupported: 2600 (planned)" | Owner decision 2026-10-15; Stella covers 2600 meanwhile | A7800-CARTRIDGES-AND-PERIPHERALS 1 |
| DEC.04 | Exact timing is a feature: hosts are told 39375000/656711 Hz NTSC and 3546894/71051 Hz PAL; nothing on by default changes speed | Existing 7800 emulators run at 60/50 Hz or the 2600's rate, which skews speedrun and TAS timing | A7800-ECOSYSTEM-AND-USERS 3; A7800-MARIA 6 |
| DEC.05 | Horizon order: players and homebrew developers (v1.0–v2.0), states and movies (v3.0), measured accuracy (v4.0), tools and AI (v5.0), everywhere (v6.0) | Personas ranked by reach and dependency | A7800-ECOSYSTEM-AND-USERS 3; PRODUCT-ROADMAP |

## Process
| ID | Decision | Because | See |
|---|---|---|---|
| DEC.06 | Pre-seeded GSD bootstrap: the kickoff resumes at roadmap creation; GSD's researcher still runs before every phase | Fastest sibling's path; owner choice | GSD-HANDOFF 1–2 |
| DEC.07 | Interactive mode, both chain flags off, one GSD step per session; a hygiene check refuses commits that flip them | Owner: stop after every phase and switch models | GSD-HANDOFF 5–6 |
| DEC.08 | Every phase ends in an automated release; phase 1 is a ROM-free walking skeleton; no phase whose goal is a study | Sibling evidence (LESSONS XFER-001–003) | research/SUMMARY.md |
| DEC.09 | Verification is automation-first; settled, never re-proposed. 2026-10-16: the owner verifies nothing by default; the agent makes the judgement calls (docs against code, images, RetroArch runs), records a one-off check as a command and exit status, and confirms post-merge truths at ship; only identity-bound consent or a run on the owner's own hardware reaches the owner | Owner directive; sibling evidence. 2026-10-16: phase 1's UAT asked the owner for two doc reads and a post-merge check that tests, CI and the agent then settled | AGENTS rule 6; GSD-HANDOFF 3 |
| DEC.10 | Proportional gates: CMake `-P`/POSIX sh, bounded review and existing finite review/gap limits. Accepted 2026-10-23: retire maintenance LOC ceilings, allocations, counting and exclusive admission machinery; use concrete contracts and ENGINEERING §11 architecture review, with optional tooling improvements as seeds and existing contract violations blocking wherever they live. Historical 2026-10-21 absolute ratchet replaced the old 10% ceiling; its Phase 5 acceptance remains historical | Owner accepted researched replacement after the Phase 6 halt; text volume did not establish maintenance burden. Preserve product/safety/evidence contracts without a substitute score or N654 approval | AGENTS rule 7; ENGINEERING 11; .planning/research/proportional-gates/RECOMMENDATION.md; historical Phase 5 CONTEXT D-17, D-19–20 and Phase 6 halt/redesign |
| DEC.11 | Confidence-qualified models: the best-supported timing model with named, cited constants; unmeasured values are labelled; re-plan only with new evidence | Several 7800 timings are fitted, not measured | ARCHITECTURE; A7800-MARIA 4.8 |
| DEC.12 | Branch before the first committing GSD step; no commits on `main` once a remote exists; one squash PR per phase, retitled as a Conventional Commit, auto-merged | GSD 1.17.0 branches only at execute; ship titles PRs "Phase N" | AGENTS rule 9; ENGINEERING 6 |
| DEC.13 | `model_profile: inherit` | The session model is the single knob the owner switches between steps | GSD-HANDOFF 5 |
| DEC.14 | Planning milestones are v1.0, v2.0…; packages are pre-1.0 semver: first release 0.1.0, `feat` and breaking changes bump minor | release-please 17.6.0 semantics; meaningful versions for frontends | ENGINEERING 5 |

## Licence and provenance
| ID | Decision | Because | See |
|---|---|---|---|
| DEC.15 | Another project's expression enters only as a seed with its notice, replacement trigger and parity proof; GPL, LGPL and unlicensed source is never opened; reference emulators run only as released binaries | Protects the MIT licence (an AI-assisted MIT emulator had to relicense in 2026) | PROVENANCE 1, 4 |
| DEC.16 | Seeds: the owner's nesturbator (CPU, bus, synthesiser, state, runner, libretro adapter, CI, hygiene); a7800's BSD-3 palette tables; ymfm for the YM2151 (v4.0) | Fastest correct path; same-owner code needs no notice | PROVENANCE 4 |
| DEC.17 | MARIA, TIA audio, RIOT, POKEY, boards, ports and boot are own code from documentation and measured facts, cross-checked against MAME/a7800 (BSD-3), MiSTer licensor-only files (MIT) and A7800Hawk (MIT) | MAME's MARIA is a per-line lump; MAME's POKEY diverges from the Altirra manual in documented ways | A7800-MARIA 10; A7800-AUDIO 5 |
| DEC.18 | No Atari BIOS, HSC ROM or BIOS-loader bytes, ever; fast boot by default; the CC0 OpenBIOS is built in CI for the BIOS path and never committed or packaged | Firmware copyright; the OpenBIOS splash draws the Atari Fuji | PROVENANCE 6; ASSET_POLICY 1, 3 |
| DEC.19 | 7800basic samples and OpenBIOS are built in CI from pinned sources (7800basic's WASM tools under wasmtime); only hashes are committed | Per-byte licences (RMT player, LZSA, unstated samples); builds are byte-identical across hosts (tested) | CONFORMANCE 2; ASSET_POLICY 3 |
| DEC.20 | MAME's CC0 `hash/a7800.xml` (board, RAM, POKEY, region, peripherals; Activision layout normalised by hash), plus the SHA-1s of its CC0 `hash/a2600.xml` to recognise 2600 images, are the only bundled databases | CC0; headerless dumps need it | A7800-CARTRIDGES-AND-PERIPHERALS 1; PROVENANCE 7 |
| DEC.21 | `LICENSE` and `THIRD-PARTY-NOTICES.md` in every release archive, checked by a packaging test | BSD-3 and MIT notices must travel with binaries | PROVENANCE 5 |

## Architecture
| ID | Decision | Because | See |
|---|---|---|---|
| DEC.22 | One `uint64_t` tick per MARIA clock; CPU cycles of 4 ticks, 6 for TIA/RIOT; a CPU-driven bus with one deadline dispatcher; devices catch up lazily | Every documented edge is an integer; cheap enough for low-power targets | A7800-SYSTEM; ARCHITECTURE |
| DEC.23 | MARIA DMA runs as one burst per line with per-fetch ticks from the PLA-derived schedule and aborts at the line-RAM swap (no fixed cap); pixel spans render up to now before each MARIA register write | Reproduces the Software Guide and the RTL; per-tick stepping costs 5–10× | A7800-MARIA 4, 10 |
| DEC.24 | HALT stops reads and writes; one `halt_window()` holds the fitted latency; RDY (WSYNC at $24, TIA at $02) holds reads only; the CPU-clock phase per power-up is a profile field | RTL and hardware reports; the fit may change | A7800-SYSTEM |
| DEC.25 | A frame runs from VBLANK start to VBLANK start; input is applied at VBLANK start | Lowest input latency | ARCHITECTURE |
| DEC.26 | Public API `muhtari_*`: opaque instance, size-tagged fixed-width structs, append-only statuses, caller-owned buffers, an error changes no state, separate ABI/behaviour/state/battery versions | Sibling API conventions that held over three milestones | ARCHITECTURE |
| DEC.27 | Video is the native MARIA byte per pixel, 320 × 242 NTSC / 292 PAL, pixel aspect 6:7 (PAL 1.0396); palettes (default warm) and the composite filter are host helpers outside the hash | Hash stability; hosts choose presentation | A7800-MARIA 6–8 |
| DEC.28 | Audio is mono int16 at a rational rate; the primary hash covers the transition stream (tick, source, level); PCM is secondary; synthesis cost is bounded per output sample. 2026-10-20: actual slow POKEY refutes fixed Q23/16x32 quality feasibility; narrowly reopen phase-table density (64 then 128 first), preserve gain/quality contracts and require fresh proofs before selecting coefficients | Mixer-independent determinism; hostile high-rate POKEY | A7800-AUDIO 8; phases/04-sound/04-06-SUMMARY.md; phases/04-sound/04-CONTEXT.md D-08, D-25–D-30 |
| DEC.29 | TIA audio steps on its own 456-tick lattice; TIA writes are not gated by INPTCTRL | Schematic; correct pitch | A7800-AUDIO 2 |
| DEC.30 | POKEY counts the bus's φ2 cycles (stretched by slow cycles, running through HALT), powers on as if SKCTL = $03, and drives IRQ only when the A78 v4 bit asks | Schematic and hardware report; MiSTer behaviour | A7800-AUDIO 4 |
| DEC.31 | Boards dispatch through a `switch` on the board kind (or pointers kept in the instance), never a const pointer table | Under PIC such a table counts as mutable data and fails the no-globals check | ENGINEERING 2 |
| DEC.32 | Controllers are modelled at pin level; devices come from A78 bytes 55/56 or a host override; a lag frame is one with no CPU read of SWCHA, SWCHB or INPT0–5 | Two-button and light-gun behaviour live at pin level | A7800-CARTRIDGES-AND-PERIPHERALS 3; HOSTS-AND-INTEGRATION 5 |
| DEC.33 | Every run call returns within a bounded number of ticks, including jammed CPUs and DMA-off ROMs | Hostile ROMs; frontends must never hang | ARCHITECTURE; ENGINEERING 3 |
| DEC.34 | Per-line DMA statistics are always kept; hooks use a dual-compiled traced loop chosen only when hooks are installed; Ir/frame proves the off path unchanged | Best-in-class observability at zero cost | A7800-MARIA 10; ENGINEERING 8 |
| DEC.35 | Fixed-size state written by one visitor, validated before applying, byte-wise little-endian, ticks capped | Sibling design; deterministic run-ahead and netplay | ARCHITECTURE |
| DEC.36 | Profile fields: RAM fill (zero default, seeded optional), CPU-clock phase, ANE/LXA constant ($00 default; $EE for the public vectors), POKEY power-on, RIOT power-on | Hardware varies; tests need fixed values | CONFORMANCE 1; A7800-SYSTEM |
| DEC.37 | Fast boot sets the documented hand-off (S = X = $16, D = 1, I = 1, INPTCTRL $16, CTRL $60, the hand-off DPP and beam position) and writes no loader into RIOT RAM | No Atari bytes; no known 7800 title reads RIOT RAM after hand-off | A7800-SYSTEM; PROVENANCE 6 |
| DEC.38 | A78 v3 is always parsed and v4 overrides it when it validates; probe order: magic → 7800 hash list → 2600 hash list → footer test → size | Few tools read v4; headerless dumps are common | A7800-CARTRIDGES-AND-PERIPHERALS 1 |

## Testing and conformance
| ID | Decision | Because | See |
|---|---|---|---|
| DEC.39 | SingleStepTests `6502/v1`: the first 100 tests per opcode committed in a compact binary form; the full set nightly at its pin | Exhaustive per-cycle CPU evidence at PR cost | CONFORMANCE 1 |
| DEC.40 | Own diagnostic ROMs measure and report numbers (RAM mailbox, on-screen hex, CRC-16); verdicts live outside the ROM, so the same bytes run on hardware | Expected values are partly fitted; hardware runs need numbers | CONFORMANCE 4 |
| DEC.41 | Oracles: the a7800 v5.2 binary nightly under Xvfb as a locator, never a gate; MiSTer RTL becomes the timing referee in v4.0; real hardware arbitrates | a7800 frames are not VBLANK-aligned; MiSTer is the strongest permissive reference | CONFORMANCE 3; A7800-ECOSYSTEM-AND-USERS 1 |
| DEC.42 | Compatibility ledger CSV → generated `docs/compatibility.md`; passes only ratchet up; the owner's dumps run only in a local preset never registered in `ci` | Measurable compatibility without committing ROMs or self-skipping tests | CONFORMANCE 5 |
| DEC.43 | Exact executed-test inventory, no self-skip, negative controls asserting the specific failure, one registration file per subsystem | Sibling test-gate failures | ENGINEERING 3 |
| DEC.44 | Property tests use a small in-repo seeded harness; fuzz targets start with the A78 probe/load and a bounded random-ROM run | `theft` needs fork/waitpid (no MSVC) | ENGINEERING 3 |

## Engineering and delivery
| ID | Decision | Because | See |
|---|---|---|---|
| DEC.45 | C17 without extensions; warnings fatal only in presets; integer-only, no globals, one symbol namespace, header valid C and C++ — each enforced by a test | Embeddable everywhere | ENGINEERING 2 |
| DEC.46 | Six platforms on every PR plus a 32-bit big-endian PowerPC lane under qemu (per PR if it stays ≤ 5 min, else nightly); armhf nightly; console compile lanes from v6.0 | Identical output on every platform is a product claim; Wii-class data model | ENGINEERING 9 |
| DEC.47 | One required check `CI required`; `push` runs only on `main`; SHA-pinned actions; timeouts twice the slowest measured run with its run ID; no cache until one is measured to pay | Sibling CI evidence | ENGINEERING 4 |
| DEC.48 | Ubuntu 26.04 runners for Linux jobs; clang-format 21 pinned | Current images; one formatter version locally and in CI | ENGINEERING 1, 2 |
| DEC.49 | Releases: release-please `simple` with a GitHub App token, exactly 18 archives, `SHA256SUMS`, verified attestations, version-free asset names, `version.txt` the only machine-edited version, every publish step exercised before 0.1.0, by a published and deleted prerelease rehearsal on the pull request | Sibling release footguns. 2026-10-16: the rehearsal moved from `workflow_dispatch` to `pull_request`, because `workflow_dispatch` fires only for workflow files on the default branch and an untagged draft cannot be published or verified; green rehearsal https://github.com/szTheory/muhtari/actions/runs/38942043999 2026-10-22: retain all five conservative selectors, verified patched gh/full signer/ref, typed checked outputs and exact-ID release-before-tag confirmed cleanup; no implicit permission fallback or reuse. | ENGINEERING 5 |
| DEC.50 | Repository settings as code (squash title and body from the PR), committed ruleset applied in phase 1 once CI reports | GSD's ship step pushes a CI-skip commit | ENGINEERING 6 |
| DEC.51 | Cachegrind instructions per frame is the only per-PR speed gate (+1 %); wall-clock percentiles up to p99.999 over ≥ 300,000 frames are nightly trend data; zero allocations after load is a test | Shared runners make wall-clock tails noise | ENGINEERING 8 |
| DEC.52 | One aggregate `quality.json` ratchet with one check in v2.0; 2026-10-22: defer performance/compatibility domain-policy centralization and retain their legacy stores/judges with explicit lifecycle limitations | Two named constructions exceeded the fixed allocation; preserve semantic guards and budget, without claiming unified exception governance | ENGINEERING 8; .planning/phases/05-pipeline-tune-up/05-CONTEXT.md D-09, D-10, D-21–23 |
| DEC.53 | Docs budgets: user-facing README ≤ 12 KB; maintainer docs in `docs/`; public header ≤ 24 KB; hardware facts graduate to `docs/hardware/` as subsystems land | Sibling README reached 114 KB | ENGINEERING 11 |
| DEC.54 | Each milestone opens with a tune-up phase (CI efficiency, weakest quality dimension, ratchets, freshness, roadmap and coverage refresh) | The owner's recurring audits, automated | SEED-007; ENGINEERING 12 |

## Hosts and integration
| ID | Decision | Because | See |
|---|---|---|---|
| DEC.55 | libretro: ProSystem-compatible RetroPad layout and BIOS names, optional firmware, few core options, rcheevos memory descriptors tested with rcheevos' own resolver | Remaps and achievements carry over | HOSTS-AND-INTEGRATION 1, 2 |
| DEC.56 | Real-RetroArch end-to-end stays two hosted cases (screenshot compare in v1.0, save round trip in v2.0), ≤ 300 test lines; semantics are proven in-process | A sibling's RetroArch e2e grew to 7,706 lines and stalled a milestone | HOSTS-AND-INTEGRATION 3 |
| DEC.57 | Runner exit statuses 0, 1, 2, 3, 4, 5, 77 in one table; `--probe` prints headered and headerless hashes incl. the RetroAchievements MD5; Playstead's continuation protocol lands in v3.0 | Playstead's adapter contract; headered and headerless dumps both exist | HOSTS-AND-INTEGRATION 4 |
| DEC.58 | Scripting order: inspection (v1.0–v3.0) → hooks, Python bindings and a vectorised Gymnasium environment (v5.0) → Lua on demand; an own text movie format with BK2 import later | YAGNI order by demand | HOSTS-AND-INTEGRATION 5 |
| DEC.59 | ROM patching stays in the host (RetroArch soft-patching); `muhtari-run --patch` (IPS/BPS, fuzzed) comes with the v5.0 tools | Core stays minimal | A7800-CARTRIDGES-AND-PERIPHERALS 6 |
| DEC.60 | HSC: the user's ROM by hash, a signature-only stub until a clean-room OpenHSC; SaveKey raw 32 KB; `.srm` = HSC ‖ SaveKey, versioned (v2.0) | No Atari firmware; interop with other emulators | A7800-CARTRIDGES-AND-PERIPHERALS 4 |

## Open questions (settled by tests or measurements in the phase named)
- RIOT port B power-on value: A7800-SYSTEM recommends ORB = $FF (One on One, Worm! hardware reports). Note 2026-10-15: the sources support $FF (MAMETesters 06060: hardware outputs high, and an ORB = 0 default broke One On One), so A7800-CARTRIDGES-AND-PERIPHERALS 3.3 was corrected from "ORB = 0". A profile field; the local predicates for those two titles confirm the default (v1.0 phase 3).
- The $2800 mirror: undecoded on the CPU bus (Summer Games) versus the rcheevos descriptor that maps it (phase 3).
- 242 vs 243 visible rows and the first-pixel tick (phase 3, diagnostics; hardware capture in v4.0).
- HALT latency and the DLI recognition tick: fitted now; one owner-console diagnostic settles them (v4.0).
- TIA : POKEY level ratio (resistor model 2.1× vs MiSTer 1.0×) and POKEY power-on state (phase 4).
- XG-1 sensor latency and brightness threshold (v2.0).
- MiSTer pin for holey DMA: the pinned commit has a regression; use the earlier commit as the oracle (v4.0).
