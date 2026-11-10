# Phase 14 Plan 27: Recovery Evidence

**Outcome:** The single checksum-validating fetch replaced the ignored Linux Wasmtime executable with the pinned Darwin arm64 executable. Task 1 identity checks, tracked-preservation checks, and the one automatic local CI workflow all passed. Plan 14-25 is now eligible for its separately selected local preparation through its owner checkpoint.

## Scope and execution-start baseline

This is the bounded local recovery authorized for Plan 14-27. It changes no source, pin, fetch script, test, inventory, preset, deadline or CI policy. The run is local; remote CI, push, dispatch, Docker, RetroArch and host operations are `N/A-local` and were not attempted. No project configure, build, CTest or workflow was manually invoked. The sole project-network command was the one pinned artifact fetch below.

Execution began at `2026-11-05T13:39:32Z` on Darwin arm64, branch `phase/14-proven-frontend-states-and-responsive-play`, HEAD `6d143498bcd03661edf997c980891c58b18828f1`. The starting worktree had an owner modification to `.planning/STATE.md`; its bytes are included in the preserved starting-state baseline and remain unstaged. The index and non-ignored untracked baselines were empty.

Ignored runtime receipts are under `build/phase14-recovery27-evidence/`. Before acquisition, the receipt directory was absent, its destination ancestry was checked for symlinks, and Git confirmed its contents are ignored. The tracked, cached and untracked baselines use repository-root pathspecs excluding only this plan's `14-27-EVIDENCE.md` and `14-27-SUMMARY.md`. The 17 protected snapshots in `protected-before/` were each byte-compared to their source before acquisition; they include the historical Plan 14-24/14-26 records, Plan 14-25 plan, pins and fetch/build inputs, project config, and planning state/roadmap/requirements. The baseline artifacts are not tracked.

The pre-acquisition host/pin/destination/snapshot command from Task 1 exited 0. The unique `wasmtime-aarch64-macos` row matched version `48.0.6`, URL `https://github.com/bytecodealliance/wasmtime/releases/download/v48.0.6/wasmtime-v48.0.6-aarch64-macos.tar.xz`, and archive SHA-256 `0b24079ba8c8a23df9cd9a11e40f0b5edc95398a70a7cdec9286133f18701c69`.

## Artifact acquisition and native identity

Before acquisition, read-only inspection found the ignored executable was a Linux AArch64 ELF file (ELF machine 183), SHA-256 `1da96b596266ad432b6e652aefbbc507de38795ed3197f859ac8197c1c5a276a`. It was not executed or separately modified.

| Command | Exit | Result |
| --- | ---: | --- |
| `env -u PINS sh scripts/fetch-pin.sh wasmtime-aarch64-macos build/pins/wasmtime` | 0 | Invoked exactly once. The existing fetch script selected the exact `pins.txt` row, retained its `curl --retry 3` transport behavior, validated the downloaded archive against the archive SHA-256 above, and extracted in place. Output: `build/pins/wasmtime`. |
| Task 1 read-only Python Mach-O identity check | 0 | `build/pins/wasmtime/wasmtime` is a non-symlink executable, little-endian 64-bit Mach-O arm64 (`cputype=0x0100000c`, executable file type 2). Observed extracted executable SHA-256: `9dad9691c32e097241d1f70371e36bea39ec3d51499134133340f77ecd9e6758`. This is the executable digest, distinct from the archive digest. |

## Automatic local workflow gate

| Command | Exit | Result |
| --- | ---: | --- |
| `cmake --workflow --preset ci` | 0 | Invoked once by execute-phase after the Plan 14-27 summary and preservation checks. Configure, build, and packaging completed; CTest passed 695/695, and the release-test suite passed 6/6. Three expected macOS arm64 packages were produced. Run URL: `N/A-local`. |

This is the sole workflow invocation authorized for Plan 14-27. It is complete and must not be repeated. No remote CI, push, workflow dispatch, Docker, RetroArch, or host action occurred.

## Preserved acceptance and follow-on boundaries

Plan 14-24 remains halted with zero qualification, Docker contact, container, build, CTest and protected-workload starts; its failed historical workflow is not retried. Plan 14-26 remains halted after its one failed automatic gate, which is spent. Neither historical record is reopened or changed by this task.

Plan 14-25 remains the separate follow-on local scope, with `depends_on: ["14-27"]`, its existing native source/actual-parent gates and its blocking owner decision before any push, remote dispatch or host action. This recovery supplies none of its host evidence or authorization. FRONT-02, FRONT-03 and FRONT-04 remain pending; this plan does not accept Phase 14.

## Task 2 preservation review

The preserved Plan 14-24 records say its malformed local MAME digest stopped preflight before qualification; qualification, Docker, container, build, CTest and protected-workload starts were zero. Its one historical automatic workflow exited 1 at configure on the unsorted inventory and is not retried. The preserved Plan 14-26 evidence says its one automatic workflow exited 8 after configure/build, when CTest could not execute the Linux ELF on Darwin arm64; that gate is spent and is not retried. No old controller, Docker command, process probe, remote tool or prior workflow was run for this review.

Plan 14-25 remains `wave: 2` with `depends_on: ["14-27"]`. Its existing source and actual-parent native gates remain intact. Its `gate="blocking-human"` decision asks for an explicit choice on one exact push and one full `workflow_dispatch` attempt, within the existing stated cost ceiling, before publication or host work. A passing recovery gate supplies no such consent and does not authorize remote or host activity.

| Read-only check | Exit | Result |
| --- | ---: | --- |
| Task 2 tracked comparator: `git diff --binary --no-ext-diff --no-renames <start-head> -- .` with only `14-27-EVIDENCE.md` and `14-27-SUMMARY.md` excluded, compared byte-for-byte to `tracked-before.patch` | 0 | The execution-start tracked diff, including the owner's starting `.planning/STATE.md` change, is unchanged outside the two new recovery outputs. |
| Task 2 cached comparator: `git diff --cached --binary --no-ext-diff --no-renames <start-head> -- .` with the same two exclusions, compared to `index-before.patch` | 0 | The execution-start index state is unchanged. |
| Task 2 untracked comparator: `git ls-files --others --exclude-standard -- .` with the same two exclusions, compared to `untracked-before.txt` | 0 | No unrelated non-ignored untracked path changed. |
| Task 2 `cmp` loop over all 17 entries in its protected-file list | 0 | Plan 14-24/14-26 history, Plan 14-25, pins/fetch/build inputs, config, STATE, ROADMAP and REQUIREMENTS match their execution-start snapshots byte-for-byte. |
| `test -s 14-27-EVIDENCE.md`, exact Plan 14-25 dependency check, and `git diff --check <start-head>` | 0 | Evidence exists, the dependency is unchanged, and the scoped diff has no whitespace errors. |

The first shell wrapper attempt for the three Python comparators exited 1 in zsh with `unmatched '`, before Python ran. Corrected wrappers ran the specified read-only comparisons, each with exit 0; no project or Git state was changed by the malformed wrapper. These task checks are local (`N/A-local`); the later automatic workflow result is recorded above.

The preservation review and the single automatic post-wave workflow passed. The workflow exited 0 after configure, build, packaging, CTest (695/695), and release tests (6/6). `requirements-completed: []`; FRONT-02/03/04 and Phase 14 acceptance remain pending.
