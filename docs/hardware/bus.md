# The 7800's bus

SALLY reaches everything through one bus: RAM, the TIA, the RIOT, MARIA's registers and the cartridge. The 7800
stretches some of its cycles, and MARIA can stop or hold the CPU. `src/bus.c` owns all of this, and it is the only
place where emulated time advances. This page covers the bus's behaviour and the run loop on top of it. The CPU
itself is in `docs/hardware/sally.md`, MARIA's display lists and DMA costs in `docs/hardware/maria.md`.

Sources:

- **The Software Guide**, the 7800 Software Guide on 7800.8bitdev.org, rev 1320: clocks, the memory map, INPTCTRL and
  the HALT gate.
- **The system notes**, `.planning/preparation/A7800-SYSTEM.md` sections 1 to 5 and 7, which gather the facts below
  with their own sources: MiSTer's Atari 7800 core at `0dc8ad2e3ff7` (facts only), MAME, and measurements posted on
  AtariAge.
- **The MARIA notes**, `.planning/preparation/A7800-MARIA.md`, section 4.1: the line events.
- **The Stella Programmer's Guide** (1979), section 3.2: a write to WSYNC holds the CPU until the TIA's horizontal
  blank begins.

## Clocks

One tick is one MARIA clock, the crystal divided by 2: 315/44 MHz on NTSC or 7,093,788 Hz
on PAL. `ticks` counts them from power-on in
one 64-bit counter.

A CPU cycle takes 4 ticks (1.79 MHz) or 6 ticks (1.19 MHz):

| Cycle | Ticks | Data on the bus | When |
|-------|-------|-----------------|------|
| Fast | 4 | at tick 4, the φ2 fall | RAM, MARIA's registers, the cartridge and every unused address |
| Slow | 6 | at tick 5 | the TIA and the RIOT, and every address while MEN = 0 |

A slow cycle stretches φ2 and the next φ1 by one tick each, so each slow cycle adds exactly 2 ticks (System notes,
section 3). `phi2` counts completed cycles, and the first cycle after power-on completes as φ2 1.

A scan line is 454 ticks, 113.5 fast cycles. The TIA runs on its own 456-tick line of 228 colour clocks.

## Memory map

With MEN = 1 (INPTCTRL bit 1), the 7800 decode applies:

| Range (mirrors) | Device | Cycle | Reads return |
|-----------------|--------|-------|--------------|
| `$0000-$001F` (`$0100`, `$0200`, `$0300`) | TIA | slow | the TIA's drivers, see Open bus |
| `$0020-$003F` (same mirrors) | MARIA | fast | MSTAT at `$28`, see Open bus; the open-bus byte elsewhere |
| `$0040-$00FF`, `$0140-$01FF` | RAM, the same bytes as `$2040-$20FF` and `$2140-$21FF` | fast | RAM |
| `$0240-$027F`, `$0340-$037F` | nothing | fast | the open-bus byte |
| `$0280-$02FF` (`$0380`) | RIOT ports and timer | slow | the RIOT, 8 bits (`docs/hardware/riot.md`) |
| `$0480-$04FF` (`$0580`) | RIOT RAM | slow | the RIOT's RAM |
| `$0400-$047F`, `$0500-$057F`, `$0600-$17FF` | cartridge | fast | the board, or the open-bus byte |
| `$1800-$27FF` | RAM, 4 KiB | fast | RAM |
| `$2800-$FFFF` | cartridge, and the BIOS while EXT = 0 | fast | the board, or the open-bus byte |

`$2800-$2FFF` is left undecoded, so it reads as cartridge space. The board answers every cartridge address through
`src/cart.c`, which picks the board's mapping with one `switch`. A linear board maps its ROM so that it ends at
`$FFFF`; below the ROM's start, and with nothing loaded, the bus floats. MARIA's DMA reads cartridge space through
the same board read.

While EXT = 0 and a BIOS is loaded, the BIOS ROM answers in place of the cartridge: with MEN = 1 at every A15 = 1
address, `$8000-$FFFF`, the image mirrored there (a 16 KiB image twice, a 4 KiB one eight times, as MiSTer maps
it), and with MEN = 0 at every A12 = 1 address, where the 2600 decode sees the image's top 4 KiB. Below `$8000` the
cartridge still answers. The cartridge sees A12, A14 and A15 low while the BIOS is mapped (System notes, section 4;
the OpenBIOS source's notes on INPTCTRL); the core does not pass those addresses to the board. MARIA's DMA reads the
BIOS through the same rule, so a BIOS's own display lists and graphics work. Writes there change nothing.

With MEN = 0, at power-on and in 2600 mode, the 2600 decode applies: with A12 = 0, A7 = 0 selects the TIA and A7 = 1
the RIOT. A12 = 1 is the BIOS while EXT = 0; with EXT = 1, or with no BIOS, nothing answers there, as the core runs
no 2600 cartridge. There is no system RAM, and every cycle is slow.

At power-on every RAM byte is 0. Other fill patterns are a later profile option.

## Slow cycles

Controller events share the existing cached deadline owner with MARIA.
Before an access data tick, the bus merges native-light sample deadlines,
delayed sensor low/release edges and motion edges chronologically with
line events. A sample flushes MARIA only through its historical emission;
line-origin rendering preserves the previous raster's x319 before swapping
line RAM. Scheduled light continues across blanking and frame returns.
Held RDY/HALT cycles and JAM retain these deadlines. A slow cycle also
delivers events due in its final tick, so instruction overshoot cannot lose
a sensor pulse. Each electrical low reaches TIA's D6 latch without a CPU
read. Observer peeks never flush, clock or commit controller state, and
automatic events never count as input reads for lag accounting.

Every cycle that selects the TIA or the RIOT is slow: `$0000-$001F`, `$0280-$02FF` and `$0480-$04FF` with their
mirrors. While MEN = 0, every cycle is slow, wherever it points. A slow cycle's data is on the bus at tick 5, so a
write to the TIA lands there. `$0240-$027F` and `$0340-$037F` sit in the RIOT's pages but select nothing, so they
take 4 ticks.

## Open bus

The data bus keeps the last byte any cycle put on it, read or written. An address nothing drives returns that byte.
So `LDA $0240` returns `$02`: the last byte on the bus was the operand's high byte. A model that returns zeros there
breaks games that read an empty address, such as Choplifter's sky (System notes, section 4).

The TIA drives only some bits of a read. The rest come from the open-bus byte:

| Register (A3-A0) | Driven | Idle value |
|------------------|--------|------------|
| `$00-$05`, `$07`: collision latches | D7-D6 | 0 |
| `$06`: CXBLPF | D7 | 0 |
| `$08-$0B`: INPT0-INPT3, paddles | D7 | 0 |
| `$0C-$0D`: INPT4-INPT5, buttons | D7 | 1, button up |
| `$0E-$0F` | nothing | |

The idle values hold until the inputs are wired. MARIA's MSTAT at `$28` drives D7 alone, VBLANK; its other bits,
and every other MARIA address, which are write-only, read the open-bus byte. VBLANK is set from line 258, t 42 to
line 16, t 42, so it is set at power-on on line 0. A write to OFFSET, `$38`, is ignored.

## INPTCTRL

INPTCTRL is a write-only latch on the TIA's chip select. Until it is locked, every write the TIA sees also writes
INPTCTRL, at any TIA address or mirror (Software Guide; System notes, section 5). Its four bits:

| Bit | Name | Set | Clear |
|-----|------|-----|-------|
| 0 | LOCK | ignore every later write until power-off | writable |
| 1 | MEN | MARIA, system RAM, the 7800 decode, NMI, fast cycles | the 2600 decode, every cycle slow, no NMI |
| 2 | EXT | the cartridge at `$8000-$FFFF` | the BIOS there |
| 3 | TIA-EN | TIA video | MARIA video |

Bits 4-7 are not stored, so `$16` acts as `$06`. INPTCTRL is `$00` at power-on: unlocked, MEN clear. A 7800 game
runs with `$07` or more set.

A BIOS's 2600 loader ends by writing `$FD` to a TIA register, which leaves LOCK, EXT and TIA-EN set and MEN clear
(System notes, section 5). So a write that locks INPTCTRL with MEN = 0, under a BIOS and before the hand-off, means
the BIOS has refused the cartridge as a 7800 one. The core sets a sticky rejection there, and `muhtari_run_frame`
stops that frame and returns `MUHTARI_STATUS_ERR_BIOS_REJECTED_CART` (`docs/embedding.md`, Booting with a BIOS).

## RDY

RDY stops SALLY in a read cycle. It never stops a write, so a read-modify-write that sets it completes both of its
writes, and the hold falls on the next read. A held read repeats whole 4-tick φ2 cycles, each counted in `phi2`, until
a cycle starts at or after the release. It then runs at its own length: 4 ticks for the next opcode's fetch from RAM.

On the 7800, RDY is MARIA's WSYNC wire-ANDed with the TIA's, so RDY is low while either holds it and the later
release wins. MARIA's is a write to `$24`: RDY stays low until t 0 of the next line. The TIA's is a write to
`$02` or any mirror: RDY goes low and stays low until the TIA's line wraps, at its horizontal blank (Stella
Programmer's Guide). The TIA's line is 456 ticks, 2 more than MARIA's, so the release slides 2 ticks against MARIA's
line every line. `tia_phase` is the TIA's position in its line at tick 0, from 0 to 455. The release is the first
tick after the write's data tick at which tick + `tia_phase` is a multiple of 456.

The release tick and the rule that the held read runs in the first cycle starting at or after it are fitted, not
measured. They follow from the sources above, and a WSYNC diagnostic on hardware would confirm or move them by one
cycle. A refit changes the behaviour revision.

So `STA WSYNC` from tick 0, with `tia_phase` 0, writes at tick 13 and ends at 14. The next opcode fetch is held
from 14 for 111 cycles, runs from 458 to 462, and `phi2` counts every held cycle.

A hold never skips MARIA. It stops at every held cycle whose data tick reaches a line event, and at each step of a
HALT window, so a CPU parked in WSYNC across t 28 still sees that line's burst run at its own time, and one parked
across t 42 still sees MSTAT move. A HALT window that falls during the hold runs c0, c1 and c2 as held cycles, and
SALLY runs on only once RDY and HALT have both released.

## HALT

MARIA stops SALLY with HALT while it fetches a line's graphics. HALT stops read and write cycles alike: the CPU's
phase enables go away, and φ2 keeps running (System notes, section 3).

**The gate.** HALT reaches SALLY only after two writes in the TIA's range (Software Guide). The count includes every
unlocked write cycle the TIA's chip select sees, at any mirror and with any data, and it stops at 2. A
read-modify-write's dummy write counts too. So `INC $06` from INPTCTRL `$02` opens the gate on its own: it reads
`$06`, since the operand left `$06` on the bus and CXBLPF drives only D7, then writes `$06` back and then writes
`$07`. INPTCTRL ends at `$07`, and the count at 2. A write after LOCK does not count. Until the gate opens, a HALT
request does nothing.

**The window.** MARIA asks for HALT from one tick until another. /HALT falls at the first cycle boundary at or after
the request: at once when the request lands on a boundary, or at the end of the cycle in flight. That end is up to
3 ticks later after a fast cycle and up to 5 after a slow one. SALLY samples HALT at the start of each φ2 and stops
only from the second sample:

- c0, the cycle that starts at the fall, runs, and so does c1, the next.
- c2 waits for m whole 4-tick cycles, where m counts the cycles from the fall to the end of the request, rounded up.
  Then it completes. It takes 4 + 4m ticks when fast, and every frozen cycle counts in `phi2`.

Release takes the same two samples, so SALLY loses exactly m cycles.

One window at a time: a request made while another is waiting for /HALT to fall, or while /HALT is low, is
ignored, and the window in flight keeps its hold. The request returns 1 when it is taken and 0 when it is ignored,
for one of two reasons: the gate is shut, or a window is already in flight. MARIA reads a 0 as no burst this line:
the line's statistics carry `dma_blocked` (`MUHTARI__LINE_DMA_BLOCKED`) and cost nothing, and the sticky feature
mask records it. The gate opens once and never shuts again, so a line refused for the gate is refused at its t 28
request, before any burst; a window still in flight at the next t 28 means the CPU has not reached that line's c2.

**MARIA's window.** MARIA asks at t 28 of each DMA line with no end tick. When the window reaches c2, the bus runs
the line's burst there: the PLA starts at T0 = fall + 6, and the burst's HALTRST is the request's end, so SALLY
loses ceil((HALTRST - fall) / 4) cycles. A store SALLY makes in c1 is in memory before the first fetch at T0 + 2 =
fall + 8; a store in c2 comes after the burst. The 6 is fitted to the empty-DL 16 and 24 ticks measured on hardware
(`docs/hardware/maria.md`, The HALT handshake).

The two cycles of latency are fitted, not measured. They are MiSTer's figure, chosen so that a 46-cycle loop counts
correctly. A scope shows the bus free by the second φ2 fall, and other emulators stop the CPU at once (System notes,
sections 2 and 3). `src/bus.c` names the constant and keeps the rule in one place. A refit changes the behaviour
revision.

## Line events

MARIA counts the beam from the tick count, never from DMA. `origin` is the tick of line 0, t 0 of the current frame;
the line is (ticks - origin) / 454 and t is the remainder, and the origin moves on by 119,402 ticks when line 263's
t 0, the next frame's line 0, is reached. Each line has up to four events (MARIA notes, section 4.1):

PAL uses the same dispatcher, with 313 lines/142,102 ticks and terminal line 308.
Its MSTAT clear/set ticks are 7306/139874, VBE is 7307, final DMA request 139860,
and line 309 has no request. Event data ordering and 454-tick line lengths are common.

| t | Lines | Event |
|---|-------|-------|
| 0 | every line | LRC: line RAM swaps and the new write buffer clears; MARIA's WSYNC releases; a burst's abort deadline |
| 28 | 17-258 | the DMA request, with CTRL's DMA mode 10; any other mode marks the line DMA off |
| 42 | 16, 258 | MSTAT's VBLANK clears on line 16 and sets on line 258 |
| 43 | 16 | VBE, with DMA on: the DLL pointer from DPPH and DPPL and zone 0's entry |

The bus fires every event whose tick is at or before a cycle's data tick, in tick order, before that cycle's HALT,
RDY and NMI logic. A read whose data tick is t 42 of line 258 sees VBLANK set. VBE reads its 7 ticks of DLL bytes
at the event and takes no CPU cycle; that is a model, not a measurement.

## The run loop

RIOT ORA and DDRA writes pass their actual data tick to the serial EEPROM model in the port owner: start + 5 in a
slow 6-tick cycle, after any bus hold. The device resolves host pin edges at that tick; reads and peeks create no
edges (`docs/hardware/controllers.md`, Serial EEPROM).

With content loaded, `muhtari_run_frame` runs SALLY from one VBLANK start, line 258, t 42, to the next: it steps the
CPU until the tick reaches the frame's deadline. Every step makes at least one bus cycle of 4 ticks or more, a jammed
CPU's read of `$FFFF` included, and every hold ends within about one line, so each frame ends for any program, jammed,
with DMA off or hostile. The loop stops at the first instruction boundary at or after the deadline. Deadlines are
absolute ticks, 119,402 apart, so the overshoot carries into the next frame and frames never drift. The audio count
of a frame is (audio_rem + elapsed x 88) / 13,125, where elapsed is the distance between the two deadlines: 800 or
801 for a full frame.

PAL VBLANK starts on line 308,t42; deadlines are 142,102 ticks apart. Sample count is
`(audio_rem + elapsed*4000)/591149`, with the remainder carried. Full frames
return 961 or962 samples. Both regions keep the same 20,730-tick instruction-tail
bound, including WSYNC/HALT and interrupt entry; see [audio](audio.md).

After execution the EEPROM catches up to the actual bus tick, including the last completed instruction's overshoot,
before returning battery memory to the host. This also runs when partial BIOS execution rejects a cartridge; an
argument/buffer refusal that executes nothing leaves it unchanged. Consequently a due page commits even if software
never polls, or the CPU jams or encounters DMA/WSYNC holds. The host's flush and the battery getter remain observers;
they cannot finish a future page by advancing time. See `docs/hardware/controllers.md`, Serial EEPROM, for reset
and the selected fixed programming interval.

The fast-boot hand-off puts the machine where the NTSC BIOS leaves it as it jumps to the cartridge (System notes,
section 7, C10): PC from `$FFFC`, S = X = `$16`, A = Y = 0, I and D set; INPTCTRL `$16` with the gate's two writes
counted; CTRL `$60` (DMA off), DPP `$1F84`, palettes, BACKGRND and CHARBASE 0; the DL at `$2200`, the next DLL entry
at `$1FF3` and OFFSET 2; RAM and the RIOT at power-on. The beam is at line 240, t 24, the first cartridge fetch's
measured row 239, column 436, which is 8,190 ticks before VBLANK start. t 24 is a multiple of 4, so the CPU's cycle
grid has offset 0 against the beam: derived from C10, fitted, not measured. Frame 1 after a load is those 8,190
ticks, 54 samples. MARIA writes each row as the beam plays it, and the rows of frame 1 the beam has not scanned
since the load are written `$00` (`docs/hardware/maria.md`, The first frame after a load).

With a BIOS, the machine starts from power-on instead (System notes, section 8): INPTCTRL `$00`, MARIA's registers
0, so CTRL is `$00` and DMA is off, RAM and the RIOT at power-on, and the beam at line 0, t 0. SALLY runs RES
(`docs/hardware/sally.md`, Reset), which reads the BIOS's reset vector through the 2600 decode. Frame 1 is the
117,174 ticks from power-on to the first VBLANK start, 785 samples. Line 0 lies inside vertical blank, which ends
at line 16, t 42, so frame 1 does not start at a VBLANK start; input set before it applies from power-on, as what
is held when the power comes on.

### PAL startup

PAL fast boot is an explicit fitted compatibility profile, not a measured BIOS
snapshot. PC comes from reset, S=X=$16, A=$60, Y=0, P=$2C; INPTCTRL=$16 counts
two gate writes, CTRL=$60 and DPP=$2707. RAM $2000–$203F/$2100–$213F is FF; RIOT
keeps its power-on fill. Undocumented DL/DLL/OFFSET residue starts at zero.
Line 308,t184 is tick 140016. The same-origin VBLANK 139874 is past, so the first
strictly future deadline is 281976, an elapsed 141960 ticks. Output/cartridge
epochs start at handoff; TIA catches up on its absolute 456-tick lattice.

Actual BIOS boot begins at 0, runs real RES and uses first deadline 139874,
946 PCM samples. Accepted 4/16 KiB geometry controls mirroring, never region.
Synthetic firmware checks mapping, first cartridge opcode and sticky rejection;
stripped OpenBIOS integration permits its distinct handoff state. No firmware,
loader or splash bytes are reconstructed by fast boot. The unchanged EEPROM 5 ms
duration is a compatibility policy, not a hardware latency measurement.

## NMI

On the 7800 only MARIA raises NMI, after the burst or the VBE that reads a DLL entry with DLI set, and never while
MEN = 0. SALLY latches the edge at the end of a φ2. The bus records `nmi_phi2`, the φ2 count of the first cycle
whose data tick falls at or after the edge: tick 4 of a fast cycle, tick 5 of a slow one. An edge raised while MEN = 0 is dropped. One edge at a time: an edge
raised while an earlier one is not yet latched is ignored, just as a latched edge the CPU has not taken keeps its
`nmi_phi2` against a later one. Raising an edge returns 0 when it is ignored, under MEN = 0 or behind another, and
1 when it is queued for latching. A queued edge can still be dropped by the latch, when an earlier latched edge has
not been taken or MEN was cleared before it latched. MARIA raises one edge per DLI and never retries, so it needs
only the 1: it keeps it as the line's `DLI_QUEUED` flag (`docs/hardware/maria.md`, The DLI). Nothing counts dropped
edges.

Held RDY and HALT cycles each carry their own φ2 count. So an edge latched during a hold names the held cycle it
fell in, not the cycle that completes the access. After `STA WSYNC`, an edge at tick 100 falls in held cycle 25 of
`CLC`'s opcode fetch, which completes at φ2 115. Where the CPU compares that count, and so after which instruction
it takes the NMI, is in `docs/hardware/sally.md`, NMI.
