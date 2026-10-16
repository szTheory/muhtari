# Phase 2: The CPU matches the public vectors - Discussion Log

> **Audit trail only.** Do not use as input to planning, research, or execution agents.
> Decisions are captured in CONTEXT.md — this log preserves the alternatives considered.

**Date:** 2026-10-16
**Phase:** 02-the-cpu-matches-the-public-vectors
**Areas discussed:**
- Cartridge identity in the public API
- The vector sample and the nightly full set
- Driving SALLY, the bus and RIOT before MARIA
- Probe evidence and the bundled tables

The owner's standing brief, recorded in phase 1 and in GSD-HANDOFF §3, applied to every area. Each area was:
- researched through every relevant lens, primary sources first, with an adversarial pass;
- settled with one coherent recommendation per point;
- locked without asking again.

All four areas were researched: one advisor agent per area, four in parallel, at calibration tier
`minimal_decisive`. Only the public API went to the owner, because it adds to a released library (USER-PROFILE: new
public API on a shipped library). The other areas were reconciled and locked as D-01 to D-31.

---

## Cartridge identity in the public API

| Option | Description | Selected |
|--------|-------------|----------|
| Probe now, load in phase 3 (Recommended) | Adds `muhtari_probe`, a 140-byte `muhtari_cart_info`, five refusal statuses and the A78 constants (API 2). The loader stays internal until a loaded cartridge can run. The core computes only SHA-1; the runner prints all eight hashes | ✓ |
| Probe and load both now | Also adds `muhtari_load` with a minimal `muhtari_media`. This freezes the media struct before BIOS, region and ports are designed, and a loaded game would show the test card until phase 3 | |

**User's choice:** Probe now, load in phase 3.

**Advisor research:**
- **Messages.** A host-layer table, `host/status_text`, beat a library `muhtari_status_text`: it keeps English out of
  the core and the ABI.
- **Output.** `key value` lines beat JSON until `--capabilities` exists, since JSON needs an escaper for hostile
  titles.
- **What probe returns.** Probe returns load's status. The strongest alternative was "probe always OK, with kind and
  board carrying the verdict", which would make hosts reimplement the support matrix.
- **Hashes.** The advisor raised the printed hashes from six to eight, reading CPU-05 literally: headered and
  headerless SHA-1, SHA-256, MD5 and CRC-32. The headerless rule is rcheevos's.

→ D-01 to D-07.

---

## The vector sample and the nightly full set

| Option | Description | Selected |
|--------|-------------|----------|
| N65V binary sample, C converter, sparse git fetch nightly (Recommended) | nesturbator's format and reader unchanged, read at test time. The full set comes by a blobless sparse fetch of `6502/v1`, checked against 256 committed SHA-256s. One CTest per opcode | ✓ |
| Commit JSON prefixes instead | About 10 MB of text, with no converter. It would reopen DEC.39's "compact binary" | |
| 256 raw-URL pins through `fetch-pin.sh` | Reuses the script, but means 256 requests under GitHub's 2025 rate limits, uncompressed | |

**User's choice:** not asked; the advisor's recommendation was followed per the brief.

**Notes:**
- **No usable archive.** No whole-repository archive works, because the commit's tree is 5.17 GB across five CPUs
  (tested). The pins row's digest is therefore the SHA-256 of the per-file list, which adjusts phase 1 D-24's
  wording.
- **Klaus Dormann's test is dropped.** Its pass address is only in its GPL listing.
- **Bruce Clark's test stays.** It is transcribed from 6502.org and runs as printed (A, C, N, V and Z).

→ D-08 to D-13.

---

## Driving SALLY, the bus and RIOT before MARIA

| Option | Description | Selected |
|--------|-------------|----------|
| Link-time bus substitution and `muhtari__` entry points (Recommended) | `cpu.c` is one OBJECT library, linked with the real bus or a flat test bus. bus.c gets the HALT and NMI entry points phase 3's MARIA will call. Shipped code has no trace; bus tests act as the CPU. RIOT is tested through the real bus | ✓ |
| A template compiled twice, or a bus function pointer | Inlining, or one binary. But the template's tests run different machine code, and the pointer costs an indirect call each cycle | |
| A minimal MARIA, or a test-only build flag | Hardware-realistic or flexible. But MARIA is phase 3's scope, and the flag build is not the shipped code | |

**User's choice:** not asked; the advisor's recommendation was followed per the brief.

**Notes:**
- **Strongest alternative weighed: a pin-driven CPU** (floooh's `m6502_tick`). It was rejected under DEC.22, which
  settles a CPU-driven bus.
- **Fallback.** If a one-off Cachegrind check shows the call boundary costs more than 5 %, shipping one translation
  unit goes to the owner in phase 3.
- **Findings from reading the code:**
  - `docs.budgets` does not cover `docs/hardware/`.
  - `cpu_phase` has no user before MARIA.
  - TIA-range writes also write INPTCTRL until it locks, which shaped the RMW test operands.

→ D-14 to D-21.

---

## Probe evidence and the bundled tables

| Option | Description | Selected |
|--------|-------------|----------|
| Build `simple` on every leg with our own wasmtime recipe; a generated, committed CC0 table; run-time slices (Recommended) | The CMake `-P` recipe maps the package to `/b` (no MSYS path problem). `tools/cartdb.cmake` writes `src/cartdb.c`, about 19 KB, with 8-byte 2600 prefixes. Synthetic images cover every class. The fuzz target is `loader`, with max_len at cap + 1. The firmware scan runs per leg, in hygiene and in `publish.sh prepare` | ✓ |
| Build once on Linux and share through the Actions cache | One download, but a cache any pull request can restore is close to an artifact (DEC.47) | |
| A runner `--hash-list FILE` option | The CLI shows hash identity end to end, but adds a parser and a fuzz surface | |

**User's choice:** not asked; the advisor's recommendation was followed per the brief.

**Notes:**
- **New evidence: run-time keys.** Slice and half-swap keys cannot be generated from the XML, because SHA-1 does
  not compose. They are computed at run time.
- **New evidence: fuzzing length.** A max_len of 65,664 would cut every seed above 64 KiB, so the large-image
  branches would never be fuzzed.
- **Conflicts the advisor flagged:**
  - An invalid v4 block is a warning, not a refusal.
  - A size field above the body. The advisor proposed refusing every shortfall as truncated; the reconciliation
    kept MAME's min rule unless the remaining bytes fail the board's geometry (D-05).

→ D-22 to D-28.

---

## Claude's Discretion

These reconciliations were made across areas without asking:
- **Test names.** Probe tests are named `cart.*`, after ENGINEERING §3's per-subsystem files.
- **Truncation rule** (D-05). Kept MAME's min rule and refused only when the remaining bytes fail the board's
  geometry, because stale headers are common.
- **No database names in the shipped table.** The public struct carries none.
- **A test-only runner.** `muhtari-run-testdb` swaps in the test hash list at link time, so `--probe` shows
  hash-list identification end to end.
- **No new pin for the decimal test.** It is assembled with the pinned 7800basic package's `dasm.wasm`.
- **A new docs page.** `docs/hardware/cartridge.md` joins the three hardware pages.
- **Reference fixes in the same pull request:** seed pins, ENGINEERING §3, CONFORMANCE §1 and §8, ASSET_POLICY §3 and
  §5, and A7800-CARTRIDGES-AND-PERIPHERALS §1.4.
- **Folded todo.** `release.firmware_scan` (D-28).

The remaining discretion items are listed in CONTEXT.md.

## Deferred Ideas

- Public load and media (phase 3).
- MARIA WSYNC, MSTAT and `cpu_phase` (phase 3).
- A database name or digest in the info struct.
- A public status-string function.
- SEEDED RAM fill.
- JSON `--probe` output.
- The other 34 samples (phase 3).
- Rebuild-and-compare for the decimal test.
- Klaus Dormann's test.
- A runner `--hash-list` option.
- Shipping one translation unit.
