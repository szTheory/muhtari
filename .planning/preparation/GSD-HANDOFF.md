---
title: "GSD hand-off: kickoff, per-phase routine, settings and owner tasks"
summary: "How to start and run muhtari with OpenGSD one step at a time: the kickoff text, the settings and why, the per-phase and milestone routines, the owner's tasks, and what was checked before hand-off."
read_when: "Before the kickoff, before each GSD step, and whenever a GSD step behaves unexpectedly."
updated: 2026-10-16
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
   researchers; expect that cost. Each advisor gets the owner's fan-out brief: every decision point in its area
   researched through all relevant lenses, primary sources first, with an adversarial pass, ending in one
   coherent recommendation per point that the step then follows without asking again (owner, 2026-10-16).
3. `/gsd-plan-phase N`
4. `/gsd-execute-phase N` — code review, the test gate (`workflow.test_command`), the verifier and the phase
   close all run inside this step [GH.01].
5. Read `.planning/phases/NN-*/NN-VERIFICATION.md`:
   - `passed` → step 6;
   - `human_needed` → `/gsd-verify-work N`, which must first close every item that code, configuration, a
     test, CI or hosted evidence can prove, and hand the owner only the irreducible rest;
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
exit status and run URL. The owner is asked only for identity-bound consent or a judgement no test or artifact
can establish, with the reason stated.

**Closeout policy:** a fix merged after a phase's verification makes that verification "stale". Accept an
override closeout at milestone end when CI passes on the merged head and the milestone audit covers the phase;
do not spend sessions refreshing digests.

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
| `model_profile`, `resolve_model_ids` | inherit, omit | Every GSD subagent uses the session's model, so switching the session model between steps switches everything; no model IDs in a public file. nesturbator later ran well on `adaptive` — the alternative if per-role models are wanted |
| `workflow.research`, `plan_check`, `verifier`, `code_review` | true | GSD's own research and checks before and after every phase |
| `workflow.ui_phase`, `ui_safety_gate`, `ui_review`, `ai_integration_phase`, `api_coverage_gate`, `schema_push_detection`, `schema_drift_gate` | false | Written for web projects; they react to words like "screen" and "vector" |
| `workflow.security_enforcement` | false | A web threat-model checklist; fuzz targets and sanitizer builds are requirements instead |
| `workflow.nyquist_validation`, `pattern_mapper`, `tdd_mode` | false | GSD's own choice at coarse granularity; no code to map before phase 1 |
| `workflow.test_command` | `cmake --workflow --preset ci` | Without it the test gate runs `true`. It fails until phase 1 creates the preset |
| `workflow.test_gate_timeout` | 900 | Phase 1 replaces it with twice the measured gate time |
| `git.branching_strategy`, `git.create_tag` | none, false | The agent branches before discuss, so planning and code share one pull request; release-please owns tags |
| `git.base_branch`, `git.protected_branches` | main, [main] | Ship targets `main`; execute and ship warn if run on it |
| `hooks.community` | true | Turns on GSD's Conventional Commit check |

Subagent reasoning effort comes from the owner's global GSD agent install, not from this repository.

## 6. What keeps GSD from running on by itself

- GSD commands get only a phase number. An argument containing `--auto` or `--chain` makes one step start the
  next, and GSD tests for the substring [GH.01].
- `/gsd-next`, `/gsd-autonomous`, `/gsd-manager` and `/gsd-progress --next` or `--do` start the following step.
- In interactive mode the phase transition only moves STATE's pointer; it does not start the next phase [GH.01].
- `scripts/hygiene.sh` refuses a commit if `.planning/config.json` leaves interactive mode or turns a chain flag
  on.

## 7. Owner tasks

| When | Task |
|---|---|
| Done 2026-10-15 | The public repository `szTheory/muhtari` exists and `scripts/apply-repo-settings.sh` applied its settings (`--check` reports no drift); the required-check ruleset is applied in phase 1 once CI reports |
| Done 2026-10-15 | GitHub App `muhtari-release` (private, webhook off; repository permissions: Contents, Pull requests, Issues and Workflows read/write, Metadata read), installed on this repository only. The owner ran a one-off helper that registered it through GitHub's App manifest flow and piped the key from GitHub's response straight into `gh secret set MUHTARI_APP_PRIVATE_KEY`, once for Actions and once with `--app dependabot` (Dependabot-triggered runs see only Dependabot secrets); the key was never shown or saved. `MUHTARI_APP_CLIENT_ID` is an Actions variable. An installation token minted with the stored key reached only this repository. It is the project's only CI credential (ENGINEERING.md has the details) |
| If the key must change | On the App's settings page generate a new private key, run `gh secret set MUHTARI_APP_PRIVATE_KEY < <key file>` and again with `--app dependabot`, then delete the old key there and the file. Agents cannot read key files (GSD's secret-read guard), so these commands are yours |
| Done 2026-10-15 | RetroArch 1.22.2 with Metal (`retroarch-metal` cask). The `retroarch` cask is Intel-only; the App Store build cannot load outside cores |
| Any time | Optional `.env.local` (copy `.env.example`): `MUHTARI_ROM_DIR` for the local-only library preset with your own cart dumps; `MUHTARI_BIOS_DIR` for booting through your own BIOS dump. Results stay on your machine |
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
