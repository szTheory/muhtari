---
phase: 10-homebrew-expansion-memory
plan: "09"
subsystem: testing
tags: [cmake, diagnostics, provenance, equality, powerpc]
requires:
  - phase: 10-07
    provides: Immutable owned BankSet template and independent geometry/native judges
  - phase: 10-08
    provides: Immutable owned expansion template and independent RAM/HSC/POKEY/host judges
provides:
  - Complete declared BankSet and expansion observations in the production equality writer
  - Actual numeric and independently judged native/raw/PCM writer fault controls
  - Source-authenticated exact six-native and native/PowerPC artifact consumers
affects: [10-13, native-equality, release-evidence]
tech-stack:
  added: []
  patterns:
    - Existing diagnostic owners judge actual observations before canonical equality emission
    - Existing artifact identities are checked against the requested immutable Git object
key-files:
  created: []
  modified:
    - tests/cmake/hashes.cmake
    - tests/diag/tests.cmake
    - tests/policy/tests.cmake
    - tests/runner/tests.cmake
    - tests/cmake/lane_equality.cmake
    - .github/workflows/ci.yml
    - docs/testing.md
    - docs/maintaining.md
key-decisions:
  - Keep semantic judgment in the real diagnostic producer and exact structural/provenance checks in artifact consumers.
  - Give overlapping page-sweep and sound-gate runs distinct declared case names without dropping either workload.
  - Authenticate matching artifact identities against the immutable source object rather than agreement alone.
requirements-completed: []
coverage:
  - id: D1
    description: Production BankSet observations cover all sixteen declared geometry and regional groups after independent numeric and native judgment
    requirement: BOARD-04
    verification:
      - kind: integration
        ref: diag.bankset and runner.hashes_file, complete local required workflow
        status: pass
      - kind: integration
        ref: policy.lane_equality.controls actual production writer report faults
        status: pass
    human_judgment: false
  - id: D2
    description: Production expansion observations cover all sixty-six declared page, window, region, topology and gate groups after independent numeric, native, storage and audio judgment
    requirement: BOARD-05
    verification:
      - kind: integration
        ref: diag.expansion and runner.hashes_file, complete local required workflow
        status: pass
      - kind: integration
        ref: policy.lane_equality.controls CRC-valid numeric and valid-length digest faults
        status: pass
    human_judgment: false
  - id: D3
    description: Existing native and PowerPC consumers reject incomplete equal schemas and forged source, configuration, fixture, member or lane identities
    requirement: BOARD-05
    verification:
      - kind: integration
        ref: policy.lane_equality.controls real complete two-lane and six-native consumer fixtures
        status: pass
      - kind: integration
        ref: policy.quality.controls and policy.ci_run.controls
        status: pass
      - kind: other
        ref: Fresh quality_evidence BUILD_BASE at 27e74d1 versus genuine pre-plan 9ef358e
        status: pass
    human_judgment: false
actuals:
  tokens: 9362
  tasks: 2
  commits: 2
plan_head_before: 9ef358e53a7ffb114e39eea548741a436e826d4f
plan_head_after: 27e74d1249b19ddb247e253e2c29af6bf6b2a8d2
duration: 15min
started: 2026-10-26T20:44:50Z
completed: 2026-10-26
status: complete
---

# Phase 10 Plan 09: Judged expansion equality Summary

**The production writer retains all956 old observations and appends2132 independently judged BankSet/expansion rows; both required artifact consumers authenticate complete evidence against its immutable source.**

## Accomplishments

- The exact declared workload is sixteen BankSet groups and sixty-six expansion groups. Each group emits26 rows: twelve native frames, lag count, seven canonical audio fields, the independently judged numeric mailbox, source commit, source/configuration/template SHA256 and actual generated variant SHA256. The total is3088 unique ordered names. Page sweeps, sound-gate comparisons and undeclared-primary cases have distinct names, including deliberately overlapping input configurations.
- Existing diagnostic scripts still generate actual variants, execute the library and runner, judge numeric records/CRC/native pages/storage/raw sources/PCM/counts and compare actual host observations. The writer then checks the exact21-field report, reruns the real mailbox judge, compares the sixteen frame/raw/PCM/count observations with the independently executed library reference and only then returns canonical rows. NTSC/PAL counts are8861/11537, with behavior revision6 and unchanged synthesis metadata.
- Actual writer controls start from real generated reports and retain the real numeric driver/reference. Missing/truncated/extra/duplicate/reordered fields, wrong region/scenario, valid-length forged frame/raw/PCM digests, incorrect samples and every existing BankSet/expansion mailbox fault fail. This includes CRC-valid wrong numbers, count/order/identity/flags/reserved-data faults, leaked hidden HSC and wrong timer/page observations. Matching bad numeric reports cannot reach canonical emission through this path.
- Structural equality requires the complete declared schema rather than enumerating whatever output exists. Both-lane faults cover missing/extra/duplicate/reordered observations, wrong geometry/region, malformed values/digests and forged source/template/configuration identities. Numeric judgment remains the producer's independent authority; artifact-only equality does not reconstruct semantic truth from arbitrary matching fabricated hashes.
- The original writer extracted from genuine pre-plan9ef358e was executed separately. Its956 rows match the first956 current rows byte-for-byte, SHA25669083f92ebc92dab557839bff9428b0d5b3b53748b78d895a05f26e2ec66c992. No old predicate, fixture, source pin, audio annotation or raw manifest/sample authority changed here. The earlier narrow causal correction remains independently attributed to its original work.
- Existing six-native CI uploads now contain exactly commit.txt/hashes.txt/identity.txt; previously those uploads contained only commit.txt/hashes.txt. The existing native/PowerPC artifact already had those three members and retains them. Both consumers now reject matching forged identities against the requested Git object's existing fixture/configuration tree and full Release options. The native consumer additionally requires exactly the six existing lane names; canonical duplicate file paths fail. Native/PowerPC directories remain distinct and commit text is exact.
- Policy fixtures now invoke the actual complete production writer instead of manufacturing an abc-filled schema. Clean complete two-lane and six-native artifacts pass; missing/extra members/artifacts, wrong lane/source/configuration/fixture, empty/partial equal content, duplicate lanes/files and matching identity forgeries fail the production consumers. Existing quality producer/join controls remain authoritative and pass without rewriting quality_join or quality_evidence.
- All binary subprocesses retain the existing emulator forwarding. No job, lane, harness, dependency, framework, evidence registry or quota was added. API7/ABI1/behavior6, advisory WASM/property status and the two existing RetroArch cases remain unchanged.

## Task commits

| Task | Commit | Result |
|---|---|---|
| 1 | 4aeb460 | Actual judged memory reports, exact additive schema, production writer/validator controls and retained consumer count |
| 2 | 27e74d1 | Immutable source/configuration/fixture authentication for complete six-native and native/PowerPC artifacts |

The persisted per-plan ledger measures two task commits before this summary commit. Actuals are ceil(37447 realized diff characters/4)=9362 across eight task files, on the estimate's diff scale. TDD applicability and TDD mode were false; no RED or no-op implementation commit was invented. The parent owns shared STATE/ROADMAP/REQUIREMENTS updates; this executor did not modify them.

## Verification evidence

All run URLs below are **N/A-local**. Output files remain ignored build artifacts; no execution log is committed.

| Command | Exit | Evidence |
|---|---:|---|
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(diag[.](bankset\|expansion)$\|policy[.]lane_equality[.]controls$\|policy[.]quality[.]controls$\|runner[.]hashes_file$\|docs[.]budgets$)' --no-tests=error --output-on-failure` |0|Eight targeted checks including fixture dependencies; actual complete writer, numeric/native reports and fault controls pass; lane control23.05s |
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^policy[.](lane_equality\|quality\|ci_run)[.]controls$' --no-tests=error --output-on-failure` |0|Five checks including fixture dependencies; actual two-lane/six-native production consumers and unchanged independent quality controls pass |
| Original hashes.cmake extracted with `git show 9ef358e:tests/cmake/hashes.cmake`, then actual `cmake -DMODE=write ... -P build/ci/task10-09-original-hashes.cmake` |0|Independent genuine pre-plan956-row output |
| Python assertions over original/current complete output |0|All956 old bytes identical; exactly3088 unique current names |
| `sh build/ci/task08-jobserver-gate.sh`, before deadline calibration |0|661 ordinary plus six release checks, packages, repeated release checks, exact661-once inventory and tree hygiene; complete lane control27.42s |
| `sh build/ci/task08-jobserver-gate.sh`, final task file bytes |0|661 ordinary in85.63s; six release in0.18s and repeated six in0.19s; three actual ZIPs, exact661-once inventory and hygiene; lane control29.52s, writer19.52s |
| `sh build/ci/task10-09-quality.sh`, source27e74d1 against genuine9ef358e |0|Fresh existing recursive BUILD_BASE producer; current661+6/base661+6; base ordinary60.93s and release0.12s; source/environment/raw identities and measured sums agree |
| Current/base JSON, original raw identity and actual three-ZIP sum assertions |0|Exact current/base SHAs, matched AppleClang21/Darwin arm64 environment and original raw fixture/sample identities |
| Verified clean temporary base worktree removal |0|Only the producer-created9ef358e worktree was removed; JSON measurements retained locally |
| `actionlint -oneline -ignore 'label "ubuntu-26[.]04(-arm)?" is unknown' -ignore 'SC2017:' -ignore 'SC2059:' .github/workflows/ci.yml` |0|Changed workflow syntax checks pass after excluding exactly unchanged optional-tool diagnostics |
| Before/after workflow assertions |0|Same jobs and pinned uploads; the entire advisory/RetroArch portion is byte-identical |
| `git diff --check`, SDK evaluation-scope, task-file and commit ancestry assertions |0|Eight task files and both task commits exist; source tree clean before summary |

Live GitHub workflow run/with syntax was fetched from the official documentation before the YAML edit. The skill's ci_monitor.cjs is absent here; no CI API operation was needed. Existing repository controls and installed actionlint were used without adding a tool or package.

The existing lane-control registration now has a provisional55s deadline: ceil(2*27.42s) from the complete expanded local required workload under DEC.47. The subsequent final full run took29.52s and passed. Workload, assertions, membership and existing timeout scaling are retained. The actual writer keeps its existing58s deadline. Hosted calibration remains final acceptance work.

The quality wrapper uses the unchanged four-slot FIFO jobserver and invokes:

`GITHUB_EVENT_PATH="$PWD/build/ci/task10-09-event.json" GITHUB_EVENT_NAME=push cmake -DROOT=. -DBUILD=build/ci -DOUT=build/ci/task10-09-quality-current.json -DSHA="$(git rev-parse HEAD)" -DCONFIG=ci -DBUILD_BASE=ON -DSOURCE=local -P tests/cmake/quality_evidence.cmake`

Its genuine before is9ef358e53a7ffb114e39eea548741a436e826d4f; candidate is27e74d1249b19ddb247e253e2c29af6bf6b2a8d2. Both envelopes retain raw trusted manifest5d355f481b2aaf34f67a003c938a2a14a3d5e554455568d3ce9437901142c527 and raw sample d549d4691aca7c004325102613fd6ac2337ecfe8a4c0cec004472e5bbcc80ca8. This genuine pre-plan already contains both admitted diagnostics; it is distinct from the earlier pre-expansion comparison base.

| Actual macOS arm64 ZIP | Candidate bytes | Genuine pre-plan bytes |
|---|---:|---:|
| Library |83245|83214|
| Runner |98522|98522|
| Libretro |89625|89625|
| Three-component sum |271392|271361|

Archive bytes remain report-only under DEC.61. This is a fresh local per-plan comparison, not a phase-versus-main or all-platform performance claim.

## Failures retained

- The first writer attempt exited8 because the new conversion expected20 fields, while an actual movie-driven runner report includes lag_frames and has21. The final parser and schema retain that field. A later shell wrapper exited1 because it assigned zsh's read-only status variable; its captured CTest report exposed the old956-row wrapper assertion after successful3088-row emission. The wrapper uses a task-specific variable, and the real consumer count was corrected.
- The first combined task verification exited8: the generated control did not create its output directory, and testing.md exceeded its existing40960-byte cap by12 bytes. Directory creation and a shorter existing paragraph fixed both; final testing.md is40913 bytes with the original cap unchanged. The subsequent eight-check run passed.
- Unfiltered optional actionlint exited1 both before and after: its installed runner-label table lacks existing ubuntu-26.04 labels and it reports two unchanged RetroArch shellcheck diagnostics. Normalized before/after diagnostics are exactly equal; scoped actionlint exits0. No unrelated workflow code or required gate was changed to satisfy this optional tool.

## Deviations from plan

1. **[Rule2 - Required direct consumer]** tests/runner/tests.cmake had the exact956-row assertion in the actual production writer wrapper. Updated it to3088 after real output exposed the failure; historical accepted aggregate predicates remain intact. Commit4aeb460.
2. **[Rule2 - Required artifact identity]** The six-native workflow uploaded only two members and compared hashes without immutable fixture/configuration authentication. Narrowly changed its existing upload and comparison step to the exact three-member contract already used by native/PowerPC. The parent explicitly authorized this owner-path correction. No job/action/advisory/frontend membership changed. Commit27e74d1.

These changes satisfy current required consumers; they are not additional formal review or gap rounds. No stub, skipped test, unrun plan verification, authentication gate, new security trust surface or required local failure remains.

## Confidence and readiness

This plan contributes to BOARD-04/BOARD-05 and does not mark either requirement complete. The artifact-only consumers establish exact structure, equality and source/configuration/fixture provenance; independent hardware semantics are established by the real diagnostic producer before emission. Arbitrary matching fabricated digests alone are not claimed to prove hardware correctness.

Neither qemu-ppc nor powerpc-linux-gnu-gcc was available on PATH locally. Schema-scoped native/PowerPC controls and emulator forwarding are verified here; full actual hosted native/PowerPC execution, all six native artifacts and final exact-source CI audit remain mandatory in10-13. This is planned later acceptance, not omitted local plan verification. The selected XM compatibility model and larger BankSet extensions retain their prior physical confidence limits. No owner verification was added.

Within the current execution step, plan10-10 is ready. Next separate allowed command: `$gsd-execute-phase 10`. No other GSD step was launched.

## Self-Check: PASSED

All eight task files exist; both task commits are reachable and the SDK resolves exactly those two commits for10-09. The measured ledger endpoints, complete local workflow, unchanged old observations and fresh genuine-base producer are verified. Shared state files remain untouched.
