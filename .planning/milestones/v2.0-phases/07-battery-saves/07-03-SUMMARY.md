---
phase: 07-battery-saves
plan: "03"
status: complete
subsystem: core
tags: [savekey, atarivox, i2c, riot, battery]
requires:
  - phase: 07-battery-saves
    provides: 07-02 authoritative raw battery span and frozen load topology
provides:
  - Bounded pin-level EEPROM through actual RIOT ORA and DDRA data ticks
  - Due page completion without polling and exact reset retention semantics
  - Documented source and compatibility-model boundaries with unchanged legacy pins
affects: [07-04, 07-05, 07-07, 07-08, 07-09]
tech-stack:
  added: []
  patterns:
    - Keep persistent bytes in the battery owner and finite pin/page state in the existing port owner
    - Settle due programming at actual execution ticks before resetting the clock epoch
key-files:
  created: []
  modified:
    - src/internal.h
    - src/ports.c
    - src/riot.c
    - src/bus.c
    - src/frame.c
    - src/instance.c
    - tests/input/test_input.c
    - tests/core/test_core.c
    - tests/built/chain.c
    - docs/hardware/controllers.md
    - docs/hardware/riot.md
    - docs/hardware/bus.md
key-decisions:
  - Use the selected fixed 35796-tick write interval as a compatibility model, not a measured hardware latency.
  - Preserve historical frame pins with explicit no-EEPROM test topology while testing automatic attachment through executed serial waveforms.
requirements-completed: []
requirements-contributed: [SAVE-02, SAVE-03]
coverage:
  - id: serial-pin-protocol
    description: Actual CPU ORA and DDR traffic programs and reads either port with SaveKey AtariVox and automatic metadata attachment
    requirement: SAVE-02
    verification:
      - kind: integration
        ref: "input.savekey.pins and input.savekey.protocol; focused CTest exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: serial-wrap-and-recovery
    description: ACK phases unsupported controls busy selection page wrap sequential wrap ignored A15 and interrupted traffic stay bounded
    requirement: SAVE-02
    verification:
      - kind: integration
        ref: "input.savekey.wrap and input.savekey.recovery; focused CTest exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: serial-completion-lifecycle
    description: Due programming respects exact access ticks completed instructions no-poll frames reset import generation and transactional topology
    requirement: SAVE-03
    verification:
      - kind: integration
        ref: "core.battery.deadline core.battery.no_poll core.battery.reset core.hsc.topology and core.no_alloc_after_load; exit 0; URL N/A-local"
        status: pass
    human_judgment: false
  - id: documented-model-and-bounds
    description: Source facts and selected timing match implemented ownership while local packages and all existing pins retain their bounds
    requirement: SAVE-03
    verification:
      - kind: other
        ref: "Agent read source and tests against hardware docs and ENGINEERING section 11; docs and quality checks exit 0; URL N/A-local"
        status: pass
      - kind: integration
        ref: "cmake --workflow --preset ci; 626 plus 6 tests; exact inventories and ZIP sum 223038 <=399937; exit 0; URL N/A-local"
        status: pass
actuals:
  tokens: 18803
  tasks: 3
  commits: 6
plan_head_before: b31e10a7a6cc9c04d8eb06f535d5e4051c9cbd57
plan_head_after: c9126e5c59fe85f9768b3b3df7de1e2d6a47edf1
duration: 15min
started: 2026-10-24T02:43:52Z
completed: 2026-10-24
---

# Phase 7 Plan 3: Serial EEPROM Summary

**SaveKey and AtariVox EEPROM now execute through RIOT pins, publish only due page writes, and retain committed bytes through power cycle.**

## Accomplishments

- Added instance-owned finite protocol state and one 64-byte page in the existing port owner. The single raw battery array remains authoritative; combined storage derives the EEPROM offset after HSC. ORA and DDRA updates use the actual slow-cycle data tick. Host drive and device SDA are separate, including low readback during output turnaround; reads and peeks do not invent pin events.
- Implemented control/address/data ACK, unsupported/busy NAK, repeated START, address-only setup, current/random/sequential reads, master ACK/final NAK, ignored A15, 64-byte page wrap and 32768-byte read wrap. A 130-byte write proves bounded overwrite of the same page. Incomplete control/address packets and un-stopped page data recover on documented STOP/START boundaries.
- STOP with data starts the selected fixed 35796-tick programming interval. Only touched bytes publish when due; a same-value page stays busy without dirtying. Completion catches up after actual frame execution, including instruction overshoot, JAM, DMA/WSYNC and partial BIOS refusal. Validation-only errors execute nothing. Reset settles already-due work, then discards future writes and protocol state while retaining committed bytes and the unsigned change token.
- Auto or the attached save identity remains accepted per frame. Adding, moving, removing or changing storage identity refuses the entire input before pending joystick state changes. Tests retain prior pointers, bytes, tokens and execution after refused loads, and prevent a due old page from leaking into a successful new load.
- Graduated pin/timing/recovery semantics and honest source/model limits. No speech, flat EEPROM CPU descriptor, extra save array, new dependency, post-load allocation or host-time completion path was introduced.

## Task Commits and TDD Evidence

1. Task 1 RED `bb33c32`, GREEN `2c441ad`: executed LDA/STA pin waveform then production protocol and wrap/recovery expansion.
2. Task 2 RED `53016a3`, GREEN `8a34104`; supplement `5e317d2`: no-poll completion/reset and actual partial-refusal/no-execution return checks.
3. Task 3 `c9126e5`: graduated docs, measured legacy topology preservation, default metadata waveform assertions and incidental whitespace-only hygiene corrections.

Both RED commands selected the intended target and produced fresh, unchanged JUnit reports in ignored build output. `check tdd-red-evidence` returned `RED_EVIDENCE_OK` for each. Semantic inspection confirmed planned assertions: the original waveform saw NAK/FF/zero generation because the protocol was absent; the no-poll test crossed its deadline in looping and JAM frames but still saw FF/zero/pending before frame catch-up existed. Setup, compilation and discovery errors were not accepted as RED. No refactor gate was needed. The six task commits are measured from the persisted ledger; 75,209 realized diff characters divided by four, rounded upward, yield 18,803 tokens on the estimate scale. Metadata commits are excluded.

## Commands, Exits and Run URLs

All evidence below is local; run URL is N/A-local. No runtime log, save, firmware or ROM bytes are committed.

| Command | Exit | Result |
|---|---:|---|
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^input[.]pa7_edge$' --no-tests=error --output-on-failure` | 0 | Temporary executed waveform plus original PA7 regression; passed before dedicated registrations |
| `ctest --preset ci -R '^input[.]pa7_edge$' --no-tests=error --output-on-failure --output-junit savekey-red.xml` | 8 expected | Named target failed planned protocol/readback assertions |
| `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/ci/savekey-red.json --raw` | 0 | RED_EVIDENCE_OK and semantic target inspection |
| `ctest --preset ci -R '^core[.]battery[.]no_poll$' --no-tests=error --output-on-failure --output-junit savekey-time-red.xml` | 8 expected | Named target failed due-page publication after actual frames |
| `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/ci/savekey-time-red.json --raw` | 0 | RED_EVIDENCE_OK and semantic target inspection |
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(input[.]savekey[.]|core[.]battery[.]|core[.]no_alloc_after_load$)' --no-tests=error --output-on-failure` | 0 | 8/8 dedicated checks |
| `ctest --preset ci -R '^(input[.]|riot[.])' --no-tests=error --output-on-failure` | 0 | All 25 original input/RIOT cases at Task 1 |
| `cmake --preset ci && ctest --preset ci -R '^(docs[.]|policy[.]quality[.]controls$)' --no-tests=error --output-on-failure` | 0 | 4/4 docs and actual evidence controls |
| `cmake --workflow --preset ci` | 0 | Final full run: 626/626 core tests and 6/6 release checks |
| `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` | 0 | Exactly 626 tests ran once |
| `cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake` | 0 | Exactly 6 tests ran once |
| `cmake --workflow --preset hygiene` | 0 | Final 24/24, including format, static analysis, tree and no-float |
| `cmake --build --preset ci --target package && ctest --preset ci-release --output-on-failure` | 0 | Three actual components and 6 release checks |
| Python exact-three archive enumeration and sum | 0 | Final library 71795, runner 80463, libretro 70780; sum 223038 <=399937 |
| `git diff --exit-code b31e10a -- tests/built/expected.txt tests/roms/manifest.txt quality.json` | 0 | All old ROM/frame pins, fixture bytes, numeric predicates and ceiling unchanged |
| `git diff --check` | 0 | No whitespace errors |

The first full CI run exited 8 solely on `basic.hiscoretable`: default metadata now attaches working SaveKey hardware and changes its picture. An ignored copy of the owned chain driver with an explicit unplugged port produced the exact old chain `9a32df28f09a0cdc002dbb7e6c463e8b3d09edc86ad63b520c5ea5882ce0bd9e`; the default path produced `d7c328b0a405ce74400b33a5a0f7b2ddbf9bec2c74a4f5857a0bbe33e6c02404`. Both observation commands exited 0. The actual probe independently reports the save hint. The committed owned chain driver now explicitly selects both ports unplugged for its historical no-EEPROM scenario, documented in `docs/emulation-tests.md`. Dedicated executed serial cases exercise actual default header/controller metadata and the save hint, as well as explicit placements. All historical pins pass without rebaselining.

## Ownership and Model Review

The existing port owner already consumes RIOT host drive and supplies controller pins. Keeping EEPROM there makes a pin change local; RIOT passes the actual data tick, the frame loop supplies the completed-execution tick, and the battery owner supplies the only persistent array. Finite phases, one page and fixed 64-byte completion work preserve boundedness. Static private helpers added are `savekey_start`, `savekey_stop`, `savekey_byte`, `savekey_read_bit`, `savekey_rise` and `savekey_fall`; no extra external helper surface or generic I/O framework was added. This follows ENGINEERING section 11 without a new registry, quota or numerical complexity score.

Manufacturer protocol facts were checked against Microchip DS20001203Y, fetched from its official PDF. The selected 35796 ticks is an integer compatibility interval based on the specified maximum, not a hardware measurement. Documentation and metadata keep AtariVox speech unavailable and the descriptor-less prohibition flagged-unverified. All ten phase assumption flags retain their lifecycle until the remaining planned evidence and phase review disposition.

## Deviations and Issues

- Exact inventory registration is enforced at configure, so the seven new cases required incremental CI/WASM inventory updates now instead of waiting for 07-04. The first configure refusal was not RED evidence; registration was corrected before the actual failing target run.
- The measured legacy frame setup required the narrow owned-driver topology correction described above, approved by the parent orchestrator within explicit override and pinned-observation contracts. No product default, fixture byte, pin or quality predicate changed.
- Hygiene exposed existing 07-02 formatting in `include/muhtari.h`, `tests/api/v4-layout.c` and `tests/api/v5.c`. Applied clang-format-only corrections; a Python comparison stripped whitespace from both HEAD and candidate bytes and proved all three identical. The final hygiene workflow passes.
- While diagnosing the sample chain, the agent opened excluded generated sample source before checking its broader provenance exclusion. That was a read mistake and stopped there. No external expression was reused; the serial implementation was already committed before that read, and the diagnosis/correction above relies on owned drivers, metadata and runtime observations only.
- Git writes used the required sandbox escalation with normal hooks and configured noreply identity. No authentication gate or owner verification was needed. No code-review fix or gap round was consumed.

## Remaining Phase Work

This plan contributes the serial and core lifecycle contracts; whole SAVE requirement completion remains pending the robustness, host and final evidence plans. Plan 07-04 can now exercise bounded generated traffic, fuzz replay, allocation and portability on the real model. Hosted/native Windows/PowerPC/final six-lane acceptance remain later work; local success does not claim them. No known stub, skipped test, unrun plan verification, new unplanned security surface or blocker remains in this plan.

## Self-Check: PASSED

All six task commits are reachable and all declared artifacts exist. Dedicated serial/core checks, full local CI, exact inventories, three actual archives and all hygiene checks pass. The protected fixtures, old pins and numeric policy match the pre-plan head. No unexpected tracked deletion or untracked generated file remains.
