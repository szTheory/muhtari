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
| 6 | Retail cartridge boards | BOARD-01, BOARD-02, BOARD-03 | Phase 5 | Complete; shipped in PR24 |
| 7 | Battery saves across hosts | SAVE-01, SAVE-02, SAVE-03, SAVE-04, SAVE-05, SAVE-06 | Phase 6 | Complete; 35/35 verified; shipped in PR #28 (merge checks pending) |
| 8 | Controllers at the pins | INPUT-01, INPUT-02, INPUT-03, INPUT-04, INPUT-05 | Phase 7 | Complete |
| 9 | PAL games and audio | PAL-01, PAL-02, PAL-03 | Phase 8 | Ready to execute |
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
**Plans:** 9/9 active plans complete in9 sequential waves; active02–10, historical01 superseded.
**Status:** Complete; independent goal verification passed33/33. BOARD-01–03 complete; shipping remains separate.
Three owned diagnostic identities produce9 complete numeric host observations plus persistent smoke; linked API1–4 smoke clients,
identified Activision AM/OM map/CPU equivalence and original probe identities,
physical Fractalus alias/lifecycle vectors, mandatory all-byte loaded maps and identified AM/OM host equivalence,
full local CI612+6, exact inventories, hygiene and local three-ZIP213899 <=399937
pass;06-09 appends30 validated equality rows after17 unchanged old rows and corrects fixture source pins.
At source25919fe/base617dfa7, calledCI40031550534 passes all six native three-ZIP sums
(worst375263 <=399937), authenticated quality and existing frontend/portability jobs.
Nightly40031550533 passes full native/PowerPC612+6,47-row equality, fuzz/property and vectors.
The accepted contract-based policy, old predicates/pins and ordinary guards persist.
Direct CI40031550434 passes its authenticated aggregate (worst375270 <=399937),
required performance and frontend jobs. Release40031550534 attempt2 passes full
equality, publication/install and confirmed cleanup. All final runs are successful.
**History:** Original01 halt and the665/654-line constructions remain preserved;
the N654 allocation question is superseded, not approved. Actual legacy-base
retirement is demonstrated, including actual direct-PR performance/aggregate acceptance;
the hosted-retirement todo is completed.

Plans:

**Wave 1**
- [x] 06-10-PLAN.md — Prove authenticated additive fixture comparability through the actual producer and unchanged join.

Historical06-01-PLAN.md is superseded;06-01-SUMMARY.md remains the original failed
construction record. It is not an active dependency or a completed BOARD outcome.

**Wave 2** *(ready after completed Wave 1)*
- [x] 06-02-PLAN.md — Run the Absolute tracer through all three deliverables with behavior docs and package measurement.

**Wave 3** *(ready after completed Wave 2)*
- [x] 06-03-PLAN.md — Publish the additive API, linked smoke coverage, exact inventories and embedding contract together.

**Wave 4** *(ready after completed Wave 3)*
- [x] 06-04-PLAN.md — Normalize identified Activision layouts and prove map/probe identity with matching behavior docs.

**Wave 5** *(ready after completed Wave 4)*
- [x] 06-05-PLAN.md — Implement physical Fractalus aliases and independent retail-map/lifecycle vectors.

**Wave 6** *(complete)*
- [x] 06-06-PLAN.md — Admit and execute three owned numeric board diagnostics.

**Wave 7** *(complete)*
- [x] 06-07-PLAN.md — Verify truthful loaded achievement maps and identified-layout host equivalence.

**Wave 8** *(complete)*
- [x] 06-08-PLAN.md — Prove bounded hostile/fuzz, transactional, allocation and instance-isolation behavior.

**Wave 9** *(complete)*
- [x] 06-09-PLAN.md — Accept board portability, behavior evidence and unchanged six-native package bounds.

**Cross-cutting constraints:**
- D-18: each slice measures exactly three component ZIPs against unchanged archive_bytes=399937; six native lanes remain the final authority.

Each wave depends on the previous plan. Plan06-10 has proven the narrow authenticated fixture relation;
current ENGINEERING §11 governs architecture review, with no maintenance-line allocation. The independent archive ceiling remains399937 bytes
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
**Plans:** 9/9 plans complete in 9 sequential waves (parallelization disabled).
**Status:** Complete, 2026-10-24. All nine plans and SAVE-01–06 pass35/35 verified must-haves. Review is clean; exact-source CI/nightly, complete native/PowerPC/WASM evidence, actual fresh-process RetroArch and all six current/base archive sums pass within399937. Shipping is the next separate step.

Plans:

**Wave 1**
- [x] 07-01-PLAN.md — Prove scoped fixture admission and the narrow behavior-metadata transition.

**Wave 2** *(blocked on Wave 1 completion)*
- [x] 07-02-PLAN.md — Execute the HSC embedder tracer, preserve frozen clients and measure real packages early.

**Wave 3** *(blocked on Wave 2 completion)*
- [x] 07-03-PLAN.md — Implement timed pin-level EEPROM, frozen attachment and completion/reset edges.

**Wave 4** *(blocked on Wave 3 completion)*
- [x] 07-04-PLAN.md — Prove battery lifecycle, isolation, no-allocation and bounded I2C properties/fuzz replay.

**Wave 5** *(blocked on Wave 4 completion)*
- [x] 07-05-PLAN.md — Deliver opt-in runner import, firmware provisioning and safe raw-save replacement.

**Wave 6** *(complete)*
- [x] 07-06-PLAN.md — Complete cadence, native controls, coalescing and actual-consumer failure proofs.

**Wave 7** *(complete)*
- [x] 07-07-PLAN.md — Publish load-time SAVE_RAM, optional HSC firmware and exact achievement maps.

**Wave 8** *(complete)*
- [x] 07-08-PLAN.md — Admit one owned combined diagnostic and wire the two-case hosted persistence oracle.

**Wave 9** *(complete)*
- [x] 07-09-PLAN.md — Complete equality/inventories, enable proved claims and accept final native/hosted bounds.

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
**Plans:** 14/14 complete; independent verification49/49 and all INPUT requirements complete.
**Status:** Complete2026-10-25; clean review after one fix round, no owner UAT; shipping in [PR #31](https://github.com/szTheory/muhtari/pull/31). Next GSD step after shipping: `$gsd-discuss-phase 9` in a new session.

Plans:

**Wave 1**
- [x] 08-01-PLAN.md — Prove narrow controller fixture comparability.

**Wave 2** *(complete)*
- [x] 08-02-PLAN.md — Publish additive input and transactional lifecycle.

**Wave 3** *(complete)*
- [x] 08-03-PLAN.md — Prove a timed driving-controller path.

**Wave 4** *(complete)*
- [x] 08-04-PLAN.md — Deliver compact typed runner replay.

**Wave 5** *(accepted after the separate DEC.61 policy recovery)*
- [x] 08-05-PLAN.md — Driving, actual RetroArch and revised package/evidence acceptance pass at62fa325 in CI40139744520. Historical cutoff failures remain; required authenticated measurements and integrity are retained. Historical05 recovery precedes the completed later slices.

**Wave 6** *(complete)*
- [x] 08-06-PLAN.md — TIA input memory and native gun emission verified locally at6fab3e0 (648+6); all historical digest/numeric pins retained.

**Wave 7** *(complete)*
- [x] 08-07-PLAN.md — XG-1 raster boundaries, owned diagnostic and all three hosts pass at9ca62b3 (648+6).

**Wave 8** *(complete)*
- [x] 08-08-PLAN.md — Full-range paddle charge, owned analog oracle and all three hosts pass atf33ad71 (648+6).

**Wave 9** *(complete)*
- [x] 08-09-PLAN.md — Connected keypad matrix, independent records and all three hosts pass at2815568 (648+6).

**Wave 10** *(complete)*
- [x] 08-10-PLAN.md — CX22/ST/Amiga timing, numeric oracles and all three hosts pass at0f590fe (648+6).

**Wave 11** *(complete)*
- [x] 08-11-PLAN.md — Complete finite adapter scans and opposite EEPROM coexistence pass at70465c9 (648+6).

**Wave 12** *(complete)*
- [x] 08-12-PLAN.md — Complete adapter host mappings and executable scans pass at03b7f96 (648+6).

**Wave 13** *(complete)*
- [x] 08-13-PLAN.md — Hostile input, noalloc/isolation, real typed workloads and comparable slice costs pass at38ec251 (648+6).

**Wave 14** *(complete)*
- [x] 08-14-PLAN.md — Exact-source CI/nightly,342-row equality,actual RetroArch and authentic six-native package observations initially pass atdf8f9a6; corrected-source70366fd acceptance and49/49 verification supersede the pre-review source.

Each plan depends on the preceding plan because core, host, diagnostic and evidence owners overlap.
08-01 proves narrow fixture comparability before fixture-dependent acceptance. 08-05 requires the
first independently judged three-deliverable controller slice and fresh exact-source limiting-native
candidate/base three-component packages before hardware expansion. Substantial slices repeat package
measurements;08-14 requires final authenticated six-native current/base observations.
DEC.61 separately retires the historical compressed cutoff after its reproduced
failure. Package integrity, typed measurements and all other contracts remain.

**Success criteria:**

1. Header defaults and host overrides select the devices consistently through append-only API, deterministic runner input and applicable libretro mappings; old callers retain behavior.
2. XG-1 trigger/sensor diagnostics distinguish lit targets, dark/blanked pixels and off-screen aim with fitted timing parameters identified.
3. Paddles and keypad diagnostics exercise dump/charge and matrix behavior at actual access ticks, including the documented timing qualifications.
4. Driving, CX22, ST/Amiga mice, SNES2Atari and Mega7800 diagnostics observe the documented pin sequences and bounded motion/button handling.

### Phase 9: PAL games and audio

**Goal:** Run PAL content at the console timing and report matching video/audio geometry to every host.
**Depends on:** Phase 8
**Requirements:** PAL-01, PAL-02, PAL-03
**Plans:** 1/8 plans executed

Plans:

**Wave 1**
- [x] 09-01-PLAN.md — Safe PAL timing/startup/arithmetic tracer and reachable host buffer/AV contracts.

**Wave 2** *(ready; Wave 1 accepted)*
- [ ] 09-02-PLAN.md — Physical-time devices, lower-raster light and battery lifecycle.

**Wave 3** *(blocked on Wave 2 completion)*
- [ ] 09-03-PLAN.md — Approved warm PAL palette, runner output and movie sessions.

**Wave 4** *(blocked on Wave 3 completion)*
- [ ] 09-04-PLAN.md — Load-stable libretro region, AV, palette and fallback.

**Wave 5** *(blocked on Wave 4 completion)*
- [ ] 09-05-PLAN.md — Exact audio carry, real extremizers and retained quality controls.

**Wave 6** *(blocked on Wave 5 completion)*
- [ ] 09-06-PLAN.md — Narrow PAL fixture admission through the real producer and join.

**Wave 7** *(blocked on Wave 6 completion)*
- [ ] 09-07-PLAN.md — Owned numeric diagnostic and 600-full-frame three-host evidence.

**Wave 8** *(blocked on Wave 7 completion)*
- [ ] 09-08-PLAN.md — Honest capability claims and exact-source portability/package acceptance.

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
| 6. Retail cartridge boards | 9/9 | Complete    | 2026-10-23 |
| 7. Battery saves across hosts | 9/9 | Complete    | 2026-10-24 |
| 8. Controllers at the pins | 14/14 | Complete    | 2026-10-25 |
| 9. PAL games and audio | 1/8 | In Progress | — |
| 10. Homebrew and expansion memory | 0/TBD | Not started | — |
| 11. Full-library acceptance | 0/TBD | Not started | — |

---
*Updated: 2026-10-25; Phases5–8 are complete. Phase8 has14/14 accepted plans,49/49 verified truths and all five INPUT requirements complete. Current-source required CI, full native/PPC/WASM equality, actual RetroArch and authenticated native packages pass; advisory10000-case isolation timeout remains unaccepted and seeded. One review-fix round used, gap round unused. Phase9 has8 checked plans across8 sequential waves and is ready to execute; phases10–11 remain unstarted. Next separate command: $gsd-execute-phase 9.*
