# Phase 27: Headless developer trace - Discussion Log

> **Audit trail only.** Decisions are captured in CONTEXT.md — this log preserves the alternatives considered.

**Date:** 2026-11-16
**Phase:** 27-Headless developer trace
**Areas discussed:** Trace selection and event meaning, output and bounds, compatibility and deterministic behavior

The owner asked for a recommendation and explicitly directed: “Let's follow your recommendations.” Under the project's authorized autonomous continuation, ordinary in-scope choices were resolved using existing runner and architecture conventions; no compatibility or scope boundary was crossed.

---

## Trace selection and event meaning

| Option | Description | Selected |
|--------|-------------|----------|
| CPU instruction and CPU-issued bus events | Filter instruction PC and access address independently; use one-based invocation frame and inclusive address range. | ✓ |
| Every bus master | Include CPU plus MARIA DMA and any other device traffic, requiring source identity and wider acceptance. | |

**Choice:** Apply the narrow CPU-focused trace slice. The records include CPU context; DMA ownership and display diagnostics are a separate future capability.
**Notes:** The phase requires both instruction and bus-access events but does not define other bus masters. The narrower scope is consistent with the one-slice milestone recommendation.

---

## Output and bounds

| Option | Description | Selected |
|--------|-------------|----------|
| Separate structured trace file | Keep regular stdout stable; use bounded versioned JSON Lines for exact numeric records. | ✓ |
| Mix trace lines into stdout | Simpler destination, but interleaves trace with existing hashes and reports. | |

**Choice:** Use a separate trace file with a finite record budget, explicit truncation status and visible I/O failure.
**Notes:** The exact default and maximum counts remain a planning-time resource calculation under ENGINEERING §11.

---

## Compatibility and deterministic behavior

| Option | Description | Selected |
|--------|-------------|----------|
| Private runner-facing observer | Keep API 8, ABI 1 and state formats unchanged; select instrumentation only when installed. | ✓ |
| Public observer API | Expose callbacks for external embedders and change compatibility surface. | |

**Choice:** Keep the observer private; pause for an explicit owner decision if that proves insufficient.
**Notes:** Live `include/muhtari.h` contains no CPU trace observer. The existing architecture specifies dual compilation and a hooks-off instruction-cost check.

---

## Reviewed backlog matches

The five weak keyword matches were read and found unrelated to this trace phase: Wasm CI promotion, Phase 2 ROM/nightly work, Phase 2 review seeds, Dependabot/release checks, and Phase 11 review notes. None was added to scope.

## the agent's Discretion

- Observer wiring, exact record layout and ordering test, bounded allocation size, file replacement mechanics, generated diagnostic shape, and which checks merit recurring CI coverage.

## Deferred Ideas

- The broader SEED-004 debugger/tooling bundle and GameDrive/BupChip work remain deferred under their existing triggers and evidence gates.
