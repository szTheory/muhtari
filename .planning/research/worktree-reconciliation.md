# Phase worktree and planning-state reconciliation

## Finding

A PR from the current dirty Phase 14 worktree is not a safe reconciliation mechanism. Git worktrees share repository history and refs but keep per-worktree state such as `HEAD` and the index separate; a linked worktree is still a distinct checkout whose files can hold an older `.planning/` snapshot ([Git `worktree` docs](https://git-scm.com/docs/git-worktree)). GitHub PRs propose merging committed changes from a branch, and show that branch's commits and file diff; local uncommitted or untracked edits are not part of that proposal ([GitHub PR docs](https://docs.github.com/en/pull-requests/get-started/about-pull-requests)).

This repo's roadmap places Phases 12, 13, and 14 in v3.0. The Phase 14 branch tip is already an ancestor of the root checkout, so its committed branch history currently contributes no unique commits there. The Phase 14 worktree has a modified `STATE.md` and an untracked debug note; the root checkout independently has dirty `STATE.md` and `ROADMAP.md` plus Phase 14 review edits. A PR from the branch as-is would omit the dirty worktree files. Committing its old Phase 14 state would instead propose a snapshot that overlaps and differs from root's current planning edits.

## Upstream evidence

- GSD issue [#1756](https://github.com/gsd-build/get-shit-done/issues/1756) documents a worktree branch that outlived a milestone transition. Its merge replaced the new `ROADMAP.md`/`STATE.md` with old milestone content and resurrected archived phase files. It distinguishes this stale-snapshot problem from the concurrent-write bug.
- GSD issue [#1571](https://github.com/gsd-build/get-shit-done/issues/1571) and its fix [PR #1561](https://github.com/gsd-build/get-shit-done/pull/1561) document a separate lost-update problem: parallel worktree agents independently wrote shared planning files, and last-merge-wins lost progress. Locking/single-writer updates address that race; they do not make an old snapshot safe to merge after root planning has moved on.
- OpenGSD issue [#1369](https://github.com/open-gsd/gsd-core/issues/1369) and [PR #1492](https://github.com/open-gsd/gsd-core/pull/1492) show that a worktree can be forked from an earlier base after a wave merge advances `HEAD`; the fail-closed base check stopped the agents before edits. This supports checking the actual base before integrating a worktree.
- OpenGSD issue [#1827](https://github.com/open-gsd/gsd-core/issues/1827), under the STATE rebuild work, records that `STATE.md` body text can drift from the roadmap and phase directories even when a lightweight sync reports success. Treat state as a projection to reconcile from the phase evidence, not as proof that a branch snapshot is canonical.

## Recommendation for muhtari

Do not open or merge a PR from this dirty Phase 14 worktree to reconcile v3.0. First use the root checkout as the integration baseline: retain the Phase 14 dirty state/debug note as evidence, compare phase summaries and the current roadmap, then make one deliberate planning reconciliation there. Keep the current halt and acceptance status intact unless new evidence changes them. Since the Phase 14 branch's commits are already present in root, only cherry-pick or merge if a later comparison identifies specific missing source commits; do not import its `.planning/STATE.md` or other planning snapshot wholesale. A PR is useful later for review of a deliberate committed diff, not as a state-repair operation.
