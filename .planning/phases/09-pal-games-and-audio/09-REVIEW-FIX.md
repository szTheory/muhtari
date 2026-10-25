---
phase: 09-pal-games-and-audio
fixed_at: 2026-10-25T20:59:26Z
review_path: .planning/phases/09-pal-games-and-audio/09-REVIEW.md
iteration: 1
findings_in_scope: 1
fixed: 1
skipped: 0
status: all_fixed
---

# Phase 9: Code Review Fix Report

**Fixed at:** 2026-10-25T20:59:26Z  
**Source review:** `.planning/phases/09-pal-games-and-audio/09-REVIEW.md` at `c8e57aa`  
**Iteration:** 1 of at most 2; scope `critical_warning`

**Summary:**

- Findings in scope: 1
- Fixed: 1
- Skipped: 0

## Fixed Issues

### CR-01: BLOCKER — Load invalidates an aliased BIOS before consuming it

**Status:** fixed  
**Files modified:** `src/instance.c`, `src/internal.h`, `tests/boot/test_boot.c`,
`tests/boot/tests.cmake`, `tests/inventory/ci.txt`, `tests/inventory/wasm32.txt`,
`docs/embedding.md`, `docs/emulation-tests.md`  
**Commit:** `af9dc83f9ff68ea14c5a007218a3477a3c2f2218`  
**Applied fix:** Copy validated BIOS bytes into unpublished instance staging
before cartridge installation can release RAM or clear battery/HSC storage.
Install the staged BIOS only after the final fallible allocation succeeds.
Rejected candidates and allocation failure retain the running BIOS and all
published live state. No public declaration, region constant, calibration,
frame algorithm or synthesis algorithm changed.

### Regression coverage

`bios.load_aliases` uses the existing allocator hooks/counting helper with a
local free-poison/quarantine wrapper. Freed cartridge storage remains filled
with `$A5` until the next free, exposing the stale read deterministically
without depending on allocator reuse or claiming a sanitizer run.

Ten combinations cover both original regions and successful reload into the
opposite region: 4/16 KiB firmware borrowed from public cartridge RAM and
EEPROM spans, plus 4 KiB from the public read-only HSC ROM span. A 16 KiB HSC
case is impossible because that public span is exactly 4 KiB. The HSC bytes
are supplied at the original load, never written through its const observer.

Each combination checks rejected BIOS size, rejected HSC media, failed
allocation, byte-identical live instance state excluding unpublished firmware
staging, unchanged borrowed bytes and no premature free. The old distinct
BIOS executes its marker after power cycle following failure. Successful
reload checks every copied byte, reset PC, region, allocation counts, and the
new firmware marker both immediately and after another power cycle. Tests
are registered separately and included in both exact portable inventories.

### Ownership, lifetime and cost

Following ENGINEERING section 11, `instance.c` continues to own the entire
validate/stage/install transaction. Reuse the existing unpublished HSC staging
pattern with a second fixed array bounded by the existing accepted maximum
BIOS size. The 4 KiB HSC array cannot hold a 16 KiB BIOS, and simultaneous HSC
and BIOS candidates must remain independent. Reusing live hardware storage
would break failed-load preservation. The alternative of splitting cartridge
allocation from its commit would widen the cartridge-owner interface and
change more call paths to avoid this fixed storage cost.

The selected change adds no dependency, allocation or large stack frame.
Staging is private for the instance lifetime, consumed only during load and
never published as hardware/save memory. At most 16 KiB is copied an extra
time per BIOS load; there is no additional per-frame work. The same native C17
`sizeof(muhtari)` probe measured 72,688 bytes before and 89,072 after: exactly
16,384 additional bytes per instance. The existing one ROM/RAM allocation
per successful load remains tested. This is a reliability storage choice,
not a new unrelated performance or resource threshold.

## Verification

All commands ran in the **main checkout** on `phase/09-pal-games-and-audio`:
`workflow.use_worktrees=false` explicitly disables isolation. There was no
worktree or cleanup sentinel. Tier 1 reread confirmed the modified load
sections and test context; Tier 2 used the actual C17 fatal-warning build.

| Command | Exit | Result |
|---|---:|---|
| `cmake --build --preset ci --target muhtari-test-boot --parallel 4` | 0 | Focused fatal-warning C build |
| `build/ci/tests/muhtari-test-boot load_aliases` before the implementation fix, after fixture correction | 1 | RED: cartridge RAM reset `$A5A5`, EEPROM `$FFFF`, HSC `$0000`; copied bytes and executed markers fail |
| Same focused build and `build/ci/tests/muhtari-test-boot load_aliases` after the fix | 0 | GREEN: ten combinations, failure preservation and real boot execution |
| `ctest --test-dir build/ci -R '^bios.load_aliases$' --output-on-failure` | 0 | Named CTest case passes, 0.02 seconds |
| `ctest --test-dir build/ci -R '^docs.budgets$' --output-on-failure` after final prose reduction | 0 | Test documentation is 40,946 bytes, below 40,960 |
| `clang-format` 21 `--dry-run --Werror src/instance.c src/internal.h tests/boot/test_boot.c` | 0 | Formatting passes |
| `git diff --check` | 0 | No whitespace errors |
| `scripts/hygiene.sh --tree` | 0 | Tree hygiene passes |
| `sh build/ci/task08-jobserver-gate.sh`, final attempt | 0 | Exact configured workflow, packaging, release checks, JUnit and tree hygiene all pass |
| `gsd_run query commit "fix(09): CR-01 preserve aliased BIOS during load" --files` with the eight paths above | 0 | One atomic fix commit; existing hooks and noreply identity active |

The final helper uses the already validated standard FIFO jobserver with
three explicit tokens and one implicit slot. Its unchanged
`cmake --workflow --preset ci` passed all **654/654** tests, generated the
three component ZIPs and passed **6/6** release checks. Its explicit package
target and repeated release check also passed **6/6**; JUnit confirmed
**654 tests ran once each**. The test suite took 52.42 seconds. Final focused
regression, formatting and diff checks ran again before the atomic commit,
with exit 0 and no intervening source changes.

Run URL: none for local commands. Hosted acceptance on this new commit is
pending the orchestrator's source-fresh CI refresh; no previous hosted run
is presented as acceptance of this fix. No logs are committed.

### Failed verification history retained

- The initial focused build exited 1 because the new test used
  `muhtari_memory` instead of the existing `muhtari_span`; corrected before
  the meaningful RED build/run.
- The first test run exited 1 with both the alias defects and an invalid
  synthetic startup sequence: writing `$2000` before enabling the 7800 map
  accessed its 2600 decode. Corrected the owned firmware to enable MEN while
  keeping BIOS mapped, then reran RED successfully against the unchanged
  old loader; the prior-firmware execution assertions then passed.
- The initial size-probe compile failed (compiler exit 1) because shell
  `printf` interpreted the C `%zu`. Replaced it with `printf '%s\n'` and
  literal source arguments; both before/after probe compiles and runs exited 0.
- The first format dry run exited 1 for two whitespace differences in the
  new test. Applied the pinned formatter; the final dry run exited 0.
- First `sh build/ci/task08-jobserver-gate.sh`: exit 8, **653/654** tests
  passed; only `docs.budgets` failed at **41,187 > 40,960** bytes. Packaging
  and release steps did not run in that attempt.
- The first prose reduction still left **40,971 > 40,960** bytes; its
  focused `docs.budgets` test exited 8. The second helper attempt was started
  before that residual failure was noticed and also exited 8 with
  **653/654** passed, solely `docs.budgets`. Packaging and release steps
  again did not run. This was not accepted as verification.
- Reduced only the added description to **40,946** bytes. Focused budget,
  formatting, diff and hygiene checks passed before starting the final helper.
  The final exact helper exited 0 as recorded above.

---

_Fixed: 2026-10-25T20:59:26Z_  
_Fixer: the agent (gsd-code-fixer)_  
_Iteration: 1_
