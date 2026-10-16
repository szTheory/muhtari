---
phase: 01-a-test-frame-in-retroarch
plan: 15
subsystem: ci
tags: [github-actions, conventional-commits, jq, cmake, hygiene, policy]

requires:
  - phase: 01-14
    provides: ci.yml with the CI required roll-up, scripts/classify-changes.sh, draft probe PR 6
  - phase: 01-11
    provides: the hygiene lane, its inventory and the Fault prevented convention
provides:
  - "scripts/check-title.sh: Conventional Commit title with a lower-case subject, no release-cutting pipeline scope, no CI skip token in title or body"
  - "title job in ci.yml on pull requests, in CI required's needs"
  - "Hygiene tests policy.title, policy.ci_required (+ self test), ci.classify, hygiene.timeout_comments (+ self test)"
  - "apply-repo-settings.sh --check reports drift on sha_pinning_required"
  - "Pending todo for promoting wasm32 into CI required"
affects: [01-17 release workflow_call, 01-21 measured timeouts and probe close, the wasm32 promotion pull request]

actuals:
  tokens: 6800
  tasks: 3
  commits: 3
plan_head_before: 944bc73a68882d844005822ba25c0c7f3db3b388
plan_head_after: 3d085a20954aac60b7d50953ea7445ae6bf13337

tech-stack:
  added: []
  patterns:
    - "A gate over workflow text reads the workflow itself (jq program, needs list, timeout lines), so the test and the workflow cannot drift apart"
    - "A self test built by mutating a configure-time copy of the real file, so the mutation stops matching (and the self test fails) when the original changes shape"
    - "Inline sh test bodies as CMake bracket arguments, written without semicolons"

key-files:
  created:
    - scripts/check-title.sh
    - tests/hygiene/fixtures/titles.txt
    - tests/cmake/ci_required.cmake
    - tests/cmake/timeouts.cmake
    - .planning/todos/pending/2026-10-16-promote-the-wasm32-lane-into-ci-required.md
  modified:
    - .github/workflows/ci.yml
    - tests/hygiene/tests.cmake
    - tests/inventory/hygiene.txt
    - docs/testing.md
    - scripts/apply-repo-settings.sh
    - .planning/preparation/ENGINEERING.md
    - .planning/STATE.md

key-decisions:
  - "check-title.sh also rejects a skip token in the title, not only the body: the title becomes the squash commit's subject"
  - "Skip tokens match case-insensitively and accept skip-checks: true with a space; over-strict in the safe direction"
  - "policy.ci_required also requires every job but the report-only wasm32 in CI required's needs, so a new job cannot fail unseen"
  - "A timeout comment needs 'run' or 'runs' and nine or more digits, or provisional (the nesturbator rule)"
  - "apply-repo-settings.sh --check now reads sha_pinning_required, which is what makes hygiene.action_pins unnecessary"

patterns-established:
  - "Every new hygiene gate prints a count of what it checked, so a run that checked nothing is visible"

requirements-completed: [FRAME-01, FRAME-08]

coverage:
  - id: D1
    description: "scripts/check-title.sh passes Conventional Commit titles and fails plan-number scopes, Phase N titles, capitalised subjects, feat/fix/perf scoped ci/release/build/test/docs, and any CI skip token in title or body"
    requirement: FRAME-08
    verification:
      - kind: unit
        ref: "tests/hygiene/tests.cmake#policy.title (8 pass and 23 fail fixtures in tests/hygiene/fixtures/titles.txt)"
        status: pass
      - kind: other
        ref: "a copy of check-title.sh that always exits 0 makes the policy.title loop exit 1 naming feat(02-03): x"
        status: pass
    human_judgment: false
  - id: D2
    description: "The title job runs on pull requests with pull-requests: read, reads the live title and body with gh pr view, and is in CI required's needs; it is green on probe PR 6"
    requirement: FRAME-08
    verification:
      - kind: e2e
        ref: "gh run watch 38937116459 --exit-status (exit 0), https://github.com/szTheory/muhtari/actions/runs/38937116459, title success"
        status: pass
      - kind: e2e
        ref: "gh run watch 38937935977 --exit-status (exit 0), https://github.com/szTheory/muhtari/actions/runs/38937935977, 16 of 16 jobs success"
        status: pass
    human_judgment: false
  - id: D3
    description: "policy.ci_required runs the CI required jq program read from ci.yml, with the step's event, heavy and called arguments, on twelve synthetic needs cases, and requires every job but wasm32 in its needs"
    requirement: FRAME-01
    verification:
      - kind: unit
        ref: "tests/hygiene/tests.cmake#policy.ci_required (12 cases)"
        status: pass
      - kind: unit
        ref: "tests/hygiene/tests.cmake#policy.ci_required.self_test (program without the heavy guard fails naming build skipped when heavy=true)"
        status: pass
    human_judgment: false
  - id: D4
    description: "ci.classify runs scripts/classify-changes.sh on mixed, renamed, empty, blank, failed-listing, look-alike and planning-only lists"
    requirement: FRAME-01
    verification:
      - kind: unit
        ref: "tests/hygiene/tests.cmake#ci.classify (9 lists)"
        status: pass
      - kind: other
        ref: "a copy of the classifier without its empty-input rule makes the ci.classify body exit 1 naming the empty list"
        status: pass
    human_judgment: false
  - id: D5
    description: "hygiene.timeout_comments requires every timeout-minutes line under .github/workflows to carry run plus a run ID or provisional"
    requirement: FRAME-01
    verification:
      - kind: unit
        ref: "tests/hygiene/tests.cmake#hygiene.timeout_comments (10 timeouts)"
        status: pass
      - kind: unit
        ref: "tests/hygiene/tests.cmake#hygiene.timeout_comments.self_test (fixture with two bad lines)"
        status: pass
    human_judgment: false
  - id: D6
    description: "ENGINEERING section 4 says why hygiene.action_pins is not built, and apply-repo-settings.sh --check reports sha_pinning_required drift; section 2 names the libc fixture as abi.undefined's negative control"
    requirement: FRAME-01
    verification:
      - kind: other
        ref: "sh scripts/apply-repo-settings.sh --check szTheory/muhtari exit 0 (no drift; live sha_pinning_required true)"
        status: pass
      - kind: other
        ref: "grep -q libc_fixture .planning/preparation/ENGINEERING.md and ! grep -q 'must fail all three' exit 0; grep -c action_pins prints 1"
        status: pass
    human_judgment: false
  - id: D7
    description: "The folded wasm32 todo is in todos/done, and a pending ci todo tracks its promotion into CI required, listed in STATE.md"
    requirement: FRAME-01
    verification:
      - kind: other
        ref: "test -f .planning/todos/done/2026-10-15-consider-a-wasm32-build-and-hash-lane-in-phase-1-ci.md and test ! -f the pending copy, exit 0; grep Promote .planning/STATE.md"
        status: pass
    human_judgment: false

duration: 13min
completed: 2026-10-16
status: complete
---

# Phase 1 Plan 15: Pull-request titles checked, and the CI rules themselves tested Summary

**`scripts/check-title.sh` holds pull-request titles to the Conventional Commit pattern, and a `title` job in `CI required`'s needs runs it on the live title and body. Five new hygiene tests run the roll-up's own `jq` program on twelve synthetic cases, run the classifier on nine path lists, and require a measured or provisional comment on every timeout. Both probe-PR runs are green.**

## Performance

- **Duration:** about 13 min
- **Started:** 2026-10-16T17:02:18Z
- **Completed:** 2026-10-16T17:14:47Z
- **Tasks:** 3
- **Files modified:** 13 (355 insertions, 4 deletions)

## Accomplishments

- **Title check.** `check-title.sh` uses ENGINEERING §4's regex. It also has a second pattern for pipeline scopes, and it scans both title and body for the six skip tokens. `policy.title` runs 31 fixtures, 8 that should pass and 23 that should fail. The test fails if the fixtures do not hold both kinds.
- **`title` job.** It runs on pull requests only, with `pull-requests: read`, and it is in `CI required`'s needs. The `edited` trigger reruns it when the title changes.
- **Roll-up test.** `policy.ci_required` reads the roll-up program and its `--arg` names from `ci.yml` itself. Each of the 12 cases maps to a clause in the plan's behaviour list. Two cases are extra: a skip after `changes` failed (heavy is empty) fails, and a cancelled build fails. The self test removes the `heavy` guard from a copy of `ci.yml`, and the run fails naming `build skipped when heavy=true`. The same copy also lets a skipped `perf` through on a pull request.
- **Classifier and timeouts.** `ci.classify` covers mixed, renamed, empty, blank-line, `(listing failed)`, `.planning-notes/` and `docs/.planning/` lists. `hygiene.timeout_comments` found 10 timeouts, all marked provisional.
- **Records.** ENGINEERING §2 and §4 now match what was built. The wasm32 promotion is a pending todo.

## Evidence (command, exit status, run URL)

- Task 1 verify:
  - `cmake --workflow --preset hygiene` exited 0 with 8 tests.
  - `gh run watch 38937116459 --exit-status` exited 0, and the `title` job concluded `success`: https://github.com/szTheory/muhtari/actions/runs/38937116459
- Task 2 verify: `cmake --workflow --preset hygiene` exited 0 with 13 tests.
- Task 3 verify: the plan's `test -f …` chain exited 0, and `grep -c action_pins` printed 1.
- Plan verification:
  - Head `3d085a2`, `gh run watch 38937935977 --exit-status` exited 0. All 16 jobs succeeded, `title` and `CI required` among them, and hosted `check_junit` reported 13 hygiene tests run once each: https://github.com/szTheory/muhtari/actions/runs/38937935977
- Local lanes:
  - `cmake --workflow --preset ci` exited 0 (56 tests, then 4).
  - `cmake --workflow --preset hygiene` exited 0 (13 tests).
- Acceptance checks:
  - `echo | scripts/check-title.sh 'feat(02-03): add x'` exited 1.
  - `'ci: probe the phase workflows'` and `'feat: show a test card in RetroArch'` each exited 0.
  - A scratch `ci.yml` with `$heavy == "false" and ` removed made `ci_required.cmake` exit 1.

## TDD note (Task 2)

The code under test already existed: the roll-up program in `ci.yml` and `classify-changes.sh`. So the RED evidence is a set of negative runs on broken copies, not a commit before the code existed:

- `ci_required.cmake` on the copy without the heavy guard: exit 1, naming `build skipped when heavy=true` and `perf skipped on a pull request`.
- The `ci.classify` body on a classifier without its empty-input rule: exit 1, naming the empty list.
- `timeouts.cmake` on the two-bad-line fixture: exit 1.

The same runs on the real files exit 0. Task 2 went in as one `test(01-15)` commit, because every file in it is test code.

## Gate-code tally

- **New this plan:**
  - `scripts/check-title.sh`: 25 lines
  - `tests/cmake/ci_required.cmake`: 90 lines
  - `tests/cmake/timeouts.cmake`: 33 lines
  - `tests/hygiene/tests.cmake`: +100 lines, now 160
  - `ci.yml`: +23 lines, now 300
  - `apply-repo-settings.sh`: +3 lines
- **All gate scripts:**
  - `tests/cmake/*.cmake` plus `cmake/MuhtariTests.cmake`: 1,109 lines
  - `scripts/*.sh`: 383 lines
- **Code under test:** `src/` is 315 lines. The tenth-of-`src/` ceiling applies from phase 3.

## Seeds

- Fetched with `gh api` into a scratch directory outside the repository, from nesturbator `9f9386c661f6` (published main; `3200aa868371` is not on GitHub):
  - `tests/cmake/ci_required_rollup.cmake`, 112 lines
  - `tests/cmake/timeout_comments.cmake`, 62 lines
- **Taken:** the needs-membership check, `jq` on synthetic needs, and the `runs? <nine digits>` rule.
- **Rewritten lean, with no lines copied:** 90 and 33 lines. The commit `b7bee69` body cites the seed.

## Task Commits

1. **Task 1: Title check end to end** - `08cda08` (ci)
2. **Task 2: Roll-up, classification and timeout-comment tests** - `b7bee69` (test)
3. **Task 3: ENGINEERING amendments and the wasm32 todos** - `3d085a2` (docs)

## Files Created/Modified

- `scripts/check-title.sh`: title and skip-token check
- `tests/hygiene/fixtures/titles.txt`: 31 `want|title|body` fixtures
- `tests/cmake/ci_required.cmake`: runs the roll-up program and checks the needs list
- `tests/cmake/timeouts.cmake`: timeout comment rule
- `tests/hygiene/tests.cmake`, `tests/inventory/hygiene.txt`: six new tests
- `.github/workflows/ci.yml`: `title` job, added to `CI required`'s needs
- `docs/testing.md`: `title` row, "The title" paragraph, "Tests of CI itself" paragraph
- `scripts/apply-repo-settings.sh`: `--check` reads `sha_pinning_required`
- `.planning/preparation/ENGINEERING.md`: §2 integer-only row and §4 Pins
- `.planning/todos/done/2026-10-15-…`: moved from pending; `.planning/todos/pending/2026-10-16-promote-the-wasm32-lane-into-ci-required.md`: new
- `.planning/STATE.md`: Pending Todos

## Decisions Made

See `key-decisions` in the frontmatter.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 2 - Missing critical] Skip token in the title**
- **Found during:** Task 1
- **Issue:** The plan checked only the body for skip tokens. The title becomes the squash commit's subject, so `ci: x [skip ci]` would also start no run on `main`.
- **Fix:** The script checks the title too. A fixture covers it.
- **Committed in:** 08cda08

**2. [Rule 1 - Bug] A closed pipe under pipefail**
- **Found during:** Task 1
- **Issue:** `grep -q` on standard input can exit early. `gh pr view` would then die of SIGPIPE under the job's `pipefail`, the same fault plan 14 found in the classifier.
- **Fix:** The script reads the whole body first.
- **Committed in:** 08cda08

**3. [Rule 2 - Missing critical] A job left out of the roll-up**
- **Found during:** Task 2
- **Issue:** The roll-up cannot see a failure in a job that is missing from its needs.
- **Fix:** `policy.ci_required` requires every job but those named in `-DREPORT_ONLY` (now `wasm32`) to be in the needs. A run without `REPORT_ONLY` fails, naming `wasm32`. The file grew from about 50 lines to 90.
- **Committed in:** b7bee69

**4. [Rule 1 - Bug] The ENGINEERING claim about `--check` was not yet true**
- **Found during:** Task 3
- **Issue:** The planned §4 text says `apply-repo-settings.sh --check` watches `sha_pinning_required`, but `--check` did not read that setting.
- **Fix:** Three lines were added to `--check`. The live run reports no drift.
- **Files modified:** `scripts/apply-repo-settings.sh`, which is not in the plan's file list
- **Committed in:** 3d085a2

**5. [Docs] docs/testing.md updated**
- **Found during:** Tasks 1 and 2
- **Issue:** Rule 5 of AGENTS.md says behaviour lands with its docs, and `docs/testing.md` is not in the plan's file list.
- **Fix:** Added the `title` row and two short paragraphs.

---

**Total deviations:** 5 (2 missing critical, 2 bugs, 1 docs).
**Impact on plan:** All five are small, and none changes a locked decision.

## Issues Encountered

- CMake's `message(FATAL_ERROR)` reflows its text. The timeout self test's regex therefore uses `.*` between lines instead of literal newlines.

## Threat Flags

| Flag | File | Description |
|------|------|-------------|
| threat_flag: candidate-controlled-classifier (carried from plan 14) | .github/workflows/ci.yml | `ci.classify` catches an accidental break: a classifier that marks code, a rename, an empty list or a failed listing as planning-only fails the hygiene job, and the hygiene job is never skipped. It does **not** stop a deliberate edit. A pull request that changes `classify-changes.sh` and the `ci.classify` lists together still marks itself light, because both run from the pull request's own head. Review of any change to `scripts/` or `tests/hygiene/` is the control for that. The exposure is bounded: a light verdict skips only the heavy jobs, while `hygiene` and `title` still run. |
| threat_flag: candidate-controlled-title-script | .github/workflows/ci.yml | The `title` job also runs `check-title.sh` from the pull request's head, so a pull request can weaken its own title check. The job reads only the title and body with a read-only token. The squash title is visible in review. |

## User Setup Required

None.

## Next Phase Readiness

- Plan 17's `release.yml` call passes `ref`. On a `push` caller, `title` and `perf` skip as the roll-up allows, and `policy.ci_required` covers both cases.
- Plan 21 sets the measured timeouts. `hygiene.timeout_comments` will then require `run <id>` on each.
- The probe PR stays a draft and open.

---
*Phase: 01-a-test-frame-in-retroarch*
*Completed: 2026-10-16*

## Self-Check: PASSED

- FOUND: scripts/check-title.sh, tests/hygiene/fixtures/titles.txt, tests/cmake/ci_required.cmake, tests/cmake/timeouts.cmake, the promotion todo
- FOUND commits: 08cda08, b7bee69, 3d085a2 (ancestors of HEAD)
