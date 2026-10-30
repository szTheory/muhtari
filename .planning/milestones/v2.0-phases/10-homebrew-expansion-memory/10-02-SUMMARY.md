---
phase: 10-homebrew-expansion-memory
plan: "02"
subsystem: cartridge
tags: [bankset, ram, bus, maria, c17]
status: complete
requires:
  - phase: 10-01
    provides: Accepted actual-edge BankSet selection and causal DMA handoff at behavior revision6
provides:
  - All six equal-half BankSet ROM geometries and the explicit declaration matrix
  - Distinct shared and HALT RAM with redirected stores and simultaneous POKEY effects
  - Physical RAM spans and transactional standalone HSC overlap rejection
affects: [10-03, 10-04, bankset, expansion-memory]
tech-stack:
  added: []
  patterns:
    - Checked per-half offsets in the existing cartridge switch
    - One physical backing allocation and existing electrical selection authority
key-files:
  created: []
  modified:
    - src/cart.c
    - src/instance.c
    - src/ports.c
    - tests/cart/test_cart.c
    - tests/cart/images.c
    - tests/cart/images.h
    - tests/bus/test_bus.c
    - tests/core/test_core.c
    - docs/hardware/cartridge.md
key-decisions:
  - Resolve the standalone 52 KiB/HSC conflict in battery preparation before staging or allocation.
  - Preserve the accepted timing seam and behavior revision6 without expectation changes.
requirements-completed: []
requirements-acceptance: pending
coverage:
  - id: D1
    description: Six ROM geometries and the explicit no-RAM/shared/HALT declaration matrix with per-half banks and independent endpoints
    requirement: BOARD-04
    verification:
      - kind: unit
        ref: tests/cart/test_cart.c#bankset_geometry
        status: pass
    human_judgment: false
  - id: D2
    description: Physical RAM ownership, redirected stores, POKEY overlaps and both electrical selection boundaries
    requirement: BOARD-04
    verification:
      - kind: unit
        ref: tests/cart/test_cart.c#bankset_ram
        status: pass
      - kind: unit
        ref: tests/bus/test_bus.c#halt_views
        status: pass
    human_judgment: false
  - id: D3
    description: Executed CPU RAM stores and actual MARIA graphics distinguish shared and HALT RAM native pixels
    requirement: BOARD-04
    verification:
      - kind: integration
        ref: tests/bus/test_bus.c#bankset_ram_frame
        status: pass
      - kind: integration
        ref: diag.bankset_tracer
        status: pass
    human_judgment: false
  - id: D4
    description: Stable physical spans and no-allocation refusal of forbidden geometry and effective standalone HSC overlap
    requirement: BOARD-04
    verification:
      - kind: unit
        ref: tests/core/test_core.c#bankset_memory_lifetime
        status: pass
    human_judgment: false
  - id: D5
    description: Full required local workflow and fresh actual candidate/preplan three-component archive observations
    requirement: BOARD-04
    verification:
      - kind: integration
        ref: cmake --workflow --preset ci
        status: pass
      - kind: other
        ref: tests/cmake/quality_evidence.cmake
        status: pass
    human_judgment: false
actuals:
  tokens: 11825
  tasks: 2
  commits: 4
plan_head_before: c54e6c611e88e4a96bc0a96b6b86d5b3fef2f69c
plan_head_after: c066d0705fc931dd397b56ba17dc5459149b7049
duration: 12 min
started: 2026-10-26T17:34:40Z
completed: 2026-10-26
---

# Phase 10 Plan 02: BankSet Geometry and Physical RAM Summary

**Six checked CPU/MARIA ROM geometries, explicit shared/HALT RAM, independent overlapping writes and transactional standalone HSC conflict rejection pass the full local workflow.**

## Accomplishments

- Equal 32/48/52/128/256/512 KiB halves load without inferring RAM. The accepted RAM matrix is checked before allocation; unsupported A8/X2/EXROM/EXFIX and unrelated v4 mapper combinations remain authoritative refusals. BankSet header/body mismatches refuse. Raw v3/v4 identity and existing database/module/firmware refusal behavior remain protected.
- Linear sets end at FFFF. Switched sets use the second-last and last banks of each half, one shared bank-zero register, and modulo the per-half count. Independent literal geometry tables and unrelated CPU/MARIA first/last tags enumerate every linear block, switched bank, all256 register values and both selector endpoints. No production offset helper supplies expected values.
- Shared RAM exposes16384 physical bytes; HALT RAM exposes32768. CPU4000–7FFF stores update CPU RAM; C000–FFFF redirects update MARIA RAM while reads remain ROM. POKEY4000 writes also reach BankSet memory, including legacy declarations, while readable ROM/RAM retains ownership. The prior Fractalus POKEY priority assertion remains unchanged.
- Four stretched-cycle rows protect both RAM selection edges, CPU-view peeks, non-HALT VBE and the final DLL sample before release. Real CPU instructions and MARIA graphics produce literal native colors16hex for shared RAM and38hex for HALT RAM. MEN/BIOS priority and no frame-time allocations are asserted.
- Public spans retain pointer, length, bytes and complete machine state across forbidden RAM geometry and standalone HSC conflicts. Explicit On, Auto metadata and supplied firmware reject the52KiB overlap before allocation/staging; explicit Off permits the image. Power cycle retains span identity and zeros both physical halves.

## Task commits and TDD evidence

1. Task1 RED: `5f0a94a` — supported geometry/RAM loads refused5/9, and unrelated valid v4 mapper+BankSet declarations incorrectly succeeded0 and allocated.
2. Task1 GREEN: `eba71df` — checked matrix, per-half ROM mapping and documentation.
3. Task2 RED: `aa99497` — POKEY swallowed RAM writes, redirects were absent, DMA selected ROM instead of RAM, and the standalone HSC conflict incorrectly succeeded0 instead of9.
4. Task2 GREEN: `c066d07` — physical RAM ownership, independent write effects, actual graphics/edge/lifetime coverage and conflict validation.

Both tasks carry tdd=true and received genuine assertion failures before production changes. Complete unchanged JUnit documents passed `check tdd-red-evidence` with RED_EVIDENCE_OK; semantic inspection confirmed the intended targets executed after successful compilation/discovery. Global TDD_MODE remained false. No REFACTOR commit was needed.

## Verification evidence

Every run URL below is **N/A-local**. No execution log, generated ROM or archive is committed.

| Command | Exit | Observation |
|---|---:|---|
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^cart[.]retail[.]boards$' --output-junit build/ci/task10-02-task1-red.xml --no-tests=error --output-on-failure` | 8 | Intended Task1 geometry/declaration failures |
| `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/ci/task10-02-task1-red.json --raw` | 0 | RED_EVIDENCE_OK on actual nested CTest report |
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^cart[.]' --no-tests=error --output-on-failure` | 0 | Task1 final21/21 checks |
| `cmake --build --preset ci && ctest --preset ci -R '^(cart[.]retail[.]boards\|core[.]get_memory[.]cart_ram\|bus[.]halt[.]latency_two_cycles)$' --output-junit /tmp/muhtari-10-02-task2-red-final.xml --no-tests=error --output-on-failure` | 8 | All3 targets fail planned RAM/edge/conflict assertions |
| `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" check tdd-red-evidence build/ci/task10-02-task2-red.json --raw` | 0 | RED_EVIDENCE_OK; cartridge target independently identified |
| `cmake --preset ci && cmake --build --preset ci && ctest --preset ci -R '^(cart[.]\|bus[.]\|maria[.]\|core[.]get_memory[.]\|core[.]load[.]keeps_previous$\|diag[.]bankset_tracer$)' --no-tests=error --output-on-failure` | 0 | Task2 final54/54 checks |
| `sh build/ci/task08-jobserver-gate.sh` at c066d07 | 0 | Unchanged workflow657 ordinary tests in49.36s;3 actual ZIPs;6 release tests in0.11s; repeated6 release tests in0.13s; exact inventory and hygiene |
| `cmake -DJUNIT=build/ci/junit-release.xml -DINVENTORY=tests/inventory/ci.txt -DLABELS=release -P tests/cmake/check_junit.cmake` | 0 | Exactly6 release tests once each |
| `sh /tmp/muhtari-10-02-quality.sh` | 0 | Existing fresh producer: candidate657+6/base657+6; base ordinary42.96s |
| Independent Python archive count/size/SHA256 and JSON equality assertions | 0 | Exactly3 real ZIPs per side; sums match measured producer values |
| `git diff --check` and repository commit hooks | 0 | No whitespace or hygiene finding; no bypass |

The full-workflow helper uses the existing four-slot FIFO jobserver. Presets, workloads, assertions, inventory membership and deadlines are unchanged. The quality wrapper uses the same jobserver and this original producer invocation:

`GITHUB_EVENT_PATH="$PWD/build/ci/task10-02-event.json" GITHUB_EVENT_NAME=push cmake -DROOT=. -DBUILD=build/ci -DOUT=build/ci/task10-02-quality-current.json -DSHA="$(git rev-parse HEAD)" -DCONFIG=ci -DBUILD_BASE=ON -DSOURCE=local -P tests/cmake/quality_evidence.cmake`

The event's genuine before commit is c54e6c611e88e4a96bc0a96b6b86d5b3fef2f69c, source-equivalent to accepted59bad03. Candidate is c066d0705fc931dd397b56ba17dc5459149b7049. The producer created a clean detached base checkout, rebuilt it, ran its full workflow and authenticated matching environments. Both reports identify native/local/measured and the actual respective source SHA. No cached base is relabeled.

Environment: AppleClang21.0.0.21000101/Darwin-arm64; fixture750d8b722c88a03aff050dec5e961a44523a703afcc134f7dce4e3ebf5107932; trusted sample identity d549d4691aca7c004325102613fd6ac2337ecfe8a4c0cec004472e5bbcc80ca8.

| Actual macos-arm64 archive | Candidate bytes | Preplan bytes | Candidate SHA256 | Preplan SHA256 |
|---|---:|---:|---|---|
| library | 82544 | 81936 | e19c6de0fd074b8c6bd333c647c1cfa95e5fad4aea2110d1e8163b0933564b7f | 8c75849207e7b94294264ad2942b9f0ef4626ee55d1cda5de67ba3f09f7851e6 |
| libretro | 88143 | 87631 | 1810810b862307d0a94d8c0d0a57bce0fb1da990734b25426cf5ddc750ad7161 | 3488842595987433707450b17f61b1ee89dfeaf08fd045ed1fef713a2345e9d8 |
| runner | 97006 | 96425 | 41fc8e767bbec1c97d985c2211f9d3876e4874b49b2fb52415e40fa09d300603 | 8be1d63c88c9914ee67866f9fc30b0ea870bc628f96e53b1250e5e04209f7dbd |
| Total, report-only | 267693 | 265992 | +1701 bytes | No archive cutoff |

Actual base archives were preserved in ignored build output before the clean temporary checkout was removed. This is a local substantive-slice comparison, not hosted or phase-versus-main acceptance.

## Deviations from Plan

**[Rule2 - Missing critical validation] Standalone HSC conflict belongs to load resolution.** Task2's declared paths omitted src/instance.c, where effective On/Auto/firmware HSC attachment is determined. The orchestrator authorized the narrow six-line validation there. The reproduced public load succeeded and allocated before the fix; tests now prove refusal before allocation or staging, retained RAM/state, and explicit Off success. Committed in c066d07. XM-driven effective HSC remains the later load-resolution plans' responsibility.

## Issues Encountered

- Initial Task1 evidence assembly looked for CTest's relative JUnit path at the project root; CTest had written it under its build working directory. The missing-record attempt returned INVALID_RED/unreadable_record. The unchanged actual XML was located and passed machine/semantic validation before GREEN.
- First Task1 GREEN build exited1 on a signed complement under -Wsign-conversion; the explicit uint32_t cast corrected it. The following cartridge run exited8 because the new independent bus-peek fixture had left MEN off. Setting its explicit7800 state corrected the test; the final21/21 pass followed before commit.
- Initial Task2 RED exited8 with the intended failures but its lifetime test continued after the unexpected successful replacement. A guard now stops that path before further span access. The fresh complete three-target RED report was collected and validated after that test correction, before production changes.
- A later Task2 test build exited1 on the misspelled internal BIOS backing field. The fixture uses the existing bios field; final54/54 and the complete workflow pass. No production timing adjustment or broad hash regeneration was attempted.

## Confidence and readiness

The six-size/memory model is locally implemented and proved. The wiki documents through128KiB per set and HALT RAM; larger sizes and shared RAM remain explicitly qualified producer/emulator extensions. The accepted electrical timing remains a derived compatibility fit, not physical measurement. The two existing unclassified assumption rows remain unresolved as recorded by10-01; this plan introduces no new resolution claim.

No new dependency, public symbol, endpoint, file-access boundary, resource quota, expected hash or behavior revision was introduced. No new stub, skipped test or unrun task verification remains. Shared STATE/ROADMAP/REQUIREMENTS updates belong to the orchestrator and were not modified here. BOARD-04/05 remain pending; all-host geometry diagnostics, expansion conflicts, map consumers, native/PPC equality and final exact-source hosted acceptance remain later work. Preserve the already-completed minimal behavior6 portion when executing10-03.

Actuals are ceil(47299 realized diff characters/4),9 changed files and4 commits measured from the persisted plan ledger through c066d07, excluding this summary commit.

Next separate allowed command: `$gsd-execute-phase 10`. Within the active orchestrated step,10-03 is ready; this executor runs no other plan or GSD step.

## Self-Check: PASSED

All9 declared/authorized changed files exist. Commits5f0a94a, eba71df, aa99497 and c066d07 are ancestors of HEAD. Root/branch assertions, clean status and whitespace checks pass. Summary existence is checked in its immediate metadata commit command. No tracked file deletion occurred.
