# Phase 15 tune-up evidence

**Outcome:** fixed the document-budget checker's relative-root false pass with one recurring synthetic control. The evidence supports a documentation-check correctness improvement; it shows no pipeline speedup or hosted result.

## Baseline and six dimensions

The pre-change source was `bd3612ebe869c4d6c88dcbd0be31a10b3b28ea91`. The saved source identities and initial outputs are in ignored `build/phase15-evidence/BASELINE.md`; no raw log is committed. Scores are agent judgments on a 1–5 scale, with lower scores indicating weaker current evidence or usability. They are not performance measurements.

| Dimension | Score | Evidence and reason |
|---|---:|---|
| Performance | 3/5 | [quality.json](../../../quality.json) sets required test and finding bounds, but the reviewed local receipts do not provide comparable per-job or performance series. The [recurring-loop guidance](../../preparation/ENGINEERING.md) calls for comparable runs before changing timing policy. |
| CI/CD | 3/5 | The [native inventory](../../../tests/inventory/ci.txt) and [policy registrations](../../../tests/policy/tests.cmake) make required checks explicit. Historical local workflows include complete passes and censored timeouts; they do not provide comparable hosted queue and job data. See the [Phase 12 local evidence](../../milestones/v3.0-phases/12-player-ready-delivery-and-playstead-bridge/12-23-LOCAL-EVIDENCE.md) and [Phase 14 verification](../../milestones/v3.0-phases/14-proven-frontend-states-and-responsive-play/14-VERIFICATION.md). |
| Ease of use | 4/5 | [docs/testing.md](../../../docs/testing.md) describes the recurring checks. Before this tune-up, it did not give the direct budget command or explain that relative and absolute roots must cover the same files. |
| Architecture | 4/5 | The [budget checker](../../../tests/cmake/budgets.cmake) is a small existing CMake policy script and already uses a clear list of limits. The defect was confined to root resolution; the new control reuses [expect_fail.cmake](../../../tests/cmake/expect_fail.cmake). |
| Docs/README | 2/5 | The pre-change relative-root command exited 0 and counted two files; the absolute-root form counted 22. The current [budget contract](../../../docs/testing.md) now documents the direct invocation and equivalent root behavior. |
| Upgrade path | 3/5 | [pins.txt](../../../pins.txt) records fetched-tool identities. The local artifacts reviewed here contain no current authenticated dependency, repository-setting, or runner-image drift receipt, so no update is inferred. |

The weakest reproducible dimension was documentation-check correctness. The checker could report success after omitting documentation from its glob, making this a concrete candidate with a local, repeatable fault. No timing, workflow topology, cache, deadline, or advisory-status change qualified from the available measurements.

## Before and after

| Check | Before the fix | After the fix |
|---|---|---|
| Repository root spelled relative: `cmake -DROOT=. -P tests/cmake/budgets.cmake` | Exit 0; reported 2 files. | Exit 0; reported 22 files. |
| Same root spelled absolute: `cmake -DROOT="$PWD" -P tests/cmake/budgets.cmake` | Exit 0; reported 22 files. | Exit 0; reported 22 files. |
| Synthetic exact-limit fixture: README, public header, regular docs and hardware docs | Registered `docs.budgets.controls` failed on the relative-root four-file assertion; CTest exit 8. The matching JUnit RED record was accepted by `check tdd-red-evidence` as `RED_EVIDENCE_OK`. | Direct driver exit 0; registered `docs.budgets.controls` passes. Both root forms report four files. |
| One byte over each unchanged limit | The relative-root coverage assertion failed before the eight fault cases could complete. | Eight faults pass through `expect_fail.cmake`, each requiring exit 1 and a diagnostic naming the file, actual bytes and limit; four file types are tested under both root forms. |
| Missing root | Required-root failure retained. | Both absolute and relative missing-root cases require the existing root error and exact exit 1. |

The final control command is `cmake -DROOT="$PWD" -DWORK="$PWD/build/phase15-evidence/budget-controls" -P tests/policy/budgets.cmake` — exit 0, `N/A-local`. Its fixture and caller working directory contain spaces. The focused registered command `ctest --test-dir build/ci --output-on-failure -R '^docs\.budgets(\.controls)?$'` passed 2/2, `N/A-local`. The `ci` and `wasm32` exact inventories both include `docs.budgets.controls` with the existing `policy` label.

The limits remain 12,288 bytes for `README.md`, 24,576 bytes for `include/muhtari.h`, and 40,960 bytes for each regular or hardware Markdown document. `docs/testing.md` is 40,953 bytes after the edit, within its unchanged limit. All entries and required floors in [quality.json](../../../quality.json) remain unchanged.

## Local workflow and hygiene

| Command | Exit | Result |
|---|---:|---|
| `cmake --preset ci` | 0 | CMake configured the exact inventory, including the new policy control. Run URL: `N/A-local`. |
| `cmake --workflow --preset ci` — first attempt | 8 | 700/701 tests passed. `docs.budgets` rejected the first expanded wording because `docs/testing.md` was 41,104 bytes, 144 bytes over budget; packaging and release checks were not reached. The paragraph was shortened while retaining the invocation and root-form contract. Run URL: `N/A-local`. |
| `cmake --workflow --preset ci` — after correcting the document-size failure | 0 | All 701 tests passed; the workflow created the library, libretro and runner ZIPs; all 6 release checks passed. CTest wall time was 76.17 seconds. Run URL: `N/A-local`. |
| `sh scripts/hygiene.sh --tree` | 0 | Tracked-tree hygiene passed. Run URL: `N/A-local`. |
| `git diff --check 22aa961 --` | 0 | The scoped change has no whitespace errors. Run URL: `N/A-local`. |

The first local workflow failure was investigated and corrected before the passing rerun. It was isolated to the task's documentation-size requirement; no test, byte limit, quality floor, timeout, or existing CI policy was changed.

## Available CI evidence and limits

The [Phase 12 local receipt](../../milestones/v3.0-phases/12-player-ready-delivery-and-playstead-bridge/12-23-LOCAL-EVIDENCE.md) records 700 ordinary tests in 182 seconds, three packages, six release checks, and 30 hygiene checks in 40 seconds. It also records earlier timeout-only local workflow attempts at existing limits. The [Phase 14 local receipt](../../milestones/v3.0-phases/14-proven-frontend-states-and-responsive-play/14-27-EVIDENCE.md) records 695 tests and six release checks on a different source. The [Phase 14 exact-parent receipt](../../milestones/v3.0-phases/14-proven-frontend-states-and-responsive-play/14-25-EVIDENCE.md) records 694/695 ordinary tests before its one-second title-pin self-test timed out; hygiene and release stages were not reached. These records stay tied to their sources and outcomes.

| Requested measure | Evidence available here | Safe conclusion |
|---|---|---|
| Queue time | None in the retained local receipts; each says `N/A-local`. | No hosted queue comparison can be made. |
| Hosted critical path and per-job duration | No authenticated job timeline or per-job receipt was available in the inspected artifacts. Current CTest reported a local 76.17-second test wall time. | A local workflow result does not establish hosted critical path or runner minutes. |
| Completed work | Current local workflow completed 701 tests, three packages and six release checks. Prior local receipts cover different source revisions and test counts. | The required local contracts pass on this source; cross-run speed or workload change is not established. |
| Cancelled work | The inspected local receipts do not contain comparable hosted cancelled-job records. Timed-out tests remain censored failures, not completed or cancelled-job measurements. | No cancellation or topology remedy is supported. |

The current [quality ratchet](../../../quality.json) retains 605 native-test, 593 ASan-test, 24 hygiene-test, 12 ABI-test, 4 fuzz-test and 3 title-test floors, plus its existing zero-finding ceilings. The full workflow passed without adjusting them. No remote workflow or settings query was run.

## Bounded dispositions

| Candidate | Disposition | Revisit condition |
|---|---|---|
| CI queue, critical path, timeout, cache or job-topology tuning | Deferred; current and historical local receipts do not provide a comparable hosted series. All existing deadlines, cache decisions, labels and fault controls stay in place. | The next separately authorized CI/release evidence review has complete, source-bound comparable run and job receipts, including queue, completed work and cancellations. |
| Repository-settings or administrator drift | Deferred; verifying live settings needs authenticated remote evidence outside this plan's local scope. No permission or admin change is inferred. | A separately authorized review can read the current settings and compare every field covered by the existing script. |
| Dependency pins and runner-image updates | No update inferred from local files alone; no package or artifact was fetched. | A separately authorized maintenance review has current official version and identity evidence. |
| Phase 12/14 host evidence and v3.0 deferred requirements | Remain blocked/deferred exactly as recorded in the [v3.0 audit](../../milestones/v3.0-MILESTONE-AUDIT.md). This tune-up starts no host, RetroArch, remote CI, publication or merge action. | Only a new, separately scoped authorization can reopen those evidence paths. |

## Phase 16 handoff

The relative-root defect is fixed and the required local workflow, hygiene and review checks passed. After the orchestrator completes its phase review and bookkeeping, the next development scope is Phase 16 planning for the offline WebAssembly core. The Phase 12 permission denial, Phase 12/14 host and remote stops, and nine deferred v3.0 requirements remain unchanged.
