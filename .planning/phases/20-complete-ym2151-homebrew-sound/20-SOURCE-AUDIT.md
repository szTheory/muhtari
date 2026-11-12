---
phase: 20-complete-ym2151-homebrew-sound
status: covered
assumption_delta_decision: no-change
assumption_delta_rationale: "Callback/batch/fallback/refusal/integrity names existing test routes and does not change the XM/YM device identity model."
discovery_level: 0
estimate_factor: 1
estimate_sample_count: 0
estimate_confidence: low
---

# Phase 20 source coverage and execution map

This audit covers the complete selected Phase 20 outcome. COVERED means a concrete executable plan owns the source item; it is not an implementation or acceptance pass. No source requirement is deferred, reduced to chip-only work or moved to a validation phase. Deferred hardware/host/sibling scope stays excluded.

Current context and research are sufficient; the existing C17/CMake owners, saved exact profile, state policy/adversarial review and integration engineering are reused. No broad upstream survey, package-manager install, UI/MVP/TDD mode, pattern-mapper or API-surface hook is selected. The workflow-selected runtime proved OpenGSD 1.20.0. Estimate calibration returned factor 1, sample count 0 and low confidence; each plan uses that measured calibration rather than self-rated confidence. Graph context is disabled. No project-local skill directory supplied additional rules.

## Execution and ownership

| Wave | Plan | Outcome | Tasks | Dependencies |
|---|---|---|---:|---|
| 1 | 20-01 | Public-C note tracer, fail-closed YM state, full C17 OPM, independent numbers and genuine exact-seed comparison | 2 | Existing source and parent-prepared seed |
| 2 | 20-02 | Complete map/clock/IRQ, signed mix/observation/carry and lifecycle/hostile bounds | 3 | 20-01 |
| 3 | 20-03 | Complete YM state, semantic refusals, unchanged legacy path and explicit versions | 2 | 20-02 |
| 4 | 20-04 | Shared owned diagnostic through public C, actual runner, loaded native core and equality consumers | 3 | 20-03 |
| 5 | 20-05 | Actual Wasm and installed/clean extracted native and portable package consumers | 3 | 20-04 |
| 5 | 20-06 | Genuine old/new byte identity and both continuation directions, safe old refusal | 2 | 20-04 |
| 6 | 20-07 | Actual ordinary/worst native/Wasm costs, docs/notices, complete local gates and bounded review | 3 | 20-05, 20-06 |

Each plan has two or three tasks and the first phase task is the end-to-end production tracer. Later tasks expand useful behavior or qualify a concrete failure boundary of that path. Plans explicitly name all production/test/CMake files; a real tracer spans multiple existing owners. ENGINEERING section 11 rejects numerical-only/file-quota blocking; a split needs a concrete ownership, coupling or execution burden. The root and coordinator reconciled the checker revision to preserve these seven coherent plans and their honest file extents. The shared build serialization already addresses the identified execution coupling; no demonstrated burden requires a new plan.

Same-wave plans 20-05 and 20-06 have no modified-file overlap. Both consume the stable current native library. They must serialize access to the shared build/ci build/test/package resource and avoid running a rebuild while another test uses it; independent compatibility drivers and package scratch have separate owned work directories. Project execution currently disables concurrent plan execution, which naturally preserves this ownership. No overlap is hidden by wildcard file ownership.

The orchestrator owns structural ROADMAP/STATE updates and phase acceptance. Executors own only their declared product/test/docs changes, task commits and source-specific SUMMARY evidence. Every summary includes requirements-completed and block-style coverage with verification kind/ref/status and human_judgment false. No owner-only validation is selected.

## Bounded checker revision: state safety and scope disposition

The demonstrated defect was real: accepting YM in the public tracer before adding YM fields to the format-1 visitor would allow a lossy save/restore. Plan 20-01 task 1 now owns src/state.c and adds a YM-only fail-closed guard in the very same commit that enables public YM loading. A loaded YM instance reports state_size zero; valid-argument save/load return state unavailable before legacy visitation, caller writes or input application. Existing argument precedence and every non-YM state path remain unchanged. The actual CI-registered ym.tracer tests size, save and load, caller/tail preservation, battery bytes/generation, stable spans and identical next execution against an independent equivalent instance. A non-YM save/restore control stays active.

Plan 20-03 task 1 replaces that guard only atomically with the complete topology-aware format-2/behavior-7 visitor, full semantic prevalidation and exact continuation. It updates the same public tracer to demand a real YM envelope and replay rather than silently dropping a safety assertion. Both the tracer and core.state.ym run at that switch. The unavailable guard supplies safety, never YM-05 completion; all original YM state/compatibility/delivery acceptance remains mandatory in this phase.

The other six checker findings were based on numerical file/task thresholds. Their resize request was explicitly superseded by the root/coordinator under ENGINEERING section 11: no complexity score, file quota or numerical threshold alone blocks a coherent change. Existing plans retain concrete ownership, exact file lists, runnable consumer tests and their two/three-task boundaries; no file extent is hidden, no required test is separated into a validation-tail phase, and no deferred scope enters the plan. The seven-plan/six-wave shape therefore remains. This is a policy disposition of generic scope warnings, not a claim that every scope estimate is an implementation measurement. The first plan estimate grows from 42000 to 44000 raw/calibrated tokens for the actual guard/test work; the existing factor 1 and low-confidence calibration are unchanged.

## Parent-prepared prerequisites

- Genuine old source: clean detached .worktrees/ym-state-baseline at fef56919cc145eb09fdb0a0939dfbd7f17012104. Existing old native library/runner/core are under build/v6-state-baseline; build/v6-state-baseline-receipt.json binds their digests and successful source/build/version probe. These are provenance/setup facts only. The actual byte identity, old→new and new→old continuation, and safe refusal remain required execution in 20-06.
- Approved original OPM inputs: build/v6-ym-seed/LICENSE and only src/ymfm_opm.cpp, ymfm_opm.h, ymfm_fm.ipp, ymfm_fm.h and ymfm.h. build/v6-ym-seed/source-receipt.json binds the full approved pin, all six input hashes and build/v6-ym-seed/ymfm_opm.o. Consume the existing object without refetching or rebuilding it. The retained first strict compile failure concerns upstream unused parameters; the subsequent successful object uses only that diagnostic exception, with no production flag change.
- 20-01 owns the new harness and actual original-object versus production-C workload. Prior qualification does not establish parity. C-only pin-bound expectations provide recurring native/portable CI coverage; fresh genuine oracle execution remains required local acceptance and update duty.
- The native CI and unchanged hygiene workflows, current pinned Emscripten script, current package/install consumers and no-skip inventories are required existing mechanisms. All proposed new command paths are created by their owning plan before use; every filtered CTest command uses no-tests=error. No inherited Phase 19 receipt audit is used as Phase 20 verification.

## Four-source coverage audit

| SOURCE | ID | Feature or binding constraint | Plan/tasks | Status |
|---|---|---|---|---|
| GOAL | Phase 20 | Documented profile produces deterministic FM with lifecycle/state across every existing deliverable and portable/package consumer | 20-01–20-07 | COVERED |
| GOAL | Success 1 | Qualified selected/autodetected topology, source-grounded map/enable/clock/status/timer/IRQ and preallocation refusal | 20-01.1–2, 20-02.1 | COVERED |
| GOAL | Success 2 | Independent non-silent bounded PCM/observations, rates/regions/carry/max mix, exact-seed parity and unchanged prior audio | 20-01.1–2, 20-02.2, 20-04.1–3 | COVERED |
| GOAL | Success 3 | Reset/isolation/state/no-late-allocation/finite work/buffer and battery ownership | 20-02.3, 20-03.1–2, 20-06.1–2 | COVERED |
| GOAL | Success 4 | Actual public/runner/loaded-native/Wasm/extracted/installed consumers and retained controls | 20-04.1–3, 20-05.1–3 | COVERED |
| GOAL | Success 5 | Existing docs describe supported request and limits; numerical and measured cost evidence is proportionate | 20-07.1–3 | COVERED |
| REQ | YM-01 | Declared digital XM/YM topology/map/enable/clock/status/timers/IRQ; unsupported load refuses safely | 20-01, 20-02, 20-06 | COVERED |
| REQ | YM-02 | Instance-owned integer C17 approved seed, independent numbers, actual pin parity, notices/update duty | 20-01, 20-07 | COVERED |
| REQ | YM-03 | Deterministic bounded mono/observations/rates/regions/carry/max mix; old PCM and duplicated mono unchanged | 20-01, 20-02, 20-04, 20-06, 20-07 | COVERED |
| REQ | YM-04 | Reset/reload/isolation/no-late-allocation/finite work/borrowed buffer bounds | 20-02, 20-03, 20-05, 20-07 | COVERED |
| REQ | YM-05 | Complete YM continuation, semantic/incompatible refusal and live/battery preservation under settled policy | 20-03, 20-04, 20-05, 20-06, 20-07 | COVERED |
| REQ | YM-06 | Same independently judged diagnostic via actual public/runner/loaded-native and retained callback alternatives | 20-04, 20-05, 20-07 | COVERED |
| REQ | YM-07 | Actual portable and clean extracted/installed package consumers, identities/notices/integrity faults | 20-04, 20-05, 20-07 | COVERED |
| REQ | YM-08 | Exact existing docs, ordinary/worst actual native/Wasm cost, headroom/extents/finite bounds | 20-02, 20-03, 20-07 | COVERED |
| RESEARCH | Approved source scope | Exactly six pinned BSD-3 inputs, only OPM C17 specialization, actual separately linked original oracle; no production C++ or package | 20-01.1–2, 20-07.2 | COVERED |
| RESEARCH | Owner seams | instance/cart eligibility and access; bus chronology/deadlines/IRQ; synth mono/observation; single state visitor | 20-01.1, 20-02.1–3, 20-03.1 | COVERED |
| RESEARCH | Source truth classes | Primary-source timer/busy/ordering numbers precede exact-seed parity and actual consumer equality; none proves physical XM | 20-01.2, 20-04.1–2, 20-07.2 | COVERED |
| RESEARCH | C17 hazards | Define signed shifts, narrowing/quantization/rounding, zero/width masks and wide product/add bounds | 20-01.1–2, 20-02.2, 20-07.3 | COVERED |
| RESEARCH | Chip corpus | Eight algorithms, envelopes, all operators, feedback, noise/LFO, both signed outputs, key/register/busy/timer/IRQ edges, reset/continuation/isolation | 20-01.2, 20-02.1–3, 20-03.1 | COVERED |
| RESEARCH | Cadence/overshoot | Rational tick/2 cadence and 64-chip-clock sample, independent HALT/slow cycles; integrated CPU/interrupt overshoot rather than frame-only ceilings | 20-01.1, 20-02.1–3, 20-07.1 | COVERED |
| RESEARCH | Signed observer | Existing uint8 caches/carry cannot hold YM; explicit signed bits and new IDs preserve old 11-byte records | 20-01.1, 20-02.2, 20-04.1–3 | COVERED |
| RESEARCH | Gain/headroom | Topology-only gain/rounding with complete mix/delta/impulse/integrator/DC/clip/visitor proof and maximum mix tests | 20-01.1, 20-02.2, 20-03.1, 20-07.1 | COVERED |
| RESEARCH | Event capacity | Coincident old/new transitions and real pending/carry maxima across regions/rates and overshoot; no guessed global widening | 20-02.2–3, 20-03.1, 20-07.1 | COVERED |
| RESEARCH | Legacy schema | YM tags/fields/array growth absent from non-YM identities and visitor; actual envelopes selected per machine | 20-03.1–2, 20-06.1–2 | COVERED |
| RESEARCH | Version consumers | Frozen prefixes/signatures/API8/ABI1; global reporting7/2 versus non-YM6/1; revision-6 movie warning/runner metadata expected | 20-03.2, 20-07.2 | COVERED |
| RESEARCH | Genuine legacy evidence | Kickoff-produced actual states and both continuation directions plus same-point bytes across regions/RAM/expansion/battery | 20-06.1–2 | COVERED |
| RESEARCH | Intermediate state safety | Every publicly accepted YM machine has tested size/save/load refusal until complete 2/7 continuation replaces it atomically; non-YM paths stay unchanged | 20-01.1, 20-03.1 | COVERED |
| RESEARCH | State semantic ownership | Complete mutable fields or proven cache reconstruction; CRC-repaired late semantic faults, exact identity/version/topology refusals, no mutation/allocation/observer/caller-tail changes | 20-03.1–2, 20-06.2 | COVERED |
| RESEARCH | Numeric diagnostic | One lawful generated CPU-port workload; old refusal and new non-silence; identical bytes through all interfaces | 20-01.1, 20-04.1–2, 20-05.1–3, 20-06.2 | COVERED |
| RESEARCH | Actual delivery | Real runner, dynamically loaded native core, emitted portable module, staged/extracted public library and clean package replay | 20-04.1–2, 20-05.1–3 | COVERED |
| RESEARCH | Retained faults | Callback/batch/fallback/missing callbacks/tails, semantic refusals, source/content/notice/pair/integrity/extraction and representative valid/invalid judges | 20-01.2, 20-02.2–3, 20-03.2, 20-04.1–3, 20-05.1–3 | COVERED |
| RESEARCH | Registration/quality | Exact native/reduced/portable inventories, source/fixture identities, existing equality and quality pass/fault mechanisms; unchanged required/advisory topology | 20-01–20-05, 20-07.3 | COVERED |
| RESEARCH | Actual cost | Ordinary/worst native and Wasm complete comparable windows, real toolchains/artifacts/regions, state/instance/observer extents; no low-power extrapolation or new estimated quota | 20-07.1 | COVERED |
| RESEARCH | Hygiene boundary | Unchanged native/hygiene gates; qualified outside-sandbox shell-heavy context, finite diagnosis/retests and no deadline relaxation | 20-07.3 | COVERED |
| CONTEXT | D-01 | Existing XM selected/autodetected declaration only; no-YM XM unchanged and unsupported requests refuse before mutation/allocation | 20-01.1, 20-02.1, 20-04.1 | COVERED |
| CONTEXT | D-02 | Exact ports, D7 of current whole-byte latch, coexistence/no aliases/no implied reset | 20-02.1, 20-07.2 | COVERED |
| CONTEXT | D-03 | Rational MARIA-tick/2 cadence and exact 64-clock output/event carry | 20-01.1, 20-02.1–2 | COVERED |
| CONTEXT | D-04 | Independent busy/timer/order qualification and valid-v4-only bit2 IRQ through existing bus | 20-01.2, 20-02.1 | COVERED |
| CONTEXT | D-05 | Approved exact BSD-3 C17 OPM specialization, notices/update duty and original oracle limits | 20-01.1–2, 20-05.3, 20-07.2 | COVERED |
| CONTEXT | D-06 | Public shape/API8/ABI1/mono duplicated-mono unchanged, signed private observations and old canonical records retained | 20-01.1, 20-02.2, 20-03.2, 20-04.1–3 | COVERED |
| CONTEXT | D-07 | Additive YM-only2/7 versus non-YM1/6 identity/schema; global-latest report and single visitor | 20-03.1–2, 20-06.1–2 | COVERED |
| CONTEXT | D-08 | Genuine kickoff two-way continuation/byte identity and metadata-only global version updates | 20-03.2, 20-06.1–2 | COVERED |
| CONTEXT | D-09 | Complete semantically validated state, no mutation, lifecycle/isolation/no-late-allocation/finite ownership | 20-02.3, 20-03.1–2, 20-05.1, 20-06.2 | COVERED |
| CONTEXT | D-10 | Gain chosen only after numeric bounds, topology preservation and complete signed mix/carry proof | 20-01.1, 20-02.2, 20-07.1 | COVERED |
| CONTEXT | D-11 | Lawful generated diagnostic, independent numbers, all-algorithm/feedback/noise/LFO/hostile parity corpus | 20-01.1–2, 20-02.1–3, 20-04.1 | COVERED |
| CONTEXT | D-12 | Same workload in actual public/runner/loaded-native/Wasm/installed/extracted consumers; current controls/mechanisms | 20-04.1–3, 20-05.1–3 | COVERED |
| CONTEXT | D-13 | Actual native/Wasm ordinary/worst costs/extents/headroom/overshoot and measurement-before-new-limit policy | 20-02.2–3, 20-07.1 | COVERED |
| CONTEXT | D-14 | Final native/hygiene/focused/parity/old-new/actual emitted/package gates; background, finite causal retries | 20-07.3 | COVERED |
| CONTEXT | D-15 | Current identities/notices/topology/integrity; exact documented profile and narrow confidence/physical limits | 20-01.2, 20-04.3, 20-05.1–3, 20-07.2 | COVERED |
| CONTEXT | D-16 | Authorized local completion/review at two fix rounds/one gap; all old stops/deferred gaps and remote/host boundaries remain | 20-07.3 and all plan execution contexts | COVERED |

No MISSING source item remains. Exclusions are exactly CONTEXT Deferred Ideas, requirements scoped to other phases and research's explicitly excluded hardware/host/sibling/general-migration/CI restructuring scope.

## Advisory spec-less probe classification

The orchestrator's separate spec-less heuristic reported 17 applicable rows and zero resolved classifications. This table preserves every row and that source/gate-specific unresolved classification. It does not claim probe success, auto-resolve unclassified rows, create a SPEC, introduce a product choice or add seventeen bespoke gates. Concrete required behavior is covered directly by the locked source contracts and the existing/new recurring checks named in plans. The heuristic's classification remains advisory; actual missing required-contract evidence blocks acceptance independently.

| Probe row | Classification retained | Concrete locked source / required coverage |
|---|---|---|
| YM-01 unclassified | Unresolved advisory | D-01–D-04; 20-02.1 topology, exact latch, cadence, status/timers and v4-only IRQ/refusals |
| YM-04 unclassified | Unresolved advisory | D-09/D-13; 20-02.3 reset/reload/isolation/allocation/buffer and hostile finite work |
| YM-06 unclassified | Unresolved advisory | D-11/D-12; 20-04 actual public/runner/loaded-core plus callback/numeric controls |
| YM-07 unclassified | Unresolved advisory | D-12/D-15; 20-05 emitted/clean installed/extracted routes and identities/notices/integrity |
| YM-02 adjacency | Unresolved advisory | D-04/D-11; numeric just-before/exact/after busy/timer/write edges in 20-01.2/20-02.1 |
| YM-02 empty | Unresolved advisory | D-09/D-11; reset silence, disabled access and undeclared/no-YM paths in 20-01.1/20-02 |
| YM-02 ordering | Unresolved advisory | D-03/D-04; due-event-before-data and exact trace ordering in 20-01.2/20-02.1 |
| YM-02 idempotency | Unresolved advisory | D-05/D-11; repeated register writes/reset and pin parity corpus in 20-01.2 |
| YM-02 concurrency | Unresolved advisory | D-05/D-09; independent instance-owned chips/interleaving in 20-01.2/20-02.3; no new thread-safety promise |
| YM-03 boundary | Unresolved advisory | D-06/D-10; signed extrema, max mix, borrowed tails, rates/regions/carry in 20-02.2 |
| YM-03 precision | Unresolved advisory | D-03/D-06/D-10; exact rational clock, signed bits and defined integer rounding in 20-01/20-02 |
| YM-05 adjacency | Unresolved advisory | D-09; mid-event/partial-carry captures and late repaired-CRC semantic faults in 20-03 |
| YM-05 empty | Unresolved advisory | D-09; no-content/zero/truncated and undersized/oversized-buffer behavior in 20-03 |
| YM-05 ordering | Unresolved advisory | D-07/D-09; complete validate-before-apply and size-before-version refusal semantics in 20-03/20-06 |
| YM-05 idempotency | Unresolved advisory | D-09; repeated identical restore and continuation in 20-03.1 |
| YM-05 concurrency | Unresolved advisory | D-09; per-instance state isolation and host-owned battery/spans in 20-02/20-03/20-05; no concurrent-call promise |
| YM-08 concurrency | Unresolved advisory | D-13; comparable restored workload windows, explicit observer cost and serialized shared build ownership in 20-07.1 and this execution map |

## Plan validation contract

Every task supplies read_first, exact files, concrete action/acceptance_criteria, automated verify with adjacent fails_when, and measurable done. Existing CMake/sh consumers and representative pass/fault coverage are reused. Frontmatter and structure are checked with the workflow-selected runtime before commit; each file must end at its complete output tag. No plan contains personal absolute paths, committed logs/media/state bytes, bypasses, forbidden entrypoints or chaining arguments.

