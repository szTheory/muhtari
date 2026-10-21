---
phase: 03-games-play-in-retroarch
plan: 19
subsystem: infra
tags: [ci, timeouts, ctest, gate-code, docs, todos, ship-title]

requires:
  - phase: 03-games-play-in-retroarch
    provides: "plans 01-18: every test and job this phase added, each with a provisional limit marked for this plan"
provides:
  - "Measured CTest TIMEOUTs for every test this phase added, each with its run in a comment; no 'provisional, phase 3' line is left under tests/"
  - "Measured timeout-minutes for every ci.yml and nightly.yml job: be32 7, prop 8, vectors-full 9, fuzz 5; library 5 and still provisional on 7 runs"
  - "The rule 7 gate-code tally in SEED-007, with perf_compare.cmake cut from 126 to 95 lines"
  - "docs/testing.md label rows linking each emulation label to its section; the re-review todo closed in part; a POKEY re-pin todo"
  - "Probe PR #12 closed unmerged; the ship title checked"
affects: [ship, phase-04, milestone tune-up]

actuals:
  tokens: 14030
  tasks: 3
  commits: 3
plan_head_before: 3123cf6a1466cea68d6b8c3e32142725b0b6dca8
plan_head_after: b727c42c2160803d2f7db278e128699840b874f3

tech-stack:
  added: []
  patterns:
    - "A CTest TIMEOUT comment reads '2 x <s> s, <leg>, slowest of <n> runs, run <id>', with <s> the slowest time over the lane's scale"
    - "A gate that needs today's date reads string(TIMESTAMP) in UTC and honours SOURCE_DATE_EPOCH, so its tests fix the date by environment"

key-files:
  created:
    - .planning/todos/pending/2026-10-19-re-pin-the-pokey-samples-when-pokey-lands.md
  modified:
    - tests/maria/tests.cmake
    - tests/frame/tests.cmake
    - tests/built/tests.cmake
    - tests/diag/tests.cmake
    - tests/input/tests.cmake
    - tests/boot/tests.cmake
    - tests/compat/tests.cmake
    - tests/compat/library.cmake
    - tests/prop/tests.cmake
    - tests/fuzz/tests.cmake
    - tests/runner/tests.cmake
    - tests/libretro/tests.cmake
    - tests/core/tests.cmake
    - tests/cmake/perf_compare.cmake
    - tests/cmake/firmware_scan.cmake
    - tests/perf/tests.cmake
    - .github/workflows/ci.yml
    - .github/workflows/nightly.yml
    - docs/testing.md
    - .planning/seeds/SEED-007-milestone-tune-up.md
    - .planning/todos/pending/2026-10-19-close-the-phase-2-re-review-findings-when-maria-and-the-run-loop-land.md

key-decisions:
  - "Per-test times come from the ctest lines in the hosted job logs, since CI uploads no JUnit file. They span 31 successful ci.yml runs (26 pull request, 4 dispatched, 1 on 3123cf6) and 11 nightly fuzz and prop runs (7 dispatched)"
  - "be32 stays per pull request: its slowest of 31 runs is 184 s, under the five-minute lane budget, so hash-equality keeps seven files. Its job limit is 7 minutes (2 x 184 s)"
  - "The prop TIMEOUT is 39 from the nightly job (190.28 s at scale 10). That gives each nightly property 390 s and covers the pull-request runs (6.57 s at most)"
  - "library keeps 'provisional' on its timeout-minutes with 7 runs. Its value drops to the 5-minute network-bound floor (slowest 34 s)"
  - "Gate-code budget: narrow gate code is 1,367 lines against 7,415 src/ lines (18.4 %, or 25.5 % without src/cartdb.c). The shortfall against the tenth is the owner's call at ship"
  - "The ship title 'feat: games play in RetroArch with MARIA, cartridges, boot and input' passes scripts/check-title.sh (exit 0). With version.txt 0.2.0 and bump-minor-pre-major true, the squash cuts 0.3.0"

patterns-established:
  - "Before a close-out plan measures limits, dispatch workflow runs until each limit has 10 samples: nightly runs in parallel, ci.yml runs one at a time"

requirements-completed: [GAME-01, GAME-02, GAME-03, GAME-04, GAME-05, GAME-06]

coverage:
  - id: D1
    description: "Every CTest TIMEOUT this phase added is twice the slowest per-test time over the lane's scale, rounded up, with the run in its comment, and no 'provisional, phase 3' line is left under tests/"
    requirement: GAME-01
    verification:
      - kind: other
        ref: "git grep -n 'provisional, phase 3' -- tests (no output, exit 1); git grep -c 'slowest of' -- tests/maria/tests.cmake tests/frame/tests.cmake tests/diag/tests.cmake (1, 1, 8)"
        status: pass
      - kind: integration
        ref: "cmake --workflow --preset ci (exit 0, 587 tests and 6 release tests)"
        status: pass
      - kind: e2e
        ref: "https://github.com/szTheory/muhtari/actions/runs/39383104719"
        status: pass
    human_judgment: false
  - id: D2
    description: "Every ci.yml and nightly.yml timeout-minutes names its run and sample count, or says provisional; be32 has 7 minutes, prop 8, vectors-full 9, and the nightly fuzz and prop jobs are measured on 11 runs"
    requirement: GAME-02
    verification:
      - kind: unit
        ref: "cmake -DDIR=.github/workflows -P tests/cmake/timeouts.cmake (exit 0, 26 timeouts); hygiene.timeout_comments in cmake --workflow --preset hygiene (exit 0, 24 tests)"
        status: pass
      - kind: e2e
        ref: "https://github.com/szTheory/muhtari/actions/runs/39383824826"
        status: pass
    human_judgment: false
  - id: D3
    description: "The rule 7 gate-code tally is in SEED-007 with its commands, definition and numbers before and after; perf_compare.cmake is trimmed with every perf.gate test passing, and the shortfall is an owner scoping item for ship"
    requirement: GAME-06
    verification:
      - kind: unit
        ref: "ctest --test-dir build/ci -R '^(release\\.|policy\\.|perf\\.)' --output-on-failure (exit 0, 21 tests); ctest --test-dir build/ci -R self_test (exit 0, 34 tests)"
        status: pass
      - kind: other
        ref: "grep -n 'Gate-code budget after phase 3' .planning/seeds/SEED-007-milestone-tune-up.md (one line, 47); SOURCE_DATE_EPOCH=1767312000 cmake ... -P tests/cmake/perf_compare.cmake: an allow line 60 days on passes, 61 days on fails, a two-digit year exits 1 as malformed"
        status: pass
    human_judgment: false
  - id: D4
    description: "docs/testing.md has a labels-table row linking its docs/emulation-tests.md section for each of maria, frame, basic, diag, input, bios, title, prop and library; prop and library link to CI; every page is within budget"
    requirement: GAME-03
    verification:
      - kind: unit
        ref: "grep -cE '^\\| `(maria|frame|basic|diag|input|bios|title|prop|library)` \\|.*emulation-tests\\.md' docs/testing.md prints 9; grep -ciE '^## (maria|frame|basic|diag|input|bios|title|prop|fuzz|ledger)' docs/emulation-tests.md prints 10; ctest --test-dir build/ci -R '^docs\\.' (exit 0, 3 tests)"
        status: pass
    human_judgment: false
  - id: D5
    description: "The re-review todo marks WR-05, IN-04 and IN-01 closed by plans 03-03 and 03-02 and keeps WR-02 and IN-05 as seeds; a new todo re-pins the POKEY samples with a behaviour revision bump when POKEY lands"
    requirement: GAME-04
    verification:
      - kind: other
        ref: "grep -c closed .planning/todos/pending/2026-10-19-close-the-phase-2-re-review-findings-when-maria-and-the-run-loop-land.md prints 3; grep -c 'behaviour revision' .planning/todos/pending/2026-10-19-re-pin-the-pokey-samples-when-pokey-lands.md prints 2"
        status: pass
    human_judgment: false
  - id: D6
    description: "The probe pull request is closed unmerged, the ship title passes the title check, and the head commit is green on every hosted ci.yml and nightly.yml job"
    requirement: GAME-05
    verification:
      - kind: other
        ref: "gh pr view 12 --json state,mergedAt prints CLOSED, merged null; gh pr list --state open --search 'ci: probe the phase workflows' --json number -q length prints 0; printf '' | scripts/check-title.sh 'feat: games play in RetroArch with MARIA, cartridges, boot and input' exit 0"
        status: pass
      - kind: e2e
        ref: "https://github.com/szTheory/muhtari/actions/runs/39383824898"
        status: pass
    human_judgment: false

duration: 35min
completed: 2026-10-20
status: complete
---

# Phase 3 Plan 19: Measured limits, the gate-code tally, docs and the probe Summary

**Every test and job phase 3 added now has a limit measured from 31 ci.yml runs and 11 nightly runs. be32 takes 7 minutes and the nightly prop job 8. The gate-code budget is tallied in SEED-007 at 18.4 % of `src/`, and `perf_compare.cmake` lost 31 lines with nothing lost. The labels table links every emulation label to its section. Probe PR #12 is closed, and the 0.3.0 ship title passes its check.**

## Performance

- **Duration:** 35 min, including about 20 min of dispatched and watched hosted runs
- **Started:** 2026-10-20T06:30:31Z
- **Completed:** 2026-10-20T07:06:00Z
- **Tasks:** 3
- **Files modified:** 22

## Accomplishments

### Where the times came from

CI uploads no JUnit file, so the per-test times come from the `ctest` lines in the job logs. I downloaded them with
`gh api --allow-escape-sequences repos/szTheory/muhtari/actions/jobs/<id>/logs` for 401 successful test-running
jobs. Each time was divided by its lane's `MUHTARI_TIMEOUT_SCALE`: be32 11, asan and wasm32 3, the nightly prop job
10, and 1 everywhere else. The slowest value per test group was taken.

- **ci.yml, 31 successful runs.** 26 pull-request runs on PR #12, from 39350776083 to 39380651205, plus four
  dispatched runs: 39381266556, 39381705491, 39382016958 and 39382343202.
- **nightly.yml.** The fuzz and prop jobs have 11 successful runs each: 39377950042, 39379798388, 39380163971,
  39380651163, and seven dispatched runs, 39381231299 to 39381246541. The other nightly jobs have 39 runs.

### CTest TIMEOUTs (twice the slowest normalised time, rounded up, at least 1)

| Tests | Runs | Slowest (normalised) | Leg | Run | Was, now |
|---|---|---|---|---|---|
| `maria.dma_line.*` | 31 | 0.20 s | macOS x64 | 39351079824 | 10, 1 |
| `frame.<case>` | 31 | 0.12 s | macOS x64 | 39375738182 | 10, 1 |
| `frame.sample_renders` | 31 | 0.67 s | macOS x64 | 39351445117 | 20, 2 |
| `input.<case>` | 18 | 0.20 s | macOS x64 | 39382016958 | 10, 1 |
| `input.cart_files` | 17 | 0.14 s | Windows arm64 | 39368351340 | 10, 1 |
| `bios.handoff_simple`, `splash_skip`, `rejects_cart` | 14 | 0.40 s | macOS x64 | 39382016958 | 30, 1 |
| `bios.k_window`, `k_first_fetch` | 13 | 0.10 s | macOS x64 | 39373524239 | 10, 1 |
| `core.load.*`, `set_input.*`, `get_memory.*`, `counters.*` | 31 | 0.06 s | Windows arm64 | 39375738182 | 10, 1 |
| `prop.*` | 19 | 19.03 s (190.28 s / 10) | nightly prop | 39381236115 | 60, 39 |
| `title.simple.fast`, `.bios` | 12 | 0.60 s | macOS x64 | 39375738182 | 60, 2 |
| `title.pin.self_test` | 12 | 0.16 s | macOS x64 | 39375738182 | 30, 1 |
| `built.<name>`, all but simple | 41 | 7.73 s | macOS x64 | 39357152918 | 10, 16 |
| `basic.<name>` | 31 | 4.67 s | macOS x64 | 39362291672 | 30, 10 |
| `basic.revision.self_test` | 31 | 3.11 s | macOS x64 | 39356734460 | 30, 7 |
| `built.openbios` | 16 | 0.40 s | Windows arm64 | 39381266556 | 60, 1 |
| `built.openbios.self_test` | 16 | 0.11 s | Windows arm64 | 39375738182 | 10, 1 |
| `basic.coverage` and its self test | 19 | 25.77 s | macOS x64 | 39362291672 | 60, 52 |
| `diag.dma`, `cpu`, `dli`, `modes`, boards | 19-27 | 0.03-0.10 s | macOS x64, Windows | various | 30, 1 |
| `diag.rebuild` | 26 | 10.66 s | macOS x64 | 39362291672 | 30, 22 |
| `diag.rebuild.self_test` | 26 | 1.09 s | Windows arm64 | 39363678798 | 30, 3 |
| `diag.expected.self_test` | 26 | 0.26 s | macOS x64 | 39354563718 | 30, 1 |
| `fuzz.run` (fuzz lane) | 11 | 61.13 s | fuzz job | 39381240735 | 123, 123 |
| `fuzz.regress.run` (fuzz lane) | 11 | 0.24 s | fuzz job | 39377950042 | 120, 1 |
| `fuzz.regress.run` (portable) | 8 | 0.09 s (wasm32 0.26 s / 3) | wasm32 | 39377950148 | 60, 1 |
| `libretro.host.input`, `layout`/`devices`/`reset`/`options`, `memory_map`, `libretro.rcheevos` | 10 | 0.02-0.31 s | various | various | 30, 1 |
| `libretro.host.bios` | 10 | 3.53 s | macOS x64 | 39382016958 | 30, 8 |
| `runner.movie.<case>` | 17 | 0.45 s | Windows arm64 | 39373524239 | 10, 1 |
| `runner.movie.boot_bios` group | 13 | 1.10 s | macOS x64 | 39382016958 | 30, 3 |
| `runner.boot_to_play` | 9 | 4.21 s | macOS x64 | 39382016958 | 60, 9 |
| `runner.boot_to_play.<control>` | 9 | 0.13 s | Windows arm64 | 39381705491 | 10, 1 |
| `runner.usage.*`, `runner.load.*` | 17, 31 | 0.16 s, 0.35 s | macOS x64 | 39377950148 | 10, 1 |
| `runner.movie_parse` | 18 | 1.10 s | macOS x64 | 39380651205 | 10, 3 |
| `library.<id>` | 7 | 0.88 s | library job | 39379798527 | 60, 2 |
| `library.results`, `ratchet`, `ratchet.self_test`, `compat_doc` | 6-7 | 0.00-0.01 s | library job | various | 30, 1 |

The phase 2 lines in `tests/hygiene/tests.cmake` and `tests/release/tests.cmake` that read `provisional, 9 samples`
are unchanged.

### Job limits

| Job | Runs | Slowest | Run | Was, now |
|---|---|---|---|---|
| changes | 31 | 10 s | 39364791699 | 5, 5 (network-bound floor) |
| hygiene | 31 | 56 s | 39380164010 | 5, 5 (floor) |
| title | 27 | 11 s | 39350776083 | 5, 5 (floor) |
| build x6 | 31 | 131 s (macOS x64) | 39382016958 | 5, 5 (floor) |
| asan | 31 | 71 s | 39368351340 | 5, 5 (floor) |
| nofp x2 | 31 | 22 s | 39382016958 | 1, 1 (2 x 22 s) |
| **be32** | 31 | **184 s** | 39377950148 | **5, 7** (2 x 184 s) |
| hash-equality | 31 | 9 s | 39359891337 | 5, 5 (roll-up floor) |
| wasm32 | 31 | 63 s | 39377950148 | 5, 5 (floor) |
| library | 7 | 34 s | 39379798527 | 10 provisional, 5 (floor), still provisional |
| retroarch | 31 | 36 s | 39366784570 | 5, 5 (floor) |
| perf | 31 | 58 s | 39382016958 | 5, 5 (floor) |
| CI required | 31 | 4 s | 39382016958 | 5, 5 (roll-up floor) |
| nightly bench x3 | 39 | 130 s | 39381242952 | 5, 5 (floor) |
| nightly bench-wasm32 | 39 | 130 s | 39381242952 | 5, 5 (2 x 130 s) |
| nightly vectors-full | 39 | 139 s | 39377950042 | 8, 9 (fetch 220 s + 2 x 139 s) |
| nightly fuzz | 11 | 85 s | 39381236115 | 8 provisional, 5 (floor) |
| nightly prop | 11 | 211 s | 39381236115 | 20 provisional, 8 (2 x 211 s) |
| nightly report | 10 (phase 2) | 6 s | 39197977418 | 5, unchanged |

### Gate-code budget (rule 7): an owner scoping item for `/gsd-ship 3`

The definition is the narrowest one. It counts the 12 hygiene- and policy-registered `tests/cmake` scripts,
`scripts/{hygiene,check-title,classify-changes,apply-repo-settings}.sh`, `tests/cmake/compat_ratchet.cmake` and
`tools/compat_doc.cmake`. The commands, each exit 0, are `cat src/*.c src/*.h | wc -l`, the same without
`src/cartdb.c`, and `cat <those 18 files> | wc -l`.

| Count | Before | After |
|---|---|---|
| `src/` | 7,415 | 7,415 |
| `src/` without `src/cartdb.c` | 5,368 | 5,368 |
| narrow gate code | 1,369 | 1,367 |
| broad: all `tests/cmake`, `scripts`, `compat_doc` | 2,772 | 2,739 |

The narrow count is 18.4 % of `src/`, or 25.5 % without `src/cartdb.c`. The tenth is 741 or 536 lines, so the
shortfall is 626 or 831 lines.

What I trimmed:
- `perf_compare.cmake`, 126 to 95 lines. It reads the date the way the ratchet does, which removed its day
  arithmetic and `-DTODAY`.
- `firmware_scan.cmake`, 225 to 223 lines. I removed a redundant `find_program` guard.

I left `apply-repo-settings.sh` alone. No CI job runs it, and a change could only be checked against the live
repository.

The owner decides four questions at ship, written in SEED-007:
1. Whether `src/cartdb.c` counts.
2. Whether `apply-repo-settings.sh` counts.
3. Whether `firmware_scan.cmake`'s tracked-tree mode is worth keeping beside `policy.rom_hash_coverage`.
4. Whether to restate the rule as a non-growing ratio or a fixed cap.

No repository-check code was added in this phase beyond the ratchet and the doc generator.

### Close-out record

- Probe PR #12 (`ci: probe the phase workflows`), with head b727c42, was closed unmerged with
  `gh pr close 12 --comment "Probe for phase 3's hosted evidence; closed unmerged."`. `gh pr view 12` prints
  `CLOSED` with `mergedAt` null.
- `printf '' | scripts/check-title.sh "feat: games play in RetroArch with MARIA, cartridges, boot and input"` exited 0.
  `version.txt` is 0.2.0 and `bump-minor-pre-major` is true, so the squash cuts 0.3.0.
- Hosted runs on head b727c42c2160, all green:
  - ci.yml https://github.com/szTheory/muhtari/actions/runs/39383824898 (19 jobs, none failed)
  - nightly.yml https://github.com/szTheory/muhtari/actions/runs/39383824826 (8 jobs, none failed)
- Hosted runs on the Task 1 commit b682b9d, both green: ci.yml
  https://github.com/szTheory/muhtari/actions/runs/39383104719 and nightly.yml run 39383104697.

## Task Commits

1. **Task 1: measured limits on a green run (tracer)**: `b682b9d` (ci). Tracer gate: `human_verify_mode` is
   end-of-phase and the verify is automated only. Its re-run passed: ci 587 + 6 tests, hygiene 24, tree exit 0, and
   hosted run 39383104719 success. No checkpoint was synthesized.
2. **Task 2: the gate-code tally, the safe trims, the owner item**: `4fd3a32` (ci)
3. **Task 3: docs and todos, close the probe, check the ship title**: `b727c42` (docs)

**Plan metadata:** the SUMMARY commit follows.

## Files Created/Modified

- `tests/{maria,frame,built,diag,input,boot,core,prop,fuzz,runner,libretro}/tests.cmake`, `tests/compat/tests.cmake`, `tests/compat/library.cmake`: measured TIMEOUTs
- `.github/workflows/ci.yml`, `nightly.yml`: measured `timeout-minutes`. The prop comment's per-test budget is now 390 s, and perf no longer passes `-DTODAY`
- `tests/cmake/perf_compare.cmake`, `tests/perf/tests.cmake`: the date comes from `string(TIMESTAMP)` and `SOURCE_DATE_EPOCH`
- `tests/cmake/firmware_scan.cmake`: one redundant guard removed
- `.planning/seeds/SEED-007-milestone-tune-up.md`: "Gate-code budget after phase 3 (2026-10-20)"
- `docs/testing.md`: label rows and links; the be32 sentence
- `.planning/todos/pending/...re-review...md`: Phase 3 outcome
- `.planning/todos/pending/2026-10-19-re-pin-the-pokey-samples-when-pokey-lands.md`: new

## Decisions Made

See `key-decisions`. The judgement calls:
- **The job comments measured in phase 2 were refreshed too.** Their times grew this phase, so the old comments no
  longer named the slowest run. Only be32 (5 to 7) and vectors-full (8 to 9) changed value.
- **Comments reach 10 samples where possible.** Seven nightly and four ci.yml runs were dispatched for that. The
  `library` job still has 7 runs and stays provisional.

## Deviations from Plan

### Auto-fixed Issues

**1. [Plan wording] Times from job logs, not JUnit artifacts**
- **Found during:** Task 1
- **Issue:** The plan collects durations from JUnit files with `gh run download`, but CI uploads no JUnit file.
  Phase 2 hit the same problem.
- **Fix:** I parsed the `ctest` result lines in the job logs instead. The per-test values are the same.
- **Commit:** b682b9d

**2. [Rule 2 - Missing critical] be32's limit was below twice its time**
- **Found during:** Task 1
- **Issue:** be32's comment said 45 s with a 5-minute limit. This phase's slowest run is 184 s, so the margin was
  1.6x.
- **Fix:** I set `timeout-minutes: 7`. The other phase 2 job comments were re-measured in the same commit, and
  vectors-full went from 8 to 9.
- **Commit:** b682b9d

**3. [Rule 3 - Blocking] perf_compare's interface change reaches its callers**
- **Found during:** Task 2
- **Issue:** Dropping `-DTODAY` touches `tests/perf/tests.cmake` and the perf step in `ci.yml`, which are outside the
  task's file list.
- **Fix:** The tests set `SOURCE_DATE_EPOCH=1767312000` (2026-01-02 UTC), and the CI step drops the argument.
  Allow lines must now match `YYYY-MM-DD` exactly. An impossible date such as month 13 is never in the 60-day window,
  so it raises no limit. Before, it was an error. Neither case lets a slowdown through.
- **Commit:** 4fd3a32

**4. [Rule 1 - Stale doc] docs/testing.md's be32 sentence**
- **Found during:** Task 3
- **Issue:** It said the hosted be32 job "takes under a minute".
- **Fix:** It now says up to about three minutes, inside the five-minute lane budget.
- **Commit:** b727c42

**5. [Plan wording] One pointer per label**
- **Found during:** Task 3
- **Issue:** The `library` lane row also named docs/emulation-tests.md, so the label grep would count 10.
- **Fix:** The lane row now points to the `library` label row, which holds the link. The count is 9.
- **Commit:** b727c42

**6. [Plan wording] Four POKEY title rows, not five**
- **Found during:** Task 3
- **Issue:** eightysix's ledger row is `boots@120`, because its RNG is seeded from INTIM, so it pins no title.
- **Fix:** The todo names four `title@120` rows and all five frame chains.
- **Commit:** b727c42

**Total deviations:** 6: 1 bug fix in docs, 1 missing-critical fix, 1 blocking fix and 3 plan-wording changes.
**Impact:** none on scope. Every truth holds. `library`'s job limit stays provisional with its sample count.

## Unreleased Pins

No frame or title pin moved, and the core is unchanged.

## Known Stubs

None.

## Threat Flags

None. No new endpoint, permission or input surface. The probe PR was closed, not merged.

## Issues Encountered

- `gh api .../jobs/<id>/logs` needs `--allow-escape-sequences`, as it did in phase 2.
- Right after the close, `gh pr list --search` still returned PR #12 from a stale search index. Within a minute it
  returned 0, and `gh pr list --state open` already printed 0.

## User Setup Required

None.

## Next Phase Readiness

All 19 plans of phase 3 have a SUMMARY. The head commit is green on ci.yml and nightly.yml, and no pull request is
open on the branch. The next command is `/gsd-ship 3`, with the title above. Under rule 6, no owner verification step
comes first. The owner's ship agenda has two items: the gate-code budget question in SEED-007, and the merge.

## Self-Check: PASSED

- FOUND: .planning/todos/pending/2026-10-19-re-pin-the-pokey-samples-when-pokey-lands.md, .planning/seeds/SEED-007-milestone-tune-up.md, docs/testing.md
- FOUND: b682b9d, 4fd3a32 and b727c42 are ancestors of HEAD (`git rev-list --count 3123cf6..HEAD` prints 3)
- Acceptance:
  - `git grep -n 'provisional, phase 3' -- tests` prints nothing;
  - 'slowest of' appears 1, 1 and 8 times in maria, frame and diag;
  - the Gate-code heading is at line 47 of SEED-007, with `wc -l` commands and two-column counts;
  - the label grep prints 9 and the section grep 10;
  - the open probe search prints 0;
  - 'closed' appears 3 times in the re-review todo and 'behaviour revision' twice in the POKEY todo.
- Local: `cmake --workflow --preset ci` exit 0 (587 + 6), `cmake --workflow --preset hygiene` exit 0 (24),
  `ctest -R '^(release\.|policy\.|perf\.)'` exit 0 (21), `ctest -R self_test` exit 0 (34), `ctest -R '^docs\.'` exit 0 (3),
  `scripts/hygiene.sh --tree` exit 0.
