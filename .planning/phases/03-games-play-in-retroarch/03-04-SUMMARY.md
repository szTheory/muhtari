---
phase: 03-games-play-in-retroarch
plan: 04
subsystem: api
tags: [api, load, input, memory, counters, lag, runner, c17, ctest]

requires:
  - phase: 03-games-play-in-retroarch
    provides: "The internal load, the fast-boot hand-off, the bounded run loop and span rendering (plans 03-02 and 03-03)"
provides:
  - "Public API version 3: four new statuses, MUHTARI_PORT_UNPLUGGED, muhtari_media, the button, switch and memory-kind constants, muhtari_input, muhtari_span, muhtari_counters, and muhtari_load, muhtari_power_cycle, muhtari_set_input, muhtari_get_memory, muhtari_get_counters"
  - "muhtari_load: validate-then-apply (probe status, region with PAL refused, BIOS size and vector), the ROM as the only allocation, the BIOS in a fixed 16 KiB array, then a power cycle into the fast boot"
  - "src/ports.c: pending input latched at each VBLANK start, device resolution from the header, live memory spans, frame and lag counters"
  - "The lag flag set by a CPU read of SWCHA, SWCHB or INPT0-5 in bus.c, not by a peek or MARIA's DMA; internal muhtari__bus_peek and muhtari__riot_peek"
  - "muhtari-run [--rom] FILE: loads through muhtari_load, refusals exit 77, malformed images 1"
  - "tests/api/v3.c and abi.api_v3; core.load.*, core.set_input.*, core.get_memory.spans, core.counters.frames_and_lag; runner.load.hash and runner.load.pal_refused"
affects: [03-05, 03-08, 03-10, 03-13, libretro adapter, movies]

actuals:
  tokens: 21167
  tasks: 3
  commits: 4
plan_head_before: 7f01a56314be970751666fbc3058fdc89e3b3a38
plan_head_after: 91aa611832350a6efb7a844ceeb608cacee806ea

tech-stack:
  added: []
  patterns:
    - "A public call that loads validates through muhtari_probe and installs through muhtari__cart_install, so the image is identified once and a refusal allocates nothing"
    - "Input that is not machine state waits in pending_ fields and is applied by muhtari__ports_latch at the start of every frame that begins at a VBLANK start"
    - "Core tests that need what the public API hides include src/internal.h and link the in-memory image builders"

key-files:
  created:
    - src/ports.c
    - tests/api/v3.c
  modified:
    - include/muhtari.h
    - src/internal.h
    - src/instance.c
    - src/cart.c
    - src/bus.c
    - src/riot.c
    - src/frame.c
    - CMakeLists.txt
    - host/status_text.c
    - runner/main.c
    - runner/exit_status.h
    - tests/api/v2.c
    - tests/abi/tests.cmake
    - tests/header/header.c
    - tests/header/header.cpp
    - tests/core/test_core.c
    - tests/core/tests.cmake
    - tests/cart/write_images.c
    - tests/runner/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - tests/inventory/nofp.txt
    - docs/embedding.md
    - docs/runner.md

key-decisions:
  - "The input latch runs at the start of muhtari_run_frame whenever the frame starts at a VBLANK start, which is every frame but the first after a load; input set before that first frame applies at the start of frame 2"
  - "A header that names no controller (A78 code 0) leaves the port unplugged; a header device this version does not model is wired as a ProLine"
  - "With no content loaded every frame counts as a lag frame, since no CPU reads an input register"
  - "Applied and pending input survive a load and a power cycle: they are the host's controllers, not the console's state"
  - "muhtari_load checks the region before the BIOS; a NULL bios with a non-zero bios_size is ERR_ARGUMENT"

patterns-established:
  - "A frozen API test for version N relaxes its predecessor's exact MUHTARI_API_VERSION check to >= when version N+1 lands, as v1.c did for v2"

requirements-completed: []

coverage:
  - id: D1
    description: "The runner loads a cartridge FILE through muhtari_load and prints a stable frame-120 hash that differs from the test card's, for the built simple sample and for a seed image, with or without --rom"
    requirement: GAME-03
    verification:
      - kind: e2e
        ref: "muhtari-run --frames 120 --hash-frame 120 build/ci/built/simple/simple.bas.a78, twice, against the no-content run (exit 0)"
        status: pass
      - kind: e2e
        ref: "ctest --test-dir build/ci -R '^runner\\.load\\.hash$' (exit 0)"
        status: pass
    human_judgment: false
  - id: D2
    description: "A PAL cartridge is refused with ERR_UNSUPPORTED_REGION, by its header or by media.region, and the runner exits 77"
    requirement: GAME-03
    verification:
      - kind: e2e
        ref: "ctest --test-dir build/ci -R '^runner\\.load\\.pal_refused$' (exit 0)"
        status: pass
      - kind: unit
        ref: "tests/core/test_core.c#load.keeps_previous"
        status: pass
    human_judgment: false
  - id: D3
    description: "muhtari_load validates before applying: a refused load (2600, PAL, bad BIOS) keeps the previous cartridge's frames and RAM; an empty or NULL-with-size image is refused with no allocation; BIOS sizes 0, 4095 and 8192 and a vector outside the image are ERR_INVALID_BIOS, and a valid BIOS is copied into the instance without allocating"
    requirement: GAME-03
    verification:
      - kind: unit
        ref: "tests/core/test_core.c#load.keeps_previous"
        status: pass
      - kind: unit
        ref: "tests/core/test_core.c#load.empty_refused"
        status: pass
      - kind: unit
        ref: "tests/core/test_core.c#load.invalid_bios"
        status: pass
    human_judgment: false
  - id: D4
    description: "muhtari_set_input refuses unknown button and switch bits, unknown devices and an explicit unmodelled device, changing nothing, and resolves device 0 from the header"
    requirement: GAME-04
    verification:
      - kind: unit
        ref: "tests/core/test_core.c#set_input.validates"
        status: pass
    human_judgment: false
  - id: D5
    description: "Accepted input is latched and reaches the machine at the next VBLANK start; of two calls before it, the second applies"
    requirement: GAME-04
    verification:
      - kind: unit
        ref: "tests/core/test_core.c#set_input.latched_at_vblank"
        status: pass
    human_judgment: false
  - id: D6
    description: "muhtari_get_memory returns live, stable spans of system RAM (4 KiB, what the CPU reads at $1800) and RIOT RAM (128 bytes), an empty span for cartridge RAM, and ERR_ARGUMENT for kinds 0 and 4"
    requirement: GAME-05
    verification:
      - kind: unit
        ref: "tests/core/test_core.c#get_memory.spans"
        status: pass
    human_judgment: false
  - id: D7
    description: "muhtari_get_counters counts frames since load or power cycle and lag frames; a CPU read of SWCHA or INPT4 clears a frame's lag, a peek and a MARIA fetch of $0280 do not"
    requirement: GAME-05
    verification:
      - kind: unit
        ref: "tests/core/test_core.c#counters.frames_and_lag"
        status: pass
    human_judgment: false
  - id: D8
    description: "tests/api/v3.c calls every new function, assigns every new field and checks every new constant; abi.api_version, abi.symbols and the C17 and C++17 header checks cover API version 3"
    requirement: GAME-03
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^abi\\.' (exit 0)"
        status: pass
    human_judgment: false
  - id: D9
    description: "docs/embedding.md describes loading, the first frame after load, input, memory and counters, and every new name; docs/runner.md describes [--rom] FILE and the 77 row"
    requirement: GAME-03
    verification:
      - kind: other
        ref: "ctest --test-dir build/ci -R '^(docs\\.|runner\\.exit\\.table|host\\.)' (exit 0)"
        status: pass
    human_judgment: false

duration: 13min
completed: 2026-10-20
status: complete
---

# Phase 3 Plan 04: Public API version 3 Summary

**API version 3 end to end: muhtari_load validates then fast-boots a cartridge, input latches at VBLANK start, live RAM spans and lag counting, and `muhtari-run [--rom] FILE` prints a stable hash for the simple sample**

## Performance

- **Duration:** 13 min
- **Started:** 2026-10-20T00:35:41Z
- **Completed:** 2026-10-20T00:48:57Z
- **Tasks:** 3
- **Files modified:** 26

## Accomplishments

- The header carries D-02's delta, and the versions struct's api comment reads `/* MUHTARI_API_VERSION */`. ABI and behaviour revision stay 1.
- `muhtari_load` checks the probe status, the region and the BIOS before it applies anything. The ROM copy is its only allocation, the BIOS goes into `uint8_t bios[16384]`, and the load ends with a power cycle into the fast boot.
- `src/ports.c` holds the pending and applied input, the latch, device resolution, memory spans and counters. The lag flag lives in the CPU read path of `bus.c`.
- The runner takes one positional FILE or `--rom FILE`. `simple.bas.a78` gives frame 120 `07c13a65...ec8df` on every run, and the test card gives `9eb011e2...05c4`.
- `tests/api/v3.c` and `abi.api_v3` now run on the ci, wasm32 and nofp lanes, along with seven new core cases and two runner cases.

## Task Commits

1. **Task 1: header delta, muhtari_load and muhtari_power_cycle, runner FILE** - `79ff79c` (feat)
2. **Task 2: set_input, get_memory, get_counters and lag** - `8c68d1c` (test, RED), `fb79feb` (feat, GREEN)
3. **Task 3: status texts and docs** - `91aa611` (docs)

## Verification

| Command | Exit |
|---|---|
| `cmake --workflow --preset ci` (441 tests) | 0 |
| `ctest --test-dir build/ci -R '^(runner\.load\.\|runner\.exit\.table\|core\.\|abi\.)'` | 0 |
| simple frame-120 hash twice, against the no-content hash (Task 1 verify) | 0 |
| `ctest --test-dir build/ci -R '^(core\.\|abi\.)'` | 0 |
| `ctest --test-dir build/ci -R '^(docs\.\|runner\.exit\.table\|host\.)'` | 0 |
| `scripts/hygiene.sh --tree` | 0 |
| `PATH=<llvm 21>:$PATH cmake --workflow --preset hygiene` (format, analyzer, 24 tests) | 0 |
| `muhtari-run --frames 1 build/ci/runner-probe/linear-v3-16k-pal` | 77 |

Tracer gate (Task 1): human_verify_mode is end-of-phase and the verify is automated only, so it was re-run. It exited 0, and the plan continued.

## TDD (Task 2)

- **RED** (`8c68d1c`): the header delta, internal declarations and stub functions in `src/ports.c`, so the cases compile and fail on their assertions. `abi.api_v3`, `core.set_input.validates`, `core.set_input.latched_at_vblank` and `core.counters.frames_and_lag` failed on status and value checks such as `muhtari_set_input(m, &in) == MUHTARI_STATUS_OK failed: 1 != 0`. `core.get_memory.spans` failed its assertions and then crashed on the NULL span the stub returned. The three `core.load.*` cases passed in RED, because Task 1 had already built the load; they pin Task 1's behaviour as regression cases. The evidence is CTest console output, a format the RED classifier does not parse, so the assessment was made by reading it: every target case ran, and the failures were on the planned assertions. `workflow.tdd_mode` is off.
- **GREEN** (`fb79feb`): all cases pass.
- **REFACTOR:** none needed.

## Files Created/Modified

- `include/muhtari.h` - API version 3 delta
- `src/internal.h` - size-first macros and layout rows for the four new structs, `struct muhtari__ports`, the counters, `bios[16384]`, peek, latch and device declarations
- `src/instance.c` - `muhtari_load`, `muhtari_power_cycle`
- `src/cart.c` - `muhtari__cart_install` split out of `muhtari__cart_load`
- `src/ports.c` - set_input, latch, device resolution, get_memory, get_counters
- `src/bus.c` - input-register reads set the lag flag; `muhtari__bus_peek`
- `src/riot.c` - `muhtari__riot_peek`
- `src/frame.c` - latch at a frame's VBLANK start; frame and lag counting
- `host/status_text.c` - messages for statuses 10-13
- `runner/main.c`, `runner/exit_status.h` - `[--rom] FILE`, the shared file reader, the 77 row
- `tests/api/v3.c`, `tests/api/v2.c`, `tests/header/header.c`, `tests/header/header.cpp`, `tests/abi/tests.cmake` - API version 3 checks
- `tests/core/test_core.c`, `tests/core/tests.cmake` - seven new cases
- `tests/cart/write_images.c` - writes `linear-v3-16k-pal`
- `tests/runner/tests.cmake` - `runner.load.*`; `usage.positional` redefined
- `tests/inventory/{ci,wasm32,nofp}.txt` - the new tests
- `docs/embedding.md`, `docs/runner.md` - loading, first frame, input, memory, counters, statuses, the runner FILE

## Decisions Made

See `key-decisions` above. Two need a word:
- **Latch placement.** The latch runs at the start of `muhtari_run_frame` when the frame starts at a VBLANK start. This is DEC.25's lowest-latency reading: input set just before a frame applies to that frame. The one exception is the frame after a load, which starts at the hand-off.
- **Header port 0.** The A78 code for no controller resolves to unplugged. Only a device code this version does not model resolves to ProLine (D-03).

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 3 - Blocking] The header delta was split across Tasks 1 and 2**
- **Found during:** Task 1
- **Issue:** `abi.symbols` fails on a declared function that nothing defines, and `abi.api_version` needs `tests/api/v3.c` once the header says version 3. Applying the whole delta in Task 1 would have left Task 1's CI red, or forced stub definitions into it.
- **Fix:** Task 1 added the statuses, `MUHTARI_PORT_UNPLUGGED`, `muhtari_media`, load and power_cycle, plus a `v3.c` covering them. Task 2 added the input, memory and counter names with their RED stubs. The final header is D-02's delta unchanged.
- **Committed in:** 79ff79c, 8c68d1c

**2. [Rule 3 - Blocking] Existing tests pinned API version 2**
- **Found during:** Task 1
- **Issue:** `tests/api/v2.c` (`== 2`), `tests/header/header.c` and `.cpp` (`== 2`), `core.info` (`v.api == 2`) and the `abi.api_version.self_test` regex (`K <= 2`) all failed at version 3.
- **Fix:** v2.c now checks `>= 2`, following the precedent v1.c set at the version 2 bump. The header checks, `core.info` and the regex now say 3, and the header checks gained the new size-first asserts and function signatures.
- **Committed in:** 79ff79c, 8c68d1c

**3. [Rule 3 - Blocking] Status texts and guide sections landed with the code that needs them**
- **Found during:** Tasks 1 and 2
- **Issue:** The runner prints `muhtari__status_text` for a refused load. `docs.api_names` fails on any header name that the guide never mentions. Leaving both for Task 3 would have left Tasks 1 and 2 red.
- **Fix:** The status texts and the Loading content and Statuses sections went in with Task 1. Input, Memory and Counters went in with Task 2. Task 3 added the first-frame section and the runner docs, and corrected the order of the guide's load checks to match the code.
- **Committed in:** 79ff79c, fb79feb, 91aa611

**4. [Rule 2 - Missing critical] Side-effect-free peeks for the lag tests**
- **Found during:** Task 2
- **Issue:** The behaviour "a peek does not clear the lag flag" needs a peek. D-03 lists one as internal, but none existed, and a RIOT timer read has side effects.
- **Fix:** Added `muhtari__bus_peek` (bus.c) and `muhtari__riot_peek` (riot.c). Neither runs a cycle, clears a flag or records an input read.
- **Committed in:** fb79feb

**5. [Rule 3 - Blocking] A PAL test image**
- **Found during:** Task 1
- **Issue:** No image that `muhtari-test-images` writes resolves to PAL, so `runner.load.pal_refused` had no input.
- **Fix:** `write_images.c` also writes `linear-v3-16k-pal`: the linear seed with the header's TV byte set. The fuzz seed list is unchanged.
- **Committed in:** 79ff79c

---

**Total deviations:** 5 auto-fixed (1 missing critical, 4 blocking).
**Impact on plan:** All five reorder or enable the planned work. Nothing outside the plan's API shape was published; the peeks are internal.

## Issues Encountered

None.

## Known Stubs

- `src/instance.c` `muhtari_load`: a valid BIOS is copied into `m->bios`, but the machine still fast-boots. The plan names this gap, and plan 03-13 closes it with the BIOS boot. It is recorded in `.planning/WINDOWS.md`.
- `MUHTARI_MEMORY_CART_RAM` returns an empty span because no board with RAM runs yet. This is correct behaviour for now, not a stub; plan 03-08 adds SuperGame RAM.

## Next Phase Readiness

- The libretro adapter and the movie runner can now load content, feed input and read memory and counters through the public API.
- Plan 03-10 wires the device pins to `muhtari__ports_device` and the applied `buttons` and `switches`. Until then, SWCHA, SWCHB and INPT read as before.
- The open item from 03-03 is still open: a DLI on zone 0's entry raises no NMI.

## Self-Check: PASSED
