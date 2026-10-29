---
phase: 11-full-library-acceptance
reviewed: 2026-10-29T12:45:00Z
depth: standard
files_reviewed: 11
files_reviewed_list:
  - docs/compatibility.md
  - docs/emulation-tests.md
  - tests/cmake/compat_doc_controls.cmake
  - tests/compat/files.cmake
  - tests/compat/library.cmake
  - tests/compat/local.cmake
  - tests/compat/local_fixture.cmake
  - tests/compat/tests.cmake
  - tests/diag/tests.cmake
  - tests/policy/tests.cmake
  - tools/compat_doc.cmake
findings:
  critical: 2
  warning: 3
  info: 4
  total: 9
status: issues_found
---

# Phase 11: Code Review Report (gap-closure round: plans 11-09, 11-10, 11-11)

**Reviewed:** 2026-10-29T12:45:00Z
**Depth:** standard
**Files Reviewed:** 11
**Status:** issues_found

## Summary

This round was reviewed against `git diff b21f31d..HEAD` plus a full read of the new `tests/compat/files.cmake`. Each finding below that makes a behavioural claim was reproduced by running the script (in the session scratchpad, not in the tree).

Resolved from the earlier review:

- The `title.pal.bios` budget (earlier CR-01) is now 4 s with its measurement.
- The shared file rule (earlier WR-01) now exists in `files.cmake`. It does not follow links, skips unreadable entries, and rejects `;` and unmatched `[` in names with a fixed message. The fixture covers it.
- The `rm -rf` of a path beside `OUT` (earlier WR-02) is now guarded by a `.local-pass` marker, but a `;` in `OUT` still defeats it (CR-02).
- The bankset race (earlier WR-03) is fixed with a separate output tree per test.
- The "public gameplay" label (earlier WR-04) is now "public title screen".
- The claims table (earlier WR-05) is only partly fixed. The new generator check that should tie each sample-backed test to its sample is dead for most of the entries it was added for (CR-01).
- The unmeasured budgets (earlier WR-06) now carry measurements. The measurements are local macOS numbers plus one unreferenced "full ci run" (IN-02).

`docs/compatibility.md` is fresh: `tools/compat_doc.cmake -DCHECK=docs/compatibility.md` exits 0. `compat_doc_controls.cmake` also passes, but its new case 9 exercises only the working half of the check (CR-01).

## Critical Issues

### CR-01: The sample-backed claim check never runs for `basic.*` tests, so most of its entries are unchecked

**File:** `tools/compat_doc.cmake:322-337` (new loop), control at `tests/cmake/compat_doc_controls.cmake:126-133`
**Issue:** The loop is guarded by `if(test MATCHES "^basic[.](.+)$" OR test MATCHES "^title[.](.+)[.](fast|bios)$")` and then reads `CMAKE_MATCH_1`. CMake evaluates both operands of `OR`. When the first regex matches and the second does not, the failed second match clears `CMAKE_MATCH_1`.

Reproduction in a scratch script: `basic.rmtramdemo -> []`, `title.simple.fast -> [simple]`.

For every `basic.<sample>` test, `sample` is therefore empty, `DEFINED sample_board_` is false, and the check silently passes. That covers `basic.rmtramdemo`, `basic.rmtramcompressiondemo`, `basic.pokey` and `basic.rmtromdemo`, which back the new `ram`, `supergame` and `pokey450` claims.

Confirmed by changing `rmtramdemo`'s board in a copy of the ledger from `supergame+ram` to `linear`. The generator still exits 0, although that sample backs both the `ram` and `supergame` board claims. The only path that works is `title.<sample>.fast|bios`, which is the only one control case 9 exercises. The phase's stated fault, "a linear sample cannot back a supergame claim", is therefore not caught for the samples that were added to back `supergame`.

**Fix:** Match separately and capture the sample from whichever regex matched, and fail instead of skipping for ledger-backed samples:
```cmake
set(sample "")
if(test MATCHES "^basic[.](.+)$")
  set(sample "${CMAKE_MATCH_1}")
elseif(test MATCHES "^title[.](.+)[.](fast|bios)$")
  set(sample "${CMAKE_MATCH_1}")
endif()
if(NOT sample STREQUAL "")
  if(NOT DEFINED sample_${kind}_${sample})
    if(test MATCHES "^basic[.]")   # basic.* tests are always 7800basic samples
      message(FATAL_ERROR "compat_doc: ${name} names ${test}, but the ledger has no 7800basic-0.41/${sample} row")
    endif()
  else()
    ...existing token check...
  endif()
endif()
```
Add a control that changes a `basic.*`-backed sample's board (for example `rmtramdemo` from `supergame+ram` to `linear`) and expects the failure, so the `basic.` path is exercised as well as the `title.` path.

### CR-02: A `;` in `OUT` splits the `rm -rf` argument and deletes an unrelated directory

**File:** `tests/compat/local.cmake:67-74` (`foreach(step "rm;-rf;${out_rows}" ...)`)
**Issue:** The steps are semicolon-joined strings expanded unquoted into `COMMAND ${CMAKE_COMMAND} -E ${step}`. `OUT` is not name-checked; only `DIR` is. With `-DOUT=<tmp>/victim;x/out.csv`, `out_rows` is `<tmp>/victim;x/out.csv.rows`. The `EXISTS` guard on line 64 passes because that full path does not exist. The step then expands to `rm -rf <tmp>/victim` and `x/out.csv.rows`.

Reproduced: a directory `victim/` holding a file `keep` was emptied by `local.cmake` (the file was deleted), then the run failed naming `.../victim` in CMake's own error.

This is the same fault family the `.local-pass` marker was added to prevent ("a relative OUT deleting a directory it did not create"). It is data loss on a path the pass did not make, and it leaks a private path. In the library preset `OUT` is `${PROJECT_BINARY_DIR}/local.csv`, so the exposure is a build directory path with `;` in it. A direct `-P local.cmake` invocation is exposed to any `OUT`. `local_fixture.cmake` has no `;`-in-`OUT` case.

**Fix:** Reject list-hostile names in `OUT` before any step, and avoid string-joined steps:
```cmake
compat_name_check(local "${OUT}")   # rejects [ ] ;  (message names only the prefix)
execute_process(COMMAND ${CMAKE_COMMAND} -E rm -rf -- "${out_rows}" ...)
```
Better still, run each command as its own `execute_process(COMMAND ${CMAKE_COMMAND} -E rm -rf "${out_rows}" ...)` with quoted arguments. Add a fixture case `drive(semiout ... "${WORK}/a;b/out.csv")` that expects a fixed fault and checks that `${WORK}/a` was not removed.

## Warnings

### WR-01: An `OUT` that is an existing directory passes the writability probe and then leaks the full path

**File:** `tests/compat/local.cmake:67-74` and `:193`
**Issue:** `cmake -E touch <directory>` succeeds, so the probe passes. The run then evaluates every present row (up to 60 s each) and fails at `file(WRITE ${OUT} ...)` with CMake's own text, `file failed to open for writing (Is a directory): <full path>`. This is the exact failure the header says the probe prevents ("CMake's own error text, which names the path, never reaches the log"). Reproduced with `-DOUT=<dir>`, which left `<dir>.rows/.local-pass` behind.

**Fix:** Add `IS_DIRECTORY "${OUT}"` to the fixed-message check next to the symlink and marker check on line 64, and add a fixture case for it:
```cmake
if(IS_DIRECTORY "${OUT}" OR IS_SYMLINK "${out_rows}" OR (EXISTS "${out_rows}" AND NOT EXISTS "${out_rows}/.local-pass"))
```
Give that case its own fixed message, or reuse "cannot write the counts file".

### WR-02: The library preset now silently skips symlinked files, and its failure message and docs do not say so

**File:** `tests/compat/library.cmake:75-83`, `docs/emulation-tests.md:306-314`
**Issue:** Before this round `file(SHA1 ...)` followed links, so a `MUHTARI_ROM_DIR` of symlinks worked for the `pr` rows. Now `compat_files` drops every link, and the count it returns (`library_unread`, `library_dir_unread`) is discarded. A row whose file is only reachable through a link fails at configure with `no file under <roots> has SHA-1 ...`, which gives no hint that a link was skipped. The new docs say "copy or hard-link dumps in rather than symlink them" only in the `library.local` paragraph. They also say a bracket or semicolon "stops the run", but the same names stop `cmake --preset library` at configure, which gates every `pr` row, so a private dump's file name can block the whole preset. That scope is not stated.

**Fix:** Append the unread count to the missing-file error and the docs:
```cmake
if(library_unread GREATER 0)
  string(APPEND library_hint "; ${library_unread} links or unreadable entries were skipped, so copy or hard-link files in")
endif()
```
Add one sentence to the library preset paragraph in `docs/emulation-tests.md` that links are skipped and a bracket or semicolon fails configure.

### WR-03: A root name containing `*` or `?` is accepted and matches other directories

**File:** `tests/compat/files.cmake:20-26` (`compat_name_check`), `:38` (`file(GLOB_RECURSE ... "${root}/*")`)
**Issue:** The check rejects `[`, `]` and `;` because they are glob metacharacters or list separators, but `*` and `?` are glob metacharacters too and are valid directory names on POSIX hosts. Reproduced: with directories `g/a*` and `g/ab`, scanning `g/a*` keeps `g/ab/f2` as well, so the pass reads files outside the directory it was given. Those files are private and could be matched into rows or counted.

**Fix:** Extend the pattern to `[][;*?]`, update the "bracket or a semicolon" wording in `compat_name_check`, `docs/emulation-tests.md` and the fixture's fixed-message list, and add a fixture directory `dir*x` alongside `dir[x`.

## Info

### IN-01: `library_source_fault` is bypassed by `file (GLOB ...)` with a space before the parenthesis

**File:** `tests/compat/local_fixture.cmake:293-300` (banned list), `:312-324` (SHA-1 sites)
**Issue:** The normalisation collapses spaces after `(` and before `)` but not between a command name and `(`, which CMake allows. `file (GLOB_RECURSE ...)`, `file (SHA1 ...)` and `compat_files (library ...)` all evade the substring and regex checks. The comment on the function claims edits that keep the rule are tolerated and edits that bypass it are caught, but this equivalent spelling is not covered by either control list. This is a guard for accidental edits, not an attacker, so the impact is low.
**Fix:** Add `string(REGEX REPLACE "([a-z_]+) \\(" "\\1(" plain "${plain}")` to `library_normalise`, and add a `file (GLOB ...)` fault copy to the control set.

### IN-02: The budget evidence is a local macOS run and an unreferenced "full ci run"

**File:** `tests/compat/tests.cmake:37-41,72-80,134-139`, `tests/policy/tests.cmake:135`
**Issue:** The new `provisional:` comments cite "macOS arm64 local, slowest of 10 runs alone and one full ci run" with no run URL or id. The older budgets cite CI run ids (`library.cmake`), and project rule 10 says evidence is a command, an exit status and a run URL. The values are labelled provisional, so this is acceptable for now, but it cannot be traced.
**Fix:** Add the CI run id, as the neighbouring comments do, on the next measurement pass, or state in the comment which run the 0.35 s and 1.49 s figures come from.

### IN-03: The image maximum is a bare literal in two places, and `local.cmake` does not check that `DIR` exists

**File:** `tests/compat/files.cmake:62` (`1048704`), `tests/compat/local_fixture.cmake` (`1048705`), `tests/compat/local.cmake:114`
**Issue:** The image maximum has no name, so the fixture's "one byte over" value must be kept in step by hand. Separately, `local.cmake` called on a missing `DIR` (a typo, or the directory removed after configure) matches nothing and exits 0 with every row `absent`. The configure-time check in `library.cmake` covers only the preset path.
**Fix:** `set(COMPAT_IMAGE_MAX 1048704)` near the top of `files.cmake`, used by the fixture. Add `if(NOT IS_DIRECTORY "${DIR}") message(FATAL_ERROR "local: the directory does not exist")`, and add the message to the fixture's fixed list.

### IN-04: Unused outputs of the new scans

**File:** `tests/compat/library.cmake:75,83`
**Issue:** `library_dir_unread` is set and never read, and `library_unread` is only useful once WR-02 is fixed. Until then they are dead values returned for a contract the caller ignores.
**Fix:** Use them as in WR-02, or drop the arguments from the second call.

---

_Reviewed: 2026-10-29T12:45:00Z_
_Reviewer: Claude (gsd-code-reviewer)_
_Depth: standard_
