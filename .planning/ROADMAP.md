# Roadmap: muhtari

## Milestones

- [x] **v1.0 — 7800 games play with sound in RetroArch** — phases 1–4, archived in [v1.0 roadmap](milestones/v1.0-ROADMAP.md).
- [ ] **v2.0 — The whole library** — phases 5–11, current milestone.

## v2.0 The whole library

### Goal

Expand 7800 library breadth with the remaining in-scope boards, battery saves, controllers and PAL, beginning with a pipeline tune-up. Planning milestone v2.0 is separate from pre-1.0 package semver. Every phase ships a usable release through all three deliverables.

## Scope boundaries

Souper/BupChip stays deferred by owner decision on 2026-10-21. XM here means RAM/POKEY/HSC, not YM2151; AtariVox here means EEPROM, not speech. Full OpenHSC, browser player, save states, rewind/run-ahead, movie recording, 2600 mode, tools and ports remain deferred. Library metadata coverage and verified gameplay are reported separately.

## Phase overview

| Phase | Deliverable | Requirements | Depends on | Status |
|---|---|---|---|---|
| 5 | Pipeline tune-up | TUNE-01, TUNE-02, TUNE-03, TUNE-04, TUNE-05 | v1.0 | Complete |
| 6 | Retail cartridge boards | BOARD-01, BOARD-02, BOARD-03 | Phase 5 | Halted at fit prerequisite |
| 7 | Battery saves across hosts | SAVE-01, SAVE-02, SAVE-03, SAVE-04, SAVE-05, SAVE-06 | Phase 6 | Not started |
| 8 | Controllers at the pins | INPUT-01, INPUT-02, INPUT-03, INPUT-04, INPUT-05 | Phase 7 | Not started |
| 9 | PAL games and audio | PAL-01, PAL-02, PAL-03 | Phase 8 | Not started |
| 10 | Homebrew and expansion memory | BOARD-04, BOARD-05 | Phase 9 | Not started |
| 11 | Full-library acceptance | COMP-01, COMP-02, COMP-03 | Phase 10 | Not started |

## Phase details

### Phase 5: Pipeline tune-up

**Goal:** Make the next capabilities ship through a measured, reliable pipeline with current docs and proportional gates.
**Depends on:** v1.0 closeout
**Requirements:** TUNE-01, TUNE-02, TUNE-03, TUNE-04, TUNE-05
**Status:** Complete — nine active slots accepted, goal verification 20/20; shipped in PR #20.

Plans:
- [x] 05-01-PLAN.md — Audit the boundary, retain savings and allocate additions.
- [x] 05-02-PLAN.md — Isolate freshness writers and measure scheduling/bounds.
- [x] 05-10-PLAN.md — Retain full native/PowerPC equality outside ordinary PR execution.
- [x] 05-04-PLAN.md — Hosted triage/monthly digest and guarded publication.
- [x] 05-05-PLAN.md — Typed ordinary quality policy and independent measured observations.
- [x] 05-13-PLAN.md — Finite aggregate recovery with retained legacy domain guards.
- [x] 05-07-PLAN.md — Arm controlled performance comparison; audit advisory lanes.
- [x] 05-15-PLAN.md — Complete release lifecycle/provisioning and actual hosted acceptance.
- [x] 05-09-PLAN.md — Final docs/install/settings, tightened actual and outcome evidence.

D-35 resumes15 and stages09 final edits before shared acceptance under the approved
15365 maximum; current actual 15287 retires 78 lines. Current direct/called/rehearsal
acceptance succeeds; final phase-goal verification passes 20/20. No new discussion, plan16 or review/gap reset is authorized.
Historical03/06/11/12/08/14 halts and their original evidence remain in their own
artifacts; they do not schedule another execution. Runtime canonical counts may
include halted 14; only the nine active slots above count as accepted outcomes.

**Success criteria:**

1. Weekly triage and monthly CI-efficiency evidence from hosted runs.
2. Measured timeout/workload bounds and repaired audio/freshness reliability, with retained signals.
3. An ordinary aggregate quality ratchet, explicit legacy domain limits, controlled performance arming and audited final line budget.
4. Accurate docs/install/settings and evidenced improvement in the weakest dimension.

### Phase 6: Retail cartridge boards

**Goal:** Run Activision, Absolute and Fractalus cartridges with correct banking and physical RAM behavior.
**Depends on:** Phase 5
**Requirements:** BOARD-01, BOARD-02, BOARD-03
**Plans:** 0/9 plans complete in 9 sequential waves;01 halted before expansion.
**Status:** Disposable construction15952 exceeds15479 by473;02–09 are blocked.
**Bounded redesign:** One independently checked candidate measures15941/net654.
See06-REDESIGN.md for the unapproved N654/C15941 proposal. Await the explicit
allocation decision; no retry/optimization loop or execution is scheduled.

Plans:
**Wave 1**
- [ ] 06-01-PLAN.md — Halted: complete counted construction15952 exceeds15479 by473; installed actual15287 unchanged.

**Wave 2** *(blocked on Wave 1 completion)*
- [ ] 06-02-PLAN.md — Run the Absolute tracer through all three deliverables with behavior docs and package measurement.

**Wave 3** *(blocked on Wave 2 completion)*
- [ ] 06-03-PLAN.md — Publish the additive API, linked smoke coverage, exact inventories and embedding contract together.

**Wave 4** *(blocked on Wave 3 completion)*
- [ ] 06-04-PLAN.md — Normalize identified Activision layouts and prove map/probe identity with matching behavior docs.

**Wave 5** *(blocked on Wave 4 completion)*
- [ ] 06-05-PLAN.md — Implement physical Fractalus aliases and independent retail-map/lifecycle vectors.

**Wave 6** *(blocked on Wave 5 completion)*
- [ ] 06-06-PLAN.md — Admit and execute three owned numeric board diagnostics.

**Wave 7** *(blocked on Wave 6 completion)*
- [ ] 06-07-PLAN.md — Verify truthful loaded achievement maps and identified-layout host equivalence.

**Wave 8** *(blocked on Wave 7 completion)*
- [ ] 06-08-PLAN.md — Prove bounded hostile/fuzz, transactional, allocation and instance-isolation behavior.

**Wave 9** *(blocked on Wave 8 completion)*
- [ ] 06-09-PLAN.md — Accept portability, final actual count and unchanged six-native package bounds.

**Cross-cutting constraints:**
- D-18: each slice measures exactly three component ZIPs against unchanged archive_bytes=399937; six native lanes remain the final authority.

Each wave depends on the previous plan. Complete measured integration/control/comparability fit within
192 net counted lines is a prerequisite in06-01. The independent archive ceiling remains399937 bytes
per native lane's three-component sum; all6 existing native lanes must fit before final acceptance.
Shared core, manifest and inventory ownership requires sequential execution.

**Success criteria:**

1. Activision image layouts produce equivalent mapped reads after identified normalization, with original hashes still available to hosts.
2. Absolute and Fractalus diagnostics exercise fixed/switched banks, initial state and RAM aliases through CPU-visible accesses.
3. Malformed sizes and random bank traffic stay bounded without out-of-range reads or state changes on a refused load.
4. New board fixtures run through library, runner and libretro; prior frame/audio predicates retain their passing behavior.

### Phase 7: Battery saves across hosts

**Goal:** Keep HSC and SaveKey progress across process and frontend restarts without putting file I/O in the core.
**Depends on:** Phase 6
**Requirements:** SAVE-01, SAVE-02, SAVE-03, SAVE-04, SAVE-05, SAVE-06
**Plans:** TBD — created by plan-phase.

**Success criteria:**

1. HSC SRAM and supplied-ROM mapping work; the ROM-free signature stub is explicitly limited to compatible direct-SRAM software.
2. SaveKey/AtariVox EEPROM diagnostics cover repeated START, page wrap, sequential reads and emulated-time busy polling.
3. The load-stable battery span has layout version 1, a correct change generation and power-cycle retention; raw .sav/.srm bytes agree.
4. Runner reload, periodic/on-demand/exit flush and failed atomic-replace tests preserve existing progress, including short/oversized inputs.
5. The second bounded hosted RetroArch case proves a save survives two fresh launches; existing in-process checks carry the detailed semantics.

### Phase 8: Controllers at the pins

**Goal:** Let software use light guns, analog and matrix controllers, quadrature devices and modern controller adapters.
**Depends on:** Phase 7
**Requirements:** INPUT-01, INPUT-02, INPUT-03, INPUT-04, INPUT-05
**Plans:** TBD — created by plan-phase.

**Success criteria:**

1. Header defaults and host overrides select the devices consistently through append-only API, deterministic runner input and applicable libretro mappings; old callers retain behavior.
2. XG-1 trigger/sensor diagnostics distinguish lit targets, dark/blanked pixels and off-screen aim with fitted timing parameters identified.
3. Paddles and keypad diagnostics exercise dump/charge and matrix behavior at actual access ticks, including the documented timing qualifications.
4. Driving, CX22, ST/Amiga mice, SNES2Atari and Mega7800 diagnostics observe the documented pin sequences and bounded motion/button handling.

### Phase 9: PAL games and audio

**Goal:** Run PAL content at the console timing and report matching video/audio geometry to every host.
**Depends on:** Phase 8
**Requirements:** PAL-01, PAL-02, PAL-03
**Plans:** TBD — created by plan-phase.

**Success criteria:**

1. PAL content/override executes 313-line frames with PAL MSTAT/DMA boundaries and documented fast/BIOS hand-off behavior.
2. Library, runner and libretro agree on PAL height, rational frame rate, palette and aspect information.
3. Sustained PAL runs retain correct audio sample carry and identical frame/raw-audio/PCM hashes across supported platforms.
4. NTSC regression fixtures retain their existing output unless an evidence-backed behavior revision explicitly changes it.

### Phase 10: Homebrew and expansion memory

**Goal:** Run BankSet and XBoard/XM memory/POKEY/HSC capabilities using the correct CPU/MARIA ownership.
**Depends on:** Phase 9
**Requirements:** BOARD-04, BOARD-05
**Plans:** TBD — created by plan-phase.

**Success criteria:**

1. BankSet diagnostics exercise separate CPU/MARIA ROM and RAM views on both HALT edges for each supported checked geometry.
2. XBoard and the XM subset respond to documented control registers and banked CPU/MARIA accesses, including integrated HSC/POKEY use.
3. Owned integration fixtures exercise the new board paths through library, runner and libretro with bounded hostile bank traffic.
4. Souper/BupChip, YM2151 and other unimplemented expansion audio remain explicitly unsupported; no incomplete configuration is advertised as supported.

### Phase 11: Full-library acceptance

**Goal:** Account for the pinned library honestly and prove every new compatibility claim without private bytes in CI.
**Depends on:** Phase 10
**Requirements:** COMP-01, COMP-02, COMP-03
**Plans:** TBD — created by plan-phase.

**Success criteria:**

1. Every entry in the pinned CC0 library has an explicit ledger disposition, with untested and unsupported entries distinguished from passes.
2. Each newly claimed board/controller/region has legally admissible automated behavioral evidence and existing passing predicates remain protected.
3. Available private content runs only through the isolated local library preset with command/exit/commit evidence; public CI and releases contain no private bytes.
4. Generated compatibility docs and host capability metadata match the evidence; the whole-library milestone name does not imply universal expansion or title accuracy.

## Coverage and verification

27 approved requirements map to exactly one phase each; no unmapped requirements. Every behavior change lands with tests in the CMake CI workflow and matching docs. Evidence uses command, exit status and hosted run URL; no committed logs. Existing deterministic, bounded-core, no-private-bytes and permissive-source constraints remain in force. Two code-review fix rounds and one gap round per phase; tooling findings remain seeds.

The v1.0 stale verification fingerprints and accepted deferrals remain historical closeout facts. New milestone evidence neither rewrites nor implicitly renews those verdicts.

## Progress

| Phase | Plans complete | Status | Completed |
|---|---|---|---|
| 5. Pipeline tune-up | 9/9 active slots | Complete    | 2026-10-22 |
| 6. Retail cartridge boards | 0/9 | Halted at measured fit | — |
| 7. Battery saves across hosts | 0/TBD | Not started | — |
| 8. Controllers at the pins | 0/TBD | Not started | — |
| 9. PAL games and audio | 0/TBD | Not started | — |
| 10. Homebrew and expansion memory | 0/TBD | Not started | — |
| 11. Full-library acceptance | 0/TBD | Not started | — |

---
*Updated: 2026-10-23; one bounded Phase 6 redesign is complete. Await allocation decision on N654/C15941; only if approved, next: `$gsd-plan-phase 6`. No dependent expansion is authorized.*
