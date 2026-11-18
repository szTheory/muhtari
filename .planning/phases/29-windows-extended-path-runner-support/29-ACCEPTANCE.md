# Phase 29 Windows path acceptance

This matrix records evidence by execution environment. A test implementation or a passing portable test
does not accept Windows behavior. Windows rows remain pending until the actual runner, SMB share lifecycle
and required cases pass on that architecture.

| Requirement / case group | Local | Windows x64 | Windows ARM64 |
|---|---|---|---|
| WINPATH-01: cartridge/probe, movie, BIOS, HSC firmware, state input, save directory, state output, frame PNG and trace through drive and extended UNC paths; non-ACP Unicode, literal dot/space suffixes and a path beyond `MAX_PATH` | Portable role/path tests pass in the corrected local workflow on source `0701a13`; Darwin does not execute the Win32 branch | Pending source-qualified `runner.trace.paths` job and run URL | Pending source-qualified `runner.trace.paths` job and run URL |
| WINPATH-02: suffix-distinct destinations, existing-file and hard-link identity, parent-junction and mixed ordinary/extended aliases, input/output and output/output overlaps | Portable collision matrix passes on source `0701a13`; actual Windows aliases not run | Pending actual-process alias matrix and run URL | Pending actual-process alias matrix and run URL |
| WINPATH-03: state, PNG, trace and derived battery success bytes; blocked create/write/replace preserves previous bytes and removes owned temporary files | Portable fault and byte-preservation tests pass on source `0701a13`; actual Windows output cases not run | Pending actual-process output matrix and run URL | Pending actual-process output matrix and run URL |
| WINPATH-04: ordinary-path controls, real temporary SMB share create/read/write/remove, filesystem/component measurements, full/derived/temp path lengths, and user documentation | Corrected configured local workflow passed 736/736 tests plus all 8 release checks; no Windows SMB measurements are available | Pending SMB lifecycle, measurements and run URL | Pending SMB lifecycle, measurements and run URL |

## Evidence record

- Initial local checkout source: `ff8c312e1998c23452d2cd5acdddb33d7f62526c`.
- Local focused command: `git diff --check && cmake --preset ci && cmake --build --preset ci --target muhtari-run muhtari-test-save-runner muhtari-test-trace && ctest --test-dir build/ci --output-on-failure -R '^runner[.]trace[.](paths|io)$|^runner[.]state[.](roundtrip|files|options)$|^runner[.]save[.](roundtrip|faults|options)$|^docs[.]budgets$'` — exit 0, 9/9 tests passed.
- Local Task 2 preflights — exit 0: LLVM 21 format and `git diff --check`; `scripts/hygiene.sh --tree`; `cmake --preset ci`; `ctest --test-dir build/ci --show-only -R '^runner[.]trace[.]paths$'`; and `ctest --test-dir build/ci --output-on-failure -R '^docs[.]budgets$'`.
- Initial configured local workflow on `ff8c312` — `cmake --workflow --preset ci`, exit 8; build passed, 735/736 tests passed, with `runner.movie.owner` the sole failure. CTest evidence remains under ignored `build/ci/Testing/Temporary/LastTest.log`, `LastTestsFailed.log` and `build/ci/junit.xml`. Review traced it to the test harness rejecting libc-owned `realpath` memory; the test-only wrapper now tracks that allocation.
- Corrected source: commit `0701a13`. The full workflow ran on the same source files before they were committed, with `HEAD` at `ff8c312`; no source files changed between the run and commit.
- Corrected local workflow: `cmake --workflow --preset ci` — exit 0; 736/736 tests passed, packaging completed, and the configured release test preset passed 8/8. The full CTest suite took 30.38 seconds; the release check took 0.78 seconds.
- After review fixes, `ctest --test-dir build/ci --output-on-failure -R '^runner[.]movie[.]owner$|^runner[.]trace[.]paths$|^runner[.]save[.]faults$'` — exit 0, 4/4 including its fixture setup.
- Corrected-source preflights — exit 0: LLVM 21 format, `git diff --check`, `scripts/hygiene.sh --tree`, `cmake --preset ci`, `ctest --test-dir build/ci --show-only -R '^runner[.]trace[.]paths$'`, and `ctest --test-dir build/ci --output-on-failure -R '^docs[.]budgets$'`.
- First PR CI run, source `c800142df5ca9bffb3ffb46ada21d371d1789086`, [run 44403010988](https://github.com/szTheory/muhtari/actions/runs/44403010988): Windows x64 and ARM64 both exited 2 during the `ci-msvc` build. MSVC warning C4456 was promoted to an error because a nested `error` local shadowed the saved `errno` local in `save_read_open`; neither Windows runner test inventory nor the path matrix ran. The fix renames the nested local to `fdopen_error`; it awaits validation on a new PR run.
- Corrected MSVC source: commit `dc7328a84135a6bf4dfdb3909ad4bb7df7d1b9df`. The local workflow ran on the same `runner/main.c` contents immediately before this commit; no product source changed between the run and commit.
- Corrected local workflow on source `dc7328a`: `cmake --workflow --preset ci` — exit 0; 736/736 tests passed, all three macOS packages were generated, and the release preset passed 8/8. CTest took 34.38 seconds; the release tests took 0.92 seconds.
- Corrected-source focused checks: LLVM 21 `clang-format --dry-run --Werror runner/main.c`, `git diff --check`, `cmake --preset ci`, and `ctest --test-dir build/ci --output-on-failure -R '^runner[.]movie[.]owner$|^runner[.]trace[.]paths$|^runner[.]save[.]faults$|^docs[.]budgets$'` — all exit 0; 5/5 tests passed including fixture setup.
- Windows x64 built source, exact test command, exit status, measurements and job/run URL: pending.
- Windows ARM64 built source, exact test command, exit status, measurements and job/run URL: pending.
- The Windows driver reports UTF-16 code-unit counts, filesystem and reported component limit, the full test-directory and longest final-output lengths, and conservative maximum sibling-temp candidate lengths. Its success line is surfaced by `runner.trace.paths`; the PowerShell wrapper reports the completed SMB create/read/write/remove lifecycle only after share cleanup succeeds.
- The Windows test must also prove no test-owned files or share remain. A missing SMB capability, self-skip, incomplete job inventory, failed cleanup or source mismatch leaves the affected requirement pending.
- CI queue, job duration, critical path and cancelled work: pending the existing current-milestone PR runs. No topology, cache or deadline change is proposed by this plan.

No requirement is accepted by this initial record.
