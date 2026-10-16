---
title: "Atari 7800 emulators, users and personas"
summary: "Every 7800 emulator's licence, timing model, record on the wiki's known-problem list, features and status; what players and developers complain about, mapped to muhtari responses; persona needs by horizon; low-power budgets; the slot muhtari fills."
read_when: "Before ranking features or horizons, choosing an accuracy oracle, writing persona or frontend requirements, planning a port, or answering 'why not emulator X'."
updated: 2026-10-15
---

# Atari 7800 emulators, users and personas

| Question | Section |
|---|---|
| Which 7800 emulators exist: licence, timing model, features, status, speed? | 1.1 |
| Which handle the wiki's known-problem games, and which is the accuracy reference? | 1.2, 1.3 |
| What do players and developers dislike or wish for, and what does muhtari do about each? | 2 |
| What does each persona need, by horizon? | 3.1 |
| Speedrun and TAS rules, movie format, lag frames, an ALE-like AI interface, frontends | 3.2–3.5 |
| Can a cycle-accurate core hold full speed on PSP, DS, 3DS, Wii, Vita, Raspberry Pi, handhelds? | 4 |
| What combination does nobody offer today? | 5 |

GPL, LGPL and unlicensed emulator source was not opened (READMEs, changelogs, trackers, posts only); permissive source (MAME and a7800 BSD-3 files, A7800Hawk outside `TIA_Sound/`) was read only to name timing models; AtariAge was read in a browser.

## 1. Emulators
### 1.1 Landscape
Timing: **line** = DMA summed once per line, CPU held in one block · **tick** = every MARIA tick · **cycle** = per CPU cycle with mid-instruction halts · **RTL** = hardware. D debugger, S states, R rewind, M movies, L Lua, RA RetroAchievements.

| Emulator | Licence · language | Timing | Features · devices | Platforms | Status | Speed |
|---|---|---|---|---|---|---|
| ProSystem 1.3 → libretro `prosystem` (1.3e) | GPL-2 · C [ECO.14][ECO.28] | line + per-game MD5 database (`hblank`, gun offsets, POKEY) [ECO.14] | S R RA, memory cheats; no gun, XM or netplay; HSC undocumented; POKEY $4000/$450/$440/$800 from header since 2026 [ECO.14][ECO.15] | every RetroArch target; sole 7800 core of RetroPie, Batocera, Recalbox, Lakka, muOS, Knulli, Onion [ECO.19] | 1.3 2008; libretro 8a88014287c7 2026-08-22 | 3DS 25–40 fps at 268 MHz, 60 at 804 [ECO.16][ECO.35] |
| OpenEmu ProSystem-Core (ProSystem JG 1.5.2) | GPL-2 · C [ECO.18] | line + ProSystem.dat | S, 60 s R, gun, partial XM, POKEY $450; no HSC; headered `.a78` only | macOS | 9a3eeff43986 2023-04-30 | – |
| JS7800 / Wii7800 (raz0red) | GPL-2 · JS / C++ [ECO.25] | not stated; "improved cycle accuracy" | online high-score server, XM, YM2151, banksets, BupChip, composite smoothing, mouse gun; Wii: Wiimote gun, HSC | browser; Wii | 0.0.9 2024-01-26; Wii 0.5 2020-03-25 | real time in a modern browser |
| Atari 7800+ / 2600+ firmware | not published · RK3128, 256 MB [ECO.29] | ProSystem at launch, then a JS7800 port [ECO.12] | HSC, paddle fixes in 2.0.1.4 (unverified); no states or rewind | the consoles, HDMI 720p | 2.0.1.4 2025-06-23 | – |
| A7800DS (wavemotion) | GPL per README, no LICENSE · C [ECO.26] | line; "all of the DMA Cycle adjustments/hacks" removed in 5.1 | HSC, one POKEY, banksets, optional BIOS; no gun, paddles, PAL, XM | DS, DSi | 5.2c 2026-03-03; 6c108cd9767a | DSi full speed; DS Lite most games |
| BupSystem (TailChao) | closed ("zlib" if released) [ECO.13][ECO.28] | sub-line: DMA "32 clocks" in, cycle-based audio | S, frame step, guns, XBoard, POKEY, BupChip | Windows | 0.9.6.4 2021-05-27 | "very low requirements" [ECO.07] |
| a7800 (RevEng, Trebor) | repo GPL-2+; driver, MARIA, carts, POKEY, RIOT files BSD-3 · C++ MAME fork [ECO.20] | line DMA (cap 430, hole +3) + per-pixel output; TIA slow cycles only with a gun | D S L M(.inp); gun, paddles, keypad, trak-ball, mice; POKEY $4000/$450/$800, XM, HSC; no SaveKey, A78 v4, R, RA; BIOS optional; `a7800dev` DMA bar | Win, macOS, Linux x64 | v5.2 2022-06-26; 7a2afdc1ea08 2024-11-17 | 50 % real time on an FX-6300 (v4.0: 85 %) |
| MAME `a7800` | BSD-3 driver files; TIA sound GPL · C++ [ECO.21] | line (cap 426, holes free, no slow cycles, line drawn at DMA time) | D S R L M; XM, XBoarD, HSC, POKEY $4000/$450; no gun, paddles, banksets, SaveKey; BIOS required | all MAME hosts; libretro MAME | 0.289 2026-07-30 | 105 % on the same FX-6300 [ECO.20] |
| BizHawk A7800Hawk | MIT · C# [ECO.22] | tick: 4/6-tick CPU cycles, HALT at tick 28, DLI 1 tick after DMA | D (no step) S R M(BK2) L RA; gun; POKEY $4000/$450; HSC; no XM, paddles, banksets; BIOS required | Windows, Linux x64 | 2.11.1 2026-05-01; core 1f4c27acc01b | not published |
| EMU7800 | GPL-2 · C# .NET [ECO.23] | undocumented | S; 2600 + 7800 | Windows, Linux ARM | 5.7.0.0 2025-12-31 | "runs too fast since 5.5.0" |
| test7800 (JetSetIlly) | GPL-3 · Go, CPU/TIA/RIOT from Gopher2600 [ECO.24] | cycle (release notes) | D (CLI), `-overlay` DMA/WSYNC/IRQ; HSC, SaveKey, paddles, trak-ball, QuadTari, ARM carts; BIOS optional; no YM2151 | Win, Linux, macOS, WASM | 0.9.0 2026-10-10 | "not written with performance in mind"; 4 cores |
| MiSTer Atari7800 (Kitrinx) | MIT repo, parts keep own notices · SystemVerilog [ECO.27] | RTL | "complete retail library"; guns, paddles, QuadTari, dual POKEY, YM2151, XM, SaveKey, BupChip, composite artifacts (2026) | DE10-Nano FPGA | 20260530.rbf; 0dc8ad2e3ff7 2026-09-11 | real time; ~10 lines video, ~0.5 ms input [ECO.09] |
| Permissive newcomers: 7800kit, rpw-a2 | MIT · C++/Python, TypeScript [ECO.33] | interpreter + static recompiler (7800kit) | 7800kit: 6 mappers; "135 good dumps" match its own interpreter | – | 7800kit created 2026-10-11 | – |

### 1.2 The wiki's known-problem games (rev 1304)
H handled · P partly · B broken · ? no evidence. Claims are each project's own unless a test is cited [ECO.20–ECO.28][ECO.12].

| Item (games) | ProSystem-lr | JS7800 | A7800DS | BupSystem | a7800 | MAME | A7800Hawk | test7800 | MiSTer |
|---|---|---|---|---|---|---|---|---|---|
| 1 DMA cost (Choplifter, Xenophobe, Alien Brigade, Summer Games, Ace of Aces, Karateka) | P per-game `hblank` | P | P | P | H (logic-analyser constants) | P (holes free) | P | P (3 games fixed) | H (claim) |
| 2 TIA floating bus (Choplifter sky) | ? | ? | ? | H | P | ? | ? | ? | ? |
| 3 RIOT power-on 2-button (One On One, Worm!) | P (database flag) | ? | H | P | H | P | ? | P | ? |
| 4 DL terminator `& 0x5F` (Ninja Golf, Crossbow) | ? | ? | ? | H | H | H | H | H | ? |
| 5 Hotspot over $8000–$BFFF (Planet Smashers) | ? | ? | ? | P | H | H | H | ? | ? |
| 6 Bank 0 at start (Legend of Silverpeak) | B (black screen) | ? | H | ? | H | H | H | ? | H |
| 7 First frame in VBLANK, live SWCHA (Pole Position II, Klax) | B (Klax black; OpenEmu PP II) | H | H | ? | ? | ? | P (input once per frame) | ? | ? |
| 8 POKEY RANDOM (Ballblazer) | H since 2026 | P (failed a RANDOM test) | P | ? | H | H | H | H | ? |
| 9 POKEY joins, 1.79 MHz, two-tone, filter (E.X.O., A.R.T.I.) | B | H (wiki reference) | B | P | H (wiki reference) | H (a7800's POKEY) | P (no two-tone) | P | ? |
| 10 Composite artifacts (Tower Toppler) | B | H | H | ? | P (shader only) | ? | B (TODO) | ? | H |

Plus consoles: Atari's list passes 55 of 57 NTSC retail titles (2 untested); launch firmware failed PAL Ace of Aces [ECO.29].

### 1.3 Accuracy reference today
No emulator is proven against hardware on the whole list. **a7800** is the practical reference — wiki-recommended, DMA constants
from logic-analyser runs, POKEY adopted by MAME, rated High by Emulation General [ECO.02][ECO.20][ECO.21][ECO.36] — but that
praise is self-assessed, its DMA is a line lump, and it has neither released since 2022 nor builds on current toolchains [ECO.20].
**MiSTer** claims the most ("everything is completely cycle accurate" as far as RevEng probed) and is being rewritten in 2026
[ECO.09][ECO.27]; A7800Hawk is the only permissive per-tick software model; test7800 closes items fastest but is GPL-3 [ECO.24].
**Recommendation:** hardware (the owner's console, a flash cart, muhtari's diagnostics) arbitrates; MiSTer RTL is the timing
reference; the a7800 5.2 binary is the software differential oracle; test7800 and A7800Hawk binaries and TASVideos' 11 movies are
extra checks. **Strongest alternative:** a7800 alone, one scriptable oracle. **Would change if:** a capture contradicts MiSTer
where a7800 agrees, or the 2026 rewrite regresses known games (then pin the 20260530 build).

## 2. Complaints and wishes → muhtari response
Who: P player, D developer, H ROM hacker, T TASer or speedrunner, F frontend or porter. **v1.0…** = requirement in that milestone; **seed vN** = horizon item; **won't** = not in this repository, with the reason.

| # | Complaint or wish (who) | Evidence | muhtari response |
|---|---|---|---|
| C.01 | ProSystem mis-runs homebrew: of ~110 titles 49 ok, 36 flawed, 20 black, 6 silent; "about 50 %"; a request for an a7800 core in RetroArch closed without one (P, D) | [ECO.12][ECO.16] | **v1.0** timing from hardware facts, no per-game hacks; those titles become local ledger rows |
| C.02 | Per-game MD5 database decides cart type, POKEY, controllers, `hblank`: "you're limiting the development scene"; "ProSystem.DAT file is not optional" (P, D) | [ECO.14][ECO.16] | **v1.0** A78 header authoritative; CC0 hash list only fills gaps; user override |
| C.03 | Header and hash confusion: a7800 rejects headerless dumps; Onion and OpenEmu need headers; RetroArch's scanner needs header-skip; a bad header silences Commando's POKEY (P, D) | [ECO.20][ECO.19][ECO.16][ECO.01] | **v1.0** loads both; `--probe` prints headered and headerless SHA-1/SHA-256 and RetroAchievements' MD5 |
| C.04 | POKEY wrong or silent: E.X.O./A.R.T.I., Ballblazer's RANDOM, init state, poly9, two-tone, $2x an octave high, aliasing from naive resampling (P, D) | [ECO.01][ECO.10][ECO.11][ECO.13] | **v1.0** POKEY at $4000/$450/$800: SKCTL reset gate, poly17/9, joins, two-tone, high-pass, band-limited output; **seed v4.0** own POKEY vs hardware recordings |
| C.05 | Devices set by hand: a7800 needs XM for YM2151 but not for POKEY@$800; HSC silences POKEY@$450; a7800, MAME and A7800Hawk ignore A78 v4 (P, D) | [ECO.07][ECO.20][ECO.21][ECO.22] | **v1.0** devices from the v3/v4 header, no manual slots; **v2.0** HSC + POKEY test |
| C.06 | Light gun: libretro core has none (most-upvoted open issue); MAME marks all 5 gun titles partial or no; beam maths wrong with DMA on (P, T) | [ECO.14][ECO.05][ECO.07] | **v2.0** XG-1 sensor from the exact beam tick incl. DMA; libretro light gun + pointer; offset option |
| C.07 | Missing devices and boards: paddles, trak-ball, SaveKey, QuadTari, dual POKEY, Souper + BupChip music (a7800 #8 open), ARM carts, 32 K RAM@$4000 (P, D) | [ECO.14][ECO.20][ECO.37][ECO.11] | **v2.0** paddles, SaveKey, HSC, Souper, RAM variants; **seed v2.0** QuadTari, trak-ball; **seed v4.0** dual POKEY, BupChip, ARM carts |
| C.08 | "Bugs" that are switch settings: 12 of 16 not-a-bug entries; switches momentary instead of latched; Reset on a face button breaks Sentinel (P) | [ECO.01][ECO.22][ECO.37] | **v1.0** both difficulty switches latched, as core options and inputs; mapping table in docs; not-a-bug list linked |
| C.09 | BIOS friction: PAL games crash with the NTSC BIOS; PAL Asteroids needs the PAL BIOS; MAME requires one (P) | [ECO.14][ECO.19][ECO.20] | **v1.0** fast boot needs none; optional per-region BIOS checked by md5 under ProSystem's names |
| C.10 | Unsigned NTSC ROMs fail the real BIOS but run where it is bypassed: "properly signed NTSC ROMs … are essential" (D) | [ECO.24][ECO.03] | **v1.0** `--boot bios` path; **seed v5.0** signature lint |
| C.11 | Palette: hues drift from a 25.7° to 27.7° step as consoles warm; emulators disagree; Trebor's tables carry no licence (P) | [ECO.08][ECO.04] | **v1.0** host helper generates COOL/WARM/HOT from published parameters; only native indices are hashed |
| C.12 | Composite artifacts: a7800 only via a shader chain (<1 fps at 100 % GPU; black screen); also frame blending, scanlines, integer scaling (P) | [ECO.08][ECO.20] | **seed v4.0** composite filter outside the hash; **won't** shaders — host |
| C.13 | Geometry: ProSystem reported 320×240 for a 320×223 picture; per-game safe areas; a7800 crops to 227 lines (P) | [ECO.14][ECO.11] | **v1.0** emit every row, report the true size; crop in the host; safe areas in the ledger (**v2.0**) |
| C.14 | Developers want hardware's strictness: DMA and CPU budgets ("easy to exceed real cycle budgets"); MAME still lacks mid-line writes (D) | [ECO.06][ECO.21] | **v1.0** exact DMA cost, overrun truncation, per-pixel register sampling, per-line stats API; **seed v5.0** overlay |
| C.15 | Want randomised undriven bits, ANE constant, RAM and MARIA power-on state, "BIOS residue" (D) | [ECO.03][ECO.13] | **v1.0** RAM-fill profile; **seed v5.0** hardware-variance profile, explicit and hashed |
| C.16 | Debugging means MAME's debugger: no symbols, bank-unaware, read watchpoints fire on writes; profiling needs a trace plus 7800heat; BizHawk cannot step (D, H) | [ECO.03][ECO.38][ECO.22] | **v1.0** side-effect-free peek; **seed v5.0** bank-aware breaks and watches, symbol traces, per-PC cycle profile |
| C.17 | a7800 stagnates: no release since 2022, fails to build on current Ubuntu, macOS and Python; MAME merged only its POKEY (D) | [ECO.20][ECO.21] | **v1.0** six-platform CI, attested releases every phase |
| C.18 | Speed: a7800 5.0 fell from 85 % to 50 % ("timer events for each pixel"); too heavy for a Pi 3B; POKEY breaks up on a Pi 400 (P) | [ECO.20][ECO.07] | **v1.0** lazy pixel spans, Ir/frame gate; **seed v6.0** device budgets (section 4) |
| C.19 | Latency: software emulators buffer ~35 ms of audio; RetroArch on a Pi is "night and day" against MiSTer (P, T) | [ECO.09] | **v1.0** frame boundary at VBLANK start, input latched there; **v3.0** run-ahead |
| C.20 | States fragile: "Loading states doesn't work, period. Breaks netplay too"; TIA/POKEY absent from states and run-ahead hung at the BIOS hand-off until 2023 (P, T) | [ECO.14] | **v3.0** state covers every device and BIOS RAM; run-ahead and netplay tests start at power-on |
| C.21 | Speedrun fairness: "nobody runs on the same fps"; ProSystem loses button B in Dark Chambers and ran One on One too fast (T) | [ECO.30][ECO.16] | **v1.0** exact rational rate, correct two-button mapping, console-time constants in docs |
| C.22 | Achievements count only on ProSystem and A7800Hawk; sets were probably built on ProSystem's 64 KiB bus view (inferred) (P) | [ECO.17] | **v1.0** same bus view and mirrors through memory descriptors; same hash rule |
| C.23 | Reach: "no good Atari 7800 emulators for Android"; PSP and Wii requests; on macOS, updater cores killed for invalid signatures (P, F) | [ECO.20][ECO.14][ECO.16] | **v1.0** signed arm64 and x64 macOS dylib, load-tested in RetroArch; **seed v6.0** ports |
| C.24 | Crashes: SRAM carts crash RetroArch's MAME core; ProSystem on archive paths, ARM unsigned `char`, big-endian MD5 (P, F) | [ECO.07][ECO.14] | **v1.0** fuzzed loader, failures are statuses, instance unchanged; big-endian nightly |
| C.25 | A polished all-in-one ("hopes that Stella would be an all-in-one…but nope"); hotkeys, drag-and-drop, autofire (P) | [ECO.24][ECO.20] | **won't** — no GUI in this repo (owner decision); RetroArch or Playstead host |
| C.26 | Plus consoles fail PETSCII Robots, Rikki & Vikki, trak-ball; a toggle for old POKEY demos silent since a7800 matched hardware reset (P) | [ECO.12][ECO.07][ECO.10] | **won't** — another product; hardware-exact default (demos were patched), listed as not-a-bug |
| C.27 | 2600 mode (P) | [ECO.14][ECO.22] | after the 7800 library (owner decision) |
| G.01 | Input latency: an NES core showed 3 frames against 2 until a one-line frame-boundary fix (P, T) | [ECO.39] | **v1.0** as C.19, measured in the RetroArch session test |
| G.02 | Stutter and audio-pacing regressions (P) | [ECO.41] | **v1.0** rational fps and audio ratio, frame-time percentiles; nightly p99.999 trends from v1.0 |
| G.03 | States lost on update: "No one wants to choose between losing all their save states or never updating" (P) | [ECO.39] | **v3.0** versioned state format; refused states return a status; battery saves never versioned away |
| G.04 | State size changes with an option; cheats disable states and run-ahead (P) | [ECO.39] | **v3.0** fixed state size; cheats are recorded RAM pokes |
| G.05 | Ports, big-endian, old compilers: 38 of 304 issues in four NES cores; libretro ports trail upstream (F) | [ECO.39] | **v1.0** big-endian nightly, adapter in the same repo and release; **v6.0** supported tier |
| G.06 | Save loss: two instances overwrite one save; no on-demand flush leaves a 24 s loss window (P) | [ECO.41][ECO.42] | **v2.0** NVRAM spans, atomic runner writes, flush on stop; **v3.0** continuation |
| G.07 | Unclear compatibility; hardware quirks reported as bugs (P) | [ECO.40][ECO.01] | **v1.0** generated `docs/compatibility.md` with the not-a-bug list |
| G.08 | Enhancements that silently change games break comparisons (F) | [ECO.40] | **won't** by default; any enhancement explicit and part of replay identity |
| G.09 | Issues switched off; single-author risk (P, D) | [ECO.39] | **v1.0** issue forms asking SHA-1, region, frontend, version |

Praise to keep: "accurate and light", "excellent debugger", run-ahead with rollback netplay, a NONBUGS list [ECO.39][ECO.40];
a7800's "tons of controls, very accurate", MiSTer's latency, BupSystem's "great accuracy" on "very low requirements" [ECO.07][ECO.09].
**Recommendation (C.02):** header first, CC0 hash list as fallback, no per-game behaviour hacks. **Strongest alternative:** a
ProSystem-style hint table. **Would change if:** a hardware-verified title needs state no header or hash list can express.

## 3. Personas and jobs to be done
### 3.1 Needs by horizon
Horizons [ECO.44]: v1.0 games + sound in RetroArch · v2.0 whole library · v3.0 states and movies · v4.0 measured accuracy · v5.0 tools and AI · v6.0 everywhere.

| Persona | Job | Table stakes [horizon] | Differentiators [horizon] |
|---|---|---|---|
| Player-collector, own carts, Mac + RetroArch | Play dumps of my carts (OSCR-type dumpers handle 7800 [ECO.43]) as my console did; keep high scores | Core loads in RetroArch on Apple Silicon [v1.0]; headered and headerless dumps [v1.0]; no BIOS [v1.0]; TIA + POKEY [v1.0]; switches [v1.0]; achievements map [v1.0]; PAL [v2.0]; HSC/SaveKey saves, gun by mouse [v2.0]; states, rewind, run-ahead [v3.0] | COOL/WARM/HOT palettes [v1.0]; ledger with not-a-bug notes [v1.0]; latency measured below ProSystem's [v3.0]; composite [v4.0] |
| Homebrew developer (7800basic, cc65, dasm) | Know it runs on a real console before burning a cart; iterate in seconds | Checklist at least as good as a7800's [v1.0 timing; v2.0 rest]; every homebrew board and POKEY address [v1.0 linear/SuperGame; v2.0 rest]; header-driven devices [v1.0]; headless runner with frame/audio/RAM hashes for their own CI [v1.0] | Per-line DMA/CPU stats [v1.0 API], overlay [v5.0]; lint: ROM writes, stray hotspots, palette reads, DL overrun, unsigned NTSC [v5.0]; variance profile, symbol traces, per-PC profile [v5.0]; `.b78` BEAD loading [ECO.04] and an Atari Dev Studio launch target [ECO.38] [seed v5.0] |
| ROM hacker | Find code and data across banks; patch; prove the patch changes only what was meant | Soft-patching in RetroArch (ProSystem lacks it [ECO.15]; core loads from memory) [v1.0]; `--probe`, peek [v1.0] | Bank-aware trace, write-to-ROM detection, DL/graphics viewer [v5.0]; hash diff of original against patched runs [v1.0] |
| Speedrunner | Practise and submit on an allowed set-up with fair timing | Exact 59.9579 / 49.9204 fps [v1.0]; documented boot path [v1.0]; practice states [v3.0] | Replay a submitted movie to verify it [v3.0]; autosplit RAM hook, input display [v5.0] |
| TASer | A movie that syncs everywhere, every time | Determinism, states, rerecord count, lag flag, power-on profile in the movie [v3.0] | Tick-stamped sub-frame input [on demand]; BK2 import [seed v3.0]; Lua [v5.0] |
| AI researcher | Train and compare agents on 7800 games at scale, reproducibly | Gymnasium-style env, pixels + RAM, sticky actions and frame-skip in the env layer [v5.0]; seeds [v1.0] | Vectorised in-process envs, video-off mode, reward files [v5.0]; two-player [seed v5.0] |
| Frontend integrator | Embed one small, stable, permissive core | `.info` with ProSystem's BIOS names [v1.0]; rcheevos map [v1.0]; runner exit codes [v1.0]; fixed serialize size [v3.0] | MIT C API, no globals, threads or FP [v1.0]; Playstead continuation [v3.0] and adapter pin [v6.0]; OpenEmu plugin [seed v6.0] |
| Low-power porter | Build for a handheld and hold 60 fps | C17, integer-only, no allocation after load, no threads [v1.0]; big-endian nightly [v1.0] | Per-device Ir budgets in CI, RGB565 helper, devkitPro/PSPSDK/VitaSDK lanes [v6.0] |

**Recommendation:** the roadmap order — players and developers (v1.0–v2.0), TAS and speedrun (v3.0), measured accuracy (v4.0),
tools and AI (v5.0), ports (v6.0). **Strongest alternative:** pull the DMA overlay into v2.0 — the stats exist from v1.0 and
`a7800dev`'s bar is what developers use [ECO.20]. **Would change if:** developers adopt the runner early and ask for it.

### 3.2 Speedrunners
- speedrun.com platform `gde33gek`: 46 games, 7 native to the 7800; all allow emulators and require video; 43 are real-time.
  179 runs: 155 emulated, 24 console; comments name ProSystem 22, EMU7800 21, a7800 14, BizHawk 11, BupSystem 2, JS7800 1 [ECO.30].
- Desert Falcon: "A7800, BupSystem, MAME, and JS7800 emulators are allowed. Bizhawk and RetroArch are banned"; Pac-Man bans
  "Emulation frontends such as Retroarch"; One on One 21 % says "use EMU7800Win v1.8" [ECO.30].
- Moderators convert to console time at 59.9227510135505 fps, the 2600's rate; the 7800's is 39375000/656711 ≈ 59.9579
  (derived). Emulators locked to 60.000 Hz — a7800 since v4.0, libretro ProSystem [ECO.20][ECO.15] — run 0.0703 % fast,
  1.26 s per 30 min; PAL at 50 Hz is 0.159 % fast, 2.87 s (derived).
- **Recommendation:** exactness is the feature: the rational rate reported to hosts, nothing that changes speed on by default,
emulated time printed by the runner, console-time constants in docs [v1.0]; movie replay verifies a run [v3.0]. **Strongest
alternative:** a standalone player for boards that ban RetroArch — **won't** (no GUI here; Playstead is the other route).
**Would change if:** boards publish allow-lists based on accuracy evidence; then submit the ledger.

### 3.3 TASers
- TASVideos: 13 publications (11 current), all BizHawk; "Atari 7800 BizHawk BK2" is preferred; A7800Hawk replaced EMU7800 in
  2017 ("not a very accurate emulator and is quite outdated"); rules say "Use the most accurate emulator available", custom RAM
  only if "proven to be possible on console", BIOS unmodified and region-matched [ECO.31].
- A7800Hawk reads input once per frame (no sub-frame input), counts a lag frame when no RIOT read of SWCHA or SWCHB happens,
  requires the BIOS, zero-fills RAM, times movies at the 2600's 59.92275 fps; NTSC boot costs 126–214 lag frames [ECO.22][ECO.31].
- BK2 is a zip: `Header.txt` (Core, Platform, ROM and firmware hashes, PAL, rerecordCount, StartsFromSavestate …), `Input Log.txt`
  (`LogKey:#Power|Reset|Select|BW|Toggle Left Difficulty|…|#P1 Up|…|`, then one `|…|` line per frame), `SyncSettings.json` [ECO.31].
- **Recommendation:** muhtari's own text movie carrying BK2's information — headered and headerless SHA-256, BIOS identity or fast
boot, region, port devices, RAM-fill profile, behaviour revision; one line per frame; optional tick-stamped events [v3.0]; lag =
no read of SWCHA, SWCHB or INPT0–5 in the frame, a superset of A7800Hawk's [v1.0]; a BK2 importer so published movies become
whole-game sync tests [seed v3.0]; real-BIOS boot for TASVideos' rules [v1.0]. **Strongest alternative:** a BizHawk Waterbox
port — BizHawk hosts native C cores (gpgx, snes9x, stella, melonDS …) [ECO.22] and brings TAStudio, Lua, RAM search and
acceptance. **Would change if:** TASVideos accepts a standalone runner, or BizHawk declines a second 7800 core.

### 3.4 AI agents (ALE docs and papers only; ALE is GPL-2)
- No 7800 RL environment exists: ALE is Stella (2600 only); stable-retro has no 7800 core; GymBizHawk (MIT) is a generic
  bridge [ECO.32][ECO.33].
- ALE conventions: v5 frame-skip 4, sticky actions 0.25 ("the highest value for which human play-testers were unable to detect
  any delay"), minimal action sets, full set 18; observations rgb 210×160, grayscale, ram 128 B; 108,000-frame cap (vector env);
  AtariVectorEnv (v0.11.0) is "native C++ code with multi-threading"; Machado et al. 2018 add game-over-only termination and
  fixed frame budgets [ECO.32]. Throughput: ALE 2013 "up to 6000 frames per second"; EnvPool (Pong, frame-skip 4) 49,439 fps on
  a 12-thread laptop, 200,428 on a 32-thread Ryzen 9 5950X — about 4–6 k fps per hardware thread [ECO.32].
- **Recommendation [v5.0]:** a Python package over the C API: Gymnasium `Env` plus a vector env stepping N instances per worker
thread (instances share nothing); observations = palette indices or RGB at 320×242 and `ram` = 4,096 B system + 128 B RIOT (+
cart RAM); actions = per-game minimal set, 36 for ProLine (9 directions × 4 button states), 18 for one-button; sticky actions,
frame-skip and max-pooling in the env layer from the env seed, never in the core; difficulty switches as ALE difficulty, Select
presses as modes; reward and terminal from data files (RAM address, BCD score, lives); a video-off mode that leaves RAM hashes
unchanged; ≥ 5,000 emulated frames/s per thread on an Apple-Silicon-class host (derived: ≤ 4.3 M Ir/frame, section 4, on the
aarch64 host that ran the NES sibling's ~25 M Ir/frame at ~1 ms/frame [ECO.39]). **Strongest alternative:** add the libretro core to stable-retro (MIT)
and reuse its tooling — least code, Python-speed stepping. **Would change if:** the native env cannot beat that by about 3×.

### 3.5 Frontends
- **libretro:** ProSystem is the only 7800 core; its `.info` lists `7800 BIOS (U).rom` (md5 0763f1ffb006ddbe32e52d497ee848ae,
  optional), Recalbox the (E) BIOS (md5 397bb566584be7b9764e7a68974c4263); it reports 60/50 fps, 320×223, 32,640 Hz; 41 cheat
  files exist for the 7800 [ECO.15][ECO.19]. rcheevos (console 51) maps the 64 KiB bus: system RAM $1800–$27FF, three virtual
  mirrors $2800–$3FFF, cart RAM $4000–$7FFF, ROM $8000–$FFFF; the hash is the MD5 after a 128-byte "ATARI7800" header [ECO.17].
  muhtari: same BIOS names, memory descriptors and hash rule [v1.0]; exact fps, 48 kHz, full height; gun, paddles and switches
  as libretro devices and options [v2.0].
- **Playstead:** adapter pin plus a continuation gate — `qualify`, `initial-run`, `continue`; exit 0 pass, 77 capability
  refusal, 1 failure; network denied; ≥ 2 agreeing oracle runs [ECO.42]. The runner's movies and frame hashes are that oracle
  [v3.0]; `a7800` is a one-way system ID there [v6.0]. **OpenEmu:** ProSystem JG, GPL-2, last updated 2023 [ECO.18] [seed v6.0].
- **Recommendation:** libretro first (the owner's Mac frontend; it also reaches 3DS, Vita, Wii, Pi); Playstead through the runner's
process contract; OpenEmu as a seed. **Strongest alternative:** OpenEmu earlier, as the Mac-native frontend. **Would change if:**
OpenEmu's maintainers ask, or the owner changes Mac frontend.

## 4. Low-power device budgets
**Work per NTSC frame** (derived): 263 × 454 = 119,402 ticks; ≤ 29,850.5 CPU cycles; ≤ 34,364 MARIA fetches (426 ticks ÷ 3 ×
242 lines); 77,440 pixels; 800 samples at 48 kHz. Full speed is 1,789,773 CPU cycles/s, so a device's **budget** is MHz ÷ 1.79
host cycles per CPU cycle. **Cost model** (unverified until the v1 Cachegrind gate): CPU 40–80 Ir per cycle, fetch 15–30 Ir,
pixel 4–8 Ir, audio ≈ 0.2 M Ir → burst/lazy design **1.8–4.3 M Ir/frame = 60–144 Ir per CPU cycle**; per-tick stepping adds 3–6 M
→ **161–345**. At an assumed in-order IPC of 0.5–0.8 (unverified) that is 75–288 host cycles per CPU cycle, per-tick 201–690.
Anchors: A7800DS (hand-tuned line lump in TCM) fits 75 on a DSi; PSP7800 (ProSystem 1.1) tops 60 fps at 222 MHz (budget 124);
libretro ProSystem needs ~225–360 on an Old 3DS; the NES sibling's per-dot design measured ~25 M Ir/frame [ECO.26][ECO.35][ECO.39].

| Device | CPU · MHz | RAM | FPU · endian | 7800 emulation today | Budget | Burst · per-tick |
|---|---|---|---|---|---|---|
| DS / DS Lite | ARM946E-S 67 (+ ARM7 33) | 4 MB, slow main bus | none · LE | A7800DS: most games; big bankswitched ones struggle; frame-skip | 37 | no · no |
| DSi | ARM946E-S 134 | 16 MB | none · LE | A7800DS: all but two games without frame-skip | 75 | no · no |
| 3DS | ARM11 ×2, 268 | 128 MB (64 to apps) | VFPv2 · LE | ProSystem 25–40 fps | 150 | marginal · no |
| New 3DS | ARM11 ×4, 804 | 256 MB | VFPv2 · LE | ProSystem ~60 fps | 449 | yes · marginal |
| PSP | Allegrex 222 / 333 | 32 / 64 MB | single + VFPU · LE | PSP7800 > 60 fps at 222 MHz | 124 / 186 | marginal · no |
| Wii | Broadway 729 | 24 + 64 MB | paired-single · **BE** | Wii7800, RetroArch ProSystem | 407 | yes · marginal |
| PS Vita | Cortex-A9 ×4, 333 / 444 | 512 + 128 MB | VFPv3, NEON · LE | RetroArch ProSystem works | 186 / 248 | yes at 444 · marginal at 444 |
| Pi Zero / W | ARM1176 1000 | 512 MB | VFPv2 · LE | lr-prosystem ok; "You can't use accurate emulators on a pi0" (NES) | 559 | yes · marginal |
| Pi Zero 2 W, Pi 3B/3B+ | Cortex-A53 ×4, 1000–1400 | 0.5–1 GB | NEON · LE | lr-prosystem; a7800 too heavy for a 3B | 559–782 | yes · marginal–yes |
| Pi 4 / 400 | Cortex-A72 ×4, 1500–1800 | 1–8 GB | NEON · LE | a7800 POKEY breaks up on a 400 | 838–1006 | yes · yes |
| RG35XX Plus/H/SP | Allwinner H700 A53 ×4, 1500 | 1 GB | NEON · LE | ProSystem only | 838 | yes · yes |
| Miyoo Mini (+) | SigmaStar SSD202D A7 ×2, 1200 | 128 MB | VFPv4 · LE | Onion lr-prosystem ✔ | 671 | yes · marginal |

Specs [ECO.34]; emulation [ECO.26][ECO.25][ECO.35][ECO.19][ECO.07]. Integer-only, so no FPU is needed; Wii (with GameCube and
Wii U) is the only big-endian family.
**Recommendation:** keep the burst/lazy design; v6.0 full-speed tiers are Pi Zero and up, A53/A7 handhelds, Wii, Vita at 444 MHz
and New 3DS; PSP and Old 3DS are "measure, then decide"; DS and DSi stay with A7800DS. **Strongest alternative:** a line-lump
"fast profile" for DS-class devices — rejected: two timing models, two hash sets. **Would change if:** v1 measures ≤ ~2 M
Ir/frame (PSP and Old 3DS become feasible) or ≥ ~8 M (Pi Zero, Vita, New 3DS slip; per-tick would leave Pi 3-class only).

## 5. The slot muhtari fills
No 7800 emulator combines a permissive licence, an embeddable C library, accuracy at least a7800's, Pi-class speed,
deterministic output across platforms and automation interfaces. The accurate software emulators are a stalled MAME fork
(a7800), a C# core bound to BizHawk (A7800Hawk) and two GPL desktop programs (test7800, EMU7800); the most accurate
implementation is FPGA (MiSTer). What most people run — libretro's only 7800 core, OpenEmu's core, the Plus consoles' firmware —
descends from ProSystem (GPL) and keeps a per-game database.
- Every distribution checked ships that one ProSystem core, without a light gun [ECO.19][ECO.14].
- Permissive 7800 code: A7800Hawk (C#, BizHawk-bound), MiSTer (RTL), 7800kit (created 2026-10-11) [ECO.22][ECO.27][ECO.33].
- None publishes cross-platform frame/audio hashes, a measured ledger or a speed gate; a7800, MAME, A7800Hawk ignore A78 v4 [ECO.20].
- No RL environment for the 7800; TAS only in BizHawk, timed at the 2600's rate, without sub-frame input [ECO.32][ECO.31].
- On DS/PSP-class devices only ProSystem derivatives run, kept fast by simplifications and per-game fudges [ECO.06][ECO.26].
- **Strongest reason this is wrong:** test7800 moves fastest (0.9.0 on 2026-10-10, WASM build) and a relicence would close much
of the slot; 7800kit could mature; MiSTer's RTL could be Verilated into a permissive, slow software model. **Would change if:**
any permissive core passes the whole checklist with save states and a libretro adapter.

## 6. Open questions
- Speed of a7800 5.2, A7800Hawk, test7800 and ProSystem on one named CPU (only a7800 #4 publishes any): measure the released
  binaries locally beside muhtari's Ir/frame in v1 phase 3; run the same binaries on the 20 checklist items as ledger rows.
- Real IPC of emulator code on ARM11, Allegrex and Cortex-A9: section 4's PSP, Old 3DS and Vita verdicts hinge on it.
- Would TASVideos publish muhtari movies (own runner, or a Waterbox port), and does BizHawk want a second 7800 core?
- Licence of the Plus consoles' JS7800 port (no source published); MiSTer per-file notices after the 2026 rewrite.
- Trebor's palettes, RevEng's test ROMs, Synthpopalooza's POKEY demos carry no licence; RetroAchievements' 7800 set count (403).

## 7. Sources
| ID | Tier | Source (URL) | Pin or access date | Supports |
|---|---|---|---|---|
| ECO.01 | T2 | 7800.8bitdev.org/index.php?title=Common_Emulator_Development_Issues | rev 1304 (2026-06-03) | 20 problem items, 16 not-a-bug items |
| ECO.02 | T2 | 7800.8bitdev.org Emulators_and_Tools; A7800_Emulator | rev 1065; rev 993 | Recommendations (written by a7800's maintainer) |
| ECO.03 | T2 | 7800.8bitdev.org Introduction_to_the_MAME_debugger; Debugging_On_Ancient_Game_Platforms; 7800AsmDevKit | rev 1192; 966; 1104 | Debugger quirks, developer workflow |
| ECO.04 | T2 | 7800.8bitdev.org The_Atari_7800_BEAD_Executable_Specification; Atari_7800_Color_Documentation | rev 603; 1107 | BEAD, palettes |
| ECO.05 | T1 | github.com/mamedev/mame hash/a7800.xml (CC0) | ac859eae1ecd | Gun titles partial/no |
| ECO.06 | T4 | forums.atariage.com/topic/ 224025, 343425, 145363, 224052 | accessed 2026-10-15 | DMA tests, ProSystem bugs, wishes |
| ECO.07 | T4 | forums.atariage.com/topic/ 268461, 268458 | accessed 2026-10-15 | a7800 changelog, complaints, Pi speed |
| ECO.08 | T4 | forums.atariage.com/topic/ 216065, 213725, 322259, 349547, 385287, 353253 | accessed 2026-10-15 | Palettes, artifacting |
| ECO.09 | T4 | forums.atariage.com/topic/ 318275, 353281 | accessed 2026-10-15 | MiSTer claims, latency, guns |
| ECO.10 | T4 | forums.atariage.com/topic/ 339785, 318358, 293427, 222380 | accessed 2026-10-15 | WSYNC, POKEY modes, reset |
| ECO.11 | T4 | forums.atariage.com/topic/383566 (test7800) | accessed 2026-10-15 | test7800 history, crops, wishes |
| ECO.12 | T4 | forums.atariage.com/topic/ 375832, 355224, 314301 | accessed 2026-10-15 | Plus consoles, 110-title ProSystem test |
| ECO.13 | T4 | forums.atariage.com/topic/ 385521, 285921 | accessed 2026-10-15 | ANE variance, BupSystem findings |
| ECO.14 | T1/T4 | github.com/libretro/prosystem-libretro README, License.txt, 23 issues, 69 PRs | 8a88014287c7 | Features, database, complaint tally |
| ECO.15 | T1 | github.com/libretro/libretro-core-info prosystem_libretro.info; docs.libretro.com/library/prosystem; libretro-database `cht/Atari - 7800` | 5a74858ab2f7; 81bf9d3a2f15 | BIOS md5, features, timing, cheats |
| ECO.16 | T4 | github.com/libretro/RetroArch issues 3241, 10042, 12585, 17773; forums.libretro.com t/15603, t/3440, t/4732 | accessed 2026-10-15 | Requests, macOS signing, 3DS speed |
| ECO.17 | T1 | github.com/RetroAchievements/rcheevos src/runtime/rc_consoleinfo.c; docs.retroachievements.org | 116d01847075; fa5bc6f3640e | Memory map, hash, supported emulators |
| ECO.18 | T1 | github.com/OpenEmu/ProSystem-Core; OpenEmu issues 1586, 2513 | 9a3eeff43986 | OpenEmu core |
| ECO.19 | T1 | RetroPie docs Atari-7800; Batocera; Recalbox wiki; muOS; Knulli; Onion; Lakka | 3b7fa9ced4d8; 716d2dc9b9ef; accessed; 719c27400130; 57afa6620096; 81e82056bcd1; 4f70542a559c | Distribution defaults |
| ECO.20 | T1/T3/T4 | github.com/7800-devtools/a7800 LICENSE.md, README, releases, issues 4, 7, 8, 13, 22, 23, 25, 27, 30, 33 | 7a2afdc1ea08 | Licence, timing, features, speed |
| ECO.21 | T1/T3 | github.com/mamedev/mame src/mame/atari/a7800.cpp, maria.cpp, src/devices/bus/a7800/; PRs 10262, 2506; mametesters 08589 | ac859eae1ecd; mame0289 | MAME timing, features |
| ECO.22 | T1/T3 | github.com/TASEmulators/BizHawk A7800Hawk (outside TIA_Sound), waterbox/readme.txt, issues 726, 3574 | c9de9a8bff0f | Timing, lag rule, Waterbox |
| ECO.23 | T1 | github.com/emu7800/emu7800.github.io | bc636528d5d1 | EMU7800 |
| ECO.24 | T1 | github.com/JetSetIlly/test7800 README, COPYING, releases, issues 27, 33, 35 | ce8e29a987a1; v0.9.0 | test7800 |
| ECO.25 | T1 | github.com/raz0red/js7800; github.com/raz0red/wii7800 | fe918ce422bc; b1d9aaed9443 | JS7800, Wii7800 |
| ECO.26 | T1 | github.com/wavemotion-dave/A7800DS README and changelog | 6c108cd9767a | DS/DSi speed and fixes |
| ECO.27 | T1 | github.com/MiSTer-devel/Atari7800_MiSTer README, LICENSE, PRs 59, 61, 62 | 0dc8ad2e3ff7 | MiSTer |
| ECO.28 | T1 | github.com/gstanton/ProSystem1_3 Help/History.htm; tailchao.com/BupSystem | 3040295934c6; 0.9.6.4 | ProSystem, BupSystem |
| ECO.29 | T1/T4 | PLAION 7800+ press release; Atari support FAQ and 2600+ compatibility list (2023-12-18); Time Extension, Gizmodo, Ctrl-Alt-Rees | accessed 2026-10-15 | Plus consoles |
| ECO.30 | T1 | speedrun.com/api/v1 (platform gde33gek games, runs); rules of Desert_Falcon, pacman, One on One; run zx5jx9em | accessed 2026-10-15 | Speedrun data and rules |
| ECO.31 | T1/T2 | tasvideos.org Publications API (A7800), EmulatorResources, PlatformResources/A7800, MovieRules, Bizhawk/BK2Format, Forum/Posts/456199 | accessed 2026-10-15 | TAS data, BK2 |
| ECO.32 | T1 | ale.farama.org; arXiv 1207.4708v2 (Bellemare 2013), 1709.06009v2 (Machado 2018), 2206.10558v2 (EnvPool) | ALE v0.12.1, 7eb70f7936ba | ALE conventions, throughput |
| ECO.33 | T1 | github.com/Farama-Foundation/stable-retro docs/supported_emulators.md; GymBizHawk; vs-sr-dev/7800kit; seanahan5000/rpw-a2 | e2fdb2a6d39f; ae43e9328cf3 | RL options, permissive 7800 code |
| ECO.34 | T1/T2 | raspberrypi.com product docs; anbernic.com rg35xx-plus; problemkaputt.de/gbatek.htm; 3dbrew.org Hardware; wiibrew.org Broadway; Wikipedia PSP, PS Vita, New 3DS | accessed 2026-10-15 | Device specs |
| ECO.35 | T1/T2/T4 | PSP-Archive/PSP7800 ChangeLog; emulation.gametechwiki.com 3DS/PSP/Vita/Wii pages; RetroPie forum 31526 | c745251; revs 151221, 139837, 150551, 151176 | Measured device results |
| ECO.36 | T2 | emulation.gametechwiki.com/index.php/Atari_7800_emulators | revid 150536 | Emulator ratings |
| ECO.37 | T4 | reddit.com r/atari7800 1kelrrb, r/RetroArch 1hxdq14, r/SBCGaming 1my935l; github.com/mattias800/spela 936 | accessed 2026-10-15 | Player complaints |
| ECO.38 | T1 | github.com/7800-devtools/7800basic; randomterrain.com/7800basic.html; cc65.github.io/doc/atari7800.html; github.com/7800-devtools/7800heat | accessed 2026-10-15 | Developer workflow |
| ECO.39 | T4/T1 | nesturbator `.planning/preparation/NES-ECOSYSTEM.md` (relayed tracker evidence); `.planning/milestones/v3-phases/15-speed-and-close-out/15-RESEARCH.md` (own measurement) | 8ae2c6e32e7f; 3f39694fd4b8 | NES precedent; ~25 M Ir/frame |
| ECO.40 | T4 | glueyneo `.planning/preparation/NEOGEO-HARDWARE-AND-ECOSYSTEM.md` (relayed) | 2e5d8849a57f | Neo Geo precedent |
| ECO.41 | T4 | gabbaboy `.planning/research/PRECEDENT.md`, `FEATURES.md` (relayed) | aa62bb52def4 | Game Boy precedent |
| ECO.42 | T1 | playstead `playstead-mac/docs/CONTINUATION-PROTOCOL.md` | 012163f047c3 | Continuation contract |
| ECO.43 | T1 | github.com/sanni/cartreader README | 060d8ae0bf4b | 7800 cart dumping |
| ECO.44 | T1 | `.planning/PRODUCT-ROADMAP.md` (this repository) | updated 2026-10-15 | Horizons |
