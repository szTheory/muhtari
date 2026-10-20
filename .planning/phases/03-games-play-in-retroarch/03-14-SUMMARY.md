---
phase: 03-games-play-in-retroarch
plan: 14
subsystem: testing
tags: [compat, ledger, title, predicate, openbios, bios, pin]

requires:
  - phase: 03-games-play-in-retroarch
    provides: "The BIOS path and the hand-off frame K (plan 03-13), the built samples and OpenBIOS fixtures (plans 03-08 and 03-12), the diagnostics (plans 03-06 to 03-09)"
provides:
  - "tests/compat/ledger.csv: 42 rows in the CONFORMANCE section 5 schema, sorted by id then predicate"
  - "muhtari-test-predicate: evaluates title@F=H and boots@F on the fast or BIOS path, and pins refusing a single-colour frame"
  - "tests/compat/pin.cmake: fills title@F=- rows and, with -DBIOS, evaluates every row on both paths"
  - "title.simple.fast, title.simple.bios and title.pin.self_test on the ci and wasm32 inventories"
affects: [03-17, 03-18, compatibility doc, library job, ratchet]

actuals:
  tokens: 10244
  tasks: 3
  commits: 3
plan_head_before: e7c2c24eca3ce250b58bc23626b37aba4ddf6a08
plan_head_after: c4bd4ea75f8e8f4498d3d3e25df0591410b044ab

tech-stack:
  added: []
  patterns:
    - "A ledger predicate's ';why=' note is kept out of CMake list splitting by a placeholder while pin.cmake edits rows"
    - "Rows sort on an 'id predicate' key, so a regenerated ledger diffs cleanly"

key-files:
  created:
    - tests/compat/predicate.c
    - tests/compat/pin.cmake
    - tests/compat/ledger.csv
    - tests/compat/tests.cmake
    - .planning/phases/03-games-play-in-retroarch/03-14-SUMMARY.md
  modified:
    - tests/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - docs/emulation-tests.md

key-decisions:
  - "Five samples whose screens depend on rand (7800basic seeds it from INTIM, which the BIOS leaves at another value) fall back to boots@F;why=rng-seeded-from-intim"
  - "hiscoretable shows one colour until frame 262, so its predicate uses frame 300 (boots@300, after the RNG fallback)"
  - "backuptest (no main loop, jams) and languagetest (draws nothing by its own source) are skipped with tier local and the reason in the predicate, since no screen predicate can judge them and a blank pin is forbidden"
  - "Sample titles use the sample name, because 7800basic's header title is the default 'My Game'; board is the probe's board plus +ram and +options; peripherals pokey450 and savekey from the probe"
  - "evidence_commit is the core commit evaluated (e7c2c24eca3c); the core is unchanged by this plan"

patterns-established:
  - "A new ROM gets a title@120=- unknown row and pin.cmake fills it; hashes are never typed"

requirements-completed: []

coverage:
  - id: D1
    description: "muhtari-test-predicate passes simple's title@120 on the fast boot (frame 120) and through OpenBIOS (frame 328, K = 323)"
    requirement: GAME-03
    verification:
      - kind: integration
        ref: "tests/compat/tests.cmake#title.simple.fast and title.simple.bios (ctest --test-dir build/ci -L '^title$', exit 0)"
        status: pass
      - kind: other
        ref: "https://github.com/szTheory/muhtari/actions/runs/39368351340 (conclusion success, 18 jobs, head commit c4bd4ea)"
        status: pass
    human_judgment: false
  - id: D2
    description: "The pin writer refuses a single-colour frame-120 picture; the blank linear seed cartridge fails with single colour"
    requirement: GAME-03
    verification:
      - kind: unit
        ref: "tests/compat/tests.cmake#title.pin.self_test (expect_fail CODE 1, REGEX single colour, exit 0)"
        status: pass
    human_judgment: false
  - id: D3
    description: "42 ledger rows (34 samples, banksets unsupported, 7 diagnostics) with 16 fields each, no unpinned title row, sorted by id then predicate"
    requirement: GAME-06
    verification:
      - kind: other
        ref: "tail -n +2 tests/compat/ledger.csv | wc -l = 42; awk -F, 'NF != 16' prints nothing; grep -c ',title@120=-,' = 0; grep -c ',unsupported,' = 1"
        status: pass
    human_judgment: false
  - id: D4
    description: "Every evaluated row (34 title rows, 5 boots fallbacks) passes on both the fast and the OpenBIOS path"
    requirement: GAME-03
    verification:
      - kind: integration
        ref: "cmake -DPREDICATE=build/ci/tests/muhtari-test-predicate -DLEDGER=tests/compat/ledger.csv -DBUILT=build/ci/built -DDIAG=tests/diag -DBIOS=build/ci/built/openbios/openbios.bin -DCOMMIT=e7c2c24eca3c -DDATE=2026-10-20 -P tests/compat/pin.cmake (exit 0, 39 rows pass)"
        status: pass
    human_judgment: false
  - id: D5
    description: "docs/emulation-tests.md title section: both paths, K and its window, boots@F and the fallbacks, the refusal, pinning and re-pinning"
    requirement: GAME-03
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^docs\\.' (exit 0)"
        status: pass
    human_judgment: false

duration: 39min
completed: 2026-10-20
status: complete
---

# Phase 3 Plan 14: Title predicates and the first ledger Summary

**The compatibility ledger now has its first 42 rows. Every built sample and diagnostic has a title row whose frame-120 SHA-256 is pinned, or a fallback whose reason is stated in the row. `muhtari-test-predicate` checks each row on the fast boot and through OpenBIOS from the hand-off frame. The pin recipe refuses a frame of one colour.**

## Performance

- **Duration:** 39 min
- **Started:** 2026-10-20T04:02:09Z
- **Completed:** 2026-10-20T04:41:28Z
- **Tasks:** 3
- **Files modified:** 8

## Accomplishments

- **Evaluator.** `muhtari-test-predicate ROM PREDICATE [--bios FILE] [--pin]` (`tests/compat/predicate.c`):
  - `title@F=H` passes on the fast boot when frame F hashes to H.
  - Under `--bios` it passes when K arrives by frame 600, K is in [300, 340], and some frame in [K, K + 300) hashes to H.
  - `boots@F` passes when, within F frames of the hand-off, DMA ran, at least two distinct frames appeared and the CPU did not jam.
  - `--pin` prints `title@F=<hex>` for the fast boot's frame F, or fails with `single colour`.
  - Its frame hash matches `muhtari-run --hash-frame`.
- **Ledger.** It has 42 rows: 34 samples, `banksets` as `unsupported` (`boots@120;why=bankset-board`), and the 7 diagnostics. The rows carry the probe's hashes, board and peripherals, region `NTSC` and tier `pr`, and the oracle columns are `-`.
- **Pin recipe.** `tests/compat/pin.cmake` fills `title@F=-` rows. With `-DBIOS` it evaluates every row except `unsupported` and `skipped` ones, sets the status of `unknown` rows, and fails by name on any disagreement.
- **Tests.** `title.simple.fast`, `title.simple.bios` and `title.pin.self_test` run on the ci and wasm32 inventories, with labels `portable,title`.
- **Images checked.** Every frame 120 was viewed as a contact sheet before pinning. multisprite's fast and BIOS frames were viewed side by side to confirm the RNG cause.

## Full evaluation (recorded)

The command was `cmake -DPREDICATE=build/ci/tests/muhtari-test-predicate -DLEDGER=tests/compat/ledger.csv -DBUILT=build/ci/built -DDIAG=tests/diag -DBIOS=build/ci/built/openbios/openbios.bin -DCOMMIT=e7c2c24eca3c -DDATE=2026-10-20 -P tests/compat/pin.cmake`. It exited 0, and all 39 evaluated rows passed on both paths. On the BIOS path the title appeared between frames 324 and 441.

The first evaluation, before the fallbacks, also exited 0, but it set 5 rows to `fail`. Each passed on the fast path and failed on the BIOS path with "title not in frames 323-622". Those are the five fallbacks below.

### Fallback rows

| Row | Predicate | Status | Reason |
|-----|-----------|--------|--------|
| 7800basic-0.41/doublebuffer | `boots@120;why=rng-seeded-from-intim` | pass | sprite directions from `rand`; 7800basic's startup does `lda #1 / ora INTIM / sta rand`, and the BIOS leaves INTIM at another value |
| 7800basic-0.41/eightysix | `boots@120;why=rng-seeded-from-intim` | pass | same |
| 7800basic-0.41/multisprite | `boots@120;why=rng-seeded-from-intim` | pass | same; positions from `rand`, the same scene with different positions was viewed |
| 7800basic-0.41/onehundredandone | `boots@120;why=rng-seeded-from-intim` | pass | same |
| 7800basic-0.41/hiscoretable | `boots@300;why=rng-seeded-from-intim` | pass | score from `rand`; one colour until frame 262 (it counts 256 frames before drawing), so frame 300 |
| 7800basic-0.41/backuptest | `boots@120;why=no-main-loop-jams` | skipped (tier local) | the program is one `backup` directive with no loop; it runs into data and jams on one colour $1A |
| 7800basic-0.41/languagetest | `boots@120;why=draws-nothing` | skipped (tier local) | "A useless program that does nothing", by its source; black through frame 600 |
| 7800basic-0.41/banksets | `boots@120;why=bankset-board` | unsupported | the bankset board is refused, and no image is built |

## Task Commits

1. **Task 1 (tracer): evaluator, simple's row and title.simple.fast/bios:** `e693862` (test). For the tracer gate (end-of-phase, automated-only `<verify>`), the verify was re-run. The `ci` workflow passed all 564 tests, the `title` label passed, and `scripts/hygiene.sh --tree` exited 0.
2. **Task 2: every row pinned and evaluated, and the pin self test:** `0dbe9c6` (test).
3. **Task 3: the title docs:** `c4bd4ea` (docs). Hosted run 39368351340 on this head succeeded on all 18 jobs.

## Files Created/Modified

- `tests/compat/predicate.c`: the evaluator.
- `tests/compat/pin.cmake`: the pin and evaluation recipe. It is fixture code, not gate code.
- `tests/compat/ledger.csv`: the 42 rows.
- `tests/compat/tests.cmake`: the driver target and the three `title.*` tests.
- `tests/tests.cmake`: includes `compat/` with the built and bios tests on ci and wasm32.
- `tests/inventory/ci.txt` and `tests/inventory/wasm32.txt`: three lines each.
- `docs/emulation-tests.md`: the title section.

## Decisions Made

See key-decisions.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Bug avoided] Two samples cannot have a title pin**
- **Found during:** Task 2.
- **Issue:** The pin recipe refused `backuptest` (one colour $1A, CPU jammed) and `languagetest` (black through frame 600). Their sources show that both draw nothing by design, so neither a `title@` nor a `boots@` predicate can pass. Pinning them would have broken the no-blank-pin rule.
- **Fix:** Both are `skipped` with tier `local` (CONFORMANCE section 5: "deliberately not run; reason"), and the reason is in the predicate. pin.cmake does not run `skipped` rows.
- **Files modified:** tests/compat/ledger.csv, tests/compat/pin.cmake.
- **Commit:** 0dbe9c6.

**2. [Rule 3 - Blocking] hiscoretable's frame 120 is blank**
- **Found during:** Task 2.
- **Issue:** The sample counts 256 frames before drawing, so frame 120 is one colour.
- **Fix:** The evaluator takes any F. The row was pinned as `title@300`, then fell back to `boots@300` with the RNG reason, because its score uses `rand`.
- **Commit:** 0dbe9c6.

**3. [Scope] Status evaluation lives in pin.cmake**
- **Issue:** The plan asked for a full evaluation of every row on both paths, recorded once.
- **Fix:** `-DBIOS` on pin.cmake runs it, so one command reproduces it. It sets the status only of `unknown` rows and reports any other row that disagrees.
- **Commit:** 0dbe9c6.

**Total deviations:** 3 (1 Rule 1, 1 Rule 3, 1 scope).
**Impact:** The truth "one title@120=H row per ROM" holds for 27 samples and all 7 diagnostics. The other 7 samples carry the documented fallbacks above, as D-16's fallback provides.

## Unreleased Pins

No frame or title pin moved, and the core is unchanged. Every title pin in this plan is new.

## Issues Encountered

None beyond the fallbacks.

## Known Stubs

None. docs/emulation-tests.md says that the library job in CI evaluates every row, but that job lands in a later plan of this phase (CONTEXT D-19). Until then, the recorded command above is the full evaluation.

## Threat Flags

None.

## User Setup Required

None.

## Next Phase Readiness

The ledger and evaluator are ready for the ratchet, the generated compatibility doc and the library job. Probe PR #12 is still a draft. It was pushed to, but it is not marked ready and has not been merged.

## Self-Check: PASSED

- tests/compat/predicate.c, pin.cmake, ledger.csv and tests.cmake exist.
- Commits e693862, 0dbe9c6 and c4bd4ea are ancestors of HEAD.
- `cmake --workflow --preset ci` exited 0 with 565 tests plus 6 release tests. The hygiene workflow passed 24 of 24, and `scripts/hygiene.sh --tree` exited 0.
- Every task's acceptance criteria pass: the header line, the simple row count of 1, the two then three inventory lines, 42 rows, the diag/dma pin, one unsupported row, 16 fields per row, and 4 `title@120` or `boots@` mentions in the docs.
- Hosted run 39368351340 succeeded with 18 of 18 jobs.
