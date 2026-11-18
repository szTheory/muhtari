# Phase 28: Measured milestone tune-up - Context

**Gathered:** 2026-11-18
**Status:** Ready for planning

<domain>
## Phase Boundary

Qualify the current release and main-branch delivery evidence, record a proportionate maintenance disposition, and correct the Phase 27 deferred-item register's mismatched aggregate counts. Preserve all register entries and verdicts. This is a maintainer-evidence phase; it does not change emulator behavior, runner paths, public compatibility, required CI coverage, or the Nightly cap.

</domain>

<decisions>
## Implementation Decisions

### Source-qualified delivery evidence
- Treat stable v0.17.0 as the current release: tag and 20 release assets point to exact source `333380b118f62b618cb6df29cdfdd4388efc7b22`. Its exact-source [CI run 44214206424](https://github.com/szTheory/muhtari/actions/runs/44214206424) passed in 47m18s with 0s run-level queue; Windows ARM64 ran 43m15s, followed by WASM32 at 3m49s. Its exact-source [Release run 44214206354](https://github.com/szTheory/muhtari/actions/runs/44214206354) passed in 52m07s; the critical path was Windows ARM64 (41m41s), WASM32 (3m54s), publish (5m45s), and latest-install (10s). Neither required run cancelled jobs.
- Treat current `main` as `f709d493e6660b3b53a98b49b693ab91a714dcb0`. Exact-source [CI run 44233889699](https://github.com/szTheory/muhtari/actions/runs/44233889699) passed in 43m50s with 0s run-level queue; Windows ARM64 ran 39m07s and WASM32 followed for 4m32s. The Windows ARM64 job spent 24m09s in its quality-observations step; this locates time but does not identify a safe reduction.
- The exact-current-main [Release run 44233889579](https://github.com/szTheory/muhtari/actions/runs/44233889579) succeeded in 15s, but release, equality, publish, and install jobs were skipped because no version change was due. Do not describe it as a package-publication check.
- Record [scheduled Nightly run 44236642588](https://github.com/szTheory/muhtari/actions/runs/44236642588) separately: on current main, its report-only 10,000-case `prop` job was cancelled at its existing 9-minute job cap, and the completeness check failed because the property workload was incomplete. Other Nightly jobs completed. This is not required CI; retain the result and do not retry or change its cap from this observation.
- Keep historical failures attached to their original sources. In particular, v0.15.0 remains an unpublished failed tag/draft and is not the current release. Do not let the older Phase 27 draft-PR wording supersede the later v10.0 audit and exact current-main receipts.
- Report queue time as 0s at run timestamp precision and job dispatch separately where useful. State source SHA, run URL, outcome, elapsed time, cancelled/skipped work, and evidence limits together.

### Proportionate maintenance disposition
- Review performance, CI/CD, ease of use, architecture, documentation, and upgrade path. Reuse Phase 26's tune-up report structure and the current Phase 27 review/verification; do not repeat settled research.
- Record a bounded no-change disposition for CI structure, timeouts, caches, parallelism, resource limits, and required/advisory membership. The exact-source release and current required CI are green; one run pair does not isolate a regression or a safe topology change. The current Windows ARM64 quality step and subsequent WASM32 tail are visible, but job timing alone does not identify removable work.
- Keep the cancelled Nightly property result report-only and distinct from the required CI verdict. Apply MU-CI-021 and XFER-012: use completed comparable samples for timeout decisions, and do not infer a cause from a cancellation. The 3 Ir/frame trace result from Phase 27 is source-specific and is not a CI-optimization measurement.
- A later cache or topology proposal needs repeated comparable evidence, a measured per-step benefit, and controls that preserve hash-equality/artifact joins and gate membership. GitHub documents that cache contents are restored as-is and untrusted; do not add cache complexity without a measured benefit and integrity boundary: [dependency caching guidance](https://docs.github.com/en/actions/concepts/workflows-and-actions/dependency-caching).
- No extra package, dependency, recurring audit program, remote dispatch, rerun, host/device check, API change, or hardware claim is needed for this phase.

### Shared deferred-items register
- The shared register is `.planning/WINDOWS.md`, as named by Phase 27's deferred item. Its 52 rows currently tally 16 open, 0 waived, and 36 fixed, while the frontmatter says 17 open and 35 fixed; `windows status --raw` currently fails with this exact mismatch (exit 1).
- Change only `open_count` from 17 to 16 and `fixed_count` from 35 to 36. Keep `waived_count: 0`, `total_count: 52`, `last_updated`, every ledger row, all entry text, and every verdict unchanged. Do not regenerate the file or alter a status to make the old counts fit.
- Verify the post-edit entry tally is 52/16/0/36, `windows status --raw` succeeds, and a normalized whole-file comparison confirms only the two authorized frontmatter values changed. This protects against an accidental description or status change that leaves the aggregate counts looking correct.

### the agent's Discretion
- Choose concise report headings/table layout and how to show timestamp precision, while keeping the source identity and evidence limits adjacent to each conclusion.
- Use the cheapest local checks that establish planning consistency, exact counts, and content preservation before the configured full CI workflow. Add no recurring check unless it catches a concrete recurring fault.

</decisions>

<code_context>
## Existing Code Insights

### Reusable Assets
- Reuse `.planning/milestones/v10.0-phases/26-measured-milestone-tune-up/26-TUNEUP.md` for the source-qualified tune-up structure.
- Reuse `.planning/milestones/v10.0-phases/27-headless-developer-trace/27-CI-EVIDENCE.md`, `27-VERIFICATION.md`, and its review for exact-source history and Phase 27 closeout evidence.
- The installed GSD 1.20.0 `windows status --raw` command validates ledger metadata against the row statuses; no extra parser or dependency is needed.

### Established Patterns
- Separate exact release, current-main, archive, and report-only Nightly evidence. The release workflow on current main can succeed while publication-related jobs are skipped when no release is due.
- Use source-qualified timestamps and job details; don't treat run-level queue time as runner dispatch time.
- Run narrow whitespace, GSD consistency, and count-preservation checks before longer workflows. Baseline results for this worktree: `git diff --check` exit 0, `validate.consistency --raw` passed, and `roadmap.analyze --raw` identifies Phase 28 as next and Phase 29 as dependent.

### Integration Points
- `.planning/WINDOWS.md` holds the shared register metadata and rows.
- `.planning/phases/28-measured-milestone-tune-up/28-TUNEUP.md` should capture TUNE-15 evidence and the bounded disposition.
- Active `.planning/ROADMAP.md`, `.planning/REQUIREMENTS.md`, and `.planning/STATE.md` must report Phase 28 progress without marking requirements complete before verification.

</code_context>

<specifics>
## Specific Ideas

Keep the one current CI/Nightly snapshot readable and auditable. Cite exact source hashes and run URLs; distinguish the package-publishing Release run from the latest-main Release workflow that skipped package work.

</specifics>

<deferred>
## Deferred Ideas

No CI topology, cache, timeout, or Nightly-cap change; no broad optimization or new audit framework; no dependencies; no host/device validation; no extra Windows namespace or runner behavior beyond the selected Phase 29 contract. Revisit pipeline changes only with repeated comparable runs and a profile that identifies safe savings without weakening coverage or source identity.

</deferred>
