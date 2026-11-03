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

### Plan 14-07 single dispatch outcome

The repaired source commit was `234c81abd5bb80b72b56c851b4031867a9a236cc`. The evidence-only commit `27818674366f80e230b8fa44493e1046282c3a66` froze that repaired commit as its first parent; `tests/cmake/quality_evidence.cmake` confirms that `workflow_dispatch` compares against `HEAD^1`. The one authorized normal push, `git push origin phase/14-proven-frontend-states-and-responsive-play`, exited 0. A subsequent read-only `git ls-remote origin refs/heads/phase/14-proven-frontend-states-and-responsive-play` advertised the frozen SHA `27818674366f80e230b8fa44493e1046282c3a66` (exit 0).

Exactly one `gh workflow run ci.yml --ref phase/14-proven-frontend-states-and-responsive-play -f phase14_host=true` was issued (exit 0). It resolved unambiguously to [run 41629846188](https://github.com/szTheory/muhtari/actions/runs/41629846188), workflow `CI`, event `workflow_dispatch`, the Phase 14 branch, frozen source SHA `27818674366f80e230b8fa44493e1046282c3a66`, attempt 1. Its state is completed/failure. `gh run watch 41629846188 --exit-status` exited 1. The saved `gh run view --json attempt,status,conclusion,headSha,url,jobs` response is `build/phase14-gap07-evidence/run.json`; the full run, job, step and API receipts are under the same ignored evidence directory. The required source audit, `cmake -DINPUT=build/phase14-gap07-evidence/run.json -DEXPECTED_SHA=27818674366f80e230b8fa44493e1046282c3a66 -P tools/check_ci_run.cmake`, exited 1 because the required aggregate did not succeed. No retry or rerun occurred. Runs `41585416888` and `41618546347` remain preserved.

The warning repair is confirmed by both required hosted compiler lanes: `build (windows-x64)` and `build (windows-arm64)` succeeded, as did fresh hosted Ubuntu x64 `asan` (`cmake --workflow --preset asan` and its inventory step). Linux x64/ARM64 and macOS ARM64 builds, library, hygiene, big-endian and no-floating-point jobs passed. The macOS x64 build and its ordinary tests/package/inventory passed, but its `Quality observations and independent base` step failed: `cmake -DROOT=. -DBUILD=build/ci -DOUT=build/ci/quality-current.json -DSHA="$(git rev-parse HEAD)" -DCONFIG=ci -DBUILD_BASE=ON -DSOURCE="$GITHUB_SERVER_URL/$GITHUB_REPOSITORY/actions/runs/$GITHUB_RUN_ID" -P tests/cmake/quality_evidence.cmake` exited 1. Its independent-parent CTest run reached 518/693 and `fuzz.regress.run` timed out at 28.06 seconds; the CTest child exited 8. The `quality` job then failed at `quality_join: exact artifact set required` because the macOS x64 producer artifact was absent. The required aggregate failed. `retroarch` was skipped before host execution; no receipt, screenshot, save, containment observation, RetroArch launch or `retroarch-frames` artifact exists. `wasm32` and `hash-equality` were skipped after failed dependencies; `title` was skipped for this non-PR dispatch. There were no cancelled or unfinished jobs or steps.

The artifact API returned 17 artifacts, all associated with run `41629846188`, the exact branch and frozen source SHA. Same-run `archives-macos-arm64` (ID `13227896979`, digest `sha256:3b952c48a2a4edc2753061f8643773f2616c3cb94a87a34ccd2162fc64ab61ca`) and `hashes-macos-arm64` (ID `13227792037`, digest `sha256:df4d05a00d8e5397343d1946c8423dd914555e82b814d1c74c46dbde443409b9`) were downloaded by their names from that run (each download command exit 0). The hash artifact's `commit.txt` exactly matches the frozen source SHA. The package archive SHA-256 values are library `c8d8d6c8c7315c32ad2a5aa9bb6f467d966ed773461d81e78ac9d87ca7238f1d`, libretro `8c46c47c69847aea132c283f1add3f187ae5a980f5b04e80bc78f03b47a97c02`, and runner `b2dcd0fae806349b565cde5f7a075e0059a7e9d4b8d2c42993a4fdd093da681c`. The artifact API contains no `retroarch-frames` artifact; none was downloaded.

Run start was `2026-11-03T11:43:40Z`, and completion was `2026-11-03T12:21:02Z` (37m22s). The job API provides `started_at` and `completed_at` but no separate `queued_at`, so start offsets combine dependency wait and runner queue and cannot be decomposed. Durations below are job start-to-completion; the start offset is measured from workflow start:

| Job | Start offset | Duration | Result |
| --- | ---: | ---: | --- |
| changes | 3s | 5s | pass |
| hygiene | 4s | 127s | pass |
| be32 | 10s | 107s | pass |
| library | 10s | 49s | pass |
| asan | 11s | 586s | pass |
| nofp (ubuntu-26.04) | 10s | 14s | pass |
| build (windows-x64) | 10s | 1392s | pass |
| build (linux-arm64) | 20s | 208s | pass |
| nofp (ubuntu-26.04-arm) | 12s | 18s | pass |
| perf | 11s | 26s | pass |
| build (macos-x64) | 12s | 929s | fail in independent-parent quality baseline |
| build (macos-arm64) | 11s | 392s | pass; same-run artifacts available |
| build (linux-x64) | 10s | 347s | pass |
| build (windows-arm64) | 10s | 2216s | pass; critical path |
| quality | 2228s | 7s | fail: exact artifact set |
| wasm32 | 2226s | 0s | skipped |
| hash-equality | 2226s | 0s | skipped |
| retroarch | 2226s | 0s | skipped before host execution |
| title | 2s | 0s | skipped for non-PR event |
| CI required | 2237s | 4s | fail |

The Windows ARM64 lane was the critical path at 36m56s; the aggregate completed about 15 seconds later. The failing macOS x64 baseline step ran from `11:56:39Z` to `11:59:17Z` (about 2m38s), including the 28.06-second timed-out test. No jobs or steps were cancelled. One run does not establish a safe structural, timeout, or cache improvement; none is supported or proposed by this evidence. The exact-parent local warm CI and serial diagnostic results remain separately recorded above, and no local ASan retry was made.

This failed/partial result exhausts the one 14-07 dispatch allowance. FRONT-02/03/04 remain pending because the actual RetroArch host job did not run. The hosted Windows warning verdict is now passing for the repaired source; the historical failure in run `41618546347` remains unchanged. No post-run push, another dispatch, job rerun, local RetroArch launch, PR, merge or release is authorized in this scope.

The six spec-less candidates retain their existing classifications: FRONT-02 boundary, precision, idempotency and concurrency remain unresolved; FRONT-03 and FRONT-04 remain unclassified. The warning repair and skipped host job do not resolve them. Both recalled prohibitions remain flagged for evidence review: do not substitute callback/mock success for actual-host picture, progress, rewind, run-ahead and durable-storage observations; do not conceal lost committed battery progress or broaden claims beyond the demonstrated host/mode. Neither is silently passed or dismissed.

## Plan 14-08 scheduling preflight (2026-11-03)

The owner authorized this distinct cause-addressed scope: change only the normal portable replay registration, complete current-source and immediate-parent local gates and review, then permit at most one normal branch push and one fresh `workflow_dispatch` with `phase14_host=true`. Both new counters were unused at the end of Task 1. Plan 14-07's failed attempt and allowance remain spent. No host launch, push, dispatch, retry, cancellation, pull request, merge, release or Phase 12 action occurred during Task 1. FRONT-02/03/04 remain pending.

### Prior replay evidence and its limits

The trigger remains [run 41629846188](https://github.com/szTheory/muhtari/actions/runs/41629846188), dispatched source `27818674366f80e230b8fa44493e1046282c3a66` with immediate parent `234c81abd5bb80b72b56c851b4031867a9a236cc`. The macOS x64 current-source `Build, test, package` step passed (run API conclusion success; exit 0). Its portable `fuzz.regress.run` duration of 14.60 seconds is reported from the run's current-source test observation; that successful test's detailed log was not retained in `build/phase14-gap07-evidence/`, so this timing is not presented as a separate preserved command/exit receipt.

The same run's independent-parent command was `cmake -DROOT=. -DBUILD=build/ci -DOUT=build/ci/quality-current.json -DSHA="$(git rev-parse HEAD)" -DCONFIG=ci -DBUILD_BASE=ON -DSOURCE="$GITHUB_SERVER_URL/$GITHUB_REPOSITORY/actions/runs/$GITHUB_RUN_ID" -P tests/cmake/quality_evidence.cmake`; it exited 1. The retained `failed-logs.log` shows CTest launched `fuzz.regress.run` at 11:56:39.823Z while `fuzz.regress.loader` was running (7.90 seconds), later overlapping `install.consumer` (6.89 seconds), then timed out at 28.06 seconds. The CTest child exited 8. These observations establish overlap and a censored timeout, not the timeout's cause.

The local macOS x64 diagnostic `Testing/Temporary/LastTest.log` retains five successful `fuzz.regress.run` repetitions with 123 inputs at 3.17–3.21 seconds while loader work was also selected. The reported standalone 3.48-second replay is not accompanied by a retained invocation/exit. Treat these Rosetta timings as reported diagnostic evidence, not a hosted reproduction. The separate warm exact-parent receipt `parent-ci-warm.log` records a 3.03-second replay within a successful full CI workflow. Neither local result establishes why the earlier hosted parent timed out.

### Task 1: normal portable replay scheduling

The baseline was captured with `ctest --preset ci --show-only=json-v1` into ignored `build/phase14-gap08-evidence/source-before.json`; the normal `build/ci/CMakeCache.txt` had `MUHTARI_FUZZ=OFF`, `MUHTARI_TIMEOUT_SCALE=1`, and the metadata contained 693 tests. The one-time `cmake -P` property gate rejected that baseline with exit 1 and the specific message `fuzz.regress.run is missing RUN_SERIAL TRUE`.

The only tracked source change is the normal portable branch's `set_tests_properties(fuzz.regress.run PROPERTIES RUN_SERIAL TRUE)` immediately after its existing `--seeds` registration. Its full command remains `muhtari-fuzz-run-replay --seeds`, the embedded run corpus still reports 123 inputs, labels remain `fuzz;portable`, and TIMEOUT remains 14 before existing lane scaling. The libFuzzer branch, assertions, corpus, test names and all other registrations are unchanged.

The first draft of the one-time metadata checker was interrupted with exit 130 after repeated full-JSON parsing exceeded a minute. A simplified checker then exposed only the generated CTest backtrace line shifts caused by the inserted lines: registrations later in `tests/fuzz/tests.cmake` moved from source lines 210/224 to 212/226. The final checker normalizes those two generated source-location fields and the single intended serial property, while requiring the complete remaining JSON metadata to match. It is ignored build output, not a recurring repository check. After `cmake --preset ci` (exit 0), the fresh `ctest --preset ci --show-only=json-v1` metadata and `cmake -DROOT="$PWD" -DINPUT=build/phase14-gap08-evidence/source-after.json -DBASELINE=build/phase14-gap08-evidence/source-before.json -P build/phase14-gap08-evidence/check-portable-scheduling.cmake` passed (exit 0): 693 names and all remaining metadata match, with only the intended property and generated line offsets.

| Command | Exit | Result |
| --- | ---: | --- |
| `cmake --workflow --preset ci` | 0 | Complete local CI workflow passed, including ordinary tests, package generation and all release tests. |
| `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` | 0 | All 693 ordinary tests ran exactly once. |
| `cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake` | 0 | All six release tests ran exactly once. |
| `cmake --workflow --preset hygiene` | 0 | All 30 hygiene checks passed. |
| `cmake -DJUNIT=build/hygiene/junit.xml -DINVENTORY=tests/inventory/hygiene.txt -P tests/cmake/check_junit.cmake` | 0 | All 30 hygiene checks ran exactly once. |
| `ctest --preset ci -R '^(fuzz\.regress\.(run(\.self_test)?|loader)|install\.consumer|retroarch\.states\.controls)$' --no-tests=error --output-on-failure --output-junit build/ci/junit-gap08-focused.xml` | 0 | Six tests passed, including the automatic `install.stage` fixture setup, empty-corpus guard, loader, install consumer, mock-only frontend control and 123-input replay. The replay took 1.96 seconds and started after the other selected tests had completed, so no selected CTest task overlapped it. The focused JUnit path did not replace either full inventory receipt. |
| `git diff --check HEAD -- tests/fuzz/tests.cmake` | 0 | No whitespace errors. |

The focused read-only review passed both Standards and Spec axes with no findings and no fixes. The diff is limited to the normal replay; it uses the existing CTest serial property already used by the instrumented replay and preserves the command, inputs, labels and timeout. The review does not establish hosted efficacy or frontend behavior. No separate local workflow wall-clock/critical-path measurement was captured; no multi-run efficiency conclusion or timeout/topology/cache change is supported.

After commit `6de1300b6e1a697162f3143a48ae0311985c1477`, the tracer gate repeated the property check, full CI, ordinary/release inventory checks, hygiene/inventory, and focused tests; each post-change command exited 0. The expected baseline property rejection remained exit 1. These post-commit receipts are under ignored `build/phase14-gap08-evidence/source-tracer-*`.

### Task 2: exact-parent preflight and diagnosed cold timeout

The fresh detached checkout `.worktrees/phase-14-gap08-parent` was clean at `6de1300b6e1a697162f3143a48ae0311985c1477`, the Task 1 repair commit, with no inherited `build/ci` or `build/hygiene` cache. `PHASE14_PARENT_SHA` was therefore `6de1300b6e1a697162f3143a48ae0311985c1477`. All six existing pin recipes passed SHA verification: `wasmtime-aarch64-macos`, `7800basic-wasm`, `7800openbios-asm`, `rcheevos`, `mame-a7800-xml`, and `mame-a2600-xml`. The only copied inputs were the one-time property checker and the preserved baseline CTest JSON with paths rooted to this checkout.

The first parent gate, `cmake --workflow --preset ci`, exited 8 after the 693-test run. Its only failed tests were `bios.splash_skip` and `bios.rejects_cart`, each timing out at the existing one-second CTest limit. CTest printed `sh: /bin/ps: Operation not permitted` while handling both timeouts. The tests run 60 frames at most; the full suite ran them alongside the other parallel CI work. After this inspection, the bounded diagnostic `ctest --preset ci -R '^(bios\.splash_skip|bios\.rejects_cart)$' --no-tests=error --output-on-failure --output-junit build/ci/junit-gap08-timeout-diag.xml` exited 0: both tests and their two fixture prerequisites passed, 4/4 in 0.21 seconds. This supports parallel local contention as the likely timeout trigger, but does not erase the failed full workflow or establish hosted behavior. The original full-run and diagnostic logs/exits are kept in the detached checkout's ignored `build/phase14-gap08-evidence/`.

The first strict parent property comparison exited 1 because the fresh CMake configuration resolved the `GIT_EXECUTABLE` argument in `policy.compat_ratchet.controls` to `/usr/bin/git`, while the source baseline had `/opt/homebrew/bin/git`; generated source-line offsets were the expected +2. The ignored one-time checker was then adjusted to normalize only the absolute `-DGIT=.../git` argument in both JSON inputs. The command `ctest --preset ci --show-only=json-v1` exited 0, and the corrected property command, `cmake -DROOT="$PWD" -DINPUT=build/phase14-gap08-evidence/parent-after.json -DBASELINE=build/phase14-gap08-evidence/parent-before.json -P build/phase14-gap08-evidence/check-portable-scheduling.cmake`, exited 0: 693 test names and all remaining metadata match after that one tool-path normalization and the two expected generated line offsets. The full replay command, labels, TIMEOUT 14 and sole `RUN_SERIAL TRUE` are required and checked unchanged.

After the two fixture-backed tests passed alone, the one bounded warm parent workflow permitted for diagnosis was run exactly once: `cmake --workflow --preset ci` exited 0. All 693 ordinary tests and all six release tests passed; the full run's portable replay took 2.03 seconds. This makes parallel local contention the likely explanation for the cold one-second BIOS timeouts, but it does not establish a hosted Intel cause or performance result. The original cold failure remains preserved in `parent-ci.log` and `parent-ci.exit`; the warm workflow output, exit and full ordinary/release JUnit are in `parent-ci-warm.log`, `parent-ci-warm.exit`, `parent-ci-warm-junit.xml`, and `parent-ci-warm-release-junit.xml`.

The remaining parent gates each passed: ordinary and release inventory commands reported 693 and six tests exactly once; hygiene passed 30/30 and its inventory reported 30 exactly once; the corrected property comparison passed; and the focused six-test command passed, including `retroarch.states.controls`, loader, empty-corpus guard, install consumer, fixture setup and portable replay. The focused replay took 1.94 seconds and ran after the other selected tests completed. Per the bounded diagnosis, the full workflow was not run again.

No push or dispatch has occurred. The next step is to commit this evidence locally, freeze that commit with immediate parent `6de1300b6e1a697162f3143a48ae0311985c1477`, run the required frozen-source local preflight, and then recheck remote/auth/trigger/PR/run state before deciding whether the single authorized push and dispatch gates pass. The earlier read-only checks showed remote branch SHA `27818674366f80e230b8fa44493e1046282c3a66` and only historical runs `41629846188`, `41618546347`, and `41585416888`; no new allowance has been used. `PHASE14_DISPATCH_SHA` is not yet frozen. FRONT-02/03/04 remain pending.

### Plan 14-08 single dispatch outcome (2026-11-03)

The final frozen source was `2e32dc51558dceb5c536c704459f9d4765f959d7`; its immediate first parent was `32b3e3eea111f43858f601774071dc068ccd29af`, which contains the `RUN_SERIAL TRUE` repair. The normal phase branch was fast-forwarded locally to that source, then pushed once. `git push origin phase/14-proven-frontend-states-and-responsive-play` exited 0; `git ls-remote origin refs/heads/phase/14-proven-frontend-states-and-responsive-play` exited 0 and returned exactly the frozen SHA. The one fresh `gh workflow run ci.yml --ref phase/14-proven-frontend-states-and-responsive-play -f phase14_host=true` command exited 0. Its exact run identity is [41656505025](https://github.com/szTheory/muhtari/actions/runs/41656505025), event `workflow_dispatch`, branch `phase/14-proven-frontend-states-and-responsive-play`, head SHA `2e32dc51558dceb5c536c704459f9d4765f959d7`, attempt 1. The pre-dispatch run-ID set was `[41629846188, 41618546347, 41585416888]`. No second push/dispatch, rerun, cancellation or post-run push occurred.

After committing the planning records required before source freeze, every exact-source local gate was repeated at `2e32dc5`; the working tree was clean before publication. The `source-final-*.log` and matching `.exit` receipts are ignored under `build/phase14-gap08-evidence/`.

| Frozen-source command | Exit | Result |
| --- | ---: | --- |
| `cmake --workflow --preset ci` | 0 | Full CI passed: all 693 ordinary tests and six release tests. |
| `cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` | 0 | All 693 ordinary tests ran once. |
| `cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake` | 0 | All six release tests ran once. |
| `cmake --workflow --preset hygiene` | 0 | All 30 hygiene checks passed. |
| `cmake -DJUNIT=build/hygiene/junit.xml -DINVENTORY=tests/inventory/hygiene.txt -P tests/cmake/check_junit.cmake` | 0 | All 30 hygiene checks ran once. |
| `cmake -P build/phase14-gap08-evidence/check-portable-scheduling.cmake` | 0 | The final CTest metadata has 693 names; the portable replay keeps its command, corpus, labels and scaled `TIMEOUT`, and adds only `RUN_SERIAL TRUE` (apart from normalized generated source offsets/tool path). |
| `ctest --preset ci -R '^(fuzz\.regress\.(run(\.self_test)?|loader)|install\.consumer|retroarch\.states\.controls)$' --no-tests=error --output-on-failure --output-junit build/ci/junit-gap08-focused.xml` | 0 | All six selected tests, including replay and mock controls, passed; replay took 2.29 seconds. |
| `ctest --preset ci --show-only=json-v1` (saved `source-final-after.json`) | 0 | Fresh exact-source registration metadata captured for the property comparison. |

The detached exact-parent source `32b3e3eea111f43858f601774071dc068ccd29af` had a first cold CI exit 8 only for `bios.splash_skip` and `bios.rejects_cart` at their existing one-second limits. The two tests plus fixtures passed the bounded diagnosis 4/4 in 0.21 seconds; the one authorized warm `cmake --workflow --preset ci` then exited 0 with 693 ordinary and six release tests. Parent ordinary/release/hygiene inventories, the portable property comparison and the six-test focused command all exited 0. The complete parent and cold/warm receipts remain in the detached checkout's ignored evidence directory. This supports local parallel contention as a likely trigger for that cold run but does not establish hosted Intel behavior.

The complete hosted run response was saved to ignored `build/phase14-gap08-evidence/run.json`; `gh run view` and the run/job/artifact API fetches exited 0. `gh run watch 41656505025 --interval 15 --compact` exited 0 because it was invoked without `--exit-status`; the authoritative run conclusion is `failure`, not a watch success. `cmake -DINPUT=build/phase14-gap08-evidence/run.json -DEXPECTED_SHA=2e32dc51558dceb5c536c704459f9d4765f959d7 -P tools/check_ci_run.cmake` exited 1 because the required aggregate failed. The complete failed-job log is `run-failed.log`; focused macOS x64 and arm64 job logs are retained as `run-macos-x64.log` and `run-macos-arm64.log`.

On macOS x64, both portable replay registrations honored the new serial policy. In current-source CI, the other 692 tests completed first; `fuzz.regress.run` then passed in 5.48 seconds. In the independent-parent repeat, the other 692 tests likewise completed first and the replay passed in 5.90 seconds. The whole CI test phases passed 693/693 in 172.74 and 249.28 seconds respectively. These one-run results show that CTest honored the property and that this attempt did not reproduce the earlier timeout. They do not prove the earlier timeout's cause or establish a general performance result.

The required hosted Ubuntu x64 ASan lane passed, both Windows architecture builds passed, both macOS builds and the standalone quality/hash-equality jobs passed. The `wasm32` job failed as an advisory job while compiling `tests/core/test_state.c:2022`: WASI marks `tmpfile` as unavailable/deprecated, and `-Werror` promotes that warning to an error. It is not a dependency of `CI required`; the required aggregate failed because `retroarch` failed.

The `retroarch` job started only after the build matrix completed and ran for 34 seconds. Its `Fresh frontend and runner battery roundtrip` step passed both 120-frame battery runs, matched all 34,816 save bytes against the independent counts and reported exact 320x242 pixel equality. The step then exited 1 on its final command, `test "$(cat tests/retroarch/* | wc -l | tr -d ' ')" -le 300`. The current files total 3,845 lines (`states.c` alone is 3,145); this guard is tracked at `.github/workflows/ci.yml:535` from `f5e67068` and is independent of this plan's scheduling change. Consequently `Opt-in owned RetroArch save/load control`, `Require the Phase 14 states pass receipt`, and the compare negative control were skipped. The uploaded receipt says `host_outcome=skipped_before_host_step` and `reason=an earlier job step failed or was cancelled`; it has no `state_mode=states` or pass outcome. No state-mode screenshot, raw save, loaded-core digest, owned-fixture judgment, containment or complete-work observation exists. The six downloaded images are only generic DMA/battery controls; the available battery screenshot was visually inspected and is not a state-mode observation.

Only artifacts from this run and the three plan-authorized names were downloaded. The run artifact API associated all three with run `41656505025`: `archives-macos-arm64` ID `13240417131`, `hashes-macos-arm64` ID `13240751319`, and `retroarch-frames` ID `13242324221`. `hashes-macos-arm64/commit.txt` exactly equals the frozen SHA, and its 76-line `identity.txt` exactly matches the tree manifest for the pinned source and CI configuration. The three archive SHA-256 values are library `c2780c6c49f47c94c606f373428187e2e5ea6a3caae1300361bc89e0b88c0702`, libretro `481347ca654f145db3268bcc17eee5b24ea4e391151a9273fe47514027e32792`, and runner `64a994a6b886f5fdf56aed0aa8aeb31afdfd00f88619b0231922598c6cc452a2`. The `retroarch-frames` artifact contains the generic battery/DMA images and only `frontend-states/host-receipt.txt`; it has no host state capture. Because the host consumer did not run, there is no loaded-core digest, state-mode/host-version receipt, owned fixture result, or state screenshot/raw save to authenticate.

The job API provides `started_at` and `completed_at` but no `queued_at`; offsets below combine dependency wait and runner queue. Run start was `2026-11-03T14:43:30Z` and the workflow ended at `15:22:38Z` (39m08s).

| Job | Start offset | Duration | Result |
| --- | ---: | ---: | --- |
| changes | +3s | 6s | pass |
| hygiene | +3s | 2m07s | pass |
| title | +2s | — | skipped for non-PR dispatch |
| nofp (ubuntu-26.04) | +12s | 10s | pass |
| asan | +11s | 10m52s | pass |
| nofp (ubuntu-26.04-arm) | +13s | 8s | pass |
| be32 | +11s | 59s | pass |
| library | +11s | 34s | pass |
| build (windows-arm64) | +12s | 36m50s | pass; longest job and critical path |
| build (windows-x64) | +21s | 24m39s | pass |
| build (macos-x64) | +12s | 8m20s | pass |
| build (macos-arm64) | +19s | 5m43s | pass; supplied same-run artifacts |
| build (linux-x64) | +11s | 5m49s | pass |
| build (linux-arm64) | +17s | 3m35s | pass |
| perf | +11s | 31s | pass (non-PR artifact step skipped) |
| hash-equality | +37m05s | 6s | pass |
| quality | +37m05s | 7s | pass |
| wasm32 | +37m05s | 25s | failed (advisory) |
| retroarch | +38m08s | 34s | failed before owned state control |
| CI required | +38m54s | 13s | failed on required `retroarch` result |

There were no cancelled jobs or steps. Windows ARM64 was the critical path; the downstream hash, quality and WASI jobs started around +37m05s, the host job around +38m08s, and the required aggregate finished at +39m07s. No separate queue time can be computed. This single run supports no safe runner topology, cache or timeout change. The five-minute host limit was not reached; the host job ended early on the line-count guard.

This new attempt exhausts the 14-08 push/dispatch allowance. The requirement verdict remains pending: this evidence establishes one local scheduling proof and one hosted current/base non-reproduction, but no actual RetroArch state-mode observation. FRONT-02, FRONT-03 and FRONT-04 remain pending. Do not repeat, rerun, cancel, push, dispatch or launch RetroArch locally. A future host attempt requires a separately authorized scope that addresses the existing line-count guard and obtains a distinct live allowance.
