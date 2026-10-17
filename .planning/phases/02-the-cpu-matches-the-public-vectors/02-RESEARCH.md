# Phase 2: The CPU matches the public vectors - Research

**Researched:** 2026-10-16
**Domain:** NMOS 6502 (SALLY) emulation, 7800 bus timing, MOS 6532, A78 cartridge probing, CMake/CTest/CI plumbing
**Confidence:** HIGH for the code map, the tested facts and the decimal arithmetic; MEDIUM for the hardware timing details marked fitted; LOW only where tagged `[ASSUMED]`

<user_constraints>
## User Constraints (from CONTEXT.md)

### Locked Decisions
D-01 to D-31 in `02-CONTEXT.md` are locked (D-01 to D-07 chosen by the owner, the rest by four advisor agents with an
adversarial pass). The planner reads them there; this file does not restate them. Where research found one of them
wrong or unbuildable, see `## Conflicts with CONTEXT.md`.

### Claude's Discretion (verbatim)
- Internal names and layouts beyond those named above: the CPU register struct, the bus fields, the deadline
  mechanism, and the cartridge state inside `struct muhtari`.
- Whether `muhtari_create` or the internal load calls `muhtari__power_on`.
- How the core's OBJECT libraries are split for D-14 and D-23: for example `muhtari_cpu`, plus one OBJECT library
  for everything except `cpu.c` and `cartdb.c`.
- Linear's smallest size and step in the geometry table (D-05).
- Whether `title` keeps the trailing spaces some tools write.
- The wording of `--help` and the docs, within D-07's messages.
- How the one-off Cachegrind check in D-14 is run.

### Deferred Ideas (OUT OF SCOPE, verbatim)
- Public `muhtari_load` and `muhtari_media`, with BIOS, region, ports and the profile (phase 3).
- MARIA WSYNC at $24 with an `INC $24` case, MSTAT, and `cpu_phase` (phase 3).
- A database name or a digest field in `muhtari_cart_info`, added with an API version when a host asks.
- A public status-string function, if outside hosts ask.
- `ram_fill` SEEDED, with the public profile.
- JSON output for `--probe`, together with `--capabilities` (phase 3).
- The remaining 34 7800basic samples (phase 3).
- Rebuild-and-compare for the decimal test's binaries (phase 3's diagnostics lane).
- Klaus Dormann's functional test, only if the nightly misses a defect at the level of instruction sequences.
- A runner option `--hash-list FILE`, unless a user needs one.
- Shipping the core as one translation unit, only if D-14's one-off check shows more than 5 %; the owner decides.
</user_constraints>

<phase_requirements>
## Phase Requirements

| ID | Description (REQUIREMENTS.md) | Research support |
|----|-------------|------------------|
| CPU-01 | `ctest -L vectors` runs the committed SingleStepTests `6502` sample on final state and every bus cycle, decimal mode included | Sample size and layout measured (1,677,886 B); decimal ADC/SBC/RRA/ISC/ARR formulas checked against all 10,000 vectors of 69, e9, eb, 6b, 67, e7 with 0 mismatches; Clark listing assembles with the pinned dasm (§Verified facts F1–F4, §Code examples) |
| CPU-02 | `ctest -L vectors-full` runs the full set at its pinned commit nightly | Sparse blobless fetch measured: 193 MiB `.git`, 1 s fetch + 87 s checkout; per-file SHA-256 list computed and its digest pinned (F5); JUnit for a failed fixture verified (F6) |
| CPU-03 | `ctest -L sally`: 4/6-tick cycles, HALT gate, RDY reads-only, NMI rules | Bus/TIA/NMI rules with the precise NESdev polling semantics (Conflict C5), `INC $06` trace derived (F12), TIA WSYNC model (F13) |
| CPU-04 | `ctest -L riot`: timer through HALT and WSYNC, flags, ports, RAM, INPTCTRL, open bus | MOS 6532 datasheet read: one INTIM formula consistent with D-19, flag-clear and PA7 rules (F11) |
| CPU-05 | `--probe` prints the A78 fields, board, region, peripherals, hashes; distinct statuses; allocator test; loader fuzz | Header bytes of the built sample read (F9); cartdb generator timed at 0.03 s (F7); notices/abi/format gates that the generated file must pass (C1, C2, P4); own 7800basic recipe reproduces the pinned bytes (F8) |
</phase_requirements>

## Project Constraints (from CLAUDE.md / AGENTS.md)

- **No GPL source opened or kept.** MAME C/C++, Stella, a7800, ProSystem, js7800, MiSTer RTL, BizHawk, Klaus Dormann's
  listing: facts only, from `.planning/preparation/`. 7800basic's compiler (GPL-2) runs only as a tool; no text from
  `7800basic.sh` may be copied. MAME `hash/*.xml` (CC0), SingleStepTests (MIT), wasmtime (Apache-2.0), rcheevos (MIT),
  nesturbator and gabbaboy (MIT, same owner) may be read.
- **Three deliverables, no window/audio/input device code.**
- **Clean tree:** no ROM/BIOS/save bytes except fixtures in `tests/roms/manifest.txt`; no personal paths, emails or
  names; noreply identity. Cite siblings as `szTheory/nesturbator @9f9386c661f6 path`.
- **Deterministic, bounded, integer-only core;** all state in the instance; every call returns in bounded emulated time.
- **Every behaviour change lands with a test run by `cmake --workflow --preset ci`.** README short; maintainer docs in
  `docs/`; header terse.
- **Automation-first verification;** the owner verifies nothing by default.
- **Proportional gates:** check code is CMake `-P` or POSIX sh; each gate names its fault; from phase 3 check code stays
  under a tenth of `src/` lines (see Risk R6).
- **One GSD step per session; branch `phase/02-…` off `origin/main`; PR with Conventional Commit title** (`feat:` gives
  0.2.0, per `release-please-config.json` `"bump-minor-pre-major": true`).
- **Evidence is a command, an exit status and a run URL**, never a committed log.
- **Style:** small modules, fixed-width types, `muhtari_` public prefix, each module prefixes its names, hardware
  comments cite a document or `docs/hardware/*.md` section, no planning IDs in code or test names.
- **SUMMARY files** carry `requirements-completed:` and a block-style `coverage:` list (no flow style, no inline `#`).

## Summary

Phase 2 is large but almost entirely de-risked. The SALLY CPU is the nesturbator 2A03 core
(`szTheory/nesturbator @9f9386c661f6 src/cpu.c`, 1,381 lines) plus three changes: NMOS decimal mode in `adc`/`sbc`/`arr`,
NMI polling by φ2 index instead of a boundary flag, and the bus seam renamed to `muhtari__bus_read/write`. Its JAM,
SHA/SHX/SHY/TAS, ANE/LXA and raw-P handling already match D-12. The decimal formulas below were checked in this session
against every one of the 10,000 vectors of six affected opcodes with zero mismatches, so the planner can treat CPU-01 as
a porting task, not a research task.

The rest of the phase is plumbing whose risk sits in repository gates, not in hardware. This research found eight places
where a locked decision collides with an existing gate or tool (Conflicts C1–C8). Each has a one-line amendment: table
symbol names, the SPDX line shape, the frozen API v1 test, a `.gitignore` rule, the NMI branch wording, a vector address
in a test expectation, the empty-zip fixture, and the git timeout scope. None changes a decision's intent. The 7800basic
sample, the riskiest cross-platform item, reproduced its pinned bytes on aarch64-macos, x86_64-macos (Rosetta) and
aarch64-linux. An own recipe, derived only from each tool's usage text, reproduces them too. Windows can only be shown by
the PR's CI run.

**Primary recommendation:** Plan the phase as seven coarse plans in this order: CPU and vectors; bus, TIA and RIOT;
header, probe and cartdb; loader and fuzz; runner and the built sample; nightly and firmware scan; docs. Push the branch
to a draft PR right after the built-sample plan, so the Windows and be32 evidence arrives while the code is still fresh.

## Conflicts with CONTEXT.md

Each item is a tested fact that a locked decision does not survive as written. Each gives one recommended amendment.

| # | Decision | Evidence | Recommended amendment |
|---|---|---|---|
| C1 | D-24 names the tables `cartdb_7800_keys`, `cartdb_7800_entries`, `cartdb_7800_layouts`, `cartdb_2600_prefix` | `abi.symbols` rejects every defined external name not starting `muhtari_`: [VERIFIED: tests/cmake/abi.cmake:89-90] `list(FILTER public INCLUDE REGEX "^muhtari_[^_]")` / `list(FILTER external EXCLUDE REGEX "^muhtari_")`, then every remaining external is "outside the muhtari_ namespace". The tables must be external (cart.c and `tests/cart/test_cartdb.c` reference them across translation units). | Name them `muhtari__cartdb_7800_keys`, `muhtari__cartdb_7800_entries`, `muhtari__cartdb_7800_layouts`, `muhtari__cartdb_2600_prefix` (and their counts). The test list defines the same names. |
| C2 | D-24's first line `/* SPDX-License-Identifier: MIT AND CC0-1.0 — generated by … do not edit */` | `policy.notices` requires the line to end right after the licence: [VERIFIED: tests/cmake/notices.cmake:33] `elseif(NOT spdx MATCHES "SPDX-License-Identifier: MIT( AND (${admitted}))?( \\*/)?$")`. It also requires the libretro `.info` licence set to equal the union of SPDX and notice licences; today [VERIFIED: libretro/muhtari_libretro.info.in:7] `license = "MIT, BSD-3-Clause"`. | Line 1: `/* SPDX-License-Identifier: MIT AND CC0-1.0 */`; line 2: `/* Generated by tools/cartdb.cmake from MAME hash/a7800.xml and hash/a2600.xml at dddf4cd1faeeca6a6dac26ecbc2da2da4242b740; do not edit. */` (ASCII only, no em dash). Set `.info.in` to `license = "MIT, BSD-3-Clause, CC0-1.0"`. |
| C3 | D-01 (`MUHTARI_API_VERSION` becomes 2) with `tests/api/v1.c` marked "FROZEN: once released this file never changes" | [VERIFIED: tests/api/v1.c:27] `CHECK_EQ_U64(MUHTARI_API_VERSION, 1);`, [VERIFIED: tests/api/v1.c:45] `CHECK_EQ_U64(v.api, 1);`, [VERIFIED: tests/core/test_core.c:531] `CHECK_EQ_U64(v.api, 1);`. With 2, `abi.api_v1` and `core.info` fail. | Edit `v1.c` lines 27 and 45 to `CHECK(MUHTARI_API_VERSION >= 1);` and `CHECK(v.api >= 1);`, and amend its header comment: version 1's names and values are frozen, its version check is a floor. Change `test_core.c:531` to 2. `v2.c` checks `== 2`. |
| C4 | D-13 commits `tests/cpu/decimal/clark-6502.bin` and `clark-65c02.bin` | `git check-ignore -v tests/cpu/decimal/clark-6502.bin` printed `.gitignore:6:*.bin` (exit 0): the file would be silently ignored. | Add `!/tests/cpu/decimal/*.bin` to `.gitignore` beside `!/tests/roms/**`. `.gitattributes` already has `*.bin binary`. The D-13 manifest rows then satisfy `hygiene.sh`'s `ROM_NAME` rule. |
| C5 | D-16 / D-18 "A taken branch without a page cross compares against its cycle 2, so it polls early" | NESdev "CPU interrupts" (fetched 2026-10-16): normal instructions use "the status of the interrupt lines at the end of the second-to-last cycle". For a branch, "interrupts are always polled before the second CPU cycle (the operand fetch)" and "not before the third CPU cycle on a taken branch". BRK: NMI "during the first four ticks" takes the NMI vector. For a 3-cycle instruction, cycle 2 *is* the penultimate cycle, so "compares against cycle 2" is not early under the BRK rule's own wording (≤ T4). | Number an instruction's cycles 1…N and let `nmi_phi2` name the cycle k whose φ2 latched the edge. Taken after this instruction iff k ≤ N − 1. A taken branch without page cross (N = 3) uses k ≤ 1. A page-crossing taken branch (N = 4) uses k ≤ 3. BRK takes $FFFA iff k ≤ 4. `cpu.nmi.branch_no_cross_polls_early`: an edge in cycle 2 of a taken no-cross branch is taken after the *next* instruction, while the same edge in cycle 2 of `LDA zp` is taken after `LDA`. |
| C6 | D-23 `runner.probe.sample_headerless` "(`$FFF8 = $FF`, `$FFF9 = $F7`, reset `$F000`; tested)" | `tail -c 8 simple.bas.bin | od -An -tx1` on the pinned build printed `ff f7 00 f0 ad f4 5f f0`: NMI = $F000, **RESET = $F4AD**, IRQ = $F05F. | Tests and docs say reset $F4AD. The footer test still passes: start page $F0 ≤ reset page $F4. |
| C7 | D-28 runs the scan inside `publish.sh prepare`; `policy.publish_set` feeds `prepare` 18 empty placeholder zips | [VERIFIED: tests/hygiene/tests.cmake:197] `file(WRITE ${hygiene_publish}/full/muhtari-${c}-${o}-${a}.zip "")`. `cmake -E tar xf` on a 0-byte file exits **0** with no members (toy, tested). A truncated or garbage zip exits 1 ("Truncated ZIP file header", "Unrecognized archive format"). A scan that does not check member counts passes an empty or truncated release silently. | The scan fails closed on a zip that yields no member. The `policy.publish_set` fixture writes real one-member zips (`cmake -E tar cf X.zip --format=zip LICENSE`, as `tests/release/tests.cmake` already does). `publish.sh` finds the script and lists relative to `$(dirname "$0")/..` and runs `${CMAKE:-cmake}`, since the hygiene test runs it from the build directory. |
| C8 | D-11 sets `GIT_HTTP_LOW_SPEED_LIMIT/TIME` on the `fetch` step | Measured with git 2.41: `fetch --depth 1 --filter=blob:none` took 1 s (trees only), `checkout` took 87 s. The blobs download lazily during checkout. | Set both variables (`set(ENV{…})`) before the fetch **and** the checkout `execute_process`. The 600 s TIMEOUT stays. |

Minor clarification (not a conflict): D-15 says the TIA drivers "drive D7 or D7–D6 … and D5–D0 come from the open-bus
byte". For the D7-only registers (`CXBLPF` $06, `INPT0`–`INPT5` $08–$0D), D6 also comes from the open-bus byte
(A7800-SYSTEM §4: "TIA drives D7 (INPT0–5, CXBLPF) or D7–D6 (other collision regs)").

## Verified facts (the CONTEXT `<specifics>` list)

All scratch work ran in the session scratch directory (`plan2/research/`, one directory per download), outside the
tree. Host: macOS arm64, Docker `ubuntu:26.04` and `gcc:14` (aarch64), Rosetta for x86_64. Digests of every download equal the
CONTEXT/pins rows.

| # | Fact | Command (abridged) → exit status / result | Confidence |
|---|---|---|---|
| F1 | Sample size, per-file test count and maxima | `python3 -I sst_stats.py sst/6502/v1` → 0. 10,000 tests in every file. N65V sample of the first 100 per opcode = **1,677,886 B**. Max cycles 11 (`02.json`), max RAM pairs 9 (`00.json`), so the u8 counts and the 32-entry log are safe. | HIGH (tested) |
| F2 | 100th-test end offset across all 256 files | same script → **max 54,614 B (`c3.json`)**. A 56 KiB prefix holds 100 tests of every file. | HIGH (tested) |
| F3 | JSON layouts | same script → **24 compact files** (`[{` at byte 0): `02 0c 12 1c 22 32 3c 42 52 5c 62 72 7c 92 93 9b 9c 9e 9f b2 d2 dc f2 fc`. The other **232** start `[\n{` with spaced arrays. `cycles` triples are `[addr, value, "read"|"write"]`; `ram` lists are unsorted. | HIGH (tested) |
| F4 | Clark's decimal test assembles with the pinned dasm | Listing extracted from 6502.org Appendix B. Equates for the 17 bytes at $00–$10 and `ORG $2000` prepended, then `wasmtime run --dir=. dasm.wasm clark.s -f3 -oclark.bin -lclark.lst` → "Complete. (0)", exit 0, **343 B**. The 65C02 variant (`JSR A65C02`/`S65C02` substituted) → exit 0, 343 B, differs from byte 43. SHA-256 `1d4e8f0e…f083c` (6502) and `8bc70bb4…448e1` (65C02) for that transcription. | HIGH (tested) |
| F5 | Sparse blobless fetch of `6502/v1` | `git init; sparse-checkout set --cone 6502/v1; fetch --depth 1 --filter=blob:none origin 2f6980a2…; checkout FETCH_HEAD` → 0. fetch 1 s, checkout 87 s, `.git` **197,648 KiB**, 256 files, 1,081,886,574 B. CMake `file(SHA256)` over all 256 takes **2.3 s**. The list `<sha256>  6502/v1/xx.json` (LF, 00…ff) has SHA-256 **`22df14296ebc6a02d69648467b52ab23feb4083f97aca1174a77d2902a5bd1e1`**, identical whether built by `shasum` or by CMake `file(GLOB)` + `file(SHA256)`. The lines are sorted by path, **not** as whole lines (`sort -c` fails at line 2). | HIGH (tested) |
| F6 | CTest JUnit for a test whose fixture setup failed | Toy project: setup test fails. Dependent test → `status="notrun"` with `<skipped message="Fixture dependency failed"/>`, ctest exit 8, and `check_junit.cmake` exits 1 naming both. Nothing passes silently. | HIGH (tested) |
| F7 | `tools/cartdb.cmake` feasibility on a2600.xml | A toy `-P` script reads both XMLs: regex `sha1="…"`, 8-byte prefixes, sort, rows; splits a7800 into `<software` blocks. → 0, **0.03 s**. 1,601 a2600 SHA-1s, **1,601 unique 8-byte prefixes** (no prefix collision). 204 a7800 entries, 214 SHA-1s. 2600 rows are 47 columns. | HIGH (tested) |
| F8 | 7800basic `simple` on other hosts, and an own recipe | `sh 7800basic.sh simple.bas` (run as a tool, not read) with x86_64-macos wasmtime under Rosetta → 0, 1.3 s. Same in Docker aarch64-linux → 0. Both give `.a78` `d09da3ed…4ecf`, `.bin` `03a8b7be…6b29`. An own recipe, derived from each tool's usage output only (see Code examples), → every stage 0, same two digests, in a fresh 4-deep directory, 0.13 s (arm64) and 1.0 s (x86_64 Rosetta). | HIGH (tested) |
| F9 | Sample header and footer | `od` of the `.a78`: version $04; magic `ATARI7800` + 7 spaces; title `My Game` + spaces, **no NUL in the 32 title bytes**; size $00008000; cart type $0000; ports 1/1; v4 block zero. Footer as C6. | HIGH (tested) |
| F10 | wasmtime `--dir HOST::GUEST` with a drive letter | [CITED: wasmtime v48.0.6 `src/common.rs` L120–128] `parse_dirs` does `s.split("::")`, takes the first part as host and the second as guest, and ignores any later parts. `D:\a\x\pkg::/b` and `D:/a/x/pkg::/b` give host `D:\a\x\pkg`, guest `/b`; a single `:` is never a separator. Running it on Windows is left to CI (Environment). | HIGH (source read) |
| F11 | MOS 6532 rules | `pdftotext` of the datasheet (archive.6502.org): "When the timer is read prior to the interrupt flag being set, the number of time intervals remaining will be read, i.e., 51, 50, 49". Worked example: 52 at ÷8, "total time to interrupt is (52 x 8) + 1 = 417T"; after interrupt "the timer register decrements at a divide by 1 rate". "whenever the timer is written or read the interrupt is reset. However, the reading of the timer at the same time the interrupt occurs will not reset the interrupt flag." "The PA7 flag is cleared when the Interrupt Flag Register is read. The timer flag is cleared when the timer register is either written or read." Flag reads put "D0-D5 … to 0". "The setting of the internal interrupt flag by an active transition on PA7 is always enabled, no matter whether PA7 is set up as an input or an output." RES zeroes "all four I/O registers", disables interrupts, selects the negative edge, and the flag "may also be set by changing the polarity". Formula in Patterns, Pattern 5. | HIGH (primary source) |
| F12 | `INC $06` from $1800 writes $06 then $07 | Derived from D-15's rules: the operand fetch leaves open bus = $06. `$06` is `CXBLPF` (TIA drives D7 only, idle 0, D6–D0 from open bus) → read $06 → dummy write $06 → write $07. Both writes are unlocked TIA-range writes, so the count goes 0 → 2 and INPTCTRL ends $07. Timing 4 + 4 + 6 + 6 + 6. | HIGH (derived from locked rules) |
| F13 | TIA WSYNC release | Stella Programmer's Guide (Sinnett reconstruction, §3.2): WSYNC halts the CPU until horizontal blank begins; RDY is set again at the leading edge of horizontal blank. On the 7800 the TIA line is 456 ticks (228 colour clocks × 2) [A7800-SYSTEM §1, §3]. Model in Patterns, Pattern 4. | MEDIUM (secondary sources; edge tick fitted) |
| F14 | Linking hidden `muhtari__` internals from a static archive | Mach-O: `cc -fvisibility=hidden -fPIC -c lib.c; ar rcs; cc main.c libx.a` → 0, prints `42 2`; `nm -m` shows `private external`; a dylib exports none. ELF (gcc:14 container): `readelf` shows `GLOBAL HIDDEN`, links and runs → 0; the `.so` dynsym has no `muhtari__`. MSVC has no visibility, so every `.lib` external links. wasm-ld resolves hidden symbols statically and hidden only affects exports [ASSUMED for wasm, MSVC; proven by the phase's own CI legs]. | HIGH (ELF/Mach-O tested) |
| F15 | A STATIC library made only of `$<TARGET_OBJECTS:…>`, with link-time substitution | Toy: `add_library(core STATIC $<TARGET_OBJECTS:core_objs> $<TARGET_OBJECTS:db_objs>)`, `install(EXPORT)` → 0. An exe of `core_objs` + a test `b()` used the test definition; the shipped exe used the real one. | HIGH (tested) |
| F16 | What `publish.sh prepare` leaves for the scan | [VERIFIED: release.yml:72-82] artifacts `archives-*` merged into `dist/` (18 zips at top level), `hashes-*-*` into `dist/legs/hashes-<os>-<arch>/{hashes.txt,commit.txt}`, then `sh scripts/publish.sh prepare dist` from the checkout root, so `tests/`, `src/cartdb.c` and `tests/built/expected.txt` are on disk. `prepare` writes only `dist/SHA256SUMS`. Its "unexpected" check looks only at regular files in `dist/`, so the scan must extract outside `dist/` (`$RUNNER_TEMP`, or a `mktemp -d`). | HIGH (read) |
| F17 | Decimal arithmetic | Model from 6502.org Appendix A (Seq. 1 for A and C, Seq. 2 for N and V, Z binary; SBC Seq. 3 with all flags binary) and 64doc ARR: **0 mismatches** over 10,000 tests each of 69, e9, eb, 6b, 67 (RRA) and e7 (ISC). The simple V formula `~(A^B) & (A^S) & 0x80` on the Seq. 1c sum equals Seq. 2 over all 131,072 inputs. An unsigned-only SBC formulation equals the signed one over all inputs. | HIGH (tested) |
| F18 | Pinned asset layouts | `7800basic-0.41-wasm.tar.gz` (1,447,952 B, 429 entries) has top dir `7800basic/`, so `--strip-components=1` works. The wasmtime zips have top dir `wasmtime-v48.0.6-x86_64-windows/` (42 MB unpacked), and `cmake -E tar xf` cannot strip it. Download sizes: macOS x64 10.7 MB, Windows x64 13.0 MB, Windows arm64 11.9 MB. | HIGH (tested) |

Not checkable here, and how each is settled: Windows bytes and the `D:` path at run time (`built.samples` on the two
`windows-*` legs of the phase PR). macOS x64 at run time (the `macos-15-intel` leg). be32 wall time (the `be32` job).
MSVC and wasm32 linking of `muhtari__` internals (the ci-msvc legs and the `wasm32` job). The HALT latency stays fitted,
by DEC.24. PROPack headers and XM-flagged homebrew need ROM bytes, so they are recorded as assumptions A4 and A5; D-25's
synthetic images cover both outcomes.

## Architectural Responsibility Map

| Capability | Primary tier | Secondary | Rationale |
|---|---|---|---|
| SALLY instruction execution | Core `src/cpu.c` (OBJECT `muhtari_cpu`) | — | Pure CPU; memory only via `muhtari__bus_read/write` |
| Cycle timing, decode, open bus, INPTCTRL, HALT gate, RDY, NMI latch, deadlines | Core `src/bus.c` | `src/tia.c` (WSYNC/RDY source, read drivers) | One place where time advances (ARCHITECTURE §2) |
| RIOT timer/ports/RAM | Core `src/riot.c` | bus.c (φ2 count) | Lazy: computed at read from φ2 |
| Probe, identify, header lint, geometry | Core `src/cart.c` + `src/sha1.c` | generated `src/cartdb.c` (data) | Pure, allocation-free |
| Internal loader | Core `src/cart.c` (`muhtari__cart_load`) | allocator from instance | Validate before allocate |
| Status text (English) | `host/status_text.c` | runner, phase 3 libretro | Keeps prose out of the core/ABI |
| `--probe`, digests (sha256/md5/crc32), file read | Runner | `src/sha1.h` reused | Host-side I/O |
| Table generation | `tools/cartdb.cmake` (dev tool) | hygiene lane check | Build-time, committed output |
| Vector conversion/reading | Test code (`tests/vectors/`) | — | D-09: not core, not check code |
| Built sample, firmware scan, fetches | CMake `-P` / POSIX sh | CI workflows | Repository-check code |

## Existing code map (what each change extends)

Line counts are current; ENGINEERING limits in brackets.

| File (lines) | Current role | Phase 2 change and pattern to follow |
|---|---|---|
| `include/muhtari.h` (72; budget 24,576 B, now 2,187) | v1 API | Append D-02 verbatim after `muhtari_run_frame`. Keep one line per field. Every new name must appear in `docs/embedding.md` (`docs.api_names`, [VERIFIED: tests/cmake/api_names.cmake:14] `set(name_regex "muhtari_[a-z0-9_]+|MUHTARI_[A-Z0-9_]+")`, every match in the header) and in `tests/api/v2.c` (`abi.api_version`). |
| `src/internal.h` (95) | `struct muhtari { allocator; audio_rem; }`, size-first macros, `_Static_assert` layouts, `muhtari__check_size` | Grow `struct muhtari` with `cpu`, `bus`, `tia`, `riot`, `profile` (D-20), `cart` sub-structs. Add `MUHTARI__CART_INFO_SIZE_FIRST` and 28 `_Static_assert`s (offsets 0…104 step 4, `title` at 104, `sizeof` 140), following lines 49–86. Declare `muhtari__bus_read/write`, `muhtari__bus_request_halt`, `muhtari__bus_raise_nmi`, `muhtari__power_on`, `muhtari__cpu_step`, `muhtari__cart_identify`, `muhtari__cart_load`, `muhtari__write_out`. |
| `src/instance.c` (126) | `muhtari__check_size`; `static void write_out` at [VERIFIED: src/instance.c:37] | Rename `write_out` to `muhtari__write_out` (extern, declared in internal.h) so `cart.c`'s probe reuses it. `muhtari_create` calls `muhtari__power_on` after the memset (discretion; recommended, so a created instance is always in the power-on state). `muhtari_destroy` frees the cart ROM copy (size recorded in the instance) before the instance. |
| `CMakeLists.txt` (202) | `muhtari` STATIC from three sources; options inline (L45–51); nofp adds `-mgeneral-regs-only` to `muhtari` only [VERIFIED: CMakeLists.txt:170] `target_compile_options(muhtari PRIVATE -mgeneral-regs-only)` | Add `function(muhtari_core_options target)`: hidden visibility, PIC, the L48–50 flags, nofp's flag when `MUHTARI_NOFP`, `muhtari_warnings`, include dirs (`include/`, `generated/`). OBJECT libs: `muhtari_objs` (instance, frame, testcard, bus, tia, riot, cart, sha1), `muhtari_cpu` (cpu.c), `muhtari_cartdb` (cartdb.c). `add_library(muhtari STATIC $<TARGET_OBJECTS:muhtari_objs> $<TARGET_OBJECTS:muhtari_cpu> $<TARGET_OBJECTS:muhtari_cartdb>)` (F15). Apply the function to all four, so `get_target_property(abi_options muhtari COMPILE_OPTIONS)` in `tests/abi/tests.cmake:14` still copies them to the abi fixtures. Add `host/status_text.c` to `muhtari_host`, and `runner/md5.c` plus `src/sha1.c`'s header include path to `muhtari-run`. Options `MUHTARI_VECTORS_FULL`, `MUHTARI_FUZZ` (see below) select inventories as `MUHTARI_HYGIENE` does (L184–190). |
| `CMakePresets.json` (302) | ci/ci-msvc/hygiene/asan/nofp/be32/wasm32/bench | Add `vectors-full` configure (inherits `ci`, `MUHTARI_VECTORS_FULL=ON`, `MUHTARI_INVENTORY=vectors-full`), build, test (filter label `^vectors-full$`, JUnit, `noTestsAction: error`), workflow (configure, build, test; no package). Recommended: `fuzz` configure (clang-21, `MUHTARI_FUZZ=ON`, `MUHTARI_INVENTORY=fuzz`), build, test and workflow presets, so the nightly job runs the same command as a local run. |
| `cmake/MuhtariTests.cmake` (69) | `muhtari_add_test`, exact inventory check | Unchanged. Fixture tests set `FIXTURES_SETUP/REQUIRED` with `set_tests_properties` after `muhtari_add_test`. |
| `tests/tests.cmake` (25) | includes per subsystem; hygiene/nofp early `return()` | Add `cpu bus riot cart built fuzz` includes. Add early-return branches: `MUHTARI_VECTORS_FULL` includes only `cpu/full.cmake`; `MUHTARI_FUZZ` includes only `fuzz/tests.cmake`. Put the CPU test targets in `tests/cpu/targets.cmake`, included by both `cpu/tests.cmake` and `cpu/full.cmake`. |
| `tests/inventory/*.txt` (ci 72, wasm32 69, nofp 13, hygiene 20) | exact, sorted name⇥labels | ci and wasm32 gain about 330 lines, including 256 `cpu.vectors.XX`; generate the sorted block with a shell loop once. **nofp gains `abi.api_v2`** (abi tests register there). hygiene gains `policy.cartdb_generated(.self_test)`, `policy.rom_hash_coverage(.self_test)`, `hygiene.firmware_scan(.self_test)`. New `vectors-full.txt` (258) and `fuzz.txt` (2). |
| `tests/abi/tests.cmake` (116) | abi checks, `abi.api_v1` executable | Add `muhtari-test-api-v2` / `abi.api_v2` (labels `abi;portable`) next to v1 (L66–73). |
| `tests/cmake/*.cmake` | `expect_fail` (19), `check_junit` (58), `budgets` (30), `pins` (49), `abi` (107), `notices` (57), `no_skip` (38) | `budgets.cmake` L11: add `file(GLOB … "${ROOT}/docs/hardware/*.md")` at 40,960. New: `fetch_vectors.cmake`, `vectors_pin.cmake`, `firmware_scan.cmake`, `rom_hash_coverage.cmake`. Every `-P` script starts `cmake_minimum_required(VERSION 3.25)` (nesturbator's lesson: script mode has no policies, and IN_LIST misbehaves). |
| `tests/core/test_core.c` (593) | counting allocator (`counts`, `count_alloc`, `count_free`, all `static`), `size_cases` with `union tagged` sized [VERIFIED: tests/core/test_core.c:341] `uint8_t bytes[sizeof(muhtari_frame) + sizeof(muhtari_config) + TAIL];` | Move the counting allocator into a shared `tests/alloc.h` (static inline, like `tests/check.h`) used by test_core.c, the cart tests and the fuzz harness. Add `muhtari_cart_info cart;` to `tagged` and size `bytes` to `sizeof(muhtari_cart_info) + TAIL + 8` so the +8 cases fit. Add `size_cases("cart_info", …, 1, call_probe, m)` with a valid synthetic image. L531 per C3. |
| `tests/api/v1.c` (99) | frozen v1 | C3 edit only. |
| `runner/main.c` (371) | parse at odd positions, `--frames/--bench/--hash-frame/--dump-frame/--help/--version` | `--probe FILE`: accept only `argc == 3`; otherwise exit 2. The read loop, output and messages are D-07. Hashes reuse `muhtari__sha256_*` and `muhtari__crc32(crc, p, n)` (runner/crc32.h); add `runner/md5.c` with the same init/update/final shape as `runner/sha256.h`. |
| `runner/exit_status.h` (23) | table printed by `--help` as `"  %d  %-7s  %s"` | Add `{77, "refused", …}` (7 chars fits `%-7s`) and widen status 1's text. Copy both lines verbatim into `docs/runner.md`'s fenced table (`runner.exit.table` matches whole lines). |
| `host/` | `palette.{c,h}` in `muhtari_host` (PIC, hidden) | Add `status_text.{c,h}`: `const char *muhtari__status_text(muhtari_status)`, plus the board name for "unsupported board: <name>". |
| `pins.txt` (22) | rows `name version url sha256`, checked by `policy.pins` | Add `sst-6502-v1 2f6980a2d95757486c7bee24355c360e40e2a224 https://github.com/SingleStepTests/65x02.git 22df14296ebc6a02d69648467b52ab23feb4083f97aca1174a77d2902a5bd1e1` with a comment that `fetch-pin.sh` does not serve it. Add the D-22 rows (`wasmtime-x86_64-macos`, `wasmtime-x86_64-windows`, `wasmtime-aarch64-windows`, `7800basic-wasm`) and the D-24 rows (`mame-a7800-xml`, `mame-a2600-xml`). All are https with 64-hex digests, so they pass `pins.cmake`. |
| `scripts/fetch-pin.sh` (52) | `set -f` (noglob) at [VERIFIED: scripts/fetch-pin.sh:12] `set -f`; unpack [VERIFIED: scripts/fetch-pin.sh:49] `*.tar.xz \| *.tar.gz) tar -xf "$file" -C "$dest" --strip-components=1 ;;` | Add `*.zip)`: extract into `$tmp/x` with `(cd "$tmp/x" && cmake -E tar xf "$file")`, then copy the single top directory's contents into `$dest`. Globbing is off, so use `find "$tmp/x" -mindepth 1 -maxdepth 1 -type d`, or `set +f` locally, and fail unless exactly one directory. |
| `scripts/hygiene.sh` (177) | `ROM_NAME` rule includes `.bin`; A78-magic and NUL rules exempted by `in_manifest` (first field) | No code change. The sample (NULs), Clark bins (`.bin`, NULs) and fuzz regressions need manifest rows. |
| `scripts/publish.sh` (176) | `prepare` L34–57 | Insert the scan after the commit.txt loop and before L55 (`SHA256SUMS`), per C7. |
| `.github/workflows/ci.yml` (477) | build matrix L119–126; asan, be32, wasm32 jobs | Add a `wasmtime:` key per matrix row (`x86_64-linux`, `aarch64-linux`, `aarch64-macos`, `x86_64-macos`, `x86_64-windows`, `aarch64-windows`). Add a step before "Build, test, package": `scripts/fetch-pin.sh wasmtime-${{ matrix.wasmtime }} build/pins/wasmtime && scripts/fetch-pin.sh 7800basic-wasm build/pins/7800basic`. Add the same two fetches to asan and be32; wasm32 already fetches wasmtime (L272) and gains the 7800basic line. The hygiene job gains the two XML fetches. Its `timeout-minutes: 1` (2 × 27 s) needs re-measuring, and the comment must keep a run ID or say provisional (`hygiene.timeout_comments`). |
| `.github/workflows/nightly.yml` (146) | bench ×3, bench-wasm32, report | Add `vectors-full` and `loader-fuzz` jobs (`timeout-minutes: … # provisional`), add both to `report.needs`, and add the vector, cart and fuzz paths to `pull_request.paths`. Recommendation: one workflow-level path union, accepting that the bench jobs also run on such PRs (minutes, never a gate), rather than adding a path-classifier job. |
| `.github/workflows/release.yml` (244) | rehearsal on PRs touching `scripts/publish.sh` | No edit. D-28 touches `publish.sh`, so the phase PR runs a rehearsal (expected). |
| `docs/embedding.md` (5,829 B), `runner.md` (5,224 B), `testing.md` (26,787 B) [40,960 each] | user/maintainer docs | `testing.md` has about 14 KB of headroom. Keep phase-2 additions under 8 KB by putting hardware facts in `docs/hardware/*.md`. |
| `THIRD-PARTY-NOTICES.md` (70) | `**Applies to:**`/`**Licence:**` sections | Add `singlesteptests` (MIT, Thomas Harte et al, LICENSE text; applies to `tests/vectors/6502-sample.n65v` and `tests/vectors/fixtures/*.json` by name) and "MAME software lists" (CC0-1.0; applies to `src/cartdb.c`; the PROVENANCE §7 courtesy line). Every backticked path must exist. |
| `.gitattributes` (5), `.gitignore` (38) | `/tests/roms/** -text`, `*.bin binary`; `*.bin` ignored except `!/tests/roms/**` | Add `*.n65v binary`, `tests/vectors/fixtures/*.json -text`, and C4's negation. |

## Sibling seeds: what carries over

**`szTheory/nesturbator @9f9386c661f6 src/cpu.c` (1,381 lines) → `src/cpu.c`.** Carries over unchanged (after renames):
the addressing helpers (64doc cycle order, dummy reads, `index_base` always-dummy for writes/RMW); RMW double write;
stack and `pull_p`/`push_p` (B and bit 5 only on the stack, which is what raw-P vectors expect); JAM's 11 reads; `store_sh`
(`reg & (H + 1)`, high byte replaced on a page cross, which matches D-12); ANE `(A | k) & X & imm` and LXA
`X = A = (A | k) & imm`; LAS; all other unofficial opcodes. The non-decimal opcodes are generated by the same harness as
nes6502/v1, which the seed passes [ASSUMED from its passing nightly; the per-PR sample re-proves it in this phase].

SALLY needs, and the seed lacks:
1. **Decimal mode.** The seed's ADC "ignores D" (comment at L157–160). Add the Code example's `adc`/`sbc` decimal paths;
   `arr` gains the 64doc decimal path. RRA and ISC call `adc`/`sbc` and need nothing else (F17).
2. **NMI by φ2 index.** The seed checks a boundary `nmi_pending` flag and IRQ `poll_latch` at the top of `cpu_step`
   (L494–523). Replace them with the C5 rule at the end of each instruction, using `m->bus.phi2` (cycles completed) and
   `m->bus.nmi_phi2` (cycle number that latched, 0 = none). Record the φ2 of cycle 1 for branches and of cycle 4 for BRK.
3. **No IRQ input in phase 2.** The 7800 has no on-board IRQ source; RIOT IRQ is unconnected (A7800-SYSTEM §3). Drop
   the seed's IRQ path and add it with the first IRQ source (cart POKEY, phase 4), so no untested branch ships.
4. **The jammed branch at the top of `cpu_step`** (the seed reads $FFFF per call) leaves cpu.c. A jammed CPU is the run
   loop's to idle (D-12, DEC.33), and the harness asserts `jammed` instead.
5. **The profile:** `ane_magic`/`lxa_magic` become one `m->profile.ane_lxa_constant`. Drop the DMC/RDY note in `store_sh`.

**`szTheory/nesturbator @9f9386c661f6 tests/vectors/n65v.{c,h}` (205/91), `tools/vecconv/vecconv.c` (606),
`tests/cpu/{test_vectors.c (206), vector_bus.{c,h} (43/28)}`.** The N65V reader carries over unchanged. Its
`n65v_init(r, buf, len, first_opcode, chunks, tests_per_chunk)` already enforces exact chunk and test counts. vecconv's
JSON reader becomes `tests/vectors/sst_json.c` (D-09); the CLI shrinks to `vecconv IN OUT XX [--first N]`. The harness
changes its argv to `cpu_vectors FILE XX`, accepts `.json` through `sst_json`, adds the `jammed` assertion and D-10's
output strings, sets `ane_lxa_constant = $EE`, and keeps raw P load/compare and `clear_ram`. `vector_bus.c` keeps
its cast from `struct muhtari *` to the containing struct (C17 6.7.2.1p15), and now also increments `m->bus.phi2` per
call and can raise `nmi_phi2` at a scripted cycle for the `cpu.nmi.*` cases.

**`… tests/cmake/fetch_vectors.cmake` (238).** Reuse its in-source guard (`cmake_path IS_PREFIX` plus `REAL_PATH`, with
`build/` allowed) and its marker-file deletion rule. D-11 replaces its per-file size+hash list with the `6502-v1.sha256`
list.

**`… tests/fuzz/replay_main.h` (70).** Carries over (macro prefix `MUHTARI_`). Its `fseek/ftell` on `long` is safe for
seeds of at most 64 MiB. Keep its "no corpus seeds supplied" → exit 1 path; `fuzz.regress.loader.self_test` expects
D-26's `no corpus inputs`, so align the string.

**`… CMakeLists.txt` L50–80.** The OBJECT-library pattern (`add_library(x_cpu OBJECT src/cpu.c)` +
`target_sources(lib PRIVATE $<TARGET_OBJECTS:x_cpu>)`) is the D-14 pattern.

**`szTheory/gabbaboy @a175a39 include/gabbaboy/gabbaboy.h` `gbb_probe_rom`.** Confirms the "pure probe returns the
load's verdict; outputs unchanged on error" contract. D-03 adds the one exception (2600 and unsupported board write
`out`).

## Standard Stack

No package-manager dependency is added. Everything external is a pinned download, verified by SHA-256 before use.

| Item | Version / pin | Purpose | Status |
|---|---|---|---|
| CMake | ≥ 4.3 (presets `cmakeMinimumRequired`); local 4.4.3 | build, `-P` checks, `file(SHA256/SHA1)`, `cmake -E tar` | existing |
| wasmtime | 48.0.6 LTS: existing x86_64-linux, aarch64-linux, aarch64-macos; new x86_64-macos `cf05506b…1c85`, x86_64-windows `89bec9b9…813a`, aarch64-windows `a2d54979…02c4` | runs the 7800basic tools and the wasm32 tests | digests re-verified this session (`shasum -a 256`) |
| 7800basic | v0.41 `7800basic-0.41-wasm.tar.gz` `62ef0a56…7fe7` | `simple` sample; its bundled `dasm.wasm` assembles Clark's test | verified |
| MAME software lists | `dddf4cd1faee…b740` a7800.xml `5343714e…66b9`, a2600.xml `efe03008…197c` | cartdb generator input | verified |
| SingleStepTests 65x02 | `2f6980a2…a224` | vectors; list digest `22df1429…d1e1` | verified (F5) |
| clang-21 + libFuzzer | apt on ubuntu-26.04 (unpinned, signed, per `pins.txt` policy) | nightly `loader-fuzz` | existing image/apt |
| valgrind | apt | the one-off D-14 Cachegrind check, run in an `ubuntu:26.04` container | run once |

## Package Legitimacy Audit

No npm, PyPI or crates package is installed in this phase, so the `package-legitimacy` seam does not apply. Pinned
artifacts come from their projects' own release or raw URLs, and each is checked by SHA-256 against `pins.txt` before
use (`scripts/fetch-pin.sh`; `policy.pins` and its self test).

| Artifact | Source | Digest matches pin | Disposition |
|---|---|---|---|
| wasmtime 48.0.6 (6 assets) | github.com/bytecodealliance/wasmtime releases | yes (all four fetched this session) | Approved |
| 7800basic 0.41 wasm | github.com/7800-devtools/7800basic releases | yes | Approved (tool only, GPL-2) |
| a7800.xml, a2600.xml | raw.githubusercontent.com/mamedev/mame @dddf4cd1 | yes | Approved (CC0) |
| 65x02 `6502/v1` | github.com/SingleStepTests/65x02 @2f6980a2 | per-file list (F5) | Approved (MIT) |

**Packages removed:** none. **Flagged:** none.

## Architecture Patterns

### System diagram (data flow)

```
 tests / fuzz harness / phase-3 run loop
        │ muhtari__cpu_step(m)
        ▼
  cpu.c (SALLY) ──one call per cycle──► muhtari__bus_read(m,a) / muhtari__bus_write(m,a,v)
        ▲  reads m->bus.phi2, m->bus.nmi_phi2,           │
        │  m->profile.ane_lxa_constant                   ▼
        │                                   bus.c: cycle length by decode (4 / 6 ticks; all 6 while MEN=0)
        │                                     ├─ fire deadlines ≤ data tick (TIA WSYNC release, HALT window, NMI)
        │                                     ├─ RDY? (reads only) → repeat whole read cycles until release
        │                                     ├─ HALT window (gate == 2) → c0, c1 run, c2 frozen m cycles
        │                                     ├─ decode ─► TIA ($00–$1F + mirrors; unlocked writes → INPTCTRL, gate++)
        │                                     │          ├► MARIA range (phase 2: open bus, fast)
        │                                     │          ├► RAM ($0040–$00FF, $0140–$01FF, $1800–$27FF)
        │                                     │          ├► RIOT ports/timer ($0280–$02FF) / RIOT RAM ($0480–$04FF)
        │                                     │          └► cart / BIOS / open bus
        │                                     └─ open-bus byte ← every read and write
        └── NMI latch: muhtari__bus_raise_nmi(m, at_tick) (ignored while MEN=0)

 bytes ─► muhtari_probe ─► size cap ─► A78 magic? ─► header parse (v3, v4 if valid) ─► geometry ─► SHA-1 of mapped body
                                     │                                                    (+ slices, + Activision half-swap)
                                     └► headerless: 7800 table → 2600 prefixes → footer test → 2600 sizes → invalid
                                                      └► muhtari__cart_identify(tables as args) ─► muhtari_cart_info / status
 runner --probe: fread loop ─► muhtari_probe ─► key/value lines + 8 digests (core SHA-1 via src/sha1.h) ─► exit 0/1/77
```

### Recommended file layout (new files)

```
src/        cpu.c bus.c tia.c riot.c cart.c sha1.c sha1.h cartdb.h cartdb.c(generated)
host/       status_text.c status_text.h
runner/     md5.c md5.h
tools/      cartdb.cmake
tests/      alloc.h
  vectors/  n65v.c n65v.h sst_json.c sst_json.h vecconv.c 6502-sample.n65v 6502-v1.sha256 fixtures/{a9,02}*.json fixtures/README.md
  cpu/      tests.cmake full.cmake targets.cmake test_vectors.c vector_bus.c vector_bus.h test_cpu.c decimal/{clark.s,clark-6502.bin,clark-65c02.bin}
  bus/      tests.cmake test_bus.c
  riot/     tests.cmake test_riot.c
  cart/     tests.cmake test_cart.c test_cartdb.c images.c images.h
  built/    tests.cmake build.cmake expected.txt
  fuzz/     tests.cmake loader.c seeds.c replay_main.h loader/regress/
  release/  firmware.txt
  roms/     manifest.txt
  api/      v2.c
  cmake/    fetch_vectors.cmake vectors_pin.cmake firmware_scan.cmake rom_hash_coverage.cmake
docs/hardware/  sally.md bus.md riot.md cartridge.md
```

### Pattern 1: one CPU object, two buses (D-14)
`muhtari-test-cpu` = `$<TARGET_OBJECTS:muhtari_cpu>` + `tests/cpu/vector_bus.c` + harness, and does **not** link `muhtari`.
The linker then rejects any cpu.c reference outside the two bus functions and the struct fields. `muhtari-test-bus` and
`muhtari-test-riot` link `muhtari` (the real bus) and add `src/` to their include path; they are the first tests to
include `src/internal.h`. `muhtari-run-testdb` = runner sources + `$<TARGET_OBJECTS:muhtari_objs>` +
`$<TARGET_OBJECTS:muhtari_cpu>` + `tests/cart/test_cartdb.c` + `muhtari_host` (F15).

### Pattern 2: NMI without per-cycle cost (D-16 with C5)
```c
/* At instruction end; phi2 counts completed cycles, so this instruction's
   cycle j has phi2 index start + j. nmi_phi2 == 0 means none latched
   (docs/hardware/sally.md, NMI; NESdev "CPU interrupts"). */
static int nmi_due(const muhtari *m, uint64_t poll_cycle_phi2)
{
    return m->bus.nmi_phi2 != 0u && m->bus.nmi_phi2 <= poll_cycle_phi2;
}
/* normal: poll_cycle_phi2 = m->bus.phi2 - 1 (penultimate)
   taken branch, no page cross: start + 1 (cycle 1)
   BRK vector choice: start + 4, read during BRK before cycle 5 */
```
The φ2 counter starts at 0 and the first cycle completes as φ2 1, so a latched edge is never 0.

### Pattern 3: cycle length and deadlines in `bus.c`
Fast = 4 ticks with data at tick 4; slow = 6 ticks with data at tick 5 (ARCHITECTURE §2, C4). Each call:
`end = ticks + len`, fire every deadline ≤ the data tick from a cached `next_deadline` (one compare on the fast path),
perform the access, update open bus, `ticks = end`, `phi2++`. HALT and RDY add whole 4-tick φ2 cycles, and `phi2` counts
them, so the RIOT counts through both (D-19).

### Pattern 4: TIA WSYNC release (fitted model)
`tia_phase` = the TIA horizontal counter, in ticks 0–455, at tick 0. A WSYNC write whose data tick is `t_w` sets
`rdy_release = the smallest t > t_w with (t + tia_phase) % 456 == 0`. The next read call repeats whole fast cycles while
the cycle's start < `rdy_release`, then runs in 4 ticks; writes are never held [ASSUMED: release edge = counter wrap,
held read completes in the first cycle starting at or after release; D-18's `bus.rdy.*` expectations encode the rule,
and docs mark it fitted]. Use unsigned 64-bit `%`; on ppc32 it lowers to `__umoddi3`, which `abi.undefined` allows
([VERIFIED: tests/cmake/abi.cmake:36] `"^Linux/(ppc|powerpc)$:__udivdi3 __umoddi3 __ashldi3 __lshrdi3"`). Signed 64-bit
division (`__divdi3`) is not allowed.

### Pattern 5: lazy 6532 timer (F11)
Let p = φ2 cycles completed since the cycle that wrote the timer (that cycle is p = 0; the next is p = 1), I = 1, 8, 64
or 1024, N = the value written:
- 1 ≤ p ≤ N·I: `INTIM = N − 1 − (p − 1) / I`, flag clear (p = 1 reads N − 1 at every interval; p = N·I reads 0).
- p ≥ N·I + 1: flag set at p = N·I + 1, `INTIM = (0xFF − (p − N·I − 1)) & 0xFF`, one count per φ2, wrapping past every
  later underflow.

This matches the datasheet's prose ("(52 x 8) + 1 = 417T") and its step 5 (`$AC` at elapsed 500). It matches D-19's
"clear at φ2 N·I, set at N·I + 1" when φ2 numbers start at 1 for the write cycle. The datasheet figure's label
"#416" is one count off from its own prose; docs record that. Flag clear: a timer read or write clears it, except a
timer read at p = N·I + 1. Reading `$285` clears only PA7's flag and returns bits 7–6 with D5–D0 = 0. PA7 edges
are detected on output pins too. The edge test reads `$285` after the polarity write and before driving the edge,
because a polarity change may itself set the flag (datasheet). RIOT reads drive all 8 bits.

### Pattern 6: probe order and bounded hashing (D-03, D-05)
Do arguments, the size tag, then `n > MUHTARI_IMAGE_SIZE_MAX` before any hashing. Header path: parse v3; take v4 when
`version ≥ 4` and the block validates; resolve conflicts; check geometry; take
`mapped = min(field, n − 128)`; one SHA-1 over `mapped`, plus one per slice of each layout whose sizes sum to `mapped`;
then, for 128 KiB misses only, one SHA-1 fed as `[8K..16K) [0..8K)` per 16 KiB bank with no copy. The worst case is a
fixed handful of passes over ≤ 1 MiB.

### Anti-patterns
- **A const struct of table pointers:** it lands in `.data.rel.ro` and fails `abi.globals` (D-23). Pass tables as arguments.
- **`fseek/ftell` in the runner's read** (`long` is 32-bit on Win64; pipes): use D-07's `fread` loop.
- **Compiling cpu.c twice** (test build vs shipped): rejected by D-14. OBJECT library only.
- **A WILL_FAIL negative control:** use `expect_fail.cmake` with an exact code and regex.

## Don't Hand-Roll

| Problem | Don't build | Use instead | Why |
|---|---|---|---|
| Decimal ADC/SBC | a new derivation | 6502.org Appendix A Seq. 1/2/3 as below; 64doc ARR | Verified bit-exact (F17); NMOS quirks are subtle |
| Vector reading/conversion | a new binary format or parser | nesturbator's N65V v1 reader and vecconv JSON reader | Already bounds-checked and fuzzed in the sibling |
| Vector fetch | 256 raw URLs, a cache | sparse blobless git checkout (F5) | Raw limits; 193 MiB vs 5.17 GB |
| Zip extraction | unzip/tar flags per OS | `cmake -E tar xf` (Git Bash tar cannot read zip) | One tool on all legs |
| Fixture ordering | ad-hoc "run this first" | CTest `FIXTURES_SETUP/REQUIRED` | Failed setup marks dependents not run (F6) |
| SHA-256, CRC-32 | new code | `runner/sha256.c`, `runner/crc32.c` | Already pinned against test vectors |
| MD5 | RFC 1321's appendix C code | own code from RFC 1321 §3 prose | The appendix carries an RSA attribution licence; the algorithm and its `T[i]` constants are facts |
| 7800basic build | copying `7800basic.sh` | the own recipe below (F8) | GPL-2 text; the script's absolute paths break under MSYS |
| Reading the cartdb rows in the scan | parse C | D-24's fixed one-row-per-line `{{0x..` form matched by regex | Simple and stable |

## Common Pitfalls

1. **`abi.symbols` and the cartdb names.** See C1. **Warning sign:** "outside the muhtari_ namespace: cartdb_…".
2. **`policy.notices` and the SPDX line.** See C2. **Warning sign:** "SPDX is not MIT or MIT AND a PROVENANCE.md licence",
   or ".info licence is 'BSD-3-Clause,MIT', the sources and notices say 'BSD-3-Clause,CC0-1.0,MIT'".
3. **`hygiene.format` reflows `src/cartdb.c`.** `.clang-format` has `ColumnLimit: 100`, and clang-format respaces
   braced lists. A 7800 key row (`{{20 × 0x..}, e, p}`) is about 115 columns, and the unspaced `{0x00,0x28,…}` form gets
   respaced. **Avoid:** wrap the data in `/* clang-format off */` … `/* clang-format on */`. `policy.cartdb_generated`
   then byte-compares the generator output.
4. **`hygiene.no_float` false positives in `src/` and `include/`.** It flags `[0-9]\.[0-9]` and `[^A-Za-z0-9_][0-9]+[eE][+-]?[0-9]`
   after removing hex literals and `section N.N`. **Avoid:** write "section 1.4", not "§1.4", in core comments. Never
   put a hex digest or version number (`48.0.6`, `0.41`) in a core comment.
5. **`release.single_version_source.next`.** No tracked file outside `version.txt`/CHANGELOG may contain `0.2.0` or
   `0.1.1`. **Avoid:** docs say "API version 2", never "since 0.2.0".
6. **nofp misses cpu.c.** `-mgeneral-regs-only` is applied only to `muhtari` today (CMakeLists.txt:170). The OBJECT libs
   need it through `muhtari_core_options` (D-14).
7. **`install(EXPORT)` with OBJECT libs.** Use `$<TARGET_OBJECTS:…>` sources, not `target_link_libraries(muhtari PRIVATE
   muhtari_objs)`, which would require exporting the object libraries (F15).
8. **Inventories.** Every new test name goes into ci.txt **and** wasm32.txt; `abi.api_v2` also goes into **nofp.txt**.
   Files are sorted (`muhtari__check_inventory` fails "is not sorted"), and 256 vector lines invite typos, so generate
   them with a loop.
9. **Clark harness and the flat-bus log.** D-10's 32-entry log fails on overflow. The decimal harness runs about 60 M
   cycles ("approximately 1 minute at 1 MHz", per the listing), so it must reset the log every `cpu_step` or use a
   log-free bus mode.
10. **`size_cases` buffer too small.** `tagged.bytes` is 80 B on 64-bit, but `muhtari_cart_info` is 140. Enlarge it
    before adding the probe's size cases, or the sentinel writes overflow.
11. **Firmware scan on empty zips.** See C7.
12. **fetch-pin's zip case and `set -f`.** Globbing is off throughout the script, and the zips carry a top directory
    (F18).
13. **The per-file vector list is sorted by path, not as lines.** `cpu.vectors.pin` must check that names run `00…ff`
    in order, not `list(SORT)` the whole lines (F5).
14. **wasmtime `--dir` on Windows** splits on `::` only (F10). Pass CMake's forward-slash paths. Never run 7800basic.sh
    there.
15. **A reused 7800basic work directory.** 7800basic writes `7800.asm`, `includes.7800`, `a78info.cfg` and
    `7800basic_variable_redefs.h` into the cwd, and postprocess picks up any `7800.asm` it finds ("User-defined 7800.asm
    found in current directory" appeared even in a fresh run). Always `file(REMOVE_RECURSE)` then create the directory
    (D-22).
16. **Hidden-symbol linking on MSVC/wasm32** is assumed safe but first proven by CI. The fallback, if a leg fails to
    resolve `muhtari__*`: test programs link the OBJECT libraries directly instead of the archive (same objects).
17. **ppc32 64-bit helpers.** Only `__udivdi3 __umoddi3 __ashldi3 __lshrdi3` are allowed. Keep 64-bit arithmetic unsigned
    (SHA-1's bit length `total << 3`, `ticks`); a signed 64-bit divide fails `abi.undefined` on be32's toolchain.
18. **Title bytes.** The sample's 32 title bytes are `My Game` plus spaces, with no NUL. Copy at most 32 printable bytes,
    NUL-terminate, and set `TITLE_NONPRINTABLE` only for bytes outside $20–$7E before the first NUL. Recommendation
    (discretion): trim trailing spaces, so `--probe` prints `title My Game`.
19. **MSVC `/W4 /WX` and `fopen`.** Phase 1's runner already compiles `fopen` under ci-msvc, so no
    `_CRT_SECURE_NO_WARNINGS` is needed for runner code. The seeded replay main uses the same calls.
20. **The hygiene job's 1-minute timeout** now includes two XML downloads (0.87 MB) and the generator (0.03 s).
    Re-measure before ship.

## Code Examples

### Decimal ADC/SBC (bit-exact against the vectors, F17)
```c
/* NMOS decimal ADC (6502.org decimal-mode tutorial, Appendix A: Seq. 1 gives
   A and C, Seq. 2 gives N and V, Z is the binary sum's). */
static void adc(muhtari *m, uint8_t v)
{
    uint32_t a = m->cpu.a, c = m->cpu.p & FLAG_C, bin = a + v + c;
    if ((m->cpu.p & FLAG_D) == 0u) { /* binary path as in the seed */ }
    uint32_t al = (a & 0x0Fu) + (v & 0x0Fu) + c;
    if (al >= 0x0Au) {
        al = ((al + 0x06u) & 0x0Fu) + 0x10u;
    }
    uint32_t s = (a & 0xF0u) + (v & 0xF0u) + al;
    set_flag(m, FLAG_N, (s & 0x80u) != 0u);
    set_flag(m, FLAG_V, ((~(a ^ v) & (a ^ s)) & 0x80u) != 0u);
    set_flag(m, FLAG_Z, (bin & 0xFFu) == 0u);
    if (s >= 0xA0u) {
        s += 0x60u;
    }
    set_flag(m, FLAG_C, s >= 0x100u);
    m->cpu.a = (uint8_t)s;
}

/* NMOS decimal SBC (Appendix A, Seq. 3): every flag is the binary result's,
   so compute flags as the seed's binary SBC does, then replace A. Unsigned
   wraparound stands in for the tutorial's negative values. */
static uint8_t sbc_decimal(uint32_t a, uint32_t v, uint32_t c)
{
    uint32_t al = (a & 0x0Fu) + c - (v & 0x0Fu) - 1u;
    if ((al & 0x80000000u) != 0u) {
        al = ((al - 0x06u) & 0x0Fu) - 0x10u;
    }
    uint32_t s = (a & 0xF0u) - (v & 0xF0u) + al;
    if ((s & 0x80000000u) != 0u) {
        s -= 0x60u;
    }
    return (uint8_t)s;
}
```
ARR in decimal (64doc, nesdev.org/6502_cpu.txt; 0 mismatches on `6b.json`): `t = A & imm`, `AH = t >> 4`, `AL = t & 15`,
`N = old C`, `A = (t >> 1) | (C << 7)`, `Z = (A == 0)`, `V = ((t ^ A) & 64) != 0`; if `AL + (AL & 1) > 5`, then
`A = (A & 0xF0) | ((A + 6) & 0xF)`; `C = AH + (AH & 1) > 5`; if C, `A = (A + 0x60) & 0xFF`.

### Clark transcription header (assembles, F4)
```asm
        processor 6502
AR      = $00        ; ... CF $01, DA $02, DNVZC $03, ERROR $04, HA $05, HNVZC $06,
                     ; N1 $07, N1H $08, N1L $09, N2 $0A, N2L $0B, NF $0C, VF $0D, ZF $0E
N2H     = $0F        ; 2 bytes ($0F-$10)
        ORG $2000    ; TEST = $2000
; then Appendix B verbatim, with the header line "Written by Bruce Clark. This code is public domain." and the URL
```
Assemble with `wasmtime run --dir=. <pkg>/dasm.wasm clark.s -f3 -oclark-6502.bin` (exit 0). The 65C02 build substitutes
`JSR A65C02`/`JSR S65C02` per Appendix D. The harness loads the bytes at $2000, pushes a sentinel return address,
runs from $2000 until `RTS` pops the sentinel (≤ 150,000,000 cycles), then reads `ERROR` at $04.

### Own 7800basic recipe (every stage exit 0; digests equal the pins, F8)
Run each stage as `wasmtime run --dir=. --dir=<pkg>::/b <pkg>/<tool>.wasm …` in a fresh directory holding
`simple.bas` and `face1.png`:
1. `7800preprocess.wasm simple.bas` → stdout to `pre.bas`
2. `7800basic.wasm -i /b` with stdin `pre.bas` (it writes `7800.asm`, `includes.7800`, `a78info.cfg` and
   `7800basic_variable_redefs.h`; stdout is empty)
3. `7800postprocess.wasm -i /b` | `7800optimize.wasm` → `simple.bas.asm`
4. `dasm.wasm simple.bas.asm -I/b/includes -f3 -p20 -osimple.bas.bin` | `7800filter.wasm`
5. `7800sign.wasm -w simple.bas.bin`
6. `7800header.wasm -o -f a78info.cfg simple.bas.bin` → `simple.bas.a78`

In CMake: one `execute_process` per stage, using `INPUT_FILE`/`OUTPUT_FILE`, multiple `COMMAND`s for the pipes, and
`RESULTS_VARIABLE` (one status per command). These stages and flags were taken only from each tool's own usage output
and D-22.

### SPDX header of the generated file (passes notices.cmake:33)
```c
/* SPDX-License-Identifier: MIT AND CC0-1.0 */
/* Generated by tools/cartdb.cmake from MAME hash/a7800.xml and hash/a2600.xml at
   dddf4cd1faeeca6a6dac26ecbc2da2da4242b740; do not edit. */
/* clang-format off */
```

## Build order, dependency graph and waves

```
 [1 CPU+vectors] ──► [2 bus/TIA/RIOT] ──────────────────────────────┐
        │                                                           │
        ├──► [6a vectors-full nightly]                              ▼
 [3 header/SHA-1/probe/cartdb] ──► [4 loader/fuzz/manifest] ──► [5 runner --probe + built sample] ──► [6b firmware scan] ──► [7 docs, reference fixes, measurements, ship]
                                                                    ▲
 (built sample: independent of everything; fold into 5 or run first)┘
```

| Plan | Contents (D-…) | Leads with (tracer) | Depends on |
|---|---|---|---|
| 1 CPU and vectors | `muhtari_core_options`, OBJECT split, internal.h CPU/profile/bus stubs, cpu.c seed + decimal + C5 NMI, flat bus, n65v/sst_json/vecconv, sample + fixtures + manifest + `singlesteptests` notice + `.gitattributes/.gitignore`, `cpu.vectors.*`, `cpu.unstable`, Clark (D-08–10, 12–14, 20) | `cpu.vectors.a9` green from the committed sample | — |
| 2 Bus, TIA, RIOT | bus.c, tia.c, riot.c, HALT/NMI entry points, `ctest -L sally`, `ctest -L riot`, Cachegrind one-off (D-15–19) | `bus.cycle.fast_4_ticks` | 1 (bus seam, NMI rule) |
| 3 Header, probe, cartdb | header v2, C3 edits, `tests/api/v2.c`, sha1.c, cart.c probe/identify, cartdb.h, `tools/cartdb.cmake`, generated cartdb.c, MAME notice + `.info`, `policy.cartdb_generated`, `images.c` (probe classes), `cart.probe.*`, `cart.identify.*`, size tags (D-01–06, 23–25) | `cart.probe.2600` | — (parallel with 1–2 in principle; touches CMakeLists after 1) |
| 4 Loader and fuzz | `muhtari__cart_load`, `tests/alloc.h`, `cart.load.no_alloc_on_reject`, `tests/fuzz/*`, `fuzz.regress.loader(.self_test)`, `tests/roms/manifest.txt` + `policy.rom_hash_coverage`, fuzz preset, nightly `loader-fuzz` (D-26, 27) | `fuzz.regress.loader` | 3 |
| 5 Runner `--probe` + built sample | pins (wasmtime ×3, 7800basic), fetch-pin zip, ci.yml fetch steps, `tests/built/*`, md5.c, status_text, exit 77, `runner.probe.*`, `muhtari-run-testdb`, `muhtari-test-images` (D-06, 07, 22, 23) | `built.samples` | 3, 4 (images.c) |
| 6 Nightly full set + firmware scan | `fetch_vectors.cmake`, vectors-full presets/inventory/job, `cpu.vectors-full.*`, `firmware_scan.cmake`, `firmware.txt`, publish.sh + hygiene fixture (C7), `release.firmware_scan`, `hygiene.firmware_scan` (D-11, 28) | `cpu.vectors-full.fetch` locally | 1, 3, 5 |
| 7 Docs and closure | `docs/hardware/*.md`, docs budget glob, embedding/runner/testing updates, preparation shrink + reference fixes (D-21, 30), timeout measurements (D-31), be32 budget check (D-29) | `docs.budgets` | all |

**Parallelism.** `.planning/config.json` has `"parallelization": false`, so the waves run in sequence. If it were
enabled, plans 1 and 3 could run side by side after a shared CMake restructure. Merge hotspots: `CMakeLists.txt`,
`src/internal.h`, `tests/tests.cmake`, `tests/inventory/{ci,wasm32,nofp,hygiene}.txt`, `ci.yml`, `nightly.yml`,
`pins.txt`, `THIRD-PARTY-NOTICES.md`, `tests/roms/manifest.txt`, `docs/testing.md`, `.gitattributes`/`.gitignore`.
Plan 1 should land the full CMake skeleton (all three OBJECT libraries and the function) so later plans only append
sources.

**Prove earliest:**
- **R1, the Windows built-sample bytes and the `D:` preopen.** Push the branch as a draft PR right after plan 5's
  `built.samples` exists. Fallback: D-22's own preset on the legs that reproduce, with the run URL recorded.
- **R2, be32 within 5 minutes** with about 300 more processes under qemu (×11 timeouts). Estimate: 256 vector processes
  at about 1 s each under qemu, over 4 vCPUs, is about 65 s, plus Clark at about 4 s. Fallback per DEC.46: move be32 to
  nightly; never drop tests.
- **R3, the OBJECT split on ci-msvc, nofp and wasm32,** proven by plan 1's first CI run.
- **R4, the CPU itself.** The decimal paths are already proven in Python (F17); the NMI rules are proven only by the
  phase's own tests.

## Environment Availability

| Dependency | Required by | Available here | Version | Fallback |
|---|---|---|---|---|
| CMake | everything | ✓ | 4.4.3 | — |
| git | vector fetch | ✓ | 2.41.0 (CI images newer) | — |
| Docker `ubuntu:26.04`, `gcc:14` | Linux lanes locally, Cachegrind | ✓ | aarch64 | x86_64 via `--platform` (slow) |
| Rosetta | x86_64-macos wasmtime | ✓ | — | — |
| wasmtime 48.0.6 | built sample, wasm32 | ✓ (scratch) | aarch64/x86_64 macOS, aarch64 linux | — |
| python3 | scratch analysis only (`-I`) | ✓ | — | never used by the repo |
| pdftotext | datasheet reading only | ✓ | — | — |
| Windows hosts | Windows byte identity | ✗ | — | CI legs `windows-2025`, `windows-11-arm` |
| valgrind | D-14 one-off | ✗ on macOS | — | apt in an `ubuntu:26.04` container |

**Missing with no fallback:** none. **Missing with a fallback:** Windows (CI), valgrind (container).

## Assumptions Log

| # | Claim | Section | Risk if wrong |
|---|---|---|---|
| A1 | wasm-ld and MSVC link hidden/plain `muhtari__*` archive symbols into test programs | F14, Pitfall 16 | Test programs fail to link on two lanes; fallback is linking the OBJECT libs directly |
| A2 | The TIA WSYNC release is at the TIA counter wrap, and the held read completes in the first fast cycle that starts at or after it | Pattern 4 | `bus.rdy.*` expected ticks shift by one cycle; docs already mark it fitted |
| A3 | The idle TIA inputs read D7 = 0 for `INPT0`–`INPT3` and collisions, and D7 = 1 for `INPT4`/`INPT5` (no button pressed) | D-15 drivers | Only `bus.open_bus.write_only` expectations change; phase 3 wires real inputs |
| A4 | No real header counts its own 128 bytes in the size field over a body that fails geometry | D-05 | Such an image is refused as truncated. D-25's "field 128 above a valid linear body" covers the case that passes |
| A5 | XM-flagged homebrew boots without the XM | D-04 | Phase 3 concern; probe reports `devices` only |
| A6 | Linear geometry: body a multiple of 4 KiB from 4 KiB to 48 KiB, or exactly 64 KiB (top 48 KiB mapped). SG: multiples of 16 KiB from 32 KiB to 1 MiB; SG9: (2ⁿ + 1) × 16 KiB. The CC0 list's linear sizes are 16/32/48/64 KiB (counts 27/57/24/11); SG 64/128 KiB; SG9 144 KiB | Discretion (D-05) | Too strict refuses odd homebrew; too loose only lets more truncated images load, as min(field, n − 128) |
| A7 | The seed's non-decimal opcodes pass 6502/v1 unchanged | Seeds | Found by plan 1's first `ctest -L vectors` run |
| A8 | The 7800 BIOS/HSC/Supercharger rows in `firmware.txt` are SHA-1 only (ASSET_POLICY §4 publishes no SHA-256) | D-28 | None; the scan compares SHA-1 for firmware and SHA-256 for built samples |

## Open Questions

1. **OpenBIOS digests in `tests/release/firmware.txt` (D-28).** Their digests exist only after an OpenBIOS build, which
   ASSET_POLICY §3 schedules with `boot.c` (phase 3). Downloading the 7800OpenBIOS repository would also fetch the
   all-rights-reserved `kiloparsec.bl1`.
   - Recommendation: phase 2 ships the §4 BIOS/HSC rows and the a2600.xml Supercharger SHA-1, plus a comment line that
     the two OpenBIOS rows arrive with the phase 3 build. If the planner wants them now, the only clean route is a
     sparse checkout that excludes `kiloparsec.bl1`, with 8,192 fill bytes substituted, recorded as a command and exit
     status.
2. **Nightly path filtering.** Recommendation as in the code map: one path union in `nightly.yml`. Revisit if the
   bench's PR runs become a nuisance.
3. **`fuzz` preset vs ad-hoc commands in the job.** Recommendation: the preset with `fuzz.txt`, matching the
   hygiene/nofp pattern.

## Risks beyond this phase

- **R6, the check-code budget from phase 3.** AGENTS rule 7: check code below a tenth of `src/` lines. Today `src/` is
  319 lines and `tests/cmake/` + `scripts/` are 1,881. After phase 2, `src/` is roughly 4,500 lines, of which about
  2,050 are generated `cartdb.c`. Phase 3 should settle whether generated data counts, before it starts.

## Sources

### Primary (HIGH)
- SingleStepTests/65x02 @2f6980a2d95757486c7bee24355c360e40e2a224: `6502/README.md`, `LICENSE` (MIT, "Thomas Harte et
  al"), `6502/v1/*.json` (fetched, measured).
- 6502.org "Decimal Mode" tutorial (https://6502.org/tutorials/decimal_mode.html), Appendices A, B, D (fetched 2026-10-16).
- MOS 6532 datasheet, http://archive.6502.org/datasheets/mos_6532_riot.pdf (fetched, read in full text).
- NESdev wiki "CPU interrupts" (fetched 2026-10-16); 64doc, nesdev.org/6502_cpu.txt (ARR decimal).
- wasmtime v48.0.6 `src/common.rs` (Apache-2.0, read); release assets (digests verified).
- 7800basic v0.41 wasm package (tool usage output and builds only).
- MAME `hash/a7800.xml`, `hash/a2600.xml` @dddf4cd1 (CC0, parsed).
- szTheory/nesturbator @9f9386c661f6: `src/cpu.c`, `CMakeLists.txt`, `tests/vectors/n65v.h`, `tests/cpu/test_vectors.c`,
  `tests/cpu/vector_bus.c`, `tests/fuzz/replay_main.h`, `tests/CMakeLists.txt`, `tests/cmake/fetch_vectors.cmake`.
- szTheory/gabbaboy @a175a39 `include/gabbaboy/gabbaboy.h`.
- This repository at `c047ad6`: every file named in the code map, read this session.

### Secondary (MEDIUM)
- Stella Programmer's Guide (Sinnett reconstruction, atariage.com/2600/programming/2600_101/docs/stella.html), WSYNC/RDY;
  a 1989 alt.sources TIA reference (tuhs.org) for the "leading edge of horizontal blank" wording.
- `.planning/preparation/A7800-SYSTEM.md` §1–8, `ARCHITECTURE.md` §2–3, `A7800-CARTRIDGES-AND-PERIPHERALS.md` §1–2 (facts
  with their own citations).

## Metadata

**Confidence:**
- Standard stack: HIGH. Every pin was fetched and its digest matched.
- Architecture: HIGH. The patterns follow the existing gates and the tested toys.
- Pitfalls: HIGH. Each one is tied to a quoted line or a command.
- Hardware timing (WSYNC edge, HALT latency): MEDIUM, fitted by design.

**Research date:** 2026-10-16. **Valid until:** 2026-11-15 (pins are fixed; only CI-runner images drift).
