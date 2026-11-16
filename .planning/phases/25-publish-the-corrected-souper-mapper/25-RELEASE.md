# Phase 25 package delivery receipt

Status: pre-publication evidence captured; no new package has been accepted yet. Public availability copy remains conservative.

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

## Current and preceding main timing

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

## Release acceptance record

Before public availability copy changes, prove one exact source identity from release-please. Required evidence: the release PR has the ordinary pre-1.0 minor bump, edits only version.txt, .release-please-manifest.json, and CHANGELOG.md, and has no version override. The release target output, tag target, and release record must agree on the release-please SHA.

The release-source CI, generated Souper library/runner/libretro/portable fixture coverage, full native/PowerPC equality, publisher preparation and integrity checks, main-ref attestations, tampered-copy rejection, 19 expected ZIPs plus SHA256SUMS, exact-tag downloads, and unchanged README latest-install command must all pass. Require publish, install-line-latest, release-ci / CI required, and release-equality / equality as completed/success. Save the complete run and exact source evidence in ignored build/phase25/; record command, exit, timestamp, and authoritative Actions URL below.

- Release PR, merge SHA, release-please source SHA, tag, release ID: pending.
- Exact-source CI and equality: pending.
- Publisher, provenance, integrity, publication, and latest install: pending.
- v0.15.0 canonical before/after comparison: pending.
- Final documentation PR, final main CI, latest alias, and final v0.15.0 comparison: pending.

Until those gates pass, README/status remain unpublished and release-version neutral beyond the existing latest package. The release job owns version inputs, tag, draft, and assets; no manual tag/release/upload path is used.
