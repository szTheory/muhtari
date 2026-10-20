# Emulation tests

The tests that check the emulated hardware, one section per CTest label. Each says what its cases cover and where
their expected values come from. Building, running and the other checks are in `docs/testing.md`.

Run one label with `ctest --test-dir build/ci -L <label> --output-on-failure`.

## maria

`maria.dma_line.*` runs MARIA's DMA burst with no CPU. Each case writes a DLL and display lists into system RAM, sets
MARIA's registers and calls the burst directly, then checks the HALTRST tick, the line's statistics, the line-RAM
cells and the feature mask. Every expected value is worked out from the constants in `docs/hardware/maria.md` before
the case first ran, with the arithmetic in a comment beside it.

| Case | Covers |
|------|--------|
| `empty_dl` | a terminator-only DL: T0 + 9 on an ordinary line, T0 + 15 on a last line, and no cell written |
| `headers` | a 4-byte and a 5-byte header with one graphics byte: 8 and 10 ticks |
| `widths` | widths 1, 8, 31 and 32 at 3 ticks a byte |
| `indirect` | 1-byte and 2-byte characters at 6 and 9 ticks, and the cells each writes |
| `holes` | H16 and H8 holes, the OFFSET-added test, a hole mid-object, and the indirect hole slot |
| `terminators` | `(b1 & 0x5F) == 0` ends the list; $40, $60, $C0 and $E0 are 5-byte headers that load WM |
| `last_line` | 6 more ticks on a zone's last line, the next entry read and its DLI |
| `abort` | an overrun dropped at LRC: the +3 and +9 tails, the objects kept and the partial one's cells |
| `line_ram` | the cells for 160A, 160B, 320A, 320B, 320C and 320D, kangaroo mode, hpos wrap and WM inheritance |
| `stats` | cost, bytes, headers and objects for a mixed DL |
| `feature_mask` | each feature bit sets on the first line that uses it and stays set |

## frame

`frame.*` runs whole frames through the run loop with SALLY, the bus and MARIA together. Each case but
`frame.sample_renders` builds a 48 KiB linear cartridge in memory from a few bytes of its own 6502 code, loads it
through the internal load and the fast-boot hand-off, and either runs frames or acts as the CPU on the bus to reach
an exact tick. Expected ticks come
from the beam in `docs/hardware/bus.md` and the line events in `docs/hardware/maria.md`.

| Case | Covers |
|------|--------|
| `frame.jam_ends` | a jammed CPU still ends every frame at VBLANK start, within one 4-tick read |
| `frame.dma_off_ends` | with DMA off every line 17-258 says so and every frame ends on time |
| `frame.hold_split_wsync_across_dma` | a TIA WSYNC held across t 28 and t 42 still runs the line's burst and moves MSTAT |
| `frame.halt_c1_write_visible` | a store in the HALT window's c1 is fetched by that line's burst; one in c2 is not |
| `frame.maria_wsync_releases_at_lrc` | MARIA's WSYNC holds until the next t 0, wire-ANDed with the TIA's |
| `frame.mstat_edges` | MSTAT's VBLANK sets at line 258, t 42 and clears at line 16, t 42 |
| `frame.handoff_state` | the hand-off's CPU, INPTCTRL, MARIA and beam state |
| `frame.first_frame_partial` | frame 1 is 8,190 ticks and 54 samples, with rows 0-223 written $00 |
| `frame.dma_blocked_flag` | a refused HALT request marks the line DMA blocked, at zero cost |
| `frame.dli_queued` | a DLI on zone 1's entry enters the NMI handler on line 32, zone 0's last, and the line says the edge was queued |
| `frame.null_video_same_state` | ten frames with and without video leave the same line RAM, statistics, CPU and RAM |
| `frame.sample_renders` | 7800basic's `simple` sample, built by `built.samples`: two instances give the same 120 frames, and frame 120 holds more than one colour and is not the test card |

`frame.sample_renders` carries the `built` label, because it needs the sample the pinned 7800basic builds.
