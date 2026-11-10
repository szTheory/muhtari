# Phase 14 Plan 26: Recovery Evidence

**Outcome:** The existing CI inventory is now in the order required by its configure-time consumer, and Plan 14-24's terminal halt remains intact. The single new automatic local wave gate reached CTest and failed because the ignored Wasmtime executable is a Linux binary on this Darwin arm64 host. The gate is spent; Plan 14-25 remains blocked and no retry is authorized under this scope.

## Inventory correction

The execution-start inventory contained the existing `policy.docker_contract.controls` row before `policy.claims_wording`. No test was added, removed, renamed, or relabeled. The row now follows `policy.digest.controls` and precedes `policy.host_support.controls`, matching the consumer's C-locale ordering requirement.

The original bytes were sorted into ignored `build/phase14-recovery26-evidence/inventory-sorted-before.txt`; the original `LC_ALL=C sort -c tests/inventory/ci.txt` exited 1 at line 571. Comparing the corrected inventory to those sorted bytes proves the change only reorders the existing rows. The original failed Plan 14-24 workflow is historical evidence and was not repeated.

## Plan 14-24 terminal closure

The preserved Plan 14-24 summary and evidence agree: the generated controller expected a 62-character MAME digest while the pin record and local input had the matching 64-character digest. The exact-input preflight failed before qualification. Qualification invocation, active claim, Docker contact, container creation, build, CTest and protected workload starts are all zero. Cleanup was not applicable because no container was created; the conditional pair remains ineligible. Offline fake-transport controls are explicitly separate from actual Docker contact.

The one Plan 14-24 automatic `cmake --workflow --preset ci` invocation exited 1 during configure because the inventory was unsorted. Build and test workflow steps did not run. That scope remains spent and is not retried.

At execution start, byte-for-byte snapshots were made and compared for `14-24-PLAN.md`, `14-24-SUMMARY.md`, `14-24-EVIDENCE.md`, `.planning/STATE.md`, `.planning/ROADMAP.md`, and `.planning/REQUIREMENTS.md`. All six comparisons passed. Their saved copies and the original sorted inventory remain under the ignored recovery evidence directory; generated snapshots are not committed.

## Plan 14-25 prerequisite

Plan 14-25 already had `depends_on: ["14-26"]` and `wave: 2` at this plan's execution start. Its prerequisite text requires this summary and a passing single automatic recovery gate, preserves the terminal-record and absent-workload review, and keeps its native source/parent gates distinct. Its `gate="blocking-human"` decision remains before any push, remote dispatch, or host action. No further Plan 14-25 edit was needed or made.

## Commands and results

| Command or check | Exit | Result |
| --- | ---: | --- |
| `LC_ALL=C sort -c tests/inventory/ci.txt` before edit | 1 | Expected initial ordering failure at line 571. |
| `LC_ALL=C sort -c tests/inventory/ci.txt` after edit | 0 | Inventory is sorted. |
| `cmp build/phase14-recovery26-evidence/inventory-sorted-before.txt tests/inventory/ci.txt` | 0 | Repaired inventory matches the independently sorted original bytes. |
| `git diff --check "$(cat build/phase14-recovery26-evidence/start-head.txt)" -- tests/inventory/ci.txt .planning/phases/14-proven-frontend-states-and-responsive-play/14-25-PLAN.md` | 0 | No whitespace errors in the scoped paths. |
| Six `cmp` checks between each saved protected file and its execution-start snapshot | 0 | Historical and phase bookkeeping files are byte-identical to the baseline. |
| Plan 14-24's `cmake --workflow --preset ci` | 1 | Historical configure failure; not rerun in this recovery. Run URL: `N/A-local`. |
| `cmake --workflow --preset ci` (Plan 14-26 automatic wave gate) | 8 | Configure and build passed; CTest failed when `built.*` fixtures could not launch Wasmtime. Run URL: `N/A-local`. |
| `uname -a` and `file build/pins/wasmtime/wasmtime` | 0 | Host is Darwin arm64; the ignored Wasmtime file is an AArch64 Linux ELF executable. This explains the recorded `exec format error` without another test invocation. |

The inventory task commit is `93c53cc` (`fix(14-26): sort CI inventory`). The configured workflow ran exactly once after the summary. No Docker, RetroArch, host, remote, or publication command was run for this plan.

## Automatic wave gate

**Terminal failure — execute-phase ran once.** `cmake --workflow --preset ci` exited 8: configure and build completed, then CTest failed as the `built.*` preprocess fixtures attempted to execute `build/pins/wasmtime/wasmtime`. `LastTest.log` records the invoked Wasmtime path and `exec format error`. Read-only inspection found the host is Darwin arm64 while that file is an AArch64 Linux ELF executable. The existing ignored pin artifact is for the wrong operating system; no project source or CI inventory failure remains in this run.

The one-use recovery gate is consumed. Do not rerun it or start Plan 14-25 under this scope. Plan 14-25 remains blocked until a distinct recovery scope is authorized to repair the local tool artifact and define a new verification allowance. This failure supplies no native-parent or actual-host acceptance evidence.

## GSD summary scan advisory

`gsd-tools verify-summary .planning/phases/14-proven-frontend-states-and-responsive-play/14-26-SUMMARY.md` exited 0 but returned `passed: false`. Its file heuristic treated backticked command text ending in `.txt` as missing generated files, and its self-check scan started at the earlier `## Verification` heading, so it saw the CTest failure before the explicit `## Self-Check: PASSED` section. Read-only checks confirmed the declared evidence, summary and inventory files exist, and the two task commits and summary commit exist (`test -s`/`git cat-file -e`, exit 0). This reproduces the advisory scanner limitation documented in GSD-HANDOFF §3.1; it does not change the plan's recorded CTest failure or acceptance status.

FRONT-02, FRONT-03, and FRONT-04 remain pending. This recovery changes no acceptance verdict and renews no Docker, remote, host, or Phase 12 allowance.
