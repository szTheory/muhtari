---
phase: 08-controllers-at-the-pins
plan: "06"
status: complete
subsystem: input
tags: [tia, maria, lightgun, deterministic, tdd]
requires:
  - phase: 08-05
    provides: Accepted extended-input ownership, chronological controller deadlines and package evidence contract
provides:
  - Shared TIA D6 low memory and D7 pot clamp for legacy and extended hosts
  - Historical native MARIA light samples and bounded delayed XG-1 sensor pulses on both ports
  - Behavior revision5 with retained historical digests and actual local package observations
affects: [08-07, 08-08, 08-09, 08-14]
tech-stack:
  added: []
  patterns:
    - Existing bus deadline owner merges controller sample and pin events
    - Pixel emission carries native color, raster and absolute tick independently of host buffers
key-files:
  created: []
  modified:
    - src/internal.h
    - src/tia.c
    - src/ports.c
    - src/riot.c
    - src/maria.c
    - src/bus.c
    - src/instance.c
    - tests/input/test_input.c
    - tests/input/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - tests/core/test_core.c
    - tests/audio/expected.txt
    - tests/built/expected.txt
    - tests/cmake/hashes.cmake
    - tests/diag/driver.c
    - tests/runner/tests.cmake
    - docs/hardware/controllers.md
    - docs/hardware/tia.md
    - docs/hardware/maria.md
    - docs/hardware/bus.md
key-decisions:
  - Capture electrical lows at pin changes; CPU reads and peeks remain observers.
  - Keep one scheduled native sample, one delayed response and one active interval per port under the existing deadline owner.
  - Retain all69 observation identities and digest/count values; update only seven named revision annotations from4 to5.
requirements-completed: []
requirements-contributed: [INPUT-01, INPUT-02, INPUT-03]
coverage:
  - id: tia-input-memory
    description: TIA owns all D6 enable/repeat/clear transitions and D7 pot clamping with two-port electrical mode changes and open bus preserved.
    requirement: INPUT-03
    verification:
      - kind: integration
        ref: input.proline_one_button and input.proline_two_button; real CPU LDA/STA and public legacy input; exit0; N/A-local
        status: pass
    human_judgment: false
  - id: emitted-light
    description: Native luma7 fails while luma8 emits the exact delayed30-tick pulse on both ports independent of output buffers and later palette changes.
    requirement: INPUT-02
    verification:
      - kind: integration
        ref: input.lightgun; independent sample+63,+64,+93,+94 CPU observations for A7,A8,08 with and without video; exit0; N/A-local
        status: pass
    human_judgment: false
  - id: unpolled-causality
    description: JAM and HALT preserve D6 capture, same-device aim updates preserve historical light, and replacement/reset clear device light.
    requirement: INPUT-02
    verification:
      - kind: integration
        ref: input.lightgun; unpolled pulse, simultaneous low-enable/disable, observer/lag and lifecycle controls; exit0; N/A-local
        status: pass
    human_judgment: false
  - id: gun-selection
    description: Extended selection and explicit load plus Auto activate the actual gun while immutable typed replay accepts its existing syntax.
    requirement: INPUT-01
    verification:
      - kind: integration
        ref: input.lightgun and runner.movie.v2_edges; both ports and actual runner; exit0; N/A-local
        status: pass
    human_judgment: false
  - id: behavior-identity
    description: Behavior5 retains ABI1/API6 and all historical69-row digests, numeric values and identities except seven named revision fields.
    requirement: INPUT-01
    verification:
      - kind: integration
        ref: core.info, audio.samples, diag.retail, runner.hashes_file and independent69-row comparison; exit0; N/A-local
        status: pass
    human_judgment: false
  - id: local-packages
    description: Complete local CI and the real three-component archive producer validate current core bytes and package integrity.
    requirement: INPUT-02
    verification:
      - kind: integration
        ref: cmake --workflow --preset ci;648+6 pass; quality_evidence.cmake reports245942 bytes at6fab3e0; exit0; N/A-local
        status: pass
    human_judgment: false
actuals:
  tokens: 11750
  tasks: 3
  commits: 6
plan_head_before: 77013a3b24a6609b36f38ecaee9d44f08c3eaa66
plan_head_after: 6fab3e0847b6f5b47128d34451986b63723c42b7
duration: 12min
started: 2026-10-24T23:42:50Z
completed: 2026-10-24
---

# Phase 8 Plan 6: Shared TIA input timing and native light Summary

**TIA remembers unpolled trigger lows and clamps pot inputs; native MARIA emissions schedule deterministic XG-1 pulses under behavior5.**

## Accomplishments

D6 captures an already-low pin, retains released lows across repeated enables and
reads, and clears/bypasses on disable. D7 grounds all four pot inputs, including
ProLine highs, without changing triggers. Packet, PB output/DDR and INPTCTRL
electrical changes notify TIA. Independent latches, open-bus bits, decode and the
independent audio lattice remain covered.

Each eligible raster samples exactly native aim x over the clipped y±4 aperture.
Native luminosity at least8 schedules [sample+64,sample+94). The original raster
and absolute tick survive rendering; NULL output computes only scheduled pixels.
The existing deadline merge runs sample and response events through CPU accesses,
JAM, HALT, RDY and cycle-completion overshoot. Fixed instance state composes
intervals without truncation; historical light survives palette/aim changes and
blanking. Device replacement, reload and power cycle clear device light.
The64/30/9/8 defaults remain unmeasured compatibility choices.

## Task Commits

1. Task1 RED `e621ac9`, GREEN `920de20`: CPU-visible TIA dump and low memory.
2. Task2 RED `3c0849d`, GREEN `2d934f6`: native emitted-light sensor path.
3. Task3 RED `d0d13d4`, GREEN `6fab3e0`: behavior identity, narrow metadata and docs.

No separate refactor commit was necessary. The final task includes formatting of
the changed C files with the repository's existing clang-format21.

## TDD Gate Compliance

Every task has a test commit before its implementation commit. Actual CTest JUnit
reports were supplied to `check tdd-red-evidence`; all three returned
RED_EVIDENCE_OK, exit0. Reports stay in ignored build output, never committed logs.

- Task1: `ctest --preset ci -R '^input[.]proline_(one|two)_button$' --output-junit ../input-latch-red.xml --output-on-failure` exited8. Released unpolled lows read128 instead of0; dumped ProLine highs read131 instead of3. Target CPU instructions executed. An earlier fixture experiment failed because VBLANK wrote an unlocked INPTCTRL; the test now locks the7800 decode first. That experiment was not accepted as RED evidence.
- Task2: `ctest --preset ci -R '^input[.]lightgun$' --output-junit ../lightgun-red.xml --output-on-failure` exited8. Public extended gun selection returned ERR_UNSUPPORTED_DEVICE13 instead of OK. The compiled target includes independent64/30 sensor assertions; GREEN executes them through real MARIA and CPU accesses. The first GREEN run exposed a test helper treating a fast MARIA write like a slow TIA access; correcting its16-versus17-tick data offset produced42/42 passes without changing sensor expectations.
- Task3: `ctest --preset ci -R '^core[.]info$' --output-junit ../revision-red.xml --output-on-failure` exited8. The successful public versions query returned4 instead of5, with ABI/API assertions passing. GREEN changes the shared identity to5.

Classifier commands, all exit0, run URL N/A-local:

```sh
node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/input-latch-red.json --raw
node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/lightgun-red.json --raw
node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/revision-red.json --raw
```

## Verification Evidence

All invocations in this plan are local, run URL **N/A-local**.

- Task1's exact automated command and its post-commit tracer rerun each exit0: `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(input[.]|audio[.]tia|riot[.])' --no-tests=error --output-on-failure`,29 selected tests. Full CI separately executes the actual TIA/audio suite.
- Task2's exact automated command exits0: `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(input[.]|maria[.]|frame[.])' --no-tests=error --output-on-failure`,42 fixture-inclusive tests.
- Final Task3 command exits0: `cmake --workflow --preset ci && cmake --build --preset ci --target package && ctest --preset ci-release --output-on-failure`. Complete CI648/648; workflow release6/6; explicit package/release rerun6/6. Exact inventories include the new input.lightgun case on native and WASM.
- `cmake --workflow --preset hygiene` exits0,24/24, including format, analyzer, integer-only and tree checks. `git diff --check` exits0.
- `cmake -DROOT=. -DBUILD=build/ci -DOUT=build/phase08-06-quality.json -DSHA=$(git rev-parse HEAD) -DCONFIG=ci -DSOURCE=local -P tests/cmake/quality_evidence.cmake` exits0 at source6fab3e0847b6f5b47128d34451986b63723c42b7. It independently judges exact648+6 inventories and records the real archive sum.
- `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check evaluation-scope --plan 08-06 --commits-only --raw` exits0, resolves all six reachable task commits.

The first complete workflow exited8 with four failures: audio.samples' eight
revision4 annotations, diag.retail's exact revision4 identity, runner.movie.v2_edges'
former unsupported-gun expectation, and runner.hashes_file's stale revision fault
injection. The second exited8 with the latter diagnostic/replay expectations still
unadjusted; the audio and malformed-revision consumers passed. These failures are
preserved, not characterized as flakes. The completed audit updated each named
metadata/support expectation, then the focused seven-test consumer run and final
complete workflow passed. No digest, mailbox number, timeout, inventory predicate
or fixture byte was repinned or relaxed.

The69-row comparison was executed both against the pre-plan local snapshot and
the retained accepted54485d6 native artifact. The following exact assertion exits0:

```sh
python3 -c 'import pathlib; a=pathlib.Path("build/battery-final-54485d6-equality/evidence/native/hashes.txt").read_text().splitlines(); b=pathlib.Path("build/ci/hashes.txt").read_text().splitlines(); assert len(a)==len(b)==69; d=[(x,y) for x,y in zip(a,b) if x!=y]; assert len(d)==7; assert all(x.endswith("audio_behaviour_revision 4") and y==x[:-1]+"5" for x,y in d)'
```

Only soundtest, pokey, activision, absolute, fractalus, battery.first and
battery.second audio_behaviour_revision values change4→5. The other62 rows and
all identity/digest/count/numeric data stay exact. All eight audio pin rows change
only their behavior column. The built sample table changes only its revision
comment. Fixture bytes remain unchanged.

## Package Observations

Actual macOS arm64 three-component ZIP sizes from the complete local workflow:

| Component | Bytes |
|---|---:|
| library | 76612 |
| libretro | 78775 |
| runner | 90555 |
| Total | 245942 |

The existing producer reports the same sum, source6fab3e0, native configuration,
local source and measured status. Archive members, notices, firmware exclusion
and floor controls pass. DEC.61 retires only the historical compressed cutoff;
this is a measurement, with no replacement quota. No hosted job was dispatched
for this intermediate report. Authenticated six-native candidate/base acceptance
remains assigned to08-14; local evidence does not claim cross-host acceptance.

## Deviations from Plan

1. **Rule2 — required electrical notifications:** changed src/riot.c and the
   INPTCTRL mutation seam in Task1. PB/DDR and mode writes can make the live trigger
   low; failing to notify TIA would violate the planned unpolled latch semantics.
   CPU PB transition tests and full CI verify the added notification.
2. **Rule2 — executable coverage registration:** added input.lightgun to the
   existing CMake registration and exact ci/wasm inventories instead of hiding the
   distinct device test under a ProLine case. Full CI and producer inventories pass.
3. **Rule3 — narrow identity consumers:** Task3 also updates tests/audio/expected.txt,
   tests/diag/driver.c and tests/runner/tests.cmake. Full CI reproduced their exact
   old revision/support assumptions. Only named revision fields and the now-supported
   gun case change; fault controls and unsupported-device refusal for remaining
   families continue through the actual consumers. tests/core/test_core.c supplies
   the meaningful public-query RED assertion required by Task3.

No dependency, generic event framework, new allocation, schema or public
calibration surface was added. No stub, skipped test or unrun required verification
was introduced. The existing signature-only firmware stub is unchanged.

## Next Plan Readiness

Ready for08-07's historical raster-boundary expansion, owned numeric light
diagnostic and actual libretro absolute-gun mapping. This summary completes only
08-06; no INPUT requirement or phase is marked complete. The orchestrator owns
STATE/ROADMAP/REQUIREMENTS and accepted counts. Its pending STATE edit was neither
staged nor committed here. No other GSD step, PR, merge or release was invoked.

Next separate execution command: `$gsd-execute-phase 8`.

## Self-Check: PASSED

All declared modified files exist; all six task commits are ancestors of HEAD.
Evaluation scope resolves the six commits. The persisted ledger measures6 commits
from77013a3 through6fab3e0; realized nonplanning diff is47000 characters/4=11750
estimate-scale tokens across21 files. No tracked file deletion or unexpected
untracked output remains. Complete CI648+6, hygiene24 and actual package producer
all exit0. Source documentation was checked against the implemented ownership
and independent vectors; no owner verification is required.
