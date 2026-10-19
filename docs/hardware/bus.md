# The 7800's bus

SALLY reaches everything through one bus: RAM, the TIA, the RIOT, MARIA's registers and the cartridge. The 7800
stretches some of its cycles, and MARIA can stop or hold the CPU. `src/bus.c` owns all of this, and it is the only
place where emulated time advances. This page covers the bus's behaviour. The CPU itself is in
`docs/hardware/sally.md`.

Sources:

- **The Software Guide**, the 7800 Software Guide on 7800.8bitdev.org, rev 1320: clocks, the memory map, INPTCTRL and
  the HALT gate.
- **The system notes**, `.planning/preparation/A7800-SYSTEM.md` sections 1 and 3 to 5, which gather the facts below
  with their own sources: MiSTer's Atari 7800 core at `0dc8ad2e3ff7` (facts only), MAME, and measurements posted on
  AtariAge.
- **The Stella Programmer's Guide** (1979), section 3.2: a write to WSYNC holds the CPU until the TIA's horizontal
  blank begins.

## Clocks

One tick is one MARIA clock, the crystal divided by 2: 7,159,090.9 Hz on NTSC. `ticks` counts them from power-on in
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
| `$0020-$003F` (same mirrors) | MARIA | fast | the open-bus byte, until MARIA's registers exist |
| `$0040-$00FF`, `$0140-$01FF` | RAM, the same bytes as `$2040-$20FF` and `$2140-$21FF` | fast | RAM |
| `$0240-$027F`, `$0340-$037F` | nothing | fast | the open-bus byte |
| `$0280-$02FF` (`$0380`) | RIOT ports and timer | slow | the RIOT, 8 bits (`docs/hardware/riot.md`) |
| `$0480-$04FF` (`$0580`) | RIOT RAM | slow | the RIOT's RAM |
| `$0400-$047F`, `$0500-$057F`, `$0600-$17FF` | cartridge | fast | the open-bus byte with no cartridge |
| `$1800-$27FF` | RAM, 4 KiB | fast | RAM |
| `$2800-$FFFF` | cartridge, and the BIOS while EXT = 0 | fast | the open-bus byte with nothing loaded |

`$2800-$2FFF` is left undecoded, so it reads as cartridge space.

With MEN = 0, at power-on and in 2600 mode, the 2600 decode applies: with A12 = 0, A7 = 0 selects the TIA and A7 = 1
the RIOT. Everything else is the cartridge. There is no system RAM, and every cycle is slow.

At power-on every RAM byte is 0. Other fill patterns are a later profile option.

## Slow cycles

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

The idle values hold until the inputs are wired. MARIA's addresses, `$20-$3F`, return the open-bus byte until
MARIA's registers exist.

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

## RDY

RDY stops SALLY in a read cycle. It never stops a write, so a read-modify-write that sets it completes both of its
writes, and the hold falls on the next read. A held read repeats whole 4-tick φ2 cycles, each counted in `phi2`, until
a cycle starts at or after the release. It then runs at its own length: 4 ticks for the next opcode's fetch from RAM.

On the 7800, RDY is MARIA's WSYNC wire-ANDed with the TIA's. MARIA's arrives with MARIA. The TIA's is a write to
`$02` or any mirror: RDY goes low and stays low until the TIA's line wraps, at its horizontal blank (Stella
Programmer's Guide). The TIA's line is 456 ticks, 2 more than MARIA's, so the release slides 2 ticks against MARIA's
line every line. `tia_phase` is the TIA's position in its line at tick 0, from 0 to 455. The release is the first
tick after the write's data tick at which tick + `tia_phase` is a multiple of 456.

The release tick and the rule that the held read runs in the first cycle starting at or after it are fitted, not
measured. They follow from the sources above, and a WSYNC diagnostic on hardware would confirm or move them by one
cycle. A refit changes the behaviour revision.

So `STA WSYNC` from tick 0, with `tia_phase` 0, writes at tick 13 and ends at 14. The next opcode fetch is held
from 14 for 111 cycles, runs from 458 to 462, and `phi2` counts every held cycle.

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
ignored, and the window in flight keeps its hold. The request returns 0 when it is ignored and 1 when it is taken,
so MARIA can check that it asked after the last window ended.

The two cycles of latency are fitted, not measured. They are MiSTer's figure, chosen so that a 46-cycle loop counts
correctly. A scope shows the bus free by the second φ2 fall, and other emulators stop the CPU at once (System notes,
sections 2 and 3). `src/bus.c` names the constant and keeps the rule in one place. A refit changes the behaviour
revision.

## NMI

On the 7800 only MARIA raises NMI, at the end of a zone's DMA, and never while MEN = 0. SALLY latches the edge at the
end of a φ2. The bus records `nmi_phi2`, the φ2 count of the first cycle whose data tick falls at or after the edge:
tick 4 of a fast cycle, tick 5 of a slow one. An edge raised while MEN = 0 is dropped. One edge at a time: an edge
raised while an earlier one is not yet latched is ignored, just as a latched edge the CPU has not taken keeps its
`nmi_phi2` against a later one. Raising an edge returns 0 when it is ignored, under MEN = 0 or behind another, and
1 when it is taken.

Held RDY and HALT cycles each carry their own φ2 count. So an edge latched during a hold names the held cycle it
fell in, not the cycle that completes the access. After `STA WSYNC`, an edge at tick 100 falls in held cycle 25 of
`CLC`'s opcode fetch, which completes at φ2 115. Where the CPU compares that count, and so after which instruction
it takes the NMI, is in `docs/hardware/sally.md`, NMI.
