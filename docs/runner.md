# muhtari-run

`muhtari-run` is the headless runner. It runs the emulator for a fixed number of frames with no window, sound
device or input device, and prints hashes, writes PNG images or reports frame times. Scripts and CI use it to
check that a build produces the expected picture.

Given a cartridge image FILE, it loads the image and starts it from the fast-boot hand-off, the state the
console's BIOS leaves as it jumps to the cartridge, or, with `--boot bios --bios FILE`, through a BIOS image from
power-on. Without one, every run shows the built-in test card (see
[Test card](#test-card)). `--movie` replays recorded controller and console-switch input, one line per frame
(see [Movies](#movies)). `--probe` reads a cartridge image and reports what it is, without running it (see
[Probing an image](#probing-an-image)).

Automatic header/hash-selected PAL and explicit `--region pal` use successful-load info for output allocation.
Native hashes cover 93440 bytes, PNGs are 320×292, and audio has 960 samples in the
fast first call then 961/962 per full frame. PNG presentation uses the selected region's warm palette.
Embedding callers must likewise requery info after every successful content load.

## Synopsis

```text
muhtari-run --frames N [--hash-frame N]... [--dump-frame N:FILE]... [--rom] FILE
muhtari-run --frames N [--hash-frame N]... [--dump-frame N:FILE]...
muhtari-run --bench N [--hash-frame N]... [--dump-frame N:FILE]... [[--rom] FILE]
muhtari-run --movie FILE [--frames N | --bench N] [--hash-frame N]...
            [--dump-frame N:FILE]... [[--rom] FILE]
muhtari-run --frames N --boot bios --bios FILE [--hash-frame N]... [--rom] FILE
muhtari-run --probe FILE
muhtari-run --help | --version
```

`--port1`, `--port2`, `--region`, `--boot`, `--bios` and `--info` may be added to any run.

## Options

| Option | Meaning |
|---|---|
| `--frames N` | Run N frames, N from 1 to 4294967295. Give `--frames` or `--bench`, once; with `--movie`, the default is the movie's frame count. |
| `--bench N` | Run N frames, time each one, and print the report described under [Output](#output). It keeps 4 bytes per frame, so where `size_t` has 32 bits an N of 1073741824 or more exits 1 as out of memory. |
| `--hash-frame N` | After frame N, print the SHA-256 of its picture. Repeat the option for more frames. N is from 1 to the frame count. |
| `--diagnostic` | After the run, report the fixed diagnostic mailbox. Requires content and explicit `--frames N` with the ordinary frame bounds; no `--bench` or implicit movie frame count. |
| `--dump-frame N:FILE` | After frame N, write its picture to FILE as a PNG. Repeat the option for more frames or files. |
| `[--rom] FILE` | Load and run one cartridge image, with or without `--rom`. Automatic PAL selection is supported. A refused image prints `muhtari-run: FILE: <message>` on standard error and runs no frame: a 2600 image or unsupported board exits 77; malformed, truncated or oversized images exit1. |
| `--movie FILE` | Replay the movie in FILE, described under [Movies](#movies). The whole file is checked before the first frame runs. |
| `--port1 D`, `--port2 D` | `auto` (the cartridge's, the default), or an explicit movie2 device below. A value other than `auto` that differs from the movie header is a usage error. Storage attaches before import; moving or adding it requires another load. AtariVox provides EEPROM only, without speech. |
| `--region R` | `auto` (the cartridge's, the default), `ntsc` or `pal`. An explicit value overrides cartridge metadata; a conflicting movie region is a usage error. Without content the test card remains NTSC. |
| `--info` | Print effective region, geometry, exact frame rate, pixel aspect ratio, palette and hardware selection before frame output. Repeat use is a usage error; this is a run option, not an addition to `--probe`. |
| `--boot B` | `fast` (the default) starts the cartridge from the fast-boot hand-off; `bios` boots it through the BIOS given by `--bios`, from power-on. `bios` without `--bios`, or `--bios` without `bios`, is a usage error. With a movie, the movie's `boot` key decides, and a different `--boot` is a usage error. |
| `--bios FILE` | The BIOS image `--boot bios` runs: 4,096 or 16,384 bytes whose reset vector points inside it. Its SHA-256 is printed on standard error as `bios_sha256 <hex>`. Any other file exits 1 with `muhtari-run: FILE: not a 4 KiB or 16 KiB BIOS image`. A cartridge the BIOS rejects exits 77 with `muhtari-run: FILE: the BIOS rejected the cartridge`, after the frame in which it did. |
| `--save-dir DIR` | Opt into per-game raw battery files in an existing accessible directory. Requires a cartridge, even when it has no battery. See [Battery files](#battery-files). |
| `--save-interval F` | Persist dirty battery bytes every F completed frames, from 1 to 4294967295; default 60. Requires `--save-dir`. |
| `--hsc auto\|on\|off` | Auto attaches for an HSC request or effective XM; On forces attachment, Off suppresses it. Requires a cartridge. |
| `--hsc-rom FILE` | Supply an exact 4,096-byte HSC firmware file, copied at load. Invalid size, unreadable file or an explicit `--hsc off` conflict exits 1 before import or execution. |
| `--probe FILE` | Identify the cartridge image in FILE and print its fields and hashes. It takes exactly one file and no other option. |
| `--help` | Print the usage text and the exit statuses. It takes no other arguments. |
| `--version` | Print `muhtari-run` and the version. It takes no other arguments. |

Anything else on the command line, including a second file name, is a usage error.

`--expansion auto|off|xboard|xm` selects hardware at load. Auto follows any nonzero A78 XM request, otherwise Off; XBoard requires explicit selection. Off suppresses expansion only. Declared YM2151, Covox and ADPCM still refuse execution with exit77 and a named sound message.

XM is a RAM/POKEY/HSC subset with medium physical confidence, without internal BIOS/top-slot switching, flash, PIA, flat48KiB mode, write protection, sticky locks, ROF aliases, PAL firmware selection or full-XM services. HSC firmware is read only for attached storage; a firmware request with detached HSC fails before file I/O. No gameplay compatibility is implied.

## Battery files

```sh
muhtari-run --frames 600 --save-dir saves --hsc on game.a78
```

Create the directory before starting the runner. Without `--save-dir`, the runner reads and creates no save
files. With it, a cartridge without battery storage runs without a save file after validating the directory.
The file name uses only the final ROM component and removes its final extension: `game.a78` becomes
`game.sav`, `game.rev.a78` becomes `game.rev.sav`, `.hidden` becomes `.hidden.sav`, and `game` becomes
`game.sav`. Both native Windows path separators are accepted. Use one writer per save path. Games with the
same basename and concurrent sessions need separate save directories; there is no locking or identity sidecar.

The file is the library's raw layout version 1: HSC SRAM alone is 2,048 bytes, EEPROM alone is 32,768 bytes,
and combined storage is 34,816 bytes with HSC first. Firmware is separate and never enters the save. Restore
finishes before the first cartridge frame. Missing and zero-length files import all FF; short files retain
their prefix and pad the rest with FF; exact files copy verbatim. An oversized file exits 4. Directory,
non-regular-file, open, read and close errors exit 1 before any frame. Rejected imports never arm persistence.
The import is clean: missing files stay absent and existing files stay unchanged until emulated bytes change.

At each save cadence, on a save request and on normal or orderly stop exit, dirty committed bytes are written
to an exclusively created sibling temporary, then checked
for complete write, stdio flush, file sync and close. POSIX `rename`, or Windows `MoveFileEx` with replacement
and write-through and no copy fallback, replaces the destination. That replacement is the commit point.
Tests exercise previous-file preservation on every pre-replacement failure and ownership of temporary files.
This is a local-filesystem guarantee, without a universal power-loss durability promise or a promise to retain
old bytes after successful replacement followed by a later error. No directory sync is required.

A save failure prints an error and exits 1 without a cleanup retry. Final flushing precedes destruction of
valid state, including after an unrelated PNG or output error following valid frames; the original failure
exit is preserved. Failed load, failed import and BIOS rejection suppress saving. Flushing never advances
emulated time to finish a pending EEPROM write. The persisted generation is updated only after replacement.

The default cadence is 60 completed frames; `--save-interval F` changes it to a positive frame count. A change
waits at most F subsequently completed frames. Refused or failed frames do not advance the cadence. Clean
cadences, requests and exits do not create or rewrite a file. On POSIX, SIGUSR1 requests a save; on Windows,
console Ctrl+Break requests one. Requests coalesce and are consumed before file I/O, so a request received
during a save remains pending for the next completed frame. Handlers only record requests; all core access
and file I/O happen in the frame loop. Forced termination has no final-flush guarantee. No IPC endpoint is added.

Without supplied HSC firmware, the runner warns that the signature-only stub supplies no firmware routines.
This permits direct SRAM callers and does not implement HSC formatting, screens, routines or full OpenHSC.
Supplied firmware prints `hsc_sha1 <40 lowercase hex digits>` and separate unknown-identity or missing-C6/FE
signature warnings as applicable. Unknown exact-sized firmware remains accepted. See
[the embedding contract](embedding.md) and [cartridge hardware](hardware/cartridge.md).

## Output

`--info` adds these newline-terminated key/value lines, in this order, after successful
load, validation and output allocation and before any frames run. For PAL:

```text
region pal
width 320
height 292
fps_num 3546894
fps_den 71051
par_num 1843750
par_den 1773447
palette warm_pal
```

NTSC reports `ntsc`, 320, 242, 39375000, 656711, 6, 7 and `warm_ntsc`, respectively.
Argument order does not change the report. It describes the effective successful session.
After the palette line, `requested_devices` and `requested_sound` retain the probe requests, `expansion` reports `off`, `xboard` or `xm`, and `hsc_attached` reports physical attachment as 0/1. These fields do not describe volatile CPU decode.

`--probe` continues to describe the file. Without `--info`, ordinary output is unchanged.

`--diagnostic` prints exactly `diagnostic `, 512 lowercase hexadecimal digits,
then a newline. The digits encode the 256 bytes at CPU $2200-$22FF in address
order, read through system RAM at offset $A00 after the requested run. It runs
no extra frames and accepts no address selector. Tests own the mailbox protocol,
CRC and numeric verdict; the runner only reports observed bytes. Invalid usage
exits 2; a report read or output failure exits 1. Other requested output keeps
its usual format.

`--hash-frame N` prints one line per requested frame, in ascending frame order, once per frame even when the same
frame is requested twice:

```text
frame 1 sha256 9eb011e22feb3f48da2c0abf3dd8d5236d4d0f5c0e67a732eb2172259e0a05c4
```

The hash covers the 320×242 NTSC or 320×292 PAL native colour bytes (one byte per pixel, the console's colour number), row by row
from the top. It is the same on every platform.

`--dump-frame N:FILE` writes an unscaled 320×242 NTSC or 320×292 PAL RGB PNG with 8 bits per channel,
through the effective region's warm NTSC or warm PAL palette. The PNG carries no aspect-ratio metadata.
Data is uncompressed; an NTSC file is 232,645 bytes.

With `--movie`, one more line follows the hash lines:

```text
lag_frames 30
```

It counts the frames in which the game read no controller or console switch (no CPU read of SWCHA, SWCHB or
INPT0 to INPT5).

With `--boot bios`, a line follows the hash lines, before `lag_frames`:

```text
handoff_frame 323
```

It is the hand-off frame: the frame of the first opcode fetch from cartridge space after the BIOS sets INPTCTRL's
EXT bit, or `none` when no hand-off came in the frames run (`docs/embedding.md`, Booting with a BIOS). The runner
reads it through an internal call of the core, not the public API. The public header holds only what an embedder
needs, and the hand-off frame is a test and diagnostic observable; the runner is built in this tree against the
static library, and already uses the core's internal SHA-1 the same way. The BIOS digest line goes to standard
error, before any frame runs:

```text
bios_sha256 46b5bc2b716a20a1c40e2b920365c29cbab1594d01da94f4567b240181365cb2
```

`--bench N` prints nine `key value` lines after the run:

| Key | Value |
|---|---|
| `frames` | frames timed |
| `fps_milli` | frames per second × 1000 |
| `p50_us`, `p90_us`, `p99_us`, `p99_9_us`, `p99_99_us`, `p99_999_us` | microseconds per frame at the 50th, 90th, 99th, 99.9th, 99.99th and 99.999th percentile |
| `max_us` | the slowest frame in microseconds |

Only the time spent running each frame is measured, not hashing or writing files. A percentile is the
nearest-rank sample: with n frames, the p-th percentile is the frame time of rank ⌈n × p⌉ in ascending order, so
every value reported is a time that was actually measured. Microseconds are truncated. With fewer than 100,000
frames, p99.999 is the maximum.

## Probing an image

`muhtari-run --probe FILE` reads FILE whole, up to one byte past the size limit, and identifies it as the library's
`muhtari_probe` does. It reads with plain sequential reads, so FILE may be a pipe such as `/dev/stdin`. Standard
output gets one `key value` line each, where the value is everything after the first space, in this order:

1. `kind source board board_options rom_size ram_size sound devices port1 port2 region`;
2. for an image with an A78 header only, `title` and the raw header fields `a78_version a78_rom_size a78_cart_type
   a78_port1 a78_port2 a78_tv a78_save a78_xm a78_mapper a78_mapper_options a78_audio a78_irq`;
3. `flag activision_half_swapped` only for an identified Activision image in the
   half-swapped input orientation, then one `warning <name>` line for each warning,
   such as `warning size_field_mismatch`;
4. `sha1 sha256 md5 crc32` of the file as given, then `sha1_headerless sha256_headerless md5_headerless
   crc32_headerless`.

Values are lowercase names (`kind 7800`, `board supergame`, `port1 proline`, `region ntsc`); sets of flags are
names separated by commas, or `none` (`sound pokey_450,ym2151`); raw header fields are hexadecimal, `0x%02x`, with
four digits for `a78_cart_type` and `a78_audio`; sizes are decimal bytes. Hashes are lowercase hexadecimal, eight
digits for CRC-32.

The headerless hashes follow the RetroAchievements rule for the 7800 (rcheevos @1433173220a7,
`src/rhash/hash_rom.c`): when the file is longer than 128 bytes and bytes 1 to 9 are `ATARI7800`, the first 128
bytes are left out; any other file, including one of exactly 128 bytes, is hashed whole. `md5_headerless` is the
RetroAchievements game hash. Where the bytes the cartridge maps differ from this input, the probe also prints
`warning size_field_mismatch`.

Activision runs from a private OM copy. Only a hash-authorized half-swap match
sets `flag activision_half_swapped` and converts the copy; the hashes above
always cover the original supplied bytes. Original AM and OM dumps therefore
retain different identities. A header's size field limits mapped bytes while
the raw headerless hashes still include trailing bytes after that mapped body.
An unknown explicit Activision declaration reports
`warning activision_layout_assumed`: OM is assumed, not detected. Modified AM
images without a match need external conversion. Unknown headerless content
retains the ordinary probe fallback; size and filename do not identify its layout.
See [the cartridge map](hardware/cartridge.md#activision) for the selected
banking and startup policies. API 4, ABI 1 and behaviour revision 3 apply.

When the image is refused, standard error gets `muhtari-run: FILE: <message>`, and fewer lines print:

| Image | Message | Lines printed | Exit status |
|---|---|---|---|
| a 2600 cartridge | `unsupported: 2600 (planned)` | `kind source rom_size` and the hashes | 77 |
| a 7800 board this version does not emulate | `unsupported board: <board>` | all the lines above | 77 |
| not a cartridge image, or a header that fits no board | `not a 7800 or 2600 cartridge image` | the hashes | 1 |
| shorter than its header says | `the image ends before the size its header gives` | the hashes | 1 |
| more than 1,048,704 bytes | `larger than 1 MiB plus a 128-byte header` | none | 1 |

A file that cannot be opened or read exits 1 with `cannot open` or `cannot read`.

## Movies

A movie is a text file in the `muhtari-movie 1` format: a header, then one line per frame.

```text
muhtari-movie 1
# Up held on port 1 for two frames, then Reset pressed
port1 proline
port2 joystick
|......|.....|...BB|
|U.....|.....|...BB|
|U.....|.....|...BB|
|......|.....|R..BB|
```

The first line is exactly `muhtari-movie 1`. Header lines follow, each a key, one space and a value, in any
order and each key once. Blank lines and lines starting with `#` are allowed in the header only.

| Key | Value |
|---|---|
| `port1`, `port2` | Required: `proline`, `joystick`, `none`, `savekey` or `atarivox`. These explicit declarations provision storage before import. |
| `rom-sha256` | Optional: 64 hex digits, the cartridge's headerless SHA-256, as `--probe` prints in `sha256_headerless`. A different cartridge exits 1, naming both digests. |
| `region` | Optional: `ntsc` or `pal`. An explicit CLI region must agree. Without it, an explicit CLI selection wins, otherwise cartridge metadata selects the console. |
| `boot` | Optional: `fast` or `bios`; `bios` also needs a `bios-sha256` line. `bios` boots through the BIOS given by `--bios FILE`, and exits 2 without it. |
| `bios-sha256` | Optional: 64 hex digits, the BIOS file's SHA-256. With `boot bios`, a `--bios` file with another digest exits 1 before any frame runs, naming both digests. |
| `behaviour` | Optional: the core behaviour revision the movie was recorded on, 1 to 999999999. A different revision prints one warning on standard error, and the replay continues. |

The first line starting with `|` ends the header; every later line is a frame line. A frame line has three fields,
port 1, port 2 and the console, each closed by `|`. Each position holds its letter when the button or switch is
held, or `.` when it is not:

| Field | Positions |
|---|---|
| ProLine | `UDLR12`: up, down, left, right, button 1 (right), button 2 (left) |
| Joystick | `UDLR1`: up, down, left, right, fire |
| `none`, `savekey`, `atarivox` | empty; save devices have no movie buttons or speech fields |
| Console | `RSP`: Reset, Select and Pause, then the left and right difficulty switches as `A` or `B` |

The difficulty switches are positions, not presses: each line gives where both switches stand.

**Timing.** Line k after the header is the input of frame k: the runner sets it before running frame k, and the
console reads it from the start of vertical blank that opens frame k. Under the fast boot, frame 1 opens at the
hand-off, not at a vertical blank, so the first frame line reaches no game; put a movie's first input on its second
line. Under `boot bios`, frame 1 opens at power-on, line 0, which lies inside vertical blank but not at its start;
the console reads line 1 from power-on, as what is held when the power comes on, so line 1 counts. OpenBIOS first
reads the console switches in frame 3, so Pause held on lines 1 to 3 skips its splash.

**Length.** `--frames` defaults to the number of frame lines. When `--frames` runs past the last line, every
button and console switch is released, and the difficulty switches stay where the last line put them.

**Precedence.** The header wins over the command line. `--port1`, `--port2`, `--region` or `--boot` naming a
different value from the header's exits 2; `auto`, or the same value, is accepted.

**Limits.** A movie is at most 33,554,432 bytes (32 MiB), 1,000,000 frame lines and 32 header lines, counting
the first. A line is at most 64 bytes, except a `rom-sha256` or `bios-sha256` line, which is its key, a space and
64 hex digits. Only printable ASCII is allowed, with lines ending in LF or CR LF; a lone CR, a NUL, a byte order
mark or a byte above `$7F` is refused. Unknown keys, repeated keys and values outside each key's set are refused
by name.

A refused movie runs no frame. It exits 1 with one line on standard error naming the first fault in the file, its
line and its column, both counted from 1:

```text
muhtari-run: run.txt:4:2: letter out of place, expected U or .
```

### Typed movies

`muhtari-movie 2` adds typed controller packets. Version 1 above retains its grammar,
diagnostics, legacy input path and EOF behavior. Version 2 uses the same header keys,
but both required port declarations accept every explicit device in this table.
`auto` is CLI-only. Each frame repeats exactly those devices; hotplug is refused:

```text
muhtari-movie 2
port1 driving
port2 none
|driving:0,0,0|none:|...BB|
|driving:4,0,1|none:|...AB|
|driving:-1,0,0|none:|...AB|
```

Frames are `|device:payload|device:payload|RSPAA|`. The console positions retain
the version 1 letters, dots and difficulty meanings. Payloads encode complete
absolute state; motion contributes once per frame, in connector edges:

| Device | Payload |
|---|---|
| `proline`, `joystick` | Existing `UDLR12` / `UDLR1` letter/dot fields |
| `none`, `savekey`, `atarivox` | Empty, including the colon: `none:` |
| `lightgun` | `x,y,flags`: native pixel coordinates x 0–319, y 0–241 for effective NTSC or 0–291 for effective PAL; flags 0–3, bit 0 trigger, bit 1 off-screen |
| `paddle` | `pot0,pot1,buttons`: positions 0–65535, increasing resistance; buttons 0–3, low bits fire 1/2 |
| `driving` | `dx,0,button`: signed 32-bit X motion, literal zero Y, button 0–1 |
| `trakball` | `dx,dy,button`: signed 32-bit motion, button 0–1 |
| `st-mouse`, `amiga-mouse` | `dx,dy,buttons`: signed 32-bit motion, buttons 0–3 |
| `keypad` | Held-key decimal mask 0–4095, row-major low 12 bits |
| `snes2atari`, `mega7800` | Decimal button mask 0–4095: low bits Up, Down, Left, Right, B, A; upper bits Y/X/L/R/Select/Start for SNES or C/X/Y/Z/Mode/Start for Mega |

Both adapter tags execute on either port. For Mega the low B/A bits name
native B/A, independent of frontend layout: B is the normal primary button,
C is bit6, and A is extended-only. SNES uses the native B/A names. A typed
adapter row never implies console switches; those remain the final explicit
field. `4095` holds every direction and native button; `0` releases them all.
The owned controller diagnostic checks complete serial/pair scans through
actual replay, then compares all12 native frames, raw transitions and PCM
with the library and loaded module. EOF is checked against explicit release
on both ports. See [emulation tests](emulation-tests.md).

Decimal integers have no whitespace, plus sign, leading zeros or negative zero.
Signed motion accepts -2147483648 through 2147483647; the core applies its bounded
motion policy. Empty, missing or extra fields, unknown/mismatched tags, invalid
masks and out-of-range values are refused. Off-screen aim remains within native
bounds; use 0,0 when aim is unknown. Syntax support does not imply physical device
support: a device not implemented by this core revision is still refused by the core.

The whole immutable file's syntax is validated before loading media. Gun Y coordinates are decoded
up to 291; after load resolves the effective region, a complete second pass checks its height before
allocating frame outputs, printing `--info`, importing saves or
creating outputs. Replay retains the bytes and one cursor with the last packet;
it creates no per-frame packet array or index and never reopens the file. At EOF,
buttons, keys and momentary console switches release; dx/dy become zero. Final
pot positions, gun aim/off-screen state and difficulty positions persist. Empty
movies need an explicit `--frames N` and use neutral devices (gun off-screen).
Non-movie sessions submit modern Auto before their first frame, even with no held
controls; explicit load overrides take priority over cartridge device metadata.

All existing total-file, frame and header limits apply. Version 2 frame lines allow
83 bytes excluding LF/CRLF: two maximum 37-byte
`amiga-mouse:-2147483648,-2147483648,3` fields, four delimiters and five console
characters. Byte 84 is refused. Other header lines retain 64 bytes and hash lines
retain their exact special limits. Refusals report the first invalid line and column
from 1, before any frame runs. CLI/header conflicts exit 2; malformed files and
identity mismatches exit 1. Behaviour revision mismatches still warn and replay.

## Stopping

On POSIX, SIGINT (Ctrl+C) or SIGTERM ends the run after the frame in progress. On Windows, console Ctrl+C
requests the same orderly stop. That frame's hash and PNG are still written, `--bench` reports the frames
that ran, output is flushed and dirty battery bytes are saved before destruction. Exit 0 requires all required
flushing to succeed; an earlier nonzero error keeps its status. A broken POSIX output pipe returns exit 1
after saving dirty bytes. Stop/save signals inherited as ignored, as under `nohup`, remain ignored;
Windows preserves its inherited Ctrl+C ignore attribute. Ctrl+Break saves without
stopping. Forced process or console termination has no final-flush guarantee.

## Exit status

`muhtari-run --help` prints this table:

```text
  0  done     every frame ran, or SIGINT or SIGTERM stopped the run after a whole frame
  1  failure  a file could not be read or written, an image or a movie is malformed, truncated or too large, a BIOS is invalid, a movie names another cartridge or BIOS, memory ran out or the core failed
  2  usage    the command line is invalid or conflicts with the movie's header, so nothing ran
  4  save-size  the save file is larger than the attached battery RAM
 77  refused  the image is a 2600 cartridge, or uses a board, device or region this version does not emulate, or the BIOS rejected it
```

Errors and usage messages go to standard error; standard output holds only the results above.

## Test card

With no cartridge loaded, every frame is the same 320×242 test card. Each element shows a particular way a host
can display the picture wrongly. Colours below are console colour numbers.

- **Corner block.** An 8×8 white block sits at the top-left corner only. If it appears at another corner, the
  picture is mirrored or flipped.
- **Border.** A 1-pixel white border runs around the whole frame. A missing edge means the host crops the picture.
- **Grey lines.** Rows 8 and 231 are grey (`$08`). They are the first and last rows of a 224-row overscan crop
  (rows 8 to 231); a host that applies that crop shows them at its top and bottom edges.
- **Colour chart.** A 16×16 chart fills rows 25 to 216 and columns 48 to 271. Each row of cells is one hue and each
  column one brightness, so cell (r, c) shows colour `$rc` and every colour from `$00` to `$FF` appears once.
  Each cell is 14 pixels wide and 12 tall. The console's pixels are narrower than they are tall (6:7), so the
  cells, and the chart as a whole, look square only when the frame is shown at an aspect ratio of 960:847.
- **Checkers.** Left of the chart, a 1×1 black-and-white checker shows filtering and non-integer scaling: on a
  correct display every pixel stays sharp. Right of the chart, a 2×1 checker matches the console's 160-pixel mode,
  where each pixel is two columns wide.
- **Bands.** A red band (`$46`) fills the top and a green band (`$C6`) the bottom. Green on top means the picture
  is upside down; a blue top band means the host has swapped red and blue.

The `--dump-frame` PNG is the reference for screenshot comparisons: it is the card exactly as the core produces
it, unscaled, through the warm NTSC palette.

`--hash-audio` adds seven lines after frame hashes, BIOS/movie reports and any
benchmark report, in this order:

```
audio_transitions_sha256 <64 lowercase hex digits>
audio_pcm_sha256 <64 lowercase hex digits>
audio_frames <completed frame count>
audio_samples <actual mono sample count>
audio_rate 48000/1
audio_behaviour_revision <muhtari_get_versions value>
audio_synthesis exact-q23-16x64-dc20
```

The transition SHA-256 covers 11 bytes per changed raw DAC level: little-endian
uint64 absolute tick, uint8 source, little-endian uint16 level (0–15).
Sources 0–1 are TIA, 2–5 primary POKEY, 6–9 secondary $440. Equal ticks use
ascending source identity and retain same-source subevents. Initial zeros and
unchanged levels add no records; a silent raw stream hashes empty input.
The PCM SHA-256 separately covers two little-endian bytes per actual signed
int16 mono sample, including silence. Neither capacity padding nor stereo
duplication enters it. Counts cover the entire completed run, including a
partial first frame, with half-open transition ownership at each deadline.

An orderly platform stop ends between completed frames. A stop before the first frame
reports empty streams and zero counts; `--frames 0` remains a usage error.
Ordinary rejected frame calls execute nothing. A sticky BIOS refusal may follow
partial execution; its tentative transition hash is discarded. Any execution,
load or PNG dump error suppresses all seven audio success lines, including when
earlier frames completed. The flag may accompany frames, bench or movie runs;
duplicates and probe/help/version combinations are usage errors.
