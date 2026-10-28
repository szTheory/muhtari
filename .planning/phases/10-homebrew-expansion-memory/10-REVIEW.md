---
phase: 10-homebrew-expansion-memory
reviewed: 2026-10-28T00:00:00Z
depth: standard
files_reviewed: 85
files_reviewed_list:
  - .github/workflows/ci.yml
  - README.md
  - docs/bankset-tests.md
  - docs/compatibility.md
  - docs/embedding.md
  - docs/emulation-tests.md
  - docs/expansion-tests.md
  - docs/hardware/bus.md
  - docs/hardware/cartridge.md
  - docs/hardware/maria.md
  - docs/hardware/pokey.md
  - docs/libretro.md
  - docs/maintaining.md
  - docs/quality.md
  - docs/runner.md
  - docs/testing.md
  - host/status_text.c
  - include/muhtari.h
  - libretro/libretro.c
  - libretro/muhtari_libretro.info.in
  - runner/exit_status.h
  - runner/main.c
  - scripts/hygiene.sh
  - src/bus.c
  - src/cart.c
  - src/instance.c
  - src/internal.h
  - src/maria.c
  - src/ports.c
  - tests/abi/tests.cmake
  - tests/alloc.h
  - tests/api/v7.c
  - tests/audio/expected.txt
  - tests/audio/test_irq.c
  - tests/audio/test_pokey.c
  - tests/boot/test_boot.c
  - tests/built/expected.txt
  - tests/bus/test_bus.c
  - tests/cart/images.c
  - tests/cart/images.h
  - tests/cart/test_cart.c
  - tests/cmake/hashes.cmake
  - tests/cmake/lane_equality.cmake
  - tests/cmake/quality.cmake
  - tests/cmake/quality_evidence.cmake
  - tests/compat/ledger.csv
  - tests/core/test_core.c
  - tests/core/tests.cmake
  - tests/diag/bankset.a78
  - tests/diag/bankset.cfg
  - tests/diag/bankset.s
  - tests/diag/driver.c
  - tests/diag/expansion.a78
  - tests/diag/expansion.cfg
  - tests/diag/expansion.s
  - tests/diag/expected.csv
  - tests/diag/tests.cmake
  - tests/fuzz/loader.c
  - tests/fuzz/run.c
  - tests/fuzz/run_cart.h
  - tests/fuzz/run_seeds.c
  - tests/fuzz/seeds.c
  - tests/fuzz/seeds.h
  - tests/fuzz/tests.cmake
  - tests/header/header.c
  - tests/header/header.cpp
  - tests/hygiene/history_control.sh
  - tests/hygiene/tests.cmake
  - tests/inventory/ci.txt
  - tests/inventory/hygiene.txt
  - tests/inventory/nofp.txt
  - tests/inventory/wasm32.txt
  - tests/libretro/bios.c
  - tests/libretro/host.c
  - tests/libretro/rcheevos.c
  - tests/libretro/tests.cmake
  - tests/policy/quality.cmake
  - tests/policy/tests.cmake
  - tests/prop/test_prop.c
  - tests/roms/manifest.txt
  - tests/runner/png_read.c
  - tests/runner/png_read.h
  - tests/runner/test_save.c
  - tests/runner/tests.cmake
  - tools/compat_doc.cmake
findings:
  critical: 1
  warning: 2
  info: 4
  total: 7
status: issues_found
---

# Phase 10: Code Review Report

**Reviewed:** 2026-10-28
**Depth:** standard
**Files Reviewed:** 85
**Status:** issues_found

## Summary

The core sources (`src/cart.c`, `src/instance.c`, `src/bus.c`, `src/maria.c`, `src/ports.c`,
`include/muhtari.h`, `src/internal.h`) were read in full against the phase diff, with the
requested focus on bounds, 32-bit and big-endian arithmetic, borrowed-media lifetime,
transactional load and expansion RAM, POKEY and HSC decode. I found no defect in the
core.

What I checked and traced, with no defect found:

- Every BankSet and expansion RAM index is bounded. XBoard indexes at most `7*16384+16383`
  and XM at most `15*8192+8191` into the 131072-byte block. BankSet offsets stay inside one
  half of the image. `cart_fits` rejects odd and over-1 MiB sizes and the RAM matrix before
  allocation. Bank registers are reduced modulo a power-of-two bank count. The `$C000`
  HALT-RAM store lands at `CART_BANK + 0..0x3FFF` of a 32 KiB RAM that `cart_fits` guarantees.
- Size arithmetic uses `size_t` with `SIZE_MAX` guards in `muhtari__cart_install`. The
  allocation length is recorded in `allocation_bytes` and used for both replacement and
  destroy.
- `muhtari_load` stages the BIOS, HSC firmware and image copy before releasing any live
  storage. `cart_install` memcpys from the image before freeing the old block, so borrowed
  media pointing at live ROM or expansion RAM stays safe. Nothing fallible runs after
  `cart_install`.
- `maria_from` and `maria_until` ordering in `bus.c`: the sentinel `halt_until` is installed at
  c1 (in `end()` or `advance()`) before `halt_cycle` runs the burst, and the true `HALTRST` is
  stored after the burst. Fetches inside the burst therefore see an open interval. The VBE
  fetch outside a HALT window selects CPU memory, as `maria.md` and `bus.md` state.
- The new struct layout assertions hold on 32-bit and 64-bit pointers (`muhtari_media` is 48
  bytes with 4-byte pointers and 80 with 8-byte pointers; `muhtari_info` is 44). The sound-bit
  name table matches the new `MUHTARI_SOUND_ADPCM` bit.
- `.gitattributes` forces `eol=lf`, so the `.s` and `.cfg` digests in `hashes.cmake` are
  stable across lanes. The `tests/diag/bankset.a78` and `expansion.a78` SHA-256 values match
  `tests/roms/manifest.txt`.

The defects are in the runner and libretro host paths and in the hygiene gate. One breaks a
documented CLI exit-status contract. Two are misleading diagnostics and an ineffective fix.
Four are minor.

## Critical Issues

### CR-01: `--hsc-rom` with a refused image exits 1 with the wrong message instead of 77

**File:** `runner/main.c:1182-1190`
**Issue:** The new pre-check in `load()` runs `muhtari_probe` and then rejects any
`--hsc-rom` request with `!attached || probed != MUHTARI_STATUS_OK`. When the probe is not OK
(a 2600 image, an unsupported board, a refused BankSet, an invalid or truncated image), the
runner prints `HSC firmware requires attached HSC` and returns `MUHTARI__EXIT_FAILURE` (1).
Before this change the request reached `muhtari_load`, which returned the real status, and the
runner mapped `UNSUPPORTED_2600`, `UNSUPPORTED_BOARD`, `UNSUPPORTED_REGION` and now
`UNSUPPORTED_DEVICE` to `MUHTARI__EXIT_REFUSED` (77). `runner/exit_status.h` and
`docs/runner.md` document 77 for these cases. The message is also false: the HSC may be
attached, and the image is what was refused. Scripts and CI that key off exit 77 for
"unsupported cartridge" now see exit 1 whenever `--hsc-rom` is present. The same `probed`
conflation hides a real 2600 refusal behind an HSC message.
**Fix:** Only enforce the attachment rule when the probe succeeded, and let `muhtari_load`
report every other status:
```c
if (opt->hsc_rom != NULL) {
    if (probed == MUHTARI_STATUS_OK && !attached) {
        fprintf(stderr, "muhtari-run: HSC firmware requires attached HSC\n");
        free(image);
        free(bios);
        return MUHTARI__EXIT_FAILURE;
    }
    /* read firmware as before; a failed probe falls through to
       muhtari_load, which returns the canonical refusal */
}
```
Add a runner test that passes `--hsc-rom` with a 2600 or unsupported-board image and expects
exit 77.

## Warnings

### WR-01: `UNSUPPORTED_DEVICE` diagnostics blame sound hardware for controller and storage refusals

**File:** `runner/main.c:1256-1262`, `libretro/libretro.c:1274-1294`
**Issue:** `MUHTARI_STATUS_ERR_UNSUPPORTED_DEVICE` has several causes. Per
`docs/embedding.md:279,450,617` these are unsupported sound topology, two EEPROM
attachments, an explicit controller device outside the legacy set, and a change to load-frozen
EEPROM topology. Both hosts now treat every `UNSUPPORTED_DEVICE` as a sound refusal.

- The runner prints `unavailable sound configuration: <bit>` for every set bit of
  `cart.sound`, including supported POKEY bits.
- The libretro core logs the same line. It falls back from the unsupported-sound mask to all
  of `cart.sound` when that mask is empty.

A cartridge with `pokey_450` loaded with two SaveKey attachments, or with an unsupported
explicit port device, therefore reports `unavailable sound configuration: pokey_450`. That is
wrong and misdirects the user. `host/status_text.c` also changed the base text from
`unsupported controller` to `unsupported device`, so the status line no longer narrows it
either.
**Fix:** Name a sound bit only when the cartridge actually declares an unsupported one, or
when the expansion and primary-owner rule from `muhtari_load` applies. Do not fall back to all
of `cart.sound`. For example:
```c
uint32_t refused = cart.sound & (MUHTARI_SOUND_YM2151 | MUHTARI_SOUND_COVOX | MUHTARI_SOUND_ADPCM);
if (refused == 0u && expansion_effective != MUHTARI_EXPANSION_OFF)
    refused = cart.sound & (MUHTARI_SOUND_POKEY_4000 | MUHTARI_SOUND_POKEY_800);
/* print only `refused`; otherwise print the generic "unsupported device" */
```
Share the mask computation between the runner and libretro so they cannot drift. Add a test
for a two-EEPROM refusal on a cartridge that declares POKEY.

### WR-02: History-scan fix is incomplete, and its control test avoids the case it claims to cover

**File:** `scripts/hygiene.sh:175-185`, `tests/hygiene/history_control.sh:13-26`
**Issue:** The change adds `--diff-filter=d` to the numstat scan so a commit that deletes a
listed binary and its manifest row passes. The later `--history` loop, `git log "$@"
--name-only ... | grep -i -E "$ROM_NAME"` followed by `in_manifest "$n"`, still lists deleted
paths. A deleted `*.a78`, `*.bin`, `*.sav` or similar is therefore still reported as
`game or BIOS image by name ... (in history)` once its manifest row is gone. I reproduced
this with a throwaway repo that has `fixture.a78` and its row in one commit, then deletes both:
```
hygiene: game or BIOS image by name: fixture.a78 (in history)
exit=1
```
`history_control.sh` uses `fixture.dat`, which does not match `ROM_NAME`, so the control test
passes while the real fixture types still fail. The check names a fault (a commit that deletes
a listed binary) that it does not actually prevent, and the test is not representative. This
contradicts the project rule that a changed check must exercise representative pass and fault
behaviour.
**Fix:** Apply the same exclusion to the name scan, for example
`git log "$@" --name-only --diff-filter=d --format=`. Then change the control fixture to
`fixture.a78` (or run both a `.dat` and a `.a78` case) so that the by-name path is covered.
The pass and fail branches should both go through `ROM_NAME`.

## Info

### IN-01: Seed index thresholds are hard-coded and coupled to table length

**File:** `tests/fuzz/seeds.c:279`, `tests/fuzz/run_seeds.c:14,175,196-200`
**Issue:** `seeds_build` guards its self-checks with `i >= 40u`, which is the index of
`bankset-shared`. `run_seeds.c` derives the profile from `(i - 38u) % 14u`, `/ 14u` and
`/ 42u`, and `RUN_SEEDS` is a literal 123. Adding or removing one earlier seed silently shifts
the self-check range or the BankSet profile, region and expansion mapping.
**Fix:** Name the base indices (`#define SEEDS_BANKSET_FIRST 40u`, `RUN_BANKSET_FIRST 38u`,
`RUN_BANKSET_PROFILES 14u`) and add `_Static_assert` checks that tie them to the table size.

### IN-02: `bankset_native_judge` reads the last character of an unchecked name

**File:** `tests/diag/driver.c` (`bankset_native_judge`, `strlen(bankset_name) - 1u`)
**Issue:** `bankset_name[strlen(bankset_name) - 1u]` indexes `[SIZE_MAX]` if the name is empty.
Callers set the name first today, but the function relies on that silently.
**Fix:** Return failure if `bankset_name[0] == '\0'` before indexing.

### IN-03: The CI "identity" line is a constant, not an observation

**File:** `.github/workflows/ci.yml:212-216`, `tests/cmake/hashes.cmake` (`hashes_lane_identity`)
**Issue:** `identity.txt` appends the literal string
`Release warnings=error tests=ON libretro=ON short_runs=OFF inventory=ci normal+release`.
Both producer and checker write the same text, so the comparison cannot detect a lane built
with different options. Only the `git ls-tree` part is a real binding.
**Fix:** Either read the options from the lane's `CMakeCache.txt` or preset, or document the line
as a label that provides no configuration binding.

### IN-04: A refused 52 KiB BankSet plus HSC reports a board refusal

**File:** `src/instance.c` (`battery_prepare`), `runner/main.c:1252`, `libretro/libretro.c:1271`
**Issue:** A 52 KiB BankSet loaded with HSC attached (Auto with an HSC request, XM Auto, or On)
returns `UNSUPPORTED_BOARD`. Both hosts then print `unsupported board: bankset`, although BankSet
is supported and the fix is `--hsc off`. `docs/runner.md` explains this, but the message does not.
**Fix:** Add a hint in the host message when `cart.board == MUHTARI_BOARD_BANKSET` and
`cart.rom_size == 106496u`, such as `52 KiB BankSet cannot share $3000 with an HSC; use HSC Off`.

---

_Reviewed: 2026-10-28_
_Reviewer: Claude (gsd-code-reviewer)_
_Depth: standard_
