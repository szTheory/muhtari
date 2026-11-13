# Requirements: muhtari v7.0 — Souper homebrew mapper

**Defined:** 2026-11-13
**Core value:** 7800 games behave as on the console, identically across platforms,
from a small MIT-licensed C library hosts can embed.
**Milestone goal:** Enable homebrew developers to run cartridges using one
published Souper mapper-only software profile, with deterministic CPU/MARIA
mapping and state continuation, while refusing unsupported image/audio forms.

## v7.0 Requirements

### Measured milestone tune-up

- [ ] **TUNE-11**: Maintainers begin the milestone from current release and documentation evidence plus a source-specific quality/efficiency review, and record either one evidence-backed low-risk correction or a bounded no-change disposition without weakening required checks or obscuring prior failures.

### Souper mapper profile

- [ ] **SOUPER-01**: A host can load a Souper-declared image only when its header, mapper declaration, and ROM/EXRAM geometry match the explicitly documented supported profile; malformed, oversized, and unsupported Souper variants are refused before allocation or live-machine mutation.
- [ ] **SOUPER-02**: CPU reads and writes observe the documented banked ROM and EXRAM windows, register decode, aliases, and independent mapping controls, including flat and segmented EXRAM behavior, with boundary and transition cases covered by independent expected bytes.
- [ ] **SOUPER-03**: MARIA fetches observe the documented mapper-specific ROM/EXRAM and character-remap views independently from CPU reads, including the fetch-trap transition through the existing tick-aware bus/HALT path.
- [ ] **SOUPER-04**: Reset, unload/reload, and separate instances deterministically initialize and own all Souper registers and EXRAM; save/restore preserves mapper state and subsequent CPU/MARIA observations, while invalid snapshots are rejected transactionally under a state-format/version policy decided before implementation.
- [ ] **SOUPER-05**: A generated patterned diagnostic checks CPU and MARIA mapping against literal expected values authored separately from production mapping logic, and includes malformed-profile and non-Souper regression controls without relying on copyrighted game images.
- [ ] **SOUPER-06**: The same supported mapper diagnostic runs through the existing public C library, headless runner, loaded libretro core, and applicable portable consumer; existing non-Souper observations remain unchanged, and no BupChip audio is claimed or emitted.
- [ ] **SOUPER-07**: Integrators can identify the exact supported Souper software profile, reset/EXRAM model policy, unsupported image and audio forms, and evidence limits in existing documentation.

## Deferred Requirements

No additional deferred v7.0 requirement is selected. Existing v3.0 gaps remain
deferred with their original evidence and stops. Broader 7800 accuracy work,
other homebrew hardware, browser/player work, and ports remain in
[PRODUCT-ROADMAP.md](PRODUCT-ROADMAP.md) until evidence supports promotion.

## Out of Scope

| Feature/action | Reason |
|---|---|
| Rikki & Vikki compatibility or its retail image layout | Conflicting size/layout claims and no lawful fixture; not needed for mapper developer capability |
| BupChip/BupBoop synthesis, firmware, or audio payloads | Separate audio subsystem; no source-qualified bounded profile selected |
| SUBPAR/SUBLET flash behavior or other Souper variants | Exceeds the one mapper-only profile and lacks a scoped acceptance contract |
| Physical cartridge accuracy claim | Available evidence defines a software model, not calibrated hardware behavior |
| New API/ABI, UI, browser host, dependency, or package-version commitment | Existing C/runner/libretro/portable paths suffice; sibling owns browser host work |
| Publication, merge, remote dispatch, or reopening Phase 12/14 host attempts | These remain separate authorization boundaries and existing stops |

## Traceability

Each requirement maps to exactly one phase in [ROADMAP.md](ROADMAP.md).

| Requirement | Phase | Status |
|---|---|---|
| TUNE-11 | Phase 21 | Pending |
| SOUPER-01 | Phase 22 | Pending |
| SOUPER-02 | Phase 22 | Pending |
| SOUPER-03 | Phase 22 | Pending |
| SOUPER-04 | Phase 22 | Pending |
| SOUPER-05 | Phase 22 | Pending |
| SOUPER-06 | Phase 22 | Pending |
| SOUPER-07 | Phase 22 | Pending |

**Coverage:** 8/8 requirements mapped exactly once; no orphaned requirements.

Scope follows the [v7.0 recommendation and evidence register](research/next-milestone/v7-SOUPER.md).
