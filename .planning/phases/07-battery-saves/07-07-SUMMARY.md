---
phase: 07-battery-saves
plan: "07"
status: complete
subsystem: libretro
tags: [battery, hsc, savekey, firmware, achievements]
requires:
  - phase: 07-battery-saves
    provides: Tested authoritative battery span and frozen load-time hardware from plans 02 through 06
provides:
  - Final SAVE_RAM publication before emulation with load-time provisioning and visible reload-only changes
  - Optional copied HSC firmware with honest stub and firmware identity warnings
  - Actual HSC SRAM and ROM descriptors checked with the pinned resolver on all retained boards
affects: [07-08, 07-09]
tech-stack:
  added: []
  patterns:
    - Keep persistence in the frontend and obtain all production storage through public core queries
    - Extend loaded-module and real resolver consumers with independent byte expectations
key-files:
  created: []
  modified:
    - libretro/libretro.c
    - tests/libretro/host.c
    - tests/libretro/rcheevos.c
    - tests/libretro/bios.c
    - tests/libretro/tests.cmake
    - tests/inventory/ci.txt
    - docs/libretro.md
    - .planning/preparation/HOSTS-AND-INTEGRATION.md
key-decisions:
  - Freeze the public battery span before publication without mirror storage or save-file operations in the adapter.
  - Keep optional HSC firmware alive across BIOS candidate retries and map only the copied installed ROM.
requirements-completed: []
requirements-contributed: [SAVE-01, SAVE-03, SAVE-06]
coverage:
  - id: final-frontend-storage
    description: All four storage lengths are published before the first frame and imported bytes reach executed HSC and serial hardware
    requirement: SAVE-03
    verification:
      - kind: integration
        ref: "libretro.host.battery and libretro.host.battery_order; actual loaded module HSC read and increment plus either EEPROM identity and port random-read waveform; exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: attachment-order-and-retention
    description: Metadata options and controller callbacks determine load topology while reset unload no-content and late requests retain their contracts
    requirement: SAVE-06
    verification:
      - kind: integration
        ref: "libretro.host.battery_order plus existing devices reset options xrgb8888 and rgb565 cases; no-setter default combined layout explicit suppression option precedence dual refusal and once-per-changed reload notice; exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: optional-installed-firmware
    description: Absent firmware visibly selects the stub while present invalid files refuse and copied exact firmware survives source deletion and BIOS retries
    requirement: SAVE-01
    verification:
      - kind: integration
        ref: "libretro.host.hsc_firmware and libretro.bios_failures; all-byte installed ROM checks unknown and missing-signature warnings exact sizes read open close nonregular allocation and path faults; exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: actual-hsc-achievement-bytes
    description: Real resolver reads every intended HSC byte and SRAM aliases SAVE_RAM with correct flags while absent and serial ranges remain holes
    requirement: SAVE-06
    verification:
      - kind: integration
        ref: "libretro.rcheevos and libretro.host.hsc_firmware; retained linear SuperGame Activision Absolute and Fractalus boards with stub and supplied ROM plus combined EEPROM; unchanged 64-descriptor and 32-region limits; exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: documented-local-acceptance
    description: Docs describe implemented precedence ownership and limits while exact native inventories retain all previous pins and capability boundaries
    requirement: SAVE-06
    verification:
      - kind: other
        ref: "Agent read docs against adapter public API and loaded host resolver consumers under ENGINEERING section 11; docs checks exit 0; URL N/A-local"
        status: pass
      - kind: integration
        ref: "cmake --workflow --preset ci; 640 non-release plus 6 release checks exact inventories and tree hygiene; three ZIPs 228469 <=399937; exit 0; URL N/A-local"
        status: pass
    human_judgment: false
actuals:
  tokens: 14689
  tasks: 3
  commits: 5
plan_head_before: c7a675996f241b3621d5db77a64827a12ddf4fa1
plan_head_after: 59bf9d06e9213fd69a06f26a61a9d50e040c0484
duration: 30min
started: 2026-10-24T03:42:35Z
completed: 2026-10-24
---

# Phase 7 Plan 7: Loaded libretro battery support Summary

**The real module publishes final core-owned save memory before execution and exposes copied HSC firmware through truthful achievement descriptors.**

## Accomplishments

- Added SaveKey and AtariVox EEPROM controller IDs and load-time HSC and per-port battery options. Explicit options override controller requests, then metadata supplies defaults. None preserves an ordinary selection and otherwise uses ProLine. No pre-load controller callback is required. Two EEPROMs refuse loading.
- SAVE_RAM immediately returns the exact public core span: absent NULL/0 or 2048/32768/34816 bytes. Actual CPU code reads imported HSC data and increments it; generated CPU random-read waveforms sample imported EEPROM data through either port and identity. All bytes and the pointer survive reset. Unload and no-content clear publication; reinitialization clears requests.
- Late requests retain installed hardware and report reload once per changed incompatible request. Auto and the installed serial identity remain valid; non-save controls retain existing behavior. Input errors are logged instead of silently discarded. No production private-state access, save-file operations, raw mirror or dirty scan was added.
- Attached HSC discovers optional highscre.bin only in the system directory. Missing firmware produces a visible signature-only stub notice. Present malformed, unreadable, nonregular, allocation or path failures refuse. Exact firmware is copied by the core before staging is freed, including BIOS retries. SHA-1 identity and missing-signature warnings are visible; no BIOS vector rule or invented known identity applies.
- SRAM descriptors alias the HSC prefix of SAVE_RAM and ROM descriptors reference only the public installed const span. The pinned rcheevos resolver checks all original maps and each retained board with both media and stub/supplied ROM. Supplied files are removed before resolving copied bytes. EEPROM has no CPU descriptor, absent HSC stays absent, and capacities are unchanged.
- Graduated public host semantics and preparation pointers. Save states, speech, full OpenHSC and firmware routines remain unavailable. The .info save and optional-HSC declarations stay unchanged until actual hosted persistence acceptance.

## Task Commits

1. Task 1 RED `c7e5cf0`; GREEN `a5a2c9f` — pre-frame publication, actual hardware imports, ordering and lifecycle.
2. Task 2 RED `6ebef27`; GREEN `e0fc69a` — visible optional firmware, failure controls and actual installed achievement bytes.
3. Task 3 `59bf9d0` — frontend documentation and source-comment reconciliation.

The persisted pre-plan ledger measures five task commits. The realized diff is 58753 characters; rounded-up chars/4 gives 14689 tokens. Duration includes the environmental timeout interruption.

## Commands, exits and run URLs

All URLs below are N/A-local. No log, runtime firmware or save bytes are committed.

| Command | Exit | Result |
|---|---:|---|
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^libretro[.]host[.]battery$' --no-tests=error --output-on-failure --output-junit battery-red.xml` | 8 expected | Target executed and failed on absent SAVE_RAM pointer and length before adapter wiring |
| `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/ci/battery-red.json --raw` | 0 | RED_EVIDENCE_OK; actual JUnit and semantic assessment retained only in ignored build output |
| `ctest --preset ci -R '^libretro[.]host[.]hsc_firmware$' --no-tests=error --output-on-failure --output-junit hsc-firmware-red.xml` | 8 expected | Target executed and failed specifically on absent visible stub warning |
| `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/ci/hsc-firmware-red.json --raw` | 0 | RED_EVIDENCE_OK; no discovery or setup error authorized GREEN |
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^libretro[.]host[.](battery|battery_order|devices|reset|xrgb8888|rgb565|options)' --no-tests=error --output-on-failure` | 0 | 8 checks including input fixture |
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(libretro[.](rcheevos|host[.](memory_map|hsc_firmware|battery))|diag[.]retail)$' --no-tests=error --output-on-failure` | 0 | 8 checks including fixture setup; real resolver plus existing retail diagnostics |
| `ctest --preset ci -R '^libretro[.](bios_failures|host[.]hsc_firmware)$' --no-tests=error --output-on-failure` | 0 | 2/2 firmware consumers |
| `cmake --workflow --preset ci` | 8 then 0 | First sandbox run had 13 unrelated timeouts after roughly 1034 seconds with process-denial diagnostics; unchanged unsandboxed run passed 640+6 in about 21 seconds; final complete run passed 640+6 in 77.61/0.45 seconds |
| `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` | 0 | Exactly 640 checks once each after final full run |
| `cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake` | 0 | Exactly 6 release checks once each |
| `ctest --preset ci -R '^(libretro[.]|docs[.])' --no-tests=error --output-on-failure` | 0 | Final module/doc focused run 26/26 |
| `ctest --preset ci -R '^docs[.]' --output-junit doc-final.xml --no-tests=error --output-on-failure` | 0 | Final prose correction 3/3; full inventory receipt preserved |
| `scripts/hygiene.sh --tree` and `git diff --check` | 0 each | No findings |
| `git diff --exit-code c7a6759 -- tests/built/expected.txt tests/roms/manifest.txt quality.json libretro/muhtari_libretro.info.in tests/cmake/info_claims.cmake` | 0 | Old pins, fixture bytes, numeric policy and pending capability claims unchanged |

Exactly three actual final macOS arm64 ZIPs total 228469 bytes, below 399937. This local result does not substitute for final six-native acceptance.

## TDD Gate Compliance

Both behavior tasks have machine-classified actual JUnit RED evidence, semantic inspection of the intended target assertion, a separate committed RED, and verified GREEN. No setup failure or unsupported report authorized implementation. No refactor commit was needed.

## Deviations from Plan

- Exact inventory entries were added with each registration because configure refuses inconsistent inventories. The host links the existing owned image builders to generate legal transient CPU programs; no new dependency or fixture byte admission was introduced.
- The first full workflow suffered widespread timeouts and sandbox process denials, without an assertion regression. Reran the unchanged workflow with ordinary process access; both complete reruns passed. No timeout, policy, expected digest or product behavior was loosened.
- Fixed the existing fast-boot prose to distinguish the console BIOS from optional installed HSC firmware. Source comments now describe attached HSC rather than unconditional holes.

## Ownership and Limits

ENGINEERING section 11 review retained the adapter as the frontend configuration owner, the instance as the only persistent-byte owner, and the frontend as file owner. The fixed snapshot caches a pointer and topology, not raw storage. Current public queries suffice; no private production API, general configuration system, dependency or descriptor-capacity increase was required. The existing actual-main firmware fault consumer and loaded host test independent observable behavior.

No new security surface outside the planned firmware reads was introduced. No unknown stub prevents this slice: the signature-only firmware stub is the intentional documented contract. No skipped or unrun plan verification remains. Whole SAVE requirements remain incomplete pending the owned diagnostic, fresh-process RetroArch and final portability/hosted acceptance in 07-08/09. Ten flagged assumptions remain visible in the phase plans pending final evidence disposition. No review-fix or gap allowance was consumed.

## Self-Check: PASSED

All eight modified files exist, all five task commits are reachable, both RED records were validated, full CI and exact inventories pass, tree hygiene passes, and protected policy/product inputs remain unchanged. No unexpected deletion or generated untracked file remains. The next plan is 07-08; the phase remains executing.
