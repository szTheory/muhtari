---
title: "Proportional gates: CI policy and migration boundaries"
summary: "Retire the LOC allocation system without retiring product, evidence, fixture or release safeguards."
read_when: "Before deciding the proportional-gate policy or planning its bounded migration."
updated: 2026-10-23
status: research-only
---

# Recommendation

Remove the hard LOC gate and its dedicated counter/inventory/admission/reporting machinery. Do not convert the same system into a permanently maintained advisory subsystem, approve N192→654 by implication, or install a new complexity analyzer as its replacement. Keep the existing product and evidence judges. Review their recurring value using observed faults, feedback time and maintenance burden at the already scheduled tune-up. This research authorizes no implementation or change to an existing bound.

The decisive local evidence is specific: phase 06 has twice constructed substantially more policy/control glue than the estimated allowance, while no board implementation has been installed. The historical fit measured 665 net lines; the bounded redesign measured 654, including 91 for a permanent LOC grant and 123 for fixture comparability. The first component exists to administer the proxy; the second protects a real comparison boundary and remains necessary after the proxy disappears. Neither total establishes an optimal implementation, impossibility within 192, or authorization to spend 654. Sources: `06-01-SUMMARY.md`, `06-REDESIGN.md`, `06-CONTEXT.md` D-16–18.

Google's engineering-productivity guidance asks whether measurement has a concrete decision, whether negative findings will change that decision, and whether the benefit exceeds measurement cost. Its discussion explicitly allows eliminating a process when its cost outweighs its benefit. It does not establish an emulator-specific LOC or complexity threshold. [Google, Measuring Engineering Productivity](https://abseil.io/resources/swe-book/html/ch07.html), accessed 2026-10-23. The recommendation above is an inference from that guidance plus the local halt evidence, not a quoted Google policy.

## Options and recurring cost

| Option | Benefit | Recurring cost, risk and portability | Assessment |
|---|---|---|---|
| Delete LOC gate and its machinery | Removes allocation loops, counted-boundary disputes and metric-specific code; retains fault-specific checks | Review must still notice unnecessary machinery; does not itself make existing CI cheap | Recommended; review additions in the existing phase review and tune-up |
| LOC report only, retaining current inventory/counter | Preserves longitudinal inventory and ratios | Inventory paths/ranges, controls and reporting still need maintenance; six native producers still count the same source; reports can become informal blockers | Reject as default. A one-off `git diff --stat`/whole-file count can answer a concrete review question without a persistent contract |
| New complexity/duplication analyzer | May locate difficult routines | Must select languages and syntax support for CMake, shell, YAML and embedded strings; a C-focused analyzer misses much of this gate surface. Adds installation/pinning, licensing, upstream and platform maintenance, false-positive triage and threshold gaming | No demonstrated need here; do not substitute a new proxy or dependency |
| Keep LOC hard gate, approve 654 | Allows this measured construction | Preserves grant/admission lifecycle and future estimation failures; the number is a constructed result, not a user requirement | A distinct explicit policy choice, not a consequence of accepting this research |

The current CMake/POSIX stack is already sufficient for the narrow migration. Prefer reuse of the current judge, dispatcher and `expect_fail.cmake`; do not add a package, framework, allowance interpreter, policy DSL or dashboard. No new max-checks, max-files or number-of-tests quota substitutes for the LOC cap.

## What protects a fault, and what administers allocation

| Active boundary | Fault or user consequence | Disposition |
|---|---|---|
| Core/frame/cart/CPU/audio tests, deterministic properties, hostile-loader/run replay | Wrong hardware behavior, refusal mutating a loaded instance, cross-instance leakage, a jammed ROM failing to return | Preserve. Inputs and emulated execution bounds are part of the contract |
| `core.no_alloc_after_load`; allocator-failure tests; bounded sample FIFO/kernel work | Runtime heap activity, failed-load damage, overflow/unbounded audio work | Preserve. Current audio bounds are samples/work/buffer bounds, not source lines (`docs/testing.md` final audio-stress section) |
| `abi.*`, public API clients, no-floating-point/global/libc checks | Host breakage, nondeterminism or unexpected platform services | Preserve; no ABI or public-header change follows from retiring LOC |
| ASan/UBSan, analyzer, fatal compiler warnings, fuzzing | Memory/undefined-behavior defects and regressions | Preserve findings and real producer checks; zero may only come from completed judges |
| Exact configure inventories and `check_junit.cmake` | Unregistered, omitted, skipped, failed or duplicated required execution | Preserve both. Aggregate test-count floors are coarse omission proxies; exact names/labels and completed JUnit are stronger evidence. Simplifying overlapping count ratchets is a later tooling seed, not a migration dependency |
| `hashes.cmake`, `lane_equality.cmake`, native/BE/WASM equality | Missing lane, mixed identity, different frame/audio outputs | Preserve names, artifact multiplicity, SHA/configuration/fixture authentication and lane status |
| Fixture manifest, rebuild, coverage and firmware scan | Unlicensed/unadmitted bytes, stale fixture digest, shipped firmware | Preserve; manifest comparability still needs a narrow repair before new board rows |
| Compatibility ledger/results judge and legacy allowance store | False playability claim or loss of a trusted public predicate | Preserve; retain known rolling-waiver lifecycle limitations rather than claiming the aggregate fixes them |
| Ir comparator and arming controls | Deterministic instruction-work regression on test-card workload | Preserve current comparator/allowance semantics. It is workload-specific, not proof of all games' performance |
| Three-package contents, OS/runtime floors, installed consumer, install line | Wrong/missing deliverable, accidental link dependency, unsupported target, broken install | Preserve independently of archive byte count |
| Archive bytes 399937 | Growth in actual compressed distribution output | Leave unchanged in this migration. Being measurable does not establish that this historical ceiling is a justified user/device/storage limit; review its purpose separately |
| Test/job timeouts, fuzz duration/max_len, frame caps | Hung CI or unbounded test/input workload | Preserve existing measured values/status. CI critical path and wall-clock/RSS trends are not presently ordinary aggregate hard metrics |
| README/header/docs byte budgets | Manuals crowding out user guidance, growing API prose | Outside this change; they are separate editorial proxies, not memory/runtime budgets |
| `quality` job, eight ordinary producer lanes, actual source run/SHA, independent event base, worst-lane reduction, reasons lifecycle | Forged/stale/missing observations or candidate policy silently admitting a regression | Preserve; remove one metric without disabling this job |
| `CI required` exact dependency membership/skip rules, branch checks, release signer/ref/archive verification | A skipped/omitted lane passing, wrong commit published, unauthenticated assets | Preserve. Required-check infrastructure has a security/product purpose |
| `gate_lines`, full/authored source ratios, counted inventory, `INITIAL_GATE_CEILING`, gate-only `exit` reason, B/S/N/C provenance and new literal phase grant | Enforce a maintenance allocation on a proxy | Retire together; do not retain disconnected optional machinery |

Sources for this map: `tests/tests.cmake`, `cmake/MuhtariTests.cmake`, `tests/cmake/{check_junit,quality,quality_evidence,quality_join,hashes,lane_equality,floor,archive_contents,rom_hash_coverage}.cmake`, `tests/{core,fuzz,policy}/tests.cmake`, `.github/workflows/{ci,be-equality,release}.yml`, `docs/{testing,quality,emulation-tests}.md`, ENGINEERING §§3–5,8–12. These are classifications of actual checks, not a claim that all current machinery has optimal value.

Google's testing guidance favors fast, reliable feedback that isolates failures and retains focused integration coverage over an E2E-heavy strategy. It supports keeping the smallest layer that catches the fault, not deleting all host tests or prescribing a numerical test ratio here. [Google Testing Blog, Just Say No to More End-to-End Tests](https://testing.googleblog.com/2015/04/just-say-no-to-more-end-to-end-tests.html), accessed 2026-10-23.

## Exact migration path map

| Path/interface | Narrow change after policy authorization | Must remain |
|---|---|---|
| `AGENTS.md` rule 7; ENGINEERING §11 gate row/revision; DEC.10 | Replace active line allocation/counting policy with fault-based proportionality and review/tune-up rules | CMake/POSIX preference, named fault/control, review/gap limits, tooling findings as seeds, no line golf or weakened safety |
| `quality.json` | Remove only `metrics.gate_lines` and its provenance; do not initialize a substitute | Every unrelated metric/bound/identity. Existing floors need no adjustment solely for counter removal |
| `tests/cmake/gate_count.cmake`; `tests/policy/gate-lines.txt` | Delete | Historical counts stay in committed historical records |
| `tests/policy/tests.cmake:174–268`; `tests/inventory/ci.txt` | Remove generated counter-only control script/registration and precisely its inventory row | `policy.quality.controls`, all other policy controls and exact inventory registration |
| `tests/cmake/quality_evidence.cmake:38–41,46,118–126` | Remove metric identity branch, direction special case, native count invocation, missing-inventory fallback and `gate-count.json` output | Completed JUnit, cache/source SHA checks, archive judge/bytes, environment identity, independent base workflow, original raw base-policy material |
| `tests/cmake/quality.cmake:89–91,149–151,190–195,217–221` | Remove gate-only reason/adoption/unavailable behavior; narrowly permit retirement of this one known base-policy key at deletion guard | Non-gate base-key retention, identity/availability checks, typed values, SHA/run identity, reason expiry/renewal and measured regression checks |
| `tests/cmake/quality_join.cmake:52,68,80` | Remove native configuration exception, base-unavailable exception and `INITIAL_GATE_CEILING` argument | Exact eight-lane artifact directory set, four files per lane, trusted raw base-policy/SHA equality, source-run binding, environments, each individual lane validation before reduction |
| `tests/policy/quality.cmake` | Replace gate-initialization-only cases with focused retirement-transition controls; remove caller argument plumbing | All ordinary controls. Unknown base metric deletion still fails; arbitrary missing evidence/loosening still fails |
| `docs/quality.md` | Remove fixed-structural-ceiling and temporary-gate-growth prose; document narrow retirement behavior | Accurate ordinary aggregate scope, independent authorities and explicitly unavailable/advisory gaps |
| `.github/workflows/ci.yml` | Normally no topology change: existing producers/join already call modified scripts | `quality` stays in needs and exact `CI required` key set; same artifacts/source run and six native producers |
| Native/PowerPC full inventories | Their common `ci.txt` loses precisely the retired counter test | BE full equality still builds/packages/runs the shared remaining inventory |
| `tests/inventory/wasm32.txt`, nofp/hygiene/fuzz inventories | No counter row exists here; no counter-related edit | WASM remains its existing advisory/portable path, ABI-only nofp and other inventory selection unchanged |
| `.github/workflows/{be-equality,nightly,release}.yml`; `lane_equality.cmake` | No LOC-specific changes needed | Same immutable-commit release/full equality and authenticated fixture/configuration/hash interfaces |

There is one counter-control registration, under `MUHTARI_INVENTORY STREQUAL ci`, with label `policy`, not `portable`. Current `ci.txt` contains **610** non-release registrations and **598** excluding ABI and release. Removing this one yields **609/597**, still above current aggregate floors **605/593**. Therefore counter deletion alone requires no floor decrease, exception or count padding. Recompute from the actual implementation tree, including legitimate later board registrations; never lower a floor by guessing. This count is registration analysis, not a claim of completed full test execution.

### Trusted-base retirement bridge

Deleting the JSON entry alone fails today: `quality.cmake` enumerates every base-policy key and requires that candidate policy still contain it. The bridge must authorize only retirement of `gate_lines`, while all unrelated base keys retain this fail-closed deletion check. Prefer a small literal reviewed exception at that existing loop, with a positive old-policy→new-policy case and negative cases for every relevant bypass. Do not add a caller-supplied list of ignored metrics or rewrite the raw base policy.

The current producer recursively invokes the **current evidence script** against the independently rebuilt base tree. Once count production is removed, both sides can emit the remaining ordinary metrics; there is no need to invent unavailable gate evidence or run the deleted counter against the base. The lane must continue carrying the original base-policy bytes, and join must still compare them exactly with independently fetched event-base policy. Remove the gate-specific initial-adoption escape entirely; other new ordinary metrics continue to require real independent baseline evidence.

Preserve exact applicable membership contracts rather than disabling aggregate validation: artifact set, per-lane files, inventory names/labels, completed JUnit, surviving metric identity and required observations. The deletion difference is exactly the retired key, not permission to drop an arbitrary policy metric. Controls should reject an unknown extra base-policy metric being deleted, removal/loosening of an unrelated bound, use of an unavailable surviving required metric, and forged base policy/SHA/run/configuration.

**Existing-schema limitation, reproduced:** the judge does not currently enforce total policy/observation metric-set equality or a closed whitelist of all metric names. A complete additional metric supplied on all direct inputs passes; an extra observation key is ignored. Thus it would be false to describe general unknown-metric rejection or full metric-set equality as an existing guarantee. Do not weaken current fail-closed missing-key checks. Broad schema hardening is a tooling seed under rule 7, not another mandatory redesign of this migration. Explicit `gap.*` observations already exist outside the ordinary policy; equality must not accidentally promote those advisory gaps into required metrics.

### Fixture comparability survives removal

`quality_evidence.cmake:25–27,153–159` hashes the **entire** fixture manifest into environment identity; join repeats equality. Adding the three required board rows still makes candidate/base incomparable. Removing `gate_lines` cannot repair this and does not authorize deleting fixture identity.

The phase already has a bounded proposed solution: allow only unique added rows for `tests/diag/{activision,absolute,fractalus}.a78`, retain every trusted old row byte-for-byte, reject deleted/changed/duplicate/unrelated rows, authenticate candidate fixture admission/rebuild, and derive comparison identity from the independently obtained trusted manifest after proving that relation. Compiler/platform and built-sample identities stay pinned; later bases already containing these rows must protect them as old rows. Reuse the reviewed helper/control construction as evidence to examine, not as an automatically accepted patch. The 654-line candidate uses controlled copies of another fixture; those are not board diagnostics and must never be installed as product evidence.

## Preventing the phase-06 replanning loop

Make one bounded policy/migration step, then one successor planning step; do not start a new sequence of speculative line-fit constructions. Update active decisions explicitly before installing contrary policy. Preserve `06-01-SUMMARY.md` as the historical failed construction, its command/exit evidence, `requirements-completed: []`, zero task commits and the existing summary count. Do not rewrite it into a pass, delete it to hide the halt, or edit the 665/654 measurements. `06-REDESIGN.md` remains a historical proposal; a separately dated disposition can record that LOC retirement superseded its grant question. Future completion has its own truthful evidence and status.

| Existing plan | Successor adjustment |
|---|---|
| 01 | Replace full-phase counted preconstruction, grant/provenance and B/S/N/C proof with the bounded retirement migration plus fixture-comparability prerequisite; prove preserved ordinary guards. Preserve old 01 halt, use explicit successor status/evidence |
| 02 | Remove counted-fit precondition and recount; keep executable Absolute tracer on library/runner/module, bounded mailbox, emulator forwarding and early three-package check |
| 03 | Remove LOC proof/recount references; retain additive API/ABI clients and exact native/nofp inventories |
| 04 | Remove LOC assertion/read-first counter/halt-table dependency; retain normalization/raw identity and real probe controls, local packages |
| 05 | Remove count cap/recount; retain physical aliases, exact RAM, reset semantics, zero allocation and portable inventory |
| 06 | Require implemented comparability rather than counted construction; retain legal assembly/config/fixture triplets, rebuilds, independent numeric expectations and nine actual host observations |
| 07 | Replace fit proof with passed prerequisite checks; retain real loaded resolver pointers/holes and AM/OM three-host equivalence |
| 08 | Remove allocation-table/count references; preserve max_len/frame/fuzz-time bounds, deep refusal state, resumed behavior and instance isolation |
| 09 | Delete final LOC tightening task/provenance/user docs; retain six-native current-head package acceptance, native/BE/arm/WASM status, hash equality, sanitizers/fuzz/properties, RetroArch, compatibility claims and agent-owned evidence |

Reconcile `06-CONTEXT.md` D-16–17, relevant `06-RESEARCH.md` sections, active state/roadmap/verification and all nine plans together at the successor planning boundary. D-18's 399937 ceiling remains. Do not mechanically replace 192 with 654. Plan ordering can remain sequential for shared source/inventory/manifest ownership, but a deleted global budget is no longer its rationale. No BOARD requirement is delivered by policy migration. Review remains at most two fix rounds/one gap round; unrelated tooling/test review findings become seeds. Existing absence of future board semantics, real numeric diagnostics and six-platform package evidence remains honest work to complete, not a reason to reopen the entire policy design.

## One bounded migration verification step

These are proposed implementation acceptance commands, **not results claimed by this research**. Extend the existing CMake quality control dispatcher and `expect_fail.cmake`; no new harness dependency is needed.

1. Retirement positive controls: trusted old policy with gate metric→candidate without it; next base with neither metric; ordinary positive/worst-lane reduction cases still pass. Retirement negatives: another deleted base key, forged/changed raw base policy, missing/wrong required observation, unavailable required value, wrong SHA/source/configuration, ordinary threshold loosening, missing/extra producer directory, and gate re-adoption through the removed initialization switch. If a future policy introduces another metric, it still needs fresh independently comparable evidence; retirement grants nothing.
2. Fixture controls: unchanged/additions/later-base positives; altered/deleted/duplicate trusted rows, duplicate added paths, unrelated path/license/digest/comment additions and compiler/sample changes fail. Normal fixture admission/rebuild remains authoritative.
3. Run the existing complete workflow and exact JUnit checks, including release checks. Confirm no consumer still asks for `gate-count.json`; inspect active references without treating historical planning evidence as dead code.

```sh
ctest --preset ci -R '^policy[.]quality[.]controls$' --no-tests=error --output-on-failure
cmake --workflow --preset ci
cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake
cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake
git diff --check
```

Run the full workflow after the focused control run as shown, or direct the focused run to a distinct JUnit path: the preset's focused CTest invocation overwrites its `junit.xml`, so a later full-inventory check must consume full-run output. CTest documents that JUnit output is overwritten and provides `--show-only=json-v1` and `--no-tests=error`; exact inventory validation remains local custom logic. [CMake 4.3 CTest manual](https://cmake.org/cmake/help/v4.3/manual/ctest.1.html), accessed 2026-10-23.

For hosted acceptance use the existing PR/push CI, not a new job: `gh run list --commit "$head" --workflow ci.yml`, inspect/wait for that exact run, record command/exit/run URL, verify all eight ordinary lanes plus requested perf artifact and successful `quality`/`CI required`. An invalid skip or omitted quality job must retain a negative control. GitHub documents that required checks may have skipped/neutral status; therefore the repository's explicit aggregate skip/membership judge is still material. [GitHub, About protected branches](https://docs.github.com/en/repositories/configuring-branches-and-merges-in-your-repository/managing-protected-branches/about-protected-branches), accessed 2026-10-23. Release workflows and auto-merge are not to be disabled to make this migration pass.

## Research evidence and limits

| Command | Exit | Observation | Run URL |
|---|---:|---|---|
| `cmake -DROOT=. -DWORK=/tmp/muhtari-quality-research-controls -P tests/policy/quality.cmake` | 0 | Current ordinary, adoption, producer and join controls execute; expected failures are asserted | N/A-local |
| `cmake -DROOT=. -DINVENTORY=tests/policy/gate-lines.txt -DOUT=/tmp/muhtari-quality-research-count.json -P tests/cmake/gate_count.cmake` | 0 | Installed 15287; full/authored source 8580/6533 | N/A-local |
| `python3 /tmp/muhtari-quality-retirement-research.py` | 0 | Current `cmake -P tests/cmake/quality.cmake`: equality exit0; extra observation exit0; complete unknown metric exit0; deleting base gate metric exit1 (`typed OBJECT required at metrics;gate_lines`) | N/A-local |
| Python standard-library inventory selection (`release`, then `abi`/`release` label exclusion) | 0 | Registered counts 610/598; retired test selected in each | N/A-local |

No code/policy change, full product run, hosted run, fixture implementation or release action was performed. The one-off driver is disposable evidence, not a proposed Python repository gate. Original historical counts were read, not recomputed against abandoned trees. Current counts do not establish future board-package fit.

## Stakeholder and adversarial check

| Concern | Failure if migration is careless | Required response |
|---|---|---|
| User wants a correct, installable emulator | Broad aggregate removal hides broken archives/ABI or omitted native lane | Delete only LOC machinery; keep named independent judges and aggregate needs |
| Maintainer wants less policy work | Advisory counter becomes permanent dead complexity or new analyzer starts threshold debates | Delete unused counter, inventory, adoption and provenance paths; use existing review/tune-up |
| Agent optimizes for green status | Drops fixture identity, raw base policy or unrelated metric while claiming retirement | Negative controls at those precise boundaries; no caller-selected ignored-key list |
| Plan author seeks progress | Treats 654 or scratch fixture observations as accepted construction/product behavior | Explicit disposition and successor plans; retain historical halt and require actual board evidence |
| Reviewer expands task | Turns unrelated test/tooling observations into mandatory migration work | Record seeds, honor review/gap limit and avoid schema-framework work |
| Release consumer needs trusted assets | Removing source proxy also relaxes contents, floor, signer or tagged-commit checks | No change to release topology, signer/ref identity, archive membership or attestations |

Remaining uncertainty: the simplest correct retirement bridge has not been implemented/tested; future six-platform archives are unknown; exact fixture-comparability helper correctness must be verified in its real installed context. General metric-set schema closure, test-floor redundancy, archive ceiling justification and empirical CI-cost optimization are separate bounded seeds. No source provides a scientifically justified replacement numeric quota for this repository.
