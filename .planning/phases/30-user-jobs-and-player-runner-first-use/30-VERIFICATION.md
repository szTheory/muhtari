---
phase: 30-user-jobs-and-player-runner-first-use
verified: 2026-11-20T02:09:36Z
status: passed
score: "5/5 must-haves verified"
covered_files:
  - .planning/phases/30-user-jobs-and-player-runner-first-use/30-01-PLAN.md
  - .planning/phases/30-user-jobs-and-player-runner-first-use/30-01-SUMMARY.md
  - .planning/phases/30-user-jobs-and-player-runner-first-use/30-REVIEW.md
  - .planning/preparation/A7800-ECOSYSTEM-AND-USERS.md
  - README.md
  - docs/libretro.md
  - docs/runner.md
  - docs/status.md
  - host/status_text.c
  - libretro/muhtari_libretro.info.in
  - tests/runner/tests.cmake
covered_digest: "v4:sha256:7515aed17494502c86079a3f538609588b1f5d3852738eaf9b1c6e6066c9c828"
behavior_unverified: 0
overrides_applied: 0
decision_coverage:
  honored: 0
  total: 0
  not_honored: []
---

# Phase 30: User jobs and player/runner first use Verification Report

**Phase Goal:** Players and runner users can reach a first useful result from the current release using accurate, concise instructions, while maintainers can trace these paths to current evidence-backed user jobs.
**Verified:** 2026-11-20T02:09:36Z
**Status:** passed
**Re-verification:** No — initial verification

## Goal Achievement

### Observable Truths

| # | Truth | Status | Evidence |
|---|---|---|---|
| 1 | The current player, runner/homebrew-developer, and C-integrator jobs map to real deliverables with dated evidence, evidence class, confidence and limits, sibling ownership, and revisit triggers. | ✓ VERIFIED | `.planning/preparation/A7800-ECOSYSTEM-AND-USERS.md` §3.1 lines 122–187 maps each job to the libretro core, runner, or library/package routes; labels the player reports as one 2026-10-29 owner session and the other two needs as inferences; records source/revision and revisit conditions. The related onboarding record is linked. |
| 2 | A player can follow the README from a supported release package to the no-content test card or a lawful cartridge load, with prerequisites, expected result, optional BIOS behavior, controls and honest compatibility limits. | ✓ VERIFIED | `README.md` lines 12–55 names the native archives, matches their six platform/architecture variants against the live release, explains configured core/info directories, Load Core/Start Core/Load Content, the expected card, lawful `.a78`/headerless `.bin` content, Fast boot and controls/BIOS links. The compatibility counts match `docs/compatibility.md`; `tests/cmake/install_line.cmake` ran against the local package and installed both core and `.info` files. The current official [core installation guide](https://docs.libretro.com/guides/download-cores/), [directory guide](https://docs.libretro.com/guides/change-directories/), and [content launch guide](https://docs.libretro.com/guides/launch-content/) support the menu and configured-directory terminology. |
| 3 | A runner user can run the test card and a finite own-image sample, distinguish probing from execution, interpret output and refusal classes, and reach the complete CLI reference. | ✓ VERIFIED | The packaged runner produced the documented card hash with exit 0. Against the generated supported fixture, the documented `--frames 120 --hash-frame 120` path produced a frame-120 hash with exit 0. The packaged `--help` output lists exit 1, 2 and 77; generated Atari 2600 probe and run calls both returned 77 and the updated refusal text. `README.md` links `docs/runner.md`. |
| 4 | Atari 2600 identification remains separate from run support, both host consumers use the neutral shared refusal message, and runner probe/load assertions preserve refusal status 77. | ✓ VERIFIED | `host/status_text.c` maps `MUHTARI_STATUS_ERR_UNSUPPORTED_2600` to `unsupported: Atari 2600`; `runner/main.c` and `libretro/libretro.c` consume that shared formatter. `tests/runner/tests.cmake` checks the exact probe message and exit 77, and the HSC refusal cases keep exit 77. Direct packaged probe and run checks returned 77 with the same message. No Atari 2600 feature is presented as upcoming; ownership points to `twentysixhundo`. |
| 5 | Status, package metadata and local links agree with current evidence, distinguish source changes from published-release evidence, and contain no stale Phase 25 entry path. | ✓ VERIFIED | The live `gh release view --repo szTheory/muhtari --json tagName,publishedAt,isDraft,isPrerelease,assets,url` query exited 0 and returned stable v0.18.0 with 20 assets ([release URL](https://github.com/szTheory/muhtari/releases/tag/v0.18.0)), matching the README archive list. `docs/status.md` points to the valid v11.0 audit anchor and explicitly says its receipt predates this phase's corrections. `policy.compat_doc_fresh`, `policy.claims_wording` and `libretro.info_claims` passed; the generated package `.info` contains the corrected Souper/BupChip description. The BankSet note in `docs/libretro.md` names the 52 KiB/HSC condition. No Phase 25 path appears in README or status. |

**Score:** 5/5 truths verified (0 present, behavior-unverified).

### Negative Constraints

| Constraint | Status | Evidence |
|---|---|---|
| Do not treat two reports from one owner session as two users or imply validated runner/integrator adoption. | ✓ VERIFIED | The evidence document states both title reports came from one session and labels runner and integrator demand as inference, with low confidence for external demand. |
| Do not imply entitlement to another person's media or turn test-card/diagnostic results into universal compatibility claims. | ✓ VERIFIED | README asks for lawfully obtained images; the evidence/status pages keep diagnostics and the 204-title ledger scoped and say unevaluated is not failing. No game or BIOS bytes were added. |
| Do not present image identification as run support or Atari 2600 emulation as an upcoming Muhtari feature. | ✓ VERIFIED | README and runner reference separate `--probe` from execution; refusal status and sibling ownership are explicit. |
| Do not use an older release receipt as proof that this phase's package corrections were published. | ✓ VERIFIED | `docs/status.md` explicitly says the v11.0 receipt predates the phase corrections. `docs/runner.md` distinguishes source wording from the previous package wording. The live release check still resolves to v0.18.0. |

### Required Artifacts

The plan artifact query returned 8/8 present with no stub/pattern issues. The following wiring was checked separately.

| Artifact | Expected | Status | Details |
|---|---|---|---|
| `README.md` | Player and runner first-use paths and entry links | ✓ VERIFIED | Substantive commands, expected results, package choices and links; directly linked to the full references. |
| `docs/runner.md` | CLI identification and refusal reference | ✓ VERIFIED | Documents probe/run distinction, command/output behavior and exit classes; linked by README. |
| `host/status_text.c` | Shared runner/libretro Atari 2600 refusal wording | ✓ VERIFIED | Formatter supplies the same text consumed by both host implementations. |
| `tests/runner/tests.cmake` | Process assertions for refusal wording and stable status | ✓ VERIFIED | Exact message and exit 77 assertions remain for probe and refused run paths. |
| `docs/libretro.md` | Controls, BIOS and configuration-specific refusal guidance | ✓ VERIFIED | README anchors resolve to `Controls` and `BIOS files`; BankSet refusal names the 52 KiB/HSC condition. |
| `docs/status.md` | Current release navigation and compatibility totals | ✓ VERIFIED | Live release and historical audit links are distinct; totals agree with the generated compatibility document. |
| `libretro/muhtari_libretro.info.in` | Source for corrected packaged core description | ✓ VERIFIED | CMake `configure_file` wiring is present; generated packaged `.info` contains supported Souper and unavailable BupChip wording. |
| `.planning/preparation/A7800-ECOSYSTEM-AND-USERS.md` | Dated evidence for the three current jobs | ✓ VERIFIED | Section 3.1 contains the three deliverable mappings, evidence bounds, owners and revisit triggers. |

### Key Link Verification

The automated key-link query verified 3/5 links. Its two false results were checked manually: the relationships are indirect through the built runner/test harness and CMake generation rather than literal imports.

| From | To | Via | Status | Details |
|---|---|---|---|---|
| `README.md` | `docs/runner.md` | Runner commands and CLI behavior | WIRED | README links to the full reference; documented card and finite run were exercised through the packaged executable. |
| `README.md` | `docs/libretro.md` | Configured setup, controls and BIOS references | WIRED | README links directly to the guide and its checked `#controls` and `#bios-files` anchors. |
| `host/status_text.c` | `tests/runner/tests.cmake` | Shared message and exit 77 assertions | WIRED (manual) | Automated query cannot see that tests invoke the built runner. The formatter is consumed by runner and libretro; process assertions match its output and require 77. |
| `docs/status.md` | `.planning/milestones/v11.0-MILESTONE-AUDIT.md` | Historical release evidence | WIRED | Link resolves to `#delivery-and-verification-chronology`; text says it is historical and not proof of this phase's publication. |
| `libretro/muhtari_libretro.info.in` | `libretro/CMakeLists.txt` | Configure packaged core description | WIRED (manual) | `configure_file` consumes the template; the locally generated package `.info` was inspected and carries the corrected description. |

### Data-Flow Trace (Level 4)

| Artifact | Data value | Source | Produces current data | Status |
|---|---|---|---|---|
| README and status page | Current release/tag and native assets | GitHub latest-release endpoint, checked with `gh release view` at 2026-11-20T02:00:35Z; [v0.18.0 release](https://github.com/szTheory/muhtari/releases/tag/v0.18.0) | Yes — v0.18.0, stable, 20 assets | ✓ FLOWING |
| `docs/compatibility.md` and `docs/status.md` | Pinned title totals | `tests/compat/ledger.csv` through `tools/compat_doc.cmake`; `policy.compat_doc_fresh` passed | Yes — generated page reports 204 pinned entries, 201 unevaluated and 3 skipped | ✓ FLOWING |
| Packaged RetroArch metadata | `.info` description | `libretro/muhtari_libretro.info.in` through CMake `configure_file` | Yes — generated package file contains the corrected description | ✓ FLOWING |

### Behavioral Spot-Checks

| Behavior | Command | Result | Status |
|---|---|---|---|
| Packaged no-content runner card | `build/ci/release/extract/runner/bin/muhtari-run --frames 1 --hash-frame 1` | Exit 0; `frame 1 sha256 9eb011e22feb3f48da2c0abf3dd8d5236d4d0f5c0e67a732eb2172259e0a05c4` | ✓ PASS |
| Finite supported-cartridge sample | `build/ci/release/extract/runner/bin/muhtari-run --frames 120 --hash-frame 120 --rom build/ci/runner-probe/linear-v3-16k` | Exit 0; `frame 120 sha256 a2039dfec6c6b253a800d8f2a22c917aee174787618bf83508d04408901840fa` | ✓ PASS |
| Probe of identified Atari 2600 fixture | Packaged runner `--probe build/ci/runner-probe/2600-4k` | Exit 77; identifies kind `2600` and prints `unsupported: Atari 2600` | ✓ PASS |
| Attempt to run Atari 2600 fixture | Packaged runner `--frames 1 --rom build/ci/runner-probe/2600-4k` | Exit 77; prints `unsupported: Atari 2600` and runs no frame | ✓ PASS |
| CLI exit classes | Packaged runner `--help` | Exit 0; help lists 1 failure, 2 usage and 77 refused | ✓ PASS |
| README package install line | `cmake -DREADME=README.md "-DBASE_URL=file://$PWD/build/ci/" -DHOME_DIR=/tmp/muhtari-phase30-verify-install-20261120 -P tests/cmake/install_line.cmake` | Exit 0; installed `muhtari_libretro.dylib` and `muhtari_libretro.info` into the isolated scratch directory | ✓ PASS |
| Focused documentation/metadata checks | `ctest --test-dir build/ci --no-tests=error --output-on-failure -R '^(policy\.compat_doc_fresh|policy\.claims_wording|libretro\.info_claims)$'` | Exit 0; all three selected checks passed | ✓ PASS |

The full `cmake --workflow --preset ci` suite was not rerun. The scope-specific package and runner checks above, source inspection, current release metadata and the recorded full-suite result were sufficient; no uncovered behavior required another full run.

Local commands have no hosted run URL (`N/A-local`); the release lookup resolves to the public v0.18.0 release URL above.

### Test Quality Audit

| Test File | Linked Requirement | Active | Skipped | Circular | Assertion Level | Verdict |
|---|---|---:|---:|---:|---|---|
| `tests/runner/tests.cmake` | RUNNER-01, DOCS-01 | Yes | 0 | 0 | Exact values, output and process status | PASS |
| `tests/cmake/install_line.cmake` | PLAYER-01 | Yes | 0 | 0 | Packaged command and installed-file integration | PASS |
| `tests/libretro/tests.cmake`, `tests/cmake/info_claims.cmake` | PLAYER-01, DOCS-01 | Yes | 0 | 0 | Simulated host behavior, capability-to-test mapping and description bound | PASS |
| `tests/policy/tests.cmake` | DOCS-01 | Yes | 0 | 0 | Required wording and generated-document equality | PASS |

No disabled or skipped test markers were found in the linked CMake checks. Generated fixtures are test inputs; expected refusal text, status codes and the card digest are asserted as fixed values, not written from the runner under test. The free-form `.info` wording is manually checked against the mapper/sound source and build output; the metadata test intentionally checks capability keys and length, not prose semantics.

### Requirements Coverage

| Requirement | Source Plan | Description | Status | Evidence |
|---|---|---|---|---|
| PERSONA-01 | `30-01-PLAN.md` | Dated, bounded evidence maps the player, runner/homebrew developer and C integrator jobs to current deliverables. | ✓ SATISFIED | User-job evidence document §3.1, with the dated onboarding record and explicit inference limits. |
| PLAYER-01 | `30-01-PLAN.md` | Release core install and lawful RetroArch content load are documented with prerequisites, result, controls, BIOS and limits. | ✓ SATISFIED | README/libretro guide; live archive list, package install consumer, and official RetroArch terminology checked. |
| RUNNER-01 | `30-01-PLAN.md` | Runner users can execute the test card or a finite own-image sample and interpret probe/output/refusal guidance. | ✓ SATISFIED | Direct packaged card and frame-120 checks, help output, refusal output and runner reference. |
| DOCS-01 | `30-01-PLAN.md` | Local links, release references, package claims and commands agree with source and current evidence. | ✓ SATISFIED | Focused policy/metadata checks passed; status, metadata output and changed anchors were inspected. |

No additional requirement is mapped to Phase 30 in `REQUIREMENTS.md`.

### Decision Coverage

No trackable decisions in CONTEXT.md.

### Anti-Patterns Found

| File | Line | Pattern | Severity | Impact |
|---|---:|---|---|---|
| `.planning/preparation/A7800-ECOSYSTEM-AND-USERS.md` | 57 | `TODO` in a historical row describing an external emulator | Info | Pre-existing source note outside the updated user-jobs section; not an unfinished Muhtari implementation task. |

No unreferenced `TBD`, `FIXME` or `XXX` debt markers, stubs, empty implementations or hollow user-facing values were found in the phase's modified artifacts.

### Human Verification Required

None. Phase scope is documentation and package guidance; local package installation, generated metadata, source wiring and runner commands were checked. No actual RetroArch host run, hardware input, broad gameplay claim or publication is claimed, consistent with the project policy.

### Gaps Summary

None. All merged roadmap and plan truths are verified, all eight planned artifacts are substantive and wired, all four phase requirements are satisfied, and no human-only item remains. The live release is still v0.18.0: the docs distinguish that published receipt from the phase's locally generated package corrections, which remain for the ordinary v12.0 patch-release delivery.

---

_Verified: 2026-11-20T02:09:36Z_  
_Verifier: the agent (gsd-verifier)_
