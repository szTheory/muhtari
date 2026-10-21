---
phase: 03-games-play-in-retroarch
fixed_at: 2026-10-20T16:49:44Z
review_path: .planning/phases/03-games-play-in-retroarch/03-REVIEW.md
iteration: 2
findings_in_scope: 1
fixed: 1
skipped: 0
status: all_fixed
---

# Phase 3: Code Review Fix Report

**Fixed at:** 2026-10-20T16:49:44Z
**Source review:** .planning/phases/03-games-play-in-retroarch/03-REVIEW.md
**Iteration:** 2
**Review budget:** Fix round 2 of 2, the final permitted round. No further review or fix round was run.

**Summary:**

- Findings in scope: 1
- Fixed: 1
- Skipped: 0

## Fixed Issues

### CR-01: Failed frames send NULL video data to frontends that do not support duplication

**Files modified:** `libretro/libretro.c`, `tests/libretro/bios.c`, `tests/libretro/host.c`,
`docs/libretro.md`
**Commit:** 24bc0816588204fb11095e312e1330add583f77e
**Status:** fixed
**Applied fix:** Initialize the converted pixel buffer to black after successful allocation. Every failed
frame presents that buffer with the normal geometry and pitch. After a successful frame it contains the
retained picture; before any successful frame it contains defined black pixels. This needs no frontend
duplication capability and preserves the existing error message, log, callback pacing and lack of audio.
The documentation describes both pictures and the use of actual pixels.

**Regression:** The existing `libretro.bios_failures` test now supplies an environment explicitly reporting no
duplication support. Through the actual adapter, it injects the core's persistent BIOS-rejection state before
the first run and verifies a non-NULL black picture, then captures a nonblack test card and checks identical
pixels on two later failed runs. The existing `libretro.host.bios` case still drives a real OpenBIOS rejection;
it now requires non-NULL video on every failed run and checks that the previous frame hash is preserved,
including after Restart. Existing assertions cover callback count, geometry, no audio, once-only reporting
and absent video callbacks. Both regressions run in the CI workflow. No new file or fixture was added.

## Verification

All checks ran in the **main checkout**, on `phase/03-games-play-in-retroarch`, because
`workflow.use_worktrees` is false. No worktree was created. The changed sections were re-read; the C17 CI build
compiled them with warnings treated as errors. The agent verified semantics automatically; no owner verification
is required.

- Before the source fix, `cmake --build --preset ci --target muhtari-test-libretro-bios` exited 0.
  `ctest --preset ci -R '^libretro.bios_failures$' --output-on-failure` exited **8**, reproducing three
  unsupported NULL video callbacks in the new regression.
- After the fix, `ctest --preset ci -R '^libretro\.' --output-on-failure` exited **0**, 18/18 including fixtures.
- On the final source tree, `cmake --workflow --preset ci` exited **0**, 588/588 tests, all three deliverables
  packaged and 6/6 release checks.
- On the final source tree, `cmake --workflow --preset hygiene` exited **0**, 24/24 checks including formatting,
  analyzer, firmware scan and tree hygiene. Its first attempt exited 8 on one indentation in the new test;
  the pinned formatter corrected that indentation, then both workflows were rerun successfully.
- `git diff --check` exited **0** before the atomic source commit.
- `gsd_run query commit 'fix(03): CR-01 present retained pixels on failed frames' --files libretro/libretro.c
  tests/libretro/bios.c tests/libretro/host.c docs/libretro.md` committed **24bc081** with the configured GitHub
  noreply author and committer. Initial staging lacked Git index write permission; the authorized elevated
  retry succeeded. No source changes were lost or left uncommitted.

The source fix was published with `git push origin phase/03-games-play-in-retroarch` (exit **0**).
`gh workflow run ci.yml -R szTheory/muhtari --ref phase/03-games-play-in-retroarch` exited **0** and created
[hosted run 39460962750](https://github.com/szTheory/muhtari/actions/runs/39460962750) on source
`24bc0816588204fb11095e312e1330add583f77e`. `gh run watch 39460962750 -R szTheory/muhtari --exit-status`
exited **0** with conclusion `success`. All six Linux, macOS and Windows x64/ARM64 builds passed, alongside
ASan, big-endian, both integer-only jobs, hygiene, library, performance, WebAssembly, hash equality,
the RetroArch screenshot comparison and `CI required`. The title check was skipped for the manual run.
No committed log or firmware bytes were added.

The preserved `03-REVIEW.md` remains the source review; the disposition ledger records CR-01 as fixed.
Both permitted fix rounds are used. No re-review or further GSD step was run. Next command:
`$gsd-code-review 3`, followed by phase re-verification in a later session.

## Prior Round History

Iteration 1 fixed all five original warnings. Its detailed report is available in Git history before this
overwrite; the source fixes remain intact:

| Finding | Applied fix | Commit |
|---|---|---|
| WR-01 | Validate hash-matched cartridge geometry, with boundary regressions | db68572 |
| WR-02 | Correct public audio-count and memory-span comments | f934e77 |
| WR-03 | Report frame failure once and keep video pacing | 6487058 |
| WR-04 | Validate BIOS candidates, handle I/O/allocation/path failures and test fallback | 0f8e45d, 70c2a05 |
| WR-05 | Replace a planning identifier in a source comment with the embedding guide | 095d6dd |

Round 1's final `cmake --workflow --preset ci` exited 0 (588/588 and 6/6 release checks), and
`cmake --workflow --preset hygiene` exited 0 (24/24). Its hosted evidence is
`gh run watch 39452704206 -R szTheory/muhtari --exit-status`, exit **0**,
[run 39452704206, attempt 2](https://github.com/szTheory/muhtari/actions/runs/39452704206), on source
`70c2a05d83e271f18b46752eb70fd6d0a87005d2`. That run predates CR-01's fix and is prior evidence only.
The subsequent review confirmed WR-01 through WR-05 resolved and raised the sole active CR-01 addressed here.
The five original Info findings remain seeds in `03-REVIEW-DISPOSITION.md`.

---

_Fixed: 2026-10-20T16:49:44Z_
_Fixer: the agent (gsd-code-fixer)_
_Iteration: 2_
