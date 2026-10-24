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
    - tests/runner/test_control.c
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
        ref: "cmake --workflow --preset ci; complete inventories 641+6; exit 0; https://github.com/szTheory/muhtari/actions/runs/40104749040"
        status: pass
      - kind: integration
        ref: "be-equality full native and PowerPC inventories 641+6 each, release floor and 69-row equality; exit 0; https://github.com/szTheory/muhtari/actions/runs/40104750452/job/120904008838"
        status: pass
    human_judgment: false
  - id: serial-eeprom-contract
    description: Real RIOT serial transactions and retained protocol properties and I2C fuzz consumers pass with independent EEPROM counters and complete media checks
    requirement: SAVE-02
    verification:
      - kind: integration
        ref: "Nightly seeded properties MUHTARI_PROP_CASES=10000, all six property tests; exit 0; https://github.com/szTheory/muhtari/actions/runs/40104750452/job/120904008627"
        status: pass
      - kind: integration
        ref: "cmake --workflow --preset fuzz; loader/run/I2C fuzz and regression controls, exact inventory 10; exit 0; https://github.com/szTheory/muhtari/actions/runs/40104750452/job/120904008773"
        status: pass
    human_judgment: false
  - id: stable-raw-media-and-equality
    description: Two fresh processes share only save storage and independently validate counters 1/1 then 2/2 and every one of 34816 raw bytes before emitting 22 battery rows
    requirement: SAVE-03
    verification:
      - kind: integration
        ref: "runner.hashes_file malformed/missing/duplicate/revision/numeric/media controls, six-native equality and full native/PowerPC byte equality of 69 rows; exit 0; https://github.com/szTheory/muhtari/actions/runs/40104749040/job/120911136284"
        status: pass
      - kind: integration
        ref: "WASM exact inventory 580 and cmp against Linux x64 complete hash file; exit 0; https://github.com/szTheory/muhtari/actions/runs/40104749040/job/120911136298"
        status: pass
    human_judgment: false
  - id: runner-import-and-fresh-reload
    description: Existing exact raw import and refusal controls plus two independently judged fresh-process exports pass on all six native platforms
    requirement: SAVE-04
    verification:
      - kind: integration
        ref: "Six actual native current and independently built parent workflows, each 641+6 including runner.save consumers and fresh battery observations; exit 0; https://github.com/szTheory/muhtari/actions/runs/40104749040"
        status: pass
    human_judgment: false
  - id: persistence-controls-and-replacement
    description: Cadence and exit persistence retain all failure and commit-point controls with actual Windows console-event and replacement execution on x64 and arm64
    requirement: SAVE-05
    verification:
      - kind: integration
        ref: "Actual runner.save.controls and runner.save.faults in full Windows x64 current/base workflows; exit 0; https://github.com/szTheory/muhtari/actions/runs/40104749040/job/120904022224"
        status: pass
      - kind: integration
        ref: "Actual runner.save.controls and runner.save.faults in full Windows arm64 current/base workflows; exit 0; https://github.com/szTheory/muhtari/actions/runs/40104749040/job/120904022187"
        status: pass
    human_judgment: false
  - id: actual-frontend-save-roundtrip
    description: Exactly two hosted cases retain DMA and prove fresh frontend battery counters and complete raw snapshots against independent expectations and fresh runner processes
    requirement: SAVE-06
    verification:
      - kind: e2e
        ref: "RetroArch 1.22.2 actual DMA and two-launch battery case, all 34816 bytes per host/launch, independent pixel oracle faults and retained DMA negative control; exit 0; https://github.com/szTheory/muhtari/actions/runs/40104749040/job/120911136203"
        status: pass
      - kind: automated_ui
        ref: "Agent viewed final-source DMA and both battery screenshots; counter 1/1 then 2/2 and supplied marker 5A then stub FF; strict all-pixel comparisons and unsafe-path isolation controls pass; https://github.com/szTheory/muhtari/actions/runs/40104749040/job/120911136203"
        status: pass
    human_judgment: false
  - id: truthful-capability-and-package-claims
    description: Optional highscre.bin and battery saves are declared only after actual host proof and every real native three-component archive sum and independently built parent fits the unchanged ceiling
    requirement: SAVE-06
    verification:
      - kind: integration
        ref: "info_claims actual host registrations and authenticated quality join: current worst 393952, independent parent worst 393994, both <=399937; exit 0; https://github.com/szTheory/muhtari/actions/runs/40104749040/job/120911136250"
        status: pass
    human_judgment: false
  - id: closed-pipe-portability-correction
    description: Closed-pipe final and during-output child regressions retain every exit and raw-byte assertion while using the existing slow-lane scale in their bounded wait
    requirement: SAVE-05
    verification:
      - kind: integration
        ref: "Actual full PowerPC runner.save.controls passes in 7.82 seconds with existing scale 13 and all 34816-byte checks; exit 0; https://github.com/szTheory/muhtari/actions/runs/40104750452/job/120904008838"
        status: pass
      - kind: other
        ref: "Actual isolated qemu-ppc consumer exits 0 in 4.92 seconds; same native consumer rejects hung-child and wrong-exit wrappers with bounded cleanup, expected exit 1 at 13.36 and 0.23 seconds; probe exits 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: reviewed-controller-regressions
    description: Loaded-module held-A storage-request cases and 112 automatic-header combinations retain effective button mapping and core-matching fallback without input refusal
    requirement: SAVE-06
    verification:
      - kind: integration
        ref: "Full final-source native current/base inventories execute libretro.host.layout and libretro.host.devices including the reviewed semantic regressions; exit 0; https://github.com/szTheory/muhtari/actions/runs/40104749040"
        status: pass
    human_judgment: false
duration: 328min
completed: 2026-10-24
actuals:
  tokens: 31541
  tasks: 3
  commits: 28
plan_head_before: 4223f7ba8b8e17a42001dee029da1db7781e4fb3
plan_head_after: 54485d630c421716d51a8d7919ded429742acf4a
---

# Phase 7 Plan 9: Final Battery Acceptance Summary

Independently judged battery observations, truthful frontend claims and all six native packages pass at the unchanged archive bound, with full native/PowerPC/WASM and actual RetroArch evidence.

## Source and Authority

Final accepted product/test checkout is `54485d630c421716d51a8d7919ded429742acf4a`. Reviewed production source is `90b9b7a5f1f17f49f2133af17bb9e1e408fa21f1`; the only later non-planning difference is the four-line acceptance-test portability correction described below. Both final workflows completed successfully at the exact checkout:

- `gh run view 40104749040 --json headSha,status,conclusion,jobs`: exit 0, CI success after the single-job same-head macOS retry, including advisory WASM. https://github.com/szTheory/muhtari/actions/runs/40104749040
- `gh run view 40104750452 --json headSha,status,conclusion,jobs`: exit 0, Nightly success. https://github.com/szTheory/muhtari/actions/runs/40104750452

The genuine independently built event parent is `6784e4758ddb8e96a6048c986ff0e6b5e4248c01`. It contains the same reviewed production source. All six lane reports identify exactly this parent and the final checkout/run URL. Downloaded raw base policies are byte-exact with unchanged `quality.json`; actual three-ZIP sums equal their authenticated current metrics. Source, compiler, platform, configuration, fixture, policy and independent-parent authority remain unchanged.

Performance dispatch retains its existing same-head authority: `3837653,3837653,3837653 Ir/frame`, spread zero. It does not invent a comparative PR baseline; the unchanged join reports comparative Ir unavailable when not requested. Required/advisory status is unchanged. Compatibility retains its ledger judge, pins and allow file.

## Post-review Acceptance Refresh

The clean re-review at `12aca2a` and disposition at `6784e47` resolve all three findings, with zero open. Production fixes are `f8cc10f` (native broken-output-pipe final saving), `bd7e686` (effective frontend button device) and `90b9b7a` (core-matching Auto fallback). Full final-source CI executes their real closed-pipe/raw-media and loaded-module held-A/112-header-combination regressions. Both allowed review-fix rounds are used; the one gap round remains unused. This refresh does not create another review-fix round or alter the old finding outcomes.

The first reviewed-source Nightly [40103107489](https://github.com/szTheory/muhtari/actions/runs/40103107489) failed only the full PPC `runner.save.controls`: the new frame-heavy child wait used 500 ten-millisecond polls regardless of the already configured `MUHTARI_TIMEOUT_SCALE=13`. Two during-output children were killed after the unscaled wait, then lacked final saves; the test took 13.92 seconds. This was actual new portability evidence, not a passing result or proof of wrong saved bytes. Its accompanying CI [40103105894](https://github.com/szTheory/muhtari/actions/runs/40103105894) was cancelled when superseded.

Acceptance correction `54485d6` changes only `tests/runner/test_control.c` and the minimal existing CMake plumbing in `tests/runner/tests.cmake`: the affected child wait consumes the existing timeout scale. Native scale 1, PPC scale 13, outer CTest deadlines, other waits, workloads, exit assertions and every raw-byte assertion remain unchanged. No product semantics, new bound/grant, dependency, skip, replan or extra review/gap round was introduced.

Targeted actual QEMU command in an isolated Ubuntu 26.04 build, exit 0, URL N/A-local:

```sh
/usr/bin/time -f 'qemu control elapsed=%e exit=%x' qemu-ppc -L /usr/powerpc-linux-gnu /work/build/tests/muhtari-test-control /work/save /src/tests/diag/battery.a78 qemu-ppc -L /usr/powerpc-linux-gnu /work/build/runner/muhtari-run
```

The build uses the existing PPC toolchain and `-DMUHTARI_TIMEOUT_SCALE=13`. The actual consumer passes all four default/inherited-ignore and final/during-output combinations, exit 1 for each runner and every one of 34816 saved bytes, in 4.92 seconds locally. One-off same-consumer wrappers deliberately hang the during-output child or exit with the wrong success status: expected consumer exit 1 occurs with bounded cleanup at 13.36 seconds and 0.23 seconds; the asserting Python probe exits 0. Initial isolated commands used the wrong runner build path and failed; only corrected `runner/muhtari-run` commands count as proof. These generated wrappers/logs/media remain ignored.

The corrected full Nightly now passes native/PPC 641+6 each, both release floors and exact identity/69-row equality. Its real PPC control consumer passes in 7.82 seconds; the downloaded complete files also match the fresh local output. Six 10000-case properties, full vectors258, fuzz10 including I2C and all retained benchmarks pass at unchanged status.

Final CI attempt 1 at the same immutable `54485d6` had two macOS x64 policy timeouts: publish130.38 seconds and quality30.18 seconds; the other639 normal tests, including the new control consumer, passed. No source or timeout changed. GitHub refused a job retry while the containing run was active; after both Windows parent workflows completed, `gh run rerun 40104749040 --job 120904022265` exited 0. Attempt2 reran that job and dependents, carrying prior successful jobs forward. Both current/base full inventories now pass; the retry's parent publish/quality controls pass at78.52/23.51 seconds. The authenticated join, six-native69 equality, actual two-case RetroArch and WASM580/byte comparison all pass. The failed attempt remains visible.

Parent's separate local reviewed-source attempt with caffeinate `-i` exited8 after broad unrelated CTest timeouts at about998.76 seconds. Its bounded retry, `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" run-with-timeout 60 -- caffeinate -is cmake --workflow --preset ci`, exited0 with exact inventories641/6. After the acceptance correction, that same bounded command and the bounded hygiene workflow again exit0 with exact inventories641/6/24. The original failure is not counted as successful evidence.

### Final actual archives after review and correction

Run URL for every row: https://github.com/szTheory/muhtari/actions/runs/40104749040 . Download/enumeration/identity/raw-policy commands exit0. All sums fit the unchanged399937 ceiling.

| Native lane | Library | Runner | Libretro | Current sum | Independently built parent sum |
|---|---:|---:|---:|---:|---:|
| Linux arm64 | 87845 | 104036 | 92664 | 284545 | 284545 |
| Linux x64 | 85802 | 101953 | 90069 | 277824 | 277824 |
| macOS arm64 | 72498 | 84370 | 73635 | 230503 | 230504 |
| macOS x64 | 74095 | 84572 | 74004 | 232671 | 232674 |
| Windows arm64 | 87121 | 153813 | 130136 | 371070 | 371131 |
| Windows x64 | 88060 | 166307 | 139585 | 393952 | 393994 |

The agent downloaded and viewed the new DMA and both battery screenshots from final [RetroArch job120911136203](https://github.com/szTheory/muhtari/actions/runs/40104749040/job/120911136203). Hardware counters are1/1 then2/2 and marker5A thenFF; every complete34816-byte frontend/runner snapshot passes independent expectations. Strict frame comparisons, battery oracle faults, flipped DMA pixel7,3 and unsafe-path/private-launch isolation controls pass. Exactly two logical hosted cases and300 physical support lines remain.

### Closure of earlier pending Windows proof

The historical `07-06-SUMMARY.md` coverage row `windows-native-controls` intentionally recorded unknown runtime status and assigned execution to this plan. It is now fulfilled by actual final-source Windows [x64](https://github.com/szTheory/muhtari/actions/runs/40104749040/job/120904022224) and [arm64](https://github.com/szTheory/muhtari/actions/runs/40104749040/job/120904022187) current/base641+6 workflows: real console BREAK/C, stop/cadence, during-write synchronization, replacement/failure and audio-stop consumers all pass. Full native/PPC evidence also closes its emulator-prefix execution cross-reference. The original historical row and serialized assumption/prohibition flags are preserved, not rewritten into invented earlier proof. Agent review disposition is clean; no owner UAT is requested.

Earlier [CI40095781311](https://github.com/szTheory/muhtari/actions/runs/40095781311) and [Nightly40095782965](https://github.com/szTheory/muhtari/actions/runs/40095782965) cover only pre-review `fae8e8f`. Their measurements and experiments below remain historical; they do not validate the reviewed fixes or this correction.

## Completed Tasks and Commits

| Task | Result | Commits |
|---|---|---|
| 1 | Validate complete first/second battery observations, old row preservation and real malformed/duplicate/missing controls | `8db47aa`, `a15ab5e` |
| 2 | Enable proved battery/optional-firmware metadata and reconcile public/preparation prose | `9789903` |
| 3 | Actual six-native/current/base, complete BE, nightly, WASM and frontend acceptance with size and portability fixes | `c03e212`, `a67bf64`, `061261b`, `88836c7`, `b404c03`, `a126aaa`, `400babb`, `788798a`, `4db85f1`, `8d00518`, `fae8e8f` |

The original execution measured14 commits and48016 realized diff characters through fae8e8f. The refreshed persisted ledger now measures28 commits through54485d6, including the intervening review/fix/report work; the realized source-span diff is126161 characters, rounded up after division by four to31541 tokens. The recorded before/after hashes make that measurement reproducible. This refreshed report and subsequent tracking commits are outside that measured span.

## Observations and Exact Inventories

The previous accepted Phase 6 output at `2fe713c1074a63bbf89384d56200bd48060c0549` supplies the independent 47-row comparison. All old identities, ordering, digest and numeric observations match exactly. Only five designated `audio_behaviour_revision` annotations advance 3 to 4. The 22 appended `battery.first.*`/`battery.second.*` rows include frame, raw audio/PCM, complete mailbox and saved-media SHA-256 after the existing independent numeric and full-media driver validates them. Both fresh runner processes share only their private save directory.

The real producer rejects malformed, missing, duplicate, wrong-revision and invalid-numeric reports and invalid media. The comparison rejects equal-but-incomplete or duplicate old/new files. Existing policy equality fixtures now carry the complete required schema instead of a one-row synthetic pass. No extra test registration or inventory removal was needed: native remains 641 normal plus 6 release; portable WASM is 580. Six native hash files, full native/PowerPC files and WASM are all byte-identical at 69 rows.

`quality.json`, `pins.txt`, `quality_join.cmake`, `lane_equality.cmake`, platform-floor and performance predicates, performance/compatibility allows and the compatibility ledger are unchanged. Existing sample/frame/audio goldens are preserved except the previously authorized behavior metadata transition.

## Pre-review Three-Component ZIP Measurements

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

Early plan 07-02 measured local macOS arm64 218325 bytes. Pre-review macOS arm64 is230440; final reviewed/corrected macOS arm64 is230503. That early local result never established a worst-platform bound. The final Windows measurements, including intermediate failures, establish the actual feasibility result without a grant or rebaseline.

## Verification Commands

Historical original-execution commands at pre-review fae8e8f, run URL `N/A-local`, all exit0; refreshed local commands are recorded above:

- `cmake --workflow --preset ci`: 641 normal plus 6 release checks pass.
- `cmake --build --preset ci --target package` and `ctest --preset ci-release --output-on-failure`: actual packages and all 6 release checks pass.
- `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake`: exactly 641 once each. The corresponding `junit-release.xml` check with `-DLABELS=release` reports exactly 6.
- `cmake --workflow --preset hygiene`, its exact JUnit inventory and `scripts/hygiene.sh --tree`: 24 hygiene checks, complete tree and analyzer pass.
- `cmake -DROOT=<absolute-project-root> -P tests/cmake/budgets.cmake`: 19 files within unchanged bounds. `git diff --check` passes.
- Independent accepted-47/current-69 Python comparison and downloaded full native/PowerPC `cmp` checks pass. All created/modified paths and all 14 commits exist and are reachable.

Pre-review hosted CI command families at fae8e8f, exit0: `cmake --workflow --preset ci` or `ci-msvc` on all six targets and their independently built parent; exact normal/release inventories; ASan current/base 626 each; no-floating-point lanes; library-only; retained compatibility; six-native equality; authenticated quality join; real RetroArch; WASM. Source/run: https://github.com/szTheory/muhtari/actions/runs/40095781311 . WASM executes 580 once each and its complete output compares equal to Linux x64, preserving its advisory status.

Pre-review Nightly command families at fae8e8f, exit0: full native and PowerPC `ci` inventories 641+6 each and 69-row equality; full-vector workflow and exact inventory 258; six seeded property tests with `MUHTARI_PROP_CASES=10000`; fuzz workflow and exact inventory 10, including actual loader/run/I2C fuzz runs and regression controls; all retained native/WASM benchmark jobs. Source/run: https://github.com/szTheory/muhtari/actions/runs/40095782965 . The full PPC package floor passes, not merely a cross-compile probe.

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

Original planned execution/feasibility work consumed no review-fix or gap round. Subsequent formal review used both allowed fix rounds and is clean with all three findings fixed. The narrowly authorized acceptance-test portability correction and same-head job retry do not add a third review-fix or consume/reset the gap allowance. Phase verification/completion remain with the orchestrator. No additional GSD step, PR, merge or ship was invoked; no phase-complete claim is made here.

## Self-Check: PASSED

All28 changed tracked paths through54485d6 exist and all28 measured commits are reachable. Final CI and Nightly identify exact54485d6; all six current/base archives fit and raw policies, source/parent identities, complete inventories and required pass/fault controls pass. Final reviewed production source90b9b7a differs afterward only in the documented acceptance-test plumbing. No unexpected deletion, committed log/runtime media, untracked generated file, unresolved implementation stub or unrun plan verification remains. Plan07-09 acceptance is refreshed; review is clean and formal phase verification/completion remain pending.
