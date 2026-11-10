---
phase: 07-battery-saves
plan: "08"
status: complete
subsystem: diagnostics
tags: [battery, fixtures, runner, libretro, retroarch, evidence]
requires:
  - phase: 07-battery-saves
    provides: Hardware battery devices and tested runner and loaded frontend storage from plans 01 through 07
provides:
  - Reproducible owned battery diagnostic admitted through actual producer and unchanged join
  - Independent hardware counters and complete-media verdicts across fresh library runner and module processes
  - Actual isolated two-launch RetroArch proof with independent glyph and complete pixel controls
affects: [07-09]
tech-stack:
  added: []
  patterns:
    - Reuse the numeric diagnostic owner for independent cross-host media judgments
    - Share only explicit save storage between disposable frontend launch trees
key-files:
  created:
    - tests/diag/battery.s
    - tests/diag/battery.cfg
    - tests/diag/battery.a78
  modified:
    - tests/roms/manifest.txt
    - tests/policy/quality.cmake
    - tests/policy/tests.cmake
    - tests/diag/driver.c
    - tests/diag/expected.csv
    - tests/diag/tests.cmake
    - tests/retroarch/compare.c
    - tests/retroarch/run_retroarch.cmake
    - tests/retroarch/test.cfg.in
    - .github/workflows/ci.yml
    - docs/testing.md
key-decisions:
  - Keep both counters hardware-driven and judge every raw byte independently before comparing hosts.
  - Reuse a controlled base observation only when its independently resolved full event-base SHA matches exactly; always run the current producer and unchanged join.
  - Preserve all numeric quality predicates and archive ceilings while measuring Windows control timeout independently.
requirements-completed: []
requirements-contributed: [SAVE-01, SAVE-02, SAVE-03, SAVE-04, SAVE-05, SAVE-06]
coverage:
  - id: admitted-owned-fixture
    description: Authentic owned source pin and signed reproducible bytes pass actual scoped producer admission and unchanged trusted join
    requirement: SAVE-06
    verification:
      - kind: integration
        ref: "policy.quality.controls and diag.rebuild; actual full-manifest addition and later trusted base with changed duplicate license and digest faults; exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: independent-hsc-and-serial-observations
    description: CPU SRAM accesses and real RIOT serial transactions increment separate counters and report completed readback
    requirement: SAVE-01
    verification:
      - kind: integration
        ref: "diag.battery; library and loaded module hardware mailbox records and complete 34816-byte independent media expectations for counts 1 and 2; exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: serial-programming-and-bounds
    description: EEPROM counter uses open-drain serial writes bounded busy polling and executed readback with hostile timeout rejection
    requirement: SAVE-02
    verification:
      - kind: integration
        ref: "diag.battery and its actual 600-frame timeout control; independent EEPROM bytes 26624 and 26625; exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: complete-imported-module-media
    description: Final published SAVE_RAM imports prior actual media before execution and exports only the independently expected raw span
    requirement: SAVE-03
    verification:
      - kind: integration
        ref: "diag.battery separate loaded-module launches; no file then prior snapshot, counters 1/1 then 2/2, every untouched byte FF; exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: fresh-runner-reload
    description: Separate runner processes restore the previous exit snapshot and match exact hardware observations and media
    requirement: SAVE-04
    verification:
      - kind: integration
        ref: "diag.battery and runner.save roundtrip import faults options cadence controls nonregular; exact length and missing short oversized stale swapped or single-medium faults; exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: runner-exit-persistence
    description: Actual runner exits create first and second immutable raw save snapshots without host seeding
    requirement: SAVE-05
    verification:
      - kind: integration
        ref: "diag.battery fresh processes and existing runner.save.faults; all-byte oracle plus foreign exclusive-create collision preservation; exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: real-fresh-frontend-roundtrip
    description: Fresh private RetroArch processes preserve both devices through only shared external save storage with supplied then stub firmware
    requirement: SAVE-06
    verification:
      - kind: e2e
        ref: "gh run view 40080473918 --job 120837711272 --log; job success, counts 1/1 and marker 90 then 2/2 and marker 255, both complete 34816-byte snapshots independently judged and compared with fresh runner; exit 0; https://github.com/szTheory/muhtari/actions/runs/40080473918/job/120837711272"
        status: pass
    human_judgment: false
  - id: independent-hosted-pixels-and-isolation
    description: Both renderers satisfy independent glyph expectations and whole-frame equality while equal black wrong-count and one-device-stale controls fail
    requirement: SAVE-06
    verification:
      - kind: automated_ui
        ref: "Actual retroarch job 120837711272 plus agent inspection of both downloaded launch screenshots; private directories and unsafe-overlap controls, unchanged DMA digest and flipped pixel 7,3, exactly two logical cases and 300 physical lines; exit 0; https://github.com/szTheory/muhtari/actions/runs/40080473918"
        status: pass
    human_judgment: false
  - id: native-portability-and-local-inventories
    description: All six native current and independently selected base inventories run without weakening release floors or existing tests
    requirement: SAVE-06
    verification:
      - kind: integration
        ref: "CI40080473918 native jobs current and base each 641 plus 6, both Linux architecture firmware and GLIBC2.17 floor tests, ASan and six-native hashes pass; gh run view exit 0; https://github.com/szTheory/muhtari/actions/runs/40080473918"
        status: pass
      - kind: integration
        ref: "Final cmake --workflow --preset ci exit 0, 641 plus 6; exact inventory checks exit 0; final seven runner save tests and four existing consumers with WASI PID branch selected exit 0; URL N/A-local"
        status: pass
    human_judgment: false
actuals:
  tokens: 17355
  tasks: 3
  commits: 15
plan_head_before: b5518f69b3c75d1db64fca21f65abfca108d740a
plan_head_after: af64beebd4b5d0d1eaec50fcf729995aedf09159
duration: 304min
started: 2026-10-24T04:48:30Z
completed: 2026-10-24
---

# Phase 7 Plan 8: Independent battery persistence evidence Summary

**Owned hardware counters survive fresh runner and RetroArch launches, with independently judged complete raw media and pixels.**

## Accomplishments

- Landed MIT source/config at authentic commit `4642ded6d026f456acf798a43efb081b76987018` before assembling the admitted fixture. SHA256 is `10f5bc279d8147b3348e102ca9f7e4dc496dd31b5099adb2ef64320e987fb4d4`. The pinned existing WASM assembler/header/sign recipe rebuilds identical bytes. Exactly one manifest row was added; removing that row produces the byte-identical prior manifest. All other fixture bytes and pins are unchanged.
- Proved the actual row through actual candidate/base producer recursion and the unchanged authenticated join before accepting its hardware behavior. Retained synthetic policy faults, actual changed/duplicate/license/digest faults, raw trusted identity and later-base behavior. Exact-base observation reuse is restricted to this controlled test owner; every current producer and join still executes and different bases still recurse.
- The CPU increments HSC SRAM at $1100/$1101 and independently writes, polls and reads the serial counter at $6000/$6001 through RIOT ORA/DDRA. Erased FF becomes zero before increment; no host seeds either counter. Busy polling is finite. Six CRC-protected mailbox records report C6/FE signatures, separate observed counters, completion and the installed ROM marker.
- `diag.battery` launches separate library, actual runner and loaded-module processes. Launch1 produces1/1 and launch2 restores actual prior media to produce2/2. Every snapshot has exactly34816 bytes: independent HSC offsets256/257, serial offsets26624/26625 and FF everywhere else. Missing/short/oversized, stale/wrong count, swapped offsets, unchanged single media, malformed/CRC/stale mailbox and actual timeout controls fail through the consumers.
- Hosted coverage has exactly DMA equivalence and one save roundtrip containing two launches. Each launch has private home/config/system/output trees; only the explicit external save directory persists. Unsafe overlap and firmware-inside-disposable-tree refuse before cleanup. The first launch copies an owned synthetic4096-byte C6/FE image with marker5A; the second has no firmware and reads stubFF. Separate fresh runner processes use the same choices. Neither firmware routines nor retail compatibility is claimed.
- Independent 3x5 glyph predicates judge HSC, EEPROM and marker fields in each renderer before whole-image equality. Equal black, wrong expected counter and only-EEPROM-stale controls fail; retained DMA flipped7,3 detection and original digest remain. All four hosted files total exactly300 physical lines. Pinned tools, two-case membership and existing50-second launch timeout are unchanged; actual hosted launches completed in roughly2.4 seconds each. Actual launch screenshots were downloaded and inspected by the agent.

## Task Commits

1. Task1: `4642ded`, `eab80ac` — owned source then authentic fixture admission and actual evidence controls.
2. Task2: `c987bfa` — independent counters, raw media and fresh-process faults.
3. Task3: `6f52ad9` — private hosted save roundtrip and independent pixel verdicts; `32e33d6`, `8903a70`, `7bf2212`, `5a4c599`, `f88337e`, `82adf44`, `fdb0fe3`, `d62c61c`, `297e60d`, `7f399ad`, `af64bee` — required portability diagnostics, bounded producer reuse, measured test timeout and precise embedding output.

The persisted ledger measures15 task commits. The realized binary-aware diff is69417 characters, giving17355 rounded-up chars/4 tokens across23 files. Duration includes actual hosted diagnostics and reruns. No generated evidence, raw save, firmware, screenshot or log is committed.

## Commands, Exits and Run URLs

Local URLs are N/A-local. Hosted evidence source is `7f399ade00e97ea2b08bcd5bea7fa7c094d49e7f` with independently resolved base `297e60d2b954fb63df27a747756652b7a436a4e9`. The final `af64bee` changes only WASI PID selection, its existing fault consumer and measured Windows control timeout; final portability acceptance remains plan09.

| Command | Exit | Evidence |
|---|---:|---|
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(policy[.]quality[.]controls|diag[.]rebuild)' --no-tests=error --output-on-failure` | 0 | Actual admission and reproducible bytes before fixture-dependent acceptance |
| `ctest --preset ci -R '^(diag[.]battery|runner[.]save[.]|libretro[.]host[.]battery)' --no-tests=error --output-on-failure` | 0 | Independent hardware and complete media through actual consumers |
| `cmake --workflow --preset ci` | 0 | Final641+6;35.47/0.10 seconds |
| `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` | 0 | Exactly641 once each |
| `cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake` | 0 | Exactly6 once each |
| `cmake --build --preset ci && ctest --preset ci -R '^runner[.]save[.]' --no-tests=error --output-on-failure` | 0 | Final test-source edit compiled;7/7 existing save consumers |
| Existing save test compiler command with `-D__wasi__=1`, then `battery-wasi-save DIR import`, `faults`, `options`, `cadence` | 0 each | Same actual runner consumer exercises no-PID path, including all32 exclusive collisions and previous-file retention; native simulation is not WASM runtime proof |
| `cmake --workflow --preset hygiene` | 8 | Final23 checks pass; tree scan alone timed out at60s without findings; unchanged focused rerun also timed out |
| `sh -x scripts/hygiene.sh --tree` | 0 | Standalone complete scan progressed through every file under local load; trace stays ignored; no timeout grant |
| `git diff --check`; old manifest byte comparison; protected policy/pin diff | 0 each | No findings or old changes |
| `wc -l tests/retroarch/*` | 0 | 76+129+66+29=300 |
| `gh run view 40080473918 --json conclusion,jobs` | 0 | All six native current/base641+6, hygiene, ASan, BE compile, nofp, perf and equality succeed; overall CI correctly fails quality and pre-fix WASM |
| `gh run view 40080473918 --job 120837711272 --log` | 0 | Actual RetroArch job success: independent first/second counters, complete raw verdicts and comparison, both pixel verdicts/controls and retained DMA |
| `gh run download 40080473918 -n retroarch-frames -D build/battery-hosted-frames` | 0 | Both actual launch shots inspected; six owned diagnostic PNGs kept only under ignored build output |
| `gh run download 40080473918 -n quality-windows-x64 -D build/battery-hosted-windows` | 0 | Authenticated archive407930 exceeds unchanged399937; independent base407981 also exceeds it |

Hosted run: https://github.com/szTheory/muhtari/actions/runs/40080473918 . The successful RetroArch job is https://github.com/szTheory/muhtari/actions/runs/40080473918/job/120837711272 . Original DMA frame120 SHA256 remains `145799ed1868d43e10f18fdcd1529120274fe5c142957962760c02da6cb67955`.

## TDD Gate Compliance

These tasks add a diagnostic fixture, test consumers and hosted test wiring, rather than a new product subsystem. They use the existing test infrastructure and actual positive/fault consumers. No synthetic product RED was invented. The required incidental compatibility changes retain existing runtime/fault tests, and their actual compiler/runtime failures supplied diagnostic evidence. Global MVP/TDD mode is false. Test inventories include the recurring new diagnostic and portable pixel consumer.

## Deviations from Plan

- Kept cross-host numeric/media orchestration in the existing diagnostic owner instead of duplicating it in runner and module host files. Portable generated BMP output reuses the host palette to exercise the same pixel judge locally; expected glyphs remain independently specified. Hosted-only orchestration remains within the four-file300-line scope.
- Native execution exposed required portability faults: MSVC CRT warnings in the portable compare target, GCC seed initialization, Windows import CRT deprecation, and missing XSI declarations in the BIOS fault harness. Added only the needed definitions/zero initialization and existing-descriptor import route. Existing failure seams and regular-file refusal remain exercised.
- On current glibc, public `stat` binds GLIBC2.33 and violates the existing module2.17 floor. The scoped glibc x86_64/aarch64 owner uses the existing binary `__xstat` ABI with layout versions1/0 respectively; normal `stat` remains elsewhere and the test-defined fault seam is retained. Primary contract: https://refspecs.linuxfoundation.org/LSB_4.0.0/LSB-Core-generic/LSB-Core-generic/baselib---xstat.html . Both actual Linux architecture firmware consumers and release.floor pass on current and independent base in the hosted run. No floor was relaxed.
- CMake text reports became CRLF on Windows; canonical UNIX newline output preserves the strict524-byte mailbox parser rather than weakening it. Existing embedding build-and-test sometimes omitted child output on both current and base; a script wrapper now reports the actual child exit/output. The original success regex and complete inventory remain unchanged; both Windows architectures pass current/base.
- Expanded actual-manifest controls exceeded the original Windows30-second limit. Removed redundant producer runs by reusing existing pass scenarios and exact independently resolved base observations, preserving every current producer, unchanged join and fault. A provisional54s followed the prior26.92s comparable control in run40031550434. Actual optimized successful maximum40.12s in run40080473918 now sets Windows81s by ceil(2x); ARM current34.25s/base32.00s and x64 current29.28s/base40.12s are measured. Other platforms retain30s. This is a test-timeout calibration, not an archive or performance-predicate grant.
- WASM compilation exposed unavailable/deprecated `getpid`. WASI now uses PID field0 in candidate names; bounded32-attempt exclusive creation remains the ownership authority, with no process-emulation dependency. Its existing fault consumer selects the same value. Native simulation of that branch passes; actual final WASM runtime belongs09.
- Diagnostic CI attempts40074461718,40074652418,40075081757,40077324209 and40077753682 supplied the above specific compile, floor, text, timeout and embedding failures; superseded attempts were canceled or rerun without bypasses. The latest actual six-native and hosted proof is40080473918. Final local hygiene tree-only timeouts under load were followed by a complete unchanged standalone scan exit0; hosted hygiene passes and no timeout was changed.

## Ownership, Bounds and Remaining Acceptance

ENGINEERING section11 review keeps assembly as observation producer, the existing diagnostic driver as numeric/media judge, the existing comparator as pixel judge and the existing launcher as disposable-directory owner. No dependency, new generic framework, production protocol, storage mirror or resource-capacity increase was introduced. The small portable BMP writer enables recurring local fault checks without moving hosted-only orchestration beyond its cap. Optional user firmware stays an external input, validated before destructive launch-tree cleanup. No new unreviewed trust boundary was introduced.

All old expected hashes, numeric quality policy, trusted digest authority, release floors, archive399937 ceiling and pending capability guards remain unchanged. There is no skipped test or unknown implementation stub. The intentional signature-only firmware stub remains documented. No owner UAT or hardware run is requested, and no review-fix or gap allowance was consumed.

Plan09 must resolve actual Windows archive407930>399937 and collect final source native/WASM/full-BE/nightly/10000-case/hosted/quality acceptance before requirements or capability claims graduate. Overall hosted CI is explicitly not green yet. The latest WASI compile issue is fixed and locally exercised, but final actual WASM proof is pending. Ten assumption flags retain their unresolved metadata until the phase evidence review. These are recorded final-acceptance work in the existing next plan, not grants or whole-phase completion. All six SAVE requirements remain unchecked.

## Self-Check: PASSED

All three created fixture/source files and the summary exist; all15 task commits are reachable. Final CI641+6 and exact inventories pass, existing save consumers pass, complete tree scan exits0, all other23 hygiene checks pass, and normal commit/push hooks pass. Actual hosted screenshots and raw/pixel verdicts were inspected. Protected bytes/pins/policy remain unchanged and no generated untracked file or unexpected deletion remains. The next plan is07-09; the phase remains executing.
