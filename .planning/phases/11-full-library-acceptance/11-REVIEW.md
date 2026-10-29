---
phase: 11-full-library-acceptance
reviewed: 2026-10-29T14:55:00Z
depth: standard
files_reviewed: 44
files_reviewed_list:
  - .github/workflows/ci.yml
  - .gitignore
  - ASSET_POLICY.md
  - CMakePresets.json
  - PROVENANCE.md
  - README.md
  - THIRD-PARTY-NOTICES.md
  - docs/bankset-tests.md
  - docs/compatibility.md
  - docs/emulation-tests.md
  - docs/expansion-tests.md
  - docs/maintaining.md
  - docs/pal-tests.md
  - docs/testing.md
  - libretro/muhtari_libretro.info.in
  - src/cart.c
  - src/internal.h
  - tests/built/expected.txt
  - tests/cart/test_cart.c
  - tests/cart/write_images.c
  - tests/cmake/ci_required.cmake
  - tests/cmake/claims_wording.cmake
  - tests/cmake/compat_doc_controls.cmake
  - tests/cmake/compat_ratchet.cmake
  - tests/cmake/compat_ratchet_controls.cmake
  - tests/cmake/info_claims.cmake
  - tests/compat/ledger.csv
  - tests/compat/library.cmake
  - tests/compat/local.cmake
  - tests/compat/local_fixture.cmake
  - tests/compat/pin.cmake
  - tests/compat/predicate.c
  - tests/compat/tests.cmake
  - tests/diag/ledger_crc.cmake
  - tests/diag/tests.cmake
  - tests/hygiene/tests.cmake
  - tests/inventory/ci.txt
  - tests/inventory/hygiene.txt
  - tests/inventory/library.txt
  - tests/inventory/wasm32.txt
  - tests/libretro/tests.cmake
  - tests/policy/tests.cmake
  - tools/compat_doc.cmake
  - tools/softlist_ledger.cmake
findings:
  critical: 1
  warning: 6
  info: 8
  total: 15
status: issues_found
---

# Phase 11: Code Review Report

**Reviewed:** 2026-10-29T14:55:00Z
**Depth:** standard
**Files Reviewed:** 44
**Status:** issues_found

## Summary

I reviewed the phase diff (`git diff origin/main...HEAD`) for every listed file. For `tests/compat/ledger.csv`
and `docs/compatibility.md` I reviewed the generators (`tools/softlist_ledger.cmake`, `tools/compat_doc.cmake`)
and spot-checked the output.

The C changes hold up. `muhtari__cart_identify_entry` sets `*entry` to `UINT32_MAX` before any early return and
writes the matched index only after a lookup hit. The plain `muhtari__cart_identify` wrapper keeps its old
behaviour. `predicate.c` now derives `frame_bytes` and the DMA line scan from the loaded region, with the scan
bounded by the statistics array. The new exit code 3 (image refused) is kept apart from harness faults.

The ratchet's tier-demotion rule, the `CI required` changes and the softlist generator's cross-check against
`src/cartdb.c` are also correct, and each has controls.

The defects are in five places:
- a test budget that is too tight to pass reliably in CI
- the local pass's promise never to put a path in its output
- a destructive `rm -rf` built from an unchecked path
- a pre-existing output-directory race that this phase extended
- the generated compatibility page, which uses a "public gameplay" label for frame-hash evidence and lists a
  backing test that does not exercise its claim

### Known observations

1. **`title.pal.bios` 2 s budget: confirmed, and raised to a blocker (CR-01).**
   - **Local timings, measured alone on this machine (Release build), `real` time:**

     | Run | Time |
     |---|---|
     | `pal.a78 boots@120 --bios`, three runs | 0.64, 0.63, 0.60 s |
     | `simple` on the BIOS path, same budget | 0.05 to 0.06 s |
     | `pokey` title on the BIOS path | 0.32 s |
     | `eightysix` on the BIOS path | 0.46 s |

   - **What this means:** the PAL BIOS path costs about 12 times what `title.simple.bios` costs, yet has the same
     2 s budget. That budget was set from the `simple` measurement (2 x 0.60 s).
   - **Precedent:** `library.7800basic-0.41.eightysix` (POKEY, BIOS path) measured 1.23 s on Linux CI and still
     ran out of a 2 s budget twice on a release head, which is why `tests/compat/library.cmake:139-142` raised it
     to 4 s.
   - **Which lanes are exposed:**
     - The scale-1 lanes (`ci`, `ci-msvc`) get 2 s.
     - Intel macOS (`ci` preset with the CI matrix's `timeout_scale: 2`), `asan` and `wasm32` get 4 s.
     - Every test preset runs with `"jobs": ""`, so all cores run tests in parallel alongside `diag.expansion`
       (about 40 s).
   - **Verdict:** 2 s gives under 2x margin over the expected Linux CI time on the unscaled lanes, and it is the
     same budget already shown to be too small for a POKEY title.

2. **`diag.bankset_tracer` and `diag.bankset` share an output directory: confirmed. The race existed before this
   phase, and this phase widened it (WR-03).**
   - Both generated scripts set `OUT` to `${PROJECT_BINARY_DIR}/tests/diag/bankset`
     (`tests/diag/tests.cmake:874`). The matrix script `include`s the tracer script, so both run case `32n` into
     `${OUT}/32n/`, and both write `${OUT}/wrong.a78`, `${OUT}/refused.a78` and the PNG and report files there.
   - Neither test has `RESOURCE_LOCK`. The only `RESOURCE_LOCK` in the tree is `build_tree` in
     `tests/abi/tests.cmake:145`.
   - `git log -S'diag.bankset_tracer'` dates both registrations to `949fc61` (#36), which is before this phase.
   - This phase added `diag_ledger_crc(diag/bankset "${dir}/report.txt")` for `32n`
     (`tests/diag/tests.cmake:248-250`). That is a new reader of a file the other test is rewriting at the same
     time, so the race now has one more way to fail.

## Critical Issues

### CR-01: `title.pal.bios` timeout is below the observed cost of a PAL plus POKEY BIOS run

**File:** `tests/compat/tests.cmake:43-46`

**Issue:** `title.pal.bios` runs `tests/diag/pal.a78` (a linear PAL cartridge with POKEY at $0450) through the
OpenBIOS hand-off: about 330 PAL frames before the hand-off, then 120 more, all with POKEY synthesis. Its budget
is `TIMEOUT 2`, marked "provisional, not yet measured in CI".
- On this machine it measures 0.60 to 0.64 s when run alone. That is about 12 times `title.simple.bios`, whose
  2 s budget was derived from a 0.60 s CI measurement.
- The closest measured analogue, `eightysix` on the BIOS path, already ran out of the same 2 s budget in CI and
  was raised to 4 s (`tests/compat/library.cmake:139-142`).
- The test is in the `ci` and `wasm32` inventories, so it runs in required lanes under all-core `ctest`
  parallelism.
- It timed out repeatedly during execution under host load.

A flaky test in a required lane makes `CI required` fail at random. ENGINEERING section 11 also requires a new
resource bound to come with comparable measurements, and this one has none.

**Fix:**

```cmake
muhtari_add_test(NAME title.pal.bios
  COMMAND muhtari-test-predicate ${PROJECT_SOURCE_DIR}/tests/diag/pal.a78 boots@120
    --bios ${compat_bios}
  LABELS portable title TIMEOUT 4)  # PAL + POKEY BIOS path: 0.64 s alone locally, n=3; eightysix analogue 1.23 s Linux CI
```

Then replace the comment with ceil(2 x the slowest CI time) once a CI run exists. The repository's convention is
2x the slowest measured run, as in `library.cmake:139-142`.

## Warnings

### WR-01: The local pass leaks full paths, and can miscount, when a dump directory has a dangling symlink, an unreadable file, or a `;` or unbalanced `[` in a file name

**File:** `tests/compat/local.cmake:97-105`; `tests/compat/library.cmake:76-96`

**Issue:** `local.cmake` promises that nothing private reaches the log. Its header lists "leaks a path, a file
name ... into a log" as a fault it prevents. But it passes each globbed file straight to `file(SIZE ${file} size)`,
and `library.cmake` does the same at configure time with `file(SIZE)` and `file(SHA1)`. I reproduced three cases
with CMake in the scratchpad:
- A dangling symlink in `DIR` makes the run stop with
  `file SIZE requested of path that is not readable: /full/path/to/dangling.a78`. CMake's own error text prints
  the full private path. An unreadable file (mode 000) fails the same way.
- A file named `semi;colon.a78` splits into two list elements. The run stops on `.../semi` and prints its path.
- A file named `Game [a.a78` (an unbalanced `[`) makes CMake treat the following `;` separators as literal text,
  so every later file merges into one element. `list(LENGTH)` reported 1 for 3 files. The run then either stops
  and prints the merged paths, or miscounts.

`local_fixture.cmake` does not exercise any of these cases.

**Fix:**
- Filter before touching each file, and count rejects without naming them:
  ```cmake
  foreach(file IN LISTS found)
    if(IS_SYMLINK "${file}" OR NOT IS_READABLE "${file}")
      math(EXPR unreadable "${unreadable} + 1")
      continue()
    endif()
    file(SIZE "${file}" size)
    ...
  ```
  `if(IS_READABLE)` needs CMake 3.29; the presets already require 4.3.
- Before globbing, reject a `DIR` that contains `[`, `]` or `;` with a fixed message ("local: the directory name
  holds a character CMake lists cannot carry"). Count file names that contain those characters as unreadable.
- Add a dangling-symlink case to `local_fixture.cmake`, and apply the same guard to the two loops in
  `library.cmake`.

### WR-02: `local.cmake` runs `rm -rf` on a path derived from an unchecked `OUT`; a bare file name targets `/local`

**File:** `tests/compat/local.cmake:50-58`

**Issue:** `get_filename_component(out_dir ${OUT} DIRECTORY)` returns an empty string for a bare file name.
`out_rows` then becomes `/local`, and the next step runs `cmake -E rm -rf /local`. I confirmed this with
`cmake -DOUT=local.csv -P`, which printed `out_rows=[/local]`.

The script's header documents direct `-P` use with `-DOUT=<local.csv>`. The Linux lanes' documented recipe runs as
root inside an `ubuntu:26.04` container, where `/local` (or any directory named `local` next to a relative `OUT`)
would be deleted.

**Fix:** Make `OUT` absolute before deriving anything from it. Require the result to have a non-empty parent, and
name the scratch directory after the output file so the script never deletes a generic name:

```cmake
cmake_path(ABSOLUTE_PATH OUT NORMALIZE)
cmake_path(GET OUT PARENT_PATH out_dir)
cmake_path(GET OUT STEM out_stem)
if(out_dir STREQUAL "" OR out_dir STREQUAL "/")
  message(FATAL_ERROR "local: cannot write the counts file")
endif()
set(out_rows ${out_dir}/${out_stem}.rows)
```

### WR-03: `diag.bankset` and `diag.bankset_tracer` race on one output tree, and this phase added a reader inside it

**File:** `tests/diag/tests.cmake:248-250`, `:874`, `:881-890`

**Issue:** Both tests set `OUT` to the same `${PROJECT_BINARY_DIR}/tests/diag/bankset` and both run case `32n`.
Each regenerates `bankset32n.a78`, `report.txt`, `pixels.png`, `filter-fault.png`, `fault-*.txt`, and
`${OUT}/wrong.a78` and `refused.a78`. Every test preset runs `ctest` with `"jobs": ""` (all cores), so the two
tests can run at the same time. One race was observed during execution.

The race itself comes from `949fc61` (#36). This phase made it more likely to show up by adding
`diag_ledger_crc(diag/bankset "${dir}/report.txt")`, which can read a `report.txt` the other process has just
truncated or half-written. The result is a spurious "no diagnostic line" or a CRC mismatch.

**Fix:** Give the two tests separate output trees. In the matrix script, set `OUT` after the shared settings and
change the tracer's generated script so it does not override an `OUT` that is already set:

```cmake
file(GENERATE OUTPUT ${diag_bankset_matrix} CONTENT
"set(BANKSET_EXPANDED TRUE)
set(OUT [==[${PROJECT_BINARY_DIR}/tests/diag/bankset-matrix]==])
...")
```

The smallest alternative is
`set_tests_properties(diag.bankset diag.bankset_tracer PROPERTIES RESOURCE_LOCK diag_bankset_out)`.

### WR-04: The compatibility page labels title-frame hashes "public gameplay", contradicting the phase's own disclaimer

**File:** `tools/compat_doc.cmake:151-163`, `:253`, `:290-298`; output in `docs/compatibility.md:54`, `:78`,
`:91`, `:95`

**Issue:** `compat_doc.cmake` labels every passing `7800basic-0.41/*` row with a `title@` predicate as
`gameplay`.
- **What the predicate checks:** `title@120=H` means only that frame 120 of the fast boot hashes to H, plus a
  window after the BIOS hand-off. That is the title or demo screen shown at that point
  (`docs/emulation-tests.md`, title).
- **What the samples are:** many are tech demos, such as `fontcheck`, `colorfade`, `soundtest`, `boxcollision`
  and `keypadcontrol`.
- **What the page publishes:**
  - "public gameplay | 27"
  - "region | NTSC | public gameplay"
  - "board | ram | public gameplay"
  - "peripheral | pokey450 | public gameplay"
- **Why it conflicts:** the phase's stated wording goal (README, `.info`, and the "metadata ... not playability"
  sentence on the page itself) is not to overclaim. A frame hash is not gameplay evidence.
- **Why no check caught it:** `claims_wording.cmake` does not catch it, because "gameplay" is not on its deny
  list.

**Fix:** Name the class after what the predicate shows:
- Rename the label (for example `title=public title screen`) and the claim class
  (`set(class "public title screen")`).
- Update the explanatory paragraph at `compat_doc.cmake:342-346` to match.
- Regenerate `docs/compatibility.md`.
- Update `compat_doc_controls.cmake:92`'s expected text.

### WR-05: The claims table lists a backing test that does not exercise the claim, and shows "public gameplay" classes with no backing test

**File:** `tools/compat_doc.cmake:34`, `:32`, `:44`; output in `docs/compatibility.md:78`, `:80`, `:91`

**Issue:**
- **`supergame` claim:** `"board:supergame:^(sg144|bank6)$:diag.sg144 title.simple.fast"` names
  `title.simple.fast` as backing for the SuperGame board. But `simple` is a linear sample: its ledger row's
  board column is `linear`. The inventory check only proves that the test exists, not that it exercises the
  claim.
- **`ram` and `pokey450` claims:** these get the class "public gameplay" from `library.*` pass rows, yet their
  backing-test column says "none".
- **The page's own sentence is false:** the page says "Each claim names its evidence class and the tests that
  exercise it". For these rows that is not true:
  - `pokey450` is exercised by `diag.pal`, `diag.expansion` and the `library.*` rows.
  - `ram` is exercised by the `library.*` `supergame+ram` rows.

**Fix:**
- Drop `title.simple.fast` from the `supergame` entry, or replace it with a test that loads a SuperGame image.
- Name the real backing tests for `ram` and `pokey450`. If those tests live only in the `library` inventory,
  either check them against `tests/inventory/library.txt` as well, or print "library preset" instead of "none".

### WR-06: Several new test budgets have no measurement at all

**File:** `tests/compat/tests.cmake:42`, `:77`, `:99`, `:118`, `:133`; `tests/policy/tests.cmake` (the
`compat_doc_fresh.self_test` budget, "provisional, eight generator runs")

**Issue:** `title.pal.fast`, `title.pin.order`, `title.pin.order.self_test`, `title.pin.recipe` and
`title.local.fixture` carry "provisional, not yet measured in CI" with no number at all. Other new budgets in
this phase at least cite "n=1, local". ENGINEERING section 11 requires "comparable measurements" for a new
resource bound.
- `title.pin.recipe` (3 s) runs two predicate processes on `diag/cpu`, including the BIOS path.
- `title.local.fixture` (20 s) runs six nested `cmake -P` drivers. Under `wasm32` each of those spawns wasmtime
  several times.

None of these numbers can be checked against a run.

**Fix:** Record at least the local measurement (command, n, max) beside each budget now, as the other new tests
do. Replace it with ceil(2 x the slowest CI time) once the first CI run lands.

## Info

### IN-01: The "N of N accounted for" headline cannot report a gap

**File:** `tools/compat_doc.cmake:114`, `:147`, `:264`

**Issue:** `pinned_accounted` is incremented on exactly the same rows as `pinned_total`. Every row that would not
be accounted for is a `FATAL_ERROR` earlier, so the headline always reads "204 of 204". It looks like a check but
carries no information.

**Fix:** Drop the "X of" part, or compare against an independent count, such as `softlist_ledger`'s entry count
from the pinned XML passed in as `-DENTRIES=`.

### IN-02: Commas in titles become a double space

**File:** `tools/softlist_ledger.cmake:159`

**Issue:** `string(REPLACE "," " / " ...)` turns "NTSC, added" into "NTSC /  added" (27 rows, for example
`beefdrop` and `crazyotto`). The same text is published in `docs/compatibility.md`.

**Fix:** Use `string(REGEX REPLACE ", *" " / " title "${desc}")`.

### IN-03: The local driver's header disagrees with its code, and skipped rows read as "absent"

**File:** `tests/compat/local.cmake:9`, `:83`

**Issue:** The header says "An unknown, present row is judged". The code judges every row that is not
`skipped`, so a future `unsupported` row would be judged too.

Separately, rows that are `skipped` (the three expansion devices) never get a key. They are counted "absent" even
when the user's directory holds the file.

**Fix:** Either say "every row that is not skipped", or filter on `unknown`. Report skipped rows as `skipped`
rather than `absent` in `local.csv` and in the counts line.

### IN-04: The wording check's deny list is substring-based and does not scan subdirectories

**File:** `tests/cmake/claims_wording.cmake:14`, `:18`, `:50-54`

**Issue:**
- **False positives:** "all games" matches inside "small games", and "every game" matches inside "every
  gameplay".
- **Misses:** "full compatibility" would also match a disclaimer such as "no claim of full compatibility".
  Meanwhile overclaims like "100% compatible" or "all titles" are not caught.
- **Scope:** `file(GLOB "${ROOT}/docs/*.md")` skips `docs/hardware/*.md`.

**Fix:** Anchor phrases on word boundaries, using `(^|[^a-z])all games([^a-z]|$)`, and use `GLOB_RECURSE` for
`docs/`.

### IN-05: The `.info` description length check passes silently when the line does not match the regex

**File:** `tests/cmake/info_claims.cmake:42-49`

**Issue:** A description with different spacing (`description="..."`) or an escaped quote skips the length check
without any message.

**Fix:** Fail when the file contains a `description` key that the strict regex does not match.

### IN-06: `docs/testing.md` lists a test under the wrong label

**File:** `docs/testing.md:123-124`

**Issue:**
- The `library` row names `policy.compat_ratchet.controls`. That test carries the `policy` label and runs in the
  `ci` and `wasm32` inventories, not in the library preset.
- The `title` row describes only `simple`, the blank-screen refusal and the local fixture. It omits
  `title.pal.*`, `title.pin.order(.self_test)` and `title.pin.recipe`.

**Fix:** Move the ratchet controls to the `policy` row, and list the PAL and pin-order tests under `title`.

### IN-07: The expansion CRC check could stop running without anyone noticing

**File:** `tests/diag/tests.cmake:65-69`

**Issue:** The canonical case is picked by comparing `"${page}${window}${pal}${dual}${gate}"` with `"00002"`.
If the case loops change, the ledger CRC for `diag/xboard` and `diag/xm` stops being checked and the run still
passes.

**Fix:** Set a flag when each check runs, and `FATAL_ERROR` at the end of the expansion run if either flag is
unset.

### IN-08: Generator guard without a control

**File:** `tests/cmake/compat_doc_controls.cmake:98-117`

**Issue:** The generator rejects a pinned-list row that carries `evidence_commit`, `evidence`, `oracle` or
`checked` (`compat_doc.cmake:142-146`). No control case exercises that guard.

**Fix:** Add `row(bad "pinned_y" ...)` with an evidence value and expect "a pinned-list row cannot carry".

---

_Reviewed: 2026-10-29T14:55:00Z_
_Reviewer: Claude (gsd-code-reviewer)_
_Depth: standard_
