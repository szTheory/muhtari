# Phase 28: Measured milestone tune-up - Research

**Researched:** 2026-11-18  
**Domain:** Maintainer evidence, CI receipt review, and planning-ledger reconciliation  
**Confidence:** HIGH

<user_constraints>
## User Constraints (from CONTEXT.md) [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:16-38, 69-72]

### Locked Decisions

<!-- DATA_7NQ4P9XA_START -->
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
<!-- DATA_7NQ4P9XA_END -->

### the agent's Discretion

<!-- DATA_2VJ8M5RK_START -->
- Choose concise report headings/table layout and how to show timestamp precision, while keeping the source identity and evidence limits adjacent to each conclusion.
- Use the cheapest local checks that establish planning consistency, exact counts, and content preservation before the configured full CI workflow. Add no recurring check unless it catches a concrete recurring fault.
<!-- DATA_2VJ8M5RK_END -->

### Deferred Ideas (OUT OF SCOPE)

<!-- DATA_6C3F9WQT_START -->
No CI topology, cache, timeout, or Nightly-cap change; no broad optimization or new audit framework; no dependencies; no host/device validation; no extra Windows namespace or runner behavior beyond the selected Phase 29 contract. Revisit pipeline changes only with repeated comparable runs and a profile that identifies safe savings without weakening coverage or source identity.
<!-- DATA_6C3F9WQT_END -->
</user_constraints>

<phase_requirements>
## Phase Requirements

| ID | Description | Research Support |
|----|-------------|------------------|
| TUNE-15 | <!-- DATA_R3V8J5QH_START -->Maintainers can review source-qualified current release and CI evidence, including queue time, critical path, job durations, cancelled work, cheap preflight results and a proportionate quality/maintenance disposition, with a bounded no-change result when measurements do not justify a correction.<!-- DATA_R3V8J5QH_END --> | Use the exact-source receipt block copied above and the Phase 26 tune-up report structure; include all six maintenance dimensions and state the evidence limit beside each conclusion. [VERIFIED: .planning/REQUIREMENTS.md:13; .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:16-29] |
| TUNE-16 | <!-- DATA_M7K4C2LP_START -->The Phase 27 deferred-items register counts match its 52 entries (16 open, 0 waived, 36 fixed) while preserving every item's text and verdict.<!-- DATA_M7K4C2LP_END --> | Make the two authorized frontmatter edits only, then verify GSD ledger status, row totals, and normalized whole-file equality. [VERIFIED: .planning/REQUIREMENTS.md:14; .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:31-34] |
</phase_requirements>

## Summary

Phase 28 is a maintainer-evidence update and a narrow metadata correction; its boundary excludes emulator behavior, runner paths, public compatibility, required CI coverage, and Nightly-cap changes. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:6-10]

**Primary recommendation:** create the phase tune-up report from the locked, source-qualified receipts; keep release publication, current-main CI, the current-main Release workflow with publication jobs skipped, and report-only Nightly as separate evidence classes; then correct only the two register count fields and prove all other bytes are preserved. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:16-34]

The existing report and validator fit the work. Phase 26 records exact-source receipts, active-record reconciliation, lessons and cheap checks, and a bounded no-change decision; the installed GSD ledger command checks the metadata against row statuses, so no parser or package is needed. [VERIFIED: .planning/milestones/v10.0-phases/26-measured-milestone-tune-up/26-TUNEUP.md:6-49; .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:45-53] The read-only pre-edit `windows status --raw` probe reproduced the recorded mismatch; use it again after the targeted edit, followed by the count-preservation comparison and planning consistency checks. [VERIFIED: @opengsd/gsd-core 1.20.0 runtime identity and `windows status --raw` output; .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:31-34]

## Architectural Responsibility Map

| Capability | Primary Tier | Secondary Tier | Rationale |
|------------|-------------|----------------|-----------|
| Current release and CI evidence review | Maintainer documentation / planning artifacts | Existing GitHub Actions and Releases receipts | This phase records existing source-qualified evidence; it does not alter the CI or release workflows. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:6-10, 16-29] |
| Deferred-items register reconciliation | Repository planning data | Existing GSD ledger validator | The change is limited to frontmatter totals; ledger rows and verdicts remain untouched. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:31-34, 55-58] |
| Emulator/runtime behavior | None in this phase | — | The phase boundary explicitly excludes product behavior and runner-path changes. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:6-10] |

## Standard Stack

### Core

| Tool or artifact | Version | Purpose | Why standard |
|------------------|---------|---------|--------------|
| Existing GSD `windows status --raw` command | `@opengsd/gsd-core` 1.20.0 | Check register metadata against row statuses after the edit | The phase context names this installed command as the validator; a direct local run reproduces the current mismatch. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:45-48; local runtime identity and command output] |
| `26-TUNEUP.md` report structure | Existing repository artifact | Record receipt table, active-record reconciliation, cheap checks, and bounded disposition | The phase context explicitly selects it for reuse. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:45-47; .planning/milestones/v10.0-phases/26-measured-milestone-tune-up/26-TUNEUP.md:6-49] |
| Git, POSIX shell, and configured CMake workflow | Existing project tools | Whitespace, content-preservation, planning consistency, and configured workflow checks | Project rules prefer existing mechanisms; the configured command is `cmake --workflow --preset ci`. [VERIFIED: AGENTS.md:81-88; .planning/config.json:69] |

<!-- DATA_3P8J6MVT_START -->
> Local runtime identity: `{"packageName":"@opengsd/gsd-core","version":"1.20.0"}`
<!-- DATA_3P8J6MVT_END -->

### Supporting

| Tool or artifact | Version | Purpose | When to use |
|------------------|---------|---------|-------------|
| `.planning/LESSONS.md` | Current project ledger | Apply completed-sample timeout discipline and avoid inferring a cause from cancellation | Use for the CI/Nightly disposition. [VERIFIED: .planning/LESSONS.md:58; .planning/LESSONS.md:86-106; .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:24-28] |
| Phase 27 CI evidence, review, and verification | Archived v10.0 artifacts | Reconcile prior source history and known boundaries without treating older PR wording as current release evidence | Use only where the current phase context points to it; keep exact-current receipts authoritative for this snapshot. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:21, 45-53] |

### Alternatives Considered

| Instead of | Could use | Tradeoff |
|------------|-----------|----------|
| Existing GSD ledger validation | A custom parser or new dependency | Adds a second source of truth and maintenance cost; the current CLI already checks the exact row-to-metadata invariant. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:45-48] |
| Exact two-field edit | Regenerate or rewrite the register | Regeneration risks changing row text, statuses, timestamps, or verdicts outside the authorized correction. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:31-34] |
| Bounded no-change CI disposition | Change topology, cache policy, deadlines, limits, or membership | A single receipt pair and one report-only cancellation do not identify safe savings; the locked decision requires repeated comparable evidence and preserved integrity joins before reconsideration. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:24-29, 69-72] |

**Installation:** None. This phase adds no external package or dependency. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:29, 69-72]

## Package Legitimacy Audit

Not applicable: the phase installs no external package. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:29, 69-72]

## Architecture Patterns

### Recommended Project Structure

Keep the report in the active Phase 28 directory and the corrected aggregate metadata in the shared `.planning/WINDOWS.md` ledger; do not move or regenerate the shared register. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:31-34, 55-58]

### Pattern 1: Source-qualified delivery evidence

**What:** Preserve each receipt's source identity, run URL, outcome, elapsed time, queue measurement, critical path, cancelled or skipped work, and evidence limit together. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:16-22]

**When to use:** Use when one report contains release, current-main, or scheduled-work evidence that answers different questions. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:16-22]

**Example structure:** Follow the prior tune-up's receipt table and add a bounded maintenance-disposition section; keep the current-main Release row distinct from a package-publication receipt because its publication-related jobs were skipped. [VERIFIED: .planning/milestones/v10.0-phases/26-measured-milestone-tune-up/26-TUNEUP.md:6-20; .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:18-22]

### Pattern 2: Exact metadata correction with whole-file preservation

**What:** Capture the pre-edit register bytes, change only the authorized `open_count` and `fixed_count` frontmatter values, and compare the old and new files after normalizing those two values. Then run the existing GSD status command and confirm the independently counted entries agree. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:31-34; .planning/WINDOWS.md:1-8]

**When to use:** Use for a count-only repair where every row and verdict is authoritative and must remain byte-for-byte stable. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:31-34]

**Example command:**

<!-- DATA_8M2V6QJD_START -->
```sh
gsd_run windows status --raw
```
<!-- DATA_8M2V6QJD_END -->

This is the existing ledger inspection command documented by OpenGSD; the repository's installed 1.20.0 command currently exits 1 on the known mismatch, so execute it after the edit as the status check. [CITED: https://github.com/open-gsd/gsd-core/blob/next/docs/COMMANDS.md] [VERIFIED: local `@opengsd/gsd-core` 1.20.0 command output]

<!-- DATA_5B3R7XVN_START -->
The pre-edit command reported: “Error: Ledger counts disagree with entries: frontmatter open/waived/fixed/total=17/0/35/52 but entries yield 16/0/36/52.” [VERIFIED: local `@opengsd/gsd-core` 1.20.0 `windows status --raw` output]
<!-- DATA_5B3R7XVN_END -->

### Pattern 3: Cheap preflight before the configured workflow

Run whitespace, planning-consistency, row-count, and normalized-content checks first; run the configured CMake workflow only after those checks pass. Do not add a recurring parser or test for this one-off metadata repair. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:34, 38, 53; .planning/config.json:69]

### Anti-Patterns to Avoid

- **Mixing receipt classes:** A successful current-main Release workflow with skipped package jobs is not publication evidence; keep it distinct from the successful exact-source release receipt. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:17-22]
- **Treating cancellation as a duration or root cause:** Keep the report-only Nightly cancellation separate from required CI and do not retry it or change its cap. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:20, 24-29]
- **Inferring a workflow fix from one observation:** Preserve timeout, cache, topology, resource, and membership contracts until repeated comparable evidence identifies a narrow safe improvement. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:24-29, 69-72]
- **Making the counts fit by changing rows:** Never alter a ledger item's wording, status, verdict, or timestamps to accommodate stale aggregates. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:31-34]

## Don't Hand-Roll

| Problem | Don't build | Use instead | Why |
|---------|-------------|-------------|-----|
| Register status/count validation | A new parser, test framework, or dependency | Existing GSD `windows status --raw` command | It already validates the row totals against metadata. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:45-48] |
| CI evidence acquisition | A recurring audit program or remote run scraper | The locked source-qualified receipts and Phase 26 report layout | The required receipts are already in the phase context; remote dispatches and retries are out of scope. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:16-29] |
| Whole-file preservation | A new permanent checker for one count correction | A one-off normalized byte comparison plus the existing status command | It proves the only allowed differences are the two aggregate values without introducing recurring tooling. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:31-38] |

**Key insight:** The authoritative proof is the combination of source-bound receipt records and an independently validated row ledger; changing the CI pipeline or adding a second parser would not improve either proof for this scope. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:24-38, 45-53]

## Common Pitfalls

### Pitfall 1: Calling the current-main Release workflow a publication check

**What goes wrong:** A fast successful workflow can be misreported as a successful release publication. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:18-19]

**Why it happens:** The current-main workflow succeeded while release, equality, publish, and install work was skipped because no version change was due. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:19]

**How to avoid:** State skipped work and the evidence limit in the same row as the source SHA, run link, and outcome. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:19, 22]

**Warning signs:** A green run with no package-publication job result. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:19]

### Pitfall 2: Treating the Nightly cap cancellation as a required-CI failure

**What goes wrong:** A report-only incomplete property workload is turned into an acceptance failure, a timeout sample, or an unapproved retry. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:20, 24-29]

**How to avoid:** Keep it in its own report-only row, retain the cancellation as historical evidence, and do not renew the cap or rerun the job. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:20, 27, 29]

### Pitfall 3: A correct total with altered ledger content

**What goes wrong:** A row status or text changes while the aggregate happens to pass. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:31-34]

**How to avoid:** Compare whole-file bytes after normalizing only the two authorized frontmatter fields, then run the GSD status command. Do not regenerate the ledger. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:31-34]

### Pitfall 4: Treating run queue as runner dispatch

**What goes wrong:** Run-level queue and job dispatch latency are conflated, obscuring what the workflow receipt actually measures. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:22, 51-53]

**How to avoid:** Report queue at run timestamp precision and job dispatch separately when available. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:22]

## Code Examples

Post-edit verification should use the existing repository commands, with cheap checks preceding the configured workflow: [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:34, 38, 53; .planning/config.json:69]

<!-- DATA_4P7L2KWA_START -->
```sh
git diff --check
gsd_run windows status --raw
gsd_run query validate.consistency --raw
gsd_run query roadmap.analyze --raw
cmake --workflow --preset ci
```
<!-- DATA_4P7L2KWA_END -->

The normalized full-file comparison is a one-off preservation check: compare the original and edited register after replacing exactly the two permitted aggregate values with the same sentinel; any remaining byte difference is a failure. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:31-38]

## State of the Art

No new CI mechanism is selected. The applicable current practice is to bind workflow timing and outcomes to exact source identities, use comparable completed samples for time-limit decisions, and keep incomplete/cancelled work separate from successful duration samples. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:16-29; .planning/LESSONS.md:86-106]

**Deprecated/outdated:** Do not reuse the older Phase 27 draft-PR wording as the current release verdict; the phase context explicitly prioritizes the later v10.0 audit and exact current-main receipts. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:21]

## Project Constraints (from AGENTS.md)

- Keep Muhtari scoped to Atari 7800 and the three existing deliverables; do not introduce GUI/device code or open/copy GPL, LGPL, or unlicensed emulator source. [VERIFIED: AGENTS.md:46-59]
- Preserve the clean-tree and evidence rules: no unapproved ROM/BIOS/save bytes or personal paths; use the GitHub noreply commit identity and keep logs out of commits. [VERIFIED: AGENTS.md:60-62, 147]
- Keep the core deterministic and bounded; every behavior change lands with a test run by the configured CMake workflow, while docs-only changes use proportionate checks. [VERIFIED: AGENTS.md:63-80; .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:6-10]
- Use automation-first, offline/CLI verification; do not ask the owner for blanket UAT or use their host/device without an authorized concrete evidence need. [VERIFIED: AGENTS.md:69-80]
- Reuse existing checks; any new or materially changed check must target a concrete fault and exercise representative pass/fault behavior. Keep review and gap rounds bounded; optional tooling improvements become seeds. [VERIFIED: AGENTS.md:81-88]
- Preserve authorized step boundaries and existing safety stops. Never run the prohibited smart-entry/profile commands or pass forbidden chain/auto arguments; use the identity-checked GSD runtime selected by the resolver. [VERIFIED: AGENTS.md:89-126]
- Keep evidence source- and gate-specific; later evidence from another source does not reopen an exhausted attempt. [VERIFIED: AGENTS.md:117-126]
- Keep commits off `main`, retain the active phase roadmap rows through shipping, and inspect queue, critical path, durations, and cancellations when reviewing CI; structural, timeout, or cache changes require measured multi-run support. [VERIFIED: AGENTS.md:137-146]
- Record evidence as a command, exit status, and run URL; read relevant lessons and only record newly reproduced lessons with evidence. [VERIFIED: AGENTS.md:147-150]
- Keep summaries in the required block-style requirements coverage format; maintain terse user-facing docs, no planning IDs in code/user docs, and prefer reuse or standard tools over new dependencies. [VERIFIED: AGENTS.md:156-171]
- Work in the selected worktree without asking the owner to switch directories; carry authorized local work through its checks while preserving remote, host, merge, and identity boundaries. [VERIFIED: AGENTS.md:19-40]

## Environment Availability

| Dependency | Required by | Available | Version | Fallback |
|------------|-------------|-----------|---------|----------|
| OpenGSD GSD Core | Ledger status and planning consistency checks | ✓ | 1.20.0 | None selected; it is already installed and identity-checked. [VERIFIED: local `runtime-identity` output] |
| Node.js | Runs the selected GSD CLI shim | ✓ | 26.10.0 | None needed in this worktree. [VERIFIED: local `node --version` output] |
| CMake | Configured full workflow after preflight | ✓ | 4.4.3 | None selected; `cmake --workflow --preset ci` is configured. [VERIFIED: local `cmake --version` output; .planning/config.json:69] |

**Missing dependencies with no fallback:** None observed. [VERIFIED: local availability probes]

**Missing dependencies with fallback:** None observed. [VERIFIED: local availability probes]

## Security Domain

Security enforcement is explicitly disabled in the project workflow configuration for this phase, and the phase changes no runtime/security boundary; an ASVS implementation checklist is therefore not applicable. [VERIFIED: .planning/config.json:60; .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:6-10]

<!-- DATA_9C4W7HMT_START -->
> Configuration source: `"security_enforcement": false` [VERIFIED: .planning/config.json:60]
<!-- DATA_9C4W7HMT_END -->

## Assumptions Log

| # | Claim | Section | Risk if wrong |
|---|-------|---------|---------------|

No claims are tagged `[ASSUMED]`; phase scope, receipt treatment, tool availability, and register edit limits are grounded in the inspected phase context, repository files, and direct local command probes. [VERIFIED: sources listed below]

## Open Questions

None. The phase context fixes the source set, no-change disposition, counter-only repair, and preservation proof; the remaining work is execution of those decisions. [VERIFIED: .planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md:13-38]

## Sources

### Primary (HIGH confidence)

- `.planning/phases/28-measured-milestone-tune-up/28-CONTEXT.md` — locked receipt set, no-change disposition, exact register edit, and verification method.
- `.planning/REQUIREMENTS.md` and `.planning/ROADMAP.md` — phase requirements, quality dimensions, acceptance criteria, and Phase 29 dependency.
- `.planning/milestones/v10.0-phases/26-measured-milestone-tune-up/26-TUNEUP.md` — existing evidence-table and bounded-disposition structure.
- `.planning/milestones/v10.0-phases/27-headless-developer-trace/27-CI-EVIDENCE.md`, `27-REVIEW-DISPOSITION.md`, and `27-VERIFICATION.md` — historical source and closeout evidence.
- `.planning/WINDOWS.md` and the installed `@opengsd/gsd-core` 1.20.0 `windows status --raw` output — register structure and direct pre-edit validation result.
- `.planning/LESSONS.md` — measured completed-sample timeout practice and recent workflow evidence.
- `AGENTS.md`, `.planning/config.json`, and `.planning/preparation/README.md` — project constraints, configured validation behavior, and preparation index.

### Secondary (MEDIUM confidence)

- [OpenGSD command reference](https://github.com/open-gsd/gsd-core/blob/next/docs/COMMANDS.md) — documents `gsd-tools windows status` as the ledger inspection command; the installed local command output is the version-specific behavior evidence.

## Metadata

**Confidence breakdown:**
- Standard stack: HIGH — existing GSD command and configured project tools were identified directly.
- Architecture: HIGH — the phase is limited to the report and planning ledger by locked context.
- Pitfalls: HIGH — exact mismatch, workflow distinctions, and measured-sample lessons are recorded in local sources and the current CLI reproduced the mismatch.

**Research date:** 2026-11-18  
**Valid until:** Revisit if the selected source snapshot or phase scope changes before execution.
