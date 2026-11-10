---
phase: 10-homebrew-expansion-memory
plan: "08"
subsystem: testing
tags: [C, XBoard, XM, HSC, POKEY, diagnostics, libretro]
requires:
  - phase: 10-04
    provides: Installed expansion RAM/HSC/POKEY owners and continuous chip-select model
  - phase: 10-07
    provides: Source-pinned fixture admission and actual three-host numeric/native consumers
provides:
  - Immutable owned expansion fixture with nineteen measured records
  - Exact authenticated fixture admission and producer/join fault controls
  - Sixty-six bounded actual-host RAM/storage/audio groups with independent expectations
  - Named unsupported-board runner refusal
affects: [semantic-equality, final-platform-acceptance]
tech-stack:
  added: []
  patterns:
    - Startup-selected measuring groups within the existing mailbox bound
    - Physical storage restoration before execution through actual host interfaces
key-files:
  created:
    - tests/diag/expansion.s
    - tests/diag/expansion.cfg
    - tests/diag/expansion.a78
    - docs/expansion-tests.md
  modified:
    - tests/diag/driver.c
    - tests/diag/expected.csv
    - tests/diag/tests.cmake
    - tests/roms/manifest.txt
    - tests/cmake/quality.cmake
    - tests/policy/quality.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - runner/main.c
    - docs/quality.md
    - docs/emulation-tests.md
key-decisions:
  - Preserve the failed original source/fixture history and use a new immutable pin for explicit MARIA write mode and PAL-safe two-line zones
  - Judge literal CPU records, native VBE/HALT pages, storage lifetimes and source/IRQ observations before host output equality
  - Retain twelve calls per startup group and the existing mailbox, timeout scaling and inventory mechanisms
requirements-completed: []
coverage:
  - id: D1
    description: Legal source-pinned owned expansion fixture rebuilds exactly without changing old manifest rows
    requirement: BOARD-05
    verification:
      - kind: integration
        ref: diag.rebuild and diag.rebuild.self_test
        status: pass
      - kind: other
        ref: Exact source/config/digest and sole-manifest-addition assertion against 7d387d7
        status: pass
    human_judgment: false
  - id: D2
    description: Real recursive producer and authenticated join admit only the exact new fixture identity
    requirement: BOARD-05
    verification:
      - kind: integration
        ref: policy.quality.controls expansion current/later-base admission and identity faults
        status: pass
    human_judgment: false
  - id: D3
    description: All eight XBoard banks and sixteen XM pages in both windows have actual CPU and VBE/HALT DMA observations through three deliverables
    requirement: BOARD-05
    verification:
      - kind: e2e
        ref: diag.expansion nineteen-record and native-row judges across sixty-six groups
        status: pass
    human_judgment: false
  - id: D4
    description: Initially hidden HSC restore and physical storage identity survive gates, console Reset and power cycle
    requirement: BOARD-05
    verification:
      - kind: e2e
        ref: diag.expansion public battery API, runner save import/export and module SAVE_RAM lifecycle checks
        status: pass
    human_judgment: false
  - id: D5
    description: Installed single and dual POKEY tone/timer/IRQ continuity survives decode disable and ignored stores in both regions
    requirement: BOARD-05
    verification:
      - kind: integration
        ref: diag.expansion matched enabled/disabled/ignored-INIT groups and raw source/PCM/count checks
        status: pass
      - kind: unit
        ref: pokey.logic, pokey.clock and pokey.maps
        status: pass
    human_judgment: false
  - id: D6
    description: Protocol and semantic faults fail independently of hashes and unsupported sound/board owners are named refusals
    requirement: BOARD-05
    verification:
      - kind: e2e
        ref: diag.expansion malformed and CRC-valid reports, wrong-page and INIT-reset executables, twelve negative loads
        status: pass
    human_judgment: false
  - id: D7
    description: Existing native, release, inventory and authenticated current/base package contracts remain intact
    requirement: BOARD-05
    verification:
      - kind: integration
        ref: Unchanged task08-jobserver-gate.sh at af8a991, 661 ordinary plus six release checks
        status: pass
      - kind: other
        ref: Fresh quality_evidence BUILD_BASE producer at af8a991 against genuine pre-plan 7d387d7
        status: pass
    human_judgment: false
actuals:
  tokens: 25159
  tasks: 3
  commits: 8
plan_head_before: 7d387d7c6ad2ab59c99970e30f4257a041f410d7
plan_head_after: af8a99170118e4b0f5438e0f9e562c8b88e2961e
duration: 38min
started: 2026-10-26T19:59:33Z
completed: 2026-10-26
status: complete
---

# Phase 10 Plan 08: Integrated expansion evidence Summary

**An immutable owned ROM measures XBoard/XM RAM, initially hidden HSC and continuous POKEY through the library, actual runner and loaded module, with independent numeric/native judgments before output equality.**

## Accomplishments

- The final source/config pin is 26357b8ef692de1d86a848992d6b679d9999f79b. The existing assembler/sign/header KEEP recipe produces the exact49280-byte canonical fixture, SHA256 b5164fe19df352231d2a6033d0349ff45019f2dcfb889f6d0d542879ae41ee9b. Its this-repo/MIT row is the sole addition relative to the genuine pre-plan manifest; all previous bytes remain identical. Synthetic firmware/save media is generated under build only.
- Nineteen actual CPU records remain within the existing27-record mailbox bound. XBoard measures all eight banks and sixteen control aliases; XM measures all sixteen physical pages in both CPU windows, disabled/revealed overlays, retained data and named absent controls. Literal expected records and runtime CRC/count/order/identity checks precede comparison.
- Forty bank/page/DLL-window groups cover every XBoard bank and XM page. Another26 bounded representative groups cover both regions, declared single/dual POKEY, undeclared installed primary and three matched sound-gate states:66 positive groups total. Each executes twelve calls through the three actual hosts. The separate sustained600-frame PAL workload is unchanged.
- Unequal CPU/MARIA pages and expansion-resident DLLs expose both non-HALT VBE and HALT ownership. Every display list explicitly initializes write mode0. Two-line zones cover PAL within256 DLL entries. OFFSET0 lines play on independently derived native rows3/5; the first uses the CPU DLL page, the second the HALTed MARIA DLL page. Exact160A pixel expansion checks all320 pixels on each selected row. CRC completion is required by call11, before the judged complete call12 frame. The runner's real PNG has its own native-palette/page judgment.
- The library imports physical HSC bytes before execution; the runner uses real expansion/HSC options and save-dir import/export; the module discovers generated firmware through its system-directory callback and restores actual SAVE_RAM before its first run. Hidden stores, visible same/changed values and later gates prove one generation increment and stable pointer/length/layout/attachment/firmware. Exact console Reset packets are asserted in library and module; subsequent power/reset clears volatile RAM/controls while retaining battery bytes/token/identity.
- Raw observations independently require primary source2 and optional secondary source6, DAC levels0/15 and0/9, ordered tick/source tuples, continuing timer IRQST FE, declared IRQ, legal topology and audible PCM. Counts use the documented rational regional clock lattice. Enabled/disabled/ignored-store controls match actual raw/PCM/count observations; all twelve native-frame hashes and output streams agree across hosts after judgment.
- The existing producer/join now authenticates precisely the final expansion source pin/digest. Current and later-base cases protect old rows, order, raw manifest/sample identities and measured package observations. Forged pins/digests, changed/missing/duplicate/unrelated rows and producer/run/configuration/base faults fail the real consumers.
- Missing/truncated/trailing reports, protocol/CRC/identity/count/order/flags/reserved-data faults and CRC-valid wrong numbers fail. Semantic controls cover wrong page, leaked hidden HSC and reset timer status. Actual wrong-page DMA fails both native and PNG consumers; an enabled INIT store fails the tone/timer consumer. YM2151, Covox, ADPCM, Souper and conflicting4000/0800 owners are refused in both profiles, with runner77 and named module logs.

## Task commits

| Task | Commit | Result |
|---|---|---|
| 1 | de267c7 | Original immutable owned source/config pin |
| 1 | 1661dae | Original canonical bytes and sole manifest addition |
| 2 | 8c1a8a5 | Exact real-row producer/join admission and controls |
| 3 RED | 7b29b6e | Executed nineteen-record diagnostic exposes missing identity21 consumer |
| 1 correction | 26357b8 | New source pin explicitly resets MARIA write mode and covers PAL DLL traversal |
| 1/2 correction | 5bd8cbc | Corrected canonical bytes and exact source/digest admission |
| 3 GREEN | 7a64219 | Integrated host/storage/audio/native judges, negative controls, docs and portable inventory |
| 3 calibration | af8a991 | Existing timeout rule applied to the complete measured workload |

The persisted ledger measures eight commits before this summary commit. Actuals are ceil(100636 realized diff characters /4)=25159 across fifteen task files, using the estimate's diff scale rather than a harness token count. Shared STATE/ROADMAP/REQUIREMENTS belong to the orchestrator and were not modified.

## TDD evidence

RED command: `ctest --preset ci -R '^diag[.]expansion$' --no-tests=error --output-on-failure --output-junit tests/diag/expansion-red.xml`, exit8, N/A-local. The actual owned ROM completed nineteen records; the old consumer expected identity0 and rejected identity21. The unchanged fresh JUnit report and evidence record remain ignored build outputs. `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/ci/tests/diag/expansion-red.json --raw` returned exit0, RED_EVIDENCE_OK. Semantic assessment: this is the planned missing numeric observation admission, not discovery, syntax, setup or a manufactured hardware failure. The existing production expansion hardware was retained.

GREEN is the implemented numeric/native/host consumer and the final passing `diag.expansion`. The later source correction preserves its failed original source and fixture history. No no-op production hardware implementation or fake RED is claimed.

## Verification evidence

All run URLs in this table are **N/A-local**. Logs, JUnit and JSON remain ignored build output; no execution log is committed.

| Command/source | Exit | Evidence |
|---|---:|---|
| Original existing assembly KEEP command, source de267c7 |0|Original canonical SHA a81acf7900419e30fd08a95ae7db0deedeed4cbe2d1eefe8fec3535fd2054e6f retained in failed history |
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^diag[.]rebuild' --no-tests=error --output-on-failure && scripts/hygiene.sh --tree` |0|Original legal/source-pinned bytes and automatic rebuild discovery |
| `ctest --preset ci -R '^policy[.]quality[.]controls$' --no-tests=error --output-on-failure` |0|Actual original-row producer/join current/later cases and faults,14.61s |
| Existing KEEP rebuild followed by `ctest --preset ci -R '^diag[.]rebuild\|^policy[.]quality[.]controls$' --no-tests=error --output-on-failure`, corrected26357b8 source |0|Final immutable source/digest; three checks pass; policy12.09s; subsequent tree hygiene0 |
| `ctest --preset ci -R '^diag[.]expansion$' --no-tests=error --output-on-failure` |0|Complete66 positive groups, protocol/semantic/identity controls and twelve negative actual loads;4.86s initial complete pass |
| `ctest --preset ci -R '^diag[.](expansion\|bankset\|bankset_tracer)$\|^(battery\|pokey\|audio\|libretro[.]expansion\|runner[.]expansion)\|^docs[.]budgets$' --no-tests=error --output-on-failure` |0|21 targeted checks and fixture dependencies; BankSet retained; audio spectral/samples and POKEY lifecycle pass; expansion5.86s |
| `cc -std=c17 -O3 -Wall -Wextra -Wpedantic -Werror -Iinclude -Isrc -Ihost tests/diag/driver.c tests/runner/png_read.c runner/sha256.c build/ci/libmuhtari.a build/ci/libmuhtari_host.a -o build/ci/task10-plan08-no-module-diag` |0|Final driver compiles/links without DIAG_LIBRETRO |
| `cmake -DDIAG_EXPANSION_RUN=ON -DROOT=. -DOUT=build/ci/task10-plan08-no-module-cases -DDRIVER=build/ci/task10-plan08-no-module-diag -DRUNNER=build/ci/runner/muhtari-run -DEXPECTED=tests/diag/expected.csv -DMODULE= -P tests/diag/tests.cmake` |0|Complete module-free matrix and controls through library and actual process |
| `sh build/ci/task08-jobserver-gate.sh` at7a64219 |0|661 ordinary checks in56.22s; six release checks, packages, repeated release checks, exact661-once inventory and hygiene; expansion6.42s |
| `ctest --preset ci -R '^diag[.]expansion$' --no-tests=error --output-on-failure`, final13s registration |0|66-group workload4.36s after calibration |
| `sh build/ci/task08-jobserver-gate.sh` at final af8a991 |0|661 ordinary checks in51.76s; six release checks in0.10s, repeated six in0.10s, three actual packages, exact661-once inventory and hygiene; expansion5.67s |
| `sh build/ci/task10-08-quality.sh` at final af8a991 against genuine7d387d7 |0|Fresh existing recursive producer; current661+6/base660+6, independent base ordinary64.55s/release0.14s; matching typed environment and raw trusted identities |
| Final source/config/digest/sole-manifest-addition and producer identity assertions |0|Exact26357b8 source/config, b5164f digest, original raw manifest preserved, correct current/base SHAs and measured sums |
| `clang-format --dry-run --Werror tests/diag/driver.c runner/main.c`, `git diff --check`, file/commit ancestry checks |0|Version21 formatter; all fifteen task paths and eight commits present; no unexpected deletions or untracked generated output |

The13s deadline is provisional ceil(2*6.42s) from the complete local required workload under DEC.47. Workload, timeout scale, membership, mailbox and existing600-frame PAL coverage are unchanged. Hosted calibration remains later acceptance work.

The quality wrapper uses the existing four-slot FIFO jobserver and invokes:

`GITHUB_EVENT_PATH="$PWD/build/ci/task10-08-event.json" GITHUB_EVENT_NAME=push cmake -DROOT=. -DBUILD=build/ci -DOUT=build/ci/task10-08-quality-current.json -DSHA="$(git rev-parse HEAD)" -DCONFIG=ci -DBUILD_BASE=ON -DSOURCE=local -P tests/cmake/quality_evidence.cmake`

The event's genuine before is7d387d7c6ad2ab59c99970e30f4257a041f410d7. Candidate source is af8a99170118e4b0f5438e0f9e562c8b88e2961e. Both envelopes preserve the same raw trusted manifest identity2152b5f4656a63d75dec20be792c0d0f75939fa0bd3285badbada17076187de9 and sample identity d549d4691aca7c004325102613fd6ac2337ecfe8a4c0cec004472e5bbcc80ca8 on AppleClang21/Darwin arm64. The clean disposable base worktree was removed after measurement; the original producer observations remain local.

| Actual macOS arm64 ZIP | Candidate bytes | Genuine pre-plan bytes |
|---|---:|---:|
| Library |83245|83213|
| Runner |98522|98477|
| Libretro |89625|89625|
| Three-component sum |271392|271315|

Archive bytes remain report-only under DEC.61. This is a local per-plan comparison, not phase-versus-main performance or hosted/all-platform acceptance.

## Failures and execution corrections retained

- Initial RED setup configure attempts exited1 for a missing required TIMEOUT and then unsorted inventory. They were corrected before the actual executed assertion and are not RED evidence.
- The initial physical-storage judge incorrectly treated an empty zero-status mailbox as completed, then assumed completion by call9. Direct execution failed1. Requiring identity21 and measuring actual completion by11 resolved those assumptions before stable display judgment.
- The original expansion matrix exited8 when XM page1 exposed a retained MARIA write mode: correct CPU records and palette were accompanied by blank native rows. Temporary driver instrumentation established WM1. Source26357b8 adds the explicit five-byte WM0 header and two-line zones; canonical/admission5bd8cbc follows that new pin. The old source/bytes remain reachable. Final native rows3/5 derive from OFFSET1/0 and the line-RAM pipeline; no old numeric/hash observations changed.
- The first runner matrix exited8 for malformed untagged version2 ProLine movie rows. They now use actual `proline:......` payloads. A separate initial module invocation used the wrong module directory and exited1; the target-file registration uses the real module.
- The complete positive matrix passed before its Souper negative test exited8: runner77 said only unsupported board. The minimal existing board-name suffix fixes that required named refusal, with no change to status77.
- A no-module CMake configure with the native ci inventory exited1 because that inventory requires libretro tests; no inventory was weakened. The established standalone compile/script path is used instead. Its first run exited1 because optional empty module arguments were omitted by CMake list forwarding; the driver now accepts both argument counts. The final full module-free matrix passes.
- An added exact module Reset assertion made `diag.expansion` exit8: the initial stimulus used the adapter/keypad L3 mapping instead of ordinary ProLine X. The correct actual X mapping now passes the retained exact assertion. Earlier failure state is also preserved through later mailbox judgment.
- Early smoke invocations used a nonexistent runner location and an unsupported report option; neither was counted as verification. Their individual statuses were not separately captured by the initial batched shell. The actual registrations use target-file paths and the supported diagnostic option.
- Initial unprivileged ledger/Git writing was denied by the workspace sandbox. Authorized escalation succeeded with hooks and the noreply identity; no hygiene bypass was used.

## Deviations from plan

1. **[Rule1 - Owned stimulus correctness]** The first immutable source had a real retained-write-mode error and insufficient one-line PAL DLL coverage. Re-pinned explicit WM0/two-line source and regenerated exact bytes/admission, preserving failed history. CPU record values did not change.
2. **[Rule2 - Required named refusal]** The new executable negative consumer demonstrated missing Souper naming in runner/main.c. Reused its existing board-name helper; runner77 and module named refusal now pass.
3. **[Rule3 - Documentation budget]** Detailed derivation lives in docs/expansion-tests.md through the existing linked-detail pattern. The overview remains40915/40960 bytes with its original cap.
4. **[Rule3 - Portable registration]** Added the exact native portable diagnostic to wasm32 inventory and supported an absent module argument in the shared driver. No new inventory, harness, dependency, framework, quota or required report was added.

These are execution corrections, not extra formal review/gap rounds. No new stub, skipped test or unrun plan verification remains. The existing signature-only HSC fallback and absent full-XM services remain intentional documented limits; this diagnostic supplies generated test firmware and does not implement firmware routines. No new production trust surface was introduced.

## Confidence and readiness

This proves the selected emulator compatibility model through actual instructions, DMA and three deliverables. XM RAM/POKEY/HSC and gate behavior retain medium physical confidence; the primary developer specification remains inaccessible. Unsupported controls remain absence probes, with no invented firmware/detection success. Cheap unit tests retain individual window gates and exact electrical-edge coverage alongside these executable groups.

API7/ABI1/behavior6 is unchanged. BOARD-05 is contributed to, not marked complete. Semantic release equality is plan10-09 work; full hosted/platform/frontend acceptance remains plan10-13 work. No third hosted RetroArch case or owner verification was added.

Within this execution step, plan10-09 is ready. Next separate allowed command: `$gsd-execute-phase 10`. No other GSD step was launched.

## Self-Check: PASSED

All fifteen task files and this summary exist; all eight recorded task commits are reachable ancestors. Final-source full workflow, module-free execution, immutable fixture identity and fresh genuine-base producer pass. The parent-owned state files remain untouched.
