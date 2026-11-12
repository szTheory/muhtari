# Requirements: muhtari v6.0 — YM2151 homebrew sound

**Defined:** 2026-11-11
**Core value:** 7800 games behave as on the console, identically across platforms,
from a small MIT-licensed C library hosts can embed.

## v6.0 Requirements

### Measured milestone tune-up

- [x] **TUNE-10**: Maintainers start feature work from current publication/docs and a source-specific quality/efficiency baseline, with one evidenced small correction or a bounded no-change disposition preserving required checks and historical failures.

### Qualified YM2151 sound

- [ ] **YM-01**: A host can load one explicitly documented YM2151 profile within selected/autodetected XM topology; map, enable, clock, busy/status, timers and established IRQ routing follow a source-qualified digital model. Unsupported topologies/devices refuse before allocation or live-machine mutation.
- [ ] **YM-02**: A homebrew register workload produces independently checked non-silent FM output from an integer, instance-owned, bounded C17 specialization of the approved BSD-3 OPM seed, with exact-pin synthesis parity separately identified from hardware accuracy and all notices/update duties retained.
- [ ] **YM-03**: A host receives deterministic bounded mono PCM and canonical audio observations from the qualified YM workload at supported rates/regions, including partial-frame carry and maximum legal mix; TIA/POKEY-only outputs and existing duplicated-mono libretro output remain unchanged.
- [ ] **YM-04**: A host can reset, unload/reload and run isolated YM instances without stale state, post-load allocation, unbounded work or writes outside borrowed output buffers.
- [ ] **YM-05**: A host can save/restore all YM state mid-stream and obtain identical subsequent audio; corrupt, unsupported or semantically invalid state preserves the live machine and committed battery bytes. Concrete API/behavior/native-format compatibility policy is documented before implementation.
- [ ] **YM-06**: The same owned YM diagnostic executes through the actual public C library, headless runner and loaded libretro core, with independent numeric expectations before cross-route hashes and retained callback/batch/fallback/refusal controls.
- [ ] **YM-07**: The portable libretro artifact and clean extracted/installed package consumers reproduce the owned YM workload's native observations, with package identity, notices and existing integrity/fault controls preserved.
- [ ] **YM-08**: Integrators can identify the supported request, exact profile and evidence limits in existing docs, and maintainers have representative ordinary/worst-legal YM native/Wasm cost, integer headroom, memory/state extent and justified finite bounds using current mechanisms.

## Deferred Requirements

Nine v3.0 gaps remain deferred: TUNE-06/07, PLAY-01–04 and FRONT-02–04. Their
original verdicts and host/remote stops remain binding. Other measured-accuracy
work, homebrew chips, browser/player adoption, ports and AI/TAS retain their seeds.

## Out of Scope

| Feature/action | Reason |
|---|---|
| Full XM services/firmware; extra YM maps | One qualified digital sound profile only |
| Souper/BupChip, Covox/ADPCM, Atari 2600 | Separate devices; 2600 belongs to twentysixhundo |
| Public stereo API, runtime C++/DSP package, second scheduler | Preserve existing public mono and C17 ownership |
| Browser UI, LibFuturo adapters, Playstead/device or physical listening proof | Host/sibling ownership and separate authority |
| Stable shared ABI, generalized cross-build snapshot migration | No new consumer contract selected |
| Retail-game compatibility or calibrated analog accuracy claims | Owned diagnostics and seed parity establish narrower evidence |
| Publication, merge, reopened Phase 12/14 host/remote attempts | Autonomous local authorization does not grant these actions |

## Traceability

| Requirement | Phase | Status |
|---|---|---|
| TUNE-10 | Phase 19 | Complete |
| YM-01 | Phase 20 | Pending |
| YM-02 | Phase 20 | Pending |
| YM-03 | Phase 20 | Pending |
| YM-04 | Phase 20 | Pending |
| YM-05 | Phase 20 | Pending |
| YM-06 | Phase 20 | Pending |
| YM-07 | Phase 20 | Pending |
| YM-08 | Phase 20 | Pending |

**Coverage:** 9/9 requirements mapped exactly once; no orphaned requirements.

Scope follows the saved [proposal](research/next-milestone/PROPOSAL.md),
[synthesis](research/next-milestone/SUMMARY.md) and explicit owner selection plus
autonomous authorization. SEED-003 contributes only YM/XM; SEED-007/009 contribute
the recurring proportional tune-up. Unrelated seed ideas remain deferred.
