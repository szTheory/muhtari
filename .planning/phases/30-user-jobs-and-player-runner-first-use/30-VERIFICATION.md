---
phase: 30-user-jobs-and-player-runner-first-use
verified: 2026-11-20T09:52:47Z
status: passed
score: "5/5 truths verified"
covered_files:
  - .planning/LESSONS.md
  - .planning/REQUIREMENTS.md
  - .planning/ROADMAP.md
  - .planning/milestones/v11.0-MILESTONE-AUDIT.md
  - .planning/phases/30-user-jobs-and-player-runner-first-use/30-01-PLAN.md
  - .planning/phases/30-user-jobs-and-player-runner-first-use/30-01-SUMMARY.md
  - .planning/phases/30-user-jobs-and-player-runner-first-use/30-REVIEW.md
  - .planning/phases/31-installed-c-library-quickstart/31-01-SUMMARY.md
  - .planning/phases/31-installed-c-library-quickstart/31-02-SUMMARY.md
  - .planning/phases/31-installed-c-library-quickstart/31-REVIEW.md
  - .planning/preparation/A7800-ECOSYSTEM-AND-USERS.md
  - AGENTS.md
  - README.md
  - docs/bankset-tests.md
  - docs/compatibility.md
  - docs/embedding-quickstart.md
  - docs/embedding.md
  - docs/libretro.md
  - docs/runner.md
  - docs/status.md
  - docs/testing.md
  - examples/embedding/CMakeLists.txt
  - examples/embedding/quickstart.c
  - host/status_text.c
  - libretro/CMakeLists.txt
  - libretro/libretro.c
  - libretro/muhtari_libretro.info.in
  - runner/exit_status.h
  - runner/main.c
  - tests/cmake/claims_wording.cmake
  - tests/cmake/info_claims.cmake
  - tests/cmake/install_line.cmake
  - tests/compat/ledger.csv
  - tests/consumer/CMakeLists.txt
  - tests/consumer/quickstart_faults.c
  - tests/emscripten/bundle.cmake
  - tests/install/tests.cmake
  - tests/libretro/tests.cmake
  - tests/policy/publish.cmake
  - tests/policy/tests.cmake
  - tests/release/ym_consumers.cmake
  - tests/runner/tests.cmake
  - tools/compat_doc.cmake
covered_digest: "v4:sha256:a392c46def98403c4efbf78b71325e35ae5064dbeccd922111092d9afddde149"
behavior_unverified: 0
overrides_applied: 0
re_verification:
  previous_status: human_needed
  previous_score: "5/5"
  gaps_closed: []
  gaps_remaining: []
  regressions: []
decision_coverage:
  honored: 0
  total: 0
  not_honored: []
---

# Phase 30: User jobs and player/runner first use Verification Report

**Phase Goal:** Players and runner users can reach a first useful result from the current release using accurate, concise instructions, while maintainers can trace these paths to current evidence-backed user jobs.

**Verified:** 2026-11-20T09:52:47Z
**Status:** passed
**Re-verification:** No — the previous report had no gaps, so this refresh used initial goal-check mode and reassessed its four human items as source-text checks.

## Goal Achievement

### Observable Truths

| # | Truth | Status | Evidence |
|---|---|---|---|
| 1 | Current player, runner/homebrew-developer and C-integrator jobs map to real deliverables with dated evidence, evidence class, confidence limits, sibling ownership and revisit triggers. | ✓ VERIFIED | The [current jobs section](../../preparation/A7800-ECOSYSTEM-AND-USERS.md#31-current-jobs-and-evidence) maps all three jobs to the libretro package, runner and C library/package route. It records the 2026-11-19 release capability evidence, 2026-10-29 player observations from one owner session, inferred runner and integrator demand, sibling ownership and revisit triggers. |
| 2 | A player can follow the current release package to the no-content test card or a lawful cartridge load, with prerequisites, expected result, optional BIOS behavior, controls and scoped compatibility evidence. | ✓ VERIFIED | [README.md](../../../README.md) names the six current native archives, matches the running RetroArch process architecture, points to configured Cores/Core Info directories, and describes Load Core, Start Core and Load Content. It states the test card and cartridge startup outcomes, fast boot, optional BIOS, RetroPad controls and the 204-title evidence limits. The refreshed latest-release lookup at 2026-11-20T09:44Z resolved to stable v0.18.0 with 20 assets, including all six native core archives. No RetroArch host run is claimed. |
| 3 | Runner users can run the test card and a finite own-image sample, distinguish identification from execution, interpret output/refusal classes and find the complete CLI reference. | ✓ VERIFIED | README gives the pinned no-content command/hash, optional probe and separate 120-frame sample, says the run hash is content-dependent, lists exit 77/1/2, and links [docs/runner.md](../../../docs/runner.md). The existing runner process tests assert the test-card digest and 2600 refusal; the current serial CI run passed all 736 tests. |
| 4 | Atari 2600 identification remains separate from run support; both host consumers use the shared refusal text and runner assertions preserve refusal status 77. | ✓ VERIFIED | [host/status_text.c](../../../host/status_text.c) returns “unsupported: Atari 2600”. [runner/main.c](../../../runner/main.c) and [libretro/libretro.c](../../../libretro/libretro.c) call the shared formatter. [tests/runner/tests.cmake](../../../tests/runner/tests.cmake) checks identification and exact refusal output/status 77; current CI serial run passed. |
| 5 | Status, package metadata and local links agree with current evidence; source corrections are distinct from published-release evidence, and no stale Phase 25 path remains in the entry documents. | ✓ VERIFIED | [docs/status.md](../../../docs/status.md) uses the moving latest-release URL, separates the v11.0 historical receipt from unpublished Phase 30 corrections, and reports 204 titles with 201 unevaluated and 3 skipped, matching the generated compatibility page. [libretro/muhtari_libretro.info.in](../../../libretro/muhtari_libretro.info.in) describes supported Souper and selected YM2151 behavior separately from unavailable BupChip audio; [libretro/CMakeLists.txt](../../../libretro/CMakeLists.txt) configures and installs the generated info file. The BankSet refusal example is conditional on a 52 KiB set with HSC attached. Search of README and docs/status.md found no Phase 25 path. Existing link/anchor review found 23 README links and anchors resolving; the reviewed README has not changed since that check. |

**Score:** 5/5 truths verified (0 present, behavior-unverified).

### Negative Constraints

All four plan prohibitions were checked against the named source text and test evidence. They are source-text consistency checks, not owner-only decisions, hardware runs, or consent boundaries. Project rule 6 says the agent makes these judgments and asks the owner only for identity-bound consent or a run on their hardware.

| Prohibition | Status | Evidence |
|---|---|---|
| Do not turn the two reports from one owner session into two independent users or imply runner/integrator adoption. | ✓ VERIFIED | The [persona evidence](../../preparation/A7800-ECOSYSTEM-AND-USERS.md#31-current-jobs-and-evidence) identifies the Choplifter and Ninja Golf reports as one 2026-10-29 owner session and explicitly labels runner/C-integrator needs as inference with no adoption evidence. |
| Do not imply entitlement to other people's cartridge/firmware media or universal compatibility. | ✓ VERIFIED | The [README](../../../README.md) requests a lawfully obtained cartridge, says Muhtari ships no game or BIOS images, and states the 204/201/3 ledger limits. The [BIOS guide](../../../docs/libretro.md#bios-files) says no firmware is bundled and provides no acquisition claim. [Project status](../../../docs/status.md) says generated mapper checks do not establish retail-game compatibility. |
| Do not conflate identification with run support or suggest Atari 2600 emulation is upcoming. | ✓ VERIFIED | [Runner docs](../../../docs/runner.md) state probe identifies without proving execution, identify 2600 images as unsupported, and assign emulation to twentysixhundo. [AGENTS.md](../../../AGENTS.md) reserves Atari 2600 emulation for that sibling while retaining recognition/refusal. Current runner process tests check kind 2600 and refusal exit 77. |
| Do not use an older release receipt to claim Phase 30 corrections are published. | ✓ VERIFIED | [Project status](../../../docs/status.md) explicitly says the v11.0 receipt predates Phase 30 corrections and does not show them in a published package. The current README uses a moving latest-release pointer; this verification makes no publication claim for the Phase 30 changes. |

### Required Artifacts

The GSD artifact query returned 8/8 present with no stub or pattern issues. Two key-link query results require manual wiring checks, recorded below.

| Artifact | Expected | Status | Details |
|---|---|---|---|
| README.md | Player and runner first-use paths and entry links | ✓ VERIFIED | Substantive setup, commands, expected results, legal-content wording, scoped limits and linked references are present. |
| docs/runner.md | CLI identification and refusal reference | ✓ VERIFIED | Documents probe/run distinction, output, refusal and exit classes; tests assert the exact 2600 status/message. |
| host/status_text.c | Shared runner/libretro 2600 refusal wording | ✓ VERIFIED | The shared formatter returns the message used by both host consumers. |
| tests/runner/tests.cmake | Process assertions for refusal wording/status | ✓ VERIFIED | Probe and HSC refusal assertions preserve exit 77 and the neutral message. |
| docs/libretro.md | Controls, BIOS and configuration guidance | ✓ VERIFIED | Documents RetroPad mapping, optional BIOS behavior and the condition-specific BankSet refusal. |
| docs/status.md | Current release and compatibility navigation | ✓ VERIFIED | Latest-release pointer and historical-receipt distinction match current release evidence. |
| libretro/muhtari_libretro.info.in | Source of packaged RetroArch description | ✓ VERIFIED | Souper support, selected YM2151 scope and unavailable BupChip audio are stated; existing info-claim checks validate the field and size. |
| .planning/preparation/A7800-ECOSYSTEM-AND-USERS.md | Dated evidence for the three product jobs | ✓ VERIFIED | Evidence class, date, confidence, limitations, owner and revisit triggers are present for each job. |

### Key Link Verification

The GSD key-link query reports 3/5 direct references. Its two “target not referenced in source” results are indirect code/test or build-generation links, which were checked manually.

| From | To | Via | Status | Details |
|---|---|---|---|---|
| README.md | docs/runner.md | Commands and CLI reference match runner behavior | ✓ WIRED | README links to the full reference; its commands and exit codes match the runner process contract tests. |
| README.md | docs/libretro.md | Configured directories, controls and BIOS links | ✓ WIRED | README links to the existing controls and BIOS sections; names and anchors resolve. |
| host/status_text.c | tests/runner/tests.cmake | Shared refusal output and status 77 | ✓ WIRED | runner/main.c and libretro/libretro.c consume the shared formatter; process assertions check its exact text and exit 77. |
| docs/status.md | .planning/milestones/v11.0-MILESTONE-AUDIT.md | Historical release evidence is distinct from the moving latest URL | ✓ WIRED | The delivery/verification anchor resolves and the page labels the receipt historical. |
| libretro/muhtari_libretro.info.in | libretro/CMakeLists.txt | Generated package description | ✓ WIRED | configure_file consumes the template and install(FILES ...) installs its generated output into info/. |

### Data-Flow Trace (Level 4)

| Artifact | Data value | Source | Produces current data | Status |
|---|---|---|---|---|
| README/status | Current stable release and archive names | GitHub latest-release query at 2026-11-20T09:44Z | Yes: stable v0.18.0, 20 assets including six native libretro archives | ✓ FLOWING |
| README and compatibility/status docs | Pinned title totals | tests/compat/ledger.csv through tools/compat_doc.cmake; current full serial CI and freshness checks | Yes: 204 total, 201 unevaluated and 3 skipped | ✓ FLOWING |
| README runner instructions | Test-card and finite-run output | Existing packaged runner evidence and process tests | Yes: pinned card hash is asserted; finite frame sample is tested by the current runner suite | ✓ FLOWING |
| Packaged RetroArch metadata | Core description | .info.in → configure_file → installed info file | Yes: generated candidate used by local package/publisher checks | ✓ FLOWING |
| Additive C-host quickstart links | Package, source and expected output | Phase 31 docs/example and installed-package consumer | Yes: current consumer test passed locally | ✓ FLOWING |

### Behavioral Spot-Checks

| Behavior | Command/evidence | Result | Status |
|---|---|---|---|
| Installed C package consumer | ctest --test-dir build/ci -R '^install[.]consumer$' | Exit 0 after current source changes. | ✓ PASS |
| Core info packaging claim and publisher controls | cmake -DROOT="$PWD" -DWORK="$PWD/build/ci/policy/publish-emscripten-production" -DPRODUCTION_ARCHIVE="$PWD/build/emscripten/muhtari-libretro-emscripten.zip" "-DEXPECTED_SHA=$(git rev-parse HEAD)" -P tests/policy/publish.cmake | Exit 0; current archive and production publisher controls passed. | ✓ PASS |
| Current full serial CI suite | ctest --preset ci --parallel 1 | Exit 0; 736/736 tests in 481.59 seconds. The default-parallel workflow previously hit four local timeouts; those four passed serially. | ✓ PASS |
| CI release package checks | cmake --build --preset ci --target package; then ctest --preset ci-release --parallel 1 | Package build exit 0; release checks 8/8 passed. | ✓ PASS |
| Focused documentation/source formatting gate | focused hygiene.format CTest | Exit 0. | ✓ PASS |
| Emscripten bundle-control CTest | Existing 30-second test under amd64 Docker on the arm64 host | Timed out locally. The production validator controls passed in the direct publisher check above; this report does not claim that CTest passed. | ⚠️ LIMITED |
| Current public release lookup | gh release view --repo szTheory/muhtari --json tagName,publishedAt,isDraft,isPrerelease,assets,url | Exit 0 at 2026-11-20T09:44Z; stable v0.18.0, 20 assets, six native libretro archives. | ✓ PASS |

### Probe Execution

No phase-declared shell probe script is present. The CLI probe/refusal contract is exercised by the runner process assertions in tests/runner/tests.cmake and the current full serial CI suite. Atari 2600 identification and refusal are distinct: identification reports kind 2600; execution refuses with status 77 and the shared unsupported message.

### Requirements Coverage

| Requirement | Source plan | Description | Status | Evidence |
|---|---|---|---|---|
| PERSONA-01 | 30-01-PLAN.md | Three current jobs trace to deliverables with dated evidence and confidence limits. | ✓ SATISFIED | Current jobs section, one-session player observations, dated release/source evidence and explicit inference labels. |
| PLAYER-01 | 30-01-PLAN.md | Install a supported core and load lawful content with steps, expected result, controls and support limits. | ✓ SATISFIED | README, latest native archive lookup, libretro guidance and local package/simulated-host checks. |
| RUNNER-01 | 30-01-PLAN.md | Run a test card or finite image sample and interpret output/refusal status. | ✓ SATISFIED | README and CLI reference match runner process tests; current serial CI passed. |
| DOCS-01 | 30-01-PLAN.md | First-use, status and package claims agree with source and current evidence. | ✓ SATISFIED | Link review, status/ledger totals, generated metadata, current release lookup and local checks. |

No other requirement in REQUIREMENTS.md maps to Phase 30. No requirement assigned to Phase 30 is orphaned from its plan.

### Anti-Patterns Found

| File | Line | Pattern | Severity | Impact |
|---|---:|---|---|---|
| .planning/preparation/A7800-ECOSYSTEM-AND-USERS.md | 57 | TODO in a historical external-emulator research row | ℹ️ Info | Historical research status, outside the refreshed current-jobs section; not unfinished Muhtari implementation. |

No debt-marker blocker, stub, empty implementation or hardcoded empty user-facing data was found in the phase artifacts or their current sources. The refreshed Phase 30 review reports no findings.

### Human Verification Required

None. The four source-text constraints were checked against their cited evidence; no owner-only validation remains.

### Gaps Summary

No failed truth, missing artifact, unwired link, implementation blocker, or human-only item was found. The five goal truths, four requirements and four negative constraints are verified. Local serial CI passed. The report does not claim a host run, hosted CI result, Phase 30 correction publication, or passing default-parallel local workflow.

---

_Verified: 2026-11-20T09:52:47Z_
_Verifier: the agent (gsd-verifier)_
