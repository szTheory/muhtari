---
phase: 11-full-library-acceptance
verified: 2026-10-29T18:10:00Z
status: gaps_found
score: 14/16 must-haves verified
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
covered_digest: "v4:sha256:fc6b3ab1435cc213d32fe757e085d6382104559e6d8cb4d914304799f723b6c2"
behavior_unverified: 0
overrides_applied: 0
re_verification:
  previous_status: gaps_found
  previous_score: 9/13
  gaps_closed:
    - "title.pal.bios budget (4 s, with measurement) and measured comments on the other new budgets"
    - "Generated page names title-frame passes 'public title screen', not gameplay"
    - "Claims table: supergame, ram and pokey450 name tests that load a sample carrying the claim; no 'none' beside a non-none class"
    - "Local pass and library scans: dangling and valid links, unreadable entries, ';' and unmatched '[' in file or directory names no longer put a path in the log"
  gaps_remaining:
    - "Sample-carries-token guard is dead for basic.* tests (review CR-01)"
    - "A ';' in OUT makes local.cmake delete an unrelated directory (review CR-02)"
  regressions: []
gaps:
  - truth: "The generator fails, naming the claim, the test and the sample, when a claim's sample-backed test (basic.<sample>, title.<sample>.fast or title.<sample>.bios) runs a sample whose ledger row lacks the claim's token, so the mislabelled backing test cannot return (11-10 truth 3)"
    status: partial
    reason: "Reproduced. In tools/compat_doc.cmake:322-337 the guard is `if(test MATCHES \"^basic[.](.+)$\" OR test MATCHES \"^title[.](.+)[.](fast|bios)$\")` followed by `set(sample \"${CMAKE_MATCH_1}\")`. CMake evaluates both operands; when the first matches and the second does not, the second clears CMAKE_MATCH_1. A scratch script gave `basic.rmtramdemo -> []` and `title.simple.fast -> [simple]`. With a copy of the ledger in which 7800basic-0.41/rmtramdemo is changed from supergame+ram to linear, `cmake -DLEDGER=<copy> ... -P tools/compat_doc.cmake` exits 0 although rmtramdemo backs both the ram and the supergame claims. The guard works only for title.<sample>.fast|bios, which is the only half the new control (case 9, `sample`) exercises. The page content is correct today (the backing tests do load samples carrying the claim), so no published claim is wrong; the regression guard the plan promised is missing for the four basic.* backing tests (rmtramdemo, rmtramcompressiondemo, pokey, rmtromdemo) that were added to back the supergame, ram and pokey450 claims."
    artifacts:
      - path: "tools/compat_doc.cmake"
        issue: "lines 322-337: CMAKE_MATCH_1 read after an OR of two regex matches; empty for every basic.* test, so `DEFINED sample_${kind}_${sample}` is false and the check is skipped"
      - path: "tests/cmake/compat_doc_controls.cmake"
        issue: "lines 126-133: case 9 mutates simple (a title.* backing test) only; no case mutates a basic.*-backed sample"
    missing:
      - "Match basic.<sample> and title.<sample>.(fast|bios) in separate if/elseif branches, taking CMAKE_MATCH_1 inside each branch"
      - "Add a control that changes rmtramdemo's board from supergame+ram to linear and expects the 'whose sample rmtramdemo does not carry' failure"
  - truth: "A relative or odd OUT never makes the local pass delete a directory it did not create; it replaces <absolute OUT>.rows only when that entry is absent or holds the pass's own marker (11-09 truth 4)"
    status: partial
    reason: "Reproduced. tests/compat/local.cmake:67-74 builds the steps as semicolon-joined strings ('rm;-rf;${out_rows}') and expands them unquoted. OUT is not name-checked. With -DOUT='<tmp>/victim;x/out.csv' the EXISTS guard passes (the full string is not a path), the step expands to `rm -rf <tmp>/victim` and `make_directory x/out.csv.rows`, and the file `victim/keep` was deleted, with CMake's own error text then naming the path. The same run created an untracked `x/` directory in the repository root (removed during this verification). Reachability is narrow: the library preset passes `${PROJECT_BINARY_DIR}/local.csv`, so only a direct `-P` call with a ';' in OUT, or a build directory containing ';', triggers it. Data loss is limited to what the invoking user could delete anyway, and no private bytes are exposed, so COMP-03's isolation holds. The plan's own wording (never deletes a directory it did not create) does not."
    artifacts:
      - path: "tests/compat/local.cmake"
        issue: "lines 67-74: string-joined steps split on ';' in OUT; no compat_name_check on OUT; no fixture case for ';' in OUT"
    missing:
      - "Reject '[', ']' and ';' in OUT with a fixed message before any step (compat_name_check) and run each step as its own execute_process with quoted arguments"
      - "Add a local_fixture case with ';' in OUT that expects the fixed fault and checks the sibling directory is untouched"
deferred: []
---

# Phase 11: Full-library acceptance Verification Report

**Phase Goal:** Account for the pinned library honestly and prove every new compatibility claim without private bytes in CI.
**Verified:** 2026-10-29T18:10:00Z
**Status:** gaps_found
**Re-verification:** Yes, after gap plans 11-09, 11-10 and 11-11. Every must-have was re-checked, not only the four earlier gaps.

## Goal Achievement

All four earlier gaps are closed in substance. The pinned library is accounted for (204 of 204 rows, none a pass), the claims are backed
by public or owned bytes, the private pass keeps paths out of the log for every input shape the earlier report named, and the page labels
what the predicate shows. Two defects from the gap-round review remain and fail stated must-haves of the gap plans: a dead guard for
basic.* tests (CR-01) and a `;`-in-OUT deletion hazard (CR-02). Both were reproduced here. Neither makes a published claim wrong or
exposes private bytes in CI, which is why the score is high and the remaining fix is small, but they are must-have failures, not advice.

### Observable Truths

Roadmap success criteria first (the contract), then the gap-plan truths.

| # | Truth | Status | Evidence |
|---|-------|--------|----------|
| 1 | SC1: every pinned entry has an explicit ledger disposition; untested and unsupported are distinguished from passes | VERIFIED | `awk` over `tests/compat/ledger.csv`: 204 rows without `/`, 201 `unknown`, 3 `skipped`, 0 pass. 49 owned rows: 42 pass/pr, 4 pass/local, 3 skipped/local. `ctest --test-dir build/hygiene -R 'softlist\|timeout_comments\|ci_required\|firmware_scan\|ci.classify'` 12 of 12 passed. |
| 2 | SC2: new board/controller/region claims have legal automated evidence; existing passes stay protected | VERIFIED | `ctest --test-dir build/ci -R '^(title\.\|policy\.(softlist_ledger\|compat\|claims\|info\|ci_required\|docs)\|docs\.\|diag\.(bankset\|pal\|retail\|sg144)\|hygiene)' -j6`: 25 of 25 passed, including `diag.retail`, `diag.sg144`, `diag.pal`, `diag.bankset`, `diag.bankset_tracer`, `policy.compat_ratchet.controls`. The ratchet and CRC binding were tamper-tested in the first verification (unchanged source since). |
| 3 | SC3: private content runs only through the isolated local library preset; CI and releases carry no private bytes | VERIFIED | `MUHTARI_ROM_DIR=build/ci/built cmake --workflow --preset library` exit 0, 47 of 47 library tests passed; `library.local` prints `local: 0 files unread (links or unreadable)` and `local: 0 of 204 present, 0 pass, 0 fail, 204 absent, commit=9ddf504e1f76 tree=clean`. `library` is in `CI required` needs and its jq key list (`.github/workflows/ci.yml:704,715`). No tracked ROM/BIOS bytes beyond the manifest (`hygiene.firmware_scan` passed). |
| 4 | SC4a: host capability metadata matches the evidence; no universal-compatibility implication | VERIFIED | `policy.claims_wording`, `.self_test`, `policy.info*` passed; `.info` and README disclaimers read in the first verification (files unchanged). |
| 5 | SC4b: the generated page matches the evidence; the evidence class names what the predicate shows | VERIFIED | `grep -i gameplay docs/compatibility.md tools/compat_doc.cmake` finds only "not gameplay" disclaimers and the project-level sentence at line 10. Evidence-kind table reads "public title screen \| 27"; claims table uses "public title screen". `policy.compat_doc_fresh` and `.self_test` passed; `tools/compat_doc.cmake` regenerated page is byte-fresh. |
| 6 | Claims table lists, per claim, tests that exercise it (11-06, 11-10 truth 2) | VERIFIED | supergame: `diag.sg144, basic.rmtramdemo` (rmtramdemo ledger board `supergame+ram`); ram: `basic.rmtramdemo, basic.rmtramcompressiondemo` (both `supergame+ram`); pokey450: `basic.pokey, basic.rmtromdemo, diag.pal` (samples carry `pokey450`). All four basic.* tests exist in `tests/inventory/ci.txt`, are registered by `tests/built/tests.cmake:57` and passed here (4 of 4). `title.simple.fast` is no longer under supergame. Only claims with class `none` print `none`. |
| 7 | 11-10 truth 3: generator fails naming claim, test and sample when a sample-backed test's sample lacks the token | FAILED (partial) | Works for `title.<sample>.fast\|bios` (control case 9 passes). Dead for `basic.<sample>`: reproduced exit 0 on a ledger with rmtramdemo set to linear. See gap 1 (review CR-01). |
| 8 | 11-10 truth 4: freshness test passes on the regenerated page under 40,960 bytes; self test has nine cases | VERIFIED | `policy.compat_doc_fresh` passed; `docs/compatibility.md` size under cap; `compat_doc_controls.cmake` runs the nine cases (kinds label, sample, and the rest). |
| 9 | 11-09 truths 1-3: no path or name reaches the log for dangling or valid links, unreadable entries, `;` or unmatched `[` in file names, bad directory names; balanced `[!]` read normally | VERIFIED | Ran `local.cmake` on a directory holding a dangling link, a link to /etc/hosts and `simple [!].a78`: output `local: 2 files unread (links or unreadable)` and the counts line, exit 0, no path. With an added `a;b.a78` it stops with `local: a file name holds an unmatched bracket or a semicolon`. `title.local.fixture` passed (1.09 s). |
| 10 | 11-09 truth 5: library preset scans use the shared file rule, guarded by a source check | VERIFIED | `tests/compat/library.cmake:73-86` calls `compat_files(library ...)` twice; library workflow passed; `title.local.fixture` (which carries the source check and fault copies) passed. |
| 11 | 11-09 truth 4: `<OUT>.rows` is replaced only when absent or marker-owned; a foreign entry stops the pass | FAILED (partial) | Holds for a plain absolute or relative OUT (fixture), but a ';' in OUT bypasses it. See gap 2 (review CR-02). |
| 12 | 11-11 truth 1: new CI test budgets measured with margin; `title.pal.bios` at least 4 s with reason; others carry measurement comments | VERIFIED | `tests/compat/tests.cmake:43-52`: `TIMEOUT 4` with the eightysix precedent; title.pal.fast, title.pin.order(.self_test), title.pin.recipe, title.local.fixture and policy.compat_doc_fresh.self_test each carry a `provisional:` measurement comment. Earlier measurement of this predicate: 0.6 s alone, 1.4 s under 18-way concurrency, 3.0 s at 36-way, so 4 s holds. `title.pal.bios` ran 0.53 s here. Comments cite a local macOS run and an unreferenced "full ci run" (review IN-02, advisory). |
| 13 | 11-11 truth 2: no budget lowered | VERIFIED | `title.pal.fast` kept at 2, `title.pin.recipe` kept at 3, `title.local.fixture` kept at 20; `title.pal.bios` raised 2 to 4. |
| 14 | 11-11 truth 3: bankset tests write separate trees | VERIFIED | `tests/diag/tests.cmake` sets `bankset-matrix` for the matrix script; `diag.bankset` and `diag.bankset_tracer` passed together at `-j6`; `deferred-items.md` records the resolution. |
| 15 | 11-11 truth 4: `cmake --workflow --preset ci` passes with 11-09 and 11-10 in place | VERIFIED | Orchestrator evidence: `cmake --workflow --preset ci` at fc51344 exited 0 (673/673 then 6/6 package tests), macOS arm64, no run URL. `git diff --stat fc51344..HEAD -- . ':!.planning'` is empty, so the code under test is unchanged. |
| 16 | Plans 11-01..11-08 truths (pin recipe, PAL predicate, ledger derivation, ratchet, local driver, docs, wording guards, planning alignment) | VERIFIED | Covered by the passing test sets above; sources unchanged except by the gap round, whose effects were re-tested. |

**Score:** 14/16 must-haves verified (0 present, behavior-unverified)

### Required Artifacts

| Artifact | Expected | Status | Details |
|----------|----------|--------|---------|
| `tests/compat/ledger.csv` | 204 pinned rows plus 49 owned rows | VERIFIED | counts above |
| `tests/compat/files.cmake` | shared file rule, `-P` entry | VERIFIED with warnings | links/unreadable skipped, `;` `[` rejected; `*` and `?` in a root name not rejected (WR-03) |
| `tests/compat/local.cmake`, `local_fixture.cmake` | counted pass, public fixture | PARTIAL | works; `;` in OUT deletes a foreign directory (gap 2); OUT that is a directory leaks its path (WR-01) |
| `tests/compat/library.cmake` | configure-time scans through the shared rule | VERIFIED | WR-02: skipped links not mentioned in the missing-file error |
| `tools/compat_doc.cmake`, `docs/compatibility.md` | generated page, claim guard | PARTIAL | page correct and fresh; sample guard dead for `basic.*` (gap 1) |
| `tests/cmake/compat_doc_controls.cmake` | nine controls | VERIFIED with gap | case 9 exercises only the working half of the guard |
| `tests/compat/tests.cmake`, `tests/policy/tests.cmake` | measured budgets | VERIFIED | |
| `tests/diag/tests.cmake` | separate bankset trees | VERIFIED | |

### Key Link Verification

| From | To | Via | Status | Details |
|------|----|-----|--------|---------|
| local.cmake | files.cmake | `include`, `compat_files(local ...)` | WIRED | line 114 in the failing-name stack trace |
| library.cmake | files.cmake | `compat_files(library ...)` x2 | WIRED | lines 73, 86 |
| local_fixture.cmake | library.cmake | source check | WIRED | `title.local.fixture` passes |
| compat_doc.cmake | ledger.csv sample rows | token check on backing tests | PARTIAL | live for `title.*`, dead for `basic.*` |
| CI required | library job | needs list + jq keys | WIRED | `ci.yml:704-715` |
| diag tests | ledger.csv | `diag_ledger_crc` | WIRED | `diag.pal` passed |

### Data-Flow Trace (Level 4)

Not applicable: no dynamic rendering. The generated page is derived from the ledger by the generator and compared byte for byte by `policy.compat_doc_fresh`.

### Behavioral Spot-Checks

| Behavior | Command | Result | Status |
|----------|---------|--------|--------|
| Phase CI tests | `ctest --test-dir build/ci -R '^(title\.\|policy\.(softlist_ledger\|compat\|claims\|info\|ci_required\|docs)\|docs\.\|diag\.(bankset\|pal\|retail\|sg144)\|hygiene)' -j6` | 25 of 25 passed | PASS |
| Hygiene ledger/CI policy | `ctest --test-dir build/hygiene -R 'softlist\|timeout_comments\|ci_required\|firmware_scan\|ci.classify'` | 12 of 12 passed | PASS |
| Backing samples | `ctest --test-dir build/ci -R '^basic\.(rmtramdemo\|rmtramcompressiondemo\|pokey\|rmtromdemo)$'` | 4 of 4 passed | PASS |
| Library preset | `MUHTARI_ROM_DIR=build/ci/built cmake --workflow --preset library` | exit 0, 47 of 47 | PASS |
| Link/odd-name handling | `cmake -DDIR=<dir with dangling+valid link, simple [!].a78> ... -P tests/compat/local.cmake` | counts only, exit 0 | PASS |
| Odd file name | same with `a;b.a78` added | fixed message, no path | PASS |
| basic.* sample guard | `cmake -DLEDGER=<rmtramdemo set to linear> -DEXPECTED=tests/diag/expected.csv -DINVENTORY=tests/inventory/ci.txt -DOUT=<tmp> -P tools/compat_doc.cmake` | exit 0 (should fail) | FAIL |
| `;` in OUT | `cmake ... -DOUT='<tmp>/victim;x/out.csv' -P tests/compat/local.cmake` | `victim/keep` deleted; untracked `x/` created in repo root | FAIL |
| OUT is a directory | `-DOUT=<existing dir>` | CMake error naming the full path (WR-01) | FAIL (warning) |
| Root name with `*` | `cmake -DPREFIX=x -DDIR='<g>/a*' -P tests/compat/files.cmake` with sibling `ab/` | `2 files kept` (reads outside DIR; WR-03) | FAIL (warning) |

### Probe Execution

No probes declared by the phase plans; step skipped.

### Requirements Coverage

| Requirement | Source Plans | Description | Status | Evidence |
|-------------|--------------|-------------|--------|----------|
| COMP-01 | 11-02, 11-06, 11-07, 11-08, 11-10 | Every pinned entry accounted for; metadata never counted as a pass | SATISFIED | truth 1; truth 5 (label no longer says gameplay) |
| COMP-02 | 11-01, 11-03, 11-06, 11-07, 11-10, 11-11 | New claims have legal automated evidence; ratchet protects passes | SATISFIED, with the guard gap | truths 2, 6, 12; the regression guard for basic.* backing tests is missing (gap 1) |
| COMP-03 | 11-01, 11-04, 11-05, 11-08, 11-09, 11-11 | Private cartridges only through the isolated local preset; CI needs no private bytes | SATISFIED, with the OUT gap | truths 3, 9, 10; `;`-in-OUT hazard (gap 2) |

All three IDs appear in REQUIREMENTS.md (lines 51-53, traceability 91-93) and in plan frontmatter; none is orphaned.

### Anti-Patterns Found

| File | Line | Pattern | Severity | Impact |
|------|------|---------|----------|--------|
| `tools/compat_doc.cmake` | 322-337 | `OR` of two regex matches then `CMAKE_MATCH_1` | Blocker for 11-10 truth 3 | gap 1 |
| `tests/compat/local.cmake` | 67-74 | string-joined, unquoted `rm -rf` steps; OUT not name-checked | Blocker for 11-09 truth 4 (narrow reach) | gap 2 |
| `tests/compat/local.cmake` | 64, 193 | OUT that is a directory passes the probe, then CMake error names the path | Warning | WR-01 |
| `tests/compat/library.cmake` | 73-86 | skipped-link count discarded; message and docs silent | Warning | WR-02, IN-04 |
| `tests/compat/files.cmake` | 20-38 | `*` and `?` not rejected in root names | Warning | WR-03 |
| `tests/compat/local_fixture.cmake` | 293-324 | `file (GLOB ...)` with a space bypasses the source check | Info | IN-01 |
| `tests/compat/tests.cmake`, `tests/policy/tests.cmake` | various | budgets cite a local run and an unreferenced "full ci run" | Info | IN-02 |
| `files.cmake`, `local.cmake` | 62, 114 | bare image-maximum literal; missing DIR exits 0 with all rows absent | Info | IN-03 |
| debt markers | n/a | `grep -nE "TBD\|FIXME\|XXX"` on the phase's changed files found none | none | |

### Review Findings Weighed

| Finding | Reproduced | Verdict against the goal |
|---------|------------|--------------------------|
| CR-01 | Yes (scratch script and full generator run) | Fails 11-10 truth 3. Gap 1. Published page content is correct today. |
| CR-02 | Yes (`victim/keep` deleted, `x/` created in repo root) | Fails 11-09 truth 4 in a pathological case. Gap 2. Not reachable through the documented preset. |
| WR-01 | Yes | Warning: leaks the caller's own OUT path, not private content. |
| WR-02 | Source read | Warning: diagnostics and docs only. |
| WR-03 | Yes (`2 files kept`) | Warning: scan reads files beside the named directory when a root name holds `*`. |
| IN-01..IN-04 | Source read | Info; left to the review fix round. |

### Human Verification Required

None. Per project rule 6 the owner verifies nothing by default: every item above was run by the agent. Hosted CI outcomes are confirmed at ship.

### Gaps Summary

Two gaps, both small and both in files the gap round touched. Neither touches the ledger accounting, the ratchet, the library job, the
page's published claims, or the budgets.

1. `tools/compat_doc.cmake`: separate the two regex matches so `basic.*` backing tests are checked, and add a control that mutates a
   `basic.*`-backed sample. Fix text is in the review (CR-01).
2. `tests/compat/local.cmake`: validate OUT with `compat_name_check` and run the `rm`/`make_directory`/`touch` steps as separate quoted
   `execute_process` calls; add a `;`-in-OUT fixture case (CR-02). WR-01 (directory OUT) is the same lines and is cheap to fix with it.

Per rule 7 the phase has had its one gap round, and these two are violations of must-haves that round declared, with new reproduction
evidence in this report. Whether to close them in a review fix round, or accept them with an explicit override (`overrides:` entries for
11-10 truth 3 and 11-09 truth 4), is the maintainer's decision; the verifier does not suggest an override for CR-01 because no alternative
implementation achieves the same intent.

---

_Verified: 2026-10-29T18:10:00Z_
_Verifier: Claude (gsd-verifier)_
