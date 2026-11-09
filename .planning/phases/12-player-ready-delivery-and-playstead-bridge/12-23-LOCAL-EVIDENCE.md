# Phase 12 Plan 12-23 Local Evidence

## Scope and source

This plan repairs ownership-aware rollback for the passive input-trace launcher and strengthens its existing offline preflight consumer. It does not provide released-core RetroArch input, a release-identity receipt, or measured Windows publication improvement. Phase 12 remains blocked at accepted 0/4 with 2/5 roadmap truths verified.

- Branch: phase/12-player-ready-delivery-and-playstead-bridge
- Pre-edit source: 2361981988f3f9334d2c2eb69f7d3e3130237d3f
- Regression-test commit: 44ebfef57b066d5e9564112c84032bddf92fc0fd
- Repair commit: 28b2e60e78153519887ba06b4b896f7cc8d55e90
- Run URL: N/A (local)
- Protected verification report: frozen SHA-256 97c4dd55f212e69927c59ee11b5c36b441f38f2f3e4d9e2115d6a618004750ea

## Regression control

The strengthened existing consumer first failed against the old launcher after it created helper.trace and refused an existing core.trace. The failure was the explicit assertion that helper.trace remained after refused preflight; the existing core trace stayed intact, and only strict synthetic children ran.

Command:

~~~sh
ctest --test-dir build/ci -R '^policy\.retroarch\.preflight_api$' --no-tests=error --output-on-failure --output-junit build/phase12-gap23-evidence/red-before-final.xml
~~~

Exit: 8. The old-source run is recorded in ignored local scratch and classified as a valid RED result.

The repaired test passed with the existing registration and five-second deadline. It verifies pre-existing helper/core sentinels, symlink preservation, no child on refusal, removal of the invocation-owned partial helper before harness teardown, and a successful same-directory synthetic retry with complete paired traces.

Command:

~~~sh
ctest --test-dir build/ci -R '^policy\.retroarch\.preflight_api$' --no-tests=error --output-on-failure
~~~

Exit: 0; 1/1 passed in 2.27 seconds. CI configure and build also passed with the existing warnings-as-errors configuration.

## Full local gates

Four default-parallel invocations of cmake --workflow --preset ci exited 8 on timeout-only CTest failures. The timed-out count fell from 22 to 2, then to 1 in each of the final two runs. The repeated final failure was policy.retroarch.preflight_api at its unchanged five-second limit; focused serial reruns passed. The first failed set passed on a serial rerun, as did the later two-test and one-test subsets. These runs and their CTest receipts remain in ignored local scratch.

The CI test preset specifies automatic worker selection. The documented explicit CTest worker limit was used after the repeated timeout evidence; no test deadline, label, registration, inventory, assertion, or workflow policy changed.

- Configure: cmake --preset ci — exit 0.
- Build: cmake --build --preset ci — exit 0.
- Complete ordinary inventory: ctest --preset ci --parallel 2 --output-on-failure --output-log build/phase12-gap23-evidence/ordinary-ci-parallel-2.log --output-junit build/phase12-gap23-evidence/ordinary-ci-parallel-2.xml — exit 0; 700/700 passed; 182 seconds.
- Packages: cpack --preset ci — exit 0; three ZIPs generated: library, libretro, runner.
- Release checks: ctest --preset ci-release --parallel 2 --output-on-failure --output-log build/phase12-gap23-evidence/release-ci.log --output-junit build/phase12-gap23-evidence/release-ci.xml — exit 0; 6/6 passed.
- Hygiene: cmake --workflow --preset hygiene — exit 0; 30/30 checks passed; 40 seconds.

The explicit worker limit affects scheduling only. All ordinary, package, release, and hygiene inventory items ran with their existing deadlines and fault controls.

## Bounded review

The change keeps trace ownership inside the existing passive launcher. Exclusive file creation is tracked per invocation; rollback is anchored to the opened directory and checks device/inode identity before unlinking. Existing regular files, symlinks, and symlink targets are preserved. Successful and post-launch traces remain available. The focused consumer exercises refusal and subsequent successful use of the same private directory. No dependency, test registration, deadline, or public interface was added. No remaining issue was found in the changed ownership boundary; WR-01 is fixed by this source and its regression control.

No RetroArch app/content child, accessibility request, host run, remote operation, publication, or Phase 14 action occurred.

## Preservation checks

- The plan's path-limited git diff --check command exited 0.
- shasum -a 256 -c build/phase12-gap23-evidence/verification.sha256 exited 0 and reported the protected 12-VERIFICATION.md as OK against its original frozen SHA-256.
- Before the metadata commit, the staged-path allowlist contained only 12-REVIEW-DISPOSITION.md, 12-23-LOCAL-EVIDENCE.md, and 12-23-SUMMARY.md; the allowlist and git diff --cached --check both exited 0.

## Code review follow-up — 2026-11-09

The refreshed standard review found two defects in the passive launcher lifecycle controls. CR-01 was reproduced with a synthetic child that exits successfully while the parent observes it at the wall deadline: the old path returned success. CR-02 was reproduced by forcing the monotonic-clock read at cleanup to fail: the old path skipped signaling and could leave the exact child running. Both controls failed before their corresponding fixes.

The launcher now checks the wall deadline when a child is reaped and preserves timeout status through cleanup. If the monotonic clock is unavailable at cleanup, it sends TERM immediately and uses bounded wait attempts; it escalates to KILL when needed. The regression controls use deterministic observation hooks, leave the five-second CTest timeout unchanged, and clean up any child left by the deliberately failing pre-fix run.

- RED — `cmake --build build/ci --target muhtari-test-trace-host && ctest --test-dir build/ci -R '^policy.retroarch.preflight_api$' --output-on-failure`: exit 8 for the successful late-exit case before the wall-deadline fix.
- RED — the same focused command: exit 8 for the forced cleanup-clock failure before the clock-independent signal/reap path.
- Focused GREEN — `cmake --build build/ci --target muhtari-test-trace-host && ctest --test-dir build/ci -R '^policy.retroarch.preflight_api$' --output-on-failure`: exit 0; 1/1 passed in 2.55 seconds.
- Full local gate — `cmake --workflow --preset ci`: exit 0; 700/700 ordinary tests, three packages generated, six release checks passed. Run URL: N/A-local.
- `git diff --check`: exit 0.

No host, remote workflow, publication, or RetroArch launch occurred. Phase 12 remains blocked at accepted 0/4 until its missing released-core input and other acceptance evidence are supplied.

## Code review follow-up — post-create identity failure — 2026-11-09

The re-review found that a trace entry created with `O_EXCL` could remain if its descriptor identity check failed immediately afterward. Added deterministic helper/core identity-failure controls. They run under `umask(0077)`, assert rollback leaves both trace names absent and no child launched, then retry in the same directory under the same umask. The retry asserts owner-only regular files at mode `0600` and complete paired traces.

- RED — `cmake --build build/ci --target muhtari-test-trace-host && ctest --test-dir build/ci -R '^policy.retroarch.preflight_api$' --output-on-failure`: exit 8; injected helper identity failure left `helper.trace` behind.
- Focused GREEN — the same command after rollback and retry fixes: exit 0; 1/1 passed in 2.61 seconds, including helper and core identity-failure cases.
- Full local gate — `cmake --workflow --preset ci`: exit 0; 700/700 ordinary tests, three packages generated, six release checks passed. Run URL: N/A-local.
- `scripts/hygiene.sh --tree`: exit 0. `git diff --check`: exit 0.
- Final standard code review: clean; no remaining findings.

Rollback uses the already-opened, owner-only trace directory. When descriptor identity is unavailable, it removes only a regular file owned by the current user with no group/other permissions; the normal device/inode-checked path remains in place when identity is known. No child had been started at this setup failure point.

No RetroArch launch, host action, remote workflow, publication, or merge occurred. Phase 12 remains blocked at accepted 0/4 pending its separate release-input and acceptance evidence.
