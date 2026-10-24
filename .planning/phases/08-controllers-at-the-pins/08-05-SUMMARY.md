---
phase: 08-controllers-at-the-pins
plan: "05"
status: complete
halted: false
tasks_completed: 3
tasks_total: 3
subsystem: input
tags: [libretro, driving, diagnostics, packages, tdd, resolved-stop]
requires:
  - phase: 08-04
    provides: Immutable typed runner replay and independently derived driving fixture
provides:
  - Real base-MOUSE driving callbacks with device-aware console controls and descriptors
  - Independent numeric and native/audio evidence through all three deliverables
  - Historical exact-source Windows package failure with component attribution and a retained narrow size repair
  - Required authenticated package and CI acceptance after the separately authorized policy-purpose recovery
affects: [08-06, 08-07, 08-08, 08-09, 08-10, 08-11, 08-12, 08-13, 08-14]
tech-stack:
  added: []
  patterns: [Independent numeric judgment before equality, existing test-only module seams, bounded complete-field copies]
key-files:
  created:
    - .planning/phases/08-controllers-at-the-pins/08-05-SUMMARY.md
    - tools/check_ci_run.cmake
    - tests/policy/ci_run.cmake
  modified:
    - quality.json
    - tests/cmake/quality.cmake
    - tests/cmake/quality_join.cmake
    - tests/policy/quality.cmake
    - tests/policy/tests.cmake
    - docs/maintaining.md
    - .github/workflows/ci.yml
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
  - Retain the verified compact complete-field copy loop; its earlier savings did not satisfy the former cutoff.
  - Retain historical cutoff failures as failures and accept the separately authorized DEC.61 retirement with mandatory authenticated measurements.
  - Close only the resolved package gate after exact-source required CI; retain advisory status and every independent domain guard.
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
  - id: authenticated-package-observations
    description: Required native current/base package measurements pass after separately authorized cutoff retirement, with historical failures preserved.
    requirement: INPUT-04
    verification:
      - kind: integration
        ref: "Actual eight-lane authenticated join at source62fa325f665984fadc6324d65a49baed77fb31d2/baseae71481227e38105ee51ca9e92f9cc5f85285b8d; exit0; peaks401683/401727 retained; https://github.com/szTheory/muhtari/actions/runs/40139744520"
        status: pass
      - kind: unit
        ref: "policy.quality.controls retirement, above-old-cutoff and retained identity/type/status/deletion faults; exit0; https://github.com/szTheory/muhtari/actions/runs/40139744520"
        status: pass
      - kind: other
        ref: "tools/check_ci_run.cmake exact source/attempt and every job; exit0 required verdict with advisory library failure exposed; https://github.com/szTheory/muhtari/actions/runs/40139744520"
        status: pass
    human_judgment: false
  - id: retained-frontend-judge
    description: The existing standalone numeric and pixel judges link the owned digest helper before actual RetroArch execution.
    requirement: INPUT-01
    verification:
      - kind: automated_ui
        ref: Corrected source9a64bbc in CI40135571745 attempt2, job121003161256; exact standalone compile, DMA pixel equality, two fresh battery complete-byte/pixel judges and negative controls pass
        status: pass
    human_judgment: false
actuals:
  tokens: 19325
  tasks: 3
  commits: 12
plan_head_before: 0d2f8b877e6ba68cc4ccb803a6d4ed9cece13c36
plan_head_after: 62fa325f665984fadc6324d65a49baed77fb31d2
duration: 101min
started: 2026-10-24T20:39:15Z
completed: 2026-10-24
recovery_source: 62fa325f665984fadc6324d65a49baed77fb31d2
recovery_run: https://github.com/szTheory/muhtari/actions/runs/40139744520

---

# Phase 8 Plan 5: Real driving hosts and early package gate Summary

**Current acceptance:** all3 tasks are complete under the separately authorized
DEC.61 measurement contract. Required exact-source CI and actual package/evidence
consumers pass. The former cutoff was exceeded and retired, not passed.
Only plan05 closes; phase5/14, no INPUT requirement complete, next08-06 ready.

## Historical execution and halt through9a64bbc

The original observations, failed exits and halt rationale below remain historical.
The policy-purpose resolution and new acceptance follow them at the end.

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

## Initial complete package cohort

Final source: f7e369902f4b054d3fcd8a59c397ef881492a170.
Genuine event-base parent: ce5397e5b52c2cfdc36058c55a6e012346c75a8f.
Completed failed run: https://github.com/szTheory/muhtari/actions/runs/40132929297.
All six native current/base builds pass 646 ordinary and 6 release tests.
The required quality job and an independent local replay of the unchanged join
both fail specifically on archive_bytes, after authenticating all eight real lanes.
Parent audit additionally found a required RetroArch job link failure in that run:
its standalone diagnostic build omitted the newly required owned SHA helper.
The earlier claim that quality was the sole required failure was incorrect.
The one-line workflow correction in9a64bbc links runner/sha256.c and passes the exact
standalone compile and DMA/battery judges locally. Fresh hosted frontend acceptance
is recorded separately below. The package observations in this initial cohort
remain specific to f7e3699/ce5397e.

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
| Halt metadata | 986e4dd | Initial durable package halt, later audited and corrected |
| 2 corrective integration | 9a64bbc | Link owned SHA helper in the actual standalone frontend judge |

Task 1 RED: the built input fixture passes, then libretro.host.input fails because
MOUSE X/LEFT query counts are0, driving pins remain neutral, and descriptors remain
JOYPAD. Task 2 RED: the real diagnostic completes with valid header/CRC and records1–5,
then record6 is255 instead of the independently expected222 with motion absent.
Both real JUnit reports return RED_EVIDENCE_OK from the installed classifier and
pass semantic inspection. Build/setup failures were corrected before collecting
either accepted RED. GREEN implements the declared behavior; no separate refactor
commit was needed. Raw RED evidence remains in ignored build output, not git logs.

The persisted plan ledger measures11 commits from the recorded base to9a64bbc,
including the initial halt metadata commit. Actual tokens are
ceil(77299 realized source/test/workflow/doc diff characters/4)=19325, excluding
planning/halt/state metadata. These values are not harness-token measurements.

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
- check evaluation-scope --plan 08-05 --commits-only --raw — initial exit0 with nine task commits; final corrective-scope check is recorded below. Added-line stub/skip scan finds none.

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
5. [Rule 1 - Required integration] Parent audit found that the standalone RetroArch
   numeric judge still compiled driver.c without runner/sha256.c after the driver
   gained digest references. This manual compile was the only such use outside
   CMake targets. Added the existing owned source to the existing command; no lane,
   native/hash/pixel/battery check or predicate was removed. The former exact
   command reproduces exit1 with four missing SHA symbols; the corrected command
   exits0 and the actual DMA numeric/statistics and battery full-byte judges pass.
   Full local646+6 CI and hygiene24 pass. Fresh hosted acceptance passes in
   https://github.com/szTheory/muhtari/actions/runs/40135571745 at source
   9a64bbc7aba72bf7ae98e2a3e796907a6eb04cb9 with genuine event parent
   986e4ddfb73c5edcb02c3ce98cfa959afbce7260. Earlier f7e3699 package measurements
   are not a new-source package pass. Defect register52 tracks this correction.

## Corrected frontend and exhaustive final audit

Corrected source:9a64bbc7aba72bf7ae98e2a3e796907a6eb04cb9.
Its genuine event parent:986e4ddfb73c5edcb02c3ce98cfa959afbce7260.
Run:https://github.com/szTheory/muhtari/actions/runs/40135571745, attempt2,
completed/failure. The actual RetroArch job121003161256 is completed/success at
that exact source. Every required outcome was inspected, including RetroArch
and the dependent CI required aggregate; no failure is inferred away from a
successful metric or native test subset.

The original standalone command, without runner/sha256.c, exits1 locally with
the same four undefined digest functions as the actual failed f7e3699 job.
The corrected command is:

```sh
cc -std=c17 -O2 -Wall -Wextra -Werror -Isrc -Iinclude -Ihost \
  tests/diag/driver.c host/palette.c runner/sha256.c \
  build/library/lib/libmuhtari.a -o build/frames/diag
```

It exits0 against the actual packaged library. The compiled program judges
dma.a78 with expected.csv/stats.csv and battery.a78 with expected.csv, both exit0.
The actual hosted command also exits0. RetroArch's DMA screenshot equals every
pixel of the retained runner frame. Both fresh frontend battery launches judge
all34816 bytes independently at counts1 then2, with each screenshot320x242/every
pixel equal. Comparison negative controls and firmware/save guards pass. No new
case, lane, weakened check or dependency was introduced. The existing actual CI
consumer catches this link dependency regression.

The corrected run's initial macOS x64 attempt timed out only in the unchanged
policy.quality.controls test at30.14s;645/646 passed. It prevented dependent
frontend execution and left the initial quality artifact set incomplete. GitHub
refused an early retry because that workflow was still running (HTTP403, not an
authentication or approval rejection). After the run completed, the targeted
same-source job retry re-requested the existing downstream jobs. macOS x64
current/base checks and actual RetroArch then passed. The failed attempt remains
recorded and is not counted as a successful timing sample. No timer was changed.

Latest attempt2 exhaustive outcomes:

- Six native current/base lanes, hygiene, ASan, both nofp lanes, BE32 compilation,
  native hash equality, WASM, library, dispatch perf control and RetroArch succeed.
- title is intentionally skipped for dispatch.
- quality fails on actual archive_bytes401687>399937.
- CI required fails because that required quality gate failed.

The new source has its own actual Windows package observations; none are
inherited from f7e3699. Exactly three candidate ZIPs are library90300,
runner169903 and libretro141484 bytes: **401687 total**, excess**1750**.
Its genuine parent totals**401724**, excess**1787**. Compiler/platform, raw
manifest/sample identities and raw parent-policy checks match the preceding
documented environment; all eight new lane artifacts were individually
authenticated with full9a64bbc/986e4dd source/base SHAs and the new run URL.
The unchanged full join is replayed with DIRECTORY=build/controller-retroarch-repair/quality-inputs,
OUT=build/controller-retroarch-repair/join, the actual parent policy there,
EXPECTED_SHA=9a64bbc7aba72bf7ae98e2a3e796907a6eb04cb9,
BASE_SHA=986e4ddfb73c5edcb02c3ce98cfa959afbce7260, PERF_REQUIRED=OFF and that
run's SOURCE URL: exit1 specifically on401687>399937, matching the hosted job.
The required resource defect51 remains open; frontend defect52 is marked fixed
only after inspecting this actual hosted compile/pixel/save acceptance.

Evidence commands: XDG_CACHE_HOME=/tmp/muhtari-gh-cache gh run view 40135571745
--json status,conclusion,attempt,headSha,jobs; gh api of job121003161256 and its
logs; downloads of all eight quality artifacts plus the three actual Windows
ZIPs; Python exact-identity/component audit — exit0. The original bad compile
exits1, the corrected compile and numeric judges exit0, the unchanged join exits1.
All hosted evidence in this section uses the corrected run URL above. Logs remain
ignored build output. The installed github-workflows skill was read; its
ci_monitor.cjs helper was absent from both the repository and skill directories,
so the authorized existing gh CLI was used without installing a substitute.

Failed/superseded hosted evidence is explicitly excluded from acceptance:

- https://github.com/szTheory/muhtari/actions/runs/40131407509 at source f463f85:
  Windows fails new diagnostic report transport; macOS x64 times out in existing
  quality controls. Cancelled once unusable; its partial package observations do
  not establish acceptance.
- https://github.com/szTheory/muhtari/actions/runs/40131986619 at source ec98fa6:
  corrected Windows current/base646+6 pass, but actual sums402260/402301 exceed
  the ceiling. macOS x64 has existing publish/quality control timeouts. Superseded
  run cancelled after conclusive size evidence; no predicate/timeout was relaxed.
- CI40132929297 executes the same controls successfully on all six native
  current/base lanes; both quality/archive and RetroArch/standalone-link fail.
  The dependent CI required aggregate also fails. No failed attempt is
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

Next separate command: `$gsd-plan-phase 8`, using the latest exact residual1750/1787-byte
resource evidence to revise the halted package gate. Preserve tested behavior,
fixture identities, three deliverables and the existing ceiling. No --gaps route
is claimed without a VERIFICATION/UAT gap report, and no other GSD step is run here.

## Self-Check: PASSED

All17 changed source/test/workflow/doc/inventory files and this summary exist. All11
recorded plan commits are ancestors of HEAD. The persisted ledger base/count/head agree
with the measured actuals. query phase-plan-index 08 and query init.execute-phase 08
both exit0: 08-05 is halted, all9 dependent plans08-06–08-14 carry blocked_by08-05,
and runnable_plans is empty. This validates the durable stop; it does not claim
the failed archive gate or plan is complete.

## Policy-purpose recovery accepted, 2026-10-24

The owner's separate review requested bounded specialist synthesis, adversarial
assessment and adoption. DEC.61 retires only the unsupported historical compressed
cutoff. It preserves required authenticated measurements, exactly3 component ZIPs,
members/notices/prohibited-asset checks, raw policy/fixture/sample authority and
every ABI/runtime/performance/required-lane contract. No replacement quota, report
direction, advisory conversion, dependency or generic policy-deletion bypass was added.

Recovery source: 62fa325f665984fadc6324d65a49baed77fb31d2; genuine independently rebuilt parent: ae71481227e38105ee51ca9e92f9cc5f85285b8d.
Run/attempt: https://github.com/szTheory/muhtari/actions/runs/40139744520, attempt1, no retry. Required CI succeeds. Overall run
conclusion is failure solely from advisory library timing; the complete audit
exposes it. Existing2-second pokey/rmtramdemo tests timed out, leaving37/39 library
results; these are unaccepted advisory evidence, not a proven flake or new gate.
WASM585 and all required jobs, including quality and actual RetroArch, pass.

All6 native current builds pass647+6; their genuine bases pass646+6. Hygiene24/24,
ASan630/629, ABI17/17, both nofp lanes, BE32 compilation and native hash equality
pass under retained status/authority. Actual RetroArch job121013890626 compiles
the standalone judge, compares every DMA pixel, judges all34816 bytes in each of
two fresh battery launches at1→2, and passes screenshot/negative controls.
Dispatch perf3838334/3838334/3838334 has zero spread; it establishes repeatability,
not phase-versus-main performance or whole-controller cost.

| Native lane | Library ZIP | Runner ZIP | Libretro ZIP | Current sum | Genuine base sum |
|---|---:|---:|---:|---:|---:|
| linux-arm64 | 90695 | 110622 | 96714 | 298031 | 298031 |
| linux-x64 | 88514 | 108535 | 93907 | 290956 | 290956 |
| macos-arm64 | 75053 | 89650 | 77128 | 241831 | 241835 |
| macos-x64 | 76500 | 89249 | 77528 | 243277 | 243279 |
| windows-arm64 | 89380 | 156698 | 131752 | 377830 | 377874 |
| windows-x64 | 90297 | 169903 | 141483 | 401683 | 401727 |

The workflow publishes base totals rather than individual base ZIPs; no base
component sizes are invented. All8 raw base policies match the genuine parent's
Git object. Each of6 downloaded candidate sets has exactly3 files, correct actual
sizes/CRCs and producer sums. Current/base source, run, compiler/configuration and
raw manifest/sample identities are checked before reduction. Joined JSON retains
both authenticated peaks401683/401727. They remain above the former cutoff.

| Command or actual consumer | Exit | Run |
|---|---:|---|
| cmake --workflow --preset ci | 0;647+6 and3 packages | N/A-local |
| check_junit.cmake with current native/release exact inventories | 0;647 and6 once each | N/A-local |
| policy.ci_run.controls and policy.quality.controls | 0;22 audit pass/fault cases and retained quality faults | N/A-local and CI40139744520 |
| gh run view40139744520 --json attempt,status,conclusion,headSha,url,jobs | 0 | https://github.com/szTheory/muhtari/actions/runs/40139744520 |
| cmake -DINPUT=build/workflow-recovery-ci-run.json -DEXPECTED_SHA=62fa325f665984fadc6324d65a49baed77fb31d2 -P tools/check_ci_run.cmake | 0 required verdict; title skip/library failure visible | https://github.com/szTheory/muhtari/actions/runs/40139744520 |
| Actual quality_join.cmake over build/workflow-recovery-62fa325/quality-inputs, exact source/base/SOURCE and genuine raw policy, PERF_REQUIRED=OFF | 0; per-lane reports and retained machine peaks | https://github.com/szTheory/muhtari/actions/runs/40139744520 |
| Downloaded artifact audit and independent ZIP/raw-policy/peak assertions | 0 | https://github.com/szTheory/muhtari/actions/runs/40139744520 |

The audit uses the existing unique CI required aggregate for membership; it adds
no second job registry or promotion of advisory jobs. The Apple quality-control
deadline uses existing measured policy, provisional46s with all workloads/oracles.
This run's x64 current/base controls completed22.52/24.10s on the first attempt.
The original19325 diff-token estimate and101min duration describe initial execution;
recovery adds source62fa325 and Task3, without inventing model-token measurements.

The canonical summary and failed history were preserved. Only after these new
consumer results did status become complete/tasks3/3. Current phase acceptance is
5/14; all five INPUT requirements remain incomplete. No phase completion, new
controller hardware, other GSD step, PR, merge or release occurred in this recovery.

Next separate command: `$gsd-execute-phase 8`, beginning with08-06.
