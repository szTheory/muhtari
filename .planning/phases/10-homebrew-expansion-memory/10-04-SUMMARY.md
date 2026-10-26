---
phase: 10-homebrew-expansion-memory
plan: "04"
subsystem: cartridge
tags: [xm, xboard, ram, hsc, pokey, c17]
status: complete
requires:
  - phase: 10-03
    provides: API7 selection, checked expansion backing and installed sound topology
  - phase: 10-01
    provides: Shared time-aware electrical bus selection at behavior revision6
provides:
  - Exact XM RAM controls and independent CPU/MARIA page nibbles
  - Initially hidden HSC restore with stable physical battery and firmware identity
  - Independent continuous POKEY DAC, PCM, phase and declared IRQ evidence
affects: [10-05, 10-06, expansion-memory, hosts]
tech-stack:
  added: []
  patterns:
    - Cartridge-local XM decode uses the existing bus electrical owner
    - Physical attachment remains separate from volatile visibility
key-files:
  created: []
  modified:
    - src/cart.c
    - src/internal.h
    - tests/cart/test_cart.c
    - tests/bus/test_bus.c
    - tests/core/test_core.c
    - tests/audio/test_pokey.c
    - tests/audio/test_irq.c
    - docs/embedding.md
    - docs/hardware/cartridge.md
    - docs/hardware/pokey.md
key-decisions:
  - Reuse the actual bus electrical interval for XM instead of introducing another DMA selector.
  - Gate both HSC windows through one visibility predicate without changing provisioned storage.
  - Preserve the existing continuously installed POKEY implementation and prove it against independently enabled controls.
requirements-completed: []
requirements-acceptance: pending
coverage:
  - id: D1
    description: Exact XM controls, both windows, every CPU/MARIA page pair and endpoint, actual DMA/VBE and assertion/release edges
    requirement: BOARD-05
    verification:
      - kind: unit
        ref: cart.retail.boards
        status: pass
      - kind: integration
        ref: bus.halt.latency_two_cycles
        status: pass
    human_judgment: false
  - id: D2
    description: Initially hidden HSC import, shared ROM/SRAM gates, stable firmware/save identity, effective conflicts and reset/power lifecycle
    requirement: BOARD-05
    verification:
      - kind: unit
        ref: core.hsc.decode
        status: pass
      - kind: unit
        ref: core.get_memory.cart_ram and core.hsc.firmware and core.load.keeps_previous
        status: pass
    human_judgment: false
  - id: D3
    description: Continuous XBoard/XM tone phase, raw DAC records, audible PCM and IRQ through disabled decode with exact installed topology
    requirement: BOARD-05
    verification:
      - kind: unit
        ref: pokey.maps and core.load.keeps_previous
        status: pass
      - kind: integration
        ref: sally.irq
        status: pass
    human_judgment: false
  - id: D4
    description: Final-source full native workflow and authenticated genuine-preplan three-component package comparison
    requirement: BOARD-05
    verification:
      - kind: integration
        ref: sh build/ci/task08-jobserver-gate.sh at e082c3b
        status: pass
      - kind: integration
        ref: tests/cmake/quality_evidence.cmake with BUILD_BASE=ON versus preplan f64d68f
        status: pass
    human_judgment: false
actuals:
  tokens: 11155
  tasks: 3
  commits: 6
plan_head_before: f64d68fea48d2d96f15165b05ff170400689985c
plan_head_after: e082c3b22a546284ba6cbef20de6da31624242ce
duration: approximately 16min
completed: 2026-10-26
---

# Phase 10 Plan 04: XM RAM, hidden HSC and continuous POKEY Summary

**Exact XM page/window decode shares BankSet's electrical bus owner; HSC visibility preserves physical saves, while installed POKEY keeps phase, audio and IRQ.**

## Accomplishments

- Exact $0470/$0478/$047C controls implement two independently enabled 8 KiB windows and sixteen pages. CPU stores/peeks retain CPU ownership; timed MARIA fetches use the existing electrical interval. No second timing authority exists. Tests exhaust both nibbles, endpoints through offset131071, unsupported aliases/bits, open reads and cartridge bank-store pass-through.
- Real DMA draws unequal expansion graphics, non-HALT VBE reads CPU pages, and four fast/slow edge combinations cover assertion and release for both windows. Existing BankSet, MARIA, MEN/BIOS and timing assertions remain intact.
- XM HSC begins physically attached but hidden. A real cartridge enables it after save import and reads the restored byte. Shared ROM/SRAM visibility preserves pointer, length, layout1, firmware identity, battery bytes and generation. Hidden/same-value stores, explicit Off, supplied/stub firmware, EEPROM coexistence, failed loads and power/console Reset distinctions pass in both regions.
- Independently loaded enabled controls match every raw tick/source/level tuple, audible PCM and chip state while the comparison session disables decode. Slow cycles, HALT, same-value control stores, ignored disabled writes, re-enable reads, single/dual chip identities and power versus console Reset are exercised for XBoard/XM in NTSC/PAL. Actual CPU execution takes only the declared XM IRQ while its register window is hidden.

API7/ABI1/behavior6, the existing two-chip bound, integer mixer and all pinned observations remain unchanged. Ten implementation/test/documentation files changed. Actual tokens are ceil(44620 realized diff characters/4), not harness tokens; the six commits are measured from the persisted pre-plan ledger through the recorded source head, excluding this metadata commit.

## Task Commits

1. Task1 RED: `e1e3d7a` — exact XM pages and electrical-view tests.
2. Task1 GREEN: `23bb026` — exact XM windows using shared HALT selection.
3. Task2 RED: `ef4a5ce` — hidden HSC import and immutable storage tests.
4. Task2 GREEN: `c5536a3` — shared HSC visibility predicate and lifecycle documentation.
5. Task3: `4df5298` — independent POKEY phase/audio/IRQ controls, topology refusals and firmware snapshot strengthening.
6. Task1 endpoint completion: `e082c3b` — every CPU/MARIA nibble pair at both physical window endpoints and accurate shared-owner prose.

All task commits ran repository hooks with the configured GitHub noreply identity. No tracked files were deleted. The orchestrator owns STATE/ROADMAP/REQUIREMENTS updates; this executor did not write them.

## Verification and Failed History

All run URLs below are **N/A-local**. No execution log is committed.

The task commands ran `cmake --preset ci && cmake --build --preset ci`, then `ctest --preset ci` with `--no-tests=error --output-on-failure` and these exact filters:

- Task1: `^(cart[.]|bus[.]|maria[.]|diag[.]bankset_tracer$)` — exit0,51 checks; final run after endpoint expansion also exit0,51 checks.
- Task2: `^(core[.]hsc[.]|core[.]battery[.]|core[.]load[.]|core[.]get_memory[.]|abi[.]api_)` — exit0,25 checks.
- Task3: `^(pokey[.]|audio[.]|sally[.]irq$|core[.]hsc[.]|core[.]battery[.]|core[.]load[.])` — exit0,30 checks, including every prior POKEY/HSC assertion and the required CPU IRQ test.

| Command or check | Exit | Evidence and disposition |
|---|---:|---|
| Initial `ctest --preset ci -R '^cart[.]retail[.]boards$' --output-junit xm-red.xml --output-on-failure` |8|XM physical offset0 stays0 instead of the stored1; ROM answers28. Initial test-only open-bus setup and bank-tag expectation were corrected before implementation. |
| Corrected RED with filter `^(cart[.]retail[.]boards\|bus[.]halt[.]latency_two_cycles)$` |8|Both named tests execute and fail on absent XM control/page behavior. |
| `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/ci/xm-red.json --raw` |0|Unmodified CTest JUnit accepted as RED_EVIDENCE_OK; semantic inspection confirms planned assertions, not setup failure. |
| Task1 focused command after adding four-row XM edges |8|One newly written synthetic edge setup used the wrong fast/slow sequence. Corrected the test to the existing four-row independent timing table; production timing was unchanged. |
| HSC RED, filter `^core[.]hsc[.]decode$`, with `--output-junit xm-hsc-red.xml` |8|Hidden SRAM/ROM are visible and hidden stores corrupt imported bytes/generation. |
| `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/ci/xm-hsc-red.json --raw` |0|RED_EVIDENCE_OK; target fails on intended visibility/storage assertions. |
| First Task2 focused command |8|The new conflict case still carried forbidden52KiB RAM geometry. Built the intended no-RAM candidate before testing XM Auto/HSC overlap; final25 checks pass. |
| Initial independent `pokey.maps` continuity checks |0|Behavior already exists in the installed chip-select/mixer owners once XM control is implemented. Investigated rather than manufacturing RED. |
| Temporary gate-off mute fault; `ctest --preset ci -R '^pokey[.]maps$' --output-on-failure` |8|New phase/raw tuple/PCM assertions detect the fault. Removed mutation; complete30-check targeted suite passes0. |
| `sh build/ci/task08-jobserver-gate.sh` at4df5298 |0|658 ordinary tests in52.61s,6 release tests, actual packages, exact inventory and hygiene. |
| `sh build/ci/task08-jobserver-gate.sh` at final e082c3b |0|658 ordinary tests in60.02s,6 release checks in0.29s, repeated6 in0.27s, all3 packages, exact658-once inventory and hygiene. |
| `sh build/ci/task10-04-quality.sh` at4df5298 |0|First fresh existing BUILD_BASE producer against genuinef64d68f; retained as earlier-source evidence only. |
| `sh build/ci/task10-04-quality.sh` at final e082c3b |0|Fresh current658+6/base658+6, matching environment and trusted fixture/sample identities; base ordinary67.11s and release0.17s. |
| `git diff --check`, file existence and commit ancestry checks |0|All ten changed paths and six task commits exist; no unexpected deletion or untracked output. |

The quality wrapper uses the existing four-slot FIFO jobserver and invokes:

`GITHUB_EVENT_PATH="$PWD/build/ci/task10-04-event.json" GITHUB_EVENT_NAME=push cmake -DROOT=. -DBUILD=build/ci -DOUT=build/ci/task10-04-quality-current.json -DSHA="$(git rev-parse HEAD)" -DCONFIG=ci -DBUILD_BASE=ON -DSOURCE=local -P tests/cmake/quality_evidence.cmake`

The event's genuine `before` is the recorded pre-plan ledger commit f64d68fea48d2d96f15165b05ff170400689985c. The final producer authenticates candidate e082c3b22a546284ba6cbef20de6da31624242ce and freshly builds that independent base. Producer worktrees were checked for tracked changes and removed after measurement; JSON and disposable wrappers remain ignored local build output.

| Actual macOS arm64 ZIP | Candidate bytes | Genuine pre-plan bytes |
|---|---:|---:|
| Library |83242|82995|
| Runner |97788|97532|
| Libretro |88705|88525|
| Three-component sum |269735|269052|

Archive bytes remain report-only under DEC.61. This is a local per-plan comparison, not phase-versus-main performance or hosted/all-platform acceptance.

## Deviations from Plan

**Existing implementation retained.** Battery preparation, XM Auto provisioning, pre-allocation52KiB conflict checks and installed POKEY topology were already completed by preceding plans. No no-op `src/instance.c` change or duplicate sound implementation was added. Task3's initial GREEN was investigated: real independent raw/PCM/IRQ controls and an injected mute fault establish its existing behavior. No Task3 failing-production/GREEN implementation sequence is claimed. Tasks1/2 have genuine validated RED/GREEN commits; global TDD mode remains false.

**Test setup corrections.** Corrected newly authored open-bus/sentinel expectations, the four-row edge stimulus and no-RAM conflict setup using the existing documented owners. No old expected hashes, numeric audio records or timing constants changed. The explicit endpoint follow-up closes the planned all-pages/all-endpoints matrix.

**Optional ledger limitation.** Best-effort `windows append` for the documented existing-GREEN deviation and intentional HSC stub each returned exit1: pre-existing ledger frontmatter17/0/35/52 disagrees with entries16/0/36/52. No ledger content changed. This optional recorder failure does not replace passing product checks; shared metadata repair remains with the orchestrator.

## Architecture and Limits

The cartridge owns private page/control fields and one HSC visibility predicate; the bus retains electrical/time authority, and existing POKEY/synth owners retain chip identity and clocks. There is no new framework, dependency, allocation, device code, endpoint, file-access surface, quota or runtime fault protocol. The existing physical128KiB backing bounds page arithmetic through131071. Observers remain logical CPU views.

XM remains an implementation-backed RAM/POKEY/HSC subset with medium physical confidence. Gating both HSC windows is explicitly an inference; continuous sound follows chip-select evidence rather than a measured mute circuit. The inaccessible primary specification, unsupported ROF/BIOS/top-slot/flash/PIA/flat48KiB/protection/locks/PAL firmware and unsupported sound owners acquire no invented behavior.

## Known Stubs

The existing signature-only HSC stub at `src/instance.c:289` intentionally supplies no firmware routines, formatting or screens. It is the documented direct-SRAM/supplied-firmware contract, not an unfinished XM requirement. No new stub, skipped test or unrun verification was introduced. Best-effort ledger recording failed as described above.

## Self-Check: PASSED

All declared changed files and six task commits exist and are ancestors of the recorded source head. Final-source required workflow, focused checks, fresh independent comparison and tree hygiene pass. No routine owner verification remains.

## Next Readiness

The core subset and local controls are ready for adapter work. BOARD-04/05 remain pending; later diagnostics and final hosted acceptance are still owned by subsequent plans. Next separate allowed command: `$gsd-execute-phase 10`. No other GSD step was run.
