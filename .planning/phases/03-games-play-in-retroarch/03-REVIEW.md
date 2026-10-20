---
phase: 03-games-play-in-retroarch
reviewed: 2026-10-20T07:17:37Z
depth: standard
files_reviewed: 136
files_reviewed_list:
  - .github/workflows/ci.yml
  - .github/workflows/nightly.yml
  - .gitignore
  - CMakeLists.txt
  - CMakePresets.json
  - README.md
  - docs/compatibility.md
  - docs/embedding.md
  - docs/emulation-tests.md
  - docs/hardware/bus.md
  - docs/hardware/cartridge.md
  - docs/hardware/controllers.md
  - docs/hardware/maria.md
  - docs/hardware/riot.md
  - docs/hardware/sally.md
  - docs/libretro.md
  - docs/runner.md
  - docs/testing.md
  - host/status_text.c
  - include/muhtari.h
  - libretro/CMakeLists.txt
  - libretro/libretro.c
  - libretro/muhtari_libretro.info.in
  - pins.txt
  - runner/exit_status.h
  - runner/main.c
  - runner/movie.c
  - runner/movie.h
  - src/boot.c
  - src/bus.c
  - src/cart.c
  - src/cpu.c
  - src/frame.c
  - src/instance.c
  - src/internal.h
  - src/maria.c
  - src/ports.c
  - src/riot.c
  - src/tia.c
  - tests/abi/tests.cmake
  - tests/api/v2.c
  - tests/api/v3.c
  - tests/boot/test_boot.c
  - tests/boot/tests.cmake
  - tests/built/chain.c
  - tests/built/coverage.c
  - tests/built/coverage.txt
  - tests/built/expected.txt
  - tests/built/frames.cmake
  - tests/built/openbios.cmake
  - tests/built/tests.cmake
  - tests/bus/test_bus.c
  - tests/cart/test_cart.c
  - tests/cart/tests.cmake
  - tests/cart/write_images.c
  - tests/cmake/ci_required.cmake
  - tests/cmake/compat_ratchet.cmake
  - tests/cmake/firmware_scan.cmake
  - tests/cmake/info_claims.cmake
  - tests/cmake/perf_compare.cmake
  - tests/compat/allow.txt
  - tests/compat/ledger.csv
  - tests/compat/library.cmake
  - tests/compat/pin.cmake
  - tests/compat/predicate.c
  - tests/compat/targets.cmake
  - tests/compat/tests.cmake
  - tests/core/test_core.c
  - tests/core/tests.cmake
  - tests/diag/assemble.cmake
  - tests/diag/bank6.a78
  - tests/diag/bank6.cfg
  - tests/diag/bank6.s
  - tests/diag/cpu.a78
  - tests/diag/cpu.cfg
  - tests/diag/cpu.s
  - tests/diag/dli.a78
  - tests/diag/dli.cfg
  - tests/diag/dli.s
  - tests/diag/dma.a78
  - tests/diag/dma.cfg
  - tests/diag/dma.s
  - tests/diag/driver.c
  - tests/diag/expected.csv
  - tests/diag/lib.s
  - tests/diag/linear16.a78
  - tests/diag/linear16.cfg
  - tests/diag/linear16.s
  - tests/diag/modes.a78
  - tests/diag/modes.cfg
  - tests/diag/modes.s
  - tests/diag/sg144.a78
  - tests/diag/sg144.cfg
  - tests/diag/sg144.s
  - tests/diag/stats.csv
  - tests/diag/tests.cmake
  - tests/frame/test_frame.c
  - tests/frame/tests.cmake
  - tests/fuzz/loader.c
  - tests/fuzz/run.c
  - tests/fuzz/run_cart.h
  - tests/fuzz/run_seeds.c
  - tests/fuzz/tests.cmake
  - tests/header/header.c
  - tests/header/header.cpp
  - tests/hygiene/tests.cmake
  - tests/input/test_input.c
  - tests/input/tests.cmake
  - tests/inventory/ci.txt
  - tests/inventory/fuzz.txt
  - tests/inventory/library.txt
  - tests/inventory/nofp.txt
  - tests/inventory/wasm32.txt
  - tests/libretro/host.c
  - tests/libretro/rcheevos.c
  - tests/libretro/tests.cmake
  - tests/maria/test_maria.c
  - tests/maria/tests.cmake
  - tests/perf/tests.cmake
  - tests/policy/tests.cmake
  - tests/prop.h
  - tests/prop/test_prop.c
  - tests/prop/tests.cmake
  - tests/release/firmware.txt
  - tests/retroarch/run_retroarch.cmake
  - tests/riot/test_riot.c
  - tests/roms/manifest.txt
  - tests/runner/boot_to_play.c
  - tests/runner/boot_to_play.cmake
  - tests/runner/png_read.c
  - tests/runner/png_read.h
  - tests/runner/test_movie.c
  - tests/runner/test_png.c
  - tests/runner/tests.cmake
  - tests/tests.cmake
  - tools/compat_doc.cmake
findings:
  critical: 0
  warning: 5
  info: 5
  total: 10
status: issues_found
---

# Phase 3: Code Review Report

**Reviewed:** 2026-10-20T07:17:37Z
**Depth:** standard
**Files Reviewed:** 136 (the shipped code in full; tests, CI and tooling sampled)
**Status:** issues_found

## Summary

Every shipped source file in scope was read in full: `src/` (instance, boot, frame, bus, maria, cart, ports,
riot and tia, plus the cpu.c diff), `include/muhtari.h`, `runner/` (main, movie, exit_status), `libretro/libretro.c`
and `host/status_text.c`. The review traced the hostile-ROM paths through the code: the run loop's bound
(WSYNC, RDY and HALT holds, and a JSR/BRK or NMI push landing on `$0124`/`$0102`), MARIA's DMA walk, line-RAM
and row indexing, the banking arithmetic, the BIOS mapping, A78 header parsing, the movie parser, and the
libretro memory map's lifetimes and ordering. Of the tests and tooling, the following were read: `tests/diag/driver.c`,
`tests/fuzz/run.c` and `run_cart.h`, `tests/runner/png_read.c`, `tests/compat/predicate.c` (header and parser),
`tests/cmake/compat_ratchet.cmake`, the `perf_compare.cmake` diff, the CI and nightly workflow diffs, and the
`tests/roms/manifest.txt` rows. The SHA-256 of each of the seven `.a78` fixtures matches its manifest row. The
remaining test files were in scope but were not read line by line.

No BLOCKER was found. The integer-only, all-in-instance and big-endian/32-bit invariants hold in the code read:
byte-wise header decoding, `size_t` row offsets bounded by `MUHTARI__PITCH_MAX`, cart RAM indexing masked by
`ram_size - 1`, and `uint16_t` wrap on DL pointers. Every `muhtari_run_frame` returns within a few lines of its
deadline. Each read can be held at most about one line by WSYNC, plus a HALT burst that ends by LRC + 9. At most
two such reads fall in one step (an instruction, then the NMI entry), so the fuzz target's four-line `RUN_BOUND`
holds by construction.

The findings below are in two groups. The first is defence-in-depth gaps on the cartridge path, where memory
safety depends on the generated table staying consistent. The second is contract text that disagrees with the
behaviour, and gaps in host-facing error handling.

## Warnings

### WR-01: A headerless image matched by hash skips the board geometry check, and the banking code assumes it passed

**File:** `src/cart.c:625-640` (consumers: `src/cart.c:799-816`, `src/cart.c:844`, `src/cart.c:879-884`)
**Issue:** The header path checks `cart_fits(&b, mapped)` after a table match overrides the board
(`cart.c:569`). The headerless path returns OK straight from `cart_entry_board()` with no geometry check. The
mapper code then relies on that geometry with unchecked arithmetic:
- `cart_sg_bank()` returns `banks - 2u` for `$4000` and `banks - 1u` for `$C000`. On a SuperGame entry whose
  image is under 32 KiB, `banks` is 0 or 1, so the return wraps to about `0xFFFFFFFF`, and
  `c->rom[bank * 0x4000 + ...]` at `cart.c:844` reads out of bounds.
- With `MUHTARI_BOARD_ROM_AT_4000` and `banks == 1`, `v % (banks - 1u)` at `cart.c:881` divides by zero
  (SIGFPE) on the first write to `$8000-$BFFF`.
- A LINEAR entry over 64 KiB makes `0x10000u - rom_size` wrap. Peeks then return open bus silently, so that case
  is safe but wrong.

Today this is reachable only if `src/cartdb.c` (generated from MAME's lists) ever pairs a SuperGame board with a
short image. The table does not record sizes, and no test pins that invariant, so memory safety rests on
unchecked generated data.
**Fix:** Apply the same check as the header path before accepting the match:
```c
cart_entry_board(e, &b);
cart_set_board(out, &b);
...
if (cart_refused(&b)) {
    return MUHTARI_STATUS_ERR_UNSUPPORTED_BOARD;
}
if (!cart_fits(&b, n)) {
    return MUHTARI_STATUS_ERR_INVALID_IMAGE;
}
return MUHTARI_STATUS_OK;
```
Optionally, also guard `cart_sg_bank()`/`muhtari__cart_write()` with `banks >= 2` (or `banks >= 3` with
ROM_AT_4000) as an assertion-free clamp.

### WR-02: The public header's contract comments contradict the behaviour a host will see

**File:** `include/muhtari.h:69`, `include/muhtari.h:211`
**Issue:** These two comments promise what the core does not do:
- `audio_count /* out: samples this frame (800 or 801) */`. The first frame after a fast boot runs 8,190 ticks
  and yields 54 samples (`src/frame.c:44-53`, which says so). The first frame of a BIOS boot runs 117,174 ticks
  and yields 785. A host that asserts or sizes on 800/801, or that treats any other count as an error, misbehaves
  on every load.
- `uint8_t *data; /* stable from load to destroy; NULL when absent */`. The `MUHTARI_MEMORY_CART_RAM` span lives
  in the cartridge allocation, which `muhtari__cart_install` frees on the next load (`src/cart.c:756-760`).
  `docs/embedding.md:298-306` documents the exception, but the header states the opposite. A host that follows
  the header and caches the pointer across a reload holds a dangling pointer and can write after it is freed.

**Fix:** Keep the header terse, but make it correct. For example: `/* out: samples this frame, at most
max_audio_samples */`, and `/* until the next load or destroy; NULL when absent */`.

### WR-03: When the libretro core fails a frame, nothing reaches the screen or the frontend's frame pacing

**File:** `libretro/libretro.c:491-498`
**Issue:** If `muhtari_run_frame` fails (most visibly `MUHTARI_STATUS_ERR_BIOS_REJECTED_CART`, which stays until
Restart), `retro_run` returns without calling `video_cb` or the audio callbacks on every later frame. The only
signal is one log line. The user sees a frozen or black picture with no explanation, even though `lr_message`
exists for exactly this. libretro expects `video_cb` once per `retro_run`, with `NULL` data to duplicate the last
frame. Skipping it entirely leaves frame pacing and audio sync to the frontend's tolerance.
**Fix:** On the first failure, show a message as well as logging it. On every failed frame, still present a frame:
```c
if (st != MUHTARI_STATUS_OK) {
    if (!run_failed) {
        LR_LOG(RETRO_LOG_ERROR, "muhtari: frame failed: %s\n", muhtari__status_text(st));
        lr_message(muhtari__status_text(st));
        run_failed = true;
    }
    if (video_cb != NULL) {
        video_cb(NULL, LR_WIDTH, LR_HEIGHT, (size_t)LR_WIDTH * sizeof pixels[0]);
    }
    return;
}
```

### WR-04: The libretro BIOS lookup gives up early and misreports failures

**File:** `libretro/libretro.c:554-579`
**Issue:** `lr_read_bios` has four faults:
- (a) It returns after the first file that `fopen` accepts. If `7800 BIOS (U).rom` exists but cannot be used
  (wrong size, so `muhtari_load` returns `ERR_INVALID_BIOS`, or a read error), `7800 BIOS (E).rom` is never
  tried, although the option text promises "U or E".
- (b) A `malloc` failure returns NULL, so the caller logs and shows "no 7800 BIOS (U).rom or 7800 BIOS (E).rom
  in the system directory", which is false.
- (c) `fread`'s short count is not checked with `ferror`, so an I/O error passes on as a short image.
- (d) The `snprintf` result is ignored. A system directory near 4 KiB is silently truncated, and a different path
  is opened.

**Fix:** Check `snprintf`'s return value against `sizeof path` and skip the name on truncation. Treat `ferror(f)`
as "not usable". Go on to the next name when a candidate is unusable, including after `bios_valid` fails (or
call `muhtari_load` per candidate). Report out-of-memory separately from "no file".

### WR-05: A planning ID in core source breaks the project rule

**File:** `src/ports.c:58-59`
**Issue:** `frame sees one input (DEC.25).` CLAUDE.md, Style: "No planning IDs in code, test names or user docs."
Hardware comments must cite a document or a `.planning/preparation` section. A decision number from the
planning tree is neither, and it means nothing to a reader of the public repository. (`src/maria.c:34`'s
"Choice 3.B" names a section of `.planning/preparation/A7800-MARIA.md`, so it is the permitted form.)
**Fix:** Replace it with the behavioural source, for example `(docs/embedding.md, Input)`, which already
describes the VBLANK-start latch.

## Info

### IN-01: The latch for the first BIOS-boot frame is detected by a coincidence of lengths

**File:** `src/frame.c:64-67`
**Issue:** `elapsed == MUHTARI__VBLANK_START` is what tells the BIOS boot's first frame (power-on to the first
VBLANK, 117,174 ticks) apart from the fast boot's (8,190 ticks), which must not latch. Any future boot path
(PAL, a different hand-off point) whose first frame happens to have one of these lengths would change input
timing silently.
**Fix:** Record the intent explicitly: set a flag such as `m->latch_first_frame` in `muhtari__boot_bios` and
`power_cycle`, and test that flag instead of the tick count.

### IN-02: `muhtari__riot_peek` duplicates `muhtari__riot_read`'s dispatch

**File:** `src/riot.c:153-181`
**Issue:** The register decode (RS, A2, A0) is written twice. Only the flag-clearing side effect differs. A
future decode fix applied to one copy will make peek and read disagree, which the diagnostics and tests rely on.
**Fix:** Factor the decode into one helper that takes a `clear` flag, or make `riot_flags_read` and
`riot_timer_read` wrap pure value functions that peek calls.

### IN-03: A movie's `bios-sha256` is ignored silently when the movie boots fast

**File:** `runner/movie.c:204-206`, `runner/movie.c:257-271`, `runner/main.c:707-731`
**Issue:** `movie_header_end` requires `bios-sha256` when `boot bios` is set, but not the reverse. A movie that
names a BIOS hash with `boot fast`, or with no `boot` key, parses and replays without checking any BIOS, because
`read_bios` runs only with `--bios`. That combination is almost certainly a mis-authored movie.
**Fix:** In `movie_header_end`, refuse `bios-sha256` unless `boot bios` is set, with an error such as "bios-sha256
without boot bios", expected "a boot bios line".

### IN-04: The libretro core falls back to the PAL BIOS on an NTSC-only machine

**File:** `libretro/libretro.c:556`, `docs/libretro.md:53-61`
**Issue:** `7800 BIOS (E).rom` is booted under NTSC timing. No real console pairs the two: the PAL BIOS skips
the cartridge signature check and was written for PAL video. The choice is documented, so it is recorded here
only as a compatibility risk. A user with only the PAL file gets a boot path no hardware reference covers.
**Fix:** None required. If it causes reports, prefer fast boot over the PAL BIOS until PAL is emulated.

### IN-05: The fuzz run target's bound is justified empirically, not structurally

**File:** `tests/fuzz/run.c:21-24`
**Issue:** `RUN_BOUND` cites "the worst seen over 3,000 random cartridges ... 794 ticks". The bound does hold by
construction: at most two held reads per step (the instruction's, then the NMI entry's after a push hits
`$0124`), each up to about one line plus a burst. The comment does not say so, though. A later change that adds
a third held read per step would get no warning from the comment. This is a test-tooling finding, so per rule 7
it is a seed.
**Fix:** State the structural argument in the comment, and keep the empirical figure as corroboration.

---

_Reviewed: 2026-10-20T07:17:37Z_
_Reviewer: Claude (gsd-code-reviewer)_
_Depth: standard_
