---
phase: 08-controllers-at-the-pins
plan: "14"
status: complete
subsystem: input
tags: [controller-equality, native-powerpc, quality, capabilities]
requires:
  - phase: 08-13
    provides: Complete controller implementations, diagnostics and bounded typed workloads
provides:
  - Independently judged controller observations in the real cross-platform equality producer
  - Qualified capability and host documentation preserving legacy behavior
  - Exact-source hosted acceptance and authenticated six-native package observations
affects: [phase-verification, shipping]
tech-stack:
  added: []
  patterns:
    - Reuse the diagnostic owner before accepting actual runner observations
    - Preserve independent accepted observations and existing lane authority
key-files:
  created: []
  modified:
    - tests/cmake/hashes.cmake
    - tests/diag/tests.cmake
    - tests/policy/tests.cmake
    - tests/runner/tests.cmake
    - tests/input/test_input.c
    - tests/fuzz/tests.cmake
    - tests/libretro/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - runner/main.c
    - README.md
    - docs/embedding.md
    - docs/testing.md
    - docs/libretro.md
    - docs/hardware/controllers.md
    - .planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md
key-decisions:
  - Judge complete actual controller reports before emitting equality rows; keep silent raw streams truthful.
  - Pin the retained69 against independent accepted source54485d6 with only seven named revision annotations.
  - Calibrate existing test deadlines from complete work; keep workloads, assertions and lane authority.
requirements-completed: []
requirements-contributed: [INPUT-01, INPUT-02, INPUT-03, INPUT-04, INPUT-05]
coverage:
  - id: controller-equality
    description: Thirteen fresh scenarios across all nine families and both connectors supply complete judged reports to342-row equality.
    requirement: INPUT-01
    verification:
      - kind: integration
        ref: "runner.hashes_file, diag.controllers, policy.lane_equality.controls; CI40153312357 and Nightly40153313963; exit0"
        status: pass
    human_judgment: false
  - id: light-model
    description: The actual light diagnostic and retained native emission and latch vectors pass every applicable final configuration.
    requirement: INPUT-02
    verification:
      - kind: integration
        ref: "input.lightgun and diag.controllers light0; six native lanes, ASan, WASM and full PowerPC; exit0"
        status: pass
    human_judgment: false
  - id: analog-matrix-models
    description: Independently judged analog0 and analog1 observations cover both pots and the keypad model before host agreement is accepted.
    requirement: INPUT-03
    verification:
      - kind: integration
        ref: "diag.controllers analog0/analog1 and libretro.host.input; exact-source CI and native/PowerPC artifacts; exit0"
        status: pass
    human_judgment: false
  - id: motion-models
    description: Driving, CX22, ST and Amiga observations retain their different connector sequences and chronological edge coverage.
    requirement: INPUT-04
    verification:
      - kind: integration
        ref: "diag.controllers motion0 through motion3, input.pa7_edge and seeded prop.instances_isolated; exit0"
        status: pass
    human_judgment: false
  - id: adapter-models
    description: All six fresh adapter groups retain complete serial and pair records on both ports across actual library, runner and module consumers.
    requirement: INPUT-05
    verification:
      - kind: integration
        ref: "diag.controllers motion4a through motion5c and libretro.host.input; final649-test native and PowerPC inventories; exit0"
        status: pass
    human_judgment: false
  - id: qualified-public-claims
    description: Help and documentation name executed support while preserving legacy refusals and explicitly unmeasured physical models.
    requirement: INPUT-01
    verification:
      - kind: integration
        ref: "runner.help, docs.budgets, libretro.info_claims and linked API clients;93 focused tests and final full CI; exit0"
        status: pass
    human_judgment: false
  - id: hosted-and-package-evidence
    description: Exact-source required CI, all nine nightly jobs, actual frontend cases and authenticated native package measurements pass without owner UAT.
    requirement: INPUT-01
    verification:
      - kind: e2e
        ref: "CI40153312357 attempt1; actual RetroArch DMA and fresh-process battery cases, complete media and pixel fault controls; exit0"
        status: pass
      - kind: integration
        ref: "Nightly40153313963 attempt1; downloaded lane_equality, exact JUnit,18 current ZIP inspections and eight-lane quality_join replay; exit0"
        status: pass
    human_judgment: false
actuals:
  tokens: 9366
  tasks: 3
  commits: 9
plan_head_before: 9931d86e0ff38178259bb4045eedb960e6f292b8
plan_head_after: df8f9a6510efc0338d7149dc35c36eb9cd23b378
duration: 66min
started: 2026-10-25T01:45:07Z
completed: 2026-10-25
---

# Phase 8 Plan 14: Final controller evidence Summary

**Complete controller reports now reach342-row equality after independent numeric judgment, with genuine six-native package observations and exact-source frontend, WASM and PowerPC acceptance.**

## Source and acceptance

Tested source is **df8f9a6510efc0338d7149dc35c36eb9cd23b378**. Its genuine first parent is
**81eba2726536ff7dbf4858ef0528305e2cdd82cc**; every native quality producer independently
rebuilt that Git object. No documentation fence manufactured this parent.

- [CI40153312357](https://github.com/szTheory/muhtari/actions/runs/40153312357), attempt1: completed/success. All20 actual jobs are terminal;19 succeed and the PR-only title job is skipped as expected. The existing **CI required** aggregate succeeds and remains authoritative.
- [Nightly40153313963](https://github.com/szTheory/muhtari/actions/runs/40153313963), attempt1: completed/success, all9 actual jobs succeed, including the advisory property lane.

All INPUT requirements remain pending the parent's independent phase review and acceptance.
This summary contributes evidence; it does not mark the phase or requirements complete.
STATE.md, ROADMAP.md and REQUIREMENTS.md were not edited, staged or committed by this executor.

## Accomplishments and task commits

Task1 reuses the existing diagnostic owner and all13 fresh scenarios rather than duplicating
stimulus definitions. Each actual runner report has12 ordered native hashes, bounded lag,
seven audio fields and one512-hex-digit mailbox. Exact field shape, lengths, version,
numeric records/counts/CRC and independent library agreement precede emission. These
diagnostics are silent: the raw stream is empty, its digest is present, and PCM bytes/counts
are still compared. Thirteen21-field reports add273 rows to the retained69.

Actual report and equality consumers reject missing, duplicate, reordered, blank,
leading-blank, malformed numeric, short-digest, wrong-revision, CRC, CRC-valid numeric
and wrong-family faults. Existing same-source, configuration, fixture and exact-member
lane guards remain intact. The old69 are pinned against downloaded accepted source
54485d630c421716d51a8d7919ded429742acf4a from
[CI40104749040](https://github.com/szTheory/muhtari/actions/runs/40104749040), not candidate goldens.
Only `audio_behaviour_revision` changes4→5 for soundtest, pokey, activision, absolute,
fractalus, battery.first and battery.second; every other old byte is identical.
The normalized retained digest is c4c9c310418422c30bbf32b204a157a564d343aadfabee4d5654b97695461761.

Task2 qualifies `--help`, corrects stale extended-device refusals and future-scenario prose,
and documents supported families, legacy behavior, console providers and physical-model limits.
README3399 bytes, header9737, embedding35111, testing40920, libretro17544 and controllers36957
all satisfy existing budgets. No calibration, universal playability, speech, state or PAL-complete
claim is added; the `.info` capability policy and experimental warning remain unchanged.

Task3 supplies the exact-source hosted and artifact acceptance below. Integration exposed
a GCC shadow diagnostic and undersized existing test deadlines, corrected without dropping
workloads, assertions, inventories or lane authority.

| Commit | Work |
|---|---|
| 429f27a | Task1 meaningful failing equality-row expectation |
| f98665b | Actual judged controller producer and consumer controls |
| 41ecb8c | Task2 meaningful failing qualified-help expectation and exact inventories |
| ffa069e | Qualified help and corrected host/model documentation |
| 751f488 | Reject blank controller reports and equality rows |
| d934f94 | Rename keypad row_mask to satisfy required GCC shadow checking |
| a5ffd6f | Reject blank retained reports and serialize the bounded fuzz regression |
| 81eba27 | Calibrate expanded host-input deadline from complete work |
| df8f9a6 | Calibrate fuzz regression deadline from successful serial execution |

All nine commits are reachable, use noreply identities and normal hooks, and delete no tracked file.

## TDD gate compliance

Existing family implementation and numeric vectors were already green; their reuse and
additional coverage are not claimed as a newly discovered core defect.

- Task1 `ctest --preset ci -R '^runner[.]hashes_file$' --output-on-failure --output-junit ../controller-hashes-red.xml` exited8: the actual producer emitted69 rows rather than the planned complete controller expansion. The initial329-row arithmetic omitted the real lag field; inspection established21 fields/report and the stricter final342-row schema. The complete unchanged JUnit and semantic assessment passed `check tdd-red-evidence build/controller-hashes-red.json --raw`, exit0, before GREEN. Both full648+6 workflow and tracer rerun then passed.
- Task2 `ctest --preset ci -R '^runner[.]help$' --output-on-failure --output-junit ../controller-help-red.xml` exited8 because the actual runner lacked the planned uncalibrated-model qualification. Existing names/formats were present. Its unchanged JUnit and semantic assessment passed `check tdd-red-evidence build/controller-help-red.json --raw`, exit0, before GREEN. The subsequent93-test focused command passed. An earlier inventory-sort configure error was corrected and is not RED evidence.
- Extra blank controller and retained-report faults each reproduced acceptance of malformed data before the parser corrections. These are real consumer failures, not fabricated reports. No refactor was needed.

All local evidence uses run URL **N/A-local**; RED artifacts remain ignored under build/.

## Verification commands and observed outcomes

At final source, these commands exit0, URL **N/A-local**:

- `cmake --workflow --preset ci`:649 normal tests plus6 release tests.
- `cmake --build --preset ci --target package && ctest --preset ci-release --output-on-failure`: exactly three ZIPs and6 release checks.
- `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake`:649 tests executed once each.
- `scripts/hygiene.sh --tree`, `cmake --workflow --preset hygiene`: tree clean and24 hygiene checks.
- `cmake -DROOT=. -DBUILD=build/ci -DOUT=build/controller-acceptance/local-quality.json -DSHA=$(git rev-parse HEAD) -DCONFIG=ci -DSOURCE=local -P tests/cmake/quality_evidence.cmake`: real producer,649+6 inventory and actual local ZIP sum259548 (library80624, libretro85066, runner93858).
- `cmake -DROOT="$(pwd)" -DWORK="$(pwd)/build/controller-lane-final" -P build/ci/policy/lane_equality_controls.cmake` and the standalone diagnostic compilation linking `runner/sha256.c`, followed by its actual motion0 mailbox judgment: exit0. Both supplied paths resolve to the absolute checkout paths actually used; the final workflow uses the same registration.
- `git diff --quiet 9931d86 HEAD -- src runner/movie.c libretro/libretro.c include/muhtari.h quality.json tests/perf/allow.txt tests/roms/manifest.txt tests/built/expected.txt tests/diag/expected.csv tests/diag/*.a78 tests/diag/*.s`, `git diff --check`, evaluation-scope and declared-file/commit checks: exit0.

Hosted commands use the two exact-source URLs above:

- `gh run view <id> --json attempt,status,conclusion,headSha,url,jobs`, exit0. `cmake -DINPUT=build/controller-acceptance/final-ci.json -DEXPECTED_SHA=df8f9a6510efc0338d7149dc35c36eb9cd23b378 -P tools/check_ci_run.cmake`, exit0, audits all20 jobs and the unique existing aggregate.
- The same CI-only audit on raw nightly JSON exits1 solely because nightly has no CI required job. The unmodified response separately passes exact source/attempt/status and all9 terminal-success assertions, exit0; no job or conclusion was synthesized to appease that CI-only tool.
- `gh run download 40153312357 ...` and `gh run download 40153313963 -D build/controller-acceptance/nightly-artifacts`, exit0; all inspected artifacts are actual downloads.
- `python3 build/controller-acceptance/inspect.py`, exit0:18 current ZIP CRC/member-notice checks, exact three component names per lane, actual sizes/sums, authentic raw base policy/SHA, every metric source identity, comparable environments, six-native/WASM342-row equality and independent retained69.
- `cmake -DDIRECTORY=build/controller-acceptance/quality-inputs -DOUT=build/controller-acceptance/quality-output -DPOLICY=quality.json -DBASE_POLICY=build/controller-acceptance/trusted-quality.json -DEXPECTED_SHA=df8f9a6510efc0338d7149dc35c36eb9cd23b378 -DBASE_SHA=81eba2726536ff7dbf4858ef0528305e2cdd82cc -DSOURCE=https://github.com/szTheory/muhtari/actions/runs/40153312357 -P tests/cmake/quality_join.cmake`, exit0. The trusted policy comes from raw `git show` of the genuine parent.
- `cmake -DNATIVE=build/controller-acceptance/nightly-artifacts/lane-equality/evidence/native -DBE=build/controller-acceptance/nightly-artifacts/lane-equality/evidence/be -DEXPECTED_SHA=df8f9a6510efc0338d7149dc35c36eb9cd23b378 -P tests/cmake/lane_equality.cmake`, exit0. Four independent `check_junit.cmake` calls on the downloaded native/PPC normal/release XMLs each exit0:649+6 on each.

Actual final logs show all six native current/base649+6, ASan/UBSan632/632,
hygiene24/24,18 nofp checks on each Linux architecture, WASM587 and advisory library43.
Nightly executes258 full-vector checks,10 bounded fuzz checks, six properties at10000
cases with seed40153313963 plus their two fixtures, and four300000-frame benchmark lanes.
Required/advisory membership is unchanged. Dispatch test-card Ir is3838689,3838689,3838689,
spread0: same-head repeatability only, not phase-versus-main performance acceptance.
The armed PR +1% guard remains unchanged. Controller/parser operating-cost observations
remain in08-13; source fingerprints above establish the same production core/movie code,
without claiming fresh physical calibration or a new global cost bound.

## Actual native packages

Bytes are actual downloaded **current** component ZIP sizes; base columns are authenticated
producer totals. Base component ZIPs are not published by this workflow and are not invented here.

| Native lane | Library | Libretro | Runner | Current sum | Genuine base sum |
|---|---:|---:|---:|---:|---:|
| Linux arm64 | 98498 | 107083 | 118109 | 323690 | 323690 |
| Linux x64 | 96115 | 103068 | 115752 | 314935 | 314935 |
| macOS arm64 | 81327 | 85299 | 94867 | 261493 | 261498 |
| macOS x64 | 82328 | 85202 | 94679 | 262209 | 262209 |
| Windows arm64 | 94860 | 136646 | 160246 | 391752 | 391854 |
| Windows x64 | 94791 | 146454 | 172879 | 414124 | 414132 |

Joined peaks are414124 current and414132 base, both report-only. The historical399937
cutoff remains retired, not passed. No replacement quota, waiver, compression explanation
or performance conclusion is inferred from these distribution-byte observations.
All existing content, notices, firmware exclusion, platform-floor and release predicates pass.

Comparable compiler/platform prefixes are GNU15.2.0/Linux/aarch64 and x86_64;
AppleClang17.0.0.17000013/Darwin/arm64 and x86_64;
MSVC19.51.36260.0/Windows/ARM64 and AMD64. Every current/base pair preserves the full
environment string, fixture manifest SHA-256
62dc23cf0a687032b3c8d82f988c79d926a1814cd7eaf0f993cbb720a002141e and sample identity
406ca181ecad9c64d4030b09203f210b69d5f5c649453f0a5149fc864444cb4b.

## Actual frontend evidence

The final RetroArch job actually compiles the standalone numeric judge with
`tests/diag/driver.c host/palette.c runner/sha256.c` and the downloaded library;
this is not inferred from package metrics. The pinned released RetroArch1.22.2 executes
the retained DMA screenshot case and the battery roundtrip case. Each DMA/battery image
matches every320x242 runner pixel. Two fresh battery processes independently establish
counters1/1→2/2, supplied marker90→stub255, and every34816 saved byte for both frontend
and runner. The changed-pixel control exits1 at7,3; media/pixel and unsafe-path controls pass.
The agent viewed actual DMA and both battery screenshots, confirming the displayed values.
Exactly two conceptual hosted cases and300 physical tests/retroarch lines remain.

## Failures retained and integration corrections

All runs below are attempt1. They are preserved as failed/cancelled evidence, not converted
into passes. Available logs from every failed/cancelled job were inspected, including failures
inside jobs whose final status was cancelled; never-started cancelled jobs have no log.

| Source and runs | Observed outcome |
|---|---|
| old remote77013a3: [CI40152394410](https://github.com/szTheory/muhtari/actions/runs/40152394410), [Nightly40152395911](https://github.com/szTheory/muhtari/actions/runs/40152395911) | Dispatch raced the unfinished push and selected the old remote head. Both were cancelled; no evidence accepted. Later dispatches waited for successful push and exact remote SHA. |
| 751f488: [CI40152407468](https://github.com/szTheory/muhtari/actions/runs/40152407468), [Nightly40152408965](https://github.com/szTheory/muhtari/actions/runs/40152408965) | GCC shadow failure in keypad test blocked Linux, required BE compilation, library, property and nightly native execution. Nightly fuzz regression timed out at1.01s; the cancelled ASan job also contained a2.01s host-input timeout. Remaining incomplete work was cancelled and is not counted as passing. |
| a5ffd6f: [CI40152879574](https://github.com/szTheory/muhtari/actions/runs/40152879574) | Linux native host input timed out at1s and ASan at2.01s. Windows x64 also timed out in lightgun1.01s and host input1.01s. The cancelled macOS x64 log contained publication-control130.29s and audio.frames10.06s timeouts. These secondary failures remain recorded. Final exact-source inventories pass; no proven-flake claim or waiver is made. |
| a5ffd6f: [Nightly40152881373](https://github.com/szTheory/muhtari/actions/runs/40152881373) | Completed/failure: native host-input timeout prevented full PPC equality. All other actual jobs passed, including serial fuzz regression0.62s and10000-case properties. |

`tools/check_ci_run.cmake` on failed CI responses exits1 as expected; raw job/attempt/source
responses and actual command logs remain ignored under build/controller-acceptance/.

**[Rule2/3 - required integration correctness]** Rename the local keypad row to row_mask;
no assertion changes. Preserve the complete fuzz workload but run its regression serially
using the existing [CTest RUN_SERIAL property](https://cmake.org/cmake/help/latest/prop_test/RUN_SERIAL.html).
Its successful Linux0.62s observation justifies ceil(2×0.62)=2s for that existing deadline.
For expanded host input,12 complete AppleClang21 ASan/UBSan runs in three batches of four
all exit0, maximum2.191250208s; ceil(2×maximum)=5s. Linux arm64 complete0.90/0.76s observations
are retained; killed1s/2s samples were not treated as complete durations. Existing timeout
scaling is unchanged. Final actual Linux ASan host input completes2.15/1.84s. Workloads,
assertions, emulated-time bounds, policies and required/advisory membership stay intact.

The final Windows arm64 publication controls themselves complete729.04s current and703.82s
base. Their substantial existing cost is retained; no publication-harness/DAG redesign or
global efficiency improvement is claimed.

Other local failures are transparent: one standalone quality command used the wrong runner
path and exited1; the actual generated/full-workflow consumers pass. Homebrew LLVM21
sanitizer/coverage calibration stalled at startup and was terminated (controller143;
separate10s bounded probe124), so it supplied no successful timing. Calibration instead
used the working system AppleClang sanitizer build and the completed Linux job. The skill's
ci_monitor.cjs is absent; repository-documented gh/API inspection and the existing CMake
auditor were used. In-progress whole-run log retrieval failed; completed-job API logs and
then complete final logs supplied evidence. The initial Git ledger write was sandbox-denied;
normal Git metadata escalation succeeded with hooks, without a bypass or auth gate.

## Ownership review, scope and intent

ENGINEERING§11 review keeps behavior in existing owners: the diagnostic owns stimulus and
numeric judgment, hashes owns report/schema acceptance, and lane_equality owns identity.
The optional collector avoids another scenario implementation. The existing registration
owner holds its schema controls; quality policy itself needs no unrelated edit. Corrections
to stale embedding/hardware/preparation prose and the GCC/deadline fixes are necessary
integration deviations, not new controller features. No dependency, runtime global, public
interface, endpoint or new trust-boundary surface was introduced. No formal phase-review
or gap round was invoked by this executor; the parent owns those finite gates.

Agent judgment disposes this plan's transparency prohibition: source/help/docs explicitly
identify fitted/unmeasured optics, RC/Q32 charge, matrix recovery/contention, motion cadence
and adapter corner policies. Diagnostic/host agreement is never claimed as measured physical
calibration or universal retail accuracy. Fixtures and notices remain owned/admitted; no
GPL/LGPL/unlicensed emulator source was opened. No owner UAT or calibration assertion is
invented. Earlier plans' source-audit edge/intent flags retain their recorded lifecycle;
executed behavior evidence is not a fabricated passed edge-probe lifecycle. Their final
phase classification belongs to the parent verifier.

No new stub, skipped test or unrun required verification remains. Historical failures and
current model limits above are not hidden. Shipping, PR/main performance comparison,
post-merge release truths and physical calibration remain separate work.

## Artifact locations and self-check

Actual ignored evidence lives under build/controller-acceptance/: final-ci.json,
final-nightly.json, final logs and historical responses; ci-artifacts/ with18 ZIPs,
eight producer artifact groups, native/WASM hashes and actual screenshots;
quality-inputs/, trusted-quality.json, quality-output/ and quality-join-report.txt;
package-measurements.json; nightly-artifacts/ with raw native/PPC identities and four
JUnit files; and local-final-* outputs. No log or measurement dump is committed.

## Self-Check: PASSED

All16 declared changed files exist and all9 source commits are ancestors of HEAD.
The persisted ledger measures9 commits from9931d86 through df8f9a6. The realized diff
is37467 characters,9366 chars/4 estimate-scale tokens, excluding this summary. Noreply
identities, normal hooks, no tracked deletion, clean tree, exact final source/parent and
all required local/hosted/artifact claims were checked. The measured verification window
is4010 seconds from the first captured RED at01:45:07Z through final audits at02:51:57Z;
initial context loading and this metadata write are not separately timed.

08-14 is complete and ready for the parent's independent review/verifier in this execution
step. No later GSD step is invoked. The next separate command is `$gsd-verify-work 8`
after the parent completes its phase acceptance; shipping remains separate.
