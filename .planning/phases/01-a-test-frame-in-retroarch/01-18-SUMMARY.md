---
phase: 01-a-test-frame-in-retroarch
plan: 18
subsystem: infra
tags: [github-actions, release, release-please, github-app, attestation, runbook]

requires:
  - phase: 01-a-test-frame-in-retroarch
    provides: "the rehearsal, publish.sh and the draft-by-ID fix (01-17); ci.yml with the ref input and the called roll-up (01-14, 01-15)"
provides:
  - "release.yml real path on push to main: release-please with the App token, auto-merge of the release PR, CI at release-please's sha, publish to the draft by ID as latest, the install line against releases/latest"
  - "publish.sh attested: attestation and tampered-copy checks before an irreversible publish, run by both paths"
  - "docs/maintaining.md release runbook"
  - "DEC.49 and ENGINEERING section 5 record the pull-request prerelease rehearsal"
  - "pending todos: release firmware scan; narrowing rehearsals to pushes that change release inputs"
affects: [release, ci, phase-close]

actuals:
  tokens: 7100
  tasks: 2
  commits: 2
plan_head_before: 83f6a274c0e87ad43fbf01bb9b7c09be347ad671
plan_head_after: 09cdd8b1ea91b9a49ba400ee6cb9a339b074f5ee

tech-stack:
  added: [actions/create-github-app-token v3.3.0, googleapis/release-please-action v5.0.0]
  patterns:
    - "Real-path jobs are gated on github.event_name == 'push' and release_created; rehearsal jobs on pull_request, so each event skips the other path"
    - "publish checks the draft state by release ID first; a published release is only verified on re-run"

key-files:
  created:
    - docs/maintaining.md
    - .planning/todos/pending/2026-10-16-add-the-release-firmware-scan-with-the-first-rom-code.md
    - .planning/todos/pending/2026-10-16-limit-release-rehearsals-to-pushes-that-change-release-inputs.md
  modified:
    - .github/workflows/release.yml
    - scripts/publish.sh
    - docs/testing.md
    - .planning/preparation/DECISIONS.md
    - .planning/preparation/ENGINEERING.md
    - .planning/STATE.md

key-decisions:
  - "The real path's job IDs are release-please, release-ci, publish and install-line-latest, because ci and install-line already name the rehearsal's jobs"
  - "Publishing is gh api PATCH releases/<id> with make_latest=true, not gh release edit <tag>, because gh finds a draft by listing releases"
  - "publish.sh attested runs the attestation and tampered-copy checks before publishing; verify runs them again after"
  - "The release PR number comes from release-please's pr output through jq in the step, so a skipped step never evaluates it"

patterns-established:
  - "Nothing irreversible before every check that can run on a draft"

requirements-completed: [FRAME-06, FRAME-07]

coverage:
  - id: D1
    description: "release.yml parses and starts on GitHub with the real path added; on the pull request the real-path jobs skip and the rehearsal, now with publish.sh attested before publishing, is green end to end and deletes its prerelease and tag"
    requirement: FRAME-06
    verification:
      - kind: e2e
        ref: "gh run watch 38945207181 --exit-status exit 0; https://github.com/szTheory/muhtari/actions/runs/38945207181"
        status: pass
      - kind: e2e
        ref: "gh run watch 38946364389 --exit-status exit 0 on 09cdd8b; https://github.com/szTheory/muhtari/actions/runs/38946364389"
        status: pass
      - kind: e2e
        ref: "rehearsal job log of run 38945207181 holds 36 lines 'a copy with its last byte flipped fails, as expected' (18 from attested before publishing, 18 in verify) and 'published with 19 assets; every download matches SHA256SUMS and verifies'"
        status: pass
    human_judgment: false
  - id: D2
    description: "The release CI call uses release-please's sha output as its ref, never github.sha, and both ci.yml caller jobs grant contents read and pull-requests read; the App token uses vars.MUHTARI_APP_CLIENT_ID"
    requirement: FRAME-06
    verification:
      - kind: other
        ref: "grep -c 'needs.release-please.outputs.sha' .github/workflows/release.yml prints 4; grep -c 'github.sha' prints 0; grep -n 'pull-requests: read' matches lines 48 and 177 under the two uses ./.github/workflows/ci.yml jobs"
        status: pass
    human_judgment: false
  - id: D3
    description: "On push to main, release-please opens the release PR, the pipeline enables auto-merge with the App token, and the merge creates the draft release"
    requirement: FRAME-06
    verification:
      - kind: e2e
        ref: "the first Release run on main after the phase pull request merges, and the run of the release pull request's merge"
        status: unknown
    human_judgment: false
  - id: D4
    description: "On release_created, CI runs at the release commit, publish fails unless all six build legs recorded that commit, uploads to the draft by ID, attests, runs attested, publishes as latest and verifies; a re-run on the published release only verifies and exits 0"
    requirement: FRAME-06
    verification:
      - kind: unit
        ref: "tests/hygiene/tests.cmake#policy.publish_set; cmake --workflow --preset hygiene exit 0 (15 tests)"
        status: pass
      - kind: e2e
        ref: "the post-merge Release run that publishes the first release: jobs release-ci and publish"
        status: unknown
    human_judgment: false
  - id: D5
    description: "The README install line installs the core and info file from releases/latest"
    requirement: FRAME-07
    verification:
      - kind: e2e
        ref: "install-line job 117125800178 of run 38945207181 against the rehearsal URL"
        status: pass
      - kind: e2e
        ref: "the post-merge Release run's install-line-latest job against releases/latest/download"
        status: unknown
    human_judgment: false
  - id: D6
    description: "release.yml's concurrency group has queue max and no cancel-in-progress"
    requirement: FRAME-06
    verification:
      - kind: other
        ref: "grep -n 'queue: max' .github/workflows/release.yml matches line 28; the only cancel-in-progress is in a comment; GitHub ran runs 38945207181 and 38946364389"
        status: pass
    human_judgment: false
  - id: D7
    description: "DEC.49 and ENGINEERING section 5 record the published prerelease rehearsal on the pull request and why workflow_dispatch and an untagged draft cannot rehearse; the firmware-scan deferral and the rehearsal-trigger todo are visible in STATE.md"
    requirement: FRAME-06
    verification:
      - kind: other
        ref: "grep -c rehearsal prints 1, 9 and 3 for DECISIONS.md, docs/maintaining.md and ENGINEERING.md; the DEC.49 row contains prerelease and workflow_dispatch; both todo files exist, the firmware one with area: release"
        status: pass
    human_judgment: false
  - id: D8
    description: "docs/maintaining.md covers how a release happens, the rehearsal and its fallback, re-runs, phase-close checks, withdrawal and pin bumps, within the docs budget and without the next version string"
    requirement: FRAME-06
    verification:
      - kind: unit
        ref: "cmake --workflow --preset ci exit 0 (56 tests and 4 release tests, including docs.budgets and release.single_version_source.next); CI run https://github.com/szTheory/muhtari/actions/runs/38946364083 success"
        status: pass
    human_judgment: false

duration: 20min
completed: 2026-10-16
status: complete
---

# Phase 1 Plan 18: The real release path Summary

**release.yml now releases from `main`. release-please runs with the App token and the pipeline turns on auto-merge for the release pull request. When a release is created, CI runs again at release-please's `sha`. `publish` then runs the same `publish.sh` steps the rehearsal proved on the draft, found by ID, and publishes it as latest. The README install line then runs against `releases/latest`. The runbook is in `docs/maintaining.md`.**

## Performance

- **Duration:** about 20 min
- **Started:** 2026-10-16T18:06:54Z
- **Completed:** 2026-10-16T18:27:09Z
- **Tasks:** 2
- **Files modified:** 9

## Accomplishments

- Real path in `release.yml`: `release-please` → `release-ci` (`uses: ./.github/workflows/ci.yml`, `ref: ${{ needs.release-please.outputs.sha }}`, `contents: read` and `pull-requests: read`) → `publish` → `install-line-latest`. It differs from the rehearsal only in its trigger, its token and the `latest` alias.
- Re-run safety: `publish` first reads the release's `draft` field by ID. On a published release it skips upload, attest and publish, and runs only `prepare` and `verify`.
- `publish.sh attested` (new subcommand) runs every attestation and tampered-copy check before the publish that cannot be undone. Both paths run it, so the rehearsal exercises the same sequence.
- Evidence on the probe pull request:
  - `gh run watch 38945207181 --exit-status` exit 0, https://github.com/szTheory/muhtari/actions/runs/38945207181. The real-path jobs skipped; `rehearsal`, `install-line` and `cleanup` succeeded.
  - On head 09cdd8b, CI https://github.com/szTheory/muhtari/actions/runs/38946364083 and Release https://github.com/szTheory/muhtari/actions/runs/38946364389 both succeeded.
- Records: the DEC.49 row and ENGINEERING §5's Rehearsal bullet describe the pull-request prerelease rehearsal, the reason (`workflow_dispatch` fires only for workflow files on the default branch, and an untagged draft cannot be published or verified), the fallback and run 38942043999.
- `docs/maintaining.md` (5,863 bytes) describes the release, the rehearsal, re-runs, phase close, withdrawal and pinned downloads. It does not write the first release's version.
- Locally `cmake --workflow --preset ci` exits 0 (56 + 4 tests) and `cmake --workflow --preset hygiene` exits 0 (15 tests).

**Proved after the merge, not here** (GSD-HANDOFF §3 steps 7–8): the App-token `release-please` job, the release PR's auto-merge, `release-ci` at the release `sha`, the real `publish` and `install-line-latest` against `releases/latest`. Coverage D3, D4 and D5 hold these as `status: unknown` entries naming the post-merge Release run.

## Task Commits

1. **Task 1: End-to-end real path:** `c4cc384` (ci)
2. **Task 2: Records, todos, runbook:** `09cdd8b` (docs)

**Plan metadata:** see the docs(01-18) completion commit.

## Files Created/Modified

- `.github/workflows/release.yml`: the four real-path jobs; the rehearsal gains a `publish.sh attested` step; the header comment covers both paths.
- `scripts/publish.sh`: `attested DIR` split out of `verify`, which now calls it first.
- `docs/testing.md`: the rehearsal row names `attested`, and a pointer to `docs/maintaining.md`.
- `docs/maintaining.md`: new runbook.
- `.planning/preparation/DECISIONS.md`, `.planning/preparation/ENGINEERING.md`: DEC.49 and §5 Rehearsal amended, dated 2026-10-16.
- `.planning/todos/pending/2026-10-16-add-the-release-firmware-scan-with-the-first-rom-code.md` (area release).
- `.planning/todos/pending/2026-10-16-limit-release-rehearsals-to-pushes-that-change-release-inputs.md` (area ci).
- `.planning/STATE.md`: both todos under Pending Todos.

## Decisions Made

See `key-decisions`. In short:

- The job IDs are `release-ci` and `install-line-latest` because job IDs must be unique.
- The release is published by an ID-addressed PATCH with `make_latest=true`.
- Attestations are checked before publishing.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 3 - Blocking] Real-path job IDs**
- **Found during:** Task 1
- **Issue:** The plan names the real-path jobs `ci` and `install-line`, but the rehearsal already uses those IDs, and job IDs must be unique in a workflow.
- **Fix:** `release-ci` and `install-line-latest`. The plan's `release-please` and `publish` are unchanged.
- **Committed in:** `c4cc384`

**2. [Rule 1 - Bug] Publish by release ID, not `gh release edit <tag>`**
- **Found during:** Task 1
- **Issue:** The plan writes `gh release edit <tag> --draft=false --latest`. Plan 17 found (run 38941477560) that gh finds a draft by listing releases, and the list can miss a fresh draft.
- **Fix:** `gh api -X PATCH releases/$ID -F draft=false -f make_latest=true`, the same form as the rehearsal.
- **Committed in:** `c4cc384`

**3. [Rule 2 - Missing critical] Attestation checks before publishing**
- **Found during:** Task 1
- **Issue:** The plan orders the steps "attest, verify, publish, verify again". `publish.sh verify` fails on a draft, so it cannot run before publishing. Publishing without the attestation checks would make an unverified release immutable.
- **Fix:** `publish.sh attested DIR`, taken out of `verify`. Both paths run it before publishing, and `verify` runs it again. `docs/testing.md` was updated with it.
- **Committed in:** `c4cc384`

**4. [Orchestrator decision] Second pending todo**
- Added `2026-10-16-limit-release-rehearsals-to-pushes-that-change-release-inputs.md` (area ci) and listed it in STATE.md, as the orchestrator directed. Push 09cdd8b shows the cost the todo describes: it changes only docs, and it rehearsed again (run 38946364389).
- **Committed in:** `09cdd8b`

---

**Total deviations:** 3 auto-fixed (1 blocking, 1 bug, 1 missing critical) and 1 orchestrator-directed addition.
**Impact on plan:** The real path is now the rehearsal's sequence exactly. Nothing irreversible happens before every check that can run on a draft.

## Gate-code tally

- **This plan:**
  - `scripts/publish.sh`: 164 lines (+7)
  - `release.yml`: 240 lines (+114)
- **All gate scripts:**
  - `tests/cmake/*.cmake` plus `cmake/MuhtariTests.cmake`: 1,109 lines
  - `scripts/*.sh`: 547 lines
- **Code under test:** `src/` is 315 lines. The tenth-of-`src/` ceiling applies from phase 3.

## Issues Encountered

- `actionlint` reports only findings that were already there: `queue` is unknown to it, the `ubuntu-26.04` label is unknown, and SC2086 on the cleanup line. It reports nothing for the new jobs.
- The action pins were checked against their `action.yml` at the pinned SHAs:
  - create-github-app-token accepts `client-id` and the `permission-*` inputs;
  - release-please-action defaults to `release-please-config.json` and `.release-please-manifest.json` and sets `prs_created`, `pr`, `release_created`, `tag_name`, `sha` and `id` for path `.`.

## Rehearsal safety

After the last run:

- `gh release list` prints nothing;
- `git ls-remote --tags origin 'rehearsal-*'` prints nothing;
- `gh api repos/szTheory/muhtari/releases --jq length` prints 0.

No `v*` tag or release was created.

## User Setup Required

None. The App variable and secret were already set (ENGINEERING §13). The workflow only references them, and the agent never read them.

## Next Phase Readiness

- Plans 19 to 21 remain in phase 1.
- After the phase pull request merges, the first Release run on `main` exercises the App-token path. The phase-close checks in `docs/maintaining.md` then confirm that no `autorelease: pending` PR is open and that `releases/latest` has 19 assets.

---
*Phase: 01-a-test-frame-in-retroarch*
*Completed: 2026-10-16*

## Self-Check: PASSED
