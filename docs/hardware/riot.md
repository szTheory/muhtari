# The RIOT (MOS 6532)

The 7800 carries the 2600's RAM-I/O-timer chip, the MOS 6532. It holds the joystick and console-switch ports, an
interval timer and 128 bytes of RAM. `src/riot.c` models it; the bus in `docs/hardware/bus.md` decides when it is
selected and gives each of its accesses a slow 6-tick cycle.

Sources:

- **The datasheet**, MOS Technology's 6532 data sheet (archive.6502.org, `mos_6532_riot.pdf`): the registers, the
  timer, the flags, PA7 and the reset state.
- **The system notes**, `.planning/preparation/A7800-SYSTEM.md` sections 3, 6 and 8, which gather the 7800's wiring
  and the power-on reports with their own sources: the 7800 Software Guide, MAME, MiSTer's Atari 7800 core
  (facts only) and measurements posted on AtariAge.

## Registers

The 6532 decodes A0-A6 and RS. On the 7800, A7 drives its chip select and A9 drives RS. So RS = 1 selects the ports and
the timer at `$0280-$02FF` (mirrored at `$0380`), and RS = 0 selects the RAM at `$0480-$04FF` (mirrored at `$0580`).
With MEN = 0, the 2600 decode selects it at A12 = 0 and A7 = 1, and A9 still picks RS (system notes, section 4).

With RS = 1, A2 picks the timer over the ports. The registers by their usual 7800 names:

| Address | Read | Write |
|---------|------|-------|
| `$0280` SWCHA | port A | ORA, port A's output register |
| `$0281` CTLSWA | DDRA, port A's direction register | DDRA |
| `$0282` SWCHB | port B | ORB |
| `$0283` CTLSWB | DDRB | DDRB |
| `$0284` INTIM | the timer (A0 = 0) | PA7 edge control (A4 = 0): A0 = 1 selects the rising edge |
| `$0285` TIMINT | the flags (A0 = 1) | the same edge control |
| `$0294-$0297` | the timer and the flags, as above | TIM1T, TIM8T, TIM64T, TIM1024T (A4 = 1, A1-A0 the interval) |

A3 enables the timer's interrupt on a timer access, and A1 enables PA7's on an edge-control write. The data of an
edge-control write is ignored. A6 and A5 are not decoded, so each register repeats every 32 bytes in the window.
Every RIOT read drives all eight data bits, so a RIOT read never shows the open-bus byte.

## Timer

The timer is a count of cycles of φ2, the CPU clock. It is not a count of time. A write of N to TIM*n*T sets an
interval I of 1, 8, 64 or 1024 cycles. Call the writing cycle p = 0, the next cycle p = 1, and so on:

- For 1 ≤ p ≤ N·I, INTIM reads `N − 1 − (p − 1) / I`. A read right after the write gives N − 1, and a read at
  p = N·I gives 0. The datasheet's "51, 50, 49" for a write of 52 matches this.
- At p = N·I + 1 the timer flag sets and INTIM reads $FF. From then on INTIM counts down once per cycle,
  whatever the interval, as `(0xFF − (p − N·I − 1)) & 0xFF`. It wraps past $00 to $FF again and keeps counting
  until the next write.

The datasheet's worked example writes 52 on the 8-cycle interval: "total time to interrupt is (52 x 8) + 1 = 417T".
Its step 5 reads $AC at elapsed time 500, and the formula gives the same: 500 − 417 = 83 cycles after $FF. The
datasheet's timing figure labels the interrupt "φ2 pulse #416". That label is one count off the datasheet's own
prose and its step 5, and the model follows the prose.

Because the timer counts φ2, it counts through every cycle the CPU does not use. Cycles frozen by HALT and cycles
held by RDY after a WSYNC write are both counted; the 7800 counts 113 per WSYNC line with DMA on or off, as
measured (system notes, section 6). A slow 6-tick cycle counts once, the same as a 4-tick cycle.

`src/riot.c` keeps the value written, the interval and the φ2 count of the writing cycle. It computes INTIM and the
flag when they are read, so the timer costs nothing per cycle.

## Flags

TIMINT returns the timer flag in D7 and PA7's flag in D6. D5-D0 read 0.

- **Timer flag.** It sets at p = N·I + 1, and any later timer read or write clears it. A timer read in the very
  cycle the flag sets leaves it set; the datasheet says "the reading of the timer at the same time the interrupt
  occurs will not reset the interrupt flag". Reading TIMINT does not clear it. Whether a later wrap past $00 sets it
  again is not modelled, because the datasheet does not say.
- **PA7 flag.** An active edge on PA7 sets it. Reading TIMINT clears it, and nothing else does.

## PA7

PA7, port A's bit 7, is also an edge detector. RES selects the falling edge. An edge-control write with A0 = 1
selects the rising edge, and one with A0 = 0 selects the falling edge again.

The datasheet says the flag sets on an active edge "no matter whether PA7 is set up as an input or an output". So a
program can set the flag itself by driving PA7 as an output through ORA and DDRA. The datasheet also says the flag
"may also be set by changing the polarity of the active interrupt". The model fits both: the detector's input is
PA7's level on the falling edge and the level inverted on the rising edge, and the flag sets when that input falls.
A polarity change with the pin high therefore sets the flag. This is a model of the datasheet's sentence, not a
measurement. A program that changes the polarity should read TIMINT before it relies on the flag.

## Ports

Each port reads `(OR & DDR) | (pins & ~DDR)`: the output register on output pins and the pin on input pins. The
controllers drive port A's pins and the console switches port B's, as `docs/hardware/controllers.md` describes; a pin
with nothing driving it reads high.

On an attached serial EEPROM port, the device's SDA pull-low also reaches the output readback during turnaround.
ORA and DDRA writes notify the existing port device owner at the access's data tick, start + 5 of the slow 6-tick
cycle, after the output/direction register changes. ORB/DDRB do not clock it. Reading or peeking a register never
creates a pin edge; PA7 still sees its resolved pin level. The separate host drive and device ACK/data behavior are
specified in `docs/hardware/controllers.md`, Serial EEPROM.

The same data-tick notification updates keypad connectivity and adapter host lines.
Only changed effective ORA/DDRA levels clock adapters; duplicate writes and device
response pins do not. Keypad driven-low rows propagate through held keys, while
output readback still returns ORA on driven pins (controllers.md, Keypad and adapters).
Autonomous motion delivers each chronological PA7 change even through JAM, HALT or
RDY holds; a complete unpolled quadrature cycle can set the flag despite ending at
its starting pin level. Peeks do not advance that history or clear TIMINT. Neither
device events nor DMA count as CPU input polling for lag accounting.

On the 7800 (system notes, section 6):

- SWCHA bits 7-4 are the left joystick's right, left, down and up, and bits 3-0 are the right joystick's. Low means
  pressed.
- SWCHB bit 0 is Reset, bit 1 Select, bit 3 Pause, and bits 6 and 7 the difficulty switches.
- A game that drives PB2 and PB4 low (CTLSWB $14, SWCHB $00) enables the two-button joysticks.

## RAM

128 bytes of static RAM, addressed by A0-A6. On the 7800 the Software Guide says not to use it. It takes the slow
cycle like every RIOT access. On hardware it fails when DMA halts the CPU in the middle of an access, and when code
runs from it. The model treats it as plain slow RAM (system notes, section 6). Power-on zero-fills it, as the
default profile fills system RAM.

## Power-on

The datasheet's RES zeroes all four I/O registers, disables both interrupts and selects the falling PA7 edge. It
does not reset the timer. The model differs in two places, both from reports on real consoles (system notes,
sections 6 and 8):

- ORB powers on to the profile's value, $FF by default. Consoles report that a game writing CTLSWB $14 without
  writing SWCHB leaves PB2 and PB4 high, so ORB acts as $FF; One on One and Worm! depend on this. Under a profile
  with ORB $00, the datasheet's value, SWCHB then reads $EB.
- The timer powers on reading $FF on the 1024-cycle interval, as if 256 had been written in the cycle before the
  first one. Its real value at power-on is undefined.

Both flags power on clear, DDRA = DDRB = ORA = $00, and PA7 is an input reading high.

## Interrupts

The 7800 leaves the 6532's IRQ output unconnected (system notes, section 3), so neither flag can interrupt SALLY,
whatever A3 and A1 select. A program can only poll the flags through TIMINT, and the model keeps no interrupt
enable.
