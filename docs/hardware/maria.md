# MARIA, the 7800's graphics chip

MARIA builds each scan line from display lists in memory. It stops SALLY with HALT, fetches the line's objects into
line RAM in one burst, and plays the other line-RAM buffer out as pixels. `src/maria.c` models the registers, the
line events, the display-list walk, the burst, line RAM, the picture and the DLI, and keeps statistics for every
line. The bus that grants the burst its time is in `docs/hardware/bus.md`.

Every constant on this page is written down with its source before any ROM runs, and the tests in
`tests/maria/test_maria.c` and `tests/frame/test_frame.c` take their expected values from here, not from the
emulator's output.

Sources:

- **The Software Guide**, the 7800 Software Guide on 7800.8bitdev.org, rev 1320: the list formats, Appendix 3's
  DMA costs, the registers and the read modes.
- **The MARIA notes**, `.planning/preparation/A7800-MARIA.md`, which gather the facts below with their own sources:
  a re-simulation of the PLA in MiSTer's Atari 7800 core at `0dc8ad2e3ff7` (facts only), MAME, a7800, A7800Hawk
  and measurements posted on AtariAge. Section numbers below are that file's.

## Registers

The XG-1 sensor samples the selected native color at emission, before host
conversion. The renderer carries the original raster and absolute pixel
tick, including x319 at the next line origin. Without caller rows it
computes only the scheduled sensor pixels; a missing output buffer cannot
change electrical input. See [controller timing](controllers.md#xg-1).
Native row241 ends at raster258 tick0; MSTAT enters VBLANK at tick42.
The renderer flushes the old raster before swapping line RAM, so the
scheduled optical response at ticks64..93 retains that final visible
pixel even when the host frame has already returned.

MARIA answers $20-$3F of pages 0-3. `reg` holds the registers, indexed by address minus $20 (MARIA notes,
section 2.4):

| Address | Register | Used by the burst |
|---------|----------|-------------------|
| $20 | BACKGRND | |
| $21-$23, $25-$27, $29-$2B, $2D-$2F, $31-$33, $35-$37, $39-$3B, $3D-$3F | palettes 0-7, colours 1-3 | |
| $24 | WSYNC | |
| $28 | MSTAT | |
| $2C, $30 | DPPH, DPPL | the DLL's address, read at VBE |
| $34 | CHARBASE | the high byte of character graphics |
| $38 | OFFSET | writes are ignored |
| $3C | CTRL | character width (bit 4), kangaroo mode (bit 2), read mode (bits 1-0) |

CTRL's other bits are colour kill (bit 7), the DMA mode (bits 6-5) and the border (bit 3). The burst samples CTRL
once, at its start: the CPU is halted for the whole burst, so nothing can write it in between (section 5).

## Display lists

**The DLL.** Each zone has a 3-byte entry: byte 0 holds DLI (bit 7), H16 (bit 6), H8 (bit 5) and OFFSET, the zone's
height minus 1 (bits 3-0); bytes 1 and 2 hold the zone's DL address, high byte first (section 2.1). VBE loads the DLL
pointer from DPPH and DPPL and reads zone 0's entry: its three bytes at T0 + 2, + 4 and + 6, 7 ticks in all
(section 4.2). Each later entry is read at the end of the previous zone's last line.

**OFFSET.** It counts down from the zone's height minus 1 to 0, one per line. The line where it is 0 is the zone's
last line. Each line restarts the DL walk from the zone's DL base, so a DL that the CPU changes between lines shows
on the next line (section 2.4).

**Headers.** b1 is a header's second byte (section 2.2):

1. `(b1 & 0x1F) != 0`: a 4-byte header. b0 = graphics address low, b1 = palette (bits 7-5) and W (bits 4-0),
   b2 = address high, b3 = hpos.
2. Else, if `b1 & 0x40`: a 5-byte header. b1 = WM (bit 7), 1, IND (bit 5), 00000; b2 = address high;
   b3 = palette and W; b4 = hpos.
3. Else the terminator: `(b1 & 0x5F) == 0`. Bits 7 and 5 are ignored, so $00, $20, $80 and $A0 all end the list,
   while $40, $60, $C0 and $E0 are 5-byte headers. Ninja Golf and Crossbow depend on it (section 9).

The width is 32 - W: 1-31 bytes for a 4-byte header, and 32 for a 5-byte header with W = 0. hpos counts 160-pixel
cells and wraps at 8 bits; cells 160-255 are off screen and keep nothing.

**Write mode.** Only a 5-byte header loads WM. It persists across headers, lines, zones and frames, and 4-byte headers
inherit it. It is 0 at power-on. IND applies to its own header only (section 2.2).

**Addresses.** Direct graphics byte i is read at hi:lo + i + OFFSET × 256; an object may cross a page. An indirect
object's character map entry i is read at hi:lo + i with no OFFSET, and character c's graphics at
CHARBASE:c + OFFSET × 256, with a 2-byte character's second byte at + 1 (section 2.3).

**Holes.** A graphics address is in a hole when A15 is set and either H16 is set with A12, or H8 with A11. The test
is made after OFFSET is added (section 2.3).

**What a DMA read returns.** RAM answers: $1800-$27FF and its mirrors at $0040-$00FF and $0140-$01FF. The TIA,
MARIA and RIOT selects are gated off while MARIA drives the bus, so their addresses return the floating byte, the
previous fetch's, with no side effect. Cartridge space goes to the board, which answers MARIA as it answers the CPU,
or leaves the floating byte where it drives none. Every fetch leaves its byte on the bus (section 4.7).

## DMA cost

The burst starts at T0, when MARIA's PLA takes the bus. Each fetch has its own address tick (section 4.2):

| Step | Ticks | Source |
|------|-------|--------|
| PLA start, to the first address | 2 | MARIA notes, section 4.2 (PLA re-simulation) |
| DL or DLL byte | 2 each | section 4.2 (PLA re-simulation) |
| 4-byte / 5-byte header | 8 / 10 | Software Guide, Appendix 3; 8 measured (section 4.8) |
| Direct graphics byte | 3 | Software Guide, Appendix 3; measured (section 4.8) |
| Indirect character, 1-byte / 2-byte | 6 / 9: a map read of 3, then 3 per graphics byte | Software Guide, Appendix 3 |
| Terminator | 8: all four bytes are read | section 4.2 (PLA re-simulation) |
| Tail, ordinary line | 1: HALTRST, then OFFSET - 1 | section 4.2 (PLA re-simulation) |
| Tail, last line | 7: the next DLL entry's three bytes, HALTRST with the third | section 4.2 (PLA re-simulation) |
| Line | 454 | section 4.1; measured (section 4.8) |
| DMA request | t 28 of the line | Software Guide; measured (section 4.8) |

**The closed form.** HALTRST comes at T0 + 9 + D on an ordinary line and at T0 + 15 + D on a zone's last line,
where D is the sum of the header, graphics, character and hole costs. The terminator's four bytes are addressed at
T0 + 2 + D, + 4, + 6 and + 8, so its tail of 1 or 7 gives 9 or 15. A DLI adds nothing (section 4.5). The
empty-DL costs are therefore 9 and 15 ticks from T0.

Worked examples, the ones `maria.dma_line.*` checks:

| DL | Ordinary line | Last line |
|----|---------------|-----------|
| terminator only | T0 + 9 | T0 + 15 |
| one 4-byte header, width 1 | T0 + 9 + 8 + 3 = T0 + 20 | T0 + 26 |
| one 5-byte header, width 32 | T0 + 9 + 10 + 96 = T0 + 115 | T0 + 121 |
| one 5-byte indirect header, two 2-byte characters | T0 + 9 + 10 + 18 = T0 + 37 | T0 + 43 |

**Holes.** A holey slot costs one slot of its object's kind and ends the object: 3 for a direct byte, so a direct
object whose first byte is in a hole costs header + 3, as measured; 6 or 9 for an indirect character, whose map is
still read and whose graphics bytes are still addressed. A hole in the middle of an object ends it after that slot.
Nothing is drawn for a holey slot (section 4.4).

**The abort at LRC.** LRC is t 0 of the next line, 454 ticks after the line's t 0, where line RAM swaps. A fetch
whose address tick is at or after LRC is dropped and the burst ends. Every graphics byte addressed before LRC is
still written, so the line shows the objects completed so far and part of the one in flight. HALTRST then comes at
LRC + 3 on an ordinary line, where OFFSET is still decremented, or at LRC + 9 on a last line, where the next DLL
entry is still read (section 4.6). There is no fixed cap of 426 or 430 ticks.

For example, five 4-byte objects of width 31 cost 8 + 93 = 101 ticks each. Object k's graphics byte j is addressed
at T0 + 10 + 101k + 3j. With LRC = T0 + 454, object 3's last byte is at T0 + 403; object 4's bytes 0-13 come at
T0 + 414 to T0 + 453 and byte 14, at T0 + 456, is dropped. Four objects complete, 138 graphics bytes are fetched, and
HALTRST is T0 + 457, against T0 + 514 without the abort.

## The HALT handshake

The burst does not start the moment MARIA asks for HALT. The bus lets SALLY run two more cycles after /HALT falls
(`docs/hardware/bus.md`, HALT), and the PLA then starts at T0 = /HALT fall + 6. The 6 is fitted. With it, the first
DMA address, T0 + 2 = fall + 8, is exactly the end of a fast second cycle, so a write the CPU makes in that cycle is
visible to the line's fetch.

SALLY loses ceil((HALTRST - fall) / 4) whole cycles. For an empty DL:

- ordinary line: HALTRST = T0 + 9 = fall + 6 + 9 = fall + 15, so ceil(15 / 4) = 4 cycles = 16 ticks;
- last line: HALTRST = T0 + 15 = fall + 6 + 15 = fall + 21, so ceil(21 / 4) = 6 cycles = 24 ticks.

Both match the 16 and 24 ticks measured on hardware for DLLs pointing at empty DLs, start-up and shut-down included
(section 4.3). Measuring T0 from the fall rather than from t 28 keeps both totals independent of where t 28 falls
within a CPU cycle.

## Line RAM and read modes

Line RAM has two buffers of 160 cells. Line n's burst fills one while line n's pixels play the other, and both swap
at t 0 (section 3.3). A cell holds 5 bits: a 3-bit palette and a 2-bit data pair, stored as palette × 4 + pair.

**Writing** (section 3.1):

| WM | Cells per byte | Cell k | Written when |
|----|----------------|--------|--------------|
| 0 | 4 | palette P2 P1 P0; pair D(7-2k) D(6-2k) | its pair is not 00, or in kangaroo mode |
| 1 | 2 | k = 0: palette P2 D3 D2, pair D7 D6; k = 1: palette P2 D1 D0, pair D5 D4 | its pair is not 00, or in kangaroo mode |

hpos moves on by 4 cells (WM 0) or 2 (WM 1) per byte; a 2-byte character writes 8 or 4 cells. In write mode 1 a cell
whose pair is 00 is not written even when D3 or D2 is set. Kangaroo mode writes every cell and keeps its palette
bits; it changes nothing else.

**Reading.** The read mode (CTRL bits 1-0; 1 behaves as 0) and WM name the six modes. The read mode changes only how
a cell turns into pixels, never what the burst writes (section 3.2):

| Mode | WM / RM | Palette; left / right 320-pixel colour |
|------|---------|----------------------------------------|
| 160A | 0 / 00 | P2 P1 P0; the pair for both |
| 160B | 1 / 00 | P2 D3 D2; D7 D6 for both |
| 320A | 0 / 11 | P2 P1 P0; D7 0 / D6 0 |
| 320B | 1 / 10 | P2 0 0; D7 D3 / D6 D2 |
| 320C | 1 / 11 | P2 D3 D2; D7 0 / D6 0 |
| 320D | 0 / 10 | P2 0 0; D7 P1 / D6 P0 |

## Line events and the frame

MARIA's clock is the tick count, never the DMA. `origin` is the tick of line 0, t 0 of the current frame; a line is
454 ticks and a frame 263 lines, 119,402 ticks. The origin moves on by a frame when line 263's t 0, the next
frame's line 0, is reached. Each line has up to four events (section 4.1):

| t | Lines | Event |
|---|-------|-------|
| 0 | every line | LRC: the previous raster's last pixel plays, line RAM swaps, the new write buffer clears, MARIA's WSYNC releases, and a burst in flight is aborted |
| 28 | 17-258 | with CTRL's DMA mode 10, the HALT request for the line's burst; else the line is marked DMA off |
| 42 | 16, 258 | MSTAT's VBLANK clears on line 16 and sets on line 258 |
| 43 | 16 | with DMA on, VBE: the DLL pointer from DPPH and DPPL and zone 0's entry |

The bus fires every event whose tick is at or before a cycle's data tick, before that cycle's HALT, RDY and NMI
logic (`docs/hardware/bus.md`, Line events). The burst requested at t 28 runs when the HALT window reaches c2 (The
HALT handshake, above).

**The frame boundary.** `muhtari_run_frame` runs from one VBLANK start, line 258, t 42, to the next. By then line
258's t 0 has played raster 257's last pixel, so a frame holds every raster from 16 to 257 whole. The run stops at
the first instruction boundary at or after the boundary; the overshoot carries into the next frame, so frames never
drift.

## Rendering

MARIA writes the native colour byte of each pixel straight into the caller's rows; the core keeps no frame buffer.
Pixel x of raster n, x from 0 to 319, plays at t 135 + x of line n, from the buffer that line n - 1's burst filled.
Each 160-pixel cell gives two 320-pixel pixels. Frame rows 0-241 are rasters 16-257; rasters 258-262 and 0-15 are
blanked and written nowhere (sections 3.3 and 6).

**Spans.** Pixels are played in spans with the registers as they stand, so a mid-line colour change lands on the
pixel it should (section 10):

- a write to any register but WSYNC, MSTAT and OFFSET first plays every pixel up to its data tick with the old
  value; the pixel after shows the new one;
- the t 0 event plays the rest of the previous raster before line RAM swaps.

**A pixel's colour.** A cell holds palette P2 P1 P0 in bits 4-2 and its data pair in bits 1-0. The read mode is
CTRL's bits 1-0 at the moment the span plays (section 3.2):

| Read mode | Palette | Left pixel's colour | Right pixel's colour |
|-----------|---------|---------------------|----------------------|
| 0, 1 (160A, 160B) | P2 P1 P0 | the pair | the pair |
| 2 (320B, 320D) | P2 0 0 | bit 1, then bit 3 | bit 0, then bit 2 |
| 3 (320A, 320C) | P2 P1 P0 | bit 1, then 0 | bit 0, then 0 |

Colour 00 shows BACKGRND ($20); palette p colour c is the register at $20 + 4p + c. Colour kill, CTRL bit 7, masks
the byte with $0F. No burst runs before line 17, so rasters 16 and 17 play cleared buffers and show only BACKGRND.
The border (CTRL bit 3) lies outside the 320 pixels and is not emitted.

**No video.** With a NULL `video`, the spans write nothing but advance exactly as they would, so a frame without
video leaves MARIA, the CPU and RAM in the same state as a frame with it.

## The DLI

A DLL entry with DLI set (bit 7) raises one NMI, when the burst on the previous zone's last line reads that entry.
NMI goes low on the second φ1 after /HALT rises at the burst's HALTRST (System notes, section 2, C6: MiSTer's
timing, fitted). SALLY's φ1s fall on its 4-tick grid from c2, where the bus runs the burst, so the edge is the
second grid tick after HALTRST: in the hold's last cycle, or at its end when HALTRST is on the grid. Where SALLY
latches the edge and after which instruction it takes the NMI are in `docs/hardware/bus.md` and
`docs/hardware/sally.md`, NMI. A DLI adds no DMA ticks (section 4.5).

MARIA raises the edge through `muhtari__bus_raise_nmi` and never retries. A 1 means the bus queued the edge, and
MARIA records it as the line's `DLI_QUEUED` flag; the latch can still drop a queued edge (`docs/hardware/bus.md`,
NMI).

**A first-zone DLI.** VBE reads zone 0's entry at t 43 of line 16 with no HALT window and ends 7 ticks later, at
t 50. A DLI set on that entry raises NMI on the second φ1 after t 50, the burst's rule with VBE's end for HALTRST.
SALLY's φ1s are its cycle starts: the end of the cycle in flight at t 43, 4 or 6 ticks long, then every 4 ticks.
MiSTer and A7800Hawk fire this DLI after VBE as well. Line 16 runs no burst and keeps no statistics, so the sticky
feature mask alone records the DLI and whether the bus queued it. The `dli` ROM's `zone0` test times it.

## The first frame after a load

The fast-boot hand-off puts the beam at line 240, t 24, where the NTSC BIOS leaves it (System notes, section 7,
C10). These conventions follow from that:

- **Frame 1** is the 8,190 ticks from the hand-off to VBLANK start: 54 audio samples, (8,190 x 88) / 13,125
  rounded down. Each later frame is 119,402 ticks, and its count is (carried remainder + elapsed x 88) / 13,125,
  800 or 801.
- **Its rows.** Rows 0-223, rasters 16-239, have not been scanned since the load and are written `$00`. Rows
  224-241 show what MARIA's hand-off state draws: CTRL $60 has DMA off; if the cartridge turns DMA on before line
  258, the DL at $2200 and the DLL entries from $1FF3 lie in RAM the cartridge has not yet written, which reads
  zero under the default fill, so the lines are empty and show BACKGRND. At the next VBE, DPP $1F84 is reloaded.
  The picture is deterministic, and it is not the BIOS's own.
- **The CPU's phase.** The hand-off fetch starts at t 24, a multiple of 4, so SALLY's cycle grid has offset 0
  against the beam. The value is derived from C10 and fitted; an RTL simulation would confirm it.

## Statistics

Every burst records its line in `stats`, one entry of 8 bytes for each of the 263 NTSC lines:

| Field | Meaning |
|-------|---------|
| `cost` | HALTRST - T0, in ticks |
| `bytes` | graphics, character-map and character-graphics bytes fetched; holey slots are not counted |
| `headers` | object headers read whole; the terminator is not counted |
| `objects` | objects whose every fetch ran, a holey object included |
| `flags` | `LAST` (the zone's last line, the next entry read), `ABORTED` (the burst reached LRC), `DLI` (the entry read has DLI set), `DLI_QUEUED`, `DMA_BLOCKED` and `DMA_OFF` |

`DLI_QUEUED`, `DMA_BLOCKED` and `DMA_OFF` are for the line events that run the burst: the bus queued the DLI's NMI
edge, the bus refused the line's HALT request, and CTRL had DMA off.

The feature mask, `features`, is sticky: a bit sets on the first line that uses a feature and stays set. Each object
sets its read mode (RM0 for read modes 0 and 1, RM2, RM3), kangaroo mode, write mode 1, the zone's H8 or H16 and,
when indirect, 1-byte or 2-byte characters. A line that reads an entry with DLI set sets DLI. A line with no object
sets no mode bit. The `DLI_QUEUED` and `DMA_BLOCKED` bits are set with the line flags of the same names.

## Diagnostic expected values

Three diagnostic ROMs measure what a program on the console can, mostly through the RIOT timer. Every value in
`tests/diag/expected.csv` is derived here from the constants above before the ROM first runs; the emulator's output
is never copied into a gating row, and a row changes only when its derivation here is shown wrong.

| ROM | Mailbox id | Records, in order | Derived in |
|-----|------------|-------------------|------------|
| `tests/diag/dma.s` | 1 | 27: `null` to `over_16` | the table below |
| `tests/diag/cpu.s` | 2 | 4: `tia`, `riot_ram`, `riot`, `ram` | The cpu ROM |
| `tests/diag/dli.s` | 3 | 11: `zone1`, `zone2`, `zone0`, `parked`, `wsync_1`, `wsync_3`, `rdy_store`, `set_lines`, `clear_lines`, `set_polls`, `clear_polls` | The dli ROM |
| `tests/diag/modes.s` | 7 | 2: `at_4000`, `zones` | The modes ROM |

Record n has test id n and is the n-th row for its ROM in `expected.csv`.

**Not measured here.** Five checks wait for runs on hardware. A 5-byte header with width 0 is taken as 32 bytes,
which no source has measured. Kangaroo mode and 320B's transparent pairs show only in the picture, so frame hashes
cover them: the samples' and the `modes` ROM's (The modes ROM). INPTCTRL's two-write HALT gate runs before
any cartridge code on a console, since the BIOS writes INPTCTRL first; the bus tests cover it. The RIOT's timer
edges are covered by the RIOT tests. A raster-numbered background split, one colour change per line, would fix the
first visible line and the first pixel on a capture.

The `dma` diagnostic ROM (`tests/diag/dma.s`) measures DMA cost as CPU cycles lost to HALT.

**The measurement.** Each test runs in one frame. In VBLANK the ROM points DPPH and DPPL at the test's DLL and turns
DMA on, so VBE at line 16 loads zone 0's entry; when MSTAT clears it turns DMA off again, so lines 17 and on run no
burst and zone 0 stays unwalked. It counts lines with MARIA's WSYNC to line 99, waits past that line's t 28, writes
the test's CTRL and writes WSYNC once more. From line 100's t 0 it runs:

| Code | Cycles | φ2 count p |
|------|--------|------------|
| `stx TIM1T` with X = 255 | 4, the write in the last | 0 |
| `nop` three times | 6 | 1-6 |
| `lda INTIM` | 4, the read in the last | 7-10 |

So line 100 has the only burst of the frame, zone 0's first line. With zone 0's OFFSET at 1 that line is an
ordinary line; with OFFSET 0 it is the zone's last line.

**The timer.** On the 1-cycle interval INTIM reads N − p (`docs/hardware/riot.md`, Timer), and the timer counts every
φ2, frozen cycles included. With no burst, p = 10 and INTIM reads 255 − 10 = **245**. A burst inside the window
freezes c2 for h more cycles, so INTIM reads 245 − h.

**The window holds the burst.** WSYNC releases the next read at the first 4-tick cycle start at or after t 0, so the
`stx` opcode fetch starts at t φ, φ from 0 to 3, and its slow RIOT write ends at t φ + 18. The NOP cycles start at
t φ + 18, + 22, + 26 and + 30; /HALT falls at the first of them at or after t 28 (p = 3 or 4), and c2 is p = 5 or 6.
All of it lies inside the window for every φ, and nothing before the timer write is held.

**The lost cycles.** The bus freezes c2 for h = ceil((HALTRST − fall) / 4) cycles (`docs/hardware/bus.md`, HALT),
and HALTRST − fall = 6 + cost (The HALT handshake), where cost is T0 + 9 + D or T0 + 15 + D from the closed form. So

- ordinary line: h = ceil((15 + D) / 4);
- last line: h = ceil((21 + D) / 4).

Neither depends on φ. INTIM = 245 − h, and the core's statistics for line 100 show cost = 9 + D or 15 + D.

The tests, in the ROM's order. Objects are direct unless marked, their graphics at $E000 + OFFSET × 256, outside
any hole; "4-byte" and "5-byte" name the header. Each object costs its header (8 or 10) plus 3 per direct byte,
or 6 (1-byte) or 9 (2-byte) per indirect character. `off` writes CTRL with DMA off, so line 100 runs no burst.

| Test | Zone 0's DL | D | Line | h | INTIM | Source |
|------|-------------|---|------|---|-------|--------|
| `null` | terminator only | 0 | ordinary | ceil(15 / 4) = 4 | 241 | hw: the 16 ticks measured for an empty DL |
| `null_last` | terminator only | 0 | last | ceil(21 / 4) = 6 | 239 | hw: the 24 ticks measured |
| `off` | DMA off | - | - | 0 | 245 | guide: the RIOT datasheet's timer |
| `h4_w1x8` | eight 4-byte, width 1 | 8 × 11 = 88 | ordinary | ceil(103 / 4) = 26 | 219 | guide |
| `h5_w1x8` | eight 5-byte, width 1 | 8 × 13 = 104 | ordinary | ceil(119 / 4) = 30 | 215 | guide |
| `h4_w8` | one 4-byte, width 8 | 8 + 24 = 32 | ordinary | ceil(47 / 4) = 12 | 233 | guide |
| `h5_w32` | one 5-byte, width 32 (W = 0) | 10 + 96 = 106 | ordinary | ceil(121 / 4) = 31 | 214 | guide |
| `h5_w32_last` | one 5-byte, width 32 | 106 | last | ceil(127 / 4) = 32 | 213 | guide |
| `dir_w8` | one 5-byte, width 8 | 10 + 24 = 34 | ordinary | ceil(49 / 4) = 13 | 232 | guide |
| `ind1_w8` | one 5-byte indirect, 8 characters of 1 byte | 10 + 8 × 6 = 58 | ordinary | ceil(73 / 4) = 19 | 226 | guide |
| `ind2_w8` | the same, 2-byte characters (CTRL bit 4) | 10 + 8 × 9 = 82 | ordinary | ceil(97 / 4) = 25 | 220 | guide |
| `term_00` | b1 = $00 | 0 | ordinary | 4 | 241 | guide: the terminator rule |
| `term_20` | b1 = $20 | 0 | ordinary | 4 | 241 | guide |
| `term_40` | b1 = $40: a 5-byte header, width 8 | 34 | ordinary | 13 | 232 | guide |
| `term_80` | b1 = $80 | 0 | ordinary | 4 | 241 | guide |
| `term_a0` | b1 = $A0 | 0 | ordinary | 4 | 241 | guide |
| `term_c0` | b1 = $C0: a 5-byte header, write mode 1, width 8 | 34 | ordinary | 13 | 232 | guide |
| `hole8_h4` | H8; eight 4-byte, width 8, at $C800 | 8 × (8 + 3) = 88 | ordinary | 26 | 219 | hw: header + 3 |
| `hole8_h5` | H8; eight 5-byte, width 8, at $C800 | 8 × (10 + 3) = 104 | ordinary | 30 | 215 | hw |
| `hole16_h4` | H16; eight 4-byte, width 8, at $D000 | 88 | ordinary | 26 | 219 | hw |
| `hole16_h5` | H16; eight 5-byte, width 8, at $D000 | 104 | ordinary | 30 | 215 | hw |
| `hole8_ctl` | H8; eight 4-byte, width 8, at $D000 | 8 × 32 = 256 | ordinary | ceil(271 / 4) = 68 | 177 | guide |
| `over_11` | eleven 4-byte, width 8 | 11 × 32 = 352 | ordinary | ceil(367 / 4) = 92 | 153 | guide |
| `over_12` | twelve 4-byte, width 8 | 384 | ordinary | ceil(399 / 4) = 100 | 145 | guide |
| `over_13` | thirteen 4-byte, width 8 | aborted | ordinary | 107 or 108 | 137-138 | rtl: the abort tail |
| `over_14` | fourteen | aborted | ordinary | 107 or 108 | 137-138 | rtl |
| `over_16` | sixteen | aborted | ordinary | 107 or 108 | 137-138 | rtl |

**Terminators.** Byte 1 of zone 0's first header takes each value of section 2.2's list. $00, $20, $80 and $A0 end
the list there, so the line costs what an empty DL costs. $40 and $C0 make a 5-byte header whose byte 3, $18, gives
width 8, followed by a terminator; $C0 also loads write mode 1, which costs nothing.

**Holes.** The address bits are tested after OFFSET is added, so zone 0's OFFSET of 1 puts the graphics at $C900,
$D100 and $E100. $C900 has A15 and A11, a hole under H8; $D100 has A15 and A12, a hole under H16; $E100 has neither.
Each object's first byte is in the hole, so it costs its header + 3 and ends. `hole8_ctl` sets H8 over the $D100
objects, which is not a hole under H8, so they cost their full 32. H8 and H16 sit in zone 0's DLL entry, bits 5
and 6, not in CTRL.

**Overrun.** Object k of width 8 has its header bytes at T0 + 2 + 32k and its graphics bytes at T0 + 10 + 32k + 3j,
the last at T0 + 31 + 32k. LRC is line 101's t 0. /HALT fell at t 28 + δ, δ from 0 to 3 by φ, so T0 = t 34 + δ and
LRC = T0 + 420 − δ, between T0 + 417 and T0 + 420.

- With twelve objects the terminator is read at T0 + 386 to + 392, before LRC, and HALTRST is T0 + 393.
- With thirteen or more, object 12's last byte at T0 + 415 runs and the terminator's (or object 13's header) second
  byte, at T0 + 420 at the earliest, does not. Thirteen objects complete and the burst aborts, so HALTRST is
  LRC + 3, cost = 423 − δ, from 420 to 423, and h = ceil((429 − δ) / 4) is 108 for δ = 0 and 107 otherwise.
  INTIM reads 137 or 138, and the value stops falling as objects are added.

The ROM's code fixes φ, but deriving it would mean following every cycle since the hand-off, so the rows give the
range. With the burst ending after line 101's t 0, line 101 and line 102 may run a burst too before the ROM turns
DMA off; the INTIM read comes before either freezes the CPU, as the read is c0 or c1 of line 101's window.

**The statistics.** `tests/diag/stats.csv` gives, for every test but `off`, the cost and objects that line 100's
entry in the core's per-line statistics must show (Statistics, above): cost 9 + D on an ordinary line or 15 + D on
a last line, and every object whose fetches all ran, holey ones included. The overrun rows give cost 420-423 and
13 objects.

The source class is the one `tests/diag/expected.csv` records: `hw` for a value that rests on a hardware
measurement, `guide` for one that rests on the Software Guide or a datasheet, `rtl` for the PLA re-simulation and
`fit` for a fitted value.

### The cpu ROM: slow accesses

The `cpu` diagnostic (`tests/diag/cpu.s`) times 64 × `LDA` absolute from one address, with DMA off for the whole
run. A slow cycle takes 6 ticks instead of 4, its data cycle only, and the RIOT timer counts it once like any other
cycle (`docs/hardware/bus.md`, Clocks; `docs/hardware/riot.md`, Timer). So INTIM alone cannot see the 2 ticks. The
ROM ends the run with MARIA's WSYNC instead. The hold that follows lasts until the next line's t 0 and counts every
held cycle, so the run's extra ticks come back as fewer held cycles.

**The run.** `sta WSYNC` starts it. The next opcode fetch runs at t φ of the next line, φ from 0 to 3, the first
cycle start at or after t 0 (`docs/hardware/bus.md`, RDY). From there, in ticks from t 0:

| Code | Ticks | Ends at | φ2 count p |
|------|-------|---------|------------|
| `ldx #255` | 8 | φ + 8 | |
| `stx TIM1T`, the write slow | 12 + 6 | φ + 26 | 0 |
| `lda.w ADDR` × 64 | 64 × 16, or 64 × 18 when ADDR is slow | φ + 1050, or φ + 1178 | 1-256 |
| `sta WSYNC` | 12, the write's data at the end | φ + 1062, or φ + 1190 | 257-259 |
| held fetch | H × 4 | t 1362 + φ | 260 to 259 + H |
| `lda INTIM`, the read slow | 12 + 6 | | 263 + H |

Both data ticks lie between t 908 and t 1362, so the WSYNC releases at t 1362, the third line's t 0. The held
fetch repeats whole 4-tick cycles until one starts at or after it: H = ceil((1362 − 1062 − φ) / 4) = 75 after fast
reads and H = ceil((1362 − 1190 − φ) / 4) = 43 after slow ones. Neither depends on φ, as 300 and 172 are multiples
of 4. The 128 ticks the slow reads add are 32 fewer held cycles.

**The timer.** TIM1T = 255 sets the 1-cycle interval, so INTIM reads 255 − p up to p = 255. From p = 256 it reads $FF
and counts down once per cycle, which is (255 − p) mod 256 again (`docs/hardware/riot.md`, Timer). The read is at
p = 263 + H:

| Test | Address | Device | Cycle | H | p | INTIM | Source |
|------|---------|--------|-------|---|---|-------|--------|
| `tia` | `$0000` | TIA | slow | 43 | 306 | 205 | hw: +2 ticks per slow access, measured |
| `riot_ram` | `$0480` | RIOT RAM | slow | 43 | 306 | 205 | hw: RIOT RAM is slow, measured |
| `riot` | `$0280` | RIOT ports | slow | 43 | 306 | 205 | hw |
| `ram` | `$1800` | RAM | fast | 75 | 338 | 173 | hw: 113 held cycles per WSYNC line, measured |

The measurements are the AtariAge threads "7800 hardware facts", "7800 Software Guide updated" and "Improving the
emulators" (`.planning/preparation/CONFORMANCE.md`, Sources). The difference, 32, is the measured +2 ticks per
access. If every cycle of a slow-access instruction were slow, the run would take 64 × 24 ticks, release at t 1816,
and the slow rows would read 187 or 188 by φ. The `lda.w` form keeps `$0000` absolute, so each test is the same four
cycles: three fast fetches from the cartridge and one data cycle.

### The dli ROM: DLI, WSYNC, RDY and MSTAT

The `dli` diagnostic (`tests/diag/dli.s`) times the DLI's NMI, MARIA's WSYNC, a store right after a WSYNC and both
MSTAT edges. Each test starts the timer, 255 to TIM1T, in the first instruction after a WSYNC releases, so p = 0 is
that write and INTIM reads (255 − p) mod 256, as in the cpu ROM.

**The phase rule.** After the release, cycle i starts at t φ + 4i + 2s, where s counts the slow cycles before it.
A later line's t 0 or t 28 is d lines on, and 454 ≡ 2 (mod 4), so the first cycle start at or after it lies φ ticks
past it when s and d have the same parity, and the cycle's index is then the same for every φ. That holds for every
row below except `zone0`, whose event is not on a line boundary and which comes out the same anyway. A burst does not
move the grid: its frozen cycles are 4-tick cycles, counted.

**The DLI tests.** In VBLANK the ROM points DPPH and DPPL at the test's DLL and turns DMA on. It counts lines with
WSYNC from MSTAT's set edge, so the timer starts at t 0 of line 16 (17 for `zone2`). Then `stx TIM1T` (cycles 0-3,
the write slow), `lda $80` (4-6) and two-cycle NOPs from cycle 7, so each NOP's opcode fetch has an odd index. The
NMI handler's first instruction is `lda INTIM`. Each zone of these DLLs is the empty DL.

- `zone1`: zone 0 has OFFSET 0, so line 17 is its last line, and zone 1's entry has DLI. /HALT falls at the first
  cycle start at or after t 28 of line 17: s = 1, d = 1, so it is cycle (482 − 2) / 4 = 120. c2 is cycle 122, the
  second cycle of the NOP whose opcode was cycle 121, and the empty last line freezes it ceil(21 / 4) = 6 cycles,
  to cycle 128. NMI falls on the second φ1 after HALTRST = fall + 21, at fall + 28, which the frozen cycle 126
  latches. The NOP polled at cycle 121, before the edge, so the CPU takes the NMI after the next NOP, cycles
  129-130. The entry is 131-137 and `lda INTIM` reads in cycle 141: p = 138, INTIM **117**.
- `zone2`: zones 0 and 1 have OFFSET 0 and zone 2's entry has DLI, so the burst of line 18 raises it. The timer
  starts on line 17, whose burst freezes 6 cycles of the sled: an even number, so the NOPs keep their odd opcode
  cycles. Line 18 is d = 1 on, so everything follows as in `zone1`: **117**.
- `zone0`, a first-zone DLI: zone 0's entry has DLI, which VBE reads at t 43 of line 16 (The DLI). VBE ends 7 ticks
  later, at t 50, and the edge falls on the second φ1 after that. Cycle k ≥ 4 starts at t φ + 4k + 2. For φ = 0 the
  φ1s after t 50 are cycles 13 and 14, so the edge is at cycle 14's start and cycle 13 latches it; for φ from 1 to 3
  they are cycles 12 and 13, and cycle 12 latches it. Either way the NOP whose opcode is cycle 13 is the first to
  poll at or after the latch, so the NMI follows it: entry 15-21, the read in cycle 25, p = 22, INTIM **233**.
- `parked`: the `zone1` DLL, and the CPU parked in WSYNC while line 17's burst raises the DLI. After the timer
  write come `bit $00` (a slow TIA read, cycles 4-6) and `sta WSYNC`, which releases at line 17; `sta WSYNC` again
  there, before t 28; then a NOP. Line 17's burst runs inside the hold and its edge is latched there. The hold ends
  at line 18's t 0: s = 2, d = 2, so the NOP's opcode fetch is cycle (908 − 4) / 4 = 226. That fetch is the NOP's
  deciding cycle and the edge came before it, so the NMI follows the NOP: entry 228-234. DMA is still on, and line
  18 is zone 1's first line, so its burst runs too: /HALT falls at cycle 233, at t φ + 28 of line 18, and c2 is
  cycle 235, the handler's opcode fetch, frozen ceil(15 / 4) = 4 cycles. `lda INTIM` reads in cycle 242: p = 239,
  INTIM **16**. The first derivation left line 18's burst out and gave 20; the first run read 16, and the burst
  accounts for the 4 cycles.

With these instructions the MiSTer edge (the second φ1 after HALTRST) and the Software Guide's (one tick after the
shutdown) both fall inside the frozen c2 and give the same rows. An NMI at the burst's start, as MAME raises it,
would be latched in c1 and taken after the NOP of cycle 121: `zone1` and `zone2` would read 119.

**WSYNC.** With DMA off, `sta WSYNC`, `stx TIM1T`, k more `sta WSYNC` and `lda INTIM`. The last hold ends at the
k-th line's t 0, s = 1, so for odd k the fetch after it is cycle (454k − 2) / 4 and INTIM is read three cycles later:

| Test | k | Fetch | p | INTIM | Source |
|------|---|-------|---|-------|--------|
| `wsync_1` | 1 | 113 | 113 | 142 | hw: $8E per line, measured |
| `wsync_3` | 3 | 340 | 340 | 171 | hw |

$8E is the "Improving the emulators" thread's figure. A held read takes the release only at a cycle start, so the
lines alternate 113 and 114 cycles, 227 per pair.

**RDY after a write.** `rdy_store` runs with DMA off, zone 0 loaded at VBE with OFFSET 1 and left unwalked, as in
the dma ROM. From line 100's t 0: `stx TIM1T`, `sta WSYNC`, then on line 101 two NOPs, `sta CTRL` with DMA on, and
`lda INTIM`. The first NOP's opcode fetch is the read RDY holds, cycle 113 at t φ (s = 1, d = 1). The `sta CTRL`
write is cycle 119, its data at t φ + 28, so the request at t 28 comes first and sees DMA off: no burst. `lda INTIM`
reads in cycle 123, p = 120, INTIM **135**. If the cycle right after a write could not be held, as MiSTer's notes
say, the first NOP's opcode would run before the hold and every later cycle 4 ticks sooner. The write would land at
t φ + 24, before the request; the line's empty burst would freeze `lda INTIM`'s third cycle 4 cycles, and INTIM
would read 132.

**MSTAT.** DMA is off. The ROM waits for VBLANK to clear, on line 16, and then counts lines with `sta WSYNC`, `inx`,
`bit MSTAT`. The `bit` reads at t φ + 20, before t 42, so a line sees the edges of earlier lines only:

| Test | What | Value | Source |
|------|------|-------|--------|
| `set_lines` | lines from line 16 until the first that reads VBLANK set: line 259 | 243 | rtl: set on line 258 |
| `clear_lines` | lines from there until the first that reads it clear: line 17 | 21 | rtl: clear on line 16 |
| `set_polls` | from line 258's t 0, polls of `iny`, `bit MSTAT`, `bpl` until it reads set | 2 | rtl: the edge at t 42 |
| `clear_polls` | the same from line 16's t 0 until it reads clear | 2 | rtl |

For the polls, the WSYNC loop releases into `dex` and `bne` not taken, and each poll is 8 cycles: the first reads at
t φ + 36, before t 42, and the second at t φ + 68, after it. An edge at t 0, as MAME puts it, would give 1; an edge
past t 68 would give 3 or more. The lines are MiSTer's `video_sync.sv` (`VBLANK_START` 258), as
`.planning/preparation/CONFORMANCE.md` cites it, and the poll counts its edge at column 0, t 42.

### The modes ROM: read modes, kangaroo mode and 2-byte characters

The `modes` diagnostic (`tests/diag/modes.s`) draws what no sample does: 160B, 320B and 320D (read mode 2), kangaroo
mode and 2-byte indirect characters. Its picture is the evidence, pinned by a frame hash; its mailbox records two
numbers, and the core's feature mask shows each mode (Statistics, above).

It is a plain SuperGame board of 4 banks, 64 KiB, each bank starting with its own number, and runs from the last bank
at $C000. Its DLL is a 16-line empty zone, then five 16-line zones each followed by an 8-line empty zone, then seven
16-line empty zones, past line 258. Only the five drawn zones' entries have DLI set. Each DLI's handler counts it and
writes CTRL from a table, so a zone's bursts and rasters all run in its own mode: the edge comes on the last line of
the empty zone before it, and the next one comes 24 lines later. In VBLANK the ROM sets CTRL back to 160A and the
count to 0. Every object has a 5-byte header, so each sets its own write mode.

| Zone | CTRL | Objects |
|------|------|---------|
| 1, 160B | $40: read mode 0 | two direct, write mode 1 |
| 2, 320B | $42: read mode 2 | two direct, write mode 1 |
| 3, 320D | $42: read mode 2 | two direct, write mode 0 |
| 4, kangaroo | $44: kangaroo, read mode 0 | two overlapping direct, write mode 0 |
| 5, 2-byte characters | $50: 2-byte characters, read mode 0 | one indirect, write mode 0 |

- `at_4000`: a plain SuperGame board maps its second-to-last bank at $4000-$7FFF (`docs/hardware/cartridge.md`,
  SuperGame). With 4 banks that is bank 2, so `lda $4000` reads **2**. Source: guide.
- `zones`: the DLIs counted from MSTAT's clear edge to its set edge in one frame after DMA is turned on. Each entry
  with DLI set raises one NMI when the previous zone's last burst reads it (The DLI), the edges are 24 lines apart,
  and zone 0's entry has none, so the count is the number of drawn zones, **5**. Source: guide.

## Fitted values

These values come from the RTL model or a fit, not from a hardware measurement. Each stays until the observation
named beside it is made, on muhtari's own diagnostic ROMs or a capture:

| Value | Chosen | Would change if |
|-------|--------|-----------------|
| Visible rows | 242 (NTSC), 292 (PAL): rasters 16-257 and 16-307 | a line-locked capture shows a 243rd unblanked raster, or content on raster 258 |
| MSTAT's edges | set on line 258 and clear on line 16, both at t 42 | `dli`'s `set_lines`, `clear_lines`, `set_polls` or `clear_polls` reads other than 243, 21, 2 and 2 on hardware |
| First pixel | t 135 | a hardware capture puts the first pixel at t 133 or 134 |
| HALT latency and handshake | two cycles after the fall, then T0 = fall + 6 | an empty-DL loop on hardware measures other than 16 and 24 ticks, or a cycle-counted loop disagrees |
| DLI to NMI | NMI on the second φ1 after /HALT rises | `dli`'s `zone1` or `zone2` reads other than 117 on hardware, the NMI a cycle earlier or later against WSYNC |
| First-zone DLI | NMI on the second φ1 after VBE's end, t 50 of line 16 | `dli`'s `zone0` reads other than 233 on hardware |
| The CPU's phase at the hand-off | 0, from the hand-off at t 24 | an RTL simulation of the hand-off gives another offset |
| RDY after a write | the NMOS rule: RDY holds the first read after the write | `dli`'s `rdy_store` reads 132, the store 4 ticks earlier, on hardware |
| Indirect hole | 6 or 9 ticks, the character's whole slot | hardware measures 3, as a7800 charges |
| Holey DMA | draws nothing | a capture shows the first holey byte drawn |
