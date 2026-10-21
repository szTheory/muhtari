---
phase: 04-sound
plan: "09"
subsystem: cartridge-audio-conformance
tags: [pokey, cartridge, mapping, dma, wiring]
requires:
  - phase: 04-06
    provides: Verified actual-clock POKEY and exact-q23-16x64-dc20 reconstruction recovery
  - phase: 04-08
    provides: Runtime cartridge mappings and atomic behavior revision 2 migration
provides:
  - Focused portable POKEY map conformance through SALLY and public mono PCM
  - ROM and RAM overlap priorities plus pure peek and actual MARIA fetch coverage
  - Legacy and hash normalization and accepted-v4 IRQ metadata coverage
  - Repaired formatting of the preceding mapping implementation
affects: [irq, audio-hashes, platform-conformance]
tech-stack:
  added: []
  patterns:
    - Generated in-memory cartridge programs and private identification tables
    - Targeted transient mutations validate existing shipped behavior assertions
key-files:
  created: []
  modified:
    - tests/audio/test_pokey.c
    - tests/audio/tests.cmake
    - tests/inventory/ci.txt
    - tests/inventory/wasm32.txt
    - docs/hardware/pokey.md
    - src/cart.c
    - src/pokey.c
    - tests/core/test_core.c
key-decisions:
  - Preserve fixed primary and secondary slots rather than compacting a lone 0440 into the primary slot.
  - Test existing mapping behavior with rejected mutations rather than introducing artificial production changes for RED.
requirements-completed:
  - SND-02
  - SND-03
coverage:
  - id: D1
    description: All five legacy and v4 layouts execute CPU register programs and return audible public mono PCM
    requirement: SND-02
    verification:
      - kind: integration
        ref: pokey.maps
        status: pass
    human_judgment: false
  - id: D2
    description: Every mirrored block and window edge retains the expected register and fixed source identity across reload and reset
    requirement: SND-02
    verification:
      - kind: integration
        ref: pokey.maps
        status: pass
    human_judgment: false
  - id: D3
    description: Overlapping ROM and RAM retain read priority and actual MARIA fetches and peeks bypass POKEY register accesses
    requirement: SND-02
    verification:
      - kind: integration
        ref: pokey.maps
        status: pass
    human_judgment: false
  - id: D4
    description: Legacy and hash mapping normalization and accepted versus ignored v4 metadata preserve primary and secondary IRQ identities
    requirement: SND-03
    verification:
      - kind: integration
        ref: pokey.maps
        status: pass
    human_judgment: false
  - id: D5
    description: Focused registration reaches the existing CI and portable inventory and the touched hardware documentation matches implemented priorities
    requirement: SND-02
    verification:
      - kind: other
        ref: cmake --workflow --preset ci
        status: pass
      - kind: other
        ref: cmake --workflow --preset hygiene
        status: pass
    human_judgment: false
actuals:
  tokens: 8831
  tasks: 2
  commits: 3
plan_head_before: 23d12614c74d3bcb1132f7768461d1b8429ad7b8
plan_head_after: eb2c3028e1a797d34c29b8d0100cf5e58c6d80db
duration: 9min
completed: 2026-10-20
status: complete
---

# Phase 4 Plan 9: Mapped POKEY Conformance Summary

`pokey.maps` now proves declared cartridge CPU-to-PCM paths, distinct observation/DMA priorities and validated wiring metadata against the shipped archive.

## Accomplishments

- Ten generated cartridge cases cover all five layouts in legacy and v4 headers: $4000-$7FFF, $0450-$045F, $0800-$0FFF, $0440-$044F and dual $0440+$0450. Real SALLY instructions enter INIT, read documentary RANDOM/IRQST=$FF, program timers and return nonzero public PCM. The INIT read waits for the documented drain. Every mirrored block, both window edges, changed layouts and power cycles are checked.
- Dual PCM differs from either single-chip rendering. A second comparison holds dual-chip normalization constant and mutes each input in turn, proving both inputs reach mono synthesis. A lone $0440 retains the secondary slot, source IDs6-9 and bit1 identity; no slot compaction was introduced.
- Linear48/64K ROM, SuperGame EXROM/EXFIX, linear/SG RAM and banked RAM overlaps check concrete ROM/RAM bytes and write priority at the start, middle and end of the window. Banked RAM's other half is covered. Peeks and MARIA board reads preserve the whole instance. Actual DMA objects fetch from mapped RANDOM addresses and assert line-RAM pixels and unchanged chip, synthesis and CPU clock state.
- CPU RANDOM/IRQST reads clock the chips without acknowledging requests; clearing IRQEN acknowledges them. All accepted v4 audio codes and all eight IRQ words, invalid mapper/options/audio/IRQ and geometry, version3 reserved metadata, future-version handling, hash overrides, legacy conflicts and primary-location precedence are covered. Reload/reset remove stale mappings, levels and wiring.
- `pokey.maps` is registered with pokey/portable labels in both exact applicable inventories. Mapping/side-effect/wiring documentation cites the existing cartridge preparation and public hardware notes. No fixture bytes, package or runtime dependency was added.

## Task commits

| Task | Commit | Purpose |
|---|---|---|
| 1 | 6cc211d | All declared layouts through SALLY, public PCM, mirrors, edges and reload |
| 2 support | 3a0d5be | Whitespace-only formatting repair for preceding mapping code/tests |
| 2 | eb2c302 | Overlap, peek, actual DMA, hash normalization and effective-v4 wiring coverage |

The plan extends existing behavior rather than adding a missing runtime feature. No artificial RED implementation commit was needed for this test-only slice. Eleven compiling mutations each made the focused executable exit1 with a concrete assertion. Every mutation was restored and the original production behavior reverified before committing. Metrics count the three implementation/test/style commits, eight changed files and ceiling(realized diff characters/4); they are not harness token usage.

## Commands and results

All commands ran from the repository root. No execution logs are committed.

| Command | Exit | Evidence |
|---|---|---|
| `cmake --workflow --preset ci` | 0 | 595 main tests plus6 release tests; exact executed inventory includes pokey.maps |
| `ctest --test-dir build/ci -L '^pokey$' --output-on-failure --no-tests=error` | 0 | pokey.logic, pokey.clock and pokey.maps pass |
| `cmake --preset dev` | 0 | Assertion-enabled Debug configuration |
| `cmake --build --preset dev --target muhtari-test-pokey` | 0 | Focused Debug target built against shipped objects |
| `ctest --test-dir build/dev -L '^pokey$' --output-on-failure --no-tests=error` | 0 | All3 focused tests pass with core assertions enabled |
| `cmake --workflow --preset hygiene` | 0 | All24 hygiene checks pass |
| `scripts/hygiene.sh --tree` | 0 | Tree policy passes |
| `git diff --check` | 0 | No whitespace errors |

Hosted run URL: unavailable for this local execution step; no push or pull request was requested. Cross-platform/hosted confirmation remains the planned platform and ship work.

The task1 tracer's complete CI command passed before expansion and passed again at its automated feedback gate. A final full workflow run passed after all source/test edits.

### Mutation evidence

For each case below, the command `cmake --build --preset ci` exited0, then `build/ci/tests/muhtari-test-pokey maps` exited1. These were transient source changes, never committed:

| Mutation | Concrete assertion rejecting it |
|---|---|
| Remove450 primary mapping | Expected location $0450 and actual programmed state |
| Truncate mirrored windows to16bytes | Mirrored IRQST must return $FF rather than floating $1F |
| Treat lone secondary as primary | Secondary selector and fixed raw source identity |
| Read POKEY before overlapping memory | Expected ROM $5A or RAM $63 |
| Send MARIA fetch through CPU register path | Actual DMA pixel value and unchanged chip/synthesis state |
| Send peek through clocked chip read | Expected floating byte and whole-instance equality |
| Wire raw IRQ from ignored v4 | Effective mask must be zero |
| Drop primary IRQ bit | Accepted effective mask must retain bit0 |
| Drop secondary IRQ bit | Accepted effective mask must retain bit1 |
| Store chip writes into overlapping RAM | RAM sentinel must survive the chip write |
| Disconnect secondary from dual mixer | Muting secondary must change dual-normalized PCM |

An initial peek mutation was rejected at compilation by the existing const-qualification warning; the listed successful mutation used an explicit integer-pointer round trip to reach and exercise the runtime assertions. This compiler rejection is not counted among the eleven runtime failures.

## Deviations and issues resolved

The first hygiene run exited8 because wave8 left clang-format violations in `src/cart.c`, `src/pokey.c` and `tests/core/test_core.c`. Comparing their committed versions reproduced the failures. Formatting those files is the only added scope; removing whitespace from before/after text proves the repairs changed no tokens. The final fresh hygiene workflow passes24/24. Existing code-review and gap-round budgets are untouched.

Test development initially read RANDOM before INIT had drained and let an adjacent dual-chip write change the other chip's SKCTL. The corrected harness follows documented drain timing and initializes each chip before its boundary check. No product behavior was changed to accommodate these expectations.

## Limits and next wave

The requirement IDs above record this plan's verified contribution. Global SND-02/SND-03 remain pending their other shared plans. This wave checks timer register status and effective cartridge wiring metadata; actual NMOS CPU IRQ sampling/entry is wave10. Canonical transition observation/serialization and cross-platform audio hashes remain later slices.

The finite tests establish the implemented functional model, not cold-silicon startup, cartridge analog loading or physical mix calibration. The recovered exact-q23-16x64-dc20 synthesis, M18360,16-tap support,85:342 currents,Q12gain4096,48kHz cadence, boot/frame timeline, behavior revision2 and API3/ABI1 are preserved. There are no new video/transition/PCM pins or public interfaces.

Wave9 is complete,9/17 plans are summarized, and phase-level verification/completion has not run because eight plans remain. Next command: `$gsd-execute-phase 4 --wave 10`.

## Self-Check: PASSED

Both task commits and the formatting commit exist on the phase branch; the summary, focused registration and both inventory rows exist. The final CI, focused Release/Debug, hygiene and tree checks pass. No tracked file was deleted. No owner verification or setup is required.
