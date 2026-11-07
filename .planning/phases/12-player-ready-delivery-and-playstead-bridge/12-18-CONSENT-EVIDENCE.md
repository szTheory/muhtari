# Phase 12 Plan 12-18: Consent and diagnostic preparation

**Disposition:** prepared; awaiting the exact owner decision below. No OS permission request, real preflight, event post, RetroArch launch, invocation reservation, or content child has occurred. Plan 12-15's denial and zero-child result remain unchanged.

## Bound execution identity

- Source checkout: `phase/12-player-ready-delivery-and-playstead-bridge`, HEAD `eca3862d843613da1455249075b07f6107d60e85`; the helper, trace module, wrapper, and cartridge generator inputs have no changes since Plan 12-15's source. The generated consent helper is private and ignored at `build/phase12-gap18-evidence/keyboard-input-trace-consent`.
- Consent helper SHA-256 `959bc018e1c8249358f21ff5aefc05676fb10e79e5acef1a01494235253bc4bd`; thin arm64 Mach-O, `keyboard-input-trace-consent`, linker ad-hoc signature, CodeDirectory CDHash `120298194fad01052647f3036d74e641f258f30e`. Built from the unchanged `tests/retroarch/keyboard_preflight.c` included under a renamed entry point plus a request-only dispatcher. Existing helper arguments forward unchanged; only the exact `--request-event-post-access` argument reaches `CGRequestPostEventAccess()`.
- Session host is directly identified by `TERM_PROGRAM=Apple_Terminal`, version `470.2`: `/System/Applications/Utilities/Terminal.app/Contents/MacOS/Terminal`, bundle `com.apple.Terminal`, Apple system signature, executable SHA-256 `c5f23927d33d886d3a3e39b4ef76381b1971a9cc74523f7ebf5e60affaf8fa8b`. The helper and both later operations are tied to this same Terminal session. No process enumeration or alternate execution identity was used.
- Compiler: Apple clang 21.0.0 (`clang-2100.1.1.101`); SDK `MacOSX26.5.sdk`. Installed `CGEvent.h` lines 405 and 408 declare `CGPreflightPostEventAccess(void)` and `CGRequestPostEventAccess(void)` as `bool`, available since macOS 10.15. The dispatcher calls neither API during preparation; the mock binary replaces both API symbols. Official references: [preflight](https://developer.apple.com/documentation/coregraphics/cgpreflightposteventaccess()) and [request](https://developer.apple.com/documentation/coregraphics/cgrequestposteventaccess()).
- Pinned host: RetroArch 1.22.2. `pins.txt` DMG SHA-256 `81b79121ba26d539064ae13b4d0419a120c3d165afbe656cf5f5412b15fdb434`; extracted executable `build/phase12-gap15-evidence/RetroArch.app/Contents/MacOS/RetroArch`, SHA-256 `ed90b54434a2899de0ddbfed59f335255fb462c8691bd970a1a761ebb5656d65`, signed `com.libretro.dist.RetroArch` by Developer ID `UK699V5ZS8`.
- Diagnostic core: `build/ci/libretro/muhtari_libretro_input_trace.dylib`, SHA-256 `5c28a6bf3b29b8ae3c56863704fa3f75127e64641a9db691b97aa79868f71a69` (diagnostic only, not the packaged core). Trace parser: `build/ci/tests/muhtari-test-trace-protocol`, SHA-256 `182affa7e457365abba33c8ee4d0892e50ee7acae8a8b1cc0d7a62e54fbc3487`. Owned standard cartridge: `build/phase12-gap18-evidence/input.a78`, SHA-256 `36009274274c48b9dcc299647bdf9e744f74a383ba174eedeb77fae729f36f35`.

## Preparation and controls

`python3 build/phase12-gap18-evidence/consent.py prepare-checks` — exit 0. It rebuilt the existing diagnostic core/parser and cartridge generator, generated the owned standard cartridge, and compiled the real-route helper and a separate mocked-API helper. The first preparation invocation exited 1 on an incorrect expected path for the existing `.info` file; the path was corrected to `build/ci/libretro/muhtari_libretro.info` and the complete preparation was rerun successfully. Neither invocation called a real access API or launched RetroArch.

The isolated controls passed: denied and permitted preflight each called only the mocked preflight once; permitted and denied request each called only the mocked request once; unknown arguments called neither; missing and changed-identity approvals blocked dispatch; denied postflight blocked diagnostic reservation. Mock helper SHA-256: `b6758caac349ac0686d72c32c252b2cc289942ba649073b1b9633c327b9ad131`.

`python3 build/phase12-gap18-evidence/consent.py audit-receipts` — exit 0. The read-only audit found no decision, request reservation/receipt, postflight reservation/receipt, diagnostic reservation/receipt, or content child. Preparation counters: real requests 0; real preflights 0; helper pre-fork rechecks 0; diagnostic reservations 0; content children 0. Current code/pin diff against Plan 12-15 inputs is empty. Scratch binaries, logs, driver, and receipts are under ignored `build/phase12-gap18-evidence/`.

## Exact operations covered by the decision

After an affirmative choice only:

1. `python3 build/phase12-gap18-evidence/consent.py request-approved` reserves before invoking this exact helper once as `keyboard-input-trace-consent --request-event-post-access`. The single `CGRequestPostEventAccess()` call may show a macOS prompt and may establish persistent event-post permission for the identified Terminal context. Timeout or ambiguous completion spends the request; no retry.
2. If the request process completes normally, `python3 build/phase12-gap18-evidence/consent.py postflight` reserves and invokes the same binary's `--preflight` once (15-second maximum), even if the request returned denied. A denied or ambiguous preflight stops before RetroArch.
3. Only with permitted preflight and the same affirmative hardware authorization, `python3 build/phase12-gap18-evidence/consent.py diagnose-once` reserves and runs the prepared existing RetroArch probe once. The helper repeats its nonprompting pre-fork gate inside that invocation. The wrapper has 600 frames, a 5-second acknowledgment deadline, 45-second helper bound, and 50-second wrapper bound; input events target only its own RetroArch child. It uses isolated paths and does not change settings or TCC state. The probe can remain inconclusive; permission success alone is not a causal finding.

Prepared wrapper command (not run):

```sh
cmake \
  -DRETROARCH=build/phase12-gap15-evidence/RetroArch.app/Contents/MacOS/RetroArch \
  -DCORE=build/ci/libretro/muhtari_libretro_input_trace.dylib \
  -DINFO=build/ci/libretro/muhtari_libretro.info \
  -DTEMPLATE=tests/retroarch/test.cfg.in \
  -DRA_DIR=build/phase12-gap18-evidence/ra-input-probe \
  -DSAVE_DIR=build/phase12-gap18-evidence/input-saves \
  -DFRAMES=600 \
  -DROM=build/phase12-gap18-evidence/input.a78 \
  -DKEYBOARD=build/phase12-gap18-evidence/keyboard-input-trace-consent \
  -DINPUT_MODE=probe \
  -DTRACE_DIR=build/phase12-gap18-evidence/input-trace \
  -DTRACE_PARSER=build/ci/tests/muhtari-test-trace-protocol \
  -DRAW_SHOTS=build/phase12-gap18-evidence/ra-input-probe/shots \
  -DSHOT=build/phase12-gap18-evidence/input-shot-released.png \
  -P tests/retroarch/run_retroarch.cmake
```

## Owner decision

The prepared scope is one access request in the identified Apple Terminal session, then at most one nonprompting preflight and one bounded RetroArch keyboard diagnostic on this machine. Choose either to permit both the consent request and diagnostic, or to retain the refusal. No decision or partial approval authorizes any operation; all real-operation counters remain zero until a complete affirmative decision is recorded.
