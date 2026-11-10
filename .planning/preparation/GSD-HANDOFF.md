---
title: "GSD hand-off: kickoff, per-phase routine, settings and owner tasks"
summary: "How to start and run muhtari with OpenGSD one step at a time: the kickoff text, the settings and why, the per-phase and milestone routines, the owner's tasks, and what was checked before hand-off."
read_when: "Before the kickoff, before each GSD step, and whenever a GSD step behaves unexpectedly."
updated: 2026-10-31
---

# GSD hand-off

| Question | Section |
|---|---|
| What do I type to start? | 2 |
| What do I do each phase? | 3 |
| What happens at a milestone boundary? | 4 |
| Why is each setting what it is? | 5 |
| What stops GSD from running on by itself? | 6 |
| What only the owner can do | 7 |
| What was verified before hand-off | 8 |

## 1. Why this file exists

muhtari is built with OpenGSD (`@opengsd/gsd-core`). Preparation wrote the files GSD would otherwise collect by
asking questions — `PROJECT.md`, `REQUIREMENTS.md`, `config.json`, `research/SUMMARY.md` and `STACK.md` — so the
kickoff resumes at roadmap creation [GH.01]. GSD's own researcher still runs before every phase
(`workflow.research: true`), which is where it does its own research and sanity checks from here on.

**Active runtime (2026-10-29): Codex**, with OpenGSD 1.19.0 installed. The owner switches between
Codex and Claude Code; both installs remain available. Section 5 records the active routing. The kickoff
instructions below describe the original Claude setup, not a prerequisite for continuing in Codex.

## 2. Kickoff

**Before it** (about ten minutes):
- The repository exists on GitHub (`szTheory/muhtari`) with its settings applied. In any fresh clone, run
  `git config core.hooksPath .githooks` first.
- `cat ~/.claude/gsd-core/VERSION` prints `1.17.0`, the version these files were checked against (section 8).
  Use **Claude Code** for v1.0. Codex has GSD 1.16.0 installed; to use Codex instead, first reinstall GSD
  1.17.0 for Codex (the GSD installer with its Codex flag) and repeat the dry run in section 8 (unverified there).
- Do the owner tasks marked "kickoff" in section 7 (repository, release App, RetroArch), so phase 1 never
  blocks halfway.
- Skim `.planning/PROJECT.md` and `.planning/REQUIREMENTS.md`; change anything that is off now, because the
  roadmap is built from them.

**Then**, in a new Claude Code session at the repository root, paste this as one message:

```
/gsd-new-project Resume the pre-seeded bootstrap. PROJECT.md, REQUIREMENTS.md, config.json and research/SUMMARY.md were written during preparation and are final for initialization: keep them as they are and continue from roadmap creation. Do not run questioning, workflow preferences, project research or requirements scoping. Structure mode is Horizontal Layers (standard template, no Mode lines). Use the "## v1.0 …" milestone heading from research/SUMMARY.md, copy each suggested phase's "Canonical refs" line into ROADMAP.md, and add "UI hint: no" to every phase. Stop after the roadmap is approved and committed.
```

Following `AGENTS.md`, the agent first creates the branch `gsd/roadmap` (off `origin/main` when a remote exists).
Expect two prompts: **Structure** → "Horizontal Layers"; **Roadmap** → approve if the checklist holds, else
"Adjust phases" and say what to change. GSD then commits `ROADMAP.md`, `STATE.md` and the updated
`REQUIREMENTS.md` and stops; the agent pushes the branch and merges it by pull request (before phase 1 adds CI,
`gh pr merge --squash` merges at once).

**Roadmap checklist:** heading `## v1.0 …`; four phases, each ending in an automated release; phase 1 contains
shipped C code, CI and the first release; no phase whose goal is a study, decision or experiment; every phase has
"Canonical refs" and "UI hint: no" and no `**Mode:**` line; every v1 requirement is mapped exactly once.

**If GSD asks its full set of questions instead:** "What do you want to build?" → "Use .planning/PROJECT.md as
written"; saved defaults found → "Configure fresh" (never "Use as-is": the owner's global defaults are yolo mode
with web checks); mode/granularity/execution/git → Interactive, Coarse, Sequential, Yes; research/plan check/
verifier → Yes; compact content, AI models, PR body sections → No, Inherit, none; research the domain first →
Skip; feature scoping → "Use the existing REQUIREMENTS.md unchanged"; structure → Horizontal Layers. GSD's config
writer keeps an existing `config.json`, so these answers matter only if that file is missing [GH.01].

**After the kickoff**, these print the values shown:
```
node ~/.claude/gsd-core/bin/gsd-tools.cjs query config-get mode --raw                    # interactive
node ~/.claude/gsd-core/bin/gsd-tools.cjs query config-get workflow.auto_advance --raw   # false
node ~/.claude/gsd-core/bin/gsd-tools.cjs query config-get workflow.test_command --raw   # cmake --workflow --preset ci
```

## 3. Each phase — one session per step, so the model can change between steps

1. The agent branches `phase/NN-slug` off `origin/main` (AGENTS rule 9; the pre-commit hook refuses commits on
   `main` once a remote exists).
2. `/gsd-discuss-phase N` — the owner's global GSD profile turns on advisor mode, so discuss also spawns advisor
   researchers; expect that cost. Each advisor gets `.planning/research/fan-out/prompt.txt`: every decision point
   in its area is researched through relevant lenses, primary sources first, with an adversarial pass, ending in one
   coherent recommendation per point (owner, 2026-10-16).
3. `/gsd-plan-phase N`
4. `/gsd-execute-phase N` — code review, the test gate (`workflow.test_command`), the verifier and the phase
   close all run inside this step [GH.01].
5. Read `.planning/phases/NN-*/NN-VERIFICATION.md`:
   - `passed` → step 6;
   - `human_needed` → `/gsd-verify-work N`, which closes every item with code, configuration, a test, CI,
     hosted evidence or the agent's own reading, viewing and running, and hands the owner only what AGENTS rule 6
     leaves to them;
   - `gaps_found` → one gap round (`/gsd-plan-phase N` for the gaps, then execute); a second only with new
     evidence.
6. `/gsd-ship N` opens the pull request titled "Phase N: name". The agent retitles it as a Conventional Commit
   (`feat: …` for a capability, `fix: …`, `ci: …` for CI-only work so no release is cut), runs
   `gh pr merge --auto --squash` in a shell — never inside GSD command text, where GSD would read the option as
   its own auto flag — and confirms that a CI run exists on the head commit (ship pushes a CI-skip commit; the
   repository's squash settings keep that token off `main`).
7. CI goes green → the pull request merges → release-please opens its release pull request with the release App's
   token → it merges when CI passes → the release publishes itself with attested archives.
8. The agent checks that the release pull request is green (a red release PR silently stops all releases).
9. Optional, about two minutes, outside the agent's sandbox: run the README's install line and play one game in
   RetroArch. This is never recorded as a verification item.

**Verification default (settled 2026-10-15; never re-proposed):** automation first. Deterministic behaviour goes
in local tests; integration, end-to-end, smoke and seam checks go in CI when they catch recurring regressions; a
one-time hosted outcome is collected by a read-only evidence command. Passing results are recorded as command,
exit status and run URL. The owner is asked only for identity-bound consent or a run on their own hardware, with
the reason stated. Narrowed 2026-10-16 and reaffirmed 2026-10-31: UAT is an agent-owned evidence register, not a
blanket owner questionnaire. The agent closes every item with `human_judgment: false` using available checks and
receipts, and leaves genuinely human-only items blocked. Judging docs, images and RetroArch's menus is the agent's
job; a truth only a merge can show is confirmed at ship (step 8), never in UAT. At each CI/release run, inspect
observed queue, durations, critical path and cancelled work for quality-preserving efficiency gains; change
timeouts, caches or workflow structure only when evidence supports the improvement.

Reaffirmed 2026-11-09: choose the smallest check that gives useful feedback. A one-off, throwaway local check is
valid; add a check to CI only when recurring regression prevention justifies its maintenance and runtime cost.
Review each CI/release run for measured queue, critical-path, duration and cancelled-work improvements while
preserving pass/fault coverage and release evidence. Record when no safe improvement is supported, then move on;
do not reopen this settled policy during later UATs.

Autonomous milestone direction, owner 2026-11-10: when the owner explicitly invokes `$gsd-autonomous`, apply the
fan-out brief and adopt its recommendation for ordinary in-scope decisions without pausing for routine “accept all”
prompts. The agent continues through the requested milestone's phases and lifecycle, carrying out useful local,
offline verification and review. Pause only for a material scope, authority, risk, cost or irreversible-action
boundary, genuinely owner-only validation, or a failed gate that needs changed scope. Existing host, identity-bound
and remote stops remain unchanged; independent authorized local work continues around them. Reconcile the live plan
index on entry and after each phase: reuse existing context and a reviewed runnable plan instead of re-discussing or
replanning it. For a phase without context, use the fan-out brief and dispatch specialists only where their
independent evidence can change the choice, scope or acceptance. For locally resolvable verification gaps, choose
the bounded gap-closure path, fix the cause and verify it; never skip a phase or call an open requirement accepted
just to advance. Do not pass `--auto` or `--chain` to any GSD command.

**Closeout policy:** a fix merged after a phase's verification makes that verification "stale". Accept an
override closeout at milestone end when CI passes on the merged head and the milestone audit covers the phase;
do not spend sessions refreshing digests.

## 3.1. Recover a stalled step without restarting the phase

For repeated retries or metadata repairs, use the
[bounded specialist prompt](../research/workflow-recovery/prompt.txt) and
[recovery decision](../research/workflow-recovery/RECOMMENDATION.md).

- Identify the failing consumer, source/run/attempt and contract. Audit every job
  before attributing a run to one known failure. Keep the existing required
  aggregate authoritative; keep advisory failures visible and advisory.
  Save the complete response including attempt and run the CMake audit in
  [maintaining](../../docs/maintaining.md#inspecting-a-ci-run) before the handoff.
- Run changed standalone consumers locally, batch fixes, then run hosted acceptance.
  A retry needs a transient hypothesis and finite attempt; preserve failures.
  Calibrate deadlines from complete successful work under DEC.47.
- The orchestrator owns STATE/ROADMAP position and accepted counts. Executors own
  task commits and evidence. Establish required SDK labels once. Summarized counts
  include designed stops; they do not establish accepted completion.
- The installed summary scanner is advisory. Its backticked-prose heuristic can
  mistake a command for a missing file. Check declared files, commits and consumers;
  record a reproduced heuristic limitation once.
- Keep canonical halted summaries and failed history. Revise only the changed
  decision using new evidence; close a halt only after revised acceptance passes.
  Assert the intended next plan is ready, then stop this GSD step.
- Label dispatch's actual base/performance scope. Same-head repeatability proves
  repeatability; stable PR-base comparison proves phase-versus-main behavior.
  Avoid documentation commits solely to manufacture identical dispatch parents.
- Report new evidence, changed blockers or material wait estimates. A mandatory
  periodic heartbeat can be one short unchanged-status line.
- Stop specialist expansion when another pass would not change a choice.
  Optional publication-harness/DAG improvements become seeds, not recovery gates.

## 4. Milestone boundaries

On a `milestone/vX.Y-slug` branch, one session each: `/gsd-audit-milestone` → `/gsd-complete-milestone` →
`/gsd-new-milestone`. Within those steps the agent also:
- refreshes `.planning/PRODUCT-ROADMAP.md` (reconcile shipped work, keep unresolved debt, re-rank
  Immediate/Near/Mid/Long, record why) and snapshots it as `milestones/vX.Y-PRODUCT-ROADMAP.md`;
- refreshes `.planning/preparation/PROMPT-COVERAGE.md` (every owner ask still has a home and a status);
- surfaces the seeds whose triggers fired; the tune-up seed is the first phase of every new milestone;
- offers a lesson export ("export lessons", `.planning/LESSONS.md`).

## 5. Settings and why

`config.json` was written by GSD's own writer with `GSD_HOME` pointing at an empty directory (unset or empty, it falls back to the home directory), so none of the owner's global defaults
leaked in, and read back with `config-get` (section 8).

| Setting | Value | Why |
|---|---|---|
| `mode` | interactive | With yolo, the end of one step can start the next [GH.01] |
| `workflow.auto_advance`, `workflow._auto_chain_active` | false | Stops discuss, plan and execute from chaining |
| `granularity` | coarse | About four phases per milestone |
| `parallelization`, `workflow.use_worktrees` | false | A small, coupled codebase; no worktree paths in tracked files |
| `runtime`, `model_profile`, `resolve_model_ids` | codex, adaptive, false | Owner choice (2026-10-29): Codex is active until switched again; role tiers select separate models. `inherit` is the fallback when one session model should drive every subagent |
| `model_profile_overrides.codex` | opus gpt-6.1-sol; sonnet/haiku gpt-6-luna | Explicit project overrides are required: OpenGSD 1.19.0 ignores global model overrides when project config exists. The Claude install and its global overrides remain available |
| `effort.routing_tier_defaults` | light xhigh, standard xhigh, heavy high | Owner's requested effort levels, also saved in the global defaults |
| `workflow.research`, `plan_check`, `verifier`, `code_review` | true | GSD's own research and checks before and after every phase |
| `workflow.ui_phase`, `ui_safety_gate`, `ui_review`, `ai_integration_phase`, `api_coverage_gate`, `schema_push_detection`, `schema_drift_gate` | false | Written for web projects; they react to words like "screen" and "vector" |
| `workflow.security_enforcement` | false | A web threat-model checklist; fuzz targets and sanitizer builds are requirements instead |
| `workflow.nyquist_validation`, `pattern_mapper`, `tdd_mode` | false | GSD's own choice at coarse granularity; no code to map before phase 1 |
| `workflow.test_command` | `cmake --workflow --preset ci` | Without it the test gate runs `true`. It fails until phase 1 creates the preset |
| `workflow.test_gate_timeout` | 0 | Supported no-timer mode for the one-shot CMake workflow; each CTest test keeps its enforced deadline. The former60s outer cap could truncate allowed publication work; resolved debug session preplan-lightgun-evidence records the evidence. |
| `git.branching_strategy`, `git.create_tag` | none, false | The agent branches before discuss, so planning and code share one pull request; release-please owns tags |
| `git.base_branch`, `git.protected_branches` | main, [main] | Ship targets `main`; execute and ship warn if run on it |
| `hooks.community` | true | Turns on GSD's Conventional Commit check |

Subagent reasoning effort is set by `effort.routing_tier_defaults` above. Codex dispatch passes the resolved
model and effort explicitly; installed agent TOML files need no model pins. When switching runtimes, update
`runtime` in the project config and global defaults, then check `query resolve-model` through that install.
Keep each runtime's model overrides under its own key so switching preserves the other runtime's choices.

## 6. Explicit GSD continuation

- Normal GSD commands do not start the next step; an argument containing `--auto` or `--chain` does, and GSD
  tests for the substring [GH.01]. Keep those flags forbidden.
- `/gsd-autonomous` is an explicit opt-in. Run it only when the owner requests autonomous continuation, and honor
  the requested milestone range plus the recommendation and safety rules in AGENTS.md rule 8.
- `/gsd-next`, `/gsd-manager` and `/gsd-progress --next` or `--do` remain outside the autonomous entrypoint.
- In interactive mode the ordinary phase transition only moves STATE's pointer; it does not start the next phase
  [GH.01].
- `scripts/hygiene.sh` refuses a commit if `.planning/config.json` leaves interactive mode or turns a chain flag
  on.

## 7. Owner tasks

| When | Task |
|---|---|
| Done 2026-10-15 | The public repository `szTheory/muhtari` exists and `scripts/apply-repo-settings.sh` applied its settings (`--check` reports no drift); the required-check ruleset is applied in phase 1 once CI reports |
| Done 2026-10-15 | GitHub App `muhtari-release` (private, webhook off; repository permissions: Contents, Pull requests, Issues and Workflows read/write, Metadata read), installed on this repository only. The owner ran a one-off helper that registered it through GitHub's App manifest flow and piped the key from GitHub's response straight into `gh secret set MUHTARI_APP_PRIVATE_KEY`, once for Actions and once with `--app dependabot` (Dependabot-triggered runs see only Dependabot secrets); the key was never shown or saved. `MUHTARI_APP_CLIENT_ID` is an Actions variable. An installation token minted with the stored key reached only this repository. It is the project's only CI credential (ENGINEERING.md has the details) |
| If the key must change | On the App's settings page generate a new private key, run `gh secret set MUHTARI_APP_PRIVATE_KEY < <key file>` and again with `--app dependabot`, then delete the old key there and the file. Agents cannot read key files (GSD's secret-read guard), so these commands are yours |
| Done 2026-10-15 | RetroArch 1.22.2 with Metal (`retroarch-metal` cask). The `retroarch` cask is Intel-only; the App Store build cannot load outside cores |
| Any time | Optional: run `MUHTARI_ROM_DIR=<dump dir> [MUHTARI_BIOS=<file>] cmake --workflow --preset library` with your own cart dumps and, to boot through your own BIOS dump, `MUHTARI_BIOS`. The counted `library.local` pass writes `build/library/local.csv`; results stay on your machine (superseded by Phase 11 on 2026-10-29: no dotenv file or template exists) |
| Each phase close | Optional two-minute smoke (section 3 step 9) |
| Later | In Playstead, adding an `a7800` system ID is a one-way decision |
| Never in this repository | `/gsd-profile-user` — it writes a developer profile into the tracked `.claude/CLAUDE.md` |

## 8. What was checked

All on 2026-10-15, against `@opengsd/gsd-core` 1.17.0.
- **Config.** Written by GSD's own writer with `GSD_HOME` pointing at an empty directory; read back with
  `config-get` (mode interactive, coarse, inherit/omit, both chain flags false, the test command, branching none,
  create_tag false, community hooks on). No `preferences`, `effort` or model IDs reached the file.
- **Resume state.** `query init.new-project` on a copy of this tree: `project_exists` true, `init_incomplete` true,
  `is_brownfield` false, `needs_codebase_map` false — the state the kickoff's resume path needs.
- **Roadmapper, run twice on copies** (GSD's `gsd-roadmapper` agent with the kickoff step's prompt): four phases;
  24/24, then 25/25 requirements mapped once; the `## v1.0 …` heading recognised by `roadmap.analyze` and
  `init.progress` (v1.0, four phases); `validate.consistency` passed with no error or warning; `list-seeds` reads
  seven seeds; no `**Mode:**` line; "UI hint: no" and "Canonical refs" on every phase. A context-monitor hook once
  wrote a false "stopped at" line into the copy's STATE.md; glance at STATE.md after the kickoff.
- **Instruction file.** GSD leaves a marker-free `.claude/CLAUDE.md` untouched and appends its profile placeholder
  only to files that already carry GSD markers (`bin/lib/profile-output.cjs`).
- **Hygiene.** `scripts/hygiene.sh --tree` passes on this tree. In a scratch repository it rejected planted home
  paths, an e-mail address, `.a78` images by name and by content, an unlisted binary, GPL text in code, a
  non-interactive config, a chain flag, a commit on `main` with a remote, a non-noreply identity, and the same in
  history; GitHub and AI-co-author noreply addresses and runner homes passed. shellcheck is clean.
- **Fact-check** by a fresh agent: 58 load-bearing claims re-fetched from primary sources (licences at their pins,
  timing arithmetic, Software Guide DMA costs, A78 offsets, the rcheevos map and hash, BIOS digests, the RetroArch
  DMG digest, release-please 17.6.0 semantics, GitHub token and Dependabot rules, runner facts, action SHAs, GSD
  behaviours): 53 held, 5 corrected in place. Both hardware conflicts settled: RIOT port B powers on at $FF, and
  $2800–$2FFF is not a reliable RAM mirror.
- **Fresh-eyes review** of the files GSD reads: 11 findings. Applied: movie replay moved into v1.0 phase 3; the
  probe and loader moved to phase 2; the a7800 oracle moved to v4.0; owner dependencies stated in requirements;
  one traceability row per requirement; FRAME-09 (allocations, frame-time percentiles, RSS); contradictions fixed.
  Rejected: adding GSD markers to `.claude/CLAUDE.md` (see "Instruction file").
- **Researchers' experiments (tested):** 7800basic samples build byte-identically on macOS arm64 and Linux arm64
  and x86-64; OpenBIOS without KiloParsec builds reproducibly; the a7800 v5.2 binary runs headless and
  deterministically under Xvfb; the rcheevos 7800 map is identical across v10–v12.
- **Not checked:** a complete `/gsd-new-project` session (only its init query and its roadmapper, on copies);
  the fixture toolchain on Windows; anything on real hardware.

## Sources

| ID | Tier | Source | Pin or access date | Supports |
|---|---|---|---|---|
| GH.01 | T1 | `@opengsd/gsd-core` installed package: `workflows/new-project.md`, `execute-phase.md`, `ship.md`, `verify-work.md`, `transition.md`, `discuss-phase.md`, `plan-phase.md`; `bin/lib/init.cjs`, `config.cjs`, `config-loader.cjs`, `roadmap-parser.cjs`, `runtime-name-policy.cjs`, `profile-output.cjs` | 1.17.0, read 2026-10-15 | Resume path, chaining, branching, config, ship behaviour, milestone headings, instruction file |
| GH.02 | T1 | nesturbator `.planning/preparation/GSD-HANDOFF.md`, `RETROSPECTIVE.md`, `MILESTONES.md` | main, read 2026-10-15 | Kickoff text, routine, closeout overrides |
| GH.03 | T1 | gabbaboy `.planning/context/LESSONS.md`, `DECISIONS.md` (D-025, D-069) | main, read 2026-10-15 | Verification default, confidence-qualified models, release PR blocking |

### Archived phase selection at ship (verified 2026-11-10)

OpenGSD 1.20.0 `init.phase-op` can deliberately replace an archived phase's directory with null when the same number remains in ROADMAP. Ship must not interpret that projection as missing implementation or create a new phase. For the completed v4.0 branch, run `$gsd-ship 16`; the agent resolves its existing directory with the same runtime's `query find-phase 16 --raw`, checks that it is the expected `milestones/v4.0-phases/16-emscripten-libretro-core`, and uses that directory for the normal verification, artifact and PR-body reads. The canonical verification must still return `passed`, and `roadmap.analyze` must still record Phase 16 complete. This is an archive-path adapter, never a waiver of a ship gate. The PR covers both phases on the milestone branch.

Keep compact completed phase records and detail sections for roadmap consumers. Archive fingerprint relocation is verifier-owned and requires byte-equivalence with the pre-archive files. The owner does not repair metadata or switch checkouts; the agent owns this preflight on resumption. No host/remote allowance or publication authority is inferred from the adapter.

### Approved isolated v4.0 publication (2026-11-10)

The owner approved publishing the verified v4.0-only candidate after combined PR #46 failed its inherited held-input gate. Resume `$gsd-ship 16` by selecting `.worktrees/phase-16-ship` internally; do not republish or rerun the combined source. The candidate keeps main's existing checks and adds only the offline Emscripten lane. Historical v3.0 planning records describe preserved, unmerged source; they do not imply that source ships in the isolated PR. All host stops remain binding.

Finalize planning before the first candidate CI run. A preparation head with `[ci skip]` can create the PR without launching a duplicate matrix; then commit the known PR number in a normal ship-note commit and push once to register exact-head CI. Verify that head and its actual run; zero checks never constitute a pass. This ordering avoids the previously observed skip-token blockage and cancelled matrix work without changing checks, deadlines, caches or concurrency.
