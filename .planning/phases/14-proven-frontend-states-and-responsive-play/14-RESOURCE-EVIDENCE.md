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

## Plan 14-11 independent repaired-parent preflight (2026-11-03)

The completed Plan 14-13 candidate is `b323ee2dac038cd4574cec35c824c06c5b3cc583`. Its actual first parent is `6ef384436912aca743ad72e94e9ef944350be76c`; the only tracked differences between those commits are the Plan 14-13 planning closeout files. A fresh detached checkout at `b323ee2` was created at ignored `.worktrees/phase-14-gap11-parent`, with its own build and output. Exact pinned inputs were prepared through the existing recipes in `scripts/fetch-pin.sh`. No build or JUnit data was copied. The checkout remained clean after verification.

Local workflow and exact inventory evidence on that detached source:

| Command | Exit | Result |
| --- | ---: | --- |
| `cmake --workflow --preset ci` | 0 | All five workflow steps completed: configure, build, 694 ordinary tests, all three packages, and six release tests. Log: ignored `build/phase14-gap11-evidence/parent-ci.log`. |
| `cmake --workflow --preset hygiene` | 0 | Configure, build, and all 30 hygiene checks completed. Log: ignored `build/phase14-gap11-evidence/parent-hygiene.log`. |
| Ordinary, release, and hygiene `tests/cmake/check_junit.cmake` inventory checks against the three parent JUnit receipts | 0, 0, 0 | All 694 ordinary, six release, and 30 hygiene entries ran exactly once without failures, errors, or skips. The checks read saved JUnit only. |
| `ctest --test-dir "$PHASE14_PARENT_DIR/build/ci" --show-only=json-v1` | 0 | Captured parent CTest metadata to ignored `build/phase14-gap11-evidence/ctest-parent.json`; SHA-256 `ad67d18d30fdd337a8e9aef05e55ffee8116709cc84e13899dc10def30bf7f92`. |
| Plan 14-13 property checker in `MODE=property` against the raw parent metadata | 1 | The checker did not reach the property assertions: its saved pre-mitigation baseline is from before the completed package workflow registered six release-only tests, so `backtraceGraph` differs. Its expected `build/phase14-gap13-evidence/ctest-before.json` is also absent in a fresh parent checkout. No project test or workflow ran from this check. |
| Read-only semantic CTest comparison after normalizing only the detached-checkout path | 0 | All 694 existing test names, commands, and properties match the saved pre-mitigation baseline after removing only `RUN_SERIAL`; the complete workflow adds exactly the six release tests. The exact serial set is the five approved BIOS tests plus the existing `fuzz.regress.run`. Earlier Plan 14-13 compare/property/fault controls remain passed. |

The parent metadata normalization changes only JSON strings rooted at the detached checkout path; raw JSON is retained. Raw and normalized SHA-256 values are respectively `ad67d18d30fdd337a8e9aef05e55ffee8116709cc84e13899dc10def30bf7f92` and `873dfdd88bf8353ede8b5a099aa03c64eef490a333d0431b396f3b4f0c3be34b`. The post-workflow CTest tree has 700 tests because release tests share the CI build tree; the separately saved ordinary and release JUnit receipts establish the exact workflow inventories. This is a checker-baseline limitation, not a test failure, and no workflow, test, configuration, deadline, or focused BIOS batch was repeated.

Parent build identity is AppleClang `21.0.0.21000101` on macOS arm64; the available pinned LLVM tools report `21.1.8`. SHA-256 identities for the source fixture and expectations are: `tests/diag/frontend-states.a78` `18d1e10cac2be0ac400fa8a4060628a693d5cb4eb8622a4e845f5c9477fe43b3`, `tests/roms/manifest.txt` `ab0dfab084e056e1436c502996c7db66205933b4e800ea0a56ac6d9ce20af31a`, and `tests/built/expected.txt` `d549d4691aca7c004325102613fd6ac2337ecfe8a4c0cec004472e5bbcc80ca8`. Parent package hashes are: libretro `f7d8f41e40142a000252c492a401b7023e5d9849f607696e7743b1f51484b8a7`, runner `af56720a566fc7554f69f239b9b083bf6c5a9599ec2fa9225d054fb57b390d78`, library `d5adc741d90441da1cd02582062ce9836da8defb92c8a2ff4d37fe69cdb674f2`. These are local artifact identities only; they do not qualify frontend behavior.

## Proposed fresh host decision scope

The proposed evidence source is a meaningful local evidence commit whose immediate first parent is `b323ee2dac038cd4574cec35c824c06c5b3cc583`. Its final source SHA will be recorded from that commit before the two source-bound local workflows. Nothing has been pushed or dispatched for this plan. The authenticated branch check found remote branch head `2e32dc51558dceb5c536c704459f9d4765f959d7` is an ancestor of the local branch, no open PR targets it, and the existing workflow only starts on `push` to `main` or PR events (not a push to this feature branch). The single ordinary feature-branch push would therefore create no extra CI run.

The proposed hosted action, only after fresh explicit owner authorization, is one normal `git push origin phase/14-proven-frontend-states-and-responsive-play`, followed by one `gh workflow run ci.yml --ref phase/14-proven-frontend-states-and-responsive-play -f phase14_host=true`. No rerun, second push/dispatch, cancellation, PR/merge, local RetroArch, CI retry, or Phase 12 action is included. The existing frontend limits stay fixed: RetroArch 1.22.2 on macOS 15, five-minute job, fifty-second child, forty-five seconds of work plus five seconds cleanup, 120 presented frames, and 64 explicit forward actions. Local macOS arm64 ASan remains unavailable; a fresh hosted Ubuntu x64 ASan success is required for acceptance. The prior advisory WASI failure remains visible.

From the workflow's configured job timeouts and six-build matrix, the ceiling is 301 runner-minutes across every configured job; the PR-only `title` job is skipped for this dispatch, making the applicable workflow_dispatch ceiling 296 runner-minutes. The latest observed whole hosted run is failed run `41656505025`, 39m08s elapsed, with a 36m50s Windows ARM64 build on the critical path. This single run does not isolate matrix cost or authorize timeout, cache, or topology changes. All previously recorded host allowances remain spent; this packet offers only the fresh decision for the exact source and one proposed attempt above.

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

## Phase-level regression gate (local, 2026-11-03)

The execute-phase regression gate resolved `workflow.test_command` to `cmake --workflow --preset ci` and `workflow.test_gate_timeout` to `0`. Since the timeout helper interprets zero as unbounded, the gate was kept bounded with its documented 600-second fallback; project configuration was not changed. The test-file handoff contains 137 prior-verification paths under the shared Git metadata directory.

The first `cmake --workflow --preset ci` run reached CTest's 693 ordinary-test suite. The log records five failures at the existing one-second CTest limit: `bios.splash_skip`, `bios.rejects_cart`, `bios.k_window`, `bios.k_first_fetch`, and `bios.load_aliases`. The wrapper did not preserve the top-level exit because zsh rejected its variable name `status` as read-only; the CTest failure and all five timed-out tests are in ignored `build/phase14-gap08-evidence/regression-gate.log`. The focused diagnostic command `ctest --test-dir build/ci -R '^bios\.(splash_skip|rejects_cart|k_window|k_first_fetch|load_aliases)$' --output-on-failure -j1` ran both fixture prerequisites and all five BIOS tests, 7/7 in 0.52 seconds, exit 0.

The one bounded warm full-workflow retest, `cmake --workflow --preset ci` under `run-with-timeout 600`, exited 8. It reproduced the same five BIOS timeouts; the other 688 ordinary tests passed, including `fuzz.regress.run`, and the release suite did not run. Its exit and log are ignored `build/phase14-gap08-evidence/regression-gate-warm.exit` and `.log`. No source or timeout change followed, and no further full local workflow was run. The isolated diagnostic supports a scheduling/load explanation but does not establish the cause; the phase-level regression gate remains failed. This is separate from the exact-source CI preflight, which passed before the later phase-level gate.

## Plan 14-09 local hosted-support ceiling repair (2026-11-03)

This local change removes only the retired line-count blocker from the existing
RetroArch battery step. The prior source total of 3,845 remains historical
evidence. The report-only command now returns 3,897 newline characters after its
52-line addition; it compares the total with no quota. No actual RetroArch,
full-workflow, host, publication, or remote action was run here. Results are
N/A-local; run URL: N/A-local.

| Command | Exit | Result |
| --- | ---: | --- |
| cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^policy\.host_support\.controls$' --no-tests=error --output-on-failure --output-junit build/ci/junit-gap09-policy.xml | 0 | The recurring CMake control passed in 0.24 seconds. It independently counted each copied support file and the aggregate, accepted 301 added newline characters, rejected missing states.c, checked comments and unrelated jobs, and required failures for the restored 300-line gate, a different numeric ceiling, and a missing reporter. |
| cmake -DMODE=inspect -DWORKFLOW=build/ci/policy/host_support_pre-repair.yml -P build/ci/policy/host_support_controls.cmake | 1 (expected) | A snapshot of the pre-repair workflow failed specifically on -le 300, proving the retired guard was the blocker. |
| cmake -DMODE=support-report -DROOT="$PWD" -P tests/retroarch/run_retroarch.cmake | 0 | Reported bmp.c 76, compare.c 319, receipt_driver.c 40, run_retroarch.cmake 281, states.c 3,145, and test.cfg.in 36 newline characters; aggregate 3,897. |
| ctest --preset ci -R '^retroarch\.states\.controls$' --no-tests=error --output-on-failure --output-junit build/ci/junit-gap09-host-controls.xml | 0 | Existing mock containment, identity, progress, image, storage, workload, and cleanup controls passed in 0.47 seconds; this supplies no real-host proof. |
| actionlint -shellcheck='' -ignore 'label "ubuntu-26\.04' -ignore 'label "ubuntu-26\.04-arm' .github/workflows/ci.yml | 0 | Workflow syntax and action expressions passed with the existing runner-label allowances. |
| git diff --check | 0 | No whitespace errors. |
| cmake -DROOT="$PWD" -DBUILD="$SAFE_GUARD_DIR" -DWORK="$SAFE_GUARD_DIR" -P build/ci/policy/host_support_controls.cmake, with SAFE_GUARD_DIR="$PWD/build/ci/policy/gap09-scratch-guard-check" | 1 (expected) | The strict-child scratch guard rejected targeting the build directory itself before cleanup; its sentinel remained present. |

The first two policy-control runs exposed assertion-handling defects in the
new control itself: CMake wrapped the missing-input and mutation diagnostics
across lines. The report correctly rejected the missing path, but the original
assertions did not match the wrapped text. The assertions now search for the
specific missing path and normalize wrapped diagnostics; the final focused
control above passes. The expected pre-repair rejection is separate from those
test-authoring failures.

### Boundary review

The changed workflow boundary is one command: the existing battery step still
runs its DMA and battery judges, then calls the report-only mode immediately
before the unchanged opt-in state step. The launcher checks the six currently
admitted support inputs, sorts and reports regular non-symlink files, and
returns before frontend argument validation, cleanup, config generation, or
process launch. Existing legacy/state launch guards, firmware and pixel/media
judges, runner/core pins, job and child deadlines, frame/action limits, and
fixture ownership remain unchanged. The policy control reads and mutates only
copies under the ignored build tree; its own cleanup is confined to a strict
child of the project build directory. The focused control took 0.24 seconds;
no dependency or broader harness was added. One review finding tightened that
scratch-path boundary; no further required finding remains.

The local regression gate recorded above is still failed on five existing
one-second BIOS timeouts. This gap plan did not rerun the complete workflow or
change those limits. FRONT-02, FRONT-03, and FRONT-04 remain pending; the
14-08 hosted allowance remains spent, Phase 12 remains halted, and no new host
or remote allowance was created.

## Plan 14-10 bounded BIOS diagnosis (local, 2026-11-03)

The previously recorded two full-workflow failures remain separate from the
focused serial result. Both full workflows exited 8 on
`bios.splash_skip`, `bios.rejects_cart`, `bios.k_window`,
`bios.k_first_fetch`, and `bios.load_aliases`, each at the existing one-second
CTest deadline. The earlier focused command passed 7/7 with its two fixture
prerequisites in 0.52 seconds; that result did not establish why the full runs
timed out.

Before the new focused run, `ctest --test-dir build/ci --show-only=json-v1`
was saved to ignored `build/phase14-gap10-evidence/ctest-before.json`. It
records the same five BIOS commands, fixture prerequisites, working directory,
labels, and unscaled one-second deadlines. The configured executable is
`build/ci/tests/muhtari-test-boot` (SHA-256
`062012c2aedc8ef7b416c42f64afa607993f793462a755df0515ad678ced5690`), built
from the current `daa6a3e66b426f648cdc2839951858f0696e829c` source tree with
CMake 4.4.3, Apple Clang 21.0.0, Release `-O3 -DNDEBUG`, and timeout scale 1.
The OpenBIOS fixture SHA-256 is
`46b5bc2b716a20a1c40e2b920365c29cbab1594d01da94f4567b240181365cb2`; the
simple cartridge fixture SHA-256 is
`d09da3ed27fe82e0a4dd2821ff2cb9bde26c9531c59890788acb1cfdc4004ecf`.
These values and the test metadata are preserved with the ignored diagnostic
receipts; no BIOS command, assertion, fixture, label, or deadline was edited.

The required focused command was run once more against those exact files:

| Command | Exit | Result |
| --- | ---: | --- |
| `ctest --test-dir build/ci -R '^bios\.(splash_skip|rejects_cart|k_window|k_first_fetch|load_aliases)$' --output-on-failure -j1` | 0 | Both fixture prerequisites and all five BIOS cases passed, 7/7 in 0.32 seconds. This is diagnostic only. |

The failed full-run logs identify the only narrowed competing CTest workload
used for three attempted pairs: `frame.sample_renders`,
`bios.handoff_simple`, `title.simple.fast`, `title.simple.bios`,
`title.pal.fast`, and `title.pal.bios`. Each pair used those six tests plus the
five timed BIOS tests and the two fixture prerequisites; default CTest preset
scheduling was compared with `--parallel 1`. No full workflow, build, worker
topology, cache, timeout, or test registration was changed.

| Pair | Default schedule | Serial schedule | Measurement and qualification |
| --- | --- | --- | --- |
| 1 | Exit 0; 13/13, 1.06s | Exit 0; 13/13, 2.02s | The CTest output shows the selected jobs overlapping. The wrapper did not capture aggregate CPU, so this pair is incomplete causal evidence. |
| 2 | Exit 0; 13/13, 0.87s | Exit 0; 13/13, 1.85s | Aggregate default CPU was 1.66s user / 0.12s system (199%); serial was 1.73s / 0.12s (99%). CTest output shows the selected jobs overlapping. No BIOS timeout reproduced. |
| 3 | Exit 1 before test launch | Exit 1 before test launch | Both commands used the invalid option `--output-failure`; CTest reported “Unknown argument”. This pair is not comparable. The three-pair cap is exhausted; no retry was made. |

The exact commands, logs, JUnit output, source and fixture identities, elapsed
times, aggregate CPU where captured, load snapshots, and CTest metadata are in
ignored `build/phase14-gap10-evidence/`. Pair 1 and pair 2 both passed under
default and serial scheduling. They confirm that those selected tests can
overlap in CTest and pass in this local observation; they do not attribute the
prior failures to those consumers or prove that serial isolation is a remedy.
Pair 3 is explicitly excluded from causal evidence.

Process ancestry, PID ownership, and per-process CPU could not be observed in
this environment: `ps`, `pgrep`, `top`, and `sysctl -n hw.ncpu` were denied by
the sandbox. `uptime` provided only a machine-wide load average. No process was
terminated and no cleanup was attempted. The earlier timeout log's
`sh: /bin/ps: Operation not permitted` message is likewise not a process
identity receipt. There is no evidence of a stale owned child or an attributable
external competitor.

The ignored `check-bios-diagnosis.cmake` validator checks the saved CTest
registrations, source/executable/fixture identity, the original five failure
names and deadlines, all three paired receipts, command comparability, and an
explicit supported mechanism. Its current receipt is intentionally rejected:
one pair omitted aggregate CPU, one pair did not launch tests, and the remaining
valid pair did not reproduce a timeout or establish a cause. Missing-pair,
changed-command, and missing-cause fault probes are recorded separately. No
causal remedy is supported, so Tasks 2 and 3, any full workflow, and any
registration edit are stopped here. `tests/boot/tests.cmake` remains unchanged;
FRONT-02, FRONT-03, and FRONT-04 remain pending, and Plan 14-11's coordinated
full-workflow preflight is blocked.

| CMake receipt check | Exit | Result |
| --- | ---: | --- |
| `cmake -DPROBE=missing-pair -P build/phase14-gap10-evidence/check-bios-diagnosis.cmake` | 1 (expected) | Rejected the missing-pair mutation. |
| `cmake -DPROBE=changed-command -P build/phase14-gap10-evidence/check-bios-diagnosis.cmake` | 1 (expected) | Rejected the changed-command mutation. |
| `cmake -DPROBE=missing-cause -P build/phase14-gap10-evidence/check-bios-diagnosis.cmake` | 1 (expected) | Rejected the missing-cause mutation. |
| `cmake -P build/phase14-gap10-evidence/check-bios-diagnosis.cmake` | 1 (expected) | Rejected the current receipt at the invalid pair 3 CTest command. |

The missing evidence is a time-aligned process record from an observed BIOS
timeout: PID, parent PID, executable and worktree ownership, launch/elapsed
time, and user/system CPU for the failing test process and any overlapping
receipt-named workload during the one-second window. Machine-wide load and
successful serial runs do not supply that attribution. The narrower next
investigation should use a process-observable local environment and one bounded
default-scheduled observation of the unchanged focused BIOS command plus only
the receipt-named workloads, collecting process ancestry and CPU near launch
and timeout. Preserve the existing one-second deadlines; if the timeout does
not recur or the process record cannot attribute it, keep the cause unresolved

## Plan 14-12 evidence review (2026-11-03)

This review reconciles the existing receipts only. Both required full local
`cmake --workflow --preset ci` runs exited 8 on the same five BIOS tests:
`bios.splash_skip`, `bios.rejects_cart`, `bios.k_window`,
`bios.k_first_fetch`, and `bios.load_aliases`, each at its existing one-second
CTest deadline. The focused five-test command passed with its two fixture
prerequisites (7/7; the recorded runs took 0.52 seconds and 0.32 seconds).
Two valid default-versus-serial pairs also passed all 13 selected tests: pair 1
took 1.06/2.02 seconds, and pair 2 took 0.87/1.85 seconds. These passes show
that the bounded observations did not reproduce the failures; they do not
identify why either full workflow timed out.

The third planned comparison pair passed the invalid `--output-failure`
argument to CTest, which exited 1 before launching tests. Plan 14-10 counted
that attempt against its three-pair limit and remains terminal halted history;
this evidence review neither retries it nor renews or charges that cap to new
work. The previously attempted read-only `ps` probe was denied by the sandbox
(exit 127, `Operation not permitted`). Process identity and CPU attribution
therefore remain unknown. The failure is a sandbox restriction, not evidence
about which processes caused the timeouts.

The scheduling hypothesis worth validating is limited to `RUN_SERIAL TRUE` on
exactly the five named BIOS tests. CTest would keep each of those tests from
overlapping other tests in the same CTest invocation. This reversible,
test-only metadata change would preserve each one-second deadline, command,
fixture prerequisite, label, inventory row, and every other test contract.
No deadline increase or broader scheduling change is proposed. The available
receipts make this a bounded hypothesis, not a diagnosis: isolation would not
establish the historical timeout cause or satisfy FRONT-02, FRONT-03, or
FRONT-04.

Plan 14-13 requires a fresh explicit owner decision on its exact five-property
source change, metadata controls, and one focused BIOS batch before any of that
work begins. Plan 14-11 owns the complete ordinary/release workflows on the
repaired candidate and on the frozen evidence commit whose immediate first
parent is that candidate, followed by its separate host decision. The three
frontend requirements remain pending.

## Plan 14-13 authorized BIOS isolation result (local, 2026-11-03)

### Owner decision and scope

On 2026-11-03 the owner explicitly replied, “yes i authorize that,” to the
Plan 14-13 checkpoint packet. The decision authorizes only RUN_SERIAL TRUE
on bios.splash_skip, bios.rejects_cart, bios.k_window,
bios.k_first_fetch, and bios.load_aliases, one CMake configure, metadata
comparison/property/fault controls, and one focused BIOS CTest batch. It does
not establish why the earlier full workflows timed out.

The base checkout was d3d5ae2e7a44751643514bea422c6ce5989bdd98; the original
tests/boot/tests.cmake blob was
619b5cddd7bf72bee132fdf5c8944162e5517dc3 (Git blob ID). The repaired source blob is
160a86846dac73b629a8e8dd536cb7c3d1feb386. The sole tracked source change adds
the five scheduling properties. Commands, fixtures, labels, assertions, exact
inventory rows, deadlines, CTest preset, and worker settings are unchanged.
Each BIOS timeout remains one second.

### Metadata and focused test receipts

| Command | Exit | Result |
| --- | ---: | --- |
| ctest --preset ci --show-only=json-v1 > build/phase14-gap13-evidence/ctest-before.json | 0 | Captured pre-change metadata at the approved base before the source edit. The five target registrations had their expected commands, fixtures, labels, and one-second deadlines. |
| cmake --preset ci | 0 | The single authorized configure. |
| ctest --preset ci --show-only=json-v1 > build/phase14-gap13-evidence/ctest-after.json | 0 | Captured regenerated metadata; the suite still contains 694 tests. |
| cmake -DROOT=. -DMODE=compare -DBEFORE=build/phase14-gap13-evidence/ctest-before.json -DAFTER=build/phase14-gap13-evidence/ctest-after.json -P build/phase14-gap13-evidence/check-bios-isolation.cmake | 0 | Only the five approved RUN_SERIAL properties were added; every other test and metadata field is preserved. Rechecked at the tracer gate. |
| cmake -DROOT=. -DINPUT=build/phase14-gap13-evidence/ctest-after.json -DMODE=property -P build/phase14-gap13-evidence/check-bios-isolation.cmake | 0 | The five new properties, existing fuzz.regress.run property, unchanged target metadata, one-second deadlines, and exact seven-row inventory pass. Rechecked at the tracer gate. |
| cmake -DROOT=. -DMODE=controls -DBEFORE=build/phase14-gap13-evidence/ctest-before.json -DAFTER=build/phase14-gap13-evidence/ctest-after.json -P build/phase14-gap13-evidence/check-bios-isolation.cmake | 0 | Rejected copied metadata with one required property removed, a new unrelated RUN_SERIAL, or a relaxed BIOS timeout. Rechecked at the tracer gate. |
| ctest --preset ci -R '^(bios\.(splash_skip|rejects_cart|k_window|k_first_fetch|load_aliases))$' -N -V | 0 | Prelaunch selection resolved to exactly five BIOS tests plus built.openbios and built.simple; no project test started. |
| ctest --preset ci -R '^(bios\.(splash_skip|rejects_cart|k_window|k_first_fetch|load_aliases))$' --no-tests=error --output-on-failure --output-junit build/phase14-gap13-evidence/junit-bios.xml | 0 | The single focused batch passed all seven tests: two fixture setups and five BIOS cases. Total CTest time was 0.26 seconds. This batch is consumed; it was not rerun. |
| cmake -DJUNIT=build/phase14-gap13-evidence/junit-bios.xml -DINVENTORY=build/phase14-gap13-evidence/inventory-bios.txt -P tests/cmake/check_junit.cmake | 0 | JUnit receipt contains the exact seven inventory tests once each, with no skips or failures. Rechecked at the tracer gate. |
| git diff --check && git diff --exit-code -- CMakePresets.json cmake/MuhtariTests.cmake tests/inventory/ci.txt .github/workflows/ci.yml | 0 | No whitespace errors or changes to protected configuration, CI, or the shared test inventory. |

The JUnit command above resolved its relative output path under the ci preset's
binary directory, producing
build/ci/build/phase14-gap13-evidence/junit-bios.xml. The first receipt-check
invocation pointed at the absent source-root path and exited 1 with
-DJUNIT=<file> is required; no test was launched by that check. The exact CTest
receipt was copied to build/phase14-gap13-evidence/junit-bios.xml, then the
inventory check passed. No test was retried.

The first checker implementation repeatedly parsed the complete metadata
document and returned no result within its initial 30-second command window.
It was replaced with a per-test-object comparison; the final compare,
property, and three fault controls passed above. An attempt to stop that
first metadata-only command exited 3 because the process-list service was
unavailable. This did not launch or alter any project test.

All local run URLs: N/A-local. The isolation property is a bounded scheduling
mitigation only; the historical timeout cause remains unknown. Plan 14-11 owns
the later complete ordinary/release workflows and its distinct host decision.
FRONT-02, FRONT-03, and FRONT-04 remain pending.

## Plan 14-11 authorized host attempt result (2026-11-03)

### Source, publication, and run identity

The owner explicitly authorized Task 2's concrete one-attempt scope. The single normal push of `phase/14-proven-frontend-states-and-responsive-play` succeeded and the remote branch matched the frozen source. The single `gh workflow run ci.yml --ref phase/14-proven-frontend-states-and-responsive-play -f phase14_host=true` invocation exited 0. No second push or dispatch, rerun, job rerun, cancellation, local RetroArch launch, PR/merge, or Phase 12 operation occurred. This allowance is consumed.

The authenticated attempt is [GitHub Actions run 41733613173](https://github.com/szTheory/muhtari/actions/runs/41733613173): event `workflow_dispatch`, branch `phase/14-proven-frontend-states-and-responsive-play`, attempt 1, exact source `a05965953bf34bbb3bdff4bf57118e491a525204`, whose first parent is `b323ee2dac038cd4574cec35c824c06c5b3cc583`. The API receipt and complete run/job/step receipts are under ignored `build/phase14-gap11-evidence/`. `gh run watch 41733613173 --exit-status` exited 1; the run API records a completed failure and the job receipt records the failed required aggregate.

The run lasted 2,060 seconds (34m20s). The Windows ARM64 build was the critical path at 2,003 seconds (33m23s). Its quality-observation step passed, along with the remaining builds, hosted Ubuntu x64 ASan, quality join, and hash equality. No job or step was cancelled. The required RetroArch job failed and the `CI required` aggregate failed; the separate advisory WASI job also failed. This single run does not justify timeout, cache, or topology changes.

### Host result and exact failure boundary

The `retroarch-frames` artifact and its `host-receipt.txt` are from the same run and source. The artifact API identifies `archives-macos-arm64` ID `13277937921`, `hashes-macos-arm64` ID `13277128464`, and `retroarch-frames` ID `13279667108`; each is unexpired and matches the run/source. All three macOS arm64 package hashes match the receipt. The receipt identifies RetroArch 1.22.2, `state_mode=states`, `driver_result=1`, and `host_step_exit=1`; the loaded core digest before and after the failed driver is unchanged.

The pre-existing DMA screenshot and fresh battery roundtrip steps passed. The opt-in states driver printed `states: fail-closed sandbox UDP containment preflight` and returned 1. It failed before `host_prepare_media` and before the states-mode RetroArch spawn. The screenshots in this artifact belong to the earlier DMA/battery steps; the artifact includes no save files. There is no states-mode `final.bmp` or committed raw save to judge, and no state-mode image/progress/save, rewind, run-ahead, replay, durability, or process-containment assertion completed.

In `tests/retroarch/states.c:3028-3031`, this message follows a compound condition covering `/usr/bin/sandbox-exec` availability, the sandbox helper result, the remaining work-time bound, `update_config_port`, and `host_validate_runahead_config`. The receipt does not identify which member failed. Do not attribute the result to sandbox capability, UDP containment, or any other single member. The identity checks and earlier normal host controls do not resolve that missing distinction.

The advisory `wasm32` job failed compilation at `tests/core/test_state.c:2022`: WASI marks `tmpfile` deprecated because it is not defined for that target, and `-Werror` rejects the diagnostic. Keep this job advisory and retain the failure without a source change or retry.

### Outcome

The authorized hosted attempt is spent. No fresh states-mode screenshot or save exists, so no independent frontend-state judgment was possible. FRONT-02, FRONT-03, and FRONT-04 remain pending; no frontend behavior or capability is qualified by this run. Earlier failures and spent allowances remain recorded. No new lesson was added because the exact failed preflight condition is still unknown.

### Outcome-only documentation verification

The first prescribed docs-filter run exited 8 because the initial `docs/testing.md` outcome note exceeded the existing budget by 1,344 bytes. The note was shortened to link to this evidence record, and the same filter then passed:

| Command | Exit | Result |
| --- | ---: | --- |
| `ctest --preset ci -R '^docs\.(budgets|api_names(\.self_test)?)$' --no-tests=error --output-on-failure --output-junit build/ci/junit-gap11-docs.xml` (initial note) | 8 | `docs.budgets` rejected the over-budget note. |
| Same docs-filter command after shortening the note | 0 | All three selected documentation checks passed. The JUnit output is under ignored `build/ci/build/ci/junit-gap11-docs.xml` because the preset resolves the relative output path from `build/ci`. |
| `git diff --check` | 0 | No whitespace errors. |

No full workflow or host command was repeated for these outcome-only edits.

## Plan 14-14 local prelaunch diagnostic result (2026-11-03)

The clean pre-edit source was `f0f64dbe4f075b65b5f54916fe154c76c14695bc` on
`phase/14-proven-frontend-states-and-responsive-play`. The diagnostic now reports
the first failed production prelaunch stage (`sandbox-exec`, `sandbox-helper`,
`work-deadline`, `config-port`, or `runahead-config`) in stderr, which the
existing CMake launcher retains in its failed receipt. The sandbox helper also
names bounded internal failure points. The shared coordinator preserves the
original operation order and deadline; failed paths release only the owned
reserved descriptor, and media preparation and frontend startup remain after
the coordinator's successful return.

`retroarch.states.controls` exercises the production coordinator through
offline callbacks. It covers all five first-failure stops, pass ordering,
deadline equality/live behavior, exact owned-descriptor cleanup, the generated
`test.cfg.in` port patch and run-ahead validation, and the existing failed
launcher receipt. No sandbox helper, network request, media preparation,
RetroArch process, host receipt pass, screenshot, or save was produced by these
controls. The config mutation/validation controls run on Darwin; non-Apple
builds retain the portable coordinator pass/fault coverage.

The pre-implementation receipt assertion first passed because its mock had
already been updated. After removing that mock diagnostic, the intended RED
ran the selected test and exited 8 because the failed receipt lacked
`states prelaunch stage=sandbox-helper failed`; this was an assertion failure,
not a build or test-discovery error. Restoring the fixture diagnostic and
implementing the production coordinator produced the recorded GREEN pass.

The boundary review also found that the external-UDP check treated a negative
`poll` result like a timeout, although it means the parent could not observe a
reply. A new offline control exercised statuses 0, 1, and -1 through the
production decision: the selected CTest failed on -1 before the fix, then
passed after only a completed no-reply result (0) was accepted. This local
correction does not identify which check failed in the historical host run.
Its failed member remains unknown and no cause is attributed.

| Command | Exit | Result |
| --- | ---: | --- |
| `cmake --preset ci` | 0 | Configured the existing CI build for the focused target. |
| `cmake --build --preset ci` | 0 | Built the updated state driver and existing receipt fixture. |
| `ctest --preset ci -R '^retroarch\\.states\\.controls$' --no-tests=error --output-on-failure --output-junit "$PWD/build/ci/junit-gap14-tracer-red.xml"` (RED) | 8 (expected) | The selected receipt control failed on the missing exact stage diagnostic. |
| `ctest --preset ci -R '^retroarch\\.states\\.controls$' --no-tests=error --output-on-failure --output-junit "$PWD/build/ci/junit-gap14-tracer-green.xml"` | 0 | The stage-specific failed receipt was retained; the focused control passed. |
| `ctest --preset ci -R '^retroarch\\.states\\.controls$' --no-tests=error --output-on-failure --output-junit "$PWD/build/ci/junit-gap14-controls.xml"` | 0 | All five stage faults, pass ordering, deadlines, config cases, and owned cleanup passed offline. |
| `ctest --preset ci -R '^retroarch\\.states\\.controls$' --no-tests=error --output-on-failure --output-junit "$PWD/build/ci/junit-gap14-udp-red.xml"` (UDP observation RED) | 8 (expected) | The offline production-decision control failed because `poll_result=-1` was accepted as a no-reply result. No network operation ran. |
| `ctest --preset ci -R '^retroarch\\.states\\.controls$' --no-tests=error --output-on-failure --output-junit "$PWD/build/ci/junit-gap14-udp-green.xml"` | 0 | Only `poll_result=0` now passes the external-UDP observation; all state prelaunch controls pass. |
| `ctest --preset ci -R '^docs\\.(budgets|api_names(\\.self_test)?)$' --no-tests=error --output-on-failure --output-junit "$PWD/build/ci/junit-gap14-docs.xml"` (first documentation edit) | 8 | The unchanged `docs/testing.md` budget correctly rejected the initial addition. The text was shortened; no budget changed. |
| `ctest --preset ci -R '^docs\\.(budgets|api_names(\\.self_test)?)$' --no-tests=error --output-on-failure --output-junit "$PWD/build/ci/junit-gap14-docs-final.xml"` | 0 | `docs.budgets`, `docs.api_names`, and its fault control passed. |
| `/opt/homebrew/opt/llvm@21/bin/clang-format --dry-run --Werror` on changed `states.c` ranges | 0 | Changed C ranges conform to clang-format 21.1.8. |
| `git diff --check f0f64dbe4f075b65b5f54916fe154c76c14695bc` | 0 | No whitespace errors against the recorded clean base. |
| `cmake --preset ci` | 0 | Configured the final Plan 14-14 source after task commits. |
| `cmake --build --preset ci` | 0 | Built the final state driver. |
| `ctest --preset ci -R '^retroarch\.states\.controls$' --no-tests=error --output-on-failure --output-junit "$PWD/build/ci/junit-gap14-controls-final.xml"` | 0 | Final offline coordinator, receipt, configuration, cleanup and UDP poll controls passed. |
| `ctest --preset ci -R '^docs\.(budgets|api_names(\.self_test)?)$' --no-tests=error --output-on-failure --output-junit "$PWD/build/ci/junit-gap14-docs-final.xml"` | 0 | Final documentation budget and API-name checks passed. |
| `git diff --check f0f64dbe4f075b65b5f54916fe154c76c14695bc` | 0 | Final source has no whitespace errors against the recorded clean base. |
| `git diff --exit-code f0f64dbe4f075b65b5f54916fe154c76c14695bc -- CMakePresets.json cmake/MuhtariTests.cmake tests/boot/tests.cmake tests/fuzz/tests.cmake tests/inventory/ci.txt tests/inventory/wasm32.txt .github/workflows/ci.yml tests/retroarch/test.cfg.in` | 0 | Protected inventories, workflow and template remain unchanged. |

The bounded review found no unresolved blocking issue. No test inventory,
deadline, host policy, workflow topology, or resource cap changed. Run URLs:
N/A-local. The prior one-attempt host allowance remains spent; FRONT-02,
FRONT-03, and FRONT-04 remain pending. Plan 14-15 owns complete source/parent
local workflows and reaches a fresh owner checkpoint before publication or
any host attempt.

### Phase-level regression gate

The phase regression gate's configured command resolves to `cmake --workflow --preset ci`, which would repeat the complete workflow. Plan 14-11 Task 3 allows only read-only audit and local outcome bookkeeping after a failed host attempt and explicitly prohibits repeating full workflows. The exact frozen source had already passed the complete local CI workflow, including all ordinary tests, before these documentation-only edits; the prescribed docs filter passed after the edits. Therefore no duplicate full workflow was run in Plan 14-14.

## Plan 14-15 independent parent workflow outcome (2026-11-03)

After Plan 14-14's source, summary and state/roadmap commits, the repaired
candidate parent was `3e31b9be45e287f58d3e02b8d2b3ab73120cc704`. A fresh clean
detached worktree at `.worktrees/phase-14-gap15-parent` used the same pinned
prerequisites as the current checkout: only the ignored `build/pins` directory
(69 MiB) was copied; no build output or test results were copied. The local
toolchain reported CMake 4.4.3 and AppleClang 21.0.0.21000101.

The complete parent CI workflow exited 8 in its ordinary CTest step. Of 694
selected ordinary tests, 12 timed out: four title checks (`title.simple.fast`,
`title.simple.bios`, `title.pal.fast`, `title.pin.self_test`) and eight Maria
DMA cases (`maria.dma_line.empty_dl`, `headers`, `widths`, `indirect`, `holes`,
`terminators`, `last_line`, `abort`). The other 682 selected tests passed. The
workflow stopped before packaging and the release suite, so no release JUnit
was produced. The saved JUnit, full CTest log and failed-test list are kept
under ignored `build/phase14-gap15-evidence/parent/`; the parent worktree is
clean. The failures and their timeouts are recorded without attributing a
cause or changing any test bound.

The independent parent hygiene workflow exited 0; all 30 selected checks
passed. JUnit validation from the current checkout confirmed the failed
ordinary receipt (exit 1 naming exactly the 12 timeout cases) and the complete
hygiene receipt (exit 0). The expected release JUnit existence check exited 1
because CI stopped before that workflow step.

Process inspection through both `ps` and `pgrep` was denied by the local
environment (`Operation not permitted` / `Cannot get process list`). The
workflow command had already exited; no process cleanup or retry was attempted.
This failed parent CI gate prevents the Plan 14-15 host decision checkpoint.
It does not reopen the spent host allowance or identify run 41733613173's
failed prelaunch member. The remaining source-local workflows are separately
bounded to one run each; no remote qualification, publication, host command or
frontend launch is part of this failure investigation.

| Command | Exit | Result |
| --- | ---: | --- |
| `cmake --workflow --preset ci` (cwd `.worktrees/phase-14-gap15-parent`) | 8 | The ordinary test step had 12 timeouts; packaging and release tests did not run. Run URL N/A-local. |
| `cmake --workflow --preset hygiene` (same cwd) | 0 | All 30 hygiene/policy checks passed. Run URL N/A-local. |
| `cmake -DJUNIT="$PWD/build/phase14-gap15-evidence/parent/junit-ci.xml" -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake` | 1 | The preserved ordinary receipt matches the expected inventory and reports exactly the 12 timeouts above. |
| `cmake -DJUNIT="$PWD/build/phase14-gap15-evidence/parent/junit-hygiene.xml" -DINVENTORY=tests/inventory/hygiene.txt -P tests/cmake/check_junit.cmake` | 0 | All 30 hygiene tests ran once and passed. |
| `test -e build/phase14-gap15-evidence/parent/junit-release.xml` | 1 | Expected absence: the CI workflow stopped before its release step. |
| `ps -axo pid,etime,command` filtered to the parent checkout | 1 | Local policy denied process listing; the workflow command itself had terminated. |
| `pgrep -fl 'phase-14-gap15-parent|ctest|muhtari-test'` | 3 | The system process-list service was unavailable; no retry or cleanup followed. |

All local run URLs: N/A-local.
