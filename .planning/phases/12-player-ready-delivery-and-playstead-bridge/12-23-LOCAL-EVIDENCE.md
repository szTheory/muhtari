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
