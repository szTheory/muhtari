# Phase 27: Code Review Fixes

## Findings and changes

| Review | Finding | Outcome |
|---------|---------|---------|
| Initial review | CR-01: Windows junction aliases could let one output replace another. | Fixed by resolving existing parent directories through Windows directory handles; added a Windows junction runner control. |
| Initial review | WR-01: `MUHTARI_TRACE_LOOP=OFF` with tests enabled failed later at link time. | Fixed with an early CMake configuration error and controls for both rejected and valid option combinations. |
| Second review | CR-02: ordinary Win32 paths ending in ASCII dots or spaces could alias paths without those suffixes. | Fixed by trimming those suffixes from the normalized Windows basename; added Windows real-runner cases for dot and space aliases. |
| Final review | WR-01: explicit `\\?\` extended paths preserve suffixes, so unconditional trimming can reject distinct outputs. | Deferred as a compatibility follow-up. This is a conservative rejection; it does not create an overwrite path. See [`27-REVIEW-DISPOSITION.md`](27-REVIEW-DISPOSITION.md). |
| Compiler portability follow-up review | WR-01: test helper accepted unexpected trailing arguments. | Fixed with mode-specific argument counts and 11 boundary self-checks; the clean re-review is recorded in [`27-REVIEW.md`](27-REVIEW.md). |

Finding IDs are scoped to a review. The compiler follow-up's WR-01 is fixed; the disposition ledger's WR-01 refers to the earlier extended Windows path warning and remains deferred.

The earlier Windows-path review found no critical issues and one deferred warning; it could not compile or execute Windows-specific code on this Darwin arm64 host. The current compiler-portability follow-up review is clean.

## Verification

| Command | Exit | Result |
|---------|------|--------|
| `ctest --test-dir build/ci --output-on-failure -R '^(runner[.]trace[.](paths|disabled_config))$'` | 0 | Both path and trace-disabled configuration tests passed on this host. |
| `cmake --workflow --preset ci` | 0 | 736 tests, all 8 release checks and package step passed on the reviewed source. |
| `cmake --workflow --preset ci` | 0 | Fresh run after the compiler portability follow-up: all 736 tests, packages, and all 8 release checks passed. |
| `ctest --test-dir build/ci --output-on-failure -R '^(runner[.]save[.]faults|runner[.]trace[.]paths)$'` | 0 | Both focused runner checks passed after adding mode-specific argument validation. |
| `cmake --workflow --preset hygiene` | 0 | All 30 hygiene and policy checks passed. |
| `/opt/homebrew/opt/llvm@21/bin/clang-format --style=file --dry-run --Werror runner/main.c tests/runner/test_save.c` | 0 | Changed C files passed formatting. |
| `git diff --check` | 0 | No whitespace errors. |

These local checks do not establish Windows runtime behavior or the authentic Linux Cachegrind measurement. Those remain for the existing Windows and Linux perf lanes at the authorized delivery CI point.
