---
phase: 13-portable-save-load-across-three-interfaces
plan: "02"
subsystem: core-state
tags: [c, sha256, crc32, state-identity, cmake]
requires:
  - phase: 13-portable-save-load-across-three-interfaces
    provides: private state payload visitor and producer field ledger from plan 13-01
provides:
  - Existing owned SHA-256 and CRC32 implementations are available through the private core archive.
  - Per-instance normalized effective-machine identity is cached after successful load and final power-on.
  - Lifecycle and topology cases verify the identity encoding, equality, differences, retention, and cache use.
affects: [phase-13-state-envelope, state-api, runner, libretro]
actuals:
  tokens: 8182.25
  tasks: 3
  commits: 3
plan_head_before: b228b5ab8768d1a37bff1c06568b4bf5a630c058
plan_head_after: 97854aa940bf63739ec5ba65038aa6c2e35b0020
commits: 3
tech-stack:
  added: []
  patterns:
    - Reuse owned runner digests through the private core archive; object consumers link the archive only as a fallback.
    - Bind immutable machine identity once at the successful content-install boundary and retain it through power cycles.
key-files:
  created:
    - .planning/phases/13-portable-save-load-across-three-interfaces/13-02-SUMMARY.md
    - .planning/phases/13-portable-save-load-across-three-interfaces/deferred-items.md
  modified:
    - CMakeLists.txt
    - tests/cart/tests.cmake
    - tests/fuzz/tests.cmake
    - tests/compat/tests.cmake
    - tests/libretro/tests.cmake
    - tests/runner/tests.cmake
    - src/instance.c
    - src/internal.h
    - src/state.c
    - tests/core/test_state.c
    - .planning/phases/13-portable-save-load-across-three-interfaces/13-FIELD-PROOF.md
key-decisions:
  - Bind identity only after cart, battery, BIOS, and final power-on choices are committed; a refused load cannot replace it.
  - Resolve load-time controller identity through muhtari__ports_device so serial ownership and cartridge fallback use existing semantics.
  - Keep immutable media, identity, and host addresses outside the private mutable payload.
patterns-established:
  - Domain-separated, tagged little-endian records make effective identity independent of paths and caller pointers.
  - State queries use the cached digest and do not rescan immutable media or allocate.
requirements-completed: []
coverage:
  - id: D1
    description: Existing owned SHA-256 and CRC32 sources link through the core archive, with available object-library consumers resolving the fallback.
    requirement: STATE-01 infrastructure slice only; phase requirement remains pending
    verification:
      - kind: integration
        ref: "cmake --preset ci -DMUHTARI_TESTS=OFF && cmake --build --preset ci; exit 0"
        status: pass
      - kind: integration
        ref: "ctest --test-dir /private/tmp/muhtari-state-13-02-harness/build -R '^(cart\\.probe\\.accepted|cart\\.identify\\.hash_override|fuzz\\.regress\\.loader|runner\\.hash\\.repeat|runner\\.probe\\.images|runner\\.load\\.hash|libretro\\.host\\.layout|libretro\\.host\\.xrgb8888)$' --no-tests=error --output-on-failure; exit 0, 9/9 including fixture setup"
        status: pass
    human_judgment: false
  - id: D2
    description: A fixed 22-record identity captures normalized installed media and configuration, survives refused replacement and power cycle, and is reused without allocation.
    requirement: STATE-01, STATE-02, STATE-03 identity foundation only; phase requirements remain pending
    verification:
      - kind: unit
        ref: "ctest --test-dir /private/tmp/muhtari-state-13-01-harness/build -R '^core\\.state\\.(lifecycle|topologies)$' --no-tests=error --output-on-failure; exit 0, 2/2"
        status: pass
      - kind: unit
        ref: "cc strict-warning compile of tests/core/test_state.c, then lifecycle and topologies cases; exit 0"
        status: pass
    human_judgment: false
  - id: D3
    description: The field ledger records identity inputs, exclusions, cache lifetime, maximum ROM hashing work, and the remaining public portability boundary.
    verification:
      - kind: other
        ref: .planning/phases/13-portable-save-load-across-three-interfaces/13-FIELD-PROOF.md
        status: pass
    human_judgment: false
duration: 21m
completed: 2026-11-01
status: complete
---

# Phase 13 Plan 02: Shared Digests and Effective Identity Summary

**Owned SHA-256 and CRC32 algorithms now link through the private core archive, and successful loads cache a normalized, domain-separated machine identity.**

## Performance

- **Duration:** 21 minutes
- **Started:** 2026-11-01T20:17:17Z
- **Completed:** 2026-11-01T20:37:52Z
- **Tasks:** 3
- **Files modified or created:** 13 plan artifacts, including this summary and the deferred-items note.

## Accomplishments

- Added the existing `runner/sha256.c` and `runner/crc32.c` to the private `muhtari` archive and wired archive fallbacks for the existing cart, fuzz, compatibility, libretro, and runner object-library consumers. No new public target or digest implementation was added.
- Added a private 32-byte identity cache. It hashes the NUL-terminated `MUHTARI-STATE-IDENTITY` domain and 22 tagged, length-prefixed records covering normalized ROM bytes, resolved board/audio/IRQ/expansion/ports, battery topology, BIOS/HSC bytes, profile, and synth wiring.
- Bound identity only after a successful cart/battery/BIOS commit and final power cycle. Refused initial and replacement loads retain the prior state; public power cycling keeps the same identity.
- Extended state lifecycle/topology cases with a golden digest vector, raw/headered and Auto/explicit equivalence, differing effective choices, failed-load retention, caller-buffer mutation, and the maximum 1 MiB ROM case. Updated the field proof with identity lifetime and exclusions.

## Verification Receipts

| Command | Result |
|---|---|
| `cmake --preset ci -DMUHTARI_TESTS=OFF && cmake --build --preset ci` | Exit 0; the library, runner, and libretro products built. |
| `cmake --build /private/tmp/muhtari-state-13-02-harness/build` | Exit 0; available cart, fuzz, compatibility, runner, libretro, and diagnostic consumers built against the archive. |
| `ctest --test-dir /private/tmp/muhtari-state-13-02-harness/build -R '^(cart\.probe\.accepted|cart\.identify\.hash_override|fuzz\.regress\.loader|runner\.hash\.repeat|runner\.probe\.images|runner\.load\.hash|libretro\.host\.layout|libretro\.host\.xrgb8888)$' --no-tests=error --output-on-failure` | Exit 0; 9/9 tests passed, including fixture setup. |
| `cmake --build /private/tmp/muhtari-state-13-01-harness/build` | Exit 0; the state test executable compiled against the rebuilt archive. |
| `ctest --test-dir /private/tmp/muhtari-state-13-01-harness/build -R '^core\.state\.(lifecycle|topologies)$' --no-tests=error --output-on-failure` | Exit 0; 2/2 tests passed after commit. |
| `cc -std=c17 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wstrict-prototypes -Wmissing-prototypes -Wvla -Wundef -Wcast-qual -Wimplicit-fallthrough -Wmissing-variable-declarations -Iinclude -Isrc tests/core/test_state.c tests/cart/images.c build/ci/libmuhtari.a -o /private/tmp/muhtari-state-13-02-strict-state-test && /private/tmp/muhtari-state-13-02-strict-state-test lifecycle && /private/tmp/muhtari-state-13-02-strict-state-test topologies` | Exit 0; the modified test source passed the project warning set and both direct cases. |
| `node "$HOME/.codex/gsd-core/bin/gsd-tools.cjs" uat classify-coverage --summary .planning/phases/13-portable-save-load-across-three-interfaces/13-02-SUMMARY.md` | Exit 0; all 3 local deliverables are automatically covered, with no coverage schema errors. |
| `git diff --check` | Exit 0. |

All receipts are local; there are no hosted run URLs. The full `cmake --preset ci` verification was attempted once during Task 1 and exited 1 before test registration because pinned `wasmtime`, `7800basic`, and OpenBIOS inputs are absent. Per the execution constraint, no pins or ROM/BIOS files were fetched or generated, and the full CI command was not retried.

## Task Commits

1. **Task 1: Make the existing digest algorithms reachable from the private library** — `299ec73` (`chore`).
2. **Task 2: Preserve all remaining host/test object consumers** — `41ea6d1` (`chore`).
3. **Task 3: Cache the exact normalized effective-machine identity** — `97854aa` (`feat`).

The plan metadata commit is separate from the three measured task commits.

## Files Created and Modified

- `CMakeLists.txt` — assigns the existing owned digest sources to the private archive and exposes their headers only to the state source.
- `tests/cart/tests.cmake`, `tests/fuzz/tests.cmake`, `tests/compat/tests.cmake`, `tests/libretro/tests.cmake`, and `tests/runner/tests.cmake` — add private archive fallbacks for object-library consumers.
- `src/instance.c`, `src/internal.h`, and `src/state.c` — cache and bind the effective machine identity after successful load.
- `tests/core/test_state.c` — checks exact encoding and normalized identity behavior.
- `13-FIELD-PROOF.md` — records identity inputs, exclusions, lifetime, and maximum ROM work.
- `deferred-items.md` — records the unavailable rcheevos target and the preexisting Windows-ledger count mismatch.

## Decisions Made

- Use the installed cart ROM after normalization, not the original A78 image or caller-owned buffer.
- Use resolved machine values and actual BIOS/HSC bytes so equivalent effective media share an identity while changed attachments differ.
- Keep identity out of the payload; future state validation can compare the cache without serializing immutable media or host addresses.

## Deviations from Plan

Task 3 was marked `tdd="true"`, but its regression cases and implementation were committed together. The cases pass, but there is no separate RED commit or RED evidence record. This execution-cadence deviation is documented in `deferred-items.md`.

## Issues Encountered

- `muhtari-test-rcheevos` could not be built because its pinned `build/pins/rcheevos` source is absent. Other available libretro testdb and diagnostic consumers compiled and passed their selected host checks. No pin was fetched.
- A best-effort `gsd_run windows append` for the incomplete full-preset verification was rejected because the existing `.planning/WINDOWS.md` header reports 17 open / 35 fixed while its rows count 16 open / 36 fixed. The existing ledger was left unchanged and the mismatch was recorded in `deferred-items.md`.
- No authentication gates occurred. No code stubs were introduced. No new endpoint, file-access path, or trust boundary was added.

## Next Phase Readiness

Plan 13-03 can consume the cached identity and private digest ownership. This plan does not complete STATE-01 through STATE-04: the public envelope/API, runner and libretro state paths, complete native/big-endian equality, and hostile restore proof remain later work. STATE-02 remains unclassified, and the Phase 12 halt is unchanged. No owner UAT is needed for this local implementation.

---
*Phase: 13-portable-save-load-across-three-interfaces*
*Completed: 2026-11-01*

## Self-Check: PASSED

- All listed plan source files, this summary, and `deferred-items.md` exist.
- Task commits `299ec73`, `41ea6d1`, and `97854aa` are ancestors of the current plan branch head.
- `git diff --check` passed, and the task commits contain no tracked-file deletions.
