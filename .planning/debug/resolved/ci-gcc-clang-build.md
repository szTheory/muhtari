---
status: resolved
trigger: "Diagnose and fix the GCC mismatched-dealloc failure in tests/runner/test_save.c:308 and the Clang undeclared realpath failure in runner/main.c:841. Run cheap consistency checks before longer builds or suites."
created: 2026-11-17
updated: 2026-11-17
---

## Symptoms

- Expected: GCC and Clang CI build lanes compile cleanly.
- Actual: GCC fails with `-Werror=mismatched-dealloc` at `tests/runner/test_save.c:308`; Clang fails because `realpath` is undeclared at `runner/main.c:841`.
- Related error: Required CI, Release, and Nightly jobs reported these compiler failures on PR 77. The GCC issue affects library/Linux/BE32 and equality lanes; the Clang issue affects ASan and fuzz builds.
- Timeline: These failures appeared in the automatic workflows for PR 77 on 2026-11-17.
- Reproduction: The corresponding CI GCC and Clang compiler lanes fail on the PR source. The exact local compiler/preset reproduction is to be established from the source and workflow configuration.

## Current Focus

focus_2026_11_17:
  status: "Local compiler-debug scope resolved; exact hosted acceptance remains outside this scope."
  hypothesis: "Confirmed independent GCC wrapper-ownership and glibc X/Open declaration-visibility causes, corrected locally."
  evidence: "Phase 27 CI evidence records GCC -Werror=mismatched-dealloc at tests/runner/test_save.c:308 and Clang's undeclared realpath at runner/main.c:841 across CI, Release, and Nightly."
  test: "Read the exact source, compile commands, toolchain definitions, and CI lane setup; use cheap syntax/configuration checks before any full build or suite."
  expecting: "Each failure is reduced to a minimal source/toolchain cause, fixed without weakening warnings or platform coverage, and verified with focused compiler checks before broader local acceptance."
  next_action: "Parent reconciles Phase 27 source-qualified verification and preserves the closed remote scope. No owner-run test is needed for these compiler-only changes."
  bug_class: "Bohrbug: deterministic compile-time failures"
  candidate_causes:
    - "code: a libc allocation declaration is macro-renamed by the test fixture before its matching release annotation is consumed"
    - "config/environment: strict C17 plus _POSIX_C_SOURCE hides the X/Open realpath declaration on glibc"
  and_gate: "No: either source independently fails its compiler lane."
  reasoning_checkpoint:
    hypothesis: "GCC's partial wrapper inlining exposes fopen/fdopen's fclose ownership attribute across test_close; glibc hides realpath without X/Open feature selection."
    confirming_evidence:
      - "GCC 15.3.0 -O3 reproduces mismatched-dealloc for both fopen and fdopen wrappers at their test_close call sites."
      - "Apple Clang 21 targeting Linux with glibc headers reproduces undeclared realpath under only _POSIX_C_SOURCE=200809L."
      - "glibc annotates fopen with its fclose deallocator, and the GCC documentation explains partial-inlining mismatches and paired allocator annotations."
    falsification_test: "If correctly paired allocator annotations still warn, or X/Open 700 leaves realpath undeclared in the same header/compiler test, the proposed mechanism is incomplete."
    fix_rationale: "Model the test wrappers' actual allocator/deallocator pairing and expose the declaration through the platform's standard feature interface; preserve warnings and alias behavior."
    blind_spots: "Local GCC is 15.3 rather than hosted 15.2; local Clang is Apple 21 targeting Linux rather than hosted upstream Clang 21. Native Windows/WASI execution is not part of this local scope."
    candidate_causes:
      - "code: test allocator wrappers lack the deallocator relationship carried by libc"
      - "config: runner consumers request POSIX but not X/Open declarations"
    and_gate: "No: independent failure paths, each with its own correction."
  limits: "Local diagnosis and fixes only. No push, hosted dispatch, rerun, merge, release, or owner-host action. Preserve earlier failed CI evidence."

## Evidence

- timestamp: 2026-11-17
  checked: "Final source complete native CI workflow"
  found: "cmake --workflow --preset ci exits 0: all 736 ordinary tests, native ZIP package creation, and all 8 release tests pass. Final full-warning GCC 15.3 -O3 fixture compile exits 0. Final docs.budgets and diff whitespace checks pass."
  implication: "Authorized local fix is verified. This does not replace exact-source hosted GCC/Clang acceptance or reopen prior spent remote attempts. Parent owns terminal commits."

- timestamp: 2026-11-17
  checked: "Focused runner tests, compiler controls, hygiene and complete native CI workflow"
  found: "95/95 runner-focused tests pass; all four unique runner translation units compile under GCC 15.3/full warnings/-O3. Annotation-only revert copy restores GCC's exact diagnostic; removing X/Open restores Clang's realpath error and reapplying passes. Initial format check flagged one declaration's line wrapping; corrected and format passes. Hygiene tree passes. Complete native CI tests passed 735/736; only the newly expanded docs/testing.md exceeded its unchanged 40960-byte ceiling. Shortened the note before retry."
  implication: "Behavior and root-cause controls pass. Finish final-source checks; no timeout, inventory, diagnostic or ceiling change is needed."

- timestamp: 2026-11-17
  checked: "Focused fixed-source compiles, CMake configure, and native build"
  found: "GCC 15.3.0 -O3/full warning-set test_save.c compile exits 0 after paired allocator annotations. Clang 21/Linux-target realpath compile exits 0 after _XOPEN_SOURCE=700. git diff --check, cmake --preset ci, and cmake --build --preset ci --parallel 4 exit 0."
  implication: "Both fixes remove their original diagnostics without disabling warnings; continue behavior and revert controls before accepting."

- timestamp: 2026-11-17
  checked: "Knowledge base, lessons, runtime, skill mapping, and relevant source/build declarations"
  found: "No matching resolved KB entry. MU-TEST-010 warns that native Clang does not establish GCC warning compatibility. Runtime is @opengsd/gsd-core 1.20.0; no debugger/project skill mapping exists. No per-test coverage/failing test spectrum or Stryker exists; SBFL and configured mutation tooling are unavailable."
  implication: "Use deterministic compiler repros and the existing runner fault suite; no specialist review mapping applies."
- timestamp: 2026-11-17
  checked: "GCC 15.3.0 container compile and Clang 21 Linux-target syntax compile"
  found: "GCC reproduces the reported mismatched-dealloc under -O3 even after adding X/Open visibility. Clang reproduces the undeclared realpath with glibc headers. The two diagnostics are independent."
  implication: "Confirmed source/config causes; retain each compiler warning and fix both independently."

- timestamp: 2026-11-17
  checked: "Phase 27 CI evidence and project state"
  found: "Automatic CI, Release, and Nightly runs reported the same GCC mismatched-deallocation diagnostic and Clang undeclared-realpath diagnostic. The Linux perf receipt passed separately; overall CI and Release remained failed."
  implication: "Investigate the two compiler failures locally as a new scope; do not retry hosted workflows under the spent PR77 authorization."

## Eliminated

- hypothesis: "GCC warning is caused by missing X/Open feature visibility or a genuine mismatched stream close in production."
  evidence: "Adding X/Open alone leaves GCC's mismatched-dealloc unchanged; test_close always invokes fclose, and pairing the test wrappers removes the warning while retained fault/cleanup tests pass."
  timestamp: 2026-11-17

## Resolution

root_cause: "Partial inlining of unannotated test fopen/fdopen wrappers crosses libc's fclose ownership association; strict C17 runner builds select POSIX declarations but omit X/Open required for glibc realpath."
oracle_type: specified
fix: "Select _XOPEN_SOURCE=700 alongside POSIX on all five runner consumers, and annotate test_fopen/test_fdopen with their actual test_close deallocator on GCC 11+. Document why both contracts matter."
verification:
  target_test:
    result: pass
    evidence: "GCC 15.3.0/full Release warning flags compiles all four unique runner translation units; Clang 21/Linux-glibc syntax compile passes with X/Open selection."
  mutation_check:
    result: skipped
    reason_if_skipped: "No Stryker or configured C mutation runner exists. Isolated fix-removal controls below demonstrate defect sensitivity without adding tooling."
  no_op_deletion:
    result: pass
    deletion_justified_by_rca: false
    evidence: "Diff adds ownership annotations and declaration selection. No behavior, branch, assertion, warning, deadline, ceiling or inventory is removed or relaxed."
  adjacent_tests:
    result: pass
    suites_run:
      - "95 focused runner tests, including fixtures"
      - "complete ci workflow: 736 ordinary tests, native packages, 8 release tests"
      - "hygiene.tree and final hygiene.format"
      - "final docs.budgets and validate.consistency"
  revert_and_reconfirm:
    result: pass
    bug_returned_on_revert: true
    fixed_on_reapply: true
    evidence: "Annotation-only isolated source revert returns mismatched-dealloc; fixed fixture compile passes. Removing only X/Open selection returns undeclared realpath; restoring passes."
  guardrail_verdict: accepted
  specialist_review: "none: no mapped C/general specialist skill in the available catalog"
files_changed: [CMakeLists.txt, tests/runner/tests.cmake, tests/runner/test_save.c, docs/maintaining.md, .planning/LESSONS.md]

## Reproduction and controls

All commands below run at the authorized worktree root. Run URL: N/A-local.

- GCC repro (exit 1 before annotation, exit 0 after): `docker run --rm --network none -v "$PWD:/work" -w /work gcc:15 gcc -DMUHTARI_TRACE_LOOP=1 -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700 -Isrc -Iinclude -Ihost -O3 -DNDEBUG -std=c17 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wstrict-prototypes -Wmissing-prototypes -Wvla -Wundef -Wcast-qual -Wimplicit-fallthrough -Wmissing-variable-declarations -Wduplicated-cond -Wlogical-op -Werror -c tests/runner/test_save.c -o /tmp/muhtari-test-save.o`. The same full flags compile `runner/main.c`, `tests/runner/test_runner.c` and `tests/runner/test_audio.c`, exit 0 each.
- GCC isolated revert (exit 1, exact original diagnostic): copy `test_save.c` into ignored `build/compiler-portability/test_save-reverted.c`, omit only the newly added forward declaration/annotation block, and compile with the same command plus `-iquote tests/runner`. Original fixed source remains untouched and recompiles exit 0.
- Clang/glibc repro (exit 1 without X/Open; exit 0 with it): `clang --target=aarch64-linux-gnu --sysroot="$PWD/build/compiler-portability/sysroot" -isystem "$PWD/build/compiler-portability/sysroot/usr/include/aarch64-linux-gnu" -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700 -DMUHTARI_TRACE_LOOP=1 -std=c17 -Isrc -Iinclude -Ihost -Werror -fsyntax-only runner/main.c`. The ignored sysroot contains dereferenced system headers from the same GCC container; native Clang is Apple Clang 21.0.0. Removing only `-D_XOPEN_SOURCE=700` restores undeclared `realpath`.
- `cmake --preset ci`, `cmake --build --preset ci --parallel 4`, `ctest --test-dir build/ci --output-on-failure -R '^runner\.' -j 4 --output-junit compiler-portability-runner.xml`: exit 0 each; focused runner run includes its required fixtures.
- `ctest --test-dir build/hygiene --output-on-failure -R '^hygiene\.format$'`: exit 0 after declaration formatting; `hygiene.tree`: exit 0. Initial combined hygiene run exited 8 for formatting only; its tree test passed.
- `ctest --test-dir build/ci --output-on-failure -R '^docs\.budgets$'`: exit 0 after moving the explanation to `docs/maintaining.md`. Two preliminary workflow attempts exited 8 only for the new testing-doc note exceeding the unchanged byte ceiling; all other tests passed in both runs.
- `node <selected-runtime>/gsd-tools.cjs query validate.consistency`: exit 0, no errors/warnings.
- Final `cmake --workflow --preset ci`: exit 0, 736 ordinary tests and 8 release tests pass with native ZIP packages. Private raw output remains ignored under `build/compiler-portability/workflow-ci-accepted.out`; no raw log is committed.

## Prevention

why_not_caught: "Native AppleClang builds did not exercise glibc feature visibility or GCC's allocation/deallocation attribute analysis. Required hosted GCC/Clang gates correctly caught both before merge."
recurrence_guard: "Existing strict GCC/Clang runner targets retain fatal warnings; all five consumers now select X/Open 700; test stream allocators declare the actual paired close wrapper. Existing runner save/state/trace fault tests remain intact."
branching_whys:
  code: "Fault wrappers substitute stream lifetime calls, but only libc carried deallocator identity; GCC's asymmetric inlining made that missing wrapper contract visible. Paired annotations express the actual wrapper relationship."
  config_environment: "The new realpath use needs X/Open visibility under glibc; existing POSIX-only runner definitions happened to compile against Darwin headers. Matching feature declarations across every implementation consumer removes that accidental dependency."
  and_gate: "No: each diagnostic independently blocks its compiler lane; both corrections are needed for the affected source set."

## Closeout boundary

The session manager owns terminal commits for `CMakeLists.txt`, `tests/runner/tests.cmake`, `tests/runner/test_save.c`, `docs/maintaining.md`, `.planning/LESSONS.md`, and this debug file. No other files are part of this fix. Local compiler-debug resolution does not claim hosted CI is green: exact hosted acceptance stays unverified, and the remote scope remains closed. No human workflow test is needed; the parent acts next on delivery-scope reconciliation.

## Specialist Review

None: the general hint maps to `engineering:debug`, which is absent from the available skill catalog. No mapped C specialist skill is installed.

## Cycles

investigation: 2
fix: 1

## References and limits

- [GCC paired allocation attributes](https://gcc.gnu.org/onlinedocs/gcc-15.1.0/gcc/Common-Function-Attributes.html): paired attributes retain allocator/deallocator identity and prevent partial inlining; applied to test wrappers only.
- [Linux realpath declarations](https://man7.org/linux/man-pages/man3/realpath.3.html): X/Open feature selection exposes the declaration in glibc.
- Existing required GCC/Clang build consumers detect the two compiler regressions; existing runner fault tests cover actual close/error behavior. No redundant new runtime test was added for a compiler-only defect.
- Exact hosted GCC 15.2/upstream Clang 21 and Windows/WASI lanes remain unexecuted in this local scope. Older failed hosted results remain terminal. No publication, remote rerun, dispatch, merge, release, RetroArch or owner-host action occurred.
