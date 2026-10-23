---
phase: 05-pipeline-tune-up
verified: 2026-10-23T01:49:01Z
status: passed
score: 20/20 must-haves verified
source_head: 8251320
implementation_head: 098bb5e767b35e34e8f6ffb9545612a4d9d594e8
scope: final-goal-verification-under-owner-approved-D-35
covered_files: [".github/workflows/be-equality.yml",".github/workflows/ci.yml",".github/workflows/digest.yml",".github/workflows/nightly.yml",".github/workflows/release.yml",".planning/phases/05-pipeline-tune-up/05-01-PLAN.md",".planning/phases/05-pipeline-tune-up/05-01-SUMMARY.md",".planning/phases/05-pipeline-tune-up/05-02-PLAN.md",".planning/phases/05-pipeline-tune-up/05-02-SUMMARY.md",".planning/phases/05-pipeline-tune-up/05-03-PLAN.md",".planning/phases/05-pipeline-tune-up/05-03-SUMMARY.md",".planning/phases/05-pipeline-tune-up/05-04-PLAN.md",".planning/phases/05-pipeline-tune-up/05-04-SUMMARY.md",".planning/phases/05-pipeline-tune-up/05-05-PLAN.md",".planning/phases/05-pipeline-tune-up/05-05-SUMMARY.md",".planning/phases/05-pipeline-tune-up/05-06-PLAN.md",".planning/phases/05-pipeline-tune-up/05-06-SUMMARY.md",".planning/phases/05-pipeline-tune-up/05-07-PLAN.md",".planning/phases/05-pipeline-tune-up/05-07-SUMMARY.md",".planning/phases/05-pipeline-tune-up/05-08-PLAN.md",".planning/phases/05-pipeline-tune-up/05-08-SUMMARY.md",".planning/phases/05-pipeline-tune-up/05-09-PLAN.md",".planning/phases/05-pipeline-tune-up/05-09-SUMMARY.md",".planning/phases/05-pipeline-tune-up/05-10-PLAN.md",".planning/phases/05-pipeline-tune-up/05-10-SUMMARY.md",".planning/phases/05-pipeline-tune-up/05-11-PLAN.md",".planning/phases/05-pipeline-tune-up/05-11-SUMMARY.md",".planning/phases/05-pipeline-tune-up/05-12-PLAN.md",".planning/phases/05-pipeline-tune-up/05-12-SUMMARY.md",".planning/phases/05-pipeline-tune-up/05-13-PLAN.md",".planning/phases/05-pipeline-tune-up/05-13-SUMMARY.md",".planning/phases/05-pipeline-tune-up/05-14-PLAN.md",".planning/phases/05-pipeline-tune-up/05-14-SUMMARY.md",".planning/phases/05-pipeline-tune-up/05-15-PLAN.md",".planning/phases/05-pipeline-tune-up/05-15-SUMMARY.md","AGENTS.md","CMakePresets.json","README.md","cmake/toolchains/ppc-linux-gnu.cmake","docs/emulation-tests.md","docs/hardware/audio.md","docs/maintaining.md","docs/quality.md","docs/testing.md","pins.txt","quality.json","scripts/digest.sh","scripts/publish.sh","src/synth.c","tests/audio/tests.cmake","tests/built/tests.cmake","tests/cmake/archive_contents.cmake","tests/cmake/ci_required.cmake","tests/cmake/gate_count.cmake","tests/cmake/lane_equality.cmake","tests/cmake/quality.cmake","tests/cmake/quality_evidence.cmake","tests/cmake/quality_join.cmake","tests/compat/allow.txt","tests/core/test_core.c","tests/hygiene/tests.cmake","tests/install/tests.cmake","tests/inventory/ci.txt","tests/perf/tests.cmake","tests/policy/digest.cmake","tests/policy/gate-lines.txt","tests/policy/publish.cmake","tests/policy/quality.cmake","tests/policy/tests.cmake","tests/runner/tests.cmake"]
covered_digest: "v3:sha256:f3288d85a767f0457de03f411ba52526bb79ba46fa5834b4ea6e430388f9bea7"
behavior_unverified: 0
overrides_applied: 0
human_verification: []
re_verification:
  previous_status: gaps_found
  previous_score: not-scored
  previous_report_commit: a2daeb660fd7ca74912fa1a8dd7c58617b863e16
  gaps_closed:
    - Complete PowerPC archive satisfies unchanged undefined-symbol and no-writable-global ABI contracts.
    - One immutable current commit completes exact native and PowerPC normal/package inventories and two-file hash equality.
  gaps_remaining: []
  regressions: []
decision_coverage:
  honored: 35
  total: 35
  not_honored: []
advisory:
  - finding: WR-01 delayed-release independent-base edge remains deferred.
    category: other
    reason: AGENTS7 and D-35 keep this tooling review finding a seed; preserve equal-base rejection and do not claim the future delayed-push case was repaired.
    evidence_status: Reproduced in 05-REVIEW.md; current direct and called PR paths have a distinct authenticated actual base.
  - finding: COST-01 raw monthly execution-cost totals are untrusted.
    category: other
    reason: The actual reporter counts copied retry timestamps and can subtract skipped negative intervals; this is a deferred tooling seed under AGENTS7/D-35, not a claimed repair.
    evidence_status: Verifier independently reproduced the actual jq filter counting two real 209-second API rows as 418 seconds.
metadata_flags:
  unclassified_probe_rows:
    - TUNE-01
    - TUNE-03
    - TUNE-04
    - TUNE-05
  disposition: disclosed-unresolved-metadata
  owner_uat: false
prohibition_flags:
  - statement: MUST NOT turn absent hosted evidence into a zero, passing metric or claimed long-term redundancy history.
    status: flagged-unverified
    verification: descriptorless
  - statement: MUST NOT buy apparent gate-budget compliance with hidden controls, moved predicates, source padding, line density or a convenient dependency.
    status: flagged-unverified
    verification: descriptorless
  - statement: MUST NOT claim test-card controls protect unmeasured gameplay or label an unqualified portability lane supported.
    status: flagged-unverified
    verification: descriptorless
  - statement: MUST NOT present a cancelled or interrupted integration run as successful replacement coverage, a complete inventory, or a successful timing sample.
    status: flagged-unverified
    verification: descriptorless
---

# Phase 5: Pipeline tune-up Verification Report

**Phase Goal:** Make the next capabilities ship through a measured, reliable pipeline with current docs and proportional gates.
**Verified:** 2026-10-23T01:49:01Z
**Status:** passed, with the disclosed tooling warnings and metadata flags below.
**Re-verification:** Yes — both historical recovery gaps closed; full final phase goal also checked.

This report explicitly supersedes the current planner-authored interrupted-05-03 recovery input. Its original content remains available at commit `a2daeb660fd7ca74912fa1a8dd7c58617b863e16`; `git cat-file -e COMMIT:.planning/phases/05-pipeline-tune-up/05-VERIFICATION.md` exits 0. That input says `verifier_ran: false` and has no prior verified timestamp or phase-wide score. It is not retroactively described as a failed full-phase audit.

The contract is the current ROADMAP's four success criteria, TUNE-01–05, active PLAN must-haves and CONTEXT D-01–35. D-35 explicitly replaces the older 300/384 safety allocations with maximum512/fixes542/N2482/C15365/strict15353, freezes existing15/09, and requires final tightening. D-21–26 explicitly defer domain-policy centralization. These are scope decisions, not verifier-created waivers. Older numerical requirements are checked at their historical slices and are not applied as present limits.

Nine active outcome slots are accepted:01,02,10,04,05,13,07,15,09. Historical03/06/11/12/08 remain halted/superseded;14 remains halted1/3 with its accepted Task1. The runtime's ten canonical entries include14 and do not constitute ten accepted plans. This verification changes no implementation, scheduling history, review/gap allowance, remote release or repository setting.

## Goal Achievement

### Observable Truths

Roadmap criteria are retained verbatim in rows1–4. Rows5–20 consolidate overlapping active PLAN truths without removing their substantive requirements. Metadata-only classifier/descriptor flags retain their original status in the separate section below.

| # | Truth | Status | Evidence |
|---|---|---|---|
|1|Weekly triage and monthly CI-efficiency evidence from hosted runs.|VERIFIED, scoped warning|Actual `digest.yml` Monday06:00 schedule, read-only preview, artifact/summary, guarded rolling-issue publisher; live final preview run39879193514 succeeds. Actual API-derived evidence exists. COST-01 means raw numeric cost totals are untrusted; first default-branch publication belongs to ship.|
|2|Measured timeout/workload bounds and repaired audio/freshness reliability, with retained signals.|VERIFIED|Eight600-frame driver/runner pairs remain; measured250s/COST125 audio bound and provisional image/caller cohorts; distinct freshness outputs. Verifier's20 parallel/random repetitions and fresh610+6 workflow pass. Failed/censored immutable-base attempts remain disclosed.|
|3|An ordinary aggregate quality ratchet, explicit legacy domain limits, controlled performance arming and audited final line budget.|VERIFIED|Real typed policy/current/base joins pass for authenticated direct9/called8; original-item authentication precedes reduction. Independent domain judges/stores remain active. Current perf actually armed/PASS. Independently counted15287 physical lines equals sole ceiling.|
|4|Accurate docs/install/settings and evidenced improvement in the weakest dimension.|VERIFIED|README/docs read against code; genuine rehearsal installation succeeds; read-only settings command exits0/no drift;19 document budgets pass. ABBA timing receipts with exact XML membership show45.6775→37.3475s local wall,18.24% improvement; CI/CD faults receive implemented fixes.|
|5|One safe weekly delivery path retains monthly evidence, actionable triage and manual recovery without a new service.|VERIFIED|D-01–04: script handles issue/PR/nightly/release/runner-image categories; complete pages/attempts and file-based bodies; publisher serialization/default-ref guard; active digest control passes. Delivery is implemented; merged-default publication remains a ship observation.|
|6|PR previews cannot publish or read App credentials; required API/write failures and duplicate digest state fail visibly.|VERIFIED|Separate least-privilege preview/publisher jobs, no App references in digest path; `policy.digest.controls` executes production script through hostile-title, denied event/ref/input, failed API/write, duplicate, monthly-preservation and empty-history cases.|
|7|Concurrent freshness writers have disjoint output ownership; exact stale-page failure remains meaningful.|VERIFIED|`policy/compatibility.md` and `policy/compat_doc_fixture/generated.md` are separate; stale CHECK fixture remains independent. Verifier repeat20/parallel2/random exits0; no lock or retry masks shared writes.|
|8|Ordering and empty evidence do not create green no-work results; exact executed membership survives schedule changes.|VERIFIED|Fresh local exact610+6, hygiene24/library43 judges; authenticated remote native/BE610+6 each. ABBA XML each610 unique expected names. Full control suites reject missing/duplicate/skipped/empty evidence; publisher queue requires complete valid zero-match pages before absence/create.|
|9|All six native build/test/package legs and PR BE cross-compilation remain required; reduced PR BE execution is replaced by full same-SHA nightly/release proof.|VERIFIED|`CI required` exact dependency predicate and controls, COUNT6 native comparison, `be32-compile`; shared read-only `be-equality.yml` runs full normal/package inventories. Actual called producer120172173115 supplies current full BE/native execution.|
|10|The shipped PowerPC archive passes unchanged ABI judges and meaningful fault controls with bit-identical signed rounding.|VERIFIED|Unsigned magnitude rounding in `src/synth.c`, literal sign/tie/boundary cases in existing `core.audio.silence`, target-only read-only constant placement. Native named test and full suite pass; authenticated full PowerPC JUnit includes actual ABI positives/controls and audio execution. ABI allowlists/judges were not relaxed.|
|11|Exactly two independent nonempty native/BE hashes agree at an immutable identical SHA/configuration, with no skipped or duplicate test execution.|VERIFIED|Fresh verifier re-download digest/member checks, exact610+6 JUnit judges for both lanes, `lane_equality.cmake EXPECTED_SHA=098bb5e...` and COUNT2 pass. Both identities contain the same semantic fixture/configuration content.|
|12|One ordinary aggregate applies real current measurements and trusted independent baselines under the sole required roll-up.|VERIFIED|`quality_evidence→quality_join→quality` is actually called by CI. Live actual base65b360d/tree0f712e7 lacks quality.json; direct tested merge has base/raw-head parents. Fresh original24 ZIP/member authentication and both production joins exit0.|
|13|Ordinary policy identity, floor/ceiling monotonicity, actual values, independent introductions and bounded dated reasons fail closed.|VERIFIED|`policy.quality.controls` fresh named test and complete workflow pass: deletion/type/SHA/configuration, measured regressions, independent introduction/forgery, calendar/lifetime/future/inherited expiry, renewal/conversion, omitted lanes and existing-bound loosening. Present-base changes cannot use structural first adoption.|
|14|Every original observation authenticates before extrema; unavailable required evidence cannot consume stale numeric state or turn into an invented success.|VERIFIED|Shared `quality_observation` checks built SHA/source/identity/status; join reads NUMBER only in measured branch; aggregate handles only authorized unavailable cases before numeric read. Existing forged-SHA/missing/environment/regression fixtures and actual9/8 joins pass.|
|15|Ordinary bounds and each legacy domain have one active authority; unconditional current compatibility equality and legacy limitations remain explicit.|VERIFIED|`quality.json` contains ordinary metrics only; perf still uses `tests/perf/allow.txt`, library still uses `tests/compat/allow.txt` with verified BASE. `compat_ratchet` checks fresh key/status/evidence both ways before pass retention. Generated-corpus43 results and flipped-row control pass; spent POKEY row removal has accepted-base/replacement-pass evidence. Merged waived nonpass can become next base; no immutable domain grants are claimed.|
|16|Perf arming follows two fresh zero-spread triples, numeric PR base and actual nightly forced-failure/recovery; unqualified lanes stay advisory.|VERIFIED|Verifier reads actual control-job logs120026721454/120031254369: each3837262×3/spread0. Forced39836662246 fails; recovery39837920876 succeeds and issue21 is closed. Current actual perf job120172220231 has `MUHTARI_PERF_ARMED: true`, passed through `-DARMED`, numeric3837262 base/head and PASS; repeated base matches. WASM9/library5/property2 qualifying-main audit remains below10; no promotion.|
|17|Perf retains exact arithmetic, comparator input/exception faults and honestly limited test-card scope.|VERIFIED|All seven `perf.gate.*` cases pass in fresh complete CI; +1.00% accepts/+1.01% rejects armed and reports unarmed, unstable/missing/zero/negative/wrong-ROM/malformed-allowance/NO_BASE checks remain. Equal-length paths,`env -i`, warm-up subtraction and matching-tool duplicate base are actual workflow steps. No DMA/TIA/POKEY/gameplay protection is claimed.|
|18|Complete physical accounting and measured retained-fault savings precede scoped additions; final unused allowance retires.|VERIFIED|Verifier independently recomputes all134 starting entries at frozenf0202e1: B12885, no mismatches. Archive set construction saves2 lines with same packaged/negative consumers. All147 current whole files count15287; components368/193/447/919/477/reserve0, N2404; D-35 maximum15365 retires78. No hidden range/comment exclusion or source-ratio ceiling.|
|19|Supported patched CLI and full repo/signer/ref identity guard actual publication and downloads; checked workflow outputs cannot mask failed prerequisites.|VERIFIED|Both attestation jobs fetch SHA256-pinned gh2.102.0 before PATH use; common runtime tuple guard and full signer/ref arguments precede PATCH. Actual extracted workflow-output controls, version/signer/ref/tamper/download controls pass. Genuine job120183602427 records2.102.0,18 archives/19 assets and successful changed-copy rejection and downloaded verification.|
|20|Complete release lookup/HTTP/retained-ID cleanup works, confirms release absence before tag deletion, and receives current genuine release/install/cleanup evidence before09 acceptance.|VERIFIED|Production status/body sentinel preserves empty204. Real-script matrix checks typed pages/IDs, LF/CRLF/status contradictions, failure short-circuiting, known-ID and absent/surviving resources. Genuine rehearsal publishes immutable never-latest prerelease421353911, installs and cleans up. Verifier's fresh successful complete listing then exact ID/tag404 confirms absence; latest remainsv0.4.0.|

**Score:**20/20 substantive truths verified;0 present-but-behavior-unverified. Runtime-dependent truths have executed production-path controls or directly observed authenticated behavior, not presence alone. Ordinary test input setup supplies real caller inputs; no new undeclared-precondition, incidental-ordering or fixture-only reliance finding is identified.

### Scope, retained flags and human verification

No owner UAT is required. This is pipeline infrastructure; AGENTS6 assigns source/doc/API/CI judgments to the agent. There are no deferred `<human-check>` blocks and no explicit `verification: backstop` truths in active plans.

Four unclassified probe rows remain **disclosed unresolved metadata**:TUNE-01/03/04/05. The three TUNE-02 rows have explicit adjacency, empty and ordering criteria with evidence in rows7/8/20. Historical seven-row statuses are preserved. The four distinct authored descriptorless prohibition statements are copied in frontmatter and remain **flagged-unverified** as descriptors: no wired negative test or classifier resolution is fabricated. Their substantive evidence boundaries were independently audited above. Under the explicit owner scope these metadata statuses do not invent additional behavior requirements or owner UAT, and are not counted as silently passing checks. The original complete384 fit forecast is superseded by D-35 and the actual447/512 safety measurement; the old forecast is not called proven.

Domain-policy centralization remains explicitly deferred by D-21–26 and current TUNE-03. This is an accepted scope boundary, not a later-phase promise or an unresolved implementation gap. No other observed substantive gap was deferred to unrelated cartridge/save/controller/PAL phases.

### Advisory

| Finding | Evidence | Disposition and practical limit |
|---|---|---|
|WR-01 delayed-release base selection|Existing real-adapter reproduction rejects selected releaseR when event.before alsoR.|WARNING/tooling seed under AGENTS7/D-35;0 open/2 deferred in disposition. Valid future delayed-push case is not repaired. Current PR base is demonstrably distinct and both callers pass. Retain equal-base rejection.|
|COST-01 monthly execution accounting|Verifier re-read real API jobs120172221128/120180716506 and executed the actual monthly jq program: copied209s interval is counted twice as418s; copy.created_at follows original completion.|WARNING/tooling seed under AGENTS7/D-35. Raw450-run preview numeric total4124.1333minutes is **UNTRUSTED**, not accepted cost evidence. One-off final cost audit excludes confirmed copied/skipped/negative placeholders and retains real failures/cancellations. Reporter is **not fixed**.|
|Forged smaller-lane fixture wording|The fixture does reject exact forged SHA, but starts with an equal-valued observation.|Retained pre-existing tooling seed: it proves identity rejection, not the claimed strictly smaller-value-extrema scenario. Authentication is visibly before reduction in production. No new acceptance requirement or round.|

There are no newly discovered unevidenced blocker candidates. The two documented review warnings stay scoped seeds because the owner's rule explicitly says review findings about test/tooling code become seeds, not requirements. This report does not misuse the generic convergence evidence gate to claim a reproduced bug is unevidenced or fixed.

### Required Artifacts

The artifact query exits0 for all nine active plans. Existence was followed by substantive code and actual consumer inspection:

| Artifact | Expected | Status | Substance and consumer |
|---|---|---|---|
|`scripts/digest.sh`|Hosted collection, triage/monthly rendering and guarded issue delivery|VERIFIED with COST-01 warning|Real API/pages/attempts and file-based report body; invoked from preview/publish jobs and fault harness.|
|`.github/workflows/digest.yml`|Weekly read-only preview plus trusted serialized publisher|VERIFIED|Cron/manual/PR paths, artifact, job summary, separate permissions and default-ref condition.|
|`tests/policy/digest.cmake`|Meaningful recurring reporter controls|VERIFIED|Runs real shell script, cause-specific denied/failure and preservation assertions; fresh full CI executes it.|
|`tests/cmake/gate_count.cmake`|Physical observations and malformed/missing/overlap rejection|VERIFIED|Real whole-file reads and integer totals; counter/manifest inclusion enforced; called by quality evidence and active controls.|
|`tests/policy/gate-lines.txt`|Complete physical boundary including controls/glue/self|VERIFIED|147 unique whole files; independently matched audited rows; comments/blanks included.|
|`quality.json`|Single ordinary metric/reason authority and final ceiling|VERIFIED|12 real ordinary metrics; gate ceiling15287/N2404; consumed by real aggregate and trusted-base comparison.|
|`tests/cmake/quality.cmake`|Typed ordinary policy and actual-value judge|VERIFIED|236 lines of typed/date/monotonicity/measurement logic; invoked by join and production fault harness.|
|`tests/cmake/quality_evidence.cmake`|Attributable real producer/current/base observations|VERIFIED|Actual JUnit/inventory/archive/cache/tool/fixture outputs; independent immutable-base build and policy lookup; no invented optional values.|
|`tests/cmake/quality_join.cmake`|Exact artifact membership and original authentication|VERIFIED|8/9 membership,4 members per ordinary lane, trusted base/policy/environment, per-item validation before extrema, actual judge invocation.|
|`tests/policy/quality.cmake`|Real judge and adapter/join adversarial controls|VERIFIED|Fresh named and full execution passes; assertions check exact exit/diagnostic and values, not file existence only.|
|`.github/workflows/ci.yml` and `tests/cmake/ci_required.cmake`|One required aggregate with exact roll-up|VERIFIED|Actual quality job dependencies and exact needs predicate; omission/failure/allowed-skip controls stay active.|
|`CMakePresets.json` and measured test registrations|Presets, timeout scales, costly-first bounded work|VERIFIED|Native/BE compile/full/release entrypoints; all610 normal+6 release retained; eight audio corpus fixtures remain.|
|`src/synth.c`, `tests/core/test_core.c`, `docs/hardware/audio.md`|Bit-preserving bounded signed arithmetic and regression coverage|VERIFIED|Unsigned dynamic division, literal both-sign boundary results, production synthesis checks; source/docs/callers agree.|
|`cmake/toolchains/ppc-linux-gnu.cmake`|Read-only generated constant placement|VERIFIED|Target-only `-mno-readonly-in-sdata`; actual full PowerPC ABI judges/controls pass.|
|`.github/workflows/be-equality.yml` and `tests/cmake/lane_equality.cmake`|Complete identical-head/fixture native/BE evidence|VERIFIED|Normal→package→release sequence, exact JUnit, independent identities and COUNT2; shared callers actually execute.|
|`tests/cmake/perf_compare.cmake`, `tests/perf/tests.cmake`, retained allow stores|Independent semantic authorities|VERIFIED|Comparator/store restored to original contract;7 boundary/fault cases active; compat fresh-results equality remains unconditional.|
|`scripts/publish.sh`|Guarded signer/runtime and fail-visible lifecycle|VERIFIED|253 substantive shell lines, explicit checked statuses and state, complete listings and exact absence; invoked by real release jobs.|
|`tests/policy/publish.cmake`|Real-script lifecycle/output/runtime/download controls|VERIFIED|Argument-aware response queue and actual extracted caller blocks; positive paths and forbidden-call assertions; fresh control passes23.28s locally.|
|`pins.txt` and `.github/workflows/release.yml`|Verified patched runtime and genuine lifecycle integration|VERIFIED|Official gh2.102.0 pinned SHA256, fetch-before-use in both jobs; current real rehearsal/install/cleanup all succeed.|
|`README.md`, `docs/testing.md`, `docs/emulation-tests.md`, `docs/maintaining.md`, `docs/quality.md`|Current user/maintainer claims within budgets|VERIFIED|Code/docs comparison, exact lanes/audio/retry/ref/cleanup/legacy-policy limits;19-file budget check and actual README release installation.|
|All active PLAN/SUMMARY records|Allocation/history/receipt index|VERIFIED as records only|Completion assessed through code/API/downloaded outputs, not filename/status claims. All historical halted files remain intact.|

### Key Link Verification

The literal key-link helper exits1 for several prose-valued `from/to` entries and indirect CMake variable/caller references. Those are parser limitations, not proof of missing wiring. Every actual connection was followed:

| From | To | Via | Status |
|---|---|---|---|
|Digest preview/publisher jobs|Real reporter→GitHub APIs→artifact/summary/one digest issue|Explicit shell invocations; checked paginated data and body-file mutations; guarded issue-write job|WIRED|
|Policy freshness and its self-test|compat_doc generator|Different OUT files and separate stale CHECK; exact diagnostic|WIRED|
|Presets|MuhtariTests registration/scales|Configure presets→root build→test include; measured TIMEOUT/COST|WIRED|
|Release test registration|archive_contents judge|Existing `archive_cmd` variables point to actual packaged ZIPs and controlled corrupt fixture|WIRED|
|Native/hygiene/ASan producer steps|quality_evidence adapter|Completed workflow outputs plus independently built unchanged event base; four-member artifacts|WIRED|
|Aggregate CI quality job|quality_join→quality judge|Actual trusted Git policy and exact own-run artifacts; failed exit propagates|WIRED|
|Quality job|CI required|Explicit needs membership and exact success/allowed-skip predicate with recurring controls|WIRED|
|Perf job|Existing comparator and ALLOW|Numeric head/base/base2 and actual armed variable; threshold/input controls retained|WIRED|
|Library caller|Existing compat judge and ALLOW|Fresh result file, ledger, verified event base; flipped-row fault control|WIRED|
|Synth rounding|Core archive→native/PowerPC audio tests|Interpolation and PCM callers; old ABI judges inspect actual archive|WIRED|
|Nightly and release equality callers|Read-only be-equality workflow|Explicit immutable ref; native/BE both built and compared within that checkout|WIRED|
|Publisher workflow|Pinned fetcher→version guard→attested/verify→PATCH|Both actual job blocks; full repo/signer/ref and tamper controls before publication|WIRED|
|Cleanup|Complete release list→retained-ID resource→tag resource|Checked HTTP/status/body; release absent before tag mutation; fresh final list/ID/tag confirmations|WIRED|
|README install instruction|install_line judge|Actual script extracts and executes the user command against built/latest/rehearsal artifacts|WIRED|

### Data-Flow Trace (Level 4)

| Artifact/output | Real source | Flow | Status |
|---|---|---|---|
|Digest triage/monthly report|Hosted public repository/run/attempt/job/issue/PR APIs|Required API collection→evidence.json→escaped Markdown→artifact/summary/guarded publisher|FLOWING; COST-01 numeric-cost interpretation excluded|
|Ordinary quality report|Exact executed JUnit, validated archive bytes, compiler/cache/fixture identity, unchanged trusted-base builds|Per-lane actual observations→original validation→worst value→typed bound/reason judge|FLOWING|
|Direct Ir report|Real Cachegrind head and two unchanged-base360/60 runs|Numeric3837262 at each measurement→armed semantic comparator→quality-perf artifact|FLOWING, test-card only|
|Full BE equality|Actual native/PowerPC executed tests, same checkout identities and hashes|610+6 per lane→exact JUnit→COUNT2 identity/hash comparison|FLOWING|
|Release verification|Actual18 built ZIPs,6 commit files, attestation service, immutable temporary release and downloads|prepare→upload19→attest/guard→PATCH→download/hash/attestation→README install→confirmed cleanup|FLOWING|

No rendered metric ends at a hardcoded empty fallback. Explicit unavailable optional dimensions remain unavailable. Zero findings derive from completed corresponding real judges and exact inventories.

### Behavioral Spot-Checks and Receipts

All commands below were executed/read independently in this verification unless explicitly labelled retained evidence. Local checks have no hosted run URL.

| Behavior | Command/check | Exit/result | Status |
|---|---|---|---|
|Whole current native workflow|`cmake --workflow --preset ci` (once)|0;610 normal+6 release; publisher control23.28s; eight audio pairs preserved|PASS|
|Signed arithmetic|`ctest --preset ci -R '^core[.]audio[.]silence$' --no-tests=error --output-on-failure`|0; literal sign/tie/boundary and real synthesis path|PASS|
|Ordinary policy behavior|`ctest --preset ci -R '^policy[.]quality[.]controls$' --no-tests=error --output-on-failure`|0; actual typed judge/adapter/join cases|PASS|
|Concurrent freshness|`ctest --preset ci -R '^policy[.]compat_doc_fresh([.]self_test)?$' --parallel 2 --schedule-random --repeat until-fail:20 --output-junit /tmp/muhtari-verify5-freshness.xml --no-tests=error --output-on-failure`|0;20 of each with disjoint outputs and exact stale fixture rejection|PASS|
|Exact local executed inventories|`cmake -DJUNIT=build/ci/junit.xml -DINVENTORY=tests/inventory/ci.txt -DEXCLUDE=release -P tests/cmake/check_junit.cmake`; corresponding release/library/hygiene selections|0 each;610/6/43/24 exactly once|PASS|
|Physical boundary|Independent Python newline+unterminated-line count of147 whole files and134 `git show f0202e1:path` starting rows|0;15287 now;12885 initial; every audited row matches|PASS|
|Document budgets and settings|`cmake -DROOT="$PWD" -P tests/cmake/budgets.cmake`; `sh scripts/apply-repo-settings.sh --check`|0 each;19 files; no settings drift|PASS|
|Tree and current diff hygiene|`sh scripts/hygiene.sh --tree`; `git diff --check`|0 each|PASS|
|Final run identity|`gh api repos/szTheory/muhtari/actions/runs/RUN` for direct39879193530/called39879193669|0;completed/success;attempt1/2;raw head098bb5e|PASS|
|Actual artifact authentication|Fresh `gh api actions/runs/RUN/artifacts --paginate --slurp`, attempt-job listings and every selected artifact ZIP in memory; SHA256, exact member bytes and original producer matching|0;direct15=9 quality+6 hash;called9=8 quality+1 equality; all24 authenticated|PASS|
|Actual ordinary joins|Production `quality_join.cmake` with each authenticated directory, independent BASE_POLICY, actual producer/base/source and PERF_REQUIRED ON/OFF|0 each;exact9/8 accepted|PASS|
|Full current BE/native execution|Production `check_junit.cmake` on all4 authenticated downloaded XML files; `lane_equality.cmake`; `hashes.cmake COUNT=6`|0 each;610+6 each;2/6 identical files|PASS|
|Perf controls/arming|Live job-log API120026721454/120031254369 and current120172220231; current script arguments/flag checked|0;two zero-spread triples and actual armed numeric PASS; correct log format is `MUHTARI_PERF_ARMED: true`|PASS|
|Nightly recovery|Live run APIs39836662246/39837920876 and issue21|0;intentional failure then success; issue CLOSED|PASS|
|Genuine lifecycle|Live jobs120183602427/120185505590/120185554406 plus actual retained producer log|0;rehearsal/install/cleanup and every required step succeeded|PASS|
|Fresh exact absence|Successful complete authenticated release-list API; then exact421353911/tag APIs with included HTTP headers|0 verifier check;both resources404;latestv0.4.0|PASS|
|Cost warning reproduction|Live original/copy job API plus actual `scripts/digest.sh` monthly jq program, run on those records|0 reproduction;418s emitted for one209s execution plus metadata copy|WARNING, preserved seed|

The initial selected-test runs wrote CTest's preset JUnit output. An attempted full-inventory check correctly rejected that one-test report; it was a verifier artifact-selection mistake, not a code failure. The one complete workflow above regenerated the full receipt, after which exact610+6 passed. The later repetition uses its own explicit JUnit path. An initial log assertion expected the wrong textual armed format; the corrected check verifies the actual logged environment plus argument forwarding and PASS. Neither failed verifier assertion is represented as passing product evidence.

### Probe Execution

No active PLAN declares a conventional `scripts/*/tests/probe-*.sh`, and discovery finds none. Required runnable verification was executed through the actual CMake control/workflow/judge entrypoints above; SUMMARY PASS markers were not substituted for probe execution. Ignored one-off evidence helpers were inspected, and their artifact/identity/cost assertions were independently rerun without relying on their JSON claims. No server or service was started; no remote CI was rerun or release mutated.

### Requirements Coverage

| Requirement | Active source plans | Description/outcome | Status | Evidence/limit |
|---|---|---|---|---|
|TUNE-01|04/09|Weekly rolling digest and monthly hosted efficiency evidence without new service|SATISFIED within accepted scope|Real current preview and guarded delivery path; COST-01 explicitly excludes raw numeric-cost correctness; first default-branch publication at ship.|
|TUNE-02|01/02/10/04/13/15/09|Measured bounds, reproduced fault repairs and retained signals|SATISFIED|Disjoint writers, eight audio cases, actual native/BE inventories, bounded current controls and matched ordering experiment; provisional/censored samples disclosed.|
|TUNE-03|01/05/13/15/09|Ordinary measured ratchet, explicit domain stores/limits and audited fixed final count|SATISFIED|Authenticated independent-base9/8 joins, typed policy controls,147 whole files and tightened15287, retired78. Central domain policies remain explicitly out of scope.|
|TUNE-04|10/05/13/07/09|Controlled arming and honest supported/advisory portability|SATISFIED|Two actual triples, current numeric armed PASS, forced nightly recovery, full BE; unearned lanes stay advisory.|
|TUNE-05|01/02/10/15/09|Current docs/install/settings and weakest-dimension improvement|SATISFIED|Actual patched-gh lifecycle/README install/settings; source comparison/budgets; substantive reliability fixes and exact-workload18.24% local latency improvement.|

All five IDs mapped to Phase5 appear in active plans; no orphaned phase requirement. Completion checkboxes/tracking are for the orchestrator to reconcile after this verdict; SUMMARY statuses were not used as behavioral proof.

### Test Quality Audit

| Test/control | Linked requirements | Active/skipped | Expected-value provenance and assertion strength | Verdict |
|---|---|---|---|---|
|Core signed-rounding/audio cases|TUNE-02/04/05|Active;0 skipped|Literal independently assessable mathematical expected results, both signs/boundaries plus production reconstruction and partition checks|Sufficient|
|Ordinary quality control dispatcher|TUNE-03|Active;0 skipped|Synthetic independent policy/base/current inputs; exact values, exit/diagnostic, expiry/state and real caller checks|Sufficient|
|Publisher control dispatcher|TUNE-02/05|Active;0 skipped|Real script + transport response queue; successful baselines, specific failure and forbidden mutation assertions; actual workflow output blocks|Sufficient for invocation/lifecycle; genuine hosted proof supplies cryptography|
|Perf comparator controls|TUNE-04|All7 active;0 skipped|Independent integer boundary/input/exception fixtures, armed/unarmed outcomes|Sufficient|
|Digest controls|TUNE-01|Active;0 skipped|Real script + synthetic paginated attempts, privilege/failure/preservation/hostile-input cases|Useful but incomplete for real copied retry/negative skipped semantics: COST-01|
|Executed-inventory/hash/ABI/archive judges and controls|TUNE-02/03/04/05|Active exact current selections;0 skipped|Missing/corrupt/writable/libc/float/member/hash fault fixtures plus real packages/archives/JUnit|Sufficient|

No required test is disabled; no new circular expected-output generation was found. The synthetic attestation transport does not prove upstream certificate semantics and is not credited for doing so. Authenticated genuine publication/download/tamper results provide that separate evidence. The disconfirmation pass found a partially met reporting dimension (raw numeric cost), a test limitation (equal-valued forged-lane fixture and omitted real retry semantics), and the deferred delayed-release error path. All retain explicit scoped dispositions rather than being hidden by the passing headline.

### Anti-Patterns Found

| File | Pattern | Severity | Impact |
|---|---|---|---|
|`scripts/digest.sh:164`|Raw timestamp aggregation accepts copied retry/negative skipped intervals|WARNING|COST-01; numeric raw cost unavailable for accepted conclusions.|
|`tests/cmake/quality_evidence.cmake:135`|Selected delayed release can equal event.before|WARNING|WR-01; valid future delayed-release case fails closed.|
|Changed implementation set|Unreferenced TBD/FIXME/XXX, TODO/HACK/PLACEHOLDER scan|None|No debt-marker blocker or hollow/unimplemented production path found.|
|Inherited broad-diff whitespace|Previously recorded EOF blank lines|INFO/retained seed|Current working diff check passes; no claim that the historical broad diff is whitespace-clean.|

### Immutable Hosted Evidence and Measured Limits

| Role | Run/attempt and producer | Independently checked result |
|---|---|---|
|Direct CI|[39879193530](https://github.com/szTheory/muhtari/actions/runs/39879193530),attempt1;tested merge`bbc4da9172ee3983d361f5c7e67dadda50d4ef79`|Success;parents are actual base65b360d and raw head098bb5e;9 quality+6 native hash artifacts authenticated; numeric armed perf.|
|Called/rehearsal|[39879193669](https://github.com/szTheory/muhtari/actions/runs/39879193669),attempt2;raw`098bb5e767b35e34e8f6ffb9545612a4d9d594e8`|Success;8 quality artifacts; full native/BE equality produced by original attempt1/job120172173115; current real rehearsal/install/cleanup.|
|Final preview|[39879193514](https://github.com/szTheory/muhtari/actions/runs/39879193514)|Final-source read-only report delivery; default-branch publish remains guarded.|
|Perf controls|[39836658273](https://github.com/szTheory/muhtari/actions/runs/39836658273),[39836675687](https://github.com/szTheory/muhtari/actions/runs/39836675687)|Each actual successful control log has3837262/3837262/3837262,spread0.|
|Nightly failure/recovery|[39836662246](https://github.com/szTheory/muhtari/actions/runs/39836662246),[39837920876](https://github.com/szTheory/muhtari/actions/runs/39837920876)|Intentional failure retained; recovery succeeds and issue21 closes.|

Actual trusted base is`65b360d1afd37b7dad9f94dcff497c85df84fd1d`, tree`0f712e7a9df091a3dbaf5bc176a538a0a5869cd5`; independent Git/API comparison proves the policy is absent there. Raw artifact ZIP IDs/digests/original producer attempts are fully enumerated in15 SUMMARY and independently reauthenticated here. Seven called quality artifacts retain original attempt1 attribution; Windows x64 artifact12470574438 belongs to actual attempt2/job120180715250. Original120172221417 passed current610+6 but failed unchanged-base fixtures; its failure is not erased or called repaired. Copied retry metadata is excluded from original execution attribution.

Genuine rehearsal job120183602427 used digest-verified gh2.102.0 (2026-09-30), archived raw098bb5e and intended signer source-ref`refs/pull/20/merge`. Retained release421353911 was observed with draft=false/prerelease=true/immutable=true and target098bb5e, published2026-10-23T01:15:31Z. It prepared18 ZIPs, uploaded19 assets, rejected modified copies, verified actual downloads/hashes/attestations, and was installed by120185505590 and cleaned by120185554406. The retained01:22:42UTC complete-list/exact404 receipt and this verifier's fresh read-only confirmation agree. Archive commit and attestation workflow-trigger identity remain distinct.

Matched local ABBA evidence executes the same610 names with two workers in all four samples:47.281/44.074s before versus37.910/36.785s publisher-first; means45.6775→37.3475s (18.24%). Summed test durations77.102396→74.412062s are occupied-test-time proxies. The committed COST725 applies the measured order. No overall hosted cost reduction is claimed: current direct3230job-seconds and called4729 include added work, different images/network and real failed base work. Genuine patched-gh successful-publication cohort n=1 is provisional; rehearsal372+install7+cleanup11=390 observed stage-seconds. Raw monthly preview totals remain untrusted under COST-01.

Final budget is B12885−S2+N2404=C15287. Approved maximum N2482/C15365/strict15353 is not renewed headroom:78 retires (safety65,BE12,quality1). Actual components count368/BE193/digest447/quality919/fixes477/reserve0 include safety447. Broad sensitivity15346 adds the unchanged59-line product RetroArch wrapper. Full/authored source8580/6533 and178.1701%/233.9966% are advisory; no old10% compliance is implied.

### Decision Coverage

All35 trackable CONTEXT decisions are honored by shipped artifacts; query exits0,35/35,none unhonored. This substring-based decision gate is advisory; actual contract/implementation/receipt verification above supplies the substantive proof.

### Human Verification Required

None. No behavior-dependent truth remains without executed evidence. Agent-verifiable judgments stay with the agent. Default-branch digest publication, final merge/head CI and post-merge release-PR health are AGENTS6/9 **ship checks**, not owner UAT or a second execute/review/gap round. No actual main product release is claimed by a disposable rehearsal.

### Gaps Summary

No substantive blocking phase gap remains under the owner-approved current contract. Both historical PowerPC recovery gaps close with current authenticated full execution and unchanged judges. The delivered pipeline has measured fault repairs, real independent quality inputs, bounded controls, true release-path proof, accurate scoped docs and a tightened fully counted budget. Deferred tooling warnings and unresolved descriptor metadata remain visible with their limits.

Stop before ship/merge. The next GSD command is `$gsd-ship 5`.

---

_Verified:2026-10-23T01:49:01Z_
_Verifier:the agent (gsd-verifier)_
