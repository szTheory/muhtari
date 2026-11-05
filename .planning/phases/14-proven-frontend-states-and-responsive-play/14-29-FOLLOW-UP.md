# Phase 14 Plan 29: Targeted Timeout Non-Reproduction

## Observed result

The one authorized `ctest --test-dir .worktrees/phase-14-host25-parent/build/ci -R '^title\.pin\.self_test$' --output-on-failure -V --output-junit ...` invocation exited 0. Its required `runner.probe.images` fixture passed in 0.0438745 seconds. `title.pin.self_test` passed in 0.0645173 seconds with the expected negative-control exit 1 and `single colour` output; its registered one-second timeout was unchanged. CTest reported 0.12 seconds total.

CTest's executable and working directory were observed. The exact predicate executable-path query began before the test, took about 0.807 seconds, and returned no PID after outlasting the test interval. The observer therefore missed the short-lived child; the record does not establish whether the child's launch or exit was observed.

## What it establishes

This single target invocation did not reproduce the earlier full-workflow timeout. The registered fixture and negative-control test completed as expected once, with the original timeout unchanged. The process query's empty result is an observation limit, not evidence that the predicate did not launch.

## What remains unknown

The earlier full-workflow timeout remains unexplained. The `/bin/ps` message is in workflow-level stderr and remains unattributed to the test. This successful run does not identify a cause or establish a timing remedy.

## Preserved stops

Plan 14-25 remains terminal after its failed full workflow. Its separate full hygiene, frozen-source, and owner gates remain unrun. The one-run diagnostic allowance is spent; no CTest retry or project workflow is authorized. The approved staged-only pre-commit hygiene hook applies only as a required side effect of Plan 14-29 documentation commits and does not run or resolve Plan 14-25's full hygiene gate. Existing remote, CI, Docker, RetroArch, and host stops remain in force.

FRONT-02/03/04 remain pending. This report provides no host acceptance.

## Recommended next decision

The owner selected **close this diagnosis as unresolved** on 2026-11-05. This particular run is closed as an honest non-reproduction; the historical cause remains unknown. If root-cause diagnosis continues later, first propose a launch-time process observer and demonstrate that it captures a controlled child shorter than the observed 0.0645173-second test interval. Then request a separate, exact one-invocation scope. Even a successful future run cannot retroactively explain the historical timeout. For further diagnosis, no test command or scope is granted.
