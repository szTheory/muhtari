---
phase: 10-homebrew-expansion-memory
plan: "05"
subsystem: hosts
tags: [runner, libretro, expansion, xm, hsc, saves, c17]
status: complete
requires:
  - phase: 10-04
    provides: API7 effective expansion selection, hidden XM HSC and continuous POKEY
provides:
  - Runner expansion selector, effective information, firmware/save setup and named exit77 sound refusals
  - Reload-only libretro expansion option with effective HSC discovery and stable physical SAVE_RAM
  - Actual process and loaded-module selection, firmware, failure and reload controls
affects: [10-06, 10-13, hosts, expansion-memory]
tech-stack:
  added: []
  patterns:
    - Small adapter-local load selection rules mirror core physical attachment
    - Existing process, module and firmware fault consumers cover new selectors
key-files:
  created: []
  modified:
    - runner/main.c
    - runner/exit_status.h
    - host/status_text.c
    - tests/runner/test_save.c
    - tests/runner/tests.cmake
    - docs/runner.md
    - libretro/libretro.c
    - tests/libretro/host.c
    - tests/libretro/bios.c
    - docs/libretro.md
    - libretro/muhtari_libretro.info.in
key-decisions:
  - Resolve physical expansion and HSC before firmware discovery, using the same rule for reload checks.
  - Retain release-before-load libretro sessions and clear all cached selection and storage on failure or unload.
  - Reuse the shared host status/name owner for generic device refusals and descriptive ADPCM output.
requirements-completed: []
requirements-acceptance: pending
coverage:
  - id: D1
    description: Runner exact selectors, raw versus effective information, requested firmware, pre-execution save import and named sound exit77
    requirement: BOARD-05
    verification:
      - kind: integration
        ref: runner.region.pal and runner.save.options and runner.save.roundtrip
        status: pass
      - kind: integration
        ref: ctest --preset ci -R '^(runner[.]|docs[.]exit_table)' --no-tests=error --output-on-failure
        status: pass
    human_judgment: false
  - id: D2
    description: Loaded libretro module selection, equivalent requests, reload warnings, immutable saves, named failures and clean unload/reload
    requirement: BOARD-05
    verification:
      - kind: integration
        ref: libretro.host.battery and libretro.host.battery_order and libretro.host.options
        status: pass
      - kind: integration
        ref: RGB565 module battery and hsc_firmware consumers
        status: pass
    human_judgment: false
  - id: D3
    description: Standalone HSC and automatic or explicit XM firmware discovery, present bad file refusal, detached HSC suppression and truthful subset documentation
    requirement: BOARD-05
    verification:
      - kind: integration
        ref: libretro.host.hsc_firmware and libretro.bios_failures and libretro.info_claims
        status: pass
    human_judgment: false
  - id: D4
    description: Final-source native workflow and authenticated actual three-component package comparison against the genuine pre-plan source
    requirement: BOARD-05
    verification:
      - kind: integration
        ref: sh build/ci/task08-jobserver-gate.sh at f9d451a
        status: pass
      - kind: integration
        ref: tests/cmake/quality_evidence.cmake with BUILD_BASE=ON against f3747a8
        status: pass
    human_judgment: false
actuals:
  tokens: 12322
  tasks: 2
  commits: 4
plan_head_before: f3747a8c271bebed41c7451796afb885f7929e24
plan_head_after: f9d451a3e318c19f97987f4c79da200525303ffc
duration: approximately 20min
completed: 2026-10-26
---

# Phase 10 Plan 05: Expansion Selection in Both Hosts Summary

**Runner and libretro select the same load-time expansion/HSC topology, discover only attached firmware, preserve physical saves, and name unsupported sound refusals.**

## Accomplishments

The runner accepts exactly `--expansion auto|off|xboard|xm`, once and with content. Explicit selection wins over the descriptive XM request. Opt-in information adds requested device/sound names, effective expansion and physical HSC attachment; ordinary output and the existing probe fields remain intact. XM HSC firmware is read before load and saves import before execution even while the CPU windows begin hidden. A firmware request without attached HSC fails before firmware file I/O. Unsupported devices return77, including independently declared YM2151, Covox and ADPCM with expansion Off. Board refusals retain their existing behavior.

The module declares the same four expansion values through both v2 and v1 option interfaces. Missing and unknown values use Auto. Effective XM provisions HSC unless explicitly suppressed; that same resolution governs optional highscre.bin discovery and reload checks. Expansion joins the cached request identity, while successful-load information supplies the installed identity. Equivalent requests do not warn, changed hardware warns once per changed request, and no late option changes SAVE_RAM bytes, pointer or length. Existing release-before-load behavior clears selection and spans on failure/unload. Logs distinguish raw requests and effective attachment, retain noncanonical byte63 warnings and name unavailable sound in both logs and frontend messages.

Actual module tests cover all selector values plus unknown/absent values across byte63 values0/1/2 and HSC Auto/On/Off, restored hidden storage,52KiB conflicts, late changes and reloads. Existing firmware fault tests now run for standalone HSC, header-selected XM and explicit XM. Actual file consumers prove missing/present/bad firmware, detached suppression, copied bytes and recovery after failed load. Additional real RGB565 module launches exercise both new consumer cases. API7/ABI1/behavior6 and core implementation remain unchanged.

## Task Commits

1. Task1 RED: `ac16935` — runner expansion/firmware assertions.
2. Task1 GREEN: `01a61d0` — selector, effective setup/info, named refusals and actual-process checks.
3. Task2 RED: `34a72b5` — actual loaded-module selection/reload assertions.
4. Task2 GREEN: `f9d451a` — frozen selection, effective discovery/reload checks, firmware controls and host documentation.

All four commits ran repository hooks with the configured GitHub noreply identity. Eleven product/test/documentation files changed; none was deleted. The persisted pre-plan ledger measures four commits and49287 realized diff characters, giving ceil(chars/4)=12322 estimated-scale actual tokens. This excludes the summary metadata commit. Shared STATE/ROADMAP/REQUIREMENTS acceptance is owned by the orchestrator and was not modified here.

## Verification and Failed History

Every run URL in this section is **N/A-local**. No execution log is committed.

| Command or check | Exit | Evidence and disposition |
|---|---:|---|
| `cmake --preset ci && cmake --build --preset ci` |0|Final native configure/build with fatal warnings. |
| `ctest --preset ci -R '^runner[.]save[.]options$' --output-junit runner-expansion-red.xml --output-on-failure` |8|The new test executes; XM selection returns usage2/no frames because the option is absent. |
| `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/ci/runner-expansion-red.json --raw` |0|Unmodified CTest JUnit accepted as RED_EVIDENCE_OK. Semantic inspection confirms the planned absent-selector assertion. |
| First runner GREEN focused command |8|Existing dual-EEPROM test expected1; the required unsupported-device mapping now returns77. Updated that expectation. |
| Early actual-process runner checks |8|New synthetic52KiB header used incorrect big-endian size offsets; then the new XM program's low reset-vector byte retained the jam fill. Corrected both new stimuli; no production timing/hash expectations changed. |
| `ctest --preset ci -R '^(runner[.]|docs[.]exit_table)' --no-tests=error --output-on-failure` |0|78 checks, including exact old transcripts, generated process selectors/probes, complete2048-byte post-import save result, named sound refusals and firmware errors. |
| `ctest --preset ci -R '^libretro[.]host[.]battery$' --output-junit libretro-expansion-red.xml --output-on-failure` |8|Actual module lacks explicit selection, effective reporting and expansion reload warnings. |
| `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/ci/libretro-expansion-red.json --raw` |0|RED_EVIDENCE_OK. The target executes and fails on absent adapter selection/cache behavior. |
| `ctest --preset ci -R '^(libretro[.]host[.]|libretro[.]bios|libretro[.]info_claims|libretro[.]vendored)' --no-tests=error --output-on-failure` |0|22 checks; all existing module/BIOS/input/audio checks plus expanded selection and firmware controls. |
| `build/ci/tests/muhtari-test-libretro-host build/ci/libretro/muhtari_libretro_rgb565.dylib battery` |0|Actual RGB565 module passes the expanded selection, save and reload matrix. |
| `build/ci/tests/muhtari-test-libretro-host build/ci/libretro/muhtari_libretro_rgb565.dylib hsc_firmware build/ci/tests/libretro-hsc-system` |0|Actual RGB565 module passes standalone/header-XM/explicit-XM file discovery and malformed-file recovery. |
| Final combined runner/libretro targeted filter |0|96 checks in5.92s after the last source/test edits. |
| `sh build/ci/task08-jobserver-gate.sh` at f9d451a |0|Unchanged workflow:658 tests in51.93s,6 release checks in0.11s, three actual ZIPs, repeated6 release checks in0.11s, exact658-once inventory and hygiene. |
| `sh build/ci/task10-05-quality.sh` at f9d451a |0|Existing BUILD_BASE producer; fresh genuine basef3747a8 passes658+6, ordinary42.39s and release0.09s; matching toolchain/fixture/sample identity. |
| `git diff --check`, file existence, commit ancestry and tracked-deletion checks |0|All eleven paths and four commits exist; no unexpected deletion or untracked generated output. |

The combined filter is `^(runner[.]|docs[.]exit_table|libretro[.]host[.]|libretro[.]bios|libretro[.]info_claims|libretro[.]vendored)` with the same `--no-tests=error --output-on-failure` flags. Both tasks have genuine RED/GREEN commits; no separate refactor was needed. Global TDD mode remains false.

The quality wrapper reuses the existing four-slot FIFO jobserver and executes:

`GITHUB_EVENT_PATH="$PWD/build/ci/task10-05-event.json" GITHUB_EVENT_NAME=push cmake -DROOT=. -DBUILD=build/ci -DOUT=build/ci/task10-05-quality-current.json -DSHA="$(git rev-parse HEAD)" -DCONFIG=ci -DBUILD_BASE=ON -DSOURCE=local -P tests/cmake/quality_evidence.cmake`

The event before is genuine pre-plan f3747a8c271bebed41c7451796afb885f7929e24; candidate is f9d451a3e318c19f97987f4c79da200525303ffc. The independent clean comparison worktree was removed after measurement. JSON, wrappers and generated media remain ignored local output.

| Actual macOS arm64 ZIP | Candidate bytes | Genuine pre-plan bytes |
|---|---:|---:|
| Library |83245|83213|
| Runner |98477|97788|
| Libretro |89617|88705|
| Three-component sum |271339|269706|

Archive bytes remain report-only under DEC.61. This is local per-plan evidence, not phase-versus-main, physical-hardware or all-platform hosted acceptance.

## Deviations from Plan

The existing real-process driver in tests/runner/tests.cmake owns exact opt-in information expectations and subprocess assertions. It was extended instead of adding a second process harness to tests/runner/test_runner.c. The existing shared host/status_text.c owner needed a generic unsupported-device description and appended ADPCM name; duplicating the name table in both adapters would leave probe output wrong. These two necessary owner adjustments preserve the existing mechanisms, registrations and exact inventory; no no-op test_runner.c edit was made.

The changed unsupported-device exit mapping intentionally updates the existing duplicate-EEPROM refusal expectation to77. New synthetic-header/reset-vector mistakes were corrected as described above. No old frame/audio pins, ABI constants, behavior revision, resource limits or default output expectations were relaxed.

Best-effort `windows append --kind deviation --phase 10 --file host/status_text.c` returned1 because the pre-existing ledger frontmatter17/0/35/52 disagrees with entries16/0/36/52. No ledger content changed. This optional recorder limitation is unchanged from the prior plan; no required verification is missing.

## Architecture and Limits

Adapters own options and firmware I/O; the core still owns validation and physical storage. Small load-selection helpers express the present shared rule without a configuration framework. Firmware discovery retains existing regular-file validation and bounded4097-byte reads; detached requests avoid firmware I/O. No new endpoint, schema, dependency, device I/O, allocation inside the core, quota or runtime fault protocol was introduced.

XM remains the documented RAM/POKEY/HSC subset, with medium physical confidence. YM2151, Covox/ADPCM, internal XM BIOS/top-slot switching, flash, PIA, flat48KiB mode, write protection, sticky locks, ROF aliases and PAL-specific firmware selection remain unavailable. Accepting the subset makes no full-XM service or gameplay claim. The existing signature-only HSC fallback intentionally supplies no firmware routines, formatting or screens; supplied firmware remains optional and separately validated. No new stub, skipped test or unrun plan verification was introduced.

## Self-Check: PASSED

After writing this summary, all eleven changed product files, the summary itself and all four task commit ancestors were verified. Coverage classification accepts four deterministic deliverables. Final-source focused/full checks, independent package evidence and hygiene pass. No routine owner verification remains.

## Next Readiness

Host selection and effective firmware/save consumers are ready for the descriptor plan. BOARD-04/05 remain pending: the read-only requirements readiness query returns both blocked by unfinished sibling plans. Final hosted acceptance is owned by10-13. Next separate allowed command: `$gsd-execute-phase 10`. No other GSD step was run.
