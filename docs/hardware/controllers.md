# Controllers and console switches

The 7800 has two controller ports and five console switches. `src/ports.c` holds the device on each port and the
buttons and switches the host applied, and gives the pins they drive. `src/riot.c` reads SWCHA and SWCHB through those
pins, and `src/tia.c` reads INPT0-INPT5. Input set through `muhtari_set_input` reaches the pins at the next VBLANK
start, so a whole frame sees one input (`docs/embedding.md`, Input).

Sources:

- **The cartridge notes**, `.planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md`: section 3.1 maps the pins to
  the registers, section 3.2 describes each device, and section 3.3 two-button mode. They gather the 7800 Software
  Guide, MAME, a7800 and MiSTer's Atari 7800 core (facts only) with their own sources.
- **The system notes**, `.planning/preparation/A7800-SYSTEM.md` section 6: the RIOT's power-on state, ORB $FF among
  it.
- **The RIOT**, `docs/hardware/riot.md`: how a port reads its pins, and PA7's edge detector.

## Devices

Each port carries a ProLine, a 2600 joystick, a SaveKey/AtariVox EEPROM, or nothing. The host names the device per port; when it names none, the
cartridge's A78 header does. A header that names no controller leaves the port unplugged. A header device that is not
modelled yet, such as a light gun or paddles, is wired as a ProLine.

The EEPROM attaches at successful load, on either port, with at most one device. Explicit load overrides win over
header declarations; a save hint defaults to port 2 unless an explicit non-save choice suppresses it. Its identity,
port and battery offset remain fixed until reload. Per-frame auto or the attached identity is accepted; adding,
removing, moving or changing that identity is refused before any pending joystick input changes. Other joystick
input retains its VBLANK latch semantics. See `docs/embedding.md`, Battery storage and HSC firmware.

## Serial EEPROM

SaveKey and AtariVox share a 32 KiB 24LC256 model. AtariVox speech is unavailable. Port 2 uses SWCHA bit 2 for SDA
and bit 3 for SCL; port 1 uses bits 6 and 7. Pins not driven by the host are pulled high. Software can drive an
output through ORA or release a zero output by changing DDRA; both reach `src/ports.c` at the RIOT access's actual
data tick, tick 5 of its 6-tick cycle. The device only pulls SDA low. SWCHA includes that low during turnaround,
even when the host drives its output high. Unused directions and buttons on an EEPROM port are inactive.

Protocol facts come from [Microchip DS20001203Y](https://ww1.microchip.com/downloads/aemDocuments/documents/MPD/ProductDocuments/DataSheets/24AA256-24LC256-24FC256-256K-I2C-Serial-EEPROM-DS20001203.pdf),
sections 4–8: START/STOP use SDA transitions while SCL is high, incoming bits use rising SCL, and ACK/data is stable
for the next high period. The wired control bytes are A0 for write and A1 for read. Two address bytes select a
15-bit address; A15 is ignored. Random reads set that address before repeated START; current reads use the retained
pointer. Sequential reads wrap at 32768 bytes and end on the master's NAK. Page data wraps within 64 bytes.

The implementation keeps host drive separate from device SDA. Only host changes that alter SDA while SCL remains
high can start or stop a packet; device ACK/data edges cannot create host events. CPU reads and peeks only observe
pins. An unsupported control byte is NAKed and clocks are ignored until START or STOP. START restarts an incomplete
packet, discarding un-stopped page data but retaining the current address. Address-only or incomplete traffic
without complete data bytes does not program. STOP schedules the completed data bytes of at most one page;
over-page traffic overwrites page-buffer positions, without growing state.

Microchip specifies a maximum 5 ms page write and no ACK during programming. muhtari selects that maximum as a
fixed deterministic compatibility model: `MUHTARI__SAVEKEY_BUSY_TICKS = 35796`, the upward-rounded
`315000000 / 8800` half-crystal ticks. This is a selected NTSC interval, not a hardware timing measurement. Pending
bytes are invisible in battery memory until the deadline. Completion increments generation once only if a byte
changes; a same-value write is still busy and stays clean. Host import is clean and never starts programming.

Programming catches up at actual pin-access ticks and after frame execution, including instruction overshoot,
JAM, DMA/stalls and partial execution ending in BIOS refusal. A rejected frame that executes nothing changes no
device state. Power cycle settles work due at or before the current tick before resetting the clock epoch, then
discards the protocol, page buffer and future programming. Thus a reset one tick before completion discards it;
reset at or after completion retains its bytes and change token. Queries, host persistence and destruction never
advance time to finish programming. The persistent array belongs to the existing instance battery span; the port
owner adds only one fixed page and finite protocol state, with no allocation after load.

## Directions

Pins 1-4 are up, down, left and right. A pressed direction closes its switch and pulls the pin to 0. Port 0 drives
SWCHA bits 7-4 and port 1 bits 3-0 (cartridge notes, section 3.1):

| Bit | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|-----|---|---|---|---|---|---|---|---|
| Port 0 | right | left | down | up | | | | |
| Port 1 | | | | | right | left | down | up |

SWCHA reads `(ORA & DDRA) | (pins & ~DDRA)`, so a bit the program drives as an output reads ORA instead. Port 0's
right is PA7: a press or a release reaches PA7's edge detector at the VBLANK start that applies it, and sets the flag
on the selected edge (`docs/hardware/riot.md`, PA7).

## Buttons

INPT0-INPT5 drive D7 only; the other bits read the open-bus byte (`docs/hardware/bus.md`, Open bus). Pins 5 and 9 reach
INPT0 and INPT1 on port 0, INPT2 and INPT3 on port 1. Pin 6 reaches INPT4 on port 0 and INPT5 on port 1 (cartridge
notes, section 3.1 and section 3.2). Button 1 is the ProLine's right button, button 2 its left.

| Device | INPT0 / INPT2 | INPT1 / INPT3 | INPT4 / INPT5 |
|--------|---------------|---------------|---------------|
| ProLine, two-button mode | 1 while button 1 is pressed | 1 while button 2 is pressed | 1 |
| ProLine, one-button mode | 1 while button 1 is pressed | 1 while button 2 is pressed | 0 while either is pressed |
| 2600 joystick | 0 | 0 | 0 while fire is pressed |
| Unplugged | 0 | 0 | 1 |

INPT0-INPT3 follow the ProLine's buttons in one-button mode too, as MAME, a7800 and MiSTer have them. The console's own
one-button readings are unverified (cartridge notes, section 3.2).

## Two-button mode

A port is in two-button mode when its SWCHB pin, PB2 for port 0 and PB4 for port 1, is an output (its CTLSWB bit 1)
driven 0 (its ORB bit 0), and INPTCTRL's TIA-EN is 0. The driven pin puts +5 V on the port's pin 6 (cartridge notes,
section 3.1 and section 3.3). The mode follows those levels whenever an input is read; no write latches it.

The RIOT's ORB powers on $FF, so a game that writes CTLSWB $14 and never writes SWCHB, as One on One and Worm! do,
leaves both ports in one-button mode. A game enables two-button mode with CTLSWB $14 and SWCHB $00. INPTCTRL's
TIA-EN, set before INPTCTRL is locked, keeps both ports in one-button mode whatever SWCHB holds.

A 2600 joystick's fire grounds pin 6. In two-button mode that shorts the pin the console drives, which the Software
Guide calls possibly harmful; INPT4 [INPT5] still reads 0. Each frame that ends with a joystick's fire held on a port in
this mode adds 1 to `io_hazard`, an internal count in `struct muhtari__ports` that the public API does not show yet.

## Switches

SWCHB's input pins carry the console switches. Reset, Select and Pause are buttons that read 0 while held. A
difficulty switch at A reads 1 and at B reads 0. D2, D4 and D5 have nothing driving them as inputs and read 1:

| Bit | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|-----|---|---|---|---|---|---|---|---|
| Switch | right difficulty | left difficulty | — | PB4 | Pause | PB2 | Select | Reset |

With nothing held and both difficulties at B, SWCHB reads $3F. Like SWCHA it reads ORB on its output pins, so in
two-button mode on both ports it reads $2B.

## Lag

A lag frame is one in which the CPU reads none of SWCHA, SWCHB and INPT0-INPT5, in any mirror. `src/bus.c` records
the read in the CPU's read path. MARIA's DMA fetches never pass through that path, since their TIA and RIOT reads float
with no side effect, and neither does an internal peek. So a display list that points at $0280 still leaves the frame
a lag frame. `muhtari_get_counters` returns the count.

## Not modelled

- The TIA's VBLANK D6 latch of INPT4 and INPT5, and its D7 dump of INPT0-INPT3, which paddles need.
- The light gun, paddles, trak-ball, driving controller, keypad, mice, AtariVox speech, SNES2Atari and Mega7800
  (cartridge notes, section 3.2).
