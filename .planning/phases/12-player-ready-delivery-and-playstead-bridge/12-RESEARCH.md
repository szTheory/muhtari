# Phase 12: Player-ready delivery and Playstead bridge — Research

**Researched:** 2026-10-30  
**Domain:** Libretro/RetroArch packaging and host acceptance, release documentation, bounded CI maintenance  
**Confidence:** MEDIUM

<user_constraints>
## User Constraints (from CONTEXT.md)

### Locked Decisions

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
- **D-04 (PLAY-03):** Reuse the existing actual-RetroArch screenshot/diagnostic and
  fresh-process battery cases, their independent expected pixels/media and negative
  controls. Existing CI already extracts the candidate archives; retain that path
  and qualify the exact extracted libretro archive rather than substituting a
  direct build-tree module. Recheck the current host/core pins before implementation and record
  precisely which candidate or published release bytes each result establishes.
  PR candidate-archive evidence and published-download evidence are distinct;
  truths available only after publication are confirmed at ship using the existing
  release/install path. Never label a source-build-only result as released-core proof.
- **D-05 (PLAY-03):** Close the actual-host input gap: an owned diagnostic must
  visibly respond to delivered gameplay input, with an independent expected result
  that distinguishes the intended held/released input from missing or misrouted
  input. Reuse an existing hosted case and the in-process input tests. Loaded-module
  callback tests are supporting semantics evidence; they do not establish that
  RetroArch delivered input. Input-driver automation is a focused research question,
  not an owner task. Investigate RetroArch's JSON-driven `test` joypad driver first:
  its upstream PR describes press/release/axis events that can reach a loaded core,
  but availability and scheduling in the pinned release binary are unverified.
  Do not freeze an unproven movie/playback mechanism, OS permission
  setup or synthetic core-side input shortcut. Missing host input proof remains a
  PLAY-03 gap, rather than being silently deferred or equated with callback success.
- **D-06 (PLAY-03):** Keep the two actual-host cases, existing no-UDP approach,
  hosted-test line bound and timeouts. Put exhaustive semantics and fault cases in
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

### the agent's Discretion

Research/planning chooses exact input automation, fixture assertions, private helper
names and the measured local publication-control remedy within these contracts.
Use standard facilities and existing CMake `-P`/POSIX sh consumers; no new dependency
is selected. Each materially changed check names its concrete fault and exercises
representative pass/fault behavior. Apply ENGINEERING §11 to ownership, independent
oracles, change locality and recurring cost; retain the two-review-fix/one-gap limits.
The owner verifies nothing by default. Reopen decisions only for new evidence that
changes feasibility, safety, scope or acceptance; stop fan-out after this synthesis.

### Deferred Ideas (OUT OF SCOPE)

Portable states and frontend save/load/rewind/run-ahead remain Phases 13–14.
Native Playstead hosting, exact Continue/history, generalized continuation,
recording, AI/TAS and browser delivery retain their existing horizon triggers.
A machine-readable launch manifest needs a demonstrated consumer/drift problem.
Broad CI/DAG redesign, COST-01 repair and WR-01 repair are not inferred from this
tune-up; preserve known defects and recheck triggers.
</user_constraints>

<phase_requirements>
## Phase Requirements

| ID | Description | Research Support |
|----|-------------|------------------|
| TUNE-06 | Maintainers can ship a small improvement to demonstrated pipeline friction selected from the recurring tune-up seeds, with representative pass/fault evidence and preserved required contracts; unselected COST-01, WR-01 and optional debt keep explicit dispositions rather than becoming a general audit. | Profile `policy.publish.controls` on Windows, preserve all semantic rows, and require comparable current/base before/after plus pass/fault evidence. |
| TUNE-07 | Maintainers can distinguish complete successful nightly-property measurements from censored or cancelled work, and receive evidence-based deadlines or a bounded explicit deferral while every case/oracle and advisory status is preserved. | Preserve the six-property exact inventory, complete-cohort definition, advisory lane and explicit calibration deferral. |
| PLAY-01 | A gamer can find the current binaries, install the correct RetroArch core, load a local game through GUI or CLI, understand controls and optional BIOS, and find honest save/compatibility limits from a short current README whose install command is automatically checked. | Keep README install consumer; make only release-truth and battery/state wording corrections, with launch detail in `docs/libretro.md`. |
| PLAY-02 | A frontend integrator can consume a release-qualified external RetroArch/muhtari launch contract covering exact artifact identity, per-game content/save paths, optional firmware, usable input, process exit and refusal reporting, without treating the headless runner as an interactive player. | Existing CLI/config/logging docs and release identity/attestation path support this contract; keep host and runner exit semantics distinct. |
| PLAY-03 | A frontend integrator has automated owned-fixture evidence of visible output, input delivery and fresh-process battery continuity through the released libretro core and actual RetroArch; final Playstead launch/input/exit acceptance is separately identified and never inferred from these host-readiness checks. | Preserve extracted-archive screenshot and two-fresh-process battery host checks; first prove JSON test-joypad input in the pinned hosted host, then keep Playstead paired flow pending. |
| PLAY-04 | The Playstead team can use a source-linked handoff that identifies its adapter/pin/readiness/state responsibilities, the quickest playable route, a native-host north star and its promotion triggers, and the exact evidence still needed for the actual cross-project flow. | Refresh `.planning/research/PLAYSTEAD-HANDOFF.md` from current inspected revision/dirty-state evidence; keep sibling implementation and paired acceptance external. |
</phase_requirements>

## Summary

Use the repository’s existing release, RetroArch, fixture, and CTest paths. The current host CI already extracts the packaged candidate core, runs an isolated RetroArch screenshot comparison, and runs fresh-process battery checks; the guide also documents the real `-L` boundary and frontend-owned battery files. The unresolved implementation question is actual gameplay-input delivery. RetroArch’s upstream `test` joypad has a released-tag configuration and a documented event model, but the pinned binary did not complete a local probe, so hosted proof is still a gate. [VERIFIED: `.github/workflows/ci.yml:417-459`, `docs/libretro.md:16-35,223-228`]

The property lane has six registered tests, and its current advisory/censored status already has an explicit bounded disposition. The publisher test’s process-setup cost remains a hypothesis: local scratch timings for a proposed wrapper change were inconsistent, and they cannot stand in for the required Windows current/base measurement. [VERIFIED: `tests/prop/tests.cmake:18-34`, `.github/workflows/nightly.yml:175-212`; `12-CONTEXT.md:103-140`; local scratch runs described below]

**Primary recommendation:** Plan a short hosted feasibility task for the pinned RetroArch test joypad inside the existing two-case/300-line budget, using the owned input-sensing cartridge and independent pixel expectations. In parallel, plan Windows profiling and current/base acceptance for the existing publisher consumer before selecting a code remedy. Keep source-linked Playstead acceptance pending until paired evidence exists. [CITED: [RetroArch test-input PR](https://github.com/libretro/RetroArch/pull/16370); VERIFIED: `.github/workflows/ci.yml:403-459,524`, `tests/input/test_input.c:1-20`]

## Architectural Responsibility Map

| Capability | Primary Tier | Secondary Tier | Rationale |
|------------|-------------|----------------|-----------|
| Player installation and honest limits | README / user docs | Release workflow | README remains the short player entry; the existing install-line consumer checks the latest release. [VERIFIED: `README.md:3-47`, `.github/workflows/release.yml:264-277`] |
| Integrator launch and refusal contract | `docs/libretro.md` | RetroArch app and core artifacts | The external host owns the window, devices and process; the core owns load refusal and raw battery span. [VERIFIED: `docs/libretro.md:3-35,223-228`] |
| Packaged-core readiness | Hosted integration job | Existing diagnostic/runner oracles | The host case extracts candidate archives, then compares actual RetroArch pixels with independent runner output. [VERIFIED: `.github/workflows/ci.yml:417-458`] |
| Actual Playstead session | Playstead adapter/installer | Muhtari handoff docs | Playstead owns pins, readiness, input/focus and child lifecycle; its paired flow remains external. [VERIFIED: `12-CONTEXT.md:15-19,89-101`; `.planning/STATE.md:60-63`] |
| Publisher-control cost | Windows CI test consumer | `scripts/publish.sh` | Keep the remedy beside the measured test boundary and production publisher calls. [VERIFIED: `tests/policy/tests.cmake:152-161`, `tests/policy/publish.cmake:1-60,298-334`] |
| Property evidence and deadlines | Nightly workflow / CTest | JUnit inventory checker | The workflow owns seed/case execution; CTest registrations and inventory/JUnit checker own completeness. [VERIFIED: `tests/prop/tests.cmake:18-34`, `.github/workflows/nightly.yml:198-207`] |

## Standard Stack

### Core

| Component | Version | Purpose | Why standard |
|---|---|---|---|
| RetroArch released binary | 1.22.2 pinned Metal universal DMG | Actual macOS libretro host in the existing hosted job | Exact version and digest are already pinned; `pins.txt:37` says verbatim `retroarch-dmg 1.22.2 https://buildbot.libretro.com/stable/1.22.2/apple/osx/universal/RetroArch_Metal.dmg 81b79121ba26d539064ae13b4d0419a120c3d165afbe656cf5f5412b15fdb434`; the job verifies the app version before use. [VERIFIED: `pins.txt:35-37`, `.github/workflows/ci.yml:426-434`] |
| CMake script mode and CTest | Existing project version/presets | Isolated host runner, policy controls and test registration | The current consumers already use these mechanisms; AGENTS requires CMake `-P` or POSIX `sh` checks. [VERIFIED: `tests/retroarch/run_retroarch.cmake:1-66`, `tests/policy/tests.cmake:152-161`, `AGENTS.md:33-38`] |
| POSIX `sh` | Platform-provided | Production publisher invocation and synthetic command stubs | Existing publisher checks exercise the production script through this interface. [VERIFIED: `tests/policy/publish.cmake:1-60,280-334`] |

### Supporting

| Component | Purpose | When to use |
|---|---|---|
| Existing owned diagnostics and runner | Independent visible/output and battery expectations | Retain for host comparisons; the input fixture already generates an owned input-sensing cartridge. [VERIFIED: `tests/input/test_input.c:1-20`, `.github/workflows/ci.yml:435-458`] |
| RetroArch `test` joypad driver | Schedule JSON test input through the host | First input-automation candidate; do not claim acceptance until it drives the pinned binary and expected pixels distinguish the intended press/release sequence. [CITED: [upstream PR #16370](https://github.com/libretro/RetroArch/pull/16370); [v1.22.2 test configuration](https://raw.githubusercontent.com/libretro/RetroArch/v1.22.2/tests-other/testinput.cfg)] |

### Alternatives Considered

| Instead of | Could use | Tradeoff |
|---|---|---|
| RetroArch JSON test joypad | Manual keyboard/controller input, movie playback, OS-level event injection, or core-side synthetic input | These do not satisfy the locked first-choice seam without new feasibility evidence; do not add an unproven dependency or host framework. [VERIFIED: `12-CONTEXT.md:69-88`] |
| Existing two host cases | Third host case or a larger harness | Locked budget is two cases and 300 lines; resolve within those limits or return concrete ENGINEERING §11 conflict evidence. [VERIFIED: `.planning/preparation/DECISIONS.md:95-97`, `.github/workflows/ci.yml:524`] |

**Installation:** None. No external package or dependency is selected. [VERIFIED: `12-CONTEXT.md:142-149`]

## Architecture Patterns

### Host input proof within existing integration

1. Generate/use the owned input-sensing cartridge; it reads SWCHA, SWCHB and INPT0–INPT5 and draws their observed values in visible bands. Use the existing generator to create the cartridge as a CI/scratch artifact; do not commit ROM bytes. [VERIFIED: `tests/input/test_input.c:1-20`, `AGENTS.md:17-19`]
2. Configure the candidate test joypad in the existing isolated RetroArch config and provide press/release input events. The upstream v1.22.2 sample contains the exact assignments `input_joypad_driver = "test"` and `test_input_file_joypad = "tests-other/test_input_joypad.ratst"`. [CITED: [v1.22.2 test configuration](https://raw.githubusercontent.com/libretro/RetroArch/v1.22.2/tests-other/testinput.cfg); [upstream PR #16370](https://github.com/libretro/RetroArch/pull/16370)]
3. Make the expected output discriminate the intended held and released observations from no input or a wrong mapping; compare host pixels against an independent runner/fixture expectation. A successful `retro_*` callback or clean host exit is supporting evidence only. [VERIFIED: `12-CONTEXT.md:69-80`, `.github/workflows/ci.yml:441-458`]
4. Keep this as an extension of an existing host case and keep the DMA screenshot and battery roundtrip oracles intact. Existing config/run helpers isolate HOME, config, save, info and core paths and fail if RetroArch exits unsuccessfully or produces no screenshot. [VERIFIED: `tests/retroarch/run_retroarch.cmake:30-66`, `tests/retroarch/test.cfg.in:1-29`]

This is an implementation-ready probe design, not proof that the driver is available/scheduled in the pinned executable: the local host probes aborted before producing RetroArch logs/screenshots, and the pinned DMG mount attempt failed in this environment. The official input-driver docs distinguish input and controller drivers; the pinned hosted macOS job is the acceptance surface. [CITED: [RetroArch input/controller driver guide](https://docs.libretro.com/guides/input-controller-drivers/); VERIFIED: bounded local probe notes, N/A-local]

### Release-qualified external launch contract

Document an explicit local config, content, per-game save and optional firmware directory, with the separately verified RetroArch app and muhtari core identity, digest, architecture and extracted paths. The existing guide already documents quoted `retroarch -L CORE GAME`, `--verbose --log-file`, and the distinction that the core exposes raw save RAM while the frontend owns `.srm` I/O. The release workflow publishes/attests assets and verifies the latest install line after publication, so candidate-host and published-artifact claims must remain distinct. [VERIFIED: `docs/libretro.md:16-35,223-228`; `.github/workflows/release.yml:239-277`; CITED: [CLI guide](https://docs.libretro.com/guides/cli-intro/), [logging guide](https://docs.libretro.com/guides/generating-retroarch-logs/), [directory guide](https://docs.libretro.com/guides/change-directories/)]

### Bounded publisher measurement

Profile only `policy.publish.controls` on Windows first. Preserve all current synthetic lifecycle, attestation and workflow-output cases, and compare complete current/base work on the same runner class with the same source and workload identities. Record representative success and fault paths. Select a smallest remedy only after the profile identifies a repeated cost; then repeat the paired comparison. [VERIFIED: `12-CONTEXT.md:103-121`, `tests/policy/tests.cmake:152-161`, `tests/policy/publish.cmake:1-60,280-334`; DEC.47 at `.planning/preparation/DECISIONS.md:80-84`]

Local scratch probe: the existing publish script passed (`exit 0`) at 36.90s cold and 23.64s/22.90s warm on this macOS host. A scratch-only direct-shell candidate also passed, but its warm results (25.94s/21.55s) were inconsistent relative to baseline. Commands used `cmake -DROOT="$PWD" -DWORK=/tmp/muhtari-phase12-publish-baseline -P tests/policy/publish.cmake` and an equivalent scratch candidate path, both timed with `/usr/bin/time -p`; run URL: N/A-local. These results do not select the wrapper change or establish Windows benefit. [VERIFIED: bounded local scratch runs; candidate file was under `/tmp`, not a product edit]

### Property-work disposition

The exact registered properties are `split_run`, `peek_pure`, `instances_isolated`, `input_at_vblank`, `battery_lifecycle`, and `battery_observers` (quoted from the two source `foreach` lists). The workflow settings read `MUHTARI_PROP_CASES: 10000`, `MUHTARI_PROP_SEED: ${{ github.run_id }}`, `timeout-minutes: 9`, and `continue-on-error: true`; the report needs list omits `prop`. Each registration says `TIMEOUT 39`, with the nightly scale giving 390 seconds per test. Preserve these values, exact JUnit coverage, and the complete-or-censored rule from D-11/D-12. [VERIFIED: `tests/prop/tests.cmake:23-34`, `.github/workflows/nightly.yml:175-212`, `tests/inventory/ci.txt:566`, `12-CONTEXT.md:123-140`]

## Don't Hand-Roll

| Problem | Don't build | Use instead | Why |
|---|---|---|---|
| RetroArch app/core installation and verification | New downloader, launcher, install schema, or second release verifier | Existing pin/fetch, release/attestation and README install-line consumers | They already establish artifact identity and latest-install behavior. [VERIFIED: `.github/workflows/ci.yml:426-434`, `.github/workflows/release.yml:239-277`] |
| Host input automation | New generic input framework or unproven movie/OS-injection path | First probe the pinned RetroArch test joypad with the owned fixture | It matches D-05’s preferred seam and requires no dependency; runtime proof is still required. [CITED: [PR #16370](https://github.com/libretro/RetroArch/pull/16370); VERIFIED: `12-CONTEXT.md:69-88`] |
| Host screenshot and battery expectations | A host callback as oracle or hashes/clean exit alone | Existing independent pixels/media and fresh-process battery checks | Existing tests explicitly judge visible output and each saved byte against independent expectations. [VERIFIED: `.github/workflows/ci.yml:448-458,488-520`] |
| Property deadline calibration | Derive limits from timeout values, partial passes or cancelled runs | Explicit deferral until exact successful full cohorts are available | A terminal workflow/job status alone is not completed property evidence. [VERIFIED: `12-CONTEXT.md:123-140`, `.planning/STATE.md:62-64`] |

## Common Pitfalls

1. **Treating callbacks as host input.** `tests/input/test_input.c` currently exercises the runner/movie input path, while loaded-module callback coverage is not actual RetroArch delivery. Require the visible owned-fixture result from the actual pinned host. [VERIFIED: `tests/input/test_input.c:13-20`, `12-CONTEXT.md:69-80`]
2. **Breaking the host budget.** The current test files total exactly 300 lines, and CI enforces `<= 300`; there is no line headroom. Preserve two cases and timeouts; if the proof cannot fit, gather concrete boundary evidence before any §11 proposal. [VERIFIED: `wc -l tests/retroarch/*` → `300 total`; `.github/workflows/ci.yml:524`; `.planning/preparation/ENGINEERING.md:257-275`]
3. **Claiming released-core proof from the wrong bytes.** CI extracts the candidate archives; direct build-tree core tests do not prove packaged archive behavior, and candidate evidence is not post-publication evidence. [VERIFIED: `.github/workflows/ci.yml:417-434`, `12-CONTEXT.md:60-68`]
4. **Using RetroArch exit status as a content/save oracle.** The helper requires exit 0 and a screenshot, but D-03 correctly warns that exit 0 alone proves neither successful content loading nor save flushing. Keep independent output and fresh-process battery checks. [VERIFIED: `tests/retroarch/run_retroarch.cmake:50-66`, `12-CONTEXT.md:47-56`]
5. **Treating incomplete property work as measurement.** The six properties are not the four stated in the stale nightly comment; `390.08s` timeout/cancelled samples are censored. Preserve inventory/JUnit and advisory status; correct the stale count without inferring a new deadline. [VERIFIED: `tests/prop/tests.cmake:18-34`, `.github/workflows/nightly.yml:175-207`, `12-CONTEXT.md:123-140`]
6. **Picking a publisher optimization from source shape alone.** Local macOS scratch runs did not prove a repeatable benefit and are not a Windows current/base comparison; do not infer a wrapper, cache, DAG or dependency change from them. [VERIFIED: bounded local scratch runs; `12-CONTEXT.md:103-121`]
7. **Leaving a stale user-facing claim.** `docs/libretro.md` still says `unsupported board: bankset`, while the adapter has the exact conditions `cart.board == MUHTARI_BOARD_BANKSET && rom_bytes == 53248u && !xm` and `cart.board == MUHTARI_BOARD_BANKSET && rom_bytes <= 53248u`. Update only the stale example based on current loader support, not by broadening compatibility claims. [VERIFIED: `docs/libretro.md:3-6`; `libretro/libretro.c:1426-1427,1442-1446,1465-1475`]

## Code Examples

The pinned RetroArch release’s official test configuration uses these exact settings (they are configuration data, not an endorsement of an unproven execution result): [CITED: [v1.22.2 testinput.cfg](https://raw.githubusercontent.com/libretro/RetroArch/v1.22.2/tests-other/testinput.cfg)]

```ini
input_joypad_driver = "test"
test_input_file_joypad = "tests-other/test_input_joypad.ratst"
```

Use the project’s explicit local config and paths for host launch, following the official CLI and logging surface: [CITED: [CLI guide](https://docs.libretro.com/guides/cli-intro/), [logging guide](https://docs.libretro.com/guides/generating-retroarch-logs/)]

```sh
retroarch --verbose --config "$CONFIG" -L "$CORE" "$CONTENT"
```

This command shape and explicit config are documented; the test-input driver still needs pinned-host proof. [CITED: [CLI guide](https://docs.libretro.com/guides/cli-intro/); VERIFIED: `12-CONTEXT.md:69-80`]

## Project Constraints (from AGENTS.md)

- Reuse permissively and never open GPL, LGPL or unlicensed emulator source; use external emulators only as released binaries. [VERIFIED: `AGENTS.md:13-15`]
- Keep the three deliverables—library, runner and libretro core—and add no device/window code. [VERIFIED: `AGENTS.md:16`]
- Preserve deterministic, integer-only, bounded core behavior and per-instance state. [VERIFIED: `AGENTS.md:20-22`]
- Land behavior changes with CI tests and documentation; keep README short and maintainer/test documentation in `docs/`. [VERIFIED: `AGENTS.md:23-25`]
- Verification is automation-first; do not make the owner perform routine checks. Use CMake `-P`/POSIX `sh`, name representative pass/fault cases, preserve contracts, follow ENGINEERING §11, and keep review/gap rounds bounded. [VERIFIED: `AGENTS.md:26-40`]
- Keep one GSD step per session, do not chain steps, use the required branch/PR flow, and record evidence as command + exit + run URL rather than logs. [VERIFIED: `AGENTS.md:41-54`]
- Read relevant lessons before planning; treat transferred lessons as hypotheses until reproduced. [VERIFIED: `AGENTS.md:52-54`]
- Keep ROM/BIOS/save bytes out of the tree except manifest-listed fixtures, and do not add personal paths or identities. [VERIFIED: `AGENTS.md:17-19`]

## State of the Art

| Old approach | Current project approach | Impact |
|---|---|---|
| Manual host play or module callback treated as input proof | A pinned-host scheduled test joypad plus visible owned-fixture and independent expected pixels | Existing callbacks support semantics but cannot establish RetroArch delivery; pinned-host execution is still an open gate. [VERIFIED: `12-CONTEXT.md:69-80`; CITED: [PR #16370](https://github.com/libretro/RetroArch/pull/16370)] |
| Source-build test treated as release test | Extract candidate archives in host CI; verify published identity at ship | Keeps candidate, published and Playstead paired claims separate. [VERIFIED: `.github/workflows/ci.yml:417-434`, `12-CONTEXT.md:60-68`] |
| Calibrate deadlines from timeout/cancelled outcomes | Defer calibration until full exact successful cohorts are available | Preserves every property, oracle and advisory disposition. [VERIFIED: `12-CONTEXT.md:123-140`] |

## Assumptions Log

| # | Claim | Section | Risk if wrong |
|---|---|---|---|
| A1 | The JSON test-joypad schedule will execute in the pinned RetroArch 1.22.2 binary and deliver events to this core. This is unverified; the plan must gate on hosted proof. | Host input proof | PLAY-03 remains unmet if the seam is unavailable or does not affect the core. |
| A2 | The additional input observation can be integrated without exceeding the existing host-case line/time bounds. This must be demonstrated before the plan freezes the implementation. | Host input proof | Existing accepted screenshot/battery contracts or test budget could be disturbed. |
| A3 | Repeated shell/process setup is a material cause of Windows publisher-control cost. This is the locked profiling hypothesis, not an established cause. | Bounded publisher measurement | A premature code change may not improve the Windows workload. |
| A4 | A single small publisher remedy will yield a comparable measurable improvement while keeping every fault row. No remedy is selected until current/base profiling confirms it. | Bounded publisher measurement | TUNE-06 cannot be completed on source-shape or documentation changes alone. |

## Open Questions

1. **Does `test` joypad input work in the pinned binary?** The upstream PR/config shows the intended test path, but local probes aborted before producing logs/screenshots and the pinned DMG could not be mounted here. Plan a finite hosted proof in the current macOS job; if it fails, retain PLAY-03 as a gap and report the concrete failure before selecting another seam. [CITED: [PR #16370](https://github.com/libretro/RetroArch/pull/16370); bounded local probe notes]
2. **Can press and release both be made independently visible in the current host budget?** Start with the existing input-sensing cartridge, a configured schedule, and independent runner/pixel expectation; keep all DMA and battery checks. The host test directory has no line headroom, so implementation must consolidate within its current total. [VERIFIED: `tests/input/test_input.c:1-20`, `.github/workflows/ci.yml:441-524`; local `wc` result]
3. **Which publisher change is actually cheapest on Windows?** Profile complete current/base executions and representative pass/fault rows on the same runner class. Local macOS timings do not decide it; if process overhead is falsified, select another narrow reproduced friction source. [VERIFIED: `12-CONTEXT.md:103-121`; local scratch runs]
4. **What current Playstead revision and dirty source state should the handoff name?** The existing handoff itself says its earlier inspection was at `3e12f16a75ec` with relevant uncommitted changes; refresh from a current read-only inspection in the phase, and keep paired launch/input/exit/battery evidence pending. [VERIFIED: `.planning/research/PLAYSTEAD-HANDOFF.md:37-50`]

## Environment Availability

| Dependency | Required by | Available here | Version/status | Fallback |
|---|---|---|---|---|
| RetroArch | Actual host proof | Local app present, but sandbox runs aborted before logs/screenshots | Local app reports `1.22.2`; pinned DMG was fetched/checksummed but mount failed with `Périphérique non configuré` | Existing pinned macOS hosted job; no owner UAT by default. [VERIFIED: bounded local probe notes; `.github/workflows/ci.yml:426-434`] |
| CMake / CTest | Host helper and policy test | Available and used by bounded scratch checks | Project-supported scripts and test registration | Existing project mechanisms. [VERIFIED: `tests/retroarch/run_retroarch.cmake:1-66`, `tests/policy/tests.cmake:152-161`] |
| Windows CI runner | Publisher before/after measurement | Not available in this local macOS environment | Required evidence must come from same-class Windows current/base runs | Keep the measurement task on hosted Windows CI. [VERIFIED: `12-CONTEXT.md:105-116`] |
| Playstead repository/runtime | Paired integration acceptance | External sibling-owned dependency; no mutation is in scope | Handoff’s old inspected revision is explicitly stale/dirty | Maintain pending paired evidence in the handoff. [VERIFIED: `.planning/research/PLAYSTEAD-HANDOFF.md:37-50`, `12-CONTEXT.md:89-95`] |

## Security Domain

`security_enforcement` is explicitly `false` in project config, so no ASVS mapping is required for this phase. The operational security constraints still apply: verify app/core separately, use exact pins and local config/data paths, preserve refusal and unsafe-path controls, and do not open prohibited emulator source. The configuration’s exact setting is `"security_enforcement": false`. [VERIFIED: `.planning/config.json:60`; `12-CONTEXT.md:41-68`; `AGENTS.md:13-19`]

The validation architecture section is omitted because config contains the exact setting `"nyquist_validation": false`. [VERIFIED: `.planning/config.json:36`]

## Sources

### Primary (HIGH confidence)

- `AGENTS.md:13-54` — reuse, deliverable, verification, branch and evidence rules.
- `.planning/phases/12-player-ready-delivery-and-playstead-bridge/12-CONTEXT.md:33-151,264-272` — locked decisions, discretion and deferrals.
- `.planning/REQUIREMENTS.md:15-20` — six phase requirements.
- `.planning/preparation/README.md:8-30` — index used before opening subsystem preparation sources.
- `.planning/preparation/ENGINEERING.md:242-290` — docs, proportional check, cost and dependency rules.
- `.planning/preparation/DECISIONS.md:80-97` — release/host decisions including DEC.47 and DEC.56.
- `.planning/preparation/GSD-HANDOFF.md:114-143` — bounded recovery, evidence and stop rules.
- `.planning/LESSONS.md:119-122,143-157,160-164,192-202` — timeout censoring, Windows publisher profiling, paired comparison, current/base runner class and onboarding evidence.
- `.github/workflows/ci.yml:403-524`, `tests/retroarch/run_retroarch.cmake:1-66`, `tests/retroarch/test.cfg.in:1-29` — actual host cases, extraction, isolation, pixel and battery checks, line cap.
- `tests/input/test_input.c:1-20`, `tests/prop/tests.cmake:18-34`, `.github/workflows/nightly.yml:175-212`, `tests/policy/publish.cmake:1-60,280-334` — owned input fixture, exact property set, advisory lane, publisher test behavior.
- `docs/libretro.md:3-35,223-265`, `.github/workflows/release.yml:239-277`, `pins.txt:35-37`, `.planning/research/PLAYSTEAD-HANDOFF.md:18-58` — current launch/save docs, artifact pins/release checks and external handoff.

### Official external sources (CITED)

- [RetroArch CLI guide](https://docs.libretro.com/guides/cli-intro/) — `-L`, explicit config and command-line launch.
- [RetroArch input/controller driver guide](https://docs.libretro.com/guides/input-controller-drivers/) — input vs controller driver concepts.
- [RetroArch log guide](https://docs.libretro.com/guides/generating-retroarch-logs/) and [directory guide](https://docs.libretro.com/guides/change-directories/) — diagnostic logs and local directory ownership.
- [RetroArch upstream test-input joypad PR #16370](https://github.com/libretro/RetroArch/pull/16370) and [v1.22.2 testinput.cfg](https://raw.githubusercontent.com/libretro/RetroArch/v1.22.2/tests-other/testinput.cfg) — candidate scheduled test driver and config names. No RetroArch emulator source was opened.

## Metadata

**Confidence breakdown:**
- Standard stack: HIGH — existing consumers and pin were read in the repository.
- Architecture: HIGH — repository docs/workflows assign ownership clearly; external Playstead paired behavior remains pending.
- Pitfalls: MEDIUM — host-input runtime could not be confirmed locally, and no Windows current/base measurement was performed during this bounded research.

**Research date:** 2026-10-30  
**Valid until:** 2026-11-29 for stable repository contracts; recheck the pinned host and Playstead source when implementation starts.
