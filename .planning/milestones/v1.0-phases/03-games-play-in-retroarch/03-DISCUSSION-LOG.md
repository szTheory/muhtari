# Phase 3: Games play in RetroArch - Discussion Log

> **Audit trail only.** Do not use as input to planning, research, or execution agents.
> Decisions are captured in CONTEXT.md — this log preserves the alternatives considered.

**Date:** 2026-10-19
**Phase:** 03-games-play-in-retroarch
**Areas discussed:**
- The public API, version 3
- MARIA, the run loop and the diagnostic ROMs
- Samples, OpenBIOS, predicates, properties and the ledger
- Movies, input, boot-to-play, libretro and RetroArch

The owner's standing brief (GSD-HANDOFF §3, phase 1) applied to every area. Each area was:
- researched through every relevant lens, primary sources first, with an adversarial pass;
- settled with one coherent recommendation;
- locked without asking again.

Four advisor agents ran in parallel at calibration tier `minimal_decisive`. Two points went to the owner:
- the public API, because it adds to a released library;
- GAME-01's wording, because new evidence shows it cannot pass as written.

---

## The public API, version 3

| Option | Description | Selected |
|--------|-------------|----------|
| Consumer-minimal v3 (Recommended) | `muhtari_load`, `power_cycle`, `set_input`, `get_memory` and `get_counters` (frames and lag only), plus 4 statuses. Each has a libretro or runner consumer. The profile, line statistics, MARIA and CPU views, peek and states stay internal, each with a named trigger. PAL is refused until LIB-04 | ✓ |
| The full ARCHITECTURE §4 sketch now | Also the profile in media, `get_line_stats`, `get_maria`, `get_cpu`, `peek`, the full counters and a `state_size` stub. About 6 structs with no outside consumer, and the line-statistics layout fixed before the MARIA timing is proven | |

**User's choice:** Consumer-minimal v3.

**Notes:**
- **The device sits in the per-frame input**, because RetroArch sets controller devices after `retro_load_game`.
- **`get_memory` returns live pointers.** That is the one deliberate exception to the struct rule, because libretro
  memory maps need them.

→ D-01 to D-03.

---

## MARIA, the run loop and the diagnostic ROMs

| Option | Description | Selected |
|--------|-------------|----------|
| A small diagnostic set on the existing toolchain (Recommended) | Three own MIT dasm ROMs (`dma`, `cpu`, `dli`) on the pinned 7800basic wasm tools. Bytes committed, with rebuild-and-compare on every leg. An in-process mailbox driver. The §13 conflicts stay on their current recommendations, labelled fitted. `cpu_phase` derived from C10 | ✓ |
| The full CONFORMANCE §4 suite plus a Verilator MiSTer simulation | Settles more of §13 now, but pulls v4.0's referee into v1.0 with a new dependency and MiSTer's facts-only top-level files | |

**User's choice:** not asked; the advisor's recommendation was followed per the brief.

**Notes:**
- **WR-05 is narrowed to "queued"**, because MARIA raises one DLI edge per zone and never retries.
- **Expected values are derived from cited constants before the first run.** Only `fit` rows may be copied from the
  emulator.

→ D-04 to D-12.

---

## Samples, OpenBIOS, predicates, properties and the ledger

| Option | Description | Selected |
|--------|-------------|----------|
| Build all 35 samples, close the gaps with own ROMs, and keep the ledger minimal (Recommended) | Pins as 600-frame SHA-256 chains in `tests/built/expected.txt` with a behaviour-revision guard. Title predicates in the ledger. A minimal ratchet and doc generator. Coverage proved from the core's observed DMA feature mask | ✓ |
| A covering subset of about 8 samples, with the full CONFORMANCE §5 machinery | Fewer frames, but it still cannot cover read mode 2, kangaroo mode or the missing boards, and it adds gate code | |

**User's choice:** not asked for the approach. The GAME-01 wording question below went to the owner.

### GAME-01 coverage gap

| Option | Description | Selected |
|--------|-------------|----------|
| Samples plus our own ROMs (Recommended) | GAME-01 reads "the samples and the project's diagnostic ROMs together cover…". MIT ROMs we write fill read mode 2, kangaroo mode, the 16 KB linear board, SuperGame bank 6 and 144 KB | ✓ |
| Narrow GAME-01 | Keep the samples only, and move the uncovered features out of v1.0 | |

**User's choice:** Samples plus our own ROMs.

**Notes:**
- **New evidence on the samples.** A grep of the pinned samples' `.bas` directives shows only 160A, 320A and 320C, and
  only 32k, 48k, 128kRAM and `banksets` sizes.
- **New evidence on OpenBIOS.** Its splash lasts about 320 frames, so GAME-03's 300 frames are counted from the
  cartridge hand-off on the BIOS path. The wording was clarified to match.
- **OpenBIOS reproduced.** The advisor built it from the pinned `.asm` with the pinned dasm under wasmtime 48.0.6.
  It exited 0 with SHA-256 `46b5bc2b…cb2`, matching CONFORMANCE §2(b).

→ D-13 to D-20.

---

## Movies, input, boot-to-play, libretro and RetroArch

| Option | Description | Selected |
|--------|-------------|----------|
| Own positional text movie, in-process input proof, rcheevos fetched by pin (Recommended) | `muhtari-movie 1`, one line per frame, with a strict header. A test-assembled read-loop cart for `-L input`. `spacephysics` for boot-to-play. rcheevos v12.1.0 fetched by pin and compiled into one test. The existing RetroArch job given a diagnostic ROM | ✓ |
| A BK2-native log, a cc65 input ROM and a vendored rcheevos | BK2 contradicts DEC.58. A cc65 step joins a gating test, and about 2,700 vendored lines drift from what RetroArch ships | |

**User's choice:** not asked; the advisor's recommendation was followed per the brief.

**Notes:**
- **rcheevos spike.** The advisor compiled three rcheevos files against our `libretro.h`. The resolver mapped $2800 to
  RAM+$800 and $3000 to NULL.

→ D-21 to D-27.

---

## Claude's Discretion

- The diagnostics' internal layout, and merging the board ROMs.
- The internal accessor names, and the plan order within the tracer order.
- Folding in the WR-02 and IN-05 seeds if they come for free.

## Deferred Ideas

- The profile, line statistics, views, peek and further counters in the public API, each with a trigger.
- PAL (LIB-04), other devices (LIB-02), battery (LIB-03), states (STATE-01).
- Diagnostics #7, #10, #11 and the raster ROM, plus the Verilator referee (v4.0).
- The a7800 differential, JSON `--probe`, and a hardware-runnable input loop.
