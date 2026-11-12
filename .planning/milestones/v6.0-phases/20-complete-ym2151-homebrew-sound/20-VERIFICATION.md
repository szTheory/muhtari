---
phase: 20-complete-ym2151-homebrew-sound
verified: 2026-11-12T15:36:15Z
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
covered_digest: "v4:sha256:ed2d5decdae7ef6feede6741d8db38c6fedbe7deea3d648419277545632d3893"
behavior_unverified: 0
overrides_applied: 0
decision_coverage:
  honored: 16
  total: 16
  not_honored: []
---

# Phase 20: Complete YM2151 homebrew sound Verification Report

**Phase Goal:** A host can load one documented XM/YM2151 profile and obtain deterministic FM sound with complete lifecycle/state behavior through every existing deliverable and portable/package consumer.
**Verified:** 2026-11-12T15:36:15Z
**Status:** passed
**Re-verification:** No — the prior report had no gaps section. This initial-mode pass rechecked the archived contract and the current observer-restore change.
**Refresh check:** The only intervening test-source change reformats a wrapped hash assignment in tests/audio/test_ym2151.c. The expression and behavior are unchanged; previously recorded targeted runtime results remain applicable. No tests were rerun.

## Goal Achievement

### Observable Truths

| # | Truth | Status | Evidence |
|---|---|---|---|
| 1 | Hosts can load the documented selected or header-autodetected XM/YM profile, with qualified ports, D7 gate, regional clock, busy/status, timers and v4-only IRQ; unsupported combinations refuse transactionally. | ✓ VERIFIED | src/cart.c routes the XM ports into src/ym2151.c; src/bus.c schedules timer deadlines and samples the YM IRQ. The active profile tests and loaded-module checks are registered in the existing audio/libretro inventories. Phase 20's final native gates passed before the current observer-only correction. The archived hardware profile names the numeric/manual evidence and physical-XM limits. |
| 2 | The homebrew produces deterministic, non-silent bounded mono FM and chronological signed observations while preserving no-YM results and duplicated-mono libretro output. | ✓ VERIFIED | The current OPM implementation emits its two signed channel samples into src/synth.c; the mixer consumes real queued source levels into frame PCM. The fixed-pinned seed oracle is separately linked and receipt-checked; source-independent busy/timer values are judged separately. The current targeted ym.observer test passes in both NTSC and PAL after the fix. Existing exact no-YM identity/audio checks and actual native/Node consumers passed in the recorded final gates. |
| 3 | YM state resumes with identical output, refusals preserve live and battery state, genuine non-YM snapshots retain their envelope, and observer replay cannot leak a discarded future carry. | ✓ VERIFIED | The state visitor validates the full YM payload before apply. After a successful apply, muhtari__synth_observe_reset clears pending/carry records and copies restored synthesis raw values into the observer cache; it leaves the timing cursor intact. The regression in tests/audio/test_ym2151.c saves a checkpoint, records a control frame, advances into an abandoned future, queues a matching future carry, restores, and compares the replay's count, sequence hash, source mask, per-channel counts and polarity totals. The lesson records current build success, ym.observer plus core.state.equality_report 2/2, and hashes_state_control success (including 78 state rows and semantic-fault controls). The genuine independent old/current ten-case, two-direction matrix and refusal controls passed in the preceding final-source compatibility gates. |
| 4 | The owned diagnostic reaches the public library, runner, loaded libretro, emitted Wasm and clean package consumers with numeric judgments before cross-route equality. | ✓ VERIFIED | The archived final native and portable receipts show public C, real runner, dynamically loaded libretro, emitted Wasm, clean extracted/installed packages and actual archive controls passing. The current correction is confined to the private observer; ordinary instances return immediately from the reset helper when no observer is registered. Current state-report and observer regression tests pass. The full cross-consumer gates have not been rerun after this observer-only change; their recorded receipts are from the immediately preceding product snapshot. |
| 5 | Integrators can reproduce the support/compatibility policy and maintainers can reproduce measured native/Wasm cost, extents and finite bounds without broader hardware claims. | ✓ VERIFIED | The archived requirements, hardware and embedding docs cover the supported profile, API 8/ABI 1, machine-specific state envelopes, seed/update duties, costs and evidence limits. The cost report records ordinary/adverse workloads in both regions, actual Wasm/Node windows, extents and structural limits. All 16 trackable context decisions are honored. Plan 07's declared cost artifact path points to the pre-archive .planning/phases/ location; the real artifact exists at this archived phase directory and was read directly. |

**Score:** 5/5 truths verified (0 present, behavior-unverified)

## Required Artifacts

| Artifact | Expected | Status | Details |
|---|---|---|---|
| src/ym2151.c | Integer, per-instance OPM engine and bounded interfaces | ✓ VERIFIED | Substantive implementation is included in the shipped library; YM clock, register, timer and output paths are called from the cart/bus owners. |
| src/state.c | Complete conditional YM state with prevalidation and transactional apply | ✓ VERIFIED | YM fields are included in the topology-aware visitor; validation precedes mutation. Successful apply now resets the private observer cache from restored synth state. |
| src/synth.c | Signed source observation, bounded mixer and observer reset | ✓ VERIFIED | muhtari__synth_observe_reset drops pending/carry records and synchronizes observer raw levels; muhtari__synth_render drains queued PCM into caller output. |
| tests/audio/test_ym2151.c | Profile, numeric sound, lifecycle, bounded output and observer replay checks | ✓ VERIFIED | Current regression asserts exact replay-summary equality; the targeted named test passes in both regions. |
| tests/core/test_state.c, tests/core/state_compat.c, tests/core/state_compat.cmake | Recurring state invariants and independent old/current compatibility | ✓ VERIFIED | Existing state report and semantic controls pass after the fix; compatibility driver has separate source/header/library provenance checks and independent continuation comparisons. |
| tests/emscripten/host.mjs, tests/emscripten/bundle.cmake, tests/release/ym_consumers.cmake | Actual portable and clean-package consumers | ✓ VERIFIED | Current sources connect native receipts to the Node consumer and actual extracted archive validation. Recorded final portable and release gates passed before the observer-only correction. |
| docs/hardware/ym2151.md, docs/embedding.md, 20-COST-EVIDENCE.md | Profile, compatibility, measured cost and evidence limits | ✓ VERIFIED | Docs were inspected. The plan's old live-phase path for the cost report is stale from archive relocation; the archived report is present. |

The selected runtime's artifact query passed 21 of 21 declared artifacts across plans 20-01 through 20-06. Plan 20-07 reported one missing cost report because its declared path still points to the former live phase directory; direct inspection confirms the report at the archived path above. This is a path relocation issue, not a missing artifact.

## Key Link Verification

| From | To | Via | Status | Details |
|---|---|---|---|---|
| tests/cart/images.c | src/cart.c | Generated CPU program writes selected XM/YM ports | ✓ WIRED | Cart read/write dispatch calls YM register read/write; profile and clock tests execute the generated diagnostic. |
| src/bus.c | src/ym2151.c | Timer deadlines and IRQ sampling | ✓ WIRED | Bus rearming consults YM timer deadlines and samples muhtari__ym2151_irq; chip access catches up to absolute ticks. |
| src/ym2151.c | src/synth.c | Signed YM outputs reach mix and observer | ✓ WIRED | Each channel is reported to observer and mixer; the mixer queues and renders PCM into the frame buffer. |
| src/state.c | src/ym2151.c | State visitor covers mutable YM state | ✓ WIRED | Visitor traverses the YM structure; current YM state and semantic-refusal tests exercise save/restore and validation. |
| runner/main.c / libretro/libretro.c | Public library | Runner and loaded module call public create/load/frame/state APIs | ✓ WIRED | Runner calls muhtari_load, muhtari_load_state and muhtari_run_frame; loaded libretro calls the same library APIs and has a real-module test. |
| tests/libretro/observe.c | tests/emscripten/host.mjs | Source-bound native receipts feed actual Node/Wasm judgment | ✓ WIRED | The portable producer invokes the host in report/verify modes with native receipts and named YM fixtures. |
| scripts/emscripten.sh | tests/emscripten/bundle.cmake | Build, inventory, archive extraction and package acceptance | ✓ WIRED | The producer executes the bundle acceptance path; the actual emitted/extracted gates passed on the prior final product snapshot. |
| tests/core/state_compat.cmake | Genuine kickoff and current libraries | Independently linked producer/consumer drivers | ✓ WIRED | The helper checks source/header/library identity and runs ten region/topology cases in both continuation directions plus fault controls. |
| docs/testing.md | tests/core/state_compat.cmake | Documented reproducible compatibility command | ✓ WIRED | The docs name the actual helper command and its genuine-old-source prerequisites. |

The generic key-link query reports false negatives where links are expressed through CMake composition, API dispatch or path arguments rather than target-path text. Direct source and command tracing above establishes those connections. The archive relocation explains the one stale planning path.

## Data-Flow Trace (Level 4)

| Artifact | Data Variable | Source | Produces Real Data | Status |
|---|---|---|---|---|
| src/ym2151.c → src/synth.c | ym->output[0..1] → synth.raw[]/pending events | Actual register-driven OPM clocks and channel output | Yes | ✓ FLOWING |
| src/synth.c → caller | PCM queue → muhtari_frame.audio and audio_count | Integer mixer/filter over queued TIA, POKEY and YM levels | Yes | ✓ FLOWING |
| libretro/libretro.c | Rendered mono frame | Current library frame buffer, delivered through callback or duplicated-mono batch | Yes | ✓ FLOWING |
| tests/emscripten/host.mjs | Native and portable observation reports | Actual native observer and actual emitted Wasm module on the named generated fixture | Yes | ✓ FLOWING |

## Behavioral Spot-Checks

| Behavior | Command / evidence | Result | Status |
|---|---|---|---|
| Rewind observer cache and carry reset | cmake --build build/v6-ym-sanitize-apple --target muhtari-test-ym2151 muhtari-test-state muhtari-run --parallel 4 | Exit 0 after the final fix; exact current command recorded in .planning/LESSONS.md. | ✓ PASS |
| Observer replay plus state equality report | ctest --test-dir build/v6-ym-sanitize-apple --output-on-failure -R '^(ym\\.observer|core\\.state\\.equality_report)$' | Exit 0, 2/2 after the final fix. | ✓ PASS |
| State hash report and semantic fault controls | cmake -DCASE=hashes_state_control -DHASHES=$PWD/tests/cmake/hashes.cmake -DSTATE_TEST=$PWD/build/v6-ym-sanitize-apple/tests/muhtari-test-state -DOUT=$PWD/build/v6-ym-sanitize-apple -DEMULATOR= -P tests/runner/tests.cmake | Exit 0; all 78 state rows and semantic fault controls passed after the final fix. | ✓ PASS |
| Formatting-only hygiene inventory | cmake --workflow --preset hygiene | Exit 0, 30/30 checks after the formatter change; supplied by the orchestrator. | ✓ PASS |
| Lane equality regression controls | ctest --test-dir build/v6-ym-sanitize-apple --output-on-failure -R '^policy\\.lane_equality\\.controls$' | Exit 0 after the observer reset implementation, before the last extra observer assertion; not treated as a post-assertion run. | ✓ PASS (earlier snapshot) |
| Broad runner hash/equality matrix | runner.hashes_file in the local Apple sanitizer build | Timed out at its configured 286-second CTest bound while processing the expansion matrix (exit 8). This is not a pass and is not evidence of a product assertion failure. | ? INCOMPLETE |
| Prior full native, compatibility, exact-seed, hygiene, portable and package gates | Exact command and source identity in 20-COST-EVIDENCE.md | Required final gates passed for the preceding final product snapshot; the new code only resets private observer state after successful state restore. | ✓ PASS (prior snapshot) |

The narrow current checks exercise the state transition introduced by this fix. The broader timed-out matrix remains an incomplete local check; it does not invalidate the passing named transition tests. A hosted PR run that failed state equality was on the older head 3cce920 and is the reason for this correction. The corrected-head hosted result remains part of shipping and is not claimed here.

## Probe Execution

No phase-declared probe or conventional scripts/*/tests/probe-*.sh was found. Probe execution is not applicable.

## Test Quality Audit

| Test File | Linked Req | Active | Skipped | Circular | Assertion Level | Verdict |
|---|---|---:|---:|---:|---|---|
| tests/audio/test_ym2151.c | YM-01–04 | Yes | 0 | No | Numeric values, state transitions, exact replay sequence | ✓ ADEQUATE |
| tests/audio/ym_oracle.cpp + tests/audio/ym_oracle.cmake | YM-02 | Yes | 0 | No | Receipt-bound actual C-vs-pinned-seed sample/status/IRQ comparison | ✓ ADEQUATE |
| tests/core/test_state.c | YM-05 | Yes | 0 | No | Repaired-integrity semantic faults, equality report, unchanged state/battery | ✓ ADEQUATE |
| tests/core/state_compat.c + tests/core/state_compat.cmake | YM-05 | Yes | 0 | No | Independent old/current bytes and multi-frame two-way continuation | ✓ ADEQUATE |
| tests/runner/tests.cmake, tests/libretro/host.c, tests/libretro/observe.c | YM-06 | Yes | 0 | No | Actual process/module execution and numeric output judgment | ✓ ADEQUATE |
| tests/emscripten/host.mjs, tests/emscripten/bundle.cmake, tests/release/ym_consumers.cmake | YM-07 | Yes | 0 | No | Actual emitted/extracted artifacts, identity checks and fault controls | ✓ ADEQUATE |
| tests/audio/test_ym2151.c, tests/emscripten/host.mjs | YM-08 | Yes | 0 | No | Positive complete timing windows, source/workload identities and extent/bound judgments | ✓ ADEQUATE |

**Disabled tests on requirements:** 0.
**Circular expected-value patterns:** 0. Writers in the mapped suites produce runtime state, fixtures, receipts or reports; the pinned C++ oracle is a separately receipt-bound seed, not the C implementation under test.
**Insufficient assertions:** 0.

## Requirements Coverage

| Requirement | Source Plan | Description | Status | Evidence |
|---|---|---|---|---|
| YM-01 | 20-01, 20-02 | Selected/autodetected profile, qualified ports/timing/IRQ, transactional refusal | ✓ SATISFIED | Actual profile, clock, timer/IRQ, runner and loaded-module checks; direct cart/bus/chip trace. |
| YM-02 | 20-01, 20-07 | Integer C17 OPM, non-silent FM, exact pinned seed parity and retained BSD-3 notices | ✓ SATISFIED | Numeric YM tests, exact-pinned oracle result, notices and provenance. |
| YM-03 | 20-02, 20-03, 20-04, 20-07 | Deterministic bounded mono/canonical audio, signed carry/mix, retained no-YM results | ✓ SATISFIED | Current observer regression and state equality report pass; final audio and consumer gates were recorded on the preceding product snapshot. |
| YM-04 | 20-02, 20-03, 20-04, 20-07 | Reset/reload/isolation, finite work, no late allocation and guarded output | ✓ SATISFIED | Lifecycle/hostile and consumer tests in final native/portable receipts; current code reset helper returns without mutation when no private observer exists. |
| YM-05 | 20-03, 20-06, 20-07 | Complete YM state and transactional refusal plus unchanged legacy compatibility | ✓ SATISFIED | Current observer rewind/state checks pass; genuine independent ten-case, two-direction legacy matrix and refusal controls passed in final compatibility receipts. |
| YM-06 | 20-04, 20-07 | Same diagnostic through public C, runner and loaded libretro | ✓ SATISFIED | Actual public, fresh runner process and loaded module tests, independent numeric checks before hashes; final native receipt. |
| YM-07 | 20-05, 20-07 | Actual portable artifact and clean extracted/installed packages with identity/notices/controls | ✓ SATISFIED | Actual emitted Wasm, extracted archive, installed consumers and offline publisher control receipts. These precede the private observer reset fix. |
| YM-08 | 20-01–20-07 | Supported profile/evidence limits and measured ordinary/adverse cost, extent and finite bounds | ✓ SATISFIED | Archived profile/embedding/cost docs and exact-source cost reports, reviewed directly. |

The archived requirements file maps YM-01 through YM-08 exactly once to Phase 20. No phase requirement is orphaned. TUNE-10 belongs to Phase 19 and is out of this phase's requirements set.

## Anti-Patterns Found

| File | Line | Pattern | Severity | Impact |
|---|---:|---|---|---|
| — | — | No unreferenced debt markers or live-path stubs found in the inspected changed code. The return-NULL matches in state tests are legitimate test lookup/allocation failure paths. | — | — |

## Decision Coverage

Selected runtime result: 16/16 trackable context decisions honored; not_honored is empty. Decision coverage is a non-blocking consistency check.

## Human Verification Required

N/A — this is a core-library/emulator phase with no user-facing visual or external-service workflow to judge manually. The requested digital behavior and the observer state transition have automated numeric/runtime evidence. Physical XM wiring, clock calibration, analog gain, audibility, retail compatibility and handheld/browser acceptance are explicitly outside this phase and are not being claimed.

## Gaps Summary

No must-have truth failed and no human-verification item remains. The targeted current restore/equality checks pass. The broader local runner.hashes_file sanitizer attempt timed out at 286 seconds and is explicitly not counted as passed; current corrected-head hosted CI remains a shipping check. The earlier hosted failure was on old head 3cce920, not this correction. Plan 20-07's cost-evidence path is stale after archive relocation, but the archived evidence file exists. These are recorded evidence/scope limits, not an observed failure of the phase goal.

---

_Verified: 2026-11-12T15:36:15Z_
_Verifier: the agent (gsd-verifier)_
