---
phase: 07-battery-saves
reviewed: 2026-10-24T13:52:00Z
depth: standard
files_reviewed: 78
files_reviewed_list:
  - .github/workflows/ci.yml
  - CMakeLists.txt
  - docs/embedding.md
  - docs/emulation-tests.md
  - docs/hardware/bus.md
  - docs/hardware/cartridge.md
  - docs/hardware/controllers.md
  - docs/hardware/riot.md
  - docs/libretro.md
  - docs/quality.md
  - docs/runner.md
  - docs/testing.md
  - host/status_text.c
  - include/muhtari.h
  - libretro/libretro.c
  - libretro/muhtari_libretro.info.in
  - runner/exit_status.h
  - runner/main.c
  - runner/movie.c
  - runner/movie.h
  - runner/stop.c
  - runner/stop.h
  - src/bus.c
  - src/cart.c
  - src/frame.c
  - src/instance.c
  - src/internal.h
  - src/ports.c
  - src/riot.c
  - tests/abi/tests.cmake
  - tests/api/v4-layout.c
  - tests/api/v5.c
  - tests/audio/expected.txt
  - tests/audio/test_contracts.c
  - tests/boot/test_boot.c
  - tests/built/chain.c
  - tests/built/expected.txt
  - tests/cmake/hashes.cmake
  - tests/cmake/info_claims.cmake
  - tests/cmake/quality.cmake
  - tests/core/test_core.c
  - tests/core/tests.cmake
  - tests/diag/battery.cfg
  - tests/diag/battery.s
  - tests/diag/driver.c
  - tests/diag/expected.csv
  - tests/diag/tests.cmake
  - tests/fuzz/i2c.c
  - tests/fuzz/replay_main.h
  - tests/fuzz/tests.cmake
  - tests/header/header.c
  - tests/header/header.cpp
  - tests/input/test_input.c
  - tests/input/tests.cmake
  - tests/install/tests.cmake
  - tests/inventory/ci.txt
  - tests/inventory/fuzz.txt
  - tests/inventory/nofp.txt
  - tests/inventory/wasm32.txt
  - tests/libretro/bios.c
  - tests/libretro/host.c
  - tests/libretro/rcheevos.c
  - tests/libretro/tests.cmake
  - tests/policy/quality.cmake
  - tests/policy/tests.cmake
  - tests/prop/test_prop.c
  - tests/prop/tests.cmake
  - tests/retroarch/bmp.c
  - tests/retroarch/compare.c
  - tests/retroarch/run_retroarch.cmake
  - tests/retroarch/test.cfg.in
  - tests/roms/manifest.txt
  - tests/runner/test_audio.c
  - tests/runner/test_control.c
  - tests/runner/test_movie.c
  - tests/runner/test_runner.c
  - tests/runner/test_save.c
  - tests/runner/tests.cmake
findings:
  critical: 2
  warning: 0
  info: 0
  total: 2
status: issues_found
---

# Phase 7: Code Review Report

**Reviewed:** 2026-10-24T13:52:00Z
**Depth:** standard
**Files Reviewed:** 78
**Status:** issues_found

## Summary

Two reproduced correctness defects remain: a native broken output pipe bypasses the runner's final save, and a refused late frontend storage request changes ordinary joystick button translation.

Scope is the explicit file array supplied by the phase evaluation resolver, relative to `d35d33aae73968c96e7d1922120145eaf814ac4e`. The resolver reports `degraded`, source `phase-range`, reason `no-task-commit-rows`; it reports no missing, outside-union or unreachable entries. The binary contents of `tests/diag/battery.a78` were excluded; its owned assembly/configuration, manifest, numeric expectations and consumers were reviewed. Production source matches `fae8e8fb5010b38965f00c090de6bd564a01500a`; the source-diff check against that commit returned no paths.

Review covered core load/storage ownership and ABI compatibility, bus/pin timing, EEPROM protocol/reset/generation, runner import/replacement/control/error paths, frontend publication and memory descriptors, diagnostic independence, scoped fixture/sample comparability, inventories and changed CI wiring. No source files were edited, no commits made, and no excluded upstream source opened.

## Narrative Findings (AI reviewer)

### CR-01: Broken stdout pipe terminates the runner before its required final save

**Classification:** BLOCKER
**File:** `runner/main.c:1384-1387`; `runner/stop.c:63-70`
**Issue:** The runner flushes stdout before calling its final `save_flush`, but native POSIX setup leaves SIGPIPE at its default terminating disposition. A consumer that closes the output pipe causes the write or `fflush(stdout)` to terminate the process instead of returning an error. Valid completed frames and dirty battery bytes therefore never reach the final save. This violates the existing D-14/public runner contract that an unrelated output error after valid frames still flushes battery state before destruction. It also occurs during earlier buffered output when enough hash lines are emitted; merely moving this one `fflush` does not address that case. The mocked stdout failure in `tests/runner/test_save.c` returns EOF and cannot reproduce the native signal.

**Evidence:** Run the following from the repository root. The Python command exits 0; its control runner exits 0 and creates a 34,816-byte save, while the closed-pipe runner exits -13 (SIGPIPE) and creates no save. Run URL: N/A-local.

```python
import pathlib, subprocess, tempfile
with tempfile.TemporaryDirectory() as directory:
    args = ["build/ci/runner/muhtari-run", "--frames", "120",
            "--hash-frame", "120", "--save-dir", directory,
            "--save-interval", "4294967295", "tests/diag/battery.a78"]
    result = subprocess.run(args, stdout=subprocess.DEVNULL,
                            stderr=subprocess.DEVNULL)
    save = pathlib.Path(directory, "battery.sav")
    assert result.returncode == 0 and save.stat().st_size == 34816
    save.unlink()
    child = subprocess.Popen(args, stdout=subprocess.PIPE,
                             stderr=subprocess.DEVNULL)
    child.stdout.close()
    assert child.wait(timeout=10) == -13 and not save.exists()
```

**Fix:** Ignore SIGPIPE in native POSIX runner initialization so broken writes return EPIPE through the existing output-error path, which must still call the dirty save path and return exit 1. Keep this host policy outside the core. Add a native child-process regression with an actually closed output pipe, asserting exit 1 and the complete expected final battery bytes; cover both end-of-run flush and output during a run.

### CR-02: A refused late save-device request drops joystick A-button fire

**Classification:** BLOCKER
**File:** `libretro/libretro.c:533-548`; related `libretro/libretro.c:484-488,588-591`
**Issue:** `lr_active_port` refuses a late SaveKey/AtariVox attachment by retaining `loaded_port[p]`, but `lr_buttons` independently translates RetroPad buttons from the raw `lr_port_device(p)` request. Load an ordinary joystick on a cartridge whose automatic controller is ProLine, hold RetroPad A, then request SaveKey on that port after load. The installed device remains `MUHTARI_PORT_JOYSTICK`, while A changes from `MUHTARI_BUTTON_1` to `MUHTARI_BUTTON_2`. The joystick reads only BUTTON_1, so its fire becomes released even though the requested storage change was refused pending reload. The same problem occurs through a late battery core option. It contradicts preserved ordinary joystick semantics and the frontend's promise to keep the installed attachment until reload.

**Evidence:** A temporary C probe included the current real adapter and linked the built core/host archives. With `loaded_port[0] = MUHTARI_PORT_JOYSTICK`, `cart.port1 = MUHTARI_PORT_PROLINE`, and an input callback holding only RetroPad A, changing `device[0]` from `LR_JOYSTICK` to `LR_SAVEKEY` changed the result from active device 5 / buttons 16 to active device 5 / buttons 32. Compilation and execution both exited 0. Run URL: N/A-local. The mismatch follows directly from the two cited production paths; `src/ports.c`'s joystick INPT handling tests BUTTON_1 only.

**Fix:** Resolve each port's effective device once in `lr_input` and use that same effective device for both `in.device[p]` and button translation, resolving automatic metadata consistently. Add a loaded-module regression that loads a joystick, holds A, requests late SaveKey/AtariVox through controller callbacks and core options, and verifies continued joystick fire plus the reload warning and unchanged save span.

## Intent and ownership review

The ten flagged assumption rows and descriptor-less prohibitions were assessed against actual implementation and public claims, without fabricating owner UAT or changing their serialized lifecycle status.

- SAVE-01/02/03: the instance owns the only persistent span and fixed serial page state; firmware remains outside saves. Public docs qualify the signature-only stub, selected 5 ms compatibility interval, EEPROM-only AtariVox and unavailable speech/OpenHSC routines. Firmware validation and old-layout clients were checked against their actual consumers.
- SAVE-04 idempotency/concurrency: staged import arms persistence only on success; clean generations avoid rewriting; naming and one-writer limitations are explicit.
- SAVE-05 concurrency: exclusive temporaries, bounded collisions, pre-replacement failure checks and consume-before-I/O requests preserve their stated boundaries. CR-01 identifies a missing native output-failure boundary.
- SAVE-06 boundary/precision/idempotency/concurrency: load-time publication, exact layout lengths, memory-map aliases, isolated launch directories and independent complete-media/counter-pixel consumers were examined. CR-02 identifies a frontend input inconsistency when a storage change is deferred. The two hosted logical cases remain DMA equivalence and battery roundtrip.

Final acceptance runs supplied by the orchestrator are [CI 40095781311](https://github.com/szTheory/muhtari/actions/runs/40095781311) and [Nightly 40095782965](https://github.com/szTheory/muhtari/actions/runs/40095782965). Their prior success does not exercise or negate these two reproduced defects. The targeted probes above are additional local review evidence; this review does not claim to have rerun the full platform matrix.

---

_Reviewer: gsd-code-reviewer_
_Depth: standard_

