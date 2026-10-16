---
title: "Atari 7800 system: clocks, line timeline, SALLY, memory map, INPTCTRL, RIOT and boot"
summary: "Tick-level system facts with sources and disagreements: crystals and the one-MARIA-clock tick, the per-line event timeline, SALLY HALT/RDY/NMI rules, the decoded memory map with slow cycles and open bus, INPTCTRL, the 6532 RIOT, the BIOS boot paths and the HLE hand-off state."
read_when: "Before designing or changing the time base, bus, CPU wrapper, RIOT, power-on profile, boot/HLE path or any timing that crosses chips."
updated: 2026-10-15
---

# Atari 7800 system timing and boot

| Question | Section |
|---|---|
| What is one tick, and how many per CPU cycle, line and frame? | 1 |
| What happens at each tick of a line, and where do sources disagree? | 2 |
| When does HALT stop SALLY, for how long; how do RDY and NMI behave? | 3 |
| What answers each address, which cycles are slow, what does an empty read return? | 4 |
| What does INPTCTRL do, and what opens the HALT gate? | 5 |
| RIOT ports, timer, clocking, RAM, power-on state | 6 |
| What the BIOSes do, and what HLE fast boot must reproduce | 7 |
| Which power-on facts become profile fields | 8 |
| Unknowns that need a diagnostic | 9 |

Coordinates: raster (row, col), col 0–453, col 0 = HSYNC start, where the row increments [SYS.09]; software time
**t = (col − 412) mod 454**, t 0 = WSYNC release, the "line start" of the Guide, MAME and A7800Hawk. Units: ticks.

## 1. Clocks and the master tick

| | NTSC | PAL | Basis |
|---|---|---|---|
| Crystal | 315/22 MHz = 14.3181818 MHz (4 × subcarrier) | 14.187576 MHz (board label) | [SYS.12][SYS.18]; MiSTer derives PAL 14.18758 MHz = 3.2 × 4.43361875 MHz [SYS.11] |
| MARIA clock = **1 tick** | 7,159,090.909 Hz (139.68 ns) | 7,093,788 Hz (140.97 ns) | crystal ÷ 2 |
| SALLY φ2 fast (RAM, ROM, MARIA, cart) | 4 ticks = 1,789,772.7 Hz | 1,773,447 Hz | [SYS.01][SYS.10] |
| SALLY φ2 slow (TIA/RIOT selected; every cycle while INPTCTRL bit 1 = 0) | 6 ticks = 1,193,181.8 Hz | 1,182,298 Hz | [SYS.01][SYS.10][SYS.29] |
| TIA clock (TCLK) | 2 ticks = 3,579,545 Hz | 3,546,894 Hz | crystal ÷ 4 [SYS.10] |
| Line | 454 ticks = 113.5 fast cycles = 227 colour clocks = 63.416 µs | 454 ticks = 64.000 µs | [SYS.01][SYS.09]; PAL 64 µs ± 5 ns measured [SYS.22] |
| Frame | 263 lines = 119,402 ticks = 29,850.5 cycles | 313 lines = 142,102 ticks = 35,525.5 cycles | [SYS.01][SYS.12]; NTSC count measured [SYS.18] |
| Frame rate | 39,375,000/656,711 ≈ 59.957881 Hz | 3,546,894/71,051 ≈ 49.920395 Hz | tick ÷ frame (derived); MiSTer PAL 3,546,895/71,051; 48 kHz = 88 samples per 13,125 ticks (NTSC) |

- φ2 (SALLY pin 39) clocks TIA, RIOT, the cartridge slot and MARIA pin 6 and keeps running through HALT [SYS.06];
  a cartridge POKEY slows audibly during slow cycles (measured by ear [SYS.29]).
- **Phase drift.** 454 ≡ 2 (mod 4): the CPU cycle grid moves half a cycle (2 ticks) against the line every line in
  both regions (PAL measured [SYS.22]); each slow cycle moves it 2 more. All cycle lengths are even, so the grid's
  parity against t (even/odd class) is fixed from MARIA enable (CPU clock restarted 5 crystal periods after MEN rises
  [SYS.10]) to power-off, and differs per power-up on hardware (a TIA-WSYNC demo alternates on ~50 % of power-ups [SYS.20]).

**C1 Tick unit.** Recommendation: one `uint64_t` counter of MARIA clocks; every φ1/φ2 edge, TIA clock, pixel and DMA
fetch is an integer; it wraps after ~81,700 years. Strongest alternative: crystal periods (half-ticks), as the NES
sibling counts half master clocks [SYS.31]. Would change if: an event needs half-tick ordering beyond the even/odd
class (MiSTer's 2.5-tick MEN restart, the ~70 ns /HALT delay [SYS.06]).
**C2 PAL rate.** Recommendation: 14,187,576 Hz (labels, MAME), only for reported rates; tick behaviour is the same.
Strongest alternative: MiSTer's 14,187,580 Hz. Would change if: a frequency-counter reading.

## 2. Tick-level line timeline (same ticks in NTSC and PAL)

| t | col | Event | Sources |
|---|---|---|---|
| 0 | 412 | RDY released (MARIA WSYNC); DMA force-stopped ("line reset count"); line-RAM buffers swap; last visible pixel | [SYS.09][SYS.10]; line start [SYS.13][SYS.16] |
| 1–27 | 413–439 | right border (a hardware photo puts it at MARIA cycle 413 [SYS.20]) | [SYS.09] |
| 28 | 440 | HBLANK starts; on DMA rows, with DMA on, MARIA requests HALT; /HALT falls at the first φ2 fall ≥ t 28: +0–3 ticks, +0–5 with a slow cycle in flight | [SYS.01][SYS.08]; ≈28 measured [SYS.19] |
| 42 | 0 | HSYNC (to col 33); row++; MSTAT bit 7 (VBLANK) sets at row 258 (PAL 308), clears at row 16 — mid-line, as measured | [SYS.09]; [SYS.19] |
| 43 | 1 | row 16 only: end-of-VBLANK DMA reads DPPH/DPPL and the first DLL entry (start-up + long shutdown) | [SYS.08][SYS.01] |
| 80–97 | 38–55 | colour burst | [SYS.09] |
| 110 | 68 | HBLANK ends; left border | [SYS.09] |
| 135–454 | 93–412 | 320 visible ticks = 160 colour clocks | [SYS.09] |
| ≤ 454 | ≤ 412 | DMA ends (window ≤ 426 ticks from t 28); an overrun is cut here and the CPU resumes just after the next t 0 | [SYS.08][SYS.13] |
| end + 1 | | DLI: NMI after the DMA of a zone's last line when the next DLL entry has DLI set | [SYS.01] |

Rows: VSYNC rows 0–2; VBLANK rows 258–262 and 0–15 (PAL 308–312 and 0–15), 21 rows (PAL count measured [SYS.22]);
DMA runs on rows 16–257 (PAL 16–307), 242 (292) lines, and each line's DMA result is shown on the following
software line [SYS.01][SYS.09][SYS.13]. MSTAT reads one CPU cycle stale; a CTRL write acts half a cycle later [SYS.10].

| Quantity | Software Guide [SYS.01] | MiSTer [SYS.06–10] | MAME [SYS.13] / a7800 [SYS.15] | A7800Hawk [SYS.16] | Hardware (T4) |
|---|---|---|---|---|---|
| DMA request | t 28 (7 cycles) | t 28 | t 28; a7800: "9 cycles and M.I.A. works" | t 28 | ≈28, just after φ2 falls (scope) [SYS.19] |
| CPU cycles run after /HALT falls | — | 2 (fitted, not measured) | 0: spins (n+3)/4 cycles | 0: next boundary | ≥1; bus free by 2nd φ2 fall (scope) [SYS.19] |
| DMA window end | one line | t 454 (lrc abort) | cap 426 / a7800 430 | t 453 | ~413 usable (opinion) [SYS.22] |
| End-of-VBLANK DMA | start-up + long shutdown | row 16, t 43 | row 16, t 28 | its line 20, t 28 | — |
| First visible pixel | t 134 (App. 4) | t 135 | a7800 t 134 | t 133 | — |
| VBLANK (MSTAT) rows | 20 (visible 16–258) | 21, edge at col 0 | 21, edge at t 0 | 20 + DLL line | PAL 21 [SYS.22]; edge mid-line [SYS.19] |
| Picture rows | 243 / 293 | 242 DMA rows | 242; a7800 screen 243 | 242 | 243 output lines [SYS.23] |
| DLI → NMI | 1 tick after shutdown | from 2nd φ1 after /HALT rises, 2 cycles wide | at DMA start | 1 tick after DMA end | WSYNC can delay it [SYS.20] |

**C3 Timeline.** Recommendation: MiSTer's schematic-derived columns and rows (picture rows are the MARIA file's call).
Strongest alternative: Software Guide App. 4 (20 VBLANK rows, pixel at t 134). Would change if: a raster diagnostic
on hardware puts MSTAT's edge or the first pixel elsewhere.

## 3. SALLY (6502C)

- NMOS 6502 with decimal mode and NMOS decimal flags (`BCD_EN=1`, "NMOS 6502 / Sally decimal mode" [SYS.07]), plus
  HALT (pin 35) from MARIA pin 40 with a 4.7 kΩ pull-up; NMI from MARIA pin 2; RDY (pin 2) = MARIA READY wire-ANDed
  with TIA RDY [SYS.06]. ANE's magic constant reads $00 on most consoles and drifts on one NCR part (measured [SYS.24]).
- **HALT** removes the core's phase enables, so reads and writes stop alike; address lines are released and R/W floats
  high [SYS.06][SYS.19]. /HALT changes only in φ1, ~70 ns after φ2 falls; SALLY samples it at each φ2 start and freezes
  from the second sample: the cycle beginning at the fall (c0) and the next (c1) run, c2 waits mid-cycle for m cycles
  (m = whole cycles /HALT stays low); release takes the same two samples, so the CPU loses exactly m cycles and c2 then
  completes. The two-cycle latency is fitted, not measured; a 46-cycle loop that must count 556 pins in = out
  [SYS.06][SYS.08]. /HALT was measured low 2.25 µs = 16.1 ticks [SYS.06] = the 16 ticks lost per empty line [SYS.18].
- **Alignment arithmetic** (derived): with g = grid position mod 4 against t, a fast cycle in flight gives a wait of
  (g − 28) mod 4 ∈ {0, 2} or {1, 3}, alternating line by line because a line is 113.5 cycles — the Software Guide's
  "every other scanline"; a slow cycle in flight allows up to 5 [SYS.01].
- **C4 Slow cycles** (MiSTer's divider [SYS.10], derived): a TIA/RIOT cycle's φ2 and the next cycle's φ1 last 3 ticks
  instead of 2 (MARIA's divider, not the TIA clock), so k slow cycles add exactly 2k ticks. Recommendation: a 6-tick
  bus call whose data and φ2 fall come at tick 5. Strongest alternative: A7800Hawk's access at tick 4, then 2 idle
  ticks [SYS.16]. Would change if: a RIOT-timer or MSTAT edge diagnostic separates the two.
- **RDY** holds read cycles only; a read-modify-write straddling it completes both writes [SYS.06][SYS.07]. Sources:
  a write to MARIA WSYNC ($24 and mirrors) drives RDY low until t 0 [SYS.10]; a write to TIA WSYNC ($02) holds until
  TIA's own HBLANK, and TIA's 228-colour-clock line (456 ticks) slides 2 ticks per MARIA line [SYS.20][SYS.11]
  (derived). The held cycle runs at the fast rate because its address is the next opcode's [SYS.20].
- **C5 RDY after a write.** Recommendation: hold the first read after the write (`STA WSYNC` stalls on the next opcode
  fetch), the documented NMOS rule. Strongest alternative: MiSTer's netlist note that the cycle right after a write
  cannot be held either, so the stall lands one cycle later [SYS.07]. Would change if: a hardware WSYNC-then-store
  diagnostic shows the store 4 ticks (2 pixels) earlier.
- **HALT gate.** HALT reaches SALLY only after two writes to INPTCTRL, data ignored [SYS.01]; MiSTer counts every
  unlocked write cycle in the TIA chip-select range, saturating at 2, so RMW dummy writes count [SYS.11].
- **NMI** (MARIA's DLI only, off while INPTCTRL bit 1 = 0 [SYS.10]) is an edge latched at the end of every φ2, also
  under RDY, and polled in φ2 of the last cycle (T2 for a branch): an edge present by the end of the penultimate cycle
  is taken after this instruction, a later one after the next; a taken branch without page cross polls early; NMI
  before T5 φ2 of BRK/IRQ takes its vector [SYS.07]. Under WSYNC the handler starts after t 0 (DLI "pushed" [SYS.20]).
- **C6 DLI timing.** Recommendation: MiSTer's — NMI low from the second φ1 after /HALT rises, two cycles wide [SYS.08].
  Strongest alternative: one tick after DMA shutdown [SYS.01][SYS.16]. Would change if: a diagnostic timing the
  handler's first write against the line fits the one-tick model.
- **IRQ:** no on-board source — the RIOT IRQ output is unconnected in MAME and MiSTer [SYS.12][SYS.11]; the cart edge
  has IRQ (pin 31, unverified [SYS.28]), which MiSTer drives only from a cart POKEY, as an option [SYS.11].
- **Reset:** RES only at power-on (derived); the Reset button is SWCHB bit 0, read by software [SYS.01]; NMOS RES
  leaves D undefined, and every BIOS starts with SEI, CLD [SYS.03][SYS.04][SYS.17].
- **Changes to the NES sibling's CPU** (nesturbator `src/cpu.c`, `e4c75a059d4d` [SYS.31]): NMOS decimal ADC/SBC (its
  ADC ignores D); NMI from a per-cycle edge sample with the penultimate, branch and hijack rules (it takes a flag at
  the next boundary); HALT freezes any cycle, so DMA cannot live only in the read call; 4- or 6-tick cycles by address
  instead of 24 fixed sub-ticks; RDY with two sources; ANE/LXA constant as a profile field; drop the DMC store note.

## 4. Memory map (INPTCTRL bit 1 = 1)

| Range (mirrors) | Device | Cycle | Reads return |
|---|---|---|---|
| $0000–$001F ($0100, $0200, $0300) | TIA; writes also hit INPTCTRL until locked | slow | TIA drives D7 (INPT0–5, CXBLPF) or D7–D6 (other collision regs); D5–D0 float [SYS.11] |
| $0020–$003F (same mirrors) | MARIA | fast | MSTAT ($28) drives D7 only; every other address is open bus [SYS.10]; hardware: palette reads $00 or noise [SYS.26][SYS.19] |
| $0040–$00FF, $0140–$01FF | RAM, = $2040–$20FF and $2140–$21FF | fast | RAM [SYS.01][SYS.10] |
| $0240–$027F, $0340–$037F | nothing on the board (Guide: "shadowed") | fast | cart or open bus [SYS.10] |
| $0280–$02FF ($0380–$03FF) | RIOT ports and timer (A9 = 1) | slow | RIOT, 8 bits |
| $0480–$04FF ($0580–$05FF) | RIOT RAM, 128 B (A9 = 0), "don't use" | slow | RAM [SYS.01][SYS.12] |
| $0400–$047F, $0500–$057F, $0600–$17FF | cartridge (POKEY $0450, XM $0400–$047F, POKEY $0800, HSC $1000–$17FF) | fast | cart or open bus |
| $1800–$27FF | RAM, two 6116; $2000–$203F and $2100–$213F reachable only here | fast | RAM |
| $2800–$2FFF | Guide: mirror of $2000–$27FF ("BIOS conflict"); MAME: page $27 only, some revisions, unreliable; MiSTer: undecoded; A7800Hawk: reads 0 or Summer Games deadlocks | fast | C8 |
| $3000–$FFFF | cartridge (HSC ROM, bankset at $3000); BIOS on A15 = 1 while INPTCTRL bit 2 = 0 | fast | signature $FF80–$FFF7, $FFF8 region, $FFF9 start page, vectors [SYS.01] |

- MEN = 0 (power-on, 2600 mode): the 2600 decode — A12 = 0 ∧ A7 = 0 TIA, A12 = 0 ∧ A7 = 1 RIOT, else cart; no system
  RAM; every cycle slow [SYS.10]. While MARIA drives the bus, TIA, RIOT and MARIA are not selected [SYS.10]; hardware
  returns "chaotic data" there [SYS.19].
- With the BIOS mapped the cart sees A12, A14 and A15 low (OpenBIOS author's note [SYS.17]); MiSTer mirrors the
  BIOS over $8000–$FFFF, MAME maps it at $C000–$FFFF [SYS.11][SYS.12].
- **C7 Open bus.** Recommendation: one data-bus byte updated by every CPU write, read and DMA fetch; undriven bits read
  it (MiSTer [SYS.11]). It explains the $00 palette read after `LDA P0C1,Y` (operand high byte $00) (derived) and
  Choplifter's sky [SYS.02]. Strongest alternative: zeros (MAME). Would change if: never — zeros break Choplifter.
- **C8 $2800–$2FFF.** Recommendation: undecoded (cart or open bus). Strongest alternative: read 0 (A7800Hawk). Would
  change if: Summer Games deadlocks on open bus, or the owner's console mirrors page $27 reliably.

## 5. INPTCTRL ($01, write-only)

| Bit | 1 | 0 |
|---|---|---|
| 0 LOCK | ignore every later write until power-off | writable |
| 1 MEN | MARIA, system RAM, 7800 decode, NMI, fast cycles | 2600 decode, every cycle slow, no NMI |
| 2 EXT | cartridge at $8000–$FFFF | BIOS at $8000–$FFFF |
| 3 TIA-EN | TIA video, two-button mode off | MARIA video |
| 4–7 | ignored ($16 acts as $06) | |

- A latch on the TIA chip-select [SYS.01][SYS.11][SYS.12]: until LOCK every TIA-range write also writes INPTCTRL and
  the reverse [SYS.20]; reads return the TIA. Power-on $00: BIOS mapped, MARIA off, unlocked, HALT gate closed
  [SYS.12][SYS.11][SYS.16]. The BIOS leaves 7800 carts unlocked ($16) [SYS.21]; its 2600 loader ends by writing $FD to
  a TIA register (LOCK + EXT + TIA-EN) [SYS.17]. So a 7800 game writes bits 0–2 set ($07) before its first TIA write.

## 6. RIOT 6532

- Select CS1 = A7, RS = A9 (1 = ports/timer, 0 = RAM), A0–A6 inside [SYS.11][SYS.30]. $280 SWCHA (reads the pins),
  $281 CTLSWA, $282 SWCHB (output bits read back ORB), $283 CTLSWB, $284 INTIM, $285 flags (D7 timer, D6 PA7, D5–D0 = 0;
  read clears PA7); writes $294–$297 = TIM1T/8T/64T/1024T (+$08 enables IRQ, e.g. TIM64TI $29E); writes $284–$287 =
  PA7 edge control (A0 polarity, A1 IRQ) [SYS.01][SYS.30].
- Timer: after writing N it reads N − 1 at once and sets the flag at φ2 number N × interval + 1, then counts every φ2
  from $FF and keeps going past later underflows (Beef Drop [SYS.19]); a timer read or write clears the flag, except a
  read in the cycle it sets [SYS.30][SYS.05]. Clock = φ2: it counts through HALT and WSYNC (113 counts per WSYNC line,
  DMA on or off, measured [SYS.26]), a count lasts 6 ticks on slow cycles [SYS.27]; games write it twice [SYS.18].
- Ports: SWCHA bits 7–4 = P0 right/left/down/up, 3–0 = P1, low = pressed; SWCHB bit 0 Reset ("Game Start"), 1 Select,
  3 Pause, 6/7 difficulty; PB2/PB4 driven low (CTLSWB $14, SWCHB $00) enable two-button pads [SYS.01][SYS.19].
- 7800-mode RAM is decoded and slow; it fails when DMA halts the CPU mid-access and when code runs from it [SYS.05].
  Recommendation: plain slow RAM. Strongest alternative: corrupt a halted access. Would change if: a title needs it.
- **C9 Power-on.** The datasheet zeroes DDRA, ORA, DDRB, ORB, disables both interrupts and selects a falling PA7 edge;
  the timer is not reset [SYS.30]. Hardware reports instead show CTLSWB = $14 without a SWCHB write leaving PB2/PB4
  high, ORB acting as $FF [SYS.25][SYS.19], which One on One and Worm! need [SYS.02]; MAME at the pin resets ORB to 0
  [SYS.14]. Recommendation: DDRA = DDRB = $00, ORA = $00, ORB = $FF, timer $FF at 1024T, flags clear. Strongest
  alternative: datasheet zeros. Would change if: the owner's console reads SWCHB = $00 after power-on with CTLSWB = $FF.

## 7. Boot

**NTSC BIOS** (4 KiB at $F000, mirrored over $8000–$FFFF while mapped; wiki disassembly, facts only [SYS.03]): from
the power-on state, SEI, CLD, INPTCTRL $02, CTRL $7F (DMA off), BACKGRND 0; RAM test over 4 KiB (last pattern $00),
page-0/1 mirror check, CPU self-test; clears $01–$2B (rewriting INPTCTRL, so it writes $02 again); copies its check
code to $2300–$27FF and Fuji data (DLL $1F84) to RAM and runs from RAM so it can unmap itself; with INPTCTRL $16 it
checks a floating bus ($FD80–$FDFF against $FE00–$FE7F), the reset vector (not $0000/$FFFF), $FFF8 and $FFF9 (low
nibble 3 or 7; high nibble = first signed page, ≥ $40 and below the vector's page; bit 2 shows the logo, DMA on) and
the signature in $FF80–$FFF7, whose length makes the hand-off moment vary per cart (derived); then CTRL $60, INPTCTRL
$16, S = X = $16, D set, jump through $FFFC [SYS.03][SYS.21]. On failure: a 128-byte loader into RIOT RAM $0480 (2600).
**PAL BIOS** (16 KiB [SYS.04]): built-in Asteroids when no cart; no signature check — header bytes plus a 2600-mirror
test of the vectors at $FFFA against $DFFA; copies the 2600 loader to RIOT RAM before the check (so it stays there
under 7800 carts too); writes $FF to $2000–$203F and $2100–$213F; DLL at $2700, DPP $2707; shows two frames, then
CTRL $60 and the same hand-off just after a VBLANK start, with A = $60.
**7800OpenBIOS** (CC0, `b8bc745a8744` [SYS.17]): 16 KiB; zeroes $2000–$27FF, then $FF into $2000–$203F/$2100–$213F;
copies its 2600 loader to $0480 before the check; PAL-style header checks plus vector-mirror checks at $DFFA, $BFFA,
$9FFA, $7FFA, $5FFA (Galaga keeps a vector copy at $DFFx); 7800 carts get a ~5 s Fuji display (B/W switch skips),
then $FF into $40–$48 ("similar to the NTSC BIOS"), CTRL $60, a wait for VBLANK start, INPTCTRL $06, S = X = $FF,
D = 0, A = $60, jump through $FFFC; 2600 carts get INPTCTRL $02 and the loader. KiloParsec (all rights reserved) sits
at $C000–$DFFF and $FC00; muhtari's build substitutes fill bytes and a no-cart stub.
**Provenance flag:** OpenBIOS's 128-byte 2600 loader matches the Atari loader instruction for instruction (derived by
comparing [SYS.17] with [SYS.03]); REUSE-AND-PROVENANCE must rule on it before any copy enters muhtari.

**C10 HLE hand-off** (no BIOS; all values are facts, no Atari bytes). Recommendation:

| State | NTSC | PAL |
|---|---|---|
| CPU | PC = ($FFFC); S = X = $16; A = Y = $00 (hardware: residue); P: I = 1, D = 1, rest 0 | same, but A = $60 |
| INPTCTRL; HALT gate | $16 (acts as $06, unlocked); two writes counted | same |
| MARIA registers | CTRL $60; DPP $1F84; palettes, BACKGRND, CHARBASE 0 (MiSTer bypass, measured off the real BIOS) [SYS.10] | CTRL $60; DPP $2707 (BIOS listing; MiSTer keeps an unmeasured $2730) |
| MARIA DMA internals | DL $2200, DL pointer $2207, zone pointer $1FF3, OFFSET 2, pixel pointer $8400 [SYS.08] | none measured |
| Beam at the first cart opcode fetch | row 239, col 436: MiSTer seeds (239, 386) 100 crystal periods earlier, measured with Choplifter [SYS.09]; 8,190 ticks before VBLANK (derived) | ≈ row 308, col 125–160: VBLANK start + 31–39 cycles of BIOS tail (derived from [SYS.04]) |
| TIA, RIOT | TIA registers 0 (the BIOS clears them); RIOT power-on state (C9), timer running | same |
| RAM, RIOT RAM | `ram_fill`; the NTSC BIOS never writes RIOT RAM on the 7800 path | `ram_fill`, $FF at $2000–$203F/$2100–$213F; RIOT RAM `ram_fill` (real BIOS: loader) |
| Cartridge | board power-on state, bank 0 | same |

Strongest alternative: hand off just after VBLANK start (OpenBIOS, PAL BIOS), or reset into the cart at row 0 with INPTCTRL
$04 (a7800 without BIOS [SYS.15]). Would change if: Klax, Pole Position II or One on One fail only with the snapshot.

| Title | Depends on | Source |
|---|---|---|
| Pole Position II | first frame starting in VBLANK (or exact DMA), and SWCHA unchanged between two reads; latch input at VBLANK start | [SYS.02] |
| Klax (prototype) | start position in the frame: a startup race, hit-or-miss on some consoles too | [SYS.02] |
| Legend of Silverpeak | cartridge in bank 0 at start | [SYS.02] |
| One on One, Worm! | RIOT ORB power-on value (C9) | [SYS.02][SYS.25] |
| Ninja Golf, One on One | "BIOS residue" (TIA cleared, RAM) | [SYS.19] (T4) |
| Summer Games | $2800 not mirroring RAM (C8) | [SYS.16] |
| Tomcat F-14 | survives CTRL $7F, $00, $40 at boot (DMA test mode briefly) | [SYS.19] |
| Decathlon (2600 mode, later) | FE banking sees the loader's stack accesses and the starting S | [SYS.29] (unverified) |

## 8. Determinism: power-on profile fields

| Field | Hardware | Default | Sources |
|---|---|---|---|
| `ram_fill` (4 KiB system RAM, 128 B RIOT RAM) | SRAM power-up pattern, unmeasured (unverified) | ZERO; SEEDED(seed) from a fixed integer generator | MiSTer Zero/Random option [SYS.11]; zero in MAME, A7800Hawk; NTSC BIOS leaves $00 [SYS.03]; zero/ones/hardware-like wish [SYS.19] |
| `cpu_phase` (grid offset 0–3 against t) | differs per power-up | MiSTer's hand-off phase, to extract by simulation | [SYS.10][SYS.20] |
| `tia_phase` (TIA horizontal counter, 0–455 ticks) | differs per power-up | 0 | [SYS.20] |
| RIOT timer at power-on | undefined | $FF at 1024T | MAME [SYS.14]; A7800Hawk $73 [SYS.16] |
| `ane_lxa_constant` | $00 on most units | $00 (vector-test lane $EE) | [SYS.24][SYS.31] |
| CPU registers before a BIOS | undefined | A = X = Y = 0, S = $FD after RES, P = I (derived) | the BIOS overwrites them [SYS.03] |
| MARIA CTRL at power-on | DMA off | $00 (MiSTer sets $FF) | [SYS.13][SYS.10] |

Every field lives in the instance, the save state and the movie header; BIOS bytes are an input, never a default.

## 9. Open questions

(1) HALT latency — 2 cycles each way in MiSTer (fitted), ≥ 1 on the scope — plus C5 and C6: one loop-count and
WSYNC/DLI diagnostic on the owner's console settles all three. (2) RIOT ORB at power-on (C9) and the $2800 mirror (C8).
(3) PAL hand-off beam and DPP (MiSTer's PAL values are unmeasured). (4) The "general_tests" suite that `sally.sv`
cites was not found. (5) OpenBIOS loader provenance (section 7).

## Sources

| ID | Tier | Source (URL) | Pin or access date | Supports |
|---|---|---|---|---|
| SYS.01 | T2 | 7800 Software Guide, https://7800.8bitdev.org/index.php/7800_Software_Guide | rev 1320 | clocks, rows, map, INPTCTRL, RIOT, App. 3/4 |
| SYS.02 | T2 | Common Emulator Development Issues, https://7800.8bitdev.org/index.php/Common_Emulator_Development_Issues | rev 1304 | game dependencies |
| SYS.03 | T2 | 7800 NTSC BIOS disassembly, https://7800.8bitdev.org/index.php/7800_NTSC_BIOS | rev 1259 | NTSC boot flow (facts only) |
| SYS.04 | T2 | 7800 PAL BIOS disassembly, https://7800.8bitdev.org/index.php/7800_PAL_BIOS | rev 1285 | PAL boot flow (facts only) |
| SYS.05 | T2 | RIOT limitations and workarounds, https://7800.8bitdev.org/index.php/RIOT_limitations_and_workarounds | rev 1308 | RIOT RAM/timer in 7800 mode |
| SYS.06 | T3 | MiSTer `rtl/6502/sally.sv`, https://github.com/MiSTer-devel/Atari7800_MiSTer | `0dc8ad2e3ff7` | HALT circuit, latency, capture, pins |
| SYS.07 | T3 | MiSTer `rtl/6502/mos6502.sv`, `mos6502_ctl.sv` (same repo) | `0dc8ad2e3ff7` | decimal, RDY, NMI polling |
| SYS.08 | T3 | MiSTer `rtl/Maria/DMA.sv` | `0dc8ad2e3ff7` | HALT request, DLI/NMI, hand-off DMA state |
| SYS.09 | T3 | MiSTer `rtl/Maria/video_sync.sv` | `0dc8ad2e3ff7` | columns, rows, hand-off beam |
| SYS.10 | T3 | MiSTer `rtl/Maria/maria.sv`, `control.sv`, `line_ram.sv` | `0dc8ad2e3ff7` | decode, slow clock, WSYNC, MSTAT, CTRL, NMI gate, hand-off registers |
| SYS.11 | T3 | MiSTer `rtl/top.sv`, `rtl/TIA.sv`, `rtl/cart.sv`, `Atari7800.sv` (facts only) | `0dc8ad2e3ff7` | INPTCTRL latch, HALT gate, open bus, TIA drivers, IRQ wiring, RAM clear, PLL |
| SYS.12 | T3 | MAME `src/mame/atari/a7800.cpp`, https://github.com/mamedev/mame (BSD-3) | `dddf4cd1faee` | clocks, map, INPTCTRL, RIOT IRQ unwired |
| SYS.13 | T3 | MAME `src/mame/atari/maria.cpp` | `dddf4cd1faee` | VBLANK rows, 426 cap, DLI |
| SYS.14 | T3 | MAME `src/devices/machine/mos6530.cpp` | `dddf4cd1faee` | RIOT reset and timer |
| SYS.15 | T3 | a7800 `src/mame/drivers/a7800.cpp`, `src/devices/machine/6532riot.cpp`, https://github.com/7800-devtools/a7800 (BSD-3) | `7a2afdc1ea08` (2024-11-17) | no-BIOS start, $2800 note, DMA start |
| SYS.16 | T3 | BizHawk A7800Hawk `Maria.cs`, `MemoryMap.cs`, `A7800Hawk.IEmulator.cs`, `M6532.cs`, https://github.com/TASEmulators/BizHawk (MIT) | `c9de9a8bff0f` | t 28/133, DLI, $2800 → 0, RIOT defaults |
| SYS.17 | T1 | 7800OpenBIOS `7800openbios.asm`, https://github.com/7800-devtools/7800OpenBIOS (CC0) | `b8bc745a8744` | OpenBIOS behaviour, 2600 loader, BIOS banking note |
| SYS.18 | T4, measured | AtariAge 224025 "7800 hardware facts", https://forums.atariage.com/topic/224025-7800-hardware-facts/ | accessed 2026-10-15 | 113.5 cycles/line, 29,850.5/frame, 16/24-tick overhead, crystals |
| SYS.19 | T4, measured | AtariAge 285921 "BupSystem", https://forums.atariage.com/topic/285921-bupsystem/ | accessed 2026-10-15 | HALT at 28 (scope), mid-line MSTAT, SWCHB $FF, register reads, residue |
| SYS.20 | T4, measured | AtariAge 318358 and 339785 (TIA/MARIA WSYNC), https://forums.atariage.com/topic/318358-hitting-tia-wsync-in-7800-mode-a-pointless-curiosity/ | accessed 2026-10-15 | two WSYNCs, INPTCTRL/TIA overlap, phase per power-up, slow decode |
| SYS.21 | T4 | AtariAge 222380 "7800 Software Guide updated", https://forums.atariage.com/topic/222380-7800-software-guide-updated/ | accessed 2026-10-15 | hand-off S = X = $16, D set, unlocked |
| SYS.22 | T4, measured | AtariAge 164554 DMA cycles per scanline, https://forums.atariage.com/topic/164554-number-of-dma-clock-cycles-per-scanline/ | accessed 2026-10-15 | PAL 64 µs, 21 VBLANK rows, half-cycle drift |
| SYS.23 | T4 | AtariAge 383566 test7800 thread, https://forums.atariage.com/topic/383566-emulator-test7800-v081/ | accessed 2026-10-15 | 243 output lines |
| SYS.24 | T4, measured | AtariAge 385521 fingerprinting 6502s, https://forums.atariage.com/topic/385521-fingerprinting-6502-processors/ | accessed 2026-10-15 | ANE constant per unit |
| SYS.25 | T4 | MAME tester 06060 (RevEng quoted), https://mametesters.org/view.php?id=6060 | accessed 2026-10-15 | RIOT outputs default high |
| SYS.26 | T4, measured | AtariAge 145363 "Improving the emulators", https://forums.atariage.com/topic/145363-improving-the-emulators/ | accessed 2026-10-15 | 113 timer counts/line, palette read $00 |
| SYS.27 | T4 | AtariAge 268458 A7800 thread, https://forums.atariage.com/topic/268458-a7800-the-atari-7800-emulator/ | accessed 2026-10-15 | timer stretch on slow cycles |
| SYS.28 | T4 | consolemods 7800 connector pinouts, https://consolemods.org/wiki/Atari_7800:Connector_Pinouts | search snippet only (403) | cart IRQ pin (unverified) |
| SYS.29 | T4 | AtariAge 86087 (slow cycles, POKEY pitch) https://forums.atariage.com/topic/86087-78006502-hardware-questions/ and 293982 (FE banking, snippet) | accessed 2026-10-15 | slow on every TIA/RIOT access; Decathlon (unverified) |
| SYS.30 | T1 | MOS 6532 datasheet, http://archive.6502.org/datasheets/mos_6532_riot.pdf | accessed 2026-10-15 | RES, timer, ports, flags |
| SYS.31 | T3 | nesturbator `src/cpu.c`, `.planning/preparation/ARCHITECTURE.md`, github.com/szTheory/nesturbator (MIT, sibling) | `e4c75a059d4d` | seed CPU, time-model precedent |
