---
phase: 01-a-test-frame-in-retroarch
plan: 17
subsystem: infra
tags: [github-actions, release, rehearsal, attestation, release-please, sha256sums]

requires:
  - phase: 01-a-test-frame-in-retroarch
    provides: "ci.yml with a workflow_call ref input and the called roll-up (01-14, 01-15), the archives and install_line.cmake (01-09), the retroarch job (01-16)"
provides:
  - "release.yml rehearsal on same-repository pull requests: CI at the head, then a published, attested, verified and deleted prerelease"
  - "scripts/publish.sh prepare | upload | verify | rehearsal-create | rehearsal-delete, shared with the real release"
  - "release-please-config.json and .release-please-manifest.json as ENGINEERING section 5 says"
  - "commit.txt in each build leg's hashes artifact"
  - "policy.publish_set and its self test"
affects: [01-18, release, ci]

actuals:
  tokens: 4900
  tasks: 2
  commits: 3
plan_head_before: 5a6307bb80eec51e2518be35b51262b9f088f3b5
plan_head_after: 44d6670e0097d2156dde2e9e5234ab20af1917c4

tech-stack:
  added: [actions/attest v4.2.2]
  patterns:
    - "Draft releases are addressed by ID from the create response, never by tag"
    - "Rehearsal tag rehearsal-<run_id>-<run_attempt>; a separate always() cleanup job deletes release then tag and checks both are gone"

key-files:
  created:
    - .github/workflows/release.yml
    - scripts/publish.sh
    - release-please-config.json
    - .release-please-manifest.json
  modified:
    - .github/workflows/ci.yml
    - tests/hygiene/tests.cmake
    - tests/inventory/hygiene.txt
    - docs/testing.md

key-decisions:
  - "The rehearsal is a published prerelease, not a draft: A4 held, so the draft-only fallback was not used"
  - "publish.sh finds the draft by the ID the create call returns; the release list did not show a just-created draft (run 38941477560)"
  - "Deletion is a fourth job, cleanup, so it can run after install-line and whatever failed before it"
  - "queue: max is accepted by GitHub in release.yml's concurrency block"

patterns-established:
  - "publish.sh deletes only rehearsal-* tags; every subcommand is safe to re-run"

requirements-completed: [FRAME-06, FRAME-07]

coverage:
  - id: D1
    description: "On the probe pull request, release.yml ran CI on the head commit, then published prerelease rehearsal-38942043999-1 with make_latest false, uploaded 18 zips and SHA256SUMS, attested them, verified each and a tampered copy of each, ran gh release verify, downloaded every zip from the public URL, checked SHA256SUMS and the signer workflow, and deleted the release and tag"
    requirement: FRAME-06
    verification:
      - kind: e2e
        ref: "gh run watch 38942043999 --exit-status exit 0; https://github.com/szTheory/muhtari/actions/runs/38942043999"
        status: pass
      - kind: e2e
        ref: "rehearsal job log of run 38942043999 holds 18 lines 'verifies; a copy with its last byte flipped fails, as expected', 'Release rehearsal-38942043999-1 verified!' and 'published with 19 assets; every download matches SHA256SUMS and verifies'"
        status: pass
    human_judgment: false
  - id: D2
    description: "The README install line installs the core and info file from the published rehearsal on macos-15"
    requirement: FRAME-07
    verification:
      - kind: e2e
        ref: "install-line job 117114301496 of run 38942043999 logs install_line installed cores/muhtari_libretro.dylib and info/muhtari_libretro.info"
        status: pass
      - kind: e2e
        ref: "the same line against releases/latest/download after 0.1.0 is published, plan 01-18"
        status: unknown
    human_judgment: false
  - id: D3
    description: "No rehearsal release or tag and no version release remain"
    requirement: FRAME-06
    verification:
      - kind: other
        ref: "gh release list prints nothing, git ls-remote --tags origin 'rehearsal-*' prints nothing, and gh api repos/szTheory/muhtari/releases counts 0 after run 38942043999"
        status: pass
    human_judgment: false
  - id: D4
    description: "release.yml's ci caller job grants contents read and pull-requests read, so the nested CI including title starts on pull_request"
    requirement: FRAME-06
    verification:
      - kind: e2e
        ref: "ci / title and ci / CI required succeeded in run 38942043999"
        status: pass
      - kind: e2e
        ref: "the nested call on push to main, plan 01-18"
        status: unknown
    human_judgment: false
  - id: D5
    description: "publish.sh prepare accepts exactly the 18 zip names, all six commit.txt equal to EXPECTED_SHA, and writes SHA256SUMS in C-locale name order; 17 files, 19 files or a renamed file exit 1"
    requirement: FRAME-06
    verification:
      - kind: unit
        ref: "tests/hygiene/tests.cmake#policy.publish_set"
        status: pass
      - kind: unit
        ref: "tests/hygiene/tests.cmake#policy.publish_set.self_test"
        status: pass
      - kind: integration
        ref: "cmake --workflow --preset hygiene exit 0 (15 tests); CI run https://github.com/szTheory/muhtari/actions/runs/38943087451 success"
        status: pass
    human_judgment: false
  - id: D6
    description: "release-please-config.json is ENGINEERING section 5's configuration and the version stays single-sourced"
    requirement: FRAME-06
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^release\\.single_version_source' passes 3 of 3; cmake --workflow --preset ci exit 0 (56 tests, 4 release tests)"
        status: pass
    human_judgment: false
  - id: D7
    description: "release.yml concurrency has queue max and no cancel-in-progress"
    requirement: FRAME-06
    verification:
      - kind: other
        ref: "grep -n 'queue: max' .github/workflows/release.yml matches and grep -c cancel-in-progress prints 0; GitHub accepted the file and ran run 38942043999"
        status: pass
    human_judgment: false

duration: 20min
completed: 2026-10-16
status: complete
---

# Phase 1 Plan 17: Release rehearsal Summary

**On the probe pull request, release.yml now runs CI on the head commit and then every publish step against a real prerelease, `rehearsal-<run>-<attempt>`. The prerelease is attested, verified, downloaded, install-line checked and then deleted. All of it goes through `scripts/publish.sh`, which accepts exactly 18 zips built from one commit.**

## Performance

- **Duration:** about 20 min
- **Started:** 2026-10-16T17:35:29Z
- **Completed:** 2026-10-16T17:55:10Z
- **Tasks:** 2
- **Files modified:** 8

## Accomplishments

- Green rehearsal: `gh run watch 38942043999 --exit-status` exit 0, https://github.com/szTheory/muhtari/actions/runs/38942043999. The jobs were nested `ci` (all 17 jobs green), `rehearsal` (3m59s), `install-line` (8s) and `cleanup` (9s).
- Tampered-copy evidence is in the `rehearsal` job log: 18 lines `publish: <zip> verifies; a copy with its last byte flipped fails, as expected`, then `✓ Release rehearsal-38942043999-1 verified!` and `publish: rehearsal-38942043999-1 published with 19 assets; every download matches SHA256SUMS and verifies`.
- **A4 held.** A same-repository `pull_request` run got `contents: write`, `id-token: write` and `attestations: write` with `GITHUB_TOKEN`. The draft-only fallback was not used.
- `queue: max` is valid: GitHub accepted the workflow and queued and ran it.
- Nothing was left behind:
  - `gh release list` prints nothing.
  - `git ls-remote --tags origin 'rehearsal-*'` prints nothing.
  - The releases API counts 0, so there is no `v*` release either.
- The asset-set rule is tested: `cmake --workflow --preset hygiene` exits 0 with 15 tests. Hosted CI run 38943087451 succeeded on 44d6670.
- Closed the pending 01-14 proof (D9). The nested `ci / changes` of run 38942043999 started at the same second as CI run 38942043783's `changes` on the same commit, and both runs succeeded.

## Task Commits

1. **Task 1: End-to-end rehearsal on the probe PR**
   - `413c4cb` (ci): the first version.
   - `e6b87b0` (ci): fix. Address the draft by ID.
2. **Task 2: The publish-set tests:** `44d6670` (test)

**Plan metadata:** see the docs(01-17) commit.

## Files Created/Modified

- `.github/workflows/release.yml`: the four rehearsal jobs, `ci`, `rehearsal`, `install-line` and `cleanup`. Triggers are `push` to `main` (jobs come in plan 18) and `pull_request` on the release inputs.
- `scripts/publish.sh` (157 lines, POSIX sh): `prepare`, `upload`, `verify`, `rehearsal-create` and `rehearsal-delete`.
- `release-please-config.json`, `.release-please-manifest.json`: ENGINEERING §5 exactly, with manifest `0.0.0`.
- `.github/workflows/ci.yml`: each build leg writes `commit.txt` into its `hashes-<os>-<arch>` artifact.
- `tests/hygiene/tests.cmake`, `tests/inventory/hygiene.txt`: `policy.publish_set` and `policy.publish_set.self_test`.
- `docs/testing.md`: a "Release rehearsal" section and a self-test row.

## Decisions Made

- Every rehearsal job reads the tag from one workflow-level `REHEARSAL_TAG: rehearsal-${{ github.run_id }}-${{ github.run_attempt }}`.
- `verify` picks the download base from the tag:
  - `rehearsal-*` downloads from `releases/download/<tag>/`;
  - any other tag downloads from `releases/latest/download/`.
- `prepare` requires exactly six `commit.txt` files. A check over zero files would pass trivially.
- `rehearsal-delete` refuses any tag that does not start with `rehearsal-`. It deletes the release first, by ID when one is given, and only then the tag.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Bug] Drafts are addressed by ID, not by tag**
- **Found during:** Task 1, first hosted run https://github.com/szTheory/muhtari/actions/runs/38941477560.
- **Issue:** `gh release create --draft` succeeded, but the release list did not show the new draft right away. The step failed with `publish: no release rehearsal-38941477560-1 after creating it`. Everything after creation would have had the same problem, because `gh release upload` and `gh release edit` also find a draft by listing releases.
- **Fix:**
  - `rehearsal-create` now creates through `gh api POST repos/.../releases` with `draft`, `prerelease` and `make_latest=false`, and prints the ID from the response.
  - `upload` posts each asset to the release's upload URL by ID. It first deletes any asset with the same name, which gives the same result as `--clobber`.
  - The publish step is `gh api PATCH releases/<id>` with `draft=false prerelease=true make_latest=false`. This is the API form of `gh release edit --latest=false`. That flag now appears in release.yml only in the step's comment.
  - `cleanup` receives the ID as a job output and checks by ID that the release is gone.
- **Files modified:** `scripts/publish.sh`, `.github/workflows/release.yml`
- **Verification:** run 38942043999 green; `gh api repos/szTheory/muhtari/releases` counts 0 afterwards.
- **Committed in:** `e6b87b0`
- **Leftover check:** run 38941477560 left nothing behind. Its `cleanup` job ran, and the releases API showed no releases afterwards.

**2. [Rule 3 - Blocking] Deletion is a separate `cleanup` job**
- **Found during:** Task 1
- **Issue:** The plan puts the delete as an `if: always()` step at the end of `rehearsal`, and also requires the delete to come after `install-line`. A step inside `rehearsal` cannot run after another job.
- **Fix:** A fourth job, `cleanup`, needs `[rehearsal, install-line]` and runs with `if: always() && needs.rehearsal.result != 'skipped'`.
- **Committed in:** `413c4cb`

**3. [Rule 3 - Blocking] The watch command uses the workflow name**
- **Issue:** `gh run list --workflow release.yml` returns HTTP 404 until release.yml exists on the default branch.
- **Fix:** Watched with `--workflow Release`, which works.
- **Files modified:** none

**4. Seed commit**
- The plan names nesturbator `3200aa868371`, which is not on GitHub. The seed was fetched from published main `9f9386c661f6`, and the commit body of `413c4cb` cites that commit.

---

**Total deviations:** 3 auto-fixed (1 bug, 2 blocking) and 1 seed-source note.
**Impact on plan:** All of them were needed for the rehearsal to work. Nothing was added beyond the plan.

## Gate-code tally

- **New this plan:**
  - `scripts/publish.sh`: 157 lines
  - `tests/hygiene/tests.cmake`: +49 lines, now 209
  - `release.yml`: 126 lines
  - `ci.yml`: +7 lines, -1
- **All gate scripts:**
  - `tests/cmake/*.cmake` plus `cmake/MuhtariTests.cmake`: 1,109 lines
  - `scripts/*.sh`: 540 lines
- **Code under test:** `src/` is 315 lines. The tenth-of-`src/` ceiling applies from phase 3.

## Seeds

- Fetched with `gh api` into a scratch directory outside the repository, from nesturbator `9f9386c661f6`:
  - `.github/workflows/release.yml`, 176 lines
  - `release-please-config.json`
- **Taken:**
  - the exact-18 check;
  - the last-byte flip;
  - the re-read loop for 19 non-empty assets;
  - the `ci` caller's permissions.
- **Not taken, because ENGINEERING §5 wins:**
  - versioned names;
  - `extra-files`;
  - `bump-patch-for-minor-pre-major: true`;
  - `permissions: {}` at workflow level.

## Issues Encountered

None beyond the deviations above.

## Rehearsal safety

- Every rehearsal is a prerelease with `make_latest=false` and a unique tag. It is deleted with its tag in the `cleanup` job.
- After both runs:
  - `gh release list` prints nothing;
  - `git ls-remote --tags origin 'rehearsal-*'` prints nothing.

## User Setup Required

None.

## Next Phase Readiness

- Plan 18 adds the push-to-main path to release.yml: App token, release-please, auto-merge, and on `release_created` the nested CI at release-please's `sha` and the publish job. That job reuses `publish.sh prepare | upload | verify` with the draft's `id` output.
- The App-token path cannot be rehearsed on a pull request. It is first exercised after the merge.

---
*Phase: 01-a-test-frame-in-retroarch*
*Completed: 2026-10-16*

## Self-Check: PASSED
