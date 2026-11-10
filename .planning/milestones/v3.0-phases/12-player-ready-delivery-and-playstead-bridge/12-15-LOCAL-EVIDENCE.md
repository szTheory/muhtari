# Plan 12-15 local input diagnostic evidence

**Disposition:** halted before launching a RetroArch content child. The qualified
helper's nonprompting CoreGraphics event-post preflight returned `denied` (exit
77). This is an access refusal, not evidence for the cause of the earlier hosted
input-acknowledgment failure. No permission request or settings change was made.

## Source and artifact identities

- Root checkout: `phase/12-player-ready-delivery-and-playstead-bridge`, clean at
  source commit `5d138dc55828facf5eac303f0ff6bd2e743b8f74` before this ledger.
- Pinned host DMG `build/dmg/RetroArch_Metal.dmg`: SHA-256
  `81b79121ba26d539064ae13b4d0419a120c3d165afbe656cf5f5412b15fdb434`, matching
  `pins.txt`.
- Extracted host: RetroArch 1.22.2, universal arm64/x86_64 executable at
  ignored `build/phase12-gap15-evidence/RetroArch.app/Contents/MacOS/RetroArch`,
  SHA-256 `ed90b54434a2899de0ddbfed59f335255fb462c8691bd970a1a761ebb5656d65`.
  It matches the previously recorded host executable identity. The DMG was
  mounted read-only, the app copied to ignored scratch, and the mount detached;
  the app was not installed or launched.
- Production core `build/ci/libretro/muhtari_libretro.dylib`: SHA-256
  `229a7df44115592f0696faf78ea1770810636e69df99135e013bbaa6ab8a77fc`.
- Diagnostic core `build/ci/libretro/muhtari_libretro_input_trace.dylib`, copied
  to ignored `build/phase12-gap15-evidence/input-diagnostic/`:
  SHA-256 `5c28a6bf3b29b8ae3c56863704fa3f75127e64641a9db691b97aa79868f71a69`.
  A byte comparison against the production core reported that they differ; the
  diagnostic build is supporting evidence only and was not packaged.
- Trace collector `muhtari-test-trace-protocol`, copied to the same ignored
  directory: SHA-256
  `182affa7e457365abba33c8ee4d0892e50ee7acae8a8b1cc0d7a62e54fbc3487`.
- Own-child keyboard helper at ignored
  `build/phase12-gap15-evidence/keyboard-input-trace`: SHA-256
  `781251d952d6dacfec1f438c5258ce78edcae4847fa59fe0d69ca1960e3e20c9`.
- Generated owned standard cartridge at ignored
  `build/phase12-gap15-evidence/input.a78`: SHA-256
  `36009274274c48b9dcc299647bdf9e744f74a383ba174eedeb77fae729f36f35`.

## Qualification commands and results

All build inputs came from the recorded root source commit above. Logs, copied
executables and the cartridge remain in ignored `build/phase12-gap15-evidence/`.

| Command | Exit | Result |
|---|---:|---|
| `cmake --build --preset ci --target muhtari_libretro_input_trace muhtari-test-trace-protocol` | 0 | Existing diagnostic core and trace collector built; output is in `diagnostic-build.log`. |
| `cmake --build --preset ci --target muhtari-test-input` | 0 | Existing owned-input generator built; output is in `input-generator-build.log`. |
| `cc -std=c17 -Wall -Wextra -Werror -DMUHTARI_INPUT_TRACE tests/retroarch/keyboard_preflight.c tests/libretro/input_trace.c -framework ApplicationServices -o build/phase12-gap15-evidence/keyboard-input-trace` | 0 | Existing helper compiled with the diagnostic trace support. |
| `build/ci/tests/muhtari-test-input --write-cart build/phase12-gap15-evidence/input.a78` | 0 | Generated only the existing owned standard cartridge; output is in `generate-input.log`. |
| `build/phase12-gap15-evidence/keyboard-input-trace --preflight > build/phase12-gap15-evidence/preflight.log 2>&1` | 77 | Output: `post-event-access=denied`. The helper's source returns 77 when `CGPreflightPostEventAccess()` denies event posting. |

The first sandboxed read-only DMG mount failed with `device not configured`;
the same read-only mount succeeded when run with the authorized local filesystem
access, then detached successfully. No install, launch, permission prompt, or
settings change followed.

## Comparison with the earlier hosted failure

The prior run 42239124226 had an effective configuration with audio disabled,
`audio_driver = null`, `pause_nonactive = false`, and Up/Z bindings, but did not
explicitly pin `video_driver`. Its helper reported event posting as permitted
and logged events to its child at 0/155/319/494/601/770 ms. Capture remained 0;
the child status was unknown at the recorded timeout. The core loaded and logged
input driver `cocoa`, joypad `mfi`, and geometry 320x242. A separate audio error
does not establish the input failure's cause.

The local preflight refusal means this checkout's helper could not safely run
the planned diagnosis without crossing the plan's explicit stop against
permission grants and settings changes. The local result therefore does not
reproduce or explain the earlier hosted failure. In particular, neither the
missing hosted capture nor this local access refusal proves whether the earlier
core polled input.

## Launch and trace accounting

- Content-child launch reservation: **not created**.
- Local RetroArch content children launched by this plan: **0**; the one-child
  allowance was not spent.
- Trace, screenshot and child-status observations from a content run: **none**.
  A bounded file check found no `*.trace`, `*.png`, or reservation file in the
  ignored evidence directory.
- Process enumeration, permission changes, host workflow attempts and hosted
  dispatches: **not run**.
- Task 2, source corrections, full CI, hygiene and formatter gates: **not run**.

## Acceptance effect

This plan is halted because its first task reached the explicitly retained
access refusal before content launch. No causal defect was demonstrated, so the
conditional repair task was not run. Phase 12 remains accepted 0/4; Plan 12-01
remains halted; Plans 12-02/03/04 remain blocked; and Plan 12-16 remains blocked
by both the missing local WASI sysroot in Plan 12-14 and this unresolved Plan
12-15 result. No new hosted attempt is qualified. The separate Phase 14 stop is
unchanged.
