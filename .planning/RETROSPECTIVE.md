# Project Retrospective

## Milestone: v1.0 — 7800 games play with sound in RetroArch

**Shipped:** 2026-10-21
**Phases:** 4 | **Plans:** 73 | **Recorded tasks:** 149

### What Was Built

- Shipped an embeddable C17 library, headless runner and libretro core with automated attested releases and one-line RetroArch installation.
- Matched all 2,560,000 public 6502 vectors and implemented bounded SALLY bus timing, RIOT and validated cartridge loading.
- Rendered linear and SuperGame cartridges with measured MARIA DMA timing, diagnostic ROMs and cross-platform frame hashes.
- Integrated fast boot, CC0 OpenBIOS, controller movies, achievement memory maps and automated RetroArch gameplay checks.
- Delivered integer TIA and POKEY sound with canonical raw/PCM hashes, libretro callback parity and clipping-free sample pins.
- Verified the production 64-phase Q23 synthesiser against spectral and alias controls, with bounded allocation-free frame execution.

### What Worked

Preset-driven local/hosted verification, actual adapter/runner parity and negative
controls exposed integration faults without owner UAT. Source-derived diagnostics
and canonical hashes made timing and platform differences reproducible. App-driven
release publication and the latest-install check closed ship-time evidence.

### What Was Inefficient

Sequential audio plans shared files and required many wave invocations until the
whole-phase command was used. Later covered-file edits invalidated earlier phase
fingerprints. Hosted workload growth exceeded provisional CI timeouts. Gate-code
proportion remained over budget, and a shared test output caused a freshness race.

### Patterns Established

Three deliverables, integer instance state, bounded emulated calls, canonical
pixel/raw/PCM evidence and source-cited hardware tests remain the contracts.
One GSD step per session can execute every dependency-ordered wave in a phase;
it does not require a separate invocation per wave. Review budgets keep tooling
findings as seeds rather than extra requirements.

### Key Lessons

1. Execute a planned phase with the whole-phase command; reserve wave selection for targeted recovery.
2. Reconcile verification after the final covered-file change; distinguish a historical pass from a current fingerprint.
3. Measure CI limits against the final workload and give concurrent checks independent output paths.
4. Keep release completion evidence separate from pre-merge verification, then reconcile planning records at closeout.

### Cost Observations

Model mix and total session count are not reliably recorded; no estimate is asserted.
The summaries record 149 tasks across 73 plans. Shared-file dependencies made
the 17 sound waves sequential even when one command dispatched the full phase.

## Milestone: v2.0 — The whole library

**Shipped:** 2026-10-29
**Phases:** 7 | **Plans:** 75 accepted / 76 summarized active slots / 82 historical summaries
**Recorded tasks:** 192 across all summary records, including superseded partial work

### What Was Built

- Shipped Activision, Absolute, Fractalus, BankSet and the XBoard/XM RAM/POKEY/HSC subset through all three deliverables.
- Added load-stable HSC/SaveKey battery storage, checked runner saves and real fresh-launch RetroArch persistence.
- Delivered typed controller input and movie2 replay for lightgun, paddle/keypad, quadrature and modern adapters, with independent pin-level diagnostics.
- Added PAL timing, geometry, boot and sustained audio carry while retaining existing NTSC observations.
- Accounted for every pinned-list entry, bound compatibility claims to public evidence and made the library ratchet required in CI.
- Improved pipeline reliability, armed the controlled performance comparison and completed automated release/install verification.

### What Worked

Independent numeric/media/pixel judgments precede equality. Real runner/module and
fresh-process frontend tests expose host seams. Existing CMake inventories, fault
controls, scoped API additions and source-bound evidence keep three deliverables
aligned. Release publication and latest-install close the post-merge evidence.

### What Was Inefficient

Fixed LOC and compressed-byte cutoffs repeatedly halted otherwise coherent work
before their accepted retirement. Long publication controls and provisional bounds
exposed recurring platform friction. Historical fingerprints and prose-only task
counting required explicit closeout reconciliation; neither proves acceptance.

### Patterns Established

Preserve halted history, actual base/source/attempt identity and every required
fault control. Give concurrent consumers independent output paths; calibrate bounds
from complete work. Keep debt explicit and optional improvements in seeds. Metadata,
model conformance, public title screens and gameplay remain distinct evidence classes.

### Key Lessons

Use the actual metadata consumer, cross-reference requirements/verification/summaries,
and count accepted plans separately from records (LESSONS MU-GSD-012–014). Follow
bounded recovery rather than repeating general replanning. Keep final package
publication separate from pre-merge and audit-time receipts.

### Cost Observations

Model mix and session counts are not reliably recorded. COST-01 monthly numeric
totals remain untrusted. The 18.24% local same-workload scheduling improvement is
historical Phase 5 evidence; no global hosted-cost reduction is claimed.
Timeline: eight calendar days from initialized v2.0 to shipped package.

## Milestone: v3.0 — Playstead-ready play and states (closed with deferred scope)

**Closeout:** 2026-11-10, `override_closeout`; not a package shipment.
**Phases:** 3 | **Recorded plans:** 64 | **Requirements:** 7/16 complete.

### What Was Built

Phase 13 delivered portable validated state through the library, runner and
libretro core. Its refreshed verification passed 44/44 and the local CI workflow
passed. The player documentation, package checks, launch contract and host-test
paths also leave reusable local artifacts, but they do not satisfy the open host
acceptance requirements.

### What Worked

Local deterministic checks and explicit phase evidence extracted useful state
functionality without needing external Playstead changes. The archive keeps the
successful state API, the two incomplete phase reports, failed attempts and the
denied owner request distinguishable.

### What Was Inefficient

Phase 12 accumulated many plans around a released-core host path whose remaining
permission request was denied before preflight. That blocked acceptance, but it
did not prevent unrelated core development. Repeatedly treating phase acceptance
as the only useful next action delayed a clear scope choice. Phase 14 similarly
has strong local wiring but no host launch evidence; source-level tests cannot
substitute for that boundary.

### Patterns Established

When an identity-bound host action is denied, preserve the exact stop and evidence,
close the milestone transparently with unmet requirements still unchecked, and
move independent development to a separately verifiable scope. Prefer a single
offline harness over additional owner-run setup when it can answer the question.

### Key Lessons

- A milestone's acceptance blocker is not automatically a project-wide development blocker.
- Override closeout is valid bookkeeping only when failed verdicts, unmet requirements and unrenewed stops stay explicit.
- The next scope should reuse an owner-ranked independent direction and start with the recurring SEED-007 tune-up.

### Cost Observations

No trustworthy per-milestone cost total was available. Do not infer productivity from
64 recorded plans or 100 recorded tasks; only one of three phase goals passed.

## Milestone: v4.0 — Browser/WebAssembly core readiness

**Completed locally:** 2026-11-10. Two phases, two plans, five tasks; all five scoped requirements verified. No package release, hosted dispatch or merge occurred.

### What Was Built

Relative-root budget correctness, the production Emscripten module, direct offline Node/native evidence and required Linux integration, with size and warmed headless reports.

### What Worked

Independent toolchain and harness research led to one vertical slice. Existing fixture/oracles, exact inventories and fault controls caught real loader and evidence bugs. One source-review fix round closed all five findings. Automation and agent document judgment needed no owner UAT.

### What Was Inefficient

Long delegated closeout/report turns and repeated status requests lengthened the critical path. Keep research and reports concise, and return terminal results as soon as required evidence exists. The archive CLI omitted one summary's accomplishments and undercounted tasks; reconcile its output against actual plan blocks rather than repeating planning. A later inventory change made the earlier fingerprint stale; refresh that exact verification before auditing.

### Patterns Established

Hash implementation headers and generated compile inputs, preserve signal failures, invalidate previous receipts, validate every retained export, and mount source read-only. Keep native numeric expectations before cross-artifact hashes and actual-emulation limits next to measurements.

### Cost Observations

No reliable model-cost or hosted pipeline-cost series was collected. Local workload receipts support correctness, not global speedup or browser/device performance. Existing v3.0 acknowledgments and denied/spent scopes remain unchanged.

## Milestone: v5.0 — Downloadable WebAssembly core

**Verified locally:** 2026-11-11; publication pending separate ship authority.
**Phases:** 2 | **Plans:** 3 | **Tasks:** 7

### What Was Built

One source-bound portable libretro ZIP with exact members, digests, identities, pinned runtime notices and a clean-extracted Node/native oracle. The existing publisher and integrity controls cover nineteen ZIPs/twenty assets; native and host-scope boundaries are preserved.

### What Worked

Early primary-source notice qualification and concrete synthetic controls narrowed faults before full gates. Overlapping finished-code review with disjoint documentation work caught extraction ownership/bounds and an older hygiene fixture before the final full batch. A qualified one-slot FIFO limited hygiene scheduling without changing checks or deadlines.

### What Needs Improvement

Routine report/commit closeouts consumed excessive elapsed time, and summary parsers repeatedly mistook command text for file claims. Reuse the known positive-only self-check/Recorded Results pattern and let the orchestrator finish small metadata repairs. Map all exact inventory consumers, including hygiene fixtures, before extending a release set. Keep the optional Markdown-wrapper checker seed visible without expanding into a new parser.

### Evidence and Limits

Final native/release, pinned SDK/extracted consumer, real publisher controls and hygiene passed at the recorded product snapshot. Native notice-path and SDK control-name failures were diagnosed and retained. No reliable hosted optimization series, browser/device evidence or new remote allowance was produced; older failures and fifty carried acknowledgments remain distinct.

## Milestone: v6.0 — YM2151 homebrew sound

**Completed:** 2026-11-12; shipped as v0.14.0 on 2026-11-13. Two phases, eight plans and nineteen actual task blocks. See the [ship receipt](milestones/v6.0-SHIP.md).

### What Was Built

One qualified XM/YM2151 digital profile with permissive integer C17 synthesis, bounded mono audio, full transactional state, genuine legacy compatibility and actual native/Wasm/clean-package consumers. Complete measured cost and accurate evidence limits accompany the feature.

### What Worked

Numeric expectations before hashes, an unchanged frozen seed and genuine kickoff products gave independent checks. Maximum coinstalled topology and the qualified assertion-enabled sanitizer exposed a shared clock-ordering fault that Release equality missed; the new Release monotonic observer now catches it. Two bounded review-fix rounds finished clean.

### What Needs Improvement

The runner hash inventory retained a stale literal count after additive YM rows. Verification bookkeeping also stalled after checks were established; a concise evidence draft and a persistence/fingerprint-only handoff completed it. Persist a minimal evidence outline early and keep later report expansion bounded. The archive moved paths without rebasing verification fingerprints; preserve byte identities, repair links and use the verification owner to refresh only relocation metadata. Collapsing the roadmap also erased the phase-completion record required by shipping. Named checklist declarations then made runtime1.20.0 expect a new live directory. Checked archive identifiers with names in parentheses preserve completion without redeclaring an active phase. Validate both init.phase-op's actual directory and roadmap.analyze's completion record before recommending shipping.

### Evidence and Limits

Required local gates and both phase verifications/integration pass. GCC13 does not qualify for the current no-FP warning set; BE/WASI tools are absent. Qualified Apple sanitizer results and actual Emscripten execution remain separate. No source quota, timeout, cache, extra dependency, remote dispatch or host scope was widened. Original failures and nine deferred v3 requirements remain preserved; no unavailable platform, hardware or publication acceptance is inferred.

## Milestone: v7.0 — Souper homebrew mapper

**Closed locally:** 2026-11-14 with `override_closeout`; not published or merged. Two phases, five plans, ten tasks, all eight requirements passed. One workspace record was newly acknowledged with its `unknown` verdict preserved; 49 earlier acknowledgments carried forward.

### What Was Built

- Added the selected mapper-only Souper software profile with independent generated CPU/MARIA observations, lifecycle and state continuation.
- Exercised the generated diagnostic through the public library, runner, runtime-loaded native libretro module and production portable consumer.
- Added loaded-module snapshot and refusal checks while retaining the existing static observer and descriptor tests.

### What Worked

Generated fixtures and separately authored literal values made the mapping checks independent of the implementation. The integration review found the missing dynamically loaded module path; a bounded test-only gap plan closed it, and ordinary local verification needed no owner UAT.

### What Was Inefficient

The runtime-loaded module route was discovered after static observer coverage was already green. Include every actual consumer boundary in the phase acceptance matrix before implementation. Archiving moved phase files and made earlier fingerprints path-stale; refreshing the coverage paths and digest is sufficient when the only change is relocation metadata.

### Patterns Established

Prove each deliverable through the artifact its consumer loads, and use independent observations plus restore/refusal controls at that boundary. Keep local verification separate from publication, host and physical-hardware evidence. Use cheap checks early where they screen a likely failure; change CI structure only when repeated measurements justify it.

### Key Lessons

1. Static libretro observer coverage does not prove the production module's exported lifecycle; retain the loaded-module check in ordinary CI (MU-TEST-016).
2. A single CI timing sample showed a 45.67-second ordinary phase and a 44.75-second publication control, but did not support changing test coverage, topology or timeouts.
3. After archiving, rebase evidence paths and refresh fingerprints without claiming product tests were rerun.

### Cost Observations

The saved local `ci` run passed 722 ordinary and 8 release tests. Its ordinary phase took 45.67 seconds and the longest observed test, `policy.publish.controls`, took 44.75 seconds. One sample is insufficient for structural CI changes. Session count and model mix are not recorded; no estimate is asserted.

## Milestone: v8.0 — GameDrive streamed-audio contract qualification

**Closed locally:** 2026-11-15 with `override_closeout`; two phases, two plans, four tasks, all five scoped requirements covered by the passing audit. Phase 23's verification fingerprint predates the merged timeout follow-up; the stale fingerprint is accepted under GSD-HANDOFF §3 because the audit covers the phase and exact-main CI passes. No package release or milestone tag was created.

### What Was Built

- Recorded source-qualified CI, Release, hygiene and quality evidence, then qualified GameDrive command/media/timing/state gaps, legal fixture/oracle feasibility, resource limits and existing-consumer consequences.
- Reached an evidence-backed **NO-GO** for implementation because authoritative stream framing/timing, independent exact-profile PCM expectations and finite resource facts remain missing.
- Calibrated only the macOS x64 PAL BIOS predicate deadline after a later exact-source failure; product behavior, test assertions and other lane limits stayed unchanged.

### What Worked

The source ledger separated first-party firmware notes, developer examples, inference and unknowns, so the firmware save/load note did not turn into an invented playback contract. The independent-oracle and license gates produced a clear no-go without using unlicensed sample/encoder code or asking for hardware access. The failed main run was investigated before any retry; one bounded deadline correction was supported by six successful main runs and twelve complete suite timings.

### What Was Inefficient

The `title.pal.bios` timeout also prevented its quality artifact upload, so the downstream exact-set check failed in the same workflow. The root failure was one under-sized test deadline; the quality failure was downstream, not a second product-test defect. The Windows ARM64 serial job remained the main CI critical path at about 42 minutes, but this observation did not justify changing the job graph, cache or coverage.

### Patterns Established

Keep firmware behavior notes distinct from a normative emulation contract. A potential fix or state-save feature motivates qualification, but does not supply stream framing, clocks, PCM expectations or finite work/memory bounds. For hosted runtime changes, use completed comparable lane samples and preserve the original failed receipt; local cross-architecture configuration does not substitute for executing on that architecture.

### Key Lessons

1. A CTest timeout must cover observed work on its exact runner while keeping all existing assertions; here twice the slowest of twelve completed macOS x64 suite samples rounded to a 12-second registration (`MU-CI-021`).
2. Main CI and a successful Release target check do not imply package publication when release-please created no package change; keep package status source-specific.
3. A no-go is a valid qualification result when a normative contract or independent oracle is absent; preserve concrete evidence and reopen only on stated triggers.

### Cost Observations

Hosted v8 delivery runs started within seconds and had no canceled jobs. Windows ARM64 remained the critical path at approximately 42 minutes. The local verification passed 723 CI tests and 8 release checks before merge; exact-main CI and Release receipts are linked in the milestone records. Session count and model mix are not recorded; no cost estimate is asserted.

## Milestone: v9.0 — Publish the corrected Souper mapper package

**Shipped:** 2026-11-16 | **Phases:** 1 | **Plans:** 1 | **Tasks:** 2

### What Was Built

- Published stable package v0.16.0 from exact release source `a0319a9c` through the existing release-please and publisher workflow.
- Preserved the failed v0.15.0 draft and tag byte-for-byte, including its original target and zero-asset state.
- Updated README/status availability and completion claims; closed the phase and milestone records without changing emulator behavior.

### What Worked

Exact-source release checks covered fixtures, package identity, provenance, publication and latest installation. The release receipt kept package-source evidence separate from documentation and closeout merge evidence. The final phase-closeout CI passed on exact main `9b75e6f`; the release workflow confirmed no new package release was needed after the planning-only merge.

### What Was Inefficient

Main CI had a one-second queue but took 48m08s. Windows ARM64 was the critical path at 45m41s, then Wasm32 took 2m16s. No jobs were cancelled. This is one more timing sample, not evidence for changing a timeout, cache or job graph. An initial `docs(25)` closeout PR title failed the repository title gate; the corrected scope-free title passed. Use the tested Conventional Commit form when opening planning-only PRs.

### Patterns Established

Publish existing behavior first, then update version-neutral public availability copy only after the latest-install path passes. Keep the failed release identity as a compared baseline, and record the exact source for each release, documentation and main-CI claim.

### Key Lessons

1. A new published package can be completed without a second release when the downstream merge contains only verified documentation and planning changes.
2. A long Windows ARM64 quality step can still be healthy; inspect the active job and completed phases before considering a retry.
3. One CI run with a longer-than-usual critical path and no queue or cancellation does not justify a structural optimization.

### Cost Observations

Release run 43801428444 took 52m23s; final phase-closeout main CI 43841380444 took 48m08s with a one-second first-job delay. Eighteen CI jobs passed; only the expected push-event title and perf jobs were skipped. Session count, model mix and monetary cost are not recorded.

## Milestone: v10.0 — Headless developer trace

**Shipped:** 2026-11-17 | **Phases:** 2 | **Plans:** 8 | **Recorded tasks:** 16

### What Was Built

- Published stable v0.17.0 with bounded, deterministic runner-only instruction and bus tracing.
- Preserved API 8, ABI 1 and state formats; fixed the GCC and Clang build failures without reducing coverage.
- Archived the phase evidence, source-qualified cost receipt and two acknowledged follow-ups.

### What Worked

The fixed-value trace oracle, output fault controls and trace-on/off state comparisons exercised the real runner path. Cheap compiler and formatting checks caught portability problems before another long matrix. Exact-main CI and Release receipts tied shipping to a single source.

### What Was Inefficient

The active plan count and delivery status in planning state lagged the accepted evidence. The archive scan also found two open records whose verdicts needed explicit preservation. Reconcile the live roadmap, phase summaries, state, and release receipts before writing the final archive.

### Patterns Established

Keep superseded partial plans as history, but derive completion counts from the active plan set. Keep a non-blocking limitation visible in the audit and defer it without changing its underlying finding. A single timing pair can confirm the main wait, but does not justify changing CI structure.

### Key Lessons

1. Run cheap source-specific consistency checks before a long CI matrix; they can isolate a compiler or formatting fault quickly.
2. When a gap-closure plan supersedes a partial plan, update the active plan count and re-run the roadmap analyzer before archival (MU-GSD-029).
3. Keep CI queue, job duration and cancellation evidence tied to the exact source; one run pair is not a basis for a timeout, cache or topology change.

### Cost Observations

Main CI and Release each began about one second after creation. Windows ARM64 was longest at 43.2 and 41.7 minutes respectively; neither run had failures or cancellations. Local session, model and monetary costs are not reliably recorded.

## Milestone: v12.0 — First-use readiness

**Shipped:** 2026-11-20 | **Phases:** 2 | **Plans:** 3 | **Tasks:** 8

### What Was Built

- Updated evidence-backed player, runner and C-host journeys against current shipped behavior.
- Added first-use RetroArch and `muhtari-run` guidance, plus a ROM-free installed C quickstart using CMake and pkg-config.
- Added staged error and cleanup fault controls for the example without changing the public API, emulation behavior or package format.

### What Worked

Cheap formatter, package-metadata, documentation-budget and focused consumer checks isolated issues before the full local suite. The exact-source hosted CI and Release rehearsal tied the corrected first-use changes to a current base; post-merge main CI and stable v0.18.1's latest-install check confirmed delivery. A read-only integration check found all five requirement paths wired and no blockers.

### What Was Inefficient

The old local build tree contained generated `.info` text that hid a clean-checkout publisher mismatch; the first hosted CI and Release attempts exposed it. Check generated metadata in a clean or refreshed tree before relying on local controls. The default-parallel full local workflow also timed out under this host's resource contention, while the serial full suite passed; one local run did not support changing test deadlines or workflow topology.

### Patterns Established

Keep user-facing release references version-free and link exact package evidence in planning records. Use the same small C source in the docs, extracted archive routes and staged consumers. Require representative failures to prove both the diagnostic and cleanup, and emit success only after those checks.

### Key Lessons

1. A generated artifact in a stale build tree can make a publisher control pass against text that a clean checkout no longer produces; refresh or source-check generated metadata before trusting the local result.
2. Cheap consistency checks narrow feedback, but full CI and the exact-source Release gates still establish the supported matrix, provenance, publication and latest-install behavior.
3. Review CI and Release while active and after completion. These runs had no meaningful queue delay or cancellations; Windows ARM64 independent-base evidence dominated wall time, but parallelizing it would change runner cost and evidence provenance. Preserve coverage and require comparable measurements before changing structure, cache or deadlines.

### Cost Observations

The six reviewed corrected-source and main runs passed without failed or cancelled jobs; workflow queue was zero at one-second precision. Corrected PR CI took 47m09s, Release rehearsal 53m57s, Nightly 22m33s, merge-commit CI 46m18s, current-main CI 48m26s and production Release 51m05s. Windows ARM64 independent-base evidence took about 25–27 minutes. The local serial CI suite took 481.59 seconds. The detailed job paths and `act` limits are in the [v12.0 audit](milestones/v12.0-MILESTONE-AUDIT.md#hosted-delivery-and-efficiency-review). Session, model and monetary costs are not recorded.

## Cross-Milestone Trends

| Milestone | Phases | Plans | Requirements | Test evidence | Closeout |
|-----------|--------|-------|--------------|---------------|----------|
| v1.0 | 4 | 73 | 25/25 scoped | 605 regular + 6 archive checks in milestone audit; green published-release run | Override: 15 deferred artifacts and four stale fingerprints |

| v2.0 | 7 | 75 accepted; 82 historical summaries | 27/27 scoped | 673 regular + 6 package checks in final phase; 55 audit integration checks; published-release run | Override: 15 newly deferred, 13 carried, seven stale fingerprints |
| v3.0 | 3 (1 verified) | 64 recorded plans | 7/16 complete | Local CI passed; Phase 13 verified 44/44; Phase 12 accepted 0/4; Phase 14 20/27 | Override: 22 newly acknowledged, 28 carried; nine requirements deferred |

Both milestones required explicit historical-fingerprint overrides and post-merge
publication checks. Repeated output/deadline friction favors existing independent
output paths and measured calibration; aggregate record counts never prove acceptance.

| v4.0 | 2 | 2 plans, 5 tasks | 5/5 scoped | Native and pinned SDK workflows passed; reviews and integration passed | Verified locally; isolated source subsequently shipped through PR47/package v0.12.0; zero new acknowledgments, 50 carried |

| v5.0 | 2 | 3 plans, 7 tasks | 6/6 scoped | Native/release, SDK, extracted package, publisher and one-slot hygiene pass | Verified locally; publication pending; zero new acknowledgments, 50 carried |
| v6.0 | 2 | 8 | 9/9 scoped | Native, sanitizer, portable, package and integration evidence passed | Shipped v0.14.0; zero new acknowledgments, 49 carried |
| v7.0 | 2 | 5 | 8/8 scoped | Native 722/722 ordinary + 8/8 release; focused Release/ASan and hygiene passed | Closed locally with override; 1 newly acknowledged, 49 carried; no publication/merge/tag |
| v8.0 | 2 | 2 plans, 4 tasks | 5/5 scoped | Local 723/723 CI + 8/8 release; final exact-main CI and Release passed; audit passed | Closed locally with override for stale Phase 23 fingerprint; 0 new acknowledgments, 50 carried; GameDrive implementation NO-GO |
| v9.0 | 1 | 1 plan, 2 tasks | 4/4 scoped | Exact-source release/install passed; final main CI and audit passed | Shipped v0.16.0; 0 new acknowledgments, 50 carried; v0.15.0 unchanged |
| v10.0 | 2 | 8 plans, 16 tasks | 8/8 scoped | Source-qualified trace, compatibility, CI and Release passed; integration audit has one non-blocking warning | Shipped v0.17.0; override closeout with 2 new acknowledgments and 50 carried |
| v11.0 | 2 | 5 plans, 10 tasks | 6/6 scoped | Windows x64/ARM64 path acceptance; exact-source CI, Release, 20 assets and latest install passed | Shipped v0.18.0; 0 new acknowledgments, 51 carried |
| v12.0 | 2 | 3 plans, 8 tasks | 5/5 scoped | Local 736 CI + 8 release checks; exact-source CI/Release rehearsal; post-merge CI, v0.18.1 publication and latest install passed | Shipped v0.18.1; override closeout with 1 new acknowledgment and 51 carried |
