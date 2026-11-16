# Roadmap: muhtari

## Milestones

- [ ] **v10.0 — Headless developer trace** — phases 26–27; selected 2026-11-16.

- [x] **v9.0 — Publish the corrected Souper mapper package** — shipped as v0.16.0 and archived 2026-11-16. [Audit](milestones/v9.0-MILESTONE-AUDIT.md) · [Archive](milestones/v9.0-ROADMAP.md) · [Requirements](milestones/v9.0-REQUIREMENTS.md) · [Product roadmap snapshot](milestones/v9.0-PRODUCT-ROADMAP.md)

- [x] **v1.0 — 7800 games play with sound in RetroArch** — phases 1–4; shipped 2026-10-21. [Archive](milestones/v1.0-ROADMAP.md)
- [x] **v2.0 — The whole library** — phases 5–11; shipped 2026-10-29. [Archive](milestones/v2.0-ROADMAP.md)
- [x] **v3.0 — Playstead-ready play and states** — phases 12–14; closed 2026-11-10 with deferred scope (`override_closeout`). Phase 13 passed; Phase 12 remains accepted 0/4 and Phase 14 remains `gaps_found` at 20/27; nine requirements remain unchecked. [Audit](milestones/v3.0-MILESTONE-AUDIT.md) · [Archive](milestones/v3.0-ROADMAP.md)
- [x] **v4.0 — Browser/WebAssembly core readiness** — phases 15–16; shipped through PR #47 and package v0.12.0. [Archive](milestones/v4.0-ROADMAP.md) · [Ship receipt](milestones/v4.0-SHIP.md)
- [x] **v5.0 — Downloadable WebAssembly core** — phases 17–18; shipped as v0.13.1 on 2026-11-11. [Archive](milestones/v5.0-ROADMAP.md) · [Ship receipt](milestones/v5.0-SHIP.md)
- [x] **v6.0 — YM2151 homebrew sound** — phases 19–20; shipped as v0.14.0 on 2026-11-13. [Archive](milestones/v6.0-ROADMAP.md) · [Ship receipt](milestones/v6.0-SHIP.md)
- [x] **v7.0 — Souper homebrew mapper** — phases 21–22; closed locally 2026-11-14 with `override_closeout`, all eight requirements passed. This is not a package shipment. [Audit](milestones/v7.0-MILESTONE-AUDIT.md) · [Archive](milestones/v7.0-ROADMAP.md) · [Requirements](milestones/v7.0-REQUIREMENTS.md)
- [x] **v8.0 — GameDrive streamed-audio contract qualification** — phases 23–24; closed locally 2026-11-15 with `override_closeout`, all five requirements covered by the audit. GameDrive implementation readiness is NO-GO. [Audit](milestones/v8.0-MILESTONE-AUDIT.md) · [Archive](milestones/v8.0-ROADMAP.md) · [Requirements](milestones/v8.0-REQUIREMENTS.md)

## Completed phase history

<details>
<summary>✅ v9.0 — Publish the corrected Souper mapper package — SHIPPED 2026-11-16</summary>

- [x] Phase 25: Publish the corrected Souper mapper (1/1 plan) — all four requirements verified.

Stable v0.16.0 was published from exact source `a0319a9c29886d776e37b818d38d36fe810f486d`; the failed v0.15.0 tag and draft were preserved. Exact-source CI [43801428495](https://github.com/szTheory/muhtari/actions/runs/43801428495) and Release [43801428444](https://github.com/szTheory/muhtari/actions/runs/43801428444) passed. README/status PR #73 and phase closeout PR #74 merged; their earlier main CI [43841380444](https://github.com/szTheory/muhtari/actions/runs/43841380444) and Release [43841380334](https://github.com/szTheory/muhtari/actions/runs/43841380334) receipts remain historical. Archive PR #75 merged source `bac988bea6a4ec070943c11b83298ac1eee5a8c3`, with passing CI [43854355858](https://github.com/szTheory/muhtari/actions/runs/43854355858) and Release [43854356086](https://github.com/szTheory/muhtari/actions/runs/43854356086). Post-archive state PR #76 established current source `89b6ff82812cd5be873e9f90d32ac55fded8694b`, with passing CI [43866551295](https://github.com/szTheory/muhtari/actions/runs/43866551295) and Release [43866551368](https://github.com/szTheory/muhtari/actions/runs/43866551368); package work was skipped because no new release was created. See the [Phase 26 tune-up report](phases/26-measured-milestone-tune-up/26-TUNEUP.md), [audit](milestones/v9.0-MILESTONE-AUDIT.md) and [release receipt](milestones/v9.0-phases/25-publish-the-corrected-souper-mapper/25-RELEASE.md).

</details>

<details>
<summary>✅ v7.0 — Souper homebrew mapper (phases 21–22) — CLOSED LOCALLY 2026-11-14</summary>

- [x] Phase 21: Measured milestone tune-up (1/1 plan) — completed 2026-11-13; TUNE-11 verified.
- [x] Phase 22: Complete Souper homebrew mapper (4/4 plans) — completed 2026-11-14; SOUPER-01 through SOUPER-07 verified.

All five plans have summaries. Native integration found and closed the loaded-module test gap; the final verifier, integration review and local gates passed. One `unknown` workspace record was acknowledged as deferred, with 49 earlier acknowledgments carried forward. See the [milestone audit](milestones/v7.0-MILESTONE-AUDIT.md) for evidence and environment limits.

</details>

<details>
<summary>✅ v8.0 — GameDrive streamed-audio contract qualification (phases 23–24) — CLOSED LOCALLY 2026-11-15</summary>

- [x] Phase 23: Measured milestone tune-up (1/1 plan) — completed 2026-11-15; TUNE-12 verified.
- [x] Phase 24: Qualify the GameDrive audio contract (1/1 plan) — completed 2026-11-15; GDA-01–04 verified.

The milestone audit and both phase reports passed. The Phase 23 verification fingerprint predates its merged timeout follow-up; this closeout accepts that stale fingerprint under GSD-HANDOFF §3 after the milestone audit and exact-main CI pass (`override_closeout`). Implementation readiness is **NO-GO** because the authoritative stream profile and independent exact-profile PCM oracle remain incomplete. No product implementation, public compatibility change or hardware claim was made. See the [audit](milestones/v8.0-MILESTONE-AUDIT.md), [qualification](milestones/v8.0-phases/24-qualify-the-gamedrive-audio-contract/24-QUALIFICATION.md) and [timeout follow-up](milestones/v8.0-phases/23-measured-milestone-tune-up/23-CI-TIMEOUT-FOLLOWUP.md).

</details>

## v10.0 — Headless developer trace

**Milestone goal:** Homebrew authors and Muhtari maintainers can diagnose Atari 7800 instruction and bus timing with a bounded, reproducible headless trace from `muhtari-run`.

The owner selected this scope on 2026-11-16. The [current requirements](REQUIREMENTS.md) and [successor review](research/next-milestone/2026-11-16-future-roadmap-review.md) govern this milestone; the older research SUMMARY is historical and its API 7 baseline is superseded by public API 8.

## Phases

- [x] **Phase 26: Measured milestone tune-up** - Preserve trustworthy release and CI evidence while making only measured, low-risk efficiency corrections. (completed 2026-11-16)
- [ ] **Phase 27: Headless developer trace** - Deliver one filtered, bounded instruction and bus-access trace with numeric acceptance and unchanged emulation results.

## Phase Details

### Phase 26: Measured milestone tune-up

**Goal**: Maintainers can begin trace work from a current, source-qualified release and CI baseline, with avoidable workflow friction corrected where the evidence supports it.
**Depends on**: Phase 25 (v9.0 shipped and archived)
**Requirements**: TUNE-14
**Success Criteria** (what must be TRUE):
  1. Maintainers can identify the current release and CI sources and see their queue time, critical path, job durations and cancellations, with failed or incomplete runs retaining their original limits.
  2. Maintainers can see which reproduced lessons and applicable cheap consistency checks were considered before longer work, including fixture identity, changed-file formatting and shared inventories where relevant.
  3. Maintainers receive an evidence-backed low-risk correction or a bounded no-change disposition; required coverage, fault detection and existing safety stops remain intact, and structural, cache or timeout changes require comparable multi-run evidence.
**Plans**: 3/3 plans complete

Plans:
- [x] 26-01-PLAN.md — Reconcile release and CI receipts and record the bounded tune-up disposition.
- [x] 26-02-PLAN.md — Reconcile roadmap execution progress while verification remains pending.
- [x] 26-03-PLAN.md — Represent future Phase 27 as explicitly unplanned without debt markers.

### Phase 27: Headless developer trace

**Goal**: Developers can inspect deterministic Atari 7800 instruction and bus-access events through the existing headless runner without changing emulated results or public compatibility.
**Depends on**: Phase 26
**Requirements**: TRACE-01, TRACE-02, TRACE-03, TRACE-04, TRACE-05, TRACE-06, TRACE-07
**Success Criteria** (what must be TRUE):
  1. A developer can opt in to a trace for a selected invocation frame and inclusive address range and receive only matching instruction and bus-access events in deterministic order. Records expose event kind, master tick, beam position and CPU context; bus records also expose address, direction and value.
  2. A developer can set a finite record limit and distinguish complete output from explicit truncation or a file-output failure. Invalid frame, range and limit arguments are refused before execution.
  3. A developer can reproduce a generated or owned diagnostic's exact numeric records and event order, and tracing enabled or disabled produces identical frame and canonical state hashes.
  4. Existing consumers retain public API 8, ABI 1 and all current state formats. Trace callbacks and output buffers remain outside emulated machine state, instrumentation is selected only for calls with tracing installed, and tracing disabled still passes the existing hooks-off instruction-cost gate.
  5. A developer can follow runner documentation covering invocation, instruction versus bus address filtering, record fields, bounds, truncation and known limits. Committed fixtures and trace evidence contain no ROM, BIOS or save bytes beyond the manifest.
**Plans**: 3 plans
- [ ] 27-01-PLAN.md — Private end-to-end trace and exact numeric timing oracle.
- [ ] 27-02-PLAN.md — Checked capture bounds, argument/path validation and atomic I/O failure controls.
- [ ] 27-03-PLAN.md — CPU-backed hooks-off cost proof, deterministic compatibility and runner documentation.

## Acceptance and boundaries

Phase 27 includes the complete runner workflow, generated/owned numeric acceptance, deterministic no-change proof, the existing hooks-off cost gate, accurate documentation and required review. There is no separate implementation-tail, UAT or release phase. Acceptance uses existing C17, CMake/CTest and CI mechanisms; no GUI, device code, new dependency or real-host validation is selected.

Keep instrumentation runner-private. If a public observer API or compatibility change proves necessary, stop for an explicit compatibility decision before expanding scope. Existing state formats remain unchanged, including legacy format 1 / behavior 6, YM format 2 / behavior 7 and Souper format 3 / behavior 8.

The nine deferred v3.0 requirements and all original Phase 12/14 host/remote stops, denied requests, spent allowances, expired windows and no-retry limits remain binding. This milestone renews none of them. GameDrive implementation remains NO-GO pending its authoritative stream profile, lawful independent PCM oracle and finite resource facts. LibFuturo owns browser/host work, Playstead owns its player integration, and twentysixhundo owns Atari 2600. Broader SEED-004 tools and title/gameplay expansion remain deferred.

## Progress

**Execution order:** Phase 26 → Phase 27. Plan counts are assigned during phase planning; no implementation or verification is claimed at roadmap creation.

| Phase | Plans Complete | Status | Completed |
|-------|----------------|--------|-----------|
| 26. Measured milestone tune-up | 3/3 | Complete    | 2026-11-16 |
| 27. Headless developer trace | 0/3 | Planned | - |

**Coverage:** All 8 v1 requirements map to exactly one phase: TUNE-14 to Phase 26 and TRACE-01–07 to Phase 27. No orphaned or duplicated requirements.
