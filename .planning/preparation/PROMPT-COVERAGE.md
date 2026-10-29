---
title: "Prompt coverage: every owner ask and where it lives"
summary: "Maps each of the owner's 53 asks (the preparation brief, all layers, plus later messages) to the file section, rule, seed or milestone that carries it, with a status and what remains."
read_when: "At kickoff review and at every milestone close; also before calling an owner ask done, deferred or out of scope."
updated: 2026-10-21
---

# Prompt coverage

A roadmap that maps all of its own requirements proves nothing about the brief those requirements came from. This
file checks the brief itself. Asks are short paraphrases, numbered in the order the owner gave them; the brief is
not committed.

| Question | Section |
|---|---|
| Where does owner ask N live, and what is left to do? | 1 |
| Which asks are only partly placed? | 1 (status Partial: 31, 51, 52) |
| What did the owner require, and what did preparation choose? | 2 |
| How is this file kept true? | 3 |

## 1. Coverage

**Status:** Documented = decided and placed in v1.0 or in a standing doc rule; Partial = has a home but something
is missing (named); Later = placed in a seed or horizon; Inapplicable = does not apply (reason given).
**Homes** are file stems with section numbers: SYSTEM, MARIA, AUDIO, CARTRIDGES (A7800-CARTRIDGES-AND-PERIPHERALS),
ECOSYSTEM (A7800-ECOSYSTEM-AND-USERS), CONFORMANCE, ENGINEERING, HOSTS (HOSTS-AND-INTEGRATION), GSD-HANDOFF,
README (this directory's index); ARCHITECTURE and DECISIONS without numbers; AGENTS rule N, PROVENANCE, ASSET_POLICY
at the root; LESSONS, PRODUCT-ROADMAP, SEED-NNN under `.planning/`. "v1.0 ph2" = milestone v1.0, phase 2.

| # | Owner ask | Home | Status | Remaining / when |
|---|---|---|---|---|
| 1 | An Atari 7800 emulator named muhtari, not another console | AGENTS preamble; SYSTEM; CARTRIDGES 1.5 (2600 images refused as "planned"); LESSONS protocol (no cross-console timing claims) | Documented | 2600 mode after the 7800 library (SEED-006, owner decision) |
| 2 | Learn from gabbaboy, glueyneo, nesturbator and the unpublished Genesis sibling, especially the fastest | LESSONS XFER-001–024; PROVENANCE 4 (nesturbator seeds CPU, bus, synthesiser, state, runner, adapter, CI); DECISIONS (pre-seeded bootstrap, the fastest sibling's path); GSD-HANDOFF 1 | Documented | Imports stay hypotheses until reproduced (AGENTS 11); each phase appends `MU-` lessons |
| 3 | Playstead's lessons, incl. high-quality homebrew usable directly in CI | CONFORMANCE 1, 2, 4, 7 (own MIT diagnostics; 7800basic samples and OpenBIOS built in CI from pins, only hashes committed); ASSET_POLICY 2–3; HOSTS 4 (Playstead pin and continuation protocol) | Documented | LESSONS XFER-025 now records it. Its phase 03-01 (playstead `edea152`) replaced licence-ambiguous freeware homebrew with a self-authored MIT test ROM, the same answer as CONFORMANCE 4: record it as an XFER row. No permissively licensed 7800 homebrew *game* is admitted yet (unlicensed ones stay local); CONFORMANCE 7 admits one when found |
| 4 | The DNA of the owner's mature OSS projects (CI/CD, release, docs) | ENGINEERING 5 (release-please, App token, attested archives), 7 (lattice_stripe issue forms), 11 (lattice_stripe README order, docs ladder, budgets); LESSONS XFER-008, -021, -022 | Documented | — |
| 5 | A reusable core with clean video, audio and input interfaces that other frontends, Playstead included, can use | ARCHITECTURE (public C API); AGENTS 2, 4; DECISIONS (three deliverables, native MARIA pixels); HOSTS 1, 4, 7; v1.0 ph1 (test card through libretro) | Documented | Playstead pin fields v2.0; continuation protocol v3.0 (SEED-002); integration v6.0 (SEED-005) |
| 6 | Incredibly fast | ENGINEERING 8 (Cachegrind Ir/frame gate per PR, no allocation after load); ECOSYSTEM 1.1, 4 (burst/lazy design, 1.8–4.3 M Ir/frame); MARIA 10; ARCHITECTURE | Documented | Baseline measured in v1.0; ceilings tightened at each tune-up (SEED-007) |
| 7 | Correct, and fixing what people dislike about other 7800 emulators | ECOSYSTEM 1.2 (wiki problem games), 2 (C.01–C.27, each with a response and milestone); CONFORMANCE 3 (oracles at VBLANK), 4 (own diagnostics); MARIA 9; SYSTEM 9; v1.0 ph2–4 | Documented | Hardware-measured accuracy and an FPGA-RTL oracle v4.0 (SEED-003) |
| 8 | Best-practice C, CMake and CI/CD; fan-out research through every stakeholder lens with an adversarial pass | ENGINEERING 2 (flags, presets, analyzers), 4 (CI); ECOSYSTEM 3 (personas); every choice names its strongest alternative and flip condition; MARIA 10 (corrections to the design spine); ECOSYSTEM 5 (strongest reason this is wrong); GSD-HANDOFF 3 (each discuss advisor gets the owner's fan-out brief, and its recommendations are followed) | Documented | GSD-HANDOFF 8 records the adversarial pass: the fact-check (58 claims re-fetched, 5 corrected), the fresh-eyes review, two roadmapper dry runs and the hygiene tests |
| 9 | Code that is a joy to read, educational and self-documenting; KISS/YAGNI per ponytail.dev | AGENTS Style; ENGINEERING 10 (names, sizes, comments cite sources, the ladder), 7 (YAGNI list); HOSTS 5 (YAGNI order) | Documented | Size and comment rules are enforced by review (ENGINEERING 10) |
| 10 | Performance, compatibility and correctness baselines across all games, always ratcheting up | CONFORMANCE 5 (ledger, predicates, ratchet, local-only preset); ENGINEERING 8 (Ir/frame gate, `quality.json` contract); DECISIONS (passes only ratchet up); v1.0 ph3–4 (frame and audio hashes on six platforms) | Documented | `quality.json` is first built at v2.0's tune-up (SEED-007); the whole-library ledger is v2.0 (SEED-001) |
| 11 | Hardware documentation with provenance, primary sources preferred, kept in `.planning` as a RAG-discoverable second brain | SYSTEM, MARIA, AUDIO, CARTRIDGES (tiered, pinned Sources tables); README (frontmatter `read_when`, Where to look); PROVENANCE 1, 6–7; ENGINEERING 11 (`docs/hardware/<chip>.md`) | Documented | Facts graduate to public `docs/hardware/` as each subsystem lands (README, "Keeping it current") |
| 12 | Maximum development velocity | GSD-HANDOFF 1 (kickoff resumes at roadmap creation), 3; AGENTS 7 (proportional gates, ≤ 2 review rounds); ENGINEERING 11 (gate code ≤ 10 % of `src/`); LESSONS XFER-001, -006, -007 | Documented | — |
| 13 | Optional telemetry and observability with no overhead | ARCHITECTURE (observability); DECISIONS (per-line DMA statistics; traced loop compiled twice, chosen only with hooks); ENGINEERING 8 (off path proven by Ir/frame); HOSTS 5, 7 (inspection v1–v3; lag counter, line stats, trace at zero cost when off) | Documented | Hook APIs, debugger views and traces with symbols v5.0 (SEED-004) |
| 14 | Great day-0/1/2 developer experience and operations; easy adoption | ENGINEERING 11 (README install one-liners and quick starts; `docs/maintaining.md` runbooks), 5 (rehearsal, withdrawal), 6 (settings as code), 13; HOSTS 3; GSD-HANDOFF 2, 7 | Documented | — |
| 15 | Property-based tests and fuzzing; safe with hostile ROMs | ENGINEERING 3 (property tests, fuzz targets); CARTRIDGES 7 (hostile-input list, parser bounds); AGENTS 4 (bounded calls, even for a jammed ROM) | Documented | — |
| 16 | Fast CI and builds | ENGINEERING 4 (triggers, timeouts, caching, cores, minutes per PR); CONFORMANCE 6 (tiers and cost); LESSONS XFER-012, -021; v1.0 ph1 | Documented | Timeouts re-derived from measured runs at each tune-up (SEED-007) |
| 17 | Ship early and often with automated releases | ENGINEERING 5; PRODUCT-ROADMAP (every phase ends in a release); GSD-HANDOFF 3 steps 6–8; LESSONS XFER-002; v1.0 ph1 (whole pipeline, ROM-free test card) | Documented | — |
| 18 | Always PRs, `main` green, issue and PR triage, auto-merge and auto-release on green CI without the owner | AGENTS 9; ENGINEERING 5 (App token), 6 (ruleset, branch policy), 7 (issue forms, labels, Dependabot auto-merge, rolling nightly issue); GSD-HANDOFF 3; `scripts/apply-repo-settings.sh` | Documented | Weekly triage digest built at v2.0's tune-up (SEED-007); release App created 2026-10-15 (GSD-HANDOFF 7) |
| 19 | 12-factor secrets: local paths passed inline, GitHub secrets in CI (superseded by Phase 11 on 2026-10-29; earlier: a dotenv file locally) | ENGINEERING 13; ASSET_POLICY 4 | Documented | — |
| 20 | A rolling near/mid/long roadmap revised every milestone | PRODUCT-ROADMAP (refresh rule, horizons, decision log); GSD-HANDOFF 4; SEED-001–006; SEED-007 (Direction) | Documented | — |
| 21 | Performance stress tests incl. p99.999 tails, throughput and memory, folded into CI efficiently | ENGINEERING 8 (per-PR Ir/frame; zero allocations after load on six legs; nightly `bench`: p50–p99.999 and max µs/frame, fps, max RSS, deadline misses over ≥ 300,000 frames); DECISIONS | Documented | Tails stay nightly trend data, never a PR gate (shared runners); tracking against budgets v4.0 (SEED-003) |
| 22 | Recurring CI/CD efficiency audits in the project's DNA | ENGINEERING 12 (per-job p50/p95, queue time, flake rate, runner minutes); SEED-007 (first phase of every milestone after v1.0); DECISIONS | Documented | First run opens v2.0, which also builds the monthly CI-efficiency report |
| 23 | Recurring software-quality "weakest dimension" reviews | ENGINEERING 12 (score 1–5: performance, CI/CD, ease of use, architecture, docs/README, upgrade path); SEED-007 | Documented | First scoring at v2.0's tune-up; the lowest score sets that phase's main change |
| 24 | Shift-left, automation-first verification, recorded so it is never re-litigated | AGENTS 6, 10; GSD-HANDOFF 3 ("Verification default", settled 2026-10-15); LESSONS XFER-004, -011; DECISIONS | Documented | — |
| 25 | Stop after each GSD phase so the owner can review and switch models | AGENTS 8 (one GSD step per session, stricter); GSD-HANDOFF 3, 5 (`model_profile: adaptive`), 6; `.planning/config.json` (interactive, chain flags off, checked by `scripts/hygiene.sh`) | Documented | — |
| 26 | Releases the owner can try on a Mac through some frontend while Playstead is not ready | HOSTS 3 (RetroArch Metal on Apple Silicon, hosted e2e); v1.0 ph1 (test card in RetroArch); GSD-HANDOFF 3 step 9, 7; ENGINEERING 11 (install line) | Documented | — |
| 27 | The owner's physical carts (legal dumps) usable for testing | ASSET_POLICY 4 (`MUHTARI_ROM_DIR`, firmware digests); CONFORMANCE 5 (`MUHTARI_ROM_DIR=<dump dir> [MUHTARI_BIOS=<file>] cmake --workflow --preset library`, never in `ci`; superseded by Phase 11 on 2026-10-29); ENGINEERING 13 | Documented | — |
| 28 | Speedrunning | ECOSYSTEM 3.2, 2 (C.21 fairness); DECISIONS (exact NTSC/PAL rates; nothing on by default changes speed); HOSTS 5 (lag frames) | Later | v1.0 gives exact timing and determinism; movies with recording v3.0 (SEED-002); speedrun aids v5.0 (SEED-004) |
| 29 | Machine- or AI-assisted speedruns (TAS) | ECOSYSTEM 3.3; HOSTS 5 (movie format, lag frames, size-tagged input) | Later | Movies v3.0 (SEED-002); TAS aids and Lua v5.0 (SEED-004). |
| 30 | AI playthroughs | ECOSYSTEM 3.4 (ALE-like interface, from ALE docs only); HOSTS 5 (Python ctypes, Gymnasium env) | Later | v5.0 (SEED-004) |
| 31 | Portability to low-power devices (DS/PSP class) | ECOSYSTEM 4 (device budgets); ENGINEERING 9 (`be32` per PR; `armhf` and console compile-only lanes nightly); SEED-005 | Partial | Preparation declines DS and DSi (A7800DS serves them) and makes PSP and Old 3DS "measure, then decide"; reopens if v1.0 measures ≤ ~2 M Ir/frame (ECOSYSTEM 4). Ports v6.0 (SEED-005) |
| 32 | A consistent frame rate, no stutter, low latency | DECISIONS (a frame runs VBLANK to VBLANK, input applied at VBLANK start; exact rational rates); ECOSYSTEM 2 (G.01, G.02, C.19); HOSTS 3 (low-latency settings); ENGINEERING 8 (deadline misses); AGENTS 4 | Documented | Run-ahead v3.0 (SEED-002) |
| 33 | Interface design with good integration ergonomics | ARCHITECTURE (public C API); ENGINEERING 10 (`muhtari_status`; an error changes no state), 11 (terse header ≤ 24 KB, `docs/embedding.md`); HOSTS 1, 4 (exit-status table), 7 (feature → host or core) | Documented | — |
| 34 | All regions | SYSTEM 1, 7 (NTSC and PAL clocks and BIOS paths); MARIA 6 (242 / 292 rows); DECISIONS (exact NTSC and PAL rates; region from header or the CC0 hash list) | Documented | PAL-01–03, Phase 9 of approved v2.0 |
| 35 | Display and monitor types | MARIA 6 (aspect, overscan, scaling, CRT vs LCD), 7 (palettes), 8 (composite artifacting); HOSTS 7 (palettes and filter as host helpers; CRT shaders in RetroArch); ECOSYSTEM 2 (C.11–C.13) | Documented | Composite artifact filter v4.0 (SEED-003) |
| 36 | Maximum cartridge/mapper compatibility, ratcheted quickly | CARTRIDGES 2.1–2.3 (every board, library share, milestone); PRODUCT-ROADMAP (v1.0 ph3 187 of 204 titles, ph4 195 of 204); CONFORMANCE 5 | Documented | BOARD-01–05, Phases 6 and 10 of approved v2.0; Souper/BupChip and later audio expansions remain deferred |
| 37 | Saves | CARTRIDGES 4 (HSC, SaveKey/AtariVox, file formats); HOSTS 1 (save RAM, `.srm`), 4 (save dir, worst-case loss) | Documented | SAVE-01–06, Phase 7 of approved v2.0; save states and rewind remain v3.0 (SEED-002) |
| 38 | Friendly to ROM hackers and modified games | CARTRIDGES 1.4, 6 (hack identity, IPS/BPS, BIOS signature); ECOSYSTEM 2 (C.02: header first, no per-game hacks); DECISIONS (patching stays in the host); HOSTS 7 (RetroArch soft-patching) | Documented | `muhtari-run --patch` and homebrew lint v5.0 (SEED-004) |
| 39 | A public GitHub repository with no PII | AGENTS 3; `scripts/hygiene.sh` and `.githooks` (every commit and push; pre-push scans the whole history); ASSET_POLICY 1, 6 | Documented | The owner consents to creating the public repository (GSD-HANDOFF 7) |
| 40 | MIT or Apache, whichever makes sense | `LICENSE` (MIT); DECISIONS (MIT: fits every frontend's licence, same as the siblings); PROVENANCE 1, 5 | Documented | — |
| 41 | Research what users like and dislike about emulators in general | ECOSYSTEM 2 (G.01–G.09 general complaints; "Praise to keep") | Documented | — |
| 42 | Strangler-fig reuse of existing code, then our own as needed | PROVENANCE 4 (seed per module, replacement trigger, parity proof); AGENTS 1; DECISIONS (seeds; own MARIA, TIA, RIOT, POKEY) | Documented | — |
| 43 | Best-in-class extras: performance, observability, automation, scripting | ENGINEERING 8; HOSTS 4 (runner hashes, exit statuses), 5 (YAGNI order: inspection, hooks, Python, Gym, Lua); ECOSYSTEM 5 (the slot muhtari fills) | Documented | Scripting and hook APIs v5.0 (SEED-004) |
| 44 | Research official docs, fan docs and other emulators' source | Sources tables of SYSTEM, MARIA, AUDIO, CARTRIDGES, ECOSYSTEM, CONFORMANCE (T1–T4, pinned); PROVENANCE 2 (licence audit at the pin), 3 (GPL and unlicensed: docs, trackers, binaries only) | Documented | Open questions close in the phase that needs them (each file's last section) |
| 45 | Clean architecture | ARCHITECTURE; AGENTS 2, 4, Style; ENGINEERING 10 (one module per chip); DECISIONS | Documented | — |
| 46 | Compile the research into `.md` files and give a kickoff command for a fresh context | README; GSD-HANDOFF 2 (the kickoff message for a new session) | Documented | The pre-seeded `PROJECT.md`, `REQUIREMENTS.md`, `research/SUMMARY.md` and `STACK.md` (GSD-HANDOFF 1) must exist before the kickoff |
| 47 | A lessons exchange with the sibling projects (periodic pasteable lessons) | LESSONS (protocol, template, XFER ledger); AGENTS 11; GSD-HANDOFF 4 (offered at each milestone boundary); SEED-007 | Documented | — |
| 48 | Use subagents for context efficiency | GSD-HANDOFF 3 (discuss spawns advisor researchers), 5 (research, plan-check, verifier and code-review agents on; `model_profile: adaptive`); preparation itself ran as parallel research agents | Documented | — |
| 49 | Usable as a core inside other emulators and frontends | HOSTS 1 (libretro), 2 (rcheevos map), 4 (runner process contract); ARCHITECTURE; AGENTS 2, 4; ENGINEERING 11 (`docs/embedding.md`) | Documented | The browser build (SEED-008); Playstead integration v6.0 (SEED-005) |
| 50 | Use the new OpenGSD (`gsd-core`), not the old GSD | GSD-HANDOFF 1, 2 (`@opengsd/gsd-core` 1.17.0 version check), 5, 8; LESSONS XFER-023, -024 | Documented | — |
| 51 | Power-efficient, for battery life on handhelds | ECOSYSTEM 4 (work per frame, device budgets); FRAME-08 (instructions-per-frame ratchet), FRAME-09 (bench); ENGINEERING 8, 9 (`armhf` nightly); PROJECT constraints (efficiency); SEED-005 | Partial | Work per frame is measured, with comparisons still report-only; TUNE-04 handles arming controls in Phase 5. Energy is not measured; battery budgets on a handheld come with the ports (SEED-005) |
| 52 | Efficient in the browser through WebAssembly; the owner ranks it very important | HOSTS 6; ARCHITECTURE (portability row); PROJECT constraints (efficiency); v1.0 ph1 (a wasm32 build and hash-equality lane on every PR, decided at the phase 1 discussion); SEED-008 | Partial | The WASM build/hash lane remains advisory. The owner considered and deferred the browser player at the v2.0 boundary on 2026-10-21 (SEED-008); TUNE-04 makes lane status explicit |
| 53 | Another copy and paste beats another dependency: small, flat dependency trees, a dependency only when truly needed | AGENTS Style; PROVENANCE 4 (permissive seeds with their notices); PROJECT out of scope (no test framework dependency); ENGINEERING 7 (YAGNI list) | Documented | — |

Totals: Documented 47, Partial 3, Later 3, Inapplicable 0.

## 2. Owner constraints vs agent recommendations

**The owner required** (change only with the owner):
- Atari 7800 only, named muhtari; a public repository with no PII; MIT (the owner allowed MIT or Apache and chose MIT).
- GPL, LGPL and unlicensed emulator source is never opened; such emulators run only as binaries. Reuse is
  strangler-fig and permissive only; KISS/YAGNI by the ponytail.dev ladder.
- No GUI in the core repository; RetroArch is the Mac test frontend until Playstead is ready.
- Atari 2600 mode only after the 7800 library is complete (decided 2026-10-15).
- OpenGSD; stop after every phase; PRs only, auto-merge and auto-release on green CI; automation-first
  verification, never re-litigated; subagents for context.
- 12-factor secrets; a rolling roadmap; recurring CI-efficiency and weakest-dimension reviews; lesson exchange.

**Preparation chose** (each choice names its strongest alternative and flip condition where it is cited; reopen
only with new evidence, recorded in DECISIONS):
- The deliverable shape: library, `muhtari-run` and libretro adapter (DECISIONS).
- Horizon order: players and homebrew developers first; saves, full PAL and the light gun v2.0; states and movies
  v3.0; measured accuracy v4.0; tools, scripting and AI v5.0; ports v6.0 (DECISIONS; PRODUCT-ROADMAP). This is why
  asks 28–30, 34 and 37 are Later.
- Fast boot without a BIOS by default; no Atari BIOS bytes ever; the CC0 OpenBIOS is built in CI (DECISIONS).
- One timing design (burst/lazy MARIA), no DS-class fast profile, PSP measured before it is promised (ECOSYSTEM
  4). This narrows ask 31.
- Cachegrind Ir/frame is the only per-PR speed gate; wall-clock tails to p99.999 are nightly trends (ENGINEERING
  8). This is how ask 21 is "folded into CI".
- Tune-ups once per milestone, not per phase; `quality.json`, the weekly digest and the monthly CI report are first
  built at v2.0's tune-up (ENGINEERING 12; SEED-007). This is how asks 22 and 23 recur.
- Python before Lua; patches, palettes, the composite filter, cheats, rewind and run-ahead live in the host or in
  host helpers outside the hashes (HOSTS 5, 7; DECISIONS).
- GSD runs one step per session (stricter than the owner's per-phase stop), sequential plans, no worktrees,
  coarse granularity, inherited model (AGENTS 8; GSD-HANDOFF 5).

## 3. Keeping this file true

- Refresh at kickoff review and at each milestone close (GSD-HANDOFF 4; SEED-007): open each home, confirm it
  still says what the row claims, update Status and Remaining, and bump `updated`.
- A new owner ask takes the next number; rows are never renumbered or deleted. An ask whose home disappears
  becomes Partial with the gap named.
- A row moves from Later to Documented only when its seed is promoted into a milestone with requirements.
- Requirement coverage in `REQUIREMENTS.md` does not replace this check: it maps requirements to phases, not the
  brief to requirements.
