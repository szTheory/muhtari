---
phase: 20-complete-ym2151-homebrew-sound
plan: "07"
subsystem: audio-consumers-and-evidence
one_liner: Complete YM2151 profile with measured native/Wasm costs, genuine compatibility, clean consumers and bounded review.
duration: 70min
tasks_completed: 3
status: complete
requires:
  - phase: 20-05
    provides: Actual emitted Wasm and clean installed/extracted consumers
  - phase: 20-06
    provides: Genuine independent old/current public-state compatibility matrix
provides:
  - Qualified all-channel and maximum coinstalled audio stress with chronological raw observations
  - Complete final-source native, sanitizer, independent parity, compatibility and portable package evidence
  - Measured ordinary/adverse cost and concrete instance/state/observer bounds
  - Reconciled user/developer documentation, notices and clean two-round review
requirements-completed:
  - YM-01
  - YM-02
  - YM-03
  - YM-04
  - YM-05
  - YM-06
  - YM-07
  - YM-08
coverage:
  - id: selected-profile
    description: Qualified selected/header-auto XM declaration, D7 ports, rational regional clocks, timers/status and v4 IRQ with transactional unsupported reload refusal.
    requirement: YM-01
    verification:
      - kind: integration
        ref: "Final native ym.profile, clock, irq, runner.ym and loaded libretro.host.ym; final-gates JSON; exit0, N/A-local"
        status: pass
    human_judgment: false
  - id: independent-opm
    description: C17 instance-owned integer OPM matches the unchanged exact-pin BSD-3 seed; manual busy expectations and non-silent signed DAC values are independently judged.
    requirement: YM-02
    verification:
      - kind: integration
        ref: "Exact frozen seed oracle; build/v6-phase20-final-oracle.log; exit0, N/A-local"
        status: pass
    human_judgment: false
  - id: bounded-audio
    description: Mono gain/rounding, FIFO/carry/headroom and all twelve mixed-source observations remain bounded and chronological in both regions; legacy observations retained.
    requirement: YM-03
    verification:
      - kind: unit
        ref: "ym.mix, observer, hostile, retained hash judges and final Apple sanitizer; final native/hygiene/sanitizer logs; exit0, N/A-local"
        status: pass
    human_judgment: false
  - id: complete-lifecycle
    description: Reset/reload/isolation, jammed and live IRQ traffic, borrowed output bounds and no frame allocation pass through actual consumers.
    requirement: YM-04
    verification:
      - kind: integration
        ref: "ym.lifecycle, hostile and native/Wasm callback, fallback, state/reset/refusal controls; final native/portable logs; exit0, N/A-local"
        status: pass
    human_judgment: false
  - id: complete-state
    description: Every YM field resumes identically; repaired-integrity semantic faults reject before mutation, and genuine old/current non-YM states remain byte-identical with both continuation directions.
    requirement: YM-05
    verification:
      - kind: integration
        ref: "core.state.ym, generated BIOS origin replay and genuine ten-case matrix plus controls; final compat/sanitizer logs and result JSON; exit0, N/A-local"
        status: pass
    human_judgment: false
  - id: native-consumers
    description: The same generated diagnostic passes independent numeric judgments through public C, fresh runner processes and actual loaded libretro; duplicated mono and callback controls retained.
    requirement: YM-06
    verification:
      - kind: e2e
        ref: "ym.public_runner, libretro.host.ym and native observer receipts; final native CI and quality producer; exit0, N/A-local"
        status: pass
    human_judgment: false
  - id: portable-and-packages
    description: Actual emitted Wasm and clean extracted/installed library, runner and libretro reproduce the diagnostic; notices, source/fixture identity, transferred receipts and fault controls remain effective.
    requirement: YM-07
    verification:
      - kind: e2e
        ref: "Final native release.ym_consumers, six-case pinned portable inventory and actual-archive offline publisher controls; final native/portable/publisher logs; exit0, N/A-local"
        status: pass
    human_judgment: false
  - id: documented-cost-and-limits
    description: Supported requests, API/ABI and per-machine version policy, provenance/update duty, numeric bounds and complete native/Wasm ordinary/adverse cost are documented with precise evidence limits.
    requirement: YM-08
    verification:
      - kind: integration
        ref: "20-COST-EVIDENCE.md, exact final measurement/identity JSON, docs/notices/budgets and clean independent review; required gates exit0, N/A-local"
        status: pass
    human_judgment: false
---

# Complete YM2151 sound and evidence

All seven implementation plans now deliver the one selected XM/YM2151 profile through the library, runner and native/portable libretro, including actual clean packages. This plan closes the eight requirements collectively; preceding summaries deliberately deferred requirement completion until these integrated final gates. The orchestrator's phase verifier still owns goal acceptance.

## Accomplishments

- Qualified a real adverse cartridge with all eight YM channels/32 operators, noise/LFO, full feedback/carrier summing, shortest running timers, dual POKEY and XM services. The runtime test observes all twelve changing sources in both regions, rather than inferring activity from register writes.
- Measured actual ordinary/adverse native and emitted-Wasm windows with exact source, fixture, compiler/runtime and artifact identity. Final native medians range3.7555–5.4932ms/frame and Node medians6.1513–7.7740ms/frame on this host context, including callbacks/copy/hash. These are descriptive observations, with no new speed quota or low-power claim.
- Relinked public allocator and actual private-observer probes against final products: default unloaded instance91,720 requested bytes, YM2,568, optional observer1,492,816, YM state179,330. No frame allocation, fixed output/carry bounds and integer headroom are separate executable/proof obligations.
- Reconciled README, hardware/profile, embedding/runner/libretro, testing/maintaining and provenance/notices without raising existing document budgets. API8/ABI1 stay unchanged; global latest7/2 and no-YM machine envelopes6/1 are distinguished.
- Completed independent review, two bounded fix rounds, and every required final local gate. No formal gap round, owner UAT, GUI/device check, new remote run, push, merge or publication was used.

## Reproduced fixes and review disposition

Initial review reproduced three contract faults: address-port reads lost open bus, several unsupported YM reloads tore down the existing game, and modulo-preserving YM clock forgeries passed state prevalidation. The first fix round resolves all three, including BIOS-origin-zero replay. The final full native gate then exposed a stale equality-row count; only its literal inventory assertion changed to include the additive fourteen YM rows, preserving historical digests.

The assertion-enabled final sanitizer additionally exposed YM output arriving behind POKEY in the coinstalled stress case. A Release-mode monotonic observer independently reproduces it before the fix. The existing TIA catch-up path now merges YM through TIA edges and the caller's actual POKEY half edge; bus completion consumes the remaining slow-cycle tick, and timer deadlines still bound held cycles. No-YM behavior stays on its previous path. Release and Apple ASan/UBSan focus pass after the fix. Independent round-two review at `5158bb6` finds no unresolved issue. [20-REVIEW.md](20-REVIEW.md) retains issue-specific evidence and both rounds.

## Recorded final results

The final executed source is `daab4c82c0bb3393b3e93ee2b23ead09236f16ec`, which differs from reviewed product head `5158bb6` only in review prose. Commands, exits, source-specific failures, complete timings, extents and qualified execution contexts are indexed in [20-COST-EVIDENCE.md](20-COST-EVIDENCE.md). Ignored final-gates/identities JSON binds actual binaries and all changed product files. No raw log, ROM or state bytes are tracked.

Required gates all exit0: complete native CI including native packages; unchanged hygiene at its qualified boundary; exact frozen seed oracle; genuine kickoff/current matrix and controls without rebuilding old products; qualified Apple combined ASan/UBSan focus and both deliberate fault detectors; full-JUnit production quality evidence; pinned emitted/extracted Wasm inventory and ordinary/adverse measurement reports; actual archive through offline mock publisher and fault controls.

The optional offline no-FP attempt configures but GCC13 cannot recognize an existing required warning, exiting2 before product compilation. No warning is removed and no no-FP pass is inferred. PowerPC/qemu and qualified WASI/Wasmtime are unavailable; the original Homebrew sanitizer startup hang remains retained and unaccepted. These are explicitly limited supplemental lanes, not replaced by claims about other platforms.

## Limits and next responsibility

Owned diagnostics, digital source conformance, exact-seed translation parity, actual emitted/package execution and observed-host timing remain distinct. They establish no retail compatibility, analog gain/clock calibration, audibility, browser or handheld acceptance. Phase12/14 stops and all nine deferred v3 requirements remain unchanged. The orchestrator continues phase verification, audit and local milestone archival; shipping retains its separate authority boundary.

## Self-Check: PASSED

The complete required gate receipts have exit0 after the final product edits; both review rounds are clean, current public/observer extents were measured and product bytes still match the executed snapshot. This self-check is not phase verification.
