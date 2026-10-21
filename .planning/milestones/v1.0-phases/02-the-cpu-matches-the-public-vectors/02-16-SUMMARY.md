---
phase: 02-the-cpu-matches-the-public-vectors
plan: 16
subsystem: infra
tags: [docs, ci, timeouts, github-actions, ctest, release-please]

requires:
  - phase: 02-the-cpu-matches-the-public-vectors
    provides: "plans 01-15: the CPU, bus, RIOT, probe, loader, fuzz target, built sample, firmware scans and their provisional timeouts"
provides:
  - "docs/testing.md sections for the vectors, vectors-full, sally and riot labels, the fixture manifest, the firmware scans, the MUHTARI_TESTS=OFF users, the nightly path union and the vector pin bump"
  - "A7800-SYSTEM sections 3-6 and the cartridge notes' section 1 as pointers into docs/hardware/ with their choice lines; every D-30 reference fix, dated"
  - "Measured timeout-minutes for every ci.yml and nightly.yml job, and measured CTest TIMEOUTs for the phase's vector, decimal, full-set, built-sample and fuzz tests"
  - "workflow.test_gate_timeout 4"
  - "Probe PR #9 closed unmerged; the ship title checked"
affects: [ship, phase-03]

actuals:
  tokens: 18744
  tasks: 3
  commits: 2
plan_head_before: f92368026d17d6a71541d972b5d10924c1f56e23
plan_head_after: 3fe77282d8546c68b077034478effc07915815c9

tech-stack:
  added: []
  patterns:
    - "A CTest TIMEOUT is twice the slowest per-test time over the lane's MUHTARI_TIMEOUT_SCALE, rounded up to whole seconds, with the run ID in a comment"
    - "A job that fetches or installs anything takes the 5-minute network-bound floor"

key-files:
  created: []
  modified:
    - docs/testing.md
    - ASSET_POLICY.md
    - .planning/preparation/A7800-SYSTEM.md
    - .planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md
    - .planning/preparation/ARCHITECTURE.md
    - .planning/preparation/ENGINEERING.md
    - .planning/preparation/CONFORMANCE.md
    - .planning/phases/01-a-test-frame-in-retroarch/01-CONTEXT.md
    - .github/workflows/ci.yml
    - .github/workflows/nightly.yml
    - tests/cpu/tests.cmake
    - tests/cpu/full.cmake
    - tests/built/tests.cmake
    - tests/fuzz/tests.cmake
    - tests/hygiene/tests.cmake
    - tests/release/tests.cmake
    - .planning/config.json

key-decisions:
  - "No be32 flip: the hosted be32 job's slowest of 13 runs is 45 s (run 39198577488), under the five-minute per-PR budget, so hash-equality keeps -DCOUNT=7"
  - "Timeouts measured from 13 ci.yml runs since the sample build (run 39193868744 on) and 10 to 17 nightly.yml runs; the build legs, asan, be32 and perf now fetch or install, so they take the network-bound floor"
  - "nightly vectors-full timeout 8 min: the fetch TIMEOUT 220 s (4 min) plus 2 x 118 s, slowest of 15 runs (run 39197673159)"
  - "workflow.test_gate_timeout is 4: twice the slowest of four clean local cmake --workflow --preset ci runs, 1.82 s"
  - "CTest TIMEOUTs are twice the slowest time divided by the lane's scale; the firmware scans stay provisional with 9 samples"
  - "The phase's squash title is feat: the CPU matches the public 6502 vectors, with the 7800 bus, RIOT and cartridge probing (check-title.sh exit 0), so release-please cuts 0.2.0"

patterns-established:
  - "Preparation sections that graduate keep their numbers, a pointer, the choice lines and the evidence the public page does not repeat"

requirements-completed: [CPU-01, CPU-02, CPU-03, CPU-04, CPU-05]

coverage:
  - id: C1
    description: "ctest -L vectors matches every opcode of the committed sample on final state and every bus cycle, decimal included, on every pull request; ctest -L vectors-full matches the full set at its pin nightly"
    requirement: CPU-01
    verification:
      - kind: integration
        ref: "ci.yml run 39199084217 (cpu.vectors.*, cpu.decimal.* on every leg)"
        status: pass
      - kind: integration
        ref: "nightly.yml run 39199084192 (vectors-full job, 258 tests)"
        status: pass
    human_judgment: false
  - id: C1b
    description: "The full SingleStepTests 6502/v1 set, fetched at its pinned commit, matches on every test"
    requirement: CPU-02
    verification:
      - kind: e2e
        ref: "nightly.yml run 39199084192 job vectors-full; 15 green runs, slowest run 39197673159"
        status: pass
    human_judgment: false
  - id: C2
    description: "ctest -L sally: 4- and 6-tick cycles, the two-write HALT gate stopping reads and writes, RDY holding only reads, an NMI in the last cycle taken after the next instruction"
    requirement: CPU-03
    verification:
      - kind: unit
        ref: "ci.yml run 39199084217 (bus.cycle.*, bus.halt.*, bus.rdy.*, bus.nmi.*, cpu.nmi.*)"
        status: pass
    human_judgment: false
  - id: C3
    description: "ctest -L riot: the timer through HALT and WSYNC, its flag, SWCHA and SWCHB with direction registers and power-on values, RIOT RAM, INPTCTRL locking and open-bus reads"
    requirement: CPU-04
    verification:
      - kind: unit
        ref: "ci.yml run 39199084217 (riot.*, bus.inptctrl.lock, bus.open_bus.*)"
        status: pass
    human_judgment: false
  - id: C4
    description: "muhtari-run --probe prints the fields and four hashes, finds a stripped image by the hash list, gives each refusal its status and message; the counting-allocator test and fuzz.regress.loader pass everywhere and the loader fuzz target runs nightly"
    requirement: CPU-05
    verification:
      - kind: integration
        ref: "ci.yml run 39199084217 (runner.probe.*, cart.*, fuzz.regress.loader on every leg)"
        status: pass
      - kind: integration
        ref: "nightly.yml run 39199084192 job loader-fuzz"
        status: pass
    human_judgment: false
  - id: C5
    description: "Merging the phase's pull request publishes 0.2.0 through the phase 1 pipeline with no manual step; confirmed at /gsd-ship, not in UAT"
    verification:
      - kind: other
        ref: "release.yml rehearsal run 39199084187; printf '' | scripts/check-title.sh 'feat: the CPU matches the public 6502 vectors, with the 7800 bus, RIOT and cartridge probing' exit 0; bump-minor-pre-major true"
        status: pass
      - kind: other
        ref: "PR #10 merged as ce8a617; release PR #11 checks green (CI run 39321341786); release.yml run 39321739023 success on d698364; gh release view v0.2.0 published, not draft, 19 assets"
        status: pass
    human_judgment: false
  - id: D1
    description: "docs/testing.md, the preparation shrink and the D-30 reference fixes pass the docs gates"
    verification:
      - kind: integration
        ref: "cmake --workflow --preset ci && ctest --test-dir build/ci -R '^(docs\\.(budgets|api_names)|runner\\.exit\\.table|release\\.single_version_source)' && cmake --workflow --preset hygiene"
        status: pass
    human_judgment: false
  - id: D2
    description: "Every timeout-minutes carries a run ID or provisional, and the head commit is green on ci.yml, nightly.yml and release.yml"
    verification:
      - kind: integration
        ref: "hygiene.timeout_comments in ci.yml run 39199084217; nightly.yml run 39199084192; release.yml run 39199084187"
        status: pass
    human_judgment: false
  - id: D3
    description: "workflow.test_gate_timeout equals twice the measured local gate time"
    verification:
      - kind: other
        ref: "jq .workflow.test_gate_timeout .planning/config.json prints 4; jq .mode prints interactive"
        status: pass
    human_judgment: false
  - id: D4
    description: "The draft probe PR is closed, never merged"
    verification:
      - kind: other
        ref: "gh pr view 9 --json state -q .state prints CLOSED"
        status: pass
    human_judgment: false

duration: 33min
completed: 2026-10-19
status: complete
---

# Phase 2 Plan 16: Docs, measured timeouts and the ship title Summary

**The maintainer docs and preparation notes now match what phase 2 built, every CI and phase-2 CTest limit is measured from 10 to 17 hosted runs, be32 stays per pull request at 45 s, the test gate is 4 s, and probe PR #9 is closed with the 0.2.0 ship title checked.**

## Performance

- **Duration:** 33 min
- **Started:** 2026-10-19T03:58:41Z
- **Completed:** 2026-10-19T04:31:00Z
- **Tasks:** 3
- **Files modified:** 17

## Accomplishments

- `docs/testing.md` (35,580 B, under its 40,960 B budget) documents the `vectors`, `vectors-full`, `sally` and
  `riot` labels, three new negative controls, the fixture manifest and `policy.rom_hash_coverage`, both firmware
  scans, the `MUHTARI_TESTS=OFF` users, the nightly path union and the vector pin bump.
- A7800-SYSTEM sections 3 to 6 and A7800-CARTRIDGES-AND-PERIPHERALS section 1 point into `docs/hardware/*.md` and
  keep their choice lines (C4 to C9, the parsing, hash and probe recommendations), plus the facts that wait for
  MARIA, the boards and boot.
- D-30's fixes, each with its file's date: ARCHITECTURE section 3 and [SYS.31] cite szTheory/nesturbator
  @9f9386c661f6; ENGINEERING section 3 names target `loader`, `fuzz.regress.loader` and `-max_len=1048705`;
  CONFORMANCE sections 1 and 8 drop Klaus Dormann's test with the reason; ASSET_POLICY section 3 names 7800basic tag
  v0.41 `03aebb85206f53046425c19a236f65ae80d9f498` with its bundled `dasm.wasm`; phase 1 D-24 points at the
  `sst-6502-v1` row.
- docs/embedding.md and docs/runner.md checked against the code: every header name appears in embedding.md, and the
  `--probe` line order, refusal messages and exit table match `runner/main.c`, `host/status_text.c` and
  `runner/exit_status.h`. No drift found.

### Timeouts (ci.yml: 13 successful runs from run 39193868744 on)

| Job | Samples | Slowest | Run | Set |
|---|---|---|---|---|
| changes | 13 | 8 s | 39195625395 | 5 (network-bound floor) |
| hygiene | 13 | 27 s | 39198577488 | 5 (network-bound floor) |
| title | 29 (phase 1) | 11 s | 38955531324 | 5, unchanged: 7 new samples only, same job |
| build x6 | 13 | 84 s (windows-arm64) | 39197902638 | 5 (network-bound floor; was 3, provisional) |
| asan | 13 | 35 s | 39198234113 | 5 (network-bound floor; was 3, provisional) |
| nofp x2 | 13 | 21 s | 39198577488 | 1 (2 x 21 s) |
| be32 | 13 | 45 s | 39198577488 | 5 (network-bound floor) |
| hash-equality | 13 | 7 s | 39198407537 | 5 (roll-up floor) |
| wasm32 | 13 | 34 s | 39197902638 | 5 (network-bound floor) |
| retroarch | 13 | 29 s | 39198079849 | 5 (network-bound floor) |
| perf | 13 | 29 s | 39198079849 | 5 (network-bound floor; was 20, provisional) |
| CI required | 13 | 4 s | 39197713488 | 5 (roll-up floor) |

### Timeouts (nightly.yml)

| Job | Samples | Slowest | Run | Set |
|---|---|---|---|---|
| bench x3 | 17 | 108 s (ubuntu-26.04) | 39197430512 | 5 (network-bound floor; was 20) |
| bench-wasm32 | 17 | 122 s | 39197611548 | 5 (2 x 122 s; was 30) |
| vectors-full | 15 | 118 s | 39197673159 | 8 = fetch TIMEOUT 220 s (4 min) + 2 x 118 s (was 30) |
| loader-fuzz | 15 | 90 s | 39195320935 | 5 (network-bound floor; was 10) |
| report | 10 | 6 s | 39197977418 | 5 (roll-up floor) |

### CTest TIMEOUTs (twice the slowest time over the lane's scale; be32 11, asan and wasm32 3)

| Test | Samples | Slowest (normalised) | Run | Was, now |
|---|---|---|---|---|
| `cpu.vectors.<op>` | 13 | 0.97 s, Windows arm64 | 39197902638 | 10, 2 |
| `cpu.decimal.clark` | 13 | 1.40 s, macOS x64 (be32 6.2 s / 11) | 39198577488 | 60, 3 |
| `cpu.decimal.clark.self_test` | 13 | 0.03 s | 39198079849 | 60, 1 |
| `cpu.vectors-full.fetch` | 15 | 109.59 s | 39197673159 | 600, 220 |
| `cpu.vectors-full.sample_match` | 15 | 2.20 s | 39197673159 | 120, 5 |
| `cpu.vectors-full.<op>` | 15 | 0.08 s | 39195625393 | 60, 1 |
| `built.samples` | 13 | 2.44 s, macOS x64 | 39195320937 | 60, 5 |
| `built.samples.self_test` | 13 | 1.68 s, Windows arm64 | 39197737154 | 60, 4 |
| `fuzz.loader` | 15 | 61.15 s | 39198059321 | 120, 123 |
| `hygiene.firmware_scan`, `release.firmware_scan` and self tests | 9 | | | provisional, 9 samples |

- **be32:** slowest 45 s of 13 runs (run 39198577488) against the five-minute per-PR budget: no flip, so DEC.46's
  nightly move is not applied and `hash-equality` keeps seven files.
- **Local gate:** four clean `cmake --workflow --preset ci` runs took 1.82, 1.69, 1.73 and 1.70 s (incremental
  0.67 to 0.69 s); `workflow.test_gate_timeout` = 4 (2 x 1.82 s, rounded up). `git diff .planning/config.json`
  showed only that value; `jq .mode` still prints `"interactive"`.

### Close-out record

- Probe PR #9 (`ci: probe the phase workflows`), head 3fe77282d854, closed unmerged with
  `gh pr close 9 --comment "Probe for phase 2 done; the phase ships through /gsd-ship."`; `gh pr view 9 --json state`
  prints `CLOSED`; no open pull request remains on the branch.
- Final runs on head 3fe77282d854, all green:
  - ci.yml https://github.com/szTheory/muhtari/actions/runs/39199084217
  - nightly.yml https://github.com/szTheory/muhtari/actions/runs/39199084192
  - release.yml https://github.com/szTheory/muhtari/actions/runs/39199084187
- `printf '' | scripts/check-title.sh "feat: the CPU matches the public 6502 vectors, with the 7800 bus, RIOT and
  cartridge probing"` exit 0. `jq '.packages["."]["bump-minor-pre-major"]' release-please-config.json` prints
  `true`, and `version.txt` is 0.1.0, so a `feat:` squash cuts 0.2.0.
- Gate code against `src/`: `src/*.c` and `*.h` are 5,513 lines, 3,466 without the generated `src/cartdb.c`.
  The repository checks (`scripts/hygiene.sh`, `check-title.sh`, `classify-changes.sh`, `apply-repo-settings.sh` and
  the 16 policy and hygiene scripts in `tests/cmake/`) are 1,332 lines, 24 % or 38 %; all of `tests/cmake/` and
  `scripts/` are 2,586 lines. AGENTS rule 7's tenth binds from phase 3, and whether `src/cartdb.c` counts is phase
  3's question (research R6).

## Task Commits

1. **Task 1: docs, the preparation shrink and the reference fixes** - `e36a603` (docs)
2. **Task 2: measured timeouts, the be32 budget and the test-gate timeout** - `3fe7728` (ci)
3. **Task 3: close the probe PR and record the ship title** - no tracked change; its record is this SUMMARY

## Files Created/Modified

- `docs/testing.md` - the phase's labels, controls, fixtures, scans, nightly union and pin bump
- `ASSET_POLICY.md` - the 7800basic v0.41 pin with its bundled dasm.wasm
- `.planning/preparation/A7800-SYSTEM.md`, `A7800-CARTRIDGES-AND-PERIPHERALS.md` - pointer sections
- `.planning/preparation/ARCHITECTURE.md`, `ENGINEERING.md`, `CONFORMANCE.md` - dated reference fixes
- `.planning/phases/01-a-test-frame-in-retroarch/01-CONTEXT.md` - D-24's vector archive is the sst-6502-v1 row
- `.github/workflows/ci.yml`, `nightly.yml` - measured `timeout-minutes`
- `tests/cpu/tests.cmake`, `tests/cpu/full.cmake`, `tests/built/tests.cmake`, `tests/fuzz/tests.cmake` - measured
  TIMEOUTs
- `tests/hygiene/tests.cmake`, `tests/release/tests.cmake` - firmware-scan TIMEOUTs marked with their sample count
- `.planning/config.json` - `workflow.test_gate_timeout` 4

## Decisions Made

See key-decisions. The workflow rule from phase 1 was applied as written; the only judgement was treating a job
that fetches pins or installs packages as network-bound, which the phase 1 comments for be32 and wasm32 already did.

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 2 - Missing critical] Firmware-scan TIMEOUTs given their sample count**
- **Found during:** Task 2
- **Issue:** Plan 15's `hygiene.firmware_scan`, `release.firmware_scan` and their self tests are provisional
  TIMEOUTs this phase added, outside the plan's file list, and their comments gave no sample count, which the
  truth requires.
- **Fix:** each comment now reads `provisional, 9 samples` (9 successful runs since plan 15; fewer than 10, so not
  measured).
- **Files modified:** tests/hygiene/tests.cmake, tests/release/tests.cmake
- **Committed in:** 3fe7728

**2. [Rule 2 - Missing critical] built.samples.self_test measured beside built.samples**
- **Found during:** Task 2
- **Issue:** It shares `built.samples`' unmeasured TIMEOUT 60 from this phase.
- **Fix:** TIMEOUT 4, 2 x 1.68 s (run 39197737154).
- **Committed in:** 3fe7728

**3. [Plan wording] `title` keeps its phase 1 measurement**
- **Found during:** Task 2
- **Issue:** `title` runs on pull requests only, so the dispatched runs gave it 7 new samples, under 10.
- **Fix:** its job is unchanged since phase 1, whose comment measures 29 runs (slowest 11 s); value and comment
  stay.

---

**Total deviations:** 3 (2 Rule 2, 1 wording)
**Impact on plan:** None on scope. Every limit is either measured with a run ID and count or provisional with its
count.

## Issues Encountered

- `gh api .../jobs/<id>/logs` refuses to print a log with terminal escapes unless given `--allow-escape-sequences`;
  the per-test times came from the job logs that way, since CI uploads no JUnit file.
- Dispatched `ci.yml` runs on one commit share a concurrency group, so the six extra runs were dispatched one at a
  time; the ten extra nightly runs ran in parallel.

## User Setup Required

None - no external service configuration required.

## Next Phase Readiness

Every plan in phase 2 has a SUMMARY. The branch is green on all three workflows and has no open pull request; the
next step is `/gsd-ship 2`, whose pull request takes the title above, and whose merge (the owner's step) publishes
0.2.0 (criterion 5).

---
*Phase: 02-the-cpu-matches-the-public-vectors*
*Completed: 2026-10-19*

## Self-Check: PASSED
