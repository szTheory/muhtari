# Phase 14 host and resource evidence

## Source-bound attempt

- **Implementation source:** `1e8f0195890940921c8a1ed8fb633d3114d755c1` (`feat(14-05): persist source-bound RetroArch host receipts`).
- **Workflow event:** `workflow_dispatch`, branch `phase/14-proven-frontend-states-and-responsive-play`, input `phase14_host=true`, attempt 1.
- **Run:** [GitHub Actions run 41585416888](https://github.com/szTheory/muhtari/actions/runs/41585416888).
- **Dispatch command:** `gh workflow run ci.yml --ref phase/14-proven-frontend-states-and-responsive-play -f phase14_host=true` — exit 0; invoked once.
- **Selection/authentication:** `gh run list --workflow ci.yml --branch phase/14-proven-frontend-states-and-responsive-play --event workflow_dispatch --limit 10 --json databaseId,headSha,event,status,conclusion,url,jobs` identified this single run at the source SHA above. `gh run watch 41585416888 --exit-status` — exit 1; run completed with failure. API run metadata reported the same SHA, event, URL, and attempt.
- **Artifacts:** `gh api repos/szTheory/muhtari/actions/runs/41585416888/artifacts` — exit 0, `total_count: 0`. There are no same-run core, runner, library, frontend, receipt, screenshot, or save artifacts to authenticate or inspect.
- **Host result:** the `retroarch` job was skipped with no steps. RetroArch was not launched, and the host sandbox, path/port containment, child ownership, version, fixture CRC, save/load, rewind, run-ahead, committed bytes, progress, image, frame, and timing observations were not reached. The skipped job is not evidence of a passing or failing host behavior.
- **Frontend claims:** FRONT-02, FRONT-03, and FRONT-04 remain pending. The local fixture/control tests exercise in-process behavior but cannot establish actual RetroArch behavior. The automatic probe report remains unchanged: FRONT-02 boundary, precision, idempotency, and concurrency are unresolved; FRONT-03 and FRONT-04 remain unclassified. No actual-host qualification is made.
- **Phase 12:** no Phase 12 operation or allowance renewal occurred.

## Local verification

Commands were run against the final local tree after the documentation and formatting updates:

| Command | Exit | Result |
| --- | ---: | --- |
| `cmake --workflow --preset ci` | 0 | Full local workflow passed, including 693 ordinary tests, package generation, and six release tests. |
| `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` | 0 | Ordinary inventory complete: 693 tests, each once. |
| `cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake` | 0 | Release inventory complete: six tests, each once. |
| `ctest --preset ci -R '^retroarch\.states\.controls$' --no-tests=error --output-on-failure` | 0 | Local receipt/control test passed on the final tree. Its required TDD RED run earlier exited 8 for the intended missing source-bound receipt; the RED evidence classifier accepted the failure. |
| `ctest --preset ci -R '^docs\.budgets$' --no-tests=error --output-on-failure` | 0 | Documentation budget passed. |
| `actionlint -shellcheck='' -ignore 'label "ubuntu-26\.04' -ignore 'label "ubuntu-26\.04-arm' .github/workflows/ci.yml` | 0 | Workflow syntax check passed. |
| `git diff --check` | 0 | No whitespace errors. |
| `/opt/homebrew/opt/llvm@21/bin/clang-format --dry-run --Werror --lines=3046:3086 tests/retroarch/states.c` | 0 | Changed host-observation output lines pass formatting. |
| `cmake --workflow --preset hygiene` | 8 | 29/30 hygiene checks passed. `hygiene.format` still reports pre-existing formatting findings in untouched portions of `tests/retroarch/states.c`; this plan’s changed output lines were formatted in commit `67319b4`. |

The first cold parallel local CI run exited 8 after seven short BIOS/title tests timed out. A serial diagnostic of those tests and fixture dependencies passed 10/10; subsequent full workflow runs passed, including the final run above. No timeout was changed. Hygiene initially could not configure because pinned MAME XML inputs were absent; fetching the exact pinned files with `scripts/fetch-pin.sh` resolved that setup issue without installing packages.

The prior Phase 13 local sandbox preflight reported `sandbox_apply: Operation not permitted`. No local RetroArch launch was attempted. The hosted job did not reach its sandbox containment checks, so containment remains unproved and there is no authorization basis for a frontend launch.

## Hosted run diagnosis and resource use

The macOS arm64 build, tests, and packaging completed, but the quality-observation step failed before artifact upload. The comparison baseline resolved to the immediate parent, `e638c53`, which is the required TDD RED commit and intentionally lacks the GREEN receipt implementation; `retroarch.states.controls` therefore fails against that baseline. The quality matrix failed on Linux x64, Linux ARM, macOS x64, and macOS ARM, suppressing the quality artifacts and causing the dependent RetroArch job to skip. This baseline selection issue is recorded as lesson MU-CI-015.

Other observed failures were existing portability/format findings outside this plan’s changed behavior: Windows x64 and arm64 fail to compile `clock_gettime`/`CLOCK_MONOTONIC` in `tests/retroarch/states.c:130`; the Linux clang ASan build fails because `realpath` is undeclared under strict C17 at the pre-existing source location; hygiene reports formatting violations elsewhere in `tests/retroarch/states.c`. The quality rollup also fails because it requires the complete artifact set. The current-source receipt control passed. `CI required` failed; hash-equality, wasm32, RetroArch, and title jobs were skipped. No jobs or steps were cancelled.

The run was created at `2026-11-03T05:41:57Z` and completed at `2026-11-03T05:53:55Z` (718 seconds). Queue and elapsed job durations from the authenticated run:

| Job | Queue | Duration | Result |
| --- | ---: | ---: | --- |
| changes | 3s | 7s | success |
| hygiene | 3s | 53s | failure |
| title | 2s | — | skipped |
| perf | 12s | 23s | success |
| be32 | 12s | 52s | success |
| nofp ubuntu-arm | 14s | 16s | success |
| build windows-x64 | 13s | 41s | failure |
| library | 12s | 51s | success |
| build macos-x64 | 13s | 689s | failure |
| asan | 12s | 19s | failure |
| nofp ubuntu | 13s | 22s | success |
| build linux-x64 | 12s | 342s | failure |
| build windows-arm64 | 12s | 58s | failure |
| build linux-arm64 | 14s | 202s | failure |
| build macos-arm64 | 12s | 251s | success |
| quality | 705s | 8s | failure |
| hash-equality | 703s | — | skipped |
| wasm32 | 703s | — | skipped |
| retroarch | 703s | — | skipped |
| CI required | 714s | 3s | failure |

The critical path was `build (macos-x64)` at 689 seconds, including roughly 350 seconds of build/test/package work and 324 seconds waiting/running quality observation. Most ordinary job queues were 2–14 seconds; the quality gate waited 705 seconds before failing. There were zero cancelled jobs or steps. This single sample supports no safe structural, timeout, or cache change; none was made. Fixed limits remain five minutes for the job, 50 seconds for the child, 45 seconds of work plus cleanup, 120 frames, and 64 explicit forward actions.

## Follow-up boundary

The run covers only source `1e8f019`; later documentation and formatting commits are not covered by it. Its original one-dispatch allowance is spent. On 2026-11-03 the owner separately authorized one bounded Phase 14 gap-closure dispatch; it was used once as run `41618546347`, documented below. Phase 12 remains halted. No retry or local frontend launch is authorized.

## Plan 14-06 local gap-closure preflight

Owner authorization on 2026-11-03 permits one additional `workflow_dispatch` with `phase14_host=true`, after the planned source, local, review, exact-parent, and publication preconditions pass. The original failed run above and Phase 12 allowance remain unchanged. No local RetroArch process was launched.

The repair is limited to declaring POSIX `realpath` under strict C17 and compiling the monotonic clock helper only where its POSIX-only callers exist, plus pinned clang-format output in the four existing test sources. No controller, containment, host behavior, assertion, timeout, preset, or workflow topology changed. The independent read-only gap-diff review passed: the only semantic changes are `_XOPEN_SOURCE 700` on non-Windows/non-WASI and the `STATES_POSIX` guard; callers of the helper are POSIX-only; remaining changes are formatting.

Local commands run on macOS arm64 against the repaired source:

| Command | Exit | Result |
| --- | ---: | --- |
| `cmake --workflow --preset ci` | 0 | Passed; 693 ordinary and six release tests passed, including compilation of the four changed C sources. |
| `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` | 0 | Ordinary inventory complete: 693 tests. |
| `cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake` | 0 | Release inventory complete: six tests. |
| `ctest --preset ci -R 'retroarch\\.states' --no-tests=error --output-on-failure` | 0 | Focused state controls passed; these use mocks and do not launch RetroArch. |
| `cmake --workflow --preset hygiene` | 0 | Passed all 30 hygiene checks. |
| `cmake -DJUNIT=build/hygiene/junit.xml -DINVENTORY=tests/inventory/hygiene.txt -P tests/cmake/check_junit.cmake` | 0 | Hygiene inventory complete: 30 checks. |
| `git diff --check` and pinned clang-format 21 dry run for the four changed sources | 0 | No whitespace errors; changed sources meet the pinned formatter. |
| `PATH="$PWD/build/phase14-tools:$PATH" cmake --workflow --preset asan` | 130 | Build completed, then macOS arm64 test processes hung before test output and were interrupted after repeated timeouts. |
| `ctest --test-dir build/asan -R '^retroarch\\.states\\.controls$' --parallel 1 --no-tests=error --output-on-failure` | 8 | Focused ASan control timed out after 15.03 seconds while running `--fault-controls`. |

ASan diagnosis: the instrumented unrelated `build/asan/tests/muhtari-test-api-v1` also hung with no output before `main`; the state binary emitted only `AddressSanitizer: libc interceptors initialized` before hanging. `ASAN_OPTIONS=detect_leaks=0` did not change the hang. The exact workflow reproduced timeouts outside the sandbox as well, including unrelated `input.cart_files`, `runner.boot_to_play`, and `basic.*` tests, so sandbox process-enumeration restrictions do not explain the failure. No source or preset was changed in response. Per the owner's 2026-11-03 decision, local macOS arm64 ASan is treated as unavailable; the single hosted Ubuntu x64 ASan job must pass before the host result can be accepted.

The bounded ASan commands above did not pass locally. Their failure is a platform limitation, not evidence that the repaired code passed ASan. The exact immediate-parent checkout was performed: CI, hygiene, and all three inventories passed after diagnosing one cold parallel BIOS timeout, and only local ASan was omitted for the documented macOS arm64 limitation. The hosted Ubuntu x64 ASan job passed in run `41618546347`; its wider workflow failed and the host job was skipped, so the single gap-closure allowance is spent without actual-host evidence.

### Repair commit and frozen-source preflight setup

- **Repair commit:** `a51732e` (`fix(14-06): restore portable frontend test builds`); it contains only the four repaired/formatted test sources and this local evidence. Its first parent is `b338797847ba4a6a4c70d1318e20a6b0c1800605`.
- **Review:** independent read-only gap-diff review passed before commit; the exact finding summary is recorded above.
- **Preflight state at evidence commit:** no open PR targeted this branch; GitHub CLI authentication was available; the only prior branch workflow run was the preserved failure `41585416888`; remote `ee5067c12b9ca9138af6de57d52e5210d181f38c` was an ancestor of local `b338797`.
- **Dispatch state at evidence commit:** the repair had not yet been pushed or dispatched. The next commit's immediate parent was the repair commit above; that exact detached parent checkout was subsequently verified and its receipts are saved in ignored `build/phase14-dispatch-evidence/`.

## Plan 14-06 bounded dispatch result

- **Owner authorization:** one additional source-bound Phase 14 attempt on 2026-11-03, with no change to the original failed run or Phase 12 allowance.
- **Pre-dispatch source:** evidence commit `0869c655008fc5344c428313ae519a91f5e9db42`; its immediate parent was `a51732ebb97fb66f4b02b861a0931f7aad51f3c7`. A detached checkout of that exact parent passed the warmed `cmake --workflow --preset ci` (693 ordinary and six release tests), CI/release inventories (693/6), `cmake --workflow --preset hygiene` (30 checks), and hygiene inventory (30). The first cold parallel CI run exited 8 on `bios.splash_skip`, `bios.rejects_cart`, and `bios.k_window`; their serial diagnostic plus fixtures passed 5/5, and the full warmed workflow then passed. No source or timeout was changed. Parent JUnit receipts and this diagnosis are saved in ignored build output. The required Ubuntu ASan lane remained for the dispatch.
- **Push:** one normal push, command `git push origin phase/14-proven-frontend-states-and-responsive-play` — exit 0. Immediately afterward `git ls-remote origin refs/heads/phase/14-proven-frontend-states-and-responsive-play` returned `0869c655008fc5344c428313ae519a91f5e9db42`, exactly matching the frozen local HEAD. No PR was open, no conflicting run was active, and `.github/workflows/ci.yml` only triggers on pushes to `main`, so this branch push created no extra CI run.
- **Dispatch:** `gh workflow run ci.yml --ref phase/14-proven-frontend-states-and-responsive-play -f phase14_host=true` — exit 0, invoked exactly once. The new run is [GitHub Actions run 41618546347](https://github.com/szTheory/muhtari/actions/runs/41618546347), event `workflow_dispatch`, branch `phase/14-proven-frontend-states-and-responsive-play`, source SHA `0869c655008fc5344c428313ae519a91f5e9db42`, attempt 1. The pre-dispatch run list contained only the original failed run `41585416888`; there is no second dispatch or job rerun.
- **Terminal result and audit:** `gh run watch 41618546347 --exit-status` — exit 1. The complete attempt/status/conclusion/head SHA/URL/jobs/steps response is saved as ignored `build/phase14-gap-run.json`; raw API and compact summary are beside it. `cmake -DINPUT=build/phase14-gap-run.json -DEXPECTED_SHA=0869c655008fc5344c428313ae519a91f5e9db42 -P tools/check_ci_run.cmake` — exit 1 because the required aggregate was not completed/success. The exact job/step failures were inspected from run logs; there were no cancelled jobs.

| Job | Start offset from run start | Duration | Result |
| --- | ---: | ---: | --- |
| changes | 3s | 16s | success |
| hygiene | 3s | 89s | success |
| title | 2s | — | skipped (non-PR event) |
| nofp (ubuntu-26.04) | 21s | 11s | success |
| asan (ubuntu-26.04) | 21s | 351s | success; `cmake --workflow --preset asan` and inventory passed |
| nofp (ubuntu-26.04-arm) | 24s | 27s | success |
| library | 21s | 48s | success |
| build (windows-arm64) | 21s | 48s | failure |
| build (macos-arm64) | 32s | 335s | success |
| be32 | 21s | 36s | success |
| perf | 21s | 28s | success |
| build (linux-x64) | 26s | 344s | success |
| build (windows-x64) | 22s | 48s | failure |
| build (macos-x64) | 23s | 621s | success |
| build (linux-arm64) | 31s | 214s | success |
| quality | 646s | 6s | failure: `quality_join: exact artifact set required` |
| wasm32 | 645s | — | skipped after failed dependency |
| retroarch | 645s | — | skipped before host execution |
| hash-equality | 645s | — | skipped after failed dependency |
| CI required | 654s | 2s | failure: required jobs did not all succeed |

The run lasted 657 seconds. The API exposes start time and duration but not a separate runner `queued_at` for each job, so the table's start offset includes both dependency wait and runner queue. The critical path was the macOS x64 job: its quality-observation step ran from 10:26:48Z to 10:31:45Z (297 seconds) after its 301-second build/test/package step, followed by the six-second quality join and the required aggregate. Ordinary jobs started 21–32 seconds after the run began. The run had zero cancelled jobs or steps. This single failed sample supports no safe change to topology, cache, or deadlines; none was made.

Both Windows build jobs fail compiling `tests/retroarch/states.c`: MSVC `/W4 /WX` reports C4100 for the unused `self_path` parameter at lines 1176, 1292, and 1453, promoted to C2220. Their build artifacts are therefore absent, and the quality join rejects the incomplete producer set. The required aggregate fails. The dispatch command set `phase14_host=true`, but `retroarch` was skipped because its required build/quality dependencies failed; no host step, containment probe, RetroArch launch, or actual state/run-ahead/rewind observation occurred.

The artifact API returned 14 artifacts. Same-run `archives-macos-arm64` (artifact 13223615092) and `hashes-macos-arm64` (artifact 13223081611) were available and downloaded with exit 0. The archive's `commit.txt` equals the frozen source SHA. The macOS arm64 archive SHA-256 values are library `7aedd1e651b3d670a551d64096344d5b4bd8a1270570b725372598b6240d7b09`, libretro `aceb29604b092eaec32ed6dbb633f6fa84c05da9de637c5d2de3dbfb077a5185`, and runner `20aa096f47bda970888c1b151f339f365cdd61e48c928ee49c30c4993ac28981`. No `retroarch-frames` artifact exists, so there is no host receipt, screenshot, or save to authenticate or inspect.

FRONT-02, FRONT-03, and FRONT-04 remain pending. The local fixture, mock controls, native contracts, parent checkout, and hosted ASan do not establish actual RetroArch behavior. The one additional allowance is consumed; do not dispatch again, rerun a job, or launch RetroArch locally. Closing the Windows warnings and obtaining fresh host evidence require a new separately authorized scope.

## Plan 14-07 warning repair and local preflight

The owner separately authorized this Phase 14 gap-closure scope on 2026-11-03 after run `41618546347`: one ordinary push of the frozen Phase 14 branch and one explicit `workflow_dispatch` with `phase14_host=true`, after all source and exact-parent gates pass. This is a new allowance distinct from 14-06. At Task 1 completion, the new push and dispatch remain unused. Phase 12 remains halted; runs `41585416888` and `41618546347` and their records remain unchanged.

The only source change is `(void)self_path;` in the existing non-POSIX branches of `run_controls`, `run_complete_workload_control`, and `run_engine_control` in `tests/retroarch/states.c`. Their POSIX path checks, path copies, child spawning, protected-fixture checks, mock assignments, assertions, and fault cases are unchanged. A focused read-only review of this exact diff passed with zero findings and zero fixes; this consumed no review-fix round and did not modify the preserved phase review or verification.

Current-source checks on macOS arm64:

| Command | Exit | Result |
| --- | ---: | --- |
| `cmake --workflow --preset ci` | 0 | All five workflow stages passed; 693 ordinary tests and six release tests passed. |
| `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` | 0 | Ordinary inventory complete: 693 tests, each once. |
| `cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake` | 0 | Release inventory complete: six tests, each once. |
| `cmake --workflow --preset hygiene` | 0 | All 30 hygiene checks passed, including pinned formatting. |
| `cmake -DJUNIT=build/hygiene/junit.xml -DINVENTORY=tests/inventory/hygiene.txt -P tests/cmake/check_junit.cmake` | 0 | Hygiene inventory complete: 30 checks, each once. |
| `ctest --test-dir build/ci -R '^retroarch\.states\.controls$' --no-tests=error --output-on-failure` | 0 | The existing fixture/controller mock controls passed; no RetroArch process was launched. |
| `git diff --check` | 0 | No whitespace errors. |

Local macOS arm64 ASan remains unavailable after the already documented pre-main hangs in unrelated instrumented binaries. It was not retried; the new hosted Ubuntu x64 ASan result remains required for acceptance.

### Plan 14-07 exact-parent and publication preflight

The repaired source commit is `234c81abd5bb80b72b56c851b4031867a9a236cc` on `phase/14-proven-frontend-states-and-responsive-play`. An independent detached checkout at that exact commit was created at `.worktrees/phase-14-gap07-parent`; its tracked tree is clean. This is the verified immediate-parent candidate for the separate evidence-only dispatch commit. `tests/cmake/quality_evidence.cmake` selects `HEAD^1` for `workflow_dispatch`; the evidence-only commit will therefore resolve to this independently tested commit, and the frozen dispatch identity will record both hashes.

The first cold parallel exact-parent `cmake --workflow --preset ci` exited 8. Its JUnit and CTest failure receipts are preserved under ignored `build/phase14-gap07-evidence/parent-ci-cold/`. Sixteen otherwise passing fixture-backed cases timed out under the initial cold parallel load: `frame.sample_renders`, six `bios.*` cases, four `title.*` cases, and five `maria.dma_line.*` cases. The failed JUnit inventory consumer also exited 1 and identified those same 16 failures. No assertion failure or source warning was reported. A bounded serial diagnostic covering those cases and their fixture prerequisites passed 19/19 with exit 0; the exact captured command and output are in ignored `parent-ci-serial-diagnostic.log`. After those passes warmed the build and fixtures, the full exact-parent `cmake --workflow --preset ci` passed with captured exit 0: all 693 ordinary tests, package generation and all six release tests completed. No timeout, source behavior or CI policy was changed.

Remaining exact-parent checks, all run after the successful full workflow and before focused CTest could replace test receipts:

| Command | Exit | Result |
| --- | ---: | --- |
| `cmake --workflow --preset ci` (warm exact-parent run) | 0 | All required stages passed; 693 ordinary tests and six release tests passed. |
| `cmake --workflow --preset hygiene` | 0 | All 30 hygiene checks passed. |
| `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` | 0 | Independent ordinary inventory: 693 tests, each once. |
| `cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake` | 0 | Independent release inventory: six tests, each once. |
| `cmake -DJUNIT=build/hygiene/junit.xml -DINVENTORY=tests/inventory/hygiene.txt -P tests/cmake/check_junit.cmake` | 0 | Independent hygiene inventory: 30 checks, each once. |
| `ctest --test-dir build/ci -R '^retroarch\.states\.controls$' --no-tests=error --output-on-failure` | 0 | Exact-parent mock-only state controls passed; no RetroArch process was launched. |

The complete warm CI output and each command's captured exit are retained in ignored `build/phase14-gap07-evidence/parent-*.log`. The exact-parent checkout's own build directory generated the JUnit files; no prior checkout receipt was reused. No local macOS arm64 ASan run was repeated. These local results establish neither Windows compiler acceptance nor hosted ASan.

Read-only publication checks on 2026-11-03: `gh auth status` succeeded (exit 0); `gh pr list --head phase/14-proven-frontend-states-and-responsive-play --state open --json number,url,state,headRefName,baseRefName` returned `[]` (exit 0); the branch workflow listing contained only preserved, completed dispatch runs `41585416888` and `41618546347`, with no active run (exit 0). `git ls-remote origin refs/heads/phase/14-proven-frontend-states-and-responsive-play refs/heads/main` showed branch SHA `0869c655008fc5344c428313ae519a91f5e9db42`; it is an ancestor of the local repair commit and there is no remote-only branch commit. `.github/workflows/ci.yml` triggers `push` only on `main`, while `workflow_dispatch` exposes the boolean `phase14_host` input. The authorized new push and dispatch remain unused at this preflight point. These remote and PR checks will be repeated immediately before the single push.
