---
title: "AtariAge forum extracts for muhtari (Atari 7800)"
summary: "Facts harvested from 30+ AtariAge 7800 threads (DMA, HALT, slow cycles, WSYNC, MSTAT, INPTCTRL, BIOS hand-off, palettes, artifacting, light gun, POKEY, audio mix, emulator accuracy, user wishes, test ROMs, licences) with handle, date and evidence type per claim."
read_when: "Before citing or relying on any AtariAge forum claim (tier T4) about 7800 hardware behaviour, emulator accuracy, test ROMs or palette/test-ROM licences."
updated: 2026-10-15
---

# AtariAge extracts (Atari 7800 only)

Fetched 2026-10-15 with Claude in Chrome: one private tab, read-only same-origin GETs of every topic page, parsed post by post (author handle, ISO post date, text; long quotes collapsed). No Wayback fallback was needed. Attachments (ROMs, captures, audio, images, videos) were **not** downloaded or viewed. Handles appear as shown on the forum, minus the "+" subscriber badge. All forum claims are tier T4 unless a section says otherwise; the posters' wording is paraphrased, and quotes are kept to short numeric phrases.

**Evidence tags:** [HW] = measured or observed on real hardware · [EMU] = observation of an emulator (behaviour, internals or changelog) · [OP] = opinion or inference · [doc] = quotes Atari/GCC documentation · [schematic] / [BIOS disassembly] = read from those · [wish] / [complaint] = user request or pain point · "derived" = arithmetic done by this harvester.

## 0. Quick reference: hardware facts (details and caveats in the cited sections)

| Topic | Fact | Evidence | § |
|---|---|---|---|
| Line / frame | 454 MARIA ticks = 113.5 CPU cycles per line; NTSC 263 lines = 29 850.5 CPU cycles per frame; PAL 313 lines (PAL line measured 64 µs ±5 ns) | HW (test ROM; logic analyser) | 1, 18 |
| Clocks | NTSC crystal 14.31818 MHz; PAL 14.187576 MHz (read off boards); PAL colour carrier from a separate 4.433 MHz crystal | HW / schematic | 1, 18 |
| CPU phase | CPU clock edge vs CSYNC moves line to line (half-cycle offset per line) | HW (PAL analyser) | 18 |
| Slow cycles | every TIA ($00–$1F) and RIOT access (RIOT RAM included) runs at 7.16/6 MHz, costing +0.5 effective CPU cycle; only the bus cycle that touches TIA/RIOT is slow | HW | 1, 3a, 12 |
| RIOT timer | keeps counting through WSYNC and DMA HALT (113 ticks per WSYNC-to-WSYNC line, DMA on or off); its ticks stretch with slow cycles; write it twice for reliability; keeps decrementing after underflow | HW + EMU | 5, 7, 1, 12, 19 |
| HALT | HALT falls ≈28 ticks (= 7 CPU cycles) into each line, just after PHI2 falls; Sally completes ≥1 more cycle; R/W high while MARIA owns the bus | HW (scope) | 19 |
| DMA overhead | zone pointing to an empty DL: 16 ticks on other lines, 24 ticks on the zone's last line (startup + shutdown included) | HW | 1 |
| DMA costs | header 2 ticks/byte (4-byte 8, 5-byte 10); direct graphics 3/byte; a holey object costs header + one graphics byte (3) | HW (two testers) | 1, 2, 12 |
| DMA budget | ≈410 (other line) / ≈402 (last line) ticks for DL work, minus up to 6 for HALT alignment; overflow truncates objects mid-line | derived + EMU | 18, 7 |
| MSTAT | the VBLANK bit changes part-way through a scanline, not at its boundary | HW | 19 |
| WSYNC | MARIA and TIA WSYNC both halt the CPU; separate horizontal counters; TIA, MARIA and CPU RDY pins are tied together; WSYNC can delay a DLI | HW + schematic | 10, 3b |
| INPTCTRL | write-only latch decoded on $00–$1F; BIOS leaves it unlocked for 7800 carts (writes $16) and locks it for 2600 carts; until locked, writes go to TIA as well | schematic + BIOS disasm | 10, 12 |
| BIOS hand-off | NTSC BIOS 7800 path: `LDX #$16 / STX INPTCTRL / TXS / SED / JMP ($FFFC)`, i.e. S = X = $16 and decimal flag set | BIOS disasm | 12 |
| Palette regs | write-only; reads return $00 or noise in every addressing mode | HW | 5, 19 |
| Controllers | SWCHB output latch is $FF after reset (one-button mode); two-button mode = SWBCNT $14 + SWCHB bits; strong pull-up on pin 5 in two-button mode; paddle lines float | HW / schematic | 19, 9 |
| Colour | hue step ≈24° at cold power-on, ≈25.7° after ~2 min, 26.7–28.7° warm/hot; internal colour pot also shifts it | HW obs | 8 |
| Audio mix | cartridge audio enters the mixer via 6.8 kΩ, TIA via 18 kΩ; TIA volume response is non-linear | HW | 20, 9 |
| POKEY | stays silent until SKCTL bits 0–1 are set; a cart POKEY's pitch drops when the CPU does slow cycles; MAME/a7800 RANDOM sequence is right | HW + EMU | 7, 11, 3a, 14 |
| Light gun | games time a screen flash; *Crossbow* polls ≈16×/line (≈20 px X resolution); a7800 converts the frame cycle at the sense read into a beam X,Y | OP + EMU | 13, 7 |
| Sally variants | identical ARR fingerprint on all units tested; the ANE "magic constant" is $00 on most units but unstable and warm-up dependent on some NCR parts | HW | 17 |

## Where to look

| Question | Sections |
|---|---|
| MARIA DMA cycle costs, holes, budget, overrun | 1, 2, 12, 18, 7 |
| HALT latency, CPU phase jitter, DMA-end rounding | 19, 18, 10, 3a |
| Slow cycles, RIOT timers, RIOT RAM | 1, 3a, 5, 7, 12, 19 |
| WSYNC, DLI, MSTAT | 3b, 10, 19, 2 |
| INPTCTRL, BIOS hand-off, power-on state | 10, 12, 19 |
| Palettes, colour pot, artifacting, overscan | 8, 21, 14, 19 |
| Light gun, paddles, keypad, two-button pads, QuadTari | 13, 7, 6, 9, 19, 14 |
| POKEY (clocking, reset, two-tone, RANDOM, dual), audio mixing | 11, 14, 7, 5, 3a, 20, 9 |
| Which emulator is most accurate, and why | 4, 6, 7, 9, 14, 16, 19 |
| User complaints and wishes | 6, 7, 14, 16, 22, 5 |
| Sally variants and undocumented opcodes | 17 |
| Atari 7800+ (2024 hardware) compatibility | 15, 16, 9 |
| Test ROMs and diagnostics | 1, 10, 3b, 11, 14, 17, 21, 22, 19 |
| Licences | end of file |

## 1. "7800 hardware facts"
- URL: https://forums.atariage.com/topic/224025-7800-hardware-facts/ (Atari 7800 Programming)
- Dates: 2014-04-06 → 2023-08-31 · Pages read: 1–2 of 2 (all)
- Test ROMs attached to OP: `7800test.bas.{ntsc,pal}.{bin,a78}` (joystick L/R = test, U/D = move level mark; NTSC watermarks only). 2023: `dmaholecycles.a78` (RevEng); bsteux posted C (cc7800) source of a holey-DMA cost test.

- [HW] RevEng 2014-04-06: NTSC 263 lines/frame, PAL 313; crystals NTSC 14.3181818 MHz, PAL 14.187576 MHz. The *later* GCC MARIA spec's DMA cycle-stealing numbers are "closest to the truth".
- [HW] RevEng 2014-04-06: RIOT timers must be written twice to behave consistently on real hardware; hitting WSYNC during a countdown made them unreliable (cause unknown). SeaGtGruff 2014-04-11 [EMU/HW recollection]: his bB timer test misbehaved on 7800 in T1024T mode.
- [HW] RevEng 2014-04-11: PAL test levels (run by gambler172) match NTSC levels.
- [HW] RevEng 2014-05-03 (after correcting a test bug): "a line length is exactly 113.5 CPU cycles"; "an NTSC frame is exactly 29850.5 CPU cycles" (263 × 113.5). No CPU clock sync when DMA is off. Any access to $00–$1F (TIA range, all addressing modes, even `nop $00`) costs +0.5 effective CPU cycle; the slowdown applies only to the data-fetch cycle of the instruction, not the whole instruction.
- [OP] Rybags 2014-05-02: 7800 shares the Atari 8-bit computers' master clock; 263 × 113.5 = 29850.5 CPU cycles/frame.
- [HW] RevEng 2014-05-04: DLL zones pointing to empty (NULL) DLs: "other line" DMA = 4 CPU cycles = 16 MARIA cycles; "last line of zone" = 6 CPU cycles = 24 MARIA cycles (both include startup + shutdown; 2-cycle NOPs used around DMA). Doc values for comparison: GCC startup 5–12, LL shutdown 19–23, OL shutdown 13–17; Atari startup 5–9, LL shutdown 10–13, OL shutdown 4–7. Real hardware is below GCC, near the upper end of Atari's figures. Caveat: RevEng also called the *later* GCC spec "closest to the truth" (04-06) and rebuilt the guide's DMA table from "the more accurate GCC spec" (05-01, §12). Which GCC document his 05-04 comparison figures come from is not stated: per-byte costs follow GCC, while startup + shutdown overhead sits at the top of Atari's ranges.
- [OP] Rybags 2014-05-04: MARIA uses /HALT, so it need not wait for an instruction to finish (only /RDY allows up to 3 more CPU writes); startup variance may come from the CPU being in a slow (TIA/RIOT) cycle. RevEng 2014-05-05 [OP]: shutdown range is likely MARIA/6502 clock-phase alignment; slow TIA/RIOT cycles affect startup only.
- [HW] nichtsnutz 2014-05-05: logic-analyser trace of a DL fetch (probably Pole Position II): HALT low, two 4-byte DL headers + their graphics fetched, then the terminating entry, HALT high, CPU resumes. LogicPort `.lpf` capture attached (`MARIA_DL4_SHORT_2.LPF.TXT`).
- [EMU] Shannon 2014-05-06: in A7800x he had to delay DMA start to make *M.I.A.* and *Kung Fu Master* work.
- [EMU] AlyoshaTAS 2017-07-22/23 (writing BizHawk's new 7800 core to replace EMU7800): with DMA implemented, Xenophobe, Kung Fu Master and Centipede's top line are the usual "troublemakers"; RevEng's tests 6 and 13 need mid-scanline colour changes. *One On One Basketball*: timer runs too fast in emulators while players move at correct speed.
- [HW] Trebor 2017-07-23: video of the full 7800test run on two real consoles, identical results: https://www.youtube.com/watch?v=Z8txsnlBSWo (reference for expected marks). Trebor credits this research for MAME's 7800 accuracy.
- [HW] RevEng 2023-08-19: Software Guide claim "no DMA penalty in a hole" is FALSE. Test: 8 objects (5-byte headers) with graphics in a hole cost 24 MARIA cycles more than predicted → MARIA pays for the first graphics byte (3 cycles), discovers the hole, aborts the object. Cost model used: 5-byte header = 10 cycles (2/byte), direct graphics = 3 cycles/byte. a7800 already modelled this (`src/mame/video/maria.cpp` ~L273). Software Guide wiki Appendix 3 "DMA timing" updated.
- [HW] bsteux 2023-08-30: independently confirmed on a PAL 7800 and a7800 v5.2 (near-identical pictures): holey-DMA access costs 3 cycles, 4-byte header = 8 cycles, so a skipped object = 8 + 3 = 11. His bars measure cycles left after "DMA saturation" (objects are drawn partially when the line's DMA budget runs out).
- [OP] bsteux 2023-08-30: MARIA renders objects in DL order until the list ends or no DMA cycles are left on the line.

## 2. "Maria DMA Cycles"
- URL: https://forums.atariage.com/topic/343425-maria-dma-cycles/ (Atari 7800 Programming)
- Dates: 2022-11-08 → 2022-11-13 · Pages read: 1 of 1

- [EMU] wavemotion (formerly llabnip; author of A7800DS, a ProSystem-derived DS emulator) 2022-11-08: ProSystem-based emulators charge the 3/6/9-cycle graphics fetch even when the fetch lands in a DMA hole.
- [OP, later superseded] RevEng 2022-11-08: agreed holes cost nothing and said A7800 implemented it that way; noted the BizHawk 7800 author gave conflicting advice in "7800 Software Guide updated" p.2 (comment 3816359). Superseded by his own 2023-08-19 measurement in thread 1 (first graphics byte, 3 cycles, is paid per holey object).
- [EMU] wavemotion 2022-11-08: on a 67 MHz ARM9 every timing nuance costs measurable speed. With simplified timing, *Xevious* showed a stray horizontal line until he restored ProSystem's DLI-overhead compensation; *b\*nQ* title screen glitched unless 3 MARIA cycles were removed. Remaining games need per-game "fudge" tweaks.
- [HW] Eagle 2022-11-08: DSLogic logic-analyser capture of an MSTAT-polling VBLANK wait + WSYNC (writes to $40/$41/$42 as markers); `.dsl` file attached (DSView). Planned further DMA/holey/DLI captures.
- [OP] RevEng 2022-11-08: emulators trade accuracy for speed; gate-level would be exact but too slow.

## 3a. "7800/6502 Hardware Questions" (topic 86087)
- URL: https://forums.atariage.com/topic/86087-78006502-hardware-questions/ (Atari 7800 Programming)
- Dates: 2006-04-05 → 2008-05-17 · Pages read: 1–2 of 2 (all). Mostly generic 6502 Q&A (indirect-Y, page-cross penalties, P register = NV-BDIZC, JMP ($xxFF) bug); 7800-specific points below.

- [OP] Rybags 2006-04-05: MARIA DMA halts the CPU mid-instruction. Flipper 2006-04-05 [OP]: Atari docs give 5–9 video cycles to halt the CPU; MARIA line RAM is double-buffered.
- [OP] DanBoris 2006-04-06: the CPU can stop mid-instruction but not mid-clock; with a 7.16 MHz MARIA clock and a 1.79/1.19 MHz CPU, up to 5 MARIA clocks can pass before the current CPU cycle ends. Registers cost the bottom 64 bytes of both page 0 and page 1.
- [OP] EricBall 2006-04-16: $0040–$00FF mirrors $2040–$20FF and $0140–$01FF mirrors $2140–$21FF.
- [OP] Bryan 2006-05-08: Atari halts the CPU on *any* DMA cycle via Sally's HALT pin (stopping the clock), unlike READY, which only stops on the next read cycle.
- [OP] EricBall 2006-05-09: the 7800 down-shifts the CPU clock for TIA and RIOT accesses (vs RAM/ROM/MARIA), and the RIOT shares that clock.
- [HW] PacMan 2006-05-17: the down-shift happens on *every* TIA/RIOT access, reads included. With an XBoard POKEY playing RMT music, a loop polling inputs lowered the song's pitch, i.e. the cartridge POKEY's clock follows the CPU clock and slows during TIA/RIOT cycles.

## 3b. "Deadline for writes to WSYNC" (topic 339785)
- URL: https://forums.atariage.com/topic/339785-deadline-for-writes-to-wsync/ (Atari 7800 Programming)
- Dates: 2022-08-22 → 2022-08-23 · Pages read: 1 of 1. Embeds topic 318358 "Hitting TIA WSYNC in 7800 mode, a pointless curiosity" (RevEng; covered in §10).

- [HW photo] Eagle 2022-08-22: right border starts at MARIA cycle 413 (= 103.25 CPU cycles); asks whether a WSYNC write deadline exists, as on the A8 (Altirra: writes up to cycle 103 wait for this line's HBLANK, 104+ for the next). No answer in thread.
- Trebor 2022-08-23 compiled earlier posts (authors not named in his post; source topics 164265, 96543, 256466, 212970, 168633, 166625, 331792):
  - [OP] (topic 164265) 7800 WSYNC is not pixel-exact: CPU cycles are 2 or 3 colour clocks long, so the CPU leaves WSYNC with one colour clock of jitter; a mid-line colour-register write makes one pixel 1.5 colour clocks wide and the next 0.5.
  - [OP] STA WSYNC halts the CPU, so a DLI can be delayed (or pushed to the next line); "WSYNC can mess up display interrupts"; MSTAT VBLANK may be later than needed; advice: don't mix WSYNC and DLIs; WSYNC outside DLIs only for PAL/NTSC detection.
  - [OP] (topic 96543) after STA WSYNC "You get 7 CPU cycles before DMA HALTs the processor."
- [EMU] mono 2022-08-23: test (`wsync.asx`/`wsync.a78` attached) measured in the A7800 emulator, not hardware: after STA WSYNC the CPU restarts at the start of the line, about 160-mode pixel 5 (derived from visible area starting at pixel 67).

## 4. "MESS 0.153... more accuracy for 7800 emulation"
- URL: https://forums.atariage.com/topic/224052-mess-0153-more-accuracy-for-7800-emulation/ (Atari 7800 Programming)
- Dates: 2014-04-07 → 2014-04-08 · Pages read: 1 of 1

- [EMU, changelog] RevEng 2014-04-07: RevEng + Trebor's MESS 0.153 changes: DMA cycle stealing added; 256K/512K ROMs with/without RAM and with/without CPUWIZ banked RAM; INTIM now matches hardware; 263/313 lines; more accurate RIOT two-button I/O control, RIOT timers and INPTCTRL; the Atari Diagnostic Test Cartridge now passes its internal hardware tests and runs.
- [EMU] RevEng 2014-04-07, known gaps: MESS DMA stealing sometimes falls short of hardware because the delays behind the GCC spec's *ranges* are not modelled; mid-scanline register (colour) changes are not displayed faithfully, "common shorcoming with all existing 7800 emulators" (2014).

## 5. "Improving the emulators"
- URL: https://forums.atariage.com/topic/145363-improving-the-emulators/ (Atari 7800 Programming)
- Dates: 2009-06-08 → 2009-07-08 · Pages read: 1 of 1

- [EMU] GroovyBee 2009-06-08 relaying raz0red: ProSystem did not advance the RIOT timer during WSYNC, nor while MARIA DMA was running (PAL *Xevious* and *Commando* depend on the timer).
- [HW] GroovyBee 2009-06-08: real-hardware test `sta WSYNC / sta TIM1T / sta WSYNC / lda INTIM`, in VBLANK, zone 1 and zone 2, DMA off and on: INTIM = $8E in all six cases → $FF − $8E = $71 = 113 timer ticks per WSYNC-to-WSYNC line. Conclusion: PHI2 keeps clocking RIOT while Sally is HALTed (WSYNC and DMA); DMA on/off makes no difference (he notes POKEY carts depend on this). Docs say 114.
- [OP] GroovyBee 2009-06-09: slow cycles are 7.16 MHz/6 = 1.1933 MHz vs normal 7.16/4 = 1.79 MHz, on every TIA/RIOT access. EricBall 2009-06-09 [OP]: so the RIOT timer is not a stable clock; cycles per line depend on how much TIA/RIOT access code does.
- [OP, wish] EricBall 2009-06-09: wanted an emulator that models MARIA cycles and cycle stealing "halfway properly"; using emulators as reference it was easy to exceed real cycle budgets; an Elite 3-D demo ran only on emulators.
- [HW] GroovyBee 2009-06-18 (real PAL 7800): reading a palette register (`lda P0C1,y`) returns $00 on hardware, but the written value in ProSystem; GCC docs: MSTAT is MARIA's only readable register.
- [EMU] GroovyBee 2009-07-06, ProSystem 1.3d POKEY bugs: POKEY must stay silent until SKCTL is written with $03 (leaving init/reset); RANDOM always returned $00 — it should read $FF while SKCTL bits 0–1 are 0, then the top 8 bits of the 17-bit poly (9-bit if AUDCTL bit 7). Rybags [OP]: SKCTL %01/%10 are also non-init states. Kr0tki [EMU]: Atari800 emulator implements RANDOM with 2^17 / 2^9 lookup tables.

## 6. "A7800 – 'new' 7800 emulator, tons of controls, very accurate"
- URL: https://forums.atariage.com/topic/268461-a7800-new-7800-emulator-tons-of-controls-very-accurate/ (Atari 7800 Programming)
- Dates: 2017-08-03 → 2017-11-11 · Pages read: 1 of 1

- [EMU, changelog] RevEng 2017-08-03: A7800 = RevEng + Trebor fork of the MAME a7800 driver: ProLine/VCS joysticks, light guns, paddles, driving controllers, keypads, Trak-Balls, Amiga and ST mice; "accurate DMA hole penalties"; improved saturated/normalised "factory set" default palette; a RIOT bug fixed; MAME syntax and config compatibility kept.
- [EMU] RevEng 2017-08-03, light gun: other emulators apply an "average correction factor" between CPU cycle and beam position; A7800 already tracks CPU cycle vs beam faithfully, which showed Atari's light-gun positioning is much tighter than assumed.
- [EMU] RevEng 2017-08-03, keypad: A7800's keypad is idealised (data valid immediately after row select); real hardware needs about 400 µs (≈716 CPU cycles at 1.79 MHz, per the Stella programmer's guide) before row data is valid.
- [OP] RevEng 2017-08-05: no emulator models the console's warm-up hue (chroma phase) drift; a user-selectable chroma phase shift was planned instead.
- [wish] Synthpopalooza 2017-08-23: frame-blended screenshots for flicker-based colour mixing (Trebor's workaround: MAME bgfx + video capture). Inky: dislikes MAME's no-cart start screen. Mord 2017-11-08/09: command-line launch failed (`7800.u7 NOT FOUND`) because MAME resolves `roms/` relative to the current directory; wants a fast edit–build–run loop. gambler172: "too difficult to install".
- [OP] CPUWIZ 2017-11-09: developers should test on real hardware via MCP dev-cart boards.

## 7. "A7800 – the Atari 7800 Emulator" (user/support thread, Trebor)
- URL: https://forums.atariage.com/topic/268458-a7800-the-atari-7800-emulator/ (Atari 7800)
- Dates: 2017-08-03 → 2025-11-18 · Pages read: 1–12 of 12, keyword-filtered (DMA/cycle/palette/POKEY/RIOT/TIA/light gun/release/bug…; ~85 of ~290 posts)

- [EMU] AlyoshaTAS 2017-08-03: RevEng's RIOT timer tests come out short unless RIOT timer ticks are also stretched during TIA/RIOT (slow) accesses, like the CPU (same clock); with that change BizHawk passed them "perfect".
- [EMU] RevEng 2017-08-09: fixed a timing bug that made *Beef Drop* run slowly in the first A7800 release.
- [EMU, changelog] v0.188-03 (Trebor 2018-07-29): console temperature (Cold/Warm/Hot) selects the palette; fixed *Super Skateboardin'* status-area glitch; real-time save states.
- [HW] SmittyB 2018-08-18: on a real Video Touch Pad (keypad), every key in the same column that is scanned after a pressed key also reads as pressed; at most 3 simultaneous presses work. Emulation handles multi-key "better" than hardware.
- [EMU/user] Jetboot Jack 2019-09-29: A7800 lacks the visual glitches of the RetroArch ProSystem core; could not quit to a frontend from a gamepad (fix: `-keyboardprovider dinput`); a horizontal distortion line drifts down the screen in vertical scrollers (*Xevious*, *Plutos*) in A7800 but not RetroArch (unresolved in thread).
- [wish] RevEng 2020-03-26: the A78 header should select controllers automatically (A7800 then needed ini lines `joy1 …`/`joy2 …`).
- [EMU, changelog] v4.0 (2020-06-11): POKEY improvements; Sally/MARIA performance work; "Framerate updated to even 50Hz/60Hz"; XM control registers; BIOS optional. Synthpopalooza [EMU]: POKEY $Ax-distortion/SKCTL demos improved, "not yet 100%" vs hardware.
- [EMU] RevEng 2021-04-11: A7800 enforces MARIA's DMA limit; overloaded DLs show partial objects as on hardware (mksmith confirms from development).
- [complaint] Atari Nut 2021-04-07: A7800 is much heavier on a low-end PC than other 7800 emulators, Altirra or Stella. Shannon 2021-11: SRAM cart types crash RetroArch's MAME core.
- [EMU, changelog] v5.0 (2022-03-31): mid-scanline register writes now shown mid-scanline; bankset bankswitching; POKEY quality; better light-gun accuracy; `a7800dev`/`a7800pdev` machines show DMA usage per scanline; balanced palette brightness/saturation; POKEY@$0800 for flat, SuperGame and bankset carts. v5.1 (2022-05-02): POKEY two-tone mode; SuperGame+RAM+POKEY@800 crash fixed.
- [EMU, changelog] v5.2 (2022-06-26): POKEY poly9 sequence corrected (credit rensoup); POKEY power-on state now matches hardware: POKEY stays in reset until SKCTL bits 0–1 are set, so many older 7800 POKEY demos fall silent (also on the MiSTer core). Trebor: "Some flash carts may take pokey out of reset during startup", retail/homebrew POKEY carts do not.
- [wish] DrVenkman 2022-06-26: a compatibility toggle for those demos; Trebor [OP] prefers fixing the demos (NESticle-era hacks analogy).
- [OP] Trebor 2022-07-18: RetroArch ProSystem core runs well on a Raspberry Pi 3 but "emulation accuracy is very questionable"; JS7800 sits between ProSystem and A7800; BupSystem (Windows only) "great accuracy" and very low requirements; BizHawk "supposed to be very good"; A7800 (MAME 0.188 fork) too heavy for a Pi 3B. pseudografx [wish]: integer-multiple window scaling.
- [EMU] Trebor 2025-02-02: A7800 v5.2 needs the XM module attached to play YM2151 audio (*1942*, *Pac-Man Collection 40th*), but must NOT have XM attached for POKEY@$0800 games (*Attack of the PETSCII Robots*, *Drone Patrol*) or they are silent. karri 2025-07-06: with HSC attached, POKEY@$0450 is silent (known bug; workaround: use XM).
- [wish] Propane13 2024-11-17: SaveKey (A7800 has HSC only). Exceleron 2025-03-12: scanline effect. Karl G 2024-04-30: POKEY audio breaks up on a Raspberry Pi 400 (speed).
- [EMU] RevEng 2025-08-08, light-gun model: on a read of the trigger/sense input, convert the current frame cycle to a beam X,Y and compare with the mouse X,Y against a distance threshold. *Ducks Away* fails because it leaves DMA on (other gun games turn DMA off), so cycle→X,Y is wrong; fix = keep a running count of CPU cycles lost to DMA in the frame.

## 8. Palettes / colour pot (three Trebor threads)
- 8a https://forums.atariage.com/topic/216065-best-7800-prosystem-palettes-why-the-shift-matters/ — "Best 7800 ProSystem Palettes – Why The Shift Matters…", 2013-08-27 → 2013-08-30, p.1 of 1
- 8b https://forums.atariage.com/topic/213725-ntsc-pal-palettes-tint-pot-saturation-variances-more/ — "NTSC & PAL Palettes – Tint, 'Pot', Saturation Variances & More", 2013-06-22 → 2013-08-15, p.1 of 1
- 8c https://forums.atariage.com/topic/322259-trebors-pro-palettes-and-colors-guide-with-conversions/ — "Trebors PRO Palettes and Colors Guide with Conversions", 2021-06-26 → 2025-01-07, p.1–2 of 2 (per-game colour reviews skimmed)

- [HW obs] Trebor 2013-08-27 (8a): nominal hue step ≈ 25.7° ("25.714" per EricBall's derivation; = 180°/7, derived) between the 15 chroma hues; at 25.7° hue $1x and hue $Fx look identical. Real consoles drift *higher* as they warm, and an internal variable resistor (the "colour pot") also shifts it. At 27.7° hues $Ex/$Fx match $1x/$2x; one of his consoles reached ~28.7° after hours. He recommends 26.7° as the default. The BIOS Fuji logo looks "perfect" only at 25.7°.
- [HW obs] Trebor 2013-08-30 (8a): phase step is ≈24° at cold power-on, reaches ≈25.7° after ~90 s–5 min (≈2 min on one unit); two of his units never exceed ~25.7–26.7° even after hours. Hypothesis [OP]: early boards stayed nearer spec, and early titles (*Joust*, *Galaga*, *Pole Position II*) look right at 25.7° while later ones (*Commando*, *Ikari Warriors*, *Midnight Mutants*, *Choplifter!*) look right at 27.7°. "Avoid color E$ like the plague" (advice to developers).
- [OP] Trebor 2013-06-22 (8b): "GCCFINAL" palette family varies console pot (3 settings: more blue→red / neutral / more blue→green), TV tint (G/N/R), brilliance and saturation (LOW/MED/XTR); 2013-08-15 normalised grayscale.
- [HW obs] Trebor 2021 (8c): $Fx tracks temperature: COOL ≈ $1x, HOT ≈ $2x, WARM in between; hue ranges $Ax/$Bx straddle blue/green and change most with warm-up (a 0.5° change is visible). NTSC and PAL palettes are near-identical but offset by one hue row (e.g. NTSC $3x ≈ PAL $4x). LCDs render $1x/$2x/$Ex/$Fx with less red and $5x–$8x with more red than CRTs.
- [HW obs] -^CrossBow^- 2021-07-10 (8c): consoles with a UAV video mod look different on LCDs; *Tower Toppler* never looks right on a UAV, even via composite. He leaves the colour trimmers alone unless grossly off on a PVM.
- [data, changelog] 8c releases: V2 2023-08-21 adds explicit phase steps COOL 25.7°, COOLPLUS 26.2°, WARM 26.7°, WARMPLUS 27.2°, HOT 27.7° and AV/BAL/RF brightness; V3 2023-10-12 adds sets calibrated to the CPS 7800 Diagnostic Test cart luminance ramp, noting raw RF output is about −8 % brightness; V4 2024-06-30 CALIBRATED/MODIFIED/UNCALIBRATED; V5 2024-08-11 adds SUPER and 2600/5200; V7 2025-01-06 = LUM1/LUM2/LUM3 × COOL/WARM/HOT, formats `.pal`, RGB/hex `.txt`.
- [EMU] Trebor 2022-11-14 (8c): A7800 defaults to WARM (COOL/HOT selectable); the MiSTer core ships Trebor `.pal` files and can load external palettes; JS7800 has LUM1/LUM3. wavemotion: A7800DS offers COOL/WARM/HOT with WARM default. Stella darkens supplied palettes with its own gamma (V4 note).
- Licence: no licence or usage terms are stated in any post of 8a/8b/8c (text search for licence/permission/copyright/public domain: no hits). V7's "Emulators" folder contains palette tables pasted into emulator source files (ProSystem `Palette.cpp`, Stella `PaletteHandler.cxx`, Gopher2600 `color.go`, a7800 `a7800.cpp`, Jum52 `colours.h`), which would be GPL-tainted; only the plain `.pal`/`.txt` data would be relevant.

## 9. "MiSTer FPGA – 7800 Core"
- URL: https://forums.atariage.com/topic/318275-mister-fpga-7800-core/ (Atari 7800)
- Dates: 2021-03-12 → 2026-09-01 · Pages read: 1–18 of 18, keyword-filtered, then only posts by developers (Kitrinx, RevEng, Trebor, Eagle, TailChao, batari, …)

- [EMU, FPGA] RevEng 2021-03-22 (helped Kitrinx and alanswx): "everything is completely cycle accurate" as far as he probed; commercial and homebrew games match hardware *including glitches shared with hardware*; A/B listening tests of TIA, POKEY and YM2151 sound authentic; the core models the TIA's **non-linear volume response** (a rumble at loud volumes).
- [HW] RevEng 2021-03-22: measured paddle capacitor charge rate vs resistance and discharge rate on his 7800. Atari's 1 MΩ paddle range is overkill: even the full range of 500 kΩ VIC-20 paddles cannot be read within one frame.
- [OP] RevEng 2021-03-22: FPGA findings transfer to software only indirectly; MAME's YM2151 is already accurate, MAME TIA audio "isn't bad" but less accurate than Kitrinx's; a7800's weakest area is "the oddball pokey modes".
- [OP] Kitrinx 2021-04-03/05: FPGA is not inherently more accurate, but HDL makes accuracy easier; MARIA DMA is written schematic-style (`rtl/Maria/DMA.sv`); one last DMA-timing issue was "quite a pain" before release.
- [EMU] Kitrinx 2021-04-10: YM2151 answers only if the A78 header's YM flag is set, or the XM flag is set *and* bit 7 of the first XM register is high; POKEYs are likewise header-gated; POKEY IRQ is a menu option; Covox is the only always-on device.
- [EMU, changelog] Kitrinx 2021-04-17 first release: whole retail library, NTSC/PAL, HSC saves, light guns/trakballs/mice/Quadtari/paddles, XEGS keyboard via POKEY at $0450 or $4000, dual POKEY, YM2151 (JT51), Covox, XM and XBoard, Activision/Absolute/Souper/SuperGame mappers ≤1 MB, Cool/Warm/Hot colours. 2021-11-29: "Improved RIOT timer edge case accuracy", custom palette loading. 2022-12-27 (via Trebor): light-gun accuracy, bankset mapper, "prevent audio mixing overflow".
- [OP] Kitrinx 2021-04-11: suggests a ~6 kHz audio low-pass (unknown whether hardware has one); "composite blending" option blends horizontally like composite (no artifact colours) for *Tower Toppler*/*Jinx*; latency ≈10 scanlines video, ~0.5 ms input; "Most software emulators have to buffer audio for around 35+ milliseconds".
- [OP] Trebor 2021-04-12: skipping the BIOS runs 99 %+ of ROMs fine; BupChip unsupported (*Rikki & Vikki* plays TIA effects only). RevEng 2021-04-29 [OP]: input latency vs RetroArch on RPi/Android is "night and day".
- [HW] RevEng / Kitrinx 2021-12-12: in two-button mode the 7800 drives a **strong pull-up on joystick pin 5**; the paddle (analog) lines float on a real console; generic SNAC adapters pull them up, breaking 7800 two-button pads.
- [EMU] Kitrinx 2023-01-04/07: core rescales analog paddle input to the game's TIA poll window (−128 = earliest read, 127 = last); she tabulated capacitor charge times in ns for future analog SNAC.
- [OP] Trebor 2024-12-16: Atari 7800+ compatibility is "slightly shy of the JS7800 emulator" (*Attack of the PETSCII Robots* and *Rikki & Vikki* unsupported then); YM2151 works (*1942*).
- [changelog] 2026-09-01 (Trebor, unstable nightlies; Eagle WIP): "Replace 6502, pokey, other vhdl"; new "more accurate 6502 core"; "New Pokey soundchip from the schematics"; BIOS vs internal-ROM toggle; BupChip for Souper; ACE mapper + SN76489; "Real Composite Blending".
- [licence, uncertain] RevEng 2025-06-01 on porting the core: "I think she licensed it as GPL3" (contradicts the brief's MIT-overall assumption; check per file at the pin).

## 10. "hitting TIA WSYNC in 7800 mode (a pointless curiosity)"
- URL: https://forums.atariage.com/topic/318358-hitting-tia-wsync-in-7800-mode-a-pointless-curiosity/ (Atari 7800 Programming)
- Dates: 2021-03-14 → 2026-03-09 · Pages read: 1–2 of 2 (all). Test ROM: `tiamariasync.bas.{a78,bin}` (RevEng).

- [HW] RevEng 2021-03-14: in 7800 mode **both** TIA WSYNC and MARIA WSYNC halt the CPU. TIA and MARIA keep separate horizontal counters running in parallel; after an RSYNC to align them, TIA's line is longer than MARIA's, so TIA-WSYNC bars drift each line; on some runs the TIA bars flicker badly (maybe RSYNC hit on a particular cycle).
- [OP/doc] RevEng 2021-03-14: INPTCTRL locks the console into 7800 or 2600 mode until power-off. The BIOS locks 2600 mode for 2600 carts but leaves INPTCTRL unlocked for 7800 carts. TIA and INPTCTRL decode the same address range, so until the lock is set, writes to INPTCTRL also reach the TIA and vice versa.
- [schematic] RevEng 2026-02-22: on the NTSC schematic the TIA, 6502 and MARIA RDY pins are tied together; MARIA pauses the CPU for DMA with HALT, apparently not with RDY.
- [EMU] JetSetIlly 2026-02-22 (author of test7800): emulates MARIA WSYNC by pulling CPU RDY; HALT exists because DMA must be able to start mid-instruction. Ecernosoft [OP]: DMA can also start/stop mid-CPU-cycle.
- [EMU, a7800 source quoted] RevEng 2026-02-23: a7800 ends DMA by rounding MARIA cycles up to whole CPU cycles, `(maria_cycles+3)/4`, on the reasoning that MARIA must HALT/unHALT on the same ÷4 clock phase. He now suspects MARIA rounds only to the nearest *half* CPU cycle (`((maria_cycles+1) & 0xFE)/4`), since it already tracks half-cycle slowdown penalties. Hardware photo is "jaggy but consistent"; a7800 5.2 renders this demo "way off" (screenshot by Karl G).
- [HW] Eagle 2026-02-23/24 (own cart, and a DragonFly cart): the TIA-WSYNC block alternates between left and right halves on successive frames on ~50 % of power-ups; on his cart, text strings sometimes show as solid white blocks (fine on DragonFly; possibly cart timing).
- [EMU] JetSetIlly 2026-02-25: fixed the slope in test7800 by correcting the slow-clock condition: after an instruction's last cycle the CPU puts PC (the next opcode address) on the bus, which is normally ROM, so the clock is fast. `STA WSYNC` (TIA) therefore costs one slow write cycle, but the RDY wait that follows runs at 1.79 MHz. The step count then matched the hardware photo. Randomising MARIA's power-on state reproduces the run-to-run variation.

## 11. "POKEY 16-bit mode and other AUDCTL settings – A7800 emulation"
- URL: https://forums.atariage.com/topic/293427-pokey-16-bit-mode-and-other-audctl-settings-a7800-emulation/ (Atari 7800 Programming)
- Dates: 2019-07-17 → 2024-10-06 · Pages read: 1–8 of 8, keyword-filtered; Synthpopalooza's ~100 music posts skimmed for emulator/hardware statements

- [HW recordings] Trebor 2019–2020: real-hardware recordings (MCP DevCart, POKEY@$0450) of most demo tunes in the thread (Arkanoid 2, Bubble Bobble, Mega Man 2, Rolling Thunder, Zanac, Super Mario World, Top Gun, Stranger Things, …) posted as `.wav`/`.mp3` next to emulator captures — a ready-made audio oracle set. 2019-07-22: for the SKCTL two-tone Arkanoid demo, hardware and A7800 both produce the same pops/static.
- [OP] Synthpopalooza 2019–2020: POKEY $Cx distortion samples the 4-bit poly; periods divisible by 3 or 5 give different timbres, by 15 give silence, and which timbre plays depends on poly state when the note starts (controllable by rewriting SKCTL, AUDFx and STIMER before each note, a method he credits to pavros). AUDCTL high-pass "filter" behaves like oscillator sync (PWM-like duty).
- [HW] Trebor 2019-10-27: a two-tone demo started in a different key on every Reset; Synthpopalooza: the phase between the paired oscillators is not constant. emkay [OP]: playing the paired channels at the highest pitch for a while stabilises it.
- [HW/EMU] Synthpopalooza 2019-09-16: confirmed on hardware (and Altirra): in two-tone mode the louder the carrier channel, the more it disturbs the other channels' volume.
- [EMU] 2019–2020 status: ProSystem and A7800 both failed 9-bit-poly ($8x with AUDCTL bit 7), high-pass and two-tone modes; A7800 played $2x at 1.79 MHz an octave high. Test tool `pokeytool.bas` (2020-05-06) steps all 16-bit waveforms by semitone.
- [EMU vs HW] Synthpopalooza 2022-04-02/22 on A7800 v5.0: 16-bit $Cx and two-tone $Ax at 64 kHz now match hardware ("100%"); remaining: $2x at 1.79 MHz an octave high, some 1.79 MHz+two-tone $Ax notes drop (same flaw in then-current Altirra and PokeyMax); overall "about 98% to 99% good to real hardware".
- [OP] Pat Brady / RevEng 2020-11-29: the A78 header (then) could only say POKEY@$0450 or @$4000; dual POKEY needed a new header bit and emulator support.
- [HW] -^CrossBow^- 2023-03-07: on the Dragonfly cart both POKEYs are mixed into the single cartridge audio line before the console; stereo would need a separate output and external mixing with TIA.
- [EMU/HW] RevEng 2024-09-27: early a7800 started POKEY already out of reset, so demos without the reset-exit write played; on real hardware they were silent (except on flash carts that release POKEY at boot), so a7800 was changed to match hardware; RevEng patched the demos in Trebor's PROPack. Eagle 2024-09-25: standard init clears AUDF0…$0F then writes SKCTL = 3; XM POKEY also needs the XM control register enabling it.
- [HW] batari 2024-10-06: HOKEY implements dual POKEY by transparent bankswitching inside 16 bytes and answers identically at $0440 and $0450, so code written for separate $0440/$0450 POKEYs works on both.
- Licence: Propane13 collected the demo sources into a compilable GitHub repo (linked in his 2024-10-06 post) while saying he has no rights to them (author Synthpopalooza; no licence stated).

## 12. "7800 Software Guide updated"
- URL: https://forums.atariage.com/topic/222380-7800-software-guide-updated/ (Atari 7800 Programming)
- Dates: 2014-02-22 → 2024-12-06 · Pages read: 1–2 of 2 (all). The wiki Software Guide (7800.8bitdev.org) supersedes the 2014 PDF attached here (last wiki edit 2023-08-19 per Trebor).

- [doc] PAC-MAN-RED 2014-02-23 quotes the *original* Atari guide's Appendix 3 (all in 7.16 MHz ticks): DMA startup 5–9; 4-byte header 8; 5-byte header 12; direct graphics byte 3; indirect 1-byte 6, 2-byte 9; character-map access 3; shutdown last line of zone 10–13, other lines 4–7; end of VBLANK = startup + long shutdown; "DMA does not begin until 7 CPU (1.79 MHz) cycles into each scan line"; one tick between DMA shutdown and DLI generation. RevEng 2014-05-01 later replaced these with the GCC-spec values in the wiki.
- [doc] Same guide: DMA is off at power-up; MARIA does not initialise WRITE MODE at power-up; enable DMA only during VBLANK. The guide's equates mark palette registers R/W (but see §5: on hardware they read $00).
- [OP] CPUWIZ 2014-02-24: CTRL DMA-mode values 0/1 ("test mode") can damage some consoles — do not use.
- [schematic] EricBall 2014-03-03: INPTCTRL is a write-only latch decoded at $0000–$001F by external logic (U11 on the NTSC schematic).
- [BIOS disassembly] EricBall 2014-03-05: the NTSC BIOS locks INPTCTRL only on the 2600 path. On the 7800 path it does `LDX #$16 / STX INPTCTRL / TXS / SED / JMP ($FFFC)`: BIOS switched out but **not locked**, S = $16, X = $16, **decimal flag set**. So a 7800 game must write $x7 to INPTCTRL before touching TIA.
- [EMU, derived] AlyoshaTAS 2017-07-24: guide error, CTRL CW bit 1 = two-byte characters. Line-RAM model: 320 entries × 5 bits. Write mode on: 4 entries per byte, `[P2,G3,G2,G7,G6]` ×2 then `[P2,G1,G0,G5,G4]` ×2; write mode off: 8 entries, `[P2,P1,P0,G7,G6]` ×2 … `[P2,P1,P0,G1,G0]` ×2. An entry is written only if its low 2 bits ≠ 0 (Kangaroo mode writes 0 instead). Read mode 2: even pixel `[D4,0,0,D1,D3]`, odd `[D4,0,0,D0,D2]`; read mode 3: even `[D4,D3,D2,D1,0]`, odd `[D4,D3,D2,D0,0]`.
- [EMU, matches HW tests] AlyoshaTAS 2017-07-29: the first graphics read that lands in a hole IS charged; later reads of the same object are not. This passes both DMA-hole tests in RevEng's test ROM and fixes the precise water effect in *Scrapyard Dog*'s sewer level.
- [HW] RevEng 2020-04-24: RIOT timers are unreliable unless written a couple of times in a row. 2020-10-01 [HW, scope]: corrects his earlier claim — RIOT RAM accesses *are* slowed to 1.19 MHz; RIOT RAM is still unreliable mainly because of DMA (wiki page "RIOT limitations and workarounds").
- [doc + test] Pat Brady 2020-10-01: the cart signature occupies $FF80–$FFF7 (not $FF7A–$FFF9); $FF7A–$FF7F is usable by carts (tested); RevEng agrees.

## 13. "XE lightgun: Real 7800 vs MiSTer, XE vs Sega Phaser, offset, accuracy, adapters, etc."
- URL: https://forums.atariage.com/topic/353281-xe-lightgun-real-7800-vs-mister-xe-vs-sega-phaser-offset-accuracy-adapters-etc/ (Atari 7800)
- Dates: 2023-07-24 → 2024-01-14 · Pages read: 1–2 of 2 (all)

- [HW obs] RockLobster 2023-07-24: on a real 7800 the XE gun shoots consistently about ½ inch right/low and is less accurate than a Sega Phaser (most visible in *Crossbow*). On MiSTer via SNAC the XE gun is *more* accurate than on the real console; the Phaser was offset on MiSTer until a core build delayed the SNAC pin sampling.
- [OP, developer] SmittyB 2023-07-26: screen-flash guns poll the sensor in a tight loop, so Y is easy but X depends on exact cycle timing; "I believe Crossbow only manages to poll it about 16 times per line", i.e. ~20-pixel X resolution, and a late sensor response costs ~40 pixels. *Ducks Away* therefore uses per-target flashing (one hit/no-hit bit per frame).
- [OP] SmittyB 2024-01-14: commercial gun games hard-code their own offsets; accuracy may vary per console; 7800GD offers an NTSC *Crossbow* calibration cheat. DrVenkman 2023: XE gun and Sega Phaser are near-identical inside, except that the Phaser's trigger is inverted (needs an adapter).

## 14. "Emulator – Test7800" (JetSetIlly's new emulator)
- URL: https://forums.atariage.com/topic/383566-emulator-test7800-v081/ (Atari 7800; title changes with version)
- Dates: 2025-08-05 → 2026-10-10 · Pages read: 1–8 of 8, keyword-filtered (~140 of ~190 posts kept)

- [EMU] JetSetIlly 2025-08-05: test7800 (Go) reuses the CPU, TIA and RIOT of his 2600 emulator Gopher2600; only MARIA and CPU interrupt handling are new (licence: Gopher2600-derived → treat as GPL; do not open source).
- [EMU] Karl G 2025-08-06: TIA "thrust"/`pewpew` sounds (pure tones, AUDC 4 and 12) differed from hardware/a7800/js7800/BupSystem and varied per trigger; cause was audio buffering vs video sync, not TIA logic (fixed v0.2.0).
- [EMU] JetSetIlly 2025-08-15/23: screen corruption was the number of pre-DMA CPU cycles per scanline; v0.4.0 DMA + interrupt timing fixes, "The 7800 Test ROM has proved helpful… DMA timings are very close to if not exactly hitting the target line"; RIOT timer fix made *Beef Drop* work. v0.3.0: *Scrapyard Dog* correct; *Ballblazer* needs SuperGame RAM without the bankswitch bit. v0.5.0 overscan change fixed *Tiger Heli*. v0.9.0 (2026-10-10): timer tick fix makes bwitruk's raycaster work; Souper mapper (*Rikki & Vikki*) without BupChip.
- [EMU] RevEng 2025-09-16: *Ballblazer* missing a music voice ⇒ POKEY RANDOM missing/broken (wiki "Common Emulator Development Issues").
- [HW + EMU] Eagle 2025-09-21/22: `randomtest.a78` plots RANDOM pairs: real hardware (PokeyMax) and A7800 v5.2 show no pattern; JS7800 failed (a read of RANDOM behaved like a >8-bit value until `AND #$FF`). RevEng: the MAME/a7800 RANDOM sequence is right, because arcade *Tempest*'s genuine-hardware check uses it and passes in MAME.
- [EMU, a7800 internals] RevEng 2025-09-29, POKEY two-tone: both ch1 and ch2 timer completion reset ch1; a reset caused by ch2 does not toggle ch1's output/sample; if both complete on the same cycle, ch2's behaviour wins; ch1→ch2 reset only matters with serial output = 1 (unused on 7800). JetSetIlly: applies when SKCTL force-break is set (as in *E.X.O.*).
- [EMU] JetSetIlly 2025-10-04: *E.X.O.* title-music errors were **aliasing**: a 31 469 Hz output rate is fine for TIA and 64/15 kHz POKEY but not for 1.79 MHz POKEY pure tones; default raised to 48 kHz. RevEng 2025-10-07: js7800 keeps a scanline-multiple sample rate and low-pass filters POKEY output instead.
- [EMU] JetSetIlly 2025-10-29: fixed undocumented `ARR` (V flag in binary mode; decimal mode) per 64doc.txt so that "RevEng's test binaries" (new 6502 tests with AdamWozniak) pass.
- [EMU, display] JetSetIlly/RevEng/Trebor 2025-12-12/13: visible-area crop differs per game (*Commando* wants top = line 27, *Tiger Heli* 23–24). Trebor: MARIA outputs 243 visible lines; A7800 crops ≈8 top + 8 bottom by default → 227 lines (21 of 28 NTSC CRTs measured by forum users averaged 224 visible lines).
- [EMU] JetSetIlly 2026-01-07: devices auto-attached from the A78 header (paddles, trakball, HSC) except SaveKey, because *A.R.T.I.* misbehaves with SaveKey in test7800 (fine on hardware).
- [HW + wish] Karl G 2026-05-02: no 7800 emulator supported QuadTari. SmittyB: QuadTari switches pairs (1/2 vs 3/4) on TIA VBLANK bit 7, and detection/switching has capacitor-charge delays like paddles; half-plugged configurations are detectable through INPT registers.
- [complaints/wishes] jeremysart 2025-11-17 and JetSetIlly: first experiences with other 7800 emulators were "frustrating"; Karl G: quit key, gamepad support, Linux debugger build; Trebor: drag-and-drop ROM loading; SmittyB/Karl G 2026-05: test7800 v0.7.3+ too slow on old PCs (uses 4 cores).
- [OP] CPUWIZ 2025-09-20: no dual-POKEY hardware existed then; "if a game cart does not run on a stock 7800, it isn't a 7800 game". Trebor 2025-09-20: A7800/JS7800 support one POKEY at $0450, $0800 or $4000; dual POKEY ($0450 + $0440) and bankset's dual option ($4000 + $0800) unsupported.

## 15. "7800 + Homebrew compatibility?" (Atari 7800+, 2024 hardware)
- URL: https://forums.atariage.com/topic/375832-7800-homebrew-compatibility/ (Atari 2600+/7800+)
- Dates: 2024-11-21 → 2025-08-19 · Pages read: 1–2 of 2 (all)

- [HW obs] Albert 2024-11-21: the 7800+ runs all AtariAge-store 7800 homebrews except *Attack of the PETSCII Robots*; *Rikki & Vikki* also fails (maybe audio only). -^CrossBow^- 2024-11-30: *E.X.O. CE* needed firmware 2.0.1.1; *Plutos* intermittently shows corrupted graphics on a 2600+. rennervision 2024-12-23: no trak-ball support for 7800 *Centipede* (TB homebrew).
- [EMU] raz0red 2024-12-08 (JS7800 author): the Plus consoles first used RetroArch ProSystem, replaced soon after launch by a port of JS7800, whose 7800 core is largely rewritten "to more closely resemble A7800"; he suggests calling it "P7800". JS7800 has supported banksets since 0.0.5 (2023-07-30). Bankset carts (PETSCII Robots) cannot be dumped on the 2600+ because its cart reader does not wire the HALT line.

## 16. ProSystem / libretro core problems (two threads)
- 16a https://forums.atariage.com/topic/355224-homebrew-compatibility/ — "Homebrew compatibility?" (Atari 2600+/7800+), 2023-09-27 → 2025-07-25, p.1–5 of 5 keyword-filtered
- 16b https://forums.atariage.com/topic/314301-7800-in-retroarchlaunchbox-issues/ — "7800 in Retroarch/Launchbox issues" (Atari 7800), 2020-12-06 → 2020-12-08, p.1 of 1

- [EMU] Defender_2600 2023-09-28 (16a), relaying Trebor's test (topic 354126, comment 5322971) of 110 mostly-homebrew titles on the RetroArch **ProSystem 1.3 core** (the first Atari 2600+ firmware used ProSystem 1.3e for 7800 carts): 49 OK; 36 "works with issue" (graphics corruption, speed/timing, missing enemies, player cannot move, wrong POKEY notes); 20 black screen; 6 no audio; 1 needs light gun (*Sentinel*). Black-screen examples: *1942*, *A.R.T.I.*, *Arkanoid*, *Attack of the PETSCII Robots*, *Bernie and the Tower of Doom*, *Block'em Sock'em*, *Drone Patrol*, *E.X.O.*, *Frogus*, *Harpy's Curse*, *Keystone Koppers*, *Klax*, *Legend of Silverpeak*, *Millie & Molly*, *Pac-Man Collection 40th*, *Robots Rumble*, *Serpentine*, *Rescue on Fractalus* (after start). Timing: *Asteroids Deluxe*, *Space Duel*; *Beef Drop*: no POKEY, enemies absent, player frozen. Useful as a regression list.
- [EMU] Trebor 2020-12-07 (16b): ProSystem often fails carts over 48K because it does not trust/parse A78 headers and relies on `ProSystem.dat` entries keyed by ROM MD5 (cart type, POKEY, controllers, region); BupSystem and A7800 read headers properly. He urges RetroArch to use MAME/A7800-based 7800 emulation (MAME ≥ 0.188). MrZarniwoop [EMU]: the libretro ProSystem core runs roughly 50 % of hacks/homebrews correctly. Stevaside/Trebor: missing *Commando* music in BupSystem was a bad-header ROM — use Trebor's PROPack ROMs.
- [wish] Stevaside 2020-12-07: consistent save/load-state hotkeys and gamepad combos across emulators in a frontend; always-fullscreen launch.
- [HW obs] Albert 2025-07-25 (16a): with current firmware nearly all AtariAge 7800 homebrews run on the Plus consoles; known exception *Attack of the PETSCII Robots*. DEANJIMMY 2023-12-05: trak-ball (TS mode) unsupported on 2600+ then.

## 17. "fingerprinting 6502 processors" (7800 Sally variants, undocumented opcodes)
- 17a https://forums.atariage.com/topic/385521-fingerprinting-6502-processors/ (Atari 7800), 2025-10-29 → 2025-11-24, p.1–2 of 2 (all)
- 17b https://forums.atariage.com/topic/385516-fingerprinting-6502-processors/ (Atari 2600 Programming), 2025-10-29 → 2025-11-02, p.1 of 1 (filtered)
- 17c https://forums.atariage.com/topic/128388-7800-uses-an-undocumented-sally-pin/ (Atari 7800 Programming), 2008-07-06 → 07, p.1 of 1
- Test ROM: RevEng's `6502fingerprint.bas.a78` (+ 7800basic source zip), several revisions, attached to 17a.

- [HW] Trebor 2025-10-30/31 (17a/b): three consoles (serials AT84…, A1…, A3…) give the identical ARR-based fingerprint `8A55CE`; `LDA #0 / LDX #$FF / ANE #$00` → $00 on all. With `ANE #$FF` (which exposes the "magic constant") A1 and A3 give $00 but the AT84 unit gave $6C, later $68.
- [HW] Trebor 2025-11-03/16: on the AT84 unit the ANE constant wandered ($6C/$6E/$68/$28, then mostly $00↔$6E) and the pattern changed with warm-up after power-on; `LAX #$FF` stayed $00 on all units; flags behaved as expected (N/Z/V consistent with the result).
- [HW] -^CrossBow^- 2025-11-22/23: an NCR-made Sally starts toggling the ANE value after 1–2 min; a Rockwell Sally never changed in 10+ h. Trebor 2025-11-23 chip IDs: AT84 = NCR C014806C-29, A1 = NCR C014806C-29 (stable), A3 = Rockwell C014806-12. RevEng: since one NCR is stable, ANE instability cannot fingerprint the vendor; most Sallys show constant $00.
- [OP] RevEng 2025-10-31: the "MOS6510 Unintended Opcodes" (No More Secrets) document has been "faithful for Sally". batari: the ANE constant is reported as $EE, $11, $10, $01, $00, $8C, $CC, $0C, $8E, $EF, $FE or $FF on different NMOS systems.
- [EMU] GorillaSapiens (AdamWozniak) 2025-10-31 (17b): the SingleStepTests 65x02 vectors for ARR disagree with each other (he reads this as silicon variation); his emulator also fails SST for SHA, TAS, SHY, SHX. JetSetIlly 2025-11-02: test7800/Gopher2600 pass all SST including ARR, but not SHA/TAS/SHY/SHX, whose final write address is sometimes the read address with the high byte masked, with no clear pattern.
- [OP, emulator design] JetSetIlly 2025-11-04: offer an option that randomises the ANE/LAX "internal parameter" (like Stella's random unused bits) so developers catch reliance on it.
- [HW] Bryan 2008-07-07 (17c): Sally pin 35 is HALT (NC on a stock 6502); the 7800 schematic's pin-34/36 R/W labelling is confusing, and pin 34 on a real Sally (C014806) is unconnected. -^CrossBow^- 2025 (17a): NCR Sallys in 7800s often carry a factory capacitor bodge from pin 25 (A15) to ground; the "evil" 7800 revision uses a Rockwell part.

## 18. "Number of DMA clock cycles per scanline?" (+ "Question about Maria DMA")
- 18a https://forums.atariage.com/topic/164554-number-of-dma-clock-cycles-per-scanline/ (Atari 7800 Programming), 2010-06-15 → 2020-10-23, p.1–2 of 2 (all)
- 18b https://forums.atariage.com/topic/337333-question-about-maria-dma/, 2022-06-26, p.1 of 1

- [HW] nichtsnutz 2010-07-28/29 (logic analyser, 200 MS/s, PAL): line = 64 µs ±5 ns = 454 MARIA clocks (14.1875 MHz ÷ 2 = 7.09 MHz) = 113.5 CPU cycles; PAL frame 313 lines, PAL VBLANK 21 lines. GCC's 452-clock diagram is wrong (454); his other horizontal positions matched the diagram. CPU = 14.1875/8 = 1.773 MHz or /12 = 1.182 MHz. **The CPU clock edge shifts against CSYNC from line to line** (half-cycle offset per line; he could not get a stable CPU-edge-to-sync relation).
- [doc] gdement / doppel / SeaGtGruff 2010 (GCC MARIA specs): regular DMA starts at the leading edge of HBLANK, End-of-VBLANK DMA at the trailing edge of VBLANK; layout: HSYNC 34 clocks, HBLANK 68, left border to clock 93, display 93–413 (320 clocks = 160 colour clocks), then 27 border + 12 blank; GCC text says 454 clocks/line but its figure says 452; GCC lists the crystal as 14.318080 MHz.
- [HW/schematic] EricBall 2010-09-22: NTSC: one 14.31818 MHz crystal (4× colour burst) drives MARIA, CPU and TIA clocks, and hue comes from a tapped delay line as on the 2600. PAL: a separate 4.433 MHz crystal generates the colour carrier. GroovyBee and Mitch 2010-09 [HW]: PAL main crystal reads 14.187576 MHz (rev C and others).
- [OP] GroovyBee 2010-06-15: MARIA reads RAM in 2 ticks, ROM in 3. TailChao 2020-10-21 (BupSystem author) [OP]: fetch time is fixed by *what* is fetched (header/DL byte 2, graphics 3), not by whether it sits in RAM or ROM. Pat Brady: DL/DLL need not be RAM, they just must answer in 2 ticks.
- [OP, derived from HW] RevEng 2020-10-15, DMA budget: 454 − 28 (7 CPU cycles before DMA) − startup/shutdown (16 other lines, 24 last line) → he writes "410=last line, and 402=other line" (arithmetic: 454−28−16 = 410 for *other* lines, 454−28−24 = 402 for the *last* line; his labels look swapped). Worst case another 6 ticks (3 start + 3 end) go to aligning HALT/resume with the ÷4 CPU clock; MARIA does not wait for an instruction to finish, so long instructions and RIOT/TIA cycles cost only through that alignment.
- [OP] Pat Brady 2020-10-15: reliably ~413 ticks per line for headers/maps/graphics, 415 if TIA/RIOT accesses are avoided during DMA; EricBall 2010-06-29: GCC figures may be for the prototype and Atari's for later silicon (5-byte header 10 vs 12).
- [OP] Eagle 2022-06-26 (18b): MARIA halts the CPU entirely for DMA, starting 7 CPU cycles into each line.

## 19. "BupSystem" (TailChao's emulator; hardware findings)
- URL: https://forums.atariage.com/topic/285921-bupsystem/ (Atari 7800 Programming)
- Dates: 2018-12-11 → 2022-03-10 · Pages read: 1–5 of 5, keyword-filtered (developer posts kept)

- [HW scope] TailChao 2019-01-20: HALT goes low ≈28 ticks (7.16 MHz) into each line, just after PHI2 falls; Sally still drives the bus for the first PHI2 tick and has released it by the second falling edge; R/W stays high while MARIA owns the bus. RevEng: 28 ticks = the guide's 7 CPU cycles. TailChao: the old "DMA starts 8 Sally cycles in" remark is half-right, because Sally always completes ≥1 cycle after HALT falls.
- [HW] TailChao 2019-01-20 (MARIA diagnostic hidden in *Rikki & Vikki*: right difficulty to the right, then R-U-D-D-L-D-U-U on joystick 2 at the PenguiNet screen): counts VBLANK and active lines via MSTAT + WSYNC and measures where MSTAT flips: **MSTAT changes state part-way through a scanline**, not at its start or end (he assumes MARIA's state changes align with its DMA start point). MARIA reading its own palette registers returns noise; MARIA reads of TIA/RIOT space return chaotic data (out-of-spec timing).
- [HW] TailChao/RevEng 2019-01-20/21: palette registers are write-only (GCC guide); reads are unstable in every addressing mode.
- [HW/schematic] TailChao 2019-04-24, controller init: write $14 to SWBCNT (PB2, PB4 outputs) to control transistors Q6/Q5 (the strong two-button pull-ups); SWCHB $00 = both ProLine two-button, $04 = P1 one-button/P2 ProLine, $10 = P1 ProLine/P2 one-button, $14 = both one-button. **SWCHB's output latch is $FF after reset** (pull-ups off ⇒ one-button mode). A ProLine in one-button mode: pressing either fire button pulls pin 6 low and raises pin 9 or 5. *One-on-One Basketball* writes SWBCNT but never SWCHB and still works only because of this ProLine behaviour (MAME tester issue 06060).
- [EMU] TailChao 2018-12-17/23: *One-on-One* and *Ninja Golf* (one title-music channel) depend on state the BIOS leaves behind; many titles rely on BIOS-initialised registers/RAM; some behave differently after the NTSC ("Atari Rainbow") vs PAL ("Asteroids") BIOS; proposes a user option for scrambled power-on state or "BIOS residue". 2020-04-12: wants configurable RAM power-on patterns (zero / ones / hardware-like).
- [EMU] TailChao 2018-12-23: MARIA timing is still off when software sets a DLI in the *first* DLL entry (*Xenophobe* is "super fussy" here).
- [EMU] TailChao 2019-04-25: *Beef Drop* broke because BupSystem's RIOT timer stopped after the second underflow; it must keep decrementing (fixed with PA7 edge detection).
- [EMU, changelog] BupSystem 0.9.6.0 (2019-08-01): VCS memory map via INPTCTRL's MEN bit; TIA INPT0–3 dump-to-ground and INPT4–5 latches; ABS,X/ABS,Y reads do dummy reads on page cross, writes are always 5 cycles with a dummy access; XBoard present but "asleep" until XCTRL is written.
- [EMU/OP] TailChao 2018-12-23 / 2019-08-01: much POKEY "noise" in emulators is aliasing from naive resampling; *Bentley Bear's Crystal Quest* sets very high frequencies (AUDF=0, AUDC=$Ax) that the real console low-passes into silence. *Donkey Kong XM* (v12) writes $00 to SKCTL ($045F) and never releases reset, yet makes sound (unresolved vs documentation).
- [HW obs] TailChao 2019-11-01: *Tomcat F-14* writes $7F, $00, $40 to CTRL at boot, briefly selecting a DMA test mode without harm, so an emulator must not crash on that (his crash-on-test-mode idea broke the game).
- [OP] TailChao 2020-01-20 / Trebor: visible lines vary (Atari's 192-line safe area fits a Sharp Linytron; a PVM shows ~240). Forum CRT survey (topic 261266): min 190, max 239, ~60 % of NTSC sets ≈224; *Water Ski* and *Tank Command* exactly fill 224 lines.
- [EMU/user checklist] Trebor 2018-12-16: his emulator test list = RevEng's 7800 Test ROM, *Kung Fu Master*, *Scrapyard Dog*, overscan, *One-on-One*, POKEY@$0450 titles (*Donkey Kong PK*, *Bentley Bear*), and the official Atari Diagnostic Test cart (expected screen after ~10 s; analysis at pages.cs.wisc.edu/~harvey/7800/diag.html; needs TIA video emulation).
- Licence: TailChao 2021-04-20: BupSystem is closed source; "If the source is ever released it'll be under zlib". 2022-03-10: recommends a7800 unless you need *Rikki & Vikki*'s hardware or have a very slow PC.

## 20. "7800 Composite Mod question on the 6.8k Pokey" (audio mixing)
- URL: https://forums.atariage.com/topic/316812-7800-composite-mod-question-on-the-68k-pokey/ (Atari 7800) · 2021-02-05 → 2021-02-07 · p.1 of 1

- [HW] NISMOPC / -^CrossBow^- 2021-02-05: the stock mixer sums cartridge (POKEY/external) audio through **6.8 kΩ** and TIA audio through **18 kΩ** (posters use the R5/R6 labels inconsistently). Tapping audio before these resistors lets TIA drown POKEY; *Commando* (TIA + POKEY) is the balance test, *Ballblazer* is POKEY-only. Derived (unverified): with equal source levels, cartridge audio is weighted ≈18/6.8 ≈ 2.6× TIA at the summing node.

## 21. Composite artifacting (three threads)
- 21a https://forums.atariage.com/topic/349547-320-mode-colours-question/ — "320 mode colours question" (Atari 7800 Programming), 2023-03-20 → 2025-12-23, p.1–6 of 6 keyword-filtered
- 21b https://forums.atariage.com/topic/385287-is-atari-7800-color-artifacting-worth-messing-around-with/ (Atari 7800 Programming), 2025-10-20 → 2025-10-27, p.1 of 1
- 21c https://forums.atariage.com/topic/353253-artifact-color-atari-7800-emulation/ (Atari 7800), 2023-07-23 → 2023-07-24, p.1 of 1

- [OP/HW] RevEng 2023-09-07 (21a): composite artifacting also occurs in 160 modes, just weaker (wiki "Atari 7800 Color Documentation" shows artifact colours from two composite sets); in 320 modes a 2-pixel-wide diagonal avoids artifacts; S-Video, 7800GD component and MiSTer HDMI show none. 320A quirk: a transparent pixel paired with a lit one shows the background colour instead of being transparent. RevEng [OP]: "a display that's incapable of composite artifacting is deficient for retro consoles"; he uses artifact colour deliberately (PETSCII Robots instruction bullets, Phoenix WIP starfield).
- [OP] Jetboot Jack 2025-10-27 (21b): *Tower Toppler* (PAL-developed) mostly relies on composite blur blending alternating lines into darker shades rather than generating new hues; *Jinx* similar; *Rikki & Vikki* uses artifacting too (MrZarniwoop). SmittyB: artifact hues are unpredictable because the palette drifts per console/session.
- [HW obs] -^CrossBow^- 2025-10-20 (21b): only RF/composite show artifacts; Lumacode→HDMI does not; the 7800GD's RGB output has a "blending" option for *Tower Toppler* (also *Rikki & Vikki*). Karl G: the Plus consoles show no artifacts.
- [EMU] Trebor 2023-07-23 (21c): A7800 shows artifact colours only through MAME's bgfx `hlsl` chain with "Enable NTSC" on (or crt-geom-deluxe for an S-Video look), not natively; -^CrossBow^- [complaint]: that chain dropped to <1 fps at 100 % GPU on an RTX 2070. Eagle 2023-06-18 (21a): `artifacts.a78` (9-colour 320C artifact test) with MiSTer "composite blending" captures.

## 22. "7800 Utility Cart" (+ 240p test-suite wish)
- 22a https://forums.atariage.com/topic/233645-7800-utility-cart/ (Atari 7800), 2015-01-10 → 2026-03-28, p.1 of 1 keyword-filtered
- 22b https://forums.atariage.com/topic/346668-feasibility-of-porting-240p-test-suite-from-nes-to-7800/ (Atari 7800), 2023-01-17 → 2023-01-28, p.1 of 1

- [tool] RevEng 2015-01-10: Utility Cart (`7800utilitycart.32k.{a78,bin}`, 128K bin for SuperGame RAM tests) reports region, BIOS CRC16, detected POKEY, memory at $4000 and HSC; joystick test, colour bars + full 256-colour chart, internal/external RAM tests (each bank of known banked formats), TIA + POKEY audio test, HSC management. Terms: "I offer no warranty of fitness for any purpose" (no licence granted).
- [OP] JetSetIlly 2026-03-28: useful for emulator work because its results on an emulator are a fixed reference to compare with hardware. -^CrossBow^- [HW use]: its cartridge-integrity test (continuous compare of cart reads; red blocks on mismatch) can fail only after an hour of warm-up; its audio test plays both TIA channels and all four POKEY voices for level balancing. CPUWIZ: some features were meant for the XM.
- [wish] MrZarniwoop 2023-01-17 (22b): no 7800 port of the 240p test suite (screen geometry, colour, black/white levels); the NES mini version (pinobatch) and Artemio's C sources are open source.

## Not fetched
- Every priority thread (1–5) loaded completely; no page failed and Wayback was never needed.
- Attachments were not downloaded or viewed (test ROMs, LogicPort/DSLogic captures, `.wav`/`.mp3` hardware recordings, screenshots, PDFs, zips). Trebor's YouTube capture of RevEng's 7800test on two consoles (§1) was not watched.
- Linked but not opened: 166451 "PAL frame timing" (scoped timings cited in §1); 214384 "Has anyone worked on an FPGA Atari 7800" (schematic inferences); 164265 "WSYNC strobe on a 7800" and 96543 "My unofficial Sally questions thread" (quoted in §3b); 354126 "2600+ w/7800 support" (origin of the 110-game ProSystem test summarised in §16); 333359 "A7800 5.0 release" (DMA-usage-per-line screenshot); 337317 "A7800 5.2 release"; 295770 "My POKEY experiments using nonstandard settings"; 118448 "Implementation of POKEY 2-tone filter in emulation"; 335904 (flash-cart copy protection / emulator detection, POKEY RANDOM); 373442 "Supporting dual POKEY on HOKEY" (search snippet only); 366095 "A7800DS"; 312464 "Which is the best Atari 7800 emulator?"; 286298 "Mame vs Prosystem emulator"; 372027 "Best 7800 emulator+debugger"; 261266 "Testing CRT screen size"; 210082 "Colors…what do you want?"; 382767 "6502 Opcodes for Hackers"; 346754 "HOKEY / POKEY sound issues"; 200088 "7800 'Stereo' Mixing Circuit?".
- Megathreads were keyword-filtered rather than read post by post: §7 (12 pages), §9 (18), §11 (8), §14 (8), §19 (5), §16a (5), §21a (6). Off-topic chatter in them was skipped by design.
- "general_tests": no AtariAge thread, MiSTer page or web result matched that name (searched 2026-10-15).
- Out of scope (not forum): the 7800.8bitdev.org wiki pages cited by posters (Software Guide Appendix 3 "DMA timing", "Common Emulator Development Issues", "RIOT limitations and workarounds", "Atari 7800 Color Documentation"); pages.cs.wisc.edu/~harvey/7800/diag.html (Atari diagnostic cart analysis); the Altirra Hardware Reference Manual (POKEY chapter, recommended by Trebor/JetSetIlly).

## Licence notes
- **RevEng test ROMs**: `7800test.bas` (2014, §1), `dmaholecycles.a78` (2023, §1), `tiamariasync.bas` (2021, §10), `6502fingerprint.bas` (2025, 7800basic source attached, §17), 7800 Utility Cart (2015, §22: "no warranty of fitness for any purpose"). No licence is stated for any of them, so treat them as all rights reserved: run them locally as black-box oracles, never vendor them into the repo. Ask RevEng before redistributing.
- **Other test ROMs/sources posted in threads**: bsteux's holey-DMA C test (inline source, §1), mono's `wsync.asx/.a78` (§3b), Eagle's `randomtest.a78`, `artifacts.a78` and SN/SN2 demos (§14, §21, §9), Karl G's `pewpew` ROM + 7800basic source (§14), Synthpopalooza's POKEY demos and `pokeytool.bas` (§11): **no licence stated** anywhere. Eagle (2024-12-16) explicitly asked that his SN2 test ROMs not be put into ROM packs. Propane13's GitHub collection of Synthpopalooza's demos says he "has no rights" to them.
- **Trebor palettes** (GCCFINAL 2013, 2X7-PROSYSTEM 2013, PRO PALETTES V1–V7 2021–2025, §8): no licence, permission or usage terms in any post (text-searched). They are nonetheless shipped by A7800, the MiSTer core repo (Trebor submitted the `.pal` set), JS7800 and A7800DS. V7's "Emulators" folder embeds the tables in GPL-family source files (ProSystem `Palette.cpp`, Stella `PaletteHandler.cxx`, Gopher2600 `color.go`): do not open those. Before shipping any Trebor table, get explicit terms from Trebor; the alternative is to generate a palette from the published phase steps (25.7°/26.7°/27.7°).
- **MiSTer Atari7800 core**: RevEng (2025-06-01) said "I think she licensed it as GPL3" (Kitrinx's core). That contradicts the brief's "MIT overall" assumption, so check LICENSE and per-file headers at the pinned commit. The 2026 nightlies "Replace 6502, pokey, other vhdl" (§9), so pins after mid-2026 contain new files of unknown licence; the JT51 YM2151 core is a separate concern.
- **BupSystem** (TailChao): closed source; "If the source is ever released it'll be under zlib" (2021-04-20, §19). Use it as a binary oracle only (Windows).
- **test7800** (JetSetIlly): its CPU, TIA and RIOT come from Gopher2600 (GPL family per the brief), so never open its source; binaries and README are fine (§14).
- **Atari 7800+ / 2600+ emulator**: a port of JS7800 (raz0red, §15), which is on the brief's never-open list. The original RetroArch ProSystem core is GPL, never open.
- **a7800**: a MAME 0.188 fork (§7). RevEng quoted its DMA-shutdown rounding (`(maria_cycles+3)/4`) in a post (§10); that is recorded here as a fact, not code. Verify per-file licences before opening any a7800 source.
- **Firmware/ROM content**: Atari BIOS images (bundled in A7800's "Optional Files"), the Atari/CPS Diagnostic Test cartridge and Trebor's ROM PROPack (commercial ROMs plus demos patched by RevEng) are copyrighted third-party content: do not redistribute or commit them. 7800OpenBIOS (CC0, per the brief) is the clean alternative.
