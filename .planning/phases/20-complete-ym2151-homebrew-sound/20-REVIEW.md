---
phase: 20-complete-ym2151-homebrew-sound
reviewed: 2026-11-12T10:50:20Z
reviewed_head: 302af89e5f43e4ba667cf4ecfd3d7ad50a4f8bb5
review_round: 1
depth: standard
files_reviewed: 63
files_reviewed_list:
  - .github/workflows/ci.yml
  - CMakeLists.txt
  - PROVENANCE.md
  - README.md
  - THIRD-PARTY-NOTICES.md
  - docs/embedding.md
  - docs/hardware/audio.md
  - docs/hardware/cartridge.md
  - docs/hardware/ym2151.md
  - docs/libretro.md
  - docs/maintaining.md
  - docs/runner.md
  - docs/testing.md
  - include/muhtari.h
  - libretro/libretro.c
  - libretro/muhtari_libretro.info.in
  - scripts/emscripten.sh
  - src/bus.c
  - src/cart.c
  - src/instance.c
  - src/internal.h
  - src/state.c
  - src/synth.c
  - src/ym2151.c
  - tests/api/v8.c
  - tests/audio/expected.txt
  - tests/audio/test_ym2151.c
  - tests/audio/tests.cmake
  - tests/audio/ym_oracle.cmake
  - tests/audio/ym_oracle.cpp
  - tests/audio/ym_vectors.h
  - tests/built/expected.txt
  - tests/cart/images.c
  - tests/cart/images.h
  - tests/cmake/hashes.cmake
  - tests/cmake/quality.cmake
  - tests/cmake/quality_evidence.cmake
  - tests/consumer/main.c
  - tests/core/state_compat.c
  - tests/core/state_compat.cmake
  - tests/core/test_core.c
  - tests/core/test_state.c
  - tests/core/tests.cmake
  - tests/diag/driver.c
  - tests/emscripten/bundle.cmake
  - tests/emscripten/host.mjs
  - tests/emscripten/tests.cmake
  - tests/emscripten/ym.cmake
  - tests/install/tests.cmake
  - tests/inventory/ci.txt
  - tests/inventory/emscripten.txt
  - tests/inventory/wasm32.txt
  - tests/libretro/host.c
  - tests/libretro/observe.c
  - tests/libretro/tests.cmake
  - tests/libretro/ym_observe.cmake
  - tests/policy/publish.cmake
  - tests/policy/quality.cmake
  - tests/policy/tests.cmake
  - tests/release/tests.cmake
  - tests/release/ym_consumers.cmake
  - tests/runner/tests.cmake
  - tests/state-compat/CMakeLists.txt
findings:
  critical: 0
  warning: 0
  info: 0
  total: 0
  necessity: 0
status: clean
---

# Phase 20: Code Review Report

**Reviewed:** 2026-11-12T10:50:20Z
**Depth:** standard
**Files Reviewed:** 63
**Reviewed Head:** `302af89e5f43e4ba667cf4ecfd3d7ad50a4f8bb5`
**Status:** clean

## Summary

Re-reviewed the seven files changed by round one against the three findings from the initial review. All three original issues were reproduced before the fixes and are resolved at the reviewed head; no regression or unresolved issue was found in this delta. The retained findings below include the before-fix reproduction and after-fix evidence. Final native, hygiene, sanitizer, oracle, compatibility and portable refresh gates and cost-document updates remain pending and are outside this review-fix disposition.

## Narrative Findings (AI reviewer)

### Resolved Findings (round 1)

#### CR-01: YM address-port reads replace the open-bus value

**File:** `src/ym2151.c:770-778`
**Severity:** BLOCKER

**Disposition:** RESOLVED at `302af89e5f43e4ba667cf4ecfd3d7ad50a4f8bb5`. Before the fix, `ym.profile` failed for open-bus latches `0x00`, `0x5a`, and `0xa5`; after the fix, `ym.profile` passed in `build/v6-phase20-review1-after.log` (24/24 targeted tests passed).

**Issue:** Both `muhtari__ym2151_read` and `muhtari__ym2151_peek` return `0xff` for the address port at `$0460`. A CPU read then stores that value into `m->bus.open_bus` in `src/bus.c:432-438`. The selected hardware contract explicitly requires `$0460` reads to return the current open-bus value (`docs/hardware/ym2151.md:12-14`), so guest reads receive the wrong byte and corrupt the latch for following undriven reads.

**Fix:** For the even YM port, return `m->bus.open_bus` in the read path and the corresponding latched value in the peek path; keep status reads at `$0461` unchanged.

#### CR-02: Some rejected YM reloads tear down the currently loaded game

**File:** `libretro/libretro.c:1368-1390,1553-1560`
**Severity:** BLOCKER

**Disposition:** RESOLVED at `302af89e5f43e4ba667cf4ecfd3d7ad50a4f8bb5`. Before the fix, `libretro.host.ym` failed for unsupported Covox, ADPCM, POKEY 4000, POKEY 800, and Souper/YM reloads because the session was cleared. The updated preflight rejects these variants before teardown; the after-fix host test passes in `build/v6-phase20-review1-after.log`.

**Issue:** The preflight rejects YM without XM, a non-XM expansion selection, and Covox/ADPCM, but it omits the remaining core load predicates in `src/instance.c:354-364`: conflicting primary POKEY declarations and POKEY 4000/800 combined with an active expansion. For example, a valid v4 linear header with XM device, IRQ bit 2, and audio value `0x0d` declares YM plus POKEY 4000; the probe succeeds, this preflight allows it, and `muhtari_load` rejects it. A reachable v4 Souper/YM header instead returns `ERR_UNSUPPORTED_BOARD`; `muhtari_probe` still fills the output metadata for that status (`src/cart.c:794-815`), but the preflight treats every non-OK probe as “not a YM refusal.” In both cases `retro_load_game` reaches `lr_release()` before `lr_load_content` fails, clearing the previously loaded session.

**Fix:** Make the preflight cover every YM-related refusal predicate used by `muhtari_load`, including the primary POKEY combinations, and inspect populated YM metadata when probe returns `ERR_UNSUPPORTED_BOARD`. Return before `lr_release()` for these rejected YM configurations. Add reload-preservation coverage for a POKEY conflict and an unsupported board carrying YM metadata.

#### WR-01: Public state accepts YM clocks that do not match the reset epoch

**File:** `src/state.c:1182-1193,1234-1238`
**Severity:** WARNING

**Disposition:** RESOLVED at `302af89e5f43e4ba667cf4ecfd3d7ad50a4f8bb5`. The pre-fix `core.state.ym` run accepted CRC-repaired clock/remainder inconsistencies; the new public-resume relation rejects a state with clocks reduced by 64 and `total_clocks` reduced by one, as well as an independently flipped remainder, transactionally. `core.state.ym` passes in `build/v6-phase20-review1-after.log`; the BIOS reset-origin-zero save/load/replay check passes in `build/v6-phase20-review1-bios.log`.

**Issue:** The validator checks that YM clocks do not exceed the absolute tick-derived ceiling, and that `sample_divider` and `total_clocks` agree with the supplied clock count, but it permits an arbitrary smaller count. On genuine states, YM reset sets `last_tick` to the bus tick at the reset origin (`src/ym2151.c:700-709`); fast boot sets that origin to 108984 NTSC or 140016 PAL before resetting the cartridge (`src/boot.c:72-79`), while BIOS boot resets the bus and YM epoch to zero (`src/boot.c:108-123`). Completed bus accesses keep YM caught up to the bus tick, and the audio tests assert `clocks == (last_tick - reset_origin) / 2` and matching remainder parity (`tests/audio/test_ym2151.c:1065-1078,1083-1106`). A CRC-repaired public state can therefore subtract 64 from `clocks` and one from the consistent modulo `total_clocks`, retaining `sample_divider` and `last_tick`; after an idle interval it passes the current checks. This is not the stale-`last_tick` catch-up problem already guarded at lines 1234-1238. It changes later Timer B programming because `ym_mode_write` aligns its start using `total_clocks & 15` (`src/ym2151.c:614-625`).

**Fix:** Once `last_tick` is known to be at or after `MARK_BUS_ORIGIN_TICK`, require `clocks == (last_tick - MARK_BUS_ORIGIN_TICK) / 2` and `clock_remainder == (last_tick - MARK_BUS_ORIGIN_TICK) % 2` for public resume. Keep the existing overflow and catch-up guards.

---

_Reviewed: 2026-11-12T10:50:20Z_
_Reviewer: the agent (gsd-code-reviewer)_
_Depth: standard_


## Implementation response — review fix round 1

All three findings were independently reproduced by the real registered tests
before source fixes (`build/v6-phase20-review1-before.log`, exit 8): non-FF
open-bus reads, both missing POKEY refusals plus Souper reload preservation, and
CRC-repaired modulo-preserving clock/remainder faults. Corrections retain the
public API/ABI, legacy snapshots and current YM envelope. The focused 24-case
YM/state/native-observer/loaded-module gate passes after fixes
(`build/v6-phase20-review1-after.log`, exit 0). A generated BIOS loop additionally
proves reset-relative state replay in both regions
(`build/v6-phase20-review1-bios.log`, exit 0). URLs N/A-local. Independent
re-review was completed at `302af89e5f43e4ba667cf4ecfd3d7ad50a4f8bb5`; all
three findings are resolved, with no unresolved issue found in this delta.
This records implementation evidence and review disposition, not phase sign-off.
Final phase gates remain pending. No formal gap round has been used.
