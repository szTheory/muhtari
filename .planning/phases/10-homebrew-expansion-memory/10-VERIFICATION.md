---
phase: 10-homebrew-expansion-memory
verified: 2026-10-28T19:00:00Z
status: gaps_found
score: 4/5 must-haves verified
covered_files:
  - ".planning/phases/10-homebrew-expansion-memory/10-01-PLAN.md"
  - ".planning/phases/10-homebrew-expansion-memory/10-01-SUMMARY.md"
  - ".planning/phases/10-homebrew-expansion-memory/10-02-PLAN.md"
  - ".planning/phases/10-homebrew-expansion-memory/10-02-SUMMARY.md"
  - ".planning/phases/10-homebrew-expansion-memory/10-03-PLAN.md"
  - ".planning/phases/10-homebrew-expansion-memory/10-03-SUMMARY.md"
  - ".planning/phases/10-homebrew-expansion-memory/10-04-PLAN.md"
  - ".planning/phases/10-homebrew-expansion-memory/10-04-SUMMARY.md"
  - ".planning/phases/10-homebrew-expansion-memory/10-05-PLAN.md"
  - ".planning/phases/10-homebrew-expansion-memory/10-05-SUMMARY.md"
  - ".planning/phases/10-homebrew-expansion-memory/10-06-PLAN.md"
  - ".planning/phases/10-homebrew-expansion-memory/10-06-SUMMARY.md"
  - ".planning/phases/10-homebrew-expansion-memory/10-07-PLAN.md"
  - ".planning/phases/10-homebrew-expansion-memory/10-07-SUMMARY.md"
  - ".planning/phases/10-homebrew-expansion-memory/10-08-PLAN.md"
  - ".planning/phases/10-homebrew-expansion-memory/10-08-SUMMARY.md"
  - ".planning/phases/10-homebrew-expansion-memory/10-09-PLAN.md"
  - ".planning/phases/10-homebrew-expansion-memory/10-09-SUMMARY.md"
  - ".planning/phases/10-homebrew-expansion-memory/10-10-PLAN.md"
  - ".planning/phases/10-homebrew-expansion-memory/10-10-SUMMARY.md"
  - ".planning/phases/10-homebrew-expansion-memory/10-11-PLAN.md"
  - ".planning/phases/10-homebrew-expansion-memory/10-11-SUMMARY.md"
  - ".planning/phases/10-homebrew-expansion-memory/10-12-PLAN.md"
  - ".planning/phases/10-homebrew-expansion-memory/10-12-SUMMARY.md"
  - ".planning/phases/10-homebrew-expansion-memory/10-13-PLAN.md"
  - ".planning/phases/10-homebrew-expansion-memory/10-13-SUMMARY.md"
  - "libretro/libretro.c"
  - "runner/main.c"
  - "scripts/hygiene.sh"
  - "src/bus.c"
  - "src/cart.c"
  - "src/instance.c"
  - "src/maria.c"
  - "tests/diag/expected.csv"
  - "tests/runner/tests.cmake"
covered_digest: "v4:sha256:21074b69b16553698faf99594249ea91fa7831e10edbedd26e37a25baaaadaec"
behavior_unverified: 0
overrides_applied: 0
gaps:
  - truth: "Unsupported boards retain existing refusals (plan 10-05): a refused image exits 77 with its refusal message, as docs/runner.md and runner/exit_status.h document"
    status: failed
    reason: "With --hsc-rom, the new pre-check in load() tests `probed != MUHTARI_STATUS_OK` and exits 1 with 'HSC firmware requires attached HSC' before muhtari_load can return the refusal. Reproduced: a 2600 image with --hsc-rom (with or without --hsc on) exits 1; without --hsc-rom the same image exits 77. Before commit 01a61d0 the request reached muhtari_load and mapped to 77."
    artifacts:
      - path: "runner/main.c"
        issue: "line 1182: `if (!attached || probed != MUHTARI_STATUS_OK)` conflates a refused image with a detached HSC; message is false for the refused-image case"
    missing:
      - "Enforce the attachment rule only when probed == MUHTARI_STATUS_OK; let a failed probe fall through to muhtari_load so the canonical 77 mapping applies"
      - "A runner test that passes --hsc-rom with a 2600 image (and with an unsupported-board image) and expects exit 77"
re_verification: false
---

# Phase 10: Homebrew and expansion memory Verification Report

**Phase Goal:** Run BankSet and XBoard/XM memory/POKEY/HSC capabilities using the correct CPU/MARIA ownership.
**Verified:** 2026-10-28
**Status:** gaps_found
**Re-verification:** No, initial verification

The goal itself is achieved in the code. One regression of a documented refusal contract in the runner (CR-01) was reproduced and is recorded as a gap. It is small and does not touch the BankSet or XBoard/XM behavior.

## Goal Achievement

### Observable Truths

| # | Truth (roadmap success criteria first) | Status | Evidence |
|---|---|---|---|
| 1 | BankSet diagnostics exercise separate CPU/MARIA ROM and RAM views on both HALT edges for each supported checked geometry | VERIFIED | `src/cart.c` `cart_bankset_rom_at(c, addr, maria)` returns the CPU half or the MARIA half (`half` offset) per owner; `$C000` HALT-RAM store redirect at cart.c:1188; bank register reduced modulo bank count. `src/bus.c` selects the owner from `maria_from`/`maria_until`. `tests/diag/expected.csv` holds 238 `bankset` rows (startup, upper, last, low, bank, all256, stored, redirect) across 14 profiles. Reran `diag.bankset`, `diag.bankset_tracer`, `diag.rebuild`: pass. |
| 2 | XBoard and the XM subset respond to documented control registers and banked CPU/MARIA accesses, including integrated HSC/POKEY use | VERIFIED | `cart_expansion_at`: XBoard latch at `$0470-$047F` (all 16 aliases), bank = control & 7, RAM select bit 3, POKEY select bit 4. XM: control `$0470`, separate `maria_pages`/`cpu_pages` nibbles at `$0478/$047C`, two independent 8 KiB windows, `cart_hsc_decoded` gates both HSC windows. `diag.expansion` (459 expansion/xm/hsc rows in expected.csv) and `core.hsc.*` pass on rebuilt ci tree. |
| 3 | Owned integration fixtures exercise the new board paths through library, runner and libretro with bounded hostile bank traffic | VERIFIED | Library: `diag.*`, `cart.*`, `core.hsc.*`. Runner: `tests/runner/tests.cmake` expansion fixtures (selector, info, save order, 52 KiB conflict, sound refusals). libretro: `tests/libretro/host.c` real-module expansion/BankSet cases, `libretro.rcheevos.expansion`, `libretro.host.hsc_firmware` pass. Hostile traffic: fuzz seeds (123 inputs, BankSet profiles) and `tests/prop/test_prop.c`. The orchestrator's `cmake --workflow --preset ci` at ac61aea (exit 0) and hosted CI run 40702203420 (success, head ae4e9b7) cover the full set. |
| 4 | Souper/BupChip, YM2151 and other unimplemented expansion audio remain explicitly unsupported; nothing incomplete is advertised | VERIFIED | `instance.c:359`: YM2151/Covox/ADPCM or conflicting POKEY owners return `ERR_UNSUPPORTED_DEVICE`. Runner test expects exit 77 with "unavailable sound configuration: <name>" for ym2151, covox, adpcm even with `--expansion off`. `docs/compatibility.md`, `README.md` and `libretro/muhtari_libretro.info.in` list the missing features and make no gameplay claim. |
| 5 | Plan 10-05: unsupported boards/images retain existing refusals (exit 77) in the runner | FAILED | Reproduced with `build/ci/runner/muhtari-run`: `--frames 1 g.a26` exits 77; `--frames 1 --hsc-rom fw.bin g.a26` exits 1 ("HSC firmware requires attached HSC"); same with `--hsc on`. See gap. |

**Score:** 4/5 truths verified (0 behavior-unverified)

Supporting check for "geometry validated before load changes state" (BOARD-04): `cart_fits` rejects odd/over-1 MiB sizes and the RAM matrix before allocation; `cart.load.no_alloc_on_reject` passes.

### Required Artifacts

| Artifact | Expected | Status | Details |
|---|---|---|---|
| `src/cart.c` | BankSet geometry, redirect, XBoard/XM windows | VERIFIED | Substantive, wired from bus read/write paths |
| `src/bus.c` / `src/maria.c` | CPU/MARIA owner tracking at HALT edges | VERIFIED | Owner interval consumed by cart peek |
| `src/instance.c` | Transactional load, expansion/HSC resolution, sound refusal | VERIFIED | Staging before release of live storage |
| `runner/main.c` | `--expansion`, `--hsc-rom`, exit mapping | PARTIAL | Works for normal use; CR-01 regression on refused image |
| `libretro/libretro.c` | reload-only `muhtari_expansion`, firmware discovery | VERIFIED | Exercised by the real-module host test |
| `tests/diag/bankset.a78`, `expansion.a78` | Owned fixtures | VERIFIED | Listed in `tests/roms/manifest.txt`; judged against `expected.csv` |

### Requirements Coverage

| Requirement | Source Plans | Description | Status | Evidence |
|---|---|---|---|---|
| BOARD-04 | 10-01, 02, 03, 05, 06, 07, 09, 10, 11, 12, 13 | BankSet CPU/MARIA ROM/RAM views at both HALT transitions; geometry validated before load changes state | SATISFIED | Truth 1 and the geometry check above |
| BOARD-05 | 10-01, 03, 04, 05, 06, 08, 09, 10, 11, 12, 13 | XBoard and XM RAM/POKEY/HSC subset with control registers and CPU/MARIA banking; unsupported sound explicit | SATISFIED for the library, libretro and normal runner use | Truths 2 to 4. The CR-01 gap is a runner exit-code regression and does not change the hardware behavior |

Both IDs appear in REQUIREMENTS.md (Phase 10, Pending) and in the plan frontmatter. No orphaned requirement. REQUIREMENTS.md still shows both as Pending, which is the orchestrator's to update after the gap closes.

### Anti-Patterns Found

| File | Line | Pattern | Severity | Impact |
|---|---|---|---|---|
| `runner/main.c` | 1182-1190 | Refused image reported as detached HSC, exit 1 instead of 77 | BLOCKER (gap) | Broken documented exit contract |

No TBD/FIXME/XXX markers were flagged in the files I checked, and no stubs in the BankSet or expansion code.

### Review Findings: Disposition by Verifier

| Finding | Verified myself | Judgment |
|---|---|---|
| CR-01 | Yes. Read runner/main.c:1182 and ran the runner: exit 1 for a 2600 image with `--hsc-rom`, 77 without. Git shows the guard came with `01a61d0` and the pre-change runner went to `muhtari_load`. | GAP. `docs/runner.md:43` and `runner/exit_status.h` document 77 for a 2600 or unsupported-board image. Plan 10-05 says "unsupported boards retain existing refusals." AGENTS rule 7 keeps violations of an existing required contract blocking. Fix is about 3 lines plus one test. |
| WR-01 | Yes. A POKEY-at-$0450 cart with `--port1 savekey --port2 savekey` exits 77 and prints "unavailable sound configuration: pokey_450" for a two-EEPROM controller refusal. | Follow-up quality item. The exit status is still the documented 77, and the sound refusals the plan requires are named correctly. The wrong name appears only in a combination the plan did not specify. Record as a seed. |
| WR-02 | Yes. A throwaway repo that deletes a listed `fixture.a78` and its manifest row still fails `--history` ("game or BIOS image by name"). `history_control.sh` uses `fixture.dat`, which `ROM_NAME` does not match. | Follow-up tooling item. It does not touch BOARD-04/05. The fix that was made does cover the binary-numstat path. The by-name path and a representative control remain, and rule 7 asks changed checks to exercise representative behavior. Record as a seed. |
| IN-01 to IN-04 | Not individually re-verified (info only) | Follow-up items, none affects a contract |

### Other Notes

- **Fix-round budget (AGENTS rule 7):** Plan 10-13 SUMMARY records that the budget was exceeded: the earlier session spent both review-fix rounds, and the resume used the gap round plus two follow-ups. Each follow-up was tied to a newly observed fault and no assertion, workload or inventory was weakened. I judged goal achievement on the code and evidence, not on this process breach. Because the single gap round for this phase is already spent, the owner should decide whether closing CR-01 needs an explicit exception.
- **Evidence consulted:** orchestrator `cmake --workflow --preset ci` at ac61aea (exit 0, local, N/A run URL); hosted CI https://github.com/szTheory/muhtari/actions/runs/40702203420 (attempt 1, success, head ae4e9b7). I also rebuilt `build/ci` (no work to do) and reran 63 related tests (`diag.*`, `core.hsc.*`, `cart.*`, `libretro.rcheevos.expansion`, `libretro.host.hsc_firmware`): 100% passed. The Nightly `prop` cancellation is advisory and recorded as SEED-010.
- **Human verification:** none. Per AGENTS rule 6 nothing here needs owner consent or owner hardware.

### Gaps Summary

One gap: the runner's `--hsc-rom` guard turns a refused image into exit 1 with a false message. Restrict the "requires attached HSC" rule to `probed == MUHTARI_STATUS_OK`, let `muhtari_load` return the refusal, and add a runner test for exit 77 with `--hsc-rom` plus a 2600 or unsupported-board image. All BankSet, XBoard/XM, HSC/POKEY and unsupported-sound behavior is otherwise present, wired, and covered by passing tests.

---

_Verified: 2026-10-28_
_Verifier: Claude (gsd-verifier)_
