---
phase: 03-games-play-in-retroarch
plan: 08
subsystem: cartridge
tags: [supergame, sg9, bank6, cart-ram, diagnostic-rom, dasm, frame-pins]

requires:
  - phase: 03-games-play-in-retroarch
    provides: "the board switch and loader (03-02), the sample pins and frames.cmake (03-05), the diagnostic pipeline (03-06, 03-07)"
provides:
  - "SuperGame, with RAM, bank 6, SG9 (2^n + 1) and banked RAM in src/cart.c's switch; linear with RAM"
  - "one allocation of rom_size + ram_size per load; destroy frees the same size"
  - "muhtari_get_memory(CART_RAM) returns the board's RAM"
  - "tests/diag/linear16, bank6, sg144 (.s, .cfg, .a78) and diag.linear16, diag.bank6, diag.sg144"
  - "pinned 600-frame chains for doublebuffer, eightysix, onehundredandone, rmtramcompressiondemo, rmtramdemo"
affects: [03-09, 03-13, 03-19, phase-4-sound]

actuals:
  tokens: 14145
  tasks: 3
  commits: 6
plan_head_before: 3b1c5bd709dff69c13f1767934eeca901c5cf62b
plan_head_after: 27f85ff71dcb306741b8732bee93de3352087e2a

tech-stack:
  added: []
  patterns:
    - "A banked diagnostic puts banks at ORG n * $4000 with RORG; lib.s pads to the font with ds and ORG's $FF fill, so the last bank can be RORG $C000"
    - "The 2^n + 1 board stores its register modulo 2^n and maps bank 1 + register at $8000"

key-files:
  created:
    - tests/diag/linear16.s
    - tests/diag/linear16.cfg
    - tests/diag/linear16.a78
    - tests/diag/bank6.s
    - tests/diag/bank6.cfg
    - tests/diag/bank6.a78
    - tests/diag/sg144.s
    - tests/diag/sg144.cfg
    - tests/diag/sg144.a78
  modified:
    - src/cart.c
    - src/internal.h
    - src/instance.c
    - src/ports.c
    - src/boot.c
    - tests/core/test_core.c
    - tests/core/tests.cmake
    - tests/cart/test_cart.c
    - tests/fuzz/loader.c
    - tests/built/expected.txt
    - tests/diag/lib.s
    - tests/diag/expected.csv
    - tests/diag/tests.cmake
    - tests/roms/manifest.txt
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - docs/hardware/cartridge.md
    - docs/embedding.md
    - docs/emulation-tests.md

key-decisions:
  - "SG9 power-on shows bank 1 at $8000: the register is 0 and the board maps 1 + register; derived before sg144 ran, and the core changed to match"
  - "Banked RAM and linear-with-RAM are mapped too, because the loader already accepts them and now allocates their RAM: banked RAM takes its 16 KiB half from D5, linear RAM sits at $4000-$7FFF"
  - "Cart RAM is zeroed at load and on every power cycle (ram_fill has only the zero fill)"
  - "An 8 KiB-RAM list entry keeps 8 KiB; the & (ram_size - 1) mask mirrors it over $4000-$7FFF"

patterns-established:
  - "Board diagnostics: each bank starts with its own number, so a window's first byte names its bank"

requirements-completed: [GAME-01]

coverage:
  - id: D1
    description: "doublebuffer (SuperGame 128K with 16K RAM) plays 600 pinned fast-boot frames through the board switch; frames 120 and 600 were viewed before pinning"
    requirement: GAME-01
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci -R '^(built|basic)\\.doublebuffer$' --output-on-failure (exit 0)"
        status: pass
      - kind: e2e
        ref: "https://github.com/szTheory/muhtari/actions/runs/39358378619 (CI on 27f85ff, conclusion success)"
        status: pass
    human_judgment: false
  - id: D2
    description: "A SuperGame load is one allocation of rom_size + ram_size, freed with the same size; a refused SuperGame image allocates nothing; banks reset at power-on and power cycle; values are reduced modulo the bank count"
    requirement: GAME-01
    verification:
      - kind: unit
        ref: "tests/core/test_core.c#load.supergame_one_allocation (ctest -R '^core\\.load\\.supergame_one_allocation$', exit 0)"
        status: pass
      - kind: unit
        ref: "tests/cart/test_cart.c expect_load and tests/fuzz/loader.c replay (ctest -R '^(cart|fuzz)\\.', exit 0)"
        status: pass
    human_judgment: false
  - id: D3
    description: "muhtari_get_memory(CART_RAM) returns a stable 16,384-byte span equal to what the CPU reads at $4000-$7FFF"
    requirement: GAME-01
    verification:
      - kind: unit
        ref: "tests/core/test_core.c#get_memory.cart_ram (ctest -R '^core\\.get_memory\\.cart_ram$', exit 0)"
        status: pass
    human_judgment: false
  - id: D4
    description: "The linear16, bank6 and sg144 board ROMs report 28 records, all equal to the guide rows derived in docs/hardware/cartridge.md before the first run"
    requirement: GAME-01
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci -L '^diag$' --output-on-failure (exit 0); ctest -R '^diag\\.rebuild$' (exit 0)"
        status: pass
      - kind: other
        ref: "git merge-base --is-ancestor a6ed97c 5833f08 (exit 0): the rows precede the ROM bytes"
        status: pass
    human_judgment: false
  - id: D5
    description: "eightysix, onehundredandone, rmtramcompressiondemo and rmtramdemo build to the research digests and have pinned chains; all 34 buildable samples are pinned"
    requirement: GAME-01
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci -L '^(built|basic)$' --output-on-failure (exit 0, 76 tests)"
        status: pass
      - kind: other
        ref: "grep -cvE '^#|^$' tests/built/expected.txt = 34; no row has - in column 6"
        status: pass
    human_judgment: false
  - id: D6
    description: "docs/hardware/cartridge.md describes the Linear and SuperGame boards, the hostile-input rules and the joint ROM and cartridge RAM allocation; docs/embedding.md gives CART_RAM's size and lifetime"
    requirement: GAME-01
    verification:
      - kind: other
        ref: "ctest --test-dir build/ci -R '^docs\\.' (exit 0); grep -cE '^### (Linear|SuperGame)$' docs/hardware/cartridge.md = 2; grep -c '16,384 bytes' docs/embedding.md = 3"
        status: pass
    human_judgment: false

duration: 16min
completed: 2026-10-20
status: complete
---

# Phase 3 Plan 08: SuperGame boards Summary

**The SuperGame family now runs from one `switch` in `src/cart.c`: plain, with RAM, bank 6, SG9 (2^n + 1) and banked RAM. The ROM and its cartridge RAM share one allocation. `doublebuffer` and the other four SuperGame samples are pinned, and three of our own board ROMs matched all 28 values derived before their first run.**

## Performance

- **Duration:** 16 min
- **Started:** 2026-10-20T02:07:43Z
- **Completed:** 2026-10-20T02:23:22Z
- **Tasks:** 3
- **Files modified:** 28

## Accomplishments

- **Boards.** For SuperGame, $C000 holds the last bank. $8000 holds the register's bank (value mod banks), and $4000 holds the second-to-last bank, 16 KiB of RAM, bank 6, or bank 0 on SG9, which shows bank 1 + (v mod 2^n) at $8000. RAM is indexed `& (ram_size - 1)`. MARIA reads the same bytes the CPU does.
- **One allocation.** `muhtari__cart_install` allocates `rom_size + ram_size` once and frees the old cartridge with the same size, as `muhtari_destroy` does. Every refusal still returns before allocating. `CART_RAM` returns the RAM span.
- **Board ROMs.** linear16 reads 22, 64, 128 and 191; the last three are open-bus high bytes. bank6 reads 0, 6, 7, then 0-7 and 3 for v = 11. sg144 reads 1, 0, 8, then 1-8 and 2 for v = 9. All matched on the first run, and the results screen shows them in hex.
- **Samples.** 34 of 34 buildable samples are pinned. The screenshots taken before pinning show real pictures: sprites over a backdrop (doublebuffer), the 86-sprite demo, the 101-sprite demo, and the two RMT title screens.

## Commit order (expected values first, ROM run second)

| Step | Commit |
|---|---|
| SuperGame core, doublebuffer pin (tracer) | `060f9de` |
| board ROM rows and derivation | `a6ed97c` |
| ROM sources, lib.s fill, SG9 power-on fix | `95fe6a9` |
| ROM bytes, manifest rows, diag tests, first run | `5833f08` |
| hygiene.no_float fix | `e8b0066` |
| four sample pins, board docs | `27f85ff` |

## Task Commits

1. **Task 1: doublebuffer plays:** `060f9de` (feat)
2. **Task 2: board options and board ROMs:** `a6ed97c`, `95fe6a9`, `5833f08` (test), `e8b0066` (fix)
3. **Task 3: four sample pins, board docs:** `27f85ff` (test)

## Verification

| Command | Exit |
|---|---|
| `cmake --workflow --preset ci` (521 tests) | 0 |
| `ctest --test-dir build/ci -R '^(built\.doublebuffer\|basic\.doublebuffer\|core\.)'` | 0 |
| `ctest --test-dir build/ci -L '^diag$'` and `-R '^diag\.rebuild$'` | 0 |
| `ctest --test-dir build/ci -L '^(built\|basic)$'` (76 tests) and `-R '^docs\.'` | 0 |
| `cmake --workflow --preset hygiene` (24 tests) | 0 |
| `scripts/hygiene.sh --tree` (each task) | 0 |
| `gh run watch 39358378619 --exit-status` (CI on 27f85ff, every leg) | 0 |

Tracer gate (Task 1): `human_verify_mode` is end-of-phase and the verify is automated only, so the gate re-ran it: ci 0, the core and doublebuffer tests 0, hygiene 0. The tracer push's hosted run 39357713996 failed `hygiene.no_float` (see Deviations), which was fixed in `e8b0066`. The final run is green on every leg.

## Files Created/Modified

- `src/cart.c`, `src/internal.h`: the board switch, `cart_ram_at`, `cart_sg_bank`, `muhtari__cart_reset`, the joint allocation
- `src/instance.c`, `src/ports.c`, `src/boot.c`: free size, reset at power-on, the CART_RAM span
- `tests/core/test_core.c`, `tests/core/tests.cmake`: `core.load.supergame_one_allocation`, `core.get_memory.cart_ram`
- `tests/cart/test_cart.c`, `tests/fuzz/loader.c`: expect `rom_size + ram_size`
- `tests/diag/{linear16,bank6,sg144}.{s,cfg,a78}`, `lib.s`, `expected.csv`, `tests.cmake`, `tests/roms/manifest.txt`
- `tests/built/expected.txt`: five SuperGame rows and the POKEY-at-$0450 note
- `tests/inventory/ci.txt`, `wasm32.txt`: 15 new tests
- `docs/hardware/cartridge.md`, `docs/embedding.md`, `docs/emulation-tests.md`

## Decisions Made

See `key-decisions`.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Bug] SG9 showed bank 0 at $8000 at power-on**
- **Found during:** Task 2, while writing the derivation. Task 1 had already written bank-6 and SG9 into the switch.
- **Issue:** The first SG9 code stored `1 + v mod 8` only on a write, so a register of 0 at power-on mapped bank 0, not bank 1.
- **Fix:** The register is kept modulo 2^n and $8000 maps bank 1 + register. The core changed after the derivation commit and before sg144 first ran.
- **Committed in:** `95fe6a9`

**2. [Rule 2 - Missing functionality] Banked RAM and linear RAM are mapped**
- **Issue:** The loader accepts both and now allocates their RAM. Leaving the RAM unmapped would allocate memory no code reaches.
- **Fix:** Banked RAM takes its half from D5 and linear RAM sits at $4000-$7FFF. Both are covered in `core.load.supergame_one_allocation`.
- **Committed in:** `060f9de`

**3. [Rule 3 - Blocking] Allocation-size checks in the cart test and the fuzz loader**
- **Fix:** Both now expect `rom_size + ram_size`. Neither file was in the plan's file list.
- **Committed in:** `060f9de`

**4. [Rule 3 - Blocking] lib.s's `ORG FONT` broke a banked last bank**
- **Fix:** It is now `ds FONT-*, $FF`, the same fill ORG used. dma, cpu and dli rebuild byte for byte.
- **Committed in:** `95fe6a9`

**5. [Rule 1 - Bug] hygiene.no_float flagged "sections 2.1 and 7" in a cart.c comment**
- **Found during:** The hosted run of the tracer push (39357713996, hygiene job)
- **Fix:** The comment now cites the two sections separately. The local hygiene preset passes 24/24.
- **Committed in:** `e8b0066`

**6. [Plan detail] Bank 6 and SG9 code landed in Task 1, not Task 2**
- **Issue:** It is part of the same switch. The D-09 order still holds: the rows (`a6ed97c`) precede the first run (`5833f08`).

---

**Total deviations:** 6 (2 bugs, 1 missing functionality, 2 blocking, 1 ordering detail).
**Impact on plan:** No released pin moved. No existing pin moved: the 29 linear chains are unchanged.

## Issues Encountered

None beyond the deviations.

## Known Stubs

None.

## Next Phase Readiness

- POKEY at $0450 reads open bus. The pins for eightysix, pokey, rmtramcompressiondemo, rmtramdemo and rmtromdemo will move when sound lands, with a behaviour-revision bump.
- The libretro adapter does not expose CART_RAM through `retro_get_memory_*` yet.
- Still open: broken-windows item 5. The machine fast-boots until 03-13.

## Self-Check: PASSED
