---
phase: 03-games-play-in-retroarch
reviewed: 2026-10-20T16:17:34Z
depth: standard
files_reviewed: 12
files_reviewed_list:
  - docs/hardware/cartridge.md
  - docs/libretro.md
  - docs/testing.md
  - include/muhtari.h
  - libretro/libretro.c
  - src/cart.c
  - src/ports.c
  - tests/cart/test_cart.c
  - tests/inventory/ci.txt
  - tests/libretro/bios.c
  - tests/libretro/host.c
  - tests/libretro/tests.cmake
findings:
  critical: 1
  warning: 0
  info: 0
  total: 1
status: issues_found
---

# Phase 3: Code Review Report

**Reviewed:** 2026-10-20T16:17:34Z
**Depth:** standard
**Files Reviewed:** 12
**Status:** issues_found

## Summary

Read every file in the supplied scope in full and examined its changes since `f79d52b`. Traced cartridge
geometry into mapper arithmetic, BIOS candidate loading into `muhtari_load`, memory-span ownership, input
latching, and frame errors into frontend callbacks. Context included the preparation index and decisions,
the cartridge and host preparation material, the embedding guide, `src/instance.c`, `src/boot.c`,
`src/frame.c` and the vendored libretro contract. No prohibited emulator source was opened.

Scope provenance is **degraded**, as reported by the canonical evaluation-scope resolver: `source: phase-range`,
`reason: no-task-commits-since`, with `--since f79d52b`. The explicit 12-file list above is the review scope;
this is a review of the fixes and surrounding code, not a fresh review of all phase 03 sources.
No structural pre-pass or external reviewer evidence was supplied. No configured project reviewer skills were found.

The original WR-01 through WR-05 are resolved as described below. One fresh **BLOCKER** remains in the new failed-frame
presentation: NULL video duplication is used without checking frontend support. The existing selected checks
pass, but a direct contract diagnostic reproduces the invalid callback. Source and test files were not modified.

## Narrative Findings (AI reviewer)

## Critical Issues

### CR-01: Failed frames send NULL video data to frontends that do not support duplication

**Classification:** BLOCKER
**File:** `libretro/libretro.c:499-500`
**Issue:** Every failed `muhtari_run_frame` now calls `video_cb(NULL, ...)`. The adapter never queries
`RETRO_ENVIRONMENT_GET_CAN_DUPE`. The vendored contract explicitly makes NULL frame duplication conditional on
that capability (`libretro/libretro.h:754-763`, `7687-7690`). A frontend may return false, or leave the command
unimplemented, and still load this software-rendered core. Such a frontend expects actual pixel data and may
dereference NULL while copying the frame. A recoverable BIOS rejection can therefore crash the host instead of
preserving the last picture.

This is reachable through the BIOS-rejection branch already exercised by `tests/libretro/host.c:819-855`.
That host's environment callback returns false for the capability command (`235-320`), but its video callback
accepts NULL anyway (`332-334`), so its passing result does not establish compliance for a frontend without
duplication support.

**Reproduction:** A temporary harness compiled the current adapter unchanged, registered an environment that
answers the duplication query with `false`, rendered one ordinary frame, set the instance's BIOS-rejection
flag, and called `retro_run` again. It observed zero capability queries and a NULL second video frame.
Compilation exited 0; the check rejecting unsupported NULL frames exited 1. The injected flag selects the same
`MUHTARI_STATUS_ERR_BIOS_REJECTED_CART` return used by an actual BIOS rejection (`src/frame.c:33-35`).

**Fix:** Present the retained `pixels` buffer on failed frames. Initialize that buffer after successful
allocation, before any run, so a BIOS that rejects the cartridge on its first frame also has a defined picture:

```c
/* After the frame-buffer allocations have succeeded, at load. */
memset(pixels, 0, LR_PIXELS * sizeof *pixels);

/* In the failed-frame branch. */
if (video_cb != NULL) {
    video_cb(pixels, LR_WIDTH, LR_HEIGHT, (size_t)LR_WIDTH * sizeof pixels[0]);
}
```

Alternatively, cache a successful `GET_CAN_DUPE` answer and use NULL only when its returned value is true;
use the initialized retained buffer otherwise.

## Prior warning resolution

These are historical dispositions, excluded from the active finding counts above.

| Prior finding | Resolution verified in current source |
|---|---|
| WR-01 | `src/cart.c:639-642` preserves unsupported-board refusal, then requires `cart_fits(&b, n)` for a headerless hash match. SuperGame's minimum two banks, SG9's minimum three banks and linear's maximum size now hold before acceptance. The eight injected cases at `tests/cart/test_cart.c:188-204` cover accepted boundaries and rejected sizes. |
| WR-02 | `include/muhtari.h:69` describes audio count by the reported maximum, permitting partial boot frames. Line 211 limits pointer validity to the next load or destruction, covering cartridge allocation replacement at `src/cart.c:759-764`. Both agree with the implementation and embedding guide. |
| WR-03 | `libretro/libretro.c:494-501` reports the first error through message and log callbacks, and calls video on every failed run. Restart clears `run_failed` at line 465. The previous silent failure is resolved; the unsupported-duplication defect introduced by that fix is CR-01 above. |
| WR-04 | `libretro/libretro.c:575-634` rejects truncated paths before opening them, distinguishes open/read failures from absence, closes and frees each candidate, validates through `muhtari_load`, tries the second file after invalid BIOS bytes, and reports allocation failure separately. Only an accepted BIOS is hashed and logged. The core copies BIOS bytes before the temporary buffer is freed (`src/instance.c:209-224`). |
| WR-05 | `src/ports.c:58-59` now cites `docs/embedding.md, Input`, whose timing description matches the latch comment. The planning decision identifier is gone. |

## Carryover seed context

The prior review's five Info entries remain seeds, recorded in `03-REVIEW-DISPOSITION.md`; they are not new
findings or requirements from this re-review and are excluded from its counts:

- IN-01: first BIOS-frame input latching inferred from elapsed length (`src/frame.c`).
- IN-02: duplicated RIOT read/peek dispatch (`src/riot.c`).
- IN-03: BIOS hash metadata on a fast-boot movie (`runner/movie.c`, `runner/main.c`).
- IN-04: PAL BIOS fallback under NTSC timing (`libretro/libretro.c`). The candidate order still includes the E
  filename and the choice remains documented; the candidate-validation fix does not resolve or elevate this seed.
- IN-05: the fuzz run bound's explanatory comment (`tests/fuzz/run.c`); a tooling seed under AGENTS rule 7.

The other four seed locations are outside this review's explicit scope and were not re-reviewed as independent
findings. No new test or tooling requirements are raised.

## Verification evidence

- `ctest --preset ci -R '^(cart\.(probe.accepted|identify\.)|libretro\.|frame\.first_frame_partial|input\.)' --output-on-failure`
  exited **0**, 40/40 including build fixtures, on the current checkout.
- The one-off contract harness used `/usr/bin/cc -std=c17 -I. -Iinclude -Ilibretro -Ihost -Irunner
  -Ibuild/ci/generated -x c - -x none build/ci/libmuhtari.a build/ci/libmuhtari_host.a
  build/ci/CMakeFiles/muhtari-test-libretro-host.dir/runner/sha256.c.o -o <temporary>/dupe-contract`:
  compiler exit **0**, diagnostic executable exit **1**, reproducing CR-01. C source was supplied through
  stdin; the temporary binary was removed. No ROM or BIOS file was created for this check.
- Existing hosted fix-round evidence remains in `03-REVIEW-FIX.md`:
  `gh run watch 39452704206 -R szTheory/muhtari --exit-status` exited **0**,
  [run 39452704206, attempt 2](https://github.com/szTheory/muhtari/actions/runs/39452704206), source `70c2a05`.
  This is prior evidence, not a new hosted run or proof of duplication support.

Review only; no source edits, fixes, commits or subsequent GSD step were performed. One of the two permitted
fix rounds has been used (`03-REVIEW-FIX.md`, iteration 1).

---

_Reviewed: 2026-10-20T16:17:34Z_
_Reviewer: the agent (gsd-code-reviewer)_
_Depth: standard_
