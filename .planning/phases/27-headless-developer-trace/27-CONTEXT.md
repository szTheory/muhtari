# Phase 27: Headless developer trace - Context

**Gathered:** 2026-11-16
**Status:** Ready for planning

<domain>
## Phase Boundary

Add one opt-in, bounded instruction and CPU bus-access trace to `muhtari-run`. A developer selects an invocation frame and inclusive address range, receives deterministic records with master tick, beam position and CPU context, and can tell complete output from truncation or an output failure. Prove exact records and ordering with generated or owned inputs; prove trace-on/off frame and canonical state hashes match; preserve API 8, ABI 1, every state format, and the hooks-off instruction-cost gate. Keep GUI, interactive debugging, symbols, scripting, DMA visualization, emulator-host work, and broader SEED-004 tooling outside this phase.

</domain>

<decisions>
## Implementation Decisions

### Trace selection and event meaning
- Use explicit runner options for a trace file, one invocation frame, and an inclusive 16-bit address range. Number invocation frames from 1, matching `--hash-frame` and `--dump-frame`.
- Apply the range independently to an instruction's starting PC and to each CPU-issued bus access address. Include matching instruction and bus records in the order the emulator observes them; do not sort records after capture. A bus record includes address, read/write direction, value, tick, beam position and CPU context.
- Keep this first trace CPU-focused. Do not claim MARIA DMA or other bus-master coverage; those need a separate event-source contract and remain a later observability option.

### Output and bounds
- Write trace records to a separate file so existing stdout reports retain their current format and order. Prefer a small, versioned JSON Lines record format with numeric machine fields and no new dependency.
- Require explicit frame and range selectors when tracing. Use a finite default record limit and a finite hard maximum; derive the values from the checked memory bound in planning rather than introducing an unlimited mode. Reject malformed, out-of-run frame, reversed/out-of-range address, zero, and over-maximum limit values before emulation.
- When another matching event arrives after the configured record limit, stop capturing but let emulation finish. Mark the trace explicitly as truncated; exact-limit output with no additional matching event remains complete. Treat trace file open/write/close failure as runner I/O failure (exit 1) and ensure an incomplete file cannot be mistaken for complete output.

### Compatibility and deterministic behavior
- Add only a private per-instance observer seam for the runner. Keep callbacks and capture storage outside serialized machine state; do not change the public header, API/ABI, behavior revision, or state formats. If a public observer contract or compatibility version change becomes necessary, stop for an owner decision before expanding scope.
- Reuse the settled dual-compiled traced-loop design: select instrumentation at run entry only when tracing is installed, and retain the existing hooks-off instructions-per-frame limit.
- The generated numeric acceptance must pin event fields and same-tick order. Trace on/off runs use identical invocation inputs and compare every applicable frame hash and canonical state hash. Committed evidence contains no ROM, BIOS or save bytes outside the fixture manifest.

### Reviewed Todos
- The five keyword matches from `todo.match-phase` were checked and excluded: they concern the required Wasm CI lane, Phase 2 ROM/nightly work, a Phase 2 re-review, Dependabot/release checks, and Phase 11 review notes. None is trace work, so none is folded into this phase.

### the agent's Discretion
- Choose exact event struct layout, observer wiring, checked allocation strategy, default and maximum record counts, file replacement mechanics, and the smallest useful generated diagnostic. Follow ENGINEERING §11 for any new bound and preserve existing runner file/path safety patterns.
- Add only recurring tests and checks with regression value; use one-off local checks for evidence that does not merit CI repetition.

</decisions>

<canonical_refs>
## Canonical References

**Downstream agents MUST read these before planning or implementing.**

### Scope and product intent
- `.planning/ROADMAP.md` — Phase 27 goal, acceptance criteria, compatibility limits and deferred work.
- `.planning/REQUIREMENTS.md` — TRACE-01 through TRACE-07 and their completion states.
- `.planning/PROJECT.md` — current product boundary, completion standard and v10.0 goal.
- `.planning/INTENT.md` — owner priorities, saved-understanding policy and autonomous-work preferences.
- `.planning/research/next-milestone/2026-11-16-future-roadmap-review.md` — selected trace direction, strongest alternative, evidence, confidence and revisit triggers.
- `.planning/seeds/SEED-004-tools-observability-ai.md` — wider observability seed; only its narrow trace foundation is selected here.

### Architecture, performance and runner conventions
- `.planning/preparation/README.md` — index of project preparation material.
- `.planning/preparation/ARCHITECTURE.md` §8 — observer design, dual-compiled loop and off-path proof.
- `.planning/preparation/DECISIONS.md` DEC.34 and DEC.58 — hook cost contract and tooling order.
- `.planning/preparation/ENGINEERING.md` §§8, 11 — performance ratchet and architecture/resource-bound review.
- `.planning/preparation/HOSTS-AND-INTEGRATION.md` §§4–5 — runner option/output conventions and observability scope.
- `.planning/preparation/GSD-HANDOFF.md` §3.1 — bounded specialist research and evidence handling.
- `.planning/LESSONS.md` — recent observer, determinism, fixture and cheap-preflight lessons; no trace-specific lesson is currently recorded.
- `docs/runner.md` — current CLI syntax, frame numbering, output and file-safety behavior.

### Live code and verification patterns
- `include/muhtari.h` — public API 8; currently no CPU instruction/bus observer surface.
- `src/frame.c` — bounded frame execution and timing boundary.
- `src/cpu.c` and `src/bus.c` — instruction and CPU bus-access producers.
- `src/internal.h` — per-instance internal state and private implementation contracts.
- `runner/main.c` — command parsing, frame outputs, and runner-owned file I/O.
- `tests/roms/manifest.txt` — only authority for committed ROM-like fixture bytes.
- `tests/runner/` — existing runner CLI and file-output acceptance patterns.

</canonical_refs>

<code_context>
## Existing Code Insights

### Reusable Assets
- `runner/main.c` already owns option parsing, frame numbering, hashes, state output and file failures; extend its existing CLI and path-validation patterns.
- The core uses per-instance `muhtari` state, bounded instruction stepping and internal CPU/bus functions. Keep the new callback and records in private instance/runner plumbing.
- Existing runner numeric/hash tests and generated diagnostic builders can support an exact trace oracle without adding copyrighted media.

### Established Patterns
- The public header defines API 8 and contains no CPU instruction or bus observer. The trace therefore needs a private seam; a public API addition is outside this authorization.
- Existing architecture decision DEC.34 requires selecting the traced loop only when observers are installed and guarding the hooks-off instruction-cost budget.
- Frame and address output options use one-based invocation numbering and inclusive/range validation should happen before the first frame.
- State and audio observations are host-owned and must not become serialized machine state. A trace must preserve the same separation.

### Integration Points
- CPU instruction boundaries are in `src/cpu.c`; CPU accesses pass through `src/bus.c`; frame orchestration and bounded execution are in `src/frame.c`.
- The CLI, capture limit, trace file, ordinary stdout reports and output-path collision checks meet in `runner/main.c`.
- The CMake/CTest runner inventory and CI workflow are the recurring acceptance path for record values, ordering, hash equality and the hooks-off gate.

</code_context>

<specifics>
## Specific Ideas

- The trace should be useful to homebrew authors in scripts as well as when read directly, so use a separate structured file and retain exact numeric fields.
- Report capture truncation explicitly while completing the emulated run, rather than treating the record cap as an emulation stop.
- Keep all trace plumbing absent from the public API and state bytes.
</specifics>

<deferred>
## Deferred Ideas

- Interactive stepping, breakpoints/watchpoints, symbols/disassembly, memory editing, display-list or DMA trace/overlay, Lua/Python/Gymnasium, BK2, patching, GUI/player integration, broad title expansion, and GameDrive/BupChip remain deferred under the selected milestone boundary and existing evidence requirements.
</deferred>

---

*Phase: 27-headless-developer-trace*
*Context gathered: 2026-11-16*
