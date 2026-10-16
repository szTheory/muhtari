---
title: "Lesson exchange"
summary: "Lessons imported from the owner's sibling emulators (hypotheses until reproduced here) and the template for exporting this project's lessons."
read_when: "At each phase or milestone boundary, when a sibling lesson might apply, or when the owner asks to export lessons."
updated: 2026-10-15
---

# Lesson exchange

Rules that came from these lessons live in `AGENTS.md` and `.planning/preparation/DECISIONS.md`, because agents
read those files; this ledger records where each rule came from and whether it has been reproduced here.

## Protocol

- **Inbound.** A sibling's lesson is a hypothesis until its mechanism is shown to apply here. Record the source
  project and revision, whether it was implemented or only planned there, how this project differs, and the
  local decision. Supersede entries; do not erase them.
- **Outbound.** When the owner says "export lessons", produce one pasteable block per lesson in the template
  below. Exclude paths, identities, private ROM names and hashes, credentials and transcripts. Never imply that
  another console shares a 7800 timing behaviour.
- **IDs.** `MU-<AREA>-<NNN>` for lessons learned here (areas: GSD, CI, REL, TEST, ARCH, PERF, AUDIO, HOST,
  LEGAL, DOCS); `XFER-NNN` for imports.
- **Status vocabulary.** observed → reproduced → adopted → superseded (or rejected, with the reason).

## Template

```
Lesson <ID> (<date>, <project>@<revision>)
Symptom and reproduction: …
Root cause / invariant: …
Fix and evidence (command, exit status, run URL): …
Applies when: …
Does not establish: …
Suggested check: …
Sources: …
Status: observed | reproduced | adopted | superseded
```

## Imported lessons (2026-10-15, preparation)

| ID | Lesson | Source | Local disposition |
|---|---|---|---|
| XFER-001 | Pre-write every artifact GSD would ask for and dry-run the roadmapper on a copy; the kickoff then resumes at roadmap creation. | nesturbator `8ae2c6e`, `5e681c8` | adopted (this preparation; GSD-HANDOFF) |
| XFER-002 | Phase 1 is a ROM-free walking skeleton that ships the whole release pipeline; every later phase ends in a release by merging. | nesturbator RETROSPECTIVE | adopted (v1.0 phase 1) |
| XFER-003 | Requirements written as the command that proves them make verification mechanical. | nesturbator v1 REQUIREMENTS | adopted (REQUIREMENTS) |
| XFER-004 | Verification is automation-first; the owner is asked only for consent or irreducible judgement. | nesturbator GSD-HANDOFF; gabbaboy D-069 | adopted (AGENTS rule 6) |
| XFER-005 | Implement the whole CPU against exhaustive public vectors before any game; never grow it opcode by opcode from a game trace. | glueyneo phase 05 history | adopted (v1.0 phase 2) |
| XFER-006 | Governance machinery must stay proportional; per-file rights digests, prose validators and threat registers consumed a sibling's milestone. | glueyneo METHODOLOGY, commit history | adopted (AGENTS rule 7) |
| XFER-007 | Review warnings about test code that grow into requirements stalled a milestone; keep real-host e2e to a screenshot compare and a save round trip. | nesturbator v4 phase 16 | adopted (AGENTS rule 7; HOSTS) |
| XFER-008 | A release PR opened with the default Actions token needs maintainer approval and blocks; mint a GitHub App token for every bot merge. | gabbaboy PR #41, #70 | adopted (ENGINEERING) |
| XFER-009 | Aligned machine-edited version comments broke formatting and silently blocked releases for three days; keep machine-edited lines out of aligned blocks and check the release PR at every phase close. | nesturbator `b8e2824` | adopted (no machine-edited version strings) |
| XFER-010 | Merges after a phase's verification make it stale and force override closeouts; decide the closeout policy up front. | nesturbator MILESTONES v1–v3 | adopted (GSD-HANDOFF) |
| XFER-011 | Tests that skip themselves hid a broken GUI path; RetroArch aborts inside the agent sandbox, so the hosted job is the evidence. | nesturbator phase 02/05 | adopted (no self-skip; hosted e2e) |
| XFER-012 | Timeouts guessed instead of measured cancelled green runs; use twice the slowest recent run with its run ID. | nesturbator phase 13 | adopted (ENGINEERING) |
| XFER-013 | A CTest regex with `--no-tests=error` does not prove each named test ran; keep an exact executed-test inventory and make negative controls assert the specific failure. | gabbaboy LESSONS | adopted (ENGINEERING) |
| XFER-014 | A globally ignored ROM extension silently dropped new fixtures; exempt the fixture directory and count tracked files. | gabbaboy `5f9f866` | adopted (`.gitignore` exemption) |
| XFER-015 | Agents read AGENTS.md and the plan, not the lessons file; a lesson becomes a control only as a rule plus a failing check. | gabbaboy phase 10 | adopted (this file's preamble) |
| XFER-016 | Prove playability with one integrated host session: boot, a control that changes play, moving picture, audible sound. | nesturbator, gabbaboy retrospectives | adopted (v1.0 phase 4) |
| XFER-017 | Diagnose a stalled guest at its first divergence with a bounded trace; more cycles prove nothing. | nesturbator GSD-HANDOFF | adopted (ARCHITECTURE observability) |
| XFER-018 | Confidence-qualified models: implement the best-supported model, label what is unmeasured, stop gap loops that lack new evidence. | gabbaboy D-025 | adopted (DECISIONS) |
| XFER-019 | A repository's licence is not the licence of every byte in a built ROM (SDK runtime, fonts, compiler libraries). | an unpublished Genesis sibling's phase 1 | adopted (ASSET_POLICY) |
| XFER-020 | A cited licence was wrong at the pinned commit; check the LICENSE at the exact pin. | nesturbator CONFORMANCE | adopted (PROVENANCE) |
| XFER-021 | CI that runs on every branch push and on the pull request doubles cost; push triggers only `main`. | gabbaboy ci.yml | adopted (ENGINEERING) |
| XFER-022 | A README that doubles as the CI manual grew to 114 KB and buried the install line; split user and maintainer docs with budgets. | nesturbator README history | adopted (AGENTS rule 5) |
| XFER-023 | GSD's config writer merges the owner's global defaults; write config with an empty `GSD_HOME`. | GSD 1.17.0 `config.cjs` (read this session) | adopted (preparation) |
| XFER-024 | GSD's ship step pushes a CI-skip commit to the PR branch; squash messages must come from the PR title and body. | GSD 1.17.0 `ship.md` (read this session) | adopted (`scripts/apply-repo-settings.sh`) |
| XFER-025 | Homebrew whose licence is unclear makes a poor CI fixture; self-authored test ROMs, designed so the oracle cannot be fooled (a counter that only a new input changes), were cheaper and safer — and a gate that exists but never executes is the real hazard. | playstead `edea152`, `.planning/spikes/MANIFEST.md`, STATE (phase 03.5) | adopted (CONFORMANCE 4, 7; ASSET_POLICY 2–3; ENGINEERING 3 exact inventory) |
| XFER-026 | A newly queued run cancels the pending one in its concurrency group even with `cancel-in-progress: false`; when a release merge's run is dropped or fails early, a later push's run creates the release while its own `github.sha` is newer, so CI must build release-please's `sha`. | lattice_stripe runs 30474364540, 30474374555, 30474391526; GitHub concurrency docs | adopted (ENGINEERING 4 `queue: max`; 5 step 3) |
| XFER-027 | A publish job must be safe to re-run: `GET releases/tags/{tag}` omits drafts, and a full re-run skips publish once the release PR is `autorelease: tagged`. | lattice_stripe `e7d6c0464437`; exifcleaner-electron `78b895773742`, `0266e1c7c4d0` | adopted (ENGINEERING 5 re-runs) |
| XFER-028 | A scheduled reporter that tested a piped `$?` under the default `bash -e` (no `pipefail`) stayed green while its report listed 109 actionable additions. | lattice_stripe `8fc5e4b18feb`; run 38228990990 | adopted (ENGINEERING 7: decide from job and step results; force one failure in phase 1) |
| XFER-029 | Pipeline fixes titled `fix(release):` cut ten patch releases in one day, and a title lint outside the required check let numeric scopes reach `main`. | lattice_stripe `9c488d3cc2a7`, `599b97a9d43f`; tags v1.7.3–v1.7.12; PRs #47, #69 | adopted (ENGINEERING 4 title) |
| XFER-030 | Check each release as a user would: download from `releases/latest/download/`, verify `SHA256SUMS`, and pin the attestation to the release workflow with `--signer-workflow`. | lattice_stripe `260edf43189e`; exifcleaner-node `a44704fd528d` | adopted (ENGINEERING 5 step 4) |
| XFER-031 | A live setting that no check covered (`allow_auto_merge: false`) made every Dependabot auto-merge run fail; a drift check must cover every setting it applies. | lattice_stripe live settings; runs 25654468833, 30473384398 | adopted (ENGINEERING 6; extends MU-CI-003) |
| XFER-032 | GitHub Actions updates are mostly majors (all five there), so a Dependabot group that includes majors never qualifies for patch/minor auto-merge. | lattice_stripe Dependabot PRs | adopted (ENGINEERING 7) |
| XFER-033 | An executable-only phase-completion gate enforced automation-first closeouts but grew to about 3,400 lines with its verifier and tests, and took a day of review-fix commits. | exifcleaner `30c5ff7a80ca`, `6b4ce18e31ed` | rejected: AGENTS rules 6 and 7 keep the policy without the machinery |

## Lessons learned here

Exported to the sibling projects on 2026-10-15 (preparation, `8ab05fa`). GSD findings were read in the installed
1.17.0 package; a sibling on another version re-checks them there.

| ID | Lesson | Evidence | Status |
|---|---|---|---|
| MU-GSD-001 | GSD's config writer merges `$GSD_HOME/.gsd/defaults.json` under the explicit choices, so a global yolo mode, web gates, `preferences`, `effort` and model IDs leak into a public `config.json`; write it with `GSD_HOME` pointing at an empty directory | `bin/lib/config.cjs` defaults merge; nesturbator `8ae2c6e` config carries `preferences` and `effort` | adopted (preparation) |
| MU-GSD-002 | `/gsd-ship` titles PRs "Phase N: name" and pushes a `[ci skip]` commit to the PR branch; with commit-list squash messages that token lands on `main` and suppresses push and release runs — squash title = PR title, squash body = PR body, retitle the PR as a Conventional Commit | `workflows/ship.md` (PR title, ship-note commit, BLOCKED recovery) | adopted (`scripts/apply-repo-settings.sh`) |
| MU-GSD-003 | `/gsd-verify-work` after a passing execute is redundant: execute already runs the verifier, `phase.complete` and the transition, and verify-work re-runs the transition and can start gap planning; run it only for `human_needed` | `workflows/execute-phase.md`, `verify-work.md` | adopted (GSD-HANDOFF 3) |
| MU-GSD-004 | GSD tests command arguments for the substring `--auto`, so any argument containing it chains steps; `/gsd-progress --next` and `--do` also dispatch the next command | `workflows/plan-phase.md`, `execute-phase.md`, `commands/gsd/progress.md` | adopted (AGENTS rule 8) |
| MU-GSD-005 | A marker-free `.claude/CLAUDE.md` is left untouched by GSD's instruction-file generator; one with GSD markers gets a "run /gsd-profile-user" placeholder and generated sections in a tracked, public file | `bin/lib/profile-output.cjs` | adopted (hand-written pointer) |
| MU-GSD-006 | `branching_strategy: phase` forks the branch only at execute, so discuss and plan commits land on whatever branch is checked out; branch before the first committing step and let a pre-commit hook refuse `main` once a remote exists | `workflows/execute-phase.md` handle_branching; `execute-phase/steps/protected-branch.md` | adopted (AGENTS rule 9; hygiene) |
| MU-GSD-007 | Dry-run the roadmap step before the real kickoff: copy the tree, `git init`, `query init.new-project`, run the `gsd-roadmapper` agent with the step's prompt, then `validate.consistency`, `roadmap.analyze`, `init.progress`, `list-seeds` | two green dry runs (GSD-HANDOFF 8) | adopted (preparation) |
| MU-REL-001 | release-please 17.6.0: `initial-version` applies only while the manifest says `0.0.0` (default 1.0.0); `feat` bumps minor before 1.0 only with `bump-patch-for-minor-pre-major: false`; the `release-as` key is deprecated and sticky — use a `Release-As:` footer | release-please `src/versioning-strategies/default.ts`, `strategies/base.ts` at v17.6.0 | adopted (ENGINEERING 5) |
| MU-REL-002 | Events caused by `GITHUB_TOKEN` start no workflow runs and its PRs wait for approval; Dependabot-triggered runs see only Dependabot secrets — mint a GitHub App token for every bot merge and store its key as a Dependabot secret too | GitHub docs on token triggers and Dependabot; gabbaboy PR #41 and #70 | adopted (owner task) |
| MU-REL-003 | Version-free asset names plus `releases/latest/download/` leave `version.txt` the only machine-edited version string, which removes formatter breakage and release-induced verification staleness | nesturbator `b8e2824`, v1–v3 override closeouts | adopted (ENGINEERING 5) |
| MU-CI-001 | `ubuntu-26.04` is GA (2026-09-17) and `ubuntu-latest` moves to it in November 2026 (GCC 15.2, LLVM 20–22, CMake 4.4.3); `windows-11-arm` resolves to the VS 2026 image; `macos-15-intel` is offered until August 2027 | actions/runner-images announcements #14747, #14748, #13045 | adopted (ENGINEERING 1) |
| MU-CI-002 | A `.planning`-only PR ran the whole matrix (15 jobs, 3,089 job-seconds); skip heavy jobs with a `changes` job while the aggregator still reports — never a trigger-level `paths` filter, which leaves the required check pending | nesturbator PR #47, run 38415960004 | adopted (ENGINEERING 4) |
| MU-CI-003 | Repository settings drift from their docs (nesturbator's live `sha_pinning_required` is off though its doc says on); keep settings as a script with a `--check` mode run at each tune-up | `gh api` read of live settings | adopted (`scripts/apply-repo-settings.sh --check`) |
| MU-ARCH-001 | Under PIC a file-scope `const` table of function pointers lives in `.data.rel.ro`, which `nm` reports as `d`, so a no-mutable-globals symbol check flags it; dispatch with `switch` or keep pointers in the instance | nesturbator `tests/cmake/global_symbols.cmake` behaviour | adopted (DEC.31) |
| MU-LEGAL-001 | A permissive licence today does not clear a repository's history: the MiSTer Atari7800 core went GPL-2.0 → CC BY-NC 4.0 → MIT (2026-08-30), files others edited in between are facts-only, and the repository ships Atari firmware; audit licence history, per-file editors and embedded firmware at the pin | MiSTer-devel/Atari7800_MiSTer `99ba8c7f08df`, `aef45f04b8dd`, `8c96e1f7cc24` | adopted (PROVENANCE 2, 3) |
| MU-TEST-001 | The `theft` property-testing library needs `fork`/`waitpid` (no MSVC); a ~60-line seeded splitmix64 harness covers the need on every platform | theft v0.4.5 `src/theft_run.c` | adopted (ENGINEERING 3) |
| MU-GSD-008 | GSD's secret-read guard blocks agents from reading `.env.local` and key files (it fires even on a grep), so secret setup is an owner command, planned as such | observed in this session | adopted (GSD-HANDOFF 7) |
| MU-REL-004 | GitHub's App manifest flow turns the release App into one owner command and two clicks: a local page posts the manifest (exact permissions, private, webhook off), and the conversion response (`POST /app-manifests/{code}/conversions`) carries the private key, which a short owner-run helper pipes into `gh secret set` for Actions and Dependabot with no file; the helper then mints an installation token with that key to prove it reaches only this repository. gh's OAuth token cannot list installations (`GET /user/installations` answers 403), so verify from the App's side | owner-run helper on 2026-10-15: App registered, variable and both secrets set, installation token listed only this repository and was revoked | adopted (GSD-HANDOFF 7) |
