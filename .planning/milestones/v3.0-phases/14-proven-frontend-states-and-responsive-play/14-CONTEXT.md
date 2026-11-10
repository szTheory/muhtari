# Phase 14: Proven frontend states and responsive play - Context

**Gathered:** 2026-11-03
**Status:** Ready for planning

<domain>
## Phase Boundary

Complete FRONT-02, FRONT-03 and FRONT-04: prove save/load, repeated rewind and one supported run-ahead path in the pinned RetroArch host, with visible game progress, deterministic continuation and safe battery persistence. Phase 13's shared state API is shipped. Phase 12 remains halted with its host allowance spent; this phase does not repair Phase 12 or establish Playstead integration.

The accepted Phase 14 requirements explicitly call for actual-host rewind and run-ahead. They are a narrow phase-specific exception to the older host-test limit in DEC.56. Keep the existing bounded RetroArch consumer and its unrelated limits; Phase 14 planning must define a separate, measured host scope. Discussion and planning do not run RetroArch or CI.

</domain>

<decisions>
## Implementation Decisions

### Phase scope and host evidence

- **D-01:** Follow the current Phase 14 roadmap and FRONT-02–04 requirements as the authority for this phase. Extend the existing bounded RetroArch host consumer to establish actual save/load, repeated rewind and one supported run-ahead path. This is a narrow exception to DEC.56; it does not authorize a general frontend test framework or unrelated host expansion.
- **D-02:** Use the existing isolated macOS RetroArch job as the starting point. Its current repository pin is RetroArch 1.22.2 on macOS 15; phase research must revalidate the exact app and core artifacts and the supported mode before planning a run. Keep the job and run within existing measured bounds unless evidence supports a specific, bounded adjustment.
- **D-03:** Phase 12's failed host attempts and spent allowance remain unchanged. Phase 14 must define its own exact host operation and resource bound. Do not reuse or renew the Phase 12 allowance. This discussion authorizes no host, CI, release, publication or Playstead action.

### Player-visible behavior and persistence

- **D-04:** A save/load pass must show both the expected picture and in-game progress after an actual RetroArch restore. A successful process exit or matching state bytes alone is insufficient. Use an admitted owned fixture and an independent visible or numeric oracle; generate state, screenshot and save outputs during the run rather than checking them into Git.
- **D-05:** Rewind evidence must cover repeated restores, and run-ahead evidence must cover one supported RetroArch mode. Verify the Phase 13 input, timing, audio and battery-state contracts, instance isolation, and absence of speculative durable writes. Keep exhaustive state semantics in the existing in-process tests and use the host case to prove the frontend boundary.
- **D-06:** Measure the complete supported run-ahead work and compare it with the same fixture's baseline. Do not invent a numeric performance threshold. Report only the RetroArch version and mode actually demonstrated; do not imply rollback netplay, other hosts, or universal run-ahead support.

### the agent's Discretion

Research and planning select the owned diagnostic fixture, how RetroArch drives save/load/rewind/run-ahead, the independent picture/progress/audio or battery observations, and the smallest reliable run sequence. They also determine whether single-instance or second-instance run-ahead is supportable and how to measure the completed path. Use official RetroArch/libretro documentation and current source; ask the owner only if evidence exposes a material product or resource decision outside these bounds.

No product-choice question remained after applying the owner's standing instruction to follow the recommended path and reconciling Phase 14's explicit criteria with DEC.56.

</decisions>

<canonical_refs>
## Canonical References

**Downstream agents MUST read these before planning or implementing.**

### Approved Phase 14 scope
- .planning/ROADMAP.md — Phase 14 goal, success criteria and scoped acceptance of actual RetroArch state consumers.
- .planning/REQUIREMENTS.md — FRONT-02, FRONT-03 and FRONT-04 acceptance requirements.
- .planning/research/SUMMARY.md — Phase 14 state-consumer research and decisions.
- .planning/research/FEATURES.md — feature ownership and milestone order.
- .planning/research/PITFALLS.md — battery rollback, speculative execution and actual-host evidence pitfalls.
- .planning/research/PLAYSTEAD-HANDOFF.md — keeps RetroArch-managed states separate from Playstead-owned Continue/history.

### State and host contracts
- .planning/phases/13-portable-save-load-across-three-interfaces/13-CONTEXT.md — state compatibility, restore and battery invariants.
- .planning/phases/13-portable-save-load-across-three-interfaces/13-VERIFICATION.md — shipped evidence and the limits of callback-level verification.
- .planning/preparation/DECISIONS.md — DEC.56's earlier two-case host limit; Phase 14 is a narrow exception under the newer accepted requirements.
- .planning/preparation/HOSTS-AND-INTEGRATION.md §1 and §3 — libretro state contract, pinned RetroArch setup and existing host boundary.
- .planning/preparation/ENGINEERING.md §11 — measured bounds and evidence standards.
- ASSET_POLICY.md §§1–3 — allowed fixtures and generated state/save/screenshot handling.
- .planning/LESSONS.md — reproduced project and owner-workflow lessons.

### Existing implementation and tests
- .github/workflows/ci.yml — current pinned RetroArch job and host-test bounds.
- tests/retroarch/run_retroarch.cmake and tests/retroarch/test.cfg.in — isolated host launch, path guards and bounded capture.
- tests/core/test_state.c and tests/libretro/host.c — in-process state continuation and loaded-module callback checks.
- src/state.c and libretro/libretro.c — shared state implementation and frontend callbacks.
- docs/libretro.md — existing state behavior and claims still awaiting actual-host proof.

### Official technical reference
- https://docs.libretro.com/guides/runahead/ — explains RetroArch's run-ahead modes and their use of in-memory save/load; research must verify the relevant behavior against the phase's pinned version.

</canonical_refs>

<code_context>
## Existing Code Insights

### Reusable Assets
- tests/retroarch/run_retroarch.cmake launches the pinned frontend with isolated directories, validates save/run path separation and captures output with a fixed timeout.
- tests/retroarch/test.cfg.in disables host audio output and routes frontend files into isolated job directories.
- tests/diag/dma.a78 and tests/diag/battery.a78 are owned diagnostics already used by the actual-host CI job.
- src/state.c, tests/core/test_state.c and tests/libretro/host.c provide the shipped serializer and in-process behavioral checks.

### Established Patterns
- .github/workflows/ci.yml runs RetroArch 1.22.2 on macos-15, uses separately built core/runner artifacts and checks pixels and battery continuity against independent outputs.
- Actual RetroArch runs are CI steps, not CTest cases. The existing job has a five-minute limit and the launch script has a fifty-second timeout.
- libretro/muhtari_libretro.info.in currently declares basic save-state support. docs/libretro.md says this proves the loaded callback path only, not actual RetroArch, rewind or run-ahead.

### Integration Points
- The libretro callbacks in libretro/libretro.c call the shared library state API.
- The phase can extend the existing retroarch job, its owned diagnostics and output judges; it should not add frontend UI or file I/O to muhtari.
- The frontend owns state files, rewind storage and UI. Muhtari owns deterministic emulation and state callbacks. Playstead remains a separate integration boundary.

</code_context>

<specifics>
## Specific Ideas

- The existing Phase 12 host halt remains in force and is independent of Phase 14's new bounded scope.
- Actual-host proof must expose picture and progress; callback-level tests and state-byte equality are supporting evidence, not the final result.
- RetroArch run-ahead uses in-memory state operations and has more than one mode; choose the one that can be demonstrated cleanly with this core and the pinned host.

</specifics>

<deferred>
## Deferred Ideas

None from this discussion. Phase 14 must not become a Playstead integration, rollback-netplay, other-frontend or general host-testing project.

### Reviewed Todos (not folded)
These existing minor items are outside FRONT-02–04 and remain separate:
- .planning/todos/pending/2026-10-16-close-the-re-review-findings-on-the-dependabot-gate-and-the-publish-checks.md
- .planning/todos/pending/2026-10-16-promote-the-wasm32-lane-into-ci-required.md
- .planning/todos/pending/2026-10-19-close-the-phase-2-re-review-findings-when-maria-and-the-run-loop-land.md
- .planning/todos/pending/2026-10-19-close-the-phase-2-review-seeds-on-the-nightly-filter-and-the-rom-tables.md
- .planning/todos/pending/2026-10-29-keep-the-phase-11-first-review-info-findings.md

</deferred>

---

*Phase: 14-Proven frontend states and responsive play*
*Context gathered: 2026-11-03*
