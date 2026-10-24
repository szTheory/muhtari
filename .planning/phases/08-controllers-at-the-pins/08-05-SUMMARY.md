---
phase: 08-controllers-at-the-pins
plan: "05"
status: halted
halted: true
tasks_completed: 2
tasks_total: 3
blocked_task: 3
subsystem: input
tags: [libretro, driving, diagnostics, packages, tdd, designed-stop]
requires:
  - phase: 08-04
    provides: Immutable typed runner replay and independently derived driving fixture
provides:
  - Real base-MOUSE driving callbacks with device-aware console controls and descriptors
  - Independent numeric and native/audio evidence through all three deliverables
  - Exact-source Windows package failure with component attribution and a retained narrow size repair
affects: [08-06, 08-07, 08-08, 08-09, 08-10, 08-11, 08-12, 08-13, 08-14]
tech-stack:
  added: []
  patterns: [Independent numeric judgment before equality, existing test-only module seams, bounded complete-field copies]
key-files:
  created:
    - .planning/phases/08-controllers-at-the-pins/08-05-SUMMARY.md
  modified:
    - libretro/libretro.c
    - tests/libretro/host.c
    - tests/libretro/tests.cmake
    - tests/diag/driver.c
    - tests/diag/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - src/ports.c
    - runner/main.c
    - runner/movie.c
    - runner/movie.h
    - tests/runner/test_movie.c
    - tests/runner/test_runner.c
    - docs/libretro.md
    - docs/emulation-tests.md
    - docs/quality.md
key-decisions:
  - Expose actual native bytes only in the existing test module because the frame core clears its temporary video pointer before the frontend callback.
  - Retain the verified compact complete-field copy loop and its observed Windows savings without claiming the package gate passed.
  - Preserve the 399937-byte ceiling and halt all dependent controller expansion until an evidence-backed resource resolution.
requirements-completed: []
requirements-contributed: [INPUT-01, INPUT-04]
coverage:
  - id: driving-callbacks
    description: Driving uses stable subclass selection and once-per-frame base MOUSE X and LEFT polling with ignored Y and CPU-visible effects.
    requirement: INPUT-01
    verification:
      - kind: integration
        ref: libretro.host.input and libretro.host.devices; exit 0 locally and in all six native current/base builds; CI40132929297
        status: pass
    human_judgment: false
  - id: console-and-storage
    description: Device-specific descriptors and one console provider preserve ordinary mappings, held-edge behavior and either EEPROM placement.
    requirement: INPUT-01
    verification:
      - kind: integration
        ref: libretro.host.input, devices, layout, battery, battery_order, reset and options; exit 0; CI40132929297
        status: pass
    human_judgment: false
  - id: independent-three-host-driving
    description: Nineteen independently derived pin records precede equality of twelve actual native frames, raw transitions and exact mono/stereo PCM counts.
    requirement: INPUT-04
    verification:
      - kind: integration
        ref: diag.controllers; exit 0 in six native current/base builds and the portable library/runner subset in WASM; CI40132929297
        status: pass
    human_judgment: false
  - id: malformed-controller-records
    description: Actual report consumers reject missing, truncated, duplicate, incorrect numeric, version, count and CRC records.
    requirement: INPUT-04
    verification:
      - kind: integration
        ref: diag.controllers numeric/protocol controls; exit 0; CI40132929297
        status: pass
    human_judgment: false
  - id: limiting-package-envelope
    description: Exact Windows x64 candidate and independently rebuilt parent exceed the unchanged archive ceiling and block expansion.
    requirement: INPUT-04
    verification:
      - kind: integration
        ref: Unchanged tests/cmake/quality_join.cmake over all eight actual lane artifacts; exit 1, archive_bytes 401690 exceeds 399937; https://github.com/szTheory/muhtari/actions/runs/40132929297
        status: fail
    human_judgment: false
actuals:
  tokens: 19137
  tasks: 2
  commits: 9
plan_head_before: 0d2f8b877e6ba68cc4ccb803a6d4ed9cece13c36
plan_head_after: f7e369902f4b054d3fcd8a59c397ef881492a170
duration: 58min
started: 2026-10-24T20:39:15Z
completed: 2026-10-24
---

# Phase 8 Plan 5: Real driving hosts and early package gate Summary

**Real driving callbacks and independent three-deliverable observations pass; Windows x64 packages still exceed the fixed ceiling after a verified narrow size repair.**

Tasks 1–2 are complete. Task 3 failed its required acceptance criterion. This is a
designed stop, not plan or phase completion: 08-06–08-14 must remain blocked.
No INPUT requirement is complete. No quota, baseline, archive contents, fixture
identity, performance rule, native/advisory status or test predicate was weakened.

## Delivered behavior

Driving is selected by MOUSE subclass 0 and polled through base MOUSE X/LEFT once
after the frame poll; Y is ignored. The adapter submits a neutral extended packet
before initial emulation. Auto resolves supported driving headers. Old JOYPAD
subclass identities remain unchanged. Descriptors refresh at load and selection
with stable storage and the exact queried base/index/ID.

The lowest ordinary ProLine/joystick supplies console controls, preserving port 1
preference when both are ordinary. Otherwise driving uses MIDDLE/BUTTON_4/RIGHT
for Pause/Select/Reset while LEFT fires. With no active controller, supplemental
port 1 L2/R2/L3 supply console actions. Difficulty options remain available;
ordinary L/R retain their edge behavior and provider changes seed held levels.
Unknown live selections warn and safely unplug ordinary input; installed storage
retains its identity/span and reports reload. Both EEPROM placements are tested.

The owned motion executable runs the predeclared schedule through library packets,
an actual movie2 runner child, and actual loaded-module mouse callbacks. Its 19
expected.csv values remain independent and unchanged. Version, identity, status,
count, record IDs, flags and CRC are checked before equality evidence. Faults with
recomputed CRC still fail on duplicate or wrong numeric records.

Twelve actual native frames and 8,861 mono samples are compared. Public module
palette pixels and duplicated stereo samples/counts are judged separately from
actual native bytes. The existing raw observer registers before load. The silent
diagnostic produces the observed empty transition hash
e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855;
PCM is ef6a8e29e8dd01256bfed1a0cd310c1580be2d7802f88ffca1ebb3652477b4eb.
Frame 5 omits library submission while runner/module retain fire with zero new
motion. Native inventory includes all three hosts; WASM executes its actual
library/runner subset and reports module unavailability without counting it.

## Authoritative package failure

Final source: f7e369902f4b054d3fcd8a59c397ef881492a170.
Genuine event-base parent: ce5397e5b52c2cfdc36058c55a6e012346c75a8f.
Completed failed run: https://github.com/szTheory/muhtari/actions/runs/40132929297.
All six native current/base builds pass 646 ordinary and 6 release tests.
The required quality job and an independent local replay of the unchanged join
both fail specifically on archive_bytes, after authenticating all eight real lanes.

| Lane | Library ZIP | Runner ZIP | Libretro ZIP | Candidate sum | Genuine base sum |
|---|---:|---:|---:|---:|---:|
| Linux arm64 | 90695 | 110622 | 96714 | 298031 | 298031 |
| Linux x64 | 88514 | 108535 | 93907 | 290956 | 290956 |
| macOS arm64 | 75053 | 89650 | 77128 | 241831 | 241835 |
| macOS x64 | 76499 | 89249 | 77528 | 243276 | 243275 |
| Windows arm64 | 89376 | 156698 | 131753 | 377827 | 377877 |
| Windows x64 | 90304 | 169903 | 141483 | **401690** | **401726** |

Windows x64 exceeds399937 by **1753 candidate /1789 base bytes**. These are actual
three-ZIP sizes, not the outer Actions artifact size. Each candidate artifact has
exactly library, runner and libretro ZIPs. Each independently rebuilt base is
measured by the unchanged producer after its three-component release checks.
The existing workflow publishes base aggregate JSON, not separate base ZIPs;
no unobserved per-component base sizes are invented.

Both Windows observations have environment
MSVC-19.51.36260.0-Windows-AMD64 on the same
windows-2025-vs2026 image20261021.278.1, identical configuration and raw identities:

- Manifest SHA-256: c1545cdf9c6e7b59fa144feab2f16731a9408e4485b26071eceb7f4f1bc82729.
- Built-sample SHA-256: 7573aae5b2eaa0796ecd04574b479b0d94bea451f97b2f446c50dd89a4849f39.

Every actual observation's full source/base SHA and run URL were checked, both
environment strings match, the recorded base SHA equals the event parent, and
raw base-policy bytes equal that parent's Git object. The unchanged join consumes
these observations and rejects the actual sum; no synthetic replacement is used.

Windows candidate attribution, from actual ZIP directory entries:

| Contribution | Compressed bytes |
|---|---:|
| Static library payload,178066 bytes uncompressed | 77951 |
| Libretro DLL payload,247296 bytes uncompressed | 136345 |
| Runner EXE payload,285184 bytes uncompressed | 165790 |
| Public header,9737 bytes uncompressed | 2901 |
| Required licence/notices in all three ZIPs | 10233 |
| CMake package files | 3268 |
| pkg-config file | 151 |
| Libretro info | 665 |
| Container/directory overhead | 4386 |
| User documentation | 0 |
| **Total** | **401690** |

Compression already uses level9; MSVC retains/O1/Ob1. Notices and contents stay
intact. Replacing 21 guarded scalar copies with one bounded fixed-four-byte loop
preserves complete-field/prefix, alignment, transaction and libc-symbol behavior.
It saves570 observed Windows candidate bytes (402260→401690) and575 on the two
independent base measurements (402301→401726). Small unchanged-build compression
variation remains visible. The repair is retained but is insufficient. Further
optimization or contract changes belong to the next evidence-backed planning step.

## Task commits and TDD evidence

| Task | Commit | Purpose |
|---|---|---|
| 1 RED | 2ec9b47 | Real callback/pin/descriptor/provider assertions |
| 1 GREEN | c51a5ce | Driving adapter and device-aware console mapping |
| 2 RED | 8db1dba | Independent numeric diagnostic judgment |
| 2 GREEN | cfccbea | Actual library/runner/module evidence and required formatting |
| 3 preparation | f463f85 | Exact-source package evidence boundary |
| 2 correction | 05b8b5c | Canonical LF runner-report transport on Windows |
| 3 attribution | ec98fa6 | Concrete shipped-byte attribution boundary |
| 3 repair | ce5397e | Bounded compact complete-field normalization |
| 3 evidence | f7e3699 | Preserve acceptance gates for size repair |

Task 1 RED: the built input fixture passes, then libretro.host.input fails because
MOUSE X/LEFT query counts are0, driving pins remain neutral, and descriptors remain
JOYPAD. Task 2 RED: the real diagnostic completes with valid header/CRC and records1–5,
then record6 is255 instead of the independently expected222 with motion absent.
Both real JUnit reports return RED_EVIDENCE_OK from the installed classifier and
pass semantic inspection. Build/setup failures were corrected before collecting
either accepted RED. GREEN implements the declared behavior; no separate refactor
commit was needed. Raw RED evidence remains in ignored build output, not git logs.

The persisted plan ledger measures9 source/task commits from the recorded base to
f7e3699. Actual tokens are ceil(76546 realized diff characters/4)=19137, excluding
this halt/state metadata. These values are not harness-token measurements.

## Command evidence

Commands use repository-relative paths. Local checks have run URL N/A-local.

- Task 1: cmake --preset ci; cmake --build --preset ci; ctest --preset ci -R '^libretro[.]host[.](devices|input)$' --output-junit tests/controller-red.xml --no-tests=error --output-on-failure — build exit0, RED exit8. check tdd-red-evidence build/ci/controller-red.json --raw — exit0, RED_EVIDENCE_OK.
- Task 2: ctest --preset ci -R '^diag.controllers$' --output-junit tests/controller-diag-red.xml --no-tests=error --output-on-failure — RED exit8. check tdd-red-evidence build/ci/controller-diag-red.json --raw — exit0, RED_EVIDENCE_OK.
- Scoped libretro devices/input/layout/battery/battery_order/reset/options execution — exit0,8 fixture-inclusive tests. Post-commit tracer command from the plan — exit0,6 fixture-inclusive tests.
- cmake --workflow --preset ci — exit0,646+6 after final source changes. Exact check_junit commands with EXCLUDE=release and LABELS=release — exit0,646 and6 respectively.
- cmake --workflow --preset hygiene — exit0,24 after formatting and again after the narrow repair. clang-format21 dry-run and git diff --check — exit0.
- ctest --preset ci -R '^(abi.undefined|abi.globals|abi.api_v6|input[.])' --output-junit tests/controller-size-probe.xml --no-tests=error --output-on-failure — exit0,22 cases after normalization repair.
- cmake --build --preset ci --target package; ctest --preset ci-release --output-on-failure — exit0/0. Final local producer invocation at source f7e3699 exits0 and measures239780 bytes; this is local-lane evidence only.
- policy.quality.controls — exit0 in full CI and a focused rerun, including actual controller-motion manifest producer/join pass/fault cases.
- gh run view 40132929297 --json status,conclusion,headSha,url — exit0, completed/failure at exact f7e3699. gh run download 40132929297 for each actual quality/archive artifact — exit0. Python artifact audit — exit0, exactly3 components, measured sums, complete source/base/run/environment/policy identities.
- cmake -DDIRECTORY=build/controller-hosted-f7e3699/quality -DOUT=build/controller-hosted-f7e3699/join -DPOLICY=quality.json -DBASE_POLICY=build/controller-hosted-f7e3699/quality/quality-windows-x64/quality-base-policy.json -DEXPECTED_SHA=f7e369902f4b054d3fcd8a59c397ef881492a170 -DBASE_SHA=ce5397e5b52c2cfdc36058c55a6e012346c75a8f -DPERF_REQUIRED=OFF -DSOURCE=https://github.com/szTheory/muhtari/actions/runs/40132929297 -P tests/cmake/quality_join.cmake — exit1, measured regression archive_bytes401690, ceiling399937. This reproduces the hosted quality job failure.
- A direct predicate reading the actual current/base JSON and quality.json ceiling exits1: values401690/401726, excess1753/1789.
- Hosted WASM workflow/check_junit — exit0,585 tests exactly once; diag.controllers passes0.22s. Native hash-equality and WASM/Linux hash comparison pass. Hosted nofp jobs on both GCC architectures pass; BE32 compilation passes. These results are from CI40132929297 above.
- Dispatch perf control — exit0, three same-head test-card observations3838334/3838334/3838334 Ir/frame, spread0. Dispatch uses NO_BASE=none; it is repeatability evidence, not phase-versus-origin performance acceptance. The armed+1% PR comparator remains unchanged and advisory/required roles are preserved.
- check evaluation-scope --plan 08-05 --commits-only --raw — exit0, all nine task commits resolve. Added-line stub/skip scan finds none; all16 modified source/test/doc/inventory files exist.

## Deviations and failed attempts

1. [Rule 3 - Blocking] The existing instance accessor cannot expose native rows after
   frame.c clears maria.rows. Added only test-module muhtari__libretro_native, consumed
   by the real loaded-module judge; production exports/semantics remain unchanged.
   The existing testdb diagnostic target also compiles the owned SHA helper.
2. [Rule 2 - Required contract] Fresh hygiene exposed preceding-runner formatting
   failures in runner/main.c, movie.c, movie.h, test_movie.c and test_runner.c.
   Applied clang-format21 only; full CI/hygiene pass. No behavior changed there.
3. [Rule 1 - Bug] Initial hosted Windows diagnostic report transfer used file(WRITE),
   causing the strict binary report consumer to refuse it. Reused the retail/battery
   canonical-LF file(CONFIGURE ... NEWLINE_STYLE UNIX) mechanism; parser strictness
   is unchanged. Final Windows current/base controller batches pass.
4. [Rule 3 - Required resource repair] Narrowly compacted the existing phase-current
   scalar normalizer in src/ports.c, authorized within the package repair. Prefix,
   alignment, signed extrema, transactional and restricted-link tests remain intact;
   all final native, hygiene, ASan and nofp checks pass. No general refactor followed.

Failed/superseded hosted evidence is explicitly excluded from acceptance:

- https://github.com/szTheory/muhtari/actions/runs/40131407509 at source f463f85:
  Windows fails new diagnostic report transport; macOS x64 times out in existing
  quality controls. Cancelled once unusable; its partial package observations do
  not establish acceptance.
- https://github.com/szTheory/muhtari/actions/runs/40131986619 at source ec98fa6:
  corrected Windows current/base646+6 pass, but actual sums402260/402301 exceed
  the ceiling. macOS x64 has existing publish/quality control timeouts. Superseded
  run cancelled after conclusive size evidence; no predicate/timeout was relaxed.
- Final CI40132929297 executes the same controls successfully on all six native
  current/base lanes; only the actual archive bound fails. No failed attempt is
  counted as a passing timing or package sample.

Local WASM and nofp configuration probes exit1 because this host lacks the Linux
clang21/WASI toolchain and nofp requires GCC instead of AppleClang. Final hosted
WASM585 and both GCC nofp jobs supply the actual checks. A local producer correctly
refused a scoped probe's overwritten default JUnit; a fresh complete workflow
restored the inventory before final observation production. Initial inventory
invocations lacking the release exclusion or using the wrong release-report name
were corrected to the workflow's exact existing commands; no inventory was weakened.

No authentication gate, owner verification, package install, code-review fix round,
gap round, pull request, ship or merge was used. No logs or downloaded archives
are committed. No known implementation stubs or new production trust boundary
remain; the unresolved defect is the required package envelope.

## Halt and next step

Accepted phase progress remains4/14. GSD's durable status:halted marker must identify
08-05 as a designed stop and propagate blocked_by08-05 through08-06–08-14. Its
mechanical summarized-plan count includes halted records; that does not make05
accepted or advance the current plan. All INPUT requirements remain pending.

Next separate command: `$gsd-plan-phase 8`, using the exact residual1753/1789-byte
resource evidence to revise the halted package gate. Preserve tested behavior,
fixture identities, three deliverables and the existing ceiling. No --gaps route
is claimed without a VERIFICATION/UAT gap report, and no other GSD step is run here.

## Self-Check: PASSED

All16 changed source/test/doc/inventory files and this summary exist. All9 recorded
task commits are ancestors of HEAD. The persisted ledger base/count/head agree
with the measured actuals. query phase-plan-index 08 and query init.execute-phase 08
both exit0: 08-05 is halted, all9 dependent plans08-06–08-14 carry blocked_by08-05,
and runnable_plans is empty. This validates the durable stop; it does not claim
the failed archive gate or plan is complete.
