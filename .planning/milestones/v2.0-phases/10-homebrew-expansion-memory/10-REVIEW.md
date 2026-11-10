---
phase: 10-homebrew-expansion-memory
reviewed: 2026-10-28T00:00:00Z
depth: standard
files_reviewed: 5
files_reviewed_list:
  - docs/runner.md
  - runner/main.c
  - tests/inventory/ci.txt
  - tests/inventory/wasm32.txt
  - tests/runner/tests.cmake
findings:
  critical: 0
  warning: 1
  info: 2
  total: 3
status: issues_found
---

# Phase 10: Code Review Report (gap-closure re-review)

**Reviewed:** 2026-10-28
**Depth:** standard
**Files Reviewed:** 5
**Status:** issues_found

## Summary

This is an incremental re-review of the changes since 8b76704 (commits e8a35a5 and 2c3a66f).
I read `git diff 8b76704 HEAD` for the five files, read the full current `load()` in
`runner/main.c`, and traced it against `muhtari_load` and `battery_prepare` in `src/instance.c`.
I also ran the new test and probed edge cases by hand with the built runner.

**CR-01 is resolved.** The runner no longer pre-checks attachment from its own probe. With
`--hsc-rom` it asks the core to load the image first, with no firmware. The core returns the real
status, and the existing status-to-exit mapping then gives exit 77 with the usual message for a
2600 image, an unsupported board, declared unsupported sound and a 52 KiB BankSet that would
attach HSC. In every refused case the firmware file is never opened. For an accepted image
the runner reads `get_battery().hsc_attached` from the first load. It exits 1 with the
attachment message if HSC is detached, and otherwise reads and validates the firmware and loads again
with it. The second load keeps the first load's attachment: `battery_prepare` attaches whenever
firmware is supplied, and `docs/embedding.md` documents "a supplied ROM implies attachment".
Firmware is only supplied after the first load reported HSC attached, so no divergence is possible.
Error paths free `image`, `bios` and `firmware` correctly, and a failed first load skips the
second.

Verification run:
- `ctest -R 'runner\.(hsc_rom|region\.pal|probe)'` in `build/ci` (up to date with HEAD): all pass,
  `runner.hsc_rom.refused` in 0.12 s.
- By hand: `--hsc off --hsc-rom` and default `--hsc-rom` on the 52 KiB BankSet exit 1 with
  "HSC firmware requires attached HSC", as the new `docs/runner.md` text says. `--hsc on --hsc-rom` on an
  XM image exits 0.
- Reasoning on the old code: each of the 24 new refusal assertions would have exited 1 before
  the fix. The 2600 and unsupported-board images failed the probe check. The sound images and the
  `--hsc off` rows failed on attachment. The `--hsc on` rows failed on firmware read. The test
  therefore names and exercises the fault, and the anchored regexes reject a stray HSC or firmware message.
- The pre-existing exit-1 rows for XM images (`--hsc off`, `--expansion off`, bad-size firmware)
  still hold with the new flow.

I found no new correctness or security defect. The inventories (`ci.txt`, `wasm32.txt`) list the new test
in the same sorted position in both files. One documented behaviour has no test, and two
maintainability notes follow.

## Warnings

### WR-03: Documented "accepted, so exit 1" branch for a 52 KiB BankSet is untested

**File:** `tests/runner/tests.cmake:1218-1229`, `docs/runner.md:53`
**Issue:** The rewritten `--hsc-rom` row states a second contract: with `--hsc off`, or with
`--hsc auto` and no XM or header HSC request, a 52 KiB BankSet set "is accepted and `--hsc-rom` exits 1".
The new `hsc_rom.refused` case covers only the refused side (XM, `--hsc on`). No test runs
`ram52.a78` with `--hsc-rom` under `--hsc off` or default `--hsc auto`, so the other half of
the documented contract, and the case where the first load succeeds on a BankSet and the
runner takes the "requires attached HSC" path, can regress unnoticed. I confirmed by hand
that the behaviour is currently correct (exit 1, attachment message), so this is a coverage gap.
Project rule 5 asks that behaviour documented as a change lands with a test.
**Fix:** Add two rows to the `ram52.a78` block of `hsc_rom.refused`, or to the `expansion` case, which already uses `ram52.a78`:
```cmake
expect_exit(1 "HSC firmware requires attached HSC" --frames 1 --hsc off
  --hsc-rom "${WORK}/hsc.bin" "${WORK}/ram52.a78")
expect_exit(1 "HSC firmware requires attached HSC" --frames 1
  --hsc-rom "${WORK}/hsc.bin" "${WORK}/ram52.a78")
```

## Info

### IN-05: `load()` grew another nesting level and duplicates its cleanup

**File:** `runner/main.c:1193-1236`
**Issue:** The firmware read now sits inside `if (opt->hsc_rom) { if (st == OK) { ... if (!attached) ...` and reaches
five levels at the `fread` loop. The `free(image); free(bios); return MUHTARI__EXIT_FAILURE;` triplet appears three
times inside this block, and `load()` is well over 100 lines. This exceeds the project's "plain control flow" style. A mismatch
between the cleanup copies is easy to introduce. Separately, a bad BIOS image now fails at the first
load, before the firmware file is read. Its message therefore takes precedence over a firmware error. Both exit 1 and
this is arguably better, but `docs/runner.md` does not say so.
**Fix:** Extract the firmware read into a helper, for example
`static uint8_t *read_hsc_firmware(const char *path)`, returning NULL after printing the
"must be a readable 4096-byte file" message. Then `load()` keeps one `goto fail`-style cleanup (or a single
`free` sequence) for the three exits.

### IN-06: The `--hsc-rom` table cell is a single 900-character paragraph

**File:** `docs/runner.md:53`
**Issue:** The row packs five outcomes (refused image, BankSet, `--hsc auto` attachment rule, `--hsc off`, size and
readability) into one table cell with nested "only when ... and ... so" clauses, which is hard to read. It duplicates
the attachment rule already given for `--hsc` in the row above and in the XM paragraph below. Rule 5 asks for a short, user-facing README
and docs. The cell also says "A 52 KiB BankSet set", which reads as a typo for "BankSet image".
**Fix:** Keep the cell to the contract: "Exact 4,096-byte HSC firmware, copied at load. An image the core refuses keeps
exit 77 and its firmware file is not read; anything else that stops the request (HSC not attached, unreadable file,
wrong size) exits 1." Move the BankSet and attachment detail to one short paragraph under the table, next to the
XM paragraph that already covers firmware reads.

---

_Reviewed: 2026-10-28_
_Reviewer: Claude (gsd-code-reviewer)_
_Depth: standard_
