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
| `$0280-$02FF` (`$0380`) | RIOT ports and timer | slow | the open-bus byte, until the RIOT exists |
| `$0480-$04FF` (`$0580`) | RIOT RAM | slow | the open-bus byte, until the RIOT exists |
| `$0400-$047F`, `$0500-$057F`, `$0600-$17FF` | cartridge | fast | the open-bus byte with no cartridge |
| `$1800-$27FF` | RAM, 4 KiB | fast | RAM |
| `$2800-$FFFF` | cartridge, and the BIOS while EXT = 0 | fast | the open-bus byte with nothing loaded |

`$2800-$2FFF` is left undecoded, so it reads as cartridge space.

With MEN = 0, at power-on and in 2600 mode, the 2600 decode applies: with A12 = 0, A7 = 0 selects the TIA and A7 = 1
the RIOT. Everything else is the cartridge. There is no system RAM, and every cycle is slow.

At power-on every RAM byte is 0. Other fill patterns are a later profile option.
