---
phase: 08-controllers-at-the-pins
plan: "13"
status: complete
subsystem: input
tags: [isolation, allocation, properties, fuzz, operating-cost]
requires:
  - phase: 08-12
    provides: All controller families and their library, runner and module diagnostics
provides:
  - Live all-family transactional, observer, allocation and frozen-storage coverage
  - Real typed property and fuzz workloads retaining earlier cases and inventories
  - Comparable local runtime, event and replay allocation observations
affects: [08-14]
tech-stack:
  added: []
  patterns:
    - Fail-after-load allocator and untouched execution controls
    - Existing deterministic generators and fixed bounded pin traffic
key-files:
  created: []
  modified:
    - tests/core/test_core.c
    - tests/prop/test_prop.c
    - tests/fuzz/run.c
    - tests/fuzz/run_seeds.c
    - tests/fuzz/seeds.c
    - docs/embedding.md
    - docs/hardware/riot.md
    - docs/testing.md
    - docs/quality.md
key-decisions:
  - Keep all-family safety coverage in the existing core owner and seeded isolation property.
  - Treat initially green lifecycle coverage honestly as test expansion; RED identifies absent generated typed workload.
  - Distinguish the 33554432-byte movie content limit from the existing extra over-limit detection byte.
requirements-completed: []
requirements-contributed: [INPUT-01, INPUT-02, INPUT-03, INPUT-04, INPUT-05]
coverage:
  - id: runtime-ownership
    description: Nine actual families on both connectors preserve accepted pending and active history after refusals, with no allocation and frozen storage through reset and reload failures.
    requirement: INPUT-01
    verification:
      - kind: unit
        ref: core.no_alloc_after_load and retained core.load/input lifecycle cases; exit0; N/A-local
        status: pass
    human_judgment: false
  - id: live-light-and-charge
    description: Rejection and peek checks begin with a real active light pulse or explicit mid-charge and matrix recovery history, then compare future controls and output.
    requirement: INPUT-02
    verification:
      - kind: unit
        ref: core.no_alloc_after_load; actual bus/JAM light pulse and TIA/RIOT/ports snapshots; exit0; N/A-local
        status: pass
    human_judgment: false
  - id: typed-generated-work
    description: Existing property and fuzz consumers execute every actual extended family with nonzero typed fields, hostile requests, signed extrema and bounded pin traffic.
    requirement: INPUT-03
    verification:
      - kind: integration
        ref: prop.instances_isolated, fuzz.regress.run, fuzz.regress.loader and full ci at38ec251; exit0; N/A-local
        status: pass
    human_judgment: false
  - id: once-only-motion
    description: Independent generated assertions require fresh displacement to drain within a full frame and its phase to remain unchanged without another submission.
    requirement: INPUT-04
    verification:
      - kind: unit
        ref: prop.instances_isolated and retained input.pa7_edge timing vectors; exit0; N/A-local
        status: pass
    human_judgment: false
  - id: adapter-and-operating-cost
    description: Partial adapter scans survive refusal and observation; comparable local event, time and immutable replay allocation observations support the existing finite policies.
    requirement: INPUT-05
    verification:
      - kind: integration
        ref: core.no_alloc_after_load, retained input.directions, runner.movie.owner and one-time cost probes described below; exit0; N/A-local
        status: pass
    human_judgment: false
  - id: exact-source-local-evidence
    description: Complete native and release inventories, hygiene and the actual three-ZIP producer pass at the final task source without altering old authorities or fixtures.
    requirement: INPUT-01
    verification:
      - kind: integration
        ref: ci648+6, explicit package/release6, hygiene24, quality_evidence.cmake at38ec251; exit0; N/A-local
        status: pass
    human_judgment: false
actuals:
  tokens: 8647
  tasks: 3
  commits: 6
plan_head_before: e83d4782cba90a8a788c841b56d740ca7e60f917
plan_head_after: 38ec2514e29ba0cf13f7d4f90621ff49652762c2
duration: 15min
started: 2026-10-25T01:25:23Z
completed: 2026-10-25
---

# Phase 8 Plan 13: Hostile input, isolation and operating cost Summary

**Nine real controller families now participate in live refusal, allocation and storage checks, seeded typed isolation and bounded fuzz replay, with measured local event and replay costs.**

## Accomplishments

The existing core allocation test adds nine families times two connectors times
five layouts: no EEPROM, opposite SaveKey or AtariVox, and either identity with
HSC. Separate allocators are set to fail immediately after successful load.
Each pair receives real nonzero typed input and effective bus traffic, then only
one receives invalid size, switch/button masks, flags, pot range, inapplicable
motion, unknown device and forbidden storage changes. Both pending and active
state survive. Three subsequent frames compare complete video, PCM/count, CPU,
RAM, port history and all six TIA input levels against the untouched control.

The preconditions are asserted: gun light is actually low after bright native
emission through the bus under JAM; explicit D7 dump/release establishes capacitor
and matrix recovery; adapter position1 establishes partial scans; motion residual
is live. Repeated peeks preserve ports, RIOT, TIA and ticks, including lag/PA7.
Reset retains battery pointer, length, layout, generation and every marker byte.
Deliberate no-memory reload retains the old machine; successful reload establishes
erased bytes and generation0. Counts prove no runtime allocation or free before
that deliberate reload. Existing tests continue to establish smaller/zero tails,
Auto/override, replacement cleanup, due/unfinished EEPROM programming, console
Reset, chronological PA7 and effective adapter edges. Documentation now also
states autonomous motion/PA7, row/adapter data-tick behavior and the allocation
contract; the existing single-threaded per-instance contract remains explicit.

The original isolation property still runs its complete legacy generated workload.
It additionally replays two typed frames in interleaved and isolated order on
different device pairs. Fields include luma-window aims/off-screen state, pot
endpoints, multi-key masks, adapter buttons and signed minimum/maximum/zero/unit
motion. Pending replacement, invalid second-port flags and every-address pure
peeks precede real ORA/DDRA/VBLANK traffic. Full frame/state equality and allocation
counts are checked. An independent assertion requires residual/deadline exhaustion
after the full frame and unchanged phase on the next frame without submission,
so two equally wrong instances cannot silently repeat motion. Executed device
coverage must include all nine families in the seeded CI run.

The run fuzzer preserves every earlier cartridge's first two frames and board
traffic, then reloads for two additional typed frames, optionally with an opposite
EEPROM on either connector. Typed nonzero input, extreme deltas, real pin writes,
malformed transaction retention, peeks, finite frame/audio/residual bounds,
output/state equality, published battery identity and fail-after-load counts run
through the existing consumer. Nine named seeds are appended to each existing
loader/run corpus:40 loader and38 run seeds. Original names/builders, empty input
and regression inputs remain. Existing CI, sanitizer, libFuzzer and nightly
registrations consume these same owners; no registration or inventory changes,
new framework, package or source module were needed.

## Task Commits

1. Task1: `c3a8461` adds all-family lifecycle/control coverage and docs;
   `28187d3` explicitly establishes live pulse, capacitor and partial-scan history.
2. Task2: RED `f0e3c5e`, GREEN `905d7fe`; `38ec251` adds the independent generated
   drain/once-only assertion after ownership review.
3. Task3: `3d7790e` documents finite operating-cost rationale after the actual probes.

All six task commits are reachable. The persisted ledger measures six commits
from e83d478 through38ec251, excluding this metadata commit. Normal hooks and
GitHub noreply identity were retained; supplied-root and protected-branch guards
ran before writes/commits. The parent exclusively owns STATE.md, ROADMAP.md and
REQUIREMENTS.md; none was edited, staged or committed by this executor.

## TDD Gate Compliance

Task1's new behavioral checks initially passed against the existing implementation.
This is honest test expansion, not a RED claim or manufactured production defect.
Review strengthened its active-history setup before acceptance. No production
behavior change was needed.

For Task2, before generator implementation,
`ctest --preset ci -R '^prop[.]instances_isolated$' --output-on-failure --output-junit ../controller-properties-red.xml`
exited8: the target executed100 legacy cases but observed typed coverage0 rather
than511. The complete unchanged JUnit report records the actual assertion. This
is missing planned generated workload, not a core device defect or configure,
missing-file or build failure. `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/controller-properties-red.json --raw`
returned RED_EVIDENCE_OK, exit0, before GREEN. The record's start timestamp was
normalized from the actual JUnit local timestamp to UTC; report bytes were not
edited. GREEN then executed all nine actual families. The later once-only assertion
started green and is recorded as coverage expansion. No refactor was necessary.
All evidence here is local, run URL **N/A-local**.

## Verification Evidence

Commands below exit0, run URL **N/A-local**, except the named RED above:

- Task1 focused `cmake --build --preset ci --target muhtari-test-core && ctest --preset ci -R '^core[.]no_alloc_after_load$' --output-on-failure`; initial and strengthened live-history checks pass.
- Task1 full `cmake --workflow --preset ci` and post-commit tracer rerun both pass648+6 before generator expansion.
- Task2 focused build of muhtari-test-prop/muhtari-fuzz-run-replay and `ctest --preset ci -R '^(fuzz[.]regress[.]run|prop[.]instances_isolated)$' --output-on-failure` pass, followed by full648+6 CI and24 hygiene.
- `MUHTARI_PROP_CASES=10000 MUHTARI_PROP_SEED=20261025 build/ci/tests/muhtari-test-prop instances_isolated build/ci/built` passes10000 cases and all three retained retail board controls at905d7fe. This run predates the final extra once-only assertion; final100-case CI executes that assertion. No current-source10000-case claim is made.
- At final task source38ec2514e29ba0cf13f7d4f90621ff49652762c2: `cmake --workflow --preset ci`,648/648 plus actual packages and release6/6; then explicit `cmake --build --preset ci --target package && ctest --preset ci-release --output-on-failure`,6/6.
- At the same final task source, `cmake --workflow --preset hygiene`,24/24, including formatter, analyzer, integer-only, tree and firmware controls. `git diff --check`,exit0.
- `cmake -DROOT=. -DBUILD=build/ci -DOUT=build/phase08-13-final-quality.json -DSHA=$(git rev-parse HEAD) -DCONFIG=ci -DSOURCE=local -P tests/cmake/quality_evidence.cmake`,exit0; real producer confirms exact648+6 inventories and source/environment/fixture identity.
- `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check evaluation-scope --plan 08-13 --commits-only --raw`,exit0, resolves all six task commits.
- Independent Python/Git checks find all nine changed files and six reachable commits, no added stubs/deletions, and no production, fixture, historic number/digest, inventory, quality-policy or performance-allowance changes from e83d478.

## Comparable Operating-Cost Observations

One-time probes use the existing library/internal bus and pure movie validator,
not a new recurring harness. The ignored probe source has SHA-256
e7a5da3df33356932f1c49883206f63947315681d60e8858f481379ee21c63ce.
Commands: `/usr/bin/cc -std=c17 -O3 -DNDEBUG -Isrc -Iinclude -Irunner build/phase08-controller-cost.c runner/movie.c build/ci/libmuhtari.a -o build/phase08-controller-cost`,
then `build/phase08-controller-cost`; both exit0. Base source is independently
extracted with `git archive e83d478`, built through an embedding CMake wrapper
with Release, /usr/bin/cc and deployment target10.13, and linked into the same
probe against its own movie.c/library; build and execution exit0. Apple Clang21,
macOS arm64, fixture bytes, optimization and workload are comparable. Candidate
production sources are byte-identical to this slice base. This is a plan-local
comparison, not phase-versus-main acceptance.

A Python subprocess loop executes seven candidate/base pairs, alternating their
order each iteration, checks every exit0 and all six1000-frame records, and takes
the median CPU microseconds. Each workload begins after one warm handoff frame;
each1000-frame run advances119401992 actual ticks (the logical total119402000
differs by instruction-boundary overshoot at its endpoints). Keypad changes use
1000 packets and32000 additional ORA/DDRA writes. All runtime allocator counts
are0.

| Workload,1000 frames | Candidate CPU µs | Base CPU µs |
|---|---:|---:|
| neutral | 234453 | 238319 |
| ordinary digital | 233866 | 233450 |
| saturated four-axis motion | 238009 | 238531 |
| gun with caller video rows | 233919 | 226460 |
| gun without caller video rows | 207978 | 204751 |
| repeated keypad network changes | 231957 | 232205 |

Whole-probe median wall times are1491754791/1472112833ns. Child-process RSS
high-water across the paired measurements is35389440bytes; it includes the
maximum movie construction and is not the retained replay allocation. Timing
variation on identical production sources is observation, not a speedup/regression
verdict. No instruction threshold or advisory promotion follows from this table.

Actual bus/JAM stepping observes255 transitions on each of four axes,1020 total,
minimum spacing454 and final scheduled transitions115770ticks after submission.
The complete119404-tick stepped window records no CPU input polling. The derived
full-frame ceiling is263 per axis,1052 across four axes, from119402/454; it is not
falsely reported as an observed1020-transition workload. Residuals stay within±255.
Both video-row settings independently observe9 qualified gun samples and9 low
pulses per port over119404ticks, with no CPU polling. Matrix closure retains its
fixed maximum seven passes over seven nodes; no new global cost quota is added.

The pure maximum-file probe validates33554432bytes/399456 maximum-length rows,
then changes the final console byte to an invalid character and rejects at
line399461. One comparable candidate/base pair measures valid55703/58867CPUµs
and late-invalid52601/53687CPUµs. These are the same source/fixture/configuration
construction and are not claimed as a latency threshold.

The existing actual runner owner command
`build/ci/tests/muhtari-test-runner movie build/ci/tests/movie-owner build/ci/tests/input-carts/input.a78`
exits0. Its counting seam measures:

| Content bytes | Frames | Allocation calls | Live/peak requested bytes | Elapsed ns |
|---:|---:|---:|---:|---:|
| 33554432 | 399456 | 15 | 33554433 | 64035000 |
| 20000038 | 1000000 | 14 | 33554432 | 90025000 |

Cursor104bytes and packet88bytes are fixed native observations. There is no
per-frame packet array/allocation. The32MiB immutable content limit and existing
one-extra detection byte are distinct, as already established in08-04; this plan
does not silently tighten or hide that byte. One-over byte/frame refusals and all
growth allocation failures retain their actual owner checks and clean up to0live
bytes. Physical gun/motion/analog calibration remains unmeasured.

## Package Observations

Actual final-source macOS arm64 ZIPs, authenticated by the real producer:

| Component | Bytes |
|---|---:|
| library | 80625 |
| libretro | 85066 |
| runner | 93802 |
| Total | 259493 |

This equals the preceding08-12 local total. Contents, notices, firmware exclusion,
platform floor and exact inventories pass. No hosted matrix was dispatched solely
for sizes. Final six-native/event-base/hosted acceptance remains08-14. The retired
archive cutoff stays retired; the armed test-card +1% Ir guard and all independent
runtime, ABI, provenance and lane authorities remain unchanged. That guard measures
the test-card workload only.

## Ownership Review and Deviations

Review follows ENGINEERING§11. All changes stay in existing test owners and docs;
ports own controller history, the bus owns elapsed time/chronology, MARIA supplies
native light and RIOT/TIA retain latch ownership. No abstraction, mutable core
global, dependency, public interface or trust-boundary surface is added. Control
comparisons prove isolation/transactional behavior; earlier independent numerical
diagnostics remain the physical-model authority, avoiding circular physics claims.
Existing bounded recurring lanes consume the changes; comparable probes justify
the selected resource policies without imposing another threshold. No new review
or gap round is requested and existing finite limits remain.

Task1 consolidates its coverage in tests/core/test_core.c rather than duplicate the
existing input owner. Task2 appends coverage to existing registrations, preserving
all exact inventories. Initially green checks remain labeled as such. Task3 uses
ignored one-time probes and the existing runner allocator seam, as the plan directs;
no measurement logs or new tracked measurement tool are committed.

## Issues Encountered

The first explicit capacitor-precondition assertion failed20 checks because the
existing model correctly retains previously accumulated charge when resistance
changes. The test setup was corrected to issue an actual D7 dump/release; no
production defect or RED is claimed for this setup error. The original gun setup
was also replaced by actual bright emission through bus/JAM stepping, and adapter
setup now asserts partial position1. Fresh strengthened checks and full CI pass.

The first local ledger write was sandbox-denied; the normal Git metadata operation
succeeded with escalation and hooks. `/usr/bin/time -l` could not read restricted
kern.clockrate, so RSS observations use Python resource accounting instead. This
does not fabricate unavailable Linux Ir or individual-process RSS attribution.
No skipped test, unrun required verification, remaining blocker, known stub or
authentication gate remains. No owner UAT or hardware verification is requested.

## Next Plan Readiness

08-13 is complete;08-14 owns final exact-source hosted/cross-host acceptance.
All INPUT requirements remain pending that acceptance. The parent owns shared
state and the current execution step; this executor invokes no additional GSD
step, PR, ship, merge or release. The next separate resume command, if needed,
is `$gsd-execute-phase 8`.

## Self-Check: PASSED

All nine changed files exist and all six task commits are ancestors of HEAD.
The persisted ledger measures6commits frome83d478 through38ec251. The realized
diff is34590characters,8647estimate-scale tokens after flooring chars/4, excluding
this summary. Final-source648+6 CI/package, release6, hygiene24, actual producer
and the changed behavioral consumers pass. No tracked deletion, unexpected
untracked output, old fixture/pin/inventory change or new stub remains. Duration
15min is measured from01:25:23Z through closeout around01:40:28Z; context loading
before that recorded start is not separately timed.
