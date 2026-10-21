---
phase: 04-sound
plan: "10"
subsystem: timer-interrupts
tags: [pokey, sally, irq, nmi, timing]
requires:
  - phase: 04-06
    provides: Completed actual-clock POKEY synthesis recovery with preserved RED history
  - phase: 04-09
    provides: Validated effective v4 wiring and fixed chip identities
provides:
  - Real timer to wired bus level to sampled SALLY decision and handler
  - Seven-cycle IRQ entry with persistent level and NMI priority and hijack
  - Timer enable and acknowledgment propagation with unheld CPU synchronization
  - Portable timer and CPU edge tests and interrupt-entry frame carry coverage
affects: [sound-integration, audio-observation, platform-conformance]
tech-stack:
  added: []
  patterns:
    - Fixed instance-local timer setup and CPU synchronization pipelines
    - Flat bus script preserves the production CPU object's two-function seam
key-files:
  created:
    - tests/audio/test_irq.c
  modified:
    - src/cpu.c
    - src/bus.c
    - src/internal.h
    - src/pokey.c
    - tests/core/test_core.c
    - tests/cpu/test_cpu.c
    - tests/cpu/vector_bus.c
    - tests/cpu/vector_bus.h
    - tests/audio/test_contracts.c
    - tests/audio/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - docs/hardware/sally.md
    - docs/hardware/pokey.md
key-decisions:
  - Use the manual's common three-unhalted-cycle status-to-entry convention and qualify analog variation.
  - Preserve fixed chip identity and validated v4 wiring and the original instruction poll marks.
  - Reuse actual half-edge delivery rather than add an IRQ deadline scheduler.
requirements-completed:
  - SND-02
  - SND-03
coverage:
  - id: D1
    description: All three timer IRQ sources reach a real handler and continued public PCM on primary and secondary and dual chip identities with unwired and invalid header controls
    requirement: SND-02
    verification:
      - kind: integration
        ref: core.audio.silence
        status: pass
    human_judgment: false
  - id: D2
    description: Active-low timer status and lost disabled events and enable setup and delayed acknowledgment propagation retain correct physical wiring while muted and in INIT and held
    requirement: SND-02
    verification:
      - kind: integration
        ref: sally.irq
        status: pass
    human_judgment: false
  - id: D3
    description: CLI and SEI and PLP use sampled old I while RTI uses restored I with concrete seven-cycle stack and vector accesses and persistent level retrigger
    requirement: SND-02
    verification:
      - kind: unit
        ref: cpu.irq.timing
        status: pass
    human_judgment: false
  - id: D4
    description: Deciding and final and held polls and branch timing and NMI priority and entry cycle-four hijack preserve BRK and JAM behavior
    requirement: SND-02
    verification:
      - kind: unit
        ref: cpu.irq.timing
        status: pass
      - kind: unit
        ref: ctest --test-dir build/ci -R ^cpu.nmi --output-on-failure --no-tests=error
        status: pass
    human_judgment: false
  - id: D5
    description: Real timer IRQ entry after combined RDY and HALT holds crosses a frame deadline with identical public PCM prefix and tail and the original finite carry bound
    requirement: SND-03
    verification:
      - kind: integration
        ref: audio.frames
        status: pass
    human_judgment: false
  - id: D6
    description: Existing flat CPU vectors retain their bus order and results and new portable checks reach CI and the exact inventories
    requirement: SND-02
    verification:
      - kind: integration
        ref: ctest --test-dir build/ci -L ^vectors$ --output-on-failure --no-tests=error
        status: pass
      - kind: other
        ref: cmake --workflow --preset ci
        status: pass
    human_judgment: false
actuals:
  tokens: 13532
  tasks: 2
  commits: 4
plan_head_before: 7680935
plan_head_after: b5f0ae2297468af51c2a12a50335425325cb9bd0
duration: 10min
completed: 2026-10-20
status: complete
---

# Phase 4 Plan 10: Timer IRQ Acceptance Summary

Wired timer events now reach the sampled CPU decision, documented stack/vector entry, a real handler, acknowledgment and continued public audio.

## Accomplishments

The bus publishes only accepted-v4 primary bit0 and fixed-secondary bit1 wiring. The CPU samples that level with I at its existing deciding cycle, retaining the decision across the final cycle. CLI/SEI/PLP use old I; RTI uses restored I. Branches retain their original deciding cycles and store one poll mark. The CPU still calls only its two bus functions; flat vector objects link independently of the chips.

IRQ shares seven-cycle entry with NMI: two reads at the next PC, PCH/PCL/status pushes with B clear and U set, then vector low/high. I is set and D retained. Eligible NMI wins; a later NMI through entry cycle4 hijacks the IRQ vector. Later edges wait for the first handler instruction. BRK preserves its return address and B-set status; a jam ignores both interrupt inputs. Persistent levels retrigger after RTI; a request revoked while masked leaves no queued CPU event.

Timers1/2/4 own active-low IRQST bits0/1/2; timer3 has no IRQ and no serial/keyboard peripheral is fabricated. Enable history enforces four-clock setup and two-clock disable boundaries. Status reads do not acknowledge; clearing IRQEN immediately clears status but cannot revoke an event already propagating. A late event may therefore reach the CPU with IRQST already reset. The bus synchronizer shifts only on completed unheld CPU accesses. Chip timers keep their actual clocks during RDY/HALT, mute and direct-clock INIT operation.

The manual reports a common three-unhalted-cycle minimum status-to-entry delay and system/temperature-dependent two-cycle cases. This deterministic model chooses two CPU synchronization cycles followed by the final instruction cycle; it does not claim analog setup/hold or metastability fidelity. The pinned January2,2026 Altirra manual was read as documentation, CPU section3.4 and POKEY section5.7. No emulator source was opened. Existing half-edge delivery already clocks every relevant event, so no additional deadline scheduler is needed.

`core.audio.silence` executes48 generated cartridge cases: three timer sources, four primary/lone-secondary/dual identities, and accepted/unwired/v3/invalid-v4 metadata. It checks handler entry, stack values, a single acknowledgment, bounded first-frame output and nonzero PCM. `sally.irq` asserts concrete status/line clocks, T4/T5 enable and T6/T7/T8 disable boundaries, lost masked events, both wiring identities, shared-line acknowledgment, muted INIT and held synchronization. `cpu.irq.timing` checks every entry access and direction, flags, instruction delays, branches, arrival/revocation timing, held polls, priority/hijack, BRK, JAM and persistent RTI. Existing NMI tests remain unchanged.

`audio.frames` extends the existing carry stress to real POKEY IRQ as well as NMI. A five-cycle indirect JMP gives a deciding poll after synchronization following combined WSYNC-style RDY and DMA-style HALT holds. The real interrupt entry crosses the horizon and preserves reference/public PCM prefix and tail, stack depth, clock parity, the original20730-tick/139-sample carry limit and the940-sample FIFO. Production synthesis remains exact-q23-16x64-dc20, M18360, sixteen taps,85:342 currents, Q12 gain4096 and48kHz. No video/raw/PCM hash repin, behavior revision, API3 or ABI1 change was required.

## Task commits and TDD

| Boundary | Commit | Result |
|---|---|---|
| Task1 RED | 2e434c2 | Compiling real cartridge test fails handler and stack assertions before IRQ acceptance |
| Task1 GREEN | bceeba9 | Full CI and focused core/NMI checks pass before expansion |
| Task2 RED | 7bebc73 | Compiling timer suite fails propagation/setup assertions; CPU timing suite passes |
| Task2 GREEN | b5f0ae2 | Complete timer propagation, unheld synchronization and final timing/carry coverage pass |

Task1 initial CTest exited8 with concrete missing-handler/stack assertions; no machine-classifier verdict is claimed for that initial run. A fresh final tracer negative control removes CPU IRQ acceptance and reproduces those missing-handler assertions; its unchanged JUnit report passes `check tdd-red-evidence` with `RED_EVIDENCE_OK`. Task2 RED exits8 and its unchanged two-test JUnit report passes the same classifier, identifying `sally.irq` as the failing test and the CPU case as passing. Evidence records remain ignored build output, not committed logs. Initial tracer construction corrected its JMP operand, controlled boot stack/RAM, and first-frame54-sample expectation before accepting RED. A later expanded harness corrected an opcode/operand index before production verification.

Nine compiling transient mutations each make the targeted CTest exit8: remove CPU IRQ acceptance; check final live I; push B set; drop primary wiring; drop secondary wiring; enable immediately; lose already-propagating events; advance CPU synchronization through held cycles; fabricate timer3 IRQ. All are restored and the shipped objects rebuilt. An initial impossible-mask mutation failed compilation and is excluded; its compiling replacement was tested. The final negative controls run after the unheld synchronization correction.

Metrics count four task commits,15 changed files and ceiling(sum of their realized diff characters/4), not runtime token usage. Duration is ten minutes from the initial tracer script creation through final task closeout.

## Commands and results

- `cmake --workflow --preset ci` — exit0;597 non-release plus6 release checks,603 total, after final production changes.
- `ctest --test-dir build/ci -L '^vectors$' --output-on-failure --no-tests=error` — exit0;266 checks including every opcode sample.
- `ctest --test-dir build/ci -R '^core.audio.silence$|^cpu.nmi|^cpu.irq|^sally.irq$|^audio.frames$' --output-on-failure --no-tests=error` — exit0;10 checks.
- `cmake --preset dev` and `cmake --build --preset dev` — exit0; final focused Debug selection including POKEY, core, IRQ/NMI and frame audio — exit0;13 checks with assertions enabled.
- `cmake --workflow --preset hygiene` — exit0;24 checks including format/analyzer. Earlier no-float failures were source-comment citations without the recognized `section` spelling; correcting the citations resolves them without changing the gate.
- `scripts/hygiene.sh --tree` — exit0, including the full hygiene workflow's tree check.
- `git diff --check` — exit0.
- `node "$GSD_TOOLS" check tdd-red-evidence build/ci/irq-red-evidence.json --raw` and the final tracer negative-control record — exit0; both `RED_EVIDENCE_OK`.
- python3 /tmp/muhtari-irq-mutations.py — exit0; nine builds exit0 and nine targeted test commands exit8, followed by restored-object rebuild exit0.

No push or hosted run was requested or dispatched; there is no hosted run URL. This is local macOS arm64 Release/Debug evidence. Platform parity remains later work. No logs, generated cartridge bytes, package or owner UAT were added.

## Deviations from Plan

[Rule2 — missing registration scope] The task file lists omit `tests/cpu/test_cpu.c`, the new timer target, registration, inventories and the existing carry harness, although the actions explicitly require these behaviors and the phase outputs name the timer target. Extended these existing seams and exact inventories so CI runs every new assertion. This adds no independent policy gate or dependency. Verified by full CI and hygiene; commits7bebc73/b5f0ae2.

The final documentary check clarified that synchronization must count unhalted CPU cycles. Moved that delay from the chip to the bus before task completion, added the held-clock assertion and reran all final checks and negative controls. Timer eligibility still uses actual chip clocks. This fulfills the existing source-derived timing requirement rather than changing a settled choice.

## Remaining work

The frontmatter requirements describe this plan's verified contribution. Global SND-02/SND-03 remain pending their other plans; `requirements.ready-ids` reports both blocked. Phase4 remains in progress at10/17. Later waves retain observation/serialization, hash migration, host integration, workloads and platform evidence. No phase verification or subsequent GSD step ran. Next: `$gsd-execute-phase 4 --wave 11`.

## Self-Check: PASSED

All15 task files exist; all four task commits are ancestors of the current branch. Final CI, hygiene, vectors, focused Release/Debug and restored negative controls pass. No tracked file was deleted. Source documentation matches the adopted timing and its physical limits.
