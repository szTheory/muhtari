# Phase 27: Headless developer trace - Research

**Researched:** 2026-11-16
**Domain:** Deterministic C emulator instrumentation and headless runner output
**Confidence:** LOW

<user_constraints>
## User Constraints (from CONTEXT.md)

### Locked Decisions

#### Trace selection and event meaning
- **D-01:** Use explicit runner options for a trace file, one invocation frame, and an inclusive 16-bit address range. Number invocation frames from 1, matching `--hash-frame` and `--dump-frame`.
- **D-02:** Apply the range independently to an instruction's starting PC and to each CPU-issued bus access address. Include matching instruction and bus records in the order the emulator observes them; do not sort records after capture. A bus record includes address, read/write direction, value, tick, beam position and CPU context.
- **D-03:** Keep this first trace CPU-focused. Do not claim MARIA DMA or other bus-master coverage; those need a separate event-source contract and remain a later observability option.

#### Output and bounds
- **D-04:** Write trace records to a separate file so existing stdout reports retain their current format and order. Prefer a small, versioned JSON Lines record format with numeric machine fields and no new dependency.
- **D-05:** Require explicit frame and range selectors when tracing. Use a finite default record limit and a finite hard maximum; derive the values from the checked memory bound in planning rather than introducing an unlimited mode. Reject malformed, out-of-run frame, reversed/out-of-range address, zero, and over-maximum limit values before emulation.
- **D-06:** When another matching event arrives after the configured record limit, stop capturing but let emulation finish. Mark the trace explicitly as truncated; exact-limit output with no additional matching event remains complete. Treat trace file open/write/close failure as runner I/O failure (exit 1) and ensure an incomplete file cannot be mistaken for complete output.

#### Compatibility and deterministic behavior
- **D-07:** Add only a private per-instance observer seam for the runner. Keep callbacks and capture storage outside serialized machine state; do not change the public header, API/ABI, behavior revision, or state formats. If a public observer contract or compatibility version change becomes necessary, stop for an owner decision before expanding scope. — **Reversibility:** one-way — A published callback and API version become an embedder contract and require a compatibility transition to remove.
- **D-08:** Reuse the settled dual-compiled traced-loop design: select instrumentation at run entry only when tracing is installed, and retain the existing hooks-off instructions-per-frame limit.
- **D-09:** The generated numeric acceptance must pin event fields and same-tick order. Trace on/off runs use identical invocation inputs and compare every applicable frame hash and canonical state hash. Committed evidence contains no ROM, BIOS or save bytes outside the fixture manifest.

#### Reviewed Todos
- The five keyword matches from `todo.match-phase` were checked and excluded: they concern the required Wasm CI lane, Phase 2 ROM/nightly work, a Phase 2 re-review, Dependabot/release checks, and Phase 11 review notes. None is trace work, so none is folded into this phase.

### the agent's Discretion
- Choose exact event struct layout, observer wiring, checked allocation strategy, default and maximum record counts, file replacement mechanics, and the smallest useful generated diagnostic. Follow ENGINEERING §11 for any new bound and preserve existing runner file/path safety patterns.
- Add only recurring tests and checks with regression value; use one-off local checks for evidence that does not merit CI repetition.

### Deferred Ideas (OUT OF SCOPE)
- Interactive stepping, breakpoints/watchpoints, symbols/disassembly, memory editing, display-list or DMA trace/overlay, Lua/Python/Gymnasium, BK2, patching, GUI/player integration, broad title expansion, and GameDrive/BupChip remain deferred under the selected milestone boundary and existing evidence requirements.
</user_constraints>

<phase_requirements>
## Phase Requirements

| ID | Description | Research Support |
|----|-------------|------------------|
| TRACE-01 | A developer can opt in to an instruction and bus-access trace from muhtari-run for a selected invocation frame and inclusive address range, and the trace contains only matching events in deterministic order. | Private runner seam; instruction-start and bus-address filters; frame-loop attachment point. |
| TRACE-02 | Each trace record identifies its event kind, master tick, beam position and CPU context; bus-access records also identify the address, direction and value, with instruction and bus address filtering semantics documented. | CPU and bus producer locations, data-edge timing, MARIA beam origin, recommended sampling contract. |
| TRACE-03 | Trace output has a finite record limit, reports when that limit truncates the trace, rejects invalid frame/range/limit arguments before execution, and reports file-output failures without claiming a complete trace. | Checked fixed-capacity capture, strict option validation, terminal record and atomic file output recommendations. |
| TRACE-04 | A generated or owned diagnostic fixture produces an exact numeric trace oracle, and enabling tracing leaves the fixture's frame and canonical state hashes unchanged. | Generated build-tree cartridge pattern and trace-on/off hash/state comparison matrix. |
| TRACE-05 | The trace remains runner-only, preserves public API 8, ABI 1 and existing state formats, and keeps callback state and output buffers outside emulated machine state. | Existing private audio observer pattern; explicit state visitor; no public CPU/bus observer in the opened header. |
| TRACE-06 | With tracing disabled, the core preserves the existing hooks-off instruction-cost gate; instrumentation is selected only for calls that have tracing installed. | Missing implementation of the documented dual compile; build and perf work required across CPU and bus event producers. |
| TRACE-07 | Runner documentation describes trace invocation, filtering, record fields, output bounds, truncation and known limits; committed fixtures and logs contain no ROM, BIOS or save bytes beyond the manifest. | Runner documentation shape, existing generated fixture, and project asset constraints. |
</phase_requirements>

## Summary

**Primary recommendation:** Add a private runner-installed observer and a trace-enabled CPU/bus call path selected once at the start of the selected frame. Capture a single invocation frame into a checked, runner-owned fixed-capacity array, then write versioned JSON Lines atomically to a separate file. Generate the diagnostic cartridge in the build directory and use it for the exact event oracle, trace-on/off hash equality, and a CPU-backed hooks-off performance comparison.

[VERIFIED: include/muhtari.h:8-327] The opened public header has no CPU instruction or bus observer surface. Its version definition is “#define MUHTARI_API_VERSION 8” at line 12. [VERIFIED: include/muhtari.h:51-57] The public versions struct declares “uint32_t abi;                /* 1 */”. [VERIFIED: docs/runner.md:81-83] Existing state versions are “legacy machines use format 1 / behavior 6, the selected XM/YM profile uses format 2 / behavior 7, and the selected Souper mapper uses format 3 / behavior 8.” These constraints permit a private runner seam. [VERIFIED: .planning/phases/27-headless-developer-trace/27-CONTEXT.md:27] D-07 states: “A published callback and API version become an embedder contract and require a compatibility transition to remove.” Treat public observer design as a one-way compatibility boundary, not a routine fallback; stop for the owner's decision if private plumbing proves infeasible.

[VERIFIED: src/internal.h:464-467] A private per-instance audio observer already uses a callback plus user pointer. [VERIFIED: runner/main.c:1569-1574] The runner installs that observer after instance creation and before loading content. [VERIFIED: src/state.c:819-855] Serialization walks named machine fields, including CPU and bus fields; [VERIFIED: src/state.c:1455-1474] successful state application uses that visitor and resets audio observation state. Inference: a separate private trace callback/user pointer that is not added to the state visitor can preserve the state formats. Keep capture storage in runner-owned memory and do not add allocation to the emulation core.

[VERIFIED: CMakeLists.txt:75-102; repository search of CMakeLists.txt, src/, runner/ and tests/] The current production object build includes src/bus.c once and “add_library(muhtari_cpu OBJECT src/cpu.c)” once; a targeted search found no MUHTARI_TRACE_LOOP, run_loop.inc or MUHTARI_TRACE implementation in those paths. The architecture and DEC.34 document a dual-compiled traced loop, but that implementation is not present in the inspected build. The phase can remain within current compatibility decisions; implementing the documented CPU and bus traced path is an essential plan task. [ASSUMED] If implementation proves that the callback must be public or serialized, pause at D-07 rather than expanding scope; publishing it starts the one-way embedder compatibility contract.

## Architectural Responsibility Map

| Capability | Primary Tier | Secondary Tier | Rationale |
|------------|-------------|----------------|-----------|
| CPU instruction and bus event production | Core library | muhtari-run | The CPU and all CPU bus cycles are produced inside the core; the runner selects the private observer and frame window. |
| Trace filtering, bounded capture and JSONL serialization | muhtari-run | — | CLI choices, host memory, file paths and output errors belong to the runner. |
| Trace file replacement | Filesystem / runner | — | The runner owns file I/O and already has an atomic sibling-temp pattern. |
| Exact deterministic acceptance | Tests / build | Core and runner | A generated cartridge drives both event records and frame/state hash equality. |

## Project Constraints (from AGENTS.md)

- [VERIFIED: AGENTS.md:56-58] Reuse permissive code only; GPL, LGPL and unlicensed emulator source must not be opened or copied.
- [VERIFIED: AGENTS.md:59-60] Keep the three deliverables as the C library, runner and libretro core; do not add window, audio-device or input-device code.
- [VERIFIED: AGENTS.md:60-65] Keep ROM/BIOS/save bytes out of the tree except manifest-listed fixtures; preserve integer-only, deterministic, instance-owned and bounded emulation.
- [VERIFIED: AGENTS.md:66-80] Land behavior changes with CI-run tests and docs; keep the public header terse; verification is automation-first and does not require owner UAT by default.
- [VERIFIED: AGENTS.md:81-86] New checks must exercise representative pass and fault behavior; new resource bounds follow ENGINEERING §11; keep review and gap rounds bounded.
- [VERIFIED: .planning/config.json:32-60] Nyquist validation is explicitly false and security enforcement is explicitly false for this project configuration. The standard Validation Architecture section is therefore omitted; no security enforcement section is required.

## Verified Source Facts

### Execution and event producers

- [VERIFIED: src/frame.c:9-19] The core loop repeatedly calls one CPU step until the absolute frame deadline or BIOS rejection. It stops at an instruction boundary at or after the deadline, so a bus instruction can overshoot the nominal frame boundary.
- [VERIFIED: src/cpu.c:650-661] One CPU step fetches the opcode through the normal bus path; a jammed CPU makes a read of address 0xFFFF and returns. [VERIFIED: src/internal.h:771-774] The CPU routes every cycle through the two bus functions. Therefore bus events can include dummy cycles as well as useful reads/writes; document them as CPU-issued accesses, not high-level memory operations.
- [VERIFIED: src/bus.c:16-21] The bus timing constants are “#define BUS_FAST_TICKS 4u”, “#define BUS_FAST_DATA 4u”, “#define BUS_SLOW_TICKS 6u”, and “#define BUS_SLOW_DATA 5u”. [VERIFIED: src/bus.c:315-319] data_at returns the fast or slow data offset. [VERIFIED: src/bus.c:407-436] A read obtains the device result, updates open bus, calls end, then returns it. [VERIFIED: src/bus.c:503-523] A write delivers the driven value to the device, updates open bus, then calls end.
- [VERIFIED: src/bus.c:321-338] begin processes events due through the bus data edge before the actual device access. [VERIFIED: src/bus.c:343-376] end advances bus ticks after the CPU sample/store. Inference: stamp a bus event at bus.ticks + data_at(len), after the read value is known or with the driven write value, and before end advances the cycle. Reading m->bus.ticks after the bus function returns would record cycle completion, not the data edge.
- [VERIFIED: src/internal.h:185-190] CPU context includes “uint16_t pc;” and “uint8_t a, x, y, s, p;”. [VERIFIED: src/maria.c:21] The line duration is “#define MARIA_LINE_TICKS 454u”. [VERIFIED: src/maria.c:539-547] Current beam position is computed as (ticks - origin) divided and remaindered by that line duration. [VERIFIED: src/maria.c:614-617] At the next frame boundary the source executes “ma->origin += muhtari__timing(m).frame_ticks;” and sets “line = 0u”. [VERIFIED: docs/hardware/bus.md:221-227] The origin is line 0/t 0; the remainder is t; PAL uses the same 454-tick line length.
- [VERIFIED: src/frame.c:31-35,88-99] A BIOS rejection can occur after partial frame execution and returns a refusal status. Inference: the runner must only write a successful “complete” trace footer after a successful selected frame; keep a refused/failed frame stream explicitly incomplete or discard it.

### Runner, file and test patterns

- [VERIFIED: runner/main.c:1645-1649] The invocation loop counts frames from 1. [VERIFIED: docs/runner.md:85-89] Frame-local state points start at 0 after loading and the first invocation frame is point 1. [VERIFIED: docs/runner.md:43-50] Frame hashes, dumps and state hashes already use invocation-local one-based frame selection, with state point 0 reserved for initial state.
- [VERIFIED: runner/main.c:511-514] Existing collision comparison is string equality on the path spelling. [VERIFIED: runner/main.c:782-809] Existing state output validation checks frame bounds and collisions with state paths, frame dumps and inputs. [VERIFIED: docs/runner.md:106-109] The user docs call these lexical path collisions. Inference: trace output should join the same validation matrix, including ROM, movie, BIOS, HSC ROM, state load/save, frame dump and save output; current spelling equality does not catch different spellings that alias one file.
- [VERIFIED: runner/main.c:1146-1205] write_sibling_atomic creates an exclusive sibling temp, writes, flushes, syncs and closes before replacement, removing an owned temp on failure. It takes an already-owned contiguous byte span. Recommendation: extend the same ownership/replacement policy for a bounded trace writer rather than writing directly to the final path; a stream helper can retain the atomic pattern without a second full-size serialization buffer.
- [VERIFIED: tests/runner/test_control.c:73-87] An existing runner test builds a small cartridge in memory at test runtime, writes it under a test build directory, and provides a known CPU loop. [VERIFIED: tests/roms/manifest.txt:1-18] The manifest controls committed binary fixture assets. Recommendation: generate this phase's diagnostic cartridge in the build directory, avoiding new ROM bytes and a manifest change.
- [VERIFIED: docs/runner.md:231-253] Ordinary runner reports are stdout output and should retain their format. [VERIFIED: docs/runner.md:489-501] Exit 1 denotes file/runtime failure, exit 2 denotes usage failure, and errors go to stderr.

### Existing performance contract

- [VERIFIED: .planning/preparation/DECISIONS.md:61] DEC.34 states: “Per-line DMA statistics are always kept; hooks use a dual-compiled traced loop chosen only when hooks are installed; Ir/frame proves the off path unchanged”. [VERIFIED: .planning/preparation/ARCHITECTURE.md:176-180] The document says run_loop.inc is compiled with MUHTARI_TRACE 0 and 1 and selected once per call only when hooks are installed. [VERIFIED: .planning/preparation/ENGINEERING.md:192-198] The off-path contract compares no-hook dual-compiled execution with MUHTARI_TRACE_LOOP=OFF and permits a difference of at most 32 Ir/frame.
- [VERIFIED: .github/workflows/ci.yml:616-633,660-683] The existing Linux Cachegrind job installs valgrind and measures the runner, but its run invokes muhtari-run with only --frames. [VERIFIED: docs/runner.md:9-10] With no cartridge the runner uses a test card. [VERIFIED: src/frame.c:107-114] The test-card path does not run CPU code. Inference: the current test-card-only perf run cannot prove the CPU hooks-off gate; feed a generated CPU cartridge to both variants within the existing job.
- [VERIFIED: .planning/preparation/ENGINEERING.md:257-275] Architecture review must inspect ownership, coupling, test independence, dependencies and recurring cost; new resource bounds need an explicit product/platform/reliability/operating-cost objective and comparable measurements. The proposed bound and measurement criteria below are candidate decisions, not yet implementation evidence.

## Inferences and Recommendations

### Private instrumentation design

[ASSUMED] Candidate CLI shape: --trace FILE --trace-frame N --trace-range LO:HI [--trace-limit N]. Require the trace file, frame selector and range selector together. Require loaded cartridge content because the no-content test-card path does not execute the CPU; reject --frames0 for trace. Resolve the effective invocation frame count, including movie defaults, before checking N against that count and before emulation.

[ASSUMED] Install a private callback and user pointer on the instance only for the selected frame; the runner owns the callback target, record buffer and output lifecycle. Do not add these pointers to the public header or state visitor. Attach immediately before the selected muhtari_run_frame call and detach on every return path. When there is no observer, choose the ordinary run path once at frame entry. In the traced path, compile CPU stepping and bus producers as traced variants so the ordinary path has no per-instruction or per-access callback branch. An outer-loop-only duplicate is insufficient if the shared bus functions perform a runtime observer test on every access.

[ASSUMED] Emit an instruction event at the opcode fetch's data edge, using that access address as the starting PC and capturing CPU registers before fetch increments PC. src/cpu.c sets SYNC for the opcode fetch and fetch increments PC only after muhtari__bus_read returns; src/bus.c clears SYNC in the read path. A traced bus reader can retain the SYNC flag, then emit an instruction record with the opcode before the matching opcode bus record, both stamped at the same data tick. Emit one bus event per CPU access, including dummy cycles, using the exact requested 16-bit address, direction, value read or driven, and CPU context at that access. Filter instruction records by opcode-fetch PC and bus records independently by bus address. Do not emit MARIA DMA events or sort captured records; preserve this defined callback order.

[ASSUMED] For each bus event, compute beam line/t from its data tick and the current MARIA frame origin after begin has processed events through the data edge. The source keeps MARIA's origin as mutable state and can advance it when line-0 events run; add an exact boundary test where data lands on a line/frame-origin transition. Document t as a beam clock within a line, not a pixel x-coordinate. Document CPU register sampling as the state immediately before each access; for instruction events, retain the pre-opcode-fetch context.

**Code example — current bus sampling seam:**

    v = muhtari__cart_read_at(m, addr, 0, m->bus.ticks + data_at(len));

[VERIFIED: src/bus.c:429-435] This is the current cartridge read and return ordering. A trace read record should preserve that same data-edge time and returned value while leaving the emulated operation unchanged.

### Bounds and truncation

[ASSUMED] A candidate common event struct can fit within 32 bytes using a 64-bit tick, 16-bit beam line/t/PC/address, and byte-sized kind/direction/value/register context; enforce this with a compile-time size assertion. A default cap of 65,536 records and hard cap of 131,072 would bound the record array at 2 MiB by default and 4 MiB maximum. Check `limit <= SIZE_MAX / sizeof(record)` before allocation, allocate before emulation, and do not reallocate from the callback. Validate the hard cap and frame/range selectors before the first frame.

[ASSUMED] The hard cap is large enough for a full-frame all-address trace if the existing overshoot proof applies. With the current constants, one slowest-access allowance is 456 + 2×454 + 12 + 6 = 1,382 ticks; 15 accesses give 20,730 ticks of overshoot. The longest PAL frame is 142,102 ticks; adding overshoot gives 162,832 ticks and at most 40,708 CPU accesses at 4 ticks per access. Allowing one instruction record plus one bus record per access yields at most 81,416 records, below the candidate hard cap of 131,072. Source values: [VERIFIED: src/internal.h:31-35] “const struct muhtari__timing pal = {313u, 292u, 142102u, 139874u, 4000u, 591149u, 962u};”; [VERIFIED: src/frame.c:9-14] “Every step makes at least one bus cycle of 4 ticks or more”; [VERIFIED: src/internal.h:48-57] the opened source defines the following values and combines them in its overshoot bound:

    #define MUHTARI__SYNTH_RDY_TICKS 456u
    #define MUHTARI__SYNTH_DMA_TICKS (2u * 454u)
    #define MUHTARI__SYNTH_HOLD_ROUND_TICKS 12u
    #define MUHTARI__SYNTH_ACCESS_TICKS 6u
    #define MUHTARI__SYNTH_OPCODE_CYCLES 8u
    #define MUHTARI__SYNTH_INTERRUPT_CYCLES 7u
    #define MUHTARI__SYNTH_OVERSHOOT_TICKS                                                             \
        ((MUHTARI__SYNTH_RDY_TICKS + MUHTARI__SYNTH_DMA_TICKS + MUHTARI__SYNTH_HOLD_ROUND_TICKS +      \
          MUHTARI__SYNTH_ACCESS_TICKS) *                                                               \
         (MUHTARI__SYNTH_OPCODE_CYCLES + MUHTARI__SYNTH_INTERRUPT_CYCLES))

Verify that tracing does not add CPU bus accesses and that the helper's worst-case premises still apply before locking the cap; recalculate if those premises change.

[ASSUMED] Use a fixed, bounded JSONL line writer with an asserted maximum record line length (candidate 192 bytes with compact keys), making the candidate maximum trace body at most 24 MiB plus a small header/footer. Store numeric machine fields as JSON numbers and include format/version and terminal status metadata. A final `complete` or `truncated` record differentiates normal completion from an unfinished prefix; set `truncated` only after observing an additional matching event beyond the cap, so exact-limit output remains complete. Treat allocation, write, flush, sync, close or replace failure as exit 1. Keep final output atomic so an older destination is preserved if new output fails, and report that no fresh trace was committed.

### Fixture and acceptance

[ASSUMED] Generate a short deterministic cartridge in the build directory, using the existing C fixture builder as the portability model. Have it exercise an immediate instruction, a memory write, a memory read and a stable loop; use a full-range trace for the exact numeric oracle and add narrow-range cases that independently prove instruction-PC and bus-address filtering. Pin instruction/bus ordering, sampled register context, read/write values, tick and beam fields. Make the generated program avoid bank hotspots and run identically in trace-on and trace-off invocations.

[ASSUMED] For identical cartridge, frame count, region, and input, compare all applicable frame hashes and canonical state hashes between trace-off and trace-on. Add controls for malformed and out-of-run frame, malformed/reversed/out-of-range address range, zero and over-maximum record limit, exact-limit-complete, one-more-event-truncated, path collision, and output open/write/close failure. Assert ordinary stdout equality across trace modes. Run the generated CPU workload under both standard dual-compiled no-observer and a MUHTARI_TRACE_LOOP=OFF build in the existing Cachegrind job; retain the DEC.34 threshold of at most 32 Ir/frame. Measure maximum capture bytes and serialized line length on the generated workload and compare them with the stated bound. These are phase planning recommendations; no tests were run during this research pass.

### Alternatives considered

| Instead of | Could use | Tradeoff |
|------------|-----------|----------|
| Dual-compiled CPU and bus event producers in the common core objects | Runner-specific traced core object variant | Isolates trace symbols and code size from other deliverables, but duplicates more of the build graph. Reconsider if private symbol visibility or shipped core size creates a measured issue; retain once-per-frame selection and the hooks-off gate either way. |
| Bounded in-memory records then atomic JSONL write | Stream records to a sibling temporary while emulation runs | Lower capture memory, but adds per-event file calls and more complex late-write failure handling. Reconsider if the measured fixed array exceeds the chosen memory objective and a writer fault-control can prove incomplete output handling. |
| Generated private cartridge fixture | Add a committed binary ROM fixture | Avoids a builder but adds an asset/provenance and manifest burden; use only if an exact oracle cannot be generated reliably in existing C test infrastructure. |

## Standard Stack

### Core

| Library/tool | Version | Purpose | Why Standard |
|--------------|---------|---------|--------------|
| C | C17 | Private observer, CPU/bus variants and runner-owned capture | Existing project language standard. [VERIFIED: CMakeLists.txt:1,14-18] Project config sets C_STANDARD 17 and disables C extensions. |
| CMake / CTest | Existing repository configuration | Build the ordinary/traced variants and register recurring runner acceptance | Existing test and CI mechanism. [VERIFIED: CMakeLists.txt:75-104,169-184] Core object libraries and the runner already build through CMake. |
| C standard library and existing runner file helpers | Existing code | Checked allocation, parsing, JSON formatting and atomic file output | No external package is needed or recommended. |

### Alternatives Considered

| Instead of | Could Use | Tradeoff |
|------------|-----------|----------|
| Existing runner/C file primitives | Add a JSON library | Not justified for this bounded numeric schema; adds a dependency to avoid a small fixed serializer. |
| Build-directory C fixture generator | Assembler or external ROM package | Adds a tool/dependency and asset provenance without improving the numeric oracle. |

**Installation:** None. No external packages should be added for this phase.

## Architecture Patterns

### Recommended project structure

Keep trace event definitions in private source headers; keep trace option parsing, record allocation, JSONL serialization and file ownership in runner code; keep the generated diagnostic builder and trace contract/fault controls beside existing runner tests; document user-visible behavior in docs/runner.md. [VERIFIED: CMakeLists.txt:169-180] The runner already has the private src include directory, which is the existing boundary for its internal calls.

### Pattern: select traced execution at the frame boundary

**What:** Choose the ordinary or traced CPU execution path once before the selected frame's core call. CPU instrumentation must use the trace-enabled bus producer; the ordinary CPU continues calling ordinary bus functions.
**When to use:** Only when the private observer is installed for the selected invocation frame.
**Source:** [VERIFIED: src/frame.c:15-19] Current execution calls one CPU step per loop; [VERIFIED: src/cpu.c:650-661] CPU step calls the opcode bus path; [VERIFIED: src/internal.h:771-774] every CPU cycle passes through the bus functions.

### Pattern: capture in memory, commit after the frame

**What:** The callback filters and appends only while within the fixed capacity; after the first additional match it sets a sticky truncation bit and returns immediately, allowing emulation to finish. Serialize after a successful selected frame and atomically replace the destination only after all I/O stages succeed.
**When to use:** One invocation frame with a finite record cap.
**Source:** [VERIFIED: runner/main.c:1146-1205] Existing atomic output owns a sibling temp through write, flush, sync, close and replacement.

### Anti-patterns to avoid

- **Per-access hooks-off conditional:** It adds a runtime decision to the hot bus path and does not implement the locked dual-compiled design.
- **Timestamp after the bus call:** It reports the end of the access rather than the hardware data sample edge.
- **Sorting by tick:** It can reorder observations that share a tick and conceal actual CPU call order.
- **Direct writes to the final trace path:** A failed or killed write can leave an incomplete file that looks like a valid trace.
- **Whole-struct state serialization:** Trace callback state and storage must remain outside the explicit canonical state visitor.
- **Calling CPU-only output a system-bus trace:** CPU events do not include MARIA DMA or other bus masters.

## Don't Hand-Roll

| Problem | Don't Build | Use Instead | Why |
|---------|------------|-------------|-----|
| CLI parsing and invocation numbering | A second command parser/frame counter | Existing runner option and frame loop | Reuses one-based invocation semantics and pre-run validation. |
| File replacement and temp ownership | Direct final-path writes or an ad hoc temp name | Existing sibling temporary/replacement mechanism | Existing code handles exclusive creation and writer completion stages. |
| Fixture provenance | New committed binary without need | Runtime-generated test cartridge | Keeps test bytes out of the tracked tree. |
| JSON generality | A generic schema framework | Small versioned numeric JSONL serializer | Schema is narrow and bounded; no package is needed. |

## Common Pitfalls

### Bus timestamp and beam drift

**What goes wrong:** Records use cycle completion tick or derive beam from stale line origin at a boundary.
**Why:** Reads return only after end advances the bus; line 0 updates the mutable MARIA origin.
**How to avoid:** Capture at the data edge and test fast/slow access plus line/frame-origin boundaries; specify the exact beam sampling instant.

### Trace changes state or timing

**What goes wrong:** Trace-on changes canonical state, frame output, observer carry, or ordinary CPU instruction cost.
**Why:** Callback fields enter serialization, instrumentation branches run per bus access, or an event callback alters emulated data.
**How to avoid:** Keep observer host-owned, callbacks read-only, compile separate traced hot producers, compare all hashes and run the existing Ir/frame gate against trace-loop-off.

### Silent partial output

**What goes wrong:** A prefix appears complete after truncation, frame refusal, or late file failure.
**Why:** Record count alone cannot distinguish exact-limit completion from a longer trace; write/close failure can happen after records were emitted.
**How to avoid:** Observe one extra matching event before declaring truncation; append explicit terminal status only on successful finalization; use atomic replacement and fault controls.

### Accidental path overwrite

**What goes wrong:** Trace output replaces an input or another requested output.
**Why:** Current comparison checks spelling, not file identity or normalized aliases.
**How to avoid:** Join trace with every existing path-conflict class and decide whether existing lexical behavior is sufficient; test same-spelling and representative alias cases without deleting user files.

### Bound underestimation

**What goes wrong:** A valid one-frame trace exceeds allocation or output budget.
**Why:** Held CPU cycles, interrupt-entry accesses, JSON formatting overhead or PAL timing were omitted from the derivation.
**How to avoid:** Enforce sizeof and serialized-line bounds, recalculate against CPU and scheduler invariants, and measure with a generated busy CPU fixture; any violated premise changes the cap calculation.

## State of the Art

| Old Approach | Current Approach | When Changed | Impact |
|--------------|------------------|--------------|--------|
| Documentation describes a generic future observer loop | Phase 27 narrows the first delivered surface to runner-only instruction and CPU bus events | Phase 27 context, 2026-11-16 | Implement only the trace foundation; defer public hooks, DMA, GUI, scripting and other observability tools. |
| Current build compiles one CPU and bus producer | Add dual compiled traced and ordinary producers selected once for the selected frame | Required by DEC.34; not implemented in this checkout | A CPU-backed off-path measurement must be added because test-card CI does not execute the CPU. |

**Deprecated/outdated:** None discovered in the inspected current code. The architecture's future run_loop.inc description is a design contract, not evidence that a traced implementation already exists.

## Assumptions Log

| # | Claim | Section | Risk if Wrong |
|---|-------|---------|---------------|
| A1 | A private observer can be installed for only one selected frame without changing public API or serialized state. | Inferences and Recommendations | If private wiring is infeasible, stop at D-07; a public callback is a one-way embedder compatibility contract. |
| A2 | Candidate caps of 65,536 default and 131,072 hard maximum fit a 32-byte event record and the one-frame worst-case event derivation. | Bounds and truncation | Too-low cap truncates valid traces; too-high cap spends excess memory. |
| A3 | A fixed-width record line can be bounded at 192 bytes and the maximum JSONL body at 24 MiB plus header/footer. | Bounds and truncation | Actual numeric formatting/key choices could exceed the storage and output objective. |
| A4 | The generated diagnostic program can exercise write/read and loop behavior without undocumented mapper or boot assumptions. | Fixture and acceptance | Exact fixture design may need adjustment to existing cartridge constructors. |
| A5 | Existing lexical path validation is acceptable if expanded to trace output, or an identity check can be added proportionally. | Common Pitfalls | Alias paths could overwrite an input or another output. |
| A6 | SYNC can identify opcode fetches for instruction records, allowing the record to use the fetch data edge, starting PC and pre-increment CPU context before the matching bus record. | Private instrumentation design | A wrong event source or order would make instruction records inconsistent with the CPU stream. |

## Open Questions (RESOLVED FOR PLANNING; implementation evidence pending)

1. **Exact event wire schema and CPU-context timing — RESOLVED**
   - What we know: CONTEXT locks numeric machine fields, event kind, address filter semantics, CPU context and deterministic observation order.
   - Disposition: 27-01 Task 1 defines the versioned numeric JSONL fields and pre-fetch/pre-access sampling points; 27-01 Task 2 must verify each field and ordering against an independent exact fixture oracle. Concrete serializer widths remain an implementation detail covered by that oracle.
2. **Does the 131,072-record hard cap meet the intended one-frame memory objective? — RESOLVED**
   - What we know: Existing instruction and held-cycle comments give a conservative upper-bound starting point; ENGINEERING §11 requires an objective and measurements.
   - Disposition: No candidate cap is accepted as fact by this research. 27-01 Task 1 and 27-02 Task 1 must prove the event ceiling, actual record and line sizes, finite memory/disk objective, and comparable measurements; replace the candidates if any premise fails before tests or documentation lock them.
3. **Should trace paths reject aliases beyond identical spelling? — RESOLVED**
   - What we know: Existing path safety compares lexical spellings.
   - Disposition: 27-02 Task 2 preserves existing lexical behavior, tests exact collisions and representative supported aliases, and requires a narrow identity/normalization check only if a trace-specific overwrite gap reproduces. This is scoped to trace paths and does not reopen general runner path policy.

## Environment Availability

| Dependency | Required By | Available | Version | Fallback |
|------------|------------|-----------|---------|----------|
| CMake | Build and tests | ✓ | 4.4.3 | — |
| CTest | Recurring tests | ✓ | 4.4.3 | — |
| Ninja | CI preset/build | ✓ | 1.13.2 | — |
| C compiler | Core and runner | ✓ | Apple clang 21.0.0 | Use CI GCC lane for cross-compiler validation. |
| Valgrind/Cachegrind | Hooks-off instruction-cost gate | ✗ locally | — | Existing Linux CI job installs valgrind; run the comparison there. |

**Missing dependencies with no fallback:** None for planning or local implementation.
**Missing dependencies with fallback:** Local Valgrind; use the existing Linux performance job.

## Security Domain

Security enforcement is explicitly disabled in .planning/config.json for this project. The trace still handles untrusted command-line values and output paths using checked parsing, bounds and the existing runner file-safety conventions.

## Sources

### Primary (project source of truth; directly inspected)

- include/muhtari.h, src/internal.h, src/frame.c, src/cpu.c, src/bus.c, src/maria.c, src/state.c — public/private observer boundary, current CPU/bus execution, timing and serialization.
- runner/main.c, tests/runner/test_control.c, tests/roms/manifest.txt, CMakeLists.txt — runner option/file patterns, generated fixture and current build topology.
- .planning/phases/27-headless-developer-trace/27-CONTEXT.md, .planning/REQUIREMENTS.md, .planning/STATE.md — locked scope, requirements and planning status.
- .planning/preparation/ARCHITECTURE.md §8, DECISIONS.md DEC.34/58, ENGINEERING.md §§8/11, HOSTS-AND-INTEGRATION.md §§4/5, .planning/LESSONS.md, SEED-004, docs/runner.md — project contracts, trace scope and prior implementation lessons.

### Environment observations (LOW confidence outside this checkout)

- Environment commands for CMake, CTest, Ninja, compiler and Valgrind availability — observed in this checkout on 2026-11-16; recheck on another build host.

## Metadata

**Confidence breakdown:**
- Overall confidence: LOW — classify-confidence returned LOW for the codebase and curated providers; direct source facts cite opened project definitions, while implementation details and resource choices remain assumptions.
- Standard stack: LOW per the classifier; project C17/CMake configuration is quoted directly, with no external package version research needed.
- Architecture: LOW overall; current call boundaries are directly source-backed, but the documented dual-compiled path is absent from implementation.
- Pitfalls and bounds: LOW — the event timing is source-backed, while proposed record/cap/output sizes still require implementation-time validation.

**Research date:** 2026-11-16
**Valid until:** 2026-12-16 for stable architecture; recheck if the CPU scheduler, frame bounds, file helper or CI performance job changes.
