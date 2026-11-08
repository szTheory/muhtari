# Phase 12 Plan 22 Local Evidence

## Result

The owner-run diagnostic and its offline controls are implemented. The local wrapper is prepared and tested without launching RetroArch. The callback trace result and the cartridge's visible progression remain separate observations; the physical owner run is still pending. Phase 12 remains accepted 0/4 and PLAY-03 remains pending.

**Run URL:** `N/A-local`

## Final local checks

| Check | Command | Result |
| --- | --- | --- |
| Full native CI, package and release workflow | `cmake --workflow --preset ci` | Exit 0. All 700 regular tests and all 6 release checks passed; total workflow test time 71.99s and release tests 0.23s. |
| Focused offline wrapper and input-trace consumer | `ctest --test-dir build/ci -R '^policy\.retroarch\.preflight_api$' --output-on-failure` | Exit 0, 1/1 passed in 3.03s after the final cleanup changes. |
| Full-workflow focused consumer | Same preflight test during final full CI | Passed in 4.79s under parallel suite load; its unchanged CTest limit is 5s. |
| Safe preparation | `scripts/retroarch-manual-input.sh --prepare-only` | Exit 0. Owned history cartridge validated, isolated config/output prepared, and no frontend launched. |
| Documentation size | `ctest --test-dir build/ci -R '^docs\.budgets$' --output-on-failure` | Exit 0, 1/1 passed in 0.03s. `docs/testing.md` is 40,940 bytes against the 40,960-byte per-document limit. |
| Formatter and source whitespace | `xcrun clang-format --dry-run --Werror tests/libretro/input_trace_host.c tests/libretro/test_input_trace.c` and `git diff --check` | Exit 0 for both. The repository formatter is Apple clang-format 21 via `xcrun`. |
| POSIX wrapper syntax | `sh -n scripts/retroarch-manual-input.sh` | Exit 0. |
| Hygiene workflow | `cmake --workflow --preset hygiene` | Exit 0, all 30 checks passed in 46.93s. |
| Hygiene tree check | `scripts/hygiene.sh --tree` | Exit 0 in 43.74s; no findings. |
| Bounded security review | Independent review of launch, trace and cleanup boundaries | **SECURED**; all three plan threats reviewed, no remaining finding. |

## Timing fixes and investigation

- **Documentation budget:** Adding the owner instructions initially pushed `docs/testing.md` past the 40 KiB document cap. I shortened its Hosted RetroArch section; the final document is 40,940 bytes, leaving 20 bytes of headroom. The `docs.budgets` test passes.
- **Formatting:** The first format check found style drift in the new C changes. The `clang-format-21` executable name was not on `PATH`; `xcrun clang-format` supplied Apple clang-format 21. I formatted both changed C files, then the explicit dry-run and `hygiene.format` passed (1.71s in the final hygiene workflow).
- **Preflight CTest budget:** A full CI run with 300ms synthetic wall and 200ms cleanup controls hit the pre-existing 5s `policy.retroarch.preflight_api` timeout at 5.02s under parallel load. I reduced only the three test-child controls (normal timeout, interruption and forced-unreaped) to 150ms wall plus 150ms grace. Production limits remain 75 seconds plus 2 seconds, and the CTest timeout remains 5 seconds. The final focused run passed in 3.03s and the final full CI run passed this test in 4.79s.
- **Hygiene timeout:** The first parallel hygiene workflow hit the existing 60s `hygiene.tree` timeout at 60.01s while the remaining checks ran concurrently. The standalone tree scan passed in 43.74s; a full hygiene workflow rerun passed all 30 checks in 46.93s. No timeout was raised.

## Offline controls

The existing `policy.retroarch.preflight_api` test covers valid child argv and config, rejected missing/altered frame caps and wrong core/content/config, exec failure, existing-file and symlink refusal with sentinel preservation, a normal timed-out child, an interrupted child that continues SIGALRM through SIGTERM, and the bounded unreaped branch. The interruption case asserts the launcher sends SIGTERM then SIGKILL to the child, reaps it with SIGKILL status, and stays within its test wall/grace allowance. `--manual-core` accepts the valid neutral → both-held → released sequence and rejects its 14 offline fault traces. The wrapper's synthetic run exercises the actual wrapper, passive launcher and trace consumers.

The launcher's cleanup uses bounded `WNOHANG` polling after SIGKILL and reports `MANUAL_UNREAPED` with the child PID if the child is still not reaped by the deadline. It does not claim cleanup in that exceptional case. EINTR from `poll`, `read`, `waitpid` or `nanosleep` returns to a monotonic deadline check. The term-grace deadline advances to exact-child SIGKILL; only actual wait errors stop cleanup as wait failures.

## Scope and remaining evidence

This local execution launched RetroArch **0** times and did not use GUI/computer input, event injection, Accessibility/TCC, permissions, owner content/data, remote CI or publication. `--prepare-only` did not create frontend traces. The wrapper retains its output directory and removes only the generated temporary cartridge it owns.

The one remaining observation is the owner's physical Up+Z hold and release through installed RetroArch. A passing callback trace proves what the diagnostic core sampled; the owner must separately observe the waiting/held/complete cartridge colors. A missing transition is inconclusive and does not attribute a cause to focus, mapping, backend or the earlier permission denial. This does not satisfy released-core visible input, DMA, battery, aggregate or paired Playstead acceptance.

**Owner command:** `scripts/retroarch-manual-input.sh --run`

Run it from the owner's normal GUI-backed Terminal, then provide the callback verdict and the separate visible-color observation. The next OpenGSD route is `$gsd-verify-work 12` to reconcile the owner evidence; Phase 12 stays at accepted 0/4 until its remaining evidence gates are independently satisfied.

## Repository commit boundary

The first normal GSD commit attempt was:

`gsd_run query commit "feat(12-22): add bounded owner-run RetroArch input diagnostic" --files scripts/retroarch-manual-input.sh tests/libretro/input_trace_host.c tests/libretro/test_input_trace.c tests/libretro/input_trace.cmake tests/policy/tests.cmake docs/testing.md AGENTS.md .planning/phases/12-player-ready-delivery-and-playstead-bridge/12-22-LOCAL-EVIDENCE.md .planning/phases/12-player-ready-delivery-and-playstead-bridge/12-22-SUMMARY.md`

It returned `committed: false`, `reason: staging_failed` because the sandbox denied creation of `.git/index.lock`; no files were staged by that attempt. The root executor then used a scoped local repository-write escalation, and GSD committed the implementation/docs as `fa567595d68535d56013f8dd054057f1503a715c` (`feat(12-22): add bounded owner-run RetroArch input diagnostic`). The commit remains local; no push or merge occurred.
