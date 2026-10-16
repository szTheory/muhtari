---
title: "A7800-MARIA: display lists, DMA timing, line RAM, video output and colour"
summary: "What maria.c must do: DLL/DL decode, read/write modes, a per-fetch DMA tick schedule decoded from the MiSTer MARIA PLA and reconciled with MAME, a7800, A7800Hawk, the Software Guide and AtariAge measurements, line RAM, per-pixel register effects, frame geometry, palettes and artifacting."
read_when: "Before writing or changing maria.c, the DMA burst, MSTAT/DLI timing, the frame format or palette helpers, or when a game's picture or timing diverges."
updated: 2026-10-15
---

# A7800-MARIA

## Where to look

| Question | Section |
|---|---|
| DLL/DL formats, terminator, 5-byte detection, width, CHARBASE/OFFSET, page crossing, latching, write mode | 2 |
| Line RAM cells, read modes, transparency, 320B pair rule, kangaroo, border, colour kill, DMA off | 3 |
| Line timeline, per-fetch DMA schedule, 16/24, "+17", holes, cap/abort, DMA reads of $0000–$3FFF | 4.1–4.7 |
| Cross-source table and recommended named constants | 4.8 |
| Mid-line register effects, CTRL latency, MSTAT, register reads | 5 |
| Visible rows, PAR/DAR, overscan, frame rates, scaling, CRT vs LCD | 6 |
| Palettes, hue step, licences | 7 |
| Composite artifacting | 8 |
| Games that expose emulator divergences | 9 |
| maria.c design (burst, spans, stats) and corrections to the design spine | 10 |
| Open questions and risks | 11 |

## 1. Conventions
- tick = one MARIA clock: NTSC 315/44 MHz = 7,159,090.9 Hz, PAL 7,093,788 Hz; a line is 454 ticks [MAR.02][MAR.09].
- **Line n** starts at t = 0 = MiSTer column 412 of row n−1: the line-RAM swap ("LRC"), which is also the MARIA WSYNC release. MiSTer column = (t + 412) mod 454. The DMA request then falls at t = 28, the guide's "7 CPU cycles into each scan line" [MAR.09][MAR.14]. The pixels of line n belong to raster n (MiSTer row n; VSYNC = rows 0–2).
- **(derived†)**: from a functional re-simulation of the `DMA.sv` PLA, written and run here. It reads the 48 product terms and 20 output planes as data and adds the register transfers coded around them. It reproduces every per-item Software Guide cost exactly (§4.2), but it is not a Verilator run of the RTL.

## 2. Display structure
**2.1 DLL entry** (3 bytes; the pointer is stored high byte first) [MAR.14][MAR.06]

| Byte | Content |
|---|---|
| 0 | b7 DLI (NMI after the DMA of the last line of the *previous* zone) · b6 H16 · b5 H8 · b4 0 · b3–0 OFFSET = zone height − 1 |
| 1, 2 | DL address high, low |

**2.2 DL header decode** (b1 = the header's second byte) [MAR.06][MAR.01][MAR.15]
1. `(b1 & 0x1F) != 0`: **4-byte** header. b0 = graphics address low, b1 = PPPWWWWW, b2 = address high, b3 = hpos.
2. Else, if `b1 & 0x40`: **5-byte** header. b1 = WM (b7) · 1 · IND (b5) · 00000; b2 = address high; b3 = PPPWWWWW; b4 = hpos.
3. Else **terminator**, i.e. `(b1 & 0x5F) == 0`; b7 and b5 are ignored (Ninja Golf, Crossbow). MARIA still fetches all four bytes of the terminating header, which costs 8 ticks (derived†).
- Width = 32 − W (W is the 5-bit field). That gives 1–31 for 4-byte headers; a 5-byte header with W = 0 means 32. The re-simulation reproduces 31 and 32 bytes (derived†).
- The palette is 3 bits. hpos counts 160-px cells: 0–159 are visible, and the value wraps at 8 bits (§3.3).
- **WM** is loaded only by 5-byte headers. It persists across headers, lines, zones and frames, and 4-byte headers inherit it [MAR.06][MAR.14]. **IND** lasts for its own header only [MAR.14].

**2.3 Address formation** (all arithmetic 16-bit, so carries cross pages) [MAR.06][MAR.01][MAR.14]

| Fetch | Address |
|---|---|
| DLL entry | ZONE_PTR, auto-incremented; loaded from DPP at the VBE fetch |
| DL byte | DL_PTR, restarted from the zone's DL base on every line |
| Direct graphics byte i | (hi:lo + i) + (OFFSET << 8); an object may straddle a page ($A3FF → $A400, derived†) |
| Character map entry i | hi:lo + i, with **no** OFFSET |
| Character graphics | (CHARBASE:c) + (OFFSET << 8); the second byte of a 2-byte character is +1, carrying into CHARBASE+1 when c = $FF |
| Hole test | on the OFFSET-added graphics address: A15 and ((H16 and A12) or (H8 and A11)) |

OFFSET counts down to 0 within a zone. H16 hides odd 4K blocks and H8 hides odd 2K blocks, only above $8000 [MAR.14].

**2.4 Latching and persistence**
- DPPH/DPPL are read once per frame, at the VBE fetch on line 16 (§4.1) [MAR.14][MAR.06][MAR.01].
- DLL entry z+1 is fetched in the last 6 PLA ticks of the DMA on zone z's last line, and still after an abort (§4.6). Zone 0's entry is fetched at VBE (derived†). A later write to an entry already fetched waits a frame. DL bytes and graphics are re-read every line, so a CPU write made between DMAs shows on the next line.
- WM at power-on: MARIA does not initialise it [MAR.14]. MAME, a7800 and MiSTer reset it to 0 [MAR.01][MAR.03][MAR.06]. A7800Hawk clears it at every frame start, which is a deviation [MAR.05].

**Choice 2.A (WM power-on).** Recommendation: 0 at power-on, never reset afterwards. Strongest alternative: a seeded random bit taken from the RAM-fill profile, which would catch homebrew that never sends a 5-byte header. Would change if: a game's first frame is found to differ with WM = 1.

## 3. Line RAM and read modes
**3.1 Write side** (one 5-bit cell per 160-px position) [MAR.07][MAR.01][MAR.22]

| WM | Cells per byte | Cell k | Cell written when |
|---|---|---|---|
| 0 | 4 | P2 P1 P0 · D(7−2k) D(6−2k) | its 2 data bits ≠ 00, or kangaroo mode |
| 1 | 2 | k=0: P2 D3 D2 · D7 D6; k=1: P2 D1 D0 · D5 D4 | its low pair (D7 D6, or D5 D4) ≠ 00, or kangaroo mode |

hpos advances by 4 cells (WM 0) or 2 cells (WM 1) per byte. An indirect 2-byte character writes 8 or 4 cells.

**3.2 Read side** (RM = CTRL bits 1–0; RM 1 behaves as RM 0) [MAR.07][MAR.01][MAR.14]

| Mode | WM / RM | Palette; colour of the left / right 320-px pixel |
|---|---|---|
| 160A | 0 / 00 | P2P1P0; data pair (both pixels) |
| 160B | 1 / 00 | P2 D3 D2; D7 D6 (both); 12 colours, values 0, 4, 8, 12 transparent |
| 320A | 0 / 11 | P2P1P0; D7 0 / D6 0 |
| 320B | 1 / 10 | P2 0 0; D7 D3 / D6 D2 |
| 320C | 1 / 11 | P2 D3 D2; D7 0 / D6 0 |
| 320D | 0 / 10 | P2 0 0; D7 P1 / D6 P0 |

- Colour 00 shows BACKGRND. In a written cell, an "off" 320-px pixel is therefore opaque background, not transparent.
- **320B pair rule.** A cell with D7 = D6 = 0 is not written even if D3 or D2 is set. A written cell shows BACKGRND for any of its pixels whose colour is 00. MiSTer derives this from MARIA's LINBUF sheet, and MAME behaves identically. The guide's prose ("either off → background, both off → transparent") is silent on the D3/D2-only case [MAR.07][MAR.01][MAR.14]. A7800Hawk gates 160B/320B cells on any of D7 D6 D3 D2, which is wrong for 160B given the guide's "0, 4, 8, 12 transparent" [MAR.05].
- **320A quirk.** RevEng: a lit and an unlit pixel in one cell show the unlit one as background (T4). This is the same cell rule [MAR.25].
- **Kangaroo** (CTRL b2): every cell is written, and colour-00 pixels show BACKGRND. MiSTer and MAME keep the palette bits (visible in 320B/C/D); Hawk writes 0 [MAR.07][MAR.01][MAR.05].
- **Border** (CTRL b3): outside the 320-px window the picture is black (b3 = 0) or BACKGRND (b3 = 1) [MAR.07][MAR.14].
- **Colour kill** (CTRL b7): output byte & $0F, border included [MAR.10][MAR.01]. Physically the burst is removed and the TV drops chroma.
- **DMA off** (DM = 11): the buffers still swap and clear, so the window shows BACKGRND [MAR.07][MAR.14]. In the test modes (DM = 00/01) the PLA free-runs and drives the address bus without HALT, so it fights the CPU (derived†). CPUWIZ warns these modes can damage consoles, and Tomcat F-14 writes $00 to CTRL briefly at boot (T4) [MAR.22][MAR.21].

**3.3 Mechanics** [MAR.07][MAR.09]
- Two buffers of 160 cells. Line n's DMA fills one buffer while line n's pixels play the other. Both swap at t = 0, and the new write buffer is flash-cleared (MiSTer keeps an occupancy bit per cell).
- hpos is 8-bit: cells 160–255 are off-screen scratch, and writes wrap from 255 to 0, so an object can enter at the left edge [MAR.07][MAR.01][MAR.05].
- Pixel x (0–319) plays at t = 135 + x in MiSTer; a7800 uses 134 + 2·cell and Hawk 133 + x [MAR.09][MAR.04][MAR.05].

**Choice 3.A (transparency).** Recommendation: the MiSTer/MAME cell rule above. Strongest alternative: Hawk's 4-bit gate. Would change if: a hardware capture of a 320B pair with only D3/D2 set shows a coloured pixel.

**Choice 3.B (DMA test modes).** Recommendation: treat DM 00/01 as DMA off and count them in the diagnostics. Strongest alternative: model the bus fight. Would change if: a title is found that depends on them (none known).

## 4. DMA timing
**4.1 Line timeline, line n** (identical in ticks for NTSC and PAL) [MAR.09][MAR.14][MAR.21]

| t | MiSTer column | Event |
|---|---|---|
| 0 | 412 of row n−1 | LRC: line-RAM swap + clear; MARIA WSYNC release; DMA abort deadline; last pixel of raster n−1 |
| 28 | 440 | HBLANK starts; DMA HALT is requested on lines 17–258 (PAL 17–308) when DM = 10 |
| 42 | 0 of row n | HSYNC; MSTAT VBLANK clears on line 16 and sets on line 258 (PAL 308) |
| 43 | 1 | VBE fetch (line 16 only) |
| 80–98 | 38–56 | colour burst |
| 110–134 | 68–92 | left border (HBLANK is t = 28–109) |
| 135–454 | 93–412 | the 320 pixels of raster n; right border t = 1–27 of line n+1 |

TailChao measured (T4): HALT falls ≈ 28 ticks into the line, just after PHI2 falls [MAR.21].

**4.2 Per-fetch schedule**, in ticks after the PLA start T0 (derived†)

| Activity | Ticks | Detail |
|---|---|---|
| PLA start | 2 | MARIA owns the address bus from T0 |
| DL or DLL byte | 2 each | addresses at T0+2, +4, … |
| 4-byte / 5-byte header | 8 / 10 | |
| Direct graphics byte | 3 | first address 2 ticks after the last header byte; line-RAM write 2 ticks after its address |
| Indirect character, 1 byte / 2 bytes | 6 / 9 | map read 3 (no OFFSET) + 3 per graphics byte |
| Hole | one slot of the object's kind (3 / 6 / 9) | then the object ends; with 2-byte characters both graphics bytes are still fetched |
| Terminator | 8 | full 4-byte read |
| Tail, other line | 1 | HALTRST, then OFFSET − 1 |
| Tail, last line | 7 | 2 states + 3 DLL bytes; HALTRST together with the third |
| VBE | 7 total | 2 + 3 DLL bytes |

In closed form, HALTRST comes at T0 + 9 + D on an ordinary line and T0 + 15 + D on a last line, where D is the sum of the header, graphics, character and hole costs. A DLI adds 0 ticks. The per-item costs match Software Guide Appendix 3 exactly (8 / 10 / 3 / 6 / 9) [MAR.14]. The last line adds 6 raw ticks; §4.3 reconciles that with the guide's +8.

**4.3 The 16 and 24 reconciled.** RevEng measured DLLs pointing at empty DLs (T4, measured): 16 ticks (4 CPU cycles) on other lines and 24 (6 CPU cycles) on a zone's last line, start-up and shut-down included [MAR.18]. Those totals already contain the terminator's 8 ticks. In the RTL they come out as PLA 10 or 16 ticks plus a HALT handshake of ≈ 6 ticks, i.e. 16 or 22, and 22 rounds up to the next CPU cycle, 24 (derived). MAME's "+6" is the raw DLL fetch. a7800's "+8" rounds early and over-charges one CPU cycle whenever D ≡ 1 or 2 (mod 4): a last line with one 5-byte header costs 8 CPU cycles in the RTL and MAME, but 9 in a7800 and by the guide's sum (derived) [MAR.01][MAR.03]. The handshake belongs to SYSTEM's `halt_window()`: ≈ 6 ticks, plus 0–3 alignment on fast cycles or 0–5 on slow ones, usually on alternate lines [MAR.14][MAR.06].

**4.4 Holes, reconciled**

| Source | Cost of a holey object | Picture |
|---|---|---|
| RevEng 2023: 8 × 5-byte objects = +24; bsteux 2023: 4-byte header 8 + 3 = 11 (T4, measured) | header + 3 (direct mode) | nothing drawn (bsteux: hardware ≈ a7800) [MAR.18] |
| Software Guide rev 1320 | 3, once per object, whatever the width | — [MAR.14] |
| a7800 | +3 at the first hole byte, direct and indirect | nothing [MAR.03] |
| MAME | 0 (hole bytes skipped) | nothing [MAR.01] |
| A7800Hawk | first slot 3 / 6 / 9, the rest free | nothing [MAR.05] |
| MiSTer RTL | one slot of the object's kind, then the object ends | see §11 for the pinned commit [MAR.06][MAR.13] |

wavemotion: ProSystem-derived emulators charge the full fetch even inside holes (T4) [MAR.19]. AlyoshaTAS: charging the first hole slot fixes the water in Scrapyard Dog's sewer level (T4) [MAR.22]. A hole in the middle of an object also ends it after its slot (derived†).

**4.5 The "+17 DLI overhead".** MAME and a7800 add 17 ticks to a last-line DMA when the next DLL entry has DLI set [MAR.01][MAR.03]. It is not a DMA cost. In the RTL, INTENBL sets DLI during the DLL fetch, one tick before HALTRST, and NMI is pulled low 1–3 CPU cycles after HALT falls (end_sr[2:1]) (derived†) [MAR.06]. The guide gives 1 tick from shutdown to DLI, and Hawk raises NMI 1 tick after the DMA [MAR.14][MAR.05]. The 17 ticks stand in, in lumped models, for the release latency, the halted instruction's remaining cycles and the 6502's NMI recognition: about 4 CPU cycles. A cycle-level CPU that samples NMI every cycle must not add it (derived). On zone 0, the DLI fires after the VBE fetch on line 16 in MiSTer and Hawk, but after line 16's merged DMA in MAME and a7800. TailChao calls Xenophobe "super fussy" about this (T4) [MAR.21].

**4.6 Cap, abort and what the CPU sees**
- MiSTer has no tick budget. LRC (t = 0 of the next line, 426 ticks after the t = 28 request) forces an abort [MAR.06][MAR.09]. After it (derived†):
  - a graphics byte whose address tick is before LRC is still written; later fetches are dropped;
  - HALTRST follows at LRC + 3 on an ordinary line (OFFSET is still decremented), or LRC + 9 on a last line (the next DLL entry is still fetched, and its DLI still raised);
  - the CPU then runs only ≈ 5 CPU cycles (ordinary line) or ≈ 3–4 (last line), depending on the handshake, before the next request at t = 28. The CPU is effectively "halted to the next line" (derived).
- Effective fetch window ≈ 426 − (6 to 12) ≈ 414–420 ticks (handshake + PLA start) (derived). Measured and estimated budgets: Pat Brady ≈ 413, or 415 with no TIA/RIOT access during DMA; RevEng 410 (T4) [MAR.20]. MAME: 426 − 16 = 410; a7800: 430 − 16 = 414 [MAR.01][MAR.03].
- On overrun MAME and a7800 spin the CPU to the next scanline, which leaves 7 CPU cycles before the next DMA [MAR.01][MAR.03]. Hawk forces shutdown at t = 453 and lets it spill into the next line [MAR.05].
- An overloaded line shows partial objects (A7800 2021, bsteux 2023; T4) [MAR.27][MAR.18].

**4.7 DMA reads of $0000–$3FFF**
- RAM ($1800–$27FF, and its page 0/1 mirrors $0040–$00FF and $0140–$01FF) reads normally. DLLs and DLs must sit in RAM or anything that answers within the 2-tick window [MAR.14][MAR.08].
- TIA, MARIA and RIOT selects are gated off while MARIA drives the bus, so those reads return the floating bus: the last byte on the bus, i.e. the previous fetch (MiSTer) [MAR.08][MAR.10]. On hardware, MARIA reads its own palette registers as noise and TIA/RIOT space as chaotic data (TailChao, T4, measured) [MAR.21]. a7800 returns 0 below $20; MAME reads through the CPU map, with device side effects [MAR.03][MAR.01].
- $0400–$17FF and $2800–$3FFF belong to the cartridge (HSC, POKEY, XM, …); where nothing answers, the bus floats.

**Choice 4.A.** Recommendation: the floating-bus value, with no device side effects. Strongest alternative: 0 (a7800). Would change if: a game or test relies on a specific value (none known).

**4.8 Comparison and recommended constants** (ticks)

| Item | MAME | a7800 | A7800Hawk | MiSTer | Guide | Measured (T4) |
|---|---|---|---|---|---|---|
| DMA request | t = 28 | t = 28 | t = 28 | t = 28 | 7 CPU cycles | ≈ 28 [MAR.21] |
| Start + stop | 16 lump | 16 | 2 + halt + 6 / 12 | PLA 2 + terminator 8 + tail 1 / 7, handshake ≈ 6 | 16 / 24 | 16 / 24 [MAR.18] |
| Header; graphics; character | 8/10; 3; 6/9 | same | same | same | same | 8, 10, 3 [MAR.18] |
| Last line | +6 | +8 | +6 | +6 | +8 | +8 after CPU rounding |
| DLI | +17 | +17 | 0, NMI +1 tick | 0, NMI 1–3 cycles after HALT | 1 tick | — |
| Hole | free | +3 once | first slot | slot, then the object ends | 3 once | header + 3 |
| Cap | 426 from start | 430 | t = 453 | abort at LRC | 426 implied | ≈ 410–415 usable [MAR.20] |
| Overrun, CPU | next line + 7 cycles | same | spills over | LRC + 3 / + 9, then ≈ 4 cycles | — | partial objects |
| Mid-line registers | no | per 2 ticks | per tick (RM per line) | per tick | — | needed [MAR.18] |

Recommended: `MARIA_LINE_TICKS 454` [MAR.09][MAR.20] · `MARIA_DMA_REQ_T 28` [MAR.14][MAR.21] · `MARIA_DMA_ABORT_T 454` (= t 0 of the next line) [MAR.06] · `MARIA_PLA_START 2` · `MARIA_DL_BYTE 2` · `MARIA_GFX_BYTE 3` · `MARIA_CHARMAP 3` · `MARIA_TERMINATOR 8` · `MARIA_TAIL_OTHER 1` · `MARIA_TAIL_LAST 7` · `MARIA_VBE_LEN 7` · `MARIA_ABORT_TAIL_OTHER 3` · `MARIA_ABORT_TAIL_LAST 9` · hole = one slot of the object's kind · `MARIA_DLI_EXTRA 0` (all derived†, cross-checked against [MAR.14]) · `MARIA_FIRST_PIXEL_T 135` [MAR.09] · HALT handshake: SYSTEM, fitted to the empty-DL 16/24 [MAR.18].

**Choice 4.B (overrun).** Recommendation: abort at LRC, then the RTL tails. Strongest alternative: MAME's 426-tick lump cap. Would change if: a hardware objects-per-line overrun test disagrees with the RTL window by one object or more.

## 5. Per-pixel effects and register semantics
- **Sampling.** MiSTer registers every output pixel per tick from the live colour map, RM, border and colour-kill bits, so mid-line changes land with 320-px resolution [MAR.07][MAR.10]. a7800 (since v5.0) samples per 160-px cell, Hawk per tick but with RM latched once per line at t = 133, and MAME once per line [MAR.04][MAR.05][MAR.01][MAR.27].
- **Colour RAM / BACKGRND writes** land at the start of the write cycle's phase 2 and show from the next tick on [MAR.08]. MiSTer notes, but leaves disabled, a one-pixel hold on colour-RAM writes ("ugly"; unverified on hardware) [MAR.07].
- **CTRL** is a two-stage latch that takes effect at the phase 1 after the write, i.e. half a CPU cycle later; each write of a read-modify-write gets its own phase 1 [MAR.08]. The DMA-side bits (DM, character width, kangaroo) and CHARBASE/DPP cannot change during DMA, because the CPU is halted, so they are sampled at the PLA start.
- **MSTAT ($28)** drives only D7 (VBLANK); D6–D0 float. The CPU latches as phase 2 opens, so an edge during the read is seen one cycle late [MAR.08]. The edge sits part-way through the line (TailChao, T4, measured) [MAR.21].
- **Every other MARIA address is write-only.** Reads float (MiSTer). That fits GroovyBee's $00 from `lda P0C1,y` on a PAL console (abs,Y leaves $00 on the bus) and TailChao/RevEng's "unstable in every mode" (T4, measured) [MAR.08][MAR.23][MAR.21]. MAME and a7800 return 0 [MAR.01][MAR.03]. Writes to $38 are ignored. A write to $24 (WSYNC) holds RDY low until t = 0 [MAR.08][MAR.10].
- **Games and tests.**
  - RevEng's 7800test tests 6 and 13 need mid-line colour changes (AlyoshaTAS, T4) [MAR.18].
  - DLI colour splits must land in HBLANK and show up as glitch lines only when timing is wrong: Choplifter's horizon, Summer Games' diving, Ace of Aces' sky, Karateka's meter [MAR.15].
  - Super Huey's lower-right flicker and Water Ski's ghost rope also happen on hardware, so muhtari must reproduce them [MAR.15].

**Choice 5.A.** Recommendation: per-tick sampling, via spans (§10). Strongest alternative: per 160-px cell (a7800). Would change if: the speed budget on low-power targets forces it; the span cost is paid per write, not per pixel.

## 6. Video output
**6.1 Rows.** 242 rows for NTSC and 292 for PAL: rasters 16–257 and 16–307.
- For: the MiSTer VBLANK decode read off the schematic (set at 258/308, cleared at 16), and MAME's `lines − 5` [MAR.09][MAR.01]. Also the guide's "starts on raster 16 and ends on raster 258", read with the guide's own exclusive convention ("41 … 233, 192 scan lines later") [MAR.14].
- Against: the guide's ASCII diagram (25 + 192 + 26 = 243; 293 for PAL), a7800's screen of rasters 16–258 / 16–308, Hawk's 243 MSTAT-clear lines, and Trebor's "MARIA outputs 243 visible lines" (T4, basis not given) [MAR.14][MAR.04][MAR.05][MAR.26].
- Content: in the RTL model, VBE comes on line 16 and the first DMA on line 17, so zone 0's first line appears on raster 18. Rasters 16–17 always show BACKGRND. The DMAs on lines 257–258 feed rasters 258–259, which are blanked (derived) [MAR.06][MAR.07][MAR.09]. Hawk agrees (content from raster 18). MAME and a7800 merge VBE into line 16's DMA, so their content starts at raster 17: a one-row shift between emulators [MAR.05][MAR.01][MAR.03].

**Choice 6.A.** Recommendation: emit 320 × 242 / 292, rasters 16–257 / 16–307. Strongest alternative: 243 / 293 (a7800). Would change if: a line-locked capture shows a 243rd unblanked raster, or content on raster 258.

**6.2 Geometry and rates** (derived; square-pixel rates 12 3/11 MHz and 14 3/4 MHz [MAR.30])

| | NTSC | PAL |
|---|---|---|
| Tick vs subcarrier | 2 × fsc exactly | fsc from a separate 4.43 MHz crystal [MAR.20] |
| Line | 15,768.92 Hz; 227 fsc cycles (not 227.5) | 15,625.08 Hz; 283.75 fsc cycles |
| Frame | 119,402 ticks; 39375000/656711 = 59.95788 Hz | 142,102 ticks; 3546894/71051 = 49.92040 Hz |
| 320-px window | 44.70 µs | 45.11 µs |
| PAR of a 320-px pixel, per 240p/288p line | 6/7 = 0.8571 (160-px pixel: 12/7) | 1.0396 (160-px pixel: 2.0793) |
| DAR of 320 × all rows | 960/847 = 1.1334 (242 rows) | 1.1393 (292 rows) |
| DAR of 320 × 224 / 274 rows | 60/49 = 1.2245 | 1.2142 |
| DAR of 372 px (borders) × all rows | 1.3176 | 1.3245 |

MiSTer's "Original" aspect settings (2979/2626 = 1.1344, 3200/2611 = 1.2256, 3471/2632 = 1.3188; PAL 1819/1595 = 1.1404) agree to within 0.1 % [MAR.12].
- **Overscan.** The guide's safe area is rasters 41–232 (192 rows) for NTSC and 41–280 (240) for PAL, i.e. frame rows 25–216 and 25–264 [MAR.14]. A forum survey of CRTs found 190–239 visible lines, about 60 % near 224 (T4) [MAR.21]. MiSTer's "no overscan" crop is rasters 24–247 (224) / 24–297 (274) [MAR.09]. MAME crops 27–250 / 35–294 [MAR.02]. A7800 crops ≈ 8 + 8 rows (227), and test7800 needs per-game crops (Commando top row 27, Tiger Heli 23–24) (T4) [MAR.26].
- **Hosts.** Report the PAR-correct DAR, with a 4:3 stretch as an option. For integer scaling, use integer vertical scale with PAR-correct width (MiSTer's V-Integer) or square pixels. 59.958 Hz and 49.920 Hz are not 60 and 50: resample audio or use VRR.
- **CRT vs LCD.** LCD decoders render hue 1 greenish, $1x/$2x/$Ex/$Fx with less red and $5x–$8x with more red than CRTs (MAME notes; Trebor, T4) [MAR.02][MAR.24]. Scanlines and composite filters are host concerns.

**Choice 6.B.** Recommendation: the core reports 320 × H with PAR 6/7 (NTSC) or 1.0396 (PAL), uncropped. Strongest alternative: 4:3, as MAME and ProSystem present it. Would change if: users find the PAR-correct default wrong in RetroArch; then add a core option.

## 7. Colour
- **Byte layout.** Hue in bits 7–4, luma in bits 3–0. Hue 0 is grey; hues 1–15 select 15 taps of one delay line whose spacing comes from an on-chip RC bias. That bias drifts with temperature, and the colour pot trims it [MAR.11][MAR.16][MAR.02]. MiSTer's volt table shows the 16 luma steps are not evenly spaced [MAR.11].
- **Hue step.** Hue 1 is the fixed reference.
  - Nominal: 25.714° (= 180/7), which makes hue 15 ≡ hue 1 [MAR.02][MAR.24].
  - Observed (T4, measured): ≈ 24° when cold (MAME's notes: ≈ 23° for seconds), ≈ 25.7° after about 2 minutes, 26.7–28.7° warm or hot [MAR.24][MAR.02].
  - Wiki: two factory standards, 25.7° and 26.7°; 24.7° cold to 27.7° warm; most of the drift in the first 20 minutes. Choplifter's $E8 ground turns from green to yellow-brown [MAR.16].
  - MiSTer's fit to measured palettes: 25.97 / 26.95 / 27.93° per code, base phase 316.5° [MAR.11].
- **PAL.** Same chip, but chroma comes from its own 4.43 MHz crystal, and hues run one row later (NTSC $3x ≈ PAL $4x) [MAR.24][MAR.20]. MAME's PAL table is simply its NTSC rows rotated by one [MAR.02].
- **Tables and licences.**
  - MAME `a7800.cpp` (BSD-3, Tuccitto): NTSC 25.7° active, with 24.7–27.7° variants commented out [MAR.02].
  - a7800 `drivers/a7800.cpp` (BSD-3): Trebor's Pro Palettes V7, LUM3, for NTSC and PAL at cool 25.7°, warm 26.7° and hot 27.7°. They were committed by their author (Tuccitto = Trebor), so the provenance audit clears them under BSD-3 with attribution [MAR.04].
  - Not usable (licence unknown): MiSTer `rtl/palettes`, `Palette/**`, `assets/*.pal`, and the forum `.pal` files [MAR.24].
  - The wiki is facts only.

**Choice 7.A.** Recommendation: the core emits native bytes (the hash domain). `palette.c`, outside the hash, ships the six a7800 BSD-3 V7 tables, defaulting to WARM (the default in A7800, MiSTer and A7800DS, and Trebor's 26.7°), plus an MIT YIQ generator with the phase step as a parameter. Strongest alternative: the generator only (all own MIT, no attribution). Would change if: the BSD-3 notice in binaries is unwanted, or Trebor grants explicit terms for newer palettes.

## 8. Composite artifacting
- **Mechanism.** On NTSC the pixel clock is exactly 2 × fsc. A light/dark alternation at 320 px is a 3.58 MHz luma signal that the TV decodes as colour; the even or odd phase gives two hues, and 2-pixel pairs give a weaker, striped set. 160 modes show it more weakly. Colour kill and S-Video/RGB/HDMI mods remove it, and the result varies by console (wiki; RevEng, T4) [MAR.16][MAR.25].
- **Stability.** NTSC: 227 fsc cycles per line and 59,701 per frame, so each column's phase is fixed and there is no dot crawl. PAL: 283.75 cycles per line plus the V-switch, with the carrier on its own crystal, so lines blend and hues are unstable (derived) [MAR.20].
- **Games.** Tower Toppler (developed on PAL; striped without composite; mostly blur blending), Jinx, Rikki & Vikki, the PETSCII Robots instruction bullets, a Phoenix work-in-progress (wiki; T4) [MAR.15][MAR.25]. MiSTer and 7800GD offer "composite blending". A7800 shows it only through MAME's HLSL chain, at under 1 fps on an RTX 2070 (T4) [MAR.29][MAR.25].
- **Reference model.** MiSTer's MIT `composite_out`/`composite_decoder` model the board's output stage. Its luma path rolls off at 4–6 MHz, so fsc survives; burst is 40 IRE (0.143 V) against 0.177 V mean chroma; the pixel grid's phase against the carrier is fixed by the chip and was set from a capture [MAR.11]. Its chroma constants come from its own palettes (licence unknown), so reuse the structure, not the tables.

**Choice 8.A.** Recommendation: an optional host-side filter (helper library plus libretro option) over the native 320-px frame, never in the hash. Strongest alternative: RGB output from the core. Would change if: hosts cannot know the column phase. They can: it is the x parity.

## 9. Divergences that games expose

| Game | Symptom when wrong | MARIA behaviour | Seen in |
|---|---|---|---|
| Ninja Golf, Crossbow | squished 160B / garbled modes | terminator `& 0x5F`, WM persistence | naive emulators [MAR.15] |
| Choplifter | horizon line misplaced or moving | DMA costs + DLI latency; the $E8 ground needs the warm palette | [MAR.15][MAR.16] |
| Xenophobe | wrong colour on the title's bottom row, periodic glitches, zone-0 DLI | DMA costs, VBE/first-DLI timing (its 320A ship line also happens on hardware) | BupSystem and others [MAR.15][MAR.21] |
| Summer Games, Ace of Aces, Karateka | flickering line, bouncing sky, flickering meter | where a DLI colour write lands (exact DMA + NMI timing) | [MAR.15] |
| Alien Brigade | corruption with many objects | DMA penalties eat CPU time | [MAR.15] |
| Kung Fu Master, M.I.A. | broken until line-RAM double buffering, or until DMA start was delayed | line RAM; start at t = 28 | MAME before 2013; A7800x [MAR.01][MAR.18] |
| Scrapyard Dog | wrong sewer water | first hole slot charged | MAME (free holes) [MAR.22] |
| Pole Position II, Klax | track selection wrong / black screen | first frame starts in VBLANK; DMA | [MAR.15] |
| Xevious, Plutos | stray or drifting horizontal line | DLI overhead / timing | A7800DS, A7800 [MAR.19][MAR.27] |
| b*nQ | title glitch unless 3 ticks are removed | DMA cost | A7800DS [MAR.19] |
| Tomcat F-14 | crash at boot | CTRL test mode $00 | [MAR.21] |
| Tower Toppler, Jinx | striped graphics | artifacting / blending | all but MiSTer and 7800GD [MAR.15] |
| Commando, Tiger Heli | wrong crop | overscan | test7800 [MAR.26] |
| 7800test tests 6 and 13 | split at the wrong position | mid-line colour | per-line renderers (MAME; a7800 before v5.0) [MAR.18][MAR.28] |
| Super Huey, Water Ski | flickering line, ghost rope | must appear (hardware does it) | — [MAR.15] |

## 10. maria.c recommendation (checked against the design spine)

**Choice 10.A (DMA as a burst at HALT): confirmed.** While MARIA owns the bus, the CPU is halted, so no register, RAM or ROM byte can change underneath it; the schedule depends only on the DL contents, T0 and the abort tick. One call runs every fetch with the exact tick of its address (§4.2), calling the cart's `read(addr, tick, by_maria)` [MAR.06]. It returns the T0..HALTRST span, the DLI tick, the OFFSET/zone update and the line statistics; SYSTEM turns the span into CPU halt cycles (handshake fitted to 16/24). Strongest alternative: per-tick PLA stepping, as Hawk and MiSTer do: the simplest correctness argument, at about 454 steps per line. Would change if: a board reacts to individual MARIA cycles beyond `by_maria`, or a Verilator diff against MiSTer shows a fetch-tick mismatch.

**Choice 10.B (rendering).** Recommendation: render spans from line RAM up to `now` before every write to $20–$3F and at t = 0, taking RM, colour kill and border control per span. Output the native byte, write each 160-px pixel twice, and apply colour kill as & $0F. Strongest alternative: render at end of line and replay a write log. Would change if: profiling shows flushes dominate (they happen per write).

**Choice 10.C (per-line statistics, always on).** Recommendation: about 10 bytes × 263 lines per frame: T0, HALTRST, CPU halt cycles, 4- and 5-byte header counts, graphics bytes, character maps, holes, and flags {overrun with abort tick, DLI, last line, VBE, DMA off, test mode}. Strongest alternative: only in TRACE builds. Would change if: the Cachegrind gate shows more than 1 % cost.

**Corrections to the design spine:**
1. The 16/24 already contain the 8-tick terminator. A last line costs +6 raw and then rounds to the CPU cycle; do not add +8 to raw ticks.
2. A hole costs one slot of the object's kind and ends the object (the 6/9 for indirect objects is unmeasured).
3. No +17: a DLI costs 0 ticks, and NMI timing comes from INTENBL during the DLL fetch plus SYSTEM's NMI latency, not "end + 1".
4. No 426/430 budget: abort at LRC, then tails of +3 / +9.
5. DMA reads of TIA, RIOT and MARIA addresses float, with no side effects.
6. Rows 16–257 are confirmed, but rows 16–17 never carry content, and rasters 258–259 are fetched and then blanked.
7. VBE is its own 7-tick fetch at t ≈ 43 of line 16, not merged into the first line.

## 11. Open questions and risks
- **MiSTer at the pin may draw hole bytes.** In #62 (2026-09-11), MiSTer removed `latch_byte <= ELRWA && ~holey`. My decode says the first byte of a holey object is now written from the bus, which contradicts every software emulator and bsteux's hardware comparison. For holes, pin the oracle to 8c96e1f7cc24 or report it upstream (derived†) [MAR.13].
- **Needs muhtari's own diagnostic ROMs on hardware:** the indirect hole cost (6/9 here vs 3 in a7800), the VBE position (t = 43 vs Hawk's 28), the first content raster (17 vs 18), 242 vs 243 rows, and the first pixel tick (133 / 134 / 135).
- **The HALT handshake (≈ 6 ticks) is fitted, not measured.** It moves every DMA total by up to one CPU cycle.
- **Palette licences:** only the MAME and a7800 BSD-3 tables are clean. No hardware capture of the 320B D3/D2-only case is known.

## Sources

| ID | Tier | Source (URL) | Pin or access date | Supports |
|---|---|---|---|---|
| MAR.01 | T3 (T1 for MAME's own model) | github.com/mamedev/mame `src/mame/atari/maria.cpp` (BSD-3) | dddf4cd1faee | decode, 426 cap, +6/+17, free holes, per-line model |
| MAR.02 | T3 | same repo, `src/mame/atari/a7800.cpp` (BSD-3) | dddf4cd1faee | crystals, screen 27–250 / 35–294, palettes and notes, timers |
| MAR.03 | T3 | github.com/7800-devtools/a7800 `src/mame/video/maria.cpp` (BSD-3) | 7a2afdc1ea08 | 430 cap, +8, hole +3, reads below $20 = 0 |
| MAR.04 | T3 | same repo, `src/mame/drivers/a7800.cpp` (BSD-3) | 7a2afdc1ea08 | pixel timer at 134, rows 16–258, Pro Palettes V7 |
| MAR.05 | T3 | github.com/TASEmulators/BizHawk `…/A7800Hawk/Maria.cs` (MIT) | c9de9a8bff0f | per-tick DMA, 133–452, hole rule, 160B gate, write mode per frame |
| MAR.06 | T3 | github.com/MiSTer-devel/Atari7800_MiSTer `rtl/Maria/DMA.sv` (MIT) + PLA re-simulation (derived†) | 0dc8ad2e3ff7 | formats, fetch schedule, abort, DLI, holes |
| MAR.07 | T3 | same repo, `rtl/Maria/line_ram.sv` (MIT) | 0dc8ad2e3ff7 | cells, write gate, read modes, border, swap |
| MAR.08 | T3 | same repo, `rtl/Maria/control.sv` (MIT) | 0dc8ad2e3ff7 | decode, CTRL latency, MSTAT D7, floating reads |
| MAR.09 | T3 | same repo, `rtl/Maria/video_sync.sv` (MIT) | 0dc8ad2e3ff7 | columns 412/440/0/93–412, VBLANK 16/258/308, crops |
| MAR.10 | T3 | same repo, `rtl/Maria/maria.sv`, `rtl/top.sv` (MIT) | 0dc8ad2e3ff7 | colour kill, RDY at LRC, bus mux |
| MAR.11 | T3 | same repo, `rtl/composite_out.sv`, `rtl/composite_decoder.sv` (MIT) | 0dc8ad2e3ff7 | delay line, RC drift, hue fits, luma path |
| MAR.12 | T3 | same repo, `Atari7800.sv` (MIT), aspect block | 0dc8ad2e3ff7 | "Original" aspect ratios |
| MAR.13 | T1 (what MiSTer does) | same repo, commits #62 vs 8c96e1f7cc24, `DMA.sv` diff | 0dc8ad2e3ff7 | hole write gate removed |
| MAR.14 | T2 | 7800.8bitdev.org/index.php/7800_Software_Guide | rev 1320 | formats, App. 3/4, latching, CTRL, raster ranges |
| MAR.15 | T2 | …/Common_Emulator_Development_Issues | rev 1304 | game list, `& 0x5F`, hardware glitches |
| MAR.16 | T2 | …/Atari_7800_Color_Documentation | rev 1107 | hue drift, Choplifter $E8, artifacting |
| MAR.18 | T4, measured | forums.atariage.com/topic/224025-7800-hardware-facts/ (RevEng 2014, 2023; bsteux 2023; AlyoshaTAS 2017) | accessed 2026-10-15 | 16/24, hole +3, tests 6/13 |
| MAR.19 | T4 | …/topic/343425-maria-dma-cycles/ (wavemotion) | accessed 2026-10-15 | ProSystem holes, Xevious, b*nQ |
| MAR.20 | T4, measured | …/topic/164554-number-of-dma-clock-cycles-per-scanline/ (nichtsnutz, EricBall, RevEng, Pat Brady) | accessed 2026-10-15 | 454 ticks, PAL crystal, 410/413 budgets |
| MAR.21 | T4, measured | …/topic/285921-bupsystem/ (TailChao) | accessed 2026-10-15 | HALT at 28, MSTAT mid-line, noisy reads, Xenophobe, Tomcat, CRT survey |
| MAR.22 | T4 | …/topic/222380-7800-software-guide-updated/ (AlyoshaTAS, CPUWIZ) | accessed 2026-10-15 | line-RAM model, first hole slot, test modes |
| MAR.23 | T4, measured | …/topic/145363-improving-the-emulators/ (GroovyBee) | accessed 2026-10-15 | palette read = $00 on PAL hardware |
| MAR.24 | T4, measured | …/topic/216065, /213725, /322259 (Trebor palette threads) | accessed 2026-10-15 | hue steps, PAL row offset, no palette licence |
| MAR.25 | T4 | …/topic/349547, /385287, /353253 (artifacting threads) | accessed 2026-10-15 | 320A quirk, games, MAME HLSL cost |
| MAR.26 | T4 | …/topic/383566-emulator-test7800-v081/ | accessed 2026-10-15 | 243-line claim, per-game crops |
| MAR.27 | T4 | …/topic/268458-a7800-the-atari-7800-emulator/ | accessed 2026-10-15 | v5.0 mid-line writes, partial objects, scroller line |
| MAR.28 | T4 | …/topic/224052-mess-0153-more-accuracy-for-7800-emulation/ | accessed 2026-10-15 | 2014: mid-line changes missing everywhere |
| MAR.29 | T4 | …/topic/318275-mister-fpga-7800-core/ | accessed 2026-10-15 | "cycle accurate", composite blending |
| MAR.30 | T2 | en.wikipedia.org/wiki/Pixel_aspect_ratio | accessed 2026-10-15 | 12 3/11 MHz and 14 3/4 MHz square-pixel rates |
