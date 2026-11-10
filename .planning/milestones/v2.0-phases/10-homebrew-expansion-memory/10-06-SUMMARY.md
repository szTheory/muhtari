---
phase: 10-homebrew-expansion-memory
plan: "06"
subsystem: libretro
tags: [memory-maps, rcheevos, bankset, xboard, xm]
requires:
  - phase: 10-05
    provides: Load-time expansion selection and physical battery attachment in libretro
provides:
  - Session-static descriptors restricted to stable CPU windows
  - Pinned resolver proof through CPU bank/page/gate stores and power cycles
  - Loaded-module map and SAVE_RAM lifecycle checks
affects: [expansion-diagnostics, phase-acceptance]
tech-stack:
  added: []
  patterns:
    - Existing static descriptors with explicit pointerless holes
    - Existing pinned MIT rcheevos resolver and generated CMake orchestration
key-files:
  created: []
  modified:
    - libretro/libretro.c
    - tests/libretro/rcheevos.c
    - tests/libretro/host.c
    - tests/libretro/tests.cmake
    - tests/inventory/ci.txt
    - docs/libretro.md
    - docs/embedding.md
key-decisions:
  - Keep expansion-overlappable cartridge and XM HSC ranges absent throughout the loaded session
  - Use CPU-half geometry for stable BankSet ROM descriptors
  - Keep physical cartridge, expansion and battery storage independent of static address visibility
requirements-completed: []
coverage:
  - id: D1
    description: Stable BankSet CPU ROM windows and deliberate shared versus HALT RAM visibility
    requirement: BOARD-04
    verification:
      - kind: integration
        ref: libretro.rcheevos and libretro.rcheevos.expansion
        status: pass
    human_judgment: false
  - id: D2
    description: XBoard and XM session-static overlay holes including XM HSC windows
    requirement: BOARD-05
    verification:
      - kind: integration
        ref: libretro.rcheevos.expansion
        status: pass
    human_judgment: false
  - id: D3
    description: Physical spans and battery identity survive CPU controls and power independently of map holes
    requirement: BOARD-05
    verification:
      - kind: integration
        ref: libretro.rcheevos.expansion and libretro.host.battery
        status: pass
    human_judgment: false
  - id: D4
    description: Actual loaded module clears released maps and saves before another profile loads
    requirement: BOARD-04
    verification:
      - kind: integration
        ref: libretro.host.memory_map and libretro.host.hsc_firmware with both pixel-format modules
        status: pass
    human_judgment: false
  - id: D5
    description: Ordinary workflow, exact test inventory and three-component package integrity remain valid
    requirement: BOARD-05
    verification:
      - kind: integration
        ref: sh build/ci/task08-jobserver-gate.sh at 8e41751
        status: pass
      - kind: other
        ref: tests/cmake/quality_evidence.cmake with BUILD_BASE=ON against a41f777
        status: pass
    human_judgment: false
actuals:
  tokens: 8536
  tasks: 2
  commits: 3
plan_head_before: a41f7779fe8903c15802417e76537df66b556ad0
plan_head_after: 8e41751a86bc1e3e94fc51a546fb45f3fdb2ed93
duration: 11min
started: 2026-10-26T18:48:21Z
completed: 2026-10-26
status: complete
---

# Phase 10 Plan 06: Stable frontend memory maps Summary

**BankSet CPU-half ROM descriptors and proven static holes for HALT RAM, expansion overlays and XM HSC, with physical storage preserved.**

## Accomplishments

- Excluded both banked and HALT-selected cartridge RAM from fixed RAM descriptors. Installed XBoard/XM suppress every cartridge descriptor at $4000-$7FFF for the whole session, including the older linear, SuperGame, Activision and Fractalus paths. XM suppresses HSC SRAM/ROM descriptors regardless of gate state. The descriptor capacity and aligned power-of-two construction are unchanged.
- BankSet uses one ROM half to derive linear bases, bank counts and fixed offsets. Only the CPU half supplies descriptors; switched $8000-$BFFF remains a hole. A 52 KiB CPU set exposes $3000-$3FFF except with XM installed. Explicit pointerless holes preserve access to later stable ranges.
- Extended the actual pinned resolver over all 65,536 addresses, all 42 accepted BankSet geometry/RAM/profile combinations, and 21 older-board/profile combinations. Expected BankSet ROM offsets are calculated independently from the cartridge geometry; shared RAM requires the exact physical pointer. Existing identified AM/OM resolver and library/module comparisons remain intact.
- Executed real SALLY LDA/STA instructions through the loaded machine's ordinary bus for all CPU ROM banks, all eight XBoard banks, all sixteen XM CPU/MARIA page selections and off/on/off HSC controls. Repeated resolver sweeps and descriptor comparisons prove session stability. Dedicated cartridge/expansion spans and SAVE_RAM retain pointer, length and topology through controls and power. The first changed HSC data store increments generation once; gate and same-value stores do not.
- The loaded-module memory-map case executes a program from fixed BankSet ROM, restores HSC before the first frame, and checks XM to Off to XBoard replacement, power reset, unload and malformed replacement. Released maps have zero descriptors and no frontend storage. Firmware cases now expect XM HSC holes while retaining standalone HSC descriptors and physical firmware validation.
- The new exact inventory row is `libretro.rcheevos.expansion`. Four named false descriptor families are rejected: HALT RAM fixed, hidden HSC exposed, off-by-one CPU half, and missing intermediate hole. No resolver, framework, dependency, per-frame descriptor update, device code or resource quota was added. The two hosted frontend cases are unchanged.

## Task Commits

1. Task 1 RED: `339dfc8` — test(10-06): expose unstable expansion memory maps
2. Task 1 GREEN: `f5b944d` — fix(10-06): publish only stable CPU memory windows
3. Task 2: `8e41751` — test(10-06): verify expansion map and storage lifecycles

The measured source diff is 34,144 characters, or 8,536 estimate-scale tokens. The persisted plan ledger measures three task commits from the genuine pre-plan base to the recorded source head; the summary commit is separate.

## Decisions and Sequencing

Followed D-16/D-18 using the existing owners and resolver. The cartridge image builder was linked during Task 1 so its RED test could reuse the established in-memory builders. The existing loaded-module firmware expectations were corrected in the same GREEN commit because XM's intended static holes necessarily change those assertions. These are narrow task-order adjustments within declared plan files, with no added product scope.

## Evidence

Every run URL below is **N/A-local**. Logs, generated images, JSON and wrappers remain ignored local output.

| Command or check | Exit | Observation |
|---|---:|---|
| `cmake --preset ci && cmake --build --preset ci` | 0 | Native fatal-warning configure/build succeeds. |
| `ctest --preset ci -R '^libretro[.]rcheevos$' --output-junit rcheevos-expansion-red.xml --output-on-failure` | 8 | Target executes and fails on missing stable BankSet CPU ROM and exposed conditional memory. |
| `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/ci/rcheevos-expansion-red.json --raw` | 0 | RED_EVIDENCE_OK for unmodified CTest JUnit; semantic inspection confirms intended map assertions, without setup/compiler failure. |
| Task 1 focused resolver/map/battery/firmware filter | 0 | Eight checks pass after GREEN. |
| First `ctest --preset ci -R '^libretro[.]rcheevos[.]expansion$' --no-tests=error --output-on-failure` | 8 | New fault stimulus removed a hole at a console-region boundary, which does not hide later regions; generation assertion also omitted the intentional changed HSC data store. |
| `build/ci/tests/muhtari-test-rcheevos --expansion` during stimulus diagnosis | 1 | Named observations distinguish the missing-hole stimulus; a temporary unconditional generation increment expectation was too broad. Replaced with a hole at $0100 within a resolver region and exact changed-store accounting. |
| Final combined focused filter below | 0 | Nine checks in 5.33 s; new expansion resolver in 1.11 s. All four false descriptor families reject. |
| RGB565 loaded-module memory_map, hsc_firmware and battery invocations below | 0 | Actual second pixel-format module passes lifecycle, physical saves and firmware cases. |
| `sh build/ci/task08-jobserver-gate.sh` at 8e41751 | 0 | Unchanged workflow: 659 ordinary tests in 60.10 s, six release tests in 0.23 s, three real ZIPs, repeated six release tests in 0.15 s, exact 659-once inventory and tree hygiene. |
| `sh build/ci/task10-06-quality.sh` at 8e41751 | 0 | Existing BUILD_BASE producer: genuine clean a41f777 base passes 658 ordinary tests in 59.97 s and six release tests in 0.11 s. Toolchain, fixture and sample identities match. |
| `git diff --check`, file existence, commit ancestry and deletion checks | 0 | All seven changed paths and all three commits exist; no tracked deletion or untracked generated output. |

The final combined filter is:

`ctest --preset ci -R '^(libretro[.]rcheevos([.]expansion)?|libretro[.]host[.](memory_map|battery|battery_order|hsc_firmware))$' --no-tests=error --output-on-failure`

The RGB565 commands use `build/ci/tests/muhtari-test-libretro-host build/ci/libretro/muhtari_libretro_rgb565.dylib`, followed respectively by:

- `memory_map build/ci/tests/input-carts/input.a78 build/ci/built/doublebuffer/doublebuffer.bas.a78 tests/diag/sg144.a78`
- `hsc_firmware build/ci/tests/libretro-hsc-system`
- `battery`

Task 1 has genuine RED/GREEN commits; no separate refactor was needed. Global TDD mode remains false. Local diagnostic failures above were new test-stimulus assumptions, resolved without production hardware changes or dropped assertions.

## Package Measurements

The wrapper uses the existing four-slot FIFO jobserver and invokes:

`GITHUB_EVENT_PATH="$PWD/build/ci/task10-06-event.json" GITHUB_EVENT_NAME=push cmake -DROOT=. -DBUILD=build/ci -DOUT=build/ci/task10-06-quality-current.json -DSHA="$(git rev-parse HEAD)" -DCONFIG=ci -DBUILD_BASE=ON -DSOURCE=local -P tests/cmake/quality_evidence.cmake`

Trusted event before: `a41f7779fe8903c15802417e76537df66b556ad0`. Candidate: `8e41751a86bc1e3e94fc51a546fb45f3fdb2ed93`. The independent clean comparison worktree was removed after the successful measurement.

| Actual macOS arm64 ZIP | Candidate bytes | Genuine pre-plan bytes |
|---|---:|---:|
| Library | 83241 | 83220 |
| Runner | 98477 | 98477 |
| Libretro | 89625 | 89617 |
| Three-component sum | 271343 | 271314 |

Archive bytes remain report-only under DEC.61. This comparison authenticates actual package outputs; it does not attribute every byte difference to code size or establish phase-versus-main performance.

## Limits and Readiness

No stubs, skipped tests, unrun plan verifications, new threat surface or unresolved blocking issue remains in this slice. No authentication gate occurred. Hardware confidence remains the established implementation-backed XM subset; these tests prove software memory-map/storage behavior, not physical XM measurements. BOARD-04/05 remain pending until later phase acceptance, so requirements-completed is intentionally empty. Final all-platform/hosted acceptance belongs to plan 13.

The orchestrator owns shared STATE/ROADMAP/REQUIREMENTS updates and the next plan dispatch. This executor stops after plan 06; plan 07 can continue within the current authorized execution step. If execution is resumed as a separate session, the allowed command is `$gsd-execute-phase 10`.

## Self-Check: PASSED

All seven declared changed files and this summary exist. The three task commits are ancestors of HEAD. Fresh focused/full/base-build evidence is recorded above; the source tree is clean after task commits, with generated output ignored and no unexpected deletion. Shared planning state was left to its designated orchestrator owner.
