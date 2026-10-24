---
phase: 07-battery-saves
plan: "09"
status: complete
subsystem: integration
tags: [battery, equality, retroarch, portability, archives]
requires:
  - phase: 07-battery-saves
    provides: Tested HSC and EEPROM devices, runner persistence, loaded frontend storage and admitted owned diagnostic from plans 01 through 08
provides:
  - Independently validated first and second battery observations in complete native PowerPC and WASM equality
  - Evidence-backed battery-save and optional HSC firmware metadata
  - Exact-source successful CI and nightly with six authenticated current and independent-base archive sums within 399937 bytes
affects: [phase-7-verification, shipping]
tech-stack:
  added: []
  patterns:
    - Reuse the diagnostic mailbox and full-media oracle before hashing fresh processes
    - Inspect only file type through the existing platform owner instead of requesting unused CRT metadata
key-files:
  created: []
  modified:
    - tests/cmake/hashes.cmake
    - tests/runner/tests.cmake
    - tests/policy/tests.cmake
    - libretro/muhtari_libretro.info.in
    - tests/cmake/info_claims.cmake
    - CMakeLists.txt
    - libretro/libretro.c
    - tests/libretro/bios.c
    - tests/audio/test_contracts.c
    - tests/core/test_core.c
    - tests/boot/test_boot.c
    - docs/libretro.md
    - docs/emulation-tests.md
    - docs/testing.md
    - docs/quality.md
    - .planning/preparation/HOSTS-AND-INTEGRATION.md
key-decisions:
  - Preserve all 47 existing observation rows and permit only the five named behavior-version metadata transitions from 3 to 4.
  - Keep the archive ceiling and actual independent event-parent authority unchanged; fix measured production size instead.
  - Use matching legacy PPC stat layout and bounded heap-owned test snapshots without raising portability floors or the WASM stack.
requirements-completed:
  - SAVE-01
  - SAVE-02
  - SAVE-03
  - SAVE-04
  - SAVE-05
  - SAVE-06
coverage:
  - id: hsc-firmware-and-sram
    description: Complete HSC and firmware consumers pass across native and PowerPC with supplied firmware and the deliberately limited signature stub
    requirement: SAVE-01
    verification:
      - kind: integration
        ref: "cmake --workflow --preset ci; complete inventories 641+6; exit 0; https://github.com/szTheory/muhtari/actions/runs/40095781311"
        status: pass
      - kind: integration
        ref: "be-equality full native and PowerPC inventories 641+6 each, release floor and 69-row equality; exit 0; https://github.com/szTheory/muhtari/actions/runs/40095782965/job/120877638097"
        status: pass
    human_judgment: false
  - id: serial-eeprom-contract
    description: Real RIOT serial transactions and retained protocol properties and I2C fuzz consumers pass with independent EEPROM counters and complete media checks
    requirement: SAVE-02
    verification:
      - kind: integration
        ref: "Nightly seeded properties MUHTARI_PROP_CASES=10000, all six property tests; exit 0; https://github.com/szTheory/muhtari/actions/runs/40095782965/job/120877638072"
        status: pass
      - kind: integration
        ref: "cmake --workflow --preset fuzz; loader/run/I2C fuzz and regression controls, exact inventory 10; exit 0; https://github.com/szTheory/muhtari/actions/runs/40095782965/job/120877638037"
        status: pass
    human_judgment: false
  - id: stable-raw-media-and-equality
    description: Two fresh processes share only save storage and independently validate counters 1/1 then 2/2 and every one of 34816 raw bytes before emitting 22 battery rows
    requirement: SAVE-03
    verification:
      - kind: integration
        ref: "runner.hashes_file malformed/missing/duplicate/revision/numeric/media controls, six-native equality and full native/PowerPC byte equality of 69 rows; exit 0; https://github.com/szTheory/muhtari/actions/runs/40095781311/job/120882923274"
        status: pass
      - kind: integration
        ref: "WASM exact inventory 580 and cmp against Linux x64 complete hash file; exit 0; https://github.com/szTheory/muhtari/actions/runs/40095781311/job/120882923223"
        status: pass
    human_judgment: false
  - id: runner-import-and-fresh-reload
    description: Existing exact raw import and refusal controls plus two independently judged fresh-process exports pass on all six native platforms
    requirement: SAVE-04
    verification:
      - kind: integration
        ref: "Six actual native current and independently built parent workflows, each 641+6 including runner.save consumers and fresh battery observations; exit 0; https://github.com/szTheory/muhtari/actions/runs/40095781311"
        status: pass
    human_judgment: false
  - id: persistence-controls-and-replacement
    description: Cadence and exit persistence retain all failure and commit-point controls with actual Windows console-event and replacement execution on x64 and arm64
    requirement: SAVE-05
    verification:
      - kind: integration
        ref: "Actual runner.save.controls and runner.save.faults in full Windows x64 current/base workflows; exit 0; https://github.com/szTheory/muhtari/actions/runs/40095781311/job/120877658058"
        status: pass
      - kind: integration
        ref: "Actual runner.save.controls and runner.save.faults in full Windows arm64 current/base workflows; exit 0; https://github.com/szTheory/muhtari/actions/runs/40095781311/job/120877658046"
        status: pass
    human_judgment: false
  - id: actual-frontend-save-roundtrip
    description: Exactly two hosted cases retain DMA and prove fresh frontend battery counters and complete raw snapshots against independent expectations and fresh runner processes
    requirement: SAVE-06
    verification:
      - kind: e2e
        ref: "RetroArch 1.22.2 actual DMA and two-launch battery case, all 34816 bytes per host/launch, independent pixel oracle faults and retained DMA negative control; exit 0; https://github.com/szTheory/muhtari/actions/runs/40095781311/job/120882923165"
        status: pass
      - kind: automated_ui
        ref: "Agent viewed final-source DMA and both battery screenshots; counter 1/1 then 2/2 and supplied marker 5A then stub FF; strict all-pixel comparisons and unsafe-path isolation controls pass; https://github.com/szTheory/muhtari/actions/runs/40095781311/job/120882923165"
        status: pass
    human_judgment: false
  - id: truthful-capability-and-package-claims
    description: Optional highscre.bin and battery saves are declared only after actual host proof and every real native three-component archive sum and independently built parent fits the unchanged ceiling
    requirement: SAVE-06
    verification:
      - kind: integration
        ref: "info_claims actual host registrations and authenticated quality join: current worst 393959, independent parent worst 393990, both <=399937; exit 0; https://github.com/szTheory/muhtari/actions/runs/40095781311/job/120882923213"
        status: pass
    human_judgment: false
duration: 197min
completed: 2026-10-24
actuals:
  tokens: 12004
  tasks: 3
  commits: 14
plan_head_before: 4223f7ba8b8e17a42001dee029da1db7781e4fb3
plan_head_after: fae8e8fb5010b38965f00c090de6bd564a01500a
---

# Phase 7 Plan 9: Final Battery Acceptance Summary

Independently judged battery observations, truthful frontend claims and all six native packages pass at the unchanged archive bound, with full native/PowerPC/WASM and actual RetroArch evidence.

## Source and Authority

Final tested source is `fae8e8fb5010b38965f00c090de6bd564a01500a`. Both final workflows completed successfully on that exact source:

- `gh run view 40095781311 --json headSha,status,conclusion,jobs`: exit 0, CI success, including the advisory WASM job. https://github.com/szTheory/muhtari/actions/runs/40095781311
- `gh run view 40095782965 --json headSha,status,conclusion,jobs`: exit 0, Nightly success. https://github.com/szTheory/muhtari/actions/runs/40095782965

The authenticated independent event parent is the genuine first parent `8d0051823cb3ec99ffac2fb817f58ffc9a21fc6f`, not a candidate-manufactured observation or substituted base. Every lane actually rebuilds it. Downloaded raw base policies are byte-exact with unchanged `quality.json`; all six base identity files match that parent. The existing producer and join retain source, SHA, platform, compiler, configuration, policy and fixture authority. Final archive metrics equal the sums of the actual downloaded three ZIPs on each lane.

The performance dispatch retains its existing authority: the same head is measured three times, yielding `3837653,3837653,3837653 Ir/frame`, spread zero. It does not invent a comparative PR baseline; the unchanged join reports comparative Ir unavailable when not requested. Required/advisory status is unchanged. Compatibility retains its ledger judge, pins and allow file.

## Completed Tasks and Commits

| Task | Result | Commits |
|---|---|---|
| 1 | Validate complete first/second battery observations, old row preservation and real malformed/duplicate/missing controls | `8db47aa`, `a15ab5e` |
| 2 | Enable proved battery/optional-firmware metadata and reconcile public/preparation prose | `9789903` |
| 3 | Actual six-native/current/base, complete BE, nightly, WASM and frontend acceptance with size and portability fixes | `c03e212`, `a67bf64`, `061261b`, `88836c7`, `b404c03`, `a126aaa`, `400babb`, `788798a`, `4db85f1`, `8d00518`, `fae8e8f` |

The persisted plan ledger measures 14 commits through the tested source. Actual tokens are 48016 realized diff characters divided by four, excluding this summary and subsequent tracking.

## Observations and Exact Inventories

The previous accepted Phase 6 output at `2fe713c1074a63bbf89384d56200bd48060c0549` supplies the independent 47-row comparison. All old identities, ordering, digest and numeric observations match exactly. Only five designated `audio_behaviour_revision` annotations advance 3 to 4. The 22 appended `battery.first.*`/`battery.second.*` rows include frame, raw audio/PCM, complete mailbox and saved-media SHA-256 after the existing independent numeric and full-media driver validates them. Both fresh runner processes share only their private save directory.

The real producer rejects malformed, missing, duplicate, wrong-revision and invalid-numeric reports and invalid media. The comparison rejects equal-but-incomplete or duplicate old/new files. Existing policy equality fixtures now carry the complete required schema instead of a one-row synthetic pass. No extra test registration or inventory removal was needed: native remains 641 normal plus 6 release; portable WASM is 580. Six native hash files, full native/PowerPC files and WASM are all byte-identical at 69 rows.

`quality.json`, `pins.txt`, `quality_join.cmake`, `lane_equality.cmake`, platform-floor and performance predicates, performance/compatibility allows and the compatibility ledger are unchanged. Existing sample/frame/audio goldens are preserved except the previously authorized behavior metadata transition.

## Actual Three-Component ZIP Measurements

All commands below exit 0. Run URL for every row: https://github.com/szTheory/muhtari/actions/runs/40095781311

| Native lane | Library | Runner | Libretro | Current sum | Independently built parent sum |
|---|---:|---:|---:|---:|---:|
| Linux arm64 | 87845 | 103973 | 92597 | 284415 | 284415 |
| Linux x64 | 85802 | 101782 | 90001 | 277585 | 277585 |
| macOS arm64 | 72507 | 84361 | 73572 | 230440 | 230436 |
| macOS x64 | 74099 | 84566 | 74002 | 232667 | 232669 |
| Windows arm64 | 87120 | 153812 | 130154 | 371086 | 371162 |
| Windows x64 | 88068 | 166307 | 139584 | 393959 | 393990 |

`gh run download 40095781311 --name archives-<lane> --dir <ignored-build-dir>` and the corresponding `quality-<lane>` download exit 0. Python standard-library enumeration asserts exactly one library, runner and libretro ZIP per lane, sums their actual filesystem byte sizes, matches authenticated current metrics and asserts both current/base sums <=399937 with exact expected SHA/source identities. Separate byte comparison checks all six raw base policies and actual first-parent identities, exit 0.

Early plan 07-02 measured local macOS arm64 218325 bytes. Final macOS arm64 is 230440. That early local result never established a worst-platform bound. The final Windows measurements, including intermediate failures, establish the actual feasibility result without a grant or rebaseline.

## Verification Commands

Local commands at the final source, run URL `N/A-local`, all exit 0:

- `cmake --workflow --preset ci`: 641 normal plus 6 release checks pass.
- `cmake --build --preset ci --target package` and `ctest --preset ci-release --output-on-failure`: actual packages and all 6 release checks pass.
- `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake`: exactly 641 once each. The corresponding `junit-release.xml` check with `-DLABELS=release` reports exactly 6.
- `cmake --workflow --preset hygiene`, its exact JUnit inventory and `scripts/hygiene.sh --tree`: 24 hygiene checks, complete tree and analyzer pass.
- `cmake -DROOT=<absolute-project-root> -P tests/cmake/budgets.cmake`: 19 files within unchanged bounds. `git diff --check` passes.
- Independent accepted-47/current-69 Python comparison and downloaded full native/PowerPC `cmp` checks pass. All created/modified paths and all 14 commits exist and are reachable.

Final hosted CI command families, exit 0: `cmake --workflow --preset ci` or `ci-msvc` on all six targets and their independently built parent; exact normal/release inventories; ASan current/base 626 each; no-floating-point lanes; library-only; retained compatibility; six-native equality; authenticated quality join; real RetroArch; WASM. Source/run: https://github.com/szTheory/muhtari/actions/runs/40095781311 . WASM executes 580 once each and its complete output compares equal to Linux x64, preserving its advisory status.

Final Nightly command families, exit 0: full native and PowerPC `ci` inventories 641+6 each and 69-row equality; full-vector workflow and exact inventory 258; six seeded property tests with `MUHTARI_PROP_CASES=10000`; fuzz workflow and exact inventory 10, including actual loader/run/I2C fuzz runs and regression controls; all retained native/WASM benchmark jobs. Source/run: https://github.com/szTheory/muhtari/actions/runs/40095782965 . The full PPC package floor passes, not merely a cross-compile probe.

Actual Windows x64 and arm64 full current/base jobs execute runner console events, save replacement and failure controls and the real adapter file/allocation faults. The new Windows probe controls exercise normal/missing/path-missing/access/directory/pipe/character/unknown-type/close outcomes through the actual adapter public load consumer, asserting one close per successful probe and no firmware read after a refused type/probe. Both Windows jobs and inventories pass.

## Actual Frontend Judgment

Exactly two hosted cases remain: retained DMA and the battery roundtrip, whose two launches are fresh frontend processes. The first supplies owned synthetic firmware, the second has no firmware and uses the limited stub. Fresh runner processes use separate save storage from the frontend; neither host's media is seeded from the other's output. Independent counters are 1/1 then 2/2, firmware pixel marker is 5A then FF, every one of 34816 bytes per host/launch matches independent expectations, and corresponding raw files compare equal.

Agent inspection of final-source DMA and both battery screenshots confirms the legible expected fields. Strict whole-frame comparison passes; the independent battery oracle rejects its constant/wrong-count/single-field controls, and flipping DMA pixel 7,3 still exits 1 naming exactly that pixel. Unsafe equal/child/parent save/run and firmware overlap controls refuse before disposable cleanup and preserve the sentinel. Launch directories remain private; only explicit save storage survives the pair. `tests/retroarch/*` totals exactly 300 physical lines. No owner UAT remains.

`libretro_saves=true` and optional firmware entry 2/count 3 now map to actual registered host battery/firmware tests and have this separate actual hosted proof. Save states, serialization, speech and full firmware routines remain unavailable. Unknown firmware and the signature-only stub retain distinct warnings and their documented scope.

## TDD Gate Compliance

Task 1 followed canonical task-level RED/GREEN. Actual command `ctest --preset ci -R '^runner[.]hashes_file$' --no-tests=error --output-on-failure --output-junit battery-hashes-red.xml` exited 8: the real producer supplied 47 rows and the target specifically required 69 complete observations. Machine classification returned `RED_EVIDENCE_OK`; semantic inspection confirmed the target assertion failed rather than setup. GREEN implements independent validation and complete controls, then passes full CI. No separate refactor was needed. Global TDD mode is false; no MVP gate was active. Reports remain ignored runtime evidence, never committed logs.

## Deviations from Plan

1. **Rule 3 — measured archive feasibility.** Actual Windows x64 failed at 408168 bytes. Top-level MSVC `/O1 /Ob1` reduced it to 404284, still failing. Host-only IPO measured 404125, only 159 bytes better, and was removed with a scoped ordinary commit. The narrow Windows HSC owner now uses existing Kernel32 CreateFileA/GetFileType/CloseHandle to inspect disk-file type, refusing directories/devices/pipes/API and close failures without requesting unused CRT time metadata. It preserves ownership and the subsequent exact firmware read. Current 393959 and independently built parent 393990 now fit. No new dependency, public-library LTO requirement, quota, archive grant or weaker predicate was introduced. Commits `c03e212`, `a126aaa`, `400babb`, `788798a`, `4db85f1`, `8d00518`, `fae8e8f`.
2. **Rule 1 — bounded WASM test stack.** Whole-instance snapshots outgrew the unchanged WASM stack after battery storage expanded the instance. Three test owners now allocate/free snapshots while retaining the complete bytewise observer/refusal assertions. No stack increase, skipped test or narrowed comparison. Commit `88836c7`; actual final WASM 580 passes.
3. **Rule 1 — required PowerPC floor.** Modern PPC32 headers selected time64 stat and imported GLIBC 2.34. The adapter requests the matching legacy layout before headers and extends its existing stable __xstat owner with PPC version 3. It only inspects type, never timestamps. The actual full PPC package and floor pass at the retained GLIBC 2.17 contract. Commit `b404c03`.
4. **Rule 3 — unchanged documentation budget and Windows test namespace.** Consolidated testing prose from 41481 to 40805 bytes rather than increasing its 40960 bound (`a67bf64`). Including Windows headers exposed the existing test token SHORT as a typedef collision; renamed that token TRUNCATED without changing cases (`8d00518`). Both actual Windows consumers now compile and pass.

Intermediate authentic failures remain failures: CI40086636081 exposed size and WASM stack; CI40089873759 proved portability/hosted behavior but still failed archive size; CI40094062390 measured the insufficient IPO candidate. CI40095611107 exposed the test token collision and was cancelled after its failure; CI40095701704 passed current Windows tests then correctly failed compiling its pre-fix independent parent. Final CI uses the genuine corrected parent and succeeds. Some unobserved local runs were interrupted by host sleep/load and timed out; directly observed unchanged commands under caffeinate pass. No timeout grant was used for these interruptions.

## Boundary and Scope Assessment

Storage and device semantics remain in their existing core owners, runner I/O remains in the host, and the adapter consumes public interfaces. The final file-type change is confined to the existing optional-firmware owner and its real consumer seam. Hash production reuses the existing independent driver, equality keeps full-file and identity authority, and all material checks exercise representative pass/fault behavior. No new trust input, registry, mandatory maintenance bound, architectural layer or dependency was added. Existing intentional stub limitations are product scope, not unfinished functionality. No security-relevant surface outside the assigned boundaries was introduced.

No phase code-review fix round or gap round was consumed by this planned execution and measured feasibility work. Formal phase review/verifier/completion remain with the execute-phase orchestrator. No additional GSD step, PR, merge or ship was invoked; no phase-complete claim is made here.

## Self-Check: PASSED

All 16 modified owners exist, all 14 measured commits are reachable, source/run SHAs match, final CI and Nightly are successful, all current/base native archives fit, complete inventories and required fault controls pass, and no unexpected deletion, committed log/runtime media, untracked generated file, unresolved implementation stub or unrun plan verification remains. This summary completes plan 07-09; phase review and verification remain pending.
