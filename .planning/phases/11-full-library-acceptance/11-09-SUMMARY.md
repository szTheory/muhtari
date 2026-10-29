---
phase: 11-full-library-acceptance
plan: "09"
subsystem: compatibility-local-pass
tags: [local-pass, library-preset, file-rule, privacy, gap-closure]
status: complete
requires:
  - phase: 11-full-library-acceptance
    provides: "The local pass, its fixture and the library preset (earlier plans of this phase), verification gap 4 (11-VERIFICATION.md), the regenerated compatibility page (plan 11-10)"
provides:
  - "tests/compat/files.cmake: compat_name_check and compat_files, the readable-file rule shared by the local pass and the library preset's configure-time scans, plus a -P entry that prints '<prefix>: K files kept, U unread'"
  - "A local pass that counts links and unreadable entries without reading or naming them, prints 'local: U files unread (links or unreadable)' beside the unchanged counts line, and stops with fixed messages on a bracket or semicolon in the directory name or an unmatched bracket or semicolon in a file name"
  - "An absolute OUT and a scratch tree named '<OUT>.rows' that the pass replaces only when absent or marked by '.local-pass'"
  - "library.cmake scans through the same rule, so no entry of MUHTARI_ROM_DIR puts a path into a configure error"
  - "title.local.fixture grown to eleven driver runs, three library-prefix scans and a normalising source check of library.cmake (four tolerated edits, three fault copies)"
affects: [compatibility-local-pass, library-preset, phase-11-gap-round]
actuals:
  tokens: 6277
  tasks: 3
  commits: 3
plan_head_before: c9c2e150a06689c63399a25db7ec88ccf63a5734
plan_head_after: c12c3dd5e2289c59324a2f36a37fedb3bd2a821c
commits: 3
tech-stack:
  added: []
  patterns:
    - "One CMake -P helper holds a file rule that two scanners share, and the test drives it under each scanner's prefix"
    - "A source check that normalises comments, case and whitespace before matching holds the fault direction of call sites a CI job exercises only on the pass path"
key-files:
  created:
    - tests/compat/files.cmake
  modified:
    - tests/compat/local.cmake
    - tests/compat/local_fixture.cmake
    - tests/compat/library.cmake
    - docs/emulation-tests.md
key-decisions:
  - "Every symlink is counted unread and never followed, as the verification asked; the rule is one condition in compat_files and reversible."
  - "compat_files reads its roots from ARGV (not ARGN) and local.cmake quotes DIR, so a ';' inside a directory name stays one root and reaches compat_name_check."
  - "The file-name test is 'holds a semicolon or is not absolute', not 'does not exist': CMake 4.4 reports EXISTS false for a write-only file, so an existence test would fault on the very unreadable file the rule must count. A name holds no '/', so the piece after a split is never absolute and a merged element holds a ';'."
  - "compat_files sorts within each root and keeps the roots in the order given, so the library preset still searches MUHTARI_ROM_DIR before build/ci/built before tests/diag when two files share a SHA-1."
requirements-completed: []
coverage:
  - id: C1
    description: "A dangling link and a valid link in the dump directory are counted unread, never followed and never named, through the real local driver"
    requirement: COMP-03
    verification:
      - kind: integration
        ref: "ctest --preset ci -R '^title[.]local[.]fixture$' -FS '.*' -V exit 0, prints 'local: 3 files unread (links or unreadable)' and 'local fixture: eleven runs as expected'; N/A-local, no run URL"
        status: pass
      - kind: other
        ref: "fault direction - with the IS_SYMLINK branch of compat_files replaced by if(FALSE), ctest --preset ci -R '^title[.]local[.]fixture$' exited 8 (base: no unread line for 3); restored, exit 0"
        status: pass
    human_judgment: false
  - id: C2
    description: "A write-only file is counted unread and not read, so file(SIZE) never prints its path; on a lane that runs as root or on Windows the file is readable and the fixture says so"
    requirement: COMP-03
    verification:
      - kind: integration
        ref: "ctest --preset ci -R '^title[.]local[.]fixture$' exit 0 on this macOS user run (IS_READABLE reports the file unreadable, the log has no 'write-only file is readable' line); the branch is unproven on a root-run lane, which prints 'local fixture: the write-only file is readable on this host'; N/A-local, no run URL"
        status: pass
      - kind: other
        ref: "fault direction - with the IS_READABLE test removed from compat_files, the same ctest command exited 8 (base: the output names a file, a path or a hash); restored, exit 0"
        status: pass
    human_judgment: false
  - id: C3
    description: "A semicolon name, an unmatched-bracket name and a bracketed directory name stop the pass with fixed messages and no path; a balanced-bracket name is read normally"
    requirement: COMP-03
    verification:
      - kind: integration
        ref: "title.local.fixture runs 7, 8 and 9 (semi, bracket, dir[x) expect the fixed local messages and exit non-zero; the base run still reports 1 of 2 present with 'simple [!].a78' evaluated; ctest --preset ci -R '^title[.]local[.]fixture$' exit 0; N/A-local, no run URL"
        status: pass
    human_judgment: false
  - id: C4
    description: "A relative OUT is taken from the working directory and cannot delete a sibling directory named local; a foreign OUT.rows tree is refused and kept; the pass replaces its own marked tree on a re-run"
    requirement: COMP-03
    verification:
      - kind: integration
        ref: "title.local.fixture runs 10 and 11 plus the re-run of the base case; ctest --preset ci -R '^title[.]local[.]fixture$' exit 0; N/A-local, no run URL"
        status: pass
      - kind: other
        ref: "fault direction - with the old scratch rule (OUT parent plus /local, no guard) in a scratchpad copy, the fixture exited 1 (relative - the run deleted a directory beside OUT); with only the marker guard removed, it exited 1 (foreign - a harness fault exited 0); the committed local.cmake, exit 0"
        status: pass
    human_judgment: false
  - id: C5
    description: "The shared rule under the library prefix passes and faults as the local pass does, and library.cmake's two configure-time scans are held to the rule"
    requirement: COMP-03
    verification:
      - kind: integration
        ref: "title.local.fixture three library-prefix scans (dir counted 5 kept, 3 unread; semi and dir[x refused with the fixed library messages) and prints 'local fixture: three library scans as expected'; N/A-local, no run URL"
        status: pass
      - kind: integration
        ref: "title.local.fixture normalising source check of library.cmake - passes the committed file and four tolerated edits (reflowed call, spaced call, renamed loop variable, trailing comment mentioning file(SIZE) and fails three fault copies (raw glob plus file(SIZE) appended, name check removed, SHA-1 loop over library_roots appended); prints 'local fixture: library.cmake scans only through the shared rule'; N/A-local, no run URL"
        status: pass
      - kind: other
        ref: "fault direction - hard-coding 'local:' in compat_name_check made the fixture exit 8 on the bracketed-directory library scan; HEAD's library.cmake restored in the working tree made it exit 8 with 'local fixture: library source: files.cmake is not included'; the new file copied back, git diff --stat tests/compat/library.cmake showed 9 insertions and 21 deletions both before and after"
        status: pass
      - kind: integration
        ref: "the library CI job runs both configure-time call sites on the pass path on every heavy change; status pending-ship, N/A-local until a run URL exists"
        status: pending
      - kind: other
        ref: "one-shot, a scratch directory holding only a dangling link - MUHTARI_ROM_DIR=<dir> cmake --workflow --preset library exit 0, ctest --test-dir build/library -R '^library[.]local$' -V printed 'local: 1 files unread (links or unreadable)' and 0 of 204 present, and the workflow output held no occurrence of the scratch path; one-shot, a directory holding only a file named a;b.a78 - MUHTARI_ROM_DIR=<dir> cmake --preset library exit 1 with 'library: a file name holds an unmatched bracket or a semicolon' and no scratch path in the output; build/library then reconfigured with MUHTARI_ROM_DIR=build/ci/built, workflow exit 0"
        status: pass
    human_judgment: false
  - id: C6
    description: "The doc describes the unread line, the no-symlink advice and the odd-name stop, within the byte cap"
    requirement: COMP-03
    verification:
      - kind: other
        ref: "ctest --preset ci -R '^(docs[.]budgets|title[.]local[.]fixture)$' exit 0; docs/emulation-tests.md is 40,946 bytes of 40,960; grep -ciF 'copy or hard-link' and grep -cF 'files unread (links or unreadable)' each print 1; the agent read the paragraph against local.cmake and files.cmake"
        status: pass
    human_judgment: false
---

# Phase 11 Plan 09: Local pass and library scan file rule Summary

The local pass and the library preset's configure-time scans now share one file rule: links and unreadable entries are counted but never read or named, names a CMake list cannot carry stop the run with a fixed message, and the pass deletes only a scratch tree it made.

## Accomplishments

- Task 1 (tracer): `tests/compat/files.cmake` with `compat_files`; `local.cmake` uses it and prints `local: U files unread (links or unreadable)` before the unchanged counts line; the fixture gains a dangling link, a valid link, a write-only file and `simple [!].a78`. This host prints `local: 3 files unread`. The local.cmake header's "An unknown, present row is judged" wording now matches the code.
- Task 2: `compat_name_check`; OUT made absolute; the scratch tree is `<OUT>.rows` with a `.local-pass` marker; fixture runs 7 to 11 (semicolon name, unmatched bracket, bracketed directory, relative OUT with a sentinel `sub/local/keep`, foreign `.rows` tree and a re-run over the pass's own tree).
- Task 3: `library.cmake` includes `files.cmake`, name-checks the raw MUHTARI_ROM_DIR value before it is made absolute and runs both scans through `compat_files`; the -P entry; three library-prefix scans; `library_source_fault` and its normaliser, with four tolerance copies and three fault copies; the doc paragraph.
- Phase gate for this plan: `cmake --workflow --preset ci` exit 0 (673 of 673 tests, then 6 of 6 release tests). I read the fixture's child outputs through its allowed-line checks and the one-shot outputs by eye: no line names a file, a path or a hash (rule 6 judgement, recorded).

## Commands

| Command | Exit | Run URL |
|---|---|---|
| cmake --preset ci && cmake --build --preset ci | 0 | N/A-local |
| ctest --preset ci -R '^title[.]local[.]fixture$' --no-tests=error --output-on-failure | 0 | N/A-local |
| ctest --preset ci -R '^(docs[.]budgets|title[.]local[.]fixture)$' --no-tests=error | 0 (5 of 5) | N/A-local |
| fault checks of the three tasks (see coverage C1 to C5) | 8 or 1, each restored to 0 | N/A-local |
| MUHTARI_ROM_DIR=build/ci/built cmake --workflow --preset library | 0 (47 of 47); library.local printed 'local: 0 files unread' and 'local: 0 of 204 present, 0 pass, 0 fail, 204 absent' | N/A-local |
| dangling-link and a;b.a78 one-shots | 0 and 1 as specified | N/A-local |
| cmake --workflow --preset ci | 0 | N/A-local |
| cmake --build --preset wasm32 | N/A-local: build/wasm32 holds no build.ninja (not configured), so the wasm32 and ci-msvc lanes are left to CI at ship | N/A-local |
| find build/ci/built tests/diag -type l | prints nothing | N/A-local |

At ship, check the ci-msvc lane log for `local fixture: links not created on this host` (the link branch is unproven there if symbolic-link creation fails) and any root-run lane log for `the write-only file is readable on this host`.

## Documentation trims

docs/emulation-tests.md was 40,934 of 40,960 bytes; the added sentences were paid for in the same paragraph and `## ledger`, ending at 40,946. Removed text:

- ledger Tiers: " There are no `nightly` rows yet."
- ledger library preset: " (a headered file by `sha1_a78`, a headerless one by `sha1_raw`)" (the columns paragraph already says it)
- ledger ratchet bullet: ", so a change that improves a row updates the ledger in its pull request"
- dumps paragraph: "The preset sets `outputOnFailure`, so the second command prints the counts line, without the dump directory." (the paragraph still states that the counts line is the output)

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Bug] The name test used EXISTS, which CMake reports false for a write-only file**
- **Found during:** Task 1
- **Issue:** The plan's test `NOT EXISTS` on each listed element faulted on `locked.a78`, the unreadable file the rule must count (EXISTS, IS_READABLE and file(TIMESTAMP) all see a write-only file as absent on CMake 4.4).
- **Fix:** A listed element is a name the list could not carry when it holds ';' or is not absolute. A file name holds no '/', so the piece after a split is never absolute and a merged element holds a ';'. Reproduced for `a;`, `;a`, `a;b`, an unmatched `[` followed by another file, and a lone unmatched `[` or `]` (carried, read normally).
- **Files modified:** tests/compat/files.cmake
- **Commit:** 8e9164d

**2. [Rule 2 - Missing critical functionality] A ';' in DIR split into two roots that each passed the name check**
- **Found during:** Task 2
- **Issue:** `compat_files(local files unread ${DIR})` with an unquoted DIR and an ARGN loop turns a ';' inside DIR into two roots, so compat_name_check never saw the ';'.
- **Fix:** local.cmake quotes DIR and compat_files reads its roots by index from ARGV, so a root with a ';' stays whole and is refused.
- **Files modified:** tests/compat/files.cmake, tests/compat/local.cmake
- **Commit:** 4a577b8

**3. [Rule 1 - Bug] A global sort would have reordered the library preset's roots**
- **Found during:** Task 3
- **Issue:** The roots are searched in order (the first file per SHA-1 wins, MUHTARI_ROM_DIR before build/ci/built before tests/diag); sorting the merged list put them in path order.
- **Fix:** compat_files sorts within each root and appends root by root; REMOVE_DUPLICATES keeps a repeated file at its first place. One root (the local pass) is unchanged.
- **Files modified:** tests/compat/files.cmake
- **Commit:** c12c3dd

**4. [Rule 3 - Blocking] The acceptance grep needs the unread line in the fixture's -V output**
- **Found during:** Task 1
- **Issue:** The fixture prints only its own status lines, so `local: 3 files unread (links or unreadable)` never reached `ctest -V`.
- **Fix:** After the base run the fixture prints that run's unread line, extracted from the driver's output by regular expression.
- **Files modified:** tests/compat/local_fixture.cmake
- **Commit:** 8e9164d

**5. Commit trailer.** The project notes asked for a `Claude Opus 5.5` trailer; the commits carry `Claude Sonnet 5.5`, the model that made them, per the session's attribution reminder.

Otherwise the plan ran as written. The source check adds one message the plan did not name, `library source: a tolerance copy did not change`, so a tolerance copy that matched nothing cannot pass vacuously.

## Known Stubs

None.

## Threat Flags

None. The files add no network, auth or schema surface; they remove a path leak.

## Self-Check: PASSED

- FOUND: tests/compat/files.cmake, tests/compat/local.cmake, tests/compat/local_fixture.cmake, tests/compat/library.cmake, docs/emulation-tests.md
- FOUND commits: 8e9164d, 4a577b8, c12c3dd
