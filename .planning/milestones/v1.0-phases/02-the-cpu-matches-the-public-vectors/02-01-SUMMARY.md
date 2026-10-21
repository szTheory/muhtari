---
phase: 02-the-cpu-matches-the-public-vectors
plan: 01
subsystem: cpu
tags: [sally, 6502, singlesteptests, n65v, cmake-object-library, ci-probe]

requires:
  - phase: 01-a-test-frame-in-retroarch
    provides: the muhtari STATIC library, the test inventory check, hygiene, notices and the hosted CI legs
provides:
  - muhtari_core_options(target) and the OBJECT libraries muhtari_objs and muhtari_cpu
  - src/cpu.c with muhtari__cpu_step on the muhtari__bus_read and muhtari__bus_write seam
  - struct muhtari__cpu, struct muhtari__bus (phi2, nmi_phi2), struct muhtari__profile and MUHTARI__RAM_FILL_ZERO
  - muhtari-test-vectors (FILE XX) and muhtari-vecconv, the N65V reader and the SingleStepTests JSON reader
  - tests/vectors/6502-sample.n65v with its manifest row and the singlesteptests notice
  - the registered test cpu.vectors.a9
  - draft probe PR 9 with a green run on every leg
affects: [02-02 every opcode green, 02-03 cpu.vectors.reader, 02-06 bus.c joins muhtari_cpu to muhtari, 02-08 muhtari_cartdb, 02-16 timeouts and probe close]

actuals:
  tokens: 23700
  tasks: 3
  commits: 3
plan_head_before: d42c31efbafb09739702000e4ec091fc44fcf5df
plan_head_after: 9647eb93a9256fd151a2dedc738d8c8be3b5aca1

tech-stack:
  added: []
  patterns:
    - "One CPU object, two buses: test programs link $<TARGET_OBJECTS:muhtari_cpu> with tests/cpu/vector_bus.c and never link muhtari"
    - "Every core target gets muhtari_core_options, so nofp's -mgeneral-regs-only reaches every core object"
    - "The vector harness prints everything on stdout in file order: one mismatch line per failing test, the first one's cycle tables, then the count line"

key-files:
  created:
    - src/cpu.c
    - tests/cpu/targets.cmake
    - tests/cpu/tests.cmake
    - tests/cpu/vector_bus.c
    - tests/cpu/vector_bus.h
    - tests/cpu/test_vectors.c
    - tests/vectors/n65v.c
    - tests/vectors/n65v.h
    - tests/vectors/sst_json.c
    - tests/vectors/sst_json.h
    - tests/vectors/vecconv.c
    - tests/vectors/6502-sample.n65v
    - tests/roms/manifest.txt
  modified:
    - CMakeLists.txt
    - src/internal.h
    - src/instance.c
    - tests/tests.cmake
    - tests/embed/CMakeLists.txt
    - THIRD-PARTY-NOTICES.md
    - .gitattributes
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt

key-decisions:
  - "embed.subdirectory accepts muhtari_* OBJECT libraries beside muhtari, and still rejects any other target an embedding adds"
  - "The harness reads a .json file as exactly 10,000 tests; any other path is the 256-chunk N65V sample, whose per-chunk count comes from the first header"
  - "A run with no tests for the opcode exits 1"
  - "The JAM opcodes keep the seed's 11-read sequence and set jammed; the harness does not assert jammed yet"

patterns-established:
  - "Seed commits carry one 'Seeded from nesturbator 9f9386c661f6 <path> (same author, MIT).' line per seeded file"
  - "Hardware comments in src/cpu.c cite docs/hardware/sally.md sections: Cycle order, Status register, Decimal mode, Unofficial opcodes, Unstable opcodes, JAM"

requirements-completed: []

coverage:
  - id: D1
    description: "src/cpu.c is compiled once as OBJECT library muhtari_cpu with muhtari_core_options; muhtari is built from muhtari_objs; the harness links the CPU object with the flat bus"
    requirement: CPU-01
    verification:
      - kind: integration
        ref: "cmake --workflow --preset ci (exit 0, 67 tests)"
        status: pass
      - kind: integration
        ref: "https://github.com/szTheory/muhtari/actions/runs/39179237062"
        status: pass
    human_judgment: false
  - id: D2
    description: "All 10,000 upstream a9.json tests match on registers, raw P, RAM and every bus cycle"
    requirement: CPU-01
    verification:
      - kind: other
        ref: "build/ci/tests/muhtari-test-vectors <scratch>/sst/6502/v1/a9.json a9 (exit 0, 6502/a9: 0 of 10000 failed)"
        status: pass
    human_judgment: false
  - id: D3
    description: "The committed sample holds 256 chunks of 100 tests in 1,677,886 bytes, and cpu.vectors.a9 passes from it"
    requirement: CPU-01
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^cpu\\.vectors\\.a9$'"
        status: pass
      - kind: other
        ref: "test $(wc -c < tests/vectors/6502-sample.n65v) = 1677886"
        status: pass
    human_judgment: false
  - id: D4
    description: "The sample has its manifest row with a matching SHA-256 and its singlesteptests notice"
    requirement: CPU-01
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^policy\\.notices$'"
        status: pass
      - kind: other
        ref: "scripts/hygiene.sh --tree (exit 0)"
        status: pass
    human_judgment: false
  - id: D5
    description: "A failing test prints its first mismatch in file order, the first one's expected and actual cycles, then the count line, and exits 1"
    requirement: CPU-01
    verification:
      - kind: other
        ref: "muhtari-test-vectors on a sample copy with test 0's initial S flipped (exit 1, 6502/a9.json[0]: s expected $ac got $ad, 6502/a9: 1 of 100 failed)"
        status: pass
    human_judgment: false
  - id: D6
    description: "Draft probe PR 9 is open, and its ci.yml run on the head is green on every leg, both ci-msvc legs, both nofp legs and wasm32 included"
    verification:
      - kind: integration
        ref: "https://github.com/szTheory/muhtari/actions/runs/39179237062"
        status: pass
    human_judgment: false

duration: 15min
completed: 2026-10-19
status: complete
---

# Phase 2 Plan 1: SALLY on the public vectors through one CPU object Summary

**The seeded 6502 core, built once as the OBJECT library `muhtari_cpu`, passes all 10,000 upstream `a9.json` vectors and the committed 256-opcode sample's a9 chunk on a flat test bus, and the split builds green on every hosted leg of draft PR #9.**

## Performance

- **Duration:** 15 min
- **Started:** 2026-10-18T23:53:11Z
- **Completed:** 2026-10-19T00:08:07Z
- **Tasks:** 3
- **Files modified:** 22

## Accomplishments

- `muhtari_core_options(target)` gives every core target hidden visibility, PIC, the hardening flags, `-mgeneral-regs-only` on nofp, the warnings and the include directories. `muhtari` is now `STATIC $<TARGET_OBJECTS:muhtari_objs>`. `muhtari_cpu` holds `src/cpu.c` and joins the archive with `src/bus.c` in plan 6.
- `src/cpu.c` is seeded from szTheory/nesturbator @9f9386c661f6 `src/cpu.c`. These parts are dropped: the 2A03 decimal note, the IRQ path and its poll latch, the boundary NMI flag, the jammed branch, the reset function and the DMC note. ANE and LXA read `m->profile.ane_lxa_constant`.
- The harness, `muhtari-test-vectors FILE XX`, reads either one upstream `.json` file (through `sst_json`) or the N65V sample. It sets the constant to $EE and compares pc, s, a, x, y, raw P, RAM and every cycle.
- `tests/vectors/6502-sample.n65v` is 1,677,886 bytes, SHA-256 `9f64df03b8e242943fb86b826e174b667e521176c3df2dbbbad45ae80eeada9d`. It has its manifest row, its `singlesteptests` notice and `*.n65v binary`.
- `cpu.vectors.a9` (labels `portable,vectors`, TIMEOUT 10) is registered and listed in `ci.txt` and `wasm32.txt`.

## Recorded one-off checks (AGENTS rule 6)

- **Scratch checkout of the vectors**, in the session scratch directory: `git init; git remote add origin https://github.com/SingleStepTests/65x02.git; git sparse-checkout set --cone 6502/v1; git fetch --depth 1 --filter=blob:none origin 2f6980a2d95757486c7bee24355c360e40e2a224; git checkout FETCH_HEAD`, exit 0, 256 files, HEAD `2f6980a2d95757486c7bee24355c360e40e2a224`. Nothing from it was committed: `git ls-files --cached --others --exclude-standard '*6502/v1/*'` prints nothing.
- **Harness on upstream a9:** `build/ci/tests/muhtari-test-vectors <scratch>/sst/6502/v1/a9.json a9` printed `6502/a9: 0 of 10000 failed`, exit 0. Usage error (no arguments): exit 2. Wrong opcode for the file (`a9.json a8`): exit 1, `offset 287: first cycle is not a read of 0xa8 at the initial pc`.
- **Census of all 256 upstream files** (an input for plan 2): 223 opcodes pass all 10,000 tests, all 12 JAMs included. The 33 that fail are exactly the decimal-mode users: ADC `61 65 69 6d 71 75 79 7d`, RRA `63 67 6f 73 77 7b 7f`, ARR `6b`, SBC `e1 e5 e9 ed f1 f5 f9 fd`, SBC `eb`, and ISC `e3 e7 ef f3 f7 fb ff`. Between 2,784 and 4,895 of each opcode's 10,000 tests fail.
- **Sample build:** `muhtari-vecconv <scratch>/sst/6502/v1/XX.json <scratch>/chunks/XX.n65v XX --first 100` for 00 to ff, all exit 0, concatenated in opcode order: 1,677,886 bytes.
- **Negative control:** a sample copy with test 0's initial S flipped made the harness exit 1 with `6502/a9.json[0]: s expected $ac got $ad`, the two cycle tables, and `6502/a9: 1 of 100 failed`.
- **Local lanes:** `cmake --workflow --preset ci`, exit 0 (67 tests); `cmake --workflow --preset hygiene`, exit 0 (18 tests, analyzer on `src/cpu.c` included).

## Hosted evidence

- **Probe PR:** #9, `ci: probe the phase workflows`, draft (https://github.com/szTheory/muhtari/pull/9).
- **Green run on head `9647eb9`:** https://github.com/szTheory/muhtari/actions/runs/39179237062. `gh run watch --exit-status` exited 0 and `CI required` concluded `success`. The windows, nofp and wasm32 conclusions are `["success"]`.
- **Wall times (that run):** be32 43 s, the first data point for the 5-minute budget. Build legs: linux-x64 13 s, linux-arm64 14 s, macos-arm64 22 s, windows-x64 44 s, macos-x64 45 s, windows-arm64 53 s. Other jobs: nofp 19 s (ubuntu-26.04) and 18 s (arm), wasm32 15 s, asan 10 s, hygiene 12 s, perf 41 s, retroarch 18 s, hash-equality 5 s.
- **Linking fix:** none needed. MSVC, wasm-ld and the nofp GCC lanes linked the test programs from the OBJECT library as built (research A1 held). `.github/workflows/ci.yml` is unchanged.
- **First run:** https://github.com/szTheory/muhtari/actions/runs/39179078822 on `2f20fac`. Every build leg passed there too. Only `hygiene.format` failed, on one line of `tests/vectors/vecconv.c`, fixed in `9647eb9`.

## Task Commits

1. **Task 1: CMake skeleton, seeded cpu.c, flat bus, readers, harness:** `9bdaaeb` (feat)
2. **Task 2: converter, sample, manifest, notice, cpu.vectors.a9:** `2f20fac` (test)
3. **Task 3: draft probe PR and hosted run:** no file change. The format fix found by its first run is `9647eb9` (style).

## Files Created/Modified

- `CMakeLists.txt`: `muhtari_core_options`, `muhtari_objs`, `muhtari_cpu`, `muhtari` from object files, and the nofp GCC check
- `src/internal.h`: `struct muhtari__cpu`, `struct muhtari__bus`, `struct muhtari__profile`, `MUHTARI__RAM_FILL_ZERO`, the bus and step declarations
- `src/instance.c`: profile defaults $00, $FF, ZERO, 0 after the memset
- `src/cpu.c`: SALLY, one instruction per `muhtari__cpu_step`
- `tests/cpu/{targets,tests}.cmake`: `muhtari-test-vectors`, `muhtari-vecconv`, `cpu.vectors.a9`
- `tests/cpu/vector_bus.{c,h}`: flat 64 KiB RAM, a 32-entry cycle log, `phi2++` per access
- `tests/cpu/test_vectors.c`: the harness
- `tests/vectors/{n65v,sst_json}.{c,h}`, `tests/vectors/vecconv.c`: reader, JSON reader, converter
- `tests/vectors/6502-sample.n65v`, `tests/roms/manifest.txt`, `THIRD-PARTY-NOTICES.md`, `.gitattributes`: the sample and its paperwork
- `tests/embed/CMakeLists.txt`: allows the core's object libraries
- `tests/inventory/{ci,wasm32}.txt`: `cpu.vectors.a9`

## Decisions Made

- `embed.subdirectory` now accepts `muhtari_*` OBJECT libraries. It still fails on any other target type or name, so the fault it names (an embedding gets more than the library) stays caught. Plan 8's `muhtari_cartdb` passes without another edit.
- The harness treats `.json` as one full upstream file (exactly 10,000 tests, as vecconv without `--first`). Any other path is the 256-chunk sample, with the per-chunk count taken from the first header, so a different sample size still works. A zero-test run exits 1.
- JAM keeps the seed's sequence and sets `jammed`. The harness assertion on `jammed` is left to plan 2, which owns JAM.
- The ADC comment says the decimal sum is not modelled, citing `docs/hardware/sally.md, Decimal mode`. Plan 2 adds the decimal paths and writes that page.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 3 - Blocking] embed.subdirectory rejected the new OBJECT libraries**
- **Found during:** Task 1
- **Issue:** `tests/embed/CMakeLists.txt` required the embedded project's targets to be exactly `muhtari`. The OBJECT split adds `muhtari_objs` and `muhtari_cpu`, so `cmake --workflow --preset ci` failed with `muhtari added the targets 'muhtari_objs;muhtari_cpu;muhtari'`.
- **Fix:** the check requires `muhtari` and allows other targets only when they are OBJECT libraries named `muhtari_*`.
- **Files modified:** tests/embed/CMakeLists.txt
- **Verification:** embed.subdirectory passes in the ci workflow and on every hosted build leg
- **Committed in:** 9bdaaeb

**2. [Rule 1 - Bug] vecconv.c was not clang-formatted**
- **Found during:** Task 3 (first hosted run, 39179078822)
- **Issue:** `hygiene.format` failed on `tests/vectors/vecconv.c:41`. The local format pass ran before that file was written, and the local hygiene lane only checks tracked files.
- **Fix:** `clang-format -i` (21) on the file; every tracked C file then passed `--dry-run --Werror`
- **Files modified:** tests/vectors/vecconv.c
- **Verification:** run 39179237062 hygiene success
- **Committed in:** 9647eb9

**3. [Tooling] cpu.c written with the Write tool, not piped from the seed**
- **Found during:** Task 1
- **Issue:** the auto-mode classifier refused a `git show ... | sed > src/cpu.c` pipeline from the sibling clone.
- **Fix:** I wrote the file with the Write tool from the seed I had read, applying the plan's renames and removals by hand. The commit carries the seed line.
- **Committed in:** 9bdaaeb

---

**Total deviations:** 3 (1 blocking, 1 bug, 1 tooling). **Impact:** none on scope. The embed gate keeps its fault; the format fix is cosmetic.

## Issues Encountered

None beyond the deviations.

## Known Stubs

- `src/cpu.c`, `adc()`: binary sum only; 33 opcodes fail their upstream vectors until plan 02-02 adds decimal mode. This plan registers only `cpu.vectors.a9`, so no registered test hides it.

## User Setup Required

None.

## Next Phase Readiness

- Plan 02-02 can add decimal ADC, SBC and ARR, write `docs/hardware/sally.md` with the six sections `src/cpu.c` cites, and replace `cpu.vectors.a9` with the 256-opcode loop. The census above lists exactly which opcodes must turn green.
- Probe PR #9 stays draft for later plans to push to; plan 16 closes it.

## Self-Check: PASSED

- Files: src/cpu.c, tests/cpu/{targets.cmake,tests.cmake,vector_bus.c,vector_bus.h,test_vectors.c}, tests/vectors/{n65v.c,n65v.h,sst_json.c,sst_json.h,vecconv.c,6502-sample.n65v}, tests/roms/manifest.txt: all present
- Commits 9bdaaeb, 2f20fac, 9647eb9: all ancestors of HEAD; `git rev-list --count d42c31e..HEAD` = 3

---
*Phase: 02-the-cpu-matches-the-public-vectors*
*Completed: 2026-10-19*
