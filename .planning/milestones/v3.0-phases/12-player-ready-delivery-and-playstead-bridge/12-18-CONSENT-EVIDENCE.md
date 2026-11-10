# Phase 12 Plan 12-18: Consent and diagnostic evidence

**Disposition:** halted before the RetroArch diagnostic. The owner explicitly authorized the exact request and one local diagnostic. The request and its one nonprompting follow-up preflight both returned denied; no diagnostic invocation or content child occurred. No permission request retry is allowed under this plan.

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

`python3 build/phase12-gap18-evidence/consent.py audit-receipts` — exit 0 before the owner checkpoint. At that point the read-only audit found no decision or real-operation reservations/receipts. The post-operation audit also exited 0 and found exactly one request reservation and one standalone preflight reservation, both identity-bound; no diagnostic reservation or child was observed. Current code/pin diff against the frozen helper inputs is empty. Scratch binaries, logs, driver, and receipts are under ignored `build/phase12-gap18-evidence/`.

## Owner decision and executed operations

The owner explicitly chose **“Yes, authorize both exact operations”** in response to the checkpoint bound to the helper SHA, Terminal identity, pinned host, diagnostic core and cartridge identities above. The private decision receipt is `build/phase12-gap18-evidence/owner-decision.json`.

1. `python3 build/phase12-gap18-evidence/consent.py request-approved` — driver exit 0; its exclusive request reservation preceded the sole helper invocation. The helper's one `CGRequestPostEventAccess()` call completed normally with output `post-event-access-request=denied` and exit 77. Whether macOS displayed a prompt is unknown. No retry followed.
2. `python3 build/phase12-gap18-evidence/consent.py postflight` — driver exit 1 after the same helper's one `--preflight` call returned `post-event-access=denied`, exit 77. Its exclusive reservation and receipt were retained. The denied result stopped execution before any diagnostic reservation or RetroArch invocation.
3. `python3 build/phase12-gap18-evidence/consent.py diagnose-once` was not run. The helper's mandatory pre-fork recheck was not reached. The prepared diagnostic invocation was not reserved, no RetroArch process/content child launched, and no trace/screenshot or causal finding was produced.

The request result alone does not establish whether system permission state changed. No Settings automation, TCC inspection/reset, alternate identity, or second request was used. The local diagnostic remains unavailable under this plan because its required standalone preflight denied access.

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

## Final accounting and acceptance effect

| Operation | Reserved | Observed result |
|---|---:|---|
| Identity-bound access request | 1 | Completed normally; denied, helper exit 77 |
| Standalone nonprompting preflight | 1 | Denied, helper exit 77 |
| Helper-owned pre-fork permission recheck | 0 | Not reached |
| Diagnostic invocation | 0 | Not reserved because preflight denied |
| RetroArch/content child | 0 | Not launched |

There is no causal finding for the earlier hosted input failure. Plan 12-18 is halted, not passed. Plan 12-19 remains blocked because it requires a supported in-scope causal finding; Plan 12-16 remains gated on Plans 12-17/18/19. Phase 12 remains accepted 0/4; Plan 12-01 remains halted and Plans 12-02/03/04 remain blocked. Plan 12-17's WASI `tmpfile()` halt and the separate Phase 14 stop are unchanged.
