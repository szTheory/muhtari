---
phase: 11-full-library-acceptance
verified: 2026-10-29T19:16:21Z
status: passed
score: 16/16 must-haves verified
covered_files:
  - ".planning/phases/11-full-library-acceptance/11-01-PLAN.md"
  - ".planning/phases/11-full-library-acceptance/11-01-SUMMARY.md"
  - ".planning/phases/11-full-library-acceptance/11-02-PLAN.md"
  - ".planning/phases/11-full-library-acceptance/11-02-SUMMARY.md"
  - ".planning/phases/11-full-library-acceptance/11-03-PLAN.md"
  - ".planning/phases/11-full-library-acceptance/11-03-SUMMARY.md"
  - ".planning/phases/11-full-library-acceptance/11-04-PLAN.md"
  - ".planning/phases/11-full-library-acceptance/11-04-SUMMARY.md"
  - ".planning/phases/11-full-library-acceptance/11-05-PLAN.md"
  - ".planning/phases/11-full-library-acceptance/11-05-SUMMARY.md"
  - ".planning/phases/11-full-library-acceptance/11-06-PLAN.md"
  - ".planning/phases/11-full-library-acceptance/11-06-SUMMARY.md"
  - ".planning/phases/11-full-library-acceptance/11-07-PLAN.md"
  - ".planning/phases/11-full-library-acceptance/11-07-SUMMARY.md"
  - ".planning/phases/11-full-library-acceptance/11-08-PLAN.md"
  - ".planning/phases/11-full-library-acceptance/11-08-SUMMARY.md"
  - ".planning/phases/11-full-library-acceptance/11-09-PLAN.md"
  - ".planning/phases/11-full-library-acceptance/11-09-SUMMARY.md"
  - ".planning/phases/11-full-library-acceptance/11-10-PLAN.md"
  - ".planning/phases/11-full-library-acceptance/11-10-SUMMARY.md"
  - ".planning/phases/11-full-library-acceptance/11-11-PLAN.md"
  - ".planning/phases/11-full-library-acceptance/11-11-SUMMARY.md"
  - ".planning/phases/11-full-library-acceptance/11-12-PLAN.md"
  - ".planning/phases/11-full-library-acceptance/11-12-SUMMARY.md"
  - "docs/compatibility.md"
  - "docs/emulation-tests.md"
  - "tests/cmake/compat_doc_controls.cmake"
  - "tests/cmake/compat_ratchet.cmake"
  - "tests/compat/files.cmake"
  - "tests/compat/ledger.csv"
  - "tests/compat/library.cmake"
  - "tests/compat/local.cmake"
  - "tests/compat/local_fixture.cmake"
  - "tests/compat/tests.cmake"
  - "tests/diag/tests.cmake"
  - "tests/policy/tests.cmake"
  - "tools/compat_doc.cmake"
covered_digest: "v4:sha256:37b9630ac0737ebeb804d8145440b7b3298e9af7e78aef5f3600a85253dd4cd9"
behavior_unverified: 0
overrides_applied: 0
re_verification:
  previous_status: gaps_found
  previous_score: 14/16
  gaps_closed:
    - "The generator's sample-token guard now checks basic.<sample> as well as title.<sample> tests; both wrong-token controls fail with the claim, test and sample named."
    - "A semicolon in OUT is rejected before the local pass can split its removal command; the victim sentinel survives, and directory OUT is rejected without creating its scratch tree."
  gaps_remaining: []
  regressions: []
decision_coverage:
  honored: 24
  total: 24
  not_honored: []
prohibitions:
  - statement: "MUST NOT treat pinned metadata or a title-frame hash as gameplay evidence."
    status: resolved
    verification: judgment
  - statement: "MUST NOT accept a compatibility claim from the existence of a backing test when its sample lacks the claimed token."
    status: resolved
    verification: test
  - statement: "MUST NOT turn local acceptance into disclosure of private file paths, names, content or results in committed evidence."
    status: resolved
    verification: test
---

# Phase 11: Full-library acceptance Verification Report

**Phase Goal:** Account for the pinned library honestly and prove every new compatibility claim without private bytes in CI.
**Verified:** 2026-10-29T19:16:21Z
**Status:** passed
**Re-verification:** Yes — after committed fix acceptance plan 11-12. The historical failed report is preserved at `0e86575` in Git.

## Goal Achievement

The pinned MAME CC0 library is represented as a separate population from owned samples and diagnostics. Its 204 entries are explicitly classified: 201 unknown, 3 skipped, and no pass, fail, or unsupported rows. Public evidence remains limited to the behavior each predicate demonstrates. The local library preset contains the private-content path; the CI and release evidence used public inputs only. Current source and tests close the two gaps from the prior 14/16 report.

### Observable Truths

| # | Truth | Status | Evidence |
|---|---|---|---|
| 1 | Every pinned entry has an explicit ledger disposition; untested and unsupported are distinct from passes. | VERIFIED | Direct ledger count: 253 total rows, 204 pinned and 49 owned; pinned rows are 201 `unknown`, 3 `skipped`, 0 `pass`, 0 `fail`, 0 `unsupported`. The required CI workflow includes the pin/ledger controls. |
| 2 | New board, controller and region claims have admissible automated evidence, and prior passes remain protected. | VERIFIED | Full CI passed 673/673 tests plus 6/6 release checks. The ledger uses the owned diagnostics and public samples; `policy.compat_ratchet.controls` and diagnostic CRC consumers remain in CI. |
| 3 | Private content is evaluated only by the isolated library preset, with reproducible command/exit/source evidence; public CI needs no private bytes. | VERIFIED | Public-only library workflow passed 47/47. The visible `library.local` line was `0 of 204 present, 0 pass, 0 fail, 204 absent, commit=0e86575f561b tree=dirty`. The driver captures predicate/Git output and writes under the ignored build directory. |
| 4 | Host metadata is limited to the evidence and does not imply universal compatibility. | VERIFIED | The compatibility page, README disclaimer and `.info` limit are enforced by the current wording/description checks; those checks passed in full CI. |
| 5 | Generated documentation labels the observed behavior and keeps its claim class tied to evidence. | VERIFIED | The page says a `title@` hash is a “public title screen,” not gameplay; it distinguishes declared metadata from playability and says no universal title compatibility is claimed. `policy.compat_doc_fresh` passed. |
| 6 | Each non-empty claim names tests that exercise it, and the sample carries the claimed token. | VERIFIED | The current claim table names the sample-backed tests. The four relevant rows carry their tokens: `rmtramdemo` and `rmtramcompressiondemo` are `supergame+ram`; `pokey` and `rmtromdemo` carry `pokey450`. |
| 7 | A wrong-token sample-backed test is rejected for both `basic.<sample>` and `title.<sample>` claims. | VERIFIED | `tools/compat_doc.cmake` matches the two patterns in separate branches, preserving `CMAKE_MATCH_1`. Current controls mutate `title.simple.fast` and `basic.rmtramdemo`; both name the claim, test and sample in the expected failure. |
| 8 | The generated page stays fresh and within its byte budget; wrong evidence cannot silently inflate claims. | VERIFIED | Current focused CTest run passed `policy.compat_doc_fresh` and its ten-case self-test; `docs.budgets` passed. The self-test includes both sample-token mutations. |
| 9 | The local pass does not disclose paths/names for links, unreadable entries or malformed names, and counts skipped entries. | VERIFIED | The current 13-run public fixture checks dangling and valid links, unreadable-file behavior where observable, semicolons/brackets, wildcards, output faults, and the unread-count line without leaking the fixture names or paths. |
| 10 | Library preset scans use the same safe file rule as the local pass. | VERIFIED | `library.cmake` includes `files.cmake` and calls `compat_files(library ...)` for both scans; `title.local.fixture` exercises the helper and source controls. The public library workflow passed. |
| 11 | `<OUT>.rows` is replaced only if absent or marker-owned; malformed output paths cannot delete unrelated directories. | VERIFIED | `local.cmake` rejects `;` before its joined commands and rejects an existing directory as `OUT`. Fixture runs 12 and 13 assert the victim sentinel survives and no `outdir.rows` is created. |
| 12 | New test budgets have measurement comments and `title.pal.bios` has at least a 4-second budget. | VERIFIED | Current `tests/compat/tests.cmake` keeps the 4-second PAL BIOS budget with its stated precedent; new registrations retain measurement comments. Full CI passed without a timeout. |
| 13 | Existing budgets were not lowered. | VERIFIED | Compared with the prior report: `title.pal.fast`, `title.pin.recipe`, and `title.local.fixture` retain their budgets; `title.pal.bios` is raised to 4 seconds. |
| 14 | The BankSet diagnostic matrix and tracer use separate output trees. | VERIFIED | `tests/diag/tests.cmake` gives `diag.bankset` the `bankset-matrix` tree while the tracer keeps its own tree; full CI passed. |
| 15 | The full CI/release gate succeeds on the accepted source. | VERIFIED | Fresh current-turn `cmake --workflow --preset ci`: exit 0, 673/673 CI tests and 6/6 release checks. No hosted URL is claimed. |
| 16 | Earlier phase truths remain intact, including pin order, ledger derivation, ratchet/CRC wiring, privacy boundary and plan alignment. | VERIFIED | Current source is unchanged from the acceptance receipt (`git diff --quiet 0e86575..HEAD -- ':!.planning/**'`, exit 0). Full CI and the public library workflow re-exercised the integrated paths. Six explicit edge guards are additionally checked below with their evidence limits retained. |

**Score:** 16/16 carried truths verified (0 present, behavior-unverified).

### 11-12 Acceptance Evidence

| Acceptance item | Current source and evidence | Status |
|---|---|---|
| CR-01: basic sample-token guard | Separate `basic`/`title` match branches; both wrong-token mutations run through the real generator in the ten-case self-test. | VERIFIED |
| CR-02: semicolon `OUT` deletion | Early fixed-message guard; fixture verifies the `victim/keep` sentinel remains. | VERIFIED |
| WR-01: directory `OUT` | Fixed fault before output creation; fixture verifies no scratch tree is created. | VERIFIED |
| WR-02: skipped-link note and missing-file explanation | Shared helper counts unread entries; source controls and documentation cover the missing-file note. The library configure missing-file branch was not forced end to end in this acceptance run. | VERIFIED within stated helper/source-control boundary |
| WR-03: wildcard root | Shared name check rejects `*` and `?` before globbing under both `local` and `library` prefixes. | VERIFIED |
| Full consumers | CI: 673/673 plus 6 release checks; public library preset: 47/47. | VERIFIED |

Plan 11-12’s three flagged prohibition intents were assessed against current code, page and tests per AGENTS.md rule 6. The plan’s pending flags are resolved here with the following explicit evidence:

| Prohibition | Assessment | Evidence |
|---|---|---|
| Pinned metadata or a title-frame hash must not be described as gameplay. | RESOLVED by agent judgment | The page calls the observation “public title screen,” says metadata is not playability and disclaims universal compatibility; wording and freshness controls pass. |
| A backing test must not substantiate a claim when its sample lacks that token. | RESOLVED by test | Current generator guard and both wrong-token controls fail with the claim, test and sample named. |
| Local acceptance must not disclose private paths, names, content or results in committed evidence. | RESOLVED by test and source inspection | Local output is limited to `local:` count/status messages; the public fixture checks secret-name/path/hash absence and output sentinels. No private bytes or results were committed or used in CI. |

These judgments use the project’s agent-owned verification policy; no owner UAT or private-content run is pending.

### Required Artifacts

| Artifact | Expected | Status | Details |
|---|---|---|---|
| `tests/compat/ledger.csv` | One derived row per pinned entry plus separate owned rows | VERIFIED | 204 pinned rows and 49 owned rows; current pin/ledger policy is exercised in CI. |
| `tools/compat_doc.cmake`, `docs/compatibility.md` | Evidence-derived claims page and guards | VERIFIED | Substantive generator, current data derived from the ledger, byte-freshness and mutation controls pass. |
| `tests/cmake/compat_doc_controls.cmake` | Wrong-token controls for title and basic samples | VERIFIED | Wired through `policy.compat_doc_fresh.self_test`; ten cases pass. |
| `tests/compat/local.cmake`, `tests/compat/local_fixture.cmake` | Counted, isolated local pass and public fault fixture | VERIFIED | Wired through `title.local.fixture`; 13 public runs assert outcomes, output secrecy and sentinels. |
| `tests/compat/files.cmake`, `tests/compat/library.cmake` | Shared safe scan for local and library paths | VERIFIED | Included by both consumers; library calls shared scan twice; source controls and library workflow pass. |
| `tests/cmake/compat_ratchet.cmake`, `tests/diag/tests.cmake` | Protected passes and ledger-bound diagnostic evidence | VERIFIED | Existing controls and diagnostic consumers pass in the full CI workflow. |
| `.planning/phases/11-full-library-acceptance/11-12-SUMMARY.md` | Source-identified acceptance receipt | VERIFIED | Exists, records source identity, current acceptance evidence and explicit limits. |

### Key Link Verification

| From | To | Via | Status | Details |
|---|---|---|---|---|
| `tools/softlist_ledger.cmake` | pinned MAME XML and `tests/compat/ledger.csv` | per-entry derivation and check mode | WIRED | `policy.softlist_ledger` runs in the CI hygiene lane; ledger count is 204 pinned entries. |
| `tests/policy/tests.cmake` | `tools/compat_doc.cmake` | freshness and ten-case generator self-test | WIRED | Both registrations ran and passed in the focused command and full CI. |
| `tests/compat/tests.cmake` | `tests/compat/local_fixture.cmake` → `tests/compat/local.cmake` | `-DLOCAL` passed to the fixture; its `drive` function invokes that path | WIRED | Source trace plus `title.local.fixture` behavior evidence. The generic key-link checker does not follow the variable, so this link was checked manually. |
| `tests/compat/library.cmake` | `tests/compat/files.cmake` | include and two `compat_files(library ...)` calls | WIRED | Full public library workflow passed. |
| `tests/diag/tests.cmake` | `tests/diag/ledger_crc.cmake` and ledger rows | canonical diagnostic calls compare mailbox CRC | WIRED | Diagnostic consumers passed in full CI. |

### Data-Flow Trace (Level 4)

| Artifact | Data variable | Source | Produces real data | Status |
|---|---|---|---|---|
| `docs/compatibility.md` | page tables and evidence counts | `tests/compat/ledger.csv` via `tools/compat_doc.cmake` | Yes; regenerated output is compared byte-for-byte by `policy.compat_doc_fresh` | FLOWING |
| `tests/compat/ledger.csv` | pinned population and dispositions | pinned MAME XML through `tools/softlist_ledger.cmake` | Yes; pin identity/count and derived row rules are checked in the hygiene lane | FLOWING |
| `build/library/local.csv` | per-row local result | files in `MUHTARI_ROM_DIR`, identified through the core and evaluated by the predicate/BIOS path | Yes; this turn used public built samples and observed 0/204 present | FLOWING; no private content was supplied |

### Six Existing Edge Guards

| Edge | Evidence and limit |
|---|---|
| COMP-01 adjacency | Generator iterates per `<software>` entry and checks entry identity; the pinned ledger retains separate rows even for equal chip hashes (`hiscore` and `xm`). `policy.softlist_ledger` passed. |
| COMP-01 empty input | `tools/softlist_ledger.cmake` has an explicit fatal guard when no `<software>` blocks are parsed. Prior one-off failure evidence is retained by plan 11-02; no dedicated empty-input CTest is claimed. |
| COMP-01 ordering | `title.pin.order` compares regenerated output with the committed ledger; page freshness also passes. |
| COMP-02 adjacency | `policy.compat_ratchet.controls` retains the “expires today” pass and “expired yesterday” failure; canonical diagnostic CRC binding remains in the four designated cases. Full CI passed. |
| COMP-02 empty input | `tests/diag/ledger_crc.cmake` explicitly fails for a missing ledger row or diagnostic line. Prior one-off failures are retained by plan 11-03; the CRC mismatch self-test is not claimed to exercise these two guards. |
| COMP-02 ordering | Page regeneration/freshness preserves ledger row order and sorted claims; `policy.compat_doc_fresh` passed. |

### Behavioral Spot-Checks

| Behavior | Command | Result | Status |
|---|---|---|---|
| Wrong-token and local-output controls plus docs budget | `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(policy[.]compat_doc_fresh([.]self_test)?|title[.]local[.]fixture|docs[.]budgets)$' --no-tests=error --output-on-failure -V` | Exit 0; 7/7 selected/setup tests passed; ten generator controls and thirteen local fixture runs reported expected completion lines. | PASS |
| Full CI and release checks | `cmake --workflow --preset ci` | Exit 0; 673/673 CI tests and 6/6 release checks passed. | PASS |
| Public library preset | `MUHTARI_ROM_DIR=build/ci/built cmake --workflow --preset library` | Exit 0; 47/47 tests passed over public built samples. | PASS |
| Visible local count | `bash -o pipefail -c "ctest --test-dir build/library -R '^library[.]local$' --no-tests=error -V | grep -E 'local: [0-9]+ of [0-9]+ present'"` | Exit 0; exactly one count line: 0/204 present, 0 pass, 0 fail, 204 absent, commit `0e86575f561b`, tree dirty. | PASS |
| Source drift since acceptance receipt | `git diff --quiet 0e86575..HEAD -- ':!.planning/**'` | Exit 0; no source, test, workflow or generated-page changes after the accepted runs. | PASS |

All commands are local (`N/A-local`); no hosted URL is claimed. No full suite was rerun by this verifier; the fresh same-turn executor results above were reused as authorized. The local count contains no private path or filename.

### Probe Execution

No phase-declared or conventional `scripts/*/tests/probe-*.sh` probes were found. Probe step skipped.

### Test Quality Audit

| Test File | Requirement | Active / skipped | Circular | Assertion strength | Verdict |
|---|---|---:|---|---|---|
| `tests/cmake/compat_doc_controls.cmake` | COMP-01, COMP-02 | 10 / 0 | No; it mutates fixture ledgers and checks independent fault text | Value/fault behavior | PASS |
| `tests/cmake/compat_ratchet_controls.cmake` | COMP-02 | 0 disabled | No; fixture repository exercises ratchet outcomes | Behavioral | PASS |
| `tests/compat/local_fixture.cmake` | COMP-03 | 13 runs / 0 disabled | No; public inputs and fixed expected counts/faults | Behavioral, including cleanup sentinel | PASS |
| `tests/diag/ledger_crc.cmake` | COMP-02 | Registered in the diagnostic workflow / 0 disabled | No | CRC value and missing-input guards | PASS, with the two empty-input guards limited to prior one-off evidence |

Disabled-test scan: 0 matches. Circular expected-output pattern: 0; fixture writes are input construction, with outcomes asserted against fixed expectations. No requirement-linked test is disabled.

### Requirements Coverage

| Requirement | Source plans | Description | Status | Evidence |
|---|---|---|---|---|
| COMP-01 | 11-02, 11-06, 11-07, 11-08, 11-10, 11-12 | Every pinned entry is accounted; metadata is never a gameplay pass. | SATISFIED | 204 pinned rows, 201 unknown / 3 skipped / 0 passes; pin/ledger policy and generated page controls pass. |
| COMP-02 | 11-01, 11-03, 11-06, 11-07, 11-10, 11-11, 11-12 | New claims have admissible public automated evidence; old passing predicates remain protected. | SATISFIED | Owned/public evidence classes, sample-token controls, ratchet/CRC controls and full CI pass. |
| COMP-03 | 11-01, 11-04, 11-05, 11-08, 11-09, 11-11, 11-12 | Private content stays in the local preset; CI has no private-byte prerequisite. | SATISFIED | Public local fixture, public-only 47-test library workflow, 0/204 visible count, and full CI pass. |

All three requirement IDs map to Phase 11 and appear in plans; no orphaned requirement is mapped here.

### Decision Coverage

The decision coverage gate reported 24/24 trackable CONTEXT.md decisions honored, with no unhonored decisions. This gate is non-blocking.

### Anti-Patterns Found

| File | Line | Pattern | Severity | Impact |
|---|---:|---|---|---|
| `tests/compat/local.cmake`, `tools/compat_doc.cmake`, `tests/compat/local_fixture.cmake` | comments only | “placeholder” describes the deliberate `<semicolon>` list-encoding token | Info | Not a stub or TODO; these paths restore the token before writing/evaluation. |

No `TBD`, `FIXME` or `XXX` markers were found in the implementation files covered by this re-verification. The code review disposition retains 8 fixed and 8 optional information findings deferred, with 0 open; four older reused information identities remain documented separately. WR-02’s acceptance boundary and COMP-03’s generic unclassified assumption remain explicit limitations, not new gaps. No additional review round was opened.

### Advisory (New Scope, Unevidenced)

None. The review’s optional items remain deferred; this bounded re-verification found no new blocker or advisory requiring a new gap plan.

### Human Verification Required

N/A — infrastructure/tooling phase with no user-facing interaction. The project policy delegates evidence judgments to the agent. No private content was available or needed for CI acceptance; no owner UAT is pending.

### Gaps Summary

Both carried gaps are closed. The basic-sample claim guard is active and mutation-tested. The local output path guard prevents the reproduced semicolon split from deleting an unrelated directory, and the fixture checks the sentinel. Current CI, release checks, public library consumers, generated-page freshness and local-output controls pass. The remaining scope limits are recorded above and in the accepted receipt; they are not failed phase requirements.

---

_Verified: 2026-10-29T19:16:21Z_
_Verifier: the agent (gsd-verifier)_
