---
phase: 06-retail-cartridge-boards
reviewed: 2026-10-23T22:08:59Z
depth: standard
files_reviewed: 67
files_reviewed_list:
  - .github/workflows/be-equality.yml
  - .github/workflows/ci.yml
  - AGENTS.md
  - CMakeLists.txt
  - README.md
  - cmake/packaging.cmake
  - docs/compatibility.md
  - docs/embedding.md
  - docs/emulation-tests.md
  - docs/hardware/cartridge.md
  - docs/libretro.md
  - docs/quality.md
  - docs/runner.md
  - docs/testing.md
  - host/status_text.c
  - include/muhtari.h
  - libretro/libretro.c
  - quality.json
  - runner/main.c
  - src/cart.c
  - src/instance.c
  - tests/abi/tests.cmake
  - tests/api/v3.c
  - tests/api/v4.c
  - tests/audio/expected.txt
  - tests/built/expected.txt
  - tests/built/build.cmake
  - tests/cart/images.c
  - tests/cart/images.h
  - tests/cart/test_cart.c
  - tests/cart/test_cartdb.c
  - tests/cart/tests.cmake
  - tests/cart/write_images.c
  - tests/cmake/hashes.cmake
  - tests/cmake/quality.cmake
  - tests/cmake/quality_evidence.cmake
  - tests/cmake/quality_join.cmake
  - tests/core/test_core.c
  - tests/diag/absolute.cfg
  - tests/diag/absolute.s
  - tests/diag/assemble.cmake
  - tests/diag/activision.cfg
  - tests/diag/activision.s
  - tests/diag/driver.c
  - tests/diag/expected.csv
  - tests/diag/fractalus.cfg
  - tests/diag/fractalus.s
  - tests/diag/tests.cmake
  - tests/fuzz/loader.c
  - tests/fuzz/run.c
  - tests/fuzz/run_cart.h
  - tests/fuzz/run_seeds.c
  - tests/fuzz/seeds.c
  - tests/fuzz/tests.cmake
  - tests/header/header.c
  - tests/header/header.cpp
  - tests/inventory/ci.txt
  - tests/inventory/nofp.txt
  - tests/inventory/wasm32.txt
  - tests/libretro/rcheevos.c
  - tests/libretro/tests.cmake
  - tests/policy/quality.cmake
  - tests/policy/tests.cmake
  - tests/prop/test_prop.c
  - tests/roms/manifest.txt
  - tests/runner/tests.cmake
  - tools/compat_doc.cmake
findings:
  critical: 0
  warning: 0
  info: 0
  total: 0
status: clean
---

# Phase 6: Code Review Report

**Reviewed:** 2026-10-23T22:08:59Z
**Depth:** standard
**Files Reviewed:** 67 (63 initial; four additional build/packaging files in correction addenda)
**Status:** clean

## Narrative Findings (AI reviewer)

No evidence-backed BLOCKER or WARNING was found in this review. No issues found.

The review covered cartridge declaration and geometry validation, hash-authorized
Activision normalization, all three board maps, CPU/MARIA/peek access and audio
priority, transactional loading, volatile lifecycle, additive API compatibility,
runner probe/report behavior, loaded libretro descriptors, numeric mailbox judges,
bounded fuzz/property behavior, hash equality, and the adopted quality migration
and trusted-manifest comparability prerequisite. Called functions and the relevant
consumer boundaries were inspected where needed to assess the changed behavior.

The implementation retains local ownership of board state and the existing
allocation and dispatch mechanisms. No concrete ownership, coupling, state-flow,
test-independence or dependency defect requiring a new helper or framework was
identified under ENGINEERING section 11. Numerical size preferences alone were
not treated as findings.

## Scope and limits

Initial review head: `d15f5027ef50ee13dced7d63051e7dab0b8c48ec`.
Initial range base: `617dfa762ddb649839b925be5aa582180066fa78`.
Initial review timestamp: `2026-10-23T20:59:14Z`.
The supplied evaluation scope reports `status: degraded`, `source: phase-range`,
`reason: no-task-commit-rows`. Its explicit file list was retained, including the
adopted maintenance-line retirement and fixture-comparability prerequisites; the
review was not narrowed to test files.

The three generated `.a78` fixtures were assessed through their owned assembly,
configuration, manifest entries and diagnostic consumers rather than as source
text. The deleted maintenance counter and inventory were checked through their
removal and remaining callers; they are absent from the source-file count.
Planning artifacts, build output and ignored local material are excluded.
No structural pre-pass or external-reviewer evidence was supplied.

This report records a code-review result, not phase acceptance. Plan 09's
current-head hosted six-native archive and authenticated aggregate evidence, and
the existing hosted portability/frontend acceptance, remain parent-owned and
pending. Their expected absence was not classified as a code bug. Selected
compatibility/profile policies were assessed against the accepted phase contract,
without claiming new hardware measurements, retail playability or PAL support.

## Focused correction follow-up

Follow-up head: `eb0893112bf48a633e9d294b9d6416fbcc89a710`.
Follow-up range base: `85e12ca9013638c7aafbe96d55636fc74d32781a`.
Only this correction diff was re-reviewed. Its 16 source/documentation files are
already members of the 63-file initial scope:

- `docs/emulation-tests.md`, `docs/quality.md`
- `host/status_text.c`, `src/cart.c`
- `tests/cart/test_cart.c`, `tests/core/test_core.c`, `tests/diag/driver.c`
- `tests/cmake/quality.cmake`, `tests/cmake/quality_evidence.cmake`
- `tests/fuzz/loader.c`, `tests/fuzz/run.c`, `tests/fuzz/run_cart.h`
- `tests/fuzz/run_seeds.c`, `tests/fuzz/seeds.c`
- `tests/policy/quality.cmake`, `tests/prop/test_prop.c`

The initial review did not detect the subsequently observed hosted compiler,
hygiene, fuzz-builder and event-base comparability faults. This follow-up assesses
their corrections; it does not erase that limitation or begin another review cycle.
No unresolved BLOCKER or WARNING was found in the correction diff. Current
frontmatter counts remain zero.

The diagnostic loader separates the local `module_run` pointer from the unchanged
`retro_run` symbol. The run-seed status comparison uses the public unsigned status
type. Remaining product-code changes are formatting and source-comment citations;
the board mappings and selectors are unchanged.

The linear fuzz builder clears accidental A78 magic after bounded copy/padding,
while explicit retail selection still precedes that path. Its minimized regression
extends the run corpus to 29 seeds. The loader corpus, three retail builders,
geometry refusals, frame deadline assertions, 131200-byte execution cap and
existing fuzz durations remain intact. Replay/property checks do not establish a
successful time-based libFuzzer run.

`quality_sample_identity` accepts raw equality or exactly one complete annotation
transition from revision 2 to 3. It checks unique annotations for the transition,
normalizes that one line and compares every remaining byte with the trusted file.
The producer obtains that file with `git show` from the independently resolved
event-base commit in both current and recursive base invocations. The comparison
identity is the raw trusted file's digest; candidate-supplied path/digest variables
do not replace it. The existing real-producer/unchanged-join controls cover the
accepted transition and changed rows/pins/trailers, duplicate/missing annotations,
reverse/other revisions and forged trust inputs. Policy thresholds, sample/audio
pins, manifest bytes, compiler/platform identity and exact join equality are not
weakened by this correction.

The parent records local full CI/hygiene and actual legacy-base reconstruction in
the plan 09 summary. This follow-up independently ran the targeted consumers below.
Local time-based fuzzing was unavailable because its runtime startup hung; no local
libFuzzer pass is claimed. Current-head hosted native, sanitizer/fuzz, portability,
frontend, package and authenticated aggregate acceptance remains pending with the
parent.

## Final measured-CI correction addendum

Final reviewed head: `f74bed5a27e5bd68d042f3bc9ca908308d3213f2`.
Addendum range base: `eb0893112bf48a633e9d294b9d6416fbcc89a710`.
This addendum reviews only the following seven changed source/documentation files:

- `docs/emulation-tests.md`
- `tests/built/build.cmake`, `tests/diag/assemble.cmake`
- `tests/cmake/hashes.cmake`, `tests/diag/tests.cmake`
- `tests/libretro/tests.cmake`, `tests/policy/quality.cmake`

The two build recipes extend the consolidated source list to 65 files; earlier
review scopes and evidence above remain historical. No unresolved BLOCKER or
WARNING was found in this additional diff. Finding counts remain zero, without
resetting the existing review or gap limits.

The quality controls reuse only the invariant current mock-workflow setup.
Every input fault case remains in the loop. Valid cases still invoke the actual
producer, rebuild their independent base, inspect produced values and reach the
unchanged join. Compiler/platform fault cases restore the saved configuration
after collecting their altered observation. Invalid cases assert the expected
failure and do not consume stale reports.

Temporary valid mailbox reports now explicitly use UNIX newlines on every host.
The fixed-length numeric parser, CRC/record verdicts and malformed-report controls
are unchanged. This addresses Windows text-file serialization without broadening
accepted mailbox syntax.

The sample and diagnostic recipes cache compiled pinned Wasmtime modules in
`build/pins/wasmtime-cache`. Each recipe/output owns its configuration outside its
fresh fixture directory. Source copying, compilation/assembly, signing and header
execution still run, and final fixture digests are still checked. No fixture
output, source bytes, pin, expected hash or comparison authority is reused or
changed by the cache. Both sample-digest and diagnostic-digest fault consumers
were exercised in the local checks below.

The libretro command helper receives `90 * MUHTARI_TIMEOUT_SCALE` from its normal
configured caller. The existing registration already scales its outer 120-second
limit through `muhtari_add_test`; the inner helper now follows that same established
lane multiplier. Native generated output retains `HOST_TIMEOUT 90`, with outer
120 unchanged. This changes neither emulated-time bounds nor test workload and
introduces no new timeout scale, lane, byte grant or policy exception.

Parent-reported full local CI/hygiene/package evidence remains in plan 09's
summary. Hosted runs 40029174334, 40029174270 and 40029174245 at this head were
pending when this addendum was requested; this review does not claim their result.
The earlier local libFuzzer limitation remains explicit.

## Final package and scoped-control note

Latest reviewed head: `25919fe822cd6cc23a6984c13ac826796e7d9cfe`.
This note reviews only `f74bed5..25919fe`: `CMakeLists.txt`,
`cmake/packaging.cmake` and `tests/policy/quality.cmake`. The two new build/packaging
files bring the consolidated source list to 67. No unresolved source-level
BLOCKER or WARNING was identified in this three-file diff; counts remain zero.

The MSVC option is restricted to top-level Release builds, before target creation,
so shipped and tested objects receive the same setting and embedding projects keep
their policy. It selects a size preference, consistent with Microsoft's
[/Os documentation](https://learn.microsoft.com/en-us/cpp/build/reference/os-ot-favor-small-code-favor-fast-code).
The possible speed tradeoff remains disclosed; no Windows performance improvement
or measured fit is claimed. Runtime selection, ABI and component membership are
unchanged. CPack ZIP compression level 9 is within the installed CMake archive
generator's documented 0–9 range, and generated `CPackConfig.cmake` retains that
exact value. Archive contents and release judges remain independent.

`quality_expect_failure` executes the unchanged `expect_fail.cmake` inside function
scope. Its local command, exit-code requirement, regex, result and merged output
are isolated from other cases. All call sites still execute the actual child
command and require exit 1 plus the original regex; a failed assertion terminates
the control run. Existing case lists, actual producer recursion, base identities,
joins, thresholds and the 30-second registration remain intact. One-off probes of
the exact current helper confirmed acceptance of the intended failure and rejection
of both a wrong exit and a wrong message.

The prior hosted Windows x64 result at `f74bed5` was 402586 bytes against 399937;
level-9-only recompression was still 400949. These remain failed historical
observations, not grants or acceptance. The parent reports local full CI 612+6,
hygiene 24 and three ZIPs totaling 213899, but those macOS results do not prove
Windows fit. Hosted runs 40031550434, 40031550533 and 40031550534 were pending when
this note was requested. Current Windows size and the final six-lane maximum
therefore remain explicitly unproved, with the unchanged ceiling authoritative.
No review/gap allowance or GSD step was reset.

## Review verification

Initial review command:

```sh
ctest --test-dir build/ci -R '^(cart[.]|diag[.]retail|libretro[.]rcheevos|fuzz[.]regress[.]|prop[.]|runner[.]hashes_file|policy[.]quality[.]controls)' --output-on-failure -j 4
```

Exit status: **0**. Result: **37/37 passed**, including required built-fixture
dependencies. Run URL: **N/A — local**. These checks supplement the source review;
passing tests were not treated as proof that no defects can exist.

Follow-up commands at `eb08931`:

```sh
ctest --test-dir build/ci -R '^(policy[.]quality[.]controls|fuzz[.]regress[.]|prop[.]|diag[.]retail)$' --output-on-failure -j 4
ctest --test-dir build/ci -R '^(fuzz[.]regress[.]|prop[.])' --output-on-failure -j 4
```

Exit statuses: **0**, **0**. The first expression selected the complete quality
producer/join controls and actual-module retail diagnostic, **2/2 passed**. The
second selected the four fuzz replay/control cases and all four properties with
their two built-fixture dependencies, **10/10 passed**. Run URLs: **N/A — local**.
No hosted or time-based fuzz result is inferred from these local executions.

Final-addendum commands at `f74bed5`:

```sh
ctest --test-dir build/ci -R '^(policy[.]quality[.]controls|diag[.]retail|diag[.]rebuild([.]self_test)?|runner[.]hashes_file|libretro[.]rcheevos|built[.]simple([.]self_test)?)$' --output-on-failure -j 4
ctest --test-dir build/ci -R '^built[.]samples[.]self_test$' --output-on-failure
```

Exit statuses: **0**, **0**. Results: **10/10 passed**, including built-fixture
dependencies and the diagnostic digest fault; **1/1 passed**, the actual sample
digest fault. Run URLs: **N/A — local**. The generated native helper was also
inspected with timeout scale 1 and `HOST_TIMEOUT 90`. Hosted Windows/BE and
time-based fuzz acceptance are not inferred from these local executions.

Latest-note commands at `25919fe`:

```sh
ctest --test-dir build/ci -R '^policy[.]quality[.]controls$' --output-on-failure
ctest --test-dir build/ci -L release --output-on-failure -j 4
```

Exit statuses: **0**, **0**. Results: **1/1** quality-control test passed and
**6/6** release checks passed. The one-off Python probe verified that the three
existing `build/ci/quality-inline-check/*.cmake` scripts contain the exact current
helper, then ran each with `cmake -P`: intended failure exited **0** through the
judge; wrong-exit and wrong-regex probes each exited **1**, as expected; the Python
assertion command exited **0**. Run URLs: **N/A — local**. The installed
`cmake --help-manual cpack-generators.7` and generated CPack configuration were
inspected for compression-level support; no hosted fit result is inferred.

No product source was modified and no commit was made by the reviewer.

---

_Reviewer: gsd-code-reviewer_
_Depth: standard_
