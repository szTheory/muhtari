---
audit_acknowledged:
  milestone: v7.0
  at: 2026-11-14
  status: unknown
---

# Workspace reconciliation — 2026-11-13

## Canonical planning state

At initial inspection, the canonical root checkout was clean on `gsd/roadmap`,
two documentation commits ahead of `origin/main`. Those commits update Phase 20
review evidence and reconcile the v6.0 publication receipt. Local `STATE.md`, `ROADMAP.md`, `continue.md`, and
the v6.0 ship receipt agree: v6.0 shipped as v0.14.0; successor scope is
unselected.
At inspection, `origin/main` still described publication as pending, so the local
documentation needs the normal PR path before the remote planning copy is current.
The prior `state_head` pointed to a commit outside this root branch's history;
the selected GSD state writer refreshed it to the current root HEAD while
recording this reconciliation.

## Archived-phase progress-index limitation

OpenGSD 1.20.0 `query init.progress` exited 0 but returned no active phases,
`phase_count: 0`, and `current_phase: null`. `query roadmap.analyze` exited 0 but
reported phases 19 and 20 as `no_directory` and `checkbox_conflict`: the active
phase index does not resolve their archived records under
`.planning/milestones/v6.0-phases/`. The phase files, verification reports, and
the checked-complete root roadmap entries remain present. This is a tool
projection limitation; it is not incomplete milestone work. Do not copy or move
archived phase files to satisfy the active-tree query. Use the saved state and
handoff for the between-milestones route.

## Worktree disposition

Removed six clean old worktree directories after confirming each commit remained
on a local and remote branch: Phase 16, CI fixture margins, fuzz deadline,
network recovery, v5 patch release, and YM state baseline. Their branch refs were
kept. Three dirty worktrees remain untouched: OpenBIOS publication, Phase 13,
and Phase 14. The clean v5 release receipt worktree remains because its branch
has two local commits and is four commits behind its release-please tracking
branch; its unique history has not been reconciled with the root. No worktree
changes were merged or cherry-picked.

Health still reports warnings for the preserved dirty/divergent worktrees and
project-owned planning files that its generic root-file allowlist does not
recognize. These findings are not auto-repairable. No source, verification, or
acceptance record was changed to silence them.
