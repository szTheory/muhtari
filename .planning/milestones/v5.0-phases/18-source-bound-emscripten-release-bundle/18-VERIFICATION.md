---
phase: 18-source-bound-emscripten-release-bundle
verified: 2026-11-11T14:54:27Z
status: passed
score: 6/6 must-haves verified
covered_files:
  - .github/workflows/be-equality.yml
  - .github/workflows/ci.yml
  - .github/workflows/nightly.yml
  - .github/workflows/release.yml
  - .planning/milestones/v5.0-phases/18-source-bound-emscripten-release-bundle/18-01-PLAN.md
  - .planning/milestones/v5.0-phases/18-source-bound-emscripten-release-bundle/18-01-SUMMARY.md
  - .planning/milestones/v5.0-phases/18-source-bound-emscripten-release-bundle/18-02-PLAN.md
  - .planning/milestones/v5.0-phases/18-source-bound-emscripten-release-bundle/18-02-SUMMARY.md
  - .planning/milestones/v5.0-phases/18-source-bound-emscripten-release-bundle/18-CONTEXT.md
  - .planning/milestones/v5.0-phases/18-source-bound-emscripten-release-bundle/18-LICENSE-RESEARCH.md
  - .planning/milestones/v5.0-phases/18-source-bound-emscripten-release-bundle/18-RESEARCH.md
  - .planning/milestones/v5.0-phases/18-source-bound-emscripten-release-bundle/18-REVIEW-DISPOSITION.md
  - .planning/milestones/v5.0-phases/18-source-bound-emscripten-release-bundle/18-REVIEW-FIX-2.md
  - .planning/milestones/v5.0-phases/18-source-bound-emscripten-release-bundle/18-REVIEW-FIX.md
  - .planning/milestones/v5.0-phases/18-source-bound-emscripten-release-bundle/18-REVIEW.md
  - .planning/milestones/v5.0-phases/18-source-bound-emscripten-release-bundle/deferred-items.md
  - PROVENANCE.md
  - README.md
  - THIRD-PARTY-NOTICES.md
  - docs/emscripten-hosting.md
  - docs/libretro.md
  - docs/testing.md
  - scripts/emscripten.sh
  - scripts/publish.sh
  - scripts/ubuntu-apt.sh
  - tests/cmake/install_line.cmake
  - tests/emscripten/bundle-members.txt
  - tests/emscripten/bundle.cmake
  - tests/emscripten/tests.cmake
  - tests/hygiene/tests.cmake
  - tests/inventory/ci.txt
  - tests/inventory/emscripten.txt
  - tests/policy/publish.cmake
  - tests/policy/tests.cmake
  - tests/policy/ubuntu_apt.sh
covered_digest: "v4:sha256:c237a7db47b88b41efabe60e8a42776997e97e045208df673fac4eefa7879fb7"
behavior_unverified: 0
overrides_applied: 0
decision_coverage:
  honored: 7
  total: 7
  not_honored: []
---

# Phase 18: Source-bound Emscripten release bundle — Verification Report

**Phase Goal:** Host integrators have a source-bound, permissively distributable Emscripten libretro ZIP that works from clean extraction and is locally ready for the existing release pipeline.
**Verified:** 2026-11-11T06:12:26Z
**Status:** passed
**Re-verification:** No — initial verification

## Goal Achievement

### Observable Truths

| # | Truth | Status | Evidence |
|---|---|---|---|
| 1 | An integrator can extract one architecture-independent ZIP with the matched loader/Wasm, metadata, source and SDK identities, pair digests, host contract and notices, with only allowed members. | ✓ VERIFIED | `build/emscripten/muhtari-libretro-emscripten.zip` passes `unzip -t`; the read-only archive assertion matched all 14 entries against `tests/emscripten/bundle-members.txt`, verified commit `f400a76ee9e66fbd37e709a8628f1e6486899c22`, sidecar/source identity, both SHA-256 entries and all five pinned SDK notice hashes. Exact allowlisting excludes ROM, BIOS, fixture, save and measurement files. |
| 2 | The existing offline lifecycle and independent native numeric oracle pass from a clean extraction, and representative faulty packages are rejected. | ✓ VERIFIED | The final Emscripten receipt at the product snapshot has 4/4 JUnit cases and zero skips. `emscripten.bundle.controls` reports clean-extracted Node/native success plus rejection of missing loader/sidecar, changed source identity, member/checksum errors, duplicate/unsafe entries, symlink escapes and five altered notices. `tests/emscripten/bundle.cmake` invokes `host.mjs` with the extracted loader path; the consumer derives adjacent Wasm and source-ID paths. |
| 3 | Maintainers can trace generated runtime/stdlib distribution rights and notices to the pinned SDK inputs, and the ZIP carries required permissive notices and provenance. | ✓ VERIFIED | `18-LICENSE-RESEARCH.md` identifies the exact Emscripten 7.0.0 image/source and five notice files; `bundle.cmake` pins their digests and rejects changes; `PROVENANCE.md` and `THIRD-PARTY-NOTICES.md` map source paths, terms and hashes. The five notice bytes in the archive match those qualified digests. |
| 4 | Existing local release checks bind the ZIP to the release source and cover the 19-ZIP/20-asset set while preserving native archives and rejection behavior. | ✓ VERIFIED | `scripts/publish.sh` adds the portable ZIP to its single sorted asset iterator and validates it against `EXPECTED_SHA` before checksums and firmware scanning; the existing six native commit receipts remain separate. CI uploads the ZIP only from Linux x64, and release workflows collect `archives-*`. Final local receipts: `python3 build/ship-local-gate.py` passed (native 692/692; release 6/6, no skips); production publisher controls accepted the real ZIP at f400 and rejected all five qualified-notice mutations. No rehearsal or publication was run. |
| 5 | Integrators have reproducible download/extraction/loading instructions, adjacent companion resolution, and clear offline-evidence limits. | ✓ VERIFIED | `README.md` links to the version-free archive and host guide. `docs/libretro.md` gives download/extraction commands, keeps the loader, Wasm and source sidecar together, shows the Node factory with adjacent Wasm bytes, and states that Node/native results do not qualify a real browser, device, performance or physical input. The download prose describes the portable release layout without claiming a hosted outcome from local checks. |
| 6 | Phase work preserves the locked local-only boundary and existing deferred requirements. | ✓ VERIFIED | The changed product files contain no core/ABI/state behavior or browser/device implementation. Phase records retain the nine deferred requirements and prior host/remote stops. The saved final checks are local; no public rehearsal, publish, push, merge, remote dispatch or actual host/device check occurred. |

**Score:** 6/6 truths verified (0 present, behavior-unverified)

### Required Artifacts

| Artifact | Expected | Status | Details |
|---|---|---|---|
| `tests/emscripten/bundle-members.txt` | Exact archive allowlist | ✓ VERIFIED | Matches the actual 14-member ZIP exactly. |
| `tests/emscripten/bundle.cmake` | Create, validate, extract, consume and fault controls | ✓ VERIFIED | Checks paths/member types and bounds, identities, SDK notices, pair hashes and extracted consumer; called by producer, tests and publisher. |
| `scripts/emscripten.sh` | Existing pinned producer with packaging | ✓ VERIFIED | Existing Linux x64 lane invokes packaging and the no-skip inventory check. |
| `docs/emscripten-hosting.md` | Packaged host contract | ✓ VERIFIED | Included as `HOSTING.md` in the artifact. |
| `tests/emscripten/tests.cmake` | Recurring package and fault tests | ✓ VERIFIED | Registers artifact, controls, bundle and bundle-controls cases. |
| `scripts/publish.sh` | Portable ZIP in exact-source release set | ✓ VERIFIED | One asset iterator preserves 18 native archives and validates the portable manifest before checksums. |
| `tests/policy/publish.cmake` | Offline 19-ZIP/20-asset and real-bundle controls | ✓ VERIFIED | Exercises publisher prepare faults and the production archive/notice branch. |
| `.github/workflows/ci.yml` | Linux x64 ZIP and sidecar evidence upload | ✓ VERIFIED | Uploads `archives-emscripten` and separately includes the source-ID sidecar in evidence. |
| `.github/workflows/release.yml` | Existing release artifact collection | ✓ VERIFIED | The release jobs download the `archives-*` pattern that includes the new artifact. |
| `docs/libretro.md` | Download, extraction and host instructions | ✓ VERIFIED | Paths and companion resolution match the packaged layout; evidence limits are explicit. |

### Key Link Verification

| From | To | Via | Status | Details |
|---|---|---|---|---|
| `scripts/emscripten.sh` | `tests/emscripten/bundle.cmake` | Pinned producer runs package checker | WIRED | Producer invokes the CMake checker after building in its existing lane. |
| `tests/emscripten/bundle.cmake` | `tests/emscripten/host.mjs` | Fresh extracted loader plus native oracle | WIRED | Consumer command uses `${extracted}/cores/muhtari_libretro.mjs`; Wasm and sidecar are resolved adjacent to it. |
| `tests/emscripten/tests.cmake` | `tests/inventory/emscripten.txt` | Registered cases and no-skip JUnit inventory | WIRED | All four registered case names occur in the inventory; producer passes that inventory to `check_junit.cmake`; final JUnit has 4 cases and zero skips. |
| `.github/workflows/ci.yml` | `.github/workflows/release.yml` | `archives-emscripten` upload and `archives-*` download | WIRED | Artifact names and collection pattern match. |
| `scripts/publish.sh` | `tests/emscripten/bundle.cmake` | `prepare` validates against `EXPECTED_SHA` | WIRED | Production CMake invocation precedes the generic 19-ZIP firmware scan and checksum creation. |
| `scripts/publish.sh` | `tests/policy/publish.cmake` | Offline publisher and production-archive controls | WIRED | The policy test calls publisher `prepare`; its production branch uses the actual SDK ZIP and checks notice mutations through the unshimmed validator. |

### Data-Flow Trace (Level 4)

| Artifact | Data | Source | Produces Real Data | Status |
|---|---|---|---|---|
| Portable ZIP | Loader, Wasm, source identity, metadata, digests and notices | Pinned SDK build outputs, current core source identity and qualified SDK notice files | Yes; archive bytes, identities and hashes were inspected | ✓ FLOWING |

### Behavioral Spot-Checks

| Behavior | Command / receipt | Result | Status |
|---|---|---|---|
| Archive is readable and matches the declared package | `unzip -t build/emscripten/muhtari-libretro-emscripten.zip`; read-only manifest/identity/hash assertion | Exit 0; exact 14 members, source and pair digests matched; all five notice hashes matched | ✓ PASS |
| Extracted lifecycle and negative package controls | Saved `build/emscripten/junit.xml` and `build/emscripten/Testing/Temporary/LastTest.log` from `sh scripts/emscripten.sh build/ci` | 4/4 passed, zero skipped; log contains expected rejections and clean-extracted Node/native success | ✓ PASS |
| Native and release-contract regression coverage | Saved `build/ci/junit.xml` and `build/ci/junit-release.xml` from `python3 build/ship-local-gate.py` | 692/692 native and 6/6 release tests passed, zero skipped | ✓ PASS |
| Real archive through publisher prepare | `tests/policy/publish.cmake` production-archive branch with expected commit `f400a76ee9e66fbd37e709a8628f1e6486899c22` | Saved final local receipt passed; exact-source ZIP accepted and all five qualified-notice mutations rejected | ✓ PASS |

### Probe Execution

No phase probe was declared; the recurring producer, package and publisher checks are CTest/CMake controls recorded above.

### Requirements Coverage

| Requirement | Source Plan | Description | Status | Evidence |
|---|---|---|---|---|
| WASM-05 | 18-01 | Source-bound architecture-independent ZIP, exact contents, identities, hashes and notices | ✓ SATISFIED | Actual ZIP matches 14-member manifest; identities and hashes verified. |
| WASM-06 | 18-01 | Clean-extraction lifecycle and independent native parity, with representative rejection controls | ✓ SATISFIED | Final 4/4 Emscripten JUnit and extracted-consumer/control log. |
| WASM-07 | 18-01 | Exact pinned SDK rights and required notice/provenance payload | ✓ SATISFIED | Five-source research, pinned digest checks, archive hash assertion and provenance docs. |
| WASM-08 | 18-02 | Exact-source existing release contract for 19 ZIPs/20 assets, preserving native checks | ✓ SATISFIED | Publisher iterator/validator wiring and final native, release and real-archive publisher receipts. |
| WASM-09 | 18-02 | Reproducible download, extraction and loading instructions with evidence limits | ✓ SATISFIED | README link and `docs/libretro.md` instructions match packaged companions and explicitly limit claims. |

All five requirements mapped to Phase 18 are covered; no orphaned Phase 18 requirement was found. The seven trackable CONTEXT decisions were honored.

### Anti-Patterns Found

| File | Line | Pattern | Severity | Impact |
|---|---:|---|---|---|
| `tests/cmake/install_line.cmake` | 39–60, 81–82 | WR-02: checker does not ignore inline-code spans or tilde-fenced examples | Warning, optional tooling seed | Current README has the required clickable links; missing, wrong-target, plain-text and backtick-fenced controls pass. This is recorded in `deferred-items.md`; the two review-fix rounds are complete. |

No unreferenced `TBD`, `FIXME` or `XXX` markers were found in the changed product files. The earlier native notice-policy and SDK notice-control failures are retained in the ignored evidence directory and summaries; their causes were fixed, and the final source snapshot passed the corresponding controls.

### Human Verification Required

None. All required checks are local and `human_judgment: false`; real browser and device behavior is expressly outside this phase and is not claimed by the documentation.

### Gaps Summary

No blocking gaps remain. The validated local artifact is bound to product snapshot `f400a76ee9e66fbd37e709a8628f1e6486899c22`; later commits changed planning/summary metadata only. The archive has not been published, and this report establishes local release readiness only. WR-02 remains a non-blocking optional checker seed.

---

_Verified: 2026-11-11T06:12:26Z_
_Verifier: the agent (gsd-verifier)_

## Shipping-source local recheck (2026-11-11)

The first hosted candidate exposed two local-readiness defects hidden by a warm SDK tree and POSIX-only fixture bytes. The bounded ship correction keeps production notice hashes and physical-containment validation intact: callers now use distinct direct BUILD children, and synthetic notices use explicit CRLF with hashes derived from packed file bytes. A fresh SDK reproduction failed before the work-path correction and the clean pinned workflow now passes all four cases without skips. The Windows notice error was reproduced locally with CRLF (old string hash exit 1); the file-byte hash passes the full publisher control, including strict production rejection of synthetic notices. The registered native publisher test passes within its unchanged deadline, and the real SDK archive plus all five notice mutations pass the production publisher branch. All local commands exit 0 after correction; URLs N/A-local.

Only `tests/emscripten/tests.cmake` and the synthetic-fixture block of `tests/emscripten/bundle.cmake` changed. The other covered artifacts and six local must-haves remain valid. The existing CLI fingerprint was regenerated after rechecking these changes. [The ship receipt](../../v5.0-SHIP.md) preserves original local timing failures, first hosted failures/cancellations, source distinctions and pending hosted gates. Local acceptance remains passed; no hosted, rehearsal, merge or release pass is inferred.

### Artifact-transfer follow-up (2026-11-11)

The actual downloaded rehearsal artifact lacks the independently compared libretro metadata file, despite producer-workspace SDK/publisher passes. The uploader now includes that file. The native publisher fixture follows the actual upload declaration; deleting the metadata entry reproduces the missing-file failure, and the corrected registered control passes within its unchanged deadline. A prospective corrected downloaded layout passes exact-source package validation and clean-extracted Node/native parity. Existing required checks and runtime/notice/source guards are unchanged. The inline verifier rechecked the changed workflow and publisher fixture and refreshed this local fingerprint. These working-tree checks do not establish a hosted upload or rehearsal pass; see the ship receipt for the separate runner DNS stop and failed history.


### Authorized hosted-package recovery (2026-11-11)

The isolated hosted diagnostic reproduces the actual Ubuntu-owned DNS failure while an alternate kernel.org mirror authenticates all four existing Resolute suites and installs real Valgrind. The production helper changes only the existing image mirror-list order; source suites, components, Ubuntu keyring and required package names remain intact. Native offline pass/fault controls exercise the actual list transformation, exact package arguments and failure propagation. The original setup fails those controls (exit 1); the corrected registered test and a qualified real Ubuntu 26.04 container install pass (exit 0). The independent operations review found no blocking issue after checking the actual source stanzas and helper.

`python3 build/ship-local-gate.py` passes the full native and release workflows, exit 0 (423.81 seconds native). Both canonical JUnit inventory consumers pass, exit 0, confirming all expected tests exactly once without skips. Shellcheck, workflow actionlint (only its unsupported current runner labels ignored), document budgets and `git diff --check` pass, exit 0. No emulator, ABI, state, bundle, SDK, publisher or native release implementation changes are part of this recovery. The inline verifier rechecked the affected workflow/helper/test/document fields and adds those covered inputs through the selected runtime fingerprint command. This preserves local readiness; exact-source hosted CI, rehearsal, publication and cleanup remain separate gates documented in the ship receipt. The earlier failed attempts and unrelated host stops remain unchanged.


### Native-only policy registration follow-up (2026-11-11)

The hosted candidate passes the package route, all native current/base checks and full PowerPC equality, but the legacy WASI configure detects the new native-only control outside its smaller inventory. The same local configure reproduces the extra-registration error (exit 1). Moving that control into the existing CI-inventory block preserves its native execution and restores both exact registration graphs (exit 0); no inventory or runtime case is dropped. Download prose now avoids stale pre-publication status wording. Focused native routing/wording/budget controls pass. The selected runtime refreshes the covered-input digest after these checks. The old candidate's required aggregate pass is not a passing rehearsal or acceptance of this corrected source; full source/run outcomes remain in the ship receipt.


The follow-up also passes the complete local WASI portable execution (617 cases in 182.57 seconds) and its exact label-specific JUnit inventory, exit 0 without skips. Native routing and wording/budget controls remain passed. This closes the locally reproduced registration defect; hosted corrected-source acceptance is still required.
