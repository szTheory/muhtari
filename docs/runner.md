# muhtari-run

`muhtari-run` is the headless runner. It runs the emulator for a fixed number of frames with no window, sound
device or input device, and prints hashes, writes PNG images or reports frame times. Scripts and CI use it to
check that a build produces the expected picture.

Given a cartridge image FILE, it loads the image and starts it from the fast-boot hand-off, the state the
console's BIOS leaves as it jumps to the cartridge. Without one, every run shows the built-in test card (see
[Test card](#test-card)). `--probe` reads a cartridge image and reports what it is, without running it (see
[Probing an image](#probing-an-image)).

## Synopsis

```text
muhtari-run --frames N [--hash-frame N]... [--dump-frame N:FILE]... [--rom] FILE
muhtari-run --frames N [--hash-frame N]... [--dump-frame N:FILE]...
muhtari-run --bench N [--hash-frame N]... [--dump-frame N:FILE]... [[--rom] FILE]
muhtari-run --probe FILE
muhtari-run --help | --version
```

## Options

| Option | Meaning |
|---|---|
| `--frames N` | Run N frames, N from 1 to 4294967295. Give `--frames` or `--bench`, once. |
| `--bench N` | Run N frames, time each one, and print the report described under [Output](#output). It keeps 4 bytes per frame, so where `size_t` has 32 bits an N of 1073741824 or more exits 1 as out of memory. |
| `--hash-frame N` | After frame N, print the SHA-256 of its picture. Repeat the option for more frames. N is from 1 to the frame count. |
| `--dump-frame N:FILE` | After frame N, write its picture to FILE as a PNG. Repeat the option for more frames or files. |
| `[--rom] FILE` | Load the cartridge image in FILE and run it. Give at most one FILE, with or without `--rom`. A refused image prints `muhtari-run: FILE: <message>` on standard error, with the messages of [Probing an image](#probing-an-image) or `unsupported region: PAL (planned)`, and runs no frame: a 2600 image, a board this version does not emulate or a PAL cartridge exits 77, and an image that is not a cartridge, is truncated or is too large exits 1. |
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

## Stopping

SIGINT (Ctrl+C) or SIGTERM ends the run after the frame in progress. That frame's hash and PNG are still written,
`--bench` reports the frames that ran, output is flushed and the exit status is 0. A signal that was already
ignored when the runner started, as under `nohup`, stays ignored.

## Exit status

`muhtari-run --help` prints this table:

```text
  0  done     every frame ran, or SIGINT or SIGTERM stopped the run after a whole frame
  1  failure  a file could not be read or written, an image or a movie is malformed, truncated or too large, a movie names another cartridge, memory ran out or the core failed
  2  usage    the command line is invalid or conflicts with the movie's header, so nothing ran
 77  refused  the image is a 2600 cartridge, or uses a board or a region this version does not emulate
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
