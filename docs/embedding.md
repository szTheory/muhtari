# Embedding the library

The library is one static archive and one header, `muhtari.h`, written in C17. It allocates memory only through
the allocator you pass, keeps all state in the instance, starts no threads, opens no files and uses no floating
point. It needs nothing from the C library beyond `memcpy`, `memmove`, `memset` and `memcmp`.

## Getting it

**Release archive.** `muhtari-library-<os>-<arch>.zip` from the
[latest release](https://github.com/szTheory/muhtari/releases/latest) holds:

```text
include/muhtari.h
lib/libmuhtari.a                 (lib/muhtari.lib on Windows)
lib/cmake/muhtari/               CMake package, target muhtari::muhtari
lib/pkgconfig/muhtari.pc
LICENSE
THIRD-PARTY-NOTICES.md
```

Unzip it anywhere; both package files find the other files relative to themselves. On Windows the library is
built against the static C runtime (`/MT`) and records no default runtime library, so it links into programs on
either runtime.

**CMake package.**

```cmake
find_package(muhtari CONFIG REQUIRED)
target_link_libraries(app PRIVATE muhtari::muhtari)
```

Configure with `-DCMAKE_PREFIX_PATH=<unzipped directory>`. Until version 1.0 a minor release may change the API,
so the package accepts a request for the same major and minor version only.

**Source tree.** `add_subdirectory` or `FetchContent` gives the same `muhtari::muhtari` target. Only the C
compiler is needed: an embedding build gets the library alone, without the runner, the libretro core, the tests
or this project's warning flags.

```cmake
add_subdirectory(third_party/muhtari)
target_link_libraries(app PRIVATE muhtari::muhtari)
```

**pkg-config.**

```sh
PKG_CONFIG_PATH=<unzipped directory>/lib/pkgconfig cc app.c $(pkg-config --cflags --libs muhtari)
```

## Expansion selection

API 7 retains ABI 1 and behavior revision 6. The appended `muhtari_media.expansion`
selects one of `MUHTARI_EXPANSION_*`: `MUHTARI_EXPANSION_AUTO`, `MUHTARI_EXPANSION_OFF`, `MUHTARI_EXPANSION_XBOARD`
or `MUHTARI_EXPANSION_XM`. Zero, omitted and incomplete fields mean Auto. Explicit
selection wins; Auto uses the resolved cartridge XM request, otherwise Off.
`muhtari_info.expansion` reports the committed effective selection. Failed loads
retain the previous selection, storage, firmware and execution; power cycle retains
the selection and battery storage while resetting volatile controls and RAM.

XBoard provides eight 16 KiB RAM banks and one continuously installed POKEY at
$0450, whose register decode starts disabled. A declared $0450 denotes that same
chip; a secondary $0440 can coexist. Expansion with a separate $4000/$0800 primary,
conflicting primary owners, YM2151, Covox or ADPCM returns
`MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE` before allocation. Probe remains descriptive,
including `MUHTARI_SOUND_ADPCM` for A78 v4 audio bit $20. Selecting Off suppresses
the expansion request without suppressing independently declared unsupported audio.
XM Auto provisions HSC unless explicitly disabled; malformed requested firmware
and a 52 KiB BankSet half with effective HSC refuse before replacement.

## Structs and sizes

Every struct you pass starts with `uint32_t size`. Zero the whole struct, then set `size`:

```c
muhtari_config cfg;
memset(&cfg, 0, sizeof cfg);
cfg.size = (uint32_t)sizeof cfg;
```

There is no init macro. The size tag lets a program built against one header run with a library from another:

- `size` promises that many readable bytes at the struct's address; the library may read all of them. A wrong
  tag is undefined behaviour, so always set it from `sizeof`.
- A size below the struct's first released size, or above 4096 bytes, returns `MUHTARI_STATUS_ERR_STRUCT_SIZE`.
  The cap only bounds how far a wrong tag can make the library read.
- A smaller size from an older header is accepted. Input fields it lacks take their zero default, and an output
  struct receives only its first `size` bytes.
- A larger size from a newer header is accepted only if every byte the library does not know is zero; otherwise
  the call returns `MUHTARI_STATUS_ERR_STRUCT_SIZE`.
- A field added later always keeps the old behaviour when it is zero.

## The allocator

`muhtari_config.allocator`, a `muhtari_allocator`, needs both `alloc` and `free`; either one missing returns
`MUHTARI_STATUS_ERR_ARGUMENT`. There is no default. `alloc` must return memory aligned for `max_align_t`, as
`malloc` does. A host with a C library can pass this wrapper:

```c
static void *host_alloc(void *user, size_t size) {
    (void)user; return malloc(size);
}
static void host_free(void *user, void *ptr, size_t size) {
    (void)user; (void)size; free(ptr);
}
```

`free` receives the size that was allocated, so a fixed arena or pool needs no header of its own. `user` is
passed back unchanged. `muhtari_create` allocates the instance and `muhtari_load` the copy of the cartridge's
ROM together with its RAM; each returns `MUHTARI_STATUS_ERR_NO_MEMORY` when `alloc` returns NULL. Running frames allocates nothing.

## A frame

```c
/* No-content NTSC example. After loading content, requery info and resize buffers. */
muhtari *m = NULL;
cfg.allocator.alloc = host_alloc;
cfg.allocator.free = host_free;
if (muhtari_create(&cfg, &m) != MUHTARI_STATUS_OK) { /* handle the error */ }

muhtari_frame io;
memset(&io, 0, sizeof io);
io.size = (uint32_t)sizeof io;
io.video = video;               /* 320 x 242 bytes, one colour number per pixel */
io.video_pitch = 320;           /* bytes per row, 320..65536 */
io.audio = audio;               /* room for 801 samples */
io.audio_capacity = 801;
muhtari_status st = muhtari_run_frame(m, &io);   /* io.audio_count: 800 or 801 */
muhtari_destroy(m);
```

`muhtari_get_info` fills a `muhtari_info` with the picture size, the frame rate, the pixel aspect ratio, the
sample rate and the largest audio count per frame, so a host need not hard-code them. A NULL `video` or `audio`
skips that output. With `video` set, a `video_pitch` outside 320..65536 returns `MUHTARI_STATUS_ERR_ARGUMENT`;
with `audio` set, an `audio_capacity` below the frame's actual sample count returns
`MUHTARI_STATUS_ERR_BUFFER_TOO_SMALL`, so size the buffer from `max_audio_samples`. `muhtari_run_frame` checks its
arguments before it changes anything. Once `m` and `io` are not NULL and the size tag is accepted, it writes
`audio_count` on every return, zero on an error.

Requery info after each successful load. PAL reports 320×292, FPS 3546894/71051,
PAR 1843750/1773447 and at most 962 samples; NTSC and no-content defaults stay
unchanged. Packed PAL video needs 93440 bytes. With padding the accessible extent
is `(height-1)*pitch+width`; the frame struct has no allocation-byte count and
cannot detect an undersized allocation. PAL fast boot returns 960 first-frame
samples, then 961 or 962 per full frame, with fractional carry retained.
Pitch validation precedes audio-capacity validation, and both precede input latching
and execution. NULL outputs still advance identical video/audio state. A refused
library load retains the prior effective region and running cartridge; the libretro
adapter instead releases its old session before loading and falls back to NTSC on failure.

PAL fast handoff at tick 140016 is selected compatibility policy, not a measured
production BIOS snapshot. The warm PAL table is a licensed presentation seed;
native hashes do not depend on it. Region is fixed at successful load and survives
power cycle; libretro region changes require reload, while palette changes affect
presentation immediately. See [PAL diagnostic evidence](pal-tests.md) and
[audio model limits](hardware/audio.md). API 6, ABI 1 and behavior revision 5
retain the append-only status and size-tag contracts; legacy API 3 callers can
now load PAL, so hosts must query geometry rather than depend on its former refusal.

## Probing a cartridge image

`muhtari_probe(image, n, &info)` reads the `n` bytes at `image` and fills a `muhtari_cart_info` with what the
cartridge is: an A78 file, a headerless 7800 image or a 2600 image. It is pure: it allocates nothing, keeps no
state, and gives the same output for the same bytes, so concurrent calls on different outputs cannot interact.
It reads at most `MUHTARI_IMAGE_SIZE_MAX` bytes (1 MiB of ROM plus a 128-byte header) and hashes them a fixed
number of times. `muhtari_load` runs an image (see [Loading content](#loading-content)).

```c
muhtari_cart_info info;
memset(&info, 0, sizeof info);
info.size = (uint32_t)sizeof info;
muhtari_status st = muhtari_probe(image, n, &info);
```

It checks, in this order:

1. `info` is NULL, or `image` is NULL with `n` above 0: `MUHTARI_STATUS_ERR_ARGUMENT`.
2. A bad size tag: `MUHTARI_STATUS_ERR_STRUCT_SIZE`.
3. `n` above `MUHTARI_IMAGE_SIZE_MAX`: `MUHTARI_STATUS_ERR_UNSUPPORTED_SIZE`, before any byte is hashed.
4. The image itself, as `docs/hardware/cartridge.md` describes: the A78 magic, the 7800 list, the 2600 list,
   the footer test and the size. A NULL `image` with `n` = 0 is `MUHTARI_STATUS_ERR_INVALID_IMAGE`.

| Status | Meaning | `info` |
|---|---|---|
| `MUHTARI_STATUS_OK` | A 7800 cartridge on a board this library runs | written |
| `MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD` | A 7800 cartridge on a board it does not run yet | written |
| `MUHTARI_STATUS_ERR_UNSUPPORTED_2600` | A 2600 image | written: only `kind`, `source`, `rom_size` and `flags` are non-zero |
| `MUHTARI_STATUS_ERR_INVALID_IMAGE` | Not a cartridge image, or a size no board has | untouched |
| `MUHTARI_STATUS_ERR_TRUNCATED_IMAGE` | An A78 file that ends early | untouched |
| `MUHTARI_STATUS_ERR_UNSUPPORTED_SIZE` | Larger than `MUHTARI_IMAGE_SIZE_MAX` | untouched |

The two refusals that write `info` are the one exception to the rule that an error writes nothing: a host can
show what it refused.

| Field | Values |
|---|---|
| `kind` | `MUHTARI_KIND_7800`, `MUHTARI_KIND_2600` |
| `source` | What identified it: `MUHTARI_SOURCE_HEADER`, `MUHTARI_SOURCE_HASH`, `MUHTARI_SOURCE_FOOTER`, `MUHTARI_SOURCE_SIZE` |
| `flags` | `MUHTARI_CART_HEADER` (it has an A78 header), `MUHTARI_CART_HASH_MATCH` (a list entry matched), `MUHTARI_CART_ACTIVISION_HALF_SWAPPED` (the identified Activision input has swapped halves) |
| `warnings` | Header defects it accepted, listed below |
| `rom_size` | Bytes the cartridge maps, header excluded |
| `board` | `MUHTARI_BOARD_LINEAR`, `MUHTARI_BOARD_SUPERGAME`, `MUHTARI_BOARD_ACTIVISION`, `MUHTARI_BOARD_ABSOLUTE`, `MUHTARI_BOARD_FRACTALUS`, `MUHTARI_BOARD_BANKSET`, `MUHTARI_BOARD_SOUPER` |
| `board_options` | Bits: `MUHTARI_BOARD_ROM_AT_4000`, `MUHTARI_BOARD_BANK6_AT_4000`, `MUHTARI_BOARD_BANKED_RAM`, `MUHTARI_BOARD_HALT_BANKED_RAM` |
| `ram_size` | Cartridge RAM in bytes |
| `sound` | Bits: `MUHTARI_SOUND_POKEY_4000`, `MUHTARI_SOUND_POKEY_450`, `MUHTARI_SOUND_POKEY_440`, `MUHTARI_SOUND_POKEY_800`, `MUHTARI_SOUND_YM2151`, `MUHTARI_SOUND_COVOX` |
| `devices` | Bits: `MUHTARI_DEVICE_HSC`, `MUHTARI_DEVICE_SAVEKEY`, `MUHTARI_DEVICE_XM` |
| `port1`, `port2` | `MUHTARI_PORT_NONE`, `MUHTARI_PORT_PROLINE`, `MUHTARI_PORT_LIGHTGUN`, `MUHTARI_PORT_PADDLE`, `MUHTARI_PORT_TRAKBALL`, `MUHTARI_PORT_JOYSTICK`, `MUHTARI_PORT_DRIVING`, `MUHTARI_PORT_KEYPAD`, `MUHTARI_PORT_ST_MOUSE`, `MUHTARI_PORT_AMIGA_MOUSE`, `MUHTARI_PORT_ATARIVOX`, `MUHTARI_PORT_SNES2ATARI`, `MUHTARI_PORT_MEGA7800`: the A78 codes 0 to 12 |
| `region` | `MUHTARI_REGION_NTSC`, `MUHTARI_REGION_PAL` |
| `a78_version`, `a78_rom_size`, `a78_cart_type`, `a78_port1`, `a78_port2`, `a78_tv`, `a78_save`, `a78_xm` | The A78 header's fields as written; zero without a header |
| `a78_mapper`, `a78_mapper_options`, `a78_audio`, `a78_irq` | The version 4 block as written; zero below version 4 |
| `title` | The header's title: at most 32 printable ASCII bytes, trailing spaces dropped, NUL-terminated and zero-padded |

Zero means absent in every field. `MUHTARI_PORT_UNPLUGGED` (256) sits outside the A78 byte range: probing never
reports it, and a host passes it for an empty controller port. A board moves from `MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD` to
`MUHTARI_STATUS_OK` in the release that runs it. The warning bits:

| Bit | Meaning |
|---|---|
| `MUHTARI_CART_WARN_HEADER_V4_IGNORED` | says version 4 or above, but its version 4 block is invalid or its board does not fit the ROM, so the cart type was used |
| `MUHTARI_CART_WARN_HEADER_CORRECTED` | names a board, RAM or sound that a list entry for the same bytes corrects |
| `MUHTARI_CART_WARN_VERSION_ABOVE_4` | gives a version above 4, read as 4 |
| `MUHTARI_CART_WARN_SIZE_FIELD_MISMATCH` | gives a ROM size other than the bytes after it |
| `MUHTARI_CART_WARN_TITLE_NONPRINTABLE` | has a title byte outside printable ASCII, shown as `?` |
| `MUHTARI_CART_WARN_UNKNOWN_PORT` | names a controller code above 12, read as `MUHTARI_PORT_NONE` |
| `MUHTARI_CART_WARN_LEGACY_SAVE_BIT` | marks the High Score Cartridge with the old bit 2 |
| `MUHTARI_CART_WARN_XM_VALUE` | gives an XM byte other than 0 or 1 |
| `MUHTARI_CART_WARN_RESERVED_NONZERO` | has a reserved byte or bit set |
| `MUHTARI_CART_WARN_CART_TYPE_CONFLICT` | sets cart-type bits that cannot all hold, resolved as `docs/hardware/cartridge.md` says |
| `MUHTARI_CART_WARN_ACTIVISION_LAYOUT_ASSUMED` | an unmatched image explicitly declares Activision, so its layout is assumed to be OM order |

API 4 adds the Activision orientation and assumed-layout bits without changing
the 140-byte `muhtari_cart_info`. `activision_half_swapped` is a descriptive probe
flag: only a hash match authorizes interpreting the supplied image as half-swapped
AM order. `activision_layout_assumed` is a warning for an unknown image that
explicitly declares Activision; it records an OM assumption, not detected layout.
These are the runner's lowercase names for the two bits. The loader normalizes
only an authorized hash match into its private OM copy; it never modifies caller bytes.

The supplied full-image and headerless hashes keep their existing meanings,
including trailer handling. Normalizing a private loaded copy does not replace
supplied-image, movie or RetroAchievements identity. An unmatched modified AM
image must be converted externally to OM order. There is no layout override in
the API or runner, and an unknown headerless image's size alone does not identify
an Activision board.

## Loading content

The executable retail geometries are exactly 128 KiB for Activision, 64 KiB for
Absolute, and 32 KiB ROM plus 2 KiB physical volatile RAM for Fractalus. Linear/A8
with 32 KiB ROM denotes Fractalus; SuperGame/A8 and BankSet/A8 remain unsupported.
Validation precedes allocation. A refused load or allocation failure preserves the
previous cartridge, its deep ROM/RAM state and subsequent execution. The same
instance-owned maps serve CPU reads, peeks and MARIA; no frame or power cycle allocates.
See [cartridge behavior](hardware/cartridge.md) for exact selector rules, aliases,
selected uncertain edge policies and the separate limits of host memory descriptors.

```c
muhtari_media media;
memset(&media, 0, sizeof media);
media.size = (uint32_t)sizeof media;
media.image = image;            /* the cartridge file's bytes, copied */
media.image_size = n;
muhtari_status st = muhtari_load(m, &media);
```

`muhtari_load` validates everything before it applies anything, in this order:

1. `m` or `media` is NULL: `MUHTARI_STATUS_ERR_ARGUMENT`.
2. A bad size tag: `MUHTARI_STATUS_ERR_STRUCT_SIZE`.
3. `bios` is NULL with a non-zero `bios_size`, or `region` is neither 0 nor a `MUHTARI_REGION_*` value:
   `MUHTARI_STATUS_ERR_ARGUMENT`.
4. The image, exactly as `muhtari_probe` judges it, with the same statuses. An empty image is
   `MUHTARI_STATUS_ERR_INVALID_IMAGE`, and a NULL image with a size is `MUHTARI_STATUS_ERR_ARGUMENT`.
5. The region: `region` when it is not 0, else the cartridge's, which comes from its header, then the hash list,
   then NTSC. Both NTSC and PAL execute; the selection commits only after successful
   cartridge installation. Probe metadata remains descriptive when overridden.
6. The BIOS, when `bios` is not NULL: it must be 4,096 or 16,384 bytes, and its reset vector, mapped to end at
   $FFFF, must point inside it. Otherwise `MUHTARI_STATUS_ERR_INVALID_BIOS`.
7. HSC firmware and storage placement, described below. Invalid HSC media returns
   `MUHTARI_STATUS_ERR_INVALID_HSC`; two EEPROM attachments return
   `MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE`. No rejected candidate replaces live storage.

A refused load changes nothing: the previous cartridge, or the test card, keeps running as if the call had not
been made. Once every check passes, the ROM is copied into one allocation, the only one a load makes, and
any cartridge loaded before is freed. BIOS input may borrow an installed cartridge RAM, battery or HSC ROM
span: its validated bytes are staged before installation can invalidate that span. A private fixed 16 KiB
staging array keeps the running BIOS intact if allocation fails; successful loads copy it to the live BIOS
array inside the instance. Without a
BIOS the machine starts from the fast-boot hand-off, the state the NTSC BIOS leaves as it jumps to the cartridge;
with one it boots through the BIOS, as [Booting with a BIOS](#booting-with-a-bios) describes. A successful load
replaces any loaded content and power-cycles the machine.

`muhtari_power_cycle(m)` runs the same power-on and boot again for the content already loaded; with nothing
loaded it returns `MUHTARI_STATUS_OK` and the test card goes on. There is no unload: `muhtari_destroy` and
`muhtari_create` return to the test card. `muhtari_get_info` describes the loaded content, so read it after a
load.

## Battery storage and HSC firmware

API 5 appends `hsc_mode`, `hsc_rom`, `hsc_rom_size` and `port[2]` to `muhtari_media`.
Old media objects keep their original minimum size; missing or incomplete appended fields
take their zero defaults. `MUHTARI_HSC_AUTO` uses resolved cartridge metadata
and provisions HSC when the effective expansion is XM;
`MUHTARI_HSC_ON` forces attachment and `MUHTARI_HSC_OFF` disables it. A supplied ROM
implies attachment and conflicts with OFF. Both pointer/length directions must agree,
and firmware is exactly 4,096 bytes. HSC firmware has no BIOS reset-vector requirement.

The load copies firmware before replacing installed state, including caller buffers that
alias installed ROM. `muhtari_get_battery` reports its SHA-1 in `hsc_sha1` and flags:
`MUHTARI_HSC_FIRMWARE_KNOWN` identifies the canonical image; an explicit unknown image
is accepted with `MUHTARI_HSC_FIRMWARE_WARN_UNKNOWN`. Missing C6/FE signature bytes at
offsets $900/$904 independently set `MUHTARI_HSC_FIRMWARE_WARN_SIGNATURE`.
Without a supplied image, `MUHTARI_HSC_FIRMWARE_STUB` describes an FF-filled ROM with
only those two signature bytes. This stub implements no firmware routines, formatting,
screens or full OpenHSC compatibility. It supports software that accesses SRAM directly.
See [HSC hardware](hardware/cartridge.md#hsc).

`port[0]` and `port[1]` are load-time device overrides: zero is automatic;
`MUHTARI_PORT_UNPLUGGED` explicitly suppresses a device. An explicit override wins over
the cartridge's controller declaration. `MUHTARI_PORT_SAVEKEY` or `MUHTARI_PORT_ATARIVOX`
reserves one EEPROM on that physical port. Otherwise a SaveKey metadata hint defaults
to port 2, unless an explicit non-save selection suppresses it. Dual EEPROM attachment
is refused, while HSC and one EEPROM may coexist. Placement is fixed at load; serial
protocol behavior is specified separately in the controller hardware documentation.

Zero and size-tag a `muhtari_battery_info` and call `muhtari_get_battery(m, &battery)`.
Layout version 1 is exactly these raw bytes, with no header or host-endian fields:

| Attachment | `bytes` | Order |
|---|---:|---|
| Absent | 0 | `data` is NULL |
| HSC | 2,048 | SRAM |
| EEPROM | 32,768 | EEPROM |
| Both | 34,816 | HSC SRAM followed by EEPROM |

The pointer, length, layout and placement remain stable through frames and power cycles,
until the next successful load or destroy. Firmware is outside the save span.
XM starts with both HSC CPU windows hidden. Restore through this same physical
span before execution; $0470 bit 3 only changes visibility. Gate stores never
move or clear storage, change firmware identity, or increment generation. Hidden
SRAM stores are ignored. Explicit HSC Off suppresses attachment even with XM.
`muhtari_get_hsc_rom` fills a size-tagged `muhtari_const_span` with the installed read-only
ROM/stub, or NULL/0 when HSC is absent. Both queries are bounded observers: they allocate
nothing, scan no battery bytes and advance no emulated time.

After a successful load, restore host save bytes through `data` before the first frame.
Boot setup executes no cartridge instructions before this import window. Imported bytes
are the clean baseline; arbitrary host mutation after this window is outside the contract.
`generation` changes only when emulated committed bytes change: reads and same-value HSC
stores leave it unchanged. It is a uint64 equality token with defined unsigned wrap;
compare for inequality, never ordering or elapsed writes. Power cycle retains bytes and
generation. Successful load starts FF-filled storage with generation zero; refused load
preserves bytes, pointers, topology, generation and subsequent execution. Hosts own files.
Power cycle retains effective expansion/HSC attachment and committed EEPROM
bytes, but clears volatile expansion RAM, controls/pages and POKEY state.
The console Reset input is a physical switch and does not power cycle them.

EEPROM page data becomes persistent only after completed programming in emulated time.
Buffered or aborted serial traffic is outside the save span, and same-value programming
is busy without changing the token. Reset settles work already due, discards future
programming and retains committed data. Neither a getter nor a host flush advances time
to finish a write; see [serial EEPROM](hardware/controllers.md#serial-eeprom).

Per-frame input may select automatic or the already attached save device. Adding,
removing or moving an EEPROM requires another successful load; a refused input leaves
the pending joystick state unchanged. Use these load-time choices before importing:

| Choice | Result |
|---|---|
| Explicit save device on one port | That port owns the EEPROM |
| Automatic port with a save-device declaration | The declared port owns it |
| Save hint without a location | Port 2 owns it |
| Explicit non-save device on the hinted port | The hint is suppressed |
| Two save devices after applying overrides | Load fails transactionally |

Every layout uses the same instance-owned array; no second save copy is synchronized
by a query. Frames, serial traffic, reset and observers allocate no memory after load.

## The first frame after load

A load without a BIOS leaves the NTSC machine at the fast-boot hand-off, 8,190 ticks before VBLANK start, not at a frame
boundary. The first `muhtari_run_frame` after a load or power cycle runs only those 8,190 ticks, so it returns 54
audio samples rather than 800 or 801. Its picture shows only the rows the beam scanned since the hand-off; every
other row is written $00. Every later frame runs from one VBLANK start to the next, a whole frame of 119,402 ticks.

PAL selects the fitted [startup profile](hardware/bus.md#pal-startup) at tick 140016,
first future deadline 281976: 141960 elapsed ticks and 960 samples. Later frames span
142102 ticks and return 961/962 samples. Power cycle retains the successful region;
failed loads retain region, content, battery and pending input. All 292 partial-frame
rows clear before rendering. This fit is not a measured PAL BIOS snapshot.

Loaded cartridges produce mono TIA and declared cartridge POKEY audio through the integer reconstruction and DC blocker. The sample grid
starts at the fast-boot hand-off with zero fractional remainder. TIA's hidden zero-start logic has already run
on its absolute 456-tick lattice from power-on; its volume remains zero until the cartridge writes it. Actual
CPU cycle counting starts at zero at the hand-off tick (NTSC line 240,t24; PAL line 308,t184), which is also the cartridge-audio clock
segment's tick origin. Fast boot invents no skipped CPU cycles. These deterministic startup values are
compatibility defaults, not measured physical power-on or skipped-BIOS residue.

Each frame returns only samples in its logical half-open interval. CPU instructions and interrupt entries
still complete across the deadline; later samples and reconstruction history carry into the next frame.
An event exactly at the deadline belongs to the next interval. A NULL audio buffer discards the same completed
samples while advancing all sound state. Frame PCM is staged inside the instance and copied only on success;
a BIOS refusal returns zero samples and leaves the caller's audio buffer untouched. Ordinary validation errors
execute nothing; a BIOS refusal follows partial execution and does not roll that execution back. Power cycle
resets chip logic, synthesis, pending samples, blocker history and the fractional remainder.
The console Reset input is a software-readable switch: holding it does not reset sound chips or
reconstruction history. Cartridge software decides how to react to that switch.

## Booting with a BIOS

With `bios` set, a load or power cycle starts the console from power-on: INPTCTRL `$00`, so the BIOS is mapped and
the 2600 decode applies; MARIA's registers 0, so CTRL is `$00` with DMA off; RAM and the RIOT at power-on; and the
beam at line 0, t 0. The CPU runs RES, which takes PC from the BIOS's reset vector with S = `$FD` and I set. The first
frame runs to the first VBLANK start, 117,174 ticks and 785 samples, and the input set before it applies from
power-on. Every later frame is whole. The BIOS then runs as on a console: it tests the machine, checks the
cartridge, shows its splash screen and hands off.

Under BIOS boot both the sound sample grid and the actual cartridge-audio clock segment start at tick zero,
CPU cycle zero. The reset and BIOS cycles advance that real timeline; cartridge hand-off does not reset it.

The hand-off frame is the frame, counted from 1 after the load or power cycle, of the first opcode fetch from
cartridge space after INPTCTRL's EXT bit goes from 0 to 1. Only that first fetch counts; a game that maps the BIOS
back in and returns does not move it. The fast boot hands off in frame 1. OpenBIOS hands `simple` off in frame 323,
after its splash; holding Pause through its first check skips the splash.

When the BIOS refuses the cartridge as a 7800 one and takes its 2600 path, it locks INPTCTRL with MEN = 0. That
frame stops there, and it and every later `muhtari_run_frame` return `MUHTARI_STATUS_ERR_BIOS_REJECTED_CART` with
no audio samples, until the next `muhtari_load` or `muhtari_power_cycle`. This version runs no 2600 cartridge.

The core does not hash the BIOS or report its digest; a host that records which BIOS ran hashes the bytes it
passed. The hand-off frame is not part of the public API either; the runner reports it (`docs/runner.md`).

## Input

```c
muhtari_input in;
memset(&in, 0, sizeof in);
in.size = (uint32_t)sizeof in;
in.buttons[0] = MUHTARI_BUTTON_UP | MUHTARI_BUTTON_1;
in.switches = MUHTARI_SWITCH_LEFT_DIFFICULTY_A;
muhtari_status st = muhtari_set_input(m, &in);
```

`muhtari_set_input` sets what both controller ports and the console's switches hold. Call it before each
`muhtari_run_frame`; the input stays until the next call.

- **Devices.** `device[0]` and `device[1]` are 0 for the cartridge's choice, or `MUHTARI_PORT_PROLINE`,
  `MUHTARI_PORT_JOYSTICK` (a 2600 one-button stick) or `MUHTARI_PORT_UNPLUGGED`. The cartridge's choice comes
  from its header: a port the header leaves empty is unplugged, and a device this version does not model, such
  as a light gun or paddles, is wired as a ProLine on this legacy path. Those frozen semantics remain even
  when the extended API models the device. An explicit device outside the legacy supported set returns
  `MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE`, and
  any other value `MUHTARI_STATUS_ERR_ARGUMENT`.
- **Buttons and switches.** A set bit is pressed, or held. `buttons[]` takes the `MUHTARI_BUTTON_*` bits:
  `MUHTARI_BUTTON_UP`, `MUHTARI_BUTTON_DOWN`, `MUHTARI_BUTTON_LEFT`, `MUHTARI_BUTTON_RIGHT`, `MUHTARI_BUTTON_1`
  and `MUHTARI_BUTTON_2`. `switches` takes the `MUHTARI_SWITCH_*` bits: `MUHTARI_SWITCH_RESET`,
  `MUHTARI_SWITCH_SELECT` and `MUHTARI_SWITCH_PAUSE` held, and the difficulty switches as positions,
  `MUHTARI_SWITCH_LEFT_DIFFICULTY_A` and `MUHTARI_SWITCH_RIGHT_DIFFICULTY_A` set for A and clear for B. Any other
  bit returns `MUHTARI_STATUS_ERR_ARGUMENT`. Opposing directions, such as up and down together, pass through to
  the pins as pressed, as a worn stick can press them. `docs/hardware/controllers.md` gives the register bit each
  one reaches.
- **Timing.** The input is latched and reaches the machine at the next VBLANK start, which is where every frame
  but the first after a load begins, so one frame sees one input. Two calls before that apply only the second.
  A refused call changes nothing, and the pending input is not machine state.

### Extended packets

`muhtari_set_input_ex(m, &in)` accepts a separate `muhtari_input_ex`; the legacy
24-byte `muhtari_input`, masks and unsupported-device refusals are unchanged.
The extended packet is 88 bytes: `size` at byte 0, two 40-byte
`muhtari_port_input` blocks at byte 4, and console `switches` at byte 84.
Within each port the scalar offsets are:

| Offset | Field | Units / meaning |
|---|---|---|
| 0 | `device` | Explicit device, or zero for Auto |
| 4 | `buttons` | Existing direction and primary button bits |
| 8, 12 | `paddle[0]`, `paddle[1]` | 0..65535, increasing resistance, independent of polling |
| 16 | `keypad` | Low 12 bits, row-major keys |
| 20, 24 | `dx`, `dy` | Signed 32-bit relative steps; +X right, +Y down |
| 28, 32 | `gun_x`, `gun_y` | Native image coordinates, zero-based |
| 36 | `flags` | `MUHTARI_INPUT_GUN_OFFSCREEN` separates off-screen aim from trigger |

The extended path supports ProLine, joystick, XG-1, paddles, keypad, driving,
CX22, ST/Amiga mice, SNES2Atari, Mega7800, unplugged and the already attached
EEPROM on either connector; unknown IDs return `ERR_ARGUMENT`. Digital devices require all
axes, keys, aim and flags zero; unplugged/EEPROM require zero buttons too. Driving
accepts only button 1 and `dx`: positive is clockwise, one unit per pin transition,
with the bounded cadence described in [Controllers](hardware/controllers.md#driving). New
adapter bits 6..11 are reserved for their named device: SNES Y/X/L/R/Select/Start
and Mega C/X/Y/Z/Mode/Start, with primary buttons B/A:

| Bit | SNES | Mega |
|---|---|---|
| 6 | `MUHTARI_SNES_BUTTON_Y` | `MUHTARI_MEGA_BUTTON_C` |
| 7 | `MUHTARI_SNES_BUTTON_X` | `MUHTARI_MEGA_BUTTON_X` |
| 8 | `MUHTARI_SNES_BUTTON_L` | `MUHTARI_MEGA_BUTTON_Y` |
| 9 | `MUHTARI_SNES_BUTTON_R` | `MUHTARI_MEGA_BUTTON_Z` |
| 10 | `MUHTARI_SNES_BUTTON_SELECT` | `MUHTARI_MEGA_BUTTON_MODE` |
| 11 | `MUHTARI_SNES_BUTTON_START` | `MUHTARI_MEGA_BUTTON_START` |

These names never broaden the
legacy mask. Gun trigger uses primary button 1; paddles use primary buttons 1/2.
Keypad uses its twelve-bit mask; CX22 shares button1, while ST/Amiga use
buttons1/2. Gun aim, pot travel, motion cadence, keypad recovery/contention
and adapter corner behavior follow the named unmeasured policies in
[controller hardware](hardware/controllers.md). Numeric diagnostic conformance
does not establish physical calibration or universal retail compatibility.

Known prefixes may end at any complete 4-byte scalar boundary, from size 4
through 88. Missing fields are neutral zero; a missing complete gun aim pair
means off-screen when that model is supported. Complete zero aim means (0,0).
Larger packets may be at most 4096 bytes and must have zero unknown tail bytes.
Both ports, all applicable ranges, masks and fields are validated before any
pending state changes. Unknown or nonzero inapplicable fields refuse the entire
packet. The latest accepted packet replaces the one pending packet at VBLANK;
held state persists without resubmission. Relative contribution transfers once
at the latch, so frames without a submission never replay a delta.

Extended Auto resolves the successful load's explicit port override, then the
descriptive header. A per-packet explicit choice wins over both; later Auto
restores the remembered load choice. Descriptive cartridge metadata is unchanged.
Explicit load choices start neutral immediately. Submit an initial extended
packet, even with no held controls: it applies before the first opcode of fast
boot's short first frame. Subsequent changes apply at VBLANK. The legacy setter
retains its initial short-frame timing and automatic ProLine fallback.

Ordinary device replacement clears only that port's applied controls; repeated
same-device packets preserve device protocol state. EEPROM identity, placement,
span, layout and bytes remain frozen until successful reload. Power cycle settles
already-due EEPROM programming, discards unfinished protocol and pending input,
clears volatile controls and restores the remembered load choices neutrally.
Successful reload starts a fresh battery baseline and neutral controls; rejected
loads and input leave execution and published storage intact. Console Reset is
only a held switch. Calls on one instance are single-threaded; independent
instances share no state. There is no shared-instance parallel-call guarantee.
Extended setters, device changes, frame execution, peeks and power cycles use
instance storage without allocating after load, including with an opposite EEPROM.

## Memory

`muhtari_get_memory(m, kind, &span)` fills a `muhtari_span` with a live pointer into the machine and its length:

| `kind` | Memory | Bytes |
|---|---|---|
| `MUHTARI_MEMORY_SYSTEM_RAM` | System RAM, $1800-$27FF | 4,096 |
| `MUHTARI_MEMORY_RIOT_RAM` | The RIOT's RAM, $0480-$04FF | 128 |
| `MUHTARI_MEMORY_CART_RAM` | Physical cartridge RAM, including both HALT halves | `ram_size`, or 0 |
| `MUHTARI_MEMORY_EXPANSION_RAM` | Physical volatile expansion RAM, independent of active windows and battery storage | 131,072, or 0 |

The pointer is not const: a host may write through it, for cheats or achievements. It is the one deliberate
exception to the rule that no struct carries a pointer into the library's state, because libretro memory maps
need live pointers. It stays valid from the load until `muhtari_destroy`, across frames and power cycles, except
the cartridge RAM's, below. A kind the loaded content does not have, such as cartridge RAM on a cartridge without
any, returns `MUHTARI_STATUS_OK` with a NULL `data` and zero `bytes`. Any other `kind` returns
`MUHTARI_STATUS_ERR_ARGUMENT`.

Physical cartridge and expansion spans describe storage, not permanently mapped
CPU ranges. Bank/page and HALT selection or disabling an overlay does not replace
their pointer or length. Expansion storage is volatile, remains separate from
battery bytes, and survives gate/bank changes; power cycle clears it without
replacing its pointer. Reacquire both spans after a successful load. A refused
library load preserves the previous spans; the libretro adapter releases its
old session before attempting replacement, so failure or unload clears its map
and SAVE_RAM. Reacquire frontend pointers only after another successful load.

Libretro's static achievement descriptors omit banked/HALT RAM, all cartridge
windows at $4000-$7FFF with XBoard/XM installed, and XM HSC windows regardless
of gate state. HSC remains physically accessible through the battery API and
SAVE_RAM. BankSet's stable ROM descriptors use CPU-half geometry and omit its
switched window and MARIA half; see [libretro memory maps](libretro.md#achievements).

`MUHTARI_MEMORY_CART_RAM` is the RAM the cartridge info's `ram_size` names: 16,384 bytes on a SuperGame board with
RAM, 32,768 with banked RAM (both halves, in order), and the list's size for a game the hash list knows. It lives in
the cartridge's allocation, so a new load replaces it: ask again after each load. It starts zeroed, and a power cycle
zeroes it again.

Fractalus exposes exactly 2,048 physical bytes in `MUHTARI_MEMORY_CART_RAM`,
in physical order. The CPU's $4000-$7FFF aliases cover those bytes as described
in [the cartridge map](hardware/cartridge.md#fractalus); the A8 header name does
not imply an 8 KiB span. This is volatile RAM, with no battery/save span or
save-device declaration. Its bus aliases do not promise complete achievement
visibility through host descriptors.

Loading a cartridge or calling `muhtari_power_cycle` restores bank zero on
Absolute and Activision and zeroes all Fractalus RAM. A power cycle preserves
the existing span pointer; loading replaces the allocation, so query the span
again. Repeated bus traffic, frames and power cycles allocate nothing after
load. `MUHTARI_SWITCH_RESET` is a console input the game reads and leaves
cartridge state intact unless the game's own code changes it. The libretro
adapter's `retro_reset` calls `muhtari_power_cycle`, so a frontend Restart
restores that deterministic profile.

## Counters

`muhtari_get_counters` fills a `muhtari_counters`. `frames` counts the frames run since the last load or power
cycle. `lag_frames` counts those in which the CPU read none of the input registers, SWCHA, SWCHB or INPT0-5:
frames in which the game did not look at the controls. A read by MARIA's DMA is not a CPU read, so it does not
count. With no content loaded every frame is a lag frame.

## Statuses

`muhtari_status` is a `uint32_t`; the `MUHTARI_STATUS_*` names are constants of an unnamed enum, so the type has the
same size on every target. In C++, a conditional expression that mixes the two, such as
`ok ? st : MUHTARI_STATUS_ERR_ARGUMENT`, draws a warning under `g++ -Wextra` (an error with `-Werror`). Compare
instead (`st == MUHTARI_STATUS_OK`), or cast the constant: `(muhtari_status)MUHTARI_STATUS_ERR_ARGUMENT`.

Statuses are only ever added. Those after the probe's:

| Status | Returned by | Meaning |
|---|---|---|
| `MUHTARI_STATUS_ERR_BIOS_REJECTED_CART` | `muhtari_run_frame` | The BIOS took its 2600 path. It stays until the next load or power cycle. |
| `MUHTARI_STATUS_ERR_INVALID_BIOS` | `muhtari_load` | The BIOS is not 4,096 or 16,384 bytes, or its reset vector points outside it. |
| `MUHTARI_STATUS_ERR_UNSUPPORTED_REGION` | `muhtari_load` | Reserved numeric region-refusal status; both current regions execute. |
| `MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE` | load, input setters | Unsupported declared sound/topology, a device outside the frozen legacy set, or an attempted change to load-frozen EEPROM topology. |
| `MUHTARI_STATUS_ERR_INVALID_HSC` | `muhtari_load` | Invalid firmware size/pointer pair, HSC mode, or supplied firmware conflicting with OFF. |

## Version numbers

`MUHTARI_API_VERSION` in the header is the API the program was compiled against. At run time,
`muhtari_get_versions` fills a `muhtari_versions`:

The current interface is API 7, ABI 1, behaviour revision 6. API 7 appends
expansion selection and effective information, ADPCM metadata and a distinct
physical expansion memory kind. The original media/info prefixes, all prior
size minima, signatures and constant values remain compatible. Revision 6
includes the corrected slow-edge DMA handoff; versioned test observations
retain their independently established values.

| Field | Meaning |
|---|---|
| `library_major`, `library_minor`, `library_patch` | The release. `library_major` is 0 before version 1.0. |
| `api` | The API the library implements. |
| `abi` | The binary interface: struct layouts and calling conventions. |
| `behaviour_revision` | Changes when the same inputs give a different frame, audio or state hash. |

`api`, `abi` and `behaviour_revision` start at 1, so zero means absent: a field that a library older than your
header does not know stays at the zero you wrote. The library version numbers carry no such meaning.

Sound output is fixed-rate 48,000/1 Hz mono signed int16. A fast-boot first
frame returns 54 NTSC or 960 PAL samples; full frames return 800/801 or 961/962. Use audio_count,
not the buffer capacity. Passing NULL for audio discards samples while preserving
synthesis history and future output. Raw-transition observation is an internal
runner seam; the installed library exposes no observer registration facility.
