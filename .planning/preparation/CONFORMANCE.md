---
title: "Conformance: test ROMs, oracles, diagnostics and the compatibility ledger"
summary: "Which 7800 test material muhtari may use under which licence, how each reports, how oracles are compared at VBLANK, what our own diagnostics measure, and how results ratchet in CI."
read_when: "Admitting a test ROM or fixture, writing diagnostics or the runner's result protocol, building the ledger/scoreboard, an oracle job, or a CI tier."
updated: 2026-10-15
---

# Conformance, oracles and the compatibility ledger

## Where to look
| Question | Section |
|---|---|
| What test ROMs exist, under which licence, and how do they report? | 1 |
| Are 7800basic ROMs reproducible? Does OpenBIOS build without KiloParsec? What does a minimal ROM need? | 2 |
| Which reference emulator, which tier, what to compare at VBLANK, how much noise? | 3 |
| Which diagnostics to write first, their result format, the hardware-run protocol | 4 |
| Ledger CSV, statuses, predicates, ratchet, local-only preset | 5 |
| What runs per PR, nightly, locally, and what it costs | 6 |
| May this fixture enter the repo? | 7 |

## 1. Test-ROM inventory
Disposition key: **commit** (every byte explicitly permissive), **build-ci** (built from a pinned source in CI, only hashes committed), **fetch** (scripted fetch at a pin, SHA-256 checked, never committed), **local** (owner-supplied via `MUHTARI_ROM_DIR`).

| Suite | Licence of the bytes → disposition | Size, pin | Proves / does not prove | Reports via | Tier |
|---|---|---|---|---|---|
| SingleStepTests 65x02 `6502/v1` [CF.08] | MIT → commit a sample | 256 files, 1,081,886,574 B, 10,000 tests each = 2,560,000 (tested); `2f6980a2d957` | per-cycle bus + final state of all 256 NMOS opcodes incl. decimal (99 of 199 sampled `69` tests have D set (tested)); not IRQ/NMI, RDY/HALT, slow cycles | JSON compare | PR sample, nightly full |
| Bruce Clark decimal test [CF.10] | "This code is public domain." → commit (transcribe the 6502.org listing, never Klaus's GPL copy) | ~1 KB (derived) | all ADC/SBC decimal cases incl. invalid BCD and NMOS N/V/Z | `ERROR` byte: 0 pass, 1 fail | PR |
| Klaus Dormann functional test [CF.09] | GPL-3.0 (repo LICENSE) → fetch | `6502_functional_test.bin` 65,536 B; `7954e2dbb49c` | documented opcodes and modes; its decimal test is Bruce Clark's code plus GPL-3 additions | trap loop (PC repeats); success address from its listing (unverified) | nightly, optional |
| muhtari diagnostics (section 4) | MIT → commit source and bytes | 4–32 KB each | MARIA/SALLY/RIOT timing | RAM mailbox + screen | PR |
| 7800OpenBIOS minus KiloParsec [CF.04] | BIOS asm CC0; KiloParsec all-rights-reserved, removed; draws the Atari Fuji logo → build-ci only, never commit or package (trademark caution [CF.31]) | 16,384 B; `b8bc745a8744` | BIOS boot path, cart-type check, hand-off with no Atari code | none (cart starts) | PR |
| 7800basic v0.41 samples [CF.01][CF.02] | compiler GPL-2 (tool only); generated asm CC0; includes mostly CC0, but `lzsa1compression.asm` is BSL-1.0 and `rmtplayer.asm`, `macro.h` state no licence [CF.31]; sample code, art and music unstated → build-ci, hashes only | 35 in `sizes.ref`, 32,896–131,200 B; tag `03aebb85206f` | boards (headers, tested): linear 32/48 K, POKEY@$450 32/48 K, SuperGame+RAM 128 K, +POKEY@$450, Banksets 64 K; controller bytes all say joystick, only `hiscoretable` sets a save device (SaveKey) | nothing → frame/audio-hash predicates | PR |
| RevEng "7800test" 2014 [CF.17] | AtariAge attachment, no licence; AtariAge blocks scripts → local | `7800test.bas.{ntsc,pal}.{bin,a78}` | DMA and CPU budgets; tests 6 and 13 need mid-line colour writes; video of two consoles exists | bars vs NTSC watermarks, joystick picks test | local |
| RevEng later ROMs [CF.17][CF.18][CF.19][CF.20] | same → local | `dmaholecycles.a78` 2023; `tiamariasync.bas` 2021; `6502fingerprint.bas.a78` 2025; utility cart 2015 ("no warranty") | hole cost; TIA+MARIA WSYNC; Sally undocumented opcodes (3 consoles: `8A55CE`); RAM/POKEY/HSC detection | screen | local |
| MiSTer "general_tests" [CF.07] | not public: cited only in `rtl/6502/sally.sv` comments; no ROM in the tree; no search hit | — | "test 15 counts a 46 cycle loop across the non vblank region and wants 556" (a value the RTL was *fitted* to) | loop counts | ask the author |
| Atari Diagnostic Test cart CB101195 [CF.11][CF.16][CF.25] | Atari service ROM, never sold, no licence → local | MAME `diagtest` 32 KB, SHA-1 `76ec76c4…`; prototype 16 KB (MAME: probably unsigned) | console self-test; needs the NTSC "Atari logo" BIOS (else code 14); passed MESS 0.153; may need TIA video [CF.22] (unverified) | screen: colour-adjust screen after ~10 s = pass; frowning face + 2-digit code = fail | local |
| Community probes [CF.14][CF.23][CF.26][CF.29] | no licences → local | `randomtest.a78`, `artifacts.a78`, `pokeytool.bas`, HSC save test, Trebor's hardware POKEY recordings | POKEY RANDOM, 320C artifacts, POKEY modes, HSC | screen / audio | local |
| Commercial library [CF.11] | copyrighted dumps → local; MAME hash list CC0 | 204 entries | games, via predicates (section 5) | — | local |

- **SALLY vs the vectors:** SST `8b` (ANE) and `ab` (LXA) fit only the magic constant $EE (399/399 and 398/398 sampled tests, tested); three real Sallys gave $00 for `LDA #0/LDX #$FF/ANE #$FF` on two units (one NCR drifts after warm-up) and $00 for `LAX #$FF` on all [CF.19].
- **Recommendation:** the magic constant is a CPU profile field; SST runs with $EE, a unit test pins the SALLY value from [CF.19]. **Strongest alternative:** list `8b`/`ab` (20,000 vectors) as permitted differences. **Would change if:** a Sally hardware run (4) shows another stable constant.
- **Recommendation:** SST sample = first 100 tests per opcode (25,600) in nesturbator's binary vector format (precedent ~1.68 MB) [CF.27]; full set nightly at the pin. **Strongest alternative:** every 100th test (needs the 1.08 GB download per PR). **Would change if:** nightly finds a failure the sample misses.

## 2. Experiments (all tested on 2026-10-15, macOS arm64 unless stated; each under 15 minutes)
**(a) 7800basic reproducibility.** v0.41 ships only WebAssembly binaries run by `wasmtime` [CF.01]; asset `7800basic-0.41-wasm.tar.gz` SHA-256 `62ef0a56…a7fe7` = GitHub digest; wasmtime v49.0.1 (published 2026-09-24) pinned by digest [CF.03]. `simple` and `pokey`, built twice in each of two working directories at different path depths, and again in Linux arm64 and Linux x86-64 (emulated) containers, gave identical bytes:

| ROM | Bytes | SHA-256 (.a78) | SHA-256 (.bin) |
|---|---|---|---|
| simple | 32,896 / 32,768 | `d09da3ed27fe82e0a4dd2821ff2cb9bde26c9531c59890788acb1cfdc4004ecf` | `03a8b7be5d4cd122382582a4f41a94e3104c92191ecdf0a664f93e6802e46b29` |
| pokey | 32,896 / 32,768 | `290bbd3ab77eb452d792d2e5d52c29b19004101611a62f9e1e5e1411c5fe8463` | `a0f10b15fdbf4e1804e5c1895d7a2b150edf169115bccd7078f64a14112d2aca` |

All 35 `sizes.ref` samples built in 12 s with sizes equal to the reference; no date strings in either ROM; the `7800sign` signature step is deterministic. Windows untested. `sizes.ref` and `make_test.sh` are in the repo, not the release package. The release tag is `03aebb85206f`; `e7c18d2ecdc6` is later master (2026-10-10).
- **Recommendation:** every PR leg downloads the pinned wasmtime and package (~13 MB), builds the 35 samples with `sh 7800basic.sh`, and checks each ROM's SHA-256 before any emulator hash, so toolchain drift is reported as drift. **Strongest alternative:** build once on Linux and share the ROMs through the Actions cache (never artifacts, section 6). **Would change if:** a Windows leg produces different bytes, or the build takes over 60 s on a leg.

**(b) 7800OpenBIOS without KiloParsec.** Replaced `incbin "kiloparsec.bl1"` / `.bl2` with `ds.b 8192,$FF` / `ds.b 896,$FF`: 16,384 B, SHA-256 `46b5bc2b716a20a1c40e2b920365c29cbab1594d01da94f4567b240181365cb2` from both dasm v2.20.17 (macOS arm64 release; prints "2.20.16") [CF.05] and 7800basic's `dasm.wasm` (2.20.15-SNAPSHOT); fill $00 gives `1ee4c780…ad4f`. Vectors NMI $FB37, RESET $F800, IRQ $FB64; $FFF8/$FFF9 = $FF/$F7; 96 B free. With no cart the BIOS jumps to `GameStart` → `JMP $CBCC` into the fill: use only with a cart. The `Start2600` block copied to RIOT RAM $0480 reads like a reproduction of Atari's loader (straight-line code with bytes kept after its `RTS`).
- **Recommendation:** build in CI from the pinned CC0 source with $FF fill and pin `46b5bc2b…`; never commit or package the binary, because it draws the Atari Fuji logo [CF.31]. **Strongest alternative:** commit a variant with the Fuji data also replaced by fill (no logo bytes; the `Start2600` question remains). **Would change if:** fetching the pinned source becomes unreliable in CI.

**(c) cc65 `atari7800`.** No release has the target (added `328781bf8a33` 2022-03-01; V2.19, 2020-11-20, is 3,903 commits behind the pin) → pin a commit and build from source: `bin` + `lib TARGETS=atari7800` in 4 s (-j8) [CF.06]. `cl65 -t atari7800` writes 32,768 B headerless; `--force-import __EXEHDR__` adds an A78 v3 header (32,896 B); the doc's "48K" and "header added automatically" are wrong at the pin. A minimal ROM needs: vectors at $FFFA/$FFFC/$FFFE; $FFF8 = $FF, $FFF9 = $F7 (hash $F000–$FFFF); a 120-byte signature at $FF80–$FFF7 [CF.21], which `7800sign` wrote exactly there, identical twice (`9b5424ac…`), needed for the NTSC BIOS but not for emulators [CF.06][CF.14]; startup `SEI, CLD, INPTCTRL=$07, CTRL=$7F, OFFSET=0, TXS` (the NTSC BIOS hands over with D set and INPTCTRL unlocked [CF.21]). crt0 links zlib runtime bytes; `conio_font.s`/`mono_font.s` state no glyph origin.

**(d) Oracles run here** (section 3): a7800 v5.2 and test7800 v0.9.0 in Linux amd64 containers.

**(e) Byte attribution from a dasm listing.** Counting byte-emitting lines per `FILE` section: `rmtplayer.asm` ("(c) Radek Sterba, Raster/C.P.U., 2002–2003") lands in `rmtromdemo`, `rmtramdemo`, `rmtramcompressiondemo`; `lzsa1compression.asm` ("Copyright John Brandwood 2021"; BSL-1.0 [CF.31]) in `rmtramcompressiondemo`; `i2c7800.inc` ("By Alex Herbert, 2004") in `hiscoretable`; `simple` and `pokey` emit none. 23 of 33 include files carry a CC0 notice, 10 do not [CF.02]. `rmtromdemo` also carries tunes whose file names suggest covers (`axel_f`, `popcorn`, `frogger`). The only sample licence line is in `trackertest.bas` ("Feel free to use the other samples … public domain", Salvo theme excluded) [CF.02].

## 3. Oracles
| Oracle | Licence, binary | BIOS | Headless facts | Effort | Tier |
|---|---|---|---|---|---|
| a7800 v5.2, tag `4dbf85ea90fb` [CF.13] | GPL-2+ as a whole, BSD-3 per-file headers; master `7a2afdc1ea08` = v5.2 + 25 palette commits [CF.31]; Linux x86-64 ELF needing SDL2, SDL2_ttf, Qt5, GL, ALSA, fontconfig | optional: runs after "OPTIONAL 7800.u7 NOT FOUND" (tested) | (tested) under Xvfb: `-noreadconfig -video none -sound none -nothrottle -seconds_to_run N -autoboot_script p.lua`; the SDL dummy driver fails ("video_init: Initialization failed!") even with `-video none`; the shipped ini needs `-noreadconfig`. Lua: `emu.register_frame_done`, `cpu.spaces["program"]:read_u8`, `cpu.state["PC"].value`, `emu.time()`, `screen:frame_number()` work; `machine.time`, `screen:vpos()` do not; `video:snapshot()` wrote no file, the exit snapshot did (320×243 PNG). Also `-bench`, `-record/-playback`, `-wavwrite`, `-state`. Three runs: identical RAM hashes at frames 1/60/120 and identical PNG; 4.9–6.4× real time in an emulated container | ~1 day (derived) | nightly |
| MAME upstream `dddf4cd1faee` [CF.12] | GPL-2+ build; a7800 driver BSD-3 | required: `7800.u7` 4 KB or `c300558-001b.u7` 16 KB with no OPTIONAL flag → will not start without them (derived) | same options, newer Lua API (unverified) | low | local |
| test7800 v0.9.0, `ce8e29a987a1` [CF.14] | GPL-3; Linux amd64, Windows, macOS binaries | `-bios=false`, `-checksum=false` | (tested) needs a display even for `-help` (ebiten/GLFW panic); stdin debugger (`CPU`, `RAM7800`…, no frame step); no seed flag and power-on registers differ per run (A=$2F, then $B2) → not deterministic | high | local, manual third opinion |
| MiSTer RTL via Verilator [CF.07] | MIT only since 2026-08-30 (pin OK; `sally.sv` header MIT); `TIA.sv`, `M6532.sv`, `cart.sv` facts-only; ships Atari firmware (`rtl/mem4.hex` HSC ROM, `rtl/ar.hex` Supercharger BIOS, RIOT-RAM loader bytes in `M6532.sv`) → a sim build never packages or commits them, and one that needs them is local-only [CF.31] | `bypass_bios` | 75 .sv, 36 .v, 5 .vhd (`dpram.vhd`, T65; the 7800 CPU is now `mos6502*.sv` + `sally.sv`); stub `pll`, `cache_ram.v`, `sdram.sv`, `ddram.sv`, `arm_mapper_memory.sv`, `jt51`; clk_sys = PLL output 0 = 14.318182 MHz → 238,804 cycles per NTSC frame (derived); speed unmeasured | 3–5 days (derived) | v4 nightly |
| BizHawk A7800Hawk, `c9de9a8bff0f` (2.11.1) [CF.15] | MIT core (not `TIA_Sound/`); Linux x86-64 needs Mono "complete", OpenAL, Lua 5.4; no macOS | required (`MissingFirmwareException`) | `--lua=/abs/path`, `--chromeless`, `--socket-ip/port`, movies; needs a display | medium | local |

**What to compare at each VBLANK start** (muhtari's frame boundary). The runner writes one record per frame.

| Field | muhtari | a7800 | RTL (v4) | Expected noise vs muhtari |
|---|---|---|---|---|
| RAM SHA-256 over $1800–$27FF + $0480–$04FF (+ cart RAM) | yes | raw bytes via `read_u8` to a file, hashed outside Lua | yes | a7800's callback fires once per 0.016678381 s from power-on (tested; = 119,402 ticks, derived), not at MSTAT's edge; a different boot path; approximate DMA/CPU timing → report the first divergent frame after the best alignment k ∈ [−2, 2]; RTL: zero expected |
| Frame SHA-256 over native index bytes | yes | 320×243 RGB PNG, mapped back through the shared palette and cropped to 242 rows | yes | crop/palette; static screens only |
| CPU registers PC A X Y S P | yes | PC only | yes | high for a7800 (informational) |
| Ticks, phase-2 cycles | yes | — | yes | RTL exact |
| Audio transition-stream hash | yes | `-wavwrite` | yes | not compared before v4 |

- **Recommendation:** a7800 v5.2 nightly on Linux x86-64 under Xvfb as the routine oracle; its RAM-hash agreement is a locator written to the ledger, never a gate (approximate timing). MiSTer RTL becomes the timing referee in v4. **Strongest alternative:** BizHawk A7800Hawk (per-tick model, MIT, built for TAS determinism), held back by its BIOS requirement. **Would change if:** most a7800 disagreements on the samples turn out to be a7800's errors, or an RTL harness proves cheaper than 3 days.

## 4. Own diagnostic ROMs (MIT)
Ranked by correctness per hour. Most MARIA tests check themselves through **stolen CPU time**: the RIOT timer keeps counting while SALLY is halted (113 timer ticks per WSYNC line with DMA on or off [CF.24]), so INTIM, or a loop count across the visible region, measures DMA cost with no pixel readback.

| # | Test | Value reported | Expected from | Hardware-checkable | Catches |
|---|---|---|---|---|---|
| 1 | DMA cost: NULL DL, 4/5-byte headers, widths 1–32, direct vs indirect 1/2-byte, last line vs others | iterations of a fixed loop across the visible region per DL setup; per-line INTIM | Software Guide App. 3; NULL-DL 16/24 ticks measured [CF.17]; MiSTer fit (test 15 = 556) | yes | Choplifter, Xenophobe, Summer Games |
| 2 | Slow access | INTIM after 64 × `LDA` from TIA $00, RIOT RAM $0480, RIOT $0280, RAM $1800, DMA off | +2 ticks per slow access, data cycle only [CF.17]; RIOT RAM slow [CF.21] | yes | timing everywhere |
| 3 | RIOT timer edges | TIM1T/8T/64T/1024T written once vs twice; INTIM at cycle offsets; post-underflow decrement; TIMINT set/clear | datasheet; "write twice" [CF.17]; $8E per line [CF.24] | yes | Beef Drop, One on One, PAL Xevious |
| 4 | DLI arrival vs WSYNC | INTIM read in the DLI after a WSYNC-started timer; zones 1, 2, a first-zone DLI, CPU parked in WSYNC | Software Guide: 1 tick after DMA shutdown | yes | Xenophobe, Xevious |
| 5 | DL terminator | DMA cost after a header whose byte 1 ∈ {$00,$20,$40,$80,$A0,$C0} | wiki: MARIA ignores bits 7 and 5, so the end test is `!(value & 0x5F)` [CF.30] | yes | Ninja Golf, Crossbow |
| 6 | Holey DMA | cost of 8 objects in a hole, 4- vs 5-byte headers | +3 ticks per object [CF.17] | yes | Scrapyard Dog |
| 7 | 5-byte width 0 | cost of width 0 vs 31 | 0 = 32 bytes (design spine; unverified) = 96 ticks (derived) | yes | — |
| 8 | MSTAT edges | WSYNC-counted line + poll count until bit 7 flips, both edges | `VBLANK_START` 258 (PAL 308) in MiSTer `video_sync.sv` [CF.07]; flips mid-line [CF.22] | yes | VBLANK-polling games |
| 9 | DMA overrun | objects completed as objects are added; picture | objects cut when the budget ends [CF.17] | photo | overloaded DLs |
| 10 | Kangaroo, 320B pair transparency, write mode | picture only → frame hash | RTL frame / hardware photo | photo | 320 modes |
| 11 | INPTCTRL two-write HALT gate | stolen cycles after one vs two INPTCTRL writes | Software Guide | no: the BIOS writes INPTCTRL first → `--boot fast` only | boot |

**Mailbox** at $2200–$22FF (plain RAM, outside the $2040–$20FF/$2140–$21FF mirrors):

| Address | Content |
|---|---|
| $2200–$2203 | magic `M7DG` |
| $2204 | protocol version 1 |
| $2205 | status: $80 running, $81 waiting for a switch or joystick, $00 done |
| $2206 / $2207 | records written / ROM id |
| $2208–$2209 | CRC-16 of $2210–$22FF |
| $2210 + 4n | record n: test id, flags (bit 0 valid, bit 1 timed out), value (16-bit LE) |

The runner peeks at each VBLANK start, stops at status $00 or a frame cap (cap = `fail`, code "hang"), and judges values against `tests/diag/expected.csv` (rom, test, value or range, source `hw|guide|rtl|oracle`); the ROM only measures, so the same bytes serve hardware runs. The screen shows one 160A row per record, `NN VVVV`, in our own 16 hex glyphs, plus the CRC-16 in double-height digits so a photo transcription can be checked.
- **Recommendation:** measured values in a RAM mailbox and on screen, verdicts outside the ROM. **Strongest alternative:** a blargg-style single result code decided inside the ROM (simpler runner, but a hardware run then reports pass/fail instead of the number we need). **Would change if:** expected values become settled and stable, so in-ROM verdicts lose nothing.

**Toolchain.**
- **Recommendation:** dasm plus `7800sign`/`7800header`, all from the pinned 7800basic package via wasmtime (one toolchain on six platforms; reproducible in 2a/2b); commit source and signed `.a78`; one Linux job rebuilds and compares bytes. **Strongest alternative:** ca65/ld65 at a pinned cc65 commit (zlib, 4 s build, explicit segments, C available, no runtime linked). **Would change if:** a test needs C, or dasm macros become a burden.

**Owner hardware-run protocol.**
1. Copy the signed ROM set and `tests/hw/session-template.csv` to the flash cart (Concerto, Dragonfly, 7800GD…).
2. Record once per session: region and board revision, BIOS ("Atari logo" NTSC or "Asteroids" PAL), Sally maker if known (NCR/Rockwell), cart model and firmware, video path, minutes since power-on.
3. Cold-boot each ROM three times (power-on variance: TIA-WSYNC alternation on ~50 % of boots [CF.18]); photograph the status screen; video for moving tests.
4. Transcribe values into the CSV; the runner checks them against the on-screen CRC; commit the CSV only. Photos stay local unless EXIF-stripped (location data) and approved.
5. Caveats: flash carts may release POKEY from reset at boot and add bus delay (one cart corrupted text that DragonFly did not [CF.18]), so tests initialise every device and assume no power-on state; NCR Sally ANE drifts after 1–2 min [CF.19].
- **Recommendation:** transcribed CSV checked by the on-screen CRC. **Strongest alternative:** commit EXIF-stripped photos as evidence. **Would change if:** outside contributors start submitting hardware results (photos then become the audit trail).

## 5. Ledger and scoreboard
`tests/compat/ledger.csv`: one header line, comma-separated; no field may contain a comma, quote or newline (a schema test enforces it, so sh and `cmake -P` parse it).

| Column | Meaning |
|---|---|
| `id` | stable key: MAME short name, `7800basic-0.41/simple`, `diag/dma-cost` |
| `title` | display name |
| `sha1_raw`, `sha256_raw` | headerless image (MAME's list keys on headerless SHA-1 [CF.11]) |
| `sha1_a78`, `sha256_a78` | headered file as identified; empty if none |
| `board`, `region` | mapper (A78 v4 name or MAME slot) · `NTSC`, `PAL`, `any` |
| `peripherals` | `+`-joined: `pokey450`, `hsc`, `savekey`, `lightgun`, `paddles`, `xm`, `ym2151`… |
| `predicate` | `boots@F` (weak: DMA on, ≥ 2 distinct frames, CPU not jammed), `frame@F=H`, `ram@F:$ADDR=VV`, `mailbox@F`, `audio@F=H`; prefix `movie:<file>/` for scripted input |
| `status` | `pass`, `fail`, `unknown` (never evaluated; seed rows), `unsupported` (needs an unclaimed board or peripheral; names it), `skipped` (deliberately not run; reason; never in CI tiers), `infrastructure_error` (harness or oracle broke; CI red, neither pass nor fail) |
| `tier` | `pr`, `nightly`, `local` |
| `evidence_commit`, `evidence` | muhtari commit (12 hex) of the last evaluation · 16-hex hash prefix, mailbox CRC or run URL |
| `oracle`, `checked` | e.g. `a7800-5.2:agree@600`, `rtl:diverge@312`, `-` · ISO date |

- **Generated doc:** `docs/compatibility.md` comes from the CSV through a `cmake -P` script (counts by status × board × region, then one row per title); `policy.compat_doc_fresh` regenerates and compares.
- **Ratchet:** key = (`id`, `predicate`). Allowed: `unknown` → anything, `fail` → `pass`, `pass` → `pass`. `pass` → anything else, or a changed predicate on a passing row, needs a dated line in `tests/compat/allow.txt` that expires within 60 days (perf precedent). CI fails when fresh results and the ledger disagree in either direction, so an improvement is committed in the same PR.
- **Protected baseline:** the job's first step checks out `github.event.pull_request.base.sha` into `baseline/` and copies `tests/compat/ledger.csv` and `scripts/compat-ratchet.sh` to `$RUNNER_TEMP`; only then is the PR checked out. Main's script judges main's ledger against the PR's results, so a PR cannot loosen its own judge.
- **Local-only preset:** `scripts/library.sh` loads gitignored `.env.local` (`MUHTARI_ROM_DIR`, optional `MUHTARI_BIOS_DIR`) and runs `cmake --workflow --preset library`. Configure fails if the directory is missing (no skip). It registers `library.<id>` tests only for files whose headerless SHA-1 is in the ledger, then writes `build/library/results.csv`, which `scripts/compat-update.sh` merges with `evidence_commit`. `ci*` presets never define `MUHTARI_ROM_DIR` or register library tests, so the exact test inventory and `policy.no-skip` hold. Hygiene rejects `.env.local`, ROM paths and ROM bytes.
- **Recommendation:** the CSV above with a generated doc. **Strongest alternative:** nesturbator's sorted TSV `key status code frames hash` [CF.27] (tabs remove the no-comma rule). **Would change if:** titles must keep commas (switch to TSV) or a dashboard consumer appears (emit JUnit as well).

## 6. CI tiers and cost
| Tier | Contents | Estimated minutes | Basis |
|---|---|---|---|
| Per PR × 6 (ubuntu x64/arm64, macOS arm64/x64, Windows x64/arm64) | units; SST sample; Bruce Clark; diagnostics (committed bytes); OpenBIOS built from the pin, then booted; 7800basic samples built on each leg + ROM SHA-256 pins + frame/audio hashes; fuzz-corpus replay | +1–2 per leg over the build baseline (precedent: slowest leg 679 s) | sample build 12 s (tested, M-series); ~13 MB downloads; ~28,800 emulated frames at an assumed ≥ 2,000 fps ≈ 15 s (hypothesis) |
| Per PR × 1 (Linux) | diagnostic rebuild-and-compare; ratchet; doc freshness | < 1 | — |
| Nightly | SST full (~190 MB transfer by precedent); Klaus (fetch); a7800 differential over samples + diagnostics; fuzz 2 × 60 s; big-endian qemu; v4: Verilator | 10–25 | a7800 4.9–6.4× real time in emulation (tested); apt ~1 min (derived) |
| Local | library preset; real-BIOS boots; Atari diagnostic cart; forum test ROMs; upstream MAME, BizHawk, test7800; RetroArch session; hardware sessions | owner time | — |

**Measure before gating:** a new lane runs report-only (outside `CI required`) for its first 10 runs on main. It then gets a timeout of 2× its slowest of the last 40 successful runs, with the run ID in a comment (precedent [CF.27]), and joins the roll-up. A lane over budget moves down a tier rather than being trimmed blindly. Oracle agreement never gates. Built third-party ROMs are never uploaded as artifacts: anyone with read access to a public repo can download artifacts, kept 90 days by default [CF.28].
- **Recommendation:** the tiers above. **Strongest alternative:** run the a7800 differential on one Linux leg per PR (~3 min, derived). **Would change if:** nightly a7800 catches a regression the PR tier missed more than once in a milestone.

## 7. Admission rules for third-party fixtures
1. Judge the **bytes that end up in the ROM**: test or game source, runtime/kernel, compiler libraries (cc65 crt0 = zlib), fonts, graphics, music, generated bytes (headers and signatures are tool output), and BIOS-derived code. A tool's own licence (dasm GPL-2, `7800sign` LGPL-2, 7800basic compiler GPL-2) does not reach its output unless the tool copies its own runtime in.
2. Licence evidence = LICENSE plus per-file headers **at the exact pin**. A repo-level grant does not cover files with their own © line (2e). For dasm builds, attribute bytes by listing `FILE` sections that emit bytes (2e method). Re-check at every pin bump (a sibling once cited a GPL-3 commit as zlib [CF.27]).
3. Disposition: **commit** only if every byte source is MIT/BSD/zlib/CC0 or explicitly public domain and no third-party logo is drawn; **build-ci** for unstated or mixed licences and for trademark-bearing bytes (OpenBIOS's Fuji logo): commit hashes only, no artifacts; **fetch** for copyleft binaries (Klaus, GPL-3); **local** for Atari ROMs/BIOS, commercial dumps and anything not script-fetchable (AtariAge attachments).
4. Firmware inside permissive repos stays out: MiSTer's `mem4.hex` (HSC ROM), `ar.hex` (Supercharger BIOS) and `M6532.sv` loader bytes are Atari's [CF.31].
5. Manifest row, as in the sibling [CF.27]: `path<TAB>source<TAB>pin<TAB>SPDX<TAB>sha256`; SPDX `NOASSERTION` when unstated and `LicenseRef-PublicDomain` for an explicit public-domain statement. build-ci rows add a recipe line (tool + digest + command) and the expected output SHA-256.
6. Our own ROMs: MIT source, committed bytes, own glyphs (no cc65 fonts), signed with `7800sign`, rebuilt and compared in CI.
- **Recommendation:** rules 1–6. **Strongest alternative:** admit by repository licence alone (simpler, but it would wrongly pass the 7800basic samples, 2e). **Would change if:** upstreams publish per-file SPDX tags, which a script could then check directly.

## 8. Open questions
- `general_tests`: does it exist publicly, under what licence, and is "test 15 = 556" from hardware or fitted? Ask the MiSTer core author (issue).
- REUSE: is OpenBIOS's `Start2600` block clean? This matters only if a logo-free OpenBIOS variant is ever committed.
- Owner: which flash cart(s) and console revisions are available, and is a PAL unit available?
- Klaus success address and whether SST alone suffices (drop Klaus if the nightly never adds a finding in a milestone).

## Sources
| ID | Tier | Source (URL) | Pin or access date | Supports |
|---|---|---|---|---|
| CF.01 | T1 | github.com/7800-devtools/7800basic (LICENSE.txt, README.txt, samples/sizes.ref, make_test.sh, release v0.41 + asset digests) | tag v0.41 `03aebb85206f`; master `e7c18d2ecdc6` | licences, wasm, sizes |
| CF.02 | T1 | 7800basic-0.41-wasm.tar.gz contents (includes headers, samples, Guide PDF) | SHA-256 `62ef0a56…a7fe7` | byte attribution |
| CF.03 | T1 | github.com/bytecodealliance/wasmtime/releases/tag/v49.0.1 | v49.0.1 | runtime |
| CF.04 | T1 | github.com/7800-devtools/7800OpenBIOS (README.txt, 7800openbios.asm) | `b8bc745a8744` | CC0, layout |
| CF.05 | T1 | github.com/dasm-assembler/dasm/releases/tag/v2.20.17 | v2.20.17 | assembler |
| CF.06 | T1 | github.com/cc65/cc65 (cfg/atari7800.cfg, doc/atari7800.sgml, libsrc/atari7800/*) | `a6530f469142` | target |
| CF.07 | T1/T3 | github.com/MiSTer-devel/Atari7800_MiSTer (LICENSE, README, rtl tree, rtl/6502/sally.sv, pll) | `0dc8ad2e3ff7` | RTL, general_tests |
| CF.08 | T1 | github.com/SingleStepTests/65x02 (6502/README, git tree sizes, 69/8b/ab sampled) | `2f6980a2d957` | vectors |
| CF.09 | T1 | github.com/Klaus2m5/6502_65C02_functional_tests (license, readme.txt, bin_files) | `7954e2dbb49c` | GPL-3 |
| CF.10 | T1 | 6502.org/tutorials/decimal_mode.html (Appendix B) | accessed 2026-10-15 | public domain |
| CF.11 | T2 | github.com/mamedev/mame hash/a7800.xml (CC0) | `dddf4cd1faee` | diagtest, 204 entries |
| CF.12 | T1 | github.com/mamedev/mame src/mame/atari/a7800.cpp (BSD-3) | `dddf4cd1faee` | BIOS required |
| CF.13 | T1 | github.com/7800-devtools/a7800 (README, LICENSE.md, release v5.2 Linux binary run) | tag v5.2 `4dbf85ea90fb`; master `7a2afdc1ea08` | oracle |
| CF.14 | T1 | github.com/JetSetIlly/test7800 (README, release v0.9.0 Linux binary run) | `ce8e29a987a1` | oracle |
| CF.15 | T1 | github.com/TASEmulators/BizHawk (README, ArgParser.cs, A7800Hawk.cs) | `c9de9a8bff0f` | oracle |
| CF.16 | T2 | atari7800.org/manuals/Diagnostic.htm | accessed 2026-10-15 | diag cart codes |
| CF.17 | T4 | forums.atariage.com/topic/224025 "7800 hardware facts" | accessed 2026-10-15 | 7800test, NULL-DL, holes, slow access, timers |
| CF.18 | T4 | forums.atariage.com/topic/318358 (tiamariasync) | accessed 2026-10-15 | WSYNC, cart effects |
| CF.19 | T4 | forums.atariage.com/topic/385521 "fingerprinting 6502 processors" | accessed 2026-10-15 | Sally ANE/LXA |
| CF.20 | T4 | forums.atariage.com/topic/233645 "7800 Utility Cart" | accessed 2026-10-15 | utility cart |
| CF.21 | T4 | forums.atariage.com/topic/222380 "7800 Software Guide updated" | accessed 2026-10-15 | signature area, BIOS hand-off, RIOT RAM |
| CF.22 | T4 | forums.atariage.com/topic/285921 "BupSystem" | accessed 2026-10-15 | MSTAT mid-line, test list |
| CF.23 | T4 | forums.atariage.com/topic/383566 (test7800 thread) | accessed 2026-10-15 | randomtest.a78 |
| CF.24 | T4 | forums.atariage.com/topic/145363 "Improving the emulators" | accessed 2026-10-15 | RIOT $8E per line |
| CF.25 | T4 | forums.atariage.com/topic/224052 (MESS 0.153) | accessed 2026-10-15 | diag cart passes |
| CF.26 | T4 | forums.atariage.com/topic/293427 (POKEY AUDCTL) | accessed 2026-10-15 | hardware recordings |
| CF.27 | T1 | github.com/szTheory/nesturbator (.planning/preparation/CONFORMANCE.md, tests/roms/manifest.txt, CI notes) | `db0f21c020c4` | precedent |
| CF.28 | T1 | docs.github.com/en/actions/how-tos/manage-workflow-runs/download-workflow-artifacts | accessed 2026-10-15 | artifact access |
| CF.29 | T4 | forums.atariage.com/topic/349547 "320 mode colours question" | accessed 2026-10-15 | artifacts.a78 |
| CF.30 | T2 | 7800.8bitdev.org/index.php/Common_Emulator_Development_Issues | oldid 1304 | terminator, DMA games |
| CF.31 | T1 | muhtari provenance audit (REUSE research file), relayed by the coordinator | 2026-10-15 | MiSTer, 7800basic, OpenBIOS, a7800 licences |
