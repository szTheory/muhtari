---
phase: 01-a-test-frame-in-retroarch
plan: 20
subsystem: infra
tags: [github-settings, ruleset, dependabot, github-app, jq, settings-as-code]

requires:
  - phase: 01-a-test-frame-in-retroarch
    provides: "apply-repo-settings.sh with a partial --check (plan 15), the CI required roll-up (plans 14, 15), the release App token pattern and docs/maintaining.md (plan 18)"
provides:
  - ".github/rulesets/main.json: default-branch ruleset requiring CI required (integration 15368), squash only, linear history, no deletion or force push, no bypass"
  - "apply-repo-settings.sh --check covering every applied setting, the eight labels and the ruleset, reporting all drift in one pass"
  - ".github/dependabot.yml: weekly github-actions updates, minor and patch grouped, ci prefix"
  - ".github/workflows/dependabot-automerge.yml: decide step (merge, wait, skip) and App-token squash auto-merge"
  - "policy.dependabot and policy.dependabot.self_test"
  - "docs/maintaining.md sections: Repository settings, Release App and secrets, Dependabot"
affects: [ci, release, repository-settings, plan-21]

actuals:
  tokens: 4850
  tasks: 3
  commits: 3

plan_head_before: 3af2b32096399ef4ebb0400aef02c8669821cc29
plan_head_after: c2d148342b45372bacb2331dbf6b2af7d7e1ab53

tech-stack:
  added: [dependabot/fetch-metadata v3.1.0]
  patterns:
    - "A drift check accumulates: one want() call per setting prints a drift line and sets a flag, the exit comes once at the end"
    - "A live ruleset is compared with the committed file after keeping only the fields and rule parameters the file sets"

key-files:
  created:
    - .github/rulesets/main.json
    - .github/dependabot.yml
    - .github/workflows/dependabot-automerge.yml
    - tests/cmake/dependabot.cmake
  modified:
    - scripts/apply-repo-settings.sh
    - tests/hygiene/tests.cmake
    - tests/inventory/hygiene.txt
    - docs/maintaining.md

key-decisions:
  - "The ruleset comparison drops server-only fields and any rule parameter the committed file does not set, then compares target, enforcement, conditions, bypass_actors and rules sorted by type; the drift line names the parts that differ"
  - "The ruleset path is resolved from the script's directory, so --check and apply work from any working directory"
  - "policy.dependabot carries labels hygiene and policy like every other policy test, and gets a self test that makes majors merge"
  - "The App's client ID is written into dependabot-automerge.yml because Actions variables may not resolve in Dependabot-triggered runs"
  - "requirements-completed copies FRAME-08 from the plan; plan 21 also declares it and applies the ruleset, so the shared-ID gate keeps it open until then"

patterns-established:
  - "Workflow decision programs are single-quoted jq -e programs that a CMake -P test extracts and runs on synthetic inputs (ci_required, dependabot)"

requirements-completed: [FRAME-08]

coverage:
  - id: D1
    description: "Committed branch ruleset: default branch, deletion, non_fast_forward, required_linear_history, pull_request with 0 approvals and squash only, CI required from integration 15368, no bypass actors"
    requirement: FRAME-08
    verification:
      - kind: other
        ref: "grep for CI required, 15368, required_linear_history and allowed_merge_methods squash in .github/rulesets/main.json, exit 0"
        status: pass
      - kind: integration
        ref: "plan 21: owner applies the ruleset, then scripts/apply-repo-settings.sh --check prints no drift"
        status: unknown
    human_judgment: false
  - id: D2
    description: "apply-repo-settings.sh --check reads every applied setting, the eight labels and the ruleset, prints one drift line per difference and exits 1"
    requirement: FRAME-08
    verification:
      - kind: integration
        ref: "scripts/apply-repo-settings.sh --check against szTheory/muhtari, exit 1 with exactly one drift line (ruleset main is missing)"
        status: pass
      - kind: integration
        ref: "a scratch copy of the script with four wrong expectations, --check against szTheory/muhtari, exit 1 with all five drift lines in one run"
        status: pass
      - kind: other
        ref: "sh -n scripts/apply-repo-settings.sh, exit 0"
        status: pass
    human_judgment: false
  - id: D3
    description: "Dependabot auto-merge decision: patch and minor merge, majors and unknown types wait, any other author is skipped"
    requirement: FRAME-08
    verification:
      - kind: unit
        ref: "tests/cmake/dependabot.cmake#policy.dependabot"
        status: pass
      - kind: unit
        ref: "tests/hygiene/tests.cmake#policy.dependabot.self_test"
        status: pass
      - kind: other
        ref: "cmake --workflow --preset hygiene, exit 0, 17 tests"
        status: pass
    human_judgment: false
  - id: D4
    description: "Weekly grouped Dependabot updates of the pinned actions with the ci prefix, merged by the App token once CI required passes"
    requirement: FRAME-08
    verification:
      - kind: other
        ref: "grep for github-actions, weekly, prefix ci and update-types in .github/dependabot.yml, exit 0"
        status: pass
      - kind: e2e
        ref: "first Dependabot pull request after the phase merges: patch or minor update auto-merged by the release App"
        status: unknown
    human_judgment: false
  - id: D5
    description: "docs/maintaining.md explains applying and checking settings, the App's variable, secret and key rotation, and Dependabot's merge rule"
    requirement: FRAME-08
    verification:
      - kind: other
        ref: "cmake --workflow --preset ci, exit 0 (docs.budgets passed)"
        status: pass
      - kind: other
        ref: "grep for apply-repo-settings.sh --check, MUHTARI_APP_CLIENT_ID, MUHTARI_APP_PRIVATE_KEY and Dependabot in docs/maintaining.md, exit 0"
        status: pass
    human_judgment: false

duration: 17min
completed: 2026-10-16
status: complete
---

# Phase 01 Plan 20: Settings as code, ruleset and Dependabot Summary

**The default-branch ruleset requiring `CI required` is committed, `apply-repo-settings.sh --check` now reads every applied setting and reports all drift in one pass, and Dependabot's grouped Actions updates auto-merge through the release App under a jq rule that `policy.dependabot` tests**

## Performance

- **Duration:** 17 min
- **Started:** 2026-10-16T19:13:14Z
- **Completed:** 2026-10-16T19:30:11Z
- **Tasks:** 3
- **Files modified:** 8

## Accomplishments
- `.github/rulesets/main.json`, seed content unchanged: `~DEFAULT_BRANCH`, deletion, non_fast_forward, required_linear_history, pull requests with 0 approvals and squash only, `CI required` from integration 15368, no bypass actors.
- `--check` covers the merge settings, secret scanning and push protection, the default token and Actions PR approval, SHA pinning, immutable releases, private vulnerability reporting, Dependabot alerts and security updates, each label's colour and description, and the ruleset. It reports all drift and then exits.
- Dependabot config and `dependabot-automerge.yml`: `decide` yields merge, wait or skip. Only merge mints the App token (contents, pull requests, workflows) and runs `gh pr merge --auto --squash`.
- `policy.dependabot` (5 synthetic events) and its self test, plus three new sections in `docs/maintaining.md`.

## Task Commits

1. **Task 1: committed ruleset and full --check** - `39f9ed7` (feat)
2. **Task 2: Dependabot config, auto-merge workflow, policy.dependabot** - `5e3ab1e` (ci)
3. **Task 3: docs/maintaining.md sections** - `c2d1483` (docs)

## Evidence

- `scripts/apply-repo-settings.sh --check` (read-only, szTheory/muhtari): exit 1, one `drift:` line, `ruleset main is missing, want .github/rulesets/main.json`. This drift is expected until the owner applies the ruleset in plan 21. The same result came from `/tmp`.
- A scratch copy with four wrong expectations: exit 1 with five drift lines in one run (merge commit, wiki, SHA pinning, a label, the ruleset).
- The ruleset comparison was run offline on three synthetic live rulesets. Server fields and extra parameters gave no difference. A changed merge-method list plus a bypass actor gave `bypass_actors,rules`, and a missing rule gave `rules`.
- `cmake --workflow --preset hygiene`: exit 0, 17 tests. `ctest --test-dir build/hygiene -R '^policy\.dependabot$'`: exit 0.
- Mutation: with `else "merge" end`, `policy.dependabot` failed. The file was restored and the self test now encodes this mutation.
- `cmake --workflow --preset ci`: exit 0 (63 tests including `docs.budgets`, plus 4 release tests).
- Pushed head c2d1483 to draft PR #6. CI https://github.com/szTheory/muhtari/actions/runs/38953745345 success. Release https://github.com/szTheory/muhtari/actions/runs/38953745486 success, with rehearsal and cleanup both successful. Nightly https://github.com/szTheory/muhtari/actions/runs/38953745893 success. Dependabot auto-merge https://github.com/szTheory/muhtari/actions/runs/38953745772 skipped, because the author is not Dependabot.
- After the runs, `gh release list` showed no `rehearsal-*` release and `git ls-remote --tags origin 'rehearsal-*'` printed nothing.

Gate-code tally: `tests/cmake/*.cmake` 1,217 lines (this plan adds 51) and `scripts/*.sh` 590 lines (this plan adds 43), against 315 lines in `src/`. AGENTS rule 7's one-tenth ceiling applies from phase 3.

## Files Created/Modified
- `.github/rulesets/main.json` - the branch ruleset the owner applies in plan 21
- `scripts/apply-repo-settings.sh` - full read-only `--check`; ruleset name and path shared with apply
- `.github/dependabot.yml` - weekly github-actions updates, minor and patch grouped, `ci` prefix
- `.github/workflows/dependabot-automerge.yml` - decide step and App-token auto-merge
- `tests/cmake/dependabot.cmake` - runs the decide program on synthetic events
- `tests/hygiene/tests.cmake`, `tests/inventory/hygiene.txt` - register `policy.dependabot` and its self test
- `docs/maintaining.md` - Repository settings, Release App and secrets, Dependabot

## Decisions Made
See key-decisions in the frontmatter.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 3 - Blocking] Seed fetched from nesturbator 9f9386c661f6, not 3200aa868371**
- **Found during:** Task 1
- **Issue:** 3200aa868371 is not on GitHub; the orchestrator confirmed this.
- **Fix:** The ruleset and dependabot.yml seeds came from published main 9f9386c661f6, and the commit bodies cite that commit. The ruleset content matches ENGINEERING §6 exactly. nesturbator has no `dependabot-automerge.yml`, so that workflow was written from ENGINEERING §7.
- **Committed in:** 39f9ed7, 5e3ab1e

**2. [Rule 2 - Missing] policy.dependabot.self_test and the hygiene,policy labels**
- **Found during:** Task 2
- **Issue:** The plan named the label `hygiene` only. Every other policy test carries `hygiene policy` and has a negative control.
- **Fix:** Both labels were used, and a self test was added that makes majors merge and must fail on that case.
- **Committed in:** 5e3ab1e

**3. [Rule 1 - Bug] The ruleset path did not depend on the working directory**
- **Found during:** Task 1
- **Issue:** The path `.github/rulesets/main.json` was relative to the working directory, so running the script from elsewhere silently skipped the ruleset.
- **Fix:** The path is now resolved from the script's directory.
- **Committed in:** 39f9ed7

**4. [Docs accuracy] The Dependabot section does not mention the weekly digest**
- **Issue:** `digest.yml` does not exist yet, so the docs say a major update waits for a person and do not promise a digest entry.

---

**Total deviations:** 4 (1 blocking, 1 missing critical, 1 bug, 1 docs accuracy)
**Impact on plan:** No change in scope. The settings and ruleset were only read; nothing was applied.

## Issues Encountered
None.

## User Setup Required
None for this plan. Plan 21 is the owner's step that applies the ruleset (`scripts/apply-repo-settings.sh`).

## Next Phase Readiness
- Ready for plan 21. After the owner applies the settings, `--check` must print `no drift on szTheory/muhtari`.
- The auto-merge path is proven live only by the first real Dependabot pull request after the merge. It is unverified whether the `workflows` permission is enough to merge edits under `.github/workflows/` (ENGINEERING §7).

---
*Phase: 01-a-test-frame-in-retroarch*
*Completed: 2026-10-16*

## Self-Check: PASSED

All four created files exist; commits 39f9ed7, 5e3ab1e and c2d1483 are on the branch. FRAME-08 stays open: plan 21 also declares it (requirements.ready-ids: 0/1 ready).
