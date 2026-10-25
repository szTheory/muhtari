---
phase: 08-controllers-at-the-pins
plan: "09"
status: complete
subsystem: input
tags: [keypad, riot, tia, libretro, diagnostics, tdd]
requires:
  - phase: 08-08
    provides: Timed pot state and independent analog diagnostic consumer
provides:
  - Seven-node connected keypad switches with exact2864-tick pot recovery
  - Owned27-record keypad scenario preserving all19 paddle records
  - Twelve independent RetroPad key mappings and separate console controls
affects: [08-10, 08-14]
tech-stack:
  added: []
  patterns:
    - Reuse existing pot deadlines and chronological bus owner for keypad recovery
    - Bound switch closure to seven passes over twelve held switches
key-files:
  created: []
  modified:
    - src/ports.c
    - src/riot.c
    - src/internal.h
    - tests/input/test_input.c
    - tests/api/v6.c
    - tests/runner/tests.cmake
    - tests/diag/controller-analog.s
    - tests/diag/controller-analog.a78
    - tests/diag/expected.csv
    - tests/diag/driver.c
    - tests/diag/tests.cmake
    - tests/roms/manifest.txt
    - libretro/libretro.c
    - tests/libretro/host.c
    - docs/hardware/controllers.md
    - docs/libretro.md
    - docs/emulation-tests.md
    - .planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md
key-decisions:
  - Reuse the existing pot action/deadline storage instead of adding a separate timer owner.
  - Propagate only real ground sources through switches; retained capacitor low never becomes a new ground source.
  - Keep the existing actual diagnostic judge and exact inventories, extending its owned scenario and faults.
requirements-completed: []
requirements-contributed: [INPUT-01, INPUT-03]
coverage:
  - id: switch-network
    description: Every row and column on both ports follows effective RIOT pins, connected switches and low-dominant contention.
    requirement: INPUT-03
    verification:
      - kind: integration
        ref: input.directions and diag.controllers;all12 keys, all-input/high/low rows, ghost paths and output readback;exit0;N/A-local
        status: pass
    human_judgment: false
  - id: recovery-history
    description: Analog columns recover at2864 elapsed master ticks with stable history and live or latched trigger behavior.
    requirement: INPUT-03
    verification:
      - kind: integration
        ref: input.directions and diag.controllers;before/at/after edges, repeated writes/polls, D7/D6, HALT/JAM and mixed-device power-on;exit0;N/A-local
        status: pass
    human_judgment: false
  - id: packet-contract
    description: All simultaneous key bits are accepted while malformed or unrelated payloads preserve pending input and legacy behavior.
    requirement: INPUT-01
    verification:
      - kind: integration
        ref: input.directions,abi.api_v6,abi.api_v6_layout,runner.movie.v2_edges and complete648-test CI;exit0;N/A-local
        status: pass
    human_judgment: false
  - id: owned-keypad-oracle
    description: Actual scenario1 requires27 independent matrix records and rejects wrong or missing new records without replacing paddle observations.
    requirement: INPUT-03
    verification:
      - kind: integration
        ref: diag.controllers,diag.rebuild,policy.quality.controls;identity16,scenario1,required count/IDs/numbers/CRC and actual consumer faults;exit0;N/A-local
        status: pass
    human_judgment: false
  - id: complete-host-mapping
    description: All twelve keys and separate console controls are reachable on both ports, with provider changes and immutable opposite storage.
    requirement: INPUT-01
    verification:
      - kind: integration
        ref: libretro.host.input,libretro.host.devices and actual module packet vectors;individual keys,4/all12 masks, callback IDs/descriptors, options and EEPROM freeze;exit0;N/A-local
        status: pass
    human_judgment: false
  - id: three-deliverables
    description: Library packets, actual immutable movie2 runner processes and loaded modules independently pass numbers and agree on frames and audio.
    requirement: INPUT-03
    verification:
      - kind: integration
        ref: diag.controllers;complete12-frame native/public pixels, actual raw transitions and exact PCM/counts;exit0;N/A-local
        status: pass
    human_judgment: false
  - id: release-integrity
    description: Complete local CI, package integrity, exact inventories and authenticated three-ZIP measurements pass.
    requirement: INPUT-03
    verification:
      - kind: integration
        ref: workflow ci648+6,explicit release6,hygiene24 and quality_evidence.cmake at2815568;exit0;N/A-local
        status: pass
    human_judgment: false
actuals:
  tokens: 17795
  tasks: 3
  commits: 6
plan_head_before: c7554b6ee85dd4601987444bafa3182262bd5370
plan_head_after: 2815568302b6a7a04d6eb3c3fbea0b3c2751622a
duration: 13min
started: 2026-10-25T00:28:34Z
completed: 2026-10-25
---

# Phase 8 Plan 9: Connected keypad matrix Summary

**Both keypads resolve real row pins through connected switches, recover pot columns at2864 master ticks and pass independent numbers through library, movie2 runner and libretro.**

## Accomplishments

The fixed network uses four rows and three columns, row-major1,2,3 /
4,5,6 /7,8,9 /*,0,#. Effective ORA/DDRA low outputs ground their components;
input rows release and driven-high rows retain ordinary output readback.
Seven bounded mask passes propagate ground through closed keys, including
D7-grounded pot columns. Mixed high/low contention is explicitly low-dominant.
No diode isolation or arbitrary three-key cap is assumed. The implementation
adds two key masks and two node masks, reusing existing pot state and event
ownership without allocation, floating point or per-tick iteration.

Last-ground disappearance projects a2864-master-tick recovery. Repeated
reads, ORA/DDRA writes and unchanged packets do not restart it. Real bus
tests cover every key on both ports and exact before/at/after boundaries,
early retained charge, ghosting, four/all12 keys, D7 overlap, live trigger,
D6 retention, JAM/HALT, reset and mixed-device power-on. The reset owner now
selects the earliest of all four pot deadlines, including mixed attachments.
Keypad payloads permit only the low12 mask bits; inapplicable fields refuse
without pending mutation. The frozen legacy setter and packet remain intact.
The old preparation recommendation counted executed cycles and inferred a
three-key limit; its section now points to the implemented source-backed
wiring and explicitly fitted recovery/contention policy.

The existing MIT controller-analog executable retains all19 scenario0 paddle
records and adds27 scenario1 keypad records. Reset is held in startup
frames1/2; both fixed keypad ports hold keys1,2,3,4 through frame3, release
in4, hold all12 in5, hold3,6 in6 and release thereafter. Independent expected
numbers are documented before equality. Exact CPU reads occur at release
+2862/+2864/+2868 on each port, with values0/128/128. The derivation is
1 release-completion tick +2804 loop ticks +42 BIT ticks +17 input-data
ticks =2864. Page alignment prevents branch carry. Other records judge
all rows, same-column history, fourth-corner ghosts, contention/readback
and D7/D6. The actual consumer rejects CRC-valid wrong values and damaged
IDs in the newly required final record, alongside every old protocol fault.

Rebuild SHA-256 is
`157f9b90cb7a50480c530668fd9c61c519ba8b53e63162d33ce508560152c891`.
The final manifest source pin6a45e21 contains the exact current assembly,
compared byte for byte. Only this phase-owned binary/hash row changed;
all old expected rows, other manifest rows, sample observations and exact
inventories remain unchanged. Actual admission controls still reject changes
to an already trusted analog row, including later comparison bases. No
candidate-made base or identity exception was added.

The tenth libretro choice is Keypad, JOYPAD subclass4, preserving all old
identities. Base JOYPAD callbacks map1=Y,2=Up,3=X,4=Left,5=B,6=Right,7=L,
8=Down,9=R,*=Select,0=A,#=Start. Individual mapped bits and4/all12 simultaneous
masks are independently checked in actual loaded-module packets. Keypad keys
have no console side effects; supplemental L2/R2/L3 supply Pause/Select/Reset
when keypad supplies console controls. Ordinary pads retain priority and
difficulty edge history, while keypad difficulties use options. Host tests
cover both connectors, exact labels/base queries, one poll per frame, all
key reads once, provider changes, Auto, missing callbacks, unknown identities,
and storage freeze on the opposite port. Fixed descriptor storage holds
both12-key ports plus three console controls and terminator.

Library, real runner children replaying generated immutable movie2 bytes,
and actual modules each satisfy all27 independent numbers before twelve
native frames, module public pixels, actual raw transitions and exact PCM
samples/counts are compared. Motion/light/paddle consumers remain enabled.

## Task Commits

1. Task1 RED `294ed81`, GREEN `d55d597`: row selection, graph, exact recovery, validation and hardware documentation.
2. Task2 RED `5bc152e`, GREEN `6a45e21`: independent scenario1 expectations, actual executable, rebuild and report faults.
3. Task3 RED `3ddfe3f`, GREEN `2815568`: twelve-key mapping, separate console controls, complete three-host observations, source pin and docs. It also adds the mixed-device earliest-deadline reset check.

All normal hooks ran with the GitHub noreply identity. No refactor commit,
tracked deletion or dependency change was needed. Eighteen production/test/doc
files changed. Shared STATE.md, ROADMAP.md and REQUIREMENTS.md belong to the
parent and were not edited, staged or committed by this executor.

## TDD Gate Compliance

Each owning task executed a meaningful behavioral RED before its implementation,
then GREEN. The real unmodified JUnit reports, actual run starts/mtimes, exit
statuses and semantic assessments are stored only in ignored build evidence.
All three classifier calls returned RED_EVIDENCE_OK, exit0, N/A-local:

```sh
node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/keypad-red.json --raw
node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/keypad-diagnostic-red.json --raw
node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/keypad-mapping-red.json --raw
```

- Task1: `ctest --preset ci -R '^input[.]directions$' --output-junit ../keypad-red.xml --output-on-failure`, exit8. The executed valid-keypad assertion received unsupported13 instead of0. No missing symbol, setup or discovery failure authorized GREEN.
- Task2: `ctest --preset ci -R '^diag[.]controllers$' --output-junit ../keypad-diagnostic-red.xml --output-on-failure`, exit8. The actual pre-expansion executable completed scenario1 with only its selector record, failing the required27-record mailbox contract. The existing motion/light/paddle scenarios passed first.
- Task3: `ctest --preset ci -R '^libretro[.]host[.]devices$' --output-junit ../keypad-mapping-red.xml --output-on-failure`, exit8. The actual module exposed9 rather than10 choices on both connectors. The real input fixture passed.

Electrical and host boundary expansions that were already satisfied by the
owning implementation were initially-green test coverage, not additional RED
claims. No unsupported/setup failure was relabeled behavioral evidence.

## Verification Evidence

All facts below are local, run URL **N/A-local**. No hosted acceptance is claimed.

- Task1 command and its post-commit tracer rerun exit0,30/30: `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(input[.]|riot[.])' --no-tests=error --output-on-failure`.
- Task2 command exits0,3/3: `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(diag[.]rebuild|policy[.]quality[.]controls|diag[.]controllers)$' --no-tests=error --output-on-failure`. This includes both required plan checks plus the actual numeric consumer.
- Focused final module/input check exits0,5/5: `cmake --build --preset ci && ctest --preset ci -R '^(input[.]directions|diag[.]controllers|libretro[.]host[.](devices|input))$' --output-on-failure` after the mixed-device test correction; the complete workflow also executes these cases.
- Final exact Task3 command exits0: `cmake --workflow --preset ci && cmake --build --preset ci --target package && ctest --preset ci-release --output-on-failure`. Complete CI648/648, workflow release6/6 and explicit package/release rerun6/6 pass. No inventory entry was added or removed.
- `cmake --workflow --preset hygiene` exits0,24/24, including formatting, analyzer, integer-only and tree hygiene. `git diff --check` exits0.
- `cmake -DROOT=. -DBUILD=build/ci -DOUT=build/phase08-09-quality.json -DSHA=$(git rev-parse HEAD) -DCONFIG=ci -DSOURCE=local -P tests/cmake/quality_evidence.cmake` exits0 at2815568302b6a7a04d6eb3c3fbea0b3c2751622a. The actual producer verifies648+6 exact inventories and measures all three archives.
- `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check evaluation-scope --plan 08-09 --commits-only --raw` exits0 and resolves all six reachable task commits.
- Independent Python assertions compare all pre-plan expected/manifest rows, the committed assembly source, changed-file existence and ancestor commits; exit0. `git diff c7554b6 -- tests/inventory tests/built/expected.txt` is empty.

## Package Observations

Actual completed-workflow macOS arm64 ZIPs, source2815568:

| Component | Bytes |
|---|---:|
| library | 79429 |
| libretro | 82858 |
| runner | 92800 |
| Total | 255087 |

The existing producer reports native/local/measured with exact source identity.
The retained08-08 local total250408 atf33ad71 is4679 bytes smaller. These
are actual local observations, not a candidate/base acceptance or six-native
claim. This invocation uses the existing local measurement path, hashes its
actual manifest and samples, and fabricates no comparison base. Actual admission
fault controls retain changed-trusted-row rejection. Final genuine event-base
and exact-source six-native evidence remains08-14. Package contents/notices,
firmware exclusion and platform-floor controls pass. DEC.61 keeps measurements
and integrity without restoring the retired cutoff or inventing a new quota.
No hosted matrix was dispatched solely for size.

## Deviations from Plan

1. **Rule2 — reuse existing private deadline state:** src/internal.h adds only held-key/node masks and the row-change notification; existing pot action/deadlines serve recovery. This avoids another timer owner while satisfying the finite graph contract. Committed ind55d597; exact bus and complete CI checks pass.
2. **Rule2 — preserve validation contracts as support grows:** tests/api/v6.c and tests/runner/tests.cmake move only keypad from the temporary unsupported set to implemented expectations. Frozen legacy declarations/refusal and all still-unsupported devices remain intact. Committed ind55d597; ABI/runner and full CI pass.
3. **Rule3 — execute the diagnostic before host expansion:** Task2 extends tests/diag/tests.cmake alongside its driver, so its RED and GREEN use real executable numbers before Task3 activates module comparison. Existing producer/join controls already exercise changed/duplicate analog rows and later trusted bases; they pass unchanged.
4. **Rule2 — mixed attachment reset:** the earliest reset deadline now considers both pots on both connectors, avoiding one attachment masking another's earlier crossing. The component power-on test uses real bus data reads at2864; committed in2815568 and complete CI passes.
5. **Rule3 — retain documentation budget:** consolidate overlapping controller-test prose instead of increasing the40960-byte limit. The resulting emulation-tests page is40945 bytes; the actual19-file budget consumer and full CI pass. Hardware details remain in the controller reference.

## Issues Encountered

The first full workflow exited8,647/648 passed, because emulation-tests grew
to42223 bytes. A second full attempt also exited8 on that same concrete budget
fault at41123 bytes. Intermediate read-only checks reported41009,40965 and
then the passing40945 bytes. An initial standalone budget invocation with
relative ROOT checked only two files; it was not accepted as documentation
coverage. The absolute-root check and final actual CTest consumer verify all19
files. The final full648+6 workflow is the accepted run; failed attempts remain
failures, not flakes or passes.

Other development corrections: one broad edit initially changed the SWCHA
paddle branch rather than the intended INPT pot branch, so at/after assertions
stayed low until corrected. Auto-header tests initially still expected keypad's
temporary ProLine fallback; only new keypad expectations were updated. The
mixed-device test initially reused a cleared zero-size packet after public power
cycle, which deliberately clears host input; its next attempt began CPU reset
before an instruction could execute. It now tests component power-on through
actual bus accesses, with the intended selected attachments and locked decode.
Those setup failures were never RED evidence. No numeric expectation was repinned
to observed output.

No authentication gate, known stub, skipped test, unrun required verification,
new trust-boundary surface or remaining blocker exists. Analog recovery and
contention remain explicit unmeasured compatibility policies. No INPUT requirement
is complete. No owner verification is required.

## Next Plan Readiness

This completes08-09 only. The parent owns shared planning position and accepted
counts. The next dependency-ready plan is08-10; no other GSD step, PR, ship,
merge or release was invoked. Next separate command: `$gsd-execute-phase 8`.

## Self-Check: PASSED

All18 declared changed files exist. All six task commits are ancestors of HEAD
and resolve through evaluation-scope. The persisted ledger measures6 commits
fromc7554b6 through2815568. The realized task diff is71183 characters/4,
floored to17795 estimate-scale tokens. Complete648+6, explicit release6,
hygiene24, byte-exact rebuild, numeric consumers, actual producer and clean-tree
checks pass. No tracked deletion, unexpected untracked output, unrelated fixture
change or exact-inventory modification remains. Duration13min is measured from
the first persisted RED at00:28:34Z through summary closeout; context loading
before that marker is not separately timed.
