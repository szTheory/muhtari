# Provenance

How code, data and hardware facts enter muhtari, which upstream files may seed it, and which notices that creates. Written on 2026-10-15, before any emulator code exists, so it governs the whole history. People and AI assistants follow it for every commit. [ASSET_POLICY.md](ASSET_POLICY.md) covers ROM, BIOS and fixture bytes; `THIRD-PARTY-NOTICES.md` holds the notices this file requires. `[PV.NN]` IDs resolve in §10.

| Question | Section |
|---|---|
| May this code, table or fact enter? | 1 |
| Is this upstream file usable at its pin? | 2, 3 |
| What seeds each module, and how is the seed retired? | 4 |
| What must a derived file, the notices file and each release zip contain? | 5 |
| BIOS, HSC ROM, RIOT-RAM loader, hash list, palettes, measured constants? | 6, 7 |

## 1. Rules
- **Licence.** muhtari is MIT ([LICENSE](LICENSE)). Inbound code or data must be MIT, BSD-2/3-Clause, Zlib, BSL-1.0, ISC or CC0-1.0 *at the exact commit cited*. Read the repository licence **and** the file's own header at that pin. Licences change: the MiSTer core was GPL-2.0, then CC BY-NC 4.0, and has been MIT only since 2026-08-30 [PV.03]. Permissive repositories also hold GPL files, vendor IP and Atari firmware (§3).
- **Facts** (addresses, timings, measured values, test expectations) enter in this project's own words, with a citation giving source, tier and pin. Facts are free to use; prose, code, tables and file structure are not.
- **Permissive expression enters only through §4.** Code, tables, comments, names or structure from another project are copied or ported only as a §4 seed: one module, with its notice (§5), a replacement trigger and a parity proof. Other permissive sources are read to learn from; the behaviour is then written from documentation and shown by a test.
- **Copyleft and unlicensed source stays closed.** GPL/LGPL source, and source with no stated licence, is never read past its licence header, never pasted into an assistant's context and never kept in the workspace (no clones, no vendored copies, no "reference" folders). Its documentation, changelogs, issue trackers and commit metadata may be read. It runs only as **released binaries**, used as oracles (a7800 v5.2, test7800 v0.9.0, MAME) [PV.14]. An oracle is never built from source here.
- **Suspect constants.** A constant, table or name that the cited sources do not explain is suspect. It is re-derived from a cited source, or removed, before it merges.
- **AI assistance is disclosed** in a co-author trailer or the PR description. An assistant writes from documentation, this repository and test results, and is never asked to port, translate or reproduce code outside §4. Why (precedent from the NES sibling, nesturbator DEC.04 [PV.13]): in 2026 an AI-assisted MIT emulator was found to contain GPL code ported from emulators kept in its workspace, and it had to relicense.
- **Firmware and game bytes never enter** (§6, ASSET_POLICY.md), whatever licence the file that carries them claims.

## 2. Upstream audit (LICENSE and file headers read at the pin, 2026-10-15)
| Source @ pin | Licence at the pin | Usable |
|---|---|---|
| MAME `dddf4cd1faee` [PV.01] | Whole: GPL-2.0; each file: its own header; `hash/`: CC0 | BSD-3: `src/mame/atari/{maria,a7800,tia}.*`, `src/devices/bus/a7800/*` (11 files, Priuli), `sound/pokey.*`, `machine/mos6530.*`, `cpu/m6502/m6502.*`. CC0: `hash/a7800.xml`. No `bus/a7800_ctrl/` at this pin |
| a7800 `7a2afdc1ea08` [PV.02] | Whole: GPL-2.0-or-later; files "may also be licensed as specified in the file header" | BSD-3: `src/mame/drivers/a7800.cpp`, `video/maria.*`, `bus/a7800/*` (+ `bankset`, `xm`), `bus/a7800_ctrl/*` (22 files, Coder, Saarna), `bus/vcs_ctrl/*`, `sound/pokey.*`, `machine/6532riot.cpp` |
| MiSTer Atari7800 `0dc8ad2e3ff7` [PV.03] | MIT since `8c96e1f7cc24` (2026-08-30) for "project-authored code"; files with their own notices keep them | MIT, edited only by the licensor: `rtl/Maria/*`, `rtl/6502/*` (incl. `sally.sv`), `rtl/Pokey/*`, `rtl/composite_*.sv`, `rtl/paddles.sv`, `rtl/a78_cart_extent.sv`. MIT header, but third parties edited it while the repo was CC BY-NC 4.0, so **facts only**: `rtl/TIA.sv`, `rtl/RIOT/M6532.sv`, `rtl/cart.sv`, `rtl/top.sv`, `Atari7800.sv`. Zlib: `rtl/souper.v` (Celimli 2015). BSD-3-style: `rtl/t65/*` (Wallner et al.) |
| BizHawk `c9de9a8bff0f` [PV.04] | The BizHawk team's C# is MIT; the repo as a whole "a minefield" | `Consoles/Atari/A7800Hawk/**` except `TIA_Sound/` |
| ymfm `81aec25ccbb9` [PV.05] | BSD-3, © 2021 Aaron Giles | `src/ymfm_opm.*`, `ymfm.h`, `ymfm_fm.*` |
| floooh/chips `9e88298ce563` [PV.06] | Zlib, © 2018 Andre Weissflog | `chips/m6502.h` (cross-check) |
| SingleStepTests 65x02 `2f6980a2d957` [PV.07] | MIT, © 2024 Thomas Harte et al | `6502/v1` vectors (data) |
| cc65 `a6530f469142` [PV.08] | Zlib | `atari7800` target (33 files): the tool, and its runtime inside our test ROMs |
| 7800basic `e7c18d2ecdc6` [PV.09] | Compiler GPL-2; included and generated 6502 code CC0; output belongs to its author | As a tool only; `includes/*` CC0 except the files named in §3; `lzsa1compression.asm` BSL-1.0 (Brandwood) |
| 7800OpenBIOS `b8bc745a8744` [PV.10] | BIOS source CC0; the KiloParsec game all rights reserved | `7800openbios.asm`, built with KiloParsec removed (§6) |
| 7800.8bitdev.org wiki [PV.11] | None stated (no rights text, no copyright page) | Facts in our own words; a short quote with a citation; prose and tables are not copied |
| nesturbator `9f9386c661f6` [PV.12] | MIT; every commit is by the owner's account or its release bot | All of it, except its third-party files (`libretro.h`, vectors and test ROMs keep their own notices) and NES facts |

Pins: a7800 `7a2afdc1ea08` is v5.2 (`4dbf85ea90fb`, the oracle binary) plus 25 palette, display and notes commits; its `pokey.cpp` last changed 2022-06-08, before v5.2. 7800basic `e7c18d2ecdc6` is v0.41 (`03aebb85206f`) plus 7 commits.

## 3. Excluded or unknown (read only up to the licence header; never copied)
"Unknown" counts as excluded until the upstream adds a notice.
- **GPL/LGPL:** MAME and a7800 `src/devices/sound/{tiasound,tiaintf}.*` (GPL-2.0+); MiSTer `rtl/jt51/*` (30 files, GPL-3), `rtl/sdram.sv` (GPL-3), `rtl/video_mixer_plus.sv` (GPL), `rtl/arm7tdmi/arm7tdmi_core.sv` (GPL-2.0-only), `sys/*` (framework: GPL, Intel IP or unmarked), `rtl/EEPROM_24LC256.sv` (no header; "for NES MiSTer", a GPL-3 repo); EMU7800 (GPL-2); ProSystem and the libretro `prosystem` core, js7800 and Stella (GPL-2); test7800 and Klaus Dormann's 6502 tests (GPL-3); the 7800basic compiler and dasm (GPL-2); sign7800 (LGPL-2) [PV.04][PV.09][PV.14].
- **Unknown:** MiSTer `rtl/lightgun.sv`, `rtl/mouse_st.sv`, `rtl/detect2600.sv`, `rtl/dpram.vhd`, `rtl/pll*`, `rtl/palettes/*`, `Palette/**` and `assets/*.pal` (Trebor's palettes with no notice), `stella.pal`, `rtl/mem0.hex` (= `assets/7800title.bin`, 16 KiB, unidentified), `rtl/ooo.hex`, `rtl/bupchip.*`, `rtl/Minnie/minnie_wave.*`, `docs/F15_Atari7800.pdf`, `releases/*.rbf`. BizHawk `A7800Hawk/TIA_Sound/`: the commit metadata shows a renamed copy of BizHawk's own 2600 TIA audio (sizes 3,703 B and 1,558 B match at `a2ab02f9d905` (derived)), whose 2012 commits read as original work, but EMU7800's GPL `TIASound.cs` sat beside it until 2017-08-29. That cannot be proven either way, so it stays excluded. 7800basic `includes/rmtplayer.asm` (© Raster/C.P.U. 2002–2003, no licence), `includes/macro.h` (after Jentzsch's 2600 file, no licence), `includes/{i2c7800,i2c7800left,speakjet}.inc` ("By Alex Herbert, 2004", no licence), and `samples/**` including `PALETTESx6.zip`.
- **Firmware and Atari code inside permissive repositories:** MiSTer `rtl/mem4.hex` is the HSC ROM and `rtl/ar.hex` is the Starpath Supercharger BIOS (both decoded and matched by SHA-1 here (tested)). MiSTer `rtl/RIOT/M6532.sv` L69–80 is the loader the BIOS leaves in RIOT RAM. MiSTer `docs/7800rom.asm` and `docs/7800source.asm` transcribe Atari's 7800 OS listing. The wiki page "7800 NTSC BIOS" (rev 1259) is a disassembly that "builds a 1:1 with the official BIOS ROM". a7800's `hash/a7800.xml` has no header (use MAME's CC0 copy).

## 4. Strangler-fig seeds
A seed is the only route by which another project's expression reaches `src/`. Retiring a seed needs a **zero-diff differential**: the seed and its replacement run in one test binary over the row's corpus, and the frame, audio-transition, PCM and state hashes must match on all six platforms. The notice leaves in the same PR.

| Module | Seed @ pin | Licence | Must change | Attribution | Replace when | Parity corpus |
|---|---|---|---|---|---|---|
| SALLY CPU | nesturbator `src/cpu.c` @`9f9386c661f6` | MIT, same owner | NMOS decimal ADC/SBC; per-cycle NMI (penultimate cycle, branch, BRK hijack); generic bus seam; drop 2A03 paths | commit-body line (§5a) | n/a (own code) | SingleStepTests `6502/v1`, every bus cycle |
| Bus, synth, state, runner, libretro, CI, hygiene | nesturbator @`9f9386c661f6` | MIT, same owner | MARIA ticks; 4/6-tick cycles; HALT on any cycle; RDY on reads only; several audio sources | commit-body line; `libretro.h` keeps its notice | n/a | its own hash pins |
| POKEY (fallback only) | a7800 `src/devices/sound/pokey.cpp` @`7a2afdc1ea08` | BSD-3-Clause | used only if the own POKEY (written from the Altirra Hardware Reference Manual, A7800-AUDIO §5) slips: C17 integer port; the `double`/`exp()` output stage (L774–818) becomes integer tables; (tick, level) transitions; plus the init, two-tone and high-pass fixes MAME lacks | §5a header; notices section in all zips | v4: checked against MiSTer `rtl/Pokey/*` | POKEY unit vectors; 7800basic POKEY and RMT samples; E.X.O., A.R.T.I., Ballblazer (local) |
| YM2151 (v4) | ymfm `src/ymfm_opm.*` @`81aec25ccbb9` | BSD-3-Clause | C++ to C17; OPM only; integer | §5a | only if size or speed demands it | ymfm itself, built as a test oracle |
| Palettes (host helper) | a7800 `src/mame/drivers/a7800.cpp` L705–1452 | BSD-3-Clause | 6 tables (NTSC/PAL × cool 25.7°, warm 26.7°, hot 27.7°) become a C table outside the hash; only the warm NTSC and warm PAL tables are seeded (`host/palette.c`), and the other four remain deferred | §5a | an own generator, if the owner wants no notice (§7) | byte-equal RGB tables |
| MARIA, TIA audio, RIOT, boards, ports, HLE boot | none: own code from the Software Guide, the A78 spec, schematics and measured facts | — | — | lifting any table turns it into a seed row | — | diagnostics; hash-list ledger |

Read, never copied: MAME and a7800 `maria.cpp`, `bus/a7800/*`, `a7800_ctrl/*`; MiSTer `rtl/Maria/*`, `sally.sv`, `TIA.sv`, `M6532.sv`, `cart.sv`, `souper.v`; floooh `m6502.h`; A7800Hawk. a7800's `pokey.cpp` credits "original info found in Ron Fries' Pokey emulator", yet MAME licenses it BSD-3 under three named holders. That is accepted as stated.

## 5. Notices
**(a) Source files.** Own files carry no header; the repository LICENSE covers them, as in nesturbator. Code taken from nesturbator gets no header either; the commit body says `Seeded from nesturbator <sha> <path> (same author, MIT).` A file derived from a seed starts like this:
```c
/* SPDX-License-Identifier: MIT AND BSD-3-Clause
 * Copyright (c) 2026 muhtari contributors
 * Derived from a7800 src/devices/sound/pokey.cpp at 7a2afdc1ea08 (BSD-3-Clause),
 * copyright-holders: Brad Oliver, Eric Smith, Juergen Buchmueller; a7800 changes: Mike Saarna.
 * Changes: C17 port, integer mixer, transition output. Texts: LICENSE, THIRD-PARTY-NOTICES.md "POKEY". */
```
Licence terms: BSD-3 clause 1 (keep the notice, conditions and disclaimer with source) is met by the holders line plus the full text in `THIRD-PARTY-NOTICES.md`. That is MAME's own practice (a `license:` line in each file, texts in `docs/legal/`) [PV.01]. Under clause 3, upstream names appear only in notices and citations, never as endorsement. MIT: the notice goes in "all copies or substantial portions". Zlib: mark altered source and keep the notice in source; binaries need nothing. BSL-1.0: source only. CC0: nothing is required (a courtesy line is added).
- **Recommendation:** an SPDX line in derived files only. **Strongest alternative:** an SPDX line in every file (REUSE style). **Would change if:** a distributor asks for REUSE compliance.

**(b) `THIRD-PARTY-NOTICES.md`** has one section per upstream, each giving: the paths it applies to in this repo; the upstream URL, path and 40-hex commit; the SHA-256 of any vendored unmodified file; what changed; and the licence text verbatim, naming the holders as the upstream header does. MAME's template reads `Copyright <YEAR> <COPYRIGHT HOLDER>` and upstream states no years, so write the holders only.

**(c) Release archives.** From the first release, the library, runner and libretro zips each hold `LICENSE` and `THIRD-PARTY-NOTICES.md` at the root. A BSD-3 or MIT seed compiles into the core, which all three zips contain, and BSD-3 clause 2 and MIT require the notice with binaries. nesturbator shipped its notices only in the libretro zip, because only `libretro.h` was third-party there [PV.12].
- `release.notices` (CMake `-P`, untested), run in `cmake --workflow --preset ci` and in release.yml before attestation: each zip must hold exactly one root `LICENSE` and one `THIRD-PARTY-NOTICES.md`, byte-identical to the release commit's. Its self-test builds a zip without them and must fail, naming the missing file. It extends nesturbator's per-component archive allowlist (`tests/cmake/check_archives.cmake`) [PV.12].
- `policy.notices` (untested): every SPDX expression in a tracked source is `MIT`, or `MIT AND` one of the licences in §1. Each derived file is listed in a notices section, every listed path exists, and the libretro `.info` `license` field names the same licences.

## 6. BIOS, HSC ROM and the RIOT-RAM loader
- The Atari BIOS (NTSC, PAL, prototype), the HSC ROM and every byte of the loader the BIOS leaves in RIOT RAM are **user-supplied only**. They are never committed, packaged, typed into C arrays or reconstructed. Their identifying digests are in ASSET_POLICY.md §4.
- **HLE fast boot** (the default, redistributable) sets the post-BIOS state from measured facts in MiSTer's licensor-only files and the BIOS behaviour documented in A7800-SYSTEM: registers (S = X = $16, D = 1, I = 1), INPTCTRL `$16`, CTRL `$60`, the hand-off DPP and beam position (`video_sync.sv` L146–152, `control.sv` L143–149) [PV.03]. It writes **no loader** into RIOT RAM `$0480–$04FF`, which keeps the RAM fill: no known 7800 title reads it; the loader only matters to 2600 games (SEED-006).
- **Recommendation:** no loader. **Strongest alternative:** the 128-byte loader OpenBIOS copies there (`7800openbios.asm` L846–852) [PV.10] — but it matches Atari's loader instruction for instruction (A7800-SYSTEM), so its provenance must be ruled on before any byte of it is used; MiSTer's image is Atari's loader with "deliberate Decathlon workarounds" and is never used. **Would change if:** a 7800 title is shown to read RIOT RAM after the hand-off; then a local trace with the user's BIOS records which bytes it reads, and those addresses and values enter as facts.
- **OpenBIOS boot** is built in CI from the pinned source, with `incbin "kiloparsec.bl1"` (L164) replaced by 8,192 fill bytes. It is never packaged, because its splash screen draws the Atari Fuji.
- **HSC:** the user's ROM. A later HLE is written only from the HSC Technical Reference (wiki rev 1337). The wiki's BIOS disassembly, MiSTer's OS listings and traces of a real BIOS are never sources for HLE code.

## 7. Data licences
- **Hash list.** MAME `hash/a7800.xml` @`dddf4cd1faee` (CC0-1.0; 204 entries; 127,842 bytes; SHA-256 `5343714eedd715f7f1f080f1e283d5ed56e6f6509ea3eff2f8f7c8c53ee066b9`) may be converted by our own generator and bundled in the core or runner. The SHA-1 column of MAME's `hash/a2600.xml` (CC0, same pin) may be bundled the same way, only to recognise and refuse 2600 images. CC0 asks for nothing, but the generated file's header names the source and pin, and `THIRD-PARTY-NOTICES.md` carries one courtesy line ("Cartridge data from MAME hash/a7800.xml (CC0-1.0), MAME software-list contributors, <commit>"). No other database enters in bulk.
- **Palettes** are host helpers outside the determinism hash. **Recommendation:** seed from a7800's "Trebor's Pro Palettes V7" (LUM3), which their author, Rob Tuccitto (trebor68), committed into the BSD-3 file himself (V3 in `f30781271bb8`, latest change `6aeb297b9c72`) [PV.02]. **Strongest alternative:** MAME's active 25.7° tables (`a7800.cpp` L484–623, BSD-3), whose PAL table is the NTSC hue rows in another order. **Would change if:** the owner wants no third-party notice; then an own generator from the wiki's colour documentation.
- **Measured constants** (crystals, line and frame lengths, DMA costs, DAC levels such as the TIA mix that MiSTer `top.sv` L578–581 derives from AtariAge thread 271920 [PV.15]) are facts. They are cited and restated in our own tables. A table copied verbatim from an MIT or BSD file is expression and becomes a §4 seed.
- **Test vectors.** A SingleStepTests sample is committed with its MIT notice; the full set is fetched nightly at the pin.

## 8. Vendored files
Vendored files are never edited. The ctest `libretro.vendored` (`tests/cmake/vendored.cmake`) checks each SHA-256 in this table, and each notice lives in `THIRD-PARTY-NOTICES.md`.

| File | Upstream | Commit | SHA-256 |
|---|---|---|---|
| `libretro/libretro.h` | RetroArch `libretro-common/include/libretro.h` (MIT, RetroArch team), copied from nesturbator's identical copy [PV.12] | `69a4f0ea1e8a` (v1.22.2) | `bd3398d29c3763d18617087020ff56b5450a48a63123403a4b674f8e79947acb` |

## 9. What this file does not do
It keeps the project's own licence clean. It is not legal advice, and it is no defence against a rights holder's claim; the asset policy handles that side by keeping game and firmware bytes out of the repository entirely.

## 10. Sources
| ID | Tier | Source (URL) | Pin or access date | Supports |
|---|---|---|---|---|
| PV.01 | T1 | github.com/mamedev/mame: `COPYING`, `docs/legal/BSD-3-Clause`, file headers, `src/mame/atari/a7800.cpp` L484–623 | `dddf4cd1faee` | whole GPL, per-file BSD-3, `hash/` CC0, palettes |
| PV.02 | T1 | github.com/7800-devtools/a7800: `LICENSE.md`, headers, tags, commit authors (`pokey.cpp`, `f30781271bb8`, `6aeb297b9c72`) | `7a2afdc1ea08` | per-file BSD-3, v5.2 offset, palette authorship |
| PV.03 | T1 | github.com/MiSTer-devel/Atari7800_MiSTer: `LICENSE` at `99ba8c7f08df`/`aef45f04b8dd`/`8c96e1f7cc24`, `README.md`, 152 `rtl/` headers, commit authors | `0dc8ad2e3ff7` | licence history, per-file classes, hand-off facts, embedded firmware |
| PV.04 | T1 | github.com/TASEmulators/BizHawk: `LICENSE`; `TIA_Sound` history; `444f90cde089`; EMU7800 `Core/LICENSE.TXT` at `a2ab02f9d905` | `c9de9a8bff0f` | MIT scope, TIA_Sound verdict, EMU7800 GPL-2 |
| PV.05 | T1 | github.com/aaronsgiles/ymfm `LICENSE`, `src/` headers | `81aec25ccbb9` | BSD-3 |
| PV.06 | T1 | github.com/floooh/chips `LICENSE`, `chips/m6502.h` L185 | `9e88298ce563` | Zlib |
| PV.07 | T1 | github.com/SingleStepTests/65x02 `LICENSE` | `2f6980a2d957` | MIT |
| PV.08 | T1 | github.com/cc65/cc65 `LICENSE`, `libsrc/atari7800/*` headers | `a6530f469142` | Zlib, Kaksonen 2022 |
| PV.09 | T1 | github.com/7800-devtools/7800basic `LICENSE.txt`, `includes/*` headers, `samples/` | `e7c18d2ecdc6` | GPL-2/CC0/BSL-1.0/LGPL-2 split, unknown includes |
| PV.10 | T1 | github.com/7800-devtools/7800OpenBIOS `README.txt`, `7800openbios.asm` L1–11, L164, L846–852 | `b8bc745a8744` | CC0, KiloParsec terms, RIOT loader |
| PV.11 | T2 | 7800.8bitdev.org `api.php` siteinfo `rightsinfo` (empty); "7800 NTSC BIOS" rev 1259 | accessed 2026-10-15 | no wiki licence; BIOS disassembly |
| PV.12 | T1 | github.com/szTheory/nesturbator `PROVENANCE.md`, `THIRD-PARTY-NOTICES.md`, `CMakeLists.txt` L115–138, `tests/cmake/check_archives.cmake` L32–50, git authors | `9f9386c661f6` | templates, archive check, same-owner seeds |
| PV.13 | T1 | nesturbator `.planning/preparation/DECISIONS.md` DEC.04, `NES-ECOSYSTEM.md` §3 | `8ae2c6e32e7f` | relicensing precedent |
| PV.14 | T1 | GitHub API `license` of test7800 (`ce8e29a987a1`), prosystem-libretro, js7800, Stella, ProSystem1_3, Klaus2m5 tests, NES_MiSTer; a7800 v5.2 and test7800 v0.9.0 release assets | accessed 2026-10-15 | GPL status; released oracle binaries |
| PV.15 | T4 | forums.atariage.com/topic/271920 "TIA sound abnormalities" (cited by MiSTer `top.sv`) | fetch returned 403, 2026-10-15 (unverified) | TIA mix measurements |
