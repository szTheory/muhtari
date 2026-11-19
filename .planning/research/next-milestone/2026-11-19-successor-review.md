---
title: "Successor review after v11.0 shipment"
summary: "Reconciles v11.0 release and CI evidence, the next player-value candidate, and a profile-first CI alternative; no successor scope is selected."
read_when: "At the v11.0 closeout and before selecting the next milestone."
updated: 2026-11-19
---

# Successor review — 2026-11-19

**Status:** recommendation only; no next milestone is selected.

## Recommendation

Prefer a small, scenario-level gameplay replay as the next product milestone
once one exact, lawful fixture and an independent expected gameplay result are
identified. Reuse the existing `muhtari-movie 2` and runner support. Limit the
claim to named in-game outcomes; do not infer broad title compatibility from
boot, title screens or frame/audio hashes alone.

The outcome is not ready for implementation today. The compatibility ledger
has 201 pinned titles marked not evaluated, but that means untested, not broken.
The current research does not identify a specific scenario with both fixture
rights and an independent gameplay oracle. Preserve the candidate without
turning that evidence gap into a bug queue or an owner-hardware requirement.

## Strongest alternative: profile the Windows ARM64 CI cost

The exact-source main CI run for v0.18.0 queued for about ten seconds. Windows
ARM64 took 43m46s and its independent-base quality comparison took 26m36s;
the previous successful PR #83 sample took 41m35s and 25m11s. This makes that
comparison a plausible profiling target, not an identified optimization. No
per-test attribution shows which work could safely become cheaper.

Keep run-by-run CI/Release review, cheap source-specific preflights and
compatible local `act` checks as routine project practice. Do not start another
general tune-up or predetermine a cache, topology, parallelism, timeout or
coverage change. Consider a bounded profiling-first scope only if additional
comparable runs or existing stage data can isolate a repeatable cost and the
scope can close without a workflow change when no safe saving appears. Preserve
full current/base inventories, fault controls, source identity, exact-source
Release/equality and publication gates.

The release-please metadata-only fast path already showed about 67–70 Windows
runner-minutes saved per qualifying release PR across four historical
comparisons; the v0.18.0 metadata PR confirmed that fast path while exact-source
Release CI still ran before publication. Do not broaden its exact changed-path
classifier without new evidence.

## Decision comparison

| Candidate | Value | Main risk | Minimum promotion evidence |
|---|---|---|---|
| Finite gameplay replay | Adds a player-visible, repeatable in-game regression signal using existing runner/movie behavior. | Fixture rights or an oracle inferred from the emulator could invalidate the result; hashes can overstate what was tested. | Exact fixture identity and rights; region, device and bounded movie inputs; at least one independent in-game expected state/event; pass and fault controls; no owner host or real-hardware dependency. |
| Windows ARM64 CI profiling | Could shorten repeated developer feedback while retaining every gate. | May measure a non-critical substep or end with another no-change result; an ungrounded cache/topology edit can affect correctness or security. | Comparable hosted source-bound runs that identify a repeated cost, plus a measured test-preserving improvement and unchanged current/base/release coverage. |
| Hold scope | Avoids promoting either an unknown gameplay claim or an unattributed CI cost. | Delays a possible player-quality signal. | Revisit when either candidate meets its evidence gate. |

**Adversarial check:** “201 not evaluated” is not a defect report. A replay
recorded from Muhtari alone is not an independent oracle. A path test moved
earlier by CTest cost metadata improves failure feedback but does not shorten
the whole suite. A successful `act` run cannot establish hosted Windows,
permissions, OIDC, artifacts or publication. These limits rule out broad claims
and speculative shortcuts.

## Evidence and confidence

| Finding | Evidence class / confidence | Source |
|---|---|---|
| v11.0 is shipped as stable v0.18.0; exact-source CI, Release, 20 assets and latest install passed. | Direct release receipts; high | [v11.0 audit](../../milestones/v11.0-MILESTONE-AUDIT.md), [main CI 44606205865](https://github.com/szTheory/muhtari/actions/runs/44606205865), [Release 44606205880](https://github.com/szTheory/muhtari/actions/runs/44606205880) |
| Windows ARM64 and its quality comparison dominate the measured critical path; queue delay is negligible. | Two comparable run samples; medium-high for observed duration, low for a fix hypothesis | [v11.0 audit](../../milestones/v11.0-MILESTONE-AUDIT.md#cicd-efficiency-review), [PR #83 CI 44597303319](https://github.com/szTheory/muhtari/actions/runs/44597303319), main CI above |
| The 201 unevaluated compatibility rows do not establish defects; existing movie files replay typed controller input. | Current project ledger and implementation documentation; high | [compatibility](../../../docs/compatibility.md), [runner movies](../../../docs/runner.md#movies), [consumer review](CONSUMERS.md) |
| No exact lawful fixture plus independent gameplay result is recorded for the proposed scenario. | Bounded evidence review; medium-high for absence in current project memory | [consumer review](CONSUMERS.md), [compatibility ledger](../../../docs/compatibility.md), [SEED-004](../../seeds/SEED-004-tools-observability-ai.md) |
| The metadata-only release fast path has saved about 67–70 Windows runner-minutes across four comparisons. | Recorded historical measurements; high for those runs, not a prediction for other changes | [Phase 28 tune-up](../../milestones/v11.0-phases/28-measured-milestone-tune-up/28-TUNEUP.md#follow-up-release-please-pre-merge-ci-cost) |

The fan-out brief was applied through player/consumer, CI/release operations,
security and adversarial lenses. Additional research would not make either
candidate ready without the missing fixture/oracle or case-level timing data.
Revisit when one of those concrete inputs arrives; retain the small-dependency
and no-new-host-authority constraints.
