# Roadmap: muhtari

## Milestones

- [x] **v1.0 — 7800 games play with sound in RetroArch** — phases 1–4, shipped 2026-10-21; [archive](milestones/v1.0-ROADMAP.md).
- [x] **v2.0 — The whole library** — phases 5–11, shipped 2026-10-29; [archive](milestones/v2.0-ROADMAP.md).
- [ ] **v3.0 — Playstead-ready play and states** — phases 12–14, approved 2026-10-29; planning, no implementation complete.

## v3.0 — Playstead-ready play and states

Make ordinary 7800 play easier to reach, then make progress safe to save and load.
Ship useful player documentation and release-qualified RetroArch readiness with a
small demonstrated tune-up first. Deliver one portable machine-state contract
through the library, runner and libretro core, then prove the frontend consumers.
The source baseline is package v0.10.0, API 7, ABI 1 and behavior revision 6;
planning v3.0 is independent of package versions. All 16 requirements are pending.

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
- [ ] **Phase 13: Portable save/load across three interfaces** — Save, validate and restore the complete supported machine through the library, runner and libretro callbacks.
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
**Plans:** 4 plans

Plans:

**Wave 1**

- [ ] 12-01-PLAN.md — Prove packaged RetroArch gameplay input with independent visible and retained battery controls.

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
**Depends on:** Phase 12
**Requirements:** STATE-01, STATE-02, STATE-03, STATE-04, STATE-05, STATE-06, FRONT-01
**Success Criteria** (what must be TRUE):
1. A library or libretro caller can query a size fixed for the loaded machine and save/restore a complete versioned byte-wise little-endian snapshot in caller-owned buffers, covering mutable supported chip/device/board/region state, pending work and emulated battery bytes; unloaded and failed-load callbacks behave safely and declarations match proven support.
2. A caller can refuse truncated, corrupt, unsupported, incompatible and checksum-valid hostile states before any live mutation, then obtain the same subsequent execution and host-visible storage metadata as if the rejected restore had not occurred.
3. A caller can save, restore and continue each supported machine topology with matching independent semantic observations, frame output and raw/PCM audio against uninterrupted execution; state bytes and continuation agree across the existing required native and big-endian lanes, with advisory evidence still labeled.
4. A host can repeatedly save/load without core allocation after content load or unbounded work, including hostile restored clocks, deadlines and lengths; changed restored battery bytes notify live dirty tracking without restoring an old token, equal/refused restores preserve the appropriate metadata, and speculative work cannot commit durable save files.
5. A runner user can save at a chosen frame, load in a fresh process and hash state for repeatable continuation, with checked file I/O, explicit refusal status and preservation of the live machine and durable files on failure.
**Plans:** TBD — no execution plans yet.
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
**Plans:** TBD — no execution plans yet.
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
ledger; all remain Pending. No phase implementation or plan is complete.

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

Execution order: 12 → 13 → 14. Discuss, plan, execute and ship are separate GSD
steps/sessions under [GSD-HANDOFF §3](preparation/GSD-HANDOFF.md#3-each-phase--one-session-per-step-so-the-model-can-change-between-steps).

| Phase | Plans Complete | Status | Completed |
|---|---|---|---|
| 12. Player-ready delivery and Playstead bridge | 0/4 | Ready to execute | - |
| 13. Portable save/load across three interfaces | 0/TBD | Not started | - |
| 14. Proven frontend states and responsive play | 0/TBD | Not started | - |

Next separate command: `$gsd-execute-phase 12`.
