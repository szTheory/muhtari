---
phase: 08-controllers-at-the-pins
reviewed: 2026-10-25T03:05:38Z
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
  critical: 1
  warning: 1
  info: 0
  total: 2
status: issues_found
---

# Phase 8: Code Review Report

**Reviewed:** 2026-10-25T03:05:38Z
**Depth:** standard
**Files Reviewed:** 72
**Status:** issues_found

## Narrative Findings (AI reviewer)

Reviewed the supplied 72-path scope at `f0ff2a8` against phase context, research,
plans, summaries and the existing public contracts. The scope resolver was
degraded (`no-task-commit-rows`); the supplied broader phase-range/summary union
was retained without narrowing. Review used the changes since
`0da9a8b7e330b7616cac2a10631e490c8b8925bf`, current source and relevant callers.

One accepted public input prefix produces incorrect sensor behavior. The
libretro guide also retains reversed physical button labels. The passing
existing checks below do not cover the failing gun-prefix boundary.

## Critical Issues

### CR-01: Complete short gun aim is incorrectly forced off-screen

**Classification:** BLOCKER  
**File:** `src/ports.c:634-636`  
**Related contract:** `docs/embedding.md:451-454`; extended-input normalization
in `08-02-PLAN.md`, Task 1.

**Issue:** The short-packet fallback tests whether the entire 40-byte port block
is present, including `flags`. A size-40 packet already includes both port-0
coordinates (`gun_x` ends at byte 36 and `gun_y` at byte 40), but omits
`flags`. Its absent flag should retain the documented neutral zero default:
only an incomplete aim pair defaults off-screen. Instead the setter accepts the
packet and overwrites `flags` with `MUHTARI_INPUT_GUN_OFFSCREEN`. Port 1 has
the same defect at size 80. Consequently valid complete aim cannot sense light
until the caller also supplies the otherwise optional flag field.

**Executable evidence:** A standalone harness linked to the current
`build/ci/libmuhtari.a` loaded the owned light diagnostic, submitted explicit
aim (100,20), selected bright native background 8, enabled D6, jammed the CPU,
and executed three real `muhtari_run_frame` calls. Command
`/private/tmp/muhtari-review-prefix` exited 0 and observed:

| Packet size | Normalized flags | Latched INPT4 D7 |
|---|---:|---:|
| 40, complete coordinates / absent flags | 1 | 128 |
| 44, same coordinates / explicit zero flags | 0 | 0 |

The smaller packet is suppressed while the equivalent zero-extended packet
captures illumination. This is an input-normalization defect, independent of
any hardware calibration assumption.

**Fix:** Set the off-screen default only while the complete coordinate pair is
absent; preserve zero normalization when only `flags` is missing. For example:

```c
uint32_t aim_end = (uint32_t)(offsetof(muhtari_input_ex, port) +
                              p * sizeof(muhtari_port_input) +
                              offsetof(muhtari_port_input, gun_y) +
                              sizeof port->gun_y);
if (gun && in->size < aim_end) {
    port->flags = MUHTARI_INPUT_GUN_OFFSCREEN;
}
```

Add regression cases for port-0 sizes 36/40/44 and port-1 sizes 76/80/84:
incomplete coordinates remain off-screen, complete coordinates plus absent flags
equal explicit zero flags, and explicit off-screen remains suppressed. Exercise
the actual sensor/latch path as well as normalized fields. The existing
`extended_digital` prefix loop uses zero Auto packets; it does not exercise a
resolved gun at this boundary.

## Warnings

### WR-01: Libretro guide reverses the ProLine physical button labels

**Classification:** WARNING  
**File:** `docs/libretro.md:15-16`  
**Related source:** `libretro/libretro.c:762-778`;
`docs/hardware/controllers.md:217-224`;
`tests/input/test_input.c:1120-1144`.

**Issue:** The public binding table labels RetroPad B as “button 1 (left)” and
A as “button 2 (right)”. The adapter maps B to `MUHTARI_BUTTON_1` and A to
`MUHTARI_BUTTON_2`; the controller contract and pin tests identify button 1
as the ProLine right button and button 2 as its left button. Readers translating
physical ProLine controls therefore receive the opposite labels. This text
predates the phase but remains in a scoped host document whose accuracy is an
explicit phase deliverable.

**Fix:** Correct the parenthetical labels to B → “button 1 (right)” and
A → “button 2 (left)”, preserving the existing tested RetroPad mapping.

## Review Evidence and Limits

All commands below were run during this review; local run URL is
`N/A-local`.

| Command or check | Exit | Result |
|---|---:|---|
| `ctest --preset ci -R '^(input\.|api\.v6|abi\.api|runner\.movie|diag\.controllers|policy\.ci_run|policy\.quality\.controls)' --output-on-failure` | 0 | 48 selected/fixture tests passed, including controller report mutants and real producer/join controls. |
| `ctest --preset ci -R '^(libretro[.]host[.]input|runner[.]hashes_file|fuzz[.]regress[.]run|prop[.]instances_isolated)$' --output-on-failure` | 0 | Nine selected/fixture tests passed, including complete 342-row equality production and the retained-row digest guard. |
| `/private/tmp/muhtari-review-prefix` | 0 | Reproduced CR-01 through real frame execution; result table above. The harness was compiled from stdin and did not change repository sources. |
| In-memory Python manifest/sample comparison against `git show 0da9a8b:<path>`, with SHA-256 checks of the three controller fixtures | 0 | All old manifest bytes retained; three new 16,512-byte fixtures match their admitted digests. Built/audio expected bytes differ only in their explicit revision-4 → revision-5 annotations. |
| `git diff --check` | 0 | No whitespace errors in the reviewed tree. |
| `git check-ignore --stdin < build/phase08-review-files.txt` | 1 | No scoped path is ignored. |
| `git diff --numstat 0da9a8b HEAD -- tests/retroarch`; `wc -l tests/retroarch/*` | 0 | Hosted frontend sources unchanged; 300 physical lines. |

The parent supplied final CI run
[40153312357](https://github.com/szTheory/muhtari/actions/runs/40153312357) and
Nightly run
[40153313963](https://github.com/szTheory/muhtari/actions/runs/40153313963).
Those runs and the package cohort were not independently fetched again by this
reviewer; their supplied acceptance evidence is not presented as a fresh
reviewer execution.

### Proportional architecture and bounds assessment

The current implementation retains existing ownership: ports own device
protocol/charge/motion state, TIA owns D6/D7, RIOT owns effective drive and PA7,
MARIA supplies native emission, and the bus merges chronological deadlines.
State remains instance-owned and fixed-size; accepted motion is clamped before
addition and consumes finite residuals. Movie 2 reuses the bounded file reader
with immutable bytes and one cursor. No new package or general event framework
is introduced. The seven-node keypad closure, finite adapter scans, optical
fit and motion cadence are explicit compatibility policies, not calibration.

The evidence extensions reuse existing numeric judges, report consumers and
authenticated producer/join controls with meaningful pass/fault cases.
ENGINEERING §11 does not justify a new quota, abstraction or review framework
here. The separately accepted DEC.61 archive-policy change was respected:
actual authenticated current/base measurements and package integrity remain
required, while the retired compressed-size cutoff was not reinstated.
The substantive boundary defect is CR-01; no additional architecture, security
or provenance defect was established in this scope. No GPL, LGPL or unlicensed
emulator source was opened.

Source files were not modified and no commit was created. The orchestrator owns
fixes, any required rerun of final acceptance, and the bounded review-fix rounds.

---

_Reviewed: 2026-10-25T03:05:38Z_  
_Reviewer: gsd-code-reviewer_  
_Depth: standard_

