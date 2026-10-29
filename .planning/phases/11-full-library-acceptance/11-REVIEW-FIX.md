---
phase: 11-full-library-acceptance
fixed_at: 2026-10-29T17:04:43Z
review_path: .planning/phases/11-full-library-acceptance/11-REVIEW.md
iteration: 1
findings_in_scope: 5
fixed: 5
skipped: 0
status: all_fixed
---

# Phase 11: Code Review Fix Report

**Fixed at:** 2026-10-29T17:04:43Z
**Source review:** .planning/phases/11-full-library-acceptance/11-REVIEW.md
**Iteration:** 1

**Summary:**
- Findings in scope: 5 (CR-01, CR-02, WR-01, WR-02, WR-03)
- Fixed: 5
- Skipped: 0

`workflow.use_worktrees` is `false` in `.planning/config.json`, so the fixes were edited and committed in the main
checkout on `phase/11-full-library-acceptance`; no worktree, temp branch or recovery sentinel was created. WR-04 to
WR-06 from the earlier review are not in the current REVIEW.md and were left alone. Info findings are out of scope.

## Fixed Issues

### CR-01: The sample-backed claim check never runs for `basic.*` tests

**Files modified:** `tools/compat_doc.cmake`, `tests/cmake/compat_doc_controls.cmake`
**Commit:** e4356c6
**Applied fix:** The two regex matches are now tried in turn (`if` / `elseif`), so a failed second match no longer
clears the captured sample for `basic.<sample>` tests. Control case 10 changes `rmtramdemo` from `supergame+ram` to
`linear` in a copy of the ledger and expects the generator to fail naming `basic.rmtramdemo`; run against the old
generator the new case fails, against the fixed one it passes. The review's second suggestion (fail when a `basic.*`
sample has no ledger row) was not applied: the claims loop walks every claim whatever ledger it is given, so the
small fixture ledgers in the controls (which hold no 7800basic rows) would then fail for an unrelated reason. A
removed ledger row is still caught by the other generator guards.

### CR-02: A `;` in `OUT` splits the `rm -rf` argument

**Files modified:** `tests/compat/local.cmake`, `tests/compat/local_fixture.cmake`
**Commit:** 3890f3a
**Applied fix:** `local.cmake` stops with the existing fixed message `local: cannot write the counts file` when
`OUT` holds a `;`, before the path is made absolute and before any step runs. Fixture run 12 passes
`<work>/victim;x/out.csv` with a `victim/keep` file beside it, expects the fixed fault and requires `keep` to
survive. Reverting only the guard makes the fixture fail.
Status: fixed. The fix is a guard on input, not a logic change to the step list; the steps stay string-joined, which
is safe now that no `;` can reach them.

### WR-01: An `OUT` that is an existing directory passes the writability probe

**Files modified:** `tests/compat/local.cmake`, `tests/compat/local_fixture.cmake`
**Commit:** 3810fa9
**Applied fix:** An `IS_DIRECTORY "${OUT}"` check, ahead of the scratch-tree check, stops with the same fixed
message. Fixture run 13 passes an existing directory as `OUT`, expects the fault and requires that no `.rows` tree
was made. Reverting the guard makes the fixture fail.

### WR-02: The library preset silently skips symlinked files with no hint

**Files modified:** `tests/compat/files.cmake`, `tests/compat/library.cmake`, `tests/compat/local_fixture.cmake`,
`docs/emulation-tests.md`
**Commit:** 6f0a1d4 (docs trimmed to the page budget in 639db89)
**Applied fix:** New `compat_unread_note` in `files.cmake` returns an empty clause or `; N links or unreadable
entries were skipped, so copy or hard-link files in`. `library.cmake` appends it to the missing-file error. The
fixture checks the helper for 0 and 2, and the existing source check of `library.cmake` now also requires the call
and the appended clause (new fault copy `d` drops it and must be caught). `docs/emulation-tests.md` says in the
library preset paragraph that links are skipped and counted and that a bracket, wildcard or semicolon in the
directory name, or an unmatched bracket or semicolon in a file name, fails with a fixed message; the `library.local`
paragraph now points to those rules. The page was 14 bytes under its 40960-byte budget, so the wording was trimmed to
fit rather than raising the budget (`docs.budgets` failed on the first wording, passes now). The missing-file error
text itself is exercised only by the source check and the helper test, not end to end: the `library` configure in a
tree that has `build/ci/built` finds every row's file, so there is no cheap way to reach that branch in `ci`.

### WR-03: A root name containing `*` or `?` matches other directories

**Files modified:** `tests/compat/files.cmake`, `tests/compat/local_fixture.cmake`, `docs/emulation-tests.md`
**Commit:** e19eb6b
**Applied fix:** `compat_name_check` now rejects `[`, `]`, `;`, `*` and `?`, with the message `the directory name holds
a bracket, wildcard or semicolon` (kept short because CMake wraps longer fatal messages, which broke the fixture's
exact-line matching). The fixture adds `dir*x` and `dir?x` to both the local pass and the library scan; the names need
not exist because the check fires before any glob, so the cases also run on Windows hosts. Reverting only the
pattern makes the fixture fail.

## Verification

Run in the main checkout (not a worktree), so the numbers reproduce from the tree:

- `cmake --workflow --preset ci` after the last commit: exit status 0, 100 percent of tests passed (about 32 s).
  The first run after WR-03 exited 8 because `docs.budgets` found `docs/emulation-tests.md` at 41301 bytes against
  40960; the trim in 639db89 fixed that and the rerun above is the one that counts.
- Focused: `ctest --preset ci -R '^title[.]local[.]fixture$'` passes after each local/files change, and was shown to
  fail with each guard reverted. `cmake -DTOOL=tools/compat_doc.cmake ... -P tests/cmake/compat_doc_controls.cmake`
  prints ten ok lines, and `tools/compat_doc.cmake -DCHECK=docs/compatibility.md` exits 0.
- The `library` preset itself was not run with a missing-file fault; `cmake --preset library` with a symlink-only
  `MUHTARI_ROM_DIR` configured cleanly because the `ci` build tree supplies every row's file.

No run URL: these are local runs on macOS arm64.

---

_Fixed: 2026-10-29T17:04:43Z_
_Fixer: Claude (gsd-code-fixer)_
_Iteration: 1_
