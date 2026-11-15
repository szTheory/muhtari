# Phase 23 measured milestone tune-up

This report records the evidence for **TUNE-12**. The configured post-plan local CI gate passed after its first configure attempt exposed missing ignored inputs; both outcomes and the verified fetches are recorded below.

## Source and publication baseline

The v8.0 product baseline is merged `main` at `5a1336f91bf401bab96a8e868e99c661cdca8dae`. The immediately preceding CI sample is `bb70dcf86a3872d3981304bd8c6f664dc58dc401`. The current Release workflow is a separate receipt on the current main source. The unpublished `v0.15.0` draft still points at `2b47656cb0307fb5fc10d9ea3fbfd2236830c355`, whose Wasm32 state test failed on its 64 KiB stack. The later test-only fix and successful newer main runs do not change that exact-source result; no tag, version or package was changed. See the [v7.0 ship receipt](../../milestones/v7.0-SHIP.md).

At the local source comparison, the executor was on `milestone/v8-gamedrive-qualification` at `43417a850667adbf099efab0fdebe143c703efb6`. The tracked working tree was clean. Only Phase 23 planning records differed from merged main; ignored files under `build/` held the fetched MAME XML inputs and CMake outputs. Later Phase 23 plan and research commits are planning metadata, not a new product-source acceptance receipt.

## CI timing and critical path

The two main runs have the same 20-job topology. Each had 18 successful jobs, the expected push-event skips for `title` and `perf`, no failed or cancelled jobs, and a two-second delay between workflow creation and first active work. Their workflow source files and job-name sets match. These are separate completed observations, not a trend.

| Run | Created → first active | Windows ARM64 | Quality and independent base | Build, test, package | `CI required` | Full workflow | Report-only Wasm32 |
|---|---:|---:|---:|---:|---:|---:|---:|
| [43725190953](https://github.com/szTheory/muhtari/actions/runs/43725190953), current main | 2 s | 40m47s | 25m09s | 14m59s | 15:17:17 UTC, 41m49s after creation | 44m58s | 15:20:25 UTC |
| [43720418561](https://github.com/szTheory/muhtari/actions/runs/43720418561), preceding main | 2 s | 43m07s | 26m27s | 15m55s | 14:26:56 UTC, 44m11s after creation | 45m25s | 14:28:09 UTC |

The dependency graph in [the CI workflow](../../../.github/workflows/ci.yml) places the Windows ARM64 matrix lane before required post-matrix work; `retroarch` finished last before the `CI required` roll-up in both runs. The path is therefore the slow matrix lane, then required post-matrix work, then the aggregate. The complete workflow waits longer for report-only Wasm32. The ARM64 job is a bottleneck component, not the entire required critical path.

`CI required` consumes `changes`, `hygiene`, `title`, `build`, `asan`, `nofp`, `be32`, `hash-equality`, `library`, `retroarch`, `perf`, and `quality`. The main-push `title` and `perf` skips are expected under their conditions. `library` is required; `wasm32` is outside the aggregate and remains report-only. The quality job passed on each source, but the saved receipts do not report per-metric values.

## Quality policy and lessons

The current [quality policy](../../../quality.json) keeps these floors and zero ceilings:

| Policy metric | Bound | Availability |
|---|---:|---|
| Native tests | floor 605 | required |
| ABI tests | floor 12 | required |
| Fuzz tests | floor 4 | required |
| Title tests | floor 3 | required |
| Hygiene tests | floor 24 | required |
| ASan tests | floor 593 | required |
| ABI findings, warnings, analyzer findings, sanitizer findings | ceiling 0 each | required |

These are policy bounds, not measured counts from the run artifacts. Keep the quality join and its named positive and rejection controls. The Windows independent-base inventory remains serial because the runner lacks CTest jobserver integration; the 68-minute Windows job deadline remains in place. The two durations do not identify a safe case to remove, parallelize, cache or re-time. See [Phase 21's tune-up](../../milestones/v7.0-phases/21-measured-milestone-tune-up/21-TUNEUP.md) and [ENGINEERING.md](../../preparation/ENGINEERING.md).

The reproduced [Windows workflow source/baseline lesson](../../LESSONS.md#reusable-workflow-checkout-ref-and-dispatch-baseline-2026-11-13) records prior source-attribution and fixture faults, their focused controls, and the retained Windows publisher preparation/rejection cases. The [WASI stack lesson](../../LESSONS.md#keep-large-wasi-test-frames-off-its-bounded-stack-2026-11-12) preserves the exact 64 KiB boundary and its test correction. The [archived-milestone progress lesson](../../LESSONS.md#opengsd-progress-over-archived-milestones-2026-11-13) explains that analyzer progress projections can omit archived phases; the current analyzer result is complete-scope and the actual active STATE/ROADMAP records remain authoritative. Inbound or historical possibilities not reproduced in this checkout remain hypotheses.

## Six quality dimensions

These 1–5 scores summarize the evidence available for this tune-up. They are not new gates or a full product audit.

| Dimension | Score | Evidence and rationale | Limit |
|---|---:|---|---|
| Performance | 2/5 | The two current completed main receipts identify Windows ARM64 as the longest lane and its independent-base step as the largest component. | Two runs do not establish a trend or distribution, and no per-case profiling or billed-runner total was collected. The score reflects evidence limits, not a measured product regression. |
| CI/CD | 4/5 | The current and preceding required aggregates pass on distinct, identified sources; the workflow has explicit required and report-only membership. | This review covers two receipts. The Release target pass below did not publish a package, and older failures remain source-specific. |
| Ease of use | 4/5 | The [README](../../../README.md) and [libretro guide](../../../docs/libretro.md) provide the existing runner, embedding and host paths. | No installation, live host or physical-audibility check was part of this report. |
| Architecture | 4/5 | The project retains its three-deliverable, deterministic-core boundary and existing [CMake workflows](../../../CMakePresets.json); this tune-up adds no runtime layer. | This report does not re-evaluate the emulator architecture or establish physical hardware conformance. |
| Docs/README | 4/5 | The current [project](../../PROJECT.md), [requirements](../../REQUIREMENTS.md), [roadmap](../../ROADMAP.md), and product roadmap describe the selected v8.0 qualification scope; testing details remain in [docs/testing.md](../../../docs/testing.md). | The README install path and live release state were not executed or refreshed here. |
| Upgrade path | 3/5 | [`pins.txt`](../../../pins.txt), the verified [pin-fetch script](../../../scripts/fetch-pin.sh), and [maintainer documentation](../../../docs/maintaining.md) preserve an auditable update route. | `v0.15.0` is still unpublished on its failed exact source; external dependency freshness and vulnerabilities were not queried. |

## Proportional maintenance disposition

The [SEED-009 review](../../seeds/SEED-009-proportional-maintenance-review.md) found no evidence for a CI or quality-policy change. The missing MAME XML files were expected ignored build inputs: [CI fetches them before hygiene configure](../../../.github/workflows/ci.yml), and the existing script verified each file against `pins.txt`. Their exact MAME data files are CC0 under [the settled provenance record](../../../PROVENANCE.md). This setup issue was repaired locally without changing tracked files.

The committed-range whitespace check reproduced a small documentation fault: three Markdown hard-break markers in `23-RESEARCH.md` were rejected as trailing whitespace. The correction replaced only those markers with explicit `<br>` tags and preserved the prose and evidence. No product, test, workflow or quality-policy bytes changed.

Windows ARM64's long baseline comparison recurred in both main samples, but two successful observations are not enough to change serial execution, the 68-minute deadline, matrix, topology, cache policy, parallelism, or required/advisory membership. Retain the existing quality floors and all positive and rejection controls. Revisit only after a reproduced required fault, or a larger comparable completed sample plus per-case profiling isolates redundant work while demonstrating equivalent fault detection. No new dashboard, recurring validator, metric system, dependency or quota was introduced.

## Local verification evidence

| Evidence | Source | Command | Exit | Run URL | Result | Limit |
|---|---|---|---:|---|---|---|
| Current main CI | `5a1336f91bf401bab96a8e868e99c661cdca8dae` | Inherited: `gh run view 43725190953 --json databaseId,workflowName,event,status,conclusion,headSha,url,createdAt,startedAt,updatedAt,jobs` | 0 | [Run 43725190953](https://github.com/szTheory/muhtari/actions/runs/43725190953) | 20 jobs; 18 success, two expected skips, none failed or cancelled. | This receipt accepts only its own source. |
| Previous main CI | `bb70dcf86a3872d3981304bd8c6f664dc58dc401` | Inherited: `gh run view 43720418561 --json databaseId,workflowName,event,status,conclusion,headSha,url,createdAt,startedAt,updatedAt,jobs` | 0 | [Run 43720418561](https://github.com/szTheory/muhtari/actions/runs/43720418561) | Same job topology and outcome classes as current main; separately timed above. | It is a previous source, not current-source acceptance. |
| Current Release | `5a1336f91bf401bab96a8e868e99c661cdca8dae` | Inherited: `gh run view 43725190733 --json databaseId,workflowName,event,status,conclusion,headSha,url,createdAt,startedAt,updatedAt,jobs` | 0 | [Release run 43725190733](https://github.com/szTheory/muhtari/actions/runs/43725190733) | `release-target` passed; `release-ci`, `release-equality`, `publish`, and `install-line-latest` skipped. | Not a package publication or fresh release acceptance result. |
| Historical v0.15.0 | `2b47656cb0307fb5fc10d9ea3fbfd2236830c355` | Inherited from [v7.0-SHIP.md](../../milestones/v7.0-SHIP.md); historical command exit not recorded here. | not recorded | [Exact-source Release failure](https://github.com/szTheory/muhtari/actions/runs/43619982705); [PR CI](https://github.com/szTheory/muhtari/actions/runs/43620055164); [main CI](https://github.com/szTheory/muhtari/actions/runs/43630097925); [main Release](https://github.com/szTheory/muhtari/actions/runs/43630097914) | The exact tagged source exceeded the Wasm32 stack in `core.state.souper`; the draft/tag remains unpublished. | Later sources do not repair this result; retargeting and publication are outside this phase. |
| Local execution | `43417a850667adbf099efab0fdebe143c703efb6` | `git rev-parse HEAD`; `git status --short --branch` | 0 | N/A-local | Actual source-comparison snapshot on the milestone branch; tracked tree clean. | Later planning-only commits are not product-source receipts. |
| Working-tree whitespace preflight | `43417a850667adbf099efab0fdebe143c703efb6` | `git diff --check` | 0 | N/A-local | No working-tree whitespace errors. | This command checks the working tree; committed history is checked separately below. |
| GSD roadmap analyzer | `43417a850667adbf099efab0fdebe143c703efb6` | `gsd_run query roadmap.analyze` (runtime identity `@opengsd/gsd-core` 1.20.0) | 0 | N/A-local | Scope complete; Phase 23 had one plan and no summary yet; Phase 24 depends on it. No archive projection warning appeared. | A planning projection is not a phase acceptance verdict. |
| Tree hygiene preflight | `43417a850667adbf099efab0fdebe143c703efb6` | `scripts/hygiene.sh --tree` | 0 | N/A-local | Passed in about 41 seconds. | Checks the current tree; it is not product CI. |
| CMake hygiene, initial attempt | `43417a850667adbf099efab0fdebe143c703efb6` | `cmake --workflow --preset hygiene` | 1 | N/A-local | Configure stopped because ignored `build/pins/mame/a7800.xml` and `a2600.xml` were absent. | This was missing local setup, not a tracked source defect. The result is retained. |
| Pinned A7800 database | `pins.txt`, MAME `dddf4cd1faee`, CC0 | `scripts/fetch-pin.sh mame-a7800-xml build/pins/mame` | 0 | N/A-local | Existing fetch script verified the pinned SHA-256 and wrote the ignored local build input. | No repository source or license policy changed. |
| Pinned A2600 recognition list | `pins.txt`, MAME `dddf4cd1faee`, CC0 | `scripts/fetch-pin.sh mame-a2600-xml build/pins/mame` | 0 | N/A-local | Existing fetch script verified the pinned SHA-256 and wrote the ignored local build input. | No repository source or license policy changed. |
| CMake hygiene, targeted retry | `43417a850667adbf099efab0fdebe143c703efb6` | `cmake --workflow --preset hygiene` | 0 | N/A-local | Configure and build passed; all 30 registered hygiene tests passed. | Local hygiene evidence only; the configured full CI gate is recorded after execution by the orchestrator. |
| Committed-range whitespace, initial | `43417a850667adbf099efab0fdebe143c703efb6` | `git diff --check origin/main..HEAD` | 2 | N/A-local | Found three trailing-space hard-break markers in the newly added `23-RESEARCH.md` lines 3, 4 and 300. | The failure is preserved; it was corrected in the bounded documentation change below. |
| Research whitespace correction | `fef074f` | `git diff --check -- .planning/phases/23-measured-milestone-tune-up/23-RESEARCH.md` | 0 | N/A-local | Only the three markers changed to explicit `<br>` tags; the focused working-tree check passed. | Documentation formatting only. |
| Committed-range whitespace, corrected | `fef074f` | `git diff --check origin/main..HEAD` | 0 | N/A-local | The committed milestone range is clean after the correction. | Does not change or extend any hosted receipt. |
| Final committed-range whitespace | `6825c98a56431eee4faa70bdf92268db58ed7698` | `git diff --check origin/main..HEAD` | 0 | N/A-local | All committed Phase 23 planning, report, summary and state changes pass the committed-range whitespace check. | Local documentation hygiene only. |
| Tracked and untracked change inventory | `43417a850667adbf099efab0fdebe143c703efb6` | `git diff --name-status 5a1336f91bf401bab96a8e868e99c661cdca8dae`; `git ls-files --others --exclude-standard` | 0 | N/A-local | Only STATE and the Phase 23 context/research/plan records differed; no untracked non-ignored files. | The comparison snapshot predates report and later planning commits. |
| Required source objects | local repository object database | Three `git cat-file -e '<sha>^{commit}'` checks for current main, preceding main and the v0.15.0 target | 0 | N/A-local | All three full commit objects were present. | Presence does not imply acceptance for the v0.15.0 target. |
| Product-source comparison | `origin/main..43417a850667adbf099efab0fdebe143c703efb6` | `git diff --exit-code origin/main..HEAD -- src include runner libretro tests tools CMakeLists.txt CMakePresets.json cmake scripts quality.json pins.txt .github/workflows` | 0 | N/A-local | No product, public-header, runner, libretro, test, build, policy, pin or workflow byte differed from merged main. | Scope is the listed tracked source paths at the comparison snapshot. |
| Post-plan CI gate, initial configure | `e55048b0a4aedfdfb61716c521eff94447f2b536` | `gsd_run run-with-timeout 0 -- bash -c 'cmake --workflow --preset ci'` | 1 | N/A-local | Configure stopped because the ignored Wasmtime, 7800basic and OpenBIOS inputs were absent; no tests ran. | Initial gate attempt; the local setup cause is preserved and resolved below. |
| Wasmtime local input | `pins.txt`: `wasmtime-aarch64-macos` 48.0.6 | `scripts/fetch-pin.sh wasmtime-aarch64-macos build/pins/wasmtime` | 0 | N/A-local | Existing fetch script verified the pinned SHA-256 and installed the ignored local runtime. | Tool binary only; no source change. |
| 7800basic local tool package | `pins.txt`: `7800basic-wasm` 0.41 | `scripts/fetch-pin.sh 7800basic-wasm build/pins/7800basic` | 0 | N/A-local | Existing fetch script verified the pinned SHA-256; the WebAssembly compiler is used only as a tool under Wasmtime. | GPL source and its scripts were not opened or copied; build output stays ignored. |
| OpenBIOS local assembly input | `pins.txt`: `7800openbios-asm` at `b8bc745a8744` | `scripts/fetch-pin.sh 7800openbios-asm build/pins/openbios` | 0 | N/A-local | Existing fetch script verified the CC0 assembly source pin. | Only the authorized assembly input was fetched; copyrighted included binaries were not fetched. |
| rcheevos local test source | `pins.txt`: rcheevos 12.1.0 | `scripts/fetch-pin.sh rcheevos build/pins/rcheevos` | 0 | N/A-local | Existing fetch script verified the MIT source pin used by its test-only resolver. | Build-tree test input only; it is not vendored or shipped. |
| Post-plan CI gate, after pinned inputs | `e55048b0a4aedfdfb61716c521eff94447f2b536` | `gsd_run run-with-timeout 0 -- bash -c 'cmake --workflow --preset ci'` | 0 | N/A-local | Configure, 267 build steps, package generation, 723/723 `ci` tests and 8/8 `ci-release` tests passed. | Local macOS arm64 run; it does not replace hosted matrix or release receipts. |
| Report link and evidence validator, first draft | `23-TUNEUP.md` | Task 2's one-off Node validation command in [23-01-PLAN.md](23-01-PLAN.md) | 1 | N/A-local | It identified that the first draft omitted an explicit `TUNE-12` mapping; the report now states the requirement. | This was a report completeness issue, not a product or CI failure. |
| Report link and evidence validator, corrected | `23-TUNEUP.md` | Same one-off Node validation command from [23-01-PLAN.md](23-01-PLAN.md) | 0 | N/A-local | Relative targets, evidence table schema, exact source/run pairs and required evidence labels passed. | One-off report validation, not a recurring project check. |
| Lesson heading fragments | `.planning/LESSONS.md` | `rg -n '^## (Reusable workflow checkout ref and dispatch baseline \(2026-11-13\)|Keep large WASI test frames off its bounded stack \(2026-11-12\)|OpenGSD progress over archived milestones \(2026-11-13\))$' .planning/LESSONS.md` | 0 | N/A-local | All three linked section headings exist at the fragment targets. | Confirms headings and links, not the lessons' evidence beyond their text. |
| Final report whitespace | untracked `23-TUNEUP.md` | `git diff --no-index --check /dev/null .planning/phases/23-measured-milestone-tune-up/23-TUNEUP.md` | 1 | N/A-local | Expected exit 1 because the untracked report differs from `/dev/null`; no whitespace diagnostics means it passes. | This check is run after the report's last edit. |

## Limits and revisit triggers

This report creates no new product-source acceptance claim beyond the exact hosted receipts above. The configured `cmake --workflow --preset ci` post-plan gate passed after its initial configure-only failure was traced to missing ignored pins and corrected with the existing hash-verifying fetch script. No hosted CI dispatch or rerun, RetroArch, hardware, release retry, publication, API/state compatibility decision, GameDrive contract research, or device/platform claim was made here. The original Phase 12/14 host and remote stops, the nine deferred v3.0 requirements, and the unpublished v0.15.0 exact-source boundary remain unchanged.

Revisit the CI disposition only if a required fault reproduces, or if a larger comparable completed sample and per-case profiling isolate redundant work while retaining equivalent fault detection. Revisit the historical release only under its separate source/version decision. The unresolved GameDrive streaming contract remains Phase 24's qualification and go/no-go scope.
