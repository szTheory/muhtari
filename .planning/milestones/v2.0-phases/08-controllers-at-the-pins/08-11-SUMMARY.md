---
phase: 08-controllers-at-the-pins
plan: "11"
status: complete
subsystem: input
tags: [snes2atari, mega7800, riot, tia, eeprom]
requires:
  - phase: 08-10
    provides: Chronological bus delivery and extended per-port lifecycle
provides:
  - Complete finite SNES2Atari latch and17-bit serial response on both ports
  - Complete six-observation Mega7800 scan and release-based rearm
  - Real CPU edge, normal-mode, lifecycle and opposite EEPROM coexistence vectors
affects: [08-12, 08-13, 08-14]
tech-stack:
  added: []
  patterns:
    - Effective RIOT host levels remain separate from controller and EEPROM responses
    - Fixed per-port protocol state saturates without read-driven advancement
key-files:
  created: []
  modified:
    - src/internal.h
    - src/ports.c
    - src/riot.c
    - tests/input/test_input.c
    - tests/api/v6.c
    - tests/runner/tests.cmake
    - docs/hardware/controllers.md
key-decisions:
  - Reuse existing RIOT data-tick and bus catch-up ownership with separate effective host levels.
  - Mega responses use current held buttons and release pins3/4 high after finite completion until pin1 rearms.
  - Preserve legacy input refusal and battery ownership while extending only the opt-in device validator.
requirements-completed: []
requirements-contributed: [INPUT-01, INPUT-05]
coverage:
  - id: snes-scan
    description: Both ports independently return all12 buttons, four high padding bits and low17-plus tail with snapshot and latch priority.
    requirement: INPUT-05
    verification:
      - kind: integration
        ref: input.directions;real cartridge ORA/DDRA writes and every serial observation;exit0;N/A-local
        status: pass
    human_judgment: false
  - id: mega-scan
    description: Both ports return class00 and ID00 followed by C/B,Start/A,Y/Z,Mode/X with each assembled bit independently checked.
    requirement: INPUT-05
    verification:
      - kind: integration
        ref: input.directions;default dataFF and all eight single-button cleared-bit vectors;exit0;N/A-local
        status: pass
    human_judgment: false
  - id: effective-edges
    description: DDR-only edges count while duplicate writes, falling clocks, reads and peeks cannot advance or restart either finite scan.
    requirement: INPUT-05
    verification:
      - kind: integration
        ref: input.directions;partial scans, new latch, held latch, release rearm, pin1-low/pin2-high wait and extra strobes;exit0;N/A-local
        status: pass
    human_judgment: false
  - id: electrical-lifecycle
    description: SNES contention and TIA D6 follow PB/INPTCTRL changes immediately; Mega retains ordinary directions and B/C while A stays extended-only; load,Auto,replacement,reload and power-cycle reset protocols.
    requirement: INPUT-01
    verification:
      - kind: integration
        ref: input.directions,input.proline_two_button,input.proline_one_button,abi.api_v6 and runner.movie.v2_edges;exit0;N/A-local
        status: pass
    human_judgment: false
  - id: storage-isolation
    description: Both adapter families coexist with both EEPROM identities in both placements, completing pending pages while retaining published span/layout/identity and device-driven SDA ACK.
    requirement: INPUT-01
    verification:
      - kind: integration
        ref: input.savekey.pins;eight combinations,all32768 bytes,one generation increment,transactional refusals and unchanged real ACK state;exit0;N/A-local
        status: pass
    human_judgment: false
  - id: release-evidence
    description: Exact local CI and release inventories, hygiene and authenticated three-component ZIP observations pass on the completed implementation.
    requirement: INPUT-05
    verification:
      - kind: integration
        ref: workflow ci648+6,hygiene24 and quality_evidence.cmake at70465c9;exit0;N/A-local
        status: pass
    human_judgment: false
actuals:
  tokens: 10212
  tasks: 2
  commits: 4
plan_head_before: cfc76394fa3a444bb13865a4e2ce201b9ac320ac
plan_head_after: 70465c947d962356c20f8dac3d23f291bfbfa936
duration: 8min
started: 2026-10-25T00:56:12Z
completed: 2026-10-25
---

# Phase 8 Plan 11: Edge-clocked adapters Summary

**SNES2Atari exposes its complete17-bit snapshot and Mega7800 its six exact two-bit observations through finite host-pin edge state on either port, preserving opposite EEPROM programming.**

## Accomplishments

RIOT notifies the existing ports owner after ORA/DDRA writes at the slow
bus data tick. The bus has already delivered old due events through that
tick. Effective host drive is ORA|~DDRA; stored clock/latch/select levels
never come from controller responses or EEPROM-driven SDA. The existing
SaveKey notification and keypad owner retain their chronology. Each port
adds one fixed8-byte adapter state, with no allocation, new scheduler,
dependency, pulse filter or loop proportional to submitted scan length.
Only real host transitions mutate its finite position.

SNES latch rising snapshots B,Y,Select,Start,Up,Down,Left,Right,A,X,L,R.
Latch high holds bit1 and falling keeps it; clock rising with latch low
advances. Simultaneous rises give latch priority. Bits13..16 are high,
bit17 and all extra clocks low. The selected two-button +5V context masks
resistor-isolated DATA high as explicitly unmeasured electrical policy.
PB output/DDR and unlocked INPTCTRL changes immediately reach live/D6
inputs. No passive SNES direction contacts or SNES2Atari++ are invented.

Mega's pin4/pin3 observations are class00, identification00, C/B, Start/A,
Y/Z, Mode/X. All released assembles state0/dataFF; each single button
clears exactly its independently specified data bit7..0. Normal/rearmed
pin1 high permits ordinary directions and ProLine B/C; A is extended-only.
Low/low entry starts0, including a later pin2 fall after pin1 falls while
pin2 high. Pin2 rising advances, falling exposes, duplicate low cannot
restart. Position6 releases response pins high and stays complete until
pin1 high aborts/rearms, including DDR release. Current held buttons drive
responses without resetting position; console Reset is an input switch.
The documented class10=three-button and01/11=two-button interpretation
comes from the pinned driver's count table; only class00 is emulated.
Malformed/restart/completion choices are compatibility policy, not captures.

Tests execute actual cartridge LDA/STA instructions for every returned bit
or pair on both ports, including read/peek/duplicate observers, partial and
extra clocks, DDR-only edges, held snapshots and new latches. They check
normal electrical modes and independent B/C buttons, unchanged same-device
state, replacement, immediate explicit load override, Auto resolution,
reload and power-cycle release. Existing test registrations and inventories
are retained. Legacy input layout/refusal and frozen clients are unchanged.

The real EEPROM waveform harness covers both adapter families, both
SaveKey/AtariVox identities and both legal placements. A committed two-byte
page crosses its actual completion tick during scanning. All32768 bytes,
exactly one generation increment, span pointer/length/layout, identity and
placement are checked. Rejected remove/replace/add/invalid packets retain
the accepted input transactionally. Another scan holds an actual device ACK
low while all other-port scan writes leave the complete EEPROM state
unchanged; the device response cannot feed host edges.

Hardware docs cite the original design, physical SNES report and exactly
the two pinned CC0 drivers. The drivers were read as protocol facts; no
assembly expression, compiler source or excluded include was imported.
The manufacturer establishes Mega normal/extended product behavior; the
precise tables and policies are labelled separately. No INPUT requirement
is complete: executable adapter diagnostics and frontend mappings remain
in the following plan.

## Task Commits

1. Task1 RED `1d22ee1`, GREEN `9ae0d4d`: complete SNES scan, real effective edges, snapshot/lifecycle/electrical vectors and hardware docs.
2. Task2 RED `0064d2e`, GREEN `70465c9`: complete Mega pairs/rearm/normal mode, opposite EEPROM programming/ACK, lifecycle tests and docs.

All normal hooks ran with the GitHub noreply identity. The supplied-root
pin and protected-branch assertion passed before every commit. No tracked
file was deleted. Seven implementation/test/doc files changed. No refactor
commit was needed. The parent exclusively owns STATE.md, ROADMAP.md and
REQUIREMENTS.md; this executor did not edit, stage or commit them.

## TDD Gate Compliance

Both tasks executed meaningful behavioral RED before implementation and
then GREEN. The unchanged actual JUnit reports and JSON records stay in
ignored build evidence. Each target executed and failed the intended valid
adapter acceptance assertion with unsupported13 instead of0; parser or
build failures did not authorize implementation. Commands, exits and run
URLs are local facts, **N/A-local**:

- Task1: `ctest --preset ci -R '^input[.]directions$' --output-junit ../snes-red.xml --output-on-failure`, exit8. `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/snes-red.json --raw`, exit0, RED_EVIDENCE_OK.
- Task2: `ctest --preset ci -R '^input[.]directions$' --output-junit ../mega-red.xml --output-on-failure`, exit8 after the existing SNES coverage passed. `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/mega-red.json --raw`, exit0, RED_EVIDENCE_OK.

Later corner/lifecycle/storage expansions were initially green against the
implemented state machines. They add required coverage and are not claimed
as extra RED cycles. The first Mega test compile exposed integer promotion
signedness in its assembled-byte expression; an explicit unsigned shift
fixed the test before behavioral RED. The later storage test initially
named the public layout field incorrectly; it was corrected to
layout_version before that verification ran. Neither compile failure is
presented as a behavioral failure or pass.

## Verification Evidence

Every command below exits0; run URL **N/A-local**. No hosted acceptance,
six-native result or owner verification is claimed.

- Task1 and its post-commit tracer feedback rerun: `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^input[.]' --no-tests=error --output-on-failure`,17/17.
- Task2: `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(input[.]|riot[.])' --no-tests=error --output-on-failure`,30/30.
- Broader early boundary check: `cmake --build --preset ci && ctest --preset ci -R '^(input[.]|riot[.]|abi[.]api_v6|runner[.]movie[.]v2_edges)' --no-tests=error --output-on-failure`,33/33.
- `cmake --workflow --preset ci`,648/648 plus package production and release6/6. It passed before Task2 GREEN and was rerun successfully at exact committed source70465c947d962356c20f8dac3d23f291bfbfa936 to authenticate final package identity.
- `cmake --workflow --preset hygiene`,24/24, including actual formatter, analyzer, integer-only and tree checks. `git diff --check`,exit0.
- `cmake -DROOT=. -DBUILD=build/ci -DOUT=build/phase08-11-quality.json -DSHA=$(git rev-parse HEAD) -DCONFIG=ci -DSOURCE=local -P tests/cmake/quality_evidence.cmake`,exit0. Actual consumer confirms exact648+6 inventories, manifest/sample identity and all three ZIPs.
- `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check evaluation-scope --plan 08-11 --commits-only --raw`,exit0, resolves all four reachable task commits.
- Independent Python/Git checks of changed-file existence, ancestor commits, persisted ledger count, ZIP count/sum against producer JSON, and unchanged inventories/manifest/diagnostic/sample expectations exit0. Added-line stub scan finds no TODO/FIXME/placeholder or unfinished output.

## Package Observations

Actual macOS arm64 packages from the completed workflow at70465c9:

| Component | Bytes |
|---|---:|
| library | 80627 |
| libretro | 84195 |
| runner | 93802 |
| Total | 258624 |

The actual producer reports native/local/measured and exact source,
compiler/environment, manifest and sample identities. The preceding08-10
local total256473 at0f590fe is2151 bytes smaller. These are attributable
local observations, not a manufactured candidate/base acceptance or a
six-native claim. Final genuine event-base comparison and six-native
measurements remain08-14. Contents/notices, firmware exclusion and platform
floor checks all pass. DEC.61 preserves required measurements/integrity
without reinstating the retired archive cutoff or introducing a quota.
No hosted run was dispatched solely for intermediate package sizes.

## Deviations from Plan

**1. Rule2 — preserve evolving opt-in validation contracts.** Both task
implementations necessarily update tests/api/v6.c and the existing real
runner movie2 consumer in tests/runner/tests.cmake, beyond the listed
artifact paths. Valid newly supported adapters now execute; inapplicable
paddle data and unknown button bits still fail transactionally. Legacy
refusals remain frozen. Commits9ae0d4d/70465c9; boundary checks and complete
CI pass. No new registration, framework or inventory adjustment is needed.

**2. Rule3 — retain proven bus and TIA ownership.** No src/bus.c or TIA
edit is needed: existing bus access catch-up and immediate PB/INPTCTRL
notifications already provide the required chronology and D6 behavior.
The RIOT seam calls only the private ports helper. Actual CPU electrical,
read/peek and opposite-storage checks pass in their owning GREEN commits.

## Issues Encountered

The initial ledger write was denied by the filesystem sandbox before any
commit; normal Git metadata escalation then succeeded with hooks and no
bypass. Two test-only compile corrections are recorded in the TDD section.
No implementation test, full workflow, hygiene or package check failed
after its final owning edits. No auth gate, remaining blocker, known stub,
skipped test, unrun required verification or new trust-boundary surface
exists. Physical undocumented adapter behavior remains unmeasured, as
explicitly required by the selected models.

## Next Plan Readiness

This completes08-11 only. The parent owns shared position and accepted
counts; next dependency-ready work is08-12. No other GSD step, PR, ship,
merge or release was invoked. Exact next separate command:
`$gsd-execute-phase 8`.

## Self-Check: PASSED

All seven declared changed files exist. All four task commits are
ancestors of HEAD and resolve through evaluation-scope. The persisted
ledger measures4 commits fromcfc7639 through70465c9. The realized diff
measures10212 estimate-scale tokens, characters/4 floored, excluding this
metadata summary. Exact648+6 committed-source CI/package checks, hygiene24,
actual pin/storage consumers and authenticated three-ZIP measurements
pass. No tracked deletion, unexpected untracked output, fixture repin or
inventory change remains. Duration8min is measured from00:56:12Z through
verification closeout01:04:35Z; preceding context loading is not separately
timed.
