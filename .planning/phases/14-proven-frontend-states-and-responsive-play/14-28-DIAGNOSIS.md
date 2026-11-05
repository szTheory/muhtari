# Plan 14-28: Saved-Evidence Diagnosis of the Plan 14-25 Native Timeout

This is a static diagnosis of the single recorded Plan 14-25 candidate-R CI failure. It uses immutable receipts, existing generated CTest records, tracked source and read-only Git inspection. No project command, test, workflow, process probe, network or host action was run for this diagnosis. It does not establish Phase 14 acceptance.

## Saved receipt attribution

**Cause status: unresolved**

The two required immutable parent receipts are present and match their recorded SHA-256 identities:

| Receipt | SHA-256 | Finding |
| --- | --- | --- |
| `build/phase14-host25-evidence/parent/full-ci.log` | `56d4ad6e1ba50e8800e469c8f930a166d89c0d4022cc447481f58247e2af5034` | The one recorded CI workflow's captured stdout followed by a separate stderr section. |
| `build/phase14-host25-evidence/parent/junit-ci.xml` | `18505b7d7ded91d7123ad3e604dcef4aeddb5a5b0fa251ca700f598fcc12a7e5` | The ordinary-suite result for the same recorded candidate-R workflow. |

`scope.json` records candidate R `5f0b4d42f076a8a84385209fa1a31383e3f05461`, matching `parent_sha`, and the detached source checkout `.worktrees/phase-14-host25-parent`. Read-only Git inspection confirmed that checkout's `HEAD` is R, its symbolic `HEAD` is detached, the commit object's first parent is RED commit `9b1a642176e74fda93a3e6b4efe44bcb61309731`, and its worktree is clean. The tracked files in the command chain below have no differences between R and this worktree's current `HEAD` (`git diff --name-status 5f0b4d42f076a8a84385209fa1a31383e3f05461 -- CMakePresets.json CMakeLists.txt cmake/MuhtariTests.cmake tests/compat/tests.cmake tests/compat/predicate.c tests/cmake/expect_fail.cmake tests/runner/tests.cmake tests/fuzz/tests.cmake tests/cart/write_images.c`: exit 0, no paths).

`scope.json:1` records exactly one parent workflow, workflow exit 8, 695 ordinary tests, 694 passed, and `title.pin.self_test` as the failed test. Its recorded failure field combines the one-second timeout and `/bin/ps` denial; that pairing is not causal attribution. It also records no package, release, hygiene, frozen workflow, checkpoint, remote dispatch/push/mutation or host start. Plan 14-25's terminal evidence independently records the same candidate identity and boundary (`14-25-EVIDENCE.md:19-33`; `14-25-SUMMARY.md:80-85,104-126`).

Parsing the immutable JUnit directly with Python's standard-library XML parser established 695 testcases, one failure, and no skipped or disabled tests. The only failure is `title.pin.self_test`, with `status="fail"`, elapsed time `1.00844` seconds and failure message `Timeout`. That testcase has empty `system-out` and no `system-err`. The saved `full-ci.exit` contains `8`, the CTest/workflow result. The outer `local-gates.cmake` wrapper returned `1` after its `run_saved(full-ci, ...)` saw that nonzero result and raised its fatal stop (`local-gates.cmake:75-89`; the outer invocation and exit are recorded at `14-25-EVIDENCE.md:25-29`). The wrapper exit is not the child predicate's exit: no saved receipt establishes that the predicate launched, progressed, or returned.

The failure list is at `full-ci.log:1714-1715`. The saved workflow stream then has the literal `--- stderr ---` marker at line 1717 and `sh: /bin/ps: Operation not permitted` at line 1718 (`full-ci.log:1714-1719`). `local-gates.cmake:18-31` captures workflow stdout and stderr in separate variables and writes stdout first, then the marker, then stderr. That placement is a stream-aggregation format, not event chronology. The denial is workflow-level stderr; it is absent from this testcase's JUnit output, and the existing `LastTest.log` has an empty `Output` block for the test. Nothing in the saved records attributes the denial to the predicate child.

The wrapper stops immediately after failed CI, before later inventory/hygiene/package/release steps (`local-gates.cmake:83-94,108-139`). The terminal record confirms that the frozen-E gate and owner checkpoint were not reached (`scope.json:1`; `14-25-EVIDENCE.md:27-33`).

## Command and capture chain

The tracked registration is a negative control for pinning a blank image. `tests/compat/tests.cmake:53-64` prepares `muhtari-test-predicate` with the generated `runner-probe/linear-v3-16k` image, predicate `title@120=-`, and `--pin`; it registers the command through `expect_fail.cmake`, expecting exit code 1 and output matching `single colour`, with fixture `probe_images` required. The saved generated CTest record corroborates this exact argv and one-second property (`.worktrees/phase-14-host25-parent/build/ci/CTestTestfile.cmake:243-244`):

```text
cmake -DCMD=build/ci/tests/muhtari-test-predicate;build/ci/runner-probe/linear-v3-16k;title@120=-;--pin \
  -DCODE=1 -DREGEX=single colour -P tests/cmake/expect_fail.cmake
```

Here the paths are normalized relative to `.worktrees/phase-14-host25-parent`. The generated command contains no cross-compiling emulator prefix. `tests/compat/predicate.c:19-24,277-296,483-510` says `--pin` hashes the selected fast-boot frame and rejects a frame whose pixels are all one colour, emitting `fail single colour ...` and returning 1. So the intended successful negative-control path is a predicate return of 1 plus that text, which lets `expect_fail.cmake` finish successfully.

`cmake/MuhtariTests.cmake:11-23` multiplies the declared timeout by `MUHTARI_TIMEOUT_SCALE`. The registration declares one second (`tests/compat/tests.cmake:60-64`); the saved `CMakeCache.txt` records `MUHTARI_TIMEOUT_SCALE:STRING=1` and source directory R (`.worktrees/phase-14-host25-parent/build/ci/CMakeCache.txt:274,290,391`). The `ci` configure preset is Release (`CMakePresets.json:17-23`); its test preset writes JUnit, excludes release-labeled tests and sets no explicit `jobs` count (`CMakePresets.json:170-177`). The existing source comment's 0.16-second timing sample is historical macOS x64 data, not a measurement of this candidate-R run (`tests/compat/tests.cmake:63`).

The image is a build-tree fixture. `tests/runner/tests.cmake:1580-1587` registers `runner.probe.images` as `FIXTURES_SETUP probe_images`; `tests/fuzz/tests.cmake:8-17` defines the image-writer target, and `tests/cart/write_images.c:1-10,39-106` describes and writes its generated seed images. The saved CI log records `runner.probe.images` passing (`full-ci.log:350,381`). This supports that fixture setup ran in the workflow, but it is not a child-process trace for `title.pin.self_test`.

`tests/cmake/expect_fail.cmake:12-19` invokes the prepared child with `execute_process`, assigns both output pipes to the same buffered variable, and only checks the result and regex after the child returns. On a normal return, that variable is exposed through the wrapper's diagnostic or success message. CTest's one-second timeout interrupted the test before the saved records show that verdict. The generated CTest record proves the command was prepared, not that the predicate process launched, made progress, or completed.

The extant generated records are corroboration only: `LastTest.log:2073-2086` repeats the normalized command and records 1.01 seconds with empty test output, while its test index denominator is 701. The immutable JUnit is the authority for the one filtered 695-test workflow. The generated files have no digest binding them to that exact workflow; no replacement metadata was created. The tracked command chain (`tests/compat/tests.cmake`, `expect_fail.cmake`, `predicate.c`, and the test-registration helper) contains no process-list invocation. That absence does not identify whether the external CMake/CTest implementation, its shell/runtime, or another process produced the `/bin/ps` diagnostic.

## Cause assessment

The timeout cause remains unknown. The denied process enumeration and the censored one-second failure are separate observations; neither proves the cause of the other. The read-only evidence supports these alternatives but does not distinguish them:

| Possibility | Supporting evidence | Counterevidence and missing discriminator | Confidence |
| --- | --- | --- | --- |
| Child launch or runtime delay | The test hit its outer one-second limit while its intended command runs the predicate on the probe image through frame 120. | JUnit and `LastTest.log` expose no child output or status. A prepared command does not prove launch or progress; no child start/exit timestamps exist. | Low; possible, unproved. |
| CMake/CTest process supervision or cleanup | The CTest failure is a timeout, and `/bin/ps` is reported in the parent workflow's stderr. A timeout cleanup path could attempt process enumeration. | Stream concatenation hides event order. No tracked call invokes `/bin/ps`, and the receipt has no process owner, executable, cwd or event-time trace for the denied call. It could be a consequence of timeout cleanup or unrelated workflow stderr. | Low; correlation only. |
| Scheduling or resource overlap | The `ci` test preset declares no explicit job count, and this was one test in a 695-test workflow. | No saved start/finish timeline for competing tests or resource measurements identifies overlap with this test. The timeout alone cannot establish contention. | Low; possible, unmeasured. |
| Command, fixture or capture fault | The test depends on a generated image and an `execute_process` capture wrapper; a bad path or wrapper fault could block the expected result. | Registration, generated argv, timeout and fixture agree; `runner.probe.images` passed in the workflow. The JUnit says timeout rather than a returned wrong code/regex. Empty captured output still leaves child launch and buffering unknown. | Low; less supported, not excluded. |

Plan 14-27's separate CI workflow passed 695/695 ordinary tests and 6/6 release tests (`14-27-SUMMARY.md:100-107`). That comparison weakens a persistent deterministic command/source-fault explanation, but it does not identify why this single R workflow timed out and does not authorize a rerun.

## Missing evidence and next authority

The smallest useful next investigation is a separately approved, one-invocation CTest scope against candidate R, targeting only `title.pin.self_test` and retaining its existing one-second timeout. A proposed command is:

```text
ctest --test-dir .worktrees/phase-14-host25-parent/build/ci -R '^title\.pin\.self_test$' --output-on-failure
```

That target requires the existing `probe_images` fixture setup, so the finite allowance would be one CTest invocation with that one target and its registered setup test. The intended observation is whether the CTest wrapper and predicate process start, which executable and cwd are involved, whether the child returns, and what each captured stream contains. Preserve the generated JUnit, LastTest output, and separate parent stdout/stderr for that one invocation; do not alter the command, timeout, fixture or source.

This is a proposal only. It needs a new explicit owner-approved scope naming the environment and granting exactly that bounded invocation plus process attribution sufficient to identify executable and cwd. The current runtime denied `/bin/ps`; Plan 14-28 does not authorize retrying or substituting a process probe here, silently moving environments, or charging another test run. Any failure or non-reproduction after the one authorized launch ends that proposed scope without retry. No workflow or source remedy is supported by the static evidence; any later remedy would need its own evidence and scope.

## Preserved stops

- Plan 14-25 remains terminal after its sole candidate-R workflow exit 8. Do not resume it, repeat the workflow, run hygiene or frozen-E validation, prepare the owner decision packet, or claim its owner checkpoint was reached.
- The prior complete-parent timeouts remain unattributed. Plan 14-25 package/release/hygiene and frozen-E gates, its owner checkpoint, and remote/host work were not reached.
- Existing host allowances remain spent; the old Docker pairs remain ineligible; Phase 12 remains halted. Plan 14-24 and Plan 14-26 keep their historical terminal outcomes. Plan 14-27's pass does not reset them.
- FRONT-02, FRONT-03 and FRONT-04 remain pending; `requirements-completed` is empty. This native prerequisite diagnosis supplies no pinned-host save/load, rewind or run-ahead evidence.
- This plan did not change `.planning/STATE.md`, `.planning/ROADMAP.md`, `.planning/REQUIREMENTS.md`, source or workflow files. `.planning/STATE.md` was already modified at execution start and is preserved as found. No project acceptance verification follows this report.

### Protected-file and receipt preservation

| File | Execution-start SHA-256 | After report checks | Result |
| --- | --- | --- | --- |
| `.planning/STATE.md` | `6bfbd74d83afa19b8a87296a9ed23901e7b74f78df5cace1b732354af7c0d110` | `6bfbd74d83afa19b8a87296a9ed23901e7b74f78df5cace1b732354af7c0d110` | Match; pre-existing worktree modification retained. |
| `.planning/ROADMAP.md` | `b0cd1d974289c0ede8869ac22a09524ac49439b7380a11c74287027bd26fb25b` | `b0cd1d974289c0ede8869ac22a09524ac49439b7380a11c74287027bd26fb25b` | Match. |
| `.planning/REQUIREMENTS.md` | `75b493967e4f7a93949a67bdbc5082aa08406d9fad52d3268c57dbd06afb6de1` | `75b493967e4f7a93949a67bdbc5082aa08406d9fad52d3268c57dbd06afb6de1` | Match; FRONT requirements remain unchecked. |
| `build/phase14-host25-evidence/parent/full-ci.log` | `56d4ad6e1ba50e8800e469c8f930a166d89c0d4022cc447481f58247e2af5034` | `56d4ad6e1ba50e8800e469c8f930a166d89c0d4022cc447481f58247e2af5034` | Match; immutable receipt. |
| `build/phase14-host25-evidence/parent/junit-ci.xml` | `18505b7d7ded91d7123ad3e604dcef4aeddb5a5b0fa251ca700f598fcc12a7e5` | `18505b7d7ded91d7123ad3e604dcef4aeddb5a5b0fa251ca700f598fcc12a7e5` | Match; immutable receipt. |

At execution start, `git status --short` showed only the pre-existing `.planning/STATE.md` edit and the index had no staged paths. After the report-only checks, that same STATE edit remains, the index is still empty, and the only new paths are this diagnosis and its SUMMARY. No extra snapshot directory was created because this plan's authorized outputs are the diagnosis and its SUMMARY only.

### Report-only verification commands

The two plan-local standard-library report-check commands and their exact exits are recorded in `14-28-SUMMARY.md`. Task 1 and Task 2 both passed after the diagnosis was finalized (exit 0 for each). Task 2 initially returned exit 1 because copying its command verbatim here made the status-string check match both alternatives; the exact commands are recorded in the SUMMARY instead. Both checks are local (`N/A-local`) and inspect saved receipts/report text only; they do not launch project code.

Other allowed evidence consisted only of read-only receipt/source/Git inspection, Python standard-library hashing/XML parsing, and preservation comparisons. Project test/build/workflow gates, CMake/CTest invocations, process probes, network, Docker, RetroArch and host actions were not run (`N/A-local`).
