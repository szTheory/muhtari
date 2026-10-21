# Phase 1: A test frame in RetroArch - Discussion Log

> **Audit trail only.** Do not use as input to planning, research, or execution agents.
> Decisions are captured in CONTEXT.md — this log preserves the alternatives considered.

**Date:** 2026-10-16
**Phase:** 01-a-test-frame-in-retroarch
**Areas discussed:** wasm32 lane (pending todo), Test card picture, What the 0.1.0 header declares, Oldest systems
the zips run on

The owner answered one combined prompt. It folded the todo and selected all three gray areas. In free text it added
a standing brief:
- Fan out on every decision point through all relevant stakeholder lenses.
- Prefer reputable primary sources, and run an adversarial pass.
- Synthesize one coherent recommendation per point and follow it without asking again.
- "Another copy and paste is better than another dep": smaller, flatter dependency trees.

One advisor researcher ran per area, four in parallel, at calibration tier `minimal_decisive`. Their
recommendations were reconciled across areas and locked as D-01 to D-27.

---

## wasm32 lane (pending todo)

| Option | Description | Selected |
|--------|-------------|----------|
| Fold in, every PR (Recommended) | Library and runner built for wasm32 under wasmtime; test-card hash equal to the other lanes; report-only for 10 runs on main, then in `CI required`; browser page stays SEED-008 | ✓ |
| Fold in, nightly only | Same lane nightly; a break is found the next morning, after its release | |
| Leave it for SEED-008 | No wasm build until the v1.0 close | |

**User's choice:** Fold in, every PR.
**Advisor research:** the advisor tested the toolchain in an `ubuntu:26.04` container. It weighed two toolchains:
- **Distro (recommended):** clang-21 and lld-21 plus the wasi-libc and compiler-rt packages, triple
  `wasm32-wasi`.
- **wasi-sdk-34 tarball:** kept as the fallback.

It weighed two runtimes:
- **wasmtime 48.0.6 LTS (recommended).**
- **Node WASI:** rejected as Experimental and escapable.

Scope: the `portable` label, as `be32` runs it. → D-18 to D-24.

---

## Test card picture

| Option | Description | Selected |
|--------|-------------|----------|
| Layout A: centred square-cell chart | 16×16 chart of 14×12 cells (square at 6:7) on the NTSC safe area. The margins carry a corner block, border, overscan-crop lines, 1×1 and 2×1 checkers, a red top band and a green bottom band | ✓ (advisor rec) |
| Layout B: nesturbator's full-width chart | Bigger cells, no aspect cue, border over the edge cells | |
| Circle on the chart (PM5544-style) | Best-known aspect cue, but it covers cells | |
| Animated card | Shows frames advancing, but the hash depends on N and the hosted screenshot needs frame alignment | |
| Text on the card | Labelled, but needs a font and changes the hash with every version | |

**User's choice:** selected the area for discussion; the advisor's recommendation was followed per the brief.
**Notes:** the advisor changed nesturbator's blue bottom band to green, so that an R/B swap and a vertical flip
look different. → D-01 to D-06.

---

## What the 0.1.0 header declares

| Option | Description | Selected |
|--------|-------------|----------|
| A: only working calls, static library | Five functions, a new instance shows the test card, no `load`, grow by appending with `MUHTARI_API_VERSION`; static PIC library, hidden visibility, CMake package and pkg-config | ✓ (advisor rec) |
| B: whole v1 surface with stubs, static and shared | All ARCHITECTURE §4 calls declared, unready ones returning "unsupported"; export macro and a shared library | |

**User's choice:** selected the area for discussion; the advisor's recommendation was followed per the brief.
**Notes:** while researching, the advisor moved off nesturbator's NULL-means-malloc default. The allocator is
required, because the core links only `memcpy memmove memset memcmp`. The `.lib`'s C runtime was reconciled to
the floors decision (`/MT` plus `/Zl`, consumers tested under `/MT` and `/MD`). → D-07 to D-12.

---

## Oldest systems the zips run on

| Option | Description | Selected |
|--------|-------------|----------|
| Linux: keep 26.04, check ELF version needs | Core `.so` ≤ GLIBC_2.17, runner ≤ 2.34, read from the binaries | ✓ (advisor rec) |
| Linux: build release binaries in an old container | Lower runner floor, but the shipped binary is not the tested one | |
| macOS: deployment target 10.13 | Matches RetroArch 1.22.2 (10.13 Intel, 11.0 arm64) | ✓ (advisor rec) |
| Windows: static CRT | No VC++ redistributable needed, like buildbot cores | ✓ (advisor rec) |
| Windows: DLL CRT (default) | Smallest binaries, but needs `VCRUNTIME140.dll` | |

**User's choice:** selected the area for discussion; the advisor's recommendation was followed per the brief.
**Notes:** the evidence came from inspecting szTheory/nesturbator v0.1.10 binaries, which set no minimums: macOS
`minos 15.0` and a `VCRUNTIME140.dll` import. → D-13 to D-17.

---

## Claude's Discretion

- The phase 1 `.info` claims no extensions until games play.
- Where the warm palette lives (host-helper code, a PROVENANCE seed).
- How far nesturbator seeds the runner, adapter, tests and CI.
- Stack protector handling in `abi.undefined`.
- **CI gating (D-25):** the lanes the requirements name gate from their first run, including the hosted RetroArch
  job, following nesturbator's `CI required`. This corrects the discussion's summary, which had that job
  report-only at first.

## Deferred Ideas

- Public palette helper (SEED-008 or phase 3).
- Shared library (v5).
- wasm f32/f64 opcode check (a seed).
- A container smoke test for the floors.
- The Linux runner below glibc 2.34.
- musl archives.
- A browser or Node hash step (SEED-008).
- The digest's pins section (v2.0 tune-up).
