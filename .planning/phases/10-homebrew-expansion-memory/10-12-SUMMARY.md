---
phase: 10-homebrew-expansion-memory
plan: "12"
subsystem: documentation
tags: [bankset, xboard, xm, hardware, api, compatibility]
requires:
  - phase: 10-homebrew-expansion-memory
    provides: Accepted local hardware, host, diagnostic, equality, lifetime and safety evidence from plans 01-11
provides:
  - Exact qualified BankSet and distinct XBoard/XM hardware documentation
  - Correct API7/ABI1/behavior6 and descriptive-probe versus effective-load contract
  - Three executed owned diagnostic ledger rows with generated capability scope
affects: [10-13, BOARD-04, BOARD-05, compatibility-accounting]
tech-stack:
  added: []
  patterns:
    - Graduate implemented hardware facts while retaining source qualification and preparation pointers
    - Existing local mailbox evidence and compatibility generator
key-files:
  created: []
  modified:
    - docs/hardware/cartridge.md
    - docs/hardware/pokey.md
    - .planning/preparation/A7800-CARTRIDGES-AND-PERIPHERALS.md
    - .planning/preparation/A7800-AUDIO.md
    - docs/embedding.md
    - tests/compat/ledger.csv
    - docs/compatibility.md
    - libretro/muhtari_libretro.info.in
    - README.md
    - docs/runner.md
    - docs/emulation-tests.md
    - tools/compat_doc.cmake
key-decisions:
  - Retain medium physical-XM confidence, derived slow-edge timing and producer/emulator qualification for larger/shared BankSet variants.
  - Preserve every prior ledger row and predicate; representative local mailbox passes confer no gameplay acceptance.
  - Keep final hosted and phase acceptance in plan13 and the failed/draft publication outside this plan.
requirements-completed: []
coverage:
  - id: qualified-bankset-contract
    description: Six ROM sizes and fourteen accepted RAM geometries, electrical ownership, redirects and overlap priority match implementation and independent diagnostics.
    requirement: BOARD-04
    verification:
      - kind: integration
        ref: "ctest --preset ci -R '^diag[.](bankset|expansion)$' at ebd6c37"
        status: pass
      - kind: other
        ref: Source and expected-record comparison; docs and ABI task selector, 16 checks
        status: pass
    human_judgment: false
  - id: qualified-expansion-contract
    description: Distinct XBoard aliases and XM exact pages, continuously installed POKEY, hidden physical HSC and pointer lifetimes retain explicit confidence limits.
    requirement: BOARD-05
    verification:
      - kind: integration
        ref: diag.expansion plus ABI1-7, memory-map, battery and HSC-firmware consumers
        status: pass
      - kind: other
        ref: Preparation audio correction and released claims checked against cart, bus, MARIA, instance, ports and public header
        status: pass
    human_judgment: false
  - id: executed-ledger-capabilities
    description: Three representative local mailbox rows preserve all 43 prior rows and distinguish capability metadata from gameplay acceptance.
    requirement: BOARD-04
    verification:
      - kind: integration
        ref: policy.compat_doc_fresh and its fault control; task2 runner/docs selector, 83 checks
        status: pass
      - kind: other
        ref: Independent prior-row equality and four image hashes plus numeric CRC for each new row
        status: pass
    human_judgment: false
  - id: workflow-and-packages
    description: Full local workflow and fresh genuine-base package producer preserve required consumers without runtime changes.
    requirement: BOARD-05
    verification:
      - kind: integration
        ref: "sh build/ci/task08-jobserver-gate.sh at 1a896ad; 661 ordinary plus six release"
        status: pass
      - kind: integration
        ref: "sh build/ci/task10-12-quality.sh versus genuine a6dfa80; current661+6/base661+6"
        status: pass
    human_judgment: false
actuals:
  tokens: 8895
  tasks: 2
  commits: 2
plan_head_before: a6dfa8073d4a12202fd1d52b74b5eb46d67ce623
plan_head_after: 1a896ad15aa6380bfa0cdc5815354e70372317db
commits: 2
duration: 10m
completed: 2026-10-26
status: complete
---

# Phase 10 Plan 12: Qualified Hardware and Compatibility Documentation Summary

**Exact BankSet geometry and selected XBoard/XM maps now accompany executed owned mailbox evidence, with physical-confidence and gameplay limits intact.**

## Accomplishments and Commits

1. **Graduate the selected hardware profile and its confidence limits** — `ebd6c37`, docs. Published the six-row/fourteen-geometry BankSet matrix and causal four-pair handoff fit; retained redirects, overlap priorities, distinct XBoard/XM maps, hidden physical HSC, continuous installed POKEY, firmware/stub limits and physical-span/static-map distinctions. Corrected stale API and multiple-primary/load claims. Preparation audio now distinguishes the historical XBoard-style decoder from selected XM, with retained citations and updated dates.
2. **Record only executed board and subset compatibility predicates** — `1a896ad`, docs. Added representative `diag/bankset`, `diag/xboard` and `diag/xm` local `mailbox@12` rows. The generated page, frontend description, README and runner instructions distinguish metadata inclusion from gameplay acceptance and explicitly exclude unsupported devices/services. All43 old ledger rows, including every passing predicate, remain exactly unchanged.

Global TDD mode is false and this plan changes documentation/evidence only. No fabricated RED or no-op commit was created. No runtime, public header, timing, hash expectation, fixture, test inventory, quota or dependency changed. Twelve task files changed; actual tokens are ceil(chars/4) over the realized source diff. The persisted per-plan ledger measures two task commits through1a896ad; summary metadata is separate. Shared STATE/ROADMAP/REQUIREMENTS remain owned by the orchestrator.

## Verification and Failed Attempts

Every command below has run URL **N/A-local**. No hosted acceptance is claimed and no log is committed.

| Command | Exit | Observation |
|---|---:|---|
| Initial task1 configure/build plus `ctest --preset ci -R '^(docs[.]\|abi[.]api_\|libretro[.]info_claims)' --no-tests=error --output-on-failure` |8|15/16 pass; shortened validation prose accidentally removed the API-name check's only `MUHTARI_REGION_*` reference |
| Same complete task1 command after restoring that reference |0|16/16 pass, including frozen callers and API7 |
| `ctest --preset ci -R '^diag[.](bankset\|expansion)$' --no-tests=error --output-on-failure` at ebd6c37 |0|BankSet1.84s, expansion5.31s; existing independent numeric/pixel, protocol-fault and actual-host checks pass |
| Initial task2 configure/build plus `ctest --preset ci -R '^(docs[.]\|compat[.]\|libretro[.]info_claims\|runner[.])' --no-tests=error --output-on-failure` |8|82/83 pass; expanded emulation-test explanation made that page41672/40960 bytes |
| `cmake -DROOT="$PWD" -P tests/cmake/budgets.cmake` after concise replacement |0|All22 documents fit; emulation-tests40905 and embedding40753 bytes |
| Final task2 selector above |0|83/83 pass |
| `ctest --preset ci -R '^(policy[.]compat_doc_fresh\|abi[.]api_\|libretro[.](info_claims\|host[.](expansion\|memory_map\|hsc_firmware\|battery)))' --no-tests=error --output-on-failure` |0|21 checks including fixtures, generated-page fault control, ABI and real host consumers |
| `cmake -DLEDGER=tests/compat/ledger.csv -DOUT=build/ci/task10-12-compatibility.md -DCHECK=docs/compatibility.md -P tools/compat_doc.cmake` |0|Existing generator reproduces the committed page |
| One-off Python csv/hashlib/binascii comparison against `git show a6dfa80:tests/compat/ledger.csv` and actual diagnostic images/reports |0|All43 prior rows unchanged; three additions have correct raw/headered SHA-1/SHA-256 and independently calculated CRC values ee7c/f157/9354 |
| `sh build/ci/task08-jobserver-gate.sh` at1a896ad |0|661 ordinary89.01s; six release0.26s and repeated six0.26s; actual packages, exact661-once inventory and hygiene pass |
| `sh build/ci/task10-12-quality.sh` at1a896ad versus a6dfa80 |0|Fresh recursive producer: current661+6/base661+6; base ordinary105.32s/release0.19s; authentic matching environment and fixture identities |
| One-off JSON/actual-ZIP comparison; `git worktree remove build/ci/quality-base` |0|Both measured sums match actual files/source identities; clean temporary baseline removed |
| `git diff --check`; file and commit-ancestry checks |0|All12 task files exist; both task commits are reachable; clean task tree |

The one-off quality wrapper supplies a four-slot FIFO jobserver and invokes the unchanged producer:

`GITHUB_EVENT_PATH="$PWD/build/ci/task10-12-event.json" GITHUB_EVENT_NAME=push cmake -DROOT=. -DBUILD=build/ci -DOUT=build/ci/task10-12-quality-current.json -DSHA="$(git rev-parse HEAD)" -DCONFIG=ci -DBUILD_BASE=ON -DSOURCE=local -P tests/cmake/quality_evidence.cmake`

The event's genuine before is a6dfa8073d4a12202fd1d52b74b5eb46d67ce623; candidate is1a896ad15aa6380bfa0cdc5815354e70372317db. Both reports identify AppleClang21.0.0.21000101/Darwin/arm64, manifest5d355f481b2aaf34f67a003c938a2a14a3d5e554455568d3ce9437901142c527 and sample d549d4691aca7c004325102613fd6ac2337ecfe8a4c0cec004472e5bbcc80ca8. The baseline already contains preceding slices; this is no phase-versus-main comparison or cached-base relabel.

| Actual macOS arm64 ZIP | Candidate bytes | Genuine pre-plan bytes |
|---|---:|---:|
| Library |83245|83220|
| Runner |98522|98522|
| Libretro |89706|89625|
| Three-component sum |271473|271367|

Archive bytes remain report-only under DEC.61. Local elapsed times establish no hosted or cross-platform performance result.

## Deviations from Plan

**[Rule 3 - Blocking issue] Update the existing generator owner.** The generated compatibility page's fixed capability prose belongs to tools/compat_doc.cmake. Task2 necessarily changes that existing file to emit the planned claims; the existing freshness consumer and representative fault test pass. No new generator or predicate framework was added. Commit1a896ad.

**[Rule 1 - Bug] Correct the existing predicate guide without exceeding its budget.** docs/emulation-tests.md still said the BankSet board was refused. Task2 narrowly replaces that statement with the independently unassembled sample limit and explains local mailbox rows; consolidation retains INTIM/title/skip semantics and fits the unchanged budget. The initial budget failure remains recorded above. Commit1a896ad.

## Limits and Next Work

The three new local rows are representative exact owned-template images, not full-library or private-title accounting. The historical unassembled 7800basic BankSet sample is not promoted. Larger/shared BankSet variants remain producer/emulator extensions; stretched-edge timing remains derived. Physical XM/HSC/POKEY confidence remains medium and implementation-backed, with inaccessible primary specification, signature-only firmware stub and unsupported full-XM services explicitly bounded. No new runtime stub, security surface, owner verification or authentication gate was introduced.

The inherited optional WINDOWS append mismatch and command-as-path summary-scanner heuristic were not rerun or repaired. Actual files, source ancestry and required consumers were verified directly. Final exact-source hosted/native/PowerPC/package acceptance, inventory reconciliation and phase review remain mandatory in10-13. BOARD-04/05 receive documentation evidence here and remain pending final acceptance. The separate failed/draftv0.8.0 publication stays historical and unaccepted; no release repair, retry or acceptance was performed.

Plan10-13 is ready within the current execution step. The next separate allowed command is `$gsd-execute-phase 10`; no other GSD step was launched.

## Self-Check: PASSED

All12 task files and both reachable task commits were checked. Both task selectors, actual diagnostic reports, every old ledger row, new image hashes/CRCs, generated freshness/fault control, full661+6 workflow and fresh genuine-base producer pass. The temporary baseline worktree is removed. The persisted ledger measures two task commits through1a896ad; shared planning state remains untouched for the orchestrator.
