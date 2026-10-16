# Maintaining muhtari

How a release is made, rehearsed, recovered and withdrawn, and how the pinned downloads move. Tests and CI are in
`docs/testing.md`.

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
   - `prepare`: exactly the 18 zips, every build leg's recorded commit equal to the release commit, `SHA256SUMS`;
   - `upload`, then `actions/attest` over `SHA256SUMS`;
   - `attested`: each zip's attestation verifies and a copy with its last byte flipped does not;
   - publish the draft as the latest release;
   - `verify`: `attested` again, `gh release verify`, 19 non-empty assets, and every zip downloaded from
     `releases/latest/download/` matches `SHA256SUMS` and verifies as signed by `release.yml`.
7. `install-line-latest` runs the README's install line verbatim on `macos-15` against the latest release.

The release workflow never cancels a run: runs on `main` queue behind each other.

A version moves to 1.0.0 only through a final `Release-As: 1.0.0` line in the body of the pull request that should
cut it.

## The rehearsal

A pull request from this repository that changes `release.yml`, `scripts/publish.sh`,
`release-please-config.json` or `cmake/packaging.cmake` runs the same publish steps before it can merge. The
rehearsal runs CI on the pull request's head, then publishes a prerelease named `rehearsal-<run id>-<attempt>` that
is never marked latest, verifies it and runs the install line against it, and a `cleanup` job deletes the release
and its tag whatever failed before it. Only the trigger, the token and the `latest` alias differ from a real
release. `docs/testing.md` lists the rehearsal's jobs.

A pull request rehearses rather than a manual run because GitHub starts `workflow_dispatch` only for workflow files
on the default branch, and a draft without a tag cannot be published or verified.

**Fallback.** If a pull request's `GITHUB_TOKEN` ever loses `contents`, `id-token` or `attestations` write, the
rehearsal shrinks to a draft: prepare, upload, attest and the attestation checks, then delete. Publishing and the
download checks are then first exercised by the real release.

Every pushed commit on such a pull request rehearses again, and each rehearsal creates and deletes one public
prerelease.

## Re-running a failed release

- **A real release:** open the failed run and choose "Re-run failed jobs". `publish` is safe to re-run: uploads
  replace the draft's assets, and on a release already published it only runs `verify`, which exits 0 when the
  release has its 19 verified assets. Do not choose "Re-run all jobs": release-please then sees the release pull
  request already tagged and skips `publish`.
- **A failed rehearsal:** choose "Re-run all jobs", which takes a fresh `rehearsal-<run id>-<attempt>` tag, or push
  a new commit. A published tag that was deleted can never be reused under immutable releases. If `cleanup` itself
  failed, delete the leftover by hand: `gh release delete rehearsal-<run id>-<attempt> --cleanup-tag --yes`.

After any rehearsal, `gh release list` must show no `rehearsal-*` release and
`git ls-remote --tags origin 'rehearsal-*'` must print nothing.

## Closing a phase

Before a phase or milestone is called done:

- no pull request labelled `autorelease: pending` is open (`gh pr list --label 'autorelease: pending'` prints
  nothing), so no release is stuck behind a red check;
- the latest release is the expected version with 19 assets:
  `gh release view --json tagName,assets --jq '.tagName, (.assets | length)'`.

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
