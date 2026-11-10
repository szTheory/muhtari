---
phase: 08-controllers-at-the-pins
plan: "08"
status: complete
subsystem: input
tags: [paddles, tia, libretro, diagnostics, tdd]
requires:
  - phase: 08-07
    provides: Proven native input timing and independent three-host controller consumer
provides:
  - Four full-range elapsed-time Q32 paddle charge channels and independent buttons
  - Reproducible owned19-record analog executable with actual admission controls
  - Absolute two-axis libretro mapping and numeric library/runner/module equality
affects: [08-09, 08-14]
tech-stack:
  added: []
  patterns:
    - Project finite charge crossings through the existing bus deadline owner
    - Materialize Q32 progress only on electrical or setting changes
key-files:
  created:
    - tests/diag/controller-analog.s
    - tests/diag/controller-analog.cfg
    - tests/diag/controller-analog.a78
  modified:
    - src/internal.h
    - src/ports.c
    - src/tia.c
    - tests/input/test_input.c
    - docs/hardware/controllers.md
    - tests/api/v6.c
    - tests/api/v6-layout.c
    - tests/runner/tests.cmake
    - tests/roms/manifest.txt
    - tests/diag/expected.csv
    - tests/diag/driver.c
    - tests/diag/tests.cmake
    - tests/policy/quality.cmake
    - libretro/libretro.c
    - tests/libretro/host.c
    - docs/libretro.md
key-decisions:
  - Keep charge crossings in the existing chronological bus scheduler because CPU data is sampled before its cycle counter advances.
  - Preserve frozen packet declarations and the old setter refusal while accepting newly implemented devices through the extended packet.
  - Use the existing numeric controller consumer and admission producer without adding dependencies or a new test inventory.
requirements-completed: []
requirements-contributed: [INPUT-01, INPUT-03]
coverage:
  - id: physical-range
    description: Both pots on both ports cross at exact endpoint and midpoint master ticks through actual CPU accesses.
    requirement: INPUT-03
    verification:
      - kind: integration
        ref: input.directions;1878/95750/95753/189625 ticks and before/at/after boundaries;exit0;N/A-local
        status: pass
    human_judgment: false
  - id: charge-history
    description: Mid-charge changes preserve Q32 history independently of polling and repeated dump/release writes.
    requirement: INPUT-03
    verification:
      - kind: integration
        ref: input.directions;1000-tick setting change plus88654 remaining ticks, sparse/frequent polling, retained high, held dump, JAM/HALT and reset/replacement;exit0;N/A-local
        status: pass
    human_judgment: false
  - id: validation-and-legacy
    description: Paddle range and payload faults retain pending input, and frozen legacy clients retain unsupported-device refusal.
    requirement: INPUT-01
    verification:
      - kind: integration
        ref: input.directions,abi.api_v3,abi.api_v6,abi.api_v6_layout,runner.movie.v2_edges;exit0;N/A-local
        status: pass
    human_judgment: false
  - id: owned-analog-oracle
    description: The19-record scenario0 executable judges independent full-range charge and both button pairs before equality.
    requirement: INPUT-03
    verification:
      - kind: integration
        ref: diag.controllers and diag.rebuild;identity16,scenario0,count,CRC and actual report fault controls;exit0;N/A-local
        status: pass
    human_judgment: false
  - id: absolute-host-input
    description: Both absolute axes and independent buttons are reachable on both connectors with ignored Y and separate console controls.
    requirement: INPUT-01
    verification:
      - kind: integration
        ref: libretro.host.input,libretro.host.devices and diag.controllers actual loaded-module endpoint/midpoint packets;EEPROM on either opposite port;exit0;N/A-local
        status: pass
    human_judgment: false
  - id: three-deliverables
    description: Library packets, actual immutable movie2 runner processes and loaded modules independently pass numbers and agree on native frames, raw transitions and exact PCM/counts.
    requirement: INPUT-03
    verification:
      - kind: integration
        ref: diag.controllers;complete12-frame schedules and report wrong/missing/duplicate/count/CRC/truncation controls;exit0;N/A-local
        status: pass
    human_judgment: false
  - id: admission-and-packages
    description: Actual analog fixture admission and later-base protection pass alongside complete CI, package integrity and authenticated local ZIP measurements.
    requirement: INPUT-03
    verification:
      - kind: integration
        ref: policy.quality.controls,648+6 workflow,hygiene24 and quality_evidence.cmake atf33ad71;exit0;N/A-local
        status: pass
    human_judgment: false
actuals:
  tokens: 16164
  tasks: 3
  commits: 6
plan_head_before: de040a71b687ba016173728002780f064ee9420f
plan_head_after: f33ad712c6710f8fa5d03853975101cb9141d40e
duration: 11min
started: 2026-10-25T00:14:49Z
completed: 2026-10-25
---

# Phase 8 Plan 8: Full-range timed paddles Summary

**Four elapsed-time Q32 paddle channels, an owned19-record analog executable and absolute libretro mappings pass independent numeric judgments through every deliverable.**

## Accomplishments

Each pot has finite action0..2^32, a duration, a last-change epoch and a
projected crossing tick. The model uses the documented7800 network:
10kΩ series plus floor(p*1000000/65535)Ω,68nF and the chosen unmeasured
threshold0.32, represented by coefficient187747/1000000. Ceil durations
are1878/95750/95753/189625 ticks at0/32767/32768/65535. Independent integer
arithmetic also reproduces action2286990040 after1000 ticks at0 and88654
remaining ticks after changing to65535. No host poll or repeated low
write introduces extra rounding. High remains high until dump; D7 clamps
immediately, independently of D6. Both pin5/pin9 channels and pin4/pin3
buttons are tested on both connectors, including payload refusal without
pending mutation, CPU slow data ticks, held dump, JAM/HALT, real frame
crossings, same-device input, replacement and power cycle.

The existing bus event owner delivers at most four charge crossings;
there is no per-tick analog iteration, allocation or floating point.
This is a fitted compatibility model, not measured component calibration.
The original network facts and numerical derivations are documented with
the implementation. The legacy24-byte setter still refuses paddles and
retains its automatic fallback; extended input accepts them.

The MIT controller-analog executable has mailbox identity16, scenario0,
nineteen records and the existing count/CRC/completion protocol. Its fixed
frame schedule exercises all four range endpoints/midpoints, two independent
button pairs and swapped endpoint positions. Finite WSYNC waits exceed the
physical189625-tick maximum rather than compressing it into a game polling
window. Exact±1 crossing checks remain in the independent CPU/bus cases.
Future keypad scenario1 retains its separate identity/count expectations;
it does not replace any paddle observation. Rebuild SHA-256 is
`afbc4e3c307b0075f7eee6ec3b15c9d46a3c13941f3d95fa9499be6ea96cc09d`.
The manifest source pind40839b contains the exact current assembly source,
independently compared byte for byte.

The ninth libretro choice is Paddle pair, selected with the ANALOG subclass.
Base ANALOG left/right X map by int32(v)+32768; independent loaded-module
vectors require -32768/-1/0/32767 →0/32767/32768/65535 exactly. Base JOYPAD
B/A reach separate buttons. Callback counts require exactly one poll and
one read of each X/button per frame, with no Y reads. Start/Select/X provide
console Pause/Select/Reset when paddles supply console input. Ordinary pads
keep priority; difficulty remains option-controlled. Both connectors work
with the opposite connector empty or occupied by the installed EEPROM.
Descriptors, Auto selection, unknown IDs and missing callbacks are covered.

The existing controller consumer independently judges every numeric mailbox
before checking native/public frames, actual raw audio transitions and exact
PCM/counts. The runner executes generated immutable movie2 bytes in actual
child processes. Module equality observes its real instance and native/audio
outputs. Five CRC-aware report faults plus missing/truncated reports retain
the existing concrete rejection contracts. New analog changed/duplicate
manifest faults run through the actual recursive producer/join both before
and after admission as a trusted base.

## Task Commits

1. Task1 RED `1203b24`, GREEN `eee4e4a`: physical charge model, exact bus tests and hardware docs.
2. Task2 RED `5a48997`, GREEN `d40839b`: owned executable, independent numeric records, actual admission and rebuild.
3. Task3 RED `48de638`, GREEN `f33ad71`: analog callbacks, actual module vectors, complete three-host observations and host docs. This also finishes boundary/transaction checks and updates the source pin to the exact completed diagnostic source.

No separate refactor commit was needed. All normal hooks ran with the
GitHub noreply identity. No tracked deletion or new dependency was introduced.

## TDD Gate Compliance

All three tasks executed meaningful behavioral RED before their owning
implementation, then GREEN. Reports were captured as unchanged JUnit from
actual commands, with actual run start/mtime, command, exit and semantic
assessment. Each classifier returned RED_EVIDENCE_OK before GREEN:

- Task1: `ctest --preset ci -R '^input[.]directions$' --output-junit ../paddle-charge-red.xml --output-on-failure`, exit8. A valid paddle packet was refused with status13 rather than0. The executed acceptance assertion proves the missing controller behavior; it was not a missing symbol or fixture error.
- Task2: `ctest --preset ci -R '^diag[.]controllers$' --output-junit ../analog-diagnostic-red.xml --output-on-failure`, exit8. The new actual executable passed scenario, dump and buttons, then record4 was0 instead of128 because its finite charge wait had not yet been implemented. Existing motion/light consumers passed.
- Task3: `ctest --preset ci -R '^libretro[.]host[.]devices$' --output-junit ../paddle-mapping-red.xml --output-on-failure`, exit8. The actual module exposed8 identities instead of the expected9 on both ports; its input cartridge fixture passed.

Classifier commands exit0, run URL N/A-local:

```sh
node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/paddle-charge-red.json --raw
node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/analog-diagnostic-red.json --raw
node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/paddle-mapping-red.json --raw
```

Ignored build reports are evidence inputs, not committed logs. The first
persisted RED run began00:14:49Z; duration11min is measured from that marker.
The earlier context-loading start was not separately captured.

## Verification Evidence

Every fact here is local; run URL **N/A-local**. No hosted acceptance is claimed.

- Task1 exact automated command and post-commit tracer rerun exit0,17/17: `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^input[.]' --no-tests=error --output-on-failure`.
- Task2 exact automated command exits0,2/2: `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(diag[.]rebuild|policy[.]quality[.]controls)$' --no-tests=error --output-on-failure`. An additional combined numeric/rebuild/admission rerun exits0,3/3.
- Final Task3 exact command exits0: `cmake --workflow --preset ci && cmake --build --preset ci --target package && ctest --preset ci-release --output-on-failure`. Complete CI648/648, workflow release6/6 and explicit package/release rerun6/6. Existing exact inventories are unchanged.
- `cmake --workflow --preset hygiene` exits0,24/24, including formatting, analyzer, integer-only and fixture hygiene. Final `sh scripts/hygiene.sh --tree` and `git diff --check` exit0.
- `cmake -DROOT=. -DBUILD=build/ci -DOUT=build/phase08-08-quality.json -DSHA=$(git rev-parse HEAD) -DCONFIG=ci -DSOURCE=local -P tests/cmake/quality_evidence.cmake` exits0 atf33ad712c6710f8fa5d03853975101cb9141d40e. Its actual consumer judges exact648+6 inventories and archive measurements.
- `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check evaluation-scope --plan 08-08 --commits-only --raw` exits0 and resolves all six reachable task commits.
- Independent Python integer assertions reproduce all four durations and the mid-charge vector, compare the source pin exactly and prove all old expected/manifest rows are preserved. `git diff de040a7 HEAD -- tests/inventory tests/built/expected.txt` is empty, and only the new analog executable differs among owned ROM fixtures. Assertions exit0.

The first complete CI execution exited8:647/648 passed, with only
abi.api_v6_layout failing its temporary extended-paddle refusal assertion.
Its frozen packet declarations and legacy refusal remain intact; the new
extended paddle acceptance expectation now passes with the old linked-client
ABI checks. The subsequent complete workflow passes; the failure is retained
here as a failure, not a flake or pass.

Other development failures were corrected before acceptance: the initial
projected read used the cycle-start counter rather than the input data tick
(fixed by scheduling crossings in the existing owner); a test used a nonexistent
reset spelling (changed to the real public power-cycle API); the synthetic
timing test needed enough bounded host calls to reach its independent deadline;
and the analog schedule initially inherited driving's frame5 no-submission
case (restricted to its original families). Auto header assertions now judge
the newly supported paddle pins rather than temporary ProLine fallback.
No expected pot timing or old digest was repinned to observed output.

## Package Observations

Actual completed-workflow macOS arm64 ZIPs:

| Component | Bytes |
|---|---:|
| library | 77799 |
| libretro | 81122 |
| runner | 91487 |
| Total | 250408 |

The existing producer reports native/local/measured atf33ad71. The retained
08-07 observation is246732 at9ca62b3, a3676-byte total increase. These are
actual local measurements, not a six-native or retired-cutoff claim.
Package members/notices, firmware exclusion, platform floors and their fault
controls pass. DEC.61 preserves these contracts and measurements without a
replacement quota. No hosted matrix was dispatched solely for size; final
exact-source candidate/base six-native evidence remains assigned to08-14.

## Deviations from Plan

1. **Rule2 — preserve data-tick correctness:** src/tia.c notifies the port owner only on real D7 edges. Four projected charge crossings join the existing bus deadline owner so actual CPU data ticks, unpolled intervals and peeks stay correct. This is the existing ownership boundary, not a new scheduler.
2. **Rule3 — execute the numeric oracle before expansion:** Task2 also wires tests/diag/driver.c and tests/diag/tests.cmake ahead of Task3 so its RED is a real numeric assertion. Task3 then enables loaded-module participation and complete mapping checks.
3. **Rule2 — update temporary unsupported expectations:** tests/api/v6.c, tests/api/v6-layout.c and tests/runner/tests.cmake adapt extended-device capability checks to new paddle support. Frozen declarations and released legacy behavior remain unchanged. The layout mismatch was exposed by the first full CI run and verified by both focused ABI and complete reruns.
4. **Rule2 — actual-row admission faults:** tests/policy/quality.cmake extends existing actual producer/join controls to analog changed/duplicate rows, including later trusted bases. All prior controls remain.

No authentication gate, known stub, skipped test, unrun required verification
or new security-relevant surface remains. Hardware comparator/dump calibration
is explicitly unmeasured as planned. No INPUT requirement is complete.

## Next Plan Readiness

This completes08-08 only. The parent owns STATE.md, ROADMAP.md and
REQUIREMENTS.md; this executor did not edit, stage or commit them. The next
dependency-ready slice is08-09. No other GSD step, PR, ship, merge or release
was invoked. Next separate command: `$gsd-execute-phase 8`.

## Self-Check: PASSED

All19 declared created/modified files exist. All six task commits are ancestors
of HEAD and resolved by evaluation-scope. The on-disk ledger measures6 commits
fromde040a7 throughf33ad71. The realized nonplanning diff is64659 characters/4,
floored to16164 estimate-scale tokens. No tracked deletion, unexpected untracked
output or change to existing exact inventories remains. Final complete648+6,
explicit release6, hygiene24, byte-exact rebuild, real numeric consumers,
actual producer and clean-tree checks exit0. No owner verification is required.
