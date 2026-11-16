---
phase: 25-publish-the-corrected-souper-mapper
verified: 2026-11-16T09:26:30Z
status: passed
score: 5/5 must-haves verified
covered_files:
  - .planning/phases/25-publish-the-corrected-souper-mapper/25-01-PLAN.md
  - .planning/phases/25-publish-the-corrected-souper-mapper/25-01-SUMMARY.md
  - .planning/phases/25-publish-the-corrected-souper-mapper/25-RELEASE.md
  - .planning/phases/25-publish-the-corrected-souper-mapper/25-REVIEW-DISPOSITION.md
  - .planning/phases/25-publish-the-corrected-souper-mapper/25-REVIEW.md
  - .release-please-manifest.json
  - CHANGELOG.md
  - README.md
  - docs/status.md
  - version.txt
covered_digest: "v4:sha256:7971a0e126c3218a47d148e0cb3983cd0d7753696f6dddcd36facf25c1ace3f9"
behavior_unverified: 0
overrides_applied: 0
decision_coverage:
  honored: 10
  total: 10
  not_honored: []
---

# Phase 25: Publish the corrected Souper mapper — Verification Report

**Phase Goal:** Publish the corrected mainline source with Souper support and leave the public README and status page accurate.
**Verified:** 2026-11-16T09:26:30Z
**Status:** passed
**Re-verification:** No — initial verification

## Goal Achievement

### Observable Truths

| # | Truth | Status | Evidence |
|---|---|---|---|
| 1 | A fresh stable v0.16.0 release was made from corrected mainline, and the prior v0.15.0 draft and tag stayed unchanged. | VERIFIED | `gh release view v0.16.0` and its tag ref identify source `a0319a9c29886d776e37b818d38d36fe810f486d`; `bb70dcf` is its ancestor (exit 0). PR [#71](https://github.com/szTheory/muhtari/pull/71) used the feature trigger; release-please PR [#72](https://github.com/szTheory/muhtari/pull/72) produced the ordinary 0.16.0 release commit. `cmp build/phase25/v015-before.json build/phase25/v015-after-final.json` exited 0; both snapshots have SHA-256 `4aa0ea4e2d5090918e6d4c03c60bc8dd8efa7090bba050846f802e7266e2c9ad`. The live v0.15.0 release remains draft ID 445523668 with its original direct tag target and no assets. |
| 2 | The established release workflow passed its required gates on the exact release source, including publication and installation through the latest alias. | VERIFIED | [Release run 43801428444](https://github.com/szTheory/muhtari/actions/runs/43801428444) is successful on `a0319a9c`. `release-ci / CI required`, `release-equality / equality`, `publish`, and `install-line-latest` all succeeded. The live release is stable and latest, with 19 ZIPs plus nonempty `SHA256SUMS` (20 assets). |
| 3 | Souper generated-fixture checks and the library, runner, and libretro packages were built from that same release source. | VERIFIED | The six native package source receipts, native and PowerPC equality receipts, and Emscripten source receipt all contain `a0319a9c`; native/PowerPC hash files compare equal (exit 0). Saved release JUnit contains passing `core.state.souper`, `diag.souper_tracer`, and `emscripten.souper` cases. The release asset list contains all three deliverables across the supported archives. |
| 4 | Public docs now accurately describe the published package, generated-fixture evidence, compatibility limits, and scoped completion standard in version-neutral language. | VERIFIED | Current [README.md](../../../README.md) and [status page](../../../docs/status.md) identify the latest package and host-supplied devices, distinguish generated checks from retail-game evidence, retain the 204-title status (201 untested, 3 skipped), identify missing GameDrive/BupChip and nine unresolved frontend requirements, and define scoped completion. Neither page hard-codes v0.15.0 or v0.16.0. [Docs PR CI 43813049138](https://github.com/szTheory/muhtari/actions/runs/43813049138) passed all 20 jobs. |
| 5 | Release and CI queue, duration, critical path, cancellation, and skip evidence was reviewed, with no unsupported pipeline change; final main CI is green. | VERIFIED | The receipt records measured runs and a bounded no-change decision. Final [main CI 43818074765](https://github.com/szTheory/muhtari/actions/runs/43818074765) passed on exact docs merge `f3e1a24d8a1372bfc689c19cb1d91dfad0575d97`: 18 jobs passed, only expected push-event `title` and `perf` skips, with no failed, cancelled, or superseded jobs. |

**Score:** 5/5 truths verified (0 present, behavior-unverified)

### Required Artifacts

| Artifact | Expected | Status | Details |
|---|---|---|---|
| `.planning/phases/25-publish-the-corrected-souper-mapper/25-RELEASE.md` | Exact source, release preservation, tests, timing, and final-main evidence | VERIFIED | `verify.artifacts` passed all 3 declared artifacts. The receipt separates release source `a0319a9c` from final docs main `f3e1a24d`. |
| `README.md` | Concise product overview and accurate latest-package instructions | VERIFIED | Latest link is stable; install line is unchanged and was exercised by `install-line-latest`. |
| `docs/status.md` | Current availability, limitations, and completion standard | VERIFIED | Content matches the linked release receipt and finite compatibility evidence. |

### Key Link Verification

| From | To | Via | Status | Details |
|---|---|---|---|---|
| Feature delivery PR squash subject | `release-please-config.json` | Feature title triggers normal pre-1.0 minor bump | VERIFIED (manual) | The static key-link check treated the descriptive PR subject as a path. Read-only PR evidence confirms #71 merged with the `feat:` title; the checked-in release config enables minor pre-major bumps and #72 generated 0.16.0 without an override. |
| `.github/workflows/release.yml` | `scripts/publish.sh` | Release source passes archive, provenance, publish, and verify gates | VERIFIED | Static key-link check passed; run 43801428444 confirms the `publish` and exact-tag verification path succeeded on `a0319a9c`. |
| `README.md` | `.github/workflows/release.yml` | Existing install command is tested against latest after publication | VERIFIED (manual) | The static checker did not resolve this runtime link. The release workflow's `install-line-latest` job supplies the README to `tests/cmake/install_line.cmake`; that job passed on the published release. The command line is unchanged in the docs merge. |

`verify.key-links` automatically matched 1 of 3 links; the other two are prose-to-workflow relationships, not file-to-file references. Both were confirmed from the merged PRs, workflow source, and successful exact-source run above. They are not implementation gaps.

### Data-Flow Trace (Level 4)

Not applicable. This phase published existing core behavior and changed documentation and release metadata; it added no dynamic UI or data-rendering path.

### Behavioral Spot-Checks

| Behavior | Command / evidence | Result | Status |
|---|---|---|---|
| Public package downloads and installs through the README's latest URL | Release run 43801428444, `publish` and `install-line-latest` jobs | Both succeeded; 20 release assets are visible on the stable release | PASS |
| Docs remain valid on final main | Main CI run 43818074765, exact head `f3e1a24d` | Required aggregate succeeded; expected skips only | PASS |

No new test suite or owner-host run was needed; the exact release and final-main CI runs already exercised the relevant installation and integration paths.

### Probe Execution

Not applicable. The phase plan declares no probes or PASS-marker checks, and no `scripts/**/tests/probe-*.sh` file was present.

### Requirements Coverage

| Requirement | Source Plan | Description | Status | Evidence |
|---|---|---|---|---|
| TUNE-13 | `25-01-PLAN.md` | Review CI timing and retain coverage while recording a safe adjustment or bounded no-change result | SATISFIED | `25-RELEASE.md` timing review covers the critical path, queue, skips, and cancellations; runs 43801428444, 43813049138, and 43818074765 are source-qualified. |
| REL-01 | `25-01-PLAN.md` | Publish fresh v0.16.0 from corrected mainline and preserve v0.15.0 | SATISFIED | Live release/tag refs and the before/after snapshot comparison show the required identities. |
| REL-02 | `25-01-PLAN.md` | Pass exact-source release CI, fixture, integrity, provenance, publish, and latest-install checks | SATISFIED | Release run 43801428444 passed on `a0319a9c`; its required release, equality, publisher, and install jobs succeeded. |
| DOC-01 | `25-01-PLAN.md` | Explain deliverables, availability, limits, and scoped completion | SATISFIED | README/status content and docs PR CI 43813049138 plus final main CI 43818074765. |

All requirement IDs in the plan are accounted for in `REQUIREMENTS.md`; no additional Phase 25 requirement was orphaned.

### Test Quality Audit

| Test File | Linked Req | Active | Skipped | Circular | Assertion Level | Verdict |
|---|---|---:|---:|---|---|---|
| `tests/core/test_state.c` (`core.state.souper`) | REL-02 | Release-source JUnit | 0 matching skip markers | No | Behavioral and value checks on state, mappings, continuation, and refusal | PASS |
| `tests/diag/tests.cmake`, `tests/diag/driver.c`, `tests/diag/expected.csv` (`diag.souper_tracer`) | REL-02 | Release-source JUnit | 0 matching skip markers | No | Fixed expected mapping, mailbox, and pixel values with fault controls | PASS |
| `tests/emscripten/tests.cmake`, `tests/emscripten/souper.cmake` (`emscripten.souper`) | REL-02 | Release-source JUnit | 0 matching skip markers | No | Behavior plus native/Wasm parity; native literal-oracle tests remain the correctness anchor | PASS |
| `tests/release/tests.cmake` | REL-02 | Release-source release inventory | 0 | No | Archive contents, firmware, and release-floor checks | PASS |
| `tests/cmake/install_line.cmake` | REL-02, DOC-01 | `install-line-latest` job | 0 | No | End-to-end execution of the README install line | PASS |
| `tests/cmake/single_version.cmake`, `tests/cmake/budgets.cmake`, `tests/cmake/claims_wording.cmake` | DOC-01 | Docs checks and PR CI | 0 matching skip markers | No | Policy and value checks | PASS |
| `tests/policy/quality_fixture_identity.cmake` | REL-02 | Release CI quality job | 0 matching skip markers | No | Exact fixture pin/digest acceptance and forged identity rejection | PASS |
| No test file is expected for the release timing analysis itself | TUNE-13 | Not applicable | Not applicable | Not applicable | Run data and bounded maintenance judgment | PASS |

The native expected mapper rows are fixed in the checked-in diagnostic oracle rather than generated from emulator output. The Emscripten comparison is corroborating parity evidence, not the sole correctness oracle. The requirement-linked test scan found no disabled test annotations, circular expected-value generation, or insufficient-assertion finding.

**Disabled tests on requirements:** 0 → no blocker  
**Circular patterns detected:** 0 → no blocker  
**Insufficient assertions:** 0 → none

### Anti-Patterns Found

| File | Line | Pattern | Severity | Impact |
|---|---:|---|---|---|
| `README.md` | 3–5 | Opening repeats the deliverables summary | Info | Dispositioned as deferred: the README remains accurate and concise, so the copy edit can wait until the overview is next revised. |

No unresolved TODO, FIXME, XXX, HACK, placeholder, or empty-implementation marker was found in the phase's public docs and receipt.

### Decision Coverage

The decision-coverage gate reported 10 of 10 trackable context decisions honored, with no unhonored decisions. The gate is non-blocking and did not change the verdict.

### Human Verification Required

None. The published release, latest installation, and integration checks have exact-source automated evidence; no owner-host or hardware judgment is required.

### Gaps Summary

No blocking gaps remain. The phase goal and all five roadmap success criteria are verified. The sole informational review note is dispositioned as deferred with a reason; it does not affect acceptance.

---

_Verified: 2026-11-16T09:26:30Z_
_Verifier: the agent (gsd-verifier)_
