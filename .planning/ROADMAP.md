# Roadmap: muhtari

## Milestones

- [x] **v1.0 — 7800 games play with sound in RetroArch** — phases 1–4, shipped 2026-10-21; [archive](milestones/v1.0-ROADMAP.md).
- [x] **v2.0 — The whole library** — phases 5–11, shipped 2026-10-29; [archive](milestones/v2.0-ROADMAP.md).
- [ ] **v3.0 — Playstead-ready play and states** — phases 12–14, approved 2026-10-29; in progress, with Phase 13 complete, Phase 12 halted at 0/4 accepted, and Phase 14 acceptance blocked at Plan 14-31. Plan 14-29 closed with the timeout cause unresolved. Plan 14-25 remains halted at its failed independent R workflow; its hygiene, frozen-source validation, and owner checkpoint were not reached. Plan 14-27's pinned Darwin arm64 Wasmtime fetch and local CI workflow passed (CTest 695/695; release tests 6/6), but does not clear the Plan 14-25 gate. Plan 14-31's two-invocation diagnostic allowance is spent: the renewed isolated self-test passed with its one-second timeout unchanged, but the historical full-workflow timeout remains unexplained. The owner separately preauthorizes bounded local builds, tests and CI workflows for new work under about one hour; this does not renew a plan-specific cap or authorize host/remote action.

## v3.0 — Playstead-ready play and states

Make ordinary 7800 play easier to reach, then make progress safe to save and load.
Ship useful player documentation and release-qualified RetroArch readiness with a
small demonstrated tune-up first. Deliver one portable machine-state contract
through the library, runner and libretro core, then prove the frontend consumers.
The source baseline is package v0.10.0, API 7, ABI 1 and behavior revision 6;
planning v3.0 is independent of package versions. Seven of 16 requirements are
complete; the remaining Phase 12 and Phase 14 requirements are pending.

### Ownership and external dependency

Muhtari owns its existing three deliverables, release artifacts, launch contract
and actual RetroArch host-readiness evidence. Playstead owns its adapter/installer,
verified two-artifact cache, `a7800` readiness, controller/focus handoff, child
lifecycle and progress UI. Those sibling changes happen in a separate session.

The actual Playstead → RetroArch → muhtari launch/input/orderly-return and
fresh-process battery flow needs paired evidence from the current Playstead
revision and exact host/core pins. It is an explicit external integration
dependency: muhtari readiness can ship before it, but cannot establish that the
Playstead flow passed. Track its status and evidence in the
[handoff](research/PLAYSTEAD-HANDOFF.md); do not mark it complete from a runner,
callback or RetroArch-only result. This dependency does not block independent
muhtari state development. Initial save/load UI belongs to RetroArch;
Playstead-native exact Continue and state history remain later work.

## Phases

- [ ] **Phase 12: Player-ready delivery and Playstead bridge** — Ship clear onboarding, release-qualified host readiness and one measured tune-up, with the sibling handoff explicit.
- [x] **Phase 13: Portable save/load across three interfaces** — Save, validate and restore the complete supported machine through the library, runner and libretro callbacks. (completed 2026-11-01)
- [ ] **Phase 14: Proven frontend states and responsive play** — Demonstrate actual RetroArch save/load, rewind and supported run-ahead without speculative persistent writes.

## Phase Details

### Phase 12: Player-ready delivery and Playstead bridge

**Goal:** Gamers can reach the current playable release easily, and a frontend integrator can use a proven muhtari/RetroArch launch boundary while maintainers ship a small measured reliability or efficiency improvement.
**Depends on:** v2.0 shipped baseline (Phase 11); actual Playstead adapter completion is an external dependency, tracked separately.
**Requirements:** TUNE-06, TUNE-07, PLAY-01, PLAY-02, PLAY-03, PLAY-04
**Success Criteria** (what must be TRUE):
1. A gamer can follow the short current README to find/install the correct core, load a local game by GUI or CLI, understand controls and optional BIOS, and distinguish battery saves, unavailable states and qualified compatibility claims.
2. An integrator can identify and verify the separate RetroArch/core artifacts and use a documented launch/refusal contract for content, per-game saves, optional firmware, usable input and orderly process exit; the source-linked Playstead handoff names sibling responsibilities, current missing paired evidence and native-host promotion triggers.
3. Automated owned-fixture checks using the released libretro core in actual RetroArch demonstrate the expected visible output, delivered input and battery continuity in a fresh process, with their host-readiness scope kept separate from actual Playstead acceptance.
4. Maintainers can demonstrate at least one shipped small improvement to reproduced pipeline friction, with comparable before/after observations and representative pass/fault behavior preserving every required contract; research or debt dispositions alone do not satisfy this outcome.
5. Maintainers can distinguish completed nightly-property work from censored/cancelled attempts and find complete evidence supporting any deadline change, or a bounded explicit deferral retaining all cases, oracles and advisory labels.
**Plans:** 11/18 active plans have recorded outcomes (12-08 is superseded and excluded from the active count; accepted 0/4).

Plans:
- [x] 12-01-PLAN.md — Prove packaged RetroArch gameplay input with independent visible and retained battery controls.

Execution halted: original run 41056151292 failed before RetroArch; scoped recovery
41062201311 attempt 1/source 28e13e1 reached the host but held input produced idle
pixels. DMA passed; later release/battery controls skipped. Both finite gates are
spent; accepted 0/4 and PLAY-03 remain pending. Preserve the
[canonical halt](phases/12-player-ready-delivery-and-playstead-bridge/12-01-SUMMARY.md)
and diagnose driver/config/event delivery with new evidence before any new bounded
host test or dependent plan.

**Additive gap plans** *(direct ordinary host acceptance pending; 12-08 is superseded and excluded from the active count; original acceptance halt and successor blocks remain)*

- [x] 12-05-PLAN.md — Published the prior portability repair; its single diagnostic failed Linux preflight before RetroArch and its allowance is spent.
- [x] 12-06-PLAN.md — Wired the ordinary one-process capture and controls; its single full CI run timed out on Windows ARM64 before RetroArch, so PLAY-03 remains pending and the allowance is spent.
- [x] 12-07-PLAN.md — Reconcile the new Windows ARM64 timeout and 12-01 commit ledger; permit one ordinary acceptance run only after a causal finding and all local gates pass.
- [ ] 12-08-PLAN.md — Superseded and excluded from the active count; its blocked attempt and evidence remain immutable history.
- [x] 12-09-PLAN.md — Reconcile current source identities and check local docs readiness; no runnable acceptance plan is available while 12-01 remains halted. Wave 3/12-07 still needs a separate scope.
- [x] 12-10-PLAN.md — Repair helper trace observability and child-status cutoff locally; add generated ordered-input history and held/released pixel controls; no host or CI action.
- [x] 12-11-PLAN.md — Wave 1, local documentation only: complete launch/refusal prose and missing-artifact disposition; record the bounded property deferral, preserve all eight current registrations and restore the existing docs budget. Original acceptance plans stay blocked; accepted 0/4.
- [x] 12-12-PLAN.md — One authorized ordinary run failed the required hygiene formatter gate before RetroArch. Its one ref, dispatch, and cancellation are spent; it supplied no host evidence.
- [x] 12-13-PLAN.md — One authorized post-format ordinary run passed the candidate package and DMA comparison but failed the input acknowledgment deadline before held/released images; battery checks were skipped and CI required failed. Its one ref and dispatch are spent; PLAY-03 remains open.
- [x] 12-14-PLAN.md — Added both missing registered-test rows and passed the exact configure inventory check. The local portable workflow halted at compilation because the machine has no WASI C-library sysroot; no tests ran and acceptance remains pending.
- [x] 12-15-PLAN.md — Halted before content launch: the qualified helper's nonprompting CoreGraphics event-post preflight returned 77 (`post-event-access=denied`). No reservation or child was created, no causal finding was made, and no permission/settings change or hosted attempt followed. Plan 12-16 remains blocked.

**Recovery Wave 1** *(additive successors; terminal 12-14/15 outcomes remain immutable history; no execution authority is created by planning)*

- [x] 12-17-PLAN.md — Complete the unchanged local wasm32 workflow and portable inventory with the exact hash-verified coherent SDK in private scratch; keep hosted-toolchain/acceptance limits explicit.
- [x] 12-18-PLAN.md — Prepare exact identities and require a new execution-time consent/owner-hardware decision before one bounded local causal diagnostic. Refusal or an inconclusive result halts without a retry.
- [x] 12-20-PLAN.md — Replaced the WASI-incompatible `tmpfile()` with a build-owned fixture; native/WASI owning tests and native CI/format/hygiene passed. The full WASI audit remains halted on `retroarch.states.controls` (direct WASM invocation without Wasmtime) and the existing `runner.movie.v2_edges`/`runner.region.pal` timeouts. No retry or timeout change. This does not clear the 12-18 denial or prove host input.

**Recovery Wave 2** *(requires a complete supported causal finding from 12-18; no permission/content allowance)*

- [ ] 12-19-PLAN.md — Correct only the demonstrated in-scope cause, retain representative fault controls, and pass the complete corrected-source local native/WASI/format/hygiene gates and bounded review.

**Recovery Wave 3** *(12-16 requires complete passing 12-20/18/19 results; 12-20's full WASI prerequisite remains unmet, and permission alone does not establish cause. Its existing one-time future host allowance and every gate remain unchanged.)*

- [ ] 12-16-PLAN.md — After all successor prerequisites pass, use its existing at-most-one source-bound ordinary host attempt; close the former 12-01 halt only on complete revised host proof.

**Original Wave 2** *(12-02 remains blocked until the 12-01 acceptance halt is closed)*

- [ ] 12-02-PLAN.md — Ship a small measured Windows publication-control improvement with complete fault/current-base evidence.

**Wave 3** *(blocked on Wave 2 completion)*

- [ ] 12-03-PLAN.md — Complete the current player/launch contract and source-linked Playstead handoff.

**Wave 4** *(blocked on Wave 3 completion)*

- [ ] 12-04-PLAN.md — Record the exact six-property evidence deferral and its bounded close revisit.

**Canonical refs:** [owner intent](INTENT.md), [accepted requirements](REQUIREMENTS.md), [research](research/SUMMARY.md#implications-for-roadmap), [feature ownership](research/FEATURES.md), [Playstead handoff](research/PLAYSTEAD-HANDOFF.md), [hosts §3–4](preparation/HOSTS-AND-INTEGRATION.md#3-retroarch-on-apple-silicon), [engineering §11](preparation/ENGINEERING.md#11-docs-standards), [tune-up seed](seeds/SEED-007-milestone-tune-up.md), [maintenance seed](seeds/SEED-009-proportional-maintenance-review.md), [property seed](seeds/SEED-010-nightly-prop-calibration.md), [lessons](LESSONS.md#local-player-onboarding-2026-10-29).
**UI hint:** no

**Acceptance boundary:** Reuse the existing README install-line, package identity,
RetroArch screenshot/input/battery and CMake/CTest consumers. New or materially
changed checks name the concrete fault and exercise representative pass/fault
behavior through that consumer. Run recurring regressions in CI and keep one-off
agent judgments as command/exit/run evidence. Select the tune-up from demonstrated
friction in the recurring seeds; optional debt review cannot substitute for an
actual improvement. Use completed comparable work for measurements under DEC.47.
COST-01 monthly totals remain untrusted, WR-01's delayed-release equal-base edge
remains unresolved and fail-closed, and unaccepted property attempts remain
unaccepted unless new scoped evidence changes the disposition. No general audit,
new framework or advisory promotion is implied. The handoff must carry the
external Playstead flow as pending until paired consumer evidence exists.

### Phase 13: Portable save/load across three interfaces

**Goal:** Hosts and runner users can restore a complete compatible machine point safely and continue identically through one bounded library-owned state contract.
**Depends on:** shipped v2.0 baseline
**Requirements:** STATE-01, STATE-02, STATE-03, STATE-04, STATE-05, STATE-06, FRONT-01
**Success Criteria** (what must be TRUE):
1. A library or libretro caller can query a size fixed for the loaded machine and save/restore a complete versioned byte-wise little-endian snapshot in caller-owned buffers, covering mutable supported chip/device/board/region state, pending work and emulated battery bytes; unloaded and failed-load callbacks behave safely and declarations match proven support.
2. A caller can refuse truncated, corrupt, unsupported, incompatible and checksum-valid hostile states before any live mutation, then obtain the same subsequent execution and host-visible storage metadata as if the rejected restore had not occurred.
3. A caller can save, restore and continue each supported machine topology with matching independent semantic observations, frame output and raw/PCM audio against uninterrupted execution; state bytes and continuation agree across the existing required native and big-endian lanes, with advisory evidence still labeled.
4. A host can repeatedly save/load without core allocation after content load or unbounded work, including hostile restored clocks, deadlines and lengths; changed restored battery bytes notify live dirty tracking without restoring an old token, equal/refused restores preserve the appropriate metadata, and speculative work cannot commit durable save files.
5. A runner user can save at a chosen frame, load in a fresh process and hash state for repeatable continuation, with checked file I/O, explicit refusal status and preservation of the live machine and durable files on failure.
**Plans:** 8/8 plans complete in 8 sequential waves.
**Wave 1**
- [x] 13-01-PLAN.md — Complete private visitor and field proofs.

**Wave 2** *(blocked on Wave 1 completion)*
- [x] 13-02-PLAN.md — Shared digests and effective machine identity.

**Wave 3** *(blocked on Wave 2 completion)*
- [x] 13-03-PLAN.md — Public state API and live battery metadata.

**Wave 4** *(blocked on Wave 3 completion)*
- [x] 13-04-PLAN.md — Independent continuation and hostile-state evidence.

**Wave 5** *(blocked on Wave 4 completion)*
- [x] 13-05-PLAN.md — Runner state files, frame points and hashes.

**Wave 6** *(blocked on Wave 5 completion)*
- [x] 13-06-PLAN.md — Libretro serialization and truthful basic claims.

**Wave 7** *(blocked on Wave 6 completion)*
- [x] 13-07-PLAN.md — State and continuation rows in existing equality producers.

**Wave 8** *(blocked on Wave 7 completion)*
- [x] 13-08-PLAN.md — Resource/docs verification and exact-source executed BE evidence.
**Canonical refs:** [accepted requirements](REQUIREMENTS.md), [state research](research/ARCHITECTURE.md), [pitfalls](research/PITFALLS.md), [stack](research/STACK.md), [architecture §7](preparation/ARCHITECTURE.md#7-state-and-saves), [settled decisions](preparation/DECISIONS.md#architecture), [hosts §1/4](preparation/HOSTS-AND-INTEGRATION.md#1-libretro-contract), [state seed](seeds/SEED-002-states-and-movies.md), [lessons](LESSONS.md), [engineering §11](preparation/ENGINEERING.md#11-docs-standards).
**UI hint:** no

**Acceptance boundary:** Extend the existing native tests, seeded hostile-input
checks, runner file controls, loaded libretro-module tests and required
native/big-endian equality producer. Pair split-run equality with an independent
owned semantic oracle so common omissions cannot pass by agreement alone. Cover
wrong media/firmware/effective configuration, complete pending device work,
validate-before-apply preservation, battery bytes versus dirty metadata, and
checked host file failure. Exact byte layout, field inventory, compatibility
bindings and measured bounds belong to phase discussion/design; keep existing
DEC.26/35 contracts and update stale preparation detail against current code.
State format, API, ABI, behavior and battery versions remain separate. No full
recording protocol or new dependency is needed; use existing replay when useful.
Actual frontend save/load and responsiveness claims await Phase 14 evidence.

### Phase 14: Proven frontend states and responsive play

**Goal:** Gamers can save/load, rewind and use the supported run-ahead path in the pinned RetroArch host with deterministic progress and safe persistent storage.
**Depends on:** Phase 13
**Requirements:** FRONT-02, FRONT-03, FRONT-04
**Success Criteria** (what must be TRUE):
1. A gamer can save and load through actual pinned RetroArch and resume the expected picture and progress, demonstrated by an automated owned-fixture round trip using the released core rather than only callback-level agreement.
2. A gamer can use RetroArch rewind through repeated restores while deterministic input, audio, timing and battery semantics remain correct; the frontend owns rewind storage and UI.
3. A gamer can use the supported RetroArch run-ahead path with verified restore/replay and instance isolation, no speculative durable writes, and comparable completed-work performance evidence; capability declarations and user instructions claim only the qualified host path.
**Plans:** 31 total; all 31 have terminal summaries (18 complete, 13 halted). Plans 14-30 and 14-31 each halted after their sole observer-qualification command returned exit 137; both one-shot scopes are spent, and their historical child exit and cleanup outcomes remain unknown. Plan 14-31's attempt receipt records PID 62950; a separately authorized read-only query `ps -p 62950 -o pid=,ppid=,stat=,etime=,comm=` exited 1 with no matching row, establishing only that no process with that PID was present when checked. Its `child.json` and `qualification.json` remain missing; no target observation occurred. No further process inspection, retry or target proposal is authorized. Frontend acceptance remains blocked. Plan 14-28 completed a saved-receipt/source diagnosis with cause unresolved; it launched no test, project workflow, process probe or remote/host action. Plan 14-29 records the separately authorized target-only observation: it passed once in 0.0645173 seconds with its fixture and one-second timeout unchanged, but the predicate process was not observed because the executable-path query outlasted the test. This is a non-reproduction only; the earlier timeout cause remains unknown and the one-run allowance is spent. The owner chose to close this diagnosis unresolved. No further CTest or project workflow is authorized by that scope. Plan 14-25 remains halted before hygiene, frozen-source validation and its owner checkpoint: its sole independent R workflow exited 8 after 694/695 ordinary tests passed, with `title.pin.self_test` timing out at the existing one-second limit and `/bin/ps` denied in workflow stderr. That gate is spent and cannot be retried under the existing scope. Plan 14-27's one local workflow passed but supplies no waiver of the failed independent gate. Plan 14-23's qualification and completed exact-owned cleanup, Plan 14-24's malformed-digest stop and configure failure, Plan 14-26's incompatible-tool failure, and all earlier terminal records remain preserved in their linked plans/summaries. No qualification, Docker pair, native or host allowance is renewed. The earlier twelve-parent-timeout cause also remains unknown; Linux evidence cannot replace native macOS or pinned RetroArch acceptance. Phase 12 remains halted, verification stays `gaps_found` (20/27), and FRONT-02/03/04 remain pending.
- [x] 14-30-PLAN.md — Wave 3: halted at the one-shot qualification; exit 137 left launch and cleanup unknown. No retry or target scope.
- [x] 14-31-PLAN.md — Wave 3: halted after its one-shot receipt-hardening command returned exit 137. The recorded PID is no longer present, but child exit/reap/cleanup proof is missing; no retry or target scope.
- [x] 14-29-PLAN.md — Record the one-run non-reproduction and stop at the separate-scope decision; no CTest or project workflow.
- [x] 14-28-PLAN.md
**Wave 1**
- [x] 14-01-PLAN.md
- [x] 14-16-PLAN.md — Reconciled the twelve saved parent CI timeouts and exact fixture closure. Process listing was denied, so the pre-workload no-overlap gate failed; neither focused command ran, the cause remains unknown, and the parent complete-CI gate stays failed.
- [x] 14-18-PLAN.md — Prepare a source-linked prerequisite decision packet, then stop at the owner environment/authority checkpoint. No probe, workload, host action or allowance renewal is included; Phase 14 acceptance remains blocked.

**Wave 2** *(blocked on Wave 1 completion)*
- [x] 14-02-PLAN.md

**Wave 3** *(blocked on Wave 2 completion)*
- [x] 14-03-PLAN.md

**Wave 4** *(blocked on Wave 3 completion)*
- [x] 14-04-PLAN.md

**Wave 5** *(blocked on Wave 4 completion)*
- [x] 14-05-PLAN.md

**Wave 6** *(blocked on Wave 5 completion)*
- [x] 14-06-PLAN.md

**Wave 7** *(blocked on Wave 6 completion)*
- [x] 14-07-PLAN.md

**Wave 8** *(blocked on Wave 7 completion)*
- [x] 14-08-PLAN.md — Isolate the normal portable replay, preflight exact source/parent, then audit one newly authorized host-enabled attempt; its allowance is spent.

**Wave 9** *(blocked on Wave 8 completion)*
- [x] 14-09-PLAN.md — Replace the retired host line ceiling with reporting and recurring fault controls.

**Wave 10** *(blocked on Wave 9 completion)*
- [x] 14-10-PLAN.md — HALTED after its bounded Task 1: no cause was attributed, no BIOS remedy was applied, and its three-pair cap is spent; terminal halted history.
- [x] 14-12-PLAN.md — Reconcile existing results and recommend a reversible five-test CTest isolation mitigation; records evidence only after completed 14-09, with no diagnostic or test run.

**Wave 11** *(blocked on Wave 10 completion)*
- [x] 14-13-PLAN.md — Add RUN_SERIAL only to the five affected BIOS tests after explicit authority; verify metadata and one focused batch while preserving one-second deadlines. Source/test work followed the evidence review and waited for its exact owner decision.

**Wave 12** *(blocked on Wave 11 completion)*
- [x] 14-11-PLAN.md — Complete local workflows and record the single authorized host attempt; the attempt failed before state-mode RetroArch launch, leaving FRONT-02/03/04 pending. It followed completed 14-12/13 and did not depend on halted 14-10 completion.

**Wave 13** *(blocked on Wave 12 completion)*
- [x] 14-14-PLAN.md — Add source-stage receipt diagnostics and offline prelaunch controls; repair a locally reproduced UDP poll-error fail-open without attributing the earlier host failure. FRONT-02/03/04 remain pending.

**Wave 14** *(blocked on Wave 13 completion)*
- [x] 14-15-PLAN.md — Halted before host qualification because the independent parent CI workflow timed out 12 ordinary tests. The frozen child passed CI/release/hygiene, but the parent gate is required; no remote decision or host work followed.

**Wave 15** *(blocked on Wave 14 completion)*
- [x] 14-17-PLAN.md — Halted before CTest metadata or tests when the single prescribed `ps` attempt was denied (exit 126). No process attribution or timeout comparison was established; FRONT-02/03/04 remain pending.

**Wave 16** *(depends on completed 14-16/14-18 and follows earlier resource-record writers; does not resume halted 14-17)*
- [x] 14-19-PLAN.md — Halted before container creation: Docker access and VM capacity passed, but locally inspected cached images exposed no independently verifiable provenance receipt. No project workload ran and both pair counters remain zero; native macOS and RetroArch acceptance remain separate.

**New prerequisite — computed wave 2** *(completed 14-16/14-18 dependencies only; 14-19 is historical evidence, not a dependency)*
- [x] 14-20-PLAN.md — Halted at the single qualification entrypoint before Docker: the exact pinned Linux arm64 Wasmtime 48.0.6 archive/binary is absent. No container or test ran; the existing pair remains unspent but ineligible, and this plan's qualification/pair commands must not be retried. The next prerequisite is the exact hash-checked Linux artifact and a separately authorized qualification scope. Phase acceptance remains blocked.

**Fresh prerequisite — computed wave 2** *(completed 14-16/14-18 only; halted 14-19/14-20 remain immutable history)*
- [x] 14-21-PLAN.md — Used the sole authorized qualification, then halted before Docker because the revision check ran `git rev-parse` on an extracted rcheevos archive without `.git` metadata and returned the parent worktree commit. Source identity remains unverified; no container or project test ran. Pair remains unspent but ineligible; no retry. A new scope must correct the archive identity check before any further qualification.

**Fresh prerequisite — computed wave 2** *(depends on completed 14-16/14-18; 14-19/14-20/14-21 remain immutable history)*
- [x] 14-22-PLAN.md — Fixed archive-based rcheevos staging and the stop receipt writer; offline controls passed. The one qualification stopped at image inspection because `.Config.Volumes` was absent from the image config. No image identity, container or test was observed; pair is unspent but ineligible. No retry; a new scope must fix the template and authorize another qualification.
- [x] 14-23-PLAN.md — Replaced template image inspection with JSON parsing and passed offline controls. The sole qualification observed the approved image and verified rcheevos, then stopped at container-contract validation with `KeyError: 0` because Docker returned an object where the checker expected an array. No source or pin archive reached the container and no project workload ran. The initial cleanup handler refused, then the exact owned container was removed successfully. Pair remains unspent but ineligible; no retry. New plan and authorization required.

**Fresh diagnostic increment — wave 1**
- [x] 14-24-PLAN.md — Offline admission/ownership controls passed. The fresh local-input gate found a truncated MAME digest before qualification; no active claim, Docker contact or workload start occurred. The single automatic wave-1 CI gate failed during configure because `tests/inventory/ci.txt` is not sorted (exit 1); it cannot be retried, and Plan 14-25 did not start. Scope is N/A/ineligible with no qualification retry.

**Recovery prerequisite — wave 1**
- [x] 14-26-PLAN.md — Corrected only the CI inventory order and preserved the halted 14-24 record. Its one automatic local workflow exited 8 in CTest because the ignored Wasmtime binary is AArch64 Linux ELF on Darwin arm64. Plan 14-26 is halted; no retry under this scope. Plan 14-27's separate automatic local workflow passed; Plan 14-25 subsequently halted at its own independent gate.

**Completed local recovery — wave 1** *(halted 14-24/14-26 remain historical evidence)*
- [x] 14-27-PLAN.md — Replaced only the ignored local Wasmtime artifact through the existing pinned Darwin arm64 fetch and verified identity/preservation read-only. Its single automatic `cmake --workflow --preset ci` gate passed (CTest 695/695; release tests 6/6). Acceptance remains blocked and FRONT-02/03/04 remain pending.

**Halted pinned-host follow-up — wave 2** *(independent native gate spent; owner decision not reached)*
- [x] 14-25-PLAN.md — Named offline receipt controls passed. Its sole independent R workflow exited 8 after 694/695 ordinary tests: `title.pin.self_test` timed out at one second and workflow stderr recorded `/bin/ps` denied. No hygiene, frozen gate, decision packet, remote or host action followed. Cause unknown; no retry.

**Canonical refs:** [accepted requirements](REQUIREMENTS.md), [research](research/SUMMARY.md#phase-14-state-consumers-and-continuation-proof), [feature ownership](research/FEATURES.md), [state/battery pitfalls](research/PITFALLS.md), [hosts §1/3](preparation/HOSTS-AND-INTEGRATION.md#1-libretro-contract), [settled host decisions](preparation/DECISIONS.md#hosts-and-integration), [Playstead handoff](research/PLAYSTEAD-HANDOFF.md), [engineering §11](preparation/ENGINEERING.md#11-docs-standards), [lessons](LESSONS.md).
**UI hint:** no

**Local integration correction:** Root-checkout reconciliation fixed the support report to include
`tests/common/bmp.c`; its existing `retroarch.states.controls` check now covers
both the report and the missing-reader failure. This local workflow check does
not change the blocked Phase 14 acceptance status or authorize a host action.

**Acceptance boundary:** Revalidate exact RetroArch/core pins and supported modes
during phase design. Put exhaustive semantics and isolation in existing in-process
tests; extend the bounded actual-host consumer only for the recurring feature
boundary it must establish. Actual host save/load and supported rewind/run-ahead
evidence are required for their claims; a successful exit or state-byte equality
alone is insufficient. Preserve existing host-test/resource bounds unless a
separate evidence-backed change under ENGINEERING §11 is needed. Measure complete
work for the supported responsiveness path without inventing a platform budget.
Battery rollback remains emulated state, dirty metadata remains live, and no
speculative pass writes durable files. RetroArch-managed states do not establish
Playstead-native Continue or close the external launch dependency by implication.

## Coverage and verification

All 16 approved requirements map exactly once: Phase 12 has six, Phase 13 seven,
and Phase 14 three. [REQUIREMENTS.md](REQUIREMENTS.md#traceability) is the status
ledger: Phase 13's seven requirements are complete; Phase 12's six and Phase 14's
three remain pending. Phase 13's eight plans and implementation are complete.

Every behavior change includes its test in `cmake --workflow --preset ci` and
affected documentation. Preserve required correctness, determinism, hostile-input,
provenance, package identity, release and evidence contracts and the existing
required-versus-advisory lane distinctions. Use CMake `-P`/POSIX sh and existing
mechanisms; add no dependency, quota, generic audit or tests-of-tests framework by
default. Apply ENGINEERING §11 to changed boundaries and new measured resource
bounds. Keep at most two review-fix rounds and one gap round per phase; optional
improvements become seeds, existing required contract violations remain blocking.
Record evidence as command, exit status and run URL, never committed logs.

Full recording, generalized continuation, AI/TAS, browser play, native Playstead
hosting/history and unrelated hardware work retain their
[rolling-horizon](PRODUCT-ROADMAP.md) triggers. Historical scopes, failed attempts,
audit overrides and deferred dispositions remain in their original archives;
this roadmap does not retroactively qualify them.

## Progress

Phase 12's player-readiness evidence and Phase 13's state foundation may proceed
independently from the shipped v2.0 baseline; Phase 14 follows Phase 13. All
approved phase requirements remain mandatory for v3.0 completion. Discuss, plan,
execute and ship are separate GSD steps/sessions under [GSD-HANDOFF §3](preparation/GSD-HANDOFF.md#3-each-phase--one-session-per-step-so-the-model-can-change-between-steps).

| Phase | Plans Complete | Status | Completed |
|---|---|---|---|
| 12. Player-ready delivery and Playstead bridge | 14/19 | Acceptance remains 0/4 and original 12-01 remains halted. Plans 12-14/15/17/18/20 are terminal halted. Plan 12-17's exact local SDK passed the focused WASI mocks, then its full workflow stopped at unsupported `tmpfile()` under `-Werror`. Plan 12-20 replaced that fixture; its native/WASI owning tests, full native CI (700 ordinary tests and 6 release/package checks), formatter and hygiene pass. Its full 622-case WASI workflow still fails `retroarch.states.controls`, which directly executes a WASI module without Wasmtime, and times out `runner.movie.v2_edges`/`runner.region.pal` at their existing 6-second/4-second limits. The exact JUnit audit rejected these three cases; no retry or timeout/source change followed. Plan 12-18's one authorized request and sole follow-up preflight both returned denied; it halted before RetroArch with zero content children, and its request allowance is spent. Plan 12-19/16 remain blocked on 12-18's missing causal result and Plan 12-20's unmet complete-WASI prerequisite; no plan is runnable. The one gap round is used, and 12-02/03/04 remain blocked by 12-01 | - |
| 13. Portable save/load across three interfaces | 8/8 | Complete    | 2026-11-01 |
| 14. Proven frontend states and responsive play | 31/31 | Acceptance blocked. Plan 14-25's sole independent R gate failed on `title.pin.self_test` at one second; workflow stderr recorded denied `/bin/ps`, with cause unknown. No hygiene, frozen gate or owner checkpoint followed; that scope is spent. Plan 14-29 records a passing target-only non-reproduction and the owner's decision to close its diagnosis unresolved. Earlier native/Docker stops, unknown parent timeouts, spent host allowance and Phase 12 halt remain preserved. Verification stays `gaps_found` (20/27); FRONT-02/03/04 pending; no allowance renewed | - |

Phase 14 verification is `gaps_found` (20/27 must-haves verified). In the single authorized run 41733613173, the battery controls passed, but the states driver failed at a compound preflight before media setup or state-mode RetroArch launch; the exact failing member is unknown and no state-mode screenshot or save exists. Plan 14-14 now retains the failing production prelaunch stage in future receipts and fixes a separately reproduced UDP poll-error fail-open; this does not identify the historical failure. Plan 14-15's fresh parent CI timed out 12 ordinary tests (four title and eight Maria DMA cases) while parent hygiene passed. The separately frozen child passed full ordinary/release CI and hygiene, but the parent/source gate did not pass, so no host decision packet or remote qualification was prepared. Plan 14-16 reconciled the exact parent receipt, metadata and fixture closure; its process-list commands were denied, so its focused pair was not launched. Plan 14-17's single prescribed `ps` invocation was also denied (exit 126), so it captured no fresh CTest metadata and launched no project test. The timeout cause remains unknown, and neither plan clears the parent complete-CI gate. The workflow attempts are not retried, and FRONT-02/03/04 remain pending. Plan 14-10's historical three-pair diagnostic cap remains spent and halted; Plan 14-13's narrowly scoped RUN_SERIAL mitigation passed without identifying the old timeout cause. Plan 14-11's host allowance is spent; no retry of either Plan 14-15 complete workflow, cancellation, push, local RetroArch, PR/merge/release, or Phase 12 action is authorized. The advisory WASI build failed at `tmpfile` under `-Werror` and remains advisory. The spent host allowance remains closed. Phase 13 is complete; Phase 12 remains halted at 0/4, unaccepted and unwaived, with its hosted allowances spent.
