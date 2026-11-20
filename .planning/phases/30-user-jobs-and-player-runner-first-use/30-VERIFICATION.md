---
phase: 30-user-jobs-and-player-runner-first-use
verified: 2026-11-20T05:23:43Z
status: passed
score: "5/5 must-haves verified"
covered_files:
  - .planning/LESSONS.md
  - .planning/milestones/v11.0-MILESTONE-AUDIT.md
  - .planning/phases/30-user-jobs-and-player-runner-first-use/30-01-PLAN.md
  - .planning/phases/30-user-jobs-and-player-runner-first-use/30-01-SUMMARY.md
  - .planning/phases/30-user-jobs-and-player-runner-first-use/30-REVIEW.md
  - .planning/phases/31-installed-c-library-quickstart/31-01-SUMMARY.md
  - .planning/phases/31-installed-c-library-quickstart/31-02-SUMMARY.md
  - .planning/preparation/A7800-ECOSYSTEM-AND-USERS.md
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
  - tests/install/tests.cmake
  - tests/libretro/tests.cmake
  - tests/policy/tests.cmake
  - tests/release/ym_consumers.cmake
  - tests/runner/tests.cmake
  - tools/compat_doc.cmake
covered_digest: "v4:sha256:0d7700839ff945af31a4ba88e8236871580b5efaf572e07e7117ae545539e562"
behavior_unverified: 0
overrides_applied: 0
re_verification:
  previous_status: passed
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
**Verified:** 2026-11-20T05:23:43Z
**Status:** passed
**Re-verification:** No — the previous report had no gaps, so this was a fresh goal check prompted by its stale README fingerprint.

## Goal Achievement

### Observable Truths

| # | Truth | Status | Evidence |
|---|---|---|---|
| 1 | Current player, runner/homebrew-developer and C-integrator jobs map to real deliverables with dated evidence, evidence class, confidence limits, sibling ownership and revisit triggers. | ✓ VERIFIED | [A7800-ECOSYSTEM-AND-USERS.md](../../preparation/A7800-ECOSYSTEM-AND-USERS.md#31-current-jobs-and-evidence:119) §3.1 maps the player to the libretro package, the runner job to `muhtari-run`, and the C host job to the C library and installed package routes. It dates the release and source evidence, labels the two runner/integrator needs as inference, limits the two player reports to one owner session, and records ownership and revisit conditions. |
| 2 | A player can follow the current release package to the no-content test card or a lawful cartridge load, with prerequisites, expected result, optional BIOS behavior, controls and scoped compatibility evidence. | ✓ VERIFIED | [README.md](../../../README.md:12) names the six native core archives and the running RetroArch process architecture, gives configured Cores/Core Info directions, Load Core/Start Core/Load Content, and the expected no-content and supported-cartridge results. Fast boot, optional BIOS, RetroPad controls and the 204-title evidence limits are stated. The current stable [v0.18.0 release](https://github.com/szTheory/muhtari/releases/tag/v0.18.0) was checked with `gh release view --repo szTheory/muhtari --json tagName,publishedAt,isDraft,isPrerelease,assets,url` on 2026-11-20: 20 assets include the six named native core archives. Current [Libretro core installation guidance](https://docs.libretro.com/guides/download-cores/), [directory guidance](https://docs.libretro.com/guides/change-directories/), [starting-a-game guidance](https://docs.libretro.com/guides/starting-a-game/) and [content-launch guidance](https://docs.libretro.com/guides/launch-content/) confirm the directory and menu terminology. The packaged install-line consumer and simulated libretro checks passed in the current local workflow. No RetroArch host run is claimed. |
| 3 | Runner users can run the test card and a finite own-image sample, distinguish identification from execution, interpret output/refusal classes and find the complete CLI reference. | ✓ VERIFIED | README lines 57–84 link the package, test-card command/hash, optional `--probe`, finite 120-frame run and [runner reference](../../../docs/runner.md). Direct packaged-runner commands returned the documented test-card hash and a frame-120 hash for the generated fixture. A 2600 image probe reports `kind 2600` and exits 77; attempting to run it reports `unsupported: Atari 2600` and exits 77. Invalid `--frames 0` exits 2, while a missing image exits 1. |
| 4 | Atari 2600 identification remains separate from run support; both host consumers use the shared refusal text, and runner assertions preserve refusal status 77. | ✓ VERIFIED | [host/status_text.c](../../../host/status_text.c:28) returns `unsupported: Atari 2600`; [runner/main.c](../../../runner/main.c:593) and [libretro/libretro.c](../../../libretro/libretro.c:1289) use the shared formatter. [tests/runner/tests.cmake](../../../tests/runner/tests.cmake:885) checks the probe kind, exact refusal text and status, and its HSC refusal case retains 77. Direct packaged probe/run checks also returned 77. README and runner docs keep 2600 ownership with `twentysixhundo`. |
| 5 | Status, package metadata and local links agree with current evidence; source corrections are distinct from published-release evidence, and no stale Phase 25 path remains in the entry documents. | ✓ VERIFIED | [docs/status.md](../../../docs/status.md:8) uses the moving latest-release URL and says the historical v11.0 receipt predates the current corrections. Its 204/201/3 totals match [the generated compatibility page](../../../docs/compatibility.md:14) and ledger source; the current local workflow passed the freshness and claims checks. The `.info` template describes supported Souper behavior separately from unavailable BupChip audio; `libretro/CMakeLists.txt` generates and installs it, and the generated local candidate was inspected. The BankSet example is scoped to 52 KiB with HSC attached and its process tests exercise that condition. The README’s Apple Silicon install command is unchanged from its pre-phase line. All 23 relative README links and anchors resolve, including the additive [C host quickstart](../../../docs/embedding-quickstart.md), [example](../../../examples/embedding/quickstart.c) and [embedding guide](../../../docs/embedding.md). Those new C-host links and package checks are additive; the incremental [Phase 30 review](30-REVIEW.md) found no defect. |

**Score:** 5/5 truths verified (0 present, behavior-unverified).

### Negative Constraints

| Constraint | Status | Evidence |
|---|---|---|
| Do not treat two reports from one owner session as separate users or imply runner/integrator adoption. | ✓ VERIFIED | The persona evidence says the Choplifter and Ninja Golf reports came from one 2026-10-29 owner session; runner and C-integrator needs are explicitly inference, not interviews or adoption. |
| Do not imply entitlement to someone else's media or turn test-card/diagnostic results into universal compatibility claims. | ✓ VERIFIED | README requests a lawfully obtained image and scopes the 204-title ledger; `docs/compatibility.md` says unevaluated means untested and diagnostics do not establish gameplay. No image assets were added. |
| Do not treat image identification as run support or present Atari 2600 emulation as an upcoming Muhtari feature. | ✓ VERIFIED | README and runner reference separate `--probe` from execution; source tests require 77 for refusal; ownership is assigned to `twentysixhundo`. |
| Do not use an older release receipt as evidence that this phase's package corrections are published. | ✓ VERIFIED | `docs/status.md` labels the v11.0 receipt historical and says it predates the corrections. The README points to the current moving release URL; no publication claim is made for this phase's source corrections. |

### Required Artifacts

The GSD artifact query returned 8/8 present with no stub or pattern issues. The additional source and link checks are shown below.

| Artifact | Expected | Status | Details |
|---|---|---|---|
| `README.md` | Player/runner first-use paths and entry links | ✓ VERIFIED | Substantive package choices, commands, expected results, legal-content wording, limits and working links; checked all 23 relative targets and anchors. |
| `docs/runner.md` | CLI identification and refusal reference | ✓ VERIFIED | Documents probe/run distinction, output, refusal and exit classes; the packaged runner produced matching results. |
| `host/status_text.c` | Shared runner/libretro 2600 refusal wording | ✓ VERIFIED | Shared formatter returns the neutral unsupported message used by both consumers. |
| `tests/runner/tests.cmake` | Process assertions for refusal wording/status | ✓ VERIFIED | Exact output and exit 77 assertions remain active and passed in the current local workflow. |
| `docs/libretro.md` | Controls, BIOS and configuration guidance | ✓ VERIFIED | Controls and optional BIOS text match source behavior; 52 KiB BankSet refusal is qualified by attached HSC. |
| `docs/status.md` | Current release and compatibility navigation | ✓ VERIFIED | Latest-release pointer, historical-receipt distinction and title totals match current evidence. |
| `libretro/muhtari_libretro.info.in` | Source of the packaged RetroArch description | ✓ VERIFIED | Souper and selected YM2151 support are distinguished from unavailable BupChip audio; the description is within the existing limit. |
| `.planning/preparation/A7800-ECOSYSTEM-AND-USERS.md` | Dated evidence for three product jobs | ✓ VERIFIED | Current jobs, confidence, evidence limits, sibling ownership and revisit triggers are present. |

### Key Link Verification

The GSD key-link query recognized 3/5 direct references. Its two remaining “target not referenced in source” results were checked manually: one is a runtime-to-process-test assertion, and one is CMake `configure_file` wiring.

| From | To | Via | Status | Details |
|---|---|---|---|---|
| `README.md` | `docs/runner.md` | Commands and CLI reference match runner behavior | ✓ WIRED | README link resolves; documented options and exit codes match packaged runner output and process tests. |
| `README.md` | `docs/libretro.md` | Configured directories, controls and BIOS links | ✓ WIRED | README links to the existing controls and BIOS sections; both targets and anchors resolve. |
| `host/status_text.c` | `tests/runner/tests.cmake` | Shared refusal output and status 77 | ✓ WIRED | Both consumers call the shared formatter. Process assertions match its exact string and exit 77; direct probe and run reproduce them. |
| `docs/status.md` | `.planning/milestones/v11.0-MILESTONE-AUDIT.md` | Historical release evidence is separate from latest URL | ✓ WIRED | The target and `delivery-and-verification-chronology` anchor resolve; status explicitly marks it historical. |
| `libretro/muhtari_libretro.info.in` | `libretro/CMakeLists.txt` | Configure and install generated `.info` | ✓ WIRED | `configure_file` consumes the template, then CMake installs the generated file to `info/`; the built candidate contains the corrected description. |

### Data-Flow Trace (Level 4)

| Artifact | Data value | Source | Produces current data | Status |
|---|---|---|---|---|
| README and status page | Latest release and native archive list | Public GitHub latest-release endpoint; checked with `gh release view` on 2026-11-20 | Yes — stable v0.18.0, 20 assets including six native core archives | ✓ FLOWING |
| README and compatibility/status docs | Pinned title totals | `tests/compat/ledger.csv` through `tools/compat_doc.cmake`; freshness and claims checks in the current local workflow | Yes — 204 total, 201 unevaluated and 3 skipped | ✓ FLOWING |
| README runner instructions | Test-card and finite-run output | Current packaged `muhtari-run`; direct process checks against a generated supported fixture | Yes — exact documented test-card hash and frame-120 result | ✓ FLOWING |
| Packaged RetroArch metadata | Core description | `.info.in` → `configure_file` → installed `info/` file; generated candidate inspected | Yes — corrected Souper/YM2151 and unavailable-audio wording | ✓ FLOWING |
| Additive C-host quickstart link | Package route, source and expected output | Phase 31 docs/example plus extracted-archive and staged-package checks in the current local workflow | Yes — local checks built and ran the documented source against the candidate archive | ✓ FLOWING |

### Behavioral Spot-Checks

| Behavior | Command | Result | Status |
|---|---|---|---|
| Packaged no-content runner card | `build/ci/release/extract/runner/bin/muhtari-run --frames 1 --hash-frame 1` | Exit 0; `frame 1 sha256 9eb011e22feb3f48da2c0abf3dd8d5236d4d0f5c0e67a732eb2172259e0a05c4` | ✓ PASS |
| Finite supported-image sample | Packaged runner `--frames 120 --hash-frame 120 --rom build/ci/runner-probe/linear-v3-16k` | Exit 0; `frame 120 sha256 a2039dfec6c6b253a800d8f2a22c917aee174787618bf83508d04408901840fa` | ✓ PASS |
| Atari 2600 image identification | Packaged runner `--probe build/ci/runner-probe/2600-4k` | Exit 77; prints `kind 2600` and `unsupported: Atari 2600` | ✓ PASS |
| Atari 2600 run refusal | Packaged runner `--frames 1 --rom build/ci/runner-probe/2600-4k` | Exit 77; prints `unsupported: Atari 2600`, runs no frame | ✓ PASS |
| Invalid usage and unreadable input | Packaged runner `--frames 0`; then `--frames 1 --rom build/ci/runner-probe/no-such-image` | Exit 2 for invalid usage; exit 1 for missing input | ✓ PASS |
| Test-card visual description | Packaged runner `--frames 1 --dump-frame 1:/tmp/muhtari-phase30-test-card.png` | Exit 0; inspected output shows the chart, red top band, green bottom band and border | ✓ PASS |
| README local links | Node checker over relative README links and anchors | 23 checked, 0 missing targets or anchors; exit 0 | ✓ PASS |
| Current full local workflow | `MUHTARI_RELEASE_QUICKSTART_PKG_CONFIG=1 cmake --workflow --preset ci` | Latest run after README and Phase 31 changes: exit 0, 736/736 CI tests and 8/8 release checks. Reused, not rerun. Local result only; no hosted CI claim or hosted run URL. | ✓ PASS |

### Probe Execution

| Probe | Command | Result | Status |
|---|---|---|---|
| CLI image probe | `build/ci/release/extract/runner/bin/muhtari-run --probe build/ci/runner-probe/2600-4k` | Exit 77; identifies kind 2600 and prints the shared unsupported message | PASS |
| Probe-script inventory | No phase-declared or conventional `scripts/*/tests/probe-*.sh` file was found | The phase's CLI probe was executed directly above; no shell probe was substituted | N/A |

### Test Quality Audit

| Test File | Linked Requirement | Active | Skipped | Circular | Assertion Level | Verdict |
|---|---|---:|---:|---:|---|---|
| `tests/runner/tests.cmake` | RUNNER-01, DOCS-01 | Yes | 0 | 0 | Exact values, output and process status | PASS |
| `tests/cmake/install_line.cmake` | PLAYER-01 | Yes | 0 | 0 | Installed package files and documented command behavior | PASS |
| `tests/libretro/tests.cmake`, `tests/cmake/info_claims.cmake` | PLAYER-01, DOCS-01 | Yes | 0 | 0 | Simulated host behavior and capability-to-test mapping | PASS |
| `tests/policy/tests.cmake`, `tests/cmake/claims_wording.cmake` | DOCS-01 | Yes | 0 | 0 | Required wording, generated-doc equality and policy controls | PASS |
| `tests/install/tests.cmake`, `tests/release/ym_consumers.cmake` | Additive C-host links | Yes | 0 | 0 | Extracted package builds, output and failure controls | PASS |

The active files contain no disabled tests. The current workflow includes `policy.no_skip`. Fixture-writing checks supply fixed inputs or explicit fault controls; expected runner messages, statuses and hashes are asserted independently rather than captured from the system under test.

### Requirements Coverage

| Requirement | Source Plan | Description | Status | Evidence |
|---|---|---|---|---|
| PERSONA-01 | `30-01-PLAN.md` | Three current jobs trace to deliverables with evidence and confidence limits. | ✓ SATISFIED | User-job evidence §3.1, the dated release lookup, one-session player notes and explicit inference labels. |
| PLAYER-01 | `30-01-PLAN.md` | Install a supported release core and load lawful Atari 7800 content with controls and support limits. | ✓ SATISFIED | README, current native archive list, official RetroArch guidance, package install consumer and simulated libretro checks. |
| RUNNER-01 | `30-01-PLAN.md` | Run a test card or finite image sample and interpret output and refusal status. | ✓ SATISFIED | Direct packaged runner checks, runner process assertions and the full local workflow. |
| DOCS-01 | `30-01-PLAN.md` | Current first-use, status and package claims agree with source and evidence. | ✓ SATISFIED | README links/anchors, status/ledger counts, generated metadata path, live release lookup and local workflow. |

No additional requirement is mapped to Phase 30 in `REQUIREMENTS.md`.

### Decision Coverage

No trackable decisions in `30-CONTEXT.md` (GSD decision-coverage query: honored 0/0).

### Anti-Patterns Found

| File | Line | Pattern | Severity | Impact |
|---|---:|---|---|---|
| `.planning/preparation/A7800-ECOSYSTEM-AND-USERS.md` | 57 | `TODO` in a historical external-emulator research row | ℹ️ Info | The marker describes an old research-status cell, not unfinished Muhtari work; it is outside the refreshed current-jobs section. |

No unreferenced `TBD`, `FIXME` or `XXX` markers, stubs, empty implementations or hollow user-facing values were found. Null initialization in the C quickstart is populated and used through its normal allocation/frame path.

### Human Verification Required

None. The documentation judgments were checked against source, generated package metadata, direct runner output, the current public release and current official RetroArch guidance. Project rule 6 does not require owner UAT for this documentation-only path; no identity-bound, hardware-only or subjective product decision remains. No actual RetroArch run is claimed.

### Gaps Summary

None. All five merged truths and four negative constraints are verified; all eight planned artifacts are substantive; all five key links are wired; all four requirements are satisfied; and no human-only item remains. The README's C-host quickstart is an additive link whose source and local package checks are covered by Phase 31. The latest public release remains v0.18.0; the status page accurately separates that historical package from this phase's not-yet-published metadata correction.

---

_Verified: 2026-11-20T05:23:43Z_
_Verifier: the agent (gsd-verifier)_
