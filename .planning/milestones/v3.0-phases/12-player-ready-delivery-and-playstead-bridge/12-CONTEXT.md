# Phase 12: Player-ready delivery and Playstead bridge - Context

**Gathered:** 2026-10-29; updated 2026-11-02
**Status:** Phase 12 remains blocked at 0/4 while local diagnostic repair planning continues.

<domain>
## Phase Boundary

Deliver TUNE-06, TUNE-07 and PLAY-01 through PLAY-04: current player onboarding,
a release-qualified external RetroArch launch contract, automated owned-fixture
host-readiness evidence, an explicit Playstead handoff and one measured small
pipeline improvement. The current published baseline is package v0.11.1,
API8/ABI1/behavior6. Phase 13 merged in PR #42 and its portable state API is in
the v0.11.1 release. Actual RetroArch save/load, rewind/run-ahead and Playstead
use remain unverified; frontend state evidence belongs to Phase 14. All six Phase
12 requirements remain pending.

Muhtari owns the library, headless runner and libretro core. Playstead owns its
adapter, installer, verified cache, readiness, input/focus and child lifecycle.
Its actual launch/input/orderly-return/battery flow is an external dependency;
independent muhtari state work can proceed while it is pending. Portable states
belong to Phase 13 and actual frontend state consumers to Phase 14.

Phase 12 remains at accepted 0/4; the PLAY-03 host-input cause is unresolved and
all recorded hosted allowances are spent. The owner directed continued local
diagnosis, lesson recording and planning for a bounded diagnostic repair. The halt
applies to external/hosted actions using spent allowances; it does not stop this
local work. This update authorizes no new RetroArch/CI run, publication, retry,
timeout change or allowance renewal.

</domain>

<decisions>
## Implementation Decisions

The owner selected consideration of all relevant areas and explicitly delegated
adoption of coherent recommendations. Three read-only advisor reviews covered
player delivery, host/sibling acceptance and the bounded tune-up. The synthesis
applied player/UX, host API/lifecycle, security/provenance, independent verification,
performance and release-operations lenses, followed by one adversarial pass.
These are discussion decisions, not implementation or hosted acceptance evidence.

### Current player path and external launch contract

- **D-01 (PLAY-01):** Preserve the current short gamer-facing README structure and
  working install line. It already supplies downloads, platform/architecture choice,
  GUI and CLI loading, keyboard controls, Reset versus Pause, optional BIOS and
  qualified compatibility. Make only residual corrections needed for release truth
  and clear battery-save versus unavailable-state wording; keep details in the
  existing libretro guide. Do not redesign onboarding or add a launcher/package.
- **D-02 (PLAY-02):** Put the integrator contract in `docs/libretro.md`, linked by
  the existing Playstead handoff. Describe separately verified RetroArch app and
  muhtari core artifacts: exact version/tag, source/release identity, digest,
  core attestation where supplied, matching architecture, extracted core and info
  paths. Keep the convenient README `latest` URL for players; an adapter records
  immutable tested pins. Reuse existing release verification and install consumers.
- **D-03 (PLAY-01/02):** Document the real `RetroArch -L CORE GAME` boundary with
  explicit local config, content, per-game save and optional firmware directories;
  quote paths and avoid dependence on ambient user configuration. Fast boot remains
  the default. Explain usable controller bindings/keyboard fallback, focus ownership,
  graceful stop and diagnostic logs. Distinguish probe refusal, core load refusal,
  host startup failure, clean exit and forced kill; RetroArch's exit codes are not
  the runner's status table. A zero exit alone proves neither successful loading
  nor save flushing. Check examples against current support, including the stale
  unsupported-BankSet example in the guide. No new machine-readable launch schema
  or general continuation protocol is justified by this single consumer.

### Packaged-core host readiness and Playstead ownership

- **D-04 (PLAY-03):** Reuse the existing actual-RetroArch screenshot/diagnostic and
  fresh-process battery cases, their independent expected pixels/media and negative
  controls. Existing CI already extracts the candidate archives; retain that path
  and qualify the exact extracted libretro archive rather than substituting a
  direct build-tree module. Recheck the current host/core pins before implementation and record
  precisely which candidate or published release bytes each result establishes.
  PR candidate-archive evidence and published-download evidence are distinct;
  truths available only after publication are confirmed at ship using the existing
  release/install path. Never label a source-build-only result as released-core proof.
- **D-05 (PLAY-03, updated 2026-11-01):** Retire the test joypad investigation
  for the current pinned RetroArch 1.22.2 binary. Inspection of the exact released
  executable matching the pinned DMG found no test-input keys or test joypad;
  reconsider availability only after a host pin changes. Keep the ordinary
  candidate-core RetroArch case and the existing owned input-sensing fixture.
  Acceptance still requires independently judged visible held and released input
  through the actual packaged core and host. In-process callback checks and local
  traces are supporting diagnostics only; they do not prove input reached the
  emulated game or changed its visible result. Missing, empty, truncated, invalid,
  unpaired, or unfinished trace data means unknown and never proves input absence.
  Run [41062201311](https://github.com/szTheory/muhtari/actions/runs/41062201311)
  showed idle pixels for held input. [Run 41323089043](https://github.com/szTheory/muhtari/actions/runs/41323089043)
  recorded 34 F8-down posts to one child (last at 4961 ms), zero captures, and
  an acknowledgment deadline at 5061 ms; the child's status was unknown.
  [Run 41329055050](https://github.com/szTheory/muhtari/actions/runs/41329055050)
  skipped RetroArch after the Windows ARM64 OpenBIOS timeout, so it produced no
  input observation. Local diagnostic-retention changes can preserve the
  prepared command and child streams but do not explain those failures or
  establish host-input acceptance. Preserve the 0/4 halt, blocked successors, failed receipts, and
  exhausted hosted allowances. Do not substitute movie/playback, OS-level event
  injection, or core-side synthetic input for actual host delivery; no new
  permission grant is authorized. This discussion authorizes no retry, rerun,
  new workflow, hosted dispatch, RetroArch launch, publication, pin/dependency/
  permission change, or allowance renewal. Any later hosted action needs a
  separate bounded scope that names the unresolved observation, source/artifact,
  cost and stopping rule. The latest [run 41461948345](https://github.com/szTheory/muhtari/actions/runs/41461948345)
  saturated its 64-record helper trace with repeated POST events before the
  acknowledgment deadline. The trace is inconclusive; the actual input cause
  remains unknown. See D-13 for the local diagnostic repair recommendation.
- **D-06 (PLAY-03):** Keep the two actual-host cases, existing no-UDP approach
  and runtime bounds. The separately approved2026-10-30 DEC.56 amendment retires
  only the numeric hosted-test line ceiling; report totals with no replacement
  quota and retain ENGINEERING §11 architecture review. Put exhaustive semantics and fault cases in
  existing C/CTest consumers. Research must establish a viable input seam and its
  bounded recurring cost before planning it. If a required outcome demonstrably
  conflicts with an existing bound, return the concrete evidence and apply
  ENGINEERING §11 to that specific boundary; no incidental quota increase or broad
  host-automation framework. Host readiness does not establish physical-controller
  ergonomics, audible quality or actual Playstead focus/handoff.
- **D-07 (PLAY-04):** Retain and refresh the source-linked Playstead handoff with
  current inspected revision/dirty-source limits, the two-artifact pin/install
  responsibility, `a7800` readiness, controller/keyboard/focus mapping, per-game
  storage, orderly child return and exact missing paired evidence. Track the actual
  Playstead → RetroArch → muhtari owned-fixture launch, expected visible response,
  usable input, graceful exit and two-fresh-process battery continuity as pending
  until paired consumer evidence exists. Make no Playstead source changes here.
- **D-08 (PLAY-04):** Keep RetroArch-managed save/load as the initial state UI once
  serialization ships. Recent play/relaunch is not exact Continue. Retain the native
  Playstead play/Continue north star and promote a focused host spike only after
  measured external-window/input/state friction or an explicit one-app release
  requirement justifies it. Native hosting, state history and recovery of newer
  durable progress remain separate work; muhtari gains no device or window code.

### One demonstrated pipeline improvement

- **D-09 (TUNE-06):** Select `policy.publish.controls` recurring Windows publication
  test overhead as the single primary tune-up. Prior observations are
  756.47/744.81s current/base in CI40135571745 and 729.04/703.82s in CI40157749294.
  The current test starts a shell/publisher for many synthetic lifecycle and
  attestation rows. Repeated process/setup cost is a hypothesis to measure, not a
  proven cause. Bound profiling to this existing consumer and implement the smallest
  confirmed remedy. Preserve all current cases, meaningful Windows execution,
  production publisher coverage and required current/base validation.
- **D-10 (TUNE-06):** Acceptance needs a shipped improvement with comparable
  complete before/after observations on the same runner class, source/workload
  identities and representative successful/fault behavior. Report the local
  workload improvement without inventing a global cost saving or speed target.
  If measurement falsifies the proposed removable overhead, return new reproduced
  evidence for a different narrow seed remedy; dispositions/docs alone cannot
  satisfy TUNE-06. Avoid a speculative DAG/matrix redesign, cache or dependency.
  COST-01 monthly numeric totals remain untrusted; the Phase 5 WR-01 delayed-release
  equal-base edge remains unresolved and fail-closed. Retain their scoped dispositions.

### Honest nightly-property evidence

- **D-11 (TUNE-07):** Choose the allowed bounded explicit deferral of property
  deadline calibration. Preserve all six currently registered properties,
  10,000 cases per property, seed reporting, every oracle and exact JUnit inventory,
  the present 390s scaled test deadlines and nine-minute job cap. Keep the job
  advisory (`continue-on-error`, outside the report's required needs). Correct the
  stale four-property wording in the existing workflow/docs. Deferral is revisited
  at Phase 12 close and, if unchanged, the next milestone tune-up; earlier complete
  qualifying evidence also triggers reconsideration.
- **D-12 (TUNE-07):** Separate a complete successful property cohort from a job
  that merely reaches a terminal workflow state. Require the current exact test and
  fixture inventory, successful CTest/JUnit results and completed 10,000-case/seed
  observations before using a run to calibrate. The 390.08s isolation timeout in
  Nightly40157750598 and later cancelled attempts are censored/unaccepted, even
  where five other properties passed. Do not derive a deadline from timeout values,
  drop cases, shrink the pool or promote the lane. Any later deadline change uses
  complete comparable measurements under DEC.47 with its run identity.
- **D-13 (continued local unblock path):** Continue a dependency-free local
  diagnostic repair plan while preserving accepted 0/4 and all spent hosted
  allowances. At published source `8e5acda806a1d7c3551bb0ca8136254648151757`,
  the helper can emit three POST records per 100ms tick into a 64-record stream.
  In run 41461948345, INIT/FORK/START plus 60 repeated POST records filled the
  stream and emitted SATURATED by 2.934s; posts continued through 4.84s and the
  acknowledgment deadline fired at 5.014s. On that cutoff iteration the helper
  exits before its next `waitpid`, leaving child status unknown. Plan to aggregate
  repeated posts, preserve capacity for lifecycle/poll/state/end evidence, check
  child completion at the cutoff, and add representative pass/fault controls
  using the existing generated input-history fixture and pixel oracle. This
  explains the diagnostic gap; the actual input-failure cause remains unknown.
  The plan does not authorize another host/CI action; any later observation needs
  its own bounded scope.

### Agent's Discretion

Research/planning chooses exact input automation, fixture assertions, private helper
names and the measured local publication-control remedy within these contracts.
Use standard facilities and existing CMake `-P`/POSIX sh consumers; no new dependency
is selected. Each materially changed check names its concrete fault and exercises
representative pass/fault behavior. Apply ENGINEERING §11 to ownership, independent
oracles, change locality and recurring cost; retain the two-review-fix/one-gap limits.
The owner verifies nothing by default. Reopen decisions only for new evidence that
changes feasibility, safety, scope or acceptance; stop fan-out after this synthesis.

</decisions>

<canonical_refs>
## Canonical References

**Downstream agents MUST read the relevant sections before planning or implementing.**

### Scope, intent and settled contracts
- `.planning/README.md` and `.planning/INTENT.md` — owner-requested starting point.
- `.planning/PROJECT.md`, `.planning/REQUIREMENTS.md`, `.planning/ROADMAP.md` —
  current baseline, six requirements and fixed phase/external boundaries.
- `AGENTS.md` — ownership, verification, provenance and one-step/branch contracts.
- `.planning/research/fan-out/prompt.txt` — owner-requested breadth/depth,
  dependency ladder, evidence and diminishing-returns policy.
- `.planning/preparation/README.md` and `.planning/preparation/DECISIONS.md` —
  reference index and settled choices, particularly DEC.09/10/15/47/49/56/61.
- `.planning/preparation/ENGINEERING.md` §11–12 and
  `.planning/preparation/GSD-HANDOFF.md` §3–3.1 — proportional gates and bounded work.
- `.planning/LESSONS.md` — workflow recovery, complete-work timing and local onboarding;
  read MU-HOST-007 for the reproduced trace-capacity/deadline-reporting defect.
- At published source `8e5acda806a1d7c3551bb0ca8136254648151757`,
  `tests/retroarch/keyboard_preflight.c`, `tests/retroarch/test_input_trace_helper.c`,
  `tests/libretro/input_trace.c` and `tests/libretro/input_trace.h` — exact helper
  schedule, fixed trace limit, parser and mocked-host controls for this observation.
- `PROVENANCE.md` and `ASSET_POLICY.md` — permitted source and owned-fixture policy.

### Product, host and release consumers
- `.planning/research/SUMMARY.md` §Implications for Roadmap — accepted phase ordering.
- `.planning/research/FEATURES.md` — inspected Playstead code/product/brand provenance.
- `.planning/research/PLAYSTEAD-HANDOFF.md` — sibling obligations and missing paired proof.
- `.planning/preparation/HOSTS-AND-INTEGRATION.md` §1/3–4 — host/runner contracts;
  historical future-state claims must be reconciled with the current roadmap.
- `README.md` and `docs/libretro.md` — existing player path and launch contract owner.
- `docs/runner.md`, `docs/testing.md`, `docs/emulation-tests.md`,
  `docs/maintaining.md` — status/save semantics and existing test/release operations.
- `.github/workflows/ci.yml` and `.github/workflows/release.yml` — packaged artifacts,
  actual-host cases, install-line consumers and post-publish identity verification.

### Tune-up and timing evidence
- `.planning/seeds/SEED-007-milestone-tune-up.md`,
  `.planning/seeds/SEED-009-proportional-maintenance-review.md`,
  `.planning/seeds/SEED-010-nightly-prop-calibration.md` — recurring scope and evidence;
  retired quotas, old export instructions and pool-reduction suggestions do not
  supersede current owner contracts.
- `.planning/research/workflow-recovery/RECOMMENDATION.md` — accepted recovery controls.
- `.planning/milestones/v2.0-phases/08-controllers-at-the-pins/08-14-SUMMARY.md` —
  repeated publication-control cost and property acceptance limits.
- `.github/workflows/nightly.yml`, `tests/prop/tests.cmake`,
  `tests/inventory/ci.txt` — actual current six-property workload and advisory authority.

### Current status and input acceptance
- `.planning/STATE.md` — current phase halt and hosted-allowance boundary; recheck its live status before any later action.
- `.planning/phases/12-player-ready-delivery-and-playstead-bridge/12-VERIFICATION.md` — remaining PLAY-03 host truth and blocked dependent plans.
- `.planning/LESSONS.md` — exact pinned-binary finding for the unavailable test joypad and diagnostic evidence limits.

### External primary references reviewed
- [Libretro CLI guide](https://docs.libretro.com/guides/cli-intro/),
  [directory guide](https://docs.libretro.com/guides/change-directories/) and
  [logging guide](https://docs.libretro.com/guides/generating-retroarch-logs/) —
  documented launch/config/logging surface; exact pinned-binary behavior needs proof.
- [Upstream test-input-driver PR](https://github.com/libretro/RetroArch/pull/16370)
  — describes an optional driver absent from the inspected current pin; reconsider
  only if the host pin changes. No GPL source was inspected.
- [CTest TIMEOUT](https://cmake.org/cmake/help/latest/prop_test/TIMEOUT.html) and
  [GitHub workflow syntax](https://docs.github.com/en/actions/reference/workflows-and-actions/workflow-syntax)
  — timeout/cancellation semantics, not successful workload measurements.

</canonical_refs>

<code_context>
## Existing Code Insights

### Reusable Assets
- `tests/cmake/install_line.cmake` — runs the README line against a built/published
  archive; preserve it and the existing CI/release consumers.
- `tests/retroarch/run_retroarch.cmake`, `tests/retroarch/test.cfg.in`,
  `tests/retroarch/compare.c`, `tests/retroarch/bmp.c` — isolated actual host and
  expected-pixel/media judgments within the existing hosted bound.
- `tests/diag/battery.s`, `tests/diag/battery.a78`, `tests/diag/expected.csv` —
  admitted owned battery diagnostic and independent media/visible expectations.
- `tests/libretro/host.c` — real loaded-module input callbacks and semantic controls;
  reuse its expectations without relabeling it as RetroArch input delivery.
- `tests/input/test_input.c` — existing owned cartridge generator with visible bands
  reflecting input-register values; supplies independent expected pixels when
  loaded through the actual host, but generating or in-process execution alone
  does not prove host input delivery.
- `tests/policy/publish.cmake`, `scripts/publish.sh` — current production consumer
  and synthetic lifecycle/attestation fault rows; scope the tune-up here.
- `tests/prop/test_prop.c`, `tests/prop/tests.cmake`, `tests/cmake/check_junit.cmake`
  — seeded properties and exact completion judgment.

### Established Patterns
- Existing docs owners, CMake registrations, exact inventories and independent
  diagnostic oracles precede new infrastructure. Equal hashes and clean exits
  alone do not establish the required consumer behavior.
- Host filesystem/config/device responsibilities remain outside the core.
  Core, runner and RetroArch status/persistence contracts remain distinct.
- Required correctness, security, artifact identity, package measurements and
  armed performance guards survive consolidation; advisory labels survive retries.

### Integration Points
- Extend the present CI RetroArch cases and extracted archive handoff; use the
  release path for published-identity truths available at ship.
- Keep public integrator instructions in `docs/libretro.md` and cross-project
  status in `.planning/research/PLAYSTEAD-HANDOFF.md`.
- Publication test changes remain local to the demonstrated cost until measured
  evidence requires a wider boundary. Property changes start with accurate
  current-workload documentation and explicit evidence disposition.

</code_context>

<specifics>
## Specific Ideas

The owner asked to consider every relevant decision through appropriate specialist
roles, prefer primary sources, compare meaningful alternatives, perform an
adversarial synthesis and follow the coherent recommendations. Another small,
understood permissive copy can be preferable to another dependency; copied code
still needs provenance and maintenance. Preserve useful findings in local project
memory without exporting lessons or manufacturing new reproduced results. This
2026-11-02 follow-up clarifies halt scope and adds D-13 from the latest trace
evidence; all other settled decisions remain locked.

</specifics>

<deferred>
## Deferred Ideas

Portable states and frontend save/load/rewind/run-ahead remain Phases 13–14.
Native Playstead hosting, exact Continue/history, generalized continuation,
recording, AI/TAS and browser delivery retain their existing horizon triggers.
A machine-readable launch manifest needs a demonstrated consumer/drift problem.
Broad CI/DAG redesign, COST-01 repair and WR-01 repair are not inferred from this
tune-up; preserve known defects and recheck triggers.

### Reviewed Todos (not folded)
- `.planning/todos/pending/2026-10-16-close-the-re-review-findings-on-the-dependabot-gate-and-the-publish-checks.md`
  — retain historical individual dispositions; this tune-up must preserve current
  publisher controls without blanket-closing unrelated Dependabot work.
- `.planning/todos/pending/2026-10-16-promote-the-wasm32-lane-into-ci-required.md`
  — unrelated advisory promotion needs its own qualifying evidence.
- `.planning/todos/pending/2026-10-19-close-the-phase-2-re-review-findings-when-maria-and-the-run-loop-land.md`
  — preserve documented Phase 3 resolutions and remaining optional CPU/HALT seeds.
- `.planning/todos/pending/2026-10-19-close-the-phase-2-review-seeds-on-the-nightly-filter-and-the-rom-tables.md`
  — preserve resolved filter history and remaining ROM/tooling dispositions.
- `.planning/todos/pending/2026-10-29-keep-the-phase-11-first-review-info-findings.md`
  — optional compatibility wording/local-driver findings do not become requirements.

Existing required-contract violations remain blocking wherever discovered; a todo's
historical optional label cannot excuse a newly reproduced required defect.

</deferred>

---

*Phase: 12-player-ready-delivery-and-playstead-bridge*
*Context gathered: 2026-10-29; updated: 2026-11-02*
