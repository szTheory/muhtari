---
phase: 12-player-ready-delivery-and-playstead-bridge
plan: "06"
subsystem: testing
tags:
  - libretro
  - retroarch
  - input
  - ci
requires:
  - phase: "12-05"
    provides: The reviewed failed receipt and repaired source inputs
provides:
  - A locally tested normal packaged-host recipe for one held and released input capture
  - A source-bound full-run receipt that records the Windows ARM64 test failure
affects:
  - PLAY-03
  - Phase 12 delivery acceptance
tech-stack:
  added: []
  patterns:
    - One normal host process produces the held and released input observations
    - Workflow policy checks bind to the actual normal host recipe
key-files:
  created:
    - .planning/phases/12-player-ready-delivery-and-playstead-bridge/12-06-SUMMARY.md
  modified:
    - .github/workflows/ci.yml
    - tests/policy/retroarch_preflight.cmake
    - tests/libretro/input_trace.cmake
    - tests/retroarch/test_input_trace_helper.c
key-decisions:
  - Stop after the first required full-run failure and preserve PLAY-03 as pending.
commits: 1
plan_head_before: 92720d04acf154cb32a01f4c1d2e5bcc37c3fc8d
plan_head_after: 7b6d8832058a89ea3ffc1dde18add4f6b832d35b
actuals:
  tokens: 8586
  tasks: 2
  commits: 1
duration: "31m 25s from the 19:55 EDT preservation baseline to terminal receipt at 20:26 EDT; earlier discovery time is not included."
completed: 2026-10-31
status: halted
requirements-completed: []
coverage:
  - id: D1-local-route
    description: The normal held/released input route is wired and passed the local policy and CI workflow checks.
    requirement: PLAY-03
    verification:
      kind: integration
      ref: "cmake -DROOT=\"$PWD\" -DWORK=build/ci/policy/retroarch_preflight -P tests/policy/retroarch_preflight.cmake; cmake --workflow --preset ci"
      status: pass
    human_judgment: false
  - id: D1-host-route
    description: The hosted normal input route was not observed because Windows ARM64 failed before RetroArch started.
    requirement: PLAY-03
    verification:
      kind: e2e
      ref: https://github.com/szTheory/muhtari/actions/runs/41255515829
      status: blocked
    human_judgment: false
  - id: D2-package
    description: The source-bound macOS arm64 package and its core/info members were produced and their digests recorded, but host loading was not observed.
    requirement: PLAY-03
    verification:
      kind: integration
      ref: archives-macos-arm64 from run 41255515829; package and member digests are recorded below
      status: pass
    human_judgment: false
  - id: D3
    description: Held and released input images and their separate comparator verdicts were not produced.
    requirement: PLAY-03
    verification:
      kind: e2e
      ref: https://github.com/szTheory/muhtari/actions/runs/41255515829
      status: blocked
    human_judgment: false
  - id: D4
    description: The normal host DMA comparison and its two fresh-process battery controls did not run.
    requirement: PLAY-03
    verification:
      kind: e2e
      ref: https://github.com/szTheory/muhtari/actions/runs/41255515829
      status: blocked
    human_judgment: false
  - id: D5
    description: The required CI aggregate failed after the Windows ARM64 build/test/package step failed.
    requirement: PLAY-03
    verification:
      kind: integration
      ref: tools/check_ci_run.cmake on run 41255515829
      status: failed
    human_judgment: false
---

# Phase 12 Plan 06: Player-Ready Delivery and Playstead Bridge Summary

The normal one-process held/released host recipe is wired and locally green, but the sole full CI run stopped at a Windows ARM64 openBIOS timeout before RetroArch input acceptance.

## Performance

- Duration: 31m 25s from the initial preservation baseline through terminal receipt; discovery before that baseline is not included.
- Preservation baseline: 2026-10-31 19:55:01 EDT.
- Terminal run receipt: 2026-10-31 20:26:26 EDT.
- Tasks recorded: 2; files: four source paths plus this uncommitted summary.

## Accomplishments

- Wired the normal packaged-host step to use one input child for both a held screenshot and a distinct released terminal screenshot, with separate 600-frame movies and held/released comparisons. The existing DMA and two fresh battery children remain in the normal route.
- Added normal-route policy checks that bind to the actual workflow step and reject missing or extra input launches, bad captures, mismatched controls, diagnostic substitution, shortened movies, and loss of DMA, battery, or aggregate checks.
- The policy pass/fault command and full local CI workflow passed. One independent bounded review passed with no blocker or warning.
- Published the exact four-path source commit once to the phase branch and submitted the single authorized full CI dispatch.
- The full run stopped on its first required failure. The run receipt, job log, annotations, and all available exact-run artifacts were captured; PLAY-03 remains pending.

## Task Commits

1. Task 1, Wire and observe ordinary packaged held/released input with the full host controls: 7b6d8832058a89ea3ffc1dde18add4f6b832d35b.
2. Task 2, Reconcile full packaged-host evidence and retain the acceptance stop: recorded in this uncommitted summary.

Measured plan ledger: one commit, from 92720d04acf154cb32a01f4c1d2e5bcc37c3fc8d through 7b6d8832058a89ea3ffc1dde18add4f6b832d35b. No metadata commit was made, as required by this scope.

## Published Source

- Parent: 92720d04acf154cb32a01f4c1d2e5bcc37c3fc8d.
- Commit: 7b6d8832058a89ea3ffc1dde18add4f6b832d35b.
- Branch and remote ref: phase/12-player-ready-delivery-and-playstead-bridge; the one normal push advertised this exact commit.
- The four commit paths were exactly .github/workflows/ci.yml, tests/policy/retroarch_preflight.cmake, tests/libretro/input_trace.cmake, and tests/retroarch/test_input_trace_helper.c. The last two matched their frozen pre-commit SHA256 values: 8ba0278f3edc9ea03c0e65833b7f2ad965e54cd333ea4d3e8335b8488b054cf9 and 38e1b1056ad8be0fe7f0ba3f7a2561225130e37a14ecbed74d0df836642d6b44.
- The normal pre-commit and pre-push hooks ran with the configured noreply identity; no bypass, amend, force push, PR, or merge was used.
- The bounded reviewer examined the four-path source delta once. Its PASS result also satisfied the configured post-execution review hook; no duplicate review ran.

## Verification

- cmake -DROOT="$PWD" -DWORK=build/ci/policy/retroarch_preflight -P tests/policy/retroarch_preflight.cmake: exit 0.
- cmake --workflow --preset ci: exit 0; all 677 CI tests and 6 release tests passed locally.
- git diff --check HEAD^ HEAD: exit 0.
- The positive mutation test created fixture outputs at the repository root that were absent from the initial snapshot; those generated outputs were removed after the check.
- The local policy needed its WORK path resolved against the repository root so generated checksum outputs were refreshed from the intended directory. The corrected policy pass/fault suite passed.

## Full CI Receipt

The sole dispatch command was:

    gh workflow run ci.yml --repo szTheory/muhtari --ref phase/12-player-ready-delivery-and-playstead-bridge -f keyboard_preflight_only=false -f keyboard_probe_only=false -f input_trace_diagnostic=false

Submission exit: 0. Run: https://github.com/szTheory/muhtari/actions/runs/41255515829

- T0: 2026-11-01 00:09:48Z. The cancellation cutoff was 02:09:48Z and the final observation endpoint was 02:19:48Z.
- The unique run was attempt 1, event workflow_dispatch, branch phase/12-player-ready-delivery-and-playstead-bridge, source 7b6d8832058a89ea3ffc1dde18add4f6b832d35b. All three dispatch inputs were false. Its 20-job expanded closure retained the 329 configured and 324 runnable runner-minute allocation.
- The run completed at 00:25:56Z, well before either time bound. Twelve jobs succeeded and the title check was skipped. Windows ARM64 failed; CI required failed; five remaining jobs were cancelled.
- In Windows ARM64, CTest case 103, built.openbios, timed out after 1.53 seconds. Its 15 dependent cases were not run; the log reports 661 of 677 tests passed, and the Build, test, package step exited 8. The exact failure annotation reports process exit code 8.
- After a fresh check confirmed the run identity and one active Windows x64 job, the single ordinary cancellation was submitted and succeeded. Windows x64 and the dependent RetroArch, WASM, quality, and hash-equality jobs ended cancelled. No retry, rerun, or second dispatch occurred.
- The terminal audit command, cmake -DINPUT=<saved terminal run JSON> -DEXPECTED_SHA=7b6d8832058a89ea3ffc1dde18add4f6b832d35b -P tools/check_ci_run.cmake, exited 1 because CI required was not completed/success.
- Fourteen artifacts were available. The macOS arm64 package artifact was tied to the expected commit and contained cores/muhtari_libretro.dylib and info/muhtari_libretro.info. The package ZIP SHA256 was 8ae72b0effb59dfee5186a302e687f3942937357b1f8a5f4deca832cdbcb540e; core member SHA256 was 146f064ea8e9b498bf6f1fcef52ca98cff35da192066dd23c338b3dc03214e3f; info member SHA256 was cb9f1d3838adc6235d6fe18fc67cd94a1745b61000c06ef5842ae937eb78f902. This proves that package artifact exists, not that the host loaded it.
- There was no Windows ARM64 archive or RetroArch artifact. The RetroArch job was cancelled before launch. No host executable/version identity or permission result was observed; no held or released images, input comparator verdicts, host DMA comparison, or two fresh-process battery results were produced. None is inferred from the local workflow or platform build artifacts.
- The GH CLI log-view command could not use its read-only local cache; the authenticated job-log API successfully supplied the terminal log. Complete receipts and downloaded artifacts remain outside the repository. No logs were committed.

## Deviations from Plan

### Auto-fixed Issues

1. Rule 3 - Blocking verification path: the policy WORK directory resolved relative to the policy script rather than the repository root. It was normalized so the policy refreshed the intended generated checksum files. The policy pass/fault suite and full local workflow passed. File: tests/policy/retroarch_preflight.cmake; commit: 7b6d8832058a89ea3ffc1dde18add4f6b832d35b.

The first required hosted failure ended this scope. No broader source or CI repair was attempted.

## Acceptance Outcome and Preserved State

- PLAY-03 is still pending, and accepted requirements remain 0/4.
- Phase 12 plan 12-01 remains halted; plans 12-02, 12-03, and 12-04 remain blocked.
- All earlier failed receipts and spent allowances remain unchanged. This failed run spends its one dispatch and one cancellation.
- The initial preservation inventory contained 885 tracked and non-ignored untracked paths. After execution, all unrelated path contents and the owner’s existing changes matched the baseline; the only source-content differences were the two explicitly editable files. The two pre-existing dirty source files included in the commit retain their frozen bytes.
- The locked fixture retained mode 0200 and size 12; its bytes remain unverified because ordinary reads were denied.
- The summary is intentionally uncommitted. STATE.md, ROADMAP.md, REQUIREMENTS.md, AGENTS.md, prior summaries and receipts, and docs/testing.md were not edited by this plan.

## Next Phase Readiness

Blocked pending diagnosis of the Windows ARM64 built.openbios timeout and later evidence adjudication. Do not accept the phase or start successors. Any additional hosted run needs a separately authorized allowance after the failure is understood.

## Self-Check: PASSED

- The summary file exists and remains untracked.
- Source commit 7b6d8832058a89ea3ffc1dde18add4f6b832d35b is an ancestor of HEAD.
- The measured plan ledger contains one commit.
- Preservation comparison found the expected single new summary path and no unexplained changes across the 885 baseline paths.
- The required no-index whitespace check emitted no whitespace diagnostics. Its exit was 1 because the untracked summary differs from /dev/null.
