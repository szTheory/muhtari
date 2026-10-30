---
phase: 08-controllers-at-the-pins
plan: "10"
status: complete
subsystem: input
tags: [cx22, st-mouse, amiga-mouse, riot, libretro, diagnostics, tdd]
requires:
  - phase: 08-09
    provides: Proven chronological controller deadlines and independent three-host diagnostic consumer
provides:
  - Fixed two-axis CX22 direction/toggles and connector-specific ST/Amiga waveforms
  - Three independently judged27-record motion scenarios with retained driving19 records
  - Distinct MOUSE subclasses with complete buttons and separate console controls
affects: [08-11, 08-13, 08-14]
tech-stack:
  added: []
  patterns:
    - Reuse the existing chronological event owner with four fixed axis states
    - Judge connector-labelled numeric tuples before comparing host frames and audio
key-files:
  created: []
  modified:
    - src/ports.c
    - src/internal.h
    - tests/input/test_input.c
    - tests/api/v6.c
    - tests/runner/tests.cmake
    - tests/diag/controller-motion.s
    - tests/diag/controller-motion.a78
    - tests/diag/expected.csv
    - tests/diag/driver.c
    - tests/diag/tests.cmake
    - tests/roms/manifest.txt
    - libretro/libretro.c
    - tests/libretro/host.c
    - docs/hardware/controllers.md
    - docs/libretro.md
    - docs/emulation-tests.md
key-decisions:
  - Preserve chronological delivery through the existing bus owner; no bus scheduler change is necessary.
  - Keep CX22 direction/toggle state separate from explicit ST and Amiga connector closure tables.
  - Reuse the actual numeric mailbox consumer and immutable movie2 path for every motion family.
requirements-completed: []
requirements-contributed: [INPUT-01, INPUT-04]
coverage:
  - id: connector-wiring
    description: Both axes and directions on both ports follow CX22 direction/toggles and distinct ST/Amiga connector cycles with independent buttons.
    requirement: INPUT-04
    verification:
      - kind: integration
        ref: input.pa7_edge;actual slow CPU reads, every connector cycle, idle, buttons and dump;exit0;N/A-local
        status: pass
    human_judgment: false
  - id: chronological-edges
    description: Every autonomous PA7 edge survives complete unpolled cycles and HALT/JAM, including exact read-clear ordering and simultaneous axes.
    requirement: INPUT-04
    verification:
      - kind: integration
        ref: input.pa7_edge;both edge polarities, before/at/after data ticks and unpolled full cycles;exit0;N/A-local
        status: pass
    human_judgment: false
  - id: bounded-input
    description: Four independent axes saturate signed extremes and residuals, cancel opposite inputs, consume once and reset replaced devices.
    requirement: INPUT-01
    verification:
      - kind: integration
        ref: input.pa7_edge,input.directions,abi.api_v6 and runner.movie.v2_edges;complete648-test CI;exit0;N/A-local
        status: pass
    human_judgment: false
  - id: numeric-oracle
    description: Owned scenarios1/2/3 require27 independent records each and reject wrong-family or axis-swapped tuples, counts, IDs and protocol faults while preserving driving19.
    requirement: INPUT-04
    verification:
      - kind: integration
        ref: diag.controllers,diag.rebuild,policy.quality.controls;byte-identical rebuild and actual numeric/fault consumers;exit0;N/A-local
        status: pass
    human_judgment: false
  - id: frontend-mappings
    description: Distinct base-MOUSE subclasses query both axes once, preserve all buttons and expose separate console controls with ordinary-provider priority and immutable opposite EEPROM.
    requirement: INPUT-01
    verification:
      - kind: integration
        ref: libretro.host.devices,libretro.host.input and diag.controllers actual module packet/query vectors;exit0;N/A-local
        status: pass
    human_judgment: false
  - id: three-deliverables
    description: Library packets, actual immutable movie2 runner processes and actual modules satisfy independent numbers before native/public frames and raw/PCM audio equality.
    requirement: INPUT-04
    verification:
      - kind: integration
        ref: diag.controllers;all seven controller cases,12 frames each, sample counts and stereo copies;exit0;N/A-local
        status: pass
    human_judgment: false
  - id: package-integrity
    description: Full local CI, exact inventories, package integrity and authenticated three-component ZIP measurements pass without a hosted size-only run.
    requirement: INPUT-04
    verification:
      - kind: integration
        ref: workflow ci648+6,explicit package/release6,hygiene24 and quality_evidence.cmake at0f590fe;exit0;N/A-local
        status: pass
    human_judgment: false
actuals:
  tokens: 19413
  tasks: 3
  commits: 6
plan_head_before: daacaf6659f8a416f43c4aed88f441d4a9305e74
plan_head_after: 0f590fec56ab7f1377ad4b92e489d0eee53eed26
duration: 8min
started: 2026-10-25T00:44:27Z
completed: 2026-10-25
---

# Phase 8 Plan 10: CX22 and ST/Amiga timed motion Summary

**CX22 direction/toggles and connector-specific ST/Amiga axes produce bounded chronological pins and pass independent numbers through library, immutable runner replay and loaded libretro modules.**

## Accomplishments

The existing bus deadline owner now serves two fixed axes on each port.
CX22 saves direction and toggles independently; ST and Amiga retain explicit
connector closure tables, including ST's RIGHT/UP to API right/down conversion.
All begin high and preserve phase/direction while idle. Four axes independently
clamp int32 inputs and residuals to±255 before overflow; excess is discarded.
The454-tick minimum spacing and fixed state bound work to263 transitions per
axis/full NTSC frame,1052 total. These are derived bounds and fitted policies,
not measured operating cost or physical speed calibration. Plan13 owns the
comparable operating-cost measurements.

Real cartridge CPU accesses check every family/axis/sign/port, all four phases,
primary fire, independent mouse right fire and TIA dump. Complete unpolled
cycles set PA7 despite ending at their initial pins, under JAM and HALT and
for both selected polarities. Same-tick axes resolve together. Fresh TIMINT
reads one tick before/at/after a relevant edge prove data-tick clearing: an
early read returns0 then the edge at its completion tick sets the flag; at
or after reads return64 then clear it. Four-axis INT32_MIN/MAX tests saturate
repeated submissions, drain through frame crossings, remain idle, cancel
opposite residuals and reset replaced devices. Existing driving timing and
frozen legacy validation remain unchanged.

The owned motion executable retains scenario0's19 driving records and adds
27 records each for1=CX22,2=ST,3=Amiga. Fixed devices and startup-switch
selectors begin each fresh execution. Frame3 moves X+4/-4, frame4 reverses
one unit, frame5 holds without a library submission; frame6 moves Y+4/-4,
frame7 reverses, frame8 releases. Independent connector-labelled SWCHA
vectors are documented before equality. CX22 X is FF,DC,FE,DC,FE versus
ST FF,DE,CC,ED,FF and Amiga FF,D7,55,7D,FF; Y and reverse tuples further
distinguish axis wiring. Button, idle and PA7 records are explicit. Actual
report consumers reject CRC-valid wrong-family and axis-swapped tuples,
changed final values/IDs, duplicate IDs, wrong counts, CRC and missing or
truncated reports. Agreement among hosts cannot override the numeric judge.

Byte-identical rebuild SHA-256 is
`33242ded7334df52bfcac8a75bd82bf096611710460d4f3df94aee8a74525106`.
The manifest source pincc11307 contains the exact current assembly, compared
byte for byte. Only this phase-owned fixture hash/source row changes. Every
pre-plan expected row and every other manifest row is preserved, as are exact
test/sample inventories and historical observations. Actual fixture admission
controls pass, including rejection of changes to already trusted rows and
later-base cases; no candidate-made comparison base was introduced.

Libretro advertises13 choices on both ports. CX22/ST/Amiga select MOUSE
subclasses1/2/3, preserving driving0 and all previous identities. Actual
base-MOUSE X/Y are queried once per frame; CX22 LEFT or RIGHT is shared fire,
while mice preserve independent LEFT/RIGHT. MIDDLE/BUTTON_4/BUTTON_5 supply
Pause/Select/Reset when the motion device supplies console controls. Ordinary
ports retain priority and difficulty-edge behavior; other providers use the
existing difficulty options. Descriptors refresh on selection; wheel inputs
never reach emulated pins. Host tests cover both ports, opposite EEPROM
placements and frozen published storage, Auto/header selection, callbacks,
provider changes, descriptor identities and missing input. Actual test-module
packet vectors cover signed extremes, buttons, ignored wheel fields, one poll
per frame, exact base query counts and neutral missing callbacks.

Library, actual runner children replaying generated immutable movie2 bytes,
and actual modules each pass independent motion numbers before comparing all
12 native frames, module public pixels, actual raw transition streams, exact
PCM/counts and stereo copies. Existing light, paddle and keypad consumers
remain enabled. No dependency, new scheduler or new test framework was added.

## Task Commits

1. Task1 RED `18ed05f`, GREEN `b82d6c4`: connector cycles, unpolled PA7, fixed axes, extended validation and hardware documentation.
2. Task2 RED `da22ef6`, GREEN `cc11307`: independent numeric expectations, owned executable, rebuild and actual report faults. GREEN also formats the preceding task's code using the required formatter.
3. Task3 RED `b9d607d`, GREEN `0f590fe`: distinct module mappings, complete host evidence, four-axis/boundary test expansion, fixture source pin and docs.

All normal hooks ran with the GitHub noreply identity. No refactor commit,
tracked deletion or dependency change was needed. Sixteen production/test/doc
files changed. Shared STATE.md, ROADMAP.md and REQUIREMENTS.md belong to the
parent and were not edited, staged or committed by this executor.

## TDD Gate Compliance

Each task executed a meaningful behavioral RED before its owning implementation,
then GREEN. Actual unmodified JUnit reports, run starts/mtimes, exit statuses
and semantic assessments remain in ignored build evidence only. All classifiers
returned RED_EVIDENCE_OK, exit0, run URL N/A-local:

```sh
node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/mouse-red.json --raw
node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/mouse-diag-red.json --raw
node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/mouse-map-red.json --raw
```

- Task1: `ctest --preset ci -R '^input[.]pa7_edge$' --output-junit ../mouse-red.xml --output-on-failure`, exit8. The executed valid CX22 assertion received unsupported13 instead of0. Initial compile/signature corrections were setup failures and did not authorize GREEN.
- Task2: `ctest --preset ci -R '^diag[.]controllers$' --output-junit ../mouse-diag-red.xml --output-on-failure`, exit8. The actual pre-expansion executable completed selector1 with only1 record; the required27-record numeric contract failed. All four existing controller scenarios passed first.
- Task3: `ctest --preset ci -R '^libretro[.]host[.]devices$' --output-junit ../mouse-map-red.xml --output-on-failure`, exit8. The actual module advertised10 instead of13 choices on both ports; its input fixture passed.

Later saturation, boundary and callback expansions that the implementation
already satisfied are initially-green coverage, not additional RED claims.
No missing symbol, setup, discovery or parser failure was relabeled RED.

## Verification Evidence

All facts below are local, run URL **N/A-local**. No hosted acceptance is claimed.

- Task1 command and post-commit tracer rerun exit0: `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^input[.]pa7_edge$' --no-tests=error --output-on-failure`. The broader input/RIOT/API/runner check passes33/33.
- Task2 command exits0,2/2: `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(diag[.]rebuild|diag[.]controllers)$' --no-tests=error --output-on-failure`.
- Focused actual-module check exits0,4/4: `cmake --build --preset ci && ctest --preset ci -R '^(diag[.]controllers|libretro[.]host[.](devices|input))$' --output-on-failure`.
- Final Task3 sequence exits0: `cmake --workflow --preset ci && cmake --build --preset ci --target package && ctest --preset ci-release --output-on-failure`. Complete CI648/648, workflow release6/6 and explicit package/release rerun6/6 pass. Exact inventories are unchanged.
- `cmake --workflow --preset hygiene` exits0,24/24, including formatting, analyzer, integer-only and tree hygiene. `git diff --check` exits0.
- `cmake -DROOT=. -DBUILD=build/ci -DOUT=build/phase08-10-quality.json -DSHA=$(git rev-parse HEAD) -DCONFIG=ci -DSOURCE=local -P tests/cmake/quality_evidence.cmake` exits0 at0f590fec56ab7f1377ad4b92e489d0eee53eed26. The actual producer verifies648+6 inventories and measures all three archives with source/environment/manifest/sample identity.
- `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check evaluation-scope --plan 08-10 --commits-only --raw` exits0 and resolves all six reachable task commits.
- Independent Python assertions compare all historical expected/manifest rows, the committed assembly source, exact inventories, changed-file existence and ancestor commits; exit0.

## Package Observations

Actual completed-workflow macOS arm64 ZIPs, source0f590fe:

| Component | Bytes |
|---|---:|
| library | 79860 |
| libretro | 83497 |
| runner | 93116 |
| Total | 256473 |

The existing producer reports native/local/measured with exact source identity.
The preceding08-09 local total255087 at2815568 is1386 bytes smaller. These
are actual local observations, not candidate/base acceptance or six-native
claims. The existing producer hashes actual manifest/sample authority and
fabricates no comparison base. Final genuine event-base and exact-source
six-native evidence remains08-14. Contents/notices, firmware exclusion and
platform-floor controls pass. DEC.61 preserves measurement and integrity
without restoring the retired archive cutoff or inventing a quota. No hosted
matrix was dispatched solely for size.

## Deviations from Plan

1. **Rule2 — preserve evolving validation contracts:** tests/api/v6.c and tests/runner/tests.cmake move the three newly supported families from temporary unsupported expectations to real motion acceptance. Remaining adapter refusals, invalid payloads and frozen legacy tests remain intact. Committed inb82d6c4;33 focused checks and full CI pass.
2. **Rule3 — reuse the proven event owner:** src/bus.c already drains each deadline chronologically, including stalls; it requires no edit. Four fixed axis states extend only the current ports owner. Committed inb82d6c4; exact PA7/read-clear tests and complete CI pass.
3. **Rule3 — execute diagnostics before module expansion:** tests/diag/tests.cmake activates new numeric/runner cases in Task2 while module selection is enabled with Task3. This supplies real executable RED/GREEN evidence without another harness. Committed incc11307/0f590fe; actual consumers pass.
4. **Rule3 — retain documentation budget:** consolidate overlapping input-test prose within40960 bytes instead of raising the limit. Final emulation-tests page40949 bytes passes the real budget consumer and full CI, committed in0f590fe.

## Issues Encountered

The first full workflow exits8,647/648 passed: emulation-tests was40979 bytes,
19 over its existing budget. The final corrected workflow exits0,648+6.
A wrapper then attempted to assign zsh's readonly variable named status and
exited1; this was a reporting-shell error after the failed workflow, not a
different test result or a pass. The final run preserves its actual exit.

During development, a sequential before-edge read at553 followed by an
at-edge read554 could not fit two real instructions. The cycle test now uses
an earlier observation and independently fresh before/at/after executions.
A long assembly branch exceeded6502 range and was replaced with a local
branch plus jump. The first expanded diagnostic cleared PA7 before Y,
which erased Amiga's X-pin4 history; clearing before frame3 correctly covers
each family's real pin4 axis. No expected tuple was repinned to observations.
The new exact clear-boundary test initially expected the flag to stay clear
after a read at edge-1; bus completion correctly delivers the next-tick edge,
so the test now asserts that documented ordering. Existing Auto/header host
expectations were extended to the newly implemented mouse families.

No authentication gate, known stub, skipped test, unrun required verification,
new trust-boundary surface or remaining blocker exists. Cadence and saturation
remain explicit uncalibrated policies. No INPUT requirement is complete and
no owner verification is required.

## Next Plan Readiness

This completes08-10 only. The parent owns shared planning position and
accepted counts. Next dependency-ready plan is08-11. No other GSD step,
PR, ship, merge or release was invoked. Next separate command:
`$gsd-execute-phase 8`.

## Self-Check: PASSED

All16 declared changed files exist and all six task commits are ancestors
of HEAD and resolve through evaluation-scope. The persisted ledger measures6
commits fromdaacaf6 through0f590fe. The realized diff measures19413
estimate-scale tokens, characters/4 floored. Complete648+6, explicit release6,
hygiene24, byte-exact rebuild, actual numeric/fault/module consumers and
authenticated package producer pass. No tracked deletion, unexpected untracked
output, unrelated fixture change or exact-inventory edit remains. Duration8min
is measured from the first persisted RED start00:44:27Z through summary
closeout; preceding context loading is not separately timed.
