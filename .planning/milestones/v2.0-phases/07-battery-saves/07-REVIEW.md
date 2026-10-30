---
phase: 07-battery-saves
reviewed: 2026-10-24T14:13:00Z
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
  critical: 0
  warning: 0
  info: 0
  total: 0
status: clean
---

# Phase 7: Code Review Report

**Reviewed:** 2026-10-24T14:13:00Z
**Depth:** standard
**Files Reviewed:** 78
**Status:** clean

## Summary

Final re-review after fix round 2: CR-01, CR-02 and CR-03 are resolved. No unresolved bugs, security vulnerabilities or blocking quality defects were found in the reviewed scope. All reviewed files meet quality standards. No issues found.

The original 78-file text scope is preserved above, relative to `d35d33aae73968c96e7d1922120145eaf814ac4e`. Resolver status remains `degraded`, source `phase-range`, reason `no-task-commit-rows`, with no missing/outside-union/unreachable entries. Binary fixture contents were excluded; owned source/configuration, provenance and consumers were reviewed. Original findings and reproductions remain preserved in Git at `6ac3648` and `974347b`.

The current verified production source is `90b9b7a5f1f17f49f2133af17bb9e1e408fa21f1`, with the fix report committed at `dc69c0f`. This final pass read the round-2 fix/report, its changed tests/docs and the actual core/input/signal call chains affected by both rounds. Unaffected source was not redundantly reread. The source-diff check against `90b9b7a` returned no paths. This reviewer made no source edits or commits.

## Narrative Findings (AI reviewer)

No open findings. Current counts are critical 0, warning 0, info 0.

## Resolved findings and evidence

### CR-01 resolution: Final save survives native broken output pipes

**Original classification:** BLOCKER. **Disposition:** resolved by `f8cc10f`, retained in final source.

Native POSIX runner initialization ignores SIGPIPE so broken output returns through the normal error path. The runner still flushes dirty valid battery state before destruction and returns exit 1; it does not treat an output error as successful execution. The policy stays in the runner, with Windows and WASI handling unchanged. SIGINT/SIGTERM/SIGUSR1 inherited-ignore semantics and consume-before-I/O save requests remain intact.

The actual native child regression passes for default and inherited-ignored SIGPIPE, both final buffered flush and output filling the buffer during execution. It asserts ordinary exit 1 and independently checks all 34,816 saved bytes. The stop test also verifies ignored SIGINT/SIGTERM remain ignored. These consumers were rerun successfully in the final targeted command below.

### CR-02 resolution: Deferred storage changes retain joystick A-button fire

**Original classification:** BLOCKER. **Disposition:** resolved by `bd7e686`, retained and corrected by `90b9b7a`.

The adapter derives button translation from the same effective port selection sent to the core. An installed EEPROM still takes priority; a refused late SaveKey/AtariVox request on an ordinary port retains its loaded choice. RetroPad A therefore continues to fire a retained one-button joystick.

The actual loaded-module regression passes for both ports, both EEPROM identities, controller callbacks and battery options, absent and HSC storage, with A held throughout. It checks the cartridge's fire samples, one warning per changed reload-required request, no repeated warning for unchanged requests, and unchanged save pointer/length/full HSC contents. The round-2 Auto repair does not bypass this shared button/device path.

### CR-03 resolution: Auto retains unsupported-header fallback

**Original classification:** BLOCKER. **Disposition:** resolved by `90b9b7a`.

`lr_active_port` now matches `muhtari__ports_device`: automatic Joystick stays Joystick; a loaded cartridge's None is unplugged; other recognized ordinary header devices use ProLine. Unknown header bytes are first normalized to None by `cart_port`, so they also unplug. No-content Auto retains ProLine. Installed serial identity and accepted explicit ordinary selections retain their prior priority. Unimplemented header codes are no longer forwarded as explicit device requests, and the public API's explicit unsupported-device refusal remains unchanged.

The loaded-module test checks 112 combinations: every ordinary recognized header code plus unknown bytes 13/255, on each port, with frontend Auto/None/ProLine/unknown callback. It independently checks direction, Select, fire and button-2 samples while both pads hold Up+A; the actual adapter log must contain no input refusal. The earlier held-A/storage tests remain passing.

The original actual-module light-gun probe was rerun against the final built module. With Auto and a light-gun header on either port, held UP plus SELECT now produces SWCHA EF and SWCHB 3D, with no input-refusal log. The probe command `/tmp/muhtari-rereview-auto` exits 0, run URL N/A-local; its self-contained harness source and compile command are preserved in the prior review at `974347b`.

## Final re-review verification

Executed now, exit **0**, run URL **N/A-local**:

```sh
ctest --preset ci -R '^(runner[.]stop|runner[.]save[.]controls|libretro[.]host[.](layout|devices|input|battery|battery_order|xrgb8888|rgb565))$' --no-tests=error --output-on-failure
```

All ten selected tests pass, including the input fixture, native controls, both original input regressions, complete Auto-header matrix and both no-content pixel formats. The source-diff check against final source `90b9b7a5f1f17f49f2133af17bb9e1e408fa21f1` returned no paths; `git diff --check` exits 0.

The committed `07-REVIEW-FIX.md` records full local 641+6 CI, exact inventories, hygiene 24 and their commands/exit statuses for the final source. Those full suites were not redundantly rerun here. Earlier hosted [CI 40095781311](https://github.com/szTheory/muhtari/actions/runs/40095781311) and [Nightly 40095782965](https://github.com/szTheory/muhtari/actions/runs/40095782965) cover pre-fix source; they are not claimed as hosted validation of the current fixes. Final-source hosted acceptance remains with the parent workflow.

## Intent and ownership disposition

The original ten flagged assumption rows and descriptor-less prohibitions retain their evidence-based intent review; no serialized lifecycle status or owner interview was fabricated. The instance owns the raw span and bounded serial state, hosts own files, firmware remains outside saves, and public claims distinguish signature-only HSC, the selected 5 ms model and EEPROM-only AtariVox. The output-failure and ordinary-input defects identified by review are repaired. Exactly two logical hosted cases remain; no bound, inventory, pin, required/advisory status or comparison authority changed.

Both permitted review-fix rounds have been used. No further review-fix round, gap round, source change or additional GSD step is requested by this report. The fixer's generic logic-change verification flag is discharged by this agent's source review and automated semantic evidence; no owner UAT remains.

---

_Reviewer: gsd-code-reviewer_
_Depth: standard_
