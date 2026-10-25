---
phase: 09-pal-games-and-audio
reviewed: 2026-10-25T20:50:30Z
depth: standard
files_reviewed: 65
files_reviewed_list:
  - PROVENANCE.md
  - README.md
  - THIRD-PARTY-NOTICES.md
  - docs/compatibility.md
  - docs/embedding.md
  - docs/emulation-tests.md
  - docs/hardware/audio.md
  - docs/hardware/bus.md
  - docs/hardware/controllers.md
  - docs/hardware/maria.md
  - docs/libretro.md
  - docs/maintaining.md
  - docs/pal-tests.md
  - docs/quality.md
  - docs/runner.md
  - docs/testing.md
  - host/palette.c
  - host/palette.h
  - include/muhtari.h
  - libretro/libretro.c
  - libretro/muhtari_libretro.info.in
  - runner/main.c
  - runner/movie.c
  - runner/movie.h
  - src/boot.c
  - src/frame.c
  - src/instance.c
  - src/internal.h
  - src/maria.c
  - src/ports.c
  - src/synth.c
  - tests/api/v3.c
  - tests/audio/test_contracts.c
  - tests/audio/test_spectral.c
  - tests/audio/test_stress.c
  - tests/audio/tests.cmake
  - tests/boot/test_boot.c
  - tests/boot/tests.cmake
  - tests/cmake/hashes.cmake
  - tests/cmake/quality.cmake
  - tests/compat/ledger.csv
  - tests/core/test_core.c
  - tests/core/test_pal.c
  - tests/core/tests.cmake
  - tests/diag/driver.c
  - tests/diag/expected.csv
  - tests/diag/pal.a78
  - tests/diag/pal.cfg
  - tests/diag/pal.s
  - tests/diag/tests.cmake
  - tests/host/test_palette.c
  - tests/input/test_input.c
  - tests/input/tests.cmake
  - tests/inventory/ci.txt
  - tests/inventory/wasm32.txt
  - tests/libretro/host.c
  - tests/libretro/tests.cmake
  - tests/maria/test_maria.c
  - tests/policy/quality.cmake
  - tests/policy/tests.cmake
  - tests/roms/manifest.txt
  - tests/runner/test_movie.c
  - tests/runner/test_runner.c
  - tests/runner/tests.cmake
  - tools/compat_doc.cmake
findings:
  critical: 1
  warning: 0
  info: 0
  total: 1
status: issues_found
---

# Phase 9: Code Review Report

**Reviewed:** 2026-10-25T20:50:30Z  
**Depth:** standard  
**Files Reviewed:** 65  
**Status:** issues_found

## Summary

Reviewed the explicit 65-path phase scope against baseline
`2cbbd6a78dabe2bc098615d49f7118231cc05626`, with call-chain inspection of
cartridge installation, bus holds, CPU interrupt/reset entry and source clocks.
The scope resolver reports `degraded` / `phase-range` / `no-task-commit-rows`;
the supplied explicit list and phase-range union agree, with no missing paths.
This scope fallback is disclosed, not treated as a source defect.

One existing memory-safety/load-lifecycle defect remains in the reviewed loader.
It affects both regions and is not introduced by the PAL timing arithmetic.
AGENTS rule 7 keeps existing required-contract violations blocking wherever found.
No additional confirmed defect was identified in the selected timing, synthesis
bounds, host geometry/palette selection, immutable movie validation, PAL numeric
judges or narrow fixture-comparison changes.

This review did not rerun tests or treat earlier green runs as proof of source
correctness. The execution evidence and advisory property timeout retain the
authority recorded in `09-08-SUMMARY.md`; the timeout is not accepted evidence.

## Narrative Findings (AI reviewer)

## Critical Issues

### CR-01: BLOCKER — Load invalidates an aliased BIOS before consuming it

**File:** `src/instance.c:349-371`  
**Related:** `src/cart.c:805-806`, `src/ports.c:1051-1053`,
`include/muhtari.h:182-190`, `src/instance.c:354-364`

**Issue:** `muhtari_load` validates `media->bios` at lines 336–337 but retains
only its pointer. It then installs the new cartridge, freeing the old combined
ROM/RAM allocation, and clears/replaces battery and HSC storage. Only afterward,
at line 371, does it copy the BIOS. Valid public inputs can therefore become
invalid or change during the call itself:

- Load a cartridge with at least 4096 bytes of cartridge RAM, obtain that span
  through `muhtari_get_memory(..., MUHTARI_MEMORY_CART_RAM, ...)`, and fill its
  first 4096 bytes with owned synthetic firmware whose reset vector is `$F000`.
  Reload any valid cartridge with this span as `media.bios` and
  `media.bios_size = 4096`. BIOS validation succeeds, then cartridge installation
  frees that span before line 371 reads it: a use-after-free.
- The same firmware placed in an attached EEPROM's public battery span is
  overwritten with `$FF` at line 355 before the BIOS copy. Likewise, a BIOS
  borrowed from `muhtari_get_hsc_rom` is cleared/replaced at lines 354–364.
  The call returns success and boots bytes different from the validated input.

The public media contract says the BIOS is copied at load and imposes no
non-alias requirement. Exposed spans are usable inputs at call entry; the
loader must consume borrowed bytes before its own installation invalidates
them. The existing HSC staging already recognizes this problem for HSC firmware,
but it does not protect the BIOS.

**Impact:** A successful reload can read freed memory, crash, or silently boot
corrupted firmware. This violates memory safety and the validated-media/load
contract even though ordinary separate-buffer BIOS tests pass.

**Fix:** Preserve validated BIOS bytes before any operation that can invalidate
their source, while retaining transactional failure behavior. For example, use
bounded instance-owned unpublished BIOS staging, or separate candidate cartridge
allocation from its commit so the BIOS can be consumed after allocation succeeds
and before old cartridge/battery storage is released. Do not overwrite the live
BIOS before the last fallible operation; a rejected or allocation-failed reload
must retain the previous running firmware. Add focused regression cases with
synthetic BIOS bytes borrowed from old cartridge RAM, battery RAM and HSC ROM,
including allocation failure and successful reload in both regions. Use an
allocator free-poison/quarantine seam or ASan to expose the old-RAM read reliably.

---

_Reviewer: the agent (gsd-code-reviewer)_  
_Depth: standard_
