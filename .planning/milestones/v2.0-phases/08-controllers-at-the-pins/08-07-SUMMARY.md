---
phase: 08-controllers-at-the-pins
plan: "07"
status: complete
subsystem: input
tags: [lightgun, maria, libretro, diagnostics, tdd]
requires:
  - phase: 08-06
    provides: Native emitted-light scheduling and shared TIA input memory under behavior5
provides:
  - Independent raster/aperture/pulse boundaries through CPU accesses and successive host calls
  - Reproducible owned19-record XG-1 executable and actual admission controls
  - Absolute libretro gun mapping and independently judged library/runner/module output
affects: [08-08, 08-14]
tech-stack:
  added: []
  patterns:
    - Reuse the existing controller diagnostic schedule, numeric judge and native/raw/PCM observer
    - Poll base LIGHTGUN IDs and convert with current core geometry and bounded integer arithmetic
key-files:
  created:
    - tests/diag/controller-light.s
    - tests/diag/controller-light.cfg
    - tests/diag/controller-light.a78
  modified:
    - tests/input/test_input.c
    - docs/hardware/controllers.md
    - docs/hardware/maria.md
    - tests/diag/expected.csv
    - tests/roms/manifest.txt
    - tests/diag/driver.c
    - tests/diag/tests.cmake
    - tests/policy/quality.cmake
    - libretro/libretro.c
    - tests/libretro/host.c
    - docs/libretro.md
key-decisions:
  - Preserve already-correct production timing; initially-green boundary tests require no artificial implementation change.
  - Keep exact tick-boundary judgments in the CPU/bus tests and executable mailbox judgments at the actual instruction observation ticks.
  - Prefer an ordinary pad for console controls, then the lowest numbered active driving/gun port with device-specific nonconflicting controls.
requirements-completed: []
requirements-contributed: [INPUT-01, INPUT-02]
coverage:
  - id: historical-raster
    description: Both ports retain x0/x100/x319 emissions, clipped apertures and exact delayed pulse boundaries with video present or absent.
    requirement: INPUT-02
    verification:
      - kind: integration
        ref: input.lightgun; CPU observations at sample+63/64/93/94 for independently derived raster vectors; exit0; N/A-local
        status: pass
    human_judgment: false
  - id: host-frame-causality
    description: The last-visible x319 pulse survives VBLANK return, a same-device offscreen packet and a following unpolled JAM host call.
    requirement: INPUT-02
    verification:
      - kind: integration
        ref: input.lightgun; both ports and NULL/video outputs, D6 enable before response, first sensor read after the next call; exit0; N/A-local
        status: pass
    human_judgment: false
  - id: owned-light-oracle
    description: The19-record owned executable independently judges threshold7/8, actual BACKGRND and DMA pixels, trigger/offscreen separation and retained light.
    requirement: INPUT-02
    verification:
      - kind: integration
        ref: diag.controllers and diag.rebuild; exact fixture reconstruction, mailbox numbers, identity/count/CRC and report fault controls; exit0; N/A-local
        status: pass
    human_judgment: false
  - id: absolute-host-mapping
    description: XG-1 selection, absolute endpoints/center, both sentinel axes, reload, once/frame polling and nonconflicting console controls work through actual modules.
    requirement: INPUT-01
    verification:
      - kind: integration
        ref: libretro.host.input, libretro.host.devices and diag.controllers loaded-module packet vectors; exit0; N/A-local
        status: pass
    human_judgment: false
  - id: three-deliverables
    description: Library, actual movie2 runner and loaded libretro module independently pass light numbers and agree on native frames, raw transitions and exact PCM/counts.
    requirement: INPUT-02
    verification:
      - kind: integration
        ref: diag.controllers; both connectors, complete12-frame schedule, actual child reports and test-only native/raw observers; exit0; N/A-local
        status: pass
    human_judgment: false
  - id: admission-and-packages
    description: The real new fixture row passes scoped producer/join controls while mutations and duplicates fail; complete local CI and actual three-ZIP evidence pass.
    requirement: INPUT-02
    verification:
      - kind: integration
        ref: policy.quality.controls, complete648+6 workflow, hygiene24 and quality_evidence.cmake at9ca62b3; exit0; N/A-local
        status: pass
    human_judgment: false
actuals:
  tokens: 13649
  tasks: 3
  commits: 5
plan_head_before: a0f38562fa94490f8697427cbf35c11da2d5cd44
plan_head_after: 9ca62b39b30fc1ff607bca87aa9d1ac4bab3a841
duration: 14min
started: 2026-10-24T23:58:04Z
completed: 2026-10-25
---

# Phase 8 Plan 7: Historical XG-1 light and absolute hosts Summary

**Independent raster-edge tests and an owned19-record emitted-light executable prove the unmeasured XG-1 model through library, typed runner replay and absolute libretro input.**

## Accomplishments

Native raster100 x319 emits at its origin+454 and responds at518..547.
Last native row241/raster257 x319 emits at raster258 tick0; VBLANK returns
at42 before the64..93 response. Tests enable D6 before that response,
submit a same-device offscreen packet and execute the following real host
call under JAM without reading the sensor until the pulse has completed.
Both ports and NULL/non-NULL outputs retain the low. The larger table
independently exercises x0/100/319, aperture inclusions/exclusions, first
and last native rows and sample+63/64/93/94 observations. Existing HALT,
same-tick access ordering, darkness/native register rewrites, lifecycle
and observer controls remain active. No owning core correction was needed.

The MIT controller-light fixture is a genuine executed addition, identity15,
scenario0, nineteen sequential records and independently checked CRC. It
uses real DMA-off BACKGRND and DMA-on native palette pixels for luma7/8,
both triggers, explicit offscreen, repeated D6 and historical final-pixel
responses. Hardware/model distinction, arithmetic and the fixed host
schedule are published in controllers.md. The CPU mailbox routine returns
after the +64 final-pixel response; its record is consequently a low after
VBLANK, while the exact +42 return/pending interval is judged by the C tests.
No expectation is taken from a production conversion helper or output hash.

XG-1 is the eighth advertised libretro choice on both ports. Polling uses
base LIGHTGUN SCREEN_X/Y, TRIGGER, IS_OFFSCREEN and RELOAD exactly once per
frame. Valid values use floor((v+32767)*(dimension-1)/65534) over current
core geometry with a uint64 product. Independent loaded-module vectors
require320-wide -32767/0/32767 to map0/159/319 and242-high center to120;
either -32768 axis is offscreen with neutral0/0 before arithmetic. Reload
holds the trigger and suppresses illumination. START/SELECT/AUX_A provide
Pause/Select/Reset when a gun supplies console controls; ordinary pads keep
priority and difficulty options remain available. Unknown IDs safely unplug
and descriptors refresh. Missing callbacks leave guns offscreen.

The existing actual controller consumer now runs driving and light schedules.
It judges independent mailbox numbers before comparing every native frame,
public converted pixels, actual raw audio transitions and exact PCM/counts.
The real runner consumes immutable movie2 bytes with fixed gun identities.
CRC-valid wrong values, missing/duplicate record IDs, wrong count, bad
protocol/CRC, missing files and truncated reports fail the same report parser.

## Task Commits

1. Task1 `83580ba`: historical raster, aperture and host-frame tests plus derivations.
2. Task2 RED `b414c30`, GREEN `3b31be6`: owned executable, numeric judge/replay wiring, reproducible fixture and actual producer admission controls.
3. Task3 RED `4ff267c`, GREEN `9ca62b3`: absolute gun selection/mapping, loaded-module vectors, three-host evidence and user docs. The final commit also strengthens the no-read successive-host-call case and pins the final fixture source commit.

No separate refactor commit was needed. Changed C files use the existing
clang-format21. No dependency, generic scheduler, allocation path, public
calibration selector or new hosted RetroArch case was added.

## TDD Gate Compliance

Task1's first meaningful last-row/x319 host-return test passed on08-06.
Investigation and all added edge vectors likewise passed. This task became
test/doc-only coverage of existing behavior, with no GREEN production expansion
and no invented failing test. The initially-green evidence is explicitly
retained; it is not claimed as RED.

Task2 executed `ctest --preset ci -R '^diag[.]controllers$' --output-junit ../light-diagnostic-red.xml --output-on-failure`, exit8. The owned image passed
scenario0, both dark sensor values128 and left-trigger254, then failed the
planned bright-light record5: actual128 versus independently expected0. The
implementation then emits native BACKGRND8 and completes the numeric cases.
The first assembly setup attempt failed with an out-of-range branch; it was
fixed before executing the behavioral test and was never accepted as RED.

Task3 executed `ctest --preset ci -R '^libretro[.]host[.]devices$' --output-junit ../gun-mapping-red.xml --output-on-failure`, exit8. The real loaded module
reported7 identities instead of the expected8 on both ports; the input-cart
fixture passed. GREEN implements and executes the gun mapping.

Both unchanged JUnit reports are in ignored build output, with report paths,
actual report timestamps/mtime and semantic assessments. Each record was
classified before GREEN as RED_EVIDENCE_OK; classifiers were rechecked after
completion. Commands, exit0, run URL N/A-local:

```sh
node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/light-diagnostic-red.json --raw
node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/gun-mapping-red.json --raw
```

## Verification Evidence

All evidence here is local: run URL **N/A-local**. No hosted acceptance is claimed.

- Task1 exact automated command and its post-commit tracer rerun exit0,42 tests: `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(input[.]|maria[.]|frame[.])' --no-tests=error --output-on-failure`.
- Task2 exact command exits0,2 tests: `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(diag[.]rebuild|policy[.]quality[.]controls)$' --no-tests=error --output-on-failure`. The final source fixture rebuild is byte-exact, SHA-256 `e8fe9e0a2e7e73e8cc888e279383b433dec1a809991f83af5c4edc6b65460f68`; its manifest source3b31be6 contains the exact current assembly source, independently compared by SHA-256.
- Both complete Task3 executions exit0: `cmake --workflow --preset ci && cmake --build --preset ci --target package && ctest --preset ci-release --output-on-failure`. Final complete CI648/648; workflow release6/6; explicit package/release rerun6/6. Exact inventories remain unchanged and include the expanded existing cases.
- `cmake --workflow --preset hygiene` exits0,24/24 on the final implementation; `sh scripts/hygiene.sh --tree` and `git diff --check` exit0. Format, analyzer, integer-only and fixture hygiene pass with normal hooks.
- `cmake -DROOT=. -DBUILD=build/ci -DOUT=build/phase08-07-quality.json -DSHA=$(git rev-parse HEAD) -DCONFIG=ci -DSOURCE=local -P tests/cmake/quality_evidence.cmake` exits0 at9ca62b39b30fc1ff607bca87aa9d1ac4bab3a841, independently judging exact648+6 inventories and reporting the actual archives.
- `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check evaluation-scope --plan 08-07 --commits-only --raw` exits0 and resolves all five reachable task commits.
- Independent Python assertions against the ledger base preserve every old expected.csv row and manifest row, with no changed old fixture. The retained54485d6 artifact comparison retains all69 historical observation identities/digests/numbers, with only the seven already-accepted behavior4→5 annotation differences. These assertions exit0; no historical output was repinned.

Two pre-fix policy-control executions exited8 because the new
controller_changed_light fault expected the trusted-row rejection message.
The actual producer correctly rejected its bad new-row license as an unrelated
fixture. The control now requires that exact new-row rejection, while its
later-base counterpart requires changed-trusted-row rejection. Subsequent
focused and complete workflows pass. The failed executions remain recorded;
they are neither flakes nor passes. Existing motion controls were preserved;
four extra light changed/duplicate cases execute with both addition and
later-base trust. Valid additions and real recursive producer/join controls
use the actual admitted light row.

## Package Observations

Actual macOS arm64 ZIPs from the completed workflow and existing producer:

| Component | Bytes |
|---|---:|
| library | 76613 |
| libretro | 79564 |
| runner | 90555 |
| Total | 246732 |

The producer reports native/local/measured at source9ca62b3. The retained
accepted08-06 producer observation is245942 at6fab3e0, a790-byte total
difference. This is an actual local observation, not a six-native size claim
or a claim about the retired cutoff. Package members/notices, binary floors,
firmware exclusion and representative fault controls pass. DEC.61 preserves
these contracts and measurements without a replacement quota. No hosted job
was dispatched solely for intermediate size; final exact-source six-native
candidate/base and hosted acceptance remain assigned to08-14.

## Deviations from Plan

1. **Task1 existing behavior:** all independently added timing vectors began
   green on08-06, so no ports.c or maria.c change was justified. The task lands
   tests/docs only and explicitly records its lack of behavioral RED.
2. **Rule3 — actual numeric test seam:** Task2 also changes tests/diag/driver.c
   and tests/diag/tests.cmake, moving the necessary library/runner light judge
   wiring ahead of Task3 so its RED is an executed numeric assertion. Task3
   then activates the actual module and complete absolute mappings.
3. **Rule2 — admission fault coverage:** tests/policy/quality.cmake adds actual
   light-row changed/duplicate controls before and after base admission, while
   retaining existing controls. A new-row versus trusted-row failure-message
   expectation was corrected without changing either production predicate.

No authentication gate, stub, skipped test, unrun required verification or
new security-relevant surface remains. Calibration remains explicitly
unmeasured as planned; it is not an owner prerequisite or a calibrated retail
accuracy claim. No INPUT requirement is complete yet.

## Next Plan Readiness

This completes08-07 only. The parent owns STATE.md, ROADMAP.md and
REQUIREMENTS.md; this executor did not edit, stage or commit those files.
The next dependency-ready slice is08-08. No other GSD step, PR, ship, merge
or release was invoked. Next separate command: `$gsd-execute-phase 8`.

## Self-Check: PASSED

All14 declared created/modified files exist; all five task commits are
ancestors of HEAD and evaluation-scope resolves them. The on-disk ledger
measures5 commits froma0f3856 through9ca62b3. The realized nonplanning diff is
54596 characters/4=13649 estimate-scale tokens. No tracked deletion or
unexpected untracked output remains. Final complete CI648+6, explicit
release6, hygiene24, fixture rebuild, real numeric consumers and actual
package producer exit0. Documentation was checked against implemented
ownership, native geometry and independent arithmetic. No owner verification
is required.
