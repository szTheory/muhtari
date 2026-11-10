---
phase: 14-proven-frontend-states-and-responsive-play
plan: "06"
subsystem: testing
tags: [libretro, retroarch, ctest, cmake, github-actions, asan]

requires:
  - phase: 14-proven-frontend-states-and-responsive-play
    provides: Existing state controls and the Phase 14 host evidence contract
provides:
  - Portable POSIX declarations and helper guard in the existing test consumer
  - Audited one-run host outcome with frontend qualification left pending
affects: [FRONT-02, FRONT-03, FRONT-04, docs/testing.md]

actuals:
  tokens: 17257.75
  tasks: 3
  commits: 3
  plan_head_before: b338797847ba4a6a4c70d1318e20a6b0c1800605
  plan_head_after: 57224203379fcf1e887a3c15cff41ec221956e2c

tech-stack:
  added: []
  patterns:
    - Host evidence is valid only for a completed, source-bound host job with matching receipt identity
key-files:
  created:
    - .planning/phases/14-proven-frontend-states-and-responsive-play/14-06-SUMMARY.md
  modified:
    - tests/retroarch/states.c
    - tests/diag/driver.c
    - tests/libretro/host.c
    - tests/retroarch/compare.c
    - docs/libretro.md
    - docs/testing.md
    - .planning/phases/14-proven-frontend-states-and-responsive-play/14-RESOURCE-EVIDENCE.md
    - .planning/phases/14-proven-frontend-states-and-responsive-play/deferred-items.md
    - .planning/LESSONS.md
key-decisions:
  - Local macOS arm64 ASan is unavailable after repeatable pre-main hangs; the hosted Ubuntu x64 ASan lane passed.
  - FRONT-02, FRONT-03, and FRONT-04 remain pending because Windows builds failed and the host job was skipped.
requirements-completed: []
coverage:
  - id: D1
    description: The existing test consumer passes local CI, hygiene, focused state controls, and exact-parent checks; the hosted Windows matrix still fails to compile.
    verification:
      - kind: integration
        ref: "Local `cmake --workflow --preset ci`, inventories, hygiene and focused controls: exit 0; exact-parent CI/hygiene/inventories: pass."
        status: pass
      - kind: integration
        ref: "[Run 41618546347](https://github.com/szTheory/muhtari/actions/runs/41618546347): Windows x64 and ARM64 fail on MSVC C4100 for `self_path`; hosted Ubuntu x64 ASan passes."
        status: fail
    human_judgment: false
  - id: D2
    description: Actual RetroArch save/load progress and picture evidence for FRONT-02 was not produced.
    requirement: FRONT-02
    verification:
      - kind: integration
        ref: "[Run 41618546347](https://github.com/szTheory/muhtari/actions/runs/41618546347), attempt 1: host job skipped; `tools/check_ci_run.cmake` exit 1; no `retroarch-frames` artifact."
        status: unknown
    human_judgment: false
  - id: D3
    description: Actual RetroArch rewind reversals for FRONT-03 were not observed.
    requirement: FRONT-03
    verification:
      - kind: integration
        ref: "[Run 41618546347](https://github.com/szTheory/muhtari/actions/runs/41618546347), attempt 1: host job skipped; `tools/check_ci_run.cmake` exit 1; no `retroarch-frames` artifact."
        status: unknown
    human_judgment: false
  - id: D4
    description: Actual RetroArch run-ahead, containment, and committed-storage evidence for FRONT-04 was not produced.
    requirement: FRONT-04
    verification:
      - kind: integration
        ref: "[Run 41618546347](https://github.com/szTheory/muhtari/actions/runs/41618546347), attempt 1: host job skipped; `tools/check_ci_run.cmake` exit 1; no `retroarch-frames` artifact."
        status: unknown
    human_judgment: false
  - id: D5
    description: User and maintainer documentation records the failed host qualification and remains within repository documentation policy.
    verification:
      - kind: unit
        ref: "`ctest --preset ci -R '^docs\\.(budgets|api_names(\\.self_test)?)$' --no-tests=error --output-on-failure`: exit 0; `git diff --check HEAD --` the five documentation files: exit 0."
        status: pass
    human_judgment: false

duration: ">= 22 min (from earliest plan task commit)"
completed: 2026-11-03
status: complete
---

# Phase 14 Plan 06: Proven Frontend States and Responsive Play Summary

**Portable test-source repairs passed local checks, but the single authenticated CI dispatch failed Windows builds before RetroArch ran; actual-host requirements remain pending.**

## Performance

- **Duration:** At least 22 minutes between the first task commit and the documentation task commit; the earlier plan start was not captured in this continuation.
- **Started:** Earliest task commit at 2026-11-03T10:16:08Z
- **Completed:** 2026-11-03T10:38:15Z
- **Tasks:** 3
- **Files modified:** 9

## Accomplishments

- Added `_XOPEN_SOURCE 700` for the existing POSIX `realpath` use and limited the monotonic clock helper to POSIX callers. The four changed C test sources pass the pinned formatter; the independent read-only gap-diff review passed.
- Local CI, release and ordinary inventories, hygiene, hygiene inventory, and focused state controls passed. The exact immediate-parent checkout passed warmed CI, hygiene, and all inventories. Local macOS arm64 ASan remained unavailable after unrelated tests also hung before `main`.
- Pushed the frozen source once and verified the advertised remote SHA matched. Exactly one `phase14_host=true` dispatch ran at source `0869c655008fc5344c428313ae519a91f5e9db42`, attempt 1. Ubuntu x64 ASan passed, but both Windows jobs failed with MSVC C4100 for unused `self_path`; the required artifact join failed and RetroArch was skipped. No host receipt or `retroarch-frames` artifact exists.
- Updated docs and deferred notes to preserve the failed history and state that actual-host save/load, rewind, and run-ahead qualification is still pending. No retry, job rerun, or local frontend launch occurred.

## Task Commits

1. **Task 1: Restore portable frontend test builds** — `a51732e` (`fix(14-06): restore portable frontend test builds`)
2. **Task 2: Freeze preflight evidence for the bounded dispatch** — `0869c65` (`docs(14-06): record source preflight evidence`)
3. **Task 3: Record the measured qualification outcome** — `5722420` (`docs(14-06): record bounded host outcome`)

The plan commits measured from `plan_head_before` to `plan_head_after` are 3. The summary metadata commit is separate.

## Files Created/Modified

- `tests/retroarch/states.c`, `tests/diag/driver.c`, `tests/libretro/host.c`, `tests/retroarch/compare.c` — portable declarations/helper guard and pinned formatting.
- `14-RESOURCE-EVIDENCE.md` — local and exact-parent evidence, dispatch identity and result, artifact audit, queue/critical-path observations, and exhausted allowance.
- `docs/libretro.md`, `docs/testing.md` — qualification and receipt rules tied to completed source-bound evidence.
- `deferred-items.md`, `.planning/LESSONS.md` — remaining Windows compiler blocker and reproduced lesson.

## Decisions Made

- Treat the local macOS arm64 ASan hang as a platform limitation, not a passing result or code defect; require the hosted Ubuntu x64 ASan lane, which passed.
- Keep FRONT-02 through FRONT-04 pending because the actual RetroArch job did not execute. The bounded dispatch allowance is spent; obtaining fresh host evidence needs a separately authorized scope.
- Make no CI topology, timeout, or cache changes from this single run. Its critical path was the macOS x64 build/quality-observation job, with no cancelled jobs or steps.

## Deviations from Plan

### Owner-directed ASan platform limitation

- **Found during:** Task 1
- **Issue:** Local macOS arm64 ASan workflow and focused controls hung before test output, including an unrelated API test. Reproductions outside the sandbox showed the same pre-main behavior.
- **Resolution:** Per the owner's direction, recorded local ASan as unavailable and continued with exact-parent checks; the source-bound hosted Ubuntu x64 ASan job passed.
- **Evidence:** `14-RESOURCE-EVIDENCE.md`; local workflow exit 130 and focused control exit 8; [hosted run 41618546347](https://github.com/szTheory/muhtari/actions/runs/41618546347).

### Cold exact-parent run required a warm pass

- **Found during:** Task 2
- **Issue:** The first cold parallel parent CI run exited 8 on three BIOS tests.
- **Resolution:** Serial focused diagnostics with fixtures passed 5/5; a warmed full CI workflow, hygiene, and all inventories then passed. No source or timeout was changed.
- **Evidence:** ignored receipts under `build/phase14-dispatch-evidence/parent-*` and `14-RESOURCE-EVIDENCE.md`.

**Total deviations:** 2. The host workflow's Windows failure followed the plan's explicit failed-result path: preserve the result, leave requirements pending, and stop live actions without retry.

## Issues Encountered

- The hosted Windows x64 and ARM64 compilers report C4100 for `self_path` at three existing state-test call sites under `/W4 /WX`; C2220 stops both jobs. The missing producer artifacts cause the quality join and required aggregate to fail. The RetroArch host job is skipped, so no actual-host behavior is established.
- The docs budget check first found `docs/testing.md` 114 bytes over its limit. The paragraph was shortened; the affected docs checks and `git diff --check` then passed.

## Stub and Threat Surface Review

No new TODO, FIXME, placeholder, or empty UI/data stubs were found in the changed files. The repair changes no network, authentication, file-access, or schema boundary; no new threat flags were found.

## Next Phase Readiness

The plan's implementation and bounded evidence bookkeeping are complete, but FRONT-02, FRONT-03, and FRONT-04 remain pending for the orchestrator's verifier. The exact blocker is the Windows MSVC C4100 failure and the resulting skipped host job; the consumed allowance cannot be reused. No additional action is authorized within this plan.

## Self-Check: PASSED

The summary exists, and all three task commits are ancestors of the current plan head recorded above.

---
*Phase: 14-proven-frontend-states-and-responsive-play*
*Completed: 2026-11-03*
