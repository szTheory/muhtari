# The libretro core

`muhtari_libretro` runs Atari 7800 cartridges in RetroArch or any other libretro frontend. It loads `.a78` images
and headerless `.bin` images from memory, NTSC only; a PAL cartridge is refused. Started with no content, it shows
the built-in test card ([docs/runner.md](runner.md#test-card)). A refused image leaves a line in the frontend's
log naming the reason, such as `muhtari: content refused: unsupported board: bankset`.

## Controls

The RetroPad follows the ProSystem core's layout, so remaps made for it carry over.

| RetroPad | Port 1 | Port 2 |
|---|---|---|
| D-pad | joystick | joystick |
| B | button 1 (right) | button 1 |
| A | button 2 (left) | button 2 |
| Select | console Select | - |
| Start | console Pause | - |
| X | console Reset | - |
| L | left difficulty switch | - |
| R | right difficulty switch | - |

L and R flip their switch on each press, and a message shows the new position, such as `Left difficulty: A`.
The console's Reset is the switch a game reads; RetroArch's own Restart is a power cycle (below).

## Devices

Each port offers fifteen devices in Quick Menu → Controls:

| Device | What the game sees |
|---|---|
| Auto (cartridge header) | the header's controller; unimplemented devices use ProLine, empty or unknown codes unplug; the default |
| ProLine | the two-button 7800 controller |
| Joystick | a one-button 2600 joystick; A fires too |
| None | nothing plugged in |
| SaveKey | one 32 KiB serial EEPROM |
| AtariVox EEPROM | the same EEPROM interface; speech is unavailable |
| Driving | relative MOUSE X moves the wheel; MOUSE LEFT fires; Y is ignored |
| XG-1 | absolute LIGHTGUN SCREEN_X/Y aim; TRIGGER fires; IS_OFFSCREEN suppresses light; RELOAD fires offscreen |
| Paddle pair | ANALOG left X/right X set pots1/2; RetroPad B/A press their independent buttons; Y is ignored |
| Keypad | twelve RetroPad controls map to all twelve held keys, independently |
| CX22 | relative MOUSE X/Y; LEFT or RIGHT is shared fire |
| ST mouse | relative MOUSE X/Y; independent LEFT/RIGHT fire |
| Amiga mouse | relative MOUSE X/Y with Amiga wiring; independent LEFT/RIGHT fire |
| SNES2Atari | RetroPad D-pad, B/Y/Select/Start/A/X/L/R retain their SNES names |
| Mega7800 | D-pad; Y→A, B→B, A→C, L→X, X→Y, R→Z, Select→Mode, Start→Start |

Driving uses `RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_MOUSE, 0)` for selection and base
`RETRO_DEVICE_MOUSE` IDs for polling, once after each frame's input poll. The frontend
must bind that abstraction; an absolute steering wheel is not mapped automatically.
Auto selects driving when the header names it. Unknown callback identities log a warning
and unplug ordinary input. Input descriptors refresh on load and device selection.

XG-1 selects `RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_LIGHTGUN, 0)` and polls the five
base LIGHTGUN inputs in the table once per frame. Deprecated relative X/Y and
PAUSE are unused. The frontend must provide absolute lightgun bindings; the
adapter has no device or host-window geometry. For a valid coordinate v in
[-32767,32767], the native coordinate is
`floor((v+32767)*(dimension-1)/65534)`, with a 64-bit integer product over the
core's current reported native width/height. Width320 maps -32767/0/32767 to
0/159/319; height242 maps them to0/120/241. Either coordinate -32768 marks the
aim offscreen before conversion, using neutral0/0 coordinates. Explicit offscreen
and reload also suppress illumination; reload independently holds the trigger.
Trigger state never implies sensed light. Missing input callbacks leave aim offscreen.
The native D64/W30/L9/T8 optical model is explicitly unmeasured; see
[controller hardware](hardware/controllers.md#xg-1). Auto activates header guns.

Console controls come from the lowest numbered ordinary ProLine/joystick port, retaining
port 1's preference when both are ordinary. Otherwise the lowest numbered active
driving/mouse/CX22/gun/paddle/keypad/adapter port supplies console controls. Driving uses MIDDLE for Pause,
BUTTON_4 for Select and RIGHT for Reset; LEFT remains fire. XG-1 uses LIGHTGUN
START for Pause, SELECT for Select and AUX_A for Reset; these do not fire or reload.
Paddles use RetroPad Start/Select/X for Pause/Select/Reset; B/A remain the two buttons.
Keypad, SNES2Atari and Mega7800 use supplemental RetroPad L2/R2/L3 for Pause/Select/Reset; their controls
never operate console switches, and difficulties use options.
With neither
port active, supplemental port 1 RetroPad L2/R2/L3 provide Pause/Select/Reset. Driving, paddles and
supplemental controls use the difficulty options. A new ordinary console provider starts
its L/R edge history at the currently held levels, so selection creates no synthetic toggle.

Paddle pair selection is `RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_ANALOG, 0)`;
polling uses base ANALOG left/right index X plus base JOYPAD B/A, once per
frame. Each signed axis v maps to the absolute physical range with
`int32(v)+32768`: -32768/-1/0/32767 →0/32767/32768/65535. No game-specific
scale or motion accumulation applies. Frontends must bind both analog X
axes and the two RetroPad buttons; Y axes and L/R are ignored for paddles,
and difficulties use options. Missing callbacks supply neutral positions0/0
and released buttons. Auto activates header paddles, descriptors refresh
on selection, and either paddle port may coexist with the opposite EEPROM.
The RC threshold/dump model is unmeasured; see
[controller hardware](hardware/controllers.md#paddles).

Keypad selection is `RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_JOYPAD, 4)`;
polling uses base JOYPAD index0, once per key after each frame's input poll.
The row-major mapping is:

| Key | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | * | 0 | # |
|---|---|---|---|---|---|---|---|---|---|---|---|---|
| RetroPad | Y | Up | X | Left | B | Right | L | Down | R | Select | A | Start |

All twelve can be held together; there is no three-key cap. Frontends must
bind these RetroPad inputs plus the supplemental console controls. Auto
activates header keypads, descriptors refresh on selection and missing
callbacks release all keys. An EEPROM on the opposite port remains fixed
until reload. Pot-column recovery and switch ghosts follow the explicit
unmeasured analog compatibility policy in
[controller hardware](hardware/controllers.md#keypad); matrix observations
do not claim calibrated voltages or isolating diodes.

CX22, ST mouse and Amiga mouse use distinct MOUSE subclasses1/2/3
(driving remains0). The frontend is queried through base MOUSE index0:
X and Y once per frame, positive right/down, one unit per connector edge.
CX22 LEFT or RIGHT is shared fire; ST/Amiga LEFT and RIGHT are independent.
Wheels and extra buttons do not drive emulated pins. When this device supplies
console controls, MIDDLE/BUTTON_4/BUTTON_5 are Pause/Select/Reset; RIGHT
remains device fire. Ordinary pads retain priority and difficulty shortcuts;
other providers use difficulty options. Auto follows supported header devices,
descriptors refresh with selection, missing callbacks supply zero/released,
and an opposite EEPROM remains frozen until reload. Frontends must support
and bind base relative MOUSE axes and the supplemental buttons; selection
alone cannot make unsupported physical bindings available. Sensitivity is a
frontend setting; the core uses its bounded, uncalibrated timing policy in
[controller hardware](hardware/controllers.md#motion).

SNES2Atari and Mega7800 select JOYPAD subclasses5/6 respectively, keeping
all earlier subclass values. Polling uses base JOYPAD index0 once per native
control per frame. Their Start, Select, X and shoulders operate only the
adapter; difficulties use options. An ordinary controller on either port
takes console priority. With two adapters the lowest numbered port supplies
supplemental L2/R2/L3. Frontends must bind the complete RetroPad plus these
supplemental controls; selecting a subclass supplies no physical adapter I/O.
Missing callbacks release controls. Auto activates supported header adapters
before the first frame, and an opposite EEPROM stays frozen until reload.
Owned numeric diagnostics exercise every listed family and both ports through
the library, actual movie2 runner and loaded module. This establishes the named
models and mappings; fitted optical/charge/recovery/cadence parameters and
adapter contention/restart policies remain unmeasured, without a universal
retail-playability claim. Detailed cases stay in process; real RetroArch retains
the two existing screenshot and save-roundtrip cases.

## Options

| Option | Values | Applies |
|---|---|---|
| Boot (`muhtari_boot`) | Fast (default), BIOS | when content is loaded |
| Palette (`muhtari_palette`) | NTSC (warm) | always |
| Left difficulty (`muhtari_left_difficulty`) | B (default), A | at load and Restart; a change sets the switch at once |
| Right difficulty (`muhtari_right_difficulty`) | B (default), A | at load and Restart; a change sets the switch at once |
| High Score Cartridge (`muhtari_hsc`) | Auto (default), On, Off | at content load |
| Port 1/2 battery device (`muhtari_battery_port1`, `muhtari_battery_port2`) | Auto (default), None, SaveKey, AtariVox | at content load |

There is no region option: this release runs NTSC only.

Battery options override controller requests supplied before load; Auto follows those requests, then
cartridge metadata. A SaveKey hint without a location selects port 2. Battery None suppresses automatic
storage on its port while retaining an ordinary controller selection; an automatic or save-controller
selection becomes ProLine. At most one EEPROM may attach, on either port; dual requests refuse load.
No controller callback before load is required. A later request that changes storage reports that content
must be reloaded and keeps the installed attachment and its button mapping. Auto and the installed save
identity remain valid;
ordinary non-save controller changes still work. A repeated unchanged request produces no repeated warning.

`RETRO_MEMORY_SAVE_RAM` is the final core-owned raw span immediately after successful content load:
2,048 HSC bytes, 32,768 EEPROM bytes, or HSC followed by EEPROM (34,816 bytes). Absent storage and
unloaded/no-content sessions return NULL/0. The frontend restores bytes before the first frame and owns
all `.srm` file I/O; the adapter has no save-file operations or mirror buffer. Pointer and length remain
stable until unload or reload. Restart retains committed battery bytes; it discards incomplete EEPROM
programming without advancing emulated time. Save states remain unavailable.

**Restart** power-cycles the console: volatile memory is cleared, the cartridge starts again the way it was loaded, and the
difficulty switches go back to their options.

If a frame fails, the core reports the error on screen and in the log once, and keeps presenting the last
picture until Restart or a new load. Before the first successful frame, that picture is black. Each failed run
sends actual pixels, including to frontends that do not support NULL-frame duplication, and sends 48 kHz audio
as identical left/right copies of each mono core sample. Batch callbacks take priority; the single-sample
callback is the fallback. Missing callbacks discard output while preserving future sound. Fast boot begins
with 54 samples, then full NTSC frames provide 800 or 801. Restart and content reload reset synthesis history.

## BIOS files

A BIOS is optional. With Boot set to BIOS, the core looks in the frontend's system directory for

- `7800 BIOS (U).rom`, the NTSC BIOS, 4,096 bytes, then
- `7800 BIOS (E).rom`, the PAL BIOS, 16,384 bytes,

and boots through the first usable file, showing the BIOS's own start-up screen before the game. The log names
the file and its SHA-256, so a user can tell which image ran. Any valid 4 KiB or 16 KiB image is accepted, such
as OpenBIOS. An invalid or unreadable file is named in the log and the next name is tried. With no usable file,
the core boots fast and says so on screen and in the log. An allocation failure reports an error and refuses the
load. Fast boot starts the cartridge where the console's BIOS hands over to it, without a console BIOS.

When HSC is attached, optional `highscre.bin` in the system directory supplies its 4,096-byte ROM. Only
absence selects the signature-only stub; a present unreadable, non-regular, short or oversized file refuses
the load. Path/allocation failure also refuses. The core copies the ROM before host staging is freed,
including when BIOS candidates are retried. The log records its SHA-1. Unknown firmware and missing
signatures are accepted with distinct visible warnings. No BIOS reset-vector rule applies to the HSC ROM.
No firmware is bundled. The stub exposes C6/FE signatures at $3900/$3904 and FF elsewhere; it contains no
routines, formatting or high-score screens and does not provide full OpenHSC functionality.

Loaded-module tests cover persistence and firmware failures. A separate automated RetroArch roundtrip
starts two fresh frontend processes, checks independently drawn HSC and EEPROM counters from 1 to 2,
and compares every saved byte with independent expectations and fresh runner processes. The package
declares battery saves and optional HSC firmware. Save states and AtariVox speech are unavailable.

## Achievements

The core declares a memory map (`SET_MEMORY_MAPS`) of the CPU's 64 KiB address space, which is how rcheevos'
Atari 7800 console (number 51) addresses memory, and tells the frontend that it supports achievements.

| Addresses | Memory |
|---|---|
| $0040-$00FF | system RAM at $2040-$20FF, which the console also shows here |
| $0140-$01FF | system RAM at $2140-$21FF, likewise |
| $0480-$04FF | the RIOT's 128 bytes of RAM |
| $1000-$17FF | attached HSC SRAM, directly aliasing the first 2,048 SAVE_RAM bytes |
| $1800-$27FF | system RAM, 4 KiB, flagged as system RAM |
| $2800-$2FFF | system RAM's second half, $2000-$27FF, again |
| $3000-$3FFF | the installed HSC ROM or signature stub, read-only, only when HSC is attached |
| $4000-$7FFF | cartridge RAM, when the board has unbanked RAM, except Fractalus as below |
| ROM | the fixed ROM, read-only: linear ROM; SuperGame's last bank at $C000 and, without RAM, its fixed bank at $4000; Absolute and Fractalus at $8000-$FFFF; Activision's four fixed windows below |

Everything else has no memory behind it: the TIA, MARIA and RIOT registers, absent HSC ranges, banked windows and the
rest of the address space. The map lists those spans too, as descriptors without a pointer, because rcheevos stops
mapping a region at the first address no descriptor covers: without them, $0040 and $C000 would be out of its
reach.

**Why $2800-$2FFF is mapped.** rcheevos' table for the 7800 calls $2800-$2FFF mirrored RAM, and achievement sets
written on other cores may read it. On the console, whether RAM answers there is not settled, and muhtari's bus
leaves the range to the cartridge. The map follows rcheevos, so those achievements read the RAM they expect.

**Why $3000-$3FFF depends on HSC.** rcheevos calls this range mirrored RAM too, but the 7800 Software Guide
gives it to external devices. Only an attached HSC supplies bytes there; its descriptor points to the actual
installed core-owned ROM, distinct from cartridge input. EEPROM remains serial and has no invented CPU range.

Banked RAM and banked ROM windows are left out because a descriptor cannot say which bank is showing.
Absolute descriptors point into original content at the same-numbered ROM
offsets $8000-$FFFF; its switched $4000-$7FFF window stays unmapped. Restart
restores bank zero, the same deterministic profile as a fresh load. Absolute
execution was introduced at behaviour revision 3 with ABI 1 and unchanged public layouts.

Activision exposes four fixed 8 KiB windows at $4000, $6000, $8000 and $E000.
Their OM source offsets are $18000, $1A000, $1C000 and $1E000. Descriptors
point into original host content, so identified AM input translates each
offset with XOR $2000 before the optional 128-byte header is added. This
preserves both the CPU-visible byte and original supplied-image identity.
The switched $A000-$DFFF range stays unmapped.

Fractalus exposes fixed ROM at $8000-$FFFF and sixteen 256-byte RAM aliases
at $4000-$4FFF. Adjacent page pairs point to one physical 256-byte page;
the library's physical cartridge-RAM span remains exactly 2,048 bytes. It is
volatile RAM; a battery-save span exists only when HSC or EEPROM is separately attached. Its bus also
mirrors that RAM at $5000-$7FFF, but those higher mirrors are deliberately
descriptor holes. These ranges provide partial achievement visibility,
not a complete bus view. The pinned resolver is tested against every intended
byte and every hole, below its unchanged 32-region cap.

Automated acceptance retains 13 resolver regions for Activision, 11 for
Absolute and 21 for Fractalus, including explicit holes. The owned Activision
diagnostic is also checked in identified AM and OM order, with and without
a header. Over 600 frames the library, actual runner and loaded test module
retain equal numeric mailbox records, mapped bytes, frame output and raw/PCM
audio for each layout; supplied buffers and their distinct raw identities
remain unchanged. The diagnostic is silent, so its raw transition stream is
empty while PCM sample count and bytes are still compared. These checks
establish selected board semantics and partial descriptor visibility, not
retail playability or complete achievement support.
