# Maintaining muhtari

How a release is made, rehearsed, recovered and withdrawn, how the pinned downloads move, and how the repository's
settings, the release App and Dependabot are kept. Tests and CI are in `docs/testing.md`.

## How a release happens

Nobody tags or uploads by hand. `.github/workflows/release.yml` does every step:

1. A pull request is squash-merged into `main`. Its title becomes the commit subject, and release-please reads
   that Conventional Commit to decide the next version: before 1.0, `feat` and a breaking change bump the minor
   version, `fix` and `perf` bump the patch, and `docs`, `chore`, `ci`, `test`, `build`, `refactor` and `style` cut
   no release. Write `ci:`, not `fix(ci):`, for pipeline work.
2. The push to `main` runs the `release-please` job. It mints a token for the project's GitHub App, then opens or
   updates the release pull request, labelled `autorelease: pending`, which edits only `version.txt`,
   `.release-please-manifest.json` and `CHANGELOG.md`. When it creates that pull request, the same job turns on
   auto-merge for it with the App's token.
3. The release pull request runs CI like any other. GitHub merges it, as the App, once `CI required` passes.
4. That merge's push runs `release.yml` again. release-please now tags the merge commit and creates a draft release
   for it. The first release is the first minor version, whatever the commit types.
5. `release-ci` runs the whole of CI again on the release's commit, the `sha` release-please reports, never the
   commit that started the run.
6. `publish` runs `scripts/publish.sh` on the draft, found by its release ID:
   - `prepare`: exactly the 18 zips, every build leg's recorded commit equal to the release commit, no firmware or
     known ROM in any zip (`tests/cmake/firmware_scan.cmake`), `SHA256SUMS`;
   - `upload`, then `actions/attest` over `SHA256SUMS`;
   - `attested`: each zip's attestation verifies from `refs/heads/main` and a copy with its last byte flipped does
     not;
   - publish the draft as the latest release;
   - `verify`: `attested` again, `gh release verify`, 19 non-empty assets, and every zip downloaded from the
     release's own `releases/download/<tag>/` URL matches `SHA256SUMS` and verifies as signed by `release.yml` from
     `refs/heads/main`.
7. `install-line-latest` runs the README's install line verbatim on `macos-15` against the latest release.

The single-version check rejects duplicate product versions in source and user docs,
including the next minor and patch. Root `PROVENANCE.md` is outside that scope:
its upstream release citations are independent of the version in `version.txt`.

Both Ubuntu publisher jobs fetch the SHA-256-verified GitHub CLI pin before use. Every
attestation entrypoint rejects an unsupported or malformed version; stable gh 2.102.0
or later is required. Genuine and downloaded assets require the full repository/signer
workflow path and intended source ref before publication. Archive commit identity remains
a separate check: a later workflow trigger can publish an earlier release commit.

Only an attestation whose source ref is `refs/heads/main` is release provenance. A rehearsal signs with the same
`release.yml`, but from `refs/pull/<number>/merge`, so a check that a download is a release must pin the ref:

```sh
gh attestation verify muhtari-runner-linux-x64.zip --repo szTheory/muhtari \
  --signer-workflow szTheory/muhtari/.github/workflows/release.yml --source-ref refs/heads/main
```

The release workflow never cancels a run: runs on `main` queue behind each other.

A version moves to 1.0.0 only through a final `Release-As: 1.0.0` line in the body of the pull request that should
cut it.

## The rehearsal

A pull request from this repository that changes `release.yml`, `be-equality.yml`,
`scripts/publish.sh`, `release-please-config.json` or `cmake/packaging.cmake` runs the same publish steps before it can merge. The
rehearsal runs CI on the pull request's head, then publishes a prerelease named `rehearsal-<run id>-<attempt>` that
is never marked latest, verifies it and runs the install line against it, and an always-running `cleanup` job retains the draft ID, confirms release absence
before deleting its tag, and confirms both resources are absent. Failed reads or
deletions fail cleanup; they never count as proof of absence. Only the trigger, the token and the `latest` alias differ from a real
release. `docs/testing.md` lists the rehearsal's jobs.

A pull request rehearses rather than a manual run because GitHub starts `workflow_dispatch` only for workflow files
on the default branch, and a draft without a tag cannot be published or verified.

There is no automatic draft-only fallback. Missing token permissions block the
rehearsal and leave publication/download acceptance unproved.

Every pushed commit on such a pull request rehearses again, and each rehearsal creates and deletes one public
prerelease.

## Re-running a failed release

- **A real release:** open the failed run and choose "Re-run failed jobs". `publish` is safe to re-run: uploads
  replace the draft's assets, and on a release already published it only runs `verify`, which exits 0 when the
  release has its 19 verified assets. Do not choose "Re-run all jobs": release-please then sees the release pull
  request already tagged and skips `publish`.
- **A failed rehearsal:** choose "Re-run all jobs", which takes a fresh `rehearsal-<run id>-<attempt>` tag, or push
  a new commit. A published tag that was deleted can never be reused under immutable releases. If `cleanup` itself
  failed, inspect its retained draft ID and rerun the guarded cleanup:
  `GITHUB_REPOSITORY=szTheory/muhtari sh scripts/publish.sh rehearsal-delete rehearsal-<run id>-<attempt> <release-id>`.

Cleanup requires a successful complete paginated release listing and fresh exact-ID
and exact-tag absence responses. Valid empty pages differ from missing, malformed or
failed evidence. A tag-only lookup cannot discover every draft. The five rehearsal
selectors are conservative triggers, not proof that every archive input is unchanged.

Rehearsals retain called CI, full native/PowerPC equality, publication, install and
cleanup. The monthly maintenance digest includes failed, retried and cancelled job
time. Compare execution cost separately from queue/dependency latency, and keep
runner-image, verifier-version and workload cohorts separate; small cohorts remain
provisional. No rehearsal-reuse mechanism is enabled.

## Closing a phase

Before a phase or milestone is called done:

- no pull request labelled `autorelease: pending` is open (`gh pr list --label 'autorelease: pending'` prints
  nothing), so no release is stuck behind a red check;
- the latest release is the expected version with 19 assets:
  `gh release view --json tagName,assets --jq '.tagName, (.assets | length)'`.

Run local workflows in the foreground. An outer command deadline must cover the
whole workflow, including its longest permitted test; a 60s wrapper cannot cover
the publisher controls' 130s allowance. Measure complete expanded work before
reusing a deadline. The full lightgun NTSC/PAL/prefix/wrap matrix measured 2.1942s
alongside the complete CI workload on Apple Clang 21 arm64; its provisional 5s
deadline follows twice-complete-success calibration and retains lane scaling.
GSD's `workflow.test_gate_timeout` is deliberately 0 for this one-shot CMake
workflow: its supported no-timer mode keeps execution in the foreground while
each CTest test retains its enforced deadline. The outer watch-mode detector is
redundant here and can otherwise preempt valid tests; no replacement cap is inferred
from another timeout.

Focused CTest runs need a separate report, for example
`ctest --preset ci -R '^input[.]' --output-junit input-focused.xml`.
Keep `junit.xml` and `junit-release.xml` for complete inventory checks.
The presets explicitly set `jobs: ""`; `CTEST_PARALLEL_LEVEL` does not override
that choice. Use explicit `ctest --parallel N` when measuring worker limits.

For a local current/base package comparison, use the existing
`tests/cmake/quality_evidence.cmake` producer with the actual event base. Preserve
failed outputs separately and use a fresh output path. Require exit 0 and freshly
completed current/base inventories, matching source/toolchain/fixture identities
and all three actual ZIPs on each side. A failed recursive invocation can leave
partial current or older base files; their presence alone establishes no pass.

## Withdrawing a release

A published release cannot be edited, so a bad one is withdrawn and fixed forward:

1. Point `latest` back at the last good release: `gh release edit v<good> --latest`.
2. Edit the bad release's notes to "Withdrawn: <reason>; use v<good>".
3. Fix forward with a `fix:` pull request, which cuts the next patch release.
4. Delete a release only when its content must not be distributed. Its tag name is then gone for good.

## Pinned downloads

`pins.txt` lists every pinned download as `name version url sha256`, and `scripts/fetch-pin.sh` checks the SHA-256
before use. Take a long-term-support patch release when it fixes a security advisory, and move to the next
long-term-support line before the current one's end of life. Each bump is one `ci:` pull request whose digest comes
from the release API (the asset's `digest` field). Packages installed with apt stay unpinned: they are signed, and
drift fails loudly as a build failure or a hash change.

## Repository settings

`scripts/apply-repo-settings.sh` holds the repository's settings as code, and `.github/rulesets/main.json` holds the
branch ruleset. The script applies, idempotently:

- squash merging only, with the pull request's title and body as the commit message, auto-merge, branch deletion
  on merge, branch updates, and no wiki or projects;
- secret scanning with push protection, private vulnerability reporting, Dependabot alerts and security updates;
- Actions pinned by full commit SHA, a read-only default token, and no pull request approval by Actions;
- immutable releases;
- the eight labels: `bug`, `compatibility`, `enhancement`, `docs`, `performance`, `needs-info`, `nightly`, `digest`;
- the ruleset, replaced by name: no deletion or force push on the default branch, linear history, pull requests
  with no required approval and squash merging only, and the `CI required` check from GitHub Actions, with no
  bypass.

The owner runs `scripts/apply-repo-settings.sh` with admin rights, once after creating the repository and again at
each milestone tune-up. The ruleset blocks a pull request until a run has reported `CI required`, so it is applied
only once CI does.

`scripts/apply-repo-settings.sh --check` only reads. It compares every setting above, each label's colour and
description, and the live ruleset with the committed file, prints one `drift:` line for each difference, and exits
1 if there was any (`no drift on <repository>` otherwise). Run it at each tune-up and after changing anything in
the repository's settings page. Change a setting in the script, not by hand, then apply it.

## Release App and secrets

The GitHub App `muhtari-release`, installed on this repository only, makes the tokens that open and merge release
pull requests and merge Dependabot updates. A merge with an App token starts the `push` runs on `main` that a
merge with `GITHUB_TOKEN` would not. Its repository permissions are Contents, Pull requests, Issues and Workflows
read and write, and Metadata read; each workflow narrows its token further.

CI holds one variable and one secret:

- `MUHTARI_APP_CLIENT_ID`, an Actions variable. It is not secret; `dependabot-automerge.yml` also writes it in,
  because Actions variables may not reach Dependabot-triggered runs.
- `MUHTARI_APP_PRIVATE_KEY`, stored twice: as an Actions secret and as a Dependabot secret, because a
  Dependabot-triggered run sees only Dependabot secrets.

To rotate the key, on the App's settings page generate a new private key, then run
`gh secret set MUHTARI_APP_PRIVATE_KEY < <key file>` and
`gh secret set MUHTARI_APP_PRIVATE_KEY --app dependabot < <key file>`, delete the old key on the settings page, and
delete the key file. Only the owner handles the key.

## Inspecting a CI run

Save the complete response before inspecting a run; a failed transport must stop the command:

```sh
gh run view RUN_ID --json attempt,status,conclusion,headSha,url,jobs > build/ci-run.json &&
cmake -DINPUT=build/ci-run.json -DEXPECTED_SHA=FULL_COMMIT_SHA -P tools/check_ci_run.cmake
```

The audit records the attempt and requires that exact source and a completed run with one successful `CI required` job. It prints every
failed, skipped or unfinished job before deciding, so one known failure cannot hide another. The existing
aggregate owns required versus advisory status: failed library/WASM reports remain visible without becoming
required gates. A passing audit establishes the required verdict, not that every report-only job passed. Inspect
every listed failure and its job log before claiming which checks failed. Keep prior attempts as failed evidence;
retry only a classified transient failure at the same source, using the existing targeted job rerun.

## Weekly digest

`policy.digest.controls` uses CMake's native PATH modification so the same fake-API controls run on Windows
and POSIX hosts; its semicolon-path case prevents CMake list expansion from breaking executable lookup.
Output directories resolve through the shell so relative and Windows drive-rooted paths share one contract.
The controls also exercise native jq CRLF output; binary mode keeps its shell scalars and TSV records intact.

The Maintenance digest workflow previews one public report, with structured evidence as a downloadable artifact
and Markdown in its job summary. Mondays at 06:00 UTC it updates the one open issue labelled `digest`. Scheduling
is best effort; use **Run workflow** on `main` with **publish** selected to recover a missed report. Pin the issue
once if useful. Read its actionable triage at later phase discussions. PR and feature-branch previews cannot
publish, and the workflow uses no release App credentials. Publishers queue without cancellation.

For local inspection, run `sh scripts/digest.sh preview build/digest`; dates default to the preceding completed
UTC calendar month. Pass start and exclusive end timestamps to inspect another window. Manual dispatch also has
a current-partial-month option. Collection includes all pages and attempts, including cancelled/control cost;
rerun passes are candidates, not proven flakes. Samples identify their window and run links. Queue/dependency,
compile share, CTest utilization and image/tool history remain explicitly unavailable without matching evidence.
A partial update preserves the prior monthly section; a newer complete month replaces it. Failed required reads
or issue writes fail visibly and leave the previously published success timestamp intact. Duplicate open digest
issues require consolidation before retrying; reports never close other work or change settings.

## Dependabot updates

`.github/dependabot.yml` updates the pinned GitHub Actions weekly; they are the project's only dependencies. Minor
and patch updates come as one grouped pull request and each major update as its own, so a major never holds the
group back. Their commits use the `ci` prefix, which cuts no release. A 7-day cooldown proposes a version only
once it is a week old, so a compromised release pulled within days never reaches the unattended merge; security
updates are not delayed.

`.github/workflows/dependabot-automerge.yml` runs on Dependabot's pull requests only, and only when Dependabot also
triggered the run. That initial eligibility check arms auto-merge; it does not revoke
auto-merge after every later human push. Its `decide` step reads the update type: a patch or minor update turns on squash auto-merge with an App token, which merges once `CI required`
passes; a major update waits for a person to read its release notes and merge it. `policy.dependabot` runs that
decision on synthetic events.
