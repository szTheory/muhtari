---
phase: 05-pipeline-tune-up
plan: "03"
status: halted
subsystem: infra
tags: [cmake, github-actions, powerpc, portability]
requires:
  - phase: 05-01
    provides: Fixed complete maintenance allocation and counter
  - phase: 05-02
    provides: Independent writers and measured scheduling bounds
provides:
  - Required PR PowerPC compilation and exact six-native-file comparison
  - Controlled same-SHA native/PowerPC evidence producer and release wiring
  - Reproduced full-inventory PowerPC ABI failures with a durable integration halt
affects: [05-04, 05-05, 05-06, 05-07, 05-08, 05-09]
tech-stack:
  added: []
  patterns: [read-only reusable equality workflow, exact artifact identity controls]
key-files:
  created:
    - .github/workflows/be-equality.yml
    - tests/cmake/lane_equality.cmake
    - .planning/phases/05-pipeline-tune-up/05-03-ALLOCATION.md
    - .planning/phases/05-pipeline-tune-up/05-03-HALTED.md
  modified:
    - .github/workflows/ci.yml
    - .github/workflows/nightly.yml
    - .github/workflows/release.yml
    - CMakePresets.json
    - tests/install/tests.cmake
    - tests/cmake/ci_required.cmake
    - tests/hygiene/tests.cmake
    - tests/policy/tests.cmake
    - tests/inventory/ci.txt
    - tests/policy/gate-lines.txt
    - docs/testing.md
key-decisions:
  - Keep C14853 and N1970 fixed; move 100 unused count lines and 80 reserve lines to BE300 before expansion.
  - Isolate the read-only reusable equality producer because GitHub validates the skipped reporter's issue-write permission.
  - Halt full BE integration on reproduced ABI failures after the fix-attempt limit; preserve controls and block later plans.
duration: 24min
completed: 2026-10-21
actuals:
  tokens: 6558
  tasks: 2
  commits: 8
plan_head_before: 613861b7ad8f2b9acae79d85adc5aa2b6f9d0c34
plan_head_after: d1b443506bac6d20c3f4cc0a1d6e51d87faa7249
requirements-completed: []
requirements-contributed: [TUNE-02, TUNE-04, TUNE-05]
coverage:
  - id: pr-compile
    description: Required PowerPC compilation and six native hash membership
    requirement: TUNE-02
    verification:
      - kind: unit
        ref: ci_required.cmake 15 cases and hygiene omission/self-test controls, exit 0
        status: pass
      - kind: integration
        ref: https://github.com/szTheory/muhtari/actions/runs/39651796047
        status: pass
    human_judgment: false
  - id: pair-controls
    description: Exact nonempty independent same-SHA and configuration evidence
    requirement: TUNE-05
    verification:
      - kind: unit
        ref: policy.lane_equality.controls valid pair and seven named faults, exit 0
        status: pass
      - kind: integration
        ref: cmake --workflow --preset ci, 607 normal and six release tests, exit 0
        status: pass
    human_judgment: false
  - id: full-be
    description: Full PowerPC normal and package inventories followed by native equality
    requirement: TUNE-02
    verification:
      - kind: integration
        ref: https://github.com/szTheory/muhtari/actions/runs/39652860054
        status: fail
    human_judgment: false
  - id: release-binding
    description: Read-only reusable equality bound to rehearsal and actual release SHA
    requirement: TUNE-04
    verification:
      - kind: integration
        ref: https://github.com/szTheory/muhtari/actions/runs/39651657362
        status: unknown
    human_judgment: false
---

# Phase 5 Plan 3: PowerPC coverage handoff Partial Summary

**Required PR PowerPC compilation and controlled equality wiring are committed;
full execution exposed ABI failures and blocks completion.**

Status is **halted**, not complete. Tasks 1 and 2 passed their local automated
verification; Task 3's mandatory hosted full-inventory/equality acceptance is
incomplete. No requirement is marked complete. Phase progress remains 2/9;
05-04 onward are blocked. Recovery details are in `05-03-HALTED.md`.

## Partial task commits

1. `33760b3` — Required PR compilation, exact six-native hashes, omission/failure/skip controls, full presets.
2. `4a0851d` — RED identity control and visible fixed-total redistribution.
3. `c752d7f` — GREEN identity validation and full normal/package/release producer.
4. `3f20455` — Rehearsal/release immutable-SHA calls and concise coverage documentation.
5. `82605b8` — Read-only reusable equality extraction after actual GitHub permission rejection.
6. `f26af3d` — Cross-built consumers receive the target toolchain and emulator.
7. `e0c2981` — Staged cross-package lookup and complete new-workflow/full-suite triggers.
8. `d1b4435` — Five-minute compile-only bound from measured hosted successes.

The persisted ledger measures eight commits between the recorded heads.
Actual tokens are ceil(26,231 realized diff characters / 4), not agent tokens;
halt/summary/state metadata are outside that pre-metadata measurement.

## Verification evidence

- `cmake --workflow --preset ci` — exit 0 after the final test-source changes:
  607 normal tests and six archive checks. A prior invocation failed only the
  four-byte docs overrun; the docs were shortened and the whole workflow passed.
- `cmake -DCI_YML=.github/workflows/ci.yml -DJQ=jq -DREPORT_ONLY=wasm32,library -P tests/cmake/ci_required.cmake`
  — exit 0, 15 cases, including required BE failure/skip and native hash skip.
  Hygiene's omitted-BE dependency and heavy-skip self-test also passed.
- `cmake --preset ci && ctest --preset ci -R '^policy[.]lane_equality[.]controls$' --output-on-failure`
  — exit 0. The valid pair passes; wrong SHA/configuration, empty hashes, missing
  member, extra member, duplicate lane and byte difference each reject by the
  named fault. `cmake --list-presets=workflow` — exit 0.
- `ctest --preset ci -R '^(install[.]|embed[.])' --output-on-failure`
  — exit 0, four native checks after cross-consumer changes.
- `cmake -DROOT=. -P tests/cmake/budgets.cmake` and `git diff --check` — exit 0.
  `docs/testing.md` is 40,955/40,960 bytes. No budget increased.
- `actionlint -shellcheck='' -ignore 'label .* is unknown' -ignore 'unexpected key "queue"'`
  on the four changed workflow files — exit 0. These exclusions cover the
  installed tool's stale runner/concurrency schema; this is a syntax check,
  not a claim that all inherited shellcheck findings were fixed.

Hosted commands `gh run view RUN --json status,conclusion,jobs,url` and the
specific job-log API commands below returned exit 0. Raw logs stay outside git.

| Run | Observed result and scope |
|---|---|
| [39651796047](https://github.com/szTheory/muhtari/actions/runs/39651796047) | All six native build/test/package legs, exact six-file hash comparison and required PowerPC compilation succeeded at f26af3d. The overall superseded run was later cancelled. |
| [39651391808](https://github.com/szTheory/muhtari/actions/runs/39651391808) | Startup failure: skipped nested nightly reporter requests issues:write, caller permits issues:none. |
| [39651657362](https://github.com/szTheory/muhtari/actions/runs/39651657362) | Read-only extracted equality and called CI start successfully; superseded run cancelled. This proves permission admission, not complete equality or publication. |
| [39651391703](https://github.com/szTheory/muhtari/actions/runs/39651391703) | First full selection exposes ABI failures and direct guest consumer execution errors; obsolete run cancelled. |
| [39652860054](https://github.com/szTheory/muhtari/actions/runs/39652860054) | Final implementation d1b4435 directly reproduces both PowerPC ABI failures. Cancelled after 6m35s because known failures cannot satisfy integration. Native 607 normal and six archive tests completed first; BE archive/equality proof is absent. |
| [39652859953](https://github.com/szTheory/muhtari/actions/runs/39652859953) | Final-head required PowerPC compilation succeeds. The run is cancelled with one native leg queued; no final roll-up success is claimed. |

`gh api --allow-escape-sequences repos/szTheory/muhtari/actions/jobs/119405429568/logs`
— exit 0. At 18:33:03 UTC the final-head PowerPC `abi.undefined` rejects
`__divdi3`; `abi.globals` rejects writable `D` symbols:
`muhtari__cartdb_2600_prefix_count`, `muhtari__cartdb_7800_entry_count`,
`muhtari__cartdb_7800_key_count`, `muhtari__cartdb_7800_layout_count`.
Both existing fault controls pass immediately afterward. The run's cancellation
is not substituted for either a green result or a complete failed JUnit inventory.

`git diff 3f20455..d1b4435 -- src include tests/cmake/abi.cmake tests/abi cmake/toolchains/ppc-linux-gnu.cmake`
— exit 0, empty. No ABI predicate, core data, target toolchain or allowlist was
changed to hide these faults. The successful full-BE timing cohort has n=0;
the 6m35s cancelled duration is censored, never a timeout-sizing success.

## TDD gate compliance

Task 2 is task-level TDD within an execute plan. RED:
`ctest --preset ci -R '^policy[.]lane_equality[.]controls$' --output-on-failure`
— exit 8. The valid comparison executed, then the wrong-SHA case failed its
expected rejection because the comparator returned 0. This was a semantic
assertion failure, not missing-file/setup failure. RED commit: `4a0851d`.
GREEN adds identity/member/nonempty/independent-directory checks and reuses
`hashes.cmake` with COUNT=2: the same command exits 0 (`c752d7f`). One control
regex was corrected for CMake's line wrapping; refactoring was unnecessary.
The phase's optional MVP/TDD runtime mode was false; no mode bypass occurred.

## Coverage and timing limits

PR PowerPC **runtime** coverage is removed explicitly. Compilation of all
three deliverables remains required; six native package legs and their exact
runtime membership remain. Full normal and package-dependent PowerPC checks
are selected nightly/release, with separate JUnit judges, but their replacement
coverage is **not accepted** while the ABI failures and missing package/equality
proof remain open. The phase cannot ship on the partial handoff.

Two direct compile-only successes take 33 s and 29 s on ubuntu-26.04,
runs 39651391712 and 39651796047. Maximum 33 s, twice maximum 66 s; the
five-minute network floor governs. n=2 is provisional. Full runtime's 30-minute
bound remains provisional with no completed success sample. The metadata
compares identical checkout SHA, pinned fixture/configuration identities and
semantic options; architecture/compiler/emulator and timeout scale are the
explicit intentional differences. There is no comparison against latest main.

## Allocation and count

`05-03-ALLOCATION.md` records redistribution **before** adding the producer:
BE 120→300, count/inventory 500→400 (368 used), reserve 150→70. Digest650,
quality450 and reproduced fixes100 are unchanged. N1970 and C14853 stay fixed.

`cmake -DROOT=. -DINVENTORY=tests/policy/gate-lines.txt -DOUT=build/ci/gate-count.json -P tests/cmake/gate_count.cmake`
— exit 0: **13,438** physical lines, **186/300** incremental BE lines,
1,415 lines below C. All workflow/preset/helper/control/inventory/consumer glue
and the new reusable workflow are charged as whole files. No exclusions or line
golf were introduced. Full/authored source denominators remain 8577/6530;
ratios 156.67%/205.79% are advisory, not hard-ratio compliance claims.

## Deviations from plan

1. **[Rule 3 — blocking integration] Reusable workflow privilege separation.**
   Actual GitHub startup rejection required moving the producer into
   `be-equality.yml`, called by nightly and release, while the existing issue
   reporter remains solely in direct nightly. Read-only callers start without
   issue permission. Approved by the orchestrator within the existing coverage
   scope; commits `82605b8` and `e0c2981`. Ledger entry 10 is fixed.
2. **[Rule 3 — blocking integration] Cross-built consumers.** The newly selected
   install/embed tests launched target binaries without qemu. They now inherit
   the toolchain/emulator. A separate controlled cross-root configure failed
   with the staging package hidden (exit 1) and passed with CMAKE_STAGING_PREFIX
   (exit 0); commits `f26af3d` and `e0c2981`. Native controls pass; complete
   PowerPC consumer acceptance remains subject to the halted full run.
3. **D-20/fix-attempt halt.** The three task-3 fix attempts were privilege
   separation, consumer execution/toolchain, and staging lookup. Further ABI
   changes are stopped. No additional scope, relaxed predicate, gap plan or
   owner rubber-stamp was substituted for the failed integration.

## Deferred issues and unrun verification

- Both PowerPC ABI failures remain open (WINDOWS entry 11).
- PowerPC archive inventory, complete no-skip JUnit and same-head native/BE
  equality remain unverified (WINDOWS entry 12). Release/rehearsal execution was
  cancelled before completion; no release or post-merge truth is claimed.
- No new stub or skipped test was introduced. Existing unrelated ledger entries
  remain unchanged. Diagnostic commands and the exact resume gate are in the
  HALTED artifact. State/roadmap retain 2/9 complete and block 05-04 onward.

## Self-Check: PASSED (halt record)

All 14 committed changed paths plus SUMMARY and HALTED files exist; all eight
recorded implementation commits are ancestors of HEAD. The persisted ledger
measures eight commits. No tracked file was deleted. This checks the partial
record only; the failed/full-inventory acceptance gate remains explicitly open.
