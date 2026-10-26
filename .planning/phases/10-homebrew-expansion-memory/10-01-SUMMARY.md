---
phase: 10-homebrew-expansion-memory
plan: "01"
subsystem: cartridge
tags: [bankset, bus, maria, diagnostics, c17]
status: halted
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
key-decisions:
  - Retain the causal slow-edge fit and preserve old pinned observations as failures pending reconciliation.
  - Stop dependent expansion work because the complete required workflow does not pass.
requirements-completed:
  - BOARD-04
  - BOARD-05
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
        status: fail
      - kind: other
        ref: tests/cmake/quality_evidence.cmake
        status: fail
    human_judgment: false
actuals:
  tokens: 9370
  tasks: 2
  commits: 5
plan_head_before: f84b4dd83f5a91a8f86c18bae2ec602ae8505540
plan_head_after: 1c251cbf6d592112bf175a8823f568ad51dfc959
duration: 17 min
started: 2026-10-26T13:08:26Z
completed: 2026-10-26
---

# Phase 10 Plan 01: Causal BankSet Tracer Summary

**Two 32 KiB ROM sets execute through all three hosts with actual-edge ownership; six retained timing observations block acceptance and dependent expansion.**

## Status and scope

Both implementation tasks and their focused checks are complete. This is a designed halt, not accepted plan or phase completion: the full required workflow fails. The requirements-completed labels above copy the plan's contribution labels; neither BOARD-04 nor BOARD-05 is accepted. No expansion hardware or broader geometry/RAM claim follows from this tracer. Plan 10-02 and all dependent work remain blocked.

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
