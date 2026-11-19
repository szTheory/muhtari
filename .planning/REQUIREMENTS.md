# Requirements: muhtari v12.0 First-use readiness

**Defined:** 2026-11-19  
**Core Value:** 7800 games behave as they do on the console, identically on every platform, from a small MIT-licensed C library that any host can embed.

## v1 Requirements

### Personas and Jobs

- [ ] **PERSONA-01**: A maintainer can trace the primary player, runner/homebrew developer, and C host integrator jobs to current Muhtari deliverables using dated evidence, while distinguishing direct feedback, source-backed needs, and hypotheses.

### Getting Started

- [ ] **PLAYER-01**: A player can install a supported release core and load a lawfully obtained Atari 7800 image in RetroArch using documented prerequisites, steps, expected result, controls, optional BIOS behavior, and support limits.
- [ ] **RUNNER-01**: A runner user can run the built-in test card or probe/run their own cartridge using the current release or documented local build, understand expected output and refusal status, and find the full CLI reference.
- [ ] **EMBED-01**: A C host integrator can build and run a complete minimal example against the installed library package using CMake or pkg-config, with the required allocator, buffers, one frame, and expected result explained.

### Documentation Quality

- [ ] **DOCS-01**: Users can navigate current first-use and status documentation whose local links, release references, package/API claims, and commands match the shipped artifacts and source; touched comments explain intent without redundant narration and hardware claims remain sourced.

## v2 Requirements

Deferred to future work; not part of this milestone.

### Further User Research

- **RESEARCH-01**: Muhtari can validate these jobs through new external user interviews or moderated usability sessions. Existing personas remain evidence-labeled hypotheses where no direct feedback exists.

### Gameplay Evidence

- **GAMEPLAY-01**: Muhtari can run a finite rights-cleared gameplay replay with an independent expected in-game outcome. Promote only after the exact fixture rights and oracle are established.

### Code and Documentation Debt

- **CLEANUP-01**: Maintainers can audit and retire code/documentation debt outside the first-use paths. Do this as targeted, evidence-backed work rather than a repository-wide subjective rewrite.

## Out of Scope

| Feature | Reason |
|---------|--------|
| New emulation behavior, public API/ABI, or serialized-state changes | The selected outcome is first-use clarity across existing deliverables; no new compatibility decision is approved. |
| GUI, window, audio/input-device or browser-host implementation | These belong to RetroArch, LibFuturo or Playstead, not Muhtari's three deliverables. |
| Atari 2600 emulation | The sibling project `twentysixhundo` owns it; existing recognition/refusal remains. |
| Bundled or newly acquired game/BIOS/save assets | Respect fixture rights and the existing asset policy; use the test card or approved/generated fixtures. |
| Universal game compatibility claim or work on all 201 untested titles | Untested ledger rows are not defect reports. |
| Broad source comment/style sweep or a general prose linter | Keep quality work scoped to user journeys and recurring, testable faults. |
| GameDrive/BupChip implementation or Windows CI structural changes | Existing source/oracle/resource or timing evidence gates remain open. |

## Traceability

| Requirement | Phase | Status |
|-------------|-------|--------|
| PERSONA-01 | Phase 30 | Pending |
| PLAYER-01 | Phase 30 | Pending |
| RUNNER-01 | Phase 30 | Pending |
| DOCS-01 | Phase 30 | Pending |
| EMBED-01 | Phase 31 | Pending |

**Coverage:**
- v1 requirements: 5 total
- Mapped to phases: 5
- Unmapped: 0 ✓

---
*Requirements defined: 2026-11-19*
*Last updated: 2026-11-19 after milestone scope and phase mapping*
