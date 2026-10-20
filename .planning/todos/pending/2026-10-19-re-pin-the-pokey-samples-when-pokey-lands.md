---
created: 2026-10-20T07:30:00.000Z
title: Re-pin the POKEY samples when POKEY lands
area: core
severity: minor
files:
  - tests/built/expected.txt
  - tests/compat/ledger.csv
  - docs/compatibility.md
  - src/instance.c
---

## Problem

Five 7800basic 0.41 samples put a POKEY at $0450: eightysix, pokey, rmtramcompressiondemo, rmtramdemo and
rmtromdemo. Phase 3 has no POKEY, so they read the open-bus byte there, and their frame chains in
`tests/built/expected.txt` and four of their ledger rows' `title@120` hashes were pinned on that byte. When
POKEY lands in phase 4, what those reads return changes, and so do their frames and titles.

By then these pins are released, because they ship in 0.3.0. D-15 says only a change to an unreleased pin may be reset
to `-` and rewritten at the same behaviour revision. A change to a released pin must bump the behaviour revision.

## Solution

In the phase 4 plan that maps POKEY at $0450:

- bump the behaviour revision (`MUHTARI__BEHAVIOUR_REVISION` in `src/instance.c`) in the same commit as the
  change. Then rewrite the frame chains with `-DWRITE=ON`, which resets every other row's frame pin to `-` for its
  own rewrite (docs/emulation-tests.md, Writing a pin);
- re-pin the four `title@120` rows (pokey and the three rmt demos; eightysix's row is `boots@120`, seeded from INTIM)
  with `tests/compat/pin.cmake`, let `library.ratchet` show the changed evidence, and
  regenerate `docs/compatibility.md` with `tools/compat_doc.cmake`;
- record each row, the cause, and the old and new hashes in that plan's SUMMARY. A pin that moves in any sample
  without POKEY is a bug to fix, not a re-pin.
