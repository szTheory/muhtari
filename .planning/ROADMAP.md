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
| 5 | Pipeline tune-up | TUNE-01, TUNE-02, TUNE-03, TUNE-04, TUNE-05 | v1.0 | In progress |
| 6 | Retail cartridge boards | BOARD-01, BOARD-02, BOARD-03 | Phase 5 | Not started |
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
**Plans:** 6/9 active plans complete;05-07/08/09 remain unstarted. Recovery05-13 is accepted. Halted05-06/05-11/05-12 stay0/3 histories, superseded for scheduling only.

05-10 restored PowerPC ABI, and its authorized debug continuation fixed evidence-path handling and proved diagnostic-free native/BE 607+6 inventories, two-file equality and release-called required CI at immutable implementation 0901c877.
05-03 is superseded for scheduling; its partial implementation and historical halt records remain.
05-04 delivered the tested read-only digest and guarded publisher; actual default-branch publication remains a ship check. The accepted Windows repair is preserved. 05-05 completed typed ordinary-policy adoption, independent measured bounds and the required exact-artifact aggregate at implementation51a3f95. Direct CI39706507168 and release-called CI39706506980 completed successfully; the latter passed on a same-head retry after an unchanged OpenBIOS timeout. These prior greens do not accept the mixed draft or changed callers. Shared requirements remain pending. Plans05-06/05-11 halted with0/3 tasks each; their RED/draft/count and unrun-acceptance histories remain in their original SUMMARY/HANDOFF files.

Plans:
**Wave 1**
- [x] 05-01-PLAN.md — Audit the complete boundary, prove retained-fault savings and fix scoped allocations.

**Wave 2** *(blocked on Wave 1 completion)*
- [x] 05-02-PLAN.md — Resolve the output race and derive measured test/job bounds.

**Wave 3** *(blocked on Wave 2 completion)*
- [x] 05-10-PLAN.md — Recover PowerPC ABI and prove diagnostic-free full native/BE equality and release-called required CI.

05-03-PLAN.md is superseded by 05-10; its implemented PR compilation and wiring are retained.

**Wave 4** *(complete)*
- [x] 05-04-PLAN.md — Deliver privilege-safe weekly triage and monthly hosted-efficiency reports.

**Wave 5** *(complete)*
- [x] 05-05-PLAN.md — Require one quality ratchet over real observations and trusted baselines.

**Wave 6** *(complete with explicit tooling warnings)*
- [x] 05-13-PLAN.md — Apply the single D-24–26 consumer-owned numeric-read construction, retain compatibility/POKEY audit work and prove strict fit/current local/direct/called acceptance.

05-13 accepted the single D-24–26 consumer-owned read construction at8fd04db and the proven spent POKEY row retirement/docs at31c52d9. Task1 measured14841/quality920; final14840/quality919 fits strict14841 and C14853 with other actuals count368/BE193/digest447/fixes30/reserve0 unchanged. All146 counted paths were compared to5535b57. No additional required counted cost, allocation, waiver, broader construction, gap round or review-limit reset was used. BE12 remains unavailable as reserve. Halted05-12's14842/quality921 and RED94d5c49 remain historical facts; its drafts were adopted selectively, not relabelled accepted.

05-06/05-11/05-12 remain halted histories, now superseded transitively for scheduling by05-13. The earlier05-11 draft measured14879/quality958:9 lines recovered from14888,38 above strict14841/quality920 and26 above C14853. CONTEXT D-09/D-10/D-21–26 retains ordinary aggregate guarantees and legacy semantic guards while deferring domain centralization/store retirement. A waived merged compatibility regression can become the next base's nonpass state; no immutable domain grants, anti-renewal, automatic restoration or unified policy is claimed.

Recovery preserves all seven perf cases/four added assertions, exact forged-SHA rejection, bidirectional fresh ledger/results equality, ordinary aggregate controls and RED/halt lineage. Malformed measured values reject and unavailable paths avoid numeric use. Actual generated-corpus POKEY fast/OpenBIOS replacement passes and accepted/current bases prove the spent row. Final local ci609+6/library43, exact inventories, document budgets and authentic direct9/called8 joins pass. [Direct CI39832530167](https://github.com/szTheory/muhtari/actions/runs/39832530167) attempt2 is completed success with measured comparable perf. [Release39832530253](https://github.com/szTheory/muhtari/actions/runs/39832530253) attempt2 has successful required called CI/quality but overall FAILURE from advisory WASM curl35 fetch; its inventory and rehearsal/install/cleanup skip. One same-head failed-job rerun each preserves initial timeout evidence. Inherited broad diff check exit2 is a scope warning; current-plan/tree checks pass0. No overall Release green, promotion, release rehearsal or final tightening is claimed. Seven assumptions and the descriptorless physical-boundary prohibition remain unresolved; shared requirements remain pending.

**Wave 7** *(ready after accepted05-13; unstarted)*
- [ ] 05-07-PLAN.md — Run unchanged D-12 fresh performance controls and D-13 conditional lane-promotion audit through retained authorities.

**Wave 8** *(blocked on05-07; unstarted)*
- [ ] 05-08-PLAN.md — Correct release signer/API safety and measured rehearsal/recovery claims.

**Wave 9** *(blocked on05-07/05-08; unstarted)*
- [ ] 05-09-PLAN.md — Verify docs/install/settings and tighten final counted acceptance.

**Success criteria:**

1. Maintainers receive one weekly triage digest and its monthly CI-efficiency section from hosted evidence.
2. The reproduced audio timeout and compatibility-output race are resolved under measured bounds; any removed check names its stronger replacement.
3. One ordinary aggregate quality ratchet rejects unexplained regressions; retained performance/compatibility stores and semantic guards have explicit legacy lifecycle limits. Performance arming follows unchanged controls, and gate code meets audited fixed allocations with final tightening and published advisory source ratios.
4. The release docs/install path and repository-setting checks match current behavior, and the measured weakest dimension has a shipped improvement.

### Phase 6: Retail cartridge boards

**Goal:** Run Activision, Absolute and Fractalus cartridges with correct banking and physical RAM behavior.
**Depends on:** Phase 5
**Requirements:** BOARD-01, BOARD-02, BOARD-03
**Plans:** TBD — created by plan-phase.

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
| 5. Pipeline tune-up | 6/9 | Recovery05-13 accepted;05-07 ready and unstarted | — |
| 6. Retail cartridge boards | 0/TBD | Not started | — |
| 7. Battery saves across hosts | 0/TBD | Not started | — |
| 8. Controllers at the pins | 0/TBD | Not started | — |
| 9. PAL games and audio | 0/TBD | Not started | — |
| 10. Homebrew and expansion memory | 0/TBD | Not started | — |
| 11. Full-library acceptance | 0/TBD | Not started | — |

---
*Approved: 2026-10-21; milestone initialization only. Next: `$gsd-discuss-phase 5`.*
