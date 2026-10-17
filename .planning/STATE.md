---
gsd_state_version: "1.0"
milestone: v1.0
milestone_name: 7800 games play with sound in RetroArch
current_phase: 2
current_phase_name: The CPU matches the public vectors
status: Phase 2 context gathered
stopped_at: Phase 2 context gathered
last_updated: "2026-10-17T00:00:24.570Z"
last_activity: 2026-10-16
state_head: 57b4880f53b5d26234d0060b8902b66ac8ef4aef
progress:
  total_phases: 4
  completed_phases: 1
  total_plans: 21
  completed_plans: 21
  percent: 25
---

# Project State

## Project Reference

See: .planning/PROJECT.md (updated 2026-10-16)

**Core value:** 7800 games behave as they do on the console, identically on every platform, from a small MIT-licensed C library that any host can embed.
**Current focus:** Phase 2 — The CPU matches the public vectors (phase 01 released as v0.1.0)

## Current Position

Phase: 2 — The CPU matches the public vectors
Plan: Not started
Status: Phase 2 context gathered
Last activity: 2026-10-16

Progress: [███░░░░░░░] 25%

## Performance Metrics

**Velocity:**
- Total plans completed: 21
- Average duration: -
- Total execution time: 0.0 hours

**By Phase:**

| Phase | Plans | Total | Avg/Plan |
|-------|-------|-------|----------|
| 01 | 21 | - | - |

**Recent Trend:**
- Last 5 plans: -
- Trend: -

*Updated after each plan completion*
**Per-Plan Metrics:**

| Plan | Duration | Tasks | Files |
|------|----------|-------|-------|
| Phase 01 P01 | 10 min | 2 tasks | 13 files |
| Phase 01 P02 | 6 min | 2 tasks | 9 files |
| Phase 01 P03 | 3 min | 2 tasks | 6 files |
| Phase 01 P04 | 4 min | 2 tasks | 10 files |
| Phase 01 P05 | 4 min | 2 tasks | 9 files |
| Phase 01 P06 | 6 min | 3 tasks | 14 files |
| Phase 01 P07 | 5 min | 3 tasks | 14 files |
| Phase 01 P08 | 4 min | 2 tasks | 11 files |
| Phase 01 P09 | 10 min | 3 tasks | 14 files |
| Phase 01 P10 | 6 min | 3 tasks | 11 files |
| Phase 01 P11 | 6 min | 3 tasks | 15 files |
| Phase 01 P12 | 19 min | 3 tasks | 17 files |
| Phase 01 P13 | 10 min | 3 tasks | 11 files |
| Phase 01 P14 | 16 min | 2 tasks | 7 files |
| Phase 01 P15 | 13 min | 3 tasks | 13 files |
| Phase 01 P16 | 11 min | 2 tasks | 8 files |
| Phase 01 P17 | 20 min | 2 tasks | 8 files |
| Phase 01 P18 | 20 min | 2 tasks | 9 files |
| Phase 01 P19 | 21 min | 3 tasks | 12 files |
| Phase 01 P20 | 17 min | 3 tasks | 8 files |
| Phase 01 P21 | 33 min | 3 tasks | 8 files |

## Accumulated Context

### Decisions

Decisions are logged in PROJECT.md Key Decisions table; the full list with sources is
`.planning/preparation/DECISIONS.md`.
Recent decisions affecting current work:

- Kickoff: CI and release refinements from the owner's lattice_stripe and exifcleaner histories are in ENGINEERING 4–7 (LESSONS XFER-026 to XFER-033) for phase 1.
- [Phase 01]: Seeds come from published nesturbator main 9f9386c661f6; the pinned 3200aa868371 is not on GitHub and differs only in CMakePresets.json
- [Phase 01]: muhtari_run_frame writes audio_count = 0 on error paths; 800 or 801 on success
- [Phase 01]: muhtari_config keeps 4 alignment bytes after size on 64-bit targets; static asserts pin allocator at sizeof(void *)
- [Phase 01]: The test inventory compares names and labels; tests/inventory/<preset>.txt holds sorted name<TAB>labels lines with comma-separated labels
- [Phase 01]: Plan 01's core already met the 0.1.0 contract; 15 mutation runs stood in for a failing RED and the core was not changed
- [Phase 01]: no_skip scans test CMake source for skip and disable properties and names file:line; check_junit treats any testcase without status run as not run
- [Phase 01]: A command list passed through add_test to a -P script escapes its semicolons as $<SEMICOLON>
- [Phase 01]: D-03 holds on the fetched a7800 warm table: $46 = 0xAF5163 (red largest), $C6 = 0x469A46 (green largest); the test card is unchanged
- [Phase 01]: nesturbator citations in PROVENANCE and ASSET_POLICY pin published main 9f9386c661f6; PV.12 cites check_archives.cmake L32-50
- [Phase 01]: muhtari_host is PIC with hidden visibility so the libretro module can link it
- [Phase 01]: The PNG writer takes RGB rows; the runner converts native bytes through muhtari__palette_rgb one row at a time
- [Phase 01]: runner.dump_png pins the card PNG at 232,645 bytes, SHA-256 1c0b16cb9c92d70869d3aee5cd581627f8902cb528e90f1f1d4252d9288ac956
- [Phase 01]: muhtari-run --bench is an alternative to --frames (give one, once); only muhtari_run_frame is timed, and the report is nine key value lines
- [Phase 01]: The exit table lives in runner/exit_status.h as a const struct array (0 done, 1 failure, 2 usage); runner.exit.table requires every --help table line verbatim in docs/runner.md
- [Phase 01]: Runner command-line checks dispatch on -DCASE in tests/runner/tests.cmake; exit-status cases include tests/cmake/expect_fail.cmake from a function
- [Phase 01]: The libretro instance is created in retro_load_game(NULL) with the adapter's own malloc wrapper; retro_init is empty and unload or deinit frees it
- [Phase 01]: The .info keeps is_experimental = true; info_claims exempts it by name as a label, not a capability
- [Phase 01]: library_version comes from MUHTARI_VERSION_STRING in the private generated version header
- [Phase 01]: libretro.vendored reads its file table from PROVENANCE.md section 8 and is registered even when the core is not built
- [Phase 01]: Library installs to literal lib/ and include/ so muhtari.pc keeps prefix=${pcfiledir}/../..; the consumer's MSVC runtime follows the build configuration through a generator expression
- [Phase 01]: Release zips are written into the build directory (build/ci/muhtari-<component>-<os>-<arch>.zip); the ci workflows test, package, then test label release into junit-release.xml
- [Phase 01]: The dev test preset also excludes label release because the dev workflow has no package step
- [Phase 01]: release.single_version_source scans tracked files only, outside version.txt, release-please files, CHANGELOG.md and .planning/; a version followed by a dot is outside the bounded match
- [Phase 01]: install_line.cmake runs the README line verbatim under sh with a scratch HOME; it is a CI step on macOS arm64, not a CTest
- [Phase 01]: abi.undefined allows bzero and __bzero on Darwin through the per-system helper table instead of building the core with -fno-builtin
- [Phase 01]: tests/api/v1.c names muhtari_allocator: abi.api_version found it missing, and v1.c is not frozen until 0.1.0 is released
- [Phase 01]: Hygiene preset sets CMAKE_OSX_SYSROOT=macosx so Homebrew clang-tidy finds the SDK headers; CMake 4 leaves the sysroot empty
- [Phase 01]: The float scan exempts cited section numbers such as section 6.2, so hardware citations in src/ stay as written
- [Phase 01]: abi self-test regexes allow a line break anywhere in the finding title, because CMake wraps messages at a path-dependent column
- [Phase 01]: abi.api_v1 and abi.header.* are registered from tests/abi/tests.cmake so the nofp lane includes one file
- [Phase 01]: The asan container lane needs libclang-rt-21-dev; local x86_64 emulated runs substitute bsdtar for GNU tar
- [Phase 01]: MUHTARI_SHORT_RUNS cuts the 600-frame runs to 60; MUHTARI_NOFP builds the core with -mgeneral-regs-only and registers only abi tests
- [Phase 01]: runner.stop stays portable on wasm32: raise(SIGINT) reaches the handler under wasmtime 48.0.6
- [Phase 01]: Libretro tests register only with the libretro core; wasm32.txt is the ci list minus libretro.*
- [Phase 01]: No be32 flip: the hosted be32 job takes 24-33 s, so hash-equality compares seven files with -DCOUNT=7
- [Phase 01]: R-10 settled on GitHub: MSVC /W4 /WX is clean, the /MT runner and DLL import only KERNEL32.dll, llvm-nm reads the COFF .lib, muhtari.lib has no /DEFAULTLIB
- [Phase 01]: hashes.txt is written with Unix newlines; CMake file(WRITE) writes CRLF on Windows
- [Phase 01]: hygiene.analyzer leaves out clang-tidy's Annex K buffer-handling check, which clang-tidy 21 enables against glibc
- [Phase 01]: check-title.sh rejects a skip token in the title as well as the body, because the title becomes the squash commit's subject
- [Phase 01]: policy.ci_required also requires every job but the report-only wasm32 in CI required's needs
- [Phase 01]: apply-repo-settings.sh --check reads sha_pinning_required, which replaces the unbuilt hygiene.action_pins
- [Phase 01]: Hosted RetroArch 1.22.2 starts the no-game core with -L and no content on macos-15 (run 38939482093); the retroarch job compares its screenshot with the runner's frame through sips and is in CI required
- [Phase 01]: Release rehearsal is a published prerelease rehearsal-<run>-<attempt> deleted by a cleanup job; A4 held (GITHUB_TOKEN on same-repo pull_request gets contents/id-token/attestations write), so no draft-only fallback
- [Phase 01]: publish.sh addresses draft releases by the ID from the create response; a new draft is missing from the release list at first (run 38941477560)
- [Phase 01]: Real release path: release-please with the App token, release-ci at release-please's sha, publish to the draft by ID as latest; re-run on a published release only verifies
- [Phase 01]: publish.sh attested runs attestation and tampered-copy checks before the irreversible publish on both paths
- [Phase 01]: The perf gate reports NO_BASE while the merge base has no CMakePresets.json and is skipped in release.yml's nested ci call (inputs.ref); measured test card Ir/frame 3837643 on ubuntu-26.04
- [Phase 01]: Nightly bench writes max_rss_kb from /usr/bin/time (-v kB on Linux, -l bytes on macOS); the forced failure step runs after bench-wasm32's upload
- [Phase 01]: Ruleset drift compares only the fields and rule parameters the committed file sets, after dropping server-only fields
- [Phase 01]: Dependabot auto-merge writes the App client ID into the workflow because vars may not reach Dependabot runs
- [Phase 01]: workflow.test_gate_timeout is 3: twice the measured 1.36 s cmake --workflow --preset ci time; re-measure it when the suite grows (phase 2 adds the CPU tests)
- [Phase 01]: Timeouts: twice the slowest of 10+ successful runs (5-minute floor for network-bound and roll-up jobs); be32 scale 11, wasm32 scale 3 from measured slowdowns
- [Phase 01]: The phase PR's squash title is feat: a test frame in RetroArch from the library, runner and libretro core, so release-please cuts 0.1.0
- [Phase 01]: The owner verifies nothing by default (AGENTS rule 6, DEC.09, 2026-10-16): the UAT closed with docs.api_names, the hosted RetroArch job and the agent's one-off checks; truths only a pull request or a merge can show are confirmed at ship
- [Phase 01]: The re-review after fix round 1 found the Dependabot actor gate and the rehearsal cleanup lookup incomplete; WR-01 to WR-03 wait for a ci: pull request after the merge, and one code-review fix round is left in this phase
- [Phase 01]: workflow.code_review_depth_overrides is set to [] explicitly: unset, config-get returns the string "[]" and the code-review depth resolver halts

### Pending Todos

- [2026-10-16] [ci] Promote the wasm32 lane into CI required — [todo file](.planning/todos/pending/2026-10-16-promote-the-wasm32-lane-into-ci-required.md)
- [2026-10-16] [ci] Arm the instructions-per-frame gate — [todo file](.planning/todos/pending/2026-10-16-arm-the-instructions-per-frame-gate.md)
- [2026-10-16] [release] Add the release firmware scan with the first ROM code — [todo file](.planning/todos/pending/2026-10-16-add-the-release-firmware-scan-with-the-first-rom-code.md)
- [2026-10-16] [ci] Limit release rehearsals to pushes that change release inputs — [todo file](.planning/todos/pending/2026-10-16-limit-release-rehearsals-to-pushes-that-change-release-inputs.md)
- [2026-10-16] [ci] Close the re-review's findings on the Dependabot gate and the publish checks — [todo file](.planning/todos/pending/2026-10-16-close-the-re-review-findings-on-the-dependabot-gate-and-the-publish-checks.md)

### Ship step (phase 01)

- Shipped 2026-10-16 as PR #7, titled `feat: a test frame in RetroArch from the library, runner and libretro core`
  so release-please cuts 0.1.0 (`scripts/check-title.sh` exit 0 on the title and body). This note is the head
  commit the pull request opened on, so CI and the Release rehearsal run once and no CI-skip commit is needed.
- Before the merge (01-UAT.md test 4), confirmed: the pull request's own Release run 38975022865 (ci,
  rehearsal, install-line, cleanup) was green on its head commit; evidence on PR #7, comment 6225897054. The owner
  merged it on 2026-10-16 as f36c884.
- After the merge (01-UAT.md test 3), confirmed 2026-10-16: release pull request #8 merged itself as 952fce9,
  and its Release run 38983378877 published v0.1.0 as latest with 19 assets and passed install-line-latest.
  `shasum -a 256 -c SHA256SUMS` and `gh attestation verify` (signer workflow, `refs/heads/main`) passed on all 18
  zips; evidence on PR #7, comment 6226988821.
- The `main` ruleset requiring `CI required` is live and `scripts/apply-repo-settings.sh --check` reports no
  drift.

### Blockers/Concerns

None yet.

## Deferred Items

Items acknowledged and deferred at milestone close, most recent first:

| Category | Item | Status | Deferred At | Milestone |
|----------|------|--------|-------------|-----------|
| *(none)* | | | | |

## Session Continuity

Last session: 2026-10-17T00:00:05.419Z
Stopped at: Phase 2 context gathered
Resume file: .planning/phases/02-the-cpu-matches-the-public-vectors/02-CONTEXT.md
