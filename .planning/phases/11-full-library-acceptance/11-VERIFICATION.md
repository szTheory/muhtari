---
phase: 11-full-library-acceptance
verified: 2026-10-29T16:10:00Z
status: gaps_found
score: 9/13 must-haves verified
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
  - "docs/compatibility.md"
  - "tests/cmake/compat_ratchet.cmake"
  - "tests/compat/ledger.csv"
  - "tests/compat/library.cmake"
  - "tests/compat/local.cmake"
  - "tests/compat/tests.cmake"
  - "tools/compat_doc.cmake"
covered_digest: "v4:sha256:4dfc5d3cc6e6590e360140021d5e8c7aafad08447d377a1380b4b547c563077d"
behavior_unverified: 0
overrides_applied: 0
re_verification: false
gaps:
  - truth: "Every new CI test carries a budget derived from comparable measurements with margin (ENGINEERING section 11; plan 11-01 title.pal.bios)"
    status: failed
    reason: "title.pal.bios has TIMEOUT 2, commented 'provisional, not yet measured in CI'. Measured here: 0.56-0.63 s alone, 1.36-1.43 s with 18 concurrent copies on 18 cores, 2.7-3.0 s with 36 concurrent copies. The closest analogue (eightysix, BIOS path, POKEY) already exhausted the same 2 s budget in CI and was raised to 4 s (library.cmake:139-142). The test is in the ci and wasm32 inventories, so it runs in required lanes (scale 1 on ci and ci-msvc) under all-core ctest parallelism."
    artifacts:
      - path: "tests/compat/tests.cmake"
        issue: "line 43-46: TIMEOUT 2 on title.pal.bios (and unmeasured budgets on title.pal.fast, title.pin.order, title.pin.order.self_test, title.pin.recipe, title.local.fixture)"
    missing:
      - "Raise title.pal.bios to TIMEOUT 4 and record the local measurement (command, n, max) in the comment"
      - "Record at least a local measurement beside the other new unmeasured budgets (WR-06)"
  - truth: "Generated compatibility docs match the evidence: a pass row's evidence class names what the predicate shows (11-06 truth, ROADMAP SC4)"
    status: failed
    reason: "tools/compat_doc.cmake labels every passing 7800basic title@ row 'gameplay' and the page publishes 'public gameplay | 27', 'region NTSC | public gameplay', 'board ram | public gameplay', 'peripheral pokey450 | public gameplay'. docs/emulation-tests.md:208 defines title@F=H as 'frame F hashes to H, the title the sample shows by then'; many of the 27 rows are tech demos (fontcheck, colorfade, soundtest, boxcollision, keypadcontrol). A frame hash is not gameplay evidence, and the page's own sentence says metadata is not playability. policy.claims_wording does not deny the word."
    artifacts:
      - path: "tools/compat_doc.cmake"
        issue: "lines 152-163, 253, 296-298: label 'gameplay' / class 'public gameplay' for title-frame-hash passes; prose at ~342-346"
      - path: "docs/compatibility.md"
        issue: "lines 54, 66, 78, 91, 95 repeat the label"
      - path: "tests/cmake/compat_doc_controls.cmake"
        issue: "line 92 expects the 'public gameplay' text"
    missing:
      - "Rename the label and class to what the predicate shows (for example 'public title screen'), update the explanatory paragraph and the control, regenerate docs/compatibility.md"
  - truth: "The claims table lists, for each claim, the tests that exercise it (11-06 truth; page sentence 'Each claim names its evidence class and the tests that exercise it')"
    status: failed
    reason: "board:supergame names title.simple.fast, but simple's ledger board column is 'linear', so that test does not load a SuperGame image (diag.sg144 is the real backing). board:ram and peripheral:pokey450 carry class 'public gameplay' with backing tests 'none' although library.* supergame+ram and pokey450 rows and diag.pal / diag.expansion exercise them. The generator only checks that a named test exists in the inventory, not that it exercises the claim."
    artifacts:
      - path: "tools/compat_doc.cmake"
        issue: "line 34 supergame entry; lines 32 and 44 ram / pokey450 entries with '-' tests"
      - path: "docs/compatibility.md"
        issue: "lines 78, 80, 91 (and 95 for the NTSC row)"
    missing:
      - "Drop title.simple.fast from the supergame entry; name real backing tests for ram and pokey450, or print 'library preset' instead of 'none' for library-only evidence"
  - truth: "The local pass prints ids and counts only; no private path or file name reaches the log (11-04 truth; local.cmake header fault list)"
    status: failed
    reason: "Reproduced: a dangling symlink in DIR makes `cmake ... -P tests/compat/local.cmake` stop with 'file SIZE requested of path that is not readable:' followed by the full path. local.cmake:97-105 and library.cmake:76-96 pass each globbed file to file(SIZE)/file(SHA1) unguarded, so CMake's own error text carries the path. Names with ';' or an unbalanced '[' split or merge list elements (review reproduction, not re-run). The leak is to the owner's own terminal, not committed, and only for pathological directory contents, so the criterion-3 isolation (no private bytes in CI or releases) holds; the stated no-path contract does not."
    artifacts:
      - path: "tests/compat/local.cmake"
        issue: "lines 97-105: unguarded file(SIZE) on globbed paths"
      - path: "tests/compat/library.cmake"
        issue: "lines 76-96: same at configure time"
    missing:
      - "Skip symlink or unreadable entries (count them, do not name them); reject a DIR holding '[', ']' or ';' with a fixed message; add a dangling-symlink case to local_fixture.cmake"
deferred: []
---

# Phase 11: Full-library acceptance Verification Report

**Phase Goal:** Account for the pinned library honestly and prove every new compatibility claim without private bytes in CI.
**Verified:** 2026-10-29T16:10:00Z
**Status:** gaps_found
**Re-verification:** No, initial verification

## Goal Achievement

The core of the phase is real and works: all 204 pinned entries have an explicit ledger row, none is counted as a pass, the ratchet and
mailbox-CRC bindings fail on tampering, the library job is required, and the private-run mechanism works end to end with no private bytes.
The defects are in the details the goal's wording turns on: a published evidence label that overstates what a frame hash shows, a claims
table whose backing-test column is partly wrong, a required-lane test budget with no margin, and a local-run path leak on odd inputs.

### Observable Truths

| # | Truth | Status | Evidence |
|---|-------|--------|----------|
| 1 | SC1: every pinned-library entry has an explicit ledger disposition; untested and unsupported distinguished from passes | VERIFIED | `awk` over `tests/compat/ledger.csv`: 204 rows with no `/` in the id; 201 `unknown local`, 3 `skipped local`, 0 pass, 0 unsupported. `policy.softlist_ledger` (+ rename, slot, board self tests) passed in the hygiene preset, which checks equality against the pinned XML. Page headline reads "204 of 204 ... 0 pass, 0 fail, 0 unsupported, 201 not evaluated, 3 skipped. Not evaluated means untested, not failing." |
| 2 | SC2: new board/controller/region claims have legal automated behavioral evidence; existing passing predicates stay protected | VERIFIED | `diag/absolute`, `diag/activision`, `diag/fractalus` are pr `title@120` rows judged by `library.diag.*`; `diag.retail`, `diag.sg144`, `diag.pal`, `diag.bankset*` passed. `tests/cmake/compat_ratchet.cmake` implements the pr-to-local demotion rule; `policy.compat_ratchet.controls` passed. Tampering the `diag/pal` ledger CRC (52a5 to 0000) makes `diag.pal` fail with "ledger evidence 0000, mailbox CRC 52a5"; ledger restored with `git checkout`. Evidence uses only committed public/owned images. |
| 3 | SC3: private content runs only through the isolated local library preset with command/exit/commit evidence; CI and releases carry no private bytes | VERIFIED | `MUHTARI_ROM_DIR=build/ci/built cmake --workflow --preset library` exit 0; follow-up `ctest --test-dir build/library -R '^library.local$' -V` prints one line "local: 0 of 204 present, 0 pass, 0 fail, 204 absent, commit=a14bead079df tree=clean" with no private path. `.env.example` is gone and no tracked file prescribes a dotenv reader (`git grep`). `hygiene.firmware_scan` and its self test passed. `11-05-SUMMARY.md` records "no private content available" for the agent's run, which is the contract (owner run is optional). |
| 4 | SC4a: host capability metadata matches the evidence; no universal-compatibility implication | VERIFIED | `libretro/muhtari_libretro.info.in`: `is_experimental = "true"`, description ends "No universal title compatibility." and says model conformance only; README line 5 carries the disclaimer; `policy.claims_wording` and self test passed. |
| 5 | SC4b: generated compatibility page matches the evidence (evidence class names what the predicate shows) | FAILED | See gap 2: "public gameplay | 27" for title-frame-hash passes. |
| 6 | Claims table backing-test column exercises each claim | FAILED | See gap 3: supergame names a linear sample's test; ram and pokey450 show "none" with a gameplay class. |
| 7 | Ratchet tier-demotion hole closed; mailbox CRCs bound; library job required (11-03) | VERIFIED | Ratchet source and controls read and run; CRC tamper test above; `.github/workflows/ci.yml:704` has `library` in `CI required` needs, jq key list includes it; `policy.ci_required` and self test passed. |
| 8 | Pin recipe, PAL predicate, banksets row (11-01) | VERIFIED | `title.pin.recipe`, `title.pin.order`, `title.pal.fast`, `title.pal.bios` and `diag/banksets` skipped row (`recipe-does-not-assemble`) present; all passed locally. |
| 9 | Softlist ledger derived and cross-checked against `src/cartdb.c` (11-02) | VERIFIED | `policy.softlist_ledger*` passed; hiscore, xboard, xm are the 3 skipped rows. |
| 10 | Local pass identifies through the core and is proven by a public fixture (11-04), including output limited to ids and counts | FAILED (partial) | `title.local.fixture` passed and the happy-path output is ids and counts only, but see gap 4 for the path leak on dangling symlinks. |
| 11 | Wording guards, README and .info disclaimer, milestone-name statement (11-07) | VERIFIED | `policy.claims_wording(.self_test)` passed; `docs/maintaining.md` states milestone names are labels (per plan; not re-read in full). |
| 12 | Planning docs and seeds aligned (11-08) | VERIFIED | `git grep` finds no dotenv, wrapper-script or `compat-update` prescriptions outside phase history (only a lesson row in `LESSONS.md`). |
| 13 | New CI test budgets are measured and carry margin (ENGINEERING section 11, AGENTS rule 7) | FAILED | See gap 1. Unscaled lanes give `title.pal.bios` 2 s against 0.6 s alone and 1.4 s under 18-way concurrency. |

**Score:** 9/13 truths verified (0 present, behavior-unverified)

### Required Artifacts

| Artifact | Expected | Status | Details |
|----------|----------|--------|---------|
| `tests/compat/ledger.csv` | 204 pinned rows plus 49 owned rows | VERIFIED | 253 data rows; 46 owned pass (42 pr, 4 local mailbox), 3 skipped |
| `tools/softlist_ledger.cmake` | derives pinned rows | VERIFIED | `policy.softlist_ledger` passes against the pinned XML |
| `tests/compat/local.cmake`, `local_fixture.cmake` | counted local pass and public fixture | VERIFIED with gap | works; path leak on odd inputs, `rm -rf` on unchecked OUT (see warnings) |
| `tests/cmake/compat_ratchet.cmake` | tier-demotion rule | VERIFIED | read and exercised |
| `tools/compat_doc.cmake`, `docs/compatibility.md` | generated page | VERIFIED with gap | fresh (`policy.compat_doc_fresh` passed, 25,572 bytes under the 40,960 cap); labels and backing column wrong |
| `libretro/muhtari_libretro.info.in`, `README.md` | disclaimers | VERIFIED | present and guarded |

### Key Link Verification

| From | To | Via | Status | Details |
|------|----|-----|--------|---------|
| diag tests | `tests/compat/ledger.csv` | `diag_ledger_crc` | WIRED | tamper test fails as intended |
| `CI required` | `library` job | needs list + jq keys | WIRED | `ci.yml:704-715` |
| `library.local` | `local.cmake` | `-P` with absolute OUT | WIRED | preset passes `${PROJECT_BINARY_DIR}/local.csv`, so the `/local` hazard is not reachable through the documented command |
| `compat_doc.cmake` | inventory | backing-test existence check | PARTIAL | checks existence only, not exercise |

### Behavioral Spot-Checks

| Behavior | Command | Result | Status |
|----------|---------|--------|--------|
| Phase CI tests (title, policy, diag, docs) | `ctest --test-dir build/ci -R '^(title\.\|policy\.(softlist_ledger\|compat\|claims\|info\|ci_required\|docs)\|docs\.\|diag\.(bankset\|pal\|retail\|sg144)\|hygiene)' -j4` | 25 of 25 passed | PASS |
| Hygiene-lane ledger and CI policy | `ctest --test-dir build/hygiene -R 'softlist\|timeout_comments\|ci_required\|firmware_scan\|ci.classify'` | 12 of 12 passed | PASS |
| Library preset, public bytes | `MUHTARI_ROM_DIR=build/ci/built cmake --workflow --preset library` | exit 0; counts line as above | PASS |
| CRC binding | tampered `diag/pal` evidence, `ctest -R '^diag.pal$'` | Failed with named message; restored | PASS |
| `title.pal.bios` margin | 18 and 36 concurrent copies of the predicate | 1.36-1.43 s and 2.71-3.00 s against a 2 s budget | FAIL |
| `local.cmake` with dangling symlink | `cmake ... -DDIR=<dir with dangling.a78> -P tests/compat/local.cmake` | prints full path in CMake error | FAIL |
| `diag.bankset` / `diag.bankset_tracer` concurrently | 6 runs at `-j2` | all passed (race not reproduced) | PASS |

### Requirements Coverage

| Requirement | Source Plans | Description | Status | Evidence |
|-------------|--------------|-------------|--------|----------|
| COMP-01 | 11-02, 11-06, 11-07, 11-08 | Every pinned entry accounted for, never counting metadata as a pass | SATISFIED | truth 1; the page labelling gap (gap 2) is a wording defect on owned rows, not a pinned-entry accounting defect |
| COMP-02 | 11-01, 11-03, 11-06, 11-07 | New claims have legally admissible automated evidence; ratchet protects prior passes | SATISFIED with gaps | truths 2, 7, 8 hold; claims-table backing (gap 3) and PAL BIOS budget (gap 1) are defects in how that evidence is presented and run |
| COMP-03 | 11-01, 11-04, 11-05, 11-08 | Private cartridges only through the isolated local preset; public CI needs no private bytes | SATISFIED with gap | truth 3 holds; path leak on odd inputs (gap 4) |

All three IDs appear in REQUIREMENTS.md (lines 51-53, traceability 91-93) and in plan frontmatter; none is orphaned. Five SUMMARY files
(11-01, 11-02, 11-03, 11-04, 11-06) carry an empty `requirements-completed:`; the union across summaries still covers all three IDs.

### Anti-Patterns Found

| File | Line | Pattern | Severity | Impact |
|------|------|---------|----------|--------|
| `tests/compat/tests.cmake` | 43-46 | TIMEOUT 2, "provisional, not yet measured" on a required-lane test | Blocker | random `CI required` failure (gap 1) |
| `tools/compat_doc.cmake` | 152-163 | frame-hash pass labelled gameplay | Blocker for SC4 | gap 2 |
| `tests/compat/local.cmake` | 50-58 | `rm -rf` on `${out_dir}/local`, `out_dir` empty for a bare `OUT`, so `/local` (reproduced by derivation: `out_dir=[]`) | Warning | unreachable through the preset (absolute OUT); reachable only by direct `-P` with a bare name as the header invites |
| `tests/diag/tests.cmake` | 874-890 | `diag.bankset` and `diag.bankset_tracer` share one OUT tree, no lock | Warning | pre-existing (949fc61), widened by the new `diag_ledger_crc` reader; not reproduced in 6 trials; already listed in `deferred-items.md` |
| `tests/compat/tests.cmake` | 42, 77, 99, 118, 133 | five more budgets with no measurement | Warning | WR-06 |
| `tools/compat_doc.cmake` | 114, 147, 264 | "N of N accounted for" cannot report a gap | Info | IN-01 |

The remaining review items (IN-02 to IN-08) are not goal-bearing; they are left to the review fix round.

### Review Findings Weighed

| Finding | Verified myself | Verdict against the goal |
|---------|-----------------|--------------------------|
| CR-01 | Yes, measured | Gap 1. A required-lane flake undermines SC2's "automated evidence in CI" and breaks the project's measurement rule; one-line fix. |
| WR-01 | Yes, reproduced with a dangling symlink | Gap 4. Does not break SC3's isolation (no private bytes in CI/releases) but breaks the stated no-path contract. |
| WR-02 | Yes, derivation reproduced | Warning only: documented command passes an absolute OUT. |
| WR-03 | Source confirmed; race not reproduced | Warning, pre-existing, already in `deferred-items.md`. |
| WR-04 | Yes, page and generator read; doc definition of `title@` read | Gap 2. SC4 requires docs that match the evidence. |
| WR-05 | Yes | Gap 3. The `supergame` claim still has real backing (`diag.sg144`), so the claim is supported but the table is misleading; ram and pokey450 evidence exists in the ledger but is shown as "none". |
| WR-06 | Yes | Warning, folded into gap 1. |

### Human Verification Required

None. The owner's optional private-dump run is not a gate (AGENTS rule 6), and hosted CI outcomes are confirmed at ship.

### Gaps Summary

Four gaps, all small and local, and none touching the ledger's accounting or the ratchet. They group into two fixes:
1. `tests/compat/tests.cmake`: raise `title.pal.bios` to 4 s and record measurements beside the other new budgets.
2. `tools/compat_doc.cmake` plus its control and the regenerated page: rename the "public gameplay" class, fix the backing-test column for
   supergame, ram and pokey450.
3. `tests/compat/local.cmake`, `library.cmake`, `local_fixture.cmake`: guard unreadable and symlink entries and add the fixture case.
WR-02 and WR-03 are cheap to fix in the same round (absolute OUT; separate bankset output tree) and are recommended there, but are not
recorded as goal gaps. Per AGENTS rule 7 this is the single gap round; no new evidence is needed to plan it beyond this report.

---

_Verified: 2026-10-29T16:10:00Z_
_Verifier: Claude (gsd-verifier)_
