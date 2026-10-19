---
phase: 02-the-cpu-matches-the-public-vectors
reviewed: 2026-10-19T00:00:00Z
depth: standard
files_reviewed: 110
files_reviewed_list:
  - .gitattributes
  - .github/workflows/ci.yml
  - .github/workflows/nightly.yml
  - .gitignore
  - ASSET_POLICY.md
  - CMakeLists.txt
  - CMakePresets.json
  - THIRD-PARTY-NOTICES.md
  - docs/embedding.md
  - docs/hardware/bus.md
  - docs/hardware/cartridge.md
  - docs/hardware/riot.md
  - docs/hardware/sally.md
  - docs/maintaining.md
  - docs/runner.md
  - docs/testing.md
  - host/status_text.c
  - host/status_text.h
  - include/muhtari.h
  - libretro/muhtari_libretro.info.in
  - pins.txt
  - runner/exit_status.h
  - runner/main.c
  - runner/md5.c
  - runner/md5.h
  - scripts/fetch-pin.sh
  - scripts/publish.sh
  - src/bus.c
  - src/cart.c
  - src/cartdb.c
  - src/cartdb.h
  - src/cpu.c
  - src/instance.c
  - src/internal.h
  - src/riot.c
  - src/sha1.c
  - src/sha1.h
  - src/tia.c
  - tests/abi/tests.cmake
  - tests/alloc.h
  - tests/api/v1.c
  - tests/api/v2.c
  - tests/built/build.cmake
  - tests/built/expected.txt
  - tests/built/tests.cmake
  - tests/bus/test_bus.c
  - tests/bus/tests.cmake
  - tests/cart/images.c
  - tests/cart/images.h
  - tests/cart/test_cart.c
  - tests/cart/test_cartdb.c
  - tests/cart/test_sha1.c
  - tests/cart/tests.cmake
  - tests/cart/write_images.c
  - tests/cmake/abi.cmake
  - tests/cmake/budgets.cmake
  - tests/cmake/fetch_vectors.cmake
  - tests/cmake/firmware_scan.cmake
  - tests/cmake/no_float.cmake
  - tests/cmake/no_test_entries.cmake
  - tests/cmake/rom_hash_coverage.cmake
  - tests/cmake/vectors_pin.cmake
  - tests/cmake/vectors_sample_match.cmake
  - tests/core/test_core.c
  - tests/cpu/decimal/clark.s
  - tests/cpu/full.cmake
  - tests/cpu/targets.cmake
  - tests/cpu/test_cpu.c
  - tests/cpu/test_reader.c
  - tests/cpu/test_vectors.c
  - tests/cpu/tests.cmake
  - tests/cpu/vector_bus.c
  - tests/cpu/vector_bus.h
  - tests/embed/CMakeLists.txt
  - tests/fuzz/loader.c
  - tests/fuzz/replay_main.h
  - tests/fuzz/seeds.c
  - tests/fuzz/seeds.h
  - tests/fuzz/tests.cmake
  - tests/header/header.c
  - tests/header/header.cpp
  - tests/hygiene/tests.cmake
  - tests/inventory/ci.txt
  - tests/inventory/fuzz.txt
  - tests/inventory/hygiene.txt
  - tests/inventory/nofp.txt
  - tests/inventory/vectors-full.txt
  - tests/inventory/wasm32.txt
  - tests/release/firmware.txt
  - tests/release/tests.cmake
  - tests/riot/test_riot.c
  - tests/riot/tests.cmake
  - tests/roms/manifest.txt
  - tests/runner/test_runner.c
  - tests/runner/tests.cmake
  - tests/tests.cmake
  - tests/vectors/6502-v1.sha256
  - tests/vectors/fixtures/02.json
  - tests/vectors/fixtures/README.md
  - tests/vectors/fixtures/a9-bad-a.json
  - tests/vectors/fixtures/a9.json
  - tests/vectors/n65v.c
  - tests/vectors/n65v.h
  - tests/vectors/sst_json.c
  - tests/vectors/sst_json.h
  - tests/vectors/vecconv.c
  - tools/cartdb.cmake
findings:
  critical: 0
  warning: 4
  info: 3
  total: 7
status: issues_found
---

# Phase 2: Code Review Report

**Reviewed:** 2026-10-19T00:00:00Z
**Depth:** standard
**Files Reviewed:** 110
**Status:** issues_found

## Narrative Findings (AI reviewer)

## Summary

I read every core source in full: `src/cpu.c`, `src/bus.c`, `src/riot.c`, `src/tia.c`, `src/cart.c`, `src/instance.c`, `src/frame.c`, `src/sha1.c` and `src/internal.h`. I also read the public header, the runner's `main.c` and `md5.c`, `host/status_text.c`, the cartridge-table generator, the vector reader, converter and JSON parser, the CPU test bus and harness, the fuzz loader, the release and vector gate scripts, and the CI and nightly workflow diffs. I checked the cartdb tables by structure (counts, row shape, the 256-entry bound), not row by row. The three binary fixtures were not opened, as instructed.

I traced the CPU against the 64doc cycle order and against the NMI deciding-cycle table in docs/hardware/sally.md:
- All 256 opcodes have a case.
- No expression holds two bus calls whose order C leaves unspecified.
- The poll marks of BRK, RTI, RTS, JSR, branches and push/pull match the doc.

I checked the bus HALT, RDY and NMI arithmetic (`hold`, `cycles_until`, `events`) against docs/hardware/bus.md. The RIOT timer and flag formulas match docs/hardware/riot.md, including the datasheet's 417T example. The A78 parsing, v3/v4 decode, hash lookup and probe order match docs/hardware/cartridge.md, and every refusal path leaves the instance unchanged. SHA-1 and MD5 follow FIPS 180-4 and RFC 1321. The vector reader and JSON parser never read past `len`. I found no blocker.

The findings, in order:
- A path-filter gap means a pull request can change what the new nightly jobs test without running them.
- The size-tag helper comment claims a protection the code does not give.
- Two latent overwrite hazards sit in the HALT and NMI entry points that MARIA will drive.
- `muhtari__cpu_step` does not enforce the "jammed CPU runs nothing" invariant that `internal.h` states.

Under rule 7, findings 1 and IN-03, which are about tooling, are seeds, not requirements.

## Warnings

### WR-01: Nightly pull-request path filter misses inputs of the vectors-full and loader-fuzz jobs

**File:** `.github/workflows/nightly.yml:18-36`
**Issue:** The comment says "A change to any job's inputs or this file runs every job on its pull request". The `paths` list leaves out files that the two new jobs build from or read:
- `src/internal.h`: the `struct muhtari` layout that `tests/cpu/vector_bus.h` embeds, and the CPU and bus declarations.
- `src/instance.c` and `src/riot.c`: `muhtari_create`, which runs `muhtari__power_on` and then `muhtari__riot_reset`, and which every fuzz iteration calls.
- `src/cartdb.h`, `src/sha1.h` and `tools/cartdb.cmake`.
- `tests/alloc.h`: the fuzz harness's counting allocator, which `tests/fuzz/**` does not match.
- `tests/inventory/vectors-full.txt` and `tests/inventory/fuzz.txt`: the "Every test ran" inventories.
- `tests/cmake/check_junit.cmake`, `CMakePresets.json` (the `fuzz` and `vectors-full` presets), `CMakeLists.txt` (the `MUHTARI_FUZZ` and `MUHTARI_VECTORS_FULL` options) and `pins.txt` (the `sst-6502-v1` row that `tests/cpu/full.cmake` parses).

Edit one of these and the pull request merges without the 2,560,000-test run or the 60 s fuzz run. The break then shows up only as a red night after the merge. That is the fault the comment says the union prevents. The bench job has the same gap for `runner/main.c`, `src/frame.c` and `src/testcard.c`; it predates this phase.
**Fix:** Add the missing inputs to the list, for example:
```yaml
      - CMakeLists.txt
      - CMakePresets.json
      - pins.txt
      - src/internal.h
      - src/instance.c
      - src/riot.c
      - src/cartdb.h
      - src/sha1.h
      - tools/cartdb.cmake
      - tests/alloc.h
      - tests/inventory/vectors-full.txt
      - tests/inventory/fuzz.txt
      - tests/cmake/check_junit.cmake
```
Or narrow the comment so it no longer claims every input.

### WR-02: The size-tag cap does not stop reads past the caller's object, as its comment says it does

**File:** `src/instance.c:13-15`, `src/instance.c:27-31`
**Issue:** `MUHTARI__SIZE_TAG_MAX` is documented as "so a garbage tag is refused without reading past the caller's object". It does not do that for a tag in `(sizeof(struct), 4096]`. In that case `muhtari__check_size` reads `bytes[ours .. size-1]`, up to about 4 KiB past the end of a struct the caller sized for `ours`. Take an uninitialised `muhtari_frame` on the stack whose first word happens to be 1000: the check reads 976 bytes beyond it. That is undefined behaviour, and it can fault at a page edge. The cap only bounds the over-read; it does not prevent it. `muhtari_get_versions`, `muhtari_create`, `muhtari_get_info`, `muhtari_run_frame` and `muhtari_probe` all reach this path.
**Fix:** Keep the behaviour, but make the comment and docs/embedding.md accurate. A tag is a promise that `size` bytes are readable, and the cap only bounds what a bad promise costs:
```c
/* The largest size tag accepted. A tag promises that many readable bytes;
   the cap bounds how far a wrong tag can make the check read, it does not
   make a wrong tag safe. */
#define MUHTARI__SIZE_TAG_MAX 4096u
```

### WR-03: A second HALT request or NMI edge silently replaces one still in flight

**File:** `src/bus.c:245-265`
**Issue:**
- `muhtari__bus_request_halt` sets `halt_stage = HALT_REQUESTED` and overwrites `halt_at`/`halt_until` whatever state the bus is in. If MARIA asks for the next window while the current one is in `HALT_LOW` (c0 or c1 done, the hold not yet applied), the pending `halt_lost` hold is dropped. The CPU then runs through a DMA window it should have lost, which silently corrupts cycle counts.
- `muhtari__bus_raise_nmi` likewise overwrites `nmi_at` while an earlier raised edge is still `nmi_pending` and not yet latched. The earlier edge is lost.

Neither entry point is called yet (MARIA is not wired), so this is latent. But `internal.h:280-285` describes both as the entry points MARIA drives, and neither the doc nor the comment states a one-at-a-time precondition. Nothing in `tests/bus/test_bus.c` pins the behaviour either way.
**Fix:** Either state and enforce the precondition, or queue the request. At minimum:
```c
void muhtari__bus_request_halt(muhtari *m, uint64_t at_tick, uint64_t until_tick)
{
    struct muhtari__bus *b = &m->bus;
    if (b->halt_gate_writes < 2u || b->halt_stage != HALT_NONE) {
        return; /* one window at a time: MARIA requests the next after this one ends */
    }
    ...
}
```
Document the rule in docs/hardware/bus.md, and add a bus test for a request made during `HALT_LOW`.

### WR-04: `muhtari__cpu_step` does not enforce the jammed invariant, and a jammed CPU advances no time

**File:** `src/internal.h:29-30`, `src/internal.h:287-288`, `src/cpu.c:1119-1121`
**Issue:** `internal.h` says "jammed is set by a JAM opcode; the CPU then runs no more instructions". The declaration of `muhtari__cpu_step` says only "Runs exactly one instruction". The function never reads `m->cpu.jammed`: called on a jammed CPU, it fetches at PC + 1 and executes whatever is there. That turns a JAM into a resume, the opposite of the hardware.

docs/hardware/sally.md hands the guard to "the run loop", which does not exist yet. So the invariant lives in prose, two layers away from the code that breaks it. If the run loop does add the guard by skipping the call, a jammed CPU makes no bus cycle, so ticks stop advancing. A frame loop of the form `while (ticks < frame_end) cpu_step(m)` then never terminates, which breaks rule 4 (every call returns within bounded emulated time, even for a jammed ROM).
**Fix:** Put the guard where the state lives, and keep time moving the way a jammed NMOS 6502 does, by continuing to read the bus:
```c
void muhtari__cpu_step(muhtari *m)
{
    if (m->cpu.jammed != 0u) {
        /* A jammed CPU keeps reading $FFFF and takes no interrupt
           (docs/hardware/sally.md, JAM). */
        muhtari__bus_read(m, 0xFFFFu);
        return;
    }
    uint8_t opcode = fetch(m);
    ...
```
Update the declaration's comment to match. The vector harness steps each test once from a cleared CPU, so its JAM checks are unaffected.

## Info

### IN-01: Stale value comment on `muhtari_versions.api`

**File:** `include/muhtari.h:45`
**Issue:** `uint32_t api; /* 1 */` while `MUHTARI_API_VERSION` is now 2, which is what `muhtari_get_versions` reports (`src/instance.c:58`). A reader of the terse public header gets the wrong value.
**Fix:** Change it to `/* 2 */`, or drop the literal so the comment cannot drift: `/* MUHTARI_API_VERSION */`.

### IN-02: The cartdb generator accepts a `reload` slice whose size differs from the chip it repeats

**File:** `tools/cartdb.cmake:198-203`
**Issue:** A `loadflag="reload"` chip appends a slice of the reload's own `size` with the previous chip's part. `cart_find_slices` then demands that this slice's SHA-1 equal that part's key, which only happens when the reload has exactly the chip's size. A smaller or larger reload produces a layout that can never match, so the entry is never identified by its slices. The script's own rule is to "stop instead of a guess", but here it emits an unmatchable row without a word. I did not check whether any entry in the pinned list has such a reload.
**Fix:** Record each keyed chip's size, and `cartdb_fail` when a reload's size differs from it.

### IN-03: The firmware scan cannot recognise multi-chip or half-swapped 7800 images

**File:** `tests/cmake/firmware_scan.cmake:136-150`
**Issue:** The scan compares each member's whole-file SHA-1 (and its header-stripped SHA-1) against the cartdb keys. For a multi-chip entry the keys are per chip, and an Activision image may be stored with its 8 KiB halves swapped. A whole image of either kind matches no key, so it passes the "no known ROM ships" gate. This is low risk, since release zips are built from tracked sources that hygiene already scans. It is a gap in the fault the header names.
**Fix:** Seed only. Either add the slice and swap passes that `cart_lookup` uses, or narrow the script's header to "single-chip images".

---

_Reviewed: 2026-10-19T00:00:00Z_
_Reviewer: Claude (gsd-code-reviewer)_
_Depth: standard_
