# Working preferences

## Communication and workflow preferences

- Keep isolated Git checkouts inside the repository under `.worktrees/`, using an ignored directory, so they stay together without cluttering the parent projects folder.

- Speak to the user as a software engineer and colleague. Use plain, direct language. Keep necessary technical terms and explain unfamiliar workflow language through its practical consequence.
- End with the actual outcome, what remains broken or blocked, and the next useful action. Usually one short paragraph or at most three short bullets is enough. State clearly when no user action is needed.
- At handoff, say explicitly when context can be cleared and give the exact next command. The agent owns service-health and receipt checks in the next session; the user should not have to interpret a vague review prerequisite before resuming. Preserve exhausted live and remote allowances; local workloads follow the standing preauthorization below.
- Keep hashes, internal task and threat IDs, exhaustive test counts, audit history, and workflow metadata in linked artifacts unless they are needed for a real decision or explicitly requested. Say what the checks establish in ordinary language.
- Give progress updates when there is new information. When a required update interval arrives during a long wait, use one brief factual line; do not repeat the whole plan or invent progress.
- Ask for user input only when the answer changes scope, authority, risk, cost, an irreversible action, or a required phase/milestone boundary. Explain the concrete choice and why it matters. Continue routine authorized work and bookkeeping without asking for acknowledgments or exact phrases.
- Owner preference (2026-10-31, clarified 2026-11-05): when given a concrete, bounded recommendation, proceed without routine approval. Ask only for a materially new remote/host/identity-bound or irreversible action, or when a local workload is expected to take about an hour or more. Local attempt caps are covered by the standing local-work authorization below; failure causes still need investigation and resolution before a retry.
- Owner preference (2026-11-02): when the owner says to keep going or asks to batch an authorized workflow, carry that scope through dependent GSD steps, implementation, local verification, review and bookkeeping without routine handoffs. Select the full runnable scope instead of requiring wave-by-wave invocations. Keep real dependencies, failed gates, explicit caps, host/CI stops and identity-bound or irreversible authorization boundaries intact.
- Owner preference (2026-11-03): give one concise recommendation and the exact next command, or say plainly that no command is valid under the current stop. Assume the owner will follow the recommendation; proceed without confirmation for internal steps. Ask only when a serious scope, authority, risk or explicit-cap boundary changes. Avoid workflow jargon, long evidence dumps and multiple command options; link details instead.
- Owner preference (2026-11-05): at the end of every task or handoff, state the exact OpenGSD command to run after clearing context. If no command is valid, say so plainly and name the concrete blocker and who acts next.
- Owner preference (2026-11-05): local builds, tests, CI workflows, and GitHub Actions workflows run through a local runner are pre-authorized; proceed without asking, including bounded retries and renewal of local attempt allowances for diagnosis or verification, unless expected runtime is about one hour or more. This does not waive the need to investigate and resolve a failure before retrying it. It covers local work only; remote dispatch, live host actions, identity-bound consent, publication, merge, and irreversible changes keep their existing boundaries.
- Owner preference (2026-11-05): do not ask the owner to switch directories between checkouts. Select the correct worktree internally with the tool working directory and leave the owner's shell/root checkout unchanged. Explain the branch/worktree reason only when it affects which snapshot is safe to use.
- Owner preference (2026-11-06): when asked to get a tangled OpenGSD state back on track, reconcile the canonical root checkout, worktree ancestry and dirt, milestone roadmap, STATE, verification and saved receipts before recommending a command. Repair recoverable local planning drift and continue the authorized work; do not hand back a stale or invalid command as the next step.
- Owner preference (2026-11-06): before merging parallel worktrees, compare committed ancestry and dirty-file contents against the root branch and `origin/main`. Keep the root checkout canonical; do not merge or cherry-pick changes already ancestral or represented by newer root content, and preserve dirty checkouts until their uncommitted contents are reconciled.
- Owner preference (2026-11-08): prefer simulated, offline and CLI checks; avoid recurring control of the owner's Mac. Run a real UI/input check only when it answers a concrete question that automation cannot, and then provide one isolated owner-run command with its evidence value and limits. Keep the relevant host and permission scope closed unless separately authorized.
- Owner preference (2026-11-06): a newly authorized bounded evidence scope is carried through its plan, local prerequisites, one explicitly capped host attempt, evidence review and bookkeeping. Keep older attempts closed and other phase-specific stops intact; at handoff give one exact next OpenGSD command, or say which actor must act and why no command is valid.
- Owner preference (2026-11-03, clarified 2026-11-05): carry approved work through implementation, local builds/tests, reviews, planning bookkeeping, and routine CI without step-by-step confirmation. A local attempt cap does not require renewed approval under the standing preauthorization above. Pause for a real plan checkpoint, a materially new remote/host/identity-bound or irreversible action, or a local workload expected to take about an hour or more.
- Owner preference (2026-11-03, clarified 2026-11-05): keep resource caps concrete and measured. Local runs and bounded retries under the standing authorization are pre-approved; qualify syntax and capability before launch, count workload only after it starts, and investigate a failure before retrying. Do not renew live or remote allowances under this local authorization.
- Owner preference (2026-11-03): distinguish a blocked phase's acceptance status from runnable prerequisite/gap work. When an execution command selects only a blocker-removal plan, name that plan and say whether it reaches a user decision checkpoint; do not imply phase acceptance is ready or being completed.
- Owner preference (2026-11-03): never end with "do nothing." Find and take the next safe, authorized action that advances the goal, including independent roadmap work when the current path is blocked. If every path needs new authority, name the exact blocker and give the single recommended decision and command that resumes progress.
- Preserve safety stops for live, remote, identity-bound, and irreversible actions when their cause remains unresolved. The standing local-work authorization covers bounded local diagnosis and verification without another approval; it does not renew a live or remote allowance or start another phase.
- Record explicit user corrections in durable instructions. Apply these preferences to OpenGSD planning, execution, reviews, progress updates, and final summaries. Keep detailed evidence available without making the user translate process records into takeaways.
- When authorized work remains and no user action is needed, keep working. Carry planning through implementation, tests and review within the selected phase; stop for a real decision or phase/milestone boundary. Removing an unnecessary confirmation is not a reason to leave the task idle.
- Halt scope: name the specific prohibited action. A spent host or remote CI allowance remains blocked; local CI and local-runner workflows follow the standing preauthorization above. Continue independent local work and give its concrete next action. Never describe a scope-specific stop as a project-wide idle state.
- Worktree handoff: keep the owner in the current checkout and use the tool working directory to operate in another existing worktree. Give a manual `cd` only when the owner explicitly needs to run a command there and no workspace selector can target it.
- At every pause or handoff, state in chat what is happening, the concrete blocker and consequence, your recommended next step, and exactly who acts next. Explicitly say when waiting for the user versus continuing work.
- Summarize every risk or decision in plain language before asking; make linked documents optional detail. When the user has no strong preference, recommend a concrete path. An uninformed or indifferent response is not risk acceptance.
- Prefer the least disruptive evidence-supported remedy. For stuck project tests, inspect attributable processes and consider targeted cleanup first; recommend a whole-machine restart only when its necessity and impact are established.

--- project-doc ---

# muhtari

An Atari 7800 emulator core in C. This repository ships three things: a C library, a headless
command-line runner (`muhtari-run`), and a libretro core. RetroArch or another host provides the window.

Project state lives in `.planning/` (PROJECT.md, REQUIREMENTS.md, ROADMAP.md, STATE.md). Reference material
with sources is indexed in `.planning/preparation/README.md`; open the file it points to before designing or
building a subsystem. Settled choices are in `.planning/preparation/DECISIONS.md`; do not reopen one without new
evidence.

## Rules

1. **Reuse permissively, never GPL.** MIT, BSD, zlib and CC0 code may seed a module, with its notice kept as
   `PROVENANCE.md` describes. GPL, LGPL and unlicensed emulator source is never opened and no copy of it is kept
   in the workspace; reference emulators run only as released binaries.
2. **Three deliverables.** Library, runner, libretro core. No window, audio-device or input-device code.
3. **Clean tree.** No ROM, BIOS or save bytes except fixtures listed in `tests/roms/manifest.txt`. No personal
   paths, e-mail addresses or names. Commits use the GitHub noreply identity. `scripts/hygiene.sh` checks every
   commit and push (enable the hooks with `git config core.hooksPath .githooks`).
4. **Deterministic, bounded core.** Integer-only. All state lives in the instance. The same inputs give the same
   frame and audio hashes on every platform. Every call returns within a bounded amount of emulated time, even
   for a hostile or jammed ROM.
5. **Tested and documented together, within budgets.** Every behaviour change lands with a test that
   `cmake --workflow --preset ci` runs. The README stays user-facing and short; testing, CI and maintainer docs
   live in `docs/`; the public header stays terse and puts behaviour prose in `docs/`.
6. **Verification is automation-first; the owner verifies nothing by default.** This is settled; do not
   re-propose it. Deterministic checks go in tests; integration, end-to-end and smoke checks go in CI when they
   catch recurring regressions; a check with no recurring value is run once by the agent and recorded as a
   command and an exit status. The agent makes the judgement calls itself: it reads docs against the code and
   views images and screenshots. It runs RetroArch only for an authorized check with a concrete evidence value.
   Prefer simulated, offline and CLI checks; do not take over the owner's Mac by default. If physical input is
   uniquely informative, prepare one isolated owner-run command and explain exactly what it proves and what it
   cannot prove. What only a merge can show is confirmed at ship (rule 9), not in UAT. During
   `$gsd-verify-work`, the agent gathers and records every result it can verify from local checks, CI receipts,
   logs, screenshots and project artifacts; do not ask for blanket UAT confirmation when no item requires human
   judgment. Ask the owner only for identity-bound consent (the App key, admin settings, a merge) or a run on
   their own hardware, and say why.
7. **Proportional gates.** Repository checks use CMake `-P` or POSIX sh and reuse existing mechanisms.
   A new or materially changed check names its concrete fault and exercises representative pass/fault behavior;
   consolidation preserves required behavior and fault detection. For architecture review and new resource
   bounds, follow `.planning/preparation/ENGINEERING.md` §11. At most two code-review fix rounds and one gap
   round per phase. Optional test/tooling improvements become seeds; violations of an existing required
   contract remain blocking wherever they live. Re-plan a gap only with new evidence.
   During GSD execution, or when workflow retries or metadata repairs repeat, follow
   `.planning/preparation/GSD-HANDOFF.md` §3.1 and its bounded fan-out prompt.
8. **OpenGSD step boundaries.** Major steps include discuss, plan, execute, phase or milestone management, audit,
   ship and archive. One major step per session is the default, but an explicit owner request to keep going or batch
   an already authorized scope overrides that handoff: continue through dependent steps and all runnable waves without
   requiring another invocation. Report concrete progress and stop only at a real decision boundary, failed gate that
   requires changed scope, exhausted live/remote attempt cap, or an action excluded from approval; local runs are covered
   by the standing preauthorization above. A `gsd-debug continue` or other
   micro-step inside an authorized scope is not a new major step; carry the approved scope through its gates,
   implementation, controls, review and evidence without routine prompts.
   Before each `gsd-debug continue`, compare `Current Focus` with the checkpoint's latest dated closeout and the
   relevant phase summary/STATE. Each continuation must add evidence, complete an authorized action, or resolve a
   blocker. If the same blocker remains, hand off its exact missing input, consequence, next action and command;
   resume only after new evidence or authority arrives.
   An explicit owner request for `$gsd-autonomous` authorizes running the autonomous workflow through the requested
   milestone; when a phase is blocked, keep advancing independent authorized local work while preserving its exact
   safety stops. This does not renew host/remote attempts or permit command arguments containing `--auto` or `--chain`.
   During an explicitly requested autonomous run, use `.planning/research/fan-out/prompt.txt` for phase decisions and
   adopt its recommended answer for ordinary, in-scope gray areas without routine acceptance prompts. Ask only when
   the recommendation would change scope, authority, risk, cost, an irreversible action, or a genuine owner-only
   validation; if no safe recommendation exists, continue independent authorized work and preserve the exact stop.
   For locally resolvable verification gaps, run the bounded gap-closure path and its required checks; do not skip a
   phase or record unresolved requirements as accepted merely to advance. Stop only when the fix crosses a real
   boundary or its documented retry cap is exhausted.
   On each autonomous context resume, treat prior summaries as pointers: reconcile the actual branch and dirty tree,
   current verification reports, review disposition, and runnable plan index before repeating a blocker or claiming
   there is no work. Continue independent authorized local work when one phase has no runnable plan.
   Keep evidence source- and gate-specific: a later pass on another source or scope does not reopen a terminal attempt,
   satisfy its exact-source gate, or replace the fresh authority required by its stop.
   Never run `/gsd-next`, `/gsd-manager`, `/gsd-progress --next` or `--do`, any GSD command whose
   arguments contain `--auto` or `--chain`, or `/gsd-profile-user`, and never set the hygiene bypasses `MUHTARI_ALLOW_CHAIN` or
   `MUHTARI_ALLOW_MAIN_COMMIT`. Stop inside a scope only at a real decision boundary, failed gate requiring changed
   scope, exhausted live/remote attempt cap, or an action excluded from the approval; local runs are covered by the
   standing preauthorization above.
   Resolve the GSD runtime using `gsd-core/references/gsd-run-resolver.md` and use that same executable for status,
   planning and execution queries; do not trust a `gsd_run` found on `PATH` unless its runtime identity and version
   match the workflow-selected installation.
9. **Branch, then pull request.** Never commit on `main` once a remote exists. Before the first GSD step that
   commits, branch off `origin/main`: `phase/NN-slug` for a phase, `milestone/vX.Y-slug` for milestone steps,
   `gsd/roadmap` for the kickoff. After `/gsd-ship`, give the pull request a Conventional Commit title, run
   `gh pr merge --auto --squash` in a shell (never inside GSD command text), and confirm a CI run exists on the
   head commit; after the merge, confirm the release pull request is green. Whenever reviewing a running or
   completed CI/release run, inspect its queue, critical path, job durations and cancelled work for low-effort
   efficiency gains. Preserve test/fault coverage and release evidence; require measured, multi-run evidence for
   structural, timeout or cache changes, and record when the run supports no safe improvement.
10. **Evidence is a command, an exit status and a run URL**, never a committed log.
11. **Lessons.** Before planning a phase or changing a subsystem, read relevant entries in `.planning/LESSONS.md`;
    record newly reproduced lessons there with evidence. Inbound lessons remain hypotheses until reproduced here.
    Keep lessons in project memory by default; export from the ledger's template only when explicitly requested.

## GSD summaries

Every plan SUMMARY carries `requirements-completed:` and a block-style `coverage:` list. Each entry has `id`,
`description`, `requirement`, `verification` (kind one of unit, integration, e2e, automated_ui,
manual_procedural, other; `ref`; `status`) and `human_judgment` (true only for what rule 6 leaves to the
owner). No flow style `{}` and no inline `#` comments in that block.

## Style

Small modules, plain control flow, fixed-width integer types. Public names start with `muhtari_`; each module
prefixes its own names. A hardware comment says what the hardware does and cites its source (a document, or a
`.planning/preparation` section). No planning IDs in code, test names or user docs. To decide how to do
something, follow the ladder at ponytail.dev: remove the need, reuse what is here, use the standard library, use
the platform, and only then write the minimum new code. Another copy and paste beats another dependency: when
outside code is needed, a permissive seed with its notice is preferred to a new package, so the dependency tree
stays small and flat.
