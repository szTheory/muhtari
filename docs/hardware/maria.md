# MARIA, the 7800's graphics chip

MARIA builds each scan line from display lists in memory. It stops SALLY with HALT, fetches the line's objects into
line RAM in one burst, and plays the other line-RAM buffer out as pixels. `src/maria.c` models the registers, the
display-list walk, the burst and line RAM, and keeps statistics for every line. The bus that grants the burst its
time is in `docs/hardware/bus.md`.

Every constant on this page is written down with its source before any ROM runs, and the tests in
`tests/maria/test_maria.c` take their expected values from here, not from the emulator's output.

Sources:

- **The Software Guide**, the 7800 Software Guide on 7800.8bitdev.org, rev 1320: the list formats, Appendix 3's
  DMA costs, the registers and the read modes.
- **The MARIA notes**, `.planning/preparation/A7800-MARIA.md`, which gather the facts below with their own sources:
  a re-simulation of the PLA in MiSTer's Atari 7800 core at `0dc8ad2e3ff7` (facts only), MAME, a7800, A7800Hawk
  and measurements posted on AtariAge. Section numbers below are that file's.

## Registers

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
previous fetch's, with no side effect. Cartridge space is the board's; the core does not route it to the board yet,
so it floats too. Every fetch leaves its byte on the bus (section 4.7).

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

## Statistics

Every burst records its line in `stats`, one entry of 8 bytes for each of the 263 NTSC lines:

| Field | Meaning |
|-------|---------|
| `cost` | HALTRST - T0, in ticks |
| `bytes` | graphics, character-map and character-graphics bytes fetched; holey slots are not counted |
| `headers` | object headers read whole; the terminator is not counted |
| `objects` | objects whose every fetch ran, a holey object included |
| `flags` | `LAST` (the zone's last line, the next entry read), `ABORTED` (the burst reached LRC), `DLI` (the entry read has DLI set), `DLI_QUEUED`, `DMA_BLOCKED` and `DMA_OFF` |

`DLI_QUEUED`, `DMA_BLOCKED` and `DMA_OFF` are for the line events that run the burst: the bus took the DLI's NMI
edge, the bus refused the line's HALT request, and CTRL had DMA off.

The feature mask, `features`, is sticky: a bit sets on the first line that uses a feature and stays set. Each object
sets its read mode (RM0 for read modes 0 and 1, RM2, RM3), kangaroo mode, write mode 1, the zone's H8 or H16 and,
when indirect, 1-byte or 2-byte characters. A line that reads an entry with DLI set sets DLI. A line with no object
sets no mode bit. The `DLI_QUEUED` and `DMA_BLOCKED` bits are set with the line flags of the same names.

## Fitted values

These values come from the RTL model or a fit, not from a hardware measurement. Each stays until the observation
named beside it is made, on muhtari's own diagnostic ROMs or a capture:

| Value | Chosen | Would change if |
|-------|--------|-----------------|
| Visible rows | 242 (NTSC), 292 (PAL): rasters 16-257 and 16-307 | a line-locked capture shows a 243rd unblanked raster, or content on raster 258 |
| First pixel | t 135 | a hardware capture puts the first pixel at t 133 or 134 |
| HALT latency and handshake | two cycles after the fall, then T0 = fall + 6 | an empty-DL loop on hardware measures other than 16 and 24 ticks, or a cycle-counted loop disagrees |
| DLI to NMI | NMI on the second φ1 after /HALT rises | the DLI diagnostic shows the NMI a cycle earlier or later against WSYNC |
| RDY after a write | the NMOS rule: RDY never holds a write | the store-timing diagnostic shows a write held |
| Indirect hole | 6 or 9 ticks, the character's whole slot | hardware measures 3, as a7800 charges |
| Holey DMA | draws nothing | a capture shows the first holey byte drawn |
