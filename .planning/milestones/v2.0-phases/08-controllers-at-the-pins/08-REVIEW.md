---
phase: 08-controllers-at-the-pins
reviewed: 2026-10-25T03:15:34Z
depth: standard
files_reviewed: 72
files_reviewed_list:
  - .github/workflows/ci.yml
  - AGENTS.md
  - README.md
  - docs/embedding.md
  - docs/emulation-tests.md
  - docs/hardware/bus.md
  - docs/hardware/controllers.md
  - docs/hardware/maria.md
  - docs/hardware/riot.md
  - docs/hardware/tia.md
  - docs/libretro.md
  - docs/maintaining.md
  - docs/quality.md
  - docs/runner.md
  - docs/testing.md
  - include/muhtari.h
  - libretro/libretro.c
  - quality.json
  - runner/main.c
  - runner/movie.c
  - runner/movie.h
  - src/bus.c
  - src/frame.c
  - src/instance.c
  - src/internal.h
  - src/maria.c
  - src/ports.c
  - src/riot.c
  - src/tia.c
  - tests/abi/tests.cmake
  - tests/api/v6-layout.c
  - tests/api/v6.c
  - tests/audio/expected.txt
  - tests/built/expected.txt
  - tests/cmake/hashes.cmake
  - tests/cmake/quality.cmake
  - tests/cmake/quality_join.cmake
  - tests/core/test_core.c
  - tests/diag/controller-analog.a78
  - tests/diag/controller-analog.cfg
  - tests/diag/controller-analog.s
  - tests/diag/controller-light.a78
  - tests/diag/controller-light.cfg
  - tests/diag/controller-light.s
  - tests/diag/controller-motion.a78
  - tests/diag/controller-motion.cfg
  - tests/diag/controller-motion.s
  - tests/diag/driver.c
  - tests/diag/expected.csv
  - tests/diag/tests.cmake
  - tests/fuzz/run.c
  - tests/fuzz/run_seeds.c
  - tests/fuzz/seeds.c
  - tests/fuzz/tests.cmake
  - tests/header/header.c
  - tests/header/header.cpp
  - tests/input/test_input.c
  - tests/input/tests.cmake
  - tests/inventory/ci.txt
  - tests/inventory/nofp.txt
  - tests/inventory/wasm32.txt
  - tests/libretro/host.c
  - tests/libretro/tests.cmake
  - tests/policy/ci_run.cmake
  - tests/policy/quality.cmake
  - tests/policy/tests.cmake
  - tests/prop/test_prop.c
  - tests/roms/manifest.txt
  - tests/runner/test_movie.c
  - tests/runner/test_runner.c
  - tests/runner/tests.cmake
  - tools/check_ci_run.cmake
findings:
  critical: 0
  warning: 0
  info: 0
  total: 0
status: clean
---

# Phase 8: Code Review Report

**Reviewed:** 2026-10-25T03:15:34Z
**Depth:** standard
**Files Reviewed:** 72
**Status:** clean
**Re-review:** fix round 1, source `25690a1`

## Narrative Findings (AI reviewer)

No open findings remain. The original standard review's 72-path scope is
retained. This re-review reused that review context and independently re-read
all four changed source/test/document files: `src/ports.c`,
`tests/input/test_input.c`, `tests/input/tests.cmake` and
`docs/libretro.md`, plus their relevant contracts and callers.

The original resolver status remains degraded (`no-task-commit-rows`).
The supplied broader phase-range/summary union was retained without narrowing.
The initial review at `f0ff2a8` is preserved in
[08-REVIEW.iter1.md](08-REVIEW.iter1.md); applied changes and their recorded RED
evidence are in [08-REVIEW-FIX.md](08-REVIEW-FIX.md). The
[disposition ledger](08-REVIEW-DISPOSITION.md) retains both finding IDs as fixed
with zero open. The clean counts above describe current findings, not deletion
of that history.

### Verified dispositions

| Original finding | Original classification | Current disposition | Independent re-review |
|---|---|---|---|
| CR-01: complete short gun aim forced off-screen | BLOCKER | Fixed by `ac76249` | The fallback now ends at the complete `gun_y` field. Port-0 size 40 and port-1 size 80 preserve neutral zero flags with complete coordinates. Incomplete coordinates still default off-screen; explicit off-screen is retained. |
| WR-01: reversed ProLine physical button labels | WARNING | Fixed by `463b3a0` | The guide now says B → button 1 (right), A → button 2 (left), matching the unchanged `lr_buttons` mapping and controller pin contract. |

The added `light_prefixes` test executes 24 cases: both ports, explicit
selection / Auto from load override / Auto from header, and incomplete aim /
complete aim with absent flags / explicit zero flags / explicit off-screen.
Its literal size calculation gives 36/40/44 on port 0 and 76/80/84 on port 1.
Assertions cover accepted pending and applied values, then actual illumination
and unpolled D6 capture through three real frame calls with bright native
background, no video buffer and a jammed CPU. Incomplete and off-screen controls
must keep the sensor high. Each fresh `boot` rebuilds the input image, so the
header-resolution case does not leak into later cases.

The original standalone reproduction was recompiled from its unchanged source
against the rebuilt current library. It now observes:

| Packet size | Normalized flags | Latched INPT4 D7 |
|---|---:|---:|
| 40, complete coordinates / absent flags | 0 | 0 |
| 44, same coordinates / explicit zero flags | 0 | 0 |

Thus the original wrong behavior no longer reproduces. Source inspection and
the expanded regression establish the corresponding second-port and Auto cases;
the standalone check provides an independent confirmation using the original
trigger.

### Proportional architecture and bounds assessment

The production change is confined to the existing normalization owner and its
actual coordinate boundary. It adds no state, allocation, dependency or
interface and does not change full-packet handling. Existing mask/range checks,
transactional pending replacement and device resolution remain intact. The
regression extends the existing sensor test and uses literal boundary values
and observable pin/latch outcomes.

Only `input.lightgun` moves from the shared one-second registration to its own
two-second registration. Existing cases, assertions, labels and inventory remain.
The existing `muhtari_add_test` still applies normal lane timeout scaling.
The supplied calibration records complete successful work under DEC.47;
the reviewer confirmed the configured native deadline is 2 seconds and ran the
current sanitizer test successfully. No broader deadline or workload change
was introduced.

The initial ENGINEERING §11 assessment remains applicable: existing
ports/TIA/RIOT/MARIA/bus ownership, fixed device state, bounded motion and movie
storage, independent numeric judges and authenticated evidence consumers remain
in place. DEC.61's separately accepted archive cutoff retirement is unchanged;
required measurements and package integrity remain separate acceptance duties.
No additional correctness, security, provenance or substantive architecture
defect was established in the changed boundary. No GPL, LGPL or unlicensed
emulator source was opened.

## Re-review Evidence and Limits

The commands below were run by this reviewer at the corrected source.
All local evidence has run URL `N/A-local`.

| Command or check | Exit | Result |
|---|---:|---|
| Recompile the unchanged original harness from stdin against `build/ci/libmuhtari.a`, then run `/private/tmp/muhtari-review-prefix` | 0 | Both sizes 40 and 44 normalize flags to zero and capture light; table above. No repository source was written. |
| `ctest --preset ci -R '^(input[.]|abi[.]api|libretro[.]host[.]input|runner[.]movie)' --output-on-failure` | 0 | 46 selected/fixture tests passed, including expanded gun prefixes, old input/API clients, real libretro mapping and replay tests. |
| `build/asan/tests/muhtari-test-input lightgun` | 0 | Expanded prefix and existing optical/raster/latch cases passed under the current ASan/UBSan build. |
| Parse `ctest --preset ci --show-only=json-v1` and inspect `input.lightgun` | 0 | Native TIMEOUT is 2.0 seconds. |
| `git diff --quiet 9e76ca3 HEAD -- include/muhtari.h tests/api tests/inventory tests/roms/manifest.txt tests/diag/expected.csv tests/audio/expected.txt tests/built/expected.txt` | 0 | Public layout, frozen API clients, inventories, admitted fixtures and numeric/digest expectations unchanged by fixes. |
| `git diff --check` | 0 | No whitespace errors. |

The parent's complete local 649-test plus six-release-check result is recorded
in the fix report; it was not rerun in full by this reviewer. No new hosted run
or corrected-source package cohort was fetched or claimed here. Earlier hosted
passes remain historical and do not establish final acceptance of these fixes.
The parent owns corrected-source hosted acceptance after this clean review.

Fix round 1 is complete; the second permitted review-fix round is unused.
This reviewer modified only this canonical report and made no source edit or
commit. No owner UAT is required.

---

_Reviewed: 2026-10-25T03:15:34Z_
_Reviewer: gsd-code-reviewer_
_Depth: standard; fix round 1 re-review_

