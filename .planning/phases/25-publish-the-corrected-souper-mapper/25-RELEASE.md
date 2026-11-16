# Phase 25 package delivery receipt

Status: v0.16.0 is published from the verified release source and is latest; its exact-source gates and unchanged v0.15.0 snapshot pass. The Task 2 README/status update passes local checks; its PR and final main CI are pending.

## Scope and source

This plan delivers the already-merged corrected Souper mapper through the established release-please and publisher path. The supported profile remains a software-model contract for generated fixtures and homebrew; it does not claim physical cartridge or retail-game compatibility. The three deliverables remain the C library, muhtari-run, and libretro core, with window, audio, and input devices supplied by a host. No emulator behavior, API/state format, dependency, release workflow, or compatibility scope changes here.

- Corrected Souper commit: bb70dcf86a3872d3981304bd8c6f664dc58dc401.
- Fetched main at start: a252aeb7dfc6a54a884ffc66995d445d75d531fc.
- Selected milestone branch start: 4649c299b29550dd90c8688fe350d3ac0f8f8743.
- The corrected commit is an ancestor of fetched main (read-only Git check, exit 0).
- Branch: milestone/v9.0-publish-souper.

## Preserved release baseline

Captured before any push or merge on 2026-11-16. The complete normalized API record and tag lookup are in ignored build/phase25/v015-before.json.

| Field | Captured value |
|---|---|
| Release ID | 445523668 |
| Tag and release name | v0.15.0 |
| Target commit | 2b47656cb0307fb5fc10d9ea3fbfd2236830c355 |
| Draft / prerelease | true / false |
| Published time | null |
| Complete asset list | 0 assets |
| Tag object | direct commit 2b47656cb0307fb5fc10d9ea3fbfd2236830c355 |
| Peeled commit | 2b47656cb0307fb5fc10d9ea3fbfd2236830c355 |
| Release-notes body SHA-256 | bf3c1e8e78a7d180af51e9235d5dc6fd15a2d38350724b290177df79c4769b6d |
| Canonical snapshot SHA-256 | 4aa0ea4e2d5090918e6d4c03c60bc8dd8efa7090bba050846f802e7266e2c9ad |

The baseline body identifies the prior Souper feature draft and its source. Latest was v0.14.0 (release ID 443575829). The v0.16.0 tag and release lookups each returned confirmed HTTP 404. The complete release list had no existing v0.16.0 identity, and autorelease: pending returned no open PRs. These are read-only observations; no old release, tag, or asset was changed.

## Checks before the release path

All local checks ran from the selected checkout. The timestamp below is when this receipt captured and checked the command results, not the original command start time. Local run URL: N/A-local.

| Command / evidence | Exit | Result | Evidence captured at (UTC) | Run URL |
|---|---:|---|---|---|
| Initial git diff --check | 0 | Clean. | 2026-11-16T02:22:02Z | N/A-local |
| Initial feature title/body: sh scripts/check-title.sh with the actual proposed title and body | 0 | feat: make corrected Souper mapper support downloadable is accepted. | 2026-11-16T02:22:02Z | N/A-local |
| Initial current single-version check | 1 | Found two forbidden historical version literals in docs/status.md; that availability paragraph was corrected. | 2026-11-16T02:22:02Z | N/A-local |
| Initial NEXT single-version check | 1 | Found the next release literal in the same paragraph; corrected with it. | 2026-11-16T02:22:02Z | N/A-local |
| Corrected current and NEXT single-version checks | 0 / 0 | Only version.txt holds each checked value. | 2026-11-16T02:22:02Z | N/A-local |
| Document budgets | 0 | All 25 checked documents are within budget. | 2026-11-16T02:22:02Z | N/A-local |
| Souper fixture identity control | 0 | Exact fixture row admitted; forged pin, digest, and unrelated path rejected. | 2026-11-16T02:22:02Z | N/A-local |
| Owning hygiene policy.title CTest | 0 | All title fixtures pass with --no-tests=error. | 2026-11-16T02:22:02Z | N/A-local |
| Owning CI release.single_version_source CTests | 0 | All five current/NEXT and guard self-tests pass with --no-tests=error. | 2026-11-16T02:22:02Z | N/A-local |
| First full cmake --workflow --preset ci | 8 | 722/723 pass; policy.claims_wording found the missing README disclaimer. Added the exact required scope sentence. | 2026-11-16T02:22:02Z | N/A-local |
| Focused policy.claims_wording CTest after the README correction | 0 | Required compatibility disclaimer now passes. | 2026-11-16T02:22:02Z | N/A-local |
| Second full cmake --workflow --preset ci after the README correction | 0 | 723/723 standard tests and all 8 release-package tests pass; local library, runner, and libretro packages were generated. Raw log is ignored. | 2026-11-16T02:22:02Z | N/A-local |
| Final cmake --workflow --preset ci on the exact Task 1 tree | 0 | Started 2026-11-16T02:24:51Z, finished 2026-11-16T02:25:23Z; 723/723 standard tests and all 8 release-package tests pass. | 2026-11-16T02:25:23Z | N/A-local |

Initial configure exposed absent pinned tools/fixtures. They were fetched only with scripts/fetch-pin.sh, which checks each pins.txt SHA-256 before extraction: MAME XML, Wasmtime, 7800basic, OpenBIOS, and rcheevos. Hygiene and CI presets then configured successfully. No dependency was added.

The README correction is an execution deviation required to unblock the existing CI gate: its claims policy already required the exact compatibility disclaimer. It states that the listed evidence does not claim universal title compatibility and adds no compatibility claim. The final exact Task 1 tree passed the cheap checks and full local CI recorded above before shipment.

## Pre-shipment main timing context

Captured full gh run view responses and paginated job records for each run under ignored build/phase25/. All four timing samples are push runs on main, attempt 1. Run durations use creation through completion; first-job queue delay uses the first job start. No jobs were cancelled in these samples.

| Workflow sample | Head | Run duration / initial delay | Longest work |
|---|---|---|---|
| [Current CI 43770820444](https://github.com/szTheory/muhtari/actions/runs/43770820444) | a252aeb7 | 44m09s / 2s | build (windows-arm64) 41m06s; independent-base step 25m13s. |
| [Previous CI 43765372722](https://github.com/szTheory/muhtari/actions/runs/43765372722) | 3fc2a84e | 45m57s / 3s | build (windows-arm64) 41m56s; independent-base step 25m32s. |
| [Current Release 43770820538](https://github.com/szTheory/muhtari/actions/runs/43770820538) | a252aeb7 | 13s / 2s | Release-please and release-target passed; all package gates skipped because no release was created. |
| [Previous Release 43765372812](https://github.com/szTheory/muhtari/actions/runs/43765372812) | 3fc2a84e | 15s / 2s | Release-please and release-target passed; all package gates skipped because no release was created. |

Each CI run had 18 successful jobs and two expected push-trigger skips (title, perf); the required aggregate passed. The two Release runs each had two successful early jobs and nine downstream skipped jobs because no release event existed. Those skipped package gates are not release acceptance. The latest-release status and Actions API files are preserved in scratch for complete status inspection.

Recent failure history remains separate: [main CI 43748043795](https://github.com/szTheory/muhtari/actions/runs/43748043795) on 6b6c7874 failed title.pal.bios at its macOS x64 deadline, then the quality join and required aggregate failed. The failure log shows all other tests in that job passed. The scoped timeout follow-up merged as PR #68; exact-main CI [43757444707](https://github.com/szTheory/muhtari/actions/runs/43757444707) passed. This plan does not reopen that attempt or alter deadlines.

The current and preceding Windows ARM64 independent-base work differs by only 19 seconds and both starts have minimal queue delay. These two samples support no safe topology, serial-baseline, cache, deadline, quality-floor, or required/advisory membership change. Keep existing coverage and limits; the README consistency correction is the bounded low-risk fix.

## Current acceptance timing

| Sample | Source | Duration / initial queue | Critical path |
|---|---|---|---|
| [Main CI 43801428495](https://github.com/szTheory/muhtari/actions/runs/43801428495) | Push a0319a9c | 43m43s / 1s | Windows ARM64 39m42s, then wasm32 3m48s. Required aggregate passed; title and perf were skipped for the push event. |
| [Release 43801428444](https://github.com/szTheory/muhtari/actions/runs/43801428444) | Push a0319a9c; release source a0319a9c | 52m23s / 1s | Release CI waited for Windows ARM64 (41m52s) and wasm32 (3m16s); release-equality (16m45s) ran in parallel. Publisher took 6m31s and latest install took 8s. |
| [Previous main CI 43797146046](https://github.com/szTheory/muhtari/actions/runs/43797146046) | Push 4486a25e | 46m33s / 1s | Windows ARM64 43m29s; independent-base step 26m06s. |
| [Previous Release 43797146173](https://github.com/szTheory/muhtari/actions/runs/43797146173) | Push 4486a25e | 27s / 1s | No new release was created; release package checks were correctly skipped. |

Both current acceptance runs had no canceled or superseded jobs, and only the event-gated skips described above. GitHub emitted a general macOS ARM64 capacity annotation for the Release run; the latest-install job started two seconds after publish and completed in eight seconds, so no material queue delay was observed. The measured timings support no safe change to workflow topology, caches, deadlines, quality floors, or required/advisory membership.

A separate scheduled [Nightly run 43803812599](https://github.com/szTheory/muhtari/actions/runs/43803812599) on a0319a9c ended with overall conclusion `cancelled`: its report-only `prop.peek_pure` test hit the configured 390s CTest timeout, and the `prop` job reached its 9-minute job limit before the test inventory completed. `prop.split_run` took 376.98s; vectors, fuzz, benchmarks, BE equality, and the report job passed. This run is outside required main CI and release acceptance. Its complete response and job log are saved as `nightly-observation.json` and `nightly-prop.log`; no retry or test/pipeline policy change is in this scoped plan.

## Release acceptance record

Publication and latest-install evidence completed before public availability wording changes. The release path stayed on the normal feature PR, release-please PR, and existing Release workflow; no manual tag, release, upload, workflow change, or version override was used.

Raw release API responses, full run responses/logs, and downloaded CI artifacts are ignored under `build/phase25/`. Compact commands, exits, sources, and authoritative run URLs follow.

- Feature PR [#71](https://github.com/szTheory/muhtari/pull/71) merged as main commit 4486a25e. PR CI [43792556216](https://github.com/szTheory/muhtari/actions/runs/43792556216) passed; main CI [43797146046](https://github.com/szTheory/muhtari/actions/runs/43797146046) passed on the squash merge.
- Release-please opened PR [#72](https://github.com/szTheory/muhtari/pull/72) with the ordinary 0.16.0 minor bump. Its diff contains only `.release-please-manifest.json`, `CHANGELOG.md`, and `version.txt`, with no version override. PR CI [43797178106](https://github.com/szTheory/muhtari/actions/runs/43797178106) passed on head 5b62aaaa; the App squash-merged it to a0319a9c.
- Release workflow [43801428444](https://github.com/szTheory/muhtari/actions/runs/43801428444) reported `release_created=true`, tag `v0.16.0`, release ID 446591624, and `RELEASE_SHA=a0319a9c29886d776e37b818d38d36fe810f486d`. The same SHA is the main push caller head. `v0.16.0` is a lightweight tag to that commit. The published stable release is latest and has publication time `2026-11-16T04:48:49Z`.
- Main CI [43801428495](https://github.com/szTheory/muhtari/actions/runs/43801428495) passed on a0319a9c. `tools/check_ci_run.cmake` accepted the run's exact caller SHA (exit 0); title and perf were the only expected push-event skips.
- Release `tools/check_ci_run.cmake` accepted caller head a0319a9c (exit 0). The required `release-ci / CI required`, `release-equality / equality`, `publish`, and `install-line-latest` jobs all completed successfully. Expected direct-workflow and event-only skips are listed in `release-run.json`; they do not replace the passing nested release-source gates.
- Downloaded release CI artifacts (exit 0) show all six native archive `commit.txt` receipts and both native/PowerPC equality receipts equal a0319a9c. The portable bundle embeds that commit, and each quality artifact identifies source a0319a9c and run 43801428444. The native and PowerPC hash and identity files compare equal. Workflow-equivalent `check_junit.cmake` inventory checks passed for both native lanes (723 tests each plus 7 release tests each) and the Emscripten lane (7 tests), including `core.state.souper` and `emscripten.souper`.
- The existing publisher passed preparation, the 19-archive and forbidden-byte checks, main-ref attestations, tampered-copy rejection, publication, and exact-tag download/checksum/attestation verification. Public release metadata contains exactly the 19 expected ZIP names plus nonempty `SHA256SUMS`; all 20 assets have positive sizes. `install-line-latest` passed against the published latest alias using the unchanged README install command.
- Read-only release/tag/latest API capture and validation passed (exit 0). `v016-release.json` identifies the target SHA and published stable state; `v016-tag.json` peels to the same commit; `latest.json` identifies v0.16.0.
- Re-fetched and canonically normalized the full v0.15.0 release, paginated assets, and tag after publication. `cmp build/phase25/v015-before.json build/phase25/v015-after.json` passed (exit 0); both snapshots hash to `4aa0ea4e2d5090918e6d4c03c60bc8dd8efa7090bba050846f802e7266e2c9ad`. Release ID 445523668, draft state, body, zero assets, direct tag object, and peeled target remain equal to the baseline above.

The release-source CI and equality evidence belong to a0319a9c. The docs follow-up PR and final main CI are still pending.

## Task 2 documentation checks

Before changing availability copy, `gh api repos/szTheory/muhtari/releases/latest --jq '{id,tag_name,target_commitish,draft,prerelease,published_at,html_url}'` and `gh pr list --state open --json number,title,headRefName,baseRefName,url` both succeeded (exit 0, 2026-11-16T05:26:23Z, [latest release](https://github.com/szTheory/muhtari/releases/latest)); v0.16.0 was stable/latest at a0319a9c and no PR was open. A fresh latest query on the clean follow-up branch at 2026-11-16T05:40:44Z again showed release 446591624, stable v0.16.0 at a0319a9c, published at 04:48:49Z.

The README and status now describe the latest published package without a numeric package version. They retain the library/runner/libretro deliverables, host-supplied devices, generated-fixture versus retail-game evidence limit, pinned compatibility results, missing GameDrive/BupChip/frontend work, and the scoped completion standard. The README's install line is unchanged. Both pages link to the latest release and this receipt identifies the exact package source.

The reviewed docs are on branch `phase/25-souper-availability-docs`, based on exact main `a0319a9c29886d776e37b818d38d36fe810f486d`. `git diff --name-only a0319a9c29886d776e37b818d38d36fe810f486d` listed only this receipt, `README.md`, and `docs/status.md`; `git diff --check` passed (exit 0, 2026-11-16T05:46:53Z). No install command, product code, release metadata, compatibility policy, or test policy changed.

| Command | Exit | Result | Captured at (UTC) | Run URL |
|---|---:|---|---|---|
| `git diff --check a0319a9c29886d776e37b818d38d36fe810f486d` | 0 | Clean; the branch diff contains only the three documented paths. | 2026-11-16T05:46:53Z | N/A-local |
| `sh scripts/check-title.sh 'docs: document the published Souper package'` with body “Update the README and project status to reflect the verified latest package and generated-fixture scope. Keep the installation command, version source, and compatibility limits unchanged.” | 0 | Conventional Commit title and actual PR body accepted. | 2026-11-16T05:46:53Z | N/A-local |
| `cmake -DROOT="$PWD" -P tests/cmake/single_version.cmake` | 0 | Current `0.16.0` literal appears only in `version.txt`. | 2026-11-16T05:46:53Z | N/A-local |
| `cmake -DROOT="$PWD" -DNEXT=ON -P tests/cmake/single_version.cmake` | 0 | `0.17.0` and `0.16.1` appear only in `version.txt`. | 2026-11-16T05:46:53Z | N/A-local |
| `cmake -DROOT="$PWD" -P tests/cmake/budgets.cmake` | 0 | All 25 checked documents are within budget. | 2026-11-16T05:46:53Z | N/A-local |
| `scripts/fetch-pin.sh` for wasmtime-aarch64-macos, 7800basic-wasm, 7800openbios-asm, mame-a7800-xml, mame-a2600-xml, and rcheevos | 0 for each | All six downloaded inputs matched `pins.txt` digests. | 2026-11-16T05:42:29Z | N/A-local |
| Initial `cmake --preset ci` and `cmake --preset hygiene` in the clean checkout | 1 each | Configure identified missing pinned inputs; fetched and verified them with the existing script above. | 2026-11-16T05:41:25Z | N/A-local |
| `cmake --preset ci` and `cmake --preset hygiene` after fetching pins | 0 each | Both owning CTest inventories configured successfully. | 2026-11-16T05:42:54Z | N/A-local |
| `ctest --test-dir build/hygiene --no-tests=error -R '^policy\.title$' --output-on-failure` | 0 | Owning hygiene title policy passed (1/1). | 2026-11-16T05:46:53Z | N/A-local |
| `ctest --test-dir build/ci --no-tests=error -R '^(policy\.claims_wording|release\.single_version_source|release\.single_version_source\.next|release\.single_version_source\.self_test|release\.single_version_source\.provenance\.(current|next))$' --output-on-failure` | 0 | All six owning claims and version-source tests passed (6/6). | 2026-11-16T05:46:53Z | N/A-local |

Drafting from the earlier milestone checkout briefly failed the existing claims policy twice because the required compatibility sentence was omitted/wrapped; it is restored verbatim on one line and the clean-branch owning test passes. An initial CTest selector used the wrong inventory and ran no tests (exit 8); `ctest -N` identified the correct configured inventory. The clean checkout's initial CMake configure also needed pinned inputs, all fetched with SHA-256 verification before retry. These failures were diagnosed and resolved without changing product behavior or policy.

The documentation follow-up PR and final main CI are pending.
