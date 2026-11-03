# Roadmap: muhtari

## Milestones

- [x] **v1.0 — 7800 games play with sound in RetroArch** — phases 1–4, shipped 2026-10-21; [archive](milestones/v1.0-ROADMAP.md).
- [x] **v2.0 — The whole library** — phases 5–11, shipped 2026-10-29; [archive](milestones/v2.0-ROADMAP.md).
- [ ] **v3.0 — Playstead-ready play and states** — phases 12–14, approved 2026-10-29; in progress, with Phase 13 complete, Phase 12 halted, and Phase 14 planned.

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
**Plans:** 3/8 active plans executed (4 acceptance plans and 4 active additive gap plans; 12-08 is superseded and excluded from the active count; accepted 0/4)

Plans:

**Wave 1**

- [ ] 12-01-PLAN.md — Prove packaged RetroArch gameplay input with independent visible and retained battery controls.

Execution halted: original run 41056151292 failed before RetroArch; scoped recovery
41062201311 attempt 1/source 28e13e1 reached the host but held input produced idle
pixels. DMA passed; later release/battery controls skipped. Both finite gates are
spent; accepted 0/4 and PLAY-03 remain pending. Preserve the
[canonical halt](phases/12-player-ready-delivery-and-playstead-bridge/12-01-SUMMARY.md)
and diagnose driver/config/event delivery with new evidence before any new bounded
host test or dependent plan.

**Additive gap plans** *(direct ordinary host observation pending; 12-08 is superseded and excluded from the active count; original acceptance halt and successor blocks remain)*

- [x] 12-05-PLAN.md — Published the prior portability repair; its single diagnostic failed Linux preflight before RetroArch and its allowance is spent.
- [ ] 12-06-PLAN.md — Wired the ordinary one-process capture and controls; its single full CI run timed out on Windows ARM64 before RetroArch, so PLAY-03 remains pending and the allowance is spent.
- [ ] 12-07-PLAN.md — Reconcile the new Windows ARM64 timeout and 12-01 commit ledger; permit one ordinary acceptance run only after a causal finding and all local gates pass.
- [ ] 12-08-PLAN.md — Superseded and excluded from the active count; its blocked attempt and evidence remain immutable history.
- [ ] 12-09-PLAN.md — Reconcile current source identities and check local docs readiness; next command is `$gsd-execute-phase 12 --gaps-only --wave 2`, then stop. Wave 3/12-07 requires a separate scope.

**Wave 2** *(blocked on Wave 1 completion)*

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
**Plans:** 6/7 plans executed across 5 sequential waves.
- [ ] 14-07-PLAN.md
- [x] 14-06-PLAN.md
**Wave 1**
- [x] 14-01-PLAN.md

**Wave 2** *(blocked on Wave 1 completion)*
- [x] 14-02-PLAN.md

**Wave 3** *(blocked on Wave 2 completion)*
- [x] 14-03-PLAN.md

**Wave 4** *(blocked on Wave 3 completion)*
- [x] 14-04-PLAN.md

**Wave 5** *(blocked on Wave 4 completion)*
- [x] 14-05-PLAN.md

**Canonical refs:** [accepted requirements](REQUIREMENTS.md), [research](research/SUMMARY.md#phase-14-state-consumers-and-continuation-proof), [feature ownership](research/FEATURES.md), [state/battery pitfalls](research/PITFALLS.md), [hosts §1/3](preparation/HOSTS-AND-INTEGRATION.md#1-libretro-contract), [settled host decisions](preparation/DECISIONS.md#hosts-and-integration), [Playstead handoff](research/PLAYSTEAD-HANDOFF.md), [engineering §11](preparation/ENGINEERING.md#11-docs-standards), [lessons](LESSONS.md).
**UI hint:** no

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
| 12. Player-ready delivery and Playstead bridge | 3/9 | Halted at 12-01 | - |
| 13. Portable save/load across three interfaces | 8/8 | Complete    | 2026-11-01 |
| 14. Proven frontend states and responsive play | 6/7 | In Progress | - |

Next command: from the Phase 14 worktree, run `$gsd-execute-phase 14` to execute the separately owner-approved plan 14-07. Its fresh host attempt has exactly one normal branch push and one `workflow_dispatch` available; preserve run 41618546347 and all prior evidence. Phase 13 is complete. Phase 12 remains halted at 0/4, is not accepted or waived, and all its hosted allowances remain spent; do not resume Phase 12 or reuse those allowances.
