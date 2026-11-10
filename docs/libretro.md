# The libretro core

`muhtari_libretro` runs Atari 7800 cartridges in RetroArch or any other libretro frontend. It loads `.a78` images
and headerless `.bin` images from memory, including automatically selected PAL. Started with no content, it shows
the built-in test card ([docs/runner.md](runner.md#test-card)). A refused load is reported in the frontend log;
unsupported sound also names the unavailable sound configuration.

## External frontend contract

Install the core archive for your system using the [README](../README.md#install-in-retroarch-macos-apple-silicon),
then select **Load Core → Atari - 7800 (muhtari)** and **Load Content**. The loader checks image bytes, including
any cartridge header; the extension alone does not establish the format. A refused image reports its reason.
Atari 2600 images are not supported. The core architecture must match the frontend process. Keep **Boot: Fast**
for a first launch; no BIOS is needed. Firmware setup is [below](#bios-files).

### Artifact identity and evidence

The host app and muhtari core are separate artifacts with separate identities. The
[`retroarch-dmg` row in `pins.txt`](../pins.txt) pins RetroArch 1.22.2 Metal for macOS as a universal DMG
(SHA-256 `81b79121ba26d539064ae13b4d0419a120c3d165afbe656cf5f5412b15fdb434`). That pin identifies the app
package only; it is not part of the core's publisher attestation. The [retained published-asset receipt](../.planning/milestones/v3.0-published-artifact-receipt.md)
records the historical macOS arm64 archive, source and extracted-byte identities, attestation scope and
release-run limits. It does not qualify current bytes or gameplay input. Select matching app and core
architectures and verify both immutable identities; an installed filename or moving `latest` URL is not a pin.

### Per-game launch paths

RetroArch owns the game window, audio/input devices and its save/state files. Playstead owns the per-game content
identity and directories, adapter pins and installed-artifact checks, controller/keyboard mappings, focus handoff,
and child-process lifecycle and return. Use one writer per game's save/state paths; do not share them between
concurrent sessions. `muhtari-run` is headless and is not an interactive player. For example, create a local
RetroArch config and pass every path explicitly:

```sh
RETROARCH="/Applications/RetroArch.app/Contents/MacOS/RetroArch"
CORE="/path/to/cores/muhtari_libretro.dylib"
GAME_ROOT="/path/to/playstead/games/game-123"
GAME="$GAME_ROOT/content/game.a78"
mkdir -p "$GAME_ROOT/config" "$GAME_ROOT/content" "$GAME_ROOT/save" \
  "$GAME_ROOT/state" "$GAME_ROOT/firmware" "$GAME_ROOT/logs"
"$RETROARCH" --verbose --config "$GAME_ROOT/config/retroarch.cfg" \
  -L "$CORE" "$GAME" --log-file "$GAME_ROOT/logs/retroarch.log"
```

The caller supplies the verified local content and separately verified executable and core for the same target
architecture. On Linux, use `retroarch` and the `.so` core; on Windows, use `retroarch.exe` and the `.dll` core.
Replace the example root with an absolute local path in the config file; RetroArch does not expand `$GAME_ROOT`
inside config values:

```ini
savefile_directory = "/path/to/playstead/games/game-123/save"
savestate_directory = "/path/to/playstead/games/game-123/state"
sort_savefiles_enable = "false"
system_directory = "/path/to/playstead/shared/firmware"
```

The save directory gives each game's battery file its own location; disabling save-file sorting keeps the normal
content-name convention. The state directory is for RetroArch-owned state files. Setting it does not establish
that save/load, rewind or run-ahead works in the selected frontend. The system directory may be empty when no
firmware is needed. Fast boot is the default; the console BIOS and `highscre.bin` are optional and are never
bundled. See [BIOS files](#bios-files) for names and behavior.

RetroArch maps controllers to the RetroPad layout in [Controls](#controls); the README lists keyboard defaults.
Playstead must provide usable controller mappings and a keyboard fallback, and ensure keyboard focus reaches
RetroArch while the child is active. Use the host's normal Quit/Close flow and wait for child exit. That gives
RetroArch its ordinary teardown and persistence opportunity; a zero exit alone does not prove the battery file
was flushed. The core exposes raw battery memory but does not read or write `.srm` files. A forced kill may lose
recent battery progress; treat persistence as unknown until the frontend file is checked.

### Exit and refusal reporting

Keep these outcomes separate when diagnosing a launch:

- `muhtari-run --probe FILE` identifies a cartridge without launching RetroArch. The runner's malformed-image
  and unsupported-board results are described in [Probing an image](runner.md#probing-an-image); a successful
  probe does not prove that RetroArch started or loaded the core.
- A core refusal appears in the frontend log as `muhtari: content refused: ...`; unsupported sound also names
  `muhtari: unavailable sound configuration: ...`. Use `--verbose --log-file` and keep the log with the
  per-game run record. For example, a YM2151 request is named rather than reported as a generic load failure.
- A RetroArch process that fails to start is a host startup failure. It has no core-load result; inspect the
  process error and host log separately from core refusal messages.
- A normal RetroArch quit and a clean process exit establish that the host ended normally, not that content
  loaded successfully or battery bytes reached disk. A forced kill has no normal teardown guarantee; battery
  continuity is unknown.

RetroArch exit status is not the `muhtari-run` status table. In particular, zero proves neither content loading
nor a flushed save. A 52 KiB BankSet geometry is supported when its other header and memory requirements are
met, but the loader refuses that geometry if High Score Cartridge storage is physically attached because its
ROM occupies the `$3000-$3FFF` window. The core reports `unsupported board` in the content-refusal log in that
case. HSC attaches in Auto mode for an HSC request or effective XM, or when explicitly On; HSC Off suppresses
that attachment. The supported equal-half per-set sizes are 32, 48, 52, 128, 256 and 512 KiB; the 48 and 52
KiB sets require no RAM, and the [cartridge map](hardware/cartridge.md#bankset) documents the complete matrix.
Do not describe BankSet as unsupported in general.

If muhtari is missing from Load Core, check **Settings → Directory → Cores** and **Core Info**, copy the
archive's corresponding files there, then restart RetroArch. When stale core information persists after an
update, close RetroArch and remove only `core_info.cache` from its Core Info directory; it is rebuilt. For a
load failure, use `--verbose --log-file` with the launch command and inspect the `muhtari:` lines. Keep personal
game files, firmware, saves, screenshots and logs outside this repository.

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
0/159/319; height242 maps them to 0/120/241 and PAL height 292 to 0/145/291. Either coordinate -32768 marks the
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
| Region (`muhtari_region`) | Auto (default), NTSC, PAL | when content is loaded; reload required |
| Palette (`muhtari_palette`) | Auto (default), NTSC (warm), PAL (warm) | always; presentation only |
| Left difficulty (`muhtari_left_difficulty`) | B (default), A | at load and Restart; a change sets the switch at once |
| Right difficulty (`muhtari_right_difficulty`) | B (default), A | at load and Restart; a change sets the switch at once |
| High Score Cartridge (`muhtari_hsc`) | Auto (default), On, Off | at content load |
| Expansion (`muhtari_expansion`) | Auto (default), Off, XBoard, XM | at content load; reload required |
| Port 1/2 battery device (`muhtari_battery_port1`, `muhtari_battery_port2`) | Auto (default), None, SaveKey, AtariVox | at content load |

Auto region follows content metadata; an explicit choice overrides it at load.
Firmware names and sizes do not select the console region.
Changing region during a session, including Restart, keeps the loaded timing and
geometry until content is reloaded. No-content operation is always the NTSC test card.
Auto palette follows the effective loaded region; explicit warm palettes only change
colours, preserving native pixels, timing and audio. Missing or unknown option values
use Auto. The existing `ntsc` palette spelling retains warm NTSC.

PAL AV is
320×292, maximum height 293, FPS 3546894/71051 and display aspect 147500000/129461631.
The frontend queries AV after load; load sends no AV/geometry update callback.
All 292 rows and up to 962 mono samples are retained in either pixel format.
Unload, failed load and deinit clear the selected AV/region and output pointers;
queries then report the NTSC fallback. The automatic palette cache also returns to NTSC.
The adapter releases the previous session before loading another image: a failed
load leaves that clean fallback, unlike the library's transactional load behavior.
Frontends requery AV after each successful load and reload content to apply region changes.

Battery options override controller requests supplied before load; Auto follows those requests, then
cartridge metadata. A SaveKey hint without a location selects port 2. Battery None suppresses automatic
storage on its port while retaining an ordinary controller selection; an automatic or save-controller
selection becomes ProLine. At most one EEPROM may attach, on either port; dual requests refuse load.
No controller callback before load is required. A later request that changes storage reports that content
must be reloaded and keeps the installed attachment and its button mapping. Auto and the installed save
identity remain valid;
ordinary non-save controller changes still work. A repeated unchanged request produces no repeated warning.

Expansion Auto follows any nonzero A78 byte63 XM request, otherwise Off; XBoard requires explicit selection.
Missing or unknown values use Auto. HSC Auto provisions storage for an HSC request or effective XM;
HSC Off suppresses it. This physical attachment stays fixed even while XM's CPU windows are hidden.
Firmware discovery uses that same effective attachment before load. Changing effective expansion or HSC
requires reload, including Restart; no live option reinterprets SAVE_RAM. Equivalent explicit and Auto
selections need no warning. Load logs distinguish cartridge requests from effective hardware, preserve
noncanonical XM warnings and name refused sound devices. Off suppresses only expansion: independently
declared YM2151, Covox and ADPCM still fail load with a named log/message.

XM is an implementation-backed RAM/POKEY/HSC subset with medium physical confidence. Internal XM BIOS,
top-slot switching, flash, PIA, flat48KiB mode, write protection, sticky locks, ROF aliases and PAL firmware
selection are unavailable. No full-XM service or gameplay compatibility is claimed.

`RETRO_MEMORY_SAVE_RAM` is the final core-owned raw span immediately after successful content load:
2,048 HSC bytes, 32,768 EEPROM bytes, or HSC followed by EEPROM (34,816 bytes). Absent storage and
unloaded/no-content sessions return NULL/0. The frontend restores bytes before the first frame and owns
all `.srm` file I/O; the adapter has no save-file operations or mirror buffer. Pointer and length remain
stable until unload or reload. Restart retains committed battery bytes; it discards incomplete EEPROM
programming without advancing emulated time.

## Save states

For loaded content, `retro_serialize_size`, `retro_serialize` and `retro_unserialize` use the same canonical
bytes as the library state API. The reported size stays fixed until unload or reload. Serialization accepts a
non-null buffer at least that large and leaves any extra capacity untouched; restore requires a non-null buffer
of exactly that length. Invalid, short or oversized states are rejected without changing the machine. Unloaded,
failed-load and no-content test-card sessions report size zero and refuse save/load callbacks.

The versioned bytes are little-endian and bound to a SHA-256 effective machine identity. Their CRC-32 detects
corruption but does not authenticate a state file; unsupported format or behavior revisions are refused without
migration.

Normal disk states, run-ahead contexts 1 and 2, and an unavailable context query all use the library's fully
validated little-endian format. If the frontend explicitly requests rollback-netplay context 3, the callbacks
report size zero and refuse save/load because that context requires big-endian integers. There is no second
netplay codec.

State bytes include emulated battery contents. Restoring a state rolls those emulated bytes back and updates
the live `RETRO_MEMORY_SAVE_RAM` span in place. The core does not read or write `.srm` files: the frontend
owns raw-save persistence and decides when to flush committed bytes. The frontend also owns state files,
rewind storage, and rewind controls and UI.

The `.info` `savestate = "true"` and `savestate_features = "deterministic"` declarations are tied to the
loaded callback, repeated-continuation, and host-control test registrations. They describe the core's state
contract; they are not evidence that a frontend has exercised it. Actual pinned RetroArch save/load, rewind,
and run-ahead qualification remains pending. In [run 41629846188](https://github.com/szTheory/muhtari/actions/runs/41629846188), the Windows x64 and ARM64 builds and hosted Ubuntu x64 ASan passed, but the macOS Intel independent-parent baseline timed out in `fuzz.regress.run`. The exact artifact join failed and the dependent RetroArch host job was skipped; no actual-host result is claimed.

The later authorized attempt, [run 41733613173](https://github.com/szTheory/muhtari/actions/runs/41733613173), authenticated the pinned source and same-run macOS arm64 artifacts, but its states driver exited before state-mode media preparation and RetroArch launch. It emitted a fail-closed preflight message from a compound check; the exact failing condition is unknown. No state-mode save/load, rewind, run-ahead, picture, progress, or durability observation was collected, so these declarations remain unqualified for an actual frontend.

**Restart** power-cycles the console: volatile memory is cleared, the cartridge starts again the way it was loaded, and the
difficulty switches go back to their options.

If a frame fails, the core reports the error on screen and in the log once, and keeps presenting the last
picture until Restart or a new load. Before the first successful frame, that picture is black. Each failed run
sends actual pixels, including to frontends that do not support NULL-frame duplication, and sends 48 kHz audio
as identical left/right copies of each mono core sample. Batch callbacks take priority; the single-sample
callback is the fallback. Missing callbacks discard output while preserving future sound. Fast boot begins
with 54 NTSC or 960 PAL samples, then full frames provide 800/801 or 961/962 respectively.
Restart and content reload reset synthesis history.

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
declares battery saves, basic savestate callback support and optional HSC firmware. AtariVox speech is
unavailable.

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
HALT-selected cartridge RAM also stays unmapped. With XBoard or XM installed,
all cartridge windows at $4000-$7FFF remain holes for the entire session, even
while expansion RAM is disabled. XM's HSC SRAM and ROM windows remain holes
regardless of their gate. Physical cartridge/expansion RAM and HSC SAVE_RAM
retain their dedicated storage access and lifetime independently of these holes.

BankSet descriptors expose only the CPU ROM half: linear sets end at $FFFF,
and switched sets expose the last bank at $C000 plus the second-to-last at
$4000 when no RAM or expansion overlaps it. Bank counts and offsets use one
set's size. The switched $8000-$BFFF window and MARIA half remain unmapped.
The 52 KiB set also exposes $3000-$3FFF except with XM installed.
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

## WebAssembly build and offline host

The production libretro adapter can also be built as a no-entry ES module and
companion Wasm file. Native `.so`, `.dylib` and `.dll` output and the existing
WASI lane keep their own presets. The required toolchain is the official
`emscripten/emsdk` 7.0.0 image pinned by index digest
`sha256:6bd7aef93408b10bf7b36642cb7c91119b302f720b9ec2d20439f73cf07fe894`;
the Linux x86-64 leaf is
`sha256:3803e37bf748ddee14d31cf712fcab09ca367fca83bbefeb78ce4c1f284e6f3f`.
The amd64 image is about 826 MB compressed. The container's CMake 3.28 is
below this repository's minimum, so the script uses the existing SHA-verified
CMake 4.4.3 pin and Unix Makefiles. Ninja is not required.

After the native `ci` workflow has generated the owned fixture and numeric
observer report, run the complete local lane with:

```sh
sh scripts/emscripten.sh build/ci
```

The container has network access disabled during configure, build and tests.
Outputs stay under `build/emscripten` and `build/ci/tests/emscripten`: the
`libretro/muhtari_libretro.mjs` loader, `libretro/muhtari_libretro.wasm`,
required CTest/JUnit inventory, native and Node observations, measured size
and throughput report, and actual Wasm import/export surface. The existing
Linux x64 CI job runs this lane as a required step after native tests and
uploads those build-owned artifacts. Missing tools or evidence fail the lane.

The generated `.mjs` exports an async factory. A direct Node consumer supplies
the companion bytes, then calls the ordinary libretro exports:

```js
import { readFile } from 'node:fs/promises';
import createCore from './muhtari_libretro.mjs';

const wasmBinary = await readFile(new URL('./muhtari_libretro.wasm', import.meta.url));
const core = await createCore({ wasmBinary });
```

`tests/emscripten/host.mjs` demonstrates the complete callback, content,
state and input lifecycle without network access. Its callback table uses
Emscripten signatures `iii` (environment), `viiii` (video), `vii` (single
sample audio), `iii` (audio batch), `v` (input poll) and `iiiii` (input
state). Its environment replies cover core-options version/options, the
supported option values, XRGB8888, no-game, controller/input descriptors,
memory maps, achievements, messages and normal savestate context; unknown
commands return false.

The wasm32 `retro_game_info` record is 16 bytes: path pointer at byte 0,
content pointer at 4, 32-bit size at 8 and metadata pointer at 12. The caller
copies the image into Wasm memory before `retro_load_game`; this core copies
accepted content before returning, so the caller can then free its input
buffer. Save and restore buffers are likewise caller-owned. Register callback
functions with `addFunction`, and remove them after `retro_deinit`. Copy video
rows using the reported pitch and copy PCM during its callback. Read
`HEAPU8`/`HEAPU32` again after calls that may grow memory; never keep a heap
view as an observation.

The current artifact imports Emscripten's assertion, abort, resize and POSIX
file/syscall helpers plus WASI `fd_close`, `fd_read`, `fd_seek` and `fd_write`.
It exports the `retro_*` entry points, Wasm memory/table and C allocation
functions; Emscripten stack/runtime exports are implementation details. The
build-owned `wasm-surface.json` contains the exact names from each artifact.

The report runs 12 warm-up frames and three measured windows of 60 neutral
frames with the same generated fixture, callback copies and SHA-256 work on
native and Node adapters. It records loader/Wasm/combined uncompressed bytes,
source and fixture hashes, tool/runtime identities, architecture and each
window plus median. It sets no speed or size threshold. Local Node execution
under Docker Desktop amd64 emulation describes that container/host only; it
does not establish browser, mobile, device or physical-input behavior.
