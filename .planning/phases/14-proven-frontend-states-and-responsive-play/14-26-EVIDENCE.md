# Phase 14 Plan 26: Recovery Evidence

**Outcome:** The existing CI inventory is now in the order required by its configure-time consumer. Plan 14-24's terminal halt remains intact. Plan 14-25 retains its 14-26 dependency and owner gate. The single new automatic local wave gate is pending for execute-phase.

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

The task commit is `93c53cc` (`fix(14-26): sort CI inventory`). No configure, build, CTest, Docker, RetroArch, host, remote, or publication command was run for this plan.

## Automatic wave gate

**Pending — execute-phase is the sole runner.** After this plan's summary and commits, execute-phase will invoke `cmake --workflow --preset ci` once as the fresh local recovery gate and record its raw result and `N/A-local` run URL. This plan did not run or duplicate that gate. A failure, incomplete workflow, or missing receipt blocks Plan 14-25 without retry. A pass supplies no native-parent or actual-host acceptance evidence.

FRONT-02, FRONT-03, and FRONT-04 remain pending. This recovery changes no acceptance verdict and renews no Docker, remote, host, or Phase 12 allowance.
