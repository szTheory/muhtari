---
phase: 05-pipeline-tune-up
plan: "01"
status: complete
subsystem: infra
tags: [cmake, packaging, quality]
requires:
  - phase: v1.0
    provides: Working package validation and native CI
provides:
  - Complete measured maintenance boundary and fixed scoped allocation
  - Tested physical counter and initial absolute gate-lines metric
affects: [05-02, 05-03, 05-04, 05-05, 05-06, 05-07, 05-08, 05-09]
tech-stack:
  added: []
  patterns: [explicit whole-file inventory, exact-failure CMake controls]
key-files:
  created:
    - tests/cmake/gate_count.cmake
    - tests/policy/gate-lines.txt
    - quality.json
  modified:
    - tests/cmake/archive_contents.cmake
    - tests/policy/tests.cmake
    - tests/inventory/ci.txt
key-decisions:
  - Freeze the conservative 134-file whole-file baseline at 12885 lines, retain actual savings of two lines, and fix N1970 including reserve150 before additions.
  - Count all inventory, admission, control and embedded policy costs; leave source ratios advisory.
  - Keep phase-wide TUNE requirements pending until their remaining plans satisfy the complete contracts.
duration: 12min
completed: 2026-10-21
actuals:
  tokens: 11540
  tasks: 2
  commits: 3
plan_head_before: f0202e1f02ee24c2b7d2bc19bbe140f11f3c1fef
plan_head_after: 16ad83924457fe4823e22329819381df74bea421
requirements-completed: []
requirements-contributed: [TUNE-02, TUNE-03, TUNE-05]
coverage:
  - id: baseline
    description: Complete starting maintenance boundary and quality dimensions
    requirement: TUNE-05
    verification:
      - kind: other
        ref: git show f0202e1f02ee24c2b7d2bc19bbe140f11f3c1fef plus physical newline count
        status: pass
    human_judgment: false
  - id: archive
    description: Smaller archive-name construction retains actual package validation
    requirement: TUNE-02
    verification:
      - kind: integration
        ref: cmake --workflow --preset ci, 606 regular tests and six release checks, exit 0
        status: pass
    human_judgment: false
  - id: counter
    description: Physical line observations reject omitted and overlapping inventory inputs
    requirement: TUNE-03
    verification:
      - kind: unit
        ref: policy.gate_count.controls, exit 0
        status: pass
      - kind: integration
        ref: cmake -DROOT=. -DINVENTORY=tests/policy/gate-lines.txt -DOUT=build/ci/gate-count.json -P tests/cmake/gate_count.cmake, exit 0
        status: pass
    human_judgment: false
---

# Phase 5 Plan 1: Audited archive simplification and counting Summary

**An audited physical-line counter observes 13,251 lines after retained-fault archive
savings and initializes the fixed 14,853-line ceiling.**

Starting evidence was frozen before source edits. Both plan tasks are complete; the
phase-wide requirements remain pending for later timeout, ratchet, reporting and
release/docs acceptance. This summary claims their scoped contributions only.

## Starting measurement

Starting SHA: `f0202e1f02ee24c2b7d2bc19bbe140f11f3c1fef`. Every entry below is a whole file;
there are no numeric block exclusions, relocation discounts or comment/blank exclusions.
Physical lines are newline count plus one for a nonempty unterminated final line.
The temporary audit reads `git show SHA:path`; it does not contribute to the starting tree.
B = **12885** lines across 134 files.
Full src/ C/header denominator = 8577; authored denominator =
6530, excluding only the 2,047-line generated `src/cartdb.c`.
Advisory ratios are 150.23% / 197.32%.
Broad sensitivity adds the product RetroArch launch wrapper (59 lines):
**12944**. No hard source-ratio compliance is claimed.

Conservative whole-file treatment deliberately charges ordinary registration/generation
inside mixed files, vector conversion/reader tooling, install/build glue and notice/admission
documents. Ordinary emulator behavioral C tests, audio/hardware expected values, ROM/vector
bytes, generated cart database records and the screenshot pixel-comparison program remain
product evidence outside this maintenance metric. Their repository-policy invocations,
identity lists, dedicated malformed-input controls and empty-replay assertions are charged.
`.planning/config.json` is charged because it supplies the enforced workflow/chain policy;
planning narrative is not executable policy. The only additional broad-sensitivity file is
`tests/retroarch/run_retroarch.cmake`, a product frontend integration wrapper.

### Complete starting inventory

| File | Physical lines |
|---|---:|
| `.clang-format` | 8 |
| `.gitattributes` | 7 |
| `.githooks/pre-commit` | 3 |
| `.githooks/pre-push` | 4 |
| `.github/dependabot.yml` | 20 |
| `.github/rulesets/main.json` | 37 |
| `.github/workflows/ci.yml` | 559 |
| `.github/workflows/dependabot-automerge.yml` | 49 |
| `.github/workflows/nightly.yml` | 262 |
| `.github/workflows/release.yml` | 244 |
| `.gitignore` | 42 |
| `.planning/config.json` | 89 |
| `.release-please-manifest.json` | 3 |
| `ASSET_POLICY.md` | 76 |
| `CMakeLists.txt` | 295 |
| `CMakePresets.json` | 393 |
| `LICENSE` | 21 |
| `PROVENANCE.md` | 118 |
| `THIRD-PARTY-NOTICES.md` | 125 |
| `cmake/MuhtariTests.cmake` | 69 |
| `cmake/muhtari.pc.in` | 9 |
| `cmake/muhtariConfig.cmake.in` | 6 |
| `cmake/packaging.cmake` | 59 |
| `cmake/toolchains/ppc-linux-gnu.cmake` | 14 |
| `cmake/toolchains/wasm32-wasi.cmake` | 20 |
| `cmake/version.h.in` | 10 |
| `libretro/CMakeLists.txt` | 41 |
| `pins.txt` | 55 |
| `release-please-config.json` | 13 |
| `scripts/apply-repo-settings.sh` | 152 |
| `scripts/check-title.sh` | 25 |
| `scripts/classify-changes.sh` | 27 |
| `scripts/fetch-pin.sh` | 67 |
| `scripts/hygiene.sh` | 177 |
| `scripts/publish.sh` | 183 |
| `tests/abi/global_fixture.c` | 14 |
| `tests/abi/libc_fixture.c` | 13 |
| `tests/abi/nofp_fixture.c` | 8 |
| `tests/abi/tests.cmake` | 135 |
| `tests/audio/tests.cmake` | 123 |
| `tests/boot/tests.cmake` | 27 |
| `tests/built/build.cmake` | 81 |
| `tests/built/coverage.txt` | 21 |
| `tests/built/frames.cmake` | 101 |
| `tests/built/openbios.cmake` | 76 |
| `tests/built/tests.cmake` | 176 |
| `tests/bus/tests.cmake` | 28 |
| `tests/cart/tests.cmake` | 63 |
| `tests/cmake/abi.cmake` | 111 |
| `tests/cmake/api_names.cmake` | 50 |
| `tests/cmake/api_version.cmake` | 48 |
| `tests/cmake/archive_contents.cmake` | 76 |
| `tests/cmake/budgets.cmake` | 31 |
| `tests/cmake/check_junit.cmake` | 58 |
| `tests/cmake/ci_required.cmake` | 91 |
| `tests/cmake/compat_ratchet.cmake` | 100 |
| `tests/cmake/dependabot.cmake` | 56 |
| `tests/cmake/exit_table.cmake` | 22 |
| `tests/cmake/expect_fail.cmake` | 19 |
| `tests/cmake/fetch_vectors.cmake` | 180 |
| `tests/cmake/firmware_scan.cmake` | 223 |
| `tests/cmake/floor.cmake` | 134 |
| `tests/cmake/hashes.cmake` | 72 |
| `tests/cmake/info_claims.cmake` | 40 |
| `tests/cmake/install_line.cmake` | 54 |
| `tests/cmake/no_float.cmake` | 59 |
| `tests/cmake/no_skip.cmake` | 38 |
| `tests/cmake/no_test_entries.cmake` | 49 |
| `tests/cmake/notices.cmake` | 57 |
| `tests/cmake/perf_compare.cmake` | 95 |
| `tests/cmake/pins.cmake` | 49 |
| `tests/cmake/rom_hash_coverage.cmake` | 69 |
| `tests/cmake/single_version.cmake` | 49 |
| `tests/cmake/timeouts.cmake` | 34 |
| `tests/cmake/vectors_pin.cmake` | 77 |
| `tests/cmake/vectors_sample_match.cmake` | 74 |
| `tests/cmake/vendored.cmake` | 38 |
| `tests/compat/allow.txt` | 8 |
| `tests/compat/library.cmake` | 157 |
| `tests/compat/pin.cmake` | 107 |
| `tests/compat/targets.cmake` | 15 |
| `tests/compat/tests.cmake` | 35 |
| `tests/consumer/CMakeLists.txt` | 15 |
| `tests/core/tests.cmake` | 33 |
| `tests/cpu/full.cmake` | 53 |
| `tests/cpu/targets.cmake` | 53 |
| `tests/cpu/test_reader.c` | 127 |
| `tests/cpu/tests.cmake` | 169 |
| `tests/diag/assemble.cmake` | 97 |
| `tests/diag/tests.cmake` | 119 |
| `tests/embed/CMakeLists.txt` | 41 |
| `tests/frame/tests.cmake` | 24 |
| `tests/fuzz/replay_main.h` | 105 |
| `tests/fuzz/tests.cmake` | 162 |
| `tests/header/header.c` | 51 |
| `tests/header/header.cpp` | 55 |
| `tests/host/tests.cmake` | 8 |
| `tests/hygiene/fixtures/titles.txt` | 31 |
| `tests/hygiene/tests.cmake` | 356 |
| `tests/input/tests.cmake` | 35 |
| `tests/install/tests.cmake` | 69 |
| `tests/inventory/ci.txt` | 613 |
| `tests/inventory/fuzz.txt` | 8 |
| `tests/inventory/hygiene.txt` | 26 |
| `tests/inventory/library.txt` | 47 |
| `tests/inventory/nofp.txt` | 15 |
| `tests/inventory/vectors-full.txt` | 260 |
| `tests/inventory/wasm32.txt` | 600 |
| `tests/libretro/audio.cmake` | 14 |
| `tests/libretro/tests.cmake` | 156 |
| `tests/maria/tests.cmake` | 18 |
| `tests/perf/allow.txt` | 6 |
| `tests/perf/tests.cmake` | 67 |
| `tests/policy/tests.cmake` | 98 |
| `tests/prop/tests.cmake` | 27 |
| `tests/release/firmware.txt` | 16 |
| `tests/release/tests.cmake` | 96 |
| `tests/riot/tests.cmake` | 21 |
| `tests/roms/manifest.txt` | 31 |
| `tests/runner/boot_to_play.cmake` | 79 |
| `tests/runner/tests.cmake` | 840 |
| `tests/tests.cmake` | 58 |
| `tests/vectors/6502-v1.sha256` | 256 |
| `tests/vectors/fixtures/02.json` | 6 |
| `tests/vectors/fixtures/a9-bad-a.json` | 3 |
| `tests/vectors/fixtures/a9.json` | 7 |
| `tests/vectors/n65v.c` | 209 |
| `tests/vectors/n65v.h` | 92 |
| `tests/vectors/sst_json.c` | 507 |
| `tests/vectors/sst_json.h` | 33 |
| `tests/vectors/vecconv.c` | 136 |
| `tools/cartdb.cmake` | 333 |
| `tools/compat_doc.cmake` | 80 |
| `version.txt` | 1 |

### Fault, consumer and control map

| Fault | Charged producer/declaration/control locations | Executing consumer |
|---|---|---|
| Private paths/bytes, identities, unsafe pins or asset admission | scripts/hygiene.sh, fetch-pin.sh, .githooks, pins.txt, tests/hygiene/tests.cmake, manifest/notices/vendored/pins/rom_hash_coverage judges, firmware.txt, ASSET_POLICY/PROVENANCE/notices, fixtures | Hooks, hygiene lane, firmware_scan and release consumer; exact digest/header-stripped/notice/identity controls |
| ABI/global/no-FP/platform floor regression | abi.cmake, floor.cmake, no_float.cmake, ABI fixtures/header sources, ABI registration, root build/toolchains/presets | Native, nofp and archive floor checks; impossible-floor/libc/global/double controls |
| Omitted, failed, duplicate or skipped test evidence | MuhtariTests, check_junit, no_skip/no_test_entries, tests/inventory, all registration glue and JUnit workflow invocations | Configure, executed-inventory checks and CI required; missing/skipped/failed controls |
| Silent fixture/vector/sample repin or forged evidence | cpu/full/targets/tests/test_reader, vectors conversion/reader/fixtures/hash list, built/diag/compat drivers, fetch_vectors/vectors_pin/sample_match | Native, WASM, full-vectors and hygiene; malformed/truncated/changed revision controls |
| Lost compatibility pass, stale docs or omitted hashes | compat_ratchet, compat_doc, hashes, compatibility allow/pin/registration, workflow hash counts | CI/quality, library and generated docs; status-flip/stale-page/hash difference controls |
| Unbounded/no-input work appears green | presets/timeouts, fuzz registration and replay_main, root sanitizer/nofp flags, audio registration | Native/sanitizer/fuzz and bounded workloads; all eight audio comparisons and no-corpus controls |
| Incorrect required skip/dependency, bot actor or release identity | ci_required/dependabot, classify-changes/check-title, workflow predicates, ruleset/dependabot declarations | CI required, trusted bot/release consumers; heavy/light/failure/actor controls |
| Missing/extra archive/member, altered notice or leaked path | archive_contents, tests/release/tests, packaging, LICENSE/notices | Actual package step then independent release checks; missing-notice plus bounded archive fault checks |
| Publication/assets/signer/settings/install claim drift | publish/apply-repo-settings, release workflow/config/version, install_line, install/embed/consumer glue | Rehearsal/publish/latest-install/settings; existing tamper/member/commit checks, later safety fixes allocated only after savings |

### Six-dimension baseline before implementation

Rubric: 1 = repeatedly broken or absent essential operation; 2 = usable with reproduced
reliability/cost gaps; 3 = functional with material evidence/coverage gaps; 4 = automated,
documented and bounded with a named remaining limitation; 5 = representative current
end-to-end evidence with no material observed gap. Scores are judgments tied to the evidence,
not new pass/fail thresholds.

| Dimension | Score | Evidence and limitation |
|---|---:|---|
| Performance | 3 | Existing perf comparator retains +1% arithmetic controls; actual arming remains absent and test-card-only (`tests/cmake/perf_compare.cmake`, `tests/perf/tests.cmake`, research Performance arming). |
| CI/CD | 2 | Reproduced freshness-output race, two audio timeouts, required BE 668/681 seconds and absent digest/aggregate; hosted failure below. Provisional weakest remains CI/CD. |
| Ease of use | 4 | Three shipped deliverables and published latest-install evidence: [release run](https://github.com/szTheory/muhtari/actions/runs/39533130210). Private ROMs intentionally supplied by users. |
| Architecture | 4 | Instance-owned integer bounded core, frozen API v1/v2/v3 fixtures and ABI/no-FP controls; source review of root build and archived milestone audit. Representative whole-library breadth remains future scope. |
| Docs/README | 3 | Concise installed-artifact instructions with docs budgets; research records stale hash/audio/lane/recovery claims requiring code/doc reconciliation. |
| Upgrade path/semver | 4 | Version-free archives, version.txt source and published v0.4.0 release; API version fixtures and release-please pre-1.0 policy. Signer/API handling gaps remain scoped safety work. |

### Comparable hosted before-change timing

Read-only commands: `gh run view RUN -R szTheory/muhtari --json jobs,attempt,headSha,url`
for each row, and `gh api repos/szTheory/muhtari/actions/runs/39533130206/jobs --paginate`:
exit 0. Data include attempt 1 only; these are matched before-change samples, not a
complete last-40 cohort or all-attempt monthly report. The two successful commits differ
in version/release inputs; diff over src/tests/workflows/cmake/presets/pins is empty, so
the execution workload is comparable. Their hosted runner image revisions/compiler versions
were not recovered here; runner labels are retained, not assumed identical images.
CMake presets/pins and the workflow define workload/tool identities.

| Run | Commit | Overall wall | Longest job | Executed job-seconds |
|---|---|---:|---:|---:|
| [39533130206](https://github.com/szTheory/muhtari/actions/runs/39533130206) | 40ce425099e1 | 693 s | BE 668 s | 1,519 |
| [39532369201](https://github.com/szTheory/muhtari/actions/runs/39532369201) | b058c5db50ce | 704 s | BE 681 s | 1,541 |

Runner work sums completed non-skipped jobs' completedAt-startedAt, including setup/teardown;
it is observed occupied job time, not billed minutes. Longest-job time is distinguished from
overall critical-path wall time including dependency/scheduling waits. For run 39533130206,
ubuntu-26.04 BE's workflow step takes 648 s; linux-x64 build/test/package takes 64 s.
The [newer failed run 39617761785](https://github.com/szTheory/muhtari/actions/runs/39617761785)
has 1,544 executed job-seconds and 671 s wall; macOS-x64 and Windows-x64 fail.
Failure durations are censored evidence, not successes used to size a timeout.
Queue time, complete rerun cost and current after-change hosted evidence are unavailable
at this slice. Local tool observations: CMake/CTest 4.4.3, Ninja 1.13.2.

## Fixed allocation after measured savings

The archive-name constructor replaces a four-line append loop with two documented CMake
list transformations. All comments, traversal, member/notice/path predicates, interfaces,
registration and existing controls are unchanged. The exact same 134-file inventory is
now **12,883** lines: actual **S = 12,885 - 12,883 = 2 > 0**. These are modest inherited
savings; no future deletion or operational exclusion is credited. Before and after,
the synthetic positive case and eight corrupt cases reject by exact exit and named fault:
missing/extra archive, missing/extra member, altered LICENSE, altered notices, source path
and build path. The one-off synthetic command is recorded below; all existing release
controls continue to run after actual packaging in the native workflow.

The finite inventory alone requires 134 entries, more than the research's optimistic
70–130 lines for the entire counter slice. The fixed allocation below makes that cost
explicit before adding machinery. It authorizes only accepted phase outcomes; inherited
rewrites receive zero allowance. Total **N = 1,970**, including a separately fixed 150-line
integration reserve, and **C = 12,885 - 2 + 1,970 = 14,853**. These are fixed incremental
physical lines including comments, controls, declarations and glue, not forecast savings.

| Component | Incremental allocation | Requirement/fault and charged paths |
|---|---:|---|
| Digest/monthly report | 650 | TUNE-01/02: complete attempts/calendar/cost, literal hostile data, safe preview and serialized publisher; scripts/digest.sh, tests/policy/digest.cmake, .github/workflows/digest.yml, tests/policy/tests.cmake, tests/inventory/ci.txt, tests/policy/gate-lines.txt |
| Quality evidence/ratchet | 450 | TUNE-03: typed actual bounds, trusted base/new-metric baseline, expiry/calendar/tampering controls; quality.json, tests/cmake/quality.cmake, tests/cmake/quality_evidence.cmake, tests/policy/quality.cmake, tests/policy/tests.cmake, tests/cmake/ci_required.cmake, tests/hygiene/tests.cmake, tests/inventory/ci.txt, tests/policy/gate-lines.txt, .github/workflows/ci.yml and release.yml |
| Count/inventory | 500 | TUNE-03: 134+ entry list, physical counter, self-inclusion/range controls, single initial quality metric and native registration/inventory; tests/cmake/gate_count.cmake, tests/policy/gate-lines.txt, tests/policy/tests.cmake, tests/inventory/ci.txt, quality.json |
| BE relocation/equality | 120 | TUNE-02/03: PR compile and full same-commit native/BE nightly equality, exact artifacts/inventory and missing/different controls; .github/workflows/ci.yml, nightly.yml and release.yml, CMakePresets.json, tests/cmake/lane_equality.cmake, tests/cmake/ci_required.cmake, tests/hygiene/tests.cmake, tests/policy/tests.cmake, tests/inventory/ci.txt, tests/policy/gate-lines.txt |
| Reproduced safety/correctness | 100 | TUNE-02/05: independent freshness writer, prepublish signer restriction, explicit API/404 handling and exact controls; tests/policy/tests.cmake, scripts/publish.sh, tests/policy/publish.cmake, .github/workflows/release.yml, tests/inventory/ci.txt, tests/policy/gate-lines.txt |
| Integration reserve, included in N | 150 | TUNE-01/02/03/05 only: cross-platform CMake/sh/jq arguments, trusted-base artifacts, release-called/direct CI identity, nightly/full-BE package joins and all-attempt hosted joins |
| Total N | 1,970 | Fixed total; unused scope cannot fund another capability |

Count/inventory's 500 lines reserve approximately 150 for the hand-maintained list,
170 for counter/report validation, 150 for fixture controls, and 30 for declaration/glue.
Any visible within-N redistribution must identify the receiving fault/path and unchanged
total; C cannot rise. At phase acceptance retire unused reserve and tighten to actual.

Current fixed redistribution, before the remaining quality integration: count368,
BE205, digest447, quality920, reproduced fixes30, reserve0; N1970/C14853 unchanged.
The prior quality850/fixes100 mapping transfers only unused fixes70 to exact
artifact joins, omission/tampering controls and release permission/roll-up glue.
Task2 quality actual711 leaves139 under850, below the remaining readable
integration need. Reproduced fixes actual30 is retained in full. No control,
counting boundary, scope, numeric metric or ordinary baseline is relaxed.
Cross-platform archive/floor/firmware semantics, trusted-base/new-metric availability,
inherited expiry, exact full-BE package inventory, release-call identities and all attempts
remain named integration obligations, not claims resolved by this first slice.

Task 1 verification: `cmake --workflow --preset ci` before and after, exit 0:
605 regular tests and six post-package release checks. `python3` synthetic archive
driver with `tests/cmake/expect_fail.cmake`, exit 0 before and after: one positive and
eight exact-fault negatives. These commands are local and have no hosted run URL.
Existing behavior was already green before the refactor; no artificial RED assertion
was invented for unchanged semantics. Task 2 requires genuine counter RED evidence.

## Task completion and final observation

1. Task 1: `17b95db` — simplify archive-name construction with unchanged fault judges.
2. Task 2 RED: `d6e4000` — physical-count fixture fails on expected 9 versus actual 0.
3. Task 2 GREEN: `16ad839` — implement counter, declare complete inventory and initial metric.

`ctest --preset ci -R '^policy[.]gate_count[.]controls$' --output-on-failure` — exit 0.
Controls cover comments/blanks, whole files, numeric ranges, unterminated final lines,
both source denominators, missing files, empty inventory, duplicate/overlapping/full-file
ranges, canonical path aliases, invalid/reversed endpoints and omitted counter/inventory.
Every invalid-input control uses `expect_fail.cmake` with an exact status and fault regex.

`cmake -DROOT=. -DINVENTORY=tests/policy/gate-lines.txt -DOUT=build/ci/gate-count.json -P tests/cmake/gate_count.cmake`
— exit 0: **13,251** physical lines; source **8,577 / 6,530**, advisory **154.49% / 202.92%**.
Report carries HEAD, configuration digest and integer millionth ratios; a missing source
denominator is null, never a false compliance result. Broad sensitivity is **13,310**.
The report is an observation; the later aggregate connects trusted-base enforcement.

Count slice actual: **368 / 500** incremental lines, leaving **132** in that component.
No reserve was spent or transferred. Phase headroom is **1,602** under C; it remains
allocated scope, not evidence that the remaining plans already fit or have completed.
The 368-line increment includes 119 counter lines, 139 manifest lines, 12 quality lines,
97 control/registration lines and one native inventory line. All 137 resulting files
are whole-file inclusions. No source or line-density padding was added.

Final `cmake --workflow --preset ci` — exit 0: **606 regular tests and six post-package
release checks**. `scripts/hygiene.sh --tree` and `git diff --check` — exit 0.
Local checks have no hosted URL. Hosted before-change evidence is linked above; this
plan makes no after-change hosted latency, phase acceptance or lane-promotion claim.

## TDD Gate Compliance

Task 1 is a semantics-preserving refactor: pre-edit expected archive controls already
pass; existing production controls are retained unchanged. It has no fabricated RED.
Task 2's JUnit record is generated by its named failing target: expected physical count
9, actual 0, CTest exit 8. `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence
build/ci/policy/gate-count-red.json --raw` — exit 0, `RED_EVIDENCE_OK`; semantic inspection
confirms the intended count assertion executed. The stub is fully replaced before GREEN.
The unchanged JUnit/evidence record remains ignored build output, not committed logs.
GREEN uses a `test:` commit to respect the repository's pipeline-only release policy;
it does not create a product feature or release bump.

## Decisions and deviations

- Whole-file conservative expansion exceeds historical partial counts and makes every
  admission/control/glue cost visible. Research ranges were estimates; the finite list
  requires a fixed 500-line count allocation before implementation, within fixed N.
- TUNE-02/03/05 are contributed to here, not globally checked complete: later plans
  contain their remaining observable contracts. This prevents premature scope claims.
- No behavior-changing archive policy was introduced, so the preexisting archive tests
  remain the recurring controls. The nine-case scratch driver verifies unchanged faults
  once before/after; it creates no maintained production harness or free source allowance.

## Known stubs and threat surface

No goal-blocking stub, skipped test or unrun plan verification remains. No new endpoint,
auth path or schema trust boundary is introduced. The local counter reads only explicitly
declared contained paths and writes its requested report; path escape and alias checks
prevent accounting outside the declared root. No emulator behavior or fixture bytes change.

## Next plan readiness

05-02 can use the frozen baseline, exact fixed allocations and real count observation.
No whole-phase completion is claimed. The original TUNE-03 edge classifier remains
unclassified; this counted metric does not silently resolve that planning assumption.

## Self-Check: PASSED

All three created artifacts and this summary exist. All three task commits are ancestors
of HEAD. The persisted plan ledger measures three commits from the frozen starting HEAD
through the recorded plan_head_after. There are no accidental tracked deletions.

