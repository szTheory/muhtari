---
phase: 10-homebrew-expansion-memory
plan: "03"
subsystem: cartridge
tags: [xboard, expansion, api, allocation, pokey, c17]
status: complete
requires:
  - phase: 10-02
    provides: Checked BankSet geometry, physical RAM and effective standalone HSC refusal
  - phase: 10-01
    provides: Independently attributed behavior revision6 and protected evidence transition
provides:
  - Append-only API7 selection and effective attachment with ABI1 and behavior6
  - XBoard eight-bank RAM and decode-gated continuously installed POKEY
  - Checked candidate backing, borrowed-media staging and exact replacement accounting
  - Stable physical expansion spans and linked new/frozen caller evidence
affects: [10-04, 10-05, 10-06, expansion-memory, hosts]
tech-stack:
  added: []
  patterns:
    - Existing candidate preparation resolves effective owners before allocation
    - One recorded ROM/cart/expansion allocation and cartridge-local decode predicates
key-files:
  created:
    - tests/api/v7.c
  modified:
    - include/muhtari.h
    - src/instance.c
    - src/internal.h
    - src/cart.c
    - src/ports.c
    - tests/core/test_core.c
    - tests/audio/test_pokey.c
    - tests/abi/tests.cmake
    - tests/header/header.c
    - tests/header/header.cpp
    - tests/inventory/ci.txt
    - docs/embedding.md
    - docs/hardware/cartridge.md
key-decisions:
  - Preserve descriptive cartridge metadata while resolving installed expansion and sound owners before allocation.
  - Record the exact combined backing length and stage borrowed firmware before successful replacement releases its source.
  - Verify the already-accepted behavior6 consumers without a no-op commit or further observation changes.
requirements-completed: []
requirements-acceptance: pending
coverage:
  - id: D1
    description: Append-only selectors, effective attachment, complete-field Auto defaults, frozen media/info prefixes and output canaries
    requirement: BOARD-05
    verification:
      - kind: integration
        ref: abi.api_v7
        status: pass
      - kind: integration
        ref: abi.api_v1 through abi.api_v6_layout and abi.header.c17 and abi.header.cxx17
        status: pass
    human_judgment: false
  - id: D2
    description: CPU-executed XBoard stores, eight banks, sixteen aliases, retained gate/bank contents and neutral power controls in both regions
    requirement: BOARD-05
    verification:
      - kind: unit
        ref: tests/core/test_core.c#expansion_xboard
        status: pass
    human_judgment: false
  - id: D3
    description: Unsupported audio and owner conflicts refuse before allocation while failed loads and borrowed ROM/BIOS/HSC preserve the live transaction
    requirement: BOARD-05
    verification:
      - kind: unit
        ref: core.load.keeps_previous
        status: pass
      - kind: unit
        ref: pokey.maps
        status: pass
      - kind: integration
        ref: abi.api_v7
        status: pass
    human_judgment: false
  - id: D4
    description: Physical expansion bytes remain distinct from HALT cart RAM and layout1 battery storage across controls, refusal and power cycle
    requirement: BOARD-05
    verification:
      - kind: integration
        ref: abi.api_v7 and core.get_memory.spans and core.load.keeps_previous
        status: pass
    human_judgment: false
  - id: D5
    description: Existing exact behavior6 observations, protected revision relation, three-host BankSet tracer and full authentic local candidate/base evidence
    requirement: BOARD-04
    verification:
      - kind: integration
        ref: policy.quality.controls and audio.samples and diag.bankset_tracer
        status: pass
      - kind: integration
        ref: cmake --workflow --preset ci
        status: pass
      - kind: other
        ref: tests/cmake/quality_evidence.cmake with BUILD_BASE=ON versus preplan4a3df69
        status: pass
    human_judgment: false
actuals:
  tokens: 13254
  tasks: 3
  commits: 5
plan_head_before: 4a3df69e0fc886dc53c9778ea34ecceb045cbf4b
plan_head_after: 862e6bb94c83b280a831595474e2e01f4c94c668
duration: 15 min
started: 2026-10-26T17:50:56Z
completed: 2026-10-26
---

# Phase 10 Plan 03: Checked Expansion Selection and XBoard Summary

**API7/ABI1/behavior6 installs XBoard through one checked transaction, exposes distinct physical expansion RAM, and preserves frozen callers and exact existing output evidence.**

## Accomplishments

Auto/Off/XBoard/XM is appended to media and effective information without changing any old minimum, offset, status or numeric constant. Complete-field-end copying makes omitted and every partial selector equivalent to Auto. Explicit selection wins over the resolved XM request, including nonzero byte63 with its existing warning. The new linked client independently freezes API6 media/info layouts, exercises old minima and zero/nonzero tails, and checks output prefix canaries. Historical v1–v6 clients and expectations remain unchanged.

The candidate resolves expansion, effective HSC and sound ownership before allocation. XM Auto provisions HSC; explicit HSC Off suppresses it. The 52KiB BankSet/HSC conflict therefore covers effective XM attachment as well as standalone HSC. Probe preserves v4 ADPCM bit0x20 as sound64; load refuses YM2151/Covox/ADPCM and conflicting primaries. Expansion's declared450 is one installed gated chip, and secondary440 can coexist. Separate4000/0800 primaries refuse. Souper remains refused by the existing board owner.

ROM, cart RAM and128KiB expansion RAM share one allocation with checked additions and a recorded exact free length. ROM copies before the prior allocation is freed; existing BIOS/HSC staging consumes borrowed firmware before replacement. Tests borrow all three inputs from live expansion storage in the same successful reload, and retain selection, pointers, physical contents and battery state on refusal/allocation failure.

XBoard's sixteen0470–047F aliases select all eight16KiB banks, RAM enable and450 register decode. Disabled RAM windows pass through; gate/bank writes retain physical contents. Installed POKEY count/source/clock state remains separate from register decode. CPU-executed stores and a mailbox load prove actual execution in NTSC/PAL. Console Reset retains controls; power cycle resets controls/chips/volatile fill while retaining effective selection and committed battery bytes. The physical131072-byte span is distinct from32768-byte HALT cartridge RAM and layout1 battery storage, and stays stable across controls, power and refusals.

Task3's five evidence files were already correctly implemented in accepted source59bad03. Inspection and44 fresh checks confirm the exact5-to6 relation, raw trusted-base identity, missing/duplicate/reverse/skipped/forged/content fault controls and unchanged observations. Those files have no diff from this plan's genuine preplan base. No further revision bump, hash change, broad regeneration or no-op commit was made.

## Task Commits and TDD

1. Task1 RED —09df004: unavailable audio incorrectly loaded with status0/new allocation; ADPCM probing returned0 instead of64. The actual linked core test failed its planned assertions, not setup or discovery. Its unchanged JUnit report passes the SDK classifier as RED_EVIDENCE_OK; semantic inspection confirms the intended missing refusal/metadata behavior.
2. Task1 GREEN —5200926: checked additive transaction, XBoard execution, sound ownership, borrowing and required documentation/private-call updates.
3. Task2 RED —557980f: the actual physical getter returned ERR_ARGUMENT1 instead of OK/NULL/0 for absent expansion RAM. Its unchanged JUnit report passes RED_EVIDENCE_OK; semantic inspection confirms the missing memory-kind behavior.
4. Task2 GREEN —f31f23d: physical getter, API7 client, inventory/ABI registration and current-header version assertions.
5. Required current-version correction —862e6bb: core.info expected7 and the embedding guide names the header's expansion family.
6. Task3 — verification only of the already-completed59bad03 contract; no new commit necessary.

No refactor-only commit was needed. The measured five commits precede this summary's metadata commit. Realized diff is53014 characters, estimated on the same chars/4 scale as the plan:13254 tokens.

## Verification Evidence

Every row has run URL **N/A-local**. No hosted acceptance is claimed. Logs and media bytes were not committed.

| Command | Exit | Observation |
|---|---:|---|
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^core[.]load[.]keeps_previous$' --output-junit /tmp/muhtari-10-03-task1-red.xml --no-tests=error --output-on-failure` | 8 | Intended Task1 sound/ADPCM assertion failures |
| `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/ci/task10-03-task1-red.json --raw` | 0 | RED_EVIDENCE_OK; real JUnit timestamps and unchanged report retained locally |
| `cmake --preset ci && cmake --build --preset ci` during Task1 implementation | 1 | Two intermediate builds caught obsolete whole-struct size assertions, then an undeclared test-helper reference; fixed before GREEN |
| `ctest --preset ci -R '^(core[.]load[.]\|core[.]hsc[.]\|core[.]battery[.]\|core[.]get_memory[.]\|cart[.])' --no-tests=error --output-on-failure` | 0 | Initial35/35 focused load/cart checks |
| `ctest --preset ci -R '^(core[.]load[.]\|core[.]hsc[.]\|core[.]battery[.]\|core[.]get_memory[.]\|core[.]api[.]size_tags$\|cart[.]\|pokey[.])' --no-tests=error --output-on-failure` | 8 | Only pokey.maps failed its obsolete silent-primary-priority assertion |
| Same expanded command after the narrow sound-contract correction | 0 | All39/39 pass, including CPU-executed XBoard and borrowed-media checks |
| `cmake --build --preset ci && ctest --preset ci -R '^core[.]get_memory[.]spans$' --output-junit /tmp/muhtari-10-03-task2-red.xml --no-tests=error --output-on-failure` | 8 | Intended Task2 physical getter assertion failure |
| `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/ci/task10-03-task2-red.json --raw` | 0 | RED_EVIDENCE_OK |
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(abi[.]api_\|abi[.]header[.]\|core[.]get_memory[.]\|core[.]load[.])' --no-tests=error --output-on-failure` before header correction | 8 |17/19 pass; only C17/C++17 current-version assertions still expected6 |
| Same command after correcting only current-header version assertions | 0 |19/19 pass, including every real linked old/new client |
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(policy[.]quality[.]controls$\|audio[.]\|diag[.]\|core[.]pal[.]\|abi[.]api_)' --no-tests=error --output-on-failure` | 0 |44/44 pass; existing three-host BankSet tracer included |
| `sh build/ci/task08-jobserver-gate.sh` at f31f23d | 8 |656/658 ordinary tests pass in47.29s; core.info expected6 and docs.api_names lacked the family name |
| `cmake --build --preset ci && ctest --preset ci -R '^(core[.]info$\|docs[.]\|abi[.]api_)' --no-tests=error --output-on-failure` | 0 |15/15 targeted corrections pass |
| `sh build/ci/task08-jobserver-gate.sh` at862e6bb | 0 |658 ordinary tests in43.78s; actual packages;6 release tests in0.14s and repeated6 in0.10s; exact inventory and hygiene |
| `sh build/ci/task10-03-quality.sh` | 0 | Fresh existing producer builds genuine preplan4a3df69; current658+6/base657+6; matching toolchain, fixture and trusted sample identity |
| `git diff --check` and `scripts/hygiene.sh --tree` | 0 | Required whitespace/tree hygiene pass; hooks ran on each task commit |

The one-off quality wrapper uses the existing four-slot FIFO jobserver and invokes:

`GITHUB_EVENT_PATH="$PWD/build/ci/task10-03-event.json" GITHUB_EVENT_NAME=push cmake -DROOT=. -DBUILD=build/ci -DOUT=build/ci/task10-03-quality-current.json -DSHA="$(git rev-parse HEAD)" -DCONFIG=ci -DBUILD_BASE=ON -DSOURCE=local -P tests/cmake/quality_evidence.cmake`

Its event's genuine `before` is4a3df69e0fc886dc53c9778ea34ecceb045cbf4b, recorded before the first task commit. The producer authenticated raw trusted fixture/sample identities and freshly built that commit; no cache or same-head run was relabeled as a baseline. Its base workflow passes657 ordinary tests in39.08s and6 release checks in0.08s. Candidate source is862e6bb94c83b280a831595474e2e01f4c94c668. The clean detached producer worktree was removed after measurement; producer JSON remains local outside that worktree.

| Actual macOS arm64 ZIP | Candidate bytes | Genuine preplan bytes |
|---|---:|---:|
| Library |83018|82490|
| Runner |97532|97006|
| Libretro |88525|88143|
| Three-component sum |269075|267639|

Archive bytes are report-only under DEC.61. This local comparison does not establish phase-versus-main performance, other platforms or final hosted acceptance.

## Deviations from Plan

**1. [Rule2 - Required conventions] Document behavior with its tests and update direct private/header consumers.**
AGENTS requires behavior documentation in the same change. Added concise XBoard/selection/physical-memory prose to docs/embedding.md and docs/hardware/cartridge.md instead of waiting for the XM plan. The new private install argument necessarily updates tests/audio/test_pokey.c. Current C/C++ header compile targets necessarily assert API7. No historical v1–v6 test/layout source or expectation was edited. Commits5200926/f31f23d.

**2. [Rule1 - Contract correction] Replace obsolete silent sound-owner precedence.**
The expanded test run independently reproduced pokey.maps expecting all-primary0x8441/0x8401 to load, while the locked D-15 contract requires refusal of independent conflicting primary owners. Before editing that predicate, the failure and rationale were reported to the orchestrator. The smallest correction preserves all three legal primary+440 maps and adds descriptive-probe/no-replacement refusal cases for exactly conflicting declarations. No numeric audio/frame/hash observation changed. Commit5200926.

**3. [Rule1 - Version consumers] Complete current API-version expectations.**
The first full workflow exposed the missed core.info6 expectation and expansion-family guide name. Both were corrected together through existing consumers, verified15/15, committed862e6bb and followed by a passing unchanged full workflow. No new gate or weakened assertion was introduced.

**4. Existing completed work, no new implementation.**
Task3's behavior6 transition was necessarily completed and independently justified in10-01; this plan verifies it and preserves that history instead of changing it again. Its old four frame-chain/eight raw/PCM changes remain attributed to the clamp-only reproduction recorded in10-01-SUMMARY.md, not to expansion selection.

## Limits and Handoff

No new stubs, skipped tests, unrun verification commands, network endpoints, file-access surfaces, dependencies, quotas or tracked-file deletions were introduced. Existing signature-only HSC firmware remains intentionally limited and documented. XBoard follows the cited implementation profile, not new physical hardware measurements. Exact XM page controls/HSC decode gating and the full independent POKEY continuity/IRQ matrix belong to10-04; adapter selection/discovery and honest host descriptors belong to their later plans. This plan exposes the checked selection/storage seam without claiming those later behaviors complete.

All three plan tasks are implemented or verified. BOARD-04/05 requirement acceptance remains pending; `requirements-completed` is intentionally empty. The orchestrator owns shared STATE/ROADMAP/REQUIREMENTS and was not modified by this executor. Next separate allowed command after orchestrator acceptance: `$gsd-execute-phase 10`. No subsequent GSD step was run.

## Self-Check: PASSED

The canonical summary and new API7 client exist. All five listed task/correction commits are ancestors of the verified source HEAD. The persisted preplan ledger resolves to4a3df69 and the measured pre-summary count is5; source HEAD is862e6bb. The tree has no unrelated edits, all required local verification passed, and the final whitespace/hygiene self-check exits0. Shared planning state was left to the orchestrator.
