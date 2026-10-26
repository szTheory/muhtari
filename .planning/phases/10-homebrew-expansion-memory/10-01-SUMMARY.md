---
phase: 10-homebrew-expansion-memory
plan: "01"
subsystem: cartridge
tags: [bankset, bus, maria, diagnostics, c17]
status: complete
requires:
  - phase: 09-pal-games-and-audio
    provides: Existing NTSC/PAL execution, native audio taps and three-host diagnostics
provides:
  - Implemented narrow 32 KiB-per-set BankSet ROM execution
  - Actual-edge electrical selection and causal stretched-cycle burst origin
  - Independently judged library, runner and loaded-module tracer
affects: [10-02, bankset, expansion-memory, retained-observations]
tech-stack:
  added: []
  patterns:
    - Instance-owned electrical interval independent of HALT scheduling
    - Existing diagnostic mailbox and native/raw/PCM taps
key-files:
  created: []
  modified:
    - src/internal.h
    - src/bus.c
    - src/cart.c
    - src/maria.c
    - tests/bus/test_bus.c
    - tests/cart/test_cart.c
    - tests/diag/driver.c
    - tests/diag/tests.cmake
    - tests/inventory/ci.txt
    - docs/hardware/bus.md
    - docs/hardware/maria.md
    - src/instance.c
    - tests/core/test_core.c
    - tests/audio/expected.txt
    - tests/built/expected.txt
    - tests/cmake/hashes.cmake
    - tests/cmake/quality.cmake
    - tests/policy/quality.cmake
    - tests/policy/tests.cmake
    - tests/runner/tests.cmake
    - docs/quality.md
key-decisions:
  - Independently attribute each changed observation by reversing only the causal T0 clamp.
  - Advance behavior revision to6 and admit only the exact demonstrated expectation transitions.
  - Preserve the original halt and failed runs as history; close it only after complete current/base acceptance.
requirements-completed: []
requirements-acceptance: pending
coverage:
  - id: D1
    description: Narrow BankSet public load/frame execution with unequal CPU and MARIA native sentinels
    requirement: BOARD-04
    verification:
      - kind: unit
        ref: tests/bus/test_bus.c#halt_latency_two_cycles
        status: pass
      - kind: integration
        ref: diag.bankset_tracer
        status: pass
    human_judgment: false
  - id: D2
    description: Shared electrical ownership interval and four causal stretched-cycle schedules
    requirement: BOARD-05
    verification:
      - kind: unit
        ref: tests/bus/test_bus.c#halt_views
        status: pass
      - kind: unit
        ref: tests/bus/test_bus.c#halt_store_order
        status: pass
    human_judgment: false
  - id: D3
    description: Actual runner and loaded-module numeric, native, raw audio, PCM and count equality
    requirement: BOARD-04
    verification:
      - kind: integration
        ref: diag.bankset_tracer
        status: pass
    human_judgment: false
  - id: D4
    description: Complete required regression workflow and authenticated candidate/preplan package observations
    requirement: BOARD-04
    verification:
      - kind: integration
        ref: cmake --workflow --preset ci
        status: pass
      - kind: other
        ref: tests/cmake/quality_evidence.cmake
        status: pass
    human_judgment: false
  - id: D5
    description: Independently attributed slow-edge outputs, exact release regressions and protected behavior revision6
    requirement: BOARD-04
    verification:
      - kind: unit
        ref: tests/bus/test_bus.c#halt_stretched_costs
        status: pass
      - kind: integration
        ref: policy.quality.controls
        status: pass
      - kind: integration
        ref: audio.samples and runner.hashes_file
        status: pass
      - kind: other
        ref: Disposable clamp-only differential build and actual transfer traces
        status: pass
    human_judgment: false
actuals:
  tokens: 17961
  tasks: 2
  commits: 8
plan_head_before: f84b4dd83f5a91a8f86c18bae2ec602ae8505540
plan_head_after: 59bad038be902f4d6879fe4513954a5a921b336d
duration: 4h 20m
started: 2026-10-26T13:08:26Z
completed: 2026-10-26
---

# Phase 10 Plan 01: Causal BankSet Tracer Summary

**Two 32 KiB ROM sets execute across all three hosts with causal ownership, independently attributed revision6 observations and passing fresh candidate/preplan package evidence.**

## Historical halt at 60b8917

The sections through the original Self-Check below retain the earlier halt and failed verification history. They are superseded by the recovery closure at the end. At that halt, both implementation tasks and their focused checks were complete, but the full workflow failed and blocked dependent plans. The earlier frontmatter copied contribution labels into requirements-completed; this is now corrected to an empty list. Neither BOARD-04 nor BOARD-05 is accepted by this narrow slice. No expansion or broader geometry/RAM claim follows from it.

The two classifier assumptions remain category unclassified, status unresolved, verification null and resolution null for BOARD-04/05, as in the plan. Four-row slow timing is a code-derived compatibility fit, not hardware measurement. No physical XM or universal game compatibility evidence is claimed.

## Accomplishments

- Public create/load/frame calls run unequal CPU/MARIA 32 KiB sets; CPU code reads its sentinel and actual native output contains the MARIA graphic's literal palette value.
- One instance-owned interval begins at the second actual phi2 fall and ends at HALTRST. CPU data commits before coincident handoff. Fast/fast, fast/slow, slow/fast and slow/slow rows retain second falls 8/9/10/11 and first fetches 8/10/10/12. Tests cover both selection boundaries, last-DLL sampling before release, CPU peeks without mutation, non-HALT VBE and absolute LRC aborts.
- The retained bus-only slow RIOT c0 / fast cartridge-store c1 counterexample observes the tick10 write and resulting graphics cell3. It is not advertised as an executed ROM instruction stream.
- diag.bankset_tracer generates owned bytes under the build directory. Six CPU reads overwrite poisoned numeric fields, checked against literal values81–86 and CRC$C84C. Native pixels are judged before comparison. Existing taps compare twelve actual library/runner/module calls, native bytes, canonical raw transitions, PCM and sample counts. Missing reports, count/identity/CRC faults, CRC-valid wrong values and CPU graphics substituted for MARIA graphics fail.

## Task commits and TDD evidence

1. Task1 RED: ef3dddd — public BankSet load returned9 instead of0 after real test execution.
2. Task1 GREEN: 6135e87 — actual-edge selection, timed MARIA fetches and four-row/ordering tests.
3. Task2 RED: 9a1825f — generated ROM executed and produced six records with identity20; the old numeric judge expected identity0 and rejected the valid report.
4. Task2 GREEN: e97d8d7 — admit diagnostic identity20 and document electrical/causal behavior.
5. Required correction: 1c251cb — preserve valid unsupported v4 declarations, update obsolete refusal coverage and fit the existing documentation budget.

Both RED commands exited8 on the intended named assertion after compilation. The current JUnit reports were submitted unchanged to `check tdd-red-evidence`, which returned RED_EVIDENCE_OK for each before GREEN. Semantic inspection confirmed real target execution, rather than a missing fixture or build failure. Initial Task1 test construction used the wrong media field names; that build error was corrected before collecting RED and was not counted as RED. Initial evidence submission used an out-of-project path and then a misspelled target key; those classifier refusals were corrected before production edits. No REFACTOR commit was needed. TDD_MODE was false, but task tdd attributes were honored.

## Verification evidence

Every run URL in this section is **N/A-local**. No log is committed. The FIFO helper invokes the unchanged workflow with four available local worker slots; workloads, assertions, presets and deadlines were not changed.

| Command | Exit | Result |
|---|---:|---|
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^bus[.]halt[.]latency_two_cycles$' --output-junit /tmp/muhtari-10-task1-red.xml --no-tests=error --output-on-failure` | 8 | Intentional public-load assertion failure before implementation |
| `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/ci/task1-red.json --raw` | 0 | RED_EVIDENCE_OK; public load refusal versus required success |
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(bus[.]\|maria[.])' --no-tests=error --output-on-failure` | 0 | 29/29; also rerun as tracer feedback gate before Task2 |
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^diag[.]bankset_tracer$' --output-junit build/ci/task2-red.xml --no-tests=error --output-on-failure` | 8 | Intentional executed-mailbox numeric identity assertion failure |
| `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/ci/task2-red.json --raw` | 0 | RED_EVIDENCE_OK; valid observed identity20 rejected by old judge |
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^diag[.]bankset_tracer$' --no-tests=error --output-on-failure` | 0 | Three actual deliverables and report/view fault controls pass |
| `sh build/ci/task08-jobserver-gate.sh` at e97d8d7 | 8 | Full workflow648/657 ordinary tests pass; nine failures retained |
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(bus[.]\|maria[.]\|diag[.]bankset_tracer\|cart[.]probe[.]unsupported_board\|cart[.]retail[.]boards\|docs[.]budgets)$' --output-junit /tmp/muhtari-10-correction.xml --no-tests=error --output-on-failure` | 0 | All33 selected checks pass after required corrections |
| `sh build/ci/task08-jobserver-gate.sh` at 1c251cb | 8 | Full unchanged workflow651/657 ordinary tests pass in32.00s; six historical timing failures remain |
| `cmake --build --preset ci --target package && ctest --preset ci-release --output-on-failure` at 1c251cb | 0 | Three actual candidate ZIPs and all6 release checks pass |
| `GITHUB_EVENT_PATH="$PWD/build/ci/task10-plan01-event.json" GITHUB_EVENT_NAME=push cmake -DROOT=. -DBUILD=build/ci -DOUT=build/ci/task10-plan01-quality-current.json -DSHA="$(git rev-parse HEAD)" -DCONFIG=ci -DBUILD_BASE=ON -DSOURCE=local -P tests/cmake/quality_evidence.cmake` | 1 | Existing producer rejects the six failed full-inventory tests before reporting bytes or building the base |
| `scripts/hygiene.sh --tree` | 0 | Clean tree hygiene; task hooks also ran on every commit |
| `git diff --check` | 0 | No whitespace errors |

The quality invocation's genuine requested comparison base is preplan f84b4dd83f5a91a8f86c18bae2ec602ae8505540, recorded by the plan ledger and a local event descriptor. It is not a hosted event or same-head comparison. An initial invocation supplied a mistyped full SHA and correctly failed built-SHA validation (exit1); the corrected invocation above reached inventory validation and failed honestly. No candidate/base package-byte acceptance exists. Earlier cached base JSON was not reused or relabeled. Hosted acceptance belongs to the final plan and was not attempted here.

## Remaining blocking evidence

The causal origin clamp changes established timing-dependent outputs. Neither run was relabeled passing:

- basic.colorfade, basic.fontcheck, basic.mousedemo and basic.screensafe fail their pinned frame-chain observations.
- audio.samples fails the pinned soundtest raw/PCM row, despite the independently checked capture remaining non-silent and clipping-free.
- runner.hashes_file fails the retained69-row aggregate: earlier numeric/digest bytes changed beyond the seven previously authorized revision annotations.

All original pinned observations are preserved. A disposition based on the new causal timing evidence is required before changing those contracts. Broadly regenerating them would contradict this plan's instruction to retain old observations. Restricting the correction to the new board would abandon the shared causal handoff described by the plan. No such architectural or acceptance change was inferred.

## Deviations from Plan

**1. [Rule1 - Bug] Valid unsupported BankSet v4 declarations fell back to v3.**
- Found during full verification: allowing one BankSet profile made the previous universal refusal predicate false, so A8 declarations silently became supported legacy boards.
- Fix: explicitly refuse unimplemented BankSet RAM/options and retain syntactically valid BankSet v4 identity before geometry validation.
- Files: src/cart.c, tests/cart/test_cart.c. Verification: cart.retail.boards and cart.probe.unsupported_board pass. Commit:1c251cb.
- The existing generic refused-hash test now uses still-refused Souper; new narrow BankSet acceptance is covered by the public-frame tracer. This directly required one test file beyond the declared list.

**2. [Rule1 - Bug] New MARIA prose exceeded its existing40KiB budget.**
- Condensed the changed handshake explanation while retaining the four-row table, all-fast arithmetic and confidence limits. docs/hardware/maria.md is40898/40960bytes; docs.budgets passes. Commit:1c251cb.

No new dependency, quota, public API declaration or tracking framework was introduced. No tracked file was deleted.

## Deferred Issues

The six failed required timing observations and absent passing current/base package measurements block this plan's acceptance. They are not optional tooling or owner UAT. Both best-effort `windows append` attempts (unmet-truth and unrun-verify) exited1 because the pre-existing ledger frontmatter says17open/35fixed while entries yield16open/36fixed. No ledger repair was attempted; this summary retains the defects for the orchestrator.

## Known Stubs and threat surface

No unfinished product stub was added. The generated diagnostic's poisoned fields and initial pending status are intentional test data overwritten by executed CPU instructions. No new endpoint, authentication path, public file-access surface or schema boundary is introduced.

## Next readiness

No owner setup or routine verification is needed. The orchestrator owns STATE/ROADMAP/requirement acceptance and must record this halt. Plan10-02 is blocked; the next separate allowed command is `$gsd-plan-phase 10` to reconcile the newly demonstrated timing-contract conflict. No later plan or GSD step ran here.

Actual tokens are ceil(realized source/test/doc diff characters37477/4), on the plan estimate's scale. Five commits are measured from the persisted ledger through plan_head_after, excluding this summary commit. Eleven implementation/test/doc files changed.

## Self-Check: PASSED

All11 changed implementation/test/doc paths and this canonical summary exist. All five listed task/correction commits are ancestors of HEAD. This verifies artifact claims; it does not convert the failed overall workflow into acceptance.

## Recovery closure: causal attribution and revised acceptance

The orchestrator authorized bounded recovery of this same plan after independent architecture/evidence reviews. The locked context D-19 permits specific evidence-backed revisions; research's behavior-version and evidence sections and the already-planned later revision task explicitly anticipate them. The original interpretation of old-value immutability was too strict. No new GSD step or later hardware plan ran. Commit60b8917 remains reachable and the failed runs above remain failed.

The halt closes only after the independently attributed predicates, unchanged full workflow and fresh current/preplan producer all pass. Both original tasks are complete, plan10-02 is ready, and BOARD-04/05 acceptance remains pending the remaining phase plans. The earlier next-command/replanning recommendation is superseded by this closure. The orchestrator alone updates shared STATE/ROADMAP and requirements.

### Differential proof and first changes

At source60b8917, a disposable archive retained every BankSet and ownership change and reversed only the three-line T0 clamp in MARIA. The build verified byte-identical payloads for every other archive object and relinked existing chain, sample, runner and diagnostic targets with the altered MARIA object. No permanent tracing switch, runtime mode, dependency or alternate behavior was added.

Reversing only that clamp restores all four historical 600-frame chains, all eight original audio rows, the old69-row digest c4c9c310418422c30bbf32b204a157a564d343aadfabee4d5654b97695461761 and the old342-row digest a036c0f9da8037b0581496182024f0b42de455e08576712e0cc890dd949462b5. Across a freshly produced complete956-row table, exactly four rows differ: soundtest raw/PCM and pokey raw/PCM; the other952 rows match. These are fresh differential observations, not cached data relabeled as package acceptance. Final revision6 output matches the attributed candidate's entire956 rows after only revision6-to-5 annotation normalization.

Temporary capture builds logged real c0/c1 transfer start, length, address and data, actual HALT/selection edges and subsequent CPU starts, plus native pixels, canonical raw events and PCM. In every case the first changed output follows a demonstrated stretched handoff; pre-handoff transfers retain their values and instruction order. For example, soundtest c0 reads its ROM operand at617016 for4ticks, then c1 reads $0280=$FF at617020 for6ticks. Its actual second fall is617025 and c2 starts617026. The next opcode read moves from617042 to617046 with the same address/data. POKEY's c1 actually reads TIA$09=$80 on its slow cycle, not an invented RIOT-only instruction sequence.

| Video sample | First different frame | y,x | Old → new native byte |
|---|---:|---|---|
| colorfade | 141 | 40,94 | 210 → 211 |
| fontcheck | 8 | 158,25 | 15 → 0 |
| mousedemo | 7 | 140,36 | 0 → 64 |
| screensafe | 8 | 195,312 | 0 → 148 |

The actual earliest CPU-resumption-changing handoffs are below. First fetch is independently calculated as fall+8 before the clamp and max(fall+8,c2) afterward. HALTRST is effective T0+observed cost; CPU resumes at c2+4×ceil((HALTRST−fall)/4). The captured post-hold transfers match those results. LRC remains absolute. None of these causal handoffs raises a DLI; trackertest is a last line without DLI, and its stale earlier NMI timestamp remains earlier than the handoff. Existing DLI-grid, abort and RDY checks continue to pass.

| Sample group | c0/c1 | Fall | Second fall | c2 / new first fetch | Cost | Old → new HALTRST | Old → new CPU resume | Absolute LRC |
|---|---|---:|---:|---:|---:|---|---|---:|
| colorfade, soundtest, soundeffects | FS | 617016 | 617025 | 617026 | 9 | 617031 → 617033 | 617042 → 617046 | 617440 |
| fontcheck, screensafe | FS | 609752 | 609761 | 609762 | 9 | 609767 → 609769 | 609778 → 609782 | 610176 |
| mousedemo | SF | 737324 | 737334 | 737334 | 9 | 737339 → 737341 | 737350 → 737354 | 737750 |
| eightysix | FS | 612930 | 612939 | 612940 | 9 | 612945 → 612947 | 612956 → 612960 | 613354 |
| pokey | FS | 1453284 | 1453293 | 1453294 | 109 | 1453399 → 1453401 | 1453410 → 1453414 | 1453708 |
| trackertest | FS | 4678952 | 4678961 | 4678962 | 121 last | 4679079 → 4679081 | 4679090 → 4679094 | 4679378 |
| rmtramcompressiondemo | SF | 1662578 | 1662588 | 1662588 | 9 | 1662593 → 1662595 | 1662604 → 1662608 | 1663002 |
| rmtramdemo, rmtromdemo | SF | 737328 | 737338 | 737338 | 169 | 737503 → 737505 | 737514 → 737518 | 737750 |

The earlier rmtramcompressiondemo cost199 handoff at1333428 changes HALTRST1333633→1333635 while both round to52 held cycles: CPU resume remains1333646. It does not falsely establish an earlier CPU divergence. Recurring bus predicates reproduce costs9/109/121-last/169/199 and assert literal selection, release, CPU-completion, fixed-LRC and no-DLI values. Existing all-fast16/24, abort and RDY predicates remain intact. Changed repeated instruction timing can later move writes earlier or later after polling/WSYNC; no blanket uniform-delay claim is made.

| Audio sample | First unequal raw event old → new | First unequal PCM sample index and values |
|---|---|---|
| soundtest | tick5393001 → 5393005, source0, level2 | 35430:1710 → 1711 |
| soundeffects | tick5393001 → 5393005, source0, level2 | 35430:1710 → 1711 |
| trackertest | tick20676381 → 20676385, source1, level4 | 137905:251 → 250 |
| eightysix | tick11755385 → 11755384, source2, level5 | 78091:288 → 287 |
| pokey | tick1458210 → 1458214, source3, level13 | 9047:−2 → −1 |
| rmtramcompressiondemo | tick2532744 → 2532746, source3, level1 | 16255:793 → 792 |
| rmtramdemo | tick1571978 → 1571982, source2, level6 | 9812:2418 → 2417 |
| rmtromdemo | tick7542080 → 7542072, source2, level7 | 49838:1241 → 1240 |

The distinct mechanisms are retimed CPU TIA-register writes and changed actual phi2 timing reaching continuously running POKEY sources. All eight full600-frame sample measurements retain479591 mono samples, rate48000/1, synthesis exact-q23-16x64-dc20 and clips0. Final outputs report behavior6. All old audio pairs remain in f84b4dd's expected file; the exact new measured pairs are recorded here:

| Sample | New raw SHA256 | New PCM SHA256 |
|---|---|---|
| soundtest, soundeffects | 3c17a63845643d0c4ee1c2aab54c419a5d855f65654952efb60998c0ed17531e | 8581cd5ba67cb0ece4fd0ffd8e772e612e08cf59bd3819597e438a8ea927609f |
| trackertest | 95bee3999dc5cb392195aceb4c674802180113065bf93ae8e909b103de79dc8f | d95f21eb8358ff137b7138ae6cf45c1108e45c430ae227c0282db73304bffb48 |
| eightysix | 5449bb49af0b2149bed41f541d5862df892ee4917947f2e3f130008c6d1902fe | 25cf13bdfb0b76bb284724423b2145688af866781f0eee45aeaf7029e0e0b5cd |
| pokey | cf12ef42ad4f37b3c86597b352d0b49be4842d18eafe8801f915b9c86b47a7fa | ec6cf133c0cedc991e7a135b3cfeefb119950964256e6c1e445d50f6f14b93c5 |
| rmtramcompressiondemo | 8e4302eb949568db3f5f7d906bb9f77b3db76eb8f20d1d6296816a01ab23936a | 7f99b24aea524a1ad6912eca4986ca1e0e69a4c2847042cd027f5e11707ba82d |
| rmtramdemo | e58dd1e920d463971642d27e48ad32340b20653bdae8b0e25f380d671c3a4191 | 88fc150384c731011bc122ae8fa20cff1b791b246b70fe2aa2b95add55f29aa9 |
| rmtromdemo | d42fced9e8b0ad00d48b234f298c10380d786d2c14495eafdc4c90246b6aa9d0 | 3dea6841fd7d1e798a611a1ad526cc21aced3c945691f9eba1d6aed2d2908d00 |

### Narrow revision and protected expectations

Commit28752c4 adds the causal release predicates. Commit59bad03 advances only behavior5→6 (API6/ABI1 stay unchanged), changes exactly the four demonstrated video chains and eight measured raw/PCM pairs, and aligns the existing report/version consumers. This pulls the minimum already-planned revision evidence forward from the later plan; it implements no API or expansion work.

The four old/new chain relations are literal allow-lists in the existing sample-identity consumer, restricted to the named frame-chain column during5→6. Every other byte remains equal and comparison identity is still the raw independently resolved base file. New pass/fault controls exercise valid/equal, missing/duplicate/forged annotations, reverse/skipped transitions, arbitrary frame pins, tool/source/ROM-hash edits, trailing content and a forged base. Existing producer-authentication controls still reject candidate-supplied trust inputs. The runner keeps its original69/342 historical aggregate digests, normalizing only the four literal soundtest/POKEY audio relations and current revision annotations. Old historical values are preserved rather than replaced with a permissive digest guard.

### Recovery verification and fresh package measurements

All following run URLs are **N/A-local**. Temporary instrumentation and captures are ignored build artifacts; no execution log or generated ROM is committed. An initial disposable sample-measure invocation omitted its required unused pin argument and exited2; the corrected invocation ran and checked all eight rows before any pin update.

| Command | Exit | Result |
|---|---:|---|
| `python3 build/ci/task10-recovery.py` with corrected five-argument sample invocations | 0 | Clamp-only archive/relink, all4 old chains and8 old audio rows restored, fresh956 rows with exactly4 changes |
| `python3 build/ci/task10-recovery/trace.py` | 0 | First native/raw/PCM changes and actual HALT timing captured for all affected samples |
| `python3 build/ci/task10-recovery/cycles.py` | 0 | Actual c0/c1 transfer values and post-hold CPU starts match the independent arithmetic |
| `ctest --preset ci -R '^(bus[.]\|maria[.])' --output-junit /tmp/muhtari-10-recovery-causal.xml --no-tests=error --output-on-failure` | 0 | All29 bus/MARIA checks including observed-cost regressions |
| `ctest --preset ci -R '^policy[.]quality[.]controls$' --output-junit /tmp/muhtari-10-transition-controls.xml --no-tests=error --output-on-failure` | 0 | Exact transition pass/fault cases and all existing producer/join controls |
| `ctest --preset ci -R '^(bus[.]\|maria[.]\|core[.]\|diag[.]\|policy[.]quality[.]controls\|audio[.]samples\|basic[.](colorfade\|fontcheck\|mousedemo\|screensafe)\|runner[.]hashes_file)' --output-junit /tmp/muhtari-10-recovery-acceptance.xml --no-tests=error --output-on-failure` | 0 | All94 selected/dependency checks pass |
| `sh build/ci/task08-jobserver-gate.sh` at59bad03 | 0 | Unchanged complete workflow:657 ordinary tests in42.60s, three actual ZIPs,6 release tests; repeated explicit6 release tests, exact657 inventory and hygiene pass |
| `cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake` | 0 | Exactly6 release tests once each |
| `sh /tmp/muhtari-10-quality.sh` | 0 | Existing quality_evidence BUILD_BASE producer below; fresh current657+6/base656+6, base ordinary42.19s |
| Final956-row comparison after only revision6→5 normalization | 0 | Every final numeric/digest byte matches the attributed candidate; no new unclassified drift |
| `git diff --check` and task hygiene hooks | 0 | No findings; no bypass used |

The quality wrapper uses the standard four-slot FIFO jobserver and exactly the original producer command from the historical verification table, with actual HEAD resolved59bad038be902f4d6879fe4513954a5a921b336d. Genuine preplan base is f84b4dd83f5a91a8f86c18bae2ec602ae8505540. Both reports have status measured, configuration native and source local. Their matching environment is AppleClang21.0.0.21000101/Darwin-arm64, fixture750d8b722c88a03aff050dec5e961a44523a703afcc134f7dce4e3ebf5107932 and raw trusted sample identity406ca181ecad9c64d4030b09203f210b69d5f5c649453f0a5149fc864444cb4b. Both exact three-archive sums were independently checked against the producer JSON. No hosted source URL, authenticated hosted join or phase-versus-main claim is invented.

| Actual archive | Candidate bytes | Preplan bytes | Candidate SHA256 | Preplan SHA256 |
|---|---:|---:|---|---|
| library macos-arm64 | 81985 | 81760 | b49c4a234cf5fe7a2b544e5abff1202ce50945a0ef0421a3a4e6e1e2e9f3126f | 5eb1062d9a5e73e8afed7741b3e45e640b178a781e690011b56d8ec2e76d6257 |
| libretro macos-arm64 | 87631 | 87534 | f3799eb39e449955ad7d4f4c8e22bf980178fcbd7321adb16d4f9370a284c741 | 6a4a4b9927b95e3794f8095a8fffde7605420ca5d5963b29f96f117080f5efc6 |
| runner macos-arm64 | 96425 | 96361 | 3ff8561ebba9b22edfc96090788f333ea37581a046dff0802400a28894fda9ca | 1d3cfb97b0db7fccd91930bf4fa767484d073b83f2b6159f8aee35a5c5b4ccf6 |
| Total, report-only | 266041 | 265655 | +386bytes | No archive cutoff |

The producer-owned clean detached base checkout was removed after preserving its actual ZIPs and reports. The phase checkout remains on its original branch. Prior failed/incomplete producer attempts above are not reused as acceptance.

### Final scope and readiness

This recovery resolves the six timing-observation failures and the blocked package-measurement gate. It adds no owner UAT, new quota, workload reduction, deadline change or hardware-calibration claim. The earlier independent source confidence and two unclassified assumption rows remain. The pre-existing WINDOWS count mismatch remains a separate metadata observation, not a silently repaired entry. The minimum behavior-revision portion of the later plan is already complete and its executor must preserve it.

There are21 changed implementation/test/doc files. Actuals17961tokens are ceil(realized diff characters71843/4). Eight commits are measured from the persisted ledger through59bad03, including the preserved historical summary and two recovery commits, excluding this closure commit. Elapsed wall time includes the designed halt and independent-review interval; the initial17-minute execution record remains historical.

Next separate allowed command: `$gsd-execute-phase 10`. Within the active orchestrated step, plan10-02 is ready. No further GSD step was started by this executor.

## Recovery Self-Check: PASSED

The canonical summary and all21 changed implementation/test/doc paths exist. All eight recorded commits are reachable ancestors of HEAD. Complete current/base inventories, measured archive identities and pass/fault consumers pass. Requirements-completed is empty; shared requirement acceptance is unchanged.
