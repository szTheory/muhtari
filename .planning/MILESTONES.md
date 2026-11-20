# Project Milestones: muhtari

## v12.0 First-use readiness (Shipped: 2026-11-20)

**Phases completed:** 2 phases, 3 plans, 8 tasks

**Closeout type:** `override_closeout`; all five requirements and both phase verification reports passed. Known verification overrides: 1 newly acknowledged, 51 carried forward from prior closes (see STATE.md Deferred Items).

**Delivery:** PR [#85](https://github.com/szTheory/muhtari/pull/85) merged as `3686819b`, then release PR [#86](https://github.com/szTheory/muhtari/pull/86) published stable [v0.18.1](https://github.com/szTheory/muhtari/releases/tag/v0.18.1). Main CI [44726104022](https://github.com/szTheory/muhtari/actions/runs/44726104022), Release [44726103946](https://github.com/szTheory/muhtari/actions/runs/44726103946) and the latest-install check passed. See the [v12.0 audit](milestones/v12.0-MILESTONE-AUDIT.md).

**Key accomplishments:**
- The README now guides players from the release package to a test card or supported cartridge, while runner and package claims match tested behavior.
- A ROM-free C17 host quickstart now builds from the installed release ZIP through CMake and pkg-config and checks the 800-sample first frame.
- The staged quickstart checks now prove error reporting and one-time cleanup across create, allocation, frame and result failures.

---

## v11.0 Windows extended-path support (Shipped: 2026-11-19)

**Phases completed:** 2 phases, 5 plans, 10 tasks

**Closeout type:** `verified_closeout`; all six requirements and both phase verification reports passed. Known verification overrides: 0 newly acknowledged, 51 carried forward from prior closes (see STATE.md Deferred Items).

**Key accomplishments:**
- Completed the source-qualified tune-up and corrected the 52-entry deferred register without changing any entry's verdict.
- Added end-to-end explicit Windows drive and extended UNC path support to the runner, including literal names, path-identity checks and safe output replacement.
- Passed actual-runner drive and SMB UNC path coverage on Windows x64 and ARM64, with the tested path limits documented.
- Published stable v0.18.0 with 20 assets; exact-source main CI, Release and latest-install checks passed.

**Audit:** [v11.0 milestone audit](milestones/v11.0-MILESTONE-AUDIT.md) found no v11.0 requirement or integration gaps. Nine v3.0 requirements remain deferred with their original evidence and stops.

**Delivery:** Release PR [#82](https://github.com/szTheory/muhtari/pull/82) merged. Stable [v0.18.0](https://github.com/szTheory/muhtari/releases/tag/v0.18.0) was published from `ac4ee132449f82848da1db493c428f78d463d6cd`; main CI [44606205865](https://github.com/szTheory/muhtari/actions/runs/44606205865) and Release [44606205880](https://github.com/szTheory/muhtari/actions/runs/44606205880) passed. Release verified all 20 assets and `install-line-latest` passed.

**Archives:** [roadmap](milestones/v11.0-ROADMAP.md), [requirements](milestones/v11.0-REQUIREMENTS.md), [product roadmap snapshot](milestones/v11.0-PRODUCT-ROADMAP.md), [audit](milestones/v11.0-MILESTONE-AUDIT.md) and [phase records](milestones/v11.0-phases/).

**Next:** No v12.0 scope is selected. Review the [rolling product roadmap](PRODUCT-ROADMAP.md) and run `$gsd-new-milestone` after the owner chooses the recommended next scope or its strongest alternative.

---

## v10.0 Headless developer trace (Shipped: 2026-11-17)

**Phases completed:** 2 phases, 8 plans, 16 tasks

**Closeout type:** `override_closeout`; both phase verification reports pass and all eight milestone requirements are satisfied. The audit records two newly acknowledged items and 50 carried forward from earlier closes (see STATE.md Deferred Items).

**Key accomplishments:**
- Added bounded, versioned instruction and bus-event JSONL tracing to `muhtari-run` for a selected frame and address range.
- Verified fixed numeric trace records, output failure handling, trace-on/off state equality, public compatibility and runner documentation.
- Accepted the source-qualified Linux hooks-off Cachegrind receipt: 3 instructions per frame against the 32-instruction limit.
- Fixed the GCC `mismatched-dealloc` and Clang `realpath` build failures without weakening test coverage.
- Published stable v0.17.0 after PR #77 merged; exact-source main CI and Release checks passed.

**Audit:** [v10.0 milestone audit](milestones/v10.0-MILESTONE-AUDIT.md) found no blocking gaps. The one integration warning is the conservative refusal of some distinct Windows extended-path destinations; the guard found no overwrite risk.

**Delivery:** PR [#77](https://github.com/szTheory/muhtari/pull/77) merged; release PR [#78](https://github.com/szTheory/muhtari/pull/78) published [v0.17.0](https://github.com/szTheory/muhtari/releases/tag/v0.17.0) from `333380b118f62b618cb6df29cdfdd4388efc7b22`. Main CI [44214206424](https://github.com/szTheory/muhtari/actions/runs/44214206424) and Release [44214206354](https://github.com/szTheory/muhtari/actions/runs/44214206354) passed. Windows ARM64 was the longest job in both runs; neither had a failed or cancelled job. One run pair did not justify changing timeouts, caches or workflow structure.

**Archives:** [roadmap](milestones/v10.0-ROADMAP.md), [requirements](milestones/v10.0-REQUIREMENTS.md), [product roadmap snapshot](milestones/v10.0-PRODUCT-ROADMAP.md), [audit](milestones/v10.0-MILESTONE-AUDIT.md) and [phase records](milestones/v10.0-phases/).

**Next:** No successor milestone is selected. Review the [rolling product roadmap](PRODUCT-ROADMAP.md) and run `$gsd-new-milestone` to choose the next scope.
---

## v9.0 Publish the corrected Souper mapper package (Shipped: 2026-11-16)

**Phases completed:** 1 phase, 1 plan, 2 tasks

**Closeout type:** `verified_closeout`; all four requirements and the phase verification pass. Known verification overrides: 0 newly acknowledged, 50 carried forward from prior closes (see STATE.md Deferred Items).

**Key accomplishments:**
- Corrected Souper is available in stable package v0.16.0 from exact source `a0319a9c29886d776e37b818d38d36fe810f486d`; the failed v0.15.0 tag and draft remain unchanged.
- README/status PR #73 and phase closeout PR #74 merged with earlier CI [43841380444](https://github.com/szTheory/muhtari/actions/runs/43841380444) and Release [43841380334](https://github.com/szTheory/muhtari/actions/runs/43841380334) receipts. Archive PR #75 merged source `bac988bea6a4ec070943c11b83298ac1eee5a8c3`; CI [43854355858](https://github.com/szTheory/muhtari/actions/runs/43854355858) and Release [43854356086](https://github.com/szTheory/muhtari/actions/runs/43854356086) passed. Post-archive state PR #76 established current source `89b6ff82812cd5be873e9f90d32ac55fded8694b`; CI [43866551295](https://github.com/szTheory/muhtari/actions/runs/43866551295) passed and Release [43866551368](https://github.com/szTheory/muhtari/actions/runs/43866551368) skipped package work. The v0.16.0 source `a0319a9c29886d776e37b818d38d36fe810f486d` passed exact-source CI [43801428495](https://github.com/szTheory/muhtari/actions/runs/43801428495) and Release [43801428444](https://github.com/szTheory/muhtari/actions/runs/43801428444). See the [Phase 26 tune-up report](phases/26-measured-milestone-tune-up/26-TUNEUP.md) for source-specific evidence and limits.

**Audit:** [v9.0 milestone audit](milestones/v9.0-MILESTONE-AUDIT.md) passed all four requirements, the phase record and all three integration flows. It also records the release run [43801428444](https://github.com/szTheory/muhtari/actions/runs/43801428444), queue/critical-path review and clean artifact scan.

**Archives:** [roadmap](milestones/v9.0-ROADMAP.md), [requirements](milestones/v9.0-REQUIREMENTS.md), [product roadmap snapshot](milestones/v9.0-PRODUCT-ROADMAP.md), [audit](milestones/v9.0-MILESTONE-AUDIT.md) and [phase records](milestones/v9.0-phases/).

**Next:** v10.0 Headless developer trace is selected and active; Phases 26–27 are recorded in [ROADMAP.md](ROADMAP.md). The durable comparison, strongest alternative and revisit triggers remain in [PRODUCT-ROADMAP.md](PRODUCT-ROADMAP.md) and the [2026-11-16 successor review](research/next-milestone/2026-11-16-future-roadmap-review.md).

---

## v8.0 GameDrive streamed-audio contract qualification (Closed locally: 2026-11-15)

**Phases completed:** 2 phases, 2 plans, 4 tasks

**Key accomplishments:**
- Source-qualified v8.0 CI baseline, local hygiene evidence, and a bounded documentation correction without CI policy changes
- GameDrive streamed-audio evidence and compatibility assessed, with implementation blocked by the incomplete public profile and independent-oracle threshold

**Closeout type:** `override_closeout`; all five requirements are covered by the passing milestone audit, and both phase verification reports retain passing verdicts. Phase 23's verification fingerprint predates the merged timeout follow-up; the stale fingerprint is accepted under GSD-HANDOFF §3 because the audit covers the phase and exact-main CI passes. Zero new artifacts acknowledged; 50 previously acknowledged items carried forward with their original verdicts (see STATE.md Deferred Items).

**Decision:** Implementation readiness is **NO-GO** until a permission-qualified normative profile, independent exact-profile PCM oracle and finite resource facts exist. No API/ABI, state-format, fixture, firmware, hardware or product implementation change was made.

**Delivery evidence:** PRs [#67](https://github.com/szTheory/muhtari/pull/67) and [#68](https://github.com/szTheory/muhtari/pull/68) merged. Main CI on the timeout-correction merge commit passed in [run 43757444707](https://github.com/szTheory/muhtari/actions/runs/43757444707). Release passed its target checks in [run 43757444676](https://github.com/szTheory/muhtari/actions/runs/43757444676); package publication was skipped. No milestone or package tag was created.

**Archives:** [roadmap](milestones/v8.0-ROADMAP.md), [requirements](milestones/v8.0-REQUIREMENTS.md), [audit](milestones/v8.0-MILESTONE-AUDIT.md), [phase records](milestones/v8.0-phases/) and [product roadmap snapshot](milestones/v8.0-PRODUCT-ROADMAP.md).

**Next:** No successor milestone is selected. The current recommendation and strongest alternative are in the [rolling product roadmap](PRODUCT-ROADMAP.md). Run `$gsd-new-milestone` to make that scope choice; no GameDrive implementation is authorized by this closeout.

---

## v7.0 shipping reconciliation (2026-11-15)

The v7.0 local archive remains historical. PR [#63](https://github.com/szTheory/muhtari/pull/63) merged the mapper implementation; release PR #64 created tag/draft v0.15.0 at `2b47656cb0307fb5fc10d9ea3fbfd2236830c355`. Its exact-source Wasm32 `core.state.souper` check failed because the test exceeded the WASI stack. PR [#65](https://github.com/szTheory/muhtari/pull/65) moved scratch storage out of the stack without reducing assertions and merged the correction. The full PR CI and Nightly equality passed; the post-merge main CI and the exact release identity remain documented in [v7.0-SHIP.md](milestones/v7.0-SHIP.md). The v0.15.0 tag still targets the failed source and the draft remains unpublished; the release workflow skipped publication for the test-only follow-up. No tag was retargeted.

---

## v7.0 Souper homebrew mapper (Closed locally: 2026-11-14)

**Phases completed:** 2 phases, 5 plans, 10 tasks

**Closeout type:** `override_closeout`; all eight requirements and both phases pass. At the time of this local planning closeout, no package publication, push, merge or tag was claimed. Known verification overrides: 1 newly acknowledged, 49 carried forward from prior closes (see STATE.md Deferred Items). The acknowledged workspace record retains its `unknown` verdict; no worktree was changed. See the separate 2026-11-15 shipping reconciliation above for subsequent evidence.

**Key accomplishments:**
- A source-qualified v0.14.0 release and quality report records a bounded no-change disposition and preserves earlier failures.
- Exact Souper profile admission and a generated initial-map diagnostic now run through every existing local consumer.
- Souper CPU banking, EXRAM segmentation, MARIA character remapping and HALT-sensitive fetch trapping now have independent literal checks and stable libretro descriptors.
- The generated Souper diagnostic now exercises the production native libretro module, with literal observations, deterministic snapshot continuation, and refusal controls.

**Audit:** [v7.0 milestone audit](milestones/v7.0-MILESTONE-AUDIT.md). The Phase 21 spec-less probe remains explicitly unresolved and nonblocking; no probe pass is claimed.
**Archive:** [roadmap](milestones/v7.0-ROADMAP.md), [requirements](milestones/v7.0-REQUIREMENTS.md), [product roadmap snapshot](milestones/v7.0-PRODUCT-ROADMAP.md), [audit](milestones/v7.0-MILESTONE-AUDIT.md), [phase records](milestones/v7.0-phases/).
**Next:** v8.0 is selected and active; see the current scope above and [ROADMAP.md](ROADMAP.md).

---

## v6.0 YM2151 homebrew sound (Shipped: 2026-11-13)

**Scope:** Two phases, eight plans, nineteen actual plan task blocks. The runtime summary fallback counted eight tasks; the plan blocks are authoritative.

**Delivered:**
- One qualified selected/header-auto XM/YM2151 profile, integer instance-owned OPM and source-qualified digital bus/timer/IRQ behavior.
- Deterministic bounded mono audio, complete transactional YM states and genuine bidirectional legacy6/1 compatibility with unchanged API8/ABI1.
- Actual public library, runner, loaded libretro, emitted Wasm and clean installed/extracted package consumers with current notices and fault controls.
- Complete ordinary/adverse native/Node cost windows, concrete space/headroom/bounds, accurate evidence limits and clean required review.

**Closeout:** `verified_closeout`; nine current requirements, both phases and all five integration flows pass. Zero newly acknowledged items; 49 previously acknowledged items retain their verdicts. The nine deferred v3 requirements and original Phase12/14 stops remain unchanged.
**Publication:** [v0.14.0](https://github.com/szTheory/muhtari/releases/tag/v0.14.0) is published with 19 ZIP archives and `SHA256SUMS` (20 assets, matching the release contract). Release run [43441765548](https://github.com/szTheory/muhtari/actions/runs/43441765548) passed, including latest installation. PR [#61](https://github.com/szTheory/muhtari/pull/61) merged; CI on its merge commit passed. See the [ship receipt](milestones/v6.0-SHIP.md).
**Archive:** [roadmap](milestones/v6.0-ROADMAP.md), [requirements](milestones/v6.0-REQUIREMENTS.md), [audit](milestones/v6.0-MILESTONE-AUDIT.md), [phase records](milestones/v6.0-phases/).
**At the v6.0 closeout, next:** v7.0 Souper mapper-only support was selected; see its [audit](milestones/v7.0-MILESTONE-AUDIT.md) and the [rolling roadmap](PRODUCT-ROADMAP.md). Its later merge and unpublished package state are recorded in the v7.0 shipping reconciliation above.

---

## v5.0 Downloadable WebAssembly core (Shipped: 2026-11-11)

**Phases completed:** 2 phases, 3 plans, 7 tasks (actual plan blocks; SDK prose count was 4)

**Key accomplishments:**
- Current publication records and original CI receipts are qualified alongside six measured quality dimensions and a reproducible local baseline.
- A pinned, source-bound libretro ZIP now carries exact runtime notices and passes clean-extraction lifecycle checks.
- The source-bound Emscripten ZIP now travels through the existing exact-source release path with local native, SDK, and hygiene gates passing.

**Closeout type:** `verified_closeout`; all six scoped requirements and both phases pass. Known verification overrides: 0 newly acknowledged, 50 carried forward from prior closes. The nine v3.0 requirement gaps and original host/remote stops remain unchanged.
**Publication:** [v0.13.1](https://github.com/szTheory/muhtari/releases/tag/v0.13.1) is published; exact-source release, public-download verification and latest installation passed. Non-release maintenance PR #53 and current-main CI passed. Failed v0.13.0 is retained separately. [Final ship receipt](milestones/v5.0-SHIP.md#final-publication-and-current-main-closure--reconciled-2026-11-11).
**Archive:** [roadmap](milestones/v5.0-ROADMAP.md), [requirements](milestones/v5.0-REQUIREMENTS.md), [audit](milestones/v5.0-MILESTONE-AUDIT.md), [phase records](milestones/v5.0-phases/).
**Next:** Continue the existing `$gsd-new-milestone` discussion after cross-project feedback; successor scope remains unselected. [Handoff](continue.md).

---

## v4.0 — Browser/WebAssembly core readiness (Shipped: 2026-11-10)

**Closeout type:** `verified_closeout`; all five scoped requirements pass. Planning completion is separate from package release. The approved isolated source subsequently shipped through PR #47 and package v0.12.0; see the ship receipt.
**Scope:** Phases 15–16, two plans and five tasks. Task totals are reconciled from the actual plan task blocks; the CLI's summary parser counted only two.

**Delivered:**
- Corrected relative-root documentation budget coverage with representative exact-limit and fault controls.
- Built the existing libretro core as a pinned Emscripten ES-module loader and companion Wasm.
- Added a direct offline Node consumer with independent native numeric expectations, callbacks, input/state parity, refusal/replay/isolation and complete export controls.
- Added a required Linux artifact step, reproducible offline command and same-workload size/throughput reporting with explicit emulation limits.

**Evidence:** Native and pinned Emscripten workflows, packaging/release checks, hygiene, independent source reviews, both phase verifications and milestone integration passed. One review-fix round resolved all five Phase 16 findings. Windows execution remains a ship check; Node results establish no browser/device acceptance.
**Carried scope:** 50 previously acknowledged artifacts, zero newly acknowledged; nine unmet v3.0 requirements and all Phase 12/14 host/remote stops remain unchanged.
**Archive:** [roadmap](milestones/v4.0-ROADMAP.md), [requirements](milestones/v4.0-REQUIREMENTS.md), [audit](milestones/v4.0-MILESTONE-AUDIT.md), [phase records](milestones/v4.0-phases/).
**Publication:** [PR #47](https://github.com/szTheory/muhtari/pull/47) and [release PR #48](https://github.com/szTheory/muhtari/pull/48) merged; [v0.12.0](https://github.com/szTheory/muhtari/releases/tag/v0.12.0) is published. Candidate CI, release-PR CI and release succeed. [Ship receipt](milestones/v4.0-SHIP.md) preserves source-specific checks and earlier failed combined-source history. No old host retry is authorized.

---

## v3.0 — Playstead-ready play and states (Closed with deferred scope: 2026-11-10)

**Closeout type:** `override_closeout`; v3.0 is closed for planning purposes, not shipped as a package.
**Scope:** 3 phases, 64 recorded plans, 100 recorded tasks; Phase 13 is the only phase goal verified.
**Requirements:** 7/16 complete. Nine remain unchecked: TUNE-06/07, PLAY-01–04 and FRONT-02–04.

**Delivered:** Phase 13 added portable save/load across the library, runner and libretro core; its refreshed verification passed 44/44. The required local CI workflow passed. The audit records integration as passed for this state path.

**Deferred acceptance:** Phase 12 remains accepted 0/4; released-core input/battery proof, paired Playstead acceptance and the measured Windows improvement remain open. The owner-run request for the exact diagnostic app was denied before preflight or content. Phase 14 remains `gaps_found` at 20/27; actual RetroArch save/load, rewind and run-ahead acceptance is absent. The original phase records and stops remain intact in the archive.

**Known verification overrides:** 22 newly acknowledged, 28 carried forward from the prior close (see STATE.md Deferred Items). No host or remote allowance was renewed; no package release or tag was created by this planning closeout.

**Archive:** [roadmap](milestones/v3.0-ROADMAP.md), [requirements](milestones/v3.0-REQUIREMENTS.md), [audit](milestones/v3.0-MILESTONE-AUDIT.md), [phase records](milestones/v3.0-phases/).

**Next:** v4.0 is scoped to the required tune-up followed by an offline WebAssembly build and deterministic host-shim checks for the existing libretro core. A browser player/UI and the deferred v3 host acceptance remain outside that scope.

---

## v2.0 — The whole library (Shipped: 2026-10-29)

**Delivered:** Expanded boards, persistent saves, scoped controllers and PAL across all three deliverables, with honest library evidence.
**Package release:** [v0.10.0](https://github.com/szTheory/muhtari/releases/tag/v0.10.0), 19 assets; planning v2.0 is separate from package semver.
**Closeout type:** override_closeout under AGENTS rules 6–7 and GSD-HANDOFF §3; audit status tech_debt.
**Phases completed:** 5–11; 7 phase goals and 27/27 scoped requirements.
**Plans:** 75 accepted; 76 summarized active slots; 82 total historical summaries.
**Recorded tasks:** 192 across all 82 summaries, including superseded/partial work; no halted plan is counted as accepted.

**Key accomplishments:**

- Shipped Activision, Absolute, Fractalus, BankSet and the XBoard/XM RAM/POKEY/HSC subset through all three deliverables.
- Added load-stable HSC/SaveKey battery storage, checked runner saves and real fresh-launch RetroArch persistence.
- Delivered typed controller input and movie2 replay for lightgun, paddle/keypad, quadrature and modern adapters, with independent pin-level diagnostics.
- Added PAL timing, geometry, boot and sustained audio carry while retaining existing NTSC observations.
- Accounted for every pinned-list entry, bound compatibility claims to public evidence and made the library ratchet required in CI.
- Improved pipeline reliability, armed the controlled performance comparison and completed automated release/install verification.

**Stats:** 469 files changed, 85,109 insertions and 1,737 deletions from initialized
v2.0 `65b360d` to shipped `d5fd241`; 58,665 tracked C/header lines, including 10,263
in src/. Timeline: 2026-10-21 to 2026-10-29, eight elapsed calendar days. Counts
exclude this closeout. The helper's six-task prose-only count is superseded by
82 parsed summary actuals/tasks_completed values totaling 192.

### Known verification overrides and debt

Known verification overrides: 15 newly acknowledged, 13 carried forward from a prior close
(see STATE.md Deferred Items). Separately, all seven historical phase fingerprints remain
stale after later accepted changes. No fingerprint or failed verdict was rewritten.
The audit reports 25 fully wired paths plus two accepted Phase 5 warnings; eight complete
flows plus one qualified monthly-report flow; no broken core/host flow or requirement gap.
COST-01 raw monthly numeric totals, WR-01 delayed-release equal-base selection, advisory
property calibration, optional Phase 10/11 findings and hardware/HSC limits remain deferred.
The cancelled Nightly remains cancelled; current release success does not renew that evidence.
Pinned-list status remains 201 unknown/3 skipped, with no gameplay passes.

**Release evidence:** `gh run view 40956113369 --json attempt,status,conclusion,headSha,url,jobs`
— exit 0, attempt 1 success at d5fd241, including publish and install-line-latest:
[release run](https://github.com/szTheory/muhtari/actions/runs/40956113369).
`gh release view v0.10.0 --json tagName,isDraft,publishedAt,targetCommitish,url,assets`
— exit 0, published with 19 assets. `cmake -DINPUT=/tmp/muhtari-v2-main-ci.json -DEXPECTED_SHA=d5fd241e3f4d701d73867cc2e465c56a6fe6793d -P tools/check_ci_run.cmake`
— exit 0, [merged CI](https://github.com/szTheory/muhtari/actions/runs/40956113335).
Release PR41 merged green: [PR CI](https://github.com/szTheory/muhtari/actions/runs/40946415397).

**Archives:** [roadmap](milestones/v2.0-ROADMAP.md),
[requirements/outcomes](milestones/v2.0-REQUIREMENTS.md),
[audit](milestones/v2.0-MILESTONE-AUDIT.md),
[phase records](milestones/v2.0-phases/),
[product direction](milestones/v2.0-PRODUCT-ROADMAP.md),
[historical position receipts](milestones/v2.0-STATE.md).

**Next:** `$gsd-new-milestone` selects scope and creates fresh requirements. Tune-up first;
states/movies and browser remain alternatives for that discussion. No new milestone begins here.
Tag creation is disabled; release-please owns package tags. Completed quick-task history stays in place.
Lesson export is available with `export lessons`; lessons remain project-local by default.


### v2.0 closeout verification

| Command | Exit / result | Run URL |
|---|---|---|
| `cmake --workflow --preset ci` | 0; 673 CI tests and 6 release/archive checks after requirements removal | N/A-local |
| `cmake --workflow --preset hygiene` | 0; 30/30 checks | N/A-local |
| `sh scripts/hygiene.sh --tree` with stable index | 0; an earlier scan overlapping staging reported old missing paths and remains an unaccepted scan | N/A-local |
| `python3 /tmp/muhtari-v2-verify.py` | 0; 221 phase files preserved, 82 plans/summaries, 7 verifications, 27/27 requirements, links resolve, originals safely archived, user edits/implementation unchanged | N/A-local |
| `node <GSD_TOOLS> query summary-extract <archived-summary> --fields requirements_completed` for all 82 summaries | 0 each; every archived frontmatter parses | N/A-local |
| `node <GSD_TOOLS> query audit-open --json` | 0; zero open, 28 acknowledged (15 new, 13 carried) | N/A-local |
| `git diff --check` and `git diff --cached --check` | 0 | N/A-local |

The safety commit `98dd410` contains the archive before active REQUIREMENTS deletion.
No verification log, raw API payload or JUnit is committed.

---

## v1.0 — 7800 games play with sound in RetroArch (Shipped: 2026-10-21)

**Delivered:** NTSC linear and SuperGame cartridges play with TIA and POKEY audio through all three deliverables.
**Package release:** v0.4.0. Planning milestone v1.0 does not change the package version.
**Closeout type:** override_closeout, explicitly approved at milestone close.
**Phases completed:** 1–4; 73 plans, 149 recorded tasks; 25/25 milestone requirements covered.

**Key accomplishments:**

- Shipped an embeddable C17 library, headless runner and libretro core with automated attested releases and one-line RetroArch installation.
- Matched all 2,560,000 public 6502 vectors and implemented bounded SALLY bus timing, RIOT and validated cartridge loading.
- Rendered linear and SuperGame cartridges with measured MARIA DMA timing, diagnostic ROMs and cross-platform frame hashes.
- Integrated fast boot, CC0 OpenBIOS, controller movies, achievement memory maps and automated RetroArch gameplay checks.
- Delivered integer TIA and POKEY sound with canonical raw/PCM hashes, libretro callback parity and clipping-free sample pins.
- Verified the production 64-phase Q23 synthesiser against spectral and alias controls, with bounded allocation-free frame execution.

**Stats:** 492 files changed, 97,449 insertions and 369 deletions from initial preparation
`8ab05fa` to shipped release commit `40ce425`; 38,684 tracked C/header lines,
including 8,577 in `src/`. Timeline: 2026-10-15 to 2026-10-21 (six elapsed calendar days).
Counts exclude this closeout. Task count is the recorded summary count, not an estimate.

### Known verification overrides

Known verification overrides: 15 newly acknowledged, 0 carried forward from a prior close
(see STATE.md Deferred Items).

Separately, phase 1, 2, 3 and 4 verification fingerprints remain stale after later
covered-file changes. The owner explicitly accepted all four overrides. Historical
reports and digests retain their original verdicts; this is not a verified_closeout.
The milestone audit found no functional, requirements or integration gaps. 15 items acknowledged as deferred (six todos, eight dormant seeds and one compatibility-test output race); four stale phase verification fingerprints explicitly accepted.

The instructions-per-frame gate remains report-only, WASM/library jobs remain
advisory, and gate code is 1,367/8,577 source lines (15.94%, above the 10% budget).
Documented title-predicate and finite spectral-matrix limits remain in force.

**Release evidence:** [Release publication and latest-install run](https://github.com/szTheory/muhtari/actions/runs/39533130210) completed successfully; v0.4.0 is published with 19 assets.

**Archives:** [Roadmap](milestones/v1.0-ROADMAP.md),
[requirements and outcomes](milestones/v1.0-REQUIREMENTS.md),
[audit](milestones/v1.0-MILESTONE-AUDIT.md),
[phase records](milestones/v1.0-phases/).

**Next:** `$gsd-new-milestone` selects the next scope and defines fresh requirements.
No next milestone is started by this closeout. Tag creation is disabled by project configuration.

## Closeout verification

`cmake --workflow --preset ci` — exit 0: 605 regular tests and six release/archive
checks passed after archival. `scripts/hygiene.sh --tree` and `git diff --check` —
exit 0. Archive integrity check — exit 0: all 177 phase files retained, including
73 plans, 73 summaries and four unchanged verification reports; only the deferred
item acknowledgment modifies a moved phase artifact. `query audit-open --json` —
exit 0: zero open, 15 acknowledged. Hosted release evidence is linked above.

## v6.0 selection and shipment — selected 2026-11-11, shipped 2026-11-13

The owner selected v6.0 YM2151 homebrew sound and authorized autonomous local
follow-through. Both phases and all nine requirements are complete; v6.0 shipped
as v0.14.0. The [ship receipt](milestones/v6.0-SHIP.md) records publication and
main closure. Earlier v5.0 successor-pending notes are historical; prior host
stops remain separate.

## v7.0 scope selection — 2026-11-13

The owner selected the recommendation **Souper homebrew mapper support** after
cross-project feedback and bounded consumer, hardware, and adversarial review.
The scope is one generated-fixture-testable mapper profile through Muhtari's
existing deliverables; BupChip/BupBoop audio and Rikki & Vikki compatibility are
excluded. The source register, uncertainty and revisit triggers are in
[v7-SOUPER research](research/next-milestone/v7-SOUPER.md). All eight
requirements mapped to Phases 21–22 in the archived
[requirements](milestones/v7.0-REQUIREMENTS.md) and
[roadmap](milestones/v7.0-ROADMAP.md).

## v7.0 local closeout — 2026-11-14

The milestone audit passed all eight requirements, both phase verifications,
twelve integration seams and five flows. The `override_closeout` preserved one
newly acknowledged workspace item with status `unknown` and carried 49 prior
acknowledgments forward. Its worktrees were left unchanged. The roadmap,
requirements, product roadmap snapshot, audit and both phase directories are
archived locally. This closeout does not claim package publication, push, merge
or a tag; successor scope remains unselected.
