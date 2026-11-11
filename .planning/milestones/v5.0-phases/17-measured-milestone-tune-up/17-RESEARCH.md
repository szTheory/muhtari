# Phase 17: Measured milestone tune-up - Research

**Researched:** 2026-11-10
**Domain:** Source-specific release records, local CMake verification, CI quality/efficiency reporting
**Confidence:** HIGH

<user_constraints>
## User Constraints (from CONTEXT.md)

### Locked Decisions

### Evidence and correction
- D-01: Reuse the kickoff correction of stale v4.0 shipping records as the one small evidenced improvement; validate current source-specific links and claims rather than inventing another change.
- D-02: Score the six SEED-007 quality dimensions from current files, the fresh saved preflight receipts and existing required local checks. Distinguish observed measurements from judgments and censored results.
- D-03: Retain existing cache, timeout, job topology and advisory/required policy: the reviewed hosted series supports no safe structural optimization. Record queue-measurement limits, critical path, job durations and cancelled advisory work.

### Scope and verification
- D-04: Reuse the ordinary native workflow, exact-source receipt audits, hygiene and small one-off documentation assertions. Add recurring tests only if an implementation fault justifies them.
- D-05: Keep all nine deferred v3.0 requirements and original denied/spent Phase 12/14 host/remote stops intact. Local preparation does not renew any of them.

### the agent's Discretion
- Choose the smallest executable plan and useful early checks. Reuse the existing temporary two-worker FIFO jobserver helper if resource contention warrants it; do not change presets or deadlines.
- Adopt ordinary recommendations under the owner's explicit autonomous authorization; no routine acceptance prompt is required.

### Deferred Ideas (OUT OF SCOPE)
Browser/device acceptance, previous host scopes, public release actions, broad historical diagnosis and unsupported pipeline optimization remain outside scope.
</user_constraints>

<phase_requirements>
## Phase Requirements

| ID | Description | Research Support |
|----|-------------|------------------|
| TUNE-09 | Maintainers can begin feature work from current publication/docs and a source-specific quality/efficiency baseline, with one evidenced small correction or an explicit bounded no-change disposition that preserves required checks and historical failures. | Reconcile the current v4.0 publication record; audit the saved exact-source receipts; report the six SEED-007 dimensions with measurements separated from judgments/censored data; run the existing local workflow and hygiene. [VERIFIED: .planning/REQUIREMENTS.md:8-10; verbatim: “TUNE-09” and “one evidenced small correction or an explicit bounded no-change disposition”] |
</phase_requirements>

## Project Constraints (from AGENTS.md)

- Preserve the three deliverables and do not add window, audio-device, input-device, browser UI, or device behavior. Keep ROM, BIOS, and save bytes out of the tree except manifest-approved fixtures; keep personal paths and names out of artifacts. [VERIFIED: AGENTS.md:50-56; verbatim: “No ROM, BIOS or save bytes except fixtures listed in `tests/roms/manifest.txt`.”]
- No core behavior change is indicated. If one becomes necessary, pair it with a test run by `cmake --workflow --preset ci`; keep README user-facing and maintainer/test material in `docs/`. [VERIFIED: AGENTS.md:60-62; verbatim: “Every behaviour change lands with a test that `cmake --workflow --preset ci` runs.”]
- Keep verification automation-first and use existing CMake `-P` or POSIX shell checks. A new/materially changed check must identify its concrete fault and exercise representative pass/fault behavior; preserve existing required behavior. [VERIFIED: AGENTS.md:63-80]
- Report evidence as a command, exit status, and run URL, not committed logs; preserve source-specific receipts and historical outcomes. [VERIFIED: AGENTS.md:111-132; verbatim: “Evidence is a command, an exit status and a run URL, never a committed log.”]
- Do not perform remote dispatch, publication, merge, or host/device actions in this phase; local work does not renew prior host/remote scopes. [VERIFIED: .planning/phases/17-measured-milestone-tune-up/17-CONTEXT.md:9,20-22]
- Before any GSD step that commits, branch from `origin/main`; never commit on `main`. Use the workflow-selected GSD runtime for status/planning/execution, and do not run the specifically forbidden entrypoints/arguments or hygiene bypass variables. [VERIFIED: AGENTS.md:83-128]
- Do not introduce GPL/LGPL/unlicensed emulator code or a dependency for this documentation/baseline phase; follow the existing standard-library/platform-first dependency ladder. [VERIFIED: AGENTS.md:50-52,145-153]
- Plan summaries must retain `requirements-completed:` and block-style `coverage:` metadata with the prescribed fields. [VERIFIED: AGENTS.md:138-143]

## Summary

Carry the kickoff correction through the current project records and verify it against the saved v4.0 candidate, release-PR, release, and Nightly receipts. The receipt files are already held in ignored `build/milestone-preflight/`, and the authoritative shipping record separates current publication from the older checkpoints. Use the existing exact-source CMake auditor and ordinary local checks; add a recurring test only if the doc correction exposes a concrete implementation fault. [CITED: .planning/phases/17-measured-milestone-tune-up/17-CONTEXT.md:32-40; VERBATIM: “.planning/milestones/v4.0-SHIP.md and ignored build/milestone-preflight/ hold exact-source hosted receipts.”; VERIFIED: .planning/milestones/v4.0-SHIP.md:51-59]

The hosted evidence supports a bounded no-change disposition for CI structure: retain the current cache, deadlines, job topology, and required/advisory policy. State the source and scope of each receipt, the critical path and durations, and what is censored. In particular, a cancelled Nightly is not property-test acceptance, and retry creation-to-latest-start intervals are not a clean queue-time measurement. [VERIFIED: .planning/phases/17-measured-milestone-tune-up/17-CONTEXT.md:15-22; .planning/milestones/v4.0-SHIP.md:57-59; verbatim: “they are not a reliable queue metric.”]

**Primary recommendation:** produce one compact, linked tune-up report and current-record correction, then run the existing local CMake workflow, hygiene, source-bound receipt checks, and only the small document assertions needed by the correction. Do not dispatch CI, query live services, or alter pipeline policy. [VERIFIED: .planning/phases/17-measured-milestone-tune-up/17-CONTEXT.md:9,15-22; .planning/seeds/SEED-007-milestone-tune-up.md:28-39]

## Architectural Responsibility Map

| Capability | Primary Tier | Secondary Tier | Rationale |
|------------|-------------|----------------|-----------|
| Current publication narrative | Planning and maintainer documentation | — | The canonical roadmap/state/ship records own the explanation of current release status and its relationship to historical failures. [VERIFIED: .planning/phases/17-measured-milestone-tune-up/17-CONTEXT.md:39-40; verbatim: “Current milestone roadmap, STATE, requirements, product roadmap and prompt coverage remain the canonical planning consumers.”] |
| Hosted receipt acceptance | Existing local CMake evidence audit | Saved build receipts | `tools/check_ci_run.cmake` binds a complete response to the expected source and authoritative required aggregate while printing other failed/skipped outcomes. [VERIFIED: tools/check_ci_run.cmake:30-78; verbatim: “source mismatch: expected”; “required aggregate is not completed/success”] |
| Reproducible local baseline | Existing CMake workflow and shell hygiene | Python FIFO jobserver helper | These existing mechanisms own native checks and repository hygiene; no emulator runtime or new build tier is part of this documentation tune-up. [VERIFIED: .planning/milestones/v4.0-SHIP.md:10,39; build/ship-local-gate.py:1-12] |

## Standard Stack

### Core

| Tool | Version | Purpose | Why Standard |
|------|---------|---------|--------------|
| Existing CMake presets and `-P` audits | Repo-defined; CMake 4.4.3 available in this environment | Native workflow, hygiene workflow, and saved CI receipt checks | The repository makes CMake workflows and `-P` checks its standard verification path. [VERIFIED: AGENTS.md:60-80; local probe `cmake --version`] |
| Existing Python FIFO helper | Python 3.14.4 available in this environment | Runs the unchanged native workflow with bounded two-worker jobserver scheduling when needed | The existing helper invokes the ordinary CMake CI workflow and owns only temporary scheduling state. [VERIFIED: build/ship-local-gate.py:1-12; local probe `python3 --version`] |
| Existing POSIX hygiene script | System shell; Git 2.41.0 available | Tracked-tree hygiene and whitespace checks | Hygiene is an existing required repository gate, not a new subsystem. [VERIFIED: AGENTS.md:54-56,60-80; .planning/milestones/v4.0-SHIP.md:39; local probe `git --version`] |

### Supporting

| Tool | Purpose | When to Use |
|------|---------|-------------|
| `tools/check_ci_run.cmake` | Validate saved hosted-run JSON against an expected source, run completion, attempt, URL, all job terminal states, and exactly one successful required aggregate. [VERIFIED: tools/check_ci_run.cmake:30-78; verbatim: “ci_run_get(head STRING headSha)”; “ci_run_get(attempt NUMBER attempt)”; “ci_run_get(jobs ARRAY jobs)”] | For the saved candidate CI, release-PR CI, and release receipts; do not fetch fresh remote data in this phase. [CITED: .planning/milestones/v4.0-SHIP.md:53-55] |
| `tests/policy/ci_run.cmake` | Existing pass/fault controls for source mismatch, incomplete runs, missing/duplicate/skipped required aggregate, and visible advisory failures. [VERIFIED: tests/policy/ci_run.cmake:1-121; verbatim: “ci_run controls: 22 pass/fault cases”] | Reuse its registered control when the change affects receipt validation; otherwise existing local workflow is sufficient. |

**Installation:** None. This phase adds no package or external dependency. [CITED: .planning/phases/17-measured-milestone-tune-up/17-CONTEXT.md:15-22]

**Package Legitimacy Audit:** Not applicable; no external packages are installed.

## Architecture Patterns

### Recommended Project Structure

Keep the correction in the existing milestone/project records and the compact report alongside the phase artifacts. Keep raw receipts under the already-used ignored build evidence area; report commands, exit status, and run URL rather than committing raw logs. [CITED: .planning/phases/17-measured-milestone-tune-up/17-CONTEXT.md:32-40; VERIFIED: AGENTS.md:129-132; verbatim: “Evidence is a command, an exit status and a run URL, never a committed log.”]

### Pattern 1: Exact-source receipt audit

**What:** Run the existing CMake checker on each saved full response using the expected source SHA; retain non-required job outcomes in the report instead of treating the aggregate as the only result. [VERIFIED: tools/check_ci_run.cmake:30-78; verbatim: “Collect every non-success before deciding.”]

**When to use:** For the existing hosted receipts only. Do not run new remote/host actions or treat different sources, attempts, or event-skipped jobs as equivalent evidence. [VERIFIED: .planning/milestones/v4.0-SHIP.md:51-59; .planning/LESSONS.md:501-502; verbatim: “Keep each receipt tied to its source, gate and cap.”]

### Pattern 2: Compact evidence report

**What:** Score performance, CI/CD, ease of use, architecture, docs/README, and upgrade path; link the evidence; show before/after only for the correction; label observations, judgments, and censored work distinctly. [VERIFIED: .planning/seeds/SEED-007-milestone-tune-up.md:28-39; .planning/phases/17-measured-milestone-tune-up/17-CONTEXT.md:17,36-40]

**When to use:** For the recurring milestone tune-up; current measurements may support a no-change disposition rather than a pipeline edit. [VERIFIED: .planning/preparation/ENGINEERING.md:303-319; verbatim: “A single noisy or failed run does not justify a timeout, cache, matrix or workflow restructuring.”]

### Code Examples

Reuse the recorded local-gate and ordinary-workflow invocations. The prior shipping receipt states: “`python3 build/ship-local-gate.py`: exit 0; ignored helper invokes the unchanged `cmake --workflow --preset ci` under a temporary standard FIFO jobserver with one explicit token plus the implicit slot.” [CITED: .planning/milestones/v4.0-SHIP.md:10]

The existing hygiene invocation is recorded as: “`cmake --workflow --preset hygiene`” and “`sh scripts/emscripten.sh build/ci`”. The first is relevant to Phase 17; the Emscripten invocation is historical evidence, not required by this phase. [CITED: .planning/milestones/v4.0-SHIP.md:39]

### Anti-Patterns to Avoid

- **Mixing source records:** PR #46’s combined-source failure remains separate from the shipped PR #47 candidate and package v0.12.0. Later success does not repair the earlier source’s result. [VERIFIED: .planning/milestones/v4.0-SHIP.md:47,51; verbatim: “The combined source and its failed host evidence remain separate.”]
- **Calling censored work a pass:** the candidate Nightly was cancelled after property tests timed out and JUnit inventory failed; do not claim complete property acceptance or authorize a retry. [VERIFIED: .planning/milestones/v4.0-SHIP.md:59; verbatim: “No complete property acceptance is claimed and no retry is authorized by this receipt.”]
- **Inferring queue improvement from retries:** creation-to-latest-start spans include earlier attempts and cannot establish queue efficiency. [VERIFIED: .planning/milestones/v4.0-SHIP.md:59; verbatim: “initial creation-to-latest-start intervals include previous attempt history; they are not a reliable queue metric.”]
- **Changing pipeline policy from insufficient evidence:** retain cache, deadlines, job topology, and advisory/required membership; no new structural tuning is supported by these receipts. [VERIFIED: .planning/milestones/v4.0-SHIP.md:57-59; .planning/preparation/ENGINEERING.md:305-309]

## Don't Hand-Roll

| Problem | Don't Build | Use Instead | Why |
|---------|-------------|-------------|-----|
| Hosted run identity and required-check audit | A second JSON parser or ad hoc `gh` query | `tools/check_ci_run.cmake` plus the complete saved response | The existing checker rejects wrong-source, incomplete, or missing/unsuccessful required aggregate evidence while reporting all non-success jobs. [VERIFIED: tools/check_ci_run.cmake:1-3,30-78; verbatim: “accepting another source, incomplete run or missing required aggregate.”] |
| Local CI scheduling | New presets, timeout overrides, or another jobserver wrapper | Existing CMake workflow; existing FIFO helper only if resource contention warrants it | The helper already runs the ordinary workflow under temporary FIFO scheduling; the phase context explicitly says not to change presets or deadlines. [VERIFIED: build/ship-local-gate.py:1-12; .planning/phases/17-measured-milestone-tune-up/17-CONTEXT.md:24-26] |

**Key insight:** The failure-resistant receipt validator and the saved full-run responses already cover the hard part: preserving exact source identity and distinguishing required failures from advisory outcomes. A new audit layer would duplicate those contracts. [VERIFIED: tools/check_ci_run.cmake:60-78; tests/policy/ci_run.cmake:1-3,11-21]

## Common Pitfalls

### Pitfall 1: Treating historical shipping notes as the current release state

**What goes wrong:** Readers can mistake earlier preparation or failure checkpoints for the published release.

**How to avoid:** Keep the current publication paragraph and exact-source receipt links authoritative; preserve older history as historical. The current record states: “Earlier sections are historical checkpoints; they do not describe the current shipping status.” [VERIFIED: .planning/milestones/v4.0-SHIP.md:49-55]

### Pitfall 2: Overstating hosted efficiency evidence

**What goes wrong:** A cancelled/censored job, a retry interval, or one run may be mistaken for comparable queue, timeout, or optimization evidence.

**How to avoid:** Report queue measurement limits explicitly; retain job durations and critical path as observations; distinguish incomplete property work from acceptance; record the bounded no-change disposition. [VERIFIED: .planning/milestones/v4.0-SHIP.md:57-59; .planning/preparation/ENGINEERING.md:303-319]

### Pitfall 3: Reusing receipts across sources

**What goes wrong:** A later green run on another source can be mistaken for proof of an earlier terminal gate.

**How to avoid:** Bind each receipt to exact source and gate, and preserve all historical outcomes. The lesson states: “A later green result from a different source or scope does not erase a terminal exact-source gate.” [VERIFIED: .planning/LESSONS.md:501]

## State of the Art

| Old Approach | Current Approach | When Changed | Impact |
|--------------|------------------|--------------|--------|
| Earlier v4 shipping checkpoints described the candidate as pending or the combined-source attempt as failed. | The final shipping record links the successful PR #47 candidate, release PR #48, and v0.12.0 publication while keeping combined-source failure separate. | Reconciled at the next milestone, recorded in v4.0 ship receipt. | Phase 17 should correct current navigation/status claims without rewriting historical outcomes. [VERIFIED: .planning/milestones/v4.0-SHIP.md:47-55] |
| Earlier queue intervals might be read as comparable across retries. | Treat creation-to-latest-start intervals as invalid queue measurements when attempts are folded together. | Documented with v4.0 receipt. | Do not recommend queue-driven changes from those intervals. [VERIFIED: .planning/milestones/v4.0-SHIP.md:59] |

## Assumptions Log

| # | Claim | Section | Risk if Wrong |
|---|-------|---------|---------------|
| — | No unverified package or external technology claim is required for this in-repository documentation and local-check phase. | Standard Stack | None; no packages are recommended for installation. |

## Open Questions (RESOLVED)

1. **RESOLVED — local helper path:** the context now names the existing ignored `build/ship-local-gate.py`, matching the saved v4 receipt. The plan uses `python3 build/ship-local-gate.py` only as an existing verification input; no helper is created under `scripts/`. [Sources: .planning/phases/17-measured-milestone-tune-up/17-CONTEXT.md; .planning/milestones/v4.0-SHIP.md:10]

## Environment Availability

| Dependency | Required By | Available | Version | Fallback |
|------------|-------------|-----------|---------|----------|
| CMake | Native workflow, hygiene workflow, receipt checks | ✓ | 4.4.3 | — |
| Python 3 | Existing FIFO helper | ✓ | 3.14.4 | Run the unchanged CMake workflow directly if scheduling helper is unnecessary. |
| Git | Hygiene and source-diff evidence | ✓ | 2.41.0 | — |
| Selected GSD runtime | Planning queries | ✓ | `@opengsd/gsd-core` 1.20.0 | None needed; keep using the selected absolute runtime. |

Availability was checked locally with `cmake --version`, `python3 --version`, `git --version`, `node --version`, and `runtime-identity --raw`; these establish tool presence/version only. [VERIFIED: local probes in this research session]

## Sources

### Primary (HIGH confidence)

- Phase 17 context, roadmap, requirements, SEED-007, v4.0 ship receipt, and the current exact-source saved responses under the ignored milestone-preflight area.
- `tools/check_ci_run.cmake` and `tests/policy/ci_run.cmake` — exact-source, terminal-run, required aggregate, and fault-control behavior.
- `.planning/preparation/ENGINEERING.md` §11–12, `.planning/preparation/GSD-HANDOFF.md` §3.1, and `.planning/LESSONS.md` MU-GSD-025 / MU-CI-017–018 — documented proportionality and source-specific evidence lessons.

## Metadata

**Confidence breakdown:**
- Standard stack: HIGH — existing commands and validators are read directly from the repository.
- Architecture: HIGH — responsibilities and boundaries are explicit in phase context and existing checkers.
- Pitfalls: HIGH — source-specific outcomes and censored work are retained in the v4.0 receipt and lessons.

**Research date:** 2026-11-10
**Valid until:** 2026-12-10 (stable repository-local workflow; re-check saved/current receipts before execution)
