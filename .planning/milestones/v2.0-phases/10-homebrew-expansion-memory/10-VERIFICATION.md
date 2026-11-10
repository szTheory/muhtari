---
phase: 10-homebrew-expansion-memory
verified: 2026-10-28T20:00:00Z
status: passed
score: 5/5 must-haves verified
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
  - ".planning/phases/10-homebrew-expansion-memory/10-14-PLAN.md"
  - ".planning/phases/10-homebrew-expansion-memory/10-14-SUMMARY.md"
  - "docs/runner.md"
  - "runner/main.c"
  - "src/bus.c"
  - "src/cart.c"
  - "src/instance.c"
  - "src/maria.c"
  - "tests/inventory/ci.txt"
  - "tests/inventory/wasm32.txt"
  - "tests/runner/tests.cmake"
covered_digest: "v4:sha256:4d4fe4740e7991a169e6a53966b882513fb7fcadf89320c899e09af6bd757e58"
behavior_unverified: 0
overrides_applied: 0
re_verification:
  previous_status: gaps_found
  previous_score: 4/5
  gaps_closed:
    - "Unsupported boards retain existing refusals (plan 10-05): a refused image exits 77 with its refusal message, also under --hsc-rom (CR-01)"
  gaps_remaining: []
  regressions: []
---

# Phase 10: Homebrew and expansion memory Verification Report

**Phase Goal:** Run BankSet and XBoard/XM memory/POKEY/HSC capabilities using the correct CPU/MARIA ownership.
**Verified:** 2026-10-28
**Status:** passed
**Re-verification:** Yes, after gap closure (plan 10-14, commits e8a35a5, 2c3a66f, 8407ec2)

The one gap from the prior report (CR-01, `--hsc-rom` with a refused image exiting 1 instead of 77) is closed in the code and covered by a registered test. No earlier must-have regressed. The re-review warning WR-03 and infos IN-05/IN-06 are follow-up items, not contract violations.

## Goal Achievement

### Observable Truths

| # | Truth | Status | Evidence |
|---|---|---|---|
| 1 | BankSet diagnostics exercise separate CPU/MARIA ROM and RAM views on both HALT edges per supported geometry | VERIFIED (regression check) | `src/cart.c`, `src/bus.c`, `src/maria.c` are unchanged since ad615b7 (`git diff --stat ad615b7 HEAD -- src include runner/exit_status.h` is empty). Reran `ctest --preset ci -R '^(diag[.](bankset|bankset_tracer|rebuild)|cart[.].*|libretro[.](rcheevos[.]expansion|host[.]hsc_firmware))$'`: all passed. |
| 2 | XBoard and XM respond to documented control registers and banked CPU/MARIA accesses, including HSC/POKEY | VERIFIED (regression check) | Same unchanged `src/`. `diag.expansion` (real runner runs with `--hsc-rom` compared to library reference hashes) passed in 6.63 s after the runner restructure, so the double load gives the same hashes as a single load. |
| 3 | Owned fixtures exercise the board paths through library, runner and libretro with bounded hostile traffic | VERIFIED | Library `diag.*`/`cart.*`/`core.hsc.*`, runner `runner.region.pal` and the new `runner.hsc_rom.refused`, libretro `libretro.rcheevos.expansion` and `libretro.host.hsc_firmware` all pass locally. |
| 4 | Souper/BupChip, YM2151 and other unimplemented expansion audio remain explicitly unsupported | VERIFIED | Runner run directly: `--frames 1 --hsc on --hsc-rom missing.bin ym2151.a78` prints the two canonical lines and exits 77; `souper-v4` exits 77 "unsupported board: souper". Now also holds when `--hsc-rom` is present. |
| 5 | Plan 10-05: unsupported boards/images retain existing refusals (exit 77), including with `--hsc-rom` | VERIFIED (was FAILED) | See "Gap closure evidence" below. |

**Score:** 5/5 truths verified (0 behavior-unverified)

### Gap closure evidence (CR-01)

Code (`runner/main.c` load(), lines about 1182 to 1236). The runner-side attachment copy and the `probed` variable are gone (`grep -c probed runner/main.c` and `grep -c MUHTARI_DEVICE_HSC runner/main.c` should both be 0; the old guard text `!attached || probed != MUHTARI_STATUS_OK` no longer exists in the file). With `--hsc-rom`:

1. `st = muhtari_load(m, &media)` runs first with `media.hsc_rom` NULL.
2. Only when `st == OK` does it read `muhtari_get_battery().hsc_attached`; detached gives the existing "HSC firmware requires attached HSC", exit 1.
3. Only then is the firmware read (size check, "must be a readable 4096-byte file", exit 1) and `muhtari_load` called a second time with firmware.
4. A non-OK first status skips the firmware read and falls to the shared failure tail, which prints the canonical lines and maps 2600/BOARD/REGION/DEVICE to `MUHTARI__EXIT_REFUSED` (77).

The runner's attachment rule is the library's (`muhtari_load` plus `muhtari_get_battery`), not a second copy.

Behavior reproduced with `build/ci/runner/muhtari-run` (built tree up to date, `ninja: no work to do`):

| Command (all `--frames 1`) | Exit | Output |
|---|---|---|
| `--hsc on --hsc-rom missing.bin ym2151.a78` | 77 | "unsupported device", "unavailable sound configuration: ym2151" |
| `--hsc-rom hsc.bin 2600-4k` | 77 | "unsupported: 2600 (planned)" (was 1) |
| `--hsc-rom hsc.bin souper-v4` | 77 | "unsupported board: souper" |
| `--expansion xm --hsc-rom missing.bin ram52.a78` | 77 | "unsupported board: bankset" (firmware file never read) |
| `--hsc-rom hsc.bin xm1.a78` | 0 | hsc_sha1 line and unknown-firmware warnings |
| `--hsc off --hsc-rom hsc.bin xm1.a78` | 1 | "HSC firmware requires attached HSC" |
| `--hsc-rom bad-hsc.bin xm1.a78` | 1 | "HSC firmware must be a readable 4096-byte file" |
| `--expansion off --hsc-rom hsc.bin xm1.a78` | 1 | "HSC firmware requires attached HSC" |
| `--expansion xm --hsc off ram52.a78` | 0 | accepted, no output |
| `--expansion xm --hsc off --hsc-rom hsc.bin ram52.a78` | 1 | "HSC firmware requires attached HSC" |

The last two rows match the documented condition in `docs/runner.md` line 53 (52 KiB BankSet refused only when HSC attaches).

Tests: `tests/runner/tests.cmake` has the `hsc_rom.refused` case (line 1192) and registration `runner.hsc_rom.refused` (line 1353) with a "Fault prevented" comment, fixtures written into its own WORK directory, anchored whole-output regexes (reject any HSC or firmware message beside the refusal), and `expect_exit(77 ...)` over 2600, Souper, YM2151/Covox/ADPCM and the 52 KiB BankSet, each with missing, wrong-size and valid firmware. Inventory rows exist in `tests/inventory/ci.txt` and `tests/inventory/wasm32.txt`. Fault detection: the SUMMARY records the red run against the unfixed runner (ctest exit 8, "got exit 1" with the HSC message), consistent with the prior verification's independent reproduction of the fault. Current run:

`ctest --preset ci -R '^(runner[.](hsc_rom[.]refused|region[.]pal|exit[.]table)|diag[.]expansion|core[.]hsc.*)$' --no-tests=error` exit 0, 10/10 passed (hsc_rom.refused 0.09 s, region.pal 0.13 s, diag.expansion 6.63 s).

Orchestrator evidence: `cmake --workflow --preset ci` at HEAD 2b4f4fb, exit 0, 662/662 and 6/6 tests (local, run URL N/A).

Docs: `docs/runner.md` line 53 states the 77 contract without a firmware read, the conditional BankSet refusal using the library's attachment rule, and the exit 1 cases; it agrees with the behavior in the table above. `runner/exit_status.h`, `include/` and `src/` are unchanged (empty diff against ad615b7), so no public API, header or core change.

### Required Artifacts

| Artifact | Expected | Status | Details |
|---|---|---|---|
| `runner/main.c` | Library-decided refusal and attachment before firmware read | VERIFIED | Substantive, wired to `muhtari_load`/`muhtari_get_battery` and the existing exit mapping |
| `tests/runner/tests.cmake` | `runner.hsc_rom.refused`, 24 anchored checks | VERIFIED | Registered, passes, inventory rows present |
| `tests/inventory/ci.txt`, `wasm32.txt` | One row each | VERIFIED | `grep -c 'hsc_rom[.]refused'` prints 1 in each |
| `docs/runner.md` | Updated `--hsc-rom` row | VERIFIED | Matches observed behavior |
| `src/cart.c`, `src/bus.c`, `src/maria.c`, `src/instance.c`, `libretro/libretro.c` | Phase 10 board behavior | VERIFIED | Unchanged since the prior verification; tests rerun green |

### Key Link Verification

| From | To | Via | Status |
|---|---|---|---|
| `runner/main.c` | `muhtari_load` | firmware-free load first, refusal reaches existing print and 77 mapping | WIRED (exercised above) |
| `runner/main.c` | `muhtari_get_battery` | `hsc_attached` gates the firmware read | WIRED (exit 1 rows above) |
| `tests/runner/tests.cmake` | `muhtari-run` | `expect_exit(77 ...)` against the real process | WIRED (test passes) |

### Requirements Coverage

| Requirement | Source Plans | Description | Status | Evidence |
|---|---|---|---|---|
| BOARD-04 | 10-01 to 10-03, 05 to 07, 09 to 14 | BankSet CPU/MARIA ROM/RAM views at both HALT transitions; geometry validated before load | SATISFIED | Truth 1; 52 KiB geometry refusal now also holds under `--hsc-rom` |
| BOARD-05 | 10-01, 03, 04, 05, 06, 08 to 14 | XBoard and XM RAM/POKEY/HSC subset with control registers and CPU/MARIA banking; unsupported sound explicit | SATISFIED | Truths 2 to 4 and the gap closure |

Both IDs are in REQUIREMENTS.md (lines 21-22, traceability lines 89-90, marked Complete) and in plan frontmatter, including 10-14. No orphaned requirement.

### Anti-Patterns Found

None blocking. No `TBD`/`FIXME`/`XXX` in `runner/main.c`. The previously blocking pattern (refused image reported as detached HSC) is gone.

### Review Findings: Disposition by Verifier

| Finding | Judgment |
|---|---|
| CR-01 | Closed, verified above. Disposition `fixed` is correct. |
| WR-03 | Coverage gap: the documented "accepted, so exit 1" branch for a 52 KiB BankSet with HSC detached has no registered test. I ran it by hand (rows above: exit 0 without `--hsc-rom`, exit 1 with) and it matches the docs. No contract is violated; behavior is correct and documented. Follow-up seed. |
| IN-05, IN-06 | Style/readability (nesting, long table cell). Follow-up. |
| WR-01, WR-02, IN-01 to IN-04 | Unchanged from the prior verification: follow-up seeds, none touches BOARD-04/05 contracts. |

### Other Notes

- **Process:** plan 10-14 records an owner exception (2026-10-28, one-time) for a second gap round; no further round was spent.
- **Deadline:** `runner.hsc_rom.refused` TIMEOUT 2 is marked provisional until a hosted run exists (per the plan). Locally it takes 0.09 s.
- **Human verification:** none. Everything was checked by running commands; per project rule 6 nothing needs owner consent or owner hardware.

### Gaps Summary

No gaps. Phase 10's goal (BankSet and XBoard/XM memory/POKEY/HSC with correct CPU/MARIA ownership, explicit unsupported sound) is achieved, and the runner's documented exit 77 refusal contract holds with `--hsc-rom`.

---

_Verified: 2026-10-28_
_Verifier: Claude (gsd-verifier)_
