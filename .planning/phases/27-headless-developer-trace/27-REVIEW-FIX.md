# Phase 27: Code Review Fixes

## Findings and changes

| Review | Finding | Outcome |
|---------|---------|---------|
| Initial review | CR-01: Windows junction aliases could let one output replace another. | Fixed by resolving existing parent directories through Windows directory handles; added a Windows junction runner control. |
| Initial review | WR-01: `MUHTARI_TRACE_LOOP=OFF` with tests enabled failed later at link time. | Fixed with an early CMake configuration error and controls for both rejected and valid option combinations. |
| Second review | CR-02: ordinary Win32 paths ending in ASCII dots or spaces could alias paths without those suffixes. | Fixed by trimming those suffixes from the normalized Windows basename; added Windows real-runner cases for dot and space aliases. |
| Final review | WR-01: explicit `\\?\` extended paths preserve suffixes, so unconditional trimming can reject distinct outputs. | Deferred as a compatibility follow-up. This is a conservative rejection; it does not create an overwrite path. See [`27-REVIEW-DISPOSITION.md`](27-REVIEW-DISPOSITION.md). |

The final review report confirms the junction, ordinary suffix, and trace-off configuration findings are fixed. It contains no critical findings and one deferred warning. The reviewer could not compile or execute Windows-specific code on this Darwin arm64 host.

## Verification

| Command | Exit | Result |
|---------|------|--------|
| `ctest --test-dir build/ci --output-on-failure -R '^(runner[.]trace[.](paths|disabled_config))$'` | 0 | Both path and trace-disabled configuration tests passed on this host. |
| `cmake --workflow --preset ci` | 0 | 736 tests, all 8 release checks and package step passed on the reviewed source. |
| `cmake --workflow --preset hygiene` | 0 | All 30 hygiene and policy checks passed. |
| `/opt/homebrew/opt/llvm@21/bin/clang-format --style=file --dry-run --Werror runner/main.c tests/runner/test_save.c` | 0 | Changed C files passed formatting. |
| `git diff --check` | 0 | No whitespace errors. |

These local checks do not establish Windows runtime behavior or the authentic Linux Cachegrind measurement. Those remain for the existing Windows and Linux perf lanes at the authorized delivery CI point.
