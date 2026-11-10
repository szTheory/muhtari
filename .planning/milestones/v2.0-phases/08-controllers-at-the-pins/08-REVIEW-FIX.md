---
phase: 08-controllers-at-the-pins
fixed_at: 2026-10-25T03:11:20Z
review_path: .planning/phases/08-controllers-at-the-pins/08-REVIEW.md
iteration: 1
findings_in_scope: 2
fixed: 2
skipped: 0
status: all_fixed
---

# Phase 8: Code Review Fix Report

**Fixed at:** 2026-10-25T03:11:20Z
**Source review:** .planning/phases/08-controllers-at-the-pins/08-REVIEW.md
**Iteration:** 1 of at most 2 code-review fix rounds

**Summary:**

- Findings in scope: 2
- Fixed: 2
- Skipped: 0

## Fixed Issues

### CR-01: Complete short gun aim is incorrectly forced off-screen

**Status:** fixed
**Files modified:** `src/ports.c`, `tests/input/test_input.c`
**Commit:** `ac76249cc984cd848b8eb07ad6765e5b9f6c0c35`
**Applied fix:** The missing-aim fallback now ends at `gun_y`, rather than the
end of the port block. Complete aim with an absent flag retains neutral zero;
incomplete aim remains off-screen. No ABI, packet layout or full-packet behavior
changed.

The existing `input.lightgun` case now exercises port-0 sizes 36/40/44 and
port-1 sizes 76/80/84, plus an explicit off-screen control. Each runs with an
explicit gun, Auto from a load override, and Auto from the cartridge header:
24 cases total. Assertions check pending and applied normalization, then actual
illumination and D6 capture through three `muhtari_run_frame` calls with bright
native background and a jammed CPU. A subsequent input-register read must show
light for complete aim with absent or explicit-zero flags, and remain high for
incomplete aim or explicit off-screen. The test runs in the normal CI inventory.

**RED evidence:** Before changing `src/ports.c`, the regression built successfully
and `ctest --preset ci -R '^input[.]lightgun$' --output-on-failure` exited **8**.
All six complete-aim/absent-flag cases failed the pending and applied flag checks
(`1 != 0`) and actual illumination/latch check (`0x80 != 0x0`). Incomplete-aim,
explicit-zero and explicit-off-screen controls did not fail. After the one-line
implementation change, the same test exited **0**. This establishes semantic
regression coverage beyond a syntax-only verification; no owner judgment is
required under AGENTS rule 6. Parent re-review remains the next workflow gate.

### WR-01: Libretro guide reverses the ProLine physical button labels

**Status:** fixed
**Files modified:** `docs/libretro.md`
**Commit:** `463b3a0fbe7d5b233246ee2d46c821945ceecf4c`
**Applied fix:** Corrected the table to RetroPad B → button 1 (right) and
RetroPad A → button 2 (left). Re-read against `lr_buttons` and the controller
hardware contract; existing adapter mappings are unchanged. The focused
ProLine/libretro input tests passed with their fixture.

## Verification

All commands ran in the **main checkout**, on `phase/08-controllers-at-the-pins`:
`workflow.use_worktrees=false` was honored. Every local evidence row has run URL
**N/A-local**. No hosted run was dispatched or claimed by this fixer. Parent
will obtain exact-source hosted acceptance after re-review.

| Command | Exit | Result |
|---|---:|---|
| `sh build/phase08-root-pin.sh` | 0 | Guard passed before the first write and every commit attempt. |
| `cmake --build --preset ci --target muhtari-test-input` | 0 | Regression compiled before RED; implementation and final formatted test compiled before commit. |
| `ctest --preset ci -R '^input[.]lightgun$' --output-on-failure` before implementation fix | 8 | Meaningful RED: six missing-flag cases failed normalization and actual sensor capture. |
| `ctest --preset ci -R '^input[.]lightgun$' --output-on-failure` after implementation fix | 0 | Regression and existing raster/latch coverage passed. |
| `ctest --preset ci -R '^(input[.]\|api[.]v6\|abi[.]api)' --output-on-failure` | 0 | 27 selected input/ABI tests passed. |
| `/opt/homebrew/opt/llvm@21/bin/clang-format --dry-run --Werror src/ports.c tests/input/test_input.c` | 0 | Both C files match the pinned formatter. |
| `ctest --preset ci -R '^(input[.]proline_two_button\|libretro[.]host[.]input)$' --output-on-failure` | 0 | Three selected/fixture tests passed. |
| `cmake --workflow --preset ci` | 0 | 649 normal tests, three actual macOS arm64 ZIP packages, and six release checks passed. |
| `cmake --workflow --preset hygiene` | 0 | All 24 hygiene checks passed, including format, analyzer, fixture digests and tree scan. |
| `sh scripts/hygiene.sh --tree` | 0 | Final tree scan including the newly created fix report passed. |
| `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` | 0 | Exactly 649 selected tests ran once each. |
| `cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake` | 0 | Exactly six release tests ran once each. |
| `cmake -DJUNIT=build/hygiene/junit.xml -DINVENTORY=tests/inventory/hygiene.txt -P tests/cmake/check_junit.cmake` | 0 | Exactly 24 hygiene tests ran once each. |
| `git diff --quiet 9e76ca3 HEAD -- include/muhtari.h tests/api tests/inventory tests/roms/manifest.txt tests/diag/expected.csv tests/audio/expected.txt tests/built/expected.txt` | 0 | ABI/header, inventory and all existing numeric/digest fixtures unchanged. |
| `git diff --check` | 0 | No whitespace errors. |

Modified source sections were re-read after edits; the C build supplies syntax
verification. Each finding was committed separately with normal hooks and the
configured GitHub noreply identity. The first sandboxed commit attempt could not
write git metadata (`staging_failed`, index lock permission); the authorized
escalated retry succeeded with normal hooks. No bypass was used, no fix was
rolled back, and no source changes remain uncommitted. This report is deliberately
uncommitted for the parent workflow. No shared planning state or source review
was changed by the fixer. No GPL, LGPL or unlicensed emulator source was opened.

---

_Fixed: 2026-10-25T03:11:20Z_
_Fixer: the agent (gsd-code-fixer)_
_Iteration: 1_

## Parent acceptance calibration

Before hosted dispatch, the parent measured the expanded lightgun regression
under the existing local Apple Clang ASan/UBSan build. Twelve complete successful
processes in three batches of four had maximum elapsed0.968114250 seconds.
The preceding accepted exact-source CI40153312357 independently completed the
Windows x64 base case in0.97 seconds. Under DEC.47, ceil(2*0.97)=2 seconds is the
provisional owning-test deadline; normal lane scaling remains unchanged. Only
`input.lightgun` is separated from the existing one-second registration group.
No workload, assertion, case, inventory membership or lane authority is removed.

`cmake --build build/asan --target muhtari-test-input -j 4` exits0.
The one-time Python subprocess probe executes
`build/asan/tests/muhtari-test-input lightgun` twelve times, each exit0; run URL
N/A-local. Its transient measurements remain in ignored build output. Hosted
acceptance at the corrected source remains pending; prior hosted passes do not
prove this correction.

After this calibration, `cmake --workflow --preset ci` exits0 with649 ordinary
tests and6 release checks; run URL N/A-local. The registration remains exactly
the same649-test inventory.
