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
| B | button 1 (left) | button 1 |
| A | button 2 (right) | button 2 |
| Select | console Select | - |
| Start | console Pause | - |
| X | console Reset | - |
| L | left difficulty switch | - |
| R | right difficulty switch | - |

L and R flip their switch on each press, and a message shows the new position, such as `Left difficulty: A`.
The console's Reset is the switch a game reads; RetroArch's own Restart is a power cycle (below).

## Devices

Each port offers four devices in Quick Menu → Controls:

| Device | What the game sees |
|---|---|
| Auto (cartridge header) | the controller the cartridge's header names; the default |
| ProLine | the two-button 7800 controller |
| Joystick | a one-button 2600 joystick; A fires too |
| None | nothing plugged in |

## Options

| Option | Values | Applies |
|---|---|---|
| Boot (`muhtari_boot`) | Fast (default), BIOS | when content is loaded |
| Palette (`muhtari_palette`) | NTSC (warm) | always |
| Left difficulty (`muhtari_left_difficulty`) | B (default), A | at load and Restart; a change sets the switch at once |
| Right difficulty (`muhtari_right_difficulty`) | B (default), A | at load and Restart; a change sets the switch at once |

There is no region option: this release runs NTSC only.

**Restart** power-cycles the console: memory is cleared, the cartridge starts again the way it was loaded, and the
difficulty switches go back to their options.

If a frame fails, the core reports the error on screen and in the log once, and keeps presenting the last
picture until Restart or a new load.

## BIOS files

A BIOS is optional. With Boot set to BIOS, the core looks in the frontend's system directory for

- `7800 BIOS (U).rom`, the NTSC BIOS, 4,096 bytes, then
- `7800 BIOS (E).rom`, the PAL BIOS, 16,384 bytes,

and boots through the first usable file, showing the BIOS's own start-up screen before the game. The log names
the file and its SHA-256, so a user can tell which image ran. Any valid 4 KiB or 16 KiB image is accepted, such
as OpenBIOS. An invalid or unreadable file is named in the log and the next name is tried. With no usable file,
the core boots fast and says so on screen and in the log. An allocation failure reports an error and refuses the
load. Fast boot starts the cartridge where the console's BIOS hands over to it, with no firmware at all.

## Achievements

The core declares a memory map (`SET_MEMORY_MAPS`) of the CPU's 64 KiB address space, which is how rcheevos'
Atari 7800 console (number 51) addresses memory, and tells the frontend that it supports achievements.

| Addresses | Memory |
|---|---|
| $0040-$00FF | system RAM at $2040-$20FF, which the console also shows here |
| $0140-$01FF | system RAM at $2140-$21FF, likewise |
| $0480-$04FF | the RIOT's 128 bytes of RAM |
| $1800-$27FF | system RAM, 4 KiB, flagged as system RAM |
| $2800-$2FFF | system RAM's second half, $2000-$27FF, again |
| $4000-$7FFF | cartridge RAM, when the board has unbanked RAM |
| ROM | the fixed ROM, read-only: a linear cartridge's whole ROM; a SuperGame cartridge's last bank at $C000 and, without RAM, its fixed bank at $4000 |

Everything else has no memory behind it: the TIA, MARIA and RIOT registers, $3000-$3FFF, banked windows and the
rest of the address space. The map lists those spans too, as descriptors without a pointer, because rcheevos stops
mapping a region at the first address no descriptor covers: without them, $0040 and $C000 would be out of its
reach.

**Why $2800-$2FFF is mapped.** rcheevos' table for the 7800 calls $2800-$2FFF mirrored RAM, and achievement sets
written on other cores may read it. On the console, whether RAM answers there is not settled, and muhtari's bus
leaves the range to the cartridge. The map follows rcheevos, so those achievements read the RAM they expect.

**Why $3000-$3FFF is not.** rcheevos calls this range mirrored RAM too, but the 7800 Software Guide gives it to
external devices, not RAM, so a byte read there would be invented. It stays without memory.

Banked RAM and banked ROM windows are left out because a descriptor cannot say which bank is showing.
