---
phase: 20-complete-ym2151-homebrew-sound
verified: 2026-11-13T16:08:51Z
status: passed
score: 5/5 goal truths verified
covered_files:
  - ".planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-01-PLAN.md"
  - ".planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-01-SUMMARY.md"
  - ".planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-02-PLAN.md"
  - ".planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-02-SUMMARY.md"
  - ".planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-03-PLAN.md"
  - ".planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-03-SUMMARY.md"
  - ".planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-04-PLAN.md"
  - ".planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-04-SUMMARY.md"
  - ".planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-05-PLAN.md"
  - ".planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-05-SUMMARY.md"
  - ".planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-06-PLAN.md"
  - ".planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-06-SUMMARY.md"
  - ".planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-07-PLAN.md"
  - ".planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-07-SUMMARY.md"
  - ".planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-COST-EVIDENCE.md"
  - ".planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-REVIEW-DISPOSITION.md"
  - ".planning/milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-REVIEW.md"
  - "build/v6-ym-sanitize-apple/runner/muhtari-run"
  - "build/v6-ym-sanitize-apple/tests/muhtari-test-state"
  - "build/v6-ym-sanitize-apple/tests/muhtari-test-ym2151"
  - "docs/embedding.md"
  - "docs/hardware/ym2151.md"
  - "scripts/emscripten.sh"
  - "src/bus.c"
  - "src/cart.c"
  - "src/internal.h"
  - "src/state.c"
  - "src/synth.c"
  - "src/ym2151.c"
  - "tests/audio/test_ym2151.c"
  - "tests/audio/ym_oracle.cmake"
  - "tests/audio/ym_oracle.cpp"
  - "tests/core/state_compat.c"
  - "tests/core/state_compat.cmake"
  - "tests/core/test_state.c"
  - "tests/emscripten/bundle.cmake"
  - "tests/emscripten/host.mjs"
  - "tests/release/ym_consumers.cmake"
covered_digest: "v4:sha256:2d8075e5e0ff7c09d86f793ba7cd7e2226ae101e0d3f9536275fa922e6ac7266"
behavior_unverified: 0
overrides_applied: 0
decision_coverage:
  honored: 16
  total: 16
  not_honored: []
---

# Phase 20: Complete YM2151 homebrew sound Verification Report

**Phase Goal:** A host can load one documented XM/YM2151 profile and obtain deterministic FM sound with complete lifecycle/state behavior through every existing deliverable and portable/package consumer.
**Verified:** 2026-11-13T16:08:51Z
**Status:** passed
**Re-verification:** No — the prior report had no \`gaps:\` section. This refresh reran the archived contract against current sources because its fingerprint inputs had changed.

## Goal Achievement

### Observable Truths

| # | Truth | Status | Evidence |
|---|---|---|---|
| 1 | A host can load the qualified selected or header-autodetected XM/YM profile, observe qualified ports, clock, busy/status, timers and IRQ, and reject unsupported requests without mutating the active machine. | ✓ VERIFIED | \`src/cart.c\` routes only the qualified ports and D7 gate to the per-instance chip; \`src/bus.c\` schedules timer deadlines and samples the YM IRQ. The registered \`ym.profile\`, \`ym.clock\`, \`ym.irq\`, cart and invalid-reload tests ran in the recorded final native gate in [20-COST-EVIDENCE.md](20-COST-EVIDENCE.md). The archived profile doc distinguishes digital-source evidence from physical XM timing. |
| 2 | The owned workload produces independently checked, non-silent bounded FM and deterministic mono/canonical audio while preserving no-YM output and duplicated-mono libretro behavior. | ✓ VERIFIED | \`src/ym2151.c\` emits signed channel values into \`src/synth.c\`; the real frame path mixes queued sources. The recorded exact-pinned BSD-3 seed oracle, numeric YM tests, native/Node consumers and legacy audio/equality gates passed for the identified product snapshot. Their limits and source identities are stated in the cost evidence. |
| 3 | YM reset, isolation, complete state continuation and transactional refusal preserve deterministic output, state and battery data; observer replay after restore does not retain abandoned future events. | ✓ VERIFIED | The current implementation validates then applies state and calls \`muhtari__synth_observe_reset\`; the current regression compares event counts, sequence hash, source masks, per-channel counts and polarity totals after replay. After rebuilding from current source, \`ym.observer\`, \`core.state.equality_report\` and \`core.state.ym\` each passed as individually named tests (exit 0). The recorded genuine old/current ten-case two-way compatibility matrix and refusal controls are separately identified in the cost evidence. |
| 4 | The same owned diagnostic reaches the public C library, runner, loaded libretro, emitted Wasm and clean installed/extracted package consumers, with numeric judgments before cross-route equality and integrity controls retained. | ✓ VERIFIED | CTest fixture wiring generates the shared cartridge; the runner and loaded-module tests consume that fixture; \`scripts/emscripten.sh\` passes actual native receipts to the Node host and extracted bundle; release tests run the clean extracted library, runner and core. The recorded full local native/portable/package gates passed on their stated source snapshot. The current \`tests/release/ym_consumers.cmake\` change passes the configured emulator to cross-compiled consumers. Release run [43441765548](https://github.com/szTheory/muhtari/actions/runs/43441765548) passed on the shipped source, including latest-install; the release has 19 ZIPs plus \`SHA256SUMS\` (20 assets), matching \`scripts/publish.sh\`. PR #61 is merged and main CI passed on its merge commit; see the [v6.0 ship receipt](../../v6.0-SHIP.md). |
| 5 | Integrators can identify the supported request, compatibility policy and evidence limits; maintainers can inspect actual ordinary/adverse cost, extents, headroom and justified finite bounds. | ✓ VERIFIED | Read \`docs/hardware/ym2151.md\`, \`docs/embedding.md\` and [20-COST-EVIDENCE.md](20-COST-EVIDENCE.md) directly. The cost artifact binds actual native/Node runs, workloads, regions and artifact identities; structural limits are separated from timings. Its local full-gate results are explicitly bound to product commit \`daab4c82\`; the post-restore observer behavior has the three passing targeted tests above. No retail, analog, physical-audibility or handheld-power claim is made. |

**Score:** 5/5 goal truths verified (0 present, behavior-unverified)

## Required Artifacts

| Artifact | Expected | Status | Details |
|---|---|---|---|
| \`src/cart.c\`, \`src/bus.c\`, \`src/ym2151.c\` | Qualified XM topology, timed YM engine and bus-owned IRQ | ✓ VERIFIED | Substantive register/timer/output paths are called through the current cart and bus owners and exercised by registered profile, clock and IRQ tests. |
| \`src/synth.c\` | Signed YM observation, bounded mix and state-restore observer reset | ✓ VERIFIED | YM outputs enter sources 10/11; queued source changes render into frame PCM. Restore drops observer carry and synchronizes its raw cache. |
| \`src/state.c\`, \`tests/core/test_state.c\`, \`tests/core/state_compat.c\` | Complete YM continuation, validation-before-apply and old/current compatibility | ✓ VERIFIED | Current YM state and equality-report tests passed; the recorded independent old/current matrix and fault controls passed. |
| \`tests/audio/test_ym2151.c\`, \`tests/audio/ym_oracle.cmake\`, \`tests/audio/ym_oracle.cpp\` | Owned diagnostics, transition regressions and receipt-bound exact-seed comparison | ✓ VERIFIED | Current observer regression passes; the recorded numeric and actual pinned-object oracle gates passed. The oracle remains test-only. |
| \`tests/libretro/host.c\`, \`tests/libretro/observe.c\`, \`tests/emscripten/host.mjs\`, \`tests/emscripten/bundle.cmake\`, \`tests/release/ym_consumers.cmake\` | Actual loaded module, emitted portable artifact and clean extracted consumers | ✓ VERIFIED | Registered consumers use the generated fixture, native receipts and extracted artifact paths; archived local gates and the accurately attributed owner release receipt are recorded above. |
| \`docs/hardware/ym2151.md\`, \`docs/embedding.md\`, \`20-COST-EVIDENCE.md\` | Profile, compatibility, measured costs and evidence limits | ✓ VERIFIED | Read directly. Plan 20-07's declared cost-evidence path still points to the pre-archive live-phase directory; the substantive artifact exists at this phase's archived path and is included in the fingerprint. |

Plan artifact queries passed 5/5, 3/3, 3/3, 4/4, 3/3 and 3/3 for plans 20-01 through 20-06. Plan 20-07 returned 3/4 only because its artifact path was not rewritten during archive relocation; direct inspection confirms the required report at the archived path above. This is a stale path in the plan, not a missing implementation artifact.

## Key Link Verification

| From | To | Via | Status | Details |
|---|---|---|---|---|
| \`tests/cart/images.c\` | \`src/cart.c\` → \`src/bus.c\` → \`src/ym2151.c\` | Generated CPU program writes YM ports through real frame execution | ✓ WIRED | The shared fixture is registered as \`ym.fixture\`; cart read/write dispatch calls the YM handlers, while bus timed events and IRQ sampling use YM deadlines and IRQ state. |
| \`src/ym2151.c\` | \`src/synth.c\` → \`src/frame.c\` | Signed source changes feed the integer mixer and public frame buffer | ✓ WIRED | The chip reports both channels through the synth raw-source API; synth render drains PCM for frame output. |
| \`tests/audio/test_ym2151.c\` | \`src/state.c\` → \`src/synth.c\` | Public restore applies validated state and resets observer carry/cache | ✓ WIRED | \`ym.observer\` exercises a saved baseline, abandoned future, restore and exact replay summary; state tests cover YM validation and equality. |
| \`tests/audio/tests.cmake\` | \`tests/audio/test_ym2151.c\` | Registered shared fixture, public runner and observer checks | ✓ WIRED | The fixture setup is a dependency of the downstream consumers and current named observer test. |
| \`tests/libretro/host.c\` | Built \`libretro\` module | Host resolves and invokes exported functions from the actual module | ✓ WIRED | Existing loaded-module test registration targets the built core; recorded native gate executed the YM route. |
| \`tests/libretro/observe.c\` | \`tests/emscripten/host.mjs\` | Native source-bound receipts feed portable numeric judgments | ✓ WIRED | \`scripts/emscripten.sh\` passes the YM native and adverse receipts to Node report/verify modes. |
| \`scripts/emscripten.sh\` / \`tests/emscripten/bundle.cmake\` | Extracted loader/Wasm → \`host.mjs\` | Production artifact is packaged, extracted and executed with receipt inputs | ✓ WIRED | Bundle commands invoke the Node host on the extracted pair; the portable final gate recorded exit 0. |
| \`tests/consumer/main.c\` | \`tests/release/ym_consumers.cmake\` | Public consumer is built against the extracted archive | ✓ WIRED | Release CMake configures the extracted package, runs the consumer, extracted runner and loaded core; emulator forwarding is explicit in the current helper. |
| \`tests/core/state_compat.cmake\` | Genuine kickoff and current libraries; \`tests/core/test_state.c\` | Separate public-only drivers produce and compare states in both directions | ✓ WIRED | The helper checks old-source/header/library provenance, builds distinct drivers and exercises the independent matrix plus controls. |
| \`docs/testing.md\` | \`tests/core/state_compat.cmake\` | Documented compatibility command | ✓ WIRED | The docs identify the real helper command and its genuine old-source prerequisites. |

The generic key-link query uses literal target-text matching and reports false negatives for CMake composition, public API dispatch and library paths passed as arguments. The connections above were traced through those call sites and registrations; none is an orphaned artifact.

## Data-Flow Trace (Level 4)

| Artifact | Data Variable | Source | Produces Real Data | Status |
|---|---|---|---|---|
| \`src/ym2151.c\` → \`src/synth.c\` | YM output → signed sources 10/11 | Register-driven OPM clocks, channel outputs and actual timer/bus schedule | Yes | ✓ FLOWING |
| \`src/synth.c\` → \`src/frame.c\` | Queued sources → frame PCM/count | Integer mixer/filter over TIA, POKEY and YM source levels | Yes | ✓ FLOWING |
| \`libretro/libretro.c\` | Rendered mono frame | Current library frame buffer delivered by callback or duplicated-mono batch | Yes | ✓ FLOWING |
| \`tests/emscripten/host.mjs\` | Native and portable observation reports | Actual native observer and emitted Wasm on the named generated workload | Yes | ✓ FLOWING |

## Behavioral Spot-Checks

| Behavior | Command / evidence | Result | Status |
|---|---|---|---|
| Observer carry/cache after state rewind | \`cmake --build build/v6-ym-sanitize-apple --target muhtari-test-ym2151 muhtari-test-state --parallel 2\`; then \`ctest --test-dir build/v6-ym-sanitize-apple --output-on-failure --no-tests=error -R '^ym[.]observer$'\` | Rebuilt from current sources; exit 0, 1/1. | ✓ PASS |
| State equality report after restore | \`ctest --test-dir build/v6-ym-sanitize-apple --output-on-failure --no-tests=error -R '^core[.]state[.]equality_report$'\` | Exit 0, 1/1. | ✓ PASS |
| YM semantic state and refusal behavior | \`ctest --test-dir build/v6-ym-sanitize-apple --output-on-failure --no-tests=error -R '^core[.]state[.]ym$'\` | Exit 0, 1/1. | ✓ PASS |
| Required complete local gates | Exact commands and source identity in [20-COST-EVIDENCE.md](20-COST-EVIDENCE.md) | Recorded exit 0 for complete native CI/packages, exact-seed oracle, genuine old/current compatibility and controls, qualified sanitizer focus, emitted Wasm/package path, quality producer and hygiene on the stated product snapshot. These were not rerun for this freshness refresh. | ✓ PASS (recorded) |
| Broad supplementary runner hash/equality matrix | \`runner.hashes_file\` in the local Apple sanitizer build | Timed out at its configured 286-second CTest bound (exit 8); retained as incomplete and not represented as a pass or product assertion failure. | ? INCOMPLETE |
| Shipped release workflow and install receipt | \`gh run view 43441765548\`, \`gh release view v0.14.0\`, \`gh pr view 61\` and \`gh run view 43427086666\` — exit 0; see [ship receipt](../../v6.0-SHIP.md) | Release/publish and latest-install passed; release has 19 ZIP archives plus SHA256SUMS; PR #61 merged; main CI passed on the merge commit. | ✓ PASS (queried) |

## Probe Execution

Not applicable. No phase-declared probe path or conventional \`scripts/*/tests/probe-*.sh\` file was found.

## Test Quality Audit

| Test File | Linked Req | Active | Skipped | Circular | Assertion Level | Verdict |
|---|---|---:|---:|---:|---|---|
| \`tests/audio/test_ym2151.c\` | YM-01–04 | Yes | 0 | No | Numeric values, lifecycle/state transitions, exact replay sequence | ✓ ADEQUATE |
| \`tests/audio/ym_oracle.cpp\`, \`tests/audio/ym_oracle.cmake\` | YM-02 | Yes | 0 | No | Receipt-bound actual C-vs-pinned-seed sample/status/IRQ comparison | ✓ ADEQUATE |
| \`tests/core/test_state.c\` | YM-05 | Yes | 0 | No | Repaired-integrity semantic faults, equality report and unchanged state/battery | ✓ ADEQUATE |
| \`tests/core/state_compat.c\`, \`tests/core/state_compat.cmake\` | YM-05 | Yes | 0 | No | Independent old/current bytes and bidirectional multi-frame continuation | ✓ ADEQUATE |
| \`tests/runner/tests.cmake\`, \`tests/libretro/host.c\`, \`tests/libretro/observe.c\` | YM-06 | Yes | 0 | No | Actual process/module execution and numeric output judgments | ✓ ADEQUATE |
| \`tests/emscripten/host.mjs\`, \`tests/emscripten/bundle.cmake\`, \`tests/release/ym_consumers.cmake\` | YM-07 | Yes | 0 | No | Actual emitted/extracted artifacts, identity checks and fault controls | ✓ ADEQUATE |
| \`tests/audio/test_ym2151.c\`, \`tests/emscripten/host.mjs\` | YM-08 | Yes | 0 | No | Complete timing windows, workload/source identities and extent/bound judgments | ✓ ADEQUATE |

**Disabled tests on requirements:** 0. **Circular expected-value patterns:** 0; test outputs are reports/fixtures, while exact parity uses the separately receipt-bound seed and manual numeric expectations. **Insufficient assertions:** 0.

## Requirements Coverage

| Requirement | Source Plan | Description | Status | Evidence |
|---|---|---|---|---|
| YM-01 | 20-01, 20-02, 20-06 | Selected/autodetected profile, qualified ports/timing/IRQ and transactional refusal | ✓ SATISFIED | Current cart/bus/chip path; recorded profile, clock, IRQ and refusal tests in the final native gate. |
| YM-02 | 20-01, 20-07 | Integer C17 OPM, non-silent FM, exact pinned-seed parity and BSD-3 notices | ✓ SATISFIED | Current C engine and notices; recorded numeric tests and actual receipt-bound oracle pass. |
| YM-03 | 20-01–20-07 | Deterministic bounded mono/canonical audio, signed carry/mix and retained no-YM behavior | ✓ SATISFIED | Current observer/state targeted tests; recorded audio, equality and native/portable consumers. |
| YM-04 | 20-02–20-05, 20-07 | Reset/reload/isolation, finite work, no late allocation and bounded output | ✓ SATISFIED | Recorded lifecycle/hostile and native/Wasm callback/refusal gates. |
| YM-05 | 20-03, 20-04, 20-05, 20-06, 20-07 | Complete YM state, transactional refusal and genuine legacy compatibility | ✓ SATISFIED | Current state/observer named tests; recorded genuine ten-case two-direction compatibility matrix and fault controls. |
| YM-06 | 20-04, 20-05, 20-07 | Same diagnostic through public C, runner and loaded libretro | ✓ SATISFIED | Actual process/module routes and numeric judgments in recorded native gate. |
| YM-07 | 20-04, 20-05, 20-07 | Actual portable artifact and clean extracted/installed package consumers | ✓ SATISFIED | Recorded emitted/extracted Wasm and package gates; the hosted latest-install check passed in release run 43441765548. |
| YM-08 | 20-01–20-07 | Supported profile/evidence limits, measured cost, extents and finite bounds | ✓ SATISFIED | Direct profile/embedding/cost evidence and source-identified native/Wasm measurements. |

The archived requirements file maps YM-01 through YM-08 once each to Phase 20. No Phase 20 requirement is orphaned; TUNE-10 is assigned to Phase 19.

## Anti-Patterns Found

| File | Line | Pattern | Severity | Impact |
|---|---:|---|---|---|
| — | — | No unreferenced debt markers, placeholders or live-path stubs found in the changed Phase 20 code and current consumer helper. Empty test lookup/allocation paths are test failure handling, not visible fallback behavior. | — | — |

## Decision Coverage

The selected runtime decision-coverage query reports 16/16 trackable CONTEXT.md decisions honored, with no unhonored decisions. This is a non-blocking consistency check.

## Human Verification Required

N/A — this is a core-library/emulator phase with no UI or owner-only action in its accepted success criteria. Digital behavior is covered by automated tests and source-specific consumer gates. Physical XM wiring, physical clock calibration, analog gain, audibility, retail compatibility and broad browser/handheld acceptance are explicitly outside the contract.

## Gaps Summary

No must-have truth failed, and no human-verification item remains. The prior report was stale because its review report, YM test source and rebuilt test binary, and release-consumer helper changed after its fingerprint. The new fingerprint covers those files. The supplementary \`runner.hashes_file\` timeout remains an incomplete local check and is not called a pass; it does not negate the required recorded gates or the post-fix named transition tests. The hosted release receipt was independently queried and is recorded in the [ship receipt](../../v6.0-SHIP.md).

---

_Verified: 2026-11-13T16:08:51Z_
_Verifier: the agent (gsd-verifier)_
