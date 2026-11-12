---
title: "Lesson exchange"
summary: "Lessons imported from the owner's sibling emulators (hypotheses until reproduced here) and the template for exporting this project's lessons."
read_when: "At each phase or milestone boundary, when a sibling lesson might apply, or when the owner asks to export lessons."
updated: 2026-11-12
---

# Lesson exchange

Rules that came from these lessons live in `AGENTS.md` and `.planning/preparation/DECISIONS.md`, because agents
read those files; this ledger records where each rule came from and whether it has been reproduced here.

## Protocol

- **Project memory.** This ledger is the durable source for future sessions. AGENTS.md directs recall before
  phase planning or related subsystem work. Save new reproduced lessons here with evidence; retain their
  uncertainty and scope. The owner's preference (2026-10-29) is automatic project recall, with export only
  on explicit request. This does not require enabling MemPalace or copying lessons by hand.
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
| MU-GSD-027 | A generic plan checker may flag file/task counts even when project engineering policy explicitly says numerical thresholds alone do not block. Preserve coherent plans unless there is a concrete ownership, coupling, or execution failure; fix demonstrated defects such as a lossy public state path. | Phase 20 checker recheck passed after the YM state-unavailable guard was added to 20-01 and atomically replaced by full 2/7 state in 20-03; advisory count findings were retained in `20-SOURCE-AUDIT.md`. Automated plan command syntax check: `python3` one-off extractor plus `sh -n`, exit 0 (N/A-local). | reproduced (AGENTS rule 7; ENGINEERING §11) |
| MU-AUDIO-001 | Public mono PCM can appear bipolar after the DC blocker even when the raw YM DAC path has lost negative-half polarity. Assert positive and negative signed raw values on both channels and compare before mono mixing. | Phase 20-01 first matched phase/envelope/step but failed signed seed parity (`seed −3656`, C `+4384`); phase-bit-9 polarity fix and raw-DAC controls pass the receipt-bound oracle and `ym.synthesis`, N/A-local ([evidence](milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-01-SUMMARY.md#recorded-results)). | reproduced |
| MU-TEST-015 | Synthetic chip-clock tests must consume PCM through the real bounded renderer. Advancing clocks without rendering fills the private FIFO; Release builds can hide the existing capacity assertion. Keep test drivers on half-open frame-cadence render/discard and preserve production bounds. | Before `757d72b`, AppleClang combined ASan/UBSan `ctest` stopped at the existing FIFO assertion; after the test-only consumer fix, the four YM C tests, receipt-bound oracle, and same focused sanitizer pass, N/A-local ([commands and exits](milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-01-SUMMARY.md#recorded-results)). | reproduced |
| MU-CI-019 | Qualify sanitizer runtime startup and fault controls before attributing a timeout to product code; keep an alternate local compiler result separate from the pinned CI runtime. | Homebrew Clang 21.1.8 tiny C probe: plain and UBSan exit 0 (0.261s/0.541s); ASan and combined probes timed out before `main` at 5s, including the controlled outside-sandbox boundary. AppleClang 21.0.0 with the same combined address/undefined flags started (0.455s), detected deliberate heap-OOB and signed-overflow faults, and passed focused YM `ctest` 4/4 after `757d72b`, N/A-local ([evidence](milestones/v6.0-phases/20-complete-ym2151-homebrew-sound/20-01-SUMMARY.md#recorded-results)). No kernel or sandbox root cause is claimed. | reproduced (environment-specific) |
| MU-REL-001 | release-please 17.6.0: `initial-version` applies only while the manifest says `0.0.0` (default 1.0.0); `feat` bumps minor before 1.0 only with `bump-patch-for-minor-pre-major: false`; the `release-as` key is deprecated and sticky — use a `Release-As:` footer | release-please `src/versioning-strategies/default.ts`, `strategies/base.ts` at v17.6.0 | adopted (ENGINEERING 5) |
| MU-REL-002 | Events caused by `GITHUB_TOKEN` start no workflow runs and its PRs wait for approval; Dependabot-triggered runs see only Dependabot secrets — mint a GitHub App token for every bot merge and store its key as a Dependabot secret too | GitHub docs on token triggers and Dependabot; gabbaboy PR #41 and #70 | adopted (owner task) |
| MU-REL-003 | Version-free asset names plus `releases/latest/download/` leave `version.txt` the only machine-edited version string, which removes formatter breakage and release-induced verification staleness | nesturbator `b8e2824`, v1–v3 override closeouts | adopted (ENGINEERING 5) |
| MU-CI-001 | `ubuntu-26.04` is GA (2026-09-17) and `ubuntu-latest` moves to it in November 2026 (GCC 15.2, LLVM 20–22, CMake 4.4.3); `windows-11-arm` resolves to the VS 2026 image; `macos-15-intel` is offered until August 2027 | actions/runner-images announcements #14747, #14748, #13045 | adopted (ENGINEERING 1) |
| MU-CI-002 | A `.planning`-only PR ran the whole matrix (15 jobs, 3,089 job-seconds); skip heavy jobs with a `changes` job while the aggregator still reports — never a trigger-level `paths` filter, which leaves the required check pending | nesturbator PR #47, run 38415960004 | adopted (ENGINEERING 4) |
| MU-CI-003 | Repository settings drift from their docs (nesturbator's live `sha_pinning_required` is off though its doc says on); keep settings as a script with a `--check` mode run at each tune-up | `gh api` read of live settings | adopted (`scripts/apply-repo-settings.sh --check`) |
| MU-ARCH-001 | Under PIC a file-scope `const` table of function pointers lives in `.data.rel.ro`, which `nm` reports as `d`, so a no-mutable-globals symbol check flags it; dispatch with `switch` or keep pointers in the instance | nesturbator `tests/cmake/global_symbols.cmake` behaviour | adopted (DEC.31) |
| MU-LEGAL-001 | A permissive licence today does not clear a repository's history: the MiSTer Atari7800 core went GPL-2.0 → CC BY-NC 4.0 → MIT (2026-08-30), files others edited in between are facts-only, and the repository ships Atari firmware; audit licence history, per-file editors and embedded firmware at the pin | MiSTer-devel/Atari7800_MiSTer `99ba8c7f08df`, `aef45f04b8dd`, `8c96e1f7cc24` | adopted (PROVENANCE 2, 3) |
| MU-LEGAL-002 | Apply the emulator-source license boundary before browsing source files; answer protocol questions from official documentation and released binaries, and stop if a search opens prohibited emulator code | Observed during Phase 14 planning: a browser inspection opened RetroArch `config.def.h` despite AGENTS rule 1; no source was copied into the repository. `scripts/hygiene.sh --tree` exit 0; no hosted run | observed |
| MU-TEST-001 | The `theft` property-testing library needs `fork`/`waitpid` (no MSVC); a ~60-line seeded splitmix64 harness covers the need on every platform | theft v0.4.5 `src/theft_run.c` | adopted (ENGINEERING 3) |
| MU-GSD-008 | GSD's secret-read guard blocks agents from reading `.env.local` and key files (it fires even on a grep), so secret setup is an owner command, planned as such | observed in this session | adopted (GSD-HANDOFF 7) |
| MU-REL-004 | GitHub's App manifest flow turns the release App into one owner command and two clicks: a local page posts the manifest (exact permissions, private, webhook off), and the conversion response (`POST /app-manifests/{code}/conversions`) carries the private key, which a short owner-run helper pipes into `gh secret set` for Actions and Dependabot with no file; the helper then mints an installation token with that key to prove it reaches only this repository. gh's OAuth token cannot list installations (`GET /user/installations` answers 403), so verify from the App's side | owner-run helper on 2026-10-15: App registered, variable and both secrets set, installation token listed only this repository and was revoked | adopted (GSD-HANDOFF 7) |

## Reproduced workflow recovery lessons (2026-10-24)

The owner requested separate purpose review and adoption of coherent recommendations.
[Decision and evidence](research/workflow-recovery/RECOMMENDATION.md),
[fan-out prompt](research/workflow-recovery/prompt.txt) and GSD-HANDOFF §3.1 carry
the operative rules. A ledger alone is not a control.

| ID | Symptom and invariant | Disposition and control |
|---|---|---|
| MU-GSD-009 | One known archive failure hid a required RetroArch linker failure. Native success is not a complete hosted outcome audit. | Reproduced run40132929297; full-job/source audit and changed standalone-consumer preflight, GSD-HANDOFF §3.1. Reproduced again in run 41056151292 attempt 1/source bf21fc3: moving the BMP reader without updating its diagnostic target skipped RetroArch; missing palette linkage surfaced in local correction. Exact-SHA check_ci_run audit exits 1; corrected comparator build and complete local 673+6 workflow exit 0, N/A-local. The halted 12-01 summary preserves the failed run; local success is not hosted input evidence. |
| MU-GSD-010 | Advisory summary scanning can mistake a command for a missing file or a failed gate for a failed summary; summary presence and prose do not prove accepted outcomes. | Reproduced verify-summary03 and halted graph. On 2026-11-06, `gsd_run verify-summary` treated the first `## Verification` heading as the self-check and misread the recorded workflow failure as a failed summary (`passed:false`, exit 0); renaming that section to `## Recorded Results` made the actual `## Self-Check: PASSED` validate (`passed:true`, exit 0). Reproduced again during Plan 12-15 closeout: the validator scans the entire self-check tail, so a factual bullet containing `failed` or `blocked` makes it report a failed self-check. Keep execution outcomes in `Recorded Results` and make the self-check positive-only. Its default file check examined zero paths here, so confirm declared files and commits directly as well. |
| MU-GSD-011 | Repeated unchanged polling, metadata reconciliation and general replanning did not change acceptance. | Observed execution/state mismatch; bounded relevant fan-out, one adversarial synthesis, explicit stop and single state writer |
| MU-GSD-015 | The one-major-step-per-session rule was applied to every debug continuation, fragmenting diagnosis, scratch controls, preflight, replay and cleanup into repeated commands and authorizations. A sample cap is a per-supervisor-attempt bound, not a tool quota. | Reproduced in [phase12 debug record](debug/phase12-packaged-input-tracer.md): preflight exit1 before invocation did not spend a sample; the later v1 supervisor attempt failed on an empty CID and was spent; v2's seven offline controls and one separately authorized sample completed without resolving the historical hosted/RetroArch cause. Stop after major OpenGSD transitions (discuss, plan, execute, phase/milestone management, audit, ship/archive), not each `gsd-debug continue`. Bundle the agent-owned preflight, scratch edits, controls, bounded review, reservation and owned cleanup within one finite scope. Ask again only at a new live/remote/irreversible boundary, a failed gate requiring changed scope, or an attempt beyond the stated cap; preserve the halt without acknowledgment. |
| MU-GSD-016 | GitHub Actions job-log responses can contain terminal escapes; a default `gh api .../logs` read may fail even when the log is available. | Reproduced: the first read of `repos/szTheory/muhtari/actions/jobs/124605180515/logs` exited1 with empty stdout and a terminal-escape refusal; one authorized `gh api --allow-escape-sequences ...` redirected to a private file exited0, and local filtering extracted test timings without printing raw escapes. [Run41224305264/job124605180515](https://github.com/szTheory/muhtari/actions/runs/41224305264/job/124605180515); [diagnostic report](debug/phase12-publication-diagnostic-proposal.md). Use the explicit option only with file redirection, inspect locally, and stop rather than repeating failed API calls. |
| MU-GSD-017 | A checkpoint focus or active-sounding STATE handoff can lag behind newer closeouts; repeating `continue` then replays completed work without producing evidence and hides the real blocker. | Reproduced 2026-11-01: `sed -n '1,14p' .planning/debug/phase12-packaged-input-tracer.md` and `sed -n '2947,2972p' .planning/debug/phase12-packaged-input-tracer.md` showed an older checkpoint focus beside later closeouts; a later read of STATE lines 106 and 150 found stale active instructions for a superseded supervisor failure and already-completed capture work. The repeated resume-manager invocations added no diagnostic evidence. Before each debug continuation, reconcile Current Focus and active STATE handoffs with the newest dated checkpoint closeout, phase summary and current project boundary. Require the next operation to have authorized scope and known inputs; mark superseded handoffs historical. If the blocker is unchanged, hand off its missing input, consequence, next action and command, then resume only on new evidence or authority. Adopted in AGENTS rule 8. |
| MU-CI-004 | A30s Apple control deadline repeatedly killed otherwise passing work; timeout samples are censored. | Successful22.79s maximum in run40135571745/job121002299364; existing twice-observed-max gives provisional46s Apple-only, same cases/oracles |
| MU-CI-005 | A~28s frontend waited behind~25min Windows publication controls across current/base. New emulator code was not the bottleneck. | Reproduced run40135571745; seed measured publication-control profiling before DAG redesign, preserve Windows failure coverage |
| MU-ARCH-001 | Historical compressed minimum399937 blocked correct growth by1750bytes without a demonstrated capacity objective. | Reproduced9a64bbc/run40135571745; DEC.61 retires only cutoff, preserves required authenticated measurements/integrity, no replacement quota |
| MU-TEST-001 | Same-head repeatability was initially overclaimed as performance comparison. | Corrected08-05 evidence; preserve armed PR+1% guard, label actual workload/base, stable PR-base acceptance |
| MU-TEST-003 | Whole-frame equality cannot detect equally wrong operands; an ORed fire pin does not distinguish button 1 from button 2. | Reproduced with owned Up/button-2 frames: old `compare --input-controls held` accepts both identical operands (exit 0), corrected judge rejects INPT0 (exit 1), N/A-local. Shared `tests/input/pixels.c` independently checks all eight bands; existing `input.proline_one_button` exercises held/release, wrong port/button, missing/stuck input and pixel/dimension faults in CI. |
| MU-HOST-003 | A fail-fast input check retained its screenshot but printed only other launches' logs, leaving driver selection and schedule consumption undiagnosed. | Reproduced [run41062201311/job124101716926](https://github.com/szTheory/muhtari/actions/runs/41062201311/job/124101716926): held comparator exits1, idle predicate on the same image exits0. The input log/config is absent from uploaded artifacts; DMA's driver message is not input-launch evidence. Capture the actual failed launch in the next scoped correction before another host test; do not infer a driver root cause. |
| MU-HOST-004 | An upstream test driver's existence does not establish its inclusion in a released app. Configuring an absent optional driver cannot supply input. | Exact pinned DMG digest81b79121 and executable digested90b544 match installed RetroArch1.22.2/git69a4f0ea. `strings`/`nm` on the released executable find ordinary driver config/MFi/HID but no test-input keys/test joypad; exit0,N/A-local. PR16370 metadata documents the optional JSON driver, not packaged availability. No host rerun; missing input remains blocked. Existing failure step now uses locally tested `tests/retroarch/report.cmake` to retain each launch's config/log and schedule. |
| MU-HOST-005 | Event-posting access belongs to the helper's actual execution identity. A local denial does not establish the hosted result, and permission does not prove delivery. | Native helper compile exits0/local preflight77; intended macos-15-arm64 image20261028.0489.1 reports permitted in [run41075600234/job124150451124](https://github.com/szTheory/muhtari/actions/runs/41075600234/job/124150451124). Permission-only step then intentionally exits1 before any RetroArch/content. Full CMake run audit1 exposes only that stop plus required aggregate; other jobs pass. Preserve failed gates and preflight under the intended identity without prompts/grants. |
| MU-TEST-004 | A one-row held movie is not a held state at a later observation: the runner releases buttons at movie EOF. | Actual600-frame runner replay with one held row makes the independent held judge reject SWCHA1; the workflow's600-row producer passes0, N/A-local. `policy.retroarch.preflight_gate` executes that actual producer, asserts600 held/released rows and rejects a one-row mutation. Scoped review caught the caller mismatch before the single hosted content probe; movie EOF behavior remains unchanged. |
| MU-TEST-005 | A failedexec mock must return an error so the real child's terminating fallback stays reachable. Test both the mock contract and the actual child ownership boundary. | [Run41086569269](https://github.com/szTheory/muhtari/actions/runs/41086569269), sourcea7aa889 attempt1: both Windows compilers reject unreachable `_exit(127)` after an always-aborting mock; actual RetroArch skips. Returning-1 plus a direct regression rejects the old mock bySIGABRT and passes locally. `policy.retroarch.child_failure` requires forwarded argv, exit127 and no input/wait/kill; wrong-argv, child-fallthrough and child-input mutants all fail the external expected-code/marker consumer. Native clang-cl compatibility passes; only a real MSVC run can establish its verdict. |
| MU-TEST-006 | Fault generation and validation must have the same platform applicability. A Mac-only full run and review did not exercise a portable check's non-Unix branch. | [Run41090838885/job124184107734](https://github.com/szTheory/muhtari/actions/runs/41090838885/job/124184107734): real MSVC compile/API/child-exec tests pass, but the shared policy check fails because its short-movie mutant is generated everywhere while execution-based validation is Unix-only. Forcing the original checker's non-Unix branch reproduces1; local common validation of actual producer start/stop/stride/destination, runner window and mode bindings passes0. A shifted equivalent600-row span passes; truncation/binding/window/stride faults reject1, with Unix execution retained. Full local677+6 and exact inventories pass; this repair has no new hosted verdict. |
| MU-TEST-007 | A platform clock identifier need not be an integer. A replacement mock that narrows the real clock API can compile on native platforms and fail under WASI. | [Run41150939552/job124397648745](https://github.com/szTheory/muhtari/actions/runs/41150939552/job/124397648745): `keyboard_clock(int, ...)` conflicts with pointer-valued WASI CLOCK_MONOTONIC, producing three compile errors before tests while all six native current/base suites pass. Complete direct-log/outcome audit exits0; required checker exits1 solely on independent host failure. WASM stays advisory; no fix or successful WASI test is claimed. Read the target header and preserve its clock identifier type before proposing a repair. |
| MU-CI-012 | A slow-lane multiplier does not establish twice-complete-work margin: the Intel PAL BIOS test allowed8s although unchanged work had completed in5.66s. A later8.72s timeout cannot measure completion. | Seven historical complete Intel current/base observations reach5.66s in [run41062201311/job124084546980](https://github.com/szTheory/muhtari/actions/runs/41062201311/job/124084546980). The owning registration now uses base6s, giving12s with unchanged Intel scale2; generated properties, unchanged predicate/fixture and full local workflow/inventories pass0,N/A-local. This restores measured margin, not proof of the failed execution's cause or native acceptance. A second CTest timeout is also censored: the Windows ARM64 OpenBIOS test uses base TIMEOUT 1 from 2x the historical 0.40s maximum across 16 runs; run41255515829 reported 1.53s at timeout, while one matching successful lane job in run41224305264 passed two observations in 0.15s and 0.06s. Keep the original evidence labeled: these samples do not establish a new upper bound or justify a global timeout increase. |
| MU-CI-013 | A pooled native job limit can lose the required margin for one lane even when per-test scaling is intact. Calibrate from complete successful jobs; completed tests inside a cancelled producer are insufficient. | [Intel budget review](debug/phase12-intel-budget-review.md):40 successful complete Intel jobs reach1085s in [run40783431377/job123150624076](https://github.com/szTheory/muhtari/actions/runs/40783431377/job/123150624076), so existing DEC.47 gives37m rather than19m. Owner-approved local Intel-only correction preserves every test/scale/join. Actual-expression pass/fault evaluation, existing timeout/policy consumers, full local workflow and exact inventories exit0,N/A-local; one bounded review PASS. Historical workload differs from current677+6, so expanded-workload margin remains provisional; no hosted success, slowdown cause or keyboard delivery is established. |
| MU-CI-014 | A stage marker written before `execute_process` proves the wrapper reached the call site, not that the child launched. If stdout is redirected to an unuploaded file and stderr is held by CMake until return, a CTest timeout leaves no child progress or status to diagnose. Treat elapsed timeout observations as censored and retain the command and both output streams in the failure artifact. | Reproduced on the same source in [run 41329055050](https://github.com/szTheory/muhtari/actions/runs/41329055050): Windows ARM64 `built.openbios` timed out at 1.74s, the stage trace ended at `assemble`, and `LastTest.log` had empty output; the authenticated ZIP contained no `assemble.log`. [The prior same-source run](https://github.com/szTheory/muhtari/actions/runs/41323089043) passed in 0.14s, establishing different outcomes but no cause. The failed recipe held stderr until return. Separately authorized local capture now records prepared argv and both streams on disk and lists them in that existing artifact step. Existing self-test checks successful stdout and a rejected real missing-module child with persistent stderr; scratch baseline control1/repaired0, focused/adjacent CTest0, both firmware digests and TIMEOUT1/1 preserved, N/A-local. [Local closeout](phases/12-player-ready-delivery-and-playstead-bridge/12-07-SUMMARY.md#local-openbios-output-capture-2026-11-01) retains exact commands/exits. Prepared command or empty streams still do not prove launch; Windows timeout/input causes unknown, no hosted allowance renewed. |
| MU-CI-015 | A workflow input that narrows one job does not narrow the workflow's matrix. The existing manual CI event cannot validate Windows ARM64 OpenBIOS alone without also running the full matrix and either launching RetroArch input or intentionally failing its preflight path. | Reproduced by source review of `.github/workflows/ci.yml`: non-PR dispatch sets `heavy=true`, `build` expands six platforms, and `ci-required` expects the full job set. `keyboard_preflight_only=true` exits 1 before content; `keyboard_probe_only=true` launches the actual keyboard child. The full dispatch ceiling is 329 configured runner-minutes. Do not spend it for the already locally validated artifact-path change or add a duplicate workflow solely for this observation. Defer hosted validation until a separately authorized source publication/full-CI scope has independent value. [Read-only review closeout](phases/12-player-ready-delivery-and-playstead-bridge/12-07-SUMMARY.md#post-capture-hosted-scope-review-2026-11-01); run URL N/A-local. |

Follow-up [run41133992538](https://github.com/szTheory/muhtari/actions/runs/41133992538)
supplies exact677+6 complete Intel current/base production/uploads in1107s,
retaining37m under DEC.47. Complete Windows-arm64 work takes2207s, so its68m
deadline lacks twice-this-sample margin (74m arithmetic); no deadline change is
authorized. Complete job/source audit exits0; required audit remains1 on host
failure. This new sample does not establish the cause of earlier slowdown.

| ID | Symptom and invariant | Disposition and control |
|---|---|---|
| MU-HOST-006 | Permitted posting and a helper's key schedule do not prove host consumption or held capture. A terminal released frame cannot substitute for held evidence. | [Run41133992538/job124346024270](https://github.com/szTheory/muhtari/actions/runs/41133992538/job/124346024270): one child logs Up/Z down/up and600frames; artifact contains only released PNG. Independent released predicate0, missing F8 held capture rejects before comparisons. Existing fail-closed consumer is retained; missing-capture cause remains unproved. Complete audit0/required audit1; sole process spent, no blind retry. |
| MU-HOST-007 | A fixed diagnostic record budget can be exhausted by repeated input-post logs before it captures the poll/state evidence needed to explain a failed host observation. Classify the child at the deadline before reporting an acknowledgment timeout. | [Run41461948345](https://github.com/szTheory/muhtari/actions/runs/41461948345): `python3 /private/tmp/muhtari-trace-observation-interpret-20261102.py` exits 0; the 64-record helper trace contains INIT/FORK/START, 60 repetitive POST records and SATURATED by 2.934s, while posts continue through 4.84s and the no-ack branch fires at 5.014s. `git show 8e5acda806a1d7c3551bb0ca8136254648151757:tests/retroarch/keyboard_preflight.c` and matching `tests/libretro/input_trace.h` confirm up to three POST writes per 100ms tick and a 64-record ceiling. On the cutoff iteration, the helper exits before its next `waitpid`; child status at the deadline is therefore unknown. Next local diagnostic repair should aggregate repeated posts, preserve space for lifecycle/poll/state/end evidence, and observe child completion at the cutoff with deterministic pass/fault controls. This reproduces an observability defect; the actual input-failure cause remains unknown. An unfinished INIT-only core prefix does not prove no polling, no input, or a stall. |
| MU-HOST-008 | A poll error is not proof that a network response was absent; only a completed no-reply observation can pass a containment check. | Reproduced with offline statuses in Plan 14-14: `poll_result=-1` passed the old decision, while the focused control fails before the fix and passes after accepting only `poll_result=0`. No sandbox, network request, or frontend process ran; N/A-local. The historical host preflight cause remains unknown. |

The separately approved acknowledgment experiment in
[run41150939552/job124397648612](https://github.com/szTheory/muhtari/actions/runs/41150939552/job/124397648612)
also posts repeatedly without capture: one child,35F8-down observations through
4925ms/capture0, generic helper failure around5.2s, no release/images/pixel judge.
The no-ack guard is consistent with timing; the exact failure branch and frontend
startup/routing cause remain unproved. Audio initialization errors are evidence,
not established causality. Complete job/step/log/artifact audit exits0; required
audit remains1, no frames artifact exists and the one-process scope is spent.
Investigate startup/capture readiness read-only before another source/live scope;
do not widen the window or infer event consumption from repeated posting.

The subsequent read-only continuation identifies a diagnostic limit: the helper
tests the5s no-ack guard before child status, and several unrelated exits share
one message. Last4925ms posting plus generic failure supports cutoff attribution
but cannot identify the executed branch or establish frontend readiness. Preserve
that uncertainty; report a named stopping condition before changing schedules.
The [diagnosis](debug/phase12-held-capture-diagnosis.md#read-only-startup-and-wasi-diagnosis-after-the-acknowledgment-experiment)
records unchanged complete receipts/audit0 and required audit1; no source repair
or new hosted execution is implied.

The separately approved instrumented
[run41158640754/job124418953455](https://github.com/szTheory/muhtari/actions/runs/41158640754/job/124418953455)
now confirms its own immediate stop: acknowledgment-deadline at5090ms with
capture0/errno0/child_status unknown. One child posts35F8downs through4988ms,
then no release/image is produced. Guard-before-wait preserves an unknown child
state; cleanup reaping cannot recover it. Named diagnostics establish a branch,
not readiness, routing, audio cause or the old execution's branch. Full outcome
audit0/required audit1; no frames artifact and allowance spent. Preserve this
distinction before proposing further readiness/routing instrumentation; another
no-ack outcome alone authorizes no retry or wider window.

| ID | Symptom and invariant | Disposition and control |
|---|---|---|
| MU-TEST-008 | A deterministic mock of a platform API must preserve its argument type. WASI's clock identifier is a pointer; native integer-like identifiers do not establish portability. | Latest [WASI job124397648745](https://github.com/szTheory/muhtari/actions/runs/41150939552/job/124397648745) fails three diagnostics before tests. `python3 /tmp/muhtari-wasi-clock.SSDai9/check.py` exits0,N/A-local: complete original mock compile1 reproduces all three; a private conditional int/clockid_t alias compiles0 for WASI/native/no-clock fallback, while signature-only clockid_t fails fallback1. Source repair awaits separate authority; no runtime or hosted acceptance is inferred. |
| MU-TEST-009 | Exit-code transport is part of a portable negative-control contract. A native127 assertion can fail before it reaches the observer when the selected runtime rejects that status. | [WASI job124418953566](https://github.com/szTheory/muhtari/actions/runs/41158640754/job/124418953566) compiles the repaired clock mock and passes its ordinary valid/fault test;605/606tests pass. Child-exit control prints its required argv/no-input/no-wait marker, then the runtime explicitly rejects127 as outside[0..126) and returns1. `python3 /tmp/muhtari-diagnostic-exec.Hak1cS/audit_outcome.py` exits0; required audit1 solely host. Runtime remains advisory/unaccepted, native127 contract unchanged; no new repair or pin/semantic change is authorized. |
| MU-TEST-010 | A WASI host-file observer cannot depend on `tmpfile()`; bind a fresh exclusive fixture to the test's existing build preopen and test both valid observation and content/cursor faults. | Plan 12-20: native and pinned Wasmtime battery tracers pass once, preserving the marker and cursor and rejecting wrong-cursor/changed-marker controls; the file is absent after each run. Full `cmake --workflow --preset wasm32` exits8 and the exact portable JUnit audit exits1 on three unrelated cases (direct execution of a WASI driver without its emulator and two existing timeout limits). The local fixture is portable, but the full WASI gate and hosted/Playstead acceptance remain unproved. |
| MU-TEST-013 | A generated CMake script that directly executes a cross-compiled test driver must carry `CMAKE_CROSSCOMPILING_EMULATOR` as a CMake list. Omitting it makes `execute_process` attempt to launch the target binary as a native executable before its controls run. | Plan 12-21: the preserved `retroarch.states.controls` script lacked the emulator on its fault controls and the WASI lane reported `exec format error`; the corrected native and WASI CTests pass with no skipped controls. The same omission recurred in `libretro.observe.ym`: [Release equality job 131218399043](https://github.com/szTheory/muhtari/actions/runs/43228680727/job/131218399043) shows the shell parsing the PowerPC ELF instead of running it through qemu. Pass the emulator list into the wrapper and expand it as command arguments. See [Plan 12-21 local evidence](phases/12-player-ready-delivery-and-playstead-bridge/12-21-LOCAL-EVIDENCE.md). |
| MU-TEST-014 | An offline API mock must validate the compiled binary before execution; source edits do not protect a stale executable when the build helper reuses an existing output. | Plan 12-26: the mock receipt records one real `CGRequestPostEventAccess` call (`denied`, exit 77); `nm -u` confirms `_CGRequestPostEventAccess` remained in the unbundled mock. The source aliases were moved around the request path afterward, but `build_controls()` skipped rebuilding because the output already existed. The prepared app was not launched. Recovery compiled to a fresh path and rejected all unresolved `CG*`, `AX*`, and `CFRelease` symbols before execution; `python3 build/phase12-gap26-evidence/launcher.py recheck-offline-controls` exited 0 with all 11 mock controls passing and zero additional real calls. See [Plan 12-26 launcher evidence](phases/12-player-ready-delivery-and-playstead-bridge/12-26-LAUNCHER-EVIDENCE.md). |

MU-TEST-009 local follow-up: the separately approved mock-only WASI transport125
preserves the real helper127 and native127 assertions. `python3
/tmp/muhtari-wasi-transport.atnGLI/checks.py` exits0,N/A-local: actual native,
fallback and native-executed WASI branch pass; wrong exit/argv/child activity,
omitted termination, wrong transport and missing marker reject1. Independent
fixtures in pinned Wasmtime48.0.6 reproduce127-plus-marker runtime rejection,
accept125-plus-marker, and reject125 without marker through the unchanged
observer. Actual WASI source compiles0, but complete C-mock target execution
still lacks a linker; no full WASI or hosted input acceptance is claimed.
Final full local workflow and exact ordinary/release inventories pass0 after
an initial docs-budget failure was corrected in prose, without changing its
limit. Source stays uncommitted and all live allowances stay spent. Earlier
awaiting-local-repair wording is historical; hosted advisory failure remains.

MU-TEST-008 follow-up: approved three-file sourcecee6dea in the same new run
repairs the actual target compile failure and passes `policy.retroarch.preflight_api`.
Its earlier awaiting-authority text is historical; MU-TEST-009 and the full
advisory failure remain. Native/fallback fault tests and six native current/base
productions also pass; compilation success alone still does not establish full
WASI runtime acceptance.

MU-TEST-009 tooling follow-up: compile-only target evidence can hide more than
one absent link input. `python3 /tmp/muhtari-wasi-tool-proposal.5zh8vi74/inspect.py`
exits0,N/A-local: the installed Clang21 driver-only plan selects absent
`wasm-ld` and WASI `libclang_rt.builtins.a`; no binary is generated or executed.
A linker-only fetch would not supply that runtime archive. The
[read-only proposal](debug/phase12-wasi-local-proposal.md) selects one coherent
private SDK rather than serial tooling fixes, with explicit local/CI sysroot
differences and a new download/execution decision. This is a proposed route,
not reproduced SDK compatibility or full runtime/host acceptance. No package
was fetched and every existing stop/allowance remains intact.

MU-TEST-009 actual-target follow-up: the separately approved private SDK batch
now links and executes the complete actual C mock as WASI. `python3
/tmp/muhtari-wasi-exec.ludxi_mr/execute.py continue-after-metadata` exits0,
N/A-local: direct ordinary0/child125 plus exact marker, both actual owning
registrations pass, all ten linked target counterfactual observers reject1.
The frozen pre-repair127 mock reproduces pinned-runtime rejection despite
the marker. [Outcome and receipts](debug/phase12-wasi-local-proposal.md#approved-execution-outcome-2026-10-31)
close focused local complete-C execution only: SDK libc differs from hosted
CI, whose full WASI result stays advisory/unaccepted; hosted input stays failed.

The same batch reproduced a tooling-metadata pitfall: a release-note summary
described LLVM21.1.0, but the exact approved SDK28 artifact's compiler/linker
report21.1.4 at its exact recorded LLVM submodule revision. Initial driver
exits1 before any target build; documented prebuild continuation reuses saved
versions and original deadline after correcting the descriptive assertion.
Asset/digest/SDK/compiler-major/semantic conditions remain unchanged, no
target attempt repeats, bounded reviewPASS. Identify actual verified tools by
their reported version and pinned component revision; a summary is not exact
binary identity. Unexpected asset/digest/revision still stops the batch.

Approved local follow-up, 2026-10-30: the actual mock now uses the conditional
private clock identifier type. `python3 /tmp/muhtari-clock-diagnostics.HhNyhn/check.py`
exits0 for changed-source WASI compile-only, native/fallback valid/fault execution
and standalone links. The helper reports named stopping conditions, last valid
observations and saved API errors; cleanup reaping does not turn an unobserved
child state into evidence. Tests preserve guard precedence and reject fabricated
child-status claims. Full local CI and exact ordinary/release inventories exit0;
one bounded review has no blocking findings. N/A-local; hosted capture/readiness
and complete WASI runtime acceptance remain unproved. This supersedes only the
pending local-repair disposition above, not historical failures or live limits.

Local continuation extends the existing helper/wrapper consumers with bounded
capture acknowledgment and monotonic timing. The actual original helper and
single-pulse/no-release/no-F8-up counterfactuals compile0/reject1; corrected
controls and full local workflow/inventories exit0,N/A-local. This reproduces
the consumer distinction, not the original hosted cause. The existing support
cap separately rejects347/300lines and remains binding despite localCI success.
The [local report](debug/phase12-held-capture-diagnosis.md) records commands,
the one bounded review and the still-pending contract decision. Its complete
Windows review also finds2324s among40 successful arm64 jobs, so DEC.47 supports
78m rather than latest-only74m; historical workload differences remain explicit.

The owner subsequently approved retiring only that numeric support ceiling,
without a replacement quota (DEC.56). Its historical347/300 rejection remains
a failure. The current workflow reports actual line totals; the existing policy
consumer accepts reporting301lines, rejects missing inputs and detects restored
numeric gating. Two cases, owned-child/process bounds, independent judges and
ordinary architecture review remain binding. The cap amendment grants no live
allowance and does not establish the original hosted missing-capture cause.

The local-only verdicts above retain their original checkpoint scope. In
[renewed run41099366366](https://github.com/szTheory/muhtari/actions/runs/41099366366),
both native Windows current/base workflows pass the portable preflight/API/child
controls; Intel's current/base PAL BIOS predicates complete in4.85s/4.34s.
The Intel job is nevertheless cancelled by its outer19m deadline, concurrent
with a reported Actions incident. Passing executor logs do not supersede a
failed required aggregate; the complete20-job audit still exits1 and host skips.
The incident's causal role remains a hypothesis, not a reproduced core defect.

The separately authorized single targeted Intel retry in
[attempt2](https://github.com/szTheory/muhtari/actions/runs/41099366366/attempts/2)
cancels again after official Actions recovery. Direct Intel log/annotations and
the complete20-job/artifact audit exit0; `check_ci_run.cmake` exits1. Current and
base677+6 pass (ordinary suites533.41s/545.41s), but the19m outer limit cancels
remaining quality production and all uploads skip. All20 original artifacts are
retained unchanged; the passing quality join consumes prior same-source evidence,
not a fresh producer. The broader job remains incomplete despite passing tests;
its censored duration cannot select a new deadline. Review complete comparable
Intel producer work under DEC.47 before another scope request; service recovery
does not establish that an unchanged retry will fit. No new keyboard child/image
exists, no cause of underlying slowdown is proved, and the allowance stays spent.

Control implementation acceptance is recorded with commands/exits and the new
exact-source run in the recovery decision. Historical size/link/timeout failures
remain failures; retirement does not retroactively make them pass.

Recovery acceptance: commit62fa325, https://github.com/szTheory/muhtari/actions/runs/40139744520, attempt1. Required CI, exact-source
outcome audit and actual8-lane join pass (exit0). Overall workflow remains failed
from explicitly advisory library timing; no retry or proven-flake claim.
Only plan05 closes to5/14, next08-06 ready. Prior threshold failures remain historical.

## Reproduced controller execution lessons (2026-10-25)

The existing fan-out prompt carries the process changes; executable regression and
existing deadline/inventory mechanisms carry the controls. No new dependency or
orchestration framework was introduced.

| ID | Symptom and invariant | Fix and evidence | Status |
|---|---|---|---|
| MU-HOST-001 | An enclosing structure's end was used to decide whether an earlier required field pair was complete; absent optional flags changed the meaning of present coordinates. | ac76249 derives completeness from the required field end;24 prefix/explicit-default/negative cases fail before the fix and pass after it. Independent sensor harness and49/49 verifier pass; CI40157749294 succeeds. | adopted |
| MU-CI-006 | Dispatch before push completion ran an older remote head; job cancellation also hid earlier failed test steps. | Await push, check advertised remote SHA, then verify actual run head/attempt. Audit failed steps before cancellation as well as terminal job outcomes. Corrected70366fd runs were pinned before dispatch; CI audit exits0 at https://github.com/szTheory/muhtari/actions/runs/40157749294. Earlier wrong-head and failed runs remain in14-SUMMARY. | adopted |
| MU-CI-007 | Expanded host/gun/fuzz cases retained deadlines derived from smaller workloads, wasting hosted runs. Timed-out samples do not measure complete work. | Re-measure changed workloads before dispatch under DEC.47; owning test registrations retain all cases and lane scaling. Host5s, fuzz2s and gun2s derive from complete successful samples recorded in14-SUMMARY/REVIEW-FIX. Final required CI passes; advisory10000 isolation remains explicitly unaccepted. | adopted |
| MU-TEST-002 | Scoped CTest overwrote the preset's full JUnit file; the exact-inventory consumer correctly rejected the partial report. | Use a distinct --output-junit path for focused evidence and preserve the full-inventory artifact. The verifier's single full cmake workflow exits0 with649+6 and restores complete evidence; N/A-local,08-VERIFICATION local checks. | reproduced |
| MU-CI-008 | A doubled lightgun matrix kept an older2s deadline; valid full-suite work measured2.1942s. Independently, a60s wrapper could kill allowed130s publication work; preset jobs also overrode CTEST_PARALLEL_LEVEL. | a93a11d sets provisional5s with every assertion/scale intact; ddf51d4 config0 removes the redundant GSD outer timer for the one-shot CMake workflow, retaining all659 positive CTest deadlines and wrapper exit propagation. Maintainer guidance requires explicit worker measurement and separate focused reports. Current653+6, unchanged base652+6, trusted producer and six actual ZIP CRCs pass exit0, N/A-local;09-07-SUMMARY/resolved debug session retain failed history. Hosted calibration remains09-08. | adopted |

## Reproduced PAL execution lessons (2026-10-25)

These observations are reproduced here. Existing loader tests, measured deadlines,
standard jobserver facilities and the bounded recovery protocol carry the controls;
no new orchestration framework or quota is introduced.

| ID | Symptom and invariant | Fix and evidence | Status |
|---|---|---|---|
| MU-ARCH-002 | Borrowed BIOS input may point into public cartridge RAM, battery storage or HSC ROM that load installation frees or clears. Validate/stage before invalidating input; commit live firmware only after the last fallible operation. | af9dc83 adds bounded unpublished instance staging; `bios.load_aliases` deterministically poisons released RAM and covers ten source/size/region combinations, failed-load preservation and real boot. RED exit1, GREEN exit0; CI40233948646 and native/PPC654+6 pass. The review-fix report records the16KiB cost and single-allocation contract. | adopted |
| MU-CI-009 | Preset `jobs: ""` overrides `CTEST_PARALLEL_LEVEL`; a pipe jobserver lost its file descriptors through `cmake --workflow`. An environment assignment alone did not prove bounded workers. | A representative8-test fixture measured pipe peak8 and FIFO peak4 with three explicit tokens plus CTest's implicit slot. The same temporary standard FIFO context ran the unchanged full workflow654+6, package/JUnit/hygiene exit0; N/A-local,09-08-SUMMARY. Presets, assertions and deadlines remain intact. | reproduced |
| MU-CI-010 | A package-provisioning delay consumed29m13s of a30m hosted job; the outer deadline then preempted partial native validation before PPC. Partial output did not establish a complete inventory or a source defect. | Preserve the complete attempt/job/step/annotation evidence before a finite targeted retry under GSD-HANDOFF3.1. Nightly40233948516 attempt1 is unaccepted; one same-source/deadline job retry used13s setup and passed full native/PPC654+6 plus956-row equality in attempt2. Advisory properties were not rerun or promoted. | reproduced |


## Shipping timing evidence (2026-10-25)

| ID | Lesson | Evidence | Status |
|---|---|---|---|
| MU-CI-011 | Identical Windows image/toolchain and concurrency do not imply comparable host CPU class. Two attempts reproduced broad slowdown and a36s PAL timeout with unchanged runtime; another blind retry did not resolve it. | CI40238824962 attempts1/2: accepted AMD family26/model2 versus failed family25/model1; CPU causality remains unproved. Existing DEC.47 applied complete ASan52.64/scale2 and Windows hash28.68 to base53/58; owning registrations retain all workloads/scales. Canonical09-08-SUMMARY preserves failures, commands and acceptance scope. | reproduced |


## Release citation ownership (2026-10-25)

| ID | Lesson | Evidence | Status |
|---|---|---|---|
| MU-CI-012 | A future-product-version scan misclassified a pinned upstream oracle release in PROVENANCE.md only after the release bump. Scope version ownership explicitly; do not disguise the citation. Positive fixtures must match the scanner's real token boundaries. | ReleaseCI40245055377 and local VERSION0.8.0/NEXT reproduce exit1 on test7800 v0.9.0. Exact-root provenance exemption retains source/docs scanning and notes.md fault detection; corrected parenthesized current/minor/patch fixtures fail the old checker and pass the new one. Full local656+6 exits0; source repair still requires hosted release CI. | reproduced |


## Reproduced milestone audit lessons (2026-10-29)

These lessons supplement the existing workflow controls; they introduce no new
gate, dependency or repeated phase-verification requirement. Source: audit commit
`3f0117f`, `.planning/v2.0-MILESTONE-AUDIT.md`.

| ID | Symptom and invariant | Fix and evidence | Status |
|---|---|---|---|
| MU-GSD-012 | Six summary frontmatters looked readable but the actual GSD parser rejected seven unquoted command references containing colon-space. Text search alone did not establish usable workflow metadata. | Quote the seven scalar values without changing their decoded contents. `node <GSD_TOOLS> query summary-extract <summary> --fields requirements_completed` then succeeds for all82 summaries; value-preservation and cross-reference assertions exit0. Run URL N/A-local; audit records the six files and exact consumer. | reproduced |
| MU-GSD-013 | Five requirements were verified and checked complete but absent from every summary completion field because executors deferred final acceptance to the parent. Separately, summarized slots included halted plans. | Reconcile the final summary against independent verification and current integration, retaining historical pending/halt receipts. Three-source assertions exit0 for27/27 IDs with0 orphaned; accepted active plans remain75, distinct from76 summarized active slots and82 total summaries. Existing `check_ci_run.cmake` exits0 for merged source in https://github.com/szTheory/muhtari/actions/runs/40946368330. This establishes metadata consistency, not fresh hardware calibration. | reproduced |


## Reproduced milestone archival lesson (2026-10-29)

| ID | Symptom and invariant | Fix and evidence | Status |
|---|---|---|---|
| MU-GSD-014 | OpenGSD 1.19.0 milestone dry-run counts six tasks from prose headings, ignoring frontmatter actuals/tasks_completed; it also counts summarized slots rather than accepted outcomes. | `query milestone.complete v2.0 --dry-run` exits 0 with tasks=6. A one-time Python parse of all 82 SUMMARY frontmatters, preferring actuals.tasks then tasks_completed and checking agreeing duplicates, exits 0 with 192 recorded tasks. Archive records use that historical count and retain 75 accepted plans separately; no halted outcome is promoted. Run URL N/A-local. | reproduced |

## Local player onboarding (2026-10-29)

| ID | Lesson | Evidence | Status |
|---|---|---|---|
| MU-HOST-002 | A working release and install line did not make the next player action clear: the README stopped at the test card. Document Load Content, keyboard Reset versus Pause, optional BIOS and direct `RetroArch -L CORE GAME` launch together. Local convenience launchers and personal media belong outside the repository. | `sh /tmp/muhtari-onboarding-release/verify-local.sh` exits 0: installed v0.10.0 test card and locally built smasteroids each run 120 frames in RetroArch 1.22.2 and match the release runner at all 320x242 pixels. The unchanged README install-line consumer exits 0 against the downloaded ZIP; both release ZIP digests match GitHub's asset digests. Run URL N/A-local. | reproduced; README and libretro guide updated |

### Voluntary gameplay observation: Choplifter! (USA)

The owner played a local cartridge dump with keyboard controls through the released
v0.10.0 arm64 core in RetroArch 1.22.2 on Apple Silicon. They reported that graphics,
sound and speed looked good, with no visual, audio or timing issues noticed during
casual play. They also successfully loaded Choplifter through RetroArch's GUI.
Session duration and game progress were not recorded. This is positive
human gameplay feedback for this image and setup, not calibrated timing, a complete
playthrough, cross-platform evidence or a recurring manual verification obligation.
It does not change the generated compatibility ledger's automated statuses.

Reproduction command used (personal paths expressed through `$HOME`):

```sh
/Applications/RetroArch.app/Contents/MacOS/RetroArch --verbose --log-file "$HOME/Documents/muhtari/Logs/launch.log" --config "$HOME/Library/Application Support/muhtari-play/retroarch.cfg" -L "$HOME/Library/Application Support/RetroArch/cores/muhtari_libretro.dylib" "$HOME/Documents/muhtari/Games/Choplifter! (USA).a78"
```

Exit 0 after the owner's session; run URL N/A-local. The dump was hash-identified
as NTSC linear 32 KiB despite its `.a78` extension having no A78 header. ROM bytes,
launch logs and any future personal results stay outside the repository.

### Voluntary gameplay observation: Ninja Golf (NTSC)

On 2026-10-29 the owner reported playing Ninja Golf (NTSC): "played great".
This is positive voluntary human gameplay feedback in the ongoing local onboarding
session. Controls, duration, progress and image identity were not specified; no
separate agent-run command or exit status is claimed. Run URL N/A-local.
Retain the observation without creating recurring owner UAT or changing the
generated compatibility ledger's automated statuses.

## Owner intent and sibling transfer (2026-10-29)

[INTENT.md](INTENT.md) preserves the owner's gamer-first priorities, Playstead
quick path/north star, writing voice, bounded primary-source fan-out and two-way
emulator-family learning. These are dated instructions, not reproduced technical
results. The [planning index](README.md) makes their sources and related lessons
discoverable. Current source-backed [research](research/SUMMARY.md) separates
inspection from inference and leaves implementation acceptance to later phases.
No new hardware or integration lesson is declared reproduced by this scoping step.

## Local diagnostic evidence (2026-10-31)

A record-aligned prefix can hide a late write failure. Initial local tracing
accepted an unfinished helper and ignored a core write error after INIT. The
bounded review exposed both; actual collector/wrapper controls reproduce the
failure boundaries. Require helper END/saturation, reject captured writer-failure
diagnostics, label killed core prefixes unfinished with writer health unknown,
and never infer missing input from missing records alone. Parent/child PIDs and
valid timestamps still require same-host/clock provenance from fresh descriptor
ownership before comparing event times.

Evidence: `python3 /private/tmp/muhtari-tracer-review-controls.py` exits0 after
both old-guard observers reject1; corrected `policy.retroarch.preflight_api`
exits0, N/A-local. Durable scope, fault mechanisms and final full-workflow receipts
are in [the approved local outcome](debug/phase12-input-tracer-proposal.md#approved-local-execution-outcome-2026-10-31).
This is diagnostic reliability, not resolution of the actual hosted input cause.

## Diagnostic artifact shell guards (2026-10-31)

MU-CI-013 (reproduced): Bash exempts negated commands from `errexit`.
The initial diagnostic identity script used `! cmp -s` to reject a diagnostic
module identical to the ordinary module; the actual extracted script continued
and exited0 for that substitution. An explicit `if cmp -s ...; then exit 1; fi`
rejects it. Hash/source identity checks alone cannot establish this branch exits.
The existing offline workflow consumer now exercises the real shell script with
valid and substituted modules, wrong run/source/parser and missing-digest faults.
`cmake -DROOT="$PWD" -DWORK=<scratch>/policy -P tests/policy/retroarch_preflight.cmake`
exits0 after the substitution control rejects; N/A-local. This establishes local
guard behavior, not hosted artifact delivery or input acceptance. See the
[local wiring outcome](debug/phase12-hosted-input-tracer-proposal.md#approved-local-wiring-outcome-2026-10-31).

## Portable diagnostic test compilation (2026-10-31)

MU-TEST-010 (reproduced): warning-clean native Clang diagnostics do not establish
GCC warning compatibility. A discarded `write` result and fixed-size paths
formed by appending suffixes to another full-size buffer passed local/Mac
checks but failed the required Linux producer builds under warnings-as-errors.
Use checked syscall results and prove path capacity in portable test fixtures;
retain each compiler's warnings and the existing required producers. The
unchanged quality join correctly rejects missing Linux artifact producers.

Evidence: both Linux build steps exit1 in
[run41208847091](https://github.com/szTheory/muhtari/actions/runs/41208847091);
`python3 /private/tmp/muhtari-frozen17-terminal.py` exits0 after complete
20job/step/artifact/log reconciliation, while the required CMake audit exits1.
The Mac diagnostic artifact identity/digest verifies0, but RetroArch is skipped
and no child/input observation exists. This establishes a test compilation
blocker, not the original hosted input cause; no repair or retry is claimed.

## Exec descriptor oracle ownership (2026-10-31)

MU-TEST-011 (reproduced): an exec-time descriptor oracle must identify the
helper's owned files, not reject unrelated inherited descriptors. Compare
`(st_dev, st_ino)` and reject only aliases of the helper file, trace directory
or core file; inode-only checks and blanket close-on-exec expectations are not
portable ownership tests.

Evidence: [run41224305264](https://github.com/szTheory/muhtari/actions/runs/41224305264)
fails both Linux `policy.retroarch.preflight_api` producers at the old combined
inode/CLOEXEC assertion before RetroArch. The corrected synthetic native/Linux
pass and owned-alias fault controls, full local workflow, hygiene, budget checks
and bounded independent review pass locally. See the [phase 12 debug record](debug/phase12-packaged-input-tracer.md).
The hosted descriptor's exact identity and the original RetroArch input cause
remain unknown; no second hosted attempt is claimed.

## Container image inspection (2026-11-04)

MU-TEST-012 (observed): Docker image config templates must tolerate optional or
absent keys. Directly evaluating `{{.Config.Volumes}}` can fail the entire
inspection when that map key is absent, before any image identity fields are
returned. Prefer structured JSON plus a parser that handles missing keys, or
prove the template's missing-key behavior with a local fixture before using it
as an admission gate.

Evidence: Plan 14-22's sole `docker image inspect` exited 1 with `template
parsing error: template: :1:192: executing "" at <.Config.Volumes>: map has
no entry for key "Volumes"`. Docker version/context/VM checks passed, but no
image identity, container or project test was observed. The qualification was
not retried; run URL N/A-local.

## Concise owner handoffs (2026-11-03)

| ID | Symptom and invariant | Fix and evidence | Status |
|---|---|---|---|
| MU-GSD-018 | Long, jargon-heavy handoffs with several candidate commands made the owner translate state after the work was complete. A workflow-suggested command can be invalid under an already-used cap. | Reproduced in the Phase 12 closeout: `node <GSD_TOOLS> query verification.status .planning/phases/12-player-ready-delivery-and-playstead-bridge` exits 0 with `gaps_found`; `node <GSD_TOOLS> query init.execute-phase 12` exits 0 with zero runnable plans, while the one allowed gap round was spent. Give one short recommendation and its exact valid command; when none is valid, say so and identify the authority needed. Assume the owner follows the recommendation; ask only at a serious scope, authority, risk or explicit-cap boundary. `AGENTS.md` now carries this preference. | adopted |
| MU-GSD-019 | A blocked phase must not leave the owner without a forward path, and a stale branch can make completed work look runnable. | Reproduced 2026-11-03: the current Phase 12 branch still described Phase 13 as the next runnable work, but `origin/main`'s roadmap and Phase 13 verification show Phase 13 shipped and Phase 14 pending; Phase 12's host allowance remains spent. Before recommending a stop or next phase, reconcile the current branch against `origin/main` and the roadmap. Continue independent approved work; if all remaining paths need new authority, give the exact decision and resume command. | adopted |
| MU-GSD-020 | A test-workload cap charged before any selected test starts can exhaust useful capacity on a command/preflight error. | Reproduced in Phase 14: diagnostic pair 3 used invalid CTest `--output-failure`, launched no tests, but consumed the three-pair cap; Plan 14-12 initially repeated the same rule for tool/option/prelaunch failures. Preflight syntax, identities and capabilities without running tests; charge the workload allowance at the first protected test launch, while any failure/non-reproduction after launch consumes it. Evidence: [14-10 summary](phases/14-proven-frontend-states-and-responsive-play/14-10-SUMMARY.md#issues-encountered), [14-10 resource record](phases/14-proven-frontend-states-and-responsive-play/14-RESOURCE-EVIDENCE.md#plan-14-10-bounded-bios-diagnosis-local-2026-11-03), and revised [14-12 plan](phases/14-proven-frontend-states-and-responsive-play/14-12-PLAN.md). | adopted |
| MU-GSD-021 | A `gsd_run` earlier on `PATH` can be a different OpenGSD version and return a different verification route, leading the orchestrator to mistake stale evidence for a current gap verdict. | Reproduced 2026-11-09: PATH-selected GSD 1.20.1 reported `gaps_found`; the workflow-selected Codex GSD 1.20.0 reported `stale`, with `next_command: $gsd-execute-phase 12`. The phase index showed zero runnable plans and all five incomplete plans blocked by recorded halts, selecting the verifier-only route. Use the exact runtime selected by `gsd-core/references/gsd-run-resolver.md` for all status, planning, and execution queries; compare runtime identity/version before trusting a PATH shim. [Verification report](phases/12-player-ready-delivery-and-playstead-bridge/12-VERIFICATION.md); local query exits 0. | adopted |
| MU-GSD-022 | When the owner explicitly requests autonomous continuation through a milestone, a blanket prohibition on the autonomous entrypoint creates an avoidable dead end; the workflow must continue safe independent work and preserve exact safety stops. | Reproduced 2026-11-09: the owner explicitly requested GSD autonomous operation through the milestone after being given a stale no-command handoff. `AGENTS.md` rule 8 now permits that explicit request, continues independent authorized work when one phase is blocked, and preserves host/remote caps and forbidden `--auto`/`--chain` arguments. | adopted |
| MU-GSD-023 | A descriptor-less judgment advisory and routine document comparison must not become an owner acknowledgment checkpoint when project instructions assign that judgment to the agent; keep missing delivery proof blocking and reserve owner gates for actual identity-bound or hardware decisions. | Reproduced 2026-11-09 in Phase 12: `AGENTS.md` rule 6 and 12-04 task 1 assign the claim review to the agent, while unexecuted 12-24 added an owner disposition checkpoint after that review. Workflow-selected GSD 1.20.0 `query init.execute-phase 12` exited 0 with only 12-25 runnable. Plan 12-25 reviewed the current sources and retained receipts, then superseded only that checkpoint; `test -s 12-25-CLAIM-REVIEW.md` and `git diff --check <saved-source> -- 12-25-CLAIM-REVIEW.md` exited 0; run URL N/A-local. Sources: `AGENTS.md` rule 6, plans 12-04/12-24/12-25, installed `prohibition-probe.md` judgment-tier soft-gate rule. No owner response or acceptance was inferred; the advisory, accepted 0/4, and existing stops remain. | adopted |
| MU-GSD-024 | A carried conversation summary can present superseded blockers as current. Treat it as a locator, reconcile the live branch/tree, verification, review disposition and runnable-plan index, then continue independent authorized local work. | Reproduced 2026-11-09: the handoff said the Phase 12 trace leak and 2/5 score remained; current 12-23 summary records the fix, 12-REVIEW has 0 findings, and 12-VERIFICATION reports 3/5. `query init.execute-phase 12` reported zero runnable plans. Phase 14 verification exposed one review-fix control not run; `cmake --preset ci` and `ctest --test-dir build/ci --output-on-failure -R '^policy\.host_support\.controls$'` both exited 0, closing that local evidence gap. `AGENTS.md` rule 8 now requires current-artifact reconciliation on autonomous context resume. No host/remote allowance was renewed and Phase 12 remains accepted 0/4. | adopted |
| MU-GSD-025 | A later green result from a different source or scope does not erase a terminal exact-source gate, and a targeted pass without causal evidence does not explain an earlier timeout. | Reproduced 2026-11-09 while reconciling Phase 14: Plan 14-25's exact parent workflow timed out at `title.pin.self_test`; Plan 14-27's later full local workflow passed after native Wasmtime recovery, but did not satisfy 14-25's frozen source/parent gate or its explicit owner checkpoint. Plan 14-29's one bounded target run passed but closed the cause as unknown. Plans 14-30/31 ended with unknown observer receipts and explicitly spent attempts. Keep each receipt tied to its source, gate and cap; do not use adjacent success to reopen a stopped scope. A new run requires a fresh bounded plan and the authority named by that boundary. | adopted |
| MU-GSD-026 | A denied identity-bound host request blocks that evidence scope, not independent core development; close the milestone with its gaps intact and choose the next offline scope. | Reproduced 2026-11-10: the exact Phase 12 diagnostic-app request returned denied before preflight/content; refreshed evidence retained Phase 12 accepted 0/4 and Phase 14 `gaps_found`, while Phase 13 and local CI passed. The user delegated downscoping; v3.0 was recorded as `override_closeout` with nine unchecked requirements and no host/remote renewal, and v4.0 was narrowed to SEED-007 tune-up plus offline WebAssembly core work. See [v3.0 audit](milestones/v3.0-MILESTONE-AUDIT.md), [v3.0 retrospective](RETROSPECTIVE.md), and [product roadmap](PRODUCT-ROADMAP.md). Do not retry a denied request or make its denial a project-wide stop. | adopted |

## TDD parent and dispatch baseline (2026-11-03)

MU-CI-015 (reproduced): this workflow's `workflow_dispatch` quality comparison uses the
dispatched commit's first parent as its base. When a required TDD RED test commit is the
implementation commit's immediate parent, the base intentionally lacks the implementation
and fails that test. The quality job then rejects missing producer artifacts, and downstream
artifact consumers such as the opt-in host job skip. Before a single-use dispatch, inspect the
baseline source and run the independent-base check while preserving the RED evidence in history.

Evidence: the current-source local CTest control and the dispatched source `1e8f019` pass
`retroarch.states.controls`, while its parent `e638c53` lacks the receipt implementation and
fails that control. [Run 41585416888](https://github.com/szTheory/muhtari/actions/runs/41585416888)
completed with no artifacts and `retroarch` skipped. That run was not retried; the later separately
authorized Plan 14-06 dispatch is a distinct run and is recorded below. No actual-host behavior is
inferred from either run's build or test failures.

MU-CI-016 (observed): POSIX-only call paths can leave parameters unused on Windows;
with `/W4 /WX`, MSVC C4100 blocks the full CI matrix even when macOS CI and hygiene
pass. Reproduced in [run 41618546347](https://github.com/szTheory/muhtari/actions/runs/41618546347):
both Windows x64 and ARM64 fail compiling `tests/retroarch/states.c` because `self_path`
is unused at three call sites; the required aggregate and exact quality artifact join fail,
so RetroArch is skipped. Plan 14-07 consumed the parameter in each non-POSIX branch;
[run 41629846188](https://github.com/szTheory/muhtari/actions/runs/41629846188) passes both
Windows x64 and ARM64 builds for that repair. The earlier Windows failure remains historical;
this compiler blocker is resolved for the repaired source. The new run's macOS Intel
independent-parent baseline separately timed out, and its RetroArch job was skipped, so no
actual-host behavior is inferred.

MU-CI-017 (observed): a single workflow-dispatch can pass current-source CI and all required
Windows builds yet fail while executing the independent-parent macOS Intel quality baseline.
In [run 41629846188](https://github.com/szTheory/muhtari/actions/runs/41629846188),
`cmake -DROOT=. -DBUILD=build/ci -DOUT=build/ci/quality-current.json -DSHA="$(git rev-parse HEAD)" -DCONFIG=ci -DBUILD_BASE=ON -DSOURCE="$GITHUB_SERVER_URL/$GITHUB_REPOSITORY/actions/runs/$GITHUB_RUN_ID" -P tests/cmake/quality_evidence.cmake`
exited 1 because its independent-parent CTest child exited 8: `fuzz.regress.run` timed out
at 28.06 seconds. The required artifact join then exited 1 and `retroarch` was skipped.
The local exact-parent workflow passed, but one hosted observation does not establish a
general timeout or resource regression; preserve the full log and require comparable evidence
before changing deadlines, topology or caches. This lesson is observed, and the underlying
timeout cause remains unknown.

2026-11-03 addendum: Plan 14-08 applies the existing CTest `RUN_SERIAL` property to the normal
portable replay. In run [41656505025](https://github.com/szTheory/muhtari/actions/runs/41656505025),
both the frozen source and its immediate parent completed the other 692 tests before the replay;
the source replay passed in 5.48 seconds and the parent replay in 5.90 seconds. This directly
confirms the property was honored in this attempt and that the earlier timeout did not recur.
It does not establish that overlap caused the earlier timeout or that the scheduling change
improves all hosted runs. The actual-host step remained unverified because its preceding battery
job exited at the existing 300-line source guard; no frontend state behavior is inferred.

MU-CI-018 (observed): With ctest --preset ci, a relative --output-junit path
resolves under the preset's binary directory. A source-root receipt check can
therefore fail with a missing-file error even after CTest passed and wrote the
JUnit file. Use an absolute output path or locate and copy the same receipt
before validating it; do not rerun a single-use test batch to correct receipt
lookup.

Evidence: Plan 14-13 ran the approved focused CTest batch once (exit 0, seven
tests passed) with --output-junit build/phase14-gap13-evidence/junit-bios.xml;
CTest wrote build/ci/build/phase14-gap13-evidence/junit-bios.xml. The initial
source-root JUnit check exited 1 because that path was absent. Copying the
exact generated receipt to the evidence directory let the seven-row inventory
checker pass (exit 0). Run URL N/A-local; no host or CI run occurred.

Lesson MU-DOCS-001 (2026-11-10, muhtari@bd3612e)
Symptom and reproduction: `cmake -DROOT=. -P tests/cmake/budgets.cmake` exited 0 and reported two files, while the equivalent absolute-root command reported 22. A synthetic CTest control failed on the relative-root exact-limit assertion with CTest exit 8; `gsd-tools check tdd-red-evidence` classified the intended target failure as `RED_EVIDENCE_OK`.
Root cause / invariant: Resolve a caller-supplied relative root against the caller's working directory once before using it for both `file(GLOB)` and `file(SIZE)`. The checked set must not depend on how the same source root is spelled.
Fix and evidence (command, exit status, run URL): `cmake -DROOT=. -P tests/cmake/budgets.cmake` and the absolute-root equivalent now both exit 0 and report 22 files. `cmake -DROOT="$PWD" -DWORK="$PWD/build/phase15-evidence/budget-controls" -P tests/policy/budgets.cmake` exits 0 after checking four exact-limit files, eight attributable one-byte over-limit faults, and both missing-root failures through both root forms. The registered `docs.budgets` and `docs.budgets.controls` tests pass 2/2; run URL N/A-local.
Applies when: A CMake script accepts a relative directory and uses it in globbing or file checks.
Does not establish: Hosted CI efficiency, queue/job timing, or any Phase 12/14 host acceptance.
Suggested check: Keep a synthetic source tree with README, public header, regular docs, and hardware docs; verify exact limits and one-byte faults with both absolute and relative roots from a working directory containing spaces.
Sources: `tests/cmake/budgets.cmake`, `tests/policy/budgets.cmake`, `docs/testing.md`, and `.planning/milestones/v4.0-phases/15-measured-milestone-tune-up/15-TUNEUP.md`.
Status: reproduced

## WebAssembly evidence attribution (2026-11-10)

- **Reproduced:** A source identity that hashes C files but omits implementation headers cannot establish matching native/WebAssembly sources. The Phase 16 disposable-copy control changed only `src/internal.h`; the corrected identity changed and returned to its original value on restoration. Include compile headers and generated version input in both fingerprint material and configure dependencies.
- **Reproduced:** Shifting Perl wait status alone discards signal termination. The corrected bounded runner explicitly checks normal exit, nonzero exit, signal termination and timeout, and the SDK lane invalidates earlier output receipts before execution. This prevents an interrupted run from presenting old evidence as fresh.
- Evidence: `cmake --workflow --preset ci` and `sh scripts/emscripten.sh build/ci`, both exit 0; run URL N/A-local. Commands and representative controls are recorded in [the Phase 16 fix report](milestones/v4.0-phases/16-emscripten-libretro-core/16-FIX-REPORT.md). This establishes local Node/core behavior; it supplies no browser/device or Phase 12/14 host acceptance.

## Milestone archive handoff reconciliation (2026-11-10)

Reproduced with OpenGSD 1.20.0: `milestone.complete v4.0 --dry-run` and the confirmed archive counted two tasks and one accomplishment although the two actual plans contain five tasks. Reconcile generated statistics with plan task blocks and both summaries; preserve a verified planning closeout separately from publication. The archival move also left verification covered paths under the old phase directory and a collapsed roadmap with no ship completion record. Before recommending ship, retain compact completed phase records for roadmap consumers, resolve the archive with `query find-phase` when `init.phase-op` masks its directory, compare moved files byte-for-byte with the pre-archive commit, and have the verifier relocate covered paths and regenerate its fingerprint through the CLI.

Evidence: `query verification.status` returned `stale` for moved PLAN/SUMMARY paths; five moved covered files matched the pre-archive source, and both archived reports then returned `passed` after verifier-owned relocation. `query roadmap.analyze` reports both 15 and 16 complete; `query init.phase-op 16` resolves the archive. All commands exited 0; run URL N/A-local. No implementation, acceptance verdict, host authority or remote allowance changed.

The installed `cmdInitPhaseOp` intentionally masks an archived directory when its phase number appears in the roadmap; minimal detail sections alone do not solve this. The project handoff documents the same-runtime `find-phase` adapter while retaining canonical verification and roadmap-completion gates. This requires no runtime modification or phase re-execution.

## Qualify shell-heavy hygiene timeouts by execution boundary (2026-11-11)

The unchanged one-slot hygiene preset can exceed its per-process tree-test deadline
inside the sandbox even when the same test and full preset pass outside it. At
`fc7d0cb`, `python3 build/phase18-hygiene-gate.py` inside the sandbox exits 8:
only `hygiene.tree` times out at 60.02s, while 29 other checks pass. The unchanged
`ctest --test-dir build/hygiene --output-on-failure -R '^hygiene[.]tree$'` outside
the sandbox exits 0 in 37.52s at the same 60s deadline; the full unchanged helper
outside exits 0 with 30/30 checks in 87.57s. Scoped process inspection finds no
leftover attributable process. This supports execution-boundary overhead, not a
specific kernel or process cause. Qualify the boundary before widening limits or
changing membership; do not weaken the check or recommend a global restart from a
timeout alone. All commands are local (`N/A-local`); ignored output is in
`build/v6-phase19-hygiene-qualified.log` and other build-only receipts.

## Shipping consumer coverage (2026-11-10)

The v4.0 PR exposed three distinct gates absent from the ordinary native workflow: pinned formatting, genuine-base fixture identity in the quality producer, and sanitizer-sensitive observer PCM parity. An owned fixture addition needs its exact named identity and representative pass/fault controls in the existing quality normalization consumer. The native audio API is mono; compare each sample against both libretro channels before hashing the duplicated stereo stream. Reading twice the native sample count can appear to pass on a silent fixture with zero-looking stack bytes.

Evidence: PR #46, CI https://github.com/szTheory/muhtari/actions/runs/42903652231. The non-silent libretro.observe.controls case fails before the PCM fix and passes after it; policy.quality.controls fails on the valid frontend-state addition before the identity fix and passes after it. Source corrections retain all original pins, bounds and failure detection. Hosted acceptance remains a separate exact-source verdict.

## Portable archive boundaries (2026-11-11)

- **Reproduced:** Adding one asset to the publisher iterator also changes older fixtures outside the main publisher-control file. The eighteen-archive hygiene fixture failed admission until its portable fixture and intended native-omission control were updated. Audit all exact inventory consumers before changing a release set.
- **Reproduced:** Lexical work-directory containment does not protect cleanup/extraction from symlink escapes. Validate physical ownership and archive membership/type/expansion before any downstream scanner extracts an archive. The focused controls preserve outside sentinels and reject over-limit declared member/total expansion before extraction. The uncompressed scratch-defense bounds do not revive the retired compressed-size quota.
- **Reproduced:** Generated archive names in a notice entry can be mistaken for tracked source paths. Describe generated applicability accurately while keeping actual upstream notice identities explicit; retain the notice-policy checker. Stricter work ownership also exposed a slash-bearing control name, fixed only in its work-directory spelling.
- Evidence: final native workflow via python3 build/ship-local-gate.py, sh scripts/emscripten.sh build/ci, production publisher controls and qualified one-slot hygiene each exited 0 at the Phase 18 final product snapshot, N/A-local. The Phase 18 summaries, review fixes and verification retain the earlier failures, source identities and concrete pass/fault commands. No host/remote scope was renewed and no lesson was exported.

## Clean package setup and byte-derived fixture hashes (2026-11-11)

- A warm SDK build hid a missing parent for the package test's WORK directory. The first v5.0 hosted CI and rehearsal both fail the fresh producer at `WORK parent must exist`; a locally preserved warm tree followed by a clean SDK tree reproduces exit 8. Using distinct direct BUILD children for producer and controls preserves the physical-containment/symlink checks and passes the clean pinned-SDK workflow, exit 0. Existing fresh hosted jobs remain the recurring regression; do not pre-create scratch parents during diagnosis and call that clean-build evidence.
- Hash fixture bytes after writing, rather than hashing an LF string and assuming its bytes were stored verbatim. Windows publisher controls reject the valid synthetic notice set because CMake's text-mode WRITE produces CRLF. Synthetic notices now use explicit CRLF on every platform and derive hashes from the packed files. That same local publisher control fails the old string hash (exit 1, exact notice-digest error), then passes the file hash (exit 0), while strict production validation still rejects synthetic notices. The real pinned SDK notice table is unchanged. CMake's [versioned implementation](https://raw.githubusercontent.com/Kitware/CMake/v4.4.3/Source/cmFileCommand.cxx) confirms text-mode WRITE; this is a fixture correction, not a notice-policy relaxation.

Evidence: [first candidate CI](https://github.com/szTheory/muhtari/actions/runs/43037222032), [first rehearsal](https://github.com/szTheory/muhtari/actions/runs/43037222263); `sh scripts/emscripten.sh build/ci` on fresh output (exit 8 before, exit 0 after); `cmake -DROOT="$PWD" -DWORK="$PWD/build/v5-ship-crlf-red" -P tests/policy/publish.cmake` (exit 1 before file-byte hash), corresponding green work directory (exit 0). Local URLs N/A-local. Superseded hosted runs remain failed/cancelled evidence and establish no acceptance. No earlier host scope, deadline, required membership or production digest was changed.

A producer-workspace pass does not prove release artifact transport. The downloaded v5.0 SDK evidence omitted `muhtari_libretro.info`, although the publisher independently compares it with the archive member. Actual downloaded-layout validation failed, exit 1; adding the independently built metadata to a prospective corrected layout passed exact-source validation and clean-extracted Node/native parity, exit 0. The CI upload now includes that file, and the existing publisher fixture copies only the build files declared in that actual upload block rather than the whole producer directory. Removing the upload entry fails the existing control, exit 1; restoring it passes the registered control in 58.43s, exit 0. Run source: [candidate rehearsal](https://github.com/szTheory/muhtari/actions/runs/43039375566); downloaded artifacts remain build-owned. This confirms the local transfer correction; a future actual hosted upload/rehearsal must establish its own outcome. Prefer this cheap transfer check before another full release matrix.


## Hosted Ubuntu package routing (2026-11-11)

A global green service page and local DNS success do not test the hosted runner's resolver. The isolated [APT diagnostic](https://github.com/szTheory/muhtari/actions/runs/43047525060) reproduces lookup timeouts for all three Ubuntu-owned mirror names under both the runner and APT users while Microsoft and kernel.org resolve immediately. Original signed APT setup reaches the diagnostic ceiling (exit 124); the alternate kernel.org route fetches/authenticates all four unchanged Resolute suites and installs real Valgrind successfully (exit 0). This establishes a usable alternate route, not the underlying provider-side DNS cause or shipping acceptance. The image already routes both ordinary and security suites through its mirror-list file, so adding the working mirror ahead of existing entries is sufficient; source suites/components/keyring are untouched. Preserve signed APT verification, package arguments, current tests/deadlines and original failed receipts.

The diagnostic's expected baseline failure uses `continue-on-error`; GitHub consequently displays that step's conclusion as success despite its exit-124 annotation. Inspect actual logs/outcomes instead of treating the green diagnostic aggregate as evidence that the original route recovered. The native offline control rejects the original setup (exit 1), accepts the helper, and rejects failed index refresh, installation, missing mirror configuration and missing package arguments. No external code was copied. Sources: [APT mirror transport](https://manpages.ubuntu.com/manpages/jammy/man1/apt-transport-mirror.1.html), [APT signed archive verification](https://manpages.ubuntu.com/manpages/resolute/man8/apt-secure.8.html). The owner explicitly authorized this scoped recovery after the read-only handoff proved unhelpful; this does not reopen earlier host attempts.


## Shared registration and reduced inventories (2026-11-11)

A new native shell policy control was initially registered outside the existing CI-only block. Native full workflows passed, but the WASI configure's exact inventory gate rejected the extra unlisted control. Both [direct WASI](https://github.com/szTheory/muhtari/actions/runs/43049895244/job/130646412738) and [rehearsal WASI](https://github.com/szTheory/muhtari/actions/runs/43049895310/job/130646984364) fail at configuration; no runtime pass is inferred. Even a report-only lane's failed registration contract must be repaired. Local `cmake -S . -B build/wasm32` reproduces that exact extra-test error (exit 1); moving the control into the existing native-CI block makes both WASI and native exact configuration pass (exit 0), and the native control still executes successfully. Configure every affected inventory locally after shared registration changes before an expensive hosted matrix. Reuse the existing configure-time exact inventory guard rather than adding another registry or weakening the smaller inventory. No test assertion or workload is removed.


## Windows deadline calibration from complete cases (2026-11-11)

A test can repeatedly time out without a behavior change because its historical limit lost the project's required headroom. The isolated-instance property has an old 39-second limit derived from 19.03 seconds, yet an unchanged complete Windows run takes 37.45 seconds. Lightgun's old six-second limit was derived from 2.70 seconds; unchanged complete Windows work now takes 5.68 seconds. Both also have actual timeout receipts. Four completed Windows x64/ARM jobs across two source40 workflows and the newer a3e lightgun pass establish comparable measurements; a Git comparison shows core/property/input/fixture/preset/version bytes unchanged. Apply DEC.47 narrowly to Windows: 75 and 12 seconds, preserving workloads, seeds, assertions, other case/platform limits, scaling and job caps. A censored timeout is not a completed timing sample, and no host-load root cause is claimed. [Measurements, failed history and commands](milestones/v5.0-SHIP.md#measured-windows-case-deadlines). Bound fixes restore finite, meaningful verification; repeated same-limit retries merely spend another matrix on a known margin defect.


## Tagged release exposes another stale Windows replay bound (2026-11-11)

The unchanged fuzz replay completes in12.94s on Windows but retains a14s limit derived from6.64s; the actual tagged release later times out at14.05s with the other692 native cases passed. Complete comparable Windows samples, including the newer13.60s same-source successful job, justify28s under DEC.47, preserving all seeds/assertions and other lane limits. A fresh runner might hide lost headroom; it does not repair the measured bound defect. Also keep release identity exact: a later CI correction cannot be silently applied to an already tagged source. The existing CI-only/no-release convention makes a patch-publication exception an explicit owner decision. [Failure, measurements and controls](milestones/v5.0-SHIP.md#unpublished-tagged-release-replay-deadline-correction).


## Preflight shipping metadata with its existing guards (2026-11-11)

A one-time release-policy exception still has to satisfy the existing title and single-version-source guards. `fix(ci)` is rejected by the scope rule; an owner-approved recovery uses an accurate unscoped `fix` title and leaves ordinary pipeline-scope rejection intact. Naming current/next product versions in a maintainer exception duplicates version.txt and is correctly rejected; link the one approved recovery PR and keep version identities in the release record. Reproduced locally and in [the full equality job](https://github.com/szTheory/muhtari/actions/runs/43082790502/job/130739393140). Before publishing metadata changes, run the actual title/body guard, the hygiene inventory's policy.title fixtures, current/next version checks and document budgets. A no-tests result is not evidence; select the owning inventory and use `--no-tests=error`. Batch those repairs before the next exact-source acceptance run.


## Calibrate the remaining native fixture margins as a class (2026-11-11)

A required one-second OpenBIOS setup can time out at1.01s and leave15 required dependents unrun even when another exact-source CI build completes the same fixture. Separate censored failures from complete measurements and separate target cohorts. Four completed Windows run pairs give OpenBIOS maxima0.52s x64/0.79s ARM64, PAL BIOS3.60s x64 and directions1.73s x64; DEC.47 yields2/2/8/4s. A bounded scan of the already observed near-limit cases avoids further one-case publication loops; all assertions, fixture relationships, scales and job caps stay intact. Keep immutable tagged-source publication distinct from ordinary non-release CI maintenance on current main. [Measurements and checks](milestones/v5.0-SHIP.md#non-release-fixture-margin-maintenance).

## Incoming integration hypotheses — not locally reproduced (2026-11-11)

LibFuturo's PAL evidence corrections are relevant when another Muhtari consumer
is added: identify the actual submitted input bytes, bind each source/installed
route to its own anchored records, and validate PAL extents independently of
NTSC. The sibling changes and retained local verification are source-backed;
their applicability as a new Muhtari control remains a hypothesis until a local
fault/control reproduces the need. Do not add another generic evidence layer
solely because the sibling has one.

## Merge every active audio source before advancing the mixer (2026-11-12)

The YM+dual-POKEY+TIA stress workload passed Release consumers while assertion-enabled
Apple ASan/UBSan stopped at the mixer's pending-timestamp ordering assertion. YM was
catching up after a later POKEY edge had already emitted. Register activity and
cross-route equality did not expose the shared scheduling error. Registering the
existing monotonic raw observer on that same stress workload reproduces the defect
in Release, with all twelve sources actually changing in both regions.

The fix merges YM through each TIA event and then the existing POKEY half-edge
limit, preserving actual slow/held-cycle timing, timer batch bounds and the no-YM
path. Never advance an independent audio source to the end of a batch after another
source has already queued a later event. Keep a Release-mode ordering assertion
alongside debug assertions when the maximum coinstalled topology is supported.

Evidence: focused CTest hostile case in build/v6-phase20-ordering-before.log exits8
before the production fix; seventeen focused Release and qualified Apple sanitizer
tests exit0 after it. Final complete native, hygiene, compatibility and actual
emitted/extracted Wasm gates also exit0 at the reviewed product snapshot. Commands,
source identities and limits are retained in the Phase20 cost report; URLs N/A-local.
No hardware, retail or other platform acceptance is inferred.

Provenance: LibFuturo local commits `ce9028e`, `98f935c`, `4fb7d1c` and the later
core-profiles Phase 2 verification; the read-only successor snapshot and exact
paths are retained in [CONSUMERS](research/next-milestone/CONSUMERS.md). No sibling
build/test/host/CI was launched here. Browser `lf_*` proof, native snapshot proof,
Muhtari `retro_*` artifact proof and hardware accuracy keep separate acceptance
boundaries. Revisit on a concrete new input/package/profile mismatch.

## Reset private audio observation state after a machine rewind (2026-11-12)

The audio observer's pending and carried events are derived from one forward
timeline and are not part of serialized machine state. Loading an earlier
checkpoint while retaining a carry from the later timeline makes the next frame
compare its new deadline with an event from the abandoned future; ASan exposed
this in both YM PAL numeric-hash consumers. After a successful state apply,
discard pending/carry records and sync the raw cache to the restored synthesis
state, while preserving the timing cursor. The public-region observer regression
compares the exact replay summary with a control continuation after advancing
through an abandoned future.

Reproduced on the initial shipping head in [ASan job 131232281140](https://github.com/szTheory/muhtari/actions/runs/43232708698/job/131232281140) and [ASan job 131218504292](https://github.com/szTheory/muhtari/actions/runs/43228680727/job/131218504292); the PAL numeric report aborts at `muhtari__observe_begin` after a state rewind. Review then caught that discarding the carry alone left its future level in the raw cache. The final fix resets that cache from restored synth state. The updated assertion-enabled local checks pass: `cmake --build build/v6-ym-sanitize-apple --target muhtari-test-ym2151 muhtari-test-state muhtari-run --parallel 4` (exit 0); `ctest --test-dir build/v6-ym-sanitize-apple --output-on-failure -R '^(ym\.observer|core\.state\.equality_report)$'` (exit 0, 2/2); and `cmake -DCASE=hashes_state_control -DHASHES=$PWD/tests/cmake/hashes.cmake -DSTATE_TEST=$PWD/build/v6-ym-sanitize-apple/tests/muhtari-test-state -DOUT=$PWD/build/v6-ym-sanitize-apple -DEMULATOR= -P tests/runner/tests.cmake` (exit 0, including all 78 state rows and semantic fault controls). `ctest --test-dir build/v6-ym-sanitize-apple --output-on-failure -R '^policy\.lane_equality\.controls$'` passed with its fixture setup tests (exit 0). The broader `runner.hashes_file` attempt in this local sanitizer build timed out at its 286-second CTest bound while processing the expansion matrix (exit 8); it is not counted as a pass. The first corrected-head CI attempt caught a formatting-only issue at `tests/audio/test_ym2151.c:1355` ([hygiene job](https://github.com/szTheory/muhtari/actions/runs/43248680525/job/131288210655)); after formatting with `/opt/homebrew/opt/llvm@21/bin/clang-format -i tests/audio/test_ym2151.c`, `cmake --workflow --preset hygiene` passes locally (exit 0, 30/30). Fresh hosted CI on the follow-up commit remains the shipping check.

## Keep compiler-visible masks within their destination width (2026-11-12)

With MSVC `/W4 /WX`, casting a complemented unsigned constant to `uint8_t` warns C4310 because the complement is wider than the destination. The warning becomes a Windows x64 and ARM64 build failure in both production and test-fixture code. Express the equivalent low-byte mask directly (for example `UINT8_C(0xfb)`) so the operation preserves its bit behavior without a truncating constant cast. The production warning appears in [CI run 43228680657](https://github.com/szTheory/muhtari/actions/runs/43228680657); after that correction, the compatibility fixture warning appears in [CI run 43231743577](https://github.com/szTheory/muhtari/actions/runs/43231743577).

## Keep quality observations within each lane's registered inventory (2026-11-12)

The hygiene preset registers repository checks and deliberately returns before registering product tests. A quality report must therefore mark the YM workload as not measured in that lane instead of invoking a test executable that is absent from its clean build. Product lanes still execute the workload and fail if its binary or report is missing. The hosted hygiene job reproduces the hidden artifact dependency with `hashes: YM numeric report ntsc: no such file or directory`: [CI run 43228680657](https://github.com/szTheory/muhtari/actions/runs/43228680657).

## Scale nested observer deadlines with the emulated lane (2026-11-12)

`muhtari_add_test` scales CTest's outer deadline, but a CMake script's
`execute_process(TIMEOUT ...)` is independent. On the `dff2f7b` merge commit,
`libretro.observe.ym` passed in the native inventory and timed out at the
wrapper's fixed 25-second child bound in the PowerPC/QEMU inventory; the outer
CTest bound was 390 seconds. This is a harness deadline mismatch, not an
observer-output mismatch. Pass `25 * MUHTARI_TIMEOUT_SCALE` to each observer
child while keeping the existing CTest multiplier and nonzero-exit failure.

The original `cmake --workflow --preset be32-full` lane failed at that child
timeout in [Nightly equality job 131292885760](https://github.com/szTheory/muhtari/actions/runs/43250000793/job/131292885760).
After the fix, the native script invocation passes with `OBSERVER_TIMEOUT=25`
(exit 0, N/A-local); `OBSERVER_TIMEOUT=0` fails with the expected positive-bound
diagnostic (exit 1); and a `/usr/bin/false` observer still fails with the
observer-failure diagnostic (exit 1). `qemu-ppc` is unavailable locally, so
only the exact-source hosted BE32 run can establish the scaled emulated path.

## Apply the configured emulator inside CMake script-mode consumers (2026-11-12)

CTest does not prepend `CMAKE_CROSSCOMPILING_EMULATOR` to binaries that a
`cmake -P` script starts with `execute_process`. In the full BE32 release
inventory, `release.ym_consumers` successfully extracted and built the package,
then the host shell tried to interpret its PowerPC consumer and returned
`Syntax error: ")" unexpected`. Pass the build emulator list to the script and
prefix every guest executable it starts, including nested runner checks and
the libretro host. Native direct invocation remains an empty emulator list.

This occurred on the exact shipping head in [Release equality job
131316658115](https://github.com/szTheory/muhtari/actions/runs/43256470708/job/131316658115);
the uploaded JUnit report identifies only `release.ym_consumers` in
`build/be32/junit-release.xml`, while both normal native and BE32 inventories
completed. The targeted local Apple sanitizer build has Debug package members,
so it cannot exercise this Release package check; `qemu-ppc` is unavailable
locally. A fresh hosted BE32 run after the correction is the required proof.
