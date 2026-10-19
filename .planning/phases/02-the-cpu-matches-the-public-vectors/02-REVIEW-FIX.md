---
phase: 02-the-cpu-matches-the-public-vectors
fixed_at: 2026-10-19T18:50:52Z
review_path: .planning/phases/02-the-cpu-matches-the-public-vectors/02-REVIEW.md
iteration: 1
findings_in_scope: 4
fixed: 3
skipped: 1
status: partial
---

# Phase 2: Code Review Fix Report

**Fixed at:** 2026-10-19T18:50:52Z
**Source review:** .planning/phases/02-the-cpu-matches-the-public-vectors/02-REVIEW.md
**Iteration:** 1

**Summary:**
- Findings in scope: 4 (WR-01 to WR-04; the IN-* findings are out of scope)
- Fixed: 3 (WR-02, WR-03, WR-04)
- Skipped: 1 (WR-01, a tooling finding)

`workflow.use_worktrees` is false, so every edit, commit and local gate ran in the main checkout on branch
`phase/02-the-cpu-matches-the-public-vectors`. The Linux lanes ran in an `ubuntu:26.04` arm64 container over that
checkout. Nothing was pushed.

## Fixed Issues

### WR-02: The size-tag cap does not stop reads past the caller's object, as its comment says it does

**Files modified:** `src/instance.c`, `docs/embedding.md`
**Commit:** be6d152
**Applied fix:** This changes only a comment and the docs, not behaviour. The `MUHTARI__SIZE_TAG_MAX` comment now
says that a tag promises that many readable bytes, and that the cap bounds how far a wrong tag can make the check
read without making a wrong tag safe. In `docs/embedding.md`, "Structs and sizes" now says the same in two short
lines: `size` promises that many readable bytes, a wrong tag is undefined behaviour, so set it from `sizeof`, and
the 4096-byte cap only bounds the read. No test was added, because no behaviour changed.

### WR-03: A second HALT request or NMI edge silently replaces one still in flight

**Files modified:** `src/bus.c`, `src/internal.h`, `docs/hardware/bus.md`, `tests/bus/test_bus.c`, `tests/bus/tests.cmake`, `tests/inventory/ci.txt`, `tests/inventory/wasm32.txt`
**Commit:** e0b2a25
**Applied fix:** The rule is one at a time.
- `muhtari__bus_request_halt` returns early when `halt_stage != HALT_NONE`, which covers both a request still
  waiting and a request after /HALT has fallen.
- `muhtari__bus_raise_nmi` returns early while `nmi_pending` is set, so the edge already raised and not yet
  latched stands. This matches `latch_nmi`, which already keeps a latched edge the CPU has not taken.
- The rule is stated in docs/hardware/bus.md, in both HALT and NMI, and in the entry-point comment in
  src/internal.h.

There are two new tests, labelled `portable,sally`, registered in tests/bus/tests.cmake, `ci.txt` and `wasm32.txt`:
- `bus.halt.one_window_at_a_time`: a request during HALT_LOW, after c0 and before c2's hold, leaves c2 frozen
  for the first window's 24 ticks. A request before the first window's fall leaves the fall at tick 8, not 4. Once
  a window ends, the next request is taken.
- `bus.nmi.one_edge_at_a_time`: an edge at tick 14, raised before the edge at tick 6 is latched, is ignored, so
  `nmi_phi2` is 2, not 4.

### WR-04: `muhtari__cpu_step` does not enforce the jammed invariant, and a jammed CPU advances no time

**Files modified:** `src/cpu.c`, `src/internal.h`, `docs/hardware/sally.md`, `tests/cpu/test_cpu.c`, `tests/cpu/tests.cmake`, `tests/inventory/ci.txt`, `tests/inventory/wasm32.txt`
**Commit:** 5d6c2ae
**Applied fix:** `muhtari__cpu_step` now starts with a jammed guard. A step on a jammed CPU makes one read of
`$FFFF`, as the vectors' trailing reads do, and returns. It takes no NMI and changes no register. Updated to match:
- the `jammed` comment and the `muhtari__cpu_step` declaration comment in src/internal.h;
- the JAM section of docs/hardware/sally.md, which no longer hands idling to "the run loop".

The new test `cpu.jam.step_reads_ffff` (labels `portable,sally`) is registered in tests/cpu/tests.cmake, `ci.txt`
and `wasm32.txt`. It runs a JAM at $0200 with LDA #$55 at $0201, then latches an NMI, then steps the jammed CPU
twice. Each step must make exactly one bus cycle, a read at $FFFF, and advance phi2 by 1. Afterwards PC, A, X, Y,
S, P and the poll mark must be unchanged, `jammed` must still be set, and the latched `nmi_phi2` must not have been
taken. The cpu.vectors tests are unaffected: `ctest --test-dir build/ci -R '^cpu\.vectors' -j8` gave exit 0, with
263 of 263 passing.

## Skipped Issues

### WR-01: Nightly pull-request path filter misses inputs of the vectors-full and loader-fuzz jobs

**File:** `.github/workflows/nightly.yml:18-36`
**Reason:** tooling finding: a seed under AGENTS rule 7. The follow-up is
`.planning/todos/pending/2026-10-19-close-the-phase-2-review-seeds-on-the-nightly-filter-and-the-rom-tables.md`.
**Original issue:** The `paths` filter on nightly's pull_request trigger leaves out inputs of the vectors-full and
loader-fuzz jobs (src/internal.h, src/instance.c, src/riot.c, src/cartdb.h, src/sha1.h, tools/cartdb.cmake,
tests/alloc.h, the two inventories, check_junit.cmake, CMakePresets.json, CMakeLists.txt, pins.txt). A pull request
that changes one of them merges without running those jobs.

## New tests fail against the pre-fix source

In each case the src change was stashed, the build rebuilt and the test run, then the stash was popped, the build
rebuilt and the test passed again.

| Command | Pre-fix exit | Pre-fix output (abridged) |
|---|---|---|
| `git stash push src/bus.c && cmake --build build/ci && build/ci/tests/muhtari-test-bus halt.one_window_at_a_time` | 1 | `c2, a second request after the fall: expected 24 ticks got 4`; `c1, a second request before the fall: expected 4 ticks got 24` |
| `build/ci/tests/muhtari-test-bus nmi.one_edge_at_a_time` (same stash) | 1 | `expected 2 as nmi_phi2 got 4` |
| `git stash push src/cpu.c && cmake --build build/ci && build/ci/tests/muhtari-test-cpu jam.step_reads_ffff` | 1 | `bus cycles in a jammed step: expected 1 got 9`; `A: expected 0 got 85`; `the latched NMI, not taken: expected 11 got 0` |

After `git stash pop` and a rebuild, all three exit 0.

## Verification

All commands ran after the last fix, at HEAD 5d6c2ae. Every commit passed the `.githooks` hygiene hook. C files
were checked with brew `llvm@21` `clang-format --dry-run --Werror`.

On the main checkout (macOS arm64):

| Command | Exit status |
|---|---|
| `cmake --workflow --preset ci` | 0 (408 tests, then the 6 `release` tests) |
| `cmake --workflow --preset hygiene` | 0 (24 tests) |

In a container, using the recipe in docs/testing.md, "Running the Linux lanes locally". The command was
`docker run --rm --platform linux/arm64 -v "$PWD:/src" -v /src/build -w /src ubuntu:26.04`, with the asan, be32
and wasm32 packages, the pinned CMake, wasmtime and 7800basic:

| Command | Exit status |
|---|---|
| `cmake --workflow --preset asan` | 0 (397 tests) |
| `cmake --workflow --preset nofp` | 0 (12 tests) |
| `cmake --workflow --preset be32` | 0 (372 tests) |
| `cmake --workflow --preset wasm32` | 0 (370 tests) |

The three new tests ran in asan, be32 and wasm32. `nofp` registers none of them.

---

_Fixed: 2026-10-19T18:50:52Z_
_Fixer: Claude (gsd-code-fixer)_
_Iteration: 1_
