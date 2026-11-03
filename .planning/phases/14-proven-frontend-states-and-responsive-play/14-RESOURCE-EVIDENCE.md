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

The run covers only source `1e8f019`; later documentation and formatting commits are not covered by it. The original one-dispatch allowance described by this attempt is spent. The owner separately authorized one bounded Phase 14 gap-closure dispatch on 2026-11-03; that additional allowance remains unused until the gates below pass. Phase 12 remains halted. No retry or local frontend launch is authorized.

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

The bounded ASan commands above did not pass locally. Their failure is a platform limitation, not evidence that the repaired code passed ASan. The exact immediate-parent checkout and its CI, hygiene, and inventory checks remain required; only local ASan is omitted there for the same documented macOS arm64 limitation. The host dispatch allowance remains unused until those checks and all publication preconditions pass.
