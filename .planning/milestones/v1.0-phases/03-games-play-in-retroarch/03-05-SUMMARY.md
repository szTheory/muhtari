---
phase: 03-games-play-in-retroarch
plan: 05
subsystem: testing
tags: [frame-pins, sha256, 7800basic, ctest, ci, be32, asan]

requires:
  - phase: 03-games-play-in-retroarch
    provides: "muhtari_load, the fast-boot hand-off and span rendering (plans 03-02 to 03-04)"
provides:
  - "muhtari-test-chain: the 600-frame SHA-256 chain h_i = SHA256(h_{i-1} || frame_i) of a fast-booted ROM, with the library's behaviour revision"
  - "tests/built/frames.cmake: the pin check with the behaviour-revision guard and -DWRITE=ON"
  - "tests/built/expected.txt: # behaviour_revision 1 and 29 linear 7800basic samples with .a78, .bin and frame-chain pins"
  - "built.<name> (FIXTURES_SETUP built_<name>; built_samples on simple) and basic.<name> for each row, plus basic.revision.self_test"
  - "Draft probe PR #12, `ci: probe the phase workflows`"
affects: [03-08, 03-13, 03-14, 03-17, 03-19, phase 4 POKEY]

actuals:
  tokens: 10129
  tasks: 3
  commits: 4
plan_head_before: 3af294f347376c7dca88f6557b079600a85814cd
plan_head_after: 084fa7f31118621df09e277799f607235660e10b

tech-stack:
  added: []
  patterns:
    - "One built.<name> and one basic.<name> per non-comment row of tests/built/expected.txt, registered by a foreach"
    - "A -P negative control whose command needs the cross-compiling emulator calls a file(GENERATE)d script, so the emulator list needs no second level of escaping"

key-files:
  created:
    - tests/built/chain.c
    - tests/built/frames.cmake
  modified:
    - tests/built/tests.cmake
    - tests/built/expected.txt
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - src/maria.c
    - .planning/preparation/CONFORMANCE.md
    - docs/emulation-tests.md
    - docs/testing.md
    - .github/workflows/ci.yml
    - tests/cart/tests.cmake
    - tests/runner/tests.cmake

key-decisions:
  - "With -DWRITE=ON and a core behaviour revision above the file's, frames.cmake rewrites the row and the revision line and resets every other row's frame pin to -, so each row's own WRITE fills it; in check mode a - column fails"
  - "A pin equal to the core's chain is kept under -DWRITE=ON at the same revision; only a differing pin is refused"
  - "Hosted run 39351079824 with all 29 samples: be32 101 s and asan 36 s, so basic stays on the per-PR be32 and asan lanes and no lane inventory changes; slowest build leg macos-x64 101 s, so no timeout-minutes changes"

patterns-established:
  - "Before pinning a sample's frames, dump frames 120 and 600 and view them against the sample's source"

requirements-completed: []

coverage:
  - id: D1
    description: "muhtari-test-chain prints the behaviour revision and the 600-frame chain of a fast-booted ROM; the chain is stable across runs and starts at frame 1, the partial hand-off frame"
    requirement: GAME-01
    verification:
      - kind: integration
        ref: "build/ci/tests/muhtari-test-chain build/ci/built/simple/simple.bas.a78 600, twice, same chain d35eaf81... (exit 0)"
        status: pass
      - kind: integration
        ref: "ctest --test-dir build/ci -R '^basic\\.simple$' (exit 0)"
        status: pass
    human_judgment: false
  - id: D2
    description: "frames.cmake fails unless the file's behaviour revision equals the core's; -DWRITE=ON fills a - column, refuses to rewrite a differing pin at the same revision, refuses a core revision below the file's, and on a greater one rewrites the row and the line and resets the other rows"
    requirement: GAME-01
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci -R '^basic\\.revision\\.self_test$' (exit 0)"
        status: pass
      - kind: other
        ref: "frames.cmake on scratch copies of expected.txt: drift, refusal at revision 1, bump from 0 with another row reset to -, refusal below revision 2 (all as expected)"
        status: pass
    human_judgment: false
  - id: D3
    description: "Every linear 7800basic 0.41 sample (29) builds byte-identically to the research's candidate digests and has a pinned 600-frame chain; banksets and the five SuperGame samples have no row, with comments saying why"
    requirement: GAME-01
    verification:
      - kind: integration
        ref: "ctest --test-dir build/ci -L '^(built|basic)$' --output-on-failure (64 tests, exit 0)"
        status: pass
      - kind: other
        ref: "grep -cvE '^#|^$' tests/built/expected.txt = 29; ctest -N -L '^basic$' | grep -c 'basic\\.' = 30"
        status: pass
    human_judgment: false
  - id: D4
    description: "Every hosted leg (six build legs, asan, be32, wasm32) builds the samples to the pinned bytes and plays the pinned chains"
    requirement: GAME-01
    verification:
      - kind: e2e
        ref: "https://github.com/szTheory/muhtari/actions/runs/39351079824 (conclusion success)"
        status: pass
      - kind: e2e
        ref: "https://github.com/szTheory/muhtari/actions/runs/39351445117 (conclusion success)"
        status: pass
    human_judgment: false
  - id: D5
    description: "The pokey row's comment and CONFORMANCE section 2(a) record that the wasmtime 48.0.6 recipe builds a96b1dce..., superseding the wasmtime 49 figure"
    requirement: GAME-01
    verification:
      - kind: other
        ref: "grep -n 'a96b1dce' tests/built/expected.txt .planning/preparation/CONFORMANCE.md (2 lines)"
        status: pass
    human_judgment: false
  - id: D6
    description: "docs/emulation-tests.md describes the basic label: the chain, the behaviour_revision guard, --hash-frame bisection and the unreleased and released re-pin routes"
    requirement: GAME-01
    verification:
      - kind: other
        ref: "ctest --test-dir build/ci -R '^docs\\.' (exit 0); grep behaviour_revision and origin/main in docs/emulation-tests.md (1 line each)"
        status: pass
    human_judgment: false

duration: 16min
completed: 2026-10-20
status: complete
---

# Phase 3 Plan 05: Frame pins for the linear samples Summary

**29 linear 7800basic samples now build byte-identically and play 600 pinned fast-boot frames on every hosted leg. Each run is checked as one SHA-256 chain, and a behaviour-revision guard stops a changed hash from being pinned silently.**

## Performance

- **Duration:** 16 min
- **Started:** 2026-10-20T00:51:45Z
- **Completed:** 2026-10-20T01:08:06Z
- **Tasks:** 3
- **Files modified:** 13

## Accomplishments

- `muhtari-test-chain FILE FRAMES` loads with `muhtari_load`, runs the frames into a 320-pitch buffer and prints `behaviour_revision N` and `chain <hex>`. It exits 0, or 1 on a load or run failure, or 2 on a usage error.
- `frames.cmake` compares a row's column 6 at the file's `# behaviour_revision 1`. It fails as `NAME: frame chain drift: expected ... got ...` and refuses a rewrite as `NAME: refusing to rewrite a pin at behaviour revision N`.
- `expected.txt` has 29 rows, sorted. Every pin came from `-DWRITE=ON` on the core under test, and none was typed by hand. `simple`'s chain is `d35eaf81...6ef40b`. Its frame 120 equals 03-03's data point, `07c13a65...`.
- Before pinning, frames 120 and 600 of every sample were viewed as images. Each matches its source: menus, text screens, the asteroids playfield, and `pokey` reporting "POKEY STATUS... NOT FOUND", which is right because this phase has no POKEY. `languagetest` is blank because it never calls `drawscreen`, and `backuptest` shows one colour.
- Draft probe PR #12 is open (https://github.com/szTheory/muhtari/pull/12). It is never to be marked ready or merged; plan 03-19 closes it.

## Hosted measurements

| Run | Head | be32 | asan | Slowest build leg |
|---|---|---|---|---|
| [39350776083](https://github.com/szTheory/muhtari/actions/runs/39350776083) (`simple` only, tracer) | 90d2a01 | 32 s | 17 s | windows-arm64 66 s |
| [39351079824](https://github.com/szTheory/muhtari/actions/runs/39351079824) (all 29 samples) | 3ff7e88 | 101 s (`basic` 180 s of CPU over 30 tests, 62.6 s of ctest wall time) | 36 s (`basic` 42 s of CPU) | macos-x64 101 s |
| [39351445117](https://github.com/szTheory/muhtari/actions/runs/39351445117) (docs) | 084fa7f | 88 s | 31 s | macos-x64 79 s |

**be32 and asan decision:** be32 takes 101 s, under the 5-minute rule (D-20, DEC.46), and asan takes 36 s, well inside its limit. Both lanes keep `basic.*` per pull request. No `be32.txt` or `asan.txt` was created, so plan 03-17 has no nightly be32 `-L basic` job to add. Every `timeout-minutes` stays at its 5-minute floor, above the measured times, so `ci.yml` changed only in a comment.

## Task Commits

1. **Task 1: chain driver, pin check with revision guard, basic.simple, probe PR** - `6e7d25e` (test), plus `90d2a01` (fix, deviation 1)
2. **Task 2: the other linear samples** - `3ff7e88` (test)
3. **Task 3: measure the hosted cost, document the basic label** - `084fa7f` (docs)

## Verification

| Command | Exit |
|---|---|
| `cmake --workflow --preset ci` (499 tests) | 0 |
| `ctest --test-dir build/ci -L '^(built\|basic)$' --output-on-failure` (64 tests) | 0 |
| `ctest --test-dir build/ci -R '^docs\.'` | 0 |
| `scripts/hygiene.sh --tree` | 0 |
| `PATH=<llvm 21>:$PATH cmake --workflow --preset hygiene` (24 tests) | 0 |
| `gh run watch 39350776083 --exit-status` (tracer) | 0 |
| `gh run watch 39351079824 --exit-status` (Task 2) | 0 |
| `gh run watch 39351445117 --exit-status` (Task 3) | 0 |

Tracer gate (Task 1): `human_verify_mode` is end-of-phase and the verify step is automated only, so the gate re-ran the verify. The first hosted run, 39350538771, failed on both Windows legs (deviation 1). After the fix, run 39350776083 passed on every leg, and only then did Task 2 begin.

## Files Created/Modified

- `tests/built/chain.c` - muhtari-test-chain
- `tests/built/frames.cmake` - pin check, revision guard, WRITE mode
- `tests/built/tests.cmake` - the per-row foreach, the chain target, the generated revision self test
- `tests/built/expected.txt` - the revision line, 29 rows, the SuperGame, banksets and pokey comments
- `tests/inventory/ci.txt`, `tests/inventory/wasm32.txt` - 29 built and 30 basic lines; `built.samples` became `built.simple`
- `src/maria.c` - MSTAT read without a C4310 cast
- `.planning/preparation/CONFORMANCE.md` - the dated pokey note in section 2(a)
- `docs/emulation-tests.md`, `docs/testing.md` - the basic section and label row; `built.simple` in place of `built.samples`
- `.github/workflows/ci.yml`, `tests/cart/tests.cmake`, `tests/runner/tests.cmake` - comments renamed to `built.<name>` and `built.simple`

## Decisions Made

See `key-decisions`. One point needs explaining. D-15 says a WRITE at a higher revision "rewrites the hashes and N", but each frames.cmake call checks a single row. So the first such call resets every other row to `-`. Those rows then fail their checks until their own WRITE runs. A re-pin at a new revision cannot leave old-revision pins standing.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 3 - Blocking] MSVC C4310 in src/maria.c broke both Windows legs**
- **Found during:** Task 1's hosted run 39350538771, the first push of phase 3
- **Issue:** `(uint8_t)~MARIA_VBLANK` from 03-02 truncates a constant, and MSVC `/W4 /WX` stops on it. The tracer's hosted verify could not pass.
- **Fix:** mask with `(MARIA_VBLANK ^ 0xFFu)`, with a comment.
- **Files modified:** src/maria.c
- **Verification:** run 39350776083 passed on every leg, both Windows legs included
- **Committed in:** 90d2a01

**2. [Rule 3 - Blocking] The revision self test runs through a generated script**
- **Found during:** Task 1
- **Issue:** The plan's expect_fail command has to carry `-DEMULATOR=<list>` inside `-DCMD=<list>`. That nests semicolons two levels deep, and on wasm32 and be32 it would split the emulator into stray arguments.
- **Fix:** `file(GENERATE)` writes `revision_self_test.cmake`. It sets CHAIN, EMULATOR, ROM, NAME and EXPECTED (a revision-0 copy) and includes frames.cmake, so the CMD is just `cmake -P <script>`. It is still expect_fail with CODE 1 and REGEX "behaviour revision".
- **Committed in:** 6e7d25e

**3. [Rule 1 - Bug] Stale built.samples names**
- **Found during:** Task 2
- **Issue:** After the rename, comments in ci.yml and the cart and runner tests, plus two docs, still named `built.samples`.
- **Fix:** renamed them to `built.<name>` or `built.simple`.
- **Committed in:** 3ff7e88, 084fa7f

---

**Total deviations:** 3 auto-fixed (2 blocking, 1 bug).
**Impact on plan:** None changes the plan's design. Fix 1 repairs a latent phase-3 bug that the hosted run exposed.

## Issues Encountered

- The PR body is exactly the text the plan specified, with no attribution line, because the PR is a probe and will never be merged.

## Known Stubs

None.

## Next Phase Readiness

- Plan 03-08 adds the five SuperGame rows. The foreach registers them, and both inventories need their `built.*` and `basic.*` lines.
- Plan 03-14's `banksets` gets an `unsupported` ledger row.
- `pokey`, `rmtromdemo` and the other POKEY-at-$0450 samples will move their pins when phase 4 adds POKEY. Released pins need a revision bump; pins not yet on main follow the unreleased route.
- Still open from 03-03: a DLI on zone 0's entry raises no NMI. A valid BIOS is copied but fast-boots until 03-13.

## Self-Check: PASSED
