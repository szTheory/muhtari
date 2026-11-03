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

The run covers only source `1e8f019`; later documentation and formatting commits are not covered by it. The one-dispatch allowance is spent. Do not dispatch again, retry, or launch the frontend locally. The current-source change to baseline selection or any new host evidence requires a separately authorized scope and a new plan.
