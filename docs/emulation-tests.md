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
