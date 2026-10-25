---
phase: 09-pal-games-and-audio
plan: "07"
subsystem: testing
tags:
  - pal
  - diagnostic
  - audio
  - libretro
  - equality
requires:
  - phase: 09-pal-games-and-audio
    provides: Selected PAL execution, independent audio bounds and owned fixture admission through09-06
provides:
  - Source-reproducible owned PAL raster and sustained-source fixture
  - Independent startup plus600-full-frame numeric judgment through actual hosts
  - Additive PAL equality observations with preserved NTSC identities
affects:
  - 09-08
tech-stack:
  added: []
  patterns:
    - Stream one native PAL frame and one PCM buffer through existing diagnostic owners
    - Judge numerical intervals and source clocks before accepting equality observations
key-files:
  created:
    - tests/diag/pal.s
    - tests/diag/pal.cfg
    - tests/diag/pal.a78
    - docs/pal-tests.md
  modified:
    - tests/roms/manifest.txt
    - tests/diag/driver.c
    - tests/diag/expected.csv
    - tests/diag/tests.cmake
    - tests/libretro/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - tests/cmake/hashes.cmake
    - tests/policy/tests.cmake
    - tests/runner/tests.cmake
    - docs/emulation-tests.md
    - docs/testing.md
key-decisions:
  - Keep the partial fast startup separate from600 complete PAL hardware intervals and retain its carried remainder.
  - Use the existing test-only module seam in both pixel formats and bounded streaming captures rather than widening NTSC arrays.
  - Independently check stable TIA commits, final all-fast POKEY periods and nonzero PCM before hashes.
  - Preserve all342 old observation bytes and append614 numerically judged PAL observations through the existing writer and equality schema.
requirements-completed: []
coverage:
  - id: D1
    description: Owned PAL fixture rebuilds from source and passes exact manifest admission and producer/join pass and fault controls.
    requirement: PAL-01
    verification:
      - kind: integration
        ref: ctest --preset ci -R '^(diag[.]rebuild|policy[.]quality[.]controls)' --no-tests=error --output-on-failure
        status: pass
    human_judgment: false
  - id: D2
    description: Separately checked startup and600 full PAL intervals satisfy independent timing, sample carry, raster, mailbox and source-clock expectations.
    requirement: PAL-03
    verification:
      - kind: integration
        ref: tests/diag/driver.c#pal_case and diag.pal
        status: pass
    human_judgment: false
  - id: D3
    description: Actual runner and loaded XRGB8888 and RGB565 modules agree with the library on all601 native frames and complete raw and PCM output.
    requirement: PAL-02
    verification:
      - kind: integration
        ref: tests/diag/tests.cmake#DIAG_PAL_RUN and diag.pal
        status: pass
    human_judgment: false
  - id: D4
    description: Existing equality consumers retain342 old rows and require614 valid additive PAL rows while rejecting missing, duplicate, reordered and malformed reports.
    requirement: PAL-03
    verification:
      - kind: integration
        ref: runner.hashes_file and policy.lane_equality.controls
        status: pass
    human_judgment: false
actuals:
  tokens: 13197
  tasks: 3
  commits: 4
plan_head_before: 009f37ef1b9082c8825419735bfa050a89271f5d
plan_head_after: 7ad503ca23428ac69edd928fecdb8e595dd98022
duration: 15min
completed: 2026-10-25
status: complete
---

# Phase9 Plan07: Sustained owned PAL evidence Summary

**One source-rebuilt PAL fixture independently judges startup plus600 full frames, actual three-host native/raw/PCM output and additive equality observations.**

## Performance

- Recorded start:2026-10-25T18:39:47Z; completed2026-10-25.
- Tasks:3. Task files:16. Four task commits, measured from the persisted pre-plan ledger.
- Actual tokens use chars/4 over the realized binary-capable Git diff, not a harness token count.

## Accomplishments

The exact new owned manifest row names source37c428687dd499caf5de14826a554136c46b6372,
MIT, and SHA256676381441af4dd33568f6d4adf7b84fc602f42606b87dc888f29c582af74b216.
All old manifest bytes/order remain intact. The existing pinned assembler/header/signing
recipe rebuilds it. Actual admitted-row candidate and later-base producer/join controls
pass with source/license/digest/duplicate/forgery faults still rejected.

Mailbox identity19 has eight independently expected values:
128,0,0,128,128,128,600,0. It measures initial blank, visible start,
raster307 visible, raster308 blank, raster309 blank, wrap blank, finite
full-frame progress and final visible. The fast handoff is fitted policy,
not a hardware measurement. VBE16 loads the DLL and DMA17 feeds raster18;
native rows2..291 have independently specified$2F endpoints at x0 and319.
The driver also observes final DMA308 and absent DMA309. Existing core tests
remain the authority for exact single-tick edges and boot snapshots.

The driver checks fitted initial tick140016, deadline281976 and elapsed141960;
startup produces960 samples/remainder336960. Exactly600 subsequent142102-tick
intervals produce281x961+319x962=576919 samples. Combined601-call output is
577879/remainder47029 at tick85543176, next deadline85685278. The distinct
zero-initial-remainder arithmetic remains576918/remainder301218. This spans
over two228-frame TIA/frame-phase recurrences; no short-run setting changes it.

Ordered raw source0/1 changes independently satisfy absolute tick modulo456
equal64 or288,224/232 gaps and alternating0/15. Real POKEY advances with
completed CPU clocks, including slow IO; once the completed fixture runs
only fast JMP cycles, actual source2 gaps are11312 ticks (28x101x4), with
alternating0/15 and at least two complete measured gaps. Every full frame
has nonzero PCM. A read-only specialist identified the POKEY-frequency and
mute-check gaps; both were resolved in the implementation before acceptance.

One93440-byte native buffer and962-sample PCM buffer suffice. Actual601-call
runner output matches every native frame digest and the entire raw/PCM streams.
Both loaded test-only modules compare every native byte and stereo sample
each call; public PAL pixels and independently derived RGB565 truncation
pass. No new test symbol enters installed modules. WASM registration retains
the full library/runner work without modules. Its execution remains09-08.

The same numeric parser rejects CRC-valid wrong raster/progress numbers,
wrong protocol/count/CRC, malformed/missing/truncated/trailing fields and
wrong call/sample totals. A real600-call runner session, containing only599
full intervals, is rejected. Valid and invalid reports also pass through
the actual equality-writer function. Native/PPC equality controls require
complete PAL rows even when both inputs agree on a malformed artifact.

The existing writer appends614 PAL rows to the342 old rows,956 total.
The independently executed pre-PAL writer at009f37e reproduces all342 old
rows byte-for-byte. Their SHA256 is
a036c0f9da8037b0581496182024f0b42de455e08576712e0cc890dd949462b5;
the unchanged69-row guard and a new complete342-row guard both pass.
No production core behavior, NTSC predicate, API/ABI/behavior revision,
hosted lane authority, retired quota or test-card performance guard changed.

## Task Commits

1. Task1 source/config: `37c4286` — owned PAL raster and sustained audio stimulus.
2. Task1 reproducible fixture/admission: `7bae373` — exact source-pinned manifest row.
3. Task2 numeric and host judgment: `c7867f2` — startup/full-frame, raster, source-clock and actual-host evidence.
4. Task3 equality consumers/docs: `7ad503c` — additive observations, exact schemas, fault controls and bounded documentation.

Source/config precede the fixture's manifest pin so its source identity is a real
ancestor commit. Task2 implements the compact streaming host comparison in the same
existing driver; Task3 separately commits its equality-writer/consumer integration.

## Verification

Every accepted command below exits0; all run URLs are **N/A-local**. No log is committed.

- Task1: `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(diag[.]rebuild|policy[.]quality[.]controls)' --no-tests=error --output-on-failure` —3/3,9.39s; rebuild3.33s, rebuild fault0.07s, quality9.38s.
- Task2: `ctest --preset ci -R '^diag[.]pal$' --no-tests=error --output-on-failure` —1/1,5.39s. Complete expanded writer controls pass5.42s; coordinated consumer checks also pass.
- Working-source gate: `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" run-with-timeout 60 -- cmake --workflow --preset ci` —653 ordinary tests31.11s and6 release checks0.11s; PAL8.75s, hashes7.53s, quality13.08s.
- `cmake --preset ci && ctest --preset ci -R '^(diag[.]pal|docs[.]budgets|policy[.]lane_equality[.]controls)$' --no-tests=error --output-on-failure && cmake --build --preset ci --target package && ctest --preset ci-release --output-on-failure` —3/3 plus6/6, all three component ZIPs; focused5.93s/release0.11s.
- Exact committed source7ad503c: the full workflow command above —653 ordinary tests45.40s,6 release checks0.12s; PAL11.37s, hashes9.14s, quality17.60s. Complete ordinary/release JUnit was restored after focused runs.
- `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` and its release form with `build/ci/junit-release.xml`/`-DLABELS=release` —653 and6 tests exactly once.
- `git show 009f37e:tests/cmake/hashes.cmake` supplies the pre-PAL writer; its actual `cmake -DMODE=write -DRUN=build/ci/runner/muhtari-run -DDRIVER=build/ci/tests/muhtari-test-diag -DDIAG=tests/diag -DOUT=build/ci/pre-pal-hashes -DSOUNDTEST=build/ci/built/soundtest/soundtest.bas.a78 -DPOKEY=build/ci/built/pokey/pokey.bas.a78 -P <pre-PAL-writer>` plus Python standard-library byte assertions prove342 old rows unchanged and614 additive PAL rows.
- Python standard-library ZIP CRC checks pass all three current archives:81762+87472+96375=265609 bytes (library/libretro/runner). This is a local package observation, not an authenticated independent-base comparison.
- `git diff --check`, normal commit hooks and `check evaluation-scope --plan 09-07 --commits-only --milestone-bound --max-commits 30 --raw` pass. Tree hygiene is separately checked before final handoff.

The new PAL test's provisional18s timeout/COST9 was selected from the complete
8.75s full-CI workload, with the existing lane scale. The final more contended
11.37s run also passes. Hosted calibration belongs to09-08; no existing deadline
or workload was relaxed. The parent owns the subsequent exact-source/genuine
pre-plan recursive producer comparison; no result from that future action is
claimed here. Full hosted/current-base portability/package acceptance remains09-08.

## Deviations from Plan

**[Rule3 - Direct consumers] Coordinated exact-schema consumers.** The first full
workflow exposed the runner's old342-row count and the synthetic lane-equality
fixture's old schema. `tests/runner/tests.cmake` and `tests/policy/tests.cmake`
now account for additive PAL rows and retain all old checks. Representative PAL
missing/duplicate/order/numeric/digest/counter faults pass through the real
equality consumer. Both focused consumers and the final full workflow pass.

**[Rule3 - Documentation budget] Keep detailed PAL prose on a linked page.** The
existing emulation-test page was near its40960-byte limit. Detailed PAL derivations
now live in `docs/pal-tests.md`; the original page links there and `docs/testing.md`
accurately describes956 rows. Both original pages remain40926 bytes and budgets
pass. No budget changed.

## Failed History and TDD Classification

The initial exploratory fixture inherited WM1 before a proper VBE and initially
aligned progress one frame late. Explicit WM0 and enabling DMA in startup VBLANK
fixed the stimulus. The original bit-at-time CRC outlasted the final interval;
the PAL-local byte table computes the identical1021/FFFF protocol in time.
These source discoveries occurred before admitted evidence and are not RED passes.

An initial driver compile failed local variable shadowing in symbol loading;
scoped declarations corrected it. Early lower-row assertions incorrectly included
native row1; documented DMA17-to-raster18 ownership correctly starts at row2.
Those were test-oracle/build failures, not product RED. A truncation control
initially used `file(CONFIGURE)`, which silently restored its missing newline;
using `file(WRITE)` for empty/truncated faults restores the intended malformed
bytes. The real control then rejects them.

The first full workflow exits8 after28.16s with650/653 tests passing: docs.budgets,
policy.lane_equality.controls and runner.hashes_file fail for the direct-consumer
reasons above. The first doc split still left164/63 excess bytes on two pages;
concise linking/count prose fixes both without removing test coverage. All failure
history is retained here; no failed attempt is counted as a pass or a flake.

This plan adds test stimuli, judges and test orchestration; production emulator
behavior is unchanged and runtime TDD mode is false. No artificial RED/GREEN pair
or unsupported product RED claim is made. Existing quality admission has genuine
pass/fault controls, exercised with the actual newly admitted row.

The best-effort WINDOWS append encountered pre-existing ledger count drift
(frontmatter17/0/35/52 versus entries16/0/36/52) and wrote nothing. No repair of
that unrelated advisory metadata was attempted. No required check, stub, owner
verification or authentication gate remains for this local plan.

## Next Plan Readiness

09-07 local obligations are implemented and verified. PAL-01–03 remain open for
09-08 and final phase acceptance. Fast startup and analog85:342 balance remain
fitted/unverified qualifications; this diagnostic does not establish new hardware
calibration. The parent exclusively owns STATE/ROADMAP/REQUIREMENTS updates and
the ready-wave boundary. No later GSD step was invoked.

## Self-Check: PASSED

All16 task files exist and all four task commits are reachable ancestors of the
phase branch. No task deletion or generated/unexpected untracked file remains.
Exact committed-source653+6, full inventories, actual host/fixture/equality
consumers and current ZIP integrity pass. The measured task range starts009f37e
and ends7ad503c; the immediate summary commit contains only this artifact.
