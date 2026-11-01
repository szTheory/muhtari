---
title: "Preparation index"
summary: "Which preparation file answers which question, and the conventions every file in this directory shares."
read_when: "First, before opening any other file in this directory; again when adding, renaming or retiring one."
updated: 2026-10-29
---

# Preparation index

Reference material gathered before muhtari's first line of code: Atari 7800 hardware facts with sources, the test
and oracle plan, engineering, host and process decisions, and how GSD is run. It is the project's searchable second
brain: each file opens with frontmatter saying when to read it and a Where to look table, cites tiered, pinned
sources, and gives one recommendation per choice. Open the file a question points to; none needs reading end to end.

Start with the [planning memory index](../README.md) for current scope and the
[shared intent](../INTENT.md) for gamer priorities, research posture and sibling reuse.

## Files
| File | Holds | Read when |
|---|---|---|
| [../README.md](../README.md) and [../INTENT.md](../INTENT.md) | Discoverable memory index and dated owner intent with sources, transfer rules and research posture | Before milestone work or adapting muhtari practices to a sibling core |
| [ARCHITECTURE.md](ARCHITECTURE.md) | The core's design spine: tick time base and frame boundary, chip modules and the bus, the public C API, state, observability | Before designing a core subsystem, the public header, the libretro adapter or the runner |
| [DECISIONS.md](DECISIONS.md) | One row per settled choice (`DEC.NN`) with its reason and supporting section; open questions at the end | Before reopening a choice, or when a phase needs to know what is settled |
| [A7800-SYSTEM.md](A7800-SYSTEM.md) | Clocks and the master tick, the per-line timeline, SALLY HALT/RDY/NMI, memory map, INPTCTRL, RIOT, boot and HLE, power-on profile | Changing the time base, bus, CPU wrapper, RIOT, boot path or any timing that crosses chips |
| [A7800-MARIA.md](A7800-MARIA.md) | Display lists, read modes, line RAM, the per-fetch DMA schedule, register effects, frame geometry, palettes, artifacting | Writing or changing `maria.c`, DMA, MSTAT/DLI timing or the frame format, or when a picture diverges |
| [A7800-AUDIO.md](A7800-AUDIO.md) | TIA audio, the board mix, cartridge POKEY and its seed, XM/YM2151, integer synthesis and audio hashes | Building TIA audio, POKEY, the synthesiser, the mixer or audio tests |
| [A7800-CARTRIDGES-AND-PERIPHERALS.md](A7800-CARTRIDGES-AND-PERIPHERALS.md) | A78 header v1–v4, every board with its library share, controller pin map, HSC/SaveKey saves, ROM hacks, hostile-input bounds | Changing the loader or probe, a board, ports, saves, patch handling or loader fuzz targets |
| [A7800-ECOSYSTEM-AND-USERS.md](A7800-ECOSYSTEM-AND-USERS.md) | Every 7800 emulator, user complaints mapped to responses, personas by horizon, low-power budgets, the slot muhtari fills | Ranking features or horizons, choosing an oracle, planning a port, answering "why not emulator X" |
| [CONFORMANCE.md](CONFORMANCE.md) | Test ROMs and their licences, oracles, own diagnostics, the compatibility ledger and ratchet, CI tiers and cost | Admitting a fixture, writing diagnostics or the result protocol, building the ledger or a CI tier |
| [HOSTS-AND-INTEGRATION.md](HOSTS-AND-INTEGRATION.md) | libretro contract, RetroAchievements map, RetroArch on Apple Silicon, `muhtari-run` and Playstead, scripting, WASM | Building the adapter or `.info`, the runner CLI, saves, the RetroArch e2e job or a scripting surface |
| [ENGINEERING.md](ENGINEERING.md) | C rules, presets, tests, fuzzing, CI, release, repository settings, triage, performance ratchet, portability, style, docs, recurring loops, secrets | Creating or changing the build, tests, workflows, repository settings, the release pipeline or quality gates |
| [GSD-HANDOFF.md](GSD-HANDOFF.md) | The kickoff text, per-phase and milestone routines, settings and why, what stops GSD running on, owner tasks | Before the kickoff, before each GSD step, and when a step behaves unexpectedly |
| [PROMPT-COVERAGE.md](PROMPT-COVERAGE.md) | Every owner ask mapped to its home with status and what remains; owner constraints vs preparation's choices | At kickoff review and at every milestone close |
| [PROVENANCE.md](../../PROVENANCE.md) | Which code, tables and facts may enter; the upstream licence audit at each pin; strangler-fig seeds; notices | Before reusing or citing any outside code or data |
| [ASSET_POLICY.md](../../ASSET_POLICY.md) | Which bytes may be committed, built in CI or kept local; firmware digests; how it is checked | Before adding a ROM, BIOS, save, state, screenshot or fixture |
| [LESSONS.md](../LESSONS.md) | Sibling lessons imported as hypotheses with their local disposition; the export template | At phase and milestone boundaries, or when the owner asks to export lessons |
| [PRODUCT-ROADMAP.md](../PRODUCT-ROADMAP.md) | Immediate / Near / Mid / Long horizons with evidence, triggers and a decision log | At every milestone step, or when a finding changes priorities |
| [seeds/](../seeds/) | SEED-001–006 and SEED-008 (the browser) hold later horizons; SEED-007 is the milestone tune-up; SEED-009 carries proportional maintenance review within that tune-up | When a seed's trigger fires, and at every new milestone |
| [ATARIAGE-EXTRACTS.md](ATARIAGE-EXTRACTS.md) | Paraphrased AtariAge forum facts (tier T4, many measured on hardware) with handle, date and evidence type; harvested with a browser because scripted fetches are blocked | Before relying on an AtariAge claim cited in another file |

## Where to look
| Question | File and section |
|---|---|
| Where are shared product goals, owner intent and reusable research guidance? | [Planning index](../README.md), [INTENT](../INTENT.md), [fan-out prompt](../research/fan-out/prompt.txt) |
| What is one tick, what happens at each tick of a line, and when does HALT stop SALLY? | A7800-SYSTEM 1–3 |
| What answers each address, which cycles are slow, and what does INPTCTRL do? | A7800-SYSTEM 4, 5 |
| What do the BIOSes do, what must fast boot reproduce, which power-on facts are profile fields? | A7800-SYSTEM 7, 8 |
| How are display lists and line RAM decoded, and when does each DMA fetch happen? | A7800-MARIA 2–4 |
| Visible rows, aspect ratio, CRT vs LCD, palettes, composite artifacts? | A7800-MARIA 6–8 |
| How do TIA audio, the board mix and cartridge POKEY work, and what seeds our POKEY? | A7800-AUDIO 2–5 |
| How are samples synthesised and hashed, and how is A/V sync kept? | A7800-AUDIO 8 |
| What is at each A78 header offset; how are headerless, bad or 2600 images told apart? | A7800-CARTRIDGES-AND-PERIPHERALS 1 |
| Which boards exist, how much of the library each covers, which milestone gets it? | A7800-CARTRIDGES-AND-PERIPHERALS 2 |
| How do controllers map to pins, and how do HSC and SaveKey saves work? | A7800-CARTRIDGES-AND-PERIPHERALS 3, 4 |
| Which parsers face hostile input, and with which bounds? | A7800-CARTRIDGES-AND-PERIPHERALS 7; ENGINEERING 3 |
| Which 7800 emulators exist, and what do users dislike or wish for? | A7800-ECOSYSTEM-AND-USERS 1, 2 |
| What do speedrunners, TASers, AI agents and frontends need; can low-power devices keep up? | A7800-ECOSYSTEM-AND-USERS 3, 4 |
| Which test ROMs may be used, and may this fixture enter the repository? | CONFORMANCE 1, 7; ASSET_POLICY 1–3 |
| Which oracle and diagnostics come first; how do the ledger and the ratchet work? | CONFORMANCE 3–5; ENGINEERING 8 |
| What runs per PR and nightly, and what does it cost? | CONFORMANCE 6; ENGINEERING 4 |
| Which flags, presets, tests and style rules apply? How is a release made or withdrawn? | ENGINEERING 2, 3, 10; ENGINEERING 5 |
| What must the libretro core export, and how does a release reach RetroArch on a Mac? | HOSTS-AND-INTEGRATION 1, 3 |
| What are the runner's options, exit statuses and Playstead mapping; in what order does scripting arrive? | HOSTS-AND-INTEGRATION 4, 5 |
| May this code or table be reused; how do I test with my own dumps? | PROVENANCE 1–5; ASSET_POLICY 4 |
| What is already decided, and how is GSD started and run each phase? | DECISIONS; GSD-HANDOFF 2, 3 |
| Repeated workflow retries, proxy budgets, metadata repair or specialist fan-out? | GSD-HANDOFF 3.1; [recovery decision](../research/workflow-recovery/RECOMMENDATION.md) and [prompt](../research/workflow-recovery/prompt.txt) |
| Does every owner ask have a home? | PROMPT-COVERAGE 1 |

## Conventions
- **Frontmatter.** Every file here starts with `title`, `summary`, `read_when` and `updated` (ISO date), then a Where
  to look table and numbered sections. Seeds use GSD's fields (`id`, `status`, `planted`, `trigger_when`, `scope`).
- **Citations.** `[SYS.07]` resolves in the Sources table (`ID | Tier | Source | Pin | Supports`) at the end of the
  same file: SYS A7800-SYSTEM, MAR A7800-MARIA, AUD A7800-AUDIO, CART A7800-CARTRIDGES-AND-PERIPHERALS, ECO
  A7800-ECOSYSTEM-AND-USERS, CF CONFORMANCE, HI HOSTS-AND-INTEGRATION, ENG ENGINEERING, ARCH ARCHITECTURE, GH
  GSD-HANDOFF, PV `PROVENANCE.md`, AP `ASSET_POLICY.md`. `DEC.NN` is a row of DECISIONS, which names the section
  behind it. ECOSYSTEM 2 numbers complaints `C.NN` (7800 emulators) and `G.NN` (emulators in general).
- **Tiers.** T1 primary: official Atari/GCC documents, datasheets, schematics, die or RTL analysis, measurements made
  here, a project's own repository for what that project does. T2 curated community reference (7800.8bitdev.org
  wiki, AtariAge FAQs). T3 one implementation's behaviour as evidence of hardware (emulator source, FPGA RTL). T4
  forum posts and issue trackers. A tier describes the source for the claim it supports.
- **Pins.** A 12-character commit SHA, a release tag, a wiki revision number, or "accessed YYYY-MM-DD". Siblings are
  cited by name and commit; the unpublished Genesis sibling is described, not named.
- **Markers.** (tested) was run here; (untested) was not; (derived) is inferred from cited facts; (unverified) comes
  from recall or was not fetched.
- **Choices.** Each gives a **Recommendation**, the **Strongest alternative**, and **Would change if**: the
  observation that would flip it.
- **IDs** use a dot (`SYS.07`, `DEC.07`), so they never collide with GSD's own `D-NN` phase decisions.
- **Licence and scope.** Facts in our own words with a citation; GPL, LGPL and unlicensed emulator source is never
  opened (PROVENANCE 1, 3). 7800 only: another console appears only as labelled precedent, never lending timing.

## Keeping it current
- A phase that finds one of these files wrong or stale fixes it and its `updated` date in the same pull request.
- When a subsystem lands, its hardware facts graduate into public `docs/hardware/<chip>.md`, which code comments
  cite (ENGINEERING 10, 11), and the preparation file's section shrinks to a pointer there.
- Research for a single phase lives with that phase under `.planning/phases/`; this directory keeps only
  cross-phase reference. A new, renamed or retired file updates both tables here; PROMPT-COVERAGE and
  PRODUCT-ROADMAP are refreshed at each milestone close (GSD-HANDOFF 4).
