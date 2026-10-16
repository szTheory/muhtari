---
phase: 01-a-test-frame-in-retroarch
plan: 21
subsystem: infra
tags: [ci, timeouts, github-ruleset, repository-settings, release-please, gsd-config]

requires:
  - phase: 01-a-test-frame-in-retroarch
    provides: "the committed ruleset and the full apply-repo-settings.sh --check (plan 20), the CI required roll-up (plans 14, 15), the release rehearsal and nightly workflows (plans 18, 19)"
provides:
  - "Measured timeout-minutes on every job with at least 10 successful samples; the rest stay provisional with their sample counts"
  - "Measured emulator timeout scales: be32 11, wasm32 3"
  - "workflow.test_gate_timeout 3, twice the measured 1.36 s ci workflow"
  - "The live main ruleset requiring CI required, applied by the owner, with no drift"
  - "Probe PR #6 closed with the branch kept, and the feat: ship title recorded"
affects: [ci, release, repository-settings, gsd-ship, phase-02]

actuals:
  tokens: 3434
  tasks: 3
  commits: 2

plan_head_before: c1aec3ef5e0060553136b56702560afb495f4cf0
plan_head_after: 59eaf9b63ceea5a133efd41aa54f0efe2f77d818

tech-stack:
  added: []
  patterns:
    - "A timeout is twice the slowest successful run, rounded up, with a 5-minute floor for network-bound and roll-up jobs; the comment names the run ID and the sample count, or says provisional"

key-files:
  created: []
  modified:
    - .github/workflows/ci.yml
    - .github/workflows/release.yml
    - .github/workflows/nightly.yml
    - .github/workflows/dependabot-automerge.yml
    - CMakePresets.json
    - docs/testing.md
    - .planning/config.json
    - .planning/STATE.md

key-decisions:
  - "release.yml's nested ci jobs count as samples for the same ci.yml jobs, giving 10 or more samples for every ci.yml job"
  - "be32 scale 11 (measured 10.72x) and wasm32 scale 3 (measured 2.53x), from ctest per-test times in 10 CI runs on tests shared with Linux x64"
  - "workflow.test_gate_timeout is 3: twice the measured 1.36 s ci workflow, rounded up; it must be re-measured when the suite grows (phase 2 adds the CPU tests)"
  - "The phase pull request's squash title is feat: a test frame in RetroArch from the library, runner and libretro core"

patterns-established:
  - "Evidence for an owner's identity-bound step is the owner's printed lines plus the agent's read-only check"

requirements-completed: [FRAME-01, FRAME-08]

coverage:
  - id: D1
    description: "Every timeout-minutes is twice the slowest measured successful run with its run ID and sample count, or provisional with its sample count"
    requirement: FRAME-01
    verification:
      - kind: other
        ref: "cmake --workflow --preset hygiene, exit 0 (hygiene.timeout_comments)"
        status: pass
    human_judgment: false
  - id: D2
    description: "be32 and wasm32 MUHTARI_TIMEOUT_SCALE set from measured emulator slowdowns, documented in docs/testing.md"
    requirement: FRAME-01
    verification:
      - kind: integration
        ref: "CI run https://github.com/szTheory/muhtari/actions/runs/38957354177 on 65fdfda, gh run watch --exit-status exit 0"
        status: pass
    human_judgment: false
  - id: D3
    description: "CI, the release rehearsal and Nightly are green on the probe PR's head commit, and CI required passes"
    requirement: FRAME-01
    verification:
      - kind: e2e
        ref: "CI https://github.com/szTheory/muhtari/actions/runs/38957354177, exit 0"
        status: pass
      - kind: e2e
        ref: "Release rehearsal https://github.com/szTheory/muhtari/actions/runs/38957354117, exit 0"
        status: pass
      - kind: e2e
        ref: "Nightly https://github.com/szTheory/muhtari/actions/runs/38957353962, exit 0"
        status: pass
      - kind: integration
        ref: "cmake --workflow --preset ci, exit 0 (63 tests plus 4 release tests)"
        status: pass
    human_judgment: false
  - id: D4
    description: "workflow.test_gate_timeout equals twice the measured local ci workflow time"
    requirement: FRAME-01
    verification:
      - kind: other
        ref: "six timed cmake --workflow --preset ci runs from an empty build directory, 1.24 to 1.36 s, exit 0 each; jq .workflow.test_gate_timeout .planning/config.json prints 3"
        status: pass
    human_judgment: false
  - id: D5
    description: "The repository settings and the committed main ruleset are applied, idempotently, and the ruleset requires CI required"
    requirement: FRAME-08
    verification:
      - kind: integration
        ref: "scripts/apply-repo-settings.sh --check run twice after the owner's apply, each printed no drift on szTheory/muhtari, exit 0"
        status: pass
      - kind: integration
        ref: "gh api repos/szTheory/muhtari/rules/branches/main lists required_status_checks with context CI required, integration 15368"
        status: pass
    human_judgment: false
  - id: D6
    description: "The probe PR is closed with the phase branch kept, and no open pull request exists for the branch"
    requirement: FRAME-08
    verification:
      - kind: other
        ref: "gh pr view 6 --json state prints CLOSED; gh pr list --head phase/01-a-test-frame-in-retroarch --state open prints nothing; git ls-remote --heads origin shows the branch at 65fdfda"
        status: pass
    human_judgment: false
  - id: D7
    description: "The phase pull request title is a release-cutting feat: Conventional Commit, recorded in STATE.md"
    requirement: FRAME-08
    verification:
      - kind: other
        ref: "echo | scripts/check-title.sh 'feat: a test frame in RetroArch from the library, runner and libretro core', exit 0; grep -c 'feat: ' .planning/STATE.md prints 2"
        status: pass
    human_judgment: false
  - id: D8
    description: "After the phase merges, the release PR is green and 0.1.0 publishes 19 assets, and the latest-release install line works"
    requirement: FRAME-01
    verification:
      - kind: e2e
        ref: "release.yml run on main after the phase PR and the release PR merge"
        status: unknown
    human_judgment: false
  - id: D9
    description: "The instructions-per-frame gate is armed by two zero-spread control runs and an arming ci: pull request"
    requirement: FRAME-08
    verification:
      - kind: e2e
        ref: "two workflow_dispatch perf control runs on main, then the arming ci: pull request"
        status: unknown
    human_judgment: false

duration: 33min
completed: 2026-10-16
status: complete
---

# Phase 01 Plan 21: Measured limits, applied ruleset and ship hand-off Summary

**Every job timeout is measured or honestly provisional, the emulator scales are 11 (be32) and 3 (wasm32), the owner's one command made the `main` ruleset requiring `CI required` live with no drift on two checks, and the probe PR is closed so `/gsd-ship 1` can open the `feat:` phase PR**

## Performance

- **Duration:** 33 min (this includes the wait for the owner's apply step)
- **Started:** 2026-10-16T19:41:02Z
- **Completed:** 2026-10-16T20:13:50Z
- **Tasks:** 3
- **Files modified:** 8

## Accomplishments
- Timeouts: every successful job on the phase branch was timed (22 CI, 11 Release and 4 Nightly runs). A job with 10 or more samples got twice its slowest run, rounded up to minutes, with a 5-minute floor for network-bound and roll-up jobs. ci.yml: changes 5 (run 38945207181, 33 samples), hygiene 1 (2 x 27 s, run 38943539777, 32), title 5 (run 38955531324, 29), build 3 (2 x 71 s, run 38939482093, 32), asan 2 (2 x 55 s, run 38937116459, 32), nofp 1 (2 x 29 s, run 38945207181, 32), be32 5 (run 38945207181), hash-equality 5 (run 38946364083), wasm32 5 (run 38938418438), retroarch 5 (run 38951834012, 26), CI required 5 (run 38953745345). release.yml: rehearsal 14 (2 x 399 s, run 38951834012, 11), install-line 5 and cleanup 5 (run 38943087443). These stay provisional, with sample counts in the comment: perf (5), release-please, publish and install-line-latest (0, they run on main only), Nightly bench and bench-wasm32 (4 each), Nightly report (0), dependabot-automerge (0).
- Emulator scales, from ctest per-test times in 10 CI runs on the tests shared with Linux x64: be32 is 10.72 times slower (scale 10 to 11) and wasm32 is 2.53 times slower (scale 5 to 3). "provisional" is gone from both preset comments, and docs/testing.md explains the scales and the timeout rule.
- Test-gate timeout: six local `cmake --workflow --preset ci` runs from an empty build directory took 1.24 to 1.36 s. `workflow.test_gate_timeout` went from 900 to 3, twice the slowest rounded up. Three seconds leaves little room for a slow moment on the Mac: a busy machine could time the gate out with nothing wrong. Re-measure it when the suite grows. `mode` is still interactive; `auto_advance` and `_auto_chain_active` are still false.
- The owner ran `scripts/apply-repo-settings.sh`. It printed `ruleset 'main' applied` and `repository settings applied to szTheory/muhtari`, with nothing on stderr. Then `--check` ran twice: each printed `no drift on szTheory/muhtari` and exited 0, so applying is idempotent. `gh api repos/szTheory/muhtari/rules/branches/main` lists deletion, non_fast_forward, required_linear_history, pull_request and required_status_checks, which requires `CI required` from integration 15368.
- Draft probe PR #6 is closed with a comment that links the green runs. The branch `phase/01-a-test-frame-in-retroarch` is kept at 65fdfda, and no open pull request exists for it.

## Ship requirement

The phase pull request that `/gsd-ship 1` opens must carry a `feat: …` squash title, or no release is cut. release-please opens a release PR only for releasable types, so a `ci:` or `docs:` title would publish nothing. The recorded title is `feat: a test frame in RetroArch from the library, runner and libretro core`. `scripts/check-title.sh` accepts it (exit 0), and STATE.md records it under "Ship step".

## Task Commits

1. **Task 1: measured timeouts and scales, test-gate timeout, three workflows green** - `65fdfda` (ci)
2. **Task 2: owner applies the repository settings and the committed ruleset** - no commit (owner action, checkpoint:human-action)
3. **Task 3: verify no drift, close the probe PR, record evidence** - `59eaf9b` (docs)

## Evidence

- Head 65fdfda, `gh run watch --exit-status` exit 0 for each: CI https://github.com/szTheory/muhtari/actions/runs/38957354177, Release (rehearsal) https://github.com/szTheory/muhtari/actions/runs/38957354117, Nightly https://github.com/szTheory/muhtari/actions/runs/38957353962.
- Local `cmake --workflow --preset hygiene` exit 0, and `cmake --workflow --preset ci` exit 0 (63 tests plus 4 release tests).
- Before the owner applied anything, `scripts/apply-repo-settings.sh --check` exited 1 with one line, `drift: ruleset main is missing, want .github/rulesets/main.json`. After the apply it printed `no drift on szTheory/muhtari` and exited 0, twice in a row. The plan's Task 3 automated check (both checks, no open PR, the `feat: ` line in STATE.md) exited 0.
- `gh release list` printed nothing. `git ls-remote --tags origin 'rehearsal-*' 'v*'` printed nothing. There are no releases, no rehearsal tags and no `v*` tags.
- `gh pr close 6` exit 0. `gh pr view 6 --json state` prints `CLOSED`.

Gate-code tally: `tests/cmake/*.cmake` has 1,217 lines and `scripts/*.sh` has 590 (1,807 together), against 315 lines of C in `src/`. That is 5.7 times `src/`. AGENTS rule 7 allows a tenth of `src/` from phase 3, and at this size the gate code would need `src/` to reach about 18,000 lines. Warning: unless phase 2 and 3 grow the core by that much, or the gate code is cut or exempted, the gate code will not fit the ceiling by phase 3 (research R-04). This plan added no gate code.

## Files Created/Modified
- `.github/workflows/ci.yml`, `release.yml`, `nightly.yml`, `dependabot-automerge.yml` - measured or provisional `timeout-minutes` with run IDs and sample counts
- `CMakePresets.json` - be32 scale 11, wasm32 scale 3
- `docs/testing.md` - the emulator scales and the timeout rule
- `.planning/config.json` - `workflow.test_gate_timeout` 3
- `.planning/STATE.md` - ship step: next commands, the `feat:` title, the applied ruleset and the closed probe PR

## Decisions Made
See key-decisions in the frontmatter.

## Deviations from Plan

**1. [No-op] README was not changed**
- **Found during:** Task 1
- **Issue:** The plan asked for README's status line to say that the first release shows the test card. It already says this (README line 5).
- **Fix:** None needed.

**2. [Classification] The owner's apply step has human_judgment false**
- **Issue:** The plan's output section marks the owner's settings command as the one human-judgement item. The orchestrator ruled that it is evidenced by the owner's printed lines and the agent's `--check`, so no judgement remains.
- **Fix:** D5 has `human_judgment: false`, with the two `--check` runs and the ruleset GET as its verification.

---

**Total deviations:** 2 (1 no-op, 1 classification)
**Impact on plan:** None on scope. The agent never ran the settings script's apply mode, merged nothing and set no bypass.

## Issues Encountered
None.

## User Setup Required
None. The owner's settings step is done.

## Next Phase Readiness
- All 21 plans have summaries. Next: `/gsd-execute-phase 1` finishes with phase verification, then `/gsd-ship 1` with the `feat:` title above.
- After the merge, these steps are still due: the release PR is green and 0.1.0 publishes 19 assets; the latest-release install line works; the two perf control runs and the arming `ci:` PR; the forced nightly failure; and the wasm32 promotion after 10 runs on main.
- Pending todos stay listed in STATE.md: wasm32 promotion, arming the Ir gate, the firmware scan, and limiting release rehearsals.
- Concern: the 3 s test-gate timeout is tight, and phase 2's CPU tests will need it re-measured.

---
*Phase: 01-a-test-frame-in-retroarch*
*Completed: 2026-10-16*

## Self-Check: PASSED

Commits 65fdfda and 59eaf9b are on the branch; the eight modified files exist; `--check` exits 0 with no drift; PR #6 is CLOSED.
