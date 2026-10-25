---
phase: 09-pal-games-and-audio
reviewed: 2026-10-25T21:02:07Z
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
  critical: 0
  warning: 0
  info: 0
  total: 0
status: clean
---

# Phase 9: Code Review Report

**Reviewed:** 2026-10-25T21:02:07Z  
**Depth:** standard  
**Files Reviewed:** 65  
**Status:** clean

## Summary

Reviewed the explicit 65-path phase scope against baseline
`2cbbd6a78dabe2bc098615d49f7118231cc05626`, with call-chain inspection of
cartridge installation, bus holds, CPU interrupt/reset entry and source clocks.
The scope resolver reports `degraded` / `phase-range` / `no-task-commit-rows`;
the supplied explicit list and phase-range union agree, with no missing paths.
This scope fallback is disclosed, not treated as a source defect.

The original review is preserved at `c8e57aa`. This final verdict carries forward
that 65-path review and independently rechecks fix-round 1 commit
`af9dc83f9ff68ea14c5a007218a3477a3c2f2218`, covering its eight changed paths,
the loader's candidate/install call chain and the regression-test helpers.
No source changes followed that commit; only the fix report was added.

## Narrative Findings (AI reviewer)

No open findings. CR-01 is resolved by `af9dc83`: `src/instance.c:346-351`
consumes the validated BIOS into private 16 KiB staging before installation can
release cartridge RAM or overwrite battery/HSC storage. `src/instance.c:375-379`
updates the live BIOS only after the final fallible cartridge allocation has
succeeded. All accepted BIOS sizes fit staging, simultaneous HSC and BIOS
candidates use separate scratch arrays, and failed loads leave live firmware,
effective region and published state intact.

The new `bios.load_aliases` test independently supplies synthetic firmware with
different old/new execution markers. Its allocator poisons and quarantines the
released cartridge allocation, making the original stale read observable rather
than relying on malloc reuse. Ten combinations cover NTSC/PAL, 4/16 KiB BIOSes
borrowed from cartridge RAM and EEPROM, and the supported 4 KiB HSC ROM span.
Checks include invalid media, allocation failure, live-state and borrowed-byte
preservation, exact installed bytes, reset PC, allocation/free counts and actual
old/new firmware execution across power cycles. Its state comparison excludes
only unpublished firmware staging. Both portable inventories register the case.

The ENGINEERING section 11 rationale in [09-REVIEW-FIX.md](09-REVIEW-FIX.md)
addresses the existing loader ownership, separate candidate lifetimes, the
measured 16,384-byte instance increase and the wider interface burden of splitting
cartridge allocation/commit. The fix adds no dependency, large stack object,
load allocation or per-frame work. No new material defect was identified in the
fix or its tests and documentation.

The fix report at `b1f4aa9` records meaningful RED/GREEN evidence, failed attempts
and local 654-test/package/release/JUnit/hygiene acceptance. This reviewer inspected
the code and evidence report without rerunning tests. Earlier hosted runs do not
establish acceptance of the new source; refreshed hosted evidence remains the
orchestrator's responsibility. The existing advisory property timeout remains
unaccepted evidence. All reviewed files meet the reviewed contracts; no open
issues were found.

---

_Reviewer: the agent (gsd-code-reviewer)_  
_Depth: standard_
