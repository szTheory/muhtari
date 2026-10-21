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

`--port1`, `--port2`, `--region`, `--boot` and `--bios` may be added to any run.

## Options

| Option | Meaning |
|---|---|
| `--frames N` | Run N frames, N from 1 to 4294967295. Give `--frames` or `--bench`, once; with `--movie`, the default is the movie's frame count. |
| `--bench N` | Run N frames, time each one, and print the report described under [Output](#output). It keeps 4 bytes per frame, so where `size_t` has 32 bits an N of 1073741824 or more exits 1 as out of memory. |
| `--hash-frame N` | After frame N, print the SHA-256 of its picture. Repeat the option for more frames. N is from 1 to the frame count. |
| `--dump-frame N:FILE` | After frame N, write its picture to FILE as a PNG. Repeat the option for more frames or files. |
| `[--rom] FILE` | Load the cartridge image in FILE and run it. Give at most one FILE, with or without `--rom`. A refused image prints `muhtari-run: FILE: <message>` on standard error, with the messages of [Probing an image](#probing-an-image) or `unsupported region: PAL (planned)`, and runs no frame: a 2600 image, a board this version does not emulate or a PAL cartridge exits 77, and an image that is not a cartridge, is truncated or is too large exits 1. |
| `--movie FILE` | Replay the movie in FILE, described under [Movies](#movies). The whole file is checked before the first frame runs. |
| `--port1 D`, `--port2 D` | The device on port 1 or port 2: `auto` (the cartridge's, the default), `proline`, `joystick` or `none`. With a movie, the movie's header names the devices, and a `D` other than `auto` that differs from it is a usage error. |
| `--region R` | `auto` (the cartridge's, the default) or `ntsc`, which runs the cartridge as NTSC. `pal` is refused as a usage error until PAL is emulated. |
| `--boot B` | `fast` (the default) starts the cartridge from the fast-boot hand-off; `bios` boots it through the BIOS given by `--bios`, from power-on. `bios` without `--bios`, or `--bios` without `bios`, is a usage error. With a movie, the movie's `boot` key decides, and a different `--boot` is a usage error. |
| `--bios FILE` | The BIOS image `--boot bios` runs: 4,096 or 16,384 bytes whose reset vector points inside it. Its SHA-256 is printed on standard error as `bios_sha256 <hex>`. Any other file exits 1 with `muhtari-run: FILE: not a 4 KiB or 16 KiB BIOS image`. A cartridge the BIOS rejects exits 77 with `muhtari-run: FILE: the BIOS rejected the cartridge`, after the frame in which it did. |
| `--probe FILE` | Identify the cartridge image in FILE and print its fields and hashes. It takes exactly one file and no other option. |
| `--help` | Print the usage text and the exit statuses. It takes no other arguments. |
| `--version` | Print `muhtari-run` and the version. It takes no other arguments. |

Anything else on the command line, including a second file name, is a usage error.

## Output

`--hash-frame N` prints one line per requested frame, in ascending frame order, once per frame even when the same
frame is requested twice:

```text
frame 1 sha256 9eb011e22feb3f48da2c0abf3dd8d5236d4d0f5c0e67a732eb2172259e0a05c4
```

The hash covers the 320×242 native colour bytes (one byte per pixel, the console's colour number), row by row
from the top. It is the same on every platform.

`--dump-frame N:FILE` writes an unscaled 320×242 RGB PNG with 8 bits per channel, through the warm NTSC palette.
The PNG carries no aspect-ratio metadata. The data is stored uncompressed, so each file is 232,645 bytes.

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
3. one `warning <name>` line for each warning, such as `warning size_field_mismatch`;
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
| `port1`, `port2` | Required: `proline`, `joystick` or `none`. |
| `rom-sha256` | Optional: 64 hex digits, the cartridge's headerless SHA-256, as `--probe` prints in `sha256_headerless`. A different cartridge exits 1, naming both digests. |
| `region` | Optional: `ntsc`. `pal` is refused. |
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
| `none` | empty |
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

## Stopping

SIGINT (Ctrl+C) or SIGTERM ends the run after the frame in progress. That frame's hash and PNG are still written,
`--bench` reports the frames that ran, output is flushed and the exit status is 0. A signal that was already
ignored when the runner started, as under `nohup`, stays ignored.

## Exit status

`muhtari-run --help` prints this table:

```text
  0  done     every frame ran, or SIGINT or SIGTERM stopped the run after a whole frame
  1  failure  a file could not be read or written, an image or a movie is malformed, truncated or too large, a BIOS is invalid, a movie names another cartridge or BIOS, memory ran out or the core failed
  2  usage    the command line is invalid or conflicts with the movie's header, so nothing ran
 77  refused  the image is a 2600 cartridge, or uses a board or a region this version does not emulate, or the BIOS rejected it
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

SIGINT/SIGTERM stops between completed frames. A stop before the first frame
reports empty streams and zero counts; `--frames 0` remains a usage error.
Ordinary rejected frame calls execute nothing. A sticky BIOS refusal may follow
partial execution; its tentative transition hash is discarded. Any execution,
load or PNG dump error suppresses all seven audio success lines, including when
earlier frames completed. The flag may accompany frames, bench or movie runs;
duplicates and probe/help/version combinations are usage errors.
