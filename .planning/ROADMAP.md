# Roadmap: muhtari

## Milestones

- [x] **v1.0 — 7800 games play with sound in RetroArch** — phases 1–4; shipped 2026-10-21. [Archive](milestones/v1.0-ROADMAP.md)
- [x] **v2.0 — The whole library** — phases 5–11; shipped 2026-10-29. [Archive](milestones/v2.0-ROADMAP.md)
- [x] **v3.0 — Playstead-ready play and states** — phases 12–14; closed 2026-11-10 with deferred scope (`override_closeout`). Phase 13 passed; Phase 12 remains accepted 0/4 and Phase 14 remains `gaps_found` at 20/27; nine requirements remain unchecked. [Audit](milestones/v3.0-MILESTONE-AUDIT.md) · [Archive](milestones/v3.0-ROADMAP.md)
- [x] **v4.0 — Browser/WebAssembly core readiness** — phases 15–16; shipped through PR #47 and package v0.12.0. [Archive](milestones/v4.0-ROADMAP.md) · [Ship receipt](milestones/v4.0-SHIP.md)
- [x] **v5.0 — Downloadable WebAssembly core** — phases 17–18; shipped as v0.13.1 on 2026-11-11. [Archive](milestones/v5.0-ROADMAP.md) · [Ship receipt](milestones/v5.0-SHIP.md)
- [x] **v6.0 — YM2151 homebrew sound** — phases 19–20; shipped as v0.14.0 on 2026-11-13. [Archive](milestones/v6.0-ROADMAP.md) · [Ship receipt](milestones/v6.0-SHIP.md)
- [x] **v7.0 — Souper homebrew mapper** — phases 21–22; closed locally 2026-11-14 with `override_closeout`, all eight requirements passed. This is not a package shipment. [Audit](milestones/v7.0-MILESTONE-AUDIT.md) · [Archive](milestones/v7.0-ROADMAP.md) · [Requirements](milestones/v7.0-REQUIREMENTS.md)
- [ ] **v8.0 — GameDrive streamed-audio contract qualification** — phases 23–24; selected 2026-11-15. Qualification only; no implementation or public compatibility change. [Requirements](REQUIREMENTS.md) · [Recommendation](research/next-milestone/2026-11-15-game-drive-qualification.md)

## Completed phase history

<details>
<summary>✅ v7.0 — Souper homebrew mapper (phases 21–22) — CLOSED LOCALLY 2026-11-14</summary>

- [x] Phase 21: Measured milestone tune-up (1/1 plan) — completed 2026-11-13; TUNE-11 verified.
- [x] Phase 22: Complete Souper homebrew mapper (4/4 plans) — completed 2026-11-14; SOUPER-01 through SOUPER-07 verified.

All five plans have summaries. Native integration found and closed the loaded-module test gap; the final verifier, integration review and local gates passed. One `unknown` workspace record was acknowledged as deferred, with 49 earlier acknowledgments carried forward. See the [milestone audit](milestones/v7.0-MILESTONE-AUDIT.md) for evidence and environment limits.

</details>

## v8.0 — GameDrive streamed-audio contract qualification (in progress)

**Goal:** Determine whether GameDrive streamed ADPCM can be supported on a complete, bounded and lawfully testable contract, while keeping Muhtari's public media/API/state compatibility unchanged until a separate decision.

**Scope boundary:** Phase 23 measures the shipped/release/CI baseline and checks for cheap applicable preflights. Phase 24 qualifies contract facts, lawful generated fixtures and an independent audio oracle, deterministic lifecycle/state needs, resource bounds, and existing-consumer consequences. A no-go is valid if a required source fact or oracle is unavailable. No ADPCM implementation, sidecar API, API/ABI change, state-format change or physical-hardware run is included.

## Phases

- [x] **Phase 23: Measured milestone tune-up** — Establish source-qualified CI, release and hygiene evidence with a bounded no-change disposition. (completed 2026-11-15)
- [ ] **Phase 24: Qualify the GameDrive audio contract** — Produce a source-qualified go/no-go without implementation or public compatibility changes.

### Phase 23: Measured milestone tune-up

**Goal:** Establish the current source-specific release, CI, hygiene and quality baseline and record a low-risk, evidence-backed correction or bounded no-change result, while using cheap applicable consistency checks before longer suites.
**Depends on:** v7.0 local closeout and merged implementation; no unfinished predecessor phase.
**Requirements:** TUNE-12
**Success Criteria** (what must be TRUE):
1. Maintainers can identify the exact current main/release source, publication state, required and report-only gates, prior failure receipts and applicable reproduced lessons without mistaking a later source pass for acceptance of the older release SHA.
2. Queue time, critical-path job durations, canceled work and the most significant current bottleneck are recorded from comparable CI evidence; timeout, cache, topology or parallelism changes are made only with sufficient repeated evidence.
3. Cheap, deterministic consistency checks that apply to the upcoming qualification work are identified and run before any longer local suite/process; failures are investigated before retry and no required workload or fault coverage is weakened.
4. The phase records one low-risk, reproduced correction or a bounded no-change disposition with commands, exit statuses, source identity and run URLs.
**Plans:** 1/1 plans complete
- [x] 23-01-PLAN.md

### Phase 24: Qualify the GameDrive audio contract

**Goal:** Produce a source-qualified go/no-go for a future GameDrive streamed-ADPCM feature, including legal independent-fixture feasibility, finite resource needs, state continuation and compatibility consequences, without implementing the feature or choosing a public API/state change.
**Depends on:** Phase 23
**Requirements:** GDA-01, GDA-02, GDA-03, GDA-04
**Success Criteria** (what must be TRUE):
1. The evidence ledger gives a source revision, evidence class, confidence and status for each needed command/register, track/sidecar identity, stream framing, playback clock, output/mixing, error, pause/resume/reset and save/load field, clearly separating facts, inference, unknowns and contradictions.
2. The work establishes whether generated legal fixtures can exercise the documented profile with expected PCM authored independently of production code; if not, the no-go names the missing normative details or permissions and does not use unlicensed sample/encoder code or copyrighted game data.
3. A bounded design note evaluates media ownership and identity, mono output, deterministic lifecycle/state continuation, memory/work bounds, refusal behavior and existing library/runner/libretro/portable consumers. It lists possible versioned API/state consequences without selecting or implementing them.
4. The final recommendation states go/no-go, strongest alternative, rejected options, compatibility risks and concrete evidence that would reopen the decision. Missing authority or a future hardware run is not inferred as passed.
**Plans:** Not planned

**Current evidence:** RetroHQ firmware 26.01.2266 notes a potential ADPCM playback-glitch fix and save/load support for ADPCM audio. The release note does not specify the complete stream timing/framing or an independent expected-PCM contract. The sample and encoder repositories remain unlicensed. See the [qualification review](research/next-milestone/2026-11-15-game-drive-qualification.md).

**Progress:** Phase 23 complete; Phase 24 not started (1/2 phases complete). Phase numbering continues after v7.0 Phase 22.

## Next boundary

Continue Phase 24 through `$gsd-autonomous`. The owner has authorized ordinary in-scope recommendation following, Git publication, PR merge and required CI/release automation at natural delivery points. Preserve the v0.15.0 exact-source release stop, original Phase 12/14 stops, nine deferred v3.0 requirements and all unaccepted hardware/platform claims.
