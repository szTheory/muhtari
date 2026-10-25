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

Each port carries a ProLine, a 2600 joystick, a driving controller, an XG-1, two paddles,
a SaveKey/AtariVox EEPROM, or nothing. The host names the device per port; when it names none, the
cartridge's A78 header does. A header that names no controller leaves the port unplugged. A header device that is not
modelled yet, such as a keypad, is wired as a ProLine. The legacy setter retains
its ProLine fallback and explicit XG-1 refusal; extended input and load overrides
can select the gun on either port.

The EEPROM attaches at successful load, on either port, with at most one device. Explicit load overrides win over
header declarations; a save hint defaults to port 2 unless an explicit non-save choice suppresses it. Its identity,
port and battery offset remain fixed until reload. Per-frame auto or the attached identity is accepted; adding,
removing, moving or changing that identity is refused before any pending joystick input changes. Other joystick
input retains its VBLANK latch semantics. See `docs/embedding.md`, Battery storage and HSC firmware.

## XG-1

The [ANTIC XG-1 hardware article](https://www.atarimagazines.com/v7n8/xg1light.php)
describes pin1 low at rest and high with the trigger pulled. SWCHA bit4 on
port1, bit0 on port2 therefore has this reversed switch polarity. Pin6 is
the separate active-low optical sensor, observed on INPT4/5. Light can
qualify without a trigger pull. Software selects one-button mode; the
existing PB2/PB4 and INPTCTRL electrical predicate remains unchanged.

The selected optical compatibility model is unmeasured: delay D=64 master
ticks, width W=30 ticks, aperture L=9 native rows and native luma threshold
T=8. For aim (x,y), sample exactly x once per row y-4..y+4, clipped to native
rows0..241, never wrapped. Qualify only actual visible MARIA emissions whose
native color low nibble is at least8. Hue, host palettes, scaling, filters
and caller video buffers cannot change this test. With DMA disabled,
visible BACKGRND still qualifies. Off-screen aim and nonvisible rasters
produce no samples. TIA video blanking bits do not blank MARIA.

A qualified historical sample at tick s schedules a sensor low on
[s+64,s+94): high at+63, low through+93, high at+94. The sample carries its
original raster, x and absolute tick; x319 emits at its raster's tick454,
before the next line's buffer swap. Later palette/aim changes, darkness
or blanking cannot erase a scheduled pulse. Touching/overlapping responses
merge without truncating active light. One sample per raster and delay
shorter than the line permit one pending response and one active interval,
with fixed state and no pixel queue. Replacement, successful reload and
power cycle clear light. Repeated same-device packets preserve history.

Extended packets use native coordinates, primary button1 for the trigger
and an explicit off-screen flag; omitted aim tails default off-screen.
No public calibration API or title-specific correction is provided.
These parameters derive from the [controller preparation, section3.4](../../.planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md),
with brightness and aperture policy made explicit here; they establish
deterministic diagnostic behavior, not hardware-calibrated retail accuracy.

Independent timing vectors use 454 ticks/line, native row r at raster r+16,
and emission s=origin+(r+16)*454+135+x. Aim(100,20) includes rows16..24,
excluding15/25; y0 clips to0..4 and y241 to237..241. At raster100,
x319 emits at origin+45854 (100*454+454), with response at origin+45918
through45947 (raster-relative518..547). The last row241/raster257 at
x319 emits at raster258 tick0; VBLANK starts at tick42 and the response
is still due at ticks64..93, including after the host returns that frame.
Tests isolate each bright sample by changing BACKGRND from dark before
emission to dark at s+1, then observe s+63/64/93/94 on both ports with
and without video. DMA is off; no completed framebuffer is the oracle.
JAM/HALT and observer tests separately prove unpolled D6 capture and
nonmutating peeks. These are arithmetic conformance vectors, not measurements.

### Light diagnostic

The owned `controller-light` executable has identity15 and scenario0. It
decodes the held startup switches before mailbox initialization; a nonzero
selector records its actual value and completes, so the judge rejects it.
Each process keeps both devices fixed. Movie2/library/module frames1/2 are
neutral with center aim(159,120). Frame3 pulls the left trigger; frame4
releases it, pulls the right trigger and marks right aim offscreen; frame5
holds that state without a library submission. Frame6 releases triggers
and aims left(0,0), right(319,241); frame7 marks both offscreen. Frame8
and subsequent frames return to center aim. Console switches remain released.

The19 records, in order, are scenario0; luma7 sensor highs128/128;
left-only trigger SWCHA254; luma8 captured lows0/0; right-only trigger
SWCHA239; left-visible/right-offscreen sensors0/128; repeated D6 enables
retain0/128; first/last-pixel lows0/0 after the frame7 VBLANK polling
routine; historical right low0 in frame8; released triggers SWCHA238;
native palette8 pixel lows0/0; and palette7 pixel highs128/128. Trigger
values follow pin1's positive polarity: released bits4/0 are low. Luma7
never reaches T8; luma8 does, even with DMA disabled. D6 clear/enable
separates the illumination cases. The frame7 CPU mailbox reads finish
after the last pixel's +64 response; the exact +42 host return and +63/64
edges are independently checked by the CPU/bus tests above.

The final two cases enable real DMA with a five-byte 160A object at hpos78,
width1 byte, graphics FF at every zone offset. Its four160A pixels cover
native x156..163, including159. Seventeen16-line zones cover the aiming
rows. Background remains7; only the native P0C3 pixel changes8→7. The
numbers follow the model before any cross-host hashes are compared.
Mailbox magic/version/status/count, sequential record IDs and CRC are judged
independently; CRC-valid wrong values, missing/duplicate IDs and wrong
counts fail through the same actual report consumer. No retail or calibrated
hardware claim follows from this diagnostic.

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
| Driving | 0 | 0 | 0 while fire is pressed |
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

## Driving

The extended setter and explicit load choice accept `MUHTARI_PORT_DRIVING` on either port. Only `dx` and button 1
apply; `dy` and other payload fields must be zero. Positive X means clockwise; one native host unit means one
connector transition, not a host pixel, revolution or complete quadrature cycle. The legacy setter retains its
released unsupported-device refusal and legacy header fallback.

Connector pins (1,2) start at (1,1), and clockwise walks (1,1) → (1,0) → (0,0) → (0,1) → (1,1).
Counterclockwise walks backward. This follows the driving switch table and connector routing in the original
Atari VCS domestic field service manual, printed pages 7-4–7-9. Pins 3 and 4 stay high; button 1 grounds pin 6.
Thus the SWCHA nibble sequence is F,D,C,E,F for positive X and F,E,C,D,F for negative X.

The deterministic compatibility policy uses 454 master ticks per transition (one native line), with at most 255
unspent transitions per axis. These are operating-cost choices, not measured wheel calibration. A fresh maximum
drains in 115770 ticks, within one full NTSC frame; an axis produces at most 263 transitions per full frame.
State is fixed-size in the instance. Each signed input is clamped to [-255,255] before addition, then the summed
residual is independently clamped; excess is discarded immediately. INT32_MIN never needs negation. Pending
submissions replace each other; VBLANK transfers the latest delta once. No submission does not repeat it.

An idle axis starts at latch+454. An in-flight deadline survives addition, cancellation and reversal; a zero
residual at that deadline makes no edge. Phase persists while idle and across same-device submissions. Device
change and power cycle restore phase F and cancel residual work. Actual CPU reads and peeks only observe pins;
the bus delivers each due transition chronologically before the access tick, through JAM, HALT/RDY and instruction
overshoot. Only actual CPU input-register reads count as input polling.

Independent numeric timing expectations: latch at tick 100 with left dx=2 and right dx=-1 gives edges at 554
(SWCHA DE), then 1008 (CE), then idle CE. At 553 SWCHA is FF. A cartridge `LDA $0280` has three 4-tick fetches
and its slow data tick at start+17, completing at start+18; `LDA $000D` has the same timing and reads right fire
in D7. With right fire held that bit is zero. From phase D, one reverse edge restores F; from C it restores D.

## Motion diagnostic

The owned `controller-motion` executable uses mailbox version 1, ROM identity 14 and explicit scenario `motion0`
(driving on both ports). Each scenario is a fresh load with fixed device overrides. Reset/Select/Pause encode the
scenario index with weights 1/2/4 (active-low SWCHB bits 0/1/3), held through frame 2 and released from frame 3.
After reset the ROM waits through visible time to the first full-frame VBLANK start before decoding the selector.
The first record is that decoded index. Scenario 0 is currently the only implemented family member.

The stimulus schedule, declared independently of observations, is: frames 1–2 neutral; frame 3 left dx=+4 and
fire, right dx=-4 without fire; frame 4 left dx=-1 without fire, right dx=+1 and fire; frame 5 has no submission;
frame 6 submits zero deltas and released fire. Devices remain driving throughout. Frame numbers include the short
first frame after fast boot. A following host integration slice supplies this schedule through all three hosts.

At each sampling boundary VBLANK starts at line 258 tick 42. `STA WSYNC` parks the next opcode until line 259 tick 0,
412 master ticks later. The following `LDA $0280` has its data at opcode-start+17: three fast 4-tick fetches then
the slow cycle's fifth tick. Whole-cycle RDY release rounds opcode-start up by 0..3 ticks, so this read occurs at
VBLANK+429..432, strictly before the first edge at +454. Storing the byte in RAM (16 ticks) and another zero-page
`STA WSYNC` (14 ticks) stay inside the line. Repeating gives reads at +883..886, +1337..1340, +1791..1794 and
+2245..2248. They fall after exactly 1,2,3,4 edges. The ROM records the actual sampled bytes after capturing them.

Independent expected records, in order, are: scenario 0; idle FF; idle fire 80/80; frame-3 samples FF, DE, CC, ED, FF;
frame-3 fire 00/80; frame-4 pre-edge FF and post-edge ED; frame-4 fire 80/00; frame-5 held ED and right fire 00;
frame-6 neutral ED and right fire 80. Fire records mask D7 so open-bus low bits cannot hide or create a press.
These 19 values follow the connector tuples and the selected cadence above, not a frame hash. The `motion0` rows
in `tests/diag/expected.csv` label fitted timing policy separately from wiring facts; count and CRC cover all records.

## Paddles

Extended input and explicit load selection accept a pair on either port. Each
`paddle[0/1]` is absolute travel0..65535; buttons1/2 ground connector pins4/3
(SWCHA7/6 on the left,3/2 on the right). Pot0 is pin5/INPT0 or2; pot1 is
pin9/INPT1 or3. Other payload fields and direction buttons must be zero.
The legacy setter keeps its paddle refusal and automatic ProLine fallback.

The original 7800 schematic C025231-001 (Stella system training manual PDF p69)
specifies10kΩ series resistors R38–R41 and68nF capacitors C43–C46. The Atari
VCS domestic field service manual figure7-4 describes the1MΩ pot and paired
button wiring. These are circuit facts; comparator threshold0.32, immediate
ideal dump and the setting-change policy below are unmeasured compatibility
fits. There is no per-game polling-window scale or hardware-calibration claim.

For position p, R=10000+floor(p*1000000/65535)Ω and the crossing duration is
ceil(R*187747/1000000) master ticks. The coefficient approximates the68nF RC
charge to the fitted threshold using the NTSC master clock. Independently
computed vectors are p0/32767/32768/65535 →1878/95750/95753/189625 ticks.
Release at100 therefore reads low at1977, high at1978 and1979 for p0.

Each instance owns four finite Q32 action accumulators, durations, change
epochs and projected crossing ticks. Action ranges0..2^32. Constant-rate
progress is elapsed*2^32/duration, floored only when a setting changes; the
new remaining delay is ceil((2^32-action)*new_duration/2^32). After1000 ticks
at p0 action is2286990040; switching to65535 crosses another88654 ticks later.
Reads project no new rounding. The existing bus deadline owner delivers only
the four possible crossings, including under JAM/HALT and across host frames.
A high capacitor remains high through setting changes until D7 grounds it.
D7-high immediately clamps all four inputs low; repeated high writes retain
zero, repeated low writes retain the charge epoch. Device replacement/reset
clears action; same-device packets preserve it. D6 controls only trigger
latches, independently of these pots. No allocation, floating point or
per-tick analog iteration is used.

The owned `controller-analog` image uses mailbox version1, identity16 and
scenario0 (`analog0`). It decodes the same fresh-load console selector as
the motion diagnostic; future scenario1 has a separate record set and count.
Frames1–5 hold left pots0/65535, right32767/32768. Frames6–12 hold left
65535/0, right0/65535. Frame3–4 buttons are left1/right2, frame5–6 left2/right1;
all others are released. This fixed schedule applies identically to library
packets, immutable movie2 bytes and the loaded module.

After frame3 VBLANK, the ROM dumps and records all pot bits as0, records
SWCHA123 (7B), releases D7 and waits five native WSYNC lines. The first
wait spans at least four full454-tick lines plus its first partial line.
The LDX/JSR, WSYNC loop exit, RTS and first slow input access add more
than84 ticks: the first INPT read is after1900 and before3000 ticks;
all four reads finish before5000, well before95750.
Independent records are128,0,0,0. Another210 lines plus record overhead puts
the next four reads beyond95753 and below189625:128,0,128,128. Another210
lines exceeds189625; all-pot bits are15. These inequalities follow454 ticks
per WSYNC line, actual CPU data ticks and the mailbox routine's bounded
instructions, rather than a game-specific pot scale. Exact±1 crossing and
mid-charge vectors are checked separately by the CPU/bus tests.

At frame5 SWCHA is183 (B7). At frame6 a fresh dump/release with swapped
endpoints and five lines gives0,128,128,0. Another420 lines gives all-pot15;
frame8 SWCHA is255. The19 sequential numeric records, protocol count,
completion and CRC are judged before any pixel/raw-transition/PCM equality.

## Not modelled

- Trak-ball, keypad, mice, AtariVox speech, SNES2Atari and Mega7800
  (cartridge notes, section 3.2).
