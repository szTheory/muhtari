---
phase: 20-complete-ym2151-homebrew-sound
plan: "05"
subsystem: portable-packages
requires:
  - phase: 20-04
    provides: Checked native YM fixtures and receipts
provides:
  - Actual Wasm YM continuation and callback alternatives
  - Installed and clean-extracted native package execution
  - Clean portable ZIP execution and exact evidence transport
plan_head_before: be7d28d
plan_head_after: 4263f20
requirements-completed: []
coverage:
  - id: wasm-ym
    description: Actual pinned emitted Wasm matches both native workloads and regions with replay, reset, isolation, callbacks, extents and refusals.
    requirement: YM-07
    verification:
      - kind: integration
        ref: "sh scripts/emscripten.sh build/ci; build/v6-phase20-05-portable-final.log; exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: native-package-ym
    description: Both installed CRT consumers and freshly extracted library, runner and loaded libretro execute the same numeric diagnostic and state continuation.
    requirement: YM-06
    verification:
      - kind: integration
        ref: "build/v6-phase20-05-native-formatted.log; installed/extracted native CTest gate; exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: portable-package-integrity
    description: Extracted loader/Wasm and source/fixture/PCM/notice controls plus transferred YM receipt completeness use the existing package and publisher paths.
    requirement: YM-07
    verification:
      - kind: integration
        ref: "emscripten.bundle and emscripten.bundle.controls in portable-final log; exit 0; URL N/A-local"
        status: pass
      - kind: integration
        ref: "build/v6-phase20-05-publish-production.log; real ZIP plus all local mocked publisher controls; exit 0; URL N/A-local"
        status: pass
    human_judgment: false
status: complete
---

# Actual portable and package delivery

The pinned offline SDK emits production loader/Wasm bytes that execute both exact named YM cartridges in NTSC and PAL. Numeric mailbox, signed PCM extrema, duplicated mono, frame/state hashes and exact state extent precede parity. Batch, per-sample and absent audio routes, reset, repeated restore, independent factories, battery ownership, oversized tails, corrupted and repaired-CRC semantic state, unsupported reload and callback/export faults are exercised. Current input-corpus checks remain intact.

Native packages are consumed after clean, physically bounded extraction and exact member/type/expansion preflight. The external public-header-only consumer links only the extracted find_package library, the runner process executes the extracted runner, and the established dynamic host loads the extracted core. The same ordinary fixture supplies independent numeric and continuation checks. Both staged CRT configurations retain their original no-argument smoke and gain YM execution.

Source identity compares the actual packaged bytes to the unique CPack stage. On Unix it also compares every library object payload against the current build: installation can rewrite an archive index without changing its code. Source/core substitution, corrupt content, missing notices and existing outside-sentinel controls stay active. The portable ZIP member set remains unchanged; YM content and receipts are external evidence. The existing CI artifact now transfers exactly two native receipts, two measurement receipts and two generated cartridges. The mocked publisher rejects a missing worst native receipt before retaining checksums.

Evidence, all exit 0 and URL N/A-local:

| Command | Result |
|---|---|
| sh scripts/emscripten.sh --preflight build/ci | Existing exact SDK, bundled Node, CMake and notices qualified |
| sh scripts/emscripten.sh build/ci | Complete emitted/extracted inventory and native/Node measurements for both workloads and regions |
| cmake --preset ci; cmake --build build/ci --parallel 2 | Exact native registration/build |
| cpack --preset ci | Fresh three native archives |
| ctest --test-dir build/ci --output-on-failure --no-tests=error --parallel 1 -R '^(ym[.]fixture\|install[.]consumer([.]dll_crt)?([.]ym)?\|release[.]ym_consumers)$' | All installed and extracted YM consumers, after pinned C formatting |
| CTest release archive_contents and firmware_scan, including their self tests | Existing integrity controls retained |
| cmake -DROOT=. -DWORK=build/v6-phase20-publish-production -DPRODUCTION_ARCHIVE=build/emscripten/muhtari-libretro-emscripten.zip -DEXPECTED_SHA=be7d28d8f1718b1cd8e15942634d6ca83798352e -P tests/policy/publish.cmake | Invocation used resolved absolute paths and full HEAD; actual ZIP, transfer-only evidence and mocked publishing controls pass |
| node --check tests/emscripten/host.mjs; sh -n scripts/emscripten.sh; git diff --check | Syntax and patch checks |

The first extracted-library identity comparison failed because raw archive index bytes differ after install. Object-by-object comparison plus unique CPack staging fixed the cause without bypassing identity or weakening digest/member controls. An earlier direct observer input check used explicit joystick/proline on a SaveKey port; automatic effective input selection fixed that fixture path in Plan 20-04. No failed result is recast as a pass; ignored logs retain attempts.

Bounded plan deviations: one small CMake portable invocation helper; existing native/WASI inventory ownership; CI upload's six explicit evidence paths; shared preflight factoring preserves the portable default and adds only known zero-sized native parent directories. No remote dispatch, upload, signing or publication was performed. Final-source gates, complete worst-legal cost qualification and genuine compatibility remain downstream. No formal review or gap round consumed.

## Self-Check: PASSED

Source commits, new helper files and summary exist. Named local gates exited zero; pending final-source/milestone acceptance is explicit.
