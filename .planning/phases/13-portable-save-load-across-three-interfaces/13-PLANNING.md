# Phase 13 planning handoff

Planning and independent checking completed on 2026-11-01. No emulator behavior changed and no implementation, emulator tests, CI dispatch, frontend run or publication occurred.

Read [CONTEXT](13-CONTEXT.md) for locked choices, [RESEARCH](13-RESEARCH.md) for source findings, and [DESIGN](13-DESIGN.md) for the exact byte/API/identity/CLI contract. Research proposals yield to the design and locked context. Planning is based on shipped v2.0, package 0.10.0, API7/ABI1/behavior6; Phase 12 runtime, tests and workflows were not copied. Current planning/instruction references were carried forward so the isolated baseline resolves the accepted v3.0 scope and routing.

| Wave | Plan | Outcome |
|---|---|---|
| 1 | [13-01](13-01-PLAN.md) | Complete private visitor and field proofs |
| 2 | [13-02](13-02-PLAN.md) | Shared digests and effective machine identity |
| 3 | [13-03](13-03-PLAN.md) | Public API and battery rollback metadata |
| 4 | [13-04](13-04-PLAN.md) | Independent continuation and hostile-state proof |
| 5 | [13-05](13-05-PLAN.md) | Runner files, invocation frame points and hashes |
| 6 | [13-06](13-06-PLAN.md) | Libretro callbacks and basic serialization claims |
| 7 | [13-07](13-07-PLAN.md) | State/continuation evidence in existing equality producers |
| 8 | [13-08](13-08-PLAN.md) | Resource/docs checks, sanitizers and executed BE evidence |

The independent checker passed after one revision: high-file-count slices now explain coupled ownership, bounded read subsets and context headroom; sanitizer verification is executable. Context estimates remain low confidence, not measurements. Generated personal runtime paths were replaced by home-relative references. Seven requirements and all seventeen discussion decisions are covered; none is marked implemented or accepted by planning.

## Verification evidence

All commands below ran in the isolated Phase 13 checkout. Run URL: N/A-local. No logs are committed.

- `gsd-tools.cjs check verify-command-paths 13 --raw`: exit 0; no blockers/warnings across 22 commands. CMake commands are outside this probe's path-resolution scope, not proof future tests exist.
- `gsd-tools.cjs check verify-failure-directions 13 --raw`: exit 0; every runnable command states its failure signal.
- `gsd-tools.cjs query check.decision-coverage-plan .planning/phases/13-portable-save-load-across-three-interfaces .planning/phases/13-portable-save-load-across-three-interfaces/13-CONTEXT.md`: exit 0; all tracked decisions covered.
- `gsd-tools.cjs check gap-analysis-plan-post .planning/phases/13-portable-save-load-across-three-interfaces 'STATE-01, STATE-02, STATE-03, STATE-04, STATE-05, STATE-06, FRONT-01' --raw`: exit 0; all requirements and decisions covered.
- Per-plan `gsd-tools.cjs query verify.plan-structure PATH` and `query frontmatter.validate PATH --schema plan`: exit 0; required plan structure/frontmatter valid. Settled version/format choices retain reversibility annotations without another redundant approval.
- `sh scripts/hygiene.sh --tree` and `git diff --check`: exit 0.
- One-time preservation inspection: all primary pre-existing files except the intentional STATE/ROADMAP handoff updates and the primary index match their starting bytes; isolated changed files are planning/instructions only. Exit 0.

## Next session and stops

Context may be cleared. Open the existing isolated `.claude/worktrees/phase-13` checkout on `phase/13-portable-save-load-across-three-interfaces`, then invoke `$gsd-execute-phase 13`. The agent owns source/environment/service/receipt checks when applicable and executes the plans within that separate invocation. No owner verification or acknowledgment is needed now.

Exact-source executed native/PowerPC equality remains mandatory. PR PowerPC compilation does not execute the tests; native PR equality, advisory nightly execution and release publication gates retain their actual labels. Plan 13-08 first uses an already installed authorized environment; unavailable sanitizer/BE tooling or receipts cause an explicit evidence checkpoint. No installation, new container image, publication or hosted attempt is authorized implicitly. A future release is not present proof.

Phase 12 remains halted at accepted 0/4 with causes unknown and every hosted allowance spent. Its failure history, source/index and downstream halt are preserved; this independent phase does not waive any v3.0 requirement. Actual RetroArch save/load, rewind and run-ahead qualification remains Phase 14. Stop after this planning step; the user starts the separate execute step when ready.
