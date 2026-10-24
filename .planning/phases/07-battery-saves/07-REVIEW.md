---
phase: 07-battery-saves
reviewed: 2026-10-24T14:03:00Z
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
  critical: 1
  warning: 0
  info: 0
  total: 1
status: issues_found
---

# Phase 7: Code Review Report

**Reviewed:** 2026-10-24T14:03:00Z
**Depth:** standard
**Files Reviewed:** 78
**Status:** issues_found

## Summary

Re-review after fix round 1: both original reproductions are corrected. One new blocking regression remains in the frontend fix: automatic unsupported header controllers now cause every input update to be refused.

The original 78-file text scope is preserved above, relative to `d35d33aae73968c96e7d1922120145eaf814ac4e`. Resolver status remains `degraded`, source `phase-range`, reason `no-task-commit-rows`, with no missing/outside-union/unreachable entries. Binary fixture contents were excluded; owned source, configuration, provenance and consumers remain in scope. This pass examined commits `f8cc10f194285ddb9d46873519a0544503e81266` and `bd7e686d07fca10578be1e5258e3fde32f6a5815`, their changed tests/docs, and affected signal/input call chains. Unaffected source was not redundantly re-reviewed. The source-diff check against `bd7e686` returned no paths.

No source edits or commits were made by this reviewer. One of at most two phase review-fix rounds has been used. The original findings are preserved in Git at `6ac3648` and summarized below.

## Narrative Findings (AI reviewer)

### CR-03: Resolving Auto to unsupported header codes rejects the whole frontend input update

**Classification:** BLOCKER
**File:** `libretro/libretro.c:488-489`; related `libretro/libretro.c:584-596`, `src/ports.c:209-217,239-242,280-296`
**Issue:** The CR-02 fix changes `lr_active_port` from returning the automatic selector (0) to returning raw `cart.port1/port2`. Supported ROMs may declare light gun, paddles, driving controllers and other devices whose existing automatic behavior is a ProLine fallback; `docs/embedding.md:390-393` and `docs/hardware/controllers.md:19-21` promise that behavior. The core intentionally rejects those codes when passed as an explicit `muhtari_input.device`. As a result, the new adapter calls `muhtari_set_input` with e.g. LIGHTGUN (2), receives ERR_UNSUPPORTED_DEVICE every frame, and discards both ports' buttons and every console switch, even when only the other port is declared unsupported. The game loads and runs but ordinary inputs, Select/Reset/Pause and difficulty changes stop reaching it. This is a regression introduced by `bd7e686`, independent of future controller implementation.

**Evidence:** A temporary harness reused `tests/libretro/host.c` and its existing input cartridge/actual built module. For each port, it changed that cartridge's header controller byte (55 or 56) to MUHTARI_PORT_LIGHTGUN, kept frontend Auto, loaded through `retro_load_game`, held port 1 UP plus SELECT, and ran three frames. Compilation and execution exited 0; both cases repeatedly logged input refusal and produced SWCHA FF / SWCHB 3F, instead of the required EF / 3D. Run URL: N/A-local.

Reproduction source, supplied to `cc -x c - -std=c17 -O2 -I. -Iinclude -Ihost -Ilibretro -Irunner -x none tests/cart/images.c runner/sha256.c build/ci/libmuhtari.a build/ci/libmuhtari_host.a -o /tmp/muhtari-rereview-auto`, then execute `/tmp/muhtari-rereview-auto`:

```c
#define main original_host_main
#include "tests/libretro/host.c"
#undef main
int main(void)
{
    if (!module_open("build/ci/libretro/muhtari_libretro.dylib"))
        return 2;
    resolve_all();
    if (!read_file("build/ci/tests/input-carts/input.a78", 0))
        return 2;
    for (unsigned p = 0; p < 2; p++) {
        files[0][55 + p] = MUHTARI_PORT_LIGHTGUN;
        start(0);
        pad[0] = BIT(UP) | BIT(SELECT);
        run_frames(3);
        const uint8_t *r = last_entry();
        printf("port %u: SWCHA=%02x SWCHB=%02x; expected ef/3d\n",
               p + 1, r[E_SWCHA], r[E_SWCHB]);
        stop();
        files[0][55 + p] = MUHTARI_PORT_PROLINE;
    }
    module_close();
    return 0;
}
```

**Fix:** Preserve the core's existing automatic-selector semantics while translating buttons from the retained effective request, or normalize automatic metadata through the same supported-device/unplugged/ProLine-fallback rules before passing an explicit device. Do not pass unimplemented header codes as explicit requests or relax the public API's explicit-device refusal. Extend the actual loaded-module regression to automatic unsupported headers on either port, requiring working button and console-switch input, no per-frame input refusal, and retained held-A behavior for deferred storage changes.

## Original findings resolved by round 1

- **CR-01 — formerly BLOCKER; resolved.** Commit `f8cc10f` ignores native POSIX SIGPIPE in runner initialization. Broken writes now reach the existing error path, and `save_flush` still runs before valid state destruction. This does not add signal/I/O state to the core or change Windows/WASI behavior. The actual child-process regression covers default and inherited-ignored SIGPIPE, final buffered flush and a buffer filled during execution; it requires ordinary exit 1 and checks every one of 34,816 saved bytes. Stop/save signals retain their inherited-ignore handling; additional SIGINT/SIGTERM controls exercise it. No remaining defect was found in this fix.
- **CR-02 — formerly BLOCKER; original reproduction resolved.** Commit `bd7e686` passes the same resolved port to device selection and button translation. The loaded-module held-A regression exercises late SaveKey/AtariVox callbacks and options on either port, absent and HSC storage, reload warnings, unchanged span and intact contents. Those assertions pass. The separate Auto fallback regression caused by its metadata resolution is CR-03 above. The fixer's “requires human verification” marker is satisfied by this agent's semantic review and automated evidence; it creates no owner UAT.

## Re-review verification

Executed in this re-review, exit 0, run URL N/A-local:

```sh
ctest --preset ci -R '^(runner[.]stop|runner[.]save[.]controls|libretro[.]host[.](layout|devices|input|battery|battery_order))$' --no-tests=error --output-on-failure
```

All eight selected tests, including the input fixture, pass. They confirm the original failure repairs and retained tested signal/storage/input behavior; they do not cover automatic unsupported-header fallback. The additional actual-module probe above demonstrates that missing case. The source-diff check against tested source `bd7e686d07fca10578be1e5258e3fde32f6a5815` returned no paths.

The committed `07-REVIEW-FIX.md` records full local 641+6 CI, exact inventories, hygiene 24 and its commands/exit statuses. Those full suites were not redundantly rerun here. Earlier hosted [CI 40095781311](https://github.com/szTheory/muhtari/actions/runs/40095781311) and [Nightly 40095782965](https://github.com/szTheory/muhtari/actions/runs/40095782965) cover pre-fix source, not the current fix commits. No unsupported hosted-success claim is made.

## Intent and ownership review

The original ten flagged assumption rows and descriptor-less prohibitions retain their prior evidence-based intent review; no serialized lifecycle status was invented or changed. The instance remains the owner of the raw span and fixed serial page state; hosts own files. Firmware stays outside saves, stub/model/speech limitations remain explicit, and the two hosted cases remain DMA equivalence and battery roundtrip. CR-01's output-failure boundary is repaired. CR-03 violates the already required ordinary-input fallback contract and blocks acceptance wherever introduced; it does not require a controller redesign, expanded scope, new bounds, or owner verification.

---

_Reviewer: gsd-code-reviewer_
_Depth: standard_
