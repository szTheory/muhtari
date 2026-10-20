---
phase: 03-games-play-in-retroarch
fixed_at: 2026-10-20T15:49:06Z
review_path: .planning/phases/03-games-play-in-retroarch/03-REVIEW.md
iteration: 1
findings_in_scope: 5
fixed: 5
skipped: 0
status: all_fixed
---

# Phase 3: Code Review Fix Report

**Fixed at:** 2026-10-20T15:49:06Z
**Source review:** .planning/phases/03-games-play-in-retroarch/03-REVIEW.md
**Iteration:** 1
**Review budget:** Fix round 1 of 2; one round remains. No re-review was run.

**Summary:**

- Findings in scope: 5
- Fixed: 5
- Skipped: 0
- Info findings were outside the requested scope and remain unchanged.

## Fixed Issues

### WR-01: A headerless image matched by hash skips the board geometry check, and the banking code assumes it passed

**Files modified:** `src/cart.c`, `tests/cart/test_cart.c`, `docs/hardware/cartridge.md`
**Commit:** db68572
**Status:** fixed
**Applied fix:** Require hash-matched headerless images to fit the board before acceptance, preserving unsupported-board
refusals. Correct the existing acceptance fixture from a 16 KiB SuperGame image to 32 KiB. Inject synthetic hash
entries to reject short SuperGame images, insufficient ROM-at-$4000 banks and oversized linear images, alongside
valid boundary cases. Document the geometry check on this probe path.
**Regression:** `cart.probe.accepted` exercises all eight injected geometry cases; the existing hash-identification
checks also pass.

### WR-02: The public header's contract comments contradict the behaviour a host will see

**Files modified:** `include/muhtari.h`
**Commit:** f934e77
**Status:** fixed
**Applied fix:** Describe `audio_count` as bounded by `max_audio_samples`, allowing partial boot frames. Limit the
memory-span pointer promise to the next load or destruction, consistent with cartridge RAM allocation lifetime.
These are comment corrections; ABI and behavior are unchanged.
**Verification:** Read the comments against `src/frame.c`, `src/cart.c` and the embedding guide; existing partial-frame,
C/C++ header, API-version and documentation checks pass.

### WR-03: When the libretro core fails a frame, nothing reaches the screen or the frontend's frame pacing

**Files modified:** `libretro/libretro.c`, `tests/libretro/host.c`, `docs/libretro.md`
**Commit:** 6487058
**Status:** fixed
**Applied fix:** Send the first frame error to the frontend's message callback and log, and duplicate the previous
picture through the video callback on every failed frame. Document the error presentation.
**Regression:** The existing BIOS host test drives an actual OpenBIOS cartridge rejection. It checks the first error
message and log, one video callback per run, duplicate-frame geometry, no audio on failed frames, no repeated
message/log, a fresh report after Restart, and a missing video callback.

### WR-04: The libretro BIOS lookup gives up early and misreports failures

**Files modified:** `libretro/libretro.c`, `tests/libretro/bios.c`, `tests/libretro/host.c`,
`tests/libretro/tests.cmake`, `tests/inventory/ci.txt`, `docs/libretro.md`, `docs/testing.md`
**Commits:** 0f8e45d, 70c2a05
**Status:** fixed
**Applied fix:** Validate each BIOS by loading it before accepting the filename; continue to the second candidate
after invalid bytes or I/O errors. Check path formatting before opening a file. Distinguish absent files from
unusable files in fallback messages; allocation failure refuses the load with an out-of-memory message and log.
Log a BIOS hash only after successful acceptance. Document the fallback and refusal behavior.
**Regression:** New `tests/libretro/bios.c` compiles the actual adapter with deterministic file/allocation substitutions,
using generated in-memory images and its public load entry point. `libretro.bios_failures`, included in the CI
inventory, checks first-file preference, short/oversized images, invalid reset vectors, read/open errors, missing
files, no usable candidate, allocation failure and path truncation without opening a truncated filename. The existing
host test also writes a damaged U file beside a valid E OpenBIOS and compares all 120 frames with the valid BIOS run.
The initial Windows build exposed MSVC's missing `max_align_t` in this test; `70c2a05` uses valid stream-pointer
tokens for the substituted operations, which never access or close those streams.

### WR-05: A planning ID in core source breaks the project rule

**Files modified:** `src/ports.c`
**Commit:** 095d6dd
**Status:** fixed
**Applied fix:** Replace the planning decision identifier with the public embedding guide's Input section, whose
VBLANK-start latch description was checked against the comment. Behavior is unchanged.
**Verification:** Re-read the affected section and run the existing input checks.

## Verification

All builds and checks ran in the **main checkout**, on `phase/03-games-play-in-retroarch`, because
`workflow.use_worktrees` is false. No worktree was created. Every modified section was re-read before committing;
the CI compiler checked the changed C sources with warnings treated as errors. Semantics were verified through
automation; no owner verification is required.

- `ctest --preset ci -R '^cart\.(probe.accepted|identify\.)' --output-on-failure`: exit 0, 11/11 including the build fixture.
- `ctest --preset ci -R '^(abi\.(header|api)|frame\.first_frame_partial|docs\.api_names)' --output-on-failure`: exit 0, 10/10.
- `ctest --preset ci -R '^libretro.host.bios$' --output-on-failure`: exit 0, 3/3 including two build fixtures.
- `ctest --preset ci -R '^libretro\.' --output-on-failure`: exit 0, 18/18 including four fixtures.
- `ctest --preset ci -R '^input\.' --output-on-failure`: exit 0, 12/12.
- `cmake --workflow --preset ci`: exit 0 on the final tree, 588/588 tests, three deliverables packaged, 6/6 release tests.
- `cmake --workflow --preset hygiene`: exit 0 on the final tree, 24/24 including format, analyzer, firmware scan and clean-tree checks.
- `git diff 85e7d4e..HEAD --check`: exit 0, no whitespace errors.
- `ctest --preset ci -R '^(cart\.probe\.accepted|libretro\.)' --output-on-failure`: exit 0, 19/19 checks in the orchestrator verification.
- `git push origin phase/03-games-play-in-retroarch`: exit 0, published the fixes through `70c2a05`.
- `gh workflow run ci.yml -R szTheory/muhtari --ref phase/03-games-play-in-retroarch`: exit 0; [hosted CI run](https://github.com/szTheory/muhtari/actions/runs/39452704206) on source commit `70c2a05d83e271f18b46752eb70fd6d0a87005d2`.

The [initial Windows ARM64 build](https://github.com/szTheory/muhtari/actions/runs/39452222905/job/118729902577)
exited 2 on the test's `max_align_t` declaration. Its superseded workflow was cancelled with
`gh run cancel 39452222905 -R szTheory/muhtari` (exit 0). The final local CI and hygiene results above were rerun
after the portable-token correction; hosted CI was dispatched again on `70c2a05`.


### Hosted CI status

At 2026-10-20T16:03:14Z, `gh run view 39452704206 -R szTheory/muhtari --json status,conclusion,jobs`
exited 0 and showed both macOS build jobs still queued without assigned runners. The run is not green.
Linux x64/ARM64, Windows x64, ASan, big-endian, both integer-only jobs, hygiene, library and performance passed.

The [Windows ARM64 job](https://github.com/szTheory/muhtari/actions/runs/39452704206/job/118731533625)
compiled successfully and passed the cartridge and libretro regressions, including `libretro.bios_failures`.
Its full test step exited 8 because `runner.usage.port`, `runner.usage.region_pal` and
`runner.usage.boot_bios_no_file` exceeded their existing two-second timeouts after printing the expected output.
This is a suspected runner timing issue, not yet confirmed by a successful retry. No timeout was changed.

`gh run rerun 39452704206 -R szTheory/muhtari --job 118731533625` exited 1. The direct retry API also
exited 1 with HTTP 403: the workflow containing the job is already running. When the queued macOS work
finishes, the next agent should run `gh run rerun 39452704206 -R szTheory/muhtari --failed` and check
its result. The local CI and hygiene results above passed on the same source tree. This hosted follow-up
requires no owner verification and does not consume another fix round by itself.

No logs or firmware bytes were committed. The original review is preserved; its per-finding disposition ledger
records the five fixes. Existing phase verification still requires re-verification of the former source-comment gap.
Next command: `$gsd-code-review 3`.

---

_Fixed: 2026-10-20T15:49:06Z_
_Fixer: the agent (gsd-code-fixer)_
_Iteration: 1_
