# muhtari-run

`muhtari-run` is the headless runner. It runs the emulator for a fixed number of frames with no window, sound
device or input device, and prints hashes, writes PNG images or reports frame times. Scripts and CI use it to
check that a build produces the expected picture.

This release has no cartridge loading, so every run shows the built-in test card (see [Test card](#test-card)).

## Synopsis

```text
muhtari-run --frames N [--hash-frame N]... [--dump-frame N:FILE]...
muhtari-run --bench N [--hash-frame N]... [--dump-frame N:FILE]...
muhtari-run --help | --version
```

## Options

| Option | Meaning |
|---|---|
| `--frames N` | Run N frames, N from 1 to 4294967295. Give `--frames` or `--bench`, once. |
| `--bench N` | Run N frames, time each one, and print the report described under [Output](#output). It keeps 4 bytes per frame, so where `size_t` has 32 bits an N of 1073741824 or more exits 1 as out of memory. |
| `--hash-frame N` | After frame N, print the SHA-256 of its picture. Repeat the option for more frames. N is from 1 to the frame count. |
| `--dump-frame N:FILE` | After frame N, write its picture to FILE as a PNG. Repeat the option for more frames or files. |
| `--help` | Print the usage text and the exit statuses. It takes no other arguments. |
| `--version` | Print `muhtari-run` and the version. It takes no other arguments. |

Anything else on the command line, including a file name, is a usage error.

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

## Stopping

SIGINT (Ctrl+C) or SIGTERM ends the run after the frame in progress. That frame's hash and PNG are still written,
`--bench` reports the frames that ran, output is flushed and the exit status is 0. A signal that was already
ignored when the runner started, as under `nohup`, stays ignored.

## Exit status

`muhtari-run --help` prints this table:

```text
  0  done     every frame ran, or SIGINT or SIGTERM stopped the run after a whole frame
  1  failure  a file could not be written, memory ran out or the core failed
  2  usage    the command line is invalid, so nothing ran
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
