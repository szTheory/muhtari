---
phase: 09-pal-games-and-audio
plan: "04"
subsystem: libretro
tags: [pal, palette, av, lifecycle, callbacks]
requires:
  - phase: 09-03
    provides: Safe PAL execution and approved warm region-aware host palettes
provides:
  - Reload-only auto/ntsc/pal console selection through existing load precedence
  - Live presentation-only auto/ntsc/pal warm palettes with neutral fallback
  - Loaded-module PAL AV, callback and lifecycle evidence in both pixel formats
affects: [09-05, 09-07, 09-08]
tech-stack:
  added: []
  patterns:
    - Capture console selection at successful load and resolve presentation against that session
    - Keep after-load AV queries distinct from callbacks restricted to retro_run
key-files:
  created: []
  modified:
    - libretro/libretro.c
    - tests/libretro/host.c
    - docs/libretro.md
key-decisions:
  - Preserve release-before-load adapter failure semantics and the distinct transactional library contract.
  - Missing or unknown region/palette options use Auto; explicit ntsc palette retains its spelling and colours.
  - Keep local pre-plan package comparison distinct from final hosted phase acceptance.
requirements-completed: []
coverage:
  - id: stable-region-options
    description: Both core-option versions declare auto/ntsc/pal; overrides apply on reload and remain stable across run and Restart
    requirement: PAL-01
    verification:
      - kind: integration
        ref: libretro.host.xrgb8888; libretro.host.rgb565; libretro.host.options
        status: pass
    human_judgment: false
  - id: regional-presentation
    description: All256 card colours and all292 PAL rows use selected warm presentation with exact AV and no AV update callbacks
    requirement: PAL-02
    verification:
      - kind: integration
        ref: libretro.host.xrgb8888; libretro.host.rgb565; host.palette
        status: pass
    human_judgment: false
  - id: callback-equivalence-and-lifetimes
    description: Nonzero exact PCM, stereo duplication, missing callbacks, reset/reload/failure/unload/reinit and neutral fallback agree with independent library sessions
    requirement: PAL-03
    verification:
      - kind: integration
        ref: libretro.host.xrgb8888; libretro.host.rgb565; libretro.host.audio
        status: pass
      - kind: other
        ref: Existing private module native/raw observation seams executed in XRGB8888 and RGB565
        status: pass
    human_judgment: false
  - id: local-product-and-packages
    description: Exact candidate and genuine pre-plan base pass complete local workflows and authenticate three native component archives
    requirement: PAL-03
    verification:
      - kind: other
        ref: cmake --workflow --preset ci; tests/cmake/quality_evidence.cmake
        status: pass
    human_judgment: false
actuals:
  tokens: 7534
  tasks: 2
  commits: 4
plan_head_before: 472b7c679d96826722a5b271441fc097237d2cf3
plan_head_after: 03646b339165345d8135c6b989aa26b1825155ee
duration: 14min
completed: 2026-10-25
status: complete
---

# Phase 9 Plan 4: Stable libretro region and palette sessions Summary

The loaded libretro core captures automatic or explicit console region at load, reports matching PAL AV and selects automatic or explicit warm presentation without changing active emulated time.

## Accomplishments

- Both option versions declare region and palette values auto/ntsc/pal, default Auto. Content load passes an explicit region through the existing media field; successful metadata resolution populates the retained adapter region. Firmware never chooses region. Neither run nor Restart reads a pending console-region choice into emulation. No new core storage, ABI declaration, dependency, timing loop or hosted scenario was added.
- Automatic palette follows the successful session; explicit warm NTSC/PAL updates presentation on option notification. Release clears selected AV, region, palette and native/converted pointers. Failed adapter loads discard the previous session and expose historical NTSC fallback, preserving their distinct contract from transactional library loads.
- The actual shipped XRGB8888 module and existing RGB565 module verify all256 test-card colours, independent RGB565 channel truncation, literal PAL B0/57/46 at native46 across all93440 bytes, including row291, and exact PAL960/962/961 startup sample counts. Every nonzero PCM sample agrees with an independent library instance and duplicates identically to stereo. Batch-only, per-sample-only, both callbacks and neither callback retain correct advancement. Canary-protected native reference storage and untouched NTSC tail rows pass.
- Scalar AV assertions retain320x292 actual/320x293 maximum, exact3546894/71051 FPS,147500000/129461631 aspect and48000Hz, plus independent decimal tolerance checks. The host rejects either AV/geometry update callback. Same-module tests cover NTSC card/cart, both override directions, pending region edits, Restart, PAL failure, unload, no-content NTSC despite a pending PAL option, missing/unknown values and deinit/reinit with cleared callbacks. Existing battery/counter paths remain green.
- One-time runs reuse the existing native/raw observation seams, comparing full native buffers, cumulative canonical raw transition digests/counts and PCM after resets, palette edits and missing callbacks. The installed module exports neither seam. The existing testdb module supplies XRGB8888; a locally compiled private RGB565 variant supplies the same source with the existing test macro. No private artifact is installed or committed.

## Task Commits

| Task | RED | GREEN |
|---|---|---|
| 1 — PAL AV and region-aware presentation | d20ae9e | fe65173 |
| 2 — Stable selection and fallback lifetimes | d2b845d | 03646b3 |

The persisted ledger measures four task commits before this summary commit. Actual tokens are7534, rounded-up characters/4 over the realized three-file diff, not harness usage. Duration14min covers17:37:52–17:51:45UTC, excluding initial reading. Three product/test/doc files changed; this summary is the fourth output file. Shared STATE/ROADMAP/requirements acceptance belongs to the orchestrator.

## Verification

Every command below exited0 unless a failure is explicitly retained. Every run URL is N/A-local.

- Task1: `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(libretro[.]host[.](xrgb8888|rgb565|audio)|host[.]palette|libretro[.]vendored)' --no-tests=error --output-on-failure` —6/6. Adding existing options coverage yielded8/8 in0.48s.
- Task2: `ctest --preset ci -R '^libretro[.]host[.](xrgb8888|rgb565|options)$' --no-tests=error --output-on-failure` —4/4 including its fixture,0.02s. Both loaded formats exercise the full sequence and all callback forms.
- Final source03646b3: `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" run-with-timeout 60 -- cmake --workflow --preset ci` —652 ordinary checks in24.48s and6 release checks in0.11s. The earlier unchanged working source also passed652+6 in25.12s/0.11s.
- `cmake --build --preset ci --target package && ctest --preset ci-release --output-on-failure` —6/6. The final-source workflow separately regenerated all three packages and reran all six release checks.
- `build/ci/tests/muhtari-test-libretro-host build/ci/tests/muhtari_libretro_testdb.dylib xrgb8888` —exit0, native/raw/PCM observation checks. `cc -dynamiclib -fvisibility=hidden -DMUHTARI_LIBRETRO_TEST -DUSE_RGB565 -Iinclude -Isrc -Ilibretro -Ihost -Irunner -Ibuild/ci/generated libretro/libretro.c runner/sha256.c build/ci/libmuhtari.a build/ci/libmuhtari_host.a -o build/ci/tests/muhtari_libretro_pal_private_rgb565.dylib && build/ci/tests/muhtari-test-libretro-host build/ci/tests/muhtari_libretro_pal_private_rgb565.dylib rgb565` —exit0 for the same checks in RGB565.
- `scripts/hygiene.sh --tree`, `git diff --check`, and `/opt/homebrew/opt/llvm@21/bin/clang-format --dry-run --Werror libretro/libretro.c tests/libretro/host.c` —exit0. `check evaluation-scope --plan 09-04 --commits-only --raw` resolves all four task commits on this branch. A read-only diff confirms the vendored header, workflow files and two hosted RetroArch cases are untouched.

## Local Package Measurements

`env GITHUB_EVENT_PATH="$PWD/build/ci/quality-libretro-local-event.json" cmake -DROOT=. -DBUILD=build/ci -DOUT=build/ci/quality-libretro-local.json -DSHA="$(git rev-parse HEAD)" -DCONFIG=ci -DSOURCE=N/A-local -DBUILD_BASE=ON -P tests/cmake/quality_evidence.cmake` —exit0 on exact source03646b339165345d8135c6b989aa26b1825155ee.

The ignored local event descriptor records genuine pre-plan472b7c679d96826722a5b271441fc097237d2cf3 and explicitly labels local slice scope. The existing producer created its detached baseline, copied existing pinned inputs and ran the complete baseline workflow:652+6,33.06s/0.10s. Both reports authenticate their exact HEAD, native configuration, manifest/sample identities, complete inventories and measured archive sums. A Node assertion compared equal toolchain/fixture environments, measured status/source/SHA and each report's sum against exactly three actual ZIP sizes; exit0.

| Source | Library | Libretro | Runner | Total bytes |
|---|---:|---:|---:|---:|
| Current03646b3 | 81761 | 87472 | 96375 | 265608 |
| Pre-plan472b7c6 | 81752 | 87073 | 96375 | 265200 |

The observed difference is408 bytes. These are report-only local slice measurements, without hosted, six-native, joined-lane or phase-versus-main acceptance claims. No quota was added. `git worktree remove build/ci/quality-base` exited0 after exact-base and clean tracked-state assertions; only the original checkout remains. Reports and logs remain ignored build output.

## TDD Gate Compliance

Both tasks have RED before GREEN. Unchanged complete CTest JUnit reports and actual exit8 were submitted to `check tdd-red-evidence`, which returned RED_EVIDENCE_OK for libretro.host.xrgb8888; both pixel-format targets executed and failed. Task1 exposed retained warm NTSC presentation instead of selected PAL. Task2 exposed ignored explicit PAL override:242 rows/54 startup samples instead of292/960. Semantic inspection distinguished those planned assertions from compile, fixture or discovery faults. Evidence records retain report timestamps, actual commands/status/output and semantic assessments in ignored build storage. No separate refactor commit was needed.

## Deviations and Retained Failed Experiments

- None in product scope or architecture. Task1 registers the region option alongside palettes; Task2 completes its captured override and lifecycle behavior. No requirements checkbox or phase acceptance is advanced.
- An early colour test used native18 with native46's expected RGB literal. The test literal was corrected before the accepted RED record. An initial tone program also left INPTCTRL unlocked; its TIA write changed decode before completing setup. The failed focused run exited8, and a later audible assertion exposed the silent program. The corrected owned program explicitly locks7800 decode, disables DMA and emits a nonzero tone; no core behavior was altered to accommodate the test.
- The first producer invocation exited1 with built SHA mismatch because its full SHA argument was incorrectly transcribed as03646b3ba22758038e9f62ff5cc358d285c182d5. The next invocation used actual git HEAD and produced candidate evidence, then exited1 when sandbox restrictions denied temporary worktree metadata creation. The authorized producer invocation above then completed both roots. Neither earlier attempt is counted as a successful comparison.
- The initial optional `cmake --preset asan` exited1 because the configured clang-21 executable name was absent from PATH. Binding the installed compiler with `cmake --preset asan -DCMAKE_C_COMPILER=/opt/homebrew/opt/llvm@21/bin/clang -DCMAKE_CXX_COMPILER=/opt/homebrew/opt/llvm@21/bin/clang++` and building succeeded. Sandboxed focused tests exited8 after both60s timeouts and /bin/ps denial. One unrestricted exact-source retry of `ctest --preset asan -R '^libretro[.]host[.](xrgb8888|rgb565)$' --no-tests=error --output-on-failure` again exited8 at60.07s before test output. A bounded stack-sample attempt exited255 because the timed-out process had already ended.
- An empty `int main(void) { return 0; }` compiled with that installed compiler's address/undefined sanitizers also timed out before output: `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" run-with-timeout 5 -- build/ci/pal-asan-empty` —exit124. This reproduces an environmental sanitizer-startup limitation independently of emulator/test code. No local sanitizer pass is claimed; no timeout, skip, workload or required/advisory authority changed.

## Deferred Issues

The optional local sanitizer observation remains unavailable as described above. Required local ci/package acceptance passes; final required hosted sanitizer/portability acceptance remains with the later phase plan. The best-effort `windows append --kind unrun-verify --phase 09 --file tests/libretro/host.c` attempt exited1 because the existing ledger frontmatter counts17/0/35/52 disagree with its entries16/0/36/52. No ledger entry was written; this existing metadata discrepancy was reported to the orchestrator without altering its shared files.

No known stubs, new trust-boundary surface, owner setup, identity consent or hardware/UAT request remains for this slice. Formal review-fix and gap rounds remain unused. PAL-01–03 stay pending successor implementation and final phase acceptance.

## Self-Check: PASSED

All three modified product/test/doc files exist, and all four task commits are ancestors of the phase branch with the exact ledger base and final source recorded above. Final-source652+6, explicit package integrity, both actual pixel modules and both private observation variants pass. SUMMARY creation and its metadata commit complete this executor's output; the orchestrator owns subsequent shared state updates.
