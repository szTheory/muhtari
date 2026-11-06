# Phase 12: Player-ready delivery and Playstead bridge - Discussion Log

> **Audit trail only.** Planning, research and execution consume `12-CONTEXT.md`.
> This file preserves alternatives and the limits of the discussion evidence.

**Date:** 2026-10-29
**Areas discussed:** Player documentation/launch contract; packaged-core actual-host
readiness and sibling handoff; publication-control overhead; property evidence.

## Owner direction and interaction

The owner invoked `$gsd-discuss-phase 12`, then required `.planning/README.md`
and `.planning/INTENT.md` first. Both were read before other project documents.
The owner supplied the fan-out brief: consider all relevant points through useful
specialist lenses, prefer reputable primary sources, compare alternatives,
perform an adversarial pass, stop at diminishing returns, and automatically follow
the coherent recommendations. This explicitly delegated area selection and routine
decisions; no artificial question-by-question interview or further approval was
needed. No auto/chain flags were used, and no downstream GSD step was started.

Three typed `gsd-advisor-researcher` agents reviewed disjoint areas read-only using
the project-resolved model/effort. The orchestrator owns shared artifacts/state.
The existing milestone commits and unrelated working changes were preserved on
the new phase branch. No Playstead files, source code or workflows were changed.

## Player onboarding and launch contract

| Option | Benefit and tradeoff | Selected |
|---|---|---|
| Keep current docs entirely unchanged | Already covers most player tasks; leaves the integrator contract incomplete | |
| Preserve README and add focused guide contract | Reuses the install check and existing docs owner; still requires exact-pin validation | Yes |
| New machine-readable launch manifest/consumer | Could prevent drift across many hosts; adds schema ownership/coupling for one consumer | |

**Decision:** D-01–03. Preserve the useful player path and close residual truth gaps
in the libretro guide. Separate moving player download links from immutable adapter
pins. Cover config/save/firmware/input/exit/refusal boundaries without inventing
RetroArch-to-runner status equivalence.

**Evidence/confidence:** High for current repository coverage from README,
`docs/libretro.md`, the install-line test and CI/release callers. Official CLI,
directory and logging guides support documented argument/config concepts; exact
current binary behavior remains execution work. The stale unsupported-BankSet
example is a documentation correction to check against current refusals.

**Revisit:** A second real consumer or reproduced contract drift could justify a
shared machine-readable surface. It would still not establish real input/focus.

## Actual-host readiness and external Playstead flow

| Option | Benefit and tradeoff | Selected |
|---|---|---|
| Relabel existing callback/direct-build tests as complete readiness | Cheap but leaves actual-host input and release identity unproved | |
| Qualify extracted artifacts and extend an existing host case with input proof | Establishes the real consumer boundary with bounded existing mechanisms; input seam needs focused research | Yes |
| Add general GUI/network/physical-device automation | Broader interaction surface but new permissions, nondeterminism and maintenance; exceeds the present justification | |

**Decision:** D-04–08. Preserve two actual-host cases and independent pixel/media
oracles. Require an input-dependent expected visible result through RetroArch,
and exact packaged-core provenance. Leave the particular automation seam to
research; absence of a demonstrated seam cannot erase PLAY-03. Current source-build
host success, PR archive success, published-download success and actual paired
Playstead success are distinct evidence classes.

**Evidence/confidence:** High for the coverage gap: current CI extracts candidate
archives and exercises pixels and two-process battery data; retain this existing
package consumer rather than inventing an archive-path gap. Loaded-module tests drive input callbacks but the
RetroArch cases drive no gameplay input. No new runtime result is claimed.
The advisor found that Remote RetroPad/network-control documentation did not
establish an automated arbitrary-input seam suitable for these cases. No such
mechanism was selected from those alternatives. The advisor identified
[upstream PR16370](https://github.com/libretro/RetroArch/pull/16370), whose description
offers a JSON-driven `test` joypad driver; investigate it first alongside the existing
visible input-register cartridge generator in `tests/input/test_input.c`.
Pinned-binary driver availability, event scheduling and oracle alignment are
unverified. No GPL source was opened. The parent's local released RetroArch `--help` attempt
exited134 in the sandbox without output, consistent with the recorded hosted-only
limitation; it establishes neither help contents nor a launch/input result.

**Revisit/blocker:** Research must establish the input seam against the exact
host. A demonstrated conflict with existing resource bounds gets a scoped
ENGINEERING §11 review. Do not move this proof to owner UAT or smuggle in Phase13
serialization to support movie playback. Paired Playstead acceptance remains
pending until the sibling consumer proves it with exact sources/pins.

## Measured small tune-up

| Option | Benefit and tradeoff | Selected |
|---|---|---|
| Record debt dispositions only | Cheap and honest, but does not ship the improvement TUNE-06 requires | |
| Measure and remove confirmed local publication-test overhead | Targets repeated friction while retaining production/fault coverage; causal cost still needs measurement | Yes |
| Redesign CI DAG, base execution or Windows matrix | Potentially wider savings, but risks required baseline/Windows coverage before cause is known | |

**Decision:** D-09–10. `policy.publish.controls` is the sole primary, with no
promised speedup or global-cost claim. Preserve all cases and representative
Windows behavior, compare completed same-runner-class work, and implement the
smallest supported remedy. If profiling disproves the candidate, require new
reproduced evidence before selecting another; paperwork is not a fallback pass.

**Historical evidence:**
- [CI40135571745](https://github.com/szTheory/muhtari/actions/runs/40135571745)
  and SEED-009 record Windows ARM current/base756.47/744.81s.
- [CI40157749294](https://github.com/szTheory/muhtari/actions/runs/40157749294)
  and 08-14 SUMMARY record729.04/703.82s on corrected source.

These repeated durations establish a target, not its present cause or an achieved
improvement. Repeated shell/publisher startup is inferred from the current
consumer. Before/after acceptance remains implementation work.

## Property calibration and evidence disposition

| Option | Benefit and tradeoff | Selected |
|---|---|---|
| Increase limits using timeouts/cancellations | Might finish more work, but censored durations cannot calibrate complete work | |
| Immediately pursue a broad property-performance project | Could produce calibration evidence, but expands this small tune-up | |
| Explicit bounded deferral with accurate current-workload docs | Preserves cases/oracles and honest evidence; calibration stays unresolved | Yes |

**Decision:** D-11–12. Correct four-property wording to the current six and retain
10,000 cases, seeds, inventory, current deadlines/cap and advisory authority.
Recheck at Phase12 close and the next milestone tune-up, or earlier upon new
complete evidence. Successful CTest/JUnit/current-workload completion is necessary
before using a cohort to change deadlines under DEC.47.

**Evidence/confidence:** High for current six registrations and inventory from
`tests/prop/tests.cmake` and `tests/inventory/ci.txt`. Historical
[Nightly40157750598](https://github.com/szTheory/muhtari/actions/runs/40157750598)
has five completed properties and isolation censored at390.08s with CTest8;
SEED-010 carries later cancelled attempts. No complete current six-property
calibration cohort or advisory promotion is established here.

## Adversarial synthesis and conflict resolution

- A README rewrite would consume effort without addressing the residual launch
  contract: retain the current flow and target actual gaps.
- A clean exit and equal runner/host hashes can share omissions: keep independent
  expected pixels/media and an input-dependent visible oracle.
- An arbitrary input injection shortcut could bypass the very RetroArch boundary
  being claimed: require actual host delivery; select no unproven seam in discuss.
- Current CI already tests extracted candidate archives. Preserve that consumer,
  bind artifacts to the evidence stage and collect post-publication-only truth at
  ship; do not replace it with direct-build tests or assume candidate publication.
- Real host readiness cannot establish Playstead installation/focus/lifecycle.
  Keep the external dependency visible without blocking independent state work.
- Removing publication fault rows could buy time by losing required evidence.
  Retain them and optimize only measured redundant process/setup work.
- Terminal jobs, partial successful properties and cancellations are not complete
  calibration samples. Preserve original failures, cases and advisory labels.
- SEED-010's historical pool-reduction option conflicts with the current accepted
  preserve-all-cases/oracles boundary. The current requirements take precedence.
- SEED-007's retired quota and lesson-export text does not revive old policy:
  current DEC.10/61 and owner-directed local lesson memory govern.

No remaining discussion uncertainty calls for an owner preference. Input seam
feasibility and publication cost attribution are focused downstream research,
not invitations for unlimited fan-out. No new hardware or integration lesson was
reproduced; no LESSONS entry or horizon refresh was manufactured.

## Agent's Discretion

Exact internal seams, assertion organization, local harness remedy and documentation
wording are delegated within CONTEXT. No new dependency, general audit, manual owner
verification, phase plan or implementation was authorized by this discussion step.

## Deferred Ideas

Retain the five inherited todo dispositions listed in CONTEXT, along with
COST-01, Phase5 WR-01 and optional maintenance seeds. State implementation and
frontend state consumers retain Phases13–14. Native Playstead hosting/Continue,
new launch schema, generalized continuation, recording/AI/TAS and browser work
retain their existing later triggers.

## Discussion verification

`python3 -` with the session's inline artifact assertions exited0: the six required
sections, 12 unique decisions, all six requirement mappings, 49 existing local
references and absence of phase plans were verified. `git diff --check`,
`sh scripts/hygiene.sh --tree` and
`cmake -DROOT="$PWD" -P tests/cmake/budgets.cmake` exited0; the budget check covered
22 files. These are local documentation/workflow checks, not product acceptance
or hosted tests. Run URL: N/A-local. No execution logs are committed.


---

## Update — 2026-11-01: PLAY-03 input proof

| Option | Description | Selected |
|--------|-------------|----------|
| Keep investigating the test joypad for the exact pinned RetroArch binary | The upstream PR describes the driver, but inspection of the shipped pinned binary found no test-input keys or test joypad. | |
| Retire that route for the current pin; keep independent held/released visible output as the acceptance bar | Removes stale guidance while preserving the actual product proof required by PLAY-03. | ✓ |
| Select a different input route or authorize another hosted run in this context update | No replacement seam is proven, and all hosted allowances are spent. A new bounded scope and explicit authority would be required. | |

**User's choice:** Update the existing Phase 12 context. The previously recorded owner direction to consider all relevant areas and adopt coherent recommendations applies to this evidence-driven refresh.

**Notes:** Three read-only advisor reviews examined host acceptance, diagnostic proof limits, and CI/release cost. All recommended updating D-05 in place, retaining the held/released pixel criterion, and preserving the halt. Run 41323089043 recorded 34 F8-down posts, zero captures, and unknown child status; run 41329055050 skipped RetroArch after the Windows ARM64 OpenBIOS timeout and produced no input observation. No test, build, hosted action, or RetroArch run was performed in this discussion.

## the agent's Discretion

Update D-05 in place with an explicit date and evidence. Leave every other locked decision unchanged. No new hosted scope or implementation authority follows from this context update.

## Deferred Ideas

None added; existing deferred items remain unchanged.

## Owner correction and trace finding — 2026-11-02

The owner clarified that preserving an exhausted host allowance must not stop
useful local work. The helper emitted up to three repeated POST records per
100ms tick; run 41461948345 filled its 64-record stream with 60 POST records by
2.934s, then continued posting through 4.84s. At the 5.014s acknowledgment cutoff,
the helper exits before its next waitpid, so child status is unknown. This
identifies a diagnostic observability defect; the actual input cause remains
unknown. Record MU-HOST-007 and plan a dependency-free local repair with
pass/fault controls. No new host/CI run is authorized.

---

## Follow-up owner authorization and local outcome

After the context discussion, the owner separately authorized a bounded local-only
scope in this writable Phase 13 checkout: add an owned generated input-history
cartridge, an independent rendered-pixel oracle, and pass/fault controls; run bounded
local verification. The implementation was uncommitted in the Phase 13 worktree at the time of that run; it has since been carried into the reconciled root checkout for verification.
This later authorization did not cover RetroArch wiring, CI/remote actions,
publication, retry, cancellation, timeout changes, hosted allowance renewal or
Phase 12 acceptance. The trace allowance remains spent; run 41461948345 must not be
repeated.

The fixture checks waiting, exact P1 Up/button-1 held, then a release in a later
sampled frame with persistent completion. Fault controls cover missing hold, wrong
port, wrong button, missing release and release-before-hold. Its pixel oracle
compares RGB output independently from the cartridge's indexed palette values.

Verification on an isolated source snapshot:

| Command | Exit | Result |
|---|---:|---|
| `cmake -S src -B build -G Ninja -DMUHTARI_INVENTORY=local -DMUHTARI_BUILD_LIBRETRO=OFF` | 0 | Configured the bounded input-only inventory |
| `cmake --build build --target muhtari-test-input --parallel 4` | 0 | Built the input test runner |
| `ctest --test-dir build -L input --output-on-failure` | 0 | All 17 input tests passed, including generated-cart fixture setup |

The direct worktree build command `cmake --build build/ci --target
muhtari-test-input --parallel 4` could not regenerate CMake because the pinned
Wasmtime, 7800basic and OpenBIOS packages are absent in this checkout; no package
fetch was attempted. The isolated snapshot run establishes the local fixture and
oracle behavior only; it is not actual pinned-host input evidence and leaves the
Phase 12 halt unchanged.

## Root-checkout integration — 2026-11-05

The authorized generated input-history refinement was carried into the root
checkout. Its CTest fixture is consumed by `input.proline_one_button`, and the
serialized-image check remains registered. Root-local configure and focused
build passed; seven selected input, battery, frontend-state-control and policy
tests passed. No RetroArch process, full CI workflow, remote action or host
acceptance ran. Phase 12 remains accepted 0/4; spent hosted allowances and the
12-01 halt are unchanged. Command/exit evidence is recorded in `12-10-SUMMARY.md`.
